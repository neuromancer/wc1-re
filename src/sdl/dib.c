/* SDL2 replacement for the recovered DirectDraw presentation boundary. */
#include "wc1.h"

void DIBerror(const char *tag, int result)
{
    fprintf(stderr, "SDL video error: %s - (%d): %s\n", tag, result,
            SDL_GetError());
    exit(1);
}

void DIBpositionWindow(void)
{
    Wc1SdlShutdownVideo();
    if (DAT_00486074 != 0)
        SDL_SetWindowSize((SDL_Window *)DAT_00486074, 320, 200);
}

void DIBreInstall(void)
{
    if (!Wc1SdlInitializeVideo((SDL_Window *)DAT_00486074))
        DIBerror("DIBreInstall", -1);
    DIBslam();
    DIBslamReal();
}

void DIBinstall(HWND window)
{
    DAT_00486074 = window;
    if (!Wc1SdlInitializeVideo((SDL_Window *)window))
        DIBerror("DIBinstall", -1);
    DAT_0046b1b4 = 0;
    memset(DAT_00486110, 0, sizeof(DAT_00486110));
    memset(DAT_00476658, 0, sizeof(DAT_00476658));
    DIBmakeDIB();
    DAT_00486518 = 1;
    DAT_00486060 = 0;
}

int DIBcascade(int mode, int *reportedResult)
{
    DAT_0046b1b4 = 0;
    if (reportedResult != 0)
        *reportedResult = 0;
    return mode < 3;
}

void DIBunInstall(void)
{
    DIBdestroyDIB();
    Wc1SdlShutdownVideo();
    DAT_00486074 = 0;
}

void DIBmakeDIB(void)
{
    DAT_00486058 = 8;
    DAT_00476644 = 0;
    DAT_00476640 = 0;
    DAT_0047664c = 320;
    DAT_00476650 = 200;
    DAT_00476648 = malloc(DAT_0047664c * DAT_00476650);
    if (DAT_00476648 == 0)
        DIBerror("DIBmakeDIB", -1);
    DAT_005a6ba0.pixels = GetDIBPixelBuffer();
    DAT_005a6ba0.allocation = DAT_005a6ba0.pixels;
    memcpy(DAT_00476648, DAT_00476658,
           DAT_0047664c * DAT_00476650);
}

void DIBdestroyDIB(void)
{
    if (DAT_00476648 != 0) {
        memcpy(DAT_00476658, DAT_00476648,
               DAT_0047664c * DAT_00476650);
        free(DAT_00476648);
    }
    DAT_00476640 = 0;
    DAT_00476644 = 0;
    DAT_00476648 = 0;
    DAT_005a6ba0.pixels = 0;
    DAT_005a6ba0.allocation = 0;
}

void DIBslam(void)
{
    DAT_00486518 = 1;
}

void DIBslamReal(void)
{
    if (DAT_00486518 != 0) {
        if (!Wc1SdlPresentIndexedFrame(DAT_00476648, DAT_00486110))
            DIBerror("DIBslamReal", -1);
        DAT_00486518 = 0;
    }
    DAT_00486060++;
}

void DIBupdate(int left, int top, int right, int bottom)
{
    if (left < 0)
        left = 0;
    else if (left > 319)
        return;
    if (right < 0)
        return;
    if (right > 319)
        right = 319;
    if (top < 0)
        top = 0;
    else if (top > 199)
        return;
    if (bottom < 0)
        return;
    if (bottom > 199)
        bottom = 199;
    if (right < left || bottom < top)
        return;
    if (!Wc1SdlPresentIndexedFrame(DAT_00476648, DAT_00486110))
        DIBerror("DIBupdate", -1);
}

unsigned char *GetDIBPixelBuffer(void)
{
    return DAT_00476648;
}

HWND GetDIBWindowHandle(void)
{
    return DAT_00486074;
}

void CachePaletteEntryFromWords(short index, unsigned short *rgb)
{
    int cacheOffset;
    int wordOffset;
    unsigned char value;

    cacheOffset = index * 4;
    wordOffset = index * 3;
    value = *(unsigned char *)&rgb[0];
    DAT_00486110[cacheOffset + 2] = value;
    DAT_005a8a50[wordOffset] = value;
    value = *(unsigned char *)&rgb[1];
    DAT_00486110[cacheOffset + 1] = value;
    DAT_005a8a50[wordOffset + 1] = value;
    value = *(unsigned char *)&rgb[2];
    DAT_00486110[cacheOffset] = value;
    DAT_00486110[cacheOffset + 3] = 1;
    DAT_005a8a50[wordOffset + 2] = value;
}

void DIBramPalette(void)
{
    if (DAT_00476648 != 0 &&
        !Wc1SdlPresentIndexedFrame(DAT_00476648, DAT_00486110))
        DIBerror("DIBramPalette", -1);
}

void DIBsetPalette(short index, short *rgb)
{
    int cacheOffset;
    int paletteIndex;
    int wordOffset;
    short value;

    paletteIndex = index;
    cacheOffset = paletteIndex * 4;
    value = rgb[0];
    if ((int)DAT_00486110[cacheOffset + 2] != (int)value ||
        (int)DAT_00486110[cacheOffset + 1] != (int)rgb[1] ||
        (int)DAT_00486110[cacheOffset] != (int)rgb[2]) {
        DAT_00486110[cacheOffset + 2] = (unsigned char)value;
        wordOffset = paletteIndex * 3;
        DAT_005a8a50[wordOffset] = (unsigned char)value;
        value = *(unsigned char *)&rgb[1];
        DAT_00486110[cacheOffset + 1] = (unsigned char)value;
        DAT_005a8a50[wordOffset + 1] = (unsigned char)value;
        value = *(unsigned char *)&rgb[2];
        DAT_00486110[cacheOffset] = (unsigned char)value;
        DAT_00486110[cacheOffset + 3] = 1;
        DAT_005a8a50[wordOffset + 2] = (unsigned char)value;
        if (DAT_00476648 != 0 &&
            !Wc1SdlPresentIndexedFrame(DAT_00476648, DAT_00486110))
            DIBerror("DIBsetPalette", -1);
    }
}

void GetPaletteEntryAsWords(short index, unsigned short *rgb)
{
    int cacheOffset;

    cacheOffset = index * 4;
    rgb[0] = DAT_00486110[cacheOffset + 2];
    rgb[1] = DAT_00486110[cacheOffset + 1];
    rgb[2] = DAT_00486110[cacheOffset];
}

void DIBwholePaletteFromTriplets(unsigned char *palette)
{
    int offset;

    Wc1SdlWaitForVerticalBlank();
    offset = 0;
    do {
        unsigned char value;

        value = palette[0];
        palette += 3;
        DAT_00486110[offset + 2] = value;
        value = palette[-2];
        DAT_00486110[offset + 1] = value;
        value = palette[-1];
        DAT_00486110[offset] = value;
        DAT_00486110[offset + 3] = 4;
        offset += 4;
    } while (offset < 0x400);
    DIBramPalette();
}

void DIBwholePaletteFromWords(unsigned short *palette)
{
    int offset;

    Wc1SdlWaitForVerticalBlank();
    offset = 0;
    do {
        unsigned char value;

        value = *(unsigned char *)palette;
        palette += 3;
        DAT_00486110[offset + 2] = value;
        value = *(unsigned char *)(palette - 2);
        DAT_00486110[offset + 1] = value;
        value = *(unsigned char *)(palette - 1);
        DAT_00486110[offset] = value;
        DAT_00486110[offset + 3] = 4;
        offset += 4;
    } while (offset < 0x400);
    DIBramPalette();
}

void DIBwaitForVerticalBlank(void)
{
    Wc1SdlWaitForVerticalBlank();
}
