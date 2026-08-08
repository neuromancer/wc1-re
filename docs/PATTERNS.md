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

## /Od idioms in the ix library

### Keep a local struct pointer

The original stores the element address in a stack local and dereferences it, rather than
re-indexing the array at each use.  Under /Od this is plainly visible as a `[EBP-4]` slot:

    MOV EAX,[EBP+8] / SHL EAX,0x5 / ADD EAX,0x5981a8
    MOV [EBP-4],EAX          <-- the local
    MOV EAX,[EBP-4] / MOV EAX,[EAX+0x4]
    MOV ECX,[EBP-4] / SUB EAX,[ECX+0x8]

```c
/* 83% */   v_array[voice].cursor = p;  v_array[voice].start = p;
/* 100% */  IxVoice *v = &v_array[voice];  v->cursor = p;  v->start = p;
```

Applying this took `ix_dspv_set_buffer` from 83.33% to 100.00%, and `get_position` /
`set_position` from ~85% to ~97%.

### The bounds-check shape

Every indexed ix entry point opens with the same guard, and writing it as a single `if`
with `||` reproduces the original's two-test/one-block layout exactly:

```c
if (voice < 0 || voice >= g_nVoiceCount_00598600) {
    ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, <line>);
    ix_log_printf("%d Invalid voice index!", voice);
    exit(-1);
}
```

The `<line>` is the real `__LINE__` from the original source, recovered from the assert
anchors; the module stubs in `src/ix/` carry them.

### `/Oi` is on in ix

`ix_log_printf` inlines `strcpy` as `repne scasb` + `rep movsd`, so the ix module is built
`/Od /Oi`, not plain `/Od`.

### Branch sense follows the source

`ix_log_printf` sat at 65% until the `if`/`else` arms were swapped to match the original's
fall-through:

```c
/* 65% */   if (fmt == 0) strcpy(...); else vsprintf(...);
/* 89% */   if (fmt != 0) vsprintf(...); else strcpy(...);
```

Under /Od the `if` arm is the fall-through and the `else` arm is jumped to, so the order of
the arms in the source is directly observable in the disassembly.

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
