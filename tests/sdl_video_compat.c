#include "wc1.h"

#include <string.h>

static int CheckSpriteCaptureClipping(void)
{
    unsigned int shapeStorage[8];
    unsigned char *shape;
    unsigned char pixels[8];
    unsigned char background[4];
    unsigned char expected[4];
    unsigned short rowOffsets[2];
    Viewport viewport;

    memset(shapeStorage, 0, sizeof(shapeStorage));
    shape = (unsigned char *)shapeStorage;
    shape[4] = 8;
    shape[16] = 4;
    shape[22] = 1;
    shape[23] = 2;

    pixels[0] = 11;
    pixels[1] = 12;
    pixels[2] = 13;
    pixels[3] = 14;
    pixels[4] = 21;
    pixels[5] = 22;
    pixels[6] = 23;
    pixels[7] = 24;
    rowOffsets[0] = 0;
    rowOffsets[1] = 4;
    viewport.pixels = pixels;
    viewport.rowOffsets = rowOffsets;
    viewport.left = 0;
    viewport.top = 0;
    viewport.right = 3;
    viewport.bottom = 1;
    viewport.allocation = pixels;

    shape[20] = 0xff;
    shape[21] = 0xff;
    memset(background, 0x5a, sizeof(background));
    memset(expected, 0x5a, sizeof(expected));
    CaptureSpriteBackground(&viewport, background, 0, 0, shape, 0);
    if (memcmp(background, expected, sizeof(background)) != 0)
        return 0;

    shape[18] = 0xff;
    shape[19] = 0xff;
    shape[20] = 0;
    shape[21] = 0;
    memset(background, 0, sizeof(background));
    CaptureSpriteBackground(&viewport, background, 0, 0, shape, 0);
    return background[0] == pixels[0];
}

static int CheckSpriteRestoreClipping(void)
{
    unsigned int shapeStorage[8];
    unsigned char *shape;
    unsigned char pixels[8];
    unsigned char background[4];
    unsigned char expected[8];
    unsigned short rowOffsets[2];
    Viewport viewport;

    memset(shapeStorage, 0, sizeof(shapeStorage));
    shape = (unsigned char *)shapeStorage;
    shape[4] = 8;
    shape[16] = 4;
    shape[22] = 1;
    shape[23] = 2;

    memset(pixels, 0x2a, sizeof(pixels));
    memcpy(expected, pixels, sizeof(expected));
    memset(background, 0x6b, sizeof(background));
    rowOffsets[0] = 0;
    rowOffsets[1] = 4;
    viewport.pixels = pixels;
    viewport.rowOffsets = rowOffsets;
    viewport.left = 0;
    viewport.top = 0;
    viewport.right = 3;
    viewport.bottom = 1;
    viewport.allocation = pixels;

    shape[20] = 2;
    RestoreSpriteBackground(&viewport, background, 0, 0, shape, 0);
    if (memcmp(pixels, expected, sizeof(pixels)) != 0)
        return 0;

    shape[20] = 0xff;
    shape[21] = 0xff;
    RestoreSpriteBackground(&viewport, background, 0, 0, shape, 0);
    if (memcmp(pixels, expected, sizeof(pixels)) != 0)
        return 0;

    shape[18] = 0xff;
    shape[19] = 0xff;
    shape[20] = 0;
    shape[21] = 0;
    RestoreSpriteBackground(&viewport, background, 0, 0, shape, 0);
    return pixels[0] == background[0];
}

int main(int argumentCount, char **arguments)
{
    unsigned char tripletPalette[256 * 3];
    unsigned char *pixels;
    unsigned short cached[3];
    short red[3];
    SDL_Window *window;

    if (GetFixedOneMillionThunkAlt(0) != 0x3e8000 ||
        GetFixedOneMillionThunk(4) != 0x3e8000)
        return 1;
    if (!CheckSpriteCaptureClipping())
        return 1;
    if (!CheckSpriteRestoreClipping())
        return 1;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0)
        return 1;
    window =
        SDL_CreateWindow("WC1 video test", 0, 0, 640, 400,
                         SDL_WINDOW_HIDDEN);
    if (window == 0) {
        SDL_Quit();
        return 1;
    }
    DIBinstall((HWND)window);
    stMouseCursorState.viewport = &stScreen;
    pixels = GetDIBPixelBuffer();
    if (pixels == 0 || nDIBWidth != 320 || nDIBHeight != 200)
        return 1;
    red[0] = 255;
    red[1] = 0;
    red[2] = 0;
    DIBsetPalette(1, red);
    GetPaletteEntryAsWords(1, cached);
    if (cached[0] != 255 || cached[1] != 0 || cached[2] != 0)
        return 1;
    pixels[0] = 1;
    pixels[319] = 1;
    pixels[320 * 199] = 1;
    pixels[320 * 200 - 1] = 1;
    DIBslam();
    DIBslamReal();
    if (bDIBSlamPending != 0)
        return 1;
    DIBupdate(0, 0, 319, 199);
    memset(tripletPalette, 0, sizeof(tripletPalette));
    tripletPalette[2 * 3 + 1] = 127;
    DIBwholePaletteFromTriplets(tripletPalette);
    GetPaletteEntryAsWords(2, cached);
    if (cached[0] != 0 || cached[1] != 127 || cached[2] != 0)
        return 1;
    DIBunInstall();
    stMouseCursorState.viewport = 0;
    if (GetDIBPixelBuffer() != 0)
        return 1;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
