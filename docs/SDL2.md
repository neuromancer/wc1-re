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
