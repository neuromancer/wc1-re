/*
 *  Globals shared by the reconstruction.
 *
 *  Names keep the original address (AGENTS.md): a global may be renamed from
 *  DAT_<addr> to g_<hungarian><Name>_<addr>, but the address must survive so
 *  that every reference stays checkable against the original image.
 *
 *  These live in one place only because the game-core compilation-unit
 *  boundaries are still unknown; each global belongs in whichever module
 *  defines it, and moves there as docs/ORDER.md is filled in.
 */
#ifndef WC1_GLOBALS_H
#define WC1_GLOBALS_H

extern unsigned int g_nHazardReferenceSpeed_00465040;
extern short g_nActiveHazards_00465044;
extern int DAT_00465058;
extern unsigned int DAT_0046505c;
extern int DAT_0046506c;
extern unsigned int g_nGuardedAllocationTotalBytes_00465060;
extern unsigned int g_nGuardedAllocationBytes_00465064;
extern unsigned int g_nGuardedAllocationPeakBytes_00465068;
extern int DAT_00465070;
extern int DAT_00465074;
extern int DAT_0046507c;
extern int DAT_00465080;
extern int DAT_004650a8;
extern unsigned int DAT_004650ac;
extern GuardedAllocation *g_pGuardedAllocationHead_004650b0;
extern const char g_szJoystickDevCapsFailure_004652dc[26];
extern short DAT_00465460;
extern short g_nAutopilotFormationShipCount_00465544;
extern int g_aiPacketReferenceTable_00465c88[4 * 0x25];
extern unsigned short DAT_00468660;
extern unsigned int DAT_00468664;
extern unsigned char DAT_0046870c;
extern unsigned char DAT_00468710;
extern int DAT_00468754;
extern char *DAT_00469004;
extern char *DAT_00469008;
extern unsigned char DAT_0046900c;
extern const short g_asPilotHandOffsets_00469018[34];
extern unsigned char *g_pCockpitExplosionBackground_00469060;
extern unsigned char * volatile g_pCockpitExplosionShape_00469064;
extern volatile short g_nCockpitExplosionFrame_00469068;
extern int g_nReleaseWeaponDisplayEnabled_0046906c;
extern volatile signed char g_cReleaseWeaponDisplayFrame_00469070;
extern volatile signed char g_cReleaseWeaponDisplayTicks_00469074;
extern volatile signed char g_cReleaseWeaponDisplayState_00469078;
extern unsigned short DAT_00469090;
extern short g_nTargetLockMarkerX_004691f4;
extern ShortRect g_stTargetBracketBounds_004691f8;
extern ShortRect g_stPreviousTargetBracketBounds_00469200;
extern short DAT_00469208;
extern unsigned char *g_pConfedCommBackground_00469278;
extern unsigned char *g_pCommStaticShape_0046927c;
extern unsigned char *g_pKilrathiCommBackground_00469280;
extern const char g_szComponentHitFormat_004692e0[8];
extern unsigned int g_dwModalBoundsTopLeft_00469440;
extern unsigned int g_dwModalBoundsBottomRight_00469444;
extern ModalTextPanel *g_pModalTextPanel_00469448;
extern unsigned char DAT_004693b0;
extern const char *g_pGameVersion_004693b4;
extern const char g_szGameVersion_004693b8[9];
extern int g_nDebugOverlayConsoleCount_00469644;
extern DWORD g_dwDebugOverlayKey_00469648;
extern DWORD g_dwDebugOverlayKeyLatch_0046964c;
extern HHOOK g_hDebugKeyboardHook_00469650;
extern const char g_szDebugOverlayFontName_00469654[8];
extern const char g_szDebugOverlaySpinner_0046965c[5];
extern const char g_szDebugOverlayNewline_00469664[4];
extern unsigned char DAT_0046999c;
extern unsigned char g_cViewportClearColour_004699a0;
extern unsigned char DAT_004699a4;
extern unsigned char DAT_004699a8;
extern unsigned char DAT_004699ac;
extern unsigned char DAT_004699b0;
extern unsigned char DAT_004699b4;
extern unsigned char DAT_004699d8;
extern unsigned char DAT_00469d5c;
extern unsigned int g_dwHighScoreNameTableBiasPadding_00469dbc;
extern unsigned short DAT_00469dc0[4];
extern const char g_szTrainSimTitle_00469dc8[24];
extern unsigned char *g_apszBuiltInHighScoreNames_00469de0[6];
extern short DAT_00469df8[26];
extern unsigned char g_aszBuiltInHighScores_00469e38[48];
extern int g_nTrainSimActive_00469e2c;
extern short g_nTrainSimMission_00469e30;
extern short g_nArcadeWave_00469e34;
extern int g_nCannedSceneMode_00469fac;
extern int g_nArcadeState_00469fb0;
extern short DAT_00469fb4;
extern short g_nFrameSkip_00469fb8;
extern int g_nOriginDevUnlock_00469ff4;
extern int DAT_00469ffc;
extern int DAT_0046a000;
extern unsigned char DAT_0046a004;
extern int DAT_0046a008;
extern int g_nShowMemoryStatus_0046a00c;
extern short g_nArcadeBonusCountdown_0046a014;
extern int g_bMouseCursorVisible_0046a018;
extern const char *g_apszComponentNames_0046a778[6];
extern const char g_szIonDrive_0046a7c4[12];
extern const char g_szPowerPlant_0046a7d0[12];
extern const char g_szShieldGenerator_0046a7dc[16];
extern const char g_szComputerSystem_0046a7ec[16];
extern const char g_szIntercomUnit_0046a7fc[16];
extern const char g_szTargetTracking_0046a80c[16];
extern ActiveSoundEntry *g_pActiveSoundHead_0046a438;
extern ActiveSoundEntry *g_pActiveSoundTail_0046a43c;
extern unsigned char DAT_0046a440;
extern WaveTableEntry *g_pWaveTableHead_0046a444;
extern WaveTableEntry *g_pWaveTableTail_0046a448;
extern const char g_szPlayWaveOpenError_0046a46c[36];
extern short DAT_0046a9f8;
extern short DAT_0046aa04;
extern unsigned int DAT_0046aa14;
extern unsigned char DAT_0046aa30;
extern unsigned char DAT_0046aa34;
extern unsigned char DAT_0046aa38;
extern unsigned char DAT_0046aa3c;
extern unsigned char DAT_0046aa40;
extern const char g_szSfxWaveFormat_0046ad2c[12];
extern unsigned char g_abTaggedAllocationPrefix_0046ad88[8];
extern short g_nCommMenuChoiceCount_0046af60;
extern short g_nCommMenuReuseMode_0046af64;
extern unsigned char DAT_0046af6c;
extern unsigned char DAT_0046afc4;
extern short g_nCommSpeakerObject_0046afc8;
extern short g_nCommSpeakerRating_0046afcc;
extern short g_nCommPortraitIndex_0046afd0;
extern unsigned short DAT_0046b168;
extern const GUID g_guidDirectDraw2_00463118;
extern int *DAT_0046b1a4;
extern short g_nCurrentWave_0046c01c;
extern int g_bIntroSecondaryScene_0046c024;
extern signed char g_abHazardObjects_0046c028[0x14];
extern short DAT_0046c010;
extern short g_nEyePitchRate_0046c004;
extern short g_nEyeYawRate_0046c008;
extern short g_nEyeRollRate_0046c00c;
extern signed char g_cCurrentObjective_0046c020;
extern int DAT_0046c03c;
extern short g_nYourWingman_0046c04c;
extern enum ObjectType g_eSelectedGunType_0046c054;
extern int g_nSelectedReleaseWeaponIndex_0046c058;
extern unsigned char DAT_0046c060;
extern short DAT_0046c064;
extern short DAT_0046c068;
extern short g_nTargetLockMode_0046c078;
extern int g_bEngageAllowed_0046c080;
extern short g_nAutoEngageTimer_0046c084;
extern MissionNavPoint g_aMissionNavPoints_0046c2f0[WC1_MISSION_NAV_POINT_COUNT];
extern MissionShipRecord g_aMissionShips_0046c948[WC1_MISSION_SHIP_COUNT];
extern const short g_asIntroCameraSequence_0046c090[];
extern const short g_asCannedSequence32_0046c0b8[];
extern const short g_asCannedSequence33_0046c0d8[];
extern const short g_asCannedSequence34_0046c0f0[];
extern const short g_asCannedSequence35_0046c108[];
extern const short g_asCannedSequence37_0046c190[];
extern const short g_asCannedSequence38_0046c1b0[];
extern const short g_asCannedSequence39_0046c1d0[];
extern const short g_asCannedSequence40_0046c1e0[];
extern const short g_asCannedSequence41_0046c230[];
extern const short g_asCannedSequence42_0046c250[];
extern const short g_asCannedSequence43_0046c270[];
extern const short g_asCannedSequence44_0046c2a0[];
extern const short g_asCannedSequence45_0046c2e8[];
extern const signed char g_acDirectionShapeFrame_0046db28[62];
extern const signed char g_acDirectionShapeFlip_0046dbe8[62];
extern const char *g_apszIntroCredits_00468a38[11];
extern int g_nIntroCreditCount_00468a30;
extern InputEvent *g_pInputEventHead_0046da90;
extern InputEvent *g_pInputEventTail_0046da94;
extern int g_bInputEventPoolInitialized_0046da98;
extern int g_nScreenWidth_0046daa4;
extern int g_nScreenHeight_0046daa8;
extern int g_aiIntelligenceEvent_0046d368[10];
extern signed char g_aDefenseManeuversNovice_0046d390[8];
extern signed char g_aDefenseManeuversVeteran_0046d398[8];
extern signed char g_aDefenseManeuversElite_0046d3a0[16];
extern signed char g_aDefenseManeuversAce_0046d3b0[16];
extern signed char g_aDefenseManeuversBoss_0046d3c0[16];
extern signed char *g_apDefenseManeuvers_0046d3d0[5];
extern ManeuverChoice g_aRatedManeuverChoices_0046d3e8[13][9][3];
extern ManeuverChoice g_aKilrathiManeuverChoices_0046d808[5][9][3];
extern signed char g_acPilotAggression_0046d9a0[24];
extern signed char g_acPilotRecovery_0046d9b8[20];
extern unsigned char *g_pDrawnMouseCursorShape_0046da9c;
extern int DAT_0046daa0;
extern const short g_asPilotHandOrigins_0046e120[10];
extern unsigned char g_abRasterPaletteTranslation_0046ff2c[256];
extern unsigned char g_abPaletteTranslation_00470678[256];
extern const char g_szSnowViewport_00470da4[16];
extern int g_nInsertKeyState_00475b68;
extern int g_nClearedKeyboardState_00475b6c;
extern int g_nOemPeriodKeyState_00475bf0;
extern int g_nHomeKeyState_00475bf4;
extern int g_nPageDownKeyState_00475bf8;
extern int g_nOemCommaKeyState_00475bfc;
extern int g_nRightArrowKeyState_00475c00;
extern int g_nEndKeyState_00475c04;
extern int g_nClearKeyState_00475c08;
extern int g_nDeleteKeyState_00475c0c;
extern int g_nUpArrowKeyState_00475c10;
extern int g_nDownArrowKeyState_00475c14;
extern char g_szStreamsPath_00475c18[0x100];
extern int g_nPageUpKeyState_00475d18;
extern int g_nLeftArrowKeyState_00475d1c;
extern char g_szReadDataFileError_00475d20[0x40];
extern char g_szCreateDataFileError_00475d60[0x40];
extern char g_szWriteDataFileError_00475da0[0x40];
extern char g_szSeekDataFileError_00475de0[0x80];
extern int g_bMonoDebugInstalled_00475e70;
extern HANDLE g_hMonoDebugDevice_00475e74;
extern IxSample *g_pLoopingWaveSample_0047654c;
extern IxSound *g_pLoopingWaveSound_00476550;
extern char g_szSfxWavePath_00476558[0xc8];
extern unsigned short DAT_00475e78;
extern unsigned char g_bCurrentManeuverReroll_00475e7c;
extern unsigned char g_abMouseCursorBackground_00475ff0[0x400];
extern DebugOverlayConsole *g_pDebugOverlay_004763f0;
extern unsigned int DAT_00476640;
extern unsigned int DAT_00476644;
extern unsigned char *DAT_00476648;
extern unsigned int DAT_0047664c;
extern unsigned int DAT_00476650;
extern unsigned char DAT_00476658[0xfa00];
extern unsigned char DAT_00476620[32];
extern int DAT_00486058;
extern int DAT_00486060;
extern HWND DAT_00486074;
extern unsigned char DAT_00486110[1024];
extern float DAT_00486510;
extern int DAT_00486518;
extern unsigned char DAT_004865a8[0x1000];
extern RasterSurface g_stRasterSurface_004875a8;
extern RasterClip g_stRasterClip_00496fc0;
extern unsigned char g_abSolidColourTranslation_00497648[256];
extern unsigned char DAT_005988de[8192];
extern char g_szTextScratchBuffer_00598b00[256];
extern int DAT_00598a30[512];
extern unsigned char DAT_00598ab0;
extern unsigned int DAT_00598ab6;
extern unsigned char DAT_00598aba;
extern unsigned int DAT_00598af4;
extern unsigned int DAT_00598c18;
extern InputEvent g_aInputEventPool_00598c40[0x100];
extern int g_bStreamerAudioPlaying_00597748;
extern int g_nMouseCursorSavedY_0059a840;
extern int g_nMouseCursorSavedX_0059a844;
extern unsigned char g_bInputMode_0059a848;
extern int DAT_0059a84c;
extern int DAT_0059a8e0;
extern int DAT_0059a8e4;
extern short g_nEventManagerActive_0059a850;
extern short g_nViewCenterX_0059a852;
extern short g_nViewCenterY_0059a854;
extern unsigned char g_abInputKeyState_0059a860[0x80];
extern volatile short g_nMouseX_0059ab10;
extern volatile short g_nMouseY_0059ab12;
extern unsigned char g_bPrimaryMouseButton_0059ab14;
extern unsigned char g_bSecondaryMouseButton_0059ab15;
extern unsigned char * volatile DAT_0059ab19;
extern unsigned short DAT_0059ab1d;
extern Viewport * volatile DAT_0059ab23;
extern unsigned char g_bMouseCursorShapeChanged_0059ab2b;
extern void (*DAT_0059ab2c)(void);
extern unsigned int DAT_0059ab54;
extern int DAT_0059ab3c;
extern int DAT_0059ab40;
extern int DAT_0059ab44;
extern int DAT_0059ab48;
extern int DAT_0059ab5c;
extern int DAT_0059ab60;
extern int g_aiInputEventSlotUsed_0059ab70[0x100];
extern TextContext *g_pCurrentTextContext_0059af8c;
extern short DAT_0059ab34;
extern int DAT_0059ab4c;
extern unsigned char DAT_0059ab58;
extern int g_anSortedObject_0059aa00[WC1_SPACE_OBJECT_COUNT];
extern FixedVector g_aObjectViewPosition_0059afa0[WC1_SPACE_OBJECT_COUNT];
extern short g_anObjectPitchRotation_0059b2a0[WC1_SPACE_OBJECT_COUNT];
extern int g_anShipSpeed_0059b320[64];
extern short g_nSpaceFrame_0059b420;
extern short g_asObjectDistance_0059b4a0[WC1_SPACE_OBJECT_COUNT];
extern FixedVector g_aShipRightVector_0059b6e0[64];
extern FixedVector g_aShipUpVector_0059b9e0[64];
extern FixedVector g_aShipForwardVector_0059bce0[64];
extern HazardField *g_pActiveHazardField_0059bfe0;
extern signed char g_acPlayerComponentDamage_0059bff0[6];
#define g_cPlayerPowerDamage_0059bff1 g_acPlayerComponentDamage_0059bff0[1]
extern unsigned int DAT_0059b430[512];
extern int g_anShipFuel_0059b470[512];
extern enum ObjectType g_aeObjectType_0059b560[96];
extern signed char g_abFlightPath_0059c000[WC1_MISSION_OBJECTIVE_COUNT];
extern FixedVector g_aShipVelocity_0059c010[512];
extern short g_anYawGoal_0059c310[16];
extern short g_asObjectCounter_0059c330[512];
extern enum SpecialManeuver g_aeSpecialManeuver_0059c3c0[WC1_SPACE_OBJECT_COUNT];
extern enum ShipMissionType g_aeShipMissionType_0059c3f0[512];
extern short g_asShipCount_0059c420[512];
extern short g_asShipMaximumSpeed_0059c440[16];
extern signed char g_acShipDamage_0059c460[10];
extern short g_asViableTargetDistance_0059c470[16];
extern signed char g_cMissionObjectiveCount_0059c46a;
extern FixedVector g_aShipPosition_0059c490[512];
extern unsigned char DAT_0059c810[512];
extern short g_nShipMissionIndices_0059c830[10];
extern signed char g_cCurrentNavPointIndex_0059c86c;
extern FixedVector g_vStarFieldMotion_0059c860;
extern short g_asObjectFlip_0059c870[WC1_SPACE_OBJECT_COUNT];
extern signed char DAT_0059c910[WC1_SPACE_OBJECT_COUNT];
extern FixedVector g_vPreviousStarFieldMotion_0059c900;
extern short g_nHazardFieldCount_0059c90c;
extern short g_asObjectRadarRadius_0059c790[WC1_SPACE_OBJECT_COUNT];
extern short g_asObjectAfterburnerVelocity_0059c9d0[WC1_SPACE_OBJECT_COUNT];
extern short g_asObjectScreenScale_0059c950[WC1_SPACE_OBJECT_COUNT];
extern signed char g_cCurrentMission_0059ca69;
extern signed char g_cCurrentSeries_0059ca6a;
extern int g_aiPersonalityDeathMission_0059ca74[8];
extern unsigned char g_abAceFlags_0059ca94[12];
extern short g_nPromotionScore_0059caa0;
extern short g_nMissionScore_0059caa2;
extern short g_nSeriesScore_0059caa4;
extern unsigned char g_aShipWeapons_0059cab0[16][0x47];
extern signed char g_acShipRating_0059cd80[16];
extern short g_asObjectScreenAngle_0059cd90[WC1_SPACE_OBJECT_COUNT];
extern short g_nTargetRange_0059ce10;
extern unsigned int DAT_0059ce18[256];
extern signed char g_acObjectOwner_0059ce20[64];
extern signed char g_acShipTarget_0059ce60[512];
extern short g_anObjectYawRotation_0059ce80[256];
extern unsigned char DAT_0059cf20[512];
extern signed char DAT_0059cf00[WC1_SPACE_OBJECT_COUNT];
/* Original class[] array.  The recovered ObjectClass order explains all of the
 * observed magic values: 0 null, 1 futurion, 10 mine, 11 missile, 12 ship and
 * 13 capital_ship. */
extern enum ObjectClass g_aeObjectClass_0059d100[512];
extern short g_asPreviousObjectDistance_0059d080[WC1_SPACE_OBJECT_COUNT];
extern enum ShipObjective g_aeShipObjective_0059d200[512];
extern short g_asObjectViewFrame_0059d230[WC1_SPACE_OBJECT_COUNT];
extern signed char g_acWingmanMessageState_0059d2c0[16];
extern short g_asCollisionCountdown_0059d2d0[16];
extern unsigned char *g_apObjectShape_0059d2f0[WC1_SPACE_OBJECT_COUNT];
extern short g_nPitchInput_0059d3f0;
extern short g_nYawInput_0059d3f2;
extern short g_asShipWingLeader_0059d400[16];
extern short g_aasShipArmor_0059d420[10][4];
extern short g_asShipWeaponEnergy_0059d470[16];
extern short g_asCannedCommand_0059d4e0[WC1_SPACE_OBJECT_COUNT];
extern unsigned short DAT_0059d500[16];
extern char g_acShipSequence_0059d520[512];
extern short g_nTargetFacing_0059d52a;
extern FixedVector g_aShipDestination_0059d530[10];
extern enum ShipTactic g_aeShipTactic_0059d5e0[512];
extern short g_aasShipShield_0059d5b0[12][2];
extern signed char g_abShipExhaustHeat_0059d610[10];
extern short g_nRenderedSpaceFrame_0059d61a;
extern signed char g_acShipStress_0059d620[16];
extern short g_anRollGoal_0059d630[16];
extern enum Side g_aeShipSide_0059d650[512];
extern signed char DAT_0059d6a0[WC1_SPACE_OBJECT_COUNT];
extern short g_aasShipMaximumShield_0059d6e0[12][2];
extern short g_asObjectCollisionRadius_0059d710[64];
extern short g_anPitchGoal_0059d7a0[16];
extern signed char g_abShipNavPointIndex_0059d7c0[512];
extern short g_anObjectRollRotation_0059d7e0[WC1_SPACE_OBJECT_COUNT];
extern unsigned char g_abShipTurn_0059d860[WC1_SPACE_OBJECT_COUNT];
extern short g_nFacingToTarget_0059d920;
extern short g_asObjectScreenY_0059d930[WC1_SPACE_OBJECT_COUNT];
extern short g_asObjectScreenX_0059d9b0[WC1_SPACE_OBJECT_COUNT];
extern signed char g_cCockpitView_0059dab0;
extern HazardField g_aHazardFields_0059d870[7];
extern MissionObjective g_aMissionObjectives_0059dac5[WC1_MISSION_OBJECTIVE_COUNT];
extern enum ShipManeuver g_aeShipManeuver_0059dcb0[512];
extern const short *g_apCannedSequence_0059dce0[WC1_SPACE_OBJECT_COUNT];
extern unsigned char g_aShipMissionSpot_0059dd10[224];
extern ObjectResourceSlot g_aObjectResourceSlots_0059ddf0[4];
extern short g_asObjectScale_0059de40[WC1_SPACE_OBJECT_COUNT];
extern short DAT_0059dee0[WC1_SPACE_OBJECT_COUNT];
extern signed char g_acShipIonDriveDamage_0059d4a0[16];
extern short g_asObjectAnimationDelay_0059b660[WC1_SPACE_OBJECT_COUNT];
extern short g_asObjectAnimationIndex_0059da30[WC1_SPACE_OBJECT_COUNT];
extern short g_nEyeRollGoal_0059c8f0;
extern short g_nEyeYawGoal_0059c944;
extern short g_nEyePitchGoal_0059d61c;
extern char DAT_0059dec0[256];
extern short g_nCurrentNavPoint_0059df60;
extern unsigned char *g_apCommPortraitShapes_0059e180[16];
extern char DAT_0059e1c0[512];
extern signed char g_abCommMenuChoiceCommand_0059e488[7];
extern unsigned int DAT_0059e490;
extern char *g_apszCommMenuChoiceText_0059e4e0[7];
extern int g_aiPacketHandles_0059e530[0x1000];
extern int g_aiPacketHandleOffsets_005a2530[0x1000];
extern int g_nPacketHandleCount_005a6530;
extern int g_aiSoundEffectSourceActive_005a66ec[0x41];
extern int *DAT_005a6538;
extern int g_nAllocateViewportCalls_005a68ec;
extern short DAT_005a6900[256];
extern Viewport DAT_005a6b60;
extern Viewport DAT_005a6b80;
extern Viewport DAT_005a6ba0;
#define DAT_005a6baa DAT_005a6ba0.top
#define DAT_005a6bae DAT_005a6ba0.bottom
extern TextContext DAT_005a6bc0;
extern unsigned char *g_pTargetLockShape_005a6bf4;
extern unsigned char *g_apTextFonts_005a6c00[4];
extern FontWorkspace **g_apFontWorkspaces_005a6c10[4];
extern TextContext DAT_005a74f0;
extern Viewport DAT_005a7510;
extern Viewport DAT_005a7530;
extern Viewport DAT_005a7550;
extern char g_szDefaultTextBuffer_005a7590[0xcc];
extern Viewport g_stModalSourceViewport_005a7670;
extern unsigned char *DAT_005a7684;
extern Viewport DAT_005a7690;
extern Viewport DAT_005a76b0;
extern TextContext DAT_005a7700;
extern TextContext g_stDefaultTextContext_005a7740;
extern unsigned short DAT_005a7780;
extern short g_nWeaponDisplayOriginX_005a7788;
extern short g_nWeaponDisplayOriginY_005a778a;
extern unsigned int DAT_005a77ec;
extern int g_nArcadeScore_005a7bc4;
extern short g_nArcadeTimeRemaining_005a7c2c;
extern HighScoreEntry g_aHighScoreEntries_005a7c30[6];
extern int g_nArcadeWaveBonus_005a7c50;
extern short g_nPlayerKillCount_005a7c9c;
extern short g_asCollisionPartner_005a7cc0[10];
extern short g_asCollisionTime_005a7ca0[16];
extern int g_nMemoryConfiguration_005a7cd4;
extern unsigned int g_dwOriginalFreeMemory_005a7cd8;
extern int g_nAvailableGameMemory_005a7ce0;
extern unsigned char DAT_005a7cec;
extern short g_nReleaseWeaponDisplayX_005a7dbc;
extern short g_nReleaseWeaponDisplayY_005a7dbe;
extern enum ObjectType g_eReleaseWeaponDisplayType_005a7dc0;
extern unsigned char g_bStickIndicatorFrame_005a7dc8;
extern unsigned char DAT_005a7dca;
extern HudMessageSlot g_aHudMessageSlots_005a7dd0[2];
#define DAT_005a7dd0 g_aHudMessageSlots_005a7dd0
#define DAT_005a7de1 g_aHudMessageSlots_005a7dd0[1]
#define DAT_005a7ddc g_aHudMessageSlots_005a7dd0[0].flashCount
#define DAT_005a7ddd g_aHudMessageSlots_005a7dd0[0].text
#define DAT_005a7ded g_aHudMessageSlots_005a7dd0[1].flashCount
#define DAT_005a7dee g_aHudMessageSlots_005a7dd0[1].text
extern signed char g_cPreviousTargetObject_005a7df2;
extern int g_nSavedMouseCursorY_005a7df4;
extern int g_nSavedMouseCursorX_005a7df8;
extern char g_szComponentHitMessage_005a7e00[40];
extern short g_nTargetLockMarkerY_005a7e28;
extern unsigned char DAT_005a7e30[2048];
extern volatile short g_nCockpitExplosionX_005a7e98;
extern volatile short g_nCockpitExplosionY_005a7e9a;
extern unsigned short DAT_005a7ea0[64];
extern unsigned char DAT_005a7ec0;
extern unsigned short DAT_005a7f00;
extern int g_nViewportAllocationCount_005a7f0c;
extern unsigned char *g_apViewportAllocations_005a7f10[128];
extern int g_nFreeViewportCalls_005a8110;
extern short g_nMissionMedalScore_005a8116;
extern int DAT_005a7d9c;
extern unsigned char *DAT_005a7cf0;
extern unsigned char DAT_005a86b0;
extern char DAT_005a8760[512];
extern short DAT_005a8692;
extern short g_nPlayerMissionShipIndex_005a8694;
extern short g_nInitialMissionShipIndices_005a8696[8];
extern unsigned char *g_pTitleShape_005a7f08;
extern unsigned char *g_pConstellationShape_005a765c;
extern const short *g_pViewScript_005a6b58;
extern unsigned char *g_pIntroFont_005a8960;
extern unsigned int DAT_005a8964;
extern JOYINFO g_aJoystickInfo_005a8970[2];
extern GuardedAllocation *g_pGuardedAllocationTail_005a89ac;
extern char g_szMemoryUsage_005a89b0[0x80];
extern HWND DAT_005a89a0;
extern HANDLE DAT_005a89a4;
extern HDC DAT_005a8a30;
extern HCURSOR DAT_005a8a34;
extern unsigned int DAT_005a8a38;
extern int DAT_005a8a3c;
extern HINSTANCE DAT_005a8a40;
extern unsigned int DAT_005a8a44;
extern unsigned short DAT_005a8a50[0x300];
extern char *PTR_s_MISSILE_LOCKED_004691d4;
extern int DAT_004691d8[2];
extern short DAT_004691e0[10];
extern char *g_apszCommMenuText_0046af90[13];
extern const char g_aszCommMenuText_0046aff8[0xe8];
extern const char g_szMissileLocked_004692a8[16];
extern unsigned long g_dwGameClockBase_005a89a8;

extern unsigned short DAT_005a7e70;
extern unsigned short DAT_005a7eb8;
extern unsigned short DAT_005a7ebc;
extern unsigned short DAT_005a7ebe;
extern unsigned int DAT_005a7ef0;
extern unsigned int DAT_005a7efc;
extern short g_nHostMouseX_0059af70;
extern int DAT_0046b1b8;
extern int DAT_0046b1bc;
extern int DAT_0046b1c4;
extern int DAT_0046b1c8;
extern float g_fSpaceFlightFrameRate_0046b1cc;
extern float g_fCinematicFrameRate_0046b1d0;
extern unsigned int g_dwStreamerState_00597cd0;
extern short DAT_0059a856;
extern unsigned char DAT_004700c9;
extern unsigned char DAT_004700ca;
extern int DAT_004688e0;
extern short g_nScriptedViewObject_0046a8d0;
extern int g_bScriptedView_0046a8d4;
extern int DAT_00469fbc;
extern int DAT_00469fc0;
extern int g_bViewportDirty_00469fc4;
extern int DAT_00469fc8[11];
/* Declared as comma-separated lists in the original tranches. */
extern short g_nHostMouseY_0059af72;
extern unsigned short DAT_005a7e74;
extern unsigned short DAT_005a7e76;
extern unsigned int DAT_005a7ef4;
extern unsigned int DAT_005a7ef8;

extern short g_nActiveInputDevice_005a819c;
extern InputDeviceSample g_aInputDeviceSamples_005a81f0[4];
extern int g_nHostMouseMessageX_005a8990;
extern int g_nHostMouseMessageY_005a8994;
extern int g_bHostPrimaryMouseButton_005a8998;
extern int g_bHostSecondaryMouseButton_005a899c;

extern ObjectTypeData g_aObjectTypeData_0046645c[OBJECT_TYPE_COUNT];
extern const ShortVector g_aChildOffsets_004682f0[35];
extern const ShortPoint g_aWeaponDisplayPositions_00468440[10];
extern int  *DAT_0046b1a8;
extern int  *DAT_0046b1ac;
extern int  *DAT_0046b1b0;
extern int   DAT_0046b1b4;
extern char  DAT_00486078[256];

extern unsigned char g_bMessageSpeed_0046af68;
extern unsigned char g_abManeuverRerollChance_00465678[47];
extern void (*g_apShipAiManeuverHandlers_004656a8[47])(short, short);

/*
 * BRAINS.C names for retail state that is still backed by the provisional
 * oversized allocations above.  Keep these as aliases until the original
 * compilation-unit data layout is known; changing the backing allocations
 * here would move every later synthetic global.
 */
#define g_nEnemySighting_00465c7c \
    (*(short *)((unsigned char *)g_aiPacketReferenceTable_00465c88 - 0x0c))
#define g_cViableTargetCount_0046c088 \
    (*(signed char *)((unsigned char *)&g_nAutoEngageTimer_0046c084 + 4))

#define g_nTargetShip_0059c3b0 \
    (*(short *)((unsigned char *)g_aeSpecialManeuver_0059c3c0 - 0x10))
#define g_asShipAfterburnerTimer_0059c810 ((short *)(void *)DAT_0059c810)
#define g_acShipCommunicator_0059c850 \
    ((signed char *)((unsigned char *)DAT_0059c810 + 0x40))
#define g_acViableTarget_0059c920 \
    ((signed char *)((unsigned char *)DAT_0059c810 + 0x110))
#define g_asActionCount_0059c930 \
    ((short *)((unsigned char *)DAT_0059c810 + 0x120))
#define g_aiPilotLevel_0059cf30 \
    ((int *)((unsigned char *)DAT_0059cf20 + 0x10))
#define g_acTurnRegulator_0059cf10 \
    ((signed char *)((unsigned char *)DAT_0059cf20 - 0x10))

#define g_anShipMissionShip_0059d4b0 \
    ((short *)((unsigned char *)DAT_0059d500 - 0x50))
#define g_vToTarget_0059d4d0 \
    (*(FixedVector *)((unsigned char *)DAT_0059d500 - 0x30))
#define g_acFormationMemberList_0059d490 \
    ((signed char *)((unsigned char *)DAT_0059d500 - 0x70))
#define g_acShipAiCooldown_0059d680 \
    ((signed char *)((unsigned char *)g_aeShipSide_0059d650 + 0x30))
#define g_acTurnInterval_0059d7d0 \
    ((signed char *)((unsigned char *)g_abShipNavPointIndex_0059d7c0 + 0x10))
#define g_vNormalizedToTarget_005a7db0 \
    (*(FixedVector *)((unsigned char *)DAT_005a7dd0 - 0x20))
#define g_nLastFoundShip_005a7cba \
    (*(short *)((unsigned char *)g_asCollisionPartner_005a7cc0 - 6))

#define g_abObjectField_0059b4a0 \
    ((unsigned char *)g_aeObjectType_0059b560 - 0xc0)
extern ShortVector g_aShipFormationOffset_0059b520[10];

/* Ordered name tables read directly out of the image; see include/wcdata.h.
 * Both are packed variable-length strings, not fixed-stride arrays, so the
 * game indexes them through a pointer table rather than by multiplication. */
extern const char g_aszObjectTypeDisplayNames_004684d4[]; /* Hornet .. Star post, then weapons */
extern const char g_aszKilrathiAceNames_0046afd4[];  /* Bhurak, Dakhath, Khajja, Bakhtosh */

#endif /* WC1_GLOBALS_H */
