# Wing Commander source reconstruction and SDL2 port

This project recreates the source of **Wing Commander** as shipped in *Wing
Commander: The Kilrathi Saga* (1996). The rebuilt `WC1.EXE` matches the
original byte for byte.

It also provides a native port for Windows, Linux, and macOS that plays the
Kilrathi Saga and FM Towns versions, the original DOS release (in part), and the
Super Wing Commander Mac demo.

No original game data or Sega CD voice recordings are included.

If you find this project useful, you can [support its development through
GitHub Sponsors](https://github.com/sponsors/neuromancer).

## Screenshots

| Mission briefing | Tiger's Claw hangar |
| --- | --- |
| [![Mission briefing and flight path aboard the Tiger's Claw](screenshots/mission-briefing.png)](screenshots/mission-briefing.png) | [![Fighter parked in the Tiger's Claw hangar](screenshots/tigers-claw-hangar.png)](screenshots/tigers-claw-hangar.png) |

| Title sequence | Cockpit combat |
| --- | --- |
| [![Wing Commander title screen](screenshots/title-screen.png)](screenshots/title-screen.png) | [![Cockpit combat targeting a Dralthi](screenshots/cockpit-combat.png)](screenshots/cockpit-combat.png) |

## Download and play

Download the package for your platform from
[GitHub Releases](https://github.com/neuromancer/wc1-re/releases):

- **macOS 15 or later:** open the `.dmg` for Apple Silicon (`arm64`) or Intel
  (`x86_64`) and drag **Wing Commander** into **Applications**.
- **Windows:** run the `-setup.exe` installer, then open **Wing Commander**
  from the Start Menu.
- **Steam Deck / Linux:** in Desktop Mode, install the `.flatpak` with
  `flatpak install --user ./downloaded.flatpak` (replace the filename), then add
  **Wing Commander** to Steam as a non-Steam game. In Game Mode the launcher
  fills the screen, with its settings and controls in tabs and the joystick
  calibration in the same window.
- **Portable ZIPs** for Windows, macOS, and Linux: extract and run, keeping the
  bundled files together.

Game data is not included. Open the app, choose a detected installation or use
**Add path…** to select the game's folder: a Steam or GOG copy's main folder,
or any folder containing `GAMEDAT`. Then select **Start WC1**. The launcher
remembers your folder and options.

- **Secret Missions:** with Kilrathi Saga or DOS data that includes the
  expansions, the launcher also offers **Start SM1** and **Start SM2**, and can
  transfer a pilot from an existing save with their statistics and awards. Both
  releases play the expansions the same way, following the Kilrathi Saga, but
  SM2's captured Dralthi keeps its DOS disguise: in the stealth missions the
  Kilrathi ignore it until it fires on them. SM2's barracks poster can again
  be clicked for a closer look, as in the DOS release. With DOS data, SM2
  keeps its saved games in `GAMEDAT/CRUSADE.WLD`, as the DOS release does, so
  your DOS SM2 saves carry over.
- **Secret Missions 1.5:** the fans' port of *Super Wing Commander*'s Covert
  Missions plays on SM2. Put its four files (`CAMP.015`, `MODULE.015`,
  `BRIEFING.015` and `TITL15.VGA`, from either its DOS or its Kilrathi Saga
  package) in `GAMEDAT` beside SM2's, and the launcher offers **Start SM1.5**
  (`--campaign=sm15` on the command line). It keeps its saved games apart from
  SM2's, in `GAMEDAT/COVERT.WLD`: its patched executable shared SM2's file,
  where a game saved in one campaign loads with the other's missions. To go on
  with a game saved there, copy that file (`SAVEGAME.WLD`, or `CRUSADE.WLD`
  with DOS data) to `COVERT.WLD`. Its first briefing plays without the edited
  `MODULE.015` the Kilrathi Saga needs.
- **Languages:** choose **Español**, **Français**, or **Japanese (FM Towns)**
  in the launcher's **Language** menu.
- **German fan translation:** a DOS installation you have patched yourself
  with the German fan translation shows its German text, including the menus
  and cockpit messages it translates in `WC.EXE`. The launcher's **Language**
  menu then shows **German** only, and some artwork may stay in English.
- **Music and sound effects:** the launcher's **Music** and **Sound** menus
  choose each on its own. A *Kilrathi Saga* installation offers its own
  **Digital** music and effects, or the DOS ones on an **AdLib** or a
  **Roland MT-32**; a DOS installation offers the AdLib and the MT-32.
- **Roland MT-32:** the DOS music and effects can play on an emulated MT-32 or
  CM-32L. Put your own control and PCM ROM files in a folder, choose
  **Roland MT-32** under **Music** or **Sound** in the launcher, and select the
  folder with **MT-32 ROMs…**, which appears with it. The launcher names the
  machine it found, preferring the CM-32L. Music and effects on one MT-32
  share its channels as the original does. Without usable ROMs the AdLib
  plays instead. The command line takes `--music=default|adlib|mt32`,
  `--sound=default|adlib|mt32`, `--mt32-roms=DIR`, `--mt32-model=ID` (such as
  `mt32_1_07`) and `--mt32-check`, which reports what a ROM folder holds.
  Without `--sound`, the effects follow MT-32 music onto the MT-32.
- **FM Towns:** copy `WC.EXP` and the complete `GAMEDAT` folder from your CD
  into a writable folder and select it. For CD music, keep the disc image and
  place its CUE sheet beside `WC.EXP` as `TOWNS.CUE`.
- **Sega CD voices:** if you own the Sega CD recordings, put them in
  `Data/Voice` beside `GAMEDAT` to hear voiced WC1 dialogue and radio messages.
- **Super Wing Commander demo:** download the
  [Mac demo](https://archive.org/details/super-wing-commander-demo-1.0.2-mac),
  unzip it, and select its `SuperWing DEMO` folder.
- **Autosave:** the game saves to slot 8 after starting or loading a campaign
  and after each mission you survive; manual saves use slots 1–7. You can turn
  it off under **Gameplay**.

## Improvements

- Resizable window, fullscreen, and aspect-correct 4:3 presentation.
- Gamepad support, rumble, and alternative joystick layouts, including
  WCAT-style four-button controls and, for WC1, a Custom mode that rebinds
  every gamepad button, trigger and stick axis, and a flight stick's buttons,
  hats and axes, and a Steam Deck preset.
- An adjustable joystick dead zone (22% by default, as in the original) and a
  launcher page that calibrates WC1's joystick. It writes `GAMEDAT/j.cal` as
  the game's own calibration does, and the game now reads all of that file,
  not only the centre.
- Launcher tabs that show the gamepad, keyboard and mouse bindings for WC1
  and Super Wing Commander and let you rebind keys, mouse buttons and the
  wheel.
- Fixes to the 1996 release, such as correctly scaled planets, static on
  knocked-out cockpit displays, landing on the *Tiger's Claw* from any
  direction, and `Esc` pausing spaceflight.
- Sound effects balanced against the music and panned with the camera, and
  mouse wheel throttle control.
- A game speed choice: spaceflight runs as fast as its frame rate, so the
  launcher's **Speed** offers **Normal** (the *Kilrathi Saga*'s 20 FPS) and
  **Slow** (16 FPS). In flight, `[` and `]` (the two keys right of `P`) lower
  and raise the rate in half steps from 8 to 32 FPS, as the *Kilrathi Saga*'s
  `Alt+N` and `Alt+M` did; the launcher keeps that rate and offers it as
  **Custom**. The command line takes `--flight-fps=N`.
- Optional extras: an enhanced renderer that draws ships at full resolution
  while keeping the original artwork, cockpit, and HUD, and can also show the
  flight HUD's text at double resolution (launcher checkbox or `--hires-hud`):
  cockpit text in Trov's double-resolution cockpit font, for English, and
  radio messages in the game's larger 11-pixel font; EGA colours; the restored
  startup intro; a cockpitless view; and a cheat mode.

| Enhanced renderer | Enhanced cockpit combat |
| --- | --- |
| [![Enhanced renderer showing two ships at output resolution](screenshots/enhanced-space-objects.webp)](screenshots/enhanced-space-objects.webp) | [![Enhanced renderer during cockpit combat](screenshots/enhanced-cockpit-combat.webp)](screenshots/enhanced-cockpit-combat.webp) |

## Controls

| Shortcut | Action |
| --- | --- |
| `Cmd+Enter` (macOS), `Alt+Enter` (Windows and Linux) | Toggle fullscreen |
| `Cmd+Q` (macOS) | Quit the game |
| `Ctrl+S` / `Ctrl+M` (spaceflight) | Toggle sound effects / music |
| `Esc` (spaceflight) | Pause; press any key to resume |
| `[` / `]` (spaceflight) | Slow the game down / speed it up (the frame rate, 8 to 32 FPS) |
| Mouse wheel (spaceflight) | Increase or decrease speed |
| Gamepad RT / LT (spaceflight) | Same as A / B: fire guns / roll and speed (missiles in the WCAT layouts) |
| Gamepad Start (spaceflight) | Pause or resume |
| Gamepad Back | Cycle views in spaceflight; Escape/back elsewhere |
| Gamepad Y (`Y/N` prompts) | Confirm Yes |
| `J` (main menu, barracks), `Ctrl+J` (spaceflight) | Calibrate the joystick, as in the original |

The launcher's **Joystick** and **Keyboard and mouse** tabs show every
binding. Click a key or mouse input to change it, or **+** to add a second one;
changes apply when the game starts. `Esc`, and Super Wing Commander's menu
shortcuts and in-flight computer keys, cannot be changed.

In the Joystick tab's **Custom** control mode, press an input on the
controller, or click it on the gamepad drawing, to rebind it for WC1's
spaceflight, and point at a choice to see what it does. A button or trigger can
take any flight key's action, cycle the views, or turn yaw into roll and pitch
into speed while held. A stick axis or trigger can pitch, yaw, roll or change
speed, and a stick axis can also set the throttle; any of these can be reversed.
Custom starts from the WCAT four-button, four-axis layout. Menus keep their
fixed buttons, and Super Wing Commander keeps the WCAT four-axis layout.

The **Steam Deck** control mode (`--joystick-mode=steam-deck`) follows the
layout shared in [#32](https://github.com/neuromancer/wc1-re/issues/32): roll
and speed on the left stick, yaw and pitch on the right, and the D-pad on the
navigation map, damage display, communications and target lock. The Deck's
rear buttons mirror the D-pad: L4 up, L5 down, R4 left and R5 right; with
Steam Input, give those directions to the rear buttons in Steam's controller
layout. A is left free, as WC1 has no jump drive. Super Wing Commander keeps
the WCAT four-axis layout in this mode too. In Custom, **Source** picks
**Automatic**, **Gamepad**, **Steam Deck** or **Joystick**: Automatic follows
the connected device, Gamepad and Steam Deck show editable drawings (the Deck's
rear buttons share the D-pad's bindings), and Joystick lists numbered inputs
even for a device SDL knows as a gamepad. Choosing the preset keeps the Custom
bindings.

A joystick that SDL does not know as a gamepad, such as a flight stick, is
shown as its axes, hats and buttons by number, each lit while held, with the
axes named where the system describes them (X axis, Z rotation, slider, rudder
and so on). Custom rebinds its first 32 buttons, two hats and eight axes, and
starts with the stick steering, a twist or rudder taking the other turn and its
throttle lever setting the speed. Elsewhere its buttons stand in for the
gamepad's: 1 for A, 2 for B, 3 for X, 4 for Y and 9 for Back, with its first
hat as the D-pad.

As in the original, a mission ends only by landing on the *Tiger's Claw*,
ejecting, or dying; quit from the barracks menu.

## Build from source

```sh
git submodule update --init --recursive
make -j modern-gui
```

This needs Python 3.10+, a C/C++ compiler, the SDL2 and LZO2 development
packages, CMake, Rust, and a C++20 compiler; the game is written to
`out-modern/`. `make -j` rebuilds the original `WC1.EXE`.

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
- [Greg14](https://www.wcnews.com/chatzone/members/greg14.11738/) for the French
  translation, adapted with permission; [provenance](translations/fr/provenance.json);
- Aaron Giles for the [DREAMM emulator](https://dreamm.aarongiles.com/), used
  to run and study the original release, and
  [ymfm](https://github.com/aaronsgiles/ymfm), which powers the SDL2 port's
  AdLib emulation;
- the [Munt project](https://github.com/munt/munt), whose libmt32emu
  (LGPL-2.1 or later) emulates the Roland MT-32 and CM-32L;
- Trov for the [double-resolution cockpit font](third_party/double-resolution-cockpit-font/README.md)
  (CC BY 4.0) that the hi-res HUD shows cockpit text in; and
- DaisenryakuFan for providing very advanced source reconstructions for the DOS
  releases of several *Wing Commander* titles.

## License

See [LICENSE](LICENSE). OpenAI Codex and Anthropic Claude were used during the
reconstruction.
