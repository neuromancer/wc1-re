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
extern unsigned char DAT_00465058;
extern unsigned int DAT_0046505c;
extern unsigned char DAT_00465074;
extern unsigned int DAT_004650ac;
extern short DAT_00465460;
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
extern unsigned char DAT_0046a004;
extern int *DAT_0046a438;
extern unsigned char DAT_0046a440;
extern unsigned char DAT_0046a9f8;
extern unsigned char DAT_0046aa04;
extern unsigned int DAT_0046aa14;
extern unsigned char DAT_0046aa30;
extern unsigned char DAT_0046aa34;
extern unsigned char DAT_0046aa38;
extern unsigned char DAT_0046aa3c;
extern unsigned char DAT_0046aa40;
extern unsigned char DAT_0046af60;
extern unsigned char DAT_0046af6c;
extern unsigned char DAT_0046afc4;
extern unsigned short DAT_0046b168;
extern int *DAT_0046b1a4;
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
extern short DAT_0046daa0;
extern unsigned short DAT_00475e78;
extern unsigned int DAT_004763f0;
extern unsigned int DAT_00476648;
extern unsigned int DAT_00486074;
extern unsigned char DAT_00486110[1024];
extern unsigned char DAT_00486518;
extern unsigned char DAT_004875a8;
extern unsigned int DAT_00496fc0;
extern unsigned char DAT_005988de[8192];
extern int DAT_00598a30[512];
extern unsigned char DAT_00598ab0;
extern unsigned int DAT_00598ab6;
extern unsigned char DAT_00598aba;
extern unsigned int DAT_00598af4;
extern unsigned int DAT_00598c18;
extern unsigned char DAT_0059a850;
extern unsigned char DAT_0059ab19;
extern unsigned int DAT_0059ab2c;
extern unsigned char DAT_0059ab3c;
extern unsigned int DAT_0059af8c;
extern int DAT_0059b320[512];
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
extern int *DAT_005a6538;
extern unsigned short DAT_005a6900[256];
extern unsigned char DAT_005a6b60;
extern unsigned char DAT_005a6ba0;
extern unsigned short DAT_005a6baa;
extern unsigned short DAT_005a6bae;
extern unsigned char DAT_005a7510;
extern unsigned char DAT_005a7550;
extern unsigned char DAT_005a7684;
extern unsigned char DAT_005a76b0;
extern unsigned short DAT_005a7780;
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
extern unsigned char DAT_005a86b0;
extern char DAT_005a8760[512];
extern unsigned int DAT_005a8964;
extern unsigned short DAT_005a897c;
extern unsigned int DAT_005a898c;
extern unsigned int DAT_005a8a40;
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
extern int DAT_0046b1c8;

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
