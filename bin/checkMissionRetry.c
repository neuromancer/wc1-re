/* Exercise the checkpoint and real game input queue without game assets.
 * Only the modal drawing and mouse grab are replaced. */
#define ShowModalTextPanel TestShowPanel
#define ReleaseModalTextPanel TestReleasePanel
#define WaitForStreamInputKey TestReadKey
#define SdlSuspendMouseGrab TestSuspendGrab
#define SdlResumeMouseGrab TestResumeGrab
#include "../src/sdl/autosave.c"
#undef WaitForStreamInputKey

short WaitForStreamInputKey(void);
static SDL_Scancode keys[3];
static int keyIndex, panels, releases, grabDepth;
static int panelAvailable = 1;
static SDL_Joystick *testJoystick;
static int testButton, testKey;

static void Require(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "FAIL: %s\n", message);
        exit(1);
    }
}

short TestShowPanel(short font, const char *format, ...)
{
    Require(font == 0 && strlen(format) < 52, "prompt fits modal text buffer");
    Require(nMouseCursorShowCount == 0 && grabDepth == 1, "modal owns input");
    panels++;
    return panelAvailable;
}

void TestReleasePanel(void)
{
    releases++;
}

void TestSuspendGrab(void)
{
    grabDepth++;
}

void TestResumeGrab(void)
{
    grabDepth--;
}

short TestReadKey(void)
{
    SDL_Event event = {0};
    if (testJoystick) {
        Require(pCurrentEventManagerPump == NULL, "retry suspends legacy mouse/button polling");
        Require(SDL_JoystickSetVirtualButton(testJoystick, testButton, 1) == 0,
                "press virtual joystick button");
        SDL_JoystickUpdate();
        SdlPumpEvents();
        Require(pInputEventHead && pInputEventHead == pInputEventTail &&
                pInputEventHead->type == INPUT_EVENT_KEY_DOWN &&
                pInputEventHead->value == testKey, "one retry choice per physical press");
        SDL_JoystickSetVirtualButton(testJoystick, testButton, 0);
        SDL_JoystickUpdate();
        SdlPumpEvents();
        return WaitForStreamInputKey();
    }
    Require(keyIndex < SDL_arraysize(keys), "prompt accepts Enter/Esc");
    event.type = SDL_KEYDOWN;
    event.key.keysym.scancode = keys[keyIndex++];
    event.key.keysym.sym = SDL_GetKeyFromScancode(event.key.keysym.scancode);
    Require(SDL_PushEvent(&event) == 1, "queue keyboard event through SDL");
    return WaitForStreamInputKey();
}

static void CheckJoystick(int mapped)
{
    char guid[33], mapping[256];
    SDL_Event event = {0};
    SDL_JoystickID instance;
    int index, choice;

    index = SDL_JoystickAttachVirtual(mapped ? SDL_JOYSTICK_TYPE_GAMECONTROLLER :
                                     SDL_JOYSTICK_TYPE_FLIGHT_STICK, 2, 6, 1);
    Require(index >= 0, "attach virtual controller/joystick");
    if (mapped) {
        SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(index), guid, sizeof(guid));
        /* Deliberately map A/B to different physical indices from raw buttons 1/2. */
        SDL_snprintf(mapping, sizeof(mapping),
            "%s,Retry probe,a:b2,b:b3,back:b4,start:b5,leftx:a0,lefty:a1,", guid);
        Require(SDL_GameControllerAddMapping(mapping) >= 0, "map virtual gamepad");
    }
    Require(SDL_IsGameController(index) == mapped, "expected mapped/raw device type");
    testJoystick = SDL_JoystickOpen(index);
    Require(testJoystick != NULL, "open virtual joystick");
    instance = SDL_JoystickInstanceID(testJoystick);
    nCurrentActiveInputDevice = -1;
    SdlPumpEvents();
    stCampaignState.missionScore = 7;
    SdlCaptureMissionCheckpoint();
    for (choice = 0; choice < 2; choice++) {
        stCampaignState.missionScore = 42;
        testButton = choice + (mapped ? 2 : 0);
        testKey = choice ? VK_ESCAPE : VK_RETURN;
        SetEventManagerPump(PollMenuInputDevices);
        Require(SdlRetryMission() == !choice, "gamepad A/B and raw buttons 1/2 select retry/continue");
        Require(stCampaignState.missionScore == (choice ? 42 : missionCheckpoint.missionScore),
                "controller choice restores or preserves the outcome");
        Require(pCurrentEventManagerPump == PollMenuInputDevices &&
                nCurrentActiveInputDevice == -1, "restore polling without changing flight device");
        SetEventManagerPump(NULL);
        SdlHandleJoystickButtonEvent(instance, mapped ? SDL_CONTROLLER_BUTTON_A : 0, 1, mapped);
        Require(!pInputEventHead, "retry binding disabled after dialog exit");

        /* The SWC host translates the same buttons through its own event loop. */
        SdlSetJoystickRetryMode(1);
        event.type = mapped ? SDL_CONTROLLERBUTTONDOWN : SDL_JOYBUTTONDOWN;
        event.jbutton.which = instance;
        event.jbutton.button = mapped ? (choice ? SDL_CONTROLLER_BUTTON_B : SDL_CONTROLLER_BUTTON_A) : choice;
        SdlTranslateSwcJoystickEvent(&event, 0);
        Require(event.type == SDL_KEYDOWN &&
                event.key.keysym.sym == (choice ? SDLK_ESCAPE : SDLK_RETURN),
                "SWC uses the same retry/continue bindings");
        SdlSetJoystickRetryMode(0);
    }
    /* A failed panel must also release the temporary bindings. */
    panelAvailable = 0;
    Require(!SdlRetryMission(), "unavailable panel does not accept controller input");
    panelAvailable = 1;
    SdlHandleJoystickButtonEvent(instance, mapped ? SDL_CONTROLLER_BUTTON_A : 0, 1, mapped);
    Require(!pInputEventHead, "allocation failure restores controller mode");
    /* An input queued to skip the preceding scene must not choose retry. */
    event.type = mapped ? SDL_CONTROLLERBUTTONDOWN : SDL_JOYBUTTONDOWN;
    event.jbutton.which = instance;
    event.jbutton.button = 0;
    SDL_PushEvent(&event);
    SdlSetJoystickRetryMode(1);
    Require(!SDL_HasEvent(event.type), "discard button presses queued before the prompt");
    SdlSetJoystickRetryMode(0);
    SdlShutdownJoysticks();
    SDL_JoystickClose(testJoystick);
    testJoystick = NULL;
    Require(SDL_JoystickDetachVirtual(index) == 0, "detach virtual joystick");
    SdlPumpEvents();
}

int main(void)
{
    CampaignState before, failed;
    PilotRecord pilots[9];
    int attempt, shown;

    SDL_setenv("SDL_VIDEODRIVER", "dummy", 1);
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    Require(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) == 0,
            "initialize headless SDL input");
    nEventManagerActive = 1;
    bInputEventPoolInitialized = 1;
    stMouseCursorState.viewport = &stModalSrcViewport;
    nMouseCursorShowCount = 3;
    SdlConfigureAutosave(0);
    ResetCampaignData();
    stCampaignState.currentSeries = 2;
    stCampaignState.currentMission = 1;
    stCampaignState.promotionScore = 7;
    before = stCampaignState;
    memcpy(pilots, aPilotEntriesList, sizeof(pilots));
    SdlCaptureMissionCheckpoint();

    for (attempt = 0; attempt < 3; attempt++) {
        stCampaignState.currentMission++;
        stCampaignState.missionScore = 99;
        stCampaignState.seriesScore = 99;
        stCampaignState.promotionScore = 0;
        stCampaignState.elapsedDate.year++;
        stCampaignState.personalityDeathMission[2] = 3;
        stCampaignState.aceFlags[1] = 1;
        stCampaignState.medals[3] = 1;
        aPilotEntriesList[8].kills++;
        aPilotEntriesList[8].rank++;
        aPilotEntriesList[2].missions++;
        bPlayerEjectedThisMission = bPromotionPending = bOfficeVisitPending = 1;
        bPlayerShipTypeChanged = bEscapePressedFlag = 1;
        nPendingMedalIndex = 3;
        nPostSeriesSequence = 64;
        nPlayerCollisionObject = 2;
        nArcadeState = 4;
        keys[0] = SDL_SCANCODE_SPACE;
        keys[1] = SDL_SCANCODE_RETURN;
        keyIndex = 0;
        /* The death music plays from the flight stream, with a trigger
           possibly still queued; retrying must stop and clear both. */
        nMusicPlaybackMode = 1;
        nCurrentMusicTrack = 32;
        bIxAudioEnabled = 1;
        ix_streamer_set_trigger(32);
        Require(SdlRetryMission(), "accept retry repeatedly with autosave disabled");
        Require(nCurrentMusicTrack == -1 && ix_streamer_get_trigger() == -1,
                "retry stops the death music and drops its pending stream trigger");
        bIxAudioEnabled = 0;
        Require(keyIndex == 2, "ignore unrelated keys");
        Require(!memcmp(&stCampaignState, &before, sizeof(before)) &&
                !memcmp(aPilotEntriesList, pilots, sizeof(pilots)),
                "restore scores, pilots, deaths, aces, medals and mission/date");
        Require(stCampaignState.currentPilot == &aPilotEntriesList[8] &&
                apWingmanPilotsList[2] == &aPilotEntriesList[2] &&
                pCurrentCampaignDate == &stCampaignState.currentDate,
                "campaign pointers remain valid");
        Require(!bPlayerEjectedThisMission && !bPromotionPending && !bOfficeVisitPending &&
                !bPlayerShipTypeChanged && !bEscapePressedFlag && !nArcadeState &&
                nPendingMedalIndex == -1 && nPostSeriesSequence == -1 &&
                nPlayerCollisionObject == -1, "discard pending failed-flight outcomes");
    }
    stCampaignState.missionScore = 42;
    aPilotEntriesList[8].kills = 17;
    nArcadeState = 2;
    failed = stCampaignState;
    keys[0] = SDL_SCANCODE_ESCAPE;
    keyIndex = 0;
    nCurrentMusicTrack = 32;
    Require(!SdlRetryMission(), "Escape declines retry");
    Require(nCurrentMusicTrack == 32, "declining keeps the death music for the funeral");
    panelAvailable = 0;
    Require(!SdlRetryMission(), "panel allocation failure declines retry");
    Require(!memcmp(&stCampaignState, &failed, sizeof(failed)) &&
            aPilotEntriesList[8].kills == 17 && nArcadeState == 2,
            "decline preserves the normal postflight outcome");
    Require(grabDepth == 0 && nMouseCursorShowCount == 3 &&
            panels == 5 && releases == 4, "restore modal/cursor state on every exit");

    panelAvailable = 1;
    CheckJoystick(0);
    CheckJoystick(1);
    shown = panels;
    nTrainSimActive = 1;
    SdlCaptureMissionCheckpoint();
    Require(!SdlRetryMission() && panels == shown, "simulator has no campaign retry");
    nTrainSimActive = 0;
    stCampaignState.currentSeries = 0;
    SdlCaptureMissionCheckpoint();
    Require(!SdlRetryMission() && panels == shown, "no retry before the campaign starts");
    stCampaignState.currentSeries = 3;
    SdlCaptureMissionCheckpoint();
    stCampaignState.currentSeries = 4;
    SdlRestoreMissionCheckpoint();
    Require(stCampaignState.currentSeries == 3, "new mission replaces old checkpoint");
    SdlResetAutosave();
    Require(!SdlRetryMission() && panels == shown, "return to title invalidates checkpoint");
    puts("PASS: keyboard/gamepad/raw joystick retry, SWC bindings, repeated restoration and cleanup.");
    SDL_Quit();
    return 0;
}
