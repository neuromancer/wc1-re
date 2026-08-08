/*
 *  Floating-point and CRT-intrinsic helpers.
 *  NOT A REAL COMPILATION UNIT, though 0x00434D80-0x004354F0 is a tight cluster
 *  and may well be one.
 *
 *  All the trig wrappers end in _ftol, whose argument arrives on the x87 stack.
 */
#include "wc1.h"
#include <math.h>
#include <string.h>

/* 0.017453292519943295 == PI/180: these take degrees. */
#define WC1_DEG2RAD 0.017453292519943295

extern long _ftol(void);

/* Function start: 0x434D80 */
long FloatToLongA(void) { return _ftol(); }

/* Function start: 0x434E00 */
long CalcVideoFn4E00(short degrees) { return (long)sin((double)degrees * WC1_DEG2RAD); }

/* Function start: 0x434E30 */
long CalcVideoFn4E30(short degrees) { return (long)cos((double)degrees * WC1_DEG2RAD); }

/* Function start: 0x4354D0 */
short LoopLocalFn54D0(const char *s) { return (short)strlen(s); }

/* Function start: 0x4354F0 */
void DoLocalFn54F0(void *dst, const void *src, size_t n) { memcpy(dst, src, n); }

/* Function start: 0x433020 */
void SetTbl00486112Fn3020(short i, unsigned short *rgb)
{
    extern unsigned char DAT_00486110[];
    int k = i * 4;

    rgb[0] = DAT_00486110[k + 2];
    rgb[1] = DAT_00486110[k + 1];
    rgb[2] = DAT_00486110[k];
}
