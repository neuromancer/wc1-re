# WC1 to WC2 name transfer

`TransferWC1NamesToWC2.java` runs Ghidra Auto Version Tracking from
`/WC1%2FWC1.EXE` to `/WC2/WC2.EXE`. Run it with WC2 as the current program.
It creates these project artifacts before applying names:

- `/WC2/backups/WC2-before-WC1-name-transfer`
- `/WC2/WC1-to-WC2-auto-v1`

The automatic pass applies function names only, and only over default names.
It does not copy prototypes, calling conventions, variables, comments, labels,
or data types. The script refuses to overwrite an existing session, making an
accidental second application visible rather than silently changing WC2.

`wc1_wc2_name_map.tsv` is the portable result of the conservative and
aggressive passes. It contains 1,278 one-to-one function mappings, both
executable SHA-256 hashes, the originating WC1 address, the WC2 address, the
evidence bucket, similarity/confidence values, and the caller/callee lists from
both programs. This makes the current result reproducible without rerunning the
heuristic search.

The `wc2-address-remap` source branch consumes that same TSV with:

```sh
python3 bin/remapWC1ToWC2.py --patch --from-wc1
```

Mapped `Function start` and prototype labels are replaced outright with WC2
addresses. Unresolved functions receive `WC2_UNMAPPED`; WC1 addresses are kept
only in `reports/wc2-address-remap.tsv` as migration evidence. On the converted
branch, `make wc2-remap-audit` checks that rerunning the projection would make
no changes.

To replay the map, open an analyzed copy of this exact WC2 executable and run
`ApplyWC1WC2NameMap.java`, passing the TSV path as its first script argument.
Pass `dry-run` as another argument to validate all hashes, functions, names,
addresses, and collisions without changing the program. The importer applies
only function names, function tags, and repeatable audit comments; it validates
the entire map before beginning. It never overwrites a non-default conflicting
name. With `allow-conflicts`, conflicting rows are skipped while the remaining
rows are applied.

`ExportWC1WC2NameMap.java` regenerates the TSV rows from the saved Version
Tracking session and pristine backup. Optional numeric script arguments select
an output offset and limit. The exporter prints the data between `MAP_BEGIN`
and `MAP_END` markers.

## Current result

WC2 began with 1,763 default-named internal functions. The transfer renamed
1,278 of them and leaves 485 default names. Counting functions that already had
the same library or API name, 1,547 of WC1's 1,830 non-default-named internal
functions now have a one-to-one internal WC2 association (84.5%).

The aggressive tiers combine structural comparisons, accepted caller/callee
anchors, iterative graph propagation, and exact local order between two
consecutive accepted anchors. They are manual matches in the Version Tracking
session, and every renamed destination has a repeatable audit comment with its
WC1 source address and available evidence.

Review candidates using the Function Tags window, roughly in this order:

1. `WC1-review-module-mismatch` (28): inferred core/ix/CRT region disagrees.
2. `WC1-fuzzy-very-low` (177): permissive second-pass candidates.
3. `WC1-fuzzy-low` (160): initial candidates below the medium threshold.
4. `WC1-callgraph-single-low` (64): lower-confidence unique one-edge matches.
5. `WC1-fuzzy-followup` (168): score >= 0.50 or at least two graph anchors.
6. `WC1-fuzzy-medium` (103): score >= 0.615 or one graph anchor.
7. `WC1-order-exact` (24): exact unmatched order between accepted anchors.
8. `WC1-callgraph-single` (21): unique one-edge matches with structural support.
9. `WC1-callgraph-propagated` (305): iterative multi-anchor graph matches.
10. `WC1-fuzzy-high` (30): score >= 0.85 or at least two graph anchors.
11. `WC1-callgraph-transfer` (22): caller/callee-implied candidates.
12. `WC1-auto-vt` (204): conservative Auto Version Tracking matches.

These tags describe evidence strength, not correctness. In particular, WC1's
operational placeholder names remain operational placeholders when transferred.
Reject a bad association in the Version Tracking session, restore the WC2
function's default name, and remove its transfer tag. The original WC2 import is
available in the backup above for a complete rollback.

## Compiler comparison

Both executables are 32-bit Microsoft C/C++ debug-runtime builds from the same
MSVC-era family: both contain the Visual C++ debug-library strings and debug
CRT behavior. They are not byte-for-byte products of an identical linker.
The WC1 PE header reports linker 4.20, while WC2 reports linker 3.10. This is
evidence for the same compiler lineage with a different tool revision. The
selected MSVC 4.1 package reports compiler 10.10.6038 and linker 3.10.6038,
matching WC2's linker stamp exactly. With `/Od /Oi`, it produces a 74.01%
average over the 1,215 reportable mapped source functions; the strongest
automatic Version Tracking tier averages 97.52% and the ix code averages
99.12%.

## Replay verification

The final map was replayed on a disposable copy of the pristine WC2 backup.
All 1,278 names, 1,306 primary/review tag assignments, and 1,278 audit
comments were restored. Across all 2,162 internal functions, the replay and
working WC2 databases had zero name, repeatable-comment, function-tag, or tag
definition differences. The disposable program and its temporary project
folder were removed after the comparison.
