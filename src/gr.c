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
    unsigned int topOffset;
    unsigned int nextOffset;
    unsigned int rowStrideOffset;

    if (viewport->pixels != DAT_005a6ba0.pixels) {
        allocation = 0;
        while (allocation < g_nViewportAllocationCount_005a7f0c) {
            if (g_apViewportAllocations_005a7f10[allocation] ==
                viewport->pixels)
                break;
            allocation++;
        }
        if (allocation >= g_nViewportAllocationCount_005a7f0c)
            exit_squadron(g_szBadViewport_00470d24);
    }
    if (viewport->pixels == DAT_005a6ba0.pixels)
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
    ValidateViewportBounds(viewport, &g_stRasterSurface_004875a8,
                           &g_stRasterClip_00496fc0);
}

/* Function start: 0x440D10 */
void SetSolidColourTranslation(unsigned char colour)
{
    memset(g_abSolidColourTranslation_00497648, colour, 255);
    g_abSolidColourTranslation_00497648[255] = 0xff;
    SetPaletteTranslationTable(g_abSolidColourTranslation_00497648);
}

/* Function start: 0x440FE0 */
void DrawSpriteTransformed(Viewport *viewport, int x, int y,
                           unsigned char *shape, int frame,
                           int angle, int scaleX, int scaleY,
                           int flip, int blendMode)
{
    unsigned char *frameData;
    unsigned char *commands;
    unsigned char *bitmap;
    unsigned short rowCode;
    int frameOffset;
    int firstFrameOffset;
    int minX;
    int maxX;
    int minY;
    int maxY;
    int sourceWidth;
    int sourceHeight;
    int drawX;
    int drawY;
    int remaining;
    int runLength;
    int code;
    int sine;
    int cosine;
    int radius;
    int maximumX;
    int maximumY;
    int destinationX;
    int destinationY;
    int sourceX;
    int sourceY;
    int absoluteScaleX;
    int absoluteScaleY;
    unsigned char colour;

    if (viewport == 0 || shape == 0 || frame < 0 ||
        viewport->pixels == 0 || viewport->rowOffsets == 0)
        return;
    frameOffset = (short)(frame * 4 + 4);
    firstFrameOffset = *(int *)(shape + 4) & 0xffffff;
    if (frameOffset >= firstFrameOffset)
        return;
    frameData = shape + (*(int *)(shape + frameOffset) & 0xffffff);
    commands = frameData + 8;
    minX = 0x7fff;
    maxX = -0x7fff;
    minY = 0x7fff;
    maxY = -0x7fff;
    rowCode = *(unsigned short *)commands;
    while (rowCode != 0) {
        drawX = *(short *)(commands + 2);
        drawY = *(short *)(commands + 4);
        commands += 6;
        runLength = rowCode >> 1;
        if (drawX < minX)
            minX = drawX;
        if (drawX + runLength - 1 > maxX)
            maxX = drawX + runLength - 1;
        if (drawY < minY)
            minY = drawY;
        if (drawY > maxY)
            maxY = drawY;
        if ((rowCode & 1) == 0) {
            commands += runLength;
        } else {
            remaining = runLength;
            while (remaining != 0) {
                code = *commands;
                runLength = code >> 1;
                if (runLength <= 0)
                    return;
                if ((code & 1) == 0)
                    commands += runLength + 1;
                else
                    commands += 2;
                remaining -= runLength;
            }
        }
        rowCode = *(unsigned short *)commands;
    }
    if (maxX < minX || maxY < minY)
        return;
    sourceWidth = maxX - minX + 1;
    sourceHeight = maxY - minY + 1;
    bitmap = (unsigned char *)malloc(sourceWidth * sourceHeight);
    if (bitmap == 0)
        return;
    memset(bitmap, 0xff, sourceWidth * sourceHeight);

    commands = frameData + 8;
    rowCode = *(unsigned short *)commands;
    while (rowCode != 0) {
        drawX = *(short *)(commands + 2);
        drawY = *(short *)(commands + 4);
        commands += 6;
        if ((rowCode & 1) == 0) {
            runLength = rowCode >> 1;
            memcpy(bitmap + (drawY - minY) * sourceWidth + drawX - minX,
                   commands, runLength);
            commands += runLength;
        } else {
            remaining = rowCode >> 1;
            while (remaining != 0) {
                code = *commands;
                runLength = code >> 1;
                if (runLength <= 0) {
                    free(bitmap);
                    return;
                }
                if ((code & 1) == 0) {
                    memcpy(bitmap + (drawY - minY) * sourceWidth +
                               drawX - minX,
                           commands + 1, runLength);
                    commands += runLength + 1;
                } else {
                    memset(bitmap + (drawY - minY) * sourceWidth +
                               drawX - minX,
                           commands[1], runLength);
                    commands += 2;
                }
                drawX += runLength;
                remaining -= runLength;
            }
        }
        rowCode = *(unsigned short *)commands;
    }

    if (scaleX == 0)
        scaleX = 1;
    if (scaleY == 0)
        scaleY = 1;
    absoluteScaleX = AbsInt(scaleX);
    absoluteScaleY = AbsInt(scaleY);
    maximumX = MaxInt(AbsInt(minX), AbsInt(maxX));
    maximumY = MaxInt(AbsInt(minY), AbsInt(maxY));
    radius = (maximumX * absoluteScaleX +
              maximumY * absoluteScaleY) / 0x100 + 3;
    if (radius > (viewport->right - viewport->left + 1) +
                 (viewport->bottom - viewport->top + 1))
        radius = (viewport->right - viewport->left + 1) +
                 (viewport->bottom - viewport->top + 1);
    sine = (int)SinFixed((short)angle);
    cosine = (int)CosFixed((short)angle);
    destinationY = -radius;
    while (destinationY <= radius) {
        drawY = y + destinationY;
        if (drawY >= viewport->top && drawY <= viewport->bottom) {
            destinationX = -radius;
            while (destinationX <= radius) {
                drawX = x + destinationX;
                if (drawX >= viewport->left && drawX <= viewport->right) {
                    sourceX = (destinationX * cosine +
                               destinationY * sine) / scaleX;
                    sourceY = (-destinationX * sine +
                               destinationY * cosine) / scaleY;
                    if ((flip & 0x10) != 0)
                        sourceX = -sourceX;
                    if ((flip & 0x20) != 0)
                        sourceY = -sourceY;
                    if (sourceX >= minX && sourceX <= maxX &&
                        sourceY >= minY && sourceY <= maxY) {
                        colour = bitmap[(sourceY - minY) * sourceWidth +
                                        sourceX - minX];
                        if (colour != 0xff && blendMode != 0)
                            colour =
                                g_abRasterPaletteTranslation_0046ff2c[colour];
                        if (colour != 0xff)
                            viewport->pixels[viewport->rowOffsets[drawY] +
                                             drawX] = colour;
                    }
                }
                destinationX++;
            }
        }
        destinationY++;
    }
    free(bitmap);
}

/* Function start: 0x441140 */
void RasterLineHook(void *marker)
{
    (void)marker;
}

/* Function start: 0x441150 */
void DrawFontGlyph(char character, TextContext *context, int height,
                   int width, int y)
{
    Viewport *viewport = context->viewport;
    unsigned char *font = context->font;
    unsigned char fontColour = font[2];
    unsigned char fontBackground = font[3];
    unsigned char oldColour = g_abPaletteTranslation_00470678[fontColour];
    unsigned char oldBackground =
        g_abPaletteTranslation_00470678[fontBackground];
    short characterIndex = (short)(signed char)character;
    unsigned char *source;
    int row;

    g_abPaletteTranslation_00470678[fontColour] = context->colour;
    g_abPaletteTranslation_00470678[fontBackground] =
        context->backgroundColour;

    if (characterIndex != 0x81 && characterIndex != 0x84 &&
        characterIndex != 0x8e && characterIndex != 0x94 &&
        characterIndex != 0x99 && characterIndex != 0x9a &&
        characterIndex != 0xe1) {
        unsigned char *characterData = font + characterIndex;
        unsigned int bitmapOffset =
            ((unsigned int)characterData[0x204] << 8) |
            characterData[0x104];

        source = font + bitmapOffset;
        row = y;
        while (height != 0) {
            unsigned int destinationOffset;
            unsigned short rowOffset = viewport->rowOffsets[row];
            unsigned char *destination;
            int column = width;

            if (viewport->top == row && (rowOffset & 0x8000) != 0)
                destinationOffset = (unsigned int)(short)context->cursorX;
            else
                destinationOffset = (unsigned int)rowOffset +
                    (unsigned int)(short)context->cursorX;
            destination = viewport->pixels + destinationOffset;

            if (fontColour == context->colour &&
                fontBackground == context->backgroundColour) {
                while (column != 0) {
                    if (*source != 0xff)
                        *destination = *source;
                    source++;
                    destination++;
                    column--;
                }
            } else {
                while (column != 0) {
                    unsigned char colour =
                        g_abPaletteTranslation_00470678[*source];

                    if (colour != 0xff)
                        *destination = colour;
                    source++;
                    destination++;
                    column--;
                }
            }
            row++;
            height--;
        }
        context->cursorX = (short)(context->cursorX +
            font[4 + characterIndex]);
    }

    g_abPaletteTranslation_00470678[fontColour] = oldColour;
    g_abPaletteTranslation_00470678[fontBackground] = oldBackground;
}

/* Function start: 0x441370 */
void __stdcall MarkActivePaletteEntries(Viewport *viewport,
                                         unsigned char *active)
{
    unsigned short rgb[3];
    short index;

    (void)viewport;
    index = 0;
    do {
        GetPaletteEntry(index, rgb);
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
    unsigned char *saved = background;
    unsigned char *commands;
    unsigned short rowCode;
    int frameOffset;

    if (background == 0 || shape == 0 || frame < 0)
        return;
    frameOffset = (short)(frame * 4 + 4);
    if (frameOffset >= *(unsigned short *)(shape + 4))
        return;

    commands = shape + 8 + *(int *)(shape + frameOffset);
    rowCode = *(unsigned short *)commands;
    while (rowCode != 0) {
        int drawX;
        int drawY;
        unsigned char *screen;

        drawX = x + *(short *)(commands + 2);
        drawY = y + *(short *)(commands + 4);
        commands += 6;
        screen = 0;
        if (drawY >= viewport->top && drawY <= viewport->bottom)
            screen = viewport->pixels + viewport->rowOffsets[drawY] + drawX;
        if ((rowCode & 1) == 0) {
            int runLength = rowCode >> 1;
            int skip = 0;
            int copyLength = runLength;
            int endX = drawX + runLength - 1;

            if (drawY >= viewport->top && drawY <= viewport->bottom &&
                drawX <= viewport->right && endX >= viewport->left) {
                if (drawX < viewport->left) {
                    skip = viewport->left - drawX;
                    copyLength -= skip;
                }
                if (endX > viewport->right)
                    copyLength -= endX - viewport->right;
                memcpy(saved, screen + skip, copyLength);
                saved += copyLength;
            }
            commands += runLength;
        } else {
            int remaining = rowCode >> 1;

            while (remaining != 0) {
                unsigned char code = *commands;
                int runLength = code >> 1;
                int skip = 0;
                int copyLength = runLength;
                int endX = drawX + runLength - 1;

                if (drawY >= viewport->top && drawY <= viewport->bottom &&
                    drawX <= viewport->right && endX >= viewport->left) {
                    if (drawX < viewport->left) {
                        skip = viewport->left - drawX;
                        copyLength -= skip;
                    }
                    if (endX > viewport->right)
                        copyLength -= endX - viewport->right;
                    memcpy(saved, screen + skip, copyLength);
                    saved += copyLength;
                }
                if ((code & 1) == 0)
                    commands += runLength + 1;
                else
                    commands += 2;
                drawX += runLength;
                if (screen != 0)
                    screen += runLength;
                remaining -= runLength;
            }
        }
        rowCode = *(unsigned short *)commands;
    }
}

/* Function start: 0x441740 */
void RestoreSpriteBackground(Viewport *viewport, unsigned char *background,
                             short x, short y, unsigned char *shape,
                             short frame)
{
    unsigned char *saved = background;
    unsigned char *commands;
    unsigned short rowCode;
    int frameOffset;

    if (background != 0 && shape != 0 && frame >= 0) {
        frameOffset = (short)(frame * 4 + 4);
        if (frameOffset < *(unsigned short *)(shape + 4)) {
            commands = shape + 8 + *(int *)(shape + frameOffset);
            rowCode = *(unsigned short *)commands;
            while (rowCode != 0) {
                int drawX;
                int drawY;
                unsigned char *screen;

                drawX = x + *(short *)(commands + 2);
                drawY = y + *(short *)(commands + 4);
                commands += 6;
                screen = 0;
                if (drawY >= viewport->top && drawY <= viewport->bottom)
                    screen = viewport->pixels + viewport->rowOffsets[drawY] + drawX;
                if ((rowCode & 1) == 0) {
                    int runLength = rowCode >> 1;
                    int skip = 0;
                    int copyLength = runLength;
                    int endX = drawX + runLength - 1;

                    if (drawY >= viewport->top && drawY <= viewport->bottom &&
                        drawX <= viewport->right && endX >= viewport->left) {
                        if (drawX < viewport->left) {
                            skip = viewport->left - drawX;
                            copyLength -= skip;
                        }
                        if (endX > viewport->right)
                            copyLength -= endX - viewport->right;
                        memcpy(screen + skip, saved, copyLength);
                        saved += copyLength;
                    }
                    commands += runLength;
                } else {
                    int remaining = rowCode >> 1;

                    while (remaining != 0) {
                        unsigned char code = *commands;
                        int runLength = code >> 1;
                        int skip = 0;
                        int copyLength = runLength;
                        int endX = drawX + runLength - 1;

                        if (drawY >= viewport->top && drawY <= viewport->bottom &&
                            drawX <= viewport->right && endX >= viewport->left) {
                            if (drawX < viewport->left) {
                                skip = viewport->left - drawX;
                                copyLength -= skip;
                            }
                            if (endX > viewport->right)
                                copyLength -= endX - viewport->right;
                            memcpy(screen + skip, saved, copyLength);
                            saved += copyLength;
                        }
                        if ((code & 1) == 0)
                            commands += runLength + 1;
                        else
                            commands += 2;
                        drawX += runLength;
                        if (screen != 0)
                            screen += runLength;
                        remaining -= runLength;
                    }
                }
                rowCode = *(unsigned short *)commands;
            }
        }
        if (viewport->pixels == DAT_005a6ba0.pixels)
            DIBslam();
    }
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
        FillRasterClip(&g_stRasterClip_00496fc0, colour);
    }
    if (viewport == &DAT_005a6ba0) {
        DIBslam();
        DIBslamReal();
    }
}

/* Function start: 0x441B60 */
int GetViewportPixel(Viewport *viewport, short x, short y)
{
    ClipViewportToScreen(viewport);
    return ReadRasterClipPixel(&g_stRasterClip_00496fc0,
                               (int)x - viewport->left,
                               (int)y - viewport->top);
}

/* Function start: 0x441BA0 */
void DrawViewportLine(Viewport *viewport, short x1, short y1,
                      short x2, short y2, short colour)
{
    ClipViewportToScreen(viewport);
    DrawClippedLine(&g_stRasterClip_00496fc0,
                    x1 - viewport->left, y1 - viewport->top,
                    x2 - viewport->left, y2 - viewport->top,
                    0, colour);
}

/* Function start: 0x441C70 */
void DrawFilledViewportRect(Viewport *viewport, short left, short top,
                            short right, short bottom, short colour)
{
    int height;
    int row;

    height = bottom - top;
    row = 0;
    ClipViewportToScreen(viewport);
    while (row <= height) {
        DrawClippedLine(&g_stRasterClip_00496fc0,
                        left - viewport->left,
                        row + top - viewport->top,
                        right - viewport->left,
                        row + top - viewport->top,
                        0, colour);
        row++;
    }
}

/* Function start: 0x441CF0 */
void DrawViewportBorder(Viewport *viewport, short left, short top,
                        short right, short bottom, short colour)
{
    ClipViewportToScreen(viewport);
    DrawClippedLine(&g_stRasterClip_00496fc0,
                    left - viewport->left, top - viewport->top,
                    right - viewport->left, top - viewport->top,
                    0, colour);
    DrawClippedLine(&g_stRasterClip_00496fc0,
                    left - viewport->left, bottom - viewport->top,
                    right - viewport->left, bottom - viewport->top,
                    0, colour);
    DrawClippedLine(&g_stRasterClip_00496fc0,
                    left - viewport->left, top - viewport->top,
                    left - viewport->left, bottom - viewport->top,
                    0, colour);
    DrawClippedLine(&g_stRasterClip_00496fc0,
                    right - viewport->left, top - viewport->top,
                    right - viewport->left, bottom - viewport->top,
                    0, colour);
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
short GetTransformedShapeBounds(Viewport *viewport, short x, short y,
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
    CheckHeapBlockSignature((int)shape);
    frameOffset = frame * 4 + 4;
    if (frameOffset <= (int)*(unsigned short *)(shape + 4)) {
        frameData = (short *)(shape + *(int *)(shape + frameOffset));
        leftExtent = frameData[1];
        topExtent = frameData[2];
        absoluteCosine =
            (int)(g_awAbsoluteCosine_00470778[angle] * scale) >> 8;
        absoluteSine =
            (int)(g_awAbsoluteSine_00470a48[angle] * scale) >> 8;
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
                 const short *geometry)
{
    const short *run;
    unsigned char *sourcePixels;
    unsigned char *destinationPixels;
    volatile unsigned int width;
    short sourceLeft;
    short sourceTop;
    short destinationX;
    short sourceY;

    if (source->pixels != 0 && destination->pixels != 0) {
        sourceLeft = geometry[2];
        sourceTop = geometry[3];
        destinationX = geometry[4];
        if (destinationX != -1) {
            run = geometry + 5;
            do {
                sourceY = run[0];
                width = (unsigned short)run[1];
                sourcePixels = source->pixels +
                    source->rowOffsets[sourceY - sourceTop] - sourceLeft +
                    destinationX;
                destinationPixels = destination->pixels +
                    destination->rowOffsets[sourceY] + destinationX;
                memcpy(destinationPixels, sourcePixels, width);
                destinationX = run[2];
                run += 3;
            } while (destinationX != -1);
        }
        if (destination->pixels == DAT_005a6ba0.pixels)
            DIBslam();
    }
}

/* Function start: 0x442300 */
void snow_viewport(Viewport *viewport, int effect, unsigned int colour)
{
    (void)effect;
    (void)colour;
    if (viewport->pixels == DAT_005a6ba0.pixels)
        DIBslam();
    RasterLineHook((void *)g_szSnowViewport_00470da4);
}

/* Function start: 0x442330 */
void UpdateStreamerStoppedFlag(void)
{
    if (DAT_00465058 != 0)
        DAT_0046aa04 = (g_dwStreamerState_00597cd0 & 4) == 0;
}

/* Function start: 0x442370 */
void InitializeAudioStreamer(HWND window)
{
    if (DAT_00465058 != 0) {
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

    if (DAT_00465058 != 0) {
        streamsDirectory = LocateStreamsDirOnDisc();
        if (streamsDirectory == 0) {
            ShowNoticeMessageBox("Unable to locate streamed music.\n");
            exit_squadron("");
        }
        sprintf(g_szStreamerPath_00597750, "%s%s",
                streamsDirectory, streamName);
        SoundDebugPrintf("Streamer_open %s", g_szStreamerPath_00597750);
        ix_streamer_open_stream_file(g_szStreamerPath_00597750);
        g_bStreamerAudioPlaying_00597748 = 0;
    }
}

/* Function start: 0x442430 */
void Streamer_play(void)
{
    if (DAT_00465058 != 0 && g_bStreamerAudioPlaying_00597748 == 0) {
        g_bStreamerAudioPlaying_00597748 = 1;
        SoundDebugPrintf("Streamer_play");
        ix_streamer_audio_play();
    }
}

/* Function start: 0x442460 */
void Streamer_stop(void)
{
    if (DAT_00465058 != 0 && g_bStreamerAudioPlaying_00597748 != 0) {
        SoundDebugPrintf("Streamer_stop");
        ix_streamer_audio_stop();
        g_bStreamerAudioPlaying_00597748 = 0;
    }
}

/* Function start: 0x4424B0 */
void ClearStreamerTrigger(void)
{
    if (DAT_00465058 != 0)
        ix_streamer_set_trigger(-1);
}

/* Function start: 0x4424D0 */
void Streamer_trigger(int trigger)
{
    if (DAT_00465058 != 0) {
        if (g_bStreamerAudioPlaying_00597748 == 0) {
            ForceStreamerTrigger(trigger);
            return;
        }
        SoundDebugPrintf("Streamer_trigger %d", trigger);
        if (trigger >= 0)
            ix_streamer_set_trigger((char)trigger);
        if (g_bStreamerAudioPlaying_00597748 == 0)
            Streamer_play();
    }
}

/* Function start: 0x442520 */
void SetStreamerIntensity(unsigned char intensity)
{
    if (DAT_00465058 != 0) {
        ix_streamer_set_intensity(intensity);
        if (g_bStreamerAudioPlaying_00597748 == 0)
            Streamer_play();
    }
}

/* Function start: 0x442550 */
void ForceStreamerTrigger(int trigger)
{
    if (DAT_00465058 != 0) {
        SoundDebugPrintf("FORCE");
        if (trigger >= 0)
            ix_streamer_force_trigger((char)trigger);
        if (g_bStreamerAudioPlaying_00597748 == 0)
            Streamer_play();
    }
}

/* Function start: 0x442590 */
void SetMusicStreamVolume(unsigned short volume)
{
    if (DAT_00465058 != 0)
        ix_streamer_set_volume(volume);
}

/* Function start: 0x4425D0 */
void Streamer_close(void)
{
    if (DAT_00465058 != 0) {
        SoundDebugPrintf("Streamer_close");
        ix_streamer_close_stream_file();
    }
}

/* Function start: 0x442600 */
int ReadCheaterFlagFromRegistry(void)
{
    HKEY key;
    DWORD type = REG_DWORD;
    DWORD size = sizeof(int);
    int enabled = 0;

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "Software\\Origin Systems\\WC: Kilrathi Saga",
                      0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS) {
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
