/*
 *  Floating-point helpers and the random-number generator.
 *
 *  Address range 0x434cd0-0x4353ff (provisional -- see docs/ORDER.md).
 *  The preceding smart unit ends at chase_speed (0x434c70).
 */
#include "wc1.h"

/* Function start: 0x434CD0 */
short RandomBelow(short n)
{
    return (short)(rand() % (int)n);
}

/* Function start: 0x434CF0 */
void SeedRandomFromClock(void)
{
    srand((unsigned int)time(0));
}

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
long MultiplyFixed(int left, int right)
{
    double leftValue = (double)left * (1.0 / 256.0);
    double rightValue = (double)right * (1.0 / 256.0);

    return (long)(leftValue * rightValue * 256.0);
}

/* Function start: 0x434DB0 */
long DivideFixed(int numerator, int denominator)
{
    float numeratorValue = (float)((double)numerator * (1.0 / 256.0));
    float denominatorValue;

    if (denominator != 0)
        denominatorValue = (float)((double)denominator * (1.0 / 256.0));
    else
        denominatorValue = 1.0f;
    return (long)(numeratorValue / denominatorValue * 256.0);
}

/* Function start: 0x434E00 */
long SinFixed(short degrees)
{
    return (long)sin((double)degrees * WC1_DEG2RAD);
}

/* Function start: 0x434E30 */
long CosFixed(short degrees)
{
    return (long)cos((double)degrees * WC1_DEG2RAD);
}

/* Function start: 0x434E90 */
long ArcCosFixed(int value)
{
    return (long)(acos((double)value * 0.00390625f) *
                  57.295779513082323);
}

/* Function start: 0x434EC0 */
long FloatToLongPassThrough(void)
{
    return _ftol();
}

/* Function start: 0x434F20 */
long ComputeFixedVectorMagnitude(const FixedVector *vector)
{
    double x = (double)vector->x * (1.0 / 256.0);
    double y = (double)vector->y * (1.0 / 256.0);
    double z = (double)vector->z * (1.0 / 256.0);

    x *= x;
    y *= y;
    z *= z;
    return (long)(sqrt(x + y + z) * 256.0);
}

/* Function start: 0x434F70 */
void SetTextCursor(unsigned short a, unsigned short b)
{
    *(unsigned short *)(DAT_0059af8c + 4) = a;
    *(unsigned short *)(DAT_0059af8c + 6) = b;
}

/* Function start: 0x434FA0 */
void SetTextContext(unsigned int v)
{
    DAT_0059af8c = v;
}

/* Function start: 0x434FB0 */
__declspec(naked) void WaitForVerticalBlankThunk(void)
{
    __asm { jmp DIBwaitForVerticalBlank }
}

/* Function start: 0x434FC0 */
unsigned int IdentityHandle(unsigned int v)
{
    return v;
}

/* Function start: 0x434FD0 */
/* __stdcall forwarder that passes its argument through; `ret 4` is callee
 * cleanup.  The Ghidra label claimed a no-argument call, which was wrong. */
void __stdcall SetWholePaletteFromTriplets(unsigned char *palette)
{
    DIBwholePaletteFromTriplets(palette);
}

/* Function start: 0x434FE0 */
unsigned short ReadWord(unsigned short *p)
{
    return *p;
}

/* Function start: 0x434FF0 */
unsigned short GetFontCharWidth(char i)
{
    return *(unsigned char *)(i + 4 + *(int *)(DAT_0059af8c + 8));
}

/* Function start: 0x435010 */
void ReleaseVideoResourcesHook(void)
{
}

/* Function start: 0x435090 */
short __stdcall IsPointInRect(short x, short y, const short *rect)
{
    if (rect[0] <= x && x <= rect[2] &&
        rect[1] <= y && y <= rect[3])
        return 1;
    return 0;
}

/* Function start: 0x4350D0 */
void SplitPackedPoint(unsigned int packed, short *p)
{
    IsPointInRect((short)packed, (short)(packed >> 16), p);
}

/* Function start: 0x4353F0 */
void ResetTextCursor(void)
{
    *(unsigned short *)(DAT_0059af8c + 4) = 0;
    *(unsigned short *)(DAT_0059af8c + 6) = 0;
}
