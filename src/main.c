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
    int selection;
    short gameFlowResult;

    (void)argc;
    (void)argv;

    GetShutdownErrorCode();
    ExitCleanupHook();
    StartupHook();
    DAT_0059ab4c = GetStartupErrorCode();
    ShutdownHook();

    _chdir("gamedat");
    LoadInstallDat();
    _chdir("..");
    if (GetCurrentDiskDriveHook() > 'B')
        DAT_0059ab34 = 1;

    SystemDebugPrintf("Loading WING COMMANDER.  Please wait...\n");
    DAT_005a7d9c = 1;
    DAT_0046a9f8 = 4;
    DAT_0059a856 = 1;

    SetCinematicFrameTiming();
    g_stCampaignState_0059ca50.currentSeries = 1;
    DAT_0046b168 = 0x13;
    g_stInitialCampaignState_004700b0.currentSeries = 1;
    g_stCampaignState_0059ca50.currentMission = 0;
    g_stInitialCampaignState_004700b0.currentMission = 0;
    LoadOriginFxDrivers();
    DAT_0046b168 = 0x13;

    LoadVolumeSettingsFromRegistry();
    SetSoundEffectsVolume(DAT_00469fc8[DAT_00469fbc / 2]);
    SetMusicStreamVolume((unsigned short)DAT_00469fc8[DAT_00469fc0 / 2]);

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
    if (IsAutopilotEngaged())
        EndCommMenu();
    if (DAT_005a7510.pixels != 0)
        free_viewport(&DAT_005a7510);
    return 0;
}

/* Function start: 0x427A00 */
unsigned int initialize_view_buffer(void)
{
    if (DAT_005a7510.pixels == 0) {
        if (AllocateViewport(&DAT_005a7510, (short)DAT_004699d8,
                             0x20) == 0)
            ReportOutOfMemoryAndExit();
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

/* Function start: 0x427BA0 */
unsigned int ResetScreenClipToFullHeight(void)
{
    free_viewport(&DAT_005a76b0);
    DAT_005a6baa = 0;
    DAT_005a6bae = 199;
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
void Update_3Space(void)
{
    house_keep();
    house_keep_objects();
    update_objects_in_space();
    set_eye_direction_and_position();
    g_nSpaceFrame_0059b420++;
}

/* Function start: 0x427C80 */
unsigned int SetDefaultCommDelay(void)
{
    if (DAT_0046c03c < 4)
        DAT_005a7780 = 0x30;
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
    if (DAT_005a7780 != 0) {
        if (DAT_0046b168 == 9 || DAT_0046b168 == 13) {
            ClearViewport(&DAT_005a7510, (short)DAT_004699ac);
            g_bViewportDirty_00469fc4 = 1;
            DAT_005a7780 = 0;
            return 0;
        }
        if (DAT_0046b168 != 0x13)
            return 0;
        DAT_005a7780 = (unsigned short)(DAT_005a7780 - 4);
        SetPaletteEntry((short)DAT_004699d8, (short *)&DAT_005a7780);
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
            FadeFlightPaletteEntry(
                (short *)&g_aPaletteFadeEntries_005a76d0[palette]);
            SetPaletteEntry((short)(palette + 0xb9),
                            (short *)&g_aPaletteFadeEntries_005a76d0[palette]);
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

/* Function start: 0x427E40 */
void PollSpaceFlightInput(void)
{
    int device;
    InputDeviceSample *sample;

    if (g_nActiveInputDevice_005a819c == -1 ||
        g_bInputPollingGuard_0046a01c != 0)
        return;

    g_bInputPollingGuard_0046a01c = 1;
    UpdateInputDeviceTransitions(0);
    device = (int)g_nActiveInputDevice_005a819c;
    sample = &g_aInputDeviceSamples_005a81f0[device];
    if (sample->x != 0 || sample->y != 0 || sample->buttons != 0 ||
        DAT_0046a020 != 0 || DAT_0046a024 != 0 || DAT_0046a028 != 0)
        TranslatePolledInputEvent(6, 0);
    DAT_0046a020 = sample->x;
    DAT_0046a024 = sample->y;
    DAT_0046a028 = sample->buttons;
    g_bInputPollingGuard_0046a01c--;
}

/* Function start: 0x427F20 */
int process_player_input(void)
{
    short keys[3];
    short *key;
    int shift;
    int control;
    short handled;

    shift = GetShiftKeyState();
    control = GetControlKeyState();
    handled = 1;
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
    while (*key != -1) {
        switch (*key++) {
        case 0x33:
        case 0x52:
            g_bMouseCursorVisible_0046a018 = 0;
            if (g_nRollInput_0059d3f4 > 0)
                g_nRollInput_0059d3f4 = 0;
            else
                g_nRollInput_0059d3f4 =
                    MaxShort((short)(g_nRollInput_0059d3f4 -
                                     (shift != 0 ? 9 : 1)), -10);
            break;
        case 0x34:
        case 0x53:
            g_bMouseCursorVisible_0046a018 = 0;
            if (g_nRollInput_0059d3f4 < 0)
                g_nRollInput_0059d3f4 = 0;
            else
                g_nRollInput_0059d3f4 =
                    MinShort((short)(g_nRollInput_0059d3f4 +
                                     (shift != 0 ? 9 : 1)), 10);
            break;
        case 0x48:
            g_bMouseCursorVisible_0046a018 = 0;
            if (control == 0) {
                if (g_nPitchInput_0059d3f0 < 0)
                    g_nPitchInput_0059d3f0 = 0;
                else
                    g_nPitchInput_0059d3f0 =
                        MinShort((short)(g_nPitchInput_0059d3f0 +
                                         (shift != 0 ? 9 : 1)), 10);
            }
            break;
        case 0x4b:
            g_bMouseCursorVisible_0046a018 = 0;
            if (control == 0) {
                if (g_nYawInput_0059d3f2 > 0)
                    g_nYawInput_0059d3f2 = 0;
                else
                    g_nYawInput_0059d3f2 =
                        MaxShort((short)(g_nYawInput_0059d3f2 -
                                         (shift != 0 ? 9 : 1)), -10);
            }
            break;
        case 0x4c:
            SetMousePosition(
                (DAT_005a7510.left + DAT_005a7510.right) / 2,
                (DAT_005a7510.top + DAT_005a7510.bottom) / 2);
            g_nRollInput_0059d3f4 = 0;
            g_nPitchInput_0059d3f0 = 0;
            g_nYawInput_0059d3f2 = 0;
            break;
        case 0x4d:
            g_bMouseCursorVisible_0046a018 = 0;
            if (control == 0) {
                if (g_nYawInput_0059d3f2 < 0)
                    g_nYawInput_0059d3f2 = 0;
                else
                    g_nYawInput_0059d3f2 =
                        MinShort((short)(g_nYawInput_0059d3f2 +
                                         (shift != 0 ? 9 : 1)), 10);
            }
            break;
        case 0x50:
            g_bMouseCursorVisible_0046a018 = 0;
            if (control == 0) {
                if (g_nPitchInput_0059d3f0 > 0)
                    g_nPitchInput_0059d3f0 = 0;
                else
                    g_nPitchInput_0059d3f0 =
                        MaxShort((short)(g_nPitchInput_0059d3f0 -
                                         (shift != 0 ? 9 : 1)), -10);
            }
            break;
        default:
            handled = 0;
            break;
        }
    }
    return handled;
}

/* Function start: 0x428480 */
unsigned int fire_players_lasers(void)
{
    if (g_asObjectCounter_0059c330[0] == -1 &&
        g_asShipWeaponEnergy_0059d470[0] > 0)
        fire_fixed_projectile_weapon(0);
    return 0;
}

/* Function start: 0x4284D0 */
unsigned int players_flight_dynamics(void)
{
    if (g_aeSpecialManeuver_0059c3c0[0] !=
            SPECIAL_MANEUVER_BLOWING_UP) {
        g_anObjectPitchRotation_0059b2a0[0] =
            (short)((g_aObjectTypeData_00466458[
                g_stCampaignState_0059ca50.playerShipType].yawRate *
                g_nPitchInput_0059d3f0) / 8);
        g_anObjectYawRotation_0059ce80[0] =
            (short)-((g_aObjectTypeData_00466458[
                g_stCampaignState_0059ca50.playerShipType].pitchRate *
                g_nYawInput_0059d3f2) / 8);
        g_anObjectRollRotation_0059d7e0[0] =
            (short)-((g_aObjectTypeData_00466458[
                g_stCampaignState_0059ca50.playerShipType].rollRate *
                g_nRollInput_0059d3f4) / 8);
    } else if (g_asObjectCounter_0059c330[0] == -1) {
        if (g_anObjectYawRotation_0059ce80[0] <
                g_aObjectTypeData_00466458[
                    g_stCampaignState_0059ca50.playerShipType].pitchRate &&
            g_anObjectPitchRotation_0059b2a0[0] <
                g_aObjectTypeData_00466458[
                    g_stCampaignState_0059ca50.playerShipType].yawRate &&
            g_anObjectRollRotation_0059d7e0[0] <
                g_aObjectTypeData_00466458[
                    g_stCampaignState_0059ca50.playerShipType].rollRate) {
            g_aeSpecialManeuver_0059c3c0[0] = SPECIAL_MANEUVER_NONE;
        } else {
            g_anObjectYawRotation_0059ce80[0] -= g_nYawInput_0059d3f2;
            g_anObjectPitchRotation_0059b2a0[0] -= g_nPitchInput_0059d3f0;
        }
    }
    return 0;
}

/* Function start: 0x4285D0 */
unsigned int player_input(void)
{
    InputEventState event;
    InputDeviceSample *sample;
    short eventType;
    int centreX;
    int centreY;
    int halfWidth;
    int halfHeight;
    unsigned int key;

    g_cPreviousKey_0046c018 = (signed char)g_bCurrentKey_0046c014;
    g_bCurrentKey_0046c014 |= 0x80;

    if (g_bMouseCursorVisible_0046a018 == 0) {
        key = PollKeyboardState();
        g_bCurrentKey_0046c014 = (unsigned char)key;
        if (key == 0) {
            g_nRollInput_0059d3f4 = 0;
            g_nPitchInput_0059d3f0 = 0;
            g_nYawInput_0059d3f2 = 0;
        } else {
            DAT_0046a02c = 0;
            process_player_input();
        }
    }

    eventType = PollInputEvent(&event, 0xff);
    while (eventType != 0) {
        switch (eventType) {
        case 2:
            if ((event.value & 1) != 0) {
                g_bCurrentKey_0046c014 = 0x39;
                fire_players_lasers();
            }
            if ((event.value & 3) == 3)
                g_bCurrentKey_0046c014 = 0x1c;
            break;
        case 3:
        case 5:
            g_bCurrentKey_0046c014 = (unsigned char)event.value;
            DAT_0046a02c = 0;
            process_player_input();
            break;
        case 4:
            if ((unsigned char)event.value == g_bCurrentKey_0046c014)
                g_bCurrentKey_0046c014 |= 0x80;
            break;
        case 6:
            if (g_nActiveInputDevice_005a819c != -1) {
                sample = &g_aInputDeviceSamples_005a81f0[
                    g_nActiveInputDevice_005a819c];
                if ((sample->buttons & 1) != 0)
                    fire_players_lasers();
                if ((sample->buttons & 3) == 3)
                    g_bCurrentKey_0046c014 = 0x1c;
                if ((sample->buttons & 2) != 0 &&
                    g_aeSpecialManeuver_0059c3c0[0] !=
                        SPECIAL_MANEUVER_AFTERBURNER) {
                    g_nRollInput_0059d3f4 = (short)sample->x;
                    celerate(0, -(sample->y / 2) * 0x100);
                } else {
                    g_nPitchInput_0059d3f0 = (short)-sample->y;
                    g_nYawInput_0059d3f2 = (short)sample->x;
                }
            }
            break;
        case 13:
            g_bMouseCursorVisible_0046a018 = 1;
            g_nMouseX_0059ab10 = event.x;
            g_nMouseY_0059ab12 = event.y;
            centreX = (DAT_005a7510.left + DAT_005a7510.right) / 2;
            centreY = DAT_0046a008 == 0 ?
                (DAT_005a7510.top + DAT_005a7510.bottom) / 2 :
                g_nViewCenterY_0059a854;
            halfWidth = (DAT_005a7510.right - DAT_005a7510.left) / 2;
            halfHeight = (DAT_005a7510.bottom - DAT_005a7510.top) / 2;
            if (halfWidth < 1)
                halfWidth = 1;
            if (halfHeight < 1)
                halfHeight = 1;
            g_nYawInput_0059d3f2 = (short)MaxInt(-8,
                MinInt(8, ((int)event.x - centreX) * 8 / halfWidth));
            g_nPitchInput_0059d3f0 = (short)MaxInt(-8,
                MinInt(8, ((int)event.y - centreY) * 8 / halfHeight));
            g_nRollInput_0059d3f4 = 0;
            break;
        }
        eventType = GetNextInputEvent(&event);
    }

    if (g_bHostPrimaryMouseButton_005a8998 != 0)
        fire_players_lasers();
    if (g_bHostPrimaryMouseButton_005a8998 != 0 &&
        g_bHostSecondaryMouseButton_005a899c != 0)
        g_bCurrentKey_0046c014 = 0x1c;
    return 0;
}
