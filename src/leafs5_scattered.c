/*
 *  Fifth leaf tranche.  NOT A REAL COMPILATION UNIT (see accessors_scattered.c).
 */
#include "wc1.h"

unsigned char  DAT_0059d620[512];
unsigned char  DAT_005a7ddd, DAT_005a7ddc, DAT_005a7dee, DAT_005a7ded;
int            DAT_005a7de1;
unsigned char  DAT_00469060;
unsigned char  DAT_005a7c30[2048];
unsigned char  DAT_005a7510;
unsigned int   DAT_004699d8;
unsigned short DAT_0046b168;
unsigned char  DAT_0046afc4;
int            DAT_0046b1c8, DAT_0046b1bc, DAT_0046b1b8;
unsigned int   DAT_00476648;
unsigned int   DAT_0059ab2c;

extern unsigned char DAT_0059a850;
extern unsigned int  DAT_0059af8c;

extern void SelectNewShipAiBehavior(short ship);
extern void DoLocalFn41B0(int *p, int v);              /* 0x004141B0 */
extern int  GetCombatFn3D20(void);
extern void GetUiFn7760(void);                         /* 0x00417760 */
extern void BlitUiFn1AE0(void *p, unsigned int v);     /* 0x00421AE0 */
extern unsigned char GetDebugFn5BD0(void);
extern void StoreDAT0046af6c(unsigned char v);
extern void CallThrough42AFC0(short period);
extern void LoopGameFlowFn6230(void);
extern void SetMultimediaTimerCallback(int period);
/* CRT helper: takes its argument on the x87 stack, hence no parameters. */
extern long _ftol(void);

/* Function start: 0x407560 */
void ShipAiState44(short ship) { DAT_0059d620[ship] = 0; SelectNewShipAiBehavior(ship); }

/* Function start: 0x4141D0 */
void SetCombatFn41D0(void)
{
    DAT_005a7ddd = 0;
    DAT_005a7ddc = 0;
    DAT_005a7dee = 0;
    DAT_005a7ded = 0;
}

/* Function start: 0x4142E0 */
void GetCombatFn42E0(int v) { DoLocalFn41B0(&DAT_005a7de1, v); }

/* Function start: 0x416C90 */
void GetCombatFn6C90(void)
{
    if (GetCombatFn3D20() && DAT_00469060 != 0)
        GetUiFn7760();
}

/* Function start: 0x425E30 */
void SetTbl005a7c30Fn5E30(short i, unsigned char b, unsigned int v)
{
    DAT_005a7c30[i * 5] = b;
    *(unsigned int *)(&DAT_005a7c30[1] + i * 5) = v;
}

/* Function start: 0x427B00 */
unsigned int UiRoutine05(void) { BlitUiFn1AE0(&DAT_005a7510, DAT_004699d8); return 0; }

/* Function start: 0x42AFA0 */
void LoopGameFlowFnAFA0(void) { while (GetDebugFn5BD0() == 0) ; }

/* Function start: 0x42DB70 */
short GetUiFnDB70(void)
{
    short v = (short)DAT_0046b168;

    if ((short)DAT_0046b168 == -1)
        v = 0x13;
    return v;
}

/* Function start: 0x430E70 */
void DoUiFn0E70(unsigned char v) { DAT_0046afc4 = v; StoreDAT0046af6c(2); }

/* Function start: 0x4320E0 */
void HelperOf432050A(void)
{
    long v;

    DAT_0046b1c8 = 1;
    v = _ftol();
    DAT_0046b1bc = 0;
    DAT_0046b1b8 = (int)v;
}

/* Function start: 0x432110 */
void CalcVideoFn2110(void)
{
    long v;

    DAT_0046b1c8 = 0;
    v = _ftol();
    DAT_0046b1bc = 0;
    DAT_0046b1b8 = (int)v;
}

/* Function start: 0x432DE0 */
unsigned int HelperOf4326E0A(void) { return DAT_00476648; }

/* Function start: 0x4353F0 */
void GameNoBodyStubv13(void)
{
    *(unsigned short *)(DAT_0059af8c + 4) = 0;
    *(unsigned short *)(DAT_0059af8c + 6) = 0;
}

/* Function start: 0x435570 */
unsigned int ExtMemRoutineAlpha(unsigned int a, void (*fn)(void))
{
    (void)a;
    DAT_0059a850 = 1;
    if (fn != 0)
        (*fn)();
    return 0;
}

/* Function start: 0x4355A0 */
unsigned short ExtMemRoutineBeta(void) { return 0; }

/* Function start: 0x4355D0 */
void ExtMemRoutineGamma(unsigned int v) { DAT_0059ab2c = v; }

/* Function start: 0x4361F0 */
void DoLocalFn61F0(short period) { CallThrough42AFC0(period); LoopGameFlowFn6230(); }

/* Function start: 0x436210 */
void ForwardSetMultimediaTimerCallbackv1(short p) { SetMultimediaTimerCallback((int)p); }

/* Function start: 0x4368C0 */
void GameNoBodyStubv18(void) {}

/* Function start: 0x436980 */
void GameNoBodyStubv21(void) {}
