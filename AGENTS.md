## Project

This repository holds the reconstructed source of **Wing Commander** as shipped in
*Wing Commander: The Kilrathi Saga* (1996), the Win32 port of the 1990 DOS game, and
an SDL2 port that runs it on modern systems.

- **The original game**: `src/*.c`, `src/*.cpp`, `src/*.asm`, `src/ix/` and the
  headers in `include/`. They rebuild `WC1.EXE` byte for byte with the original
  toolchain: Microsoft Visual C++ 4.20 under wibo, and UASM for the two hand-written
  assembly files. The retail executable is a debug build, so its `assert()`s, the
  MSVC debug heap and the `\\.\MONODEBG.VXD` channel are original behaviour, not
  artifacts to remove.
- **The SDL2 port**: the same sources compiled with `SDL_PORT` defined, together
  with the port's own code in `src/port/`.
- **Super Wing Commander**: `src/port/swc/`, a second engine for the Mac demo and
  the full game. It reuses the original game's code wherever the two games share
  logic, instead of copying it.

## Building and checking

    make                 # WC1.EXE, checked against the retail SHA-256
    make verify-hash     # the same check; it must print "WC1.EXE: OK"
    make modern          # the SDL2 port, out-modern/wc1-modern
    make modern-gui      # the SDL2 port with its Slint launcher
    make modern-swc      # the SWC demo viewer
    make check-fuzz      # the libFuzzer flight fuzzer (see the Makefile)
    make run-original    # the retail WC1.EXE in DREAMM
    make run             # the rebuilt WC1.EXE in DREAMM

Run `make verify-hash` after any change the Win32 build can see. Build `modern` and
`modern-gui` after any change to code the port compiles.

Game data, disc images, MT-32 ROMs and the retail executable are never committed;
they live in the ignored `data/`.

## The original game

`WC1.EXE` must stay byte-identical. The original files change only for the port
(inside `#ifdef SDL_PORT`) or for a fix that keeps the hash.

- Never split, merge, reorder or reformat an original file, and never add one; new
  code goes in `src/port/`. The link order fixes every address: the units in
  `SRCS_ORDERED_CORE` and the ix lists in the Makefile, and the functions inside
  each file, are in the original's order.
- Keep every `/* Function start: 0x... */` annotation on a function and every
  `/* 0x004xxxxx */` address comment on a global.
- CL 4.20's output depends on more than the code. Each declaration a unit sees
  takes symbol indices that break register-allocation ties, so adding, removing or
  moving a declaration in a header can change functions elsewhere
  (`include/wc1extern.h` keeps three uncalled prototypes for that reason). Put
  what the port adds to an original file or header inside `#ifdef SDL_PORT`, and
  let `make verify-hash` decide.
- The `ix_log_printf` calls in `src/ix/` pass the retail file names and line
  numbers as literals. Keep them.
- The game core is C, ported from 16-bit DOS code, so most game state is `short`;
  keep the types. Only `debug.cpp`, `pilot.cpp` and `sound.c` (compiled with
  `/TP`) are C++.
- `bin/extractRasterLibrary.py` generates `src/raster.asm` and holds its names and
  comments: change the script and regenerate rather than edit the file.
- Function names are the developers' own where the binary's strings state them;
  the others describe what the function does.

### Style of the original files

An `SDL_PORT` block follows the format of the file around it. The files share:

- 4-space indentation, no tabs, braces on the same line, one statement per line,
  and never a function body on the line of its signature.
- Struct members by field name; an unknown member is `field_<offset>`. No raw
  `*(short *)(p + 0xE)` where a field will do.
- Globals with Hungarian names and no `g_` prefix (`bEscapePressedFlag`,
  `aeObjectClass`); an unidentified one keeps `DAT_<address>`. Rename a symbol at
  every use; never add an alias `#define`.
- In `src/ix/`, class members are named without `this->`.
- Globals used by several units are declared in `include/globals.h` and functions
  in `include/wc1funcs.h`, both reached through `wc1.h`.

## The SDL2 port

Port code lives in `src/port/`, one directory per job, on the model of ScummVM's
structure. Port files may be split, merged and moved freely.

| Directory | Owns |
| --- | --- |
| `platform/` | Program entry and options, C runtime and Win32 stand-ins, timers, threads, settings (`wc1-modern.cfg`) |
| `graphics/` | Frame presentation, the OpenGL renderer, hi-res HUD text, Smooth spaceflight |
| `input/` | Host events, keyboard and mouse, joysticks and gamepads, rebinding tables |
| `audio/` | Mixing and sampled sound, OriginFX and AdLib, MT-32, speech |
| `game/` | What the port adds to WC1's data and behaviour: packet loading, data patches, campaigns and SM1.5, autosave, translations, the DOS intro, FM Towns data |
| `swc/` | The Super Wing Commander engine, for the Mac demo and the full game |
| `launcher/` | The Slint launcher (C++) |

- Original files reach the host only through port functions; SDL calls stay in
  `src/port/`.
- An `SDL_PORT` block in an original file is a call into the port, or a short
  behaviour fix that names its reason (usually an issue number). Longer logic
  belongs in `src/port/`.
- A port file keeps its state `static` and declares what it offers in a header
  beside it (`joystick.c`, `joystick.h`). It does not reach into another file's
  state. `include/wc1sdl.h` includes these headers for the original files;
  `platform/win32.h` holds the Win32 types and functions the game uses, and
  `game/original_globals.h` the original units' globals the port shares.
- A header compiles on its own: it includes what it uses and forward-declares
  the game's structs.
- Names: keep existing names. New port functions take the `Sdl` prefix and
  PascalCase, like the names around them.
- Format: as the original files.
- Headers: include guards named `SDL_PORT_<NAME>_H`. Include another area's header
  by its path from `src/port/` (`"graphics/video_internal.h"`).

## Comments

Keep comments clean and lean, as ScummVM does. This holds for port files and for
`SDL_PORT` blocks in original files.

- A comment says what the code does, or why, in a line or two. It is not a report:
  no measurements, levels, timings, investigations, alternatives tried or history.
  Those belong in commit messages and issues.
- Comment only what the code cannot say for itself. Point to an issue (`#46`)
  rather than retell it.
- A file's header says what the file is for, in a few lines.
- For example, not a paragraph of loudness measurements above a volume constant,
  but `/* The recordings are full scale; mix them 8 dB down. */`.

## Behaviour

- Same game, same behaviour: the port plays the same whichever release's data it
  runs (DOS, Kilrathi Saga, FM Towns); only the data formats differ. The Kilrathi
  Saga executable is the reference.
- Behaviour changes only when a change says so. A restructuring change carries no
  feature.
