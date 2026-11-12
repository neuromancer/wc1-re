/* libFuzzer-driven WC1/SM1/SM2 flight controls; use runGameTests.py --fuzz.
 * Input: campaign (0-2, or 3 for the training simulator), series (1-based;
 * 0 for the simulator), mission (0-based), start (bits 0-3: flight-path legs
 * travelled before control begins), little-endian PRNG seed (4 bytes), then
 * pairs of action and hold bytes. Action bits 0-5 select a control; bit 6
 * keeps the controls already held. Hold bits 0-5 give 1..64 ticks and bits
 * 6-7 multiply them by 1, 2, 4 or 8. Assets come from a disposable copy of
 * the player's installation. WC1_FLIGHT_TRACE prints where each input started
 * and ended. */
#include "wc1.h"

enum { ACTION_NONE, ACTION_KEY, ACTION_POINTER, ACTION_BUTTONS, ACTION_WHEEL };

typedef struct FlightAction {
    unsigned char kind;
    SDL_Scancode key;
    short x; /* pointer x, button mask or wheel step */
    short y;
} FlightAction;

/* Keys and mouse events reach player_input and HandleSpaceFlightControls
 * through the port's SDL translation; Shift and Ctrl combine with held keys.
 * Pause, Escape, quit and the inflight computer are modal and stay out. */
static const FlightAction actions[] = {
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

typedef char FlightActionsFitSixBits[SDL_arraysize(actions) <= 64 ? 1 : -1];

static const char *const campaignNames[] = {"wc1", "sm1", "sm2", "sim"};
static Uint8 keys[SDL_NUM_SCANCODES];
static Uint32 buttons;
static int pointerX = 160, pointerY = 100;
static SDL_Scancode repeatedKey = SDL_SCANCODE_UNKNOWN;
static unsigned char missionCounts[3][13];
static int simulatorSelected;
static int campaignFilter = -1, seriesFilter = -1, missionFilter = -1;
static int tickLimit = 600;
static int inputActive;
static int traceInputs;
static unsigned char initialVideoImagesSuppressed;
static short initialFrameSkip;

static void Require(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "FAIL: %s (%s)\n", message, SDL_GetError());
        abort();
    }
}

static void CheckFuzzExit(void)
{
    /* Original fatal errors may call exit(0); make libFuzzer retain the input. */
    Require(!inputActive, "game exited during flight input or initialization");
}

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

static void ApplyFlightAction(const FlightAction *action)
{
    Uint32 pressed;

    switch (action->kind) {
    case ACTION_KEY:
        if (!keys[action->key] && !OpensInflightComputer(action->key)) {
            QueueFlightKey(action->key, 1);
            repeatedKey = action->key;
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

static void ReleaseFlightControls(void)
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

/* One RunSpaceFlight frame. */
static void FlightStep(void)
{
    Require(HandleSpaceFlightControls() != -1, "flight controls continue");
    if (!nArcadeState) {
        Update_3Space();
        RenderSpaceViewFrame();
        update_cockpit();
    }
}

/* Escort waits per input: three minutes at the default 20 frames per second. */
#define ESCORT_WAIT_TICKS 3600

/* Reach a later part of the mission as autopilot arrival would: the player
 * stops short of the next flight-path objective with the formation that
 * auto_pilot_sequence takes along and enters that nav point's sphere, and
 * the game's own objective checks register the arrival, advancing the flight
 * path and applying the nav triggers. No controls are held meanwhile. */
static void TravelFlightPath(int legs)
{
    FixedVector destination;
    signed char travelling[10];
    short formationSlot;
    short ship;
    int previousLeg;
    int tick;
    int waited = 0;

    while (legs-- > 0 && !nArcadeState && cCurrentNavPtIdx >= 0 &&
           cCurrentNavPtIdx < (int)SDL_arraysize(abFlightPathList) &&
           abFlightPathList[cCurrentNavPtIdx] != -1) {
        previousLeg = cCurrentNavPtIdx;
        destination = aMissionObjectivesArray[abFlightPathList[cCurrentNavPtIdx]].position;
        if ((unsigned short)distance_from_point(0, &destination) >= 1500) {
            for (ship = 0; ship < 10; ship++)
                travelling[ship] = ship != 0 && aeObjectClass[ship] >= OBJECT_CLASS_SHIP &&
                    aeSpecialManeuvers[ship] != SPECIAL_MANEUVER_UNKNOWN_9 &&
                    aeShipSide[ship] == SIDE_IMPERIAL &&
                    is_team_member(nShipMissionIndices[ship]) && !kilrathi_near(ship, 10000);
            point_at(0, destination);
            aShipPositionsArray[0] = destination;
            position_relative_ijk(&aShipPositionsArray[0], 0, 0, 0, -1000);
            formationSlot = 0;
            for (ship = 1; ship < 10; ship++) {
                if (!travelling[ship])
                    continue;
                auto_position(ship, &formationSlot);
                copy_frame(0, ship);
                anShipSpeed[ship] = anShipSpeed[0];
                aShipVelocity[ship] = aShipVelocity[0];
            }
            /* Enter the sphere from the arrival view, as house_keep does after
               manual flight: set_up_action_sphere explodes the ships it removes
               while they are on screen, so stale departure positions would
               destroy ships left far behind. */
            set_eye_direction_and_position();
            Draw_3Space_Frame();
            ReleaseStaleNavTarget();
        }
        /* update_objective_location checks one objective per frame. */
        for (tick = 0; tick <= cMissionObjectiveCountValue && !nArcadeState; tick++)
            FlightStep();
        /* Wait there like the player: flag_reached holds the leg while an
           escorted ship is on its way, and that ship registers the arrival
           itself (cruise_to_destination's flag_reached(..., 1)). */
        for (tick = 0; waited < ESCORT_WAIT_TICKS && cCurrentNavPtIdx == previousLeg &&
             escorting_a_ship() && find_ship_index(anShipMissionShips[0]) != -1 &&
             !nArcadeState; tick++, waited++)
            FlightStep();
        if (cCurrentNavPtIdx == previousLeg) {
            /* An inactive nav point, objectives still hidden, or a late escort. */
            if (traceInputs)
                fprintf(stderr, "TRACE leg %d stops at objective %d (type %d, range %d) "
                        "after waiting %d ticks\n", previousLeg, abFlightPathList[previousLeg],
                        aMissionObjectivesArray[abFlightPathList[previousLeg]].type,
                        distance_from_point(0, &aMissionObjectivesArray[
                            abFlightPathList[previousLeg]].position), tick);
            break;
        }
    }
}

static int ActionIndex(unsigned char kind, SDL_Scancode key, short x)
{
    int index;

    for (index = 0; index < (int)SDL_arraysize(actions); index++) {
        if (actions[index].kind == kind && actions[index].key == key &&
            ((kind != ACTION_POINTER && kind != ACTION_BUTTONS) || actions[index].x == x))
            return index;
    }
    Require(0, "seed action");
    return 0;
}

static void WriteSeed(const char *directory, int campaign, int series, int mission, int legs)
{
    char path[PATH_MAX];
    FILE *file;
    unsigned char seed[32] = {
        (unsigned char)campaign, (unsigned char)series, (unsigned char)mission,
        (unsigned char)legs, 0, 0, 0, 0
    };
    size_t size = 8;

    /* Throttle, steer while firing, aim and click, cycle weapons, fire a
     * missile, call a wingman, light the afterburner, then try autopilot. */
    seed[size++] = ActionIndex(ACTION_KEY, SDL_SCANCODE_EQUALS, 0); seed[size++] = 31;
    seed[size++] = ActionIndex(ACTION_KEY, SDL_SCANCODE_UP, 0); seed[size++] = 15;
    seed[size++] = ActionIndex(ACTION_KEY, SDL_SCANCODE_SPACE, 0) | 0x40; seed[size++] = 31;
    seed[size++] = ActionIndex(ACTION_POINTER, SDL_SCANCODE_UNKNOWN, 160); seed[size++] = 7;
    seed[size++] = ActionIndex(ACTION_BUTTONS, SDL_SCANCODE_UNKNOWN, SDL_BUTTON_LMASK);
    seed[size++] = 15;
    seed[size++] = ActionIndex(ACTION_KEY, SDL_SCANCODE_G, 0); seed[size++] = 0;
    seed[size++] = ActionIndex(ACTION_KEY, SDL_SCANCODE_W, 0); seed[size++] = 0;
    seed[size++] = ActionIndex(ACTION_KEY, SDL_SCANCODE_RETURN, 0); seed[size++] = 7;
    seed[size++] = ActionIndex(ACTION_KEY, SDL_SCANCODE_C, 0); seed[size++] = 3;
    seed[size++] = ActionIndex(ACTION_KEY, SDL_SCANCODE_1, 0); seed[size++] = 15;
    seed[size++] = ActionIndex(ACTION_KEY, SDL_SCANCODE_TAB, 0); seed[size++] = 15;
    seed[size++] = ActionIndex(ACTION_KEY, SDL_SCANCODE_A, 0); seed[size++] = 31;
    snprintf(path, sizeof(path), "%s/%s-%d-%d-leg%d", directory, campaignNames[campaign],
             series, mission, legs);
    file = fopen(path, "rb");
    if (file != NULL) {
        fclose(file);
        return;
    }
    file = fopen(path, "wb");
    Require(file != NULL && fwrite(seed, size, 1, file) == 1 && fclose(file) == 0,
            "write flight seed");
}

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    SDL_Window *window;
    const char *scratch = getenv("WC1_TEST_ROOT");
    const char *filter = getenv("WC1_FLIGHT_CASE");
    const char *campaignName = getenv("WC1_FLIGHT_CAMPAIGN");
    const char *ticks = getenv("WC1_FLIGHT_TICKS");
    const char *seeds = getenv("WC1_FLIGHT_SEEDS");
    /* Changed flight keys, as the launcher passes them (src/sdl/keymap.c). */
    const char *keyBindings = getenv("WC1_FLIGHT_KEYS");
    const int seriesCounts[] = {13, 8, 9};
    int campaign, series, mission, legs, cases = 0;
    int list = getenv("WC1_FLIGHT_LIST") != NULL;
    (void)argc;
    (void)argv;

    Require(atexit(CheckFuzzExit) == 0, "register early-exit check");
    inputActive = 1;
    traceInputs = getenv("WC1_FLIGHT_TRACE") != NULL;
    if (keyBindings != NULL)
        SdlSetKeyBindings(keyBindings);
    Require(scratch != NULL, "use runGameTests.py --fuzz to prepare disposable data");
    if (campaignName)
        campaignFilter = atoi(campaignName);
    if (filter)
        Require(sscanf(filter, "%d:%d:%d", &campaignFilter, &seriesFilter, &missionFilter) == 3,
                "campaign:series:mission filter");
    if (ticks)
        tickLimit = atoi(ticks);
    Require(campaignFilter >= -1 && campaignFilter < 4 && tickLimit > 0 && tickLimit <= 3600,
            "valid campaign and tick budget");
    Require(chdir(scratch) == 0, "enter scratch installation");
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
    SDL_ClearError(); /* Dummy-driver setup errors are not failures. */
    for (campaign = 0; campaign < 4; campaign++) {
        if ((campaign == 1 || campaign == 2) && !SdlHasSecretMissions(campaign))
            continue;
        if (list)
            printf("INSTALLED %d\n", campaign);
        if (campaignFilter >= 0 && campaignFilter != campaign)
            continue;
        if (campaign == 3) {
            /* RunTrainSim's four missions; they have no flight path. */
            simulatorSelected = seriesFilter <= 0;
            for (mission = 0; simulatorSelected && mission < 4; mission++) {
                if (missionFilter >= 0 && missionFilter != mission)
                    continue;
                if (list)
                    printf("CASE 3 0 %d\n", mission);
                if (seeds != NULL)
                    WriteSeed(seeds, 3, 0, mission, 0);
                cases++;
            }
            continue;
        }
        LoadPacketIntoBuffer(asCampaignPilotFiles[campaign], 1, pMissionCampaignInfo);
        for (series = 1; series <= seriesCounts[campaign]; series++) {
            if (seriesFilter >= 0 && seriesFilter != series)
                continue;
            missionCounts[campaign][series - 1] = pMissionCampaignInfo[(series - 1) * 90 + 2];
            for (mission = 0; mission < missionCounts[campaign][series - 1]; mission++) {
                if (missionFilter >= 0 && missionFilter != mission)
                    continue;
                if (list)
                    printf("CASE %d %d %d\n", campaign, series, mission);
                for (legs = 0; seeds != NULL && legs < 4; legs++)
                    WriteSeed(seeds, campaign, series, mission, legs);
                cases++;
            }
        }
    }
    inputActive = 0;
    if (list) {
        FILE *file = fopen("probe-complete", "w");
        Require(file != NULL && fclose(file) == 0, "record completed inventory");
        exit(0);
    }
    Require(cases > 0, "matching missions installed");
    SdlSetKeyboardSnapshot(keys);
    return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    const FlightAction *action;
    int campaign, series, mission, simulator, entry, tick = 0, hold, frame;
    int startLeg, reachedLeg;
    size_t position;
    unsigned int seed;

    if (size < 10)
        return -1;
    campaign = data[0];
    series = data[1];
    mission = data[2];
    simulator = campaign == 3;
    if (simulator) {
        if (!simulatorSelected || series != 0 || mission > 3 ||
            (missionFilter >= 0 && missionFilter != mission))
            return -1;
    } else if (campaign >= 3 || series < 1 || series > 13 ||
               mission >= missionCounts[campaign][series - 1] ||
               (missionFilter >= 0 && missionFilter != mission)) {
        return -1;
    }
    inputActive = 1;
    seed = (unsigned int)data[4] | ((unsigned int)data[5] << 8) |
           ((unsigned int)data[6] << 16) | ((unsigned int)data[7] << 24);
    srand(seed);
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
    startLeg = cCurrentNavPtIdx;
    if (!simulator)
        TravelFlightPath(data[3] & 15);
    reachedLeg = cCurrentNavPtIdx;
    for (position = 8; position + 1 < size && tick < tickLimit && !nArcadeState; position += 2) {
        if (!(data[position] & 0x40))
            ReleaseFlightControls();
        action = &actions[(data[position] & 63) % SDL_arraysize(actions)];
        hold = (1 + (data[position + 1] & 63)) << (data[position + 1] >> 6);
        ApplyFlightAction(action);
        for (frame = 0; frame < hold && tick < tickLimit && !nArcadeState; frame++, tick++) {
            /* Keyboard repeat applies to the most recent key. */
            if (frame && frame % 4 == 0 && repeatedKey != SDL_SCANCODE_UNKNOWN &&
                keys[repeatedKey] && !OpensInflightComputer(repeatedKey))
                QueueFlightKey(repeatedKey, 2);
            FlightStep();
        }
    }
    if (traceInputs)
        fprintf(stderr, "TRACE campaign %d series %d mission %d: nav %d -> %d (%d legs asked) "
                "-> %d, %d ticks, arcade state %d, speed %d at %d %d %d\n", campaign, series,
                mission, startLeg, reachedLeg, simulator ? 0 : data[3] & 15, cCurrentNavPtIdx,
                tick, nArcadeState, anShipSpeed[0], aShipPositionsArray[0].x,
                aShipPositionsArray[0].y, aShipPositionsArray[0].z);
    ReleaseFlightControls();
    SdlPumpEvents();
    FlushInputEvents();
    ClearInputKeyStatePreservingModifiers();
    SetEventManagerPump(NULL);
    free_cockpit();
    free_all_slots();
    free_3Space();
    nTrainSimActive = 0;
    inputActive = 0;
    return 0;
}
