/* libFuzzer-driven WC1/SM1/SM2 flight controls; use runGameTests.py --fuzz.
 * Input: campaign, series (1-based), mission (0-based), nav, little-endian
 * PRNG seed (4 bytes), then pairs of key index and hold duration (1..64 ticks).
 * Only controls are mutated; assets come from the player's installation. */
#include "wc1.h"

static Uint8 keys[SDL_NUM_SCANCODES];
static unsigned char missionCounts[3][13];
static int campaignFilter = -1, seriesFilter = -1, missionFilter = -1;
static int tickLimit = 600;
static int inputActive;

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

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    SDL_Window *window;
    const char *scratch = getenv("WC1_TEST_ROOT");
    const char *filter = getenv("WC1_FLIGHT_CASE");
    const char *campaignName = getenv("WC1_FLIGHT_CAMPAIGN");
    const char *ticks = getenv("WC1_FLIGHT_TICKS");
    const int seriesCounts[] = {13, 8, 9};
    int campaign, series, mission, cases = 0;
    int list = getenv("WC1_FLIGHT_LIST") != NULL;
    (void)argc;
    (void)argv;

    Require(atexit(CheckFuzzExit) == 0, "register early-exit check");
    inputActive = 1;
    Require(scratch != NULL, "use runGameTests.py --fuzz to prepare disposable data");
    if (campaignName)
        campaignFilter = atoi(campaignName);
    if (filter)
        Require(sscanf(filter, "%d:%d:%d", &campaignFilter, &seriesFilter, &missionFilter) == 3,
                "campaign:series:mission filter");
    if (ticks)
        tickLimit = atoi(ticks);
    Require(campaignFilter >= -1 && campaignFilter < 3 && tickLimit > 0 && tickLimit <= 3600,
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
    nFrameIntervalMs = 0;
    SDL_ClearError(); /* Dummy-driver setup errors are not failures. */
    for (campaign = 0; campaign < 3; campaign++) {
        if (campaign && !SdlHasSecretMissions(campaign))
            continue;
        if (list)
            printf("INSTALLED %d\n", campaign);
        if (campaignFilter >= 0 && campaignFilter != campaign)
            continue;
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
    /* Non-modal commands only: pause, quit and menus need acknowledgement. */
    static const SDL_Scancode choices[] = {
        SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN,
        SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_COMMA, SDL_SCANCODE_PERIOD,
        SDL_SCANCODE_SPACE, SDL_SCANCODE_SPACE, SDL_SCANCODE_SPACE,
        SDL_SCANCODE_EQUALS, SDL_SCANCODE_EQUALS, SDL_SCANCODE_MINUS, SDL_SCANCODE_TAB,
        SDL_SCANCODE_RETURN, SDL_SCANCODE_G, SDL_SCANCODE_W, SDL_SCANCODE_T,
        SDL_SCANCODE_L, SDL_SCANCODE_D, SDL_SCANCODE_C,
        SDL_SCANCODE_H, SDL_SCANCODE_B, SDL_SCANCODE_1, SDL_SCANCODE_2,
        SDL_SCANCODE_F1, SDL_SCANCODE_F2, SDL_SCANCODE_F3, SDL_SCANCODE_F4,
        SDL_SCANCODE_F5, SDL_SCANCODE_F6, SDL_SCANCODE_F7, SDL_SCANCODE_F8, SDL_SCANCODE_F9
    };
    int campaign, series, mission, nav, candidate, tick = 0, hold, frame;
    size_t position;
    unsigned int seed;
    SDL_Scancode held = SDL_SCANCODE_UNKNOWN;

    if (size < 10)
        return -1;
    campaign = data[0];
    series = data[1];
    mission = data[2];
    if (campaign >= 3 || series < 1 || series > 13 ||
        mission >= missionCounts[campaign][series - 1] ||
        (missionFilter >= 0 && missionFilter != mission))
        return -1;
    inputActive = 1;
    seed = (unsigned int)data[4] | ((unsigned int)data[5] << 8) |
           ((unsigned int)data[6] << 16) | ((unsigned int)data[7] << 24);
    srand(seed);
    InitGameClockEpoch();
    ResetCampaignData();
    memset(keys, 0, sizeof(keys));
    SdlPumpEvents();
    FlushInputEvents();
    ClearInputKeyStatePreservingModifiers();
    bCurrentKey = cPreviousKey = SCAN_NONE;
    stCampaignState.campaignIndex = nCampaignDataSet = campaign;
    if (campaign == 2)
        SdlSetSm2Pilots(aPilotEntriesList, 1);
    LoadPacketIntoBuffer(asCampaignPilotFiles[campaign], 0, pConstellationDefinitionsPtr);
    LoadPacketIntoBuffer(asCampaignPilotFiles[campaign], 1, pMissionCampaignInfo);
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
    Require(init_mission(series, mission) == 0, "initialize mission");
    /* Campaigns share one process; only SM2 replaces the Spikeri. */
    Require(!strcmp(aObjectTypeData[OBJECT_TYPE_SPIKERI].displayName, "Hriss") == (campaign == 2),
            "campaign ship definitions");
    nav = data[3] % ACTIVE_MISSION_NAV_POINT_COUNT;
    for (candidate = 0; candidate < ACTIVE_MISSION_NAV_POINT_COUNT; candidate++) {
        if (aMissionNavPoints[nav].type >= 0 && aMissionNavPoints[nav].type != 2)
            break;
        nav = (nav + 1) % ACTIVE_MISSION_NAV_POINT_COUNT;
    }
    Require(candidate < ACTIVE_MISSION_NAV_POINT_COUNT, "mission contains flight encounters");
    aShipPositionsArray[0] = aMissionNavPoints[nav].position;
    set_up_action_sphere(nav);
    bEscapePressedFlag = 1; /* Skip only the launch animation. */
    LaunchPlayerShip();
    SetEventManagerPump(get_player_input);
    bInputModeState = 1;
    bMouseCursorVisible = 0;
    FlushInputEvents();
    ClearInputKeyStatePreservingModifiers();
    bCurrentKey = cPreviousKey = SCAN_NONE;
    for (position = 8; position + 1 < size && tick < tickLimit && !nArcadeState; position += 2) {
        QueueFlightKey(held, 0);
        held = choices[data[position] % SDL_arraysize(choices)];
        hold = 1 + (data[position + 1] & 63);
        QueueFlightKey(held, 1);
        for (frame = 0; frame < hold && tick < tickLimit && !nArcadeState; frame++, tick++) {
            if (frame && frame % 4 == 0)
                QueueFlightKey(held, 2);
            Require(HandleSpaceFlightControls() != -1, "flight controls continue");
            if (!nArcadeState) {
                Update_3Space();
                RenderSpaceViewFrame();
                update_cockpit();
            }
        }
    }
    QueueFlightKey(held, 0);
    SdlPumpEvents();
    FlushInputEvents();
    ClearInputKeyStatePreservingModifiers();
    SetEventManagerPump(NULL);
    free_cockpit();
    free_all_slots();
    free_3Space();
    inputActive = 0;
    return 0;
}
