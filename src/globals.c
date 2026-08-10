/*
 *  Definitions for the globals declared in include/globals.h.
 *
 *  Provisional: the original scatters these across the modules that own them,
 *  so this file has no counterpart in the shipped image and contributes no
 *  code.  It exists so that regrouping functions cannot break the link.
 */
#include "wc1.h"

unsigned int g_nHazardReferenceSpeed_00465040;
short g_nActiveHazards_00465044;
int DAT_00465058 = 1;
unsigned int DAT_0046505c;
unsigned int g_nGuardedAllocationTotalBytes_00465060;
unsigned int g_nGuardedAllocationBytes_00465064;
unsigned int g_nGuardedAllocationPeakBytes_00465068;
int DAT_0046506c;
int DAT_00465070;
int DAT_00465074 = 1;
int DAT_0046507c = 1;
int DAT_00465080;
int DAT_004650a8;
unsigned int DAT_004650ac;
GuardedAllocation *g_pGuardedAllocationHead_004650b0;
const char g_szJoystickDevCapsFailure_004652dc[26] =
    "Joystick: !joyGetDevCaps\n";
short DAT_00465460;
short g_nAutopilotFormationShipCount_00465544;
short g_nEnemySighting_00465c7c = 0x7fff;
int DAT_00465c84;
int g_aiPacketReferenceTable_00465c88[4 * 0x25];
const ShortVector g_aaFormationPositions_00465ed8[5][8] = {
    {
        { 0, 0, 0 }, { -750, 0, 0 }, { 750, 0, 0 }, { 0, 0, -750 },
        { 0, 0, 750 }, { -750, 0, -750 }, { 750, 0, -750 },
        { 0, 0, -1500 }
    },
    {
        { 0, 0, 0 }, { 750, 0, 0 }, { -750, -100, -250 },
        { 1500, -100, -250 }, { -1500, -200, -500 },
        { -2250, -300, -750 }, { 2250, -200, -500 },
        { 3000, -300, -750 }
    },
    {
        { 0, 0, 0 }, { 750, 0, -500 }, { -750, 0, -500 },
        { 0, 0, -1000 }, { -1500, 0, -1000 }, { -750, 0, -1500 },
        { 1500, 0, -1000 }, { 750, 0, -1500 }
    },
    {
        { 0, 0, 0 }, { 750, 0, -250 }, { 0, 325, -500 },
        { 750, -325, -750 }, { 0, 325, -500 }, { 750, -325, -750 },
        { 0, 0, -1000 }, { 750, 0, -1250 }
    },
    {
        { 0, 0, 0 }, { 0, 0, -750 }, { -750, 0, -500 },
        { -750, 0, -1250 }, { 750, 0, -500 }, { 750, 0, -1250 },
        { 0, 500, -500 }, { 0, 500, -1250 }
    }
};
const int g_anPilotTurnInterval_00465fc8[16] = {
    5, 5, 4, 4, 3, 3, 3, 2, 2, 1, 3, 3, 3, 3, 2, 2
};
unsigned char *g_pIntroSceneResource_00467b84;
unsigned char *g_pIntroSceneResourceMirror_00467c0b;
unsigned char *g_pIntroBackgroundResource_00467eae;
unsigned short DAT_00468660;
unsigned int DAT_00468664 = 1;
unsigned char DAT_0046870c;
unsigned char DAT_00468710;
int DAT_00468754;
int DAT_004688cc;
int DAT_004688d0;
int DAT_004688d4;
int DAT_004688d8;
short DAT_004688dc;
int DAT_004688e0;
short DAT_004688e4 = -1;
short DAT_004688e8 = -1;
unsigned short DAT_004688ec = 1;
int DAT_004688f0;
const char *g_pszIntroOpeningText_00468910 =
    "In the distant future,\n"
    "mankind is locked in a deadly war...";
int g_nIntroCreditCount_00468a30 = 11;
const char *g_apszIntroCredits_00468a38[20] = {
    "Design\nby\nChris Roberts",
    "Software Engineers\nChris Roberts\nKen Demarest III\nPaul C. Isaac\nSteve Muchow\nHerman Miller\nSteve Beeman\n",
    "Dogfight Intelligence\nKen Demarest III\n\nDogfight Choreography\nSteve Beeman\nErin Roberts",
    "3Space System\nby\nChris Roberts\n\nOriginFX Graphic System\nChris Roberts\nJohn Miles",
    "OriginFX Sound System\nby\nHerman Miller",
    "Artwork\nDenis Loubet\nGlen Johnson\nDaniel Bourbonnais\nKeith Berdak\nJohn Watson",
    "Screenplay by Jeff George\n\nAdditional Writing\nSteve Cantrell\nPhilip Brogden",
    "Soundtrack by\nGeorge A. Sanger and Dave Govett",
    "Sound Effects by Marc Schaefgen",
    "Produced by\nChris Roberts and Warren Spector",
    "Directed by\nChris Roberts",
    "Windows 95 Team",
    "Combat Programmers\n\nJeff Mangler Everett\nJeff jefftep Grills\nChuck Bishop Karpiak\nKris Goblin Pelley",
    "Sound System\n\nRichard Cupcake Lyle",
    "Soundtrack Rescored by\n\nI Need Names",
    "Head Whiner\n\nAnthony Sommers",
    "Whiners\n\nMonte Mathis\nHal Milton\nDieter Martin",
    "Richard Zinser\nKanon Lillemon\n",
    "Special Thanks To\n\nSocks\nand\nCaffeine",
    0
};

TitleMenuRegion g_aTitleMenuRegions_00468a88[5] = {
    { 1, 49, 48, 283, 99 },
    { 1, 49, 91, 283, 149 },
    { 1, 49, 134, 283, 149 },
    { 1, 49, 177, 283, 209 },
    { -1, 0, 0, 0, 0 }
};

PacketResourceDescriptor g_aIntroResourceDescriptors_00468ac0[3] = {
    { &g_pIntroBackgroundResource_00467eae, 3, 2 },
    { &g_pIntroSceneResource_00467b84, 3, 5 },
    { 0, 0, 0 }
};

int g_bTitleMenuSceneInitialized_00468ad8;
int g_nCapitalShipViewDistance_00468ff4 = 0x7d000;
unsigned char DAT_00468ff8;
int g_bAlternateChaseView_00468ffc;
char *DAT_00469004;
char *DAT_00469008;
unsigned char DAT_0046900c = 0xff;
const short g_asPilotHandOffsets_00469018[34] = {
    6, -3, 7, 2, 7, 9, 7, 12, 8, 13, 0, -1, -1, -1,
    -4, -1, -6, -1, 6, 0, 8, 0, 10, 0, 13, 3, 8, -7,
    6, -9, 5, -11, 5, -14
};
unsigned char *g_pCockpitPilotShape_0046905c;
unsigned char *g_pCockpitExplosionBackground_00469060;
unsigned char * volatile g_pCockpitExplosionShape_00469064;
volatile short g_nCockpitExplosionFrame_00469068 = 8;
unsigned char *g_pReleaseWeaponDisplayBackground_0046906c;
volatile signed char g_cReleaseWeaponDisplayFrame_00469070 = -1;
volatile signed char g_cReleaseWeaponDisplayTicks_00469074;
volatile signed char g_cReleaseWeaponDisplayState_00469078;
unsigned short DAT_00469090 = 0xffff;
char *PTR_s_MISSILE_LOCKED_004691d4 = (char *)g_szMissileLocked_004692a8;
unsigned char g_bTargetBracketVisible_004691d8 = 1;
unsigned char g_abTargetBracketPadding_004691d9[7];
short DAT_004691e0[10] = {
    18, 14, 71, 5, 80, 29, 49, 27, 14, 13
};
short g_nTargetLockMarkerX_004691f4 = -0x7fff;
ShortRect g_stTargetBracketBounds_004691f8 = {-0x7fff, 0, 0, 0};
ShortRect g_stPreviousTargetBracketBounds_00469200 = {-0x7fff, 0, 0, 0};
short DAT_00469208 = -1;
const ShortPoint g_aaCockpitDamagePositions_00469228[5][4] = {
    {{224, 5}, {132, 96}, {233, 107}, {149, 161}},
    {{177, 6}, {153, 142}, {103, 140}, {55, 183}},
    {{107, 25}, {211, 32}, {21, 178}, {300, 178}},
    {{74, 10}, {294, 19}, {197, 105}, {105, 134}},
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}}
};
unsigned char *g_pConfedCommBackground_00469278;
unsigned char *g_pCommStaticShape_0046927c;
unsigned char *g_pKilrathiCommBackground_00469280;
const char g_szComponentHitFormat_004692e0[8] = "%s HIT";
unsigned char DAT_004693b0;
const char *g_pGameVersion_004693b4 = g_szGameVersion_004693b8;
const char g_szGameVersion_004693b8[9] = "1.03F-95";
unsigned int g_dwModalBoundsTopLeft_00469440 = 0x00280018;
unsigned int g_dwModalBoundsBottomRight_00469444 = 0x003c0128;
ModalTextPanel *g_pModalTextPanel_00469448;
const short g_asCampaignPilotFiles_00469450[3] = {58, 61, 74};
const short g_asCampaignBriefingFiles_00469458[3] = {10, 62, 73};
const short g_asMissionDataFiles_00469460[3] = {15, 52, 72};
int g_nDebugOverlayConsoleCount_00469644;
DWORD g_dwDebugOverlayKey_00469648;
DWORD g_dwDebugOverlayKeyLatch_0046964c;
HHOOK g_hDebugKeyboardHook_00469650;
const char g_szDebugOverlayFontName_00469654[8] = "Courier";
const char g_szDebugOverlaySpinner_0046965c[5] = "-\\|/";
const char g_szDebugOverlayNewline_00469664[4] = "\n";
char g_szDiskMarkerFile_00469688[9] = "DISK.000";
short g_nDiskPromptBorderColour_00469694 = 0x50;
short g_nKeyboardPointerStep_004696a4 = 4;
const signed char g_acGunRefireDelay_0046995c[4] = {6, 10, 4, 0};
const char g_szComponentFixedFormat_00469984[8] = "%s FIXD";
unsigned char DAT_0046999c;
unsigned char g_cViewportClearColour_004699a0 = 15;
unsigned char DAT_004699a4 = 0x25;
unsigned char DAT_004699a8 = 0x47;
unsigned char DAT_004699ac = 0x50;
unsigned char DAT_004699b0 = 0xaa;
unsigned char DAT_004699b4 = 0xa6;
unsigned char g_cDefaultTextColour_004699cc = 0xa8;
unsigned char DAT_004699d8 = 0xbf;
int g_bGraphicsActive_00469a20;
PacketResourceDescriptor g_aMissionResourceDescriptors_00469c20[5] = {
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_HYPERSPACE_JUMP_FLASH].shapeSet,
      3, 14 },
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_EXPLOSION1].shapeSet, 3, 2 },
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_EXPLOSION2].shapeSet, 3, 3 },
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet,
      3, 5 },
    { 0, 0, 0 }
};
PacketResourceDescriptor g_aCockpitSecondaryResources_00469ce0[5] = {
    { &g_pCockpitDamageShape_005a76f4, -1, 7 },
    { &g_pCockpitWeaponShape_005a7564, -1, 9 },
    { &g_pCockpitIndicatorShape_005a7658, 8, 4 },
    { &g_pTargetLockShape_005a6bf4, 8, 0 },
    { 0, 0, 0 }
};
PacketResourceDescriptor g_aCockpitPrimaryResources_00469d08[8] = {
    { &g_pCockpitPilotShape_0046905c, -1, 4 },
    { &g_pCockpitDamageShape_005a76f4, -1, 7 },
    { &g_pCockpitWeaponShape_005a7564, -1, 9 },
    { &g_apCockpitShapes_005a7c80[0], -1, 0 },
    { &g_apCockpitShapes_005a7c80[1], -1, 1 },
    { &g_apCockpitShapes_005a7c80[2], -1, 2 },
    { &g_apCockpitShapes_005a7c80[3], -1, 3 },
    { 0, 0, 0 }
};
unsigned int g_bIntroSceneResourcesActive_00469d48 = 1;
int g_bCockpitResourcesActive_00469d58;
unsigned char DAT_00469d5c;
Viewport g_stTrainSimPanelViewport_00469da8;
/* This is four zero bytes, not a pointer table.  The original instruction at
 * 0x425E07 uses 0x469DBC as a compiler-generated biased displacement so that
 * index 9 lands on the real pointer table at 0x469DE0. */
unsigned int g_dwHighScoreNameTableBiasPadding_00469dbc;
const ShortRect g_stTrainSimPanelBounds_00469dc0 = {
    0x30, 0x1d, 0x110, 0x6d
};
const char g_szTrainSimTitle_00469dc8[24] = "SQUADRON: TRAINSIM";
unsigned char *g_apszBuiltInHighScoreNames_00469de0[6] = {
    &g_aszBuiltInHighScores_00469e38[0],
    &g_aszBuiltInHighScores_00469e38[8],
    &g_aszBuiltInHighScores_00469e38[16],
    &g_aszBuiltInHighScores_00469e38[24],
    &g_aszBuiltInHighScores_00469e38[32],
    &g_aszBuiltInHighScores_00469e38[40]
};
short DAT_00469df8[26] = {
    1, 47, 29, 67, 49, 1, 47, 89, 67, 109,
    1, 251, 29, 271, 49, 1, 251, 89, 271, 109,
    -1, 0, 0, 0, 0, 0
};
int g_nTrainSimActive_00469e2c;
short g_nTrainSimMission_00469e30;
short g_nArcadeWave_00469e34;
unsigned char g_aszBuiltInHighScores_00469e38[48] =
    "BISHOP\0\0"
    "GOBLIN\0\0"
    "JEFFTEP\0"
    "MANGLER\0"
    "THE MAN\0"
    "MONGO\0\0\0";
unsigned int g_dwTrainSimStringPadding_00469e68;
const char g_szTextFlushToken_00469e6c[4] = "%P";
const char g_szNewPilotPrompt_00469e70[80] =
    "CONGRATULATIONS!\n"
    "YOU HAVE A TOP SCORE!\n"
    "PLEASE ENTER YOUR\n"
    "NAME AND CALLSIGN:\n";
const char g_szDefaultPilotName_00469ec0[8] = "Blair";
const char g_szPilotNameLabel_00469ec8[12] = "LAST NAME: ";
const char g_szDefaultCallsign_00469ed4[12] = "Maverick";
const char g_szCallsignLabel_00469ee0[12] = "CALLSIGN : ";
const char g_szCheaterCallsign_00469eec[8] = "CHEATER";
const char g_szHighScoreCongratulations_00469ef4[68] =
    "*******\n"
    "CONGRATULATIONS!\n"
    "YOU HAVE SCORE NUMBER\n"
    ">>>> %d <<<<\n"
    "*******";
const char g_szLowScoreMessage_00469f38[56] =
    "> SORRY <\n\n"
    "YOUR SCORE IS ONLY\n"
    "%ld0\n\n"
    "PLEASE PLAY AGAIN!";
const char g_szHighScoresHeading_00469f70[16] = "%JHIGH SCORES%P";
const char g_szHighScoreNumberFormat_00469f80[8] = "%0ld";
const char g_szHighScoreRowFormat_00469f88[16] =
    "%X%Y%d. %s%X%s0";
int g_nCannedSceneMode_00469fac;
int g_nArcadeState_00469fb0;
short DAT_00469fb4 = 1;
short g_nFrameSkip_00469fb8 = 1;
int DAT_00469fbc = 0x14;
int DAT_00469fc0 = 0x14;
int g_bViewportDirty_00469fc4;
int DAT_00469fc8[11] = {
    0, 40000, 50000, 55000, 60000, 61000,
    61500, 62000, 63000, 63500, 64000
};
int g_nOriginDevUnlock_00469ff4;
int DAT_00469ffc = 1;
int DAT_0046a000 = 1;
unsigned char DAT_0046a004;
int DAT_0046a008;
int g_nShowMemoryStatus_0046a00c;
short DAT_0046a010 = -1;
short g_nArcadeBonusCountdown_0046a014;
int g_bMouseCursorVisible_0046a018;
unsigned char g_bInputPollingGuard_0046a01c;
int DAT_0046a020;
int DAT_0046a024;
unsigned int DAT_0046a028;
int DAT_0046a02c;
ActiveSoundEntry *g_pActiveSoundHead_0046a438;
ActiveSoundEntry *g_pActiveSoundTail_0046a43c;
unsigned char DAT_0046a440;
WaveTableEntry *g_pWaveTableHead_0046a444;
WaveTableEntry *g_pWaveTableTail_0046a448;
const char g_szPlayWaveOpenError_0046a46c[36] =
    "playWAVE Unable to open file '%s'";
unsigned char *DAT_0046a748;
const char *g_apszComponentNames_0046a778[6] = {
    g_szIonDrive_0046a7c4,
    g_szPowerPlant_0046a7d0,
    g_szShieldGenerator_0046a7dc,
    g_szComputerSystem_0046a7ec,
    g_szIntercomUnit_0046a7fc,
    g_szTargetTracking_0046a80c
};
short g_nScriptedViewObject_0046a8d0 = -1;
int g_bScriptedView_0046a8d4;
short DAT_0046a9f8;
int g_bMusicCommandSuppressed_0046a9fc;
short DAT_0046aa04 = 1;
unsigned int DAT_0046aa14 = 0xffffffff;
int g_nMusicStreamSet_0046aa18 = -1;
int DAT_0046aa24 = -1;
int DAT_0046aa2c = -1;
unsigned char DAT_0046aa30 = 1;
unsigned char DAT_0046aa34 = 1;
unsigned char DAT_0046aa38 = 1;
unsigned char DAT_0046aa3c;
unsigned char DAT_0046aa40;
const char g_szSfxWaveFormat_0046ad2c[12] = "sfx%02i.wav";
unsigned char g_abTaggedAllocationPrefix_0046ad88[8] = {
    'j', 'e', 'f', 'f', 0, 0, 0, 0
};
short g_nInputDoubleClickInterval_0046af54 = 1;
short g_nMenuPointerSpeed_0046af58 = 2;
short g_nCommMenuChoiceCount_0046af60 = -1;
short g_nCommMenuReuseMode_0046af64;
unsigned char DAT_0046af6c = 1;
unsigned char DAT_0046af70;
unsigned char DAT_0046af78 = 1;
unsigned char DAT_0046afc4 = 0xff;
short g_nCommSpeakerObject_0046afc8;
short g_nCommSpeakerRating_0046afcc;
short g_nCommPortraitIndex_0046afd0 = -1;
unsigned short DAT_0046b168 = 0xffff;
const GUID g_guidDirectDraw2_00463118 = {
    0xb3a6f3e0, 0x2b43, 0x11cf,
    { 0xa2, 0xde, 0x00, 0xaa, 0x00, 0xb9, 0x33, 0x56 }
};
int *DAT_0046b1a4;
int *DAT_0046b1a8;
int *DAT_0046b1ac;
int *DAT_0046b1b0;
int DAT_0046b1b4 = -1;
int DAT_0046b1b8 = 62;
int DAT_0046b1bc;
int DAT_0046b1c4;
int DAT_0046b1c8;
float g_fSpaceFlightFrameRate_0046b1cc = 20.0f;
float g_fCinematicFrameRate_0046b1d0 = 16.0f;
const short g_aasCockpitLightX_0046dca8[5][7] = {
    {101, 101, 189, 137, 265, -99, 203},
    {198, 198, 133, 145, 234, 234, -99},
    {76, 76, 139, 257, 20, 20, -99},
    {194, 194, 96, 137, 96, 96, -99},
    {-99, -99, -99, 179, 133, -99, -99}
};
const short g_aasCockpitLightY_0046dcf0[5][7] = {
    {122, 145, 121, 7, 121, -99, 147},
    {120, 139, 114, 114, 179, 188, -99},
    {164, 183, 22, 181, 162, 178, -99},
    {119, 142, 136, 15, 126, 116, -99},
    {-99, -99, -99, 115, 114, -99, -99}
};
const signed char g_aacCockpitLightOffFrame_0046dd38[5][7] = {
    {14, 14, 14, 11, 13, 14, 14},
    {12, 12, 12, 13, 12, 12, 12},
    {9, 9, 9, 14, 9, 9, 9},
    {15, 15, 13, 11, 13, 13, 13},
    {5, 5, 5, 5, 5, 5, 5}
};
const signed char g_aacCockpitLightOnFrame_0046dd60[5][7] = {
    {3, 3, 3, 0, 2, 3, 3},
    {2, 2, 2, 3, 2, 2, 2},
    {1, 1, 1, 6, 1, 1, 1},
    {4, 4, 2, 0, 2, 2, 2},
    {1, 1, 1, 1, 1, 1, 1}
};
const CockpitBarDefinition g_aaCockpitBars_0046dd88[5][8] = {
    {
        {0, 215, 122, 219, 152, 31, 16, 5},
        {2, 143, 97, 175, 99, 33, 12, 1},
        {0, 114, 127, 123, 130, 4, 17, 6},
        {1, 114, 143, 123, 146, 4, 20, 9},
        {2, 109, 132, 112, 141, 4, 18, 7},
        {3, 125, 132, 128, 141, 4, 19, 8},
        {0, 109, 122, 128, 125, 4, 15, 4},
        {1, 109, 148, 128, 151, 4, 21, 10}
    },
    {
        {2, 96, 14, 134, 17, 39, 10, 0},
        {2, 185, 14, 223, 17, 39, 11, 1},
        {0, 213, 124, 220, 127, 4, 15, 5},
        {1, 213, 138, 220, 141, 4, 18, 8},
        {2, 205, 129, 208, 136, 4, 16, 6},
        {3, 225, 129, 228, 136, 4, 17, 7},
        {0, 205, 116, 228, 122, 7, 14, 4},
        {1, 205, 143, 228, 149, 7, 19, 9}
    },
    {
        {2, 249, 165, 293, 169, 45, 11, 3},
        {2, 131, 13, 189, 17, 59, 8, 0},
        {0, -99, -99, -99, -99, 0, 0, 0},
        {1, -99, -99, -99, -99, 0, 0, 0},
        {2, 45, 172, 51, 181, 7, 12, 4},
        {3, 70, 172, 76, 181, 7, 13, 5},
        {0, 49, 164, 72, 170, 7, 10, 2},
        {1, 49, 183, 72, 189, 7, 15, 7}
    },
    {
        {0, 123, 118, 126, 148, 31, 14, 3},
        {2, 144, 100, 174, 102, 31, 12, 1},
        {0, 207, 124, 216, 127, 4, 17, 6},
        {1, 207, 140, 216, 143, 4, 20, 9},
        {2, 202, 129, 205, 138, 4, 18, 7},
        {3, 218, 129, 221, 138, 4, 19, 8},
        {0, 202, 119, 221, 122, 4, 16, 5},
        {1, 202, 145, 221, 148, 4, 21, 10}
    },
    {
        {0, 180, 124, 184, 146, 22, 6, 2},
        {2, 143, 110, 175, 112, 33, 4, 0},
        {0, -99, -99, 0, 0, 0, 0, 0},
        {0, -99, -99, 0, 0, 0, 0, 0},
        {0, -99, -99, 0, 0, 0, 0, 0},
        {0, -99, -99, 0, 0, 0, 0, 0},
        {0, 134, 124, 138, 146, 32, 7, 3},
        {0, -99, -99, 0, 0, 0, 0, 0}
    }
};
const short g_asCockpitLayout_0046e000[144] = {
    0, 0, 0, 0, 99, 130, 227, 122, 38, 164, 192, 126,
    -99, -99, 0, 0, 99, 140, 227, 140, 38, 184, 192, 136,
    -99, -99, 0, 0, 201, 105, 156, 14, 197, 145, 262, 102,
    219, 115, 0, 0, 112, 105, 156, 19, 105, 145, 38, 102,
    71, 115, 0, 0, 10, 133, 82, 198, 0, 99, 73, 165,
    6, 1, 78, 66, 11, 111, 84, 176, 48, 126, 120, 191,
    236, 133, 309, 198, 246, 97, 319, 165, 241, 1, 313, 66,
    235, 111, 308, 176, 198, 126, 270, 191, 159, 126, 145, 113,
    173, 139, 103, 132, 89, 119, 117, 145, 160, 39, 146, 26,
    174, 52, 159, 129, 145, 116, 173, 142, 159, 130, 145, 117,
    173, 143, 0, 0, 120, 152, 203, 199, 120, 152, 205, 199,
    120, 151, 213, 199, 120, 152, 205, 199, 120, 152, 203, 199
};
unsigned char g_bCurrentKey_0046c014 = 0x80;
signed char g_cPreviousKey_0046c018 = (signed char)0x80;
short g_nCurrentWave_0046c01c = -1;
int g_bIntroSecondaryScene_0046c024;
signed char g_abHazardObjects_0046c028[0x14] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};
short g_nEyePitchRate_0046c004 = 1;
short g_nEyeYawRate_0046c008 = 1;
short g_nEyeRollRate_0046c00c = 1;
short DAT_0046c010 = -1;
signed char g_cCurrentObjective_0046c020 = -1;
int DAT_0046c03c = -1;
signed char g_cViewObject_0046c000 = -1;
short g_nExternalViewShip_0046c040 = -1;
short g_nClosestVisibleObject_0046c048 = -1;
short g_nYourWingman_0046c04c = -1;
short g_nPlayerCollisionObject_0046c050 = -1;
enum ObjectType g_eSelectedGunType_0046c054 = (enum ObjectType)-1;
int g_nSelectedReleaseWeaponIndex_0046c058 = -1;
short DAT_0046c05c;
short g_bTargetLockReadoutDirty_0046c060;
short g_nTargetLockCountdown_0046c064;
short g_nTargetLockMarkerAngle_0046c068;
signed char g_cTargetDisplayObject_0046c06c = -1;
int g_bTargetLockAcquired_0046c074;
short g_nTargetLockMode_0046c078;
int g_bExternalViewSoundEnabled_0046c07c;
int g_bEngageAllowed_0046c080;
short g_nAutoEngageTimer_0046c084 = -1;
const short g_asIntroCameraSequence_0046c090[20] = {
    0, -1000, 0, -4263, 2, 15, 1, 0, 30, 0,
    3, 15, 4, 30, 1, 13, 14, 400, -1, 0
};
const short g_asCannedSequence32_0046c0b8[] = {
    0, 20, 1, 0, 0, 180, 40, 1, 0, 0, 180, 40,
    2, 0, 400, -1
};
const short g_asCannedSequence33_0046c0d8[] = {
    0, 20, 0, 20, 1, 0, -15, -60, 50, 0, 400, -1
};
const short g_asCannedSequence34_0046c0f0[] = {
    0, 20, 0, 20, 1, 0, 15, 60, 50, 0, 400, -1
};
const short g_asCannedSequence35_0046c108[42] = {
    0, 17, 0, 6, 3, 0, 6, 3, 0, 6, 3, 0, 6, 3,
    0, 6, 3, 0, 6, 3, 0, 6, 3,
    1, 0, 0, 180, 40, 1, 0, 0, 180, 40,
    1, 0, -30, 60, 40, 0, 400, -1
};
const short g_asCannedSequence37_0046c190[] = {
    0, 100, 1, 0, 30, 0, 50, 0, 20, 2, 0, 400, -1
};
const short g_asCannedSequence38_0046c1b0[] = {
    0, 110, 1, 0, 30, 0, 50, 0, 10, 2, 0, 400, -1
};
const short g_asCannedSequence39_0046c1d0[] = {
    0, 10, 2, 0, 400, -1
};
const short g_asCannedSequence40_0046c1e0[] = {
    0, 40,
    3, 1, 0, 0, 90, 60, 3, 1, 0, 0, 90, 60,
    3, 1, 0, 0, 90, 60, 3, 1, 0, 0, 90, 60,
    3, 1, 0, 0, 90, 60, 3, 1, 0, 0, 90, 60, -1
};
const short g_asCannedSequence41_0046c230[3] = { 0, 400, -1 };
const short g_asCannedSequence42_0046c250[] = {
    0, 120, 1, 0, 0, 180, 50, 3, 1, 0, 0, 180, 50, 3, -1
};
const short g_asCannedSequence43_0046c270[] = {
    0, 120, 1, 0, 0, 180, 50, 3, 1, 0, 0, 180, 50,
    3, 1, 0, 0, 180, 50, 3, -1
};
const short g_asCannedSequence44_0046c2a0[] = {
    0, 5, 3, 0, 5, 3, 0, 5, 3, 0, 5, 3,
    1, 0, 0, 180, 50, 1, 0, 0, 180, 50,
    1, 0, 0, 180, 50, 1, 0, 0, 180, 50, -1
};
const short g_asCannedSequence45_0046c2e8[] = { 0, 20, 2, -1 };

const signed char g_acDirectionShapeFrame_0046db28[62] = {
    0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2,
    14, 13, 12, 11, 10, 9, 8, 9, 10, 11, 12, 13,
    15, 16, 17, 18, 19, 20, 21, 20, 19, 18, 17, 16,
    28, 27, 26, 25, 24, 23, 22, 23, 24, 25, 26, 27,
    29, 30, 31, 32, 33, 34, 35, 34, 33, 32, 31, 30,
    36
};
const signed char g_acDirectionShapeFlip_0046dbe8[62] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    0
};

MissionNavPoint g_aMissionNavPoints_0046c2f0[WC1_MISSION_NAV_POINT_COUNT] = {
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
    {
        "", 1, { 0, 0, 0 }, 50000,
        { { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 } },
        { OBJECT_TYPE_DRALTHI, OBJECT_TYPE_HORNET },
        { 32, 33, 34, 35, -1, -1, -1, -1, -1, -1 }
    },
    {
        "", 1, { 0, 0, 0 }, 50000,
        { { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 } },
        { OBJECT_TYPE_ASTEROID_FIELD, (enum ObjectType)-1 },
        { 36, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
    },
    {
        "", 1, { 0, 0, 0 }, 50000,
        { { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 } },
        { OBJECT_TYPE_GRATHA, OBJECT_TYPE_RAPIER },
        { 37, 38, 39, 40, 41, -1, -1, -1, -1, -1 }
    },
    {
        "", 1, { 0, 0, 0 }, 50000,
        { { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 } },
        { OBJECT_TYPE_KRANT, OBJECT_TYPE_SCIMITAR },
        { 42, 43, 44, 45, -1, -1, -1, -1, -1, -1 }
    }
};

MissionShipRecord g_aMissionShips_0046c948[WC1_MISSION_SHIP_COUNT] = {
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
    { OBJECT_TYPE_DRALTHI, SIDE_KILRATHI, -1, MISSION_TYPE_CANNED_SEQUENCE,
      16, { 0, 0, 0 }, -155, 0, 0, 0, 40, 3,
      { g_asCannedSequence32_0046c0b8 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_DRALTHI, SIDE_KILRATHI, -1, MISSION_TYPE_CANNED_SEQUENCE,
      16, { -154521, -25600, -232012 }, -155, 0, 0, 0, 40, 3,
      { g_asCannedSequence33_0046c0d8 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_DRALTHI, SIDE_KILRATHI, -1, MISSION_TYPE_CANNED_SEQUENCE,
      16, { -61849, 25600, -232012 }, -155, 0, 0, 0, 40, 3,
      { g_asCannedSequence34_0046c0f0 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_HORNET, SIDE_IMPERIAL, -1, MISSION_TYPE_CANNED_SEQUENCE,
      16, { 324582, 0, 696038 }, -155, 0, 0, 0, 40, 3,
      { g_asCannedSequence35_0046c108 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_ASTEROID_FIELD, SIDE_IMPERIAL, -1,
      MISSION_TYPE_CANNED_SEQUENCE, 17, { 0, 0, 0 }, 0, 0, 0, 0,
      30000, 3, { 0 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_GRATHA, SIDE_KILRATHI, -1, MISSION_TYPE_CANNED_SEQUENCE,
      18, { 102400, -153600, -332800 }, 0, 0, -30, 0, 80, 3,
      { g_asCannedSequence37_0046c190 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_GRATHA, SIDE_KILRATHI, -1, MISSION_TYPE_CANNED_SEQUENCE,
      18, { 0, 256000, -332800 }, 0, 0, 90, 0, 62, 3,
      { g_asCannedSequence38_0046c1b0 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_GRATHA, SIDE_KILRATHI, -1, MISSION_TYPE_CANNED_SEQUENCE,
      18, { 0, 256000, -153600 }, 0, 0, -30, 0, 80, 3,
      { g_asCannedSequence39_0046c1d0 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_RAPIER, SIDE_IMPERIAL, -1, MISSION_TYPE_CANNED_SEQUENCE,
      18, { 0, 256000, -819200 }, 0, 0, 30, 0, 60, 3,
      { g_asCannedSequence40_0046c1e0 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_RAPIER, SIDE_IMPERIAL, -1, MISSION_TYPE_CANNED_SEQUENCE,
      18, { 102400, -153600, -819200 }, 0, 0, 0, 0, 80, 3,
      { g_asCannedSequence41_0046c230 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_KRANT, SIDE_KILRATHI, -1, MISSION_TYPE_CANNED_SEQUENCE,
      19, { -51200, 76800, -1382400 }, 0, 0, 0, 0, 50, 3,
      { g_asCannedSequence42_0046c250 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_KRANT, SIDE_KILRATHI, -1, MISSION_TYPE_CANNED_SEQUENCE,
      19, { 153600, 0, -1280000 }, 0, 0, 60, 0, 50, 3,
      { g_asCannedSequence43_0046c270 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_KRANT, SIDE_KILRATHI, -1, MISSION_TYPE_CANNED_SEQUENCE,
      19, { -249856, 0, 1139200 }, 155, 0, 0, 0, 50, 3,
      { g_asCannedSequence44_0046c2a0 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_SCIMITAR, SIDE_IMPERIAL, -1,
      MISSION_TYPE_CANNED_SEQUENCE, 19, { -76800, 0, 768000 },
      155, 0, 0, 0, 50, 3, { g_asCannedSequence45_0046c2e8 },
      0, 0, 0, -1, 0, 0 }
};
int g_aiIntelligenceEvent_0046d368[10];
signed char g_aDefenseManeuversNovice_0046d390[8] = {
    24, 34, 13, 14, -1, 0, 0, 0
};
signed char g_aDefenseManeuversVeteran_0046d398[8] = {
    8, 13, 15, 14, 19, 24, -1, 0
};
signed char g_aDefenseManeuversElite_0046d3a0[16] = {
    8, 15, 17, 23, 19, 9, 20, 34, 14, -1, 0, 0, 0, 0, 0, 0
};
signed char g_aDefenseManeuversAce_0046d3b0[16] = {
    17, 23, 15, 19, 9, 14, 20, 12, -1, 0, 0, 0, 0, 0, 0, 0
};
signed char g_aDefenseManeuversBoss_0046d3c0[16] = {
    15, 19, 12, 11, 17, 23, 7, 35, -1, 0, 0, 0, 0, 0, 0, 0
};
signed char *g_apDefenseManeuvers_0046d3d0[5] = {
    g_aDefenseManeuversNovice_0046d390,
    g_aDefenseManeuversVeteran_0046d398,
    g_aDefenseManeuversElite_0046d3a0,
    g_aDefenseManeuversAce_0046d3b0,
    g_aDefenseManeuversBoss_0046d3c0
};
ManeuverChoice g_aRatedManeuverChoices_0046d3e8[13][9][3] = {
    {
        { { 70, 40, 30 }, { 80, 40, 30 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 80, 29, 37 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 50, 7, 8 }, { 75, 14, 17 }, { 100, 21, -1 } },
        { { 80, 41, 29 }, { 80, 41, 24 }, { 100, 21, -1 } },
        { { 60, 26, 29 }, { 70, 26, 29 }, { 90, 26, 29 } },
        { { 50, 16, 12 }, { 50, 13, 20 }, { 100, 21, -1 } },
        { { 100, 19, -1 }, { 50, 9, 20 }, { 100, 21, -1 } },
        { { 100, 2, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 40, 40, 30 }, { 80, 40, 30 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 50, 31, 40 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 55, 12, 19 }, { 70, 11, 12 }, { 100, 21, -1 } },
        { { 60, 29, 41 }, { 80, 29, 41 }, { 100, 33, -1 } },
        { { 80, 26, 29 }, { 50, 26, 29 }, { 100, 29, -1 } },
        { { 50, 15, 16 }, { 50, 16, 32 }, { 100, 21, -1 } },
        { { 50, 7, 10 }, { 50, 14, 9 }, { 100, 21, -1 } },
        { { 50, 25, 2 }, { 50, 25, 2 }, { 100, 2, -1 } }
    },
    {
        { { 40, 40, 30 }, { 60, 40, 30 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 50, 29, 40 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 60, 15, 17 }, { 50, 12, 17 }, { 100, 21, -1 } },
        { { 20, 41, 30 }, { 30, 41, 17 }, { 40, 41, 21 } },
        { { 100, 26, -1 }, { 100, 26, -1 }, { 100, 26, -1 } },
        { { 40, 32, 17 }, { 20, 32, 16 }, { 100, 21, -1 } },
        { { 50, 17, 20 }, { 50, 23, 36 }, { 100, 20, -1 } },
        { { 100, 2, -1 }, { 100, 2, -1 }, { 100, 21, -1 } }
    },
    {
        { { 50, 40, 30 }, { 90, 40, 30 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 80, 30, 37 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 44, -1 }, { 100, 44, -1 } },
        { { 70, 7, 16 }, { 100, 44, -1 }, { 100, 44, -1 } },
        { { 20, 31, 41 }, { 100, 44, -1 }, { 100, 44, -1 } },
        { { 80, 26, 31 }, { 100, 44, -1 }, { 100, 44, -1 } },
        { { 75, 16, 15 }, { 75, 16, 15 }, { 100, 44, -1 } },
        { { 50, 12, 16 }, { 100, 44, -1 }, { 100, 44, -1 } },
        { { 100, 2, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 60, 40, 30 }, { 100, 40, -1 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 50, 29, 40 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 39, -1 } },
        { { 50, 15, 16 }, { 60, 15, 7 }, { 70, 20, 16 } },
        { { 70, 41, 17 }, { 80, 41, 20 }, { 100, 20, -1 } },
        { { 100, 26, -1 }, { 100, 26, -1 }, { 100, 26, -1 } },
        { { 50, 16, 13 }, { 50, 16, 20 }, { 100, 21, -1 } },
        { { 50, 20, 36 }, { 50, 11, 10 }, { 100, 20, -1 } },
        { { 25, 25, 2 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 40, 40, 30 }, { 80, 40, 30 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 20, 29, 40 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 39, -1 } },
        { { 50, 7, 8 }, { 50, 9, 24 }, { 100, 12, -1 } },
        { { 80, 41, 29 }, { 80, 41, 20 }, { 100, 41, -1 } },
        { { 100, 26, -1 }, { 100, 26, -1 }, { 100, 26, -1 } },
        { { 20, 32, 16 }, { 50, 16, 20 }, { 100, 21, -1 } },
        { { 50, 19, 14 }, { 50, 14, 12 }, { 100, 8, -1 } },
        { { 50, 17, 23 }, { 50, 17, 23 }, { 30, 25, 2 } }
    },
    {
        { { 50, 30, 31 }, { 60, 40, 31 }, { 100, 39, -1 } },
        { { 80, 29, 37 }, { 80, 29, 37 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 39, -1 } },
        { { 50, 9, 10 }, { 60, 7, 11 }, { 100, 20, -1 } },
        { { 50, 31, 41 }, { 80, 31, 23 }, { 100, 33, -1 } },
        { { 80, 26, 29 }, { 70, 26, 29 }, { 100, 26, -1 } },
        { { 50, 32, 16 }, { 50, 20, 16 }, { 100, 21, -1 } },
        { { 50, 36, 12 }, { 50, 20, 8 }, { 50, 19, 20 } },
        { { 80, 25, 2 }, { 50, 25, 17 }, { 100, 2, -1 } }
    },
    {
        { { 50, 40, 30 }, { 80, 40, 30 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 50, 29, 40 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 60, 15, 8 }, { 70, 19, 14 }, { 100, 21, -1 } },
        { { 70, 29, 20 }, { 50, 29, 20 }, { 100, 41, -1 } },
        { { 100, 26, -1 }, { 100, 26, -1 }, { 100, 26, -1 } },
        { { 50, 16, 19 }, { 50, 16, 8 }, { 100, 21, -1 } },
        { { 50, 36, 12 }, { 50, 19, 20 }, { 100, 21, -1 } },
        { { 100, 2, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 100, -1, -1 }, { 100, -1, -1 }, { 100, -1, -1 } },
        { { 100, -1, -1 }, { 100, -1, -1 }, { 100, -1, -1 } },
        { { 100, -1, -1 }, { 100, -1, -1 }, { 100, -1, -1 } },
        { { 100, -1, -1 }, { 100, -1, -1 }, { 100, -1, -1 } },
        { { 100, -1, -1 }, { 100, -1, -1 }, { 100, -1, -1 } },
        { { 100, -1, -1 }, { 100, -1, -1 }, { 100, -1, -1 } },
        { { 100, -1, -1 }, { 100, -1, -1 }, { 100, -1, -1 } },
        { { 0, -1, -1 }, { 0, -1, -1 }, { 0, -1, -1 } },
        { { 0, -1, -1 }, { 0, -1, -1 }, { 0, -1, -1 } }
    },
    {
        { { 60, 40, 29 }, { 60, 40, 29 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 50, 29, 40 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 50, 16, 38 }, { 75, 7, 20 }, { 100, 21, -1 } },
        { { 60, 41, 31 }, { 60, 41, 31 }, { 100, 21, -1 } },
        { { 100, 26, -1 }, { 100, 26, -1 }, { 100, 26, -1 } },
        { { 50, 32, 20 }, { 50, 32, 20 }, { 100, 21, -1 } },
        { { 50, 19, 20 }, { 50, 9, 20 }, { 100, 21, -1 } },
        { { 100, 25, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 50, 40, 31 }, { 70, 40, 31 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 50, 29, 40 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 97, 12, 22 }, { 95, 20, 22 }, { 100, 21, -1 } },
        { { 40, 41, 31 }, { 20, 41, 31 }, { 100, 21, -1 } },
        { { 50, 26, 31 }, { 100, 31, -1 }, { 100, 26, -1 } },
        { { 50, 16, 20 }, { 50, 19, 20 }, { 100, 21, -1 } },
        { { 100, 20, 36 }, { 50, 12, 20 }, { 100, 21, -1 } },
        { { 100, 25, -1 }, { 100, 25, -1 }, { 100, 2, -1 } }
    },
    {
        { { 70, 40, 30 }, { 70, 16, 30 }, { 100, 21, -1 } },
        { { 100, 18, -1 }, { 100, 18, -1 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 50, 23, 20 }, { 75, 12, 17 }, { 100, 21, -1 } },
        { { 30, 41, 29 }, { 50, 41, 20 }, { 100, 21, -1 } },
        { { 100, 26, -1 }, { 100, 26, -1 }, { 100, 26, -1 } },
        { { 20, 32, 20 }, { 50, 16, 20 }, { 100, 21, -1 } },
        { { 50, 16, 19 }, { 50, 12, 20 }, { 100, 21, -1 } },
        { { 100, 25, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 80, 40, 31 }, { 90, 40, 31 }, { 100, 21, -1 } },
        { { 100, 18, -1 }, { 100, 18, -1 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 50, 15, 20 }, { 75, 16, 19 }, { 100, 21, -1 } },
        { { 50, 41, 31 }, { 50, 41, 31 }, { 100, 21, -1 } },
        { { 80, 26, 31 }, { 100, 26, -1 }, { 100, 26, -1 } },
        { { 50, 16, 20 }, { 50, 32, 20 }, { 100, 21, -1 } },
        { { 70, 19, 17 }, { 50, 11, 20 }, { 100, 21, -1 } },
        { { 100, 25, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    }
};
ManeuverChoice g_aKilrathiManeuverChoices_0046d808[5][9][3] = {
    {
        { { 70, 40, 45 }, { 80, 40, 45 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 50, 29, 40 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 90, 17, 46 }, { 90, 17, 46 }, { 100, 21, -1 } },
        { { 50, 46, 45 }, { 80, 46, 45 }, { 100, 21, -1 } },
        { { 100, 45, -1 }, { 100, 45, -1 }, { 100, 45, -1 } },
        { { 100, 46, -1 }, { 20, 17, 46 }, { 100, 21, -1 } },
        { { 100, 46, -1 }, { 25, 17, 46 }, { 100, 21, -1 } },
        { { 100, 2, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 40, 40, 45 }, { 70, 40, 45 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 50, 29, 30 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 50, 8, 46 }, { 50, 14, 46 }, { 100, 21, -1 } },
        { { 70, 45, 46 }, { 60, 45, 46 }, { 100, 21, -1 } },
        { { 100, 45, -1 }, { 100, 45, -1 }, { 100, 45, -1 } },
        { { 100, 46, -1 }, { 100, 46, -1 }, { 100, 21, -1 } },
        { { 50, 38, 46 }, { 100, 46, -1 }, { 100, 21, -1 } },
        { { 100, 2, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 80, 40, 29 }, { 80, 40, 29 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 50, 29, 40 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 60, 46, 45 }, { 60, 46, 45 }, { 100, 21, -1 } },
        { { 30, 41, 45 }, { 30, 41, 45 }, { 100, 21, -1 } },
        { { 100, 29, -1 }, { 100, 45, -1 }, { 100, 45, -1 } },
        { { 100, 46, -1 }, { 100, 46, -1 }, { 100, 46, -1 } },
        { { 50, 29, 46 }, { 100, 46, -1 }, { 100, 21, -1 } },
        { { 100, 2, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 50, 40, 29 }, { 50, 40, 29 }, { 100, 21, -1 } },
        { { 100, 18, -1 }, { 100, 18, -1 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 39, -1 } },
        { { 30, 12, 29 }, { 98, 46, 22 }, { 100, 21, -1 } },
        { { 90, 29, 31 }, { 100, 29, -1 }, { 100, 21, -1 } },
        { { 100, 45, -1 }, { 100, 45, -1 }, { 100, 45, -1 } },
        { { 100, 46, -1 }, { 100, 46, -1 }, { 100, 46, -1 } },
        { { 50, 36, 46 }, { 100, 46, -1 }, { 100, 21, -1 } },
        { { 100, 2, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 40, 40, 29 }, { 40, 40, 29 }, { 100, 21, -1 } },
        { { 100, 18, -1 }, { 100, 18, -1 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 39, -1 } },
        { { 90, 46, 36 }, { 90, 46, 12 }, { 100, 21, -1 } },
        { { 100, 29, -1 }, { 100, 29, -1 }, { 100, 21, -1 } },
        { { 100, 45, -1 }, { 100, 45, -1 }, { 100, 45, -1 } },
        { { 100, 46, -1 }, { 100, 46, -1 }, { 100, 21, -1 } },
        { { 10, 11, 46 }, { 100, 46, -1 }, { 100, 46, -1 } },
        { { 100, 2, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    }
};
signed char g_acPilotAggression_0046d9a0[24] = {
    3, 3, 3, 2, 2, 3, 2, 3, 0, 3, 2, 5,
    4, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0
};
signed char g_acPilotRecovery_0046d9b8[20] = {
    6, 7, 8, 8, 9, 8, 6, 8, 10, 7,
    8, 9, 7, 0, 7, 7, 8, 8, 0, 0
};
InputEvent *g_pInputEventHead_0046da90;
InputEvent *g_pInputEventTail_0046da94;
int g_bInputEventPoolInitialized_0046da98;
unsigned char *g_pDrawnMouseCursorShape_0046da9c;
int DAT_0046daa0;
int g_nScreenWidth_0046daa4 = 320;
int g_nScreenHeight_0046daa8 = 200;
const short g_aScreenViewportGeometry_0046dab8[6][8] = {
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 320, 128, 0, 24, 0, 24, -24576, -1 },
    { 320, 200, 0, 0, 0, 0, -1536, -1 }
};
const short g_asPilotHandOrigins_0046e120[10] = {
    154, 187, 154, 187, 154, 187, 154, 187, 154, 187
};
CampaignDate g_stSavedCampaignDate_0046e188 = {20, 340};
unsigned char g_abRasterPaletteTranslation_0046ff2c[256];
CampaignState g_stInitialCampaignState_004700b0 = {
    0,
    OBJECT_TYPE_HORNET,
    {0, 0, 0, 0, 0, 1, 1},
    0,
    {0, 0, 0, 0},
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    0,
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1},
    {110, 2654},
    {6, 0},
    0,
    0,
    0,
    0
};
PilotRecord g_aInitialPilotRecords_00470108[9] = {
    {"TANAKA",   "SPIRIT",  3, 1, 11, 14, 1},
    {"ST.JOHN",  "HUNTER",  4, 2, 25, 32, 4},
    {"CHEN",     "BOSSMAN", 1, 3, 35, 37, 2},
    {"CASEY",    "ICEMAN",  0, 3, 28, 43, 1},
    {"DEVEREAUX", "ANGEL",   0, 2, 22, 20, 1},
    {"TAGGART",  "PALADIN", 2, 3, 42, 34, 2},
    {"MARSHALL", "MANIAC",  4, 0, 5, 6, 1},
    {"KHUMALO",  "KNIGHT",  3, 2, 18, 23, 3},
    {"PELLEY",   "GOBLIN",  0, 0, 0, 0, 0}
};
int DAT_00470510;
unsigned char g_abPaletteTranslation_00470678[256] = {
      0,   1,   2,   3,   4,   5,   6,   7,
      8,   9,  10,  11,  12,  13,  14,  15,
     16,  17,  18,  19,  20,  21,  22,  23,
     24,  25,  26,  27,  28,  29,  30,  31,
     32,  33,  34,  35,  36,  37,  38,  39,
     40,  41,  42,  43,  44,  45,  46,  47,
     48,  49,  50,  51,  52,  53,  54,  55,
     56,  57,  58,  59,  60,  61,  62,  63,
     64,  65,  66,  67,  68,  69,  70,  71,
     72,  73,  74,  75,  76,  77,  78,  79,
     80,  81,  82,  83,  84,  85,  86,  87,
     88,  89,  90,  91,  92,  93,  94,  95,
     96,  97,  98,  99, 100, 101, 102, 103,
    104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127,
    128, 129, 130, 131, 132, 133, 134, 135,
    136, 137, 138, 139, 140, 141, 142, 143,
    144, 145, 146, 147, 148, 149, 150, 151,
    152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167,
    168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183,
    184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199,
    200, 201, 202, 203, 204, 205, 206, 207,
    208, 209, 210, 211, 212, 213, 214, 215,
    216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231,
    232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247,
    248, 249, 250, 251, 252, 253, 254, 255
};
unsigned short g_awAbsoluteCosine_00470778[360] = {
    255, 255, 255, 255, 255, 255, 255, 254, 254, 253, 252, 251,
    250, 250, 248, 247, 246, 245, 243, 242, 241, 239, 237, 236,
    234, 232, 230, 228, 226, 224, 221, 219, 217, 215, 212, 210,
    207, 204, 202, 198, 196, 193, 190, 187, 184, 181, 178, 175,
    171, 168, 165, 161, 158, 154, 150, 147, 143, 139, 136, 131,
    128, 124, 120, 116, 112, 108, 104, 100,  96,  92,  88,  83,
     79,  74,  71,  66,  62,  58,  53,  49,  45,  40,  36,  31,
     27,  22,  18,  13,   9,   4,   0,   4,   9,  13,  18,  22,
     27,  31,  36,  40,  45,  49,  53,  58,  62,  66,  71,  75,
     79,  83,  88,  92,  96, 100, 104, 108, 112, 116, 120, 124,
    128, 131, 136, 139, 143, 147, 150, 154, 158, 161, 165, 168,
    171, 175, 178, 181, 184, 187, 190, 193, 196, 198, 202, 204,
    207, 210, 212, 215, 217, 219, 221, 224, 226, 228, 230, 232,
    234, 236, 237, 239, 241, 242, 243, 245, 246, 247, 248, 250,
    250, 251, 252, 253, 254, 254, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 254, 254, 253, 252, 251,
    250, 250, 248, 247, 246, 245, 243, 242, 241, 239, 237, 236,
    234, 232, 230, 228, 226, 224, 221, 219, 217, 215, 212, 210,
    207, 204, 202, 198, 196, 193, 190, 187, 184, 181, 178, 175,
    171, 168, 165, 161, 158, 154, 150, 147, 143, 139, 136, 131,
    128, 124, 120, 116, 112, 108, 104, 100,  96,  92,  88,  83,
     79,  74,  71,  66,  62,  58,  53,  49,  45,  40,  36,  31,
     27,  22,  18,  13,   9,   4,   0,   4,   9,  13,  18,  22,
     27,  31,  36,  40,  45,  49,  53,  58,  62,  66,  71,  75,
     79,  83,  88,  92,  96, 100, 104, 108, 112, 116, 120, 124,
    128, 131, 136, 139, 143, 147, 150, 154, 158, 161, 165, 168,
    171, 175, 178, 181, 184, 187, 190, 193, 196, 198, 202, 204,
    207, 210, 212, 215, 217, 219, 221, 224, 226, 228, 230, 232,
    234, 236, 237, 239, 241, 242, 243, 245, 246, 247, 248, 250,
    250, 251, 252, 253, 254, 254, 255, 255, 255, 255, 255, 255
};
unsigned short g_awAbsoluteSine_00470a48[360] = {
      0,   4,   9,  13,  18,  22,  27,  31,  36,  40,  45,  49,
     53,  58,  62,  66,  71,  75,  79,  83,  88,  92,  96, 100,
    104, 108, 112, 116, 120, 124, 128, 131, 136, 139, 143, 147,
    150, 154, 158, 161, 165, 168, 171, 175, 178, 181, 184, 187,
    190, 193, 196, 198, 202, 204, 207, 210, 212, 215, 217, 219,
    221, 224, 226, 228, 230, 232, 234, 236, 237, 239, 241, 242,
    243, 245, 246, 247, 248, 250, 250, 251, 252, 253, 254, 254,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 254, 254, 253, 252, 251, 250, 250, 248, 247, 246, 245,
    243, 242, 241, 239, 237, 236, 234, 232, 230, 228, 226, 224,
    221, 219, 217, 215, 212, 210, 207, 204, 202, 198, 196, 193,
    190, 187, 184, 181, 178, 175, 171, 168, 165, 161, 158, 154,
    150, 147, 143, 139, 136, 131, 128, 124, 120, 116, 112, 108,
    104, 100,  96,  92,  88,  83,  79,  74,  71,  66,  62,  58,
     53,  49,  45,  40,  36,  31,  27,  22,  18,  13,   9,   4,
      0,   4,   9,  13,  18,  22,  27,  31,  36,  40,  45,  49,
     53,  58,  62,  66,  71,  75,  79,  83,  88,  92,  96, 100,
    104, 108, 112, 116, 120, 124, 128, 131, 136, 139, 143, 147,
    150, 154, 158, 161, 165, 168, 171, 175, 178, 181, 184, 187,
    190, 193, 196, 198, 202, 204, 207, 210, 212, 215, 217, 219,
    221, 224, 226, 228, 230, 232, 234, 236, 237, 239, 241, 242,
    243, 245, 246, 247, 248, 250, 250, 251, 252, 253, 254, 254,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 254, 254, 253, 252, 251, 250, 250, 248, 247, 246, 245,
    243, 242, 241, 239, 237, 236, 234, 232, 230, 228, 226, 224,
    221, 219, 217, 215, 212, 210, 207, 204, 202, 198, 196, 193,
    190, 187, 184, 181, 178, 175, 171, 168, 165, 161, 158, 154,
    150, 147, 143, 139, 136, 131, 128, 124, 120, 116, 112, 108,
    104, 100,  96,  92,  88,  83,  79,  74,  71,  66,  62,  58,
     53,  49,  45,  40,  36,  31,  27,  22,  18,  13,   9,   4
};
const char g_szSnowViewport_00470da4[16] = "snow_viewport";
int g_nInsertKeyState_00475b68;
int g_nClearedKeyboardState_00475b6c;
int g_nOemPeriodKeyState_00475bf0;
int g_nHomeKeyState_00475bf4;
int g_nPageDownKeyState_00475bf8;
int g_nOemCommaKeyState_00475bfc;
int g_nRightArrowKeyState_00475c00;
int g_nEndKeyState_00475c04;
int g_nClearKeyState_00475c08;
int g_nDeleteKeyState_00475c0c;
int g_nUpArrowKeyState_00475c10;
int g_nDownArrowKeyState_00475c14;
char g_szStreamsPath_00475c18[0x100];
int g_nPageUpKeyState_00475d18;
int g_nLeftArrowKeyState_00475d1c;
char g_szReadDataFileError_00475d20[0x40];
char g_szCreateDataFileError_00475d60[0x40];
char g_szWriteDataFileError_00475da0[0x40];
char g_szSeekDataFileError_00475de0[0x80];
int g_bMonoDebugInstalled_00475e70;
HANDLE g_hMonoDebugDevice_00475e74;
unsigned short DAT_00475e78;
unsigned char g_bCurrentManeuverReroll_00475e7c;
unsigned char g_abMouseCursorBackground_00475ff0[0x400];
DebugOverlayConsole *g_pDebugOverlay_004763f0;
int g_nFlightFrameTotalTicks_004764f8;
int g_nFlightCockpitTicks_004764fc;
LARGE_INTEGER g_liFlightAfterInput_00476500;
LARGE_INTEGER g_liFlightFrameEnd_00476508;
int g_nFlightPresentTicks_00476510;
LARGE_INTEGER g_liFlightFrameStart_00476518;
LARGE_INTEGER g_liFlightAfterSimulation_00476520;
int g_nFlightSimulationTicks_00476528;
LARGE_INTEGER g_liFlightAfterCockpit_00476530;
int g_nFlightInputTicks_00476538;
LARGE_INTEGER g_liFlightAfterRender_00476540;
int g_nFlightRenderTicks_00476548;
IxSample *g_pLoopingWaveSample_0047654c;
IxSound *g_pLoopingWaveSound_00476550;
char g_szSfxWavePath_00476558[0xc8];
unsigned char DAT_00476620[32];
unsigned int DAT_00476640;
unsigned int DAT_00476644;
unsigned char *DAT_00476648;
unsigned int DAT_0047664c;
unsigned int DAT_00476650;
unsigned char DAT_00476658[0xfa00];
int DAT_00486058;
int DAT_00486060;
HWND DAT_00486074;
char DAT_00486078[256];
unsigned char DAT_00486110[1024];
float DAT_00486510;
int DAT_00486518;
unsigned char DAT_004865a8[0x1000];
RasterSurface g_stRasterSurface_004875a8;
RasterClip g_stRasterClip_00496fc0;
unsigned char g_abSolidColourTranslation_00497648[256];
extern unsigned int g_dwStreamerState_00597cd0;
int g_bStreamerAudioPlaying_00597748;
char g_szStreamerPath_00597750[128];
int DAT_00598888;
int DAT_0059888c;
int DAT_00598890;
unsigned char *g_pMissionCampaignData_005988bc;
PilotRecord g_aPilotRecords_005988d0[9];
unsigned char *g_pPilotCampaignData_00598a28;
PilotRecord *g_apWingmanPilots_00598a30[8];
short DAT_00598ab0;
unsigned int DAT_00598ab6;
int DAT_00598aba;
unsigned int DAT_00598af4;
char g_szTextScratchBuffer_00598b00[256];
unsigned int DAT_00598c18;
InputEvent g_aInputEventPool_00598c40[0x100];
int g_nMouseCursorSavedY_0059a840;
int g_nMouseCursorSavedX_0059a844;
unsigned char g_bInputMode_0059a848;
int DAT_0059a84c;
short g_nEventManagerActive_0059a850;
short g_nViewCenterX_0059a852;
short g_nViewCenterY_0059a854;
short DAT_0059a856;
unsigned char g_abInputKeyState_0059a860[0x80];
int DAT_0059a8e0;
int DAT_0059a8e4;
signed char g_cScreenViewportMode_0059a9f2;
const short * volatile g_pScreenViewportGeometry_0059a9f4;
int g_anSortedObject_0059aa00[WC1_SPACE_OBJECT_COUNT];
volatile short g_nMouseX_0059ab10;
volatile short g_nMouseY_0059ab12;
unsigned char g_bPrimaryMouseButton_0059ab14;
unsigned char g_bSecondaryMouseButton_0059ab15;
unsigned char g_bInputReserved_0059ab16;
unsigned short g_wInputFlags_0059ab17;
unsigned char * volatile DAT_0059ab19;
unsigned short DAT_0059ab1d;
Viewport * volatile DAT_0059ab23;
unsigned char g_bMouseCursorShapeChanged_0059ab2b;
void (*DAT_0059ab2c)(void);
short DAT_0059ab34;
int DAT_0059ab3c;
int DAT_0059ab40;
int DAT_0059ab44;
int DAT_0059ab48;
int DAT_0059ab4c;
short g_nViewportOriginY_0059ab50;
short g_nViewportOriginX_0059ab52;
unsigned int DAT_0059ab54;
unsigned char DAT_0059ab58;
int DAT_0059ab5c;
int DAT_0059ab60;
int g_aiInputEventSlotUsed_0059ab70[0x100];
short g_nHostMouseX_0059af70;
short g_nHostMouseY_0059af72;
unsigned char g_bPreviousPrimaryInputButton_0059af74;
unsigned char g_bPreviousSecondaryInputButton_0059af75;
unsigned char g_bMenuInputReserved_0059af76;
unsigned short g_wPreviousMenuInputFlags_0059af77;
TextContext *g_pCurrentTextContext_0059af8c;
int g_nInputTickScale_0059af90;
FixedVector g_aObjectViewPosition_0059afa0[WC1_SPACE_OBJECT_COUNT];
short g_anObjectPitchRotation_0059b2a0[WC1_SPACE_OBJECT_COUNT];
int g_anShipSpeed_0059b320[64];
short g_nSpaceFrame_0059b420;
unsigned int DAT_0059b430[512];
int g_anShipFuel_0059b470[512];
short g_asObjectDistance_0059b4a0[WC1_SPACE_OBJECT_COUNT];
ShortVector g_aShipFormationOffset_0059b520[10];
enum ObjectType g_aeObjectType_0059b560[96];
short g_asObjectAnimationDelay_0059b660[WC1_SPACE_OBJECT_COUNT];
FixedVector g_aShipRightVector_0059b6e0[64];
FixedVector g_aShipUpVector_0059b9e0[64];
FixedVector g_aShipForwardVector_0059bce0[64];
HazardField *g_pActiveHazardField_0059bfe0;
signed char g_acPlayerComponentDamage_0059bff0[6];
signed char g_abFlightPath_0059c000[WC1_MISSION_OBJECTIVE_COUNT + 1];
FixedVector g_aShipVelocity_0059c010[512];
short g_anYawGoal_0059c310[16];
short g_asObjectCounter_0059c330[512];
enum SpecialManeuver g_aeSpecialManeuver_0059c3c0[WC1_SPACE_OBJECT_COUNT];
enum ShipMissionType g_aeShipMissionType_0059c3f0[512];
short g_asShipCount_0059c420[512];
short g_asShipMaximumSpeed_0059c440[16];
signed char g_acShipDamage_0059c460[10];
signed char g_cMissionObjectiveCount_0059c46a;
short g_asViableTargetDistance_0059c470[16];
FixedVector g_aShipPosition_0059c490[512];
short g_asObjectRadarRadius_0059c790[WC1_SPACE_OBJECT_COUNT];
unsigned char DAT_0059c810[512];
short g_nShipMissionIndices_0059c830[10];
FixedVector g_vStarFieldMotion_0059c860;
signed char g_cCurrentNavPointIndex_0059c86c;
short g_asObjectFlip_0059c870[WC1_SPACE_OBJECT_COUNT];
short g_nEyeRollGoal_0059c8f0;
FixedVector g_vPreviousStarFieldMotion_0059c900;
short g_nHazardFieldCount_0059c90c;
signed char DAT_0059c910[WC1_SPACE_OBJECT_COUNT];
short g_nEyeYawGoal_0059c944;
short g_asObjectScreenScale_0059c950[WC1_SPACE_OBJECT_COUNT];
short g_asObjectAfterburnerVelocity_0059c9d0[WC1_SPACE_OBJECT_COUNT];
CampaignState g_stCampaignState_0059ca50;
unsigned char g_aShipWeapons_0059cab0[16][0x47];
signed char g_acShipRating_0059cd80[16];
short g_asObjectScreenAngle_0059cd90[WC1_SPACE_OBJECT_COUNT];
short g_nTargetRange_0059ce10;
unsigned int DAT_0059ce18[256];
signed char g_acObjectOwner_0059ce20[64];
signed char g_acShipTarget_0059ce60[512];
short g_anObjectYawRotation_0059ce80[256];
signed char DAT_0059cf00[WC1_SPACE_OBJECT_COUNT];
unsigned char DAT_0059cf20[512];
short g_asObjectDrawY_0059cf80[WC1_SPACE_OBJECT_COUNT];
short g_asObjectDrawX_0059d000[WC1_SPACE_OBJECT_COUNT];
short g_asPreviousObjectDistance_0059d080[WC1_SPACE_OBJECT_COUNT];
enum ObjectClass g_aeObjectClass_0059d100[512];
enum ShipObjective g_aeShipObjective_0059d200[512];
short g_asObjectViewFrame_0059d230[WC1_SPACE_OBJECT_COUNT];
signed char g_acWingmanMessageState_0059d2c0[16];
short g_asCollisionCountdown_0059d2d0[16];
unsigned char *g_apObjectShape_0059d2f0[WC1_SPACE_OBJECT_COUNT];
short g_nPitchInput_0059d3f0;
short g_nYawInput_0059d3f2;
short g_nRollInput_0059d3f4;
short g_asShipWingLeader_0059d400[16];
short g_aasShipArmor_0059d420[10][4];
short g_asShipWeaponEnergy_0059d470[16];
signed char g_acShipIonDriveDamage_0059d4a0[16];
short g_asCannedCommand_0059d4e0[WC1_SPACE_OBJECT_COUNT];
unsigned short DAT_0059d500[16];
char g_acShipSequence_0059d520[512];
short g_nTargetFacing_0059d52a;
FixedVector g_aShipDestination_0059d530[10];
short g_aasShipShield_0059d5b0[12][2];
enum ShipTactic g_aeShipTactic_0059d5e0[512];
signed char g_abShipExhaustHeat_0059d610[10];
short g_nRenderedSpaceFrame_0059d61a;
short g_nEyePitchGoal_0059d61c;
signed char g_acShipStress_0059d620[16];
short g_anRollGoal_0059d630[16];
enum Side g_aeShipSide_0059d650[512];
signed char DAT_0059d6a0[WC1_SPACE_OBJECT_COUNT];
short g_aasShipMaximumShield_0059d6e0[12][2];
short g_asObjectCollisionRadius_0059d710[64];
signed char g_acShipPointingMode_0059d790[16];
short g_anPitchGoal_0059d7a0[16];
signed char g_abShipNavPointIndex_0059d7c0[512];
short g_anObjectRollRotation_0059d7e0[WC1_SPACE_OBJECT_COUNT];
unsigned char g_abShipTurn_0059d860[WC1_SPACE_OBJECT_COUNT];
HazardField g_aHazardFields_0059d870[7];
short g_nFacingToTarget_0059d920;
short g_asObjectScreenY_0059d930[WC1_SPACE_OBJECT_COUNT];
short g_asObjectScreenX_0059d9b0[WC1_SPACE_OBJECT_COUNT];
short g_asObjectAnimationIndex_0059da30[WC1_SPACE_OBJECT_COUNT];
signed char g_cCockpitView_0059dab0;
MissionObjective g_aMissionObjectives_0059dac5[WC1_MISSION_OBJECTIVE_COUNT + 1];
enum ShipManeuver g_aeShipManeuver_0059dcb0[512];
const short *g_apCannedSequence_0059dce0[WC1_SPACE_OBJECT_COUNT];
unsigned char g_aShipMissionSpot_0059dd10[224];
ObjectResourceSlot g_aObjectResourceSlots_0059ddf0[4] = {
    { -1, 0, 0, 0 }, { -1, 0, 0, 0 },
    { -1, 0, 0, 0 }, { -1, 0, 0, 0 }
};
signed char g_acShipDestroyedWeaponCount_0059de30[16];
short g_asObjectScale_0059de40[WC1_SPACE_OBJECT_COUNT];
char DAT_0059dec0[256];
short g_asShipAccumulatedDamage_0059dee0[WC1_SPACE_OBJECT_COUNT];
short g_nCurrentNavPoint_0059df60;
unsigned char *g_apCommPortraitShapes_0059e180[16];
char DAT_0059e1c0[512];
signed char g_abCommMenuChoiceCommand_0059e488[7];
unsigned int DAT_0059e490;
char *g_apszCommMenuChoiceText_0059e4e0[7];
int g_anInputButton2PressTime_0059e500[2];
short g_asInputButton1DoubleClick_0059e508[2];
short g_asInputButton1Changed_0059e50c[2];
short g_asInputButton2Changed_0059e510[2];
short g_asPreviousInputButtons_0059e514[2];
int g_anInputButton1PressTime_0059e518[2];
short g_asInputButton2DoubleClick_0059e520[2];
int g_aiPacketHandles_0059e530[0x1000];
int g_aiPacketHandleOffsets_005a2530[0x1000];
int g_nPacketHandleCount_005a6530;
int *DAT_005a6538;
int g_aiSoundEffectSourceActive_005a66ec[0x41];
int g_nAllocateViewportCalls_005a68ec;
short DAT_005a6900[256];
const short *g_pViewScript_005a6b58;
short g_nExternalViewDistance_005a6b5c;
Viewport DAT_005a6b60;
Viewport DAT_005a6b80;
unsigned char *g_pScreenViewportPacket_005a6b94;
Viewport DAT_005a6ba0;
TextContext DAT_005a6bc0;
Viewport DAT_005a6be0;
unsigned char *g_pTargetLockShape_005a6bf4;
unsigned char *g_apTextFonts_005a6c00[4];
FontWorkspace **g_apFontWorkspaces_005a6c10[4];
TextContext DAT_005a74f0;
Viewport DAT_005a7510;
Viewport DAT_005a7530;
Viewport DAT_005a7550;
unsigned char *g_pCockpitWeaponShape_005a7564;
char g_szDefaultTextBuffer_005a7590[0xcc];
unsigned char *g_pCockpitIndicatorShape_005a7658;
unsigned char *g_pConstellationShape_005a765c;
Viewport g_stModalSourceViewport_005a7670;
unsigned char *DAT_005a7684;
Viewport DAT_005a7690;
Viewport DAT_005a76b0;
FixedVector g_aPaletteFadeEntries_005a76d0[6];
unsigned char *g_pCockpitDamageShape_005a76f4;
TextContext DAT_005a7700;
TextContext DAT_005a7720;
TextContext g_stDefaultTextContext_005a7740;
unsigned short DAT_005a7780;
short g_nWeaponDisplayOriginX_005a7788;
short g_nWeaponDisplayOriginY_005a778a;
unsigned int DAT_005a77ec;
int g_nArcadeScore_005a7bc4;
TextContext g_stTrainSimTextContext_005a7bd0;
short g_nArcadeTimeRemaining_005a7c2c;
HighScoreEntry g_aHighScoreEntries_005a7c30[6];
int g_nArcadeWaveBonus_005a7c50;
unsigned char g_cCockpitLogicalFile_005a7c74;
unsigned char *g_apCockpitShapes_005a7c80[4];
unsigned char *g_pCinematicViewBackdrop_005a7c90;
unsigned char *g_pRearViewBackdrop_005a7c94;
short g_nPlayerKillCount_005a7c9c;
short g_asCollisionTime_005a7ca0[16];
short g_asCollisionPartner_005a7cc0[10];
int g_nMemoryConfiguration_005a7cd4;
unsigned int g_dwOriginalFreeMemory_005a7cd8;
unsigned char *g_pMouseCursorResource_005a7cdc;
int g_nAvailableGameMemory_005a7ce0;
int g_nSceneResourceBudget_005a7ce4;
unsigned char DAT_005a7cec;
unsigned char *DAT_005a7cf0;
Viewport g_stDiskPromptBackgroundViewport_005a7d00;
char g_cDiskPromptDriveLetter_005a7d21;
Viewport g_stDiskPromptViewport_005a7d40;
int g_bPointerMovedByKeyboard_005a7d54;
TextContext g_stDiskPromptTextContext_005a7d60;
unsigned int g_dwDiskPromptTopLeft_005a7d80;
unsigned int g_dwDiskPromptBottomRight_005a7d84;
int DAT_005a7d9c;
short g_nReleaseWeaponDisplayX_005a7dbc;
short g_nReleaseWeaponDisplayY_005a7dbe;
enum ObjectType g_eReleaseWeaponDisplayType_005a7dc0;
unsigned char *DAT_005a7dc4;
unsigned char g_bStickIndicatorFrame_005a7dc8;
short DAT_005a7dca;
HudMessageSlot g_aHudMessageSlots_005a7dd0[2];
signed char g_cPreviousTargetObject_005a7df2;
int g_nSavedMouseCursorY_005a7df4;
int g_nSavedMouseCursorX_005a7df8;
char g_szComponentHitMessage_005a7e00[40];
short g_nTargetLockMarkerY_005a7e28;
short g_nCarrierMissionShipIndex_005a7e2a;
unsigned char DAT_005a7e30[0x40];
unsigned char g_abCockpitLightState_005a7e70[7];
volatile short g_nCockpitExplosionX_005a7e98;
volatile short g_nCockpitExplosionY_005a7e9a;
unsigned short DAT_005a7ea0[64];
unsigned char g_abCockpitLightGoal_005a7eb8[7];
unsigned char DAT_005a7ec0;
unsigned int g_anCockpitDamageState_005a7ef0[4];
unsigned short DAT_005a7f00;
short g_nExternalViewAngle_005a7f06;
unsigned char *g_pTitleShape_005a7f08;
int g_nViewportAllocationCount_005a7f0c;
unsigned char *g_apViewportAllocations_005a7f10[128];
int g_nFreeViewportCalls_005a8110;
short DAT_005a8114;
short g_nMissionMedalScore_005a8116;
short g_nCampaignDataSet_005a8118;
short g_nActiveInputDevice_005a819c;
int g_nJoystickVerticalDeadZone_005a81a0;
int g_nJoystickHorizontalDeadZone_005a81a4;
int g_nJoystickUpScale_005a81a8;
int g_nJoystickLeftScale_005a81ac;
int g_nJoystickMaximumX_005a81b0;
int g_nJoystickMaximumY_005a81b4;
int g_nJoystickMinimumX_005a81b8;
int g_nJoystickMinimumY_005a81bc;
int g_nJoystickRawX_005a81c0;
int g_nJoystickRawY_005a81c4;
int g_nJoystickVerticalRange_005a81c8;
int g_nJoystickHorizontalRange_005a81cc;
int g_nJoystickRightScale_005a81d0;
int g_nJoystickDownScale_005a81d4;
int g_nJoystickCentreY_005a81d8;
int g_nJoystickCentreX_005a81dc;
int g_nJoystickFailureValue_005a81e0;
InputDeviceSample g_aInputDeviceSamples_005a81f0[4];
unsigned char g_abMissionAuxData_005a8218[0x28];
unsigned char g_abSeriesAuxData_005a8240[0x28];
MissionObjectiveSource g_aMissionObjectiveSources_005a8270[16];
short g_nMissionEntryNavPoint_005a8690;
short DAT_005a8692;
short g_nPlayerMissionShipIndex_005a8694;
short g_nInitialMissionShipIndices_005a8696[8];
short DAT_005a86a6;
CampaignDate *g_pCurrentCampaignDate_005a86a8;
CampaignDate *g_pElapsedCampaignDate_005a86ac;
unsigned char DAT_005a86b0;
char DAT_005a8760[512];
unsigned char *g_pIntroFont_005a8960;
unsigned int DAT_005a8964;
JOYINFO g_aJoystickInfo_005a8970[2];
int g_nHostMouseMessageX_005a8990;
int g_nHostMouseMessageY_005a8994;
int g_bHostPrimaryMouseButton_005a8998;
int g_bHostSecondaryMouseButton_005a899c;
HWND DAT_005a89a0;
HANDLE DAT_005a89a4;
unsigned long g_dwGameClockBase_005a89a8;
GuardedAllocation *g_pGuardedAllocationTail_005a89ac;
char g_szMemoryUsage_005a89b0[0x80];
HDC DAT_005a8a30;
HCURSOR DAT_005a8a34;
unsigned int DAT_005a8a38;
int DAT_005a8a3c;
HINSTANCE DAT_005a8a40;
unsigned int DAT_005a8a44;
unsigned short DAT_005a8a50[0x300];

/* Embedded animation command streams at 0x00466030-0x00466400.  Each
 * command occupies the low word of a four-byte source record. */
static unsigned int g_anAnimExplosion0_00466030[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa000, 0
};
static unsigned int g_anAnimExplosion1_00466060[] = {
    0, 0x406, 1, 0x406, 2, 0x406, 0x406, 3, 0x406, 0x406,
    4, 0x406, 0x406, 0x406, 5, 0x406, 0x406, 0x406, 0x406,
    0x406, 0xa000, 0
};
static unsigned int g_anAnimExplosion2_004660b8[] = {
    0, 0x406, 1, 0x406, 2, 0x406, 3, 0x406, 4, 0x406, 5,
    6, 7, 0xa000
};
static unsigned int g_anAnimLaserSpark_004660f0[] = {
    0, 1, 2, 3, 4, 5, 0xa000, 0
};
static unsigned int g_anAnimBlueSpark_00466110[] = {
    0, 1, 2, 3, 0xa000, 0
};
static unsigned int g_anAnimRedSpark_00466128[] = {
    0, 1, 2, 3, 0xa000, 0
};
static unsigned int g_anAnimSparkTrail_00466140[] = {
    0, 1, 2, 3, 0xa000, 0
};
static unsigned int g_anAnimGirder_00466158[] = {
    0, 1, 2, 3, 4, 5, 0x9000, 0
};
static unsigned int g_anAnimTubing_00466178[] = {
    6, 7, 8, 9, 10, 11, 0x9000, 0
};
static unsigned int g_anAnimGlass_00466198[] = {
    12, 13, 14, 15, 16, 17, 18, 19, 0x92, 0x91, 0x90,
    0x8f, 0x8e, 0x8d, 0x8c, 20, 0x9000, 0
};
static unsigned int g_anAnimORing_004661e0[] = {
    21, 22, 23, 24, 25, 26, 0x9000, 0
};
static unsigned int g_anAnimPipe_00466200[] = {
    27, 28, 29, 30, 31, 32, 0x9000, 0
};
static unsigned int g_anAnimMetalSheet_00466220[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0x9000
};
static unsigned int g_anAnimWing_00466258[] = {
    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
    27, 28, 0x9000, 0
};
static unsigned int g_anAnimMine_004662a0[] = {
    0, 1, 2, 0x41, 0x9000, 0, 0, 1, 2, 3, 4, 5, 0x9000, 0
};
static unsigned int g_anAnimAsteroidForward_004662d8[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 0x9000, 0
};
static unsigned int g_anAnimAsteroidShortForward_00466318[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0x9000
};
static unsigned int g_anAnimAsteroidReverse_00466350[] = {
    13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0x9000, 0
};
static unsigned int g_anAnimAsteroidShortReverse_00466390[] = {
    12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0x9000
};
static unsigned int g_anAnimEjectedPilot_004663c8[14] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0x9000
};

/* Child/hardpoint offsets at 0x004682F0. */
const ShortVector g_aChildOffsets_004682f0[35] = {
    { 120, 10, 20 }, { -120, 10, 20 }, { 75, 45, -30 },
    { -75, 45, -30 }, { 0, 50, 10 }, { -100, 10, -40 },
    { -90, 0, 30 }, { -30, -40, -30 }, { 0, 0, 0 },
    { 30, -40, -30 }, { 100, 10, -40 }, { 90, 0, 30 },
    { -140, 10, 30 }, { -100, 10, 0 }, { -75, 0, -40 },
    { -30, 10, -20 }, { 0, 10, 10 }, { 30, 10, -20 },
    { 75, 0, -40 }, { 100, 10, 0 }, { 140, 10, 30 },
    { -120, -10, 0 }, { -100, 10, -20 }, { -90, 0, 40 },
    { -30, 20, -20 }, { 0, 10, -80 }, { 0, 10, 10 },
    { 30, 20, -20 }, { 90, 0, 40 }, { 100, 10, -20 },
    { 120, -10, 0 }, { 0, 10, 10 }, { 0, 0, -60 },
    { 0, 0, 500 }, { -200, 0, 250 }
};

const ShortPoint g_aWeaponDisplayPositions_00468440[10] = {
    {64, 28}, {8, 28}, {45, 25}, {27, 25}, {36, 23},
    {20, 32}, {27, 24}, {31, 31}, {36, 22}, {41, 31}
};

const ShortPoint g_aWeaponDisplayOrigins_004684c0[5] = {
    {0, 16}, {0, 16}, {0, 16}, {0, 16}, {0, 16}
};

ObjectTypeData g_aObjectTypeData_00466458[OBJECT_TYPE_COUNT] = {
    /* 0: Hornet */
    {
        &g_aszObjectTypeDisplayNames_004684d4[0x00],
        OBJECT_CLASS_SHIP, 100, 125, 1024, 5, 3392, 3, 5, 4000,
        42, 30, 0, 819, 8, 9, 8, 900,
        {
            5,
            24, 0, 0, 0, 0, 0, 0,
            24, 0, 0, 0, 1, 0, 0,
            28, 0, 0, 0, 2, 0, 0,
            28, 0, 0, 0, 3, 0, 1,
            29, 0, 0, 0, 4, 0, 1
        },
        40, 40, 45, 40, 30, 30, 0, 0
    },
    /* 1: Rapier */
    {
        &g_aszObjectTypeDisplayNames_004684d4[0x08],
        OBJECT_CLASS_SHIP, 120, 135, 1024, 3, -12144, 3, 6, 6000,
        45, 25, 0, 1075, 10, 10, 10, 1000,
        {
            9,
            24, 0, 0, 0, 14, 0, 1,
            24, 0, 0, 0, 18, 0, 1,
            25, 0, 0, 0, 12, 0, 0,
            25, 0, 0, 0, 20, 0, 0,
            31, 0, 0, 0, 16, 0, 1,
            30, 0, 0, 0, 15, 0, 0,
            30, 0, 0, 0, 17, 0, 1,
            28, 0, 0, 0, 13, 0, 1,
            28, 0, 0, 0, 19, 0, 1
        },
        80, 75, 60, 55, 50, 50, 0, 0
    },
    /* 2: Scimitar */
    {
        &g_aszObjectTypeDisplayNames_004684d4[0x10],
        OBJECT_CLASS_SHIP, 165, 160, 1152, 6, 17856, 4, 7, 6000,
        36, 15, 0, 614, 6, 6, 7, 1300,
        {
            7,
            26, 0, 0, 0, 6, 0, 0,
            26, 0, 0, 0, 11, 0, 0,
            28, 0, 0, 0, 5, 0, 0,
            28, 0, 0, 0, 10, 0, 1,
            29, 0, 0, 0, 7, 0, 1,
            29, 0, 0, 0, 8, 0, 1,
            29, 0, 0, 0, 9, 0, 1
        },
        60, 50, 85, 80, 65, 65, 0, 0
    },
    { &g_aszObjectTypeDisplayNames_004684d4[0x1c] }, /* 3: Raptor */
    { &g_aszObjectTypeDisplayNames_004684d4[0x24] }, /* 4: Venture */
    { &g_aszObjectTypeDisplayNames_004684d4[0x2c] }, /* 5: Dilligent */
    { &g_aszObjectTypeDisplayNames_004684d4[0x38] }, /* 6: Drayman */
    { &g_aszObjectTypeDisplayNames_004684d4[0x40] }, /* 7: Exeter */
    { &g_aszObjectTypeDisplayNames_004684d4[0x48] }, /* 8: Tiger's Claw */
    { &g_aszObjectTypeDisplayNames_004684d4[0x58] }, /* 9: Salthi */
    /* 10: Dralthi */
    {
        &g_aszObjectTypeDisplayNames_004684d4[0x60],
        OBJECT_CLASS_SHIP, 160, 140, 1024, 6, 3392, 3, 7, 6000,
        40, 23, 0, 768, 10, 14, 10, 1200,
        {
            7,
            24, 0, 0, 0, 0, 0, 0,
            24, 0, 0, 0, 1, 0, 0,
            33, 0, 0, 0, 32, 0, 0,
            33, 0, 0, 0, 32, 0, 1,
            33, 0, 0, 0, 32, 0, 1,
            29, 0, 0, 0, 31, 0, 0,
            29, 0, 0, 0, 31, 0, 1
        },
        50, 50, 45, 35, 30, 30, 0, 0
    },
    /* 11: Krant */
    {
        &g_aszObjectTypeDisplayNames_004684d4[0x68],
        OBJECT_CLASS_SHIP, 140, 126, 1024, 5, 3392, 3, 6, 6000,
        36, 20, 0, 716, 7, 10, 7, 1200,
        {
            6,
            24, 0, 0, 0, 0, 0, 0,
            24, 0, 0, 0, 1, 0, 0,
            30, 0, 0, 0, 31, 0, 1,
            29, 0, 0, 0, 31, 0, 0,
            29, 0, 0, 0, 31, 0, 1,
            29, 0, 0, 0, 31, 0, 1
        },
        80, 80, 90, 100, 80, 80, 0, 0
    },
    /* 12: Gratha */
    {
        &g_aszObjectTypeDisplayNames_004684d4[0x70],
        OBJECT_CLASS_SHIP, 140, 126, 1024, 4, 3392, 3, 7, 7000,
        32, 20, 0, 614, 6, 6, 14, 1400,
        {
            10,
            24, 0, 0, 0, 0, 0, 0,
            24, 0, 0, 0, 1, 0, 0,
            26, 0, 0, 0, 21, 0, 0,
            26, 0, 0, 0, 30, 0, 0,
            31, 0, 0, 0, 31, 0, 1,
            29, 0, 0, 0, 31, 0, 0,
            29, 0, 0, 0, 31, 0, 1,
            33, 0, 0, 0, 32, 0, 1,
            33, 0, 0, 0, 32, 0, 1,
            33, 0, 0, 0, 32, 0, 1
        },
        100, 95, 140, 120, 100, 100, 0, 0
    },
    { &g_aszObjectTypeDisplayNames_004684d4[0x78] }, /* 13: Jalthi */
    { &g_aszObjectTypeDisplayNames_004684d4[0x80] }, /* 14: Spikeri */
    { &g_aszObjectTypeDisplayNames_004684d4[0x88] }, /* 15: Dorkir */
    { &g_aszObjectTypeDisplayNames_004684d4[0x90] }, /* 16: Lumbari */
    { &g_aszObjectTypeDisplayNames_004684d4[0x98] }, /* 17: Ralari */
    { &g_aszObjectTypeDisplayNames_004684d4[0xa0] }, /* 18: Fralthi */
    { &g_aszObjectTypeDisplayNames_004684d4[0xa8] }, /* 19: Snakeir */
    { &g_aszObjectTypeDisplayNames_004684d4[0xb0] }, /* 20: Sivar */
    { &g_aszObjectTypeDisplayNames_004684d4[0xb8] }, /* 21: Kilrathi base */
    { 0 }, /* 22: asteroid field */
    { 0 }, /* 23: mine field */
    /* 24: laser cannon bolt */
    {
        &g_aszObjectTypeDisplayNames_004684d4[0xc4],
        OBJECT_CLASS_PROJECTILE, 10, 0, 512, 7, 30, 0, 25, 0,
        160, 0, 0, 0, 0, 0, 0, 0, { 0 },
        0, 0, 0, 0, 0, 0, 0, 0
    },
    { &g_aszObjectTypeDisplayNames_004684d4[0xd4],
      OBJECT_CLASS_PROJECTILE, 10, 1, 832, 14, 20, 0, 40, 0,
      140, 0 }, /* 25: neutron particle gun */
    { &g_aszObjectTypeDisplayNames_004684d4[0xe0],
      OBJECT_CLASS_PROJECTILE, 10, 1, 768, 14, 25, 0, 30, 0,
      120, 0 }, /* 26: mass driver cannon */
    { &g_aszObjectTypeDisplayNames_004684d4[0xec],
      OBJECT_CLASS_PROJECTILE, 10, 0, 512, 7, 30, 0, 25, 0,
      160, 0 }, /* 27: turret bolt; resources alias laser */
    { &g_aszObjectTypeDisplayNames_004684d4[0xf0],
      OBJECT_CLASS_MISSILE, 10, 10, 512, 0, 180, 20, 20, 0,
      80, 60 }, /* 28 */
    { &g_aszObjectTypeDisplayNames_004684d4[0xf8],
      OBJECT_CLASS_MISSILE, 10, 10, 512, 0, 180, 20, 20, 0,
      80, 60 }, /* 29 */
    { &g_aszObjectTypeDisplayNames_004684d4[0x104],
      OBJECT_CLASS_MISSILE, 10, 10, 512, 0, 180, 20, 20, 0,
      80, 60 }, /* 30 */
    { &g_aszObjectTypeDisplayNames_004684d4[0x110],
      OBJECT_CLASS_MISSILE, 10, 10, 512, 0, 180, 20, 20, 0,
      80, 60 }, /* 31 */
    { &g_aszObjectTypeDisplayNames_004684d4[0x11c],
      OBJECT_CLASS_MISSILE, 20, 20, 768, 0, 240, 40, 40, 0,
      60, 40 }, /* 32 */
    { &g_aszObjectTypeDisplayNames_004684d4[0x120],
      OBJECT_CLASS_MINE, 20, 5, 768, 110, 120, 4, 100, 10000,
      20, 20, (unsigned char *)g_anAnimMine_004662a0,
      0, 0, 2, 2 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x12c],
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimAsteroidForward_004662d8,
      0, 0, 1, 13 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x130],
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimAsteroidShortForward_00466318,
      0, 0, 1, 12 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x134],
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimAsteroidReverse_00466350,
      0, 0, 1, 13 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x138],
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimAsteroidShortReverse_00466390,
      0, 0, 1, 12 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x13c],
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimAsteroidForward_004662d8,
      0, 0, 2, 13 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x140],
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimAsteroidShortForward_00466318,
      0, 0, 2, 12 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x144],
      OBJECT_CLASS_DEBRIS, 10, 4, 192, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimAsteroidForward_004662d8,
      0, 0, 2, 13 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x148],
      OBJECT_CLASS_DEBRIS, 10, 1, 2048, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimGirder_00466158,
      0, 0, 2, 5 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x14c],
      OBJECT_CLASS_DEBRIS, 10, 1, 2048, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimTubing_00466178,
      0, 0, 2, 5 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x150],
      OBJECT_CLASS_DEBRIS, 20, 2, 1280, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimMetalSheet_00466220,
      0, 0, 1, 11 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x154],
      OBJECT_CLASS_DEBRIS, 20, 2, 1280, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimWing_00466258,
      0, 0, 1, 15 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x158],
      OBJECT_CLASS_DEBRIS, 20, 2, 768, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimGlass_00466198,
      0, 0, 1, 15 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x15c],
      OBJECT_CLASS_DEBRIS, 2, 1, 1792, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimORing_004661e0,
      0, 0, 1, 5 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x160],
      OBJECT_CLASS_DEBRIS, 6, 1, 1536, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimPipe_00466200,
      0, 0, 1, 5 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x164],
      OBJECT_CLASS_EXPLOSION, 0, 0, 768, 0, 0, 0, -1, 6000,
      0, 0, (unsigned char *)g_anAnimExplosion0_00466030,
      0, 0, 1, 0 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x168],
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, 0, -1, 6000,
      0, 0, (unsigned char *)g_anAnimExplosion1_00466060,
      0, 0, 1, 0 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x16c],
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, 0, -1, 6000,
      0, 0, (unsigned char *)g_anAnimExplosion2_004660b8,
      0, 0, 1, 0 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x170],
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimLaserSpark_004660f0,
      0, 0, 1, 0 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x174],
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimRedSpark_00466128,
      0, 0, 2, 3 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x178],
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimBlueSpark_00466110,
      0, 0, 2, 3 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x17c],
      OBJECT_CLASS_EXPLOSION, 1, 1, 256, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimSparkTrail_00466140,
      0, 0, 2, 3 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x180],
      OBJECT_CLASS_FIXED_OBJECT, 0, 0, 256, 0, 0, 0, -1 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x184],
      OBJECT_CLASS_DEBRIS, 6, 1, 512, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimEjectedPilot_004663c8,
      0, 0, 1, 12 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x188],
      OBJECT_CLASS_EXPLOSION, 0, 0, 1024, 0, 0, 0, -1, 0,
      0, 0, 0, 0, 0, 1, 0 }
};
unsigned char g_bMessageSpeed_0046af68 = 2;
unsigned char g_abManeuverRerollChance_00465678[47] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 3, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0
};
void (*g_apShipAiManeuverHandlers_004656a8[47])(short, short) = {
    /* 00 */ (void (*)(short, short))Mnone,
    /* 01 */ (void (*)(short, short))Mnone,
    /* 02 */ (void (*)(short, short))Mveer_away,
    /* 03 */ (void (*)(short, short))Mreset,
    /* 04 */ (void (*)(short, short))Mnone,
    /* 05 */ (void (*)(short, short))Mnone,
    /* 06 */ (void (*)(short, short))Mnone,
    /* 07 */ (void (*)(short, short))Mnone,
    /* 08 */ (void (*)(short, short))Mnone,
    /* 09 */ (void (*)(short, short))Mnone,
    /* 10 */ (void (*)(short, short))Mnone,
    /* 11 */ (void (*)(short, short))Mnone,
    /* 12 */ (void (*)(short, short))Mnone,
    /* 13 */ (void (*)(short, short))Mnone,
    /* 14 */ (void (*)(short, short))Mnone,
    /* 15 */ (void (*)(short, short))Mnone,
    /* 16 */ (void (*)(short, short))Mnone,
    /* 17 */ (void (*)(short, short))Mnone,
    /* 18 */ (void (*)(short, short))Mnone,
    /* 19 */ (void (*)(short, short))Mnone,
    /* 20 */ (void (*)(short, short))Mnone,
    /* 21 */ (void (*)(short, short))Mrout_me,
    /* 22 */ (void (*)(short, short))Mnone,
    /* 23 */ (void (*)(short, short))Mnone,
    /* 24 */ (void (*)(short, short))Mzig_zag,
    /* 25 */ (void (*)(short, short))Mgloat,
    /* 26 */ (void (*)(short, short))Mtail_fire,
    /* 27 */ (void (*)(short, short))Mtarget_laser,
    /* 28 */ (void (*)(short, short))Mnone,
    /* 29 */ (void (*)(short, short))Mstrafe_enemy,
    /* 30 */ (void (*)(short, short))Mbest_strafe,
    /* 31 */ (void (*)(short, short))Mstrafe_n_roll,
    /* 32 */ (void (*)(short, short))Mnone,
    /* 33 */ (void (*)(short, short))Mnone,
    /* 34 */ (void (*)(short, short))Mzig_zag_pitch,
    /* 35 */ (void (*)(short, short))Mnone,
    /* 36 */ (void (*)(short, short))Mbest_strafe,
    /* 37 */ (void (*)(short, short))Mnone,
    /* 38 */ (void (*)(short, short))Mnone,
    /* 39 */ (void (*)(short, short))Mnone,
    /* 40 */ (void (*)(short, short))Mzip_past,
    /* 41 */ (void (*)(short, short))Mnone,
    /* 42 */ (void (*)(short, short))Mline_up_drop,
    /* 43 */ (void (*)(short, short))Mnone,
    /* 44 */ (void (*)(short, short))ShipAiState44,
    /* 45 */ (void (*)(short, short))Mbest_strafe,
    /* 46 */ (void (*)(short, short))Mreset
};

char *g_apszCommMenuText_0046af90[13] = {
    (char *)&g_aszCommMenuText_0046aff8[0x00],
    (char *)&g_aszCommMenuText_0046aff8[0x10],
    (char *)&g_aszCommMenuText_0046aff8[0x24],
    (char *)&g_aszCommMenuText_0046aff8[0x38],
    (char *)&g_aszCommMenuText_0046aff8[0x48],
    (char *)&g_aszCommMenuText_0046aff8[0x58],
    (char *)&g_aszCommMenuText_0046aff8[0x64],
    (char *)&g_aszCommMenuText_0046aff8[0x78],
    (char *)&g_aszCommMenuText_0046aff8[0x8c],
    (char *)&g_aszCommMenuText_0046aff8[0x9c],
    (char *)&g_aszCommMenuText_0046aff8[0xb0],
    (char *)&g_aszCommMenuText_0046aff8[0xc4],
    (char *)&g_aszCommMenuText_0046aff8[0xd8]
};

/* Ordered display names transcribed from the image.  The Sega CD inspector
 * proves that these index ObjectType, not a ship-only class enum. */
const char g_aszObjectTypeDisplayNames_004684d4[0x18c] =
    "Hornet\0\0"
    "Rapier\0\0"
    "Scimitar\0\0\0\0"
    "Raptor\0\0"
    "Venture\0"
    "Dilligent\0\0\0"
    "Drayman\0"
    "Exeter\0\0"
    "Tiger's Claw\0\0\0\0"
    "Salthi\0\0"
    "Dralthi\0"
    "Krant\0\0\0"
    "Gratha\0\0"
    "Jalthi\0\0"
    "Spikeri\0"
    "Dorkir\0\0"
    "Lumbari\0"
    "Ralari\0\0"
    "Fralthi\0"
    "Snakeir\0"
    "Sivar\0\0\0"
    "Star post\0\0\0"
    "Laser cannon\0\0\0\0"
    "Neutron gun\0"
    "Mass driver\0\0\0\0\0"
    "Dart DF\0"
    "Javelin HS\0\0"
    "Pilum FF\0\0\0\0"
    "Spiculum IR\0\0\0\0\0"
    "Porcupine\0\0\0";

const char g_aszKilrathiAceNames_0046afd4[32] =
    "Bhurak\0\0"
    "Dakhath\0"
    "Khajja\0\0"
    "Bakhtosh";

const char g_aszCommMenuText_0046aff8[0xe8] =
    "Never mind...\0\0\0"
    "Attack my target!\0\0\0"
    "Help me out here\0\0\0\0"
    "Return to base.\0"
    "Die furball!\0\0\0\0"
    "Slag off!\0\0\0"
    "Bite it cat face.\0\0\0"
    "Break and attack.\0\0\0"
    "Keep formation!\0"
    "Form on my wing.\0\0\0\0"
    "Keep radio silence\0\0"
    "Broadcast freely\0\0\0\0"
    "Request Landing";

const char g_szMissileLocked_004692a8[16] = "MISSILE LOCKED ";

const char g_szIonDrive_0046a7c4[12] = "Ion drive";
const char g_szPowerPlant_0046a7d0[12] = "Power plant";
const char g_szShieldGenerator_0046a7dc[16] = "Shield gen'r";
const char g_szComputerSystem_0046a7ec[16] = "Computer sys";
const char g_szIntercomUnit_0046a7fc[16] = "InterCom unit";
const char g_szTargetTracking_0046a80c[16] = "Target track";
