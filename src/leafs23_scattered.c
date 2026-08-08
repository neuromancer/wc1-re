/*
 *  Twenty-third leaf tranche.  NOT A REAL COMPILATION UNIT.
 *
 *  The two scanners below allocate from the same DAT_0059D100 slot table but
 *  from different ranges: 1..9 for the first, 10..0x3C for the second, which
 *  looks like reserved-vs-general object slots.
 */
#include "wc1.h"

short          DAT_0046c010;
unsigned short DAT_0059d9b0[512];
unsigned int   DAT_0059d5e0[512];

extern unsigned char DAT_0059d100[];
extern unsigned char DAT_0059ce60[];

extern void ShipAiRoutine20(short a, short b);

/* Function start: 0x419B70 */
short ScanTbl0059d100Fn9B70(void)
{
    DAT_0046c010 = 1;
    do {
        if (DAT_0059d100[DAT_0046c010] == 0)
            return DAT_0046c010;
        DAT_0046c010 = DAT_0046c010 + 1;
    } while (DAT_0046c010 < 10);
    DAT_0046c010 = -1;
    return -1;
}

/* Function start: 0x419BA0 */
short ScanTbl0059d100Fn9BA0(void)
{
    short i = 10;

    do {
        if (DAT_0059d100[i] == 0) {
            DAT_0059d9b0[i] = 0x8001;
            return i;
        }
        i = i + 1;
    } while (i < 0x3d);
    return -1;
}

/* Function start: 0x422CD0 */
unsigned int SetTbl0059d5e0Fn2CD0(short i, unsigned int v)
{
    ShipAiRoutine20(i, -1);
    DAT_0059d5e0[i] = v;
    DAT_0059ce60[i] = 0xff;
    return 0;
}
