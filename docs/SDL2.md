# SDL2 port

The native target reuses the reconstructed game and `ix` audio library.
`src/sdl/` owns host windows, input, timing, paths, audio, and presentation;
native objects stay under `out-modern/` and never enter the MSVC comparison
build.

The game still renders a 320x200 indexed frame. Both backends present it in a
centered 4:3 viewport and use that viewport for pointer mapping. `--enhanced`
selects the optional OpenGL 3.2 space-object renderer; the original software
path remains the default and fallback.

Kilrathi Saga data is fully supported. DOS support includes compressed packet
resources, `MUSIC.MID`, `WINGLDR.TIM`, AdLib music, and synthesized OriginFX
effects. In-flight sampled and synthesized effects use camera-relative stereo
panning; synthesized effects retain distance falloff.

Kilrathi Saga wave effects receive 6 dB of mix headroom so full-scale and
overlapping weapon sounds do not mask the streamed music. The DOS OriginFX
path retains its separate synthesized music-and-effects mix.

Development builds use ASan and UBSan. Release builds select
`MODERN_RELEASE=1`.

## Graphical launcher

`make modern-gui` builds `out-modern/wc1-modern-gui` (`.exe` on Windows).
The launcher opens by default without arguments. With arguments, use `--gui`
to show it before startup; otherwise the existing command-line path is used.
`make modern` continues to build the SDL-only executable.

The Slint window follows the WC2 launcher: title banner, installation picker,
graphics and gameplay controls, joystick controls, and Cancel/Launch buttons.
The WC1 controls map to:

| Control | Command-line equivalent |
| --- | --- |
| Enhanced OpenGL | `--enhanced` |
| EGA 16-colour dithering | `--ega` |
| Cockpitless view | `-c` (campaign flight; the training simulator keeps its cockpit) |
| Rumble | `--joystick-rumble` |
| Control mode | `--joystick-mode=original`, `4button-2axis`, or `4button-4axis` |
| Axis layout | `--joystick-axes=` with a layout listed below |

The axis selector is enabled for four-axis controls. Command-line options seed
the controls, and accepted choices take precedence. Options are applied after
the dialog so EGA and rumble can be unchecked. WC1's SDL frame-rate text output
is a stub, so the launcher does not expose `-f`. WC2's balanced-difficulty option
has no WC1 counterpart.

The selected directory must contain `GAMEDAT/MODULE.000`, `CAMP.000`, and
`INSTALL.DAT`. Validation reads byte 7 of `MODULE.000`, matching `SdlUsingDosData`:
1 identifies DOS compression, while other values identify Kilrathi Saga's raw
packets. Saga data also requires readable `PREFLITE.STR`, `POSFLITE.STR`, and
`MISSION.STR` in the sibling `STREAMS` directory. Names are matched without
case sensitivity. These are startup checks, not a complete game-data audit.
Invalid selections disable Launch; folder-picker errors stay in the dialog.

`src/sdl/slint/` is a separate CMake project using C++20, CMake 3.21+, and Rust
1.88+. It pins Slint 1.16.1 and Native File Dialog Extended 1.3.0 and links them
statically with the existing native game objects. Linux needs Fontconfig and
D-Bus development packages, plus a working XDG desktop portal for browsing.
Windows retains WC2's static Slint import fix. On macOS, the folder picker is
initialized after the native event loop starts so SDL can take focus on launch.
The GUI changes the working directory only on acceptance; cancellation exits
before SDL or the game is initialized. No settings file is written.

Release jobs build this integrated executable and package it as `wc1-modern`.
The MSVC reference executable and comparison flags are unchanged.

## Joystick input

`--joystick-mode=original` preserves the original two-axis, two-button layout.
The `4button-2axis` and `4button-4axis` modes add WCAT-style controls. The
four-axis mode accepts:

| `--joystick-axes=` | Extra-axis behavior |
| --- | --- |
| `twin-stick-roll` | Extra X rolls; extra Y is relative throttle |
| `twin-stick-yaw` | Extra X yaws; extra Y is relative throttle |
| `hotas-yaw` | Primary X rolls; rudder yaws; final axis is throttle |
| `hotas-roll` | Primary X yaws; rudder rolls; final axis is throttle |
| `linear-throttle` | Third axis is throttle; keyboard controls roll |
| `rudder-yaw` | Third axis yaws |
| `rudder-roll` | Third axis rolls |

Use `--joystick-debug` for event diagnostics and `--joystick-rumble` for
supported feedback. The README documents the common controller bindings.

## Commands

```sh
make modern
make modern-gui
make modern-test
make modern-test-launcher
make run-modern
make run-modern-dos
make run-modern-mission SERIES=1 MISSION=0
```

`modern-test` runs data-free SDL2 initialization, text, and ship-resource
cleanup checks. `make modern-test-landing MODERN_RUN_DIR=/path/to/WC1` also
runs all four fighter landing animations at all four damage levels using
installed game data, checking the damage comment and restored screen bounds.

`modern-test-launcher` exercises argument forwarding, GUI defaults, cancellation,
and option overrides with a stand-in dialog and SDL's dummy drivers. Its separate
directory validator uses temporary fixtures for DOS/Saga, missing or truncated
files, WC2 data, mixed-case filenames, and UTF-8 paths. Neither check launches
gameplay or requires retail data. The validator can also check an installation
with `out-modern/tests/sdl_launcher_paths /path/to/WC1`.

The Windows v0.0.12 crash offset `0x1a9da` reported in
[issue #14](https://github.com/neuromancer/wc1-re/issues/14) is the object-class
lookup in `free_ship` (original entry `0x0040BC70`). The carrier-return
sequence frees slots 1 and 2 even when a nav transition has marked them empty
with type `-1`. Native builds must skip that sentinel before converting it to
an enum and indexing the object-type table. Capital ships still require
cleanup when their `shapeSet` is null. The subsequent `landing` animation
(original entry `0x00408650`) must index its canopy and damage-comment tables
as pointer arrays, whose entries grow from four to eight bytes on 64-bit
hosts. Both fixes are confined to `SDL_PORT`.
