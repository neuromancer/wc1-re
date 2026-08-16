# WC2 address-remap similarity report

This report compares the current WC1 reconstruction against WC2 code at the reviewed WC1→WC2 function destinations. Unmapped functions are excluded rather than compared against arbitrary bytes.

The converted source contains WC2 labels only. Historical WC1 addresses live in the row-level migration manifest so a future `wc2-re` fork does not need a dual-game annotation or build path.

## Coverage and headline result

| Metric | Result |
| --- | ---: |
| WC1 source function markers | 1478 |
| Markers with a WC2 destination | 1235 (83.56%) |
| Explicitly unresolved markers | 243 |
| Functions compared by binary-comp | 1215 |
| Compared / mapped markers | 98.38% |
| Exact machine-code matches | 303 |
| Similarity >= 90% | 461 |
| Similarity < 50% | 254 |
| Average WC2 similarity | 74.01% |
| Median WC2 similarity | 80.58% |
| Report errors / missing exports | 0 |
| WC1 baseline functions compared | 1454 |
| WC1 baseline average similarity | 98.45% |

Low-confidence address assignments and genuinely changed code both depress the aggregate, so the evidence tiers below are more informative than the headline average on its own.

## Compiler/code-generation control

| Rebuild configuration | Compared | Average | Exact | >=90% | <50% |
| --- | ---: | ---: | ---: | ---: | ---: |
| MSVC 4.20, WC1 optimized core (`/Og /Oi /Ot /Oy /Ob1 /Gs`) | 1215 | 47.48% | 153 | 176 | 781 |
| MSVC 4.20, unoptimized core (`/Od /Oi`) | 1215 | 74.01% | 302 | 460 | 254 |
| MSVC 4.1, unoptimized core (`/Od /Oi`) | 1215 | 74.01% | 303 | 461 | 254 |

Disabling game-core optimization gains 26.54 similarity points. WC2's core consistently has frame pointers, unconditional EBX/ESI/EDI saves, spilled temporaries, and shared epilogues, so `/Od` is the appropriate comparison control. This establishes a code-generation difference from WC1. Both executables use the Microsoft debug CRT, but their PE linker versions differ: WC1 reports 4.20 and WC2 reports 3.10. The selected MSVC 4.1 package identifies its compiler as 10.10.6038 and its linker as 3.10.6038, an exact match for WC2's linker stamp.

Moving from MSVC 4.20 `/Od` to MSVC 4.1 `/Od` leaves the rounded average at 74.01% but gains 1 exact match and 1 function at >=90%.

## Similarity by mapping evidence

| Evidence tag | Compared | Average | Median | Exact | >=90% | <50% |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `WC1-auto-vt` | 183 | 97.52% | 100.00% | 145 | 168 | 1 |
| `WC1-order-exact` | 24 | 82.35% | 87.98% | 8 | 9 | 1 |
| `WC1-callgraph-transfer` | 8 | 81.58% | 87.81% | 1 | 4 | 1 |
| `WC1-fuzzy-low` | 157 | 80.58% | 88.24% | 31 | 67 | 12 |
| `WC1-fuzzy-followup` | 165 | 78.75% | 82.30% | 22 | 50 | 13 |
| `WC1-callgraph-single` | 21 | 73.48% | 82.76% | 4 | 9 | 5 |
| `WC1-fuzzy-medium` | 97 | 73.38% | 76.00% | 22 | 33 | 16 |
| `WC1-fuzzy-high` | 27 | 70.67% | 84.40% | 7 | 13 | 7 |
| `WC1-fuzzy-very-low` | 176 | 63.90% | 63.22% | 11 | 27 | 58 |
| `WC1-callgraph-propagated` | 297 | 62.31% | 62.63% | 42 | 69 | 114 |
| `WC1-callgraph-single-low` | 60 | 58.04% | 53.65% | 10 | 12 | 26 |

## Similarity by source area

| Area | Compared | Average | Median | Exact | >=90% | <50% |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| game/core | 1089 | 71.11% | 75.66% | 196 | 336 | 253 |
| ix audio | 126 | 99.12% | 100.00% | 107 | 125 | 1 |

## Explicit review flags

| Flag | Compared | Average | Median | Exact | >=90% | <50% |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `WC1-review-module-mismatch` | 21 | 50.51% | 50.88% | 3 | 4 | 9 |

## Per-file summary

| Source file | Compared | Average | Median | Exact | >=90% | <50% |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `src/auto.c` | 5 | 90.32% | 93.02% | 2 | 3 | 0 |
| `src/barracks.c` | 19 | 41.44% | 40.70% | 0 | 1 | 15 |
| `src/brains.c` | 124 | 74.30% | 81.72% | 24 | 53 | 26 |
| `src/cdrom.c` | 7 | 78.73% | 83.33% | 2 | 3 | 1 |
| `src/cmpgn.c` | 12 | 37.59% | 38.03% | 0 | 0 | 10 |
| `src/cockpt.c` | 107 | 70.91% | 73.08% | 13 | 27 | 22 |
| `src/debug.cpp` | 15 | 73.05% | 72.13% | 1 | 4 | 2 |
| `src/dib.c` | 18 | 78.88% | 86.86% | 1 | 4 | 2 |
| `src/disk.c` | 29 | 56.52% | 52.99% | 1 | 4 | 12 |
| `src/eventmgr.c` | 31 | 67.09% | 73.33% | 5 | 9 | 9 |
| `src/geom.c` | 85 | 76.74% | 88.31% | 28 | 37 | 17 |
| `src/gr.c` | 36 | 75.49% | 74.22% | 2 | 6 | 1 |
| `src/hudmsg.c` | 33 | 64.61% | 60.99% | 2 | 8 | 8 |
| `src/ix/dsp.cpp` | 19 | 99.86% | 100.00% | 18 | 19 | 0 |
| `src/ix/dsps.cpp` | 10 | 98.72% | 100.00% | 6 | 10 | 0 |
| `src/ix/dspv.cpp` | 15 | 98.94% | 100.00% | 9 | 15 | 0 |
| `src/ix/ixlog.cpp` | 1 | 45.61% | 45.61% | 0 | 0 | 1 |
| `src/ix/lzo1x.cpp` | 6 | 100.00% | 100.00% | 6 | 6 | 0 |
| `src/ix/mixer.cpp` | 2 | 99.05% | 99.05% | 1 | 2 | 0 |
| `src/ix/sample.cpp` | 4 | 98.72% | 98.73% | 2 | 4 | 0 |
| `src/ix/sound.cpp` | 7 | 99.94% | 100.00% | 6 | 7 | 0 |
| `src/ix/streamer.cpp` | 25 | 99.67% | 100.00% | 23 | 25 | 0 |
| `src/ix/system.cpp` | 27 | 100.00% | 100.00% | 27 | 27 | 0 |
| `src/ix/thread.cpp` | 10 | 99.09% | 100.00% | 9 | 10 | 0 |
| `src/killbrd.c` | 14 | 62.77% | 60.77% | 3 | 4 | 5 |
| `src/logic.c` | 111 | 67.88% | 71.15% | 7 | 27 | 25 |
| `src/main.c` | 21 | 63.13% | 68.60% | 0 | 2 | 7 |
| `src/mathfp.c` | 25 | 74.82% | 80.70% | 5 | 7 | 4 |
| `src/mathutil.c` | 3 | 82.69% | 86.96% | 1 | 1 | 0 |
| `src/mono.c` | 18 | 75.83% | 96.55% | 8 | 10 | 6 |
| `src/music.c` | 33 | 67.12% | 72.22% | 2 | 4 | 7 |
| `src/nav.c` | 42 | 73.35% | 78.30% | 8 | 11 | 8 |
| `src/pilot.cpp` | 22 | 61.50% | 61.52% | 2 | 3 | 8 |
| `src/pload.c` | 10 | 77.30% | 87.85% | 2 | 5 | 2 |
| `src/screen.c` | 55 | 67.19% | 65.52% | 6 | 11 | 12 |
| `src/screens.c` | 69 | 84.44% | 100.00% | 48 | 51 | 13 |
| `src/ship.c` | 31 | 65.73% | 67.74% | 2 | 4 | 9 |
| `src/smart.c` | 21 | 81.92% | 87.67% | 1 | 6 | 1 |
| `src/sound.c` | 17 | 72.77% | 79.31% | 5 | 5 | 4 |
| `src/spc.c` | 20 | 71.62% | 75.86% | 1 | 4 | 5 |
| `src/strdos.c` | 7 | 79.35% | 75.00% | 3 | 3 | 1 |
| `src/sysinput.c` | 8 | 84.12% | 100.00% | 5 | 5 | 1 |
| `src/system.c` | 5 | 57.20% | 60.00% | 0 | 0 | 1 |
| `src/text.c` | 1 | 37.25% | 37.25% | 0 | 0 | 1 |
| `src/winmain.c` | 35 | 75.44% | 85.21% | 6 | 14 | 8 |

## Unresolved mapping inventory

| Source file | Count | Functions |
| --- | ---: | --- |
| `src/barracks.c` | 6 | `InitializeBarracksAnimation`, `FreeBarracksMenuLabel`, `FreeBarracksMenuLabels`, `SaveGameWithNamePrompt`, `SetBunkMenuLabel`, `UpdateBarracksScreen` |
| `src/brains.c` | 23 | `ShipAiState35`, `Mchill`, `Mkickit`, `Mroll_over`, `Msplit_left`, `Msplit_right`, `Mtail_fire`, `Mtarget_laser`, … (+15) |
| `src/cmpgn.c` | 4 | `ParseFaceAnimation`, `LongTalk`, `UpdateMap`, `CloseLook` |
| `src/cockpt.c` | 16 | `IsCockpitExplosionActive`, `EraseCockpitReadoutAtPosition`, `ClearHudGunReadouts`, `set_global_message`, `CockpitMessage`, `update_status_text`, `sighted`, `visited`, … (+8) |
| `src/dib.c` | 4 | `GetDIBWindowHandle`, `CachePaletteEntryFromWords`, `DIBwaitForVerticalBlank`, `DirectDrawResultToText` |
| `src/disk.c` | 3 | `LoadPacketIntoBuffer`, `GetZeroUnused`, `remove_weapon` |
| `src/eventmgr.c` | 30 | `AllocateInputEvent`, `ReleaseInputEventQueue`, `RemoveInputEvent`, `GetNextInputEvent`, `IsInputEventQueued`, `ResetAllocationDepth`, `CheckCursor`, `ApplyPackedMousePosition`, … (+22) |
| `src/geom.c` | 13 | `GetMusicDriverPresent`, `drain_fuel`, `damage_ion_drive`, `position_relative_ijk`, `intfract_sign`, `SignShort`, `IsPairEqualityDifferentFromFlag`, `ChooseRandomSignedMagnitude`, … (+5) |
| `src/gr.c` | 7 | `ClipViewportToScreen`, `MarkActivePaletteEntries`, `DrawViewportLine`, `TriangleRasterizerHook`, `UpdateStreamerStoppedFlag`, `Streamer_stop`, `ForceStreamerTrigger` |
| `src/hudmsg.c` | 7 | `FigureArcadeTime`, `drop_player_mine`, `personality_killed`, `find_next_gun`, `select_guns`, `WaitForDebugStep`, `FrameTimerCallback` |
| `src/ix/dsp.cpp` | 2 | `ix_dsp_static_initializer`, `ix_dsp_static_initialization_hook` |
| `src/ix/lzo1x.cpp` | 1 | `ix_file_read` |
| `src/ix/mixer.cpp` | 2 | `ix_mixer_static_initializer`, `ix_mixer_static_initialization_hook` |
| `src/ix/sound.cpp` | 1 | `ix_sound_is_playing` |
| `src/ix/thread.cpp` | 1 | `IxStreamFile::ix_stream_file_tell` |
| `src/killbrd.c` | 6 | `ClearRoomMenuLabel`, `IsRoomMenuLabelEmpty`, `DrawRoomMenuLabel`, `ClearRoomMenuCursorFrame`, `SelectRoomMenuLabel`, `ResetCampaignData` |
| `src/logic.c` | 22 | `find_weapon`, `InitializeEventManagerResources`, `GetFxDriverInitResult`, `GetMessagePumpResult`, `GetFxDriverStatus`, `unflag_ace`, `flag_ace`, `kill_ace`, … (+14) |
| `src/main.c` | 2 | `get_player_input`, `SelectPreviousExternalViewObject` |
| `src/mathfp.c` | 4 | `WaitForVerticalBlankThunk`, `IdentityHandle`, `AppendTextCharacter`, `ResetTextCursor` |
| `src/music.c` | 11 | `initialize_scripted_view`, `GetTargetColourIndex`, `FadeMusic`, `PaletteFadeHook`, `FlushSoundEffects`, `MapMusicTrackToStreamerCommand`, `ProcessMusicScriptCommand`, `GetMusicMode`, … (+3) |
| `src/nav.c` | 10 | `NavMapPointInsideReservedArea`, `ResetNavMapReservedAreas`, `ResetNavMapLabels`, `add_statistics`, `FullMissionScore`, `PlayersMissionScore`, `MoveNewCampaign`, `GetPaletteReadyUnused`, … (+2) |
| `src/pilot.cpp` | 11 | `WaitForKeyExceptXOrF12`, `ApplyAnswerTextCipher`, `GetHighScoreValue`, `SetHighScoreEntry`, `SortTrainSimHighScores`, `FindTrainSimHighScore`, `InsertTrainSimHighScore`, `IsHighScoreSlotUsed`, … (+3) |
| `src/screen.c` | 13 | `too_busy`, `reply`, `ShouldSuspendCursorForRect`, `MessagePumpHook`, `MapPacketHandleToBlock`, `GetFixedOneMillionThunk`, `CreateCannedSceneObject`, `GetPendingMenuAction`, … (+5) |
| `src/screens.c` | 13 | `DrawMedalChest`, `MedalEstablish`, `PinMedal`, `EstablishingShot`, `DrawBriefingLongShot`, `ReturnToBriefingLongShot`, `DrawDebriefingLongShot`, `DebriefingEstablishingShot`, … (+5) |
| `src/ship.c` | 4 | `check_next_wave`, `the_creator`, `send_at_point`, `rnd_sign` |
| `src/smart.c` | 3 | `handle_collisions`, `reset_stress`, `any_defense` |
| `src/sound.c` | 4 | `ServiceSoundSystem`, `FxDriverShutdownHook`, `InitializeDiskPromptTextContext`, `GetJoystickPresentUnused` |
| `src/spc.c` | 2 | `WaitForJoystickButtonRelease`, `WaitForJoystickButtonPress` |
| `src/strdos.c` | 9 | `DosFarPtrToNear`, `CopyFarString`, `DosMemcpy`, `DosMemset`, `GetEventManagerStatus`, `RegisterEventManagerShutdown`, `InitializeEventManager`, `ShutdownEventManager`, … (+1) |
| `src/sysinput.c` | 1 | `SetMousePositionDuplicate` |
| `src/winmain.c` | 8 | `easy2see`, `near_field`, `ok_hazard_spot`, `extra_hazard`, `match_ship_to_eye`, `start_hazard_field`, `add_hazard_field`, `GetJoystickButtons` |

The complete row-level mapping and unresolved list is in `reports/wc2-address-remap.tsv`.
All binary-comp rows, sorted from lowest to highest similarity, are in `reports/wc2-similarity.tsv`.

## Lowest-scoring mapped comparisons

| Source | Function | WC1 | WC2 | Evidence | Similarity |
| --- | --- | ---: | ---: | --- | ---: |
| `src/eventmgr.c` | `IsFrameTickElapsed` | `0x436240` | `0x44A03C` | `WC1-callgraph-propagated` | 2.61% |
| `src/logic.c` | `ReleaseMusicTrackHook` | `0x424CF0` | `0x40E31F` | `WC1-callgraph-propagated` | 4.46% |
| `src/logic.c` | `drop_mine` | `0x4212A0` | `0x448D58` | `WC1-callgraph-single-low` | 6.07% |
| `src/mathfp.c` | `SetTextCursor` | `0x434F70` | `0x488B20` | `WC1-fuzzy-high` | 6.25% |
| `src/logic.c` | `FreeConstellationObject` | `0x4243B0` | `0x44C796` | `WC1-callgraph-propagated` | 6.95% |
| `src/music.c` | `ResetSoundStateForScene` | `0x42EEA0` | `0x4732E0` | `WC1-fuzzy-low` | 8.33% |
| `src/screens.c` | `DrawMedals` | `0x4375C0` | `0x452AC8` | `WC1-callgraph-single-low` | 9.33% |
| `src/logic.c` | `UpdateSceneAnimationObject` | `0x424EF0` | `0x446A1D` | `WC1-callgraph-single-low` | 11.41% |
| `src/brains.c` | `scramble` | `0x408200` | `0x429423` | `WC1-callgraph-propagated` | 11.92% |
| `src/barracks.c` | `CreateEmptySaveGameFile` | `0x41ADA0` | `0x40230E` | `WC1-callgraph-propagated` | 12.33% |
| `src/main.c` | `RefreshMemoryStatusOverlay` | `0x427C30` | `0x40963B` | `WC1-callgraph-propagated` | 12.59% |
| `src/geom.c` | `random_radial` | `0x418800` | `0x413A3B` | `WC1-callgraph-propagated` | 13.02% |
| `src/nav.c` | `Title_Sequence` | `0x40FB70` | `0x42F1FD` | `WC1-callgraph-propagated` | 13.77% |
| `src/logic.c` | `skill_check` | `0x4236B0` | `0x45655D` | `WC1-callgraph-single-low` | 14.89% |
| `src/barracks.c` | `LoadGame` | `0x41B710` | `0x446F14` | `WC1-callgraph-propagated` | 14.96% |
| `src/screens.c` | `ShowGameOverScreen` | `0x439A80` | `0x45F7EC` | `WC1-callgraph-propagated` | 15.34% |
| `src/barracks.c` | `SaveGame` | `0x41B1E0` | `0x402385` | `WC1-callgraph-propagated` | 15.35% |
| `src/eventmgr.c` | `WaitForFrameTick` | `0x436230` | `0x484453` | `WC1-fuzzy-high` | 15.38% |
| `src/winmain.c` | `GetF1KeyLatch` | `0x402520` | `0x473CBA` | `WC1-fuzzy-high` | 16.67% |
| `src/geom.c` | `vector_component_in_dir` | `0x418BB0` | `0x401FDD` | `WC1-callgraph-propagated` | 18.69% |
| `src/killbrd.c` | `CorrectPointers` | `0x43F640` | `0x47EA00` | `WC1-fuzzy-medium` | 18.75% |
| `src/geom.c` | `IsPointWithinEyeViewCone` | `0x41A130` | `0x43CF5A` | `WC1-callgraph-propagated` | 18.99% |
| `src/brains.c` | `DrawScrambleFrame` | `0x407E10` | `0x42C7BC` | `WC1-callgraph-single-low` | 19.24% |
| `src/disk.c` | `PromptInsertNumberedDisk` | `0x41D760` | `0x437A44` | `WC1-callgraph-propagated` | 19.35% |
| `src/screens.c` | `AwardCampaignMedal` | `0x436F50` | `0x44BEE5` | `WC1-callgraph-propagated` | 20.61% |
| `src/logic.c` | `clear_crash_cache` | `0x422440` | `0x446D55` | `WC1-callgraph-single-low` | 21.43% |
| `src/mathfp.c` | `MeasureShapeFrameStorage` | `0x435340` | `0x417124` | `WC1-fuzzy-medium` | 21.51% |
| `src/screen.c` | `ShowTigerClawEscapeScene` | `0x430150` | `0x446823` | `WC1-callgraph-propagated` | 21.76% |
| `src/mono.c` | `GetLineLength` | `0x403890` | `0x42C0A2` | `WC1-callgraph-single` | 22.11% |
| `src/brains.c` | `landing` | `0x408650` | `0x42EF12` | `WC1-callgraph-propagated` | 22.99% |
| `src/screen.c` | `ShowCampaignVictorySequence` | `0x42FC00` | `0x433C84` | `WC1-callgraph-propagated` | 23.06% |
| `src/hudmsg.c` | `UpdateArcadeScoreDisplay` | `0x429EE0` | `0x435064` | `WC1-callgraph-propagated` | 23.41% |
| `src/brains.c` | `funeral_sequence` | `0x408DE0` | `0x459D74` | `WC1-callgraph-propagated` | 23.44% |
| `src/logic.c` | `nearest_enemy_range` | `0x4230F0` | `0x4608E8` | `WC1-callgraph-propagated` | 23.87% |
| `src/barracks.c` | `ConfirmAwakenAfterBadData` | `0x41BF60` | `0x436722` | `WC1-callgraph-propagated` | 24.18% |
| `src/disk.c` | `borrow_dust` | `0x41DF40` | `0x41040D` | `WC1-callgraph-single` | 24.49% |
| `src/cmpgn.c` | `AddPCName` | `0x404E10` | `0x452930` | `WC1-callgraph-propagated` | 24.59% |
| `src/barracks.c` | `ConfirmReplaceFaultyData` | `0x41BFE0` | `0x43641C` | `WC1-callgraph-propagated` | 24.86% |
| `src/pilot.cpp` | `PromptForAnswerText` | `0x4259B0` | `0x40DA0C` | `WC1-callgraph-single-low` | 25.26% |
| `src/mathfp.c` | `SetTextContext` | `0x434FA0` | `0x40F882` | `WC1-callgraph-propagated` | 25.53% |

## Reproduction

```sh
make wc2-remap-audit
make export-asm
make report
```
