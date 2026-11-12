/* libFuzzer-driven SWC flight controls; use make check-fuzz-swc.
 * Input: data set (0-2, or 3 for the simulator), series (1-based; ignored by
 * the simulator), mission (0-based), a reserved byte, little-endian PRNG seed
 * (4 bytes), then pairs of action index and hold duration (1..64 ticks).
 * Only controls are mutated; the SWC directory is read, never written. */
#include "wc1.h"
#include "swc.h"
#include "../src/sdl/swc_flight.c"

static Uint8 keys[SDL_NUM_SCANCODES];
static unsigned char missionCounts[3][13];
static SwcCmf fuzzCockpitCmf, fuzzSpaceCmf;
static SwcFlightImages fuzzImages[OBJECT_TYPE_COUNT];
static SDL_Color fuzzColors[256];
static ObjectTypeData fuzzSavedObjects[OBJECT_TYPE_COUNT];
static char fuzzMissionPath[PATH_MAX];
static char fuzzResourcePath[PATH_MAX];
static SDL_Renderer *fuzzRenderer;
static int fuzzCockpitless;
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
    Require(!inputActive, "game exited during SWC flight input or initialization");
}

/* Held keys feed SwcFlightTick; presses repeat SdlRunSwcMission's flight
 * handlers. Autopilot, the computer, pause and ejection are modal or end
 * the flight, so they need a separate driver. */
static const SDL_Keycode actions[] = {
    SDLK_UNKNOWN, SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_COMMA, SDLK_PERIOD,
    SDLK_SPACE, SDLK_SPACE, SDLK_SPACE, SDLK_EQUALS, SDLK_EQUALS, SDLK_MINUS, SDLK_TAB,
    SDLK_BACKSPACE, SDLK_BACKSLASH, SDLK_RETURN, SDLK_g, SDLK_w, SDLK_t, SDLK_l, SDLK_n,
    SDLK_c, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_y, SDLK_d, SDLK_f, SDLK_F1
};

static void PressFlightKey(SDL_Keycode key)
{
    int view;
    int choice;
    int command;
    int recipient;

    switch (key) {
    case SDLK_SPACE:
        swcGunFirePending = 1;
        break;
    case SDLK_BACKSPACE:
        anShipSpeed[0] = 0;
        break;
    case SDLK_BACKSLASH:
        celerate(0, asShipMaximumSpeedArray[0] * 256 - anShipSpeed[0]);
        break;
    case SDLK_n:
        if (abFlightPathList[0] != -1)
            cycle_next_objective();
        break;
    case SDLK_y:
    case SDLK_d:
    case SDLK_f:
        if (!bCurrentPlayerDestroyed && nArcadeState == 0) {
            view = key == SDLK_y ? 0 : key == SDLK_d ? 14 : 4;
            if (view == nCameraViewMode)
                view = 0;
            if (view != nCameraViewMode)
                SwcSelectFlightView(view);
        }
        break;
    case SDLK_l:
        nTargetLockMode = (nTargetLockMode == 0);
        break;
    case SDLK_t:
        cycle_onscreen_targets();
        break;
    case SDLK_g:
        select_new_gun();
        break;
    case SDLK_w:
        select_new_release_weapon((enum ObjectType)-1);
        SdlSwcLockOff();
        break;
    case SDLK_RETURN:
        swcReleaseWeaponPending = 1;
        break;
    case SDLK_c:
        if (!bCurrentPlayerDestroyed && nArcadeState == 0) {
            if (IsCommChoiceMenuOpen())
                CloseCommChoiceMenu();
            else
                HandleCommunicationMenuRequest();
        }
        break;
    case SDLK_1:
    case SDLK_2:
    case SDLK_3:
    case SDLK_4:
        choice = key - SDLK_1;
        if (!bCurrentPlayerDestroyed && IsCommChoiceMenuOpen() && choice < nCommMenuChoiceCount) {
            command = GetPendingMenuAction() == 2 ? abCommMenuChoiceCommands[choice] : -1;
            recipient = cCommMenuRecipient;
            Chosen_communicate_option(choice);
            if (command > 0)
                SdlSwcVocalizeMessage(command);
            if (command == 12)
                SdlSwcCockpitMessage(bLandingAuthorized ? "Landing clearance granted" :
                    "Landing clearance denied", 0x81, 0, -1);
            else if (command > 0 && recipient > 0 && recipient < 10)
                SdlSwcCockpitMessage(acWingmanMessageStatesArray[recipient] == 1 ?
                    "Unable to comply" : "Message sent", 0x81, 0, -1);
        }
        break;
    case SDLK_F1: /* Ctrl+F1 in the host. */
        fuzzCockpitless = !fuzzCockpitless;
        break;
    default:
        break;
    }
}

static void WriteSeed(const char *directory, const char *name, int campaign, int series, int mission)
{
    char path[PATH_MAX];
    SDL_RWops *file;
    const unsigned char seed[] = {
        (unsigned char)campaign, (unsigned char)series, (unsigned char)mission, 0, 0, 0, 0, 0,
        10, 31, 3, 15, 7, 31, 17, 7, 16, 0, 22, 3, 23, 15
    };

    SDL_snprintf(path, sizeof(path), "%s/%s", directory, name);
    file = SDL_RWFromFile(path, "rb");
    if (file != NULL) {
        SDL_RWclose(file);
        return;
    }
    file = SDL_RWFromFile(path, "wb");
    Require(file != NULL && SDL_RWwrite(file, seed, sizeof(seed), 1) == 1 &&
            SDL_RWclose(file) == 0, "write SWC flight seed");
}

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    SDL_Window *window;
    SwcBuffer campaign = {0};
    unsigned char *savedCampaign = pMissionCampaignInfo;
    const char *directory = getenv("WC1_SWC_DATA");
    const char *seeds = getenv("WC1_SWC_FLIGHT_SEEDS");
    const char *ticks = getenv("WC1_FLIGHT_TICKS");
    char path[PATH_MAX];
    char name[32];
    int set;
    int series;
    int mission;
    (void)argc;
    (void)argv;

    Require(atexit(CheckFuzzExit) == 0, "register early-exit check");
    inputActive = 1;
    if (ticks)
        tickLimit = atoi(ticks);
    Require(tickLimit > 0 && tickLimit <= 3600, "tick budget in 1..3600");
    Require(directory != NULL && chdir(directory) == 0,
            "set WC1_SWC_DATA to an SWC directory containing CMFs");
    Require(SdlFindSwcMissionData(fuzzMissionPath, sizeof(fuzzMissionPath)), "SWC CMFs/Data.CMF");
    /* As SdlRunSwcMission, before its startup movies. */
    SwcInitCharacters();
    nCampaignDataSet = 0;
    Require(SdlLoadSwcMissionData(fuzzMissionPath, 1, 0) == 0, "load the first SWC mission");
    Require(SdlResolvePath("CMFs/Spaceflight.CMF", path, sizeof(path)) &&
            SwcCMOpen(path, &fuzzSpaceCmf) == 0, "open Spaceflight.CMF");
    Require(SdlFindSwcResources(NULL, fuzzResourcePath, sizeof(fuzzResourcePath)) &&
            SwcReadPalette(fuzzResourcePath, 251, fuzzColors) == 0, "SWC application resources");
    SDL_SetMainReady();
    Require(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) == 0, "SDL init");
    window = SDL_CreateWindow("SWC flight fuzzer", 0, 0, 320, 240, SDL_WINDOW_HIDDEN);
    Require(window != NULL, "headless window");
    fuzzRenderer = SdlCreateSwcRenderer(window);
    Require(fuzzRenderer != NULL && SDL_RenderSetLogicalSize(fuzzRenderer, 320, 240) == 0,
            "SWC renderer");
    swcRasterCanvas = SDL_CreateTexture(fuzzRenderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING, SWC_FRAME_WIDTH, SWC_FRAME_HEIGHT);
    Require(swcRasterCanvas != NULL &&
            SDL_SetTextureBlendMode(swcRasterCanvas, SDL_BLENDMODE_BLEND) == 0, "raster canvas");
    Require(SdlCreateSwcTextFont(fuzzRenderer, &swcPauseFont, 10) == 0, "SWC text font");
    swcFlightActive = 1;
    swcCinemaWindow = window;
    swcCinemaRenderer = fuzzRenderer;
    swcCinemaCockpitCmf = &fuzzCockpitCmf;
    swcCinemaSpaceCmf = &fuzzSpaceCmf;
    swcCinemaColors = fuzzColors;
    swcCinemaImages = fuzzImages;
    SwcSetObjectData(fuzzSavedObjects);
    srand(1);
    Require(SwcStartMissionShips() >= 0 &&
            SwcInitializeCockpit(stCampaignState.playerShipType, fuzzResourcePath) == 0,
            "initial SWC cockpit");
    Require(SdlInitSwcSound(fuzzResourcePath) == 0, "SWC sound");
    SetEventManagerPump(NULL);
    stMouseCursorState.viewport = &stSpaceBufferViewport;
    SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
    for (set = 0; set < 3; set++) {
        nCampaignDataSet = set;
        if (SdlLoadSwcCampaignData(fuzzMissionPath, &campaign) != 0) {
            SDL_ClearError();
            continue;
        }
        for (series = 1; series <= 13; series++) {
            missionCounts[set][series - 1] = pMissionCampaignInfo[(series - 1) * 90 + 2];
            for (mission = 0; seeds != NULL && mission < missionCounts[set][series - 1]; mission++) {
                SDL_snprintf(name, sizeof(name), "swc%d-%d-%d", set, series, mission);
                WriteSeed(seeds, name, set, series, mission);
            }
        }
        pMissionCampaignInfo = savedCampaign;
    }
    SDL_free(campaign.data);
    for (mission = 0; seeds != NULL && mission < 4; mission++) {
        SDL_snprintf(name, sizeof(name), "simulator-%d", mission);
        WriteSeed(seeds, name, 3, 0, mission);
    }
    nCampaignDataSet = 0;
    inputActive = 0;
    return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    int campaign;
    int series;
    int mission;
    int simulator;
    int shipCount;
    int object;
    int tick = 0;
    int hold;
    int frame;
    size_t position;
    unsigned int seed;
    Uint32 clock = 0;
    SDL_Keycode held = SDLK_UNKNOWN;

    if (size < 10)
        return -1;
    campaign = data[0];
    series = data[1];
    mission = data[2];
    simulator = campaign == 3;
    if (simulator) {
        if (mission > 3)
            return -1;
        series = 0;
    } else if (campaign > 2 || series < 1 || series > 13 ||
               mission >= missionCounts[campaign][series - 1]) {
        return -1;
    }
    inputActive = 1;
    seed = (unsigned int)data[4] | ((unsigned int)data[5] << 8) |
           ((unsigned int)data[6] << 16) | ((unsigned int)data[7] << 24);
    srand(seed);
    memset(keys, 0, sizeof(keys));
    SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
    SwcResetFlightInput();
    SwcInitCharacters();
    nSwcBossmanDeath = 0;
    nArcadeState = 0;
    bCurrentPlayerDestroyed = 0;
    nTrainSimActive = 0;
    nCampaignDataSet = simulator ? 0 : campaign;
    stCampaignState.campaignIndex = nCampaignDataSet;
    stCampaignState.currentSeries = series;
    stCampaignState.currentMission = mission;
    Require(SdlLoadSwcMissionData(fuzzMissionPath, series, mission) == 0, "load SWC mission");
    shipCount = SwcStartMissionShips();
    Require(shipCount >= 0, "start SWC mission ships");
    Require(SwcInitializeCockpit(simulator ? 4 : stCampaignState.playerShipType,
                                 fuzzResourcePath) == 0, "SWC cockpit");
    if (simulator) {
        /* As the host's simulator_mission label (retail ArcadeGame). */
        nTrainSimMission = mission;
        nCurrentArcadeScore = 0;
        nTrainSimActive = 1;
        nArcadeWave = 0;
        nArcadeBonusCountdown = 0;
        nCurrentWave = aMissionNavPoints[nCurrentNavigationPoint + 1].type == 2 ? 2 : -1;
        FigureArcadeTime();
    }
    /* As the host's begin_flight label, without window and mouse capture. */
    bSwcWaitingForEscort = 0;
    nSwcCinemaResult = 0;
    SwcResetFlightInput();
    SetEventManagerPump(NULL);
    stMouseCursorState.viewport = &stSpaceBufferViewport;
    bCockpitlessView = 0;
    SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
    bHostPrimaryMouseButton = 0;
    bHostSecondaryMouseButton = 0;
    nCurrentSpaceFrameValue = 0;
    swcExhaustFrame = -1;
    nRenderedSpaceFrame = 0;
    fuzzCockpitless = 0;
    SwcTransformObjectsToView();
    SdlUpdateSwcCockpit();
    SetSpaceFlightFrameTiming();
    for (object = 1; object < shipCount; object++)
        Require(SwcLoadFlightImages(&fuzzCockpitCmf, &fuzzSpaceCmf, aeObjectTypeList[object],
                                    &fuzzImages[aeObjectTypeList[object]]) == 0, "SWC ship images");
    for (position = 8; position + 1 < size && tick < tickLimit &&
         nArcadeState == 0 && !bCurrentPlayerDestroyed; position += 2) {
        if (held != SDLK_UNKNOWN)
            keys[SDL_GetScancodeFromKey(held)] = 0;
        held = actions[data[position] % SDL_arraysize(actions)];
        hold = 1 + (data[position + 1] & 63);
        if (held != SDLK_UNKNOWN)
            keys[SDL_GetScancodeFromKey(held)] = 1;
        PressFlightKey(held);
        for (frame = 0; frame < hold && tick < tickLimit &&
             nArcadeState == 0 && !bCurrentPlayerDestroyed; frame++, tick++) {
            clock += SDL_max(1, nFrameIntervalMs);
            nTickCount60Hz = (unsigned int)((uint64_t)clock * 60 / 1000);
            SwcFlightTick(keys, 0);
            Require(SwcDrawFlight(fuzzRenderer, swcCockpitTexture, &fuzzCockpitCmf, &fuzzSpaceCmf,
                                  fuzzColors, fuzzImages, fuzzCockpitless) == 0, "draw SWC flight");
        }
    }
    SdlStopSwcSounds();
    SwcResetFlightInput();
    if (IsCommChoiceMenuOpen())
        CloseCommChoiceMenu();
    nTrainSimActive = 0;
    memset(keys, 0, sizeof(keys));
    inputActive = 0;
    return 0;
}
