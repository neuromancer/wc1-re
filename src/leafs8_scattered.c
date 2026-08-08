/*
 *  Eighth leaf tranche.  NOT A REAL COMPILATION UNIT (see accessors_scattered.c).
 */
#include "wc1.h"
#include <stdlib.h>

unsigned int   DAT_00465040;
unsigned int   DAT_00598ab6;
unsigned char  DAT_004875a8;
unsigned int   DAT_00496fc0;

extern unsigned char DAT_0059ab3c;
extern unsigned char DAT_00598aba;
extern short         DAT_0046daa0;
extern unsigned long g_dwGameClockBase_005a89a8;

extern void DoLocalFn3BA0(int a, int b, unsigned int c, int d, int e);  /* 0x00403BA0 */
extern void ShipAiRoutine02(short a, unsigned int b, int c);            /* 0x00406200-family */
extern void ValidateViewportBounds(int *a, int *b, unsigned int *c);    /* 0x00440C00 */
extern void SetTbl00486112Fn3020(short a, unsigned short *b);           /* 0x00443020 */
/* 0x0044A7D0 is the debug CRT free helper (_free_dbg family). */
extern void _free_dbg(void *p, int blockType);

/* Function start: 0x401250 */
unsigned int GetSfxFn1250(void) { return abs(0x19 - (int)DAT_00465040); }

/* Function start: 0x401270 */
void DoLocalFn1270(void) { MinShort(0x14, (short)RandomBelowOrEqual(7) + 10); }

/* Function start: 0x401870 */
void GetSfxFn1870(void) { DoLocalFn3BA0(0, 0x14, DAT_00465040, 0x10cc, 0xc1c); }

/* Function start: 0x4030B0 */
void InitGameClockRandomEpoch(void)
{
    g_dwGameClockBase_005a89a8 = GetTickCount() + (rand() & 3600000);
}

/* Function start: 0x407350 */
void ShipAiState24(short ship, unsigned int arg) { ShipAiRoutine02(ship, arg, 0); }

/* Function start: 0x407370 */
void ShipAiState34(short ship, unsigned int arg) { ShipAiRoutine02(ship, arg, 0x23); }

/* Function start: 0x435DC0 */
unsigned int DoUiFn5DC0(void) { DAT_0046daa0 = 0; return 1; }

/* Function start: 0x436240 */
int GetGameFlowFn6240(void) { return DAT_0059ab3c == 0; }

/* Function start: 0x436950 */
void DoLocalFn6950(void *p) { if (p != 0) _free_dbg(p, 1); }

/* Function start: 0x43F730 */
void GetSaveLoadFnF730(short i)
{
    int v = *(int *)(DAT_00598ab6 + i * 4);

    if (v != 0)
        DAT_00598aba = (unsigned char)v;
}

/* Function start: 0x440CF0 */
void CombatRoutine04(int *p)
{
    ValidateViewportBounds(p, (int *)&DAT_004875a8, &DAT_00496fc0);
}

/* Function start: 0x4413C0 */
void DoLocalFn13C0(short a, unsigned short *b) { SetTbl00486112Fn3020(a, b); }

/* NOTE: Ghidra's prototype for 0x00425BB0 is void(void), but the disassembly
 * pushes an argument at every call site -- it takes a string.  The decompiler's
 * *signatures* are as untrustworthy as its names when the DB prototype is wrong. */
extern void Helper4272F0(void);             /* 0x004272F0 */
extern void DoLocalFn5BB0(const char *s);   /* 0x00425BB0 */
extern void WriteDebugString(const char *s);
extern void SetDebugFn5C20(void);           /* 0x00425C20 */
extern void AbortToDesktop(void);           /* 0x00402110 */

/* Function start: 0x427370 */
void exit_squadron(const char *msg)
{
    Helper4272F0();
    DoLocalFn5BB0(msg);
    DoLocalFn5BB0("[SYSTEM]: Exit_squadron\n");
    WriteDebugString("[SYSTEM] exit_squadron");
    WriteDebugString(msg);
    SetDebugFn5C20();
    AbortToDesktop();
    exit(0);
}
