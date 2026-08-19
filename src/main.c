/*
 *  WINGLEADER main module.
 *
 *  Address range 0x4274e0-0x427fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: main() at 0x004274E0, confirmed against the leaked DOS source screenshot.
 */
#include "wc1.h"

/* Function start: 0x4274E0 */
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

    GetShutdownErrorCode(abDiskPromptDriveState);
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
    nMusicPlaybackMode = 4;
    bExpandedMemoryAvailable = 1;
    ResetCampaignData();

    for (argumentIndex = 0;
         argumentIndex < argumentCount;
         argumentIndex++) {
        argument = pStartupArguments[argumentIndex];
        if (strcmp("Origin", argument) == 0)
            nOriginDevUnlock = 1;

        switch (argument[0]) {
        case '?':
            SystemDebugPrintf("Version %s.\n", pGameVersion);
        case '-':
            if (argument[1] == 'm')
                nShowMemoryStatus = 1;
            if (nOriginDevUnlock != 0) {
                switch (argument[1]) {
                case 'b':
                    bPlayerCollisionResponse = 0;
                    break;
                case 'f':
                    bShowFrameRate = 1;
                    break;
                case 'k':
                    bPlayerVulnerable = 0;
                    break;
                case 'q':
                    bDirectDrawModeCascadeEnabled = 0;
                    break;
                }
            }
            break;
        case 'A':
        case 'a':
            if (argument[1] == 's' || argument[1] == 'S') {
                nStartNavPointOverride = (short)atoi(argument + 2);
            } else {
                nMusicPlaybackMode = 2;
                nArcadeStartupParameter =
                    (short)atoi(argument + 1);
            }
            break;
        case 'E':
        case 'e':
            bSlowSceneAnimation = 1;
            break;
        case 'P':
        case 'p':
            nMusicPlaybackMode = 3;
            break;
        case 'R':
        case 'r':
            nMusicPlaybackMode = 1;
            break;
        case 'T':
        case 't':
            bSlowSceneAnimation = 3;
            break;
        case 'V':
        case 'v':
            bSlowSceneAnimation = 0;
            break;
        case 'Z':
        case 'z':
            DAT_005a7d9c = 1;
            break;
        case 'l':
            if (nOriginDevUnlock != 0)
                launchMission = 1;
            break;
        case 'm':
            if (nOriginDevUnlock != 0)
                mission = (short)atoi(argument + 1);
            break;
        case 's':
            if (nOriginDevUnlock != 0) {
                bCampaignActive = 1;
                series = (short)atoi(argument + 1);
            }
            break;
        case 'w':
            if (nOriginDevUnlock != 0) {
                animationDemo = 1;
                mission = (short)atoi(argument + 1);
            }
            break;
        }
    }

    SetCinematicFrameTiming();
    stCampaignState.currentSeries = (signed char)series;
    nVideoMode = 0x13;
    stInitialCampaignState.currentSeries =
        (signed char)series;
    stCampaignState.currentMission = (signed char)mission;
    stInitialCampaignState.currentMission =
        (signed char)mission;
    LoadOriginFxDrivers();
    nVideoMode = 0x13;

    if (animationDemo == 1) {
        SystemDebugPrintf("Pre animation: %lu.\n",
                          GetFixedOneMillionThunkAlt(0));
        WaitForKeyAcknowledge(0);
        RunAnimationDemoLoop((signed char)mission);
        SystemDebugPrintf("Post animation: %lu.\n",
                          GetFixedOneMillionThunkAlt(0));
        WaitForKeyAcknowledge(0);
    }

    LoadVolumeSettingsFromRegistry();
    SetSoundEffectsVolume(
        anVolumeLevels[nSfxVolumeSetting / 2]);
    SetMusicStreamVolume((unsigned short)anVolumeLevels[
        nMusicVolumeSetting / 2]);

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
        if (bCockpitEnabled != 0)
            LaunchPlayerShip();
#endif
        RunSpaceFlight(nStartNavPointOverride);
        exit_squadron("Bye!");
        return 0;
    }

#ifdef WC1_SDL
    Wc1SdlPlayDosStartupIntro();
#endif
    bEscapePressed = 0;
    for (;;) {
        FrameStartHook(0);
        bCampaignStartupMode = 1;
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
    if (stSpaceBuffer.pixels != 0)
        free_viewport(&stSpaceBuffer);
    return 0;
}

/* Function start: 0x427A00 */
unsigned int initialize_view_buffer(void)
{
    if (bViewBufferEnabled != 0 && stSpaceBuffer.pixels == 0) {
        if (AllocateViewport(&stSpaceBuffer,
                             (short)cPrimaryViewBufferColour,
                             0x20) == 0)
            ReportOutOfMemoryAndExit(szSpaceBuffer);
    }
    return 0;
}

/* Function start: 0x427A40 */
unsigned int dump_buffer_to_screen(void)
{
    short bottom;

#ifdef WC1_SDL
    Wc1SdlCompleteSpaceFrame();
#endif
    if (bCockpitlessView > 0) {
        CopyViewportContents(&stSpaceBuffer, &stScreen);
        ShowMemoryStatusDebug();
        return 0;
    }
    switch ((int)cScreenViewportMode) {
    case 4:
        bottom = stScreen.bottom;
        stScreen.top = 24;
        stScreen.bottom = 152;
        CopyViewportContents(&stSpaceBuffer, &stScreen);
        stScreen.bottom = bottom;
        stScreen.top = 0;
        break;
    case 5:
        CopyViewportContents(&stSpaceBuffer, &stScreen);
        break;
    default:
        fizzle_fade(&stSpaceBuffer, &stScreen,
                    pScreenViewportGeometry);
        break;
    }
    ShowMemoryStatusDebug();
    return 0;
}

/* Function start: 0x427B00 */
unsigned int clear_view_buffer(void)
{
    ClearViewport(&stSpaceBuffer,
                  (unsigned char)cPrimaryViewBufferColour);
    return 0;
}

/* Function start: 0x427B20 */
unsigned int InitializeConversationViewport(void)
{
    ClearViewport(&stModalSourceViewport, cBlackColour);
    stScreen.top = 24;
    stScreen.bottom = 151;
    stSceneBuffer.left = 0;
    stSceneBuffer.right = 319;
    stSceneBuffer.top = 0;
    stSceneBuffer.bottom = 127;
    if (AllocateViewport(&stSceneBuffer,
                         (short)cBlackColour, 0) == 0)
        ReportPacketLoadError(0, 0, 0, 0,
                              szAllocateBufferTag);
    return 0;
}

/* Function start: 0x427BA0 */
unsigned int ResetScreenClipToFullHeight(void)
{
    free_viewport(&stSceneBuffer);
    stScreen.top = 0;
    stScreen.bottom = 199;
    return 0;
}

/* Function start: 0x427BC0 */
unsigned int InitializeConversationText(void)
{
    stConversationTextViewport =
        stModalSourceViewport;
    stConversationTextViewport.top = 152;
    stConversationTextContext.viewport =
        &stConversationTextViewport;
    stConversationTextContext.text =
        szDefaultTextBuffer;
    stConversationTextContext.alignment = 2;
    InitializeTextContextFromFont(&stConversationTextContext,
                                  0,
                                  cViewportClearColour,
                                  cBlackColour);
    SetTextContext(&stConversationTextContext);
    return 0;
}

/* Function start: 0x427C30 */
unsigned int RefreshMemoryStatusOverlay(void)
{
    WaitForVerticalBlankThunk();
    CopyViewportContents(&stSceneBuffer, &stScreen);
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
    nSpaceFrame++;
    return 0;
}

/* Function start: 0x427C80 */
unsigned int TriggerPlayerHitPaletteFlash(void)
{
    if (nCameraViewMode <= 3)
        asDamageFlashColour[0] = 0x30;
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
    if (asDamageFlashColour[0] != 0) {
        switch ((int)(short)nVideoMode) {
        case 9:
        case 13:
            ClearViewport(&stSpaceBuffer, (short)cRedColour);
            bViewportDirty = 1;
            asDamageFlashColour[0] = 0;
            break;
        case 0x13:
            asDamageFlashColour[0] =
                (short)(asDamageFlashColour[0] - 4);
            SetPaletteEntry((short)cPrimaryViewBufferColour,
                            asDamageFlashColour);
            break;
        }
    }
    return 0;
}

/* Function start: 0x427D40 */
unsigned int house_keep(void)
{
    short palette;

    if (nCannedSceneMode == 0 &&
        nTrainSimActive == 0) {
        if ((nSpaceFrame & 0x1f) == 0)
            ReleaseStaleNavTarget();
        if (nHazardFieldCount != 0 &&
            (nSpaceFrame & 0xf) == 0)
            check_hazards();
    }
    if (nCameraViewMode == 0) {
        palette = 0;
        do {
            FadeFlightPaletteEntry(aPaletteFadeEntries[palette]);
            SetPaletteEntry((short)(palette + 0xb9),
                            aPaletteFadeEntries[palette]);
            palette++;
        } while (palette < 6);
        return 0;
    }
    if (nDamageAlarmSfxHandle != 0) {
        ((void (__cdecl *)(int, int))FlushSoundEffectsAndLog)(
            nDamageAlarmSfxHandle, 1);
        nDamageAlarmSfxHandle = 0;
        abCockpitLightGoal[3] = 0;
    }
    return 0;
}

/* Function start: 0x427DF0 */
void init_player_input(void)
{
    SetMousePosition(
        (stSpaceBuffer.right - stSpaceBuffer.left) / 2 + 1,
        nViewCenterY);
    ClearDebugPauseFlags();
    bMouseCursorVisible = 0;
    bPointerMovedByKeyboard = 1;
}

/* Function start: 0x427E40 */
void get_player_input(void)
{
    int device;
    InputDeviceSample *sample;

    if (nActiveInputDevice != -1 &&
        bInputPollingGuard == 0) {
        bInputPollingGuard++;
        UpdateInputDeviceTransitions(0);
        device = (int)nActiveInputDevice;
        sample = &aInputDeviceSamples[device];
        if (sample->x == 0 && sample->y == 0 && sample->buttons == 0) {
            if (sample->x != stLastPolledFlightInput.x ||
                sample->y != stLastPolledFlightInput.y ||
                sample->buttons !=
                    stLastPolledFlightInput.buttons) {
                TranslatePolledInputEvent(6, 0);
                device = (int)nActiveInputDevice;
                sample = &aInputDeviceSamples[device];
                bInputPollingGuard--;
                stLastPolledFlightInput = *sample;
                return;
            }
        } else {
            TranslatePolledInputEvent(6, 0);
            device = (int)nActiveInputDevice;
            sample = &aInputDeviceSamples[device];
            stLastPolledFlightInput = *sample;
        }
        bInputPollingGuard--;
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
    switch ((signed char)bCurrentKey) {
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
        keys[0] = (short)(signed char)bCurrentKey;
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
            bMouseCursorVisible = 0;
            if (nRollInput > 0) {
                nRollInput = 0;
            } else {
                if (shift != 0)
                    nRollInput = -9;
                if (nRollInput > -9)
                    nRollInput--;
                else if (cPreviousKey < 0)
                    nRollInput--;
                else
                    nRollInput++;
            }
            break;
        case 0x34:
        case 0x53:
            bMouseCursorVisible = 0;
            if (nRollInput < 0) {
                nRollInput = 0;
            } else {
                if (shift != 0)
                    nRollInput = 9;
                if (nRollInput < 9)
                    nRollInput++;
                else if (cPreviousKey < 0)
                    nRollInput++;
                else
                    nRollInput--;
            }
            break;
        case 0x48:
            bMouseCursorVisible = 0;
            if (nPitchInput < 0) {
                nPitchInput = 0;
            } else if (control == 0) {
                if (shift != 0)
                    nPitchInput = 9;
                if (nPitchInput < 9 ||
                    cPreviousKey < 0) {
                    bMouseCursorVisible = 0;
                    nPitchInput++;
                } else {
                    nPitchInput--;
                }
            } else {
                nSfxVolumeSetting++;
                if (nSfxVolumeSetting > 20)
                    nSfxVolumeSetting = 20;
                SaveVolumeSettingsToRegistry();
                SetSoundEffectsVolume(anVolumeLevels[
                    nSfxVolumeSetting / 2]);
                ShowOnScreenMessage(0, 0, "SFX VOLUME: %d.",
                                    nSfxVolumeSetting / 2);
            }
            break;
        case 0x4b:
            if (control != 0) {
                nMusicVolumeSetting--;
                if (nMusicVolumeSetting < 0)
                    nMusicVolumeSetting = 0;
                SaveVolumeSettingsToRegistry();
                SetMusicStreamVolume((unsigned short)
                    anVolumeLevels[
                        nMusicVolumeSetting / 2]);
                ShowOnScreenMessage(0, 0, "MUSIC VOLUME: %d.",
                                    nMusicVolumeSetting / 2);
                return;
            }
            bMouseCursorVisible = 0;
            if (nYawInput > 0) {
                nYawInput = 0;
            } else {
                if (shift != 0)
                    nYawInput = -9;
                if (nYawInput > -9)
                    nYawInput--;
                else if (cPreviousKey < 0)
                    nYawInput--;
                else
                    nYawInput++;
            }
            break;
        case 0x4c:
            WarpMouseTo(
                (short)((stSpaceBuffer.left +
                         stSpaceBuffer.right) / 2),
                (short)((stSpaceBuffer.top +
                         stSpaceBuffer.bottom) / 2));
            nRollInput = 0;
            nPitchInput = 0;
            nYawInput = 0;
            init_player_input();
            break;
        case 0x4d:
            if (control != 0) {
                nMusicVolumeSetting++;
                if (nMusicVolumeSetting > 20)
                    nMusicVolumeSetting = 20;
                SaveVolumeSettingsToRegistry();
                SetMusicStreamVolume((unsigned short)
                    anVolumeLevels[
                        nMusicVolumeSetting / 2]);
                ShowOnScreenMessage(0, 0, "MUSIC VOLUME: %d.",
                                    nMusicVolumeSetting / 2);
                return;
            }
            bMouseCursorVisible = 0;
            if (nYawInput < 0) {
                nYawInput = 0;
            } else {
                if (shift != 0)
                    nYawInput = 9;
                if (nYawInput < 9)
                    nYawInput++;
                else if (cPreviousKey < 0)
                    nYawInput++;
                else
                    nYawInput--;
            }
            break;
        case 0x50:
            bMouseCursorVisible = 0;
            if (nPitchInput > 0) {
                nPitchInput = 0;
            } else if (control == 0) {
                if (shift != 0)
                    nPitchInput = -9;
                if (nPitchInput > -9)
                    nPitchInput--;
                else if (cPreviousKey < 0)
                    nPitchInput--;
                else {
                    bMouseCursorVisible = 0;
                    nPitchInput++;
                }
            } else {
                nSfxVolumeSetting--;
                if (nSfxVolumeSetting < 0)
                    nSfxVolumeSetting = 0;
                SetSoundEffectsVolume(anVolumeLevels[
                    nSfxVolumeSetting / 2]);
                SaveVolumeSettingsToRegistry();
                ShowOnScreenMessage(0, 0, "SFX VOLUME: %d.",
                                    nSfxVolumeSetting / 2);
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
    if (asObjectCounter[0] == -1 &&
        asShipWeaponEnergy[0] > 0) {
        fire_fixed_projectile_weapon(0);
        if (acShipTarget[0] != -1 &&
            get_mode(1) == 5)
            SelectCockpitVduMode(1, 3);
    }
    return 0;
}

/* Function start: 0x4284D0 */
unsigned int players_flight_dynamics(void)
{
    ObjectTypeData *typeData;

    if (aeSpecialManeuver[0] ==
            SPECIAL_MANEUVER_BLOWING_UP) {
        if (asObjectCounter[0] == -1) {
            typeData = &aObjectTypeData[
                stCampaignState.playerShipType];
            if (anObjectYawRotation[0] < typeData->pitchRate &&
                anObjectPitchRotation[0] < typeData->yawRate &&
                anObjectRollRotation[0] < typeData->rollRate) {
                aeSpecialManeuver[0] = SPECIAL_MANEUVER_NONE;
            } else {
                anObjectYawRotation[0] -= nYawInput;
                anObjectPitchRotation[0] -= nPitchInput;
            }
        }
        return 0;
    }

    typeData = &aObjectTypeData[
        stCampaignState.playerShipType];
    anObjectPitchRotation[0] =
        (short)((typeData->yawRate * nPitchInput) / 8);
    anObjectYawRotation[0] =
        (short)-((typeData->pitchRate * nYawInput) / 8);
    anObjectRollRotation[0] =
        (short)-((typeData->rollRate * nRollInput) / 8);
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

    cPreviousKey = (signed char)bCurrentKey;
    nPreviousYawInput = nYawInput;
    nPreviousPitchInput = nPitchInput;
    nPreviousRollInput = nRollInput;
    keyboardRoll = 0;
    eventType = PollInputEvent(&event, 0xff);
    modifiers = event.modifiers;
    wCurrentInputModifiers = (unsigned short)modifiers;
    TranslatePolledInputEvent((unsigned short)eventType, event.value);
    bJoystickEventQueued = IsInputEventQueued(6);
    bMouseMoveEventQueued = IsInputEventQueued(13);
    queuedKeyEvent = IsInputEventQueued(5);
    queuedKeyEvent |= IsInputEventQueued(3);
    queuedKeyEvent |= IsInputEventQueued(4);
    bKeyboardEventQueued = queuedKeyEvent;
    bMouseButtonEventQueued = IsInputEventQueued(2);
    if (bMouseMoveEventQueued == 0)
        bCurrentKey |= 0x80;
    bCurrentKey |= 0x80;

    if (bMouseCursorVisible == 0) {
        key = PollKeyboardState();
        bCurrentKey = (unsigned char)key;
        if (bCurrentKey == 0) {
            nRollInput = 0;
            bFlightRollLatch = 0;
            nPitchInput = 0;
            nYawInput = 0;
        } else {
            bMouseAfterburnerControl = 0;
            process_player_input();
            switch (bCurrentKey) {
            case 0x33:
            case 0x34:
            case 0x52:
            case 0x53:
                keyboardRoll = 1;
            }
        }
    }

    if (bMouseButtonEventQueued == 0) {
        buttons = bHostSecondaryMouseButton * 2 |
                  bHostPrimaryMouseButton;
        if (buttons == 0) {
            bAfterburnerButtonLatched = 0;
        } else {
            if (buttons == 3) {
                bCurrentKey = 0x1c;
            } else if (buttons == 1) {
                bCurrentKey = 0x39;
                fire_players_lasers();
            }
            if ((buttons & 2) == 0)
                cPreviousKey = 0;
            if (cPreviousKey == 0x0f && buttons == 2)
                bCurrentKey = 0x0f;
            if (buttons == 1)
                fire_players_lasers();
        }
    }

    while ((eventType = GetNextInputEvent(&event)) != 0) {
        switch (eventType) {
        case 2:
            if ((short)event.value == 1) {
                bCurrentKey = 0x39;
                if ((event.modifiers & 4) != 0) {
                    if (aeSpecialManeuver[0] ==
                            SPECIAL_MANEUVER_AFTERBURNER)
                        fire_players_lasers();
                    else
                        bCurrentKey = 0x1c;
                }
            }
            if ((short)event.value == 3)
                bCurrentKey = 0x1c;
            if ((short)event.value == 2 &&
                bAfterburnerButtonLatched == 0) {
                if ((int)(nTickCount60Hz -
                        dwLastSecondaryButtonPress) <=
                        nInputTickScale)
                    bCurrentKey = 0x0f;
                bAfterburnerButtonLatched = 1;
            }
            if (cPreviousKey == 0x0f &&
                (short)event.value == 2)
                bCurrentKey = 0x0f;
            if ((short)event.value == 1)
                fire_players_lasers();
            dwLastSecondaryButtonPress = nTickCount60Hz;
            break;
        case 3:
        case 5:
            bMouseAfterburnerControl = 0;
            wCurrentInputModifiers =
                (unsigned short)event.modifiers;
            bCurrentKey = (unsigned char)event.value;
            process_player_input();
            break;
        case 6:
            bMouseAfterburnerControl = 0;
            bMouseCursorVisible = 0;
            if (((unsigned char)
                    stLastPolledFlightInput.buttons & 3) == 3) {
                if (aeSpecialManeuver[0] ==
                        SPECIAL_MANEUVER_AFTERBURNER)
                    fire_players_lasers();
                else
                    bCurrentKey = 0x1c;
            } else if ((stLastPolledFlightInput.buttons & 1) != 0) {
                fire_players_lasers();
            }
            buttons = (stLastPolledFlightInput.buttons & 2) >> 1;
            if (buttons != 0 &&
                aeSpecialManeuver[0] ==
                    SPECIAL_MANEUVER_AFTERBURNER)
                buttons = 0;
            if (buttons != 0) {
                nRollInput =
                    (short)stLastPolledFlightInput.x;
                accelerate((short)-(
                    stLastPolledFlightInput.y / 2));
            } else {
                if (nRollInput != 0 &&
                    bFlightRollLatch == 0 &&
                    keyboardRoll == 0) {
                    stPreviousFlightInput.x = -1;
                    nRollInput = 0;
                }
                if (stPreviousFlightInput.x !=
                        stLastPolledFlightInput.x ||
                    stPreviousFlightInput.y !=
                        stLastPolledFlightInput.y ||
                    stLastPolledFlightInput.x != 0 ||
                    stLastPolledFlightInput.y != 0) {
                    nPitchInput =
                        (short)-stLastPolledFlightInput.y;
                    nYawInput =
                        (short)stLastPolledFlightInput.x;
                }
            }
            if (asInputButton2DoubleClick[
                    nActiveInputDevice] != 0)
                bCurrentKey = 0x0f;
            if (cPreviousKey == 0x0f &&
                (stLastPolledFlightInput.buttons & 2) != 0)
                bCurrentKey = 0x0f;
            break;
        case 13:
            afterburnerControl =
                (unsigned short)(modifiers & 4) >= 1;
            if (afterburnerControl != 0 &&
                aeSpecialManeuver[0] ==
                    SPECIAL_MANEUVER_AFTERBURNER)
                afterburnerControl = 0;
            if (bMouseCursorVisible == 0) {
                bMouseCursorVisible = 1;
                stMouseCursorState.frame = 2;
            }
            if (bCockpitlessView == 0) {
                horizontal = (short)(event.x +
                    (stSpaceBuffer.left -
                     stSpaceBuffer.right) / 2 + 1);
                vertical = (short)(event.y +
                    (stSpaceBuffer.top - stSpaceBuffer.bottom) / 2);
            } else {
                if (cCockpitView == 0)
                    event.y = (short)(event.y - 10);
                else if (cCockpitView == 1)
                    event.y = (short)(event.y - 25);
                horizontal = (short)(event.x +
                    (stSpaceBuffer.left -
                     stSpaceBuffer.right) / 2 + 1);
                vertical = (short)(event.y - nViewCenterY);
            }
            stMouseCursorState.x = event.x;
            stMouseCursorState.y = event.y;
            for (yawInput = 0;
                 asMouseYawThresholds[yawInput] <=
                     abs((int)horizontal);
                 yawInput++)
                ;
            if (horizontal < 0)
                yawInput = (short)-yawInput;
            for (pitchInput = 0;
                 asMousePitchThresholds[pitchInput] <=
                     abs((int)vertical);
                 pitchInput++)
                ;
            if (vertical < 0)
                pitchInput = (short)-pitchInput;
            viewportLeft = (int)stSpaceBuffer.left;
            if ((int)event.x - 4 <= viewportLeft)
                yawInput = -8;
            if ((int)stSpaceBuffer.right <= (int)event.x + 4)
                yawInput = 8;
            if ((int)event.y - 4 <= (int)stSpaceBuffer.top)
                pitchInput = -8;
            if ((int)stSpaceBuffer.bottom <= (int)event.y + 4)
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
                bMouseAfterburnerControl = 1;
                pitchInput = (short)-pitchInput;
                nMouseYawInput = yawInput;
                nRollInput = yawInput;
                nMousePitchInput = pitchInput;
                accelerate((short)(pitchInput / 2));
            } else if (bMouseAfterburnerControl == 1) {
                nRollInput = 0;
                bMouseAfterburnerControl = 0;
                nMouseYawInput = 0;
                nYawInput = 0;
                nMousePitchInput = 0;
                nPitchInput = 0;
                WarpMouseTo(
                    (short)((viewportLeft + stSpaceBuffer.right) / 2),
                    (short)((stSpaceBuffer.bottom +
                             stSpaceBuffer.top) / 2));
            } else {
                nRollInput = 0;
                nMouseYawInput = yawInput;
                nYawInput = yawInput;
                nMousePitchInput = pitchInput;
                nPitchInput = pitchInput;
            }
            break;
        }
    }

#ifdef WC1_SDL
    Wc1SdlApplyJoystickFlightControls();
#endif
    stPreviousFlightInput =
        stLastPolledFlightInput;
    return 0;
}

/* Function start: 0x428C90 */
unsigned int SelectNextExternalViewObject(void)
{
    short object;

    object = (short)cViewObject;
    cViewObject = -1;
    do {
        object++;
        if (object > 9)
            object = 0;
        if (aeObjectClass[object] >= OBJECT_CLASS_SHIP)
            cViewObject = (signed char)object;
    } while (cViewObject == -1);
    return 0;
}

/* Function start: 0x428CD0 */
unsigned int SelectPreviousExternalViewObject(void)
{
    short object;

    object = (short)cViewObject;
    cViewObject = -1;
    do {
        object--;
        if (object < 0)
            object = 9;
        if (aeObjectClass[object] >= OBJECT_CLASS_SHIP)
            cViewObject = (signed char)object;
    } while (cViewObject == -1);
    return 0;
}

/* Function start: 0x428D10 */
unsigned int HandleFleetOverviewInput(void)
{
    signed char key;

    key = (signed char)bCurrentKey;
    if (nCameraViewMode != 8)
        return 0;

    bCurrentKey = 0;
    switch (key) {
    case 0x1c:
        cViewObject--;
        bCurrentKey = 0x29;
        break;
    case 0x47:
        nCapitalShipViewDistance -= 0x3200;
        break;
    case 0x48:
        rotate_about_i(-7,
                       &aShipUpVector[WC1_EYE_OBJECT],
                       &aShipForwardVector[WC1_EYE_OBJECT]);
        break;
    case 0x4b:
        rotate_about_j(7,
                       &aShipRightVector[WC1_EYE_OBJECT],
                       &aShipForwardVector[WC1_EYE_OBJECT]);
        break;
    case 0x4d:
        rotate_about_j(-7,
                       &aShipRightVector[WC1_EYE_OBJECT],
                       &aShipForwardVector[WC1_EYE_OBJECT]);
        break;
    case 0x4f:
        nCapitalShipViewDistance += 0x3200;
        break;
    case 0x50:
        rotate_about_i(7,
                       &aShipUpVector[WC1_EYE_OBJECT],
                       &aShipForwardVector[WC1_EYE_OBJECT]);
        break;
    case 0x52:
        aShipUpVector[WC1_EYE_OBJECT].z = -0x100;
        aShipForwardVector[WC1_EYE_OBJECT].y = 0x100;
        aShipRightVector[WC1_EYE_OBJECT].x = 0x100;
        aShipForwardVector[WC1_EYE_OBJECT].z = 0;
        aShipForwardVector[WC1_EYE_OBJECT].x = 0;
        aShipUpVector[WC1_EYE_OBJECT].y = 0;
        aShipUpVector[WC1_EYE_OBJECT].x = 0;
        aShipRightVector[WC1_EYE_OBJECT].z = 0;
        aShipRightVector[WC1_EYE_OBJECT].y = 0;
        break;
    default:
        bCurrentKey = (unsigned char)key;
        break;
    }
    return 0;
}
