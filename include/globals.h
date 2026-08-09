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

extern unsigned int DAT_00465040;
extern short g_nActiveSoundEffectShips_00465044;
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
extern short DAT_00465460;
extern int g_aiPacketReferenceTable_00465c88[4 * 0x25];
extern unsigned short DAT_00468660;
extern unsigned int DAT_00468664;
extern unsigned char DAT_0046870c;
extern unsigned char DAT_00468710;
extern char *DAT_00469004;
extern char *DAT_00469008;
extern unsigned char DAT_0046900c;
extern unsigned char DAT_00469060;
extern unsigned char DAT_00469068;
extern unsigned short DAT_00469090;
extern short DAT_00469208;
extern unsigned char DAT_00469648;
extern unsigned char DAT_0046964c;
extern unsigned char DAT_0046999c;
extern unsigned short DAT_004699a8;
extern unsigned short DAT_004699b0;
extern unsigned int DAT_004699d8;
extern unsigned char DAT_00469d5c;
extern unsigned char *DAT_00469dbc[64];
extern unsigned int DAT_00469e34;
extern unsigned char DAT_00469fb4;
extern short g_nFrameSkip_00469fb8;
extern short g_nOriginDevUnlock_00469ff4;
extern int DAT_00469ffc;
extern int DAT_0046a000;
extern unsigned char DAT_0046a004;
extern int *DAT_0046a438;
extern unsigned char DAT_0046a440;
extern WaveTableEntry *g_pWaveTableHead_0046a444;
extern WaveTableEntry *g_pWaveTableTail_0046a448;
extern short DAT_0046a9f8;
extern short DAT_0046aa04;
extern unsigned int DAT_0046aa14;
extern unsigned char DAT_0046aa30;
extern unsigned char DAT_0046aa34;
extern unsigned char DAT_0046aa38;
extern unsigned char DAT_0046aa3c;
extern unsigned char DAT_0046aa40;
extern unsigned char g_abTaggedAllocationPrefix_0046ad88[8];
extern unsigned char DAT_0046af60;
extern unsigned char DAT_0046af6c;
extern unsigned char DAT_0046afc4;
extern unsigned short DAT_0046b168;
extern const GUID g_guidDirectDraw2_00463118;
extern int *DAT_0046b1a4;
extern signed char g_abSoundEffectShips_0046c028[0x14];
extern short DAT_0046c010;
extern unsigned char DAT_0046c03c;
extern short DAT_0046c04c;
extern unsigned char DAT_0046c060;
extern short DAT_0046c064;
extern short DAT_0046c068;
extern unsigned char DAT_0046c080;
extern unsigned short DAT_0046c084;
extern int DAT_0046da90;
extern int DAT_0046da94;
extern int DAT_0046daa0;
extern int g_bMonoDebugInstalled_00475e70;
extern HANDLE g_hMonoDebugDevice_00475e74;
extern unsigned short DAT_00475e78;
extern unsigned int DAT_004763f0;
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
extern int DAT_004875a8[5];
extern unsigned int DAT_00496fc0[5];
extern unsigned char DAT_005988de[8192];
extern int DAT_00598a30[512];
extern unsigned char DAT_00598ab0;
extern unsigned int DAT_00598ab6;
extern unsigned char DAT_00598aba;
extern unsigned int DAT_00598af4;
extern unsigned int DAT_00598c18;
extern unsigned char g_aInputEventSlots_00598c40[0x1c00];
extern int DAT_0059a84c;
extern int DAT_0059a8e0;
extern int DAT_0059a8e4;
extern unsigned char DAT_0059a850;
extern unsigned char * volatile DAT_0059ab19;
extern unsigned short DAT_0059ab1d;
extern Viewport * volatile DAT_0059ab23;
extern void (*DAT_0059ab2c)(void);
extern unsigned int DAT_0059ab54;
extern int DAT_0059ab3c;
extern int DAT_0059ab40;
extern int DAT_0059ab44;
extern int DAT_0059ab48;
extern int DAT_0059ab5c;
extern int DAT_0059ab60;
extern int g_aiInputEventSlotUsed_0059ab70[0x100];
extern unsigned int DAT_0059af8c;
extern short DAT_0059ab34;
extern int DAT_0059ab4c;
extern unsigned char DAT_0059ab58;
extern int DAT_0059b320[512];
extern int g_nSoundEffectSlotCount_0059bfe0;
extern unsigned int DAT_0059b430[512];
extern int DAT_0059b470[512];
extern unsigned char DAT_0059b560[512];
extern unsigned char DAT_0059c310[512];
extern short g_asShipSavedSlotState_0059c330[512];
/* Queued ShipOrder, applied once a jump-out completes.  SetShipOrder writes the
 * live order to g_aeShipOrder_0059d200, but 0x00422C30 diverts the write here
 * when the current order is ORDER_JUMP_OUT.  Every observed value (5, 7, 8, 10)
 * is a ShipOrder enumerator. */
extern unsigned char g_abShipQueuedOrder_0059c3f0[512];
extern unsigned short DAT_0059c420[512];
extern short DAT_0059c440[512];
extern unsigned char DAT_0059c490[8192];
extern unsigned char DAT_0059c810[512];
extern unsigned char DAT_0059ca94[256];
extern short DAT_0059ce10;
extern unsigned int DAT_0059ce18[256];
extern unsigned char DAT_0059ce60[512];
extern unsigned char DAT_0059ce80[512];
extern unsigned char DAT_0059cf20[512];
/* Per-ship slot state.  0 means the slot is unused (DestroyShip clears it to 0
 * and the allocator scans for 0); GetShipSlotState stashes the old value in
 * g_asShipSavedSlotState_0059c330 before forcing 1.  Values above 0x0B mean the
 * ship has its data loaded, and 0x0D specifically owns a packet that must be
 * freed.  Not the ship class -- see include/wcdata.h. */
extern unsigned char g_abShipSlotState_0059d100[512];
extern unsigned short DAT_0059d2d0[512];
extern unsigned char DAT_0059d400[512];
extern unsigned short DAT_0059d500[2048];
extern char DAT_0059d520[512];
extern short DAT_0059d52a;
extern unsigned int DAT_0059d5e0[512];
extern unsigned char DAT_0059d620[512];
extern unsigned char DAT_0059d630[512];
extern unsigned char DAT_0059d650[512];
extern short DAT_0059d710[512];
extern unsigned char DAT_0059d7a0[512];
extern unsigned short DAT_0059d9b0[512];
extern unsigned char DAT_0059daca[8192];
extern unsigned char DAT_0059dacf[8192];
extern unsigned char DAT_0059dcb0[512];
extern unsigned char DAT_0059dd10[8192];
extern char DAT_0059dec0[256];
extern short DAT_0059df60;
extern unsigned char DAT_0059e1c0[512];
extern unsigned int DAT_0059e490;
extern int g_aiPacketHandles_0059e530[0x1000];
extern int g_aiPacketHandleOffsets_005a2530[0x1000];
extern int g_nPacketHandleCount_005a6530;
extern int g_aiSoundEffectShipActive_005a66f0[0x40];
extern int *DAT_005a6538;
extern int g_nAllocateViewportCalls_005a68ec;
extern unsigned short DAT_005a6900[256];
extern Viewport DAT_005a6b60;
extern Viewport DAT_005a6ba0;
#define DAT_005a6baa DAT_005a6ba0.top
#define DAT_005a6bae DAT_005a6ba0.bottom
extern Viewport DAT_005a7510;
extern Viewport DAT_005a7550;
extern unsigned char DAT_005a7684;
extern Viewport DAT_005a76b0;
extern unsigned short DAT_005a7780;
extern unsigned int DAT_005a77ec;
extern unsigned int DAT_005a7c2c;
extern unsigned char DAT_005a7c30[2048];
extern unsigned char DAT_005a7c31[2048];
extern unsigned short DAT_005a7cc0[16];
extern unsigned char DAT_005a7cec;
extern unsigned char DAT_005a7dc8;
extern unsigned char DAT_005a7dca;
extern unsigned char DAT_005a7dd0[8192];
extern int DAT_005a7de1;
extern unsigned char DAT_005a7e30[2048];
extern unsigned short DAT_005a7ea0[64];
extern unsigned char DAT_005a7ec0;
extern int g_nViewportAllocationCount_005a7f0c;
extern unsigned char *g_apViewportAllocations_005a7f10[128];
extern int g_nFreeViewportCalls_005a8110;
extern int DAT_005a7d9c;
extern unsigned char *DAT_005a7cf0;
extern unsigned char DAT_005a86b0;
extern char DAT_005a8760[512];
extern unsigned int DAT_005a8964;
extern unsigned short DAT_005a897c;
extern unsigned int DAT_005a898c;
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
extern char *PTR_s_MISSILE_LOCKED_004691d4[8];
extern char *g_apszCommMenuText_0046af90[32];
extern unsigned long g_dwGameClockBase_005a89a8;

extern short DAT_0059ab10;
extern unsigned char DAT_005a7ddc;
extern unsigned char DAT_005a7ddd;
extern unsigned char DAT_005a7ded;
extern unsigned char DAT_005a7dee;
extern unsigned short DAT_005a7e70;
extern unsigned short DAT_005a7eb8;
extern unsigned short DAT_005a7ebc;
extern unsigned short DAT_005a7ebe;
extern unsigned int DAT_005a7ef0;
extern unsigned int DAT_005a7efc;
extern short DAT_0059af70;
extern int DAT_0046b1b8;
extern int DAT_0046b1bc;
extern int DAT_0046b1c4;
extern int DAT_0046b1c8;
extern unsigned int g_dwStreamerState_00597cd0;
extern short DAT_0059a856;
extern unsigned char DAT_0059ca69;
extern unsigned char DAT_0059ca6a;
extern unsigned char DAT_004700c9;
extern unsigned char DAT_004700ca;
extern int DAT_004688e0;
extern int DAT_00469fbc;
extern int DAT_00469fc0;
extern int DAT_00469fc8[16];
extern char g_szStreamsPath_00475c18[0x100];

/* Declared as comma-separated lists in the original tranches. */
extern short DAT_0059ab12;
extern short DAT_0059af72;
extern unsigned short DAT_005a7e74;
extern unsigned short DAT_005a7e76;
extern unsigned int DAT_005a7ef4;
extern unsigned int DAT_005a7ef8;

extern unsigned char DAT_00466472[65536];
extern int  *DAT_0046b1a8;
extern int  *DAT_0046b1ac;
extern int  *DAT_0046b1b0;
extern int   DAT_0046b1b4;
extern unsigned char DAT_0046c97a[16384];
extern char  DAT_00486078[256];

extern unsigned char g_bMessageSpeed_0046af68;

/* Ordered name tables read directly out of the image; see include/wcdata.h.
 * Both are packed variable-length strings, not fixed-stride arrays, so the
 * game indexes them through a pointer table rather than by multiplication. */
extern const char g_aszShipClassNames_004684d4[];    /* Hornet .. Star post, then weapons */
extern const char g_aszKilrathiAceNames_0046afd4[];  /* Bhurak, Dakhath, Khajja, Bakhtosh */

#endif /* WC1_GLOBALS_H */
