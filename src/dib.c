/*
 *  DirectDraw back end.
 *
 *  Address range 431f00-43390f (see docs/ORDER.md).
 *  Boundary evidence: PROVEN: the named DirectDraw routines are followed by
 *  DirectDrawResultToText and its compiler-generated switch tables.
 *  ThrottleFrameAndDrawFps belongs here: it uses this unit's statics
 *  szFrameRateLabel and fSampledFps, and its 1000.0 constant follows
 *  the unit's DirectDraw GUIDs in .rdata.
 */
#include "wc1.h"
#ifndef SDL_PORT
#include <sys/stat.h>
#endif

static LPDIRECTDRAW2 pDirectDraw2 = 0; /* 0x0046b1a4 */
static LPDIRECTDRAWSURFACE pPrimarySurface = 0; /* 0x0046b1a8 */
static LPDIRECTDRAWSURFACE pSecondarySurface = 0; /* 0x0046b1ac */
static LPDIRECTDRAWPALETTE pDirectDrawPalette = 0; /* 0x0046b1b0 */
static int nDIBCascadeLevel = -1; /* 0x0046b1b4 */
int nFrameIntervalMs = 62; /* 0x0046b1b8 */
static int nFrameDeadlineMs = 0; /* 0x0046b1bc */
/* Nothing in the shipped executable references this zero word. */
static int DAT_0046b1c0 = 0; /* 0x0046b1c0 */
static int nPreviousFrameTimeMs = 0; /* 0x0046b1c4 */
static int bSpaceFlightFrameTiming = 0; /* 0x0046b1c8 */
static float fSpaceFlightFrameRate = 20.0f; /* 0x0046b1cc */
static float fCinematicFrameRate = 16.0f; /* 0x0046b1d0 */
/* Nothing in the shipped executable references this zero word either. */
static int DAT_0046b1d4 = 0; /* 0x0046b1d4 */

/* DirectDraw's SDK identifiers occur in this order before the frame-timing
 * constants. The original SDK also emitted the unused class/interface IDs. */
const GUID CLSID_DirectDraw = {
    0xd7b70ee0, 0x4340, 0x11cf,
    { 0xb0, 0x63, 0x00, 0x20, 0xaf, 0xc2, 0xcd, 0x35 }
}; /* 0x004630e8 */
const GUID CLSID_DirectDrawClipper = {
    0x593817a0, 0x7db3, 0x11cf,
    { 0xa2, 0xde, 0x00, 0xaa, 0x00, 0xb9, 0x33, 0x56 }
}; /* 0x004630f8 */
const GUID IID_IDirectDraw = {
    0x6c14db80, 0xa733, 0x11ce,
    { 0xa5, 0x21, 0x00, 0x20, 0xaf, 0x0b, 0xe5, 0x60 }
}; /* 0x00463108 */
const GUID guidDirectDraw2 = {
    0xb3a6f3e0, 0x2b43, 0x11cf,
    { 0xa2, 0xde, 0x00, 0xaa, 0x00, 0xb9, 0x33, 0x56 }
}; /* 0x00463118 */
const GUID IID_IDirectDrawSurface = {
    0x6c14db81, 0xa733, 0x11ce,
    { 0xa5, 0x21, 0x00, 0x20, 0xaf, 0x0b, 0xe5, 0x60 }
}; /* 0x00463128 */
const GUID IID_IDirectDrawSurface2 = {
    0x57805885, 0x6eec, 0x11cf,
    { 0x94, 0x41, 0xa8, 0x23, 0x03, 0xc1, 0x0e, 0x27 }
}; /* 0x00463138 */
const GUID IID_IDirectDrawPalette = {
    0x6c14db84, 0xa733, 0x11ce,
    { 0xa5, 0x21, 0x00, 0x20, 0xaf, 0x0b, 0xe5, 0x60 }
}; /* 0x00463148 */
const GUID IID_IDirectDrawClipper = {
    0x6c14db85, 0xa733, 0x11ce,
    { 0xa5, 0x21, 0x00, 0x20, 0xaf, 0x0b, 0xe5, 0x60 }
}; /* 0x00463158 */

/* Frame buffers, DirectDraw window and palette state, and frame-rate text.
 * CL places these statics by identifier hash, so their spellings reproduce
 * the original order. The unused entries are unreferenced original storage;
 * the window handle is also read by the SDL event adapter. Group the frame
 * dimensions and image pointer first; this preserves the native operand
 * order as well as the hash-ordered storage (see docs/PATTERNS.md). */
static unsigned int nDIBFrameHeight; /* 0x00476650 */
static unsigned int nDIBFrameWidth; /* 0x0047664c */
static unsigned char *pDIBImage; /* 0x00476648 */
static unsigned char szFrameRateLabel[32]; /* 0x00476620 */
static unsigned int DAT_00476640; /* 0x00476640 */
static unsigned int DAT_00476644; /* 0x00476644 */
static unsigned char abDIBPixelBackup[0xfa00]; /* 0x00476658 */
static int DAT_00486058; /* 0x00486058 */
static int nUnusedDIBData; /* 0x0048605c */
static int nDIBSlamTotal; /* 0x00486060 */
static int nUnusedDIBValue; /* 0x00486064 */
static unsigned char abUnusedDIBArea[12]; /* 0x00486068 */
/* These two names share a hash key; CL places the later declaration first. */
static char szDIBFailureString[0x98]; /* 0x00486078 */
#ifndef SDL_PORT
static
#endif
HWND hDIBParentWindow; /* 0x00486074 */
static unsigned char abDIBPaletteQuads[1024]; /* 0x00486110 */
static float fSampledFps; /* 0x00486510 */
static int nUnusedDIBLevel; /* 0x00486514 */
static int bPendingDIBSlam; /* 0x00486518 */
static unsigned char abUnusedDIBRect[8]; /* 0x00486520 */
static char szSpaceFlightFpsCapMessage[0x80]; /* 0x00486528 */

/* Linker commons defined here; LINK orders them by identifier hash. */
signed char cCurrentNavPtIdx; /* 0x0059c86c */
short asObjectFlipArray[SPACE_OBJECT_COUNT]; /* 0x0059c870 */
short nEyeRollTargetValue; /* 0x0059c8f0 */
FixedVector vPreviousStarFieldMotion; /* 0x0059c900 */
short nCurrentHazardFieldTotal; /* 0x0059c90c */
signed char acPreviousAttackers[16]; /* 0x0059c910 */
signed char acViableTargetList[16]; /* 0x0059c920 */
short asActionCounts[10]; /* 0x0059c930 */
short nEyeYawGoalValue; /* 0x0059c944 */
short asObjScreenScalesArray[SPACE_OBJECT_COUNT]; /* 0x0059c950 */
short asObjAfterburnerVelocities[SPACE_OBJECT_COUNT]; /* 0x0059c9d0 */
CampaignState stCampaignState; /* 0x0059ca50 */
signed char aShipWeaponsTable[10][0x47]; /* 0x0059cab0 */

/* DIBreInstall calls DIBcascade before its definition. */
int DIBcascade(int mode, int *reportedResult);

/* Function start: 0x431F00 */
void ThrottleFrameAndDrawFps(HDC dc)
{
    DWORD now;

    if (bShowFrameRate != 0) {
        sprintf((char *)szFrameRateLabel, "%f",
                fSampledFps);
        TextOutA(dc, 0, 0, (char *)szFrameRateLabel,
                 strlen((char *)szFrameRateLabel));
    }

    if (stMouseCursorState.viewport->pixels ==
            pDIBImage) {
        while (timeGetTime() < (DWORD)nFrameDeadlineMs) {
            Sleep(0);
            RefreshMouseCursorDisplay();
        }
    } else {
        while (timeGetTime() < (DWORD)nFrameDeadlineMs)
            Sleep(0);
    }

    if (bShowFrameRate != 0) {
        if (nPreviousFrameTimeMs != 0) {
            now = timeGetTime();
            fSampledFps =
                1000.0f / ((float)now - nPreviousFrameTimeMs);
        }
        nPreviousFrameTimeMs = timeGetTime();
        sprintf((char *)szFrameRateLabel, "%f",
                fSampledFps);
        TextOutA(dc, 0, 0, (char *)szFrameRateLabel,
                 strlen((char *)szFrameRateLabel));
    }

    nFrameDeadlineMs = timeGetTime() + nFrameIntervalMs;
}

/* Function start: 0x432050 */
void ReportSpaceFlightMaxFps(float adjustment)
{
    double frameRate;

    frameRate = fSpaceFlightFrameRate;
    frameRate += adjustment;
    fSpaceFlightFrameRate = frameRate;
    if (fSpaceFlightFrameRate < 8.0)
        fSpaceFlightFrameRate = 8.0f;
    else if (fSpaceFlightFrameRate > 32.0)
        fSpaceFlightFrameRate = 32.0f;
    sprintf(szSpaceFlightFpsCapMessage,
            "Space Flight Max FPS : %.1f",
            (double)fSpaceFlightFrameRate);
    SetHudMessageText(szSpaceFlightFpsCapMessage, 0x50, 0x14);
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
#ifdef SDL_PORT
    const char *text = SDL_GetError();

    sprintf(szDIBFailureString, "ERROR: %s - (%s)", tag, text);
    SdlShutdownVideo();
    OutputDebugStringA(szDIBFailureString);
    if (hDIBParentWindow != 0)
        SDL_SetWindowSize((SDL_Window *)hDIBParentWindow, 320, 200);
    MessageBoxA(0, szDIBFailureString, "SDL Video Error",
                MB_ICONERROR);
    exit(1);
#else
    FILE *errorFile;
    char *text = DirectDrawResultToText(hr);

    sprintf(szDIBFailureString, "ERROR: %s - (%s)", tag, text);
    COM_RELEASE(pSecondarySurface);
    COM_RELEASE(pPrimarySurface);
    IDirectDraw2_RestoreDisplayMode(pDirectDraw2);
    IDirectDraw2_Release(pDirectDraw2);
    OutputDebugStringA(szDIBFailureString);
    SetWindowPos(hDIBParentWindow, HWND_BOTTOM, 0, 0, 320, 200,
                 SWP_SHOWWINDOW);
    errorFile = fopen("direct.err", "wt+");
    fprintf(errorFile, szDIBFailureString);
    fclose(errorFile);
    MessageBoxA(0, szDIBFailureString, "DirectDraw Error",
                MB_ICONERROR);
    exit(1);
#endif
}

/* Function start: 0x432230 */
void DIBpositionWindow(void)
{
#ifdef SDL_PORT
    SdlShutdownVideo();
    if (hDIBParentWindow != 0)
        SDL_SetWindowSize((SDL_Window *)hDIBParentWindow, 320, 200);
#else
    COM_RELEASE(pSecondarySurface);
    COM_RELEASE(pPrimarySurface);
    IDirectDraw2_RestoreDisplayMode(pDirectDraw2);
    IDirectDraw2_Release(pDirectDraw2);
    SetWindowPos(hDIBParentWindow, HWND_BOTTOM, 0, 0, 320, 200,
                 SWP_SHOWWINDOW);
#endif
}

/* Function start: 0x4322B0 */
void DIBreInstall(void)
{
#ifndef SDL_PORT
    int err;

    if (bDirectDrawModeCascadeEnabled != 0) {
        IDirectDraw2_SetCooperativeLevel(
            pDirectDraw2, hDIBParentWindow, 0x13);
        if (DIBcascade(-2, &err) == 0)
            DIBerror("DIBreInstall   DIBcascade Failure", err);
    } else {
        nDIBCascadeLevel = -1;
    }
#else
    if (!SdlInitializeVideo((SDL_Window *)hDIBParentWindow))
        DIBerror("DIBreInstall", -1);
#endif
    DIBslam();
    DIBslamReal();
}

/* Function start: 0x432310 */
void DIBinstall(HWND window)
{
#ifndef SDL_PORT
    LPDIRECTDRAW directDraw;
    HRESULT result;
    int cascadeResult;
#endif

    hDIBParentWindow = window;
#ifdef SDL_PORT
    if (!SdlInitializeVideo((SDL_Window *)window))
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
            pDirectDraw2, hDIBParentWindow, 0x13);
    else
        result = IDirectDraw2_SetCooperativeLevel(
            pDirectDraw2, hDIBParentWindow, 0x17);
    if (result != DD_OK)
        DIBerror("DIBmakeInstall   SetCooperativeLevel", result);

    if (bDirectDrawModeCascadeEnabled != 0) {
        cascadeResult = DIBcascade(-1, 0);
        if (cascadeResult == 0)
            DIBerror("DIBinstall   DIBcascade Failure", result);
    } else {
        nDIBCascadeLevel = -1;
    }
#endif

    hDIBParentWindow = window;
    memset(abDIBPaletteQuads, 0,
           sizeof(abDIBPaletteQuads));
    memset(abDIBPixelBackup, 0,
           sizeof(abDIBPixelBackup));
    DIBmakeDIB();
    bPendingDIBSlam = 1;
    nDIBSlamTotal = 0;
}

/* Function start: 0x432410 */
int DIBcascade(int mode, int *reportedResult)
{
#ifdef SDL_PORT
    nDIBCascadeLevel = 0;
    if (reportedResult != 0)
        *reportedResult = 0;
    return mode < 3;
#else
    DDSURFACEDESC surface;
    HRESULT result;

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
            OutputDebugStringA("320x200 achieved...testing\n");
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
            OutputDebugStringA("640x400 achieved...testing\n");
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
            OutputDebugStringA("640x480 achieved...testing\n");
            break;
        default:
            return 0;
        }

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
        if (result != DD_OK) {
            OutputDebugStringA(" failed\n");
            mode = 0;
            IDirectDrawSurface_Release(pPrimarySurface);
            IDirectDraw2_RestoreDisplayMode(pDirectDraw2);
            reportedResult = 0;
            continue;
        }

        IDirectDrawSurface_Unlock(
            pPrimarySurface, surface.lpSurface);
        IDirectDrawSurface_Release(
            pPrimarySurface);
        pPrimarySurface = 0;
        OutputDebugStringA(" successful\n");
        return 1;
    }
#endif
}

/* Function start: 0x432680 */
/* Full teardown: destroy the DIB, release its surfaces, restore the display
 * mode and release DirectDraw itself. */
void DIBunInstall(void)
{
    DIBdestroyDIB();
#ifdef SDL_PORT
    SdlShutdownVideo();
    hDIBParentWindow = 0;
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
#ifndef SDL_PORT
    DDSURFACEDESC surface;
    unsigned char entries[0x400];
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
#ifndef SDL_PORT
    offset = 0;
    do {
        entries[offset] = abDIBPaletteQuads[offset + 2];
        entries[offset + 1] = abDIBPaletteQuads[offset + 1];
        entries[offset + 2] = abDIBPaletteQuads[offset];
        entries[offset + 3] = 0;
        offset += 4;
    } while (offset < 0x400);

    result = IDirectDraw2_CreatePalette(
        pDirectDraw2, DDPCAPS_8BIT, (LPPALETTEENTRY)entries,
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
    nDIBFrameWidth = 320;
    nDIBFrameHeight = 200;
    pDIBImage = malloc(64000);
#ifdef SDL_PORT
    if (pDIBImage == 0)
        DIBerror("DIBmakeDIB", -1);
#endif
    stScreenView.allocation = GetDIBPixelBuffer();
    stScreenView.pixels = stScreenView.allocation;
    memcpy(pDIBImage, abDIBPixelBackup,
           nDIBFrameWidth * nDIBFrameHeight);
}

/* Function start: 0x4328A0 */
void DIBdestroyDIB(void)
{
#ifndef SDL_PORT
    int result;
#endif

#ifdef SDL_PORT
    if (pDIBImage != 0) {
#endif
    memcpy(abDIBPixelBackup, pDIBImage,
           nDIBFrameWidth * nDIBFrameHeight);
#ifdef SDL_PORT
    }
#endif
#ifndef SDL_PORT
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
    if (pDIBImage != 0)
        free(pDIBImage);
    DAT_00476640 = 0;
    DAT_00476644 = 0;
    pDIBImage = 0;
#ifdef SDL_PORT
    stScreenView.pixels = 0;
    stScreenView.allocation = 0;
#endif
}

/* Function start: 0x432960 */
void DIBslam(void)
{
    bPendingDIBSlam = 1;
}

/* Function start: 0x432970 */
void DIBslamReal(void)
{
#ifndef SDL_PORT
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
    if (bPendingDIBSlam != 0) {
#ifndef SDL_PORT
        memset(&surface, 0, sizeof(surface));
        surface.dwSize = sizeof(surface);
#endif

#ifdef SDL_PORT
        if (stMouseCursorState.viewport != 0 &&
            stMouseCursorState.viewport->pixels ==
                pDIBImage) {
#else
        if (stMouseCursorState.viewport->pixels ==
                pDIBImage) {
#endif
            CaptureMouseCursorBackground();
            DrawMouseCursor();
        }

#ifndef SDL_PORT
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
        source = pDIBImage;
        if (nDIBCascadeLevel <= 0) {
            IDirectDraw2_WaitForVerticalBlank(
                pDirectDraw2, DDWAITVB_BLOCKBEGIN, 0);
        }
        row = 0;
        if ((int)nDIBFrameHeight > 0) {
            do {
                memcpy(destination, source, nDIBFrameWidth);
                destination += surface.lPitch;
                source += nDIBFrameWidth;
                row++;
            } while ((int)nDIBFrameHeight > row);
        }

        if (nDIBCascadeLevel > 0) {
            IDirectDrawSurface_Unlock(
                pSecondarySurface, surface.lpSurface);
        } else {
            IDirectDrawSurface_Unlock(
                pPrimarySurface, surface.lpSurface);
        }
#else
        if (!SdlPresentIndexedFrame(pDIBImage,
                                       abDIBPaletteQuads))
            DIBerror("DIBslamReal", -1);
#endif

#ifdef SDL_PORT
        if (stMouseCursorState.viewport != 0 &&
            stMouseCursorState.viewport->pixels ==
                pDIBImage)
#else
        if (stMouseCursorState.viewport->pixels ==
                pDIBImage)
#endif
            RestoreMouseCursorBackground();

#ifndef SDL_PORT
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
        bPendingDIBSlam = 0;
    }

    nDIBSlamTotal++;
    ServiceSoundSystem();
#ifdef SDL_PORT
    ThrottleFrameAndDrawFps(0);
#else
    dc = GetDC(hDIBParentWindow);
    ThrottleFrameAndDrawFps(dc);
    ReleaseDC(hDIBParentWindow, dc);
#endif
}

/* Function start: 0x432C60 */
void DIBupdate(int left, int top, int right, int bottom)
{
#ifndef SDL_PORT
    unsigned char *destination;
    unsigned char *source;
    DDSURFACEDESC surface;
    int width;
    int height;
    HRESULT result;
    int destinationOffset;
    int sourceOffset;
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

#ifdef SDL_PORT
    if (right < left || bottom < top)
        return;
    if (!SdlPresentIndexedFrame(pDIBImage,
                                   abDIBPaletteQuads))
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

    destinationOffset = surface.lPitch * top;
    destinationOffset += left;
    destination = (unsigned char *)surface.lpSurface + destinationOffset;
    sourceOffset = nDIBFrameWidth * top;
    sourceOffset += left;
    source = pDIBImage + sourceOffset;
    IDirectDraw2_WaitForVerticalBlank(
        pDirectDraw2, DDWAITVB_BLOCKBEGIN, 0);
    while (height > 0) {
        memcpy(destination, source, width);
        destination += surface.lPitch;
        source += nDIBFrameWidth;
        height--;
    }
    IDirectDrawSurface_Unlock(
        pPrimarySurface, surface.lpSurface);
#endif
}

/* Function start: 0x432DE0 */
unsigned char *GetDIBPixelBuffer(void)
{
    return pDIBImage;
}

/* Function start: 0x432DF0 */
/* Empty, and nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it sits between the DIB accessors. */
void DIBpixelBufferHook(void)
{
}

/* Function start: 0x432E00 */
HWND GetDIBWindowHandle(void)
{
    return hDIBParentWindow;
}

/* Function start: 0x432E10 */
/* Nothing in the shipped executable calls this. */
unsigned int GetDIBWidth(void)
{
    return nDIBFrameWidth;
}

/* Function start: 0x432E20 */
/* Nothing in the shipped executable calls this. */
unsigned int GetDIBHeight(void)
{
    return nDIBFrameHeight;
}

/* Function start: 0x432E30 */
void CachePaletteEntryFromWords(short index, short *rgb)
{
    awPaletteRgbWordsList[index * 3] =
        abDIBPaletteQuads[index * 4 + 2] = (unsigned char)rgb[0];
    awPaletteRgbWordsList[index * 3 + 1] =
        abDIBPaletteQuads[index * 4 + 1] = (unsigned char)rgb[1];
    awPaletteRgbWordsList[index * 3 + 2] =
        abDIBPaletteQuads[index * 4] = (unsigned char)rgb[2];
    abDIBPaletteQuads[index * 4 + 3] = 1;
}

/* Function start: 0x432EA0 */
void DIBramPalette(void)
{
#ifdef SDL_PORT
    if (pDIBImage != 0 &&
        !SdlPresentIndexedFrame(pDIBImage,
                                   abDIBPaletteQuads))
        DIBerror("DIBramPalette", -1);
#else
    unsigned char entries[0x400];
    int offset = 0;
    HRESULT result;

    do {
        entries[offset] = abDIBPaletteQuads[offset + 2];
        entries[offset + 1] = abDIBPaletteQuads[offset + 1];
        entries[offset + 2] = abDIBPaletteQuads[offset];
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
    int cacheOffset;
    short *components;
#ifndef SDL_PORT
    PALETTEENTRY entry;
    HRESULT result;
#endif

    cacheOffset = index * 4;
    components = *(short *volatile *)&rgb;
    if (abDIBPaletteQuads[cacheOffset + 2] != components[0] ||
        abDIBPaletteQuads[cacheOffset + 1] != components[1] ||
        abDIBPaletteQuads[cacheOffset] != components[2]) {
        awPaletteRgbWordsList[index * 3] =
            abDIBPaletteQuads[cacheOffset + 2] = (unsigned char)components[0];
        awPaletteRgbWordsList[index * 3 + 1] =
            abDIBPaletteQuads[cacheOffset + 1] = (unsigned char)components[1];
        awPaletteRgbWordsList[index * 3 + 2] =
            abDIBPaletteQuads[cacheOffset] = (unsigned char)components[2];
        abDIBPaletteQuads[cacheOffset + 3] = 1;

        /* SDL_PORT consumes this cache on the next normal frame submission.
           A DirectDraw palette entry update did not blit or wait for vertical
           blank, so flight fades must not submit additional SDL frames. */
#ifndef SDL_PORT
        entry.peRed = (unsigned char)awPaletteRgbWordsList[index * 3];
        entry.peGreen = (unsigned char)awPaletteRgbWordsList[index * 3 + 1];
        entry.peBlue = (unsigned char)awPaletteRgbWordsList[index * 3 + 2];
        entry.peFlags = 0;
        result = IDirectDrawPalette_SetEntries(
            pDirectDrawPalette, 0, index, 1, &entry);
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
void GetPaletteEntryAsWords(short i, short *rgb)
{
    int k = i * 4;

    rgb[0] = abDIBPaletteQuads[k + 2];
    rgb[1] = abDIBPaletteQuads[k + 1];
    rgb[2] = abDIBPaletteQuads[k];
}

/* Function start: 0x433060 */
void DIBwholePaletteFromTriplets(unsigned char *palette)
{
#ifndef SDL_PORT
    unsigned char entries[0x400];
#endif
    int offset = 0;
#ifndef SDL_PORT
    int error;

    IDirectDraw2_WaitForVerticalBlank(
        pDirectDraw2, DDWAITVB_BLOCKBEGIN, 0);
#else
    SdlWaitForVerticalBlank();
#endif
    do {
        unsigned char value = palette[0];

        palette = palette + 3;
        abDIBPaletteQuads[offset + 2] = value;
#ifndef SDL_PORT
        entries[offset] = value;
#endif
        value = palette[-2];
        abDIBPaletteQuads[offset + 1] = value;
#ifndef SDL_PORT
        entries[offset + 1] = value;
#endif
        value = palette[-1];
        abDIBPaletteQuads[offset] = value;
#ifndef SDL_PORT
        entries[offset + 2] = value;
        entries[offset + 3] = 0;
#endif
        abDIBPaletteQuads[offset + 3] = 4;
        offset = offset + 4;
    } while (offset < 0x400);

#ifdef SDL_PORT
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
void DIBwholePaletteFromWords(short *palette)
{
#ifndef SDL_PORT
    unsigned char entries[0x400];
#endif
    int offset = 0;
#ifndef SDL_PORT
    int error;

    IDirectDraw2_WaitForVerticalBlank(
        pDirectDraw2, DDWAITVB_BLOCKBEGIN, 0);
#else
    SdlWaitForVerticalBlank();
#endif
    do {
        unsigned char value = (unsigned char)palette[0];

        palette = palette + 3;
        abDIBPaletteQuads[offset + 2] = value;
#ifndef SDL_PORT
        entries[offset] = value;
#endif
        value = (unsigned char)palette[-2];
        abDIBPaletteQuads[offset + 1] = value;
#ifndef SDL_PORT
        entries[offset + 1] = value;
#endif
        value = (unsigned char)palette[-1];
        abDIBPaletteQuads[offset] = value;
#ifndef SDL_PORT
        entries[offset + 2] = value;
        entries[offset + 3] = 0;
#endif
        abDIBPaletteQuads[offset + 3] = 4;
        offset = offset + 4;
    } while (offset < 0x400);

#ifdef SDL_PORT
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
#ifdef SDL_PORT
    SdlWaitForVerticalBlank();
#else
    IDirectDraw2_WaitForVerticalBlank(
        pDirectDraw2, DDWAITVB_BLOCKBEGIN, 0);
#endif
}

#ifndef SDL_PORT

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
