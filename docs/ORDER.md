# Compilation-unit order

Object link order fixes every address in the image, so this file is the backbone of the
reconstruction. MSVC emits functions in source order and the linker concatenates objects in
command-line order, so **address order == link order**.

`src/map` is the address-sorted function list; adjacency in it is evidence of shared module
ownership. Regenerate with `make sort`.

## Layout

```
0x00401000 ┬ game core (C)                        1327 functions   boundaries UNKNOWN
           │   ... many compilation units ...
           │   main module  ~0x00427000-0x0042A800 (see below)
0x00442750 ┼ ix audio library (C++)                124 functions   boundaries EXACT
0x00449100 ┼ MSVC 4.2 static debug CRT (LIBCMTD)   386 functions   not reconstructed
0x00463000 ┴ end of .text
```

The core count includes `FrameTimerCallback` at `0x0042AFB0`, recovered from
the callback pointer passed to `timeSetEvent`; it was not present in the original
Ghidra function inventory.

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

## Game core — provisional file split

The game core is now split into subsystem-named files by **address range**, which is the best
available proxy for link order: address order *is* link order, so a file holding one
contiguous range is at least shaped like a compilation unit, and every function inside it is
address-sorted (`make sort`). This replaces the earlier `leafsN_scattered.c` grouping, which
was by the order things happened to get written and therefore carried no information at all.

**These boundaries are provisional except where the table says PROVEN.** A wrong boundary
costs a file split or merge later; it does not invalidate the per-function comparison, because
that is anchored by the `/* Function start: */` annotation, not by the file.

| File | Range | Fns | Contents | Boundary evidence |
|---|---|---|---|---|
| `src/winmain.c` | `0x401000`–`0x402dff` | 25 | Win32 shell: window creation, message loop, exit paths | CreateMainWindow/MainWindowProc/AbortToDesktop cluster; string band 0x465048-0x465354 |
| `src/sysinput.c` | `0x402e00`–`0x4030ff` | 8 | Mouse, keyboard and clock services over the Win32 API | contiguous run of Win32 input/timing wrappers with no string references |
| `src/cdrom.c` | `0x403100`–`0x4034ff` | 5 | CD-ROM location and disc-swap prompting | LocateStreamsDirOnDisc..PromptInsertCorrectCd; string band 0x46535C-0x4653FC |
| `src/mono.c` | `0x403500`–`0x403e4f` | 20 | Win32 data-file wrappers, scaled interstitial text, canned sequences, and MONODEBG.VXD | MonoDebug_install/MonoDebug_print anchor the final block; the Mac `auto` unit starts at the next function |
| `src/auto.c` | `0x403e50`–`0x40460f` | 5 | Autopilot travel and interstitial flight sequences | Mac `auto` symbols preserve all five functions in exact order; Ghidra shows the next Win32 function is a private palette-file loader |
| `src/cmpgn.c` | `0x404610`–`0x40609f` | 2 | Campaign briefing/mission-packet decoding | Mac segment name `cmpgn`; `LoadBriefingData` and `LoadMissionData` are recovered at `0x00405910` and `0x004059b0` |
| `src/brains.c` | `0x4060a0`–`0x40cfff` | 98 | NPC pilot intelligence with adjacent flight and mission helpers | Exact Mac `brain` unit and surviving `BRAINS.C` map `cruise_home`–`FF_missile_intelligence` to `0x409760`–`0x40b66f`; adjacent `fl`/`miss` boundaries remain provisional |
| `src/nav.c` | `0x40d000`–`0x40ffff` | 9 | Nav map, location readouts and the virtual screen | DrawNav* family; string band 0x4687AC-0x4688F4 |
| `src/spc.c` | `0x410000`–`0x412fff` | 19 | Space-object simulation with preceding Win32 input helpers | Mac `spc` run maps `rotate_eye_to_goal`–`object_intelligence` to `0x410a30`–`0x4139ff`; outer split remains provisional |
| `src/cockpt.c` | `0x413000`–`0x417fff` | 92 | Cockpit HUD: weapon, damage, target, objective, and message displays | Mac `cockpt` symbols identify the surviving functions throughout this range; port-specific split helpers remain interleaved |
| `src/geom.c` | `0x418000`–`0x41ad4f` | 77 | Geometry, transforms, and modal text-panel drawing | Geometry helper run followed by the contiguous modal-panel block; save/load starts at 0x41ada0 |
| `src/debug.cpp` | `0x41c760`–`0x41d0bf` | 15 | Win32 developer overlay console, keyboard hook, worker and display modes | PROVEN: constructor/destructor new/delete call sites, ECX member ABI, and the following `MinShort` boundary |
| `src/mathutil.c` | `0x41d000`–`0x41d24f` | 3 | Integer min/max used across the game core | MinShort/MaxShort pair, 94 call sites, no other content in the gap |
| `src/disk.c` | `0x41d250`–`0x41efff` | 6 | Disk data files and packet fetching with retry | OpenDiskDataFile/FetchDiskPacketRetrying/PromptInsertNumberedDisk |
| `src/ship.c` | `0x41f000`–`0x420fff` | 13 | Ship damage, explosions, and weapon logic | Ordered Mac `ship` symbols map across the larger `0x41dee0`–`0x42193f` Win32 run; this file currently owns its central tranche |
| `src/logic.c` | `0x421000`–`0x424fff` | 118 | Ship mission logic with adjacent Win32 ship and game-mode routines | Exact Mac `logic` unit at `0x422010`–`0x423cdf`; surrounding boundaries remain provisional |
| `src/pilot.cpp` | `0x425000`–`0x426fff` | 17 | Pilot name entry, TrainSim high scores/opponent selection, inter-scene transitions, and the debug-console owner | EnterPilotNameAndCallsign/ShowTrainSimHighScores/SelectTrainSimMission; new/delete console wrapper at 0x425B00; string band 0x469D74-0x469F98 |
| `src/system.c` | `0x427000`–`0x4274df` | 2 | Process-level services: memory reporting and exit | exit_squadron/ShowMemoryStatusDebug; string band 0x46A064-0x46A10C |
| `src/main.c` | `0x4274e0`–`0x427fff` | 6 | WINGLEADER main module | main() at 0x004274E0, confirmed against the leaked DOS source screenshot |
| `src/hudmsg.c` | `0x428000`–`0x42afff` | 24 | On-screen messages, TrainSim menu input, debug keys, targeting/warp cleanup, and weapon selection | Exact nested Mac `targ` and `select` units at `0x42a8f0`–`0x42af9f`; enclosing split remains provisional |
| `src/pload.c` | `0x42b000`–`0x42b3ff` | 9 | Packet loader and game-side sound-cache lists | PROVEN: PacketLoad prints "Library\\Source\\Pload.c PacketLoad" |
| `src/sound.c` | `0x42b400`–`0x42cfff` | 10 | Wave playback, volume settings and INSTALL.DAT | playWAVE/PlaySfxWaveByIndex/LoadInstallDat; string band 0x46A46C-0x46A710 |
| `src/music.c` | `0x42d000`–`0x42efff` | 28 | Music state machine, streaming music script, and viewport/sprite helpers | PROVEN by the names the routines print: StopMusic, FadeMusic, SetMusicOn, ... |
| `src/screen.c` | `0x42f000`–`0x431fff` | 28 | Screen scopes, prompts and the comm menu | PushMemoryStackFrame/ShowChoosePrompt/ShowEnemyTargetSelectMenu |
| `src/dib.c` | `0x432000`–`0x43390f` | 23 | DirectDraw back end | PROVEN: named DirectDraw routines followed by DirectDrawResultToText and its generated switch tables |
| `src/text.c` | `0x433910`–`0x433abf` | 1 | Cockpit information display | Exact Mac `show_info_disp` name; the following `smart` unit starts at `0x433ac0` |
| `src/smart.c` | `0x433ac0`–`0x434ccf` | 24 | Collision avoidance, formation flight, stress, and maneuver selection | PROVEN: the Mac `smart` symbol run maps across this block and ends at `chase_speed`; `RandomBelow` starts the following random unit at `0x434cd0` |
| `src/mathfp.c` | `0x434cd0`–`0x4353ff` | 22 | Floating-point helpers and the random-number generator | Starts at `RandomBelow`, immediately after the Mac `smart` unit's final `chase_speed` symbol |
| `src/strdos.c` | `0x435400`–`0x4355ff` | 13 | 16-bit DOS C string and memory shims | all __stdcall with short-width arguments, each forwarding to one CRT routine |
| `src/eventmgr.c` | `0x4355f0`–`0x436fff` | 53 | Event manager, mouse pointer, keyboard state, and frame timer | FM Towns preserves `source\\eventmgr.c`; Win32 keeps the same queue and input-state layout |
| `src/screens.c` | `0x437000`–`0x43cfff` | 4 | Full-screen presentation screens and raster primitives | ShowGetReady/Victory/GameOver plus the adjacent raster support block |
| `src/killbrd.c` | `0x43d000`–`0x440bff` | 7 | Kill board, conversation scenes and save-slot flags | ShowTigersClawKillBoard/RunConversationScene; string band 0x4705DC-0x470668 |
| `src/gr.c` | `0x440c00`–`0x44274f` | 14 | Rasteriser primitives and screen-space effects | PROVEN by name: shadow_draw, fizzle_fade, snow_viewport |

Four whole-file boundaries and four nested source units are proven rather than guessed:

- **`dib.c`** — the back-end routines identify themselves through `DIBerror`; DirectDrawResultToText occupies the unit tail.
- **`pload.c`** — `PacketLoad` prints `"Library\Source\Pload.c PacketLoad …"`, the only
  game-core source path anywhere in the image. It also fixes the original directory layout:
  the game core lived in `Library\Source\` and used short DOS 8.3 filenames, which is why the
  files above are named the way they are.
- **`music.c`** — the routines name themselves (`StopMusic`, `FadeMusic`, `SetMusicOn`, …).
- **`smart.c`** — the Super Wing Commander Mac symbols preserve the source-unit name and
  ordered function run. Win32 expands two collision/formation operations into private
  helpers and swaps the local placement of `control_speed`/`chase_location`, but the
  `0x00433AC0` start and `chase_speed`/`RandomBelow` end boundary remain unambiguous.
- **Mac `brain` unit** — CODE 2 preserves 45 consecutive symbols from `cruise_home` through
  `FF_missile_intelligence`. Checked Win32 bodies form the same run at
  `0x00409760`–`0x0040B66F`; Win32 adds the `BRAINS.C`-named
  `heat_seeking_missile_intelligence` between `mine_intelligence` and the final Mac symbol.
- **Mac `targ` unit** — CODE 4 ends with the consecutive symbols `find_objective`,
  `arrive_from_warp`, `unwarp`, `warp`, `drop_player_mine`, `personality_killed`, and
  `clean_up_cockpit`. Their checked Win32 bodies form the same uninterrupted run at
  `0x0042A8F0`–`0x0042ACFF`; `0x0042AD00` begins the separate weapon-selection run.
- **Mac `select` unit** — CODE 15 preserves the consecutive symbols `find_next_gun`,
  `select_guns`, `select_new_gun`, and `select_new_release_weapon`. Their checked Win32
  bodies form the same uninterrupted run at `0x0042AD00`–`0x0042AF9F`; the timer/debug
  routine at `0x0042AFA0` starts the following unit.
- **Mac `logic` unit** — CODE 5 preserves the semantic run from `ace_status` through
  `mine_available`. Checked Win32 bodies occupy `0x00422010`–`0x00423CDF`; Win32 inserts
  several private split helpers and swaps the local placement of `are_alive`/`trim_goals`,
  while `0x00423CE0` begins unrelated resource/UI loading code.

### String-literal locality: a boundary tool that is not yet conclusive

MSVC emits each object file's string literals as one contiguous block and the linker
concatenates those blocks in link order, so with `/Gf` **off** — which it is here — a
function's string references must lie inside its own object's block. That makes
(code address, string address) pairs a boundary detector: the string band should advance
monotonically with code address, and a band that jumps backwards means the two functions share
an object.

196 game-core functions reference strings, enough to anchor the clusters used in the table
above but not enough to fix every boundary. Reproduce with `bin/nameOracle.py --top` for the
call-site side; the band extraction is the same PE walk.

## Game core — what remains unknown

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
| `0x00406000`–`0x004075D0` | Ship AI: 42 behaviour handlers + `perform_maneuver` dispatcher |
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
