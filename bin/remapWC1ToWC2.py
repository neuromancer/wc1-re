#!/usr/bin/env python3
"""Replace WC1 function-address metadata with the reviewed WC2 name map.

The script never edits files itself.  --patch emits an apply_patch patch for
the caller to review and apply.  Mapped source markers remain active for
binary-comp; unresolved markers are made non-numeric so a WC2 report cannot
silently compare unrelated bytes. WC1 provenance belongs in the generated
migration manifest, not in the converted WC2 source tree.

    python3 bin/remapWC1ToWC2.py --summary
    python3 bin/remapWC1ToWC2.py --patch --from-wc1
    python3 bin/remapWC1ToWC2.py --check
    python3 bin/remapWC1ToWC2.py --manifest --from-wc1
"""

from __future__ import annotations

import argparse
import csv
import difflib
import io
import re
import sys
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
DEFAULT_MAP = ROOT / "ghidra_scripts" / "wc1_wc2_name_map.tsv"
DEFAULT_MANIFEST = ROOT / "reports" / "wc2-address-remap.tsv"
SOURCE_ROOT = ROOT / "src"
METADATA_HEADERS = (
    ROOT / "include" / "wc1.h",
    ROOT / "include" / "wc1extern.h",
    ROOT / "include" / "wc1funcs.h",
    ROOT / "src" / "ix" / "ix.h",
)

HEADER_RE = re.compile(
    r"^(?P<indent>\s*)/\*\s*Function start:\s*"
    r"(?P<active>0x[0-9A-Fa-f]+|WC2_UNMAPPED)"
    r"(?P<inside>[^*]*?)\s*\*/"
    r"(?:\s*/\*\s*WC1 start:\s*0x(?P<original>[0-9A-Fa-f]+)\s*\*/)?"
    r"(?P<tail>.*)$"
)
ADDRESS_COMMENT_RE = re.compile(
    r"/\*\s*(?:"
    r"(?P<raw>0x[0-9A-Fa-f]+)"
    r"|(?P<unmapped>WC2\s+unmapped)"
    r"|WC2\s+(?P<wc2>0x[0-9A-Fa-f]+|unmapped);\s*"
    r"WC1\s+0x(?P<original>[0-9A-Fa-f]+)"
    r")\s*\*/"
)
FUNCTION_NAME_RE = re.compile(
    r"(operator\s+(?:new|delete)|"
    r"[~A-Za-z_][A-Za-z0-9_]*(?:::[~A-Za-z_][A-Za-z0-9_]*)*)\s*\("
)
FUNCTION_NAME_SKIP = {
    "__declspec",
    "for",
    "if",
    "sizeof",
    "switch",
    "while",
}


@dataclass(frozen=True)
class Mapping:
    source: int
    destination: int
    name: str
    evidence: str
    review_flags: str


@dataclass(frozen=True)
class Marker:
    path: Path
    line: int
    source: int | None
    destination: int | None
    source_name: str
    mapping_name: str
    evidence: str
    review_flags: str


@dataclass(frozen=True)
class Provenance:
    by_destination: dict[int, int]
    by_symbol: dict[tuple[str, str], int]
    by_name: dict[str, int]
    wc2_only_by_symbol: dict[tuple[str, str], int]


def parse_address(text: str) -> int:
    return int(text, 16)


def format_address(address: int) -> str:
    return f"0x{address:X}"


def load_map(path: Path) -> dict[int, Mapping]:
    if not path.is_file():
        raise SystemExit(f"error: mapping file not found: {path}")

    with path.open(newline="", encoding="utf-8") as stream:
        lines = [line for line in stream if not line.startswith("# ")]
    reader = csv.DictReader(lines, delimiter="\t")
    required = {
        "source_address",
        "destination_address",
        "function_name",
        "evidence",
        "review_flags",
    }
    if reader.fieldnames is None or not required.issubset(reader.fieldnames):
        raise SystemExit("error: unexpected WC1/WC2 map schema")

    result: dict[int, Mapping] = {}
    destinations: set[int] = set()
    for row in reader:
        source = parse_address(row["source_address"])
        destination = parse_address(row["destination_address"])
        if source in result:
            raise SystemExit(f"error: duplicate WC1 source {format_address(source)}")
        if destination in destinations:
            raise SystemExit(
                f"error: duplicate WC2 destination {format_address(destination)}"
            )
        destinations.add(destination)
        result[source] = Mapping(
            source=source,
            destination=destination,
            name=row["function_name"],
            evidence=row["evidence"],
            review_flags=row["review_flags"],
        )
    return result


def load_provenance(path: Path) -> Provenance:
    """Load WC1 origins retained outside the converted source tree.

    A destination lookup handles already-mapped functions.  The path/name
    lookup is needed for WC2_UNMAPPED functions that acquire a mapping during
    a later review pass.
    """
    if not path.is_file():
        raise SystemExit(f"error: provenance manifest not found: {path}")

    with path.open(newline="", encoding="utf-8") as stream:
        reader = csv.DictReader(stream, delimiter="\t")
        required = {"source_file", "wc1_address", "wc2_address", "source_name"}
        if reader.fieldnames is None or not required.issubset(reader.fieldnames):
            raise SystemExit("error: unexpected WC2 remap manifest schema")

        by_destination: dict[int, int] = {}
        wc2_only_by_symbol: dict[tuple[str, str], int] = {}
        symbols: dict[tuple[str, str], set[int]] = {}
        names: dict[str, set[int]] = {}
        for row in reader:
            key = (row["source_file"], row["source_name"])
            if not row["wc1_address"]:
                if row["wc2_address"] and row.get("status") == "wc2-only":
                    wc2_only_by_symbol[key] = parse_address(row["wc2_address"])
                continue
            source = parse_address(row["wc1_address"])
            if row["wc2_address"]:
                destination = parse_address(row["wc2_address"])
                previous = by_destination.setdefault(destination, source)
                if previous != source:
                    raise SystemExit(
                        "error: conflicting provenance for WC2 destination "
                        f"{format_address(destination)}"
                    )
            symbols.setdefault(key, set()).add(source)
            if row["source_name"]:
                names.setdefault(row["source_name"], set()).add(source)

    by_symbol = {
        key: next(iter(sources))
        for key, sources in symbols.items()
        if len(sources) == 1 and key[1]
    }
    by_name = {
        name: next(iter(sources))
        for name, sources in names.items()
        if len(sources) == 1
    }
    return Provenance(
        by_destination=by_destination,
        by_symbol=by_symbol,
        by_name=by_name,
        wc2_only_by_symbol=wc2_only_by_symbol,
    )


def source_files() -> list[Path]:
    return sorted(
        path
        for path in SOURCE_ROOT.rglob("*")
        if path.suffix in (".c", ".cpp") and path.is_file()
    )


def original_address(
    match: re.Match[str],
    destinations: dict[int, Mapping],
    from_wc1: bool,
    provenance_source: int | None = None,
) -> int | None:
    original = match.group("original")
    if original is not None:
        return parse_address(original)
    active = match.group("active")
    if not from_wc1 and provenance_source is not None:
        return provenance_source
    if active == "WC2_UNMAPPED":
        return None
    address = parse_address(active)
    if from_wc1:
        return address
    mapping = destinations.get(address)
    if mapping is not None:
        return mapping.source
    return address


def find_source_name(lines: list[str], start: int) -> str:
    text = "".join(lines[start:start + 12])
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.DOTALL)
    text = re.sub(r"//[^\n]*", " ", text)
    for match in FUNCTION_NAME_RE.finditer(text):
        name = re.sub(r"\s+", " ", match.group(1))
        if name not in FUNCTION_NAME_SKIP:
            return name
    return ""


def rewrite_source(
    path: Path,
    mappings: dict[int, Mapping],
    from_wc1: bool,
    provenance: Provenance | None,
) -> tuple[str, list[Marker]]:
    old_text = path.read_text(encoding="utf-8")
    lines = old_text.splitlines(keepends=True)
    markers: list[Marker] = []
    rewritten: list[str] = []
    destinations = {mapping.destination: mapping for mapping in mappings.values()}

    for line_number, line in enumerate(lines, 1):
        ending = "\n" if line.endswith("\n") else ""
        body = line[:-1] if ending else line
        match = HEADER_RE.match(body)
        if match is None:
            rewritten.append(line)
            continue

        source_name = find_source_name(lines, line_number)
        symbol_key = (str(path.relative_to(ROOT)), source_name)
        wc2_only_destination = None
        if provenance is not None:
            wc2_only_destination = provenance.wc2_only_by_symbol.get(symbol_key)
        if wc2_only_destination is not None:
            active = format_address(wc2_only_destination)
            replacement = (
                f"{match.group('indent')}/* Function start: {active}"
                f"{match.group('inside')} */"
                f"{match.group('tail')}{ending}"
            )
            rewritten.append(replacement)
            markers.append(
                Marker(
                    path=path,
                    line=line_number,
                    source=None,
                    destination=wc2_only_destination,
                    source_name=source_name,
                    mapping_name=source_name,
                    evidence="WC2-only",
                    review_flags="",
                )
            )
            continue

        provenance_source = None
        if provenance is not None:
            provenance_source = provenance.by_symbol.get(
                symbol_key
            )
            active = match.group("active")
            if provenance_source is None and active != "WC2_UNMAPPED":
                provenance_source = provenance.by_destination.get(
                    parse_address(active)
                )
        source = original_address(
            match, destinations, from_wc1, provenance_source
        )
        mapping = None if source is None else mappings.get(source)
        if mapping is None:
            active = "WC2_UNMAPPED"
            destination = None
            mapping_name = ""
            evidence = ""
            review_flags = ""
        else:
            active = format_address(mapping.destination)
            destination = mapping.destination
            mapping_name = mapping.name
            evidence = mapping.evidence
            review_flags = mapping.review_flags

        replacement = (
            f"{match.group('indent')}/* Function start: {active}"
            f"{match.group('inside')} */"
            f"{match.group('tail')}{ending}"
        )
        rewritten.append(replacement)
        markers.append(
            Marker(
                path=path,
                line=line_number,
                source=source,
                destination=destination,
                source_name=source_name,
                mapping_name=mapping_name,
                evidence=evidence,
                review_flags=review_flags,
            )
        )

    return "".join(rewritten), markers


def find_header_function_name(text: str, comment_start: int) -> str:
    statement_end = text.rfind(";", 0, comment_start)
    if statement_end < 0:
        statement_end = comment_start
    previous_semicolon = text.rfind(";", 0, statement_end)
    previous_open_brace = text.rfind("{", 0, statement_end)
    previous_close_brace = text.rfind("}", 0, statement_end)
    statement_start = max(
        previous_semicolon, previous_open_brace, previous_close_brace
    )
    statement = text[statement_start + 1 : statement_end]
    statement = re.sub(r"/\*.*?\*/", " ", statement, flags=re.DOTALL)
    statement = re.sub(r"//[^\n]*", " ", statement)
    for function_match in FUNCTION_NAME_RE.finditer(statement):
        name = re.sub(r"\s+", " ", function_match.group(1))
        if name not in FUNCTION_NAME_SKIP:
            return name
    return ""


def rewrite_metadata_header(
    path: Path,
    mappings: dict[int, Mapping],
    from_wc1: bool,
    provenance: Provenance | None,
) -> str:
    old_text = path.read_text(encoding="utf-8")
    destinations = {mapping.destination: mapping for mapping in mappings.values()}

    def replacement(match: re.Match[str]) -> str:
        if match.group("original") is not None:
            source = parse_address(match.group("original"))
        elif from_wc1 and match.group("raw") is not None:
            address = parse_address(match.group("raw"))
            source = address
        elif provenance is not None:
            name = find_header_function_name(old_text, match.start())
            source = provenance.by_name.get(name)
            if source is None:
                raw = match.group("raw")
                if raw is not None:
                    address = parse_address(raw)
                    if address in destinations:
                        return f"/* {format_address(address)} */"
                return match.group(0)
        else:
            return match.group(0)
        mapping = mappings.get(source)
        if mapping is None:
            return "/* WC2 unmapped */"
        return f"/* {format_address(mapping.destination)} */"

    return ADDRESS_COMMENT_RE.sub(replacement, old_text)


def compute_changes(
    mappings: dict[int, Mapping],
    from_wc1: bool = False,
    provenance: Provenance | None = None,
) -> tuple[dict[Path, str], list[Marker]]:
    changes: dict[Path, str] = {}
    markers: list[Marker] = []

    for path in source_files():
        new_text, file_markers = rewrite_source(
            path, mappings, from_wc1, provenance
        )
        markers.extend(file_markers)
        if new_text != path.read_text(encoding="utf-8"):
            changes[path] = new_text

    for path in METADATA_HEADERS:
        new_text = rewrite_metadata_header(
            path, mappings, from_wc1, provenance
        )
        if new_text != path.read_text(encoding="utf-8"):
            changes[path] = new_text

    return changes, markers


def emit_patch(changes: dict[Path, str]) -> None:
    print("*** Begin Patch")
    for path in sorted(changes):
        old_text = path.read_text(encoding="utf-8")
        relative = path.relative_to(ROOT)
        print(f"*** Update File: {relative}")
        diff = difflib.unified_diff(
            old_text.splitlines(keepends=True),
            changes[path].splitlines(keepends=True),
            fromfile=str(relative),
            tofile=str(relative),
            n=2,
        )
        for line in list(diff)[2:]:
            if line.startswith("@@"):
                sys.stdout.write("@@\n")
            else:
                sys.stdout.write(line)
        if old_text and not old_text.endswith("\n"):
            print()
    print("*** End Patch")


def print_summary(markers: list[Marker], mappings: dict[int, Mapping]) -> None:
    mapped = sum(marker.destination is not None for marker in markers)
    unresolved = len(markers) - mapped
    mapped_sources = {
        marker.source
        for marker in markers
        if marker.destination is not None and marker.source is not None
    }
    print(f"map_rows={len(mappings)}")
    print(f"source_markers={len(markers)}")
    print(f"mapped_markers={mapped}")
    print(f"unresolved_markers={unresolved}")
    print(f"mapped_not_implemented={len(mappings) - len(mapped_sources)}")
    if markers:
        print(f"mapped_percent={mapped / len(markers) * 100:.2f}")


def render_manifest(markers: list[Marker]) -> str:
    stream = io.StringIO()
    writer = csv.writer(stream, delimiter="\t", lineterminator="\n")
    writer.writerow(
        (
            "source_file",
            "source_line",
            "wc1_address",
            "wc2_address",
            "source_name",
            "mapping_name",
            "evidence",
            "review_flags",
            "status",
        )
    )
    for marker in markers:
        writer.writerow(
            (
                marker.path.relative_to(ROOT),
                marker.line,
                "" if marker.source is None else f"{marker.source:08X}",
                "" if marker.destination is None else f"{marker.destination:08X}",
                marker.source_name,
                marker.mapping_name,
                marker.evidence,
                marker.review_flags,
                (
                    "unmapped"
                    if marker.destination is None
                    else "wc2-only"
                    if marker.source is None
                    else "mapped"
                ),
            )
        )
    return stream.getvalue()


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--map", type=Path, default=DEFAULT_MAP)
    parser.add_argument(
        "--provenance-manifest", type=Path, default=DEFAULT_MANIFEST
    )
    parser.add_argument(
        "--from-wc1",
        action="store_true",
        help="interpret unqualified numeric labels as pre-conversion WC1 addresses",
    )
    action = parser.add_mutually_exclusive_group(required=True)
    action.add_argument("--patch", action="store_true")
    action.add_argument("--check", action="store_true")
    action.add_argument("--summary", action="store_true")
    action.add_argument("--manifest", action="store_true")
    action.add_argument("--manifest-patch", action="store_true")
    args = parser.parse_args()

    mappings = load_map(args.map.resolve())
    provenance = None
    if not args.from_wc1:
        provenance = load_provenance(args.provenance_manifest.resolve())
    changes, markers = compute_changes(
        mappings, from_wc1=args.from_wc1, provenance=provenance
    )

    if args.patch:
        if not changes:
            print("*** Begin Patch\n*** End Patch")
        else:
            emit_patch(changes)
        return 0
    if args.check:
        print_summary(markers, mappings)
        if changes:
            print(f"remap_drift_files={len(changes)}")
            for path in sorted(changes):
                print(path.relative_to(ROOT))
            return 1
        print("remap_drift_files=0")
        return 0
    if args.summary:
        print_summary(markers, mappings)
        print(f"files_requiring_remap={len(changes)}")
        return 0
    if args.manifest or args.manifest_patch:
        if any(
            marker.source is None and marker.destination is None
            for marker in markers
        ):
            raise SystemExit(
                "error: one or more source markers lack WC1 provenance"
            )
        manifest = render_manifest(markers)
        if args.manifest_patch:
            emit_patch({args.provenance_manifest.resolve(): manifest})
        else:
            sys.stdout.write(manifest)
        return 0
    return 2


if __name__ == "__main__":
    sys.exit(main())
