/*
 *  Globals shared by the reconstruction.
 *
 *  Names carry no address (AGENTS.md): a global renamed from DAT_<addr> to
 *  <hungarian><Name> keeps its address in the trailing 0x004xxxxx comment on
 *  this declaration and on its definition, so it stays checkable against the
 *  original image without cluttering the use sites.
 *
 *  These live in one place only because the game-core compilation-unit
 *  boundaries are still unknown; each global belongs in whichever module
 *  defines it, and moves there as docs/ORDER.md is filled in.
 */
#ifndef WC1_GLOBALS_H
#define WC1_GLOBALS_H

extern const int anRLEQuarterCosine[901]; /* 0x0043d4bf */
extern const unsigned char abIffBmhdChunkId[4]; /* 0x0043e778 */
extern const unsigned char abIffCmapChunkId[4]; /* 0x0043e77c */
extern const unsigned char abIffBodyChunkId[4]; /* 0x0043e780 */
extern TitleMenuRegion aBarracksMenuRegions[21]; /* 0x00463008 */
extern short nHazardReferenceSpeed; /* 0x00465040 */
extern short nActiveHazards; /* 0x00465044 */
extern int bIxAudioEnabled; /* 0x00465058 */
extern unsigned int bKeyEventQueueEnabled; /* 0x0046505c */
extern int bShowKilrathiSagaCredits; /* 0x0046506c */
extern unsigned int nGuardedAllocationTotalBytes; /* 0x00465060 */
extern unsigned int nGuardedAllocationBytes; /* 0x00465064 */
extern unsigned int nGuardedAllocationPeakBytes; /* 0x00465068 */
extern int bShowFrameRate; /* 0x00465070 */
extern int bDirectDrawModeCascadeEnabled; /* 0x00465074 */
extern int bCockpitEnabled; /* 0x0046507c */
extern int bMainWindowMinimized; /* 0x00465080 */
extern int bWindowMessagePumpActive; /* 0x004650a8 */
extern unsigned int bF1KeyLatch; /* 0x004650ac */
extern GuardedAllocation *pGuardedAllocationHead; /* 0x004650b0 */
extern const char szJoystickDevCapsFailure[26]; /* 0x004652dc */
extern short nPacketError; /* 0x00465460 */
extern short nAutopilotFormationShipCount; /* 0x00465544 */
extern const short asEjectionPrimaryFrames[8]; /* 0x00465550 */
extern const short asEjectionSecondaryFrames[8]; /* 0x00465560 */
extern const short asEjectionViewScript[12]; /* 0x00465570 */
extern const char *pStrandedMessage; /* 0x00465588 */
extern const char szStrandedMessage[72]; /* 0x0046558c */
extern const char szViewTemplates[16]; /* 0x004655d4 */
extern const char szStrandedTheEnd[12]; /* 0x004655e4 */
extern const short asMouthFramesByPhoneme[26]; /* 0x004655f0 */
extern const char szConversationIntegerFormat[4]; /* 0x00465628 */
extern const char szConversationIntegerFormatAlt[4]; /* 0x0046562c */
extern const char szCampaignDateFormat[12]; /* 0x00465630 */
extern const char szSavedCampaignDateFormat[12]; /* 0x0046563c */
extern const char szCampaignTimeFormat[12]; /* 0x00465648 */
extern const char szConversationTextFormat[12]; /* 0x00465654 */
extern const char szBriefingMapTextFormat[12]; /* 0x00465660 */
extern const char szCloseLookTextFormat[12]; /* 0x0046566c */
extern signed char cScrambleLeftWalkerFrame; /* 0x00465768 */
extern signed char cScrambleRightWalkerFrame; /* 0x0046576c */
extern signed char acScrambleWalkerOverlayFrames[8]; /* 0x00465770 */
extern short nScrambleLeftWalkerX; /* 0x00465778 */
extern short nScrambleRightWalkerX; /* 0x0046577c */
extern short nScrambleBackgroundX; /* 0x00465780 */
extern const unsigned short ausScrambleActorAnimationA[8]; /* 0x00465788 */
extern const unsigned short ausScrambleActorAnimationB[11]; /* 0x00465798 */
extern ScrambleAnimationActor aScrambleAnimationActors[5]; /* 0x004657b0 */
extern const ScrambleShipDetail aaScrambleShipDetails[4][32]; /* 0x00465828 */
extern const int anLandingDamageDetailCounts[4]; /* 0x00465aa8 */
extern const char *apszLandingDamageComments[4]; /* 0x00465ab8 */
extern const signed char acLandingCanopyFramesHeavy[24]; /* 0x00465ac8 */
extern const signed char acLandingCanopyFramesLight[24]; /* 0x00465ae0 */
extern const signed char acLandingCanopyFramesModerate[16]; /* 0x00465af8 */
extern const signed char *apLandingCanopyFrames[4]; /* 0x00465b08 */
extern const ShortPoint aFuneralParticleOrigins[7]; /* 0x00465b18 */
extern const short asFuneralSceneBySeries[15]; /* 0x00465b36 */
extern int bFuneralShowTheEnd; /* 0x00465b54 */
extern const char szLandingCleanComment[32]; /* 0x00465b58 */
extern const char szLandingLightDamageComment[48]; /* 0x00465b78 */
extern const char szLandingHeavyDamageComment[36]; /* 0x00465ba8 */
extern const char szLandingSurvivalComment[44]; /* 0x00465bcc */
extern const char szLandingCommentFormat[12]; /* 0x00465bf8 */
extern const char szFuneralTheEnd[8]; /* 0x00465c04 */
extern const char szFuneralTextFormat[12]; /* 0x00465c0c */
extern const char szFuneralCompanyCommand[20]; /* 0x00465c18 */
extern const char szFuneralAttentionCommand[20]; /* 0x00465c2c */
extern const char szFuneralPrepareArmsCommand[20]; /* 0x00465c40 */
extern const char szFuneralFireCommand[16]; /* 0x00465c54 */
extern short nEnemySighting; /* 0x00465c7c */
extern short nDebriefingPersonality; /* 0x00465c80 */
extern void *aapPacketReferences[4][0x25]; /* 0x00465c88 */
extern int bInitialFormationSetup; /* 0x00465c84 */
extern const ShortVector aaFormationPositions[5][8]; /* 0x00465ed8 */
extern const int anPilotTurnInterval[18]; /* 0x00465fc8 */
extern const char szErrorLoadingPilotSpeech[32]; /* 0x00466010 */
extern short nNavMapCoordinateScaling; /* 0x00468660 */
extern short nNavMapScale; /* 0x00468664 */
extern NavMapObjectiveStyle aNavMapObjectiveStyles[10]; /* 0x00468668 */
extern unsigned char *pNavMapShape; /* 0x00468708 */
extern short nNavMapLabelCount; /* 0x0046870c */
extern short nNavMapReservedAreaCount; /* 0x00468710 */
extern char szCampaignSector[16]; /* 0x00468718 */
extern char *apszShipMissionTypeNames[11]; /* 0x00468728 */
extern char szMissionPatrol[8]; /* 0x00468758 */
extern char szMissionEscort[8]; /* 0x00468760 */
extern char szMissionStrike[8]; /* 0x00468768 */
extern char szMissionDefend[8]; /* 0x00468770 */
extern char szMissionWingman[8]; /* 0x00468778 */
extern char szMissionFlee[8]; /* 0x00468780 */
extern char szMissionGotoWarp[12]; /* 0x00468788 */
extern char szMissionWarpArriveError[4]; /* 0x00468794 */
extern char szMissionCannedSequenceError[4]; /* 0x00468798 */
extern char szMissionRendezvous[12]; /* 0x0046879c */
extern char szMissionComeHomeError[4]; /* 0x004687a8 */
extern char szNavLabelTextFormat[12]; /* 0x004687ac */
extern char szNavAsteroids[12]; /* 0x004687b8 */
extern char szNavMines[8]; /* 0x004687c4 */
extern char szInflightTimeFieldFormat[8]; /* 0x004687cc */
extern char szStandardTimeFormat[24]; /* 0x004687d4 */
extern char szStandardTimeBlank[4]; /* 0x004687ec */
extern char szInflightTimeSuffixFormat[8]; /* 0x004687f0 */
extern char szStandardTimeColon[4]; /* 0x004687f8 */
extern char szNavLegendNewline[4]; /* 0x004687fc */
extern char szNavMissionFlightPath[20]; /* 0x00468800 */
extern char szNavHomeBase[12]; /* 0x00468814 */
extern char szNavBlankLine[4]; /* 0x00468820 */
extern char szNavTitleFormat[8]; /* 0x00468824 */
extern char szNavSectorFormat[12]; /* 0x0046882c */
extern char szNavSystemFormat[16]; /* 0x00468838 */
extern char szNavMissionFormat[8]; /* 0x00468848 */
extern char szNavShipFormat[8]; /* 0x00468850 */
extern char szNavNotesHeading[8]; /* 0x00468858 */
extern char szNavNoteFormat[4]; /* 0x00468860 */
extern char szNavLocationFormat[48]; /* 0x00468864 */
extern char szNavViewportName[8]; /* 0x00468894 */
extern char szBriefingNavMapTitle[20]; /* 0x0046889c */
extern char szConfedNavScan[16]; /* 0x004688b0 */
extern int bInflightComputerActive; /* 0x00468754 */
extern volatile short nNearHeapActive; /* 0x004688c0 */
extern volatile short nNearHeapMaxDescriptors; /* 0x004688c4 */
extern volatile int nNearHeapRelocationBytes; /* 0x004688c8 */
extern int bOfficeVisitPending; /* 0x004688cc */
extern int bPromotionPending; /* 0x004688d0 */
extern int bPlayerEjectedThisMission; /* 0x004688d4 */
extern int bPlayerShipTypeChanged; /* 0x004688d8 */
extern short nPreviousPlayerShipType; /* 0x004688dc */
extern int bCampaignActive; /* 0x004688f0 */
extern short nPendingMedalIndex; /* 0x004688e4 */
extern short nPostSeriesSequence; /* 0x004688e8 */
extern unsigned short bSeriesFailed; /* 0x004688ec */
extern char *pszIntroOpeningText; /* 0x00468910 */
extern int nIntroCreditCount; /* 0x00468a30 */
extern char *apszIntroCredits[20]; /* 0x00468a38 */
extern char szCalculating[12]; /* 0x0046931c */
extern char szRangeKilometresSuffix[4]; /* 0x00469328 */
extern char szNoObjective[8]; /* 0x0046932c */
extern char szUnknownObjective[8]; /* 0x00469334 */
extern char szCompNavigation[16]; /* 0x0046933c */
extern char szDestinationFormat[20]; /* 0x0046934c */
extern char szNavigationRangeLabel[12]; /* 0x00469360 */
extern char szNewObjectivePrompt[20]; /* 0x0046936c */
extern TitleMenuRegion aTitleMenuRegions[5]; /* 0x00468a88 */
extern PacketResourceDescriptor aIntroResourceDescriptors[3]; /* 0x00468ac0 */
extern int bTitleMenuSceneInitialized; /* 0x00468ad8 */
extern short asVduSelectionSound[2]; /* 0x00469000 */
extern char *pszPendingHudMessage; /* 0x00469004 */
extern char *pszDisplayedHudMessage; /* 0x00469008 */
extern unsigned char cRenderedPilotHandFrame; /* 0x0046900c */
extern short asPilotHandOffsets[34]; /* 0x00469018 */
extern unsigned char *pCockpitExplosionBackground; /* 0x00469060 */
extern unsigned char * volatile pCockpitExplosionShape; /* 0x00469064 */
extern volatile short nCockpitExplosionFrame; /* 0x00469068 */
extern unsigned char *pReleaseWeaponDisplayBackground; /* 0x0046906c */
extern signed char cReleaseWeaponDisplayFrame; /* 0x00469070 */
extern signed char cReleaseWeaponDisplayTicks; /* 0x00469074 */
extern signed char cReleaseWeaponDisplayState; /* 0x00469078 */
extern char *pszObjectiveStatusMessage; /* 0x0046908c */
extern short nScannerTargetObject; /* 0x00469090 */
extern int nDisplayedObjectiveRange; /* 0x00469088 */
extern int aiScannerGridRows[79]; /* 0x00469098 */
extern short nTargetLockMarkerX; /* 0x004691f4 */
extern ShortRect stTargetBracketBounds; /* 0x004691f8 */
extern ShortRect stPreviousTargetBracketBounds; /* 0x00469200 */
extern short nNavPointerObject; /* 0x00469208 */
extern Viewport stTrainSimVduSource; /* 0x00469210 */
extern unsigned char *pConfedCommBackground; /* 0x00469278 */
extern unsigned char *pCommStaticShape; /* 0x0046927c */
extern unsigned char *pKilrathiCommBackground; /* 0x00469280 */
extern int nCommPortraitFrame; /* 0x00469284 */
extern int bForceDamageDisplayRedraw; /* 0x00469288 */
extern char szObjectiveStatusMessage[28]; /* 0x0046928c */
extern char szMissileLocked[16]; /* 0x004692a8 */
extern char szAlreadyNear[16]; /* 0x004692b8 */
extern char szEnemyNear[12]; /* 0x004692c8 */
extern char szHazardNear[12]; /* 0x004692d4 */
extern char szComponentHitFormat[8]; /* 0x004692e0 */
extern char szAlreadyVisited[16]; /* 0x00469380 */
extern char szObjectiveReached[20]; /* 0x00469390 */
extern char szWaitForFormat[12]; /* 0x004693a4 */
extern unsigned int dwModalBoundsTopLeft; /* 0x00469440 */
extern unsigned int dwModalBoundsBottomRight; /* 0x00469444 */
extern ModalTextPanel *pModalTextPanel; /* 0x00469448 */
extern const short asCampaignPilotFiles[3]; /* 0x00469450 */
extern const short asCampaignBriefingFiles[3]; /* 0x00469458 */
extern const short asMissionDataFiles[3]; /* 0x00469460 */
extern unsigned char cEmptyHudMessage; /* 0x004693b0 */
extern char *pGameVersion; /* 0x004693b4 */
extern char szGameVersion[9]; /* 0x004693b8 */
extern const ShortPoint aBarracksBunkOrigins[8]; /* 0x004693c8 */
extern const char *apszSaveCampaignMenuLabels[2]; /* 0x004693e8 */
extern char *apszBarracksMenuLabels[20]; /* 0x004693f0 */
extern const char szSaveCampaignMenuLabel[24]; /* 0x00469468 */
extern const char szSaveCampaignMenuLabelAlt[24]; /* 0x00469480 */
extern const char szMissionHangarMenuLabel[16]; /* 0x00469498 */
extern const char szReturnToBarMenuLabel[20]; /* 0x004694a8 */
extern const char szQuitGameMenuLabel[20]; /* 0x004694bc */
extern const char szViewMedalsMenuLabel[20]; /* 0x004694d0 */
extern short nPaletteTransitionInitialise; /* 0x00469640 */
extern int nDebugOverlayConsoleCount; /* 0x00469644 */
extern DWORD dwDebugOverlayKey; /* 0x00469648 */
extern DWORD dwDebugOverlayKeyLatch; /* 0x0046964c */
extern HHOOK hDebugKeyboardHook; /* 0x00469650 */
extern const char szDebugOverlayFontName[8]; /* 0x00469654 */
extern const char szDebugOverlaySpinner[5]; /* 0x0046965c */
extern const char szDebugOverlayNewline[4]; /* 0x00469664 */
extern char szDiskMarkerFile[9]; /* 0x00469688 */
extern short nDiskPromptBorderColour; /* 0x00469694 */
extern short nKeyboardPointerStep; /* 0x004696a4 */
extern const short asPlayerDamageSystemTable[50]; /* 0x00469878 */
extern const enum ObjectType aeShipHitDebrisTypes[3]; /* 0x00469950 */
extern const signed char acGunRefireDelay[4]; /* 0x0046995c */
extern const char szWeaponDestroyed[20]; /* 0x00469960 */
extern const char szFuelTanksHit[16]; /* 0x00469974 */
extern const char szComponentFixedFormat[8]; /* 0x00469984 */
extern const char szNeedLock[12]; /* 0x0046998c */
extern short bSlowSceneAnimation; /* 0x00469998 */
extern unsigned char cBlackColour; /* 0x0046999c */
extern unsigned char cViewportClearColour; /* 0x004699a0 */
extern unsigned char cBlueColour; /* 0x004699a4 */
extern unsigned char cYellowColour; /* 0x004699a8 */
extern unsigned char cRedColour; /* 0x004699ac */
extern unsigned char cDarkGreenColour; /* 0x004699b0 */
extern unsigned char cPrimaryTextColour; /* 0x004699b4 */
extern unsigned char cOrangeColour; /* 0x004699b8 */
extern unsigned char cDarkBlueColour; /* 0x004699bc */
extern unsigned char cDarkGreyColour; /* 0x004699c0 */
extern unsigned char cLightGreyColour; /* 0x004699c4 */
extern unsigned char cMagentaColour; /* 0x004699c8 */
extern unsigned char cDefaultTextColour; /* 0x004699cc */
extern unsigned char cBrownColour; /* 0x004699d0 */
extern unsigned char cAsteroidColour; /* 0x004699d4 */
extern unsigned char cPrimaryViewBufferColour; /* 0x004699d8 */
extern const unsigned char abLegacyVideoModeColours[14]; /* 0x004699e0 */
extern short asConversationTextColours[24]; /* 0x004699f0 */
extern int bGraphicsActive; /* 0x00469a20 */
extern const int anBaseMemoryReservationByVideoMode[4]; /* 0x00469a90 */
extern const int anFullMusicMemoryReservationByVideoMode[4]; /* 0x00469aa0 */
extern const int anExpandedMemoryReservationByVideoMode[3]; /* 0x00469ab0 */
extern PacketResourceDescriptor aCommon3SpaceResources[12]; /* 0x00469bc0 */
extern int nCapitalShipViewDistance; /* 0x00468ff4 */
extern unsigned char bLandingAuthorized; /* 0x00468ff8 */
extern int bAlternateChaseView; /* 0x00468ffc */
extern unsigned char *pCockpitPilotShape; /* 0x0046905c */
extern unsigned int bIntroSceneResourcesActive; /* 0x00469d48 */
extern short asConstellationObjectIndices[4]; /* 0x00469d50 */
extern int b3SpaceObjectsActive; /* 0x00469d5c */
extern int bCockpitResourcesActive; /* 0x00469d58 */
extern const short asSceneAnimationLogicalFiles[8]; /* 0x00469d60 */
extern short bSceneAnimationWaitCommand; /* 0x00469d70 */
extern const char szSceneAnimationTextFormat[12]; /* 0x00469d74 */
extern PacketResourceDescriptor aMissionResourceDescriptors[5]; /* 0x00469c20 */
extern PacketResourceDescriptor aCockpitResourceDescriptors[19]; /* 0x00469c48 */
extern PacketResourceDescriptor aCockpitSecondaryResources[5]; /* 0x00469ce0 */
extern PacketResourceDescriptor aCockpitPrimaryResources[8]; /* 0x00469d08 */
extern const char szMeanwhile[16]; /* 0x00469d80 */
extern const char szEmptyAnswerInput[4]; /* 0x00469d90 */
extern const char szAnswerLabel[9]; /* 0x00469d94 */
extern Viewport stTrainSimPanelViewport; /* 0x00469da8 */
extern const ShortRect stTrainSimPanelBounds; /* 0x00469dc0 */
extern const char szTrainSimTitle[24]; /* 0x00469dc8 */
extern unsigned char *apszBuiltInHighScoreNames[6]; /* 0x00469de0 */
extern TitleMenuRegion aTrainSimMissionRegions[5]; /* 0x00469df8 */
extern unsigned char aszBuiltInHighScores[48]; /* 0x00469e38 */
extern unsigned int dwTrainSimStringPadding; /* 0x00469e68 */
extern const char szTextFlushToken[4]; /* 0x00469e6c */
extern const char szNewPilotPrompt[80]; /* 0x00469e70 */
extern const char szDefaultPilotName[8]; /* 0x00469ec0 */
extern const char szPilotNameLabel[12]; /* 0x00469ec8 */
extern const char szDefaultCallsign[12]; /* 0x00469ed4 */
extern const char szCallsignLabel[12]; /* 0x00469ee0 */
extern const char szCheaterCallsign[8]; /* 0x00469eec */
extern const char szHighScoreCongratulations[68]; /* 0x00469ef4 */
extern const char szLowScoreMessage[56]; /* 0x00469f38 */
extern const char szHighScoresHeading[16]; /* 0x00469f70 */
extern const char szHighScoreNumberFormat[8]; /* 0x00469f80 */
extern const char szHighScoreRowFormat[16]; /* 0x00469f88 */
extern const char szSelectEnemy[16]; /* 0x00469f98 */
extern int nTrainSimActive; /* 0x00469e2c */
extern short nTrainSimMission; /* 0x00469e30 */
extern short nArcadeWave; /* 0x00469e34 */
extern int nCannedSceneMode; /* 0x00469fac */
extern int nArcadeState; /* 0x00469fb0 */
extern short nFrameSkipCounter; /* 0x00469fb4 */
extern short nFrameSkip; /* 0x00469fb8 */
extern int nOriginDevUnlock; /* 0x00469ff4 */
extern int bPlayerCollisionsEnabled; /* 0x00469ff8 */
extern int bPlayerVulnerable; /* 0x00469ffc */
extern int bPlayerCollisionResponse; /* 0x0046a000 */
extern int bViewBufferEnabled; /* 0x0046a004 */
extern int bCockpitlessView; /* 0x0046a008 */
extern int nShowMemoryStatus; /* 0x0046a00c */
extern short nStartNavPointOverride; /* 0x0046a010 */
extern short nArcadeBonusCountdown; /* 0x0046a014 */
extern int bMouseCursorVisible; /* 0x0046a018 */
extern unsigned char bInputPollingGuard; /* 0x0046a01c */
extern InputDeviceSample stLastPolledFlightInput; /* 0x0046a020 */
extern int bMouseAfterburnerControl; /* 0x0046a02c */
extern const short asMouseYawThresholds[6]; /* 0x0046a030 */
extern const short asMousePitchThresholds[6]; /* 0x0046a040 */
extern unsigned int dwLastSecondaryButtonPress; /* 0x0046a04c */
extern unsigned char bFlightRollLatch; /* 0x0046a050 */
extern unsigned char bAfterburnerButtonLatched; /* 0x0046a054 */
extern short nMouseYawInput; /* 0x0046a058 */
extern short nMousePitchInput; /* 0x0046a05c */
extern const char szSpaceBuffer[16]; /* 0x0046a1d0 */
extern const char szAllocateBufferTag[4]; /* 0x0046a1e0 */
extern unsigned char *pDamageDisplayBackground; /* 0x0046a748 */
extern const ShortPoint aDamageDisplayPositions[9]; /* 0x0046a750 */
extern const char *apszComponentNames[9]; /* 0x0046a778 */
extern const unsigned char abDamageDisplayFrames[9]; /* 0x0046a7a0 */
extern const char *apszDamageSeverityNames[5]; /* 0x0046a7b0 */
extern const char szIonDrive[12]; /* 0x0046a7c4 */
extern const char szPowerPlant[12]; /* 0x0046a7d0 */
extern const char szShieldGenerator[16]; /* 0x0046a7dc */
extern const char szComputerSystem[16]; /* 0x0046a7ec */
extern const char szIntercomUnit[16]; /* 0x0046a7fc */
extern const char szTargetTracking[16]; /* 0x0046a80c */
extern const char szAccelerationAbsorbers[16]; /* 0x0046a81c */
extern const char szEjectorSystem[16]; /* 0x0046a82c */
extern const char szRepairSystems[16]; /* 0x0046a83c */
extern const char szDamageOk[4]; /* 0x0046a84c */
extern const char szDamageLight[8]; /* 0x0046a850 */
extern const char szDamageModerate[12]; /* 0x0046a858 */
extern const char szDamageHeavy[8]; /* 0x0046a864 */
extern const char szDamageDestroyed[12]; /* 0x0046a86c */
extern const char szDamageReport[16]; /* 0x0046a878 */
extern const char szNoInternalDamage[20]; /* 0x0046a888 */
extern const char szDamagedUnitCountFormat[20]; /* 0x0046a89c */
extern const char szDamageStatusFormat[16]; /* 0x0046a8b0 */
extern const char szDamageStatusFormatHighRes[16]; /* 0x0046a8c0 */
extern ActiveSoundEntry *pActiveSoundHead; /* 0x0046a438 */
extern ActiveSoundEntry *pActiveSoundTail; /* 0x0046a43c */
extern int bAudioSystemInitialized; /* 0x0046a440 */
extern WaveTableEntry *pWaveTableHead; /* 0x0046a444 */
extern WaveTableEntry *pWaveTableTail; /* 0x0046a448 */
extern const char szPlayWaveOpenError[36]; /* 0x0046a46c */
extern const short asCarrierLaunchApproachDeltaX[24]; /* 0x0046a550 */
extern const signed char acCarrierLaunchApproachFrames[24]; /* 0x0046a580 */
extern const ShortPoint aCarrierLaunchFighterPath[9]; /* 0x0046a598 */
extern const short asCarrierLaunchFighterDeltaY[16]; /* 0x0046a5bc */
extern const short asCarrierLaunchViewData[16]; /* 0x0046a5dc */
extern const char szPressAnyKeyWhenReady[28]; /* 0x0046a5fc */
extern short nMusicPlaybackMode; /* 0x0046a9f8 */
extern int bMusicCommandSuppressed; /* 0x0046a9fc */
extern short nMusicTrackComplete; /* 0x0046aa04 */
extern short nFuneralSequenceActive; /* 0x0046aa10 */
extern int nCurrentMusicTrack; /* 0x0046aa14 */
extern int nMusicStreamSet; /* 0x0046aa18 */
extern volatile int nInFlightMusicSlotA; /* 0x0046aa24 */
extern volatile int nInFlightMusicSlotB; /* 0x0046aa2c */
extern int nWaitForMusicEnabled; /* 0x0046aa30 */
extern int nFlightSoundEffectsEnabled; /* 0x0046aa34 */
extern int nInitialFlightMusicPending; /* 0x0046aa38 */
extern int nCombatMusicActive; /* 0x0046aa3c */
extern int nInFlightMusicActive; /* 0x0046aa40 */
extern short nPassingShipSoundObject; /* 0x0046aa48 */
extern short nPassingShipSoundCountdown; /* 0x0046aa4c */
extern const unsigned char abFireworkSoundDescriptor[7]; /* 0x0046ab70 */
extern const char szSfxWaveFormat[12]; /* 0x0046ad2c */
extern unsigned char abTaggedAllocationPrefix[8]; /* 0x0046ad88 */
extern const char *apszCampaignVictoryText[4]; /* 0x0046ad90 */
extern const char *pszTigerClawEscapeOpening; /* 0x0046ada0 */
extern const char *pszTigerClawEscapeJump; /* 0x0046ada4 */
extern const char *pszTigerClawEscapeClosing; /* 0x0046ada8 */
extern const ShortVector aCampaignVictoryProjectileOrigins[4]; /* 0x0046adb0 */
extern const char *pszTheEnd; /* 0x0046adc8 */
extern const char *pszForNow; /* 0x0046adcc */
extern const char szCampaignVictoryOpening[68]; /* 0x0046add0 */
extern const char szCampaignVictoryAttack[44]; /* 0x0046ae14 */
extern const char szCampaignVictoryConclusion[52]; /* 0x0046ae40 */
extern const char szTigerClawEscapeOpening[64]; /* 0x0046ae74 */
extern const char szTigerClawEscapeJump[48]; /* 0x0046aeb4 */
extern const char szTigerClawEscapeClosing[44]; /* 0x0046aee4 */
extern const char szTheEnd[8]; /* 0x0046af10 */
extern const char szForNow[12]; /* 0x0046af18 */
extern const char szCampaignVictoryTextFormat[12]; /* 0x0046af24 */
extern const char szTigerClawEscapeOpeningFormat[12]; /* 0x0046af30 */
extern const char szTigerClawEscapeJumpFormat[12]; /* 0x0046af3c */
extern const char szTigerClawEscapeClosingFormat[12]; /* 0x0046af48 */
extern short nCommMenuChoiceCount; /* 0x0046af60 */
extern short nCommMenuReuseMode; /* 0x0046af64 */
extern signed char cPendingCommMenuAction; /* 0x0046af6c */
extern unsigned char bRadioSilence; /* 0x0046af70 */
extern unsigned char bVideoImagesSuppressed; /* 0x0046af74 */
extern unsigned char bCommVideoEnabled; /* 0x0046af78 */
extern signed char cCommMenuRecipient; /* 0x0046afc4 */
extern short nCommSpeakerObject; /* 0x0046afc8 */
extern short nCommSpeakerRating; /* 0x0046afcc */
extern short nCommPortraitIndex; /* 0x0046afd0 */
extern const char szConfedCommFormat[8]; /* 0x0046b150 */
extern const char szKilrathiAceCommFormat[8]; /* 0x0046b158 */
extern const char szShipCommFormat[8]; /* 0x0046b160 */
extern short nInputDoubleClickInterval; /* 0x0046af54 */
extern short nMenuPointerSpeed; /* 0x0046af58 */
extern const short asConstellationVelocity[16]; /* 0x0046a8d8 */
extern const short asConstellationFrame[16]; /* 0x0046a8f8 */
extern short nConstellationDirection; /* 0x0046a918 */
extern unsigned char *pPacketDecompressionWorkspace; /* 0x0046a91c */
extern unsigned short wPacketDecompressionInputSizeOverride; /* 0x0046a920 */
extern unsigned short wPacketCompressionFormatFlags; /* 0x0046a924 */
extern const ShortRect aTargetArmorClipRects[4]; /* 0x0046a928 */
extern char szEmptyTargetDisplayText[4]; /* 0x0046a948 */
extern const char szLockedTarget[20]; /* 0x0046a94c */
extern const char szTextColourStringColourFormat[8]; /* 0x0046a960 */
extern const char szAutoTargetting[20]; /* 0x0046a968 */
extern const char szTextColourStringFormat[8]; /* 0x0046a97c */
extern const char szTargetLabel[12]; /* 0x0046a984 */
extern const char szNoTarget[8]; /* 0x0046a990 */
extern const char szWingmanTargetNameFormat[4]; /* 0x0046a998 */
extern const char szAceTargetNameFormat[4]; /* 0x0046a99c */
extern const char szShipTargetNameFormat[4]; /* 0x0046a9a0 */
extern const char szRangeLabel[12]; /* 0x0046a9a4 */
extern const char szTargetTooFar[8]; /* 0x0046a9b0 */
extern const char szRangeMetresSuffix[4]; /* 0x0046a9b8 */
extern const char szTargetOffscreenRange[8]; /* 0x0046a9bc */
extern unsigned short nVideoMode; /* 0x0046b168 */
extern const signed char acGraphicsModeByAnimationSpeed[8]; /* 0x0046b178 */
extern const CockpitLayout stCockpitLayout; /* 0x0046e008 */
extern const short aasCockpitLightX[5][7]; /* 0x0046dca8 */
extern const short aasCockpitLightY[5][7]; /* 0x0046dcf0 */
extern const signed char aacCockpitLightOffFrame[5][7]; /* 0x0046dd38 */
extern const signed char aacCockpitLightOnFrame[5][7]; /* 0x0046dd60 */
extern const CockpitBarDefinition aaCockpitBars[5][8]; /* 0x0046dd88 */
extern const GUID guidDirectDraw2; /* 0x00463118 */
extern LPDIRECTDRAW2 pDirectDraw2; /* 0x0046b1a4 */
extern unsigned char bCurrentKey; /* 0x0046c014 */
extern signed char cPreviousKey; /* 0x0046c018 */
extern short nCurrentWave; /* 0x0046c01c */
extern int bIntroSecondaryScene; /* 0x0046c024 */
extern signed char abHazardObjects[0x14]; /* 0x0046c028 */
extern short DAT_0046c010;
extern short nEyePitchRate; /* 0x0046c004 */
extern short nEyeYawRate; /* 0x0046c008 */
extern short nEyeRollRate; /* 0x0046c00c */
extern signed char cCurrentObjective; /* 0x0046c020 */
extern int nCameraViewMode; /* 0x0046c03c */
extern signed char cViewObject; /* 0x0046c000 */
extern short nExternalViewShip; /* 0x0046c040 */
extern short nEjectedPilotObject; /* 0x0046c044 */
extern short nClosestVisibleObject; /* 0x0046c048 */
extern short nYourWingman; /* 0x0046c04c */
extern short nPlayerCollisionObject; /* 0x0046c050 */
extern enum ObjectType eSelectedGunType; /* 0x0046c054 */
extern int nSelectedReleaseWeaponIndex; /* 0x0046c058 */
extern short DAT_0046c05c;
extern short bTargetLockReadoutDirty; /* 0x0046c060 */
extern short nTargetLockCountdown; /* 0x0046c064 */
extern short nTargetLockMarkerAngle; /* 0x0046c068 */
extern signed char cTargetDisplayObject; /* 0x0046c06c */
extern int bTargetLockAcquired; /* 0x0046c074 */
extern short nTargetLockMode; /* 0x0046c078 */
extern int bMissileCameraEnabled; /* 0x0046c07c */
extern int bEngageAllowed; /* 0x0046c080 */
extern short nAutoEngageTimer; /* 0x0046c084 */
extern signed char cViableTargetCount; /* 0x0046c088 */
extern MissionNavPoint aMissionNavPoints[WC1_MISSION_NAV_POINT_COUNT]; /* 0x0046c2f0 */
extern MissionShipRecord
    aMissionShips[WC1_MISSION_SHIP_STORAGE_COUNT]; /* 0x0046c948 */
extern const short asIntroCameraSequence[]; /* 0x0046c090 */
extern const short asCannedSequence32[]; /* 0x0046c0b8 */
extern const short asCannedSequence33[]; /* 0x0046c0d8 */
extern const short asCannedSequence34[]; /* 0x0046c0f0 */
extern const short asCannedSequence35[]; /* 0x0046c108 */
extern const short asCampaignVictoryViewScript[24]; /* 0x0046c160 */
extern const short asCannedSequence37[]; /* 0x0046c190 */
extern const short asCannedSequence38[]; /* 0x0046c1b0 */
extern const short asCannedSequence39[]; /* 0x0046c1d0 */
extern const short asCannedSequence40[]; /* 0x0046c1e0 */
extern const short asCannedSequence41[]; /* 0x0046c230 */
extern const short asTigerClawEscapeViewScript[12]; /* 0x0046c238 */
extern const short asCannedSequence42[]; /* 0x0046c250 */
extern const short asCannedSequence43[]; /* 0x0046c270 */
extern const short asCannedSequence44[]; /* 0x0046c2a0 */
extern const short asCannedSequence45[]; /* 0x0046c2e8 */
extern const signed char acDirectionShapeFrame[ /* 0x0046db28 */
    WC1_DIRECTION_VIEW_COUNT * WC1_DIRECTION_SHAPE_TABLE_COUNT];
extern const signed char acDirectionShapeFlip[ /* 0x0046dbe8 */
    WC1_DIRECTION_VIEW_COUNT * WC1_DIRECTION_SHAPE_TABLE_COUNT];
extern InputEvent *pInputEventHead; /* 0x0046da90 */
extern InputEvent *pInputEventTail; /* 0x0046da94 */
extern int bInputEventPoolInitialized; /* 0x0046da98 */
extern short nScreenWidth; /* 0x0046daa4 */
extern short nScreenHeight; /* 0x0046daa8 */
extern const ScreenViewportGeometry aScreenViewportGeometry[6]; /* 0x0046dab8 */
extern int aiIntelligenceEvent[10]; /* 0x0046d368 */
extern signed char aDefenseManeuversNovice[8]; /* 0x0046d390 */
extern signed char aDefenseManeuversVeteran[8]; /* 0x0046d398 */
extern signed char aDefenseManeuversElite[16]; /* 0x0046d3a0 */
extern signed char aDefenseManeuversAce[16]; /* 0x0046d3b0 */
extern signed char aDefenseManeuversBoss[16]; /* 0x0046d3c0 */
extern signed char *apDefenseManeuvers[5]; /* 0x0046d3d0 */
extern ManeuverChoice aRatedManeuverChoices[13][9][3]; /* 0x0046d3e8 */
extern ManeuverChoice aKilrathiManeuverChoices[5][9][3]; /* 0x0046d808 */
extern signed char acPilotAggression[24]; /* 0x0046d9a0 */
extern signed char acPilotRecovery[20]; /* 0x0046d9b8 */
extern unsigned char *pDrawnMouseCursorShape; /* 0x0046da9c */
extern int nMouseCursorShowCount; /* 0x0046daa0 */
extern const short asPilotHandOrigins[10]; /* 0x0046e120 */
extern CampaignDate stSavedCampaignDate; /* 0x0046e188 */
extern const TalkingHeadOrigin aTalkingHeadOrigins[11]; /* 0x0046e190 */
extern const signed char abBriefingSmallCharacterAnimation[24]; /* 0x0046e1e8 */
extern const signed char abBriefingLargeCharacterAnimation[24]; /* 0x0046e200 */
extern BriefingCharacterLayout aBriefingCharacters[8]; /* 0x0046e218 */
extern const short asMedalDisplayX[5]; /* 0x0046e2d0 */
extern const char *apszMedalNames[5]; /* 0x0046e2e0 */
extern unsigned char *pMedalSceneShape; /* 0x0046e2f4 */
extern const char *pszMedalsPilotSummary; /* 0x0046e2f8 */
extern const signed char aBriefingPortraitOffsetX[8][12]; /* 0x0046e300 */
extern const signed char aBriefingPortraitOffsetY[8][12]; /* 0x0046e360 */
extern const short aBriefingPortraitScale[8][12]; /* 0x0046e3c0 */
extern const int aiBriefingLeftPanelVelocity[12]; /* 0x0046e480 */
extern const int aiBriefingPodiumVelocity[12]; /* 0x0046e4b0 */
extern const int aiBriefingRightPanelVelocity[12]; /* 0x0046e4e0 */
extern const signed char abBriefingPodiumFrames[40]; /* 0x0046e510 */
extern const signed char abDebriefingEstablishDeltas[48]; /* 0x0046e538 */
extern short nDebriefingLeftX; /* 0x0046e56c */
extern short nDebriefingPilotX; /* 0x0046e570 */
extern short nDebriefingRightX; /* 0x0046e574 */
extern short nDebriefingOfficerX; /* 0x0046e578 */
extern short nDebriefingPodiumX; /* 0x0046e57c */
extern short nConversationCharacter; /* 0x0046e580 */
extern short nTalkingHeadFace; /* 0x0046e584 */
extern short nConversationBackdropFrame; /* 0x0046e588 */
extern int bConversationConstellation; /* 0x0046e58c */
extern int bConversationOverlay; /* 0x0046e590 */
extern const char szBronzeStar[12]; /* 0x0046e594 */
extern const char szSilverStar[12]; /* 0x0046e5a0 */
extern const char szGoldStar[12]; /* 0x0046e5ac */
extern const char szGoldenSun[12]; /* 0x0046e5b8 */
extern const char szTerranMedalOfValor[24]; /* 0x0046e5c4 */
extern const char szMedalsPilotSummary[40]; /* 0x0046e5dc */
extern const char szViewMedalsTextFormat[12]; /* 0x0046e604 */
extern const char szMedalChestTextFormat[12]; /* 0x0046e610 */
extern const char szMedalLongShotTextFormat[12]; /* 0x0046e61c */
extern const char szMedalEstablishTextFormat[12]; /* 0x0046e628 */
extern const char szPinMedalTextFormat[12]; /* 0x0046e634 */
extern const char szEstablishingShotTextFormat[12]; /* 0x0046e640 */
extern const char szBriefingReturnTextFormat[12]; /* 0x0046e64c */
extern const char szDismissedTextFormat[12]; /* 0x0046e658 */
extern const char szDebriefEstablishTextFormat[12]; /* 0x0046e664 */
extern const char szFuneralLongShotTextFormat[12]; /* 0x0046e670 */
extern void *apRasterDriverCallbacks[13]; /* 0x0046e69c */
extern char szRasterDriverName[13]; /* 0x0046e6d0 */
extern unsigned char *pRLEEncodeBuffer; /* 0x0046e6dd */
extern int nRLEPendingSkip; /* 0x0046e6e1 */
extern unsigned char *pRLEScanlineStart; /* 0x0046e6e5 */
extern unsigned char *pRLEScanCursor; /* 0x0046e6e9 */
extern unsigned char *pRLEOutputCursor; /* 0x0046e6ed */
extern unsigned char *pRLELiteralStart; /* 0x0046e6f1 */
extern int nRLEEncodedMinimumX; /* 0x0046e705 */
extern int nRLEEncodedMinimumY; /* 0x0046e709 */
extern int nRLEEncodedMaximumX; /* 0x0046e70d */
extern int nRLEEncodedMaximumY; /* 0x0046e711 */
extern unsigned char abRasterDecodeBuffer[0xd00]; /* 0x0046ec15 */
extern unsigned char abRasterPaletteUsageScratch[0x300]; /* 0x0046f915 */
extern unsigned char abRasterFadeAccumulator[0x300]; /* 0x0046fc15 */
extern const unsigned char abGifLZWCodeMasks[9]; /* 0x0046ff15 */
extern const unsigned char abGifInterlaceRowSteps[5]; /* 0x0046ff1e */
extern const unsigned char abGifInterlaceRowStarts[5]; /* 0x0046ff23 */
extern RasterClip *pGifRasterTarget; /* 0x0046ff28 */
extern unsigned char abRasterPaletteTranslation[256]; /* 0x0046ff2c */
extern RLETransformVertex aRLETransformVertices[4]; /* 0x0047002c */
extern int anRLESourceSteps[4]; /* 0x0047007c */
extern const char *pszBlankRoomMenuLabel; /* 0x00470090 */
extern const char *apszPilotRankNames[5]; /* 0x00470098 */
extern CampaignState stInitialCampaignState; /* 0x004700b0 */
extern PilotRecord aInitialPilotRecords[9]; /* 0x00470108 */
extern signed char abSpiritRecRoomAnimation[48]; /* 0x00470260 */
extern signed char abHunterRecRoomAnimation[40]; /* 0x00470290 */
extern signed char abAngelRecRoomAnimation[48]; /* 0x004702b8 */
extern signed char abKnightRecRoomAnimation[40]; /* 0x004702e8 */
extern signed char abIcemanRecRoomAnimation[40]; /* 0x00470310 */
extern signed char abManiacRecRoomAnimation[24]; /* 0x00470338 */
extern signed char abPaladinRecRoomAnimation[56]; /* 0x00470350 */
extern signed char abBossmanRecRoomAnimation[48]; /* 0x00470388 */
extern signed char abShotglassIdleAnimation[24]; /* 0x004703b8 */
extern signed char abShotglassGlassAnimation[32]; /* 0x004703d0 */
extern signed char abShotglassPourAnimation[80]; /* 0x004703f0 */
extern signed char abShotglassWipeAnimation[24]; /* 0x00470440 */
extern signed char *apRecRoomAnimations[14]; /* 0x00470458 */
extern const ShortPoint aRecRoomCharacterOrigins[3]; /* 0x00470490 */
extern short nRecRoomPanelOriginX; /* 0x0047049c */
extern short nRecRoomPanelOriginY; /* 0x0047049e */
extern TitleMenuRegion aRecRoomMenuRegions[7]; /* 0x004704a0 */
extern char *apszRecRoomBaseLabels[3]; /* 0x004704e8 */
extern char *apszRecRoomMenuLabels[6]; /* 0x004704f8 */
extern CampaignDate stChalkBoardDate; /* 0x00470514 */
extern short asChalkBoardPilotOrder[9]; /* 0x00470518 */
extern const char szBlankRoomMenuLabel[4]; /* 0x0047052c */
extern const char szSecondLieutenantRank[8]; /* 0x00470530 */
extern const char szFirstLieutenantRank[8]; /* 0x00470538 */
extern const char szCaptainRank[8]; /* 0x00470540 */
extern const char szMajorRank[8]; /* 0x00470548 */
extern const char szLieutenantColonelRank[12]; /* 0x00470550 */
extern char szTalkToShotglass[20]; /* 0x0047055c */
extern char szTalkToFirstPilot[24]; /* 0x00470570 */
extern char szTalkToSecondPilot[24]; /* 0x00470588 */
extern char szCheckPilotScores[20]; /* 0x004705a0 */
extern char szEnterBarracks[16]; /* 0x004705b4 */
extern char szFlyTrainingMission[24]; /* 0x004705c4 */
extern const char szRoomMenuLabelFormat[12]; /* 0x004705dc */
extern const char szEmptyRoomMenuLabel[4]; /* 0x004705e8 */
extern const char szTalkToPilotFormat1[16]; /* 0x004705ec */
extern const char szTalkToPilotFormat2[16]; /* 0x004705fc */
extern const char szChalkBoardHeading[32]; /* 0x0047060c */
extern const char szChalkBoardPilotHeading[12]; /* 0x0047062c */
extern const char szChalkBoardScoreHeading[20]; /* 0x00470638 */
extern const char szChalkBoardPilotFormat[12]; /* 0x0047064c */
extern const char szChalkBoardScoreFormat[12]; /* 0x00470658 */
extern const char szKilledInAction[4]; /* 0x00470664 */
extern const char szChalkBoardKiaFormat[16]; /* 0x00470668 */
extern unsigned char abPaletteTranslation[256]; /* 0x00470678 */
extern unsigned short awAbsoluteCosine[360]; /* 0x00470778 */
extern unsigned short awAbsoluteSine[360]; /* 0x00470a48 */
extern int bPanRoomTransition; /* 0x00470510 */
extern const char szInvalidShapeAllocation[12]; /* 0x00470d18 */
extern const char szBadViewport[12]; /* 0x00470d24 */
extern const char szShapeRLEVersion[8]; /* 0x00470d30 */
extern const char szShapeRLEOverflow[20]; /* 0x00470d38 */
extern const char szBadShapeFlip[12]; /* 0x00470d4c */
extern const char szCopyLine[12]; /* 0x00470d58 */
extern const char szWipeLine[12]; /* 0x00470d64 */
extern const char szTriangle[12]; /* 0x00470d70 */
extern const char szShadedTriangle[16]; /* 0x00470d7c */
extern const char szShadowDraw[12]; /* 0x00470d8c */
extern const char szFizzleFade[12]; /* 0x00470d98 */
extern const char szSnowViewport[16]; /* 0x00470da4 */
extern int nInsertKeyState; /* 0x00475b68 */
extern int nClearedKeyboardState; /* 0x00475b6c */
extern int nOemPeriodKeyState; /* 0x00475bf0 */
extern int nHomeKeyState; /* 0x00475bf4 */
extern int nPageDownKeyState; /* 0x00475bf8 */
extern int nOemCommaKeyState; /* 0x00475bfc */
extern int nRightArrowKeyState; /* 0x00475c00 */
extern int nEndKeyState; /* 0x00475c04 */
extern int nClearKeyState; /* 0x00475c08 */
extern int nDeleteKeyState; /* 0x00475c0c */
extern int nUpArrowKeyState; /* 0x00475c10 */
extern int nDownArrowKeyState; /* 0x00475c14 */
extern char szStreamsPath[0x100]; /* 0x00475c18 */
extern int nPageUpKeyState; /* 0x00475d18 */
extern int nLeftArrowKeyState; /* 0x00475d1c */
extern char szReadDataFileError[0x40]; /* 0x00475d20 */
extern char szCreateDataFileError[0x40]; /* 0x00475d60 */
extern char szWriteDataFileError[0x40]; /* 0x00475da0 */
extern char szSeekDataFileError[0x80]; /* 0x00475de0 */
extern int bMonoDebugInstalled; /* 0x00475e70 */
extern HANDLE hMonoDebugDevice; /* 0x00475e74 */
extern WaveTableEntry *pSnowStaticWave; /* 0x0047654c */
extern IxSound *pSnowStaticSound; /* 0x00476550 */
extern int nDisplayedComponentDamage; /* 0x00476554 */
extern char szSfxWavePath[0xc8]; /* 0x00476558 */
extern short DAT_00475e78;
extern unsigned char bCurrentManeuverReroll; /* 0x00475e7c */
extern unsigned char abMouseCursorBackground[0x400]; /* 0x00475ff0 */
extern DebugOverlayConsole *pDebugOverlay; /* 0x004763f0 */
extern int nFlightFrameTotalTicks; /* 0x004764f8 */
extern int nFlightCockpitTicks; /* 0x004764fc */
extern LARGE_INTEGER liFlightAfterInput; /* 0x00476500 */
extern LARGE_INTEGER liFlightFrameEnd; /* 0x00476508 */
extern int nFlightPresentTicks; /* 0x00476510 */
extern LARGE_INTEGER liFlightFrameStart; /* 0x00476518 */
extern LARGE_INTEGER liFlightAfterSimulation; /* 0x00476520 */
extern int nFlightSimulationTicks; /* 0x00476528 */
extern LARGE_INTEGER liFlightAfterCockpit; /* 0x00476530 */
extern int nFlightInputTicks; /* 0x00476538 */
extern LARGE_INTEGER liFlightAfterRender; /* 0x00476540 */
extern int nFlightRenderTicks; /* 0x00476548 */
extern unsigned int DAT_00476640;
extern unsigned int DAT_00476644;
extern unsigned char *pDIBPixelBuffer; /* 0x00476648 */
extern unsigned int nDIBWidth; /* 0x0047664c */
extern unsigned int nDIBHeight; /* 0x00476650 */
extern unsigned char abDIBPixelBackup[0xfa00]; /* 0x00476658 */
extern unsigned char szFrameRateText[32]; /* 0x00476620 */
extern int DAT_00486058;
extern int nDIBSlamCount; /* 0x00486060 */
extern HWND hDIBWindow; /* 0x00486074 */
extern unsigned char abDIBPaletteCache[1024]; /* 0x00486110 */
extern float fMeasuredFrameRate; /* 0x00486510 */
extern int bDIBSlamPending; /* 0x00486518 */
extern char szSpaceFlightMaxFpsMessage[0x80]; /* 0x00486528 */
extern unsigned char abCursorSaveArea[0x1000]; /* 0x004865a8 */
extern RasterSurface stRasterSurface; /* 0x004875a8 */
extern unsigned char abShapeTransformScratch[0xfa00]; /* 0x004875c0 */
extern RasterClip stRasterClip; /* 0x00496fc0 */
extern unsigned char abSolidColourTranslation[256]; /* 0x00497648 */
extern unsigned char abShapeRLEScratch[0x100000]; /* 0x00497748 */
extern int DAT_00598888;
extern int DAT_0059888c;
extern int DAT_00598890;
extern Viewport stRoomScreenViewport; /* 0x005988a0 */
extern short nSavedRoomControllerX; /* 0x005988b4 */
extern unsigned char *pRecRoomRoster; /* 0x005988b8 */
extern unsigned char *pMissionCampaignData; /* 0x005988bc */
extern unsigned char *apRecRoomCharacterShapes[3]; /* 0x005988c0 */
extern PilotRecord aPilotRecords[9]; /* 0x005988d0 */
extern ConstellationObjectDefinition
    *pConstellationDefinitions; /* 0x00598a28 */
extern PilotRecord *apWingmanPilots[8]; /* 0x00598a30 */
extern unsigned char *pRecRoomBackgroundShape; /* 0x00598a50 */
extern Viewport stRoomDisplayViewport; /* 0x00598a60 */
extern Viewport stRoomMouseViewport; /* 0x00598a80 */
extern unsigned char *apRecRoomTextData[3]; /* 0x00598aa0 */
extern char szTextScratchBuffer[256]; /* 0x00598b00 */
extern short nRoomMenuCursorFrame; /* 0x00598ab0 */
extern TitleMenuRegion *pRoomMenuRegions; /* 0x00598ab2 */
extern char **ppszRoomMenuLabels; /* 0x00598ab6 */
extern const char *pszCurrentRoomMenuLabel; /* 0x00598aba */
extern TextContext stRoomMenuTextContext; /* 0x00598abe */
extern unsigned char *apRecRoomSceneData[3]; /* 0x00598ae0 */
extern unsigned char *pBriefingPacket; /* 0x00598aec */
extern unsigned char *pBriefingTextData; /* 0x00598af0 */
extern short *pMouthAnimationCommands; /* 0x00598af4 */
extern unsigned char *pMedalTextData; /* 0x00598af8 */
extern unsigned char *pDebriefingSceneData; /* 0x00598afc */
extern unsigned char *pBriefingSceneData; /* 0x00598c00 */
extern unsigned char *pConversationBackdropShape; /* 0x00598c04 */
extern short nConversationMedalIndex; /* 0x00598c08 */
extern short nConversationSceneType; /* 0x00598c0a */
extern unsigned char *pTalkingHeadShape; /* 0x00598c0c */
extern short nConversationTextColour; /* 0x00598c10 */
extern unsigned char *pBriefingAnimationShape; /* 0x00598c14 */
extern short *pFaceAnimationCommands; /* 0x00598c18 */
extern unsigned char *pBriefingBodyShape; /* 0x00598c1c */
extern unsigned char *pMedalSceneData; /* 0x00598c20 */
extern unsigned char *pBriefingPortraitShape; /* 0x00598c24 */
extern unsigned char *pDebriefingTextData; /* 0x00598c28 */
extern unsigned char *pBriefingCloseupShape; /* 0x00598c2c */
extern unsigned char *pConversationOverlayShape; /* 0x00598c30 */
extern unsigned char bFilteredKeyWaitStarted; /* 0x00598c34 */
extern InputEvent aInputEventPool[0x100]; /* 0x00598c40 */
extern int bStreamerAudioPlaying; /* 0x00597748 */
extern char szStreamerPath[128]; /* 0x00597750 */
extern int nMouseCursorSavedY; /* 0x0059a840 */
extern int nMouseCursorSavedX; /* 0x0059a844 */
extern unsigned char bInputMode; /* 0x0059a848 */
extern unsigned char bFilteredKeyWaitActive; /* 0x0059a849 */
extern int bMouseCursorDrawn; /* 0x0059a84c */
extern int nMouseCursorDrawnY; /* 0x0059a8e0 */
extern int nMouseCursorDrawnX; /* 0x0059a8e4 */
extern int anObjectDepthPlaced[WC1_SPACE_OBJECT_COUNT]; /* 0x0059a8f0 */
extern short DAT_0059a9f0;
extern signed char cScreenViewportMode; /* 0x0059a9f2 */
extern const ScreenViewportGeometry * volatile
    pScreenViewportGeometry; /* 0x0059a9f4 */
extern short nEventManagerActive; /* 0x0059a850 */
extern short nViewCenterX; /* 0x0059a852 */
extern short nViewCenterY; /* 0x0059a854 */
extern short nPacketDecompressSourceFile; /* 0x0059a858 */
extern unsigned char abInputKeyState[0x80]; /* 0x0059a860 */
extern short nPacketDecompressInputPosition; /* 0x0059ab00 */
extern void *pPacketDecompressInput; /* 0x0059ab04 */
extern unsigned short wCurrentInputModifiers; /* 0x0059ab08 */
extern MouseCursorState stMouseCursorState; /* 0x0059ab10 */
extern void (*pEventManagerPump)(void); /* 0x0059ab2c */
extern int nPacketDecompressResult; /* 0x0059ab30 */
extern unsigned int nTickCount60Hz; /* 0x0059ab54 */
extern volatile int bFrameTickPending; /* 0x0059ab3c */
extern int bMouseCursorDamagePending; /* 0x0059ab40 */
extern int nMouseCursorDamageRight; /* 0x0059ab44 */
extern int nMouseCursorDamageBottom; /* 0x0059ab48 */
extern short nViewportOriginY; /* 0x0059ab50 */
extern short nViewportOriginX; /* 0x0059ab52 */
extern int nMouseCursorDamageLeft; /* 0x0059ab5c */
extern int nMouseCursorDamageTop; /* 0x0059ab60 */
extern short DAT_0059ab64;
extern int aiInputEventSlotUsed[0x100]; /* 0x0059ab70 */
extern TextContext *pCurrentTextContext; /* 0x0059af8c */
extern short DAT_0059ab34;
extern short nPacketDecompressPending; /* 0x0059ab36 */
extern unsigned short wPacketDecompressInputSize; /* 0x0059ab38 */
extern short nPacketDecompressWorkspaceSegment; /* 0x0059ab3a */
extern int DAT_0059ab4c;
extern unsigned char bEscapePressed; /* 0x0059ab58 */
extern int anSortedObject[WC1_SPACE_OBJECT_COUNT]; /* 0x0059aa00 */
extern FixedVector aObjectViewPosition[WC1_SPACE_OBJECT_COUNT]; /* 0x0059afa0 */
extern short anObjectPitchRotation[WC1_SPACE_OBJECT_COUNT]; /* 0x0059b2a0 */
extern int anShipSpeed[64]; /* 0x0059b320 */
extern short nSpaceFrame; /* 0x0059b420 */
extern FixedVector vPlayerAcceleration; /* 0x0059b460 */
extern short asObjectDistance[WC1_SPACE_OBJECT_COUNT]; /* 0x0059b4a0 */
extern FixedVector aShipRightVector[64]; /* 0x0059b6e0 */
extern FixedVector aShipUpVector[64]; /* 0x0059b9e0 */
extern FixedVector aShipForwardVector[64]; /* 0x0059bce0 */
extern HazardField *pActiveHazardField; /* 0x0059bfe0 */
extern signed char acPlayerComponentDamage[9]; /* 0x0059bff0 */
extern unsigned int anShipAlertFlags[12]; /* 0x0059b430 */
extern int anShipFuel[12]; /* 0x0059b470 */
extern enum ObjectType aeObjectType[WC1_SPACE_OBJECT_COUNT]; /* 0x0059b560 */
extern signed char abFlightPath[WC1_MISSION_OBJECTIVE_COUNT]; /* 0x0059c000 */
extern FixedVector aShipVelocity[WC1_SPACE_OBJECT_COUNT]; /* 0x0059c010 */
extern short anYawGoal[16]; /* 0x0059c310 */
extern short asObjectCounter[WC1_SPACE_OBJECT_COUNT]; /* 0x0059c330 */
extern short nTargetShip; /* 0x0059c3b0 */
extern enum SpecialManeuver aeSpecialManeuver[12]; /* 0x0059c3c0 */
extern enum ShipMissionType aeShipMissionType[12]; /* 0x0059c3f0 */
extern short asShipCount[16]; /* 0x0059c420 */
extern short asShipMaximumSpeed[16]; /* 0x0059c440 */
extern signed char acShipDamage[10]; /* 0x0059c460 */
extern short asViableTargetDistance[16]; /* 0x0059c470 */
extern signed char cMissionObjectiveCount; /* 0x0059c46a */
extern FixedVector aShipPosition[WC1_SPACE_OBJECT_COUNT]; /* 0x0059c490 */
extern short asShipAfterburnerTimer[16]; /* 0x0059c810 */
extern short nShipMissionIndices[10]; /* 0x0059c830 */
extern signed char acShipCommunicator[16]; /* 0x0059c850 */
extern signed char cCurrentNavPointIndex; /* 0x0059c86c */
extern FixedVector vStarFieldMotion; /* 0x0059c860 */
extern short asObjectFlip[WC1_SPACE_OBJECT_COUNT]; /* 0x0059c870 */
extern signed char acLastAttacker[16]; /* 0x0059c910 */
extern signed char acViableTarget[16]; /* 0x0059c920 */
extern short asActionCount[10]; /* 0x0059c930 */
extern FixedVector vPreviousStarFieldMotion; /* 0x0059c900 */
extern short nHazardFieldCount; /* 0x0059c90c */
extern short asObjectRadarRadius[WC1_SPACE_OBJECT_COUNT]; /* 0x0059c790 */
extern short asObjectAfterburnerVelocity[WC1_SPACE_OBJECT_COUNT]; /* 0x0059c9d0 */
extern short asObjectScreenScale[WC1_SPACE_OBJECT_COUNT]; /* 0x0059c950 */
extern CampaignState stCampaignState; /* 0x0059ca50 */
extern unsigned char aShipWeapons[10][0x47]; /* 0x0059cab0 */
extern signed char acShipRating[16]; /* 0x0059cd80 */
extern short asObjectScreenAngle[WC1_SPACE_OBJECT_COUNT]; /* 0x0059cd90 */
extern short nTargetRange; /* 0x0059ce10 */
extern unsigned int anVduModeCache[2]; /* 0x0059ce18 */
extern signed char acObjectOwner[64]; /* 0x0059ce20 */
extern signed char acShipTarget[16]; /* 0x0059ce60 */
extern short nPreviousPitchInput; /* 0x0059ce70 */
extern short nPreviousYawInput; /* 0x0059ce72 */
extern short nPreviousRollInput; /* 0x0059ce74 */
extern short anObjectYawRotation[WC1_SPACE_OBJECT_COUNT]; /* 0x0059ce80 */
extern signed char acTurnRegulator[16]; /* 0x0059cf10 */
extern unsigned char abCollisionAlertTarget[0x10]; /* 0x0059cf20 */
extern int aiPilotLevel[12]; /* 0x0059cf30 */
extern short asTargetListRange[16]; /* 0x0059cf60 */
extern signed char acPilotHitPoints[16]; /* 0x0059cf00 */
extern short asObjectDrawY[WC1_SPACE_OBJECT_COUNT]; /* 0x0059cf80 */
extern short asObjectDrawX[WC1_SPACE_OBJECT_COUNT]; /* 0x0059d000 */
/* Original class[] array.  The recovered ObjectClass order explains all of the
 * observed magic values: 0 null, 1 futurion, 10 mine, 11 missile, 12 ship and
 * 13 capital_ship. */
extern enum ObjectClass aeObjectClass[WC1_SPACE_OBJECT_COUNT]; /* 0x0059d100 */
extern short asPreviousObjectDistance[WC1_SPACE_OBJECT_COUNT]; /* 0x0059d080 */
extern enum ShipObjective aeShipObjective[12]; /* 0x0059d200 */
extern short asObjectViewFrame[WC1_SPACE_OBJECT_COUNT]; /* 0x0059d230 */
extern int nCurrentObjectiveRange; /* 0x0059d2b0 */
extern signed char acWingmanMessageState[16]; /* 0x0059d2c0 */
extern short asCollisionCountdown[16]; /* 0x0059d2d0 */
extern unsigned char *apObjectShape[WC1_SPACE_OBJECT_COUNT]; /* 0x0059d2f0 */
extern short nPitchInput; /* 0x0059d3f0 */
extern short nYawInput; /* 0x0059d3f2 */
extern short nRollInput; /* 0x0059d3f4 */
extern short asShipWingLeader[16]; /* 0x0059d400 */
extern short aasShipArmor[10][4]; /* 0x0059d420 */
extern short asShipWeaponEnergy[10]; /* 0x0059d470 */
extern signed char acFormationMemberList[16]; /* 0x0059d490 */
extern short anShipMissionShip[16]; /* 0x0059d4b0 */
extern FixedVector vToTarget; /* 0x0059d4d0 */
extern short asCannedCommand[16]; /* 0x0059d4e0 */
extern unsigned short ausVduModeStack[16]; /* 0x0059d500 */
extern char acShipSequence[10]; /* 0x0059d520 */
extern short nTargetFacing; /* 0x0059d52a */
extern FixedVector aShipDestination[10]; /* 0x0059d530 */
extern enum ShipTactic aeShipTactic[12]; /* 0x0059d5e0 */
extern short aasShipShield[10][2]; /* 0x0059d5b0 */
extern signed char abShipExhaustHeat[10]; /* 0x0059d610 */
extern short nRenderedSpaceFrame; /* 0x0059d61a */
extern signed char acShipStress[16]; /* 0x0059d620 */
extern short anRollGoal[16]; /* 0x0059d630 */
extern enum Side aeShipSide[12]; /* 0x0059d650 */
extern signed char acShipAiCooldown[16]; /* 0x0059d680 */
extern FixedVector vCollisionDelta; /* 0x0059d690 */
extern signed char acLastCollisionObject[WC1_SPACE_OBJECT_COUNT]; /* 0x0059d6a0 */
extern short aasShipMaximumShield[10][2]; /* 0x0059d6e0 */
extern short asObjectCollisionRadius[64]; /* 0x0059d710 */
extern signed char acShipPointingMode[16]; /* 0x0059d790 */
extern short anPitchGoal[16]; /* 0x0059d7a0 */
extern signed char abShipNavPointIndex[16]; /* 0x0059d7c0 */
extern signed char acTurnInterval[16]; /* 0x0059d7d0 */
extern short anObjectRollRotation[WC1_SPACE_OBJECT_COUNT]; /* 0x0059d7e0 */
extern signed char abShipTurn[16]; /* 0x0059d860 */
extern short nFacingToTarget; /* 0x0059d920 */
extern short asObjectScreenY[WC1_SPACE_OBJECT_COUNT]; /* 0x0059d930 */
extern short asObjectScreenX[WC1_SPACE_OBJECT_COUNT]; /* 0x0059d9b0 */
extern signed char cCockpitView; /* 0x0059dab0 */
extern HazardField aHazardFields[7]; /* 0x0059d870 */
extern MissionObjective aMissionObjectives[WC1_MISSION_OBJECTIVE_COUNT]; /* 0x0059dac0 */
extern enum ShipManeuver aeShipManeuver[12]; /* 0x0059dcb0 */
extern const short *apCannedSequence[12]; /* 0x0059dce0 */
extern FixedVector aShipMissionSpot[10]; /* 0x0059dd10 */
extern short asCapitalShipViewFrame[16]; /* 0x0059dd90 */
extern signed char acObjectCollisionGraceTicks[ /* 0x0059ddb0 */
    WC1_SPACE_OBJECT_COUNT];
extern ObjectResourceSlot aObjectResourceSlots[4]; /* 0x0059ddf0 */
extern signed char acShipDestroyedWeaponCount[16]; /* 0x0059de30 */
extern short asObjectScale[WC1_SPACE_OBJECT_COUNT]; /* 0x0059de40 */
extern short asShipAccumulatedDamage[WC1_SPACE_OBJECT_COUNT]; /* 0x0059dee0 */
extern signed char acShipIonDriveDamage[16]; /* 0x0059d4a0 */
extern short asObjectAnimationDelay[WC1_SPACE_OBJECT_COUNT]; /* 0x0059b660 */
extern short asObjectAnimationIndex[WC1_SPACE_OBJECT_COUNT]; /* 0x0059da30 */
extern short nEyeRollGoal; /* 0x0059c8f0 */
extern short nEyeYawGoal; /* 0x0059c944 */
extern short nEyePitchGoal; /* 0x0059d61c */
extern signed char acVduModeStackDepth[16]; /* 0x0059dec0 */
extern signed char acShipSpawnNavPoint[16]; /* 0x0059ded0 */
extern short nCurrentNavPoint; /* 0x0059df60 */
extern int nJoystickCalibrationMinimumY; /* 0x0059df64 */
extern int nJoystickCalibrationMinimumX; /* 0x0059df68 */
extern int nJoystickCalibrationMaximumX; /* 0x0059df6c */
extern int nJoystickCalibrationMaximumY; /* 0x0059df70 */
extern unsigned short awPaletteEntryAllocation[256]; /* 0x0059df80 */
extern unsigned char *apCommPortraitShapes[16]; /* 0x0059e180 */
extern char szHudMessageBuffer[0x60]; /* 0x0059e1c0 */
extern char *aapszPilotSpeech[14][11]; /* 0x0059e220 */
extern signed char abCommMenuChoiceCommand[7]; /* 0x0059e488 */
extern const char *pszCommMenuHeading; /* 0x0059e490 */
extern char szCommMenuHeadingBuffer[0x40]; /* 0x0059e4a0 */
extern const char *apszCommMenuChoiceText[7]; /* 0x0059e4e0 */
extern void *apPacketHandles[0x1000]; /* 0x0059e530 */
extern int aiPacketHandleOffsets[0x1000]; /* 0x005a2530 */
extern int nPacketHandleCount; /* 0x005a6530 */
extern Viewport *pAllocatedScreenViewport; /* 0x005a6534 */
extern unsigned short awScreenRowOffsets[202]; /* 0x005a6540 */
extern int nScreenAllocationState; /* 0x005a66e0 */
extern Viewport *pAllocatedScreenViewportMirror; /* 0x005a66e4 */
extern int aiSoundEffectSourceActive[0x41]; /* 0x005a66ec */
extern unsigned char abSoundPlaybackSlots[0xf8]; /* 0x005a67f0 */
extern int nPassingShipSoundCooldown; /* 0x005a68e8 */
extern int *DAT_005a6538;
extern int nAllocateViewportCalls; /* 0x005a68ec */
extern void *pLastPacketAllocation; /* 0x005a68f0 */
extern FireworkState aFireworks[30]; /* 0x005a6900 */
extern unsigned char *pFireworkShape; /* 0x005a6a68 */
extern ConstellationStar aConstellationStars[10]; /* 0x005a6a70 */
extern Viewport *pConstellationViewport; /* 0x005a6aac */
extern short nConstellationStarCount; /* 0x005a6ab0 */
extern ConstellationParticle aConstellationParticles[16]; /* 0x005a6ac0 */
extern Viewport stConstellationViewport; /* 0x005a6b40 */
extern short nConstellationParticleCount; /* 0x005a6b54 */
extern short nExternalViewDistance; /* 0x005a6b5c */
extern Viewport stPilotHand; /* 0x005a6b60 */
extern Viewport stLeftVdu; /* 0x005a6b80 */
extern ScreenViewportPacket *pScreenViewportPacket; /* 0x005a6b94 */
extern Viewport stScreen; /* 0x005a6ba0 */
extern Viewport stCockpitBar; /* 0x005a6be0 */
extern TextContext stHudMessageTextContext; /* 0x005a6bc0 */
extern unsigned char *pTargetLockShape; /* 0x005a6bf4 */
extern unsigned char *apTextFonts[4]; /* 0x005a6c00 */
extern FontWorkspace **apFontWorkspaces[4]; /* 0x005a6c10 */
extern FixedVector aDirectionViewRightVector[ /* 0x005a6c20 */
    WC1_DIRECTION_VIEW_COUNT];
extern FixedVector aDirectionViewUpVector[ /* 0x005a6f10 */
    WC1_DIRECTION_VIEW_COUNT];
extern FixedVector aDirectionViewForwardVector[ /* 0x005a7200 */
    WC1_DIRECTION_VIEW_COUNT];
extern TextContext stLeftVduTextContext; /* 0x005a74f0 */
extern Viewport stSpaceBuffer; /* 0x005a7510 */
extern Viewport stRightVdu; /* 0x005a7530 */
extern Viewport stPilotHandBackdrop; /* 0x005a7550 */
extern unsigned char *pCockpitWeaponShape; /* 0x005a7564 */
extern Viewport stConversationTextViewport; /* 0x005a7570 */
extern char szDefaultTextBuffer[0xc8]; /* 0x005a7590 */
extern Viewport stModalSourceViewport; /* 0x005a7670 */
extern unsigned char *pCockpitIndicatorShape; /* 0x005a7658 */
extern unsigned char *pCommMenuCursorShape; /* 0x005a7660 */
extern unsigned char *pPilotHandShape; /* 0x005a7684 */
extern Viewport stPilotHandComposite; /* 0x005a7690 */
extern Viewport stSceneBuffer; /* 0x005a76b0 */
extern unsigned char *pCockpitDamageShape; /* 0x005a76f4 */
extern short aPaletteFadeEntries[6][3]; /* 0x005a76d0 */
extern TextContext stRightVduTextContext; /* 0x005a7700 */
extern TextContext stCockpitReadoutTextContext; /* 0x005a7720 */
extern TextContext stDefaultTextContext; /* 0x005a7740 */
extern TextContext stConversationTextContext; /* 0x005a7760 */
extern short asDamageFlashColour[3]; /* 0x005a7780 */
extern short nDamageDisplayTicks; /* 0x005a7786 */
extern short nWeaponDisplayOriginX; /* 0x005a7788 */
extern short nWeaponDisplayOriginY; /* 0x005a778a */
extern char szDamageStatusText[0x48]; /* 0x005a7790 */
extern ShortPoint stDamageSpritePosition; /* 0x005a77d8 */
extern signed char cDamageDisplayComponent; /* 0x005a77dc */
extern signed char cDamageDisplayFrame; /* 0x005a77dd */
extern signed char cDamagedComponentCount; /* 0x005a77de */
extern int nDamageDisplayState; /* 0x005a77e0 */
extern int nDamageDisplayPhase; /* 0x005a77e4 */
extern unsigned char *pLaunchDoorShape; /* 0x005a77e8 */
extern unsigned int nFrameTimerId; /* 0x005a77ec */
extern unsigned char abPaletteTriplets[256][3]; /* 0x005a77f0 */
extern InputDeviceSample stPreviousFlightInput; /* 0x005a7af0 */
extern short bMouseButtonEventQueued; /* 0x005a7afc */
extern short bKeyboardEventQueued; /* 0x005a7afe */
extern short bMouseMoveEventQueued; /* 0x005a7b00 */
extern char *pStartupArguments[30]; /* 0x005a7b10 */
extern short bJoystickEventQueued; /* 0x005a7b88 */
extern short nArcadeStartupParameter; /* 0x005a7b8a */
extern Viewport stTrainSimTitleDisplayViewport; /* 0x005a7b90 */
extern Viewport stTrainSimHighScoreBufferViewport; /* 0x005a7bb0 */
extern int nArcadeScore; /* 0x005a7bc4 */
extern TextContext stTrainSimTextContext; /* 0x005a7bd0 */
extern Viewport stTrainSimHighScoreDisplayViewport; /* 0x005a7bf0 */
extern TextContext stTrainSimHighScoreTextContext; /* 0x005a7c10 */
extern short nArcadeTimeRemaining; /* 0x005a7c2c */
extern HighScoreEntry aHighScoreEntries[6]; /* 0x005a7c30 */
extern int nArcadeWaveBonus; /* 0x005a7c50 */
extern unsigned char *pSceneAnimationSceneData; /* 0x005a7c54 */
extern unsigned char *pSceneAnimationPrimaryShape; /* 0x005a7c58 */
extern unsigned char *pSceneAnimationTextData; /* 0x005a7c5c */
extern unsigned char *pSceneAnimationPacket; /* 0x005a7c60 */
extern SceneAnimationObject *pSceneAnimationObjects; /* 0x005a7c64 */
extern short nSceneAnimationWaitFrames; /* 0x005a7c68 */
extern unsigned char *pSceneAnimationDefinitions; /* 0x005a7c6c */
extern unsigned char *pSceneAnimationSecondaryShape; /* 0x005a7c70 */
extern signed char cCockpitLogicalFile; /* 0x005a7c74 */
extern unsigned char *apCockpitShapes[4]; /* 0x005a7c80 */
extern unsigned char *pCinematicViewBackdrop; /* 0x005a7c90 */
extern unsigned char *pRearViewBackdrop; /* 0x005a7c94 */
extern int bPlayerDestroyed; /* 0x005a7c98 */
extern short nPlayerKillCount; /* 0x005a7c9c */
extern short asCollisionPartner[10]; /* 0x005a7cc0 */
extern short asCollisionTime[10]; /* 0x005a7ca0 */
extern int nWingmanKilledThisMission; /* 0x005a7cb4 */
extern short nWingmanKillCount; /* 0x005a7cb8 */
extern short nLastFoundShip; /* 0x005a7cba */
extern int nMemoryConfiguration; /* 0x005a7cd4 */
extern unsigned int dwOriginalFreeMemory; /* 0x005a7cd8 */
extern unsigned char *pMouseCursorResource; /* 0x005a7cdc */
extern int nAvailableGameMemory; /* 0x005a7ce0 */
extern int nSceneResourceBudget; /* 0x005a7ce4 */
extern int nAfterburnerSoundDeadline; /* 0x005a7ce8 */
extern unsigned char *pScannerMarkerBackground; /* 0x005a7dc4 */
extern int bAfterburnerSfxActive; /* 0x005a7cec */
extern int bPointerMovedByKeyboard; /* 0x005a7d54 */
extern FixedVector vNormalizedToTarget; /* 0x005a7db0 */
extern short nReleaseWeaponDisplayX; /* 0x005a7dbc */
extern short nReleaseWeaponDisplayY; /* 0x005a7dbe */
extern enum ObjectType eReleaseWeaponDisplayType; /* 0x005a7dc0 */
extern unsigned char bStickIndicatorFrame; /* 0x005a7dc8 */
extern short nMessageTimer; /* 0x005a7dca */
extern short nPendingCockpitDamage; /* 0x005a7dcc */
extern HudMessageSlot aHudMessageSlots[2]; /* 0x005a7dd0 */
extern signed char cPreviousTargetObject; /* 0x005a7df2 */
extern int nSavedMouseCursorY; /* 0x005a7df4 */
extern int nSavedMouseCursorX; /* 0x005a7df8 */
extern char szComponentHitMessage[40]; /* 0x005a7e00 */
extern short nTargetLockMarkerY; /* 0x005a7e28 */
extern short nCarrierMissionShipIndex; /* 0x005a7e2a */
extern CockpitReadout aCockpitReadouts[6]; /* 0x005a7e30 */
extern short nScannerCursorX; /* 0x005a7e6c */
extern short nScannerCursorY; /* 0x005a7e6e */
extern short asScannerObjectY[11]; /* 0x005a7e80 */
extern volatile ShortPoint stCockpitExplosionPosition; /* 0x005a7e98 */
extern short asScannerObjectX[11]; /* 0x005a7ea0 */
extern short asScannerBackgroundColour[10]; /* 0x005a7ed0 */
extern int nDamageAlarmSfxHandle; /* 0x005a7ec0 */
extern unsigned short DAT_005a7f00;
extern short nExternalViewAngle; /* 0x005a7f06 */
extern int nViewportAllocationCount; /* 0x005a7f0c */
extern unsigned char *apViewportAllocations[128]; /* 0x005a7f10 */
extern int nFreeViewportCalls; /* 0x005a8110 */
extern short nPendingCampaignIndex; /* 0x005a8114 */
extern short nMissionMedalScore; /* 0x005a8116 */
extern short nCampaignDataSet; /* 0x005a8118 */
extern volatile int nNearHeapSize; /* 0x005a811c */
extern volatile int nNearHeapBase; /* 0x005a8120 */
extern int nNearHeapFirstDescriptor; /* 0x005a8124 */
extern void * volatile pNearHeapAllocation; /* 0x005a8128 */
extern short nNavMapMaximumX; /* 0x005a812c */
extern short nNavMapMinimumX; /* 0x005a812e */
extern unsigned short awNavObjectiveLabelIndex[16]; /* 0x005a8130 */
extern short nNavMapMaximumY; /* 0x005a8150 */
extern short nNavMapCentreX; /* 0x005a8152 */
extern short nNavMapMinimumY; /* 0x005a8154 */
extern TextContext stNavMapTextContext; /* 0x005a8160 */
extern short nNavMapCentreY; /* 0x005a817c */
extern TextContext stNavLabelTextContext; /* 0x005a8180 */
extern int DAT_005a7d9c;
extern DiskFileRecord *pDiskFileRecords; /* 0x005a7cf0 */
extern Viewport stDiskPromptBackgroundViewport; /* 0x005a7d00 */
extern unsigned char abDiskPromptDriveState[2]; /* 0x005a7d20 */
extern Viewport stDiskPromptViewport; /* 0x005a7d40 */
extern TextContext stDiskPromptTextContext; /* 0x005a7d60 */
extern unsigned int dwDiskPromptTopLeft; /* 0x005a7d80 */
extern unsigned int dwDiskPromptBottomRight; /* 0x005a7d84 */
extern short *pPaletteTransitionDirection; /* 0x005a7d88 */
extern short *pPaletteTransitionDelta; /* 0x005a7d8c */
extern short nPaletteTransitionMaxDelta; /* 0x005a7d90 */
extern short *pPaletteTransitionAccumulator; /* 0x005a7d94 */
extern short nPaletteTransitionCountdown; /* 0x005a7d98 */
extern signed char cCapitalShipLogicalFile; /* 0x005a7da0 */
extern signed char cObjectResourceLogicalFile; /* 0x005a86b0 */
extern CampaignDate *pCurrentCampaignDate; /* 0x005a86a8 */
extern CampaignDate *pElapsedCampaignDate; /* 0x005a86ac */
extern Viewport *pScrambleViewport; /* 0x005a86b4 */
extern short nScrambleCanopyFrame; /* 0x005a86b8 */
extern unsigned char *pScrambleDetailShape; /* 0x005a86bc */
extern short nScrambleCockpitScale; /* 0x005a86c0 */
extern short nScrambleCockpitDetailX; /* 0x005a86c4 */
extern short nScrambleCockpitDetailY; /* 0x005a86c6 */
extern unsigned char *pConversationSpecialShape; /* 0x005a86ec */
extern short nFuneralCasketX; /* 0x005a86c8 */
extern short nFuneralCasketY; /* 0x005a86ca */
extern short nScrambleShipDetailCount; /* 0x005a86cc */
extern signed char acScrambleShipDetailIndices[24]; /* 0x005a86d0 */
extern signed char cScrambleWalkTicks; /* 0x005a86e8 */
extern ShortPoint aFuneralParticles[7]; /* 0x005a86f0 */
extern unsigned char *pScrambleBackgroundShape; /* 0x005a870c */
extern short nFuneralParticleDistance; /* 0x005a8710 */
extern short nScrambleBackgroundY; /* 0x005a8712 */
extern short nScrambleBackgroundRightX; /* 0x005a8714 */
extern short nFuneralForegroundX; /* 0x005a8718 */
extern short nFuneralForegroundY; /* 0x005a871a */
extern short nFuneralMainScale; /* 0x005a871c */
extern short nFuneralRifleFrame; /* 0x005a871e */
extern short nFuneralBaseX; /* 0x005a8720 */
extern short nFuneralBaseY; /* 0x005a8722 */
extern short nScrambleShipX; /* 0x005a8724 */
extern short nScrambleShipY; /* 0x005a8726 */
extern short nFuneralParticleScale; /* 0x005a8728 */
extern unsigned char *pScrambleHangarShape; /* 0x005a872c */
extern unsigned char *pScrambleCockpitShape; /* 0x005a8730 */
extern short nScrambleWalkerY; /* 0x005a8734 */
extern short nScrambleCanopyOffset; /* 0x005a8736 */
extern short nFuneralMainDistance; /* 0x005a8738 */
extern signed char bScrambleCanopyClosed; /* 0x005a873a */
extern short nFuneralGuardFrame; /* 0x005a873c */
extern short nScrambleOverlayX; /* 0x005a8740 */
extern short nScrambleOverlayY; /* 0x005a8742 */
extern unsigned char *pScrambleOverlayShape; /* 0x005a8744 */
extern signed char cScrambleWalkerPair; /* 0x005a8748 */
extern unsigned char *pScrambleCanopyShape; /* 0x005a874c */
extern unsigned char *pScrambleShipShape; /* 0x005a8750 */
extern short nTalkingHeadFaceX; /* 0x005a8754 */
extern short nTalkingHeadFaceY; /* 0x005a8756 */
extern short nTalkingHeadMouthY; /* 0x005a8758 */
extern short nTalkingHeadMouthX; /* 0x005a875a */
extern char szSoundDebugMessage[512]; /* 0x005a8760 */
extern short nHomeMissionShipIndex; /* 0x005a8692 */
extern short nMissionEntryNavPoint; /* 0x005a8690 */
extern short nPlayerMissionShipIndex; /* 0x005a8694 */
extern short nInitialMissionShipIndices[8]; /* 0x005a8696 */
extern short DAT_005a86a6;
extern unsigned char abMissionAuxData[0x28]; /* 0x005a8210 */
extern unsigned char abSeriesAuxData[0x28]; /* 0x005a8240 */
extern MissionObjectiveSource aMissionObjectiveSources[16]; /* 0x005a8270 */
extern unsigned char *pTitleShape; /* 0x005a7f08 */
extern unsigned char *pConstellationShape; /* 0x005a765c */
extern const short *pViewScript; /* 0x005a6b58 */
extern unsigned char *pIntroFont; /* 0x005a8960 */
extern unsigned int nSystemKeyDown; /* 0x005a8964 */
extern JOYINFO aJoystickInfo[2]; /* 0x005a8970 */
extern GuardedAllocation *pGuardedAllocationTail; /* 0x005a89ac */
extern char szMemoryUsage[0x80]; /* 0x005a89b0 */
extern HWND hMainWindow; /* 0x005a89a0 */
extern HANDLE hSingleInstanceSemaphore; /* 0x005a89a4 */
extern HDC hMainWindowDC; /* 0x005a8a30 */
extern HCURSOR hPreviousCursor; /* 0x005a8a34 */
extern unsigned int nSessionEndTime; /* 0x005a8a38 */
extern int bMainWindowAlive; /* 0x005a8a3c */
extern HINSTANCE hAppInstance; /* 0x005a8a40 */
extern unsigned int nSessionStartTime; /* 0x005a8a44 */
extern unsigned short awPaletteRgbWords[0x300]; /* 0x005a8a50 */
extern const char *PTR_s_MISSILE_LOCKED_004691d4;
extern unsigned char bTargetBracketVisible; /* 0x004691d8 */
extern unsigned char abTargetBracketPadding[7]; /* 0x004691d9 */
extern short asHudMessageOrigins[10]; /* 0x004691e0 */
extern const char *apszCommMenuText[13]; /* 0x0046af90 */
extern const char *apszKilrathiAceNames[4]; /* 0x0046af80 */
extern const char aszCommMenuText[0xe8]; /* 0x0046aff8 */
extern ShortPoint aaCockpitDamagePositions[5][4]; /* 0x00469228 */
extern unsigned long dwGameClockBase; /* 0x005a89a8 */

extern unsigned char abCockpitLightState[7]; /* 0x005a7e70 */
extern unsigned char abCockpitLightGoal[7]; /* 0x005a7eb8 */
extern unsigned int anCockpitDamageState[4]; /* 0x005a7ef0 */
extern MouseCursorState stHostMouseState; /* 0x0059af70 */
extern int nFrameIntervalMs; /* 0x0046b1b8 */
extern int nFrameDeadlineMs; /* 0x0046b1bc */
extern int nPreviousFrameTimeMs; /* 0x0046b1c4 */
extern int bSpaceFlightFrameTiming; /* 0x0046b1c8 */
extern float fSpaceFlightFrameRate; /* 0x0046b1cc */
extern float fCinematicFrameRate; /* 0x0046b1d0 */
extern unsigned int dwStreamerState; /* 0x00597cd0 */
extern short bExpandedMemoryAvailable; /* 0x0059a856 */
extern int bCampaignStartupMode; /* 0x004688e0 */
extern NavMapLabel aNavMapLabels[20]; /* 0x00475e80 */
extern ShortRect aNavMapExclusionRects[21]; /* 0x00475f48 */
extern short nScriptedViewObject; /* 0x0046a8d0 */
extern int bScriptedView; /* 0x0046a8d4 */
extern int nSfxVolumeSetting; /* 0x00469fbc */
extern int nMusicVolumeSetting; /* 0x00469fc0 */
extern int bViewportDirty; /* 0x00469fc4 */
extern int anVolumeLevels[11]; /* 0x00469fc8 */
extern int nInputTickScale; /* 0x0059af90 */
extern short asInputButton1DoubleClick[2]; /* 0x0059e508 */
extern short asInputButton1Changed[2]; /* 0x0059e50c */
extern short asInputButton2Changed[2]; /* 0x0059e510 */
extern short asPreviousInputButtons[2]; /* 0x0059e514 */
extern int anInputButton1PressTime[2]; /* 0x0059e518 */
extern int anInputButton2PressTime[2]; /* 0x0059e500 */
extern short asInputButton2DoubleClick[2]; /* 0x0059e520 */

extern short nActiveInputDevice; /* 0x005a819c */
extern int nJoystickVerticalDeadZone; /* 0x005a81a0 */
extern int nJoystickHorizontalDeadZone; /* 0x005a81a4 */
extern int nJoystickUpScale; /* 0x005a81a8 */
extern int nJoystickLeftScale; /* 0x005a81ac */
extern int nJoystickMaximumX; /* 0x005a81b0 */
extern int nJoystickMaximumY; /* 0x005a81b4 */
extern int nJoystickMinimumX; /* 0x005a81b8 */
extern int nJoystickMinimumY; /* 0x005a81bc */
extern int nJoystickRawX; /* 0x005a81c0 */
extern int nJoystickRawY; /* 0x005a81c4 */
extern int nJoystickVerticalRange; /* 0x005a81c8 */
extern int nJoystickHorizontalRange; /* 0x005a81cc */
extern int nJoystickRightScale; /* 0x005a81d0 */
extern int nJoystickDownScale; /* 0x005a81d4 */
extern int nJoystickCentreY; /* 0x005a81d8 */
extern int nJoystickCentreX; /* 0x005a81dc */
extern int nJoystickFailureValue; /* 0x005a81e0 */
extern InputDeviceSample aInputDeviceSamples[2]; /* 0x005a81f0 */
extern short nMenuInputRepeatDelay; /* 0x005a8208 */
extern int nHostMouseMessageX; /* 0x005a8990 */
extern int nHostMouseMessageY; /* 0x005a8994 */
extern int bHostPrimaryMouseButton; /* 0x005a8998 */
extern int bHostSecondaryMouseButton; /* 0x005a899c */

extern ObjectTypeData aObjectTypeData[OBJECT_TYPE_COUNT]; /* 0x00466458 */
extern const ShortVector aChildOffsets[56]; /* 0x004682f0 */
extern const ShortPoint aWeaponDisplayPositions[32]; /* 0x00468440 */
extern const ShortPoint aWeaponDisplayOrigins[5]; /* 0x004684c0 */
extern LPDIRECTDRAWSURFACE pPrimarySurface; /* 0x0046b1a8 */
extern LPDIRECTDRAWSURFACE pSecondarySurface; /* 0x0046b1ac */
extern LPDIRECTDRAWPALETTE pDirectDrawPalette; /* 0x0046b1b0 */
extern int   nDIBCascadeLevel; /* 0x0046b1b4 */
extern char  szDIBErrorMessage[0x98]; /* 0x00486078 */

extern unsigned char bMessageSpeed; /* 0x0046af68 */
extern unsigned char abManeuverRerollChance[47]; /* 0x00465678 */
extern void (*apShipAiManeuverHandlers[47])(short, short); /* 0x004656a8 */

extern ShortVector aShipFormationOffset[10]; /* 0x0059b520 */

/* Ordered name tables read directly out of the image; see include/wcdata.h.
 * Both are packed variable-length strings, not fixed-stride arrays, so the
 * game indexes them through a pointer table rather than by multiplication. */
extern const char aszObjectTypeDisplayNames[]; /* Hornet .. Star post, then weapons */ /* 0x004684d4 */
extern const char aszKilrathiAceNames[];  /* Bhurak, Dakhath, Khajja, Bakhtosh */ /* 0x0046afd4 */

#endif /* WC1_GLOBALS_H */
