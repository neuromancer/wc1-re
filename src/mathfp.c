/*
 *  Floating-point helpers and the random-number generator.
 *
 *  Address range 0x434cd0-0x4353ff (provisional -- see docs/ORDER.md).
 *  The preceding smart unit ends at chase_speed (0x434c70).
 */
#include "wc1.h"

#pragma function(sin, cos, asin, acos, sqrt)

/* Function start: 0x4618E0 */
short RandomBelow(short n)
{
    return (short)(rand() % (int)n);
}

/* Function start: 0x4618FF */
void __stdcall SeedRandomFromClock(void)
{
    srand((unsigned int)time(0));
}

/* Function start: 0x461922 */
unsigned short GetRandomWord(void)
{
    return rand() & 0xffff;
}

/* Function start: 0x461942 */
short RandomInRange(unsigned short lo, unsigned short hi)
{
    short span;

    span = (short)(hi - lo);
    if (span == 0)
        span = 1;
    span = (short)(rand() % (span + 1));
    return (short)(span + lo);
}

/* Function start: 0x4619A1 */
short RandomBelowOrEqual(short n)
{
    if (n != -1 && n != 0)
        return (short)(rand() % (n + 1));
    return 0;
}

/* Function start: 0x4619E2 */
long MultiplyFixed(int left, int right)
{
    double leftValue = (double)left * (1.0 / 256.0);
    double rightValue = (double)right * (1.0 / 256.0);

    return (long)(leftValue * rightValue * 256.0);
}

/* Function start: 0x461A2D */
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

/* Function start: 0x461A91 */
long SinFixed(short degrees)
{
#if 0
    return (long)(sin((double)degrees * WC1_DEG2RAD) * 256.0);
#else
    float angle;
    long result;

    angle = (float)degrees;
    result = (long)(sin((double)angle * WC1_DEG2RAD) * 256.0);
    return result;
#endif
}

/* Function start: 0x461AD6 */
long CosFixed(short degrees)
{
#if 0
    return (long)(cos((double)degrees * WC1_DEG2RAD) * 256.0);
#else
    float angle;
    long result;

    angle = (float)degrees;
    result = (long)(cos((double)angle * WC1_DEG2RAD) * 256.0);
    return result;
#endif
}

/* Function start: 0x461B1B */
short ArcSin(int value)
{
#if 0
    return (long)(asin((double)value * 0.00390625f) *
                  57.295779513082323);
#else
    float scale;
    float normalized;
    short result;

    scale = 256.0f;
    normalized = (float)value / scale;
    result = (short)(asin((double)normalized) * 57.295779513082323);
    return result;
#endif
}

/* Function start: 0x461B6B */
short ArcCos(int value)
{
#if 0
    return (long)(acos((double)value * 0.00390625f) *
                  57.295779513082323);
#else
    float scale;
    float normalized;
    short result;

    scale = 256.0f;
    normalized = (float)value / scale;
    result = (short)(acos((double)normalized) * 57.295779513082323);
    return result;
#endif
}

/* Function start: 0x461BBB */
long Magnitude(int value)
{
#if 0
    return (long)(sqrt((double)value * 0.00390625f) * 256.0);
#else
    float scale;
    float normalized;
    long result;

    scale = 256.0f;
    normalized = (float)value / scale;
    result = (long)(sqrt((double)normalized) * scale);
    return result;
#endif
}

/* Function start: 0x461C06 */
long PlanarMagnitude(int x, int y)
{
    double scaledX = (double)x * (1.0 / 256.0);
    double scaledY = (double)y * (1.0 / 256.0);

    scaledX *= scaledX;
    scaledY *= scaledY;
    return (long)(sqrt(scaledX + scaledY) * 256.0);
}

/* Function start: 0x461C71 */
long Vector_magnitude(const FixedVector *vector)
{
#if 0
    double x = (double)vector->x * (1.0 / 256.0);
    double y = (double)vector->y * (1.0 / 256.0);
    double z = (double)vector->z * (1.0 / 256.0);

    x *= x;
    y *= y;
    z *= z;
    return (long)(sqrt(x + y + z) * 256.0);
#else
    float x = (float)vector->x / 256.0;
    float y = (float)vector->y / 256.0;
    float z = (float)vector->z / 256.0;

    x *= x;
    y *= y;
    z *= z;
    return (long)(sqrt(x + y + z) * 256.0);
#endif
}

/* Function start: 0x461D02 */
void SetTextCursor(unsigned short a, unsigned short b)
{
    g_pCurrentTextContext_005c8d1c->cursorX = (short)a;
    g_pCurrentTextContext_005c8d1c->cursorY = (short)b;
}

/* Function start: 0x461D29 */
void SetTextContext(TextContext *context)
{
    g_pCurrentTextContext_005c8d1c = context;
    if (g_pCurrentTextContext_005c8d1c != NULL &&
        g_pCurrentTextContext_005c8d1c->text != NULL)
        *g_pCurrentTextContext_005c8d1c->text = 0;
    return;
}

/* Function start: 0x461D68 */
void WaitForVerticalBlankThunk(void)
{
    DIBwaitForVerticalBlank();
}

/* Function start: 0x461D7D */
void *__stdcall IdentityHandle(void *v)
{
    return v;
}

/* Function start: 0x461D90 */
/* __stdcall forwarder that passes its argument through; `ret 4` is callee
 * cleanup.  The Ghidra label claimed a no-argument call, which was wrong. */
void __stdcall SetWholePaletteFromTriplets(unsigned char *palette)
{
    DIBwholePaletteFromTriplets(palette);
}

/* Function start: 0x461DA7 */
unsigned short __stdcall ReadWord(unsigned short *p)
{
    return *p;
}

/* Function start: 0x461DBD */
unsigned short __stdcall GetFontCharWidth(char i)
{
    return g_pCurrentTextContext_005c8d1c->font[4 + (int)i];
}

/* Function start: WC2_UNMAPPED */
void ReleaseVideoResourcesHook(void)
{
}

/* Function start: 0x461DF0 */
short GetShapeFrameBounds(short *bounds, short x, short y,
                          unsigned char *shape, short frame)
{
    short frameTableOffset;
#ifdef WC1_SDL
    short frameData[4];
#else
    short *frameData;
#endif

    frameTableOffset = (short)(frame * 4);
    if ((int)frameTableOffset < (int)*(unsigned short *)(shape + 4)) {
        frameTableOffset = (short)(frameTableOffset + 4);
#ifdef WC1_SDL
        memcpy(frameData,
               shape + *(unsigned short *)(shape + frameTableOffset),
               sizeof(frameData));
#else
        frameData = (short *)(shape +
            *(unsigned short *)(shape + frameTableOffset));
#endif
        bounds[2] = (short)(frameData[0] + x);
        bounds[0] = (short)(x - frameData[1]);
        bounds[1] = (short)(y - frameData[2]);
        bounds[3] = (short)(frameData[3] + y);
        return -1;
    }
    return 0;
}

/* Function start: 0x461E97 */
short __stdcall IsPointInRect(short x, short y, const short *rect)
{
    if (rect[0] <= x && x <= rect[2] &&
        rect[1] <= y && y <= rect[3])
        return 1;
    return 0;
}

/* Function start: 0x461EFE */
void __stdcall SplitPackedPoint(ShortPoint point, short *p)
{
    IsPointInRect(point.x, point.y, p);
}

/* Function start: 0x461F22 */
void DrawTextString(const char *text)
{
    const char *cursor;
    const char *lineStart;
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
        lineWidth = g_pCurrentTextContext_005c8d1c->cursorX;
        while (*cursor == ' ')
            cursor++;
        lineStart = cursor;
        right = g_pCurrentTextContext_005c8d1c->viewport->right;
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
                    g_pCurrentTextContext_005c8d1c->font[4 + value];
                if (lineWidth >= right) {
                    cursor--;
                    wrapped = 1;
                    lineWidth -=
                        g_pCurrentTextContext_005c8d1c->font[4 + value];
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
                                g_pCurrentTextContext_005c8d1c
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

        if (g_pCurrentTextContext_005c8d1c->alignment == 2) {
            savedX = g_pCurrentTextContext_005c8d1c->cursorX;
            g_pCurrentTextContext_005c8d1c->cursorX = (short)(
                g_pCurrentTextContext_005c8d1c->viewport->left +
                ((g_pCurrentTextContext_005c8d1c->viewport->right -
                  g_pCurrentTextContext_005c8d1c->viewport->left) -
                 lineWidth + savedX + 1) / 2);
        }
        while (lineStart < cursor) {
            DrawTextCharacter(*lineStart);
            lineStart++;
        }
        if (g_pCurrentTextContext_005c8d1c->alignment == 2)
            g_pCurrentTextContext_005c8d1c->cursorX = (short)savedX;
        if (wrapped != 0) {
            g_pCurrentTextContext_005c8d1c->cursorX =
                g_pCurrentTextContext_005c8d1c->viewport->left;
            wrapped = 0;
            g_pCurrentTextContext_005c8d1c->cursorY +=
                *(short *)g_pCurrentTextContext_005c8d1c->font;
        }
        if (finished != 0)
            return;
    }
}

/* Function start: 0x4621D5 */
void DrawTextCharacter(char character)
{
    TextContext *context;
    unsigned char *font;
    int fontHeight;
    unsigned int glyphWidth;
    int cursorY;

    if (g_pCurrentTextContext_005c8d1c == 0)
        return;
    if (character == '\n') {
        g_pCurrentTextContext_005c8d1c->cursorX =
            g_pCurrentTextContext_005c8d1c->viewport->left;
        g_pCurrentTextContext_005c8d1c->cursorY =
            (short)(g_pCurrentTextContext_005c8d1c->cursorY +
                    *(short *)g_pCurrentTextContext_005c8d1c->font);
    } else if (character == '\r') {
        g_pCurrentTextContext_005c8d1c->cursorX =
            g_pCurrentTextContext_005c8d1c->viewport->left;
    } else if (character != 0) {
        font = g_pCurrentTextContext_005c8d1c->font;
        fontHeight = *(short *)font;
        glyphWidth = font[4 + (int)(signed char)character];
        context = g_pCurrentTextContext_005c8d1c;
        cursorY = context->cursorY;
        DrawFontGlyph(character, context, fontHeight, glyphWidth, cursorY);
    }
}

/* Function start: 0x4622BD */
void AppendTextCharacter(char character)
{
    *g_pCurrentTextContext_005c8d1c->textCursor = character;
    g_pCurrentTextContext_005c8d1c->textCursor++;
    *g_pCurrentTextContext_005c8d1c->textCursor = 0;
    return;
}

/* Function start: WC2_UNMAPPED */
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

/* Function start: WC2_UNMAPPED */
void ResetTextCursor(void)
{
    g_pCurrentTextContext_005c8d1c->cursorX = 0;
    g_pCurrentTextContext_005c8d1c->cursorY = 0;
}
