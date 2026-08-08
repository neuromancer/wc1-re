/*
 *  Twentieth leaf tranche.  NOT A REAL COMPILATION UNIT.
 */
#include "wc1.h"
#include <stdlib.h>

unsigned short DAT_005a6900[256];
unsigned char  DAT_0046aa04;
unsigned char  DAT_0059e1c0[512];
unsigned int   DAT_00598c18;
unsigned int   DAT_00598af4;

extern unsigned char DAT_0046a9f8;
extern unsigned short DAT_004699a8;

extern void DoLocalFn5BB0(const char *s);
extern void SetDebugFn5C20(void);
extern void GetG004763f0Fn5BC0(void);
extern void LoopLocalFn5470(void *dst, const char *src);   /* 0x00435470 */
extern void SetUiFn6DE0(void *a, unsigned short b, unsigned short c);  /* 0x00436DE0 */
extern unsigned int GetG0046ad88FnFA90(int a, int b);      /* 0x0040FA90 */

/* Function start: 0x42D270 */
unsigned int HelperOf439910A(void)
{
    short i = 0;

    do {
        DAT_005a6900[i * 3] = 0xffff;
        i = i + 1;
    } while (i < 0x1e);
    return 0;
}

/* Function start: 0x42E020 */
void HelperOf42E050A(void)
{
    DoLocalFn5BB0(0);
    SetDebugFn5C20();
    GetG004763f0Fn5BC0();
    exit(1);
}

/* Function start: 0x42E8D0 */
unsigned short GetG0046a9f8FnE8D0(void)
{
    if (DAT_0046a9f8 != 0 && DAT_0046a9f8 != 3 && DAT_0046aa04 != 0)
        return 1;
    return 0;
}

/* Function start: 0x4314F0 */
void GetUiFn14F0(char *text, unsigned short arg)
{
    LoopLocalFn5470(DAT_0059e1c0, text);
    SetUiFn6DE0(DAT_0059e1c0, DAT_004699a8, arg);
}

/* Function start: 0x438B90 */
unsigned int SetUiFn8B90(void)
{
    DAT_00598c18 = GetG0046ad88FnFA90(0x140, 0);
    DAT_00598af4 = GetG0046ad88FnFA90(0x140, 0);
    return 0;
}
