/* WINGLEADER main game module: process services, flight control, and HUD.
 * The initialized state and complete literal order identify this group.
 * Its trailing cursor/palette helpers remain provisional; see docs/ORDER.md.
 */
#include <stdio.h>
#define WC1_MAIN_SOURCE
#ifndef SDL_PORT
#include <dos.h>
#endif
#include "wc1.h"

/* Legacy overlay setting: named in the WINGLEADER source screenshot and
 * retained, without references, before game mode/status in the Win32 image. */
unsigned int _ovrbuffer = 0x1370; /* 0x00469fa8 */
int nCannedSceneMode = 0; /* 0x00469fac */
int nArcadeState = 0; /* 0x00469fb0 */
short nFrameSkipCounter = 1; /* 0x00469fb4 */
short nFrameSkip = 1; /* 0x00469fb8 */
int nSfxVolumeSetting = 0x14; /* 0x00469fbc */
int nMusicVolumeSetting = 0x14; /* 0x00469fc0 */
int bViewportDirty = 0; /* 0x00469fc4 */
/* 0x00469fc8 */
#ifndef SDL_PORT
static
#endif
int anVolumeLevels[11] = {
    0, 40000, 50000, 55000, 60000, 61000,
    61500, 62000, 63000, 63500, 64000
};
int nOriginDevUnlock = 0; /* 0x00469ff4 */
int bPlayerCollisionsEnabled = 1; /* 0x00469ff8 */
int bPlayerVulnerable = 1; /* 0x00469ffc */
int bPlayerCollisionResponse = 1; /* 0x0046a000 */
int bViewBufferEnabled = 0; /* 0x0046a004 */
int bCockpitlessView = 0; /* 0x0046a008 */
static int nShowMemoryStatus = 0; /* 0x0046a00c */
short nStartNavPointOverride = -1; /* 0x0046a010 */
short nArcadeBonusCountdown = 0; /* 0x0046a014 */
int bMouseCursorVisible = 0; /* 0x0046a018 */
unsigned char bInputPollingGuard = 0; /* 0x0046a01c */
static InputDeviceSample stLastPolledFlightInput = {0}; /* 0x0046a020 */
#ifndef SDL_PORT
static
#endif
int bMouseAfterburnerControl = 0; /* 0x0046a02c */
/* 0x0046a030 */
static short asMouseYawThresholds[6] = {
    10, 37, 52, 57, 62, 1070
};
/* 0x0046a040 */
static short asMousePitchThresholds[6] = {
    5, 18, 27, 35, 38, 1040
};
static unsigned int dwLastSecondaryButtonPress = 0; /* 0x0046a04c */
#ifndef SDL_PORT
static
#endif
unsigned char bFlightRollLatch = 0; /* 0x0046a050 */
static unsigned char bAfterburnerButtonLatched = 0; /* 0x0046a054 */
#ifndef SDL_PORT
static
#endif
short nMouseYawInput = 0; /* 0x0046a058 */
#ifndef SDL_PORT
static
#endif
short nMousePitchInput = 0; /* 0x0046a05c */

/* Flight-loop performance-counter readings and their low-part deltas. CL
 * places these statics by identifier hash, so their spellings reproduce the
 * original order. */
static int nFlightFrameTicks; /* 0x004764f8 */
static int nHudDelta; /* 0x004764fc */
static LARGE_INTEGER liFlightAfterInputCounter; /* 0x00476500 */
static LARGE_INTEGER liFlightFrameEndCounter; /* 0x00476508 */
static int nFlightPresentDelta; /* 0x00476510 */
static LARGE_INTEGER liFrameStart; /* 0x00476518 */
static LARGE_INTEGER liSimulationDone; /* 0x00476520 */
static int nSimulationDelta; /* 0x00476528 */
static LARGE_INTEGER liFlightAfterCockpitCounter; /* 0x00476530 */
static int nInputTicks; /* 0x00476538 */
static LARGE_INTEGER liRenderDoneCounter; /* 0x00476540 */
static int nDrawDelta; /* 0x00476548 */

/* Linker commons defined here; LINK orders them by identifier hash. */
int nDmgDispState; /* 0x005a77e0 */
int nDamageDisplayPhase; /* 0x005a77e4 */
unsigned char *pLaunchDoorShape; /* 0x005a77e8 */
unsigned int nCurrentFrameTimerIdValue; /* 0x005a77ec */
unsigned char abPaletteTripletsArray[256][3]; /* 0x005a77f0 */
InputDeviceSample stPreviousFlightInput; /* 0x005a7af0 */
short bMouseButtonEventQueuedFlag; /* 0x005a7afc */
short bCurrentKeyboardEvtQueued; /* 0x005a7afe */
short bMouseMoveEvtQueued; /* 0x005a7b00 */
char *pCurrentStartupArgumentsPtr[30]; /* 0x005a7b10 */
short bJoystickEventQueued; /* 0x005a7b88 */
short nMainArcadeStartupParameterValue; /* 0x005a7b8a */
Viewport stCurrentTrainSimTitleDisplayViewport; /* 0x005a7b90 */

/* Function start: 0x4272F0 */
short LogMemoryUsage(void)
{
    ShutdownHook(0x21, (void *)nSavedDosInterruptVector);
    EMShutDown();
    GetMessagePumpResult();
    chdir("..");
    ShutdownVideoHook(3);
    if (nOriginDevUnlock != 0) {
        SystemDebugPrintf(
            "Original FMem %lu.  Current FMem %lu.  Current NMem %u.\n",
            dwOriginalFreeMem,
            GetFixedOneMillionThunkAlt(0),
            (unsigned int)(int)(short)GetOriginalFreeMemory());
    }
    MouseIdleHook();
    MessagePumpHook(8);
    return 0;
}

/* Function start: 0x427370 */
void exit_squadron(const char *msg)
{
    LogMemoryUsage();
    SystemDebugPrintf(msg);
    SystemDebugPrintf("[SYSTEM]: Exit_squadron\n");
    WriteDebugString("[SYSTEM] exit_squadron");
    WriteDebugString(msg);
    ClearDebugPauseFlags();
    AbortToDesktop();
    exit(0);
}

/* Function start: 0x4273C0 */
unsigned int ShowMemoryStatusDebug(void)
{
    TextContext savedContext;
    TextContext *previousContext;
    char value[60];

    previousContext = pCurTextContext;
    savedContext = stDefaultTxtContext;
    if (nShowMemoryStatus != 0) {
        InitializeTextContextFromFont(
            &stDefaultTxtContext, 1,
            (unsigned char)cViewportClearColour,
            cBlackColour);
        SetTextContext(&stDefaultTxtContext);
        DrawFormattedText("%X%YCurrent NMem %d.",
                          0, 176, (int)(short)GetOriginalFreeMemory());
        sprintf(value, "%ld", GetFixedOneMillionThunkAlt(0));
        DrawFormattedText("%X%YCurrent FMem %s.", 0, 184, value);
        sprintf(value, "%ld", dwOriginalFreeMem);
        DrawFormattedText("%X%YOriginal FMem %s.", 0, 0, value);
        stDefaultTxtContext = savedContext;
        pCurTextContext = previousContext;
    }
    return 0;
}

/* Function start: 0x4274C0 */
unsigned int GetJoystickButtonEdge(unsigned int a, short b)
{
    (void)a;
    if (b < 0)
        return JoystickEdgeHook(-1);
    return 0;
}

/* Function start: 0x4274E0 */
#ifdef SDL_PORT
int GameMain(short argc, char **argv)
#else
int main(short argc, char **argv)
#endif
{
    char **argumentSlot;
    char *argument;
    char *option;
    int animationDemo;
    int selection;
    int launchMission;
    short argumentIndex;
    short argumentCount;
    short animationIndex;
    short mission;
    short series;
    short gameFlowResult;

    mission = 0;
    launchMission = 0;
    series = 1;
    animationDemo = 0;

    GetShutdownErrorCode(abDiskPromptDriveStates);
    ExitCleanupHook();
    StartupHook(GetJoystickButtonEdge);
    nSavedDosInterruptVector = GetStartupErrorCode(0x21);
    ShutdownHook(0x21, DosInterruptHandlerHook);

    argumentCount = LoadWingCmdrCfgFile(argc, argv);
    chdir("gamedat");
    LoadInstallDat();
    chdir("..");
#ifdef SDL_PORT
    if (SdlUsingDosData())
        bCurrentRunningFromHardDiskFlag = 1;
#endif
    if (GetCurrentDiskDriveHook() > 'B')
        bCurrentRunningFromHardDiskFlag = 1;

    SystemDebugPrintf("Loading WING COMMANDER.  Please wait...\n");
#ifndef SDL_PORT
    if (sizeof(int) != sizeof(short *))
        SystemDebugPrintf("Sizeof int(enum) != sizeof short *\n");
#endif
    DAT_005a7d9c = 1;
    nMusicPlaybackMode = 4;
    bCurrentExpandedMemoryAvail = 1;
    ResetCampaignData();

    for (argumentIndex = 0;
         argumentIndex < argumentCount;
         argumentIndex++) {
        argumentSlot = &pCurrentStartupArgumentsPtr[argumentIndex];
        if (strcmp("Origin", (*argumentSlot)) == 0)
            nOriginDevUnlock = 1;

        argument = *argumentSlot;
        switch (argument[0]) {
        case '?':
            SystemDebugPrintf("Version %s.\n", pGameVersion);
        case '-': {
            option = *argumentSlot + 1;

            if (*option == 'm')
                nShowMemoryStatus = 1;
            if (nOriginDevUnlock != 0) {
                switch (*option) {
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
        }
        case 'A':
        case 'a': {
            option = argument + 1;

            if (*option == 's' || *option == 'S') {
                nStartNavPointOverride = (short)atoi(argument + 2);
            } else {
                nMusicPlaybackMode = 2;
                nMainArcadeStartupParameterValue =
                    (short)atoi(option);
            }
            break;
        }
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
                animationIndex = (short)atoi(argument + 1);
            }
            break;
        }
    }

    SetCinematicFrameTiming();
    stInitialCampaignState.currentSeries =
        stCampaignState.currentSeries = (signed char)series;
    stInitialCampaignState.currentMission =
        stCampaignState.currentMission = (signed char)mission;
    nVideoMode = 0x13;
    LoadOriginFxDrivers();
    nVideoMode = 0x13;

    if (animationDemo == 1) {
        SystemDebugPrintf("Pre animation: %lu.\n",
                          GetFixedOneMillionThunkAlt(0));
        WaitForKeyAcknowledge(0);
        RunAnimationDemoLoop((signed char)animationIndex);
        SystemDebugPrintf("Post animation: %lu.\n",
                          GetFixedOneMillionThunkAlt(0));
        WaitForKeyAcknowledge(0);
    }

    LoadVolumeSettingsFromRegistry();
    SetSoundEffectsVolume(
        anVolumeLevels[nSfxVolumeSetting / 2]);
    SetMusicStreamVolume((unsigned short)anVolumeLevels[
        nMusicVolumeSetting / 2]);

    if (launchMission == 0) {
#ifdef SDL_PORT
        SdlPlayDosStartupIntro();
#endif
        bEscapePressedFlag = 0;
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

#ifdef SDL_PORT
    if (init_mission(series, mission) != 0) {
        fprintf(stderr,
                "Mission SERIES=%d MISSION=%d is absent or invalid.\n",
                (int)series, (int)mission);
        return 1;
    }
#else
    init_mission(series, mission);
#endif
#ifdef SDL_PORT
    /* Use the normal campaign setup unless cockpitless mode was requested. */
    if (bCockpitEnabled != 0)
        LaunchPlayerShip();
#endif
    RunSpaceFlight(nStartNavPointOverride);
    exit_squadron("Bye!");
    return 0;
}

/* Function start: 0x4279D0 */
unsigned int GetScreenUpdateFlag(void)
{
    if (message_showing())
        EndCommMenu();
    if (stSpaceBufferViewport.pixels != 0)
        free_viewport(&stSpaceBufferViewport);
    return 0;
}

/* Function start: 0x427A00 */
unsigned int initialize_view_buffer(void)
{
    if (bViewBufferEnabled != 0 && stSpaceBufferViewport.pixels == 0) {
        if (AllocateViewport(&stSpaceBufferViewport,
                             (short)cPrimaryViewBufferColour,
                             0x20) == 0)
            ReportOutOfMemoryAndExit("SPACE BUFFER");
    }
    return 0;
}

/* Function start: 0x427A40 */
unsigned int dump_buffer_to_screen(void)
{
    short bottom;

#ifdef SDL_PORT
    SdlCompleteSpaceFrame();
#endif
    if (bCockpitlessView > 0) {
        CopyViewportContents(&stSpaceBufferViewport, &stScreenView);
        ShowMemoryStatusDebug();
        return 0;
    }
    switch ((int)cScreenViewMode) {
    case 4:
        bottom = stScreenView.bottom;
        stScreenView.top = 24;
        stScreenView.bottom = 152;
        CopyViewportContents(&stSpaceBufferViewport, &stScreenView);
        stScreenView.bottom = bottom;
        stScreenView.top = 0;
        break;
    case 5:
        CopyViewportContents(&stSpaceBufferViewport, &stScreenView);
        break;
    default:
        BlitViewportScanlineRuns(&stSpaceBufferViewport, &stScreenView,
                                 pScreenViewGeometry);
        break;
    }
    ShowMemoryStatusDebug();
    return 0;
}

/* Function start: 0x427B00 */
unsigned int clear_view_buffer(void)
{
    ClearViewport(&stSpaceBufferViewport,
                  (unsigned char)cPrimaryViewBufferColour);
    return 0;
}

/* Function start: 0x427B20 */
unsigned int InitializeConversationViewport(void)
{
    ClearViewport(&stModalSrcViewport, cBlackColour);
    stScreenView.top = 24;
    stScreenView.bottom = 151;
    stSceneBuffer.left = 0;
    stSceneBuffer.right = 319;
    stSceneBuffer.top = 0;
    stSceneBuffer.bottom = 127;
    if (AllocateViewport(&stSceneBuffer,
                         (short)cBlackColour, 0) == 0)
        ReportPacketLoadError(0, 0, 0, 0,
                              "AB");
    return 0;
}

/* Function start: 0x427BA0 */
unsigned int ResetScreenClipToFullHeight(void)
{
    free_viewport(&stSceneBuffer);
    stScreenView.top = 0;
    stScreenView.bottom = 199;
    return 0;
}

/* Function start: 0x427BC0 */
unsigned int InitializeConversationText(void)
{
    stCurrentConvTextViewport =
        stModalSrcViewport;
    stCurrentConvTextViewport.top = 152;
    stCurrentConvTxtCtx.viewport =
        &stCurrentConvTextViewport;
    stCurrentConvTxtCtx.text =
        szDefaultTxtBuf;
    stCurrentConvTxtCtx.alignment = 2;
    InitializeTextContextFromFont(&stCurrentConvTxtCtx,
                                  0,
                                  cViewportClearColour,
                                  cBlackColour);
    SetTextContext(&stCurrentConvTxtCtx);
    return 0;
}

/* Function start: 0x427C30 */
unsigned int RefreshMemoryStatusOverlay(void)
{
    WaitForVerticalBlankThunk();
    CopyViewportContents(&stSceneBuffer, &stScreenView);
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
    nCurrentSpaceFrameValue++;
    return 0;
}

/* Function start: 0x427C80 */
unsigned int TriggerPlayerHitPaletteFlash(void)
{
#ifdef SDL_PORT
    /* SWC CODE_09 glare_players_view +0x00f0: impacts cannot interrupt
       the longer explosion glare. WC1's palette remains unchanged. */
    if (SdlSwcFlightActive()) {
        if (nCameraViewMode <= 3 && !bSwcExplosionGlare)
            nSwcSpaceGlareTicks = 4;
        return 0;
    }
#endif
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
        switch (nVideoMode) {
        case 9:
        case 13:
            ClearViewport(&stSpaceBufferViewport, (short)cRedColour);
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
    short count;

    if (nCannedSceneMode == 0 &&
        nTrainSimActive == 0) {
        count = nCurrentSpaceFrameValue;
        if ((count & 0x1f) == 0) {
            ReleaseStaleNavTarget();
            count = nCurrentSpaceFrameValue;
        }
        if (nCurrentHazardFieldTotal != 0 &&
            (count & 0xf) == 0)
            check_hazards();
    }
    if (nCameraViewMode == 0) {
        count = 0;
        do {
            FadeFlightPaletteEntry(aPaletteFadeEntries[count]);
            SetPaletteEntry((short)(count + 0xb9),
                            aPaletteFadeEntries[count]);
            count++;
        } while (count < 6);
        return 0;
    }
    if (nCurrentDamageAlarmSfxHandle != 0) {
        FlushSoundEffectsAndLog(
            nCurrentDamageAlarmSfxHandle, 1);
        nCurrentDamageAlarmSfxHandle = 0;
        abCockpitLightGoalArray[3] = 0;
    }
    return 0;
}

/* Function start: 0x427DF0 */
void init_player_input(void)
{
    SetMousePosition(
        (stSpaceBufferViewport.right - stSpaceBufferViewport.left) / 2 + 1,
        nCurrentViewCenterPosY);
    ClearDebugPauseFlags();
    bMouseCursorVisible = 0;
    bPtrMovedByKeyboardState = 1;
}

/* Function start: 0x427E40 */
void get_player_input(void)
{
    int device;

    if (nCurrentActiveInputDevice != -1 &&
        bInputPollingGuard == 0) {
        bInputPollingGuard++;
        UpdateInputDeviceTransitions(0);
        device = (int)nCurrentActiveInputDevice;
        if (aInputDevSamples[device].x == 0 &&
            aInputDevSamples[device].y == 0 &&
            aInputDevSamples[device].buttons == 0) {
            if (aInputDevSamples[device].x != stLastPolledFlightInput.x ||
                aInputDevSamples[device].y != stLastPolledFlightInput.y ||
                aInputDevSamples[device].buttons !=
                    stLastPolledFlightInput.buttons) {
                TranslatePolledInputEvent(6, 0);
                stLastPolledFlightInput = aInputDevSamples[nCurrentActiveInputDevice];
                bInputPollingGuard--;
                return;
            }
        } else {
            TranslatePolledInputEvent(6, 0);
            stLastPolledFlightInput = aInputDevSamples[nCurrentActiveInputDevice];
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
    int shift;
    short *key;
    int control;
    short finished;
    short handled;

    handled = 1;
    finished = 0;
    shift = GetShiftKeyState();
    control = GetControlKeyState();
    key = keys;
    switch (bCurrentKey) {
    case 0x47:
        *key++ = 0x48;
        *key++ = 0x4b;
        break;
    case 0x49:
        *key++ = 0x48;
        *key++ = 0x4d;
        break;
    case 0x4f:
        *key++ = 0x50;
        *key++ = 0x4b;
        break;
    case 0x51:
        *key++ = 0x50;
        *key++ = 0x4d;
        break;
    default:
        *key++ = (short)bCurrentKey;
        break;
    }
    *key = -1;

    key = keys;
    while (finished == 0) {
        switch (*key++) {
        default:
            handled--;
            break;
        case 0x33:
        case 0x52:
            bMouseCursorVisible = 0;
            if (nCurrentRollInputValue > 0) {
                nCurrentRollInputValue = 0;
            } else {
                if (shift != 0)
                    nCurrentRollInputValue = -9;
                if (nCurrentRollInputValue > -9)
                    nCurrentRollInputValue--;
                else if (cPreviousKey < 0)
                    nCurrentRollInputValue--;
                else
                    nCurrentRollInputValue++;
            }
            break;
        case 0x34:
        case 0x53:
            bMouseCursorVisible = 0;
            if (nCurrentRollInputValue < 0) {
                nCurrentRollInputValue = 0;
            } else {
                if (shift != 0)
                    nCurrentRollInputValue = 9;
                if (nCurrentRollInputValue < 9)
                    nCurrentRollInputValue++;
                else if (cPreviousKey < 0)
                    nCurrentRollInputValue++;
                else
                    nCurrentRollInputValue--;
            }
            break;
        case 0x48:
            bMouseCursorVisible = 0;
            if (nPitchInput < 0) {
                nPitchInput = 0;
            } else if (control != 0) {
                nSfxVolumeSetting++;
                if (nSfxVolumeSetting > 20)
                    nSfxVolumeSetting = 20;
                SaveVolumeSettingsToRegistry();
                SetSoundEffectsVolume(anVolumeLevels[
                    nSfxVolumeSetting / 2]);
                ShowOnScreenMessage(0, 0, "SFX VOLUME: %d.",
                                    nSfxVolumeSetting / 2);
            } else {
                if (shift != 0)
                    nPitchInput = 9;
                if (nPitchInput < 9)
                    nPitchInput++;
                else if (cPreviousKey < 0)
                    nPitchInput++;
                else
                    nPitchInput--;
            }
            break;
        case 0x50:
            bMouseCursorVisible = 0;
            if (nPitchInput > 0) {
                nPitchInput = 0;
            } else if (control != 0) {
                nSfxVolumeSetting--;
                if (nSfxVolumeSetting < 0)
                    nSfxVolumeSetting = 0;
                SetSoundEffectsVolume(anVolumeLevels[
                    nSfxVolumeSetting / 2]);
                SaveVolumeSettingsToRegistry();
                ShowOnScreenMessage(0, 0, "SFX VOLUME: %d.",
                                    nSfxVolumeSetting / 2);
            } else {
                if (shift != 0)
                    nPitchInput = -9;
                if (nPitchInput > -9)
                    nPitchInput--;
                else if (cPreviousKey < 0)
                    nPitchInput--;
                else
                    nPitchInput++;
            }
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
            if (nYawInputValue < 0) {
                nYawInputValue = 0;
            } else {
                if (shift != 0)
                    nYawInputValue = 9;
                if (nYawInputValue < 9)
                    nYawInputValue++;
                else if (cPreviousKey < 0)
                    nYawInputValue++;
                else
                    nYawInputValue--;
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
            if (nYawInputValue > 0) {
                nYawInputValue = 0;
            } else {
                if (shift != 0)
                    nYawInputValue = -9;
                if (nYawInputValue > -9)
                    nYawInputValue--;
                else if (cPreviousKey < 0)
                    nYawInputValue--;
                else
                    nYawInputValue++;
            }
            break;
        case 0x4c:
            WarpMouseTo(
                (short)((stSpaceBufferViewport.left +
                         stSpaceBufferViewport.right) / 2),
                (short)((stSpaceBufferViewport.top +
                         stSpaceBufferViewport.bottom) / 2));
            nCurrentRollInputValue = 0;
            nPitchInput = 0;
            nYawInputValue = 0;
            init_player_input();
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
    if (asObjectCounterArray[0] == -1 &&
        asShipWpnEnergyList[0] > 0) {
        fire_fixed_projectile_weapon(0);
        if (acShipTargetTable[0] != -1 &&
            get_mode(1) == 5)
            SelectCockpitVduMode(1, 3);
    }
    return 0;
}

/* Function start: 0x4284D0 */
unsigned int players_flight_dynamics(void)
{
    enum ObjectType type;

    if (aeSpecialManeuvers[0] ==
            SPECIAL_MANEUVER_BLOWING_UP) {
        if (asObjectCounterArray[0] == -1) {
            type = stCampaignState.playerShipType;
            if (anObjectYawRotation[0] < aObjectTypeData[type].pitchRate &&
                anObjPitchRot[0] < aObjectTypeData[type].yawRate &&
                anObjectRollRotationList[0] < aObjectTypeData[type].rollRate) {
                aeSpecialManeuvers[0] = SPECIAL_MANEUVER_NONE;
            } else {
                anObjectYawRotation[0] -= nYawInputValue;
                anObjPitchRot[0] -= nPitchInput;
            }
        }
        return 0;
    }

    type = stCampaignState.playerShipType;
    anObjPitchRot[0] =
        (short)((aObjectTypeData[type].yawRate * nPitchInput) / 8);
    anObjectYawRotation[0] =
        (short)-((aObjectTypeData[type].pitchRate * nYawInputValue) / 8);
    anObjectRollRotationList[0] =
        (short)-((aObjectTypeData[type].rollRate * nCurrentRollInputValue) / 8);
    return 0;
}

/* Function start: 0x4285D0 */
unsigned int player_input(void)
{
#ifdef SDL_PORT
    /* Motion/empty polls do not populate every field of the legacy event. */
    InputEventState event = {0};
#else
    InputEventState event;
#endif
    short modifiers;
    short eventType;
    short queuedKeyEvent;
    short horizontal;
    short vertical;
    unsigned short horizontalMagnitude;
    unsigned short verticalMagnitude;
    short yawInput;
    short pitchInput;
    int keyboardRoll;
    int viewportLeft;
    int mouseX;
    int afterburnerControl;
    unsigned int buttons;
    unsigned int key;

    cPreviousKey = bCurrentKey;
    *(volatile short *)&nPrevYawInput = nYawInputValue;
    memcpy(&nPrevPitchInputValue, &nPitchInput, sizeof(nPrevPitchInputValue));
    memcpy(&nPreviousRollInputValue, &nCurrentRollInputValue,
           sizeof(nPreviousRollInputValue));
    keyboardRoll = 0;
    eventType = PollInputEvent(&event, 0xff);
    wCurrentInputModifiers = (unsigned short)event.modifiers;
    modifiers = event.modifiers;
    TranslatePolledInputEvent((unsigned short)eventType, event.value);
    bJoystickEventQueued = IsInputEventQueued(6);
    bMouseMoveEvtQueued = IsInputEventQueued(13);
    queuedKeyEvent = IsInputEventQueued(5);
    queuedKeyEvent |= IsInputEventQueued(3);
    queuedKeyEvent |= IsInputEventQueued(4);
    bCurrentKeyboardEvtQueued = queuedKeyEvent;
    bMouseButtonEventQueuedFlag = IsInputEventQueued(2);
    if (bMouseMoveEvtQueued == 0)
        bCurrentKey |= 0x80;
    bCurrentKey |= 0x80;

    if (bMouseCursorVisible == 0) {
        key = PollKeyboardState();
        bCurrentKey = (signed char)key;
        if (bCurrentKey == 0) {
            nCurrentRollInputValue = 0;
            bFlightRollLatch = 0;
            nPitchInput = 0;
            nYawInputValue = 0;
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

    if (bMouseButtonEventQueuedFlag == 0) {
        buttons = bHostSecondaryMouseButton * 2 |
                  bHostPrimaryMouseButton;
        if (buttons != 0) {
            if (buttons == 3) {
                bCurrentKey = 0x1c;
            } else if (buttons == 1) {
                bCurrentKey = 0x39;
                fire_players_lasers();
            }
            if ((buttons & 2) == 0)
                cPreviousKey = 0;
            if (cPreviousKey == 0x0f && buttons == 2) {
                bCurrentKey = 0x0f;
                if (buttons == 1)
                    fire_players_lasers();
            }
        } else {
            bAfterburnerButtonLatched = 0;
        }
    }

    while ((eventType = GetNextInputEvent(&event)) != 0) {
        switch (eventType) {
        case 2:
            if ((short)event.value == 1) {
                bCurrentKey = 0x39;
                if ((event.modifiers & 4) != 0) {
                    if (aeSpecialManeuvers[0] ==
                            SPECIAL_MANEUVER_AFTERBURNER)
                        fire_players_lasers();
                    else
                        bCurrentKey = 0x1c;
                }
            }
            if ((short)event.value == 3)
                bCurrentKey = 0x1c;
            if ((short)event.value == 2 &&
                *(volatile unsigned char *)&bAfterburnerButtonLatched == 0) {
                if ((int)(nTickCount60Hz -
                        dwLastSecondaryButtonPress) <=
                        nInputTickScale)
                    bCurrentKey = 0x0f;
                bAfterburnerButtonLatched = 1;
            }
            if (cPreviousKey == 0x0f &&
                (short)event.value == 2) {
                bCurrentKey = 0x0f;
                if ((short)event.value == 1)
                    fire_players_lasers();
            }
            dwLastSecondaryButtonPress = nTickCount60Hz;
            break;
        case 3:
        case 5:
            bMouseAfterburnerControl = 0;
            bCurrentKey = (signed char)event.value;
            process_player_input();
            break;
        case 6:
            bMouseAfterburnerControl = 0;
            bMouseCursorVisible = 0;
            if (((unsigned char)
                    stLastPolledFlightInput.buttons & 3) == 3) {
                if (aeSpecialManeuvers[0] ==
                        SPECIAL_MANEUVER_AFTERBURNER)
                    fire_players_lasers();
                else
                    bCurrentKey = 0x1c;
            } else if ((stLastPolledFlightInput.buttons & 1) != 0) {
                fire_players_lasers();
            }
            buttons = (stLastPolledFlightInput.buttons & 2) >> 1;
            if (buttons != 0 &&
                aeSpecialManeuvers[0] ==
                    SPECIAL_MANEUVER_AFTERBURNER)
                buttons = 0;
            if (buttons != 0) {
                *(volatile short *)&nCurrentRollInputValue =
                    (short)stLastPolledFlightInput.x;
                accelerate((short)-(
                    stLastPolledFlightInput.y / 2));
            } else {
                if (nCurrentRollInputValue != 0 &&
                    bFlightRollLatch == 0 &&
                    keyboardRoll == 0) {
                    stPreviousFlightInput.x = -1;
                    nCurrentRollInputValue = 0;
                }
                if (stPreviousFlightInput.x !=
                        stLastPolledFlightInput.x ||
                    stPreviousFlightInput.y !=
                        stLastPolledFlightInput.y ||
                    stLastPolledFlightInput.x != 0 ||
                    stLastPolledFlightInput.y != 0) {
                    *(volatile short *)&nYawInputValue =
                        (short)stLastPolledFlightInput.x;
                    nPitchInput =
                        (short)-stLastPolledFlightInput.y;
                }
            }
            if (asInputButton2DoubleClick[
                    nCurrentActiveInputDevice] != 0)
                bCurrentKey = 0x0f;
            if (cPreviousKey == 0x0f &&
                (stLastPolledFlightInput.buttons & 2) != 0)
                bCurrentKey = 0x0f;
            break;
        case 13:
            afterburnerControl =
                (unsigned short)(modifiers & 4) >= 1;
            if (afterburnerControl != 0 &&
                aeSpecialManeuvers[0] ==
                    SPECIAL_MANEUVER_AFTERBURNER)
                afterburnerControl = 0;
            if (*(volatile int *)&bMouseCursorVisible == 0) {
                bMouseCursorVisible = 1;
                stMouseCursorState.frame = 2;
            }
            if (*(volatile int *)&bCockpitlessView != 0) {
                switch (cCurrentCockpitView) {
                case 0:
                    event.y = (short)(event.y - 10);
                    break;
                case 1:
                    event.y = (short)(event.y - 25);
                    break;
                }
                horizontal = (short)(event.x +
                    (stSpaceBufferViewport.left -
                     stSpaceBufferViewport.right) / 2 + 1);
                vertical = (short)(event.y -
                    *(volatile short *)&nCurrentViewCenterPosY);
            } else {
                horizontal = (short)(event.x +
                    (stSpaceBufferViewport.left -
                     stSpaceBufferViewport.right) / 2 + 1);
                vertical = (short)(event.y +
                    (stSpaceBufferViewport.top - stSpaceBufferViewport.bottom) / 2);
            }
            stMouseCursorState.x = event.x;
            stMouseCursorState.y = event.y;
            horizontalMagnitude = (short)abs((int)horizontal);
            verticalMagnitude = (short)abs((int)vertical);
            for (yawInput = 0;
                 asMouseYawThresholds[yawInput] <=
                     (short)horizontalMagnitude;
                 yawInput++)
                ;
            if (horizontal < 0)
                yawInput = (short)-yawInput;
            for (pitchInput = 0;
                 asMousePitchThresholds[pitchInput] <=
                     (short)verticalMagnitude;
                 pitchInput++)
                ;
            if (vertical < 0)
                pitchInput = (short)-pitchInput;
            mouseX = event.x;
            viewportLeft = (int)stSpaceBufferViewport.left;
            if (mouseX - 4 <= viewportLeft)
                yawInput = -8;
            if ((int)stSpaceBufferViewport.right <= mouseX + 4)
                yawInput = 8;
            if ((int)event.y - 4 <= (int)stSpaceBufferViewport.top)
                pitchInput = -8;
            if ((int)stSpaceBufferViewport.bottom <= (int)event.y + 4)
                pitchInput = 8;
            if (yawInput >= 9)
                yawInput = 8;
            if (yawInput <= -9)
                yawInput = -8;
            if (pitchInput >= 9)
                pitchInput = 8;
            if (pitchInput <= -9)
                pitchInput = -8;
            if (afterburnerControl != 0) {
                bMouseAfterburnerControl = 1;
                pitchInput = (short)-pitchInput;
                nMouseYawInput = yawInput;
                nCurrentRollInputValue = yawInput;
                nMousePitchInput = pitchInput;
                accelerate((short)(pitchInput / 2));
            } else if (bMouseAfterburnerControl == 1) {
                nCurrentRollInputValue = 0;
                bMouseAfterburnerControl = 0;
                nMouseYawInput = 0;
                nYawInputValue = 0;
                nMousePitchInput = 0;
                nPitchInput = 0;
                WarpMouseTo(
                    (short)((viewportLeft + stSpaceBufferViewport.right) / 2),
                    (short)((stSpaceBufferViewport.bottom +
                             stSpaceBufferViewport.top) / 2));
            } else {
                nCurrentRollInputValue = 0;
                nMouseYawInput = yawInput;
                nYawInputValue = yawInput;
                nMousePitchInput = pitchInput;
                nPitchInput = pitchInput;
            }
            break;
        }
    }

#ifdef SDL_PORT
    SdlApplyJoystickFlightControls();
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

/* Fleet-overview, modal-message, and HUD-text callees from other modules. */
void rotate_about_i(short angle, FixedVector *j,
                    FixedVector *k);
void rotate_about_j(short angle, FixedVector *i,
                    FixedVector *k);
short MinShort(short a, short b);
short __stdcall DosStrlen(const char *s);
short ShowModalTextPanel(short fontIndex, const char *format, ...);
void FatalErrorAndExit(const char *format, ...);
void SetHudMessageText(char *text, unsigned short colour,
                       unsigned short duration);
void ShowHudTextLine(char *s, unsigned short b);

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
    short key;

    key = bCurrentKey;
    if (nCameraViewMode == 8) {
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
                           &aShipUpVector[EYE_OBJECT],
                           &aShipFwdVec[EYE_OBJECT]);
            break;
        case 0x4b:
            rotate_about_j(7,
                           &aShipRightVector[EYE_OBJECT],
                           &aShipFwdVec[EYE_OBJECT]);
            break;
        case 0x4d:
            rotate_about_j(-7,
                           &aShipRightVector[EYE_OBJECT],
                           &aShipFwdVec[EYE_OBJECT]);
            break;
        case 0x4f:
            nCapitalShipViewDistance += 0x3200;
            break;
        case 0x50:
            rotate_about_i(7,
                           &aShipUpVector[EYE_OBJECT],
                           &aShipFwdVec[EYE_OBJECT]);
            break;
        case 0x52:
            aShipUpVector[EYE_OBJECT].z = -0x100;
            aShipFwdVec[EYE_OBJECT].y = 0x100;
            aShipRightVector[EYE_OBJECT].x = 0x100;
            aShipFwdVec[EYE_OBJECT].z = 0;
            aShipFwdVec[EYE_OBJECT].x = 0;
            aShipUpVector[EYE_OBJECT].y = 0;
            aShipUpVector[EYE_OBJECT].x = 0;
            aShipRightVector[EYE_OBJECT].z = 0;
            aShipRightVector[EYE_OBJECT].y = 0;
            break;
        default:
            bCurrentKey = (signed char)key;
            break;
        }
    }
    return 0;
}

/* Function start: 0x428E70 */
/* Message dwell time: grows with text length, scaled by the speed setting. */
short MeasureMessageWidth(const char *text)
{
    return (short)((MinShort(5, (short)(DosStrlen(text) >> 1)) + 5) *
                   ((char)bMessageSpeed + 1));
}

/* Function start: 0x428EA0 */
void WaitForKeyAcknowledge(int mode)
{
    int acknowledged;
    int key;

#ifdef SDL_PORT
    /* Every modal wait blocks here, including both pause paths: Ctrl+P
     * calls this directly and P reaches it through ShowOnScreenMessage's
     * 9999 duration.  Free the pointer for the duration of the wait. */
    SdlSuspendMouseGrab();
#endif
    if (mode != 0) {
        acknowledged = 0;
        do {
            PumpWindowMessages();
            if (IsInputEventQueued(4) != 0)
                acknowledged = 1;
        } while (acknowledged == 0);
        acknowledged = 0;
        FlushInputEvents();
        ClearDebugPauseFlags();
        do {
            PumpWindowMessages();
            if (IsInputEventQueued(3) != 0)
                acknowledged = 1;
        } while (acknowledged == 0);
        FlushInputEvents();
        ClearDebugPauseFlags();
#ifdef SDL_PORT
        SdlResumeMouseGrab();
#endif
        return;
    }
    FlushInputEvents();
    ClearDebugPauseFlags();
    do {
        key = PumpMessagesDuringWait();
    } while (key == 0x19 || key == 0x50 || key == 0x0c);
    FlushInputEvents();
#ifdef SDL_PORT
    SdlResumeMouseGrab();
#endif
}

/* Function start: 0x428F20 */
void ShowModalMessage(const char *format, ...)
{
    char text[52];

#ifdef SDL_PORT
    va_list arguments;

    va_start(arguments, format);
    vsprintf(text, format, arguments);
    va_end(arguments);
#else
    vsprintf(text, format, (char *)(&format + 1));
#endif
    if (ShowModalTextPanel(1, text) != 0) {
        WaitForKeyAcknowledge(0);
        ReleaseModalTextPanel();
        return;
    }
    SystemDebugPrintf(text);
    WaitForKeyAcknowledge(0);
}

/* Function start: 0x428F80 */
void ReportOutOfMemoryAndExit(const char *resource)
{
    ShowModalMessage("ERROR: Out of memory for %s", resource);
    FatalErrorAndExit(szInsufficientMemory);
}

/* Function start: 0x428FA0 */
void ShowOnScreenMessage(int flags, short duration,
                         const char *format, ...)
{
    short messageDuration;
    short modalShown;
    char text[52];

#ifdef SDL_PORT
    va_list arguments;

    va_start(arguments, format);
    vsprintf(text, format, arguments);
    va_end(arguments);
#else
    vsprintf(text, format, (char *)(&format + 1));
#endif
    modalShown = 0;
    FlushInputEvents();
    messageDuration = duration;
    if (messageDuration == 9999)
        modalShown = ShowModalTextPanel(1, text);
    if (modalShown == 0) {
        if (messageDuration == 0)
            messageDuration = MeasureMessageWidth(text);
        SetHudTextColour(1);
        DosStrcpy(szHudMessageBuf, text);
        SetHudMessageText(szHudMessageBuf,
                          cRedColour, messageDuration);
        if (messageDuration == 9999) {
            ShowHudTextLine(szHudMessageBuf,
                            (unsigned short)cRedColour);
            dump_buffer_to_screen();
        }
    }
    if (messageDuration == 9999) {
        if (flags != 0)
            WaitForKeyAcknowledge(1);
        else
            WaitForKeyAcknowledge(0);
    }
    if (modalShown != 0) {
        ReleaseModalTextPanel();
        return;
    }
    if (messageDuration == 9999)
        SetHudMessageText("", cRedColour, 2);
}

/* Function start: 0x4290A0 */
void ShowGamePausedBanner(short showBanner)
{
    if (showBanner != 0) {
        ShowOnScreenMessage(1, 9999, "GAME PAUSED");
        return;
    }
    WaitForKeyAcknowledge(1);
}

/* Function start: 0x4290D0 */
void ShowVersionBanner(void)
{
    ShowOnScreenMessage(1, 9999, "WING COMMANDER VER. %s",
                        pGameVersion);
}

/* Function start: 0x4290F0 */
void SetMessageDisplaySpeed(void)
{
    bMessageSpeed =
        (unsigned char)(((signed char)bMessageSpeed + 1) % 5);
    ShowOnScreenMessage(0, 0, "MESSAGES SPEED IS NOW %d.",
                        (signed char)bMessageSpeed + 1);
}

/* Function start: 0x429120 */
void ReportFramesSkipped(short adjustment)
{
    nFrameSkip = MinShort(
        MaxShort((short)(nFrameSkip + adjustment), 1), 5);
    ShowOnScreenMessage(0, 0, "%d FRAMES SKIPPED.",
                        nFrameSkip - 1);
}

/* Function start: 0x429160 */
int HandleSpaceFlightControls(void)
{
    int notRepeated;
    int control;
    int selectedWeapon;
    short wingman;

    player_input();
    players_flight_dynamics();
    notRepeated = bCurrentKey != cPreviousKey;
    control = GetControlKeyState();
    GetKeyboardModifiers();
    HandleFleetOverviewInput();

    if (nTrainSimActive == 0) {
        switch (bCurrentKey) {
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            if (notRepeated && get_mode(1) == 4 &&
                nCameraViewMode == 0 &&
                bCurrentKey >= 2 &&
                bCurrentKey <=
                    nCommMenuChoiceCount + 2 &&
                get_mode(1) == 4) {
                Chosen_communicate_option(
                    (short)(bCurrentKey - 2));
            }
            break;
        case 0x12:
            if (notRepeated &&
#ifdef SDL_PORT
                (control != 0 ||
                 (wCurrentInputModifiers & 0x2000) != 0) &&
#else
                control != 0 &&
#endif
                acPlayerComponentDamageList[7] != 4) {
                if (RandomInRange(
                        0, acPlayerComponentDamageList[7]) == 0)
                    nArcadeState = 2;
                else
                    malf_sound();
            }
            break;
        case 0x1e:
            if (notRepeated) {
                bMouseCursorVisible = 0;
                if (get_mode(1) != 5)
                    SelectCockpitVduMode(1, 5);
                if (bCockpitlessView == 0) {
                    auto_pilot_sequence();
                } else {
                    GetScreenUpdateFlag();
                    SetViewportRect(
                        &stSpaceBufferViewport, 0, 0,
                        (short)(nScreenWidth - 1),
                        (short)(nScreenHeight - 1));
                    initialize_view_buffer();
                    bCockpitlessView = -2;
                    auto_pilot_sequence();
                    bCockpitlessView = 1;
                    GetScreenUpdateFlag();
                    SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
                    initialize_view_buffer();
                }
                FlushInputEvents();
                ClearDebugPauseFlags();
                bMouseCursorVisible = 0;
                bPtrMovedByKeyboardState = 1;
            }
            break;
        case 0x1f:
            if (control != 0 && notRepeated) {
                if (nFlightSoundEffectsEnabled == 1)
                    ResetSoundStateForScene();
                else
                    ResetSoundStateForFlight();
            }
            break;
        case 0x23:
            if (notRepeated && (wingman = *(volatile short *)&nYourWingman) != -1 &&
                aeShipObjectiveList[wingman] !=
                    OBJECTIVE_HOLD_FORMATION) {
                request(0, wingman, 9);
            }
            break;
        case 0x2e:
            if (notRepeated) {
                if (message_showing() != 0) {
                    EndCommMenu();
                } else {
                    if (get_mode(1) != 4)
                        SelectCockpitVduMode(1, 4);
                    else
                        CloseCommChoiceMenu();
                }
            }
            break;
        case 0x2f:
            if (notRepeated && control == 0) {
                bVideoImagesSuppressed =
                    bVideoImagesSuppressed == 0;
                if (bVideoImagesSuppressed != 0)
                    SetHudMessageText("VIDEO IMAGES SUPRESSED",
                                      cRedColour, 20);
                else
                    SetHudMessageText("VIDEO IMAGES ENABLED",
                                      cPrimaryTextColour, 20);
            }
            break;
        case 0x30:
            if (notRepeated && (wingman = *(volatile short *)&nYourWingman) != -1 &&
                aeShipObjectiveList[wingman] ==
                    OBJECTIVE_HOLD_FORMATION &&
                any_enemy(0, 14000) != 0) {
                request(0, nYourWingman, 7);
            }
            break;
        case 0x31:
            if (notRepeated)
                SelectCockpitVduMode(1, 5);
            init_player_input();
            break;
        case 0x32:
            if (notRepeated && control == 0)
                SetMessageDisplaySpeed();
            break;
        case 0x3b:
            if (GetF1KeyLatch() != 0) {
                if (cScreenViewMode == 0) {
                    bCockpitlessView = bCockpitlessView == 0;
                    GetScreenUpdateFlag();
                    if (bCockpitlessView != 0) {
                        SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
                        {
                            signed char screenMode = cScreenViewMode;

                            screenMode++;
                            cScreenViewMode = screenMode;
                            initialize_cockpit((int)screenMode - 1);
                        }
                        SetMousePosition(
                            ((int)stSpaceBufferViewport.right -
                             (int)stSpaceBufferViewport.left) / 2 + 1,
                            nCurrentViewCenterPosY);
                    } else {
                        SetViewportRect(
                            &stSpaceBufferViewport, 0, 0,
                            (short)(nScreenWidth - 1),
                            (short)(nScreenHeight - 1));
                        {
                            signed char screenMode = cScreenViewMode;

                            screenMode++;
                            cScreenViewMode = screenMode;
                            initialize_cockpit((int)screenMode - 1);
                        }
                        SetMousePosition(
                            ((int)stSpaceBufferViewport.right -
                             (int)stSpaceBufferViewport.left) / 2 + 1,
                            ((int)stSpaceBufferViewport.bottom -
                             (int)stSpaceBufferViewport.top) / 2);
                    }
                } else {
                    bMouseCursorVisible = 0;
                    if (bCockpitlessView == 0) {
                        new_view(0, 0);
                    } else {
                        GetScreenUpdateFlag();
                        SetViewportRect(
                            &stSpaceBufferViewport, 0, 0,
                            (short)(nScreenWidth - 1),
                            (short)(nScreenHeight - 1));
                        initialize_view_buffer();
                        new_view(0, 0);
                        GetScreenUpdateFlag();
                        SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
                        initialize_view_buffer();
                    }
                }
                FlushInputEvents();
                ClearDebugPauseFlags();
                bMouseCursorVisible = 0;
                bPtrMovedByKeyboardState = 1;
            }
            break;
        case 0x3c:
            bMouseCursorVisible = 0;
            if (bCockpitlessView == 0) {
                new_view(2, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0,
                            (short)(nScreenWidth - 1),
                            (short)(nScreenHeight - 1));
            initialize_view_buffer();
            new_view(2, 0);
            goto restore_normal_viewport;
        case 0x3d:
            bMouseCursorVisible = 0;
            if (bCockpitlessView == 0) {
                new_view(1, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0,
                            (short)(nScreenWidth - 1),
                            (short)(nScreenHeight - 1));
            initialize_view_buffer();
            new_view(1, 0);
            goto restore_normal_viewport;
        case 0x3e:
            bMouseCursorVisible = 0;
            if (bCockpitlessView == 0) {
                new_view(3, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0,
                            (short)(nScreenWidth - 1),
                            (short)(nScreenHeight - 1));
            initialize_view_buffer();
            new_view(3, 0);
            goto restore_normal_viewport;
        case 0x3f:
            bMouseCursorVisible = 0;
            if (bCockpitlessView == 0) {
                new_view(4, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0,
                            (short)(nScreenWidth - 1),
                            (short)(nScreenHeight - 1));
            initialize_view_buffer();
            new_view(4, 0);
            goto restore_normal_viewport;
        case 0x40:
            bMouseCursorVisible = 0;
            if (bCockpitlessView == 0) {
                new_view(14, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0,
                            (short)(nScreenWidth - 1),
                            (short)(nScreenHeight - 1));
            initialize_view_buffer();
            new_view(14, 0);
            goto restore_normal_viewport;
        case 0x41:
            if (acShipTargetTable[0] == -1)
                break;
            bMouseCursorVisible = 0;
            if (bCockpitlessView == 0) {
                new_view(7, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0,
                            (short)(nScreenWidth - 1),
                            (short)(nScreenHeight - 1));
            initialize_view_buffer();
            new_view(7, 0);
            goto restore_normal_viewport;
        case 0x42:
            if (notRepeated) {
                int missileCameraEnabled;

                missileCameraEnabled = bMissileCameraEnabled ^ 1;
                bMouseCursorVisible = 0;
                bMissileCameraEnabled = missileCameraEnabled;
                if (missileCameraEnabled != 0)
                    SetHudMessageText("MISSILE CAMERA ON",
                                      cRedColour, 20);
                else
                    SetHudMessageText("MISSILE CAMERA OFF",
                                      cPrimaryTextColour, 20);
            }
            break;
        case 0x43:
            if (notRepeated) {
                bMouseCursorVisible = 0;
                SelectNextExternalViewObject();
                if (bCockpitlessView == 0) {
                    force_view(4, (short)cViewObject);
                    break;
                }
                GetScreenUpdateFlag();
                SetViewportRect(
                    &stSpaceBufferViewport, 0, 0,
                    (short)(nScreenWidth - 1),
                    (short)(nScreenHeight - 1));
                initialize_view_buffer();
                force_view(4, (short)cViewObject);
                goto restore_normal_viewport;
            }
            break;
        }
    }
    goto primary_controls_complete;

restore_normal_viewport:
    GetScreenUpdateFlag();
    SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
    initialize_view_buffer();

primary_controls_complete:
    switch (bCurrentKey) {
    case 1:
        bEscapePressedFlag = 0;
        if (nTrainSimActive != 0)
            return -1;
        if (get_mode(1) == 4) {
            CloseCommChoiceMenu();
            return 0;
        }
#ifdef SDL_PORT
        /* The original leaves Escape inert in flight, leaving no way to
         * halt a mission.  Pause on it, exactly as the pause key does. */
        ShowGamePausedBanner(1);
        SetFrameTimerPeriodDirect(1);
        return 0;
#endif
        break;
    case 0x0c:
    case 0x4a:
        if (control != 0) {
            ReportFramesSkipped(-1);
            return 0;
        }
        accelerate(-1);
        return 0;
    case 0x0d:
    case 0x4e:
        if (control != 0) {
            ReportFramesSkipped(1);
            return 0;
        }
        accelerate(1);
        return 0;
    case 0x0e:
        anShipSpeed[0] = 0;
        return 0;
    case 0x0f:
    case 0x37:
        your_afterburner();
        return 0;
    case 0x11:
        if (notRepeated) {
            SelectCockpitVduMode(0, 1);
            return 0;
        }
        break;
    case 0x14:
        if (notRepeated) {
            SelectCockpitVduMode(1, 3);
            return 0;
        }
        break;
    case 0x19:
        ShowGamePausedBanner((short)(control == 0));
        SetFrameTimerPeriodDirect(1);
        return 0;
    case 0x1c:
        if (notRepeated &&
            (selectedWeapon = nSelectedReleaseWeaponIndex) != -1) {
            if (((ShipWeaponSlot *)&aShipWeaponsTable[0][1])[
                    selectedWeapon].type == OBJECT_TYPE_SPACE_MINE) {
                drop_player_mine(0);
                return 0;
            }
            if (nExternalViewShip == -1) {
                nExternalViewShip =
                    (short)fire_missile(0);
                if (bMissileCameraEnabled == 1 &&
                    nExternalViewShip != -1) {
                    new_view(6, nExternalViewShip);
                    return 0;
                }
            }
        }
        break;
    case 0x32:
        if (notRepeated && control != 0) {
            if (nMusicVolumeSetting == 0)
                nMusicVolumeSetting = 20;
            else
                nMusicVolumeSetting = 0;
            SetMusicStreamVolume(
                (unsigned short)anVolumeLevels[
                    nMusicVolumeSetting / 2]);
            ShowOnScreenMessage(0, 0, "MUSIC VOLUME: %d.",
                                nMusicVolumeSetting / 2);
            return 0;
        }
        break;
    case 0x1f:
        if (control != 0 && notRepeated) {
            if (nSfxVolumeSetting == 0)
                nSfxVolumeSetting = 20;
            else
                nSfxVolumeSetting = 0;
            SetSoundEffectsVolume(
                anVolumeLevels[
                    nSfxVolumeSetting / 2]);
            ShowOnScreenMessage(0, 0, "SFX VOLUME: %d.",
                                nSfxVolumeSetting / 2);
            return 0;
        }
        break;
    case 0x20:
        if (notRepeated) {
            SelectCockpitVduMode(0, 2);
            return 0;
        }
        break;
    case 0x22:
        if (notRepeated) {
            SelectCockpitVduMode(0, 1);
            return 0;
        }
        break;
    case 0x24:
        if (control != 0) {
            CalibrateJoystickInteractive();
            return 0;
        }
        break;
    case 0x26:
        if (notRepeated) {
            nTargetLockMode =
                (short)(nTargetLockMode == 0);
            PlaySfxWaveFileByNumber(0x19, -1, 0);
            if (get_mode(1) == 3) {
                InvalidateVduMode(1);
                return 0;
            }
        } else {
            init_player_input();
            return 0;
        }
        break;
    case 0x2b:
        accelerate(9000);
        return 0;
    case 0x2f:
        if (notRepeated && control != 0) {
            ShowVersionBanner();
            return 0;
        }
        break;
    case 0x39:
        fire_players_lasers();
        break;
    }
    return 0;
}

/* Function start: 0x429DD0 */
unsigned int Draw_3Space_Frame(void)
{
    UpdateSpacePaletteFade();
    nFrameSkipCounter--;
    if (nFrameSkipCounter > 0)
        return 0;
    nFrameSkipCounter = nFrameSkip;
    nRenderedSpaceFrame++;
    transform_objects_to_your_view();
    update_star_field();
    place_exhaust_on_ships();
    reposition_fixed_child_objects();
    sort_object_depth();
#ifdef SDL_PORT
    SdlBeginSpaceFrame(
        pScreenViewGeometry,
        (int)cScreenViewMode,
        bCockpitlessView > 0,
        (unsigned char)cPrimaryViewBufferColour);
#endif
    draw_sorted_objects_to_buffer();
    if (nCameraViewMode == 0)
        overlay_head_up_display();
    return 1;
}

/* Function start: 0x429E30 */
void GetArcadeBonus(void)
{
    nArcadeWaveBonusValue =
        (nArcadeTimeRemaining *
             (nTrainSimMission + 1) +
         (nTrainSimMission +
          (nArcadeWave * 5 + 5) * 2) * 50) * 2;
}

/* Function start: 0x429E70 */
void FigureArcadeTime(void)
{
    nArcadeTimeRemaining =
        (short)((nArcadeWave + 6) * 400);
}

/* Function start: 0x429E90 */
void DrawArcadeScorePanel(short x, short y)
{
    char score[20];

    sprintf(score, "%0ld", nCurrentArcadeScore);
    DrawFormattedText("%X%YScore: %s0 %XTime: %u %X1 UP",
                      x, y, score, x + 0x82,
                      nArcadeTimeRemaining, x + 0xbe);
}

/* Function start: 0x429EE0 */
void UpdateArcadeScoreDisplay(void)
{
    char bonus[20];

    if (nTrainSimActive != 0) {
        SetTextContext(&stHudMessageTextContext);
        DrawArcadeScorePanel(10, 10);
        if (nArcadeBonusCountdown <= 0) {
            nCurrentArcadeScore++;
            nArcadeTimeRemaining--;
            if (nArcadeTimeRemaining <= 0) {
                nArcadeState = 4;
                return;
            }
        } else {
            sprintf(bonus, "%0ld", nArcadeWaveBonusValue);
            SetTextCursor((unsigned short)stSpaceBufferViewport.left,
                          (unsigned short)((stSpaceBufferViewport.top +
                                            stSpaceBufferViewport.bottom) / 2 - 5));
            if (nCurrentWave != -1) {
                FormatTextBufferFromStart(
                    "Wave %d complete.\n\nBonus Points: %s0%P",
                    nArcadeWave + 1, bonus);
                return;
            }
            FormatTextBufferFromStart(
                "Mission %d complete.\n\nBonus Points: %s0%P",
                nTrainSimMission + 1, bonus);
        }
    }
}

/* Function start: 0x429FC0 */
unsigned int RenderSpaceViewFrame(void)
{
    if (Draw_3Space_Frame() == 0)
        return 0;
    check_message();
    UpdateArcadeScoreDisplay();
    RestoreCockpitExplosionIfVisible();
    dump_buffer_to_screen();
    if (nCameraViewMode == 0)
        RestoreTransientCockpitGraphics();
    if (bCockpitlessView == 0 && nTrainSimActive != 0) {
        DrawFilledViewportRect(&stSpaceBufferViewport, 10, 10,
                               stSpaceBufferViewport.right, 0x11,
                               cPrimaryViewBufferColour);
        if (nArcadeBonusCountdown != 0) {
            nArcadeBonusCountdown--;
            if (nArcadeBonusCountdown == 0) {
                if (Vector_magnitude(
                        &aShipPositionsArray[0]) > 0x271000)
                    zero_vector(&aShipPositionsArray[0]);
                nCurrentArcadeScore += nArcadeWaveBonusValue;
                if (nCurrentWave == -1)
                    nArcadeState = 1;
                else
                    nArcadeWave++;
                ClearViewport(&stSpaceBufferViewport,
                              cPrimaryViewBufferColour);
            }
        }
    }
    ClearViewport(&stSpaceBufferViewport, cPrimaryViewBufferColour);
    return 1;
}

/* Function start: 0x42A0C0 */
unsigned int RefreshCockpitStatus(void)
{
    Update_3Space();
    if (nFrameSkipCounter <= 1)
        clear_view_buffer();
    return Draw_3Space_Frame();
}

/* Function start: 0x42A0E0 */
short GetShipDistanceToNavPoint(short ship, MissionNavPoint *navPoint)
{
    FixedVector delta;

    ComputeVectorDelta(&aShipPositionsArray[ship],
                       &navPoint->position, &delta);
    return FixedToShortSaturating(Vector_magnitude(&delta));
}

/* Function start: 0x42A120 */
short FindNearestNavPoint(short ship)
{
    short navPointIndex = 0;
    MissionNavPoint *navPoint = aMissionNavPoints;

    do {
        if (navPoint->type == 1 &&
            GetShipDistanceToNavPoint(ship, navPoint) < navPoint->proximityRadius)
            return navPointIndex;
        navPointIndex++;
        navPoint++;
    } while (navPointIndex < ACTIVE_MISSION_NAV_POINT_COUNT);

    return nCurrentNavigationPoint;
}

/* Function start: 0x42A170 */
unsigned int ReleaseStaleNavTarget(void)
{
    short v = FindNearestNavPoint(0);

    if (nCurrentNavigationPoint != v)
        set_up_action_sphere(v);
    return 0;
}

/* Function start: 0x42A190 */
int RunSpaceFlight(short entryNavPoint)
{
    Viewport *savedViewport;
    unsigned int frameReady;

    bCockpitlessView = 0;
    if (nTrainSimActive == 0 &&
        bCockpitEnabled == 0)
        bCockpitlessView = 1;
    nFrameSkipCounter = 1;
    bInputModeState = 1;
#ifdef SDL_PORT
    SdlSetMouseGrab(1);
#endif
    SetEventManagerPump(get_player_input);
    savedViewport = (Viewport *)stMouseCursorState.viewport;
    stMouseCursorState.viewport = &stSpaceBufferViewport;
    init_inflight_music();

    if (entryNavPoint == -1)
        entryNavPoint = aMissionShips[
            stCurrentMissionHeader.playerMissionShip].navPoint;
    set_up_action_sphere(entryNavPoint);

    if (bCockpitlessView != 0) {
        GetScreenUpdateFlag();
        SetViewportRect(&stSpaceBufferViewport, 0, 0,
                        (unsigned short)(nScreenWidth - 1),
                        (unsigned short)(nScreenHeight - 1));
        initialize_view_buffer();
        new_view(0, 0);
        GetScreenUpdateFlag();
        SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
        cScreenViewMode++;
        initialize_cockpit(cScreenViewMode - 1);
        SetMousePosition(
            (stSpaceBufferViewport.right - stSpaceBufferViewport.left) / 2 + 1,
            nCurrentViewCenterPosY);
        bMouseAfterburnerControl = 0;
        bMouseCursorVisible = 0;
        initialize_view_buffer();
        FlushInputEvents();
    }

    copy_frame(0, 62);
    WarpMouseTo((short)((stSpaceBufferViewport.left +
                         stSpaceBufferViewport.right) / 2),
                (short)((stSpaceBufferViewport.top +
                         stSpaceBufferViewport.bottom) / 2));
    FlushInputEvents();
    bMouseAfterburnerControl = 0;
    bMouseCursorVisible = 0;
    nArcadeState = 0;
    DIBslam();
    DIBslamReal();
    SetSpaceFlightFrameTiming();
    FlushInputEvents();
    ClearDebugPauseFlags();
    bMouseCursorVisible = 0;
    bPtrMovedByKeyboardState = 1;
    frameReady = 1;

    while (nArcadeState == 0) {
        ReadPerformanceCounter(&liFrameStart);
        if (HandleSpaceFlightControls() == -1) {
            nArcadeState = 5;
            break;
        }
        ReadPerformanceCounter(&liFlightAfterInputCounter);
        if (nArcadeState == 0) {
            Update_3Space();
            ReadPerformanceCounter(&liSimulationDone);
            frameReady = RenderSpaceViewFrame();
            ReadPerformanceCounter(&liRenderDoneCounter);
            update_cockpit();
        } else {
            liSimulationDone =
                liRenderDoneCounter =
                    liFlightAfterInputCounter;
        }
        ReadPerformanceCounter(&liFlightAfterCockpitCounter);
        if (frameReady != 0) {
            frameReady = 0;
            DIBslam();
            DIBslamReal();
        }
        ReadPerformanceCounter(&liFlightFrameEndCounter);
        nFlightPresentDelta =
            (int)((unsigned int)liFlightFrameEndCounter.QuadPart -
                  (unsigned int)liFlightAfterCockpitCounter.QuadPart);
        nHudDelta =
            (int)((unsigned int)liFlightAfterCockpitCounter.QuadPart -
                  (unsigned int)liRenderDoneCounter.QuadPart);
        nDrawDelta =
            (int)((unsigned int)liRenderDoneCounter.QuadPart -
                  (unsigned int)liSimulationDone.QuadPart);
        nSimulationDelta =
            (int)((unsigned int)liSimulationDone.QuadPart -
                  (unsigned int)liFlightAfterInputCounter.QuadPart);
        nInputTicks =
            (int)((unsigned int)liFlightAfterInputCounter.QuadPart -
                  (unsigned int)liFrameStart.QuadPart);
        nFlightFrameTicks =
            (int)((unsigned int)liFlightFrameEndCounter.QuadPart -
                  (unsigned int)liFrameStart.QuadPart);
        nUnusedFlightProfileTicksA = 0;
        nUnusedFlightProfileTicksB = 0;
        nUnusedFlightProfileTicksC = 0;
    }

#ifdef SDL_PORT
    SdlSetMouseGrab(0);
    SdlCancelSpaceFrame();
    if (SdlUsingDosData())
        SdlStopDosSoundEffects();
#endif
    SetCinematicFrameTiming();
    SetViewportRect(&stSpaceBufferViewport, 0, 0,
                    (unsigned short)(nScreenWidth - 1),
                    (unsigned short)(nScreenHeight - 1));
    bCockpitlessView = 0;
    if (nArcadeState == 1)
        flag_objective(find_objective(1, -1), 2);
    bCockpitlessView = 0;
    ResetCockpitPaletteEntries();
    stMouseCursorState.viewport = savedViewport;
    free_inflight_music();
    SetEventManagerPump(0);
    bMouseCursorVisible = 0;
    QueueInputEvent(13, 160, 100, 0, 0, 0, 0);
    SetMouseCursorShape(stMouseCursorState.shape, 0);
    return nArcadeState;
}

/* Function start: 0x42A520 */
int calculate_damage_level(void)
{
    ObjectTypeData *typeData;
    short damage;

    typeData = &aObjectTypeData[aeObjectTypeList[0]];
    damage = (short)(((typeData->armorLeft -
                       aasShipArmor[0][2]) * 4) /
                     typeData->armorLeft);
    damage = (short)(damage +
        ((typeData->armorRear - aasShipArmor[0][1]) * 4) /
            typeData->armorRear);
    damage = (short)(damage +
        ((typeData->armorRight - aasShipArmor[0][3]) * 4) /
            typeData->armorRight);
    damage = (short)(damage +
        ((typeData->armorFront - aasShipArmor[0][0]) * 4) /
            typeData->armorFront);
    damage = (short)(
        (acShipDmg[0] * 30) / typeData->damageCapacity +
        damage * 2);
    damage = (short)(damage +
                     asShipAccumulatedDmg[0] * 5);

    if (damage < 5)
        return 0;
    if (damage < 40)
        return 1;
    if (damage < 70)
        return 2;
    return 3;
}

/* Function start: 0x42A610 */
void UpdateTrainSimMenuCursor(void)
{
    short mouseX;
    short mouseY;
    short frame;
    TitleMenuRegion *region;

    frame = 0;
    mouseX = stMouseCursorState.x;
    mouseY = stMouseCursorState.y;
    region = aTrainSimMissionRegions;
    while (region->frame != -1) {
        if (IsPointInRect(mouseX, mouseY, &region->left) != 0)
            frame = region->frame;
        region++;
    }
    SetMouseCursorShape(stMouseCursorState.shape, frame);
}

/* Function start: 0x42A670 */
void ResetMouseCursorFrame(void)
{
    SetMouseCursorShape(stMouseCursorState.shape, 0);
}

/* Function start: 0x42A680 */
void UpdateRoomMenuCursor(void)
{
    short mouseX;
    short mouseY;
    short state;
    short frame;
    const TitleMenuRegion *region;
    short index;

    mouseX = stMouseCursorState.x;
    frame = stCurrentRoomMenu.cursorFrame;
    index = 0;
    region = stCurrentRoomMenu.regions;
    mouseY = stMouseCursorState.y;
    ClearRoomMenuLabel();
    while (region->frame != -1) {
        state = IsPointInRect(mouseX, mouseY, &region->left);
        if (state != 0) {
            frame = region->frame;
            if (index >= 20)
                return;
            if (index < 0)
                return;
            SelectRoomMenuLabel(index);
        }
        index++;
        region++;
    }
    SetMouseCursorShape(stMouseCursorState.shape, frame);
}

/* Function start: 0x42A700 */
void __stdcall FadeViewportPaletteToColour(Viewport *viewport,
                                           unsigned short colour,
                                           short enabled)
{
    unsigned char *indices;
    unsigned short target[3];
    short *currentPalette;
    short *targetPalette;
    unsigned int paletteBytes;
    short index;
    short paletteIndex;
    short activeCount;

    (void)enabled;
    if (nVideoMode == 0x13) {
        indices = AllocateTaggedMemory(256, 0);
        if (indices == 0)
            return;
        memset(indices, 0, 256);
        activeCount = CollectActivePaletteIndices(viewport, indices, 256);
        paletteBytes = (unsigned int)(activeCount * 6);
        currentPalette = AllocateTaggedMemory(paletteBytes, 0);
        if (currentPalette == 0) {
            ReleasePacketHandle(indices);
            return;
        }
        memset(currentPalette, 0, paletteBytes);
        targetPalette = AllocateTaggedMemory(paletteBytes, 0);
        if (targetPalette == 0) {
            ReleasePacketHandle(indices);
            ReleasePacketHandle(currentPalette);
            return;
        }
        memset(targetPalette, 0, paletteBytes);

        GetPaletteEntry((short)colour, target);
        for (index = 0; index < activeCount; index++) {
            GetPaletteEntry((short)indices[index],
                            (unsigned short *)&currentPalette[index * 3]);
            memcpy(&targetPalette[index * 3], target, 6);
        }

        while (StepPaletteTransition(
                   currentPalette, targetPalette,
                   (short)(activeCount * 3)) != 0) {
            for (paletteIndex = 0; paletteIndex < activeCount; paletteIndex++) {
                abPaletteTripletsArray[indices[paletteIndex]][0] =
                    (unsigned char)currentPalette[paletteIndex * 3];
                abPaletteTripletsArray[indices[paletteIndex]][1] =
                    (unsigned char)currentPalette[paletteIndex * 3 + 1];
                abPaletteTripletsArray[indices[paletteIndex]][2] =
                    (unsigned char)currentPalette[paletteIndex * 3 + 2];
            }
            SetWholePaletteFromTriplets(&abPaletteTripletsArray[0][0]);
        }

        ReleasePacketHandle(targetPalette);
        ReleasePacketHandle(currentPalette);
        ReleasePacketHandle(indices);
    }
    DIBslam();
    DIBslamReal();
}
