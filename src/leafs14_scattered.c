/*
 *  Fourteenth leaf tranche.  NOT A REAL COMPILATION UNIT.
 */
#include "wc1.h"

short         DAT_0059af70, DAT_0059af72, DAT_0059ab10, DAT_0059ab12;
unsigned char DAT_0059d400[512];
unsigned char DAT_0059d630[512];

extern int           DAT_0059b320[];
extern unsigned char DAT_0059c310[];
extern unsigned char DAT_0059d7a0[];
extern unsigned char DAT_005a7c30[];

extern void DoLocalFn9990(int *a, int *b, short c);
extern void SetInputFn6160(short x, short y);      /* 0x00436160 */
extern void GetTbl0059b320FnE820(short i);         /* 0x0040E820 */
extern void ReleaseTbl0046c028Fn9BD0(short i);

/* Function start: 0x401680 */
void DoLocalFn1680(int p, int *q)
{
    DoLocalFn9990((int *)(p + 4), q, *(short *)(p + 0x10) + 0x10cc);
}

/* Function start: 0x401CE0 */
void SetInputFn1CE0(short x, short y)
{
    DAT_0059af70 = x;
    DAT_0059af72 = y;
    DAT_0059ab10 = x;
    DAT_0059ab12 = y;
    SetInputFn6160(x, y);
}

/* Function start: 0x403EE0 */
unsigned int GetTbl0059d400Fn3EE0(short i)
{
    if (i != -1 && DAT_0059d400[i] == 0)
        return 1;
    return 0;
}

/* Function start: 0x403F10 */
void DoTbl0059b320Fn3F10(short i, short v)
{
    DAT_0059b320[i] = (int)v << 8;
    GetTbl0059b320FnE820(i);
}

/* Function start: 0x40BEA0 */
void LoopLocalFnBEA0(void)
{
    short i = 0;

    do {
        ReleaseTbl0046c028Fn9BD0(i);
        i = i + 1;
    } while (i < 10);
}

/* Function start: 0x41E7C0 */
unsigned int ShipAiRoutine08(short ship)
{
    DAT_0059c310[ship] = 0;
    DAT_0059d7a0[ship] = 0;
    DAT_0059d630[ship] = 0;
    return 0;
}

/* Function start: 0x425ED0 */
void ScanTbl005a7c30Fn5ED0(short v)
{
    short i = 5;

    do {
        if ((char)DAT_005a7c30[i * 5] == v)
            return;
        i = i - 1;
    } while (i != 0);
}
