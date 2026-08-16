# Compilation-unit order

MSVC emits functions in source order, and the linker concatenates object files
in command-line order. Address order is therefore the primary evidence for
source ownership and link order.

`src/map` is now the address-sorted inventory of the 1,235 source functions
with a mapped WC2 destination. The 243 unresolved functions are omitted until
their destinations are established.

The physical source and object order still preserves the WC1 reconstruction
while WC2 compilation-unit ownership is recovered. Do not mechanically reorder
the files from low-confidence transfer rows: `make sort` is expected to report
out-of-order WC2 labels during this migration. The sections below record the
historical WC1 boundaries as provenance, not current WC2 placement claims.

## Historical WC1 image layout

```text
0x00401000  game core and Win32 support (mostly C; boundaries provisional)
0x004426A0  ix_log_printf (C++; owning object boundary unproven)
0x00442750  ix audio object sequence (C++; boundaries exact)
0x004492E0  MSVC 4.20 static debug CRT (not reconstructed)
```

The final core function before `ix_log_printf` starts at `0x00442600`. The
first CRT `__FILE__` anchor is at `0x004492E0`; the final `ix` routine occupies
the preceding range.

## Historical WC1 `ix` library order

Live assertion `__FILE__` and `__LINE__` strings fix these object boundaries
and their link order:

| Order | Range | Reconstructed source |
| ---: | --- | --- |
| 1 | `0x00442750`–`0x00443DA5` | `src/ix/streamer.cpp` |
| 2 | `0x00443DA6`–`0x0044490F` | `src/ix/thread.cpp` |
| 3 | `0x00444910`–`0x004451B4` | `src/ix/dsp.cpp` |
| 4 | `0x004451B5`–`0x00445F5F` | `src/ix/dsps.cpp` |
| 5 | `0x00445F60`–`0x004467C4` | `src/ix/mixer.cpp` |
| 6 | `0x004467C5`–`0x004471FF` | `src/ix/dspv.cpp` |
| 7 | `0x00447200`–`0x00447CD7` | `src/ix/system.cpp` |
| 8 | `0x00447CD8`–`0x0044879B` | `src/ix/sound.cpp` |
| 9 | `0x0044879C`–`0x004492DF` | `src/ix/sample.cpp` |

`src/ix/ixlog.cpp` contains `ix_log_printf` at `0x004426A0`. It is linked
immediately before the exact sequence above, but its original object extent is
not yet established.

## Historical WC1 game-core working split

The core is grouped into short subsystem-named files following address order.
The windows below are placement guides, not claims that every edge is an
original object boundary. A nested source unit can be proven even when its
containing file boundary remains provisional.

| File | Working address window | Main evidence |
| --- | --- | --- |
| `src/winmain.c` | `0x401000`–`0x402DFF` | Win32 entry, window, message loop, shutdown |
| `src/sysinput.c` | `0x402E00`–`0x4030FF` | Win32 input and clock wrappers |
| `src/cdrom.c` | `0x403100`–`0x4034FF` | CD location and disc prompts |
| `src/mono.c` | `0x403500`–`0x403E4F` | MONODEBG and adjacent host routines |
| `src/auto.c` | `0x403E50`–`0x40460F` | Exact Mac `auto` symbol run |
| `src/cmpgn.c` | `0x404610`–`0x40609F` | Campaign/briefing Mac symbols |
| `src/brains.c` | `0x4060A0`–`0x40CFFF` | Mac `fl`/`brain` symbols and recovered `BRAINS.C` |
| `src/nav.c` | `0x40D000`–`0x40FFFF` | Nav drawing and location strings |
| `src/spc.c` | `0x410000`–`0x412FFF` | Space simulation and Mac `spc` symbols |
| `src/cockpt.c` | `0x413000`–`0x417FFF` | Mac `cockpt` symbols |
| `src/geom.c` | `0x418000`–`0x41AD4F` | Geometry and modal panel sequence |
| `src/barracks.c` | `0x41ADA0`–`0x41C75F` | Save/load and barracks sequence |
| `src/debug.cpp` | `0x41C760`–`0x41D0BF` | Proven C++ debug-console object |
| `src/mathutil.c` | `0x41D000`–`0x41D24F` | Integer min/max cluster |
| `src/disk.c` | `0x41D250`–`0x41EFFF` | Disk files and packet retry paths |
| `src/ship.c` | `0x41F000`–`0x420FFF` | Ship damage, explosions, and weapons |
| `src/logic.c` | `0x421000`–`0x424FFF` | Mac `logic` unit and adjacent mission logic |
| `src/pilot.cpp` | `0x425000`–`0x426FFF` | Pilot/TrainSim flow and C++ console owner |
| `src/system.c` | `0x427000`–`0x4274DF` | Process-level reporting and exit |
| `src/main.c` | `0x4274E0`–`0x427FFF` | Recovered WINGLEADER main module |
| `src/hudmsg.c` | `0x428000`–`0x42AFFF` | HUD messages plus Mac `targ` and `select` units |
| `src/pload.c` | `0x42B000`–`0x42B3FF` | Proven `Library\Source\Pload.c` path |
| `src/sound.c` | `0x42B400`–`0x42CFFF` | Wave playback, settings, and `INSTALL.DAT` |
| `src/music.c` | `0x42D000`–`0x42EFFF` | Self-naming music diagnostics |
| `src/screen.c` | `0x42F000`–`0x431FFF` | Screen scopes, prompts, and comm menus |
| `src/dib.c` | `0x432000`–`0x43390F` | Proven DirectDraw `DIB*` unit |
| `src/text.c` | `0x433910`–`0x433ABF` | Exact Mac `show_info_disp` symbol |
| `src/smart.c` | `0x433AC0`–`0x434CCF` | Proven Mac `smart` symbol run |
| `src/mathfp.c` | `0x434CD0`–`0x4353FF` | Random and floating-point helpers |
| `src/strdos.c` | `0x435400`–`0x4355EF` | DOS-width string/memory shims and setup calls |
| `src/eventmgr.c` | `0x4355F0`–`0x436FFF` | Event queue, pointer, keyboard, and timers |
| `src/screens.c` | `0x437000`–`0x43F5FF` | Full-screen scenes and raster support |
| `src/killbrd.c` | `0x43F600`–`0x440BFF` | Kill board, conversations, save flags |
| `src/gr.c` | `0x440C00`–`0x44269F` | Raster primitives, effects, and game audio wrappers |

`src/globals.c` is temporary storage for globals whose original owner or
declaration order is not yet proven. It is not an original compilation unit.

## Strong boundary anchors

- `src/pload.c` identifies itself through the string
  `Library\Source\Pload.c PacketLoad`.
- `src/dib.c` and `src/music.c` identify their routines in diagnostics.
- The debug overlay's constructor, destructor, ECX methods, and allocation
  sites prove `src/debug.cpp` as a C++ unit.
- Macintosh symbols establish the nested `auto`, `fl`, `brain`, `logic`,
  `targ`, `select`, and `smart` runs after their Win32 bodies are checked.
- With string pooling disabled, each object's string literals form a local
  band. Code/string address pairs are useful boundary evidence where enough
  literals survive.

## Global ownership

Globals referenced by several units are declared in `include/globals.h`.
Compilation-unit-private declarations stay in their owner. Definitions move
out of `src/globals.c` only when both owner and declaration order are supported
by the original image.

`bin/collectGlobalDefinitions.py` produces `out/globals-audit.c` so verification
can inspect distributed definitions as one manifest. Use `make globals-data`
before and after moving a global band.

## Recovering another boundary

1. Run `make order` and inspect adjacent entries in `src/map`.
2. Compare callers, private globals, strings, and cross-release symbol order.
3. Do not promote a boundary based on subsystem similarity alone.
4. Update this file and `SRCS_ORDERED_CORE` together once the evidence is
   sufficient.

A wrong object boundary shifts every later rebuilt address, so uncertain edges
remain explicitly provisional.
