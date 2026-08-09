# wc1-re

Source reconstruction of **Wing Commander** as shipped in *Wing Commander: The Kilrathi Saga*
(1996) — the Win32 port of the 1990 DOS original, internally still called **WINGLEADER**.

The goal is a byte-faithful rebuild: the same compiler, the same flags, the same link order, so
that [`binary-comp`](https://github.com/gg-sl-oss/binary-comp) can check the rebuilt executable
against the original at the instruction and data-layout level.

## Target

| | |
|---|---|
| Executable | `WC1.EXE`, 518,656 bytes, MD5 `b20a68b7e45f837e59f7e31bab2e2020` |
| Built | 1996-09-24 16:33 UTC |
| Toolchain | Microsoft Visual C++ **4.20**, static **debug** multithreaded CRT (LIBCMTD) |
| Language | **C** for the game core, **C++** for the `ix` audio library |
| Developer functions | ~1,450 (1,326 game core + 124 `ix`); a further 386 are CRT |

The shipped retail binary is a debug build — live `assert()`s, the MSVC debug heap, and a
`\\.\MONODEBG.VXD` developer channel are all present. That is what made the `ix` library's
module map exactly recoverable.

See [`docs/COMPILER.md`](docs/COMPILER.md) for how the toolchain and every compiler flag was
derived from the binary.

## Setup

```sh
git submodule update --init --recursive     # msvc420, wibo, binary-comp
```

Then supply the pieces that cannot be vendored:

1. **The original executable.** `make data/full/WC1.ORI.EXE` copies it from
   `../releases/win32/WC1.EXE`; override with `ORIGINAL_SRC=/path/to/WC1.EXE`.
2. **`3rdparty/msvcrt40.dll`** — the MSVC420 submodule ships one that does not work under
   wibo; drop a working copy here.
3. **The game disc**, if you want to run it. Point `WC1_ISO` at an image or a directory, or
   drop it at `data/full/wc1.iso`; the binary really does look for its CD
   (`FindCdRomDriveByVolumeLabel`, `PromptInsertCorrectCd`) and the streaming music lives
   there.
4. **`code-full/`** — the original disassembly. `make export-asm` generates it from the PE
   for whatever is annotated in `src/`; for the full export with recovered names, globals
   and strings, run binary-comp's `ghidra_scripts/ExportToCompile.java` from Ghidra's
   Script Manager. See [`docs/EXPORT.md`](docs/EXPORT.md).

`ddraw.lib` / `dsound.lib` and their headers are already inside the MSVC420 submodule, so no
separate DirectX SDK is needed.

If you already have the sibling project checked out, you can reuse its `binary-comp` instead
of the submodule:

```sh
make report BINARY_COMP="env PYTHONPATH=../my-teacher-is-an-alien-re/binary-comp/src python3 -m binary_comp.cli"
```

## Building

```sh
make                        # build WC1.EXE
make export-asm             # (re)generate code-full/ from the original PE
make compare-func FUNC=X    # compare one function against the original
make report                 # per-function similarity + summary
make order                  # compilation-unit boundary hints
make verify                 # the primary verification checklist
make progress               # reimplementation progress
make sort                   # check source files are address-sorted
make run                    # build and launch in DREAMM
```

## Running it

`make run` launches the rebuilt executable in [DREAMM](https://aarongiles.com/dreamm), which
is downloaded on demand into `.dreamm/` on first use. `make run-original` does the same for
the retail binary, and `make debug` launches DREAMM's debugger.

**Wine is not used.** The Kilrathi Saga port is a 1996 Win32 binary that drives DirectDraw and
DirectSound directly and expects a real Windows 95 environment. DREAMM emulates that; Wine
reimplements those APIs, changing exactly the behaviour this project exists to observe.

Put the Kilrathi Saga disc image in `data/` (or set `WC1_ISO=`) and the game installs itself:
`make run` extracts the disc's `/WC1` tree — `WC1.EXE`, `GAMEDAT`, `STREAMS`, `WINGCMDR.CFG`,
142 MB of the disc's 634 — into `data/full/`, mounts the image at `D:` for the CD check and
the streaming music, and mounts `data/full/hd` as a writable `C:` so the registry settings and
saved games persist between runs.

All comparison and verification goes through **binary-comp**; the Makefile exposes every
one of its commands (`calls`, `compare`, `data`, `exe`, `export-asm`, `global-access`,
`globals`, `order`, `report`, `seh`, `triage`, `values`, `vtables`). The only targets from
the sibling project that are absent are the `*-demo` ones -- WC1 shipped no demo build.

## Layout

```
src/            game core (C), one file per address range -- see docs/ORDER.md
src/ix/         ix audio library (C++), one file per original module
include/        wc1.h plus the generated globals.h / wc1funcs.h / wc1extern.h
config/         binary-comp configuration
docs/           COMPILER.md, PATTERNS.md, EXPORT.md, ORDER.md, LABELS.md
bin/            showProgress.py, sortByAddress.py, sweepFlags.py,
                auditAddresses.py, nameOracle.py
tools/          analyze_clang.sh, analyze_static.sh (analysis-only Clang passes)
code-full/      Ghidra export (not vendored)
data/full/      original executable and game data (not vendored)
out/            build artifacts (.obj/.asm/.stdout)
```

## Where the analysis came from

This repository is seeded by the reverse-engineering work in the parent directory:

- `../WC1_ANALYSIS.md` — toolchain evidence, region map, `ix` module layout, the ship-AI
  dispatch table, the labelling scheme.
- `../wc1_function_evidence.csv` — all 1,836 functions with module, size, 16-bit-operand
  density, callers, nearest named ancestor, imports, strings and assert anchors.

Three community projects in the parent directory are also used as naming evidence —
`WCMissionTools` for the data-file record layouts and the ship/order/pilot enumerations,
`WingCommanderArduinoBridge` for the pilot record's field order, and `wcdx` for PE layout.
Everything adopted from them is checked against the image first and recorded in
[`include/wcdata.h`](include/wcdata.h); that check corrected two ship names.

Two things to read before writing code:

- [`docs/ORDER.md`](docs/ORDER.md) — the `ix` link order is exact; the game-core order is
  **not**, and a wrong boundary invalidates every address after it.
- [`docs/LABELS.md`](docs/LABELS.md) — only 437 of the 1,450 Ghidra names are evidence-backed.
  The other 1,013 are `<Verb><Object>Fn<addr>` labels describing *mechanism, not purpose*.
  Do not treat one as a statement of intent.

## Rules

[`AGENTS.md`](AGENTS.md) holds the reconstruction rules. The important WC1-specific ones:
`.c` files are correct for the game core, never add C++ exception handling, do not rely on
identical string literals being pooled, and prefer `short` over `int` — the core was ported
from 16-bit DOS C.

## Status

**347 of 1,450 developer functions reimplemented (23.9%)** — 325 game core, 22 `ix`.
Of the 348 written so far, **234 match the original exactly** and 259 are at 90% or better.

Every implemented function carries a real name: the developer's own where the binary states
one, otherwise a description of what it does. `bin/nameOracle.py` recovers the former from the
debug build's own diagnostic strings — that is how `FadeMusic`, `SetMusicOn`, `SetMusBreakpt`,
`FlushSoundEffects` and the `DIB*` family got their names back.

`src/ix/*.cpp` carry the full per-module function lists (in original source order, with
recovered assert line numbers) as stubs. `src/main.c` documents `main()` at `0x004274E0` but
deliberately does not implement it: its body *is* the init call order, and writing it before
the callees are recovered would assert the very thing the comparison exists to check.
