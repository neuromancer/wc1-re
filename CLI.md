# WC.EXE startup options

This document describes the startup-option parser in the Win32/Kilrathi Saga
version of `WC.EXE`. It is based on the reconstructed source in `src/` and the
original executable disassembly in `code-full/`; it does not rely on the DOS
version or on externally documented options.

## Key result: these are not Windows command-line options

The shipped Win32 executable ignores the `lpCmdLine` argument passed to
`WinMain` and calls its internal `main` function with `argc == 0`. Consequently,
arguments appended to `WC.EXE` at a Windows command prompt have no effect.

The option tokens are instead read from `WINGCMDR.CFG` in the game's working
directory. Tokens are separated by arbitrary whitespace, so they may be placed
on one line or on separate lines.

Evidence:

- `src/winmain.c`, `WinMain` at `0x00401E30`
- `code-full/FUN_00401E30.disassembled.txt`, especially the call at
  `0x00402012`
- `src/sound.c`, `LoadWingCmdrCfgFile` at `0x0042C580`
- `src/main.c`, `main` at `0x004274E0`

## Parser behavior and pitfalls

`WINGCMDR.CFG` is read twice.

1. `CheckLauncherAndConfig` reads every token before the game window is
   created. It recognizes `b`, `c`, `f`, `k`, `q`, and the `$#SAGA.EXE`
   launcher marker. For the five one-letter options it accepts an optional
   leading `-`.
2. `LoadWingCmdrCfgFile` loads the tokens for the main option parser. It returns
   one fewer token than it actually read, so the final token in the file is
   ignored by this second parser.

The last-token bug does not affect `b`, `c`, `f`, `k`, `q`, or `$#SAGA.EXE`,
because the first pass sees every token. For all other options, append an
otherwise unknown sentinel token. Unknown tokens are silently ignored. For
example:

```text
Origin w3 ignored
```

Other parsing details:

- `Origin` is exact and case-sensitive.
- Developer selectors `l`, `mN`, `sN`, and `wN` are lowercase and only take
  effect after an earlier `Origin` token, unless the registry `Cheater` flag has
  already enabled developer mode.
- `b`, `c`, `f`, `k`, and `q` are lowercase in the first parser.
- `a`, `e`, `p`, `r`, `t`, `v`, and `z` accept upper- or lowercase spellings.
- `asN` accepts either `s` or `S` as its second character.
- Numeric suffixes are parsed with `atoi`; a missing or invalid number becomes
  zero.

## General, gameplay, and debugging options

| Token | Effect |
| --- | --- |
| `Origin` | Enables developer options appearing later in the file. It also causes newly created or loaded pilots to receive the callsign `CHEATER`. It does not itself enable invulnerability or disabled collisions unless the registry `Cheater` flag was what enabled it. |
| `b` or `-b` | Disables the central collision handler whenever object 0, the player, is either participant. Most player collision detection and response is therefore suppressed. This is separate from special-purpose logic such as carrier landing. |
| `c` or `-c` | Clears the startup flag that otherwise assumes the normal preflight scenes established the flight display. This makes `RunSpaceFlight` allocate its view buffer and initialize the cockpit itself, and is required with direct `l` launch. |
| `f` or `-f` | Draws a floating-point frame-rate counter at the top-left of the screen. |
| `k` or `-k` | Makes the player ship invulnerable by returning immediately from damage processing when the victim is object 0. |
| `q` or `-q` | Bypasses the explicit DirectDraw display-mode cascade and renders through the primary surface path. Normally the game tries 320x200, 640x400, and then 640x480. This is not a normal windowed-mode switch. |
| `-m` | Enables the on-screen memory-status display: current near memory, current far memory, and original far memory. It does not require `Origin`. |
| `?` | Attempts to print the executable version through `SystemDebugPrintf`. That function is an empty `RET` in the retail executable, so bare `?` has no visible effect. |

The main parser redundantly gates dashed `-b`, `-f`, `-k`, and `-q` behind
`Origin`. In the actual Win32 startup path, the earlier configuration pass has
already applied those options unconditionally, so they do not really require
`Origin` when supplied through `WINGCMDR.CFG`.

Relevant implementation locations include `src/winmain.c` at
`CheckLauncherAndConfig`, `src/spc.c` in the central collision handler,
`src/hudmsg.c` in `RunSpaceFlight`, `src/screen.c` in the FPS overlay,
`src/ship.c` in `inflict_damage`, `src/dib.c` in DirectDraw initialization, and
`src/system.c` in `ShowMemoryStatusDebug`.

## Mission and scene-test options

| Token | Effect |
| --- | --- |
| `sN` | With developer mode enabled, selects campaign series/system `N` and marks campaign state as present. The default before parsing is series 1. |
| `mN` | With developer mode enabled, selects mission `N`. The default is mission 0. |
| `asN` | Forces the mission's starting action sphere/navigation point to `N`. It is not gated by `Origin`. Without this token, the normal mission-defined starting point is used. |
| `l` | With developer mode enabled, directly initializes the selected `sN`/`mN` mission, enters spaceflight, and exits after spaceflight returns. It bypasses the title, campaign rooms, briefing, and normal post-mission flow. |
| `wN` | With developer mode enabled, selects and runs internal scene-test entry `N`. The dispatcher recognizes entries 0 through 25, runs one entry, and then exits the game. It is a one-shot test dispatcher, not a continuous demo loop. |

### Important `wN` reconstruction correction

The current `src/main.c` reconstruction assigns the `wN` suffix to `mission`
and omits the call to the scene dispatcher. The original code at
`0x004277F5-0x00427809` instead stores a separate scene number and calls
`RunAnimationDemoLoop` at `0x004094E0`. That dispatcher always reaches
`exit_squadron` after the selected entry returns. Thus `wN` does not overwrite
`mN`, and it does not continue to the ordinary title/campaign loop.

The recovered dispatcher contains the following useful entries:

| Selector | Dispatched test |
| --- | --- |
| `w0` | Initializes a 3D scene, prepares the forward cockpit, runs the player death sequence, and waits for input. |
| `w1` | Initializes a 3D scene, prepares the forward cockpit, runs the ship-launch sequence, and waits for input. |
| `w2` | Runs the briefing for the selected `sN` and `mN`. |
| `w3` | Runs the finite scramble-hangar walking scene. It has no additional dispatcher-level input wait. |
| `w4` | Runs the scramble/boarding scene and then waits for input. |
| `w5` | Runs the internal post-flight/landing scene with variant 1, cleans up its 3D resources, and waits for input. |
| `w6` | Runs the internal post-flight scene with variant 3 and then waits for input. |
| `w7` | Calls the complete title/intro sequence. It eventually waits at the title menu. |
| `w8` | Enters the rec room, which is interactive. |
| `w9` | Empty dispatch slot; after the initial test-mode key gate, the game exits. This is useful as a startup/initialization smoke test but renders no test scene. |
| `w10` | Runs the debriefing for the selected `sN` and `mN`. |
| `w11` | Runs the player funeral sequence. |
| `w12` | Runs the internal medal/award scene using the current award selector. |
| `w13` | Enters the barracks, which is interactive. |
| `w14` | Runs post-flight scene variant 0 and waits for input. |
| `w15` | Enters the office scene. |
| `w16`, `w17` | Run the two special campaign-ending cinematic paths. |
| `w18`-`w21` | Run both outcome variants of campaign transition scenes 0 through 3, with an input wait between the variants and another after the second. |
| `w22` | Runs the alternate funeral sequence. |
| `w23` | Runs outcome variant 1 of campaign transition scene 4 and then waits for input. |
| `w24` | Runs both outcome variants of campaign transition scene 5, with an input wait between the variants and another after the second. |
| `w25` | Runs outcome variant 0 of campaign transition scene 6 and then waits for input. |

Several entries depend on valid campaign, mission, pilot, or award state and are
therefore less suitable as generic smoke tests. The mapping above describes the
dispatch, not a guarantee that every entry is valid against the reset/default
campaign state.

## Legacy audio and graphics/resource selectors

| Token | Effective Win32 behavior |
| --- | --- |
| `aN` | Selects legacy audio mode 2. Streamed music remains enabled. The numeric suffix is stored in `g_nArcadeStartupParameter_005a7b8a`, but no code in this executable reads it afterward. |
| `p` | Selects legacy audio mode 3, which suppresses streamed/background music. Sound effects are handled separately. |
| `r` | Selects legacy audio mode 1. Streamed music remains enabled; this Win32 executable does not select a separate Roland output backend. |
| `v` | Selects the normal `.VGA` resource family and normal cinematic timing. This is the default. |
| `e` | Selects the `.EGA` resource family and adds an extra timing tick to scene animation. |
| `t` | Selects the `.TGA`/Tandy resource family and uses the same slower scene timing as `e`. |
| `z` | Sets a flag that was already set immediately before parsing. The original executable contains writes but no reads of the flag, so this is a no-op here. |

The `A`, `P`, and `R` letters retain legacy DOS-era audio-selector intent, but
the effective Win32 distinction is the internal mode number described above.
Modes 1, 2, and the default mode 4 permit streamed music; mode 3 suppresses it.

The `V`, `E`, and `T` value is passed into the resource loader, which replaces
the first extension character of applicable install-table filenames. It also
controls the cinematic frame-timing adjustment.

## Launcher marker

`$#SAGA.EXE` is a launcher/configuration marker rather than a normal gameplay
option. That exact token enables nine additional Win95/Kilrathi Saga staff
credits after the original credit list.

## Practical automated-test paths

### Modern-port direct mission launch

The SDL launcher applies the original first-stage launcher options to host
arguments, then passes them to the recovered main module. This makes the
retained developer options available without editing `WINGCMDR.CFG`. The
`run-modern-mission` target supplies the exact original token sequence,
including the `c` needed to initialize the flight viewport and the
otherwise-required ignored final token:

```sh
make run-modern-mission SERIES=1 MISSION=0
```

`SERIES` uses the game's one-based series numbers and `MISSION` is zero-based.
The playable Vega-campaign pairs recorded in `CAMP.000` are series 1 with
missions 0-1, series 2-11 with missions 0-2, and series 12-13 with missions
0-3. This original developer path uses the default campaign data set
(`MODULE.000`); no original startup token selects `MODULE.001` or
`MODULE.002`.

The optional `NAV` value supplies the original `asN` starting-action-sphere
override. Additional original flags may be provided through `MISSION_FLAGS`:

```sh
make run-modern-mission SERIES=1 MISSION=0 NAV=2 \
    MISSION_FLAGS='k b f'
```

This target starts interactive spaceflight; it does not run an automated game
test. The more general spelling remains available as:

```sh
make run-modern MODERN_ARGS='c Origin s1 m0 l ignored'
```

There is no fully unattended combat-demo or recorded-input mode in this
executable. The title menu has no attract-mode timeout, direct mission launch
still processes player controls, and no startup option transfers the player
ship to AI control.

The closest built-in facility is `wN` scene-test mode. Before dispatching the
scene, `main` calls `WaitForKeyAcknowledge(0)`, whose Win32 implementation blocks
until a keyboard event is received. After that initial key, a finite entry such
as `w3` plays automatically and the dispatcher exits the process:

```text
Origin w3 ignored
```

This is the best built-in visual smoke test found so far: launch the game,
provide one key after the window appears, let the hangar animation finish, and
observe a clean automatic exit. `w9` is an even shorter initialization-only
smoke test, again requiring the initial key:

```text
Origin w9 ignored
```

For a manual flight test that bypasses all menus, use a valid series and mission
with `l`; adding `k`, `b`, and `f` makes repeated renderer/flight testing easier:

```text
c Origin s1 m0 l k b f ignored
```

That path is not automatic: it enters live spaceflight and continues to call
the normal player-input and flight-dynamics routines until the flight ends.
External window/input automation could supply the one key needed by `w3` or
`w9`, but the executable has no option that removes this initial key gate.
