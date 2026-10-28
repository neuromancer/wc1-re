# Compilation-unit order

MSVC emits functions in source order and the linker concatenates objects in
command-line order. Use `src/map` and `make sort`; uncertain core boundaries
remain provisional.

```text
0x00401000  game core and Win32 support (command-line objects)
0x00439C40  raster library (raster.obj, assembled), then killbrd.obj and gr.obj
0x004426A0  ix_log_printf (ixlog.obj, the last command-line object)
0x00442750  ix library members
0x004492C6  DirectSound and DirectDraw call thunks
0x004492E0  static debug CRT, first search pass (not reconstructed)
0x004614B0  RtlUnwind call thunk, second search pass
0x004614C0  LZO decoder, then the ix file module at 0x00461630
0x004617C0  CRT members reached through OLDNAMES, second search pass
```

## Proven `ix` order

| Range | Source | Library |
| --- | --- | --- |
| `0x00442750`–`0x00443B0F` | `src/ix/streamer.cpp` | streamer |
| `0x00443B10`–`0x004446FF` | `src/ix/thread.cpp` | streamer |
| `0x00444700`–`0x0044490F` | `src/ix/streamfile.cpp` | streamer |
| `0x00444910`–`0x0044518F` | `src/ix/dsp.cpp` | dsp |
| `0x00445190`–`0x00445F5F` | `src/ix/dsps.cpp` | dsp |
| `0x00445F60`–`0x0044679F` | `src/ix/mixer.cpp` | dsp |
| `0x004467A0`–`0x00446F73` | `src/ix/dspv.cpp` | dsp |
| `0x00446F74`–`0x004471FF` | `src/ix/dspmix.asm` | dsp |
| `0x00447200`–`0x00447A1F` | `src/ix/system.cpp` | sound |
| `0x00447A20`–`0x004486AF` | `src/ix/sound.cpp` | sound |
| `0x004486B0`–`0x004492C5` | `src/ix/sample.cpp` | sound |
| `0x004614C0`–`0x0046162F` | `src/ix/lzo1x.cpp` | lzo |
| `0x00461630`–`0x004617BF` | `src/ix/ixfile.cpp` | file |

Diagnostic source-file anchors establish function order, but the first
diagnostic in a file need not occur in its first function. Linker INT3 fill
establishes these object boundaries:

| Fill bytes | Following object |
| --- | --- |
| `0x00443B09`–`0x00443B0F` (7) | `thread.cpp`, beginning with `ix_streamer_thread_proc` |
| `0x004446F6`–`0x004446FF` (10) | the six `IxStreamFile` methods in `streamfile.cpp` |
| `0x00447A19`–`0x00447A1F` (7) | `sound.cpp`, beginning with `IxSound::ix_system_sound_init` |
| `0x004486A6`–`0x004486AF` (10) | `sample.cpp`, beginning with `IxSample::ix_sample_construct` |

The stream-file object's original filename is not present in the diagnostic
strings. `streamfile.cpp` describes its contents. The unaligned entries at
`0x00443DA6`, `0x00447CD8`, and `0x0044879C` are interior functions, not object
starts. Keeping them in their preceding groups lets the compiler and linker
produce the observed boundaries without explicit padding.

The sample object's final `RET 4` occupies `0x004492C3`–`0x004492C5`.
Its six functions span 3,094 bytes. The following three six-byte import
stubs belong to DirectSound and DirectDraw, followed by eight INT3 bytes
before the CRT at `0x004492E0`; they are not part of `sample.cpp`.
See `docs/EXPORT.md` for the report's handling of these import boundaries.

The retail image also has INT3 fill at `0x00445181`–`0x0044518F` before the
first initializer pair and at `0x00446795`–`0x0044679F` before the second pair.
The pairs begin `dsps.cpp` and `dspv.cpp`, respectively: a 21-byte `$E2`
calls a 16-byte `$E1`, followed immediately by the first ordinary function.
VC++ 4.20 emits these exact 37 bytes for each global array of non-aggregate
state records, even when no explicit constructor exists. Private fields with
the existing free functions as friends reproduce this property. This is a
matching declaration choice, not evidence of the original access-specifier
spelling. Public aggregate records omit the pair; explicit empty constructors
add calls and loop code absent from the executable.

The compiler also emits the two `.CRT$XCU` entries, at original addresses
`0x00465008` and `0x0046500C`, in stream-then-voice order. The handwritten
stand-ins formerly placed at the ends of `dsp.cpp` and `mixer.cpp` did not
register these initializers. `make verify-array-initializers` compares both
complete pairs with binary-comp and checks their registration order and base
relocations. Their original boundaries remain in the recovered map because
compiler-generated functions have no source annotations.

The voice-control group ends with `ix_dspv_recalc_mix`'s `RET` at
`0x00446F70`, followed by three INT3 bytes. The separately assembled mixer
starts at `0x00446F74` and ends at `0x004471FE`, followed by one INT3 byte.
Its shift encodings independently identify MASM output (see
`docs/COMPILER.md`); it is a separate member after `dspv.obj` in `dsp.lib`.

The DSP stream array starts at `0x00598138`. The voice array immediately
after it starts at `0x005981A8`, leaving 112 bytes: exactly two 56-byte
`IxStream` records. This agrees with the stream count of two set by
`ix_dsp_init`; the former eight-entry reconstruction overallocated 336 bytes.

The two stream counters and array occupy a 120-byte run in the original:
count at offset zero, allocated count at four, and the array at eight.
Renaming the allocation counter from `nStreamsAllocated` to
`nAllocatedStreams` reproduces those offsets and the complete 120-byte
`dsps.obj` `.bss`. The former spelling placed the allocation counter at
`0x78`, after the array, producing 124 bytes. Moving both counters into
`dsp.cpp` was an unsuccessful earlier probe: it produced 1,080 DSP bytes
and a 112-byte stream section, with incorrect counter positions. Both
counters remain in the stream-control unit.

DSP state also constrains data ownership. The initialized master-volume word
at `0x0047198C` contains `0xFFFF` and precedes the allocator callbacks at
`0x00471990` and `0x00471994` in `dsp.cpp`. Its `asPanGains` table occupies
`0x00597D28`–`0x00598127`, immediately before the DSP tick and primary-buffer
pointer. The voice array, mixer critical section, voice count, and allocated
voice count occupy `0x005981A8`–`0x00598607` in `dspv.cpp`. These definitions,
along with `dsps.cpp`'s stream state, are included in the global-data audits.

The following seven sound-system words occupy `0x00598608`–`0x00598623`.
Their identities follow from the original initialization and sound/sample
list operations. Their relative positions now match in `system.obj`:

| Global | Original and rebuilt block offset |
| --- | --- |
| `dwSoundStateFlags` | `0x00` |
| `pIdleSoundList` | `0x04` |
| `pSamples` | `0x08` |
| `nActiveVoices` | `0x0C` |
| `nSystemVoices` | `0x10` |
| `pPlayingSoundList` | `0x14` |
| `pQueuedSoundList` | `0x18` |

VC++ 4.20's uninitialized-data ordering depends on identifier spelling.
Fresh isolated compilations reproduce the former wrong order with no
function bodies, reversed function order, and reversed definitions.
Changing the names changes COFF offsets, including in a minimal file with
only forward-declared record types and the seven storage definitions.
The assembly listing prints declaration order and does not establish the
actual storage layout. These symbols are section definitions, not linker
common symbols. Earlier linkage, owner-extern suppression, private-flag,
and single free-list rename probes did not recover the block; explicit
zero initializers changed storage to `.data` and were also rejected.

Six descriptive system-global renames recover the seven-word order.
Renaming `voices` to `aVoices` places the 34-record voice array before
`csMixer` and the two voice counters. Seven DSP-global renames recover its
handle, buffer, pan-table, and tick order, including four compiler-produced
alignment bytes before the pan table. No flags, types, initializers,
control flow, or storage owners change. The spellings are matching
reconstruction choices; the developer's original identifiers are unknown.

`make verify-unit-data` checks these four complete `.bss` contributions:

| Object | Original start | Size |
| --- | --- | --- |
| `ix/dsp.obj` | `0x00597D00` | 1,072 |
| `ix/dsps.obj` | `0x00598130` | 120 |
| `ix/dspv.obj` | `0x005981A8` | 1,120 |
| `ix/system.obj` | `0x00598608` | 28 |

The gate verifies all 26 named positions, object section sizes and storage
flags, empty relocation sets, and loader-visible zeros through binary-comp.
It also verifies the complete adjacent 2,340-byte run with one permitted
address shift. This establishes matching relative storage in the current
owners, not original identifier spelling or final executable placement.

The assert anchors name three source trees, `D:\Rnd\Prj\ix\Src\Streamer\`,
`D:\rnd\prj\ix\win95\dsp\` and `D:\Rnd\prj\ix\src\sound\`; each was a separate
library. `src/ix/ixlog.cpp` is the one ix unit linked as an object.

The logger ends at `0x00442738`, followed by seven compiler padding bytes.
The aligned entry at `0x00442740` returns a 32-bit one, then has ten bytes
of compiler padding before the first streamer function. It has no known
references or naming evidence; `ixlog.obj` is only its provisional owner.
The recovered boundary map keeps this unimplemented function separate from
the logger, whose own bytes match with the core optimizer profile.

## Link line

LINK 4.20 resolves symbols in the order they are first referenced: by each
command-line object in turn, then the entry point, then each library member in
the order it is pulled in. Library members are laid out in pull order, and a
library is searched again on later passes when new references appear. The CRT
member order, the call thunks and the import tables therefore record the link
line. The reference build reproduces all of them:

| Evidence in the original | Link-line choice |
| --- | --- |
| Import descriptors run KERNEL32, USER32, GDI32, ADVAPI32, WINMM, DSOUND, DDRAW | Import libraries in that order |
| DirectSound and DirectDraw thunks follow `sample.cpp`, DSOUND first | `dsound.lib`, `ddraw.lib` after the ix libraries |
| `wincrt0.obj` sits between `strncmp.obj` and the CRT members only ix needs | No `/ENTRY`; LINK adds the entry point after the objects |
| `_CrtDbgReport` and `strncmp`, needed first by `ix_log_printf`, precede `wincrt0.obj` | `ixlog.obj` on the command line, the other ix units in libraries |
| ix members appear streamer, thread, stream-file, dsp, dsps, mixer, dspv, system, sound, sample | Streamer, dsp and sound libraries searched in that order |
| LZO decoder after the second-pass `RtlUnwind` thunk, then INT3 padding and the file module, before the OLDNAMES members | The two in libraries of their own, decoder first, after `kernel32.lib` and before the other ix libraries |
| `strupr`, `xtoa`, `flength`, `cprintf`, `chdir` are pulled on the second pass | The game calls the old names, resolved through OLDNAMES |

With these choices every import table matches exactly, and every CRT member
sits at one constant offset from the original.

## Core placement windows

| Source | Range |
| --- | --- |
| `src/winmain.c` | `0x401000`–`0x402E7F` |
| `src/winsys.c` | `0x402E80`–`0x4034FF` |
| `src/mono.c` | `0x403500`–`0x403E4F` |
| `src/auto.c` | `0x403E50`–`0x40469F` |
| `src/eject.c` | `0x4046A0`–`0x404CCF` |
| `src/cmpgn.c` | `0x404CD0`–`0x40609F`; the face parser's exact owner remains provisional |
| `src/fl.c` | `0x4060A0`–`0x40770F` |
| `src/cutscene.c` | `0x407710`–`0x40975F` |
| `src/brains.c` | `0x409760`–`0x40B66F` |
| `src/mission.c` | `0x40B670`–`0x40D08F`; trailing joystick/navigation helpers remain provisional |
| `src/objdata.c` | Data only; original code ownership is unresolved |
| `src/nav.c` | `0x40D090`–`0x40E88F` |
| `src/nearheap.c` | `0x40E890`–`0x40EFDF` |
| `src/gameflow.c` | `0x40EFE0`–`0x40FA2F` |
| `src/title.c` | `0x40FA30`–`0x4102AF` |
| `src/joystick.c` | `0x4102B0`–`0x41073F` |
| `src/spc.c` | `0x410740`–`0x413A0F` |
| `src/cockpt.c` | `0x413A10`–`0x41807F` |
| `src/version.c` | Data only; original code ownership is unresolved |
| `src/geom.c` | `0x418080`–`0x41A9CF` |
| `src/barracks.c` | `0x41A9D0`–`0x41C50F` |
| `src/fade.c` | `0x41C510`–`0x41C75F` |
| `src/debug.cpp` | `0x41C760`–`0x41D0BF` |
| `src/mathutil.c` | `0x41D0C0`–`0x41D11F`; read-only `0x4630DC`–`0x4630E7` |
| `src/disk.c` | `0x41D120`–`0x41DEDF` |
| `src/ship.c` | `0x41DEE0`–`0x4219BF` |
| `src/init.c` | `0x4219C0`–`0x42200F` |
| `src/logic.c` | `0x422010`–`0x42572F` |
| `src/pilot.cpp` | `0x425730`–`0x425C2F` |
| `src/trainsim.c` | `0x425C30`–`0x4272EF` |
| `src/main.c` | `0x4272F0`–`0x42A8EF`; trailing cursor/palette helpers remain provisional |
| `src/targ.c` | `0x42A8F0`–`0x42ACFF` |
| `src/select.c` | `0x42AD00`–`0x42AF9F` |
| `src/timer.c` | `0x42AFA0`–`0x42B04F`; internal ownership remains provisional |
| `src/pload.c` | `0x42B050`–`0x42B15F` |
| `src/sound.c` | `0x42B160`–`0x42B99F` |
| `src/launch.c` | `0x42B9A0`–`0x42C40F`; boundary with installation remains provisional |
| `src/install.c` | `0x42C410`–`0x42C7FF`; leading empty hook's exact owner remains provisional |
| `src/damage.c` | `0x42C800`–`0x42CDAF` |
| `src/view.c` | `0x42CDB0`–`0x42E31F` |
| `src/music.c` | `0x42E320`–`0x42EFBF` |
| `src/screen.c` | `0x42EFC0`–`0x42FA8F` |
| `src/ending.c` | `0x42FA90`–`0x43070F` |
| `src/input.c` | `0x430710`–`0x430BBF` |
| `src/comm.c` | `0x430BC0`–`0x4318EF` |
| `src/video.c` | `0x4318F0`–`0x431A0F` |
| `src/joycal.c` | `0x431A10`–`0x431EFF` |
| `src/dib.c` | `0x431F00`–`0x43390F` |
| `src/game.c` | `0x433910`–`0x433ABF` |
| `src/smart.c` | `0x433AC0`–`0x434CCF` |
| `src/data.c` | Data only, `0x46DA90`–`0x46E14B`; original code ownership is unknown |
| `src/mathfp.c` | `0x434CD0`–`0x4353FF` |
| `src/strdos.c` | `0x435400`–`0x4355EF` |
| `src/eventmgr.c` | `0x4355F0`–`0x436CFF` |
| `src/screens.c` | `0x436D00`–`0x439C3F` |
| `src/raster.asm` | `0x439C40`–`0x43F635` |
| `src/killbrd.c` | `0x43F640`–`0x44089F` |
| `src/gr.c` | `0x4408A0`–`0x44269F` |

The former system/main/HUD division split the main module's initialized
state from its first literal users. The main window now begins with the
process/memory helpers and includes flight input, banners, and the flight
loop. The Mac `targ` and `select` runs corroborate the following target and
weapon-selection groups. The trailing cursor/palette helpers before `targ`
and the Win32 timer helpers after `select` retain provisional ownership;
their positions alone do not prove their original object boundaries.

`LoadPaletteTripletsFile` at `0x00404610` closes `auto.c`. Its `"rb"`
literal is at `0x00465548`, immediately after the initialized autopilot
formation count at `0x00465544` and before the ejection tables at
`0x00465550`. The formation count has only local original references and is
private in `auto.c`. The packet error word at `0x00465460` similarly precedes
`mono.c`'s data-file error strings; it remains shared with packet consumers.
These initialized globals are kept in their owning units and included in
the distributed global audits.

`ejection_sequence` and `stranded_sequence` occupy `eject.c`. Its three
private animation tables and stranded-message pointer precede the message,
`"View Templates"`, and `"THE END"` literals. The complete section is 156
bytes; four linker alignment bytes precede the mouth-animation table at
`0x004655F0`. That initialized table cannot follow compiler literals within
one CL 4.20 object, establishing a boundary between the two data groups.
The old combined unit represented the literals as oversized named arrays
and owned eight bytes that should not have been part of those declarations.

The remaining campaign/conversation group starts with `ParseFaceAnimation`
at `0x00404CD0`, immediately before `ParseMouthAnimation`. The first parser
has no unit-local data reference, so that precise code boundary remains
provisional. The mouth parser indexes its private 26-word phoneme table
with `character - 'a'`, explaining the original displacement `0x0046552E`.
Its empty local-array initializer produces the literal at `0x00465624`;
the following two `"%d"`, two date, one time, and three conversation formats
remain separate use-site literals. The complete 135-byte campaign section
ends at `0x00465676`, with one linker alignment byte before maneuver data.

The beginning of the constant and initialized-data sections also constrains
ownership and qualifiers:

| Original data | Owner and storage |
| --- | --- |
| `0x00463008`–`0x004630D9` | `barracks.c`: the 21 constant menu regions passed directly by `BarracksScreen` |
| `0x004630E8`–`0x00463167` | `dib.c`: eight DirectDraw class/interface GUIDs, followed by its frame-rate constants |
| `0x004631C8`–`0x004631FF` | `gr.c`: constant registry-key and `Cheater` value-name arrays used by `ReadCheaterFlagFromRegistry` |
| `0x00465040`–`0x00465057` | `winmain.c`: two initialized hazard counters and two writable eight-byte hazard tables |
| `0x00465058`–`0x004650B3` | `winmain.c`: window/audio flags, heap counters, class/title arrays, and message-pump state; two intervening words remain unidentified |
| `0x004650B4`–`0x0046535A` | `winmain.c`: configuration, window, memory, joystick, and heap-corruption literals |
| `0x0046535C`–`0x0046545E` | `winsys.c`: game-data paths, CD paths, and disc prompts |
| `0x00465460`–`0x00465540` | `mono.c`: packet-error word, file diagnostics, and MONODEBG literals |
| `0x00465544`–`0x0046554A` | `auto.c`: formation count and palette-file mode literal |
| `0x00465550`–`0x004655EB` | `eject.c`: animation tables, stranded-message pointer, and three literals |
| `0x004655F0`–`0x00465676` | `cmpgn.c`: phoneme frames and conversation-format literals |
| `0x00465678`–`0x00465763` | `fl.c`: maneuver reroll weights and handler pointers |
| `0x00465768`–`0x00465C78` | `cutscene.c`: scramble, landing, and funeral tables followed by their literals and the animation-demo diagnostic |
| `0x00465C7C`–`0x00465C7D` | `brains.c`: enemy-sighting state |
| `0x00465C80`–`0x0046602A` | `mission.c`: debriefing personality, formation state, packet references, formation/timing tables, and pilot-speech diagnostic |
| `0x00466030`–`0x0046865C` | `objdata.c`: animation streams, object definitions, weapon coordinates, and object-name literals; original object name and associated code remain unknown |
| `0x00468660`–`0x004688BF` | `nav.c`: map state, objective styles, mission-name pointers, and the following navigation literals |
| `0x004688C0`–`0x004688CB` | `nearheap.c`: near-heap state and descriptor limit |
| `0x004688CC`–`0x0046890B` | `gameflow.c`: campaign state and the following viewport-disposal diagnostic; the helper's unit ownership remains provisional |
| `0x00468910`–`0x00468F01` | `title.c`: opening-text pointer, Secret Missions message, credits, title-menu and resource records, and the following cinematic literals |
| `0x00468F04`–`0x00468FF1` | `joystick.c`: five interactive calibration prompts and three separate calibration-file literals |
| `0x00468FF4`–`0x00468FFF` | `spc.c`: capital-ship view distance, landing authorization, and alternate chase-view state |
| `0x00469000`–`0x004693B0` | `cockpt.c`: cockpit state, the unreferenced eight zero bytes at `0x00469010`, shared weapon-name array, tables, and following message literals |
| `0x004693B4`–`0x004693C0` | `version.c`: version pointer and its following literal; original object name and associated code remain unknown |
| `0x004693C8`–`0x0046963F` | `barracks.c`: bunk origins, menu pointers, modal bounds/state, campaign file numbers, and the following string literals |
| `0x00469640`–`0x00469641` | `fade.c`: palette-transition initialization flag |
| `0x00469644`–`0x00469665` | `debug.cpp`: console count, keyboard state/hook, and font, spinner, and newline literals |
| `0x00469668`–`0x00469872` | `disk.c`: graphics-mode and error-message pointers, disk marker, prompt settings, and the complete following literal pool |
| `0x00469878`–`0x00469995` | `ship.c`: damage-system and debris tables, gun refire delays, and the following four HUD literals |
| `0x00469998`–`0x00469BBD` | `init.c`: graphics defaults, the shared low-memory message, reservation tables, and startup literals |
| `0x00469BC0`–`0x00469D7E` | `logic.c`: resource descriptors, scene state, and the scene text-format literal |
| `0x00469D80`–`0x00469D9C` | `pilot.cpp`: transition subtitle, empty answer input, and answer label |
| `0x00469DA0`–`0x00469FA6` | `trainsim.c`: an unreferenced word, panel state, high-score name pointers, menu regions, simulator state, and the following UI literals |
| `0x00469FA8`–`0x0046A3A1` | `main.c`: legacy overlay setting, game/input state, and process, startup, flight, HUD, and score literals |
| `0x0046A3A4`–`0x0046A436` | `pload.c`: compressed-data and non-pushed-buffer diagnostics; trailing audio helpers retain provisional ownership |
| `0x0046A748`–`0x0046A8CD` | `damage.c`: cockpit damage tables and the following report literals |
| `0x0046A8D0`–`0x0046A9F0` | `view.c`: scripted-view state, constellation tables, packet settings, target armor rectangles, and the following target/display literals |
| `0x0046AD88`–`0x0046AF50` | `ending.c`: allocation tag, campaign-ending pointers and projectile origins, subtitle literals, and four separate text formats |
| `0x0046AF54`–`0x0046AF59` | `input.c`: double-click interval and menu-pointer speed |
| `0x0046AF60`–`0x0046B167` | `comm.c`: menu/video state, ace and menu-label pointers, and the complete communication literal pool |
| `0x0046B168`–`0x0046B171` | `video.c`: video mode and the palette-allocation diagnostic literals |
| `0x0046B178`–`0x0046B1A1` | `joycal.c`: graphics-mode table, two unused driver names, and both calibration-file literals |

The former large brains window is divided into flight maneuvers, cutscenes,
NPC intelligence, and mission setup. The Mac `fl` symbol run corroborates
the maneuver group, including the dispatcher using both private tables.
All 47 dispatch pointers must target the correct one of 42 named function
entries; matching another handler or an interior byte is rejected. Its
236-byte initialized section has four alignment bytes before the scene tables.

All 19 cutscene declarations have only local original references. The four
landing comments are now pointer-initializer literals, followed by the
subtitle formats, commands, and animation-demo diagnostic. Their 289-byte
pool ends at `0x00465C78`, establishing the boundary before the AI state.
The seven funeral particle origins end at `0x00465B33`. Four compiler
alignment bytes precede the scene-selection array at `0x00465B38`; the
original lookup uses `series * 2 + 0x00465B36`, or `array[series - 1]`.
Thirteen selections and two alignment bytes reproduce the following flag
at `0x00465B54`. This declaration matches the observed bytes; the binary
does not distinguish its final padding from an additional zero element.
The former 15-element declaration started inside the preceding alignment.
The SDL build preserves the zero results for series 0 and 14 explicitly.
The complete 1,297-byte section and all eleven stored pointers match.

The surviving WINGLEADER `BRAINS.C` and Mac `brain` run corroborate
`cruise_home` through `FF_missile_intelligence`. Its enemy-sighting word
is shared with mission setup. The following 939-byte mission section has
the shared debriefing personality and packet-reference table, plus private
formation state and formation/timing arrays. SDL adapters retain access to
the last three through SDL-only declarations. The 27-byte pilot-speech
diagnostic ends before object definitions, with five linker alignment bytes.
The division between the AI word and mission definitions remains provisional:
no intervening literal proves that internal boundary. The joystick helpers
beginning at `0x0040CAA0` and navigation helpers at `0x0040CB60` also retain
provisional ownership in `mission.c`. Its final `Build_objective_list` extends
through `0x0040D08F`; the old window boundary at `0x0040D000` cut through it.
Its complete 448-byte span now matches, including the signed sentinel index
and source-type dispatch. This validates the function span, not the provisional
internal unit boundaries. The conversation group's final `CloseLook` likewise
matches all 704 bytes through `0x0040609F`, preserving its four-byte local frame.

The object-data block begins after the pilot-speech diagnostic at
`0x00466010` and ends with its name literals before navigation state at
`0x00468660`. Initialized declarations cannot follow compiler literals in
one CL 4.20 object, so both edges establish data boundaries. `objdata.c`
describes the recovered contents; the original filename and any associated
functions remain unknown. It is linked after the provisional mission group
and before navigation without assigning it invented function ownership.

Its 21 private animation arrays occupy 1,064 bytes, followed by 58 packed
135-byte object records, two compiler alignment bytes, and the child-offset
and weapon-coordinate tables. The 393-byte literal pool begins at
`0x004684D4`. Each non-null display-name initializer owns its literal:
30 nonempty names and 26 distinct empty names. The two all-zero object records
have null names. The former flattened string array included the three
linker alignment bytes before navigation; ordinary separate literals now
reproduce that layout without explicit padding. All 80 relocations stay
inside the block: 56 name pointers and 24 animation pointers. The strict
unit gate checks all 9,773 bytes and all 25 declaration offsets, including
interior animation targets and the separate empty-name addresses.

The object-type records retain their 135-byte stride with one signed
32-bit lifetime/fuel-capacity field at offset `0x10`. The two words formerly
declared there are one value: the first four ship records contain 200000,
250000, 280000, and 300000. The full-width fuel division and projectile
lifetime loads independently establish this layout. Combining the initializer
words preserves every table byte; no field follows at offset `0x12`.
The next field, damage capacity, remains at `0x14`.

The navigation unit's 360-byte literal pool follows its map state and pointer
tables, ending immediately before the initialized near-heap state at
`0x004688C0`. This establishes a boundary before `MergeAdjacentNearHeapBlocks`
at `0x0040E890`: those initialized definitions cannot follow function literals
in one CL 4.20 object. The 34 former named navigation string arrays are now
single-use literals, including the three separate `"err"` mission names.
Six initialized variables and the two uninitialized navigation arrays have
only local original references. The SDL mission-name consumer retains access
through the SDL-only header.

The near-heap routines occupy the next nine-function run, followed by campaign
scoring and mission flow. Their internal division into `nearheap.c` and
`gameflow.c` follows their distinct state and call groups; no intervening
literal pool proves that particular boundary. The adjacent `free_viewport`
helper remains in `gameflow.c` provisionally. Its diagnostic ends before the
title's initialized data at `0x00468910`, proving another object boundary.
The unreferenced constant helper at `0x0040FA30` remains with title provisionally.

Removing the near-heap source marker preserves this boundary and all data
addresses. Its initialized state now follows the first two descriptor
helpers, which use none of it; no literals intervene. This is a matching
source placement, not evidence of the original declaration position. See
the three-marker cleanup in `docs/PATTERNS.md`.

The title data includes the previously omitted 279-byte Secret Missions
message at `0x00468918`, including its terminator. No surviving code or data
relocation targets it. A writable named character array reproduces those
bytes and the single alignment byte before the credit count. Its descriptive
name does not claim recovery of the original identifier. The opening-text
pointer and all nineteen non-null credit pointers target the following
1,062-byte literal pool, which retains the original spelling and newlines.

The interactive calibration prompts follow the title literals at `0x00468F04`.
The three `"j.cal"` literals at `0x00468FDC`, `0x00468FE4`, and `0x00468FEC`
belong respectively to calibration failure, file opening, and write failure.
They remain separate use-site literals. Initialized space state follows at
`0x00468FF4`, proving an object boundary after the calibration literal pool.
The three calibration/button-wait functions now occupy `joystick.c`; its
238-byte `.data` section has no named globals. The title/joystick code boundary
follows their distinct behavior and call groups; contiguous literal storage
alone does not prove that particular division.

The following space state occupies twelve initialized bytes. The alternate
chase-view flag has only local original references and is private in the native
build; the SDL camera adapter retains access through the SDL-only header.
`SetFleetOverviewView` remains at the start of `spc.c` provisionally: it has no
unit-local data to establish its exact original ownership. The Mac `spc`
symbol run identifies `rotate_eye_to_goal` through `object_intelligence`;
the next recovered function, `EmitTextString` at `0x00413A10`, begins the
cockpit window. These windows no longer overlap the surviving space functions.

The main camera routine, `set_eye_direction_and_position`, now reproduces its
3,264-byte span and 60-byte local frame exactly. The fleet overview retains
its 752-byte span and 36-byte frame, with only the maximum-range comparison
and branch differing. Its native compiler overlays an uninitialized range
local with an initialized vector component; explicit initialization had
changed both that read and the vector placement. The SDL build preserves the
observed fallback value explicitly. See `docs/PATTERNS.md` for the stack
evidence and the camera branch ordering.

Within this window, `start_dust`, `update_star_field`, and
`house_keep_objects` reproduce their complete 288-, 784-, and 1,008-byte
aligned spans. The housekeeping dispatch changes register allocation in
later routines in the same object. Retaining the animation's promoted index
and scale-command switch restores the collision routine's original 92-byte
frame; its corrected sound arguments, collision stores, and mass arithmetic
leave only two exchanged initial loads in the 1,968-byte span. All nineteen
space routines retain their original aligned sizes. The animation's remaining
scale-arithmetic differences are recorded in `docs/PATTERNS.md`; recovering
the preceding movement dispatch restored its two separate stores. The
movement and AI dispatchers now match all instruction mnemonics in their
816- and 400-byte spans, with four and one differing encodings respectively.
A separate afterburner-speed local made movement exact but enlarged the
collision routine; using the existing velocity-magnitude local preserves all
original spans. No unit boundary or linker-order change is inferred from
these compiler effects.

The unit-data gate checks the complete reconstructed `.data` sections:
system paths (259 bytes), file/developer diagnostics (225), autopilot (7),
ejection (156), conversation/campaign (135), maneuvers (236), cutscenes
(1,297), AI (2), mission setup (939), object data (9,773), navigation (608),
near-heap (12), campaign flow (64), title (1,522), joystick (238), and space
state (12). Together with the verified 723-byte startup tail, these form a
16,248-byte run with 187 pointer relocations and 40 inter-object alignment
bytes. The six referenced colour
variables and the two title resource-record pointers must resolve to their
original globals and field offsets. This proves the data layout independently
of the provisional internal code boundaries.

The cockpit's `"None"` weapon name at `0x00469080` is a named array before
the objective-range state. Both missing-weapon branches in `show_weapon_disp`
reference that same array; the former empty literals changed the text and
added extra storage. The seven bytes after the target-bracket visibility
flag are compiler alignment before the message-origin array. A separately
declared padding array added eight bytes and is removed.

The 293-byte cockpit literal pool begins at `0x0046928C` with the writable
objective-message initializer and missile-lock message. Its remaining labels
are use-site literals. The objective pointer is written through by
`flag_reached`, and the original PE keeps this literal writable. The SDL
adapter instead retains its existing writable array in the SDL cockpit unit.
The normal-arrival branch must precede the waiting branch in source to emit
`"Already Visited"`, `"Objective Reached"`, and `"Wait for %s"` in that order,
even though CL schedules the waiting path first in machine code. The final
empty literal supplies the byte load in `print_message_text`.

Twenty-two initialized cockpit declarations have only local original
references and now have private storage; shared resource pointers, the pending
message, and the navigation-pointer object retain external visibility. The
unit-data gate checks a 16-byte prefix and a 921-byte tail, including every
declared offset and both string pointers. The current COFF section has 937
bytes. The eight zero bytes at `0x00469010`–`0x00469017` have no reference;
alignment cannot produce them, so they are an unreferenced original object,
kept as `DAT_00469010[2]`. The complete 945-byte section now matches.

The version pointer follows the cockpit literal pool at `0x004693B4`, proving
another object boundary: CL emits initialized declarations before literals.
A pointer initialized directly with `"1.03F-95"` reproduces the complete
13-byte block. `version.c` isolates that data without assigning any geometry
functions to it. Its original filename and whether it shared an object with
some of those functions remain unresolved. Three alignment bytes separate
the cockpit tail from version data, and seven precede the barracks tables.
The tail, version, and eight following units through the pilot literals form
a checked 3,461-byte run with 62 pointer relocations and 24 alignment bytes.
A separate
28-byte check joins space state to the cockpit prefix. Neither bridges the
unrecovered cockpit gap or establishes absolute placement.

The GUID sequence includes `CLSID_DirectDrawClipper`, absent from the bundled
DirectDraw header, between `CLSID_DirectDraw` and `IID_IDirectDraw`. The bytes
in the retail image establish its value and position. Unused SDK identifiers
are real original data, so their definitions must survive the reconstruction.

The window class name at `0x00465088` is a writable character array used for
both `WNDCLASSA` names and `CreateWindowExA`'s class argument. Its separate
title array is at `0x00465098`. Both precede the configuration-file literals,
so they are declarations rather than literals emitted at the call sites.
The surrounding zero-valued flags and heap counters need explicit zero
initializers: tentative C definitions put them in the linker's common storage
instead of this initialized-data range. The words at `0x00465078` (one) and
`0x00465084` (zero) have no reference anywhere; they are kept with their
values as `DAT_00465078` and `DAT_00465084`, and the complete 795-byte
section now matches.

Three partial-section checks cover the 56-byte prefix, eight-byte middle,
and 723-byte tail around those missing words. Their rebuilt owner contains
787 initialized bytes, versus the original 795. The tail includes the
679-byte literal pool. `GetJoystickDevCaps` owns its diagnostic as a use-site
literal at `0x004652DC`, between the memory report and corruption-severity
strings. `WinMain` passes an empty literal at `0x00465218` to `main`; the
former `"Vj"` spelling had two incorrect nonzero bytes that only the strict
pool check exposed. The checked tail and following sixteen data units are
currently eight bytes early. This is the exact combined deficit of the two
unidentified words, not evidence that their source declarations are known.

The same storage distinction applies to the zero-valued definitions still in
`globals.c`. The 592-byte packet-reference table at `0x00465C88`, the input
sample at `0x0046A020`, and the DirectDraw pointers at `0x0046B1A4` are among
87 definitions restored to initialized data. They retain their provisional
owner until the original compilation units are established.

The default modal-text bounds at `0x00469440`–`0x00469447` form one
`ShortRect` initialized to `{24, 40, 296, 60}`. All five calls to
`InitializeModalTextPanel` pass a rectangle by value, and the initializer
copies those eight bytes as a unit. The bounds and following panel pointer
belong to `barracks.c`, together with the preceding menu tables and following
campaign file-number arrays. Only the campaign file-number arrays have users
outside this unit.

The menu-label literals follow these definitions at `0x00469468`–`0x004694E3`.
The modal renderer references `"%P"` at `0x004694E4`, immediately before
`CreateEmptySaveGameFile`'s `"SAVEGAME.WLD"` at `0x004694E8`. This literal order
places the modal-panel routines and `AnySavedGames` at the beginning of
`barracks.c`, moving its boundary to `0x0041A9D0`. The objects remain adjacent
in `SRCS_ORDERED_CORE`; `get_right_shape` closes `geom.c`.

Writing the six menu labels as literals in their pointer-table initializers
reproduces their original four-byte spacing. Separate named character arrays
introduced eight extra alignment bytes. Both `"Save this campaign  "` literals
must remain distinct. With these declarations, `barracks.obj` contributes
exactly 632 initialized-data bytes through the airlock message. The entire
block matches through binary-comp with one address shift, including its six
internal pointers, literal order, and padding; `make verify-barracks-data`
enforces this. Its absolute address remains displaced until preceding units'
data ownership is recovered.

The palette initialization flag follows that literal pool at `0x00469640`.
Named declarations precede compiler literals in VC4.20, including function-static
declarations, so the flag cannot be another barracks declaration. `fade.c`
begins at its sole user, `StepPaletteTransition`. The adjacent `DosStrcat`
entry remains in this range provisionally; its exact original ownership is
not established. Both retain their observed callee-cleanup conventions. The
five palette scratch variables are private to this unit, but their common-symbol
storage still differs from the original uninitialized-data layout.

The flag occupies two initialized bytes, followed by two linker alignment bytes.
The debug-console unit then has four initialized words and three literals:
`"Courier"`, the spinner sequence, and `"\n"`. Each string has one use site.
Named arrays reproduced their contents but required eight-byte object alignment,
placing the console count four bytes too late. Literals restore four-byte
alignment and preserve the exact constructor instructions, including the original
string copy. The complete `debug.obj` data section is 34 bytes; two alignment
bytes follow before the disk tables. Both keyboard words have external users;
the count and hook are private declarations.

The complete initialized-data run from `0x004693C8` through `0x00469D7E` now
matches with one address shift across barracks, fade, debug, disk, ship, init,
and logic. `verify-unit-data` checks all 2,487 bytes, the seven unit positions,
59 relocated pointers, and 13 inter-object alignment bytes. It does not insert
padding or claim the absolute addresses are restored. The training-simulator
block remains separate because ownership of the intervening scene data is open.

The disk unit begins with the graphics-mode pointer table. The next three
pointers at `0x00469678`, `0x0046967C`, and `0x00469680` are loaded directly
by `ReportPacketLoadError`; they point to the read-operation label,
allocation-operation label, and error format. Direct literals in that
routine omitted both the pointer storage and the original loads. A further
unreferenced sequence at `0x00469698` contains pointers to `"(OVR)"` and
`""`, then zero. Its two HIGHLOW relocations and null word are preserved as
`DAT_00469698[3]`; no purpose is assigned to it without a known user.

The resulting `disk.obj` initialized-data section is exactly 523 bytes.
All values, nine pointer relocations, and the 459-byte literal pool match
with one address shift. Five linker padding bytes precede the ship table at
`0x00469878`. `make verify-unit-data` checks the barracks, debug, and disk
blocks, their object sizes, and every global's relative position. Absolute
placement remains dependent on the preceding units' data recovery.

The ship unit's initialized data begins with the 50-word damage-system
table at `0x00469878`. Four compiler alignment bytes precede the existing
explosion-debris table at `0x004698E0`; the hit-debris types and gun refire
delays follow at `0x00469950` and `0x0046995C`. All four tables are private
to `ship.c`. The four HUD strings follow in function-use order at
`0x00469960`, `0x00469974`, `0x00469984`, and `0x0046998C`. Writing these
as literals at their use sites reproduces the original four-byte spacing;
separate character arrays introduced extra alignment. The complete
286-byte `ship.obj` data section matches literally through binary-comp,
including its padding, with no relocations. Two linker padding bytes
precede the next unit's scene-animation flag at `0x00469998`.
`make verify-unit-data` covers this block as well, including the private
explosion table that is absent from the linker's public-symbol list.

The following startup block establishes a separate initialization unit.
Its declarations end with the expanded-memory reservation table at
`0x00469AB0`; the literals from `LoadGamePaletteFile`, `EMStartUp`, and
`LoadOriginFxDrivers` follow at `0x00469ABC`–`0x00469BBD`. The resource
descriptor tables begin after this literal pool, at `0x00469BC0`.
A controlled VC4.20 core-profile probe confirms that named declarations,
including function-static arrays, precede the unit's literal pool even
when they appear after functions in source. These two data groups therefore
require separate objects. `init.c` contains the twelve startup functions
through `GetFxDriverStatus`; the following ace and mission routines remain
in `logic.c`. Interior boundaries within `logic.c` remain provisional.

The two low-memory branches both push `0x00469A28`. This 100-byte message
precedes the reservation tables, so it is a shared writable array rather
than a pair of compiler literals. `szInsufficientMemory` restores that
storage and both references without string pooling. The complete
550-byte initialized-data section in `init.obj` matches literally, including
the spacing between colour bytes and every startup string.

The next 447-byte block contains five resource tables, their active flags,
constellation indices, scene-file numbers, a scene-wait flag, and the final
`"%X%Y%F%s%P"` literal used by `PlaySceneAnimation`. Its 44 relocated pointers
address shape fields and pointer slots in other units. `verify-unit-data`
checks each against the corresponding rebuilt global and original field
offset; null contents at the destination cannot stand in for the correct
address. All other bytes and relative global positions match exactly.
The initialization and logic checks also enforce each object's complete
data size. Two alignment bytes separate the blocks, and one follows the
scene-format literal before the pilot unit's data at `0x00469D80`.

`ShowMeanwhileTransition` and `PromptForAnswerText` own the next three
literals: `"Meanwhile..."`, the empty answer input, and `"Answer: "`.
Their complete 29-byte section matches, including the empty literal's byte
load. Named arrays previously overallocated these literals and placed them
in the temporary globals unit. Three alignment bytes follow the label;
the unreferenced word `10` at `0x00469DA0` follows them. A unit's
initialized variables precede its literals, so that word cannot belong to
`pilot.cpp`: it is `trainsim.c`'s first variable, `DAT_00469da0`, and the four
zero bytes after it are the eight-byte alignment of the panel viewport.

The training-simulator block starts at `0x00469DA8`, after the scene
literals and the Win32 console wrappers. Its viewport, bounds, shared title
array, six high-score name pointers, menu regions, and three simulator-state
variables precede the literal pool at `0x00469E38`. The six names are separate
literals in the pointer initializer, not a manually concatenated array.
Empty pilot input, the text-flush token, and every following UI string are
literals at their respective call sites. The title remains a named array:
two calls use the same address before the pointer table.

`trainsim.c` now contains `EraseTextContextBackground` through `RunTrainSim`,
ending before the process-exit routines. These game functions use the C core
profile; the console allocation wrapper and its `/GX` construction cleanup
remain in `pilot.cpp`. That file's internal C/C++ boundary is still provisional.
The complete 511-byte simulator data section and all six name-pointer targets
match with one address shift, now from the unreferenced word `10` at
`0x00469DA0`.

The next word, `0x1370` at `0x00469FA8`, has stronger evidence. The surviving
WINGLEADER main-source screenshot (`../squadronsourcecode.png`) declares
`extern unsigned _ovrbuffer = 0x1370`, immediately before game mode/status
and the two frame counters. The Win32 bytes preserve that value and the
same neighboring state, despite having no relocation that references the
word. It is a retained legacy overlay setting, not padding. Its original
external identifier is preserved in `main.c`; the earlier analysis's claim
that it had no Win32 counterpart was incorrect.

The 29 initialized declarations occupy 184 bytes. The private volume table,
memory-display flag, last input sample, mouse thresholds, button timestamps,
and input latches have original users only within this main group. Five
remain available to SDL adapters through SDL-only declarations. Shared game
state stays declared in `globals.h`. The following 834-byte literal pool
starts with the process helpers' diagnostics, continues through `main`,
and ends with the HUD score messages. All 1,018 bytes and all declaration
offsets match; no pointer relocations occur in this data block.

The unreferenced diagnostic at `0x0046A15C` describes an `int`/pointer size
check. A controlled VC4.20 probe confirms that `sizeof(int) != sizeof(short *)`
is folded away while its diagnostic literal survives in initialized data.
This source form, inferred from the text, restores the original 36-byte
literal without adding runtime code. `ReportOutOfMemoryAndExit` passes the
shared message at `0x00469A28`; the former shortened use-site message added
incorrect storage. The buffer tags are separate literals, and the two
volume-adjustment cases and volume toggles preserve their original literal
order without string pooling.

`PacketLoad`'s compressed-data case precedes its default case in source,
although CL schedules the default code first. This emits both fatal
diagnostics before the non-pushed-buffer diagnostic, matching all 147 bytes
of its literal section. The following audio routines share state with the
playback and registry routines and now reside together in `sound.c`.

The audio unit starts at `InitializeAudioSystem`, `0x0042B160`, and ends
after `SaveVolumeSettingsToRegistry`. Its five initialized state variables
occupy `0x0046A438`–`0x0046A44B`. The three references to each registry
value name share the arrays at `0x0046A450` and `0x0046A460`; writing a
separate literal at each call would incorrectly duplicate their storage.
By contrast, the two full registry-key literals are distinct. The following
222-byte literal pool preserves both `SNOW` copies, the mismatched `sfx23`
diagnostic, and the SFX-volume message's trailing newline. All 274 bytes,
seven declaration offsets, and the separate eight-byte private `.bss` match.

Six linker alignment bytes separate that literal pool from the launch
tables at `0x0046A550`. Their position proves a new compilation unit before
`DrawLaunchDoorFrame`. The five private tables occupy 172 bytes. The first
table changes screen Y, so it is named `asCarrierLaunchApproachDeltaY`.
The last two tables begin at `0x0046A5C0` and `0x0046A5E0`, each containing
14 words. The previous 16-word declarations included four preceding alignment
bytes apiece. The fighter loop uses frames 9 through 22 and the displacement
`0x0046A5AE`: indexing the real table by `frame - 9` preserves that operand.
The scripted-view call passes `0x0046A5E0` directly. Normal compiler alignment
supplies both gaps; neither belongs to an array.

The configuration and installation literals occupy the next 325 bytes,
`0x0046A5FC`–`0x0046A740`. The disk prompt is a use-site literal, and each
installation-failure message remains separate. These routines and the
launch routines use the C profile. Their internal boundary is provisional:
the data establishes separation from the preceding audio and following
damage units but does not independently distinguish launch from installation.
The empty shutdown hook remains with installation provisionally.

Simulator, main, loader, audio, launch, installation, damage, and view data
form one checked 3,145-byte run, with 20 stored pointers and 19 linker alignment
bytes. That run is currently 24 bytes early: eight bytes each remain missing
in startup state, cockpit state, and the gap before the simulator.

The maneuver proximity threshold at `0x00475E78` and reroll byte at
`0x00475E7C` are private to `fl.c`. All references occur in the threshold
setter, `Mveer_away`, and `perform_maneuver`. Their five-byte `.bss` contains
two compiler alignment bytes between the declarations, followed by three
linker alignment bytes before navigation. The threshold is named for its
use in the proximity-based boost and maneuver-reset decisions.

The navigation label and exclusion arrays at `0x00475E80` and `0x00475F48`
have references only in `nav.c`. Their existing 20 packed ten-byte label
records and 21 eight-byte rectangles now occupy a private 368-byte `.bss`
section, with the rectangle array at offset `0xC8`. The separate 1,024-byte
cockpit cursor background at `0x00475FF0` is used only by
`overlay_head_up_display` and `RestoreTransientCockpitGraphics`; it now
occupies `cockpt.c`'s `.bss`. Together with the preceding maneuver state and
following console pointer, these form a verified 1,404-byte run with three
inter-object alignment bytes.

### Static `.bss`: `winsys.c` through `gr.c`

Every unit's uninitialized statics now occupy their original addresses, from
`winsys.c`'s key states at `0x00475B68` to the end of `gr.c`'s stream path at
`0x005977D0`. CL 4.20 places a unit's uninitialized statics in its own `.bss`
in ascending order of the key `(h ^ (h >> 16)) & 0x3FF`, where `h` is C1's
identifier hash (`h = (h >> 4) + 4 * h + c` over the characters). Equal keys
go in reverse declaration order. Arrays of eight bytes or more are 8-aligned,
smaller objects 4-aligned. Declaration order cannot recover these layouts.
The identifier spellings do. Each static in this range is named so that its
key follows the original addresses. `szDIBFailureString` and
`hDIBParentWindow` share key 867, so the failure string is declared first.

DIB's frame height, frame width, and image pointer are grouped before its
other BSS declarations when removing `WC1_DIB_SOURCE`. Their hash keys keep
the storage order unchanged while the declaration order restores the
original multiplication operands. The complete executable remains exact;
this does not recover the original header or declaration text.

| Range | Owner | Contents |
| --- | --- | --- |
| `0x00475B68`–`0x00475D1F` | `winsys.c` | 14 polled key states, an unreferenced 128-byte area, the CD streams directory |
| `0x00475D20`–`0x00475E77` | `mono.c` | four data-file error texts, an unreferenced 80-byte area, the MONODEBG flag and handle |
| `0x00475E78`–`0x004763EF` | `fl.c`, `nav.c`, `cockpt.c` | maneuver state, navigation labels, cursor background (below) |
| `0x004763F0`–`0x004764F7` | `pilot.cpp` | console pointer and an unreferenced 256-byte C++ global |
| `0x004764F8`–`0x0047654B` | `main.c` | the twelve flight-profiler counters and timestamps |
| `0x0047654C`–`0x0047661F` | `sound.c`, `damage.c`, `music.c` | snow-static wave, displayed damage, SFX path (below) |
| `0x00476620`–`0x004865A7` | `dib.c` | frame-rate text, frame buffer, DIB window, palette, failure and FPS texts, four unreferenced slots |
| `0x004865A8`–`0x004875A7` | `eventmgr.c` | cursor save area (below) |
| `0x004875A8`–`0x005977CF` | `gr.c` | raster target and clip, transform, colour and RLE buffers, an unreferenced 1,648-byte table, streaming flag and path |

No original HIGHLOW relocation targets the unreferenced ranges: `0x00475B70`
(128 bytes), `0x00475E20` (80), `0x004763F8` (256), `0x0048605C` and
`0x00486064` (ints), `0x00486068` (12), `0x00486514` (int), `0x00486520` (8)
and `0x00496FD8` (1,648). The sources declare them as `…Unused…` statics
sized to their gaps. CL keeps an unreferenced uninitialized static only when
its unit also has a referenced one. `trainsim.c` has no other static, so the
256-byte buffer stays a C++ global at the end of `pilot.cpp`. CL emits no
COFF symbol for an unreferenced static, so `bin/collectGlobalDefinitions.py`
leaves these out of the map-based data audit. `verify-unit-data` checks their
slots through the complete section sizes and the offsets of the referenced
statics around them, and checks all thirteen contributions as one
1,186,920-byte run.

The `ix` objects continue the run: the logger's 1,024-byte buffer at
`0x005977D0`, then `streamer.cpp`'s 27 uninitialized C++ globals and one
unreferenced word at `0x00597BD8`, ending at `0x00597CFB`. Relocations in the
matching streamer and worker-thread functions give every global's original
address. Uninitialized C++ globals follow the same identifier-hash order as C
statics, so the streamer names were chosen to reproduce that order.
`verify-unit-data` checks both objects and the whole run from `winsys.c` to
`system.cpp`.

### Linker commons from `0x00598888`

The tentative C definitions follow the static region. LINK lays them out in
reverse first-encounter order: objects in link order, each object's commons
in its COFF order (descending identifier-hash key). Read upward from the top,
the original therefore shows one block per defining unit in link order.
Unit-private blocks are clear in the original (joycal, video, comm, input,
screen, music, view, damage, launch, timer, main, trainsim, logic, init,
ship, disk, fade, geom, cockpt, spc, title, gameflow, nearheap, mission,
cutscene, cmpgn, mono, winsys, winmain). The shared ship and object arrays sit
between `eventmgr.c` and `joycal.c`, where `game.c` links. LINK aligns a common to
its size rounded up to a power of two, at most 16 bytes, and keeps no
unreferenced common. Every gap in the original must therefore be alignment or
part of a larger object.

Six reconstruction types broke that rule and are corrected:

| Original | Correction |
| --- | --- |
| `0x00598AB0`–`0x00598AD8` | the room-menu cursor frame, regions, labels, current label and text context are one packed `RoomMenu` (its pointers sit at 2 mod 4) |
| `0x005A66E0`–`0x005A66EF` | a twelve-byte `ScreenAllocation` record, then the pointer `InitializeDIBScreenViewport` stores to it at `0x005A66EC`; the sound-source flags start at `0x005A66F0` (64 entries, indexed directly) |
| `0x005A8690`–`0x005A86A7` | the current mission header is one `MissionHeaderDisk` copy; its eight initial ships are scanned as one array |
| `0x0059D870` | `aHazardFields` holds eight 22-byte entries; the 22-byte gap before `0x0059D920` is the eighth |
| `0x005A7D20` | the disk prompt's drive buffer is 17–32 bytes (32 used) |
| `0x005A86C4`, `0x005A8718`, `0x005A8740` | three coordinate pairs are `ShortPoint`s; their 4-byte alignment explains the 2-byte gaps before them |

The static definitions replace these variables' shared declarations in
`globals.h`, so other units now see 36–53 fewer declarations. That shifts
CL's register tie-breaks: exact report matches fell from 1,324 to 1,271.
Restoring each unit's former count with probe declarations after its includes
recovers 37 of the 42 exact functions lost at object level. The loss is
therefore a symbol-count effect. It will be resolved in a dedicated
declaration-count pass, not by keeping declarations of other units' statics.

All 488 game commons now occupy their original addresses. Each tentative
definition sits in the unit whose block holds it, under that unit's
`/* Linker commons defined here… */` comment, in address order. Within a
block, LINK orders commons by the same identifier-hash key as the statics
above. Of the 488 commons, 320 were renamed so that their keys follow the
original addresses, and 294 definitions moved to their block's unit. Every
new name still describes its variable. A block fixes which object must
define a common in link order. Where the referencing units do not establish
the original owner, the block's unit is placement evidence only. `sound.c`
is compiled as C++, where a tentative definition becomes ordinary `.bss`
storage, so it holds no commons. The `nUnused…` commons are write-only. The
three flight-profiler words at `0x00598888` are only zeroed with the
profiler's deltas, and the input-timer flag at `0x0059AB64` is only set to 1.

The weapon-display origin at `0x005A7788` is now one four-byte `ShortPoint`
common in `targ.c`, named `stWpnDispOrigin`; its Y field occupies
`0x005A778A`. This replaces the two scalar declarations. The structure
recovers the original register use in `RemovePlayerReleaseWeapon` and the
X reload in `show_damage_disp`. Its identifier-hash key, 334, preserves
its position and every surrounding common address.

The 200-byte `szSfxWavePath` buffer at `0x00476558` now resides in `music.c`.
Its two original references both occur in `PlaySfxWaveFileByNumber`: `sprintf`
formats the filename into it, then `playWAVE` receives the same buffer. Its
end is immediately before the DirectDraw frame-rate text at `0x00476620`.
The private uninitialized definition produces a 200-byte `.bss` section.
The unit-data gate checks its owner and size, and the sound pointers, damage
state, and this buffer as one 212-byte run with no intervening alignment bytes.

The cursor save buffer at `0x004865A8` similarly belongs to `eventmgr.c`.
Its three original references occur in the mouse-background capture and
restore routines in that unit. The 4,096-byte buffer ends immediately before
the graphics workspace at `0x004875A8`; it is distinct from the cockpit's
1,024-byte mouse-background buffer at `0x00475FF0`. Its private definition
produces the original `.bss` storage and size. These buffers remain shared
only in the SDL build, whose audio and renderer adapters use them through
declarations in `wc1sdl.h`.

The damage-display unit starts with `show_damage_disp` at `0x0042C800`.
Its initialized tables start at `0x0046A748`, after the installation
literal pool ending at `0x0046A740` and seven linker alignment bytes.
CL places a unit's initialized definitions before its literals, so these
tables require a separate owner from the preceding installation routines.
The display positions, component-name pointers, frame indices, and severity
pointers precede the literal pool at `0x0046A7C4`. The component names and
severity names are individual literals in their pointer initializers; named
character arrays impose different alignment. The two status-format literals
at `0x0046A8B0` and `0x0046A8C0` remain separate use-site strings.

`verify-unit-data` verifies the entire 390-byte damage `.data` section,
including all 14 pointers, and its separate four-byte `.bss`. The three
private tables and private damage state have no declarations in the shared
header. `WC1.map` gains their actual linked addresses from COFF symbols and
relocations, as described in `docs/EXPORT.md`. Two alignment bytes follow the
last literal before the scripted-view state at `0x0046A8D0`.
The constant-return routine at `0x0042CDA0` provisionally closes this unit.

The following `parse_view_script` at `0x0042CDB0` starts `view.c`, whose
initialized state, constellation tables, packet settings, and target armor
rectangles occupy `0x0046A8D0`–`0x0046A947`. The target/display literal pool
follows immediately and ends with the second `"not MCGA"` at `0x0046A9F0`.
Music's initialized state starts at `0x0046A9F8`, after those literals;
CL's definitions-before-literals layout therefore requires a separate unit
before `FadeMusic` at `0x0042E320`. The packet and viewport helpers remain in
the contiguous view unit; no internal source-file boundary is established.

`verify-unit-data` checks the complete 289-byte view `.data` section and all
nine variable offsets. It also checks damage and view as one 681-byte run,
including the two alignment bytes between them. Seven view variables have
only local references in the recovered original code and are private;
the two scripted-view controls remain shared. The map supplement now resolves
these private symbols from `view.obj` alongside the damage symbols.
The BSS used by the view functions remains in temporary storage until its
original declaration order is recovered.

The music unit owns the initialized state beginning at `0x0046A9F8` and
`aabSoundEffectDescriptors` at `0x0046AA50`. The descriptor table is 37
records of eight bytes, with the last byte increasing from 0 through 36.
The sole surviving caller, `TheEndFireWorks`, passes record 36 at
`0x0046AB70`; the earlier seven-byte definition omitted its final index byte
and all preceding records. The table remains writable, as in the original.
Five state variables have references only within music and are private;
the other state and the table remain shared. The map supplement resolves
private music symbols from COFF in the same way as damage and view.

The music unit's layout is now complete. Six zero-valued slots at
`0x0046AA00`, `0x0046AA08`, `0x0046AA1C`, `0x0046AA20`, `0x0046AA28` and
`0x0046AA44` have no reference anywhere and are kept as `DAT_` statics at
their positions. The literal pool also contains an unreferenced
`" queue_start "` at `0x0046AC98` and two `"sfx%02i.wav"` copies at
`0x0046AD04` and `0x0046AD20`. With `/Gf` off each literal belongs to a use
site, and CL 4.20 keeps the literal of a use the optimizer deletes (checked:
both an `if (0)` call and a call after `return` keep their literals and emit
no code). So the original had eliminated uses at those points in source
order: in `ProcessMusicScriptCommand` before the live queue-start trace, and
in the two stubs `FlushSoundEffectsAndLog` and `sound_effect`. The source
reproduces them as a compiled-out trace and as unreachable wave-name
formatting after each stub's `return`; the original form of that code is
unknown, but all three functions remain exact.

The linked-data audit checks the complete 296-byte descriptor table and 408
bytes of used music literals in four separate runs. The final run also checks
the following 80 bytes of screen allocation and memory-stack diagnostics.
The run metadata is in `config/data-literals.h`; it deliberately excludes
the unrecovered literals.
These are partial data checks, not the complete-section guarantee provided
for damage and view by `verify-unit-data`.

The next complete data block begins at `0x0046AD88`, after the screen and
memory-stack diagnostics. `ending.c` groups the tagged-allocation helpers
starting at `0x0042FA90` with the campaign-ending scenes through `0x0043070F`.
The allocation tag occupies eight bytes, followed by the victory text table,
retreat text pointers, four projectile origins, and two ending-title pointers.
The original reads both dwords of the tag; retain the full eight-byte array.

All eight variables have references confined to this group and are private.
The text pointer initializers contain their own literals, followed by the
four separate use-site `"%X%Y%s%P"` formats. Keeping named character arrays
instead changes alignment and adds gaps. The full `.data` section is 457
bytes, ending with the last format's terminator at `0x0046AF50`.
`verify-unit-data` checks the complete block, all variable offsets, and all
eight pointer targets with one address shift and no string fallback.

The input state at `0x0046AF54` follows those literals. The two initialized
words occupy six bytes in `input.obj`, with the second at offset four.
The communication block starts at `0x0046AF60`, after six bytes of linker
alignment. A single object's two-word prefix would place the first menu
word only eight bytes after the input state, instead of the observed twelve.
The separate objects reproduce this spacing without inventing a variable
in the gap. `input.c` contains the four functions from
`UpdateInputDeviceTransitions` through `PollMenuInputDevices`; `comm.c`
begins with `get_face` at `0x00430BC0`.

The communication declarations occupy 116 bytes, followed by a 404-byte
literal pool. The ace and menu tables contain 17 pointers to separate
literals. In particular, `"Bakhtosh"` requires nine bytes including its
terminator; the former 32-byte packed ace-name array truncated that zero.
The three identical `"%s: %Fs"` formats are separate literals in
`real_vid_transmit`. The pending menu action and menu-label table are private
to this unit. Other declarations remain shared with their original or SDL
callers. `verify-unit-data` checks the complete 520-byte `.data` section,
all 13 declaration offsets, and all 17 pointer targets with one address shift.
It also checks this block's position within the adjacent run from ending
through joystick calibration, described below.

Video mode state follows the communication formats at `0x0046B168`.
The mode word and `PrintPaletteAllocationMap`'s `"%c"` and newline literals
form a complete ten-byte `.data` section. The next initialized table begins
at `0x0046B178`, after those literals and six bytes of alignment, requiring
a separate object. `video.c` contains the four functions from
`ShutdownVideoHook` at `0x004318F0` through `PrintPaletteAllocationMap`.

The graphics-mode table occupies eight bytes at `0x0046B178`. The strings
`"TM.DRV"` and `"SUPERTM.DRV"` follow at `0x0046B180` and `0x0046B188`;
neither has a relocation or disassembled reference in the original. Private
character arrays retain this real data, and the compiler supplies the one
alignment byte between them. These are matching declarations, not recovered
original variable names. The two independent `"j.cal"` literals follow at
`0x0046B194` and `0x0046B19C`, used by `LoadJoystickCalibrationFile`.
All 42 bytes match as `joycal.obj`'s complete `.data` section. The source
window begins with that function at `0x00431A10` and retains the adjacent
rectangle helper through `0x00431EFF`; its internal boundary is provisional.

`verify-unit-data` checks both complete sections and all four declaration
offsets. Ending, input, communication, video, and calibration data also match
as a single 1,050-byte run with all 15 inter-object alignment bytes included.
The boundary between the allocation helpers and ending scenes remains
provisional. These placement windows do not establish original filenames.

DirectDraw's initialized interface pointers, cascade level, and frame-timing
state belong to `dib.c`. The first seven declarations occupy 28 bytes at
`0x0046B1A4`–`0x0046B1BF`; four further declarations occupy 16 bytes at
`0x0046B1C4`–`0x0046B1D3`. The ten variables used only by this unit are
private, while `nFrameIntervalMs` remains shared with the SDL flight code.
The zero words at `0x0046B1C0` and `0x0046B1D4` have no reference anywhere;
they are kept as `DAT_0046b1c0` and `DAT_0046b1d4` at their positions, so the
whole state run matches.

The complete 3,620-byte DirectDraw literal pool follows at `0x0046B1D8`,
ending with `DirectDrawResultToText`'s `"<undefined>"` terminator at
`0x0046BFFB`. `DIBcascade` must spell the surface-lock failure branch before
the success path so their separate diagnostic strings appear in that order.
This pool is checked through `abDIBDiagnosticLiterals` comparison metadata.
With the two words recovered, `dib.obj`'s `.data` section has the observed
3,672 bytes; its uninitialized storage still remains in `globals.c`.

The shared game state begins at `0x0046C000`, after the DirectDraw literal
pool. Its camera scripts, mission records, and maneuver tables precede
`show_info_disp`'s literal pool at `0x0046D9CC`. This definitions-before-literals
order supports their placement together in `game.c`, renamed from the former
information-display-only `text.c`. This is a descriptive filename; the
original spelling is not recovered. All 59 declarations retain their
original address order. Thirteen canned scripts and five defense-maneuver
lists are private because only the pointer tables in this unit reference
them; shared state and tables keep their public declarations.

The four zero bytes at `0x0046C070` have no reference anywhere; they are kept
as `DAT_0046c070`, which also restores the alignment before the first script.
`verify-unit-data` now checks the complete 6,796-byte section.

Policy for unreferenced initialized data: earlier checkpoints left such
words out rather than add "filler" declarations. A byte-identical
executable needs them, so each is now an unidentified `DAT_<address>`
global with its original value at its original position, as the naming
policy prescribes for unidentified globals. Each is placed only where
alignment cannot account for the bytes, and none is referenced by code.

The full 6,652-byte run from `asIntroCameraSequence` at `0x0046C090` through
the final display-string terminator at `0x0046DA8B` matches with one address
shift. It includes 20 navigation records, 48 ship records, both weighted
maneuver tables, all intervening alignment, and the 192-byte display literal
pool. The gate checks all 18 pointer relocations against their positions
within this run, with no string fallback. Array annotations precede their
declarations, and the canned-script lengths are explicit so the ordinary
global-data audit also checks their complete contents.

The briefing and medal unit begins with `LoadBriefingRoom` at `0x00436D00`,
so the `eventmgr.c` placement window ends at `0x00436CFF`. Its initialized
state begins at `0x0046E188`, after the keyboard diagnostic. The character,
portrait, medal, and scene-animation tables precede the medal-name literals
at `0x0046E594`; ten separate text-format literals follow in function order.
The last `"Game Over"` terminator at `0x0046E699` precedes the raster object.
These 27 declarations now reside in `screens.c`, in original address order;
17 are private, while the ten used by other units retain shared declarations.

The character table uses a reconstructed capacity of ten 18-byte records,
with its eight populated initializers unchanged. The original clears ten
activity bytes in `ReturnToBriefingLongShot`, even though its main briefing
loops visit eight characters. Two implicitly zero-initialized records occupy
the 36 bytes from `0x0046E2A8` through `0x0046E2CB`; normal compiler alignment
supplies the four remaining zeros before the medal-position array at
`0x0046E2D0`. No pointer relocation occurs in that tail. This capacity is an
inference from the activity storage and complete data layout, not a uniquely
recovered source declaration. `CloseLook` also retains its original
fourteen-iteration path; the storage reconstruction does not alter that loop.

One complete-section check now covers all 1,298 bytes from `0x0046E188`
through the final string terminator. It verifies all 27 declaration offsets,
all 14 pointer relocations, and the formerly omitted tail with one address
shift. The table-capacity change leaves every function comparison unchanged.
The adjacent raster object has 6,640 initialized bytes, 22 public data
symbols, and no data relocations. Screens, raster, killbrd, and gr now also
match as one 11,486-byte run, including all ten inter-object alignment bytes.
These checks establish relative layout; their absolute addresses still differ.

The debriefing movement table contains 50 signed samples. The original
nonzero bytes at `0x0046E568` and `0x0046E569` are its final two `-2` samples,
followed by two alignment bytes before the first position word. The draw
loop still uses only 48 samples. Keeping ten explicit zero initializers for
`ReturnToBriefingLongShot`'s local flags also removes the extra empty-string
literal produced by `char active[10] = ""`; the complete 262-byte literal
pool now has the original contents, duplicate formats, and alignment.

The shared-data range contains input state at `0x0046DA90`, seven
viewport geometry records at `0x0046DAB8`, and direction/cockpit tables from
`0x0046DB28`. It follows `game.c`'s literals (after the eight-byte section
alignment at `0x0046DA8C`) and precedes the only two literals of the next
units, `DrawTextString`'s `"FATAL : INVALID STRING"` at `0x0046E14C` and
`SetInputKeyState`'s keyboard diagnostic at `0x0046E16C`. A unit's variables
precede its literals, so neither of those units owns the block: it is a
separate data-only object linked before them, `src/data.c`, after `smart.c`
(which has no data). The Super Wing Commander symbols also name a `data`
module. Moving the definitions out of the temporary `globals.c` puts the
whole block at its original address.

The 1,572-byte run from `0x0046DB28` through `0x0046E14B` matches with one
address shift, including all table alignment. Its final twenty bytes contain
ten signed words initialized to `-400`, now retained as `DAT_0046e138[10]`.
No identified direct reference establishes their name or purpose; the scalar
width follows the repeated two-byte values, without claiming the original
array declaration. The earlier 26-byte input-state and 96-byte geometry runs
also match individually. The twelve zero bytes at `0x0046DAAC` are three
unreferenced words, and the sixteen at `0x0046DB18` are a seventh, all-zero
geometry record that no mode selects. The complete 1,724-byte block is
checked as `data.obj`, adjacent to `game.obj`.

The room/kill-board initialized-data block runs from `0x00470090` through
`0x00470673`, after the raster library. Its 25 declarations now reside in
`killbrd.c`; 22 are private, while the pilot-rank names, initial campaign
state, and room-transition flag remain shared. The complete 1,508-byte
section matches with one address shift, including 24 pointer relocations
and the 328-byte literal pool. The initial pilot records retain their inline
name/callsign arrays. The menu-label pointer arrays instead hold separate
compiler-emitted literals, including two writable pilot-name placeholders.

The adjacent words `{158, 28}` at `0x0047049C` and `0x0047049E` are represented
as `ShortPoint stRecRoomPanelOrigin`. Two independent short definitions insert
alignment between them and move the following menu table by eight bytes.
The paired representation preserves the observed four bytes and the complete
section layout. No inbound reference establishes the original symbol or
declaration form; it is not claimed as recovered source spelling.

`ResetCampaignData` at `0x00440800` copies the unit's initial campaign and
pilot records. The shape helpers beginning with `CheckHeapBlockSignature`
at `0x004408A0` use the following graphics literal pool instead: its first
diagnostic is at `0x00470D18`, after the palette and trigonometric tables.
The six helpers through `SignExtendClipCoord` now open `gr.c`, ahead of
`ValidateViewportBounds`. The literal-free packet-section reader at
`0x00440840` retains its provisional placement at the end of `killbrd.c`;
the exact boundary around that one routine remains unproven.

The graphics unit's complete initialized-data section runs from `0x00470678`
through `0x00470E65`. Its 256-byte palette translation table and two 720-byte
absolute trigonometric tables are private to `gr.c`. The following 334 bytes
contain the shape, raster, streamer, and registry literals in function order.
All 2,030 bytes match with one address shift, and the adjacent room/graphics
gate checks the whole 3,542-byte run, including the four alignment bytes
between the two objects.

The two literal references at `0x00441351` and `0x00441361` establish separate
diagnostic functions at `0x00441350` and `0x00441360`. Each pushes its own
name (`__copyline` or `__wipeline`), calls the existing empty raster hook,
cleans the argument, and returns. Both 16-byte spans now match, including
compiler alignment. These unreferenced routines were missing from the seeded
function inventory; their bytes must not be included in `DrawFontGlyph`'s span.

Private C definitions require separate evidence: a trial moving the two
MONODEBG variables to `static` storage in `mono.c` emitted the device handle
before the installed flag, opposite to the original. Reversing their source
declarations did not change that order. Those definitions remain in
`globals.c`; the C++ result does not justify applying the same layout claim
to all private C variables. Superseded: identifier spellings, not declaration
order, decide this order; see "Static `.bss`: `winsys.c` through `gr.c`".

The registry arrays occupy gr's complete 56-byte read-only section, checked
at `0x004631C8` with one address shift and the value-name array at offset
`0x30`. The same routine's `"reg cheating\n"` and `"reg not cheating\n"`
diagnostic literals occupy
writable data. Changing the whole unit's string-storage policy would therefore
be incorrect. The three single-precision constants at `0x004630DC`–`0x004630E7`
encode pi, degrees per radian, and radians per degree; no base-relocated
pointer targets them. Their read-only data must come from a unit linked
between `barracks.obj` and `dib.obj`, none of which has other read-only data,
so they are now `mathutil.c`'s `fPi`, `fDegreesPerRadian` and
`fRadiansPerDegree`, beside the integer min/max helpers. The names follow
the values; the original identifiers are unknown. With them in place the
whole `.rdata` section matches the original except for 21 bytes, all of them
CRT code pointers displaced by the sixteen bytes `sort_viable_target_list`
still lacks; the CRT block holding `28.0f` again starts at `0x00463200`.

The raster library is the one assembled object, and its edges are proven.
CL pads every optimized function with NOPs to a 16-byte boundary, so the
`mov edi, edi` after `FillRasterBytes` closes `screens.obj` at `0x439C40`. The
library's own functions are unpadded, and LINK fills the gap after it with
INT3, the only such run in the core, up to `killbrd.obj` at `0x43F640`. Its
`.data` (`0x0046E69C`–`0x0047008B`) follows the same order, between the
"Game Over" literal of `screens.c` and the kill board tables.

`src/globals.c` now supplies only the SDL raster tables; native storage is
owned by the original units, including `raster.asm`. It is excluded from
the native source list and retained in the SDL build and audit manifest.
Linking without its former empty object produces a byte-identical executable
after timestamp normalization, as checked with `binary-comp byte-compare`.
It does not affect function matching or addresses. Move a global only when
ownership and declaration order are proven. When a boundary changes, update
this table and `SRCS_ORDERED_CORE` together, then run `make order` and
`make verify`.

The unreferenced music wait routine at `0x0042E970` reads a word at
`0x0046AA0C` that the seeded global list omitted. Its original value is one.
This is `nMusicWaitForInput` in `music.c`: when nonzero, it keeps polling for
input even when the music status word is zero. The routine also respects the
existing music-mode and wait-enable settings.

Several unreferenced event and graphics routines likewise lie between entries
in the seeded function map. Recovered entries are recorded in `src/map`.
Do not replace such gaps with padding. The apparent
entry at `0x004393F0` lies in a switch table/padding range and is not a function.

The boundary supplement now holds only the four reproduced array
initializers. The 26 empty or constant-return routines it formerly listed
are implemented at their original positions: one closes the event-manager
control entries in `strdos.c`, 24 lie in `eventmgr.c`, and the return-one
entry at `0x00442740` follows `ix_log_printf` in `ixlog.cpp`. Each occupies
sixteen bytes with compiler padding, 416 bytes in all, and each is exact.
The four `RET 4`/`RET 8` entries at `0x00436300`–`0x00436330` are
`__stdcall`; their argument counts are the only interface the retail bodies
reveal. Nothing in WC1 or in WC2's matching runs calls any of them, the
1990 DOS event library is far-pascal assembly in a different order, and the
other ports supply no names, so their names only place them among their
neighbours (cursor, frame tick, memory service, keyboard, viewport, joystick,
shutdown) and their comments say so. Adding them changed no other report row.
The 432-byte rectangle routine is reconstructed before `FillGraphicSuffix`
at `0x00436C70`.

The entry at `0x00436390` is now `ResetInputKeyStatePreservingModifiers`.
The original jumps directly to `ClearInputKeyStatePreservingModifiers`, whose
key-array clear, retained Ctrl/Alt bytes, and debug-pause reset establish the
forwarder's behavior. A normal C call lets CL emit the tail jump; no assembly
is written by hand. Binary-comp verifies all sixteen bytes, including padding,
literally, and its decoded rebuilt target resolves to the existing reset
routine. The entry has no identified callers elsewhere. Its prototype was
initially visible only to `eventmgr.c` through `WC1_EVENTMGR_SOURCE`.
Exposing that unused declaration to every unit changed 57 report rows and
lost five net exact matches; scoping the declaration preserved all other
units and every pre-existing event-manager result. The recovered boundary
now comes from the source annotation rather than the supplement.
The later header cleanup removes the unused prototype and source marker
entirely. `PeekInputEvent` uses the common named prototype, and
`IsVectorWithinRange` no longer stores its single-use range expression in
a temporary. This combination preserves the complete original executable;
see the event-manager header cleanup in `docs/PATTERNS.md`.

The empty entry at `0x004368D0` is now `DosInterruptHandlerHook`, retained
in `eventmgr.c` between `MouseIdleHook` and `GetNavRangeSentinel`. Its role
comes from a relocated pointer operand at `0x0042752C`: startup passes this
entry with vector `0x21` to the legacy vector-setting hook. Cleanup restores
the previously saved vector. The former source incorrectly passed the
neighboring mouse hook at `0x004368C0`. The original contains only `RET`
and compiler padding; a plain empty C function reproduces its sixteen bytes.
Its original name and unused parameter list are not recoverable from that
body. The minimal void signature preserves its return convention, and its
`wc1funcs.h` prototype is visible to its caller through `WC1_MAIN_SOURCE`.
The boundary supplement no longer supplies this implemented entry.

At the autopilot/input size checkpoint of 2026-10-18, the 1,521-function
report has 1,235 normalized exact matches, 261 same-size differences, and
25 size differences totaling 166 absolute bytes (net +10). The rebuilt
`.text` is 896 bytes shorter than the original; the missing aligned spans
account for 880 of those bytes. The data virtual-size deficit remains 1,856
bytes. Recovering the three input/autopilot spans removed 48 excess bytes
but shifted downstream code: strict positional matches fell from 122,403
to 53,323 of 518,656 bytes (10.2810%). This is an intermediate function-level
improvement, not a whole-file match; the strict comparison still fails.

The subsequent cursor/frame-rate checkpoint has 1,238 normalized exact
matches, 258 same-size differences, and the same 25 size differences
(166 absolute bytes, net +10). Cursor-resource initialization and the
space-flight frame-rate adjustment now match their complete spans;
packet seeking drops from four differing instructions to two. The other
1,518 report rows are unchanged from the palette/answer-loader checkpoint.
The code and data deficits remain unchanged. Strict positional matches
are 53,324 of 518,656 bytes (10.2812%), two fewer than that checkpoint at
this still-incorrect layout. The file remains 1,024 bytes short, and the
strict whole-file comparison still fails.

At the startup/key-reset checkpoint, the report covers 1,522 functions:
1,241 normalized exact matches, 257 same-size differences, and 24 size
differences (150 absolute bytes, net -6). The added sixteen-byte reset entry
and removal of `main`'s sixteen excess bytes leave the total code-section
deficit at 896 bytes. The remaining 28 missing spans account for 864 bytes.
Three existing function rows improve and the recovered entry is exact;
the other 1,518 rows are unchanged. Literal whole-file matches rise by 616
to 53,940 of 518,656 bytes (10.4000%). The file-size and data virtual-size
deficits remain 1,024 and 1,856 bytes respectively; the strict comparison
still fails.

The subsequent callback checkpoint covers 1,523 functions: 1,242 normalized
exact matches, 257 same-size differences, and the same 24 reported size
differences (150 absolute bytes, net -6). All 1,522 pre-existing report rows
are unchanged. The two corrected callback targets were hidden by address
normalization; the added code-pointer gate independently verifies 39
references in 26 routines. The new empty callback reduces the code-section
deficit to 880 bytes, of which 848 are the remaining missing spans. The
known import-stub boundary artifact described in `docs/EXPORT.md` remains
among the reported size differences. Literal whole-file matches increase
by 103 to 54,043 of 518,656 bytes (10.4198%). The file remains 1,024 bytes
short and the data virtual-size deficit remains 1,856 bytes. The strict
whole-file comparison still fails. Win32 and modern builds, the required
verification gates, and both deliberately incorrect callback checks pass;
the existing eleven call-analysis and 37 global-access diagnostics remain.

At the save-game checkpoint, the same 1,523-function report has 1,243
normalized exact matches, 256 same-size differences, and 24 size differences
(150 absolute bytes, net -6). `LoadGameFromSlot` matches its full 336-byte
span; `SaveGame` drops from 84 differing encodings to four, and
`CreateEmptySaveGameFile` drops from 36 to fourteen. All other 1,520 report
rows are unchanged. The loader's two campaign-state stores and subsequent
dataset reload also pass binary-comp's relocated-pointer check against
their named globals, independently of instruction address normalization.
The code, data virtual-size, and file-size deficits remain 880, 1,856, and
1,024 bytes. Strict positional matches are 54,040 of 518,656 bytes
(10.4192%), three fewer at this still-incorrect layout; whole-file equality
still fails. Both builds and the required verification gates pass, with
the same eleven call-analysis and 37 global-access diagnostics.

The console/disk checkpoint has 1,244 normalized exact matches, 255 same-size
differences, and the same 24 size differences across 1,523 functions.
`SortSignedByteValuesAscending` also passes a separate literal comparison of
all 112 bytes, including padding. The console's `DrawPendingLines` drops from
twelve differing encodings to nine, and `WaitForInputKey` drops from 31 to
fifteen. All other 1,520 report rows, including every previous exact result,
are unchanged. Average mnemonic similarity remains 99.55%; it is not a
whole-executable completion percentage. The code, data virtual-size, and
file-size deficits remain 880, 1,856, and 1,024 bytes respectively. Literal
whole-file matches rise by four to 54,044 of 518,656 bytes (10.4200%); strict
equality still fails. Both builds and the required gates pass, with the same
eleven call-analysis and 37 global-access diagnostics. The executable hash
is unchanged through verification, so the final report describes the checked
binary. No compilation-unit boundary or link-order change is involved.

The target-sort checkpoint preserves all 1,523 headline report rows and all
1,244 normalized exact matches. Its detailed binary-comp byte comparison
reduces `sort_viable_target_list` from 24 differing encodings to fifteen and
recovers the original eight-byte frame; the aligned span remains sixteen
bytes short. A separate baseline-to-current byte comparison confirms that
the other 63 ship functions are unchanged under address normalization.
The code, data virtual-size, and file-size deficits remain 880, 1,856, and
1,024 bytes. The relocation section is now 36,286 bytes versus the original
36,306: the remaining cached distance pointer removes an absolute reference.
Strict positional matches are 54,043 of 518,656 bytes (10.4198%), one fewer
than the preceding checkpoint at this still-incorrect layout. Whole-file
equality still fails. Both builds and the required verification gates pass,
with the same eleven call-analysis and 37 global-access diagnostics. The
executable hash remains unchanged through verification. Audio storage-order
and comparison-operand experiments are discarded; their findings are recorded
above and in `docs/PATTERNS.md`.

The DSP declaration checkpoint raises normalized exact matches from 1,244
to 1,249 across the same 1,523 functions. Six report spans become exact,
including the 694-byte stream allocator and 261-byte stream volume setter;
the stream-release guard loses its former exact match. The ten stream
routines' detailed differing-encoding total falls from 191 to 41, including
the unlock guard's regression. All other 1,514 headline report rows remain
unchanged. There are now 253 same-size differences and 21 size differences
(146 absolute bytes, net -6); average mnemonic similarity remains 99.55%.
The declaration ordering preserves the voice owner's previous output and
the 74 compiler-generated array-initializer bytes and registrations.

No function body, calling convention, storage definition, compilation-unit
boundary, or link order changes in this checkpoint. The sound-system BSS
offsets above remain unresolved. The code, data virtual-size, and file-size
deficits remain 880, 1,856, and 1,024 bytes; relocation-section sizes remain
36,306 original and 36,286 rebuilt. Strict positional matches rise by 36 to
54,079 of 518,656 bytes (10.4268%), but whole-file equality still fails.
Both builds and the required verification gates pass, with the same eleven
call-analysis and 37 global-access diagnostics. Both value checks cover all
1,523 functions with zero mismatches or skips, and the 1,097 linked globals
and 39 embedded function pointers retain zero mismatches. The executable
hash is unchanged through verification.

The mixer declaration checkpoint raises normalized exact matches from 1,249
to 1,250. Its service routine now matches its complete 932-byte span and 262
instructions; the thread routine retains its exact 1,180-byte span. Moving
the existing buffer-size and write-offset extern declarations to the front
of their local block recovers both comparison operand orders without changing
function bodies, types, storage, flags, module boundaries, or link order.
All other 1,522 headline report rows remain unchanged. The report now has
252 same-size differences and 21 size differences (146 absolute bytes,
net -6), with average mnemonic similarity still 99.55%.

The new mixer-reference verification requires all 79 operands referring to
ten identified globals to retain their original targets and function-relative
positions. It rejects two independent operand swaps in temporary executable
copies even though both altered routines still score normalized exact. This
guards against the hidden target exchange observed when unused mixer externs
were removed during a discarded probe; it is not an absolute-layout check.

Both builds and the required verification gates pass. The 1,097 linked
globals, 39 embedded function pointers, and both value checks across all
1,523 functions retain zero mismatches; the eleven call-analysis and 37
global-access diagnostics remain. The executable hash is unchanged through
verification. Code, data virtual-size, relocation-section, and file sizes
are unchanged from the preceding checkpoint. Strict positional matches
remain 54,079 of 518,656 bytes (10.4268%), and whole-file equality still fails.

The stream-preparation checkpoint raises normalized exact matches from 1,250
to 1,251 by placing the allocation-count declaration before the stream-count
declaration for the stream owner. `ix_dsps_prepare` now matches all 342 bytes
and 95 instructions. The added byte consumes one final alignment byte, so
`ix_dsps_unlock`'s span changes from four excess bytes to three; its guard
still differs. All other 1,521 headline report rows are unchanged. There are
now 252 same-size differences and 20 size differences (144 absolute bytes,
net -6), with average mnemonic similarity still 99.55%.

The ten stream routines' detailed differing-encoding total falls from 41 to
34. An additional binary-comp relocated-pointer check verifies the identities
and order of 86 operands referring to their five globals, including array
field offsets; the six exact routines also retain their operand positions.
No function body, type, calling convention, storage definition, compiler
flag, unit boundary, or link order changes. The counter-ownership and
sound-system declaration experiments described above are reverted.

Both builds and the required verification gates pass, with the same eleven
call-analysis and 37 global-access diagnostics. Both value checks cover all
1,523 functions without mismatches or skips; the 1,097 linked globals, 39
embedded function pointers, and 79 mixer-global operands also pass. The
executable hash is unchanged through verification. Section sizes and the
1,024-byte file deficit remain unchanged. Strict positional matches are
54,072 of 518,656 bytes (10.4254%), seven fewer at this still-incorrect
layout; whole-file equality still fails.

The sprite-capture checkpoint improves `CaptureSpriteBackground` from
84.55% to 91.82% mnemonic similarity and from 168 to 94 differing instruction
encodings out of 220. Its original 752-byte span and forty-byte local frame
are preserved. The retained declaration, word-copy, qualified-access, and
cursor-update forms are described in `docs/PATTERNS.md`. Every other 1,522
function-report row remains unchanged, including sprite restoration and
the transformed-bounds routine. No global definitions, compiler flags,
compilation-unit boundaries, or link order change.

The report retains 1,251 normalized exact matches, 252 same-size differences,
and twenty size differences (144 absolute bytes, net -6). Average mnemonic
similarity rises to 99.56%; 1,512 functions reach 90%, leaving eleven below
that threshold. Both builds and all required verification gates pass.
The 1,097 linked globals, 39 embedded function pointers, 79 mixer-global
operands, and both value checks across all 1,523 functions report zero
mismatches; the value checks skip none. The eleven call-analysis and 37
global-access diagnostics remain unchanged. The modern build retains its
existing `sprintf` deprecation and packed-pointer/alignment warnings.

The executable hash is unchanged through verification. The code, data
virtual-size, and file-size deficits remain 880, 1,856, and 1,024 bytes.
Strict positional matches are 54,064 of 518,656 bytes (10.4239%), eight
fewer at the current incorrect layout. The improved function comparison
does not constitute whole-file progress by that positional measure, and
literal executable equality still fails.

The sprite-restoration checkpoint applies the corresponding memory-access
and cursor-update forms to `RestoreSpriteBackground`. It improves from
84.82% to 91.52% mnemonic similarity and from 173 to 109 differing encodings
out of 224, preserving the original 768-byte span and forty-byte frame.
All other 1,522 report rows remain unchanged. Exact, same-size, and different-
size counts remain 1,251, 252, and twenty; average mnemonic similarity is
99.56%. There are now 1,513 functions at or above 90% and ten below it.

Both builds and all required verification gates pass. Both value checks
cover all 1,523 functions without mismatches or skips; the 1,097 linked
globals, 39 embedded function pointers, and 79 mixer-global operands also
pass. The eleven call-analysis and 37 global-access diagnostics and the
existing modern-build warnings remain. The executable hash is unchanged
through verification. Section sizes and the 880-byte code, 1,856-byte data
virtual-size, and 1,024-byte file deficits are unchanged. Strict positional
matches rise by four to 54,068 of 518,656 bytes (10.4246%); literal executable
equality still fails.

The rectangle routine's stack reads need particular care. Relative to ESP
after its four register saves, its two arrays of sixteen 32-bit coordinates
start at offsets `0x24` and `0x64`. The branch at `0x00436B97` compares the
first X slot (`[ESP+0x24]`) with four; that slot was already initialized
from the first rectangle's left coordinate. One selected index likewise
loads `[ESP+0x14]`, where that coordinate was saved. Only the other two
index slots, `[ESP+0x18]` and `[ESP+0x1C]`, have no preceding writes in the
routine. Its four result stores alternate between offsets zero and two,
so the maximum calls overwrite the earlier minimum results.

Probes using two sixteen-element arrays, an uninitialized count, and three
selected indices recovered the 432-byte span but only 89.66% instruction
similarity, with different stack reads. Direct field expressions, LONG
coordinate elements, a two-plane array, and a fourth selected-index element
did not recover the original accesses; the fourth element also enlarged the
frame from 148 to 152 bytes. These probes are not retained. In particular,
do not infer four independent uninitialized scalars merely from the final
branch and index loads: some of those slots already contain coordinates.

A follow-up rectangle probe explicitly tested `x[0]` and initialized the
first selected index from the first rectangle's left coordinate. With three
index slots it emitted a 144-byte frame; four slots restored 148 bytes but
still tested the cached left coordinate at `[ESP+0x10]` instead of the
original `[ESP+0x24]`. Both reached only 85.34% mnemonic similarity. The
compiler also cached a selected coordinate across minimum/maximum calls
where the original reloads it. Sequencing those calls through a real result
temporary, volatile X-array accesses, and volatile selection loads did not
recover the original accesses; the latter enlarged the frame to 152 bytes.
These probes are not retained; they did not recover the rectangle routine.

The compiler slot reuse proven in `SetFleetOverviewView` makes overlapping
uninitialized locals a possible explanation, but does not establish this
rectangle routine's declarations. A three-scalar index probe coalesces all
three reads into the same X-coordinate slot and shortens the frame to 140
bytes. Volatile scalars read separate slots but enlarge it to 152 bytes.
A four-element selection array with an uninitialized count gives 148 bytes
when loading bottom coordinates before right coordinates, but puts both
the count and the selection at the wrong end of the frame. Initializing
its first element from the left coordinate enlarges the frame to 152 bytes
again. Separating the coordinate temporaries and selection array into
scopes gives the original 148-byte frame and 432-byte span, but still reads
the wrong slots and differs in all 116 compared instruction encodings.
These additional probes are also discarded; matching the span alone does
not reconstruct the observed behavior.

A single 32-element coordinate array produces the same unsuccessful
148-byte frame and 432-byte span as the two-array scoped trial: 89.66%
mnemonic similarity, with all 116 instruction encodings different. Keeping
three volatile selected-index scalars in the later scope and capturing each
once also gives a 148-byte frame, but places the guard and index reads at
the end of that frame and shortens the span by sixteen bytes (88.79%).
Neither reproduces the three explicit index shifts. Both are reverted,
including the exploratory shared prototype, whose insertion also changed
unrelated compilation units.

The cross-release rectangle recovery resolves the missing source structure.
WC2's original routine at `0x004651B7` constructs the same sixteen coordinate
pairs and performs the same nested minimum/maximum calls and overwriting
result stores. Its original caller at `0x0043F110` passes cockpit and VDU
rectangles, supporting the descriptive name `GetViewportIntersection`.
The WC2 disassembly, rather than its reconstructed C, is the useful evidence:
it initializes the point index but never initializes the inside count.
Its loop tests the point index's postincrement; the comparison with the point
count is a discarded iteration expression. The first condition is zero,
so the body does not execute. The reconstructed WC2 C initializes the inside
count and uses a different loop form; neither detail can be copied here.

Retaining the reversed `for` clauses, the uninitialized inside count, the
four-element inside array, and the coordinate construction lets CL 4.20
produce WC1's 148-byte frame and 432-byte span. This full source structure
recovers the guard at `[ESP+0x24]`, the three selected-index reads at
`[ESP+0x14]`, `[ESP+0x18]`, and `[ESP+0x1C]`, the coordinate arrays at
`[ESP+0x24]` and `[ESP+0x64]`, and all three explicit index shifts by two.
No padding, dummy locals, hand-written stack accesses, or assembly is added.
The short return and `__stdcall` convention follow WC1's AX results and
`RET 0xC`. Both final maxima still overwrite the left/top fields, as shipped.
An alternative `do`/`break` loop emits 608 bytes and only 69.70% mnemonic
similarity; the recovered `for` form emits 432 bytes at 96.55%, with 79 of
116 instruction encodings still different in register allocation and
scheduling.

The new prototype is visible only to `eventmgr.c` and omits parameter names.
A named prototype loses the exact matches of `RetainInputEventsOfType` and
`GetNextInputEvent`; moving it elsewhere in the header does not help.
Omitting parameter names preserves every existing exact match. The required
declaration still changes `ReleaseInputEvent` from ten to thirteen differing
encodings and `shrink_vector` from nine to eight. The other 1,521 existing
report rows are unchanged. The new annotation replaces the boundary-map
entry, reducing the unimplemented inventory from 27 spans/848 bytes to
26 spans/416 bytes.

The rectangle checkpoint compares 1,524 source functions: 1,251 normalized
exact, 253 same-size differences, and twenty size differences (144 absolute
bytes, net -6). Average mnemonic similarity remains 99.56%, with 1,514
functions at or above 90% and ten below it. The source-order inventory and
address audits pass. Reversing `shrink_vector`'s source operand order does
not change its eight differing encodings and is not retained.

Both builds and all required verification gates pass. Both value checks
cover all 1,524 functions without mismatches or skips. The 1,097 linked
globals, 39 embedded function pointers, and 79 mixer-global operands retain
zero mismatches. Call analysis checks 1,138 functions with the same eleven
diagnostics; global-access analysis checks 1,059 with the same 37. The
modern compiler warns about the deliberately discarded loop comparison,
in addition to the existing warnings. No gameplay validation is claimed.

The executable hash is unchanged through validation and the restored
operand-order probe. Adding the recovered 432-byte routine reduces the
code-section deficit from 880 to 448 bytes; 416 bytes of that remaining
deficit are the missing small routines. File size rises from 517,632 to
518,144 bytes, now 512 bytes short. The data virtual-size deficit remains
1,856 bytes, and the relocation section remains 36,286 bytes versus 36,306
original. Strict positional matches rise by 3,423 to 57,491 of 518,656 bytes
(11.0846%). Literal executable equality still fails.

## Audio uninitialized-data ordering checkpoint

The name-dependent COFF ordering described above recovers the DSP, stream,
voice, and sound-system storage as one 2,340-byte adjacent run. Fifteen
descriptive identifier changes suffice. The source changes are identifier
substitutions and address comments; types, initializers, logic, conventions,
compiler settings, and link order stay unchanged. The handwritten mixer's
external voice-array symbol is renamed consistently.

The complete report retains all 1,524 previous rows unchanged: 1,251
normalized exact, 253 same-size differences, and twenty size differences
(144 absolute bytes, net -6). Average mnemonic similarity remains 99.56%,
with ten routines below 90%. A separate binary-comp reference check verifies
379 operands across 58 functions against the 26 audio globals, including
array field offsets. Target order and multiplicity match throughout;
instruction offsets also match in every normalized-exact routine.

The permanent unit-data gate now covers all four audio sections and their
adjacency. Four negative map probes exchange two named positions in each
block; each is rejected. The seven sound-system definitions now carry
original-address annotations and participate in the collected global audit,
increasing its coverage from 1,097 to 1,104 entries. The existing 79-operand
mixer gate follows the renamed symbols without changing its checking policy.

The stream section shrinks from 124 to its original 120 bytes, changing
subsequent linker alignment. The complete `.data` virtual size is now
1,329,724 versus the original 1,331,596: a 1,872-byte deficit. The recovered
audio run still starts at the wrong absolute address. File size remains
518,144, code size 400,663, and relocation size 36,286. The file is still
512 bytes short and code 448 bytes short; the 26 missing routines account
for 416 bytes of the latter.

Strict positional matches increase by thirty to 57,521 of 518,656 bytes
(11.0904%). The executable SHA-256 is
`dc3590e78d6e0ee89feaadd9200479f6380b9e746712ade647112c493ce67445`.
Literal executable equality remains incomplete.

Both builds, the source-order and address audits, and all required
verification gates pass. Both value checks cover all 1,524 functions with
zero mismatches or skips. The 1,104 linked globals, 39 code pointers, and
79 mixer operands pass. Call analysis retains eleven diagnostics across
1,138 checked functions; global-access analysis retains 37 across 1,059.
The executable hash stays unchanged through the final audit expansion and
verification. No gameplay validation is claimed.

## Core declaration-scope checkpoint

Superseded: the profiler variables are now `main.c` statics (see
"Static `.bss`: `winsys.c` through `gr.c`"), so no other unit declares them.

The profiler storage probe identifies a useful scope correction without
moving its definitions: the event-manager, startup, training, and video
units do not reference the twelve flight-profiler globals. Their owner
markers now exclude those declarations. The event owner also omits the
parameter names in `PeekInputEvent`'s prototype; its external callers retain
their existing declaration. No function body, type, calling convention,
storage definition, compiler flag, or object order changes.

Six functions become normalized exact:

- `ReleaseInputEvent`: thirteen differing encodings removed.
- `GetViewportIntersection`: all 432 bytes now pass the function comparison,
  including its original frame, register allocation, and optimized remnants
  of the reversed `for` clauses; formerly 79 of 116 encodings differed.
- `initialize_direction_view_frames`: eight differing encodings removed.
- `SortTrainSimHighScores`: eight differing encodings removed.
- `InitializeTrainSimHighScores`: eighteen differing encodings removed.
- `ReleaseContiguousPaletteEntries`: five differing encodings removed.

The cursor capture, draw, and restoration routines each retain only two
differing encodings, down from twenty, fifteen, and seventeen. The background
ship renderer improves from 21 to eleven, palette reservation from nine to
three, and `RunTrainSim` from thirteen to twelve. Three collateral changes
are retained: `GetNextInputEvent` loses its exact match to an EDI/EDX exchange
in six encodings, `ReadTextInput` rises from nine to eleven, and
`UpdateTrainSimHighScores` from eight to nine. The result is 185 fewer
differing instruction encodings across fifteen changed report rows; the
other 1,509 rows are unchanged. Attempts to remove the event-reading
regression are recorded in `docs/PATTERNS.md` and are not retained.

The complete report compares 1,524 functions: 1,256 normalized exact,
248 same-size differences, and twenty size differences (144 absolute bytes,
net -6). There are 1,370 mnemonic-perfect routines and 1,514 at or above 90%;
average mnemonic similarity rises to 99.57%. The 26 missing routines still
account for 416 bytes. Code remains 448 bytes short, data virtual size 1,872
bytes short, and file size 512 bytes short.

Both builds, source-order and address audits, and all required verification
gates pass. Both value checks cover all 1,524 functions without mismatches or
skips. The 1,104 linked globals, 39 code pointers, 79 mixer operands, and the
2,340-byte audio storage run retain their passing results. Call analysis
retains eleven diagnostics across 1,138 checked functions; global-access
analysis retains 37 across 1,059. No gameplay validation is claimed.

The executable SHA-256 remains
`60ac420938d363ad255164dc3543eafd127f21f0c737b71280ca3787f1907be9`
through final verification. Strict positional matches increase by two to
57,523 of 518,656 bytes (11.0908%). Literal executable equality still fails.

## Graphics declaration-scope checkpoint

Superseded: the five graphics/streaming variables are now `gr.c` statics (see
"Static `.bss`: `winsys.c` through `gr.c`").

The five provisional graphics/streaming globals remain in `globals.c` while
storage recovery continues. Their declarations are now omitted from the
DIB, ejection, joystick-calibration, developer-console, navigation, and
screen-sequence units. Each owner defines its marker before including
`wc1.h`. All five globals are used only by `gr.c`, so the scoped units lose
no dependency. Function bodies, types, conventions, storage, compiler flags,
and link order remain unchanged.

Fourteen of the 1,524 function-report rows change, with 354 fewer differing
instruction encodings overall; the other 1,510 rows are unchanged. Eight
routines become normalized exact:

| Function | Previous differing encodings |
| --- | ---: |
| `DIBslamReal` | 2 |
| `MeasureScaledIntroTextWidth` | 12 |
| `GetLineLength` | 12 |
| `NavMapLabelPositionAvailable` | 11 |
| `FindNavMapLabelAtPoint` | 10 |
| `DrawNavSquareMarker` | 23 |
| `DrawMedalLongShot` | 10 |
| `PanToScreen` | 115 |

`CalibrateJoystickInteractive` improves from 213 to 39 differing encodings,
with mnemonic similarity rising from 96.39% to 98.36%. `ejection_sequence`
improves from 88 to 85. Four small regressions accompany these gains:
`UpdateInflightNavText` changes from exact to five differing encodings,
`ReturnToBriefingLongShot` from exact to nine, `EstablishingShot` from seven
to nine, and `SceneDirector` from two to four. The nav and screens probes
that omit only the raster declarations or only the streaming declarations
lose additional exact matches; both are reverted.

The complete report now has 1,262 normalized exact matches, 242 same-size
differences, and twenty size differences (144 absolute bytes, net -6).
There are 1,372 mnemonic-perfect routines; 1,514 remain at or above 90%,
with ten below. Average mnemonic similarity remains 99.57%.

Both builds, source-order and address audits, and all required verification
gates pass. Both value checks cover all 1,524 functions with zero mismatches
or skips. The global-data audit retains 1,104 matches without missing
symbols; the 39 code pointers and 79 mixer operands also pass. A separate
binary-comp reference check covers 65 named-global operands in the eight
newly exact functions. Their targets, order, multiplicity, and
function-relative relocation offsets all match. Call analysis retains
eleven diagnostics across 1,138 checked functions; global-access analysis
retains 37 across 1,059. No gameplay validation is claimed.

The file remains 518,144 bytes versus 518,656, the code section 400,663
versus 401,111, and the data virtual size 1,329,724 versus 1,331,596.
The 26 missing routines still account for 416 of the 448 absent code bytes.
Strict positional matches increase by 26 to 57,549 of 518,656 bytes
(11.0958%); literal executable equality still fails. The executable SHA-256
matches the full-report snapshot and remains unchanged through validation:
`42802b70085e54fe0bffd9f8f688f2ca7bc0208d59e5dcaf21faf8e480e5c7a8`.

## Font and cockpit-bar size checkpoint

`DrawFontGlyph` now occupies its original 512 bytes instead of 528.
Declaration ordering retains the font source in EAX, the row induction value
in ESI, and the viewport in ECX. Initializer expressions execute in their
previous order, and the 24-byte local frame is unchanged. Mnemonic similarity
rises from 89.02% to 91.98%, with 61 differing encodings over the complete span.

`vdu_polygon` now occupies its original 464 bytes instead of 432.
Declaring the frame bytes first and capturing bounds after their global
stores recovers the original saved values and stack slots. Qualified updates
and reads preserve the original viewport memory operations instead of
substituting cached bounds. The function reaches 98.36% mnemonic similarity
with four differing encodings: divisor/index scheduling and the exchanged
EAX/EDX multiplication inputs. The detailed source patterns and rejected
alternatives are recorded in `docs/PATTERNS.md`.

Only these two rows change in the complete report; the other 1,522 are
unchanged. The report retains 1,262 normalized exact functions, now with
244 same-size differences and eighteen size differences (96 absolute bytes,
net +10). There are still 1,372 mnemonic-perfect functions. The count at or
above 90% rises to 1,516, leaving eight below that threshold, and average
mnemonic similarity rises to 99.58%.

Both builds, source-order and address audits, and all required verification
gates pass. Both value checks cover 1,524 functions with zero mismatches or
skips. All 1,104 linked globals, 39 code pointers, and 79 mixer operands pass.
Call analysis retains eleven diagnostics across 1,138 checked functions.
Global-access diagnostics decrease from 37 to 36 across 1,059 checked
functions: `vdu_polygon` now includes its original left/top viewport reads.
No gameplay validation is claimed.

The code section grows by sixteen bytes overall to 400,679, leaving a
432-byte deficit against the original 401,111. The 26 missing routines still
account for 416 bytes. Relocation data grows from 36,286 to 36,294 bytes,
versus the original 36,306, as the recovered memory operands add their
relocations. Data virtual size remains 1,329,724 versus 1,331,596, and the
file remains 518,144 versus 518,656 bytes. The entry point moves to
`0x0044E5C0`, still below the original `0x0044E770`.

Strict positional matches rise by 24,872 to 82,421 of 518,656 bytes
(15.8913%). Literal executable equality still fails. The executable hash
remains unchanged between the full report and final verification:
`d8961a1ea5b2ccd7ec554597fac255defb6a55a277c6e2c03d4b344885393ca5`.

## HUD text exact checkpoint

`print_message_text` now matches its complete original 512-byte span instead
of occupying 528 bytes. Numeric locals precede the cursor pointers, and the
promoted character position is updated before the newline store. These
choices recover the original registers and subtraction. Removing a redundant
width temporary preserves allocation in the later cockpit functions. The
220-byte local frame, all 143 instructions, four-entry switch table, and
padding match after address normalization. The original forward newline
offset is preserved.

Only this row changes in the full report; the other 1,523 are unchanged.
There are now 1,263 normalized exact functions, 244 same-size differences,
and seventeen reported span differences (80 absolute bytes, net -6).
The mnemonic-perfect count rises to 1,373. There are still 1,516 functions
at or above 90%, eight below, and 99.58% average mnemonic similarity.

Additional launcher parsing and cockpit-bar expression probes produce no
retained changes. The launcher still holds zero in EBX instead of the
original prefix character, leaving its 272-byte span sixteen bytes short.
Source forms that recover its size through different instructions are
rejected. `docs/PATTERNS.md` records these experiments and the HUD correction.

Both builds, source-order and address audits, and all required verification
gates pass. Both value checks cover 1,524 functions with zero mismatches or
skips. The global-data audit retains 1,104 matches, and all 39 code pointers
and 79 mixer operands pass. Call analysis retains eleven diagnostics across
1,138 checked functions; global-access analysis retains 36 across 1,059.
No gameplay validation is claimed.

Code now occupies 400,663 bytes versus 401,111, a 448-byte deficit. The 26
missing routines still account for 416 bytes. The data virtual size remains
1,329,724 versus 1,331,596, relocation data 36,294 versus 36,306, and the file
518,144 versus 518,656. The entry point is `0x0044E5B0` versus `0x0044E770`.

Strict positional matches fall to 57,394 of 518,656 bytes (11.0659%), with
461,262 differing positions in 22,917 runs. The corrected HUD size removes
an accidental cancellation of the launcher's sixteen-byte deficit:
`print_message_text` still starts at `0x00416250` versus `0x00416260`, while
the following `ShowHudTextLine` moves from `0x00416460` to `0x00416450`
against the original `0x00416460`. This exposes the earlier displacement
through more subsequent code even though the HUD function is now exact.
Literal file equality still fails; positional percentage is not a measure
of how much source has been reconstructed. The executable SHA-256 remains
unchanged between the full report and final verification:
`ab859f71414cc6c4d78064f162b292a101bdd4f298e9e5fa73871178879b46fb`.

## Win32 declaration-scope checkpoint

Superseded: the profiler and raster declarations no longer exist (see
"Static `.bss`: `winsys.c` through `gr.c`").

The Win32 shell now omits twelve unused flight-profiler declarations and
three unused raster declarations. Storage, types, calling conventions,
compiler flags, and link order are unchanged. `make_shard` becomes exact
over its original 272-byte span without changing its body. Preserving the
F1 latch's shared initial write in both repeat-state branches then recovers
the complete 1,264-byte `MainWindowProc`. Saving the first heap-scan block
address before scanning preserves `CheckAllGuardedAllocations`' existing
112-byte exact match under the new declaration scope.

Only three full-report rows change: `make_shard` improves from 67 differing
encodings to exact, `MainWindowProc` from 28 to exact, and
`FreeGuardedAllocation` changes from exact to one reversed tail-pointer
comparison. Its size, control flow, and remaining 78 instructions match.
The other 1,521 rows are unchanged. This removes 94 differing encodings
overall and leaves 1,264 normalized exact functions, 243 same-size
differences, and seventeen reported span differences (80 absolute bytes,
net -6). There are 1,375 mnemonic-perfect routines, 1,516 at or above 90%,
eight below, and 99.58% average mnemonic similarity.

The documented shell/system boundary is corrected to `0x00402E80`.
`FreeGuardedAllocation` starts at `0x00402DB0` and occupies 208 bytes;
`SetMousePosition` begins the system-services unit at its end. The prior
`0x00402E00` documentation split the heap-release routine. No functions or
objects move between units.

The launcher remains 272 bytes against the original 288. None of the tested
profiler/raster/stream declaration scopes changes its EBX constant choice.
The selected scope and rejected mouse, heap, and prototype experiments are
recorded in `docs/PATTERNS.md`.

Both builds, source-order and address audits, the order report, and all
required verification gates pass. Both value checks cover 1,524 functions
with zero mismatches or skips. The global-data audit retains 1,104 matches;
all 39 code pointers and 79 mixer operands pass. A separate binary-comp
reference check verifies 32 named-global operands in the two newly exact
routines and the restored heap scan, including target identity, multiplicity,
order, and function-relative relocation offsets. Call analysis retains eleven
diagnostics across 1,138 checked functions. Global-access diagnostics decrease
from 36 to 35 across 1,059 checked functions. No gameplay validation is claimed.

All section sizes and placements remain unchanged: code is 400,663 bytes
versus 401,111, data virtual size is 1,329,724 versus 1,331,596, relocation
data is 36,294 versus 36,306, and the file is 518,144 versus 518,656.
The entry point remains `0x0044E5B0` versus `0x0044E770`. The 26 missing
routines still account for 416 bytes.

Strict positional matches rise by 164 to 57,558 of 518,656 bytes (11.0975%).
There are 461,098 differing positions in 22,925 runs; literal executable
equality still fails. The executable SHA-256 remains unchanged between the
full report and final verification:
`1f9908d45f246339a7d4491197bceac3ed9492893675154a1ac087672fee67cf`.

## Sound declaration and local-scope checkpoint

The fourteen-function `ix/sound.cpp` unit now has twelve normalized exact
matches, up from nine. It sees only its five required system globals, with
the two voice counts preceding the three sound-list heads. This removes
42 unused external declarations from that owner's scope without changing
other owners, storage, types, compiler flags, or link order. The complete
109-byte free-list unlink and 838-byte sound-start functions become exact.
Explicit returns after the two release warnings recover the original
epilogue jumps and make the 177-byte release function exact as well.

The priority routine's selected sounds and released voices are declared
inside their branches. Its original local slots are now restored: priority
at EBP-4, waiting sound at EBP-8, released voice at EBP-12, playing sound at
EBP-16, reassigned voice at EBP-20, and the implicit object at EBP-24.
The frame remains 24 bytes. One comparison still loads the playing-list
head before the local sound, whereas the original loads the local and
compares against the global. This leaves 810 bytes against 811 and changes
the affected branch displacements. Binary-comp reports fifteen differing
encodings on its unequal-size comparison basis; the previous same-size
result had forty positional differences. Those counts are not directly
interchangeable. The final `ix_sound_is_playing` still has an exact
19-instruction body; its reported span changes from 52 to 57 against the
original 56 because of object-end alignment. The unit's overall extent is
unchanged. Rejected scope and source-form probes are recorded in
`docs/PATTERNS.md`.

Only these five rows change in the complete report; the other 1,519 are
unchanged. There are now 1,267 normalized exact functions, 241 same-size
differences, and sixteen reported span differences (74 absolute bytes,
net -6). There are still 1,375 mnemonic-perfect functions, 1,516 at or above
90%, eight below, and 99.58% average mnemonic similarity.

A clean rebuild removes and regenerates all 78 native objects, archives,
and executable outputs before measurement. Both builds, source-order and
address audits, the order report, and all required verification gates pass.
Both value checks cover 1,524 functions with zero mismatches or skips. All
1,104 global-data definitions, 39 code pointers, and 79 mixer operands pass.
A separate binary-comp reference check verifies 379 operands referring to
26 named audio globals across 58 functions. It checks identity, order, and
multiplicity throughout, and function-relative offsets in normalized-exact
routines, including the newly exact unlink and start functions. Call and
global-access analyses retain eleven and 35 diagnostics across 1,138 and
1,059 checked functions respectively. No gameplay validation is claimed.

All section sizes and placements remain unchanged: code is 400,663 bytes
versus 401,111, data virtual size is 1,329,724 versus 1,331,596, relocation
data is 36,294 versus 36,306, and the file is 518,144 versus 518,656.
The entry point remains `0x0044E5B0` versus `0x0044E770`. The 26 missing
routines still account for 416 bytes.

Strict positional matches rise by 38 to 57,596 of 518,656 bytes (11.1049%).
There are 461,060 differing positions in 22,972 runs; literal executable
equality still fails. The executable SHA-256 remains unchanged between
the full report and final verification:
`49fc6c3748d6f08ab47ea043f7bce1fbcedef927108c67a6a11c874d8f67ca4c`.

## Voice and sound-system local-layout checkpoint

The existing `ix/dspv.cpp` and `ix/system.cpp` boundaries remain unchanged.
Five routines improve through evidence-backed local allocation and declaration
scope changes. Their types, calling conventions, storage definitions, compiler
flags, and link order are unchanged. Binary-comp's detailed comparisons cover
all 33 functions in the two owners and find only these five changes:

| Function | Original / rebuilt span | Differing encodings before / after |
| --- | --- | --- |
| `ix_dspv_set_pan` | 293 / 292 | 33 / 9 |
| `ix_dspv_recalc_mix` | 181 / 179 | 15 / 0 |
| `ix_system_delete_all_sounds` | 169 / 169 | 19 / 0 |
| `ix_system_assign_voice` | 551 / 551 | 33 / 4 |
| `ix_system_find_free_voice` | 89 / 89 | 20 / 0 |

The pan routine's nested scopes place position, voice pointer, delta, and step
at EBP-4, -8, -12, and -16. Its one-byte bounds-comparison difference remains.
The mix routine's descriptive local names place scaled volume, pan position,
and voice pointer at -4, -8, and -12. Its complete 178-byte body now matches;
the reported two-byte deficit is final object alignment. The original has
three trailing INT3 bytes, and the rebuild has one. These are matching source
choices, not recovered original identifiers or block structure.

Each sound-list successor is now declared inside its traversal loop. The
deletion function retains its sixteen-byte frame and restores all four original
slots. Voice assignment retains the sample in the outer scope, elapsed time
and position in the positioning block, and loop length in the looping branch.
Its original four slots are likewise restored. The system owner omits the 29
unused streamer globals, recovering the original free-voice comparison and
82-byte body with seven alignment bytes. Assignment still exchanges the
sample-rate numerator and denominator comparison operands in both branches.
Rejected local-name and declaration-scope alternatives are in `docs/PATTERNS.md`.

The changes remove 107 differing instruction encodings. Their span sizes are
unchanged, so the before/after counts use the same comparison basis. The full
report changes three rows; the two voice improvements remain hidden behind
their unchanged size classifications. It now has 1,269 normalized exact
functions, 239 same-size differences, and sixteen reported span differences
(74 absolute bytes, net -6). There are 1,376 mnemonic-perfect functions,
1,516 at or above 90%, eight below, and 99.58% average mnemonic similarity.
These normalized comparisons do not establish literal executable equality.

A clean rebuild regenerates all 78 native objects, archives, and executable
outputs. Both builds, source-order and address audits, the order report, and
all required verification gates pass. Both value checks cover 1,524 functions
with zero mismatches or skips. All 1,104 global-data definitions, 39 code
pointers, 79 mixer operands, and recovered unit-data blocks pass. A separate
binary-comp reference check covers 384 operands referring to 27 named audio
globals across 59 functions, including master volume. It verifies target
identity, order, and multiplicity, plus function-relative positions in exact
routines and the now-exact mix body. Call and global-access analyses retain
eleven and 35 diagnostics across 1,138 and 1,059 functions respectively.
No gameplay validation is claimed.

Section sizes and placements remain unchanged: code is 400,663 bytes versus
401,111, data virtual size is 1,329,724 versus 1,331,596, relocation data is
36,294 versus 36,306, and the file is 518,144 versus 518,656. The entry point
remains `0x0044E5B0` versus `0x0044E770`; the 26 missing routines still account
for 416 code bytes. Strict positional matches are 57,595 of 518,656 bytes
(11.1047%), one fewer than the preceding checkpoint. There are 461,061
differing positions in 22,971 runs; literal executable equality still fails.
The executable SHA-256 is unchanged between the clean-build report and final
verification:
`0dd72c35d172451ac377ec81e3e3bdfcba48513fc9e2356fb15bb97256b75dbf`.

## Additional core declaration scopes

Superseded: the profiler, raster and streaming variables are now statics in
`main.c` and `gr.c` (see "Static `.bss`: `winsys.c` through `gr.c`").

The existing compilation-unit boundaries remain unchanged. A bounded check
of all seven combinations of the twelve flight-profiler declarations, three
raster declarations, and two streaming declarations identifies the following
additional exclusions. Each group is unused by its selected owner. Original
definitions, declaration order within retained groups, types, function bodies,
compiler flags, and link order are unchanged.

| Owner | Newly omitted groups | Fewer differing encodings | Net exact functions |
| --- | --- | ---: | ---: |
| `barracks.c` | profiler, raster | 193 | +1 |
| `brains.c` | streaming | 40 | +2 |
| `cutscene.c` | profiler | 106 | 0 |
| `damage.c` | profiler, raster, streaming | 1 | 0 |
| `fade.c` | profiler, raster | 117 | 0 |
| `gameflow.c` | profiler, raster, streaming | 17 | +1 |
| `killbrd.c` | profiler | 2 | 0 |
| `launch.c` | streaming | 3 | 0 |
| `logic.c` | profiler, raster | 88 | +1 |
| `main.c` | raster | 50 | +1 |
| `music.c` | raster, streaming | 1 | 0 |
| `nearheap.c` | streaming | 1 | 0 |
| `select.c` | streaming | 3 | 0 |
| `trainsim.c` | raster | 72 | 0 |

Existing owner markers suffice for main and training; the other twelve units
gain their own marker before `wc1.h`. The baseline is restored and rebuilt
after the probes, reproducing its executable hash before combining the retained
choices. The combined full report reproduces every selected unit result.
These are measured matching scopes, not recovered original header contents.

Ten functions become normalized exact: `SaveGame`, `check_destroy_target`,
`approach_and_engage`, `funeral_player`, `show_damage_disp`,
`StartNewCampaign`, `report_kilrathi_rout`, `find_ship_index`,
`build_squad_list`, and `house_keep`. Four lose exact matches: `landing`
has two differing encodings, `UpdateDamageDisplay` six, `get_rear_spot`
eight, and `scan_for_enemy` four.

The largest retained reductions are `BarracksScreen` (219 to 30),
`StepPaletteTransition` (123 to six), `PlayScrambleHangarScene` (121 to 25),
`UpdateSceneAnimationObject` (157 to 81), and `SelectTrainSimMission`
(168 to 93). The training selector rises from 89.44% to 97.03% mnemonic
similarity. `evaluate_damage` also crosses 90%, from 88.89% to 95.24%, but
its differing-encoding count increases from eleven to 24; the whole logic
unit still improves by 88. Three further regressions are retained:
`GetNearHeapBlockSize` rises from two differences to four,
`ShowTrainSimHighScores` from 109 to 112, and `RunTrainSim` from twelve
to thirteen. No reported function span changes.

The complete report changes 34 rows and leaves 1,490 unchanged. It removes
794 differing encodings and gains six normalized exact functions overall:
1,275 exact, 233 same-size differences, and sixteen reported span differences
(74 absolute bytes, net -6). There are 1,380 mnemonic-perfect functions,
1,518 at or above 90%, six below, and 99.60% average mnemonic similarity.

A clean rebuild regenerates all 78 native objects, archives, and executable
outputs. Both builds, source-order and address audits, the order report, and
all required verification gates pass. Both value checks cover all 1,524
functions with zero mismatches or skips. All 1,104 linked globals, 39 code
pointers, 79 mixer operands, and recovered unit-data blocks retain their
passing results. A separate binary-comp reference check verifies the target
identities, order, multiplicity, and function-relative positions of 130 named
global operands in the ten newly exact routines. Call and global-access
analyses retain eleven and 35 diagnostics across 1,138 and 1,059 checked
functions respectively. No gameplay validation is claimed.

All section sizes and placements remain unchanged. Code is still 400,663
bytes versus 401,111, data virtual size is 1,329,724 versus 1,331,596,
relocation data is 36,294 versus 36,306, and the file is 518,144 versus
518,656. The entry point remains `0x0044E5B0` versus `0x0044E770`, and the
26 missing routines still account for 416 code bytes. Strict positional
matches fall by sixteen to 57,579 of 518,656 bytes (11.1016%). There are
461,077 differing positions in 22,966 runs; literal executable equality
still fails despite the function-local improvements. The executable hash
is unchanged between the clean-build report and final verification:
`c8f44dc32489d1681721fadefa3d9ca8a4083048b38334879964bc52df847ebe`.

## DIB and MONODEBG declaration scope checkpoint

Superseded: these variables are now statics in `dib.c` and `mono.c` (see
"Static `.bss`: `winsys.c` through `gr.c`").

Two more declaration groups have evidence-backed single-unit use. All
original direct references to the nine DIB buffer globals from
`szFrameRateText` at `0x00476620` through `nDIBSlamCount` at `0x00486060`
occur inside `0x00431F00`–`0x0043390F`. References to mono's four file-error
buffers and two MONODEBG variables, at `0x00475D20`–`0x00475E74`, occur
inside `0x00403500`–`0x00403E4F`. Their reconstructed use sites agree.
This establishes declaration use; it does not recover the original header
organization or justify moving their temporary storage.

Checking each group and their combination across eligible core units gives
four retained scope changes:

| Unit | Unused declarations omitted | Fewer differing encodings | Added exact functions |
| --- | --- | ---: | ---: |
| `cmpgn.c` | DIB buffer state | 1 | 0 |
| `dib.c` | File-error and MONODEBG state | 40 | 0 |
| `eject.c` | Both groups | 11 | 0 |
| `screens.c` | DIB buffer state | 13 | 2 |

`EstablishingShot` and `SceneDirector` now have exact normalized spans of
352 and 1,136 bytes, including alignment and the director's switch data.
`DIBupdate` retains its 384-byte span while dropping from 45 differing
instruction encodings to five. `ejection_sequence` drops from 85 to 74,
and `LoadFace` from 58 to 57. `LoadMissionData` retains 209 differing
encodings while mnemonic similarity rises from 95.73% to 96.58%.

The six changed report rows remove 65 differing encodings; the other 1,518
rows are unchanged. No previously exact routine regresses, and all reported
spans are unchanged. The report has 1,277 normalized exact functions, 231
same-size differences, and sixteen size differences (74 absolute bytes,
net -6). Mnemonic totals remain 1,380 perfect, 1,518 at or above 90%, six
below, and 99.60% average. There are no missing exports, fallbacks, or errors.
Function bodies, types, storage, calling conventions, compiler settings, and
link order are unchanged.

The unused-mono declaration probe in `geom.c` improves four routines to
exact but disturbs three existing exact routines, for only three fewer
differing encodings overall. It is not retained. The other group placements
are also reverted. `docs/PATTERNS.md` records the unsuccessful small-source
probes so that these bounded searches need not be repeated.

A clean rebuild regenerates all 78 native objects, archives, and executable
outputs. Native and modern builds, source-order and address audits, the order
report, and all required verification gates pass. Both value checks cover
1,524 functions with zero mismatches or skips. All 1,104 linked globals,
39 code pointers, 79 mixer operands, and recovered unit-data blocks pass.
A separate binary-comp reference check verifies the identities, order,
multiplicity, and function-relative positions of 46 named global operands
in the two newly exact routines. Call and global-access analyses retain
eleven and 35 diagnostics across 1,138 and 1,059 checked functions. Modern
linking retains the existing unaligned-pointer warnings. No gameplay
validation is claimed.

Section sizes and placements remain unchanged: code is 400,663 bytes versus
401,111, data virtual size is 1,329,724 versus 1,331,596, relocation data is
36,294 versus 36,306, and file size is 518,144 versus 518,656. The entry point
remains `0x0044E5B0` versus `0x0044E770`. The 26 missing routines still
account for 416 of the 448 absent code bytes. Strict positional matches
increase by six to 57,585 of 518,656 bytes (11.1027%); 461,071 positions
differ in 22,969 runs. Literal executable equality remains incomplete.
The executable hash stays unchanged through final verification:
`e71a0b9cf7d337af801a6381d3a1452f7b0680e314aa0ae01b78a90ac0fdba4e`.

## Caller word-width checkpoint

Several routines matched every mnemonic while using wrong argument and
return widths. The original callers, together with unchanged callee output,
support four corrections:

- Barracks sees a word copy-count parameter for `DosMemcpy`. Its label
  routine increments the allocation length in ECX but the copy length in
  CX. The native shim still forwards a dword stack slot to `memmove`; its
  definition and other owners' declarations stay unchanged. A caller-only
  declaration preserves this original inconsistency. Explicit casts under
  the size_t prototype add extensions absent from the original.
- The two bunk-label functions take short series and mission arguments.
  `GetBunkInfo` sign-extends the saved campaign bytes into word registers.
  Its second label call also passes the description buffer, as does its
  first. The address calculation precedes a 24-byte argument cleanup;
  treating it as a post-cleanup address caused the former pilot-name offset.
- The native cutscene owner uses the existing short-scene prototype for
  `init_3Space_objects`. Default argument promotion under the previous
  unspecified-argument declaration widened three demo-call arguments.
- `try2rout` has a short return type and `send_message` takes a short
  message number. Their shared declarations and definitions agree. These
  types restore the communication request's word result test and word
  subtraction, while both callee comparisons remain unchanged.

All calling conventions and stack argument sizes remain unchanged. The
barracks memory-copy declaration is native-only; the modern build retains
the shim's size_t interface. No wrappers, aliases, hand-written glue, flags,
storage, or compilation-unit boundaries are added or changed.

The wave initializer also uses a direct conditional expression selecting
two or minus one. This restores its original dword mask and decrement before
the word store; the former explicit mask expression emitted word arithmetic.

| Routine | Previous differing encodings | Current result | Aligned span |
| --- | ---: | --- | ---: |
| `SetAwakenBarracksMenuLabel` | 10 | Exact | 112 |
| `GetBunkInfo` | 44 | Exact | 176 |
| `RunAnimationDemoLoop` | 144 | Exact | 640 |
| `request` | 193 | Exact | 832 |
| `set_up_action_sphere` | 85 | 73 differing encodings | 352 |

The five changed report rows remove 403 differing encodings. The other
1,519 rows are unchanged, including `SetBunkMenuLabel`, `send_message`,
and `try2rout`. No existing exact match regresses and no reported span size
changes. The report has 1,281 normalized exact functions, 227 same-size
differences, and sixteen size differences (74 absolute bytes, net -6).
Mnemonic totals remain 1,380 perfect, 1,518 at or above 90%, six below, and
99.60% average. There are no missing exports, fallbacks, or errors.

The clean rebuild regenerates all 78 native objects, archives, and executable
outputs. Native and modern builds, source-order and address audits, the order
report, and all required verification gates pass. Both value checks cover
1,524 functions without mismatches or skips. All 1,104 linked globals,
39 code pointers, 79 mixer operands, and recovered unit-data blocks pass.
The four new exact routines also pass a separate binary-comp check of the
identities, ordering, multiplicity, and relative positions of their thirty
named global operands. Call and global-access analyses retain eleven and
35 diagnostics across 1,138 and 1,059 checked functions. The modern linker's
existing unaligned-pointer warnings remain. No gameplay validation is claimed.

All section sizes and placements are unchanged. Code remains 400,663 bytes
versus 401,111, data virtual size 1,329,724 versus 1,331,596, relocations
36,294 versus 36,306, and file size 518,144 versus 518,656. The entry point
is still `0x0044E5B0` versus `0x0044E770`; 26 missing routines account for
416 of the 448 absent code bytes. Strict positional matches fall by nineteen
to 57,566 of 518,656 bytes (11.0991%), with 461,090 differing positions in
22,962 runs. Literal executable equality still fails despite the corrected
function-relative encodings. The clean-build and post-verification hashes
agree:
`c003b4cbffa028dfeee8c78b2e00f0539f7b8a11eeb679cf83a211280289fc19`.

## Scene local-value checkpoint

The original register uses expose two more source-level distinctions.
`PlaySfxWaveFileByNumber` compares the long vector magnitude in EAX after
copying it to the integer playback distance in EDI. Retaining both values
recovers its 135-byte body and complete 144-byte span. `AnimateBarracks`
saves the bunk animation frame before selecting its sprite, and keeps the
two final eye-sprite draw paths separate. This restores its repeated frame
constant and branch-based literal pushes, matching its 553-byte body and
complete 560-byte span. Both routines retain their original stack frames.

The ending owner also needs the short-scene declaration already recovered
for cutscene callers. Its `ShowTigerClawEscapeScene` extends the signed
campaign byte into AX. This owner's prototype leaves the parameter unnamed;
naming it changes the preceding `ReleasePacketHandle` from exact to seven
differing encodings. The unnamed declaration preserves the allocator and
reduces the escape scene from 203 differences to 159 at its unchanged
928-byte span. Its body still has 36 differing encodings and is two bytes
shorter than the original.

`ShutdownGameWindow` saves the process handle before setting its priority.
The original calls `GetCurrentProcess` before pushing the priority constant,
whereas the nested call expression pushes it first. The recovered ordering
restores every mnemonic, leaving six cursor-call register differences in
its unchanged 112-byte span.

| Routine | Previous differing encodings | Current result | Aligned span |
| --- | ---: | --- | ---: |
| `AnimateBarracks` | 150 | Exact | 560 |
| `PlaySfxWaveFileByNumber` | 25 | Exact | 144 |
| `ShowTigerClawEscapeScene` | 203 | 159 differing encodings | 928 |
| `ShutdownGameWindow` | 8 | 6 differing encodings | 112 |

The four changed report rows remove 221 differing encodings; all other
1,520 rows are unchanged. No existing exact match regresses and no reported
span size changes. The report has 1,283 normalized exact functions, 225
same-size differences, and sixteen size differences (74 absolute bytes,
net -6). Mnemonic totals are 1,382 perfect, 1,518 at or above 90%, six below,
and 99.61% average. There are no missing exports, fallbacks, or errors.
Calling conventions, storage, compiler settings, and unit boundaries are
unchanged. The unused local-value probes are recorded in `docs/PATTERNS.md`.

The clean rebuild regenerates all 78 native objects, archives, and executable
outputs. Native and modern builds, source-order and address audits, the order
report, and all required verification gates pass. Both value checks cover
1,524 functions with zero mismatches or skips. All 1,104 linked globals,
39 code pointers, 79 mixer operands, and recovered unit-data blocks pass.
A separate binary-comp reference check verifies all seven named global
operands in the two newly exact routines, including their identities,
order, multiplicity, and function-relative positions. Call and global-access
analyses retain eleven and 35 diagnostics across 1,138 and 1,059 checked
functions. The modern linker's existing unaligned-pointer warnings remain.
No gameplay validation is claimed.

Section sizes and placements are unchanged: code is 400,663 bytes versus
401,111, data virtual size is 1,329,724 versus 1,331,596, relocation data is
36,294 versus 36,306, and file size is 518,144 versus 518,656. The entry point
remains `0x0044E5B0` versus `0x0044E770`; 26 missing routines account for
416 of the 448 absent code bytes. Strict positional matches increase by
26 to 57,592 of 518,656 bytes (11.1041%), with 461,064 differing positions
in 22,973 runs. Literal executable equality remains incomplete. The
clean-build and post-verification SHA-256 hashes agree:
`6128b5232f8e0526c7819f8c27573e8c4ec3f518680502827d8c2353db764445`.

## Input-state declaration checkpoint

The keyboard polling variables are now `winsys.c` statics (see
"Static `.bss`: `winsys.c` through `gr.c`"). The event-list and calibration
exclusions below remain for `mono.c` and `view.c`. Flight's exclusions and
its source marker have since been removed with exact matching; see the
three-marker cleanup in `docs/PATTERNS.md`.

Three groups have only one observed native user unit each: the three
input-event list globals are referenced within eventmgr, the four joystick
calibration bounds within joycal, and fourteen keyboard polling variables
within winsys. The original disassembly confirms these reference ranges;
source uses agree. Their existing storage and link order are unchanged.

All seven combinations of omitted declarations were checked across eligible
core owners. Keep event-list, calibration, and keyboard exclusions in `fl.c`;
event-list and keyboard exclusions in `mono.c`; and event-list and
calibration exclusions in `view.c`. These conditional declarations reproduce
measured compiler context, not recovered original header text. Applying the
same exclusions indiscriminately introduces size differences and loses
existing exact matches, so the other placements are discarded.

In the selected flight context, `Mtarget_missile` also needs a separate
promoted object index for its owner and class table reads. Declaring that
integer before its short counter at function scope restores the original
EAX/CX allocation and preserves its exact 176-byte span. A loop-scoped index
leaves six register differences and is not retained. This body change also
changes `perform_maneuver`'s instruction selection; the final retained output
has eighteen differing encodings versus 34 previously, while mnemonic
similarity falls from 95.19% to 94.23%.

| Routine | Previous differing encodings | Current result | Aligned span |
| --- | ---: | --- | ---: |
| `Mfull_ahead` | 5 | Exact | 64 |
| `Mgloat` | 11 | Exact | 176 |
| `advance_canned_sequence` | 23 | Exact | 240 |
| `CalcRectangleArea` | 4 | Exact | 64 |
| `perform_maneuver` | 34 | 18 differing encodings | 320 |
| `print_subtitle` | 37 | 18 differing encodings | 352 |

`OpenPacketSection` also drops from eighteen differing encodings to sixteen
and reaches 100% mnemonic similarity. Two small regressions remain:
`InitializeConstellationField` and `DrawConstellationField` each rise from
two differing encodings to four. All reported spans are unchanged and no
previously exact function regresses. The nine changed report rows remove
76 differing encodings overall; the other 1,515 rows are unchanged.
The report has 1,287 normalized exact functions, 221 same-size differences,
and sixteen size differences (74 absolute bytes, net -6). Mnemonic totals
are 1,385 perfect, 1,518 at or above 90%, six below, and 99.61% average.
There are no missing exports, fallbacks, or errors. Calling conventions,
global types and storage, compiler settings, and unit boundaries are unchanged.

The geometry declaration alternative gains four exact routines but loses
three for only seven fewer encodings; the music alternative trades one exact
routine for another for only one fewer encoding. Neither is retained.
The unsuccessful body probes and their results are recorded in
`docs/PATTERNS.md` to avoid repeating them.

The clean rebuild regenerates all 78 native objects, archives, and executable
outputs. Native and modern builds, source-order and address audits, the order
report, and all required verification gates pass. Both value checks cover
1,524 functions with zero mismatches or skips. All 1,104 linked globals,
39 code pointers, 79 mixer operands, and recovered unit-data blocks pass.
A separate binary-comp reference check verifies the identities, ordering,
multiplicity, and relative positions of 26 named global operands across
the four new exact routines and `Mtarget_missile`. Call and global-access
analyses retain eleven and 35 diagnostics across 1,138 and 1,059 checked
functions. The modern linker's existing unaligned-pointer warnings remain.
No gameplay validation is claimed.

All section sizes and placements are unchanged. Code remains 400,663 bytes
versus 401,111, data virtual size 1,329,724 versus 1,331,596, relocation data
36,294 versus 36,306, and file size 518,144 versus 518,656. The entry point
is still `0x0044E5B0` versus `0x0044E770`; 26 missing routines account for
416 of the 448 absent code bytes. The strict positional comparison remains
57,592 matching positions out of 518,656 (11.1041%), with 461,064 differing
positions in 22,973 runs. Literal executable equality remains incomplete.
The clean-build and post-verification SHA-256 hashes agree:
`51d0edfc94877a4a38a3d6516499031eccec1740b2532df6352d815b4d13bf94`.

## Cockpit access-order checkpoint

`fire_computer_graphic_missile` remains in the established cockpit range.
Qualified accesses to its two coordinate words recover the original memory
updates and three-register save sequence. Its aligned span remains 336 bytes,
mnemonic similarity rises from 85.87% to 96.59%, and differing encodings fall
from 82 to 58. The body comparison improves from sixty differences to ten;
the remaining duplicated shape-pointer load makes the body 335 bytes versus
329 originally. Qualifying only Y instead enlarges the span to 352 bytes and
is not retained.

The same unit's `objective_lost` preserves the signed-byte mission-state read
before the objective-type read through qualified accesses. Its body remains
78 bytes within the original 80-byte span. All mnemonics now match, up from
84.62%; thirteen register encodings still differ, down from fifteen. These
qualifications reproduce observed accesses without claiming the original
source declared volatile globals. No declarations, global storage, compiler
settings, calling conventions, or unit boundaries change.

The full report changes only these two rows, removing 26 differing encodings
with all other 1,522 rows unchanged. Normalized exact matches remain 1,287,
with 221 same-size differences and sixteen size differences (74 absolute
bytes, net -6). There are now 1,386 functions at 100% mnemonic similarity,
1,520 at or above 90%, four below, and 99.63% average. There are no missing
exports, fallbacks, or errors. A separate binary-comp reference check verifies
32 named global operands in the two changed routines, excluding the known
duplicated shape-pointer load; the objective operands also retain their
original relative positions.

The layout audit initially reports a false adjacent-global dependency for
the qualified Y compound assignments. Its text scanner treats += and -= as
pointer offsets. Original instructions at `0x00414E18` and `0x00414E21`, and
rebuilt instructions at `0x00414E0E` and `0x00414E17`, use two-byte memory
operands at Y itself. Neither reaches the following type object. The existing
review configuration records this specific false positive; original and
rebuilt global-span checks remain enabled.

A read-only relocation scan also found no HIGHLOW targets inside the
unexplained keyboard gap `0x00475B70`–`0x00475BEF`, mono gap
`0x00475E60`–`0x00475E6F`, or console-to-profiler gap
`0x004763F4`–`0x004764F7`. This supplies no evidence for new storage objects,
so their ownership and content remain unresolved. Resolved later: these gaps
hold unreferenced statics (see "Static `.bss`: `winsys.c` through `gr.c`").

The clean rebuild regenerates all 78 native objects, archives, and executable
outputs. Native and modern builds, source-order and address audits, the order
report, and all required verification gates pass. Both value checks cover
1,524 functions with zero mismatches or skips. All 1,104 linked globals,
39 code pointers, 79 mixer operands, and recovered unit-data blocks pass.
Call and global-access analyses retain eleven and 35 diagnostics across
1,138 and 1,059 checked functions. The modern linker's existing
unaligned-pointer warnings remain. No gameplay validation is claimed.

Code remains 400,663 bytes versus 401,111, data virtual size 1,329,724 versus
1,331,596, and file size 518,144 versus 518,656. Relocation data increases
by four bytes to 36,298, versus the original 36,306. Section placements and
the entry point remain unchanged; the latter is `0x0044E5B0` versus
`0x0044E770`. The 26 missing routines still account for 416 of the 448
absent code bytes. Strict positional matches increase by 88 to 57,680 of
518,656 bytes (11.1211%), with 460,976 differing positions in 23,061 runs.
Literal executable equality remains incomplete. Clean-build and
post-verification SHA-256 hashes agree:
`ef3713080970987a3284340e6b4a0adf67d6f6ae2e3ca882fa14932b7e1f28a8`.

## Button and screen declaration checkpoint

The DIB and near-heap button-history exclusions recorded below have since
been removed with exact matching. The fade, logic, and main exclusions
remain. See [WORKAROUNDS.md](WORKAROUNDS.md) for the current worklist.

Six button-history globals have eighteen original relocated references,
all in `input.c`. Four raw-coordinate/dead-zone globals have eight references,
all in `joycal.c`. Six packet-handle and screen-allocation globals have
23 references, all in `screen.c`. Source uses agree with these owners. The
second-button double-click result has an additional main-module user and
is deliberately outside the private button group.

All seven combinations of omitted declarations were tested across eligible
core units. Retain button-history exclusions in `dib.c`, `fade.c`, `logic.c`,
`main.c`, and `nearheap.c`; joystick-state exclusions in `main.c`; and
screen-table exclusions in `fade.c`, `logic.c`, and `main.c`. Existing unit
markers express these choices, so no function bodies change. These are
measured compiler contexts, not recovered original header text. Global
definitions and storage, types, calling conventions, compiler settings, and
compilation-unit ranges remain unchanged. Other placements introduce size
changes or produce worse overall comparisons and are discarded.

| Routine | Previous differing encodings | Current result | Aligned span |
| --- | ---: | --- | ---: |
| `PurgeNearHeapBlocks` | 4 | Exact | 144 |
| `SelectPreviousExternalViewObject` | 10 | Exact | 64 |
| `FreeNearHeapBlock` | 4 | 2 differing encodings | 96 |
| `GetNearHeapBlockSize` | 4 | 2 differing encodings | 128 |
| `DIBupdate` | 5 | 2 differing encodings | 384 |
| `StepPaletteTransition` | 6 | 3 differing encodings | 560 |
| `get_player_input` | 8 | 11 differing encodings | 224 |

The near-heap purge restores its original loop comparisons; the previous-view
selector restores its EAX/DX/ECX/BL allocation. Their complete bodies are
135 and 59 bytes respectively. The other two heap routines now have matching
mnemonics but retain reversed initial base/size loads. The remaining DIB
differences are its destination pitch multiplication operands. The palette
transition reaches 99.35% mnemonic similarity.

The selected logic context makes six more routines exact: `try2rout`,
`get_rear_spot`, `FindEnemyAimingWithinRange`, `in_danger`, `abandoned`, and
`FindSceneAnimationCommand`. `UpdateSceneAnimationObject` drops from 81
differing encodings to nine, while `scan_for_enemy` drops from four to two.
Three previously exact routines regress: `report_kilrathi_rout` has eleven
differences, `find_ship_index` six, and `set_special` two. `attacker_in_range`
rises from eleven to 36 differences and falls from 95.83% to 85.42% mnemonic
similarity; `hostile_sphere` rises from six differences to ten. The logic
tradeoff removes 42 differing encodings overall and adds three exact matches
net, without changing spans.

A separate distance local in the attacker routine reduces its differences
to sixteen, but raises the scene-update routine from nine to 170; folding
the distance assignment into the comparison leaves both unchanged. Neither
body probe is retained. Earlier collision, navigation, and object-initializer
probes are recorded in `docs/PATTERNS.md`; their sources were also restored.
A forced rebuild reproduced the preceding checkpoint's executable hash
before the declaration selection.

The twenty changed report rows remove 63 differing encodings overall; the
other 1,504 rows and every reported span are unchanged. Eight new exact
routines and three regressions raise normalized exact matches by five, to
1,292, with 216 same-size differences and sixteen size differences (74
absolute bytes, net -6).
Mnemonic totals are 1,390 perfect, 1,519 at or above 90%, five below, and
99.63% average. There are no missing exports, fallbacks, or errors. A separate
binary-comp reference check verifies the identities, order, multiplicity,
and relative positions of all 43 named global operands across the eight
new exact routines.

The final clean rebuild regenerates all 78 native objects, archives, and
executable outputs. Native and modern builds, source-order and address
audits, the order report, and all required verification gates pass. Both
value checks cover 1,524 functions with zero mismatches or skips. All 1,104
linked globals, 39 code pointers, 79 mixer operands, and recovered unit-data
blocks pass. Call and global-access analyses retain eleven and 35 diagnostics
across 1,138 and 1,059 checked functions. The modern linker's existing
unaligned-pointer warnings remain. No gameplay validation is claimed.

All section sizes and placements are unchanged. Code remains 400,663 bytes
versus 401,111, data virtual size 1,329,724 versus 1,331,596, relocation data
36,298 versus 36,306, and file size 518,144 versus 518,656. The entry point
is still `0x0044E5B0` versus `0x0044E770`; 26 missing routines account for
416 of the 448 absent code bytes. Strict positional matches remain 57,680
out of 518,656 bytes (11.1211%), with 460,976 differing positions in
23,061 runs. Literal executable equality remains incomplete. Clean-build
and post-verification SHA-256 hashes agree:
`6cf88c069df006c1b0a67956c5a167f29edfdcef8b1c73eab2e87ad157881ec7`.

## Briefing and joystick declaration checkpoint

Seven briefing/medal globals have 48 original relocated references, all
within `screens.c`. Source uses agree. Omit their declarations from
`gameflow.c`, which does not use them, through its existing unit marker.
This restores the original operand order of both screen-pointer comparisons
in `free_viewport`: its 228-byte body and 240-byte aligned span are now
normalized exact, eliminating two differing encodings.

The joystick button press/release wait helpers have ten original direct
references, all within `joystick.c`; their reconstructed uses agree. Omit
their prototypes from `main.c` through its existing marker. The saved-input
comparisons in `get_player_input` recover their original operand order,
reducing eleven differing encodings to eight. Its 222-byte body, 224-byte
span, and 96.61% mnemonic similarity remain unchanged. No other main-module
routine changes.

Three matrices tested all seven combinations of their respective groups
across eligible core units: room/briefing/conversation globals,
mission-map/navigation-label/simulator globals, and mono/campaign/joystick
helper prototypes. Only the two exclusions above are retained. The other
cases offer no suitable improvement; detailed ownership evidence and failed
audio/heap probes are recorded in `docs/PATTERNS.md`. Every discarded probe
was restored, and forced rebuilds reproduced the preceding checkpoint's
executable hash. These guards reproduce measured compiler contexts, not
recovered original header text. Function bodies, storage, types, linkage,
calling conventions, compiler settings, and compilation-unit ranges are
unchanged.

The final clean rebuild regenerates all 78 native objects, archives, and
executable outputs. Only the two expected report rows change, removing five
differing encodings overall. The other 1,522 rows and every reported span
are unchanged, and no previously exact routine regresses. Normalized exact
matches rise from 1,292 to 1,293, with 215 same-size differences and sixteen
size differences (74 absolute bytes, net -6). Mnemonic totals remain 1,390
perfect, 1,519 at or above 90%, five below, and 99.63% average. There are no
missing exports, fallbacks, or errors. A separate binary-comp reference
check verifies the identities, order, multiplicity, and relative positions
of all seventeen named global operands in `free_viewport`.

Native and modern builds, source-order and address audits, the order report,
and all required verification gates pass. Both value checks cover 1,524
functions with zero mismatches or skips. All 1,104 linked globals, 39 code
pointers, 79 mixer operands, and recovered unit-data blocks pass. Call and
global-access analyses retain eleven and 35 diagnostics across 1,138 and
1,059 checked functions. Existing modern-linker unaligned-pointer warnings
remain. No gameplay validation is claimed.

All section sizes and placements remain unchanged: code is 400,663 bytes
versus 401,111, data virtual size 1,329,724 versus 1,331,596, relocation data
36,298 versus 36,306, and file size 518,144 versus 518,656. The entry point
remains `0x0044E5B0` versus `0x0044E770`. Strict positional matches remain
57,680 out of 518,656 bytes (11.1211%), with 460,976 differing positions
in 23,061 runs. Literal executable equality remains incomplete. Clean-build
and post-verification SHA-256 hashes agree:
`4a1fceb32eba2f3b75e06ea14d81fe8512e5c5b98fae8b698dfcc02efa0aae8f`.
