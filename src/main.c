/**
 * @file
 * @ingroup flight
 * @brief WINGLEADER's main module: process services, flight control and the
 * HUD.
 *
 * Address range 4272f0-42a8ef. The initialized state and the complete
 * literal order identify this group; its trailing cursor and palette helpers
 * are provisional.
 */
#ifndef SDL_PORT
#include <dos.h>
#endif
#include "wc1.h"
#ifndef SDL_PORT
#include <malloc.h>
#endif

/* Startup installs this event-manager hook for DOS vector 0x21. */
void DosInterruptHandlerHook(void);

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
volatile int bMouseCursorVisible = 0; /* 0x0046a018 */
unsigned char bInputPollingGuard = 0; /* 0x0046a01c */
static InputDeviceSample stLastPolledFlightInput = {0}; /* 0x0046a020 */
#ifndef SDL_PORT
static
#endif
int bMouseAfterburnerControl = 0; /* 0x0046a02c */

/* Input-queue callees of the flight-input routines below. */
short __stdcall PollInputEvent(InputEventState *event, short filter);
short __stdcall IsInputEventQueued(short type);

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
static volatile unsigned char bAfterburnerButtonLatched = 0; /* 0x0046a054 */
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
struct Shape *pLaunchDoorShape; /* 0x005a77e8 */
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
            cViewportClearColour,
            cBlackColour);
        SetTextContext(&stDefaultTxtContext);
        DrawFormattedText("%X%YCurrent NMem %d.",
                          0, 176, (short)GetOriginalFreeMemory());
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
    bSavedGameExists = 1;
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
                nStartNavPointOverride = atoi(argument + 2);
            } else {
                nMusicPlaybackMode = 2;
                nMainArcadeStartupParameterValue =
                    atoi(option);
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
            bSavedGameExists = 1;
            break;
        case 'l':
            if (nOriginDevUnlock != 0)
                launchMission = 1;
            break;
        case 'm':
            if (nOriginDevUnlock != 0)
                mission = atoi(argument + 1);
            break;
        case 's':
            if (nOriginDevUnlock != 0) {
                bCampaignActive = 1;
                series = atoi(argument + 1);
            }
            break;
        case 'w':
            if (nOriginDevUnlock != 0) {
                animationDemo = 1;
                animationIndex = atoi(argument + 1);
            }
            break;
        }
    }

    SetCinematicFrameTiming();
    stInitialCampaignState.currentSeries =
        stCampaignState.currentSeries = series;
    stInitialCampaignState.currentMission =
        stCampaignState.currentMission = mission;
    nVideoMode = 0x13;
    LoadOriginFxDrivers();
    nVideoMode = 0x13;

    if (animationDemo == 1) {
        SystemDebugPrintf("Pre animation: %lu.\n",
                          GetFixedOneMillionThunkAlt(0));
        WaitForKeyAcknowledge(0);
        RunAnimationDemoLoop(animationIndex);
        SystemDebugPrintf("Post animation: %lu.\n",
                          GetFixedOneMillionThunkAlt(0));
        WaitForKeyAcknowledge(0);
    }

    LoadVolumeSettingsFromRegistry();
    SetSoundEffectsVolume(
        anVolumeLevels[nSfxVolumeSetting / 2]);
    SetMusicStreamVolume(anVolumeLevels[
        nMusicVolumeSetting / 2]);

#ifdef SDL_PORT
    if (nLaunchCampaign && !SdlHasSecretMissions(nLaunchCampaign)) {
        fprintf(stderr, "Secret Missions %d data is missing or unsupported.\n", nLaunchCampaign);
        return 1;
    }
#endif
    if (launchMission == 0) {
#ifdef SDL_PORT
        SdlPlayDosStartupIntro();
#endif
        bEscapePressedFlag = 0;
        for (;;) {
#ifdef SDL_PORT
            /* Death, stranding, or ending a game returns here. A declined
             * autosave overwrite applies only until this new title session. */
            SdlResetAutosave();
#endif
            FrameStartHook(0);
            bCampaignStartupMode = 1;
#ifdef SDL_PORT
            /* The original transfer utility opens the barracks directly. */
            selection = nPilotTransferState ? 3 : Title_Sequence();
#else
            selection = Title_Sequence();
#endif
            switch (selection) {
            case 0:
#ifdef SDL_PORT
                StartNewCampaign(nLaunchCampaign);
#else
                StartNewCampaign(0);
#endif
                break;
            case 2:
                StartNewCampaign(1);
                break;
            case 3:
#ifdef SDL_PORT
                nPilotTransferCampaign = nLaunchCampaign;
                nPilotTransferState = 1;
                selection = BarracksScreen();
                nPilotTransferState = 0;
                bEscapePressedFlag = 1;
                if (selection != 7)
                    continue;
#else
                StartNewCampaign(2);
#endif
                break;
            default:
                break;
            }
            while (GameFlow() != 0)
                ;
        }
    }

#ifdef SDL_PORT
    if (init_mission(series, mission) != 0) {
        fprintf(stderr,
                "Mission SERIES=%d MISSION=%d is absent or invalid.\n",
                series, mission);
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
                             cPrimaryViewBufferColour,
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
    switch (cScreenViewMode) {
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
                  cPrimaryViewBufferColour);
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
                         cBlackColour, 0) == 0)
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
#ifdef SDL_PORT
    /* Docking ends SWC flight before another movement/collision pass. */
    if (SdlSwcFlightActive() && nCannedSceneMode == 0 && nArcadeState == 1)
        return 0;
#endif
    update_objects_in_space();
#ifdef SDL_PORT
    if (SdlSwcFlightActive()) {
        SdlUpdateSwcFlightCamera();
        if (nCameraViewMode != 16)
            SdlUpdateSwcFlightMusic();
    } else
#endif
    {
        set_eye_direction_and_position();
        servicetrack();
    }
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
        entry[0] = (entry[0] - 4);
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
            ClearViewport(&stSpaceBufferViewport, cRedColour);
            bViewportDirty = 1;
            asDamageFlashColour[0] = 0;
            break;
        case 0x13:
            asDamageFlashColour[0] =
                (asDamageFlashColour[0] - 4);
            SetPaletteEntry(cPrimaryViewBufferColour,
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
#ifdef SDL_PORT
    /* The CMF renderer owns SWC cockpit lights and damage flashes. */
    if (SdlSwcFlightActive())
        return 0;
#endif
    if (nCameraViewMode == 0) {
        for (count = 0; count < 6; count++) {
            FadeFlightPaletteEntry(aPaletteFadeEntries[count]);
            SetPaletteEntry((count + 0xb9),
                            aPaletteFadeEntries[count]);
        }
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
#ifdef SDL_PORT
    if (SdlSwcFlightActive() && nCurrentActiveInputDevice >= 0) {
        /* The SWC host already sampled normalized SDL axes this tick. */
        stLastPolledFlightInput = aInputDevSamples[nCurrentActiveInputDevice];
        TranslatePolledInputEvent(INPUT_EVENT_JOYSTICK_MOVE, 0);
        return;
    }
#endif
    if (nCurrentActiveInputDevice != -1 &&
        bInputPollingGuard == 0) {
        bInputPollingGuard++;
        UpdateInputDeviceTransitions(0);
        if (aInputDevSamples[nCurrentActiveInputDevice].x == 0 &&
            aInputDevSamples[nCurrentActiveInputDevice].y == 0 &&
            aInputDevSamples[nCurrentActiveInputDevice].buttons == 0) {
            if (aInputDevSamples[nCurrentActiveInputDevice].x != stLastPolledFlightInput.x ||
                aInputDevSamples[nCurrentActiveInputDevice].y != stLastPolledFlightInput.y ||
                aInputDevSamples[nCurrentActiveInputDevice].buttons !=
                    stLastPolledFlightInput.buttons) {
                TranslatePolledInputEvent(INPUT_EVENT_JOYSTICK_MOVE, 0);
                stLastPolledFlightInput = aInputDevSamples[nCurrentActiveInputDevice];
                bInputPollingGuard--;
                return;
            }
        } else {
            TranslatePolledInputEvent(INPUT_EVENT_JOYSTICK_MOVE, 0);
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
    case SCAN_HOME:
        *key++ = SCAN_UP;
        *key++ = SCAN_LEFT;
        break;
    case SCAN_PAGEUP:
        *key++ = SCAN_UP;
        *key++ = SCAN_RIGHT;
        break;
    case SCAN_END:
        *key++ = SCAN_DOWN;
        *key++ = SCAN_LEFT;
        break;
    case SCAN_PAGEDOWN:
        *key++ = SCAN_DOWN;
        *key++ = SCAN_RIGHT;
        break;
    default:
        *key++ = bCurrentKey;
        break;
    }
    *key = -1;

    key = keys;
    while (finished == 0) {
        switch (*key++) {
        default:
            handled--;
            break;
        case SCAN_COMMA:
        case SCAN_INSERT:
            bMouseCursorVisible = 0;
            if (stRotationInput.z > 0) {
                stRotationInput.z = 0;
            } else {
                if (shift != 0)
                    stRotationInput.z = -9;
                if (stRotationInput.z > -9)
                    stRotationInput.z--;
                else if (cPreviousKey < 0)
                    stRotationInput.z--;
                else
                    stRotationInput.z++;
            }
            break;
        case SCAN_PERIOD:
        case SCAN_DELETE:
            bMouseCursorVisible = 0;
            if (stRotationInput.z < 0) {
                stRotationInput.z = 0;
            } else {
                if (shift != 0)
                    stRotationInput.z = 9;
                if (stRotationInput.z < 9)
                    stRotationInput.z++;
                else if (cPreviousKey < 0)
                    stRotationInput.z++;
                else
                    stRotationInput.z--;
            }
            break;
        case SCAN_UP:
            bMouseCursorVisible = 0;
            if (stRotationInput.x < 0) {
                stRotationInput.x = 0;
            } else if (control != 0) {
                nSfxVolumeSetting++;
                if (nSfxVolumeSetting > 20)
                    nSfxVolumeSetting = 20;
                SaveVolumeSettingsToRegistry();
                SetSoundEffectsVolume(anVolumeLevels[
                    nSfxVolumeSetting / 2]);
                ShowOnScreenMessage(0, 0, _("SFX VOLUME: %d."),
                                    nSfxVolumeSetting / 2);
            } else {
                if (shift != 0)
                    stRotationInput.x = 9;
                if (stRotationInput.x < 9)
                    stRotationInput.x++;
                else if (cPreviousKey < 0)
                    stRotationInput.x++;
                else
                    stRotationInput.x--;
            }
            break;
        case SCAN_DOWN:
            bMouseCursorVisible = 0;
            if (stRotationInput.x > 0) {
                stRotationInput.x = 0;
            } else if (control != 0) {
                nSfxVolumeSetting--;
                if (nSfxVolumeSetting < 0)
                    nSfxVolumeSetting = 0;
                SetSoundEffectsVolume(anVolumeLevels[
                    nSfxVolumeSetting / 2]);
                SaveVolumeSettingsToRegistry();
                ShowOnScreenMessage(0, 0, _("SFX VOLUME: %d."),
                                    nSfxVolumeSetting / 2);
            } else {
                if (shift != 0)
                    stRotationInput.x = -9;
                if (stRotationInput.x > -9)
                    stRotationInput.x--;
                else if (cPreviousKey < 0)
                    stRotationInput.x--;
                else
                    stRotationInput.x++;
            }
            break;
        case SCAN_RIGHT:
            if (control != 0) {
                nMusicVolumeSetting++;
                if (nMusicVolumeSetting > 20)
                    nMusicVolumeSetting = 20;
                SaveVolumeSettingsToRegistry();
                SetMusicStreamVolume(
                    anVolumeLevels[
                        nMusicVolumeSetting / 2]);
                ShowOnScreenMessage(0, 0, _("MUSIC VOLUME: %d."),
                                    nMusicVolumeSetting / 2);
                return;
            }
            bMouseCursorVisible = 0;
            if (stRotationInput.y < 0) {
                stRotationInput.y = 0;
            } else {
                if (shift != 0)
                    stRotationInput.y = 9;
                if (stRotationInput.y < 9)
                    stRotationInput.y++;
                else if (cPreviousKey < 0)
                    stRotationInput.y++;
                else
                    stRotationInput.y--;
            }
            break;
        case SCAN_LEFT:
            if (control != 0) {
                nMusicVolumeSetting--;
                if (nMusicVolumeSetting < 0)
                    nMusicVolumeSetting = 0;
                SaveVolumeSettingsToRegistry();
                SetMusicStreamVolume(
                    anVolumeLevels[
                        nMusicVolumeSetting / 2]);
                ShowOnScreenMessage(0, 0, _("MUSIC VOLUME: %d."),
                                    nMusicVolumeSetting / 2);
                return;
            }
            bMouseCursorVisible = 0;
            if (stRotationInput.y > 0) {
                stRotationInput.y = 0;
            } else {
                if (shift != 0)
                    stRotationInput.y = -9;
                if (stRotationInput.y > -9)
                    stRotationInput.y--;
                else if (cPreviousKey < 0)
                    stRotationInput.y--;
                else
                    stRotationInput.y++;
            }
            break;
        case SCAN_KP_5:
            WarpMouseTo(
                ((stSpaceBufferViewport.left +
                         stSpaceBufferViewport.right) / 2),
                ((stSpaceBufferViewport.top +
                         stSpaceBufferViewport.bottom) / 2));
            stRotationInput.z = 0;
            stRotationInput.x = 0;
            stRotationInput.y = 0;
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
                anObjectYawRotation[0] -= stRotationInput.y;
                anObjPitchRot[0] -= stRotationInput.x;
            }
        }
        return 0;
    }

    type = stCampaignState.playerShipType;
    anObjPitchRot[0] =
        ((aObjectTypeData[type].yawRate * stRotationInput.x) / 8);
    anObjectYawRotation[0] =
        -((aObjectTypeData[type].pitchRate * stRotationInput.y) / 8);
    anObjectRollRotationList[0] =
        -((aObjectTypeData[type].rollRate * stRotationInput.z) / 8);
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
    short pitchInput;
    short eventType;
    short queuedKeyEvent;
    short horizontal;
    short vertical;
    unsigned short horizontalMagnitude;
    unsigned short verticalMagnitude;
    short yawInput;
    short modifiers;
    int keyboardRoll;
    int viewportLeft;
    int mouseX;
    int afterburnerControl;
    unsigned int buttons;
    unsigned int key;

    cPreviousKey = bCurrentKey;
    stPreviousRotationInput.y = stRotationInput.y;
    stPreviousRotationInput.x = stRotationInput.x;
    stPreviousRotationInput.z = stRotationInput.z;
    keyboardRoll = 0;
    eventType = PollInputEvent(&event, INPUT_EVENT_FILTER_ALL);
    wCurrentInputModifiers = event.modifiers;
    modifiers = event.modifiers;
    TranslatePolledInputEvent(eventType, event.value);
    bJoystickEventQueued = IsInputEventQueued(INPUT_EVENT_JOYSTICK_MOVE);
    bMouseMoveEvtQueued = IsInputEventQueued(INPUT_EVENT_MOUSE_MOVE);
    queuedKeyEvent = IsInputEventQueued(INPUT_EVENT_KEY_REPEAT);
    queuedKeyEvent |= IsInputEventQueued(INPUT_EVENT_KEY_DOWN);
    queuedKeyEvent |= IsInputEventQueued(INPUT_EVENT_KEY_UP);
    bCurrentKeyboardEvtQueued = queuedKeyEvent;
    bMouseButtonEventQueuedFlag = IsInputEventQueued(INPUT_EVENT_BUTTON_DOWN);
    if (bMouseMoveEvtQueued == 0)
        bCurrentKey |= SCAN_RELEASE_FLAG;
#ifdef SDL_PORT
    bCurrentKey |= SCAN_RELEASE_FLAG;
#else
    /* Preserve the separate memory update without marking the key's address
     * as taken throughout this unit. */
    ((volatile signed char)bCurrentKey) |= SCAN_RELEASE_FLAG;
#endif

    if (bMouseCursorVisible == 0) {
        key = PollKeyboardState();
        bCurrentKey = key;
        if ((volatile signed char)bCurrentKey == SCAN_NONE) {
            stRotationInput.z = 0;
            bFlightRollLatch = 0;
            stRotationInput.x = 0;
            stRotationInput.y = 0;
        } else {
            bMouseAfterburnerControl = 0;
            process_player_input();
            switch (bCurrentKey) {
            case SCAN_COMMA:
            case SCAN_PERIOD:
            case SCAN_INSERT:
            case SCAN_DELETE:
                keyboardRoll = 1;
            }
        }
    }

    if (bMouseButtonEventQueuedFlag == 0) {
        buttons = bHostSecondaryMouseButton * INPUT_BUTTON_SECONDARY |
                  bHostPrimaryMouseButton;
        if (buttons != 0) {
            if (buttons == INPUT_BUTTON_BOTH) {
                bCurrentKey = SCAN_RETURN;
            } else if (buttons == INPUT_BUTTON_PRIMARY) {
                bCurrentKey = SCAN_SPACE;
                fire_players_lasers();
            }
            if ((buttons & INPUT_BUTTON_SECONDARY) == 0)
                cPreviousKey = SCAN_NONE;
            if (cPreviousKey == SCAN_TAB && buttons == INPUT_BUTTON_SECONDARY) {
                bCurrentKey = SCAN_TAB;
                if (buttons == INPUT_BUTTON_PRIMARY)
                    fire_players_lasers();
            }
        } else {
            bAfterburnerButtonLatched = 0;
        }
    }

    while ((eventType = GetNextInputEvent(&event)) != 0) {
        switch (eventType) {
        case INPUT_EVENT_BUTTON_DOWN:
            if ((short)event.value == INPUT_BUTTON_PRIMARY) {
                bCurrentKey = SCAN_SPACE;
                if ((event.modifiers & INPUT_MOD_SECONDARY_BUTTON) != 0) {
                    if (aeSpecialManeuvers[0] ==
                            SPECIAL_MANEUVER_AFTERBURNER)
                        fire_players_lasers();
                    else
                        bCurrentKey = SCAN_RETURN;
                }
            }
            if ((short)event.value == INPUT_BUTTON_BOTH)
                bCurrentKey = SCAN_RETURN;
            if ((short)event.value == INPUT_BUTTON_SECONDARY &&
                bAfterburnerButtonLatched == 0) {
                if ((int)(nTickCount60Hz -
                        dwLastSecondaryButtonPress) <=
                        nInputTickScale)
                    bCurrentKey = SCAN_TAB;
                bAfterburnerButtonLatched = 1;
            }
            if (cPreviousKey == SCAN_TAB &&
                (short)event.value == INPUT_BUTTON_SECONDARY) {
                bCurrentKey = SCAN_TAB;
                if ((short)event.value == INPUT_BUTTON_PRIMARY)
                    fire_players_lasers();
            }
            dwLastSecondaryButtonPress = nTickCount60Hz;
            break;
        case INPUT_EVENT_KEY_DOWN:
        case INPUT_EVENT_KEY_REPEAT:
            bMouseAfterburnerControl = 0;
            bCurrentKey = event.value;
            process_player_input();
            break;
        case INPUT_EVENT_JOYSTICK_MOVE:
            bMouseCursorVisible = bMouseAfterburnerControl = 0;
            if (((unsigned char)
                    stLastPolledFlightInput.buttons &
                    (JOY_BUTTON1 | JOY_BUTTON2)) ==
                    (JOY_BUTTON1 | JOY_BUTTON2)) {
                if (aeSpecialManeuvers[0] ==
                        SPECIAL_MANEUVER_AFTERBURNER)
                    fire_players_lasers();
                else
                    bCurrentKey = SCAN_RETURN;
            } else if ((stLastPolledFlightInput.buttons & JOY_BUTTON1) != 0) {
                fire_players_lasers();
            }
            buttons = (stLastPolledFlightInput.buttons & JOY_BUTTON2) >> 1;
            if (buttons != 0 &&
                aeSpecialManeuvers[0] ==
                    SPECIAL_MANEUVER_AFTERBURNER)
                buttons = 0;
            if (buttons != 0) {
                stRotationInput.z = stLastPolledFlightInput.x;
                accelerate(-(
                    stLastPolledFlightInput.y / 2));
            } else {
                if (stRotationInput.z != 0 &&
                    bFlightRollLatch == 0 &&
                    keyboardRoll == 0) {
                    stPreviousFlightInput.x = -1;
                    stRotationInput.z = 0;
                }
                if (stPreviousFlightInput.x !=
                        stLastPolledFlightInput.x ||
                    stPreviousFlightInput.y !=
                        stLastPolledFlightInput.y ||
                    stLastPolledFlightInput.x != 0 ||
                    stLastPolledFlightInput.y != 0) {
                    stRotationInput.y =
                        stLastPolledFlightInput.x;
                    stRotationInput.x =
                        -stLastPolledFlightInput.y;
                }
            }
            if (asInputButton2DoubleClick[
                    nCurrentActiveInputDevice] != 0)
                bCurrentKey = SCAN_TAB;
            if (cPreviousKey == SCAN_TAB &&
                (stLastPolledFlightInput.buttons & JOY_BUTTON2) != 0)
                bCurrentKey = SCAN_TAB;
            break;
        case INPUT_EVENT_MOUSE_MOVE:
            afterburnerControl =
                (unsigned short)(modifiers & INPUT_MOD_SECONDARY_BUTTON) >= 1;
            if (afterburnerControl != 0 &&
                aeSpecialManeuvers[0] ==
                    SPECIAL_MANEUVER_AFTERBURNER)
                afterburnerControl = 0;
            if (bMouseCursorVisible == 0) {
                bMouseCursorVisible = 1;
                stMouseCursorState.frame = 2;
            }
            if (*(volatile int *)&bCockpitlessView != 0) {
                switch (cCurrentCockpitView) {
                case 0:
                    event.y -= 10;
                    break;
                case 1:
                    event.y -= 25;
                    break;
                }
                horizontal = event.x +
                    (stSpaceBufferViewport.left -
                     stSpaceBufferViewport.right) / 2 + 1;
                vertical = event.y -
                    *(volatile short *)&nCurrentViewCenterPosY;
            } else {
                horizontal = event.x +
                    (stSpaceBufferViewport.left -
                     stSpaceBufferViewport.right) / 2 + 1;
                vertical = event.y +
                    (stSpaceBufferViewport.top - stSpaceBufferViewport.bottom) / 2;
            }
            stMouseCursorState.x = event.x;
            stMouseCursorState.y = event.y;
            horizontalMagnitude = abs(horizontal);
            verticalMagnitude = abs(vertical);
            for (yawInput = 0;
                 asMouseYawThresholds[yawInput] <=
                     (short)horizontalMagnitude;
                 yawInput++)
                ;
            if (horizontal < 0)
                yawInput = -yawInput;
            for (pitchInput = 0;
                 asMousePitchThresholds[pitchInput] <=
                     (short)verticalMagnitude;
                 pitchInput++)
                ;
            if (vertical < 0)
                pitchInput = -pitchInput;
            mouseX = event.x;
            viewportLeft = stSpaceBufferViewport.left;
            if (mouseX - 4 <= viewportLeft)
                yawInput = -8;
            if (stSpaceBufferViewport.right <= mouseX + 4)
                yawInput = 8;
            if (event.y - 4 <= stSpaceBufferViewport.top)
                pitchInput = -8;
            if (stSpaceBufferViewport.bottom <= event.y + 4)
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
                pitchInput = -pitchInput;
                stRotationInput.z = nMouseYawInput = yawInput;
                nMousePitchInput = pitchInput;
                accelerate((pitchInput / 2));
            } else if (bMouseAfterburnerControl == 1) {
                stRotationInput.z = 0;
                bMouseAfterburnerControl = 0;
                stRotationInput.y = nMouseYawInput = 0;
                stRotationInput.x = nMousePitchInput = 0;
                WarpMouseTo(
                    ((viewportLeft + stSpaceBufferViewport.right) / 2),
                    ((stSpaceBufferViewport.bottom +
                             stSpaceBufferViewport.top) / 2));
            } else {
                stRotationInput.z = 0;
                stRotationInput.y = nMouseYawInput = yawInput;
                stRotationInput.x = nMousePitchInput = pitchInput;
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

    object = cViewObject;
    cViewObject = -1;
    do {
        object++;
        if (object > 9)
            object = 0;
        if (aeObjectClass[object] >= OBJECT_CLASS_SHIP)
            cViewObject = object;
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

    object = cViewObject;
    cViewObject = -1;
    do {
        object--;
        if (object < 0)
            object = 9;
        if (aeObjectClass[object] >= OBJECT_CLASS_SHIP)
            cViewObject = object;
    } while (cViewObject == -1);
    return 0;
}

/* Function start: 0x428D10 */
unsigned int HandleFleetOverviewInput(void)
{
    short key;

    key = bCurrentKey;
    if (nCameraViewMode == 8) {
        bCurrentKey = SCAN_NONE;
        switch (key) {
        case SCAN_RETURN:
            cViewObject--;
            bCurrentKey = SCAN_GRAVE;
            break;
        case SCAN_HOME:
            nCapitalShipViewDistance -= 0x3200;
            break;
        case SCAN_UP:
            rotate_about_i(-7,
                           &aShipUpVector[EYE_OBJECT],
                           &aShipFwdVec[EYE_OBJECT]);
            break;
        case SCAN_LEFT:
            rotate_about_j(7,
                           &aShipRightVector[EYE_OBJECT],
                           &aShipFwdVec[EYE_OBJECT]);
            break;
        case SCAN_RIGHT:
            rotate_about_j(-7,
                           &aShipRightVector[EYE_OBJECT],
                           &aShipFwdVec[EYE_OBJECT]);
            break;
        case SCAN_END:
            nCapitalShipViewDistance += 0x3200;
            break;
        case SCAN_DOWN:
            rotate_about_i(7,
                           &aShipUpVector[EYE_OBJECT],
                           &aShipFwdVec[EYE_OBJECT]);
            break;
        case SCAN_INSERT:
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
            bCurrentKey = key;
            break;
        }
    }
    return 0;
}

/* Function start: 0x428E70 */
/* Message dwell time: grows with text length, scaled by the speed setting. */
short MeasureMessageWidth(const char *text)
{
    return ((MinShort(5, (DosStrlen(text) >> 1)) + 5) *
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
            if (IsInputEventQueued(INPUT_EVENT_KEY_UP) != 0)
                acknowledged = 1;
        } while (acknowledged == 0);
        acknowledged = 0;
        FlushInputEvents();
        ClearDebugPauseFlags();
        do {
            PumpWindowMessages();
            if (IsInputEventQueued(INPUT_EVENT_KEY_DOWN) != 0)
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
    /* The pause filter retains the DOS P code alongside Win32 key values. */
    do {
        key = PumpMessagesDuringWait();
    } while (key == SCAN_P || key == 'P' || key == VK_CLEAR);
    FlushInputEvents();
#ifdef SDL_PORT
    SdlResumeMouseGrab();
#endif
}

/* Function start: 0x428F20 */
void ShowModalMessage(const char *format, ...)
{
    char text[52];
    va_list arguments;

    va_start(arguments, format);
    vsprintf(text, format, arguments);
    va_end(arguments);

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
    short modalShown;
    char text[52];
    va_list arguments;

    va_start(arguments, format);
    vsprintf(text, format, arguments);
    va_end(arguments);

    modalShown = 0;
    FlushInputEvents();
    if (duration == 9999)
        modalShown = ShowModalTextPanel(1, text);
    if (modalShown == 0) {
        if (duration == 0)
            duration = MeasureMessageWidth(text);
        SetHudTextColour(1);
        DosStrcpy(szHudMessageBuf, text);
        SetHudMessageText(szHudMessageBuf,
                          cRedColour, duration);
        if (duration == 9999) {
            ShowHudTextLine(szHudMessageBuf,
                            cRedColour);
            dump_buffer_to_screen();
        }
    }
    if (duration == 9999) {
        if (flags != 0)
            WaitForKeyAcknowledge(1);
        else
            WaitForKeyAcknowledge(0);
    }
    if (modalShown != 0) {
        ReleaseModalTextPanel();
        return;
    }
    if (duration == 9999)
        SetHudMessageText("", cRedColour, 2);
}

/* Function start: 0x4290A0 */
void ShowGamePausedBanner(short showBanner)
{
    if (showBanner != 0) {
        ShowOnScreenMessage(1, 9999, _("GAME PAUSED"));
        return;
    }
    WaitForKeyAcknowledge(1);
}

/* Function start: 0x4290D0 */
void ShowVersionBanner(void)
{
    ShowOnScreenMessage(1, 9999, _("WING COMMANDER VER. %s"),
                        pGameVersion);
}

/* Function start: 0x4290F0 */
void SetMessageDisplaySpeed(void)
{
    bMessageSpeed =
        (((signed char)bMessageSpeed + 1) % 5);
    ShowOnScreenMessage(0, 0, _("MESSAGES SPEED IS NOW %d."),
                        (signed char)bMessageSpeed + 1);
}

/* Function start: 0x429120 */
void ReportFramesSkipped(short adjustment)
{
    nFrameSkip = MinShort(
        MaxShort((nFrameSkip + adjustment), 1), 5);
    ShowOnScreenMessage(0, 0, _("%d FRAMES SKIPPED."),
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
        case SCAN_1:
        case SCAN_2:
        case SCAN_3:
        case SCAN_4:
        case SCAN_5:
        case SCAN_6:
        case SCAN_7:
        case SCAN_8:
        case SCAN_9:
            if (notRepeated && get_mode(1) == 4 &&
                nCameraViewMode == 0 &&
                bCurrentKey >= SCAN_1 &&
                bCurrentKey <=
                    nCommMenuChoiceCount + SCAN_1 &&
                get_mode(1) == 4) {
                Chosen_communicate_option(
                    (bCurrentKey - SCAN_1));
            }
            break;
        case SCAN_E:
            if (notRepeated &&
#ifdef SDL_PORT
                (control != 0 ||
                 (wCurrentInputModifiers & INPUT_MOD_CONTROL) != 0) &&
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
        case SCAN_A:
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
                        (nScreenWidth - 1),
                        (nScreenHeight - 1));
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
        case SCAN_S:
            if (control != 0 && notRepeated) {
                if (nFlightSoundEffectsEnabled == 1)
                    ResetSoundStateForScene();
                else
                    ResetSoundStateForFlight();
            }
            break;
        case SCAN_H:
            if (notRepeated && (wingman = nYourWingman) != -1 &&
                aeShipObjectiveList[wingman] !=
                    OBJECTIVE_HOLD_FORMATION) {
                request(0, wingman, 9);
            }
            break;
        case SCAN_C:
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
        case SCAN_V:
            if (notRepeated && control == 0) {
                bVideoImagesSuppressed =
                    bVideoImagesSuppressed == 0;
                if (bVideoImagesSuppressed != 0)
                    SetHudMessageText(_("VIDEO IMAGES SUPRESSED"),
                                      cRedColour, 20);
                else
                    SetHudMessageText(_("VIDEO IMAGES ENABLED"),
                                      cPrimaryTextColour, 20);
            }
            break;
        case SCAN_B:
            if (notRepeated && (wingman = nYourWingman) != -1 &&
                aeShipObjectiveList[wingman] ==
                    OBJECTIVE_HOLD_FORMATION &&
                any_enemy(0, 14000) != 0) {
                request(0, nYourWingman, 7);
            }
            break;
        case SCAN_N:
            if (notRepeated)
                SelectCockpitVduMode(1, 5);
            init_player_input();
            break;
        case SCAN_M:
            if (notRepeated && control == 0)
                SetMessageDisplaySpeed();
            break;
        case SCAN_F1:
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
                            initialize_cockpit(screenMode - 1);
                        }
                        SetMousePosition(
                            (stSpaceBufferViewport.right -
                             stSpaceBufferViewport.left) / 2 + 1,
                            nCurrentViewCenterPosY);
                    } else {
                        SetViewportRect(
                            &stSpaceBufferViewport, 0, 0,
                            (nScreenWidth - 1),
                            (nScreenHeight - 1));
                        {
                            signed char screenMode = cScreenViewMode;

                            screenMode++;
                            cScreenViewMode = screenMode;
                            initialize_cockpit(screenMode - 1);
                        }
                        SetMousePosition(
                            (stSpaceBufferViewport.right -
                             stSpaceBufferViewport.left) / 2 + 1,
                            (stSpaceBufferViewport.bottom -
                             stSpaceBufferViewport.top) / 2);
                    }
                } else {
                    bMouseCursorVisible = 0;
                    if (bCockpitlessView == 0) {
                        new_view(0, 0);
                    } else {
                        GetScreenUpdateFlag();
                        SetViewportRect(
                            &stSpaceBufferViewport, 0, 0,
                            (nScreenWidth - 1),
                            (nScreenHeight - 1));
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
        case SCAN_F2:
            bMouseCursorVisible = 0;
            if (bCockpitlessView == 0) {
                new_view(2, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0,
                            (nScreenWidth - 1),
                            (nScreenHeight - 1));
            initialize_view_buffer();
            new_view(2, 0);
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
            initialize_view_buffer();
            break;
        case SCAN_F3:
            bMouseCursorVisible = 0;
            if (bCockpitlessView == 0) {
                new_view(1, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0,
                            (nScreenWidth - 1),
                            (nScreenHeight - 1));
            initialize_view_buffer();
            new_view(1, 0);
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
            initialize_view_buffer();
            break;
        case SCAN_F4:
            bMouseCursorVisible = 0;
            if (bCockpitlessView == 0) {
                new_view(3, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0,
                            (nScreenWidth - 1),
                            (nScreenHeight - 1));
            initialize_view_buffer();
            new_view(3, 0);
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
            initialize_view_buffer();
            break;
        case SCAN_F5:
            bMouseCursorVisible = 0;
            if (bCockpitlessView == 0) {
                new_view(4, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0,
                            (nScreenWidth - 1),
                            (nScreenHeight - 1));
            initialize_view_buffer();
            new_view(4, 0);
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
            initialize_view_buffer();
            break;
        case SCAN_F6:
            bMouseCursorVisible = 0;
            if (bCockpitlessView == 0) {
                new_view(14, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0,
                            (nScreenWidth - 1),
                            (nScreenHeight - 1));
            initialize_view_buffer();
            new_view(14, 0);
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
            initialize_view_buffer();
            break;
        case SCAN_F7:
            if (acShipTargetTable[0] == -1)
                break;
            bMouseCursorVisible = 0;
            if (bCockpitlessView == 0) {
                new_view(7, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0,
                            (nScreenWidth - 1),
                            (nScreenHeight - 1));
            initialize_view_buffer();
            new_view(7, 0);
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
            initialize_view_buffer();
            break;
        case SCAN_F8:
            if (notRepeated) {
                int missileCameraEnabled;

                missileCameraEnabled = bMissileCameraEnabled ^ 1;
                bMouseCursorVisible = 0;
                bMissileCameraEnabled = missileCameraEnabled;
                if (missileCameraEnabled != 0)
                    SetHudMessageText(_("MISSILE CAMERA ON"),
                                      cRedColour, 20);
                else
                    SetHudMessageText(_("MISSILE CAMERA OFF"),
                                      cPrimaryTextColour, 20);
            }
            break;
        case SCAN_F9:
            if (notRepeated) {
                bMouseCursorVisible = 0;
                SelectNextExternalViewObject();
                if (bCockpitlessView == 0) {
                    force_view(4, cViewObject);
                    break;
                }
                GetScreenUpdateFlag();
                SetViewportRect(
                    &stSpaceBufferViewport, 0, 0,
                    (nScreenWidth - 1),
                    (nScreenHeight - 1));
                initialize_view_buffer();
                force_view(4, cViewObject);
                GetScreenUpdateFlag();
                SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
                initialize_view_buffer();
                break;
            }
            break;
        }
    }

    switch (bCurrentKey) {
    case SCAN_ESCAPE:
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
    case SCAN_MINUS:
    case SCAN_KP_MINUS:
        if (control != 0) {
            ReportFramesSkipped(-1);
            return 0;
        }
        accelerate(-1);
        return 0;
    case SCAN_EQUALS:
    case SCAN_KP_PLUS:
        if (control != 0) {
            ReportFramesSkipped(1);
            return 0;
        }
        accelerate(1);
        return 0;
    case SCAN_BACKSPACE:
        anShipSpeed[0] = 0;
        return 0;
    case SCAN_TAB:
    case SCAN_KP_MULTIPLY:
        your_afterburner();
        return 0;
    case SCAN_W:
        if (notRepeated) {
            SelectCockpitVduMode(0, 1);
            return 0;
        }
        break;
    case SCAN_T:
        if (notRepeated) {
            SelectCockpitVduMode(1, 3);
            return 0;
        }
        break;
    case SCAN_P:
        ShowGamePausedBanner((control == 0));
        SetFrameTimerPeriodDirect(1);
        return 0;
    case SCAN_RETURN:
        if (notRepeated &&
            (selectedWeapon = nSelectedReleaseWeaponIndex) != -1) {
            if (((ShipWeaponSlot *)&aShipWeaponsTable[0][1])[
                    selectedWeapon].type == OBJECT_TYPE_SPACE_MINE) {
                drop_player_mine(0);
                return 0;
            }
            if (nExternalViewShip == -1) {
                nExternalViewShip =
                    fire_missile(0);
                if (bMissileCameraEnabled == 1 &&
                    nExternalViewShip != -1) {
                    new_view(6, nExternalViewShip);
                    return 0;
                }
            }
        }
        break;
    case SCAN_M:
        if (notRepeated && control != 0) {
            if (nMusicVolumeSetting == 0)
                nMusicVolumeSetting = 20;
            else
                nMusicVolumeSetting = 0;
            SetMusicStreamVolume(
                anVolumeLevels[
                    nMusicVolumeSetting / 2]);
            ShowOnScreenMessage(0, 0, _("MUSIC VOLUME: %d."),
                                nMusicVolumeSetting / 2);
            return 0;
        }
        break;
    case SCAN_S:
        if (control != 0 && notRepeated) {
            if (nSfxVolumeSetting == 0)
                nSfxVolumeSetting = 20;
            else
                nSfxVolumeSetting = 0;
            SetSoundEffectsVolume(
                anVolumeLevels[
                    nSfxVolumeSetting / 2]);
            ShowOnScreenMessage(0, 0, _("SFX VOLUME: %d."),
                                nSfxVolumeSetting / 2);
            return 0;
        }
        break;
    case SCAN_D:
        if (notRepeated) {
            SelectCockpitVduMode(0, 2);
            return 0;
        }
        break;
    case SCAN_G:
        if (notRepeated) {
            SelectCockpitVduMode(0, 1);
            return 0;
        }
        break;
    case SCAN_J:
        if (control != 0) {
            CalibrateJoystickInteractive();
            return 0;
        }
        break;
    case SCAN_L:
        if (notRepeated) {
            nTargetLockMode =
                (nTargetLockMode == 0);
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
    case SCAN_BACKSLASH:
        accelerate(9000);
        return 0;
    case SCAN_V:
        if (notRepeated && control != 0) {
            ShowVersionBanner();
            return 0;
        }
        break;
    case SCAN_SPACE:
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
        cScreenViewMode,
        bCockpitlessView > 0,
        cPrimaryViewBufferColour);
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
        ((nArcadeWave + 6) * 400);
}

/* Function start: 0x429E90 */
void DrawArcadeScorePanel(short x, short y)
{
    char score[20];

    sprintf(score, "%0ld", nCurrentArcadeScore);
    DrawFormattedText(_("%X%YScore: %s0 %XTime: %u %X1 UP"),
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
            SetTextCursor(stSpaceBufferViewport.left,
                          ((stSpaceBufferViewport.top +
                                            stSpaceBufferViewport.bottom) / 2 - 5));
            if (nCurrentWave != -1) {
                FormatTextBufferFromStart(
                    _("Wave %d complete.\n\nBonus Points: %s0%P"),
                    nArcadeWave + 1, bonus);
                return;
            }
            FormatTextBufferFromStart(
                _("Mission %d complete.\n\nBonus Points: %s0%P"),
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
    savedViewport = stMouseCursorState.viewport;
    stMouseCursorState.viewport = &stSpaceBufferViewport;
    init_inflight_music();

    if (entryNavPoint == -1)
        entryNavPoint = aMissionShips[
            stCurrentMissionHeader.playerMissionShip].navPoint;
    set_up_action_sphere(entryNavPoint);

    if (bCockpitlessView != 0) {
        GetScreenUpdateFlag();
        SetViewportRect(&stSpaceBufferViewport, 0, 0,
                        (nScreenWidth - 1),
                        (nScreenHeight - 1));
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
    WarpMouseTo(((stSpaceBufferViewport.left +
                         stSpaceBufferViewport.right) / 2),
                ((stSpaceBufferViewport.top +
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
            ((unsigned int)liFlightFrameEndCounter.QuadPart -
                  (unsigned int)liFlightAfterCockpitCounter.QuadPart);
        nHudDelta =
            ((unsigned int)liFlightAfterCockpitCounter.QuadPart -
                  (unsigned int)liRenderDoneCounter.QuadPart);
        nDrawDelta =
            ((unsigned int)liRenderDoneCounter.QuadPart -
                  (unsigned int)liSimulationDone.QuadPart);
        nSimulationDelta =
            ((unsigned int)liSimulationDone.QuadPart -
                  (unsigned int)liFlightAfterInputCounter.QuadPart);
        nInputTicks =
            ((unsigned int)liFlightAfterInputCounter.QuadPart -
                  (unsigned int)liFrameStart.QuadPart);
        nFlightFrameTicks =
            ((unsigned int)liFlightFrameEndCounter.QuadPart -
                  (unsigned int)liFrameStart.QuadPart);
        nUnusedFlightProfileTicksA = 0;
        nUnusedFlightProfileTicksB = 0;
        nUnusedFlightProfileTicksC = 0;
    }

#ifdef SDL_PORT
    SdlSetMouseGrab(0);
    SdlCancelSpaceFrame();
    if (SdlUsingDosData() || SdlUsingTownsData())
        SdlStopDosSoundEffects();
#endif
    SetCinematicFrameTiming();
    SetViewportRect(&stSpaceBufferViewport, 0, 0,
                    (nScreenWidth - 1),
                    (nScreenHeight - 1));
    bCockpitlessView = 0;
    if (nArcadeState == 1)
        flag_objective(find_objective(1, -1), 2);
    bCockpitlessView = 0;
    ResetCockpitPaletteEntries();
    stMouseCursorState.viewport = savedViewport;
    free_inflight_music();
    SetEventManagerPump(0);
    bMouseCursorVisible = 0;
    QueueInputEvent(INPUT_EVENT_MOUSE_MOVE, 160, 100, 0, 0, 0, 0);
    SetMouseCursorShape(stMouseCursorState.shape, 0);
    return nArcadeState;
}

/* Objective-flag callee of the damage evaluation below. */
void flag_objective(short objective, unsigned char flags);

/* Function start: 0x42A520 */
int calculate_damage_level(void)
{
    ObjectTypeData *typeData;
    short damage;

    typeData = &aObjectTypeData[aeObjectTypeList[0]];
    damage = (((typeData->armorLeft -
                       aasShipArmor[0][2]) * 4) /
                     typeData->armorLeft);
    damage = (damage +
        ((typeData->armorRear - aasShipArmor[0][1]) * 4) /
            typeData->armorRear);
    damage = (damage +
        ((typeData->armorRight - aasShipArmor[0][3]) * 4) /
            typeData->armorRight);
    damage = (damage +
        ((typeData->armorFront - aasShipArmor[0][0]) * 4) /
            typeData->armorFront);
    damage = (
        (acShipDmg[0] * 30) / typeData->damageCapacity +
        damage * 2);
    damage = (damage +
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
    for (region = aTrainSimMissionRegions; region->frame != -1; region++) {
        if (IsPointInRect(mouseX, mouseY, &region->left) != 0)
            frame = region->frame;
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
    short target[3];
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
        paletteBytes = (activeCount * 6);
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

        GetPaletteEntry(colour, target);
        for (index = 0; index < activeCount; index++) {
            GetPaletteEntry(indices[index],
                            &currentPalette[index * 3]);
            memcpy(&targetPalette[index * 3], target, 6);
        }

        while (StepPaletteTransition(
                   currentPalette, targetPalette,
                   (activeCount * 3)) != 0) {
            for (paletteIndex = 0; paletteIndex < activeCount; paletteIndex++) {
                abPaletteTripletsArray[indices[paletteIndex]][0] =
                    currentPalette[paletteIndex * 3];
                abPaletteTripletsArray[indices[paletteIndex]][1] =
                    currentPalette[paletteIndex * 3 + 1];
                abPaletteTripletsArray[indices[paletteIndex]][2] =
                    currentPalette[paletteIndex * 3 + 2];
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
