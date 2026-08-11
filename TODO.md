# Missing function backlog

Audit date: 2026-08-11.

This is the source-reconstruction backlog obtained by comparing the live Ghidra
program `WC1%2FWC1.EXE` with every `/* Function start: 0x... */` annotation in
`src/**/*.c` and `src/**/*.cpp`. The ordinary developer-written backlog is now
complete. The only two inventory entries without source bodies are **compiler
thunks**, which must not be reproduced manually.

The proposed compilation units follow the current source adjacency and
`docs/ORDER.md`; boundaries explicitly described as provisional there remain
provisional here. In particular, the contiguous raster run remains assigned to
`src/screens.c` through `0x0043F5A9`, because the first function currently in
`src/killbrd.c` is at `0x0043F640`.

The Mac name column was checked against all 881 MacsBug symbols in
`../releases/mac/extracted/symbols.json`, including the established ordered
cross-release mappings in `docs/LABELS.md`. There are **no safe exact Mac-name
matches for the two remaining compiler thunks**. A dash therefore means “no
defensible Mac counterpart”, not “the original function had no name”. Super
Wing Commander is a later code base, so positional similarity alone is not
enough to assign a name.

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
| `0x0040CB20` | `ThunkForwarder40CB20` | — | `src/brains.c` | verified; exported; compiler thunk—do not hand-write |
| `0x00434D10` | `_rand` | — | `src/mathfp.c` | verified; compiler thunk—do not hand-write |

## Mac ordering evidence retained for later naming

- `0x00434D10` is in the historical `rand` neighborhood but is a compiler
  jump thunk to the CRT, not a Mac-named game routine.
- The completed `0x00439C0E`–`0x0043F5A9` raster block has no safe mapping to
  the Mac renderer; that renderer was substantially rewritten.

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
- Two aligned routines between the prior `0x00431900` function and
  `LoadJoystickCalibrationFile` were absent from the live function table.
  `ReleaseContiguousPaletteEntries` was force-created over
  `0x00431970`–`0x004319A0`, and `PrintPaletteAllocationMap` over
  `0x004319B0`–`0x00431A03`. Their listings, calling conventions, prototypes,
  and expected game-core tags were verified before the program was saved.
- `FillViewportEllipse` was force-created at `0x00441E20` after a call to the
  raster ellipse filler exposed its previously unrecognized aligned body. Its
  body `0x00441E20`–`0x00441E69`, six-argument `__cdecl` prototype, and the
  same game-core/UI/native tags as its adjacent ellipse wrapper were verified.
- The Mac `AllocTextLayer` symbol was checked against the newly recovered
  palette-entry allocation cluster but was not assigned: the later Mac text-layer
  sequence does not provide a body or ordering match strong enough to establish
  identity.
- Every source annotation now resolves to an exact live Ghidra function entry.
  The 21 completed ordinary backlog functions and both remaining compiler
  thunks resolve to exact entries with non-empty listings. The completed
  functions have behavior names, reconstructed prototypes, and plate comments
  recording that they are believed unreachable. The Ghidra program was saved
  after verification.

## Export and implementation progress

- All 28 assembly snippets that were absent at the start of this audit, the two
  newly discovered palette-entry helpers, and `FillViewportEllipse` now exist
  in `code-full`. Each export has the same instruction count and return form as
  its exact live Ghidra function. Existing exports were not rewritten.
- Ninety confirmed `wc-developer` functions were reconstructed across thirteen
  tranches after binary-comp comparison:

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
  | `0x0042C510` | `RewriteDiskFileGraphicsExtensions` | `src/sound.c` | 100.00% |
  | `0x0042F890` | `GetFreeNearHeapBytes` | `src/screen.c` | 90.91% |
  | `0x00431900` | `ReserveContiguousPaletteEntries` | `src/screen.c` | 100.00% |
  | `0x00431970` | `ReleaseContiguousPaletteEntries` | `src/screen.c` | 100.00% |
  | `0x004319B0` | `PrintPaletteAllocationMap` | `src/screen.c` | 100.00% |
  | `0x0043E478` | `GetRawImageHeight` | `src/screens.c` | 100.00% |
  | `0x0043E48B` | `GetRawFrameWidth` | `src/screens.c` | 100.00% |
  | `0x0043EA6D` | `CopyPCXPaletteFromFileTail` | `src/screens.c` | 100.00% |
  | `0x0043EA98` | `GetPCXImageSize` | `src/screens.c` | 100.00% |
  | `0x0043F05B` | `GetRLEFrameCount` | `src/screens.c` | 100.00% |
  | `0x0043E784` | `FindIFFChunkData` | `src/screens.c` | 100.00% |
  | `0x0043E98D` | `CopyILBMPalette` | `src/screens.c` | 100.00% |
  | `0x0043E9BE` | `GetILBMImageSize` | `src/screens.c` | 100.00% |
  | `0x0043EE42` | `CopyGIFPalette` | `src/screens.c` | 100.00% |
  | `0x0043EEA3` | `GetGIFImageSize` | `src/screens.c` | 100.00% |
  | `0x0043E675` | `BlitRawScanline` | `src/screens.c` | 100.00% |
  | `0x0043EABF` | `ResetGIFLZWDictionary` | `src/screens.c` | 100.00% |
  | `0x0043EB07` | `ReadGIFDataSubBlockByte` | `src/screens.c` | 100.00% |
  | `0x0043EB20` | `ReadGIFLZWCode` | `src/screens.c` | 100.00% |
  | `0x0043EB66` | `AppendGIFLZWDictionaryEntry` | `src/screens.c` | 100.00% |
  | `0x0043EBAC` | `EmitGIFDecodedPixel` | `src/screens.c` | 100.00% |
  | `0x00404610` | `LoadPaletteTripletsFile` | `src/cmpgn.c` | 100.00% |
  | `0x004219C0` | `LoadGamePaletteFile` | `src/logic.c` | 100.00% |
  | `0x00421F50` | `LoadSpaceflightResources` | `src/logic.c` | 100.00% |
  | `0x00425770` | `ShowMeanwhileTransition` | `src/pilot.cpp` | 96.88% |
  | `0x00426000` | `AddRandomTrainSimHighScores` | `src/pilot.cpp` | 92.21% |
  | `0x0042C420` | `InitializeDiskPromptTextContext` | `src/sound.c` | 100.00% |
  | `0x00408650` | `landing` | `src/brains.c` | 94.95% |
  | `0x004094E0` | `RunAnimationDemoLoop` | `src/brains.c` | 100.00% |
  | `0x0042BC00` | `ShowCarrierLaunchSequence` | `src/sound.c` | 93.08% |
  | `0x0042FB40` | `CreateCannedSceneObject` | `src/screen.c` | 95.35% |
  | `0x0042FC00` | `ShowCampaignVictorySequence` | `src/screen.c` | 92.35% |
  | `0x00430150` | `ShowTigerClawEscapeScene` | `src/screen.c` | 93.98% |
  | `0x004304F0` | `ShowTheEndScreen` | `src/screen.c` | 91.28% |
  | `0x00431A10` | `LoadJoystickCalibrationFile` | `src/screen.c` | 93.42% |
  | `0x00436F50` | `AwardCampaignMedal` | `src/screens.c` | 99.12% |
  | `0x004176C0` | `DrawPendingCockpitDamage` | `src/cockpt.c` | 100.00% |
  | `0x0042B680` | `PlaySnowStaticSound` | `src/sound.c` | 16.67% |
  | `0x0042F740` | `InitializeDIBScreenViewport` | `src/screen.c` | 97.22% |
  | `0x0043C4A2` | `EncodeRLEScanline` | `src/screens.c` | 99.28% |
  | `0x0043C62B` | `EmitRLEScanlineRun` | `src/screens.c` | 98.18% |
  | `0x0043D1C1` | `FillRasterEllipse` | `src/screens.c` | 98.31% |
  | `0x0043E4AB` | `BlitRawFrame` | `src/screens.c` | 94.25% |
  | `0x00441E20` | `FillViewportEllipse` | `src/gr.c` | 100.00% |
  | `0x004259B0` | `PromptForAnswerText` | `src/pilot.cpp` | 94.57% |
  | `0x00439C0E` | `FillRasterBytes` | `src/screens.c` | 100.00% |
  | `0x00439C3E` | `CopyRasterDriverName` | `src/screens.c` | 100.00% |
  | `0x00439C69` | `InstallRasterDriverCallbacks` | `src/screens.c` | 100.00% |
  | `0x0043C015` | `GetRLEFrameBounds` | `src/screens.c` | 100.00% |
  | `0x0043C18D` | `EncodeRasterClipToRLEFrame` | `src/screens.c` | 100.00% |
  | `0x0043C410` | `TranslateRLEFramePalette` | `src/screens.c` | 100.00% |
  | `0x0043CC83` | `ScrollRasterClipWrapped` | `src/screens.c` | 100.00% |
  | `0x0043E63E` | `BlitSelectedRawFrames` | `src/screens.c` | 100.00% |
  | `0x0043E7C6` | `DecodeIFFImage` | `src/screens.c` | 100.00% |
  | `0x0043E9EB` | `DecodePCXImage` | `src/screens.c` | 100.00% |
  | `0x0043EC29` | `ExpandGIFLZWImage` | `src/screens.c` | 100.00% |
  | `0x0043EEDB` | `GetRLEFrameDimensions` | `src/screens.c` | 100.00% |
  | `0x0043EEFD` | `GetRLEFrameExtents` | `src/screens.c` | 100.00% |
  | `0x0043EF7E` | `ApplyRLEFramePalette` | `src/screens.c` | 100.00% |
  | `0x0043EFC9` | `CopyRLEFramePalette` | `src/screens.c` | 100.00% |
  | `0x0043F011` | `SetRLEFramePalette` | `src/screens.c` | 100.00% |
  | `0x0043F06E` | `CollectUniqueRLEImageFrames` | `src/screens.c` | 100.00% |
  | `0x0043F0D0` | `CollectUniqueRLEPaletteFrames` | `src/screens.c` | 100.00% |
  | `0x0043F425` | `FadeRasterPaletteToPalette` | `src/screens.c` | 100.00% |
  | `0x0043F5A9` | `CollectRasterClipColours` | `src/screens.c` | 100.00% |

- Ghidra was synchronized with the reconstructed prototypes and behavior-based
  names, then saved after each tranche.
- `landing` is the exact Mac-derived name for `0x00408650`; the other eight
  names in the eleventh tranche are behavior-based names grounded in their
  assembly, strings, globals, and callers.
- All eight names in the twelfth tranche are behavior-based; none has a safe
  exact Mac counterpart.
- All 21 names in the thirteenth tranche are behavior-based; none has a safe
  exact Mac counterpart. Each source body is explicitly marked as believed
  unreachable because the shipped executable has no known inbound reference.
- `PlaySnowStaticSound` is the best rule-compliant C fallback. The original
  directly invokes C++ `IxSample` and `IxSound` methods; reproducing those calls
  from the core C unit would require a forbidden wrapper/thunk or changing the
  unit's language. The generic wave path preserves audible looping static, but
  consequently remains below the normal similarity threshold.
- The post-tranche `make verify` completed successfully. A live Ghidra xref
  audit found no inbound reference of any kind to the 21 completed ordinary
  backlog functions. The only called remaining inventory entry is the CRT
  `_rand` jump thunk at `0x00434D10`, which remains excluded from manual
  reconstruction.
- The five GIF LZW helpers are called internally by the decoder at
  `0x0043EC29`, but that top-level decoder has no inbound code xref, stored
  function pointer, or PE export. The feature is therefore linked raster-library
  functionality rather than a path known to be reached by WC1 game code.
- `ThunkForwarder40CB20` and `_rand` remain explicitly excluded as
  compiler/linker-generated jump thunks.
- The startup crash report was confirmed against the original disassembly.
  `WinMain` now creates the debug overlay console after initializing the game
  clock, and both normal and early shutdown paths destroy it in the original
  order. The `-w` animation path now invokes `RunAnimationDemoLoop` with the
  selected animation number.

The compiler configuration and original executable were not modified, and the
game was not run. `make compare-func` and `make verify` only rebuilt and linked
the reconstruction for comparison and repository audits.
