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
3. **`3rdparty/dx/lib/{ddraw,dsound}.lib`** plus matching headers in `3rdparty/dx/include`.
   `DDRAW.DLL` and `DSOUND.DLL` are bound through the import table, so import libraries are
   needed at link time and MSVC 4.2 predates them.
4. **`code-full/`** — the Ghidra-exported disassembly, strings and globals for `WC1.EXE`.

If you already have the sibling project checked out, you can reuse its `binary-comp` instead
of the submodule:

```sh
make report BINARY_COMP="env PYTHONPATH=../my-teacher-is-an-alien-re/binary-comp/src python3 -m binary_comp.cli"
```

## Building

```sh
make                # build WC1.EXE
make progress       # reimplementation progress
make report         # per-function similarity against the original
make order          # compilation-unit boundary hints
make verify         # the primary verification checklist
make run-wine       # build and launch under Wine
make sort           # check source files are address-sorted
```

## Layout

```
src/            game core (C).  src/map is the address-sorted function inventory
src/ix/         ix audio library (C++), one file per original module
include/        shared headers (wc1.h)
config/         binary-comp configuration
docs/           COMPILER.md, ORDER.md, LABELS.md
bin/            showProgress.py, sortByAddress.py
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

Scaffolding only. `src/ix/*.cpp` carry the full per-module function lists (in original source
order, with recovered assert line numbers) as stubs; `src/main.c` documents `main()` at
`0x004274E0` but is deliberately not implemented, because writing it before its init callees
are recovered would mean inventing the call order that the comparison exists to verify.
