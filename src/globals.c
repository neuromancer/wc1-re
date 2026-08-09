/*
 *  Definitions for the globals declared in include/globals.h.
 *
 *  Provisional: the original scatters these across the modules that own them,
 *  so this file has no counterpart in the shipped image and contributes no
 *  code.  It exists so that regrouping functions cannot break the link.
 */
#include "wc1.h"

unsigned int DAT_00465040;
short g_nActiveSoundEffectShips_00465044;
int DAT_00465058 = 1;
unsigned int DAT_0046505c;
int DAT_0046506c;
unsigned int g_nGuardedAllocationTotalBytes_00465060;
unsigned int g_nGuardedAllocationBytes_00465064;
unsigned int g_nGuardedAllocationPeakBytes_00465068;
int DAT_00465070;
int DAT_00465074 = 1;
int DAT_0046507c = 1;
int DAT_00465080;
int DAT_004650a8;
unsigned int DAT_004650ac;
GuardedAllocation *g_pGuardedAllocationHead_004650b0;
short DAT_00465460;
int g_aiPacketReferenceTable_00465c88[4 * 0x25];
unsigned short DAT_00468660;
unsigned int DAT_00468664;
unsigned char DAT_0046870c;
unsigned char DAT_00468710;
char *DAT_00469004;
char *DAT_00469008;
unsigned char DAT_0046900c;
unsigned char DAT_00469060;
unsigned char DAT_00469068;
unsigned short DAT_00469090;
short DAT_00469208;
unsigned char DAT_00469648;
unsigned char DAT_0046964c;
unsigned char DAT_0046999c;
unsigned short DAT_004699a8;
unsigned short DAT_004699b0;
unsigned int DAT_004699d8;
unsigned char DAT_00469d5c;
unsigned char *DAT_00469dbc[64];
unsigned int DAT_00469e34;
unsigned char DAT_00469fb4;
short g_nFrameSkip_00469fb8 = 1;
short g_nOriginDevUnlock_00469ff4;
int DAT_00469ffc = 1;
int DAT_0046a000 = 1;
unsigned char DAT_0046a004;
int *DAT_0046a438;
unsigned char DAT_0046a440;
WaveTableEntry *g_pWaveTableHead_0046a444;
WaveTableEntry *g_pWaveTableTail_0046a448;
short DAT_0046a9f8;
short DAT_0046aa04;
unsigned int DAT_0046aa14;
unsigned char DAT_0046aa30;
unsigned char DAT_0046aa34;
unsigned char DAT_0046aa38;
unsigned char DAT_0046aa3c;
unsigned char DAT_0046aa40;
unsigned char g_abTaggedAllocationPrefix_0046ad88[8] = {
    'j', 'e', 'f', 'f', 0, 0, 0, 0
};
short g_nCommMenuChoiceCount_0046af60;
short g_nCommMenuReuseMode_0046af64;
unsigned char DAT_0046af6c;
unsigned char DAT_0046afc4;
unsigned short DAT_0046b168;
const GUID g_guidDirectDraw2_00463118 = {
    0xb3a6f3e0, 0x2b43, 0x11cf,
    { 0xa2, 0xde, 0x00, 0xaa, 0x00, 0xb9, 0x33, 0x56 }
};
signed char g_abSoundEffectShips_0046c028[0x14];
short DAT_0046c010;
unsigned char DAT_0046c03c;
short g_nYourWingman_0046c04c;
unsigned char DAT_0046c060;
short DAT_0046c064;
short DAT_0046c068;
int g_bEngageAllowed_0046c080;
short g_nAutoEngageTimer_0046c084;
MissionNavPoint g_aMissionNavPoints_0046c2f0[WC1_MISSION_NAV_POINT_COUNT];
int DAT_0046da90;
int DAT_0046da94;
int g_aiIntelligenceEvent_0046d368[512];
signed char g_acPilotAggression_0046d9a0[64];
signed char g_acPilotRecovery_0046d9b8[64];
unsigned char *g_pDrawnMouseCursorShape_0046da9c;
int DAT_0046daa0;
int g_bMonoDebugInstalled_00475e70;
HANDLE g_hMonoDebugDevice_00475e74;
unsigned short DAT_00475e78;
unsigned char g_bCurrentManeuverReroll_00475e7c;
unsigned int DAT_004763f0;
unsigned int DAT_00476640;
unsigned int DAT_00476644;
unsigned char *DAT_00476648;
unsigned int DAT_0047664c;
unsigned int DAT_00476650;
unsigned char DAT_00476658[0xfa00];
unsigned char DAT_00476620[32];
int DAT_00486058;
int DAT_00486060;
HWND DAT_00486074;
unsigned char DAT_00486110[1024];
float DAT_00486510;
int DAT_00486518;
unsigned char DAT_004865a8[0x1000];
int DAT_004875a8[5];
unsigned int DAT_00496fc0[5];
unsigned char DAT_005988de[8192];
int DAT_00598a30[512];
unsigned char DAT_00598ab0;
unsigned int DAT_00598ab6;
unsigned char DAT_00598aba;
unsigned int DAT_00598af4;
unsigned int DAT_00598c18;
unsigned char g_aInputEventSlots_00598c40[0x1c00];
int g_nMouseCursorSavedY_0059a840;
int g_nMouseCursorSavedX_0059a844;
int DAT_0059a84c;
int DAT_0059a8e0;
int DAT_0059a8e4;
unsigned char DAT_0059a850;
unsigned char * volatile DAT_0059ab19;
unsigned short DAT_0059ab1d;
Viewport * volatile DAT_0059ab23;
unsigned char g_bMouseCursorShapeChanged_0059ab2b;
void (*DAT_0059ab2c)(void);
unsigned int DAT_0059ab54;
int DAT_0059ab3c;
int DAT_0059ab40;
int DAT_0059ab44;
int DAT_0059ab48;
int DAT_0059ab5c;
int DAT_0059ab60;
int g_aiInputEventSlotUsed_0059ab70[0x100];
unsigned int DAT_0059af8c;
short DAT_0059ab34;
int DAT_0059ab4c;
unsigned char DAT_0059ab58;
int g_anShipSpeed_0059b320[64];
FixedVector g_aShipForwardVector_0059bce0[64];
int g_nSoundEffectSlotCount_0059bfe0;
unsigned int DAT_0059b430[512];
int DAT_0059b470[512];
enum ObjectType g_aeObjectType_0059b560[96];
FixedVector g_aShipVelocity_0059c010[512];
unsigned char DAT_0059c310[512];
short g_asObjectCounter_0059c330[512];
enum SpecialManeuver g_aeSpecialManeuver_0059c3c0[12];
enum ShipMissionType g_aeShipMissionType_0059c3f0[512];
short g_asShipCount_0059c420[512];
short g_asShipMaximumSpeed_0059c440[24];
FixedVector g_aShipPosition_0059c490[512];
unsigned char DAT_0059c810[512];
signed char g_cCurrentNavPointIndex_0059c86c;
unsigned char DAT_0059ca94[256];
signed char g_acShipRating_0059cd80[16];
short g_nTargetRange_0059ce10;
unsigned int DAT_0059ce18[256];
signed char g_acShipTarget_0059ce60[512];
unsigned char DAT_0059ce80[512];
unsigned char DAT_0059cf20[512];
enum ObjectClass g_aeObjectClass_0059d100[512];
enum ShipObjective g_aeShipObjective_0059d200[512];
unsigned short DAT_0059d2d0[512];
short g_nPitchInput_0059d3f0;
short g_nYawInput_0059d3f2;
short g_asShipWingLeader_0059d400[64];
unsigned short DAT_0059d500[2048];
char g_acShipSequence_0059d520[512];
short g_nTargetFacing_0059d52a;
enum ShipTactic g_aeShipTactic_0059d5e0[512];
unsigned char DAT_0059d620[512];
unsigned char DAT_0059d630[512];
enum Side g_aeShipSide_0059d650[512];
short DAT_0059d710[512];
unsigned char DAT_0059d7a0[512];
signed char g_abShipNavPointIndex_0059d7c0[512];
unsigned short DAT_0059d9b0[512];
unsigned char DAT_0059daca[8192];
unsigned char DAT_0059dacf[8192];
enum ShipManeuver g_aeShipManeuver_0059dcb0[512];
unsigned char g_aShipMissionSpot_0059dd10[8192];
char DAT_0059dec0[256];
short g_nCurrentNavPoint_0059df60;
unsigned char DAT_0059e1c0[512];
signed char g_abCommMenuChoiceCommand_0059e488[7];
unsigned int DAT_0059e490;
char *g_apszCommMenuChoiceText_0059e4e0[7];
int g_aiPacketHandles_0059e530[0x1000];
int g_aiPacketHandleOffsets_005a2530[0x1000];
int g_nPacketHandleCount_005a6530;
int g_aiSoundEffectShipActive_005a66f0[0x40];
int *DAT_005a6538;
int g_nAllocateViewportCalls_005a68ec;
unsigned short DAT_005a6900[256];
Viewport DAT_005a6b60;
Viewport DAT_005a6ba0;
Viewport DAT_005a7510;
Viewport DAT_005a7550;
unsigned char DAT_005a7684;
Viewport DAT_005a76b0;
unsigned short DAT_005a7780;
unsigned int DAT_005a77ec;
unsigned int DAT_005a7c2c;
unsigned char DAT_005a7c30[2048];
unsigned char DAT_005a7c31[2048];
unsigned short DAT_005a7cc0[16];
unsigned char DAT_005a7cec;
unsigned char g_bStickIndicatorFrame_005a7dc8;
unsigned char DAT_005a7dca;
unsigned char DAT_005a7dd0[8192];
int DAT_005a7de1;
unsigned char DAT_005a7e30[2048];
unsigned short DAT_005a7ea0[64];
unsigned char DAT_005a7ec0;
int g_nViewportAllocationCount_005a7f0c;
unsigned char *g_apViewportAllocations_005a7f10[128];
int g_nFreeViewportCalls_005a8110;
int DAT_005a7d9c;
unsigned char *DAT_005a7cf0;
unsigned char DAT_005a86b0;
char DAT_005a8760[512];
short DAT_005a8692;
unsigned int DAT_005a8964;
unsigned short DAT_005a897c;
unsigned int DAT_005a898c;
GuardedAllocation *g_pGuardedAllocationTail_005a89ac;
char g_szMemoryUsage_005a89b0[0x80];
HWND DAT_005a89a0;
HANDLE DAT_005a89a4;
HDC DAT_005a8a30;
HCURSOR DAT_005a8a34;
unsigned int DAT_005a8a38;
int DAT_005a8a3c;
HINSTANCE DAT_005a8a40;
unsigned int DAT_005a8a44;
unsigned short DAT_005a8a50[0x300];
short DAT_0059ab10;
unsigned char DAT_005a7ddc;
unsigned char DAT_005a7ddd;
unsigned char DAT_005a7ded;
unsigned char DAT_005a7dee;
unsigned short DAT_005a7e70;
unsigned short DAT_005a7eb8;
unsigned short DAT_005a7ebc;
unsigned short DAT_005a7ebe;
unsigned int DAT_005a7ef0;
unsigned int DAT_005a7efc;
short DAT_0059af70;
int DAT_0046b1b8;
int DAT_0046b1bc;
int DAT_0046b1c4;
int DAT_0046b1c8;
extern unsigned int g_dwStreamerState_00597cd0;
short DAT_0059a856;
unsigned char DAT_0059ca69;
unsigned char DAT_0059ca6a;
unsigned char DAT_004700c9;
unsigned char DAT_004700ca;
int DAT_004688e0;
int DAT_00469fbc = 0x14;
int DAT_00469fc0 = 0x14;
int DAT_00469fc8[16] = {
    0, 40000, 50000, 55000, 60000, 61000,
    61500, 62000, 63000, 63500, 64000
};
char g_szStreamsPath_00475c18[0x100];
short DAT_0059ab12;
short DAT_0059af72;
unsigned short DAT_005a7e74;
unsigned short DAT_005a7e76;
unsigned int DAT_005a7ef4;
unsigned int DAT_005a7ef8;
unsigned char DAT_00466472[65536];
int  *DAT_0046b1a8;
int  *DAT_0046b1ac;
int  *DAT_0046b1b0;
int   DAT_0046b1b4;
unsigned char DAT_0046c97a[16384];
char  DAT_00486078[256];
unsigned char g_bMessageSpeed_0046af68;
unsigned char g_abManeuverRerollChance_00465678[47] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 3, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0
};
void (*g_apShipAiManeuverHandlers_004656a8[47])(short, short) = {
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp,
    (void (*)(short, short))ShipAiStateNoOp
};

/* Defined here because the pre-refactor tranche that owned them is gone. */
int *DAT_0046b1a4;
char *PTR_s_MISSILE_LOCKED_004691d4[8];
char *g_apszCommMenuText_0046af90[32];
unsigned long g_dwGameClockBase_005a89a8;

/* Ordered display names transcribed from the image.  The Sega CD inspector
 * proves that these index ObjectType, not a ship-only class enum. */
const char g_aszObjectTypeDisplayNames_004684d4[] =
    "Hornet\0" "Rapier\0" "Scimitar\0" "Raptor\0" "Venture\0" "Dilligent\0"
    "Drayman\0" "Exeter\0" "Tiger's Claw\0" "Salthi\0" "Dralthi\0" "Krant\0"
    "Gratha\0" "Jalthi\0" "Spikeri\0" "Dorkir\0" "Lumbari\0" "Ralari\0"
    "Fralthi\0" "Snakeir\0" "Sivar\0" "Star post\0";

const char g_aszKilrathiAceNames_0046afd4[] =
    "Bhurak\0" "Dakhath\0" "Khajja\0" "Bakhtosh\0";
