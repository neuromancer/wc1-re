# Wing Commander reconstructed source

This is the reconstructed C and C++ source of **Wing Commander** as shipped in
*Wing Commander: The Kilrathi Saga* (1996). Built with Microsoft Visual C++
4.20, it reproduces the original Win32 `WC1.EXE` byte for byte.

Only the original Win32 build is shown: code that exists solely for the SDL2
port (`#ifdef SDL_PORT` blocks and the `src/sdl` tree) has been stripped.

- The files below are grouped by subsystem; each name opens its listing.
- **Files > Globals**, **Data Structures** and the search box find any
  function, variable or type; listings link each use to its definition, and
  each definition lists its callers.
- `/* Function start: 0x... */` comments give each function's address in the
  original executable.

Source: [github.com/neuromancer/wc1-re](https://github.com/neuromancer/wc1-re)

## Source files
