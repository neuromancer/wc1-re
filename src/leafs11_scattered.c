/*
 *  Eleventh leaf tranche.  NOT A REAL COMPILATION UNIT (see accessors_scattered.c).
 */
#include "wc1.h"
#include <stdio.h>

unsigned char DAT_0059b560[512];
unsigned char DAT_0059ce80[512];
unsigned int  DAT_0046aa14;
unsigned char DAT_0046a9f8;
unsigned char DAT_0059ce60[512];

extern unsigned char DAT_0046aa34;
extern unsigned int  DAT_0059af8c;
extern unsigned char DAT_005a7c30[];

extern void DoLocalFn9990(int *a, int *b, short c);      /* 0x00419990 */
extern void ScanTbl0059cab0Fn0AA0(short a);              /* 0x0040AA0 family */
extern void GetG005a8760Fn3DB0(const char *fmt, ...);
extern void Streamer_stop(void);                         /* 0x00442460 */
extern void SetShipAiFnEE80(void);
extern unsigned short GetTbl0059dec0Fn47E0(short i);
extern void SetTbl0059dec0Fn48A0(short i, int v);        /* 0x004148A0 */
extern void StoreDAT0046af6c(unsigned char v);
extern int  ShipAiRoutine18(short s);                    /* 0x00407060 */
extern void DoLocalFn5090(short a, short b, short *p);   /* 0x00435090 */
extern unsigned int GameNoBodyStubv20(void);             /* 0x00427440 family */

/* Function start: 0x4016A0 */
void DoLocalFn16A0(int p, int *q)
{
    DoLocalFn9990((int *)(p + 4), q, *(short *)(p + 0x10));
}

/* Function start: 0x40B110 */
void GetTbl0059b560FnB110(short i)
{
    if (DAT_0059b560[i] == 0x15) {
        DAT_0059ce80[i] = 4;
        ScanTbl0059cab0Fn0AA0(i);
    }
}

/* Function start: 0x4274C0 */
unsigned int DoLocalFn74C0(unsigned int a, short b)
{
    (void)a;
    if (b < 0)
        return GameNoBodyStubv20();
    return 0;
}

/* Function start: 0x42E350 */
void StopMusic(void)
{
    GetG005a8760Fn3DB0("StopMusic");
    DAT_0046aa14 = 0xffffffff;
    Streamer_stop();
    GetG005a8760Fn3DB0("");
}

/* Function start: 0x42E8B0 */
void SoundG0046a9f8FnE8B0(void)
{
    if (DAT_0046a9f8 != 0 && DAT_0046a9f8 != 3)
        StopMusic();
}

/* Function start: 0x42EEB0 */
void DoSceneFnEEB0(void) { SetShipAiFnEE80(); DAT_0046aa34 = 1; }

/* Function start: 0x430D80 */
int DoLocalFn0D80(void) { return (short)GetTbl0059dec0Fn47E0(1) == 4; }

/* Function start: 0x430DC0 */
void DoLocalFn0DC0(void) { SetTbl0059dec0Fn48A0(1, 4); StoreDAT0046af6c(1); }

/* Function start: 0x430E30 */
int GetShipAiFn0E30(void) { return ShipAiRoutine18((short)DAT_0059ce60[0]) == 0; }

/* Function start: 0x434F70 */
void GameNoBodyStubv11(unsigned short a, unsigned short b)
{
    *(unsigned short *)(DAT_0059af8c + 4) = a;
    *(unsigned short *)(DAT_0059af8c + 6) = b;
}

/* Function start: 0x4350D0 */
void DoLocalFn50D0(unsigned int packed, short *p)
{
    DoLocalFn5090((short)packed, (short)(packed >> 16), p);
}

/* ---- retiring stubs: implementing previously-stubbed callees ---- */

char DAT_005a8760[512];
unsigned char DAT_005a7cec;
unsigned char DAT_005a7ec0;

extern void MonoDebug_print(const char *s);              /* 0x00403DE0 */
extern void ShipAiRoutine21(short a, int b);
extern void ShipAiRoutine20(short a, short b);
extern void ShipAiState29(short a, short b);             /* 0x00406FB0 */
extern void ShipAiState40(short a, short b);             /* 0x00406D80 */
extern void DoLocalFn4180(int *p);                       /* 0x00414180 */
extern void DoLocalFnE3C0(void);
extern short DAT_0059d52a;

/* Function start: 0x403DB0 */
void GetG005a8760Fn3DB0(const char *fmt, ...)
{
    vsprintf(DAT_005a8760, fmt, (char *)(&fmt + 1));
    MonoDebug_print(DAT_005a8760);
}

/* Function start: 0x4060B0 */
void SelectNewShipAiBehavior(short ship)
{
    ShipAiRoutine21(ship, -1);
    ShipAiRoutine20(ship, -1);
}

/* Function start: 0x407030 */
void ShipAiState30(short ship, short target)
{
    if (DAT_0059d52a < 0x50) {
        ShipAiState29(ship, target);
        return;
    }
    ShipAiState40(ship, target);
}

/* Function start: 0x4141B0 */
void DoLocalFn41B0(int *p, int v)
{
    if (*(int *)((int)p + 0xd) == v)
        DoLocalFn4180(p);
}

/* Function start: 0x42E330 */
void DoLocalFnE330(void) { GetG005a8760Fn3DB0("SetMusicOn %d"); }

/* Function start: 0x42EE80 */
void SetShipAiFnEE80(void)
{
    DoLocalFnE3C0();
    DAT_005a7cec = 0;
    DAT_005a7ec0 = 0;
}
