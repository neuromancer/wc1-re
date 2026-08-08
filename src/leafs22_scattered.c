/*
 *  Twenty-second leaf tranche.  NOT A REAL COMPILATION UNIT.
 */
#include "wc1.h"

unsigned char DAT_005a7dd0[8192];
short         DAT_0059d710[512];

extern char          DAT_0059dec0[];
extern unsigned char DAT_0059c490[];

extern void  DoLocalFn4180(int *p);
extern short GetTbl0059d710Fn9210(short a, void *p);
extern int   GetShipAiFn0E30(void);
extern int   GetShipAiFn0E10(void);

/* Function start: 0x4148E0 */
void HelperOf430DE0A(short i)
{
    DoLocalFn4180((int *)(&DAT_005a7dd0[0] + i * 0x11));
    DAT_0059dec0[i] = DAT_0059dec0[i] - 1;
}

/* Function start: 0x418560 */
/* Normalise a degree value into (-180, 180]. 0x168 == 360, 0xB4 == 180. */
short DoLocalFn8560(short degrees)
{
    short v = degrees % 0x168;

    if (v < -0xb4)
        v = v + 0x168;
    if (0xb4 < v)
        v = v - 0x168;
    return v;
}

/* Function start: 0x419260 */
short GetTbl0059d710Fn9260(short a, short i)
{
    return GetTbl0059d710Fn9210(a, &DAT_0059c490[0] + i * 3) - DAT_0059d710[i];
}

/* Function start: 0x430E50 */
unsigned short DoLocalFn0E50(void)
{
    if (GetShipAiFn0E30() == 0) {
        if (GetShipAiFn0E10())
            return 0;
    }
    return 1;
}
