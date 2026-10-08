# Super Wing Commander Mac demo

The GUI recognizes the SWC Mac demo and starts an experimental Enyo 1 flight
with five ships, four objectives, and the original 320x240 cockpit and sprites.
SDL2 handles input and presentation; WC1 supplies mission state, ship setup,
flight dynamics, movement, projection, ship-view selection, navigation, and
player weapons, including projectile collisions, damage, and missile guidance.
The Hornet cockpit now has speed and navigation readouts, fuel/throttle/energy
gauges, player/target shield and armor displays, radar contacts, weapon status,
navigation and hostile-direction markers, warning lamps, and damaged displays.
The original Hornet launch movies play before flight, with embedded audio.
Enemy AI, nav-sphere transitions, mission completion, the remaining HUD,
flight audio, music, and scripted scenes are not connected yet. Ship parameters
still come from WC1.

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
`src/sdl/swc_cockpit.c` presents the Hornet instruments and bitmap text.
`src/swc/movie.c` reads and decodes the Mac movies; `src/sdl/swc_movie.c`
supplies SDL2 presentation, queued audio, and the launch sequence.
The viewer needs only the host C compiler and SDL2. Mission tests also link
the existing native core and need its normal build dependencies, including LZO.
Flight uses SDL2 2.0.18 or later for the VDU polygon rendering.

## Run

Build the GUI with `make modern-gui`, run `out-modern/wc1-modern-gui`, select the
extracted **SuperWing DEMO** directory, and click **Start Enyo 1**. To preselect
the directory or start flight directly:

```sh
out-modern/wc1-modern-gui --gui --swc-demo "data/swc-demo/SuperWing DEMO"
out-modern/wc1-modern --swc-demo "data/swc-demo/SuperWing DEMO"
```

With the archive's `Movies/` directory present, the Hornet launch plays first.
Space, Enter, Esc, or a left click skips the entire sequence on release; P
pauses it. Losing focus pauses video and audio together. Closing the movie
window exits without starting flight. Once in flight, Esc resumes its normal
exit behavior and Space/Enter/mouse buttons resume their weapon bindings.

Flight uses WC1's DOS/Win32 mouse and keyboard controls through SDL2:

| Control | Action |
| --- | --- |
| Mouse movement | Steer using WC1's dead zones and response curve |
| Hold right button and move | Horizontal motion rolls; vertical motion changes throttle |
| Double-click right button and hold | Afterburner; release stops renewing the boost timer |
| Arrows or keypad 2/4/6/8 | Pitch/yaw with WC1's gradual steering response |
| Home/PgUp/End/PgDn or keypad 7/9/1/3 | Diagonal steering |
| Comma/period, Insert/Delete, or keypad 0/decimal | Roll |
| Shift with a steering key | WC1's fast steering response |
| Keypad 5 | Centre steering and the mouse pointer |
| +/- or keypad +/- | Change commanded speed while held |
| Backspace | Set commanded speed to zero; the ship decelerates |
| Tab or keypad * | Afterburner, subject to WC1's fuel and timer checks |
| Space / left mouse button | Fire selected guns; holding repeats while energy and cooldown permit |
| Enter / press both mouse buttons | Release one selected missile; guided weapons require a lock |
| G / W | Cycle gun selection / missile type |
| T | Cycle visible targets using WC1's range ordering and enemy preference |
| N / M / C | Next objective / nav map / cockpit |
| P / Esc | Pause / exit |

The window title shows the selected objective, actual and commanded speed,
and afterburner state. The pointer is confined to the flight window; P releases
it while paused. Losing focus also pauses movement and releases the pointer.
Keyboard steering takes over until the next mouse movement. Releasing the
right-button roll/throttle mode recentres the pointer, as in WC1. Holding both
buttons releases one missile per press; a latched right-button afterburner keeps
its existing behavior and allows left-button guns. Joystick flight is still
unsupported; the GUI disables its options. Q/E are no longer roll bindings.

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

## Launch movies

The Mac demo stores 38 custom `LMov` files under `Movies/`, with `.dcMov`
extensions. These are not ordinary QuickTime movies. The new decoder follows
CODE_12 `LMovieOpen`, `LMovieTask`, and `LMovieDrawFrameMinRect`; the SDL host
replaces QuickDraw blits, Sound Manager double buffers, and Mac event handling.
WC1's `scramble` constructs its launch from sprites, so it cannot play these
files. Shared WC1 flight initialization and gameplay still run after playback.

CODE_03 `scramble` +0x45d0 and its A5-relative strings specify this order:
`ARMOR`, `HALL`, `LAUNCH01`, `LAUNCH02`, `LAUNCH03`, `LAUNCH04.00`,
`LAUNCH06.00`, `LAUNCH07.00`, `LAUNCH08.00`. The `.00` suffix selects the
Hornet. The host checks that all nine files can be opened before playback;
missing/unreadable movies produce a stderr message and leave the existing
direct-flight path available. Invalid movie data reports an error. The finite
`--check` flight path bypasses movies.

| Movie data | Recovered contract |
| --- | --- |
| Header | Big endian; `LMov`, version `0x00010000`; 184 fixed bytes followed by 16 bytes per indexed record |
| Layout | Metadata size at +0x08; record count at +0x0c; integer frame rate at +0x10; audio-present bit at +0x14; height/width at +0x18/+0x1a; initial pixel value at +0x1c |
| Index | Flags, persisted cache pointer, payload length, absolute file offset; flags 2 = video, 1/0x11 = audio |
| Palette | Length at +0xac; 2056-byte Mac color table immediately after the index; 256 colors in table order, ignoring `ColorSpec.value` |
| Audio | Block bytes at +0x9c; 16.16 sample rate at +0xa0; sample bits at +0xa4; valid sample count at +0xa6; audio flags at +0xaa |
| Demo audio | Unsigned 8-bit mono, flags 2, 11128-byte stored blocks, 22254.545... Hz; the sample count trims the last block and excludes padded audio after it |
| Frame commands | Upper three bits select end-row, short/long skip, short/long literal, short/long repeat, or no-op |
| Run lengths | Short: `(command & 31) + 1`; long: `((command & 31) << 8) + nextByte + 1`; skips retain preceding pixels |

The host accepts the demo's 320x240, 256-color layout, checks record and scanline
bounds, and limits loaded movies to 64 MiB. Every video delta is decoded in
order even when presentation must catch up. SDL2 displays each movie's own
palette, queues its embedded PCM, and rounds the original sample rate to an
integer Hz. Playback falls back to silent video if no audio device is available.
The video clock and audio device pause together, and the last frame remains
while longer audio finishes. All movie resources are released on finish, skip,
error, or window close.

The separate `MIDI/ARMOR.MooV` soundtrack selected by the original launch,
palette fades, opening/landing/funeral integration, and the scripted briefing
and conversation scenes remain pending. No external video library is needed
for this slice.

Validation for this pass consists of original-instruction review, offline
inspection of all 38 original containers and 3,521 encoded frame streams,
static source review, and `make modern modern-gui`. This is not a playback
test: no game, GUI, interactive check, or audio device was run. Playback and
audio synchronization still need user review.

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
| `PollKeyboardState`, `process_player_input` | WC1 keyboard priorities, diagonals, gradual steering, reversals, and Shift response | Deliberately uses DOS/Win32 controls; SWC's Mac event/device layer differs |
| `player_input`, `QueueInputEvent`, `WarpMouseTo` | Original mouse response, edge limits, right-button roll/throttle, and recentering | SWC supplies only motion events and the right-button modifier; weapon events are excluded |
| `accelerate`, `celerate`, `your_afterburner`, `fire_afterburner` | Throttle bounds, boost activation, and boost timers | Audio calls stop at the SDL boundary until SWC sound is implemented |
| `players_flight_dynamics`, `rotate_object`, `accelerate_and_move_object` | Steers and moves the entry-nav ships and active weapons | WC1 ship parameters; NPC ship AI is not active |
| `fire_players_lasers`, `fire_fixed_projectile_weapon`, `fire_weapon`, `fire_missile` | Shared hardpoints, energy use, cooldowns, projectile initialization and missile release | SDL-only SWC aiming offset and laser cooldown; other weapon parameters remain WC1 |
| `house_keep_objects`, `update_objects_in_space` | Shared lifetimes, collisions, damage, missile guidance, shield/energy recharge and animations | NPC ship intelligence is skipped during SWC flight; mission transitions remain disconnected |
| `select_new_gun`, `select_new_release_weapon`, `cycle_onscreen_targets`, `target_locking` | Shared selection and missile lock state | SDL presents SWC art and text instead of WC1's cockpit packets |
| `draw_nav_pointer`, `auto_pilot_valid`, `missile_on_tail`, `calculate_damage_level` | Shared objective projection and warning queries | SWC navigation activation and stale-marker reset; autopilot uses WC1 proximity/hazard gates, with travel still disconnected |
| `malf`, `your_internal_damage`, `place_damage_on_cockpit` | Shared component damage and impact flow | SDL-only SWC malfunction countdown and three physical damage regions; original Mac static, damage and spark artwork |
| `SetSpaceFlightFrameTiming` | Initializes the shared flight tick interval, 50 ms at WC1's default 20 Hz | Replaces the host's hard-coded 62 ms; SWC's elapsed-tick translation scaling is not reproduced |
| `generate_stars`, `update_star_field`, `transform_objects_to_your_view`, `get_right_shape` | Projects the scene and chooses among 37 decoded ship views | SWC hotspots, scale, and exact presentation still need reconstruction |

The SDL version of `nav_getxy` writes coordinates through byte copies because
the existing packed objective records can place them at odd addresses. Its
calculations are unchanged; the Win32 reference implementation is preserved.
During SWC flight, `get_right_shape` returns the selected frame to the SDL
texture cache instead of fetching WC1 capital-ship packets.

The keyboard adapter passes one SDL state sample to the original Win32 key
polling interface for each simulation tick. Held keypad keys use the same
bindings as the existing SDL key events. It clears released axes independently
and samples throttle and boost independently of steering, without relying on
operating system key repeat. The mouse adapter initializes WC1's input viewport
to a 320x200 rectangle centered at SWC's scene origin (160,100), then queues one
motion sample per simulation tick and calls `player_input` directly. This uses
the existing response tables, direction signs, right-button behavior, and
recentering code without duplicating the mouse algorithm. SDL converts window
coordinates and pointer warps through its logical renderer, including resize
and letterboxing; the extra WC1 cockpitless camera offsets are not applied.

The SWC event loop owns event pumping. Its calls into `player_input` cannot
consume unfiltered SDL button or window events through WC1's separate loop.
Primary/secondary host button states stay clear, and no weapon button events
are queued. Right-button state is passed only as a motion modifier; two presses
within WC1's 20/60-second interval activate the shared `your_afterburner` routine
while the button is held. This timing-only check tolerates WC1's pointer
recentering between clicks. SDL handles weapon buttons separately and calls the
shared firing routines. Short left clicks remain pending until a simulation
tick. `HandleSpaceFlightControls` is still bypassed because its remaining HUD
and mission commands are not connected. SDL builds zero-initialize the legacy
input event before polling, since motion and empty polls leave some fields
unset. In SDL builds only, `accelerate` multiplies signed throttle steps by 256
instead of left-shifting negative values. The reference build retains the
original expressions and event declaration.

### Movement comparison

Ghidra inspection of the original 68000 instructions supports sharing the
movement algorithms, with these qualifications:

- CODE_09 `players_flight_dynamics` +0x02de computes the same axis-rate times
  input divided by eight, with the same yaw/roll signs and blowing-up branch.
  SWC uses wider fields in its 204-byte object definitions.
- CODE_02 `rotate_object` +0x4306 applies pitch, yaw, and roll, then moves each
  rotation value toward zero. CODE_04 `alter_pitch` +0x2b1c, `alter_yaw` +0x2b6c,
  and `alter_roll` +0x2bba use the same basis-vector rotation and normalization
  sequence as WC1.
- CODE_05 `celerate` +0x09c6 clamps commanded speed to zero and maximum speed
  times 256. `accelerate` +0x428c applies the same malfunction adjustment before
  calling it. `your_afterburner` +0x42e0 and `fire_afterburner` +0x3a38 use the
  same fuel gate, eight/two-tick boost renewal, and velocity limit. Sound
  scheduling is platform-specific.
- CODE_02 `accelerate_and_move_object` +0x439e shares the forward target velocity,
  acceleration toward that target, drift, boost/brake timer, and fuel-drain
  calculations. Its final position update multiplies velocity by a seven-entry
  table indexed by elapsed Mac ticks clamped to 0..6 (+0x43ba..+0x43d2 and
  +0x471e..+0x4762). WC1 adds velocity once per simulation tick. The host retains
  WC1's fixed-step model rather than mixing the two timing systems.
- SWC also differs in CODE_04 `max_acceleration` +0x1a10 (one-third bonus for
  ratings 1..7 as well as ratings above 8) and `drain_fuel` +0x1944 (recalculate
  maximum speed when fuel reaches or passes zero). The shared WC1 routines
  apply those two conditions during SWC flight. WC1's original rating gate
  and retail array-address fuel check remain in place for WC1.
- CODE_01 `get_player_input` +0x4508 and `process_player_input` +0x3cba operate
  on Mac events, key-character state, and action flags. A matching function
  name does not establish identical input handling.

The movement and mouse-control updates were compiled with `make modern modern-gui`
and reviewed statically. No game, GUI, or interactive check was run for those updates.

### Player weapons

The flight host now calls WC1's object lifecycle instead of moving only the
initial ships. Guns use the shared energy gate, hardpoints, projectile creation,
convergence, collision/damage, and recharge code. Missiles use the shared loadout,
lock gate, launch timer, guidance, collision grace, expiration, and explosion code.
WC1 handles shield/armor damage, debris, and ammunition removal. Player destruction
stops simulation/input and displays an exit message; the death scene is pending.

Original instructions support this reuse:

- CODE_09 `fire_players_lasers` +0x021a has the same cooldown/energy gate and
  selected-gun call. CODE_05 `fire_fixed_projectile_weapon` +0x38fe iterates
  enabled projectile slots; `fire_missile` +0x3856 requires a lock for HS/IR.
- CODE_05 `fire_weapon` +0x3320 shares the allocation, hardpoint, energy,
  lifetime, velocity and release flow. Its player convergence point adds 400
  world units along up for lasers and 300 for neutron/mass driver
  (+0x34c8..+0x3532). Expanded DATA/0 +0x31f2 has refire delays `{4,10,4,0}`,
  compared with WC1's `{6,10,4,0}`. Both differences apply only to SWC.
- CODE_02 `house_keep_objects` +0x3d38 and `update_objects_in_space` +0x418c
  follow the same lifecycle stages. This does not establish complete gameplay
  equivalence: WC1 still supplies collision/damage algorithms and ship/weapon
  definitions, and SWC's elapsed-tick movement scaling remains pending.
- CODE_02 `animate_shape` +0x4790 uses the same frame, loop, scale and removal
  commands. SWC explosion types 48/49/50 use 15/20/22 frames, scale 768/512/1024,
  and one tick per frame. The host supplies the recovered sequences from
  DATA/0 +0x2564/+0x261c/+0x25c0 to WC1's interpreter and restores the original
  definitions on exit. Mac handle management and explosion screen shake are absent.

CODE_08 `get_Mac_mem` +0x0050..+0x022c identifies the projectile/effect OBJT
chunks; CODE_03 supplies debris aliases. The SDL cache loads each used resource
with its own frame count, including intentionally empty frames. Missiles use
MS00..03/1 (22 views). Ship views remain 37-frame sets. Resource data is borrowed
only while CMFs are open, and all textures are freed on normal/error exits.

The renderer now adds projected Y to the scene origin, matching both original
engines. WC1's cockpit-packet drawing is bypassed for messages, missile removal,
VDU changes and damage overlays; shared gameplay state is retained. Win32
reference branches remain unchanged. NPC ship AI, wave spawning, mission
completion and weapon audio are still pending.

### Hornet cockpit

The cockpit presents shared WC1 state through the Mac demo's artwork.
WC1's `update_digital_readouts` supplies actual and commanded speed; the SDL
readout adapter applies SWC's four-digit zero padding and original positions.
`get_color` classifies contacts, `rotational_pos_to_scanner_pos` supplies the
radar math, and `set_objective_range` / `objective_name` supply navigation data.
The shared scanner routine selects the Hornet geometry and SWC's extra one-pixel
X offset only during SWC flight. The Win32 reference branches are preserved.

| Instrument | Original evidence | Current presentation |
| --- | --- | --- |
| Actual / commanded speed | CODE_13 `update_digital_readouts` +0x0660; expanded DATA/0 +0x666 / +0x696 | Baselines (96, 32) / (205, 32), using the shared WC1 speed calculations |
| Weapon energy / fuel / throttle | CODE_13 `update_bars` +0x0520; DATA/0 +0x420 / +0x78c / +0x786 | PC00 chunks 15 / 16 / 17; Hornet frame maxima 10 / 13 / 13 |
| Hull, armor and shields | CODE_13 `show_weapon_disp` +0x0e54 | PC00/10: hull, armor flashing below half strength, fore/aft shield presence |
| Radar | CODE_13 `draw_3d_scanner` +0x285a; DATA/0 +0x300 / +0x390 | CKPT/31 contact sprites, 30,000-unit cutoff, nav marker, then PC00/18 grid at (139, 151) |
| Destination / range | CODE_13 `update_digital_readouts`; DATA/0 +0x6f6 | Baseline (200, 94), 11-pixel line spacing, `FAR` at 32,000 or above |
| Target / range | CODE_13 `update_digital_readouts`; DATA/0 +0x6c6 | Baseline (200, 110), using WC1's object name and surface range |
| Target hull / armor / shields | CODE_13 `show_target_disp` +0x1270; DATA/0 +0x756 | STnn/2 or SHnn/38 at (230,136), with PC00/9 shields |
| Gun / missile selection | CODE_13 `update_status_text` +0x102e; DATA/0 +0x726 | Original short names at (51,84); host keeps them visible and adds ammunition/lock state |
| Reticle | CODE_13 `overlay_head_up_display` +0x2f56; DATA/0 +0x792 | CKPT/1 frame 0 at (144,96), including the frame's offsets |
| Forward navigation marker | CODE_13 `draw_nav_pointer` +0x3460 | Shared WC1 projection, CKPT/1 frame 3; visible in target mode too, with stale offscreen positions cleared |
| Hostile direction | CODE_13 `draw_3d_scanner` +0x285a | CKPT/1 frames 6..13; tracks the last hostile target or first enemy contact within scanner range |
| Autopilot / missile / severe damage lamps | CODE_13 `update_lights` +0x0408; DATA/0 +0x3d8 | PC00/12, /14, /13; shared WC1 warning queries refreshed every seven simulation ticks, damage alarm at (156,45) |
| Malfunctioning VDUs | CODE_13 `malf` +0x1a28, `update_VDUs` +0x0aa8, `update_dead_disp` +0x0dda; CODE_03 `init_vdus` +0x0914 | VDUS/1 and /2 static mapped onto the original Hornet polygons, replacing live displays |
| Component status | CODE_13 `show_weapon_disp` +0x0fc8, `update_status_text` +0x102e; DATA/0 +0x886 / +0x89a / +0x8ae | Cycles damaged components every 30 healthy weapon-display updates, with original names, severity labels and colors |
| Damaged panels / sparks | CODE_13 `place_damage_on_cockpit` +0x41ae, `cockpit_explosion` +0x4222, `explosion_draw` +0x40a0; DATA/0 +0x7f2 | PC00/7 overlays disable radar/left VDU/right VDU; CKPT/6 supplies the eight-frame spark sequence |

The layout constants come from DATA/0 expanded using the original CODE_01
startup decoder. Offsets above refer to the expanded A5-relative data, not
compressed resource-file offsets. Resource lists at +0x433e and +0x43e6 identify
the shape sets. The SDL cache verifies their frame counts and places each frame
using its signed x/y offsets, following CODE_01 `MacDraw2` +0x0ef4. Radar contacts
are transformed from their current world positions, as in SWC; this avoids the
scene projection cache omitting objects close to the player.

Spatial markers and target brackets are drawn before the cockpit artwork,
following CODE_13 `update_cockpit` +0x02d4, so solid panels mask them. Text remains
on its own overlay, and panel damage/sparks are drawn last. Target bounds include
the SDL sprite rotation around the same explicit pivot used to draw the ship.
The hostile arrow is suppressed outside scanner range to avoid stale coordinates.

Display state, random static frames, component cycling and sparks advance only
on simulation ticks, so render rate and pausing do not change their duration.
The shared `malf` entry point uses SWC's countdown during SWC flight, including
when WC1 targeting or damage logic calls it. CODE_04 `dp_random` +0x5d44,
`random_number` +0x5d8c and `Random3DO` +0x5dea exclude the upper bound: SWC's
malfunction test samples 0..14 and its countdown samples 5..9, while dead VDUs
select frames 0..2 from the four-frame resource. The SDL adapter adjusts WC1's
inclusive random bounds without replacing its RNG. Likewise CODE_05
`your_internal_damage` +0x184a selects three cockpit regions, not WC1's four.

VDU rendering respects the shared mode stack and SWC's physical-damage flags.
`show_navigation_disp` +0x121a, `show_damage_disp` +0x123a and `show_info_disp`
+0x1256 are empty functions in the Mac demo. Destination/range text is produced
by the separate readout layer, not a missing navigation-page implementation.
CODE_01 `MacDrawcel2` +0x14a4 maps static over the quad from DATA/0 +0x426/+0x5a6;
the SDL replacement uses `SDL_RenderGeometry` and ignores frame-origin offsets
for this operation. Cockpit damage and malfunction state reset on a new flight.

This pass supports the Enyo 1 Hornet cockpit. Ship capacities still use WC1
definitions. Shield, armor and weapon-energy displays now respond to shared
combat state. Target brackets use SDL corners around the rotated sprite bounds;
the original MacScale1 rasterizer and CKPT/36 corner artwork remain unimplemented.
Target names use WC1 object names rather than SWC's pilot/ace-name substitutions.
Component messages and missile-lock notices use WC1 lifetimes at host positions.
The cockpit toggle retains the spatial HUD, weapon/component status,
target/navigation text, and messages. Automatic target acquisition, target-lock
controls/animation, ITTS, cockpit banking and pilot animation, side/rear views,
communication portraits, original message timing and other cockpitless instruments
remain pending. Lamps reuse WC1's availability/proximity queries; SWC-specific
autopilot escort gates and autopilot travel are not connected. Damage sounds and
alarms remain silent until SWC audio is implemented.

#### Text font

The inspected Mac cockpit layers request QuickDraw font ID 21 (Helvetica) at
size 9: CODE_06 `AllocTextLayer` +0x1d9a and CODE_14 `init_text` +0x1fec.
CODE_06 `tprintf` +0x1ef4 draws QuickDraw text, advancing a newline by font size
plus two pixels. SDL2 has no built-in font. The current substitute is a
monochrome, nine-pixel ASCII atlas derived from Anonymous Pro Regular, with a
five-pixel character advance. This is a provisional display substitute and does
not claim identical Monaco metrics or original SWC typography.

The generated **SWC HUD Bitmap** is compiled into the executable, so normal
builds and installations need neither SDL2_ttf nor a system font. The original
TTF, source hash and SIL Open Font License are in
[`third_party/anonymouspro`](../third_party/anonymouspro/README.md). The optional
[`bin/buildSwcHudFont.py`](../bin/buildSwcHudFont.py) generator uses Pillow and
records its rasterizer versions in the output. Release archives include the
font license. Bytes outside printable ASCII currently display as `?`.

Cockpit and weapon validation consists of original-instruction and resource inspection,
static source review, and `make modern modern-gui`. No game, GUI, or interactive
check was run for this pass; its appearance still needs user review in flight.

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
2. Refine ship-sprite hotspots, SWC projection, scaling, and the remaining
   320x240 HUD. Instrument sprites use their original offsets; the current
   SDL renderer still centers ship sprites selected and projected by WC1.
3. Connect nav-sphere transitions, NPC AI/firing, carrier return, and the
   remaining HUD modes. The demo explicitly disables its simulator, so campaign
   flight is the useful first playable milestone.
4. Connect opening/landing/funeral movies to game flow; add scripted scenes,
   AIFF speech/effects, music, and palette transitions. The LMov decoder and
   Hornet launch playback are now present; other scene entry points are pending.
