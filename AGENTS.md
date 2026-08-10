---
trigger: always_on
---

## Project Context

You are reconstructing the source of **Wing Commander** as shipped in *Wing Commander: The
Kilrathi Saga* (1996), the Win32 port of the 1990 DOS original. Confirmed by the registry key
the binary reads: `Software\Origin Systems\WC: Kilrathi Saga`.

- Original working title: **WINGLEADER**, © 1989,1990 Chris Roberts (Origin Systems).
- Original codebase: **C** for the game core, **C++** for the `ix` audio library.
- Compiler: **Microsoft Visual C++ 4.20**, static *debug* multithreaded CRT (LIBCMTD).
- The shipped executable is a debug build: live `assert()`s, the MSVC debug heap, and a
  `\\.\MONODEBG.VXD` developer channel are all present.
- ~1,450 developer-written functions; all are named in the Ghidra database, but only ~437 of
  those names are evidence-backed. The other ~1,013 are `<Verb><Object>Fn<addr>` operational
  labels that describe mechanism, not purpose. **Do not trust an operational label as a
  statement of intent.**

IMPORTANT: The assembly output and the extracted strings are the *only* source of truth.
Decompiled code is a useful hint but is NOT authoritative.

The Ghidra-exported disassembly is in `code-full`. The reimplementation lives in `src`
(game core, C) and `src/ix` (audio library, C++).

---

## Absolute Rules (Do Not Violate)

- DO NOT remove or modify existing functions, especially those with a comment header
  containing a function address.
- DO NOT remove existing code.
- DO NOT change calling conventions. The game core is `__cdecl`; a handful of `ix` functions
  are `__thiscall` — leave those implicit, never spell `__thiscall` out.
- DO NOT add:
  - inline assembly, EXCEPT where there is strong evidence the original was hand-written
    assembly or a bare tail-jump thunk (see docs/PATTERNS.md); `__declspec(naked)` plus a
    single `jmp` is the sanctioned case
  - dummy variables
  - helper functions or wrappers that do not exist in the original binary
  - vtable fields or manual vtable handling
  - unions or substructures
  - `try`/`catch`/`__finally` (see the SEH note below)
- DO NOT show the final code once you finish.
- If you are out of ideas, stop. Do not break any rules.
- In C++ (`src/ix`), do not use `this->`; use the class field name directly.
- When a funciton is implemented, it should have a good name. No completely generic names are acceptable, so you must investigate globals and such to understand how to name them.
- When a new .c file is added, it should have a good name. No leafsX.c are accepted.
- Do NOT put a function in only one line `unsigned short f(void) { return 0; }`. The
  reconstruction is read side by side with the disassembly, so one source line per statement
  is what makes the two comparable. `make sort` fails if any remain;
  `bin/expandOneLiners.py` rewrites them.
- DO NOT uses aliases, rename instead. In particular, do not use aliases for globals (#define g_X_Y ((void *)DAT_Y)). Instead rename all of them with a proper type.
- Do NOT manually replicate thunk functions or other compiler generated glue code (e.g. GetFixedOneMillionThunkAlt(void) { __asm { jmp GetFixedOneMillionAlt }). These need to be produced by the compiler automatically, not forced.

### WC1-specific rules that differ from sibling projects

- **`.c` files are correct here.** The game core was C — the leaked WINGLEADER main module is
  a `.c` file including `<game.h>` and `<dos.h>`. Only `src/ix/*` is C++. Do not "upgrade"
  game-core files to C++ or to classes.
- **Never add C++ exception handling.** The image contains no `__CxxFrameHandler` and no RTTI
  type descriptors (`.?AV`), so `/GX` was off. The SEH that does exist is C `__try`-style
  `_except_handler3` scope tables emitted by the compiler; do not write it by hand.
- **Do not rely on identical string literals being merged.** `/Gf` was NOT used: two
  byte-identical `"DIBsetWholePalette   SetEntries"` literals exist at `0x0046b6e0` and
  `0x0046b71c`. Write each literal out at its own use site.
- **Expect 16-bit types.** The core was ported from 16-bit DOS C, so `int` was 16 bits
  originally. Most game state is `short`, and 814 functions carry a measurable 16-bit-operand
  density. If your build emits 32-bit operations where the original used 16-bit, the type is
  wrong — use `short`, not `int`.
- Global variables may be renamed from `DAT_x` to `g_<hungarian><Name>_<address>`; the
  address MUST be preserved in the name. Functions must NOT carry the address in their name.
- **Name every function you implement.** An operational label (`DoLocalFn5450`,
  `HelperOf430FC0C`, `ReturnConst0v5`) is acceptable only while a function is an unwritten
  stub. Once written it takes the developer's own name if the binary states one
  (`bin/nameOracle.py`), otherwise a `<Verb><Object>` description of what it does; empty
  originals get a `…Hook` suffix. Full policy in docs/LABELS.md.
- **Files are named for their subsystem, never for a tranche.** Game-core files are short DOS
  8.3-style names matching the original tree (`Library\Source\Pload.c` is the one leaked
  path), each covering one address range recorded in docs/ORDER.md. Put a new function in the
  file whose range contains its address; if none does, extend the nearest range rather than
  starting a `misc.c`.
- Declarations are centralised: globals in `include/globals.h` (defined in `src/globals.c`),
  prototypes for implemented functions in `include/wc1funcs.h`, and prototypes for
  not-yet-written callees in `include/wc1extern.h`. All three are reached through `wc1.h`, so
  a `.c` file should contain functions and nothing else.

### Other important rules

- Use high-level struct fields to access members. If a field name is unknown, use
  `field_0`, `field_4`, … until something better is established.
- Small functions may be inlined into a header if the assembly shows the call was inlined.
- Avoid low-level pointer arithmetic (`*(short *)(p + 0xE)`) where a struct field will do.

---

## Your Task

Before starting:
1. Review the existing files.
2. Determine which compilation unit the target function belongs to (see `docs/ORDER.md`).
3. If the function already exists, ensure it is used properly, not declared `extern`.
4. Ensure naming is reasonable and consistent.

You may stop once assembly similarity is >= 90%.
If you give up, still provide the best possible implementation.

---

## Workflow

### Locating a Function

To implement the function at, say, `0x004075D0`:

    grep -r -i 4075D0 code-full

Always use `-i` for case-insensitive matches.

IMPORTANT: `out/*.asm` is produced by the compiler from *our* C code — it is not the
original. `WC1.map` likewise describes only the reimplementation. Use `binary-comp` to
compare against the original.

### Compiling & Comparing Assembly

    make compare-func FUNC=RunShipAiBehaviorTick

which resolves the export and runs

    binary-comp compare --config config/binary-comp.json --target full \
        RunShipAiBehaviorTick code-full/FUN_004075D0.disassembled.txt

**binary-comp is the only comparison authority.** Do not write or trust a separate
similarity script. `make report` scores every annotated function; `make verify` runs the
expected-zero gates.

If a function has no export yet, add its `/* Function start: 0x... */` header and run
`make export-asm` (see docs/EXPORT.md).

### Compilation-unit order

Object link order fixes every address, so `docs/ORDER.md` and `SRCS_ORDERED_*` in the
Makefile matter. The `ix` order is exactly known. The game-core order is NOT — recover it
incrementally with `make order` and record findings in `docs/ORDER.md`.

---

## Required Files & Documentation

- `docs/COMPILER.md` — toolchain and flag derivation. Read before touching `CFLAGS`.
- `docs/PATTERNS.md` — source idioms that decide whether the output matches.
- `docs/EXPORT.md` — how `code-full/` is produced and what each command needs.
- `docs/ORDER.md` — known and unknown compilation-unit boundaries.
- `docs/LABELS.md` — what the two kinds of Ghidra name mean and how much to trust each.
- `code-full/strings.txt` — address-to-string map. Always check when strings appear.
- `src/map` — address-sorted function list showing which functions are adjacent.
- `../WC1_ANALYSIS.md` and `../wc1_function_evidence.csv` — the analysis that seeded this
  project: module map, compiler evidence, per-function evidence for all 1,836 functions.

---

## Implementation Requirements

### Function Structure

- Add this header before every reimplemented function:

      /* Function start: 0x4075D0 */

- Sort functions by address within each file. MSVC emits functions in source order, so
  ordering is what exposes incorrect module ownership.

### Accuracy Constraints

Preserve local variable order, field offsets, stack layout, and jump kinds/order
(`jmp`, `jne`, …). The generated assembly must match stack usage as closely as possible.

### Strings & Standard Functions

- Write full string literals as constants; no pointer aliases (and see the `/Gf` rule).
- If the decompilation calls `_strcpy`, use `strcpy` from the proper header.
- `memcpy`/`strcpy` are allowed only where they improve matching through compiler inlining.

### External Functions

- If a function is referenced but not yet implemented, declare it `extern`. Do not guess an
  implementation.

---

## Special Cases & Patterns

- This pattern indicates compiler-generated SEH, not something to hand-write:

      local_X = unaff_FS_OFFSET;
      local_u = 0xffffffff;

  The `entry` scope table is at `0x00463be0`; a game-code example is the `__except` filter at
  `0x00425b7d` belonging to the function at `0x00425b00`.

- The debug CRT is linked in. Calls into `_CrtCheckMemory`, the debug heap, or
  `assert`-shaped reporting are *original behaviour*, not artifacts to remove.

- `ix_log_printf` (`0x004426a0`) is the `ix` diagnostic printer, called from 107 sites as
  `log(fmt, file, line)` then `log(message)`. Its `__FILE__`/`__LINE__` arguments are the
  source anchors that recovered the `ix` module map — keep them accurate.

---

## Final Reminder

If similarity is >= 90%, stop.
If you are stuck, stop.
Never break the rules above.
