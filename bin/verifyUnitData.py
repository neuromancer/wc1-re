#!/usr/bin/env python3
"""Verify recovered compilation-unit data blocks through binary-comp.

Only one address shift is allowed per block. Identical literals must retain
their separate locations, and empty-string pointers must remain correct.
Pointers outside a block must target declared globals at their original
field offsets or named function entries, using the rebuilt linker symbols.
Read-only blocks must retain their COFF .rdata storage and declared size.
Uninitialized blocks must retain their COFF .bss storage and declared size.
"""
from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path
import struct
import sys

from appendStaticSymbols import recover_layout

from binary_comp.analyzers.data import (
    AddressComparison, RelocatedAddressRange, relocated_pointer_match,
)
from binary_comp.core.mapfile import parse_msvc_map_symbols
from binary_comp.core.pe import PEImage
from binary_comp.core.symbols import normalize_compiled


@dataclass(frozen=True)
class UnitData:
    original_start: int
    size: int
    globals: tuple[tuple[str, int], ...]
    pointers: frozenset[int]
    external_globals: tuple[tuple[str, int, int], ...] = ()
    storage: str = '.data'
    owner: str | None = None
    # Set only for a verified span within an incompletely recovered section.
    section_size: int | None = None


UNITS = {
    'winmain': UnitData(0x00465040, 795, (
        ('nHazardReferenceSpeed', 0x00),
        ('nActiveHazards', 0x04),
        ('acHazardTravelTimeByView', 0x08),
        ('acHazardPitchRange', 0x10),
        ('bIxAudioEnabled', 0x18),
        ('bKeyEventQueueEnabled', 0x1C),
        ('nGuardedAllocationTotalBytes', 0x20),
        ('nGuardedAllocationBytes', 0x24),
        ('nGuardedAllocationPeakBytes', 0x28),
        ('bShowKilrathiSagaCredits', 0x2C),
        ('bShowFrameRate', 0x30),
        ('bDirectDrawModeCascadeEnabled', 0x34),
        ('DAT_00465078', 0x38),
        ('bCockpitEnabled', 0x3C),
        ('bMainWindowMinimized', 0x40),
        ('DAT_00465084', 0x44),
        ('szMainWindowClassName', 0x48),
        ('szMainWindowTitle', 0x58),
        ('bWindowMessagePumpActive', 0x68),
        ('bF1KeyLatch', 0x6C),
        ('pGuardedAllocationHead', 0x70),
    ), frozenset(), owner='winmain.obj'),
    'winsys': UnitData(0x0046535C, 259, (), frozenset()),
    'mono': UnitData(0x00465460, 225, (
        ('nPacketError', 0x00),
    ), frozenset()),
    'auto': UnitData(0x00465544, 7, (
        ('nAutopilotFormationShipCount', 0x00),
    ), frozenset()),
    'eject': UnitData(0x00465550, 156, (
        ('asEjectionPrimaryFrames', 0x00),
        ('asEjectionSecondaryFrames', 0x10),
        ('asEjectionViewScript', 0x20),
        ('pStrandedMessage', 0x38),
    ), frozenset((0x38,))),
    'cmpgn': UnitData(0x004655F0, 135, (
        ('asMouthFramesByPhoneme', 0x00),
    ), frozenset()),
    'fl': UnitData(0x00465678, 236, (
        ('abManeuverRerollChance', 0x00),
        ('apShipAiManeuverHandlers', 0x30),
    ), frozenset(range(0x30, 0xEC, 4)), (
        ('Mline_up_drop', 0x004060D0, 1),
        ('Mwabble', 0x00406130, 1),
        ('ShipAiState35', 0x00406200, 1),
        ('Mfull_ahead', 0x00406310, 1),
        ('Mchill', 0x00406350, 1),
        ('Mdrop_a_mine', 0x004063B0, 1),
        ('Mthink', 0x00406400, 1),
        ('Mtight_loop', 0x00406440, 1),
        ('Mhard_break', 0x004064F0, 1),
        ('Msit_n_spin', 0x004065A0, 1),
        ('Mturn_n_spin', 0x004067A0, 1),
        ('Mburnout', 0x00406860, 1),
        ('Mkickit', 0x004068D0, 1),
        ('Mturn_n_kick', 0x00406910, 1),
        ('Mroll_over', 0x00406990, 1),
        ('Mhard_turn', 0x004069F0, 1),
        ('Mfish_hook', 0x00406A50, 1),
        ('Mtry2tail', 0x00406B60, 1),
        ('Msplit_left', 0x00406BD0, 1),
        ('Msplit_right', 0x00406C20, 1),
        ('Mgloat', 0x00406C70, 1),
        ('Mtail_fire', 0x00406D20, 1),
        ('Mzip_past', 0x00406D80, 1),
        ('Mtarget_missile', 0x00406E10, 1),
        ('Mram_missile', 0x00406EC0, 1),
        ('Mbuzz_debris', 0x00406F20, 1),
        ('Mstrafe_enemy', 0x00406FB0, 1),
        ('Mbest_strafe', 0x00407030, 1),
        ('Msit_n_fire', 0x00407060, 1),
        ('Mstrafe_n_roll', 0x004070D0, 1),
        ('Mkill_missile', 0x00407100, 1),
        ('Msuicide_run', 0x004071B0, 1),
        ('Mget_distance', 0x004071E0, 1),
        ('Mzig_zag', 0x00407350, 1),
        ('Mzig_zag_pitch', 0x00407370, 1),
        ('Mcorkscrew', 0x00407390, 1),
        ('Mveer_away', 0x00407450, 1),
        ('ShipAiState44', 0x00407560, 1),
        ('Mtarget_laser', 0x00407580, 1),
        ('Mrout_me', 0x004075A0, 1),
        ('Mnone', 0x004075B0, 1),
        ('Mreset', 0x004075C0, 1),
    )),
    'cutscene': UnitData(0x00465768, 1297, (
        ('cScrambleLeftWalkerFrame', 0x000),
        ('cScrambleRightWalkerFrame', 0x004),
        ('acScrambleWalkerOverlayFrames', 0x008),
        ('nScrambleLeftWalkerX', 0x010),
        ('nScrambleRightWalkerX', 0x014),
        ('nScrambleBackgroundX', 0x018),
        ('ausScrambleActorAnimationA', 0x020),
        ('ausScrambleActorAnimationB', 0x030),
        ('aScrambleAnimationActors', 0x048),
        ('aaScrambleShipDetails', 0x0C0),
        ('anLandingDamageDetailCounts', 0x340),
        ('apszLandingDamageComments', 0x350),
        ('acLandingCanopyFramesHeavy', 0x360),
        ('acLandingCanopyFramesLight', 0x378),
        ('acLandingCanopyFramesModerate', 0x390),
        ('apLandingCanopyFrames', 0x3A0),
        ('aFuneralParticleOrigins', 0x3B0),
        ('asFuneralSceneBySeries', 0x3D0),
        ('bFuneralShowTheEnd', 0x3EC),
    ), frozenset((0x053, 0x06B, 0x083,
                  *range(0x350, 0x360, 4), *range(0x3A0, 0x3B0, 4)))),
    'brains': UnitData(0x00465C7C, 2, (
        ('nEnemySighting', 0x00),
    ), frozenset()),
    'mission': UnitData(0x00465C80, 939, (
        ('nDebriefingPersonality', 0x000),
        ('bInitialFormationSetup', 0x004),
        ('aapPacketReferences', 0x008),
        ('aaFormationPositions', 0x258),
        ('anPilotTurnInterval', 0x348),
    ), frozenset()),
    'fl-bss': UnitData(0x00475E78, 5, (
        ('nManeuverProximityRange', 0x00),
        ('bCurrentManeuverReroll', 0x04),
    ), frozenset(), storage='.bss', owner='fl.obj'),
    'objdata': UnitData(0x00466030, 9773, (
        ('anAnimExplosion0', 0x000),
        ('anAnimExplosion1', 0x030),
        ('anAnimExplosion2', 0x088),
        ('anAnimLaserSpark', 0x0C0),
        ('anAnimBlueSpark', 0x0E0),
        ('anAnimRedSpark', 0x0F8),
        ('anAnimSparkTrail', 0x110),
        ('anAnimGirder', 0x128),
        ('anAnimTubing', 0x148),
        ('anAnimGlass', 0x168),
        ('anAnimORing', 0x1B0),
        ('anAnimPipe', 0x1D0),
        ('anAnimMetalSheet', 0x1F0),
        ('anAnimWing', 0x228),
        ('anAnimMine', 0x270),
        ('anAnimAsteroidForward', 0x2A8),
        ('anAnimAsteroidShortForward', 0x2E8),
        ('anAnimAsteroidReverse', 0x320),
        ('anAnimAsteroidShortReverse', 0x360),
        ('anAnimEjectedPilot', 0x398),
        ('anAnimHyperspaceJumpFlash', 0x3D0),
        ('aObjectTypeData', 0x428),
        ('aChildOffsets', 0x22C0),
        ('aWeaponDisplayPositions', 0x2410),
        ('aWeaponDisplayOrigins', 0x2490),
    ), frozenset((*[0x428 + row * 0x87
                    for row in range(58) if row not in (22, 23)],
                  *[0x428 + row * 0x87 + 0x1C
                    for row in range(33, 58) if row != 55]))),
    'nav': UnitData(0x00468660, 608, (
        ('nNavMapCoordinateScaling', 0x00),
        ('nNavMapScale', 0x04),
        ('aNavMapObjectiveStyles', 0x08),
        ('pNavMapShape', 0xA8),
        ('nNavMapLabelCount', 0xAC),
        ('nNavMapReservedAreaCount', 0xB0),
        ('szCampaignSector', 0xB8),
        ('apszShipMissionTypeNames', 0xC8),
        ('bInflightComputerActive', 0xF4),
    ), frozenset((*[0x0C + row * 16 + column * 4
                    for row in range(5) for column in range(3)],
                  *range(0xC8, 0xF4, 4))), (
        ('cBlackColour', 0x0046999C, 1),
        ('cViewportClearColour', 0x004699A0, 1),
        ('cRedColour', 0x004699AC, 1),
        ('cPrimaryTextColour', 0x004699B4, 1),
        ('cMagentaColour', 0x004699C8, 1),
        ('cDefaultTextColour', 0x004699CC, 1),
    )),
    'nearheap': UnitData(0x004688C0, 12, (
        ('nNearHeapActive', 0x00),
        ('nNearHeapMaxDescriptors', 0x04),
        ('nNearHeapRelocationBytes', 0x08),
    ), frozenset()),
    'gameflow': UnitData(0x004688CC, 64, (
        ('bOfficeVisitPending', 0x00),
        ('bPromotionPending', 0x04),
        ('bPlayerEjectedThisMission', 0x08),
        ('bPlayerShipTypeChanged', 0x0C),
        ('nPreviousPlayerShipType', 0x10),
        ('bCampaignStartupMode', 0x14),
        ('nPendingMedalIndex', 0x18),
        ('nPostSeriesSequence', 0x1C),
        ('bSeriesFailed', 0x20),
        ('bCampaignActive', 0x24),
    ), frozenset()),
    'title': UnitData(0x00468910, 1522, (
        ('pszIntroOpeningText', 0x000),
        ('szMissingSecretMissionsMessage', 0x008),
        ('nIntroCreditCount', 0x120),
        ('apszIntroCredits', 0x128),
        ('aTitleMenuRegions', 0x178),
        ('aIntroResourceDescriptors', 0x1B0),
        ('bTitleMenuSceneInitialized', 0x1C8),
    ), frozenset((0x000, *range(0x128, 0x174, 4), 0x1B0, 0x1B8)), (
        ('aObjectTypeData', 0x00466458, 58 * 0x87),
    )),
    'joystick': UnitData(0x00468F04, 238, (), frozenset()),
    'spc': UnitData(0x00468FF4, 12, (
        ('nCapitalShipViewDistance', 0x00),
        ('bLandingAuthorized', 0x04),
        ('bAlternateChaseView', 0x08),
    ), frozenset()),
    'cockpt': UnitData(0x00469000, 945, (
        ('asVduSelectionSound', 0x000),
        ('pszPendingHudMessage', 0x004),
        ('pszDisplayedHudMessage', 0x008),
        ('cRenderedPilotHandFrame', 0x00C),
        ('DAT_00469010', 0x010),
        ('asPilotHandOffsets', 0x018),
        ('pCockpitPilotShape', 0x05C),
        ('pCockpitExplosionBackground', 0x060),
        ('pCockpitExplosionShape', 0x064),
        ('nCockpitExplosionFrame', 0x068),
        ('pReleaseWeaponDisplayBackground', 0x06C),
        ('cReleaseWeaponDisplayFrame', 0x070),
        ('cReleaseWeaponDisplayTicks', 0x074),
        ('cReleaseWeaponDisplayState', 0x078),
        ('szNoWeaponName', 0x080),
        ('nDisplayedObjectiveRange', 0x088),
        ('pszObjectiveStatusMessage', 0x08C),
        ('nScannerTargetObject', 0x090),
        ('aiScannerGridRows', 0x098),
        ('pszMissileLockedMessage', 0x1D4),
        ('bTargetBracketVisible', 0x1D8),
        ('asHudMessageOrigins', 0x1E0),
        ('nTargetLockMarkerX', 0x1F4),
        ('stTargetBracketBounds', 0x1F8),
        ('stPreviousTargetBracketBounds', 0x200),
        ('nNavPointerObject', 0x208),
        ('stTrainSimVduSource', 0x210),
        ('aaCockpitDamagePositions', 0x228),
        ('pConfedCommBackground', 0x278),
        ('pCommStaticShape', 0x27C),
        ('pKilrathiCommBackground', 0x280),
        ('nCommPortraitFrame', 0x284),
        ('bForceDamageDisplayRedraw', 0x288),
    ), frozenset((0x08C, 0x1D4)), owner='cockpt.obj'),
    'version': UnitData(0x004693B4, 13, (
        ('pGameVersion', 0x00),
    ), frozenset((0x00,))),
    'barracks': UnitData(0x004693C8, 632, (
        ('aBarracksBunkOrigins', 0x00),
        ('apszSaveCampaignMenuLabels', 0x20),
        ('apszBarracksMenuLabels', 0x28),
        ('stModalTextBounds', 0x78),
        ('pModalTextPanel', 0x80),
        ('asCampaignPilotFiles', 0x88),
        ('asCampaignBriefingFiles', 0x90),
        ('asMissionDataFiles', 0x98),
    ), frozenset((0x20, 0x24, 0x68, 0x6C, 0x70, 0x74))),
    'fade': UnitData(0x00469640, 2, (
        ('nPaletteTransitionInitialise', 0x00),
    ), frozenset()),
    'debug': UnitData(0x00469644, 34, (
        ('nDebugOverlayConsoleCount', 0x00),
        ('dwDebugOverlayKey', 0x04),
        ('dwDebugOverlayKeyLatch', 0x08),
        ('hDebugKeyboardHook', 0x0C),
    ), frozenset()),
    'disk': UnitData(0x00469668, 523, (
        ('apszGraphicsModeNames', 0x00),
        ('pszPacketReadOperation', 0x10),
        ('pszPacketAllocationOperation', 0x14),
        ('pszPacketLoadErrorFormat', 0x18),
        ('szDiskMarkerFile', 0x20),
        ('nDiskPromptBorderColour', 0x2C),
        ('DAT_00469698', 0x30),
        ('nKeyboardPointerStep', 0x3C),
    ), frozenset((0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x30, 0x34))),
    'ship': UnitData(0x00469878, 286, (
        ('asPlayerDamageSystemTable', 0x00),
        ('aeShipHitDebrisTypes', 0xD8),
        ('acGunRefireDelay', 0xE4),
    ), frozenset()),
    'init': UnitData(0x00469998, 550, (
        ('bSlowSceneAnimation', 0x00),
        ('cBlackColour', 0x04),
        ('cViewportClearColour', 0x08),
        ('cBlueColour', 0x0C),
        ('cYellowColour', 0x10),
        ('cRedColour', 0x14),
        ('cDarkGreenColour', 0x18),
        ('cPrimaryTextColour', 0x1C),
        ('cOrangeColour', 0x20),
        ('cDarkBlueColour', 0x24),
        ('cDarkGreyColour', 0x28),
        ('cLightGreyColour', 0x2C),
        ('cMagentaColour', 0x30),
        ('cDefaultTextColour', 0x34),
        ('cBrownColour', 0x38),
        ('cAsteroidColour', 0x3C),
        ('cPrimaryViewBufferColour', 0x40),
        ('abLegacyVideoModeColours', 0x48),
        ('asConversationTextColours', 0x58),
        ('bGraphicsActive', 0x88),
        ('szInsufficientMemory', 0x90),
        ('anBaseMemoryReservationByVideoMode', 0xF8),
        ('anFullMusicMemoryReservationByVideoMode', 0x108),
        ('anExpandedMemoryReservationByVideoMode', 0x118),
    ), frozenset()),
    'logic': UnitData(0x00469BC0, 447, (
        ('aCommon3SpaceResources', 0x000),
        ('aMissionResourceDescriptors', 0x060),
        ('aCockpitResourceDescriptors', 0x088),
        ('aCockpitSecondaryResources', 0x120),
        ('aCockpitPrimaryResources', 0x148),
        ('bIntroSceneResourcesActive', 0x188),
        ('asConstellationObjectIndices', 0x190),
        ('bCockpitResourcesActive', 0x198),
        ('b3SpaceObjectsActive', 0x19C),
        ('asSceneAnimationLogicalFiles', 0x1A0),
        ('bSceneAnimationWaitCommand', 0x1B0),
    ), frozenset((*range(0x000, 0x058, 8), *range(0x060, 0x080, 8),
                  *range(0x088, 0x118, 8), *range(0x120, 0x140, 8),
                  *range(0x148, 0x180, 8))), (
        ('aObjectTypeData', 0x00466458, 58 * 0x87),
        ('pCockpitPilotShape', 0x0046905C, 4),
        ('pCockpitExplosionShape', 0x00469064, 4),
        ('pConfedCommBackground', 0x00469278, 4),
        ('pCommStaticShape', 0x0046927C, 4),
        ('pKilrathiCommBackground', 0x00469280, 4),
        ('apCommunicationPortraitShapesTable', 0x0059E180, 16 * 4),
        ('pTgtLockShapePtr', 0x005A6BF4, 4),
        ('pCockpitWeaponShapePtr', 0x005A7564, 4),
        ('pCockpitIndicatorShape', 0x005A7658, 4),
        ('pCurrentPilotHandShapePtr', 0x005A7684, 4),
        ('pCockpitDamageShape', 0x005A76F4, 4),
        ('apCockpitShapesList', 0x005A7C80, 4 * 4),
        ('pCinematicViewBackdropPtr', 0x005A7C90, 4),
        ('pLogicRearViewBackdropPtr', 0x005A7C94, 4),
    )),
    'pilot-data': UnitData(0x00469D80, 29, (), frozenset(), owner='pilot.obj'),
    'trainsim': UnitData(0x00469DA0, 519, (
        ('DAT_00469da0', 0x00),
        ('stTrainSimPanelViewport', 0x08),
        ('stTrainSimPanelBounds', 0x20),
        ('szTrainSimTitle', 0x28),
        ('apszBuiltInHighScoreNames', 0x40),
        ('aTrainSimMissionRegions', 0x58),
        ('nTrainSimActive', 0x8C),
        ('nTrainSimMission', 0x90),
        ('nArcadeWave', 0x94),
    ), frozenset(range(0x40, 0x58, 4))),
    'main': UnitData(0x00469FA8, 1018, (
        ('_ovrbuffer', 0x000),
        ('nCannedSceneMode', 0x004),
        ('nArcadeState', 0x008),
        ('nFrameSkipCounter', 0x00C),
        ('nFrameSkip', 0x010),
        ('nSfxVolumeSetting', 0x014),
        ('nMusicVolumeSetting', 0x018),
        ('bViewportDirty', 0x01C),
        ('anVolumeLevels', 0x020),
        ('nOriginDevUnlock', 0x04C),
        ('bPlayerCollisionsEnabled', 0x050),
        ('bPlayerVulnerable', 0x054),
        ('bPlayerCollisionResponse', 0x058),
        ('bViewBufferEnabled', 0x05C),
        ('bCockpitlessView', 0x060),
        ('nShowMemoryStatus', 0x064),
        ('nStartNavPointOverride', 0x068),
        ('nArcadeBonusCountdown', 0x06C),
        ('bMouseCursorVisible', 0x070),
        ('bInputPollingGuard', 0x074),
        ('stLastPolledFlightInput', 0x078),
        ('bMouseAfterburnerControl', 0x084),
        ('asMouseYawThresholds', 0x088),
        ('asMousePitchThresholds', 0x098),
        ('dwLastSecondaryButtonPress', 0x0A4),
        ('bFlightRollLatch', 0x0A8),
        ('bAfterburnerButtonLatched', 0x0AC),
        ('nMouseYawInput', 0x0B0),
        ('nMousePitchInput', 0x0B4),
    ), frozenset()),
    'pload': UnitData(0x0046A3A4, 147, (), frozenset()),
    'sound-data': UnitData(0x0046A438, 274, (
        ('pActiveSoundHead', 0x00),
        ('pActiveSoundTail', 0x04),
        ('bAudioSystemInitialized', 0x08),
        ('pWaveTableHead', 0x0C),
        ('pWaveTableTail', 0x10),
        ('szMusicVolumeValueName', 0x18),
        ('szSfxVolumeValueName', 0x28),
    ), frozenset(), owner='sound.obj'),
    'launch': UnitData(0x0046A550, 172, (
        ('asCarrierLaunchApproachDeltaY', 0x00),
        ('acCarrierLaunchApproachFrames', 0x30),
        ('aCarrierLaunchFighterPath', 0x48),
        ('asCarrierLaunchFighterDeltaY', 0x70),
        ('asCarrierLaunchViewData', 0x90),
    ), frozenset()),
    'install': UnitData(0x0046A5FC, 325, (), frozenset()),
    'pilot': UnitData(0x004763F0, 264, (
        ('pDebugOverlay', 0x00),
        ('abUnusedOverlayBuffer', 0x08),
    ), frozenset(), storage='.bss'),
    'sound': UnitData(0x0047654C, 8, (
        ('pSnowStaticWave', 0x00),
        ('pSnowStaticSound', 0x04),
    ), frozenset(), storage='.bss'),
    'damage': UnitData(0x0046A748, 390, (
        ('pDamageDisplayBackground', 0x00),
        ('aDamageDisplayPositions', 0x08),
        ('apszComponentNames', 0x30),
        ('abDamageDisplayFrames', 0x58),
        ('apszDamageSeverityNames', 0x68),
    ), frozenset((*range(0x30, 0x54, 4), *range(0x68, 0x7C, 4)))),
    'view': UnitData(0x0046A8D0, 289, (
        ('nScriptedViewObject', 0x00),
        ('bScriptedView', 0x04),
        ('asConstellationVelocity', 0x08),
        ('asConstellationFrame', 0x28),
        ('nConstellationDirection', 0x48),
        ('pPacketDecompressionWorkspace', 0x4C),
        ('wPacketDecompressionInputSizeOverride', 0x50),
        ('wPacketCompressionFormatFlags', 0x54),
        ('aTargetArmorClipRects', 0x58),
    ), frozenset()),
    'ending': UnitData(0x0046AD88, 457, (
        ('abTaggedAllocationPrefix', 0x00),
        ('apszCampaignVictoryText', 0x08),
        ('pszTigerClawEscapeOpening', 0x18),
        ('pszTigerClawEscapeJump', 0x1C),
        ('pszTigerClawEscapeClosing', 0x20),
        ('aCampaignVictoryProjectileOrigins', 0x28),
        ('pszTheEnd', 0x40),
        ('pszForNow', 0x44),
    ), frozenset((0x08, 0x0C, 0x10, 0x18, 0x1C, 0x20, 0x40, 0x44))),
    'input': UnitData(0x0046AF54, 6, (
        ('nInputDoubleClickInterval', 0x00),
        ('nMenuPointerSpeed', 0x04),
    ), frozenset()),
    'comm': UnitData(0x0046AF60, 520, (
        ('nCommMenuChoiceCount', 0x00),
        ('nCommMenuReuseMode', 0x04),
        ('bMessageSpeed', 0x08),
        ('cPendingCommMenuAction', 0x0C),
        ('bRadioSilence', 0x10),
        ('bVideoImagesSuppressed', 0x14),
        ('bCommVideoEnabled', 0x18),
        ('apszKilrathiAceNames', 0x20),
        ('apszCommMenuText', 0x30),
        ('cCommMenuRecipient', 0x64),
        ('nCommSpeakerObject', 0x68),
        ('nCommSpeakerRating', 0x6C),
        ('nCommPortraitIndex', 0x70),
    ), frozenset(range(0x20, 0x64, 4))),
    'video': UnitData(0x0046B168, 10, (
        ('nVideoMode', 0x00),
    ), frozenset()),
    'joycal': UnitData(0x0046B178, 42, (
        ('acGraphicsModeByAnimationSpeed', 0x00),
        ('szTMDriverName', 0x08),
        ('szSuperTMDriverName', 0x10),
    ), frozenset()),
    'game': UnitData(0x0046C000, 6796, (
        ('cViewObject', 0x0000),
        ('nEyePitchRate', 0x0004),
        ('nEyeYawRate', 0x0008),
        ('nEyeRollRate', 0x000C),
        ('DAT_0046c010', 0x0010),
        ('bCurrentKey', 0x0014),
        ('cPreviousKey', 0x0018),
        ('nCurrentWave', 0x001C),
        ('cCurrentObjective', 0x0020),
        ('bIntroSecondaryScene', 0x0024),
        ('abHazardObjects', 0x0028),
        ('nCameraViewMode', 0x003C),
        ('nExternalViewShip', 0x0040),
        ('nEjectedPilotObject', 0x0044),
        ('nClosestVisibleObject', 0x0048),
        ('nYourWingman', 0x004C),
        ('nPlayerCollisionObject', 0x0050),
        ('eSelectedGunType', 0x0054),
        ('nSelectedReleaseWeaponIndex', 0x0058),
        ('DAT_0046c05c', 0x005C),
        ('bTargetLockReadoutDirty', 0x0060),
        ('nTargetLockCountdown', 0x0064),
        ('nTargetLockMarkerAngle', 0x0068),
        ('cTargetDisplayObject', 0x006C),
        ('DAT_0046c070', 0x0070),
        ('bTargetLockAcquired', 0x0074),
        ('nTargetLockMode', 0x0078),
        ('bMissileCameraEnabled', 0x007C),
        ('bEngageAllowed', 0x0080),
        ('nAutoEngageTimer', 0x0084),
        ('cViableTargetCount', 0x0088),
        ('asIntroCameraSequence', 0x0090),
        ('asCannedSequence32', 0x00B8),
        ('asCannedSequence33', 0x00D8),
        ('asCannedSequence34', 0x00F0),
        ('asCannedSequence35', 0x0108),
        ('asCampaignVictoryViewScript', 0x0160),
        ('asCannedSequence37', 0x0190),
        ('asCannedSequence38', 0x01B0),
        ('asCannedSequence39', 0x01D0),
        ('asCannedSequence40', 0x01E0),
        ('asCannedSequence41', 0x0230),
        ('asTigerClawEscapeViewScript', 0x0238),
        ('asCannedSequence42', 0x0250),
        ('asCannedSequence43', 0x0270),
        ('asCannedSequence44', 0x02A0),
        ('asCannedSequence45', 0x02E8),
        ('aMissionNavPoints', 0x02F0),
        ('aMissionShips', 0x0948),
        ('aiIntelligenceEvent', 0x1368),
        ('aDefenseManeuversNovice', 0x1390),
        ('aDefenseManeuversVeteran', 0x1398),
        ('aDefenseManeuversElite', 0x13A0),
        ('aDefenseManeuversAce', 0x13B0),
        ('aDefenseManeuversBoss', 0x13C0),
        ('apDefenseManeuvers', 0x13D0),
        ('aRatedManeuverChoices', 0x13E8),
        ('aKilrathiManeuverChoices', 0x1808),
        ('acPilotAggression', 0x19A0),
        ('acPilotRecovery', 0x19B8),
    ), frozenset((
        0x1030, 0x1066, 0x109C, 0x10D2, 0x113E, 0x1174,
        0x11AA, 0x11E0, 0x1216, 0x124C, 0x1282, 0x12B8,
        0x12EE, 0x13D0, 0x13D4, 0x13D8, 0x13DC, 0x13E0,
    )), owner='game.obj'),
    'data': UnitData(0x0046DA90, 1724, (
        ('pInputEventHead', 0x000),
        ('pInputEventTail', 0x004),
        ('bInputEventPoolInitialized', 0x008),
        ('pDrawnMouseCursorShape', 0x00C),
        ('nMouseCursorShowCount', 0x010),
        ('nScreenWidth', 0x014),
        ('nScreenHeight', 0x018),
        ('DAT_0046daac', 0x01C),
        ('DAT_0046dab0', 0x020),
        ('DAT_0046dab4', 0x024),
        ('aScreenViewportGeometry', 0x028),
        ('acDirectionShapeFrame', 0x098),
        ('acDirectionShapeFlip', 0x158),
        ('aasCockpitLightX', 0x218),
        ('aasCockpitLightY', 0x260),
        ('aacCockpitLightOffFrame', 0x2A8),
        ('aacCockpitLightOnFrame', 0x2D0),
        ('aaCockpitBars', 0x2F8),
        ('stCockpitLayout', 0x578),
        ('asPilotHandOrigins', 0x690),
        ('DAT_0046e138', 0x6A8),
    ), frozenset(), owner='data.obj'),
    'screens': UnitData(0x0046E188, 1298, (
        ('stSavedCampaignDate', 0x000),
        ('aTalkingHeadOrigins', 0x008),
        ('abBriefingSmallCharacterAnimation', 0x060),
        ('abBriefingLargeCharacterAnimation', 0x078),
        ('aBriefingCharacters', 0x090),
        ('asMedalDisplayX', 0x148),
        ('apszMedalNames', 0x158),
        ('pMedalSceneShape', 0x16C),
        ('pszMedalsPilotSummary', 0x170),
        ('aBriefingPortraitOffsetX', 0x178),
        ('aBriefingPortraitOffsetY', 0x1D8),
        ('aBriefingPortraitScale', 0x238),
        ('aiBriefingLeftPanelVelocity', 0x2F8),
        ('aiBriefingPodiumVelocity', 0x328),
        ('aiBriefingRightPanelVelocity', 0x358),
        ('abBriefingPodiumFrames', 0x388),
        ('abDebriefingEstablishDeltas', 0x3B0),
        ('nDebriefingLeftX', 0x3E4),
        ('nDebriefingPilotX', 0x3E8),
        ('nDebriefingRightX', 0x3EC),
        ('nDebriefingOfficerX', 0x3F0),
        ('nDebriefingPodiumX', 0x3F4),
        ('nConversationCharacter', 0x3F8),
        ('nTalkingHeadFace', 0x3FC),
        ('nConversationBackdropFrame', 0x400),
        ('bConversationConstellation', 0x404),
        ('bConversationOverlay', 0x408),
    ), frozenset((*range(0x09D, 0x11C, 18), *range(0x158, 0x16C, 4), 0x170))),
    'raster': UnitData(0x0046E69C, 6640, (
        ('apRasterDriverCallbacks', 0x0000),
        ('szRasterDriverName', 0x0034),
        ('pRLEEncodeBuffer', 0x0041),
        ('nRLEPendingSkip', 0x0045),
        ('pRLEScanlineStart', 0x0049),
        ('pRLEScanCursor', 0x004D),
        ('pRLEOutputCursor', 0x0051),
        ('pRLELiteralStart', 0x0055),
        ('nRLEEncodedMinimumX', 0x0069),
        ('nRLEEncodedMinimumY', 0x006D),
        ('nRLEEncodedMaximumX', 0x0071),
        ('nRLEEncodedMaximumY', 0x0075),
        ('abRasterDecodeBuffer', 0x0579),
        ('abRasterPaletteUsageScratch', 0x1279),
        ('abRasterFadeAccumulator', 0x1579),
        ('abGifLZWCodeMasks', 0x1879),
        ('abGifInterlaceRowSteps', 0x1882),
        ('abGifInterlaceRowStarts', 0x1887),
        ('pGifRasterTarget', 0x188C),
        ('abRasterPaletteTranslation', 0x1890),
        ('aRLETransformVertices', 0x1990),
        ('anRLESourceSteps', 0x19E0),
    ), frozenset()),
    'killbrd': UnitData(0x00470090, 1508, (
        ('pszBlankRoomMenuLabel', 0x000),
        ('apszPilotRankNames', 0x008),
        ('stInitialCampaignState', 0x020),
        ('aInitialPilotRecords', 0x078),
        ('abSpiritRecRoomAnimation', 0x1D0),
        ('abHunterRecRoomAnimation', 0x200),
        ('abAngelRecRoomAnimation', 0x228),
        ('abKnightRecRoomAnimation', 0x258),
        ('abIcemanRecRoomAnimation', 0x280),
        ('abManiacRecRoomAnimation', 0x2A8),
        ('abPaladinRecRoomAnimation', 0x2C0),
        ('abBossmanRecRoomAnimation', 0x2F8),
        ('abShotglassIdleAnimation', 0x328),
        ('abShotglassGlassAnimation', 0x340),
        ('abShotglassPourAnimation', 0x360),
        ('abShotglassWipeAnimation', 0x3B0),
        ('apRecRoomAnimations', 0x3C8),
        ('aRecRoomCharacterOrigins', 0x400),
        ('stRecRoomPanelOrigin', 0x40C),
        ('aRecRoomMenuRegions', 0x410),
        ('apszRecRoomBaseLabels', 0x458),
        ('apszRecRoomMenuLabels', 0x468),
        ('bPanRoomTransition', 0x480),
        ('stChalkBoardDate', 0x484),
        ('asChalkBoardPilotOrder', 0x488),
    ), frozenset((0x000, *range(0x008, 0x01C, 4),
                  *range(0x3C8, 0x3E8, 4), *range(0x3EC, 0x3FC, 4),
                  0x458, 0x45C, 0x460, 0x474, 0x478, 0x47C))),
    # The absolute cosine and sine rows form one table; its sine row starts
    # at 0x3D0 (0x00470a48).
    'gr': UnitData(0x00470678, 2030, (
        ('abPaletteTranslation', 0x000),
        ('aawAbsoluteTrig', 0x100),
    ), frozenset()),
    'gr-rdata': UnitData(0x004631C8, 56, (
        ('szGameRegistryKey', 0x00),
        ('szCheaterRegistryValue', 0x30),
    ), frozenset(), storage='.rdata', owner='gr.obj'),
    'gr-bss': UnitData(0x004875A8, 1114664, (
        ('stRasterTarget', 0x00),
        ('abShapeTransformPixels', 0x18),
        ('stRasterClip', 0xFA18),
        ('abSolidColourLookup', 0x100A0),
        ('abShapeRLEBuffer', 0x101A0),
        ('bStreamerAudioRunning', 0x1101A0),
        ('szStreamerPathBuffer', 0x1101A8),
    ), frozenset(), storage='.bss', owner='gr.obj'),
    # The angular constants form mathutil's complete read-only section,
    # between barracks' menu regions and dib's DirectDraw GUIDs.
    'mathutil-angular': UnitData(0x004630DC, 12, (
        ('fPi', 0x00),
        ('fDegreesPerRadian', 0x04),
        ('fRadiansPerDegree', 0x08),
    ), frozenset(), storage='.rdata', owner='mathutil.obj'),
    # The static .bss of each unit, in link order from winsys.c to gr.c.
    # Unreferenced statics occupy their slots but emit no COFF symbol, so
    # the section sizes and the following symbols' offsets cover them.
    'winsys-bss': UnitData(0x00475B68, 440, (
        ('nInsertKeyDown', 0x00),
        ('nClearedKeyboard', 0x04),
        ('nOemPeriodKeyStatus', 0x88),
        ('nHomeState', 0x8C),
        ('nPageDownKeyHeld', 0x90),
        ('nOemCommaKeyPressed', 0x94),
        ('nRightArrowKeyHeld', 0x98),
        ('nEndKeyDown', 0x9C),
        ('nClearKeyState', 0xA0),
        ('nDeleteKeyDown', 0xA4),
        ('nUpArrowKeyState', 0xA8),
        ('nDownArrowKeyState', 0xAC),
        ('szStreamsDir', 0xB0),
        ('nPageUpKeyState', 0x1B0),
        ('nLeftArrowKeyState', 0x1B4),
    ), frozenset(), storage='.bss', owner='winsys.obj'),
    'mono-bss': UnitData(0x00475D20, 344, (
        ('szDataFileReadError', 0x00),
        ('szDataCreateError', 0x40),
        ('szWriteDataError', 0x80),
        ('szSeekDataError', 0xC0),
        ('bMonoDebugAvailable', 0x150),
        ('hMonoDebugDriver', 0x154),
    ), frozenset(), storage='.bss', owner='mono.obj'),
    'main-bss': UnitData(0x004764F8, 84, (
        ('nFlightFrameTicks', 0x00),
        ('nHudDelta', 0x04),
        ('liFlightAfterInputCounter', 0x08),
        ('liFlightFrameEndCounter', 0x10),
        ('nFlightPresentDelta', 0x18),
        ('liFrameStart', 0x20),
        ('liSimulationDone', 0x28),
        ('nSimulationDelta', 0x30),
        ('liFlightAfterCockpitCounter', 0x38),
        ('nInputTicks', 0x40),
        ('liRenderDoneCounter', 0x48),
        ('nDrawDelta', 0x50),
    ), frozenset(), storage='.bss', owner='main.obj'),
    'dib-bss': UnitData(0x00476620, 65416, (
        ('szFrameRateLabel', 0x00),
        ('DAT_00476640', 0x20),
        ('DAT_00476644', 0x24),
        ('pDIBImage', 0x28),
        ('nDIBFrameWidth', 0x2C),
        ('nDIBFrameHeight', 0x30),
        ('abDIBPixelBackup', 0x38),
        ('DAT_00486058', 0xFA38),
        ('nDIBSlamTotal', 0xFA40),
        ('hDIBParentWindow', 0xFA54),
        ('szDIBFailureString', 0xFA58),
        ('abDIBPaletteQuads', 0xFAF0),
        ('fSampledFps', 0xFEF0),
        ('bPendingDIBSlam', 0xFEF8),
        ('szSpaceFlightFpsCapMessage', 0xFF08),
    ), frozenset(), storage='.bss', owner='dib.obj'),
    'damage-bss': UnitData(0x00476554, 4, (
        ('nDisplayedComponentDamage', 0x00),
    ), frozenset(), storage='.bss', owner='damage.obj'),
    'music-bss': UnitData(0x00476558, 200, (
        ('szSfxWavePath', 0x00),
    ), frozenset(), storage='.bss', owner='music.obj'),
    'eventmgr-bss': UnitData(0x004865A8, 4096, (
        ('abCursorSaveArea', 0x00),
    ), frozenset(), storage='.bss', owner='eventmgr.obj'),
    'nav-bss': UnitData(0x00475E80, 368, (
        ('aNavMapLabels', 0x00),
        ('aNavMapExclusionRects', 0xC8),
    ), frozenset(), storage='.bss', owner='nav.obj'),
    'cockpt-bss': UnitData(0x00475FF0, 1024, (
        ('abMouseCursorBackground', 0x00),
    ), frozenset(), storage='.bss', owner='cockpt.obj'),
    'ix-log-bss': UnitData(0x005977D0, 1024, (
        ('szLogBuf', 0x000),
    ), frozenset(), storage='.bss', owner='ix/ixlog.obj'),
    # Uninitialized C++ globals follow the same identifier-hash order as C
    # statics; the word at +0x008 is unreferenced.
    'ix-streamer-bss': UnitData(0x00597BD0, 300, (
        ('pStreamerPacketOffsetTable', 0x000),
        ('dwStreamThreadTick', 0x004),
        ('dwUnusedStreamerWord', 0x008),
        ('pStreamerPackFile', 0x00C),
        ('pStreamIdleFiles', 0x010),
        ('adwStreamCallStack', 0x018),
        ('pStreamerLzoBuffer', 0x098),
        ('pStreamReadRequests', 0x09C),
        ('dwIxStreamerAudioBufferSize', 0x0A0),
        ('pIxStreamerInputBuffer', 0x0A4),
        ('bStreamerIntensityLevel', 0x0A8),
        ('hStreamThread', 0x0AC),
        ('pStreamFileEntries', 0x0B0),
        ('pStreamerPackageInfo', 0x0B4),
        ('pStreamedMusicAudioChunks', 0x0B8),
        ('pMusicStreamBranchList', 0x0BC),
        ('pStreamFileBlocks', 0x0C0),
        ('csStreamerFileList', 0x0C8),
        ('csIxStreamerThread', 0x0E0),
        ('nStreamerAudioBlock', 0x0F8),
        ('dwIxStreamerThreadId', 0x0FC),
        ('dwIxStreamerStatus', 0x100),
        ('hStreamerWakeupEvent', 0x104),
        ('nIxStreamerCurrentFileChunk', 0x108),
        ('dwStreamBytesPerSecond', 0x10C),
        ('csStreamer', 0x110),
        ('pStreamTriggerList', 0x128),
    ), frozenset(), storage='.bss', owner='ix/streamer.obj'),
    'ix-dsp-bss': UnitData(0x00597D00, 1072, (
        ('hIxPlaybackThread', 0x000),
        ('hPlaybackWindow', 0x004),
        ('hAudioWakeEvent', 0x008),
        ('dwMixerWriteOffset', 0x00C),
        ('pDspDevice', 0x010),
        ('dwSoundBufferSize', 0x014),
        ('dwDspFlags', 0x018),
        ('dwMixerThreadId', 0x01C),
        ('pMixerBuffer', 0x020),
        ('asPanGains', 0x028),
        ('dwDspTick', 0x428),
        ('pDspPrimaryBuffer', 0x42C),
    ), frozenset(), storage='.bss', owner='ix/dsp.obj'),
    'ix-dsps-bss': UnitData(0x00598130, 120, (
        ('nStreamCount', 0x00),
        ('nAllocatedStreams', 0x04),
        ('streams', 0x08),
    ), frozenset(), storage='.bss', owner='ix/dsps.obj'),
    'ix-dspv-bss': UnitData(0x005981A8, 1120, (
        ('aVoices', 0x000),
        ('csMixer', 0x440),
        ('nVoiceCount', 0x458),
        ('nVoicesAllocated', 0x45C),
    ), frozenset(), storage='.bss', owner='ix/dspv.obj'),
    'ix-system-bss': UnitData(0x00598608, 28, (
        ('dwSoundStateFlags', 0x00),
        ('pIdleSoundList', 0x04),
        ('pSamples', 0x08),
        ('nActiveVoices', 0x0C),
        ('nSystemVoices', 0x10),
        ('pPlayingSoundList', 0x14),
        ('pQueuedSoundList', 0x18),
    ), frozenset(), storage='.bss', owner='ix/system.obj'),
}


ADJACENT_UNITS = (
    ('winmain', 'winsys', 'mono', 'auto', 'eject', 'cmpgn', 'fl',
     'cutscene', 'brains', 'mission', 'objdata', 'nav', 'nearheap',
     'gameflow', 'title', 'joystick', 'spc', 'cockpt'),
    ('cockpt', 'version', 'barracks', 'fade', 'debug', 'disk', 'ship',
     'init', 'logic', 'pilot-data', 'trainsim'),
    ('trainsim', 'main', 'pload', 'sound-data', 'launch', 'install',
     'damage', 'view'),
    ('game', 'data'),
    ('barracks', 'fade', 'debug', 'disk', 'ship', 'init', 'logic'),
    ('damage', 'view'),
    ('ending', 'input', 'comm', 'video', 'joycal'),
    ('killbrd', 'gr'),
    ('screens', 'raster', 'killbrd', 'gr'),
    ('winsys-bss', 'mono-bss', 'fl-bss', 'nav-bss', 'cockpt-bss', 'pilot',
     'main-bss', 'sound', 'damage-bss', 'music-bss', 'dib-bss',
     'eventmgr-bss', 'gr-bss', 'ix-log-bss', 'ix-streamer-bss',
     'ix-dsp-bss', 'ix-dsps-bss', 'ix-dspv-bss', 'ix-system-bss'),
)


def require(condition: bool, message: str) -> None:
    if not condition:
        raise ValueError(message)


def data_section_size(path: str, storage: str) -> int:
    data = Path(path).read_bytes()
    require(len(data) >= 20, f'{path}: truncated COFF header')
    machine, sections, _, _, _, optional, _ = struct.unpack_from('<HHIIIHH', data)
    require(machine == 0x14C, f'{path}: expected an i386 COFF object')
    require(len(data) >= 20 + optional + sections * 40,
            f'{path}: truncated COFF section table')
    sizes = []
    for index in range(sections):
        offset = 20 + optional + index * 40
        if data[offset:offset + 8].rstrip(b'\0').decode('ascii') == storage:
            size, raw_pointer, reloc_pointer = struct.unpack_from('<III', data, offset + 16)
            reloc_count = struct.unpack_from('<H', data, offset + 32)[0]
            flags = struct.unpack_from('<I', data, offset + 36)[0]
            if storage == '.bss':
                require(flags & 0xC0 == 0x80 and raw_pointer == 0 and
                        reloc_pointer == 0 and reloc_count == 0,
                        f'{path}: .bss must contain uninitialized storage only')
            else:
                require(flags & 0xC0 == 0x40,
                        f'{path}: {storage} must contain initialized storage')
                if storage == '.rdata':
                    require(flags & 0xC0000000 == 0x40000000,
                            f'{path}: .rdata must be readable and not writable')
            sizes.append(size)
    require(len(sizes) == 1, f'{path}: expected one {storage} section')
    return sizes[0]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--original', default='data/full/WC1.ORI.EXE')
    parser.add_argument('--rebuilt', default='WC1.EXE')
    parser.add_argument('--map', dest='map_path', default='WC1.map')
    parser.add_argument('--objects-root', type=Path, default=Path('out'))
    parser.add_argument('--unit', choices=UNITS, action='append')
    args = parser.parse_args()

    original = PEImage(args.original)
    rebuilt = PEImage(args.rebuilt)
    symbols = parse_msvc_map_symbols(args.map_path)

    def address(owner: str | None, name: str) -> int:
        matches = [entry.va for entry in symbols
                   if normalize_compiled(entry.symbol) == name
                   and (owner is None or
                        (entry.object_file or '').split(':')[-1] == owner)]
        require(len(matches) == 1, f'expected one {owner}:{name} in map')
        return matches[0]

    starts = {}
    for unit in args.unit or UNITS:
        block = UNITS[unit]
        object_path = args.objects_root / (block.owner or unit + '.obj')
        owner = object_path.name
        if block.globals:
            start = address(owner, block.globals[0][0])
        else:
            sections, _, bases = recover_layout(object_path, symbols, rebuilt)
            matches = [bases[index] for index, section in sections.items()
                       if section[0] == block.storage and index in bases]
            require(len(matches) == 1,
                    f'{owner}: cannot resolve the {block.storage} section base')
            start = matches[0]
        starts[unit] = start
        section_size = block.size if block.section_size is None else block.section_size
        require(section_size >= block.size,
                f'{unit}: verified span exceeds its containing section')
        require(data_section_size(str(object_path), block.storage) == section_size,
                f'{owner}: {block.storage} must contain exactly {section_size} bytes')
        for name, offset in block.globals:
            require(address(owner, name) == start + offset,
                    f'{name}: incorrect offset within {unit} data')

        for image, base in ((original, block.original_start), (rebuilt, start)):
            section = image.section_for_va(base)
            pe_storage = '.data' if block.storage == '.bss' else block.storage
            require(section is not None and section.name == pe_storage and
                    base + block.size <= section.start + section.virtual_size,
                    f'{image.path}: {unit} block must occupy the {pe_storage} section')
            if block.storage != '.bss':
                require(base + block.size <= section.start + section.rawsize,
                        f'{image.path}: {unit} block must occupy initialized {pe_storage}')
            offsets = {va - base for va in image.relocations()
                       if base <= va < base + block.size}
            require(offsets == block.pointers,
                    f'{image.path}: incorrect HIGHLOW relocations for {unit}')

        original_bytes = original.read(block.original_start, block.size)
        rebuilt_bytes = rebuilt.read(start, block.size)
        require(original_bytes is not None and rebuilt_bytes is not None,
                f'cannot read {unit} data')
        ranges = (RelocatedAddressRange(block.original_start,
                                       block.original_start + block.size, start),)
        ranges += tuple(RelocatedAddressRange(base, base + size, address(None, name))
                        for name, base, size in block.external_globals)
        # Do not supply PE images: string-content fallback would accept a
        # pointer to another, byte-identical literal at an incorrect offset.
        if block.pointers:
            matches = relocated_pointer_match(original_bytes, rebuilt_bytes, {}, ranges)
        else:
            matches = AddressComparison(block.original_start, block.size, start,
                                        original_bytes, rebuilt_bytes).matches
        require(matches, f'{unit} data differs after its single permitted address shift')
        for offset in sorted(block.pointers):
            require(relocated_pointer_match(original_bytes[offset:offset + 4],
                                            rebuilt_bytes[offset:offset + 4], {}, ranges),
                    f'{unit} pointer at +0x{offset:02X} has an incorrect target')
        scope = ' (partial section)' if block.section_size is not None else ''
        print(f'{owner}{scope}: {block.size} {block.storage} bytes match with one address shift; '
              f'{len(block.globals)} globals and {len(block.pointers)} '
              'pointer relocations verified')
    for group in ADJACENT_UNITS:
        if not all(unit in starts for unit in group):
            continue
        first, last = UNITS[group[0]], UNITS[group[-1]]
        original_start = first.original_start
        rebuilt_start = starts[group[0]]
        size = last.original_start + last.size - original_start
        for unit in group:
            require(starts[unit] - rebuilt_start ==
                    UNITS[unit].original_start - original_start,
                    f'{unit}: incorrect position in adjacent data run')
        pointers = {UNITS[unit].original_start - original_start + offset
                    for unit in group for offset in UNITS[unit].pointers}
        for image, base in ((original, original_start), (rebuilt, rebuilt_start)):
            offsets = {va - base for va in image.relocations()
                       if base <= va < base + size}
            require(offsets == pointers,
                    f'{image.path}: incorrect relocations in adjacent data run')
        ranges = (RelocatedAddressRange(original_start, original_start + size,
                                       rebuilt_start),)
        ranges += tuple(RelocatedAddressRange(base, base + length, address(None, name))
                        for unit in group
                        for name, base, length in UNITS[unit].external_globals)
        original_bytes = original.read(original_start, size)
        rebuilt_bytes = rebuilt.read(rebuilt_start, size)
        require(original_bytes is not None and rebuilt_bytes is not None,
                'cannot read adjacent data run')
        if pointers:
            matches = relocated_pointer_match(original_bytes, rebuilt_bytes, {}, ranges)
        else:
            matches = AddressComparison(original_start, size, rebuilt_start,
                                        original_bytes, rebuilt_bytes).matches
        require(matches,
                'adjacent data run differs, including inter-object alignment bytes')
        padding = size - sum(UNITS[unit].size for unit in group)
        print(f'{group[0]} through {group[-1]}: {size} adjacent data bytes match; '
              f'{len(group)} unit positions and {padding} alignment bytes verified')
    return 0


if __name__ == '__main__':
    try:
        sys.exit(main())
    except (OSError, ValueError) as error:
        print(f'unit data verification failed: {error}', file=sys.stderr)
        sys.exit(1)
