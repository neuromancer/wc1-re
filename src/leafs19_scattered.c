/*
 *  Nineteenth leaf tranche.  NOT A REAL COMPILATION UNIT.
 */
#include "wc1.h"

unsigned char *DAT_00469dbc[64];
unsigned char  DAT_005988de[8192];

extern unsigned char DAT_0059c3f0[];
extern unsigned char DAT_0059d100[];
extern short         DAT_0059c330[];
extern unsigned char DAT_005a7c30[];
extern unsigned char DAT_005a7e30[];

extern short ScanTbl0059d100Fn54C0(void);      /* 0x004154C0 */
extern int   DoLocalFn0D80(void);
extern short GetCombatFn0DA0(void);
extern void  ShowEnemyTargetSelectMenu(void);  /* 0x00430E90 */
extern int   GetShipAiFn0E30(void);
extern int   GetShipAiFn0E10(void);

/* Function start: 0x40B700 */
unsigned int GetTbl0059d100FnB700(short i)
{
    unsigned int prev;

    if (DAT_0059c3f0[i] == 7) {
        prev = DAT_0059d100[i];
        DAT_0059d100[i] = 1;
        DAT_0059c330[i] = (short)prev;
    }
    return 0;
}

/* Function start: 0x413F70 */
unsigned int HelperOf415180A(char slot, int rec)
{
    int k = slot * 10;

    *(int *)(&DAT_005a7e30[0] + k) = rec;
    *(unsigned short *)(&DAT_005a7e30[4] + k) = *(unsigned short *)(rec + 4);
    *(unsigned short *)(&DAT_005a7e30[6] + k) = *(unsigned short *)(rec + 6);
    return 0;
}

/* Function start: 0x415510 */
unsigned int GetShipAiFn5510(void)
{
    if (DAT_0059c3f0[0] != 1) {
        if (ScanTbl0059d100Fn54C0() == 0)
            return 0;
    }
    return 1;
}

/* Function start: 0x425DF0 */
unsigned char *GetTbl005a7c30Fn5DF0(short i)
{
    short k = (short)(char)DAT_005a7c30[i * 5];

    if (8 < k)
        return DAT_00469dbc[k];
    return &DAT_005988de[0] + k * 0x26;
}
