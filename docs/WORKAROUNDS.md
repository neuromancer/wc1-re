# Removing matching workarounds

The native reconstruction should use one common header context by default,
with `SDL_PORT` selecting port-specific behavior. Remove the remaining
`WC1_*_SOURCE` exceptions incrementally, without losing literal executable
equality or replacing them with different source-specific header tricks.

Acceptance requires all **518,656 bytes** of `WC1.EXE` to match
`data/full/WC1.ORI.EXE`. The mandatory Makefile SHA-256 is
`d517169e57f1d6254682c03dec5c254b5101ccd787d191f639b0cac1e8a616c4`.
Use the installed MSVC 4.20 toolchain through **wibo**, with the existing
flags and link order. Do not use Wine.

## Execution order

Work **from start to end**: follow the compilation-unit order in
[ORDER.md](ORDER.md), then the actual compiler input order within each unit:
the source preamble, recursively included headers, and the function bodies.
Function address order locates the bodies but does not describe the earlier
header dependencies. Include access casts, captures, context-only includes,
and redundant declarations in the inventory; source markers alone are not
the complete list.

Keep the original file/include structure during compilation. A flattened
preprocessed file is useful for reading, but opening a header itself affects
CL's symbol numbering, so compiling that flattened file would change the
experiment. When a declaration-context change needs diagnosis, retain the
real compilation's `/Bk` intermediates and inspect the global/local symbol
keys and expression stream. These are diagnostic evidence; binary-comp and
the complete executable hash still decide acceptance.

Record each attempted removal's input location, first affected function,
whole-file difference, and accepted/blocked status. A source marker selects
its header branches; do not count deleting the marker and deleting those
branches as independent cleanups. Related branches may encode a pair of
declaration moves that must be tested together. After a blocked item, restore
it and continue forward; after an accepted item, use the simplified source
as the baseline for the next removal.

Earlier declarations and locals can change CL's choices in later functions
of the same unit. Starting at the beginning avoids repeatedly disturbing
later cleanups. A shared-header change can affect any unit, so process that
dependency as a group at its first affected entry and check every dependent
unit, including ones already reviewed.

Accept only exact changes. When a bounded investigation is blocked, retain
the matching source, record the cause and rejected attempts, and advance to
the next entry. A reviewed entry is not necessarily free of workarounds;
revisit a blocker when new evidence or a relevant header change warrants it.

## Procedure for each removal

1. **Record the dependency.** Find the source definition and every header
   consumer of the marker. Record whether it changes a type, omits a
   declaration, changes declaration order, or omits prototype parameter
   names. Check the unit boundary in [ORDER.md](ORDER.md), the actual source
   uses, and the original disassembly before changing ownership or types.
2. **Measure the isolated removal.** Start from an exact build. Remove only
   the marker in a scratch copy, freshly compile its object, and link a
   separate complete executable against the current production objects.
   For an `ix` member, rebuild its containing archive as well. Stop on any
   compile/link error; never compare a stale executable. Keep the candidate
   source, build log, binary-comp configuration, and result together.
3. **Locate the cause.** Use `binary-comp byte-compare` for the full-file
   difference, then `binary-comp report --no-build --bytes` and `compare`
   for the affected functions. Inspect operand widths, register assignments,
   instruction order, and data addresses. A large positional count can be
   the consequence of one size change. A 100% normalized function score can
   still hide different bytes or exchanged global operands.
4. **Test an ordinary source explanation.** Prefer removing redundant
   locals, unnecessary volatile casts, unused includes, and redundant
   forward declarations. Check the preceding functions too: their symbols
   can affect later code selection. Existing local declaration order and
   placement of private state can matter. Preserve lifetimes, side effects,
   initialized-data order, and all address annotations. Do not introduce
   dummy declarations, wrappers, new function-pointer captures, new inline
   assembly, or replacement per-source headers to hide the dependency.
5. **Accept only an exact candidate.** Rebuild with the original installed
   compiler and require zero differing bytes. Compiler diagnostics and
   symbol-hash models can suggest a candidate; they are not matching
   authorities. Record failed hypotheses so the next session can start from
   new evidence. A matching C spelling does not prove the original source
   used that exact spelling or declaration order.
6. **Integrate and verify.** Remove the dead header branches as well as the
   marker. Build the accepted changes together, rerun the complete byte and
   hash checks, then run the required audits, `make verify`, and
   `make modern`. Update this worklist and the evidence in
   [PATTERNS.md](PATTERNS.md). Keep a small exact checkpoint before starting
   another batch.

The shared-header cleanup can affect more than one unit even when the
first experiment changes only one source. The final production build must
therefore rebuild the header's dependents. See [EXPORT.md](EXPORT.md) for
forced rebuilds and the Make 3.81 timestamp pitfall.

Scratch binary-comp configurations must retain the native preprocessor
context: `_MSC_VER=1020` defined, and `SDL_PORT` and
`SDL_PORT_LEGACY_DEBUG_OUTPUT` undefined. Without it, a report can omit
native functions that have SDL alternatives. The full-file byte check
remains mandatory regardless of report coverage.

Run the acceptance checks from the repository root after integrating a
candidate. Each command must succeed before proceeding to the next:

```sh
make WC1.EXE
binary-comp byte-compare --fail-on-difference data/full/WC1.ORI.EXE WC1.EXE
make sort audit-addresses order verify-hash
make verify
make modern
make verify-hash
```

## When to work individually or as a group

| Dependency | Scope of the experiment |
| --- | --- |
| Only one unit's declaration context changes | Remove one marker and simplify that unit; this worked for DIB, near-heap allocation, and flight maneuvers. |
| Several markers select different orders or omissions for the same declarations | First measure each marker independently. Group them only around a specific proposed common declaration layout, then rebuild every affected unit. |
| Different argument or global types are selected | Investigate the definition and every caller/access first. Test the common type across the whole group; a matching caller alone is insufficient. |

Do not group unrelated markers just because each has a small byte count.
Conversely, do not stop at a failing marker-only probe: the removed exception
may become unnecessary after a real simplification in the same unit.

## Current worklist: 21 active markers

Names below omit the common `WC1_` prefix and `_SOURCE` suffix. Counts are
whole-executable positional differences from scratch probes, not remaining
bytes in the retained build, which is exact. Most are historical marker-only
measurements at `05937f6`; remeasure before using them to judge a candidate.
MONO and MAIN include the supporting changes described below.

| Order | Marker / unit start | Probe difference | Dependency / next investigation |
| ---: | --- | ---: | --- |
| 1 | `WINMAIN` — `0x401000` | 394,412 | Reviewed: three flags have original byte writes here and integer accesses elsewhere. Common integer declarations enlarge `CheckLauncherAndConfig` by 16 bytes. |
| 2 | `MONO` — `0x403500` | 6 with cleanup | Partially cleaned: the event-list omission is removed. Restoring the shared sprite prototype and removing the local declaration still swaps registers in `SeekDataFile`. |
| 3 | `AUTO` — `0x403E50` | 1 | Reviewed: wingman comparison still differs after removing either or both clearance temporaries from `auto_position`. |
| 4 | `CUTSCENE` — `0x407710` | 546 | Reviewed with ENDING/MISSION: a common short scene prototype still removes MISSION's original sign extension. Removing the three local landing prototypes separately leaves two bytes. |
| 5 | `BRAINS` — `0x409760` | 460 | Reviewed: grouped core/audio prototype restoration reaches 17 bytes in three functions, with the wingman exception retained. |
| 6 | `GAMEFLOW` — `0x40EFE0` | 20 | Omitted room globals and scene-buffer declaration order; `PostMission` and `free_viewport`. |
| 7 | `JOYSTICK` — `0x4102B0` | 405 | Partially cleaned: three direct bound products are exact. The X-bound declaration swap still causes two bytes; the maximum-Y/failure-value swap causes 405. |
| 8 | `COCKPT` — `0x413A10` | 1 | Reviewed: indicator omission, escort captures, six repeated prototypes, and both volatile reads remain blocked. Removing the indicator declaration globally also changes WINMAIN. |
| 9 | `GEOM` — `0x418080` | 297,911 after cleanup | Partially cleaned: `fix_objects_ijk` has one common named prototype; the redundant text-scan pointer is removed. Either remaining direct-call/prototype group still leaves seven bytes in `GetShipAccelerationRate`. |
| 10 | `BARRACKS` — `0x41A9D0` | 22 after cleanup | Partially cleaned: prototype names restored, two unused includes and the animation-frame copy removed. Only the copy-count type remains different; the caller increments CX while the callee forwards a dword. |
| 11 | `FADE` — `0x41C510` | 391 | Partially cleaned: countdown snapshot removed. Restoring the packet, screen, or input-history declarations separately still leaves nine, nine, or two bytes. |
| 12 | `SHIP` — `0x41DEE0` | 11,952 | Partially cleaned: weapon record accesses, damage-table cast, and afterburner frame copy. Restoring `call_enemy` with the unused assertion header removed leaves eight bytes in `replenish_shields`; the wingman exception separately leaves two. |
| 13 | `LOGIC` — `0x422010` | 259,057 | Reviewed: removing duplicate local prototypes while restoring common parameter names leaves 14 bytes in `find_ship_index` and `get_front_spot`. The resource-table cast is removed; header and volatile blockers remain. |
| 14 | `MAIN` — `0x4272F0` | 553 after cleanup, with shared hook | Partially cleaned: both message functions use standard varargs; two copies and the early stdio include are removed. Header omissions and input qualifiers remain blocked. Continue at `HandleSpaceFlightControls`. |
| 15 | `VIEW` — `0x42CDB0` | 32 | Event/calibration omissions, pitch/yaw order, and two omitted prototypes; three functions differ. |
| 16 | `ENDING` — `0x42FA90` | 798 | Scene-initialization prototype, with unnamed parameter; grouped with CUTSCENE. |
| 17 | `GR` — `0x4408A0` | 135,464 | Viewport declaration order and prototype names; separate layout propagation from the first code difference. |
| 18 | `IX_DSPS` — `0x445190` | 2,829 | Shared `ix.h` record/API omissions and count declarations. |
| 19 | `IX_DSPV` — `0x4467A0` | 2,373 | Shared `ix.h` record/API omissions and count declarations. |
| 20 | `IX_SYSTEM` — `0x447200` | 54 | Shared `ix.h` omissions; currently only `ix_system_assign_voice` and `ix_system_find_free_voice` differ. |
| 21 | `IX_SOUND` — `0x447A20` | 2,783 | Shared `ix.h` sound/voice declaration order and omissions. |

VIEW, GAMEFLOW, and IX_SYSTEM have now had a bounded follow-up investigation
at `5d8b643`; their marker-only differences remain 32, 20, and 54 bytes.
The accepted temporary removals below also leave those counts unchanged.
Revisit these markers when there is a new declaration-context or source
idiom hypothesis, rather than repeating local permutations.

The ordered pass has reviewed WINMAIN through LOGIC and MAIN through its
message functions. It has partially cleaned MONO, JOYSTICK, GEOM, BARRACKS,
FADE, SHIP, LOGIC, and MAIN. AUTO, CUTSCENE,
BRAINS, and COCKPT remain blocked; their probes below retain no source changes.
GAMEFLOW's preceding follow-up already covers its current source and
headers. Continue within MAIN at `HandleSpaceFlightControls` (`0x429160`),
then its remaining flight, damage, cursor, and palette functions.
Revisit a blocked entry only with new evidence or a relevant dependency
change, rather than repeating the recorded probes.

## Compiler input pass: MAIN

This pass starts from `de15577`. The isolated baseline is exact. Probes keep
the normal include graph and save the real compiler's `/Bk` intermediates.
Counts below are differing positions in the complete executable; every
nonzero candidate is discarded.

Three groups are accepted. The redundant early `<stdio.h>` include can be
removed because `wc1.h` supplies it. `ShowModalMessage` now uses the standard
`va_list` sequence already used by SDL, paired with removing `main`'s
`gameFlowResult` copy and testing `GameFlow()` directly in the loop.
`ShowOnScreenMessage` also shares its standard varargs sequence between
builds, paired with using its by-value `duration` parameter directly instead
of copying it into `messageDuration`. Each accepted group is byte-exact.

Adding the modal `va_list` alone leaves four bytes in
`calculate_damage_level`: the left/rear armor loads exchange their address
operands. The normalized function byte report labels this exact, but the
complete file comparison and detailed instruction comparison expose the
changed operands. Removing the GameFlow copy alongside it restores equality.
The original message functions' argument-address calculation and 52-byte
text buffers remain unchanged; these ordinary C spellings need no new
instructions. This does not prove the original author's exact spelling.

| Input entry / attempted removal | File difference | First affected function / result |
| --- | ---: | --- |
| `WC1_MAIN_SOURCE`, with the existing interrupt-hook declaration made common in scratch | 546 before cleanup; 553 after | `house_keep` before; `FadeFlightPaletteEntry` after. Six functions still differ after cleanup. Without the hook declaration, the marker removal does not compile. |
| Early `<stdio.h>` | 0 | Accepted. |
| Native `<dos.h>` | 1,291 | `player_input`. |
| Packet or screen declaration omissions | 573 each | `get_player_input`. |
| Input-history declaration omissions | 1,568 | PE layout also changes; first code regression is in `house_keep`. |
| Joystick dead zones, raw coordinates, or wait-prototype omissions | 236 each | `get_player_input`, then `RunSpaceFlight`. |
| `main` argument-slot, selection, or GameFlow result copy, individually | 281 each | `RunSpaceFlight`; only the GameFlow copy is accepted as part of the varargs group. |
| `house_keep` frame snapshot | 157 | `house_keep`. |
| `get_player_input` device-index copy | 281 | `RunSpaceFlight`. |
| `player_input` previous-yaw access qualifier | 73 | `player_input`. |
| Pitch word `memcpy` replaced with assignment | 14 | The preceding `players_flight_dynamics`; the edited input function itself stays exact. |
| Roll word `memcpy` replaced with assignment | 930 | `player_input`. |
| Key-OR, key-test, or previous-key access qualifier | 19 / 2 / 15 | `player_input`; the two-byte candidate changes `CMP AL,BL` to `TEST AL,AL` at `0x4286B7`. |
| Afterburner access qualifier | 1,338 | PE layout also changes; first code regression is in `player_input`. |
| Joystick-yaw access qualifier | 821 | The preceding `players_flight_dynamics`. |
| Cursor-visible, cockpitless-view, or view-center access qualifier | 1,292 / 1,259 / 1,298 | `player_input`. |
| Viewport-left, mouse-X, or key-value copy | 1,421 each | `player_input`. |
| Eight repeated prototypes before `SelectPreviousExternalViewObject` | 21 | `SelectPreviousExternalViewObject`; object and view/sentinel registers exchange DX/BX and BL/DL. |

The header and local-prototype omissions were also tested as a group.
Restoring all eighteen omitted declaration keys while removing seventeen
local prototype parameter names leaves 529 bytes, starting in `house_keep`.
Also removing any one of the three tested `main` copies leaves 532, starting
in `FadeFlightPaletteEntry`. Returning later local keys to their baseline
does not undo the changed global declaration context.

Without the eight local prototypes, folding the decrement into the loop
test still leaves 21 bytes. Moving the existing sentinel reset into the
loop leaves twelve, also rejected. Replacing the qualified key test with
a test of the existing key copy or a switch leaves 141 or 349 bytes.
All nine input access qualifiers and both word-copy probes were rechecked
after the accepted varargs cleanup; their difference counts are unchanged.

The marker and all header exceptions remain. Project totals stay at 21
source markers, 34 volatile access casts, and four volatile object/parameter
definitions. This checkpoint ends at the message functions; the next body
is `HandleSpaceFlightControls` at `0x429160`.

The native rebuild, source/address/order audits, full `make verify`, and
`make modern` pass. Both value checks cover 1,550 functions with zero
mismatches or skips; existing call/global diagnostics remain 11/55.
The complete byte comparison and mandatory SHA-256 still match all
518,656 bytes. The installed compiler and flags are unchanged. All native
compiles use wibo; Wine was not used.

## Compiler input pass: LOGIC

This pass starts from `23e263d`. The isolated baseline is exact. Probes keep
the normal include graph and save the real compiler's `/Bk` intermediates.
Counts below are differing positions in the complete executable; every
nonzero candidate is discarded.

| Input entry / attempted removal | File difference | First affected function / result |
| --- | ---: | --- |
| `WC1_LOGIC_SOURCE` | 259,057 | `crash_time`; it and `change_mission_type` change size. |
| Unused `<sys/types.h>` | 340 | `find_ship_index`. |
| Early target-range declaration | 6 | `scan_for_enemy`; the target and range loads at `0x00422FF7` and `0x00422FFE` exchange order. |
| Armor/tactic declaration swap | 31 | `evaluate_damage`; armor-term instruction ordering changes. |
| Packet or screen declaration omissions | 266 each | `detect_collisions`; thirteen functions differ. |
| Input-history declaration omissions | 906 | `detect_collisions`; eighteen functions differ. |
| Omitted mission/tactic prototype names | 389 | `find_ship_index`; seventeen functions differ. |
| Omitted front/rear-position prototype names | 906 | `detect_collisions`; eighteen functions differ. |
| Unused `<crtdbg.h>` | 280 | `detect_collisions`. Removing both unused includes leaves 367 bytes, starting in `trim_goals`. |
| Cockpit explosion-resource cast | 0 | Accepted: `pCockpitExplosionShape` already has the required `unsigned char *` type. |
| `real_crash_time` radius qualifier | 6 | `real_crash_time`; the argument loads and radius operands exchange order. |
| Redundant `unactive` declaration | 77 | `detect_collisions`. |
| `report_kilrathi_rout` mode copy | 91 | `report_kilrathi_rout`. |
| Mission/formation declaration group | 827 | `find_ship_index`. |
| Routed-mission declaration | 302 | `try2rout`. |
| Tactic-selection declaration | 301 | `set_special`. |
| `reset_tactic` qualified parameter read | 5 | `reset_tactic`; EAX and ECX exchange the promoted index and tactic. |
| Either `set_special` pointer/value capture | 69 | `any_enemy`; the edited function stays exact, but later functions change. |
| `close_behind` volatile parameter | 256,507 | `close_behind`; its global load uses CX instead of AX, growing its body by one byte and its aligned span by sixteen. |
| Range/resource callee declaration group | 246 | `scan_for_enemy`. |
| `get_follow_point` type capture, or either damage type/class capture | 38 each | `evaluate_damage`. |
| Animation object-count capture | 8 | `UpdateSceneAnimationObject`; the rotation value and divisor exchange CX/DI. |
| Animation object-index capture | 520 | `UpdateSceneAnimationObject`. |
| Animation goal-flags capture | 350 | `UpdateSceneAnimationObject`. |
| Separate skip-loop counter | 5 | `PlaySceneAnimation`; reusing `remaining` exchanges the skip counter and object pointer between BP/EDI. |

The strongest grouped candidate restores all 66 omitted prototype parameter
names and removes the duplicate local declarations that contributed those
66 names later in the unit. Only two functions then differ: six bytes in
`find_ship_index`, where the mission index and class use CX/EDX in place of
DX/ECX, and
eight in `get_front_spot`, where the destination and position pointers
exchange ESI/EDI. The actual first-parameter keys move from 27,345 to
27,399 and from 27,484 to 27,534, respectively. From `scan_for_enemy`
onward, the keys and generated bytes return to the exact baseline.

Removing `close_behind`'s volatile qualifier under that grouped context
adds no differences: the same fourteen bytes remain. Removing the radius
or tactic qualifier instead raises the total to twenty or nineteen.
Ordinary multiply/shift spellings in `get_front_spot` leave fourteen;
flattening the ship search's initial guard leaves 99. Moving the existing
resource definitions beside their users preserves their source order but
leaves 163. Removing the mode copy while retaining its unsigned switch
conversion and one existing later prototype leaves 34; also removing the
special-state pointer and retaining the existing two-parameter release
prototype leaves the same count. None of these grouped variants is retained.

Other bounded follow-ups also fail. Combining named spot prototypes with
the unused types-header removal leaves 239 bytes. Reversing the range
comparison leaves six, while a separate range rejection leaves 235.
Combining the radius terms into one sum, in either tested grouping, leaves
six. Reversing `close_behind`'s comparison leaves 256,507 positions; a direct
Boolean return leaves 256,509. The common armor order plus a direct tactic
read leaves 36, and removing both damage captures with that order leaves
227. Exchanging the existing animation property/value declarations does
not fix the eight-byte regression after removing the count capture,
despite the diagnostic model's prediction.

Only the redundant resource-table cast is removed. The marker, two volatile
access casts, and volatile parameter in this unit remain, as do its
header-context and declaration workarounds. Project totals remain 21 source
markers, 34 volatile access casts, and four volatile object/parameter
definitions. MAIN is next; revisit LOGIC with evidence that explains the
two remaining register assignments rather than repeating these probes.

The native rebuild, source/address/order audits, full `make verify`, and
`make modern` pass. Both value checks cover 1,550 functions with zero
mismatches or skips; existing call/global diagnostics remain 11/55.
The final complete byte comparison and mandatory SHA-256 still match all
518,656 bytes. The installed compiler and flags are unchanged. All native
compiles use wibo; Wine was not used.

## Compiler input pass: SHIP

This pass starts from `db2b5f0`. The isolated baseline is exact. The two
header exceptions, unused includes, and body candidates are measured with
the normal include graph and saved `/Bk` intermediates. Counts below are
whole-executable differing positions; failed candidates are discarded.

| Input entry / attempted removal | File difference | First affected function / result |
| --- | ---: | --- |
| `WC1_SHIP_SOURCE` | 11,952 | `sort_viable_target_list`; five other functions also differ. |
| Wingman declaration-order exception | 2 | `inflict_damage`; comparisons at `0x0041EA74` and `0x0041EB98` reverse their operands. |
| Omitted `call_enemy` prototype | 11,950 | `sort_viable_target_list`; four other functions also differ. |
| Unused `<sys/stat.h>` | 192 | `match_rotation_goal`; ten later functions also differ. |
| Unused `<setjmp.h>` | 376 | `rotate_object_to_goal`; five later functions also differ. |
| Unused `<assert.h>` | 272 | `rotate_object_to_goal`; five later functions also differ. |
| Common `call_enemy` prototype with `<assert.h>` removed | 8 | Only `replenish_shields` differs. Removing the whole marker with this header cleanup leaves eleven bytes, including `inflict_damage` and `Explosion`. |
| Slot byte arithmetic in `any_selected` and the preferred-type lookup in `remove_weapon` | 0 | Accepted: index the existing packed record type. |
| Scalar overlay copy in `remove_weapon` | 0 | Accepted: copy the named type, hardpoint, and disabled fields, preserving the original dword/word/byte sequence. |
| Whole-record assignment in that copy | 199 | `remove_weapon`; making the existing entry pointer typed leaves 136 bytes for field copies and 223 for whole-record assignment. |
| Target-sort outer/inner promoted indices, individually | 16 / 8 | Removing the outer index changes the sorter and `replenish_shields`; removing the inner index changes only the latter. |
| Either or both damage-path volatile casts | 14 | `inflict_damage`; the armor store and screen-X test exchange order. Also restoring the wingman declaration leaves sixteen bytes. |
| `call_enemy` promoted index | 22 | `call_enemy`, then `replenish_shields`. |
| `internal_damage` promoted victim index | 51 | `internal_damage`, then `replenish_shields`. |
| Damage-table pointer reinterpretation | 0 | Accepted: convert the table value to signed char and then short. The original sign-extending byte load remains. |
| Player weapon-count volatile read | 473 | `your_internal_damage`; instruction selection and positions change. |
| Either child-search parameter snapshot | 15 | `find_child_object`, then `replenish_shields`. |
| Each `fire` offset, turret loadout offset, mine/exhaust promoted index, or either repositioning index | 8 each | The edited function stays exact; `replenish_shields` differs later. |
| Raw type/hardpoint scalar accesses in `fire_weapon` | 0 | Accepted: use the packed record's fields. |
| Replace `fire_weapon`'s byte offset with a typed slot pointer | 1,204 | `fire_weapon`; retain the existing offset calculation. |
| Fixed-projectile loadout offset | 25 | `fire_fixed_projectile_weapon`, then `replenish_shields`. |
| Afterburner frame snapshot | 0 | Accepted: read the frame global directly in both expressions. |

Restoring `call_enemy` adds two frontend symbols before the bodies. Removing
`<assert.h>` removes two, preserving the parameter bases of
`initialize_object` (27,265), `sort_viable_target_list` (27,395), and
`call_enemy` (27,464). Its definition then needs one fewer new symbol:
`internal_damage` moves from 27,473 to 27,472 and `replenish_shields` from
28,054 to 28,053. The latter's `current` local moves from 28,064 to 28,063;
SI and BX exchange roles and its comparison/jump reverse, leaving eight
bytes. Removing its type, delay, or current captures, combining the first
two, flattening the recharge condition, or grouping its value declarations
does not make this header candidate exact. The flattened condition with
the original headers separately leaves seven bytes.

The accepted record accesses use the existing seven-byte packed type, so
the SDL-only copy/access alternatives can be removed too. No new pointer
capture, declaration substitute, helper, or assembly is introduced. The
remaining marker still differs by 11,952 bytes after the accepted changes;
the prototype/assertion group still differs by eight. There remain 21
source markers, 34 volatile access casts, and four volatile object/parameter
definitions. SHIP retains its three access qualifiers. LOGIC is next.

The native rebuild, source/address/order audits, full `make verify`, and
`make modern` pass. Both instruction-value checks cover 1,550 functions
with zero mismatches or skips; existing call/global diagnostics remain
11/55. The final binary-comp comparison confirms all 518,656 bytes match,
with SHA-256
`d517169e57f1d6254682c03dec5c254b5101ccd787d191f639b0cac1e8a616c4`.
The installed compiler and flags are unchanged. All native compiles use
wibo; Wine was not used.

## Compiler input pass: FADE

This pass starts from `9995022`. The isolated baseline reproduces all
518,656 bytes. Traverse the three shared-header omission groups before
the captures in `StepPaletteTransition` and `DosStrcat`. FADE has no
volatile qualifiers, extra includes, or repeated local prototypes.

| Input entry / attempted removal | File difference | First affected function / result |
| --- | ---: | --- |
| `WC1_FADE_SOURCE` | 391 | `StepPaletteTransition`; address-operand order changes shorten an instruction and move subsequent instructions within its unchanged aligned span. |
| Packet-handle declaration omission | 9 | `StepPaletteTransition`; maximum/loop comparisons and one indexed address change operand order. |
| Screen-storage declaration omission | 9 | `StepPaletteTransition`; the same comparison/address differences. |
| Six input-history declaration omissions | 2 | `StepPaletteTransition`; the comparison at `0x0041C5CC` and its conditional jump reverse. |
| Allocation byte-count temporary | 446 | `StepPaletteTransition`; first-loop index and difference registers change. |
| Palette pointer snapshot | 402 | `StepPaletteTransition`; the component-count load moves before the initialization test. |
| Countdown snapshot | 0 | Accepted: test the existing countdown's post-decrement directly. |
| Accumulator element pointer | 64 | `StepPaletteTransition`; final-loop register allocation and instruction positions change. |
| String-end temporary | 18 | `DosStrcat`; nesting the search call changes argument evaluation and push order. |

The accepted countdown removal preserves the original word load, memory
decrement, and test of the saved value at `0x0041C688`. It removes one
frontend symbol: `StepPaletteTransition` still starts at key 27,231, its
accumulator moves from 27,240 to 27,239, and `DosStrcat` starts at 27,261
instead of 27,262. The actual `/Bk` expression references confirm these
keys; binary-comp confirms the complete executable.

Restoring either three-declaration group shifts the first function's
parameter key by three; the six-declaration history group shifts it by six.
The complete marker removal shifts it by twelve. Removing the countdown
snapshot does not resolve any of these header differences. Writing the
history candidate's maximum comparison with `difference` on the left also
leaves the same two bytes. No failed header variant is retained.

The other four capture removals were measured again on the accepted
countdown baseline and retain the counts above. Keep their matching source
forms; the assembly does not establish that each capture was absent from
the original source. There remain 21 source markers, 34 volatile access
casts, and four volatile object/parameter definitions. SHIP is next.

The native rebuild, source/address/order audits, full `make verify`, and
`make modern` pass. Both instruction-value checks cover all 1,550 functions
with zero mismatches or skips. The final binary-comp comparison confirms
all 518,656 bytes match, and the mandatory SHA-256 remains
`d517169e57f1d6254682c03dec5c254b5101ccd787d191f639b0cac1e8a616c4`.
The installed compiler hash and flags are unchanged. All native compiles
use wibo; Wine was not used.

## Compiler input pass: BARRACKS

This pass starts from `46c8e7c`. The isolated baseline is exact, and actual
`/Bk` intermediates preserve the compiler's input structure. BARRACKS already
has no volatile qualifiers; its remaining candidates are declaration
context, pointer-based field accesses, and the animation-frame snapshot.
All counts below are whole-executable positional differences from
binary-comp, not the number of unmatched instructions.

| Input entry / attempted removal | File difference | First affected function / result |
| --- | ---: | --- |
| `WC1_BARRACKS_SOURCE` before cleanup | 39 | `SetAwakenBarracksMenuLabel`; two later functions also change. |
| Omitted `DosMemcpy` parameter names alone | 17 | `LoadGame`, then `DrawBarracksStaticDetails`. |
| Narrow copy-count type alone | 22 | `SetAwakenBarracksMenuLabel`; the copy increment becomes `INC ECX`. |
| Unused `<dos.h>` alone | 6 | `DrawBarracksStaticDetails`. |
| Unused `<process.h>` alone | 12 | `FreeBarracksMenuLabel`; two later functions also change. |
| Unused `<sys/stat.h>` alone | 286,937 | `CreateEmptySaveGameFile`; a later bunk-rendering span shrinks by sixteen bytes. |
| Unused `<assert.h>` alone | 44 | `SaveGame`; two later functions also change. |
| Panel rectangle overlay replaced by four field assignments | 290,531 | `InitializeModalTextPanel`; its span grows by sixteen bytes. |
| Viewport rectangle overlays replaced by field assignments | 270 | `InitializeModalTextPanel`; instruction selection and scheduling change. |
| Empty-save position byte-pointer casts replaced by direct vector fields | 130 | `CreateEmptySaveGameFile`. |
| Loaded occupied-byte cast replaced by `diskRecord.occupied` | 9 | `LoadGame`; the occupied-byte load moves before the last description-byte store. |
| `AnimateBarracks` frame snapshot | 0 | Accepted: use the existing bunk-state field directly. |
| Prototype names restored with `<dos.h>` and `<assert.h>` removed | 0 | Accepted on top of the animation cleanup. |
| Both initial load-record accesses simplified together | 287,220 | `LoadGame`; its span grows by sixteen bytes. |
| Remaining marker after cleanup | 22 | `SetAwakenBarracksMenuLabel` is the only affected function; instruction positions and relocations change. |

Restoring the three prototype names adds three symbols before the bodies.
Removing `<dos.h>` subtracts one, and removing `<assert.h>` subtracts two.
Together they retain the original parameter keys: 27,330 for
`InitializeModalTextPanel`, 27,423 for `SetAwakenBarracksMenuLabel`, 27,508
for `LoadGame`, and 27,583 for `DrawBarracksStaticDetails`. The frame-copy
removal independently moves `HandleBarracksBunkSelection` from 27,638 to
27,637 without changing any executable byte. The native source and shared
header retain no replacement declarations or captures.

The unused process and stat includes remain blocked with the accepted
changes too, leaving twelve and 286,937 bytes respectively. Removing the
marker now preserves every recorded function base: both `DosMemcpy`
branches have the same parameter names. Its sole remaining dependency is
the unsigned-short count type. At `0x0041B164`, the original uses `INC CX`
and then pushes ECX; the common `size_t` declaration emits `INC ECX`, making
the instruction one byte shorter. The callee at `0x004354F0` forwards the
whole dword to `memmove`. Earlier short-count definition/local probes still
apply; this pass does not replace the mismatch with a different type trick.

The retained cleanup removes two unused includes, the prototype-name
omission, and one redundant local. There remain 21 source markers,
34 volatile access casts, and four volatile object/parameter definitions.
FADE is the next entry.

The native rebuild, mandatory hash, source/address/order audits, full
`make verify`, and `make modern` pass. Both instruction-value checks cover
all 1,550 functions with zero mismatches or skips. The final binary-comp
comparison confirms all 518,656 bytes still match. The installed compiler
hash and flags are unchanged; all native compiles use wibo, without Wine.

## Compiler input pass: COCKPT and GEOM

This pass starts from `949669e`. Both isolated unit baselines reproduce the
complete executable. Every count below comes from binary-comp on a freshly
linked scratch executable; no failing candidate is retained.

COCKPT is traversed from its header exception through the escort helper,
the repeated local prototypes, and the two qualified reads:

| Input entry / attempted removal | File difference | First affected function / result |
| --- | ---: | --- |
| `WC1_COCKPT_SOURCE` | 1 | `animate_pilot`; reversed comparison encoding. |
| Indicator declaration removed from the shared header entirely | 0 for COCKPT; 22 for WINMAIN | Owner remains exact, but the first other unit changes `WarpMouseTo`; wider rebuild is unnecessary to reject this candidate. |
| Each of the three `escorting_a_ship` captures, independently | 63 each | `flag_reached`; removing all three leaves 221 bytes. |
| Repeated `FixedToShortSaturating` or `RandomBelowOrEqual` prototype | 25 each | `ShowHudTextLine`; its two argument registers exchange. |
| Repeated `transform_to_objects_frame`, `DrawViewportPixel`, or `GetViewportPixel` prototype | 183 each | `draw_3d_scanner`; additional later functions differ. |
| Repeated `rectangular_to_spherical` prototype | 181 | `draw_target_box`. |
| Entire six-prototype block | 993 | `objective_lost`. |
| HUD-message volatile access cast | 177 | `RestoreTransientCockpitGraphics`; ECX becomes EAX and shortens the absolute load by one byte. |
| Pilot-frame volatile access cast | 28 | `animate_pilot`; removing its marker too leaves six bytes with the two globals exchanged. |

The indicator is only referenced by COCKPT, but removing its unused shared
declaration still changes WINMAIN's compiler context. The two module
measurements above each substitute only that module's rebuilt object; they
are not a full rebuild with the declaration removed. In COCKPT, removing
the marker moves the indicator's frontend key from 27,262 to 23,688 without
changing the recorded function bases. Removing one escort capture instead shifts
`flag_reached` from 27,648 to 27,647. Removing a one-parameter repeated
prototype shifts `ShowHudTextLine` from 27,872 to 27,871. The expression
intermediate confirms the keys used by the actual compilation.

Giving the ordinary HUD snapshot block scope, or removing the snapshot
entirely, still leaves 177 bytes. Using the stored pending-message global
or an assignment argument in `ShowHudTextLine` does not repair the
one-prototype removal's 25 bytes. These variants are rejected.

GEOM's accepted group removes `MeasureTextPixelWidthClamped`'s redundant
`scan` copy and restores the named `fix_objects_ijk(short obj)` declaration
for its owner. The existing `text` parameter supplies the cursor. Restoring
the prototype adds one frontend symbol before the first function; removing
the copy removes one before the next function. The first function's
parameter key changes from 27,232 to 27,233, while `SeekPacketSection`
retains 27,241 and `GetShipAccelerationRate` retains 27,295. Every executable
byte matches. Other units already saw the named prototype.

| GEOM input entry / attempted removal | File difference | First affected function / result |
| --- | ---: | --- |
| Marker before this pass's accepted cleanup | 165 | `GetShipAccelerationRate`. |
| `ComputeVectorDelta` prototype omission alone | 303,827 | `GetShipAccelerationRate`; later rotation spans also grow. |
| Either one-parameter prototype omission alone | 297,664 each | `GetShipAccelerationRate`; later code shifts. |
| Text-scan copy alone | 618 | `CollectActivePaletteIndices`. |
| Text-scan copy plus `fix_objects_ijk` prototype exception | 0 | Accepted; the shared named prototype is now unconditional. |
| Damage-limit copy plus either one-parameter prototype exception | 2 | `damage_ion_drive`; `CMP EDX,EAX; JL` becomes `CMP EAX,EDX; JG`. |
| Marker after the accepted cleanup | 297,911 | `GetShipAccelerationRate`; remaining declaration shifts change later code sizes. |
| Damage-read volatile cast after cleanup | 30 | `damage_ion_drive`; load scheduling and registers change. |
| Damage-limit copy after cleanup | 297,905 | `damage_ion_drive`; later code shifts too. |
| Delta-call capture alone after cleanup | 297,933 | `MinInt`. |
| Shrink-call capture alone after cleanup | 297,903 | `rotate_about_j`. |
| Delta-call capture plus both remaining prototype exceptions | 7 | `GetShipAccelerationRate`; the input and promoted-index registers exchange. |
| Shrink-call capture plus its prototype exception | 7 | Same acceleration-register exchange. |
| Facing low-word pointer read replaced by a cast, point variant | 125 | `get_facing_range_from_point`; original stack behavior is lost. |
| Facing low-word pointer read replaced by a cast, object variant | 297,168 | `get_facing_range_from_object`; its span shrinks by sixteen bytes. |

Restoring `shrink_vector`'s parameter name instead of `fix_objects_ijk`'s
also pairs exactly with the text-scan removal, but the accepted change uses
the earlier header entry. Restoring all names and removing both call
captures before the text cleanup leaves seven bytes; adding the text
cleanup leaves 297,910. The counts demonstrate that removing more symbols
is not a monotonic route to matching. No alternative header placement,
new capture, or replacement workaround is retained.

This checkpoint removes one header exception and one redundant local. It
does not remove a source marker or volatile qualifier: 21 markers, 34 access
casts, and four volatile object/parameter definitions remain. The next
entry is BARRACKS.

The shared-header dependents were rebuilt with wibo. The production link,
complete binary-comp byte comparison, mandatory hash, source/address/order
audits, full `make verify`, and `make modern` pass. Both instruction-value
checks cover 1,550 functions with zero mismatches or skips. The final image
still matches all 518,656 bytes; the installed compiler hash is unchanged.
Wine was not used.

## Compiler input pass: JOYSTICK

This pass starts from `7224617`. The unmodified isolated compile reproduces
the exact executable before any removal is attempted. Its normal include
structure is retained, and `/Bk` supplies the actual symbol records.
All differences below are complete-file byte counts from binary-comp.
The first affected function for every rejected candidate is
`CalibrateJoystickInteractive` (`0x004102B0`).

| Input entry | File difference | Result |
| --- | ---: | --- |
| `WC1_JOYSTICK_SOURCE`, selecting both header swaps | 405 | Blocked; identifies the combined declaration dependency. |
| `<sys/types.h>` before `wc1.h` | 333,245 | Blocked; calibration's aligned span grows by sixteen bytes. |
| `<crtdbg.h>` before `wc1.h` | 333,269 | Blocked; calibration's aligned span grows by sixteen bytes. |
| X-bound declaration swap in `globals.h` | 2 | Blocked; `CMP ESI,EDX; JLE` becomes `CMP EDX,ESI; JGE` at `0x0041054E`. |
| Maximum-Y/failure-value declaration swap in `globals.h` | 405 | Blocked; register choices and instruction positions change. |
| Upper-bound in-place scale multiplication | 0 | Accepted: calculate the product directly in the bound expression. |
| Right-bound in-place scale multiplication | 0 | Accepted on top of the upper-bound cleanup. |
| Lower-bound in-place scale multiplication | 0 | Accepted on top of both earlier cleanups. |

The three accepted edits remove destructive updates to scale locals after
their scale values have been published. Each bound now expresses its centre
plus or minus scale times range, as the minimum-X bound already did. There
are no new locals, casts, calls, or assembly instructions.

The baseline symbol records assign minimum-X/maximum-X keys 23,872/23,874
and failure-value/maximum-Y keys 23,873/23,885. The two header exceptions
exchange those pairs. Removing `<sys/types.h>` shifts the observed global
and local keys by seven; removing `<crtdbg.h>` shifts the observed globals
by 36 and locals by 35. This is why a single uniform offset is insufficient
to describe every include removal. All recorded keys remain unchanged for
the three accepted expression simplifications.

Rechecking both header swaps and the complete marker removal with the
simplified source leaves their two/405/405-byte differences unchanged.
They remain documented blockers. This batch removes no source marker or
volatile qualifier: 21 markers, 34 access casts, and four volatile
object/parameter definitions remain.

The native rebuild, `make modern`, source/address/order audits, and full
`make verify` pass. Both instruction-value checks cover 1,550 functions
with zero mismatches or skips. The final full-file byte comparison and
mandatory hash still pass; the installed compiler is unchanged. Wine was
not used.

## Ordered pass: AUTO, CUTSCENE, and BRAINS

AUTO's marker removal still reverses the comparison at `0x004041D4`.
Inlining `auto_position`'s collision-radius sum, separation expression, or
both into its clearance test leaves that one-byte difference unchanged.
The existing local-order probes are not repeated. The marker remains.

The shared scene prototype still needs to explain MISSION as well as
CUTSCENE and ENDING. Under a common unnamed short prototype, casting
MISSION's series argument to `int`, `long`, or `unsigned short` still
removes the original `MOVSX ECX,SI` and leaves 88 differing file positions.
Removing CUTSCENE's unused `<dos.h>` with that prototype reduces its
sixteen-byte difference to fourteen, but the caller group is still inexact.

Separately, CUTSCENE's three redundant declarations before `landing`
(`FetchDiskPacketRetrying`, `spacetrack`, and `SetEventManagerPump`) each
cause the same two-byte regression when removed, alone or together, with
the existing scene prototype retained. The loop comparison at `0x0040869A`
reverses operands and its branch condition. Moving the existing frame local
among its declarations or also removing `<dos.h>` does not fix it. No
replacement declaration or capture is introduced.

BRAINS' full marker removal still differs by 460 bytes. Restoring the
formation/wingman and music-control prototype groups together, while
removing unused `<sys/types.h>`, reduces the difference to seventeen:
seven register bytes in `cruise_home`, three wingman-comparison bytes in
`imperial_formation`, and seven register bytes in `strike_mission`.
Swapping `cruise_home`'s existing locals does not help. Giving this candidate
the common wingman declaration instead of its exception increases the
difference to 397 bytes. Other tested prototype groups also remain inexact.

All candidates were isolated wibo builds compared by binary-comp; none is
retained. The production source, headers, compiler, marker count, and
volatile inventory are unchanged. The mandatory hash and complete byte
comparison still pass: all 518,656 bytes match. No full verification rerun
is needed for this documentation-only checkpoint.

## Ordered pass: WINMAIN and MONO

WINMAIN's marker-only probe still differs in `CheckLauncherAndConfig`,
whose span grows by sixteen bytes. The original writes one byte to each of
three integer-backed flags. Changing their common storage types would also
need to explain the integer accesses in MAIN, AUTO, SHIP, and other callers.
The previously tested byte casts change register allocation. No replacement
cast or per-source declaration is introduced; this marker remains blocked.

MONO now sees the three common event-list declarations. Its existing console
buffer, availability flag, and driver handle are declared beside
`MonoDebug_install`. Their storage, spelling, visibility, and addresses are
unchanged. The three existing linker-common definitions follow that state,
keeping the storage declarations in ascending address order. This removes
MONO's exception in `globals.h`; the complete executable remains exact and
the layout audit needs no new exception.

The remaining `WC1_MONO_SOURCE` consumer only omits `DrawSpriteDefault` from
the shared function header in favor of its existing local declaration.
Removing that exception and the redundant local declaration initially leaves
45 bytes in four functions. With the console-state placement, only six bytes
remain: `SeekDataFile` swaps ESI and EDI for its offset and result. File-buffer
placement, ordinary result type spellings, assignment in the condition, and
16-bit seek-origin declarations do not resolve it. Those probes are discarded.

Before adopting the ordered pass, the MAIN/FADE/LOGIC probes reproduced their
blockers. A common DOS interrupt-hook declaration permits MAIN to compile
without its marker but leaves 546 bytes in six functions. FADE still differs
by 391 bytes after removing its palette snapshot, countdown snapshot, and
byte-count temporary individually or together. LOGIC's marker removal still
changes two function sizes and leaves 259,057 positional differences. None
of these failed source or header variants is retained.

## Follow-up: VIEW, GAMEFLOW, IX_SYSTEM

Two ordinary C simplifications are accepted with the existing headers:

- `InitializeFireworks` writes `-1` directly, removing the constant `empty`
  temporary. The original full-width constant load and word stores remain.
- `free_viewport` indexes the allocation table with `nViewAllocTotal--`,
  removing `oldCount`. The original pre-decrement index and decrement-before-
  table-load instruction order remain.

Both isolated candidates and the combined production build preserve all
518,656 executable bytes. No source marker or volatile qualification is
removed in this follow-up: 21 markers, 34 access casts, and four volatile
object/parameter definitions remain.

| Marker | Remaining functions | Follow-up result |
| --- | --- | --- |
| `VIEW` | `OpenPacketSection`, `AllocateViewport`, `AlignSpriteFrameToRectCorner` | Removing selected temporaries individually still leaves 32 bytes. Moving private packet/target state fixes the packet comparison but introduces differences in earlier effects routines and leaves the viewport differences. |
| `GAMEFLOW` | `PostMission`, `free_viewport` | Private-state placement and existing local ordering leave 20 bytes. The accepted count simplification also leaves 20 bytes when the marker is removed. |
| `IX_SYSTEM` | `ix_system_assign_voice`, `ix_system_find_free_voice` | Multiplication order/grouping, loop forms, and existing local scopes do not remove the 54-byte difference. |

VIEW's command snapshot, counter snapshot, and promoted section index can
each be removed independently with its marker retained. They cannot simply
be combined with the accepted constant removal: later constellation-loop
and rectangle-area instructions then differ. Those additional removals are
not retained. This is why an individually exact function or cleanup is not
enough to accept a group.

## Accepted batch: DIB, NEARHEAP, FL

| Removed marker | Marker alone | Accepted supporting cleanup | Final complete EXE |
| --- | ---: | --- | ---: |
| `WC1_DIB_SOURCE` | 14 | Remove unused `memory.h`/`sys/utime.h` includes and the redundant `DIBslamReal` declaration; group frame dimensions and the image pointer before other BSS state. | 0 differences |
| `WC1_NEARHEAP_SOURCE` | 12 | Place initialized heap state after the two descriptor helpers; remove `nextDescriptorAddress`; adjust existing locals in allocation/free routines. | 0 differences |
| `WC1_FL_SOURCE` | 42 | Remove `Mfish_hook`'s ship-index copy; declare the existing promoted index before the scan counter in `Mtarget_missile`; replace `Mveer_away`'s volatile range capture with a direct comparison. | 0 differences |

The three candidates also match when built together. All global storage
addresses and compilation-unit boundaries remain unchanged. The flight
change removes one volatile access cast, leaving 34 access casts and four
volatile object/parameter definitions. The guarded allocator's existing
assembly exception is separate from the near-heap allocator changed here.

DIB's grouping needs two additional reviewed declaration-order notes in
the layout-audit configuration. These describe unchanged storage placed by
identifier hash; the complete BSS and executable checks remain mandatory.
Declaration order is still a matching constraint, even after removing the
source-specific header switch.

## Rejected approaches worth preserving

- A common unnamed `init_3Space_objects(short)` prototype makes ENDING exact,
  but leaves 16 differing bytes in CUTSCENE and 88 in MISSION. The original
  `init_mission` sign-extends its series word before pushing it; the typed
  prototype removes that instruction. TITLE, LOGIC, and AUTO stay exact in
  this experiment. Do not accept the apparently successful ENDING result
  while the other callers regress.
- DIB's two unused includes reduce its marker-only difference from 14 to
  four bytes; the frame-state grouping is also required. Removing the
  existing local `DIBcascade` declaration still regresses, so it remains as
  a separately unresolved redundant declaration.
- Simply removing NEARHEAP's marker reverses comparisons in three routines.
  State placement, the redundant next-address removal, and the two local
  declaration changes are needed together. Compiler-model predictions of
  local permutations did not by themselves reproduce both loop comparisons.
- FL's redundant promoted scan-index removal helps `perform_maneuver`, but
  leaves the wrong scan registers in `Mtarget_missile`. Keep the existing
  useful scan index; removing the preceding `Mfish_hook` copy instead allows
  both functions to match.
- Moving GAMEFLOW's private initialized state, folding its saved allocation
  count into a decrement, or changing the saved allocation pointer to
  `void *` leaves its 20-byte difference. Reversing IX_SYSTEM's multiply
  factors and ordinary loop-form changes also fail. None is retained.
