/*
 *  Twenty-sixth leaf tranche.  NOT A REAL COMPILATION UNIT.
 */
#include "wc1.h"

int *DAT_005a6538;

extern void LoopLocalFn8EA0(int mode);              /* 0x00428EA0 */
extern void ShowOnScreenMessage(short f, short d, const char *fmt, ...);
extern int CheckMcgaVideoMode();

/* Function start: 0x42F7E0 */
/* Initialises a 320x200 viewport record (0x13F == 319, 199) then validates it. */
void DoUiFnF7E0(int *vp, short arg)
{
    DAT_005a6538 = vp;
    *(unsigned short *)(vp + 2) = 0;
    *(unsigned short *)(vp + 3) = 0x13f;
    *(unsigned short *)((int)vp + 0xe) = 199;
    *(unsigned short *)((int)vp + 10) = 0;
    CheckMcgaVideoMode(vp, arg, 0);
}

/* Function start: 0x430BC0 */
short DoLocalFn0BC0(short v, int flag)
{
    if (v == -1)
        return 0xd - (unsigned short)(flag == 0);
    if (flag == 1)
        v = v - 1;
    return v;
}
