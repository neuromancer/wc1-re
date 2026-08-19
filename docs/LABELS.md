# Function naming policy

Every function in the Ghidra project has a label, but many labels describe only
mechanism. A name such as `ScanTbl0046C028Fn1210` means the body scans a table;
it does not establish why that table exists. Assembly and extracted strings
remain authoritative.

## Names used in reconstructed source

An implemented function takes the first applicable kind of name:

1. The developer's original name when supported by direct evidence. Preserve
   spelling and case, including forms such as `playWAVE`, `shadow_draw`, and
   `SetMusBreakpt`.
2. A specific `<Verb><Object>` description of observable behavior when no
   original name is known.
3. A name ending in `Hook` when the original function is genuinely empty.

Do not retain operational labels, relationship labels such as `HelperOf...`,
generic counters such as `ReturnConst0v5`, or addresses in function names.
Globals are different: identified globals use
`<hungarian><Name>` with the address in a trailing `/* 0x004xxxxx */` comment on the
declaration and the definition, and unidentified globals retain `DAT_<address>`.

## Evidence priority

Use naming evidence in this order:

1. Names stated by the Win32 binary in diagnostics, paths, symbols, or imports.
2. Names from recovered source or another release, after matching the Win32
   body and callers.
3. Behavior demonstrated by the Win32 disassembly.
4. A temporary operational label for an unwritten function only.

The main external sources are the recovered `BRAINS.C`, Amiga and FM Towns
analysis, Sega CD and Macintosh symbols, WCMissionTools, and the sibling
analysis files [`WC1_ANALYSIS.md`](../../WC1_ANALYSIS.md) and
[`wc1_function_evidence.csv`](../../wc1_function_evidence.csv). These sources
can supply identifiers or structure, but never override Win32 control flow,
constants, widths, or calling conventions.

Cross-release order alone is insufficient. Accept an external name only when
the Win32 body, surrounding functions, callers, and state accesses support the
mapping. Later ports may reorder, split, add, or remove functions.

## Name oracle

The debug build retains many developer-facing log strings. The name oracle
finds functions that print those strings:

```sh
bin/nameOracle.py --top
bin/nameOracle.py
```

It scans call sites and associates the last pushed string with the enclosing
function. This recovers names such as the `DIB*` and music functions. Always
inspect the disassembly before accepting a result because a wrapper may log on
behalf of its callee.

## Operational labels

Operational labels are verified descriptions of code shape, not intent:

| Prefix | Established mechanism |
| --- | --- |
| `Blit`, `Draw`, `Palette` | Raster or palette calls |
| `Sound`, `Stream` | Audio entry points |
| `Load`, `Release` | Resource loading or release paths |
| `Text` | Formatting, string lookup, or display |
| `Input`, `Ai` | Input or AI calls |
| `Win32` | Direct Win32 API use |
| `Calc` | Floating-point calculation |
| `Scan` | Loop over an indexed global table |
| `Dispatch`, `Loop` | Switch dispatch or a non-table loop |
| `Set`, `Get` | Predominantly writes or reads state |

Use the shared table token to investigate related functions, then replace the
label once the behavior is understood. Subsystem hypotheses belong in Ghidra
tags, not source identifiers.

## Acceptance checklist

Before renaming or implementing a function:

1. Read its original disassembly and extracted strings.
2. Verify its address, callers, arguments, return width, and terminating `RET`.
3. Check external evidence without assuming positional equivalence.
4. Choose the narrowest name justified by those facts.
5. Update Ghidra, `src/map`, declarations, and call sites consistently.
6. Run `make audit-addresses` and compare the function with `binary-comp`.

Functions reached only through a pointer table or relocation are still real
functions. Check the Ghidra reachability tags before treating a no-caller body
as dead or compiler-generated glue.
