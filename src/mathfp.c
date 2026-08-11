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
void __stdcall SeedRandomFromClock(void)
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

/* Function start: 0x434E60 */
long ArcSin(int value)
{
    return (long)(asin((double)value * 0.00390625f) *
                  57.295779513082323);
}

/* Function start: 0x434E90 */
long ArcCos(int value)
{
    return (long)(acos((double)value * 0.00390625f) *
                  57.295779513082323);
}

/* Function start: 0x434EC0 */
long Magnitude(int value)
{
    return (long)(sqrt((double)value * 0.00390625f) * 256.0);
}

/* Function start: 0x434EE0 */
long PlanarMagnitude(int x, int y)
{
    double scaledX = (double)x * (1.0 / 256.0);
    double scaledY = (double)y * (1.0 / 256.0);

    scaledX *= scaledX;
    scaledY *= scaledY;
    return (long)(sqrt(scaledX + scaledY) * 256.0);
}

/* Function start: 0x434F20 */
long Vector_magnitude(const FixedVector *vector)
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
void __stdcall SetTextCursor(unsigned short a, unsigned short b)
{
    g_pCurrentTextContext_0059af8c->cursorX = (short)a;
    g_pCurrentTextContext_0059af8c->cursorY = (short)b;
}

/* Function start: 0x434FA0 */
void __stdcall SetTextContext(TextContext *context)
{
    g_pCurrentTextContext_0059af8c = context;
}

/* Function start: 0x434FB0 */
void WaitForVerticalBlankThunk(void)
{
    DIBwaitForVerticalBlank();
}

/* Function start: 0x434FC0 */
unsigned int __stdcall IdentityHandle(unsigned int v)
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
unsigned short __stdcall ReadWord(unsigned short *p)
{
    return *p;
}

/* Function start: 0x434FF0 */
unsigned short __stdcall GetFontCharWidth(char i)
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
void __stdcall SplitPackedPoint(ShortPoint point, short *p)
{
    IsPointInRect(point.x, point.y, p);
}

/* Function start: 0x4350F0 */
void __stdcall DrawTextString(char *text)
{
    char *cursor;
    char *lineStart;
    char value;
    int lineWidth;
    int right;
    int wrapped;
    int savedX;
    int finished;

    wrapped = 0;
    finished = 0;
    cursor = text;
    for (;;) {
        lineWidth = g_pCurrentTextContext_0059af8c->cursorX;
        while (*cursor == ' ')
            cursor++;
        lineStart = cursor;
        right = g_pCurrentTextContext_0059af8c->viewport->right;
        if (lineWidth < right) {
            for (;;) {
                value = *cursor;
                cursor++;
                if (value == '\n' || value == '\r')
                    break;
                if (value == 0) {
                    finished = 1;
                    break;
                }
                lineWidth +=
                    g_pCurrentTextContext_0059af8c->font[4 + value];
                if (lineWidth >= right) {
                    cursor--;
                    wrapped = 1;
                    lineWidth -=
                        g_pCurrentTextContext_0059af8c->font[4 + value];
                    if (*cursor != ' ') {
                        if (cursor <= text) {
                            SystemDebugPrintf(
                                "FATAL : INVALID STRING '%s'n", text);
                            ClearDebugPauseFlags();
                            PumpMessagesDuringWait();
                            exit(0);
                        }
                        do {
                            value = *cursor;
                            cursor--;
                            lineWidth -=
                                g_pCurrentTextContext_0059af8c
                                    ->font[4 + value];
                        } while (*cursor != ' ');
                        if (cursor <= text) {
                            SystemDebugPrintf(
                                "FATAL : INVALID STRING '%s'n", text);
                            ClearDebugPauseFlags();
                            PumpMessagesDuringWait();
                            exit(0);
                        }
                    }
                    break;
                }
            }
        }

        if (g_pCurrentTextContext_0059af8c->alignment == 2) {
            savedX = g_pCurrentTextContext_0059af8c->cursorX;
            g_pCurrentTextContext_0059af8c->cursorX = (short)(
                g_pCurrentTextContext_0059af8c->viewport->left +
                ((g_pCurrentTextContext_0059af8c->viewport->right -
                  g_pCurrentTextContext_0059af8c->viewport->left) -
                 lineWidth + savedX + 1) / 2);
        }
        while (lineStart < cursor) {
            DrawTextCharacter(*lineStart);
            lineStart++;
        }
        if (g_pCurrentTextContext_0059af8c->alignment == 2)
            g_pCurrentTextContext_0059af8c->cursorX = (short)savedX;
        if (wrapped != 0) {
            g_pCurrentTextContext_0059af8c->cursorX =
                g_pCurrentTextContext_0059af8c->viewport->left;
            wrapped = 0;
            g_pCurrentTextContext_0059af8c->cursorY +=
                *(short *)g_pCurrentTextContext_0059af8c->font;
        }
        if (finished != 0)
            return;
    }
}

/* Function start: 0x435290 */
void __stdcall DrawTextCharacter(char character)
{
    TextContext *context;
    unsigned char *font;
    short fontHeight;
    unsigned char glyphWidth;
    short cursorY;

    if (character == '\n') {
        g_pCurrentTextContext_0059af8c->cursorX =
            g_pCurrentTextContext_0059af8c->viewport->left;
        g_pCurrentTextContext_0059af8c->cursorY =
            (short)(g_pCurrentTextContext_0059af8c->cursorY +
                    *(short *)g_pCurrentTextContext_0059af8c->font);
    } else if (character == '\r') {
        g_pCurrentTextContext_0059af8c->cursorX =
            g_pCurrentTextContext_0059af8c->viewport->left;
    } else if (character != 0) {
        font = g_pCurrentTextContext_0059af8c->font;
        fontHeight = *(short *)font;
        glyphWidth = font[4 + (int)(signed char)character];
        context = g_pCurrentTextContext_0059af8c;
        cursorY = context->cursorY;
        DrawFontGlyph(character, context, fontHeight, glyphWidth, cursorY);
    }
}

/* Function start: 0x435310 */
void __stdcall AppendTextCharacter(char character)
{
    *g_pCurrentTextContext_0059af8c->textCursor = character;
    g_pCurrentTextContext_0059af8c->textCursor++;
    *g_pCurrentTextContext_0059af8c->textCursor = 0;
}

/* Function start: 0x435340 */
int __stdcall MeasureShapeFrameStorage(unsigned char *shape, short frame)
{
    unsigned char *run;
    unsigned short rowLength;
    unsigned char command;
    int frameTableOffset;
    int size;

    size = 0;
    if (shape != 0 && frame >= 0) {
        frameTableOffset = frame * 4 + 4;
        if (frameTableOffset < *(unsigned short *)(shape + 4)) {
            run = shape + *(int *)(shape + frameTableOffset) + 8;
            rowLength = *(unsigned short *)run;
            run += 2;
            while (rowLength != 0) {
                run += 4;
                if ((rowLength & 1) != 0) {
                    rowLength >>= 1;
                    while (rowLength != 0) {
                        command = *run;
                        run++;
                        if ((command & 1) != 0) {
                            command >>= 1;
                            run++;
                            rowLength =
                                (unsigned short)(rowLength - command);
                            size += command;
                        } else {
                            command >>= 1;
                            rowLength =
                                (unsigned short)(rowLength - command);
                            size += command;
                            run += command;
                        }
                    }
                } else {
                    rowLength >>= 1;
                    size += rowLength;
                    run += rowLength;
                }
                rowLength = *(unsigned short *)run;
                run += 2;
            }
        }
    }
    return size;
}

/* Function start: 0x4353F0 */
void ResetTextCursor(void)
{
    g_pCurrentTextContext_0059af8c->cursorX = 0;
    g_pCurrentTextContext_0059af8c->cursorY = 0;
}
