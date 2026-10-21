# Matching patterns

The original disassembly and extracted strings are authoritative. Decompiled
code, operational labels, and other releases are only leads. Use `binary-comp`
for similarity decisions.

Binary-comp's byte report uses positional comparison for equal-sized spans
and aligned instruction encodings when their sizes differ. A differing-
instruction count across a size correction therefore changes its basis.
Inspect the original instructions and both span sizes as well as the count;
do not reject a recovered size solely because its positional count is larger.

## Source choices that affect matching

- A consumer unit may declare another unit's global with a narrower type.
  `CheckLauncherAndConfig` stores single bytes to three of `main.c`'s int
  flags while `main.c` stores dwords. Byte casts (`*(unsigned char *)&x = 0`)
  reproduce the stores but not the allocation: their zero constant attracts
  EBX, whereas byte-typed globals leave EBX to the `'-'` comparison and store
  immediate zeros, as the original does. A `WC1_WINMAIN_SOURCE` branch in
  `globals.h` declares the three flags as `char` for winmain.c only, keeping
  the declaration count and order. The function becomes exact at all 64
  probe offsets and its 16 missing bytes return. When an owner and a consumer
  disagree on access width, try the consumer's width as a declaration before
  reaching for casts.
- VC++ 4.20's file-static C globals and uninitialized C++ globals have
  name-dependent COFF ordering.
  Definition order, first-use order, and the assembly listing are not reliable
  substitutes for actual COFF offsets. Isolated sound-system compilations
  retain the same order after removing or reversing function bodies and
  reversing definitions, but change it when identifiers change. Keep names
  descriptive and preserve types and storage: `nAllocatedStreams` puts the
  allocation count before the stream array, recovering 120 bytes instead of
  124; `aVoices` puts the voice array before its lock and counters. The DSP
  and system names likewise recover their original relative positions.
  These are matching spellings, not recovered original names. Validate the
  complete function report and named reference targets after renaming;
  normalized instruction equality alone can hide wrong global operands.
  The four adjacent audio `.bss` blocks are covered by the unit-data gate.
- C file statics follow a measured rule. CL 4.20 orders a unit's
  uninitialized statics in its `.bss` by ascending key
  `(h ^ (h >> 16)) & 0x3FF`, where C1 hashes the identifier as
  `h = (h >> 4) + 4 * h + c`. Equal keys go in reverse declaration order.
  Arrays of eight or more bytes are 8-aligned, smaller objects 4-aligned.
  An unreferenced uninitialized static keeps its slot only when its unit
  also has a referenced static, and it gets no COFF symbol. Choose
  descriptive spellings whose keys follow the original addresses; see
  "Static `.bss`" in `docs/ORDER.md`.
- Superseded by the static `.bss` checkpoint: the profiler, raster and
  streaming variables in the next four items, and the DIB, MONODEBG and
  keyboard groups described later, are now statics in their owners. Their
  shared declarations and owner exclusions no longer exist. The matches those
  scopes recovered are to be restored in a dedicated declaration-count pass.
- Unused flight-profiler declarations also affect core instruction selection.
  Keep them out of `eventmgr.c`, `init.c`, `trainsim.c`, and `video.c` through
  the corresponding owner markers. This recovers normalized-exact output for
  `GetViewportIntersection`, `ReleaseInputEvent`,
  `initialize_direction_view_frames`, both high-score sorting/initialization
  routines, and `ReleaseContiguousPaletteEntries`, without changing their
  bodies. In the event owner, omit `PeekInputEvent`'s prototype parameter
  names; its callers retain their existing declaration. This preserves its
  exact match and improves the complete event-unit result, although
  `GetNextInputEvent` now exchanges EDI and EDX in six encodings of its first
  coordinate clamp. The two training text/update routines also gain two and
  one differing encodings. The complete report improves by 185 differing
  encodings and five exact functions overall; see `docs/ORDER.md`.
  These declaration scopes are matching choices, not recovered header text.
  Peek local declaration permutations, declaration initializers, const and
  register qualifiers, unsigned temporaries, a local pointer, and equivalent
  loop syntax do not repair its register swap when prototype names remain.
  Removing names from all four event-reader prototypes instead loses the
  rectangle match. Widening GetNext's limit or narrowing its saved X value
  is worse; long X, declaration order, an explicit field pointer, and a
  scoped X local do not remove its six differences. None of those body
  probes is retained.
- Additional core owners benefit from smaller combinations of the twelve
  profiler, three raster, and two streaming declarations. All seven group
  combinations were checked across eligible core units; definitions stayed
  in place, and groups actually referenced by a unit stayed visible. The
  retained owner combinations are recorded in `docs/ORDER.md`. They change
  34 function-report rows without changing any reported span size, remove
  794 differing encodings, and gain six normalized exact functions overall.
  In particular, profiler plus raster exclusions reduce `BarracksScreen`
  from 219 differences to 30 and `StepPaletteTransition` from 123 to six.
  Profiler exclusion alone reduces `PlayScrambleHangarScene` from 121 to 25;
  raster exclusion in the training owner reduces `SelectTrainSimMission`
  from 168 to 93 and raises it from 89.44% to 97.03% mnemonic similarity.
  No function bodies, types, storage, flags, or link order change. These
  measured declaration scopes are not recovered original headers.
  Four exact matches are lost alongside ten gains: `landing` gains two
  differing encodings, `UpdateDamageDisplay` six, `get_rear_spot` eight,
  and `scan_for_enemy` four. Other small regressions and the improved
  mnemonic score but increased byte differences in `evaluate_damage` are
  recorded with the checkpoint. Do not apply any tested declaration group
  globally: other owners gain size mismatches or lose more exact functions.
- The five graphics/streaming declarations for `stRasterClip`,
  `abSolidColourTranslation`, `abShapeRLEScratch`, `bStreamerAudioPlaying`,
  and `szStreamerPath` are unused outside `gr.c`. Omit them in `dib.c`,
  `eject.c`, `joystick.c`, `mono.c`, `nav.c`, and `screens.c` through their
  owner markers. This changes fourteen report rows and removes 354 differing
  encodings overall, gaining eight normalized-exact functions and losing
  two. The largest gains are `CalibrateJoystickInteractive` (213 to 39
  differences) and `PanToScreen` (115 to zero). Four small regressions are
  retained: `UpdateInflightNavText` gains five differences,
  `ReturnToBriefingLongShot` gains nine, and `EstablishingShot` and
  `SceneDirector` each gain two. In nav and screens, omitting only the
  three raster declarations or only the two streaming declarations loses
  additional exact matches; neither subset is retained. These are measured
  declaration scopes, not recovered original header text.
  A complete graphics-storage move with descriptive renames reproduces the
  order of the seven identified objects, but removes these declarations
  everywhere and changes 150 function rows, losing fifteen exact matches
  overall and introducing four size differences. An RLE-buffer-only move
  also regresses the complete report. Both storage moves are reverted;
  their recovered ordering and unexplained gap are recorded in `docs/ORDER.md`.
- `winmain.c` omits the twelve flight-profiler declarations and the three
  raster declarations through `WC1_WINMAIN_SOURCE`. It uses none of them.
  This makes the complete 272-byte `make_shard` exact and leaves only the
  F1-latch scheduling different in `MainWindowProc`. Preserve the latch's
  initial one-write in both branches with a qualified volatile store; the
  repeated-key branch then clears it. CL merges the shared store after the
  bit test, reproducing the original order and the complete 1,264-byte span.
  A cached bit or Boolean, a comma expression, or qualifying the original
  single store leaves that scheduling unchanged. Unqualified split stores
  lose the original transient write.
  The same scope changes initially disturb `CheckAllGuardedAllocations`.
  Capture the allocation's block before scanning its prefix and use that
  saved pointer in the first corruption report. Declare it before the scan
  cursor and counters. This retains the block in EAX and recovers the entire
  original 112-byte function. Its suffix report must still reload the block.
  Counter permutations, merging the counters, changing their integer types,
  and placing the block local after the counters or inside the loop do not
  recover this result.
  The two streaming declarations remain visible in this owner: omitting them
  as well swaps the mouse-warp coordinate registers. Local coordinate copies,
  parameter names, and register/const parameter qualifiers do not restore that
  match. Omitting only the profiler declarations instead disturbs window
  creation. These scopes are measured matching choices, not recovered headers.
  One regression remains: `FreeGuardedAllocation` reverses the operands of its
  tail-pointer comparison, with unchanged size, control flow, and other bytes.
  Reversing the source equality, qualifying the global read, caching the tail
  or predicate, and negating inequality all leave it unchanged. An early
  return adds sixteen bytes; separating pointer initializers and permuting
  their declarations introduces further differences. Omitting parameter names
  from owner-specific prototypes either has no effect or disturbs other
  functions. None of these probes is retained. The complete change removes
  94 differing encodings and gains one exact function overall.
- `ix/sound.cpp` omits 42 unused system, allocator, mixer, voice, stream,
  and streamer globals through `WC1_IX_SOUND_SOURCE`. Its five required
  declarations appear as `nActiveVoices`, `nSystemVoices`, `pIdleSoundList`,
  `pPlayingSoundList`, and `pQueuedSoundList`. This recovers the complete
  109-byte free-list unlink and 838-byte sound-start routines. Other owners
  retain their declarations, and no storage, types, flags, or link order
  changes. These are measured declaration scopes, not recovered header text.
  Explicit returns after the two release warnings reproduce their original
  jumps to the epilogue, making `ix_sound_release` exact over 177 bytes.
  In `ix_sound_reprioritise`, declare each selected sound and released voice
  inside its branch. This restores all five original local slots and the
  implicit object slot in the 24-byte frame. One comparison still loads the
  global head before the local sound instead of the reverse, leaving the
  routine one byte short and its final unit alignment one byte longer.
  All 120 orders of the five globals were checked with these local scopes;
  none removes this difference while retaining the other matches. Reversed
  equality, local pointer qualifiers, shared branch-local names, separated
  initializers, wider local scopes, an explicit else block, and qualified
  reads also leave it unchanged. None of these further probes is retained.
  A const-qualified free-function pointer parameter changes CL 4.20's C++
  mangling and fails to link; do not use it to tune operand selection.
- `ix_system_delete_all_sounds` declares each saved successor inside its
  own traversal loop. This restores sound at EBP-4 and the three successors
  at -8, -12, and -16, making the complete 169-byte function exact.
  `ix_system_assign_voice` keeps its sample in the outer scope, elapsed time
  and position in the positioning block, and loop length inside the looping
  branch. These restore the corresponding -4, -8, -12, and -16 slots.
  Moving only loop length into its branch leaves the other slots wrong.
  The system owner also excludes the 29 unused streamer globals through
  `WC1_IX_SYSTEM_SOURCE`. This recovers the original free-voice comparison
  and the complete 89-byte final span. Assignment retains four differing
  encodings in its two sample-rate equalities, down from 33 before the local
  correction. Without this declaration scope it retains two differences in
  a multiplication instead, but the free-voice routine loses its match.
  Omitting the six DSP globals, the three mixer globals, or all 38 unused
  globals makes assignment exact but leaves the free-voice comparison wrong.
  The remaining group combinations do not recover both. Reversing the
  loop-buffer multiplication operands does not improve the original-header
  result. None of those alternatives is retained.
- `ix_dspv_set_pan` uses nested local scopes to retain pan position at
  EBP-4, the voice pointer at -8, delta at -12, and step at -16. Its frame
  stays sixteen bytes; only its bounds-comparison operand order and related
  branch displacements remain different, leaving 292 bytes against 293.
  In `ix_dspv_recalc_mix`, the descriptive names `voiceState`, `panPosition`,
  and `scaledVolume` recover the original -12, -8, and -4 slots without
  additional scopes. All 178 body bytes then match. Its reported two-byte
  shortage is trailing object alignment, caused by the preceding voice
  comparison differences. These scopes and names reproduce allocation;
  they are not recovered original source spellings. Flat pan-local renames
  and grouped scopes do not recover all four slots. Nested mix scopes also
  work, but the descriptive flat declarations are sufficient.
  Excluding unrelated streamer, stream, and system globals from the voice
  owner, individually and in combinations, trades existing exact guards
  for other matches. Those declaration probes are discarded.
- Biased array operands can precede the actual array. The carrier-launch
  fighter loop loads from `0x0046A5AE + frame * 2` for frames 9 through 22.
  Its real 14-word table starts at `0x0046A5C0`; `frame - 9` reproduces the
  operand. The four bytes before the table are normal alignment, as are
  the four before the following view script. Including these bytes as
  array elements changes compiler layout and invents unused source data.
- Confirm arguments at call sites, return width from register use, and calling
  convention from the terminating `RET`. Core functions are normally
  `__cdecl`; `ix` methods receive the implicit object in ECX.
- Prefer `short` when the original emits 16-bit operations.
- `ObjectTypeData.lifetime` is a signed 32-bit field at offset `0x10`,
  also used as ship fuel capacity. `update_lights` divides by the full
  dword, and the damage routines divide it by four with signed rounding.
  The original Hornet, Rapier, Scimitar, and Raptor records contain
  200000, 250000, 280000, and 300000 respectively. The former pair of
  shorts (`lifetime` and the unused `weaponDamage`) split those values
  incorrectly. Combine their initializer words into one int and access
  the field directly; the packed 135-byte record and all following offsets
  remain unchanged. Projectile code can narrow the result while still
  loading the field as a dword, as `fire`, `fire_weapon`, and
  `house_keep_objects` do. The complete object-data gate checks that the
  corrected declarations preserve the original bytes and relocations.
- The word array at `0x0059C790` is unsigned object mass, copied from
  `ObjectTypeData.mass` at offset `0x0A`. Its only readers are the two
  force-application routines and `object_collision`: they zero-extend it
  before dividing force or computing mass ratios. The former `radarRadius`
  name and signed type were misleading. `awObjMassArray` preserves the original
  storage while expressing those reads directly.
- The runtime scale array at `0x0059DE40` is also unsigned. The original
  `house_keep_objects` halves it with a word `SHR`, and `animate_shape`
  uses `SHR` in both scale-command calculations. `transform_objects_to_your_view`,
  `ShowCarrierLaunchSequence`, `ShipExplosion`, and `reposition_fixed_child_objects`
  zero-extend their scale reads. Declaring `awObjectScales` as unsigned short
  reproduces those logical shifts without per-use casts; a signed array
  incorrectly preserves the high bit while shifting. Its storage size and
  original address annotation stay unchanged. The full function report
  improves only the two shift users and preserves every other result.
- Preserve visible local-variable order, branch direction, fall-through, and
  epilogue placement. Equivalent C often emits different instructions.
- `SortSignedByteValuesAscending` keeps the saved next index for the outer
  loop's closing test, but tests the narrowed live inner index at the inner
  loop's entry. Declare and initialize the swap value inside the inner
  `do` body. Together these choices recover all 112 bytes literally, with
  the original eight-byte frame, saved word at `[ESP+0x10]`, `CMP BP,AX` /
  `JLE`, and BL-held swap value. Scoping the value alone leaves four differing
  encodings: the saved word uses `[ESP+0x12]`, and the entry comparison and
  branch are reversed. Ordinary nested `for` loops recover the stack slot
  and branches but leave seven register differences; a promoted index or
  explicit element pointer does not improve the complete result. Volatile
  or int saved-index locals also fail and are not retained.
- `DebugOverlayConsole::DrawPendingLines` declares its device-context handle
  before its row counter. This keeps the handle in the original EBX and
  reduces differing encodings from twelve to nine over the unchanged
  128-byte span. The remaining differences exchange the row and cached
  `TextOutA` registers. Counter scopes, `for` syntax, declaration initializers,
  long counters, and register hints leave those nine differences. Constructor
  multiplication-order and count-capture probes do not fix its four existing
  differences; some also disturb the exact `Clear` routine. Inlining the
  process handle in `WaitForKey` increases its differences from nineteen to
  23. None of those additional changes is retained.
- `WaitForInputKey` dispatches the signed key through a `switch` for the
  ignored `0x1D` code. Its integer promotion reproduces the original
  `MOVSX EAX,BL` and 32-bit comparison, reducing differing encodings from
  31 to fifteen at the original 256-byte span. Mnemonic similarity falls
  from 98.48% to 92.42% because the remaining default and clear blocks have
  the opposite order; the byte comparison improves. Keep the existing
  guarded polling loop: replacing it with a plain `while` loses sixteen
  bytes. Explicit int/long promotions and plain-char keys leave the old
  byte comparison unchanged; switch case reordering and early exits do not
  improve the retained fifteen differences. `DrawTextAt`'s declaration-order,
  saved-pointer constness, and alignment signedness probes likewise leave
  its ten differences unchanged and are not retained.
- `fire` initializes its mine-present flag at declaration and dispatches
  weapon classes with a switch. Keep the weapon's byte offset as an integer
  while accessing the typed slot fields; retaining an absolute slot pointer
  changes both the load and the disabled-flag store. Assign the complete
  mine-firing condition to `shouldFire` after computing the intercept.
  A conditional update instead keeps the flag live across `vector_angle`,
  changing its register and the weapon-data registers. That call receives
  the owner's velocity first and the target's second. Together with the
  corrected lifetime field, these choices recover an exact 1,296-byte span
  from 93.18% and a sixteen-byte shortage. The original uninitialized angle
  check and contradictory facing conditions remain as observed.
- `fire_weapon` computes the projectile range in a separate assignment
  before `ScaleFixedVector`, and divides the missile interception distance
  by velocity before preparing the next vector call. Inlining either
  calculation into an argument changes argument scheduling. Its sound
  switch assigns the sound number and calls the playback function inside
  each case; CL merges their common call and cleanup. The default path
  reaches the single final return. Retaining the used sound-number local
  preserves the original register allocation, whereas literal-only calls
  also disturb later functions in the unit. With the 32-bit lifetime field,
  these choices recover an exact 1,248-byte span from 94.07% and a
  sixteen-byte excess, including both switch tables.
- `fire_missile` initializes its short weapon index in the `for` statement
  after loading the slot pointer and signed-byte count. The loop supplies
  its initial count test without a separate positive-count guard. Both firing
  paths pass the ship argument; replacing the player path's argument with a
  literal zero changes the original register argument. Together these forms
  reduce differing encodings from 71 to 24 of 79 instructions at the original
  208-byte span. This initially reversed a comparison/branch pair in each
  of `replenish_shields` and `your_afterburner`, increasing their differing
  counts from nine and zero to eleven and two. The promoted index in
  `call_enemy`, described below, subsequently restores both pairs. Moving
  the counter initialization before
  the slot lookup, restoring an outer count guard with a `do` loop, or changing
  only the ship argument leaves 79 differing instructions. Reordering the
  count and pointer declarations does not improve the retained loop. A
  conditional expression for the afterburner deadline emits the same reversed
  comparison and is not retained.
- `call_enemy` keeps the short loop counter and its promoted array index
  as separate locals, declaring the int index first. Sign-extend the counter
  once at the top of the loop and use that index for all four object arrays.
  These lifetimes recover the original DI counter, ESI index, and argument
  load, matching the complete 112-byte span from nine differing encodings.
  Declaring the counter first leaves those nine differences. The unit-wide
  allocation also restores `your_afterburner`'s exact match and reduces
  `replenish_shields` from eleven differing encodings to nine. `fire_turrets`
  rises from 96.52% to 97.39% mnemonic similarity but still differs in 107
  of 115 instruction encodings; the other ship results are unchanged.
- `landing` keeps the damage-level table byte offset in its four-byte frame,
  reloading it for the sound hooks, canopy table, and final comment. A volatile
  offset local restores that storage and frees EBP for the actor shape. The
  canopy phase has its own short counter and scoped frame-table pointer;
  this gives the counter SI and the pointer EDI. Initialize that counter in
  the `for` statement after the sound call and frame-skip assignment. An
  earlier initialization hoists the clear and table load ahead of the
  coordinate stores. These choices recover an exact 1,248-byte function,
  including both switch tables, from 95.27% and a sixteen-byte shortage.
  The other eleven functions in the unit are unchanged. The qualifier and
  scopes reproduce the observed storage and lifetimes; their original
  source spelling is unknown. Scoping only the canopy pointer still leaves
  fourteen differing instructions; a separate counter reduces that to eight,
  and its later initialization removes the remaining eight.
- `SetFleetOverviewView` leaves its player-range local uninitialized on entry
  to the descending ship loop. CL 4.20 overlays that local with the X field of
  the initialized orientation vector, at `[ESP+0x1C]` after the four register
  saves. The generated listing assigns both symbols the same stack offset,
  and the original initializes and reads that exact slot. The former explicit
  `0xFF` initialization replaced the read with an immediate and exchanged the
  orientation and offset vectors' stack slots. Keep the native uninitialized
  local; SDL explicitly reads `orientation.x` to preserve the observed fallback
  without depending on compiler slot reuse. Initialize the single-object
  player range before copying it into the maximum range. These choices retain
  the original 36-byte frame and 752-byte span, reducing differing instruction
  encodings from 174 to two: the maximum-range comparison and branch have
  reversed operands and sense. Reversing the source comparison or assigning
  the measured range inside it does not fix them; a ternary introduces extra
  moves. Explicit qualified or volatile vector reads recover the memory read
  but leave the wrong slots. Declaration order, a narrower offset-vector
  scope, and a descriptive vector rename do not repair those slots either.
- `set_eye_direction_and_position` puts the large roll correction first,
  testing the absolute error against four with `>`. Camera modes five and six
  likewise put their distant-object paths first, testing against `0x7D000`
  and `0xFA00`. Early returns for the near paths instead exchange the blocks
  and change branch lengths. When mode seven has no target, handle the normal
  cockpit with an early return before the cockpitless viewport restoration.
  Restoring these four branch layouts recovers the complete 3,264-byte span,
  including its sixteen-entry switch table, with the original 60-byte frame.
  The function is exact, up from 89.68% and 584 differing instruction
  encodings, without changing any other space routine's result.
- `auto_pilot_sequence` initializes its formation slot after copying the
  destination, matching the original copy and flag-store scheduling. Cache
  each participating ship's short cruise speed before comparing and copying
  it. The final travel-mode switch already rejects values outside one through
  three; an extra `travelMode[ship] >= 1` guard adds a byte-width comparison
  absent from the original. Removing it restores the 1,472-byte span and
  retains the 44-byte frame. Similarity rises from 96.91% to 98.81%, with
  fifteen differing instruction encodings out of 419. The other autopilot
  rows are unchanged. Scoped nearest-range locals, explicit promoted ship
  indices, reversed wingman comparisons, and an inlined initial memcpy do
  not recover the remaining BP/BX allocation and scheduling differences.
- The three changing-mode calls to `initialize_cockpit` pass an incremented
  signed byte promoted to int and then decremented at 32-bit width. Use an
  int-facing prototype and narrow to a signed-byte local in the callee after
  the message-closing calls. Narrowing before those calls hoists its stack
  read; narrowing after them retains the callee's prior result. This is a
  reconstruction consistent with both sides, not proof of the original
  prototype spelling. In `RunSpaceFlight`, write the increment and the
  minus-one argument separately: a post-increment argument loses the
  original promotion/subtraction sequence in this context. Copy the idle
  timestamp to the render timestamp before the simulation timestamp, and
  store input ticks before total ticks. The flight loop retains its 912-byte
  span and improves from 95.11% / 83 differing encodings to 96.89% / 77.
  The shared parameter correction also raises `HandleSpaceFlightControls`
  to 96.32%; its differing encodings rise from 775 to 781 as later offsets
  change, while its 3,184-byte span remains correct.
  Explicit qualified timestamp captures improve the flight loop's mnemonic
  score but disturb the following cursor routine and leave the profile
  registers wrong. Declaration permutations, a separate present-tick local,
  and separate delta calculations do not yield a better complete-unit
  result. These temporaries are not retained; the cursor routine stays exact.
- `HandleFleetOverviewInput` saves the signed key byte in a short before
  dispatching. The original sign-extends the byte into CX, then CX into EAX
  for the switch. Keep that switch inside the `nCameraViewMode == 8` block
  and the final zero return outside it. An early return for other camera
  modes places a short branch and return before the switch instead of the
  original near branch to the final return. The word local and enclosing
  condition recover the entire 352-byte span, including both dispatch
  tables, from 94.03% similarity and 65 differing instructions. Every other
  main-unit result is unchanged.
- `start_dust` adds the forward offset with the origin first, but its right
  and up additions pass the offset first. Preserve the observed argument
  order even though vector addition is commutative. Shift the short streak
  accumulator before the final random call, add that result to the same
  local, and then store it. A combined expression instead accumulates in AX
  and moves the DI restore before the store. These choices recover an exact
  288-byte span from twelve differing instruction encodings.
- `house_keep_objects` shares one zero-counter guard between the destruction
  path and the warp-maneuver dispatch. The destruction path exits the outer
  object-class switch; the two warp maneuvers use an inner switch. Repeating
  the counter condition loads it into AX, displacing the original wingman
  load, while an if-chain changes the maneuver comparisons. The shared guard
  and switch recover the complete 1,008-byte span exactly. A separate ticks
  or maneuver local changes allocation and is not retained.
- `accelerate_and_move_object` dispatches engine-cut and drift-stop maneuvers
  with a switch. Its alert test reads only AX, so narrow the `alert_flag`
  result to short at this call site. Compute the afterburner velocity
  magnitude before passing it to `ScaleFixedVector`, and likewise save the
  final clamped acceleration before the later scale call. Direct argument
  expressions prepare vector addresses too early. Reusing the existing
  magnitude local for the desired afterburner velocity recovers all 235
  mnemonics in the original 816-byte span and 32-byte frame, with only four
  register differences in the afterburner timer decrement, down from 226
  differing instruction encodings. A separate block-local afterburner speed
  makes this routine exact but enlarges `object_collision` by sixteen bytes,
  so it is not retained. Combining the timer decrement and condition leaves
  the four differences unchanged.
- `object_intelligence` tests a signed remainder of `nCurrentSpaceFrameValue` divided by
  four and narrows that result before the zero test. An absolute value and
  bit mask omit the original sign restoration and word test. Compare the
  canned-sequence class against `OBJECT_CLASS_SHIP` with `>=`, and promote
  the signed-byte missile target into a short before comparing or passing it.
  These choices recover all 123 mnemonics in the original 400-byte span.
  The routine is exact in the earlier movement context; the recovered engine
  switch leaves one reversed camera-object comparison, down from 78 differing
  instruction encodings. Reversing its source operands or nesting the frame
  and camera guards does not correct it. A volatile-qualified camera read
  worsens scheduling and adds sixteen bytes; that probe is also reverted.
- `update_star_field` skips directly to the loop increment when a selected
  star has no yaw or pitch rotation. Put that `continue` inside the star
  case instead of combining the rotation test with its class and random
  tests. The combined condition moves the star body behind the loop footer
  and shortens its branches. In the forward-dust path, assign the first
  random distance, add the eye's collision radius, double the same local,
  then add the second random distance. CL folds the last two operations
  into the original `LEA` while retaining the earlier accumulator in EDI.
  Together these choices recover the original 784-byte span and an exact
  comparison, from 91.32% and a sixteen-byte shortage. The unchanged
  `hit_asteroid` then exchanges its asteroid and fragment-count registers,
  leaving nine differing instructions in its unchanged 160-byte span.
  Splitting the fragment-count assignment, using an unsigned counter, and
  scoping that counter inside the destruction branch do not correct this
  collateral allocation change; those probes are reverted.
- `show_damage_disp` narrows the space-or-plural suffix to a signed byte
  before passing it to `sprintf`. This reproduces the byte `SBB`, `AND`,
  and `ADD` followed by sign extension. Its sprite arguments also reload
  the just-stored X origin from memory. A qualified read preserves that
  access without changing the shared declaration; it does not establish
  the original declaration's qualifiers. These changes recover the
  368-byte span and raise similarity from 90.82% to 95.92%, leaving seven
  differing instructions out of 98 in coordinate-load scheduling and
  register use. Separate coordinate locals do not improve those seven
  differences and disturb the otherwise exact `UpdateDamageDisplay`.
  The retained reload also removes this routine's missing-X-read diagnostic
  from the global-access report.
  Qualifying the X store instead gives the same seven differences, while
  reversing the coordinate additions without a qualified access still
  omits the reload. These alternatives are not retained.
  Indexing the origin table directly with `cCurrentCockpitView` also omits the
  reload. A qualified Y-coordinate read into the separate locals leaves
  the same seven differences and the following routine's six register
  differences; both further probes are reverted.
- `ShowCarrierLaunchSequence` zero-extends the unsigned scale word but
  performs a signed division after shifting it. Promote the word to a
  signed int before the shift. The second animation phase passes an
  advancing fade level to its empty palette hook: the original increments
  BP by four each frame, rather than always passing zero. The final phase
  tests its frame limit at the loop head and leaves on Escape at the tail.
  Restoring these operations recovers the 2,064-byte span and raises
  similarity from 93.65% to 95.98%. The local frame is still four bytes
  short, and 337 of 520 instructions differ in bytes, including stack
  slots, register assignments, and relative branch displacements. An int
  fade local moves its sign extension after the update; narrowing before
  the addition moves the extension back but widens the add. A long
  intermediate with a short local leaves the retained output unchanged.
  Those alternatives are reverted; the short local is retained.
  Retaining a separate integer object index for the array accesses likewise
  leaves the frame and all 337 byte differences unchanged, so that probe is
  also reverted.
- `FormatTextTokens` uses the VC4.20 `va_list` and `va_arg` macros. Its native
  short and unsigned-short arguments consume four-byte slots while loading
  only the original halfwords. Treating the cursor as an `unsigned long *`
  instead changes argument-load order and width throughout the formatter.
  The outer character dispatch is a `switch`: CL promotes the character
  before comparing it with `%`, whereas an `if` keeps a short comparison.
  Together these choices produce an exact function comparison, including
  both dispatch tables and the original 64-byte stack frame. SDL continues
  to use its platform's `va_list` with default argument promotions.
- `Title_Sequence` dispatches input events with a switch. Mouse activation
  enters the shared selection path directly; the keyboard cases initialize
  their activation flag, handle the key, and fall through only when requested.
  An if-chain instead extends the flag's lifetime and spills the alternate
  menu shape. The switch restores the original 28-byte frame and 1,856-byte
  aligned span. A real integer local for the selected menu index also restores
  its signed-byte promotion before the bounds tests and the keyboard-switch
  registers. Together these changes raise similarity from 92.39% to 97.25%,
  leaving 74 differing instructions out of 472. Selection branch placement
  and mouse-coordinate argument loads still differ. The original title call
  sign-extends both coordinates, whereas the original barracks, training-menu,
  and recreation-room calls pass dwords whose low words hold the coordinates;
  do not widen the shared short-argument prototype from the title call alone.
  Reversing the bounds condition and spelling its invalid path with a shared
  label leave the same output, so those probes are not retained.
- `InitializeConstellationField` saves the viewport height in its four-byte
  frame, at `[ESP+0x12]` after the register saves. The star loop loads that
  word into DI once; the particle loop reloads it for each Y-position call.
  A volatile saved height and a separate function-scope `starHeight` recover
  those lifetimes, including BX for the star counter and DI for the particle
  counter. Retain a byte offset into the particle records, while accessing
  their members through `ConstellationParticle` fields. An element index
  instead omits the original `LEA EBX,[EAX*8]` and changes the stores.
  Promote density into one integer `population`, use it for the star count,
  then multiply that same local by sixteen for the particle count. This
  schedules `SHL ECX,4` before the star-count division, as in the original;
  independent product locals and a register hint do not. The 368-byte span
  is unchanged, but similarity rises from 84.54% to 98.97% and differing
  instructions fall from 89 to two of 97. Only the closing star-loop
  comparison and branch remain reversed. These source choices reproduce
  allocation; the original qualifiers and local declarations are unknown.
  Keeping `starHeight` inside the guarded loop exchanges BX and DI. Direct
  array indexing removes the explicit offset but exchanges the particle
  loop's registers. Neither variant is retained.
  The unit change also exchanges the height and width calculations in
  `CalcRectangleArea`, changing its previously exact result to four differing
  instructions out of fourteen. Its source, size, and arithmetic are unchanged.
  Reordering its product or assignments, splitting the arithmetic into
  compound assignments, returning the dimension expressions directly,
  renaming its parameter, making width unsigned, and adding one before
  subtracting the opposite edge do not restore the original allocation.
  Volatile bounds reads preserve height-first ordering
  but exchange AX and DX, increasing the differences to six. Those area
  probes are not retained; the initializer removes 87 differences while the
  area helper gains four.
- `DrawConstellationField` retains a byte offset into its parallel velocity
  and frame word tables across the random-position calls. The original
  sign-extends the random short and saves twice that value with `LEA`; retaining
  an element index instead omits both scaled-offset instructions. Expressing
  the shared byte offset recovers the original 560-byte span and raises
  similarity from 96.38% to 99.28%, with only the star loop's closing comparison
  and conditional branch differing. Reversing that source comparison,
  spelling the loop as a guarded do/while, and using a volatile count read
  all leave those two instructions unchanged; these probes are not retained.
- `CalibrateJoystickInteractive` sets text alignment before testing the loaded
  font. Each sampled axis supplies both its 32-bit global and its 16-bit saved
  calibration word; chained assignments preserve the original load/store order.
  Its range divisors and four scale factors remain in registers until clamped.
  The zero-scale clamps increment in left, up, right, down order, followed by
  the global stores. Real locals for these values reproduce that sequence and
  retain the original 36-byte stack frame. Register allocation and the final
  bound calculations still differ; the aligned span remains sixteen bytes short.
- In `/Od` code, keep explicit pointer locals when the original stores them.
- Repeat identical literals at each use site; string pooling was disabled.
  Source switch-case order also determines literal order independently of
  the final code layout. In `process_player_input`, the SFX up/down cases
  precede music up/down. The flight-control music toggle precedes the SFX
  toggle. In `PacketLoad`, the compressed-data case precedes default even
  though CL schedules the default code first. These arrangements recover
  all separate literals without changing the existing function scores.
  CL also retains a diagnostic inside a folded-away `sizeof` check. The
  unreferenced main-module size diagnostic is reproduced this way, as a
  controlled compiler probe confirms; a named filler string is unnecessary.
  In `DIBcascade`, write the surface-lock failure branch before the success
  path. CL emits its second `" failed\n"` before `" successful\n"`, matching
  the original addresses. Reversing those source paths can preserve the
  normalized function match while swapping the literal order. The complete
  3,620-byte DirectDraw literal-pool check catches this difference.
  Distinguish literals from named arrays: `CreateMainWindow` references the
  same writable class-name array three times, plus a separate title array.
  Four independent literals reproduce the text but not the original storage.
  `WinMain` passes an empty literal to `main`, and `GetJoystickDevCaps` emits
  its failure diagnostic between the memory report and corruption strings.
  The complete 679-byte startup literal check exposed a former `"Vj"`
  spelling that preserved the function's normalized instruction match while
  changing the pointed-to bytes. Use the original image to check even short
  literals omitted by the extracted-string listing.
  Both low-memory branches in `LoadOriginFxDrivers` likewise reference one
  100-byte writable message array before its memory-reservation tables.
  Repeating the message at the call sites adds a duplicate to the literal
  pool and loses the original declaration order.
  `ReportOutOfMemoryAndExit` also uses this shared message; its former shorter
  literal both changed the original text and added unwanted literal storage.
- `flag_reached` writes its normal-arrival branch before its waiting branch
  in source. CL still schedules the waiting path first, but emits the arrival
  strings before the wait format, matching the cockpit literal pool. The
  final empty HUD string remains a use-site literal whose first byte is read;
  replacing it with an immediate zero omits original storage and a relocation.
- The target display's empty string, three separate `" %s"` formats, and
  other labels are use-site literals. Keeping named arrays introduces extra
  eight-byte alignment and separates them from their following display-mode
  messages. `DrawTargetRangeReadout` emits `"TOO FAR"`, `" m"`, and `"----- m"`
  in that order: test the visible-target case first, then the excessive range.
  Its numeric path scans the result of `itoa` with `strlen` and copies the
  suffix to the scratch-buffer base plus that length. The original `NOT ECX`
  and base-relative `LEA` distinguish this from `strcat`, which uses the
  scanner's ending pointer directly. Ordinary `strcpy` calls produce the
  shared eight-byte readout copies. This restores the original function size;
  the placement of the excessive-range branch remains different.
- Preserve diagnostic whitespace. The music wrappers' original literals
  include newlines, and `StopMusic` prints a trailing space before its stream
  stop and a separate newline afterwards. Normalized function comparisons
  alone did not expose these different strings; linked literal-span checks do.
- `RecRoom`'s two pilot-name formats each have a separate empty-string
  literal in the dead-pilot branch. They are not offsets into padded format
  arrays. `InitializeRoomMenu` reads the first byte of its own empty literal;
  spelling that expression directly preserves the original byte load under
  CL 4.20. The chalk-board KIA argument is emitted before its format string,
  as the original data order requires. The complete room-data gate checks
  these short literals and every repeated format without string fallback.
- `PrepareShapeRLEData` loads the first four bytes of the `"1.00"` literal
  into the shape buffer as a dword. Preserve that memory load and the literal's
  own terminator; an immediate version value would omit its storage and
  relocation. The graphics unit gate checks its position between the viewport
  and overflow diagnostics.
- `ReturnToBriefingLongShot` clears ten local flag bytes with ten explicit
  zero initializers. An empty-string initializer instead emits a separate
  literal and clears the trailing bytes through a different sequence. Its
  retained character pointer addresses the animation-phase field, after the
  random activation test, rather than the whole character record. Test the
  frame limit against 40 to retain the original `CMP`/`JL` pair. These forms
  restore the function's size and mnemonic sequence; some registers differ.
- `DebriefingEstablishingShot` reads the frame-skip counter into a short local,
  optionally replaces it with one on the last frame, then decrements and
  stores that local before testing it. Separate operations on the global
  keep an unnecessary constant in a saved register and add sixteen bytes to
  the aligned span. The retained short value reproduces the original sequence.
- `DrawMedals` keeps the medal row's base coordinate separately from the
  badge row coordinate, and wraps badges at `x >= 232`. Keeping that base
  in a short local restores the aligned function size. Its addition/load
  order and register allocation still differ; a separate increment followed
  by a copy lets CL coalesce the locals and loses the recovered size again.
- `ProcessMusicScriptCommand` returns zero when the track is disabled or
  commands are suppressed, and one after dispatch. Its stop-command and
  repeated-track exits leave EAX unspecified, so retain their bare `return`
  statements in the integer-returning C function. The only caller discards
  the result. Declaring the function `void` omitted the two explicit result
  instructions and shortened its aligned span by 16 bytes. The restored
  return type and paths produce an exact function comparison.
- `MapMusicTrackToStreamerCommand` retains separate return blocks for cases
  27/28 and 29/30 even though each pair returns the same value. Its default
  label enters the return after case 19's diagnostic. Grouping those cases
  or moving default to the end changes both the branch encoding and the
  switch table. Restoring these blocks recovers sixteen bytes and gives an
  exact comparison. In `gametrack`, place the damage-at-least-two branch
  before the conditional choice between tracks five and zero; its complete
  function then matches too.
- `IsPointWithinEyeViewCone` returns zero for a projection below `0x94`, then
  returns one. Separate returns produce the original `MOV CX,0`; a result
  variable initialized to zero instead produces `XOR CX,CX`. That one-byte
  difference crosses an alignment boundary. The return form recovers the
  original 160-byte span and gives an exact comparison. A direct Boolean
  return and an explicit result-variable `if`/`else` do not match.
- `set_background_objects_rotation` clears the projected vector's Z field
  before calculating X and Y. Its short angle takes a remainder by 360
  immediately after `ArcCos`, before the X-sign test. The original `CWD` and
  `IDIV CX` establish this operation even though the usual angle range makes
  it redundant. Both changes recover the missing sixteen bytes and produce
  an exact comparison.
- `LoadShapeSet` calls `FetchDiskPacketRetrying` separately in its default-file
  and descriptor-file branches. CL shares their call tail but preserves the
  distinct argument pushes, including the branch's section-word reload.
  Replacing the descriptor file in a local before one common call loses
  those instructions. Separate source calls restore the original 112-byte
  span and match exactly.
- `sort_viable_target_list` recovers its saved next-outer index by scoping
  the distance temporary inside the inner loop and assigning the live
  inner index before saving it. See the later target-sort note for the
  remaining stack-slot, register, and size differences. Natural nested
  `for` loops and reordered function-wide declarations did not recover the
  spill; extra stack storage must follow from real variables.
- Check neighboring functions after these small source changes too. The five
  exact recoveries above preserve every previously exact function, but
  `transform_objects_to_your_view` and `servicetrack` each gain one differing
  instruction at unchanged sizes and mnemonic scores;
  `new_space_music_changes` loses one. Restoring the view-cone result local
  recovers its neighbor's old allocation but loses the view-cone match, so
  that trial is not retained.
- Single-use console strings are literals: the constructor's font and spinner,
  and the formatted logger's newline. Their offsets alone also fit named arrays,
  but arrays impose eight-byte object alignment. Literal storage reproduces the
  four-byte alignment required immediately after the palette flag.
- `StepPaletteTransition` computes one quarter of the maximum component delta
  before filling its accumulator array, then divides the maximum again for the
  countdown. Keep the first result in a real local; division inside the fill
  loop repeats work absent from the original. Making the maximum private static
  storage does not reproduce this sequence with VC4.20.
- Initialize structure fields individually when the original does so.
  `CreateMainWindow` assigns every `WNDCLASSA` field; a preliminary `memset`
  adds a `REP STOSD` absent from the original. The field order also controls
  how CL schedules stores around `LoadIconA` and `GetStockObject`.
- Use typed fields and arrays. Keep offset-based field names until their intent
  is proven.
- Use the matching CRT routine when the binary calls or inlines it.
- Check the original section before adding `const` to a table. The hazard and
  explosion-debris tables occupy writable data even though their reconstructed
  users only read them; the barracks menu regions occupy read-only data.
- Preserve initialized storage even when every byte is zero. In CL 4.20 C,
  a tentative definition becomes a linker common symbol, while `= 0` or
  `= {0}` puts the variable in initialized data. The packet-reference table,
  input state, music flags, and DirectDraw pointers occupy initialized blocks
  in the original. Checking their values alone does not catch wrong storage.
  Do not infer this distinction from raw section size alone: trailing file
  padding can overlap the start of uninitialized storage.
- Uninitialized C++ definitions have different storage from C tentative
  definitions. The overlay pointer in `pilot.cpp` and the two private wave
  variables in `sound.c` naturally produce `.bss` in
  declaration order. Keeping them in the temporary C globals unit instead
  produces linker common symbols with different positions. Restore proven
  ownership and verify object storage as well as zero values; zero-filled
  memory by itself cannot establish a matching layout. The following private
  C damage-display variable needs `static` to retain its `.bss` storage,
  as do the music filename buffer and event-manager cursor save buffer.
  The latter two remain externally visible only in the SDL build, whose
  adapters share them through the SDL-only header. The original build keeps
  their definitions private and uninitialized in the owning C units.
- Put a global's address comment before its declaration or after its final
  semicolon. A comment inside a multiline initializer is recognized by the
  layout audit but missed by the linked-data comparison's simpler parser.
  Put it before the declaration when interior comments contain semicolons,
  as in `aObjectTypeData`; those can terminate the simpler parser's scan.
- Object display names are separate literals in the packed record initializers,
  including 26 distinct empty strings. The compiler emits the complete pool
  after the coordinate tables with four-byte literal alignment. The last empty
  literal ends the object; the following three zero bytes are linker alignment
  before navigation. A flattened character array incorrectly owns those bytes.
- Recover biased array references from storage alignment and indexing together.
  The funeral scene lookup's displacement is two bytes before its aligned
  array. `asFuneralSceneBySeries[currentSeries - 1]` emits the original word
  lookup while allowing the preceding particle array's four alignment bytes
  and the following flag's alignment to come from the compiler.
- `perform_maneuver` uses a switch for the three inactive-target cases, placing
  their comparisons before the case bodies. Its proximity test compares the
  global threshold against the saved range. Let the reroll byte promote to
  `int` when comparing the random result: a cast to `short` produces a word
  comparison instead of the original sign extension and dword comparison.
  These forms improve the routine from 83.65% to 97.12%; scheduling and
  registers in the radius calculation still differ.
- `DIBupdate` keeps `top` in its argument stack slot while width and height
  occupy EBX and EBP. A volatile qualification on the definition's `top`
  parameter preserves the original memory comparisons, clipped store, and
  repeated accesses. It retains the 116-byte frame and restores the original
  384-byte span, raising similarity from 84.11% to 100% of mnemonics without
  changing other function comparisons. The qualification reproduces observed
  accesses; it does not establish the original source spelling or change
  the function's calling convention or parameter type compatibility.
  The two row-offset products still load their operands in reverse order.
  One extra body byte shifts subsequent relative operands, leaving 45 of 107
  instructions different in the byte comparison. Reversing multiplication
  operands and splitting the products through a real offset local did not
  change that output. Qualifying the source pointer instead preserved the
  size but displaced the left bound and reduced similarity to 86.92%.
  Earlier register hints, reordered extent declarations, an explicitly
  guarded do/while loop, and reusing the right/bottom parameters as extents
  also failed to recover the original allocation. These trials are not retained.
- `DIBsetPalette` calculates its four-byte palette-cache offset before
  capturing the RGB pointer through a qualified volatile lvalue. Use that
  pointer for the component comparisons and stores, and retain the original
  index for the word palette and DirectDraw call. Together these lifetimes
  recover the original pointer load after the first two register saves,
  followed by the original DX/EBX/EDI comparison allocation. The complete
  272-byte span is exact, up from 85.33% and 51 differing encodings, with
  the original four-byte local frame. All other DIB rows remain unchanged.
  The qualification reproduces the observed read; it does not prove the
  original source spelling. An offset local alone, or a volatile index
  captured into an int local, did not recover the sequence and is not retained.
- `ReportSpaceFlightMaxFps` reads the stored rate before adding its argument:
  `FLD [fSpaceFlightFrameRate]` precedes `FADD [ESP+4]`. Capture the rate in
  a real float local through a qualified read, then add the argument through
  a second qualified read in the next statement. This preserves both reads
  in order without adding a stack slot and recovers the complete 144-byte
  span. Reversing the addition expression or qualifying only the global
  read leaves the two differing instructions unchanged. The retained form
  leaves every other DirectDraw function unchanged, including `DIBsetPalette`.
- `InitializeEventManagerResources` stores the fetched shape into the cursor,
  then clears its frame, then assigns its viewport. Qualified stores to all
  three fields preserve this observed order and recover the complete 80-byte
  span. Leaving the frame store unqualified still leaves two differing
  instructions. These qualifiers reproduce the assembly; they do not
  establish the original source spelling.
- `SeekPacketSection` captures the section's starting file offset in the
  existing result local before calculating its end. Reuse that captured
  offset in the origin switch and lower clamp, then overwrite the local with
  the seek result after the call. This restores the lower-bound comparison
  and branch at the original 112-byte span, reducing differing instructions
  from four to two and raising similarity to 97.73%. The upper clamp still
  reverses its comparison operands and branch sense. A separate start local
  or a conditional-expression upper clamp also disturbs later geometry
  functions. Materializing the upper comparison in the reused local swaps
  which clamp matches but still leaves two differences. These alternatives
  are not retained. Likewise, a `for` loop or a qualified capacity read does
  not recover `CollectActivePaletteIndices`' final comparison; the loop
  rewrite also disturbs other geometry functions and is reverted.
- Capturing `EraseCockpitReadoutRegion`'s bottom parameter through a qualified
  read restores its load before the top parameter, but leaves the comparison
  and branch reversed. It also disturbs `target_locking` and
  `place_damage_on_cockpit`, outweighing the two-instruction improvement.
  Qualifying the comparison directly, capturing the top instead, or writing
  the captured value back to the parameter is worse. These probes are
  reverted, as is a separate cockpit-light frame scope that leaves its
  target unchanged while disturbing the same neighboring functions.
- `LoadAnswerPromptAndResponse` saves the fetched packet in its existing
  four-byte stack slot, then reloads it for the response and prompt pointers.
  A volatile pointer declaration retains both reads; qualified lvalue casts
  on an ordinary local are still folded into the fetched register value by
  CL 4.20. Capture the scan pointer in the packet-assignment chain and use
  a counted `for` loop so its word counter is initialized between the disk
  prompt and packet-fetch calls. These choices retain the 160-byte span and
  raise similarity from 89.83% to 93.22%, reducing differing encodings from
  49 to 46 of 59. Scan/prompt/counter registers and some scheduling remain
  wrong. Reversing the assignment chain or pointer declarations does not
  improve that result. A separate doubled-entry index worsens both scheduling
  and comparison direction and is not retained. Other pilot-unit rows remain
  unchanged, including the compiler-generated construction cleanup.
- Keep direct array-field accesses when the original repeatedly uses an index
  plus an absolute field address. A cached structure pointer can produce the
  same mnemonics with shorter displacements, changing the function's size.
  `ConfigureScrambleActor`, `NavMapPointInsideReservedArea`, and
  `ReserveNavMapArea` recover their original sizes with direct field accesses.
- `InitializeCockpitResources` copies each VDU and pilot-hand rectangle as
  an eight-byte record. Four separate halfword assignments add instructions.
  Index the readout origins and hand bounds directly: cached pointers add
  address calculations absent from the original. Keep the scanner sizing
  call nested inside its allocation so the outer zero argument is pushed
  first. The final damage-background loop also pushes an extra zero before
  sizing, then cleans twelve bytes after `MaxShort`; its preceding sizing
  loops clean eight. `MaxShort` reads only its first two short arguments.
  A variadic declaration preserves both observed call forms without changing
  its exact body or calling convention; it does not establish the original
  header's spelling. These changes recover the original function size and
  mnemonic sequence. Seven instructions still differ in comparison operand
  order or weapon-loop registers. Reusing the frame index, moving the weapon
  local declaration, and reversing the source comparison did not resolve them.
- `draw_3d_scanner` retains one grid value across all four pixel calls but
  reloads the current cockpit's scanner coordinates at each call. Its grid
  counter is 32-bit, unlike the short object index. Initialize the counter
  before testing the first grid entry, and initialize the grid pointer only
  inside that guard. The target blink uses a short signed remainder by two
  and selects its colour in the draw-call argument; assigning a replacement
  colour first omits the original sign extension and branch sequence. This
  restores the function's size and mnemonics, with register and comparison
  operand differences still remaining.
- `vdu_polygon` indexes the cockpit-bar table by view and bar directly.
  Initialize its retained length word within the extent expression; loading
  it beforehand or caching the view index shortened the routine in earlier
  trials. The frame swap saves the empty frame first. Direct indexing
  recovers sixteen missing bytes, but the viewport bound reads and register
  allocation remain unresolved. Chained field assignments and reusing the
  percentage argument did not improve the match; copying the frame bytes
  with `memcpy` also enlarged the local frame and is not retained. Removing
  the length local leaves this function's instructions unchanged but alters
  register allocation in later mission functions. Check the entire unit
  when changing locals, not just the edited function.
  Declaring bottom before right improves the current match to 88.52%, but
  its aligned span remains 32 bytes short. Moving the right-bound store
  before the bottom-bound load does not change the generated instructions.
  The later correction declares the three frame bytes before the word
  locals and captures each bound with `local = stCockpitBar.field = table.field`.
  Keep that assignment direction: the global receives the table value before
  the retained local does. Together with qualified volatile updates of the
  bottom/right bounds and qualified top/left reads in the first comparisons,
  this recovers the original 464-byte span, four-byte local frame, saved
  registers, and frame-byte stack slots. It leaves four differing encodings
  at 98.36% mnemonic similarity: the divisor/index scheduling and exchanged
  EAX/EDX multiplication inputs. The other cockpit functions are unchanged.
  Reversing the chained assignment direction fails to recover this match.
  A wholly volatile viewport pointer instead
  produces 496 bytes and disturbs two other functions; it is not retained.
  On the corrected source, reversing the extent multiplication, loading the
  length separately, and explicitly promoting the percentage to int or long
  all leave the four remaining differences unchanged. Those probes are reverted.
- `fire_computer_graphic_missile` retains a signed-byte frame across the
  coordinate calculations. Refresh it after the VDU-mode query and after
  restoring the old background; use that value for background capture, then
  reload the global for drawing. These lifetimes reproduce the original
  frame reads and recover sixteen bytes, restoring the 336-byte span at
  85.87%. Explicit shape/background pointer locals and a promoted Y local
  did not improve the match and are not retained. Coordinate caching and
  an extra saved register remained different at that checkpoint.
  Qualified volatile accesses to both coordinate words now preserve the
  original reloads and direct memory ADD/SUB updates, removing the extra
  EBP save. The aligned span stays 336 bytes, mnemonic similarity reaches
  96.59%, and differing encodings fall from 82 to 58. The body comparison
  drops from sixty differences to ten; the rebuilt body is 335 bytes versus
  the original 329 because the shape-pointer load is duplicated across the
  restore branches. Qualifying Y alone instead produces a 352-byte span
  and retains the extra register save, so that intermediate form is discarded.
  These access qualifications constrain code generation; they do not prove
  the original globals were declared volatile.
  The layout audit's address-tail scanner interprets the compound assignment
  operators as pointer arithmetic and reports an adjacent-global dependency.
  This is reviewed at `REBUILT_LAYOUT_ADJACENT_SPLIT:0x005a7dbe` in the
  configuration: both images use word ADD/SUB at Y itself, and neither
  accesses the following type object through that address. The original and
  rebuilt global-span checks stay enabled.
- `target_locking` dispatches through a weapon-type switch, with the
  heat-seeking body before image recognition. Keep the countdown in a
  short local, increment it, and store it before returning. Accumulate the
  squared screen distance in a real int local in two steps, and declare
  the short Y coordinate before X. Together these choices recover the
  original branch order, register allocation, and complete 256-byte span
  exactly. Reversing a single sum expression or moving the countdown
  declaration alone did not reproduce the allocation; promoting Y into
  the distance local before squaring it was worse.
- `place_damage_on_cockpit` retains the selected damage-state pointer after
  its two mode guards. Each explosion branch assigns both the pending
  damage and its state. CL shares those assignments after testing the
  shape pointer, preserving the original flags through the two stores.
  This restores its exact match after changes earlier in the unit.
  Hoisting the assignments in source delays the shape test; saving a
  Boolean result instead adds arithmetic instructions and sixteen bytes.
- The cockpit changes leave `cycle_onscreen_targets` with the equivalent
  reversed comparison and branch, two differing instructions at unchanged
  size. Reversing its source comparison, moving the target test into the
  loop body, caching the target count, and reordering local declarations
  do not recover the original order; those trials are reverted.
  `draw_target_box` improves to 97.62% at unchanged size and differing-byte
  count, while `print_message_text` returns to 95.83% with its existing
  sixteen-byte excess. Neither function's source changed in this batch.
- `print_message_text` keeps its character limit as a short and initializes
  the two wrapping pointers after measuring the viewport. Test the character
  just written to the output buffer so CL retains the loaded byte instead
  of rereading the input after a potentially aliasing store. Preserve the
  original forward newline offset from the current output cursor. Index the
  message origins directly with `cCurrentCockpitView` in each draw call. Caching
  that view in an int changes how CL shares the argument setup between the
  switch cases and the normal cockpit path. The direct accesses
  originally recovered 95.83% mnemonic similarity, but pointer/index allocation
  left two extra body bytes and therefore a 16-byte larger aligned span.
  Explicit break/index locals and alternate pointer initialization alone did
  not remove that difference. The full-unit check also recovered the exact
  `build_your_target_list` without changing that function's source.
  Qualifying the read cursor as `const char *` likewise leaves the generated
  instructions unchanged and is not retained.
  The complete correction declares the numeric locals before the two cursor
  pointers. This retains input in ESI, output in EDI, and the short loop index
  in CX. Promote the index into a real int local after the space test, use it
  in the remainder expression, and subtract the last-space index from that
  same local before indexing the output. CL then reuses EBP for the subtraction,
  matching the original three-instruction newline path. An offset temporary
  confined to that path or an unchanged promoted local does not suffice.
  Remove the redundant width temporary and divide the returned short directly
  when assigning the short character limit. Keeping both the width and promoted
  index locals makes this routine exact but disturbs `place_damage_on_cockpit`;
  removing the width local preserves every other cockpit function. The final
  result is exact over all 143 instructions, the switch table, and padding:
  512 bytes with the original 220-byte local frame. Narrowing the newline offset
  to short also recovers the span but introduces different word operations,
  so that probe is rejected.
- `vid_transmit` tests the cockpitless-view flag both before and after its
  frame-parity test. Keeping that repeated condition preserves EBX and the
  second `TEST EBX,EBX`, restoring the missing sixteen-byte aligned block.
  Its background-selection branches each contain their own sprite draw call.
  CL merges those calls, placing the common zero argument before the branch
  and matching the subsequent coordinate loads and portrait draw. Assigning
  a temporary background pointer or using a conditional expression in one
  shared call instead changes argument scheduling. The repeated guard and
  separate source calls reproduce the entire 432-byte span exactly, up from
  91.06% similarity, without changing other function comparisons.
- `fire_afterburner` measures velocity before querying the maximum velocity.
  Save the measured speed in a real int local before the comparison: CL 4.20
  evaluates the two calls in the opposite order when both appear directly
  in the comparison, even if its operands are reversed. The explicit speed
  restores 30 of the original 32 instructions at the byte-comparison level.
  Only the equivalent reversed comparison and branch remain different.
  An early return, a named limit, and a long speed local did not resolve them.
- `get_right_shape` retains object class and type as short values and the
  selected frame as a signed byte. Read the type only in the non-missile
  branch. Its pitch rounding clamps the promoted increment or decrement
  before narrowing back to short, and tests remainders against 15. Direction
  selection uses seven switch cases; the out-of-range path leaves its local
  unset, as the original stack load shows. Do not add a default direction or
  an extra yaw-sector correction. Zero the projected vector's Z component
  before the dot products, and reload the saved capital-ship frame after the
  screen update before indexing packet references. These choices reproduce
  the original 84-byte stack frame and complete 960-byte function span,
  including four relocated switch targets, seven selector bytes, and padding.
- `object_collision` reloads ownership, object classes, and its own mass
  after calls; caching their values across those calls removes original reads.
  Five vector workspaces suffice: one holds the projectile force, separation,
  object component, and successive impulses; a second holds each object's
  tangent in turn. The other three retain relative velocity, the partner's
  component, and their difference. Distinct projectile and collision damage
  words reproduce the observed local lifetimes. Keep the no-collision update
  in the final `else`, and nest the response limits as `MinInt(MaxInt(...), ...)`.
  These choices recover most of the routine. Its retained frame is 92 bytes,
  matching the original; other unit contexts increase it to 96 bytes because
  the compiler spills the mass pointer.
  An explicit mass pointer, a `register` qualifier, and a shared scale local
  did not recover the original allocation and are not retained.
  The ship/capital-ship collision sound call pushes four arguments, with
  two trailing zeros, and cleans sixteen bytes; its former three-argument
  reconstruction changed subsequent scheduling. Keep promoted int indices
  for the object and partner array accesses, while the calls retain their
  short arguments. Write the partner's collision latch first, calculate the
  first mass ratio before the nested clamp calls, and test missile grace
  against zero with `<= 0`. Together these recover all 607 instruction
  mnemonics, the original 92-byte frame, and the 1,968-byte span. Only the
  initial two index loads remain exchanged, down from 319 differing
  instruction encodings. Reordering their assignments, embedding the
  assignments in the vector-call arguments, or scoping the indices inside
  the collision branch does not correct those two instructions.
- Check argument widths at callers even when the callee ignores an argument.
  `GetTransformedShapeBounds` takes a short flip value: widening it to `int`
  adds a sign extension in `easy2see` that the original does not contain.
  The solid-colour sprite functions likewise take a short colour value even
  though `SetSolidColourTranslation` uses only its low byte. Their callers
  extend byte colours into AX before pushing them.
- All ten direct callers of `FlushSoundEffectsAndLog` pass two arguments
  and clean eight stack bytes. Keep its int sound handle and short fade
  argument in the declaration, even though the Win32 body ignores both
  and tail-calls `FlushSoundEffects`. Ordinary typed calls replace the
  former incompatible function-pointer casts. The afterburner-expiry path
  passes its current global value and zero; an argumentless call omits
  original instructions.
  `PlayScrambleHangarScene` and `scramble` read an unset four-byte local
  handle at the flush calls. Preserve that native stack read, with zero
  initialization only for SDL. Their fade arguments are ten and zero.
  The hangar scene also passes `0x1b` to the empty `SceneLeaveHook`.
  Restoring these arguments recovers the original four-byte frames and
  the complete 720-byte and 1,104-byte function spans, respectively.
- `AnimateScrambleWalk` retains the escape byte after decrementing the
  frame-skip counter and refreshes it after the draw calls, alongside the
  left walk-frame increment. Use that value for the exit test after the
  coordinate updates. Its lifetime makes CL update the right walker's X
  directly in memory, rather than keeping a third coordinate in a register.
  This removes sixteen excess bytes and restores the 624-byte span at
  98.74%. Load scheduling and register allocation still differ. A compound
  assignment or reordered coordinate updates did not help; an explicit
  left-coordinate local, background-coordinate local, or separate escape
  assignments in the draw/skip branches was worse and is not retained.
- `auto_position` tests the formation slot's low bit directly in its
  `650 : -650` conditional. Comparing the masked value with one instead
  produces a branch and byte operations. The direct condition emits the
  original word mask, `SBB`, mask, and addition, recovering sixteen bytes
  and the complete mnemonic sequence. Only the formation-count comparison
  has reversed operands. Reversing that source comparison or retaining a
  separate slot local did not change it. Selecting a sign and multiplying
  by 650 restores the size but leaves an extra multiplication.
- `InitializeModalTextPanel` takes an eight-byte `ShortRect` by value and
  three short colour arguments. The adjacent default-bound words are one
  rectangle, not two independent integer arguments. Its chained rectangle
  assignment produces an eight-byte compiler temporary; separate `memcpy`
  calls omit that temporary and shorten the function. The reconstructed
  panel and viewport retain flat fields, accessed as a `ShortRect` for this
  aggregate copy. `ShowModalTextPanel` saves the default rectangle before
  allocating the panel but passes the global rectangle to its first
  initialization, reloading its bounds after allocation.
- Write signed arithmetic at its source level when the instructions implement
  division. `ShowModalTextPanel`'s `width * 8 / 16` generates the original
  shift, `CDQ`, bias, and arithmetic shift. Manually expressing the bias lets
  CL simplify the sequence differently and changes register allocation.
- The disk-prompt bounds at `0x005A7D80` are likewise one `ShortRect`.
  `PromptInsertNumberedDisk` copies them into both viewport rectangles with
  two DWORD loads and four stores, then reloads them after selecting or
  allocating the background viewport. Splitting them into independent
  halfword stores adds code absent from the original. The initializer in
  `sound.c` writes the same rectangle's top, left, bottom, and right fields.
- Empty compatibility functions can still take arguments. The text-mode
  disk prompt pushes `2` and `10` for `ResetDiskPromptTimerHook`, then cleans
  eight stack bytes. Preserve these arguments even though the hook is a
  bare `RET`.
- Distinguish a message pointer from the literal it addresses.
  `ReportPacketLoadError` reloads three initialized pointers in `disk.c`.
  It restores `nPacketError` immediately after `LogMemoryUsage`, before
  selecting an operation label. Keeping its packet-size local before the
  error word also reproduces the original EDI/BP register assignment.
- `FetchDiskPacketRetrying` retains the file-table index across retries,
  but reloads `pDiskFileRecords` after every `FreePacketAndClear` call.
  Use the indexed field at each `PacketLoad` call; caching the name pointer
  removes those loads and shortens the function. Its initial packet-size
  comparison is signed (`JLE`), and retry exhaustion is `retries <= 0`
  (`TEST`/`JLE`), rather than a comparison against one. The first error call
  passes the flags word already held in a register, even though that path
  has established that its value is zero.
- `OpenPacketSection` keeps the directory-entry offset across its two reads,
  adding four before the second. Recomputing it from the section index omits
  a stack slot and changes the register lifetimes. Mask the entry offset
  once before the last-section branch, retain the compression word's `0xff`
  mask, and clear the next entry's high byte through its byte representation:
  the original uses `MOV BYTE`, not a full-width `AND`. These choices recover
  its 24-byte frame and 320-byte span at 99.05% similarity, with eighteen
  differing instructions out of 105. The file-size and section-end slots
  remain exchanged, and the section guard and final stores still differ.
  Reversing the guard's source branches added sixteen bytes and reduced the
  match to 80%; that trial is not retained.
- `DecompressPacketSection` tests the nonzero format-flags path first and
  keeps its fallback-allocation flag as a byte, matching `TEST BL,BL`.
  The caller-supplied-workspace path reads its size local independently into
  EDI and EBP after applying the input-size override. A volatile size local
  preserves both original reads; an ordinary local or chained assignment
  merges them into a load and register copy, shortening the aligned span by
  sixteen bytes. This is a matching declaration, not evidence of the original
  source spelling. The complete 752-byte span now has matching mnemonics,
  with twelve differing instructions out of 222: its packet and size slots
  remain exchanged, and two initialization stores are scheduled differently.
  Making the packet pointer or `OpenPacketSection`'s file size volatile did
  not change their code and is not retained. Reordering declarations, using
  DWORD header locals, and changing the scratch pointer's element type also
  failed to recover the remaining layout.
- The constant-zero callback at `0x004362B0` still takes the packet pointer:
  its sole caller pushes that value and cleans four stack bytes afterwards.
  `GetPacketDecompressionResult` describes its observed use, and the following
  empty callback is `FinishPacketDecompressionHook`; their former video-release
  names were unsupported. Both bodies remain exact. Keep the new callback's
  shared prototype argument unnamed: adding a parameter identifier changes
  CL 4.20 register allocation in unrelated compilation units. The declaration
  still supplies the complete pointer type, and its definition names the
  argument. Whole-project reports caught this otherwise invisible regression.
- Keyboard cursor movement passes signed coordinates to `QueueInputEvent`.
  The original sign-extends both short fields; unsigned casts generate
  extra instructions and change argument preparation. Keep the observed
  clamp of 320 for both coordinates, including the vertical coordinate.
- Preserve switch dispatch and shared action paths. `BarracksScreen` has
  jump tables for event types and menu regions, plus a key switch. Keyboard
  activation and mouse clicks reach the same flag assignment; cursor events
  and unhandled events go directly to the common redraw. Its frame-timer
  result is tested in AX even though the timer function returns a full int.
- `RecRoom` likewise uses switches for event types, keyboard activation, and
  menu regions. Keep its byte keyboard-click flag separate from the short
  room-refresh flag. Only the two pilot regions test their shape pointers;
  Shotglass enters the shared conversation directly. Recompute the campaign
  roster index after the first pilot's loading and formatting calls, and
  write through the base-label pointers. The second pilot's sentinel test
  deliberately still reads the first pilot's animation ID, as in the binary.
  Preserve the separate character-mask tests and the shape-pointer recheck
  after obtaining the first pilot's bounds; copy the resulting viewport
  rectangle unconditionally. Restart Shotglass's animation through a byte
  local, and advance each animation pointer inside its draw-call argument.
  Saving the frame first changes register scheduling. These source forms
  reproduce the complete 2,976-byte function span, including both switch
  tables, under the relocated function comparison.
- `ShowChalkBoard` initializes only the text context's two colour bytes,
  assigns its existing modal viewport, and installs the text buffer after
  loading the font. It neither clears the whole context nor copies the screen
  viewport, and its rank lookup has no clamp. Its exit flag is a byte set to
  zero before sorting. During sorting, retain the candidate pilot's short
  index but reload the current slot when swapping; retaining both indices
  extends register lifetimes and adds a spill. These changes recover the
  original function size and 98.51% mnemonic similarity. The sorting address
  calculations and context-initialization scheduling still differ; cached
  score expressions, reversed comparisons, and extra cached indices did not
  resolve them. Removing the viewport copy also removes the extra `stScreenView`
  read reported by the global-access audit.
- `CloseLook` shares one short counter between the scene-frame loop and
  the animation countdown. Initialize it before the finished flag, and
  assign both animation pointers before setting the frame-skip counter.
  CL schedules those assignments into the original entry sequence, including
  the zero registers and the constant-one load between the format arguments
  and their call. The animation-command switch's restart case must fall
  through to load the first frame and its delay after resetting the cursor;
  merely resetting the cursor skips an original operation. Increment the
  finished flag and narrow the timer result before testing it, matching
  `INC DI` and `TEST AX,AX`.
  Read the saved start pointer through a volatile-qualified lvalue at the
  restart. This retains its stack store and reload in the original four-byte
  frame; qualifying the whole local instead makes an eight-byte frame.
  The compiler listing assigns the saved pointer and the uninitialized frame
  word the same slot, reproducing the original overlapping stack accesses.
  The qualifier expresses the observed accesses, not a claim about original
  source spelling. Keep the forwarded unused argument as an int to retain
  its original dword load; the sole caller passes zero and the cdecl stack
  layout is unchanged. Together these choices recover all 704 bytes from
  86.00% and 165 differing instruction encodings. The shared prototype change
  leaves every other function's report unchanged. The original fourteen-
  character loop is retained despite the ten-record character array.
- `IsRoomMenuLabelEmpty` returns a byte: the original uses `SBB AL,AL`
  and `NEG AL`, and `RefreshRoomMenuLabel` tests AL after its call. Declare
  the predicate as returning `char`; its zero/one result does not establish
  signedness. Read `stCurrentRoomMenu.currentLabel` through a volatile-qualified
  pointer lvalue to produce the original direct memory comparison against
  one; the ordinary expression first loads EAX and compares that register.
  The predicate's complete 16-byte span and its caller's 48-byte span now
  match. This qualified access is a matching choice, not proof of original
  source spelling or asynchronous updates. Plain and unsigned byte results,
  explicit return branches, a named byte result, and logical negation did not
  remove the extra load without that access qualification.
  Qualifying the global pointer declaration also matches those functions,
  but binary-comp's declaration parser omits a pointer qualified after its
  asterisk, reducing the linked-data audit from 1,097 globals to 1,096.
  That declaration form is not retained. Qualifying only the predicate's
  read preserves the global's ordinary declaration and verification coverage.
- `DecodeShapeFrame` receives a full-width height argument and narrows its
  maximum coordinate locally. Its caller sign-extends the height before
  pushing it. Read the two signed coordinate words into short locals before
  promoting them, advancing the command pointer after each read. The row
  command is an ordinary unsigned short, not volatile; the original tests
  the loaded register without an intervening memory reload. Clipped copy
  lengths are signed shorts. Adjust the length before calculating the left
  skip, and advance the destination and X coordinate inside each encoded-run
  branch. CL merges the common tail while preserving separate run-length
  promotions. Initialize both maximum coordinates before the command pointer.
  Keep the encoded runs' clipped length and right edge in their shared
  encoded-row block. The raw-row branch has its own right edge and an inner
  clipped-length word. These independent lifetimes recover the original
  destination-offset register copy, the 640-byte span, and the encoded-run
  clipping registers. The current result is 97.96% mnemonic similarity with
  eight differing instructions out of 196, all in raw-row clipping. Giving
  each of the three copy paths separate length, right-edge, and skip locals
  instead reaches 98.98% similarity but differs in 93 instructions; the byte
  comparison, not that higher mnemonic score, decides which form to retain.
- `PrepareShapeRLEData` serializes the four short extents and four integer
  bounds with individual copies through an advancing byte pointer. Taking
  the bounds' addresses preserves their four stack slots. Copy the frame
  count and final prepared-shape pointer from their locals as well: direct
  typed stores lose the pointer's stack slot and change the offset-table
  clear from the original `REP STOSD` to a sequence with a byte remainder.
  The copies restore the 44-byte local frame and 656-byte function span at
  97.70% mnemonic similarity. Local slot ordering and register scheduling
  remain different. Reordering the prepared pointer's declaration did not
  help; assigning the initial cached-shape lookup to it adds an absent store.
  Removing the unused header pointer also changes allocation in the later
  `RestoreSpriteBackground` and `GetShapeFrameCorners` routines. Keep checking
  the complete unit rather than retaining an unused declaration to influence
  the compiler.
- `DrawFontGlyph` reloads the font through its text context after writing the
  palette translation entries and again when advancing the cursor. Retaining
  the initial font pointer across those writes omits original memory reads.
  On the viewport's top row, load the row-offset word once, test its high bit,
  and add its unsigned value only when that bit is clear. A direct memory-bit
  test followed by another offset read produces a different branch sequence.
  Keep the row and background initialization before the font-colour reads.
  Load the viewport pointer before those font-colour bytes as well. With the
  two following diagnostic functions now delimited separately, the original
  span is 512 bytes; the current rebuilt span is 528 bytes.
  Initialize the existing character-data pointer from the font before loading
  the viewport, and use it for the two initial font-colour reads. Reload it
  after writing the palette entries, as before. This retains the second
  font-colour byte in a register for the translation-table store.
  Each row loop now reads the viewport's row-offset table into a local pointer
  before its top-row test, matching the original shared field load. Together
  with the decoder's corrected scopes, this reduces the previous 544-byte
  span by sixteen bytes and preserves the palette-marker's exact match.
  Mnemonic similarity falls from 91.62% to 89.02%, but the byte comparator's
  differing-instruction count also falls, from 115 of 167 to 111 of 164.
  A single function-scope row-table pointer retains the excess 32 bytes;
  separate column locals worsen allocation. A volatile background byte
  restores the previous mnemonic score but introduces a different stack slot
  and more differing instructions. Those alternatives are not retained.
  Register allocation and scheduling remain different. Keep the font's
  split-byte offset in the source-pointer expression; a separate offset
  local also perturbs later functions. Reversing pointer-addition operands
  or adding a row register hint emitted the same code. Using the argument
  directly as the row reduced the frame below the original 24 bytes;
  reusing the source pointer for the character lookup introduced a spill.
  The later declaration-order correction puts row, column, and destination
  offset first. The destination pointer follows the character-data pointer;
  the viewport pointer follows the source pointer after the colour locals.
  Initialize the existing locals separately in their original order.
  This retains the source in EAX, the scaled row in ESI, and the viewport
  in ECX, recovering the original 512-byte span and keeping the 24-byte
  local frame. Similarity rises from 89.02% to 91.98%; the complete-span
  comparison has 61 differing encodings. Every other graphics function is
  unchanged. Separate pointer advances, a shared pixel temporary, const
  source pointers, long row indices, and reversed colour comparisons do not
  improve the previous form. A volatile background declaration or qualified
  background accesses also fail. Permuting only the colour declarations
  leaves the counter-first result unchanged; pointer declaration order is
  what recovers the final sixteen bytes.
- `GetTransformedShapeBounds` returns a short through an integer intersection
  flag. Its null-shape branch assigns the point-in-viewport comparison to that
  integer before returning; this emits full-width constant results, whereas
  direct short returns change the instruction width. Read all four signed
  frame extents before scaling. The first zero-coefficient corrections use
  increments; the second pair assigns one. Keep the transformed left edge as
  an int until the short comparison and output store: narrowing the whole
  calculation early replaces the original 32-bit subtraction sequence with
  word operations. These forms recover 87.25% mnemonic similarity at the
  original 432-byte span and eight-byte local frame. The remaining coefficient
  scheduling and temporary allocation did not improve with separate scaling
  statements, in-place span sums, accumulated products, or reuse of the Y
  argument for the top edge.
- `GetShapeFrameCorners` keeps its translated extents as ints until the
  short output stores. Four short locals instead replace the original
  sign extensions and 32-bit additions with word arithmetic, reducing
  similarity to 73.81%. Advance the shape pointer to its frame, then load
  each extent and add its corresponding origin in separate statements.
  This restores the first extent load to EAX and reduces differing encodings
  from 27 to 26 of 42 instructions, despite a mnemonic score decrease from
  93.02% to 90.70%. The 144-byte span and every other graphics-unit result
  remain unchanged. A shared origin local emits the former code. Moving the
  font source, destination, and translated-byte locals into narrower scopes
  does not recover `DrawFontGlyph`'s excess sixteen bytes and worsens its
  comparison; that separate probe is not retained.
- `CaptureSpriteBackground` and `RestoreSpriteBackground` also read both
  coordinate words into short locals before promotion. Keep separate command
  pointer advances for the two words. Both retain a separate run-end
  calculation before the horizontal-bound tests. The declaration and
  qualified-access forms below preserve the original aligned function sizes.
  The original
  spills the run length while retaining clipping values in registers; this
  allocation remains unresolved. Narrowing the run-length declaration's
  scope, adding a register hint to the skip, caching promoted bounds, and
  duplicating the cursor updates did not improve the match.
  Separating screen-pointer initialization from its offset addition also
  emits the same bytes, as do register hints on both clipping locals.
  Computing the run length before the command-type branch adds a byte copy
  and moves the row count into a register, worsening both functions. These
  trials are not retained.
  In capture, declare the existing count and run-length words before the
  pointer locals. This recovers the original row-address addition order.
  Inline the two count-word reads through `memcpy`, and assign the decoded
  run through a qualified volatile access inside the count subtraction.
  The assignment value feeds that subtraction without an extra reload.
  Qualify the run read used to calculate the right edge as well, retaining
  the original store/reload pattern. Keep the cursor advances in each
  encoded-run branch and retain one function-scope clipped-length local.
  Together these forms reduce capture's differing instruction encodings
  from 168 to 94 of 220 and raise mnemonic similarity from 84.55% to 91.82%,
  preserving its 752-byte span and forty-byte frame. All other 1,522 report
  rows remain unchanged. These qualified accesses constrain the generated
  memory operations; they do not establish the original source qualifiers.
  The run's stack slot and its retained BP copy still differ, as do clipping
  registers and some branch operands. Declaring the entire run volatile
  instead produces a thirty-six-byte frame and excess code. Separate
  clipped-length scopes also affect later graphics functions and give a
  worse combined result; neither alternative is retained.
  Restoration benefits from the same count declarations, inline word
  copies, chained qualified run store, qualified right-edge read, and
  branch-local cursor advances. With those forms, calculating both encoded
  and raw run ends before their horizontal tests improves the result over
  the previous assignments inside the second test. Its differing encodings
  fall from 173 to 109 of 224 and mnemonic similarity rises from 84.82% to
  91.52%, preserving the 768-byte span and forty-byte frame. The remaining
  1,522 report rows, including capture, are unchanged. Keeping common cursor
  advances or removing the qualified store adds sixteen bytes; separate
  clipped-length scopes disturb the later transformed-bounds routine.
  These alternatives are not retained. Run-length spill placement and
  clipping-register allocation remain unresolved in both routines.
- `rotate_about_j` multiplies the forward component by sine before multiplying
  the saved right component by cosine. After correcting graphics-workspace
  visibility, CL evaluates these addition terms in the opposite order.
  Swapping the source terms does not change its output. An explicit product
  local restores call order but reduces the stack frame from eight bytes to
  four and worsens the function-size mismatch, so it is not retained.
- `ReadTextInput` dispatches both the input byte and the input mode with
  switches. This retains the zero-extended key for character classification
  and places Enter/Escape and uppercase/numeric branches after their defaults.
  Save the text pointer before the viewport pointer, then copy the viewport
  through `pCurTextContext` after installing the input buffer. Using the
  cached viewport pointer omits an original reload. The key itself holds the
  translated character; no separate character temporary is needed.
- `UpdateSceneAnimationObject` returns a short and reads frame commands as
  signed bytes, promoting them only for the draw call. Cache the object's
  layer across each drawing loop, increment the byte stop flag, and let the
  `J` command fall through to `G`'s label search. Keeping an opcode local for
  a later `J` test adds a branch and changes the switch layout. Advance the
  label pointer separately over its command and value. The object-copy command
  narrows its scene-count product before adding the object index at full width;
  narrowing the final sum is different from the original. OR every goal result
  into the completion word. These choices restore the 12-byte local frame and
  1,552-byte span at 98.96% mnemonic similarity. Register allocation, initial
  scheduling, and one comparison operand order still differ. A register hint,
  moving stop initialization past cursor selection, and reversing the label
  comparison did not improve the match.
- `PlaySceneAnimation` writes the packet base into each object's script pointer
  before adding the signed script offset. Combining those writes omits an
  original store. Its three object loops test the object count before loading
  their countdown locals; guarded `do` loops reproduce this. Decrement a
  nonzero wait count before the completion branch, and test the timer result
  as a short. The complete 560-byte span now has the original instruction
  sequence; 15 instructions still differ in loop-register allocation.
- `SceneDirector` uses one record loop. When a conditional test selects another
  record, restart that loop before checking Escape; a nested source loop
  instead makes CL retain the shot and spill the duration, adding repeated
  loads. Initialize the record and prior-state locals before assigning the
  scene-type global. Use `abs` for the objective index, save the selected shot
  before clearing the face in the common cases, and update the cached text
  colour before looking up its palette word. Keep constellation initialization's
  shot assignment after its two global stores so that branch retains its own
  face reset. These choices restore the 1,136-byte span and all mnemonics;
  only two equality comparisons retain reversed operands. Register hints,
  declaration reordering, unsigned shot storage, and a label-based restart
  did not improve the result. The unit change also reverses two comparison/
  branch pairs in the previously exact `PanToScreen` (four of 183 instructions).
  Its source is unchanged: reversed source conditions and guarded palette loops
  did not restore the earlier encodings. Continue checking the entire unit.
  Binary-comp separately verifies all six switch tables in this director and
  the animation interpreter: 60 relocated targets, their HIGHLOW entries, and
  210 literal selector/padding bytes match at one address shift per function.
- Keep the observed form of record copies. In `LoadGame`, a byte source
  pointer and direct destination array fields retain the four original
  counted loops. Caching the destination base adds instructions; naming
  both arrays directly lets CL replace the loops with bulk copies. A byte
  pointer for the 17-byte description copy also preserves the interleaved
  loads and stores in the loader and both save writers. In the writers,
  advance the runtime source pointer before the disk destination pointer:
  reversing that order moves the destination increment ahead of its store
  and introduces an extra displacement byte.
- `LoadGameFromSlot` retains `LoadGame`'s result in a short, as its original
  `MOV BP,AX` and two `TEST BP,BP` instructions show. Declare that result
  before the save record to recover BP for the result and EBX for the campaign
  pointer. An int result leaves 70 differing encodings; a short declared
  after the record or inside the panel block leaves 23. The retained form
  matches the complete 336-byte span, including padding. Store
  `nCampaignDataSet` before `nQueuedCampaignIndex`, and use the dataset for
  both pilot-file lookups. The original stores target `0x005A8118` then
  `0x005A8114`, and the reload after the first packet call targets
  `0x005A8118`. Address normalization alone cannot validate that ordering.
- `SaveGame` instead sign-extends the write result into EDI. Its result local
  must be an int, and its final Boolean expression retains the original
  one-bit mask. Both save writers interleave each scalar campaign-field
  read with its corresponding disk-record store. Qualifying those reads
  and stores preserves that sequence; qualifying only one side does not.
  These are code-generation constraints, not proof of the original qualifier
  spelling. `SaveGame` retains its 576-byte span and 828-byte frame, improving
  from 84 differing instruction encodings to four. Those four are equivalent
  base/index register orders in the medal, badge, history, and ace-flag loops.
  Caching the source byte pointers worsens the result to 143 differences;
  reversing the subscript spelling leaves the four differences unchanged.
- The same field-transfer sequence reduces `CreateEmptySaveGameFile` from
  36 differing encodings to fourteen at its original 640-byte span and
  1,776-byte frame. Its remaining differences are in the objective-position
  copy. Explicit position pointers, a const objective pointer, and an inlined
  `memcpy` do not improve that copy. One pointer makes the later `SaveGame`
  exact but loses the loader's exact match; adding both position pointers
  also regresses `LoadGame` and `HandleBarracksBunkSelection`. Those probes
  are reverted. The retained three-function change preserves all other
  function-report results and the original empty writer's uninitialized fields.
- Calculate the bunk row before `DrawBarracksBunks` prepares its draw-call
  arguments. Keeping the division inside the argument expression occupies
  AX with the frame number and changes both coordinate indexing and push
  order. A short row local restores CX for the frame and the original size.
  Widening the row to int adds an unwanted sign extension after the division
  and increases differing encodings from two to 31; retain the short local.
- A shared call in the executable can come from calls in both source branches.
  In `intro_drawbackgroundships`, separate sprite calls let CL 4.20 prepare
  the common colour and frame arguments before selecting the shape. Selecting
  the shape first, including with a conditional argument, moves those pushes
  after the branch and does not reproduce the original instruction order.
- `your_internal_damage` likewise uses direct component-damage calls in
  each switch branch. CL merges their common call and stack cleanup while
  retaining the original constant pushes; assigning component and amount
  locals before a shared source label introduces register moves instead.
  Reject the wrong quadrant before handling drive damage, and use the
  `quadrant == 1 ? 3 : 1` selection for the alternate damage table. The
  first hull-damage event decrements the event count unconditionally before
  calling `pilot_hit`. The random weapon index is narrowed to signed char
  and then sign-extended to the short argument passed to `remove_weapon`.
- A typed loop filling a short array can compile to `REP STOSD` followed by
  `REP STOSW`. `Title_Sequence` uses this to mark unused menu options as -1;
  a byte-oriented `memset` produces a different remainder sequence.
- Preserve a shared sentinel write after a variable-length list is built.
  `process_player_input` writes one or two keys through an advancing pointer,
  then appends -1. Writing a separate sentinel in each switch case changes
  the original common tail.
- `player_input` saves the polled modifiers to their global before copying
  them to a local. Its later keyboard-event cases do not overwrite that
  global. Sign-extend the key for its switch, and retain the original nesting
  of the mouse-button fire checks, including the apparently contradictory
  tests after the previous-key check. The queued button case updates its
  timestamp after that conditional, on both paths. Calculate both mouse
  magnitudes once as signed shorts before their threshold loops; comparing
  promoted `abs` results instead emits different arithmetic and increases
  the frame. Handle the cockpitless branch first, switch on the signed view
  byte, and clamp at >=9/<=-9. These changes recover the 28-byte frame and
  restore the original 1,728-byte span at 100% mnemonic similarity.
  Qualified volatile lvalues retain the two separate key-mask updates and
  the conditional previous-key clear. Qualified reads keep the afterburner
  latch, mouse visibility, cockpitless flag, and view centre inside their
  event branches. These qualifiers express the observed accesses; they do
  not establish the original source spelling. In the joystick branch,
  store yaw before pitch. Capture the event X coordinate in an int before
  loading the left viewport bound for the mouse-edge checks.
  Two sample-X loads still use AX where the original reads EAX. Each adds
  an operand-size byte and shifts later instructions, leaving 227 differing
  encodings out of 397 despite the matching span and mnemonic sequence.
  Qualified destination-word stores recover the DWORD loads but disturb
  the initial input reads and shorten the span; they are not retained.
  Qualified sample reads, a volatile sample definition, LONG sample fields,
  explicit LONG casts, entry snapshots, and narrower mouse-local scopes do
  not repair the complete function. The input changes also exchange CX/DX
  in six `HandleFleetOverviewInput` instructions while making
  `ReportFramesSkipped` exact. A register hint on the fleet key does not
  restore its prior exact result and is not retained.
- `init_intelligence_data` takes its canned-sequence pointer from the
  existing 32-bit pilot-level slot before replacing that slot with 2. Reading
  it again from the mission record adds an index calculation absent from the
  original. The native build retains the full pointer from the mission record
  because its pointer cannot fit in the legacy slot. Test pilot levels >4
  before the default rating branch. The resulting 336-byte span matches the
  original length at 93.33% similarity; register choices and vector-copy
  scheduling still differ.
- `draw_nav_pointer` stores its object index and clears the old screen X and
  distance after the creation branch, including when the object already
  exists. The original thus discards the previous projection before every
  visibility check. Reload the global object index for the projected X, Y,
  and distance stores, and narrow the masked screen width to a short before
  shifting it. This restores its 512-byte span at 96.27% similarity. Creation
  scheduling and a missing saved register still change stack offsets and
  encodings; equal size is not an exact match.
- Signedness matters even for 32-bit fields. In `IxStream`, `pending` and
  `lockLen` are signed counts, while the consumed-byte delta is unsigned.
  The original tests `pending` with a signed comparison and updates it
  through a load/subtract/store sequence. Treating all these values as
  unsigned lets CL 4.20 replace that sequence with different memory arithmetic.
- An `/Od` format check can retain a jump through an empty default branch.
  `ix_dsps_alloc` has two such branches for its default eight-bit and mono
  formats. Omitting the `else` bodies removes ten original code bytes.
- Unused external globals in `ix.h` affect VC++ 4.20's `/Od` operand
  selection. `WC1_IX_DSPS_SOURCE` hides unrelated sound-system, allocator,
  mixer, and streamer state from the stream-control unit. Its five required
  globals remain visible. Voice declarations follow stream declarations for
  DSP users, while `WC1_IX_DSPV_SOURCE` preserves the voice owner's preceding
  declarations. These are matching header arrangements, not recovered
  original header spellings; function bodies, types, calling conventions,
  storage definitions, and flags are unchanged.
  The resulting full spans of `ix_dsp_set_master_volume` (141 bytes),
  `ix_dsp_free` (44), `ix_dsps_alloc` (694), `ix_dsps_play` (304),
  `ix_dsps_stop` (254), and `ix_dsps_set_volume` (261) compare exactly.
  The free wrapper's earlier instruction body was already exact; its final
  alignment now matches after the preceding master-volume correction.
  Across the ten stream routines, binary-comp's detailed differing-encoding
  count falls from 191 to 41. `ix_dsps_get_buffer_free` falls from 43 to eleven
  differences but becomes one byte short. The tradeoffs remain explicit:
  `ix_dsps_free` loses its exact match (seven differing encodings and one
  missing byte), and `ix_dsps_unlock` gains eight differing encodings in its
  guard. Both have reversed comparison operands and branch direction.
  The voice-control unit and every other report row outside DSP/stream
  control retain their previous results.
  Excluding only the system flag or sound globals, or additionally omitting
  streamer types and functions, produces different incomplete results.
  Removing redundant owner externs, renaming the stream index, and spelling
  the guards as negated conjunctions do not improve the retained output.
  Qualified reads do not fix the intended comparisons and can disturb later
  routines. A volatile release parameter makes the body eighteen bytes
  longer than the original; a subtraction-based range check adds instructions.
  Those probes are reverted.
  Follow-up probes with a register parameter, address-ordered stream
  declarations, and bare blocks around the fatal checks leave the comparison
  gaps unchanged. Wrapping every check in `do/while (0)` recovers the release
  routine but regresses later stream routines; wrapping only its first check
  also disturbs later output. An assert-style Boolean expression adds ten
  bytes per check. None of these guard-form probes is retained.
  Declaring `nStreamsAllocated` before `nStreamCount` specifically for the
  stream owner recovers `ix_dsps_prepare`'s original bounds comparison and
  complete 342-byte span. The other units retain their preceding declaration
  order. All 120 permutations of the five required stream globals yield
  four combinations of measured sizes and differing-encoding counts. The
  retained order reduces the ten routines' total from 41 to 34 while preserving
  every preceding exact match. The extra preparation byte consumes one byte
  of the unit's final alignment, reducing the unlock span's excess from four
  to three bytes; its eight differing encodings remain.
  A separate relocated-pointer check verifies all 86 operands referring to
  the five globals, including array-field offsets, across these ten routines.
  The six exact spans also retain every checked operand's function-relative
  position. This does not establish their absolute data layout.
  Consistently qualifying the indices as const, or renaming them, leaves the
  remaining differences unchanged. Putting the standard-library include
  before `ix.h` regresses allocation. Hiding unused DSP/system, streamer,
  and file declarations, individually and in combinations, can recover other
  guards but loses existing matches. Those further probes are discarded.
- Local names affect CL 4.20's `/Od` stack allocation. In
  `ix_mixer_thread_proc`, declaration reordering alone did not recover the
  frame. The descriptive names `primaryCaps`, `resultCode`, `bufferDescriptor`,
  `outputFormat`, and `soundCaps` place the real locals at EBP offsets -20,
  -24, -44, -60, and -156 respectively, matching the original. These names
  reproduce the allocation; they are not recovered original spellings.
  The former layout needed longer displacement encodings, adding 27 bytes.
  Binary-comp now reports the complete 1,180-byte function as exact, without
  changes to types, fields, flags, or instructions written by hand.
- `ix_mixer_service` has separate initial-fill and refill-loop locals. Keep
  the initial buffer, byte count, result, and delay outside the loop, with
  the two refill buffers, counts, cursor positions, buffered-byte count,
  result, and requested-byte count inside it. The request is assigned `0x16f8`
  before the lock call. The two success-result writes before and after the
  loop are absent from the original. The refill locals need separate scopes
  too: second-byte count and play cursor at EBP -20/-24; result, hardware
  write cursor, and buffered-byte count at -28/-32/-36; first-byte count and
  first-buffer pointer at -40/-44; and requested-byte count and second-buffer
  pointer at -48/-52. In that innermost scope, declare the pointer before
  the requested count. Keep the cursor query and delay calculation before
  the buffer scopes, and write the cursor-wrap test as
  `playCursor > dwMixerWriteOffset`. Reversing that test introduces an extra
  instruction byte and changes subsequent branch offsets. The scopes recover
  allocation; they do not prove the original block structure.
  Declaring `dwMixerBufferSize` and then `dwMixerWriteOffset` first in the
  local extern block recovers the final wrap check: load the size, compare
  the offset, and branch with `JB`. Both functions now have normalized exact
  spans, including all 262 service instructions, its 52-byte frame, and its
  932-byte span. The declaration arrangement matches the generated output;
  it does not establish the original header spelling or storage order.
  Merely reversing the two externs in their former positions fixes that
  final comparison but disturbs the earlier cursor comparison. Changing
  the buffer-size type, adding qualified reads, and reversing or negating
  the source comparison do not recover both. Alternative descriptive local
  names, a signed cursor, and flattening the refill scopes also fail; those
  probes are not retained.
  Removing the unused thread/window externs leaves the thread routine's
  normalized exact result unchanged while exchanging its primary and mixer
  buffer operands. This probe is discarded. `verify-mixer-references` now
  requires the original identities and function-relative positions of all
  79 operands referring to ten named globals across these two functions.
  Independent operand-swap probes on temporary executables still score
  normalized exact but fail that gate. Address normalization alone cannot
  establish reference identity.
- Preserve the observed order of calls inside comparisons. The audio-service
  loops call `ix_thread_get_audio_chunk_size` before `ix_dsps_get_buffer_free`;
  putting the chunk-size call on the left of `<` reproduces the original
  call order and unsigned branch direction under the library profile.
- The message pump retains a `POINT` cursor position alongside its `MSG` and
  `RECT`, giving a 52-byte local frame. Two independent cursor integers let
  CL overlay their uninitialized slots and shorten the frame. Its minimized
  flag is read again immediately after being set; `volatile int` preserves
  those accesses and the original loop branch instead of sinking the store.
  The minimized-state test follows the `IsIconic` branch, and the minimized
  process priority is `IDLE_PRIORITY_CLASS` (`0x40`).
- Keep the full-width arguments passed by `MainWindowProc` to
  `QueueInputEvent`. Its key value is the original `WPARAM`; its mouse
  coordinates have already been extracted from the message's low/high words.
  Additional short casts introduce zero extensions absent from the original.
  A separate `HANDLE` result from `GetCurrentProcess` also preserves the
  original call-before-priority-push order in the window procedure and pump.
- Both joystick pollers shift the full 32-bit button sample before narrowing
  the second button's mask to a word. Narrowing first emits `SHR AX` instead
  of `SHR EAX`. Store the host secondary-button byte before the primary byte.
  `PollMenuInputDevices` also writes each selected coordinate bound on both
  paths: conditional expressions assigned back to the fields reproduce these
  stores; conditional stores do not. Its change flag is sign-extended from a
  byte for the queued event. These forms recover its 672-byte span at 97.35%
  mnemonic similarity; `PollJoystickButtonEvents` reaches 96.08%. The menu's
  initial zero register, movement temporary, and cursor-copy scheduling remain
  different. A separate horizontal-step local and reordered cursor assignments
  did not improve them and are not retained.
- `LoadJoystickCalibrationFile` keeps both promoted input ranges in 32-bit
  locals through the scale divisions and coordinate bounds. This recovers the
  original register pressure and scale reloads. Choose the adjusted calibration
  bounds in an `if`/`else`, rather than writing the centres first, and increment
  each zero scale. Together these choices restore the 784-byte span at 98.35%
  similarity. The only call to `SampleBothJoysticks` pushes signed -1, while
  the callee reads its fallback as a word. A signed-short fallback parameter
  reproduces both; the callee remains exact. Its final conversion to the
  unsigned fallback of `GetJoystickPosition` preserves the low 16 bits.
- `ReadCalibratedJoystick` reads Y inside the short-circuit failure condition,
  after X passes its check. Chain the three sample-field clears to retain the
  original shared zero register. Capture the device, failure sentinel, minimum
  X bound, and axis centres through qualified volatile reads. Compare the
  cached X sample with the minimum before publishing the clamped sample;
  the bound load precedes the raw-coordinate stores. Declare raw Y before
  raw X, then subtract the centre and divide before narrowing each result.
  These choices recover the original 384-byte span without a local stack
  frame, reaching 96.69% with 71 differing encodings out of 121. The other
  calibration-unit rows remain unchanged. The four scale divisions still
  use memory operands instead of the original register divisors. A separate
  qualified divisor capture, a register hint, long divisor/raw-value locals,
  and alternative division expressions do not fix them. Ordinary unqualified
  sentinel/centre captures are folded back into memory operands. No claim
  about original volatile declarations follows from these matching choices.
- Check neighbouring functions after input changes. The unit's allocation
  changes also affect `ReserveContiguousPaletteEntries`. Reusing its count for
  the fill loop restores the original branch direction and all mnemonics, but
  nine of 39 instructions now differ at byte level, versus three before these
  input changes. Initial zeroing and fill-loop registers remain different;
  restoring the menu's cached index did not resolve them. No previously exact
  function loses its exact match in this change.

- Startup's animation-demo index and mission index occupy distinct word
  slots. The `w` option writes the former; the `m` option writes the latter.
  Keep an argument-slot pointer through the option switch: the original
  reloads the string pointer after `strcmp` and the version-print call.
  The normal campaign loop falls through before the direct-mission path.
  These choices recover the 20-byte frame and raise `main` from 82.95% to
  96.62%; sixteen excess bytes and allocation differences remain.
  The subsequent pointer-lifetime recovery removes those sixteen bytes.
  Capture the current argument after the `Origin` comparison, then use a
  separate option pointer for the `-` and arcade branches. Reload that option
  after the version-print call, and retain it across flag stores. The other
  mission options use the captured argument instead of reloading the slot.
  Capture the two campaign values as signed bytes after the timing call and
  qualify their five destination stores to preserve the observed order.
  The complete 1,264-byte span now matches in size, at 98.86% similarity and
  six differing instructions. The final argument comparison is reversed,
  and the `0x13` constant load still precedes the timing call instead of
  following the two byte loads. A guarded do/while loop, reversed source
  comparison, qualified count read, and an explicit video-mode local do not
  fix these remaining differences. Reusing the short argument locals for
  the campaign bytes adds unwanted sign extensions; retain byte locals.
  At that checkpoint, sharing one option local between both switch branches
  leaves `main` unchanged but loses the newly exact `ReleaseStaleNavTarget`,
  so separate scopes are retained. `SelectNextExternalViewObject` also becomes exact.
  The added startup locals initially disturb `UpdateTrainSimMenuCursor`.
  Testing `IsPointInRect`'s result directly, without the saved condition
  local, restores its original allocation and complete 96-byte match.
  Declaration initializers, reordered locals, and a for-loop spelling did
  not restore it. All other main-unit report rows remain unchanged.
  Recovering startup's distinct DOS interrupt callback subsequently changes
  this allocation context again. A shared option pointer now retains both
  branch lifetimes and restores the cursor's exact match while preserving
  every other main-unit report result, including `ReleaseStaleNavTarget`.
  Restoring a saved cursor condition, adding declaration initializers, and
  spelling its loop as a guarded do/while or for loop did not recover the
  original allocation in this context; none of those cursor probes remains.
- The recovered DOS callback also changes `shrink_vector`'s allocation.
  Expressing the three shrink calls as a bitwise-OR loop condition preserves
  their original X/Y/Z call order and restores the previous 64-byte span,
  mnemonic score, and nine differing instruction encodings. Explicit
  component-pointer locals leave ten or eleven differences and change the
  prologue scheduling; those probes are not retained. The callback's unused
  prototype is scoped to its actual caller rather than exposed before every
  event-manager function, preserving `ReleaseInputEvent`'s allocation.
- `UpdateDamageDisplay` saves the prior component count in a word local
  before counting damaged components. Comparing the promoted new byte count
  with that saved word reproduces the original `MOVSX`/word comparison and
  ESI save. Comparing the two byte values directly loses those instructions.
  Store the new count before the comparison, then store it again on the
  unchanged-count path. Putting the first store inside the changed-count
  branch lets CL merge the two stores and loses sixteen bytes. The counting
  loop has its own signed-byte index, separate from the component selected
  for display later. This recovers the initial BL index and ECX constant
  without changing the later selection loop. The complete 1,072-byte span
  now compares exactly.
- `AllocateNearHeapBlockByFlags` reuses its descriptor pointer when examining
  the last block; those pointer lifetimes do not overlap. A separate last-block
  local changes common-subexpression handling, adds an alignment spill, and
  enlarges the local frame from four bytes to eight. Keep the masked block
  size signed and retain the raw size-and-flags value separately for the
  allocation-bit test. Declare the masked size before the raw value to recover
  their ECX and EDX allocation. These forms match the entire 464-byte span,
  including alignment. Reversing the heap-base/size sum, introducing explicit
  loaded-value locals in `GetNearHeapBlockSize`, and removing redundant casts
  in the end allocator did not resolve their remaining differences; those
  trials are not retained.
- `replenish_shields` promotes its short ship argument to a retained integer
  index after the damaged-power-plant guard. Use that index for the shield
  and object-type arrays. This recovers the original 176-byte span and the
  shield-update loop; nine instructions still differ in the prologue, guard,
  and initial argument promotion. Moving the shield-value locals
  to function scope worsens the loop allocation. A separate recharge divisor,
  reversed index/counter declarations, and a parameter `register` hint do not
  resolve the guard. These trials are not retained.
- After the sound-flush declaration correction, `DecodeShapeFrame` became
  sixteen bytes short, before the clipping-scope correction described above.
  Grouping the destination offset, separating the bitmap
  assignment from its addition, and retaining an explicit row offset do not
  recover the original extra register copy. Explicit promoted clipping bounds
  also leave this function unchanged and perturb later graphics functions.
  After the scope correction, a separate raw-row right-limit integer and
  reversing the row-length/X addition still leave its eight differing
  instructions unchanged. These trials are not retained.
- Shared declarations affect optimized CL 4.20 register allocation even in
  units that do not use those declarations. Removing damage's private tables
  and former named literals from `globals.h` changes unrelated functions:
  `DIBsetPalette` becomes exact, while `ComputeVectorDelta` exchanges its
  source and destination registers and loses its exact match. Check the
  complete report after correcting header visibility. The damage split gives
  57 newly exact functions and 55 formerly exact functions with differences;
  its verified data layout does not imply every code comparison improves.
- Correcting the sound-flush declaration likewise changes unrelated
  allocation: the complete report gains 27 exact functions and loses 16,
  including the previous target-lock and cockpit-damage matches. Four
  targeted routines recover their original sizes, but `DecodeShapeFrame`
  and `replenish_shields` become sixteen bytes short and
  `AllocateNearHeapBlockByFlags` becomes sixteen bytes long. The net result
  is eleven additional exact functions, one fewer size mismatch, and
  32 fewer bytes of total absolute size error. These remaining differences
  must be resolved in source; they are not reasons to omit original arguments.
- `AppendCommMenuChoice` compares existing choices through the global count,
  then saves that count's post-increment result in a 32-bit index for the two
  shared stores. This preserves the original branch-specific count loads and
  gives an exact 128-byte span. A short index copied before the shared stores
  let CL hoist its load across the reuse branch, removing seven instruction
  bytes and shortening the aligned span by sixteen. The earlier explicit
  `else`, shared-store label, and `register` trials did not fix that lifetime;
  keeping the post-increment at the index capture does. No other function's
  report changes with this correction.
- `prepare_mission` retains the player's mission index as a word through
  `load_ship`, `set_objects_data`, and `Set_up_ship_info`. Use that index for
  the mission-record fields rather than caching a record pointer and later
  reloading the global index. The initial-wingman loop reads only the low
  word of each record's pilot value, then sign-extends it for `is_alive`.
  A `short` local reproduces that load and both pilot-range comparisons.
  The carrier search writes its current index in the limit test as well as
  after incrementing it. Keeping both observed stores restores its 416-byte
  span at 96.23%, up from 85.98% and a sixteen-byte deficit. Separate player
  and loop indices preserve the other functions' allocation; reusing one
  local perturbed three neighbouring functions. Four scheduling differences
  remain, and fourteen instructions still differ at byte level.
- `free_ship` indexes `aObjectResourceSlotsArray` and `aObjectTypeData` directly
  instead of retaining pointers to their selected records. The original
  keeps the resource byte offset on its stack and the type byte offset in
  EDI, adding the array bases at each use. Direct typed subscripts reproduce
  this allocation and the reloads, restore sixteen bytes, and give an exact
  comparison for the complete 432-byte function.
- `BuildMap` likewise indexes mission objectives directly instead of
  retaining a record pointer. Hazard dispatch is a switch with asteroid
  and mine cases. Initialize the navigation-point pointer immediately
  after `SetTextContext` in source: CL schedules its load just before
  `InitializeTextContextFromFont`, matching the original. Initializing
  before text setup or in the loop header leaves scheduling differences.
  These choices recover 32 bytes and match the complete 928-byte span,
  including its switch tables and alignment.
- `Build_objective_list` loads the source's short index before clearing
  the destination flags, then dispatches its type with a switch: zero for
  navigation points and one through four for mission ships. Keep the flight-
  path append in each case and declare the type before the display-name
  pointer. These choices recover the original EDI type and EBX name pointer.
  Its final sentinel index sign-extends the signed objective count; an
  unsigned-byte cast is incorrect. All 448 bytes now match, from 85.04% and
  115 differing encodings, with the original twelve-byte frame. Loading the
  source index after the flag store leaves scheduling differences;
  declaring the name pointer first exchanges its register with the type.
- `find_next_ship_turn_slot` retains its promoted object index separately
  from the short collision-search counter. Declaring the int index first
  recovers ESI for the object arrays, reducing differing encodings from 20
  to 15 of 36 at the original 128-byte span. The short counter and object
  argument still exchange CX and DI, and the final comparison is reversed.
  This local also restores the following `SetScale`'s complete 416-byte
  match from twelve differing encodings; the remaining mission functions
  are unchanged. Delaying the counter initialization until after the global
  stores worsens the target to 23 differences. A register hint on the object
  argument and initialization of the counter at its declaration leave the
  retained fifteen differences unchanged. These further probes are reverted.
- `init_ship` retains the record's full-width object type through its calls.
  Its normal and hazard paths form an `if`/`else`, with an immediate return
  after successful ship setup and a shared final return for failed setup or
  a hazard. The hazard path never initializes the returned short: the original
  reads its stack slot at `0x0040C937`. Preserve that native behavior; the
  SDL branch retains its existing explicit -1 result. The dead-pilot test
  compares against eight with `JG`, rather than nine with `JGE`. Together
  these choices recover the 16-byte frame, saved EBP, and complete 336-byte
  function exactly.
- Removing the cached pointers in `free_ship` also changes the formation
  helper's first array-index calculation: its formation-index and spot loads
  exchange registers. `set_formation_position` loses its exact match with
  three differing instructions; its size and mnemonic score remain unchanged.
  Reordering its locals, caching its spot, and renaming its record parameter
  did not recover the original load order. These trials are not retained.
- The launcher's missing sixteen bytes are still unresolved. The original
  holds `'-'` in EBX and uses immediate zero stores; CL instead holds zero
  in EBX. Moving the command local outside the loop or using `strcmp` did
  not change that choice; a direct conditional-expression switch was worse.
  Volatile zero stores and an explicit leading default case also leave the
  same output; neither probe is retained.
  Parsing through a character pointer adds different address calculations
  and lowers similarity to 94.37%. A register-qualified prefix local leaves
  the target unchanged while disturbing `MainWindowProc`. Both are reverted.
  Further guarded-loop and EOF-exit forms, memset-based flag clearing,
  signed/unsigned command bytes, declaration permutations, and local renames
  do not resolve the 272-byte span or EBX constant choice. An explicit prefix
  branch or switch
  can recover a 288-byte span but adds instructions absent from the original
  and still keeps zero in EBX. A short command likewise recovers the size
  through incorrect word operations. No such size-only workaround is retained.
  Similarly, reversing comparison/addition operands or renaming the stream
  parameter did not alter the one-byte differences in the `/Od` DSP stream
  functions. These trials are not retained. Distinguish such body differences
  from the last function's linker padding: `ix_thread_get_audio_chunk_size`
  has a matching instruction body, but object-end alignment contributes a
  size difference. At the target-sort checkpoint, binary-comp also matched
  all 83 instructions and 290 code bytes of `ix_dsps_unlock`; two original
  trailing INT3 bytes became seven, explaining its five-byte excess then.
  The later DSP declaration checkpoint described above changes its guard
  and body size, so the earlier body-exact result is not a current claim.
  `ix_dsp_free` now matches its complete span after the preceding function's
  size correction.
- `ix_sound_start` has three one-byte excesses where its spilled fastcall
  argument is compared with `pFreeSoundList`. The original loads the global
  into EAX and compares the stack slot; the rebuild loads the stack slot
  and compares the global. The corresponding comparison in
  `ix_sound_unlink_from_free_list` has the same issue. Qualifying those
  global reads as volatile leaves these comparisons unchanged and worsens
  neighboring sound routines, so that probe is not retained.
- The two game-side audio-list removal routines retain comparison-operand
  differences despite 100% mnemonic scores. Reversing equality operands,
  separating declaration order from initialization order, and changing the
  parameter names to `wave`/`sound` all produced the same instructions.
  An explicit pointer to the pilot-table slot likewise did not improve
  `add_statistics` and changed allocation in neighboring functions.
  These trials were reverted; binary-comp confirmed that the restored
  executable was identical to the previously verified build.
- `PostMission` divides the saved kill count by five before incrementing the
  mission count. Its badge tests compare directly against five and 25. The
  wingman's statistics call passes an immediate one; the alternate call
  passes the random mission count and a conditional kill count. Keep the
  random kill result unsigned through argument promotion: the original
  emits `MOVZX EAX,AX`. These choices recover the complete function, including
  its switch tables and deliberate first-mission fallthrough.
- The mission-score routines declare the objective index before the score
  pointer and initialize the accumulated score before calculating that
  pointer. Both now compare exactly. `UpdateSeries` calls
  `PlayersMissionScore` first, retains that word result, then directly
  compares the result of `FullMissionScore` with it. The successful-series
  branch precedes failure, with immediate flag stores in each branch.
  Compare the saved short ship type against the full-width ship enum rather
  than narrowing both operands. This restores its missing sixteen bytes
  and gives an exact comparison.
- `GameFlow` skips both debriefing and the promotion rank increment after
  ejection; the original jump at `0x0040F71B` lands at `0x0040F7CE`. A failed
  promotion roll still reaches debriefing at `0x0040F7AE`. Keep promotion
  eligibility as one short-circuit expression and the ending dispatch as
  a switch with direct calls. The generic ending passes `!bSeriesFailed`:
  the original `CMP`/`SBB`/`NEG` sequence produces one for a zero flag.
  Declaring the flown mission before the flown series also recovers the
  original series register and mission stack slot. The 1,168-byte span
  and all mnemonics now match; twelve instructions still differ because
  the preflight zero and minus-one registers are exchanged.
- `free_viewport` searches its allocation table using `viewport->allocation`
  while retaining a separate allocation local through the release calls.
  CL eliminates the repeated field read but keeps the original register
  copy and search-loop allocation. This reduces differing instructions
  from 50 to six at the same size; loop comparison direction and four
  equality-operand orders remain different. Reversing their source operands
  did not change the output. Explicit additions instead of compound
  assignments in `add_statistics` likewise did not improve its allocation;
  those trial forms are not retained.

- The AIFF and WAV sample loaders retain the file pointer in the outer scope
  and declare their end and chunk pointers in the subsequent chunk-walking
  block. This reproduces their original -4, -8, and -12 stack slots under
  `/Od`; merely reordering the outer declarations does not. Keep explicit
  default/break cases in both switches. Their otherwise unreachable jumps
  are present in the original spans and account for five bytes in each.
  The AIFF payload's byte-swap loop reads unsigned words. Its channel-count
  swap masks after the left shift, while the bits-per-sample swap masks
  before the right shift. These expressions recover the different original
  byte-register sequences; cast the channel count to unsigned before the
  left shift to preserve the operation for negative input without signed
  shift overflow. Both complete loader spans, 1,263 and 953 bytes, now match.
  The AIFF expression correction also restores the following WAV loop's
  comparison order. A `for` loop, reversed comparison operands, alternate
  pointer casts, and an additive byte-swap form did not improve these
  routines and are not retained.
- The extended-number converter masks the exponent's second input byte
  with `0xFF`, producing the original `MOV CH,CL` and direct low-byte load.
  Its signed-result dispatch has an explicit `else`, preserving the
  unreachable jump after the negative return. Keep the low mantissa in the
  outer scope, the exponent in the next scope, and the value, high mantissa,
  and byte pointer in the innermost scope. Their EBP offsets are then -4,
  -8, -16, -20, and -24, matching every original local slot without adding
  storage. These scopes reproduce allocation; they do not prove the original
  source's block structure. Binary-comp's explicit 378-byte body comparison
  improves from 45 differing instructions to 10 of 116, with unchanged
  length and 100% mnemonic similarity. The remaining differences are the
  mantissa byte-load and shift order: the original high word reads offsets
  2, 3, 4, 5 and its low word reads 8, 7, 6, 9.
  Regrouping OR expressions, volatile input reads, unsigned-int casts, and
  reordered low-word terms did not improve those accesses. Additive assembly
  of the words increased the count to 16; signed input bytes with explicit
  masks added 36 body bytes. These trials are not retained. Its reported
  size also includes following import stubs and alignment;
  `docs/EXPORT.md` records the separate body comparison.
- `close_behind` keeps its range parameter in memory by qualifying that
  parameter as volatile. This recovers the original global load into AX and
  the complete 32-byte span; the shared prototype and calling convention
  are unchanged. The qualifier is a matching declaration choice, not proof
  of the original source spelling. The remaining `CMP` and conditional jump
  have reversed operands and direction. Reversing the source comparison,
  with or without the qualifier, does not fix them. A saved target-range
  local also failed and perturbed `scan_for_enemy`; it is not retained.
- `set_up_screen_viewport` selects static geometry with switch cases four
  and five. CL emits the original lower-bound `JL`, upper-bound `JLE`, and
  packet-geometry fall-through; equivalent if/goto forms place the static
  geometry first. Store the height directly in `nScreenHeight`, then divide
  that word for the vertical center. This keeps the geometry pointer in ECX
  and the height in DX, eliminating the extra ESI save/restore and recovering
  the original 336-byte span. Both screen dimensions already have short
  declarations, so their stores need no pointer casts. All mnemonics now
  match; the origin-Y load/store pair in each branch still uses CX where
  the original uses DX. A separate origin-Y local does not alter that result.
- `sort_viable_target_list` declares and initializes its distance temporary
  inside the inner loop, and computes the next index into `inner` before
  assigning `nextOuter`. Together these recover the original eight-byte
  frame and reduce differing instruction encodings from 24 to fifteen.
  Binary-comp confirms that all other 63 functions in `ship.c` retain their
  bytes under address normalization. The saved index is still at
  `[ESP+0x12]` instead of `[ESP+0x10]`, the entry comparison is reversed,
  and CL caches an outer-distance pointer absent from the original.
  The aligned span remains 144 bytes instead of 160; the 93.48% mnemonic
  score and report's size classification conceal this partial improvement.
  Scoping the distance alone leaves the four-byte frame. A guarded `do`,
  further next-index or target-value scopes, and qualified distance reads
  do not improve the retained output. An explicit candidate-distance local
  also changes neighboring functions and loses exact comparisons; it is
  reverted. The earlier volatile-next-index and ordinary nested-for probes
  likewise remain discarded.
- `get_player_input` copies each twelve-byte input sample with an inlined
  `memcpy`. This also recovers the earlier guard's original load, test,
  increment, and store, raising similarity from 86.44% to 96.61% without
  changing its 224-byte span. Eleven instructions still differ: comparison
  operand direction, early guard-decrement scheduling, and the copy's final
  load/store order. Explicit guard, destination-pointer, and sampled-field
  locals did not improve the result and could perturb neighbouring functions.
  Reversing comparison operands or moving the early decrement after the copy
  also failed to recover the original sequence; those trials are not retained.
- The flight-control mouse coordinates passed to `SetMousePosition` stay
  32-bit through the division and increment. Narrowing them to `short` first
  inserts conversions absent from the original. The target-lock key case
  handles a new press before the repeated-key `else` path. Together with
  restricting the later event-modifier ejection fallback to SDL, these changes
  recover `HandleSpaceFlightControls`' 3,184-byte span at 95.69% similarity.
  The native ejection path tests only `GetControlKeyState`'s result; the
  fallback added in commit `e28f61c` remains available in the modern port.
  Register allocation, viewport-mode increment arguments, and several cached
  global loads still differ. Reversing the previous/current-key comparison
  did not recover the original initial byte register and is not retained.
- `UpdateArcadeScoreDisplay` tests both countdowns with `<= 0`, preserving
  the original zero comparison and `TEST AX,AX` after decrement. Using `< 1`
  emits different jumps and an extra instruction byte that increases the
  aligned span by sixteen bytes. The complete 224-byte function now matches.
- `objective_lost` indexes its mission record directly, without copying the
  objective parameter or type into redundant locals. Its own comparison is
  unchanged, but the resulting unit allocation restores exact matches in
  `target_locking` and `place_damage_on_cockpit`. A cached record pointer and
  a widened index did not improve the objective routine. Neither did a signed
  state local with a nonzero test. In `onboard_explosion`, direct array access,
  explicit promoted indices, a cached collision radius, and moving the byte
  offset calculation all retained the incorrect twelve-byte frame and reduced
  similarity. These trials are reverted; its original frame is sixteen bytes.
  Qualifying the objective routine's signed-byte state read and subsequent
  integer type read as volatile preserves their original order. Its 78-byte
  body and 80-byte aligned span are unchanged, mnemonic similarity rises
  from 84.62% to 100%, and differing encodings fall from fifteen to thirteen.
  EAX/ECX/EDX allocation still differs. Together with the missile-coordinate
  qualifications above, the other 123 cockpit report rows remain unchanged.
- `select_new_release_weapon` retains the starting cycle index as a signed
  word, even though its running index and weapon count are signed bytes.
  The original sign-extends DL into DI and later compares AX with DI.
  Correcting that one local raises similarity from 87.88% to 94.90% while
  preserving the 336-byte span. The preferred-type search still reuses a
  cached count where the original reloads memory, and several comparisons
  have reversed operands. Changing the search to `do`/`while` or using plain
  `char` for the count did not help; reading the count through a signed-byte
  pointer or moving the current-weapon capture into its test made it worse.
  Those trials are not retained.
- `find_next_gun` returns the selected slot's type directly, narrowed to a
  short, rather than returning the cached enum. This restores the original
  word reload from the slot and reduces differing encodings from 22 to 21
  of 60 at the unchanged 160-byte span. A register hint on the found-current
  flag emits the same code. Replacing the guarded `do` loop with a `for`
  loop worsens the byte comparison to 55 differences despite raising its
  mnemonic score; neither extra change is retained.
- `DrawScrambleActor` retains the script-control mask as an unsigned byte
  and adds the base frame to its signed-byte frame before sign-extending the
  draw argument. A 16-bit frame sum omits the original byte wrapping.
  Each screen coordinate combines its background offset with the actor's
  compound position update. This recovers the exact coordinate and draw-call
  sequence and raises similarity from 81.94% to 97.22%, with the original
  256-byte span. Fourteen instructions still differ because the script index
  and control mask exchange BL and DL, moving the initial index load past
  the scale calculation. Descriptive local renames and removing the frame
  mask's redundant cast did not help; a promoted actor-index local was worse.
  Separate world-coordinate and screen-coordinate additions also gave a
  different schedule and are not retained.
- `evaluate_damage` still moves the front-armour contribution after the
  other armour terms. A short running accumulator, explicit word casts on
  each quotient, and changing the source addition order all produced the
  same differing instruction sequence. These trials are not retained.
- `wing_status` saves the current campaign mission number in an int local
  after its no-death guard, then uses it in both comparisons. Repeating the
  arithmetic in each condition makes CL reverse the shared comparison and
  final branch. The saved value recovers the complete 64-byte span exactly.
- `AllocateViewport` calculates width directly from the two viewport fields
  before saving the left edge. This keeps the original `SUB CX,AX` and delays
  the left-edge stack store until after the display-mode comparison. Pass
  the clear-colour word unchanged to `ClearViewport`; narrowing it to a byte
  adds an absent `MOVZX AX,AL`. These two changes retain the 320-byte span,
  raise similarity from 84.16% to 87.13%, and reduce differing instructions
  from 99 to 55 of 101. The original also reloads the row-table field in its
  loop and uses a signed, initially guarded comparison against height plus
  two. Applying those forms together changed register allocation and added
  sixteen bytes. Moving the offset initialization before the second allocation
  improved scheduling but did not recover the size. Widening top increased
  the local frame; widening height, removing the temporary allocation pointer,
  and removing unsigned promotion casts did not improve the retained result.
  These broader trials are not retained.
  Subsequent captured-pointer and volatile-parameter forms still allocate
  the wrong registers, add a stack slot or sixteen code bytes, and do not
  improve the retained result. Removing the allocation-result local does
  not recover the height register. The bounds routine's explicit angle-index
  local and qualified sine-table read likewise leave its output unchanged;
  moving the first zero-coefficient correction before the sine calculation
  changes instruction order without improving the match. These trials are
  also reverted.
- Earlier constellation probes removed the initializer's promoted
  particle-index local, used separate short counters, or initialized height
  before width. None recovered its saved height and register-held particle
  counter; the storage and cache correction above does. Narrowing the draw routine's
  random index to a short introduced a stack spill and reduced its match.
  These trials are not retained. Negating the combined valid-index condition
  in `ix_dsps_prepare` also left its reversed comparison unchanged.
- `animate_shape` uses a second switch for the original 32-bit scale-command
  tests. Earlier unit contexts made that switch enlarge `object_collision`
  by sixteen bytes and disturb the then-exact `hit_asteroid`, so those probes
  were reverted. After recovering the housekeeping dispatch, a short
  scale-change local and a promoted int object index retain the animation's
  original 352-byte span and restore the collision routine's 92-byte frame.
  Recovering the preceding movement routine's engine switch then restores
  the original animation-pointer and parameter registers and the two distinct
  scale stores. Similarity is now 96.15%, with nine differing instruction
  encodings out of 104 in the scale-factor preparation and multiplication,
  down from 75 before the two dispatch corrections. Reversing the product
  operands leaves these nine differences unchanged. Earlier probes with a
  saved current-scale word, scoped animation pointer and type, register hint,
  qualified scale store, or separate masked-factor assignment either left
  the differences unchanged or disturbed the following collision routine;
  none is retained. The existing nine register differences in `hit_asteroid`
  also remain unchanged. A guarded decrement loop and a register hint there
  do not resolve them either.

- Giving `objective_lost` an explicit mission-ship index, as either an int
  or a short, leaves its allocation unchanged. Reversing the command/opcode
  equality in `FindSceneAnimationCommand` likewise leaves its compared-byte
  order unchanged. Saving `nTgtRange` in a short before `Mveer_away`'s
  proximity test does not change its reversed loads and comparison. These
  trials are not retained.

Do not hand-write thunks, vtable dispatch, exception tables, or other compiler
glue. `make audit-compiler-glue` enforces this. A real function annotation must
immediately precede its definition; validate annotations with
`make audit-addresses`.

## Edit loop

```sh
make compare-func FUNC=FunctionName
make report FILTER=source_name
make verify
```

Stop at 90% similarity unless a clear evidence-backed improvement remains.
Run the Win32 build through DREAMM rather than Wine; the original depends on
Windows 95 DirectDraw and DirectSound behavior.
- The one-byte comparison differences in `ix_dsp_set_master_volume` and
  `ix_thread_handle_file_chunk` survive volatile global reads, volatile local
  indices, and long indices in place of int indices. None of those probes
  changed their comparison order or size, so they are not retained.
  Casting both operands of `ix_dspv_set_active`'s upper-bound comparison to
  long likewise leaves its differing comparison unchanged, while disturbing
  other voice routines and the following mixer span. That probe is reverted.
  `ix_sound_is_playing` also has a matching instruction body; its reported
  four-byte shortage comes from object-end alignment, like the thread and
  DSP final functions discussed above.
- `ReadRequiredPilotField` still loads and pushes its Y argument through EDX
  where the original uses EAX. An unsigned cursor-X local produces the same
  output; moving the X expression directly into `SetTextCursor` worsens the
  comparison from two to eight differing instructions. Both probes are
  reverted, preserving the original load scheduling recovered so far.
- Reversing the comparison operands in `ix_dsp_set_master_volume`,
  `ix_thread_handle_file_chunk`, `ix_thread_service_streams`, and
  `ix_system_find_free_voice` does not improve their current output.
  Declaring the master-volume loop index in the `for` initializer and
  naming the free-voice index `voiceIndex` likewise leaves their comparisons
  unchanged. A volatile input-pointer local in the extended-number converter,
  combined with original-order low-mantissa terms, still leaves ten differing
  body instructions. These probes are not retained.
- A volatile byte-offset local in `onboard_explosion` recovers a sixteen-byte
  local frame but drops the original saved EBP, leaving its effective stack
  slots wrong. Byte differences increase from 49 to 58 of 61 instructions
  despite the mnemonic score rising from 81.97% to 88.52%. Assigning that
  offset within the velocity-copy address expression still leaves 58
  differences and drops similarity to 75.41%. Neither probe is retained.
  Limiting the volatile read to the later position calculation still drops
  the saved EBP and leaves 58 differences. Chaining the offset store into
  a real velocity offset, caching the radius before the vector copy, and
  moving the offset calculation after the scale update do not improve the
  original 49-difference result. The latter restores four saved registers
  but hoists the debris index before `set_objects_data`, leaving 54
  differences. An inlined `memcpy` emits the same copy as struct assignment.
  These further probes are also reverted.
- `control_speed` still saves an extra EBP where the original reuses its
  speed-address register for the braking margin. A separate speed-difference
  local leaves the target function unchanged and disturbs `chase_speed`.
  Combining the multiply and subtraction into one expression changes
  scheduling without reducing its 63 differing encodings. Calculating the
  speed difference before multiplying restores part of the register reuse,
  but still saves EBP, leaves 53 differences, and also disturbs `chase_speed`.
  These probes are not retained.
- `GetLargestFreeNearHeapBlock` still reverses the operands and branch sense
  of its two descriptor-bound comparisons. Reversing the source comparison,
  spelling the loop as an explicit guard and `do` loop, and qualifying the
  descriptor-bound read as volatile all emit the same four differing
  instructions at the original eighty-byte span. None is retained.
- `GetViewportIntersection` preserves a reversed `for` header: it tests the
  point index's postincrement and discards the coordinate-count comparison
  as its iteration expression. The inside count is not initialized. WC2's
  original unoptimized counterpart exposes that control flow; its current
  reconstructed C is not authoritative. Under WC1's optimizer, the complete
  source retains the two coordinate arrays and reproduces the original
  overlapping stack reads, three explicit index shifts, 148-byte frame, and
  432-byte span. Do not repair the loop or initialize its count: these are
  original defects in an unreferenced routine. The final maximum stores also
  intentionally overwrite the preceding minimum results. A `do`/`break`
  rewrite does not produce the same code. Keep the owner-scoped prototype's
  parameters unnamed; naming them disturbs existing event-manager matches.
  Detailed cross-release and stack evidence is recorded in `docs/ORDER.md`.
- `AllocateFontWorkspace`'s loop reloads the workspace before advancing its
  byte offset. A cached entry pointer leaves ten differing encodings, versus
  the existing eight. A typed array loop with the increment in the width
  assignment recovers every mnemonic, but swaps the workspace and index
  registers, leaving fourteen differences. Reordering those declarations
  does not resolve the swap. These font probes are not retained.
- Superseded (static `.bss` checkpoint; these are now statics in `dib.c` and
  `mono.c`). Two further unused-declaration groups affect core code selection: the nine
  DIB buffer declarations from `DAT_00476640` through `nDIBSlamCount`, and
  mono's four file-error buffers plus its two MONODEBG variables. Original
  references place the first group only in `dib.c` and the second only in
  `mono.c`. Omitting the DIB declarations in `cmpgn.c`, `eject.c`, and
  `screens.c`, and the mono declarations in `dib.c` and `eject.c`, removes
  65 differing instruction encodings without losing an existing exact
  function or changing any reported span. `EstablishingShot` and
  `SceneDirector` become exact; `DIBupdate` falls from 45 differing
  instructions to five. These guards reproduce declaration context, not
  recovered original header names. Both individual groups and their
  combination were checked across eligible core units; the other placements
  are not retained. In particular, omitting mono's declarations in `geom.c`
  gains four exact functions but loses three existing exact functions for
  only three fewer differing encodings overall, so it is rejected.
- Further small source probes do not improve the retained build.
  `close_behind` is unchanged by descriptive range-parameter names, a
  const-volatile parameter, an early failure return, or nested tests.
  Chaining `ResetStringBuilder`'s cursor assignment with its byte clear,
  or clearing through the assigned cursor, leaves its four register
  differences. Reversing `animate_pilot`'s equality or saving its desired
  byte frame does not fix its four body differences; the saved frame also
  disturbs `place_damage_on_cockpit`. Separate wingman assignment in
  `clean_up_cockpit`, and scoped or function-scope enum-class locals in
  `check_futurion`, leave their output unchanged. An explicit failure test
  in `equ_vector` leaves that routine unchanged while disturbing later
  geometry functions. `Mfull_ahead`'s direct post-decrement test becomes
  exact but disturbs `general_zig` and increases the unit's total differing
  encodings by six. Saving the post-decrement result in the existing live
  counter local restores the baseline output throughout. None of these
  source probes is retained.
- Check argument and return widths even when every mnemonic matches.
  `SetAwakenBarracksMenuLabel` increments its allocation length in ECX,
  but its later copy length in CX. The native barracks unit therefore keeps
  a word-count declaration for `DosMemcpy`; the shim itself still forwards
  the complete dword stack argument to `memmove`. Keep this declaration
  discrepancy local to that caller. Casting the length or sum to signed or
  unsigned short under the size_t prototype adds extensions absent from
  the original. The local prototype recovers the entire 112-byte span.
- `SetBunkMenuLabel` and `SetAwakenBarracksMenuLabel` accept signed-word
  series and mission arguments. This restores the four byte-to-word sign
  extensions in `GetBunkInfo`; the callee's dword forwarding loads do not
  establish wide formal arguments. Both bunk-menu calls receive the save
  record's description buffer. In the second call, the original address
  calculation occurs before the preceding call's 24-byte stack cleanup;
  account for that cleanup instead of inferring a pointer into pilot data.
  These corrections recover `GetBunkInfo`'s complete 176-byte span, while
  both menu-label callees retain their exact results.
- The native cutscene owner sees `init_3Space_objects(short scene)` rather
  than an unspecified-argument declaration. Its three signed campaign-byte
  arguments then extend into AX, matching `RunAnimationDemoLoop`'s 536-byte
  body and complete 640-byte span. Other native owners retain their existing
  declarations; this is recovered caller context, not original header text.
- `try2rout` returns a short and `send_message` takes a short message number.
  The former restores `request`'s `TEST AX,AX`; the latter restores its
  `SUB BX,2` rather than `SUB BL,2`. The shared declarations and definitions
  agree, their calling conventions and callee comparisons are unchanged,
  and the complete 832-byte request span now matches. A local result cast
  alone fixes the test but leaves the message subtraction one byte short.
- `set_up_action_sphere` assigns its initial wave with the ordinary
  conditional expression selecting two or minus one. CL emits the original
  dword mask and decrement before the word store. The previous explicit
  mask expression truncated those operations to AX. The change removes
  twelve differing encodings at the unchanged 352-byte span; object-class
  register allocation and a loop branch still differ.
- Further explosion and screen-row probes are not retained. An explicit
  debris-velocity destination pointer or a second byte offset worsens
  `onboard_explosion`'s comparison; keeping both destination pointers, at
  function or branch scope, also loses sixteen bytes from its aligned span.
  `InitializeDIBScreenViewport` with a running word offset produces a body
  three bytes too short and worsens its span comparison from nine to eleven
  differing encodings, with either a for loop or a postincrement do loop.
- `PlaySfxWaveFileByNumber` retains the vector magnitude in a branch-local
  long before assigning its integer playback distance. Test the magnitude
  against 32,000, rather than the copied distance. CL then emits the original
  `MOV EDI,EAX` followed by `CMP EAX,32000`, recovering the complete
  144-byte span. An assignment inside the condition leaves the old EDI
  comparison; a ternary clamp adds seven body bytes. Neither is retained.
- `AnimateBarracks` saves the bunk's animation frame in a short before
  selecting either `24 - animationFrame` or 24. An if/else or ternary
  selection reproduces the original second `MOV DX,24`; setting the
  default before the condition lets CL omit that instruction. Keep the
  final open-eye and closed-eye sprite calls in separate branches so CL
  emits the original literal pushes instead of Boolean mask arithmetic.
  Together these choices recover all 560 bytes, including alignment.
- The native ending owner also sees a short-scene prototype for
  `init_3Space_objects`. Its escape scene sign-extends the campaign byte
  into AX, not EAX. Keep this owner's prototype parameter unnamed: naming
  it `scene` disturbs the earlier exact `ReleasePacketHandle` by seven
  encodings. The unnamed declaration preserves that routine and removes
  44 differences from the escape scene's unchanged 928-byte span.
- `ShutdownGameWindow` saves `GetCurrentProcess()` in a handle before
  setting its priority. The original calls the getter before pushing the
  priority constant; a nested call expression reverses those instructions.
  The saved handle restores every mnemonic at the unchanged 112-byte span,
  leaving six register-allocation differences in its cursor calls.
- Further local-value probes are not retained. An explicit promoted ship
  index leaves `reset_tactic`'s four register differences unchanged.
  Making `PromptForTextInput`'s width accumulator long, or retaining the
  prompt width in another integer local, leaves its five body differences
  and two-byte body shortage unchanged.
- Three more private declaration groups affect native code selection:
  eventmgr's three event-list globals, joycal's four calibration bounds,
  and winsys's fourteen keyboard polling variables (the keyboard group is now
  `winsys.c` statics, so its exclusions no longer exist). Every original direct
  reference to these groups stays within its corresponding unit. All seven
  combinations were checked in owners that do not reference the variables.
  Retain all three exclusions in `fl.c`, event-list plus keyboard exclusions
  in `mono.c`, and event-list plus calibration exclusions in `view.c`.
  These are measured declaration scopes, not recovered original headers.
- With those flight declarations omitted, `Mtarget_missile` keeps a promoted
  integer object index separately from its short loop counter. Declare the
  integer before the short at function scope and use it for both object
  tables. This preserves the original `MOVSX EAX,CX` and the routine's exact
  176-byte span. A loop-scoped integer restores the six register differences
  instead. The retained unit context also makes `Mfull_ahead` and `Mgloat`
  exact; `perform_maneuver` improves from 34 differing encodings to eighteen,
  although mnemonic similarity falls from 95.19% to 94.23%.
- The mono declaration context makes `advance_canned_sequence` exact and
  reduces `print_subtitle` from 37 differing encodings to eighteen. The view
  context makes `CalcRectangleArea` exact and improves `OpenPacketSection`
  by two encodings, while each constellation routine gains two differences.
  All spans remain unchanged, and no existing exact routine regresses.
  Other tested placements are discarded: in particular, the geometry
  alternative gains four exact functions but loses three for only seven
  fewer encodings, and the music alternative trades one exact function for
  another for only one fewer encoding.
- Further small body probes are discarded. Saving `MoveNewCampaign`'s day
  separately, including a qualified rollover-pointer read, leaves its
  missing pointer reload unchanged. Grouping `DIBupdate`'s integer offsets
  before adding their base pointers, and saving `RenderSpaceViewFrame`'s
  bonus in a local, leave their output unchanged. Moving the mine search's
  counter initialization into its for header worsens its comparison, as
  does retaining an integer weapon-slot offset with that change. Reversing
  the three palette-loop source comparisons leaves the same six differences.
  Inlining the barracks bunk-row expression loses sixteen aligned bytes.
- Three additional private declaration groups have confirmed original users:
  six button-history fields in `input.c`, four raw-coordinate/dead-zone
  fields in `joycal.c`, and six packet/screen allocation fields in `screen.c`.
  All 49 relocated references stay within those respective owners. All seven
  combinations were checked in eligible core units. Retain button-history
  exclusions in `dib.c`, `fade.c`, `logic.c`, `main.c`, and `nearheap.c`;
  joystick-state exclusions in `main.c`; and screen-table exclusions in
  `fade.c`, `logic.c`, and `main.c`.
  The shared second-button double-click result remains available to `main.c`.
  These are measured declaration scopes, not recovered original headers.
- This context makes `PurgeNearHeapBlocks` exact at 144 bytes and
  `SelectPreviousExternalViewObject` exact at 64 bytes. `FreeNearHeapBlock`
  and `GetNearHeapBlockSize` reach 100% mnemonic similarity with two remaining
  differences each: their initial base/size loads remain reversed.
  `DIBupdate` falls from five differing encodings to two, retaining the
  reversed destination-pitch multiplication operands. `StepPaletteTransition`
  falls from six differences to three and reaches 99.35% mnemonic similarity.
  `get_player_input` rises from eight differences to eleven.
- In the selected logic context, `try2rout`, `get_rear_spot`,
  `FindEnemyAimingWithinRange`, `in_danger`, `abandoned`, and
  `FindSceneAnimationCommand` become exact. `UpdateSceneAnimationObject`
  drops from 81 differing encodings to nine. Three previous exact matches
  regress: `report_kilrathi_rout` has eleven differences, `find_ship_index`
  six, and `set_special` two. `attacker_in_range` rises from eleven to 36
  differences and falls to 85.42% mnemonic similarity; `hostile_sphere` rises
  from six differences to ten. These tradeoffs still remove 42 encodings
  within logic, and 63 across all five selected units, with five additional
  exact functions overall and no size changes.
  A scoped integer distance in `attacker_in_range` restores its 154-byte body
  and reduces its differences to sixteen, but raises the scene-update routine
  from nine differences to 170. Folding the distance assignment into the
  comparison leaves both routines unchanged. Both body probes are discarded.
- Before these declaration changes, reversing the promoted index captures
  in `object_collision` leaves its two load-order differences unchanged.
  Reversing the navigation square's bottom sum likewise leaves two
  differences; saving that bottom in a short local instead shortens its body
  from sixty to 57 bytes and leaves seven body differences. In
  `set_objects_data`, qualified scale/owner reads merely move the two
  scheduling differences to the following scale store. Scoped scale and
  owner-byte locals plus a qualified destination store leave three differences.
  All of these body probes are discarded.
- Seven briefing/medal globals have 48 original relocated references, all
  within `screens.c`. Omitting their declarations from `gameflow.c` restores
  the operand order of both screen-pointer comparisons in `free_viewport`.
  Its 228-byte body and complete 240-byte span now compare exactly. No
  other game-flow routine changes. The existing `WC1_GAMEFLOW_SOURCE` marker
  controls the exclusion; storage, types, and function bodies are unchanged.
- The room, briefing, and conversation declaration matrix checked all seven
  combinations across eligible core units. The room group's seven globals
  have 54 original relocated references; the conversation group's five have
  ten, all in their respective owners. Only the briefing exclusion above is
  retained. Keep `pDebriefingSceneDataPtr` outside this experiment: some encoded
  operands point to its last byte because they address the following scratch
  buffer through `szTxtWorkspaceBuffer - 1`. An encoded displacement inside
  a global's range does not by itself prove an access to that global.
- A second seven-case matrix covers six mission map-bound globals, three
  navigation-label globals, and six simulator globals. Their twenty, 34,
  and 87 original relocated references stay within `mission.c`, `nav.c`,
  and `trainsim.c` respectively. None of these tested exclusions improves
  a unit while preserving its exact matches and sizes, or offers a useful
  net improvement, so none is retained.
- Unused function prototypes also affect CL 4.20 instruction selection.
  `WaitForJoystickButtonRelease` and `WaitForJoystickButtonPress` have ten
  original direct references, all within `joystick.c`. Omitting their
  prototypes from `main.c` reduces `get_player_input` from eleven differing
  encodings to eight at its unchanged 224-byte span and 222-byte body. Its
  three saved-input comparisons regain the original operand order. The
  remaining differences concern scheduling and the copied sample's index
  register. No other main-module routine changes. All seven combinations
  with four mono helpers and three campaign helpers were tested; other
  exclusions are discarded. These are measured declaration contexts, not
  recovered original header text or changes to linkage or calling convention.
- Further audio and heap probes are discarded. Renaming the DSP voice
  parameter to `voiceIndex` leaves all fourteen voice-module report rows
  unchanged. Qualifying the heap-base read, or both base and size reads in
  the same expression, leaves the two reversed loads in `FreeNearHeapBlock`
  and `GetNearHeapBlockSize` unchanged. Sequencing the two qualified reads
  through the existing descriptor variable adds one body byte to each and
  leaves seven and nine body differences respectively. Each probe was
  restored and force-rebuilt to the preceding checkpoint's executable hash.

## Symbol-index costs and offset-independent shapes

CL 4.20 breaks register and operand-order ties by symbol index. Register
choice depends on the index modulo 32. Comparison and commutative operand
order repeat with period 257, with windows about 17 indices apart inside each
period. A function is therefore judged by whether it is exact at some probe
offset, and a unit by whether one offset makes all of its functions exact.
Across the long sweeps of every unit, 196 constrained routines are exactly
32-periodic, 80 are exactly 257-periodic, and 97 mix both, so a function is
exact when both its mod-32 and its mod-257 conditions hold. A two-local tie probe
(each local live across calls, used symmetrically) measures what a source
fragment consumes, independent of the code it generates:

| Construct | Indices |
| --- | ---: |
| function definition | 2 |
| parameter, local, prototype, extern, typedef | 1 each |
| static local | 2 |
| string literal, user label, implicit function declaration | 1 each |
| `if` | 1 |
| `else` | +1 |
| `for`, `while`, `do` | 3 |
| `\|\|` in a branch condition | 1 per condition (0 as a value) |
| `&&`, `!`, `?:`, casts, `return`, `break`, `continue`, calls to declared functions, bare blocks | 0 |
| `switch` | 4 + 1 per `case` or `default` |

Consequences: an `else` after a returning branch, a temporary local, or a
`||` condition shifts every later function in the unit even when the code is
unchanged, and the order of a function's local declarations moves its own
period-17 windows without moving any other function.

Shapes recovered with this method:

- `sort_viable_target_list` is the natural selection sort: nested `for`
  loops over `outer`/`inner`, the distance read into a local before the
  comparison, and the target byte saved before the four stores. CL then
  spills the `outer + 1` common subexpression and the `count - 1` bound,
  restoring the original eight-byte frame and 160-byte span; the explicit
  next-index variable and guarded inner loop of the earlier reconstruction
  made CL cache the outer distance address instead.
- `explosion_shock_wave` passes `the_creator(obj)` directly as
  `inflict_damage`'s first argument. Without the attacker local, CL keeps the
  ship-class constant in EBX as the original does.
- `PromptForTextInput` and `ReadRequiredPilotField`
  compute their width and cursor arguments inline; the named
  temporaries of the earlier reconstruction changed scheduling or kept the
  sum in the wrong register.
- `AllocateFontWorkspace` is a one-entry `for` loop over the workspace array;
  CL strength-reduces the index into the 12-byte stride the original uses.
- `FreeNearHeapBlock` declares `descriptorAddress`, `block`, `address` in that
  order; the descriptor comparison's operand order is a period-17 tie between
  those locals.
- `ix_sample_convert_from_ieee_extended` keeps Apple's
  `(unsigned long)(bytes[i] & 0xFF) << n` mantissa terms. CL 4.20 orders the
  four OR operands by a period-17 key, and the masked terms reproduce the
  original order at offsets 16 and 33.
- The release-weapon display position is one `ShortPoint` common at
  0x005A7DBC. CL 4.20 does not enregister fields of a global structure, so
  `fire_computer_graphic_missile` reads and updates the position in memory
  exactly as the original does, with no volatile casts.
  `RemovePlayerReleaseWeapon` indexes the player's loadout through an inline
  slot cast; a named loadout pointer swapped the index and narrowed-weapon
  registers.
- `MoveNewCampaign` adds each random day count directly to the campaign
  date in its two branches. With the day local, CL kept the date pointer in
  ECX instead of reloading it after the day word overwrote EAX.
- `drop_player_mine` reads the loadout count through the array in its `for`
  condition and declares `type` before `weapon`. The original compares the
  count byte with the zero index register, a guard CL folds away when the
  count is a separate local. `clean_up_cockpit` uses `nYourWingman` directly.
  With these, all seven `targ.c` routines are exact at offsets that are
  31 mod 32.
- Every maneuver handler in the dispatch table takes `(short ship, short
  target)`. `Mgloat` lacked the second parameter, so `perform_maneuver`'s
  direct call needed a function-pointer cast, and CL ranked the ship
  parameter below the target. `perform_maneuver` calls
  `SetManeuverProximityRange` in each facing branch; CL merges the calls,
  leaving the weight in ECX as the original does.
- Casts and unnamed prototype parameters consume no symbol indices; each named
  prototype parameter consumes one. A prototype's parameter names therefore
  shift every later declaration's index.
- `UpdateDamageDisplay` declares `componentIndex` after the other locals; with
  that order all three `damage.c` routines are exact at the current offset.
- `dib.c` declares `nDIBFrameHeight` before the other frame statics. Its index
  decides the operand order of `DIBslamReal`'s row-limit comparison; the
  identifier-hash order keeps its storage at 0x00476650. With it, all 28
  `dib.c` routines are exact together at offsets such as 27 and 61.
- `cmpgn.c` defines the three mission disk-record structures just before
  `LoadMissionData`, their only user, and drops the reconstruction's
  compile-time size checks. Every exact `cmpgn.c` routine now shares one
  offset window.
- `InitializeCockpitResources` indexes the player's loadout slot directly in
  its missile-background loop rather than through a slot pointer.
- Tie windows are not periodic with period 544: `DIBupdate`'s exact offsets
  step by 17 but slip one extra every 257 offsets. A probe offset therefore
  cannot be converted into "hide N declarations" by modular arithmetic;
  measure the actual configuration. Hiding declarations reaches combinations
  of the two residues that probe offsets 0 to 543 never sample:
  `IsPointInNavMapLabel` is exact with five fewer declarations although no
  probe offset in that range makes it exact.
- A redundant redeclaration costs only its named prototype parameters; a
  repeated `extern` or an unnamed prototype costs nothing.
- A global's common-subexpression temporary carries the global's own index,
  so the order of the `extern` declarations in `include/globals.h` decides
  ties between globals and locals in every unit. Probes inserted before the
  deciding declaration make such a function exact; probes after it, or after
  the includes, never do. The header therefore departs from address order
  where code requires it. Moving `nTgtRange` makes `futurion_intelligence`,
  `close_behind`, `FindEnemyAimingWithinRange`, `in_danger`,
  `chase_location`, and `chase_speed` exact, and moving
  `nEventMousePointerDamageBottom` fixes all three mouse-cursor routines.
  Each accepted move was checked against every unit and costs no other
  exact routine.
- `DecodeShapeFrame` declares its four run-copy temporaries at function scope
  among the other locals. With the recovered order, the run clipping keeps
  the original registers and the routine regains its 16-byte alignment span.
- `DrawFontGlyph`'s recovered local order gives the original 512-byte size;
  its inner loops still differ in register choice.
- `DrawBarracksBunks` declares `frame`, `bunk`, then the short `row` used for
  `bunk / 2`. Keeping the division result short restores the original
  16-bit multiply and the 192-byte span, replacing the 176-byte inline
  expression. Only the scheduling of `PUSH ECX` against `SUB EAX,EDX`
  differs; mnemonic similarity rises from 89.55% to 97.01%. All six local
  declaration orders produce the same result, and the other barracks
  routines are unchanged. Earlier synthetic declaration offsets could make
  the inline expression exact, but that did not hold at the real offset.
- `GetShipAccelerationRate` indexes the type and rating arrays with its short
  `ship` parameter. The explicit promoted `shipIndex` temporary emitted no
  additional instructions, but shifted the later geometry routines' symbol
  indices. Removing it restores `equ_vector`'s comparison operands and
  `rotate_about_j`'s register choices. Declare `cosine`, `sine`, `old` in
  `rotate_about_i`, and `sine`, `cosine`, `old` in `rotate_about_k`; all three
  rotations and the equality routine then compare exactly. The first
  rotation recovers its four-byte frame and 288-byte span, eliminating
  sixteen excess code bytes. All other geometry report rows are unchanged.
  Keeping the promoted index and only permuting the three acceleration
  locals leaves its five differing encodings unchanged. Removing both
  the index and the object-type local disturbs the second rotation.
  The complete report changes only these four rows: normalized exact
  functions rise from 1,320 to 1,324 out of 1,550. The code section now
  has the original 401,111 bytes, and the entry point is `0x0044E770`.
  Binary-comp's strict whole-file comparison rises from 148,530 to
  355,656 matching positions out of 518,656 (28.6375% to 68.5726%).
  The remaining 163,000 differing positions include a relocation section
  that is still four bytes too large; complete executable equality remains
  incomplete.
  A clean native build, modern build, source-order and address audits,
  order report, and `make verify` pass. Both value checks cover 1,550
  functions with zero mismatches or skips. The clean-build and final hashes
  agree: `e25e0dd45e71cf29d12cf930c6e1c75fb0b557f900222f8560ef9eb9561df5e7`.
- Reordering the first three terms of either IEEE-extended mantissa
  expression in `IxSample::ix_sample_convert_from_ieee_extended` does not
  change CL 4.20's operand order. All six orders per component retain ten
  differing encodings in the actual 378-byte body. Use explicit ends at
  `DirectSoundCreate` when checking it with binary-comp; the report's
  documented 26-byte import-stub excess is unrelated to those differences.
- `explosion_shock_wave` declares `other` before its two vectors. This changes
  its remaining comparison from `CMP SI,DI` to the original `CMP DI,SI`,
  making the routine exact without changing any other ship report row.
- `auto.c` defines its private initialized formation count immediately before
  `auto_position`, its first user. This changes the register tie with
  `nYourWingman` in `auto_pilot_sequence`, eliminating a one-byte body excess
  and sixteen aligned bytes. Moving the definition before any of the three
  earlier functions leaves that excess. The sequence's final four short
  locals are `nearestShipRange`, `other`, `cruiseSpeed`, `destinationRange`;
  this removes four further differing encodings. Its span is now the original
  1,472 bytes, with 17 differing encodings and 98.81% mnemonic similarity.
  The other five autopilot-unit routines remain exact. Together with the
  barracks row correction, this fixes two size errors that previously
  canceled in the total code length. The private count retains its original
  owner, type, initialization, and address; the audit collector now preserves
  addressed storage declared between functions.
  These three source edits are the only changed rows in the full report:
  normalized exact functions rise from 1,324 to 1,325 out of 1,550, and size
  differences fall from 20 to 18. Strict whole-file matching rises from
  355,656 to 432,850 positions out of 518,656 (68.5726% to 83.4561%).
  The code section size and entry point remain exact; the relocation section is
  still four bytes too large, and 85,806 file positions still differ.
  The clean native build, modern build, source/address audits, order report,
  collector tests, and `make verify` pass. Both value checks cover all 1,550
  functions with zero mismatches or skips; the existing nonfatal call/global
  access diagnostics remain 11/56. The clean-build and post-verification
  hashes agree:
  `3d7f68a96b204b1c8cab112c70f9a75ec17819d64898c309a758aa3c1415b63c`.
- `PlaceNavMapLabel` passes `offset == 12` directly at its four forced-placement
  call sites. CL retains the same shared comparison and instructions; removing
  the redundant `force` local changes the later symbol indices and makes all
  55 instructions of `DrawNavTriangleOutline` exact. No other navigation
  report row changes. A label pointer instead recovers the placement routine's
  336-byte span but combines its two original absolute stores into one base
  address calculation, so that version is not retained. An explicit label
  index, wider force flag, and moving initialized storage after the first
  helpers do not recover its original address calculation.
- `detect_collisions` declares `candidate`, `other`, `time`, `closestTime` in
  that order. Moving the minimum-time local last restores `CMP DI,AX` and
  `JLE`, making the function exact; all other logic report rows are unchanged.
- Further bounded size probes are discarded. Moving the calibration loader's
  horizontal-range declaration through the other locals or scoping both ranges
  to their calculations leaves its sixteen-byte excess. Joystick range/scale
  declaration swaps can recover its 1,040-byte span, but leave register and
  scheduling differences; the best permutation retaining the 1,024-byte span
  improves mnemonic similarity while reducing literal file matching. Removing
  launch's promoted object local likewise raises its mnemonic score but moves
  its body from 2,046 to 2,041 bytes, further from the original 2,053, and loses
  literal matches. Shape-pointer declaration permutations leave it unchanged.
  Inlining the barracks row with an explicit short cast loses its recovered
  sixteen bytes. A cached menu-label pointer does not fix its comparisons and
  disturbs later barracks routines; the opcode scope and wave-list local-order
  probes leave their one differing comparison unchanged.
  The retained navigation and collision changes are the full report's only
  changed rows: 1,327 of 1,550 functions are now normalized exact, with the
  same eighteen size differences. Strict file matching rises by two positions
  to 432,852 of 518,656 (83.4565%); 85,804 positions still differ. The clean
  native and modern builds, source/address audits, order report, and full
  verification pass. Both value checks have zero mismatches or skips across
  all 1,550 functions; the existing call/global-access diagnostics remain
  11/56. The executable hash is unchanged after verification:
  `0d8b7cad8d0ecb5f1651098829584924a8553f3ec0d209a631ff469a87fa8d29`.
- Declare `nIxStreamerCurrentFileChunk` immediately before the file-chunk
  table it indexes. This makes the cached-chunk comparison load the global
  before comparing the local, recovering `ix_thread_handle_file_chunk`'s
  original one-byte-shorter body. The unit's final padding grows by that byte,
  so `ix_thread_get_audio_chunk_size` also becomes exact. Moving this shared
  declaration changes no other function's report row.
- For the stream owner, declare `nStreamCount` before `IxStream`. This keeps
  the six earlier exact stream routines and additionally matches
  `ix_dsps_get_buffer_free` and `ix_dsps_lock`. The unlock routine's remaining
  excess falls from three bytes to one. This supersedes the earlier placement
  after `nAllocatedStreams`; storage definitions, types, and addresses stay
  unchanged. Moving the count before the voice types instead fixes release
  but loses allocation's exact bounds check; moving its voice-pointer local
  into the allocation body does not recover that check.
  Voice-count declaration moves trade existing exact bounds checks for others,
  and unnamed voice prototype parameters do not change those results. All
  six local declaration orders leave `ix_thread_service_streams`' three
  differing encodings unchanged. These probes are not retained.
  The full report changes only those five stream/thread rows: normalized
  exact functions rise from 1,327 to 1,331 out of 1,550, and size differences
  fall from eighteen to fourteen. Strict whole-file matching rises from
  432,852 to 434,707 positions out of 518,656 (83.4565% to 83.8141%);
  83,949 positions still differ. Compilation-unit ownership and link order
  are unchanged.
  The clean native and modern builds, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `c3c9bd31a7dc96bb10e78f5e67bb9fb4e79821192e398c1630e22dcc24d906de`.
- `show_weapon_disp` declares its signed-byte loop counter before the other
  locals. This restores the original `CMP BL,[0x0059CAB0]` and `JL` at the loop
  tail, making the routine exact. `target_locking` declares `x` before `y`;
  its squared-distance calculation then sign-extends the coordinates into
  the original registers. No other cockpit report row changes. Moving the
  squared-radius local through the declarations instead leaves that routine
  unchanged.
- `ship.c` and `brains.c` see `nYourWingman` after the targeting-state
  declarations, immediately before `bEngageAllowed`. This makes
  `send_appropriate_message` and `analyze_kill` exact, and reduces
  `imperial_formation` from 107 differing encodings to three at its unchanged
  span. `cruise_home` and `inflict_damage` each lose one differing encoding.
  Moving the shared declaration for every unit instead regresses `try2rout`
  and `new_space_music_changes`, so the retained order is specific to these
  two owners. Storage, linkage, types, and the other owners' declaration
  order stay unchanged. These are measured matching contexts, not recovered
  original header text.
  Moving the private maneuver-proximity limit among the existing file-scope
  declarations does not repair `Mveer_away`; those probes are discarded.
  Moving the sound owner's playing-list declaration to later type/prototype
  boundaries likewise leaves its remaining comparison unchanged or regresses
  the start/stop routines; the sound header is unchanged.
  These are the only seven changed rows in the full report: normalized exact
  functions rise from 1,331 to 1,335 out of 1,550, with fourteen size
  differences remaining. Strict whole-file matching rises from 434,707 to
  435,090 positions out of 518,656 (83.8141% to 83.8880%); 83,566 positions
  still differ. No exact match is lost, and compilation-unit boundaries and
  link order are unchanged.
  Binary-comp comparisons of the raw PE sections, whose file offsets and
  sizes agree, establish exact equality for `.rdata` (7,168 bytes), `.data`
  (68,608), and `.idata` (4,096). The remaining differing file positions are
  67,139 in `.text`, 16,425 in `.reloc`, and two in the headers. The final
  106,040 raw bytes of `.text` already match continuously. These are literal
  file comparisons, separate from normalized function matching.
  The clean native and modern builds, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `ec32b209895cdaf557c9ad4cd678318167882616330a2b20faf5b55b6eec628b`.
- Recover the navigation and interactive-calibration spans together. A real
  label pointer for `PlaceNavMapLabel`'s two initial stores restores its
  336-byte span. Declaring `upScale` before `leftScale` and `verticalRange`
  after `rightScale` restores `CalibrateJoystickInteractive`'s 1,040-byte
  span. Their previous sixteen-byte errors canceled in the total code size
  while displacing the code between them. This supersedes the earlier
  decision to discard the label-pointer candidate in isolation.
  The full report changes only these two routines and `DrawNavTriangleOutline`,
  whose register choices gain ten differing encodings. Normalized exact
  functions fall from 1,335 to 1,334 of 1,550, while size differences fall
  from fourteen to twelve. Swapping placement's width/offset declarations,
  or introducing a short or int label index at any of the three local
  declaration positions, retains the size error; these probes are discarded.
  Strict whole-file matching rises by 11,224 positions, from 435,090 to
  446,314 of 518,656 (83.8880% to 86.0520%). The remaining 72,342 differences
  comprise 56,212 in `.text`, 16,128 in `.reloc`, and two in the headers.
  `.rdata`, `.data`, and `.idata` remain literally exact. The relocation
  section's virtual-size excess falls from four bytes to two; raw section
  sizes, compilation-unit ownership, and link order remain unchanged.
  This percentage counts equal positions throughout the file: the continuous
  matching prefix is only 292 bytes, with the first difference at offset
  `0x124`. The final 106,040 raw bytes of `.text` still match continuously.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `d58c403989c82c5d1f8991fb8d9b9110fe2a0759700b9a47e76514fd5da319fb`.
- Recover launch and calibration-loader sizes as another pair. Declaring
  `fighterScreenY` before `approachScale` restores
  `ShowCarrierLaunchSequence`'s 2,064-byte aligned span, with 95.98% mnemonic
  similarity. In `LoadJoystickCalibrationFile`, capture the two division
  results as `leftScale` and `upScale`, declared after `rangeX`. Publish them
  as the left/up scales, and reuse the unadjusted results for the right/down
  scales before their separate zero corrections, as the original does.
  This restores its 784-byte span, although mnemonic similarity falls to
  93.90%. The declaration-count change also improves `ReadCalibratedJoystick`
  from 92.56% to 96.69% and from 74 differing encodings to 73.
  These are the full report's only three changed rows. All 1,334 normalized
  exact matches remain, and size differences fall from twelve to ten; all
  remaining reported size differences are in the audio library, including
  the documented 26-byte sample-converter/import boundary artifact.
  Moving the left-scale global among seventeen nearby declaration positions
  leaves the loader's size error. Qualified conditional scale stores and
  explicit scaled ranges likewise leave the excess; those probes are not
  retained. Launch frame-local moves can also restore its aligned size, but
  leave more differing encodings than the retained fighter-Y declaration.
  Strict whole-file matching rises by 19,339 positions, from 446,314 to
  465,653 of 518,656 (86.0520% to 89.7807%). The remaining 53,003 differences
  comprise 38,955 in `.text`, 14,046 in `.reloc`, and two in the headers.
  The code section now matches at 90.2954%; `.rdata`, `.data`, and `.idata`
  remain literally exact. All raw section sizes still agree; the relocation
  section's virtual size is now two bytes short. Compilation-unit ownership
  and link order are unchanged.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `7209e0be09ccd451f325b8343dd1fe682fa473556b4dc3689a6a9c3e5d4602c0`.
- `new_view` declares its temporary vector inside the view-selection switch,
  where all its uses occur. This reverses the three vector-comparison loads
  to the original order, removes two body bytes, and restores the jump-table
  position. The normalized report becomes exact. A separate raw comparison
  of the 1,776-byte span still finds six differing bytes: the pitch/roll
  reset stores are reversed. Write both reset groups in roll, yaw, pitch
  order, matching the original. Binary-comp then verifies literal equality
  of all 1,776 bytes, including the jump table and padding. Normalized
  `EXACT` alone does not establish literal address operands or store order.
  Moving the up-vector global instead also fixes `new_view` but worsens
  `set_eye_direction_and_position` from 120 to 420 differing encodings.
  That declaration move is discarded. Swapping the two locals has no effect;
  narrower vector scopes recover the comparison order without the regression.
  Scramble-walk local and frame-counter declaration probes do not improve
  literal file matching. A qualified escape read also disturbs later scene
  sizes. The indexed and direct-table forms of `players_flight_dynamics`
  reduce its differing encodings from 55 to nine, but their relocation
  changes reduce whole-file matching, so those probes are also discarded.
  Only `new_view` changes in the final report: normalized exact functions
  rise from 1,334 to 1,335 of 1,550, with the same ten size differences.
  Strict whole-file matching rises by 1,138 positions, from 465,653 to
  466,791 of 518,656 (89.7807% to 90.0001%). There are still 51,865 differing
  positions; complete executable equality remains unachieved. Compilation-unit
  ownership, link order, and shared declarations are unchanged.
  A raw audit of 1,130 normalized-exact C function spans finds fourteen that
  still differ at their original file positions. Follow-up comparisons confirm
  a one-byte audio-call address shift in `ReleaseFinishedSoundEntries` and
  eight operand-address bytes from reordered axis-error terms in
  `rotate_eye_to_goal`. These remain candidates for subsequent matching work.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `4652c92897708dc191aa338828128087e6714233ad1148f0a3fda81c00703946`.
- Resolve three literal operand/store-order differences hidden by normalized
  `EXACT`. `CopyTrainSimPilotViewToRightVdu` assigns the right edge before
  the top edge, restoring the original two reads and stores across its
  complete 160-byte span. `DIBmakeDIB` assigns the pixel-buffer result to
  `stScreenView.allocation` before copying it to `pixels`, restoring both
  stores and its complete 448-byte span. `replenish_weapon_energy_bank`
  starts both shield sums at element zero and adds element one; CL commutes
  these into the original element-one-first loads. Its full 160-byte span
  is now literally exact as well.
  Reordering `check_for_lost_control`'s absolute-value numerator reduces
  its 192-byte span from eight literal differences to six. The sum remains
  equivalent; its remaining axis and rate operands are still reordered.
  All six axis-term permutations were checked in both rotation-goal routines
  and the loss-of-control routine. Neither rotation-goal routine improves.
  Reversed heap sums/subtractions, objective and pilot comparisons, and the
  wave-index sum likewise leave the emitted bytes unchanged. Local-pointer
  probes in the barracks-label and string-reset helpers, and declaration
  swaps in rectangle-area and wave-entry removal, also have no effect.
  These ineffective probes are discarded.
  All 1,550 normalized report rows remain unchanged: 1,335 exact functions,
  205 same-size differences, and ten size differences. Raw binary-comp
  comparisons establish the improvements that normalization does not expose.
  Strict whole-file matching rises by twelve positions, from 466,791 to
  466,803 of 518,656 (90.0001% to 90.0024%); 51,853 positions still differ.
  The continuous matching prefix remains 292 bytes, with the first difference
  at file offset `0x124`. Compilation-unit ownership, shared declarations,
  and link order remain unchanged.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `6b94832b97d4e99507ebd5ae41c21d219c99d772c735b0b00f569bcff3f6eb1c`.
- Stream and voice controls omit the unused sound-system API prototypes from
  their `ix.h` declaration scopes. Their respective `nStreamCount` and
  `nVoiceCount` declarations precede the voice types. Other owners retain
  their previous declarations. These are measured compiler contexts, not
  recovered original header text; function bodies, types, storage, compiler flags,
  and link order are unchanged.
  `ix_dsps_free` and `ix_dsps_unlock` become exact over their complete 279-
  and 292-byte spans. All ten stream-function sizes now agree. In voice
  control, `ix_dspv_set_bits_per_sample` and `ix_dspv_recalc_mix` become exact
  over 201 and 181 bytes, including the latter's original object-end padding.
  Separate raw binary-comp comparisons verify literal equality at the
  original file positions for all four routines.
  Three normalized matches regress: `ix_dsps_alloc` has 116 differing
  encodings, `ix_dsps_stop` has 51, and `ix_dspv_set_position` is one byte
  short. The allocation and stop spans remain correct because their bounds
  checks and voice-index sums change in opposite directions. These tradeoffs
  replace displacement across many following routines with fewer literal
  differences overall. At original file positions, stop improves from 233
  differing bytes to 184, and voice-position setting from 108 to 26; allocation
  regresses from literal equality to 405 differing bytes.
  A raw audit of all 24 stream/voice routines finds twelve complete spans
  literally exact, versus one before. The seven changed normalized report
  rows gain one exact match overall, from 1,335 to 1,336 of 1,550. Same-size
  differences rise from 205 to 207; size differences fall from ten to seven.
  No other report row changes.
  All seven combinations of unused DSP, system, and streamer API groups
  were checked in the two control owners. Removing the streamer APIs from
  the sound owner regresses literal matching. Eighteen count-declaration
  positions per control owner and eight further stream-owner voice-count
  positions were checked; none of the additional placements improves on the
  retained result. Ineffective and regressing probes are discarded.
  Strict whole-file matching rises by 3,030 positions, from 466,803 to
  469,833 of 518,656 (90.0024% to 90.5866%); 48,823 positions still differ.
  The identical prefix remains 292 bytes, with the first difference at
  offset `0x124`. Clean native and modern builds, source/address audits,
  the order report, and full verification pass. Both value checks cover
  all 1,550 functions with zero mismatches or skips; existing call/global-access
  diagnostics remain 11/56. Clean-build and post-verification hashes agree:
  `55504d9fe3cb2774aca34a9c01d0d4ba3dfaff415573ea9a544411f1bf36687f`.
- Narrow the shape-frame locals to the valid-frame branch in
  `GetShapeFrameCorners`. In `GetTransformedShapeBounds`, retain
  `absoluteCosine`, `frameOffset`, and `intersects` at function scope in
  that order, and move the remaining frame locals into the valid-frame
  branch. These measured declaration scopes improve register allocation;
  they do not establish the original source spelling. Types, expressions,
  calls, compilation-unit ownership, and link order remain unchanged.
  Frame-corner mnemonic similarity rises from 90.70% to 92.86%, retaining
  26 differing encodings and its 144-byte span. Transformed bounds rises
  from 86.58% to 95.30%, reducing differing encodings from 108 to 101 and
  retaining its 432-byte span. Separate raw binary-comp comparisons at the
  original positions improve from 68 to 71 equal bytes for frame corners
  and from 141 to 201 for transformed bounds. Neither span is literally
  exact; the normalized mnemonic percentages measure a different property.
  Other extent, coefficient, dimension, pointer, and local-scope permutations
  do not improve on these candidates. A separate `AllocateViewport` probe
  combines the original signed pretest loop, row-table reload, and earlier
  offset initialization with eight local layouts. All eight add sixteen
  bytes and regress whole-file matching, so those probes are discarded.
  Only the two shape-function rows change in the full report. Normalized
  exact matches remain 1,336 of 1,550, with 207 same-size differences and
  seven size differences. Functions below 90% fall from five to four.
  Strict whole-file matching rises by 63 positions, from 469,833 to
  469,896 of 518,656 (90.5866% to 90.5988%); 48,760 positions still differ.
  This counts equal positions throughout the file. The continuous matching
  prefix remains 292 bytes, with the first difference at offset `0x124`.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. Clean-build and post-verification hashes agree:
  `43fdbe15ecaad4ccb7fc8903fc8eb3e16be769b600b69d209a88bcde9c7945df`.
- The logic owner declares `nTgtRange` immediately before `nTargetShip`.
  Other owners retain their previous declaration order. This restores
  `in_danger`'s complete 176-byte span, improves `attacker_in_range` from
  85.42% to 95.83% mnemonic similarity and from 36 differing encodings to
  eleven, and reduces `scan_for_enemy` from four differing encodings to two.
  `close_behind` regresses from exact to two differing bytes in its unchanged
  32-byte span. Raw matches across these four routines increase by 75:
  32 to 30, 232 to 238, 83 to 144, and 166 to 176 bytes respectively for
  close-behind, enemy scanning, attacker range, and danger detection.
  Twelve range-declaration positions in the maneuver owner and twenty-four
  in logic were checked. The closest alternative gains 71 positions without
  regressing close-behind, but the retained placement matches four more file
  positions overall. Removing close-behind's volatile parameter qualifier,
  or replacing it with const, adds sixteen bytes; neither probe is retained.
- Voice control omits unused sample/sound records and state, stream records
  and interfaces, and file-streaming records, interfaces, and state from its
  `ix.h` scope. It retains the stream count used by mix recalculation. Other
  owners retain their previous declarations; function bodies, record layouts,
  storage, flags, and link order are unchanged. These are measured compiler
  contexts, not recovered original header text.
  `ix_dspv_set_active` loses its excess byte, restoring the original positions
  of the first seven voice routines. Raw binary-comp comparisons establish
  literal equality of all seven complete spans, the consecutive 731 bytes
  at `0x004467C5`–`0x00446A9F`. Exact voice spans increase from four to
  nine of fourteen. There are tradeoffs: bit-depth setting gains one byte,
  and position, pan, frequency, and bit-depth setting lose literal matches.
  Frequency still reports normalized exact despite its shifted position.
  The voice change adds 307 matching file positions overall.
  All seven combinations of the unused declaration groups and eighteen
  further voice-count declaration positions were checked. None improves on
  the retained combination. Separate dot-product expression permutations
  leave its call order unchanged. A staged sum makes that routine exact but
  disturbs two rotations; their local-order permutations do not recover a
  whole-file gain. Scoping the scene-command opcode inside its loop also
  leaves its output unchanged. These body probes are discarded.
  The combined report changes only six rows: 1,336 of 1,550 normalized exact
  functions, 207 same-size differences, and seven size differences remain.
  Mean mnemonic similarity rises to 99.64%; four functions remain below 90%.
  Strict whole-file matching rises by 382 positions, from 469,896 to 470,278
  of 518,656 (90.5988% to 90.6724%); 48,378 positions still differ. The
  continuous matching prefix remains 292 bytes and the first difference
  remains at file offset `0x124`.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The post-verification executable retains its build hash:
  `48fe9cfc26a799dca18a207be0c9d84c29f129232e67a5b22a8293984ae94652`.
- `DrawTextString` declares its right boundary before the accumulated line
  width. This restores both comparison operand orders and their branches,
  making the complete 416-byte span literally exact at its original position.
  Moving the boundary earlier among the other declarations or into the loop
  also works, but the adjacent declaration swap is sufficient. Reversing
  the source comparisons alone leaves the four differing bytes unchanged.
  The swap byte in `SortSignedByteValuesAscending` is declared first at
  function scope and assigned inside the inner loop. Its swap comparison
  now matches, raising mnemonic similarity from 95.35% to 97.67% and raw
  matching from 108 to 110 of 112 bytes. The inner-loop entry comparison
  retains two differing bytes. Other outer-local permutations, placing the
  swap byte last or in the outer loop, and testing the saved next index do
  not improve the retained result.
  `shrink_vector` explicitly accumulates its three component-call results in
  the original X/Y/Z order, repeating while any component shrinks. Its
  64-byte span remains unchanged; mnemonic similarity rises from 92.31% to
  100%, differing encodings fall from eleven to nine, and raw matching rises
  from 50 to 55 bytes. Register allocation remains different. Local-scope
  and loop-form probes in `RetainInputEventsOfType` leave its bytes unchanged.
- The sound-system owner omits unused stream records, state and APIs, and
  file-streaming records and APIs from its `ix.h` scope. Other owners retain
  their declaration contexts. This restores the two sample-rate equality
  comparisons in `ix_system_assign_voice`, removing its four normalized
  differences. Raw matching rises from 544 to 548 of 551 bytes: three call
  displacements still differ because the volume, frequency and bit-depth
  routines remain shifted. Normalized exactness does not establish literal
  equality of this span.
  Other record/interface exclusions in stream, sound and system owners do
  not improve on this result. Windows SDK lean-header profiles regress
  literal matching; additionally disabling USER and GDI declarations fails
  to build these owners. Voice-interface parameter-name removal and
  address-order declarations leave output unchanged, while limiting its
  API to the required forward declaration or omitting its own state externs
  regresses matching. All unused or regressing probes are discarded; SDK
  headers and compiler flags remain unchanged. These are measured compiler
  contexts, not recovered original header text.
  Only these four rows change in the full report: normalized exact functions
  rise from 1,336 to 1,338 of 1,550, same-size differences fall from 207 to
  205, and seven size differences remain. Mean mnemonic similarity remains
  99.64%, with four functions below 90%. All function spans and compilation-
  unit placements are unchanged.
  Strict whole-file matching rises by fifteen positions, from 470,278 to
  470,293 of 518,656 (90.6724% to 90.6753%); 48,363 positions still differ.
  This counts equal positions throughout the file. The continuous matching
  prefix remains 292 bytes, with the first difference at offset `0x124`.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `119342bd62e36cf91eefacb0cec210b73a873f5214802b5210ac29c11cfb50de`.
- `BarracksScreen` declares its scalar locals as `lastMedalsTick`, `clicked`,
  `result`, `background`, `eventType`, and `region`, following its two records.
  This restores the original allocation of the four values live across calls.
  Its shared activation label and switch bodies remain unchanged. Mnemonic
  similarity rises from 95.54% to 97.76%, differing encodings fall from 219
  to 30, and raw matching rises from 93 to 831 of 928 bytes. Duplicating the
  click assignment instead of sharing the label adds sixteen aligned bytes;
  moving the cursor case does not help. Those switch probes are discarded.
- `RunSpaceFlight` converts each timestamp's `QuadPart` to `unsigned int`
  before subtracting it. This preserves the existing low-32-bit subtraction
  and wrap behavior, while allowing CL to reuse the low words between the
  six delta stores. Reading the `LowPart` union members separately instead
  leaves six redundant absolute loads. The routine now has the original
  74 address relocations, down from 80, with its 912-byte span unchanged.
  Mnemonic similarity rises from 96.89% to 99.11%, differing encodings fall
  from 77 to 61, and raw matching rises from 638 to 697 bytes. Plain local
  captures do not recover this reuse and disturb the following cursor helper;
  signed/unsigned-long casts and swapping the two existing locals do not
  improve the retained result. These alternatives are discarded.
- `ShowCampaignVictorySequence` selects subtitle indices through a switch
  on its short frame counter, matching the original's single promoted read
  and case branches. The former volatile qualification is unnecessary.
  Its projectile pointer follows `slot` in the declarations, improving the
  initialization and rendering loops' register allocation. The planet-overlay
  draw uses `projectileShape`, the packet loaded from file 9, item 2: the
  original pushes that pointer from ESI. `planetShape`, from item 3, remains
  the shape passed to `CreateCannedSceneObject`. The earlier reconstruction
  incorrectly passed that first pointer to the overlay as well.
  Mnemonic similarity rises from 92.35% to 96.99%, differing encodings fall
  from 314 to 154, and raw matching rises from 238 to 843 of 1,360 bytes.
  Moving the other frame, shape and vacant-slot declarations does not improve
  the retained result. These source forms reproduce observed code generation;
  the original local names and declaration order are not established.
  The flight and victory changes must be assessed together. In isolation
  they lose 210 and 521 matching file positions respectively, despite
  improving their instruction comparisons. Together they gain 1,277 because
  relocation-block padding and offsets interact. Victory restores the
  original 141/239 relocation counts on code pages `0x0042F000` and
  `0x00430000`, versus 140/240 before. The final raw audit attributes all
  1,402 additional matching code bytes to these three functions and another
  652 to `.reloc`; `.rdata`, `.data` and `.idata` remain literally exact.
  A separate relocation inventory identifies ten remaining core functions
  with count differences: label placement, interactive calibration, shape
  animation, missile-display animation, the two internal-damage routines,
  player flight dynamics, flight controls, release-weapon selection, and
  calibration-file loading. Counts guide investigation; binary-comp remains
  the authority for matching. Snapshot typing, local scopes and viewport-
  pointer captures in `InflightComputer` do not improve it. Mixed pointer
  and indexed stores in label placement add sixteen bytes. Removing the
  preferred-weapon search's redundant guard or comparing its index directly
  leaves output unchanged; a qualified count reload makes it worse. These
  probes are discarded.
  Only these three rows change in the full report. Normalized exact functions
  remain 1,338 of 1,550, with 205 same-size differences and seven size
  differences. Mean mnemonic similarity rises to 99.65%; four functions
  remain below 90%. All function spans and compilation-unit placements are
  unchanged.
  Strict whole-file matching rises by 2,054 positions, from 470,293 to
  472,347 of 518,656 (90.6753% to 91.0713%); 46,309 positions still differ.
  This counts equal positions throughout the file. The continuous matching
  prefix remains 292 bytes, with the first difference at offset `0x124`.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `8dd267a6c12c7eb1cf5ccf3c99005da27364beec689de21ca761fa2a122eeb0c`.
- `players_flight_dynamics` indexes `aObjectTypeData` with a captured object
  type instead of retaining a pointer to the record. The original retains
  the record's byte offset and uses absolute field displacements for pitch,
  yaw and roll; the pointer form folds the table base into that offset.
  Indexed access restores all 24 address relocations, versus 22 before,
  and reduces differing encodings from 55 to nine. Raw matching rises from
  56 to 231 of 256 bytes. Instruction scheduling still differs, so the
  mnemonic score falls from 100% to 96.77% despite the improved bytes.
  Int-index and direct-index spellings produce the same result. Earlier
  rejection of the indexed form predates the intervening relocation-layout
  corrections; its current whole-file effect is positive.
- `your_internal_damage` qualifies the weapon-count guard as a volatile
  signed-byte read. The original compares the byte in memory, then reloads
  and sign-extends it when preparing the random weapon bound. An ordinary
  read lets CL retain AL across the guard and omit the second absolute
  reference. The qualification restores the original 30 relocations and
  makes all 944 bytes of the function's aligned span literally exact,
  improving from 606 matching bytes and 117 differing encodings. It
  constrains the observed access sequence without establishing that the
  original global declaration was volatile.
  The other damage routine has a separate opportunity: a capital-ship
  accumulated-damage pointer shared across both side branches eliminates
  a duplicated address calculation. Combined with the original shield-store
  order, signed-byte weapon count, narrowed random index and fall-through
  return, it reduces differing encodings from 225 to 53. Removing its extra
  relocation currently shifts later relocation blocks away from the
  original; even paired with the player-damage correction, it loses 4,461
  matching positions against the retained build. These probes are not
  retained. In `animate_shape`, separate branch-local scale changes and
  qualified scale stores leave the merged store and executable unchanged.
  Only the two retained function rows change in the full report. Normalized
  exact functions rise from 1,338 to 1,339 of 1,550, same-size differences
  fall from 205 to 204, and seven size differences remain. Mean mnemonic
  similarity stays 99.65%, with four functions below 90%. All function spans
  and compilation-unit placements are unchanged.
  Strict whole-file matching rises by 7,608 positions, from 472,347 to
  479,955 of 518,656 (91.0713% to 92.5382%); 38,701 positions still differ.
  Of the gain, 513 positions are in `.text`, 7,094 in `.reloc`, and one in
  the PE headers. The `.rdata`, `.data` and `.idata` sections remain literally
  exact. This counts equal positions throughout the file. The continuous
  matching prefix grows from 292 to 544 bytes; the first difference is now
  at file offset `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `9807fa4c8092c9023b84698a95b4a8c80f8e3ddf849a49c933363769f6cc42d5`.
- The two changing-mode calls in `HandleSpaceFlightControls` need the same
  explicit increment, signed-byte promotion and int subtraction already
  recovered in `RunSpaceFlight`. The original increments and stores the
  mode byte before promoting the updated value and subtracting one for
  `initialize_cockpit`. A post-increment argument instead passes the old
  byte and emits a different sequence. Separating those operations reduces
  differing encodings from 781 to 593 and raises mnemonic similarity from
  96.32% to 96.83%. Raw matching rises from 346 to 1,149 of the unchanged
  3,184-byte span. No other function-report row changes.
  Explicit wingman and selected-weapon captures, a qualified communication-
  key guard, and a missile-camera value capture were also checked. Individual
  captures either reduce whole-file matching or add sixteen aligned bytes.
  Their combination with the mode correction improves only 32 file
  positions and disturbs `FindNearestNavPoint` and `UpdateTrainSimMenuCursor`,
  so the mode-only correction is retained. The handler still has 176 address
  relocations versus the original 174.
  In `LoadJoystickCalibrationFile`, a qualified minimum-Y reload, qualified
  Y-bound accesses, and products retained in the two range locals with
  qualified scale reloads all add sixteen aligned bytes, separately or in
  combination. These probes are discarded. Reversing the square marker's
  Y-plus-size expression, swapping the navigation-clock local declarations,
  and reversing the first rectangle-union minimum expression leave their
  output unchanged and are likewise discarded.
  The full report retains 1,339 normalized exact functions of 1,550, with
  204 same-size differences, seven size differences and 99.65% mean mnemonic
  similarity. Four functions remain below 90%. Function spans and
  compilation-unit placements remain unchanged.
  Strict whole-file matching rises by 803 positions, from 479,955 to
  480,758 of 518,656 (92.5382% to 92.6930%); 37,898 positions still differ.
  All of the gain is in this handler's code. Relocation-section matching
  remains 30,038 of 36,352 bytes, and `.rdata`, `.data` and `.idata` remain
  literally exact. The continuous matching prefix is still 544 bytes,
  with the first difference at file offset `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `cda0f3c5bc8718e83ae7fad7f58eda2e3594a1dd91d054865b94333d588e3201`.
- `HandleSpaceFlightControls` handles an existing communication message
  before querying the VDU mode. With no message, it selects mode four on
  the unequal path and closes the choice menu on equality. This recovers
  the original call/branch ordering in key case `0x2e`. Mnemonic similarity
  rises from 96.83% to 97.59%; the 593 differing encodings and 1,149 literal
  matching bytes of its 3,184-byte span remain unchanged. Reversing only
  the outer branch loses one matching file position; both branches are
  retained together.
- Qualifying `ix_dspv_set_volume`'s voice-count read as volatile changes
  CL 4.20's later comparison choices without changing the setter's compared
  bytes. In the resulting unit, `ix_dspv_set_pan` regains its missing byte
  and `ix_dspv_set_bits_per_sample` loses its extra byte. Their complete
  293- and 201-byte spans now match literally. The intervening frequency
  setter returns to its original address and all 123 of its bytes match
  as well. Raw matches rise from 45/293, 179/201 and 15/123 respectively.
  Twelve of fourteen voice routines are now literally exact; the position
  and volume setters still have compensating minus-one/plus-one size errors.
  This qualification is a measured compiler-context choice. The original
  global's volatile declaration is not established, and the global storage,
  public declarations, calling conventions and flags remain unchanged.
  Qualifying the position setter's upper-bound parameter read produces the
  same improvement. Moving the count qualification earlier, into either
  the position getter or buffer setter, makes the position setter exact
  but leaves the volume setter's extra byte, shifting later code until the
  final object's padding absorbs it. That alternative loses 915 matching
  file positions and is discarded. Reversed guard operands, moving or
  scoping the existing pointer declaration, and const or volatile pointer
  qualifications do not improve the output. VC 4.20 rejects a volatile
  parameter definition against its unchanged ordinary prototype; that
  failed probe is also restored without changing the interface.
  Only these three normalized report rows change. Exact functions rise
  from 1,339 to 1,341 of 1,550, same-size differences remain 204, and size
  differences fall from seven to five. Mean mnemonic similarity remains
  99.65%, with four functions below 90%. Compilation-unit placements and
  the total executable size remain unchanged.
  Strict whole-file matching rises by 392 positions, from 480,758 to
  481,150 of 518,656 (92.6930% to 92.7686%); 37,506 positions still differ.
  The gain is 381 code bytes and eleven relocation bytes. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The continuous
  matching prefix remains 544 bytes, with the first difference at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree, including
  after restoring the final discarded probes:
  `87ed227524654c6cbd80de1ad701d9a4371bc8e42f264cbe8d81f17c26f87387`.
- `LoadMissionData` copies the initial-ship words with post-increments in
  the assignment. The original advances the source before storing through
  the destination, then advances the destination. Keeping both increments
  in a `for` update instead adds a displacement to the store and shifts
  most of the remaining body by one byte. Declare `initialShip` before
  `sourceInitialShip`, `ship` before `diskShip`, and `index` before
  `diskObjective` to recover the original pointer and index registers.
  These changes reduce differing encodings from 209 to 16 and raise
  mnemonic similarity from 95.73% to 98.29%. Literal matching rises from
  120 to 758 of the unchanged 784-byte span. The remaining differences
  concern the preload-type loop's registers and instruction scheduling.
- `FadeViewportPaletteToColour` skips the palette work when the video mode
  differs from `0x13`, then calls both `DIBslam` and `DIBslamReal`. Its old
  early return incorrectly skipped those calls. Enclose the palette work
  in the mode-equality branch, retaining the separate allocation-failure
  returns. Declaring `index` before `activeCount` also restores the original
  loop-comparison directions. Mnemonic similarity rises from 96.95% to
  98.17%; the 123 differing encodings and 137 literal matching bytes of
  its 496-byte span remain unchanged. The initial variable-length memset
  still emits an extra register copy, displacing the following code.
  Inlining the byte-count expression, either everywhere or only at the
  memset calls, adds sixteen aligned bytes and is discarded. Changing its
  signedness or moving its declaration does not improve the output.
  Reversing the mission loader's remaining loop updates slightly reduces
  literal matching and is likewise discarded.
  Bounded sound-reprioritization probes leave its compensating one-byte
  size errors or lose matching bytes. Interactive-calibration declaration
  changes and an explicit action-sphere class local regress the file;
  wave-list declaration order and a complete bunk-row coordinate leave it
  unchanged. None of these probes is retained.
  Only the mission loader and palette routine change in the full report.
  The totals remain 1,341 normalized exact functions of 1,550, 204 same-size
  differences, five size differences and 99.65% mean mnemonic similarity.
  Four functions remain below 90%. Compilation-unit placements, function
  spans and relocation counts are unchanged.
  Strict whole-file matching rises by 656 positions, from 481,150 to
  481,806 of 518,656 (92.7686% to 92.8951%); 36,850 positions still differ.
  The gain is 638 code bytes and eighteen relocation bytes. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The continuous
  matching prefix remains 544 bytes, with the first difference at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `35f0771b9a9547fb3e963562c0faa0026037ec661d76c2387c8dd5518bc89a96`.
- `select_new_release_weapon` captures the selected index once, tests that
  register, and reloads the loadout count for both comparisons in its
  preferred-type search. Qualify the initial selected-index read and those
  two signed-byte count reads to retain the observed access sequence.
  The count guard compares against the running index, already zero, rather
  than a literal zero. Declare `weapon` before `weaponCount` to restore the
  three remaining comparison pairs. The complete 336-byte span now matches
  literally, up from 50 matching bytes and 85 differing encodings, and its
  relocation count returns from fourteen to the original fifteen.
  Qualifying only the index capture loses sixteen aligned bytes; qualifying
  only the count comparisons leaves the index reload and worsens file
  matching. The pair must be assessed together. These access qualifications
  do not establish that the original global declarations were volatile.
- `ReadPacketSectionData` declares `end` before `offset`. CL then retains
  the destination in EAX and adds the data size into ECX, matching all 96
  bytes of the routine, up from twelve matching bytes. Computing the end
  first instead leaves seven differing encodings, while a separate data-
  offset local leaves the old output unchanged; neither is retained.
- `house_keep` captures the space-frame count and refreshes it after
  `ReleaseStaleNavTarget`. Reuse that short for the later palette loop.
  This recovers the original CX/CL reads and tests, making all 176 bytes
  literal matches, up from 27. A separate frame local also makes this
  routine exact but disturbs `UpdateTrainSimMenuCursor` and the palette
  fade's loop comparisons. Reusing the existing counter preserves both
  later routines and adds no local.
  Declaring `weaponSlot` before `found` in `select_guns` restores their
  original registers: differing encodings fall from fifteen to seven and
  literal matching rises from 97 to 105 of 112 bytes. The other declaration
  permutations do not finish that routine. Putting the slot initializer
  first in `find_next_gun` regresses file matching and is discarded.
  Declaring `entryCount` before `entry` in
  `ReleaseContiguousPaletteEntries` gains one literal byte, from 55 to 56
  of 64. Eight differing encodings remain, and mnemonic similarity falls
  from 100% to 94.12%; the retained choice follows the literal-byte goal.
  Animation-local declaration changes, rectangle-area declaration order,
  an explicit first-left capture in the rectangle union, and an explicit
  formatted-range exit do not improve their output and are discarded.
  Only these five function rows change in the full report. Normalized exact
  functions rise from 1,341 to 1,344 of 1,550; same-size differences fall
  from 204 to 201, and five size differences remain. Mean mnemonic
  similarity is 99.66%, with four functions below 90%. Function spans and
  compilation-unit placements are unchanged.
  Strict whole-file matching rises by 525 positions, from 481,806 to
  482,331 of 518,656 (92.8951% to 92.9963%); 36,325 positions still differ.
  Code gains 528 matching positions while relocation matching loses three.
  The `.rdata`, `.data` and `.idata` sections remain literally exact.
  The continuous matching prefix is still 544 bytes, with the first
  difference at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `8c398f464042fe324011eb7ac153cc63e2cac7cd554120268ebfe32ce2ca7c51`.
- `SelectTrainSimMission` keeps its four opponent shapes in a pointer array,
  ordered top left, bottom left, top right, bottom right. Three elements
  remain in registers and the third spills at stack offset `0x44`; the
  complete array explains the twelve bytes missing from the previous
  `0x40` frame. The rebuilt frame now matches the original `0x4c` allocation.
  Promote the signed-short key value to an int before its two comparisons.
  Likewise, retain the returned signed-byte region for the selection update
  while comparing its promoted int against zero and three. This preserves
  the original EAX result and separate ECX range check. An unsigned-short
  cancellation flag tested for equality to zero recovers the final SBB/NEG
  boolean sequence. All 1,040 bytes now match literally, up from 585;
  differing encodings fall from 168 to zero. Promoting the region without
  the key adds sixteen aligned bytes, so the related changes must be
  assessed together. Reusing the key local for the promoted region also
  matches, but separate names describe their uses more clearly.
- `PrintPaletteAllocationMap` declares row, column, then index. This assigns
  DI, BX and SI respectively, matching all 96 bytes, up from 84. The other
  declaration permutations leave differing registers.
  In `ShowTrainSimHighScores`, advance the line height after setting the
  display bounds. CL then schedules that addition after the viewport copy,
  as in the original. Differing encodings fall from 109 to 103, mnemonic
  similarity rises from 97.79% to 98.67%, and literal matching rises from
  501 to 520 of 880 bytes. Its two-byte displacement after the bounds copy
  remains. Advancing the pilot prompt's X parameter before passing it to
  `SetTextCursor` restores the parameter-first addition and gains one byte,
  from 68 to 69 of 96; eight differing encodings remain.
  An unsigned animation-scale temporary leaves its output unchanged.
  Explicit component-index and damage-byte locals, a captured goal limit,
  and earlier high-score context setup regress file matching. Reversing
  the prompt's addition operands or qualifying its viewport-left read does
  not improve it. These trials are not retained.
  Only these four function rows change in the full report. Normalized exact
  functions rise from 1,344 to 1,346 of 1,550; same-size differences fall
  from 201 to 199, and five size differences remain. Mean mnemonic
  similarity is 99.67%, with three functions below 90%. Function spans,
  relocation counts and compilation-unit placements are unchanged.
  Strict whole-file matching rises by 487 positions, from 482,331 to
  482,818 of 518,656 (92.9963% to 93.0902%); 35,838 positions still differ.
  All gains are in code. The `.rdata`, `.data` and `.idata` sections remain
  literally exact, and relocation-section matching remains 30,064 of
  36,352 bytes. The continuous matching prefix is still 544 bytes, with
  the first difference at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `b0e08a23e0f956e2c38d1d0ed64eef22c81ac293195b5bfe3db4b2bc1e852107`.
- `AddRandomTrainSimHighScores` tests `pilot <= 8` after excluding pilot
  eight. CL can then reuse the first comparison's flags for the original
  `JE`/`JG` pair; spelling the bound as `< 9` emits another comparison.
  Declare `pilot` before `scale` to recover their SI/DI allocation.
  Differing encodings fall from 68 to 24, mnemonic similarity rises from
  92.21% to 97.37%, and literal matching rises from 31 to 180 of 224 bytes.
  The stack counter's offset and the base-score/divisor register swap remain.
  Moving the counter or score declarations, initializing the counter in its
  declaration, and making that counter unsigned leave the selected output
  unchanged.
- `someone_coming` captures the current navigation-point byte before its
  search. The original already keeps that read in BL; the explicit local
  also recovers CX for the ship counter and EAX for its scaled index.
  Its complete 80-byte span now matches, up from 74. In
  `update_objective_location`, declaring `range` before `object` restores
  SI and BX respectively, matching all 224 bytes, up from 217.
  Cockpit-light declaration changes and an explicit promoted object index
  in `get_color` regress file matching. Reordering the target-box validity
  and horizontal-center locals does not change its output. These probes
  are not retained.
- `parse_view_script` tests the end marker before advancing the script
  pointer. The former source consumed `-1`; the original leaves the pointer
  on that marker while clearing the scripted-view state. Dispatch directly
  on the post-incremented script word, preserving the original reread after
  the marker checks. For opcode sixteen, capture the mission index in a
  short and assign the selected object inside the matching branch, without
  a second range test after the search.
  The initial hold-marker branch joins the return after the wait-counter
  store; a hold reached after executing commands uses the earlier return.
  The explicit exit label reproduces those original branch destinations
  and the final store/epilogue schedule. A single loop guarded by the hold
  marker leaves five differing encodings. With the shared exit, all 1,040
  bytes match literally, up from 75, and all 235 differing encodings vanish.
  Retaining the used short mission-index local also restores
  `CalcRectangleArea`'s allocation, making its entire 64-byte span exact,
  up from 60. Renaming that operand from `command` to `missionIndex` leaves
  the executable identical. These compiler-context effects do not establish
  the original local names.
  Only these five function rows change in the full report. Normalized exact
  functions rise from 1,346 to 1,350 of 1,550; same-size differences fall
  from 199 to 195, and five size differences remain. Mean mnemonic
  similarity is 99.68%, with three functions below 90%. No existing exact
  function is lost, and function spans and compilation-unit placements are
  unchanged.
  Strict whole-file matching rises by 1,142 positions, from 482,818 to
  483,960 of 518,656 (93.0902% to 93.3104%); 34,696 positions still differ.
  Code gains 1,131 matching bytes and relocations gain eleven. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The continuous
  matching prefix is still 544 bytes, with the first difference at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. Relocation counts within all five changed functions are
  unchanged. The clean-build and post-verification hashes agree:
  `2b60896813798730068f1903dc6f9b2b90bea9df63745193e667b019ed7d0b15`.
- `DrawScrambleFrame` draws when the decremented frame-skip counter is less
  than one, then reaches the unconditional return. The positive drawing
  guard recovers the original compare-to-one and forward `JGE`; an early
  return for a positive counter emits a different branch sequence.
  The Hornet cockpit-detail call passes the word at `0x005a86c0` as its
  angle and literal 255 as its scale. The former reconstruction passed
  angle zero and used that word as scale. Both the original pushes and
  `DrawSpriteScaled`'s argument reads establish the correct order.
  Rename the word to `nScrambleCockpitAngle`, retaining its declaration,
  definition, type and address annotations. The old and new common-symbol
  hash keys, 424 and 398, both sort between `pScrambleDetailShape` (105)
  and `stScrambleCockpitDetail` (473), preserving its storage position.
  A clean rebuild proves the rename alone leaves the executable identical.
  Guard the detail loop with its positive count before initializing the
  index, then use a do/while loop. Together these changes reduce differing
  encodings from 177 to two, raise mnemonic similarity from 96.55% to
  99.61%, and raise literal matching from 345 to 1,006 of 1,008 bytes.
  The remaining comparison reverses its operands and uses `JG` instead of
  `JL`; moving the detail locals does not change it. The unit's compiler
  context also gains three literal bytes in `funeral_sequence`, from 144
  to 147 of 1,792; its differing encodings fall from 411 to 410 and its
  mnemonic similarity rises from 96.80% to 97.71%.
- In `EmitTextString`, spell the guarded character loop as an explicit
  nonzero test followed by do/while. Its own 48 bytes are unchanged, but
  this restores one encoding in the later scanner. Declaring `row` first
  and `grid` after `object` in `draw_3d_scanner` restores ESI for the row
  and EDI for the grid pointer. Together they make all 736 scanner bytes
  exact, up from 724. Moving only the grid declaration leaves one byte
  different. Cursor/callback locals in the text emitter provide no further
  improvement and are discarded.
  Qualified escape reads and a delayed second escape read in
  `AnimateScrambleWalk` regress layout or literal matching. An explicit
  decremented-counter local does not improve that routine and loses two
  file positions when combined with the selected renderer. Qualifying its
  counter read also regresses the file. None of these trials is retained.
  All other declaration permutations in `ReserveContiguousPaletteEntries`
  either leave its output unchanged or lose six matching bytes.
  Only three function rows change in the full report. Normalized exact
  functions rise from 1,350 to 1,351 of 1,550; same-size differences fall
  from 195 to 194, and five size differences remain. Mean mnemonic
  similarity is 99.68%, with three functions below 90%. No existing exact
  function is lost, and function spans and compilation-unit placements are
  unchanged. Relocation counts within the affected functions are unchanged.
  Strict whole-file matching rises by 741 positions, from 483,960 to
  484,701 of 518,656 (93.3104% to 93.4533%); 33,955 positions still differ.
  Code gains 676 matching bytes and relocations gain 65. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The continuous
  matching prefix is still 544 bytes, with the first difference at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `079ee72d4590f077a32ae540210ee4f7048db3bb01f0dd97f1fcc3a90535afb6`.
- `funeral_sequence` declares its primary scene and text pointers before the
  follow-up scene and text pointers. This restores EDI/EBP for the primary
  pair and the two stack slots for the follow-up pair, including the original
  scheduling while extracting their packet offsets. All 1,792 bytes now
  match literally, up from 147; all 410 differing encodings disappear.
  Interleaving the primary and follow-up declarations, moving only the
  follow-up text pointer, or exchanging only the primary scene/text pointers
  does not improve literal matching.
- `mine_intelligence` captures its promoted object index in an int declared
  after the two short locals. Use that index for the counter test and object
  type lookup; keep the short object parameter for calls and comparisons.
  This restores EBX for the promoted index and DI for the other-object
  counter, matching all 144 bytes, up from 135. Declaring the promoted index
  first leaves the mine routine unchanged. Both positions improve three
  bytes in `heat_seeking_missile_intelligence`, from 295 to 298 of 320,
  reducing its differing encodings from seventeen to fourteen.
- `fire_computer_graphic_missile` captures its background pointer separately
  on the two restore paths, after the restore call on the path that makes
  one. Use the captured pointer for the later background capture. This
  reproduces the original duplicated load and restores its missing HIGHLOW
  relocation: 32 references now match the original count, up from 31.
  All 336 bytes match literally, up from 125; all 58 differing encodings
  disappear. An explicit frame local also matches this routine but disturbs
  two exact neighbors. Keeping only the background local preserves those
  matches. A separate shape local leaves seven differences and is discarded.
  The retained compiler context loses four literal bytes in `get_color`,
  from 216 to 212 of 224, while `draw_target_box` stays at 147 of 1,008.
  The latter's mnemonic similarity rises from 93.54% to 94.22%, without
  reducing its 267 differing encodings.
- `UpdateInputDeviceTransitions` declares the previous button word first,
  followed by buttons, changed bits, time and device. Literal matching rises
  from 123 to 145 of 288 bytes, even though differing encodings rise from
  45 to 51; mnemonic similarity improves from 90.91% to 92.42%.
  Other tested declaration orders and a separate XOR assignment leave the
  output unchanged. An explicit promoted device index regresses file matching.
  A signed time local or qualified time read shortens the aligned function
  by sixteen bytes and disturbs subsequent placement; neither is retained.
  Swapping the mine's two short locals, swapping packet-release counters,
  separating the return-to-master range assignment, capturing the barracks
  menu-label pointer, and changing sound-list declaration/loop forms leave
  their output unchanged. A promoted navigation-pointer index likewise does
  not resolve that routine's missing saved register. These trials are not
  retained.
  Only seven function rows change in the full report. Normalized exact
  functions rise from 1,351 to 1,354 of 1,550; same-size differences fall
  from 194 to 191, and five size differences remain. Mean mnemonic
  similarity is 99.68%, with three functions below 90%. No existing exact
  function is lost, and function spans and compilation-unit placements are
  unchanged.
  Strict whole-file matching rises by 2,049 positions, from 484,701 to
  486,750 of 518,656 (93.4533% to 93.8483%); 31,906 positions still differ.
  Code gains 1,886 matching bytes and relocations gain 163. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The continuous
  matching prefix is still 544 bytes. Its first difference, at `0x220`,
  is the relocation section's virtual-size field; the restored missile
  reference reduces that size deficit from six bytes to four.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `15e00c14df7b85cfa3ebd386cbf0702c52f5d3477115f38482ffd49503bd779e`.
- `sort_object_depth` declares the placed-object pointer first, followed by
  the previous distance, screen offset, selected object, next-best distance,
  unsigned distance and object counter. In the outer loop, store the selected
  object before resetting the next-best distance to -1. Together these
  choices recover the original cursor load and second-scan registers:
  EBP for the best distance, EBX for the screen offset, ECX for the object,
  EDI for the placed-object pointer, EAX for distance, EDX for the previous
  distance and ESI for the selected object. All 192 bytes now match literally,
  up from 166; all nineteen differing encodings disappear. Declaration order
  alone leaves ten differences, and exchanging the best-distance and screen
  offset declarations does not finish the match.
- `LoadPacketAllocated` captures the promoted logical-file index in an int
  after prompting for the disk. Use it for the size lookup and packet-load
  lookup. Keep an explicit file-table pointer for the retry loop, refreshing
  it after capturing the packet-error word. The original likewise reloads
  the table after the call, before testing the retries and error.
  The promoted index recovers ESI for the scaled index and BP for the section.
  Differing encodings fall from 37 to 32, and literal matching rises from
  66 to 71 of 144 bytes. The table/error register swap and its one-byte
  instruction-length effect remain. Keeping both the table and error locals
  preserves the byte-sort routine's existing two differences; the index
  alone, a merged retry condition, or an implicit index leaves that neighbor
  with four. These local spellings reproduce measured compiler context;
  they do not establish the original local names.
  Packet-size narrowing and the tested index/error declaration orders do
  not improve the packet loader further. Save-game counter declaration moves
  and explicit do/while forms for its four byte-copy loops leave its four
  base/index-encoding differences unchanged. A sprite frame-count local leaves
  its target routine unchanged while disturbing later output, so it is discarded.
  The launch-pointer array restores the frame size but does not recover the
  original pointer slots; its four-byte literal gain comes with two additional
  differing encodings, and that trial is not retained.
  Palette component-count captures change the aligned function size and are
  discarded. Palette local-order changes, scene-animation pointer declaration
  moves, input-coordinate declaration moves, target-box type/index captures,
  and packet-stack count captures do not improve their targets. Explicit
  input-event candidate pointers and communication object/class captures
  regress the file. These probes are not retained.
  Only the two intended function rows change in the full report. Normalized
  exact functions rise from 1,354 to 1,355 of 1,550; same-size differences fall
  from 191 to 190, and five size differences remain. Mean mnemonic similarity
  is 99.68%, with three functions below 90%. No existing exact function is lost,
  and function spans and compilation-unit placements are unchanged.
  Strict whole-file matching rises by 31 positions, from 486,750 to
  486,781 of 518,656 (93.8483% to 93.8543%); 31,875 positions still differ.
  All gains are in code. The `.rdata`, `.data` and `.idata` sections remain
  literally exact; relocation-section matching remains 30,303 of 36,352 bytes.
  Packet loading and depth sorting retain their original relocation counts
  of five and eight respectively.
  The continuous matching prefix is still 544 bytes, and the first difference
  remains the relocation section's virtual-size field at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `0f39e0c35d0cd8dc3c62fc4d18c89125488ea4dfd9df244ddb1d43974c2faef5`.
- `draw_sorted_objects_to_buffer` makes the special-object draw call in
  each shape-selection branch. CL merges the calls while preparing the
  frame argument before choosing the shape, as the original does. Selecting
  the shape before a single source call delays that argument and changes
  the object-index allocation. Declare screen Y before screen X to recover
  DX/CX for the special-object coordinates. Together these changes reduce
  differing encodings from 57 to nineteen, raise mnemonic similarity from
  91.67% to 97.22%, and raise literal matching from 190 to 274 of 304 bytes.
  The ordinary-object path still differs in register allocation and load
  scheduling. Moving the object or shape declarations, loading X first, and
  using an enum class local do not improve the selected result. Separate
  branch scopes for the shape pointers regress it and are discarded.
- `shrink_vector` names the Y and Z component pointers that the original
  computes before its loop. Declare the result first, then Z and Y, while
  assigning Y before Z. This restores ESI for the vector, EBP for Y, EBX
  for Z and EDI for the accumulated result. All 64 bytes now match literally,
  up from 55, and all nine differing encodings disappear. Declaring Y, Z
  and the result in that order leaves five differences. The loop still
  evaluates and combines the X, Y and Z calls in their original order.
- `approach_speed` declares acceleration before delta. This restores EDI
  for the speed difference, increasing literal matching from 97 to 103 of
  112 bytes and reducing differing encodings from fifteen to nine. The ship
  and acceleration registers remain exchanged. Capturing the current speed
  or promoted ship index leaves this routine unchanged while disturbing
  neighbors; updating the target-speed parameter directly also regresses
  output. These trials are not retained.
  Explicit object-class and promoted-index captures in `set_up_action_sphere`
  regress file matching. Moving all camera locals into the switch and
  reversing its vector equalities leave `set_eye_direction_and_position`
  unchanged. A previous-angle local also leaves that routine unchanged while
  disturbing its unit. A separate direction vector in camera mode five or
  an explicit component capture for its comparisons regresses the file.
  Moving `animate_shape`'s promoted index first or exchanging its declaration
  with the object type leaves its output unchanged. None of these probes
  is retained.
  Only the three intended function rows change in the full report. Normalized
  exact functions rise from 1,355 to 1,356 of 1,550; same-size differences fall
  from 190 to 189, and five size differences remain. Mean mnemonic similarity
  is 99.69%, with three functions below 90%. No existing exact function is lost,
  and function spans and compilation-unit placements are unchanged.
  Strict whole-file matching rises by 104 positions, from 486,781 to
  486,885 of 518,656 (93.8543% to 93.8744%); 31,771 positions still differ.
  Code gains 99 matching bytes and relocations gain five. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The three routines
  retain their original relocation counts: one for speed adjustment,
  27 for sorted-object drawing and none for vector shrinking. The continuous
  matching prefix remains 544 bytes, with the first difference at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `057193fbf5697a765cffe83cc88fb9b4766c2df632fb54c2e1f0ad3577e1088c`.
- `UpdateTitleMenuCursor` declares mouse X and Y before its frame. This
  restores SI/DI for the coordinates and BX for the frame, removing all
  seven differing encodings. Its entire 96-byte span now matches literally,
  up from 89 bytes.
- `drop_mine` captures the promoted mine index after choosing its lifetime.
  Use that int for the collision-grace and counter stores, then return it
  after removing the weapon. The original likewise sign-extends the mine
  into EDI before those stores and preserves it through the final call.
  This also recovers SI for the owner and DI for the newly created mine.
  All 176 bytes now match literally, up from 160; all sixteen differing
  encodings disappear. Exchanging the two original local declarations alone
  leaves the old register allocation unchanged.
- In `gametrack`'s training-simulator branch, declare the current track
  before the stream set. Capture the stream set first, then capture the
  current track only if the stream-set test needs it. After changing tracks,
  reload the current track and then the stream set for the diagnostic.
  The original performs those same reads, retaining the guard's values on
  the path that makes no track change. These captures recover EAX for the
  track and ECX for the stream set, matching all 336 bytes, up from 320,
  and removing all seven differing encodings. Capturing only the diagnostic
  arguments does not fix this routine. Both forms improve one literal byte
  in the following `servicetrack`, from 118 to 119 of 464, reducing its
  differing encodings from 95 to 94 without changing mnemonic similarity.
  Declaration and loop changes in `ReleaseContiguousPaletteEntries` do not
  improve literal matching; the entry-first declaration raises mnemonic
  similarity to 100% but loses one matching file byte. A promoted first-entry
  index also regresses output. A shared polling-exit label in `WaitForInputKey`
  and explicit left-bound locals in `UnionRectBounds` leave their output
  unchanged. A promoted index in `pilot_hit` leaves that routine unchanged
  while disturbing neighbors. The second communication-loop index capture,
  cruise-local declaration swap and save-writer position pointers likewise
  give no improvement; a captured cruise destination regresses the file.
  These probes are not retained.
  Only four function rows change in the full report. Normalized exact
  functions rise from 1,356 to 1,359 of 1,550; same-size differences fall
  from 189 to 186, and five size differences remain. Mean mnemonic similarity
  stays 99.69%, with three functions below 90%. No existing exact function
  is lost, and function spans and compilation-unit placements are unchanged.
  Strict whole-file matching rises by 40 positions, from 486,885 to
  486,925 of 518,656 (93.8744% to 93.8821%); 31,731 positions still differ.
  All gains are in code. The `.rdata`, `.data` and `.idata` sections remain
  literally exact, and relocation-section matching remains 30,308 of
  36,352 bytes. Relocation counts in the affected routines are unchanged.
  The continuous matching prefix remains 544 bytes, with the first
  difference at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `2f12c11cf88a0cf4c1108d950e8f7b60230540bca792a0663e6a22f594eff742`.
