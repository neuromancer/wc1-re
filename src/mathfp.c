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
    return (long)(sin((double)degrees * WC1_DEG2RAD) * 256.0);
}

/* Function start: 0x434E30 */
long CosFixed(short degrees)
{
    return (long)(cos((double)degrees * WC1_DEG2RAD) * 256.0);
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
    g_pCurrentTextContext_0059af8c->cursorX = (short)a;
    g_pCurrentTextContext_0059af8c->cursorY = (short)b;
}

/* Function start: 0x434FA0 */
void SetTextContext(TextContext *context)
{
    g_pCurrentTextContext_0059af8c = context;
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
    return g_pCurrentTextContext_0059af8c->font[4 + (int)i];
}

/* Function start: 0x435010 */
void ReleaseVideoResourcesHook(void)
{
}

/* Function start: 0x435020 */
short __stdcall GetShapeFrameBounds(short *bounds, short x, short y,
                                    unsigned char *shape, short frame)
{
    short frameTableOffset;
    short *frameData;

    frameTableOffset = (short)(frame * 4);
    if ((int)frameTableOffset < (int)*(unsigned short *)(shape + 4)) {
        frameTableOffset = (short)(frameTableOffset + 4);
        frameData = (short *)(shape +
            *(unsigned short *)(shape + frameTableOffset));
        bounds[2] = (short)(frameData[0] + x);
        bounds[0] = (short)(x - frameData[1]);
        bounds[1] = (short)(y - frameData[2]);
        bounds[3] = (short)(frameData[3] + y);
        return -1;
    }
    return 0;
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

/* Function start: 0x4350F0 */
void DrawTextString(char *text)
{
    TextContext *context = g_pCurrentTextContext_0059af8c;
    char *cursor = text;
    int lineWidth;
    int savedX = 0;
    int wrapped = 0;
    int finished = 0;

    for (;;) {
        char *lineStart;
        char *scan;

        lineWidth = context->cursorX;
        while (*cursor == ' ')
            cursor++;
        lineStart = cursor;
        scan = cursor;

        if (lineWidth < context->viewport->right) {
            for (;;) {
                char *character = scan;
                signed char value = (signed char)*character;

                scan = character + 1;
                if (value == '\n' || value == '\r')
                    break;
                if (value == 0) {
                    finished = 1;
                    break;
                }
                lineWidth += context->font[4 + (int)value];
                if (lineWidth >= context->viewport->right) {
                    lineWidth -= context->font[4 + (int)value];
                    scan = character;
                    wrapped = 1;
                    if (*character != ' ') {
                        do {
                            signed char previous;

                            if (character <= text) {
                                SystemDebugPrintf(0);
                                ClearDebugPauseFlags();
                                PumpMessagesDuringWait();
                                exit(0);
                            }
                            previous = (signed char)*character;
                            character--;
                            lineWidth -= context->font[4 + (int)previous];
                        } while (*character != ' ');
                        scan = character;
                        if (character <= text) {
                            SystemDebugPrintf(0);
                            ClearDebugPauseFlags();
                            PumpMessagesDuringWait();
                            exit(0);
                        }
                    }
                    break;
                }
            }
        }

        if (context->alignment == 2) {
            savedX = context->cursorX;
            context->cursorX = (short)(context->viewport->left +
                (((context->viewport->right - context->viewport->left) -
                  lineWidth + 1 + savedX) / 2));
        }
        while (lineStart < scan) {
            DrawTextCharacter(*lineStart);
            lineStart++;
        }
        if (context->alignment == 2)
            context->cursorX = (short)savedX;
        if (wrapped) {
            context->cursorX = context->viewport->left;
            context->cursorY = (short)(context->cursorY +
                *(short *)context->font);
            wrapped = 0;
        }
        if (finished)
            return;
        cursor = scan;
    }
}

/* Function start: 0x435290 */
void __stdcall DrawTextCharacter(char character)
{
    TextContext *context = g_pCurrentTextContext_0059af8c;

    if (character == '\n') {
        context->cursorX = context->viewport->left;
        context->cursorY = (short)(context->cursorY +
            *(short *)context->font);
    } else if (character == '\r') {
        context->cursorX = context->viewport->left;
    } else if (character != 0) {
        DrawFontGlyph(character, context, *(short *)context->font,
                      context->font[4 + (int)(signed char)character],
                      context->cursorY);
    }
}

/* Function start: 0x4353F0 */
void ResetTextCursor(void)
{
    g_pCurrentTextContext_0059af8c->cursorX = 0;
    g_pCurrentTextContext_0059af8c->cursorY = 0;
}
