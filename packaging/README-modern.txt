Wing Commander SDL2 port
========================

This archive contains the work-in-progress native SDL2 port. AddressSanitizer
and UndefinedBehaviorSanitizer are always enabled, including in releases, so a
detected memory error deliberately terminates the program with a diagnostic.

No copyrighted game data is included. Extract every file in this archive into
an installed Kilrathi Saga or DOS WC1 directory, then start wc1-modern
(wc1-modern.exe on Windows) from that directory. Keep the bundled runtime
libraries beside the executable.

The DOS data path supports the original compressed packet resources and plays
OriginFX music and synthesized sound effects through an embedded YM3812
(AdLib) emulator using MUSIC.MID and WINGLDR.TIM. No external MIDI synthesizer
or sound bank is required. DOS game-data support remains partial.

The macOS archives are ad-hoc signed, not notarized with an Apple developer
certificate. macOS may therefore require the usual confirmation for software
downloaded outside the App Store.

The corresponding source is available from the Git tag attached to the same
GitHub Release. See LICENSE.txt for the project license and
THIRD-PARTY-NOTICES.txt for bundled runtime components.
