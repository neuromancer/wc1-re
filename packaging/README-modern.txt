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
campaign flight, cheat mode, joystick modes and axis layouts, and rumble.
Clicking Launch remembers the directory and options in launcher.ini for the
next GUI launch.
Cancel leaves the saved settings unchanged. The file is stored in:

  macOS: ~/Library/Application Support/Origin Systems/WC Kilrathi Saga/
  Linux: ~/.local/share/Origin Systems/WC Kilrathi Saga/
  Windows: %APPDATA%\Origin Systems\WC Kilrathi Saga\

Linux uses XDG_DATA_HOME instead of ~/.local/share when set. Delete launcher.ini
to reset the GUI preferences. Command-line arguments start the game directly
from the working directory; add --gui to review them in the launcher before
starting. Explicit arguments override saved settings. Direct launches do not
read or write the INI file.
Linux's Browse button uses an installed XDG desktop portal. The directory can
also be entered directly.

Cheat mode enables player invulnerability and disables player collisions in
DOS, Kilrathi Saga, and the SWC Mac demo. Use the GUI checkbox or --cheats.
The checkbox is saved with the other launcher options. Existing legacy cheat
arguments and configuration settings still work independently.

The DOS data path supports the original compressed packet resources and plays
OriginFX music and synthesized sound effects through an embedded YM3812
(AdLib) emulator using MUSIC.MID and WINGLDR.TIM. No external MIDI synthesizer
or sound bank is required. DOS game-data support remains partial.

Kilrathi Saga sound effects are balanced against the streamed music. During
spaceflight, Ctrl+M toggles music and Ctrl+S toggles sound effects.

The experimental Super Wing Commander Mac demo path includes sequence music
for its first-mission scenes and flight. Keep swc-music.sf2 beside the
executable. The demo's resource forks (the application, MIDI/*.MooV and
AIFF/Stranded.aif) may stay native, come as raw .rsrc or AppleDouble ._
sidecars, or remain MacBinary as archives of the original disc serve them:
discmaster's "SuperWing DEMO" folder works without further extraction.
WC1_SWC_SOUNDFONT can select another complete General MIDI SoundFont. The
bundled bank is a host substitute for the original QuickTime instruments.
Select the demo directory in the launcher, or run the game from inside it. Its
OpenGL rendering, cockpitless view, joystick modes and axis layouts, and rumble
use the same launcher controls and command-line switches as WC1. EGA dithering
is WC1-only. In demo menus, use the D-pad to browse, A to confirm, B/Back to go
back or skip, and Start to pause. Escape and P toggle pause during gameplay;
Escape skips cutscenes, while P pauses them. Escape or Backspace goes back
from carrier panels; Backspace also skips scenes. Cmd+U and Cmd+N (Ctrl+U and
Ctrl+N on Windows and Linux) turn the demo's music and sound off or on, as in
its Options menu. Cmd+B/Ctrl+B toggles text messages (subtitles and room
labels); Cmd+V/Ctrl+V opens Volume Controls. These settings are remembered.

The macOS archives are ad-hoc signed, not notarized with an Apple developer
certificate. macOS may therefore require the usual confirmation for software
downloaded outside the App Store.

The corresponding source is available from the Git tag attached to the same
GitHub Release. See LICENSE.txt for the project license and
THIRD-PARTY-NOTICES.txt for bundled runtime components.
