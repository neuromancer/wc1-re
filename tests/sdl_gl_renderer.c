#include "wc1.h"

#include "video_internal.h"

#include <SDL_opengl.h>

#include <string.h>

typedef void(APIENTRY *Wc1TestGlReadPixelsProc)(GLint x, GLint y,
                                                GLsizei width, GLsizei height,
                                                GLenum format, GLenum type,
                                                void *pixels);
typedef void(APIENTRY *Wc1TestGlReadBufferProc)(GLenum source);

static int CheckSharpBilinearSprite(Viewport *viewport, unsigned char *pixels)
{
    unsigned char *shape;
    unsigned short value;
    int result;

    shape = AllocateTaggedMemory(32, 0x40);
    if (shape == 0)
        return 0;
    memset(shape, 0, 32);
    *(int *)(shape + 4) = 8;
    value = 2;
    memcpy(shape + 16, &value, sizeof(value));
    shape[22] = 1;
    memset(pixels, 0, 320 * 200);
    Wc1SdlBeginSpaceFrame(0, 5, 1, 0);
    result =
        Wc1SdlRecordSpaceSprite(viewport, 160, 100, shape, 0, 0, 0x100, 0);
    Wc1SdlCompleteSpaceFrame();
    DIBslam();
    DIBslamReal();
    Wc1SdlCancelSpaceFrame();
    ReleasePacketHandle(shape);
    return result;
}

static void WriteShapeWord(unsigned char **output, short value)
{
    memcpy(*output, &value, sizeof(value));
    *output += sizeof(value);
}

static unsigned char *CreateTransformTestShape(unsigned char colour)
{
    unsigned char *output;
    unsigned char *shape;
    int row;
    int column;

    shape = AllocateTaggedMemory(96, 0x40);
    if (shape == 0)
        return 0;
    memset(shape, 0, 96);
    *(int *)(shape + 4) = 8;
    output = shape + 8;
    WriteShapeWord(&output, 3);
    WriteShapeWord(&output, 4);
    WriteShapeWord(&output, 2);
    WriteShapeWord(&output, 1);
    row = 0;
    while (row < 4) {
        WriteShapeWord(&output, 16);
        WriteShapeWord(&output, -4);
        WriteShapeWord(&output, (short)(row - 2));
        column = 0;
        while (column < 8) {
            *output++ = colour;
            column++;
        }
        row++;
    }
    WriteShapeWord(&output, 0);
    return shape;
}

static int FindIndexedBounds(const unsigned char *pixels, unsigned char colour,
                             int *left, int *top, int *right, int *bottom)
{
    int x;
    int y;

    *left = 320;
    *top = 200;
    *right = -1;
    *bottom = -1;
    y = 0;
    while (y < 200) {
        x = 0;
        while (x < 320) {
            if (pixels[y * 320 + x] == colour) {
                if (x < *left)
                    *left = x;
                if (x > *right)
                    *right = x;
                if (y < *top)
                    *top = y;
                if (y > *bottom)
                    *bottom = y;
            }
            x++;
        }
        y++;
    }
    return *right >= *left && *bottom >= *top;
}

static int FindRgbaBounds(const unsigned char *pixels, int colour, int *left,
                          int *top, int *right, int *bottom)
{
    int x;
    int y;

    *left = 640;
    *top = 400;
    *right = -1;
    *bottom = -1;
    y = 0;
    while (y < 400) {
        x = 0;
        while (x < 640) {
            const unsigned char *pixel;

            pixel = pixels + ((399 - y) * 640 + x) * 4;
            if ((colour == 1 && pixel[0] > 32 && pixel[0] > pixel[1] * 2 &&
                 pixel[0] > pixel[2] * 2) ||
                (colour == 2 && pixel[1] > 32 && pixel[1] > pixel[0] * 2 &&
                 pixel[1] > pixel[2] * 2)) {
                if (x < *left)
                    *left = x;
                if (x > *right)
                    *right = x;
                if (y < *top)
                    *top = y;
                if (y > *bottom)
                    *bottom = y;
            }
            x++;
        }
        y++;
    }
    return *right >= *left && *bottom >= *top;
}

static int CheckSharpBilinearTransform(Viewport *viewport,
                                       unsigned char *pixels)
{
    Wc1TestGlReadPixelsProc readPixels;
    Wc1TestGlReadBufferProc readBuffer;
    unsigned char expected[320 * 200];
    unsigned char *frame;
    unsigned char *shape;
    int expectedLeft;
    int expectedTop;
    int expectedRight;
    int expectedBottom;
    int actualLeft;
    int actualTop;
    int actualRight;
    int actualBottom;
    int result;

    shape = CreateTransformTestShape(1);
    frame = (unsigned char *)malloc(640 * 400 * 4);
    readPixels =
        (Wc1TestGlReadPixelsProc)SDL_GL_GetProcAddress("glReadPixels");
    readBuffer =
        (Wc1TestGlReadBufferProc)SDL_GL_GetProcAddress("glReadBuffer");
    if (shape == 0 || frame == 0 || readPixels == 0 || readBuffer == 0) {
        free(frame);
        if (shape != 0)
            ReleasePacketHandle(shape);
        return 0;
    }
    memset(pixels, 0, 320 * 200);
    DrawSpriteScaled(viewport, 160, 100, shape, 0, 37, 0x200, 0);
    memcpy(expected, pixels, sizeof(expected));
    result = FindIndexedBounds(expected, 1, &expectedLeft, &expectedTop,
                               &expectedRight, &expectedBottom);
    memset(pixels, 0, 320 * 200);
    Wc1SdlBeginSpaceFrame(0, 5, 1, 0);
    result = result && Wc1SdlRecordSpaceSprite(viewport, 160, 100, shape, 0,
                                               37, 0x200, 0);
    DIBslam();
    DIBslamReal();
    Wc1SdlCompleteSpaceFrame();
    DIBslam();
    DIBslamReal();
    readBuffer(GL_FRONT);
    readPixels(0, 0, 640, 400, GL_RGBA, GL_UNSIGNED_BYTE, frame);
    result = result && FindRgbaBounds(frame, 1, &actualLeft, &actualTop,
                                      &actualRight, &actualBottom);
    result = result && actualLeft >= expectedLeft * 2 - 3 &&
             actualLeft <= expectedLeft * 2 + 3 &&
             actualTop >= expectedTop * 2 - 3 &&
             actualTop <= expectedTop * 2 + 3 &&
             actualRight >= expectedRight * 2 - 3 &&
             actualRight <= expectedRight * 2 + 4 &&
             actualBottom >= expectedBottom * 2 - 3 &&
             actualBottom <= expectedBottom * 2 + 4;
    Wc1SdlCancelSpaceFrame();
    free(frame);
    ReleasePacketHandle(shape);
    return result;
}

static int CheckSharpBilinearDrawList(Viewport *viewport,
                                      unsigned char *pixels)
{
    Wc1TestGlReadPixelsProc readPixels;
    Wc1TestGlReadBufferProc readBuffer;
    unsigned char *firstShape;
    unsigned char *secondShape;
    unsigned char *frame;
    int firstLeft;
    int firstTop;
    int firstRight;
    int firstBottom;
    int secondLeft;
    int secondTop;
    int secondRight;
    int secondBottom;
    int result;

    firstShape = CreateTransformTestShape(1);
    secondShape = CreateTransformTestShape(2);
    frame = (unsigned char *)malloc(640 * 400 * 4);
    readPixels =
        (Wc1TestGlReadPixelsProc)SDL_GL_GetProcAddress("glReadPixels");
    readBuffer =
        (Wc1TestGlReadBufferProc)SDL_GL_GetProcAddress("glReadBuffer");
    if (firstShape == 0 || secondShape == 0 || frame == 0 || readPixels == 0 ||
        readBuffer == 0) {
        free(frame);
        if (firstShape != 0)
            ReleasePacketHandle(firstShape);
        if (secondShape != 0)
            ReleasePacketHandle(secondShape);
        return 0;
    }
    memset(pixels, 0, 320 * 200);
    Wc1SdlBeginSpaceFrame(0, 5, 1, 0);
    result =
        Wc1SdlRecordSpaceSprite(viewport, 80, 70, firstShape, 0, 23, 0x300, 0);
    result = result && Wc1SdlRecordSpaceSprite(viewport, 230, 130, secondShape,
                                               0, -31, 0x500, 0x10);
    Wc1SdlCompleteSpaceFrame();
    DIBslam();
    DIBslamReal();
    readBuffer(GL_FRONT);
    readPixels(0, 0, 640, 400, GL_RGBA, GL_UNSIGNED_BYTE, frame);
    result = result && FindRgbaBounds(frame, 1, &firstLeft, &firstTop,
                                      &firstRight, &firstBottom);
    result = result && FindRgbaBounds(frame, 2, &secondLeft, &secondTop,
                                      &secondRight, &secondBottom);
    result = result && firstRight < secondLeft && firstBottom < secondTop &&
             firstRight - firstLeft > 20 &&
             secondRight - secondLeft > firstRight - firstLeft;
    Wc1SdlCancelSpaceFrame();
    free(frame);
    ReleasePacketHandle(secondShape);
    ReleasePacketHandle(firstShape);
    return result;
}

static ScreenViewportGeometry *
CreateTestViewportGeometry(int originX, int originY, int width, int height)
{
    ScreenViewportGeometry *geometry;
    short *words;
    size_t wordCount;
    int row;

    wordCount = (size_t)height * 3 + 5;
    geometry = (ScreenViewportGeometry *)malloc(wordCount * sizeof(short));
    if (geometry == 0)
        return 0;
    words = (short *)geometry;
    words[0] = (short)width;
    words[1] = (short)height;
    words[2] = (short)originX;
    words[3] = (short)originY;
    row = 0;
    while (row < height) {
        words[4 + row * 3] = (short)originX;
        words[5 + row * 3] = (short)(originY + row);
        words[6 + row * 3] = (short)width;
        row++;
    }
    words[4 + height * 3] = -1;
    return geometry;
}

/* The shipped geometries describe a run of whole rows as one memcpy rather
 * than one entry per row: the mode 4 table at 0x0046dab8 covers 320x128 with
 * the single run (0, 24, 40960). */
static ScreenViewportGeometry *CreateTestViewportBlockGeometry(int originX,
                                                               int originY,
                                                               int width,
                                                               int height)
{
    ScreenViewportGeometry *geometry;
    short *words;

    geometry = (ScreenViewportGeometry *)malloc(8 * sizeof(short));
    if (geometry == 0)
        return 0;
    words = (short *)geometry;
    words[0] = (short)width;
    words[1] = (short)height;
    words[2] = (short)originX;
    words[3] = (short)originY;
    words[4] = (short)originX;
    words[5] = (short)originY;
    words[6] = (short)(width * height);
    words[7] = -1;
    return geometry;
}

/* Every texel of this shape carries a different palette index, so the
 * rendered colour identifies which index the sampler resolved. */
static unsigned char *CreateIndexRampShape(void)
{
    unsigned char *output;
    unsigned char *shape;
    int row;
    int column;

    shape = AllocateTaggedMemory(96, 0x40);
    if (shape == 0)
        return 0;
    memset(shape, 0, 96);
    *(int *)(shape + 4) = 8;
    output = shape + 8;
    WriteShapeWord(&output, 3);
    WriteShapeWord(&output, 4);
    WriteShapeWord(&output, 2);
    WriteShapeWord(&output, 1);
    row = 0;
    while (row < 4) {
        WriteShapeWord(&output, 16);
        WriteShapeWord(&output, -4);
        WriteShapeWord(&output, (short)(row - 2));
        column = 0;
        while (column < 8) {
            *output++ = (unsigned char)(row * 8 + column + 1);
            column++;
        }
        row++;
    }
    WriteShapeWord(&output, 0);
    return shape;
}

static int CheckSharpBilinearPaletteIndices(Viewport *viewport,
                                            unsigned char *pixels)
{
    Wc1TestGlReadPixelsProc readPixels;
    Wc1TestGlReadBufferProc readBuffer;
    unsigned char *frame;
    unsigned char *shape;
    const unsigned char *pixel;
    short entry[3];
    int index;
    int row;
    int column;
    int logicalX;
    int logicalY;
    int deviceX;
    int deviceY;
    int result;

    shape = CreateIndexRampShape();
    frame = (unsigned char *)malloc(640 * 400 * 4);
    readPixels =
        (Wc1TestGlReadPixelsProc)SDL_GL_GetProcAddress("glReadPixels");
    readBuffer =
        (Wc1TestGlReadBufferProc)SDL_GL_GetProcAddress("glReadBuffer");
    if (shape == 0 || frame == 0 || readPixels == 0 || readBuffer == 0) {
        free(frame);
        if (shape != 0)
            ReleasePacketHandle(shape);
        return 0;
    }
    index = 1;
    while (index <= 32) {
        entry[0] = (short)(index * 7);
        entry[1] = (short)(255 - index * 5);
        entry[2] = (short)(index * 3 + 64);
        DIBsetPalette((short)index, entry);
        index++;
    }
    memset(pixels, 0, 320 * 200);
    Wc1SdlBeginSpaceFrame(0, 5, 1, 0);
    result =
        Wc1SdlRecordSpaceSprite(viewport, 160, 100, shape, 0, 0, 0x100, 0);
    Wc1SdlCompleteSpaceFrame();
    DIBslam();
    DIBslamReal();
    readBuffer(GL_FRONT);
    readPixels(0, 0, 640, 400, GL_RGBA, GL_UNSIGNED_BYTE, frame);
    Wc1SdlCancelSpaceFrame();

    row = 0;
    while (row < 4 && result != 0) {
        column = 0;
        while (column < 8) {
            index = row * 8 + column + 1;
            logicalX = 160 - 4 + column;
            logicalY = 100 - 2 + row;
            deviceX = logicalX * 2;
            deviceY = logicalY * 2;
            pixel = frame + ((399 - deviceY) * 640 + deviceX) * 4;
            if (pixel[0] != (unsigned char)(index * 7) ||
                pixel[1] != (unsigned char)(255 - index * 5) ||
                pixel[2] != (unsigned char)(index * 3 + 64)) {
                result = 0;
                break;
            }
            column++;
        }
        row++;
    }
    free(frame);
    ReleasePacketHandle(shape);
    return result;
}

/* RotateRLEImage rejects a rotated frame larger than its 0xfa00 scratch
 * buffer but has no limit on the unrotated fast path. */
static int CheckSharpBilinearRotateLimit(Viewport *viewport,
                                         unsigned char *pixels)
{
    unsigned char *shape;
    short *header;
    int rotated;
    int upright;

    shape = CreateTransformTestShape(1);
    if (shape == 0)
        return 0;
    header = (short *)(shape + *(int *)(shape + 4));
    header[0] = 200;
    header[1] = 200;
    header[2] = 200;
    header[3] = 200;
    memset(pixels, 0, 320 * 200);
    Wc1SdlBeginSpaceFrame(0, 5, 1, 0);
    rotated =
        Wc1SdlRecordSpaceSprite(viewport, 160, 100, shape, 0, 23, 0x300, 0);
    upright =
        Wc1SdlRecordSpaceSprite(viewport, 160, 100, shape, 0, 0, 0x100, 0);
    Wc1SdlCancelSpaceFrame();
    ReleasePacketHandle(shape);
    return rotated == 0 && upright != 0;
}

static int CheckSharpBilinearMultiRowRun(Viewport *viewport,
                                         unsigned char *pixels)
{
    Wc1TestGlReadPixelsProc readPixels;
    Wc1TestGlReadBufferProc readBuffer;
    ScreenViewportGeometry *rowGeometry;
    ScreenViewportGeometry *blockGeometry;
    Viewport localViewport;
    unsigned char *frame;
    unsigned char *shape;
    int rowLeft;
    int rowTop;
    int rowRight;
    int rowBottom;
    int blockLeft;
    int blockTop;
    int blockRight;
    int blockBottom;
    int result;

    shape = CreateTransformTestShape(1);
    rowGeometry = CreateTestViewportGeometry(0, 20, 320, 100);
    blockGeometry = CreateTestViewportBlockGeometry(0, 20, 320, 100);
    frame = (unsigned char *)malloc(640 * 400 * 4);
    readPixels =
        (Wc1TestGlReadPixelsProc)SDL_GL_GetProcAddress("glReadPixels");
    readBuffer =
        (Wc1TestGlReadBufferProc)SDL_GL_GetProcAddress("glReadBuffer");
    if (shape == 0 || rowGeometry == 0 || blockGeometry == 0 || frame == 0 ||
        readPixels == 0 || readBuffer == 0) {
        free(frame);
        free(blockGeometry);
        free(rowGeometry);
        if (shape != 0)
            ReleasePacketHandle(shape);
        return 0;
    }
    localViewport = *viewport;
    localViewport.left = 0;
    localViewport.top = 0;
    localViewport.right = 319;
    localViewport.bottom = 99;

    memset(pixels, 0, 320 * 200);
    Wc1SdlBeginSpaceFrame(rowGeometry, 0, 0, 0);
    result = Wc1SdlRecordSpaceSprite(&localViewport, 160, 60, shape, 0, 23,
                                     0x300, 0);
    Wc1SdlCompleteSpaceFrame();
    DIBslam();
    DIBslamReal();
    readBuffer(GL_FRONT);
    readPixels(0, 0, 640, 400, GL_RGBA, GL_UNSIGNED_BYTE, frame);
    result = result && FindRgbaBounds(frame, 1, &rowLeft, &rowTop, &rowRight,
                                      &rowBottom);
    Wc1SdlCancelSpaceFrame();

    memset(pixels, 0, 320 * 200);
    Wc1SdlBeginSpaceFrame(blockGeometry, 0, 0, 0);
    result = result && Wc1SdlRecordSpaceSprite(&localViewport, 160, 60, shape,
                                               0, 23, 0x300, 0);
    Wc1SdlCompleteSpaceFrame();
    DIBslam();
    DIBslamReal();
    readBuffer(GL_FRONT);
    readPixels(0, 0, 640, 400, GL_RGBA, GL_UNSIGNED_BYTE, frame);
    result = result && FindRgbaBounds(frame, 1, &blockLeft, &blockTop,
                                      &blockRight, &blockBottom);
    result = result && blockLeft == rowLeft && blockTop == rowTop &&
             blockRight == rowRight && blockBottom == rowBottom;
    Wc1SdlCancelSpaceFrame();
    free(frame);
    free(blockGeometry);
    free(rowGeometry);
    ReleasePacketHandle(shape);
    return result;
}

static int CheckStaticViewportHeight(Viewport *viewport,
                                     unsigned char *pixels)
{
    Wc1TestGlReadPixelsProc readPixels;
    Wc1TestGlReadBufferProc readBuffer;
    unsigned char *frame;
    unsigned char *shape;
    int left;
    int top;
    int right;
    int bottom;
    int result;

    shape = CreateTransformTestShape(1);
    frame = (unsigned char *)malloc(640 * 400 * 4);
    readPixels = (Wc1TestGlReadPixelsProc)
        SDL_GL_GetProcAddress("glReadPixels");
    readBuffer = (Wc1TestGlReadBufferProc)
        SDL_GL_GetProcAddress("glReadBuffer");
    if (shape == 0 || frame == 0 || readPixels == 0 || readBuffer == 0) {
        free(frame);
        if (shape != 0)
            ReleasePacketHandle(shape);
        return 0;
    }
    memset(pixels, 0, 320 * 200);
    Wc1SdlBeginSpaceFrame(
        &g_aScreenViewportGeometry_0046dab8[4], 4, 0, 0);
    result = Wc1SdlRecordSpaceSprite(
        viewport, 160, 130, shape, 0, 0, 0x100, 0);
    Wc1SdlCompleteSpaceFrame();
    DIBslam();
    DIBslamReal();
    readBuffer(GL_FRONT);
    readPixels(0, 0, 640, 400, GL_RGBA, GL_UNSIGNED_BYTE, frame);
    result = result && !FindRgbaBounds(
        frame, 1, &left, &top, &right, &bottom);
    Wc1SdlCancelSpaceFrame();
    free(frame);
    ReleasePacketHandle(shape);
    return result;
}

static int CheckSharpBilinearViewportOffset(Viewport *viewport,
                                            unsigned char *pixels)
{
    Wc1TestGlReadPixelsProc readPixels;
    Wc1TestGlReadBufferProc readBuffer;
    ScreenViewportGeometry *geometry;
    Viewport localViewport;
    unsigned char *frame;
    unsigned char *shape;
    int baseLeft;
    int baseTop;
    int baseRight;
    int baseBottom;
    int offsetLeft;
    int offsetTop;
    int offsetRight;
    int offsetBottom;
    int result;

    shape = CreateTransformTestShape(1);
    geometry = CreateTestViewportGeometry(40, 30, 200, 100);
    frame = (unsigned char *)malloc(640 * 400 * 4);
    readPixels =
        (Wc1TestGlReadPixelsProc)SDL_GL_GetProcAddress("glReadPixels");
    readBuffer =
        (Wc1TestGlReadBufferProc)SDL_GL_GetProcAddress("glReadBuffer");
    if (shape == 0 || geometry == 0 || frame == 0 || readPixels == 0 ||
        readBuffer == 0) {
        free(frame);
        free(geometry);
        if (shape != 0)
            ReleasePacketHandle(shape);
        return 0;
    }
    localViewport = *viewport;
    localViewport.left = 0;
    localViewport.top = 0;
    localViewport.right = 199;
    localViewport.bottom = 99;

    memset(pixels, 0, 320 * 200);
    Wc1SdlBeginSpaceFrame(0, 5, 1, 0);
    result = Wc1SdlRecordSpaceSprite(&localViewport, 180, 80, shape, 0, 23,
                                     0x300, 0);
    Wc1SdlCompleteSpaceFrame();
    DIBslam();
    DIBslamReal();
    readBuffer(GL_FRONT);
    readPixels(0, 0, 640, 400, GL_RGBA, GL_UNSIGNED_BYTE, frame);
    result = result && FindRgbaBounds(frame, 1, &baseLeft, &baseTop,
                                      &baseRight, &baseBottom);
    Wc1SdlCancelSpaceFrame();

    memset(pixels, 0, 320 * 200);
    Wc1SdlBeginSpaceFrame(geometry, 0, 0, 0);
    result = result && Wc1SdlRecordSpaceSprite(&localViewport, 180, 80, shape,
                                               0, 23, 0x300, 0);
    Wc1SdlCompleteSpaceFrame();
    DIBslam();
    DIBslamReal();
    readBuffer(GL_FRONT);
    readPixels(0, 0, 640, 400, GL_RGBA, GL_UNSIGNED_BYTE, frame);
    result = result && FindRgbaBounds(frame, 1, &offsetLeft, &offsetTop,
                                      &offsetRight, &offsetBottom);
    result =
        result && offsetLeft - baseLeft >= 78 && offsetLeft - baseLeft <= 82 &&
        offsetTop - baseTop >= 58 && offsetTop - baseTop <= 62 &&
        offsetRight - baseRight >= 78 && offsetRight - baseRight <= 82 &&
        offsetBottom - baseBottom >= 58 && offsetBottom - baseBottom <= 62;
    Wc1SdlCancelSpaceFrame();
    free(frame);
    free(geometry);
    ReleasePacketHandle(shape);
    return result;
}

static int CheckLetterboxedInputMapping(SDL_Window *window)
{
    int logicalX;
    int logicalY;
    int windowHeight;
    int windowWidth;
    int windowX;
    int windowY;
    int result;

    SDL_SetWindowSize(window, 1000, 700);
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    result = windowWidth == 1000 && windowHeight == 700;
    result =
        result && Wc1SdlMapLogicalToWindow(window, 0, 0, &windowX, &windowY);
    result = result && windowX == 20 && windowY == 50;
    result = result &&
             Wc1SdlMapLogicalToWindow(window, 160, 100, &windowX, &windowY);
    result = result && windowX == 500 && windowY == 350;
    result = result &&
             Wc1SdlMapWindowToLogical(window, 20, 50, &logicalX, &logicalY);
    result = result && logicalX == 0 && logicalY == 0;
    result = result &&
             Wc1SdlMapWindowToLogical(window, 500, 350, &logicalX, &logicalY);
    result = result && logicalX == 160 && logicalY == 100;
    SDL_SetWindowSize(window, 640, 400);
    return result;
}

static int RunGlRendererChecks(Viewport *viewport, unsigned char *pixels)
{
    if (!CheckSharpBilinearSprite(viewport, pixels)) {
        fprintf(stderr, "GL sprite recording test failed.\n");
        return 0;
    }
    if (!CheckSharpBilinearTransform(viewport, pixels)) {
        fprintf(stderr, "GL sprite transform test failed.\n");
        return 0;
    }
    if (!CheckSharpBilinearDrawList(viewport, pixels)) {
        fprintf(stderr, "GL draw-list test failed.\n");
        return 0;
    }
    if (!CheckSharpBilinearViewportOffset(viewport, pixels)) {
        fprintf(stderr, "GL viewport-offset test failed.\n");
        return 0;
    }
    if (!CheckSharpBilinearMultiRowRun(viewport, pixels)) {
        fprintf(stderr, "GL viewport-mask test failed.\n");
        return 0;
    }
    if (!CheckStaticViewportHeight(viewport, pixels)) {
        fprintf(stderr, "GL static viewport-height test failed.\n");
        return 0;
    }
    if (!CheckSharpBilinearRotateLimit(viewport, pixels)) {
        fprintf(stderr, "GL rotation-limit test failed.\n");
        return 0;
    }
    if (!CheckSharpBilinearPaletteIndices(viewport, pixels)) {
        fprintf(stderr, "GL palette-index test failed.\n");
        return 0;
    }
    return 1;
}

int main(void)
{
    unsigned char *pixels;
    short colour[3];
    SDL_GLContext probeContext;
    SDL_Window *window;
    Uint32 windowFlags;
    int result;
    int row;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "Skipping GL renderer tests: %s\n", SDL_GetError());
        return 77;
    }
    Wc1SdlSetVideoBackend(WC1_SDL_VIDEO_BACKEND_GL_SHARP_BILINEAR);
    windowFlags = SDL_WINDOW_HIDDEN;
    if (!Wc1SdlConfigureVideoWindow(&windowFlags)) {
        fprintf(stderr, "Skipping GL renderer tests: %s\n", SDL_GetError());
        SDL_Quit();
        return 77;
    }
    window =
        SDL_CreateWindow("WC1 GL renderer test", 0, 0, 640, 400, windowFlags);
    if (window == 0) {
        fprintf(stderr, "Skipping GL renderer tests: %s\n", SDL_GetError());
        SDL_Quit();
        return 77;
    }
    probeContext = SDL_GL_CreateContext(window);
    if (probeContext == 0) {
        fprintf(stderr, "Skipping GL renderer tests: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 77;
    }
    SDL_GL_MakeCurrent(window, 0);
    SDL_GL_DeleteContext(probeContext);
    if (!CheckLetterboxedInputMapping(window)) {
        fprintf(stderr, "GL renderer input mapping test failed.\n");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    DIBinstall((HWND)window);
    SDL_GL_SetSwapInterval(0);
    DAT_005a6ba0.rowOffsets = g_awScreenRowOffsets_005a6540;
    DAT_005a6ba0.left = 0;
    DAT_005a6ba0.top = 0;
    DAT_005a6ba0.right = 319;
    DAT_005a6ba0.bottom = 199;
    row = 0;
    while (row < 202) {
        g_awScreenRowOffsets_005a6540[row] = (unsigned short)(row * 320);
        row++;
    }
    g_stMouseCursorState_0059ab10.viewport = &DAT_005a6ba0;
    pixels = GetDIBPixelBuffer();
    result = pixels != 0 && DAT_0047664c == 320 && DAT_00476650 == 200;
    if (result) {
        colour[0] = 255;
        colour[1] = 0;
        colour[2] = 0;
        DIBsetPalette(1, colour);
        colour[0] = 0;
        colour[1] = 255;
        DIBsetPalette(2, colour);
        result = RunGlRendererChecks(&DAT_005a6ba0, pixels);
    }
    DIBunInstall();
    g_stMouseCursorState_0059ab10.viewport = 0;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return result ? 0 : 1;
}
