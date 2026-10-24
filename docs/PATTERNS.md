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
  the explicit range-label form documented below also recovers branch placement.
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
- `process_maneuver_node` captures the promoted object index after its
  choice, rating and maneuver declarations. Use that int for the rating,
  side and current-maneuver table accesses, while retaining the short object
  argument for calls. This recovers SI for the object and EDI for its index,
  as in the original `MOVSX EDI,SI`, and removes all ten differing encodings.
  The full 128-byte span is now literally exact, up from 114 matching bytes.
  Declaring the rating before the choice pointer instead regresses output.
- `heat_seeking_missile_intelligence` declares the candidate-list index
  before exhaust heat and the ship-scan index. This recovers DX for the heat
  countdown and the original ordering of its clear and target store. The
  complete mnemonic sequence now matches, and differing encodings fall
  from fourteen to eleven. Literal matching rises from 298 to 309 of 320
  code bytes, with another matching byte in relocations. The candidate-loop
  index still exchanges AX and BX with its promoted table index; the first
  scan also retains its reversed equality comparison. The other local
  declaration orders do not improve on this result. Sharing the two loop
  indices can reproduce it but gives no further gain; explicitly capturing
  the selected target index leaves eleven differences and regresses the file.
- `PlaceNavMapLabel` indexes the label array directly for its initial colour
  and text stores, then captures the label pointer only when reserving the
  final rectangle. This preserves the 336-byte span and restores the seventh
  address relocation, matching the original count. The earlier pointer used
  for the initial stores combined their two absolute references into one.
  This is still an intermediate match: code matching falls from 58 to 50
  bytes, but relocation matching gains 117 bytes, improving the complete file
  by 109. Its normalized report row is unchanged. Capturing an initial short
  or int label index gives a smaller gain; mixing indexed and pointer access
  in the initial stores displaces later code. Those alternatives are discarded.
  Reversing or regrouping `dot_product`'s three terms leaves its output
  unchanged. Sequential accumulation makes that routine exact, but disturbs
  `rotate_about_j` and adds sixteen bytes to `rotate_about_k`, shifting later
  code. Int/long accumulators and the product/sum/result names do not avoid
  those regressions, so none is retained. Reversed menu-label comparisons,
  a captured menu-label value, reversed formation comparisons, separate
  master-range assignment, a master-range rename, packet-release local order,
  a promoted `Mgloat` index, and speed-controller local order or speed-pointer
  captures give no improvement. Formation index/spot captures, a previous-view
  class minimum, a guarded particle loop, and captured flight inputs regress
  the file; explicit int constellation-loop comparisons leave output unchanged.
  Only two normalized report rows change; the navigation routine also changes
  literally as described above. Exact functions rise from 1,359 to 1,360 of
  1,550, same-size differences fall from 186 to 185, and five size differences
  remain. Mean mnemonic similarity is 99.69%, with three functions below 90%.
  No existing exact function is lost, and spans and unit placements are
  unchanged. Strict whole-file matching rises by 135 positions, from 486,925
  to 487,060 of 518,656 (93.8821% to 93.9081%); 31,596 positions still differ.
  Code gains seventeen matching bytes and relocations gain 118. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The rebuilt image now
  has 15,445 HIGHLOW relocations against the original 15,446; the relocation
  directory remains four bytes short. The continuous matching prefix remains
  544 bytes, with the first difference at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `b2130c6ac766c0347c387186a224c01ca7000ddeb8109d8c292bd7ca722364a4`.
- `LoadMissionData` captures each auxiliary packet's source pointer before
  calling `DosMemcpy`. This restores the scaled-address calculations before
  the length pushes and recovers the original preload-copy register choices.
  Increment each navigation/objective loop's index before its disk pointer
  to recover the remaining instruction order. Together these changes remove
  all sixteen differing encodings and make the entire 784-byte span literally
  exact, up from 758 matching bytes. Explicit source/destination pointers for
  the preload loop regress output and are discarded.
- `reposition_fixed_child_objects` indexes parent tables with the short
  parent directly in its turret/thruster branch. Keep the promoted index
  capture only for the final scale update. The original sign-extends the
  parent into EAX and retains its doubled word index in EDI across the
  trigonometric and multiply calls; the earlier int capture instead retains
  an unscaled index. Removing that early capture eliminates all 81 differing
  encodings and matches all 384 bytes, up from 92. Changing the scale local
  to unsigned short alone leaves output unchanged.
- `intelligence_events` initializes the target-gone flag after reading the
  target and previous stress. Place the inactive-target case after the
  active-target event selection, matching the original branch layout. This
  reduces differing encodings from 129 to one and raises mnemonic similarity
  from 95.10% to 100%. Literal code matching rises from 85 to 495 of 496 bytes,
  with another thirteen matching relocation bytes. The remaining difference
  reverses the operands of the wingman equality comparison.
- `DIBupdate` calculates destination and source pixel offsets in separate
  int locals before adding their buffer bases. This restores the original
  integer arithmetic sequence, reducing differing encodings from five to
  three and increasing literal matching from 371 to 381 of 384 bytes, plus
  one relocation byte. The remaining differences allocate the DirectDraw
  pointer to EDX instead of ECX. Reusing a single offset local reaches the
  same target result but disturbs neighboring functions. Swapping the two
  offset declarations gives no further gain; placing both before the buffer
  pointers or capturing the DirectDraw pointer regresses output.
  Component-index/damage captures in `ReportComponentRepaired`, a turn-interval
  capture in `find_next_ship_turn_slot`, and an int target-list index in
  `build_your_target_list` regress the file. Reordering its existing locals
  or capturing a promoted object index gives no improvement. The `any_selected`
  local-order swap leaves output unchanged; a captured weapon-slot pointer
  regresses it. All high-score local permutations and changing the previous
  score's signedness leave `UpdateTrainSimHighScores` unchanged. Explicit
  triangle endpoints, an asteroid velocity pointer and a captured enemy
  distance regress their routines. A string-builder pointer and alternative
  on-screen-message flag declaration/assignment orders give no improvement.
  None of these probes is retained.
  Only the four intended function rows change in the full report. Normalized
  exact functions rise from 1,360 to 1,362 of 1,550; same-size differences fall
  from 185 to 183, and five size differences remain, totaling 26 extra bytes.
  Mean mnemonic similarity rises to 99.70%, with three functions below 90%.
  No existing exact function is lost, and spans and unit placements are
  unchanged. Strict whole-file matching rises by 752 positions, from 487,060
  to 487,812 of 518,656 (93.9081% to 94.0531%); 30,844 positions still differ.
  Code gains 738 matching bytes and relocations gain fourteen. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. All four routines
  retain their original relocation counts, and the relocation directory
  remains four bytes short. The continuous matching prefix remains 544 bytes,
  with the first difference at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `68582717d4c9980815fac25dbb57e68ba9f0caf8ce553d058f83cdb86691cf47`.
- `LoadWingCmdrCfgFile` declares the destination pointer before the argument
  count. This restores EBP for the destination and BX for the count, including
  the original initialization order. All eighteen differing encodings
  disappear, and the full 224-byte span matches literally, up from 197 bytes.
  Initializing the count before the argument index gives the same result
  and is unnecessary.
- `print_subtitle` declares the character first, then scan, line count, Y
  and X. This recovers EBX for the initial text pointer, EDI for the scan,
  SI for Y and BP for X. Differing encodings fall from 37 to six, mnemonic
  similarity rises from 95.45% to 96.97%, and literal matching rises from
  304 to 338 of 352 bytes. The remaining differences schedule the constant
  320 before the argument push at the two line-length calls. Swapping X/Y
  alone gives a smaller gain. Moving the character first without that swap,
  rotating the scan declaration last, or capturing the earlier file-seek
  offset likewise gives a smaller gain; the offset capture leaves the seek
  routine unchanged. The selected declaration order needs no extra local.
- `GetShapeFrameCorners` declares left, top, right and bottom in that order.
  This recovers EBX for the right coordinate and the original code length
  within its 144-byte span. Differing encodings fall from 26 to sixteen,
  mnemonic similarity rises from 92.86% to 95.24%, and literal matching rises
  from 71 to 118 bytes. The Y-origin load still occurs too early, and the
  top and corner-pointer registers remain different. Other tested coordinate
  orders give no larger gain; combining or reversing the Y additions leaves
  the selected output unchanged.
- `easy2see` captures its promoted object index after the bounds and coordinate
  declarations, then uses it for all seven object-table accesses. Its emitted
  128-byte span is unchanged. The resulting compiler context restores EAX for
  the guarded block, ECX for the prefix-corruption count and EDX for the first
  countdown in `CheckAllGuardedAllocations`. That routine loses all nine
  differing encodings and matches its complete 112-byte span, up from 103.
  This is a measured declaration-context choice, not proof that the original
  visibility routine spelled out the promotion. Reordering the guard counters
  or scoping the block pointer gives a smaller four-byte improvement; moving
  the block declaration alone does nothing. Visibility-local permutations
  leave output unchanged.
  Reversed wingman, wave-entry and ship-side equalities leave their output
  unchanged. Inlining the pilot-field cursor offset loses a byte, an assignment
  expression leaves the old output, and a qualified X parameter regresses
  literal matching despite reaching 100% mnemonic similarity. Front-position
  pointer/index captures and a promoted Futurion index regress the file.
  An unsigned event-slot index gives no literal gain and lowers mnemonic
  similarity. A captured full-ahead counter makes both `Mfull_ahead` and
  `Mgloat` exact, but regresses `Mfish_hook` and loses eighteen file bytes.
  Separate cockpit-light draw scopes make the lights and `flag_reached`
  exact, but regress other cockpit routines and lose 58 file bytes. Promoted
  AI-initialization indices leave that target unchanged while disturbing
  neighbors; a captured constellation-definition pointer regresses output.
  These probes are not retained.
  Only four function rows change in the full report. Normalized exact
  functions rise from 1,362 to 1,364 of 1,550; same-size differences fall
  from 183 to 181, and five size differences remain, totaling a net 26 extra
  bytes. Mean mnemonic similarity remains 99.70%, with three functions below
  90%. No existing exact function is lost, and spans and unit placements are
  unchanged. Strict whole-file matching rises by 117 positions, from 487,812
  to 487,929 of 518,656 (94.0531% to 94.0756%); 30,727 positions still differ.
  All gains are in code. The `.rdata`, `.data` and `.idata` sections remain
  literally exact, while relocation matching stays at 30,440 of 36,352 bytes.
  The affected routines retain their original relocation counts. The
  continuous matching prefix remains 544 bytes, with the first difference
  at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `e8519157166f8bce702ffc83a4de6a53713ab52cfc3c1d451f80d67d7f7b095d`.
- `Mfull_ahead` captures its counter pointer after the speed-adjustment call,
  then reads and decrements through that pointer. This restores EAX for the
  address and CX for the old count, matching all 64 bytes instead of 57.
  Retain this together with two declaration changes in the same unit:
  `Mfish_hook` captures its promoted ship index after `advanceSequence`, and
  `Mtarget_missile` declares the short object before its promoted index.
  These preserve the original SI/BX allocation in the fish-hook maneuver and
  CX/EAX allocation in the missile scan. Their complete 272- and 176-byte
  spans remain literally exact. The resulting context also makes all 176
  bytes of `Mgloat` exact, up from 161, removing eleven differing encodings.
  The combined gain is 22 code bytes with no neighboring regression.
  Putting the fish-hook index first does not recover its allocation; an
  unsigned sequence flag, a register hint and reusing the flag for cruise
  velocity do not help. Without the missile-local reorder, that routine
  acquires six differing encodings.
- `RunTrainSim` handles a failed flight before its success branch, matching
  the original jump and block order. Save the campaign index before the data
  set to restore the DI/AX load sequence. Together these changes remove all
  thirteen differing encodings, raise mnemonic similarity from 93.94% to
  100%, and make the full 624-byte span literally exact, up from 560 bytes.
  Reversing the branches alone leaves three differences and gains 45 bytes;
  both changes together gain 64.
- `ValidateViewportBounds` declares its allocation index after the three
  row-offset locals. The viewport-pixel load now uses EAX as in the original.
  Differing encodings fall from thirteen to twelve, and literal matching
  rises from 223 to 225 of 240 bytes. The allocation counter and scan pointer
  still exchange registers, and several comparisons remain reversed. The
  intermediate index-declaration positions give no improvement.
- `EstablishingShot` captures its promoted frame index before the character
  loop and uses it for the animation-table reads. Its emitted 352-byte span
  is unchanged. This declaration context fixes one equality-comparison byte
  in `SceneDirector`, reducing its differing encodings from four to three
  and increasing literal matching from 1,132 to 1,133 of 1,136 bytes. This
  is a measured compiler-context choice rather than proof of the original
  local spelling.
  The player-mine slot-pointer probe superficially gains seven file bytes,
  but merges two original absolute references: the routine drops from its
  correct four relocations to three. Only two code bytes improve, while
  unrelated relocation mismatches partly cancel. Discard that form. Separate
  promoted weapon-index locals retain direct table access but regress output.
  Four objective-local rotations leave `flag_reached` unchanged; its promoted
  index capture regresses the unit. Separate frame, Y or X/Y locals in the
  cockpitless light branch make the light routine exact but regress the
  complete file. Pixel-pointer captures in viewport validation and Y-origin
  captures in corner extraction likewise regress the file. All five remaining
  navigation-label local orders leave output unchanged. Removing its final
  label pointer and passing both indexed coordinates directly adds sixteen
  aligned bytes, with either width/offset declaration order. None of these
  probes is retained.
  Only five function rows change in the full report. Normalized exact
  functions rise from 1,364 to 1,367 of 1,550; same-size differences fall
  from 181 to 178, and five size differences remain, totaling a net 26 extra
  bytes. Mean mnemonic similarity rises to 99.71%, with three functions below
  90%. No existing exact function is lost, and spans and unit placements are
  unchanged. Strict whole-file matching rises by 89 positions, from 487,929
  to 488,018 of 518,656 (94.0756% to 94.0928%); 30,638 positions still differ.
  All gains are in code. The `.rdata`, `.data` and `.idata` sections remain
  literally exact, and relocation matching remains 30,440 of 36,352 bytes.
  The affected routines retain their original relocation counts. The
  continuous matching prefix remains 544 bytes, with the first difference
  at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `da8920e4cebd42e56d4576780bd7827e467738541611350ba64b6702db1d9146`.
- `DrawTargetRangeReadout` uses an explicit branch to numeric formatting and
  a shared readout label after the excessive-range copy. This retains the
  source literal order `"TOO FAR"`, `" m"`, `"----- m"` while allowing CL to
  schedule numeric formatting before the excessive-range block, as in the
  original. All 21 differing encodings disappear, mnemonic similarity rises
  from 95.15% to 100%, and the complete 384-byte span matches literally,
  up from 323 bytes. Its 26 relocations remain at the original positions.
  Reversing the ordinary if/else branches also reaches normalized exactness,
  but swaps the first two literals: eleven data bytes and two code address
  bytes then differ. That form is not retained. A ternary, separate distance
  tests, flattened visibility cases and a Boolean switch do not recover both
  code and data order. Capturing the distance or reversing the comparison's
  spelling leaves the old output unchanged.
  Packet-section temporary changes, packet-loader declaration/assignment
  changes, navigation-coordinate declaration changes, a viewport for-loop,
  gun-counter declaration changes, hostile-ship local/comparison changes,
  and early-return/promoted-index ship searches give no suitable file gain.
  Neither packet-release local order nor reversed barracks-label comparisons,
  navigation sums, heap-address sums or a byte-sort skip label helps.
  Explicit packet-handle traversal pointers superficially gain three file
  bytes but add a fourth relocation where the original has three, even with
  an ordinary captured count. A qualified count read prevents that extra
  reference and brings the target closer, but regresses the formerly exact
  `PushMemoryStackFrame` and loses file bytes. All six declaration orders and
  two attempts to preserve that neighbor with its own index capture fail to
  improve the file. These probes are discarded.
  Only the range-readout row changes in the full report. Normalized exact
  functions rise from 1,367 to 1,368 of 1,550; same-size differences fall
  from 178 to 177, and five size differences remain, totaling a net 26 extra
  bytes. Mean mnemonic similarity remains 99.71%, with three functions below
  90%. No existing exact function is lost, and spans and unit placements are
  unchanged. Strict whole-file matching rises by 61 positions, from 488,018
  to 488,079 of 518,656 (94.0928% to 94.1046%); 30,577 positions still differ.
  All gains are in code. The `.rdata`, `.data` and `.idata` sections remain
  literally exact, and relocation matching stays at 30,440 of 36,352 bytes.
  The continuous matching prefix remains 544 bytes, with the first difference
  at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `7230b2fed4950a97479b885b2309fbaa1ba89ae889f7159625ef9e4f61e8a6c3`.
- `DrawScrambleActor` declares `animationFrame` before `frame`. CL then
  keeps the animation index in BL and uses EAX for the state comparison,
  matching the original prologue and branch bodies. Differing encodings
  fall from 27 to thirteen, and literal matching rises from 191 to 225 of
  256 bytes. Mnemonic similarity remains 97.22%; animation-pointer load
  scheduling and control-mask promotion still differ. The routine retains
  all twenty relocations, and their improved positions gain three more
  file bytes. Other byte-local orders and the X/Y swap give no greater gain.
  Putting the control byte first reaches 100% mnemonic similarity but gains
  fewer literal bytes. Scoping that byte, splitting its mask assignment,
  or using an unsigned switch provides no further improvement. Animation-
  pointer and promoted-control locals likewise give no larger file gain.
- `ReadTextInput` initializes `accepted` before saving the background colour.
  This selects CL for the saved byte and restores the original final stores,
  pointer reload and epilogue scheduling. All nine differing encodings
  disappear, mnemonic similarity rises from 98.52% to 100%, and all 928
  bytes match literally, up from 910. Its 22 relocations are unchanged.
  Moving only the saved-background declaration leaves the old output.
  Qualifying its final read gains three file bytes but retains six differing
  encodings, including when combined with the earlier result initialization;
  that form is not retained.
  Separating the master-return range assignment or using an early distant
  return leaves output unchanged, as does the cruise-local declaration swap.
  Reversing the imperial formation's wave comparison does nothing; capturing
  that wave, with or without a qualified read, regresses the file. An explicit
  polling-drain branch in `WaitForInputKey` leaves its fifteen differences.
  These probes are discarded.
  Only the two selected function rows change in the full report. Normalized
  exact functions rise from 1,368 to 1,369 of 1,550; same-size differences
  fall from 177 to 176, and five size differences remain, totaling a net
  26 extra bytes. Mean mnemonic similarity remains 99.71%, with three
  functions below 90%. No existing exact function is lost, and spans and
  unit placements are unchanged. Strict whole-file matching rises by 55
  positions, from 488,079 to 488,134 of 518,656 (94.1046% to 94.1152%);
  30,522 positions still differ. Code gains 52 matching positions and
  relocation data gains three, reaching 30,443 of 36,352. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The continuous
  matching prefix remains 544 bytes, with the first difference at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `a5f3d2ec4328160eda84cce9e0c387124396c11de2c6984a78b268c6181dfdf2`.
- `animate_shape` inlines both scale products and uses the short object
  parameter for the decrease branch. Removing the scale-change local
  restores EBX for the promoted object index and EDX for its type. The
  increase path regains its separate absolute scale store. Differing
  encodings fall from 31 to 21, mnemonic similarity rises from 99.04% to
  99.05%, and literal matching rises from 273 to 286 of 352 bytes. Relocation
  data gains another two matching positions. The decrease still computes
  its scale address once, so the routine retains fifteen relocations against
  the original sixteen; that discrepancy is unresolved.
  Inlining both products alone gains nine file bytes. Inlining only one,
  reusing the scale-change local for the result, or adding explicit scale
  branches/frame-tail labels does not improve the earlier output. Using
  the short index for the increase branch gives the same file gain as the
  selected decrease form. A short index only in the product read, an explicit
  mixed-index subtraction, captured decrease-scale words, omitted product
  casts and an inline frame-tail label give no greater gain.
- `initialize_cockpit` writes the zero top edge before the zero left edge,
  matching the original store addresses. This gains two literal bytes,
  from 825 to 827 of 1,088; address normalization hides the change in its
  function report. Its 72 relocations remain unchanged.
  The original also lacks the reconstructed null checks after loading the
  mode-four backdrop and before releasing the rear-view backdrop. Removing
  both, together with the store-order correction, makes all 1,088 cockpit
  bytes exact. However, that compiler context changes `PlaySceneAnimation`
  from 545 matching bytes to 114 of 560, and loses 175 file bytes overall.
  Removing either guard separately also regresses the file. Five scene-local
  layouts, two captured-index positions, reuse of the animation parameter,
  reversed multiplication operands, completion/wait initialization order
  and a chained initial object assignment do not recover that neighbor.
  Chained cockpit zero stores, reversed right/bottom stores and initializing
  the mode at declaration likewise give no suitable combined gain. These
  broader changes are not retained; the two original backdrop paths remain
  work to complete alongside the scene-allocation discrepancy.
  Moving the scramble actor's coordinate pair or using long DirectDraw pixel
  offsets gives no improvement. A C90 identifier-list definition of the
  barracks detail routine produces identical bytes, but its function row is
  omitted by the source scanner; retain its prototype-style definition.
  Only the animation row changes in the full report. Normalized exact
  functions remain 1,369 of 1,550, with 176 same-size differences and five
  size differences totaling a net 26 extra bytes. Mean mnemonic similarity
  remains 99.71%, with three functions below 90%. No existing exact function
  is lost, and spans and unit placements are unchanged. Strict whole-file
  matching rises by seventeen positions, from 488,134 to 488,151 of 518,656
  (94.1152% to 94.1185%); 30,505 positions still differ. Code gains fifteen
  matching positions and relocation data gains two, reaching 30,445 of
  36,352. The `.rdata`, `.data` and `.idata` sections remain literally exact.
  The continuous matching prefix remains 544 bytes, with the first difference
  at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `7765a9f19df191ba4adde74cbe3af1bd275a940813619c0f9f27f06dd46f6077`.
- `show_target_disp` computes each shield ratio into the existing short
  frame local before passing it to `MinShort`. This restores the original
  division, argument preparation and register choices for both shield draws.
  All 25 differing encodings disappear, mnemonic similarity rises from
  95.07% to 100%, and all 784 bytes match literally, up from 696. Its 46
  relocations and the complete view unit's 307 relocations remain unchanged.
  A separate short shield-level local also makes this routine exact, but
  disturbs `CalcRectangleArea` and gains four fewer file bytes. Capturing
  only the scaled numerator gains two bytes and retains 23 differing
  encodings. Reusing the existing frame local preserves every neighboring
  report row and requires no additional declaration.
  Eight autopilot local-order variants give no gain. Early-return and
  captured-frame-count forms of `DrawSpriteTransformed` regress the file;
  eight existing-local layouts in `GetViewportIntersection` leave its output
  unchanged. The latter's original reversed loop expression and uninitialized
  state remain preserved. Capturing the first navigation-label availability
  result or the communication object's class regresses output. Capturing
  the NPC speaker for the second loop or splitting subtitle centering into
  a remaining-width local leaves output unchanged.
  Eight combinations of qualified parameter/count reads in the position
  and volume voice setters produce the same executable. Their compensating
  minus-one/plus-one size errors remain unresolved. Swapping local declaration
  or initialization order, or using an explicit search break, in the wave
  and active-sound removal routines gives no improvement. Capturing the
  formation spot before calculating the formation delta also regresses the
  file. All of these probes are discarded.
  Only the target-display row changes in the full report. Normalized exact
  functions rise from 1,369 to 1,370 of 1,550; same-size differences fall
  from 176 to 175, and five size differences remain, totaling a net 26
  extra bytes. Mean mnemonic similarity remains 99.71%, with three functions
  below 90%. No existing exact function is lost, and spans and unit placements
  are unchanged. Strict whole-file matching rises by 88 positions, from
  488,151 to 488,239 of 518,656 (94.1185% to 94.1354%); 30,417 positions
  still differ in 6,224 runs. All gains are in code, which now matches at
  376,899 of 401,408 positions. Relocation matching remains 30,445 of
  36,352, and the `.rdata`, `.data` and `.idata` sections remain literally
  exact. The continuous matching prefix remains 544 bytes, with the first
  difference at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `36cd2f4404ac58841aa000721338d9d8ea30f989794e0858d3c87d77e3a5db2c`.
- `CalibrateJoystickInteractive` multiplies its existing left, up, right
  and down scale locals by their corresponding ranges after publishing the
  scales, then uses those products to calculate the four bounds. This
  restores retention of the minimum-X value through the final validity
  comparison, removing its extra absolute reload. The routine now has the
  original 53 relocations instead of 54. Differing encodings fall from 131
  to 105, mnemonic similarity rises from 95.75% to 96.73%, and literal
  matching rises from 577 to 666 of its unchanged 1,040-byte span. The
  relocation section gains another 207 matching positions.
  Capturing minimum X in a new local at four declaration positions, or
  repeating its expression in the final condition, also removes the extra
  relocation but regresses the file. Interleaving the first bound store
  between the up and right products produces the retained executable.
  Placing each product immediately before its bound, grouping products by
  axis, or permuting the horizontal-range/left-scale/right-scale declarations
  gives no greater gain. Lower-right sample stores retain a three-byte
  scheduling discrepancy. Separating their global and record assignments,
  or qualifying the record stores or Y read, does not improve the file;
  qualifying their global stores shortens the aligned span by sixteen bytes.
  These additional variants are discarded.
- Capturing `animate_shape`'s masked decrease factor, at function or branch
  scope, worsens its output. Explicit subtraction and equivalent negated
  products leave it unchanged. The shrink branch still shares one computed
  scale address and has fifteen relocations against the original sixteen.
  Flight-control captures for the wingman, selected weapon, communication
  guard and missile-camera value increase mnemonic similarity to 98.35%,
  but retain 176 relocations versus 174 and regress literal matching. Six
  declaration orders give the same result. Qualifying the captured wingman
  and weapon reads removes both remaining excess references and reaches
  98.99% mnemonic similarity, but loses 8,137 matching file positions against
  the retained calibration build and disturbs two exact neighboring routines.
  A captured signed or unsigned key difference recovers 52 of those positions;
  the inline difference does not. These controls remain a coupled matching
  problem, so none of the captures is retained in this checkpoint.
  Five cockpit-light local permutations and byte/int frame types regress
  output. Reversing the VDU percentage product leaves its four differences;
  capturing that product in an int also leaves them and disturbs neighbors.
  Those probes are discarded as well.
  Only the calibration row changes in the full report. Normalized exact
  functions remain 1,370 of 1,550, with 175 same-size differences and five
  size differences totaling a net 26 extra bytes. Mean mnemonic similarity
  remains 99.71%, with three functions below 90%. No existing exact function
  is lost, and spans and unit placements are unchanged. Strict whole-file
  matching rises by 296 positions, from 488,239 to 488,535 of 518,656
  (94.1354% to 94.1925%); 30,121 positions still differ in 6,019 runs.
  Code gains 89 matching positions, reaching 376,988 of 401,408; relocation
  matching rises to 30,652 of 36,352. The `.rdata`, `.data` and `.idata`
  sections remain literally exact. The continuous matching prefix remains
  544 bytes, with the first difference at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `7de2c9f5768cb531f58a55aa61379f727cbe604ba050bfd612c0dd08ab8a13dd`.
- `SortSignedByteValuesAscending` again declares and initializes the swap
  byte inside the inner loop. In the current unit layout this restores the
  original memory-first swap comparison and its `JGE`, eliminating the two
  remaining differing encodings. All 112 bytes now match literally, up from
  110, with no neighboring report changes. Reversing the source comparison
  alone leaves output unchanged. Qualifying the inner read changes register
  allocation, and capturing its value in a separate byte local regresses
  output; neither alternative is retained.
- `LoadJoystickCalibrationFile` multiplies its existing Y and X range locals
  by their scales between the centre-X and centre-Y publications, then uses
  those products for all four bounds. This moves the two products and first
  subtraction ahead of the centre-Y store, as in the original. Mnemonic
  similarity rises from 93.90% to 94.72%, while differing encodings remain
  86 of 243. Literal matching gains two bytes, from 536 to 538 of the same
  784-byte span. The loader still has 30 relocations against the original
  34; the change neither repairs nor worsens that discrepancy. Placing both
  products before the centre stores gives the same literal gain. Computing
  X before Y, or publishing both centres before the products, gives no gain.
  Capturing cockpit frames or message pointers, constellation-definition
  pointers, and intelligence-object indices gives no improvement. Weapon
  coordinate captures, gun-selection local scopes and initialization order,
  and external-view selection captures also fail to improve the file.
  A mine-slot pointer used for the disabled test gains seven file bytes but
  removes one of the original four relocations, so it is discarded. A pointer
  used only for the type, qualified field reads, and swapping its existing
  declarations leave the baseline unchanged. Reversing the navigation-square
  bottom sum or wingman equality leaves output unchanged; capturing the
  bottom coordinate regresses output. These bounded trials are discarded.
  Only the sorting and calibration-loader rows change in the full report.
  Normalized exact functions rise from 1,370 to 1,371 of 1,550; same-size
  differences fall from 175 to 174. Five size differences remain, totaling
  a net 26 extra bytes. Mean mnemonic similarity rises from 99.71% to 99.72%,
  with three functions below 90%. No existing exact function is lost, and
  spans and unit placements are unchanged. Strict whole-file matching rises
  by four positions, from 488,535 to 488,539 of 518,656 (94.1925% to 94.1933%);
  30,117 positions still differ in 6,018 runs. All gains are in code, which
  now matches at 376,992 of 401,408 positions. Relocation matching remains
  30,652 of 36,352, and the `.rdata`, `.data` and `.idata` sections remain
  literally exact. The continuous matching prefix remains 544 bytes, with
  the first difference at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `b40c38f54b4b1db8efcdd8643332aa47214822619deda85d1e3cc2ebdfb7c752`.
- Qualify both boundary reads in `InitializeNearHeap`'s relocation-byte
  calculation. Its own 304 emitted bytes stay unchanged, including its eight
  remaining literal operand-address differences, but CL emits the following
  `ShutdownNearHeap` addition in the original base-before-size order. This
  makes all 96 shutdown bytes literally exact, up from 94. The entire
  executable changes at only those two operand bytes. This is a measured
  compiler-context choice, not evidence that the original C used `volatile`.
  Qualifying the second calculation as well produces the same executable.
  Either boundary read alone, or qualifying only the second calculation,
  instead changes the shutdown register allocation and loses 77 file bytes.
  Shared and separate arithmetic locals, at function or branch scope, shrink
  the initializer's aligned span by sixteen bytes and are discarded. A
  captured shutdown end address or qualified shutdown operands also regresses
  output. Qualifying the free-byte query's base or size leaves it unchanged.
  The initializer and shutdown retain their original 27 and 6 relocations.
- Further flight-control work is preserved as investigation evidence only.
  Capture the incremented signed view-mode byte inside each of the two
  cockpit branches, and let the final laser-fire case break to the function's
  common return. Combined with the earlier wingman/weapon captures, qualified
  communication-key read and missile-camera capture, this reproduces every
  mnemonic and the original 174 relocations, versus 176 in the retained
  handler. Only seven non-branch operand choices remain: the initial key
  difference uses AL instead of CL, and the two volume-table loads and pushes
  use ECX instead of EAX. The key sequence is two bytes short, displacing the
  following instructions. Differing encodings rise from 593 to 768 of 789
  despite mnemonic similarity rising from 97.59% to 100%. Literal matching
  falls from 1,149 to 424 of the unchanged 3,184-byte span. Whole-file
  matching loses 8,027 positions, including 7,297 relocation-section bytes,
  so this coherent candidate is not retained yet. Its normalized report
  changes only the handler; branch-scoped mode captures preserve the other
  main-unit rows. Function-scoped captures disturb neighboring routines,
  while qualified mode reads can shrink `RunSpaceFlight` by sixteen bytes.
  Capturing the previous key or both keys, changing those captures' scopes
  and assignment order, or reordering the adjacent current/previous/view
  declarations does not fix the remaining register choices.
  Guarded-allocation counter captures and the five other counter-declaration
  orders give no gain. Direct or separately captured trimming goals, reversed
  target comparisons and captured sides, hostile-sphere side captures,
  ship-lookup guard layouts, and flight-dynamics input captures also fail to
  improve the file. Wave-index and objective-range captures regress output;
  swapping the near-heap base/size declarations loses 21 matching positions.
  These trials are all restored and discarded.
  A fresh raw audit of all 1,371 normalized-exact functions finds fourteen
  spans that still differ at their original addresses before this checkpoint,
  including operand-order differences and audio spans affected by a one-byte
  displacement. The shutdown correction reduces that count to thirteen.
  All 1,550 normalized report rows remain unchanged: 1,371 exact functions,
  174 same-size differences, and five size differences totaling a net 26
  extra bytes. Mean mnemonic similarity remains 99.72%, with three functions
  below 90%. Strict whole-file matching rises by two positions, from 488,539
  to 488,541 of 518,656 (94.1933% to 94.1936%); 30,115 positions still differ
  in 6,016 runs. Code matches at 376,994 of 401,408 positions. Relocation
  matching remains 30,652 of 36,352, and `.rdata`, `.data` and `.idata` remain
  literally exact. Function spans, unit placements and the continuous
  544-byte matching prefix are unchanged; the first difference is at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `21570d859814a894ffe07809fb330651d2fdc9488f29aa94511486ccae1b085f`.
- `ReleaseInputEvent` walks the used-slot table with an explicit pointer,
  advancing that pointer before the slot index. This restores the original
  order of the two loop updates and the full instruction-body length.
  Differing encodings fall from thirteen to five; literal matching rises
  from 28 to 43 of the unchanged 48-byte span. The used-table pointer and
  event parameter still exchange EAX/EDX. The changed compiler context also
  makes `RetainInputEventsOfType` exact: its seven differing encodings
  disappear, and all 96 bytes match, up from 89.
  Keep `sort_object_depth`'s existing placed pointer after its object local.
  Without this declaration move, the added release-loop pointer disturbs
  nineteen depth-sort encodings. Moving the declaration restores the entire
  original 192-byte span without another local or logic change. Placing it
  last or scoping it to the inner search also restores that span. Reversing
  the release loop's update order loses three file bytes; a declaration
  initializer or do/while form gives no further gain. The three routines
  retain their original two, three and eight relocations, respectively.
  Separate or scoped joystick double-click locals and captured button-state
  bytes regress output. Capturing the first device index leaves it unchanged.
  Separating the ordinary sprite's X/Y loads and adding X before the shape
  load leaves the renderer unchanged; adding X afterward regresses it.
  Captured Y-origin reads in the frame-corner routine, including qualified
  reads, do not improve it and shift later code. Branch-assigned or direct
  navigation-label returns also regress output. Separate high-score lookup
  indices at three declaration scopes, and using the message-duration
  parameter directly, leave the executable unchanged. These trials are
  discarded.
  Only the release and retain rows change in the full report. Normalized
  exact functions rise from 1,371 to 1,372 of 1,550; same-size differences
  fall from 174 to 173. Five size differences remain, totaling a net 26
  extra bytes. Mean mnemonic similarity remains 99.72%, with three functions
  below 90%. No existing exact function is lost, and spans and unit placements
  are unchanged. Strict whole-file matching rises by 22 positions, from
  488,541 to 488,563 of 518,656 (94.1936% to 94.1979%); 30,093 positions
  still differ in 6,004 runs. All gains are in code, now matching at 377,016
  of 401,408 positions. Relocation matching remains 30,652 of 36,352; `.rdata`,
  `.data` and `.idata` remain literally exact. The continuous matching prefix
  remains 544 bytes, with the first difference at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `50c818521448a11a3f50d8bcf5e3100005d24f8456114704687caf5dafa3dff3`.
- `ReturnToBriefingLongShot` captures the current character's activity byte
  through an explicit pointer before its random activation test, and retains
  that pointer through the draw call. This restores the original register
  allocation: all nine differing encodings disappear, and the complete
  512-byte span matches literally, up from 503 bytes.
  The added local also changes later compiler decisions. Move `PanToScreen`'s
  existing indices-pointer declaration after its count locals to retain its
  exact 560-byte span. Without that move, two indexed reads use the shorter
  ESI-base encoding instead of the original EBP-base encoding, displacing the
  remaining instructions and producing 115 differing encodings. Placing the
  declaration after the target array or palette pointers does not repair it.
  A separately promoted palette index leaves one literal difference at best.
- In the same context, declare `Dismissed`'s existing left coordinate before
  its podium-frame local. This recovers SI for the podium frame and the
  original initialization sequence. Correct the reverse left-panel velocity
  index to `11 - (frame - 24)`: on frame 25 the original reads entry 10, not
  entry 11. Its address calculation is `0x0046e50c - frame * 4`, with the
  table beginning at `0x0046e480`; the previous expression emitted
  `0x0046e510 - frame * 4`. Normalized address comparison concealed this
  one-entry error. Differing encodings fall from eighteen to nine, mnemonic
  similarity rises from 97.39% to 100%, and literal matching improves from
  541 to 567 of 576 bytes. The remaining differences exchange BX/DI for the
  left-panel and podium coordinates. The compiler-context change also removes
  one differing encoding in `SceneDirector`, improving literal matching from
  1,133 to 1,134 of 1,136 bytes; two reversed comparisons remain.
  Moving only the briefing character declaration does not improve output.
  Scoping the activity pointer to the inner loop fails to make the briefing
  routine exact. Other dismissal declaration orders give no further gain.
  Reversing the wave-list equality, swapping its local declarations, and
  spelling its search as a break loop leave output unchanged. Pilot-prompt
  coordinate expressions and qualified reads/stores do not improve the file;
  a qualified X read reaches 100% mnemonic similarity but loses 42 literal
  positions. Component-index captures regress neighboring code without
  improving the repair routine. Qualifying the empty-save objective-position
  copy leaves output unchanged. These probes are discarded.
  Only the briefing, dismissal and scene-director rows change in the full
  report. Normalized exact functions rise from 1,372 to 1,373 of 1,550;
  same-size differences fall from 173 to 172. Five size differences remain,
  totaling a net 26 extra bytes. There are now 1,442 mnemonic-perfect
  functions; mean similarity remains 99.72%, with three below 90%. No exact
  function is lost, and spans and unit placements are unchanged. The four
  inspected routines retain their original 21, 32, 63 and 1 relocations.
  Strict whole-file matching rises by 37 positions, from 488,563 to 488,600
  of 518,656 (94.1979% to 94.2050%); 30,056 positions still differ in 5,988
  runs. Code gains 36 positions, reaching 377,052 of 401,408; relocation
  matching gains one, reaching 30,653 of 36,352. The `.rdata`, `.data` and
  `.idata` sections remain literally exact. The continuous matching prefix
  remains 544 bytes, with the first difference at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `1e204bcaaf11ca80199a9a29080a56899edfad15f891b56a590e8c63fb20fd43`.
- `ShowTrainSimHighScores` initializes its row counter immediately before
  the score loop, after setting the text alignment. Moving that existing
  assignment out of the viewport setup lets CL hoist the zero into the
  original argument-preparation position and recover the two rectangle
  copies' register choices. All 103 differing encodings disappear, mnemonic
  similarity rises from 98.67% to 100%, and the complete 880-byte span
  matches literally, up from 520 bytes. Its 69 relocations retain the
  original count and now also occupy the original function-relative positions.
  Initializing the row after the score-buffer allocation or immediately
  before the heading produces the same executable. Moving it only after the
  font-context initializer leaves 24 differing encodings and gains 248 file
  bytes; moving the declaration does not improve that intermediate result.
  Qualified rectangle copies leave the earlier output unchanged. Replacing
  either copy with `memcpy` gives inferior results and is not retained.
  Other trials are discarded. Explicit seek-offset captures and a result
  initializer leave the file-seek routine unchanged. Front-spot position
  pointers and promoted indices, communication-class captures, and navigation
  label coordinate/index captures regress the file. Reversed label comparisons,
  captured colon flags, and moved/scoped packet-table declarations do not
  improve their targets. Explicit clock assignments on the input routine's
  conditional paths introduce an extra clock load and sixteen code bytes.
  A qualified explosion offset combined with a promoted debris index still
  drops the original saved EBP and fails to recover its instruction sequence. Qualifying
  navigation-pointer initialization stores gives no gain.
  In the calibration loader's current Y-before-X product order, qualified
  scale reloads recover all 34 original relocations but enlarge the span from
  784 to 800 bytes. Its RET is at `0x00431d22`, three bytes after the
  original `0x00431d1f`, crossing the next alignment boundary. Qualifying
  only the Y-bound stores and minimum read gives
  32 relocations at that same oversized span; combining the two changes gives
  34. All three shift subsequent code and are restored, leaving this as
  investigation evidence rather than a retained correction.
  Only the high-score row changes in the full report. Normalized exact
  functions rise from 1,373 to 1,374 of 1,550; same-size differences fall
  from 172 to 171. Five size differences remain, totaling a net 26 extra
  bytes. There are now 1,443 mnemonic-perfect functions; mean similarity
  remains 99.72%, with three below 90%. No exact function is lost, and
  function spans and unit placements are unchanged.
  Strict whole-file matching rises by 359 positions, from 488,600 to 488,959
  of 518,656 (94.2050% to 94.2742%); 29,697 positions still differ in 5,981
  runs. Code gains 360 positions, reaching 377,412 of 401,408. The serialized
  relocation section loses one matching position, reaching 30,652 of 36,352,
  despite the corrected function's relocation positions now matching exactly.
  The `.rdata`, `.data` and `.idata` sections remain literally exact. The
  continuous matching prefix remains 544 bytes, with the first difference
  at `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/56. The clean-build and post-verification hashes agree:
  `15a7a819ad37a63c1b796dcecec5edb0a07a0125b57c9cdc613b1dd0217d7d9d`.
- `LoadJoystickCalibrationFile` can recover all 34 original relocations
  without crossing its 784-byte alignment boundary. Reload the up and left
  scales through qualified reads into their existing locals, then multiply
  the original short range parameters instead of the promoted range locals.
  Both products fold their scale operands into memory-source multiplies.
  Its RET is now one byte before the original, rather than three bytes
  beyond it; alignment retains the original next function address. Mnemonic
  similarity rises from 94.72% to 98.77%, differing encodings fall from 86
  to 83 of 243, and literal matching rises from 538 to 649 bytes.
  Reloading the scales while retaining the promoted-range products still
  exceeds the boundary by one byte. Product order, centre-store placement,
  unsigned product casts and qualified zero-scale increments give no better
  candidate. These qualifiers reproduce measured reads, not recovered
  original source spelling.
- That loader correction must be evaluated with the earlier damage and
  flight-control reference corrections. Alone it loses 8,536 matching file
  positions because the serialized relocation table moves. Combining only
  the loader and damage changes still loses 4,555 positions; combining only
  the loader and controls loses 7,922. All three together gain 4,088 before
  repairing two neighboring ship routines. Individual whole-file losses
  therefore did not invalidate these original-count corrections.
  In `internal_damage`, retain a branch-scoped pointer to the capital ship's
  accumulated damage across both side-specific updates and the capacity
  test. This reproduces the original persistent address and removes its
  extra reference: 31 relocations, versus 32 before. The correct function
  span ends at `revise_shields`, `0x0041f1a0`, and is 896 bytes; extending it
  to `your_internal_damage` would incorrectly count seven following
  references. Differing encodings fall from 225 to 118 of 266, mnemonic
  similarity rises from 96.99% to 98.12%, and literal matching rises from
  216 to 613 bytes.
  Retain the previously tested flight-control captures for the wingman,
  selected weapon, missile-camera value and branch-local signed view-mode
  byte, the qualified communication-key guard, and the common laser-fire
  return. Its 174 relocations now match the original count, versus 176
  before. Every mnemonic matches, but its initial AL key difference remains
  two bytes shorter than the original CL sequence; two volume loads and
  pushes also use the other register. Literal matching falls from 1,149 to
  424 of 3,184 bytes, and differing encodings rise from 593 to 768 of 789.
  Captured signed or unsigned key differences, including a scoped capture,
  do not repair that sequence. The three target routines recover the
  original relocation counts, not all original relocation positions.
- The added capital-damage pointer changes later compiler choices. Give
  `Create_ship_hit_debris` an explicit entry guard and a do/while loop,
  keeping its existing created-count declaration before the debris local.
  The guard restores all 128 bytes of `analyze_kill`; the declaration order
  restores the debris loop's comparison direction and all 192 debris bytes.
  These two corrections recover another 23 file positions. Moving the
  vector declaration after both counters gives the same result. Pointer
  declaration placement and a more specific type-local name do not improve
  damage output. A captured type-data pointer shortens its aligned span;
  an inline system-choice expression disturbs later layout. Reversed ship
  comparisons, an explicit side difference and shock-wave declaration moves
  give no further gain. The medal display's separate row increment still
  shrinks its aligned span; qualifying that update preserves its size but
  worsens matching. These alternatives are discarded.
  One normalized-exact neighbor is lost: `explosion_shock_wave` reverses an
  equivalent comparison and matches 351 of 352 bytes. A raw audit of all
  120 functions in the three changed units also finds an arithmetic reorder
  hidden by normalization: `calculate_damage_level` exchanges the independent
  rear and right armour terms, matching 236 of 240 bytes while its report
  still says `EXACT`. No other function's literal match count changes.
  Four normalized report rows change. There are 1,373 exact functions,
  172 same-size differences and five size differences totaling a net 26
  extra bytes. There are 1,444 mnemonic-perfect functions; average similarity
  remains 99.72%, with three below 90%. Spans and unit placements are unchanged.
  Strict whole-file matching rises by 4,111 positions, from 488,959 to
  493,070 of 518,656 (94.2742% to 95.0669%). There remain 25,586 differing
  positions in 3,529 runs. Code loses 222 matching positions, reaching
  377,190 of 401,408; the relocation section gains 4,333, reaching 34,985
  of 36,352. The `.rdata`, `.data` and `.idata` sections remain literally
  exact. The continuous matching prefix remains 544 bytes, with the first
  difference at `0x220`. Total HIGHLOW entries rise from 15,444 to 15,445,
  against the original 15,446; `animate_shape` still has fifteen against
  the original sixteen.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips. Call diagnostics remain 11; global-access
  diagnostics fall from 56 to 55. The clean-build and post-verification
  hashes agree:
  `8b5d7a376263aca53b11a56c1c501fcf66bd7cb0175e1c114216b8ac736a0994`.
- `auto_pilot_sequence` initializes its nearest-ship range after obtaining
  the destination distance. This changes CL's scheduling of the constant
  initialization to match the original, including the argument preparation
  around the two preceding calls. Declare its final four short locals as
  other ship, destination range, cruise speed, then nearest-ship range.
  This restores the nearest-range and loop-index registers and reduces
  differing encodings from seventeen to five of 419. Every mnemonic now
  matches, up from 98.81%; literal matching rises from 1,427 to 1,467 of
  the unchanged 1,472-byte span. The remaining differences reverse one
  comparison and exchange BX/SI for the destination range and ship-class
  constant. All 64 relocations now occupy their original relative positions.
  Moving only the initialization gains 32 file bytes but leaves fourteen
  differing encodings; the selected local order gains another nine.
  Initializing the other-ship index before the distance call regresses
  output. The other four tested permutations of the three range/index
  declarations give smaller gains.
- `print_subtitle` stores each `GetLineLength` result in its existing X
  local before centering that coordinate. The helper already returns a
  short, so this adds no narrowing beyond the original call interface.
  Separating these expressions lets CL push the argument before loading
  the constant 320 at both calls, matching the original scheduling.
  All six differing encodings disappear, mnemonic similarity rises from
  96.97% to 100%, and all 352 bytes match literally, up from 338.
  Capturing the width in a separate int local, declared first or last,
  also makes this target exact; the existing-local form needs no new local.
  Its seven relocations remain at their original positions.
  Other bounded trials are discarded. Reordering the damage-level armour
  terms leaves the emitted order unchanged; moving its damage declaration
  loses four file bytes, and qualified armour reads shorten its span.
  Unsigned or qualified flight-control key comparisons leave output unchanged.
  Capturing or qualifying barracks labels, copying the bunk row before
  division, and qualifying or negating the scramble-detail loop bound give
  no gain. Qualified camera-vector comparisons leave their reversed loads
  unchanged. Capturing the animation scale with qualified reads, stores or
  both still fails to recover its missing relocation and regresses matching.
  Shard-local order, scope and initializer changes leave output unchanged.
  Only the autopilot and subtitle rows change in the full report. A raw
  audit of all 25 functions in their two units also confirms that only those
  two emitted spans change. Normalized exact functions rise from 1,373 to
  1,374; same-size differences fall from 172 to 171. Five size differences
  remain, totaling a net 26 extra bytes. There are 1,446 mnemonic-perfect
  functions, with mean similarity 99.73% and three below 90%. Spans and
  compilation-unit placements remain unchanged.
  Strict whole-file matching rises by 55 positions, from 493,070 to
  493,125 of 518,656 (95.0669% to 95.0775%). There remain 25,531 differing
  positions in 3,519 runs. Code gains 54 matching positions, reaching
  377,244 of 401,408; relocations gain one, reaching 34,986 of 36,352.
  The `.rdata`, `.data` and `.idata` sections remain literally exact.
  The uninterrupted matching prefix remains 544 bytes. Its first difference
  at `0x220` is the relocation section's virtual size: 36,304 bytes rebuilt
  versus 36,306 original. Both raw relocation sections are 36,352 bytes.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `c0e7c80616d966b6333ae776e2bc46e1fc7c71f82e03ce312cccb7d178ff2173`.
- `ShowOnScreenMessage` initializes its modal-state local after formatting
  the text, immediately before flushing input. This restores the original
  zero/duration register allocation and removes all fourteen differing
  encodings. Mnemonic similarity rises from 97.50% to 100%, and the complete
  256-byte span matches literally, up from 240. Its next function is
  `ShowGamePausedBanner` at `0x004290a0`; using `0x004290b0` would incorrectly
  include sixteen bytes and one relocation from that neighbor. The message
  routine itself has seven relocations, all at their original positions.
  Moving the initialization just after the flush produces the same result;
  exchanging its two short declarations alone leaves output unchanged.
- `GameFlow` declares the flown series before the flown mission. This
  restores the original instruction order, raising mnemonic similarity
  from 99.28% to 100% and reducing differing encodings from 21 to 12 of
  279. Literal matching rises from 1,144 to 1,156 of 1,168 bytes. The
  remaining differences exchange EBX/EDI for zero and minus one during
  initial state resets. All 80 relocations retain their original positions.
  Moving the initial room or launch assignments, exchanging the next-series
  and next-mission declarations, and moving other selection/result locals
  give no further gain.
- `ShowTigerClawEscapeScene` captures the sprite scale and projected Y
  coordinate in real locals before drawing. Computing these divisions
  separately restores the original instruction lengths around the draw
  call and gains 552 file bytes. Initializing the existing depth local
  after the opening text-format call, before the frame flag assignment,
  gains another three. Mnemonic similarity rises from 93.98% to 97.69%,
  differing encodings fall from 159 to 31 of 216, and literal matching
  rises from 336 to 860 of the unchanged 928-byte span. Its 52 relocations
  retain the original count; their relative positions are not all exact.
  Register allocation and coordinate-load scheduling still differ.
  Capturing only the scale gives no literal gain. Capturing X as well,
  substituting X for Y, and initializing depth or both motion locals before
  the opening format call give smaller gains. Reversing the Y addition or
  moving the scale declaration gives the same result as the initial
  scale/Y capture; moving the depth declaration loses one file byte.
  Other bounded trials are discarded. Flight-control repeat-flag branch
  forms, unsigned flags and local scopes do not repair its short initial
  key sequence. Moving the fade byte-count calculation before its guard
  regresses matching. Passing the existing zero-valued input-change local
  instead of a zero constant leaves menu polling unchanged. Separate
  branch-local joystick double-click flags lose two file bytes.
  Only these three target rows change in the full report. Direct raw
  before/after comparisons of all 70 functions in their units also confirm
  that only those three emitted spans change. Normalized exact functions
  rise from 1,374 to 1,375; same-size differences fall from 171 to 170.
  Five size differences remain, totaling a net 26 extra bytes. There are
  1,448 mnemonic-perfect functions, with mean similarity 99.73% and three
  below 90%. Spans and compilation-unit placements remain unchanged.
  Strict whole-file matching rises by 583 positions, from 493,125 to
  493,708 of 518,656 (95.0775% to 95.1899%). There remain 24,948 differing
  positions in 3,463 runs. Code gains 552 matching positions, reaching
  377,796 of 401,408; relocations gain 31, reaching 35,017 of 36,352.
  The `.rdata`, `.data` and `.idata` sections remain literally exact. The
  continuous matching prefix remains 544 bytes, with the same relocation
  virtual-size difference at file offset `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `a33c100bff73adb5b561d1ab17fad30e1062411f5d131cd4b7196380577c580f`.
- A further 29 bounded trials across ten routines yield no retained change.
  In `AllocateGuardedMemory`, capturing the allocated-byte total in a
  function local, a scoped local or the now-unused size parameter leaves
  its three register differences unchanged. `ReleaseNearHeapBlock` likewise
  keeps its reversed final comparison after exchanging its locals, reusing
  the next-descriptor local for the final bound, or qualifying that read.
  Moving the constellation index declaration last loses ten file positions;
  an explicit guard and do/while particle loop loses eight. Moving the
  fleet key read inside its view guard, with either local scope, loses
  nineteen positions and changes instruction scheduling.
  All five alternative cockpit-light coordinate/frame declaration orders
  regress matching; one also shortens the span by sixteen bytes and loses
  a relocation. Reversing the VDU length product or exchanging length and
  extent declarations leaves output unchanged. Capturing the communication
  object's class within its guard loses eighteen file positions; reading
  it before the guard loses 48. These changes are not retained.
  Inlining the pilot-field X-coordinate addition, in either operand order,
  loses one file position; qualifying the original left-bound read leaves
  output unchanged. Saving the generated high-score product in the existing
  range or a new score local, and explicitly capturing its loop divisor,
  leaves the executable unchanged. Promoted mission indices declared first
  or last, and a branch-local warp-vector copy, each lose 110 file positions
  while leaving the mission initializer's report row unchanged. Exchanging
  its target and spot declarations leaves the executable unchanged.
  Every probe is restored and force-rebuilt. The final executable retains
  the preceding verified hash and 493,708 matching positions; no additional
  source changes or broad verification reruns are needed for this pass.
- `control_speed` keeps the acceleration-times-travel-time product in a
  separate int local. Use the braking-margin local first for the desired
  minus current speed, then subtract its absolute value from that product.
  This allows CL to reuse the speed-address register after its last read:
  EDI holds the product, and the extra saved EBP disappears. All 63 differing
  encodings are resolved, mnemonic similarity rises from 91.55% to 100%,
  and the complete 176-byte span matches literally, up from twenty bytes.
  Its one relocation now occupies the original position, gaining one more
  file byte. Merely retaining an explicit speed pointer leaves output
  unchanged. Capturing the unscaled acceleration and delaying multiplication
  until after the difference gives only 48 matching code bytes and retains
  EBP; it is superseded by the correctly sequenced product. Reusing the range
  parameter for the quotient gives the same selected target and file score;
  the retained version keeps the existing travel-time local.
- `playWAVE` declares the file-data pointer before the wave-table pointer.
  This restores EBX for the file data and EBP for the descriptor and later
  wave-table entry. It also recovers the longer EBP-relative name accesses
  and the original final return position. All 35 differing encodings
  disappear under normalized comparison. Literal matching rises from 358
  to 414 of 416 bytes, gaining 56 file positions. The two remaining bytes
  are relative-call displacements to `ix_sound_set_delete_on_stop`: its
  original entry is `0x00448645`, while the rebuilt entry remains one byte
  early. The WAV routine's three relocations retain their original positions.
  Moving the wave-table declaration last also reaches this target score;
  placing the file descriptor before the data pointer or moving the data
  pointer last leaves the earlier executable unchanged.
- `LoadAnswerPromptAndResponse` declares its line counter before the three
  line pointers. This restores DI for the counter and gains one literal
  byte, from 47 to 48 of the unchanged 160-byte span. Differing encodings
  fall from 46 to 45 of 59; mnemonic similarity remains 93.22%. The scan
  and prompt pointers and scheduling still differ. Placing the counter
  between pointer declarations leaves the earlier output unchanged;
  explicitly initializing it before the fetch call loses thirteen file
  positions. There are no relocations in this routine.
  Other bounded trials are discarded. Narrowing the release-weapon
  hardpoint local leaves output unchanged; capturing both display
  coordinates retains its extra register and loses 103 file positions.
  Delaying the chalkboard viewport assignment until just before font
  initialization improves mnemonic similarity but loses one file byte.
  Moving its swap declaration loses two; reusing the display pilot local
  during sorting adds sixteen code bytes. A scoped current-pilot index
  loses eighty file positions and disturbs `ReadPacketSectionData`.
  Only the three retained target rows change in the full report. Direct
  before/after raw comparisons of all sixty functions in their units
  confirm that only those three emitted spans change. Normalized exact
  functions rise from 1,375 to 1,377; same-size differences fall from 170
  to 168. Five size differences remain, totaling a net 26 extra bytes.
  There are 1,449 mnemonic-perfect functions; mean similarity rises to
  99.74%, with three below 90%. Spans and unit placements are unchanged.
  Strict whole-file matching rises by 214 positions, from 493,708 to
  493,922 of 518,656 (95.1899% to 95.2311%). There remain 24,734 differing
  positions in 3,441 runs. Code gains 213 positions, reaching 378,009 of
  401,408; relocations gain one, reaching 35,018 of 36,352. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The uninterrupted
  matching prefix remains 544 bytes, with the same relocation virtual-size
  difference at file offset `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `3056dac369920a05200dad723b1c06e45d184ef5aed9665c3c9bc5056150d443`.
- `ejection_sequence` uses a single-case switch for the frame-ten event.
  This produces the original promoted frame comparison, recovering the two
  missing bytes before the explosion call and realigning the final cleanup.
  Initialize the second phase's Y and frame after its sound call, and the
  third phase's frame after `SetMusBreakpt`. These placements recover the
  original scheduling of the constant loads. Declare the frame before the
  background pointer and the ejection-shape pointer after Y to recover the
  registers in both sprite phases. Together these changes resolve all 88
  differing encodings, raise mnemonic similarity from 97.83% to 100%, and
  match all 1,344 bytes, up from 1,098. The 74 relocations now also occupy
  their original positions. The switch alone gains 196 file positions;
  the complete change gains 257. Moving the background after Y instead
  leaves seven differing encodings and is not retained.
- `Msit_n_spin` clears its advance flag after the point-at-destination call
  in case zero and the speed-approach call in case three. This restores the
  original zeroing schedule and separate stack cleanup on those paths,
  reducing differing encodings from 100 to two. Spell the case-seven word
  counter test as preincrement followed by `<= 5`; `< 6` instead produces
  the equivalent but different `CMP 6`/`JGE` pair. With the original bound,
  every mnemonic and byte matches: all 512 bytes, up from 188. Its 23
  relocations now occupy their original positions. The complete change
  gains 340 file positions. Moving only the point-call flag assignment
  gains 127 positions; moving only the speed-call assignment gains one.
- `ShowCampaignVictorySequence` captures the signed vertical-offset
  quotient in an int before preparing the planet-overlay arguments.
  This recovers the original division/argument scheduling and removes the
  four-byte displacement through the following code. Capturing the complete
  narrowed screen Y instead regresses matching and is discarded. Initialize
  the vacant-slot search's pointer, slot and found count in that order.
  For the final animation, declare elapsed time before the animation frame,
  initialize the frame after clearing the viewport, and initialize elapsed
  time after waiting for scene advance. These recover the original saved
  registers and initialization schedule in that phase.
  Mnemonic similarity rises from 96.99% to 100%; differing encodings fall
  from 154 to eighteen of 366. Literal matching rises from 843 to 1,336
  of 1,360 bytes. The remaining 24 byte differences are register choices
  in projectile initialization and vacant-slot scanning. All forty
  relocations now occupy their original positions. The complete change
  gains 514 file positions. The quotient capture alone gains 457; moving
  the vacant count last adds five, moving both animation initializations
  adds 43, and exchanging the animation declarations then adds nine.
  Exchanging those declarations before moving the initializations gives
  no gain. A counted for-loop for projectile initialization loses 8,587
  file positions and is discarded. Merely moving the vacant pointer
  between the two counters leaves the quotient-capture output unchanged.
  Both palette-pointer declaration variants and both chained profile-zero
  assignment orders in `RunSpaceFlight` leave the executable unchanged.
  All 26 bounded probes are restored before selecting the final combination.
  Only these three target rows change in the full report. Direct raw
  before/after comparisons of all 57 functions in their units confirm that
  only those three emitted spans change. Normalized exact functions rise
  from 1,377 to 1,379; same-size differences fall from 168 to 166. Five
  size differences remain, totaling a net 26 extra bytes. There are 1,452
  mnemonic-perfect functions, with mean similarity 99.74% and three below
  90%. Spans and compilation-unit placements remain unchanged.
  Strict whole-file matching rises by 1,111 positions, from 493,922 to
  495,033 of 518,656 (95.2311% to 95.4453%). There remain 23,623 differing
  positions in 3,329 runs. Code gains 1,063 positions, reaching 379,072 of
  401,408; relocations gain 48, reaching 35,066 of 36,352. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The uninterrupted
  matching prefix remains 544 bytes, with the same relocation virtual-size
  difference at file offset `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `4e837e7fbb6206541ffcf7c125ac9a09a4a6399304b427f6bccd31e91f1b0a15`.
- `Explosion` skips the shock wave from its later capital-ship branch,
  after reloading the object's class. Testing the class captured at entry
  after both branches is incorrect and keeps that snapshot in EBX across
  calls. Place a shock-wave call in the non-capital ship branch and another
  after the non-ship setup. CL merges the common tail, frees EBX for the
  scaled object index, and emits the original direct jump around the shock
  wave from the capital-ship path. A shared source label also matches this
  function, but changes registers in `fire_missile` and `drop_mine`, losing
  42 and sixteen literal bytes respectively. The two source calls preserve
  both neighbors' exact matches.
  Use `< 3` for the random radio-message test, select the explosion type with
  an if/else, and assign the returned explosion object again in the non-ship
  branch before `set_objects_data`. Each reproduces an observed instruction
  or store. Initialize the replacement-wingman scan after setting the killed
  flag; this restores CX for the counter and resolves the final seven
  encodings. Moving its declaration instead gives no improvement.
  The complete change raises mnemonic similarity from 94.62% to 100%,
  resolves all 123 differing encodings, and matches all 480 bytes, up from
  36. All 22 relocations now occupy their original positions. Including the
  relocation section, this gains 465 matching file positions.
- `fire_turrets` tests the target count with a postdecrement. The original
  stores the returned count, decrements its stack word, and tests the old
  value in AX. A separate decrement followed by comparison with -1 changes
  the instruction sequence and the two stack-local slots. The postdecrement
  restores both, gaining 361 file positions. Literal matching rises from
  30 to 382 of 384 bytes; differing encodings fall from 107 to two, and
  mnemonic similarity rises from 96.52% to 99.13%. The two remaining bytes
  reverse the circular-scan comparison and its equivalent branch condition.
  All nine relocations now occupy their original positions. Exchanging the
  count and weapon declarations, reversing the comparison operands, or
  folding the target-index increment into that comparison gives no further
  improvement.
- `transform_objects_to_your_view` tests the class against
  `OBJECT_CLASS_EXPLOSION` with `>=`, reproducing the original `CMP 5`/`JL`
  boundary instead of `CMP 4`/`JLE`. This gains two literal bytes, reaching
  476 of 864; differing encodings fall from 110 to 109 and mnemonic
  similarity rises from 96.14% to 96.62%.
  The broader clamp reconstruction remains unretained. An unsigned short
  scale local reproduces the original clamp and single common store, but
  exposes the undersized switch table and shortens the full span by sixteen
  bytes. The original table has six entries, including individual no-op
  entries for debris and asteroids. Adding those two empty cases produces
  only five entries, with 48 relocations against the original 49. Changing
  one break to continue, separating the cases in source, regrouping the dust
  frame expression, or capturing its computed base does not recover the
  missing entry. A conditional radius assignment also leaves the output
  unchanged. These trials are restored rather than retaining shifted units.
  Cockpit message captures leave output unchanged; frame captures lose
  three file positions, or five with the message capture. Promoting the
  housekeeping object index into a used int local loses 23 or nine file
  positions depending on declaration order. None is retained.
  All thirty bounded probes are complete. Only the three retained target
  rows change in the full report. Direct before/after raw comparisons of
  all 159 functions in their two units confirm that only those three spans
  change. Normalized exact functions rise from 1,379 to 1,380; same-size
  differences fall from 166 to 165. Five size differences remain, totaling
  a net 26 extra bytes. There are 1,453 mnemonic-perfect functions, mean
  similarity is 99.75%, and three functions remain below 90%. Spans and
  compilation-unit placements are unchanged.
  Strict whole-file matching rises by 828 positions, from 495,033 to
  495,861 of 518,656 (95.4453% to 95.6050%). There remain 22,795 differing
  positions in 3,285 runs. Code gains 798 positions, reaching 379,870 of
  401,408; relocations gain thirty, reaching 35,096 of 36,352. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The uninterrupted
  matching prefix remains 544 bytes, with the same relocation virtual-size
  difference at file offset `0x220`.
  The clean native build, modern build, source/address audits, order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `53c55a599cdbeac580daf364adb871542b722ab692342af47f68f967a69c264b`.
- `OpenPacketSection` captures the directory entry's compression byte before
  clearing the handle's final-section flag. This restores the original entry
  load before the handle-pointer load, resolving two differing encodings and
  gaining four literal bytes, from 290 to 294 of 320. Its single relocation
  retains its original position. Exchanging the file-size and section-end
  declarations does not improve the remaining fourteen differing encodings.
- `DecompressPacketSection` qualifies the supplied-workspace branch's input-
  size store as volatile. Together with the existing qualified allocation-
  size local, this preserves the original word store before the local size
  initialization. This is a matching source choice, not proof of the original
  qualifiers. It gains twelve code bytes, from 729 to 741 of 752, and one
  relocation-section byte. All 32 relocations now occupy their original
  positions. The remaining eleven differing encodings exchange the packet
  and allocation-size stack slots. Moving the packet declaration before the
  size declaration gives no further gain.
  Other bounded trials are discarded. Explicit packet-handle count captures
  and a for-loop give no gain; an advancing entry pointer loses 43 file
  positions and adds an unwanted fourth relocation. Visibility shape-pointer
  captures and promoted shard-index captures leave their target scores
  unchanged but lose fourteen file positions in their unit. Capturing the
  scramble background shape loses 55. Moving the gun-search found-flag
  initializer first leaves the literal count unchanged while reducing
  mnemonic similarity; moving it after the count loses 91 positions.
  Direct goal-array comparisons do not improve the trimming routine's score
  and lose 461 file positions overall. Capturing joystick button bytes loses
  two positions; capturing its first device index gives no improvement.
  Splitting the largest-heap-block base calculation, qualifying the scene
  animation stop local, and moving its cursor declaration first give no gain.
  All 23 probes are complete. Only the two retained function rows change
  in the full report. Direct before/after raw comparisons of all seventeen
  functions in `view.c` confirm that only those two spans change. Both target
  routines retain 100% mnemonic similarity. The full report remains at 1,380
  normalized exact functions, 165 same-size differences and five size
  differences totaling a net 26 extra bytes. Mean similarity remains 99.75%.
  Strict whole-file matching rises by seventeen positions, from 495,861 to
  495,878 of 518,656 (95.6050% to 95.6083%). There remain 22,778 differing
  positions in 3,277 runs. Code reaches 379,886 of 401,408 matching positions;
  relocations reach 35,097 of 36,352. The `.rdata`, `.data` and `.idata`
  sections remain literally exact. Spans, compilation-unit placements, the
  544-byte uninterrupted matching prefix and the header difference at
  file offset `0x220` are unchanged.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `a44f21bba92f8e1ccf626fa24a8b0988505741e73ccdc6e3b624cefd9c313909`.
- `StepPaletteTransition` captures its palette pointer and component count
  separately on the initialization and update paths. This places the palette
  load before the first loop's positive-count test, eliminating the separate
  pointer load and jump on its zero-count path. Declare the index before the
  difference: CL then keeps the scaled index in EAX and the component delta
  in DX, reproducing the original `ADD EAX,EAX` and the complete first loop.
  The phase captures alone gain 31 file positions; the declaration order
  raises that gain to 258. Moving the difference declaration first retains
  only the smaller gain. These are measured source choices, not recovered
  original local declarations.
  Differing encodings fall from 133 to 63 of 154, and mnemonic similarity
  rises from 94.19% to 97.40%. Literal matching rises from 106 to 347 of
  560 bytes. All 35 relocation operands remain; 22 now occupy their original
  positions, up from six. The component-count load is still hoisted into the
  prologue instead of being repeated on the two paths, and the final update
  loop still uses different registers. Qualifying the conditional input
  reads or the count parameter does not prevent that hoist. Capturing the
  accumulated value gives no literal gain and reverses another comparison;
  a countdown postdecrement also gives no gain.
  The other bounded trials are discarded. Restricting decompression's size
  qualifier to its accesses does not recover its exchanged stack slots.
  Reusing the prepared-shape size local for the frame offset leaves output
  unchanged; a separate offset local worsens the complete executable.
  Qualifying the sprite-copy length reads enlarges each sprite routine by
  sixteen bytes; also qualifying the following cursor-advance read enlarges
  each by 48 bytes. Neither preserves the original layout.
  All seventeen probes are complete. Only the palette routine's row changes
  in the full report, and direct raw before/after comparisons of both
  functions in `fade.c` confirm that its neighbor is unchanged. The report
  remains at 1,380 normalized exact functions, 165 same-size differences and
  five size differences totaling a net 26 extra bytes. There are 1,453
  mnemonic-perfect functions, mean similarity is 99.75%, and three remain
  below 90%. Function spans and compilation-unit placements are unchanged.
  Strict whole-file matching rises by 258 positions, from 495,878 to
  496,136 of 518,656 (95.6083% to 95.6580%). There remain 22,520 differing
  positions in 3,249 runs. Code gains 241 positions, reaching 380,127 of
  401,408; relocations gain seventeen, reaching 35,114 of 36,352. The
  `.rdata`, `.data` and `.idata` sections remain literally exact. The
  uninterrupted matching prefix remains 544 bytes, with the same header
  difference at file offset `0x220`.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `38a5f57eb1dae6b648024633da8f4ce3022df616f81367ea8778393c1172a9d8`.
- `LoadGame` needs the same qualified scalar reads and stores as the save
  writers to preserve the original campaign-field interleaving. This restores
  the current-pilot, ship-type, mission/series, date and score transfers without
  changing the record copies or counted byte loops. These are matching source
  choices, not evidence of the original qualifier spelling. Initialize the
  disk death-mission pointer before the runtime pointer, but declare the
  runtime pointer first. The original then loads the source into ESI and the
  destination into EDX, and the complete widening loop matches.
  Qualified transfers gain 95 literal bytes, initialization order adds six,
  and declaration order adds seven. Literal matching rises from 492 to 600
  of 624 bytes; differing encodings fall from 33 to nine of 162, and mnemonic
  similarity rises from 97.53% to 98.77%. The remaining differences involve
  the description's last byte, four commuted array-address encodings, and the
  objective-pointer calculation's scheduling. Qualifying the occupied flag
  gives no further gain. Moving objective-pointer initialization after the
  mission-score transfer loses 48 positions against the selected loader.
- `MainWindowProc` declares the secondary mouse-button local before the
  primary one. This restores EBX for the primary button, gaining four literal
  bytes, from 1,225 to 1,229 of 1,264. Differing encodings fall from 28 to 24
  of 278; mnemonic similarity stays at 98.56%. The full message parameter
  and secondary button still exchange registers. Signed button locals and
  earlier command initialization give no additional improvement. Capturing
  the full message parameter in a separate local worsens the result.
  Other bounded trials are discarded. Packet-search count captures leave
  output unchanged; an explicit cursor loses 43 positions and adds an unwanted
  fourth relocation. Reordering the weapon hardpoint declaration gives no
  gain, while coordinate captures leave that target unchanged and lose 103
  file positions elsewhere in its unit. Qualifying the font row-table reads
  leaves output unchanged; branch-local table loads enlarge the routine by
  32 bytes. Input timestamp captures on separate branches add sixteen bytes.
  Capturing either barracks-detail pointer gives no gain. Navigation-label
  coordinate captures improve their target but regress neighboring functions;
  an unsigned placement result leaves output unchanged. None is retained.
  All 26 probes are complete. Only the two selected rows change in the full
  report. Direct before/after raw comparisons of all 74 functions in their
  units confirm that only those two spans change. Their one and fifty
  relocation operands respectively retain their original positions. The
  report remains at 1,380 normalized exact functions, 165 same-size differences
  and five size differences totaling a net 26 extra bytes. There are 1,453
  mnemonic-perfect functions, mean similarity is 99.75%, and three remain
  below 90%. Spans and compilation-unit placements are unchanged.
  Strict whole-file matching rises by 112 positions, from 496,136 to
  496,248 of 518,656 (95.6580% to 95.6796%). There remain 22,408 differing
  positions in 3,225 runs. All gains are in code, which reaches 380,239 of
  401,408 matching positions. Relocations remain at 35,114 of 36,352; `.rdata`,
  `.data` and `.idata` remain literally exact. The 544-byte uninterrupted
  matching prefix and the header difference at file offset `0x220` are unchanged.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `12546759bf2d75046ca4189e9a9e96b577448a3ed43956f9c14197a818503d05`.
- `inflict_damage` uses a signed short quadrant. The original sign-extends
  it both after the forward-vector dot product and before the armor lookup;
  an unsigned declaration adds masking instructions and displaces the body.
  Correcting the type gains 387 file positions. Qualifying both the armor-zero
  store and the following screen-X read preserves their original order and
  gains fourteen more; qualifying only the store leaves output unchanged.
  These qualifiers are matching source choices, not recovered declarations.
  The remaining branch at `0x0041EA43` originally reaches the epilogue at
  `0x0041EC4A`, skipping the return-register clear. The non-ship survivor path
  therefore returns without supplying a value. An explicit bare return and
  a fallthrough variant both reproduce that branch and gain two positions.
  Keep the explicit path, with the modern port retaining its defined zero
  return. The complete 688-byte span now matches literally, up from 295 bytes;
  all twenty relocation operands occupy their original positions, up from
  ten. Differing encodings fall from 128 to zero of 220, and mnemonic
  similarity rises from 97.30% to 100%.
- `Title_Sequence` captures the mouse coordinates into integer locals,
  reading Y before X. This restores the original sign-extending word loads
  before the menu-region arguments are pushed. Either declaration order
  produces the same improvement; keep X before Y. Literal matching rises
  from 1,615 to 1,789 of 1,856 bytes, differing encodings fall from 74 to 28
  of 472, and mnemonic similarity rises from 97.25% to 97.88%. All 77
  relocation operands now occupy their original positions, up from 71.
  The keyboard registers and selection-branch ordering still differ. A
  conditional selection expression gives no further gain.
- `init_intelligence_data` qualifies the mission-target read and the
  special-maneuver store. Together they restore the target load immediately
  after `clear_alert`; qualifying the read alone leaves output unchanged.
  Literal matching rises from 250 to 264 of 336 bytes, differing encodings
  fall from thirty to 29 of 75, and mnemonic similarity rises from 93.33%
  to 96.00%. All 28 relocation operands remain, with 25 in their original
  positions instead of 23. Register allocation and vector-copy scheduling
  still differ. Capturing the navigation index as either an integer or a
  short does not improve this routine and worsens the complete unit.
  The remaining bounded trials are discarded. Reordering the intelligence
  locals, replacing its vector assignments with memcpy, reordering the
  hostile-sphere locals or reversing its comparison operands, and reusing
  the other argument local in conversation-test cases six and seven leave
  output unchanged. Moving the target-sort distance declaration after either
  loop index loses 112 file positions.
  All 22 probes are complete. Only these three rows change in the full
  report. Direct before/after raw comparisons of all 112 functions in their
  units confirm that only the selected spans change. All 3,784 mapped symbol
  addresses are unchanged. The report reaches 1,381 normalized exact
  functions and 1,454 mnemonic-perfect functions, with 164 same-size
  differences and five size differences totaling a net 26 extra bytes.
  Mean similarity remains 99.75%, with three functions below 90%.
  Strict whole-file matching rises by 599 positions, from 496,248 to
  496,847 of 518,656 (95.6796% to 95.7951%). There remain 21,809 differing
  positions in 3,195 runs. Code gains 581 positions, reaching 380,820 of
  401,408; relocations gain eighteen, reaching 35,132 of 36,352. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The uninterrupted
  matching prefix remains 544 bytes, with the same header difference at
  file offset `0x220`.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `a471bc4f94941b8a5f1fee7d0774b0a225e02037cca48ff7b8ddbbd346e5b220`.
- `servicetrack` dispatches asteroid and ship effects with a switch on the
  object class. The asteroid case comes first, followed by shared ship and
  capital-ship cases. This reproduces the original early class tests and
  branches before the asteroid body; the former if/else chain placed the
  ship-range tests after that body. Keep the existing distance, visibility,
  countdown and audio-cooldown tests inside their respective cases.
  The complete 464-byte span now matches literally, up from 119 bytes.
  All 94 differing encodings disappear, mnemonic similarity rises from
  91.30% to 100%, and all 23 relocation operands occupy their original
  positions, up from five. The switch gains 345 code positions and eighteen
  relocation-section positions. Its preceding passing-object reset retains
  the original pair of unequal-class tests joined with OR.
- `ReportComponentRepaired` qualifies the decremented damage-byte store
  and the following component-name pointer read. This preserves the original
  store before the name load, although the component-index register and
  message-buffer address scheduling still differ. Literal matching rises
  from 92 to 93 of 112 bytes; differing encodings fall from six to five of
  31, and mnemonic similarity rises from 90.32% to 93.55%. All five relocation
  operands remain, with the same three in their original positions. Qualifying
  only the store gives no gain. A message-buffer pointer at either function
  or branch scope leaves the target score unchanged and regresses its unit.
  These qualifiers are matching choices, not recovered original spelling.
  Other bounded trials are discarded. Qualified maneuver-proximity reads
  leave their target unchanged and regress the unit. Moving the shock-wave
  loop variable after its vectors or distance, declaring the wave-list entry
  first, capturing the intelligence routine's wingman in either declaration
  position, and capturing the barracks label value give no gain. Promoted
  ship-search indices regress the file. Ordered mission-argument capture
  improves that search and makes `close_behind` exact, but regresses
  `evaluate_damage` and the scene-animation goal comparison. An explicit
  packet-reference row cursor enlarges the release routine by sixteen bytes.
  Either flight-input capture order loses four file positions.
  Qualified constellation-definition reloads give no gain; a pointer capture
  loses five positions. Narrowing the palette byte-count scope gives no
  literal gain and reduces mnemonic similarity; separate or promoted palette
  update indices also give no gain. Moving the calibration failure local
  first or last and capturing the HUD-message pointer leave output unchanged.
  Moving the target-bracket valid flag before or after its bounds raises its
  mnemonic similarity but loses ninety file positions. None is retained.
  All 29 probes are complete. Only the two selected rows change in the full
  report. Direct before/after raw comparisons of all 93 functions in their
  units confirm that only those two spans change. All 3,784 mapped symbol
  addresses are unchanged. The report reaches 1,382 normalized exact
  functions and 1,455 mnemonic-perfect functions, with 163 same-size
  differences and five size differences totaling a net 26 extra bytes.
  Mean similarity rises to 99.76%, with three functions below 90%.
  Strict whole-file matching rises by 364 positions, from 496,847 to
  497,211 of 518,656 (95.7951% to 95.8653%). There remain 21,445 differing
  positions in 3,161 runs. Code gains 346 positions, reaching 381,166 of
  401,408; relocations gain eighteen, reaching 35,150 of 36,352. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The uninterrupted
  matching prefix remains 544 bytes, with the same header difference at
  file offset `0x220`.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `e9e8597f36e1a90c6f3cfeca82c72e6feb584da2d2f38d76e70726d31cbe1621`.
- `initialize_cockpit` removes the reconstructed null checks after fetching
  the cinematic backdrop and before releasing the rear-view backdrop. The
  original executes those draw/release paths without testing the fetched
  pointer. Give each of these two load branches its own backdrop local;
  retain the existing function-scope local for modes zero through three.
  This resolves the earlier whole-unit regression: removing the guards alone
  makes the cockpit exact but loses 172 file positions through neighboring
  register allocation. Scoping only one of the two pointers loses 184.
  Scoping both preserves `PlaySceneAnimation` and gains 283 file positions.
  Scoping all six load branches gives the same result, so keep the smaller
  change. These scopes are matching choices, not recovered original spelling.
  The complete 1,088-byte cockpit span now matches literally, up from 827
  bytes. All 84 differing encodings disappear, mnemonic similarity rises
  from 98.70% to 100%, and all 72 relocation operands occupy their original
  positions, up from 57. The gain is 261 code and 22 relocation-section bytes.
- `SetFleetOverviewView` declares the existing range local immediately
  before the maximum-range local. This restores the original `CMP ESI,EAX`
  and `JGE` instead of the reversed comparison and `JLE`. All 752 bytes now
  match literally, up from 750; the two differing encodings disappear and
  mnemonic similarity rises from 99.56% to 100%. Its 36 relocation operands
  retain their original positions. Moving range before player range instead
  gives no improvement.
- `ParseTests` tests the terminating zero code in its while condition and
  returns the current record after the loop. This moves that return after
  the other return blocks, matching its original placement at `0x00438AA3`.
  Preserve the existing pair of switches: both dispatch tables occur in the
  original. Literal matching rises from 1,489 to 1,517 of 2,608 bytes,
  differing encodings fall from 386 to 368 of 820, and mnemonic similarity
  rises from 95.49% to 97.80%. All 76 relocations remain, although the number
  at original positions decreases from 57 to 56. The compiler also reverses
  the text-colour equality comparison in `SceneDirector` at `0x00438DAB`,
  reducing its literal match from 1,134 to 1,133 of 1,136 bytes. Its 63
  relocation operands remain at their original positions. The parser change
  gains 26 file positions overall, including that one-byte neighboring cost
  and one lost relocation-section byte.
  An equivalent for condition produces the same result. Breaking from the
  original infinite loop to a trailing return leaves output unchanged.
  Separate per-case argument scopes lose one more file position through
  `SceneDirector`; reversing the argument-local declarations gives no gain.
  Moving the barracks activation label outside its keyboard switch, placing
  the outer activation cases first, and putting the calibration case first
  leave output unchanged. Capturing the collision object before its partner
  through a qualified read enlarges that routine by 32 bytes. None of these
  alternatives is retained.
  All seventeen probes are complete. Only these three targets and the
  described `SceneDirector` row change in the full report. Direct before/after
  raw comparisons of all 160 functions in their units confirm that only those
  four spans change. All 3,797 mapped symbol/address entries, representing
  3,784 distinct names, retain their addresses. The report reaches 1,384
  normalized exact functions and 1,457 mnemonic-perfect functions, with 161
  same-size differences and five size differences totaling a net 26 extra
  bytes. Mean similarity rises to 99.77%, with three functions below 90%.
  Strict whole-file matching rises by 311 positions, from 497,211 to
  497,522 of 518,656 (95.8653% to 95.9252%). There remain 21,134 differing
  positions in 3,110 runs. Code gains 290 positions, reaching 381,456 of
  401,408; relocations gain 21, reaching 35,171 of 36,352. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The uninterrupted
  matching prefix remains 544 bytes, with the same header difference at
  file offset `0x220`.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `be804c9f7579fe1b09264f2264b84a1f4f01cbe4c5b233367c33a2cfd581e0f7`.
- `SelectNavObjectiveAtPoint` declares the existing X coordinate before Y.
  This restores SI for X and several original instruction positions, although
  Y and the path counter then exchange registers. Literal matching rises
  from 245 to 249 of 272 bytes. Mnemonic similarity rises from 95.29% to
  97.65%, while differing encodings increase from twelve to fifteen of 85;
  retain the change for its literal-byte gain. All eight relocation operands
  keep their original positions. Moving X immediately before map X loses
  two file positions; declaring X first and Y last gives no net gain.
- `AllocateNearHeapBlockByFlags` declares the masked block size before the
  descriptor pointer. This restores the original size comparison and its
  two branch directions at `0x0040EDDD`, but reverses the later descriptor-
  shifting loop comparison. Literal matching rises from 461 to 462 of 464
  bytes, differing encodings fall from three to two of 141, and mnemonic
  similarity rises from 98.58% to 99.29%. All nineteen relocation operands
  retain their original positions. Moving the shift-address declaration
  before the descriptor address or after the allocation address gives no
  further gain. Moving block size just before alignment loses four file
  positions instead. These declaration orders are matching choices, not
  recovered original spelling.
  Other bounded trials are discarded. Promoted input-device indices in
  either declaration position leave their target unchanged and lose two
  file positions. Declaring the palette free count first loses six positions;
  a separate allocation index loses twelve. Narrowing the frame-corner
  vertical locals' scope loses 47 positions, and capturing its Y origin
  leaves the target unchanged while enlarging other code. Qualifying the
  pilot-field alignment store leaves output unchanged. A promoted weapon
  index worsens the selected-weapon query and loses 113 positions.
  Replacing the empty-save objective-position assignment with memcpy, or
  capturing its source position pointer before the name fields, leaves
  output unchanged. Neither is retained.
  Separate ordinary-object coordinate scopes reduce the sorted-object
  draw routine from nineteen to seven differing encodings, but regress
  `intro_drawbackgroundships` and `shrink_vector`, losing thirteen file
  positions overall. Either coordinate declaration order gives that result.
  Moving the special-object coordinates into their own scope also gives no
  improvement. Qualifying the ordinary X, Y, origin-X and shape loads in
  their original order reduces the target to three differing encodings,
  but still loses ten file positions through the neighboring regressions.
  Scoping only X or only Y, with either load order, loses eleven or twelve
  file positions. Using the qualified loads with the existing function-scope
  coordinates loses 224 positions. Sharing a narrower scope inside the
  object-class guard loses 224 with Y declared first and six with X first.
  These sprite variants are all restored rather than retained.
  All thirty probes are complete. Only the two selected rows change in
  the full report. Direct before/after raw comparisons of all 43 functions
  in their units confirm that only those spans change. All 3,797 mapped
  symbol/address entries, representing 3,784 distinct names, are unchanged.
  The report remains at 1,384 normalized exact and 1,457 mnemonic-perfect
  functions, with 161 same-size differences and five size differences
  totaling a net 26 extra bytes. Mean similarity remains 99.77%, with three
  functions below 90%.
  Strict whole-file matching rises by five positions, from 497,522 to
  497,527 of 518,656 (95.9252% to 95.9262%). There remain 21,129 differing
  positions in 3,116 runs. All gains are in code, which reaches 381,461 of
  401,408 matching positions. Relocations remain at 35,171 of 36,352;
  `.rdata`, `.data` and `.idata` remain literally exact. The 544-byte
  uninterrupted matching prefix and the header difference at file offset
  `0x220` are unchanged.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `fe50d3789638d323eb0e6ed997084f45dea75a560355168053ff760086f6a5de`.
- `ReserveContiguousPaletteEntries` clears first entry before free count and
  entry index. This restores the original initial `XOR DX,DX`, `XOR CX,CX`,
  `XOR EAX,EAX` order. Literal matching rises from 103 to 106 of 112 bytes,
  and differing encodings fall from nine to six of 39. The remaining six
  differences are register choices in the final allocation loop. Its span
  and both relocation positions remain unchanged. Reusing entry index for
  that loop or clearing free count last gives no gain. This initialization
  order is a matching choice, not recovered original source spelling.
  A raw audit of all 1,384 normalized-exact functions finds fifteen spans
  with literal differences. They are existing operand-order, register and
  displaced-audio-call differences; normalized exactness does not establish
  literal equality. Qualifying or capturing the two objective-range reads
  creates six differing encodings and loses more than 13,000 file positions.
  The equivalent pilot-frame read trials leave output unchanged. Qualifying
  the next-wave index operands also leaves output unchanged; capturing their
  sum loses 105 file positions. Ordering the free-heap boundary reads loses
  sixty positions, and moving the damage-level declaration first loses four.
  Qualifying the sprite-background run length and capturing its shared
  cursor advance, either in a separate local or the existing skip local,
  enlarges each capture/restore variant by sixteen bytes. Separate locals
  also regress neighboring graphics functions. All four trials are discarded.
  Debug-console declaration order, moving its context before the wait,
  initializing the console in its declaration, and capturing the clock
  function pointer give no literal gain. The clock-pointer trials also
  reduce mnemonic similarity. Two object-colour switch variants lose 209
  and 281 file positions. Capturing enemy side inside the class/manoeuvre
  guard leaves that target unchanged but loses 72 positions elsewhere.
  Six parser trials leave output unchanged: moving the code or cursor
  declarations, returning an advanced scene pointer in case six, case seven,
  or all applicable cases, and reversing pointer-addition operands in case
  six. None is retained.
  All twenty-nine probes are complete. Only the palette-allocation row
  changes in the full report. Direct before/after raw comparisons of all
  four video-unit functions confirm that only its span changes. All 3,797
  mapped symbol/address entries, representing 3,784 distinct names, retain
  their addresses. The report remains at 1,384 normalized exact and 1,457
  mnemonic-perfect functions, with 161 same-size differences and five size
  differences totaling a net 26 extra bytes. Mean similarity remains 99.77%,
  with three functions below 90%.
  Strict whole-file matching rises by three positions, from 497,527 to
  497,530 of 518,656 (95.9262% to 95.9268%). There remain 21,126 differing
  positions in 3,113 runs. Code reaches 381,464 of 401,408 matching positions;
  relocations remain at 35,171 of 36,352. The `.rdata`, `.data` and `.idata`
  sections remain literally exact. The uninterrupted matching prefix remains
  544 bytes, with the same header difference at file offset `0x220`.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `ffe5f1f79f91507364888606d4846e713470eacc4c6ee1fa55bd33c5b2f929e0`.
- `PrepareShapeRLEData` declares the prepared-shape pointer immediately after
  the frame index. This restores the original `CMP EAX,[ESP+0x18]` and `JL`
  at the frame-loop tail, replacing the reversed comparison and `JG`.
  Literal matching rises from 303 to 305 of 656 bytes, differing encodings
  fall from 127 to 125 of 217, and mnemonic similarity rises from 97.24%
  to 97.70%. Its 44-byte local frame, function span and ten relocation
  positions remain unchanged; nine relocation positions match the original.
  This declaration order is a matching choice, not recovered source spelling.
  Reusing the existing prepared-size local for the frame offset gives no
  further gain, and moving the prepared pointer before all other locals
  gives no gain. Separate opaque-run counters in either declaration order
  worsen the target and enlarge neighboring graphics code.
  Other bounded trials are discarded. Qualifying the maneuver-proximity
  limit, alone or with the current range, loses file positions. Narrowing
  the scene-command opcode scope, changing wave-removal declaration order
  or using an explicit search break, capturing the intelligence wingman,
  and moving the shock-wave index after its vectors give no gain.
  Capturing the navigation-square bottom coordinate loses 87 positions.
  A separate palette-upload index gives no gain. Recomputing only the first
  palette-clear byte count enlarges that routine by sixteen bytes; narrowing
  the existing byte-count local's scope gives no literal gain and lowers
  mnemonic similarity. Capturing the first rectangle's left edge in the
  existing X local, with either an ordinary or qualified read, shortens the
  intersection routine by sixteen bytes and regresses the file. Unsigned
  menu change flags with the original signed event conversion, or declaring
  its device index first, give no gain.
  All twenty-one probes are complete. Only the shape-preparation row
  changes in the full report. Direct before/after raw comparisons of all
  sixty graphics-unit functions confirm that only its span changes. All
  3,797 mapped symbol/address entries, representing 3,784 distinct names,
  retain their addresses. The report remains at 1,384 normalized exact and
  1,457 mnemonic-perfect functions, with 161 same-size differences and five
  size differences totaling a net 26 extra bytes. Mean similarity remains
  99.77%, with three functions below 90%.
  Strict whole-file matching rises by two positions, from 497,530 to
  497,532 of 518,656 (95.9268% to 95.9272%). There remain 21,124 differing
  positions in 3,111 runs. Code reaches 381,466 of 401,408 matching positions;
  relocations remain at 35,171 of 36,352. The `.rdata`, `.data` and `.idata`
  sections remain literally exact. The uninterrupted matching prefix remains
  544 bytes, with the same header difference at file offset `0x220`.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `72c10a4a3428e0075d2cda77993382782d4a15bbb788df70fd4e66556f5c39c8`.
- `ReleasePacketHandle` traverses the packet-reference table with one
  advancing pointer, declared before its two loop counters. Keep the
  existing ascending group and entry loops. CL supplies the original
  descending machine counters and allocates ESI to the traversal pointer
  and EDX to the handle. The complete 64-byte span now matches literally,
  up from 51 bytes, and all seven differing encodings disappear. Its one
  relocation moves back to the original position. The traversal follows
  the observed assembly; declaration order is a matching choice, not
  recovered original spelling.
  This compiler context also improves the victory sequence's free-firework
  search, restoring EBX for its record cursor and AX for its object index.
  `ShowCampaignVictorySequence` gains ten literal bytes, from 1,336 to
  1,346 of 1,360, and differing encodings fall from eighteen to fourteen
  of 366. Its forty relocation operands retain their original positions.
  Declaring the release cursor last or writing explicit descending source
  counters gives only eight file positions overall and leaves ten differing
  encodings in the release routine. A separate row cursor instead enlarges
  it by sixteen bytes and adds an unwanted second relocation.
  Other bounded trials are discarded. Moving the packet-search index
  initialization inside its guard or capturing the count in either local
  order gives no gain. A qualified count capture reduces that routine from
  22 to seven differing encodings but regresses `PushMemoryStackFrame`,
  losing thirteen file positions. Qualified or captured barracks-label
  pointers, a long seek result, a qualified ship-side comparison and a
  separate master-range assignment give no gain. A register goal local
  likewise gives no gain; separate axis scopes lose 27 positions. Moving
  the ship-search failure store after its guard loses 39 positions, and
  do/while rout and attacker queries give no gain. Narrowing the viewport
  allocation-index scope gives no gain. Sprite shape/frame captures leave
  their target unchanged but regress the font, corner and bounds helpers,
  including two sixteen-byte increases.
  All twenty-one probes are complete. Only the release and victory rows
  change in the full report. Direct before/after raw comparisons of all
  eight ending-unit functions confirm that only those spans change. All
  3,797 mapped symbol/address entries, representing 3,784 distinct names,
  retain their addresses. The report reaches 1,385 normalized exact
  functions, with 160 same-size differences and five size differences
  totaling a net 26 extra bytes. There remain 1,457 mnemonic-perfect
  functions, mean similarity 99.77%, and three functions below 90%.
  Strict whole-file matching rises by 24 positions, from 497,532 to
  497,556 of 518,656 (95.9272% to 95.9318%). There remain 21,100 differing
  positions in 3,098 runs. Code gains 23 positions, reaching 381,489 of
  401,408; relocations gain one, reaching 35,172 of 36,352. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The uninterrupted
  matching prefix remains 544 bytes, with the same header difference at
  file offset `0x220`.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `7bb1f2d77364b49f459c984183d174531dbdfc6800c26668c9f51f3ccfd909fb`.
- `EstablishingShot` declares its promoted frame index before the character
  counter. This restores EBX for the promoted frame and BP for the character
  loop, removing all seven differing encodings. Its complete 352-byte span
  now matches literally, up from 345 bytes. All sixteen relocation operands
  retain their original positions. Declaring the frame index immediately
  before the frame counter, or scoping it around the character loop, gives
  no gain. This declaration order is a matching choice, not recovered
  original local spelling.
- `ShowCampaignVictorySequence` uses a separate short initialization counter,
  scoped to the projectile-initialization loop. This restores the original
  DX counter, ECX record pointer and EAX sentinel. In the later vacant-slot
  search, initialize the vacant count before the slot index, after assigning
  the record pointer. Together these changes increase literal matching from
  1,346 to 1,353 of 1,360 bytes and reduce differing encodings from fourteen
  to seven of 366. The 376-byte local frame and all forty original relocation
  positions remain unchanged. The remaining differences exchange CX/DX for
  the vacant count and sentinel.
  Changing only the vacant-search initialization order gains one byte.
  Four other initialization orders lose five positions each. Reversing the
  first loop's pointer/counter assignments gives no gain. A function-scope
  initialization counter declared first loses two positions despite reducing
  the target's differing-encoding count; combining it with the new vacant
  order loses three. Declaring that counter after the slot gives no gain.
  Moving the vacant-count declaration first gives no gain, and moving the
  projectile-pointer declaration first loses 906 positions. None of these
  alternatives is retained.
  Other bounded trials are discarded. Capturing the shutdown routine's
  cursor imports, individually or in either combined declaration order,
  loses file positions. Initializing the dismissal podium coordinate before
  the left coordinate loses two positions; initializing the left coordinate
  before the podium frame loses 22. Chaining the disk-prompt rectangle copy,
  copying the second rectangle from the first, and using a local rectangle
  all leave output unchanged. Moving its saved-context declaration last or
  its background-colour declaration first likewise gives no gain.
  All twenty-six probes are complete. Only the victory and establishing-shot
  rows change in the full report. Direct before/after raw comparisons of all
  forty functions in their two units confirm that only those spans change.
  All 3,797 mapped symbol/address entries, representing 3,784 distinct names,
  retain their addresses. The report reaches 1,386 normalized exact
  functions, with 159 same-size differences and five size differences
  totaling a net 26 extra bytes. There remain 1,457 mnemonic-perfect
  functions, mean similarity 99.77%, and three functions below 90%.
  Strict whole-file matching rises by fourteen positions, from 497,556 to
  497,570 of 518,656 (95.9318% to 95.9345%). There remain 21,086 differing
  positions in 3,085 runs. Code gains all fourteen positions, reaching
  381,503 of 401,408; relocations remain at 35,172 of 36,352. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The uninterrupted
  matching prefix remains 544 bytes, with the same header difference at
  file offset `0x220`.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `6c7674a2ca00529211282a4f600e58d581c3addb9707c1253a46c48a85e065f0`.
- `process_player_input` declares the shift-key state before the key cursor.
  This restores ESI for the shift state and EDI for the advancing cursor,
  removing all eleven differing encodings. The complete 1,376-byte span
  now matches literally, up from 1,365 bytes, with all 106 relocation
  operands at their original positions. Moving the shift declaration before
  the key array or moving the cursor declaration last emits the same
  executable. Retain the adjacent declaration swap. These are matching
  declaration choices, not recovered original local spelling.
- `FreeNearHeapBlock` captures each descriptor's address in an integer
  local, declared between the descriptor pointer and the requested address.
  Use that captured value for both interval comparisons. This restores
  `CMP ECX,ESI` and `JG` instead of the reversed comparison and `JL`.
  The complete 96-byte span now matches literally, up from 94 bytes, and
  mnemonic similarity rises from 96.97% to 100%. All four relocation
  operands retain their original positions.
- `prepare_mission` keeps five initial stores and the player-index read as
  qualified accesses: mission score, wingman-killed state, player mission
  index, player-destroyed state, medal score, and wingman kill count. Place
  the medal-score clear before the wingman-kill clear. This prevents the
  player-index load from preceding the initial reset sequence and restores
  the original leading register-save instructions. Literal matching rises
  from 360 to 390 of 416 bytes, differing encodings fall from twelve to
  five of 106, and mnemonic similarity rises from 98.11% to 100%.
  Its relocation count remains 34; offsets coinciding with the original
  increase from 28 to 32. The player-kill clear still moves ahead of its
  original position, and scheduling differences remain. The qualifiers
  express observed memory accesses, not recovered original source spelling.
  Qualifying only the player-index read, reordering only the two score
  stores, or combining those changes gives no gain. Qualifying just the
  leading score/wingman stores and player read gains 24 file positions;
  qualifying every startup access gains 22. Adding only the player-destroyed
  store, or that store and the medal-score store, to the leading group loses
  one position. Adding the wingman-kill store produces the retained gain of
  34 file positions, including four relocation bytes.
  Other bounded trials are discarded. Map slot/pointer declaration changes,
  promoted slot captures in either scope, and narrowing the navigation
  pointer's scope give no gain. Initializing the gun-selection found flag
  first gives no gain; initializing its cursor first loses ten positions.
  Capturing the tactic value leaves its target unchanged and loses 27 file
  positions. Capturing the afterburner timer through a scoped pointer loses
  565 positions. A captured timer value makes the acceleration routine exact
  but regresses `animate_shape`, losing five file positions overall. Its
  scoped form and declarations before or between the vector locals do the same.
  Reusing the existing object-data value local for scale gives no gain.
  Capturing pilot health leaves that target unchanged and regresses three
  later ship functions, losing sixty positions. Enemy-range captures in
  either declaration order leave their target unchanged and lose 29.
  Moving the target-sort distance declaration first gives no gain. A promoted
  inner index makes both the sort and pilot-hit routines exact but regresses
  four other ship functions, losing forty positions overall. Narrowing the
  asteroid fragment-counter scope gives no gain.
  All thirty-two probes are complete. Only the input, heap-release and
  mission-preparation rows change in the full report. Direct before/after
  raw comparisons of all 106 functions in their three units confirm that
  only those spans change. All 3,797 mapped symbol/address entries,
  representing 3,784 distinct names, retain their addresses. The report
  reaches 1,388 normalized exact and 1,459 mnemonic-perfect functions, with
  157 same-size differences and five size differences totaling a net 26
  extra bytes. Mean similarity remains 99.77%, with three functions below 90%.
  Strict whole-file matching rises by 47 positions, from 497,570 to
  497,617 of 518,656 (95.9345% to 95.9436%). There remain 21,039 differing
  positions in 3,072 runs. Code gains 43 positions, reaching 381,546 of
  401,408; relocations gain four, reaching 35,176 of 36,352. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The uninterrupted
  matching prefix remains 544 bytes, with the same header difference at
  file offset `0x220`.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `0a4a28f511935c9976516251ff9d73d57eb95ec34c65c0a5f1a6cd8e4c7aa2dd`.
- `AllocateNearHeapBlockFromEnd` declares the descriptor address and block
  size before its two descriptor pointers, in that order. This restores
  both descriptor-bound comparisons and the comparison before the shift
  loop. The shift-loop backedge becomes reversed, while the block-size
  comparisons remain reversed. The net improvement is four literal bytes:
  439 to 443 of 448. Differing encodings fall from nine to five of 130,
  and mnemonic similarity rises from 96.15% to 97.69%. All eighteen
  relocation operands retain their original positions. This declaration
  order is a matching choice, not recovered original local spelling.
  Declaring only the block size first gains three bytes by restoring the
  block-size comparisons instead. Putting the descriptor address after that
  size declaration produces the same gain. Moving the shift address first
  gives no gain. Retain the order with the best whole-file result.
  Other bounded trials are discarded. Narrowing either the cruise range or
  objective scope gives no gain. Reversing the debug formatter's product,
  qualifying only its column read, capturing the row offset, and calculating
  that offset in two ordinary assignments all leave output unchanged.
  Qualifying both sequential offset reads loses twelve file positions.
  Explicit `TextOutA` and `ShowCursor` captures in the console's draw and
  wait routines lose 25 and 53 positions respectively. Moving the navigation
  clock's cursor declaration first gives no gain. Using the object-class
  enum for the futurion's saved class, either at function or block scope,
  gives no gain. A separate NPC candidate counter likewise gives no gain.
  Qualifying the idle-formation wingman comparison loses 377 positions.
  Moving the constellation index declaration first loses seven positions;
  a separate particle counter loses eighteen. Qualifying the guarded-heap
  peak read leaves its target unchanged and loses fourteen positions.
  Capturing the front-point fixed distance loses 94 positions. Initializing
  the approach-acceleration local in its declaration gives no gain.
  All twenty-three probes are complete. Only the end-allocation row changes
  in the full report. Direct before/after raw comparisons of all nine
  near-heap functions confirm that only its span changes. All 3,797 mapped
  symbol/address entries, representing 3,784 distinct names, retain their
  addresses. The report remains at 1,388 normalized exact and 1,459
  mnemonic-perfect functions, with 157 same-size differences and five size
  differences totaling a net 26 extra bytes. Mean similarity remains 99.77%,
  with three functions below 90%.
  Strict whole-file matching rises by four positions, from 497,617 to
  497,621 of 518,656 (95.9436% to 95.9443%). There remain 21,035 differing
  positions in 3,069 runs. Code gains all four positions, reaching 381,550
  of 401,408; relocations remain at 35,176 of 36,352. The `.rdata`, `.data`
  and `.idata` sections remain literally exact. The uninterrupted matching
  prefix remains 544 bytes, with the same header difference at file offset
  `0x220`.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `78be240599b7b64424db045467f7e503cc24b34a6ca88a06e1aaaaab5e9325fe`.
- `internal_damage` uses a signed-byte weapon count and narrows the random
  weapon index to a signed byte before passing it as a short to
  `remove_weapon`. The original loads and tests AL, sign-extends it to AX
  before decrementing the count, and sign-extends AL to AX again after the
  random call. Retain both narrowing steps. Clear the shield words in their
  observed order: maximum fore, current fore, maximum aft, current aft.
  These changes gain 231 matching file positions. Qualifying the four
  shield stores produces the same executable and is unnecessary.
  The fighter-damage loop's exit does not set the return register in the
  original. Use a bare return for the native reconstruction and retain a
  defined zero return for the modern port. This removes the extra XOR and
  restores the initial no-events branch to the shared epilogue, gaining
  twelve more file positions. Native fallthrough produces the same result.
  Moving the event, system or weapon-count declaration first adds no gain.
  The complete 896-byte span improves from 613 to 839 literal matches;
  differing encodings fall from 118 to 53 of 266, and mnemonic similarity
  rises from 98.12% to 99.25%. All 31 relocation operands now occupy their
  original positions. Register allocation and the weapon-case decrement
  scheduling still differ. The capital-ship damage pointer remains scoped
  to its branch, preserving the original relocation count.
  Other bounded trials are discarded. Four promoted-key declaration/scope
  variants leave the fleet-input target unchanged and lose four file
  positions. A byte count alone gains 22 positions; narrowing only the
  random result gains 37; combining both gains 225 before reordering the
  shield stores. Three barracks key if-chain layouts add sixteen bytes and
  lose whole-file matches. Capturing the last weapon or promoted victim
  index improves the damage target further but worsens later ship routines,
  losing 55 and 51 positions relative to the retained width/store candidate.
  A separate weapon-index local or capital-ship damage increment loses
  sixty positions relative to that candidate. Capturing the hangar escape
  byte, or its two coordinates in either declaration order, leaves that
  target unchanged and loses 39 file positions.
  All twenty-four probes are complete. Only the internal-damage row changes
  in the full report. Direct before/after raw comparisons of all 64 ship
  functions confirm that only its span changes. All 3,797 mapped
  symbol/address entries, representing 3,784 distinct names, retain their
  addresses. The report remains at 1,388 normalized exact and 1,459
  mnemonic-perfect functions, with 157 same-size differences and five size
  differences totaling a net 26 extra bytes. Mean similarity remains 99.77%,
  with three functions below 90%.
  Strict whole-file matching rises by 243 positions, from 497,621 to
  497,864 of 518,656 (95.9443% to 95.9912%). There remain 20,792 differing
  positions in 3,047 runs. Code gains 226 positions, reaching 381,776 of
  401,408; relocations gain seventeen, reaching 35,193 of 36,352. The
  `.rdata`, `.data` and `.idata` sections remain literally exact. The
  uninterrupted matching prefix remains 544 bytes, with the same header
  difference at file offset `0x220`.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `022f65eb7c55c2f27c4ba667356d1ca039d69c5c80b1e3d5bed7b51b557deb21`.
- `player_input` qualifies three stores: the initial previous-yaw snapshot,
  and the joystick X values written to the current roll and yaw words.
  Together these retain the original full-width sample-X reads before the
  two word stores, while preserving the initial input-read/store schedule.
  The two former operand-size prefixes displaced much of the function by
  one or two bytes. The complete 1,728-byte span now matches at 1,724
  positions, up from 860. Differing encodings fall from 227 to four of 397;
  mnemonic similarity remains 99.50%. All 137 relocation operands now
  occupy their original positions, versus seventy before. The only four
  differing bytes are the reversed operands and branch directions of the
  two initial mouse-threshold comparisons. These qualified accesses are
  matching source choices, not recovered original qualifier spelling.
  This also improves the preceding `players_flight_dynamics` from 231 to
  243 literal matches in its 256-byte span. Differing encodings fall from
  nine to three of 62, with mnemonic similarity unchanged at 96.77%.
  Its relocation count remains 24, with original-position offsets increasing
  from 22 to 23. The pitch-input load and multiply still exchange order with
  the shared type-offset calculation.
  Qualifying only the joystick roll store gains 129 file positions; only the
  yaw store loses 53. Combining those two without the previous-yaw store
  shortens the function by sixteen bytes. Adding all three previous-input
  stores restores its size and gains 911 positions. Only the previous-yaw
  store is needed: removing the pitch qualification recovers 35 more
  positions, and removing the roll qualification leaves output unchanged.
  Qualifying the previous pitch alone retains the size loss; qualifying the
  previous roll alone, or pitch and roll together, gains 873 positions.
  A declaration initializer for the keyboard-roll flag gains three positions
  over the all-three-previous-stores candidate, still below the retained
  result. Guarded mouse-threshold loops, with or without qualified initial
  reads, lose four positions. Qualifying both complete threshold loops
  leaves output unchanged and is unnecessary.
  Four transformed-bounds trials are discarded. Moving its height local
  before the frame pointer adds sixteen bytes. Moving the sine coefficient
  to function scope or the cosine coefficient into the frame branch loses
  25 and seventeen file positions respectively. Sequential assignments for
  the left-edge calculation leave output unchanged.
  All eighteen probes are complete. Only the two input/dynamics rows change
  in the full report. Direct before/after raw comparisons of all 53 functions
  in `main.c` confirm that only those spans change. All 3,797 mapped
  symbol/address entries, representing 3,784 distinct names, retain their
  addresses. The report remains at 1,388 normalized exact and 1,459
  mnemonic-perfect functions, with 157 same-size differences and five size
  differences totaling a net 26 extra bytes. Mean similarity remains 99.77%,
  with three functions below 90%.
  Strict whole-file matching rises by 946 positions, from 497,864 to
  498,810 of 518,656 (95.9912% to 96.1736%). There remain 19,846 differing
  positions in 2,959 runs. Code gains 876 positions, reaching 382,652 of
  401,408; relocations gain seventy, reaching 35,263 of 36,352. The
  `.rdata`, `.data` and `.idata` sections remain literally exact. The
  uninterrupted matching prefix remains 544 bytes, with the same header
  difference at file offset `0x220`.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `5c51835e28e3441090478fef0c0297a90f48b4b762d259715f19d0a6f247bcd5`.
- `draw_target_box` declares the X centre before the Y centre and scopes
  the segment length to the outline-drawing branch. Together these choices
  restore the original instruction lengths and every mnemonic. Literal
  matching rises from 147 to 980 of 1,008 bytes, differing encodings fall
  from 267 to 28 of 294, and mnemonic similarity rises from 94.22% to 100%.
  All 45 relocation operands now occupy their original positions, versus
  five before. The span ends at `remove_nav_pointer`, `0x004168A0`, before
  that helper's 32 bytes. Remaining differences are register choices and
  operand scheduling. These are matching declaration choices, not recovered
  original local spelling or scope.
  Moving only the X declaration first gains 822 file positions. Putting it
  immediately after Y leaves output unchanged. Narrowing only the segment
  length's scope gains two positions; combining it with X first gains 873.
  Qualifying the two centre-coordinate reads gives no gain.
- `scramble` qualifies its initial canopy-offset clear and overlay-X
  sentinel store. Together they restore the initial store order and gain
  24 file positions: 22 in code and two in relocations. Neither qualifier
  alone changes output. Its 1,104-byte span improves from 555 to 577 literal
  matches, differing encodings fall from 146 to 139 of 264, and mnemonic
  similarity rises from 96.21% to 96.97%. The relocation count stays at
  eighty, with original-position offsets increasing from 45 to 47. The
  qualifiers express observed accesses, not recovered source declarations.
  Loop scheduling, register allocation, and early-exit branch placement
  remain different. A direct first-loop escape to resource release loses
  eleven positions. All five other local declaration orders give no further
  gain; two lose two positions relative to the qualified candidate.
  Capturing the first loop's backdrop coordinate, alone or with the second
  loop's canopy offset, shortens the function by sixteen bytes. Capturing
  only the canopy offset improves mnemonic similarity but gives no literal
  gain and is unnecessary. All those trials are discarded. The native
  unset sound-handle read and the modern port's initialization are retained.
  The flight-dynamics trials are also discarded. Capturing the pitch input
  in either declaration position, or capturing its scaled rate, leaves the
  target's three differences unchanged and loses four file positions.
  Qualifying the pitch read makes that function exact but shortens the
  following input routine by sixteen bytes. Qualifying its previous-pitch
  or previous-roll stores restores the size but still loses 21 or six file
  positions respectively; qualifying both loses 21.
  All twenty-four probes are complete. Only the target-box and scramble
  rows change in the full report. Direct before/after raw comparisons of
  all 137 functions in their two units confirm that only those spans change.
  All 3,797 mapped symbol/address entries, representing 3,784 distinct names,
  retain their addresses. The report remains at 1,388 normalized exact
  functions, with 157 same-size differences and five size differences
  totaling a net 26 extra bytes. Mnemonic-perfect functions rise from 1,459
  to 1,460, and mean similarity rises from 99.77% to 99.78%; three functions
  remain below 90%.
  Strict whole-file matching rises by 897 positions, from 498,810 to
  499,707 of 518,656 (96.1736% to 96.3465%). There remain 18,949 differing
  positions in 2,921 runs. Code gains 855 positions, reaching 383,507 of
  401,408; relocations gain 42, reaching 35,305 of 36,352. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The uninterrupted
  matching prefix remains 544 bytes, with the same header difference at
  file offset `0x220`.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `7404a9c49b38cf44860e09a2ad57046703960d942797241aea73c06d03064e0d`.
- `scramble` captures its first-phase backdrop coordinate and second-phase
  canopy offset in one short local declared after the frame counter. Each
  source store follows the intervening ship/detail updates. This restores
  the first loop's original scheduling. Its escape goes directly to the common
  resource-release block at `0x004085B0`, preserving the original near
  branch and the complete 1,104-byte span. Literal matching rises from 577
  to 1,072 bytes, differing encodings fall from 139 to 29 of 264, and
  mnemonic similarity rises from 96.97% to 99.24%. All eighty relocation
  operands now occupy their original positions, versus 47 before.
  Remaining differences are register allocation and the second loop's
  choice of coordinate to update in a register. The shared local is a
  matching source choice, not a recovered original name or declaration.
  The native unset sound-handle read and modern-port initialization remain.
  Combining the direct escape with only the backdrop capture gains 521
  file positions. Separate backdrop and canopy locals gain 522, but also
  exchange the funeral routine's final frame counter and step registers,
  losing six previously exact bytes there. Reusing the one coordinate
  local recovers those six bytes without changing the scramble result.
  Declaring that shared local before the frame counter loses six positions.
  Moving the backdrop local alone to function scope gives no gain after
  the frame counter and loses six positions before it. Moving both separate
  locals to function scope, narrowing the funeral scene-pair scope, or
  exchanging its frame/volley declarations does not repair the regression.
  Separate later-phase counters gain only 513 or 519 positions. Qualifying
  the canopy store leaves output unchanged; qualifying its read adds an
  unwanted relocation operand and loses 16,309 file positions.
  Three `flag_reached` probes are discarded: reusing the captured carrier
  for its lookup, moving that capture before the objective-type read, and
  combining both changes lose 381, 13,970 and 380 positions respectively.
  All seventeen probes are complete. Only the scramble row changes in the
  full report. Direct before/after raw comparisons of all twelve cutscene
  functions confirm that only its span changes; `funeral_sequence` retains
  all 1,792 matching bytes. All 3,797 mapped symbol/address entries,
  representing 3,784 distinct names, retain their addresses. The report
  remains at 1,388 normalized exact and 1,460 mnemonic-perfect functions,
  with 157 same-size differences and five size differences totaling a net
  26 extra bytes. Mean similarity remains 99.78%, with three functions
  below 90%.
  Strict whole-file matching rises by 528 positions, from 499,707 to
  500,235 of 518,656 (96.3465% to 96.4483%). There remain 18,421 differing
  positions in 2,904 runs. Code gains 495 positions, reaching 384,002 of
  401,408; relocations gain 33, reaching 35,338 of 36,352. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The uninterrupted
  matching prefix remains 544 bytes, with the same header difference at
  file offset `0x220`.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `a88044eb1bfe5584f05ce9cdca92522dc44491497c2f966d2a400a686cc1ef97`.
- A strict literal audit now covers all 1,550 annotated function spans at
  their original file positions, using binary-comp for every comparison.
  Of the 1,388 normalized-exact functions, 1,373 are literally exact and
  fifteen retain operand-order or shifted-call differences. All 162 other
  report entries retain literal differences, giving 177 remaining spans.
  The source spans cover 296,316 `.text` bytes and contain all 17,406 code
  differences. Concatenating the four intervening unannotated intervals
  and comparing them with binary-comp proves their complete 105,092 bytes
  exact, including both generated initializer pairs, the import thunks,
  and CRT regions. The ten LZO/file spans absent from `src/map` are bounded
  by their original disassembly exports and also match literally.
  The whole-file remainder is therefore 17,406 code positions, 1,014
  relocation-section positions, and the one existing header position.
  Every source span has its original relocation count except `animate_shape`
  at `0x00412CD0`: sixteen original HIGHLOW operands versus fifteen rebuilt.
  The complete images contain 15,446 and 15,445 HIGHLOW entries respectively.
  The largest literal deficits are `HandleSpaceFlightControls` (2,760 bytes),
  `ShowCarrierLaunchSequence` (1,566), `ParseTests` (1,091),
  `ShowChalkBoard` (688), and `InflightComputer` (660).
  Twenty-three bounded source probes give no whole-file gain and are
  discarded. Initializing the rout/attacker counters in their declarations,
  or applying register hints, leaves output unchanged. Capturing only the
  navigation triangle's top loses 116 file positions; four explicit bounds
  lose 147. Promoting the font character index into a separate integer in
  either declaration order adds sixteen bytes. Moving its existing index
  declaration first or before its character-data pointer also adds sixteen;
  qualifying and splitting the font-pointer reload adds 32.
  Declaring the sprite-background saved pointer before its command pointer,
  moving the run length after the clipping bounds, or initializing the saved
  cursor in its declaration leaves both capture and restoration unchanged.
  Qualifying either formation-table index read gives no gain. The existing
  afterburner timer capture, under either descriptive local spelling,
  makes acceleration exact but still regresses the following animation
  routine, losing five file positions overall. Unsigned address arithmetic
  or an unsigned byte accumulator leaves the free-heap query unchanged.
  All probes restore their sources and force a native rebuild; every restore
  reproduces the preceding fully verified checkpoint. No reconstruction
  change is retained. Whole-file matching remains 500,235 of 518,656
  (96.4483%), with 18,421 differences in 2,904 runs and the first difference
  at `0x220`. The retained executable SHA-256 remains
  `a88044eb1bfe5584f05ce9cdca92522dc44491497c2f966d2a400a686cc1ef97`.
- `DrawMedals` uses a separate short counter scoped to its medal-stack
  loop. This changes eleven register-encoding bytes without changing its
  244 literal matches in the 432-byte span, its 54 differing encodings,
  or its twenty relocation operands. The compiler context also restores
  the original text-colour comparison in `SceneDirector` at `0x00438DAB`,
  recovering the byte lost during the earlier parser improvement. That
  routine now matches 1,134 of 1,136 bytes, with two differing encodings
  out of 257 and all 63 relocations still at their original positions.
  The local scope is a matching choice, not recovered original source.
  Narrowing the stack and row-coordinate scopes gives no additional gain.
  Updating the row coordinate through a chained assignment, with either
  shared or separate counters, shortens the medal routine by sixteen bytes
  and is discarded.
  The other bounded trials give no gain. Signed animation-scale captures
  lose nine file positions; division by 64 instead of shifting, or using
  the short object index for the animation tail, leaves output unchanged.
  The animation routine still has fifteen relocations versus sixteen in
  the original. Capturing the action-sphere object class as an enum or
  integer, before or after its guard, loses 117 positions. Signed or long
  palette byte counts and assigning the palette-clear return values leave
  the fade output unchanged. Capturing either width of navigation-label
  index loses 57 positions; narrowing its final reservation-pointer scope
  gives no gain. All twenty-three probes are complete.
  The full report changes only the `SceneDirector` row. Direct before/after
  comparisons of all 32 screens-unit spans confirm that only it and
  `DrawMedals` change. The complete executable changes twelve byte
  positions, with a net gain of one original match: 500,236 of 518,656
  (96.4485%), leaving 18,420 differences in 2,903 runs. Code matches
  384,003 of 401,408 bytes; relocations remain at 35,338 of 36,352.
  The `.rdata`, `.data` and `.idata` sections remain literally exact,
  and the first difference remains at file offset `0x220`.
  All 3,797 mapped symbol/address entries, representing 3,784 distinct
  names, retain their addresses. Normalized exact functions remain at
  1,388 and mnemonic-perfect functions at 1,460; mean similarity is 99.78%.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `1523315b036c936cfa7bc1411f3f4c28bfaedf67cef601081bb4050946112599`.
- `ShowCarrierLaunchSequence` scopes the actor's Y coordinate to the
  branch containing its later animation phases. Its initialization and
  uses stay in place. This changes register allocation and scheduling
  after the first phase, increasing literal matching from 498 to 806
  bytes in the unchanged 2,064-byte span. Differing encodings fall from
  423 to 349 of 520, although mnemonic similarity falls from 95.98% to
  95.79%. The literal-byte objective determines the retained choice.
  All 107 relocation operands remain, with original-position offsets
  increasing from 25 to 38. The native frame is still 28 bytes versus
  the original 32. This is a matching scope choice, not recovered source.
  Scoping only actor X gains five file positions; scoping both coordinates
  gains sixteen, in either declaration order. Scoping the fade counter
  alone leaves output unchanged, and adding it to the two-coordinate
  scope gives no further gain. Capturing the fighter type at either end
  of the declarations, or separating the first-phase scale at function
  or block scope, shortens the routine by sixteen bytes and is discarded.
  All thirty-one bounded probes are complete. The other trials give no
  gain. Promoting the tactic-reset index loses 27 file positions; also
  capturing its tactic argument loses 77 in either declaration order.
  The futurion's enum-class local, at function or branch scope, leaves
  output unchanged. Separate constellation star and particle counters
  lose eight positions; narrowing the cached star-height scope loses
  seven, or fifteen with separate counters. Capturing the constellation
  definition index loses five at either scope; reversing its addition
  leaves output unchanged. Narrowing the viewport-intersection coordinate
  scopes and capturing the gun-slot type at either scope give no gain.
  Separating the scene-command pointer increment leaves output unchanged;
  returning directly on a command match loses 81 positions. Scoping the
  flight-control weapon and shared wingman locals gives no gain; separate
  wingman locals for its two cases lose four positions.
  Only the launch row changes in the full report. Direct before/after
  comparisons of all three launch-unit spans and the complete code section
  locate all 397 changed code bytes in that one routine. Fourteen
  relocation-section positions also change. All 3,797 mapped symbol/address
  entries, representing 3,784 distinct names, retain their addresses.
  The report remains at 1,388 normalized exact and 1,460 mnemonic-perfect
  functions, with 157 same-size differences and five size differences
  totaling a net 26 extra bytes. Mean similarity remains 99.78%, with
  three functions below 90%.
  Strict whole-file matching rises by 321 positions, from 500,236 to
  500,557 of 518,656 (96.4485% to 96.5104%). There remain 18,099 differing
  positions in 2,902 runs: 17,097 in code, 1,001 in relocations, and one
  in the headers. Code matches 384,311 of 401,408 bytes; relocations match
  35,351 of 36,352. The `.rdata`, `.data` and `.idata` sections remain
  literally exact, and the first difference remains at file offset `0x220`.
  The largest remaining function gaps are flight controls (2,760 bytes),
  carrier launch (1,258), the conversation-test parser (1,091), the chalk
  board (688), and the inflight computer (660).
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `7b03409334133528d32263109ef924ee91865fe072e589a3d08c7f37ac1f379e`.
- `AnimateScrambleWalk` declares its escape snapshot inside each loop
  iteration. Initialization, refreshes and uses remain in place. Literal
  matching rises from 58 to 100 bytes in its unchanged 624-byte span;
  differing encodings fall from 147 to 134 of 159. Mnemonic similarity
  falls from 100% to 98.74%, so this is another choice driven by the
  literal-byte objective. All 57 relocation operands remain, with offsets
  also present in the original increasing from four to seven. Most of the
  draw sequence is still displaced by one byte. The loop-local signed-byte
  variant gives the same result, as does moving the initial escape read
  before the decrement. Retain the existing unsigned type and read order.
  Scoping the elapsed counter to the positive-tick branch gives a smaller
  gain with the loop-local escape flag, and regresses without it. Capturing
  the first backdrop pointer at draw or loop scope, in either declaration
  order, gives no further gain. Splitting the escape reads between the
  draw and skip branches is also worse than the retained scope.
  `ShowCarrierLaunchSequence` uses an int for the fade accumulator, retaining
  the explicit short conversion on every increment. This moves its sign
  extension from the call argument to the loop update, raising literal
  matching from 806 to 807 of 2,064 bytes. Its 349 differing encodings,
  95.79% mnemonic score, 28-byte frame and 107 relocations are unchanged.
  These are measured source choices, not recovered original declarations.
  Reusing the short object local in the later phases loses 89 file
  positions, or 77 with the promoted fade accumulator. Short fade-hook
  arguments lose 326 positions; adding the object guard loses 286, or 78
  with the promoted accumulator. Keep the existing call types. Separate
  first-phase approach counters lose 59 or 68 positions depending on scope.
  The other large-gap probes give no retained change. Main-only signed or
  plain-char key declarations leave flight controls unchanged; short or
  unsigned-short repeat flags enlarge it by sixteen bytes. Register hints
  on an explicit key difference lose four file positions. Parser case-13
  return assignments, pointer/short destination captures, explicit else
  blocks, and inverted guards leave output unchanged, including the
  combinations with its kill and objective tests. Chalkboard score captures
  lose 84 positions; a scoped short pilot index adds sixteen bytes, while
  a scoped int loses 85 positions. Narrowing its swap and inner-index scopes
  gives no gain. Inflight viewport-store ordering and qualified viewport
  stores or input-mode reads leave output unchanged. Narrowing menu polling's
  movement and double-click scopes, separately or together, also gives no gain.
  Stream-allocation comparison reversal, later or narrower voice-pointer
  declarations, and descriptive parameter/local renames give no gain.
  Reversing its voice-index addition adds one byte and displaces later
  audio code. Qualifying the stream-count read loses 1,156 file positions.
  Reversing both stop expressions or renaming its index gives no gain.
  All 61 bounded probes are terminal; discarded
  variants restore the preceding executable hash after a forced rebuild.
  Only the animation row changes in the normalized report. Raw comparisons
  of all fifteen cutscene/launch spans identify exactly the two retained
  routines as changed: 581 animation bytes and 43 launch bytes. Their sum
  accounts for all 624 changed code positions. Another 56 positions change
  in the relocation section. All 3,797 mapped symbol/address entries,
  representing 3,784 distinct names, retain their addresses. Normalized
  exact functions remain at 1,388, with 157 same-size differences and five
  size differences totaling a net 26 extra bytes. Mnemonic-perfect functions
  fall from 1,460 to 1,459; mean similarity remains 99.78%, with three below 90%.
  Strict whole-file matching gains 46 positions, from 500,557 to 500,603
  of 518,656 (96.5104% to 96.5193%). There remain 18,053 differences in
  2,900 runs: 17,054 in code, 998 in relocations, and one in the headers.
  Code matches 384,354 of 401,408 bytes; relocations match 35,354 of 36,352.
  The `.rdata`, `.data` and `.idata` sections remain literally exact, and
  the first difference remains at file offset `0x220`.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `cf86e7b8ff7bc717ad74a71a639d3c0a19f738d6947061f378f02a3a3f1ed0cd`.
- `ParseTests` keeps the medal-test destination in a case-local int,
  receiving the same signed-short result from `int_value`. This changes
  the return scheduling: case 26 becomes two bytes shorter and cancels the
  earlier displacement, bringing case 28 and the later tests back into
  alignment from `0x00438941`. Case 13 itself still differs from the
  original register sequence. This is a measured matching choice, not
  recovered original source spelling.
  Literal matching rises from 1,517 to 1,811 bytes in the unchanged
  2,608-byte parser span. Differing encodings fall from 368 to 264 of 820,
  and mnemonic similarity rises from 97.80% to 97.93%. All 76 relocation
  operands remain; the count at original offsets falls from 56 to 55.
  Moving the destination declaration to any of three function-scope
  positions, reversing the pointer addition, using an indexed address,
  or making the scene pointer const gives the same result. Keep the
  smallest scope and ordinary pointer addition. Promoting the destination
  only inside its guard gives no gain; narrowing it at the return loses
  78 file positions relative to the preceding checkpoint. Reusing the
  existing second short for the medal test also leaves output unchanged.
  Promoting the kill-test destination alone gains 134 file positions;
  combining it with the retained medal destination gains 265, less than
  the retained change's 293. Promoting the shared first argument short
  removes sixteen bytes from the parser span. Qualifying the scene pointer
  volatile removes 64 bytes, with or without the medal change. Discard
  those span-changing alternatives.
  The other large-gap trials give no retained change. Capturing the current
  chalkboard pilot record, at loop or function scope, loses 60 file
  positions and one relocation operand. Capturing both compared records
  removes sixteen code bytes and two relocation operands. Reversing the
  flight-control key comparison, expressing it as a wrapped or promoted
  difference, and expressing it as XOR give no literal gain. Moving the
  keyboard declarations among meaningful keyboard groups also gives no
  gain; two placements lose two positions. A separate unset launch sound
  handle recovers one position through its DWORD guard, but does not fix
  the 28-byte frame versus the original 32. That original stack slot has
  only a short initialization, and its interpretation as a separate
  handle remains unconfirmed; do not retain that speculative declaration.
  All thirty-four probes are complete, and discarded sources were restored
  with forced rebuilds reproducing the preceding checkpoint's hash.
  Only the parser row changes in the full report. Direct before/after
  comparisons of all 32 screens-unit spans confirm that only `ParseTests`
  changes. The complete code section changes at 346 positions, all in that
  routine, and one relocation-section position changes. All 3,797 mapped
  symbol/address entries, representing 3,784 distinct names, retain their
  addresses. The report remains at 1,388 normalized exact and 1,459
  mnemonic-perfect functions, with 157 same-size differences and five size
  differences totaling a net 26 extra bytes. Mean similarity remains
  99.78%, with three functions below 90%.
  Strict whole-file matching rises by 293 positions, from 500,603 to
  500,896 of 518,656 (96.5193% to 96.5758%). The code section gains 294
  positions, reaching 384,648 of 401,408; relocations lose one, reaching
  35,353 of 36,352. There remain 17,760 differing positions in 2,899 runs:
  16,760 in code, 999 in relocations, and one in the headers. The `.rdata`,
  `.data` and `.idata` sections remain literally exact. The uninterrupted
  matching prefix remains 544 bytes, with the first difference at file
  offset `0x220`. The largest remaining function gaps are flight controls
  (2,760 bytes), carrier launch (1,257), the conversation-test parser (797),
  the chalkboard (688), and the inflight computer (660).
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `407fa6297b4ce9e664ca0a5ff30b1f64448f71dbc6ef18ff5af252e45e3b83b2`.
- `PollMenuInputDevices` uses an int for its change accumulator. This
  reproduces the initial full-register zero and its reuse in the transition
  call and first button comparisons. The event argument still receives only
  zero or three; the subsequent button count and zero/nonzero movement
  result are unchanged. Later accumulator operations remain wider than the
  original, so this is an intermediate matching choice rather than evidence
  of the original declaration.
  Literal matching rises from 69 to 185 bytes in the unchanged 672-byte
  span. Differing encodings fall from 142 to 125 of 151, while mnemonic
  similarity falls from 97.35% to 94.12%. Retain the change for its literal
  gain. All 59 relocation operands remain, with original-position offsets
  increasing from six to 22. Byte casts on the zero tests give the same
  result. A byte cast on the event argument adds sixteen aligned bytes,
  including when combined with byte tests and updates. Using the known-zero
  byte flag in the first button comparisons leaves the old output unchanged,
  as do a register hint and initialization at the declaration.
  Explicit else blocks after either or both palette-allocation failure
  branches give no improvement. Scoping the shape encoder's bitmap and pixel
  pointers to each frame loses 174 file positions; scoping its prepared
  pointer to the final allocation loses three. Narrower extent and run-count
  scopes leave output unchanged. Capturing the frame-start offset at any of
  three declaration positions regresses the encoder and later graphics code.
  The projection clamp and switch-table work remains unretained. Placing
  the empty debris case before dust or planet and the empty asteroid case
  before the missile/ship cases, with or without an explicit default, still
  leaves a five-entry table. The routine remains sixteen bytes short, with 48
  relocation operands versus the original 49.
  Audio allocation has two opposite one-byte operand-order displacements:
  its stream-count comparison and its voice-index addition. Qualified reads
  leave those target sequences unchanged and regress neighboring stream
  controls. Reversing the addition fixes its operand order but exposes a
  one-byte excess in the allocation routine. Reversed or negated bounds
  comparisons, a negated valid-range check, and checking the upper bound
  first do not recover the missing correction. Restore all audio probes,
  including the owning DSP archive before each forced rebuild.
  All thirty-one distinct probes are complete. Discarded sources were
  restored with forced rebuilds reproducing the preceding checkpoint's hash.
  Only the menu-input row changes in the full report. Direct before/after
  comparisons of all four input-unit spans confirm that only
  `PollMenuInputDevices` changes. The complete code section changes at 477
  positions, all in that routine; 44 relocation-section positions change.
  All 3,797 mapped symbol/address entries, representing 3,784 distinct names,
  retain their addresses. The report remains at 1,388 normalized exact and
  1,459 mnemonic-perfect functions, with 157 same-size differences and five
  size differences totaling a net 26 extra bytes. Mean similarity remains
  99.78%, with three functions below 90%.
  Strict whole-file matching rises by 125 positions, from 500,896 to
  501,021 of 518,656 (96.5758% to 96.5999%). Code gains 116 positions,
  reaching 384,764 of 401,408; relocations gain nine, reaching 35,362 of
  36,352. There remain 17,635 differing positions in 2,899 runs: 16,644 in
  code, 990 in relocations, and one in the headers. The `.rdata`, `.data`
  and `.idata` sections remain literally exact, and the first difference
  remains at file offset `0x220` after a 544-byte matching prefix. The
  menu-input gap falls from 603 bytes to 487; the three largest gaps remain
  flight controls (2,760), carrier launch (1,257), and the parser (797).
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `a34899861a7e8c0c95ee8b675e6e837f8bb06835823f832b0e59efb86d1559da`.
- `StepPaletteTransition` qualifies the initialization-flag and initial
  component-count reads, and captures the accumulator element's address
  after its first update. The four register saves and flag comparison now
  occupy their original positions. The final loop also recovers the
  original accumulator-address and value registers. The count still loads
  before the conditional jump, and the separate else-path reload remains
  missing; this is an intermediate match. Qualifying that else-path read
  as well gives identical output and is unnecessary.
  Literal matching rises from 347 to 358 bytes in the unchanged 560-byte
  span. Differing encodings fall from 63 to 59 of 154 at 97.40% mnemonic
  similarity. All 35 relocation operands remain, with original-position
  offsets increasing from 22 to 23. The read qualifiers alone recover eleven
  file positions, the function-scope pointer alone one, and their combination
  twelve. A loop-scope pointer gives no gain. Qualifying only the else-path
  count read loses 247 positions; a separate initialization-count local at
  either function or branch scope gives no gain.
  `InflightComputer` keeps its saved input mode in a volatile short. This
  restores the original 72-byte frame from 68 bytes and the original
  snapshot, viewport, and event-buffer offsets. Its saved navigation point
  and objective flag remain two bytes above their original slots, and the
  viewport-pointer store still creates a one-byte displacement in later
  code. Literal matching rises from 380 to 386 of 1,040 bytes, with differing
  encodings falling from 207 to 203 of 278. The mnemonic sequence remains
  exact and all 52 relocation operands remain. Moving this declaration to
  four other positions or its branch scope, or making it unsigned, gives
  the same output. Qualifying the saved navigation point regresses the
  routine; qualifying the objective flag adds sixteen aligned bytes.
  The larger-gap experiments remain unretained. Reversing the flight-control
  repeat flag gains 130 file positions but changes its INC to NEG and
  reverses 21 conditional jumps, contrary to the original branch sequence.
  Capturing either or both input keys instead loses four positions. A
  creation-only navigation-object index gains four file positions but
  leaves the missing saved register and worsens the creation displacement;
  restore it. A captured shape-slot pointer adds sixteen bytes and another
  relocation. Explicit allocation-success else blocks and a separate
  navigation-frame local give no gain. Narrowing joystick-transition local
  scopes and all five alternative answer-parser pointer declaration orders
  also give no gain. Capturing the shard velocity pointer loses fourteen
  positions, and qualified flight-profile zero stores lose 494.
  All forty-four native probes are complete. One additional audio probe is
  rejected by VC++ 4.20: a const-qualified definition parameter conflicts
  with its existing unqualified prototype under C2579. No shared prototype
  change is retained, and the two related prepared probes are not run.
  Discarded sources and the DSP archive were restored with forced rebuilds
  reproducing the preceding checkpoint's hash. These qualifiers and local
  choices reproduce observed code generation; they do not establish the
  original source declarations.
  Only the palette and inflight-computer rows change in the full report.
  Direct before/after comparisons of all 36 fade/nav function spans confirm
  that only those two routines change. Their 76 and 19 changed positions
  account for all 95 changed code bytes; four relocation-section positions
  change. All 3,797 mapped symbol/address entries, representing 3,784
  distinct names, retain their addresses. The report remains at 1,388
  normalized exact and 1,459 mnemonic-perfect functions, with 157 same-size
  differences and five size differences totaling a net 26 extra bytes.
  Mean similarity remains 99.78%, with three functions below 90%.
  Strict whole-file matching rises by eighteen positions, from 501,021 to
  501,039 of 518,656 (96.5999% to 96.6033%). Code gains seventeen positions,
  reaching 384,781 of 401,408; relocations gain one, reaching 35,363 of
  36,352. There remain 17,617 differing positions in 2,892 runs: 16,627 in
  code, 989 in relocations, and one in the headers. The `.rdata`, `.data`
  and `.idata` sections remain literally exact. The uninterrupted matching
  prefix remains 544 bytes, with the first difference at file offset
  `0x220`. The largest gaps remain flight controls (2,760 bytes), carrier
  launch (1,257), and the conversation-test parser (797).
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `2cbca12678b63be36063b1d8be7cb5557988d0f965fd3a61f01f0226f537daf3`.
- `GetTransformedShapeBounds` declares its absolute-sine local after the
  span and transformed-dimension locals. This changes CL 4.20's multiply
  operand choices and register reuse, removing the two-byte displacement
  at the failure return. The 432-byte span, eight-byte frame, two original
  relocation operands, and all following function addresses are preserved.
  Literal matching rises from 201 to 262 bytes. Differing encodings fall
  from 101 to 76 of 149, while mnemonic similarity falls from 95.30% to
  94.63%; retain this intermediate source order for the literal gain.
  Placing the sine declaration after the left-coordinate local gives the
  same executable. Placing it after the height local gives the same matching
  count with different multiply encodings. Moving the cosine declaration
  after the frame-offset local, or exchanging the right/top extent
  declarations, gives no further gain. Moving cosine after the intersection
  result or into the frame branch regresses the function. Five dimension
  declaration orders alone range from four fewer file matches to three
  more; none approaches the retained 61-position gain.
  The larger-gap trials remain unretained. Chalkboard order-slot pointers,
  at function or loop scope and with either one or both slots captured,
  lose 101 file positions. Moving and qualifying its viewport store recovers
  the original store position relative to font initialization but loses one
  file position; the earlier sort displacement remains. A long score
  multiplier, reversed score comparison, and a comma-sequenced swap capture
  leave the executable unchanged. An earlier inline assignment in one
  subtraction operand was unsequenced relative to a read in the other;
  discard that candidate rather than relying on its compiler output.
  Scoping the palette byte count to the allocation sequence, with or
  without const qualification, gives no literal gain. Capturing an action-
  sphere mission index loses 117 positions. Capturing the lower-right
  joystick device index gives no gain; a sample pointer adds sixteen bytes
  and four relocations. Reversing navigation-label stores adds sixteen
  bytes. Inflight viewport-pointer captures at three declaration scopes
  give no gain. Reversing or qualifying the audio-position bounds check,
  moving its voice-pointer declaration before the guard, or narrowing that
  pointer's scope also leaves output unchanged. The one-byte audio
  discrepancy remains; restore its owning DSP archive on each rebuild.
  Thirty-five valid native probes are complete, plus the discarded
  unsequenced candidate. All discarded sources were restored with forced
  rebuilds reproducing the preceding checkpoint's hash. The retained local
  order reproduces observed code generation; it does not establish the
  original declaration order.
  Only the shape-bounds row changes in the full report. Direct before/after
  comparisons of all 60 graphics-unit spans confirm that only this routine
  changes; its 193 changed positions account for all changed code bytes.
  Relocations and every other section remain unchanged from the preceding
  checkpoint. All 3,797 mapped symbol/address entries, representing 3,784
  distinct names, retain their addresses. The report remains at 1,388
  normalized exact and 1,459 mnemonic-perfect functions, with 157 same-size
  differences and five size differences totaling a net 26 extra bytes.
  The rounded mean similarity is 99.77%, with three functions below 90%.
  Strict whole-file matching rises by 61 positions, from 501,039 to 501,100
  of 518,656 (96.6033% to 96.6151%). All gains are in code, reaching 384,842
  of 401,408 bytes. Relocations remain at 35,363 of 36,352. There remain
  17,556 differing positions in 2,893 runs: 16,566 in code, 989 in relocations,
  and one in the headers. The `.rdata`, `.data` and `.idata` sections remain
  literally exact. The uninterrupted matching prefix remains 544 bytes,
  with the first difference at file offset `0x220`. The largest gaps remain
  flight controls (2,760 bytes), carrier launch (1,257), and the conversation-
  test parser (797); the shape-bounds gap falls from 231 bytes to 170.
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `e4a96386ec5f011f12b5fdfc92c8db01fb4e0afbaa5ccf492d62d6264874f2cf`.
- `ParseTests` shares an int destination between the medal test in case 13
  and the late-ejection test in case 26. Both still receive the signed-short
  result of `int_value`. This changes return-value scheduling and recovers
  eighty literal code bytes in the unchanged 2,608-byte span, from 1,811
  to 1,891 matches. All 76 relocated operands remain; operands at original
  offsets increase from 55 to 71, recovering sixteen relocation-section
  positions. The four-byte local frame remains unchanged. Differing
  encodings fall from 264 to 253 of 820, while mnemonic similarity falls
  from 97.93% to 97.20%. This is a measured source choice for literal
  matching, not a claim that the original destination declaration is known.
  A separate case-26 int gives the same parser result but changes the
  previously exact `ShowVictoryScreen`, losing eleven file positions.
  Sharing the existing destination avoids that regression. Declaring the
  shared destination first, last, or around the second switch gives the
  same literal gain; keep it last at function scope to minimize the edit.
  Extending it to case 6, 7, or 14 gives a worse result. Capturing the
  kill-test destination only inside its guard gives no additional gain.
- `CaptureSpriteBackground` and `RestoreSpriteBackground` declare their
  existing run-end coordinate before the count words. Their calculations
  and memory accesses are unchanged. Capture gains eleven literal bytes,
  from 487 to 498 of 752, with differing encodings falling from 104 to 101
  of 220 and mnemonic similarity rising from 91.36% to 92.27%. Restoration
  gains three bytes, from 508 to 511 of 768; its differing encodings fall
  from 108 to 106 of 224, while mnemonic similarity falls from 93.30% to
  92.41%. Both retain their forty-byte local frames. Capture has no
  relocations; restoration retains its one operand at the original offset.
  Moving only the capture skip local gives a smaller gain, and moving its
  run end after the pixel pointer gains ten bytes. Combining run-end and
  skip declaration moves gives no further gain, or regresses restoration.
  The run-length spill and remaining clipping-register differences persist.
- Other bounded trials are discarded. Weapon-position pointer captures,
  qualified cockpit message/frame reads, font destination-pointer moves,
  reversed font pointer sums, and first-rectangle coordinate captures give
  no net improvement; several disturb aligned spans or later code.
  Qualifying the loop-local scramble escape snapshot leaves that routine's
  comparison unchanged but disturbs later layout. Capturing its decrement
  in a loop-local short loses 49 file positions. Six further RLE local
  declaration orders give no gain. Promoting parser destinations in cases
  7, 8, 14, 15, or 18 regresses the file, as does a guarded case-26 capture
  or an additional promoted kill destination. Captured camera comparison
  pointers also regress matching. All fifty native probes are complete,
  with discarded sources force-restored and checked against the baseline
  executable hash before applying the selected changes.
  Only the three selected rows change in the full report. Direct raw
  comparisons of all 92 graphics/screens function spans locate all 1,363
  changed code positions in those three routines; 21 relocation-section
  positions change. All 3,797 mapped symbol/address entries, representing
  3,784 distinct names, retain their addresses. The report remains at
  1,388 normalized exact and 1,459 mnemonic-perfect functions, with 157
  same-size differences and five size differences totaling a net 26 extra
  bytes. Mean similarity remains 99.77%, with three functions below 90%.
  Strict whole-file matching rises by 110 positions, from 501,100 to
  501,210 of 518,656 (96.6151% to 96.6363%). There remain 17,446 differing
  positions in 2,882 runs: 16,472 in code, 973 in relocations, and one in
  the headers. Code matches 384,936 of 401,408 bytes; relocations match
  35,379 of 36,352. The `.rdata`, `.data` and `.idata` sections remain
  literally exact, and the first difference remains at file offset `0x220`.
  The largest remaining function gaps are flight controls (2,760 bytes),
  carrier launch (1,257), the test parser (717), the chalk board (688),
  and the inflight computer (654).
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `c509175d23e1f1c9250bd650fe1b2e3124e460e9bc9bede3f00c393f5b41722e`.
- `LoadJoystickCalibrationFile` declares the vertical centre before the
  file-buffer extrema, and the horizontal centre and vertical range last.
  The calculations are unchanged. The failed-read flag now uses EBP as in
  the original, the vertical centre uses ESI, and the final bound sums
  recover their original instruction lengths and operand order. The
  unchanged 784-byte span gains 97 literal code positions, from 649 to 746.
  All 34 relocation operands now occupy their original offsets, up from
  29, recovering five relocation-section positions. Differing encodings
  fall from 83 to 25 of 243; mnemonic similarity remains 98.77%, and the
  forty-byte local frame is unchanged. The remaining centre/range register
  swap and arithmetic scheduling differences persist. Moving the vertical
  centre alone gives only seven file positions; moving the horizontal
  centre last gives 41. Moving the vertical range last as well gives 102.
  Other tested declaration orders give at most 101, and moving centres or
  ranges into the active-device block regresses matching or aligned size.
- `IsPushedPacketHandle` stores the found entry's pushed status in an int
  result before returning it. This recovers the original ESI index and
  the move-one/conditional-return shape. The count still uses ECX instead
  of EAX, leaving its initial load one byte longer than the original.
  Literal matching improves from five to seven of 64 bytes, with differing
  encodings falling from 22 to 21 of 22; mnemonic similarity stays 90.91%.
  Its three relocations remain, with none at the original offsets. Explicit
  count captures and their declaration orders add no gain. Explicit entry
  pointers add a relocation and regress matching. Reversing direct-return
  branches or changing the search-loop shape gives worse assembly despite
  small incidental literal gains, so those forms are discarded.
- `CaptureSpriteBackground` and `RestoreSpriteBackground` declare the
  existing copy length before the run-end coordinate. Calculations and
  memory accesses are unchanged. Capture gains seven literal positions,
  from 498 to 505 of 752; differing encodings fall from 101 to 94 of 220,
  while mnemonic similarity falls from 92.27% to 91.82%. Restoration gains
  five positions, from 511 to 516 of 768, and differing encodings fall from
  106 to 101 of 224 at unchanged 92.41% similarity. Both keep their forty-
  byte local frames and unchanged zero/one relocated operands. Declaring
  the copy length after the run end or run length gives no additional gain.
  These are measured source choices for literal matching, not recovered
  original declaration text; the run-length spill differences remain.
- Other bounded trials are discarded. Moving the camera's case-four
  vectors into their only use block leaves its output unchanged. Byte-sized
  menu change flags passed as the initial zero argument retain the shorter
  initial clear and lose 125 file positions. Additional short parser
  destination captures leave the target unchanged while disturbing a later
  exact function; a long destination changes nothing. Consistent scalar
  const parameters across the DSP definitions and their declarations compile
  successfully but leave all tested audio bytes unchanged. This differs
  from the earlier definition-only const trial, which failed to compile;
  neither form gives a retained change. Launch shape-pointer arrays were
  revisited with the current actor-coordinate scope and promoted fade level:
  grouping all three loses 307 file positions, and grouping the actor and
  fighter loses three. All 51 native probes are complete, with discarded
  sources force-restored and checked against the baseline executable hash.
  Only the four selected rows change in the full report. Direct raw
  comparisons of all 83 graphics/calibration/screen function spans locate
  all 179 changed code positions in those four routines; six relocation-
  section positions change. All 3,797 mapped symbol/address entries,
  representing 3,784 distinct names, retain their addresses. The report
  remains at 1,388 normalized exact and 1,459 mnemonic-perfect functions,
  with 157 same-size differences and five size differences totaling a net
  26 extra bytes. Mean similarity remains 99.77%, with three functions
  below 90%.
  Strict whole-file matching rises by 116 positions, from 501,210 to
  501,326 of 518,656 (96.6363% to 96.6587%). There remain 17,330 differing
  positions in 2,824 runs: 16,361 in code, 968 in relocations, and one in
  the headers. Code matches 385,047 of 401,408 bytes; relocations match
  35,384 of 36,352. The `.rdata`, `.data` and `.idata` sections remain
  literally exact, and the first difference remains at file offset `0x220`.
  The largest remaining function gaps are flight controls (2,760 bytes),
  carrier launch (1,257), the test parser (717), the chalk board (688),
  and the inflight computer (654).
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `69ca6bb8d970757162ee0710dcf6357b90f400c2837f84c750d74e88076e1c9b`.
- `InflightComputer` assigns the cursor viewport before reading the saved
  input mode. With the retained volatile short, this makes CL use EAX for
  the viewport address and CX for the saved mode, as the original does.
  The five-byte absolute pointer store removes the one-byte displacement
  through the remainder of the function. Moving the saved navigation-point
  declaration after the event-type local also restores the original
  displayed-objective comparison order. Literal matching rises from 386
  to 1,035 of 1,040 bytes; differing encodings fall from 203 to five of 278,
  with the mnemonic sequence still exact and the 72-byte frame unchanged.
  All 52 relocation operands now occupy their original offsets, up from
  twenty. The source-order change alone recovers 648 code and 32 relocation
  positions; the declaration move adds one code position, for 681 total.
  The five residual bytes are stack offsets: the saved navigation point and
  objective flag remain two bytes above their original slots. A separate
  captured input-mode short or byte gives the same first gain, so keep the
  smaller source-order change. Moving the pointer store after the mode flag
  gives no gain. Moving the objective flag declaration adds nothing, and
  moving the displayed-objective declaration regresses the result.
  An ordinary saved-mode short restores the wrong 68-byte frame. Qualifying
  only its store or its final byte/word read does not retain the 72-byte
  frame. A typed cursor-state snapshot copied with memcpy leaves the output
  unchanged; structure assignments regress matching. These forms are not
  retained.
- `ReadCalibratedJoystick` captures each division's axis scale in one shared
  int local. Its complete 384-byte span remains unchanged, including its
  thirty relocation operands and unresolved memory-form divisors. This
  measured declaration context makes the following `UnionRectBounds`
  match all 96 bytes, recovering six positions and its four differing
  encodings. The original loads the first rectangle pointer before the
  second and uses the first left edge as the initial minimum operand.
  Capturing any one scale gives the same unit result, as does qualifying
  all four reads; keep the ordinary shared scale. Directly reversing the
  rectangle's minimum expression or capturing either left edge does not
  improve its output. An explicit local/if minimum loses eleven positions.
- `AllocateViewport` compares its unsigned-short row and height after int
  promotion. The range still fits signed int, and this restores the
  original signed JL in place of JB. It gains one literal position, from
  184 to 185 of 320, and raises mnemonic similarity from 87.13% to 88.12%.
  Its 55 differing encodings, eight-byte frame and seven relocations remain.
  Reloading the viewport's row-table field, using a separate row-count
  bound, and moving the offset initialization earlier leave the aligned
  size intact but regress the bytes. Six further height, row-table and
  offset declaration placements give no additional gain. The original
  initial loop guard and register allocation remain unresolved.
- Other bounded trials are discarded. Capturing the chalkboard's current
  pilot before the swap adds sixteen aligned bytes; qualified captures in
  the existing short leave output unchanged, while a scoped int loses
  eighty positions. Extending a promoted navigation-object index through
  the initial screen-coordinate stores adds sixteen bytes. Interactive
  calibration range/scale declaration moves regress the file. Advancing
  the RLE frame-table pointer before its store loses 181 positions, and a
  loop-local frame-offset capture disturbs later output. All 46 native
  probes are complete; discarded sources were force-restored and checked
  against the baseline executable hash before applying the selected forms.
  Only three rows change in the full report. Direct raw comparisons of all
  54 navigation/calibration/viewport function spans locate all 657 changed
  code positions in the inflight computer, rectangle union and viewport
  allocator; 32 relocation-section positions change. The calibrated
  joystick reader's bytes are unchanged. All 3,797 mapped symbol/address
  entries, representing 3,784 distinct names, retain their addresses.
  Normalized exact functions rise from 1,388 to 1,389, with 1,459 mnemonic-
  perfect functions, 156 same-size differences and five size differences
  totaling a net 26 extra bytes. Mean similarity remains 99.77%, with three
  functions below 90%.
  Strict whole-file matching rises by 688 positions, from 501,326 to
  502,014 of 518,656 (96.6587% to 96.7913%). There remain 16,642 differing
  positions in 2,747 runs: 15,705 in code, 936 in relocations and one in
  the headers. Code matches 385,703 of 401,408 bytes; relocations match
  35,416 of 36,352. The `.rdata`, `.data` and `.idata` sections remain
  literally exact, and the first difference remains at file offset `0x220`.
  The largest remaining function gaps are flight controls (2,760 bytes),
  carrier launch (1,257), the test parser (717), the chalk board (688),
  and the scramble walk (524).
  Clean native and modern builds, source/address audits, the order report,
  and full verification pass. Both value checks cover all 1,550 functions
  with zero mismatches or skips; existing call/global-access diagnostics
  remain 11/55. The clean-build and post-verification hashes agree:
  `084f3ae163a3f747a77fc7293b5dcb5ef6bdc9678a39224ba29396ee2e32b4cb`.
- The next thirteen bounded probes produce no retained reconstruction
  change. Moving all twelve flight-profiler statics to their sole consumer,
  `RunSpaceFlight`, loses 180 matching file positions; moving only the six
  timestamps or six deltas loses 175. The flight-controls comparison stays
  at 768 differing encodings out of 789, with its 3,184-byte span and 174
  relocation operands unchanged.
  In `make_shard`, a forward-vector pointer at function or inner-block
  scope loses fourteen file positions. Splitting the speed calculation or
  expressing either vector copy with memcpy leaves output unchanged.
  Qualifying the medal row read loses 97 positions; a separate medal X
  coordinate loses eleven, and combining those changes loses 108.
  Capturing either release-weapon display coordinate separately loses
  nine positions. Every probe completed and restored the source with a
  forced native rebuild; the final executable reproduces the preceding
  verified checkpoint's SHA-256. Work stopped at the user's request before
  any further source trials.
  Whole-file matching remains 502,014 of 518,656 bytes (96.7913%), with
  16,642 differing positions in 2,747 runs. The first difference remains
  at file offset `0x220`; this percentage measures matching positions
  throughout the file, not a matching prefix. No source changes are
  retained, so the preceding native/modern builds and verification results
  still describe the executable. This checkpoint records the discarded
  probes only.

## Include placement and leaked-source symbol counts

The data layout is complete, so the remaining tie-break work is the
declaration-count pass. Two levers realize a unit's symbol offset with
ordinary source rather than probe or dummy declarations.

- `#include` placement. A standard header that `wc1.h` does not include
  adds its symbols wherever it appears. After the unit's includes it shifts
  only the unit's own symbols; before them it also shifts every header
  global. A header that `wc1.h` already includes, placed first, only moves
  its symbols ahead of `<windows.h>` and leaves the total unchanged. Costs
  were measured by matching each compiled exact set against a 544-offset
  `logic.c` sweep. Any new header file costs at least one: `<limits.h>`,
  `<memory.h>`, `<share.h>`, `<dos.h>` and `<windowsx.h>` 1, `<assert.h>` 2,
  `<signal.h>`, `<setjmp.h>`, `<search.h>` and `<locale.h>` 5, `<sys/types.h>`
  7, `<malloc.h>` 14, `<float.h>` 16, `<sys/stat.h>` 19, `<crtdbg.h>` 35,
  `<process.h>` 62. `<stddef.h>` is already reached through other headers.
  Guard headers the SDL port lacks with `#ifndef SDL_PORT`.
  `ix/sound.cpp` and `ix/sample.cpp`, both from `ix\src\sound`, include
  `<dos.h>`. This makes `IxSound::ix_sound_reprioritise` and
  `IxSample::ix_sample_convert_from_ieee_extended` exact; it also removes the
  former's one-byte shortfall and `ix_sound_is_playing`'s one-byte excess.
  `system.cpp`, from the same tree, loses `ix_system_find_free_voice` with
  the include and does not take it.
- Leaked-source shape. `..\BRAINS.C`, the surviving 1990 WINGLEADER module,
  keeps locals and branch nesting that cost symbols without emitting code.
  Restored in `brains.c`: `imperial_formation`'s unused `target`, `dest` and
  `closest` and its nested special-maneuver tests; `orbit_sphere`'s nested
  range tests (the leaked `mega_ship`); `ship_intelligence`'s unused
  `target`; `capital_ship_intelligence`'s inner tactic `switch` without a
  type local; and `heat_seeking_missile_intelligence`'s leaked declaration
  order `exhaustHeat`, `targetShip`, `targ`. Every function from
  `cruise_home` to `defend_mission` then needs one common offset, eight
  symbols fewer than the shared headers give it. The NPC module therefore
  does not see the five streamer trigger and volume prototypes
  (`WC1_BRAINS_SOURCE` in `wc1funcs.h`). 45 of 46 routines are exact;
  `FF_missile_intelligence` now needs a different offset from the
  heat-seeker before it. Its tie is local (`obj` against its own promoted
  index) and the leaked text of both routines matches ours, so the
  difference is not yet explained.
  A shared loop variable in the heat-seeker also matches its registers,
  but the leaked declarations keep three locals, so it is not retained.
- `approach_speed` declares `delta` before `acceleration`. The old order is
  never exact in 0..543; the new one is exact at offsets 3..30 of 32, so it
  waits for `ship.c`'s offset to be realized.

Planning tools (scratchpad): a separable fit of each function's 544-offset
sweep (exact iff R[s mod 32] and C[s mod 257]) extrapolates windows to any
shift, and a DP over the 8,224 residues gives the fewest offset changes per
unit. In `brains.c` these changes cut the boundaries from two to one.
The byte report improves from 1,389 to 1,394 exact functions and from five
size differences to three; `.text` matches 96.2%.
- Single-offset units realized with includes after `wc1.h` (as the leaked
  main module follows `<game.h>` with `<dos.h>`): `smart.c` takes `<dos.h>`
  and `<process.h>` (+63), `targ.c` and `cutscene.c` take `<process.h>`
  (+62), the near-heap and memory-stack module `screen.c` takes
  `<malloc.h>` (+14), and `sound.c` also takes `<malloc.h>`. `ix/thread.cpp`
  takes `<process.h>`, `<crtdbg.h>` and `<fcntl.h>`. `winmain.c` includes
  `<stdio.h>` ahead of `wc1.h`, which moves only header symbols and fixes
  `ShutdownGameWindow`'s import-pointer registers. It then takes `<crtdbg.h>`
  and `<assert.h>` (+37). Every exact routine in those units stays exact.
  `cutscene.c`'s `DrawScrambleActor` declares `frame`, `control`, then
  `animationFrame`; no order with `animationFrame` first is exact.
  `smart.c`, `targ.c` and `ix/thread.cpp` are now fully exact. The byte
  report rises from 1,394 to 1,404 exact functions; never-exact
  `PlayScrambleHangarScene` shows more differing bytes at the new
  `cutscene.c` offset.
- `ending.c` defines the victory-text, escape-text, projectile-origin and
  end-caption statics immediately before `ShowCampaignVictorySequence`,
  their first user, keeping their declaration order and the literal order
  ahead of the scene format strings; `.data` is unchanged. The move takes
  their twelve symbols (statics and literals) away from the allocation
  helpers. With `slot` declared first in the victory sequence and
  `<sys/types.h>` (+7) included, both the allocation helpers and the
  victory sequence are exact; only `ShowTigerClawEscapeScene`, a register
  permutation, remains.
- `screens.c` declares `Dismissed`'s `leftX` after `podiumFrame` and
  `SceneDirector`'s `shot` before `previousShot`; both routines are then
  exact at every probe offset. Moving `nCommSpeakerObject`'s `extern` seven
  declarations later in `globals.h` fixes `npc_communication` with no loss
  in any unit. `nav.c` includes `<dos.h>` and `<float.h>`, `ship.c`
  `<sys/stat.h>` and `<setjmp.h>`, and `logic.c` `<crtdbg.h>`. These gain
  twelve routines and lose four (`find_child_object`, `replenish_shields`,
  `your_afterburner`, `evaluate_damage`), which need a different offset
  within their units. In `cockpt.c`, `<dos.h>` with `<float.h>` gains three
  more routines but shortens `vdu_polygon` by sixteen bytes and shifts every
  later function; it is not retained. Check section sizes, not only exact
  counts, after any include change.
- With every other order fixed, `SaveGame` declares its loop `index` first
  and `AllocateNearHeapBlockByFlags` declares its block pointer after the
  three addresses; both are then exact at the current offset. A search of
  every local order for all 55 offset-only routines found only these two,
  and `unitperm` sweeps of `logic.c` reach at most one more routine at any
  uniform shift, so most offset-only routines need in-unit count changes.
  `eventmgr.c` includes `<malloc.h>` and `<sys/types.h>`, `main.c`
  `<stdio.h>` ahead of `wc1.h`, and `spc.c` `<float.h>`; this gains
  `GetNextInputEvent`, `ReleaseInputEvent`, `GetViewportIntersection`,
  `RunSpaceFlight` and `house_keep_objects` and loses `sort_object_depth`.
  The WC2-shaped `ValidateViewportBounds` (empty-bodied `for` search,
  block-scoped `allocation`, `topOffset`/`rowStrideOffset`/`nextOffset`)
  matches everything except one `stScreenView` operand-order tie, which no
  probe offset in 0..63 or single `extern` move fixes; it is not yet
  retained.
- A uniform shift and local orders combine: choose the unit offset where
  the most routines are exact, then reorder the remaining routines' locals
  at that offset. `nearheap.c` includes `<process.h>` ahead of `wc1.h` and
  declares `GetNearHeapBlockSize`'s locals as `block`, `descriptorAddress`,
  `size`, `address`; all nine heap routines are exact. `video.c` declares
  `entry` before `entryCount`, puts `index` first in the allocation map,
  and includes `<dos.h>` and `<sys/types.h>` (+8); all four routines are
  exact. `MainWindowProc` declares `primaryButton` before
  `secondaryButton` and is exact; `debug.cpp` includes `<signal.h>`
  (gaining the worker and printf routines, losing `Clear`). Check every
  include for new size mismatches: `<direct.h>` in `ix/dsps.cpp` fixed
  `ix_dsps_stop` but changed `ix_dsps_alloc`'s size and is not retained.
- Hiding a unit's own prototype makes its definition cost one more symbol,
  a +1 step at that function. With each routine's exact offsets fitted from
  a 544-offset sweep, a bitmask DP over the unit (scratchpad
  `protoplan2.py`) finds the smallest set of hidden own prototypes and the
  uniform include shift that make every offset-only routine exact.
  `brains.c` hides `formation_break`, `imperial_wingman`,
  `kilrathi_wingman`, `wingman_mission` and
  `heat_seeking_missile_intelligence` and includes `<dos.h>`,
  `<sys/types.h>` and `<assert.h>` (+10); all 46 routines are exact.
  `view.c` hides `DrawConstellationField` and `CloseDataFileByHandle`
  (`InitializeConstellationField` is exact). `barracks.c` needs only a
  uniform +84: `<dos.h>`, `<process.h>`, `<sys/stat.h>` and `<assert.h>`
  make both offset-only routines exact. `debug.cpp` (C++) does not follow
  the model's prediction for a hidden worker prototype; the remaining
  units' plans need negative steps, which hiding cannot produce.
- `onboard_explosion` copies `aShipVelocity[obj]` directly into the debris
  velocity after changing its scale, then sets the debris counter to six.
  Use the position arrays directly in `AddFixedVectors` too. With this
  statement order, CL spills the common source-vector offset itself and
  saves EBP for the three-dword copy, recovering the original 16-byte frame.
  The explicit `objectOffset` local and byte-pointer arithmetic prevented
  that allocation. Moving the counter alone while retaining a promoted
  source index still leaves nine differing encodings; putting the counter
  before the direct copy leaves fifty. The successful form matches all
  208 bytes at the original address, including alignment, up from 60.
  The unoptimized WC2 counterpart corroborates copy-before-counter order;
  WC1's original assembly and literal binary-comp comparison establish the
  retained result.
  Removing the local shifts later declaration indices by one. `ship.c`
  omits `call_enemy`'s own prototype, whose definition precedes its only
  caller, and includes `<assert.h>`. The two header symbols replace the
  omitted prototype and parameter; the definition restores the later
  one-symbol step. These are measured declaration contexts, not recovered
  original header text. Without them, `analyze_kill` loses its exact match
  and `replenish_shields` gains one differing encoding. With them, all
  401,200 code bytes outside the explosion span are unchanged.
  The full report gains one exact function (1,431 to 1,432); only this
  function's row changes, and `AllocateViewport` is the sole routine below
  90% mnemonic similarity. Whole-file matching gains 155 positions: 148
  code bytes and seven relocation bytes, reaching 502,486 of 518,656.
  The remaining 16,170 differences still prevent a whole-executable match.
  Full verification, the modern build, sorting and address audits pass.
  Both value checks cover all 1,550 functions with zero mismatches or skips;
  existing call/global-access diagnostics remain 11/55. The rebuilt image
  retains SHA-256
  `6b6bae80050a8ac7cc76d02ce31638ef537a67618d95f63433ac7fd709bad2a3`
  through verification.
- Further voice-setter, flight-key and viewport trials at this checkpoint
  are discarded. Standard-header shifts, removing the volume setter's
  qualification, and equivalent negated bounds guards do not improve the
  voice unit. Captured key bytes, qualified key reads, signed-key declarations,
  and moving the previous-key declaration by up to sixteen declarations
  leave flight controls' initial AL allocation unchanged. A guarded viewport
  loop with direct row-table field access spills height instead of top and
  adds sixteen bytes. All 720 orders of its six locals and all combinations
  of register hints retain that error; explicit promoted height/top locals
  can recover the span but worsen its instructions. No probe declarations,
  comparison helpers, or discarded source forms are retained.
- `LoadPacketAllocated` becomes exact when `nPacketError` is declared before
  `bKeyEventQueueEnabled` in `globals.h`. Moving the existing declaration
  twelve places earlier changes its common-subexpression temporary's symbol
  index. The retry loop then keeps the file table in ECX and the error word
  in AX, as the original does. This restores the one-byte-longer table load
  and all later instruction positions, eliminating 32 differing encodings.
  All 144 bytes now match literally, up from 71. The function body and its
  types are unchanged; this is measured declaration context, not recovered
  original header text.
  Only the packet loader's row changes in the full report, raising normalized
  exact functions from 1,432 to 1,433 of 1,550. All 401,264 code bytes outside
  its span are unchanged. The three changed relocation bytes also match,
  giving a whole-file gain of 76 positions: 502,562 of 518,656 (96.8970%),
  with 16,094 differences remaining. No function span or symbol address moves.
  Earlier packet trials with reordered or scoped locals, an inline packet
  size, implicit table/error captures, and equivalent retry guards do not
  finish the match. Moving the key declarations within the tested header
  neighborhood leaves flight controls' initial AL allocation unchanged.
  Chalkboard swap scopes, score-index captures, and viewport-store placement
  give no retained improvement. Viewport offset initialization placement and
  signedness likewise fail to recover its original height/top allocation.
  Full verification, the modern build, sorting and address audits pass.
  Both value checks cover all 1,550 functions with zero mismatches or skips;
  existing call/global-access diagnostics remain 11/55. The image retains
  SHA-256 `104bd745ec3000913c7cb57d72785393889e6ae576bf948489e51154e676e997`
  through verification.
- `draw_nav_pointer` initializes a newly allocated object's class before
  its frame count, angle, scale and shape, and assigns its owner last.
  Keeping this source order lets CL retain the promoted object index in
  EDI, its four-byte array offset in ESI and the shared constant three in
  EDX. The original saved EDI and all later instruction positions return.
  The 36-byte local frame and 512-byte function span are unchanged; all
  512 bytes now match literally, up from 23, eliminating 132 differing
  encodings. The unoptimized WC2 counterpart corroborates this store order;
  the WC1 assembly and binary-comp establish the retained match. Three of
  the 720 independent initialization orders emit the exact function.
  All 27 relocation operands now occupy their original offsets, up from
  one. The navigation-pointer change alone gains 517 file positions:
  489 code bytes and 28 relocation bytes. All 400,896 code bytes outside
  this span and all symbol addresses remain unchanged.
- `ShowCarrierLaunchSequence` declares its object index before the frame
  counter, initializes fighter Y before loading the fighter shape, and sets
  the scripted-view object before initializing fighter X. This recovers
  twelve literal code bytes, from 807 to 819 of 2,064, and reduces differing
  encodings from 349 to 339 of 520. Mnemonic similarity remains 95.79%; the
  28-byte local frame still differs from the original 32 bytes. Relocation
  data is unchanged. These are measured ordering choices, not recovered
  original declarations. The initialization changes alone gain two bytes;
  moving the object declaration adds ten. Other tested declaration positions
  give no further gain. Moving the player-shape store earlier does not
  improve matching, and some variants shrink the aligned function span.
  Only these two rows change in the full report, raising normalized exact
  functions from 1,433 to 1,434. The combined gain is 529 file positions:
  503,091 of 518,656 (96.9990%), with 15,565 differences remaining.
  All 398,832 code bytes outside the two targets and all 3,797 map entries
  remain unchanged. Full verification, the modern build, sorting and address
  audits pass. Both value checks cover all 1,550 functions with zero
  mismatches or skips; existing call/global-access diagnostics remain 11/55.
  The executable retains SHA-256
  `73f58aaeb7f2fa54650f28f7a6b373c23e7f48c8053678c00a8129e61c3c4e50`
  through verification.
- `spc.c` drops the unused `<float.h>` include. Its declaration-count shift
  had made `house_keep_objects` exact but caused acceleration's common
  four-byte array offset to occupy EBX throughout the routine. That forced
  the magnitude onto the stack and enlarged its local frame from 32 to
  36 bytes. Without the include, `accelerate_and_move_object` again keeps
  magnitude in EBP and acceleration in EBX. Giving the desired afterburner
  speed its own int local finishes the timer register allocation: all 816
  bytes match literally, up from 78, and all 226 differing encodings vanish.
  A captured short timer or a block-scoped afterburner-speed local emits the
  same exact routine. Retain the separately named speed used by the scaling
  call; the measured velocity-delta magnitude has a different purpose.
  The header removal trades away the existing housekeeping match. Its
  short object counter and common four-byte offset exchange SI and DI,
  losing 51 literal bytes and leaving 49 differing encodings out of 247.
  The complete-file byte objective determines this choice. Promoted index
  and object-class captures, equivalent loop forms, register hints, tested
  standard-header contexts, and nearby object-array declaration moves fail
  to improve the combined result. With `<float.h>` retained, all tested
  acceleration local orders and timer/speed captures retain its bad frame.
- `animate_shape` declares the object type before the animation pointer.
  In the new unit context this recovers nine literal bytes, from 277 to 286
  of 352, reducing differing encodings from 29 to 21 of 104. Removing the
  promoted object index loses another byte and is discarded.
  Only these three rows change in the full report. Normalized exact functions
  remain 1,434 of 1,550: acceleration gains an exact match and housekeeping
  loses one. All spans, all 3,797 map entries, the other data sections, and
  all 399,232 code bytes outside these routines remain unchanged. The net
  gain is 696 code bytes and 33 relocation bytes, reaching 503,820 of
  518,656 matching file positions (97.1395%), with 14,836 differences left.
  Full verification, the modern build, sorting and address audits pass.
  Both value checks cover all 1,550 functions with zero mismatches or skips;
  existing call/global-access diagnostics remain 11/55. The executable
  retains SHA-256
  `9ddb5fa93803f3fa89e51d860da6fb9ee40b2398d89c880fd6422c4d3a4fabd4`
  through verification.
- `cutscene.c` replaces its unused `<process.h>` include with `<dos.h>`
  and `<sys/types.h>` before `wc1.h`. In `PlayScrambleHangarScene`, this
  lets the escape byte occupy AL after the counter comparison, instead
  of keeping it in BL. The draw calls and loop tail regain their original
  instruction positions. Initializing background X after the left walker's
  X also restores the scheduling of the shared zero before viewport setup.
  The 720-byte routine now matches 701 literal bytes, up from 255, and
  differing encodings fall from 121 to 15 of 177. Its complete mnemonic
  sequence matches; the loop's remaining register assignments do not.
  These are measured declaration and initialization contexts, not recovered
  original header text. The source comparison still uses the original
  signed counter test and conditional jumps.
  The new context initially disturbed four exact neighboring routines.
  Declaring `DrawScrambleActor`'s X coordinate after its frame byte and
  `funeral_sequence`'s frame before its followup-text pointer restores
  both complete matches. Moving `nSceneScrambleShipDetailCountValue` eight
  declarations earlier in `globals.h`, before `pScrambleView`, restores
  the comparison directions in `DrawScrambleFrame` and `landing`. All
  previously exact routines remain exact. `scramble` loses five literal
  bytes, from 1,072 to 1,067 of 1,104, and its differing encodings rise
  from 29 to 34 of 264. Moving the count five to nine declarations later
  improves the hangar routine further in isolation, but loses ten bytes
  in carrier launch; complete native builds reject that alternative.
- `main.c` includes `<dos.h>` before `wc1.h`, retaining its early stdio
  include. This restores the arithmetic scheduling in `calculate_damage_level`:
  all 240 bytes now match literally, up from 232. The normalized report
  had already called this routine exact, so its row does not expose the
  gain. Every other main-unit byte remains unchanged.
  Only the hangar and scramble rows change in the full report. Normalized
  exact functions remain 1,434 of 1,550, and mnemonic-perfect functions
  increase from 1,479 to 1,480. All spans and all 3,797 symbol/address pairs
  remain unchanged. Binary-comp confirms that all 399,344 code bytes
  outside the three changed routines are identical to the previous build.
  The net gain is 449 code bytes and 39 relocation bytes: 504,308 of
  518,656 matching positions (97.2336%), with 14,348 differences in 2,316
  runs. The data sections remain exact. The unchanged relocation-count
  deficit and relocation-section virtual size still prevent a whole-file
  match, alongside the remaining code differences.
- Additional flight-control expressions, local orders, header contexts,
  and chalkboard pilot-record declaration moves do not repair their large
  shifts. Palette-fade declaration exchanges, allocation guards, size
  expressions and a named component count leave its extra register copy.
  Counter captures at function, loop and block scope leave the walking
  animation unchanged. Its equivalent nonpositive counter test gains 451
  file positions by compensating the existing shift, but emits TEST/JG
  where the original has CMP/JGE. Discard that compensation. All trials
  use binary-comp for both function and literal whole-file comparisons;
  no dummy declarations, padding, flags or assembly changes are retained.
  Full verification, the modern build, sorting and address audits pass.
  Both value checks cover all 1,550 functions with zero mismatches or skips;
  existing call/global-access diagnostics remain 11/55. The executable
  retains SHA-256
  `f6ec55f85f47283060ea52681e863106fd29dd48b17aca9e17dab5d7b72d3026`
  through verification.
- `PrepareShapeRLEData` copies a named frame-start offset into the frame
  table with `memcpy`, matching its other serialized fields. The inlined
  copy computes the offset in EAX, recovering the original five-byte
  subtraction instead of the previous six-byte ECX form. The related WC2
  original at `0x00425BF6` corroborates this source idiom: it stores the
  offset in a real stack local and passes its address to a four-byte copy.
  WC1's own instructions remain the matching authority.
  Scope the offset to the frame loop and the prepared-data pointer to the
  final allocation block. Express the final byte count at its three uses;
  the optimizer retains the common result in EBX. These scopes keep the
  44-byte frame while restoring its original spill layout. Computing the
  bottom bound before the right bound gains two more literal bytes.
  These scopes and ordering choices are measured reconstructions, not
  recovered original source.
  The routine now matches 629 of 656 literal bytes, up from 305. Differing
  encodings fall from 125 to 14 of 217; mnemonic similarity remains 97.70%.
  The remaining differences are bound-calculation scheduling and register
  choices, including the operand loads for bitmap-size multiplication.
- `gr.c` includes `<stdio.h>` before `wc1.h`; it already uses `sprintf`.
  The declaration context restores the viewport surface-pointer arithmetic
  scheduling in `ValidateViewportBounds`, gaining six literal bytes, from
  225 to 231 of 240. Its mnemonic sequence now matches completely, with
  nine differing encodings instead of twelve. Only these two rows change
  in the full report. All 1,434 normalized exact routines remain exact, and
  mnemonic-perfect routines increase from 1,480 to 1,481 of 1,550.
  All spans, all 3,797 symbol/address pairs and all 400,512 code bytes outside
  the two routines remain unchanged. The net gain is 330 code bytes and one
  relocation byte: 504,639 of 518,656 matching file positions (97.2974%),
  with 14,017 differences in 2,302 runs. The other data sections remain
  exact. The original/rebuilt relocation counts remain 15,446/15,445, and
  their virtual section sizes remain 36,306/36,304 bytes.
- RLE trials that merely reorder or rename the original locals do not fix
  the offset register. A function-scope offset copy improves that register
  but enlarges the local frame to 48 bytes and disturbs later glyph code.
  Keeping a separate final-size local with the narrower scopes matches 631
  target bytes, but enlarges `DrawFontGlyph`'s aligned span by sixteen bytes.
  Tested header contexts do not recover the complete-file
  gain. Reusing the final-size local for the frame offset also enlarges the
  encoder span and is discarded. Further bound declaration/assignment
  orders and multiplication operand orders do not improve the retained
  result. Input-menu byte flags, joystick divisor captures and standard-header
  contexts in the geometry, calibration and mission units give no net gain.
  All comparisons use binary-comp; no flags, dummy locals, padding or
  handwritten assembly are introduced.
  Full verification, the modern build, sorting and address audits pass.
  Both value checks cover all 1,550 functions with zero mismatches or skips;
  existing call/global-access diagnostics remain 11/55. The executable
  retains SHA-256
  `7c3baaa799a6aa45cd4766b4a5d9c64eaca004d7ef653942544ecbda0510412b`
  through verification.
- `sort_object_depth` regains its complete literal match at the current
  event-manager declaration context. Declare the previous distance, unsigned
  candidate distance, selected object, scan index, placed-object pointer,
  screen offset and next-best distance in that order, followed by the sorted
  cursor and count. The second scan then uses EBP for its best distance,
  EBX for its screen offset, ECX for the object, EDI for the placed pointer
  and EAX for the candidate distance. In particular, moving the placed
  pointer out of EBP removes the extra displacement byte in its zero test.
  All 192 bytes match, up from 109, and all 35 differing encodings disappear.
  This is a measured declaration order; the routine's statements are unchanged.
  Direct array indexing with a loop-scoped best distance gets within four
  literal bytes, but removing the offset declaration disturbs the later
  rectangle routine. Separate scan-index/distance scopes and rectangle
  corner-index captures do not beat the retained declaration order. Reordering
  the five existing scan declarations preserves the unit's other code.
  Only the sorter's row changes in the full report, raising normalized
  exact functions from 1,434 to 1,435 of 1,550. All 3,797 symbol/address
  pairs and all 401,216 code bytes outside the sorter remain unchanged.
  The gain is 83 code bytes and three relocation bytes: 504,725 of 518,656
  matching file positions (97.3140%), with 13,931 differences in 2,291 runs.
  The other data sections remain exact; the relocation-count deficit and
  section-size discrepancy are unchanged.
- Projection's unsigned scale clamp and separate empty debris/asteroid
  cases still cannot be retained. All 120 case-block orders produce five
  table entries, while the original has six. Its two no-op entries remain
  distinct despite pointing to the same loop increment. Break, continue
  and a common next-object label, with or without an explicit default,
  also leave the collapsed table. Those builds remain sixteen bytes short
  and shift subsequent functions. Do not replace the missing source
  distinction with handwritten table entries or padding.
  Full verification, the modern build, sorting and address audits pass.
  Both value checks cover all 1,550 functions with zero mismatches or skips;
  existing call/global-access diagnostics remain 11/55. The executable
  retains SHA-256
  `d4d8b732dce20d49ab2d46f40fb6e285bc7d47397703772ca3183123cec38540`
  through verification.
- `screen.c` moves its existing `<malloc.h>` include before `wc1.h`, retaining
  the SDL guard. This restores EAX for `IsPushedPacketHandle`'s count, EDX
  for its table pointer and ECX for its handle argument. The count load
  recovers the original five-byte accumulator encoding, bringing the loop
  and found-entry test back to their original instruction addresses. Literal
  matching rises from seven to 61 of 64 bytes, and differing encodings fall
  from 21 to two of 22. Mnemonic similarity remains 90.91%. The final zero
  return still exchanges the order of POP ESI and XOR EAX,EAX; its three
  differing bytes are the sole remaining literal mismatch in the routine.
  The include move initially reverses both loop comparisons in
  `GetLargestFreeNearHeapBlock`. Writing its condition with the lower bound
  on the left restores that routine's preceding bytes. It retains the two
  existing differences in the commutative base/size loads, hidden by the
  normalized exact report. Local declaration orders, alternate arithmetic
  grouping, separate initialization statements and named heap-bound captures
  do not improve the complete-file result.
  Only the packet-test row changes in the report; all 1,435 normalized exact
  routines remain exact. All 3,797 symbol/address pairs and all 401,344 code
  bytes outside the packet test remain unchanged. The gain is 54 code bytes
  and three relocation bytes: 504,782 of 518,656 matching file positions
  (97.3250%), with 13,874 differences in 2,285 runs. The other data sections
  remain exact, and the relocation-count deficit is unchanged.
- Packet-test return rewrites, result widths/scopes and explicit count
  captures do not fix the final return scheduling. For the answer-prompt
  loader, all local declaration orders, tested standard-header contexts,
  register hints, packet-assignment chains and line-start/difference captures
  leave the scan/prompt register mismatch unresolved. Those trials are
  discarded; compiler flags and the console construction cleanup are unchanged.
  Full verification, the modern build, sorting and address audits pass.
  Both value checks cover all 1,550 functions with zero mismatches or skips;
  existing call/global-access diagnostics remain 11/55. The executable
  retains SHA-256
  `dc296c1c5d9e0c8b6105bd806b14af6d8f31c63cf121db057ad68c2592d0861a`
  through verification.
- `logic.c` includes `<sys/types.h>` before `wc1.h` in the native build.
  With `UpdateSceneAnimationObject` declaring `labelNumber` before `frame`,
  `property` after `goalFlags`, and `delay` after `layer`, its complete
  1,552-byte span becomes exact, including its switch tables and alignment.
  Literal matching rises from 1,377 to 1,552 bytes; all 155 differing
  encodings disappear. The include alone restores the object/cursor register
  assignment and leaves nine differing encodings. The declaration moves
  resolve the negative-rotation wrap's CX/DI allocation and the label-search
  comparison direction. This is a measured compiler context, not evidence
  of the original header arrangement or local declaration spelling.
  The same context makes `init_constellation` exact at 176 bytes, up from
  120. `attacker_in_range` captures the returned short distance in a scoped
  integer before storing its low word and comparing the promoted value;
  this restores the original 154-byte body after the include alone shortened
  it by one byte. That declaration also makes `FindSceneAnimationCommand`
  exact at 192 bytes, up from 191. Declaring `find_ships_sphere`'s navigation
  pointer and fallback before its ship index recovers most of that routine's
  register assignment after the added distance local.
  A qualified first collision-radius read preserves `real_crash_time`'s
  original argument-load order, and reversing the source comparison in
  `close_behind` preserves its original operand order. Both routines retain
  their preceding exact bytes. The read qualifier is a matching choice;
  it does not establish that the original array declaration was volatile.
  The retained unit tradeoff improves `hostile_sphere` from 73 to 90 of
  96 bytes, but loses 22 bytes in `attacker_in_range`, seven in
  `evaluate_damage`, six each in `no_goal` and `find_ships_sphere`, two in
  `scan_for_enemy`, and one in `InitializeCockpitResources`.
  `find_ships_sphere` and `InitializeCockpitResources` lose their normalized
  exact labels, with six and one differing encodings, respectively.
  `no_goal` still reports normalized exact despite its six
  literal differences in the order of the three global operands. The
  attacker and damage routines fall to 87.50% and 88.89% mnemonic similarity;
  retain this intermediate tradeoff for its verified whole-file byte gain.
  All 3,797 symbol/address pairs and all 397,584 code bytes outside the ten
  changed spans remain unchanged. The net gain is 205 code bytes and three
  relocation bytes: 504,990 of 518,656 matching file positions (97.3651%),
  with 13,666 differences in 2,141 runs. The `.rdata`, `.data` and `.idata`
  sections remain exact, and the relocation-count deficit remains one.
  The report reaches 1,436 normalized exact and 1,484 mnemonic-perfect
  functions, with 111 same-size differences and three size differences
  totaling a net 26 bytes. Three functions are below 90%; the mean remains
  99.83%.
- Further logic trials are discarded. Header orders, local permutations,
  initialization orders, register hints, argument captures, qualified goal
  reads, damage-term permutations and casts, explicit damage accumulators,
  and qualified denominators do not improve the selected complete-file
  result. Alternate distance-assignment chains and `long` captures leave
  the attacker result unchanged. Subtracting the distance before testing
  its sign adds instructions absent from the original. Additional captures
  can change later functions' register allocation even when the edited
  routine's output is unchanged; do not assess these probes in isolation.
  Full verification, the modern build, sorting and address audits pass.
  Both value checks cover all 1,550 functions with zero mismatches or skips;
  existing call/global-access diagnostics remain 11/55. The executable
  retains SHA-256
  `5504a12307d0bb35d5b91c4e8ca57af705ad7ad5b294992af761c3ea653cd578`
  through verification.
- `animate_shape` casts the shrink factor's scale-read index to unsigned
  int while retaining the signed destination index. CL 4.20 then emits
  separate absolute scale operands instead of one shared computed address.
  The routine regains its sixteenth HIGHLOW relocation, bringing the whole
  executable to the original 15,446. The relocation directory's virtual
  size becomes the original 36,306 bytes, and all 1,024 PE header bytes now
  match literally when rebuilt from the C source.
  This source distinction is a matching choice; it does not establish the
  original index declaration. The scale arithmetic still uses the original
  word operands and logical right shift.
  Literal matching in the 352-byte animation span rises from 286 to 288.
  The shrink path still has a memory SUB; the original subtracts in a
  register and then stores the word. It retains 21 differing encodings and
  64 literal differences, and mnemonic similarity falls from 99.05% to
  97.12%. The retained benefit includes seventeen more matching relocation
  bytes and the final header byte, for twenty additional file positions.
  All 401,056 code bytes outside this function and all 3,797 mapped
  symbol/address pairs remain unchanged. Every source span now has its
  original relocation count, although relocation positions still differ.
  Whole-file matching reaches 505,010 of 518,656 positions (97.3690%), with
  13,646 differences in 2,128 runs. The continuous matching prefix grows
  from 544 to 1,088 bytes; the first remaining difference is at file offset
  `0x440`, in code. There remain 12,820 differing code bytes and 826
  relocation bytes. The `.rdata`, `.data` and `.idata` sections remain
  exact, and every section's virtual and raw size matches the original.
  Only the animation row changes in the function report. Counts remain
  1,436 normalized exact, 111 same-size differences and three size
  differences totaling a net 26 bytes, with 1,484 mnemonic-perfect
  functions and a mean of 99.83%.
- Additional transformed-bounds, animation and mono probes are discarded.
  For `GetTransformedShapeBounds`, pair swaps and single moves through the
  fourteen branch locals, span/dimension and bound permutations, split
  arithmetic statements, named span corrections, promoted scale/angle
  captures, wider bound temporaries with word comparisons, and all eight
  multiply-operand reversals in combination give no whole-file gain.
  Animation scale/factor captures, signed access casts, qualifiers and
  explicit subtraction forms fail to improve on the retained mixed-index
  compound assignment. All 120 declaration orders with a scale-change
  local and all 120 with a saved scale retain the missing relocation.
  Making both shrink indices unsigned gains more animation bytes but less
  across the complete executable and retains only fifteen relocations.
  Mono header placements, string-pointer captures and seek-result widths
  leave its two register mismatches unresolved. None of these unsuccessful
  probes changes the retained source.
  Full verification, the modern build, sorting and address audits pass.
  Both value checks cover all 1,550 functions with zero mismatches or skips;
  existing call/global-access diagnostics remain 11/55. The executable
  retains SHA-256
  `5e4eda13530862a1090aa7463047e9b2043e473f4e78fa0c01d6486d8e2af900`
  through verification.
- In the `/Od` C++ `ix` units every explicit cast consumes one symbol index,
  unlike the C core. CL's `$L`/`$SG` listing numbers read the counter
  directly: the first label number inside each `PROC` shows how many indices
  the preceding source consumed, without a probe sweep. A probe prototype
  costs two indices in these units and `extern int` probes cost one, so sweep
  `ix` units with integer probes. The comparison operand-order tie in these
  bounds checks depends on the counter at the function's start: a code-free
  declaration inside the body moves only later functions.
- `dsps.cpp` writes the voice gain bytes without the redundant outer
  `(unsigned char)` casts, in both the allocator and the volume setter, and
  assigns the mixer rate without a `(short)` cast. With `<assert.h>` included
  after `<stdlib.h>`, all ten stream routines are exact; `ix_dsps_alloc` and
  `ix_dsps_stop` were the two misses. The include position matters: placed
  before `<stdlib.h>`, the same header shifts the last four routines by two
  more indices.
- `dspv.cpp` computes the voice position as
  `(int)((unsigned long)v->cursor - (unsigned long)v->start)`; the three casts
  emit the same subtraction and supply the three indices the following
  setters need. The volume setter's volatile voice-count read is gone; it cost
  one index and was compensating for the missing ones. All fourteen voice
  routines are exact, and the one-byte call displacements in
  `ix_system_assign_voice` and `ix_system_service_sounds` disappear.
  These are measured source contexts, not recovered original text.
  The report reaches 1,440 normalized exact functions; whole-file matching
  reaches 505,761 of 518,656 positions. The remaining size difference is the
  sample converter's span over the DirectSound import stubs.
- `DIBupdate` takes an ordinary `top` parameter. Its former volatile
  qualifier imitated a symbol-count effect: without it the routine is exact
  at every seventeenth offset. The whole unit then needs 27 more symbols
  before its first routine, but the frame-size product in `DIBmakeDIB` and
  `DIBdestroyDIB` is an operand-order tie between two unit statics, so the
  split around the static definitions matters as well: 23 symbols before
  them and four after. `<memory.h>`, `<sys/stat.h>` and `<sys/utime.h>`
  supply the 27; `dib.c` forward-declares `DIBcascade` and `DIBslamReal`,
  which `DIBreInstall` calls before their definitions, after its globals
  instead of seeing them in `wc1funcs.h`. All 28 routines are exact.
  An object-level byte compare that masks relocated fields cannot see such
  global operand swaps; resolve each relocation through the map and compare
  it with the original's operand before trusting an exact result.
- Ten routines that the normalized report called exact differed only in the
  globals their relocated operands named: evaluation and commutative-operand
  ties between globals, such as the rotation-error sums. Checking each
  relocation's resolved address exposes them. Moving six `extern`
  declarations in `globals.h` fixes seven of them with no other change in
  any unit: `anObjectYawRotation` two declarations later fixes
  `rotate_eye_to_goal`; `nNearHeapBase` two earlier fixes
  `InitializeNearHeap`, `GetFreeNearHeapBytes` and
  `GetLargestFreeNearHeapBlock`; `anPitchGoal` four later fixes
  `rotate_object_to_goal`; `anRollGoal` three later fixes `no_goal`;
  `nEnemySighting` seven later fixes `imperial_formation`; and
  `nCurObjectiveRangeValue` two later fixes `check_objectives`. Each move was
  accepted only after compiling every unit and lowering the total count of
  differing bytes. These are measured declaration orders, not recovered
  header text.
- The sprite background capture and restore routines take the shape of
  WC2's unoptimized counterparts in `../wc2-re/src/gr.c`: the frame word is
  scaled in place (`frame++; frame <<= 2;`), the run byte doubles as the run
  length, the colour byte is read into its own local, and the clipping
  arithmetic subtracts into `copyLength` and adds into `skip`. The WC2-only
  viewport and allocation-signature guards are absent from WC1. With the
  capture locals ordered `copyLength`, `commands`, `save`, `screen`,
  `runRight`, then WC2's order, `CaptureSpriteBackground` is exact; the old
  reconstruction with volatile run-length reads differed in 247 bytes.
  `RestoreSpriteBackground` keeps WC1's trailing DIB refresh, reached by a
  `goto` from the frame-range test, and differs only in two registers
  (23 bytes, down from 252).
  The WC2 shapes consume three fewer symbol indices than the old code, and
  `CopyViewportContents` supplies them as WC2 writes it: the blit result is
  kept in a local and passed to a discarded `abs` call that the optimizer
  removes. Without that, the later `DrawFilledViewportRect` loses its exact
  match. A trial `<crtdbg.h>` include kept every exact routine but grew
  `DrawFontGlyph` by sixteen bytes and shifted all later code; reject
  context changes that alter any routine's size, exact or not.
- `GetTransformedShapeBounds` follows its WC2 counterpart: the frame index
  is scaled in place, the cosine and sine products are shifted in separate
  statements, and the left edge is formed by reusing `extentY` for the
  scaled span. Without WC2's allocation-signature guard and with `topEdge`
  declared after `cosine`, the routine is exact (it differed in 170 bytes).
- `transform_objects_to_your_view` follows WC2's projection loop where the
  WC1 code agrees: the clamped screen scale is kept in a `screenScale`
  local before it is stored, and the dust frame is formed in two statements
  that reuse `dustSize`. The original six-entry jump table gives debris and
  asteroids their own empty slots. Separate debris and asteroid case blocks
  reproduce it only when each block holds a statement the optimizer later
  deletes; empty blocks, `continue` and shared labels all collapse the
  table to five entries. Dead stores to `dustSize` stand in for the original
  case bodies. With `objectRadius` declared before `distance`, the routine
  is exact (it differed in 301 bytes).
- `PollMenuInputDevices` takes WC2's structure: the polling guard encloses
  the body instead of returning early, each button block computes its own
  signed-char double-click flag, `changes` counts as an unsigned char, the
  idle path reads the device sample directly, and the cursor state is copied
  as x, y, primary, secondary, flags. It is exact (it differed in 487 bytes);
  of the 120 copy orders only WC2's is exact. `PollJoystickButtonEvents`
  uses WC2's two early returns and copies the buttons before releasing the
  guard, falling from 32 to 11 differing bytes. `UpdateInputDeviceTransitions`
  keeps WC1's form: WC1 stores 1 into its change tables and tests one XOR of
  the button words, where WC2 increments and masks each bit.
- `ParseTests` returns `sceneData + int_value(&test)` directly in case 1
  and uses the short `first` for the medal and late-ejection tests (cases
  13 and 26); the case-local int `destination` was a matching device and is
  gone. With case 1 spelled this way, every return block takes the original
  register pattern (`movsx edx` with the record base reloaded, or `movsx eax`
  with the base added from the stack). Probe declarations after the includes
  never change those patterns, which depend only on the expression shape
  earlier in the function. The two mission-score comparisons then still had
  their operands swapped. That is a global-index tie, so it moves with
  probes placed before the includes but not after them. Moving the
  `stCampaignState` extern four places up in `include/globals.h` fixes it
  without changing any other unit, and the routine is exact (it differed
  in 717 bytes).
- `ShowChalkBoard` compares the two pilots' scores by reading both order
  slots in the condition. It loads the swap value from the candidate slot
  only inside the taken branch, and it declares `other` before `index`.
  CL canonicalizes the comparison. Writing it as `<` or as the mirrored `>`
  emits identical code. The declaration order of the two loop indexes
  decides which score is computed first, and with it which slot address
  stays hoisted in ESI. The text context's viewport is assigned after the
  previous context is saved, so the store lands between the font call's
  argument pushes. The routine is exact (it differed in 688 bytes).
- `bCurrentKey` is a signed char. Every original load of it sign-extends
  (`movsx`) and none zero-extends, so the earlier `unsigned char` type with a
  `(signed char)` cast at each use was a reconstruction artifact. With the
  signed type and no casts on the key, `HandleSpaceFlightControls` is exact
  (it differed in 2,760 bytes) and so is `HandleFleetOverviewInput`. The
  case 2..10 test then reads the key twice from memory, as the original does.
  So does its first comparison against `cPreviousKey`, which uses CL. The
  volatile casts on `bCurrentKey` and `cPreviousKey` in `player_input` had
  to go: CL marks a global whose address is taken anywhere in the unit,
  including in functions that come after the use, and that marking moved
  registers in both routines. The release-weapon test in case 0x1c reads
  `nSelectedReleaseWeaponIndex` into `selectedWeapon` without a volatile
  cast or a slot pointer, and both volume toggles use if/else.
  `player_input` now differs in 25 bytes, up from 4. CL keeps the key in a
  register across the two release ORs and across the `cPreviousKey` reset;
  the original performs all of them in memory. No header position and no
  probe offset prevents that promotion. A volatile declaration of the key,
  `#pragma optimize("g", off)` around the routine, and every on/off
  combination of its remaining volatile casts are all worse.
- `LoadJoystickCalibrationFile` and `ReadCalibratedJoystick` no longer use
  volatile casts. Address-taking in either routine marked the joystick
  scales for the whole unit, which turned Read's register divisions into
  memory divisions. Read now takes its WC2 shape: direct indexed sample
  accesses and a `result` local for the sampling call. Load keeps the
  calibration scales in memory across their conditional increments while
  CL still promotes the minimum X bound. That split comes from the capped
  promotion set, which depends on the globals' symbol indexes, so it needs
  a shift of the header globals: `<signal.h>` before `wc1.h` provides it,
  and `<sys/types.h>` after it sets the unit's own offset. Load forms the
  left product before the up product, increments `horizontalRange` with
  `++`, and declares `rangeY`, `rangeX` and `centreX` first. Moving the
  `nCurrentJoystickFailureValue` extern one place down in `globals.h` fixes
  Read's failure-value comparison order without changing another unit. Both
  routines are exact; together they differed in 241 bytes. The
  `UnionRectBounds` helper that follows now differs in 6 bytes, an operand
  order tie twelve probe units away. No include pair satisfies all three
  routines.
- `nVideoMode` is a short and the pilot-hand frame globals
  (`bStickIndicatorFrameState`, `cRenderedPilotHandFrame`) are signed chars.
  The original only ever sign-extends them. The casts that compensated at each use are
  gone; these retypes leave every routine's bytes unchanged.
- `spc.c` includes `<signal.h>` and `<sys/stat.h>` ahead of `wc1.h`. The
  shift of every header symbol makes `set_eye_direction_and_position` exact
  (it differed in 409 bytes), along with `house_keep_objects` (51). This
  matches the offsets where pre-include probes had made both exact. It
  costs `rotate_eye_to_goal` four bytes, a global operand order that no
  loss-free extern move restores, and adds eight to `animate_shape`.
  Moving `nExternalViewShip` five places up restores `object_intelligence`
  without touching another unit.
- `joystick.c` includes `<sys/types.h>` and the debug CRT's `<crtdbg.h>`
  ahead of `wc1.h`. That header-symbol shift puts all three calibration
  sample blocks of `CalibrateJoystickInteractive` on their original
  pattern: the first two keep the X sample in EAX and form the Y address
  separately, and the third loads both from a shifted base. Declaring the
  four scales before the two ranges then brings the routine from 257 to
  60 differing bytes. What remains is the horizontal range's register and
  the interleaving of the scale stores with the bound products. Forming the
  products inline in the bound expressions is worse under every include
  pair.
- `StepPaletteTransition` uses `componentCount` directly instead of
  copying it into a `count` local in both branches. With the copy, CL
  merges the two parameter loads ahead of the initialization test; the
  original reloads it in each branch. Declaring `palette` before
  `previousCountdown` fixes the remaining addressing-operand order, and the
  volatile cast on `nPaletteTransitionInitialise` is unnecessary. The
  routine is exact (it differed in 202 bytes).
- `AnimateScrambleWalk` tests `bEscapePressedFlag` directly after the
  coordinate updates, like the other cutscene loops; the escape-snapshot
  local was a matching device. The original's early flag loads come from
  CL's capped promotion set. The routine keeps the background and left
  walker coordinates and the escape flag in registers, and leaves the right
  walker in memory. Which candidates are dropped depends on the symbols'
  indexes: with the direct read, post-include probe offsets 8–10 make the
  routine exact. In real source, `<signal.h>` ahead of `<dos.h>` and
  `<memory.h>` after `wc1.h` make `PlayScrambleHangarScene` exact (it
  differed in 19 bytes) and bring `AnimateScrambleWalk` from 524 to 27
  differing bytes. There the cap drops the left walker instead of the
  right. Header pairs that make `AnimateScrambleWalk` exact cost the
  hangar scene, `scramble` or `landing` more.
- `find_next_gun` follows its WC2 counterpart: separately initialized
  locals, a `for` loop that advances the slot pointer in its increment
  clause, and the slot type compared directly. With `firstGun` declared
  first and `foundCurrent` last it is exact (it differed in 50 bytes).
  The `for` loop moves the unit's later symbols; `<assert.h>` ahead of
  `wc1.h` and `<process.h>` after it keep `select_new_release_weapon`
  exact.
- `init_intelligence_data` follows WC2's order: it clears the special
  maneuver right after `clear_alert`, then reads the mission target. It
  assigns `aShipMissionSpotArray[obj]` directly instead of through a
  `missionSpot` pointer. The pointer let CL schedule the vector copies
  differently, and the volatile casts were matching devices. The routine is
  exact (it differed in 72 bytes).
- `check_futurion` copies the object class into the counter before
  replacing it, as WC2 does, without a `prev` local. It is exact (8 bytes).
  That removes one symbol ahead of `shape_loaded`.
  `<process.h>` and `<crtdbg.h>` ahead of `wc1.h` restore it and also make
  `set_formation_position` exact (3 bytes). `find_next_ship_turn_slot`
  gains two differing bytes.
- `animate_shape` takes WC2's shape: it indexes by `obj` directly, stores
  the masked animation index and re-reads it to fetch the next command, and
  uses plain scale updates. It drops the earlier `objectIndex` local and
  odd index casts. It falls from 60 to 25 differing bytes. What remains
  (the type-data offset in EDI instead of EAX, and the factor order in the
  scale products) does not move with local order, operand spelling or
  probe offsets.
- `ShowCarrierLaunchSequence`'s original frame is 32 bytes because
  `object` is not a declared short. The routine indexes with the
  `sceneObject` parameter, and CL spills its sign-extended copy into a
  dword slot next to the fighter shape. Using the parameter directly
  restores the 0x20 frame. With fighter shape, view object and both
  fighter coordinates set in that order, it also restores the 2,064-byte
  span. The spill slots of the four index temporaries come out in a
  different order, though, so that form differs in more bytes (1,367) than
  the retained one (927).
- `debug.cpp` includes `<malloc.h>` and `<locale.h>` ahead of `wc1.h`. The
  shift of the header symbols changes which import pointers
  `DebugOverlayConsole::WaitForKey` keeps in registers. It also moves the
  row and `TextOutA` registers in `DrawPendingLines` and makes `Clear`
  exact. The unit falls from 130 to 38 differing bytes (object
  comparison); `DebugOverlayPrintf` now differs in its two row-times-width
  operand orders.
- `DrawMedals` adds the medal-row offset to `rowY` in place and copies it
  into `medalRowY`, which is an `unsigned short`. With matching types CL
  merges the two variables and drops the copy. The original keeps the copy
  and reuses `rowY`'s slot for the hoisted frame number. `medalCount` is
  declared at function scope between `medalRowY` and `rowY`, and `stack`
  last. The routine falls from 155 to 2 differing bytes; the loop test
  still reloads the medal count into CX instead of AX under every local
  order and probe offset.
- Moving the `nAfterburnerSoundDeadline` extern ten places down in
  `globals.h` makes `your_afterburner` exact without changing another
  unit.
- Moving the `cScrambleWalkerPair` extern eleven places up in `globals.h`
  makes `AnimateScrambleWalk` exact, and moving `nCurrentWave` seven places
  down makes `set_up_next_wave` exact. No other unit changes under either
  move.
- `ShowTigerClawEscapeScene` follows its sibling `ShowCampaignVictorySequence`
  (`planetScale`/`projectedY`). The vertical projection goes into an `int`
  local, and the call adds the view centre itself. Folding the centre into a
  separate `screenY` statement makes CL hoist the wrong centre global.
  `verticalOffset` is assigned right after `depth` and declared before it.
  That order puts depth in EBP and the offset in EDI, and leaves ESI to the
  CSE'd constant 1. The routine is exact.
- `Title_Sequence` fetches both menu shapes before it stores `menuOptions[0]`
  and then `optionCount = 1`. Setting the count between the two fetches puts
  the CSE'd constant 1 in EDI before the first call instead of the second.
  `title.c` includes `<sys/stat.h>` after `wc1.h`; that shift moves the
  switch key from CX to DX. The routine falls from 61 to 14 differing bytes
  (object comparison). Only the range check on the selected menu entry
  remains: CL emits the `state = menuOptions[...] + 1` branch first under
  every if/else, ternary and early-break form tried, and under every probe
  offset.
- `BarracksScreen` uses no labels. The keyboard cases fall into the click
  case through `if (clicked == 0) break;`, and the region handling lives in
  `case 2: case 10:`, so the jump table sends clicks straight to
  `FindMenuRegionAtPoint`. The old `goto activateRegion`/`goto redraw`
  version set `clicked` and tested it after the switch. That moved the
  `UpdateRoomMenuCursor` case ahead of the activation code. The routine is
  exact.
- `AddRandomTrainSimHighScores` stores the new score in an `unsigned int`
  local before calling `InsertTrainSimHighScore`. The assignment gives the
  original `LEA ECX,[EAX+EBX+0x32]` / `PUSH ECX`; the inline argument adds
  in EAX instead. Declaring `baseScore` after `scoreRange` and `score` puts
  it in EBX and the scale conversion in EBP. The routine falls from 42 to 2
  differing bytes; the original keeps `remaining` at the low half of its
  four-byte slot (`[ESP]`), ours at the high half. CL overlays locals with
  disjoint lifetimes in one slot (`CloseLook` shares -4 between `frame` and
  `start`), so the original's slot partner is still unidentified.
- `internal_damage` follows WC2's capital-ship branch: each side adds
  `MaxShort(...)` straight into `asShipAccumulatedDmg[victim]`, with no
  pointer local and no `events` store. `onboard_explosion` sits in the
  `else` of the explosion test. That `else` supplies the one symbol the
  dropped block local held, which keeps `analyze_kill` and
  `replenish_shields` on their original offsets. The routine falls from 57
  to 37 differing bytes; the remainder is a swap of the victim index (EBX)
  and the event count (SI) that no declaration order or probe offset fixes.
- `sort_viable_target_list` declares `outer`, `distance`, `target`, `count`,
  `inner` in that order. This restores the inner index in DI and the
  original `count <= inner` loop test. The routine falls from 14 to 9
  differing bytes; `distance` still lands in AX instead of SI with the
  comparison reversed, under every declaration order, probe offset and
  array-comparison form tried.
- `scan_for_enemy` declares `distance` before `other`, which makes it exact.
  `PlaySceneAnimation` declares `complete`, `objectCount`, `objects`,
  `remaining` and then `object`, which takes it from 15 to 5 differing
  bytes. Both orders came from a sweep of every local-declaration order of
  the non-exact routines with at most five contiguous locals (scratchpad
  `permall.py`). The sweep found nothing else at the current offsets.
- Swapping two `extern` lines in `globals.h` changes only those two symbols'
  indices; a single move also shifts every extern it passes. Swapping
  `cCurrentCockpitView` with `stPreviousFlightInput`, 88 lines away, makes
  `InitializeCockpitResources` exact (its `mode == cCurrentCockpitView`
  operand order). No other unit changes. Moving `cCurrentCockpitView` the
  same distance broke `AnimateTrainSimTitle` and `update_scripted_view`
  through the shifted externs in between. Search swaps with scratchpad
  `gswap.py`.
- `PlaceNavMapLabel` reads the new label's position as
  `aNavMapLabels[nNavMapLabelCount].x`/`.y` for `ReserveNavMapArea`. A
  `label` pointer local folds the array base into a `LEA` and loses the
  original `[ECX+disp32]` operand. Dropping the local removes one symbol.
  `AddUniqueObjectiveNavLabel` restores it with
  `if (previous < objective) return; else PlaceNavMapLabel(...);`, which
  keeps `IsPointInNavMapLabel` and `SelectNavObjectiveAtPoint` on their
  offsets. `PlaceNavMapLabel` is exact.
- Two more clean `globals.h` swaps. Swapping `nViewAllocTotal` with
  `nScannerCursorY` restores `free_viewport`'s allocation-count comparison
  order (4 to 2 differing bytes). Swapping `anObjectRollRotationList` with
  `aasShipMaximumShieldTable` restores the order of `check_for_lost_control`'s
  three absolute-rotation terms (5 to 2). The remaining `free_viewport`
  bytes are its `stSceneBuffer` comparisons, which no swap changes.
  `check_for_lost_control` still sums the turn rates as pitch, yaw, roll
  instead of roll, yaw, pitch; CL orders those terms by the `ObjectTypeData`
  member symbols, which no extern swap moves.
- `DrawMedals` counts each campaign medal with a `for` loop. The earlier
  `medalCount = 0; while (...) { medalCount++; ... }` shape put the
  sign-extended medal count in `CX` for the bottom-of-loop test; with the
  increment in the loop's own step (or at the end of the body) CL uses `AX`,
  as the original does. The routine is exact.
- `GameFlow` resets its per-visit flags with `nPostSeriesSequence = -1;`
  before `bPlayerEjectedThisMission = 0;`. CL hoists the `0` and `-1`
  constants out of the barracks loop into callee-saved registers; with the
  `-1` store first, `0` takes `EBX` and `-1` takes `EDI` as in the original.
  CL still emits the eject-flag store before the sequence store, so the
  source order shows only in the register choice. The routine is exact.
- Further carrier-launch probes do not improve the retained implementation.
  With `sceneObject` used directly, all 24 orders of the fighter-shape,
  scripted-view-object and two fighter-coordinate initializations recover
  the 32-byte frame, but retain either a sixteen-byte span shortage or worse
  bytes at the original span. Moving the player-shape store before the
  initial object fields and moving actor Y to function scope do not finish
  the recovery. All 122 distinct single-local moves in that combined form
  also fail to improve the executable. Capturing the object as an int or
  long, or as a signed byte, gives no gain; an unsigned capture also loses
  the original sign extension and is not a valid recovery.
  The original's four index slots, relative to ESP after the saved registers
  and before call arguments, are: promoted index at `+0x18`, doubled index
  at `+0x20`, quadrupled index at `+0x24`, and the sign-extended word at
  `+0x28`. The tested direct-parameter form instead puts them at `+0x28`,
  `+0x18`, `+0x20`, and `+0x24`. Matching the frame size alone therefore
  does not establish the original stack layout. Keep the current source
  until those temporaries and the instruction schedule are recovered together.
- Smaller probes at this checkpoint are also discarded. Capturing the text
  pointer in `ResetStringBuilder` retains its EAX/ECX swap. Early returns,
  loop breaks and alternate integer result types do not repair
  `IsPushedPacketHandle`'s final POP/XOR order. Qualified proximity reads do
  not repair `Mveer_away`'s comparison and lose executable byte matches.
  Captured yaw-rate, pitch-input and product locals do not improve
  `players_flight_dynamics`, nor do the 251 tested swaps of
  `nPitchInput` with single-line externs in the same preprocessor context
  within 150 header lines. Reordering the release-weapon coordinates leaves
  its extra saved EDI; moving the state stores earlier or inlining its
  hardpoint index produces only one to four incidental equal file positions
  while degrading its instruction schedule (94.59% to 83.78% or 70.27%).
  These variants do not resolve the original register lifetime and are not
  retained. Every compiled candidate was checked through binary-comp,
  including its strict positional comparison with the normal link timestamp.
  The unchanged executable has 514,661 matching positions out of 518,656,
  with 3,995 differences; the function report has 1,467 normalized exact
  entries out of 1,550. These remain separate measures of incomplete recovery.
- `nTickCount60Hz` and `UpdateInputDeviceTransitions`'s `now` snapshot are
  signed ints. Correcting both types restores the original register choices
  and the timestamp load shared by the two button branches; changing only
  one type does not. All 288 bytes of the routine now match, eliminating
  143 differing code bytes and eleven differing relocation bytes. Rebuilding
  every core unit changes no other code or data bytes. Normalized exact
  functions rise to 1,468 of 1,550, and strict file matching rises to 514,815
  of 518,656 (99.2594%), with 3,841 bytes still different. The signed elapsed
  comparisons and divisions at the clock's other use sites agree with this
  declaration; the GetTickCount-based producer still performs its original
  unsigned arithmetic before storing the clock.
- `AllocateTaggedMemory` takes a signed int size. Its unsigned prototype
  made `FadeViewportPaletteToColour` retain an extra copy of the memset
  count, shifting the later instructions. Correcting the declaration and
  definition reduces that routine from 359 differing bytes to nine at the
  unchanged 496-byte span; only the second loop's AX/DI allocation remains.
  The shared prototype also exposes `AllocateViewport`'s missing initial
  loop guard. Initialize its row offset before allocating the row table,
  capture `height + 2` in an int row count, reload `viewport->rowOffsets`
  in the guarded loop, and advance the offset before incrementing the row.
  Together these recover its eight-byte frame, original registers and all
  320 bytes. Keeping the bound only as an expression spills height and
  enlarges the frame; a captured unsigned bound also loses the match.
  Both corrections preserve every function address and all data bytes.
  Code gains 485 matching positions and relocations gain five, bringing
  strict file matching to 515,305 of 518,656 (99.3539%), with 3,351 bytes
  still different. Only these two function-report rows change: normalized
  exact functions rise to 1,469 of 1,550, and two remain below 90% mnemonic
  similarity.
- `FadeViewportPaletteToColour` uses separate short counters for preparing
  the palette entries and applying each transition step. Declare the second
  counter between the first counter and `activeCount`. This restores DI for
  the second loop's counter, EAX for its promoted index, and the original
  comparison direction. All 496 bytes now match. Only nine code bytes change;
  every other executable byte is unchanged. Normalized exact functions rise
  to 1,470 of 1,550, and strict file matching reaches 515,314 of 518,656
  (99.3556%), leaving 3,342 differing bytes.
- `DrawFontGlyph` initializes the viewport and drawing colour immediately
  after the row, before the background and font pointer. CL schedules the
  resulting setup exactly as the original: background load and spill, font
  pointer, viewport, drawing colour, and the two font-colour bytes. Only
  thirteen code bytes change, all toward the original. The 512-byte span
  and 24-byte frame remain intact; differing encodings fall from 61 to 57
  of 162, and literal differences fall from 168 to 155. Every other file
  byte is unchanged. Strict matching reaches 515,327 of 518,656 (99.3581%),
  leaving 3,329 differing bytes; normalized exact functions remain 1,470.
  Alternate destination-offset integer types and direct font lookups emit
  the preceding code. Capturing the promoted character index, splitting
  address locals between rendering branches, duplicating cursor advancement,
  and moving individual pointer or counter declarations do not improve it.
  All 120 declaration orders in `LoadAnswerPromptAndResponse` leave at least
  its existing 112 literal differences. A release-weapon position pointer
  loses whole-file matches; copying its position structure removes the extra
  saved register but substitutes dword accesses for the original word loads.
  Those alternatives are not retained.
- `easy2see` keeps the raw screen X coordinate in a separate short from
  the translated drawing X coordinate. Declare that snapshot between X
  and Y, test it for the off-screen sentinel, then add the view centre
  into X. This restores the original index and coordinate registers and
  all 128 bytes, gaining 28 literal positions. A direct table test followed
  by the coordinate expression gets most registers right but adds two
  instruction bytes and sixteen aligned bytes; reusing X for the snapshot
  retains the preceding allocation. The new local changes no other code.
- `find_next_ship_turn_slot` captures the pilot's interval in a signed char
  before storing it in the object table, then uses that unchanged value
  throughout the search. This matches the original BL lifetime and restores
  its final `CMP BL,DL` / `JL`, gaining two bytes. Its 128-byte span remains;
  the argument/counter register swap still differs. Other declaration and
  initialization orders give no larger gain. Captured cockpit frame/view
  state and split raw coordinates in sorted-object drawing do not improve
  those routines and are discarded.
  Together, only thirty executable bytes change, all toward the original.
  All 3,797 symbol/address pairs remain unchanged. Normalized exact functions
  rise to 1,471 of 1,550; strict matching reaches 515,357 of 518,656
  (99.3639%), leaving 3,299 differing bytes. The common prefix grows from
  1,088 to 8,308 bytes.
- `GetShapeFrameCorners` increments and scales the frame parameter before
  copying the resulting byte offset into its existing `frameOffset` local.
  This restores the original delayed Y-origin load, bottom-coordinate
  register and corner-pointer register. All 144 bytes now match; only the
  routine's 26 differing code bytes change, with every other executable
  byte and all 3,797 symbol/address pairs unchanged. Removing the offset
  local instead leaves ten differing bytes and changes one byte in the
  previously exact `GetTransformedShapeBounds`; retain the local used by
  the range check and frame-table access. Coordinate declaration orders,
  addition order, and alternate bottom-coordinate types add no improvement.
  Normalized exact functions rise to 1,472 of 1,550; strict file matching
  reaches 515,383 of 518,656 (99.3689%), leaving 3,273 differing bytes.
  Native and SDL builds, order/sorting/address audits and full verification
  pass. Both value checks cover all 1,550 functions with zero mismatches
  or skips; existing call/global-access diagnostics remain 11/56. The
  executable retains SHA-256
  `3b08624964e762b903ec2966881b5874f2c495bbe8f6d5df299f2783b6218c1f`
  through verification.
- Further allocator, seek, launch and prompt trials are discarded. Signed
  guarded-allocation sizes lose matches; unsigned-long sizes and counters
  do not improve them. Permuting the allocation-counter externs yields no
  gain. Alternate seek-offset and result integer types leave its register
  swap. Capturing the launch object's promoted index, or using its short
  snapshot at the late calls, loses file matches despite some variants
  recovering the original frame size. Separate prompt-cursor X captures
  also lose matches. Copying individual release-weapon position fields
  before translation leaves the extra saved register. Alternate input-key
  switch exits leave the differing block order or worsen it. None of these
  variants is retained.
- `dot_product` accumulates the Y product, then the Z product, and finally
  adds the X product. Separate full expressions preserve the original call
  order, which the single addition expression reverses for Y and Z.
  Inline the single-use `PlanarMagnitude` result into `DivideFixed` in
  `rectangular_to_spherical`, retaining its saved Z coordinate across the
  call. That routine's bytes stay exact, and removing its temporary preserves
  the later geometry routines' compiler context after adding the dot-product
  accumulator. The accumulator alone still grows `rotate_about_k` by sixteen
  bytes and disturbs `rotate_about_j`; that isolated change is not usable.
  The combined change matches all eighty dot-product bytes and changes only
  its four coordinate offsets in the executable. Every other byte and all
  3,797 symbol/address pairs remain unchanged. Normalized exact functions
  rise to 1,473 of 1,550; strict matching reaches 515,387 of 518,656
  (99.3697%), leaving 3,269 differing bytes.
  Native and SDL builds, sorting/address audits and full verification pass.
  Both value checks cover all 1,550 functions without mismatches or skips;
  existing call/global-access diagnostics remain 11/56. Verification preserves
  the executable's SHA-256:
  `5a63695cda0cc40e3c91741eee0b5247ac4020f07be7939f11da30764a8bc7d6`.
- Further sprite-restoration and carrier trials are discarded. Moving the
  run-end, minimum-X or copy-length declarations, splitting the run-end
  arithmetic, register hints, reversed clipping comparisons and a long
  run-end local do not improve restoration. Scoping the run end to the row
  loop loses five matching bytes.
  The carrier's object-word spill is later read as a dword by its sound
  calls without an intervening full-width initialization. A separate
  uninitialized sound-handle local reproduces that kind of stack reuse,
  but does not recover the routine's layout. With direct parameter indexing
  it restores the 32-byte frame while shortening the function by sixteen
  bytes; initialization orders do not repair the complete result. This is
  still an unresolved source hypothesis, not a retained uninitialized local.
