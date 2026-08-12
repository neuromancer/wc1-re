#include "wc1.h"

#include <string.h>

int main(void)
{
    unsigned char tripletPalette[256 * 3];
    unsigned char *pixels;
    unsigned short cached[3];
    short red[3];
    SDL_Window *window;

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
    pixels = GetDIBPixelBuffer();
    if (pixels == 0 || DAT_0047664c != 320 || DAT_00476650 != 200)
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
    if (DAT_00486518 != 0)
        return 1;
    DIBupdate(0, 0, 319, 199);
    memset(tripletPalette, 0, sizeof(tripletPalette));
    tripletPalette[2 * 3 + 1] = 127;
    DIBwholePaletteFromTriplets(tripletPalette);
    GetPaletteEntryAsWords(2, cached);
    if (cached[0] != 0 || cached[1] != 127 || cached[2] != 0)
        return 1;
    DIBunInstall();
    if (GetDIBPixelBuffer() != 0)
        return 1;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
