/*
 *  Sixth leaf tranche.  NOT A REAL COMPILATION UNIT (see accessors_scattered.c).
 */
#include "wc1.h"

unsigned char  DAT_0059daca[8192];
unsigned short DAT_005a7ea0[64];
unsigned short DAT_00469090;
short          DAT_00469208;
unsigned int   DAT_005a7ef0, DAT_005a7ef4, DAT_005a7ef8, DAT_005a7efc;
unsigned char  DAT_0059c810[512];
unsigned short DAT_005a7cc0[16];
unsigned short DAT_0059c420[512];
unsigned char  DAT_00598ab0;

extern unsigned char DAT_0059dcb0[];
extern char          DAT_0059d520[];
extern unsigned int  DAT_0059b430[];

extern void ShipAiState30(short a, short b);                  /* 0x00407030 */
extern void ReleaseTbl0046c028Fn9BD0(short i);                /* 0x00409BD0 */
extern void SetTbl0059d710Fn9290(short a, int *p);            /* 0x00419290 */
extern void ShipAiRoutine04(short a, short b);                /* 0x00406310-family */
extern void ShipAiRoutine21(short a, int b);                  /* 0x004075A0-family */
extern void ShipAiRoutine07(short a, int b);                  /* 0x00406910-family */
extern void PumpWindowMessages(void);                         /* 0x00402320 */
extern void DispatchTbl0046da90Fn59C0(int a);                 /* 0x004359C0 */

/* Function start: 0x402E80 */
/* Byte-identical duplicate of SetMousePosition (0x00402E80); both exist because
 * VC++ 4.2's linker did no COMDAT folding. */
void ForwardSetCursorPos(int x, int y) { SetCursorPos(x, y); }

/* Function start: 0x407580 */
void ShipAiState27(short ship, short target) { ShipAiState30(ship, target); }

/* Function start: 0x4150B0 */
void GetTbl0059dacaFn50B0(short i, unsigned char bits)
{
    DAT_0059daca[i * 0x1f] |= bits;
}

/* Function start: 0x415A70 */
void LoopCombatFn5A70(void)
{
    short i = 10;

    do {
        DAT_005a7ea0[i] = 0;
        i = i - 1;
    } while (i != 0);
    DAT_00469090 = 0xffff;
}

/* Function start: 0x4168A0 */
void GetUiFn68A0(void)
{
    if (DAT_00469208 != -1)
        ReleaseTbl0046c028Fn9BD0(DAT_00469208);
}

/* Function start: 0x417610 */
void SetCombatFn7610(void)
{
    DAT_005a7ef0 = 0;
    DAT_005a7ef4 = 0;
    DAT_005a7ef8 = 0;
    DAT_005a7efc = 0;
}

/* Function start: 0x419390 */
void DoLocalFn9390(short a, int *p) { SetTbl0059d710Fn9290(a, p); }

/* Function start: 0x4193B0 */
void DoLocalFn93B0(short a, short b) { ShipAiRoutine04(a, b); }

/* Function start: 0x4213B0 */
unsigned int ShipAiRoutine15(short ship)
{
    DAT_0059c810[ship] = 10;
    ShipAiRoutine21(ship, 3);
    return 0;
}

/* Function start: 0x422110 */
int ShipAiRoutine16(short ship, unsigned int bits)
{
    return (DAT_0059b430[ship] & bits) != 0;
}

/* Function start: 0x422440 */
void ScanTbl005a7cc0Fn2440(void)
{
    short i = 0;

    do {
        DAT_005a7cc0[i] = 0xffff;
        i = i + 1;
    } while (i < 10);
}

/* Function start: 0x422D30 */
void ShipAiRoutine20(short ship, short state)
{
    DAT_0059dcb0[ship] = (unsigned char)state;
    DAT_0059c420[ship] = 0;
    DAT_0059d520[ship] = 0;
}

/* Function start: 0x422DD0 */
unsigned int DoLocalFn2DD0(short ship) { ShipAiRoutine07(ship, 0); return 0; }

/* Function start: 0x435CC0 */
void DoLocalFn5CC0(int a) { PumpWindowMessages(); DispatchTbl0046da90Fn59C0(a); }

/* Function start: 0x4369E0 */
void GameNoBodyStubv23(void) {}

/* Function start: 0x43F720 */
void DoSaveLoadFnF720(void) { DAT_00598ab0 = 0; }
