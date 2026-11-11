/* Seeded WC1/SM1/SM2 flight controls. Run through runGameTests.py so saves
 * and assets are isolated. Each process exercises one mission's nav points. */
#include "wc1.h"

static Uint8 keys[SDL_NUM_SCANCODES];
static uint32_t inputRandom;

static void Require(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "FAIL: %s (%s)\n", message, SDL_GetError());
        exit(1);
    }
}

static void MarkProbeComplete(void)
{
    FILE *file = fopen("probe-complete", "w");
    Require(file != NULL && fclose(file) == 0, "record completed probe");
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

static SDL_Scancode ChooseFlightKey(void)
{
    /* Continuous flight controls and non-modal HUD/camera commands. Pause,
     * quit and modal menus need a separate driver to acknowledge them. */
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
    inputRandom = inputRandom * 1664525u + 1013904223u;
    return choices[(inputRandom >> 16) % SDL_arraysize(choices)];
}

int main(int argc, char **argv)
{
    SDL_Window *window;
    SDL_Scancode held = SDL_SCANCODE_UNKNOWN;
    int campaign, series, mission, nav, tick, ticks, completed = 0;
    const int seriesCounts[] = {13, 8, 9};
    unsigned int seed;

    setvbuf(stdout, NULL, _IONBF, 0);
    Require(argc == 3 || argc == 7, "scratch --list or scratch campaign series mission seed ticks");
    Require(chdir(argv[1]) == 0, "enter scratch installation");
    SDL_SetMainReady();
    Require(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) == 0, "SDL init");
    window = SDL_CreateWindow("flight test", 0, 0, 640, 400, 0);
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
    InitGameClockEpoch();
    nVideoMode = 0x13;
    LoadOriginFxDrivers();
    nFrameIntervalMs = 0;
    if (argc == 3) {
        Require(!strcmp(argv[2], "--list"), "list mode");
        for (campaign = 0; campaign < 3; campaign++) {
            if (campaign && !SdlHasSecretMissions(campaign))
                continue;
            LoadPacketIntoBuffer(asCampaignPilotFiles[campaign], 1, pMissionCampaignInfo);
            for (series = 1; series <= seriesCounts[campaign]; series++)
                for (mission = 0; mission < pMissionCampaignInfo[(series - 1) * 90 + 2]; mission++)
                    printf("CASE %d %d %d\n", campaign, series, mission);
        }
        MarkProbeComplete();
        return 0;
    }
    campaign = atoi(argv[2]);
    series = atoi(argv[3]);
    mission = atoi(argv[4]);
    seed = (unsigned int)strtoul(argv[5], NULL, 10);
    ticks = atoi(argv[6]);
    Require(campaign >= 0 && campaign < 3 && series >= 1 &&
            series <= seriesCounts[campaign] && mission >= 0 && ticks > 0 && ticks <= 3600,
            "valid mission and tick budget");
    Require(!campaign || SdlHasSecretMissions(campaign), "expansion installed");
    srand(seed);
    inputRandom = seed;
    stCampaignState.campaignIndex = nCampaignDataSet = campaign;
    if (campaign == 2)
        SdlSetSm2Pilots(aPilotEntriesList, 1);
    LoadPacketIntoBuffer(asCampaignPilotFiles[campaign], 0, pConstellationDefinitionsPtr);
    LoadPacketIntoBuffer(asCampaignPilotFiles[campaign], 1, pMissionCampaignInfo);
    Require(mission < pMissionCampaignInfo[(series - 1) * 90 + 2], "mission exists");
    stCampaignState.currentSeries = series;
    stCampaignState.currentMission = mission;
    SdlSetKeyboardSnapshot(keys);
    nCurrentActiveInputDevice = -1;
    for (nav = 0; nav < ACTIVE_MISSION_NAV_POINT_COUNT; nav++) {
        nArcadeState = 0;
        bCockpitlessView = 0;
        Require(init_mission(series, mission) == 0, "initialize mission");
        if (aMissionNavPoints[nav].type >= 0 && aMissionNavPoints[nav].type != 2) {
            printf("NAV campaign=%d series=%d mission=%d nav=%d seed=%u\n",
                   campaign, series, mission, nav, seed);
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
            for (tick = 0; tick < ticks && !nArcadeState; tick++) {
                if (tick % 16 == 0) {
                    QueueFlightKey(held, 0);
                    held = ChooseFlightKey();
                    printf("INPUT tick=%d key=%d (%s)\n", tick, held, SDL_GetScancodeName(held));
                    QueueFlightKey(held, 1);
                } else if (tick % 4 == 0) {
                    QueueFlightKey(held, 2);
                }
                Require(HandleSpaceFlightControls() != -1, "flight controls continue");
                if (!nArcadeState) {
                    Update_3Space();
                    RenderSpaceViewFrame();
                    update_cockpit();
                }
            }
            printf("END ticks=%d outcome=%d position=%d,%d,%d\n", tick, nArcadeState,
                   aShipPositionsArray[0].x, aShipPositionsArray[0].y, aShipPositionsArray[0].z);
            QueueFlightKey(held, 0);
            held = SDL_SCANCODE_UNKNOWN;
            SdlPumpEvents();
            FlushInputEvents();
            completed++;
        }
        free_cockpit();
        free_all_slots();
        free_3Space();
    }
    Require(completed > 0, "mission contains flight encounters");
    SdlSetKeyboardSnapshot(NULL);
    MarkProbeComplete();
    printf("PASS: %d nav encounters\n", completed);
    return 0;
}
