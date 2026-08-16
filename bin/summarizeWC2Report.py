#!/usr/bin/env python3
"""Summarize a binary-comp WC2 report against the address-remap manifest."""

from __future__ import annotations

import argparse
import csv
import re
import statistics
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path

from remapWC1ToWC2 import DEFAULT_MAP, ROOT, compute_changes, load_map


SECTION_RE = re.compile(r"^=== (.+) ===$")
ROW_RE = re.compile(
    r"^\s{2}(?P<name>.*?)\s+(?P<address>0x[0-9A-Fa-f]+)"
    r"\s+(?P<similarity>[0-9.]+)%\s*$"
)
SUMMARY_RE = {
    "compared": re.compile(r"^Total compared:\s+(\d+)"),
    "exact": re.compile(r"^\s+100%:\s+(\d+)"),
    "ge90": re.compile(r"^\s+>=90%:\s+(\d+)"),
    "lt90": re.compile(r"^\s+<90%:\s+(\d+)"),
    "errors": re.compile(r"^\s+Errors/NOT FOUND:\s+(\d+)"),
    "missing": re.compile(r"^\s+Missing disassembly exports:\s+(\d+)"),
    "average": re.compile(r"^\s+Average similarity:\s+([0-9.]+)%"),
}


@dataclass(frozen=True)
class ReportRow:
    source_file: str
    name: str
    address: int
    similarity: float


def parse_report(path: Path) -> tuple[list[ReportRow], dict[str, float]]:
    rows: list[ReportRow] = []
    summary: dict[str, float] = {}
    source_file = ""
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        section = SECTION_RE.match(line)
        if section is not None:
            source_file = section.group(1)
            continue
        row = ROW_RE.match(line)
        if row is not None:
            rows.append(
                ReportRow(
                    source_file=source_file,
                    name=row.group("name").rstrip(),
                    address=int(row.group("address"), 16),
                    similarity=float(row.group("similarity")),
                )
            )
            continue
        for key, pattern in SUMMARY_RE.items():
            match = pattern.match(line)
            if match is not None:
                summary[key] = float(match.group(1))
                break
    return rows, summary


def metric_row(values: list[float]) -> tuple[int, float, float, int, int, int]:
    if not values:
        return 0, 0.0, 0.0, 0, 0, 0
    return (
        len(values),
        statistics.mean(values),
        statistics.median(values),
        sum(value == 100.0 for value in values),
        sum(value >= 90.0 for value in values),
        sum(value < 50.0 for value in values),
    )


def metric_cells(values: list[float]) -> str:
    count, average, median, exact, ge90, lt50 = metric_row(values)
    return (
        f"{count} | {average:.2f}% | {median:.2f}% | "
        f"{exact} | {ge90} | {lt50}"
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("report", type=Path)
    parser.add_argument("--baseline", type=Path)
    parser.add_argument(
        "--optimized-core-control",
        type=Path,
        help="WC2 report produced with WC1's optimized game-core flags",
    )
    parser.add_argument(
        "--msvc420-od-control",
        type=Path,
        help="WC2 report produced with MSVC 4.20 and /Od",
    )
    parser.add_argument("--map", type=Path, default=DEFAULT_MAP)
    parser.add_argument(
        "--rows-tsv",
        type=Path,
        help="write every compared function as a review-friendly TSV",
    )
    args = parser.parse_args()

    mappings = load_map(args.map.resolve())
    _changes, markers = compute_changes(mappings)
    rows, summary = parse_report(args.report)
    baseline = None
    if args.baseline is not None:
        _baseline_rows, baseline = parse_report(args.baseline)
    optimized_control_rows: list[ReportRow] = []
    optimized_control = None
    if args.optimized_core_control is not None:
        optimized_control_rows, optimized_control = parse_report(
            args.optimized_core_control
        )
    msvc420_od_rows: list[ReportRow] = []
    if args.msvc420_od_control is not None:
        msvc420_od_rows, _msvc420_od = parse_report(args.msvc420_od_control)

    by_destination = {
        marker.destination: marker
        for marker in markers
        if marker.destination is not None
    }
    missing_markers = [row for row in rows if row.address not in by_destination]
    if missing_markers:
        addresses = ", ".join(f"0x{row.address:X}" for row in missing_markers[:10])
        raise SystemExit(f"error: report rows absent from remap manifest: {addresses}")

    if args.rows_tsv is not None:
        with args.rows_tsv.open("w", newline="", encoding="utf-8") as stream:
            writer = csv.writer(stream, delimiter="\t", lineterminator="\n")
            writer.writerow(
                (
                    "source_file",
                    "source_name",
                    "mapping_name",
                    "wc1_address",
                    "wc2_address",
                    "evidence",
                    "review_flags",
                    "binary_similarity",
                    "similarity_band",
                )
            )
            for row in sorted(rows, key=lambda item: (item.similarity, item.address)):
                marker = by_destination[row.address]
                if row.similarity == 100.0:
                    band = "exact"
                elif row.similarity >= 90.0:
                    band = "ge90"
                elif row.similarity < 50.0:
                    band = "lt50"
                else:
                    band = "50to90"
                writer.writerow(
                    (
                        marker.path.relative_to(ROOT),
                        marker.source_name,
                        marker.mapping_name,
                        f"{marker.source:08X}",
                        f"{row.address:08X}",
                        marker.evidence,
                        marker.review_flags,
                        f"{row.similarity:.2f}",
                        band,
                    )
                )

    mapped_markers = [marker for marker in markers if marker.destination is not None]
    unresolved_markers = [marker for marker in markers if marker.destination is None]
    row_values = [row.similarity for row in rows]

    print("# WC2 address-remap similarity report")
    print()
    print(
        "This report compares the current WC1 reconstruction against WC2 code at "
        "the reviewed WC1→WC2 function destinations. Unmapped functions are "
        "excluded rather than compared against arbitrary bytes."
    )
    print()
    print(
        "The converted source contains WC2 labels only. Historical WC1 addresses "
        "live in the row-level migration manifest so a future `wc2-re` fork does "
        "not need a dual-game annotation or build path."
    )
    print()
    print("## Coverage and headline result")
    print()
    print("| Metric | Result |")
    print("| --- | ---: |")
    print(f"| WC1 source function markers | {len(markers)} |")
    print(
        f"| Markers with a WC2 destination | {len(mapped_markers)} "
        f"({len(mapped_markers) / len(markers) * 100:.2f}%) |"
    )
    print(f"| Explicitly unresolved markers | {len(unresolved_markers)} |")
    print(f"| Functions compared by binary-comp | {len(rows)} |")
    print(
        f"| Compared / mapped markers | "
        f"{len(rows) / len(mapped_markers) * 100:.2f}% |"
    )
    print(f"| Exact machine-code matches | {sum(v == 100.0 for v in row_values)} |")
    print(f"| Similarity >= 90% | {sum(v >= 90.0 for v in row_values)} |")
    print(f"| Similarity < 50% | {sum(v < 50.0 for v in row_values)} |")
    print(f"| Average WC2 similarity | {statistics.mean(row_values):.2f}% |")
    print(f"| Median WC2 similarity | {statistics.median(row_values):.2f}% |")
    print(
        f"| Report errors / missing exports | "
        f"{int(summary.get('errors', 0) + summary.get('missing', 0))} |"
    )
    if baseline is not None:
        print(f"| WC1 baseline functions compared | {int(baseline.get('compared', 0))} |")
        print(f"| WC1 baseline average similarity | {baseline.get('average', 0):.2f}% |")
    print()
    print(
        "Low-confidence address assignments and genuinely changed code both "
        "depress the aggregate, so the evidence tiers below are more informative "
        "than the headline average on its own."
    )

    if optimized_control is not None:
        control_values = [row.similarity for row in optimized_control_rows]
        print()
        print("## Compiler/code-generation control")
        print()
        print("| Rebuild configuration | Compared | Average | Exact | >=90% | <50% |")
        print("| --- | ---: | ---: | ---: | ---: | ---: |")
        print(
            "| MSVC 4.20, WC1 optimized core (`/Og /Oi /Ot /Oy /Ob1 /Gs`) | "
            f"{len(control_values)} | {statistics.mean(control_values):.2f}% | "
            f"{sum(value == 100.0 for value in control_values)} | "
            f"{sum(value >= 90.0 for value in control_values)} | "
            f"{sum(value < 50.0 for value in control_values)} |"
        )
        if msvc420_od_rows:
            msvc420_od_values = [row.similarity for row in msvc420_od_rows]
            print(
                "| MSVC 4.20, unoptimized core (`/Od /Oi`) | "
                f"{len(msvc420_od_values)} | "
                f"{statistics.mean(msvc420_od_values):.2f}% | "
                f"{sum(value == 100.0 for value in msvc420_od_values)} | "
                f"{sum(value >= 90.0 for value in msvc420_od_values)} | "
                f"{sum(value < 50.0 for value in msvc420_od_values)} |"
            )
        print(
            "| MSVC 4.1, unoptimized core (`/Od /Oi`) | "
            f"{len(row_values)} | {statistics.mean(row_values):.2f}% | "
            f"{sum(value == 100.0 for value in row_values)} | "
            f"{sum(value >= 90.0 for value in row_values)} | "
            f"{sum(value < 50.0 for value in row_values)} |"
        )
        print()
        print(
            f"Disabling game-core optimization gains "
            f"{statistics.mean(row_values) - statistics.mean(control_values):.2f} "
            "similarity points. WC2's core consistently has frame pointers, "
            "unconditional EBX/ESI/EDI saves, spilled temporaries, and shared "
            "epilogues, so `/Od` is the appropriate comparison control. This "
            "establishes a code-generation difference from WC1. Both executables "
            "use the Microsoft debug CRT, but their PE linker versions differ: "
            "WC1 reports 4.20 and WC2 reports 3.10. The selected MSVC 4.1 package "
            "identifies its compiler as 10.10.6038 and its linker as 3.10.6038, "
            "an exact match for WC2's linker stamp."
        )
        if msvc420_od_rows:
            msvc420_od_values = [row.similarity for row in msvc420_od_rows]
            print()
            print(
                "Moving from MSVC 4.20 `/Od` to MSVC 4.1 `/Od` leaves the "
                f"rounded average at {statistics.mean(row_values):.2f}% but gains "
                f"{sum(value == 100.0 for value in row_values) - sum(value == 100.0 for value in msvc420_od_values)} "
                "exact match and "
                f"{sum(value >= 90.0 for value in row_values) - sum(value >= 90.0 for value in msvc420_od_values)} "
                "function at >=90%."
            )

    evidence_values: dict[str, list[float]] = defaultdict(list)
    area_values: dict[str, list[float]] = defaultdict(list)
    file_values: dict[str, list[float]] = defaultdict(list)
    review_values: dict[str, list[float]] = defaultdict(list)
    for row in rows:
        marker = by_destination[row.address]
        evidence_values[marker.evidence].append(row.similarity)
        area = "ix audio" if "/ix/" in marker.path.as_posix() else "game/core"
        area_values[area].append(row.similarity)
        file_values[marker.path.relative_to(ROOT).as_posix()].append(row.similarity)
        if marker.review_flags:
            for flag in marker.review_flags.split("|"):
                review_values[flag].append(row.similarity)

    print()
    print("## Similarity by mapping evidence")
    print()
    print("| Evidence tag | Compared | Average | Median | Exact | >=90% | <50% |")
    print("| --- | ---: | ---: | ---: | ---: | ---: | ---: |")
    for evidence, values in sorted(
        evidence_values.items(), key=lambda item: (-statistics.mean(item[1]), item[0])
    ):
        print(f"| `{evidence}` | {metric_cells(values)} |")

    print()
    print("## Similarity by source area")
    print()
    print("| Area | Compared | Average | Median | Exact | >=90% | <50% |")
    print("| --- | ---: | ---: | ---: | ---: | ---: | ---: |")
    for area, values in sorted(area_values.items()):
        print(f"| {area} | {metric_cells(values)} |")

    if review_values:
        print()
        print("## Explicit review flags")
        print()
        print("| Flag | Compared | Average | Median | Exact | >=90% | <50% |")
        print("| --- | ---: | ---: | ---: | ---: | ---: | ---: |")
        for flag, values in sorted(review_values.items()):
            print(f"| `{flag}` | {metric_cells(values)} |")

    print()
    print("## Per-file summary")
    print()
    print("| Source file | Compared | Average | Median | Exact | >=90% | <50% |")
    print("| --- | ---: | ---: | ---: | ---: | ---: | ---: |")
    for source_file, values in sorted(file_values.items()):
        print(f"| `{source_file}` | {metric_cells(values)} |")

    unresolved_by_file: dict[str, list[str]] = defaultdict(list)
    for marker in unresolved_markers:
        source_file = marker.path.relative_to(ROOT).as_posix()
        unresolved_by_file[source_file].append(marker.source_name)
    print()
    print("## Unresolved mapping inventory")
    print()
    print("| Source file | Count | Functions |")
    print("| --- | ---: | --- |")
    for source_file, names in sorted(unresolved_by_file.items()):
        preview = ", ".join(f"`{name}`" for name in names[:8])
        if len(names) > 8:
            preview += f", … (+{len(names) - 8})"
        print(f"| `{source_file}` | {len(names)} | {preview} |")
    print()
    print(
        "The complete row-level mapping and unresolved list is in "
        "`reports/wc2-address-remap.tsv`."
    )
    print(
        "All binary-comp rows, sorted from lowest to highest similarity, are in "
        "`reports/wc2-similarity.tsv`."
    )

    print()
    print("## Lowest-scoring mapped comparisons")
    print()
    print("| Source | Function | WC1 | WC2 | Evidence | Similarity |")
    print("| --- | --- | ---: | ---: | --- | ---: |")
    for row in sorted(rows, key=lambda item: (item.similarity, item.address))[:40]:
        marker = by_destination[row.address]
        print(
            f"| `{marker.path.relative_to(ROOT)}` | `{row.name}` | "
            f"`0x{marker.source:X}` | `0x{row.address:X}` | "
            f"`{marker.evidence}` | {row.similarity:.2f}% |"
        )

    print()
    print("## Reproduction")
    print()
    print(chr(96) * 3 + "sh")
    print("make wc2-remap-audit")
    print("make export-asm")
    print("make report")
    print(chr(96) * 3)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
