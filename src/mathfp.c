/**
 * @file
 * @ingroup data
 * @brief Random numbers, fixed-point arithmetic and trigonometry computed in
 * floating point, and text-cursor and shape-frame helpers.
 *
 * Address range 434cd0-43540f. The preceding smart unit ends at chase_speed
 * (434c70).
 */
#include "wc1.h"

/* Linker commons defined here; LINK orders them by identifier hash. */
short nPacketDecompressPending; /* 0x0059ab36 */
unsigned short wPacketDecompressInputSize; /* 0x0059ab38 */
short nCurrentPacketDecompressScratchSegment; /* 0x0059ab3a */
volatile int bFrameTickPendingFlag; /* 0x0059ab3c */
int bMouseCursorDamagePending; /* 0x0059ab40 */
int nMouseCursorDamageRight; /* 0x0059ab44 */
int nEventMousePointerDamageBottom; /* 0x0059ab48 */
int nSavedDosInterruptVector; /* 0x0059ab4c */
short nViewportOriginPositionY; /* 0x0059ab50 */
short nViewportOriginPositionX; /* 0x0059ab52 */
int nTickCount60Hz; /* 0x0059ab54 */
unsigned char bEscapePressedFlag; /* 0x0059ab58 */
int nMouseCursorDamageLeft; /* 0x0059ab5c */
int nMousePointerDamageTopValue; /* 0x0059ab60 */

/* Function start: 0x434CD0 */
short RandomBelow(short n)
{
    return (rand() % n);
}

/* Function start: 0x434CF0 */
/* `ret 4` shows one stack argument; the clock seeds the generator and the
 * argument is never read. */
void __stdcall SeedRandomFromClock(int seed)
{
    srand(time(0));
}

/* Function start: 0x434D10 */
unsigned short RandomWord(void)
{
    return rand();
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
        return (rand() % (n + 1));
    return 0;
}

/* Function start: 0x434D80 */
long MultiplyFixed(int left, int right)
{
    double leftValue = left * (1.0 / 256.0);
    double rightValue = right * (1.0 / 256.0);

    return (leftValue * rightValue * 256.0);
}

/* Function start: 0x434DB0 */
long DivideFixed(int numerator, int denominator)
{
    float numeratorValue = (numerator * (1.0 / 256.0));
    float denominatorValue;

    if (denominator != 0)
        denominatorValue = (denominator * (1.0 / 256.0));
    else
        denominatorValue = 1.0f;
    return (numeratorValue / denominatorValue * 256.0);
}

/* Function start: 0x434E00 */
long SinFixed(short degrees)
{
    return (sin(degrees * DEGREES_TO_RADIANS) * 256.0);
}

/* Function start: 0x434E30 */
long CosFixed(short degrees)
{
    return (cos(degrees * DEGREES_TO_RADIANS) * 256.0);
}

/* Function start: 0x434E60 */
long ArcSin(int value)
{
    return (asin(value * 0.00390625f) *
                  57.295779513082323);
}

/* Function start: 0x434E90 */
long ArcCos(int value)
{
    return (acos(value * 0.00390625f) *
                  57.295779513082323);
}

/* Function start: 0x434EC0 */
long Magnitude(int value)
{
    return (sqrt(value * 0.00390625f) * 256.0);
}

/* Function start: 0x434EE0 */
long PlanarMagnitude(int x, int y)
{
    double scaledX = x * (1.0 / 256.0);
    double scaledY = y * (1.0 / 256.0);

    scaledX *= scaledX;
    scaledY *= scaledY;
    return (sqrt(scaledX + scaledY) * 256.0);
}

/* Function start: 0x434F20 */
long Vector_magnitude(const FixedVector *vector)
{
    double x = vector->x * (1.0 / 256.0);
    double y = vector->y * (1.0 / 256.0);
    double z = vector->z * (1.0 / 256.0);

    x *= x;
    y *= y;
    z *= z;
    return (sqrt(x + y + z) * 256.0);
}

/* Function start: 0x434F70 */
void __stdcall SetTextCursor(unsigned short a, unsigned short b)
{
    pCurTextContext->cursorX = a;
    pCurTextContext->cursorY = b;
}

/* Function start: 0x434FA0 */
void __stdcall SetTextContext(TextContext *context)
{
    pCurTextContext = context;
}

/* Function start: 0x434FB0 */
void WaitForVerticalBlankThunk(void)
{
    DIBwaitForVerticalBlank();
}

/* Function start: 0x434FC0 */
void *__stdcall IdentityHandle(void *v)
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
unsigned short __stdcall FontHeight(const void *p)
{
    return *(const unsigned short *)p;
}

/* Function start: 0x434FF0 */
unsigned short __stdcall GetFontCharWidth(char i)
{
#ifdef SDL_PORT
    /* The packed format reserves one width entry for every byte value. */
    return pCurTextContext->font[4 + (unsigned char)i];
#else
    return pCurTextContext->font[4 + i];
#endif
}

/* Function start: 0x435010 */
void ReleaseVideoResourcesHook(void)
{
}

/* Function start: 0x435020 */
short __stdcall GetShapeFrameBounds(short *bounds, short x, short y,
                                    struct Shape *shape, short frame)
{
    short frameTableOffset;
    struct ShapeFrame *frameData;

    frameTableOffset = (frame * 4);
    if (frameTableOffset < (unsigned short)shape->frameOffsets[0]) {
        frameTableOffset = (frameTableOffset + 4);
        shape = (struct Shape *)((unsigned char *)shape +
            *(unsigned short *)((unsigned char *)shape + frameTableOffset));
        frameData = (struct ShapeFrame *)shape;
        bounds[2] = (frameData->right + x);
        bounds[0] = (x - frameData->left);
        bounds[1] = (y - frameData->top);
        bounds[3] = (frameData->bottom + y);
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
void __stdcall DrawTextString(const char *text)
{
    const char *cursor;
    const char *lineStart;
    char value;
    int right;
    int lineWidth;
    int wrapped;
    int savedX;
    int finished;

    finished = 0;
    wrapped = 0;
    cursor = text;
    do {
        lineWidth = pCurTextContext->cursorX;
        while (*cursor == ' ')
            cursor++;
        lineStart = cursor;
        right = pCurTextContext->viewport->right;
        while (lineWidth < right) {
            value = *cursor;
            cursor++;
            if (value == '\n' || value == '\r')
                break;
            if (value == 0) {
                finished = 1;
                break;
            }
            lineWidth +=
#ifdef SDL_PORT
                pCurTextContext->font[4 + (unsigned char)value];
#else
                pCurTextContext->font[4 + value];
#endif
            if (lineWidth >= right) {
                /* Back up to the last space so the word wraps whole. */
                lineWidth -=
#ifdef SDL_PORT
                    pCurTextContext->font[4 + (unsigned char)value];
#else
                    pCurTextContext->font[4 + value];
#endif
                cursor--;
                wrapped = 1;
                if (*cursor != ' ') {
                    while (*cursor != ' ' && cursor > text) {
                        lineWidth -=
                            pCurTextContext
#ifdef SDL_PORT
                                ->font[4 + (unsigned char)*cursor];
#else
                                ->font[4 + *cursor];
#endif
                        cursor--;
                    }
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

        if (pCurTextContext->alignment == 2) {
            savedX = pCurTextContext->cursorX;
            pCurTextContext->cursorX = (
                pCurTextContext->viewport->left +
                ((pCurTextContext->viewport->right -
                  pCurTextContext->viewport->left) -
                 lineWidth + pCurTextContext->cursorX + 1) / 2);
        }
        while (lineStart < cursor) {
            DrawTextCharacter(*lineStart);
            lineStart++;
        }
        if (pCurTextContext->alignment == 2)
            pCurTextContext->cursorX = savedX;
        if (wrapped != 0) {
            pCurTextContext->cursorX =
                pCurTextContext->viewport->left;
            pCurTextContext->cursorY +=
                *(short *)pCurTextContext->font;
            wrapped = 0;
        }
    } while (finished == 0);
}

/* Function start: 0x435290 */
void __stdcall DrawTextCharacter(char character)
{
    TextContext *context;
    unsigned char *font;
    int fontHeight;
    unsigned int glyphWidth;
    int cursorY;

    if (character == '\n') {
        pCurTextContext->cursorX =
            pCurTextContext->viewport->left;
        pCurTextContext->cursorY =
            (pCurTextContext->cursorY +
                    *(short *)pCurTextContext->font);
    } else if (character == '\r') {
        pCurTextContext->cursorX =
            pCurTextContext->viewport->left;
    } else if (character != 0) {
        font = pCurTextContext->font;
        fontHeight = *(short *)font;
#ifdef SDL_PORT
        glyphWidth = font[4 + (unsigned char)character];
#else
        glyphWidth = font[4 + (signed char)character];
#endif
        context = pCurTextContext;
        cursorY = context->cursorY;
        DrawFontGlyph(character, context, fontHeight, glyphWidth, cursorY);
    }
}

/* Function start: 0x435310 */
void __stdcall AppendTextCharacter(char character)
{
    *pCurTextContext->textCursor = character;
    pCurTextContext->textCursor++;
    *pCurTextContext->textCursor = 0;
}

/* Function start: 0x435340 */
int __stdcall MeasureShapeFrameStorage(struct Shape *shape, short frame)
{
    unsigned char *run;
    unsigned short rowLength;
    unsigned char command;
    int frameTableOffset;
    int size;

    size = 0;
    if (shape != 0 && frame >= 0) {
        frameTableOffset = frame * 4 + 4;
        if (frameTableOffset < (unsigned short)shape->frameOffsets[0]) {
            run = ((struct ShapeFrame *)((unsigned char *)shape +
                *(int *)((unsigned char *)shape + frameTableOffset)))->commands;
#ifdef SDL_PORT
            memcpy(&rowLength, run, sizeof(rowLength));
#else
            rowLength = *(unsigned short *)run;
#endif
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
                                (rowLength - command);
                            size += command;
                        } else {
                            command >>= 1;
                            rowLength =
                                (rowLength - command);
                            size += command;
                            run += command;
                        }
                    }
                } else {
                    rowLength >>= 1;
                    size += rowLength;
                    run += rowLength;
                }
#ifdef SDL_PORT
                memcpy(&rowLength, run, sizeof(rowLength));
#else
                rowLength = *(unsigned short *)run;
#endif
                run += 2;
            }
        }
    }
    return size;
}

/* Function start: 0x4353F0 */
void ResetTextCursor(void)
{
    pCurTextContext->cursorX = 0;
    pCurTextContext->cursorY = 0;
}
