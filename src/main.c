/*
 *  WINGLEADER main module.
 *
 *  Address range 0x4274e0-0x427fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: main() at 0x004274E0, confirmed against the leaked DOS source screenshot.
 */
#include "wc1.h"

/* Function start: 0x4274E0 */
int main(short argc, char **argv)
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
    if (GetCurrentDiskDriveHook() > 'B')
        DAT_0059ab34 = 1;

    SystemDebugPrintf("Loading WING COMMANDER.  Please wait...\n");
    DAT_005a7d9c = 1;
    DAT_0046a9f8 = 4;
    DAT_0059a856 = 1;
    ResetCampaignData();

    argumentIndex = 0;
    while (argumentIndex < argumentCount) {
        argument = g_pStartupArguments_005a7b10[argumentIndex];
        if (strcmp("Origin", argument) == 0)
            g_nOriginDevUnlock_00469ff4 = 1;

        switch (argument[0]) {
        case '?':
            SystemDebugPrintf("Version %s.\n", g_pGameVersion_004693b4);
        case '-':
            if (argument[1] == 'm')
                g_nShowMemoryStatus_0046a00c = 1;
            if (g_nOriginDevUnlock_00469ff4 != 0) {
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
                DAT_0046a010 = (short)atoi(argument + 2);
            } else {
                DAT_0046a9f8 = 2;
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
            DAT_0046a9f8 = 3;
            break;
        case 'R':
        case 'r':
            DAT_0046a9f8 = 1;
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
            if (g_nOriginDevUnlock_00469ff4 != 0)
                launchMission = 1;
            break;
        case 'm':
            if (g_nOriginDevUnlock_00469ff4 != 0)
                mission = (short)atoi(argument + 1);
            break;
        case 's':
            if (g_nOriginDevUnlock_00469ff4 != 0) {
                DAT_004688f0 = 1;
                series = (short)atoi(argument + 1);
            }
            break;
        case 'w':
            if (g_nOriginDevUnlock_00469ff4 != 0) {
                animationDemo = 1;
                mission = (short)atoi(argument + 1);
            }
            break;
        }
        argumentIndex++;
    }

    SetCinematicFrameTiming();
    g_stCampaignState_0059ca50.currentSeries = (signed char)series;
    DAT_0046b168 = 0x13;
    g_stInitialCampaignState_004700b0.currentSeries =
        (signed char)series;
    g_stCampaignState_0059ca50.currentMission = (signed char)mission;
    g_stInitialCampaignState_004700b0.currentMission =
        (signed char)mission;
    LoadOriginFxDrivers();
    DAT_0046b168 = 0x13;

    if (animationDemo == 1) {
        SystemDebugPrintf(
            "Pre animation: %lu.\n",
            ((unsigned int (__cdecl *)(int))
                GetFixedOneMillionThunkAlt)(0));
        WaitForKeyAcknowledge(0);
        RunAnimationDemoLoop((signed char)mission);
        SystemDebugPrintf(
            "Post animation: %lu.\n",
            ((unsigned int (__cdecl *)(int))
                GetFixedOneMillionThunkAlt)(0));
        WaitForKeyAcknowledge(0);
    }

    LoadVolumeSettingsFromRegistry();
    SetSoundEffectsVolume(
        g_anVolumeLevels_00469fc8[g_nSfxVolumeSetting_00469fbc / 2]);
    SetMusicStreamVolume((unsigned short)g_anVolumeLevels_00469fc8[
        g_nMusicVolumeSetting_00469fc0 / 2]);

    if (launchMission != 0) {
        init_mission(series, mission);
        RunSpaceFlight(DAT_0046a010);
        exit_squadron("Bye!");
        return 0;
    }

    DAT_0059ab58 = 0;
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

/* Function start: 0x4279D0 */
unsigned int GetScreenUpdateFlag(void)
{
    if (message_showing())
        EndCommMenu();
    if (DAT_005a7510.pixels != 0)
        free_viewport(&DAT_005a7510);
    return 0;
}

/* Function start: 0x427A00 */
unsigned int initialize_view_buffer(void)
{
    if (DAT_0046a004 != 0 && DAT_005a7510.pixels == 0) {
        if (AllocateViewport(&DAT_005a7510, (short)DAT_004699d8,
                             0x20) == 0)
            ReportOutOfMemoryAndExit(g_szSpaceBuffer_0046a1d0);
    }
    return 0;
}

/* Function start: 0x427A40 */
unsigned int dump_buffer_to_screen(void)
{
    short bottom;

    if (DAT_0046a008 > 0) {
        CopyViewportContents(&DAT_005a7510, &DAT_005a6ba0);
        ShowMemoryStatusDebug();
        return 0;
    }
    switch ((int)g_cScreenViewportMode_0059a9f2) {
    case 4:
        bottom = DAT_005a6ba0.bottom;
        DAT_005a6ba0.top = 24;
        DAT_005a6ba0.bottom = 152;
        CopyViewportContents(&DAT_005a7510, &DAT_005a6ba0);
        DAT_005a6ba0.bottom = bottom;
        DAT_005a6ba0.top = 0;
        break;
    case 5:
        CopyViewportContents(&DAT_005a7510, &DAT_005a6ba0);
        break;
    default:
        fizzle_fade(&DAT_005a7510, &DAT_005a6ba0,
                    g_pScreenViewportGeometry_0059a9f4);
        break;
    }
    ShowMemoryStatusDebug();
    return 0;
}

/* Function start: 0x427B00 */
unsigned int clear_view_buffer(void)
{
    ClearViewport(&DAT_005a7510, (unsigned char)DAT_004699d8);
    return 0;
}

/* Function start: 0x427B20 */
unsigned int InitializeConversationViewport(void)
{
    ClearViewport(&g_stModalSourceViewport_005a7670, DAT_0046999c);
    DAT_005a6ba0.top = 24;
    DAT_005a6ba0.bottom = 151;
    DAT_005a76b0.left = 0;
    DAT_005a76b0.right = 319;
    DAT_005a76b0.top = 0;
    DAT_005a76b0.bottom = 127;
    if (AllocateViewport(&DAT_005a76b0, (short)DAT_0046999c, 0) == 0)
        ReportPacketLoadError(0, 0, 0, 0,
                              g_szAllocateBufferTag_0046a1e0);
    return 0;
}

/* Function start: 0x427BA0 */
unsigned int ResetScreenClipToFullHeight(void)
{
    free_viewport(&DAT_005a76b0);
    DAT_005a6baa = 0;
    DAT_005a6bae = 199;
    return 0;
}

/* Function start: 0x427BC0 */
unsigned int InitializeConversationText(void)
{
    g_stConversationTextViewport_005a7570 =
        g_stModalSourceViewport_005a7670;
    g_stConversationTextViewport_005a7570.top = 152;
    g_stConversationTextContext_005a7760.viewport =
        &g_stConversationTextViewport_005a7570;
    g_stConversationTextContext_005a7760.text =
        g_szDefaultTextBuffer_005a7590;
    g_stConversationTextContext_005a7760.alignment = 2;
    InitializeTextContextFromFont(&g_stConversationTextContext_005a7760,
                                  0,
                                  g_cViewportClearColour_004699a0,
                                  DAT_0046999c);
    SetTextContext(&g_stConversationTextContext_005a7760);
    return 0;
}

/* Function start: 0x427C30 */
unsigned int RefreshMemoryStatusOverlay(void)
{
    WaitForVerticalBlankThunk();
    CopyViewportContents(&DAT_005a76b0, &DAT_005a6ba0);
    ShowMemoryStatusDebug();
    return 0;
}

/* Function start: 0x427C50 */
unsigned int Update_3Space(void)
{
    house_keep();
    house_keep_objects();
    update_objects_in_space();
    set_eye_direction_and_position();
    servicetrack();
    g_nSpaceFrame_0059b420++;
    return 0;
}

/* Function start: 0x427C80 */
unsigned int TriggerPlayerHitPaletteFlash(void)
{
    if (DAT_0046c03c <= 3)
        DAT_005a7780[0] = 0x30;
    return 0;
}

/* Function start: 0x427CA0 */
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

/* Function start: 0x427CD0 */
unsigned int UpdateSpacePaletteFade(void)
{
    if (DAT_005a7780[0] != 0) {
        switch ((int)(short)DAT_0046b168) {
        case 9:
        case 13:
            ClearViewport(&DAT_005a7510, (short)DAT_004699ac);
            g_bViewportDirty_00469fc4 = 1;
            DAT_005a7780[0] = 0;
            break;
        case 0x13:
            DAT_005a7780[0] = (short)(DAT_005a7780[0] - 4);
            SetPaletteEntry((short)DAT_004699d8,
                            DAT_005a7780);
            break;
        }
    }
    return 0;
}

/* Function start: 0x427D40 */
unsigned int house_keep(void)
{
    short palette;

    if (g_nCannedSceneMode_00469fac == 0 &&
        g_nTrainSimActive_00469e2c == 0) {
        if ((g_nSpaceFrame_0059b420 & 0x1f) == 0)
            ReleaseStaleNavTarget();
        if (g_nHazardFieldCount_0059c90c != 0 &&
            (g_nSpaceFrame_0059b420 & 0xf) == 0)
            check_hazards();
    }
    if (DAT_0046c03c == 0) {
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
        g_abCockpitLightGoal_005a7eb8[3] = 0;
    }
    return 0;
}

/* Function start: 0x427DF0 */
void init_player_input(void)
{
    SetMousePosition(
        (DAT_005a7510.right - DAT_005a7510.left) / 2 + 1,
        g_nViewCenterY_0059a854);
    ClearDebugPauseFlags();
    g_bMouseCursorVisible_0046a018 = 0;
    g_bPointerMovedByKeyboard_005a7d54 = 1;
}

/* Function start: 0x427E40 */
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

/* Function start: 0x427F20 */
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
                ShowOnScreenMessage(0, 0, "SFX VOLUME: %d.",
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
                ShowOnScreenMessage(0, 0, "MUSIC VOLUME: %d.",
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
                (short)((DAT_005a7510.left + DAT_005a7510.right) / 2),
                (short)((DAT_005a7510.top + DAT_005a7510.bottom) / 2));
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
                ShowOnScreenMessage(0, 0, "MUSIC VOLUME: %d.",
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
                ShowOnScreenMessage(0, 0, "SFX VOLUME: %d.",
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

/* Function start: 0x428480 */
unsigned int fire_players_lasers(void)
{
    if (g_asObjectCounter_0059c330[0] == -1 &&
        g_asShipWeaponEnergy_0059d470[0] > 0) {
        fire_fixed_projectile_weapon(0);
        if (g_acShipTarget_0059ce60[0] != -1 &&
            (short)get_mode(1) == 5)
            SelectCockpitVduMode(1, 3);
    }
    return 0;
}

/* Function start: 0x4284D0 */
unsigned int players_flight_dynamics(void)
{
    ObjectTypeData *typeData;

    if (g_aeSpecialManeuver_0059c3c0[0] ==
            SPECIAL_MANEUVER_BLOWING_UP) {
        if (g_asObjectCounter_0059c330[0] != -1)
            return 0;
        typeData = &g_aObjectTypeData_00466458[
            g_stCampaignState_0059ca50.playerShipType];
        if (g_anObjectYawRotation_0059ce80[0] < typeData->pitchRate &&
            g_anObjectPitchRotation_0059b2a0[0] < typeData->yawRate &&
            g_anObjectRollRotation_0059d7e0[0] < typeData->rollRate) {
            g_aeSpecialManeuver_0059c3c0[0] = SPECIAL_MANEUVER_NONE;
        } else {
            g_anObjectYawRotation_0059ce80[0] -= g_nYawInput_0059d3f2;
            g_anObjectPitchRotation_0059b2a0[0] -= g_nPitchInput_0059d3f0;
        }
        return 0;
    }

    typeData = &g_aObjectTypeData_00466458[
        g_stCampaignState_0059ca50.playerShipType];
    g_anObjectPitchRotation_0059b2a0[0] =
        (short)((typeData->yawRate * g_nPitchInput_0059d3f0) / 8);
    g_anObjectYawRotation_0059ce80[0] =
        (short)-((typeData->pitchRate * g_nYawInput_0059d3f2) / 8);
    g_anObjectRollRotation_0059d7e0[0] =
        (short)-((typeData->rollRate * g_nRollInput_0059d3f4) / 8);
    return 0;
}

/* Function start: 0x4285D0 */
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
    eventType = PollInputEvent(&event, 0xff);
    modifiers = event.modifiers;
    g_wCurrentInputModifiers_0059ab08 = (unsigned short)modifiers;
    TranslatePolledInputEvent((unsigned short)eventType, event.value);
    g_bJoystickEventQueued_005a7b88 = IsInputEventQueued(6);
    g_bMouseMoveEventQueued_005a7b00 = IsInputEventQueued(13);
    queuedKeyEvent = IsInputEventQueued(5);
    queuedKeyEvent |= IsInputEventQueued(3);
    queuedKeyEvent |= IsInputEventQueued(4);
    g_bKeyboardEventQueued_005a7afe = queuedKeyEvent;
    g_bMouseButtonEventQueued_005a7afc = IsInputEventQueued(2);
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
                    if (g_aeSpecialManeuver_0059c3c0[0] ==
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
                if (g_aeSpecialManeuver_0059c3c0[0] ==
                        SPECIAL_MANEUVER_AFTERBURNER)
                    fire_players_lasers();
                else
                    g_bCurrentKey_0046c014 = 0x1c;
            } else if ((g_stLastPolledFlightInput_0046a020.buttons & 1) != 0) {
                fire_players_lasers();
            }
            buttons = (g_stLastPolledFlightInput_0046a020.buttons & 2) >> 1;
            if (buttons != 0 &&
                g_aeSpecialManeuver_0059c3c0[0] ==
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
                g_aeSpecialManeuver_0059c3c0[0] ==
                    SPECIAL_MANEUVER_AFTERBURNER)
                afterburnerControl = 0;
            if (g_bMouseCursorVisible_0046a018 == 0) {
                g_bMouseCursorVisible_0046a018 = 1;
                DAT_0059ab1d = 2;
            }
            if (DAT_0046a008 == 0) {
                horizontal = (short)(event.x +
                    (DAT_005a7510.left - DAT_005a7510.right) / 2 + 1);
                vertical = (short)(event.y +
                    (DAT_005a7510.top - DAT_005a7510.bottom) / 2);
            } else {
                if (g_cCockpitView_0059dab0 == 0)
                    event.y = (short)(event.y - 10);
                else if (g_cCockpitView_0059dab0 == 1)
                    event.y = (short)(event.y - 25);
                horizontal = (short)(event.x +
                    (DAT_005a7510.left - DAT_005a7510.right) / 2 + 1);
                vertical = (short)(event.y - g_nViewCenterY_0059a854);
            }
            g_nMouseX_0059ab10 = event.x;
            g_nMouseY_0059ab12 = event.y;
            yawInput = 0;
            while (g_asMouseYawThresholds_0046a030[yawInput] <=
                    abs((int)horizontal))
                yawInput++;
            if (horizontal < 0)
                yawInput = (short)-yawInput;
            pitchInput = 0;
            while (g_asMousePitchThresholds_0046a040[pitchInput] <=
                    abs((int)vertical))
                pitchInput++;
            if (vertical < 0)
                pitchInput = (short)-pitchInput;
            viewportLeft = (int)DAT_005a7510.left;
            if ((int)event.x - 4 <= viewportLeft)
                yawInput = -8;
            if ((int)DAT_005a7510.right <= (int)event.x + 4)
                yawInput = 8;
            if ((int)event.y - 4 <= (int)DAT_005a7510.top)
                pitchInput = -8;
            if ((int)DAT_005a7510.bottom <= (int)event.y + 4)
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
                    (short)((viewportLeft + DAT_005a7510.right) / 2),
                    (short)((DAT_005a7510.bottom + DAT_005a7510.top) / 2));
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

    g_stPreviousFlightInput_005a7af0 =
        g_stLastPolledFlightInput_0046a020;
    return 0;
}

/* Function start: 0x428C90 */
unsigned int SelectNextExternalViewObject(void)
{
    short object;

    object = (short)g_cViewObject_0046c000;
    g_cViewObject_0046c000 = -1;
    do {
        object++;
        if (object > 9)
            object = 0;
        if (g_aeObjectClass_0059d100[object] >= OBJECT_CLASS_SHIP)
            g_cViewObject_0046c000 = (signed char)object;
    } while (g_cViewObject_0046c000 == -1);
    return 0;
}

/* Function start: 0x428CD0 */
unsigned int SelectPreviousExternalViewObject(void)
{
    short object;

    object = (short)g_cViewObject_0046c000;
    g_cViewObject_0046c000 = -1;
    do {
        object--;
        if (object < 0)
            object = 9;
        if (g_aeObjectClass_0059d100[object] >= OBJECT_CLASS_SHIP)
            g_cViewObject_0046c000 = (signed char)object;
    } while (g_cViewObject_0046c000 == -1);
    return 0;
}

/* Function start: 0x428D10 */
unsigned int HandleFleetOverviewInput(void)
{
    signed char key;

    key = (signed char)g_bCurrentKey_0046c014;
    if (DAT_0046c03c != 8)
        return 0;

    g_bCurrentKey_0046c014 = 0;
    switch (key) {
    case 0x1c:
        g_cViewObject_0046c000--;
        g_bCurrentKey_0046c014 = 0x29;
        break;
    case 0x47:
        g_nCapitalShipViewDistance_00468ff4 -= 0x3200;
        break;
    case 0x48:
        rotate_about_i(-7,
                       &g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT],
                       &g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT]);
        break;
    case 0x4b:
        rotate_about_j(7,
                       &g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT],
                       &g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT]);
        break;
    case 0x4d:
        rotate_about_j(-7,
                       &g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT],
                       &g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT]);
        break;
    case 0x4f:
        g_nCapitalShipViewDistance_00468ff4 += 0x3200;
        break;
    case 0x50:
        rotate_about_i(7,
                       &g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT],
                       &g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT]);
        break;
    case 0x52:
        g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT].z = -0x100;
        g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT].y = 0x100;
        g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT].x = 0x100;
        g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT].z = 0;
        g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT].x = 0;
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
