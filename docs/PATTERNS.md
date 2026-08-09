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

## Do not trust the operational labels

Three functions scored 0-40% because I implemented what the Ghidra *name* claimed instead of
what the disassembly showed.  `docs/LABELS.md` warns about exactly this; the warning is real:

| Label | What it actually is |
|---|---|
| `ReturnConst3E8000` (0x42FB20) | a bare `JMP 0x4362E0` tail-jump thunk -- Ghidra followed the jump and folded the callee's `return 0x3e8000` into the display |
| `ReturnConst1v3` (0x422130) | `mov eax,0x59b430 / cmp eax,1 / sbb eax,eax / inc eax` -- the `>= 1` boolean idiom applied to an *address*, which is always non-zero, so Ghidra folded it to `return 1` |
| `CallThrough433060` (0x434FD0) | a `__stdcall` forwarder that passes its argument through (`ret 4`), not a no-argument call |

All three reached 100% once written from the disassembly.  **Read the export before writing
the body, every time.**

Tail-jump thunks need `__declspec(naked)` plus inline asm, which is permitted where the
evidence says the original really is a bare jump:

```c
unsigned int Target(void);            /* forward declaration required */
__declspec(naked) void Thunk(void) { __asm { jmp Target } }
```

## The decompiler's *signatures* are as untrustworthy as its names

`exit_squadron` (0x00427370) sat at 63% because Ghidra's decompilation showed

    DoLocalFn5BB0();
    DoLocalFn5BB0();

with no arguments -- Ghidra's DB prototype for 0x00425BB0 is `void(void)`.  The
disassembly pushes an argument at every call site:

    push esi                 ; the message
    call 0x425bb0
    push 0x46a0a0            ; "[SYSTEM]: Exit_squadron\n"
    call 0x425bb0

Writing it with the real one-argument signature took it to 100%.  Same rule as the names:
**the export is the source of truth, the decompilation is a hint.**  When a call in the
decompilation looks argument-less, check the push sequence before believing it.

## Do not infer a stub's address from its name

Operational names encode only the LOW FOUR hex digits: `RunFrameUpdate` is at
`0x00429DD0`, not `0x00409DD0`.  Filling in the high bits by eye produced **25 wrong
`Function start:` annotations** before I noticed.

A wrong annotation is worse than none: `make export-asm` exports whatever original lives
at that address, and `make report` then scores your function against an unrelated one, so
the number looks fine and means nothing.

    make audit-addresses          # check every annotation against the inventory
    bin/auditAddresses.py --fix   # rewrite them to the real addresses

Run it before trusting a report.

## Prose in a comment can re-create an annotation

binary-comp parses a whole block comment as one node: if the text contains `Function start:`
*and* a `0x…` literal anywhere in the block, it binds that address to the **next function
definition** in the file. Writing

```c
/*
 *  main() lives at 0x004274E0 ... no `Function start:` marker here, because
 *  a marker with no function under it scores the following function against main().
 */
```

did exactly what it was warning about: `GetScreenUpdateFlag` (`0x004279D0`) was silently
scored against `main` and dropped to 3.65%. Never put both the phrase and a hex address in
one comment block. The same rule makes a *dangling* annotation dangerous — an annotation with
a TODO and no function under it always steals the next function.

## Two names must never share an address, and one name must never span two

`bin/auditAddresses.py` originally only checked that the annotated address existed. That let
two real bugs through:

- `ForwardSetCursorPos` was annotated `0x00402E80`, which is `SetMousePosition`; the real
  address is `0x004030E0`. Both are byte-identical `SetCursorPos` wrappers (VC++ 4.2 does no
  COMDAT folding), so the wrong one still "compared" at 100% and meant nothing.
- `0x00432680` was named `DIBmakeDIB`, but `DIBmakeDIB` is at `0x004326E0`; `0x00432680` is the
  teardown, `DIBunInstall`.

The audit now cross-checks the annotated address against the name's address in the inventory,
so a name borrowed from the wrong function is reported. It also has to skip comments when
looking for the signature — comments routinely mention other functions by name — while still
handling `/* Function start: 0x... */ /* TODO */ void f(void){}` all on one line.

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
