# Toolchain and compiler-flag derivation

Target: `WC1.EXE`, MD5 `b20a68b7e45f837e59f7e31bab2e2020`, 518,656 bytes.
Built **1996-09-24 16:33 UTC**. Image base `0x00400000`.

## Toolchain: Microsoft Visual C++ 4.20

| Signal | Value | Reading |
|---|---|---|
| `MajorLinkerVersion.Minor` | 4.20 | link.exe shipped with Visual C++ 4.2 (Aug 1996) |
| Rich header | absent | `DanS` only appears from VC++ 6.0's linker |
| Sections | `.text .rdata .data .idata .reloc` | separate `.idata` = pre-VC5 linker default |
| DOS stub | "This program cannot be run in DOS mode." | MS `winstub`, not Borland/Watcom |
| Imports | no `MSVCRT.DLL`; `GetStringTypeW`, `LCMapStringW`, `RtlUnwind`, `TlsAlloc`, `SetHandleCount` | static MS CRT fingerprint |
| CRT `__FILE__` set | `aw_cmp.c`, `aw_env.c`, `aw_loc.c`, `aw_map.c`, `aw_str.c` | 4.x generation (VC5+ split these into `a_*.c` + `w_*.c`) |
| Startup `entry` @ `0x0044E770` | `_amsg_exit(28)` / `_amsg_exit(16)` on heap/thread init failure | VC 5.0+ uses `fast_error_exit()` here |
| Startup | **no** `__set_app_type()` | introduced in VC++ 5.0's `crt0.c` |
| Startup | `_mtinit`, `tidtable.c` | multithreaded CRT |

The generation (MSVC 4.x) is certain. The point release comes from the 4.20 linker stamp,
corroborated by the Sept 1996 build date.

For contrast: the DOS original (`../releases/dos/WC.EXE`) is **Borland Turbo C++ 1.0** with
VROOMM overlays (`Turbo C++ - Copyright 1990 Borland Intl.`, `Runtime overlay error`). So the
Win32 port is a genuine recompile, not a wrapper.

## Flag derivation

Each flag in the Makefile is backed by an observation. Do not change these casually — the
comparison depends on them.

### `/MTd` — static debug multithreaded CRT

The shipped retail executable links **LIBCMTD**. Present in the image:
`dbgheap.c`, `dbgrpt.c`, `dbgdel.cpp`, `_CrtCheckMemory`, `_CrtIsValidHeapPointer`,
`_CrtMemCheckpoint`, `DAMAGE: after %hs block (#%d) at 0x%08X.`, `Detected memory leaks!`,
`client block at 0x%08X, subtype %x, %u bytes long.`, plus `Microsoft Visual C++ Debug Library`.
The multithreaded half is confirmed by `tidtable.c`, `_mtinit`, and the
`CreateThread`/`TlsAlloc`/`InitializeCriticalSection` imports.

### `/Oy` — omit frame pointers

Almost every game function reads its arguments as `[esp+N]` with no `push ebp; mov ebp,esp`
prologue. Typical recovered prologue: `push esi; push edi; mov si, word [esp+0xC]`.

### NO `/Gf` — string pooling off

Two byte-identical `"DIBsetWholePalette   SetEntries"` literals exist, at `0x0046B6E0` and
`0x0046B71C`, emitted from two different call sites. Identical strings were not merged.

### `/GX` — off by default, enabled for the console owner

Most game-core units have no C++ unwind metadata and use no RTTI type descriptors (`.?AV`,
`.?AU`).  Their SEH is the C `_except_handler3` scope-table form; for example, `entry`'s table
is at `0x00463BE0` (`{prev=-1, filter=0x0044E987, handler=0x0044E9A7}`).

The wrapper at `0x00425B00` is the one proven exception.  Its handler thunk at `0x00425B73`
loads the C++ function-info record at `0x00464B48` (magic `0x19930520`) and jumps to
`__CxxFrameHandler` at `0x0044DA70`; the unwind action at `0x00425B7D` passes the pending
allocation to `operator delete`.  MSVC 4.20 emits this sequence from the console `new`
expression only when the owning `pilot.cpp` unit is compiled with `/GX`.  The flag is therefore
target-specific rather than part of `CFLAGS_CORE`.

### Isolated C++ debug-overlay utility

The game logic remains C, but the Win32 developer-console unit at `0x0041C760`–`0x0041D0BF`
is a proven C++ exception: `0x00425B00` allocates a 0x45C-byte object with `operator new`,
passes it in ECX to the constructor at `0x0041C760`, and `0x00425B90` invokes the ECX
destructor at `0x0041C910` before `operator delete`.  The console's other ECX methods and
callee-cleanup arguments use the same member-function ABI.  The class has no vtable or RTTI;
only its owner needs `/GX`, for the constructor-failure cleanup described above.

### Game-side wave unit compiled as C++

The source unit retained as `src/sound.c` is a second, narrower language exception. Its
game-facing functions keep C linkage, but `playWAVE` (`0x0042B4A0`) and
`PlaySnowStaticSound` (`0x0042B680`) load an `IxSample` or `IxSound` into ECX and call the
library's C++ member functions directly. A C compilation cannot emit those implicit-this
calls without non-original glue. Compiling this unit with `/TP` reproduces both functions at
100.00% similarity while retaining the core optimizer flags and keeping `/GX` disabled.

### Optimizer set `/Og /Oi /Ot /Ob1` — starting point, NOT yet verified

Carried over from the sibling MSVC 4.20 project as a plausible baseline. These still need
per-function confirmation; `make report` similarity is the signal. If a whole class of
functions is systematically off, suspect this line before suspecting the source.

### 16-bit operand density — a source property, not a flag

The core was ported from 16-bit DOS C where `int` was 16 bits. 814 developer functions carry a
measurable `66` operand-size-prefix density (median 0.091 across the game core, 0.000 in the
CRT). This is achieved by declaring variables `short`, **not** by a compiler switch. If your
build emits 32-bit ops where the original used 16-bit, the declared type is wrong.

## Link settings

- `/SUBSYSTEM:WINDOWS`, `/ENTRY:WinMainCRTStartup` — subsystem 2, subsystem version 4.0.
- SectionAlignment `0x1000`, FileAlignment `0x200` (MSVC 4.x defaults).
- Base relocations are **present** in this EXE. Later MSVC linkers made `/FIXED` the default
  for executables; keeping `.reloc` matches the mid-90s behaviour.
- Import libraries needed: `ddraw.lib`, `dsound.lib` from a DirectX 2/3-era SDK (MSVC 4.2
  predates them). `DDRAW.DLL` and `DSOUND.DLL` are bound through the import table, not
  `LoadLibrary`.

Full import set: `KERNEL32` (95), `USER32` (29), `GDI32` (9), `ADVAPI32` (4), `WINMM` (6),
`DSOUND` (2: `DirectSoundCreate`, `DirectSoundEnumerateA`), `DDRAW` (1: `DirectDrawCreate`).
DirectDraw interfaces are then used through COM vtables — e.g. `IDirectDraw2` vtbl `+0x58`
(`WaitForVerticalBlank`), `IDirectDrawPalette` `+0x18` (`SetEntries`), `IDirectDrawSurface`
`+0x7C` (`SetPalette`).
