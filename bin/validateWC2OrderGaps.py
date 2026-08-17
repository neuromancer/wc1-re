#!/usr/bin/env python3
"""Validate small WC1/WC2 order gaps with binary-comp and align them.

``auditWC2FunctionOrder.py`` identifies functions between trusted anchors.  An
equal number of functions can be zipped directly, but a single WC2 insertion
or deletion shifts every positional candidate in an unequal gap.  This tool
compares the complete source-by-target matrix for small gaps using
``binary-comp`` and then selects an order-preserving alignment.

No local similarity metric is calculated: every matrix value is the score
reported by binary-comp.  The alignment is a candidate generator, not mapping
authority; reviewed results still flow through ``updateWC2Map.py``.
"""

from __future__ import annotations

import argparse
import csv
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass
from pathlib import Path

from remapWC1ToWC2 import (
    DEFAULT_MANIFEST,
    DEFAULT_MAP,
    ROOT,
    Marker,
    compute_changes,
    load_map,
    load_provenance,
)


SIMILARITY_RE = re.compile(r"Similarity:\s*([0-9.]+)%")
DEFAULT_AUDIT = Path("/tmp/wc2-order-audit.tsv")
DEFAULT_SIMILARITY = ROOT / "reports" / "wc2-similarity.tsv"


@dataclass(frozen=True, order=True)
class GapKey:
    source_file: str
    left_wc1: int
    left_wc2: int
    right_wc1: int
    right_wc2: int
    wc1_count: int
    wc2_count: int


@dataclass(frozen=True)
class OrderedFunction:
    address: int
    name: str
    instruction_count: int


@dataclass
class Gap:
    key: GapKey
    sources: list[OrderedFunction]
    targets: list[OrderedFunction]


@dataclass(frozen=True)
class Comparison:
    source: OrderedFunction
    target: OrderedFunction
    similarity: float | None
    error: str


def parse_address(text: str) -> int:
    return int(text, 16)


def format_address(address: int | None) -> str:
    return "" if address is None else f"{address:08X}"


def load_audit(path: Path) -> list[Gap]:
    if not path.is_file():
        raise SystemExit(f"error: order-audit TSV not found: {path}")
    grouped: dict[GapKey, list[dict[str, str]]] = {}
    with path.open(newline="", encoding="utf-8") as stream:
        reader = csv.DictReader(stream, delimiter="\t")
        required = {
            "source_file",
            "left_wc1",
            "left_wc2",
            "right_wc1",
            "right_wc2",
            "gap_count_wc1",
            "gap_count_wc2",
            "ordinal",
            "wc1_address",
            "wc1_name",
            "wc1_instruction_count",
            "proposed_wc2_address",
            "wc2_current_name",
            "wc2_instruction_count",
        }
        if reader.fieldnames is None or not required.issubset(reader.fieldnames):
            raise SystemExit(f"error: unexpected order-audit TSV schema: {path}")
        for row in reader:
            key = GapKey(
                source_file=row["source_file"],
                left_wc1=parse_address(row["left_wc1"]),
                left_wc2=parse_address(row["left_wc2"]),
                right_wc1=parse_address(row["right_wc1"]),
                right_wc2=parse_address(row["right_wc2"]),
                wc1_count=int(row["gap_count_wc1"]),
                wc2_count=int(row["gap_count_wc2"]),
            )
            grouped.setdefault(key, []).append(row)

    gaps: list[Gap] = []
    for key, rows in sorted(grouped.items()):
        rows.sort(key=lambda row: int(row["ordinal"]))
        sources = [
            OrderedFunction(
                address=parse_address(row["wc1_address"]),
                name=row["wc1_name"],
                instruction_count=int(row["wc1_instruction_count"]),
            )
            for row in rows
            if row["wc1_address"]
        ]
        targets = [
            OrderedFunction(
                address=parse_address(row["proposed_wc2_address"]),
                name=row["wc2_current_name"],
                instruction_count=int(row["wc2_instruction_count"]),
            )
            for row in rows
            if row["proposed_wc2_address"]
        ]
        if len(sources) != key.wc1_count or len(targets) != key.wc2_count:
            raise SystemExit(
                "error: incomplete gap rows for "
                f"{key.source_file} {format_address(key.left_wc1)}"
            )
        gaps.append(Gap(key=key, sources=sources, targets=targets))
    return gaps


def load_similarities(path: Path | None) -> dict[int, float]:
    if path is None:
        return {}
    if not path.is_file():
        raise SystemExit(f"error: similarity TSV not found: {path}")
    result: dict[int, float] = {}
    with path.open(newline="", encoding="utf-8") as stream:
        reader = csv.DictReader(stream, delimiter="\t")
        required = {"wc1_address", "binary_similarity"}
        if reader.fieldnames is None or not required.issubset(reader.fieldnames):
            raise SystemExit(f"error: unexpected similarity TSV schema: {path}")
        for row in reader:
            if row["wc1_address"]:
                result[parse_address(row["wc1_address"])] = float(
                    row["binary_similarity"]
                )
    return result


def run_comparison(
    source: OrderedFunction,
    source_name: str,
    target: OrderedFunction,
) -> Comparison:
    target_path = ROOT / "code-full" / (
        f"FUN_{target.address:08X}.disassembled.txt"
    )
    command = [
        "binary-comp",
        "compare",
        "--config",
        str(ROOT / "config" / "binary-comp.json"),
        "--target",
        "full",
        "--no-build",
        source_name,
        str(target_path),
    ]
    process = subprocess.run(command, cwd=ROOT, capture_output=True)
    output = (process.stdout + b"\n" + process.stderr).decode(
        "utf-8", "replace"
    )
    match = SIMILARITY_RE.search(output)
    if match is not None:
        return Comparison(source, target, float(match.group(1)), "")
    lines = [line.strip() for line in output.splitlines() if line.strip()]
    error = " | ".join(lines[-3:])[:500]
    return Comparison(source, target, None, error)


def alignment(
    gap: Gap,
    scores: dict[tuple[int, int], float],
    minimum_score: float,
) -> set[tuple[int, int]]:
    """Return an order-preserving maximum-score set of matrix matches."""
    source_count = len(gap.sources)
    target_count = len(gap.targets)
    values = [
        [(0.0, 0) for _target in range(target_count + 1)]
        for _source in range(source_count + 1)
    ]
    moves = [
        ["" for _target in range(target_count + 1)]
        for _source in range(source_count + 1)
    ]
    for source_index in range(1, source_count + 1):
        moves[source_index][0] = "source-skip"
    for target_index in range(1, target_count + 1):
        moves[0][target_index] = "target-skip"

    for source_index in range(1, source_count + 1):
        source = gap.sources[source_index - 1]
        for target_index in range(1, target_count + 1):
            target = gap.targets[target_index - 1]
            choices = [
                (values[source_index - 1][target_index], "source-skip"),
                (values[source_index][target_index - 1], "target-skip"),
            ]
            score = scores.get((source.address, target.address))
            if score is not None and score >= minimum_score:
                previous = values[source_index - 1][target_index - 1]
                choices.append(
                    ((previous[0] + score - minimum_score, previous[1] + 1),
                     "match")
                )
            best_value, best_move = max(
                choices,
                key=lambda choice: (
                    choice[0][0], choice[0][1], choice[1] == "match"
                ),
            )
            values[source_index][target_index] = best_value
            moves[source_index][target_index] = best_move

    selected: set[tuple[int, int]] = set()
    source_index = source_count
    target_index = target_count
    while source_index > 0 or target_index > 0:
        move = moves[source_index][target_index]
        if move == "match":
            source = gap.sources[source_index - 1]
            target = gap.targets[target_index - 1]
            selected.add((source.address, target.address))
            source_index -= 1
            target_index -= 1
        elif move == "source-skip":
            source_index -= 1
        elif move == "target-skip":
            target_index -= 1
        else:
            break
    return selected


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("audit_tsv", nargs="?", type=Path, default=DEFAULT_AUDIT)
    parser.add_argument("--map", type=Path, default=DEFAULT_MAP)
    parser.add_argument(
        "--provenance-manifest", type=Path, default=DEFAULT_MANIFEST
    )
    parser.add_argument(
        "--similarity-tsv", type=Path, default=DEFAULT_SIMILARITY
    )
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--max-gap", type=int, default=12)
    parser.add_argument("--jobs", type=int, default=8)
    parser.add_argument("--minimum-alignment-score", type=float, default=30.0)
    parser.add_argument("--minimum-report-score", type=float, default=40.0)
    parser.add_argument("--include-equal", action="store_true")
    parser.add_argument(
        "--no-build",
        action="store_true",
        help="trust the existing rebuilt executable and linker map",
    )
    args = parser.parse_args()

    if args.max_gap < 1 or args.jobs < 1:
        parser.error("--max-gap and --jobs must be positive")
    if not args.no_build:
        process = subprocess.run(["make"], cwd=ROOT)
        if process.returncode != 0:
            raise SystemExit("error: build failed")

    mappings = load_map(args.map.resolve())
    provenance = load_provenance(args.provenance_manifest.resolve())
    changes, markers = compute_changes(mappings, provenance=provenance)
    if changes:
        raise SystemExit(
            "error: source annotations drift from the map; run "
            "remapWC1ToWC2.py --patch first"
        )
    markers_by_source: dict[int, Marker] = {
        marker.source: marker for marker in markers if marker.source is not None
    }
    similarities = load_similarities(args.similarity_tsv.resolve())

    gaps = [
        gap
        for gap in load_audit(args.audit_tsv.resolve())
        if max(len(gap.sources), len(gap.targets)) <= args.max_gap
        and (args.include_equal or len(gap.sources) != len(gap.targets))
        and any(source.address in markers_by_source for source in gap.sources)
    ]

    jobs: list[tuple[Gap, OrderedFunction, str, OrderedFunction]] = []
    for gap in gaps:
        for source in gap.sources:
            marker = markers_by_source.get(source.address)
            if marker is None or not marker.source_name:
                continue
            for target in gap.targets:
                jobs.append((gap, source, marker.source_name, target))

    comparisons: list[tuple[Gap, Comparison]] = []
    with ThreadPoolExecutor(max_workers=args.jobs) as executor:
        results = executor.map(
            lambda job: (job[0], run_comparison(job[1], job[2], job[3])),
            jobs,
        )
        comparisons.extend(results)

    by_gap: dict[GapKey, list[Comparison]] = {}
    for gap, comparison in comparisons:
        by_gap.setdefault(gap.key, []).append(comparison)

    selected: set[tuple[GapKey, int, int]] = set()
    for gap in gaps:
        scores = {
            (comparison.source.address, comparison.target.address):
                comparison.similarity
            for comparison in by_gap.get(gap.key, [])
            if comparison.similarity is not None
        }
        for source, target in alignment(
            gap, scores, args.minimum_alignment_score
        ):
            selected.add((gap.key, source, target))

    destination_map = {
        mapping.destination: mapping for mapping in mappings.values()
    }
    rows: list[dict[str, object]] = []
    for gap in gaps:
        source_groups: dict[int, list[Comparison]] = {}
        for comparison in by_gap.get(gap.key, []):
            if comparison.similarity is not None:
                source_groups.setdefault(comparison.source.address, []).append(
                    comparison
                )
        ranks: dict[tuple[int, int], int] = {}
        for source_address, group in source_groups.items():
            group.sort(
                key=lambda item: (-item.similarity, item.target.address)
            )
            for rank, comparison in enumerate(group, 1):
                ranks[(source_address, comparison.target.address)] = rank

        for comparison in by_gap.get(gap.key, []):
            score = comparison.similarity
            is_selected = (
                gap.key, comparison.source.address, comparison.target.address
            ) in selected
            if score is None or (
                score < args.minimum_report_score and not is_selected
            ):
                continue
            current = mappings.get(comparison.source.address)
            occupant = destination_map.get(comparison.target.address)
            current_score = similarities.get(comparison.source.address)
            rows.append(
                {
                    "source_file": gap.key.source_file,
                    "left_wc1": format_address(gap.key.left_wc1),
                    "left_wc2": format_address(gap.key.left_wc2),
                    "right_wc1": format_address(gap.key.right_wc1),
                    "right_wc2": format_address(gap.key.right_wc2),
                    "gap_count_wc1": gap.key.wc1_count,
                    "gap_count_wc2": gap.key.wc2_count,
                    "wc1_address": format_address(comparison.source.address),
                    "source_name": markers_by_source[
                        comparison.source.address
                    ].source_name,
                    "candidate_wc2_address": format_address(
                        comparison.target.address
                    ),
                    "wc2_current_name": comparison.target.name,
                    "candidate_similarity": f"{score:.2f}",
                    "source_rank": ranks[
                        (comparison.source.address, comparison.target.address)
                    ],
                    "alignment_selected": "yes" if is_selected else "",
                    "current_wc2_address": format_address(
                        None if current is None else current.destination
                    ),
                    "current_similarity": ""
                    if current_score is None
                    else f"{current_score:.2f}",
                    "target_occupant_wc1": format_address(
                        None if occupant is None else occupant.source
                    ),
                }
            )

    fieldnames = [
        "source_file",
        "left_wc1",
        "left_wc2",
        "right_wc1",
        "right_wc2",
        "gap_count_wc1",
        "gap_count_wc2",
        "wc1_address",
        "source_name",
        "candidate_wc2_address",
        "wc2_current_name",
        "source_rank",
        "alignment_selected",
        "current_wc2_address",
        "current_similarity",
        "target_occupant_wc1",
        "candidate_similarity",
    ]
    with args.output.resolve().open(
        "w", newline="", encoding="utf-8"
    ) as stream:
        writer = csv.DictWriter(
            stream, fieldnames=fieldnames, delimiter="\t", lineterminator="\n"
        )
        writer.writeheader()
        writer.writerows(
            sorted(
                rows,
                key=lambda row: (
                    row["source_file"],
                    row["left_wc1"],
                    row["wc1_address"],
                    int(row["source_rank"]),
                ),
            )
        )

    errors = sum(
        comparison.similarity is None
        for _gap, comparison in comparisons
    )
    selected_rows = [row for row in rows if row["alignment_selected"]]
    improvements = [
        row
        for row in selected_rows
        if not row["current_similarity"]
        or float(row["candidate_similarity"]) > float(row["current_similarity"])
    ]
    print(
        f"gaps={len(gaps)} matrix_comparisons={len(comparisons)} "
        f"errors={errors} reported_rows={len(rows)} "
        f"aligned_matches={len(selected_rows)} improvements={len(improvements)}"
    )
    for row in sorted(
        improvements,
        key=lambda item: (-float(item["candidate_similarity"]), item["wc1_address"]),
    ):
        old = row["current_similarity"] or "unmapped"
        print(
            f"{row['candidate_similarity']:>6}% {row['source_file']:<20} "
            f"{row['source_name']:<38} {row['wc1_address']} -> "
            f"{row['candidate_wc2_address']} (old {old})"
        )
    print(f"Wrote {args.output.resolve()}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
