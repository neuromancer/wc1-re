Wing Commander SDL2 port
========================

This archive contains the work-in-progress native SDL2 port. Release binaries
are optimized and do not include the AddressSanitizer or
UndefinedBehaviorSanitizer instrumentation used by development builds.

No copyrighted game data is included. Extract every file in this archive and
start wc1-modern (wc1-modern.exe on Windows) without arguments to open the
graphical launcher. Keep the bundled runtime libraries beside the executable.
Choose the installed WC1 directory containing GAMEDAT, select the graphics and
joystick options, and click Launch WC1. Kilrathi Saga also needs the sibling
STREAMS directory with PREFLITE.STR, POSFLITE.STR, and MISSION.STR.

The launcher offers enhanced OpenGL rendering, EGA dithering, cockpitless
campaign flight, joystick modes and axis layouts, and rumble. Settings apply
to the current launch. Command-line arguments start the game directly from the
working directory; add --gui to review them in the launcher before starting.
Linux's Browse button uses an installed XDG desktop portal. The directory can
also be entered directly.

The DOS data path supports the original compressed packet resources and plays
OriginFX music and synthesized sound effects through an embedded YM3812
(AdLib) emulator using MUSIC.MID and WINGLDR.TIM. No external MIDI synthesizer
or sound bank is required. DOS game-data support remains partial.

Kilrathi Saga sound effects are balanced against the streamed music. During
spaceflight, Ctrl+M toggles music and Ctrl+S toggles sound effects.

The macOS archives are ad-hoc signed, not notarized with an Apple developer
certificate. macOS may therefore require the usual confirmation for software
downloaded outside the App Store.

The corresponding source is available from the Git tag attached to the same
GitHub Release. See LICENSE.txt for the project license and
THIRD-PARTY-NOTICES.txt for bundled runtime components.
