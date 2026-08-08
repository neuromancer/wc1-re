/*
 *  Twenty-fourth leaf tranche.  NOT A REAL COMPILATION UNIT.
 */
#include "wc1.h"

unsigned char DAT_00466472[65536];   /* ship-type records, 0x87-byte stride */
unsigned char DAT_0046c97a[16384];   /* per-object records, 0x36-byte stride */

extern unsigned int  DAT_0059d5e0[];
extern unsigned char DAT_0059b560[];

extern void ShipAiRoutine20(short a, short b);
extern void ShipAiRoutine07(short a, int b);

/* Function start: 0x422D00 */
unsigned int DoTbl0059d5e0Fn2D00(short i, unsigned int v)
{
    ShipAiRoutine20(i, -1);
    DAT_0059d5e0[i] = v;
    return 0;
}

/* Function start: 0x422E50 */
unsigned int ShipAiRoutine22(short ship)
{
    ShipAiRoutine07(ship,
        (int)*(short *)(&DAT_00466472[0] + DAT_0059b560[ship] * 0x87) << 8);
    return 0;
}

/* Function start: 0x423610 */
unsigned int GetTbl0046c97aFn3610(short i)
{
    if (i != -1 && DAT_0046c97a[i * 0x36] != 3)
        return 0;
    return 1;
}
