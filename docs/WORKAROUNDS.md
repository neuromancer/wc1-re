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
[ORDER.md](ORDER.md), then increasing function address within each unit.
The queue below follows that order. Include the unit's remaining access
casts, captures, and redundant declarations when investigating its header
exception; the marker count alone is not a complete workaround inventory.

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
| 3 | `AUTO` — `0x403E50` | 1 | Wingman declaration order; `auto_pilot_sequence` comparison. Existing local-order and temporary-removal attempts failed. |
| 4 | `CUTSCENE` — `0x407710` | 546 | Typed `init_3Space_objects` prototype; group with ENDING and all scene-initialization callers. |
| 5 | `BRAINS` — `0x409760` | 460 | Wingman declaration order and omitted core/audio prototypes; shares the wingman group with AUTO and SHIP. |
| 6 | `GAMEFLOW` — `0x40EFE0` | 20 | Omitted room globals and scene-buffer declaration order; `PostMission` and `free_viewport`. |
| 7 | `JOYSTICK` — `0x4102B0` | 405 | Calibration-bound declaration order. |
| 8 | `COCKPT` — `0x413A10` | 1 | Omitted indicator-state declaration; `animate_pilot` operand order. Removing its volatile read also does not match. |
| 9 | `GEOM` — `0x418080` | 165 | Omitted prototype parameter names; inspect with the existing call captures. |
| 10 | `BARRACKS` — `0x41A9D0` | 39 | `DosMemcpy` count width and prototype names; investigate caller arithmetic with the full-width callee. |
| 11 | `FADE` — `0x41C510` | 391 | Omitted input-history and screen-storage declarations; shared group with MAIN and LOGIC. |
| 12 | `SHIP` — `0x41DEE0` | 11,952 | Wingman declaration order and omitted `call_enemy` prototype. |
| 13 | `LOGIC` — `0x422010` | 259,057 | Range/armor/tactic declaration order, prototype names, and shared unused-global groups. |
| 14 | `MAIN` — `0x4272F0` | 546 with shared hook | A common `DosInterruptHandlerHook` declaration resolves compilation, but six functions differ. Also has input/screen/joystick omissions. |
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

The ordered pass has reviewed WINMAIN and partially cleaned MONO. The next
entry is AUTO, followed by CUTSCENE and BRAINS. AUTO's existing one-byte
blocker should not trigger another identical local-permutation sweep: look
for a new shared wingman-declaration hypothesis or advance to CUTSCENE.

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
