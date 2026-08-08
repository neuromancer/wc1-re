/*
 *  Seventeenth leaf tranche.  NOT A REAL COMPILATION UNIT.
 */
#include "wc1.h"
#include <stdlib.h>

unsigned char DAT_0059d650[512];
unsigned char DAT_005a7e30[2048];

extern short DAT_0059ab10, DAT_0059ab12;

extern void DIBpositionWindow(void);
extern void DispatchDebugFn9F80(short i);            /* 0x00409F80 */
extern void DispatchTbl0059d400FnA030(short i);      /* 0x0040A030 */
extern void DoLocalFn3C40(void *p);                  /* 0x00413C40 */

/* Function start: 0x401390 */
/* Divides in place and returns the quotient: *p keeps only the multiple of n. */
short DoLocalFn1390(short *p, short n)
{
    short v = *p;

    *p = v - v % n;
    return v / n;
}

/* Function start: 0x4020E0 */
void ShowNoticeMessageBox(const char *text)
{
    DIBpositionWindow();
    MessageBoxA(0, text, "NOTICE", 0x10);
    exit(0);
}

/* Function start: 0x40A130 */
void GetTbl0059d650FnA130(short i)
{
    if (DAT_0059d650[i] == 0) {
        DispatchDebugFn9F80(i);
        return;
    }
    DispatchTbl0059d400FnA030(i);
}

/* Function start: 0x40DE70 */
void DoLocalFnDE70(unsigned char *out)
{
    unsigned char tmp[4];

    DoLocalFn3C40(tmp);
    out[1] = tmp[2];
    out[0] = tmp[3];
}

/* Function start: 0x436160 */
void SetInputFn6160(short x, short y)
{
    DAT_0059ab10 = x;
    DAT_0059ab12 = y;
}
