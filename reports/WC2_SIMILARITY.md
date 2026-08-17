# WC2 address-remap similarity report

This report compares the current WC1 reconstruction against WC2 code at the reviewed WC1→WC2 function destinations. Unmapped functions are excluded rather than compared against arbitrary bytes.

The converted source contains WC2 labels only. Historical WC1 addresses live in the row-level migration manifest so a future `wc2-re` fork does not need a dual-game annotation or build path.

## Coverage and headline result

| Metric | Result |
| --- | ---: |
| WC1 source function markers | 1534 |
| Markers with a WC2 destination | 1295 (84.42%) |
| Explicitly unresolved markers | 239 |
| Functions compared by binary-comp | 1279 |
| Compared / mapped markers | 98.76% |
| Exact machine-code matches | 602 |
| Similarity >= 90% | 870 |
| Similarity < 50% | 94 |
| Average WC2 similarity | 88.73% |
| Median WC2 similarity | 97.56% |
| Report errors / missing exports | 0 |

Low-confidence address assignments and genuinely changed code both depress the aggregate, so the evidence tiers below are more informative than the headline average on its own.

## Similarity by mapping evidence

| Evidence tag | Compared | Average | Median | Exact | >=90% | <50% |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `WC2-manual-verified` | 1 | 100.00% | 100.00% | 1 | 1 | 0 |
| `WC1-auto-vt` | 180 | 99.03% | 100.00% | 150 | 174 | 0 |
| `WC2-only` | 56 | 97.91% | 100.00% | 40 | 53 | 0 |
| `manual-verified` | 28 | 95.97% | 100.00% | 18 | 27 | 0 |
| `WC1-fuzzy-high` | 14 | 93.95% | 96.55% | 5 | 11 | 0 |
| `WC1-manual-verified` | 209 | 92.98% | 100.00% | 115 | 172 | 7 |
| `WC1-fuzzy-low` | 105 | 92.95% | 95.29% | 41 | 72 | 0 |
| `WC1-callgraph-transfer` | 8 | 92.77% | 92.22% | 3 | 6 | 0 |
| `WC1-order-exact` | 69 | 92.59% | 100.00% | 45 | 53 | 1 |
| `WC1-order-semantics-verified` | 24 | 91.66% | 94.55% | 11 | 17 | 0 |
| `WC1-fuzzy-medium` | 60 | 91.26% | 100.00% | 31 | 42 | 2 |
| `WC1-order-callgraph-verified` | 9 | 91.18% | 90.00% | 3 | 5 | 0 |
| `WC1-callgraph-single` | 16 | 89.47% | 95.06% | 7 | 11 | 1 |
| `WC1-fuzzy-followup` | 148 | 85.34% | 88.57% | 37 | 67 | 9 |
| `WC1-callgraph-single-low` | 26 | 80.83% | 96.60% | 11 | 15 | 6 |
| `WC1-fuzzy-very-low` | 107 | 80.52% | 87.66% | 23 | 46 | 15 |
| `WC1-callgraph-propagated` | 216 | 75.54% | 86.81% | 61 | 98 | 51 |
| `same per-object update-loop caller; collision/vector callee family; 70.78% binary-comp; target 00445087 is capital-ship strike mission case` | 1 | 70.78% | 70.78% | 0 | 0 | 0 |
| `function order; parameter shape; unique BriefingMap_DisplayMap caller; scanner helper anchors; assembly control flow` | 2 | 42.67% | 42.67% | 0 | 0 | 2 |

## Similarity by source area

| Area | Compared | Average | Median | Exact | >=90% | <50% |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| game/core | 1151 | 87.52% | 95.45% | 492 | 742 | 94 |
| ix audio | 128 | 99.56% | 100.00% | 110 | 128 | 0 |

## Explicit review flags

| Flag | Compared | Average | Median | Exact | >=90% | <50% |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `WC1-review-module-mismatch` | 10 | 83.56% | 99.08% | 5 | 6 | 0 |

## Per-file summary

| Source file | Compared | Average | Median | Exact | >=90% | <50% |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `src/auto.c` | 5 | 92.82% | 95.35% | 2 | 3 | 0 |
| `src/barracks.c` | 13 | 45.54% | 40.91% | 0 | 1 | 11 |
| `src/brains.c` | 136 | 87.90% | 97.78% | 65 | 94 | 11 |
| `src/cdrom.c` | 5 | 89.75% | 90.41% | 2 | 3 | 0 |
| `src/cmpgn.c` | 10 | 51.27% | 38.64% | 2 | 2 | 7 |
| `src/cockpt.c` | 124 | 88.05% | 95.75% | 57 | 79 | 11 |
| `src/debug.cpp` | 12 | 81.02% | 84.69% | 1 | 5 | 1 |
| `src/dib.c` | 24 | 88.93% | 90.23% | 2 | 12 | 0 |
| `src/disk.c` | 28 | 79.07% | 87.72% | 8 | 13 | 5 |
| `src/eventmgr.c` | 66 | 92.66% | 96.28% | 26 | 53 | 1 |
| `src/geom.c` | 87 | 94.92% | 100.00% | 51 | 73 | 1 |
| `src/gr.c` | 46 | 89.10% | 91.92% | 16 | 28 | 0 |
| `src/hudmsg.c` | 27 | 80.47% | 92.31% | 11 | 16 | 4 |
| `src/ix/dsp.cpp` | 19 | 99.86% | 100.00% | 18 | 19 | 0 |
| `src/ix/dsps.cpp` | 10 | 98.72% | 100.00% | 6 | 10 | 0 |
| `src/ix/dspv.cpp` | 15 | 98.94% | 100.00% | 9 | 15 | 0 |
| `src/ix/ixlog.cpp` | 1 | 100.00% | 100.00% | 1 | 1 | 0 |
| `src/ix/lzo1x.cpp` | 7 | 100.00% | 100.00% | 7 | 7 | 0 |
| `src/ix/mixer.cpp` | 2 | 99.05% | 99.05% | 1 | 2 | 0 |
| `src/ix/sample.cpp` | 4 | 98.72% | 98.73% | 2 | 4 | 0 |
| `src/ix/sound.cpp` | 8 | 99.94% | 100.00% | 7 | 8 | 0 |
| `src/ix/streamer.cpp` | 25 | 99.67% | 100.00% | 23 | 25 | 0 |
| `src/ix/system.cpp` | 27 | 100.00% | 100.00% | 27 | 27 | 0 |
| `src/ix/thread.cpp` | 10 | 99.09% | 100.00% | 9 | 10 | 0 |
| `src/killbrd.c` | 11 | 80.79% | 98.15% | 5 | 6 | 2 |
| `src/logic.c` | 110 | 88.15% | 92.92% | 40 | 68 | 4 |
| `src/main.c` | 21 | 78.94% | 89.66% | 8 | 10 | 5 |
| `src/mathfp.c` | 27 | 94.17% | 96.43% | 13 | 20 | 0 |
| `src/mathutil.c` | 4 | 94.24% | 95.00% | 2 | 3 | 0 |
| `src/mono.c` | 14 | 87.37% | 100.00% | 9 | 11 | 3 |
| `src/music.c` | 38 | 88.45% | 100.00% | 21 | 24 | 2 |
| `src/nav.c` | 42 | 84.68% | 89.93% | 15 | 21 | 4 |
| `src/pilot.cpp` | 18 | 95.03% | 100.00% | 15 | 15 | 1 |
| `src/pload.c` | 9 | 91.12% | 98.04% | 3 | 7 | 0 |
| `src/screen.c` | 54 | 83.97% | 91.23% | 16 | 29 | 4 |
| `src/screens.c` | 60 | 91.68% | 100.00% | 48 | 51 | 7 |
| `src/ship.c` | 35 | 79.57% | 88.41% | 9 | 16 | 5 |
| `src/smart.c` | 24 | 91.58% | 92.56% | 4 | 14 | 0 |
| `src/sound.c` | 15 | 95.11% | 97.14% | 7 | 12 | 0 |
| `src/spc.c` | 22 | 81.24% | 87.51% | 3 | 7 | 2 |
| `src/strdos.c` | 4 | 100.00% | 100.00% | 4 | 4 | 0 |
| `src/sysinput.c` | 9 | 98.25% | 100.00% | 8 | 8 | 0 |
| `src/system.c` | 3 | 75.79% | 67.96% | 1 | 1 | 0 |
| `src/text.c` | 6 | 98.67% | 100.00% | 4 | 6 | 0 |
| `src/winmain.c` | 42 | 88.44% | 93.94% | 14 | 27 | 3 |

## Unresolved mapping inventory

| Source file | Count | Functions |
| --- | ---: | --- |
| `src/barracks.c` | 12 | `CreateEmptySaveGameFile`, `EnsureSaveGameFile`, `InitializeBarracksAnimation`, `FreeBarracksMenuLabel`, `FreeBarracksMenuLabels`, `SaveGame`, `SaveGameWithNamePrompt`, `LoadGame`, … (+4) |
| `src/brains.c` | 16 | `AnimateScrambleWalk`, `DrawScrambleActor`, `ConfigureScrambleActor`, `DrawScrambleFrame`, `scramble`, `landing`, `funeral_player`, `funeral_wingman`, … (+8) |
| `src/cdrom.c` | 2 | `AllocateFontWorkspace`, `FreeFontWorkspace` |
| `src/cmpgn.c` | 7 | `ParseFaceAnimation`, `ParseMouthAnimation`, `AddPCName`, `LongTalk`, `Briefing`, `LoadBriefingData`, `CloseLook` |
| `src/cockpt.c` | 3 | `FormatTextBufferFromStart`, `EraseCockpitReadoutAtPosition`, `PlayTargetLockSfx` |
| `src/debug.cpp` | 3 | `DebugOverlayConsole::~DebugOverlayConsole`, `DebugKeyboardHookProc`, `DebugOverlayConsole::SetOpaqueBackground` |
| `src/dib.c` | 1 | `DirectDrawResultToText` |
| `src/disk.c` | 8 | `LoadPacketIntoBuffer`, `ReleaseTextFont`, `SortSignedByteValuesAscending`, `PromptInsertNumberedDisk`, `GetZeroUnused`, `CheckEscaped`, `MoveMenuPointerFromKeyboard`, `WaitForStreamInputKey` |
| `src/eventmgr.c` | 2 | `shrink_vector`, `shrink` |
| `src/geom.c` | 11 | `SeekPacketSection`, `GetMusicDriverPresent`, `intfract_sign`, `SignShort`, `MakeRandomNormalizedVector`, `ConvertShortVectorToFixedVector`, `ship_vs_point`, `ship_vs_ship`, … (+3) |
| `src/gr.c` | 2 | `DrawSpriteScaled`, `ClearStreamerTrigger` |
| `src/hudmsg.c` | 14 | `GetArcadeBonus`, `FigureArcadeTime`, `UpdateArcadeScoreDisplay`, `RefreshCockpitStatus`, `ResetMouseCursorFrame`, `UpdateRoomMenuCursor`, `find_objective`, `warp`, … (+6) |
| `src/ix/dsp.cpp` | 2 | `ix_dsp_static_initializer`, `ix_dsp_static_initialization_hook` |
| `src/ix/mixer.cpp` | 2 | `ix_mixer_static_initializer`, `ix_mixer_static_initialization_hook` |
| `src/ix/thread.cpp` | 1 | `IxStreamFile::ix_stream_file_is_reading` |
| `src/killbrd.c` | 10 | `CorrectPointers`, `ClearRoomMenuLabel`, `IsRoomMenuLabelEmpty`, `DrawRoomMenuLabel`, `RefreshRoomMenuLabel`, `ClearRoomMenuCursorFrame`, `SelectRoomMenuLabel`, `InitializeRoomMenu`, … (+2) |
| `src/logic.c` | 27 | `find_weapon`, `drop_mine`, `InitializeEventManagerResources`, `GetFxDriverInitResult`, `GetMessagePumpResult`, `GetFxDriverStatus`, `ace_status`, `unflag_ace`, … (+19) |
| `src/main.c` | 2 | `init_player_input`, `SelectPreviousExternalViewObject` |
| `src/mathfp.c` | 3 | `ReleaseVideoResourcesHook`, `MeasureShapeFrameStorage`, `ResetTextCursor` |
| `src/mono.c` | 4 | `MeasureScaledIntroTextWidth`, `GetLineLength`, `update_canned_sequence`, `ResetStringBuilder` |
| `src/music.c` | 9 | `parse_view_script`, `update_scripted_view`, `initialize_scripted_view`, `DecompressPacketSection`, `GetTargetColourIndex`, `SetMusicOn`, `SelectFlightMusicTrack`, `MapMusicTrackToStreamerCommand`, … (+1) |
| `src/nav.c` | 11 | `NavMapPointInsideReservedArea`, `MergeAdjacentNearHeapBlocks`, `add_statistics`, `FullMissionScore`, `PlayersMissionScore`, `UpdateSeries`, `MoveNewCampaign`, `StartNewCampaign`, … (+3) |
| `src/pilot.cpp` | 22 | `WaitForKeyExceptXOrF12`, `ShowMeanwhileTransition`, `ApplyAnswerTextCipher`, `LoadAnswerPromptAndResponse`, `PromptForAnswerText`, `EraseTextContextBackground`, `AnimateTrainSimTitle`, `GetHighScoreEntry`, … (+14) |
| `src/pload.c` | 1 | `FindActiveSoundEntryBySample` |
| `src/screen.c` | 14 | `ShouldSuspendCursorForRect`, `FrameStartHook`, `CreateCannedSceneObject`, `ShowCampaignVictorySequence`, `ShowTigerClawEscapeScene`, `UpdateInputDeviceTransitions`, `get_face`, `LoadCommPortraitShape`, … (+6) |
| `src/screens.c` | 22 | `LoadBriefingRoom`, `AwardCampaignMedal`, `DrawMedalChest`, `MedalEstablish`, `PinMedal`, `DrawMedals`, `EstablishingShot`, `DrawBriefingLongShot`, … (+14) |
| `src/ship.c` | 2 | `check_for_lost_control`, `the_creator` |
| `src/smart.c` | 1 | `process_maneuver_node` |
| `src/sound.c` | 6 | `LaunchPlayerShip`, `ShowCarrierLaunchSequence`, `FxDriverShutdownHook`, `InitializeDiskPromptTextContext`, `LoadInstallDat`, `GetJoystickPresentUnused` |
| `src/strdos.c` | 12 | `DosFarPtrToNear`, `DosStrcpy`, `CopyFarString`, `DosMemcpy`, `DosMemset`, `GetEventManagerStatus`, `RegisterEventManagerShutdown`, `InitializeEventManager`, … (+4) |
| `src/sysinput.c` | 1 | `SetMousePositionDuplicate` |
| `src/system.c` | 2 | `RunTrainSim`, `GetJoystickButtonEdge` |
| `src/text.c` | 1 | `show_info_disp` |
| `src/winmain.c` | 3 | `RestoreGamePalette`, `easy2see`, `GetF1KeyLatch` |

The complete row-level mapping and unresolved list is in `reports/wc2-address-remap.tsv`.
All binary-comp rows, sorted from lowest to highest similarity, are in `reports/wc2-similarity.tsv`.

## Lowest-scoring mapped comparisons

| Source | Function | WC1 | WC2 | Evidence | Similarity |
| --- | --- | ---: | ---: | --- | ---: |
| `src/nav.c` | `Title_Sequence` | `0x40FB70` | `0x407E40` | `WC1-manual-verified` | 22.16% |
| `src/brains.c` | `funeral_sequence` | `0x408DE0` | `0x459D74` | `WC1-callgraph-propagated` | 23.44% |
| `src/barracks.c` | `ConfirmAwakenAfterBadData` | `0x41BF60` | `0x436722` | `WC1-callgraph-propagated` | 24.18% |
| `src/barracks.c` | `ConfirmReplaceFaultyData` | `0x41BFE0` | `0x43641C` | `WC1-callgraph-propagated` | 24.86% |
| `src/killbrd.c` | `ShowChalkBoard` | `0x440510` | `0x459C4D` | `WC1-callgraph-propagated` | 25.75% |
| `src/hudmsg.c` | `UpdateTrainSimMenuCursor` | `0x42A610` | `0x419A40` | `WC1-callgraph-single-low` | 25.87% |
| `src/disk.c` | `OpenDiskDataFile` | `0x41D6C0` | `0x4342E8` | `WC1-callgraph-propagated` | 25.88% |
| `src/brains.c` | `init_intelligence_data` | `0x40C950` | `0x44E44F` | `WC1-callgraph-propagated` | 25.99% |
| `src/screens.c` | `DrawFuneralLongShot` | `0x439220` | `0x42E868` | `WC1-callgraph-single-low` | 27.52% |
| `src/cmpgn.c` | `stranded_sequence` | `0x404BE0` | `0x4251F2` | `WC1-callgraph-propagated` | 28.12% |
| `src/ship.c` | `explode` | `0x420040` | `0x414835` | `WC1-callgraph-propagated` | 29.77% |
| `src/main.c` | `HandleFleetOverviewInput` | `0x428D10` | `0x45F200` | `WC1-callgraph-propagated` | 29.92% |
| `src/screens.c` | `Dismissed` | `0x437B80` | `0x433F94` | `WC1-callgraph-propagated` | 30.59% |
| `src/cmpgn.c` | `Office` | `0x405840` | `0x409B80` | `WC1-callgraph-propagated` | 30.67% |
| `src/hudmsg.c` | `WaitForKeyAcknowledge` | `0x428EA0` | `0x418ECD` | `WC1-callgraph-propagated` | 30.85% |
| `src/barracks.c` | `PromptForTextInput` | `0x41B420` | `0x42E3CF` | `WC1-callgraph-propagated` | 31.48% |
| `src/barracks.c` | `AnimateBarracks` | `0x41BCE0` | `0x40815F` | `WC1-callgraph-propagated` | 31.54% |
| `src/cockpt.c` | `build_your_target_list` | `0x416E90` | `0x43DA68` | `WC1-callgraph-propagated` | 31.71% |
| `src/winmain.c` | `FreeGuardedAllocation` | `0x402DB0` | `0x4138A8` | `WC1-fuzzy-very-low` | 33.03% |
| `src/disk.c` | `EraseLastTextInputCharacter` | `0x41DDF0` | `0x4345A1` | `WC1-callgraph-propagated` | 34.04% |
| `src/logic.c` | `free_cockpit` | `0x4249A0` | `0x458196` | `WC1-callgraph-propagated` | 35.03% |
| `src/nav.c` | `GameFlow` | `0x40F4B0` | `0x46591A` | `WC1-callgraph-propagated` | 35.50% |
| `src/music.c` | `AlignSpriteFrameToRectCorner` | `0x42E1D0` | `0x46505E` | `WC1-fuzzy-followup` | 35.77% |
| `src/ship.c` | `inflict_damage` | `0x41E9B0` | `0x411C72` | `WC1-callgraph-propagated` | 36.41% |
| `src/barracks.c` | `BarracksScreen` | `0x41C170` | `0x419831` | `WC1-callgraph-propagated` | 36.96% |
| `src/music.c` | `DrawConstellationField` | `0x42D500` | `0x45A634` | `WC1-callgraph-propagated` | 37.40% |
| `src/main.c` | `player_input` | `0x4285D0` | `0x466B02` | `WC1-fuzzy-followup` | 37.45% |
| `src/cmpgn.c` | `CloseTalk` | `0x4054B0` | `0x4098F2` | `WC1-callgraph-propagated` | 37.67% |
| `src/barracks.c` | `WarnLoadGameFirst` | `0x41B550` | `0x4346E9` | `WC1-callgraph-propagated` | 37.68% |
| `src/screen.c` | `ShowTheEndScreen` | `0x4304F0` | `0x429261` | `WC1-callgraph-propagated` | 37.72% |
| `src/pilot.cpp` | `DisplayTrainSimHighScoreTable` | `0x425C60` | `0x42C43D` | `WC1-callgraph-propagated` | 38.00% |
| `src/screens.c` | `DrawMedalLongShot` | `0x437250` | `0x434177` | `WC1-callgraph-single-low` | 38.13% |
| `src/logic.c` | `LoadSpaceflightResources` | `0x421F50` | `0x420B12` | `WC1-fuzzy-very-low` | 38.18% |
| `src/screen.c` | `BuildCommunicationCommandMenu` | `0x430FC0` | `0x447890` | `WC1-fuzzy-followup` | 38.18% |
| `src/cmpgn.c` | `DeBriefing` | `0x4056F0` | `0x424D4D` | `WC1-fuzzy-very-low` | 38.26% |
| `src/cmpgn.c` | `LoadFace` | `0x4050B0` | `0x4099A8` | `WC1-callgraph-propagated` | 38.40% |
| `src/logic.c` | `InitializeCockpitResources` | `0x4245B0` | `0x4577D7` | `WC1-callgraph-propagated` | 38.46% |
| `src/brains.c` | `get_pilot_talk` | `0x40C1C0` | `0x434043` | `WC1-callgraph-propagated` | 38.61% |
| `src/screens.c` | `ShowVictoryScreen` | `0x439910` | `0x408CC8` | `WC1-callgraph-propagated` | 38.64% |
| `src/cmpgn.c` | `ejection_sequence` | `0x4046A0` | `0x44EBCA` | `WC1-callgraph-propagated` | 38.89% |

## Reproduction

```sh
make wc2-remap-audit
make export-asm
make report
```
