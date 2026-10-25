# Compiler contract

Reference target: `WC1.EXE`, MD5 `b20a68b7e45f837e59f7e31bab2e2020`, built
1996-09-24 with image base `0x00400000`.

The linker stamp, pre-VC5 image layout and startup, and static CRT identify
Microsoft Visual C++ 4.20. The exact executable configuration lives in the
Makefile; change it only when `binary-comp` demonstrates an improvement across
the affected compilation unit.

## Profiles

| Sources | Language and flags |
| --- | --- |
| Game core | C, `/MTd /Og /Oi /Ot /Oy /Ob1 /Gs` |
| `src/ix/` library members | C++, `/MTd /Od /Oi` |
| `src/ix/ixlog.cpp` | C++, core profile |
| `src/sound.c` | Core profile compiled as C++ with `/TP` |
| `src/debug.cpp` | C++, core profile without `/GX` |
| `src/pilot.cpp` | Core profile plus target-specific `/GX` |

Evidence that constrains these settings:

- Debug-heap strings, `_CrtCheckMemory`, `_mtinit`, and `tidtable.c` prove the
  static debug multithreaded CRT (`/MTd`).
- Core functions normally omit EBP and schedule registers; `ix` library
  functions use EBP, stack temporaries, and a shared epilogue.
- The separately linked `ixlog.cpp` matches the core profile. Its variadic
  logger retains EBP but uses short branches, scheduled argument loads and
  `POP EBP`, rather than the library profile's near branches and `LEAVE`.
- Duplicate literals at `0x0046B6E0` and `0x0046B71C` prove string pooling was
  disabled; do not add `/Gf` or deduplicate literals in source.
- Core operand-size prefixes reflect 16-bit DOS-era types. Use `short` where
  the original uses 16-bit operations; this is not a compiler flag.
- The console allocation wrapper proves `pilot.cpp` needs construction cleanup.
  The training-simulator routines following the console wrappers are in
  `trainsim.c` and use the C core profile. Other units must not gain C++
  exception handling.
- `src/sound.c` directly calls `ix` member functions and matches only as C++.
  Its wave and active-sound lists share initialized state with those calls.
  The following launch, installation, and damage-display routines use C.
  Launch's initialized tables follow sound's literal pool, proving that
  sound's C++ unit ends before `DrawLaunchDoorFrame`.
- The global `IxStream` and `IxVoice` arrays require non-aggregate record
  declarations to reproduce VC++ 4.20's empty `$E2`/`$E1` initializer pairs
  and `.CRT$XCU` registrations. Private fields and friend declarations for
  their existing users produce both pairs byte for byte under `/Od /Oi`.
  No explicit constructor, exception handling, or extra compiler flag is needed.

## Backend diagnostics

The installed `C2.EXE` has SHA-256
`674fb9e410481378c6980c3f21914e513128c85001e8337aca73b587b6273ae9`.
Its option table contains `-norecy`, `-nosched` and `-nocombine`; CL forwards
them through `/d2norecy`, `/d2nosched` and `/d2nocombine`. Temporary
`CFLAGS_CORE` overrides allow individual units to be rebuilt for diagnosis.
These controls are not part of the retained compiler profile.

In this backend image, the recycling control word is at `0x0047A98C`,
initially one, and is tested at `0x00422F27`. The scheduling control is at
`0x0047A994`, also initially one, and is tested at `0x00401480`. These
addresses come from the option table and disassembly of the compiler itself.

Binary-comp probes on the 32-byte-frame carrier candidate distinguish two
remaining differences. Disabling scheduling leaves the escape reset after
the first two indexed word stores, in source order, but still doubles the
index with `ADD` instead of the original `LEA`. Disabling recycling also
retains the destructive doubling and changes other registers and scheduling.
Disabling combining leaves the complete carrier comparison unchanged.
The target-list sorter's register exchange likewise survives disabling
recycling. These probes give no reason to change the production flags; the
carrier's index calculation still needs a source-level explanation.

## Hand-written assembly

Three regions were written in assembly, through different tools:

- The LZO decoder (`0x004614C0`) is an `/Od` function around an `__asm` block.
  Its padding (`nop`, `lea ecx,[ecx]`) is what CL 4.20's inline assembler emits
  for `ALIGN 4`, and its forward jumps are near, as inline assembly emits them
  in `/Od` code.
- The raster library (`0x00439C40`–`0x0043F635`, data
  `0x0046E69C`–`0x0047008B`) was assembled. All 98 of its shifts and
  rotates by one use the `D1`/`D0` encodings, which neither CL 4.20 nor its
  inline assembler emits (both write `C1`/`C0` with an immediate 1), and it
  keeps data tables in `.text`, which inline assembly cannot declare. Its frames open with
  MASM's `add esp, -N`, every jump has the shortest encoding, as MASM 6
  sizes them, and both segments start on 4-byte boundaries, the alignment
  `.model flat` gives `_TEXT` and `_DATA`; no function in it is padded.
  `src/raster.asm` is that source, extracted from the retail executable by
  `bin/extractRasterLibrary.py`, which checks that UASM (a MASM-compatible
  assembler, built from the `compilers/uasm` submodule) reproduces every
  byte and relocation. Regenerate it with the script rather than editing it.
- The ix sample mixer (`0x00446F74`–`0x004471FE`) was also assembled.
  Its two `shl dx, 1` instructions use `66 D1 E2`, and its branches have
  MASM's shortest encodings. Three INT3 bytes after the preceding C++
  function align this object to four bytes; one follows its final `RET`
  before the next C++ object. `src/ix/dspmix.asm` preserves this boundary
  and is assembled with UASM. Compiling the same instructions as an `/Od`
  inline-assembly block emitted longer jumps and immediate-one shifts.

`FillRasterBytes` (`0x00439C10`), just before the library, is compiled C: its
body is CL's inline `memset`, and the `mov edi, edi` before and after it is
CL's padding. In optimized code CL ends every function with NOPs up to a
16-byte boundary, so all 1,291 core functions start 16-byte aligned, and the
only INT3 fill LINK adds in the core is the run after the raster library,
which ends unaligned.

Linking uses `/SUBSYSTEM:WINDOWS /ALIGN:4096` without `/ENTRY`, so LINK
supplies `WinMainCRTStartup` itself. The command line lists the core objects
(with `raster.obj` between `screens.obj` and `killbrd.obj`), `ixlog.obj`,
then `kernel32`, `user32`, `gdi32`, `advapi32`, `winmm`, the five
`ix` libraries, `dsound` and `ddraw`; LIBCMTD and OLDNAMES follow as default
libraries. Base relocations remain enabled. `docs/ORDER.md` records the
evidence for each choice.
