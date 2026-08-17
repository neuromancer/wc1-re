#!/usr/bin/env python3
"""Emit an apply_patch patch that regenerates src/map for the WC2 branch."""

from __future__ import annotations

import difflib

from remapWC1ToWC2 import (
    DEFAULT_MANIFEST,
    DEFAULT_MAP,
    ROOT,
    compute_changes,
    load_map,
    load_provenance,
)


def main() -> int:
    mappings = load_map(DEFAULT_MAP)
    provenance = load_provenance(DEFAULT_MANIFEST)
    changes, markers = compute_changes(mappings, provenance=provenance)
    if changes:
        raise SystemExit(
            "error: source annotations do not match the mapping; "
            "run remapWC1ToWC2.py --patch first"
        )

    mapped = [marker for marker in markers if marker.destination is not None]
    unresolved = [marker for marker in markers if marker.destination is None]
    destinations = [marker.destination for marker in mapped]
    if len(destinations) != len(set(destinations)):
        raise SystemExit("error: duplicate WC2 destination in source markers")
    unnamed = [marker for marker in mapped if not marker.source_name]
    if unnamed:
        raise SystemExit(
            f"error: mapped marker has no source name at {unnamed[0].path}:"
            f"{unnamed[0].line}"
        )

    lines = [
        "# Address-sorted mapped developer functions in WC2.EXE.",
        "# Generated from reports/wc2-address-remap.tsv; unresolved functions are",
        "# intentionally omitted until a WC2 destination is established.",
        f"# {len(mapped)} mapped source functions; {len(unresolved)} unresolved.",
        "",
    ]
    for marker in sorted(mapped, key=lambda item: item.destination):
        lines.append(f"0x{marker.destination:08X} {marker.source_name}")
    new_text = "\n".join(lines) + "\n"

    path = ROOT / "src" / "map"
    old_text = path.read_text(encoding="utf-8")
    print("*** Begin Patch")
    print("*** Update File: src/map")
    diff = difflib.unified_diff(
        old_text.splitlines(keepends=True),
        new_text.splitlines(keepends=True),
        fromfile="src/map",
        tofile="src/map",
        n=2,
    )
    for line in list(diff)[2:]:
        if line.startswith("@@"):
            print("@@")
        else:
            print(line, end="")
    print("*** End Patch")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
