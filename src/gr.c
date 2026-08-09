/*
 *  Rasteriser primitives and screen-space effects.
 *
 *  Address range 0x440c00-0x44274f (provisional -- see docs/ORDER.md).
 *  Boundary evidence: PROVEN by name: shadow_draw, fizzle_fade, snow_viewport.
 */
#include "wc1.h"

/* Function start: 0x440C00 */
void ValidateViewportBounds(Viewport *viewport, int *surface,
                            unsigned int *clip)
{
    int topOffset;
    int nextOffset;

    if (viewport == 0 || viewport->pixels == 0 ||
        viewport->rowOffsets == 0) {
        memset(surface, 0, 5 * sizeof(*surface));
        memset(clip, 0, 5 * sizeof(*clip));
        return;
    }
    if (viewport == &DAT_005a6ba0)
        DAT_00486518 = 1;
    topOffset = (int)SignExtendClipCoord(
        viewport->rowOffsets[viewport->top]);
    nextOffset = (int)SignExtendClipCoord(
        viewport->rowOffsets[viewport->top + 1]);
    surface[0] = (int)(viewport->pixels + viewport->left + topOffset);
    surface[1] = nextOffset - topOffset - 1;
    surface[2] = viewport->bottom - viewport->top;
    surface[3] = 0;
    surface[4] = 0;
    clip[0] = (unsigned int)surface;
    clip[1] = 0;
    clip[2] = 0;
    clip[3] = viewport->right - viewport->left;
    clip[4] = viewport->bottom - viewport->top;
}

/* Function start: 0x440CF0 */
void ClipViewportToScreen(Viewport *viewport)
{
    ValidateViewportBounds(viewport, DAT_004875a8, DAT_00496fc0);
}

/* Function start: 0x440FE0 */
void DrawSpriteTransformed(Viewport *viewport, int x, int y,
                           unsigned char *shape, int frame,
                           int angle, int scaleX, int scaleY,
                           int flip, int blendMode)
{
    unsigned char *commands;
    unsigned short rowCode;
    int frameOffset;

    if (viewport == 0 || shape == 0 || frame < 0 ||
        viewport->pixels == 0 || viewport->rowOffsets == 0)
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
                memcpy(screen + skip, commands + skip, copyLength);
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
                    if ((code & 1) == 0)
                        memcpy(screen + skip, commands + 1 + skip, copyLength);
                    else
                        memset(screen + skip, commands[1], copyLength);
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

/* Function start: 0x441140 */
void RasterLineHook(void)
{
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
void ClearViewport(Viewport *viewport, unsigned char colour)
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

/* Function start: 0x442330 */
void UpdateStreamerStoppedFlag(void)
{
    if (DAT_00465058 != 0)
        DAT_0046aa04 = (g_dwStreamerState_00597cd0 & 4) == 0;
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
