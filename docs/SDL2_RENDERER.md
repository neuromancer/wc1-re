# SDL2 enhanced renderer

The SDL2 port includes an experimental OpenGL renderer for space objects. It
evaluates sprite rotation and scaling at the window's resolution and applies a
sharp-bilinear filter, reducing the blockiness and frame-to-frame shimmer of
the original 320x200 software transform.

The renderer is opt-in. Start the game with:

```sh
./wc1-modern --enhanced
```

The default path still uses the existing SDL indexed-frame renderer and does
not create an OpenGL context.

## Rendering model

The game continues to produce an indexed 320x200 frame. While a space frame is
being drawn, the SDL2 integration records the ordered object sprites instead
of asking the original software rasterizer to transform them. The OpenGL
backend then:

1. uploads the completed indexed base frame and current palette;
2. draws the base frame with nearest-neighbour sampling;
3. decodes each recorded object into its original palette indices;
4. applies the game's position, scale, rotation, flip, and clipping values at
   output resolution; and
5. samples the object with a one-output-pixel sharp-bilinear transition.

Palette indices stay live until presentation. Palette fades, damage flashes,
and animated glow colours therefore affect the enhanced objects in the same
frame as the rest of the display.

The cockpit, HUD, text, scanner, and other interface elements remain part of
the 320x200 base frame. A mask derived from the game's viewport geometry keeps
the enhanced object layer inside the visible space area, and pixels already
occupied by the cockpit or HUD occlude that layer.

If an object cannot be recorded, the generic hook returns control to the
original software draw call for that object. The default renderer always takes
that original path.

## Code boundary

Renderer-specific code is confined to the SDL2 host:

| File | Responsibility |
| --- | --- |
| `src/sdl/video.c` | Backend-neutral video facade and indexed renderer |
| `src/sdl/video_state.c` | Backend selection, output viewport, and input-coordinate mapping |
| `src/sdl/gl_renderer.c` | OpenGL context, shaders, sprite recording, and compositing |
| `src/sdl/video_internal.h` | Private SDL2 video and input contract |

Recovered game units contain only generic `WC1_SDL` frame and object hooks.
They contain no OpenGL state, shader details, or backend-selection logic. This
keeps the reference MSVC build unchanged and lets future SDL renderers use the
same narrow boundary.

## Window and input scaling

At 320x200 or larger, the OpenGL backend chooses the largest centred integer
scale that fits the window and letterboxes the remainder. Smaller windows use
a centred fractional scale. Mouse events and cursor warps use the same viewport
calculation, including after a fullscreen transition, so input remains aligned
with the displayed game area.

## Compatibility

The enhanced path requires an OpenGL 3.2 core-profile context. It currently
enhances only objects that pass through the space-object draw list; all other
game screens retain the indexed renderer's appearance. The option is
experimental and remains disabled unless `--enhanced` is supplied.

The implementation preserves the original rotated-sprite size limit: the
software transform rejects frames larger than its `0xfa00`-byte scratch
buffer, while its unrotated fast path has no such limit.

## Verification

Run the renderer and the other standalone SDL2 regressions with:

```sh
make modern-test-full
```

The exhaustive suite keeps the headless indexed-renderer tests separate from
`tests/sdl_gl_renderer.c`. The OpenGL test exercises transforms, ordering,
palette-index fidelity, viewport masks, the original rotation limit, and
letterboxed input mapping. It reports a skip when no real OpenGL display is
available; an available display must pass the test.

Routine builds do not compile these standalone test executables. `make
modern-test` runs only the native executable's integrated sanitizer smoke check;
use the exhaustive target when changing an SDL2 subsystem.
