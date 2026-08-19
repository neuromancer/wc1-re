/*
 *  DirectDraw back end.
 *
 *  Address range 0x432000-0x43390f (see docs/ORDER.md).
 *  Boundary evidence: PROVEN: the named DirectDraw routines are followed by
 *  DirectDrawResultToText and its compiler-generated switch tables.
 */
#include "wc1.h"

/* Function start: 0x432050 */
void ReportSpaceFlightMaxFps(float adjustment)
{
    fSpaceFlightFrameRate += adjustment;
    if (fSpaceFlightFrameRate < 8.0)
        fSpaceFlightFrameRate = 8.0f;
    else if (fSpaceFlightFrameRate > 32.0)
        fSpaceFlightFrameRate = 32.0f;
    sprintf(szSpaceFlightMaxFpsMessage,
            "Space Flight Max FPS : %.1f",
            (double)fSpaceFlightFrameRate);
    SetHudMessageText(szSpaceFlightMaxFpsMessage, 0x50, 0x14);
    if (bSpaceFlightFrameTiming == 1)
        SetSpaceFlightFrameTiming();
}

/* Function start: 0x4320E0 */
void SetSpaceFlightFrameTiming(void)
{
    bSpaceFlightFrameTiming = 1;
    nFrameIntervalMs =
        (long)(1000.0 / fSpaceFlightFrameRate);
    nFrameDeadlineMs = 0;
}

/* Function start: 0x432110 */
void SetCinematicFrameTiming(void)
{
    bSpaceFlightFrameTiming = 0;
    nFrameIntervalMs =
        (long)(1000.0 / fCinematicFrameRate);
    nFrameDeadlineMs = 0;
}

/* Function start: 0x432140 */
void DIBerror(const char *tag, int hr)
{
#ifdef WC1_SDL
    const char *text = SDL_GetError();

    sprintf(szDIBErrorMessage, "ERROR: %s - (%s)", tag, text);
    Wc1SdlShutdownVideo();
    OutputDebugStringA(szDIBErrorMessage);
    if (hDIBWindow != 0)
        SDL_SetWindowSize((SDL_Window *)hDIBWindow, 320, 200);
    MessageBoxA(0, szDIBErrorMessage, "SDL Video Error",
                MB_ICONERROR);
    exit(1);
#else
    FILE *errorFile;
    char *text = DirectDrawResultToText(hr);

    sprintf(szDIBErrorMessage, "ERROR: %s - (%s)", tag, text);
    COM_RELEASE(pSecondarySurface);
    COM_RELEASE(pPrimarySurface);
    IDirectDraw2_RestoreDisplayMode(pDirectDraw2);
    IDirectDraw2_Release(pDirectDraw2);
    OutputDebugStringA(szDIBErrorMessage);
    SetWindowPos(hDIBWindow, HWND_BOTTOM, 0, 0, 320, 200,
                 SWP_SHOWWINDOW);
    errorFile = fopen("direct.err", "wt+");
    fprintf(errorFile, szDIBErrorMessage);
    fclose(errorFile);
    MessageBoxA(0, szDIBErrorMessage, "DirectDraw Error",
                MB_ICONERROR);
    exit(1);
#endif
}

/* Function start: 0x432230 */
void DIBpositionWindow(void)
{
#ifdef WC1_SDL
    Wc1SdlShutdownVideo();
    if (hDIBWindow != 0)
        SDL_SetWindowSize((SDL_Window *)hDIBWindow, 320, 200);
#else
    COM_RELEASE(pSecondarySurface);
    COM_RELEASE(pPrimarySurface);
    IDirectDraw2_RestoreDisplayMode(pDirectDraw2);
    IDirectDraw2_Release(pDirectDraw2);
    SetWindowPos(hDIBWindow, HWND_BOTTOM, 0, 0, 320, 200,
                 SWP_SHOWWINDOW);
#endif
}

/* Function start: 0x4322B0 */
void DIBreInstall(void)
{
#ifndef WC1_SDL
    int err;

    if (bDirectDrawModeCascadeEnabled != 0) {
        IDirectDraw2_SetCooperativeLevel(
            pDirectDraw2, hDIBWindow, 0x13);
        if (DIBcascade(-2, &err) == 0)
            DIBerror("DIBreInstall   DIBcascade Failure", err);
    } else {
        nDIBCascadeLevel = -1;
    }
#else
    if (!Wc1SdlInitializeVideo((SDL_Window *)hDIBWindow))
        DIBerror("DIBreInstall", -1);
#endif
    DIBslam();
    DIBslamReal();
}

/* Function start: 0x432310 */
void DIBinstall(HWND window)
{
#ifndef WC1_SDL
    LPDIRECTDRAW directDraw;
    HRESULT result;
    int cascadeResult;
#endif

    hDIBWindow = window;
#ifdef WC1_SDL
    if (!Wc1SdlInitializeVideo((SDL_Window *)window))
        DIBerror("DIBinstall", -1);
    nDIBCascadeLevel = 0;
#else
    result = DirectDrawCreate(0, &directDraw, 0);
    if (result != DD_OK)
        DIBerror("DIBInstall   Create", result);

    result = IDirectDraw_QueryInterface(
        directDraw, &guidDirectDraw2,
        (void **)&pDirectDraw2);
    if (result != DD_OK)
        DIBerror("DIBInstall   Unable to acquire DirectDraw2 interface", result);

    if (bDirectDrawModeCascadeEnabled != 0)
        result = IDirectDraw2_SetCooperativeLevel(
            pDirectDraw2, hDIBWindow, 0x13);
    else
        result = IDirectDraw2_SetCooperativeLevel(
            pDirectDraw2, hDIBWindow, 0x17);
    if (result != DD_OK)
        DIBerror("DIBmakeInstall   SetCooperativeLevel", result);

    if (bDirectDrawModeCascadeEnabled == 0) {
        nDIBCascadeLevel = -1;
    } else {
        cascadeResult = DIBcascade(-1, 0);
        if (cascadeResult == 0)
            DIBerror("DIBinstall   DIBcascade Failure", result);
    }
#endif

    hDIBWindow = window;
    memset(abDIBPaletteCache, 0,
           sizeof(abDIBPaletteCache));
    memset(abDIBPixelBackup, 0,
           sizeof(abDIBPixelBackup));
    DIBmakeDIB();
    bDIBSlamPending = 1;
    nDIBSlamCount = 0;
}

/* Function start: 0x432410 */
int DIBcascade(int mode, int *reportedResult)
{
#ifdef WC1_SDL
    nDIBCascadeLevel = 0;
    if (reportedResult != 0)
        *reportedResult = 0;
    return mode < 3;
#else
    DDSURFACEDESC surface;
    HRESULT result;
    const char *modeText;

    for (;;) {
        if (mode != -2) {
            if (mode == -1)
                nDIBCascadeLevel = 0;
            else
                nDIBCascadeLevel++;
        }

        switch (nDIBCascadeLevel) {
        case 0:
            result = pDirectDraw2->lpVtbl->SetDisplayMode(
                pDirectDraw2, 320, 200, 8, 0, 0);
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
            result = pDirectDraw2->lpVtbl->SetDisplayMode(
                pDirectDraw2, 640, 400, 8, 0, 0);
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
            result = pDirectDraw2->lpVtbl->SetDisplayMode(
                pDirectDraw2, 640, 480, 8, 0, 0);
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
            if (pSecondarySurface != 0) {
                result = IDirectDrawSurface_Restore(
                    pSecondarySurface);
                if (reportedResult != 0)
                    *reportedResult = result;
                if (result != DD_OK)
                    DIBerror("DIBcascade   Unable to restore surface (secondary)",
                             result);
            }

            result = IDirectDrawSurface_Restore(
                pPrimarySurface);
            if (reportedResult != 0)
                *reportedResult = result;
            if (result != DD_OK)
                DIBerror("DIBcascade   Unable to restore surface", result);

            result = IDirectDrawSurface_SetPalette(
                pPrimarySurface,
                pDirectDrawPalette);
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
            pDirectDraw2, &surface,
            &pPrimarySurface, 0);
        if (reportedResult != 0)
            *reportedResult = result;
        if (result != DD_OK) {
            OutputDebugStringA(" failed\n");
            mode = 0;
            IDirectDraw2_RestoreDisplayMode(pDirectDraw2);
            reportedResult = 0;
            continue;
        }

        OutputDebugStringA(" successful\n locking surface:");
        result = IDirectDrawSurface_Lock(
            pPrimarySurface, 0, &surface,
            DDLOCK_WAIT, 0);
        if (reportedResult != 0)
            *reportedResult = result;
        if (result == DD_OK) {
            IDirectDrawSurface_Unlock(
                pPrimarySurface, surface.lpSurface);
            IDirectDrawSurface_Release(
                pPrimarySurface);
            pPrimarySurface = 0;
            OutputDebugStringA(" successful\n");
            return 1;
        }

        OutputDebugStringA(" failed\n");
        mode = 0;
        IDirectDrawSurface_Release(pPrimarySurface);
        reportedResult = 0;
        IDirectDraw2_RestoreDisplayMode(pDirectDraw2);
    }
#endif
}

/* Function start: 0x432680 */
/* Full teardown: destroy the DIB, release its surfaces, restore the display
 * mode and release DirectDraw itself. */
void DIBunInstall(void)
{
    DIBdestroyDIB();
#ifdef WC1_SDL
    Wc1SdlShutdownVideo();
    hDIBWindow = 0;
#else
    COM_RELEASE(pSecondarySurface);
    COM_RELEASE(pPrimarySurface);
    IDirectDraw2_RestoreDisplayMode(pDirectDraw2);
    IDirectDraw2_Release(pDirectDraw2);
#endif
}

/* Function start: 0x4326E0 */
void DIBmakeDIB(void)
{
#ifndef WC1_SDL
    DDSURFACEDESC surface;
    PALETTEENTRY entries[256];
    HRESULT result;
    int offset;

    memset(&surface, 0, sizeof(surface));
    surface.dwSize = sizeof(surface);
    surface.dwFlags = DDSD_CAPS;
    surface.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_MODEX;
    result = IDirectDraw2_CreateSurface(
        pDirectDraw2, &surface,
        &pPrimarySurface, 0);
    if (result != DD_OK)
        DIBerror("DIBmakeDIB   CreateSurface (primary)", result);
#endif

    DAT_00486058 = 8;
#ifndef WC1_SDL
    offset = 0;
    do {
        entries[offset / 4].peRed = abDIBPaletteCache[offset + 2];
        entries[offset / 4].peGreen = abDIBPaletteCache[offset + 1];
        entries[offset / 4].peBlue = abDIBPaletteCache[offset];
        entries[offset / 4].peFlags = 0;
        offset += 4;
    } while (offset < 0x400);

    result = IDirectDraw2_CreatePalette(
        pDirectDraw2, DDPCAPS_8BIT, entries,
        &pDirectDrawPalette, 0);
    if (result != DD_OK)
        DIBerror("DIBmakeDIB   CreatePalette", result);

    result = IDirectDrawSurface_SetPalette(
        pPrimarySurface,
        pDirectDrawPalette);
    if (result != DD_OK)
        DIBerror("DIBmakeDIB   CreatePalette", result);

    if (nDIBCascadeLevel > 0) {
        memset(&surface, 0, sizeof(surface));
        surface.dwSize = sizeof(surface);
        surface.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
        surface.dwWidth = 320;
        surface.dwHeight = 200;
        surface.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
        result = IDirectDraw2_CreateSurface(
            pDirectDraw2, &surface,
            &pSecondarySurface, 0);
        if (result != DD_OK)
            DIBerror("DIBmakeDIB   CreateSurface (secondary)", result);
    }
#endif

    DAT_00476644 = 0;
    DAT_00476640 = 0;
    nDIBWidth = 320;
    nDIBHeight = 200;
    pDIBPixelBuffer = malloc(64000);
#ifdef WC1_SDL
    if (pDIBPixelBuffer == 0)
        DIBerror("DIBmakeDIB", -1);
#endif
    stScreen.pixels = GetDIBPixelBuffer();
    stScreen.allocation = stScreen.pixels;
    memcpy(pDIBPixelBuffer, abDIBPixelBackup,
           nDIBWidth * nDIBHeight);
}

/* Function start: 0x4328A0 */
void DIBdestroyDIB(void)
{
#ifndef WC1_SDL
    int result;
#endif

#ifdef WC1_SDL
    if (pDIBPixelBuffer != 0) {
#endif
    memcpy(abDIBPixelBackup, pDIBPixelBuffer,
           nDIBWidth * nDIBHeight);
#ifdef WC1_SDL
    }
#endif
#ifndef WC1_SDL
    if (nDIBCascadeLevel > 0) {
        result = IDirectDrawSurface_Release(pSecondarySurface);
        if (result != 0)
            DIBerror("DIBdestroyDIB   secondary->Release", result);
        pSecondarySurface = 0;
    }
    if (pPrimarySurface != 0) {
        result = IDirectDrawSurface_Release(pPrimarySurface);
        if (result != 0)
            DIBerror("DIBdestroyDIB   primary->Release", result);
        pPrimarySurface = 0;
    }
#endif
    if (pDIBPixelBuffer != 0)
        free(pDIBPixelBuffer);
    DAT_00476640 = 0;
    DAT_00476644 = 0;
    pDIBPixelBuffer = 0;
#ifdef WC1_SDL
    stScreen.pixels = 0;
    stScreen.allocation = 0;
#endif
}

/* Function start: 0x432960 */
void DIBslam(void)
{
    bDIBSlamPending = 1;
}

/* Function start: 0x432970 */
void DIBslamReal(void)
{
#ifndef WC1_SDL
    DDSURFACEDESC surface;
    unsigned char *destination;
    unsigned char *source;
    RECT destinationRect;
    RECT sourceRect;
    HRESULT result;
    int row;
    HDC dc;
#endif

    UpdateStreamerStoppedFlag();
    if (bDIBSlamPending != 0) {
#ifndef WC1_SDL
        memset(&surface, 0, sizeof(surface));
        surface.dwSize = sizeof(surface);
#endif

#ifdef WC1_SDL
        if (stMouseCursorState.viewport != 0 &&
            stMouseCursorState.viewport->pixels ==
                pDIBPixelBuffer) {
#else
        if (stMouseCursorState.viewport->pixels ==
                pDIBPixelBuffer) {
#endif
            CaptureMouseCursorBackground();
            DrawMouseCursor();
        }

#ifndef WC1_SDL
        if (nDIBCascadeLevel > 0) {
            result = IDirectDrawSurface_Lock(
                pSecondarySurface,
                0, &surface, DDLOCK_WAIT, 0);
        } else {
            result = IDirectDrawSurface_Lock(
                pPrimarySurface,
                0, &surface, DDLOCK_WAIT, 0);
        }
        if (result != DD_OK) {
            if (result == DDERR_SURFACELOST) {
                if (nDIBCascadeLevel > 0) {
                    result = IDirectDrawSurface_Restore(
                        pSecondarySurface);
                    if (result != DD_OK)
                        DIBerror("DIBslamReal   Unable to restore surface (secondary)",
                                 result);
                }
                result = IDirectDrawSurface_Restore(
                    pPrimarySurface);
                if (result != DD_OK)
                    DIBerror("DIBslamReal   Unable to restore surface (primary)",
                             result);

                if (nDIBCascadeLevel > 0) {
                    result = IDirectDrawSurface_Lock(
                        pSecondarySurface,
                        0, &surface, DDLOCK_WAIT, 0);
                } else {
                    result = IDirectDrawSurface_Lock(
                        pPrimarySurface,
                        0, &surface, DDLOCK_WAIT, 0);
                }
                if (result != DD_OK)
                    DIBerror("DIBslamReal   Unable to lock restored surface",
                             result);
            } else {
                IDirectDrawSurface_Unlock(
                    pPrimarySurface, surface.lpSurface);
                DIBerror("DIBslamReal   secondary->Lock", result);
            }
        }

        destination = surface.lpSurface;
        source = pDIBPixelBuffer;
        if (nDIBCascadeLevel <= 0) {
            IDirectDraw2_WaitForVerticalBlank(
                pDirectDraw2, DDWAITVB_BLOCKBEGIN, 0);
        }
        row = 0;
        if ((int)nDIBHeight > 0) {
            do {
                memcpy(destination, source, nDIBWidth);
                destination += surface.lPitch;
                source += nDIBWidth;
                row++;
            } while ((int)nDIBHeight > row);
        }

        if (nDIBCascadeLevel > 0) {
            IDirectDrawSurface_Unlock(
                pSecondarySurface, surface.lpSurface);
        } else {
            IDirectDrawSurface_Unlock(
                pPrimarySurface, surface.lpSurface);
        }
#else
        if (!Wc1SdlPresentIndexedFrame(pDIBPixelBuffer,
                                       abDIBPaletteCache))
            DIBerror("DIBslamReal", -1);
#endif

#ifdef WC1_SDL
        if (stMouseCursorState.viewport != 0 &&
            stMouseCursorState.viewport->pixels ==
                pDIBPixelBuffer)
#else
        if (stMouseCursorState.viewport->pixels ==
                pDIBPixelBuffer)
#endif
            RestoreMouseCursorBackground();

#ifndef WC1_SDL
        if (nDIBCascadeLevel > 0) {
            destinationRect.left = 0;
            destinationRect.top = 0;
            destinationRect.right = 639;
            destinationRect.bottom = 399;
            sourceRect.left = 0;
            sourceRect.top = 0;
            sourceRect.right = 319;
            sourceRect.bottom = 199;
            if (nDIBCascadeLevel == 2) {
                destinationRect.top = 40;
                destinationRect.bottom = 439;
            }

            result = IDirectDrawSurface_Blt(
                pPrimarySurface, &destinationRect,
                pSecondarySurface, &sourceRect,
                DDBLT_WAIT, 0);
            if (result != DD_OK) {
                if (result == DDERR_SURFACELOST) {
                    result = IDirectDrawSurface_Restore(
                        pPrimarySurface);
                    if (result != DD_OK)
                        DIBerror("DIBslamReal (BLIT)  Unable to restore surface (primary)",
                                 result);
                    result = IDirectDrawSurface_Restore(
                        pSecondarySurface);
                    if (result != DD_OK)
                        DIBerror("DIBslamReal (BLIT)  Unable to restore surface (secondary)",
                                 result);
                    result = IDirectDrawSurface_Blt(
                        pPrimarySurface, &destinationRect,
                        pSecondarySurface, &sourceRect,
                        DDBLT_WAIT, 0);
                    if (result != DD_OK)
                        DIBerror("DIBslamReal (BLIT)  Unable to blit to restored surface (primary)",
                                 result);
                } else {
                    DIBerror("DIBslamReal   primary->Blt", result);
                }
            }
        }
#endif
        bDIBSlamPending = 0;
    }

    nDIBSlamCount++;
    ServiceSoundSystem();
#ifdef WC1_SDL
    ThrottleFrameAndDrawFps(0);
#else
    dc = GetDC(hDIBWindow);
    ThrottleFrameAndDrawFps(dc);
    ReleaseDC(hDIBWindow, dc);
#endif
}

/* Function start: 0x432C60 */
void DIBupdate(int left, int top, int right, int bottom)
{
#ifndef WC1_SDL
    unsigned char *destination;
    unsigned char *source;
    DDSURFACEDESC surface;
    int width;
    int height;
    HRESULT result;
#endif

    if (nDIBCascadeLevel > 0)
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

#ifdef WC1_SDL
    if (right < left || bottom < top)
        return;
    if (!Wc1SdlPresentIndexedFrame(pDIBPixelBuffer,
                                   abDIBPaletteCache))
        DIBerror("DIBupdate", -1);
#else
    width = right - left + 1;
    height = bottom - top + 1;
    memset(&surface, 0, sizeof(surface));
    surface.dwSize = sizeof(surface);
    result = IDirectDrawSurface_Lock(
        pPrimarySurface, 0, &surface,
        DDLOCK_WAIT, 0);
    if (result != DD_OK) {
        IDirectDrawSurface_Unlock(
            pPrimarySurface, surface.lpSurface);
        DIBerror("DIBupdate   primary->Lock", result);
    }

    destination = (unsigned char *)surface.lpSurface
                + surface.lPitch * top + left;
    source = pDIBPixelBuffer + nDIBWidth * top + left;
    IDirectDraw2_WaitForVerticalBlank(
        pDirectDraw2, DDWAITVB_BLOCKBEGIN, 0);
    while (height > 0) {
        memcpy(destination, source, width);
        destination += surface.lPitch;
        source += nDIBWidth;
        height--;
    }
    IDirectDrawSurface_Unlock(
        pPrimarySurface, surface.lpSurface);
#endif
}

/* Function start: 0x432DE0 */
unsigned char *GetDIBPixelBuffer(void)
{
    return pDIBPixelBuffer;
}

/* Function start: 0x432E00 */
HWND GetDIBWindowHandle(void)
{
    return hDIBWindow;
}

/* Function start: 0x432E30 */
void CachePaletteEntryFromWords(short index, unsigned short *rgb)
{
    int cacheOffset = index * 4;
    int wordOffset = index * 3;
    unsigned char value;

    value = *(unsigned char *)&rgb[0];
    abDIBPaletteCache[cacheOffset + 2] = value;
    awPaletteRgbWords[wordOffset] = value;
    value = *(unsigned char *)&rgb[1];
    abDIBPaletteCache[cacheOffset + 1] = value;
    awPaletteRgbWords[wordOffset + 1] = value;
    value = *(unsigned char *)&rgb[2];
    abDIBPaletteCache[cacheOffset] = value;
    abDIBPaletteCache[cacheOffset + 3] = 1;
    awPaletteRgbWords[wordOffset + 2] = value;
}

/* Function start: 0x432EA0 */
void DIBramPalette(void)
{
#ifdef WC1_SDL
    if (pDIBPixelBuffer != 0 &&
        !Wc1SdlPresentIndexedFrame(pDIBPixelBuffer,
                                   abDIBPaletteCache))
        DIBerror("DIBramPalette", -1);
#else
    unsigned char entries[0x400];
    int offset = 0;
    HRESULT result;

    do {
        entries[offset] = abDIBPaletteCache[offset + 2];
        entries[offset + 1] = abDIBPaletteCache[offset + 1];
        entries[offset + 2] = abDIBPaletteCache[offset];
        entries[offset + 3] = 0;
        offset += 4;
    } while (offset < 0x400);

    result = IDirectDrawPalette_SetEntries(
        pDirectDrawPalette, 0, 0, 256,
        (LPPALETTEENTRY)entries);
    if (result != DD_OK)
        DIBerror("DIBramPalette   SetEntries", result);
#endif
}

/* Function start: 0x432F10 */
void DIBsetPalette(short index, short *rgb)
{
    short value;
    int wordOffset;
    int cacheOffset;
    int paletteIndex;
#ifndef WC1_SDL
    PALETTEENTRY entry;
    HRESULT result;
#endif

    paletteIndex = (int)index;
    cacheOffset = paletteIndex * 4;
    value = rgb[0];
    if ((int)abDIBPaletteCache[cacheOffset + 2] != (int)value ||
        (int)abDIBPaletteCache[cacheOffset + 1] != (int)rgb[1] ||
        (int)abDIBPaletteCache[cacheOffset] != (int)rgb[2]) {
        abDIBPaletteCache[cacheOffset + 2] = (unsigned char)value;
        wordOffset = paletteIndex * 3;
        awPaletteRgbWords[wordOffset] = (unsigned char)value;
        value = *(unsigned char *)&rgb[1];
        abDIBPaletteCache[cacheOffset + 1] = (unsigned char)value;
        awPaletteRgbWords[wordOffset + 1] = (unsigned char)value;
        value = *(unsigned char *)&rgb[2];
        abDIBPaletteCache[cacheOffset] = (unsigned char)value;
        abDIBPaletteCache[cacheOffset + 3] = 1;
        awPaletteRgbWords[wordOffset + 2] = (unsigned char)value;

        /* WC1_SDL consumes this cache on the next normal frame submission.
           A DirectDraw palette entry update did not blit or wait for vertical
           blank, so flight fades must not submit additional SDL frames. */
#ifndef WC1_SDL
        entry.peBlue = (unsigned char)value;
        entry.peRed = (unsigned char)awPaletteRgbWords[wordOffset];
        entry.peGreen =
            (unsigned char)awPaletteRgbWords[wordOffset + 1];
        entry.peFlags = 0;
        result = IDirectDrawPalette_SetEntries(
            pDirectDrawPalette, 0, paletteIndex, 1, &entry);
        if (result != DD_OK)
            DIBerror("DIBsetPalette   SetEntries", result);

        result = IDirectDrawSurface_SetPalette(
            pPrimarySurface,
            pDirectDrawPalette);
        if (result != DD_OK)
            DIBerror("DIBmakeDIB   CreatePalette", result);
#endif
    }
}

/* Function start: 0x433020 */
void GetPaletteEntryAsWords(short i, unsigned short *rgb)
{
    extern unsigned char abDIBPaletteCache[];
    int k = i * 4;

    rgb[0] = abDIBPaletteCache[k + 2];
    rgb[1] = abDIBPaletteCache[k + 1];
    rgb[2] = abDIBPaletteCache[k];
}

/* Function start: 0x433060 */
void DIBwholePaletteFromTriplets(unsigned char *palette)
{
#ifndef WC1_SDL
    unsigned char entries[0x400];
#endif
    int offset = 0;
#ifndef WC1_SDL
    int error;

    IDirectDraw2_WaitForVerticalBlank(
        pDirectDraw2, DDWAITVB_BLOCKBEGIN, 0);
#else
    Wc1SdlWaitForVerticalBlank();
#endif
    do {
        unsigned char value = palette[0];

        palette = palette + 3;
        abDIBPaletteCache[offset + 2] = value;
#ifndef WC1_SDL
        entries[offset] = value;
#endif
        value = palette[-2];
        abDIBPaletteCache[offset + 1] = value;
#ifndef WC1_SDL
        entries[offset + 1] = value;
#endif
        value = palette[-1];
        abDIBPaletteCache[offset] = value;
#ifndef WC1_SDL
        entries[offset + 2] = value;
        entries[offset + 3] = 0;
#endif
        abDIBPaletteCache[offset + 3] = 4;
        offset = offset + 4;
    } while (offset < 0x400);

#ifdef WC1_SDL
    DIBramPalette();
#else
    error = IDirectDrawPalette_SetEntries(
        pDirectDrawPalette, 0, 0, 0x100,
        (LPPALETTEENTRY)entries);
    if (error != 0)
        DIBerror("DIBsetWholePalette   SetEntries", error);

    error = IDirectDrawSurface_SetPalette(
        pPrimarySurface, pDirectDrawPalette);
    if (error != 0)
        DIBerror("DIBmakeDIB   CreatePalette", error);
#endif
}

/* Function start: 0x433120 */
void DIBwholePaletteFromWords(unsigned short *palette)
{
#ifndef WC1_SDL
    unsigned char entries[0x400];
#endif
    int offset = 0;
#ifndef WC1_SDL
    int error;

    IDirectDraw2_WaitForVerticalBlank(
        pDirectDraw2, DDWAITVB_BLOCKBEGIN, 0);
#else
    Wc1SdlWaitForVerticalBlank();
#endif
    do {
        unsigned char value = *(unsigned char *)palette;

        palette = palette + 3;
        abDIBPaletteCache[offset + 2] = value;
#ifndef WC1_SDL
        entries[offset] = value;
#endif
        value = *(unsigned char *)(palette - 2);
        abDIBPaletteCache[offset + 1] = value;
#ifndef WC1_SDL
        entries[offset + 1] = value;
#endif
        value = *(unsigned char *)(palette - 1);
        abDIBPaletteCache[offset] = value;
#ifndef WC1_SDL
        entries[offset + 2] = value;
        entries[offset + 3] = 0;
#endif
        abDIBPaletteCache[offset + 3] = 4;
        offset = offset + 4;
    } while (offset < 0x400);

#ifdef WC1_SDL
    DIBramPalette();
#else
    error = IDirectDrawPalette_SetEntries(
        pDirectDrawPalette, 0, 0, 0x100,
        (LPPALETTEENTRY)entries);
    if (error != 0)
        DIBerror("DIBsetWholePalette   SetEntries", error);

    error = IDirectDrawSurface_SetPalette(
        pPrimarySurface, pDirectDrawPalette);
    if (error != 0)
        DIBerror("DIBmakeDIB   CreatePalette", error);
#endif
}

/* Function start: 0x4331E0 */
void DIBwaitForVerticalBlank(void)
{
#ifdef WC1_SDL
    Wc1SdlWaitForVerticalBlank();
#else
    IDirectDraw2_WaitForVerticalBlank(
        pDirectDraw2, DDWAITVB_BLOCKBEGIN, 0);
#endif
}

#ifndef WC1_SDL

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

#endif
