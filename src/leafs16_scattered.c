/*
 *  Sixteenth leaf tranche.  NOT A REAL COMPILATION UNIT.
 */
#include "wc1.h"
#include <stdlib.h>

int  *DAT_0046a438;
unsigned int DAT_0059e490;
unsigned short DAT_004699a8;

extern unsigned char DAT_0059d100[];
extern unsigned int  DAT_004763f0;
extern unsigned char DAT_005a7510;

extern void ReleasePacketHandle(int h);              /* 0x0042FAE0 */
extern long CalcVideoFn4F20(void);
extern void DoLocalFn84C0(int v);
extern void ReleaseTbl0046c028Fn9BD0(short i);
extern void DebugOverlayThreadProc(int p);           /* 0x0041C910 */
extern int  GetCombatFn49F0(void);
extern void DoCombatFn14C0(void);
extern void ReleaseTbl005a7f10FnF940(void *p);
extern void GetCombatFn42B0(int a, unsigned short b, int c);  /* 0x004142B0 */

/* Function start: 0x41D100 */
void FreePacketAndClear(int *p)
{
    if (*p != 0) {
        ReleasePacketHandle(*p);
        *p = 0;
    }
}

/* Function start: 0x41DF40 */
short ScanTbl0059d100FnDF40(void)
{
    short i = 0x22;

    do {
        if (DAT_0059d100[i] == 4)
            return i;
        i = i + 1;
    } while (i < 0x2a);
    return -1;
}

/* Function start: 0x41E7F0 */
void ShipAiRoutine09(void) { DoLocalFn84C0((int)CalcVideoFn4F20()); }

/* Function start: 0x424B80 */
unsigned int LoopLocalFn4B80(void)
{
    short i = 0;

    do {
        ReleaseTbl0046c028Fn9BD0(i);
        i = i + 1;
    } while (i < 0x40);
    return 0;
}

/* Function start: 0x425B90 */
void GetG004763f0Fn5B90(void)
{
    void *p = (void *)DAT_004763f0;

    if (DAT_004763f0 != 0) {
        DebugOverlayThreadProc((int)DAT_004763f0);
        free(p);
    }
}

/* Function start: 0x425BE0 */
void HelperOf41D760A(void) {}

/* Function start: 0x4279D0 */
unsigned int GetUiFn79D0(void)
{
    if (GetCombatFn49F0())
        DoCombatFn14C0();
    if (DAT_005a7510 != 0)
        ReleaseTbl005a7f10FnF940(&DAT_005a7510);
    return 0;
}

/* Function start: 0x42B3F0 */
int *LoopG0046a438FnB3F0(int key)
{
    int *p = DAT_0046a438;

    while (p != 0) {
        if (*(int *)(*p + 4) == key)
            return p;
        p = (int *)p[1];
    }
    return 0;
}

/* Function start: 0x430D50 */
void GetG004699a8Fn0D50(unsigned int a, int b)
{
    GetCombatFn42B0(b, DAT_004699a8, 0xff);
    DAT_0059e490 = a;
}
