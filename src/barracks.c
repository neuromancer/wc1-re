/*
 *  Barracks and campaign save/load room.
 *
 *  Address range 0x41ada0-0x41c75f (provisional -- see docs/ORDER.md).
 *  Boundary evidence: contiguous SAVEGAME.WLD helpers ending at BarracksScreen;
 *  the Win32 debug-overlay compilation unit begins at 0x41c760.
 */
#include "wc1.h"

/* Function start: 0x41C170 */
short BarracksScreen(void)
{
    InputEventState event;
    unsigned char *background;
    unsigned char savedInputMode;
    short eventType;
    short index;
    short region;
    short result;
    short x;
    short y;

    result = 0;
    index = 0;
    do {
        g_apszBarracksMenuLabels_004693f0[index * 2] =
            (char *)g_apszSaveCampaignMenuLabels_004693e8[0];
        g_apszBarracksMenuLabels_004693f0[index * 2 + 1] =
            (char *)g_apszSaveCampaignMenuLabels_004693e8[1];
        index++;
    } while (index < 8);
    g_apszBarracksMenuLabels_004693f0[16] =
        (char *)g_szMissionHangarMenuLabel_00469498;
    g_apszBarracksMenuLabels_004693f0[17] =
        (char *)g_szReturnToBarMenuLabel_004694a8;
    g_apszBarracksMenuLabels_004693f0[18] =
        (char *)g_szQuitGameMenuLabel_004694bc;
    g_apszBarracksMenuLabels_004693f0[19] =
        (char *)g_szViewMedalsMenuLabel_004694d0;

    PreloadMusicTrackHook(35);
    StartMusicTrack(35, 2, 1);
    InitializeRoomViewports();
    background = (unsigned char *)FetchDiskPacketRetrying(5, 12, 0);
    InitializeRoomMenu(g_aBarracksMenuRegions_00463008,
                       g_apszBarracksMenuLabels_004693f0,
                       &g_stRoomScreenViewport_005988a0,
                       g_szDefaultTextBuffer_005a7590, 2);

    DrawSpriteDefault(&DAT_005a76b0, 0, 0, background, 0);
    index = 0;
    do {
        x = (short)((index & 1) * 31 + 143);
        y = (short)((index / 2) * 5 + 167);
        DrawSpriteDefault(&DAT_005a76b0, x, y, background, 10);
        index++;
    } while (index < 8);
    CopyViewportContents(&DAT_005a76b0,
                         &g_stRoomScreenViewport_005988a0);

    savedInputMode = g_bInputMode_0059a848;
    g_bInputMode_0059a848 = 1;
    DAT_0059ab23 = &g_stRoomDisplayViewport_00598a60;
    EnterAllocationScope();
    WarpMouseTo(160, 100);
    SetFrameTimerPeriodDirect(0);
    FlushInputEvents();
    *(short *)&g_aInputDeviceSamples_005a81f0[2].x = 2;

    while (result == 0) {
        PumpWindowMessages();
        if (IsFrameTickElapsed() != 0) {
            RefreshRoomMenuLabel();
            SetFrameTimerPeriodDirect(2);
        }
        eventType = PollInputEvent(&event, 0xff);
        if (eventType == 13) {
            UpdateRoomMenuCursor();
        } else if (eventType == 2 || eventType == 10) {
            region = FindMenuRegionAtPoint(
                event.x, event.y, g_aBarracksMenuRegions_00463008);
            if (region == 16 && DAT_004688f0 != 0)
                result = 7;
            else if (region == 17 && DAT_004688f0 != 0)
                result = 8;
        } else if (eventType == 3 || eventType == 5) {
            ClearInputKeyStatePreservingModifiers();
            if ((short)event.value == 0x1c ||
                (short)event.value == 0x39) {
                region = FindMenuRegionAtPoint(
                    g_nMouseX_0059ab10, g_nMouseY_0059ab12,
                    g_aBarracksMenuRegions_00463008);
                if (region == 16 && DAT_004688f0 != 0)
                    result = 7;
                else if (region == 17 && DAT_004688f0 != 0)
                    result = 8;
            } else {
                MoveMenuPointerFromKeyboard(&event);
            }
        }
        ShowMemoryStatusDebug();
        DIBslam();
        DIBslamReal();
    }

    LeaveAllocationScope();
    *(short *)&g_aInputDeviceSamples_005a81f0[2].x =
        g_nSavedRoomControllerX_005988b4;
    EventManagerHook(0);
    SetEventManagerPump(0);
    FreePacketAndClear((int *)&background, 0);
    ReleaseTextFont(0);
    free_viewport(&DAT_005a76b0);
    g_bInputMode_0059a848 = savedInputMode;
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook(35);
    return result;
}

/* Function start: 0x41C510 */
unsigned short __stdcall StepPaletteTransition(short *current,
                                                const short *target,
                                                short componentCount)
{
    unsigned int byteCount;
    short difference;
    short index;
    short previousCountdown;

    if (g_nPaletteTransitionInitialise_00469640 != 0) {
        byteCount = (unsigned int)(componentCount * 2);
        g_pPaletteTransitionAccumulator_005a7d94 =
            (short *)AllocateTaggedMemory(byteCount, 0);
        g_pPaletteTransitionDelta_005a7d8c =
            (short *)AllocateTaggedMemory(byteCount, 0);
        g_pPaletteTransitionDirection_005a7d88 =
            (short *)AllocateTaggedMemory(byteCount, 0);
        if (g_pPaletteTransitionAccumulator_005a7d94 == 0 ||
            g_pPaletteTransitionDelta_005a7d8c == 0 ||
            g_pPaletteTransitionDirection_005a7d88 == 0) {
            if (g_pPaletteTransitionAccumulator_005a7d94 != 0)
                ReleasePacketHandle(
                    (int)g_pPaletteTransitionAccumulator_005a7d94);
            if (g_pPaletteTransitionDelta_005a7d8c != 0)
                ReleasePacketHandle((int)g_pPaletteTransitionDelta_005a7d8c);
            if (g_pPaletteTransitionDirection_005a7d88 != 0)
                ReleasePacketHandle(
                    (int)g_pPaletteTransitionDirection_005a7d88);
            return 0;
        }

        g_nPaletteTransitionMaxDelta_005a7d90 = 0;
        index = 0;
        while (index < componentCount) {
            difference = (short)(current[index] - target[index]);
            if (difference < 0) {
                difference = (short)-difference;
                g_pPaletteTransitionDirection_005a7d88[index] = 4;
            } else {
                g_pPaletteTransitionDirection_005a7d88[index] = -4;
            }
            g_pPaletteTransitionDelta_005a7d8c[index] = difference;
            if (g_nPaletteTransitionMaxDelta_005a7d90 < difference)
                g_nPaletteTransitionMaxDelta_005a7d90 = difference;
            index++;
        }

        index = 0;
        while (index < componentCount) {
            g_pPaletteTransitionAccumulator_005a7d94[index] =
                (short)(g_nPaletteTransitionMaxDelta_005a7d90 / 4);
            index++;
        }
        g_nPaletteTransitionInitialise_00469640 = 0;
        g_nPaletteTransitionCountdown_005a7d98 =
            (short)(g_nPaletteTransitionMaxDelta_005a7d90 / 4);
    }

    previousCountdown = g_nPaletteTransitionCountdown_005a7d98;
    g_nPaletteTransitionCountdown_005a7d98--;
    if (previousCountdown == 0) {
        ReleasePacketHandle((int)g_pPaletteTransitionAccumulator_005a7d94);
        ReleasePacketHandle((int)g_pPaletteTransitionDelta_005a7d8c);
        ReleasePacketHandle((int)g_pPaletteTransitionDirection_005a7d88);
        g_nPaletteTransitionInitialise_00469640 = 1;
        return 0;
    }

    index = 0;
    while (index < componentCount) {
        g_pPaletteTransitionAccumulator_005a7d94[index] =
            (short)(g_pPaletteTransitionAccumulator_005a7d94[index] +
                    g_pPaletteTransitionDelta_005a7d8c[index]);
        if (g_pPaletteTransitionAccumulator_005a7d94[index] >
            g_nPaletteTransitionMaxDelta_005a7d90) {
            g_pPaletteTransitionAccumulator_005a7d94[index] =
                (short)(g_pPaletteTransitionAccumulator_005a7d94[index] -
                        g_nPaletteTransitionMaxDelta_005a7d90);
            current[index] =
                (short)(current[index] +
                        g_pPaletteTransitionDirection_005a7d88[index]);
        }
        index++;
    }
    return 1;
}

/* Function start: 0x41C740 */
char *__stdcall DosStrcat(char *destination, const char *source)
{
    char *end;

    end = DosStrchr(destination, 0);
    DosStrcpy(end, source);
    return destination;
}
