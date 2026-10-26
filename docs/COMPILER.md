# Compiler contract

Reference target: `WC1.EXE`, MD5 `b20a68b7e45f837e59f7e31bab2e2020`, built
1996-09-24 with image base `0x00400000`.

The linker stamp, pre-VC5 image layout and startup, and static CRT identify
Microsoft Visual C++ 4.20. The exact executable configuration lives in the
Makefile; change it only when `binary-comp` demonstrates an improvement across
the affected compilation unit.

## Profiles

| Sources | Language and flags |
| --- | --- |
| Game core | C, `/MTd /Og /Oi /Ot /Oy /Ob1 /Gs` |
| `src/ix/` library members | C++, `/MTd /Od /Oi` |
| `src/ix/ixlog.cpp` | C++, core profile |
| `src/sound.c` | Core profile compiled as C++ with `/TP` |
| `src/debug.cpp` | C++, core profile without `/GX` |
| `src/pilot.cpp` | Core profile plus target-specific `/GX` |

Evidence that constrains these settings:

- Debug-heap strings, `_CrtCheckMemory`, `_mtinit`, and `tidtable.c` prove the
  static debug multithreaded CRT (`/MTd`).
- Core functions normally omit EBP and schedule registers; `ix` library
  functions use EBP, stack temporaries, and a shared epilogue.
- The separately linked `ixlog.cpp` matches the core profile. Its variadic
  logger retains EBP but uses short branches, scheduled argument loads and
  `POP EBP`, rather than the library profile's near branches and `LEAVE`.
- Duplicate literals at `0x0046B6E0` and `0x0046B71C` prove string pooling was
  disabled; do not add `/Gf` or deduplicate literals in source.
- Core operand-size prefixes reflect 16-bit DOS-era types. Use `short` where
  the original uses 16-bit operations; this is not a compiler flag.
- The console allocation wrapper proves `pilot.cpp` needs construction cleanup.
  The training-simulator routines following the console wrappers are in
  `trainsim.c` and use the C core profile. Other units must not gain C++
  exception handling.
- `src/sound.c` directly calls `ix` member functions and matches only as C++.
  Its wave and active-sound lists share initialized state with those calls.
  The following launch, installation, and damage-display routines use C.
  Launch's initialized tables follow sound's literal pool, proving that
  sound's C++ unit ends before `DrawLaunchDoorFrame`.
- The global `IxStream` and `IxVoice` arrays require non-aggregate record
  declarations to reproduce VC++ 4.20's empty `$E2`/`$E1` initializer pairs
  and `.CRT$XCU` registrations. Private fields and friend declarations for
  their existing users produce both pairs byte for byte under `/Od /Oi`.
  No explicit constructor, exception handling, or extra compiler flag is needed.

## Backend diagnostics

The installed `C2.EXE` has SHA-256
`674fb9e410481378c6980c3f21914e513128c85001e8337aca73b587b6273ae9`.
Its option table contains `-norecy`, `-nosched` and `-nocombine`; CL forwards
them through `/d2norecy`, `/d2nosched` and `/d2nocombine`. Temporary
`CFLAGS_CORE` overrides allow individual units to be rebuilt for diagnosis.
These controls are not part of the retained compiler profile.

In this backend image, the recycling control word is at `0x0047A98C`,
initially one, and is tested at `0x00422F27`. The scheduling control is at
`0x0047A994`, also initially one, and is tested at `0x00401480`. These
addresses come from the option table and disassembly of the compiler itself.

Binary-comp probes on the 32-byte-frame carrier candidate distinguish two
remaining differences. Disabling scheduling leaves the escape reset after
the first two indexed word stores, in source order, but still doubles the
index with `ADD` instead of the original `LEA`. Disabling recycling also
retains the destructive doubling and changes other registers and scheduling.
Disabling combining leaves the complete carrier comparison unchanged.
The target-list sorter's register exchange likewise survives disabling
recycling. These probes give no reason to change the production flags; the
carrier's index calculation still needs a source-level explanation.

The backend also exposes the expression hash used to discover global
register candidates. In this same image, `0x0041DA38` calls `0x0041DC26`,
masks the result with `0x1F`, and searches one of 32 linked lists at
`0x004861C0`. New entries go at the head of their bucket. `0x0041DC26`
combines the root's hash with those of its immediate operands, using
`0x0041DB5B` for each contribution. The latter reads the node opcode and
type, handles integer constants directly, and incorporates a named
symbol's key at record offset `+0x24`; opcode `0x26` has a separate
address/value path. These details are visible in the compiler's assembly.

`0x0041F1EA` walks the buckets in ascending order when numbering candidate
live ranges. The use accounting in `0x0041DED6` includes a block weight of
four times the signed word at block offset `+0x46`, plus one. An assertion
in that routine refers to `d:\backend\src\P2\nglobreg.c`. This identifies
candidate discovery and numbering; physical-register selection is a later
step. The hash explains why the measured modulo-32 symbol windows are
useful while still being incomplete predictors for computed expressions.
An isolated analysis of the unchanged compiler supplied these addresses;
the compiler executable and production flags remain unchanged.

CL's `/BkPREFIX` option retains the frontend's intermediate files. For
example, the normal core profile plus `/Bkout\c2trace\ship` leaves
`shipin`, `shipgl`, `shipex` and `shipsy` under an existing `out/c2trace`
directory, while still producing the requested object and assembly listing.
These files supply a repeatable backend input for investigating allocation
and scheduling. They are generated diagnostics, not source inputs to keep
in version control.

CL invokes `C2.EXE` with its options in the `MSC_CMD_FLAGS` environment
variable. This invocation replays the saved `ship.c` intermediates using
the same backend options observed in the normal core compilation:

```sh
env MSC_CMD_FLAGS='-ef compilers\msvc420\bin\c23.err -il out\c2trace\ship -f src/ship.c -W 1 -G4 -dos -Fdvc40.idb -MTd -Gs4096 -Foout\c2trace\ship.obj -FAa -Fa out\c2trace\ship.asm' \
    ./wibo compilers/msvc420/bin/C2.EXE
```

The optimization settings are carried in the frontend intermediates;
their absence from this backend command is not a change to the compiler
profile. Binary-comp confirms that substituting the replayed ship object
and relinking reproduces all 518,656 bytes of the previously verified
executable. The object itself differs only in its COFF timestamp at offset
four. A host debugger is unnecessary for retaining and replaying this input.

### Expression IL

The `ex` intermediate is a postfix stream of one-byte opcodes. The first
record (opcode `0x5B`) holds the file offset of the first expression; the
bytes before it are zero. `FUN_0041328C` builds a node for each opcode and
dispatches on a per-opcode operand class, a byte table at `0x00480300`
(jump table `0x00413560`). The classes read, in order:

| Class | Opcodes | Operands |
| --- | --- | --- |
| 0, 16 | arithmetic, comparisons, statement ends | none |
| 1 | loads (`0x30`), assignment (`0x32`), arguments (`0x55`), ... | type |
| 2 | symbol (`0x26`), labels and branches (`0x29`, `0x38`, `0x39`, `0x3A`) | symbol |
| 5 | conversion (`0x2C`), `0x34` | type, byte |
| 6 | constant (`0x33`) | type, value |
| 7 | switch dispatch (`0x3C`) | type, symbol |
| 8 | case label (`0x3D`) | symbol |
| 9 | call (`0x3E`, `0x7E`) | type, byte |
| 12 | metadata (`0x4F`) | 16-bit subtype, then the subtype's format |
| 13 | `0x50`-`0x54`, `0x5B` | 32-bit value |

A type is one byte, or two (`((b & 0x7F) << 8) | next`) when the first has
bit 7 set; a structure type of size zero is followed by its 32-bit size. A
16-bit value is one signed byte, or `0x80` and two raw bytes
(`FUN_00402106`); a 32-bit value is the same with four bytes
(`FUN_004021A2`). Symbol numbers are two raw bytes (`FUN_00403244`).
Metadata subtypes take their operand formats from the string table at
`0x00480700`: subtype 1 is a line number, 2 a symbol.

Operand counts come from the flag word table at `0x004803B8` (`flags & 3`:
0 binary, 1 unary, 2 leaf, 3 an argument-list form closed by `0x4C`).
Calls, intrinsic block operations (`0x3F`, `0x40`), switch dispatches and
function definitions (`0x46`) open such a list. A function definition's
`0x2D` children are its parameters. Statement roots are `0x4B` (expression
statement), `0x38`/`0x39` (branch if false/true), `0x41` (return) and `0x3B`
(switch).

The symbol numbers in this stream are the keys that C2 hashes (record
offset `+0x24`, read from the `sy` file). They are the C1 symbol counter
that numbers `/Fa` labels. C1 allocates a function's exit label immediately
after its last parameter, before its locals; `bin/symbolIndex.py param`
accounts for it.

### Operand canonicalization

`FUN_004193F8` canonicalizes an expression tree, repeating until nothing
changes. It is called after the tree is read and again after later
optimizations, so the last pass before code generation decides the
operand order. Its walker `FUN_0040B4CE` visits a node's right child, then
its left child, then computes the node's signature with `FUN_0040A609`.
For a commutative operator (flag `0x08`, which includes the comparisons
`0x1F`-`0x24`), `FUN_0040EBE2` exchanges the operands when the left
signature is smaller, so the larger signature ends up on the left; a
reversible operator (flag `0x10`) is replaced by its mirror image. An
associative operator (flag `0x04`: `ADD`, `MUL`, `AND`, `OR`, `XOR`) whose
left child has the same opcode is first reassociated by `FUN_0040EB6D`
when the left child's smaller operand sorts below the right operand,
giving a chain in descending signature order. Equal signatures keep the
existing order.

A signature is a 16-bit value, a weight in bits 4 and above plus a 4-bit
hash:

- symbol leaf (`0x26`): `0x10 | h(key)` with
  `h(k) = (((((k >> 8) - k) mod 2^32) >> 4) - (k >> 8) + k) & 0xF`; a
  temporary (symbol kind `0x18`) gives `0x10`. The offset folded into a
  symbol node does not contribute.
- integer constant: a 4-bit hash of the value with weight zero, so
  constants sort right.
- load (`0x30`): `((c - 0x2D) & 0xF) | ((c + 0x10) & 0xFFF0)` for child
  signature `c`; a scalar variable read is therefore
  `0x20 | ((h(key) + 3) & 0xF)`.
- other unary nodes: `(((op >> 4) - op + c) & 0xF) | ((c + 0x20) & 0xFFF0)`.
- binary nodes: `(((op >> 4) - op + l + r) & 0xF) | ((l + r + 0x20) & 0xFFF0)`;
  the comma operator (`0x44`) takes its right operand's signature.

Because the hash of consecutive keys rises by one except where `k - (k >> 8)`
crosses a multiple of 16, two adjacent symbols compare in index order except
at those duplicate points and where the hash wraps; this is the source of
the period-16 and period-17 comparison windows measured earlier, and of
the extra slip every 256 keys. The first operand of the emitted `CMP` or
arithmetic instruction is the left operand.

This model reproduces all 32 key positions of a two-local comparison
calibrated with tiny synthetic files. It also predicts the earlier
measurement that `IsPointInNavMapLabel` becomes exact when five symbols
are hidden: CSE replaces the label coordinates with temporaries, and the
parameter reads sort below them only for shifts of -6 to -4. Shifting
`perform_maneuver`'s symbols by +10, as predicted, reverses its final
comparison to the original operand order. The emitted instruction then
loads the stack operand instead of the global, so that routine needs
another change as well.

Intermediate trees can be inspected without changing the compiler used for
the build. A copy of `C2.EXE` whose call to `FUN_0040A609` at `0x0040B516`
is redirected through a short routine in the `.text` slack prints each
node's address, opcode, signature, key and child pointers with the
imported `printf`. C2 redirects standard output into the `/Fa` listing, so
each function's records precede its `PROC` line. For
`check_for_lost_control`, the log shows the three rate fields as
`LD(ADD(ADD(MUL(type, 135), aObjectTypeData), offset))` with signatures
`0xC3`, `0xC1` and `0xBF` for offsets 36, 38 and 40, so the sum is
emitted in ascending offset order. Later passes fold the offsets into the
symbol and the terms tie. For terms of this shape, no base signature
produces the original descending order, so that routine needs a different
expression tree, not a different symbol index.

### Register colouring

`0x0041F1EA` numbers live ranges (webs) from 32 upward, walking the 32
candidate buckets in ascending order and taking each bucket's newest
candidate first. A variable's bucket is its key plus offset, masked with
`0x1F`; compiler temporaries have fixed buckets. The colourer keeps its nodes
in 96 lists at `0x00486F20` (precoloured nodes, `0x4000 | web`,
`0x8000 | n`), indexed by web number modulo 32. `FUN_0041CAD1` repeatedly
removes the colourable node of highest degree (ties: the first in its
prepended traversal), or else the node of lowest priority. `FUN_0041CC26`
then colours the nodes in reverse removal order with the preference table at
`0x0047B370`: EAX, ECX, EDX, ESI, EDI, EBX, EBP. Byte-sized webs get
precoloured edges to ESI, EDI and EBP. Replaying this on the logged
interference graphs reproduces all 73 colouring rounds of `mission.c`.
Statement-local scratch registers (for example `AllocateGuardedMemory`'s
EDX, or the two-dword copy in `PromptInsertNumberedDisk`) are chosen later
by the code generator and are not covered by this model.

### Stack frame layout

With `/Og`, `FUN_00421A14` lays out the locals after register allocation.
It scans every instruction of every block, second operand first, and calls
`FUN_004326D8` for each memory operand that names an automatic variable or a
temporary on the frame. That routine keeps one list sorted by size
ascending. Within a size, entries are sorted by reference count descending,
and an entry whose count grows moves ahead of entries with an equal count.
`FUN_00432E9F` then packs the list in order: each variable joins the first
slot none of whose members interferes with it. The slots are merge-sorted
(stable) by the comparator at `0x00436F7D`. When both slots are at least
four bytes, the one with the higher density (`weight * 1000 / size`) comes
first, then the larger one. Otherwise the smaller slot comes first.
`FUN_0040E19E` hands out the slots from the frame top down, aligning
two-byte slots to two and larger ones to four. A flag passed in by the
allocation driver `FUN_0041B4D6` reverses the sorted order and places each
member at its slot's base rather than its top. The frames checked so far
behave as if it is set, so `ReadTextInput`'s eight offsets follow from its
reference counts, as do `DecompressPacketSection`'s three dwords.
Predicting from the final listing reproduces 31 of the 83 temporary-free
frames with at least two locals; the rest differ because some sizes are
not recovered from the `sy` file and because the instruction order and
counts seen at layout time are not exactly those of the listing. Symbol keys do not enter this layout at all;
a slot-order difference therefore needs a different reference pattern,
size or lifetime, not a symbol shift. The SEH and C++ EH records
(`__$SEHRec`, `__$EHRec`, via `0x00440268`) are allocated before every
local.

### Planning symbol shifts

The logs of an instrumented `C2.EXE` copy (signatures, candidates, webs,
interference graphs and colours) let the operand-order and colouring models
predict, offline, each function's decisions when its own keys move by
`s`. For each differing function, the change set that turns our code into
the original is read off the aligned diff: a register rotation must equal
the model's predicted web moves. All shifts producing exactly that set are
its target. Every exact function in the unit gets a tolerance window of
shifts that change nothing. Additions can only raise the shift of all later
functions, so a fewest-step nondecreasing profile through the windows and
targets says where symbols must be added. Before writing any source, one
scratch compile with placeholder `extern`s at those points confirms the
prediction. The model was wrong twice: `DrawNavSquareOutline` at +12 also
reverses its additions, and `perform_maneuver` at +30 keeps its comparison.
The confirmed profiles are realized with local prototypes of external
callees first used after the insertion point; a redundant prototype costs
one symbol per named parameter. `cockpt.c` (+17 before `check_sighting`,
+15 before `print_message_text`) and `main.c` (+17 before
`SelectPreviousExternalViewObject`) were fixed this way.

Some units constrain insertion-only profiles. In `logic.c`,
`scan_for_enemy` tolerates only 0, 17, 34 or 96 and
`UpdateSceneAnimationObject`, later in the unit, only 0, -32, -64 or -96,
so such a profile leaves the interval between them at 0. That excludes the
targets of `attacker_in_range` and `hostile_sphere`, and any route to
`find_ship_index` or `get_front_spot` needs four to six steps of 64 or more
symbols.

Global-key changes combined with balanced local-count changes can escape
that constraint. In `evaluate_damage`, neither an armor-array key change nor
a local-pointer key change alone produces the original armor-term order.
Swapping `aasShipArmor` with `aeShipTacticArray` only for `logic.c` moves
the armor key from 23,517 to 23,529. Capturing the object type and class
before declaring `typeData` moves its key from 27,775 to 27,777. Together
they recover rear, front, left, right evaluation and the exact 208-byte
span. Removing the redundant logical-file local in `LoadShapeSet` and
reusing the backdrop pointer for cockpit case 7 balance those two added
symbols. Declaring the cockpit mode before its result preserves the
comparison order. All other executable bytes remain unchanged; these are
measured matching declarations, not recovered original source spelling.

Local permutations can also recover a register exchange without shifting
later functions. For `auto_pilot_sequence`, the existing interference graph
predicts six orders of its final six short locals that exchange only the
destination distance and hoisted ship-class constant registers. The selected
order is destination distance, other ship, ship, initial distance, cruise
speed, nearest-ship distance. It moves `destinationRange` from key 27,277
to 27,274 and `ship` from 27,274 to 27,276, recovering four encodings.
Moving `nYourWingman` eleven declarations later only for `auto.c`, from key
23,288 to 23,299, reverses the remaining ship/wingman comparison. The
compiler and `binary-comp` confirm both predictions: all 1,472 bytes match,
with no change anywhere else in the executable. Initializations, arithmetic
and control flow are unchanged; the declaration order is a matching choice.

`SelectNavObjectiveAtPoint` shows how expression ordering can also repair
stack slots without changing types or adding storage. Of the permutations
of its seven short locals, the model finds 35 that preserve register
colouring while reversing the coordinate-sum and saved-point comparisons.
The selected order is `mapY`, `oldNavPoint`, `x`, `pathIndex`, `mapX`, `y`,
`selected`. Computing the X distance before Y restores both coordinate
argument offsets as well as the original arithmetic order. The final test
also becomes `CMP DI,[saved point]`. All 272 bytes then match. This is an
indirect stack-layout effect through the generated operations; symbol keys
do not themselves order the stack slots.

`build_your_target_list` needs its parameter key moved from 27,990 to
28,005 to recover ECX for the output pointer and ESI for the scaled
enum-array index. The compiler model predicts the exchange; `binary-comp`
confirms the complete 160-byte span. In the preceding cockpit restore
routine, two used call-pointer captures with eleven named parameters and
one scoped X-coordinate capture supply fourteen symbols. A scoped HUD-text
capture in `SetHudTextColour` supplies the fifteenth and also restores the
target bracket's marker-colour comparison order. CL folds the call pointers
to the same direct calls and preserves both helper routines byte for byte.
Declaring `cycle_onscreen_targets`' index before its saved target preserves
its array-address operand order under the new context. These declarations
reproduce measured compiler contexts, not recovered original source text.

`flag_reached` demonstrates a balanced change confined to one function's
context. Three used captures in `escorting_a_ship` move its objective
parameter key from 27,645 to 27,648. The model predicts the ESI/EDI exchange;
the compiler also recovers the original prologue and load schedule, making
the complete 416-byte span exact. Omitting three parameter names from the
existing redundant `ComputeVectorDelta` prototype immediately afterwards
restores the following functions' symbol keys. The public prototype retains
its parameter names, and the escort helper's instructions remain unchanged.

`PollJoystickButtonEvents` becomes exact with 25 more symbols
before it. Scoped input, button and timing snapshots in
`UpdateInputDeviceTransitions`, together with correctly typed captures of
its two sampling calls, now supply that context. The 22 used locals and
three named call-pointer parameters move the poller's `doubleClick` key
from 27,256 to 27,281. They compile away, preserving the transition routine
and both other functions in the unit; the poller's first event block gains
the original AX flag, ECX index and argument-push schedule.
`UnionRectBounds` needs +12 (or -5) and has no callees.
Its +12 context is now supplied by scoped sample, center and scale captures
in the four axis-normalization branches of `ReadCalibratedJoystick`.
Those captures compile away without changing either joystick routine,
while the rectangle helper becomes byte-exact.

## Hand-written assembly

Three regions were written in assembly, through different tools:

- The LZO decoder (`0x004614C0`) is an `/Od` function around an `__asm` block.
  Its padding (`nop`, `lea ecx,[ecx]`) is what CL 4.20's inline assembler emits
  for `ALIGN 4`, and its forward jumps are near, as inline assembly emits them
  in `/Od` code.
- The raster library (`0x00439C40`–`0x0043F635`, data
  `0x0046E69C`–`0x0047008B`) was assembled. All 98 of its shifts and
  rotates by one use the `D1`/`D0` encodings, which neither CL 4.20 nor its
  inline assembler emits (both write `C1`/`C0` with an immediate 1), and it
  keeps data tables in `.text`, which inline assembly cannot declare. Its frames open with
  MASM's `add esp, -N`, every jump has the shortest encoding, as MASM 6
  sizes them, and both segments start on 4-byte boundaries, the alignment
  `.model flat` gives `_TEXT` and `_DATA`; no function in it is padded.
  `src/raster.asm` is that source, extracted from the retail executable by
  `bin/extractRasterLibrary.py`, which checks that UASM (a MASM-compatible
  assembler, built from the `compilers/uasm` submodule) reproduces every
  byte and relocation. Regenerate it with the script rather than editing it.
- The ix sample mixer (`0x00446F74`–`0x004471FE`) was also assembled.
  Its two `shl dx, 1` instructions use `66 D1 E2`, and its branches have
  MASM's shortest encodings. Three INT3 bytes after the preceding C++
  function align this object to four bytes; one follows its final `RET`
  before the next C++ object. `src/ix/dspmix.asm` preserves this boundary
  and is assembled with UASM. Compiling the same instructions as an `/Od`
  inline-assembly block emitted longer jumps and immediate-one shifts.

`FillRasterBytes` (`0x00439C10`), just before the library, is compiled C: its
body is CL's inline `memset`, and the `mov edi, edi` before and after it is
CL's padding. In optimized code CL ends every function with NOPs up to a
16-byte boundary, so all 1,291 core functions start 16-byte aligned, and the
only INT3 fill LINK adds in the core is the run after the raster library,
which ends unaligned.

Linking uses `/SUBSYSTEM:WINDOWS /ALIGN:4096` without `/ENTRY`, so LINK
supplies `WinMainCRTStartup` itself. The command line lists the core objects
(with `raster.obj` between `screens.obj` and `killbrd.obj`), `ixlog.obj`,
then `kernel32`, `user32`, `gdi32`, `advapi32`, `winmm`, the five
`ix` libraries, `dsound` and `ddraw`; LIBCMTD and OLDNAMES follow as default
libraries. Base relocations remain enabled. `docs/ORDER.md` records the
evidence for each choice.
