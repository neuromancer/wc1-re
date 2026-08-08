# Matching patterns

Things learned by actually matching functions. Each entry is a case where the obvious
source form compiles to *almost* the right code, and the fix is not a compiler flag.

## The two halves use different optimizer settings

Established before writing any code, from the shape of the original functions:

- **Game core: optimized.** `GetShiftKeyState` (`0x00403060`) is four instructions with no
  prologue. `MinShort` (`0x0041D0C0`) reads arguments straight off `ESP`.
  `RandomBelowOrEqual` (`0x00434D50`) schedules `POP ESI` between `CDQ` and `IDIV` and
  tail-duplicates its epilogue.
- **`ix` library: `/Od`.** Every `ix` function opens
  `PUSH EBP / MOV EBP,ESP / PUSH EBX / PUSH ESI / PUSH EDI` — saving all three registers
  whether used or not — spills to stack temporaries like `[EBP-4]`, and jumps to one shared
  `POP EDI / POP ESI / POP EBX / LEAVE / RET` epilogue.

`CFLAGS_CORE` and `CFLAGS_IX` in the Makefile keep these apart.

A flag sweep (`bin/sweepFlags.py`) over `MinShort`/`MaxShort` showed the whole `/O2` family
(`/O2`, `/Ox`, `/Og /Oi /Ot /Oy /Ob1`, ±`/Gs`) plateaus at the same result, while every
`/Os` (favour size) variant is dramatically worse. So the optimizer *family* is settled;
residual differences are source-level.

## Early return vs assign-then-return

`MinShort` originally ends with the one-byte-shorter 32-bit move:

    MOV EAX,ECX        ; original
    mov ax, cx         ; what we first produced

No flag changes this. The difference is the source idiom:

```c
/* emits  mov ax, cx  -- WRONG */
short MinShort(short a, short b) { if (b <= a) a = b; return a; }

/* emits  mov eax, ecx  -- matches */
short MinShort(short a, short b) { if (a < b) return a; return b; }
```

With two `return` statements the compiler moves the whole register (the upper half is
already garbage from a 16-bit load and the result is only read as `short`). With
assign-then-return it keeps everything 16-bit.

**Rule of thumb: prefer early return when the original ends in a 32-bit `mov` between
registers that were loaded 16-bit.**

## Calling conventions are not uniform

`RandomInRange` (`0x00434D20`) ends in `RET 0x8` — callee cleanup — so it is `__stdcall`,
while its neighbour `RandomBelowOrEqual` (`0x00434D50`) ends in a bare `RET` and is
`__cdecl`. Both live in the same address neighbourhood.

Always check the terminating `RET` before writing the prototype. `RET n` ⇒ `__stdcall`
(MSVC emits the same code for `__pascal`), bare `RET` ⇒ `__cdecl`.

## Return width decides whether a sign-extension appears

`RandomInRange` originally finishes:

    MOV DI,word ptr [ESP + 0xc]     ; 16-bit load, upper half of EDI is garbage
    ...
    LEA EAX,[EDI + EDX*0x1]         ; uses the full EDI anyway

Declaring it `int` made us emit `movsx eax, di; add eax, edx` — correct C, wrong code.
Declaring it `short` reproduces the `LEA`: the upper garbage does not matter because only
`AX` is read. If the original uses a value in a 32-bit operation right after loading it
16-bit, the return/expression type is probably 16-bit.

## `BOOLEAN` collides with windows.h

The DOS source has `BOOLEAN window_colored = FALSE;`, but `<windows.h>` already defines
`BOOLEAN` as `BYTE` and MSVC 4.2 rejects redefinition with a different base type. The port
must therefore be using the windows.h one; do not redeclare it.

## Reading the comparison

**All comparisons go through binary-comp.** It is the only scorer; do not hand-roll one.

    make compare-func FUNC=MinShort    # one function, instruction-by-instruction
    make report                        # every annotated function + summary
    make report FILTER=mathutil        # restrict to matching files
    make order                         # compilation-unit boundary evidence
    make verify                        # the full expected-zero gate list

`compare-func` prints the two instruction streams side by side with the original's
addresses on the right, then a similarity percentage.

Both `compare-func` and `report` require:

- `code-full/FUN_<addr>.disassembled.txt` for the function -- `make export-asm` generates
  these from the PE using the `/* Function start: 0x... */` annotations in `src/`
  (see docs/EXPORT.md);
- a **linked** `WC1.EXE` and `WC1.map`, which is why `src/stubs.c` provides a `WinMain`
  stub. Nothing can be compared until the project links.

`bin/sweepFlags.py` drives builds across flag combinations and scores each one with
`make report`, so flag calibration uses the same authority as everything else.

## Current state

    make report
    ... 9 of 10 at 100.00%; WinMain is a deliberate stub at 1.22%
