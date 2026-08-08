/*
 *  Fourth leaf tranche.  NOT A REAL COMPILATION UNIT (see accessors_scattered.c).
 */
#include "wc1.h"
#include <io.h>

unsigned char  DAT_0059d100[512];
short          DAT_00465460;
unsigned short DAT_0059d500[2048];
unsigned char  DAT_0046aa3c;
unsigned char  DAT_0046aa40;
unsigned char  DAT_0046aa38;
unsigned short DAT_005a6baa;
unsigned short DAT_005a6bae;
unsigned char  DAT_0046c03c;
unsigned short DAT_005a7780;
unsigned int   DAT_005a7c2c;
unsigned int   DAT_00469e34;
unsigned char  DAT_0046aa30;
unsigned char  DAT_005a76b0;
unsigned short DAT_0059d2d0[512];
unsigned int   DAT_0059b430[512];
unsigned char  DAT_0059cf20[512];

extern char           DAT_0059dec0[];
extern unsigned char  DAT_0059ca94[];
extern unsigned char  DAT_005a7dca;
extern unsigned char  DAT_0059a850;
extern unsigned int   DAT_0059af8c;
extern void          *DAT_0046b1a4;

extern void PlaySfxWaveFileByNumber(int n, int a, int b);
extern void DoUiFn5590(void);
extern void ReleaseTbl005a7f10FnF940(void *p);         /* 0x0042F940-family */
extern void GetSfxFnB640(void);
extern void GetG005a8760Fn3DB0(const char *fmt, ...);
extern void DoLocalFnE330(void);                       /* 0x0042E330 */

/* Function start: 0x401930 */
void GetTbl0059d100Fn1930(short i)
{
    if (DAT_0059d100[i] == 4)
        DAT_0059d100[i] = 0;
}

/* Function start: 0x403500 */
void DoUiFn3500(unsigned int fd) { DAT_00465460 = (short)_close(fd & 0xffff); }

/* Function start: 0x4147E0 */
unsigned short GetTbl0059dec0Fn47E0(short i)
{
    return DAT_0059d500[((int)DAT_0059dec0[i] + i * 4) * 2];
}

/* Function start: 0x4149F0 */
int GetCombatFn49F0(void) { return 0 < DAT_005a7dca; }

/* Function start: 0x414AE0 */
void ForwardPlaySfxWaveFileByNumberv1(void) { PlaySfxWaveFileByNumber(0x1f, -1, 0); }

/* Function start: 0x417F00 */
void ForwardPlaySfxWaveFileByNumberv2(void) { PlaySfxWaveFileByNumber(0x19, -1, 0); }

/* Function start: 0x4184E0 */
int DoLocalFn84E0(int a, int b) { if (a <= b) b = a; return b; }

/* Function start: 0x4184F0 */
int DoLocalFn84F0(int a, int b) { if (b <= a) b = a; return b; }

/* Function start: 0x4185F0 */
void DoLocalFn85F0(unsigned int *p) { p[2] = 0; p[1] = 0; p[0] = 0; }

/* Function start: 0x421A40 */
unsigned int GetUiFn1A40(void)
{
    if (DAT_0059a850 != 0)
        DoUiFn5590();
    return 0;
}

/* Function start: 0x422030 */
void GetTbl0059ca94Fn2030(short i, unsigned char bits) { DAT_0059ca94[i] &= ~bits; }

/* Function start: 0x422140 */
unsigned int GetShipAiFn2140(short i, unsigned int bits)
{
    DAT_0059b430[i] |= bits;
    return bits & 0xffff0000;
}

/* Function start: 0x422160 */
void SetTbl0059cf20Fn2160(short i)
{
    DAT_0059d2d0[i] = 0;
    DAT_0059b430[i] = 0;
    DAT_0059cf20[i] = 0xff;
}

/* Function start: 0x424C60 */
unsigned int SetSceneFn4C60(void)
{
    DAT_0046aa3c = 0;
    DAT_0046aa40 = 1;
    DAT_0046aa38 = 1;
    return 0;
}

/* Function start: 0x427BA0 */
unsigned int SetUiFn7BA0(void)
{
    ReleaseTbl005a7f10FnF940(&DAT_005a76b0);
    DAT_005a6baa = 0;
    DAT_005a6bae = 199;
    return 0;
}

/* Function start: 0x427C80 */
unsigned int GetG0046c03cFn7C80(void)
{
    if (DAT_0046c03c < 4)
        DAT_005a7780 = 0x30;
    return 0;
}

/* Function start: 0x429E70 */
void GetUiFn9E70(void) { DAT_005a7c2c = (DAT_00469e34 + 6) * 400; }

/* Function start: 0x42E3A0 */
void DoLocalFnE3A0(void) { GetSfxFnB640(); GetG005a8760Fn3DB0("FlushSoundEffect"); }

/* Function start: 0x42E3C0 */
void DoLocalFnE3C0(void) { GetSfxFnB640(); GetG005a8760Fn3DB0("FlushSoundEffects"); }

/* Function start: 0x42EEE0 */
void DoSceneFnEEE0(void) { DAT_0046aa30 = 1; DoLocalFnE330(); }

/* Function start: 0x4331E0 */
void GetVideoFn31E0(void)
{
    (*(void (**)(void *, int, int))(*(int **)DAT_0046b1a4 + 0x58 / 4))(DAT_0046b1a4, 1, 0);
}

/* Function start: 0x434FF0 */
unsigned short UiRoutine09(char i)
{
    return *(unsigned char *)(i + 4 + *(int *)(DAT_0059af8c + 8));
}
