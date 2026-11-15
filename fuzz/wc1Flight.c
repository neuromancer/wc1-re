/* WC1, SM1 and SM2 for the flight fuzzer (flight.h): RunSpaceFlight driven
 * through the port's SDL input. runFuzzer.py runs it on a disposable copy of
 * the player's Kilrathi Saga installation. */
#include "flight.h"

/* Keys and mouse events reach player_input and HandleSpaceFlightControls
 * through the port's SDL translation; Shift and Ctrl combine with held keys.
 * Pause, Escape, quit and the inflight computer are modal and stay out. */
const FlightAction flightActions[] = {
    {ACTION_NONE, SDL_SCANCODE_UNKNOWN, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_UP, 0, 0}, {ACTION_KEY, SDL_SCANCODE_DOWN, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_LEFT, 0, 0}, {ACTION_KEY, SDL_SCANCODE_RIGHT, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_HOME, 0, 0}, {ACTION_KEY, SDL_SCANCODE_PAGEUP, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_END, 0, 0}, {ACTION_KEY, SDL_SCANCODE_PAGEDOWN, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_COMMA, 0, 0}, {ACTION_KEY, SDL_SCANCODE_PERIOD, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_INSERT, 0, 0}, {ACTION_KEY, SDL_SCANCODE_DELETE, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_SPACE, 0, 0}, {ACTION_KEY, SDL_SCANCODE_EQUALS, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_MINUS, 0, 0}, {ACTION_KEY, SDL_SCANCODE_TAB, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_BACKSPACE, 0, 0}, {ACTION_KEY, SDL_SCANCODE_BACKSLASH, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_LSHIFT, 0, 0}, {ACTION_KEY, SDL_SCANCODE_LCTRL, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_RETURN, 0, 0}, {ACTION_KEY, SDL_SCANCODE_G, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_W, 0, 0}, {ACTION_KEY, SDL_SCANCODE_T, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_L, 0, 0}, {ACTION_KEY, SDL_SCANCODE_D, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_C, 0, 0}, {ACTION_KEY, SDL_SCANCODE_H, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_B, 0, 0}, {ACTION_KEY, SDL_SCANCODE_A, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_N, 0, 0}, {ACTION_KEY, SDL_SCANCODE_V, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_E, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_1, 0, 0}, {ACTION_KEY, SDL_SCANCODE_2, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_3, 0, 0}, {ACTION_KEY, SDL_SCANCODE_4, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_5, 0, 0}, {ACTION_KEY, SDL_SCANCODE_6, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_7, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_F1, 0, 0}, {ACTION_KEY, SDL_SCANCODE_F2, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_F3, 0, 0}, {ACTION_KEY, SDL_SCANCODE_F4, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_F5, 0, 0}, {ACTION_KEY, SDL_SCANCODE_F6, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_F7, 0, 0}, {ACTION_KEY, SDL_SCANCODE_F8, 0, 0},
    {ACTION_KEY, SDL_SCANCODE_F9, 0, 0},
    {ACTION_POINTER, SDL_SCANCODE_UNKNOWN, 40, 25},
    {ACTION_POINTER, SDL_SCANCODE_UNKNOWN, 160, 25},
    {ACTION_POINTER, SDL_SCANCODE_UNKNOWN, 280, 25},
    {ACTION_POINTER, SDL_SCANCODE_UNKNOWN, 40, 100},
    {ACTION_POINTER, SDL_SCANCODE_UNKNOWN, 160, 100},
    {ACTION_POINTER, SDL_SCANCODE_UNKNOWN, 280, 100},
    {ACTION_POINTER, SDL_SCANCODE_UNKNOWN, 40, 175},
    {ACTION_POINTER, SDL_SCANCODE_UNKNOWN, 160, 175},
    {ACTION_POINTER, SDL_SCANCODE_UNKNOWN, 280, 175},
    {ACTION_BUTTONS, SDL_SCANCODE_UNKNOWN, SDL_BUTTON_LMASK, 0},
    {ACTION_BUTTONS, SDL_SCANCODE_UNKNOWN, SDL_BUTTON_RMASK, 0},
    {ACTION_BUTTONS, SDL_SCANCODE_UNKNOWN, SDL_BUTTON_LMASK | SDL_BUTTON_RMASK, 0},
    {ACTION_WHEEL, SDL_SCANCODE_UNKNOWN, 1, 0}, {ACTION_WHEEL, SDL_SCANCODE_UNKNOWN, -1, 0}
};
const size_t flightActionCount = SDL_arraysize(flightActions);

typedef char FlightActionsFitSixBits[SDL_arraysize(flightActions) <= 64 ? 1 : -1];

/* Throttle, steer while firing, aim and click, cycle weapons, fire a
 * missile, call a wingman, light the afterburner, then try autopilot. */
const FlightSeedStep flightSeed[] = {
    {{ACTION_KEY, SDL_SCANCODE_EQUALS, 0, 0}, 0, 31},
    {{ACTION_KEY, SDL_SCANCODE_UP, 0, 0}, 0, 15},
    {{ACTION_KEY, SDL_SCANCODE_SPACE, 0, 0}, 1, 31},
    {{ACTION_POINTER, SDL_SCANCODE_UNKNOWN, 160, 25}, 0, 7},
    {{ACTION_BUTTONS, SDL_SCANCODE_UNKNOWN, SDL_BUTTON_LMASK, 0}, 0, 15},
    {{ACTION_KEY, SDL_SCANCODE_G, 0, 0}, 0, 0},
    {{ACTION_KEY, SDL_SCANCODE_W, 0, 0}, 0, 0},
    {{ACTION_KEY, SDL_SCANCODE_RETURN, 0, 0}, 0, 7},
    {{ACTION_KEY, SDL_SCANCODE_C, 0, 0}, 0, 3},
    {{ACTION_KEY, SDL_SCANCODE_1, 0, 0}, 0, 15},
    {{ACTION_KEY, SDL_SCANCODE_TAB, 0, 0}, 0, 15},
    {{ACTION_KEY, SDL_SCANCODE_A, 0, 0}, 0, 31}
};
const size_t flightSeedLength = SDL_arraysize(flightSeed);

static Uint8 keys[SDL_NUM_SCANCODES];
static Uint32 buttons;
static int pointerX = 160, pointerY = 100;
static SDL_Scancode repeatedKey = SDL_SCANCODE_UNKNOWN;
static unsigned char initialVideoImagesSuppressed;
static short initialFrameSkip;

/* The port's button events add or remove one button from SDL's own mouse
 * state, which pushed events never change; report the driver's buttons. */
Uint32 SDL_GetMouseState(int *x, int *y)
{
    if (x != NULL)
        *x = pointerX;
    if (y != NULL)
        *y = pointerY;
    return buttons;
}

static void QueueFlightKey(SDL_Scancode code, int pressed)
{
    SDL_Event event = {0};
    if (code == SDL_SCANCODE_UNKNOWN)
        return;
    keys[code] = pressed != 0;
    event.type = pressed ? SDL_KEYDOWN : SDL_KEYUP;
    event.key.state = pressed ? SDL_PRESSED : SDL_RELEASED;
    event.key.repeat = pressed == 2;
    event.key.keysym.scancode = code;
    event.key.keysym.sym = SDL_GetKeyFromScancode(code);
    Require(SDL_PushEvent(&event) == 1, "queue flight key through SDL");
}

/* Window ID 0 has no window to map from, so the port takes these positions
 * as its 320x200 coordinates. */
static void QueueMouseEvent(Uint32 type, Uint8 button, int wheel)
{
    SDL_Event event = {0};

    event.type = type;
    if (type == SDL_MOUSEMOTION) {
        event.motion.state = buttons;
        event.motion.x = pointerX;
        event.motion.y = pointerY;
    } else if (type == SDL_MOUSEWHEEL) {
        event.wheel.y = wheel;
        event.wheel.direction = SDL_MOUSEWHEEL_NORMAL;
    } else {
        event.button.button = button;
        event.button.state = type == SDL_MOUSEBUTTONDOWN ? SDL_PRESSED : SDL_RELEASED;
        event.button.clicks = 1;
        event.button.x = pointerX;
        event.button.y = pointerY;
    }
    Require(SDL_PushEvent(&event) == 1, "queue mouse event through SDL");
}

/* N on the navigation display opens the inflight computer, which waits for
 * its own input (SelectCockpitVduMode, mode 5). */
static int OpensInflightComputer(SDL_Scancode code)
{
    return code == SDL_SCANCODE_N && get_mode(1) == 5;
}

void ApplyFlightAction(const FlightAction *action)
{
    SDL_Scancode code = (SDL_Scancode)action->key;
    Uint32 pressed;

    switch (action->kind) {
    case ACTION_KEY:
        if (!keys[code] && !OpensInflightComputer(code)) {
            QueueFlightKey(code, 1);
            repeatedKey = code;
        }
        break;
    case ACTION_POINTER:
        pointerX = action->x;
        pointerY = action->y;
        QueueMouseEvent(SDL_MOUSEMOTION, 0, 0);
        break;
    case ACTION_BUTTONS:
        pressed = (Uint32)action->x & ~buttons;
        buttons |= (Uint32)action->x;
        if (pressed & SDL_BUTTON_LMASK)
            QueueMouseEvent(SDL_MOUSEBUTTONDOWN, SDL_BUTTON_LEFT, 0);
        if (pressed & SDL_BUTTON_RMASK)
            QueueMouseEvent(SDL_MOUSEBUTTONDOWN, SDL_BUTTON_RIGHT, 0);
        break;
    case ACTION_WHEEL:
        QueueMouseEvent(SDL_MOUSEWHEEL, 0, action->x);
        break;
    default:
        break;
    }
}

void ReleaseFlightControls(void)
{
    int code;

    for (code = 0; code < SDL_NUM_SCANCODES; code++) {
        if (keys[code])
            QueueFlightKey((SDL_Scancode)code, 0);
    }
    if (buttons & SDL_BUTTON_LMASK) {
        buttons &= ~SDL_BUTTON_LMASK;
        QueueMouseEvent(SDL_MOUSEBUTTONUP, SDL_BUTTON_LEFT, 0);
    }
    if (buttons & SDL_BUTTON_RMASK) {
        buttons &= ~SDL_BUTTON_RMASK;
        QueueMouseEvent(SDL_MOUSEBUTTONUP, SDL_BUTTON_RIGHT, 0);
    }
    repeatedKey = SDL_SCANCODE_UNKNOWN;
}

int FlightContinues(void)
{
    return !nArcadeState;
}

/* One RunSpaceFlight frame. */
void FlightStep(int frame)
{
    /* Keyboard repeat applies to the most recent key. */
    if (frame > 0 && frame % 4 == 0 && repeatedKey != SDL_SCANCODE_UNKNOWN &&
        keys[repeatedKey] && !OpensInflightComputer(repeatedKey))
        QueueFlightKey(repeatedKey, 2);
    Require(HandleSpaceFlightControls() != -1, "flight controls continue");
    if (!nArcadeState) {
        Update_3Space();
        RenderSpaceViewFrame();
        update_cockpit();
    }
}

void ViewFlightArrival(void)
{
    set_eye_direction_and_position();
    Draw_3Space_Frame();
}

void StartFlightGame(void)
{
    SDL_Window *window;

    SDL_SetMainReady();
    Require(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) == 0, "SDL init");
    window = SDL_CreateWindow("flight fuzzer", 0, 0, 640, 400, SDL_WINDOW_HIDDEN);
    Require(window != NULL, "headless window");
    DIBinstall(window);
    hMainWindow = window;
    SdlStartEventPump();
    bIxAudioEnabled = 0;
    bCurrentRunningFromHardDiskFlag = 1;
    nMusicPlaybackMode = 0;
    Require(chdir("GAMEDAT") == 0, "enter assets");
    LoadInstallDat();
    Require(!SdlUsingDosData() && !SdlUsingTownsData(), "Kilrathi Saga data");
    Require(chdir("..") == 0, "return to installation root");
    ResetCampaignData();
    srand(0);
    InitGameClockEpoch();
    nVideoMode = 0x13;
    LoadOriginFxDrivers();
    /* As initialize(): the HUD and the simulator's score panel draw with these. */
    InitializeGameTextContexts();
    nFrameIntervalMs = 0;
    initialVideoImagesSuppressed = bVideoImagesSuppressed;
    initialFrameSkip = nFrameSkip;
    SdlSetKeyboardSnapshot(keys);
    SDL_ClearError(); /* Dummy-driver setup errors are not failures. */
}

int LoadFlightCampaign(int campaign, unsigned char counts[14])
{
    static const int seriesCounts[] = {13, 8, 9};
    int series;

    if (campaign != 0 && !SdlHasSecretMissions(campaign))
        return 0;
    LoadPacketIntoBuffer(asCampaignPilotFiles[campaign], 1, pMissionCampaignInfo);
    for (series = 1; series <= seriesCounts[campaign]; series++)
        counts[series] = pMissionCampaignInfo[(series - 1) * 90 + 2];
    return 1;
}

void BeginFlight(int campaign, int series, int mission)
{
    int simulator = campaign == FLIGHT_SIMULATOR;
    int entry;

    InitGameClockEpoch();
    ResetCampaignData();
    memset(keys, 0, sizeof(keys));
    buttons = 0;
    pointerX = 160;
    pointerY = 100;
    repeatedKey = SDL_SCANCODE_UNKNOWN;
    SdlPumpEvents();
    FlushInputEvents();
    ClearInputKeyStatePreservingModifiers();
    bCurrentKey = cPreviousKey = SCAN_NONE;
    bHostPrimaryMouseButton = bHostSecondaryMouseButton = 0;
    /* Settings that flight keys toggle and no mission resets. */
    bVideoImagesSuppressed = initialVideoImagesSuppressed;
    nFrameSkip = initialFrameSkip;
    stCampaignState.campaignIndex = nCampaignDataSet = simulator ? 0 : campaign;
    if (campaign == 2)
        SdlSetSm2Pilots(aPilotEntriesList, 1);
    LoadPacketIntoBuffer(asCampaignPilotFiles[nCampaignDataSet], 0, pConstellationDefinitionsPtr);
    LoadPacketIntoBuffer(asCampaignPilotFiles[nCampaignDataSet], 1, pMissionCampaignInfo);
    stCampaignState.currentSeries = series;
    stCampaignState.currentMission = mission;
    nTrainSimActive = 0;
    nCurrentActiveInputDevice = -1;
    /* GameFlow enables this before flight: each key then queues a virtual-key
     * event ahead of its scan code, and player_input handles all but the first. */
    bKeyEventQueueEnabled = 1;
    nArcadeState = 0;
    bCockpitlessView = 0;
    nCameraViewMode = -1;
    if (simulator) {
        /* As RunTrainSim before RunSpaceFlight(nArcadeWave). */
        nArcadeWave = 0;
        nCurrentArcadeScore = 0;
        nArcadeBonusCountdown = 0;
        cCurrentCockpitView = 4;
        cCockpitLogicalFile = 21;
        nCannedSceneMode = 0;
        nTrainSimActive = 1;
        nTrainSimMission = mission;
        FigureArcadeTime();
        Require(init_mission(0, mission) == 0, "initialize simulator mission");
        ShowGetReadyScreen(); /* 40 frames; its force_view sets up the space view. */
        InvalidateVduMode(0);
        InvalidateVduMode(1);
        entry = nArcadeWave;
    } else {
        /* As GameFlow: init_mission, LaunchPlayerShip, RunSpaceFlight(-1). */
        Require(init_mission(series, mission) == 0, "initialize mission");
        /* Campaigns share one process; only SM2 replaces the Spikeri. */
        Require(!strcmp(aObjectTypeData[OBJECT_TYPE_SPIKERI].displayName, "Hriss") ==
                    (campaign == 2), "campaign ship definitions");
        bEscapePressedFlag = 1; /* Skip only the launch animation. */
        LaunchPlayerShip();
        entry = aMissionShips[stCurrentMissionHeader.playerMissionShip].navPoint;
    }
    /* RunSpaceFlight's setup, without window grabs, music or frame pacing.
     * Its pointer warp would consume bPtrMovedByKeyboardState; none happens. */
    nFrameSkipCounter = 1;
    bInputModeState = 1;
    SetEventManagerPump(get_player_input);
    set_up_action_sphere(entry);
    copy_frame(0, 62);
    bMouseAfterburnerControl = 0;
    bMouseCursorVisible = 0;
    nArcadeState = 0;
    FlushInputEvents();
    ClearDebugPauseFlags();
    ClearInputKeyStatePreservingModifiers();
    bCurrentKey = cPreviousKey = SCAN_NONE;
    bPtrMovedByKeyboardState = 0;
}

void EndFlight(void)
{
    SdlPumpEvents();
    FlushInputEvents();
    ClearInputKeyStatePreservingModifiers();
    SetEventManagerPump(NULL);
    free_cockpit();
    free_all_slots();
    free_3Space();
}
