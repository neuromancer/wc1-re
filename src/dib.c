/*
 *  DirectDraw back end.
 *
 *  Address range 0x432000-0x43390f (see docs/ORDER.md).
 *  Boundary evidence: PROVEN: the named DirectDraw routines are followed by
 *  DirectDrawResultToText and its compiler-generated switch tables.
 */
#include "wc1.h"

/* Function start: 0x4320E0 */
void SetFpsCapEnabled(void)
{
    long v;

    DAT_0046b1c8 = 1;
    v = (long)(1000.0 / g_fSpaceFlightFrameRate_0046b1cc);
    DAT_0046b1bc = 0;
    DAT_0046b1b8 = (int)v;
}

/* Function start: 0x432110 */
void SetFpsCapDisabled(void)
{
    long v;

    DAT_0046b1c8 = 0;
    v = (long)(1000.0 / g_fCinematicFrameRate_0046b1d0);
    DAT_0046b1bc = 0;
    DAT_0046b1b8 = (int)v;
}

/* Function start: 0x432140 */
void DIBerror(const char *tag, int hr)
{
    char *text = DirectDrawResultToText(hr);

    sprintf(DAT_00486078, "ERROR: %s - (%s)", tag, text);
    COM_RELEASE(DAT_0046b1ac);
    COM_RELEASE(DAT_0046b1a8);
}

/* Function start: 0x432230 */
void DIBpositionWindow(void)
{
    COM_RELEASE(DAT_0046b1ac);
    COM_RELEASE(DAT_0046b1a8);
    (**(void (**)(void *))(*DAT_0046b1a4 + 0x4c))(DAT_0046b1a4);  /* RestoreDisplayMode */
    (**(void (**)(void *))(*DAT_0046b1a4 + 8))(DAT_0046b1a4);     /* Release */
}

/* Function start: 0x4322B0 */
void DIBreInstall(void)
{
    int err;

    if (DAT_00465074 == 0) {
        DAT_0046b1b4 = -1;
    } else {
        (**(void (__stdcall **)(void *, HWND, int))(*DAT_0046b1a4 + 0x50))
            (DAT_0046b1a4, DAT_00486074, 0x13);                   /* SetCooperativeLevel */
        if (DIBcascade(-2, &err) == 0)
            DIBerror("DIBreInstall   DIBcascade Failure", err);
    }
    DIBslam();
    DIBslamReal();
}

/* Function start: 0x432310 */
void DIBinstall(HWND window)
{
    LPDIRECTDRAW directDraw;
    HRESULT result;
    int cascadeResult;

    DAT_00486074 = window;
    result = DirectDrawCreate(0, &directDraw, 0);
    if (result != DD_OK)
        DIBerror("DIBInstall   Create", result);

    result = IDirectDraw_QueryInterface(
        directDraw, &g_guidDirectDraw2_00463118,
        (void **)&DAT_0046b1a4);
    if (result != DD_OK)
        DIBerror("DIBInstall   Unable to acquire DirectDraw2 interface", result);

    if (DAT_00465074 != 0)
        result = IDirectDraw2_SetCooperativeLevel(
            (LPDIRECTDRAW2)DAT_0046b1a4, DAT_00486074, 0x13);
    else
        result = IDirectDraw2_SetCooperativeLevel(
            (LPDIRECTDRAW2)DAT_0046b1a4, DAT_00486074, 0x17);
    if (result != DD_OK)
        DIBerror("DIBmakeInstall   SetCooperativeLevel", result);

    if (DAT_00465074 == 0) {
        DAT_0046b1b4 = -1;
    } else {
        cascadeResult = DIBcascade(-1, 0);
        if (cascadeResult == 0)
            DIBerror("DIBinstall   DIBcascade Failure", result);
    }

    DAT_00486074 = window;
    memset(DAT_00486110, 0, sizeof(DAT_00486110));
    memset(DAT_00476658, 0, sizeof(DAT_00476658));
    DIBmakeDIB();
    DAT_00486518 = 1;
    DAT_00486060 = 0;
}

/* Function start: 0x432410 */
int DIBcascade(int mode, int *reportedResult)
{
    DDSURFACEDESC surface;
    HRESULT result;
    const char *modeText;

    for (;;) {
        if (mode != -2) {
            if (mode == -1)
                DAT_0046b1b4 = 0;
            else
                DAT_0046b1b4++;
        }

        switch (DAT_0046b1b4) {
        case 0:
            result = ((LPDIRECTDRAW2)DAT_0046b1a4)->lpVtbl->SetDisplayMode(
                (LPDIRECTDRAW2)DAT_0046b1a4, 320, 200, 8, 0, 0);
            if (reportedResult != 0)
                *reportedResult = result;
            if (result != DD_OK) {
                reportedResult = 0;
                mode = 0;
                continue;
            }
            modeText = "320x200 achieved...testing\n";
            break;
        case 1:
            result = ((LPDIRECTDRAW2)DAT_0046b1a4)->lpVtbl->SetDisplayMode(
                (LPDIRECTDRAW2)DAT_0046b1a4, 640, 400, 8, 0, 0);
            if (reportedResult != 0)
                *reportedResult = result;
            if (result != DD_OK) {
                reportedResult = 0;
                mode = 0;
                continue;
            }
            modeText = "640x400 achieved...testing\n";
            break;
        case 2:
            result = ((LPDIRECTDRAW2)DAT_0046b1a4)->lpVtbl->SetDisplayMode(
                (LPDIRECTDRAW2)DAT_0046b1a4, 640, 480, 8, 0, 0);
            if (reportedResult != 0)
                *reportedResult = result;
            if (result != DD_OK) {
                reportedResult = 0;
                mode = 0;
                continue;
            }
            modeText = "640x480 achieved...testing\n";
            break;
        default:
            return 0;
        }

        OutputDebugStringA(modeText);
        if (mode == -2) {
            if (DAT_0046b1ac != 0) {
                result = IDirectDrawSurface_Restore(
                    (LPDIRECTDRAWSURFACE)DAT_0046b1ac);
                if (reportedResult != 0)
                    *reportedResult = result;
                if (result != DD_OK)
                    DIBerror("DIBcascade   Unable to restore surface (secondary)",
                             result);
            }

            result = IDirectDrawSurface_Restore(
                (LPDIRECTDRAWSURFACE)DAT_0046b1a8);
            if (reportedResult != 0)
                *reportedResult = result;
            if (result != DD_OK)
                DIBerror("DIBcascade   Unable to restore surface", result);

            result = IDirectDrawSurface_SetPalette(
                (LPDIRECTDRAWSURFACE)DAT_0046b1a8,
                (LPDIRECTDRAWPALETTE)DAT_0046b1b0);
            if (result != DD_OK)
                DIBerror("DIBcascade   CreatePalette", result);
            return 1;
        }

        OutputDebugStringA(" acquiring surface:");
        memset(&surface, 0, sizeof(surface));
        surface.dwSize = sizeof(surface);
        surface.dwFlags = DDSD_CAPS;
        surface.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
        result = IDirectDraw2_CreateSurface(
            (LPDIRECTDRAW2)DAT_0046b1a4, &surface,
            (LPDIRECTDRAWSURFACE *)&DAT_0046b1a8, 0);
        if (reportedResult != 0)
            *reportedResult = result;
        if (result != DD_OK) {
            OutputDebugStringA(" failed\n");
            mode = 0;
            IDirectDraw2_RestoreDisplayMode((LPDIRECTDRAW2)DAT_0046b1a4);
            reportedResult = 0;
            continue;
        }

        OutputDebugStringA(" successful\n locking surface:");
        result = IDirectDrawSurface_Lock(
            (LPDIRECTDRAWSURFACE)DAT_0046b1a8, 0, &surface,
            DDLOCK_WAIT, 0);
        if (reportedResult != 0)
            *reportedResult = result;
        if (result == DD_OK) {
            IDirectDrawSurface_Unlock(
                (LPDIRECTDRAWSURFACE)DAT_0046b1a8, surface.lpSurface);
            IDirectDrawSurface_Release(
                (LPDIRECTDRAWSURFACE)DAT_0046b1a8);
            DAT_0046b1a8 = 0;
            OutputDebugStringA(" successful\n");
            return 1;
        }

        OutputDebugStringA(" failed\n");
        mode = 0;
        IDirectDrawSurface_Release((LPDIRECTDRAWSURFACE)DAT_0046b1a8);
        reportedResult = 0;
        IDirectDraw2_RestoreDisplayMode((LPDIRECTDRAW2)DAT_0046b1a4);
    }
}

/* Function start: 0x432680 */
/* Full teardown: destroy the DIB, drop the clipper and palette, restore the
 * display mode (vtable +0x4c) and release DirectDraw itself (+0x08). */
void DIBunInstall(void)
{
    DIBdestroyDIB();
    COM_RELEASE(DAT_0046b1ac);
    COM_RELEASE(DAT_0046b1a8);
    (**(void (**)(void *))(*DAT_0046b1a4 + 0x4c))(DAT_0046b1a4);
    (**(void (**)(void *))(*DAT_0046b1a4 + 8))(DAT_0046b1a4);
}

/* Function start: 0x4326E0 */
void DIBmakeDIB(void)
{
    DDSURFACEDESC surface;
    PALETTEENTRY entries[256];
    HRESULT result;
    int offset;

    memset(&surface, 0, sizeof(surface));
    surface.dwSize = sizeof(surface);
    surface.dwFlags = DDSD_CAPS;
    surface.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_MODEX;
    result = IDirectDraw2_CreateSurface(
        (LPDIRECTDRAW2)DAT_0046b1a4, &surface,
        (LPDIRECTDRAWSURFACE *)&DAT_0046b1a8, 0);
    if (result != DD_OK)
        DIBerror("DIBmakeDIB   CreateSurface (primary)", result);

    DAT_00486058 = 8;
    offset = 0;
    do {
        entries[offset / 4].peRed = DAT_00486110[offset + 2];
        entries[offset / 4].peGreen = DAT_00486110[offset + 1];
        entries[offset / 4].peBlue = DAT_00486110[offset];
        entries[offset / 4].peFlags = 0;
        offset += 4;
    } while (offset < 0x400);

    result = IDirectDraw2_CreatePalette(
        (LPDIRECTDRAW2)DAT_0046b1a4, DDPCAPS_8BIT, entries,
        (LPDIRECTDRAWPALETTE *)&DAT_0046b1b0, 0);
    if (result != DD_OK)
        DIBerror("DIBmakeDIB   CreatePalette", result);

    result = IDirectDrawSurface_SetPalette(
        (LPDIRECTDRAWSURFACE)DAT_0046b1a8,
        (LPDIRECTDRAWPALETTE)DAT_0046b1b0);
    if (result != DD_OK)
        DIBerror("DIBmakeDIB   CreatePalette", result);

    if (DAT_0046b1b4 > 0) {
        memset(&surface, 0, sizeof(surface));
        surface.dwSize = sizeof(surface);
        surface.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
        surface.dwWidth = 320;
        surface.dwHeight = 200;
        surface.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
        result = IDirectDraw2_CreateSurface(
            (LPDIRECTDRAW2)DAT_0046b1a4, &surface,
            (LPDIRECTDRAWSURFACE *)&DAT_0046b1ac, 0);
        if (result != DD_OK)
            DIBerror("DIBmakeDIB   CreateSurface (secondary)", result);
    }

    DAT_00476644 = 0;
    DAT_00476640 = 0;
    DAT_0047664c = 320;
    DAT_00476650 = 200;
    DAT_00476648 = malloc(64000);
    DAT_005a6ba0.pixels = GetDIBPixelBuffer();
    DAT_005a6ba0.allocation = DAT_005a6ba0.pixels;
    memcpy(DAT_00476648, DAT_00476658,
           DAT_0047664c * DAT_00476650);
}

/* Function start: 0x4328A0 */
void DIBdestroyDIB(void)
{
    int result;

    memcpy(DAT_00476658, (void *)DAT_00476648,
           DAT_0047664c * DAT_00476650);
    if (DAT_0046b1b4 > 0) {
        result = (**(int (__stdcall **)(void *))(*DAT_0046b1ac + 8))
            (DAT_0046b1ac);
        if (result != 0)
            DIBerror("DIBdestroyDIB   secondary->Release", result);
        DAT_0046b1ac = 0;
    }
    if (DAT_0046b1a8 != 0) {
        result = (**(int (__stdcall **)(void *))(*DAT_0046b1a8 + 8))
            (DAT_0046b1a8);
        if (result != 0)
            DIBerror("DIBdestroyDIB   primary->Release", result);
        DAT_0046b1a8 = 0;
    }
    if (DAT_00476648 != 0)
        free((void *)DAT_00476648);
    DAT_00476640 = 0;
    DAT_00476644 = 0;
    DAT_00476648 = 0;
}

/* Function start: 0x432960 */
void DIBslam(void)
{
    DAT_00486518 = 1;
}

/* Function start: 0x432970 */
void DIBslamReal(void)
{
    DDSURFACEDESC surface;
    unsigned char *destination;
    unsigned char *source;
    RECT destinationRect;
    RECT sourceRect;
    HRESULT result;
    int row;
    HDC dc;

    UpdateStreamerStoppedFlag();
    if (DAT_00486518 != 0) {
        memset(&surface, 0, sizeof(surface));
        surface.dwSize = sizeof(surface);

        if (DAT_0059ab23->pixels == DAT_00476648) {
            CaptureMouseCursorBackground();
            DrawMouseCursor();
        }

        if (DAT_0046b1b4 > 0) {
            result = IDirectDrawSurface_Lock(
                (LPDIRECTDRAWSURFACE)DAT_0046b1ac,
                0, &surface, DDLOCK_WAIT, 0);
        } else {
            result = IDirectDrawSurface_Lock(
                (LPDIRECTDRAWSURFACE)DAT_0046b1a8,
                0, &surface, DDLOCK_WAIT, 0);
        }
        if (result != DD_OK) {
            if (result == DDERR_SURFACELOST) {
                if (DAT_0046b1b4 > 0) {
                    result = IDirectDrawSurface_Restore(
                        (LPDIRECTDRAWSURFACE)DAT_0046b1ac);
                    if (result != DD_OK)
                        DIBerror("DIBslamReal   Unable to restore surface (secondary)",
                                 result);
                }
                result = IDirectDrawSurface_Restore(
                    (LPDIRECTDRAWSURFACE)DAT_0046b1a8);
                if (result != DD_OK)
                    DIBerror("DIBslamReal   Unable to restore surface (primary)",
                             result);

                if (DAT_0046b1b4 > 0) {
                    result = IDirectDrawSurface_Lock(
                        (LPDIRECTDRAWSURFACE)DAT_0046b1ac,
                        0, &surface, DDLOCK_WAIT, 0);
                } else {
                    result = IDirectDrawSurface_Lock(
                        (LPDIRECTDRAWSURFACE)DAT_0046b1a8,
                        0, &surface, DDLOCK_WAIT, 0);
                }
                if (result != DD_OK)
                    DIBerror("DIBslamReal   Unable to lock restored surface",
                             result);
            } else {
                IDirectDrawSurface_Unlock(
                    (LPDIRECTDRAWSURFACE)DAT_0046b1a8, surface.lpSurface);
                DIBerror("DIBslamReal   secondary->Lock", result);
            }
        }

        destination = (unsigned char *)surface.lpSurface;
        source = DAT_00476648;
        if (DAT_0046b1b4 <= 0) {
            IDirectDraw2_WaitForVerticalBlank(
                (LPDIRECTDRAW2)DAT_0046b1a4, DDWAITVB_BLOCKBEGIN, 0);
        }
        row = 0;
        if ((int)DAT_00476650 > 0) {
            do {
                memcpy(destination, source, DAT_0047664c);
                destination += surface.lPitch;
                source += DAT_0047664c;
                row++;
            } while ((int)DAT_00476650 > row);
        }

        if (DAT_0046b1b4 > 0) {
            IDirectDrawSurface_Unlock(
                (LPDIRECTDRAWSURFACE)DAT_0046b1ac, surface.lpSurface);
        } else {
            IDirectDrawSurface_Unlock(
                (LPDIRECTDRAWSURFACE)DAT_0046b1a8, surface.lpSurface);
        }

        if (DAT_0059ab23->pixels == DAT_00476648)
            RestoreMouseCursorBackground();

        if (DAT_0046b1b4 > 0) {
            destinationRect.left = 0;
            destinationRect.top = 0;
            destinationRect.right = 639;
            destinationRect.bottom = 399;
            sourceRect.left = 0;
            sourceRect.top = 0;
            sourceRect.right = 319;
            sourceRect.bottom = 199;
            if (DAT_0046b1b4 == 2) {
                destinationRect.top = 40;
                destinationRect.bottom = 439;
            }

            result = IDirectDrawSurface_Blt(
                (LPDIRECTDRAWSURFACE)DAT_0046b1a8, &destinationRect,
                (LPDIRECTDRAWSURFACE)DAT_0046b1ac, &sourceRect,
                DDBLT_WAIT, 0);
            if (result != DD_OK) {
                if (result == DDERR_SURFACELOST) {
                    result = IDirectDrawSurface_Restore(
                        (LPDIRECTDRAWSURFACE)DAT_0046b1a8);
                    if (result != DD_OK)
                        DIBerror("DIBslamReal (BLIT)  Unable to restore surface (primary)",
                                 result);
                    result = IDirectDrawSurface_Restore(
                        (LPDIRECTDRAWSURFACE)DAT_0046b1ac);
                    if (result != DD_OK)
                        DIBerror("DIBslamReal (BLIT)  Unable to restore surface (secondary)",
                                 result);
                    result = IDirectDrawSurface_Blt(
                        (LPDIRECTDRAWSURFACE)DAT_0046b1a8, &destinationRect,
                        (LPDIRECTDRAWSURFACE)DAT_0046b1ac, &sourceRect,
                        DDBLT_WAIT, 0);
                    if (result != DD_OK)
                        DIBerror("DIBslamReal (BLIT)  Unable to blit to restored surface (primary)",
                                 result);
                } else {
                    DIBerror("DIBslamReal   primary->Blt", result);
                }
            }
        }
        DAT_00486518 = 0;
    }

    DAT_00486060++;
    ServiceSoundSystem();
    dc = GetDC(DAT_00486074);
    ThrottleFrameAndDrawFps(dc);
    ReleaseDC(DAT_00486074, dc);
}

/* Function start: 0x432C60 */
void DIBupdate(int left, int top, int right, int bottom)
{
    unsigned char *destination;
    unsigned char *source;
    DDSURFACEDESC surface;
    int width;
    int height;
    HRESULT result;

    if (DAT_0046b1b4 > 0)
        return;
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

    width = right - left + 1;
    height = bottom - top + 1;
    memset(&surface, 0, sizeof(surface));
    surface.dwSize = sizeof(surface);
    result = IDirectDrawSurface_Lock(
        (LPDIRECTDRAWSURFACE)DAT_0046b1a8, 0, &surface,
        DDLOCK_WAIT, 0);
    if (result != DD_OK) {
        IDirectDrawSurface_Unlock(
            (LPDIRECTDRAWSURFACE)DAT_0046b1a8, surface.lpSurface);
        DIBerror("DIBupdate   primary->Lock", result);
    }

    destination = (unsigned char *)surface.lpSurface
                + surface.lPitch * top + left;
    source = DAT_00476648 + DAT_0047664c * top + left;
    IDirectDraw2_WaitForVerticalBlank(
        (LPDIRECTDRAW2)DAT_0046b1a4, DDWAITVB_BLOCKBEGIN, 0);
    while (height > 0) {
        memcpy(destination, source, width);
        destination += surface.lPitch;
        source += DAT_0047664c;
        height--;
    }
    IDirectDrawSurface_Unlock(
        (LPDIRECTDRAWSURFACE)DAT_0046b1a8, surface.lpSurface);
}

/* Function start: 0x432DE0 */
unsigned char *GetDIBPixelBuffer(void)
{
    return DAT_00476648;
}

/* Function start: 0x432E00 */
HWND GetDIBWindowHandle(void)
{
    return DAT_00486074;
}

/* Function start: 0x432E30 */
void CachePaletteEntryFromWords(short index, unsigned short *rgb)
{
    int cacheOffset = index * 4;
    int wordOffset = index * 3;
    unsigned char value;

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

/* Function start: 0x432EA0 */
void DIBramPalette(void)
{
    unsigned char entries[0x400];
    int offset = 0;
    HRESULT result;

    do {
        entries[offset] = DAT_00486110[offset + 2];
        entries[offset + 1] = DAT_00486110[offset + 1];
        entries[offset + 2] = DAT_00486110[offset];
        entries[offset + 3] = 0;
        offset += 4;
    } while (offset < 0x400);

    result = IDirectDrawPalette_SetEntries(
        (LPDIRECTDRAWPALETTE)DAT_0046b1b0, 0, 0, 256,
        (LPPALETTEENTRY)entries);
    if (result != DD_OK)
        DIBerror("DIBramPalette   SetEntries", result);
}

/* Function start: 0x432F10 */
void DIBsetPalette(short index, short *rgb)
{
    short value;
    int wordOffset;
    int cacheOffset;
    int paletteIndex;
    PALETTEENTRY entry;
    HRESULT result;

    paletteIndex = (int)index;
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

        entry.peBlue = (unsigned char)value;
        entry.peRed = (unsigned char)DAT_005a8a50[wordOffset];
        entry.peGreen = (unsigned char)DAT_005a8a50[wordOffset + 1];
        entry.peFlags = 0;
        result = IDirectDrawPalette_SetEntries(
            (LPDIRECTDRAWPALETTE)DAT_0046b1b0, 0, paletteIndex, 1, &entry);
        if (result != DD_OK)
            DIBerror("DIBsetPalette   SetEntries", result);

        result = IDirectDrawSurface_SetPalette(
            (LPDIRECTDRAWSURFACE)DAT_0046b1a8,
            (LPDIRECTDRAWPALETTE)DAT_0046b1b0);
        if (result != DD_OK)
            DIBerror("DIBmakeDIB   CreatePalette", result);
    }
}

/* Function start: 0x433020 */
void GetPaletteEntryAsWords(short i, unsigned short *rgb)
{
    extern unsigned char DAT_00486110[];
    int k = i * 4;

    rgb[0] = DAT_00486110[k + 2];
    rgb[1] = DAT_00486110[k + 1];
    rgb[2] = DAT_00486110[k];
}

/* Function start: 0x433060 */
void DIBwholePaletteFromTriplets(unsigned char *palette)
{
    unsigned char entries[0x400];
    int offset = 0;
    int error;

    (**(int (__stdcall **)(void *, int, int))(*DAT_0046b1a4 + 0x58))
        (DAT_0046b1a4, 1, 0);
    do {
        unsigned char value = palette[0];

        palette = palette + 3;
        DAT_00486110[offset + 2] = value;
        entries[offset] = value;
        value = palette[-2];
        DAT_00486110[offset + 1] = value;
        entries[offset + 1] = value;
        value = palette[-1];
        DAT_00486110[offset] = value;
        entries[offset + 2] = value;
        entries[offset + 3] = 0;
        DAT_00486110[offset + 3] = 4;
        offset = offset + 4;
    } while (offset < 0x400);

    error = (**(int (__stdcall **)(void *, int, int, int, void *))
             (*DAT_0046b1b0 + 0x18))
        (DAT_0046b1b0, 0, 0, 0x100, entries);
    if (error != 0)
        DIBerror("DIBsetWholePalette   SetEntries", error);

    error = (**(int (__stdcall **)(void *, void *))(*DAT_0046b1a8 + 0x7c))
        (DAT_0046b1a8, DAT_0046b1b0);
    if (error != 0)
        DIBerror("DIBmakeDIB   CreatePalette", error);
}

/* Function start: 0x433120 */
void DIBwholePaletteFromWords(unsigned short *palette)
{
    unsigned char entries[0x400];
    int offset = 0;
    int error;

    (**(int (__stdcall **)(void *, int, int))(*DAT_0046b1a4 + 0x58))
        (DAT_0046b1a4, 1, 0);
    do {
        unsigned char value = *(unsigned char *)palette;

        palette = palette + 3;
        DAT_00486110[offset + 2] = value;
        entries[offset] = value;
        value = *(unsigned char *)(palette - 2);
        DAT_00486110[offset + 1] = value;
        entries[offset + 1] = value;
        value = *(unsigned char *)(palette - 1);
        DAT_00486110[offset] = value;
        entries[offset + 2] = value;
        entries[offset + 3] = 0;
        DAT_00486110[offset + 3] = 4;
        offset = offset + 4;
    } while (offset < 0x400);

    error = (**(int (__stdcall **)(void *, int, int, int, void *))
             (*DAT_0046b1b0 + 0x18))
        (DAT_0046b1b0, 0, 0, 0x100, entries);
    if (error != 0)
        DIBerror("DIBsetWholePalette   SetEntries", error);

    error = (**(int (__stdcall **)(void *, void *))(*DAT_0046b1a8 + 0x7c))
        (DAT_0046b1a8, DAT_0046b1b0);
    if (error != 0)
        DIBerror("DIBmakeDIB   CreatePalette", error);
}

/* Function start: 0x4331E0 */
void DIBwaitForVerticalBlank(void)
{
    (**(void (__stdcall **)(void *, int, int))(*DAT_0046b1a4 + 0x58))
        (DAT_0046b1a4, 1, 0);
}

/* Function start: 0x4331F0 */
char *DirectDrawResultToText(int result)
{
    switch (result) {
    case DDERR_ALREADYINITIALIZED:
        return "DDERR_ALREADYINITIALIZED";
    case DDERR_CANNOTATTACHSURFACE:
        return "DDERR_CANNOTATTACHSURFACE";
    case DDERR_CANNOTDETACHSURFACE:
        return "DDERR_CANNOTDETACHSURFACE";
    case DDERR_CURRENTLYNOTAVAIL:
        return "DDERR_CURRENTLYNOTAVAIL";
    case DDERR_EXCEPTION:
        return "DDERR_EXCEPTION";
    case DDERR_GENERIC:
        return "DDERR_GENERIC";
    case DDERR_HEIGHTALIGN:
        return "DDERR_HEIGHTALIGN";
    case DDERR_INCOMPATIBLEPRIMARY:
        return "DDERR_INCOMPATIBLEPRIMARY";
    case DDERR_INVALIDCAPS:
        return "DDERR_INVALIDCAPS";
    case DDERR_INVALIDCLIPLIST:
        return "DDERR_INVALIDCLIPLIST";
    case DDERR_INVALIDMODE:
        return "DDERR_INVALIDMODE";
    case DDERR_INVALIDOBJECT:
        return "DDERR_INVALIDOBJECT";
    case DDERR_INVALIDPARAMS:
        return "DDERR_INVALIDPARAMS";
    case DDERR_INVALIDPIXELFORMAT:
        return "DDERR_INVALIDPIXELFORMAT";
    case DDERR_INVALIDRECT:
        return "DDERR_INVALIDRECT";
    case DDERR_LOCKEDSURFACES:
        return "DDERR_LOCKEDSURFACES";
    case DDERR_NO3D:
        return "DDERR_NO3D";
    case DDERR_NOALPHAHW:
        return "DDERR_NOALPHAHW";
    case DDERR_NOCLIPLIST:
        return "DDERR_NOCLIPLIST";
    case DDERR_NOCOLORCONVHW:
        return "DDERR_NOCOLORCONVHW";
    case DDERR_NOCOOPERATIVELEVELSET:
        return "DDERR_NOCOOPERATIVELEVELSET";
    case DDERR_NOCOLORKEY:
        return "DDERR_NOCOLORKEY";
    case DDERR_NOCOLORKEYHW:
        return "DDERR_NOCOLORKEYHW";
    case DDERR_NODIRECTDRAWSUPPORT:
        return "DDERR_NODIRECTDRAWSUPPORT";
    case DDERR_NOEXCLUSIVEMODE:
        return "DDERR_NOEXCLUSIVEMODE";
    case DDERR_NOFLIPHW:
        return "DDERR_NOFLIPHW";
    case DDERR_NOGDI:
        return "DDERR_NOGDI";
    case DDERR_NOMIRRORHW:
        return "DDERR_NOMIRRORHW";
    case DDERR_NOTFOUND:
        return "DDERR_NOTFOUND";
    case DDERR_NOOVERLAYHW:
        return "DDERR_NOOVERLAYHW";
    case DDERR_NORASTEROPHW:
        return "DDERR_NORASTEROPHW";
    case DDERR_NOROTATIONHW:
        return "DDERR_NOROTATIONHW";
    case DDERR_NOSTRETCHHW:
        return "DDERR_NOSTRETCHHW";
    case DDERR_NOT4BITCOLOR:
        return "DDERR_NOT4BITCOLOR";
    case DDERR_NOT4BITCOLORINDEX:
        return "DDERR_NOT4BITCOLORINDEX";
    case DDERR_NOT8BITCOLOR:
        return "DDERR_NOT8BITCOLOR";
    case DDERR_NOTEXTUREHW:
        return "DDERR_NOTEXTUREHW";
    case DDERR_NOVSYNCHW:
        return "DDERR_NOVSYNCHW";
    case DDERR_NOZBUFFERHW:
        return "DDERR_NOZBUFFERHW";
    case DDERR_NOZOVERLAYHW:
        return "DDERR_NOZOVERLAYHW";
    case DDERR_OUTOFCAPS:
        return "DDERR_OUTOFCAPS";
    case DDERR_OUTOFMEMORY:
        return "DDERR_OUTOFMEMORY";
    case DDERR_OUTOFVIDEOMEMORY:
        return "DDERR_OUTOFVIDEOMEMORY";
    case DDERR_OVERLAYCANTCLIP:
        return "DDERR_OVERLAYCANTCLIP";
    case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
        return "DDERR_OVERLAYCOLORKEYONLYONEACTIVE";
    case DDERR_PALETTEBUSY:
        return "DDERR_PALETTEBUSY";
    case DDERR_COLORKEYNOTSET:
        return "DDERR_COLORKEYNOTSET";
    case DDERR_SURFACEALREADYATTACHED:
        return "DDERR_SURFACEALREADYATTACHED";
    case DDERR_SURFACEALREADYDEPENDENT:
        return "DDERR_SURFACEALREADYDEPENDENT";
    case DDERR_SURFACEBUSY:
        return "DDERR_SURFACEBUSY";
    case DDERR_CANTLOCKSURFACE:
        return "DDERR_CANTLOCKSURFACE";
    case DDERR_SURFACEISOBSCURED:
        return "DDERR_SURFACEISOBSCURED";
    case DDERR_SURFACELOST:
        return "DDERR_SURFACELOST";
    case DDERR_SURFACENOTATTACHED:
        return "DDERR_SURFACENOTATTACHED";
    case DDERR_TOOBIGHEIGHT:
        return "DDERR_TOOBIGHEIGHT";
    case DDERR_TOOBIGSIZE:
        return "DDERR_TOOBIGSIZE";
    case DDERR_TOOBIGWIDTH:
        return "DDERR_TOOBIGWIDTH";
    case DDERR_UNSUPPORTED:
        return "DDERR_UNSUPPORTED";
    case DDERR_UNSUPPORTEDFORMAT:
        return "DDERR_UNSUPPORTEDFORMAT";
    case DDERR_UNSUPPORTEDMASK:
        return "DDERR_UNSUPPORTEDMASK";
    case DDERR_VERTICALBLANKINPROGRESS:
        return "DDERR_VERTICALBLANKINPROGRESS";
    case DDERR_WASSTILLDRAWING:
        return "DDERR_WASSTILLDRAWING";
    case DDERR_XALIGN:
        return "DDERR_XALIGN";
    case DDERR_INVALIDDIRECTDRAWGUID:
        return "DDERR_INVALIDDIRECTDRAWGUID";
    case DDERR_DIRECTDRAWALREADYCREATED:
        return "DDERR_DIRECTDRAWALREADYCREATED";
    case DDERR_NODIRECTDRAWHW:
        return "DDERR_NODIRECTDRAWHW";
    case DDERR_PRIMARYSURFACEALREADYEXISTS:
        return "DDERR_PRIMARYSURFACEALREADYEXISTS";
    case DDERR_NOEMULATION:
        return "DDERR_NOEMULATION";
    case DDERR_REGIONTOOSMALL:
        return "DDERR_REGIONTOOSMALL";
    case DDERR_CLIPPERISUSINGHWND:
        return "DDERR_CLIPPERISUSINGHWND";
    case DDERR_NOCLIPPERATTACHED:
        return "DDERR_NOCLIPPERATTACHED";
    case DDERR_NOHWND:
        return "DDERR_NOHWND";
    case DDERR_HWNDSUBCLASSED:
        return "DDERR_HWNDSUBCLASSED";
    case DDERR_HWNDALREADYSET:
        return "DDERR_HWNDALREADYSET";
    case DDERR_NOPALETTEATTACHED:
        return "DDERR_NOPALETTEATTACHED";
    case DDERR_NOPALETTEHW:
        return "DDERR_NOPALETTEHW";
    case DDERR_BLTFASTCANTCLIP:
        return "DDERR_BLTFASTCANTCLIP";
    case DDERR_NOBLTHW:
        return "DDERR_NOBLTHW";
    case DDERR_NODDROPSHW:
        return "DDERR_NODDROPSHW";
    case DDERR_OVERLAYNOTVISIBLE:
        return "DDERR_OVERLAYNOTVISIBLE";
    case DDERR_NOOVERLAYDEST:
        return "DDERR_NOOVERLAYDEST";
    case DDERR_INVALIDPOSITION:
        return "DDERR_INVALIDPOSITION";
    case DDERR_NOTAOVERLAYSURFACE:
        return "DDERR_NOTAOVERLAYSURFACE";
    case DDERR_EXCLUSIVEMODEALREADYSET:
        return "DDERR_EXCLUSIVEMODEALREADYSET";
    case DDERR_NOTFLIPPABLE:
        return "DDERR_NOTFLIPPABLE";
    case DDERR_CANTDUPLICATE:
        return "DDERR_CANTDUPLICATE";
    case DDERR_NOTLOCKED:
        return "DDERR_NOTLOCKED";
    case DDERR_CANTCREATEDC:
        return "DDERR_CANTCREATEDC";
    case DDERR_NODC:
        return "DDERR_NODC";
    case DDERR_WRONGMODE:
        return "DDERR_WRONGMODE";
    case DDERR_IMPLICITLYCREATED:
        return "DDERR_IMPLICITLYCREATED";
    case DDERR_NOTPALETTIZED:
        return "DDERR_NOTPALETTIZED";
    case DDERR_UNSUPPORTEDMODE:
        return "DDERR_UNSUPPORTEDMODE";
    case DDERR_NOMIPMAPHW:
        return "DDERR_NOMIPMAPHW";
    case DDERR_INVALIDSURFACETYPE:
        return "DDERR_INVALIDSURFACETYPE";
    case DDERR_DCALREADYCREATED:
        return "DDERR_DCALREADYCREATED";
    case DDERR_CANTPAGELOCK:
        return "DDERR_CANTPAGELOCK";
    case DDERR_CANTPAGEUNLOCK:
        return "DDERR_CANTPAGEUNLOCK";
    case DDERR_NOTPAGELOCKED:
        return "DDERR_NOTPAGELOCKED";
    }
    return "<undefined>";
}
