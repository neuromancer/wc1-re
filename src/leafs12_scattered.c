/*
 *  Twelfth leaf tranche.  NOT A REAL COMPILATION UNIT (see accessors_scattered.c).
 */
#include "wc1.h"

int            DAT_00598a30[512];
unsigned short DAT_005a7eb8, DAT_005a7e70, DAT_005a7ebc, DAT_005a7e74;
unsigned short DAT_005a7ebe, DAT_005a7e76;
unsigned char  DAT_005a7684;
unsigned char  DAT_0046900c;
unsigned char  DAT_005a7dc8;
unsigned char  DAT_005a6b60;
unsigned char  DAT_005a7550;
unsigned short DAT_004699b0;
unsigned int   DAT_0046505c;

extern void SetCombatFn71D0(void);                       /* 0x004171D0 */
extern void DrawCombatFn7260(void);                      /* 0x00417260 */
extern void DoLocalFn1A90(void *a, void *b);
extern void CombatRoutine02(short a, int b, unsigned int c, int d, int e); /* 0x00417A00 */
extern short UiRoutine03(void);                          /* 0x00429E30 family */

/* Function start: 0x40EFE0 */
void SetTbl00598a30FnEFE0(short i, short dx, short dy)
{
    short *p = (short *)(DAT_00598a30[i] + 0x20);

    (void)dy;
    *p = *p + dx;
}

/* Function start: 0x414410 */
void *SetShipAiFn4410(void)
{
    DAT_005a7eb8 = 0;
    DAT_005a7e70 = 0;
    DAT_005a7ebc = 0;
    DAT_005a7e74 = 0;
    DAT_005a7ebe = 0;
    DAT_005a7e76 = 0;
    return &DAT_005a7eb8;
}

/* Function start: 0x416220 */
void DoLocalFn6220(int p, unsigned short a, unsigned short b,
                   unsigned short c, unsigned short d)
{
    *(unsigned short *)(p + 8) = a;
    *(unsigned short *)(p + 10) = b;
    *(unsigned short *)(p + 12) = c;
    *(unsigned short *)(p + 14) = d;
}

/* Function start: 0x4173C0 */
void GetCombatFn73C0(void)
{
    if (DAT_005a7684 != 0) {
        SetCombatFn71D0();
        if (DAT_0046900c != DAT_005a7dc8)
            DrawCombatFn7260();
    }
}

/* Function start: 0x4173F0 */
void GetCombatFn73F0(void)
{
    if (DAT_005a7684 != 0) {
        DAT_0046900c = 0xff;
        DoLocalFn1A90(&DAT_005a6b60, &DAT_005a7550);
        GetCombatFn73C0();
    }
}

/* Function start: 0x417B10 */
void GetG004699b0Fn7B10(short a)
{
    CombatRoutine02(a, 1, DAT_004699b0, 0x17, 0);
}

/* Function start: 0x418540 */
unsigned int DoLocalFn8540(int v)
{
    if (v < 0)
        return 0xffffff00;
    if (v < 1)
        return 0;
    return 0x100;
}

/* Function start: 0x41A110 */
void DoLocalFnA110(short *p)
{
    if (*p > 0x1e) {
        *p = 0x1e;
        return;
    }
    if (*p < -0x1e)
        *p = -0x1e;
}
