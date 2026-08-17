#!/usr/bin/env python3
"""Audit WC1-to-WC2 mappings using function order between trusted anchors.

MSVC emits functions in source order.  When a compilation unit kept the same
order in WC2, two trusted mappings bound a useful sequence-alignment problem:
if both binaries contain the same number of functions between the anchors,
their ordinal positions are strong mapping candidates.

This script deliberately does not calculate assembly similarity.  It reports
order candidates and any existing ``binary-comp`` score supplied through the
similarity TSV; every proposed mapping still needs a direct binary-comp check.

Examples::

    python3 bin/auditWC2FunctionOrder.py \
        --similarity-tsv /tmp/wc2-current-similarity.tsv

    python3 bin/auditWC2FunctionOrder.py \
        --source-file src/geom.c --include-confirmed --include-unequal
"""

from __future__ import annotations

import argparse
import csv
import itertools
import re
import sys
from dataclasses import dataclass
from pathlib import Path

from remapWC1ToWC2 import (
    DEFAULT_MANIFEST,
    DEFAULT_MAP,
    ROOT,
    Marker,
    Mapping,
    compute_changes,
    load_map,
    load_provenance,
)


EXPORT_RE = re.compile(r"^FUN_([0-9A-Fa-f]+)\.disassembled\.txt$")
FUNCTION_RE = re.compile(r"^Function:\s*(.*)$", re.MULTILINE)
ADDRESS_RE = re.compile(r"^Address:\s*0x([0-9A-Fa-f]+)$", re.MULTILINE)
DEFAULT_WC1_EXPORTS = ROOT.parent / "wc1-test" / "code-full"
DEFAULT_WC2_EXPORTS = ROOT / "code-full"
DEFAULT_SIMILARITY = ROOT / "reports" / "wc2-similarity.tsv"
DEFAULT_TRUSTED_EVIDENCE = (
    "WC1-auto-vt",
    "WC1-manual-verified",
)


@dataclass(frozen=True)
class ExportFunction:
    address: int
    name: str
    instruction_count: int


@dataclass(frozen=True)
class Gap:
    source_file: str
    left: Marker
    right: Marker
    wc1_functions: tuple[ExportFunction, ...]
    wc2_functions: tuple[ExportFunction, ...]

    @property
    def equal_count(self) -> bool:
        return len(self.wc1_functions) == len(self.wc2_functions)


@dataclass(frozen=True)
class Candidate:
    gap: Gap
    ordinal: int
    wc1: ExportFunction | None
    wc2: ExportFunction | None
    marker: Marker | None
    current: Mapping | None
    occupant: Mapping | None
    similarity: float | None
    status: str


def format_address(address: int | None) -> str:
    if address is None:
        return ""
    return f"{address:08X}"


def read_export(path: Path) -> ExportFunction:
    match = EXPORT_RE.match(path.name)
    if match is None:
        raise ValueError(f"unexpected export name: {path.name}")
    filename_address = int(match.group(1), 16)
    text = path.read_text(encoding="utf-8", errors="replace")
    function_match = FUNCTION_RE.search(text)
    address_match = ADDRESS_RE.search(text)
    if function_match is None or address_match is None:
        raise ValueError(f"missing Function/Address header: {path}")
    address = int(address_match.group(1), 16)
    if address != filename_address:
        raise ValueError(
            f"filename/header address mismatch in {path}: "
            f"{filename_address:08X} != {address:08X}"
        )
    instruction_count = 0
    body_started = False
    for line in text.splitlines():
        if body_started:
            if line.strip():
                instruction_count += 1
        elif not line.strip() and address_match is not None:
            body_started = True
    return ExportFunction(
        address=address,
        name=function_match.group(1).strip(),
        instruction_count=instruction_count,
    )


def load_exports(directory: Path) -> tuple[list[ExportFunction], dict[int, int]]:
    if not directory.is_dir():
        raise SystemExit(f"error: disassembly directory not found: {directory}")
    functions: list[ExportFunction] = []
    for path in directory.iterdir():
        if EXPORT_RE.match(path.name):
            functions.append(read_export(path))
    functions.sort(key=lambda item: item.address)
    if not functions:
        raise SystemExit(f"error: no disassembly exports found in {directory}")
    indices = {function.address: index for index, function in enumerate(functions)}
    if len(indices) != len(functions):
        raise SystemExit(f"error: duplicate export address in {directory}")
    return functions, indices


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
            if not row["wc1_address"]:
                continue
            result[int(row["wc1_address"], 16)] = float(
                row["binary_similarity"]
            )
    return result


def marker_file(marker: Marker) -> str:
    return str(marker.path.relative_to(ROOT))


def is_trusted_anchor(
    marker: Marker,
    similarities: dict[int, float],
    minimum_similarity: float,
    trusted_evidence: set[str],
) -> bool:
    if marker.source is None or marker.destination is None:
        return False
    similarity = similarities.get(marker.source)
    return (
        similarity is not None and similarity >= minimum_similarity
    ) or marker.evidence in trusted_evidence


def collect_gaps(
    markers: list[Marker],
    wc1_exports: list[ExportFunction],
    wc1_indices: dict[int, int],
    wc2_exports: list[ExportFunction],
    wc2_indices: dict[int, int],
    similarities: dict[int, float],
    minimum_similarity: float,
    trusted_evidence: set[str],
    source_files: set[str],
) -> tuple[list[Gap], int, int]:
    by_file: dict[str, list[Marker]] = {}
    for marker in markers:
        if marker.source is None:
            continue
        relative = marker_file(marker)
        if source_files and relative not in source_files:
            continue
        by_file.setdefault(relative, []).append(marker)

    gaps: list[Gap] = []
    anchor_count = 0
    inversion_count = 0
    for relative, file_markers in sorted(by_file.items()):
        anchors = [
            marker
            for marker in sorted(file_markers, key=lambda item: item.source)
            if is_trusted_anchor(
                marker,
                similarities,
                minimum_similarity,
                trusted_evidence,
            )
            and marker.source in wc1_indices
            and marker.destination in wc2_indices
        ]
        anchor_count += len(anchors)
        for left, right in itertools.pairwise(anchors):
            assert left.source is not None and right.source is not None
            assert left.destination is not None and right.destination is not None
            wc1_left = wc1_indices[left.source]
            wc1_right = wc1_indices[right.source]
            wc2_left = wc2_indices[left.destination]
            wc2_right = wc2_indices[right.destination]
            if wc2_right <= wc2_left:
                inversion_count += 1
                continue
            gaps.append(
                Gap(
                    source_file=relative,
                    left=left,
                    right=right,
                    wc1_functions=tuple(wc1_exports[wc1_left + 1 : wc1_right]),
                    wc2_functions=tuple(wc2_exports[wc2_left + 1 : wc2_right]),
                )
            )
    return gaps, anchor_count, inversion_count


def candidate_status(
    wc1: ExportFunction | None,
    wc2: ExportFunction | None,
    marker: Marker | None,
    current: Mapping | None,
    occupant: Mapping | None,
    equal_count: bool,
) -> str:
    if wc1 is None:
        return "wc2-insertion"
    if wc2 is None:
        return "wc1-elimination"
    if not equal_count:
        return "unequal-gap-position"
    if current is not None and current.destination == wc2.address:
        return "confirmed-order"
    if marker is None and current is None:
        return "untracked-export"
    source_conflict = current is not None
    target_conflict = occupant is not None and occupant.source != wc1.address
    if source_conflict and target_conflict:
        return "source-and-target-conflict"
    if source_conflict:
        return "candidate-remap"
    if target_conflict:
        return "candidate-target-conflict"
    return "candidate-unmapped"


def make_candidates(
    gap: Gap,
    mappings: dict[int, Mapping],
    by_destination: dict[int, Mapping],
    markers_by_source: dict[int, Marker],
    similarities: dict[int, float],
) -> list[Candidate]:
    candidates: list[Candidate] = []
    pairs = itertools.zip_longest(gap.wc1_functions, gap.wc2_functions)
    for ordinal, (wc1, wc2) in enumerate(pairs, 1):
        marker = None if wc1 is None else markers_by_source.get(wc1.address)
        current = None if wc1 is None else mappings.get(wc1.address)
        occupant = None if wc2 is None else by_destination.get(wc2.address)
        similarity = None if wc1 is None else similarities.get(wc1.address)
        status = candidate_status(
            wc1,
            wc2,
            marker,
            current,
            occupant,
            gap.equal_count,
        )
        candidates.append(
            Candidate(
                gap=gap,
                ordinal=ordinal,
                wc1=wc1,
                wc2=wc2,
                marker=marker,
                current=current,
                occupant=occupant,
                similarity=similarity,
                status=status,
            )
        )
    return candidates


def candidate_is_visible(
    candidate: Candidate,
    include_confirmed: bool,
    include_untracked: bool,
    include_unequal: bool,
) -> bool:
    if not candidate.gap.equal_count and not include_unequal:
        return False
    if candidate.status == "confirmed-order" and not include_confirmed:
        return False
    if candidate.status == "untracked-export" and not include_untracked:
        return False
    if candidate.marker is None and candidate.current is None:
        return include_untracked or candidate.wc1 is None
    return True


def anchor_text(marker: Marker) -> str:
    assert marker.source is not None and marker.destination is not None
    return (
        f"{format_address(marker.source)} {marker.source_name} -> "
        f"{format_address(marker.destination)}"
    )


def print_human(
    gaps: list[Gap],
    candidates_by_gap: list[list[Candidate]],
    args: argparse.Namespace,
    anchor_count: int,
    inversion_count: int,
) -> None:
    equal_count = sum(gap.equal_count for gap in gaps)
    unequal_count = len(gaps) - equal_count
    oversize = sum(
        max(len(gap.wc1_functions), len(gap.wc2_functions)) > args.max_gap
        for gap in gaps
    )
    print(
        f"Trusted anchors: {anchor_count}; bounded gaps: {len(gaps)}; "
        f"equal-count gaps: {equal_count}; unequal gaps: {unequal_count}; "
        f"inversions skipped: {inversion_count}; oversized gaps: {oversize}"
    )
    print(
        "Order is a candidate signal only; validate proposed destinations with "
        "binary-comp before updating the map."
    )

    displayed = 0
    for gap, candidates in zip(gaps, candidates_by_gap):
        size = max(len(gap.wc1_functions), len(gap.wc2_functions))
        if size == 0 or size > args.max_gap:
            continue
        visible = [
            candidate
            for candidate in candidates
            if candidate_is_visible(
                candidate,
                args.include_confirmed,
                args.include_untracked,
                args.include_unequal,
            )
        ]
        if not visible:
            continue
        displayed += len(visible)
        relation = "equal" if gap.equal_count else "unequal"
        print()
        print(
            f"{gap.source_file}: {anchor_text(gap.left)} .. "
            f"{anchor_text(gap.right)}"
        )
        print(
            f"  gap counts WC1={len(gap.wc1_functions)} "
            f"WC2={len(gap.wc2_functions)} ({relation})"
        )
        for candidate in visible:
            wc1_address = format_address(
                None if candidate.wc1 is None else candidate.wc1.address
            )
            wc1_name = "-" if candidate.wc1 is None else candidate.wc1.name
            wc2_address = format_address(
                None if candidate.wc2 is None else candidate.wc2.address
            )
            wc2_name = "-" if candidate.wc2 is None else candidate.wc2.name
            details: list[str] = [candidate.status]
            if candidate.current is not None and (
                candidate.wc2 is None
                or candidate.current.destination != candidate.wc2.address
            ):
                details.append(
                    f"current={format_address(candidate.current.destination)}"
                )
            if candidate.occupant is not None and (
                candidate.wc1 is None
                or candidate.occupant.source != candidate.wc1.address
            ):
                details.append(
                    "target-owned-by="
                    f"{format_address(candidate.occupant.source)}"
                )
            if candidate.similarity is not None:
                details.append(f"current-score={candidate.similarity:.2f}%")
            print(
                f"  {candidate.ordinal:>2}. {wc1_address} {wc1_name} -> "
                f"{wc2_address} {wc2_name} [{'; '.join(details)}]"
            )
    print()
    print(f"Displayed candidate/context rows: {displayed}")


def write_tsv(path: Path, candidates: list[Candidate]) -> None:
    with path.open("w", newline="", encoding="utf-8") as stream:
        writer = csv.writer(stream, delimiter="\t", lineterminator="\n")
        writer.writerow(
            (
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
                "current_wc2_address",
                "current_binary_similarity",
                "target_occupant_wc1",
                "status",
            )
        )
        for candidate in candidates:
            wc1 = candidate.wc1
            wc2 = candidate.wc2
            writer.writerow(
                (
                    candidate.gap.source_file,
                    format_address(candidate.gap.left.source),
                    format_address(candidate.gap.left.destination),
                    format_address(candidate.gap.right.source),
                    format_address(candidate.gap.right.destination),
                    len(candidate.gap.wc1_functions),
                    len(candidate.gap.wc2_functions),
                    candidate.ordinal,
                    format_address(None if wc1 is None else wc1.address),
                    "" if wc1 is None else wc1.name,
                    "" if wc1 is None else wc1.instruction_count,
                    format_address(None if wc2 is None else wc2.address),
                    "" if wc2 is None else wc2.name,
                    "" if wc2 is None else wc2.instruction_count,
                    format_address(
                        None
                        if candidate.current is None
                        else candidate.current.destination
                    ),
                    ""
                    if candidate.similarity is None
                    else f"{candidate.similarity:.2f}",
                    format_address(
                        None
                        if candidate.occupant is None
                        else candidate.occupant.source
                    ),
                    candidate.status,
                )
            )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--wc1-exports", type=Path, default=DEFAULT_WC1_EXPORTS)
    parser.add_argument("--wc2-exports", type=Path, default=DEFAULT_WC2_EXPORTS)
    parser.add_argument("--map", type=Path, default=DEFAULT_MAP)
    parser.add_argument(
        "--provenance-manifest", type=Path, default=DEFAULT_MANIFEST
    )
    parser.add_argument(
        "--similarity-tsv",
        type=Path,
        default=DEFAULT_SIMILARITY,
        help="summarizeWC2Report.py --rows-tsv output",
    )
    parser.add_argument(
        "--minimum-anchor-similarity", type=float, default=90.0
    )
    parser.add_argument(
        "--trusted-evidence",
        action="append",
        default=[],
        help="mapping evidence accepted as an anchor without a report score",
    )
    parser.add_argument(
        "--source-file",
        action="append",
        default=[],
        help="restrict to a compilation unit such as src/geom.c",
    )
    parser.add_argument("--max-gap", type=int, default=32)
    parser.add_argument("--include-confirmed", action="store_true")
    parser.add_argument("--include-untracked", action="store_true")
    parser.add_argument("--include-unequal", action="store_true")
    parser.add_argument("--tsv", type=Path)
    args = parser.parse_args()

    if args.max_gap < 1:
        parser.error("--max-gap must be positive")
    trusted_evidence = set(DEFAULT_TRUSTED_EVIDENCE)
    trusted_evidence.update(args.trusted_evidence)

    mappings = load_map(args.map.resolve())
    provenance = load_provenance(args.provenance_manifest.resolve())
    changes, markers = compute_changes(mappings, provenance=provenance)
    if changes:
        raise SystemExit(
            "error: source annotations drift from the map; run "
            "remapWC1ToWC2.py --patch first"
        )
    similarities = load_similarities(
        None if args.similarity_tsv is None else args.similarity_tsv.resolve()
    )
    wc1_exports, wc1_indices = load_exports(args.wc1_exports.resolve())
    wc2_exports, wc2_indices = load_exports(args.wc2_exports.resolve())
    gaps, anchor_count, inversion_count = collect_gaps(
        markers,
        wc1_exports,
        wc1_indices,
        wc2_exports,
        wc2_indices,
        similarities,
        args.minimum_anchor_similarity,
        trusted_evidence,
        set(args.source_file),
    )

    by_destination = {
        mapping.destination: mapping for mapping in mappings.values()
    }
    markers_by_source = {
        marker.source: marker for marker in markers if marker.source is not None
    }
    candidates_by_gap = [
        make_candidates(
            gap,
            mappings,
            by_destination,
            markers_by_source,
            similarities,
        )
        for gap in gaps
    ]
    print_human(
        gaps,
        candidates_by_gap,
        args,
        anchor_count,
        inversion_count,
    )
    if args.tsv is not None:
        write_tsv(
            args.tsv.resolve(),
            [candidate for group in candidates_by_gap for candidate in group],
        )
        print(f"Wrote complete order audit TSV: {args.tsv.resolve()}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
