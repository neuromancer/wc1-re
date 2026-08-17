/*
 *  WINGLEADER main module.
 *
 *  Address range 0x4274e0-0x427fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: main() at 0x004274E0, confirmed against the leaked DOS source screenshot.
 */
#include "wc1.h"

/* Function start: 0x45C558 */
#ifdef WC1_SDL
int Wc1GameMain(short argc, char **argv)
#else
int main(short argc, char **argv)
#endif
{
    char *argument;
    int animationDemo;
    int selection;
    int launchMission;
    short argumentCount;
    short argumentIndex;
    short mission;
    short series;
    short gameFlowResult;

    mission = 0;
    launchMission = 0;
    series = 1;
    animationDemo = 0;

    GetShutdownErrorCode(g_abDiskPromptDriveState_005a7d20);
    ExitCleanupHook();
    StartupHook(GetJoystickButtonEdge);
    DAT_0059ab4c = GetStartupErrorCode(0x21);
    ShutdownHook(0x21, MouseIdleHook);

    argumentCount = LoadWingCmdrCfgFile(argc, argv);
    _chdir("gamedat");
    LoadInstallDat();
    _chdir("..");
#ifdef WC1_SDL
    if (Wc1SdlUsingDosData())
        DAT_0059ab34 = 1;
#endif
    if (GetCurrentDiskDriveHook() > 'B')
        DAT_0059ab34 = 1;

    SystemDebugPrintf("Loading WING COMMANDER.  Please wait...\n");
    DAT_005a7d9c = 1;
    g_nMusicDriverMode_0049be8c = 4;
    DAT_0059a856 = 1;
    ResetCampaignData();

    for (argumentIndex = 0;
         argumentIndex < argumentCount;
         argumentIndex++) {
        argument = g_pStartupArguments_005a7b10[argumentIndex];
        if (strcmp("Origin", argument) == 0)
            g_nOriginDevUnlock_0049d774 = 1;

        switch (argument[0]) {
        case '?':
            SystemDebugPrintf("Version %s.\n", g_pszGameVersion_0049b528);
        case '-':
            if (argument[1] == 'm')
                g_nShowMemoryStatus_0049d784 = 1;
            if (g_nOriginDevUnlock_0049d774 != 0) {
                switch (argument[1]) {
                case 'b':
                    DAT_0046a000 = 0;
                    break;
                case 'f':
                    DAT_00465070 = 1;
                    break;
                case 'k':
                    DAT_00469ffc = 0;
                    break;
                case 'q':
                    DAT_00465074 = 0;
                    break;
                }
            }
            break;
        case 'A':
        case 'a':
            if (argument[1] == 's' || argument[1] == 'S') {
                g_nMissionEntryNavOverride_0049d790 = (short)atoi(argument + 2);
            } else {
                g_nMusicDriverMode_0049be8c = 2;
                g_nArcadeStartupParameter_005a7b8a =
                    (short)atoi(argument + 1);
            }
            break;
        case 'E':
        case 'e':
            g_bSlowSceneAnimation_00469998 = 1;
            break;
        case 'P':
        case 'p':
            g_nMusicDriverMode_0049be8c = 3;
            break;
        case 'R':
        case 'r':
            g_nMusicDriverMode_0049be8c = 1;
            break;
        case 'T':
        case 't':
            g_bSlowSceneAnimation_00469998 = 3;
            break;
        case 'V':
        case 'v':
            g_bSlowSceneAnimation_00469998 = 0;
            break;
        case 'Z':
        case 'z':
            DAT_005a7d9c = 1;
            break;
        case 'l':
            if (g_nOriginDevUnlock_0049d774 != 0)
                launchMission = 1;
            break;
        case 'm':
            if (g_nOriginDevUnlock_0049d774 != 0)
                mission = (short)atoi(argument + 1);
            break;
        case 's':
            if (g_nOriginDevUnlock_0049d774 != 0) {
                DAT_004688f0 = 1;
                series = (short)atoi(argument + 1);
            }
            break;
        case 'w':
            if (g_nOriginDevUnlock_0049d774 != 0) {
                animationDemo = 1;
                mission = (short)atoi(argument + 1);
            }
            break;
        }
    }

    SetCinematicFrameTiming(20.0f);
    g_stCampaignState_0059ca50.currentSeries = (signed char)series;
    g_nSpacePaletteFadeMode_004901e8 = 0x13;
    g_stInitialCampaignState_004700b0.currentSeries =
        (signed char)series;
    g_stCampaignState_0059ca50.currentMission = (signed char)mission;
    g_stInitialCampaignState_004700b0.currentMission =
        (signed char)mission;
    LoadOriginFxDrivers();
    g_nSpacePaletteFadeMode_004901e8 = 0x13;

    if (animationDemo == 1) {
        SystemDebugPrintf("Pre animation: %lu.\n",
                          GetLargestFreeMemoryBlockByType(0));
        WaitForKeyAcknowledge(0);
        RunCampaignGameLoop((short)mission);
        SystemDebugPrintf("Post animation: %lu.\n",
                          GetLargestFreeMemoryBlockByType(0));
        WaitForKeyAcknowledge(0);
    }

    LoadVolumeSettingsFromRegistry();
    SetSoundEffectsVolume(
        g_anVolumeLevels_00469fc8[g_nSfxVolumeSetting_00469fbc / 2]);
    SetMusicStreamVolume((unsigned short)g_anVolumeLevels_00469fc8[
        g_nMusicVolumeSetting_00469fc0 / 2]);

    if (launchMission != 0) {
#ifdef WC1_SDL
        if (init_mission(series, mission) != 0) {
            fprintf(stderr,
                    "Mission SERIES=%d MISSION=%d is absent or invalid.\n",
                    (int)series, (int)mission);
            return 1;
        }
#else
        init_mission(series, mission);
#endif
#ifdef WC1_SDL
        /* Use the normal campaign setup unless cockpitless mode was requested. */
        if (DAT_0046507c != 0)
            LaunchPlayerShip();
#endif
        RunSpaceFlight(g_nMissionEntryNavOverride_0049d790);
        exit_squadron("Bye!");
        return 0;
    }

#ifdef WC1_SDL
    Wc1SdlPlayDosStartupIntro();
#endif
    g_bSceneEscapeRequested_0049d4b0 = 0;
    for (;;) {
        FrameStartHook(0);
        DAT_004688e0 = 1;
        selection = Title_Sequence();
        switch (selection) {
        case 0:
            StartNewCampaign(0);
            break;
        case 2:
            StartNewCampaign(1);
            break;
        case 3:
            StartNewCampaign(2);
            break;
        default:
            break;
        }
        gameFlowResult = GameFlow();
        while (gameFlowResult != 0)
            gameFlowResult = GameFlow();
    }
}

/* Function start: 0x465CBC */
void free_view_buffer(void)
{
    if (message_showing())
        EndCommMenu();
    if (g_stViewBuffer_005d2b00.pixels != 0)
        free_viewport(&g_stViewBuffer_005d2b00);
#if 0
    return 0;
#endif
}

/* Function start: 0x465CF6 */
void initialize_view_buffer(void)
{
#if 0
    if (g_bSpaceViewBufferEnabled_0049d7a4 != 0 && g_stViewBuffer_005d2b00.pixels == 0) {
        if (AllocateViewport(&g_stViewBuffer_005d2b00, (short)g_cPrimaryViewBufferColour_0049cb88,
                             0x20) == 0)
            ReportOutOfMemoryAndExit(g_szSpaceBuffer_0046a1d0);
    }
    return 0;
#else
    short allocated;

    if (g_bSpaceViewBufferEnabled_0049d7a4 != 0 &&
        g_stViewBuffer_005d2b00.pixels == 0) {
        allocated = (short)AllocateViewport(
            &g_stViewBuffer_005d2b00,
            (short)g_cPrimaryViewBufferColour_0049cb88, 0x20);
        if (allocated == 0)
            ReportOutOfMemoryAndExit(g_szSpaceBuffer_0046a1d0);
    }
#endif
}

/* Function start: 0x465D55 */
unsigned int dump_buffer_to_screen(void)
{
    short bottom;

#ifdef WC1_SDL
    Wc1SdlCompleteSpaceFrame();
#endif
    if (g_nCockpitDisplayMode_0049d71c > 0) {
        CopyViewportContents(&g_stViewBuffer_005d2b00, &g_stScreenViewport_005d21a0);
        ShowMemoryStatusDebug();
        return 0;
    }
    switch ((int)g_cScreenViewportMode_005c82a6) {
    case 4:
        bottom = g_stScreenViewport_005d21a0.bottom;
        g_stScreenViewport_005d21a0.top = 24;
        g_stScreenViewport_005d21a0.bottom = 152;
        CopyViewportContents(&g_stViewBuffer_005d2b00, &g_stScreenViewport_005d21a0);
        g_stScreenViewport_005d21a0.bottom = bottom;
        g_stScreenViewport_005d21a0.top = 0;
        break;
    case 5:
        CopyViewportContents(&g_stViewBuffer_005d2b00, &g_stScreenViewport_005d21a0);
        break;
    default:
        fizzle_fade(&g_stViewBuffer_005d2b00, &g_stScreenViewport_005d21a0,
                    g_pScreenViewportGeometry_0059a9f4);
        break;
    }
    ShowMemoryStatusDebug();
    return 0;
}

/* Function start: 0x465E88 */
unsigned int clear_view_buffer(void)
{
    ClearViewport(&g_stViewBuffer_005d2b00, (unsigned char)g_cPrimaryViewBufferColour_0049cb88);
    return 0;
}

/* Function start: 0x437CBF */
void InitializeConversationViewport(void)
{
    ClearViewport(&g_stModalSourceViewport_005d2c50, g_cSecondaryViewBufferColour_0049cb4c);
    if (g_stSecondaryViewBuffer_005d2c90.pixels != 0)
        free_viewport(&g_stSecondaryViewBuffer_005d2c90);
    g_stScreenViewport_005d21a0.top = 24;
    g_stScreenViewport_005d21a0.bottom = 151;
    g_stSecondaryViewBuffer_005d2c90.left = 0;
    g_stSecondaryViewBuffer_005d2c90.right = 319;
    g_stSecondaryViewBuffer_005d2c90.top = 0;
    g_stSecondaryViewBuffer_005d2c90.bottom = 127;
    if (AllocateViewport(&g_stSecondaryViewBuffer_005d2c90, (short)g_cSecondaryViewBufferColour_0049cb4c, 0) == 0)
        ReportPacketLoadError(0, 0, 0, 0,
                              g_szAllocateBufferTag_0046a1e0);
#if 0
    return 0;
#endif
}

/* Function start: 0x437D68 */
void ResetScreenClipToFullHeight(void)
{
    free_viewport(&g_stSecondaryViewBuffer_005d2c90);
    g_stScreenViewport_005d21a0.top = 0;
    g_stScreenViewport_005d21a0.bottom = 199;
#if 0
    return 0;
#endif
}

/* Function start: 0x437D92 */
void InitializeConversationText(void)
{
    g_stConversationTextViewport_005a7570 =
        g_stModalSourceViewport_005d2c50;
    g_stConversationTextViewport_005a7570.top = 152;
    g_stConversationTextContext_005a7760.viewport =
        &g_stConversationTextViewport_005a7570;
    g_stConversationTextContext_005a7760.text =
        g_szDefaultTextBuffer_005d2b80;
    g_stConversationTextContext_005a7760.alignment = 2;
    InitializeTextContextFromFont(&g_stConversationTextContext_005a7760,
                                  0,
                                  g_cViewportClearColour_004699a0,
                                  g_cSecondaryViewBufferColour_0049cb4c);
    SetTextContext(&g_stConversationTextContext_005a7760);
#if 0
    return 0;
#endif
}

/* Function start: 0x465EA9 */
void RefreshMemoryStatusOverlay(void)
{
    WaitForVerticalBlankThunk();
    CopyViewportContents(&g_stSecondaryViewBuffer_005d2c90, &g_stScreenViewport_005d21a0);
    DIBslam();
    DIBslamReal();
    if (g_nShowMemoryStatus_0049d784 != 0)
        ShowMemoryStatusDebug();
}

/* Function start: 0x465EE9 */
void Update_3Space(void)
{
#if 0
    house_keep();
    house_keep_objects();
    update_objects_in_space();
    set_eye_direction_and_position();
    servicetrack();
    g_nSpaceFrame_00493134++;
    return 0;
#else
    house_keep();
    CheckAllGuardedAllocations();
    house_keep_objects();
    if (g_nArcadeState_0049d75c != 0)
        return;
    update_objects_in_space();
    set_eye_direction_and_position();
    if (g_nCannedSceneMode_0049021c == 0)
        servicetrack();
    g_nSpaceFrame_00493134++;
#endif
}

/* Function start: 0x465F3A */
unsigned int TriggerPlayerHitPaletteFlash(void)
{
    if (g_nCurrentView_00492fa8 <= 3)
        g_asSpacePaletteFade_005d2d60[0] = 0x30;
    return 0;
}

/* Function start: 0x465F5B */
unsigned int FadeFlightPaletteEntry(short *entry)
{
    if (entry[0] != 0) {
        entry[0] = (short)(entry[0] - 4);
        entry[1] = 0;
        entry[2] = 0;
        return 0;
    }
    entry[1] = 0;
    return 0;
}

/* Function start: 0x465FA3 */
void UpdateSpacePaletteFade(void)
{
#if 0
    if (g_asSpacePaletteFade_005d2d60[0] != 0) {
        switch ((int)(short)g_nSpacePaletteFadeMode_004901e8) {
        case 9:
        case 13:
            ClearViewport(&g_stViewBuffer_005d2b00, (short)DAT_004699ac);
            g_bViewportDirty_0049d76c = 1;
            g_asSpacePaletteFade_005d2d60[0] = 0;
            break;
        case 0x13:
            g_asSpacePaletteFade_005d2d60[0] = (short)(g_asSpacePaletteFade_005d2d60[0] - 4);
            SetPaletteEntry((short)g_cPrimaryViewBufferColour_0049cb88,
                            g_asSpacePaletteFade_005d2d60);
            break;
        }
    }
    return 0;
#else
    if (g_asSpacePaletteFade_005d2d60[0] != 0) {
        switch ((int)(short)g_nSpacePaletteFadeMode_004901e8) {
        case 0x13:
            g_asSpacePaletteFade_005d2d60[0] =
                (short)(g_asSpacePaletteFade_005d2d60[0] - 0x10);
            SetPaletteEntry((short)g_cPrimaryViewBufferColour_0049cb88,
                            g_asSpacePaletteFade_005d2d60);
            break;
        case 9:
        case 13:
            ClearViewport(&g_stViewBuffer_005d2b00,
                          g_ucSpaceClearColour_0049cb5c);
            g_bViewportDirty_0049d76c = 1;
            g_asSpacePaletteFade_005d2d60[0] = 0;
            break;
        }
    }
#endif
}

/* Function start: 0x46604F */
unsigned int house_keep(void)
{
    short palette;

    if (g_nCannedSceneMode_0049021c == 0 &&
        g_nTrainSimActive_00469e2c == 0) {
        if ((g_nSpaceFrame_00493134 & 0x1f) == 0)
            ReleaseStaleNavTarget();
        if (g_nHazardFieldCount_0059c90c != 0 &&
            (g_nSpaceFrame_00493134 & 0xf) == 0)
            check_hazards();
    }
    if (g_nCurrentView_00492fa8 == 0) {
        palette = 0;
        do {
            FadeFlightPaletteEntry(g_aPaletteFadeEntries_005a76d0[palette]);
            SetPaletteEntry((short)(palette + 0xb9),
                            g_aPaletteFadeEntries_005a76d0[palette]);
            palette++;
        } while (palette < 6);
        return 0;
    }
    if (DAT_005a7ec0 != 0) {
        ((void (__cdecl *)(int, int))FlushSoundEffectsAndLog)(
            DAT_005a7ec0, 1);
        DAT_005a7ec0 = 0;
        g_abCockpitLightGoal_005d1eb8[3] = 0;
    }
    return 0;
}

/* Function start: WC2_UNMAPPED */
void init_player_input(void)
{
    SetMousePosition(
        (g_stViewBuffer_005d2b00.right - g_stViewBuffer_005d2b00.left) / 2 + 1,
        g_nViewCenterY_0059a854);
    ClearDebugPauseFlags();
    g_bMouseCursorVisible_0046a018 = 0;
    g_bPointerMovedByKeyboard_005a7d54 = 1;
}

/* Function start: 0x4661C2 */
void get_player_input(void)
{
    int device;
    InputDeviceSample *sample;

    if (g_nActiveInputDevice_005a819c != -1 &&
        g_bInputPollingGuard_0046a01c == 0) {
        g_bInputPollingGuard_0046a01c++;
        UpdateInputDeviceTransitions(0);
        device = (int)g_nActiveInputDevice_005a819c;
        sample = &g_aInputDeviceSamples_005a81f0[device];
        if (sample->x == 0 && sample->y == 0 && sample->buttons == 0) {
            if (sample->x != g_stLastPolledFlightInput_0046a020.x ||
                sample->y != g_stLastPolledFlightInput_0046a020.y ||
                sample->buttons !=
                    g_stLastPolledFlightInput_0046a020.buttons) {
                TranslatePolledInputEvent(6, 0);
                device = (int)g_nActiveInputDevice_005a819c;
                sample = &g_aInputDeviceSamples_005a81f0[device];
                g_bInputPollingGuard_0046a01c--;
                g_stLastPolledFlightInput_0046a020 = *sample;
                return;
            }
        } else {
            TranslatePolledInputEvent(6, 0);
            device = (int)g_nActiveInputDevice_005a819c;
            sample = &g_aInputDeviceSamples_005a81f0[device];
            g_stLastPolledFlightInput_0046a020 = *sample;
        }
        g_bInputPollingGuard_0046a01c--;
    }
}

/* Function start: 0x4663A2 */
/* The two volume-adjustment exits deliberately use bare returns.  Retail
 * preserves the value left in EAX by ShowOnScreenMessage on those paths. */
int process_player_input(void)
{
    short keys[4];
    short *key;
    int shift;
    int control;
    short finished;
    short handled;

    finished = 0;
    handled = 1;
    shift = GetShiftKeyState();
    control = GetControlKeyState();
    switch ((signed char)g_bCurrentKey_0046c014) {
    case 0x47:
        keys[0] = 0x48;
        keys[1] = 0x4b;
        keys[2] = -1;
        break;
    case 0x49:
        keys[0] = 0x48;
        keys[1] = 0x4d;
        keys[2] = -1;
        break;
    case 0x4f:
        keys[0] = 0x50;
        keys[1] = 0x4b;
        keys[2] = -1;
        break;
    case 0x51:
        keys[0] = 0x50;
        keys[1] = 0x4d;
        keys[2] = -1;
        break;
    default:
        keys[0] = (short)(signed char)g_bCurrentKey_0046c014;
        keys[1] = -1;
        break;
    }

    key = keys;
    while (finished == 0) {
        switch (*key++) {
        default:
            handled--;
            break;
        case 0x33:
        case 0x52:
            g_bMouseCursorVisible_0046a018 = 0;
            if (g_nRollInput_0059d3f4 > 0) {
                g_nRollInput_0059d3f4 = 0;
            } else {
                if (shift != 0)
                    g_nRollInput_0059d3f4 = -9;
                if (g_nRollInput_0059d3f4 > -9)
                    g_nRollInput_0059d3f4--;
                else if (g_cPreviousKey_0046c018 < 0)
                    g_nRollInput_0059d3f4--;
                else
                    g_nRollInput_0059d3f4++;
            }
            break;
        case 0x34:
        case 0x53:
            g_bMouseCursorVisible_0046a018 = 0;
            if (g_nRollInput_0059d3f4 < 0) {
                g_nRollInput_0059d3f4 = 0;
            } else {
                if (shift != 0)
                    g_nRollInput_0059d3f4 = 9;
                if (g_nRollInput_0059d3f4 < 9)
                    g_nRollInput_0059d3f4++;
                else if (g_cPreviousKey_0046c018 < 0)
                    g_nRollInput_0059d3f4++;
                else
                    g_nRollInput_0059d3f4--;
            }
            break;
        case 0x48:
            g_bMouseCursorVisible_0046a018 = 0;
            if (g_nPitchInput_0059d3f0 < 0) {
                g_nPitchInput_0059d3f0 = 0;
            } else if (control == 0) {
                if (shift != 0)
                    g_nPitchInput_0059d3f0 = 9;
                if (g_nPitchInput_0059d3f0 < 9 ||
                    g_cPreviousKey_0046c018 < 0) {
                    g_bMouseCursorVisible_0046a018 = 0;
                    g_nPitchInput_0059d3f0++;
                } else {
                    g_nPitchInput_0059d3f0--;
                }
            } else {
                g_nSfxVolumeSetting_00469fbc++;
                if (g_nSfxVolumeSetting_00469fbc > 20)
                    g_nSfxVolumeSetting_00469fbc = 20;
                SaveVolumeSettingsToRegistry();
                SetSoundEffectsVolume(g_anVolumeLevels_00469fc8[
                    g_nSfxVolumeSetting_00469fbc / 2]);
                ShowOnScreenMessage(0, "SFX VOLUME: %d.",
                                    g_nSfxVolumeSetting_00469fbc / 2);
            }
            break;
        case 0x4b:
            if (control != 0) {
                g_nMusicVolumeSetting_00469fc0--;
                if (g_nMusicVolumeSetting_00469fc0 < 0)
                    g_nMusicVolumeSetting_00469fc0 = 0;
                SaveVolumeSettingsToRegistry();
                SetMusicStreamVolume((unsigned short)
                    g_anVolumeLevels_00469fc8[
                        g_nMusicVolumeSetting_00469fc0 / 2]);
                ShowOnScreenMessage(0, "MUSIC VOLUME: %d.",
                                    g_nMusicVolumeSetting_00469fc0 / 2);
                return;
            }
            g_bMouseCursorVisible_0046a018 = 0;
            if (g_nYawInput_0059d3f2 > 0) {
                g_nYawInput_0059d3f2 = 0;
            } else {
                if (shift != 0)
                    g_nYawInput_0059d3f2 = -9;
                if (g_nYawInput_0059d3f2 > -9)
                    g_nYawInput_0059d3f2--;
                else if (g_cPreviousKey_0046c018 < 0)
                    g_nYawInput_0059d3f2--;
                else
                    g_nYawInput_0059d3f2++;
            }
            break;
        case 0x4c:
            WarpMouseTo(
                (short)((g_stViewBuffer_005d2b00.left + g_stViewBuffer_005d2b00.right) / 2),
                (short)((g_stViewBuffer_005d2b00.top + g_stViewBuffer_005d2b00.bottom) / 2));
            g_nRollInput_0059d3f4 = 0;
            g_nPitchInput_0059d3f0 = 0;
            g_nYawInput_0059d3f2 = 0;
            init_player_input();
            break;
        case 0x4d:
            if (control != 0) {
                g_nMusicVolumeSetting_00469fc0++;
                if (g_nMusicVolumeSetting_00469fc0 > 20)
                    g_nMusicVolumeSetting_00469fc0 = 20;
                SaveVolumeSettingsToRegistry();
                SetMusicStreamVolume((unsigned short)
                    g_anVolumeLevels_00469fc8[
                        g_nMusicVolumeSetting_00469fc0 / 2]);
                ShowOnScreenMessage(0, "MUSIC VOLUME: %d.",
                                    g_nMusicVolumeSetting_00469fc0 / 2);
                return;
            }
            g_bMouseCursorVisible_0046a018 = 0;
            if (g_nYawInput_0059d3f2 < 0) {
                g_nYawInput_0059d3f2 = 0;
            } else {
                if (shift != 0)
                    g_nYawInput_0059d3f2 = 9;
                if (g_nYawInput_0059d3f2 < 9)
                    g_nYawInput_0059d3f2++;
                else if (g_cPreviousKey_0046c018 < 0)
                    g_nYawInput_0059d3f2++;
                else
                    g_nYawInput_0059d3f2--;
            }
            break;
        case 0x50:
            g_bMouseCursorVisible_0046a018 = 0;
            if (g_nPitchInput_0059d3f0 > 0) {
                g_nPitchInput_0059d3f0 = 0;
            } else if (control == 0) {
                if (shift != 0)
                    g_nPitchInput_0059d3f0 = -9;
                if (g_nPitchInput_0059d3f0 > -9)
                    g_nPitchInput_0059d3f0--;
                else if (g_cPreviousKey_0046c018 < 0)
                    g_nPitchInput_0059d3f0--;
                else {
                    g_bMouseCursorVisible_0046a018 = 0;
                    g_nPitchInput_0059d3f0++;
                }
            } else {
                g_nSfxVolumeSetting_00469fbc--;
                if (g_nSfxVolumeSetting_00469fbc < 0)
                    g_nSfxVolumeSetting_00469fbc = 0;
                SetSoundEffectsVolume(g_anVolumeLevels_00469fc8[
                    g_nSfxVolumeSetting_00469fbc / 2]);
                SaveVolumeSettingsToRegistry();
                ShowOnScreenMessage(0, "SFX VOLUME: %d.",
                                    g_nSfxVolumeSetting_00469fbc / 2);
            }
            break;
        case -1:
            finished++;
            break;
        }
    }
    return handled;
}

/* Function start: 0x466908 */
unsigned int fire_players_lasers(void)
{
    if (g_asObjectCounter_00494be0[0] == -1 &&
        g_asShipWeaponEnergy_0059d470[0] > 0) {
        fire_fixed_projectile_weapon(0);
        if (g_acShipTarget_00495f20[0] != -1 &&
            get_mode(1) == 5)
            SelectCockpitVduMode(1, 3);
    }
    return 0;
}

/* Function start: 0x46696E */
unsigned int players_flight_dynamics(void)
{
    ObjectTypeData *typeData;

    if (g_aeSpecialManeuver_00495600[0] ==
            SPECIAL_MANEUVER_BLOWING_UP) {
        if (g_asObjectCounter_00494be0[0] == -1) {
            typeData = &g_aObjectTypeData_00496d30[
                g_nPlayerShipType_00493464];
            if (g_anObjectYawRotation_0059ce80[0] < typeData->pitchRate &&
                g_anObjectPitchRotation_0059b2a0[0] < typeData->yawRate &&
                g_anObjectRollRotation_0059d7e0[0] < typeData->rollRate) {
                g_aeSpecialManeuver_00495600[0] = SPECIAL_MANEUVER_NONE;
            } else {
                g_anObjectYawRotation_0059ce80[0] -= g_nYawInput_0059d3f2;
                g_anObjectPitchRotation_0059b2a0[0] -= g_nPitchInput_0059d3f0;
            }
        }
        return 0;
    }

    typeData = &g_aObjectTypeData_00496d30[
        g_nPlayerShipType_00493464];
    g_anObjectPitchRotation_0059b2a0[0] =
        (short)((typeData->yawRate * g_nPitchInput_0059d3f0) / 8);
    g_anObjectYawRotation_0059ce80[0] =
        (short)-((typeData->pitchRate * g_nYawInput_0059d3f2) / 8);
    g_anObjectRollRotation_0059d7e0[0] =
        (short)-((typeData->rollRate * g_nRollInput_0059d3f4) / 8);
    return 0;
}

/* Function start: 0x466B02 */
unsigned int player_input(void)
{
    InputEventState event;
    short modifiers;
    short eventType;
    short queuedKeyEvent;
    short horizontal;
    short vertical;
    short yawInput;
    short pitchInput;
    int keyboardRoll;
    int viewportLeft;
    int afterburnerControl;
    unsigned int buttons;
    unsigned int key;

    g_cPreviousKey_0046c018 = (signed char)g_bCurrentKey_0046c014;
    g_nPreviousYawInput_0059ce72 = g_nYawInput_0059d3f2;
    g_nPreviousPitchInput_0059ce70 = g_nPitchInput_0059d3f0;
    g_nPreviousRollInput_0059ce74 = g_nRollInput_0059d3f4;
    keyboardRoll = 0;
    eventType = PollInputEvent(&event);
    modifiers = event.modifiers;
    g_wCurrentInputModifiers_0059ab08 = (unsigned short)modifiers;
    TranslatePolledInputEvent((unsigned short)eventType, event.value);
    g_bJoystickEventQueued_005a7b88 = FindQueuedInputEvent(6) != 0;
    g_bMouseMoveEventQueued_005a7b00 = FindQueuedInputEvent(13) != 0;
    queuedKeyEvent = FindQueuedInputEvent(5) != 0;
    queuedKeyEvent |= FindQueuedInputEvent(3) != 0;
    queuedKeyEvent |= FindQueuedInputEvent(4) != 0;
    g_bKeyboardEventQueued_005a7afe = queuedKeyEvent;
    g_bMouseButtonEventQueued_005a7afc = FindQueuedInputEvent(2) != 0;
    if (g_bMouseMoveEventQueued_005a7b00 == 0)
        g_bCurrentKey_0046c014 |= 0x80;
    g_bCurrentKey_0046c014 |= 0x80;

    if (g_bMouseCursorVisible_0046a018 == 0) {
        key = PollKeyboardState();
        g_bCurrentKey_0046c014 = (unsigned char)key;
        if (g_bCurrentKey_0046c014 == 0) {
            g_nRollInput_0059d3f4 = 0;
            g_bFlightRollLatch_0046a050 = 0;
            g_nPitchInput_0059d3f0 = 0;
            g_nYawInput_0059d3f2 = 0;
        } else {
            g_bMouseAfterburnerControl_0046a02c = 0;
            process_player_input();
            switch (g_bCurrentKey_0046c014) {
            case 0x33:
            case 0x34:
            case 0x52:
            case 0x53:
                keyboardRoll = 1;
            }
        }
    }

    if (g_bMouseButtonEventQueued_005a7afc == 0) {
        buttons = g_bHostSecondaryMouseButton_005a899c * 2 |
                  g_bHostPrimaryMouseButton_005a8998;
        if (buttons == 0) {
            g_bAfterburnerButtonLatched_0046a054 = 0;
        } else {
            if (buttons == 3) {
                g_bCurrentKey_0046c014 = 0x1c;
            } else if (buttons == 1) {
                g_bCurrentKey_0046c014 = 0x39;
                fire_players_lasers();
            }
            if ((buttons & 2) == 0)
                g_cPreviousKey_0046c018 = 0;
            if (g_cPreviousKey_0046c018 == 0x0f && buttons == 2)
                g_bCurrentKey_0046c014 = 0x0f;
            if (buttons == 1)
                fire_players_lasers();
        }
    }

    while ((eventType = GetNextInputEvent(&event)) != 0) {
        switch (eventType) {
        case 2:
            if ((short)event.value == 1) {
                g_bCurrentKey_0046c014 = 0x39;
                if ((event.modifiers & 4) != 0) {
                    if (g_aeSpecialManeuver_00495600[0] ==
                            SPECIAL_MANEUVER_AFTERBURNER)
                        fire_players_lasers();
                    else
                        g_bCurrentKey_0046c014 = 0x1c;
                }
            }
            if ((short)event.value == 3)
                g_bCurrentKey_0046c014 = 0x1c;
            if ((short)event.value == 2 &&
                g_bAfterburnerButtonLatched_0046a054 == 0) {
                if ((int)(DAT_0059ab54 -
                        g_dwLastSecondaryButtonPress_0046a04c) <=
                        g_nInputTickScale_0059af90)
                    g_bCurrentKey_0046c014 = 0x0f;
                g_bAfterburnerButtonLatched_0046a054 = 1;
            }
            if (g_cPreviousKey_0046c018 == 0x0f &&
                (short)event.value == 2)
                g_bCurrentKey_0046c014 = 0x0f;
            if ((short)event.value == 1)
                fire_players_lasers();
            g_dwLastSecondaryButtonPress_0046a04c = DAT_0059ab54;
            break;
        case 3:
        case 5:
            g_bMouseAfterburnerControl_0046a02c = 0;
            g_wCurrentInputModifiers_0059ab08 =
                (unsigned short)event.modifiers;
            g_bCurrentKey_0046c014 = (unsigned char)event.value;
            process_player_input();
            break;
        case 6:
            g_bMouseAfterburnerControl_0046a02c = 0;
            g_bMouseCursorVisible_0046a018 = 0;
            if (((unsigned char)
                    g_stLastPolledFlightInput_0046a020.buttons & 3) == 3) {
                if (g_aeSpecialManeuver_00495600[0] ==
                        SPECIAL_MANEUVER_AFTERBURNER)
                    fire_players_lasers();
                else
                    g_bCurrentKey_0046c014 = 0x1c;
            } else if ((g_stLastPolledFlightInput_0046a020.buttons & 1) != 0) {
                fire_players_lasers();
            }
            buttons = (g_stLastPolledFlightInput_0046a020.buttons & 2) >> 1;
            if (buttons != 0 &&
                g_aeSpecialManeuver_00495600[0] ==
                    SPECIAL_MANEUVER_AFTERBURNER)
                buttons = 0;
            if (buttons != 0) {
                g_nRollInput_0059d3f4 =
                    (short)g_stLastPolledFlightInput_0046a020.x;
                accelerate((short)-(
                    g_stLastPolledFlightInput_0046a020.y / 2));
            } else {
                if (g_nRollInput_0059d3f4 != 0 &&
                    g_bFlightRollLatch_0046a050 == 0 &&
                    keyboardRoll == 0) {
                    g_stPreviousFlightInput_005a7af0.x = -1;
                    g_nRollInput_0059d3f4 = 0;
                }
                if (g_stPreviousFlightInput_005a7af0.x !=
                        g_stLastPolledFlightInput_0046a020.x ||
                    g_stPreviousFlightInput_005a7af0.y !=
                        g_stLastPolledFlightInput_0046a020.y ||
                    g_stLastPolledFlightInput_0046a020.x != 0 ||
                    g_stLastPolledFlightInput_0046a020.y != 0) {
                    g_nPitchInput_0059d3f0 =
                        (short)-g_stLastPolledFlightInput_0046a020.y;
                    g_nYawInput_0059d3f2 =
                        (short)g_stLastPolledFlightInput_0046a020.x;
                }
            }
            if (g_asInputButton2DoubleClick_0059e520[
                    g_nActiveInputDevice_005a819c] != 0)
                g_bCurrentKey_0046c014 = 0x0f;
            if (g_cPreviousKey_0046c018 == 0x0f &&
                (g_stLastPolledFlightInput_0046a020.buttons & 2) != 0)
                g_bCurrentKey_0046c014 = 0x0f;
            break;
        case 13:
            afterburnerControl =
                (unsigned short)(modifiers & 4) >= 1;
            if (afterburnerControl != 0 &&
                g_aeSpecialManeuver_00495600[0] ==
                    SPECIAL_MANEUVER_AFTERBURNER)
                afterburnerControl = 0;
            if (g_bMouseCursorVisible_0046a018 == 0) {
                g_bMouseCursorVisible_0046a018 = 1;
                g_stMouseCursorState_0059ab10.frame = 2;
            }
            if (g_nCockpitDisplayMode_0049d71c == 0) {
                horizontal = (short)(event.x +
                    (g_stViewBuffer_005d2b00.left - g_stViewBuffer_005d2b00.right) / 2 + 1);
                vertical = (short)(event.y +
                    (g_stViewBuffer_005d2b00.top - g_stViewBuffer_005d2b00.bottom) / 2);
            } else {
                if (g_cCockpitView_0059dab0 == 0)
                    event.y = (short)(event.y - 10);
                else if (g_cCockpitView_0059dab0 == 1)
                    event.y = (short)(event.y - 25);
                horizontal = (short)(event.x +
                    (g_stViewBuffer_005d2b00.left - g_stViewBuffer_005d2b00.right) / 2 + 1);
                vertical = (short)(event.y - g_nViewCenterY_0059a854);
            }
            g_stMouseCursorState_0059ab10.x = event.x;
            g_stMouseCursorState_0059ab10.y = event.y;
            for (yawInput = 0;
                 g_asMouseYawThresholds_0046a030[yawInput] <=
                     abs((int)horizontal);
                 yawInput++)
                ;
            if (horizontal < 0)
                yawInput = (short)-yawInput;
            for (pitchInput = 0;
                 g_asMousePitchThresholds_0046a040[pitchInput] <=
                     abs((int)vertical);
                 pitchInput++)
                ;
            if (vertical < 0)
                pitchInput = (short)-pitchInput;
            viewportLeft = (int)g_stViewBuffer_005d2b00.left;
            if ((int)event.x - 4 <= viewportLeft)
                yawInput = -8;
            if ((int)g_stViewBuffer_005d2b00.right <= (int)event.x + 4)
                yawInput = 8;
            if ((int)event.y - 4 <= (int)g_stViewBuffer_005d2b00.top)
                pitchInput = -8;
            if ((int)g_stViewBuffer_005d2b00.bottom <= (int)event.y + 4)
                pitchInput = 8;
            if (yawInput > 8)
                yawInput = 8;
            if (yawInput < -8)
                yawInput = -8;
            if (pitchInput > 8)
                pitchInput = 8;
            if (pitchInput < -8)
                pitchInput = -8;
            if (afterburnerControl != 0) {
                g_bMouseAfterburnerControl_0046a02c = 1;
                pitchInput = (short)-pitchInput;
                g_nMouseYawInput_0046a058 = yawInput;
                g_nRollInput_0059d3f4 = yawInput;
                g_nMousePitchInput_0046a05c = pitchInput;
                accelerate((short)(pitchInput / 2));
            } else if (g_bMouseAfterburnerControl_0046a02c == 1) {
                g_nRollInput_0059d3f4 = 0;
                g_bMouseAfterburnerControl_0046a02c = 0;
                g_nMouseYawInput_0046a058 = 0;
                g_nYawInput_0059d3f2 = 0;
                g_nMousePitchInput_0046a05c = 0;
                g_nPitchInput_0059d3f0 = 0;
                WarpMouseTo(
                    (short)((viewportLeft + g_stViewBuffer_005d2b00.right) / 2),
                    (short)((g_stViewBuffer_005d2b00.bottom + g_stViewBuffer_005d2b00.top) / 2));
            } else {
                g_nRollInput_0059d3f4 = 0;
                g_nMouseYawInput_0046a058 = yawInput;
                g_nYawInput_0059d3f2 = yawInput;
                g_nMousePitchInput_0046a05c = pitchInput;
                g_nPitchInput_0059d3f0 = pitchInput;
            }
            break;
        }
    }

#ifdef WC1_SDL
    Wc1SdlApplyJoystickFlightControls();
#endif
    g_stPreviousFlightInput_005a7af0 =
        g_stLastPolledFlightInput_0046a020;
    return 0;
}

/* Function start: 0x465E25 */
void SelectNextExternalViewObject(void)
{
#if 0
    short object;

    object = (short)g_cViewObject_0049313c;
    g_cViewObject_0049313c = -1;
    do {
        object++;
        if (object > 9)
            object = 0;
        if (g_aeObjectClass_00495328[object] >= OBJECT_CLASS_SHIP)
            g_cViewObject_0049313c = (signed char)object;
    } while (g_cViewObject_0049313c == -1);
    return 0;
#else
    signed char object;

    object = g_cViewObject_0049313c;
    g_cViewObject_0049313c = -1;
    while (g_cViewObject_0049313c == -1) {
        object++;
        if (object > 9)
            object = 0;
        if (g_aeObjectClass_00495328[object] >= OBJECT_CLASS_SHIP)
            g_cViewObject_0049313c = object;
    }
#endif
}

/* Function start: WC2_UNMAPPED */
unsigned int SelectPreviousExternalViewObject(void)
{
    short object;

    object = (short)g_cViewObject_0049313c;
    g_cViewObject_0049313c = -1;
    do {
        object--;
        if (object < 0)
            object = 9;
        if (g_aeObjectClass_00495328[object] >= OBJECT_CLASS_SHIP)
            g_cViewObject_0049313c = (signed char)object;
    } while (g_cViewObject_0049313c == -1);
    return 0;
}

/* Function start: 0x45F200 */
unsigned int HandleFleetOverviewInput(void)
{
    signed char key;

    key = (signed char)g_bCurrentKey_0046c014;
    if (g_nCurrentView_00492fa8 != 8)
        return 0;

    g_bCurrentKey_0046c014 = 0;
    switch (key) {
    case 0x1c:
        g_cViewObject_0049313c--;
        g_bCurrentKey_0046c014 = 0x29;
        break;
    case 0x47:
        g_nCapitalShipViewDistance_00468ff4 -= 0x3200;
        break;
    case 0x48:
        rotate_about_i(-7,
                       &g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT],
                       &g_aShipForwardVector_00494208[WC1_EYE_OBJECT]);
        break;
    case 0x4b:
        rotate_about_j(7,
                       &g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT],
                       &g_aShipForwardVector_00494208[WC1_EYE_OBJECT]);
        break;
    case 0x4d:
        rotate_about_j(-7,
                       &g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT],
                       &g_aShipForwardVector_00494208[WC1_EYE_OBJECT]);
        break;
    case 0x4f:
        g_nCapitalShipViewDistance_00468ff4 += 0x3200;
        break;
    case 0x50:
        rotate_about_i(7,
                       &g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT],
                       &g_aShipForwardVector_00494208[WC1_EYE_OBJECT]);
        break;
    case 0x52:
        g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT].z = -0x100;
        g_aShipForwardVector_00494208[WC1_EYE_OBJECT].y = 0x100;
        g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT].x = 0x100;
        g_aShipForwardVector_00494208[WC1_EYE_OBJECT].z = 0;
        g_aShipForwardVector_00494208[WC1_EYE_OBJECT].x = 0;
        g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT].y = 0;
        g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT].x = 0;
        g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT].z = 0;
        g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT].y = 0;
        break;
    default:
        g_bCurrentKey_0046c014 = (unsigned char)key;
        break;
    }
    return 0;
}
