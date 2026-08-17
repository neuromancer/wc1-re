#!/usr/bin/env python3
"""Emit an apply_patch patch for reviewed WC1-to-WC2 map corrections.

The mapping TSV contains long diagnostic columns, so editing it by hand is
both slow and prone to leaving duplicate sources or destinations.  This tool
keeps the file one-to-one, evicts any row occupying a reviewed destination,
sorts by WC2 address, and emits a patch for review instead of writing files.

    python3 bin/updateWC2Map.py \
        --set 0041E7C0,004118A9,steady_object \
        --reason "Preserved order and binary-comp candidate comparison."
"""

from __future__ import annotations

import argparse
import csv
import difflib
import io
from dataclasses import dataclass
from pathlib import Path

from remapWC1ToWC2 import DEFAULT_MAP, ROOT, format_address, parse_address


@dataclass(frozen=True)
class Replacement:
    source: int
    destination: int
    name: str


def parse_replacement(text: str) -> Replacement:
    fields = text.split(",", 2)
    if len(fields) != 3 or not fields[2]:
        raise argparse.ArgumentTypeError("expected WC1,WC2,function_name")
    try:
        source = parse_address(fields[0])
        destination = parse_address(fields[1])
    except ValueError as error:
        raise argparse.ArgumentTypeError(str(error)) from error
    return Replacement(source, destination, fields[2])


def load_document(path: Path) -> tuple[list[str], list[str], list[dict[str, str]]]:
    lines = path.read_text(encoding="utf-8").splitlines()
    comments = [line for line in lines if line.startswith("# ")]
    data_lines = [line for line in lines if not line.startswith("# ")]
    reader = csv.DictReader(data_lines, delimiter="\t")
    if reader.fieldnames is None:
        raise SystemExit("error: mapping TSV has no header")
    return comments, reader.fieldnames, list(reader)


def render_document(
    comments: list[str], fieldnames: list[str], rows: list[dict[str, str]]
) -> str:
    count = len(rows)
    comments = [
        f"# rows={count}" if line.startswith("# rows=") else line
        for line in comments
    ]
    stream = io.StringIO()
    for line in comments:
        stream.write(line + "\n")
    writer = csv.DictWriter(
        stream, fieldnames=fieldnames, delimiter="\t", lineterminator="\n"
    )
    writer.writeheader()
    writer.writerows(rows)
    return stream.getvalue()


def emit_patch(path: Path, old_text: str, new_text: str) -> None:
    relative = path.relative_to(ROOT)
    print("*** Begin Patch")
    print(f"*** Update File: {relative}")
    diff = difflib.unified_diff(
        old_text.splitlines(keepends=True),
        new_text.splitlines(keepends=True),
        fromfile=str(relative),
        tofile=str(relative),
        n=2,
    )
    for line in list(diff)[2:]:
        if line.startswith("@@"):
            print("@@")
        else:
            print(line, end="")
    print("*** End Patch")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--map", type=Path, default=DEFAULT_MAP)
    parser.add_argument(
        "--set",
        action="append",
        default=[],
        type=parse_replacement,
        metavar="WC1,WC2,NAME",
    )
    parser.add_argument(
        "--remove", action="append", default=[], type=parse_address, metavar="WC1"
    )
    parser.add_argument("--evidence", default="WC1-manual-verified")
    parser.add_argument("--reason", required=True)
    args = parser.parse_args()

    path = args.map.resolve()
    old_text = path.read_text(encoding="utf-8")
    comments, fieldnames, rows = load_document(path)
    by_source = {parse_address(row["source_address"]): row for row in rows}
    by_destination = {
        parse_address(row["destination_address"]): row for row in rows
    }

    for source in args.remove:
        row = by_source.pop(source, None)
        if row is not None:
            by_destination.pop(parse_address(row["destination_address"]), None)

    replacements = args.set
    if len({item.source for item in replacements}) != len(replacements):
        raise SystemExit("error: duplicate WC1 source in --set arguments")
    if len({item.destination for item in replacements}) != len(replacements):
        raise SystemExit("error: duplicate WC2 destination in --set arguments")

    for item in replacements:
        old_source_row = by_source.pop(item.source, None)
        if old_source_row is not None:
            by_destination.pop(
                parse_address(old_source_row["destination_address"]), None
            )
        old_destination_row = by_destination.pop(item.destination, None)
        if old_destination_row is not None:
            by_source.pop(parse_address(old_destination_row["source_address"]), None)

        row = {field: "" for field in fieldnames}
        row.update(
            {
                "source_address": f"{item.source:08x}",
                "destination_address": f"{item.destination:08x}",
                "function_name": item.name,
                "evidence": args.evidence,
                "review_flags": "",
                "association_votes": "0",
                "correlator": "Manual Match",
                "audit_comment": (
                    f"[WC1 transfer] {args.reason} WC1 "
                    f"{format_address(item.source)} -> WC2 "
                    f"{format_address(item.destination)} ({item.name})."
                ),
            }
        )
        by_source[item.source] = row
        by_destination[item.destination] = row

    rows = sorted(by_source.values(), key=lambda row: parse_address(
        row["destination_address"]
    ))
    if len(rows) != len(by_destination):
        raise SystemExit("error: destination collision remains after update")

    new_text = render_document(comments, fieldnames, rows)
    emit_patch(path, old_text, new_text)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
