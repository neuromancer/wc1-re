# Original-code exports

`code-full/` is the comparison input. Files are keyed by entry address, for
example `FUN_0041D0C0.disassembled.txt`, regardless of the function name.

For an annotated function, export directly from the retail PE:

```sh
make export-asm
make export-asm EXPORT_ASM_FLAGS=--discover
make export-asm EXPORT_ASM_FLAGS='--clean --discover'
```

This path uses `/* Function start: 0xADDR */` annotations and produces the
disassembly needed by `compare-func`, `report`, and `order`.
Keep `--discover` when overriding the default export flags: discovered CRT
and thunk boundaries also delimit the byte spans of adjacent source functions.

Rapid source probes need a forced rebuild of their generated outputs. With
the local GNU Make 3.81, `make -W src/ix/sound.cpp WC1.EXE` can leave a stale
archive or executable when writes fall within one timestamp interval. Remove
the affected object, its owning archive, and `WC1.EXE` before rebuilding;
do the same when restoring a probe. For the sound owner these are
`out/ix/sound.obj`, `out/ix/sound.lib`, and `WC1.EXE`. A clean rebuild of all
native objects and archives reproduced the preceding checkpoint's SHA-256,
confirming that its retained source and measured executable agreed. Run
reports and verification with `--no-build` after an explicit build, and check
the executable hash before and after validation.

When forcing a compile without removing outputs, keep the object rebuild and
relink in separate commands:

```sh
set -e
make -W src/nav.c out/nav.obj
make -W out/nav.obj WC1.EXE
```

Do not pass both `-W src/nav.c` and `-W out/nav.obj` to the same invocation:
Make treats the object as already updated and can relink its stale contents.
Check that the object and listing were regenerated before judging a probe.
Scripted probes must stop on compilation or link errors before running a
comparison. Otherwise a later successful command can hide the failure and
score the preceding executable instead of the edited source.

Keep `nop` out of `padding_mnemonics` in `config/checkValuesCapstone.json`.
The original high-score routine contains a reachable `NOP` at `0x00426BAD`.
Filtering it creates a gap in the decoded control flow and drops the following
18 instructions, including calls to `GetHighScoreEntry` and `DrawFormattedText`.
Two CRT routines also contain reachable NOPs. The current policy filters only
`int3`; normal return and function-boundary handling excludes trailing padding.
Run `make export-asm` after changing this policy to refresh existing exports.

The seeded Ghidra list and automatic discovery both miss unreferenced routines.
`config/original-boundaries.map.txt` preserves verified entry points that lack
source annotations, using the MSVC map format accepted by `binary-comp`.
It is recovered metadata, not an original linker map. `make export-asm` loads
it even for clean exports. Remove an entry when its source annotation supplies
the boundary, so the export takes the implemented function's name.

The map also retains four compiler-generated DSP array initializer entries.
They deliberately have no source annotations and are checked separately by
`make verify-array-initializers` (included in `make verify`). The function
report now counts 1,550 source entries after removal of their four
handwritten stand-ins and recovery of the two graphics diagnostic functions
`__copyline` and `__wipeline`, the C key-reset forwarder at `0x00436390`,
the DOS interrupt callback at `0x004368D0`, the rectangle routine
`GetViewportIntersection` at `0x00436AC0`, and the 26 unreferenced
event-manager, memory-service and logger stand-ins;
the separate gate covers the 74 generated array-initializer code bytes.

`make verify-code-pointers`, included in `make verify`, checks the targets
of HIGHLOW-relocated function pointers embedded in reconstructed routines.
It uses binary-comp's source/map correspondence and relocated-pointer check,
preserving each routine's reference order and multiplicity. It currently
checks 39 pointers in 26 functions. This supplements instruction comparison,
whose address normalization can accept a pointer to the wrong entry. The
gate found startup's substitution of the neighboring mouse hook for the
DOS callback, and `EMStartUp`'s substitution of joystick polling for
`get_player_input`. Both references now match the original targets. This
gate checks reference identity, not instruction similarity or absolute layout;
function pointers stored in data remain covered by the unit-data checks.
Negative checks on temporary executable copies restore each wrong target
independently; the gate rejects both and names the affected caller. The
recovered DOS callback also passes a separate literal binary-comp comparison
of its complete sixteen-byte span, including compiler padding.

`make verify-mixer-references`, included in `make verify`, checks 79 operands
referring to ten named globals in `ix_mixer_thread_proc` and
`ix_mixer_service`. Original global addresses come from the source annotations,
and rebuilt addresses come from their linker symbols. The gate requires the
original function-relative relocation offsets and checks each pointer with
binary-comp's relocated-pointer comparison using only those named mappings.
It rejects missing or ambiguous metadata. Two negative checks swap the
primary/mixer buffer operands and the buffer-size/write-offset operands in
separate temporary executable copies. Both functions still compare as
normalized exact, but this gate rejects each swap and identifies its routine
and globals. This is a targeted reference check, not proof of absolute layout
or coverage of other globals, string literals, or imports.

`make verify-unit-data`, also included in `make verify`, checks the complete
initialized-data sections recovered for winsys (259 bytes), mono (225),
auto (7), eject (156), cmpgn (135), fl (236), cutscene (1,297),
brains (2), mission (939), objdata (9,773), nav (608), nearheap (12),
gameflow (64), title (1,522), joystick (238), spc (12), version (13),
barracks (632), fade (2),
debug (34), disk (523), ship (286), init (550), logic (447), pilot (29),
trainsim (511), main (1,018), pload (147), sound (274), launch (172), install (325),
damage (390), view (289), ending (457), input (6), comm (520), video (10),
joycal (42), screens (1,298), raster (6,640), killbrd (1,508), and gr (2,030).
It also checks gr's complete
56-byte read-only section, including the registry key and value arrays and
their alignment. The read-only block must retain its COFF storage flags.
It delegates byte matching to binary-comp, allowing one address shift per
block, and verifies object sizes, global positions, and pointer relocations.
String-content fallback is disabled so duplicate or empty literals cannot
conceal incorrect pointer targets. Resource pointers outside their unit must
resolve to the named global at the original field offset. These checks prove
relative layout; final absolute addresses remain a whole-executable task.
The default check also verifies the seven adjacent units from barracks through
logic as one 2,487-byte run, including all 13 inter-object alignment bytes.
It also checks damage and view as a 681-byte run, including their two alignment
bytes, and ending through joycal as a 1,050-byte run, including 15 alignment
bytes. The room and graphics units form another 3,542-byte run, including
four alignment bytes. These checks catch correct individual blocks linked
at incorrect relative positions.
The broader screens, raster, room, and graphics check covers 11,486 bytes
with all four object positions and ten inter-object alignment bytes. Raster's
complete data check also verifies its 22 public symbol offsets and empty
relocation set, in addition to matching the linked bytes.
The same gate checks the uninitialized blocks owned by fl (5 bytes), nav (368),
cockpt (1,024), pilot (4),
sound (8), damage (4), music (200), eventmgr (4,096),
ix/dsp (1,072), ix/dsps (120), ix/dspv (1,120), and ix/system (28).
These must be genuine COFF `.bss` sections with no raw data
or relocations, and their linker symbols must belong to the expected object
at the recovered relative offsets. Binary-comp compares the loader-visible
zero-filled bytes. These checks establish local storage and layout;
they do not claim that the surrounding uninitialized region matches.
The four ix blocks must also form one 2,340-byte run with all 26 named
positions intact. Negative checks exchange two named positions in temporary
map copies for each block; the gate rejects all four exchanges. Library
objects are located beneath `out/ix`, while map ownership uses their object
basenames. This extends the same verification to library members without
changing its byte-comparison policy.
The graphics check covers only its 64,024-byte uninitialized prefix: the
20-byte raster surface, four alignment bytes, and 64,000-byte transformation
buffer. It requires the current 64,024-byte COFF section and both private
symbols at the original offsets. The remaining graphics storage is still
unrecovered, so this is explicitly a partial-section check.
The sound, damage, and music blocks must also form one 212-byte run; the cursor
buffer and graphics prefix must form one 68,120-byte run. Both require the original
unit positions with no inter-object padding. Pointer-free runs use
binary-comp's direct address comparison, while runs containing pointers use
its relocation-aware comparison.
Maneuver state, navigation, cockpit cursor storage, and the console pointer
form another 1,404-byte run with three alignment bytes. The gate checks both
maneuver-state offsets, both navigation array offsets, and all four owners.
`make verify-barracks-data` retains the barracks-only check.

Three partial startup checks cover 56, eight, and 723 bytes. Their owner must
contain exactly 787 initialized bytes; the original 795-byte section has two
unidentified words between those spans. The checks deliberately do not bridge
either missing word. The startup tail, system paths, file diagnostics,
autopilot, ejection, conversation, maneuvers, cutscenes, AI, mission, object
data, navigation, near-heap, campaign flow, title, joystick, and space form
one 16,248-byte run. The combined check includes 40 alignment bytes and
187 pointer relocations. It establishes relative positions despite the
unrecovered startup words and the resulting eight-byte absolute displacement.
Startup, system-path, developer-channel, ejection, and campaign literal
metadata covers 679, 259, 221, 96, and 83 bytes respectively. The startup
check rejects the former nonempty spelling of `WinMain`'s empty argument
literal; the ejection check requires the stranded-message pointer to target
the beginning of its own literal. The six campaign formats retain their
distinct positions after the mouth parser's empty initializer and two
separate integer formats.
The 47 maneuver dispatch entries must target the correct one of 42 named
function entries, each represented by a one-byte external range. This rejects
both a different handler and an interior address in the correct handler.
The cutscene and mission literal metadata covers 289 and 27 bytes; complete
unit checks additionally require their positions and all cutscene pointers.
The object block contains 21 animation arrays, 58 object records, three
coordinate tables, and 56 separate name literals. Its 80 pointers remain
inside the block. The gate rejects a name pointer redirected to another
byte-identical empty literal, as well as a shifted animation pointer.
The 393-byte object-name metadata resolves this pool in the linked-data audit;
the strict unit check also covers its exact position and final alignment.
The title block includes the restored 279-byte Secret Missions message.
The navigation, title, and joystick literal metadata covers 360, 1,062, and
238 bytes, allowing the linked-data audit to resolve individual string pointers. The full
unit checks retain their stricter positional checks for duplicate strings.
The internal near-heap/campaign and viewport-helper boundaries remain
provisional as described in `docs/ORDER.md`, as do the title/joystick division
and the fleet-view helper's exact owner.

The system-path, joystick, pilot, and packet-loader sections contain only
compiler literals, so they have no public data symbols to anchor the
comparison. The unit gate uses the same COFF layout
resolver as the private-symbol map: public code symbols establish the code
base, and linked `DIR32` operands establish the literal section's base. Every
such operand must have a PE `HIGHLOW` relocation and agree on that base.
This also rejects a reference redirected to another byte-identical `"j.cal"`
copy. Strict binary-comp address comparisons then check all 259, 238, 29,
and 147 bytes respectively.

Two partial-section checks cover the cockpit's 16-byte prefix and 921-byte
tail. They verify 32 declarations and both string-pointer relocations and
require the current 937-byte COFF section. Eight unexplained original zero
bytes between the blocks prevent a complete-unit claim. The tail includes
the full 293-byte literal pool, also represented in the linked-data metadata.
The version pointer and its nine-byte literal form a separate complete
13-byte block. The cockpit tail through version and the following units to
the pilot literals must form a 3,461-byte run, including 62 pointer relocations
and 24 alignment bytes. Space state and the cockpit prefix form a separate 28-byte
run. These checks leave the missing cockpit bytes explicitly unresolved.

The scene transition and answer literals occupy all 29 bytes of `pilot.obj`'s
initialized section. The following unidentified word at `0x00469DA0` prevents
joining that run to the simulator. The eight units from simulator through
scripted view form a separate 3,145-byte run, with 20 pointer relocations and
19 alignment bytes. Main's complete section checks 29 declaration offsets, including the
legacy overlay setting corroborated by the surviving DOS source, and all
834 literal bytes. The `sizeof` diagnostic survives compilation even though
the compiler removes its condition and call. Separate volume literals must
retain their original order. The packet loader's 147-byte pool likewise
retains both fatal diagnostics before its buffer-ownership diagnostic.
Audio's complete initialized section includes two shared registry-name arrays,
two separate registry-key literals, both `SNOW` literals, and the restored
newline in its volume diagnostic. Launch's five private tables retain their
original offsets, including compiler alignment before its two 14-word arrays.
Installation's section includes all three distinct failure messages and the
last terminating zero; seven linker alignment bytes follow before damage data.
All five literal pools have linked-data metadata; no comparison-only storage is
compiled into the game.

`bin/collectGlobalDefinitions.py` accepts data-only owners such as
`objdata.c` and `version.c`: without an annotated function boundary,
declarations extend from the owner include to the end of the file. Their
`.data` sizes and pointers are verified by the same strict unit gate as the
other owners.

The collector also retains addressed file-scope definitions between functions,
including the private autopilot formation count immediately before its first
user. It uses binary-comp's parser to exclude function bodies while preserving
initializers and address comments. The declaration inventory is unchanged;
`python3 bin/testCollectGlobalDefinitions.py` checks interleaved storage, local
exclusion, unused statics, and data-only owners.

Three explicitly marked partial-section checks cover `game.c`: two scalar
state runs of 112 and 21 bytes, and the 6,652-byte run of scripts, mission
records, maneuver tables, and display literals. Each permits only one address
shift and checks its declarations and relocation set. They also require the
current containing COFF section size of 6,788 bytes. The unresolved four bytes
between the two state runs prevents a complete-unit layout claim; these
checks do not silently treat that gap as matched.

The complete 1,298-byte `screens.c` check replaces its former two partial
checks. It verifies all 27 declarations and 14 pointer relocations, including
the character table's two zero-initialized tail records and the compiler's
following four alignment bytes. The full 50-byte debriefing movement table
and all 262 bytes of literal storage retain their original relative positions.
The ten-slot table capacity is a reconstruction supported by the original
activity-array capacity and data layout; see `docs/ORDER.md` for its limits.

`config/data-literals.h` contains comparison metadata for anonymous literal
spans. It is never compiled or included by game code. The data-check targets
append it to `out/globals-data-audit.c`; source-level global audits continue
to use the separate `out/globals-audit.c`. Binary-comp's unique-byte-sequence
lookup locates the complete disk literal pool and verifies pointers into it,
including three-character graphics-mode names and the empty string. Its
ordinary string-content fallback cannot resolve those short strings.
The damage literal pool similarly establishes the two-character `"Ok"`
severity string's position; the separate unit-data check enforces all offsets.
Four separately checked spans cover 408 bytes of used music literals and
the following 80 bytes of screen diagnostics. They preserve original newlines
and trailing spaces; the three unused literals between these runs remain
unrecovered. The 296-byte sound-descriptor table is checked directly through
the source declaration. These partial checks do not
establish the complete music section or its absolute placement.

The DirectDraw metadata span covers all 3,620 bytes from its first frame-rate
format through its last error-name terminator. This checks the relative order
of duplicate diagnostics as well as their text and alignment. Its eleven
initialized variables are checked individually; two unresolved zero words
still prevent a complete-unit layout claim for `dib.c`.

LINK 4.20 omits private C data symbols from its public map. The build preserves
that map as `out/WC1.link.map`, then `bin/appendStaticSymbols.py` copies it to
`WC1.map` with two private winmain symbols, one private auto symbol, four
private eject symbols, one private cmpgn symbol, four private fl symbols,
nineteen private cutscene symbols, three private mission symbols,
twenty-one private objdata symbols,
eight private nav symbols, two private nearheap symbols,
three private gameflow symbols, six private title symbols, one private spc
symbol, twenty-three private cockpt symbols, eleven private main symbols,
four private damage symbols,
seven private view symbols,
six private music symbols, eight private ending symbols, two private
comm symbols, two private joycal symbols, ten private dib symbols,
eighteen private game symbols, one private eventmgr symbol,
seventeen private screens symbols,
twenty-two private killbrd symbols, and five private gr symbols added.
Their addresses come from the object's COFF symbols, public section anchors,
and linked `DIR32`
references with PE `HIGHLOW` relocations. Conflicting anchors or unresolved
symbols fail the build. This step changes only comparison metadata; it does
not modify the executable or infer addresses from the original binary.
The shared resolver handles both named storage and compiler literals. COFF
addends are subtracted modulo 32 bits: indexed references can encode negative
offsets as unsigned DWORDs, as the communication unit's ace-name lookup does.

The room literal metadata covers all 328 bytes from the one-space label
through the final chalk-board format. It resolves short and empty strings
by their positions in that complete pool. The kill-board unit gate checks
these same bytes together with the initial campaign/pilot records, animation
sequences, menu state, all intervening alignment, and all 24 pointer targets.
The source initializer audit serializes `ShortPoint` as two signed halfwords;
its size alone is insufficient to distinguish the coordinate fields.

The graphics literal metadata covers 334 bytes after the palette translation
and absolute sine/cosine tables. It includes the four-character shape version
and the streamer's separate empty string. The unit gate checks this entire
pool and all three tables together as one 2,030-byte initialized section.

The shared-renderer metadata checks the 1,572-byte run of direction-frame,
cockpit-light, cockpit-layout, and pilot-hand tables through the ten
unidentified `-400` words. Their owner remains unresolved, so storage stays
in `globals.c`; this check proves the run's bytes and internal alignment,
not an original compilation-unit boundary. The preceding twelve- and
sixteen-byte gaps remain outside the verified run.

`verify-unit-data` also checks the three angular constants in the temporary
globals object's 12-byte read-only section. It verifies their original
values, offsets, and storage flags. It does not establish their original
owner or their placement relative to other read-only objects.

Keep address annotations outside initializers so the linked-data inventory
can see them. For the 7,830-byte object-type table, the annotation precedes
the declaration: a semicolon in an interior comment ends the small inventory
parser's initializer scan early. The display-name pool and all 21 animation
arrays also have external annotations. Binary-comp locates the complete
1,064-byte animation run uniquely, resolving the duplicate short sequences
by their positions within the run. This enables full table and resource-field
pointer checks without adding comparison-only declarations to game code.

A per-function `EXACT` result covers only that entry's
span up to the next exported function, even if the next function has no source
implementation. Cross-check `make compare-functions` address shifts before
treating the report as complete code coverage. Review discovered entries
against the raw bytes: a jump table can also look like an instruction stream.

The current binary-comp map reader recognizes function owners ending in
`.obj`, but omits the `.dll` owners of import stubs. This makes the reported
rebuilt span of `ix_sample_convert_from_ieee_extended` extend to `fclose`
instead of `DirectSoundCreate`. The original export has the import boundary
at `0x004492C6`. With the sample loaders' sizes corrected, the report's
`SIZE +26` comprises three six-byte import stubs and eight alignment bytes;
the converter's actual body is 378 bytes in both images. Binary-comp's
`compare_function_bytes` API with explicit ends at the two corresponding
`DirectSoundCreate` entries now finds 10 differing instructions out of 116
in that body, after recovering the original local stack slots. Do not shorten
the function to compensate for the report boundary.
Whole-file `byte-compare` is unaffected by this limitation.

For literal identity of the complete files, including headers and relocations:

```sh
binary-comp byte-compare --fail-on-difference data/full/WC1.ORI.EXE WC1.EXE
```

For a complete refresh, run binary-comp's
[`ExportToCompile.java`](https://github.com/gg-sl-oss/binary-comp/blob/main/ghidra_scripts/ExportToCompile.java)
from Ghidra and point it at `code-full/`. That export also preserves Ghidra
names and writes the decompiled files, `globals.h`, and `strings.txt` required
by call/global audits.

| Command | Required input |
| --- | --- |
| `make compare-func`, `report`, `order` | Disassembly exports, linked `WC1.EXE`, `WC1.map` |
| `make verify-calls` | Decompiled exports too |
| `make verify-globals` | `code-full/globals.h` too |
| `make compare`, `compare-functions` | Retail and rebuilt executables |
