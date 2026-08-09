/*
 *  Floating-point helpers and the random-number generator.
 *
 *  Address range 0x434900-0x4353ff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: _ftol wrappers and rand()/srand() shims, contiguous and free of globals.
 */
#include "wc1.h"

/* Function start: 0x434CD0 */
unsigned short RandomBelow(short n) { return (short)(rand() % (int)n); }

/* Function start: 0x434CF0 */
void SeedRandomFromClock(void) { srand((unsigned int)time(0)); }

/* Function start: 0x434D20 */
short __stdcall RandomInRange(short lo, short hi)
{
    short span = hi - lo;

    if (span == 0)
        span = 1;
    return lo + rand() % (span + 1);
}

/* Function start: 0x434D50 */
short RandomBelowOrEqual(short n)
{
    if (n != -1 && n != 0)
        return (short)(rand() % (n + 1));
    return 0;
}

/* Function start: 0x434D80 */
long FloatToLong(void) { return _ftol(); }

/* Function start: 0x434E00 */
long SinFixed(short degrees) { return (long)sin((double)degrees * WC1_DEG2RAD); }

/* Function start: 0x434E30 */
long CosFixed(short degrees) { return (long)cos((double)degrees * WC1_DEG2RAD); }

/* Function start: 0x434EC0 */
long FloatToLongPassThrough(void) { return _ftol(); }

/* Function start: 0x434F70 */
void SetTextCursor(unsigned short a, unsigned short b)
{
    *(unsigned short *)(DAT_0059af8c + 4) = a;
    *(unsigned short *)(DAT_0059af8c + 6) = b;
}

/* Function start: 0x434FA0 */
void SetTextContext(unsigned int v) { DAT_0059af8c = v; }

/* Function start: 0x434FB0 */
void ReleaseDirectDrawPalette(void)
{
    (**(void (**)(void *, int, int))(*DAT_0046b1a4 + 0x58))(DAT_0046b1a4, 1, 0);
}

/* Function start: 0x434FC0 */
unsigned int IdentityHandle(unsigned int v) { return v; }

/* Function start: 0x434FD0 */
/* __stdcall forwarder that passes its argument through; `ret 4` is callee
 * cleanup.  The Ghidra label claimed a no-argument call, which was wrong. */
void __stdcall SetWholePaletteFromTriplets(unsigned char *palette)
{
    DIBwholePaletteFromTriplets(palette);
}

/* Function start: 0x434FE0 */
unsigned short ReadWord(unsigned short *p) { return *p; }

/* Function start: 0x434FF0 */
unsigned short GetFontCharWidth(char i)
{
    return *(unsigned char *)(i + 4 + *(int *)(DAT_0059af8c + 8));
}

/* Function start: 0x435010 */
void ReleaseVideoResourcesHook(void) {}

/* Function start: 0x4350D0 */
void SplitPackedPoint(unsigned int packed, short *p)
{
    DoLocalFn5090((short)packed, (short)(packed >> 16), p);
}

/* Function start: 0x4353F0 */
void ResetTextCursor(void)
{
    *(unsigned short *)(DAT_0059af8c + 4) = 0;
    *(unsigned short *)(DAT_0059af8c + 6) = 0;
}
