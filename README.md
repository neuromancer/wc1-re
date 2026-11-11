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
- **Steam Deck / Linux (experimental):** in Desktop Mode, install the `.flatpak`
  with `flatpak install --user ./downloaded.flatpak` (replace the filename), then
  add **Wing Commander (Experimental)** to Steam as a non-Steam game. It has not
  been tested on Steam Deck yet; please report your results in
  [Issues](https://github.com/neuromancer/wc1-re/issues).
- **Portable ZIPs** for Windows, macOS, and Linux: extract and run, keeping the
  bundled files together.

Game data is not included. Open the app, choose a detected installation or use
**Add path…** to select the folder containing `GAMEDAT`, then select
**Start WC1**. The launcher remembers your folder and options.

- **Secret Missions:** with Kilrathi Saga data that includes the expansions,
  the launcher also offers **Start SM1** and **Start SM2**, and can transfer a
  pilot from an existing save with their statistics and awards.
- **Languages:** choose **Español**, **Français**, or **Japanese (FM Towns)**
  in the launcher's **Language** menu.
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
  WCAT-style four-button controls.
- Fixes to the 1996 release, such as correctly scaled planets, static on
  knocked-out cockpit displays, landing on the *Tiger's Claw* from any
  direction, and `Esc` pausing spaceflight.
- Sound effects balanced against the music and panned with the camera, and
  mouse wheel throttle control.
- Optional extras: an enhanced renderer that draws ships at full resolution
  while keeping the original artwork, cockpit, and HUD; EGA colours; the
  restored startup intro; a cockpitless view; and a cheat mode.

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
| Mouse wheel (spaceflight) | Increase or decrease speed |
| Gamepad Start (spaceflight) | Pause or resume |
| Gamepad Back | Escape/back |
| Gamepad Y (`Y/N` prompts) | Confirm Yes |

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
  AdLib emulation; and
- DaisenryakuFan for providing very advanced source reconstructions for the DOS
  releases of several *Wing Commander* titles.

## License

See [LICENSE](LICENSE). OpenAI Codex and Anthropic Claude were used during the
reconstruction.
