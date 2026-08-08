# Compilation-unit order

Object link order fixes every address in the image, so this file is the backbone of the
reconstruction. MSVC emits functions in source order and the linker concatenates objects in
command-line order, so **address order == link order**.

`src/map` is the address-sorted function list; adjacency in it is evidence of shared module
ownership. Regenerate with `make sort`.

## Layout

```
0x00401000 ┬ game core (C)                        1326 functions   boundaries UNKNOWN
           │   ... many compilation units ...
           │   main module  ~0x00427000-0x0042A800 (see below)
0x00442750 ┼ ix audio library (C++)                124 functions   boundaries EXACT
0x00449100 ┼ MSVC 4.2 static debug CRT (LIBCMTD)   386 functions   not reconstructed
0x00463000 ┴ end of .text
```

## ix library — exact

Recovered from live assert `__FILE__` anchors in the shipped debug build. Each object file
occupies one contiguous range, and they link in this order:

| Order | Range | Source |
|---|---|---|
| 1 | `0x00442750`–`0x00443DA5` | `ix/Src/Streamer/streamer.cpp` |
| 2 | `0x00443DA6`–`0x0044490F` | `ix/Src/Streamer/thread.cpp` |
| 3 | `0x00444910`–`0x004451B4` | `ix/win95/dsp/dsp.cpp` |
| 4 | `0x004451B5`–`0x00445F5F` | `ix/win95/dsp/dsps.cpp` |
| 5 | `0x00445F60`–`0x004467C4` | `ix/win95/dsp/mixer.cpp` |
| 6 | `0x004467C5`–`0x004471FF` | `ix/win95/dsp/dspv.cpp` |
| 7 | `0x00447200`–`0x00447CD7` | `ix/src/sound/system.cpp` |
| 8 | `0x00447CD8`–`0x0044879B` | `ix/src/sound/sound.cpp` |
| 9 | `0x0044879C`–`0x004492DF` | `ix/src/sound/sample.cpp` |

Per-module source line numbers are recorded in the generated stubs in `src/ix/`, so functions
within each file can be ordered against the original source layout too. Example, `dsps.cpp`:
line 26 `ix_dsps_alloc`, 62 `ix_dsps_free`, 77 `ix_dsps_prepare`, 87 `ix_dsps_play`,
99 `ix_dsps_stop`, 109 `set_volume`, 122 `get_flags`, 128 `get_buffer_free`, 169 `lock`,
188 `unlock`.

## Game core — mostly unknown

**No source-file anchors exist below `0x00442750`**, so the module map that worked for `ix`
cannot be reproduced here. Boundaries must be recovered incrementally with `make order`.

### What IS known

**The main module** — the leaked source screenshot identifies it as
*"MAIN GAME C SOURCE MODULE (Main loop etc.)"* from the WINGLEADER tree.

- `main()` is at **`0x004274E0`** (called once from `WinMain` at `0x00401E30`).
- Module code sits roughly **`0x00427000`–`0x0042A800`**, containing: `main`, argv parsing,
  `exit_squadron` (`0x00427370`), `ShowMemoryStatusDebug` (`0x004273C0`),
  `AllocateSpaceBuffer` (`0x00427A00`), `ShowVolumeAdjustMenu` (`0x00427F20`),
  `ReadPlayerInputAndDispatch` (`0x004285D0`), `ShowOnScreenMessage` (`0x00428FA0`),
  `ShowGamePausedBanner`, `ShowVersionBanner`, `SetMessageDisplaySpeed`,
  `ReportFramesSkipped`, `HandleDebugCheatKeys` (`0x00429160`), the score panel.
- Module statics band around **`0x00469E00`–`0x0046A400`** — including `_FrameSkip` at
  `0x00469FB8`. This band is touched by 105 functions across the whole image, which is
  expected: a main module's globals are the most widely shared state in the program.

**Named subsystem clusters** (addresses approximate, from string/API evidence):

| Range | Subsystem |
|---|---|
| `0x00401E30`–`0x00403E30` | Win32 shell: `WinMain`, window creation, wndproc, message pump, CD/disc location, `MonoDebug_*`, joystick, keyboard |
| `0x00406000`–`0x004075D0` | Ship AI: 42 behaviour handlers + `RunShipAiBehaviorTick` dispatcher |
| `0x0041C760`–`0x0041CF00` | Debug overlay console (GDI text, keyboard hook, worker thread) |
| `0x0041ADA0`–`0x0041BFE0` | Save/load (`SAVEGAME.WLD`) and confirmation prompts |
| `0x0042B810`–`0x0042B930` | Registry settings (`Software\Origin Systems\WC: Kilrathi Saga`) |
| `0x0042E350`–`0x0042F000` | Music/SFX layer, music script commands |
| `0x00432140`–`0x004331F0` | DirectDraw video layer: the `DIB*` family (original names) |
| `0x00434D10`–`0x00435600` | Math/util: `MinShort`, `MaxShort`, `RandomInRange`, `RandomBelowOrEqual`, `__ftol` wrappers |
| `0x00440500`–`0x00442600` | Rasteriser and screen effects: `shadow_draw`, `fizzle_fade`, `snow_viewport`, triangle/line dispatchers |

### `globals.c` placement is provisional

`src/globals.c` is a synthetic collection module — the original had its globals spread across
the real compilation units, not gathered in one file. It carries no code, so its position only
affects **data** layout, but data layout is compared too.

It currently links after `main.c` and before the `ix` modules, which matches the broad
observation that the game's initialised statics (`0x00465000`–`0x00475C00`) sit below the `ix`
statics (`0x00597CD0`, `0x00598138`, `0x005981A8`). Note that a lot of *game* state also lives
above `0x00475C00` (e.g. the ship arrays at `0x0059C420`, `0x0059D710`), so game and `ix` data
are interleaved in that upper region and a single placement cannot satisfy both.

Use `make globals-data` to see the real deltas before moving it.

### Recovering the rest

1. `make order` — binary-comp's boundary analysis over original addresses.
2. Cross-check with `src/map` adjacency: a run of consecutive addresses that all reference the
   same static band is very likely one object file.
3. Record each confirmed boundary here **and** in `SRCS_ORDERED_CORE` in the Makefile.

Do not guess boundaries. A wrong boundary shifts every later address and invalidates the whole
comparison downstream of it.
