# Wing Commander source reconstruction and SDL2 port

This project recreates the source of **Wing Commander** as shipped in *Wing
Commander: The Kilrathi Saga* (1996). The reconstructed game core is C, the
`ix` audio library is C++, and the reference build uses Microsoft Visual C++
4.20 to reproduce the original Win32 executable.

A native SDL2 port is available for Windows, Linux, and macOS. It supports
Kilrathi Saga and FM Towns data, has partial support for the original DOS game
data, and also runs the Super Wing Commander Mac demo.

No original game archives or Sega CD voice recordings are included.

If you find this project useful, you can [support its development through
GitHub Sponsors](https://github.com/sponsors/neuromancer).

## Status

The reconstruction has reached **100% executable similarity**: the rebuilt
`WC1.EXE` matches the original byte for byte (all 518,656 bytes, SHA-256
`d517169e57f1d6254682c03dec5c254b5101ccd787d191f639b0cac1e8a616c4`).
A few matching workarounds remain in the source:

- one compilation unit (LOGIC) still selects its own header context;
- a four-instruction inline assembly block in the guarded memory allocator;
- some `volatile` qualifiers and access casts; and
- header declaration order and include placement chosen to reproduce the
  compiler's internal symbol ordering, plus three prototypes that nothing
  calls but that ordering still needs.

## Screenshots

| Mission briefing | Tiger's Claw hangar |
| --- | --- |
| [![Mission briefing and flight path aboard the Tiger's Claw](screenshots/mission-briefing.png)](screenshots/mission-briefing.png) | [![Fighter parked in the Tiger's Claw hangar](screenshots/tigers-claw-hangar.png)](screenshots/tigers-claw-hangar.png) |

| Title sequence | Cockpit combat |
| --- | --- |
| [![Wing Commander title screen](screenshots/title-screen.png)](screenshots/title-screen.png) | [![Cockpit combat targeting a Dralthi](screenshots/cockpit-combat.png)](screenshots/cockpit-combat.png) |

## Download and run the SDL2 port

Download the package for your platform from
[GitHub Releases](https://github.com/neuromancer/wc1-re/releases):

- **macOS:** open the `.dmg` for Apple Silicon (`arm64`) or Intel (`x86_64`)
  and drag **Wing Commander** into **Applications**. Requires macOS 15 or later.
- **Windows:** run the `-setup.exe` installer, then open **Wing Commander**
  from the Start Menu. It installs for the current user without administrator
  privileges and includes an uninstaller in Windows Settings.
- **Portable ZIPs:** available for Windows, macOS, and Linux. Extract the
  archive and keep the bundled libraries, sound bank, `translations/`, and
  `voices/` mapping beside the executable.

Game data is not included. Open the app to choose a detected installation, or
use **Add path…** to select a Kilrathi Saga, DOS, or FM Towns Wing Commander
directory containing `GAMEDAT`. Choose the graphics and joystick options, then select
**Start game**. Kilrathi Saga installations
also need `STREAMS/PREFLITE.STR`, `POSFLITE.STR`, and `MISSION.STR`. With DOS
data, compressed resources, OriginFX/AdLib music, and synthesized sound effects
work; other DOS-specific data or behavior may not. Starting the game saves the
directory and options for the next session. Enable **Start in fullscreen** under
**Graphics** to open the game in desktop fullscreen mode.

With command-line arguments the game starts directly from the working
directory; add `--gui` to review the options in the launcher first:

```sh
cd /path/to/WC1
./wc1-modern --enhanced --joystick-rumble
```

For Spanish, choose **Language → Español** or use `--language=es`. The launcher
switches immediately; WC1 and Secret Missions support Spanish with DOS or
Kilrathi Saga data. Super Wing Commander remains English.

**Sega CD voices:** supply your own recordings in `Data/Voice` beside `GAMEDAT`,
or keep WingLoader's original `Games/WC1/GAMEDAT` and `Data/Voice` layout. A
`WingLoader/` folder inside the game directory also works. Detected installations
show **Sega CD voices** in the game list. Only the PO filename mapping ships;
recordings and WingLoader scripts are never bundled. Currently covers mapped WC1
dialogue with approximate lip sync, plus in-flight radio with rank variants.
Endings, dynamic rank/kill variants in scenes, and Secret Missions remain unsupported.

Use `--fullscreen` or `--windowed` to choose the startup display mode. With
`--gui`, these also override the saved fullscreen setting in the launcher.

**Autosave (slot 8)** under **Gameplay** defaults to on. It saves after starting
or loading a campaign and after surviving missions; death and endings preserve
the previous checkpoint. Manual saves use slots 1–7; load autosaves from the last
bunk. Declining **Overwrite autosave?** preserves that save and disables autosave
until returning to the title screen or restarting. Use `--autosave` or
`--no-autosave` to override the setting.

The Super Wing Commander Mac demo supports the same options except EGA
dithering and autosave (SWC saving/loading is not implemented yet). To play it,
download the zip from
[archive.org](https://archive.org/details/super-wing-commander-demo-1.0.2-mac), unzip it,
and select its `SuperWing DEMO` folder in the launcher. Its Options commands keep
their original keys: `Cmd+U`/`Cmd+N` (`Ctrl+U`/`Ctrl+N` on Windows and Linux) turn
music and sound off or on, `Cmd+B`/`Ctrl+B` toggles text messages, and
`Cmd+V`/`Ctrl+V` opens Volume Controls.

### FM Towns data

Extract `WC.EXP` and the complete `GAMEDAT/` folder from your original FM Towns
CD into a writable game folder. Preserve the subdirectories, including `S1/`
and `S2/`, and all `.FMP` sound files. Select the folder containing `WC.EXP` in
the launcher; choose **Japanese (FM Towns)** or use `--language=ja` for Japanese
text. Towns OS files and a font ROM are not needed.

For CD music, also keep the original raw `.bin`/`.img` disc image and its CUE
sheet. Place the sheet beside `WC.EXP` as `TOWNS.CUE` (or set `WC1_TOWNS_CUE`
to its path), keeping its `FILE` entries pointed at the image files.

### Fixes and features

The first group is always active; the rest need a switch, and switches can be
combined.

| Fix or feature | Enable with |
| --- | --- |
| Runs from Kilrathi Saga, DOS, or FM Towns game data, detected automatically | always on |
| Resizable window, fullscreen toggle, and mouse capture | always on |
| Aspect-correct 4:3 presentation and pointer mapping | always on |
| Camera-relative left/right panning for in-flight sound effects | always on |
| Kilrathi Saga sound effects balanced against streamed music | always on |
| Background planets drawn with correct per-object scaling (WCDX fix) | always on |
| Static on knocked-out cockpit displays, which the 1996 port dropped | always on |
| Landing on the *Tiger's Claw* from any bearing; retail also required the Claw's bow to face you | always on |
| Mouse wheel throttle control during spaceflight | always on |
| `Esc` pauses during spaceflight, where the original left it inert | always on |
| Pointer confined only during unpaused, focused spaceflight | always on |
| Gamepad support with automatic SDL mappings and hot-plug | always on |
| Start in fullscreen | GUI checkbox or `--fullscreen` |
| Automatic campaign checkpoints in reserved save slot 8 | GUI checkbox or `--autosave` |
| OpenGL renderer drawing space objects at output resolution | `--enhanced` |
| EGA 16-colour dithering | `--ega` |
| Restored, music-synchronized startup intro | `--enhanced`, or automatic with DOS data |
| OriginFX intro music | `--enhanced`, or automatic with DOS data |
| Heavy-weapon, damage, collision, and afterburner rumble | `--joystick-rumble` |
| WCAT-style four-button joystick layouts | `--joystick-mode=4button-2axis` or `4button-4axis` |
| Alternate twin-stick, HOTAS, throttle, and rudder axis layouts | `--joystick-axes=<layout>` |
| Joystick diagnostics on stderr | `--joystick-debug` |
| Cockpitless view | `-c` |
| Cheat mode: player invulnerability and no player collisions | GUI checkbox or `--cheats` |

The enhanced renderer keeps the original indexed artwork, palettes, cockpit,
and HUD; only space objects are redrawn at output resolution.

| Output-resolution ships | Cockpit combat |
| --- | --- |
| [![Enhanced renderer showing two ships at output resolution](screenshots/enhanced-space-objects.webp)](screenshots/enhanced-space-objects.webp) | [![Enhanced renderer during cockpit combat](screenshots/enhanced-cockpit-combat.webp)](screenshots/enhanced-cockpit-combat.webp) |

### Controls

| Shortcut | Action |
| --- | --- |
| `Cmd+Enter` (macOS), `Alt+Enter` (Windows and Linux) | Toggle fullscreen |
| `Cmd+Q` (macOS) | Quit the game |
| `Ctrl+S` / `Ctrl+M` (spaceflight) | Toggle sound effects / music |
| `Esc` (spaceflight) | Pause; press any key to resume |
| Mouse wheel (spaceflight) | Increase or decrease speed |
| Gamepad Start (spaceflight) | Pause or resume |
| Gamepad Back | Escape/back |
| Gamepad Y (`Y/N` prompts) | Confirm Yes |

As in the original, a mission ends only by landing on the *Tiger's Claw*,
ejecting, or dying; quit from the barracks menu.

The default `--joystick-mode=original` keeps the original two-axis, two-button
controls. The WCAT-style layouts use buttons 1–4 (A, B, X, Y on mapped
controllers) for fire, missile, afterburner, and either a roll/throttle
modifier (`4button-2axis`) or target cycling with extra roll and throttle axes
(`4button-4axis`). LB/RB cycle guns and missiles, LT/RT open the navigation map
and engage autopilot, and the D-pad provides full speed, stop, communications,
and target lock. `--joystick-axes=` selects `twin-stick-roll` (the default),
`twin-stick-yaw`, `hotas-yaw`, `hotas-roll`, `linear-throttle`, `rudder-yaw`, or
`rudder-roll`.

## Build from source

Clone the submodules first:

```sh
git submodule update --init --recursive
```

### SDL2 port

Install Python 3.10+, a C/C++ compiler, and SDL2/LZO2 development packages, then run
`make -j modern`. The executable is written to `out-modern/wc1-modern` (`.exe`
on Windows); `make run-modern` launches it with Kilrathi Saga data in
`data/full`, and `make run-modern-dos` with DOS data in `data/dos`.

The graphical launcher (`make -j modern-gui`) also needs CMake 3.21+, Rust
1.92+, and a C++20 compiler; Linux builds need the D-Bus and Fontconfig
development packages. It produces `out-modern/wc1-modern-gui`, which release
archives ship as `wc1-modern`.

### Translation and voice mappings

Edit `translations/es/*.po` or `translations/ja/ui.po` for the game,
`translations/es/LC_MESSAGES/launcher.po` for the GUI, and `voices/segacd.po`
for voice filenames. Preserve contexts, IDs,
format directives and `$C`-style substitutions. Empty/fuzzy entries use the
original text or remain unvoiced. Game IDs contain no original English prose.
Use `_()`/`C_()` in C, `N_()`/`NC_()` for static strings, and `@tr()` in Slint.
The reference build keeps the original English expressions.

```sh
make update-translations update-launcher-translations # needs gettext and slint-tr-extractor
make modern modern-gui                               # compiles mappings; no recordings needed
make check-translations MODERN_RUN_DIR=/path/to/WC1
make check-launcher check-voices                     # synthetic GUI/audio checks
```

For debugging, `python3 bin/importWc1Spanish.py --english /path/to/GAMEDAT --debug-english`
exports English strings to ignored `out/english-strings.json`; do not commit it.
For recordings outside the installation, compile a local index with
`python3 bin/compileVoices.py --voice-root /path/to/Data/Voice --output out-modern/voices.dat`
and launch with `--voices=/absolute/path/to/voices.dat`.

### Release packaging

Build with `MODERN_RELEASE=1 MODERN_OUT_DIR=out/release` to keep optimized release
objects separate from development builds. Packages contain only executables,
runtime dependencies, mappings and notices; copyrighted recordings stay external.

```sh
make modern-gui MODERN_RELEASE=1 MODERN_OUT_DIR=out/release
packaging/package-modern.sh VERSION PLATFORM ARCH out/release/wc1-modern-gui
packaging/package-macos.sh VERSION ARCH out/release/wc1-modern-gui
packaging/package-windows.sh VERSION x86_64 dist/wc1-re-VERSION-windows-x86_64-sdl2
```

Use the `.exe` suffix on Windows. macOS packaging needs Xcode command-line tools,
dependencies targeting macOS 15 (`packaging/build-macos-deps.sh`), and the Python
packages in `packaging/requirements-macos.txt`; `DMGBUILD` can select its executable.
Windows installers need NSIS 3.13 (`MAKENSIS` overrides its path). Outputs go to
`dist/`; `DIST_DIR` overrides it. macOS apps are ad-hoc signed, not notarized;
Windows installers are unsigned. The release workflow provides platform build
environments and publishes only on release tags.

### Reconstructed Win32 build

`make -j` builds `WC1.EXE` with the original MSVC 4.20 toolchain under wibo and
assembles the hand-written raster library with UASM. Each link checks the
executable against the original SHA-256; `make verify-hash` repeats the check.
The reference release can replace `WC1.EXE` in a Kilrathi Saga installation;
back up the existing executable first. It requires compatible 32-bit Windows.
To run it in an emulated Windows 95 environment through DREAMM, provide a
Kilrathi Saga disc image:

```sh
make run GAME_ISO=/path/to/kilrathi-saga.iso
```

### Reconstruction workflow

[`binary-comp`](https://github.com/gg-sl-oss/binary-comp) is required for the
assembly and layout comparisons. These commands also need the retail executable
at `data/full/WC1.ORI.EXE` and the original-code exports under `code-full/`:

```sh
python3 -m pip install "binary-comp[all] @ git+https://github.com/gg-sl-oss/binary-comp.git"
make compare-func FUNC=perform_maneuver
make verify
```

## Acknowledgements

Special thanks to:

- [Origin Systems](https://en.wikipedia.org/wiki/Origin_Systems)
  who created *Wing Commander* and developed its *Kilrathi Saga* port;
- [Electronic Arts](https://www.ea.com/) for publishing *Wing Commander: The
  Kilrathi Saga*;
- [GOG](https://www.gog.com/en/game/wing_commander_1_2) for keeping *Wing
  Commander 1 + 2* readily available;
- AllTinker for the [W.C.A.T. overhaul](https://alltinker.itch.io/wcat), whose
  fixes and analysis of the DOS release have been valuable references;
- the [WCDX project](https://github.com/Bekenn/wcdx) for its pioneering
  compatibility work on the *Kilrathi Saga* release;
- L@Zar0 and darkpadawan for the Spanish translation v1.1; its
  [original terms](translations/upstream/Leeme.txt) specify GPLv3 with an additional
  no-charge condition. [Provenance](translations/provenance.json) records the import;
- WingLoader v0.91 for the initial associations between dialogue and recording filenames;
- Aaron Giles for the [DREAMM emulator](https://dreamm.aarongiles.com/), used
  to run and study the original release, and
  [ymfm](https://github.com/aaronsgiles/ymfm), which powers the SDL2 port's
  AdLib emulation; and
- DaisenryakuFan for providing very advanced source reconstructions for the DOS
  releases of several *Wing Commander* titles.

## License

See [LICENSE](LICENSE). OpenAI Codex and Anthropic Claude were used during the
reconstruction.
