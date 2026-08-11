# Missing function backlog

Audit date: 2026-08-11.

This is the source-reconstruction backlog obtained by comparing the live Ghidra
program `WC1%2FWC1.EXE` with every `/* Function start: 0x... */` annotation in
`src/**/*.c` and `src/**/*.cpp`. It contains **64 functions**: **62 ordinary
candidates** and **2 compiler thunks** which must not be reproduced manually.

The proposed compilation units follow the current source adjacency and
`docs/ORDER.md`; boundaries explicitly described as provisional there remain
provisional here. In particular, the contiguous raster run remains assigned to
`src/screens.c` through `0x0043F5A9`, because the first function currently in
`src/killbrd.c` is at `0x0043F640`.

The Mac name column was checked against all 881 MacsBug symbols in
`../releases/mac/extracted/symbols.json`, including the established ordered
cross-release mappings in `docs/LABELS.md`. There are **no safe exact Mac-name
matches in this remaining backlog**. A dash therefore means “no defensible Mac
counterpart”, not “the original function had no name”. Super Wing Commander is
a later code base, so positional similarity alone is not enough to assign a
name.

Ghidra status meanings:

- **verified** — an exact live function entry and a non-empty listing were
  checked at the stated address.
- **exported** — the corresponding `code-full/FUN_<address>.disassembled.txt`
  file was generated from the original PE and checked against the live Ghidra
  listing.
- **recreated** — the old Ghidra object contained only a leading `0x66` prefix;
  it was rebuilt from the true instruction boundary and its metadata restored.
- **new live discovery** — valid function absent from the stale
  `out/funcs_v5.json` inventory.

| Address | Current Ghidra/inventory name | Mac name | Proposed compilation unit | Ghidra status |
|---|---|---|---|---|
| `0x00404610` | `HelperOf4219C0B` | — | `src/cmpgn.c` | verified |
| `0x00408650` | `BlitTbl005a86d0Fn8650` | — | `src/brains.c` | verified |
| `0x004094E0` | `RunAnimationDemoLoop` | — | `src/brains.c` | verified |
| `0x0040CB20` | `ThunkForwarder40CB20` | — | `src/brains.c` | verified; exported; compiler thunk—do not hand-write |
| `0x004176C0` | `DrawG0046905cFn76C0` | — | `src/cockpt.c` | verified |
| `0x004219C0` | `LoadGamePaletteFile` | — | `src/logic.c` | verified |
| `0x00421F50` | `SetNavFn1F50` | — | `src/logic.c` | verified |
| `0x00425770` | `ShowMeanwhileTransition` | — | `src/pilot.cpp` | verified |
| `0x004259B0` | `PromptForAnswerText` | — | `src/pilot.cpp` | verified; exported |
| `0x00426000` | `ScanTbl0059ca74Fn6000` | — | `src/pilot.cpp` | verified |
| `0x0042B680` | `PlaySfxWaveByIndex` | — | `src/sound.c` | verified |
| `0x0042BC00` | `BlitTbl0046a5aeFnBC00` | — | `src/sound.c` | verified |
| `0x0042C420` | `WaitForAnyKeyPrompt` | — | `src/sound.c` | verified |
| `0x0042C510` | `LoopUiFnC510` | — | `src/sound.c` | verified |
| `0x0042F740` | `ScanTbl005a6540FnF740` | — | `src/screen.c` | verified |
| `0x0042F890` | `LoopNavFnF890` | — | `src/screen.c` | verified; exported |
| `0x0042FB40` | `SetTbl0059cd90FnFB40` | — | `src/screen.c` | verified |
| `0x0042FC00` | `BlitTbl0059c950FnFC00` | — | `src/screen.c` | verified |
| `0x00430150` | `BlitTbl0059de40Fn0150` | — | `src/screen.c` | verified |
| `0x004304F0` | `BlitTbl005a6900Fn04F0` | — | `src/screen.c` | verified |
| `0x00431900` | `TextTbl0059df80Fn1900` | — | `src/screen.c` | verified; exported |
| `0x00431A10` | `LoadJoystickCalibrationFile` | — | `src/screen.c` | verified |
| `0x00434D10` | `_rand` | — | `src/mathfp.c` | verified; compiler thunk—do not hand-write |
| `0x00436F50` | `LoadTbl0059ca58Fn6F50` | — | `src/screens.c` | verified |
| `0x00439C0E` | `FUN_00439c0e` | — | `src/screens.c` | new live discovery; verified; exported |
| `0x00439C3E` | `LoadMcgaDll` | — | `src/screens.c` | verified; exported |
| `0x00439C69` | `LoopG0046e69cFn9C69` | — | `src/screens.c` | verified |
| `0x0043C015` | `LoopLocalFnC015` | — | `src/screens.c` | verified |
| `0x0043C18D` | `LoopG0046e6e5FnC18D` | — | `src/screens.c` | verified |
| `0x0043C410` | `LoopG0046ff2cFnC410` | — | `src/screens.c` | verified |
| `0x0043C4A2` | `LoopG0046e6e9FnC4A2` | — | `src/screens.c` | verified |
| `0x0043C62B` | `ScanTbl0046e6edFnC62B` | — | `src/screens.c` | verified |
| `0x0043CC83` | `DoLocalFnCC83` | — | `src/screens.c` | verified |
| `0x0043D1C1` | `LoopLocalFnD1C1` | — | `src/screens.c` | verified |
| `0x0043E478` | `DoLocalFnE478` | — | `src/screens.c` | verified |
| `0x0043E48B` | `DoLocalFnE48B` | — | `src/screens.c` | verified |
| `0x0043E4AB` | `LoopLocalFnE4AB` | — | `src/screens.c` | verified |
| `0x0043E63E` | `LoopLocalFnE63E` | — | `src/screens.c` | verified |
| `0x0043E675` | `LoopLocalFnE675` | — | `src/screens.c` | verified |
| `0x0043E784` | `LoopLocalFnE784` | — | `src/screens.c` | verified |
| `0x0043E7C6` | `LoopG0046f915FnE7C6` | — | `src/screens.c` | verified |
| `0x0043E98D` | `LoopG0043e77cFnE98D` | — | `src/screens.c` | verified |
| `0x0043E9BE` | `GetG0043e778FnE9BE` | — | `src/screens.c` | verified |
| `0x0043E9EB` | `LoopG0046ec15FnE9EB` | — | `src/screens.c` | verified |
| `0x0043EA6D` | `LoopLocalFnEA6D` | — | `src/screens.c` | verified |
| `0x0043EA98` | `DoLocalFnEA98` | — | `src/screens.c` | verified |
| `0x0043EABF` | `LoopLocalFnEABF` | — | `src/screens.c` | verified |
| `0x0043EB07` | `DoLocalFnEB07` | — | `src/screens.c` | verified |
| `0x0043EB20` | `DoTbl0046ff15FnEB20` | — | `src/screens.c` | verified |
| `0x0043EB66` | `DoLocalFnEB66` | — | `src/screens.c` | verified |
| `0x0043EBAC` | `GetTbl0046ec15FnEBAC` | — | `src/screens.c` | verified |
| `0x0043EC29` | `LoopG0046ff28FnEC29` | — | `src/screens.c` | verified |
| `0x0043EE42` | `LoopLocalFnEE42` | — | `src/screens.c` | verified |
| `0x0043EEA3` | `DoLocalFnEEA3` | — | `src/screens.c` | verified |
| `0x0043EEDB` | `DoLocalFnEEDB` | — | `src/screens.c` | verified |
| `0x0043EEFD` | `DoLocalFnEEFD` | — | `src/screens.c` | verified |
| `0x0043EF7E` | `LoopLocalFnEF7E` | — | `src/screens.c` | verified |
| `0x0043EFC9` | `LoopLocalFnEFC9` | — | `src/screens.c` | verified |
| `0x0043F011` | `LoopLocalFnF011` | — | `src/screens.c` | verified |
| `0x0043F05B` | `DoLocalFnF05B` | — | `src/screens.c` | verified |
| `0x0043F06E` | `LoopLocalFnF06E` | — | `src/screens.c` | verified |
| `0x0043F0D0` | `LoopLocalFnF0D0` | — | `src/screens.c` | verified |
| `0x0043F425` | `ScanTbl0046f915FnF425` | — | `src/screens.c` | verified |
| `0x0043F5A9` | `ScanTbl0046f915FnF5A9` | — | `src/screens.c` | verified |

## Mac ordering evidence retained for later naming

- `0x00404610` is a Win32-private loader between the exact survivors
  `auto_pilot_sequence` and `ejection_sequence`; neither Mac name applies to it.
- `0x004176C0` is a Win32 split helper between the exact Mac-derived
  `explosion_draw` and `cockpit_explosion` functions in the `cockpt` unit.
- `0x00434D10` is in the historical `rand` neighborhood but is a compiler
  jump thunk to the CRT, not a Mac-named game routine.
- The large `0x00439C0E`–`0x0043F5A9` raster block has no safe mapping to the
  Mac renderer; that renderer was substantially rewritten.

## Ghidra creation and repair audit

- `FrameTimerCallback` at `0x0042AFB0` was genuinely absent from the live
  Ghidra function table even though it is already implemented in
  `src/hudmsg.c`. It was force-created with body
  `0x0042AFB0`–`0x0042AFBC`, the five-argument `__stdcall` callback prototype,
  and the expected game-core tags. Its two instructions are the write to
  `0x0059AB3C` followed by `RET 0x14`.
- `LoopNavFnE950` at `0x0040E950` and `CalcRectangleArea` at `0x0042E050`
  existed only as malformed one-byte function objects over an operand-size
  prefix. They were recreated as bodies `0x0040E950`–`0x0040E9D6` and
  `0x0042E050`–`0x0042E085`, respectively. Their prior prototypes, tags, and
  plate comment were restored.
- Every source annotation now resolves to an exact live Ghidra function entry.
  Every one of the 64 backlog rows above also resolves to an exact entry and a
  non-empty listing. The Ghidra program was saved after verification.

## Export and implementation progress

- All 28 assembly snippets that were absent at the start of this audit now
  exist in `code-full`. Each export has the same instruction count and return
  form as its exact live Ghidra function. Existing exports were not rewritten.
- Twenty-five confirmed `wc-developer` functions were removed from the backlog
  across five reconstruction tranches after binary-comp comparison:

  | Address | Implemented name | Compilation unit | Similarity |
  |---|---|---|---:|
  | `0x0041D650` | `SortSignedByteValuesAscending` | `src/disk.c` | 95.35% |
  | `0x00425730` | `WaitForKeyExceptXOrF12` | `src/pilot.cpp` | 100.00% |
  | `0x0042E050` | `CalcRectangleArea` | `src/music.c` | 100.00% |
  | `0x00432050` | `ReportSpaceFlightMaxFps` | `src/dib.c` | 100.00% |
  | `0x00418840` | `MakeRandomNormalizedVector` | `src/geom.c` | 100.00% |
  | `0x00418980` | `ConvertShortVectorToFixedVector` | `src/geom.c` | 100.00% |
  | `0x004258D0` | `ApplyAnswerTextCipher` | `src/pilot.cpp` | 100.00% |
  | `0x00425910` | `LoadAnswerPromptAndResponse` | `src/pilot.cpp` | 89.83% |
  | `0x00441BF0` | `TriangleRasterizerHook` | `src/gr.c` | 100.00% |
  | `0x00442350` | `SignalAudioMixerWakeEvent` | `src/gr.c` | 100.00% |
  | `0x004185C0` | `IsPairEqualityDifferentFromFlag` | `src/geom.c` | 100.00% |
  | `0x004198A0` | `MoveObjectAlongDirection` | `src/geom.c` | 100.00% |
  | `0x004265A0` | `ReadRequiredPilotField` | `src/pilot.cpp` | 100.00% |
  | `0x004354A0` | `CopyFarString` | `src/strdos.c` | 100.00% |
  | `0x00435510` | `DosMemset` | `src/strdos.c` | 100.00% |
  | `0x0040E890` | `MergeAdjacentNearHeapBlocks` | `src/nav.c` | 100.00% |
  | `0x0040E900` | `ReleaseNearHeapBlock` | `src/nav.c` | 96.30% |
  | `0x0040E950` | `PurgeNearHeapBlocks` | `src/nav.c` | 100.00% |
  | `0x0040E9E0` | `InitializeNearHeap` | `src/nav.c` | 100.00% |
  | `0x0041A130` | `IsPointWithinEyeViewCone` | `src/geom.c` | 98.00% |
  | `0x00401000` | `SaveGamePalette` | `src/winmain.c` | 100.00% |
  | `0x0040EB70` | `AllocateNearHeapBlockFromEnd` | `src/nav.c` | 91.54% |
  | `0x0040ED30` | `AllocateNearHeapBlockByFlags` | `src/nav.c` | 95.04% |
  | `0x0042B160` | `InitializeAudioSystem` | `src/pload.c` | 100.00% |
  | `0x0042B810` | `RegistryQueryValue` | `src/sound.c` | 100.00% |

- Ghidra was synchronized with the reconstructed prototypes and behavior-based
  names, then saved after each tranche.
- `ThunkForwarder40CB20` and `_rand` remain explicitly excluded as
  compiler/linker-generated jump thunks.

The compiler configuration and original executable were not modified, and the
game was not run. `make compare-func` and `make verify` only rebuilt and linked
the reconstruction for comparison and repository audits.
