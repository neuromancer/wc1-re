/*
 *  Rasteriser primitives and screen-space effects.
 *
 *  Address range 4408a0-44269f (provisional -- see docs/ORDER.md).
 *  Boundary evidence: PROVEN by name: shadow_draw, fizzle_fade, snow_viewport.
 */
#include "wc1.h"

const char szGameRegistryKey[42] =
    "Software\\Origin Systems\\WC: Kilrathi Saga"; /* 0x004631c8 */
const char szCheaterRegistryValue[8] = "Cheater"; /* 0x004631f8 */

/* 0x00470678 */
static unsigned char abPaletteTranslation[256] = {
      0,   1,   2,   3,   4,   5,   6,   7,
      8,   9,  10,  11,  12,  13,  14,  15,
     16,  17,  18,  19,  20,  21,  22,  23,
     24,  25,  26,  27,  28,  29,  30,  31,
     32,  33,  34,  35,  36,  37,  38,  39,
     40,  41,  42,  43,  44,  45,  46,  47,
     48,  49,  50,  51,  52,  53,  54,  55,
     56,  57,  58,  59,  60,  61,  62,  63,
     64,  65,  66,  67,  68,  69,  70,  71,
     72,  73,  74,  75,  76,  77,  78,  79,
     80,  81,  82,  83,  84,  85,  86,  87,
     88,  89,  90,  91,  92,  93,  94,  95,
     96,  97,  98,  99, 100, 101, 102, 103,
    104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127,
    128, 129, 130, 131, 132, 133, 134, 135,
    136, 137, 138, 139, 140, 141, 142, 143,
    144, 145, 146, 147, 148, 149, 150, 151,
    152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167,
    168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183,
    184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199,
    200, 201, 202, 203, 204, 205, 206, 207,
    208, 209, 210, 211, 212, 213, 214, 215,
    216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231,
    232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247,
    248, 249, 250, 251, 252, 253, 254, 255
};
/* 0x00470778 */
static unsigned short awAbsoluteCosine[360] = {
    255, 255, 255, 255, 255, 255, 255, 254, 254, 253, 252, 251,
    250, 250, 248, 247, 246, 245, 243, 242, 241, 239, 237, 236,
    234, 232, 230, 228, 226, 224, 221, 219, 217, 215, 212, 210,
    207, 204, 202, 198, 196, 193, 190, 187, 184, 181, 178, 175,
    171, 168, 165, 161, 158, 154, 150, 147, 143, 139, 136, 131,
    128, 124, 120, 116, 112, 108, 104, 100,  96,  92,  88,  83,
     79,  74,  71,  66,  62,  58,  53,  49,  45,  40,  36,  31,
     27,  22,  18,  13,   9,   4,   0,   4,   9,  13,  18,  22,
     27,  31,  36,  40,  45,  49,  53,  58,  62,  66,  71,  75,
     79,  83,  88,  92,  96, 100, 104, 108, 112, 116, 120, 124,
    128, 131, 136, 139, 143, 147, 150, 154, 158, 161, 165, 168,
    171, 175, 178, 181, 184, 187, 190, 193, 196, 198, 202, 204,
    207, 210, 212, 215, 217, 219, 221, 224, 226, 228, 230, 232,
    234, 236, 237, 239, 241, 242, 243, 245, 246, 247, 248, 250,
    250, 251, 252, 253, 254, 254, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 254, 254, 253, 252, 251,
    250, 250, 248, 247, 246, 245, 243, 242, 241, 239, 237, 236,
    234, 232, 230, 228, 226, 224, 221, 219, 217, 215, 212, 210,
    207, 204, 202, 198, 196, 193, 190, 187, 184, 181, 178, 175,
    171, 168, 165, 161, 158, 154, 150, 147, 143, 139, 136, 131,
    128, 124, 120, 116, 112, 108, 104, 100,  96,  92,  88,  83,
     79,  74,  71,  66,  62,  58,  53,  49,  45,  40,  36,  31,
     27,  22,  18,  13,   9,   4,   0,   4,   9,  13,  18,  22,
     27,  31,  36,  40,  45,  49,  53,  58,  62,  66,  71,  75,
     79,  83,  88,  92,  96, 100, 104, 108, 112, 116, 120, 124,
    128, 131, 136, 139, 143, 147, 150, 154, 158, 161, 165, 168,
    171, 175, 178, 181, 184, 187, 190, 193, 196, 198, 202, 204,
    207, 210, 212, 215, 217, 219, 221, 224, 226, 228, 230, 232,
    234, 236, 237, 239, 241, 242, 243, 245, 246, 247, 248, 250,
    250, 251, 252, 253, 254, 254, 255, 255, 255, 255, 255, 255
};
/* 0x00470a48 */
static unsigned short awAbsoluteSine[360] = {
      0,   4,   9,  13,  18,  22,  27,  31,  36,  40,  45,  49,
     53,  58,  62,  66,  71,  75,  79,  83,  88,  92,  96, 100,
    104, 108, 112, 116, 120, 124, 128, 131, 136, 139, 143, 147,
    150, 154, 158, 161, 165, 168, 171, 175, 178, 181, 184, 187,
    190, 193, 196, 198, 202, 204, 207, 210, 212, 215, 217, 219,
    221, 224, 226, 228, 230, 232, 234, 236, 237, 239, 241, 242,
    243, 245, 246, 247, 248, 250, 250, 251, 252, 253, 254, 254,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 254, 254, 253, 252, 251, 250, 250, 248, 247, 246, 245,
    243, 242, 241, 239, 237, 236, 234, 232, 230, 228, 226, 224,
    221, 219, 217, 215, 212, 210, 207, 204, 202, 198, 196, 193,
    190, 187, 184, 181, 178, 175, 171, 168, 165, 161, 158, 154,
    150, 147, 143, 139, 136, 131, 128, 124, 120, 116, 112, 108,
    104, 100,  96,  92,  88,  83,  79,  74,  71,  66,  62,  58,
     53,  49,  45,  40,  36,  31,  27,  22,  18,  13,   9,   4,
      0,   4,   9,  13,  18,  22,  27,  31,  36,  40,  45,  49,
     53,  58,  62,  66,  71,  75,  79,  83,  88,  92,  96, 100,
    104, 108, 112, 116, 120, 124, 128, 131, 136, 139, 143, 147,
    150, 154, 158, 161, 165, 168, 171, 175, 178, 181, 184, 187,
    190, 193, 196, 198, 202, 204, 207, 210, 212, 215, 217, 219,
    221, 224, 226, 228, 230, 232, 234, 236, 237, 239, 241, 242,
    243, 245, 246, 247, 248, 250, 250, 251, 252, 253, 254, 254,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 254, 254, 253, 252, 251, 250, 250, 248, 247, 246, 245,
    243, 242, 241, 239, 237, 236, 234, 232, 230, 228, 226, 224,
    221, 219, 217, 215, 212, 210, 207, 204, 202, 198, 196, 193,
    190, 187, 184, 181, 178, 175, 171, 168, 165, 161, 158, 154,
    150, 147, 143, 139, 136, 131, 128, 124, 120, 116, 112, 108,
    104, 100,  96,  92,  88,  83,  79,  74,  71,  66,  62,  58,
     53,  49,  45,  40,  36,  31,  27,  22,  18,  13,   9,   4
};

/* Raster targets and shape work buffers. CL places these statics by
 * identifier hash, so their spellings reproduce the original order. */
static RasterSurface stRasterTarget; /* 0x004875a8 */
static unsigned char abShapeTransformPixels[0xfa00]; /* 0x004875c0 */
static RasterClip stRasterClip; /* 0x00496fc0 */
/* Nothing in the shipped executable references this table. */
static unsigned char abUnusedGrTable[0x670]; /* 0x00496fd8 */
static unsigned char abSolidColourLookup[256]; /* 0x00497648 */
static unsigned char abShapeRLEBuffer[0x100000]; /* 0x00497748 */
static int bStreamerAudioRunning; /* 0x00597748 */
static char szStreamerPathBuffer[128]; /* 0x00597750 */

/* Linker commons defined here; LINK orders them by identifier hash. */
int nUnusedFlightProfileTicksA; /* 0x00598888 */
int nUnusedFlightProfileTicksB; /* 0x0059888c */
int nUnusedFlightProfileTicksC; /* 0x00598890 */
Viewport stRoomScreenViewport; /* 0x005988a0 */
short nSavedRoomCtrlX; /* 0x005988b4 */
unsigned char *pRecRoomRoster; /* 0x005988b8 */
unsigned char *pMissionCampaignInfo; /* 0x005988bc */
unsigned char *apRecRoomFigureShapes[3]; /* 0x005988c0 */
PilotRecord aPilotEntriesList[9]; /* 0x005988d0 */
ConstellationObjectDefinition *pConstellationDefinitionsPtr; /* 0x00598a28 */
PilotRecord *apWingmanPilotsList[8]; /* 0x00598a30 */

/* Function start: 0x4408A0 */
void CheckHeapBlockSignature(unsigned char *shape)
{
    if (*(int *)(shape - 8) != 0x6666656a)
        exit_squadron("not jefftep");
}

/* Function start: 0x4408C0 */
unsigned char *GetPreparedShapeData(unsigned char *shape)
{
#ifdef SDL_PORT
    return *(unsigned char **)(shape - 8 - sizeof(unsigned char *));
#else
    return *(unsigned char **)(shape - 4);
#endif
}

/* Function start: 0x4408D0 */
short __stdcall GetShapeFrameCount(unsigned char *shape)
{
    CheckHeapBlockSignature(shape);
    return (short)((*(unsigned short *)(shape + 4) >> 2) - 1);
}

/* Function start: 0x4408F0 */
void GetShapeFrameExtents(unsigned char *shape, short frame,
                          short *width, short *height,
                          short *leftExtent, short *topExtent)
{
    int rightExtent;
#ifndef SDL_PORT
    short *frameHeader;
#endif
    int frameOffset;
    int left;
    int top;
    int bottom;
#ifdef SDL_PORT
    short frameExtents[4];
#endif

    frameOffset = (int)(short)(frame * 4 + 4);
    if (frameOffset < (int)*(unsigned short *)(shape + 4)) {
#ifdef SDL_PORT
        memcpy(frameExtents, shape + *(int *)(shape + frameOffset),
               sizeof(frameExtents));
        rightExtent = frameExtents[0];
        left = frameExtents[1];
        top = frameExtents[2];
        bottom = frameExtents[3];
#else
        frameHeader = (short *)(shape + *(int *)(shape + frameOffset));
        rightExtent = *frameHeader++;
        left = *frameHeader++;
        top = *frameHeader++;
        bottom = *frameHeader;
#endif
        *width = (short)(left + rightExtent + 1);
        *height = (short)(top + bottom + 1);
        *leftExtent = (short)left;
        *topExtent = (short)top;
    }
}

/* Function start: 0x440960 */
void DecodeShapeFrame(unsigned char *shape, short frame,
                      unsigned char *bitmap, int width, int height,
                      int leftExtent, int topExtent)
{
    unsigned char colour;
    unsigned short rowCode;
    int y;
    short runCopyLength;
    int runRight;
    short rowCopyLength;
    unsigned char *commands;
    int rowRight;
    int x;
    int frameOffset;
    short maximumY;
    unsigned char code;
    short maximumX;
    short sourceX;
    unsigned char *destination;
    short sourceY;
    int skip;
    unsigned short runLength;

    if (shape == 0 || frame < 0)
        return;
    frameOffset = (int)(short)(frame * 4 + 4);
    if (frameOffset >= (int)*(unsigned short *)(shape + 4))
        return;

    maximumX = (short)(width - 1);
    maximumY = (short)(height - 1);
    commands = shape + *(int *)(shape + frameOffset) + 8;
#ifdef SDL_PORT
    memcpy(&rowCode, commands, sizeof(rowCode));
#else
    rowCode = *(unsigned short *)commands;
#endif
    commands += 2;
    while (rowCode != 0) {
#ifdef SDL_PORT
        memcpy(&sourceX, commands, sizeof(sourceX));
#else
        sourceX = *(short *)commands;
#endif
        commands += 2;
#ifdef SDL_PORT
        memcpy(&sourceY, commands, sizeof(sourceY));
#else
        sourceY = *(short *)commands;
#endif
        commands += 2;
        x = leftExtent + sourceX;
        y = topExtent + sourceY;
        destination = bitmap + y * width + x;
        if ((rowCode & 1) != 0) {
            rowCode >>= 1;
            while (rowCode != 0) {
                code = *commands;
                commands++;
                if ((code & 1) != 0) {
                    code >>= 1;
                    colour = *commands;
                    commands++;
                    runLength = code;
                    rowCode = (unsigned short)(rowCode - runLength);
                    if (y >= 0 && y <= maximumY) {
                        runRight = x + runLength - 1;
                        if (x <= maximumX && runRight >= 0) {
                            skip = 0;
                            runCopyLength = (short)runLength;
                            if (x < 0) {
                                runCopyLength =
                                    (short)(runCopyLength + (short)x);
                                skip = -x;
                            }
                            if (maximumX < runRight)
                                runCopyLength =
                                    (short)(runCopyLength - runRight + maximumX);
                            memset(destination + skip, colour,
                                   (short)runCopyLength);
                        }
                    }
                    x += runLength;
                    destination += runLength;
                } else {
                    code >>= 1;
                    runLength = code;
                    rowCode = (unsigned short)(rowCode - runLength);
                    if (y >= 0 && y <= maximumY) {
                        runRight = x + runLength - 1;
                        if (x <= maximumX && runRight >= 0) {
                            skip = 0;
                            runCopyLength = (short)runLength;
                            if (x < 0) {
                                runCopyLength =
                                    (short)(runCopyLength + (short)x);
                                skip = -x;
                            }
                            if (maximumX < runRight)
                                runCopyLength =
                                    (short)(runCopyLength - runRight + maximumX);
                            memcpy(destination + skip, commands + skip,
                                   (short)runCopyLength);
                        }
                    }
                    commands += runLength;
                    x += runLength;
                    destination += runLength;
                }
            }
        } else {
            rowCode >>= 1;
            if (y >= 0 && y <= maximumY) {
                rowRight = x + rowCode - 1;
                if (x <= maximumX && rowRight >= 0) {
                    skip = 0;
                    rowCopyLength = (short)rowCode;
                    if (x < 0) {
                        rowCopyLength = (short)(rowCopyLength + (short)x);
                        skip = -x;
                    }
                    if (maximumX < rowRight)
                        rowCopyLength =
                            (short)(rowCopyLength - rowRight + maximumX);
                    memcpy(destination + skip, commands + skip,
                           (short)rowCopyLength);
                }
            }
            commands += rowCode;
        }
#ifdef SDL_PORT
        memcpy(&rowCode, commands, sizeof(rowCode));
#else
        rowCode = *(unsigned short *)commands;
#endif
        commands += 2;
    }
}

/* Function start: 0x440BE0 */
int SignExtendClipCoord(volatile short v)
{
    if ((unsigned short)v < 0xfdc0)
        return (unsigned short)v;
    return (int)(short)v;
}
/* Function start: 0x440C00 */
void ValidateViewportBounds(Viewport *viewport, RasterSurface *surface,
                            RasterClip *clip)
{
    int topOffset;
    int nextOffset;
    int rowStrideOffset;
    int allocation;

    if (viewport->pixels != stScreenView.pixels) {
        allocation = 0;
        while (allocation < nViewAllocTotal) {
            if (apViewportAllocations[allocation] ==
                viewport->pixels)
                break;
            allocation++;
        }
        if (allocation >= nViewAllocTotal)
            exit_squadron("bad vport");
    }
    if (viewport->pixels == stScreenView.pixels)
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
    ValidateViewportBounds(viewport, &stRasterTarget,
                           &stRasterClip);
}

/* Function start: 0x440D10 */
void SetSolidColourTranslation(unsigned char colour)
{
    memset(abSolidColourLookup, colour, 255);
    abSolidColourLookup[255] = 0xff;
    SetPaletteTranslationTable(abSolidColourLookup);
}

/* Function start: 0x440D50 */
void PrepareShapeRLEData(unsigned char *shape)
{
    unsigned char *bitmap;
    unsigned char *pixel;
    unsigned char *output;
    int *frameOffset;
    int preparedSize;
    int frameCount;
    short width;
    short height;
    short leftExtent;
    short topExtent;
    int frame;
    unsigned char *preparedShape;
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

    *(int *)abShapeRLEBuffer =
        *(const int *)"1.00";
    frameCount = GetShapeFrameCount(shape);
    memcpy(abShapeRLEBuffer + 4, &frameCount, sizeof(frameCount));
    frameOffset = (int *)(abShapeRLEBuffer + 8);
    memset(frameOffset, 0, frameCount * 8);
    output = abShapeRLEBuffer + 8 + (frameCount << 3);

    for (frame = 0; frame < frameCount; frame++) {
        *frameOffset = (int)(output - abShapeRLEBuffer);
        frameOffset += 2;
        GetShapeFrameExtents(shape, (short)frame, &width, &height,
                             &leftExtent, &topExtent);
        memcpy(output, &height, sizeof(height));
        output += sizeof(height);
        memcpy(output, &width, sizeof(width));
        output += sizeof(width);
        memcpy(output, &topExtent, sizeof(topExtent));
        output += sizeof(topExtent);
        memcpy(output, &leftExtent, sizeof(leftExtent));
        output += sizeof(leftExtent);
        frameLeft = -leftExtent;
        frameTop = -topExtent;
        frameRight = width - leftExtent - 1;
        frameBottom = height - topExtent - 1;
        memcpy(output, &frameLeft, sizeof(frameLeft));
        output += sizeof(frameLeft);
        memcpy(output, &frameTop, sizeof(frameTop));
        output += sizeof(frameTop);
        memcpy(output, &frameRight, sizeof(frameRight));
        output += sizeof(frameRight);
        memcpy(output, &frameBottom, sizeof(frameBottom));
        output += sizeof(frameBottom);

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

                    runCode = output++;
                    runLength = 0;
                    while (remaining != 0 && runLength < 0x7f &&
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
                    while (remaining != 0 && runLength < 0xff &&
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

    preparedSize = (int)(output - abShapeRLEBuffer);
    if (preparedSize > (int)sizeof(abShapeRLEBuffer))
        exit_squadron("qq copy overflow");
    preparedShape = AllocateTaggedMemory(preparedSize, 0);
    memcpy(preparedShape, abShapeRLEBuffer, preparedSize);
#ifdef SDL_PORT
    memcpy(shape - 8 - sizeof(unsigned char *), &preparedShape,
           sizeof(preparedShape));
#else
    memcpy(shape - sizeof(preparedShape), &preparedShape,
           sizeof(preparedShape));
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
                exit_squadron("bad flip");
            }
        }
        if (blendMode != 0) {
            RotateRLEImage(&stRasterClip,
                           GetPreparedShapeData(shape), frame,
                           x - viewport->left, y - viewport->top,
                           abShapeTransformPixels,
                           angle * 10, scaleX * 256, scaleY * 256, 1);
        } else {
            RotateRLEImage(&stRasterClip,
                           GetPreparedShapeData(shape), frame,
                           x - viewport->left, y - viewport->top,
                           abShapeTransformPixels,
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
    int row;
    unsigned int destinationOffset;
    unsigned char background;
    unsigned char *characterData;
    unsigned char *destination;
    unsigned char fontBackground;
    unsigned char colour;
    unsigned char fontColour;
    unsigned char *source;
    Viewport *viewport;
    int column;
    unsigned char translated;
    unsigned short rowOffset;
    short characterIndex;

    row = y;
    background = context->backgroundColour;
    characterData = context->font;
    viewport = context->viewport;
    fontColour = characterData[2];
    fontBackground = characterData[3];
    colour = context->colour;

    abPaletteTranslation[fontColour] = colour;
    abPaletteTranslation[fontBackground] = background;
#ifdef SDL_PORT
    /* Packed fonts provide lookup entries for the complete byte range. */
    characterIndex = (short)(unsigned char)character;
#else
    characterIndex = (short)(signed char)character;
    if (characterIndex != 0x81 && characterIndex != 0x84 &&
        characterIndex != 0x8e && characterIndex != 0x94 &&
        characterIndex != 0x99 && characterIndex != 0x9a &&
        characterIndex != 0xe1) {
#endif
        characterData = context->font + characterIndex;
        source = context->font +
                 (((unsigned int)characterData[0x204] << 8) +
                  characterData[0x104]);
        if (fontColour == colour && fontBackground == background) {
            for (; height-- != 0; row++) {
                unsigned short *rowOffsets = viewport->rowOffsets;

                if (viewport->top == row) {
                    destinationOffset = (unsigned int)context->cursorX;
                    rowOffset = rowOffsets[row];
                    if ((rowOffset & 0x8000) == 0)
                        destinationOffset += rowOffset;
                } else {
                    destinationOffset = rowOffsets[row] +
                                        (unsigned int)context->cursorX;
                }
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
                unsigned short *rowOffsets = viewport->rowOffsets;

                if (viewport->top == row) {
                    destinationOffset = (unsigned int)context->cursorX;
                    rowOffset = rowOffsets[row];
                    if ((rowOffset & 0x8000) == 0)
                        destinationOffset += rowOffset;
                } else {
                    destinationOffset = rowOffsets[row] +
                                        (unsigned int)context->cursorX;
                }
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
        context->cursorX += context->font[4 + characterIndex];
        abPaletteTranslation[fontColour] = fontColour;
        abPaletteTranslation[fontBackground] = fontBackground;
#ifndef SDL_PORT
    }
#endif
}

/* Function start: 0x441350 */
void __copyline(void)
{
    RasterLineHook("__copyline");
}

/* Function start: 0x441360 */
void __wipeline(void)
{
    RasterLineHook("__wipeline");
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
    unsigned short count;
    unsigned short runLength;
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
    short sourceX;
    short sourceY;
    int frameOffset;
    int drawX;
    int drawY;
    int endX;
    int skip;

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
    memcpy(&count, commands, sizeof(count));
    pixels = viewport->pixels;
    commands += 2;
    while (count != 0) {
#ifdef SDL_PORT
        memcpy(&sourceX, commands, sizeof(sourceX));
#else
        sourceX = *(short *)commands;
#endif
        commands += 2;
#ifdef SDL_PORT
        memcpy(&sourceY, commands, sizeof(sourceY));
#else
        sourceY = *(short *)commands;
#endif
        commands += 2;
        drawX = x + sourceX;
        drawY = y + sourceY;
        screen = pixels + (SPRITE_ROW_OFFSET(viewport, drawY) + drawX);
        if ((count & 1) != 0) {
            count >>= 1;
            while (count != 0) {
                code = *commands;
                commands++;
                if ((code & 1) != 0) {
                    commands++;
                    count -= (*(volatile unsigned short *)&runLength =
                              (unsigned short)(code >> 1));
                    if (top <= drawY && bottom >= drawY) {
                        endX = drawX +
                               *(volatile unsigned short *)&runLength - 1;
                        if (drawX <= right && left <= endX) {
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
                    }
                    drawX += runLength;
                    screen += runLength;
                } else {
                    count -= (*(volatile unsigned short *)&runLength =
                              (unsigned short)(code >> 1));
                    if (top <= drawY && bottom >= drawY) {
                        endX = drawX +
                               *(volatile unsigned short *)&runLength - 1;
                        if (drawX <= right && left <= endX) {
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
                    }
                    commands += runLength;
                    drawX += runLength;
                    screen += runLength;
                }
            }
        } else {
            count >>= 1;
            if (top <= drawY && bottom >= drawY) {
                endX = drawX + count - 1;
                if (drawX <= right && left <= endX) {
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
            }
            commands += count;
        }
        memcpy(&count, commands, sizeof(count));
        commands += 2;
    }
}

/* Function start: 0x441740 */
void RestoreSpriteBackground(Viewport *viewport, unsigned char *background,
                             short x, short y, unsigned char *shape,
                             short frame)
{
    unsigned short count;
    unsigned short runLength;
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
    short sourceX;
    short sourceY;
    int frameOffset;
    int drawX;
    int drawY;
    int endX;
    int skip;

    saved = background;
    if (background == 0)
        return;
    if (shape == 0)
        return;
    if (frame < 0)
        return;
    frameOffset = (short)(frame * 4 + 4);
    if ((int)*(unsigned short *)(shape + 4) <= frameOffset)
        goto refreshScreen;
    left = viewport->left;
    right = viewport->right;
    top = viewport->top;
    bottom = viewport->bottom;
    commands = shape + *(int *)(shape + frameOffset) + 8;
    memcpy(&count, commands, sizeof(count));
    pixels = viewport->pixels;
    commands += 2;
    while (count != 0) {
#ifdef SDL_PORT
        memcpy(&sourceX, commands, sizeof(sourceX));
#else
        sourceX = *(short *)commands;
#endif
        commands += 2;
#ifdef SDL_PORT
        memcpy(&sourceY, commands, sizeof(sourceY));
#else
        sourceY = *(short *)commands;
#endif
        commands += 2;
        drawX = x + sourceX;
        drawY = y + sourceY;
        screen = pixels + (SPRITE_ROW_OFFSET(viewport, drawY) + drawX);
        if ((count & 1) != 0) {
            count >>= 1;
            while (count != 0) {
                code = *commands;
                commands++;
                if ((code & 1) != 0) {
                    commands++;
                    count -= (*(volatile unsigned short *)&runLength =
                              (unsigned short)(code >> 1));
                    if (top <= drawY && bottom >= drawY) {
                        endX = drawX +
                               *(volatile unsigned short *)&runLength - 1;
                        if (drawX <= right && left <= endX) {
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
                    }
                    drawX += runLength;
                    screen += runLength;
                } else {
                    count -= (*(volatile unsigned short *)&runLength =
                              (unsigned short)(code >> 1));
                    if (top <= drawY && bottom >= drawY) {
                        endX = drawX +
                               *(volatile unsigned short *)&runLength - 1;
                        if (drawX <= right && left <= endX) {
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
                    }
                    commands += runLength;
                    drawX += runLength;
                    screen += runLength;
                }
            }
        } else {
            count >>= 1;
            if (top <= drawY && bottom >= drawY) {
                endX = drawX + count - 1;
                if (drawX <= right && left <= endX) {
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
            }
            commands += count;
        }
        memcpy(&count, commands, sizeof(count));
        commands += 2;
    }
refreshScreen:
    if (viewport->pixels == stScreenView.pixels)
        DIBslam();
}

/* Function start: 0x441A40 */
void DrawSolidColourSprite(Viewport *viewport, short x, short y,
                           unsigned char *shape, short frame,
                           short colour)
{
    SetSolidColourTranslation((unsigned char)colour);
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
    if (viewport == &stScreenView) {
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
void DrawViewportPolygonHook(Viewport *viewport, const ShortPoint *points,
                             int count, int colour)
{
    (void)viewport;
    (void)points;
    (void)count;
    (void)colour;
}

/* Function start: 0x441C00 */
void triangle(Viewport *viewport, const ShortPoint *points, int colour)
{
    DrawViewportPolygonHook(viewport, points, 3, colour);
    if (viewport->pixels == stScreenView.pixels)
        DIBslam();
    RasterLineHook("triangle");
}

/* Function start: 0x441C40 */
void shaded_triangle(Viewport *viewport)
{
    if (viewport->pixels == stScreenView.pixels)
        DIBslam();
    RasterLineHook("shaded_triangle");
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
    if (viewport->pixels == stScreenView.pixels)
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
    if (viewport->pixels == stScreenView.pixels)
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
    if (viewport->pixels == stScreenView.pixels)
        DIBslam();
}

/* Function start: 0x441EC0 */
void shadow_draw(Viewport *viewport)
{
    if (viewport->pixels == stScreenView.pixels)
        DIBslam();
    RasterLineHook("shadow_draw");
}

/* Function start: 0x441EF0 */
void DrawShapeFrameScaled(Viewport *viewport, unsigned char *shape,
                          int frame, int x, int y, int angle, int scale)
{
    if (shape != 0 && frame >= 0)
        DrawSpriteTransformed(viewport, x, y, shape, frame,
                              angle, scale, scale, 0, 0);
}

/* The original packet frame begins with these four signed extents. */
typedef struct ShapeFrameExtents {
    short right;
    short left;
    short top;
    short bottom;
} ShapeFrameExtents;

/* Function start: 0x441F30 */
void GetShapeFrameCorners(unsigned char *shape, int frame, int x, int y,
                          ShortPoint *corners, int *cornerCount)
{
    int frameOffset;

    *cornerCount = 0;
    CheckHeapBlockSignature(shape);
    frameOffset = frame * 4 + 4;
    if ((int)*(unsigned short *)(shape + 4) >= frameOffset) {
        ShapeFrameExtents *extents;
        int left;
        int top;
        int right;
        int bottom;

        shape += *(int *)(shape + frameOffset);
        extents = (ShapeFrameExtents *)shape;
        left = extents->left;
        left += x;
        right = extents->right;
        right += x;
        top = extents->top;
        top += y;
        bottom = extents->bottom;
        bottom += y;
        corners[0].x = (short)left;
        corners[0].y = (short)top;
        corners[1].x = (short)left;
        corners[1].y = (short)bottom;
        corners[2].x = (short)right;
        corners[2].y = (short)bottom;
        corners[3].x = (short)right;
        corners[3].y = (short)top;
        *cornerCount = 4;
    }
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
                                 short colour)
{
    SetSolidColourTranslation((unsigned char)colour);
    DrawSpriteTransformed(viewport, x, y, shape, frame, angle,
                          scale, scale, flip, 1);
}

/* Function start: 0x442050 */
short GetTransformedShapeBounds(Viewport *viewport, short x, short y,
                              unsigned char *shape, short frame,
                              short angle, short scale, short flip,
                              short *bounds)
{
    int absoluteCosine;
    int frameOffset;
    int intersects = 0;

    if (shape == 0) {
        intersects = viewport->left <= x && x <= viewport->right &&
                     viewport->top <= y && y <= viewport->bottom;
        return intersects;
    }
    CheckHeapBlockSignature(shape);
    frameOffset = frame * 4 + 4;
    if (frameOffset <= (int)*(unsigned short *)(shape + 4)) {
        ShapeFrameExtents *frameData;
        int leftExtent;
        int rightExtent;
        int topExtent;
        int bottomExtent;
        int absoluteSine;
        int horizontalSpan;
        int verticalSpan;
        int transformedHeight;
        int transformedWidth;
        int left;
        short top;
        short right;
        short bottom;

        frameData = (ShapeFrameExtents *)(
            shape + *(int *)(shape + frameOffset));
        leftExtent = frameData->left;
        rightExtent = frameData->right;
        topExtent = frameData->top;
        bottomExtent = frameData->bottom;
        absoluteCosine =
            (int)(awAbsoluteCosine[angle] * scale) >> 8;
        absoluteSine =
            (int)(awAbsoluteSine[angle] * scale) >> 8;
        if (absoluteCosine == 0)
            absoluteCosine++;
        if (absoluteSine == 0)
            absoluteSine++;
        horizontalSpan = rightExtent + leftExtent;
        verticalSpan = topExtent + bottomExtent;
        transformedHeight =
            absoluteSine * horizontalSpan +
            absoluteCosine * verticalSpan;
        if ((char)transformedHeight != 0)
            transformedHeight += 0x100;
        transformedHeight >>= 8;
        transformedWidth =
            absoluteCosine * horizontalSpan +
            absoluteSine * verticalSpan;
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
        left = ((absoluteSine * topExtent >> 8) -
                (absoluteCosine * leftExtent >> 8) + x) -
               ((absoluteSine * verticalSpan >> 8) + 1);
        right = (short)(transformedWidth + left);
        if (viewport->left <= right && (short)left <= viewport->right &&
            viewport->top <= bottom && top <= viewport->bottom) {
            bounds[0] = (short)left;
            bounds[2] = right;
            bounds[1] = top;
            bounds[3] = bottom;
            intersects = 1;
        } else {
            intersects = 0;
        }
    }
    (void)flip;
    return intersects;
}

/* Function start: 0x442200 */
void BlitViewportScanlineRuns(Viewport *source, Viewport *destination,
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
        if (destination->pixels == stScreenView.pixels)
            DIBslam();
    }
}

/* Function start: 0x4422D0 */
void fizzle_fade(Viewport *source, Viewport *destination)
{
    if (destination->pixels == stScreenView.pixels)
        DIBslam();
    RasterLineHook("fizzle_fade");
    (void)source;
}

/* Function start: 0x442300 */
void snow_viewport(Viewport *viewport, int effect, unsigned short colour)
{
#ifdef SDL_PORT
    /* Retail draws nothing here: the Kilrathi Saga port emptied the body
     * down to the DIB slam and the RasterLineHook name marker, which is
     * why a knocked-out display shows no static even though malf_noise
     * still plays the sound. */
    SdlDrawViewportStatic(viewport, effect, colour);
#else
    (void)effect;
    (void)colour;
#endif
    if (viewport->pixels == stScreenView.pixels)
        DIBslam();
    RasterLineHook("snow_viewport");
}

/* Function start: 0x442330 */
void UpdateStreamerStoppedFlag(void)
{
    if (bIxAudioEnabled != 0)
    nMusicTrackComplete =
        (dwIxStreamerStatus & 4) == 0;
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
        sprintf(szStreamerPathBuffer, "%s%s",
                streamsDirectory, streamName);
        SoundDebugPrintf("Streamer_open %s", szStreamerPathBuffer);
        ix_streamer_open_stream_file(szStreamerPathBuffer);
        bStreamerAudioRunning = 0;
    }
}

/* Function start: 0x442430 */
void Streamer_play(void)
{
    if (bIxAudioEnabled != 0 &&
        bStreamerAudioRunning == 0) {
        bStreamerAudioRunning = 1;
        SoundDebugPrintf("Streamer_play");
        ix_streamer_audio_play();
    }
}

/* Function start: 0x442460 */
void Streamer_stop(void)
{
    if (bIxAudioEnabled != 0 &&
        bStreamerAudioRunning != 0) {
        SoundDebugPrintf("Streamer_stop");
        ix_streamer_audio_stop();
        bStreamerAudioRunning = 0;
    }
}

/* Function start: 0x442490 */
int GetStreamerTrigger(void)
{
    if (bIxAudioEnabled == 0)
        return 0;
    return ix_streamer_get_trigger();
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
        if (bStreamerAudioRunning == 0) {
            ForceStreamerTrigger(trigger);
            return;
        }
        SoundDebugPrintf("Streamer_trigger %d", trigger);
        if (trigger >= 0)
            ix_streamer_set_trigger((char)trigger);
        if (bStreamerAudioRunning == 0)
            Streamer_play();
    }
}

/* Function start: 0x442520 */
void SetStreamerIntensity(unsigned char intensity)
{
    if (bIxAudioEnabled != 0) {
        ix_streamer_set_intensity(intensity);
        if (bStreamerAudioRunning == 0)
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
        if (bStreamerAudioRunning == 0)
            Streamer_play();
    }
}

/* Function start: 0x442590 */
void SetMusicStreamVolume(unsigned short volume)
{
    if (bIxAudioEnabled != 0)
        ix_streamer_set_volume(volume);
}

/* Function start: 0x4425B0 */
unsigned int GetMusicStreamVolume(void)
{
    if (bIxAudioEnabled == 0)
        return 0;
    return ix_streamer_get_volume();
}

/* Function start: 0x4425D0 */
void Streamer_close(void)
{
    if (bIxAudioEnabled != 0) {
        SoundDebugPrintf("Streamer_close");
        ix_streamer_close_stream_file();
    }
}

/* Function start: 0x4425F0 */
void ShutdownAudioStreamer(void)
{
    if (bIxAudioEnabled != 0)
        ix_streamer_destroy();
}

/* Function start: 0x442600 */
int ReadCheaterFlagFromRegistry(void)
{
    HKEY key;
    DWORD type;
    DWORD size;
    int enabled = 0;

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      szGameRegistryKey,
                      0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS) {
        type = REG_DWORD;
        size = sizeof(int);
        RegQueryValueExA(key, szCheaterRegistryValue, 0, &type,
                         (LPBYTE)&enabled, &size);
        RegCloseKey(key);
    }
    if (enabled != 0)
        OutputDebugStringA("reg cheating\n");
    else
        OutputDebugStringA("reg not cheating\n");
    return enabled;
}
