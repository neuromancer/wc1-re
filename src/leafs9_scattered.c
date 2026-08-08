/*
 *  Ninth leaf tranche.  NOT A REAL COMPILATION UNIT (see accessors_scattered.c).
 */
#include "wc1.h"

unsigned char  DAT_0059dd10[8192];
unsigned char  DAT_0059dacf[8192];
short          DAT_0059c330[512];
unsigned char  DAT_0046c060;
short          DAT_0046c064;
short          DAT_0046c068;
char          *DAT_00469004;

extern unsigned char DAT_0046c03c;
extern char         *PTR_s_MISSILE_LOCKED_004691d4[];

extern void GetTbl0059d710Fn9210(short a, int *p);              /* 0x00419210 */
extern long CalcVideoFn4F20(void);                              /* 0x00434F20 */
extern void DoLocalFn84C0(int v);                               /* 0x004184C0 */
extern void DispatchTbl0059b9e0Fn17D0(void *a, int b, short c); /* 0x004117D0 */
extern void GetCombatFn42E0(int v);
extern void DispatchG004691e2Fn6260(char *a, unsigned char b);  /* 0x00416260 */

/* Function start: 0x40A160 */
void GetShipAiFnA160(short ship)
{
    GetTbl0059d710Fn9210(ship, (int *)(&DAT_0059dd10[0] + ship * 0xc));
}

/* Function start: 0x40DF50 */
char *HelperOf40DF70B(short i)
{
    char *p = *(char **)(&DAT_0059dacf[0] + i * 0x1f);

    if (*p == '?')
        p = p + 1;
    return p;
}

/* Function start: 0x410AD0 */
void DoLocalFn0AD0(void) { DoLocalFn84C0((int)CalcVideoFn4F20()); }

/* Function start: 0x4117B0 */
unsigned int DoUiFn17B0(void *p, unsigned int arg)
{
    DAT_0046c03c = 0xff;
    DispatchTbl0059b9e0Fn17D0(p, (int)p, (short)arg);
    return 0;
}

/* Function start: 0x412410 */
unsigned short GetShipAiFn2410(short ship)
{
    if (DAT_0059c330[ship] != -1)
        DAT_0059c330[ship] = DAT_0059c330[ship] - 1;
    return ship;
}

/* Function start: 0x415FC0 */
void SetCombatFn5FC0(unsigned short v)
{
    DAT_0046c060 = 0;
    DAT_0046c064 = v;
    DAT_0046c068 = (short)RandomBelowOrEqual(0x167);
}

/* Function start: 0x416010 */
void SetCombatFn6010(void)
{
    if (DAT_0046c064 >= 0)
        DAT_0046c060 = 1;
    GetCombatFn42E0((int)PTR_s_MISSILE_LOCKED_004691d4[0]);
    DAT_0046c064 = -1;
}

/* Function start: 0x416460 */
void DoCombatFn6460(char *s, unsigned char b)
{
    DAT_00469004 = s;
    DispatchG004691e2Fn6260(s, b);
}

/* Function start: 0x418520 */
unsigned short DoLocalFn8520(short v)
{
    if (v < 0)
        return 0xffff;
    return (unsigned short)(0 < v);
}
