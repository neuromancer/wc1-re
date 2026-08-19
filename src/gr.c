/*
 *  Rasteriser primitives and screen-space effects.
 *
 *  Address range 0x440c00-0x44274f (provisional -- see docs/ORDER.md).
 *  Boundary evidence: PROVEN by name: shadow_draw, fizzle_fade, snow_viewport.
 */
#include "wc1.h"

/* Function start: 0x440C00 */
void ValidateViewportBounds(Viewport *viewport, RasterSurface *surface,
                            RasterClip *clip)
{
    int allocation;
    int topOffset;
    int nextOffset;
    int rowStrideOffset;

    if (viewport->pixels != stScreen.pixels) {
        allocation = 0;
        while (allocation < nViewportAllocationCount) {
            if (apViewportAllocations[allocation] ==
                viewport->pixels)
                break;
            allocation++;
        }
        if (allocation >= nViewportAllocationCount)
            exit_squadron(szBadViewport);
    }
    if (viewport->pixels == stScreen.pixels)
        DIBslam();
    topOffset = SignExtendClipCoord(viewport->rowOffsets[viewport->top]);
    nextOffset = SignExtendClipCoord(
        viewport->rowOffsets[viewport->top + 1]);
    rowStrideOffset = SignExtendClipCoord(
        viewport->rowOffsets[viewport->top]);
    surface->pixels = viewport->pixels + viewport->left + topOffset;
    surface->maximumX = nextOffset - rowStrideOffset - 1;
    surface->maximumY = viewport->bottom - viewport->top;
    surface->field_C = 0;
    surface->field_10 = 0;
    clip->surface = surface;
    clip->left = 0;
    clip->top = 0;
    clip->right = viewport->right - viewport->left;
    clip->bottom = viewport->bottom - viewport->top;
}

/* Function start: 0x440CF0 */
void ClipViewportToScreen(Viewport *viewport)
{
    ValidateViewportBounds(viewport, &stRasterSurface,
                           &stRasterClip);
}

/* Function start: 0x440D10 */
void SetSolidColourTranslation(unsigned char colour)
{
    memset(abSolidColourTranslation, colour, 255);
    abSolidColourTranslation[255] = 0xff;
    SetPaletteTranslationTable(abSolidColourTranslation);
}

/* Function start: 0x440D50 */
void PrepareShapeRLEData(unsigned char *shape)
{
    RLEFrameHeader *frameHeader;
    unsigned char *bitmap;
    unsigned char *pixel;
    unsigned char *output;
    unsigned char *preparedShape;
    int *frameOffset;
    int preparedSize;
    int frameCount;
    short width;
    short height;
    short leftExtent;
    short topExtent;
    int frame;
    int row;
    int remaining;
    int runLength;
    int frameLeft;
    int frameTop;
    int frameRight;
    int frameBottom;

    CheckHeapBlockSignature(shape);
    if (GetPreparedShapeData(shape) != 0)
        return;

    *(int *)abShapeRLEScratch =
        *(const int *)szShapeRLEVersion;
    frameCount = GetShapeFrameCount(shape);
    *(int *)(abShapeRLEScratch + 4) = frameCount;
    memset(abShapeRLEScratch + 8, 0,
           (unsigned int)(frameCount << 3));
    frameOffset = (int *)(abShapeRLEScratch + 8);
    output = abShapeRLEScratch + 8 + (frameCount << 3);

    for (frame = 0; frame < frameCount; frame++) {
        *frameOffset = (int)(output - abShapeRLEScratch);
        frameOffset += 2;
        GetShapeFrameExtents(shape, (short)frame, &width, &height,
                             &leftExtent, &topExtent);
        frameHeader = (RLEFrameHeader *)output;
        frameHeader->height = height;
        frameHeader->width = width;
        frameHeader->topExtent = topExtent;
        frameHeader->leftExtent = leftExtent;
        frameLeft = -leftExtent;
        frameTop = -topExtent;
        frameRight = width - leftExtent - 1;
        frameBottom = height - topExtent - 1;
        frameHeader->left = frameLeft;
        frameHeader->top = frameTop;
        frameHeader->right = frameRight;
        frameHeader->bottom = frameBottom;
        output += sizeof(RLEFrameHeader);

        bitmap = AllocateTaggedMemory(
            (unsigned int)((int)width * height), 0);
        pixel = bitmap;
        memset(bitmap, 0xff, (unsigned int)((int)width * height));
        DecodeShapeFrame(shape, (short)frame, bitmap, width, height,
                         leftExtent, topExtent);
        for (row = 0; row < height; row++) {
            remaining = width;
            while (remaining > 0) {
                if (*pixel != 0xff) {
                    unsigned char *runCode;

                    runLength = 0;
                    runCode = output++;
                    while (remaining > 0 && runLength < 0x7f &&
                           *pixel != 0xff) {
                        *output = *pixel;
                        runLength++;
                        remaining--;
                        output++;
                        pixel++;
                    }
                    *runCode = (unsigned char)(runLength * 2 + 1);
                } else {
                    runLength = 0;
                    while (remaining > 0 && runLength < 0xff &&
                           *pixel == 0xff) {
                        runLength++;
                        remaining--;
                        pixel++;
                    }
                    *output++ = 1;
                    *output++ = (unsigned char)runLength;
                }
            }
            *output++ = 0;
        }
        ReleasePacketHandle(bitmap);
    }

    preparedSize = (int)(output - abShapeRLEScratch);
    if (preparedSize > (int)sizeof(abShapeRLEScratch))
        exit_squadron(szShapeRLEOverflow);
    preparedShape = AllocateTaggedMemory(preparedSize, 0);
    memcpy(preparedShape, abShapeRLEScratch, preparedSize);
#ifdef WC1_SDL
    *(unsigned char **)(shape - 8 - sizeof(unsigned char *)) = preparedShape;
#else
    *(unsigned char **)(shape - 4) = preparedShape;
#endif
}

/* Function start: 0x440FE0 */
void DrawSpriteTransformed(Viewport *viewport, int x, int y,
                           unsigned char *shape, int frame,
                           int angle, int scaleX, int scaleY,
                           int flip, int blendMode)
{
    if (shape != 0 && frame >= 0 && viewport->pixels != 0 &&
        viewport->rowOffsets != 0 && frame < GetShapeFrameCount(shape)) {
        PrepareShapeRLEData(shape);
        ClipViewportToScreen(viewport);
        if (flip != 0) {
            if (flip == 0x10) {
                scaleX = -scaleX;
            } else if (flip == 0x20) {
                scaleY = -scaleY;
            } else if (flip == 0x30) {
                scaleX = -scaleX;
                scaleY = -scaleY;
            } else {
                exit_squadron(szBadShapeFlip);
            }
        }
        if (blendMode != 0) {
            RotateRLEImage(&stRasterClip,
                           GetPreparedShapeData(shape), frame,
                           x - viewport->left, y - viewport->top,
                           abShapeTransformScratch,
                           angle * 10, scaleX * 256, scaleY * 256, 1);
        } else {
            RotateRLEImage(&stRasterClip,
                           GetPreparedShapeData(shape), frame,
                           x - viewport->left, y - viewport->top,
                           abShapeTransformScratch,
                           angle * 10, scaleX * 256, scaleY * 256, 0);
        }
    }
}

/* Function start: 0x441140 */
void RasterLineHook(const void *marker)
{
    (void)marker;
}

/* Function start: 0x441150 */
void DrawFontGlyph(char character, TextContext *context, int height,
                   int width, int y)
{
    unsigned char *font = context->font;
    Viewport *viewport = context->viewport;
    unsigned char colour = context->colour;
    unsigned char background = context->backgroundColour;
    unsigned char fontColour = font[2];
    unsigned char fontBackground = font[3];
    int row = y;
    unsigned char *characterData;
    unsigned char *source;
    unsigned char *destination;
    unsigned char translated;
    unsigned int bitmapOffset;
    unsigned int destinationOffset;
    short characterIndex;
    int column;

    abPaletteTranslation[fontColour] = colour;
    abPaletteTranslation[fontBackground] = background;
    characterIndex = (short)(signed char)character;
    if (characterIndex != 0x81 && characterIndex != 0x84 &&
        characterIndex != 0x8e && characterIndex != 0x94 &&
        characterIndex != 0x99 && characterIndex != 0x9a &&
        characterIndex != 0xe1) {
        characterData = font + characterIndex;
        bitmapOffset = ((unsigned int)characterData[0x204] << 8) +
                       characterData[0x104];
        source = font + bitmapOffset;
        if (fontColour == colour && fontBackground == background) {
            for (; height-- != 0; row++) {
                if (viewport->top == row &&
                    (viewport->rowOffsets[row] & 0x8000) != 0)
                    destinationOffset = (unsigned int)context->cursorX;
                else
                    destinationOffset = viewport->rowOffsets[row] +
                                        (unsigned int)context->cursorX;
                destination = viewport->pixels + destinationOffset;
                for (column = width; column-- != 0;) {
                    if (*source != 0xff)
                        *destination = *source;
                    source++;
                    destination++;
                }
            }
        } else {
            for (; height-- != 0; row++) {
                if (viewport->top == row &&
                    (viewport->rowOffsets[row] & 0x8000) != 0)
                    destinationOffset = (unsigned int)context->cursorX;
                else
                    destinationOffset = viewport->rowOffsets[row] +
                                        (unsigned int)context->cursorX;
                destination = viewport->pixels + destinationOffset;
                for (column = width; column-- != 0;) {
                    translated = abPaletteTranslation[*source];
                    if (translated != 0xff)
                        *destination = translated;
                    source++;
                    destination++;
                }
            }
        }
        context->cursorX += font[4 + characterIndex];
        abPaletteTranslation[fontColour] = fontColour;
        abPaletteTranslation[fontBackground] = fontBackground;
    }
}

/* Function start: 0x441370 */
void __stdcall MarkActivePaletteEntries(Viewport *viewport,
                                         unsigned char *active)
{
    unsigned short rgb[3];
    int index;

    (void)viewport;
    index = 0;
    do {
        GetPaletteEntry((short)index, rgb);
        if (rgb[0] != 0 || rgb[1] != 0 || rgb[2] != 0)
            active[index] = 1;
        index++;
    } while (index < 256);
}

/* Function start: 0x4413C0 */
void __stdcall GetPaletteEntry(short index, unsigned short *rgb)
{
    GetPaletteEntryAsWords(index, rgb);
}

/* Function start: 0x4413E0 */
void __stdcall SetPaletteEntry(short index, short *rgb)
{
    DIBsetPalette(index, rgb);
}

/* Function start: 0x441400 */
void DrawSpriteDefault(Viewport *viewport, short x, short y,
                       unsigned char *shape, short frame)
{
    if (shape != 0 && frame >= 0)
        DrawSpriteTransformed(viewport, x, y, shape, frame,
                              0, 0x100, 0x100, 0, 0);
}

/* Function start: 0x441450 */
void CaptureSpriteBackground(Viewport *viewport, unsigned char *background,
                             short x, short y, unsigned char *shape,
                             short frame)
{
    unsigned char *commands;
    unsigned char *saved;
    unsigned char *screen;
    unsigned char *pixels;
    unsigned char code;
    short left;
    short top;
    short right;
    short bottom;
    short copyLength;
    int frameOffset;
    int drawX;
    int drawY;
    int endX;
    int skip;
    unsigned short count;
    unsigned short runLength;
#ifdef WC1_SDL
    short coordinate;
#endif

    saved = background;
    if (background == 0)
        return;
    if (shape == 0)
        return;
    if (frame < 0)
        return;
    frameOffset = (short)(frame * 4 + 4);
    if ((int)*(unsigned short *)(shape + 4) <= frameOffset)
        return;
    left = viewport->left;
    right = viewport->right;
    top = viewport->top;
    bottom = viewport->bottom;
    commands = shape + *(int *)(shape + frameOffset) + 8;
#ifdef WC1_SDL
    memcpy(&count, commands, sizeof(count));
#else
    count = *(unsigned short *)commands;
#endif
    pixels = viewport->pixels;
    commands += 2;
    while (count != 0) {
#ifdef WC1_SDL
        memcpy(&coordinate, commands, sizeof(coordinate));
        drawX = x + coordinate;
        commands += 2;
        memcpy(&coordinate, commands, sizeof(coordinate));
        drawY = y + coordinate;
        commands += 2;
#else
        drawX = x + *(short *)commands;
        commands += 2;
        drawY = y + *(short *)commands;
        commands += 2;
#endif
        screen = pixels + (WC1_SPRITE_ROW_OFFSET(viewport, drawY) + drawX);
        if ((count & 1) != 0) {
            count >>= 1;
            while (count != 0) {
                code = *commands;
                commands++;
                if ((code & 1) != 0) {
                    runLength = (unsigned short)(code >> 1);
                    commands++;
                    count -= runLength;
                    endX = drawX + runLength - 1;
                    if (top <= drawY && bottom >= drawY && drawX <= right &&
                        left <= endX) {
                        copyLength = (short)runLength;
                        skip = 0;
                        if (drawX < left) {
                            copyLength = copyLength - left + drawX;
                            skip = left - drawX;
                        }
                        if (right < endX)
                            copyLength = copyLength - endX + right;
                        memcpy(saved, screen + skip, copyLength);
                        saved += copyLength;
                    }
                } else {
                    runLength = (unsigned short)(code >> 1);
                    count -= runLength;
                    endX = drawX + runLength - 1;
                    if (top <= drawY && bottom >= drawY && drawX <= right &&
                        left <= endX) {
                        copyLength = (short)runLength;
                        skip = 0;
                        if (drawX < left) {
                            copyLength = copyLength - left + drawX;
                            skip = left - drawX;
                        }
                        if (right < endX)
                            copyLength = copyLength - endX + right;
                        memcpy(saved, screen + skip, copyLength);
                        saved += copyLength;
                    }
                    commands += runLength;
                }
                drawX += runLength;
                screen += runLength;
            }
        } else {
            count >>= 1;
            endX = drawX + count - 1;
            if (top <= drawY && bottom >= drawY && drawX <= right &&
                left <= endX) {
                copyLength = (short)count;
                skip = 0;
                if (drawX < left) {
                    copyLength = copyLength - left + drawX;
                    skip = left - drawX;
                }
                if (right < endX)
                    copyLength = copyLength - endX + right;
                memcpy(saved, screen + skip, copyLength);
                saved += copyLength;
            }
            commands += count;
        }
#ifdef WC1_SDL
        memcpy(&count, commands, sizeof(count));
#else
        count = *(unsigned short *)commands;
#endif
        commands += 2;
    }
}

/* Function start: 0x441740 */
void RestoreSpriteBackground(Viewport *viewport, unsigned char *background,
                             short x, short y, unsigned char *shape,
                             short frame)
{
    unsigned char *commands;
    unsigned char *saved;
    unsigned char *screen;
    unsigned char *pixels;
    unsigned char code;
    short left;
    short top;
    short right;
    short bottom;
    short copyLength;
    int frameOffset;
    int drawX;
    int drawY;
    int endX;
    int skip;
    unsigned short count;
    unsigned short runLength;
#ifdef WC1_SDL
    short coordinate;
#endif

    saved = background;
    if (background == 0)
        return;
    if (shape == 0)
        return;
    if (frame < 0)
        return;
    frameOffset = (short)(frame * 4 + 4);
    if ((int)*(unsigned short *)(shape + 4) <= frameOffset)
        return;
    left = viewport->left;
    right = viewport->right;
    top = viewport->top;
    bottom = viewport->bottom;
    commands = shape + *(int *)(shape + frameOffset) + 8;
#ifdef WC1_SDL
    memcpy(&count, commands, sizeof(count));
#else
    count = *(unsigned short *)commands;
#endif
    pixels = viewport->pixels;
    commands += 2;
    while (count != 0) {
#ifdef WC1_SDL
        memcpy(&coordinate, commands, sizeof(coordinate));
        drawX = x + coordinate;
        commands += 2;
        memcpy(&coordinate, commands, sizeof(coordinate));
        drawY = y + coordinate;
        commands += 2;
#else
        drawX = x + *(short *)commands;
        commands += 2;
        drawY = y + *(short *)commands;
        commands += 2;
#endif
        screen = pixels + (WC1_SPRITE_ROW_OFFSET(viewport, drawY) + drawX);
        if ((count & 1) != 0) {
            count >>= 1;
            while (count != 0) {
                code = *commands;
                commands++;
                if ((code & 1) != 0) {
                    runLength = (unsigned short)(code >> 1);
                    commands++;
                    count -= runLength;
                    endX = drawX + runLength - 1;
                    if (top <= drawY && bottom >= drawY && drawX <= right &&
                        left <= endX) {
                        copyLength = (short)runLength;
                        skip = 0;
                        if (drawX < left) {
                            copyLength = copyLength - left + drawX;
                            skip = left - drawX;
                        }
                        if (right < endX)
                            copyLength = copyLength - endX + right;
                        memcpy(screen + skip, saved, copyLength);
                        saved += copyLength;
                    }
                } else {
                    runLength = (unsigned short)(code >> 1);
                    count -= runLength;
                    endX = drawX + runLength - 1;
                    if (top <= drawY && bottom >= drawY && drawX <= right &&
                        left <= endX) {
                        copyLength = (short)runLength;
                        skip = 0;
                        if (drawX < left) {
                            copyLength = copyLength - left + drawX;
                            skip = left - drawX;
                        }
                        if (right < endX)
                            copyLength = copyLength - endX + right;
                        memcpy(screen + skip, saved, copyLength);
                        saved += copyLength;
                    }
                    commands += runLength;
                }
                drawX += runLength;
                screen += runLength;
            }
        } else {
            count >>= 1;
            endX = drawX + count - 1;
            if (top <= drawY && bottom >= drawY && drawX <= right &&
                left <= endX) {
                copyLength = (short)count;
                skip = 0;
                if (drawX < left) {
                    copyLength = copyLength - left + drawX;
                    skip = left - drawX;
                }
                if (right < endX)
                    copyLength = copyLength - endX + right;
                memcpy(screen + skip, saved, copyLength);
                saved += copyLength;
            }
            commands += count;
        }
#ifdef WC1_SDL
        memcpy(&count, commands, sizeof(count));
#else
        count = *(unsigned short *)commands;
#endif
        commands += 2;
    }
    if (viewport->pixels == stScreen.pixels)
        DIBslam();
}

/* Function start: 0x441A40 */
void DrawSolidColourSprite(Viewport *viewport, short x, short y,
                           unsigned char *shape, short frame,
                           unsigned char colour)
{
    SetSolidColourTranslation(colour);
    DrawSpriteTransformed(viewport, x, y, shape, frame, 0,
                          0x100, 0x100, 0, 1);
}

/* Function start: 0x441A90 */
void CopyViewportContents(Viewport *source, Viewport *destination)
{
    RasterClip destinationClip;
    RasterClip sourceClip;
    RasterSurface destinationSurface;
    RasterSurface sourceSurface;

    ValidateViewportBounds(source, &sourceSurface, &sourceClip);
    ValidateViewportBounds(destination, &destinationSurface,
                           &destinationClip);
    BlitRasterClip(&sourceClip, 0, 0, &destinationClip, 0, 0,
                   0xffffffff);
}

/* Function start: 0x441AE0 */
void ClearViewport(Viewport *viewport, short colour)
{
    if (viewport->pixels != 0 && viewport->rowOffsets != 0) {
        ClipViewportToScreen(viewport);
        FillRasterClip(&stRasterClip, colour);
    }
    if (viewport == &stScreen) {
        DIBslam();
        DIBslamReal();
    }
}

/* Function start: 0x441B20 */
void DrawViewportPixel(Viewport *viewport, short x, short y,
                       short colour)
{
    ClipViewportToScreen(viewport);
    SetRasterClipPixel(&stRasterClip,
                       (int)x - viewport->left,
                       (int)y - viewport->top, colour);
}

/* Function start: 0x441B60 */
int GetViewportPixel(Viewport *viewport, short x, short y)
{
    ClipViewportToScreen(viewport);
    return ReadRasterClipPixel(&stRasterClip,
                               (int)x - viewport->left,
                               (int)y - viewport->top);
}

/* Function start: 0x441BA0 */
void DrawViewportLine(Viewport *viewport, short x1, short y1,
                      short x2, short y2, short colour)
{
    ClipViewportToScreen(viewport);
    DrawClippedLine(&stRasterClip,
                    x1 - viewport->left, y1 - viewport->top,
                    x2 - viewport->left, y2 - viewport->top,
                    0, colour);
}

/* Function start: 0x441BF0 */
void TriangleRasterizerHook(void)
{
}

/* Function start: 0x441C70 */
void DrawFilledViewportRect(Viewport *viewport, short left, short top,
                            short right, short bottom, short colour)
{
    int row;
    int height;

    height = bottom - top;
    ClipViewportToScreen(viewport);
    for (row = 0; row <= height; row++) {
        DrawClippedLine(&stRasterClip,
                        left - viewport->left,
                        row + top - viewport->top,
                        right - viewport->left,
                        row + top - viewport->top,
                        0, colour);
    }
}

/* Function start: 0x441CF0 */
void DrawViewportBorder(Viewport *viewport, short left, short top,
                        short right, short bottom, short colour)
{
    ClipViewportToScreen(viewport);
    DrawClippedLine(&stRasterClip,
                    left - viewport->left, top - viewport->top,
                    right - viewport->left, top - viewport->top,
                    0, colour);
    DrawClippedLine(&stRasterClip,
                    left - viewport->left, bottom - viewport->top,
                    right - viewport->left, bottom - viewport->top,
                    0, colour);
    DrawClippedLine(&stRasterClip,
                    left - viewport->left, top - viewport->top,
                    left - viewport->left, bottom - viewport->top,
                    0, colour);
    DrawClippedLine(&stRasterClip,
                    right - viewport->left, top - viewport->top,
                    right - viewport->left, bottom - viewport->top,
                    0, colour);
}

/* Function start: 0x441DD0 */
void DrawViewportEllipse(Viewport *viewport, short x, short y,
                         short verticalRadius, short horizontalRadius,
                         short colour)
{
    ClipViewportToScreen(viewport);
    DrawRasterEllipse(&stRasterClip, x, y,
                      horizontalRadius, verticalRadius, colour);
    if (viewport->pixels == stScreen.pixels)
        DIBslam();
}

/* Function start: 0x441E20 */
void FillViewportEllipse(Viewport *viewport, short x, short y,
                         short verticalRadius, short horizontalRadius,
                         short colour)
{
    ClipViewportToScreen(viewport);
    FillRasterEllipse(&stRasterClip, x, y,
                      horizontalRadius, verticalRadius, colour);
    if (viewport->pixels == stScreen.pixels)
        DIBslam();
}

/* Function start: 0x441E70 */
void DrawViewportEllipseShadow(Viewport *viewport, short x, short y,
                               short verticalRadius,
                               short horizontalRadius, short colour)
{
    ClipViewportToScreen(viewport);
    DrawRasterEllipse(&stRasterClip, x, y,
                      horizontalRadius, verticalRadius, colour);
    if (viewport->pixels == stScreen.pixels)
        DIBslam();
}

/* Function start: 0x441FC0 */
void DrawSpriteScaled(Viewport *viewport, short x, short y,
                      unsigned char *shape, short frame, short angle,
                      short scale, short flip)
{
    DrawSpriteTransformed(viewport, x, y, shape, frame, angle,
                          scale, scale, flip, 0);
}

/* Function start: 0x442000 */
void DrawSolidColourSpriteScaled(Viewport *viewport, short x, short y,
                                 unsigned char *shape, short frame,
                                 short angle, short scale, short flip,
                                 unsigned char colour)
{
    SetSolidColourTranslation(colour);
    DrawSpriteTransformed(viewport, x, y, shape, frame, angle,
                          scale, scale, flip, 1);
}

/* Function start: 0x442050 */
int GetTransformedShapeBounds(Viewport *viewport, short x, short y,
                              unsigned char *shape, short frame,
                              short angle, short scale, int flip,
                              short *bounds)
{
    short *frameData;
    int frameOffset;
    int leftExtent;
    int topExtent;
    int absoluteCosine;
    int absoluteSine;
    int horizontalExtent;
    int transformedHeight;
    int transformedWidth;
    short left;
    short top;
    short right;
    short bottom;

    if (shape == 0) {
        if (viewport->left <= x && x <= viewport->right &&
            viewport->top <= y && y <= viewport->bottom)
            return 1;
        return 0;
    }
    CheckHeapBlockSignature(shape);
    frameOffset = frame * 4 + 4;
    if (frameOffset <= (int)*(unsigned short *)(shape + 4)) {
        frameData = (short *)(shape + *(int *)(shape + frameOffset));
        leftExtent = frameData[1];
        topExtent = frameData[2];
        absoluteCosine =
            (int)(awAbsoluteCosine[angle] * scale) >> 8;
        absoluteSine =
            (int)(awAbsoluteSine[angle] * scale) >> 8;
        if (absoluteCosine == 0)
            absoluteCosine = 1;
        if (absoluteSine == 0)
            absoluteSine = 1;
        horizontalExtent = topExtent + frameData[3];
        transformedHeight =
            absoluteSine * (frameData[0] + leftExtent) +
            absoluteCosine * horizontalExtent;
        if ((char)transformedHeight != 0)
            transformedHeight += 0x100;
        transformedHeight >>= 8;
        transformedWidth =
            absoluteCosine * (frameData[0] + leftExtent) +
            absoluteSine * horizontalExtent;
        if ((char)transformedWidth != 0)
            transformedWidth += 0x100;
        transformedWidth >>= 8;
        if (absoluteCosine == 0)
            absoluteCosine = 1;
        if (absoluteSine == 0)
            absoluteSine = 1;
        top = (short)(y - (absoluteSine * leftExtent >> 8) -
                      (absoluteCosine * topExtent >> 8));
        bottom = (short)(transformedHeight + top);
        left = (short)(((absoluteSine * topExtent >> 8) -
                        (absoluteCosine * leftExtent >> 8) + x) -
                       ((absoluteSine * horizontalExtent >> 8) + 1));
        right = (short)(transformedWidth + left);
        if (viewport->left <= right && left <= viewport->right &&
            viewport->top <= bottom && top <= viewport->bottom) {
            bounds[0] = left;
            bounds[2] = right;
            bounds[1] = top;
            bounds[3] = bottom;
            return 1;
        }
    }
    (void)flip;
    return 0;
}

/* Function start: 0x442200 */
void fizzle_fade(Viewport *source, Viewport *destination,
                 const ScreenViewportGeometry *geometry)
{
    const short *run;
    unsigned char *sourcePixels;
    unsigned char *destinationPixels;
    unsigned short width;
    short sourceLeft;
    short sourceTop;
    short destinationX;
    short sourceY;

    if (source->pixels != 0 && destination->pixels != 0) {
        run = &geometry->originX;
        sourceLeft = *run++;
        sourceTop = *run++;
        destinationX = *run++;
        if (destinationX != -1) {
            do {
                sourceY = *run++;
                width = (unsigned short)*run++;
                sourcePixels = source->pixels +
                    source->rowOffsets[sourceY - sourceTop] - sourceLeft +
                    destinationX;
                destinationPixels = destination->pixels +
                    destination->rowOffsets[sourceY] + destinationX;
                memcpy(destinationPixels, sourcePixels, width);
                destinationX = *run++;
            } while (destinationX != -1);
        }
        if (destination->pixels == stScreen.pixels)
            DIBslam();
    }
}

/* Function start: 0x442300 */
void snow_viewport(Viewport *viewport, int effect, unsigned short colour)
{
    (void)effect;
    (void)colour;
    if (viewport->pixels == stScreen.pixels)
        DIBslam();
    RasterLineHook(szSnowViewport);
}

/* Function start: 0x442330 */
void UpdateStreamerStoppedFlag(void)
{
    if (bIxAudioEnabled != 0)
    nMusicTrackComplete =
        (dwStreamerState & 4) == 0;
}

/* Function start: 0x442350 */
void SignalAudioMixerWakeEvent(void)
{
    if (bIxAudioEnabled != 0)
        ix_streamer_configure(2, 0);
}

/* Function start: 0x442370 */
void InitializeAudioStreamer(HWND window)
{
    if (bIxAudioEnabled != 0) {
        ix_streamer_configure(3, (void *)1);
        ix_streamer_configure(0, window);
        ix_streamer_init();
        ix_streamer_set_intensity(0x19);
    }
}

/* Function start: 0x4423B0 */
void Streamer_open(const char *streamName)
{
    char *streamsDirectory;

    if (bIxAudioEnabled != 0) {
        streamsDirectory = LocateStreamsDirOnDisc();
        if (streamsDirectory == 0) {
            ShowNoticeMessageBox("Unable to locate streamed music.\n");
            exit_squadron("");
        }
        sprintf(szStreamerPath, "%s%s",
                streamsDirectory, streamName);
        SoundDebugPrintf("Streamer_open %s", szStreamerPath);
        ix_streamer_open_stream_file(szStreamerPath);
        bStreamerAudioPlaying = 0;
    }
}

/* Function start: 0x442430 */
void Streamer_play(void)
{
    if (bIxAudioEnabled != 0 &&
        bStreamerAudioPlaying == 0) {
        bStreamerAudioPlaying = 1;
        SoundDebugPrintf("Streamer_play");
        ix_streamer_audio_play();
    }
}

/* Function start: 0x442460 */
void Streamer_stop(void)
{
    if (bIxAudioEnabled != 0 &&
        bStreamerAudioPlaying != 0) {
        SoundDebugPrintf("Streamer_stop");
        ix_streamer_audio_stop();
        bStreamerAudioPlaying = 0;
    }
}

/* Function start: 0x4424B0 */
void ClearStreamerTrigger(void)
{
    if (bIxAudioEnabled != 0)
        ix_streamer_set_trigger(-1);
}

/* Function start: 0x4424D0 */
void Streamer_trigger(int trigger)
{
    if (bIxAudioEnabled != 0) {
        if (bStreamerAudioPlaying == 0) {
            ForceStreamerTrigger(trigger);
            return;
        }
        SoundDebugPrintf("Streamer_trigger %d", trigger);
        if (trigger >= 0)
            ix_streamer_set_trigger((char)trigger);
        if (bStreamerAudioPlaying == 0)
            Streamer_play();
    }
}

/* Function start: 0x442520 */
void SetStreamerIntensity(unsigned char intensity)
{
    if (bIxAudioEnabled != 0) {
        ix_streamer_set_intensity(intensity);
        if (bStreamerAudioPlaying == 0)
            Streamer_play();
    }
}

/* Function start: 0x442550 */
void ForceStreamerTrigger(int trigger)
{
    if (bIxAudioEnabled != 0) {
        SoundDebugPrintf("FORCE");
        if (trigger >= 0)
            ix_streamer_force_trigger((char)trigger);
        if (bStreamerAudioPlaying == 0)
            Streamer_play();
    }
}

/* Function start: 0x442590 */
void SetMusicStreamVolume(unsigned short volume)
{
    if (bIxAudioEnabled != 0)
        ix_streamer_set_volume(volume);
}

/* Function start: 0x4425D0 */
void Streamer_close(void)
{
    if (bIxAudioEnabled != 0) {
        SoundDebugPrintf("Streamer_close");
        ix_streamer_close_stream_file();
    }
}

/* Function start: 0x442600 */
int ReadCheaterFlagFromRegistry(void)
{
    HKEY key;
    DWORD type;
    DWORD size;
    int enabled = 0;

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "Software\\Origin Systems\\WC: Kilrathi Saga",
                      0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS) {
        type = REG_DWORD;
        size = sizeof(int);
        RegQueryValueExA(key, "Cheater", 0, &type,
                         (LPBYTE)&enabled, &size);
        RegCloseKey(key);
    }
    if (enabled != 0)
        OutputDebugStringA("reg cheating\n");
    else
        OutputDebugStringA("reg not cheating\n");
    return enabled;
}
