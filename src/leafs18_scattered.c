/*
 *  Eighteenth leaf tranche.  NOT A REAL COMPILATION UNIT.
 */
#include "wc1.h"

int           DAT_0059b470[512];
unsigned char DAT_00465058;
unsigned char DAT_0046a440;

extern void GetTbl0059ca94Fn2030(short i, unsigned char bits);
extern void GetTbl0059ca94Fn2050(short i, unsigned char bits);
extern void LoopG0059861cFn7610(void);     /* 0x00427610 family */
extern void LoopAudioSoundFn7548(void);    /* 0x00427548 */
extern void GetAudioVoiceFn73F3(void);     /* 0x004273F3 */
extern void LoopAudioStreamFnB300(void);   /* 0x0042B300 */

/* Function start: 0x418280 */
void GetTbl0059b470Fn8280(short i, short delta)
{
    DAT_0059b470[i] = DAT_0059b470[i] - (int)delta;
}

/* Function start: 0x418500 */
int DoLocalFn8500(int v)
{
    if (v < 0)
        v = -v;
    return v;
}

/* Function start: 0x4220D0 */
void DoLocalFn20D0(short i)
{
    GetTbl0059ca94Fn2030(i, 0x1a);
    GetTbl0059ca94Fn2050(i, 0x20);
}

/* Function start: 0x42B1B0 */
void GetAudioStreamFnB1B0(void)
{
    if (DAT_00465058 != 0 && DAT_0046a440 != 0) {
        LoopG0059861cFn7610();
        LoopAudioSoundFn7548();
        GetAudioVoiceFn73F3();
        LoopAudioStreamFnB300();
        DAT_0046a440 = 0;
    }
}
