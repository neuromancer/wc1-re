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
 *
 *  Declarations follow address order except where code requires otherwise.
 *  CL 4.20 breaks register and operand-order ties by symbol index, and a
 *  global keeps its own index in every function that uses it, so a few
 *  externs sit where the original header must have had them
 *  (docs/PATTERNS.md, "Symbol-index costs").
 */
#ifndef GAME_GLOBALS_H
#define GAME_GLOBALS_H

extern const int anRLEQuarterCosine[901]; /* 0x0043d4bf */
extern const unsigned char abIffBmhdChunkId[4]; /* 0x0043e778 */
extern const unsigned char abIffCmapChunkId[4]; /* 0x0043e77c */
extern const unsigned char abIffBodyChunkId[4]; /* 0x0043e780 */
extern short nHazardReferenceSpeed; /* 0x00465040 */
extern short nActiveHazards; /* 0x00465044 */
extern int bIxAudioEnabled; /* 0x00465058 */
extern short nPacketError; /* 0x00465460 */
extern unsigned int bKeyEventQueueEnabled; /* 0x0046505c */
extern int bShowKilrathiSagaCredits; /* 0x0046506c */
extern unsigned int nGuardedAllocationTotalBytes; /* 0x00465060 */
extern unsigned int nGuardedAllocationBytes; /* 0x00465064 */
extern unsigned int nGuardedAllocationPeakBytes; /* 0x00465068 */
extern int bShowFrameRate; /* 0x00465070 */
extern int bDirectDrawModeCascadeEnabled; /* 0x00465074 */
extern int bCockpitEnabled; /* 0x0046507c */
extern volatile int bMainWindowMinimized; /* 0x00465080 */
extern int bWindowMessagePumpActive; /* 0x004650a8 */
extern unsigned int bF1KeyLatch; /* 0x004650ac */
extern GuardedAllocation *pGuardedAllocationHead; /* 0x004650b0 */
extern short nDebriefingPersonality; /* 0x00465c80 */
extern void *aapPacketReferences[4][0x25]; /* 0x00465c88 */
#ifdef SDL_PORT
extern ShortVector aaFormationPositions[5][8]; /* 0x00465ed8 */
#endif
extern short nNavMapCoordinateScaling; /* 0x00468660 */
extern short nNavMapScale; /* 0x00468664 */
extern int bInflightComputerActive; /* 0x00468754 */
extern short nNearHeapMaxDescriptors; /* 0x004688c4 */
extern int bOfficeVisitPending; /* 0x004688cc */
extern short nEnemySighting; /* 0x00465c7c */
extern int bPromotionPending; /* 0x004688d0 */
extern int bPlayerEjectedThisMission; /* 0x004688d4 */
extern int bPlayerShipTypeChanged; /* 0x004688d8 */
extern short nPreviousPlayerShipType; /* 0x004688dc */
extern int bCampaignActive; /* 0x004688f0 */
extern char *pszPendingHudMessage; /* 0x00469004 */
extern unsigned char *pCockpitExplosionBackground; /* 0x00469060 */
extern short nNavPointerObject; /* 0x00469208 */
extern unsigned char *pCockpitExplosionShape; /* 0x00469064 */
extern short nStartNavPointOverride; /* 0x0046a010 */
extern unsigned char *pReleaseWeaponDisplayBackground; /* 0x0046906c */
extern unsigned char *pConfedCommBackground; /* 0x00469278 */
extern unsigned char *pCommStaticShape; /* 0x0046927c */
extern unsigned char *pKilrathiCommBackground; /* 0x00469280 */
extern short asCampaignPilotFiles[3]; /* 0x00469450 */
extern short asCampaignBriefingFiles[3]; /* 0x00469458 */
extern short asMissionDataFiles[3]; /* 0x00469460 */
extern char *pGameVersion; /* 0x004693b4 */
extern DWORD dwDebugOverlayKey; /* 0x00469648 */
extern DWORD dwDebugOverlayKeyLatch; /* 0x0046964c */
extern short nDiskPromptBorderColour; /* 0x00469694 */
extern short bSlowSceneAnimation; /* 0x00469998 */
extern unsigned char cBlackColour; /* 0x0046999c */
extern unsigned char cViewportClearColour; /* 0x004699a0 */
extern unsigned char cBlueColour; /* 0x004699a4 */
extern unsigned char cYellowColour; /* 0x004699a8 */
extern unsigned char cRedColour; /* 0x004699ac */
extern unsigned char cDarkGreenColour; /* 0x004699b0 */
extern unsigned char cPrimaryTextColour; /* 0x004699b4 */
extern unsigned char cOrangeColour; /* 0x004699b8 */
extern unsigned char cDarkGreyColour; /* 0x004699c0 */
extern unsigned char cLightGreyColour; /* 0x004699c4 */
extern unsigned char cMagentaColour; /* 0x004699c8 */
extern unsigned char cDefaultTextColour; /* 0x004699cc */
extern unsigned char cAsteroidColour; /* 0x004699d4 */
extern unsigned char cPrimaryViewBufferColour; /* 0x004699d8 */
extern short asConversationTextColours[24]; /* 0x004699f0 */
extern int bGraphicsActive; /* 0x00469a20 */
extern PacketResourceDescriptor aCommon3SpaceResources[12]; /* 0x00469bc0 */
extern int nCapitalShipViewDistance; /* 0x00468ff4 */
extern unsigned char bLandingAuthorized; /* 0x00468ff8 */
extern unsigned char *pCockpitPilotShape; /* 0x0046905c */
extern char szInsufficientMemory[100]; /* 0x00469a28 */
extern unsigned int bIntroSceneResourcesActive; /* 0x00469d48 */
extern PacketResourceDescriptor aMissionResourceDescriptors[5]; /* 0x00469c20 */
extern PacketResourceDescriptor aCockpitResourceDescriptors[19]; /* 0x00469c48 */
extern TitleMenuRegion aTrainSimMissionRegions[5]; /* 0x00469df8 */
extern int nTrainSimActive; /* 0x00469e2c */
extern short nTrainSimMission; /* 0x00469e30 */
extern short nArcadeWave; /* 0x00469e34 */
extern int nCannedSceneMode; /* 0x00469fac */
extern int nArcadeState; /* 0x00469fb0 */
extern short nFrameSkipCounter; /* 0x00469fb4 */
extern short nFrameSkip; /* 0x00469fb8 */
#ifdef WC1_WINMAIN_SOURCE
/* The Win32 configuration reader declares three of main.c's int flags as
 * bytes: it stores single bytes, and only byte-typed globals give its 0 and
 * '-' constants their original register allocation. */
extern char nOriginDevUnlock; /* 0x00469ff4 */
extern int bPlayerCollisionsEnabled; /* 0x00469ff8 */
extern char bPlayerVulnerable; /* 0x00469ffc */
extern char bPlayerCollisionResponse; /* 0x0046a000 */
#else
extern int nOriginDevUnlock; /* 0x00469ff4 */
extern int bPlayerCollisionsEnabled; /* 0x00469ff8 */
extern int bPlayerVulnerable; /* 0x00469ffc */
extern int bPlayerCollisionResponse; /* 0x0046a000 */
#endif
extern int bViewBufferEnabled; /* 0x0046a004 */
extern short nCockpitExplosionFrame; /* 0x00469068 */
extern int bCockpitlessView; /* 0x0046a008 */
extern short nArcadeBonusCountdown; /* 0x0046a014 */
extern int bMouseCursorVisible; /* 0x0046a018 */
extern unsigned char bInputPollingGuard; /* 0x0046a01c */
extern unsigned char *pDamageDisplayBackground; /* 0x0046a748 */
extern const char *apszComponentNames[9]; /* 0x0046a778 */
extern short nMusicTrackComplete; /* 0x0046aa04 */
extern short nFuneralSequenceActive; /* 0x0046aa10 */
extern int nCurrentMusicTrack; /* 0x0046aa14 */
extern volatile int nInFlightMusicSlotA; /* 0x0046aa24 */
extern volatile int nInFlightMusicSlotB; /* 0x0046aa2c */
extern int nWaitForMusicEnabled; /* 0x0046aa30 */
extern int nFlightSoundEffectsEnabled; /* 0x0046aa34 */
extern int nInitialFlightMusicPending; /* 0x0046aa38 */
extern int nCombatMusicActive; /* 0x0046aa3c */
extern short nMusicPlaybackMode; /* 0x0046a9f8 */
extern int nInFlightMusicActive; /* 0x0046aa40 */
extern unsigned char aabSoundEffectDescriptors[37][8]; /* 0x0046aa50 */
extern short nCommMenuChoiceCount; /* 0x0046af60 */
extern short nCommMenuReuseMode; /* 0x0046af64 */
extern unsigned char bRadioSilence; /* 0x0046af70 */
extern unsigned char bVideoImagesSuppressed; /* 0x0046af74 */
extern unsigned char bCommVideoEnabled; /* 0x0046af78 */
extern signed char cCommMenuRecipient; /* 0x0046afc4 */
extern short nCommSpeakerRating; /* 0x0046afcc */
extern short nCommPortraitIndex; /* 0x0046afd0 */
extern short nInputDoubleClickInterval; /* 0x0046af54 */
extern short nMenuPointerSpeed; /* 0x0046af58 */
extern short nVideoMode; /* 0x0046b168 */
extern signed char acGraphicsModeByAnimationSpeed[8]; /* 0x0046b178 */
extern CockpitLayout stCockpitLayout; /* 0x0046e008 */
extern short nCommSpeakerObject; /* 0x0046afc8 */
extern short aasCockpitLightX[5][7]; /* 0x0046dca8 */
extern short aasCockpitLightY[5][7]; /* 0x0046dcf0 */
extern signed char aacCockpitLightOffFrame[5][7]; /* 0x0046dd38 */
extern signed char aacCockpitLightOnFrame[5][7]; /* 0x0046dd60 */
extern CockpitBarDefinition aaCockpitBars[5][8]; /* 0x0046dd88 */
extern const GUID guidDirectDraw2; /* 0x00463118 */
extern signed char bCurrentKey; /* 0x0046c014 */
extern signed char cPreviousKey; /* 0x0046c018 */
extern short nExternalViewShip; /* 0x0046c040 */
extern signed char cViewObject; /* 0x0046c000 */
extern int bIntroSecondaryScene; /* 0x0046c024 */
extern signed char abHazardObjects[0x14]; /* 0x0046c028 */
extern short DAT_0046c010;
extern short nEyePitchRate; /* 0x0046c004 */
extern short nEyeYawRate; /* 0x0046c008 */
extern short nEyeRollRate; /* 0x0046c00c */
extern signed char cCurrentObjective; /* 0x0046c020 */
extern short nCurrentWave; /* 0x0046c01c */
extern int nCameraViewMode; /* 0x0046c03c */
extern short nEjectedPilotObject; /* 0x0046c044 */
extern short nClosestVisibleObject; /* 0x0046c048 */
extern short nPlayerCollisionObject; /* 0x0046c050 */
extern enum ObjectType eSelectedGunType; /* 0x0046c054 */
#if !defined(WC1_SHIP_SOURCE) && !defined(WC1_BRAINS_SOURCE) && \
    !defined(WC1_AUTO_SOURCE)
extern short nYourWingman; /* 0x0046c04c */
#endif
extern int nSelectedReleaseWeaponIndex; /* 0x0046c058 */
extern short DAT_0046c05c; /* 0x0046c05c */
extern short bTargetLockReadoutDirty; /* 0x0046c060 */
extern short nTargetLockCountdown; /* 0x0046c064 */
extern signed char cViableTargetCount; /* 0x0046c088 */
extern short nTargetLockMarkerAngle; /* 0x0046c068 */
extern signed char cTargetDisplayObject; /* 0x0046c06c */
extern int bTargetLockAcquired; /* 0x0046c074 */
extern short nTargetLockMode; /* 0x0046c078 */
extern int bMissileCameraEnabled; /* 0x0046c07c */
#if defined(WC1_SHIP_SOURCE) || defined(WC1_BRAINS_SOURCE)
/* These units need the wingman declaration after the targeting state. */
extern short nYourWingman; /* 0x0046c04c */
#endif
extern int bEngageAllowed; /* 0x0046c080 */
#ifdef WC1_AUTO_SOURCE
/* Autopilot's ship/wingman comparison uses this later declaration. */
extern short nYourWingman; /* 0x0046c04c */
#endif
extern short nAutoEngageTimer; /* 0x0046c084 */
extern MissionNavPoint aMissionNavPoints[MISSION_NAV_POINT_COUNT]; /* 0x0046c2f0 */
extern MissionShipRecord
    aMissionShips[MISSION_SHIP_STORAGE_COUNT]; /* 0x0046c948 */
extern short asIntroCameraSequence[]; /* 0x0046c090 */
extern short asCampaignVictoryViewScript[24]; /* 0x0046c160 */
extern short asTigerClawEscapeViewScript[12]; /* 0x0046c238 */
extern signed char acDirectionShapeFrame[ /* 0x0046db28 */
    DIRECTION_VIEW_COUNT * DIRECTION_SHAPE_TABLE_COUNT];
extern signed char acDirectionShapeFlip[ /* 0x0046dbe8 */
    DIRECTION_VIEW_COUNT * DIRECTION_SHAPE_TABLE_COUNT];
/* Event-list state is private to eventmgr.c. These owners omit its unused
 * declarations; the code-selection evidence is in docs/PATTERNS.md. */
#if !defined(WC1_FL_SOURCE) && !defined(WC1_MONO_SOURCE) && \
    !defined(WC1_VIEW_SOURCE)
extern InputEvent *pInputEventHead; /* 0x0046da90 */
extern InputEvent *pInputEventTail; /* 0x0046da94 */
extern int bInputEventPoolInitialized; /* 0x0046da98 */
#endif
extern short nScreenWidth; /* 0x0046daa4 */
extern short nScreenHeight; /* 0x0046daa8 */
extern ScreenViewportGeometry aScreenViewportGeometry[7]; /* 0x0046dab8 */
extern int aiIntelligenceEvent[10]; /* 0x0046d368 */
extern signed char *apDefenseManeuvers[5]; /* 0x0046d3d0 */
extern short nTalkingHeadFace; /* 0x0046e584 */
extern ManeuverChoice aRatedManeuverChoices[13][9][3]; /* 0x0046d3e8 */
extern ManeuverChoice aKilrathiManeuverChoices[5][9][3]; /* 0x0046d808 */
extern signed char acPilotAggression[24]; /* 0x0046d9a0 */
extern signed char acPilotRecovery[20]; /* 0x0046d9b8 */
extern unsigned char *pDrawnMouseCursorShape; /* 0x0046da9c */
extern int nMouseCursorShowCount; /* 0x0046daa0 */
extern short asPilotHandOrigins[10]; /* 0x0046e120 */
extern CampaignDate stSavedCampaignDate; /* 0x0046e188 */
extern TalkingHeadOrigin aTalkingHeadOrigins[11]; /* 0x0046e190 */
extern BriefingCharacterLayout aBriefingCharacters[10]; /* 0x0046e218 */
extern const char *apszMedalNames[5]; /* 0x0046e2e0 */
extern short nDebriefingPodiumX; /* 0x0046e57c */
extern short nConversationCharacter; /* 0x0046e580 */
extern short nConversationBackdropFrame; /* 0x0046e588 */
extern int bConversationConstellation; /* 0x0046e58c */
extern int bConversationOverlay; /* 0x0046e590 */
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
extern unsigned char abGifLZWCodeMasks[9]; /* 0x0046ff15 */
extern unsigned char abGifInterlaceRowSteps[5]; /* 0x0046ff1e */
extern unsigned char abGifInterlaceRowStarts[5]; /* 0x0046ff23 */
extern RasterClip *pGifRasterTarget; /* 0x0046ff28 */
extern unsigned char abRasterPaletteTranslation[256]; /* 0x0046ff2c */
extern RLETransformVertex aRLETransformVertices[4]; /* 0x0047002c */
extern int anRLESourceSteps[4]; /* 0x0047007c */
extern unsigned char *pMissionCampaignInfo; /* 0x005988bc */
extern const char *apszPilotRankNames[5]; /* 0x00470098 */
extern CampaignState stInitialCampaignState; /* 0x004700b0 */
extern int bPanRoomTransition; /* 0x00470510 */
extern int nUnusedFlightProfileTicksA; /* 0x00598888 */
extern int nUnusedFlightProfileTicksB; /* 0x0059888c */
extern int nUnusedFlightProfileTicksC; /* 0x00598890 */
extern Viewport stRoomScreenViewport; /* 0x005988a0 */
extern short nSavedRoomCtrlX; /* 0x005988b4 */
extern unsigned char *pRecRoomRoster; /* 0x005988b8 */
extern unsigned char *apRecRoomFigureShapes[3]; /* 0x005988c0 */
extern PilotRecord aPilotEntriesList[9]; /* 0x005988d0 */
extern ConstellationObjectDefinition
    *pConstellationDefinitionsPtr; /* 0x00598a28 */
extern PilotRecord *apWingmanPilotsList[8]; /* 0x00598a30 */
extern unsigned char *pRecRoomBgShape; /* 0x00598a50 */
extern Viewport stRoomDisplayView; /* 0x00598a60 */
extern Viewport stCurrentRoomMouseView; /* 0x00598a80 */
extern unsigned char *apRecRoomTxtDataArray[3]; /* 0x00598aa0 */
extern char szTxtWorkspaceBuffer[256]; /* 0x00598b00 */
extern RoomMenu stCurrentRoomMenu; /* 0x00598ab0 */
extern unsigned char *apRecRoomSceneDataList[3]; /* 0x00598ae0 */
extern unsigned char *pBriefingPacket; /* 0x00598aec */
extern unsigned char *pCurrentBriefingTextData; /* 0x00598af0 */
#ifndef WC1_GAMEFLOW_SOURCE
extern short *pSceneMouthAnimCmds; /* 0x00598af4 */
extern unsigned char *pRoomMedalTextData; /* 0x00598af8 */
#endif
extern unsigned char *pDebriefingSceneDataPtr; /* 0x00598afc */
extern unsigned char *pBriefingSceneInfo; /* 0x00598c00 */
extern unsigned char *pCurrentConversationBackdropShapePtr; /* 0x00598c04 */
extern short nCurrentConversationMedalIndex; /* 0x00598c08 */
extern short nTalkSceneType; /* 0x00598c0a */
extern unsigned char *pTalkHeadShape; /* 0x00598c0c */
extern short nConvTextColorValue; /* 0x00598c10 */
extern unsigned char *pCurrentBriefingAnimShape; /* 0x00598c14 */
#ifndef WC1_GAMEFLOW_SOURCE
extern short *pFaceAnimationCmds; /* 0x00598c18 */
extern unsigned char *pSceneBriefingBodyShape; /* 0x00598c1c */
extern unsigned char *pMedalSceneData; /* 0x00598c20 */
extern unsigned char *pSceneBriefingPortraitShape; /* 0x00598c24 */
#endif
extern unsigned char *pDebriefingTxtInfo; /* 0x00598c28 */
#ifndef WC1_GAMEFLOW_SOURCE
extern unsigned char *pBriefingCloseupShape; /* 0x00598c2c */
#endif
extern unsigned char *pConvOverlayShapePtr; /* 0x00598c30 */
extern unsigned char bFilteredKeyDelayStartedState; /* 0x00598c34 */
extern InputEvent aInputEvtPool[0x100]; /* 0x00598c40 */
extern int nMouseCursorSavedCoordY; /* 0x0059a840 */
extern int nMouseCursorSavedX; /* 0x0059a844 */
extern unsigned char bInputModeState; /* 0x0059a848 */
extern unsigned char bFilteredKeyWaitActive; /* 0x0059a849 */
extern int bEventMouseCursorDrawnFlag; /* 0x0059a84c */
extern int nMousePointerDrawnY; /* 0x0059a8e0 */
extern int nMousePointerDrawnX; /* 0x0059a8e4 */
extern int anEventObjectDepthPlacedList[SPACE_OBJECT_COUNT]; /* 0x0059a8f0 */
extern short DAT_0059a9f0;
extern signed char cScreenViewMode; /* 0x0059a9f2 */
extern const ScreenViewportGeometry *pScreenViewGeometry; /* 0x0059a9f4 */
extern short nEventManagerActive; /* 0x0059a850 */
extern short nCurrentViewCenterPosX; /* 0x0059a852 */
extern short nCurrentViewCenterPosY; /* 0x0059a854 */
extern short nPktDecompressSourceFile; /* 0x0059a858 */
extern unsigned char abInputKeyStateArray[0x80]; /* 0x0059a860 */
extern short nPacketDecompressInputPositionValue; /* 0x0059ab00 */
extern void *pPktDecompInput; /* 0x0059ab04 */
extern unsigned short wCurrentInputModifiers; /* 0x0059ab08 */
extern MouseCursorState stMouseCursorState; /* 0x0059ab10 */
extern void (*pCurrentEventManagerPump)(void); /* 0x0059ab2c */
extern int nPacketDecompressResult; /* 0x0059ab30 */
extern int nEventMousePointerDamageBottom; /* 0x0059ab48 */
extern int nTickCount60Hz; /* 0x0059ab54 */
extern volatile int bFrameTickPendingFlag; /* 0x0059ab3c */
extern int bMouseCursorDamagePending; /* 0x0059ab40 */
extern TextContext *pCurTextContext; /* 0x0059af8c */
extern int nMouseCursorDamageRight; /* 0x0059ab44 */
extern short nViewportOriginPositionY; /* 0x0059ab50 */
extern short nViewportOriginPositionX; /* 0x0059ab52 */
extern int nMouseCursorDamageLeft; /* 0x0059ab5c */
extern int nMousePointerDamageTopValue; /* 0x0059ab60 */
extern short nUnusedInputTimerFlag; /* 0x0059ab64 */
extern int aiInputEventSlotUsedTable[0x100]; /* 0x0059ab70 */
extern short bCurrentRunningFromHardDiskFlag; /* 0x0059ab34 */
extern short nPacketDecompressPending; /* 0x0059ab36 */
extern unsigned short wPacketDecompressInputSize; /* 0x0059ab38 */
extern short nCurrentPacketDecompressScratchSegment; /* 0x0059ab3a */
extern int nSavedDosInterruptVector; /* 0x0059ab4c */
extern unsigned char bEscapePressedFlag; /* 0x0059ab58 */
extern int anSortedObjects[SPACE_OBJECT_COUNT]; /* 0x0059aa00 */
extern FixedVector aObjectViewPositionsList[SPACE_OBJECT_COUNT]; /* 0x0059afa0 */
extern short anObjPitchRot[SPACE_OBJECT_COUNT]; /* 0x0059b2a0 */
extern int anShipSpeed[64]; /* 0x0059b320 */
extern short nCurrentSpaceFrameValue; /* 0x0059b420 */
extern FixedVector vPlayerAccel; /* 0x0059b460 */
extern short asObjDistancesTable[SPACE_OBJECT_COUNT]; /* 0x0059b4a0 */
extern FixedVector aShipRightVector[64]; /* 0x0059b6e0 */
extern FixedVector aShipUpVector[64]; /* 0x0059b9e0 */
extern FixedVector aShipFwdVec[64]; /* 0x0059bce0 */
extern signed char acPlayerComponentDamageList[9]; /* 0x0059bff0 */
extern unsigned int anShipAlertFlags[12]; /* 0x0059b430 */
extern int anShipFuelsTable[12]; /* 0x0059b470 */
extern enum ObjectType aeObjectTypeList[SPACE_OBJECT_COUNT]; /* 0x0059b560 */
extern signed char abFlightPathList[MISSION_OBJECTIVE_COUNT]; /* 0x0059c000 */
extern FixedVector aShipVelocity[SPACE_OBJECT_COUNT]; /* 0x0059c010 */
extern HazardField *pActiveHazardField; /* 0x0059bfe0 */
extern short anYawTarget[16]; /* 0x0059c310 */
extern short asObjectCounterArray[SPACE_OBJECT_COUNT]; /* 0x0059c330 */
#ifdef WC1_LOGIC_SOURCE
/* Logic's range-comparison operands depend on this earlier declaration. */
extern short nTgtRange; /* 0x0059ce10 */
#endif
extern short nTargetShip; /* 0x0059c3b0 */
extern enum SpecialManeuver aeSpecialManeuvers[12]; /* 0x0059c3c0 */
extern enum ShipMissionType aeShipMissionType[12]; /* 0x0059c3f0 */
extern short asShipTotal[16]; /* 0x0059c420 */
extern short asShipMaximumSpeedArray[16]; /* 0x0059c440 */
extern signed char acShipDmg[10]; /* 0x0059c460 */
extern short asViableTgtDistList[16]; /* 0x0059c470 */
extern signed char cMissionObjectiveCountValue; /* 0x0059c46a */
extern FixedVector aShipPositionsArray[SPACE_OBJECT_COUNT]; /* 0x0059c490 */
extern short asShipAfterburnerTimer[16]; /* 0x0059c810 */
extern short nShipMissionIndices[10]; /* 0x0059c830 */
extern signed char acShipCommunicatorArray[16]; /* 0x0059c850 */
extern signed char cCurrentNavPtIdx; /* 0x0059c86c */
extern FixedVector vStarFieldMotion; /* 0x0059c860 */
#ifndef WC1_LOGIC_SOURCE
extern short nTgtRange; /* 0x0059ce10 */
#endif
extern short asObjectFlipArray[SPACE_OBJECT_COUNT]; /* 0x0059c870 */
extern signed char acPreviousAttackers[16]; /* 0x0059c910 */
extern signed char acViableTargetList[16]; /* 0x0059c920 */
extern short asActionCounts[10]; /* 0x0059c930 */
extern FixedVector vPreviousStarFieldMotion; /* 0x0059c900 */
extern CampaignState stCampaignState; /* 0x0059ca50 */
extern short nCurrentHazardFieldTotal; /* 0x0059c90c */
extern unsigned short awObjMassArray[SPACE_OBJECT_COUNT]; /* 0x0059c790 */
extern short asObjAfterburnerVelocities[SPACE_OBJECT_COUNT]; /* 0x0059c9d0 */
extern short asObjScreenScalesArray[SPACE_OBJECT_COUNT]; /* 0x0059c950 */
extern unsigned char aShipWeaponsTable[10][0x47]; /* 0x0059cab0 */
extern signed char acShipRating[16]; /* 0x0059cd80 */
extern short asObjScreenAngles[SPACE_OBJECT_COUNT]; /* 0x0059cd90 */
extern unsigned int anVduModeCache[2]; /* 0x0059ce18 */
extern signed char acObjOwnersTable[64]; /* 0x0059ce20 */
extern signed char acShipTargetTable[16]; /* 0x0059ce60 */
extern short nPrevPitchInputValue; /* 0x0059ce70 */
extern short nPrevYawInput; /* 0x0059ce72 */
extern short nPreviousRollInputValue; /* 0x0059ce74 */
extern signed char acTurnRegulatorsArray[16]; /* 0x0059cf10 */
extern unsigned char abCollisionAlertTargets[0x10]; /* 0x0059cf20 */
extern short anObjectYawRotation[SPACE_OBJECT_COUNT]; /* 0x0059ce80 */
extern int aiPilotLevel[12]; /* 0x0059cf30 */
extern short asTargetListRangesArray[16]; /* 0x0059cf60 */
extern signed char acPilotHitPointsArray[16]; /* 0x0059cf00 */
extern short asObjectDrawPosY[SPACE_OBJECT_COUNT]; /* 0x0059cf80 */
extern short asObjectDrawPosX[SPACE_OBJECT_COUNT]; /* 0x0059d000 */
/* Original class[] array.  The recovered ObjectClass order explains all of the
 * observed magic values: 0 null, 1 futurion, 10 mine, 11 missile, 12 ship and
 * 13 capital_ship. */
extern enum ObjectClass aeObjectClass[SPACE_OBJECT_COUNT]; /* 0x0059d100 */
extern short asPreviousObjectDistance[SPACE_OBJECT_COUNT]; /* 0x0059d080 */
extern enum ShipObjective aeShipObjectiveList[12]; /* 0x0059d200 */
extern short asObjectViewFrames[SPACE_OBJECT_COUNT]; /* 0x0059d230 */
extern signed char acWingmanMessageStatesArray[16]; /* 0x0059d2c0 */
extern short asCollisionDelayList[16]; /* 0x0059d2d0 */
extern int nCurObjectiveRangeValue; /* 0x0059d2b0 */
extern unsigned char *apObjShapeArray[SPACE_OBJECT_COUNT]; /* 0x0059d2f0 */
extern short nPitchInput; /* 0x0059d3f0 */
extern short nYawInputValue; /* 0x0059d3f2 */
extern short nCurrentRollInputValue; /* 0x0059d3f4 */
/* Logic's armor-term ordering depends on this declaration pair being reversed.
 * See docs/PATTERNS.md for the measured compiler context. */
#ifdef WC1_LOGIC_SOURCE
extern enum ShipTactic aeShipTacticArray[12]; /* 0x0059d5e0 */
#else
extern short aasShipArmor[10][4]; /* 0x0059d420 */
#endif
extern short asShipWingLeaderArray[16]; /* 0x0059d400 */
extern short asShipWpnEnergyList[10]; /* 0x0059d470 */
extern signed char acFormationMemberLists[16]; /* 0x0059d490 */
extern short anShipMissionShips[16]; /* 0x0059d4b0 */
extern FixedVector vToTgt; /* 0x0059d4d0 */
extern short asCannedCommands[16]; /* 0x0059d4e0 */
extern unsigned short ausVduModeStack[16]; /* 0x0059d500 */
extern short asObjectCollisionRadius[64]; /* 0x0059d710 */
extern char acShipSequence[10]; /* 0x0059d520 */
extern short nTargetFacingValue; /* 0x0059d52a */
extern FixedVector aShipDestinationsArray[10]; /* 0x0059d530 */
#ifdef WC1_LOGIC_SOURCE
extern short aasShipArmor[10][4]; /* 0x0059d420 */
#else
extern enum ShipTactic aeShipTacticArray[12]; /* 0x0059d5e0 */
#endif
extern short aasShipShield[10][2]; /* 0x0059d5b0 */
extern signed char abShipExhaustHeatList[10]; /* 0x0059d610 */
extern short nRenderedSpaceFrame; /* 0x0059d61a */
extern signed char acShipStressList[16]; /* 0x0059d620 */
extern enum Side aeShipSide[12]; /* 0x0059d650 */
extern signed char acShipAiCooldowns[16]; /* 0x0059d680 */
extern FixedVector vCollisionDelta; /* 0x0059d690 */
extern short anRollGoal[16]; /* 0x0059d630 */
extern signed char acLastCollisionObjects[SPACE_OBJECT_COUNT]; /* 0x0059d6a0 */
extern short anObjectRollRotationList[SPACE_OBJECT_COUNT]; /* 0x0059d7e0 */
extern signed char acShipPointingModeList[16]; /* 0x0059d790 */
extern signed char abShipNavPtIdx[16]; /* 0x0059d7c0 */
extern signed char acTurnIntervalArray[16]; /* 0x0059d7d0 */
extern short aasShipMaximumShieldTable[10][2]; /* 0x0059d6e0 */
extern signed char abShipTurnList[16]; /* 0x0059d860 */
extern short anPitchGoal[16]; /* 0x0059d7a0 */
extern short nFacingToTargetValue; /* 0x0059d920 */
extern short asObjectScreenYArray[SPACE_OBJECT_COUNT]; /* 0x0059d930 */
extern short asObjectScreenXList[SPACE_OBJECT_COUNT]; /* 0x0059d9b0 */
extern InputDeviceSample stPreviousFlightInput; /* 0x005a7af0 */
extern HazardField aHazardFields[8]; /* 0x0059d870 */
extern MissionObjective aMissionObjectivesArray[MISSION_OBJECTIVE_COUNT]; /* 0x0059dac0 */
extern enum ShipManeuver aeShipManeuversTable[12]; /* 0x0059dcb0 */
extern const short *apCannedSequenceList[12]; /* 0x0059dce0 */
extern FixedVector aShipMissionSpotArray[10]; /* 0x0059dd10 */
extern short asCapitalShipViewFramesTable[16]; /* 0x0059dd90 */
extern signed char acObjCollisionGraceTicks[SPACE_OBJECT_COUNT]; /* 0x0059ddb0 */
extern ObjectResourceSlot aObjectResourceSlotsArray[4]; /* 0x0059ddf0 */
extern signed char acShipDestroyedWeaponCountArray[16]; /* 0x0059de30 */
extern unsigned short awObjectScales[SPACE_OBJECT_COUNT]; /* 0x0059de40 */
extern short asShipAccumulatedDmg[SPACE_OBJECT_COUNT]; /* 0x0059dee0 */
extern short nCurrentNavigationPoint; /* 0x0059df60 */
extern signed char acShipIonDriveDmg[16]; /* 0x0059d4a0 */
extern short asObjectAnimationDelay[SPACE_OBJECT_COUNT]; /* 0x0059b660 */
extern short asObjectAnimIndexes[SPACE_OBJECT_COUNT]; /* 0x0059da30 */
extern short nEyeRollTargetValue; /* 0x0059c8f0 */
#ifndef WC1_VIEW_SOURCE
/* The flight unit needs the pitch goal declared before the yaw goal; the
 * scripted-view unit sees them in the other order. */
extern short nEyePitchTargetValue; /* 0x0059d61c */
#endif
extern short nEyeYawGoalValue; /* 0x0059c944 */
#ifdef WC1_VIEW_SOURCE
extern short nEyePitchTargetValue; /* 0x0059d61c */
#endif
extern signed char acVduModeStackDepth[16]; /* 0x0059dec0 */
extern signed char acShipSpawnNavPt[16]; /* 0x0059ded0 */
/* Calibration bounds are used only by joycal.c. */
#if !defined(WC1_FL_SOURCE) && !defined(WC1_VIEW_SOURCE)
extern int nJoystickCalibrationMinimumY; /* 0x0059df64 */
extern int nJoystickCalibrationMinimumX; /* 0x0059df68 */
extern int nJoystickCalibrationMaximumX; /* 0x0059df6c */
extern int nJoystickCalibrationMaxY; /* 0x0059df70 */
#endif
extern unsigned short awPalEntryAllocation[256]; /* 0x0059df80 */
extern unsigned char *apCommunicationPortraitShapesTable[16]; /* 0x0059e180 */
extern char szHudMessageBuf[0x60]; /* 0x0059e1c0 */
extern char *aapszPilotSpeeches[14][11]; /* 0x0059e220 */
extern signed char abCommMenuChoiceCommands[7]; /* 0x0059e488 */
extern const char *pszCommMenuHeading; /* 0x0059e490 */
extern char szCommMenuHeadingBuf[0x40]; /* 0x0059e4a0 */
extern const char *apszCommMenuChoiceTextTable[7]; /* 0x0059e4e0 */
/* Packet-handle tables and screen row storage have only screen.c users. */
#if !defined(WC1_FADE_SOURCE) && !defined(WC1_MAIN_SOURCE) && \
    !defined(WC1_LOGIC_SOURCE)
extern void *apPacketHandlesList[0x1000]; /* 0x0059e530 */
extern int aiPktHandleDeltas[0x1000]; /* 0x005a2530 */
extern int nPacketHandleTotal; /* 0x005a6530 */
#endif
extern Viewport *pAllocatedScreenViewport; /* 0x005a6534 */
#if !defined(WC1_FADE_SOURCE) && !defined(WC1_MAIN_SOURCE) && \
    !defined(WC1_LOGIC_SOURCE)
extern unsigned short awScreenRowOffsets[202]; /* 0x005a6540 */
extern ScreenAllocation stCurrentScreenAlloc; /* 0x005a66e0 */
extern ScreenAllocation *pScreenAllocPtr; /* 0x005a66ec */
#endif
extern int aiSoundEffectSourceActiveArray[0x40]; /* 0x005a66f0 */
extern unsigned char abAudioPlaybackSlots[0xf8]; /* 0x005a67f0 */
extern int nPassingShipAudioCooldown; /* 0x005a68e8 */
extern int *pFullScreenRecord; /* 0x005a6538 */
extern int nAllocateViewportCallsValue; /* 0x005a68ec */
extern void *pLastPacketAllocation; /* 0x005a68f0 */
extern FireworkState aFireworksList[30]; /* 0x005a6900 */
extern unsigned char *pFireworkShape; /* 0x005a6a68 */
extern ConstellationStar aConstellationStarsTable[10]; /* 0x005a6a70 */
extern Viewport *pConstellationViewportPtr; /* 0x005a6aac */
extern short nViewConstellationStarCount; /* 0x005a6ab0 */
extern ConstellationParticle aConstellationParticlesList[16]; /* 0x005a6ac0 */
extern Viewport stConstellationViewport; /* 0x005a6b40 */
extern short nConstellationParticleTotal; /* 0x005a6b54 */
extern short nCurrentExternalViewDistance; /* 0x005a6b5c */
extern Viewport stPilotHandView; /* 0x005a6b60 */
extern Viewport stLeftVdu; /* 0x005a6b80 */
extern ScreenViewportPacket *pCurrentScreenViewportPkt; /* 0x005a6b94 */
/* Graphics' first viewport comparison depends on this declaration pair. */
#ifdef WC1_GR_SOURCE
extern FixedVector aDirectionViewUpVectorArray[ /* 0x005a6f10 */
    DIRECTION_VIEW_COUNT];
#else
extern Viewport stScreenView; /* 0x005a6ba0 */
#endif
extern Viewport stCockpitBar; /* 0x005a6be0 */
extern TextContext stHudMessageTextContext; /* 0x005a6bc0 */
extern unsigned char *pTgtLockShapePtr; /* 0x005a6bf4 */
extern unsigned char *apTextFonts[4]; /* 0x005a6c00 */
extern FontWorkspace **apFontWorkspaces[4]; /* 0x005a6c10 */
extern FixedVector aDirectionViewRightVector[ /* 0x005a6c20 */
    DIRECTION_VIEW_COUNT];
#ifdef WC1_GR_SOURCE
extern Viewport stScreenView; /* 0x005a6ba0 */
#else
extern FixedVector aDirectionViewUpVectorArray[ /* 0x005a6f10 */
    DIRECTION_VIEW_COUNT];
#endif
extern FixedVector aDirectionViewForwardVector[ /* 0x005a7200 */
    DIRECTION_VIEW_COUNT];
extern TextContext stLeftVduTextContext; /* 0x005a74f0 */
extern Viewport stSpaceBufferViewport; /* 0x005a7510 */
extern Viewport stRightVduViewport; /* 0x005a7530 */
extern Viewport stPilotHandBackdropView; /* 0x005a7550 */
extern unsigned char *pCockpitWeaponShapePtr; /* 0x005a7564 */
extern Viewport stCurrentConvTextViewport; /* 0x005a7570 */
#ifdef WC1_GAMEFLOW_SOURCE
extern Viewport stSceneBuffer; /* 0x005a76b0 */
#endif
extern char szDefaultTxtBuf[0xc8]; /* 0x005a7590 */
extern Viewport stModalSrcViewport; /* 0x005a7670 */
extern unsigned char *pCockpitIndicatorShape; /* 0x005a7658 */
extern unsigned char *pCommMenuPointerShape; /* 0x005a7660 */
extern unsigned char *pCurrentPilotHandShapePtr; /* 0x005a7684 */
extern Viewport stPilotHandComposite; /* 0x005a7690 */
#ifndef WC1_GAMEFLOW_SOURCE
extern Viewport stSceneBuffer; /* 0x005a76b0 */
#endif
extern unsigned char *pCockpitDamageShape; /* 0x005a76f4 */
extern short aPaletteFadeEntries[6][3]; /* 0x005a76d0 */
extern TextContext stRightVduTxtContext; /* 0x005a7700 */
extern TextContext stCurrentCockpitReadoutTextCtx; /* 0x005a7720 */
extern TextContext stDefaultTxtContext; /* 0x005a7740 */
extern TextContext stCurrentConvTxtCtx; /* 0x005a7760 */
extern short asDamageFlashColour[3]; /* 0x005a7780 */
extern short nCurrentDmgDispTicks; /* 0x005a7786 */
extern ShortPoint stWpnDispOrigin; /* 0x005a7788 */
typedef char WpnDispOrigin_size_must_be_4[
    sizeof(stWpnDispOrigin) == 4 ? 1 : -1];
extern char szDmgStateText[0x48]; /* 0x005a7790 */
extern ShortPoint stDamageSpritePos; /* 0x005a77d8 */
extern signed char cDamageDisplayComponentValue; /* 0x005a77dc */
extern signed char cDamageDisplayFrame; /* 0x005a77dd */
extern signed char cDamagedComponentCount; /* 0x005a77de */
extern int nDmgDispState; /* 0x005a77e0 */
extern int nDamageDisplayPhase; /* 0x005a77e4 */
extern unsigned char *pLaunchDoorShape; /* 0x005a77e8 */
extern unsigned int nCurrentFrameTimerIdValue; /* 0x005a77ec */
extern unsigned char abPaletteTripletsArray[256][3]; /* 0x005a77f0 */
extern Viewport stCurrentTrainSimTitleDisplayViewport; /* 0x005a7b90 */
extern signed char cCurrentCockpitView; /* 0x0059dab0 */
extern short bMouseButtonEventQueuedFlag; /* 0x005a7afc */
extern short bCurrentKeyboardEvtQueued; /* 0x005a7afe */
extern short bMouseMoveEvtQueued; /* 0x005a7b00 */
extern char *pCurrentStartupArgumentsPtr[30]; /* 0x005a7b10 */
extern short bJoystickEventQueued; /* 0x005a7b88 */
extern short nMainArcadeStartupParameterValue; /* 0x005a7b8a */
extern Viewport stTrainSimulatorHighScoreBufView; /* 0x005a7bb0 */
extern int nCurrentArcadeScore; /* 0x005a7bc4 */
extern TextContext stTrainSimTxtContext; /* 0x005a7bd0 */
extern Viewport stTrainSimHighScoreDisplayViewport; /* 0x005a7bf0 */
extern TextContext stTrainSimHighScoreTextContext; /* 0x005a7c10 */
extern short nArcadeTimeRemaining; /* 0x005a7c2c */
extern HighScoreEntry aHighScoreEntriesArray[6]; /* 0x005a7c30 */
extern int nArcadeWaveBonusValue; /* 0x005a7c50 */
extern unsigned char *pSceneAnimationSceneDataPtr; /* 0x005a7c54 */
extern unsigned char *pCurrentSceneAnimationPrimaryShape; /* 0x005a7c58 */
extern unsigned char *pSceneAnimationTextData; /* 0x005a7c5c */
extern unsigned char *pSceneAnimationPacketPtr; /* 0x005a7c60 */
extern SceneAnimationObject *pSceneAnimationObjects; /* 0x005a7c64 */
extern short nSceneAnimationDelayFrames; /* 0x005a7c68 */
extern unsigned char *pSceneAnimationDefinitionsPtr; /* 0x005a7c6c */
extern unsigned char *pSceneAnimationSecondaryShape; /* 0x005a7c70 */
extern signed char cCockpitLogicalFile; /* 0x005a7c74 */
extern unsigned char *apCockpitShapesList[4]; /* 0x005a7c80 */
extern unsigned char *pCinematicViewBackdropPtr; /* 0x005a7c90 */
extern unsigned char *pLogicRearViewBackdropPtr; /* 0x005a7c94 */
extern int bCurrentPlayerDestroyed; /* 0x005a7c98 */
extern short nPlayerKillTotal; /* 0x005a7c9c */
extern short asCollisionPartnerTable[10]; /* 0x005a7cc0 */
extern volatile unsigned int dwOriginalFreeMem; /* 0x005a7cd8 */
extern short asCollisionTimes[10]; /* 0x005a7ca0 */
extern int nCurrentWingmanKilledThisMission; /* 0x005a7cb4 */
extern short nWingmanKillCount; /* 0x005a7cb8 */
extern HudMessageSlot aCockpitHudMessageSlotsTable[2]; /* 0x005a7dd0 */
extern short nLogicLastFoundShipValue; /* 0x005a7cba */
extern int nMemoryConfig; /* 0x005a7cd4 */
extern unsigned char *pMouseCursorResource; /* 0x005a7cdc */
extern int nAvailableGameMemory; /* 0x005a7ce0 */
extern int nSceneResourceBudgetValue; /* 0x005a7ce4 */
extern unsigned char *pRadarMarkerBackground; /* 0x005a7dc4 */
extern int bAfterburnerSfxEnabled; /* 0x005a7cec */
extern int bPtrMovedByKeyboardState; /* 0x005a7d54 */
extern FixedVector vNormalizedToTarget; /* 0x005a7db0 */
extern ShortPoint stReleaseWeaponDisplayPosition; /* 0x005a7dbc */
extern enum ObjectType eReleaseWpnDisplayType; /* 0x005a7dc0 */
extern signed char bStickIndicatorFrameState; /* 0x005a7dc8 */
extern short nCurrentNoticeTimer; /* 0x005a7dca */
extern short nPendingCockpitDamageValue; /* 0x005a7dcc */
extern signed char cLastTargetObject; /* 0x005a7df2 */
extern int nAfterburnerSoundDeadline; /* 0x005a7ce8 */
extern int nSavedMouseCursorY; /* 0x005a7df4 */
extern int nStoredMouseCursorPosX; /* 0x005a7df8 */
extern char szComponentHitMessage[40]; /* 0x005a7e00 */
extern short nTargetLockMarkerY; /* 0x005a7e28 */
extern short nCarrierMissionShipIndexValue; /* 0x005a7e2a */
extern CockpitReadout aCockpitReadouts[6]; /* 0x005a7e30 */
extern short nScannerCursorX; /* 0x005a7e6c */
extern int nViewAllocTotal; /* 0x005a7f0c */
extern short asScannerObjectY[11]; /* 0x005a7e80 */
extern volatile ShortPoint stCockpitExplosionPosition; /* 0x005a7e98 */
extern short asScannerObjectPositionX[11]; /* 0x005a7ea0 */
extern short asScannerBackgroundColour[10]; /* 0x005a7ed0 */
extern int nCurrentDamageAlarmSfxHandle; /* 0x005a7ec0 */
extern unsigned short nPendingHudMessageColour; /* 0x005a7f00 */
extern short nNavigationMapMinimumCoordX; /* 0x005a812e */
extern short nExternalViewAngle; /* 0x005a7f06 */
extern short nScannerCursorY; /* 0x005a7e6e */
extern unsigned char *apViewportAllocations[128]; /* 0x005a7f10 */
extern int nFreeViewCallsValue; /* 0x005a8110 */
extern short nQueuedCampaignIndex; /* 0x005a8114 */
extern short nMissionMedalScoreValue; /* 0x005a8116 */
extern int nNearHeapBase; /* 0x005a8120 */
extern short nCampaignDataSet; /* 0x005a8118 */
extern int nNearHeapSize; /* 0x005a811c */
extern int nCurrentNearHeapFirstDescriptor; /* 0x005a8124 */
extern void * volatile pCurrentNearHeapAlloc; /* 0x005a8128 */
extern short nNavMapMaximumPositionX; /* 0x005a812c */
extern unsigned short awMapNavObjectiveLabelIndex[16]; /* 0x005a8130 */
extern short nNavMapMaximumPosY; /* 0x005a8150 */
extern short nNavMapCentreX; /* 0x005a8152 */
extern short nNavigationMapMinimumPositionY; /* 0x005a8154 */
extern TextContext stCurrentNavMapTxtCtx; /* 0x005a8160 */
extern short nNavMapCenterPositionY; /* 0x005a817c */
extern TextContext stMapNavigationLabelTextContext; /* 0x005a8180 */
extern int DAT_005a7d9c;
extern DiskFileRecord *pDiskFileRecords; /* 0x005a7cf0 */
extern Viewport stDiskPromptBgView; /* 0x005a7d00 */
extern unsigned char abDiskPromptDriveStates[0x20]; /* 0x005a7d20 */
extern Viewport stDiskPromptViewport; /* 0x005a7d40 */
extern TextContext stDiskPromptTextContext; /* 0x005a7d60 */
extern ShortRect stDiskPromptBounds; /* 0x005a7d80 */
extern signed char cCapitalShipLogicalFile; /* 0x005a7da0 */
extern signed char cCurrentObjResourceLogicalFile; /* 0x005a86b0 */
extern CampaignDate *pCurrentCampaignDate; /* 0x005a86a8 */
extern CampaignDate *pElapsedCampaignDatePtr; /* 0x005a86ac */
extern ShortPoint aFuneralParticlesArray[7]; /* 0x005a86f0 */
extern short nSceneScrambleShipDetailCountValue; /* 0x005a86cc */
extern Viewport *pScrambleView; /* 0x005a86b4 */
extern short nSceneScrambleCanopyFrameValue; /* 0x005a86b8 */
extern unsigned char *pScrambleDetailShape; /* 0x005a86bc */
extern short nScrambleCockpitAngle; /* 0x005a86c0 */
extern ShortPoint stScrambleCockpitDetail; /* 0x005a86c4 */
extern unsigned char *pCurrentConversationSpecialShape; /* 0x005a86ec */
extern short nFuneralCasketPositionX; /* 0x005a86c8 */
extern short nFuneralCasketPosY; /* 0x005a86ca */
extern signed char acScrambleShipDetailSlots[24]; /* 0x005a86d0 */
extern signed char cCutsceneScrambleWalkTime; /* 0x005a86e8 */
extern unsigned char *pSceneScrambleBgShapePtr; /* 0x005a870c */
extern short nFuneralParticleDistance; /* 0x005a8710 */
extern short nScrambleBackdropPositionY; /* 0x005a8712 */
extern short nScrambleBackgroundRightCoordX; /* 0x005a8714 */
extern ShortPoint stCutsceneFuneralForeground; /* 0x005a8718 */
extern short nSceneFuneralMainScale; /* 0x005a871c */
extern short nFuneralRifleFrame; /* 0x005a871e */
extern short nCutsceneFuneralBaseX; /* 0x005a8720 */
extern short nCutsceneFuneralBasePosY; /* 0x005a8722 */
extern short nCurrentScrambleShipPositionX; /* 0x005a8724 */
extern signed char cScrambleWalkerPair; /* 0x005a8748 */
extern short nScrambleShipY; /* 0x005a8726 */
extern short nFuneralParticleScale; /* 0x005a8728 */
extern unsigned char *pCurrentScrambleHangarShape; /* 0x005a872c */
extern unsigned char *pScrambleCockpitShapePtr; /* 0x005a8730 */
extern short nSceneScrambleWalkerPositionY; /* 0x005a8734 */
extern short nScrambleCanopyOffset; /* 0x005a8736 */
extern short nFuneralMainDist; /* 0x005a8738 */
extern signed char bScrambleCanopyClosed; /* 0x005a873a */
extern short nCutsceneFuneralGuardFrame; /* 0x005a873c */
extern ShortPoint stScrambleOverlay; /* 0x005a8740 */
extern unsigned char *pScrambleOverlayShape; /* 0x005a8744 */
extern unsigned char *pScrambleCanopyShape; /* 0x005a874c */
extern unsigned char *pScrambleShipShape; /* 0x005a8750 */
extern short nTalkingHeadFaceX; /* 0x005a8754 */
extern short nTalkingHeadFaceCoordY; /* 0x005a8756 */
extern short nTalkingHeadMouthY; /* 0x005a8758 */
extern short nTalkingHeadMouthX; /* 0x005a875a */
extern char szSoundDebugMsg[512]; /* 0x005a8760 */
extern MissionHeaderDisk stCurrentMissionHeader; /* 0x005a8690 */
extern unsigned char abMissionAuxData[0x28]; /* 0x005a8210 */
extern unsigned char abSeriesAuxData[0x28]; /* 0x005a8240 */
extern MissionObjectiveSource aMissionObjectiveSources[16]; /* 0x005a8270 */
extern unsigned char *pTitleShape; /* 0x005a7f08 */
extern unsigned char *pCurrentConstellationShape; /* 0x005a765c */
extern const short *pCurrentViewScriptPtr; /* 0x005a6b58 */
extern unsigned char *pIntroFont; /* 0x005a8960 */
extern unsigned int nSystemKeyDown; /* 0x005a8964 */
extern JOYINFO aJoystickInfos[2]; /* 0x005a8970 */
extern GuardedAllocation *pGuardedAllocationTail; /* 0x005a89ac */
extern char szMemoryUsage[0x80]; /* 0x005a89b0 */
extern HWND hMainWindow; /* 0x005a89a0 */
extern HANDLE hSingleInstanceSemaphore; /* 0x005a89a4 */
extern HDC hMainWindowDC; /* 0x005a8a30 */
extern HCURSOR hPrevPointer; /* 0x005a8a34 */
extern unsigned int nSessionEndTime; /* 0x005a8a38 */
extern int bMainWindowAlive; /* 0x005a8a3c */
extern HINSTANCE hCurrentAppInstance; /* 0x005a8a40 */
extern unsigned int nSessionStartTime; /* 0x005a8a44 */
extern unsigned short awPaletteRgbWordsList[0x300]; /* 0x005a8a50 */
extern const char *apszKilrathiAceNames[4]; /* 0x0046af80 */
extern unsigned long dwGameClockBaseValue; /* 0x005a89a8 */

extern unsigned char abCockpitIndicatorStatesArray[7]; /* 0x005a7e70 */
extern unsigned char abCockpitLightGoalArray[7]; /* 0x005a7eb8 */
extern unsigned int anCockpitDamageState[4]; /* 0x005a7ef0 */
extern MouseCursorState stHostMouseState; /* 0x0059af70 */
extern int nFrameIntervalMs; /* 0x0046b1b8 */
extern unsigned int dwIxStreamerStatus; /* 0x00597cd0 */
extern short bCurrentExpandedMemoryAvail; /* 0x0059a856 */
extern int bCampaignStartupMode; /* 0x004688e0 */
extern short nScriptedViewObject; /* 0x0046a8d0 */
extern int bScriptedView; /* 0x0046a8d4 */
extern int nSfxVolumeSetting; /* 0x00469fbc */
extern int nMusicVolumeSetting; /* 0x00469fc0 */
extern int bViewportDirty; /* 0x00469fc4 */
extern int nInputTickScale; /* 0x0059af90 */
/* These button-history fields are private to input.c. Selected owners omit
 * their unused declarations; docs/PATTERNS.md records the matching evidence. */
#if !defined(WC1_DIB_SOURCE) && !defined(WC1_FADE_SOURCE) && \
    !defined(WC1_MAIN_SOURCE) && !defined(WC1_NEARHEAP_SOURCE) && \
    !defined(WC1_LOGIC_SOURCE)
extern short asInputButton1DoubleClicksList[2]; /* 0x0059e508 */
extern short asInputBtn1ChangedTable[2]; /* 0x0059e50c */
extern short asInputButton2ChangedTable[2]; /* 0x0059e510 */
extern short asPreviousInputButtons[2]; /* 0x0059e514 */
extern int anInputButton1PressTimes[2]; /* 0x0059e518 */
extern int anInputButton2PressTime[2]; /* 0x0059e500 */
#endif
extern short asInputButton2DoubleClick[2]; /* 0x0059e520 */

extern short nCurrentActiveInputDevice; /* 0x005a819c */
/* Dead zones and raw coordinates are used only by joycal.c. */
#ifndef WC1_MAIN_SOURCE
extern int nJoystickVertDeadZoneValue; /* 0x005a81a0 */
extern int nJoystickHorizontalDeadZone; /* 0x005a81a4 */
#endif
extern int nJoystickUpScale; /* 0x005a81a8 */
extern int nCurrentJoystickLeftScale; /* 0x005a81ac */
/* Interactive calibration's bounds use a distinct declaration order. */
#ifdef WC1_JOYSTICK_SOURCE
extern int nStickMinimumPositionX; /* 0x005a81b8 */
extern int nCurrentJoystickFailureValue; /* 0x005a81e0 */
extern int nStickMaxX; /* 0x005a81b0 */
#else
extern int nStickMaxX; /* 0x005a81b0 */
extern int nStickMaxY; /* 0x005a81b4 */
extern int nStickMinimumPositionX; /* 0x005a81b8 */
#endif
extern int nStickMinimumPositionY; /* 0x005a81bc */
#ifndef WC1_MAIN_SOURCE
extern int nCurrentJoystickRawCoordX; /* 0x005a81c0 */
extern int nCurrentJoystickRawCoordY; /* 0x005a81c4 */
#endif
extern int nJoystickVerticalRange; /* 0x005a81c8 */
extern int nJoystickHorizontalRangeValue; /* 0x005a81cc */
extern int nJoystickRightScale; /* 0x005a81d0 */
extern int nJoystickDownScaleValue; /* 0x005a81d4 */
extern int nJoystickCentreY; /* 0x005a81d8 */
extern int nJoystickCentreX; /* 0x005a81dc */
extern short nMenuInputRepeatDelay; /* 0x005a8208 */
#ifdef WC1_JOYSTICK_SOURCE
extern int nStickMaxY; /* 0x005a81b4 */
#else
extern int nCurrentJoystickFailureValue; /* 0x005a81e0 */
#endif
extern int nHostMouseMessageX; /* 0x005a8990 */
extern int nHostMouseMessageY; /* 0x005a8994 */
extern int bHostPrimaryMouseButton; /* 0x005a8998 */
extern int bHostSecondaryMouseButton; /* 0x005a899c */

extern ObjectTypeData aObjectTypeData[OBJECT_TYPE_COUNT]; /* 0x00466458 */
extern ShortVector aChildOffsets[56]; /* 0x004682f0 */
#ifdef SDL_PORT
extern const ShortVector aSwcChildOffsets[63];
#endif
extern ShortPoint aWeaponDisplayPositions[32]; /* 0x00468440 */
extern InputDeviceSample aInputDevSamples[2]; /* 0x005a81f0 */
extern ShortPoint aWeaponDisplayOrigins[5]; /* 0x004684c0 */

extern unsigned char bMessageSpeed; /* 0x0046af68 */

extern ShortVector aShipFormationDeltaList[10]; /* 0x0059b520 */

#endif /* GAME_GLOBALS_H */
