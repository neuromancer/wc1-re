# Super Wing Commander Mac demo

The GUI recognizes the SWC Mac demo and starts an experimental Enyo 1 flight
with five ships, four objectives, and the original 320x240 cockpit and sprites.
SDL2 handles input and presentation; WC1 supplies mission state, ship setup,
flight dynamics, movement, projection, ship-view selection, and navigation.
Combat, enemy AI, nav-sphere transitions, mission completion, the full HUD,
audio, and movies are not connected yet. Ship parameters still come from WC1.

`make modern-swc` builds the asset viewer. It loads the demo's CMF files,
decodes sprites, reads the resource-fork palette, and displays the cockpit and
13 ship shape sets at 320x240. Left/right select a ship view, up/down select a
ship, C toggles the cockpit, and Esc exits.

`src/swc/` contains the resource readers. `src/sdl/swc_mission.c` translates
Mac mission records into WC1 state through the existing `LoadMissionData`
entry point in SDL builds. The CMF reader and mission adapter are linked into
`wc1-modern`; the original Win32 reference build does not include them.
`src/sdl/swc_demo.c` remains a separate presentation/inspection entry point.
`src/sdl/swc_flight.c` hosts the experimental flight using the shared core.
The viewer needs only the host C compiler and SDL2. Mission tests also link
the existing native core and need its normal build dependencies, including LZO.

## Run

Build the GUI with `make modern-gui`, run `out-modern/wc1-modern-gui`, select the
extracted **SuperWing DEMO** directory, and click **Start Enyo 1**. To preselect
the directory or start flight directly:

```sh
out-modern/wc1-modern-gui --gui --swc-demo "data/swc-demo/SuperWing DEMO"
out-modern/wc1-modern --swc-demo "data/swc-demo/SuperWing DEMO"
```

Arrows steer, Q/E roll, +/- change speed, N cycles objectives, M toggles the nav
map, C toggles the cockpit, and Esc exits. The window title shows the selected
objective and speed. The GUI disables graphics and joystick features that the
SWC flight host does not support yet.

Point `SWC_DATA_DIR` at the extracted **SuperWing DEMO** directory containing
`CMFs/`. For example, from the repository root on macOS:

```sh
unar -o data/swc-demo ../releases/mac/SuperWing_DEMO.sit
make run-modern-swc SWC_DATA_DIR="data/swc-demo/SuperWing DEMO"
```

The viewer and flight host first try `SuperWingCommanderDemo.rsrc` inside that directory,
then the native resource fork of `Super Wing Commander Demo`. The palette is
`clut` ID 251, named `pcshipv00`. A stripped PEF executable does not contain it.
For the GUI or flight host, copy an extracted **raw** resource fork to
`SuperWingCommanderDemo.rsrc` inside the demo directory. The asset viewer also
accepts an explicit path:

```sh
out-modern/swc-demo "/path/to/SuperWing DEMO" \
    --rsrc ../releases/mac/extracted/SuperWingCommanderDemo.rsrc
```

AppleDouble sidecars (`._...`, or `.rsrc` from `unar -forks visible`) are containers,
not raw resource forks, and are not currently supported. On macOS, `unar`'s
default fork-preserving extraction works directly. Paths and filenames currently
use the spelling in the demo archive. Game assets stay outside version control.

The asset viewer's `--check` presents one frame and exits. `--snapshot FILE.bmp` also saves that
320x240 frame and exits. The check runs with SDL's dummy video driver:

```sh
make modern-test-swc
make modern-test-swc-mission
make modern-test-swc-data SWC_DATA_DIR="/path/to/SuperWing DEMO"
SDL_VIDEODRIVER=dummy out-modern/swc-demo "/path/to/SuperWing DEMO" \
    --rsrc /path/to/SuperWingCommanderDemo.rsrc \
    --snapshot out-modern/swc-preview.bmp
```

The data-free tests cover both RLE commands including 128-byte runs, truncation,
output overrun, endianness, frame padding, signed offsets, empty frames, CMF
table bounds, stale handles, and resource-map/color-table bounds. The corpus
check opens every supplied CMF and decodes every compressed chunk as a sprite
set, a property verified for this demo rather than assumed by the resource API.
The known demo contains 23 CMFs, 1,011 chunks, 267 compressed sprite sets,
3,446 frames, and 18,315,140 decoded chunk bytes. Development builds use ASan
and UBSan, like the existing SDL2 target.

The mission fixture test also runs as part of `make modern-test`. It covers
big-endian records, mission-versus-series indexing, narrow WC1 field limits,
objective terminators, navigation references, formation cycles, and failure
without changing the previously loaded mission. It exercises WC1's objective
builder and navigation with synthetic data, so no game assets are needed.
`modern-test-swc-data` additionally calls `LoadMissionData(1, 0)` against the
real demo and checks the player's position, speed, and pilot, four objectives,
objective cycling, and map scaling. It then launches the real native executable
for 120 input/dynamics/render ticks. SWC test targets stop on UBSan errors.
`make modern-test-launcher` covers recognition, invalid installations, and
dispatch to SWC instead of the WC1 startup. A native GUI check uses
`wc1-modern-gui --gui --swc-demo DIRECTORY --check`; this invokes the real
launch callback automatically after the window is shown.

## Recovered format contracts

These contracts come from the original 68000 instructions and actual demo data.
Ghidra's C output alone is insufficient: classic Mac A-line traps truncate some
decompilations, and `GetFramePtr` returns its pointer in A0.

| Data | Contract | Original evidence |
| --- | --- | --- |
| CMF header | `CMF1`; LE32 TOC offset/length at 12/16 | CODE_09 `CMOpenFile` +0x26ae |
| CMF TOC | LE32 relative type-table offset; type/count groups; 16-byte entries | CODE_09 `CMReformatTOC` +0x2fac |
| CMF entry | ID, file offset, flags/size, old handle; length mask `0x00ffffff`; compressed bit `0x10000000` | CODE_09 `CMGetChunk` +0x22f6 |
| Compressed chunk | BE32 output length, then byte RLE | CODE_09 `CMDecompressRLE` +0x2b26 |
| RLE | `(control & 127) + 1` bytes; high bit repeats the next byte, otherwise literal copy | CODE_09 +0x2ba4..+0x2bfe |
| Sprite set | BE32 count; each frame has a 14-byte header plus payload padded to a multiple of four | CODE_01 `GetFramePtr` +0x05f4 |
| Frame | BE16 encoding, signed x/y, width/height; BE32 payload length | CODE_01 `GetFramePtr` and `DrawRLEImage` |
| Pixels | Encoding 1 is raw; 2 is byte RLE; palette index 0 is transparent | CODE_01 `DrawRLEImage` +0x17aa |
| Palette | Classic resource-fork `clut`, 16-bit RGB channels reduced to their high byte | Demo resource fork, clut/251 |
| Mission chunks | `Data.CMF`, `MOD0..2` IDs 1..6; index `series * 4 + mission` for IDs 1..5, series alone for ID 6 | CODE_03 `LoadMissionData` +0x16d2 |
| Mission records | 24-byte header; 16 × 88-byte nav points; 16 × 68-byte objectives; 32 × 76-byte ships; two 40-byte auxiliary blocks | CODE_03 `LoadMissionData`, CODE_04 record consumers |

The host ignores persisted Mac handles and cache state. Compressed allocations
are limited to 64 MiB and impossible compression ratios are rejected. Frame
payloads borrow the decoded set; owned buffers and CMFs have explicit lifetimes.
The viewer centers extracted ship images for inspection. It does not claim to
reproduce flight projection, sprite hotspots, scaling, or original draw order.

## WC1 reuse and original-code evidence

The implementation uses WC1 functions wherever their behavior is suitable.
SWC adapters decode resource formats and translate record layouts at the input
boundary. They do not introduce separate mission structures or duplicate the
shared gameplay functions. Multi-byte mission fields are read explicitly as
big endian, and values that do not fit the WC1 fields or supported tables are
rejected before publishing any state. Canned-sequence mission records are also
rejected because a persisted Mac pointer cannot be used as a host pointer.

| Existing WC1 code | Current SWC use | Remaining qualification |
| --- | --- | --- |
| `LoadMissionData` in `src/cmpgn.c` | Detects `CMFs/Data.CMF` and invokes the SDL adapter | The direct SDL flight host also calls this adapter; campaign scenes are not connected |
| `set_sphere_point` in `src/brains.c` | Adds the existing nav position and ship-relative position | Validated with synthetic data and Enyo 1 |
| `Set_up_ship_info` / `init_intelligence_data` | Sets the Enyo player's position, orientation, speed, pilot, and mission state | Other mission modes and SWC AI differences need review |
| `Build_objective_list` | Builds Enyo's three nav objectives and carrier-return objective | Other SWC objective behavior is not established |
| `cycle_next_objective`, `nav_getxy`, `SetScale` | Cycles and projects the shared objective state | This uses WC1 map units/layout; SWC's `nav_getxy` uses wider coordinates and different scaling |
| `players_flight_dynamics`, `rotate_object`, `accelerate_and_move_object` | Steers and moves the five entry-nav ships | WC1 ship parameters; NPC AI and combat are not active |
| `generate_stars`, `update_star_field`, `transform_objects_to_your_view`, `get_right_shape` | Projects the scene and chooses among 37 decoded ship views | SWC hotspots, scale, and exact presentation still need reconstruction |

The SDL version of `nav_getxy` writes coordinates through byte copies because
the existing packed objective records can place them at odd addresses. Its
calculations are unchanged; the Win32 reference implementation is preserved.
During SWC flight, `get_right_shape` returns the selected frame to the SDL
texture cache instead of fetching WC1 capital-ship packets.

SWC binary similarity measurement is no longer part of this work: the original
Mac compiler/toolchain is unavailable. Validation uses original instructions,
actual data, and functional tests. WC1's reference-build verification remains
in place. Passing these tests is not proof that all SWC gameplay is equivalent.

[`config/swc-provenance.json`](../config/swc-provenance.json) records reference
hashes, CODE-resource offsets, source symbols, and differences in the host
adaptations. It is an evidence inventory, not a guessed tool configuration.
Keep CODE-resource offsets distinct from WC1 virtual addresses and Ghidra's
temporary import base. Ordinary `Function start:` annotations are reserved for
the WC1 executable so its export and verification tools do not mix binaries.

The current resource readers are host adaptations with extra bounds checks and
different memory APIs. `SwcDecodeFrame` only uses `DrawRLEImage` as format
evidence; it is not a reconstruction of that whole drawing routine. SDL window,
resource-fork, allocation, and input plumbing have no one-to-one original game
function mapping. Do not infer code reuse percentages from matching names;
many WC1 names were already derived from this Mac executable's MacsBug strings.

## Next support slices

1. Recover SWC object definitions and adapt them to the shared flight code.
   CODE_08's initialization
   uses 83 records of 204 bytes; WC1 uses 58 records of 135 bytes. SWC also has
   wider numeric fields, 10-byte weapon slots, and 102-byte loadouts. Direct
   copies into WC1 structures would corrupt state. The current adapter rejects
   object IDs outside WC1's table; Enyo 1 currently uses WC1 object definitions.
   Review original instructions before sharing behavior that differs in SWC.
2. Refine sprite hotspots, SWC projection, scaling, and the 320x240 HUD. The
   current SDL renderer centers sprites selected and projected by WC1.
3. Connect nav-sphere transitions, NPC AI, combat, carrier return, and the
   HUD. The demo explicitly disables its simulator, so campaign flight is the
   useful first playable milestone.
4. Add scenes, AIFF speech/effects, music, and the custom `LMov` movie format.
   The `.dcMov` files are not ordinary QuickTime movies; renaming them is not
   a decoder implementation.
