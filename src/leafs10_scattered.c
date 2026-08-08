/*
 *  Tenth leaf tranche.  NOT A REAL COMPILATION UNIT (see accessors_scattered.c).
 */
#include "wc1.h"

unsigned char DAT_0059c490[8192];
int           DAT_0059b320[512];
short         DAT_0059c440[512];

extern unsigned char DAT_0059ca94[];

extern void DoLocalFn8780(int a, short b, int *p);       /* 0x00418780 */
extern void GetG0059c490Fn9660(short a, void *p);        /* 0x00419660 */
extern void DoShipAiFn9810(short a, void *p);            /* 0x00419810 */
extern void DoLocalFn9950(unsigned int *p, int v);       /* 0x00419950 */
extern void ReportComponentHit(char a, char b, char c);  /* 0x00414BF0 */
extern void GetTbl0059b470Fn8280(short a, short b);      /* 0x00418280 */
extern void ShipAiRoutine07(short a, int b);

/* Function start: 0x4187E0 */
void DoLocalFn87E0(short a, int *p) { DoLocalFn8780(0, a, p); }

/* Function start: 0x4196A0 */
void GetG0059c490Fn96A0(short a, short i)
{
    GetG0059c490Fn9660(a, &DAT_0059c490[0] + i * 3);
}

/* Function start: 0x419850 */
void GetG0059c490Fn9850(short a, short i)
{
    DoShipAiFn9810(a, &DAT_0059c490[0] + i * 3);
}

/* Function start: 0x419970 */
void DoLocalFn9970(unsigned int *p, short v) { DoLocalFn9950(p, (int)v << 8); }

/* Function start: 0x41F5D0 */
unsigned int DoLocalFnF5D0(void) { ReportComponentHit(3, 1, 3); return 0; }

/* Function start: 0x4208C0 */
short DoLocalFn08C0(short v)
{
    if ((short)RandomBelowOrEqual(1) != 0)
        return v;
    return -v;
}

/* Function start: 0x421760 */
unsigned int GetTbl0059b320Fn1760(short i)
{
    if (0 < DAT_0059b320[i])
        GetTbl0059b470Fn8280(i, 5);
    return 0;
}

/* Function start: 0x422010 */
int GetTbl0059ca94Fn2010(short i, unsigned char bits)
{
    return (DAT_0059ca94[i] & bits) == bits;
}

/* Function start: 0x4220F0 */
short DoLocalFn20F0(short range)
{
    return (short)RandomBelowOrEqual(range * 2) - range;
}

/* Function start: 0x422E80 */
unsigned int ShipAiRoutine23(short ship)
{
    ShipAiRoutine07(ship, (int)DAT_0059c440[ship] << 8);
    return 0;
}

/* Function start: 0x422EA0 */
unsigned int GetTbl0059b320Fn2EA0(short ship, short i)
{
    ShipAiRoutine07(ship, DAT_0059b320[i]);
    return 0;
}
