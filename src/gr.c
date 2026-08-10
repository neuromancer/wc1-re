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
    int topOffset;
    int nextOffset;

    if (viewport == 0 || viewport->pixels == 0 ||
        viewport->rowOffsets == 0) {
        memset(surface, 0, sizeof(*surface));
        memset(clip, 0, sizeof(*clip));
        return;
    }
    if (viewport == &DAT_005a6ba0)
        DAT_00486518 = 1;
    topOffset = (int)SignExtendClipCoord(
        viewport->rowOffsets[viewport->top]);
    nextOffset = (int)SignExtendClipCoord(
        viewport->rowOffsets[viewport->top + 1]);
    surface->pixels = viewport->pixels + viewport->left + topOffset;
    surface->maximumX = nextOffset - topOffset - 1;
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
    memset(bitmap, 0, sourceWidth * sourceHeight);

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
                        if (colour != 0)
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
    (void)blendMode;
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
    int width;
    int height;
    int destinationWidth;
    int destinationHeight;
    int row;

    if (source == 0 || destination == 0 || source->pixels == 0 ||
        destination->pixels == 0 || source->rowOffsets == 0 ||
        destination->rowOffsets == 0) {
        return;
    }
    width = source->right - source->left + 1;
    height = source->bottom - source->top + 1;
    destinationWidth = destination->right - destination->left + 1;
    destinationHeight = destination->bottom - destination->top + 1;
    if (destinationWidth < width)
        width = destinationWidth;
    if (destinationHeight < height)
        height = destinationHeight;
    row = 0;
    while (row < height) {
        memcpy(destination->pixels +
                   destination->rowOffsets[destination->top + row] +
                   destination->left,
               source->pixels + source->rowOffsets[source->top + row] +
                   source->left,
               width);
        row++;
    }
    if (destination == &DAT_005a6ba0)
        DAT_00486518 = 1;
}

/* Function start: 0x441AE0 */
void ClearViewport(Viewport *viewport, short colour)
{
    int width;
    short row;

    if (viewport != 0 && viewport->pixels != 0 &&
        viewport->rowOffsets != 0) {
        width = viewport->right - viewport->left + 1;
        row = viewport->top;
        while (row <= viewport->bottom) {
            memset(viewport->pixels + viewport->rowOffsets[row] +
                       viewport->left,
                   colour, width);
            row++;
        }
    }
    if (viewport == &DAT_005a6ba0) {
        DAT_00486518 = 1;
        DIBslamReal();
    }
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

/* Function start: 0x442050 */
int GetTransformedShapeBounds(Viewport *viewport, short x, short y,
                              unsigned char *shape, short frame,
                              short angle, short scale, int flip,
                              short *bounds)
{
    short *frameData;
    unsigned int frameOffset;
    int rightExtent;
    int leftExtent;
    int topExtent;
    int bottomExtent;
    int widthExtent;
    int heightExtent;
    int sine;
    int cosine;
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
    frameOffset = (unsigned int)(frame * 4 + 4);
    if (*(unsigned short *)(shape + 4) < frameOffset)
        return 0;
    frameData = (short *)(shape + *(int *)(shape + frameOffset));
    rightExtent = frameData[0];
    leftExtent = frameData[1];
    topExtent = frameData[2];
    bottomExtent = frameData[3];
    sine = (int)(SinFixed(angle) * scale) >> 8;
    cosine = (int)(CosFixed(angle) * scale) >> 8;
    if (sine == 0)
        sine = 1;
    if (cosine == 0)
        cosine = 1;
    widthExtent = rightExtent + leftExtent;
    heightExtent = topExtent + bottomExtent;
    transformedHeight = cosine * widthExtent + sine * heightExtent;
    if ((char)transformedHeight != 0)
        transformedHeight += 0x100;
    transformedHeight >>= 8;
    transformedWidth = sine * widthExtent + cosine * heightExtent;
    if ((char)transformedWidth != 0)
        transformedWidth += 0x100;
    transformedWidth >>= 8;
    top = (short)(y - (cosine * leftExtent >> 8) -
                  (sine * topExtent >> 8));
    bottom = (short)(transformedHeight + top);
    left = (short)(((cosine * topExtent >> 8) -
                    (sine * leftExtent >> 8) + x) -
                   ((cosine * heightExtent >> 8) + 1));
    right = (short)(transformedWidth + left);
    if (viewport->left <= right && left <= viewport->right &&
        viewport->top <= bottom && top <= viewport->bottom) {
        bounds[0] = left;
        bounds[2] = right;
        bounds[1] = top;
        bounds[3] = bottom;
        return 1;
    }
    (void)flip;
    return 0;
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

/* Function start: 0x442460 */
void Streamer_stop(void)
{
    if (DAT_00465058 != 0 && g_bStreamerAudioPlaying_00597748 != 0) {
        SoundDebugPrintf("Streamer_stop");
        ix_streamer_audio_stop();
        g_bStreamerAudioPlaying_00597748 = 0;
    }
}

/* Function start: 0x442590 */
void SetMusicStreamVolume(unsigned short volume)
{
    if (DAT_00465058 != 0)
        ix_streamer_set_volume(volume);
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
