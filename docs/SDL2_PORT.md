# SDL2 port plan

The SDL2 port is a second build of the reconstruction.  It must not replace or
weaken the MSVC 4.20 reference build: the default `make`, `make verify`, and
`binary-comp` workflows continue to compile the recovered Win32 source exactly
as before.  Native-only changes are selected with `WC1_SDL`, and the original
branch of every condition remains the assembly-comparison authority.

## Design constraints

- Reuse the game core, resource loaders, raster routines, event manager, and
  `ix` scheduling logic.  SDL owns only facilities supplied by Win32,
  DirectDraw, DirectSound, or WinMM in the shipped executable.
- Keep platform declarations in a narrow compatibility header.  Do not spread
  SDL types through recovered game structures or function signatures.
- Keep port-only state in port source files.  Original-address globals retain
  their recovered types and ownership in the reference build.
- Preserve the game's 320x200, 8-bit indexed framebuffer and 256-entry palette.
  Convert to the host texture format only when presenting a frame.
- Translate SDL input into the existing `QueueInputEvent` and
  `SetInputKeyState` contracts, including IBM PC scan codes.  Gameplay code
  must not consume SDL events directly.
- Preserve the original `ix` public API.  SDL's audio callback replaces the
  DirectSound device/voice backend while the original sample, stream, sound,
  priority, and mixing code remains shared wherever possible.
- A native build must not become an input to reconstruction reports.  Native
  objects and generated files live under `out-sdl/`.

## Platform boundary inventory

| Boundary | Current owner | SDL2 port direction |
| --- | --- | --- |
| Process entry/window/messages | `src/winmain.c`, `src/debug.cpp` | SDL entry point, window lifetime, and event pump that feed the existing input queue |
| Keyboard/mouse/joystick | `src/winmain.c`, `src/sysinput.c` | SDL events/state, relative or warped mouse coordinates, game-controller axes/buttons |
| 8-bit display/palette | `src/dib.c` | CPU indexed framebuffer plus SDL texture/renderer presentation |
| Frame timing | `src/screen.c`, `src/eventmgr.c` | SDL monotonic ticks, delays, and timers |
| Digital audio | `src/ix/dsp.cpp`, `mixer.cpp`, `dspv.cpp` | SDL audio device/callback behind the existing `ix` API |
| Threads/events/locks | `src/ix/streamer.cpp`, `thread.cpp`, `dsp.cpp` | SDL threads, mutexes, condition variables, and semaphores |
| Registry/config | `src/sound.c`, `src/gr.c` | File-backed preferences with original defaults |
| Disc/path probing | `src/cdrom.c`, CRT calls in core | Portable paths and an explicit data root; retain ordinary stdio loaders |
| Debug/host services | `src/mono.c`, `src/debug.cpp`, message boxes | stderr/log files and SDL message boxes |
| MSVC x86 assembly | `src/screens.c`, `src/ix/lzo1x.cpp`, isolated core sites | Portable C/C++ alternatives selected only for `WC1_SDL` |

## Delivery steps

Each checked step is committed and pushed independently.

1. **Build boundary and host types.** Add a separate native build, define
   `WC1_SDL`, introduce fixed-width Win32-compatible declarations, and compile a
   platform smoke target against the installed SDL2 development package.
2. **Native source compile.** Bring the shared game sources through a complete
   modern C/C++ compile by isolating MSVC-only syntax and supplying portable
   implementations for proven hand-written assembly routines.  Add compile and
   link checks that cannot affect `WC1.EXE`.
3. **Window and input.** Implement SDL startup/shutdown, event pumping, scan-code
   translation, mouse capture/warping, controller discovery, and focus/quit
   handling through the existing game APIs.
4. **Video and timing.** Implement the indexed framebuffer, palette upload,
   scaling/fullscreen presentation, vertical-update behavior, monotonic clock,
   and timer callbacks.
5. **Audio and streaming.** Replace DirectSound voices and Win32 synchronization
   with an SDL audio backend while retaining `ix` sound/stream state machines.
6. **Host services and data paths.** Port preferences, debug output, CD/data
   discovery, save paths, and remaining CRT/Win32 compatibility calls.
7. **End-to-end verification.** On each supported host, configure, compile, link,
   start from the game data directory, and exercise startup/input/video/audio.
   Also run `make verify` to prove the reference build is unchanged.

## Verification gates

Every port commit must pass the narrowest applicable native build/test.  Any
commit that touches a recovered source or shared header must additionally pass
`make verify`.  Before declaring the port complete:

- the native executable configures, builds, and links with SDL2 on at least
  macOS and Linux-compatible toolchains;
- it finds an external Kilrathi Saga WC1 data tree without copying game data
  into the repository;
- window, keyboard, mouse, controller, indexed video, palette fades, sound
  effects, and streaming music have all been exercised;
- the default MSVC build still has zero verification-gate regressions.

The README remains unchanged until the port reaches the end-to-end milestone.
