# Matching patterns

The original disassembly and extracted strings are authoritative. Decompiled
code, Ghidra signatures, operational labels, and source from another release
are hints that must be checked against the instructions.

Use `binary-comp` for every similarity decision. Do not substitute a separate
scorer.

## Compilation profiles

The two main code groups use visibly different compiler settings:

- The game core is optimized with the `CFLAGS_CORE` profile. Functions commonly
  omit frame pointers, schedule registers across expressions, and duplicate
  short epilogues.
- The `ix` library uses `/Od /Oi`. Its functions preserve EBP, EBX, ESI, and
  EDI, expose stack locals directly, and share one epilogue.

Do not tune a single function by changing a compilation-unit profile unless
the surrounding original functions prove that the unit used different flags.
The evidence for the current flags is in [COMPILER.md](COMPILER.md).

## Types and calling conventions

The core descends from 16-bit DOS C. A large amount of state and many
intermediate values are `short`, even though the Win32 compiler's `int` is 32
bits. A 32-bit operation where the original uses an operand-size prefix usually
means the declaration is too wide.

Check each function's terminating instruction and every call site:

- bare `RET` normally indicates `__cdecl`;
- `RET n` indicates callee cleanup such as `__stdcall`;
- C++ `ix` methods receive the implicit object in ECX; and
- loads, sign extension, and how the caller consumes EAX determine the return
  width more reliably than the decompiler.

Do not copy a Ghidra prototype without checking the pushes before each call.
Several recovered calls were initially wrong because the decompiler omitted
arguments after learning an incorrect callee signature.

`BOOLEAN` is supplied by the Win32 headers. Do not redeclare it from the DOS
source with a different underlying type.

## Source shape affects generated code

Equivalent C can produce different instruction order. Match the original
control-flow shape rather than merely its result.

### Returns and branches

An early return may produce a full-register move where assigning to a local and
returning once produces a 16-bit move. Likewise, swapping `if` and `else` arms
changes fall-through direction, especially in `/Od` `ix` code. Follow the
original jumps and epilogue placement.

### Locals in `/Od` code

When the original stores an array element address in a stack slot, declare the
corresponding local pointer and reuse it. Re-indexing the array at every access
is behaviorally equivalent but does not reproduce the unoptimized instruction
stream. Preserve local declaration order when stack layout is visible.

### Strings and library calls

Identical string literals were not pooled. Write the literal at each original
use site instead of introducing an alias. Use the matching standard function
when the binary calls or inlines `strcpy`, `memcpy`, or another CRT routine.

### High-level data access

Use typed structure fields and arrays. If a field's purpose is unknown, retain
an offset-based field name until evidence supports a better one. Raw pointer
arithmetic often hides a wrong width and makes comparison harder.

## Analysis traps

### Operational labels are not intent

A label such as `ReturnConst...`, `CallThrough...`, or `ScanTbl...` describes
what an earlier analysis observed. It may reflect decompiler folding or an
incorrect signature. Read the exported instructions before writing the body,
then apply [the naming policy](LABELS.md).

### Function annotations must be exact

Operational labels may contain only the low four address digits. Never infer a
full address from the name. A wrong `/* Function start: 0x... */` annotation can
compare a function against an unrelated original and still produce a plausible
score.

Do not place both the phrase `Function start:` and a hexadecimal address in a
prose comment. The exporter can bind that comment to the next definition.
Every real annotation must immediately precede its function.

Run:

```sh
make audit-addresses
```

### Compiler-generated glue

Do not hand-write jump thunks, import stubs, vtable dispatch, exception tables,
or other compiler/linker glue. Express a real forwarding operation in ordinary
C and let the compiler emit the tail jump. `make audit-compiler-glue` rejects
manual replicas.

Do not add C++ exception handling or spell out `__thiscall`. Inline assembly is
allowed only when the original routine itself is strongly evidenced as
hand-written assembly.

## Comparison workflow

```sh
make compare-func FUNC=FunctionName
make report FILTER=source_name
make verify
```

`compare-func` is the normal edit loop. Stop once the function is at least 90%
similar unless a clear, evidence-backed improvement remains. `make verify`
runs the repository-wide expected-zero gates. Export requirements are listed in
[EXPORT.md](EXPORT.md).

Run the reconstructed Win32 executable through DREAMM, not Wine. DREAMM
provides the Windows 95 DirectDraw and DirectSound behavior being reconstructed;
Wine substitutes different host implementations. The default DREAMM settings
use an 8-bit palettized display because the original DirectDraw palette path
depends on it.
