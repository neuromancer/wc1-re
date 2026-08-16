# SDL2 port

The SDL2 target is a native host for the reconstructed game. It reuses the game
logic, resource loaders, event queue, indexed framebuffer, and `ix` audio API.
Platform code is selected with `WC1_SDL` and must not alter the MSVC 4.20
reference build or its binary comparisons.

## Port boundary

- `src/sdl/` owns windows, input translation, timing, host audio, paths, and
  video presentation.
- SDL input is converted to the game's existing scan codes and event queue;
  gameplay code does not consume SDL events directly.
- The game still renders a 320x200 indexed framebuffer and 256-colour palette.
- Port-only state remains in SDL files. Original-address globals keep their
  reconstructed types and ownership.
- Native objects live under `out-modern/` and never enter `WC1.EXE` or
  `binary-comp` comparisons.

Normal native development builds are compiled with AddressSanitizer and
UndefinedBehaviorSanitizer. A sanitizer report is a port bug, even when the
original executable happened to tolerate the same memory access. Release
archives use `MODERN_RELEASE=1` to omit sanitizer instrumentation.

## Game data and audio

Kilrathi Saga data is the primary supported data set. DOS game-data support is
partial: compressed packet resources, `MUSIC.MID`, `WINGLDR.TIM`, AdLib music,
and synthesized OriginFX sound effects are supported, while other DOS-specific
data or behavior may not be.

The DOS release describes sound effects as compact OriginFX command records,
not sampled WAV or VOC files. The SDL host interprets those records and mixes
their YM3812 output with music in the existing audio callback.

## Enhanced renderer

`--enhanced` selects the optional OpenGL 3.2 renderer. It records the ordered
space-object layer and redraws it at output resolution with sharp-bilinear
sampling. Ships, missiles, mines, projectiles, asteroids, debris, explosions,
stars, planets, the intro logo, and launch-bay doors use this path. The cockpit,
HUD, text, palette effects, and other screens remain in the indexed base frame.

The default renderer always uses the original software drawing path. If the
enhanced renderer cannot record an object, that object also falls back to the
software path. Renderer-specific OpenGL state stays in `src/sdl/`; recovered
game files expose only narrow `WC1_SDL` hooks.

## Joystick input

The default `--joystick-mode=original` mode preserves the original X/Y and
two-button controls. SDL's mapped controller interface supplies the left stick
and A/B buttons on recognized gamepads. Other devices use their first two axes
and buttons. Device removal and reconnection are handled without restarting the
port.

The optional WCAT-style modes give each action its own button. On mapped
gamepads, buttons 1–4 are A/B/X/Y:

| Mode | Buttons 1–4 |
| --- | --- |
| `--joystick-mode=4button-2axis` | Fire, missile, afterburner, roll/throttle modifier |
| `--joystick-mode=4button-4axis` | Fire, missile, afterburner, cycle target |

The four-axis mode reads the mapped right stick, raw axes 3/4, or the X/Y axes
of a second joystick. Its default axis layout is `twin-stick-roll`; select a
different layout with `--joystick-axes=<layout>`:

| Layout | Horizontal and extra axes |
| --- | --- |
| `twin-stick-roll` | Primary X yaws; extra X rolls; extra Y is relative throttle |
| `twin-stick-yaw` | Primary X rolls; extra X yaws; extra Y is relative throttle |
| `hotas-yaw` | Primary X rolls; rudder yaws; final axis is linear throttle |
| `hotas-roll` | Primary X yaws; rudder rolls; final axis is linear throttle |
| `linear-throttle` | Primary X yaws; third axis is linear throttle; keyboard controls roll |
| `rudder-yaw` | Primary X rolls; third axis yaws |
| `rudder-roll` | Primary X yaws; third axis rolls |

Primary Y always controls pitch. Directional extra axes use a 25-percent
deadzone. For example:

```sh
./wc1-modern --joystick-mode=4button-4axis \
  --joystick-axes=hotas-yaw
```

Add `--joystick-debug` to print the detected device type and each joystick
axis, button, and hat event to the terminal.

Add `--joystick-rumble` to enable gentle heavy-weapon, damage, collision, and
afterburner feedback on supported controllers and force-feedback joysticks. It
is disabled by default; devices without rumble support continue without
feedback.

Gamepad Start pauses and resumes during spaceflight, Back acts as Escape, and
the Y face button answers Yes at `Y/N` prompts. Mapped SDL controllers use
their named buttons. Both WCAT-style modes also provide these spaceflight
bindings:

The four primary actions map to A/B/X/Y: fire guns, fire the selected release
weapon, hold afterburner, and the mode-specific fourth action.

| Control | Action |
| --- | --- |
| LB | Cycle guns |
| RB | Cycle missiles and other release weapons |
| LT or left-stick click | Navigation display; press again to open the map |
| RT or right-stick click | Autopilot |
| D-pad Up / Down | Full speed / stop |
| D-pad Left / Right | Communications / target lock |

While the communications list is open, D-pad Up/Down moves the highlighted
choice, Right selects it, and Left closes the list. The flight sticks and
A/B/X/Y actions remain active while choosing a response.

The raw 12-button fallback uses buttons 5/6 for LB/RB, 7/8 for LT/RT, 9/10
for Back/Start, 11/12 for the stick clicks, and the first hat for the D-pad.

## Development commands

```sh
make modern
make run-modern
make run-modern-dos
make run-modern-mission SERIES=1 MISSION=0
```

`make modern-test` runs the integrated sanitizer smoke check.
`make modern-test-full` adds the standalone SDL subsystem checks and the OpenGL
renderer check; the latter skips when no display is available.
