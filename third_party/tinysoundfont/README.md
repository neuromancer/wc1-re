# TinySoundFont

`tsf.h` and MIT license from
[schellingb/TinySoundFont](https://github.com/schellingb/TinySoundFont/tree/853a0a171759f1ddba0de1442133a75912bbeffa),
commit `853a0a171759f1ddba0de1442133a75912bbeffa`.

SHA-256 of upstream `tsf.h`:
`70d55963c98f60ebb81518eaa1f25d46888d5180eb5f5289fd6b74ffc177d197`.

Only `src/sdl/swc_music.c` compiles the implementation. It renders decoded
QuickTime note sequences with the bundled GeneralUser bank. The reference
Win32 build and WC1's existing SDL music backends do not use this synthesizer.

Local fix: perform the coarse sample-offset shift on an unsigned value.
SoundFonts can contain negative offsets; shifting a negative signed integer
is undefined in C and trips UBSan while loading the bundled bank.
