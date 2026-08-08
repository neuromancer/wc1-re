/*
 *  Twenty-fifth leaf tranche.  NOT A REAL COMPILATION UNIT.
 */
#include "wc1.h"
#include "globals.h"

unsigned char DAT_00469d5c;
unsigned char DAT_0046a004;
unsigned char DAT_0046999c;

extern unsigned int GetUiFn79D0(void);
extern void ReleaseTbl00469d50Fn4490(void);   /* 0x00424490 */
extern void ScanTbl0046c028Fn1210(void);      /* 0x00401210 */
extern unsigned int LoopLocalFn4B80(void);
extern void SetNavFn4BE0(void);               /* 0x00424BE0 */
extern unsigned int BlitUiFn1AE0(int *p, char c);
extern short LoopLocalFn54D0(const char *s);

/* Function start: 0x424BA0 */
unsigned int SetG00469d5cFn4BA0(void)
{
    if (DAT_00469d5c == 0)
        return 0;
    DAT_00469d5c = 0;
    GetUiFn79D0();
    DAT_0046a004 = 0;
    ReleaseTbl00469d50Fn4490();
    ScanTbl0046c028Fn1210();
    LoopLocalFn4B80();
    SetNavFn4BE0();
    return 0;
}

/* Function start: 0x425C30 */
unsigned int DoG0046999cFn5C30(unsigned int *p)
{
    char c = *(char *)((int)p + 0xd);

    if (*(char *)((int)p + 0xd) == -1)
        c = DAT_0046999c;
    BlitUiFn1AE0((int *)*p, c);
    return 0;
}

/* Function start: 0x428E70 */
/* Message dwell time: grows with text length, scaled by the speed setting. */
short GetCombatFn8E70(const char *text)
{
    short n = MinShort(5, LoopLocalFn54D0(text) >> 1);

    return (n + 5) * ((char)g_bMessageSpeed_0046af68 + 1);
}
