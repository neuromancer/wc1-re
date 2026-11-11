/* Run through runGameTests.py, which provides a disposable installation.
 * Script menu input and mission outcomes; exercise the real campaign, flight
 * encounters, scenes, save/load, autosave and retry implementations. */
#include "wc1.h"
static int fullScenes;
static UINT TestSceneTimer(UINT delay, UINT resolution, LPTIMECALLBACK callback,
                           DWORD user, UINT eventType)
{
    /* Keep animation scripts intact; shorten only timed reading pauses. */
    if (fullScenes)
        delay = delay / 100 + 1;
    return timeSetEvent(delay, resolution, callback, user, eventType);
}
#define timeSetEvent TestSceneTimer
#include "../src/timer.c"
#undef timeSetEvent
static void TestPilotCreation(void);
static short __stdcall TestTitleInput(InputEventState *, short);
static short TestCampaignRoom(void);
static short TestCampaignBarracks(void);
static int TestCampaignFlight(short);
static int TestRetryMission(void);
static unsigned int TestEndScreen(short);
#define RunTrainSim TestPilotCreation
#define RecRoom TestCampaignRoom
#define BarracksScreen TestCampaignBarracks
#define RunSpaceFlight TestCampaignFlight
#define SdlRetryMission TestRetryMission
#define ShowTheEndScreen TestEndScreen
#include "../src/gameflow.c"
#undef RunTrainSim
#undef RecRoom
#undef BarracksScreen
#undef RunSpaceFlight
#undef SdlRetryMission
#undef ShowTheEndScreen
#define PollInputEvent TestTitleInput
#include "../src/title.c"
#undef PollInputEvent
static short __stdcall TestBarracksInput(InputEventState *, short);
static short TestConfirmationKey(void);
#define PollInputEvent TestBarracksInput
#define WaitForStreamInputKey TestConfirmationKey
#include "../src/barracks.c"
#undef PollInputEvent
#undef WaitForStreamInputKey
static short __stdcall TestRecRoomInput(InputEventState *, short);
#define PollInputEvent TestRecRoomInput
#include "../src/killbrd.c"
#undef PollInputEvent

static short titleKeys[3];
static int titleKeyIndex;
static int cancelTransfer;
static int loseCampaign, campaignFlights;
static int flightOutcome = 1, acceptRetry;
static SDL_atomic_t retryKey;
static int recRoomChoice;

static void Require(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "FAIL: %s (%s)\n", message, SDL_GetError());
        exit(1);
    }
}

static void TestPilotCreation(void)
{
    Require(bCampaignStartupMode && bCampaignActive, "new campaign prologue");
}

static short __stdcall TestTitleInput(InputEventState *event, short filter)
{
    static int captured;
    (void)filter;
    if (!captured && getenv("WC1_CAMPAIGN_MENU_CAPTURE") && aTitleMenuRegions[1].frame != -1) {
        SDL_Surface *frame = SDL_CreateRGBSurfaceWithFormat(0, 640, 400, 32, SDL_PIXELFORMAT_ARGB8888);
        Require(frame && SDL_RenderReadPixels(SDL_GetRenderer(hMainWindow), NULL,
                frame->format->format, frame->pixels, frame->pitch) == 0 &&
                SDL_SaveBMP(frame, getenv("WC1_CAMPAIGN_MENU_CAPTURE")) == 0, "capture title menu");
        SDL_FreeSurface(frame);
        captured = 1;
    }
    Require(titleKeyIndex < 3, "title menu handles requested shortcut");
    memset(event, 0, sizeof(*event));
    event->value = titleKeys[titleKeyIndex++];
    if ((short)event->value < 0) {
        const TitleMenuRegion *region = &aTitleMenuRegions[-(short)event->value - 1];
        event->x = (region->left + region->right) / 2;
        event->y = (region->top + region->bottom) / 2;
        return INPUT_EVENT_BUTTON_DOWN;
    }
    event->x = 60;
    event->y = 50;
    return INPUT_EVENT_KEY_DOWN;
}

static void CheckTitle(short first, short second, int expected)
{
    bEscapePressedFlag = 1;
    titleKeyIndex = 0;
    titleKeys[0] = first;
    titleKeys[1] = second;
    titleKeys[2] = SCAN_S;
    Require(Title_Sequence() == expected, "title campaign selection");
}

static short __stdcall TestBarracksInput(InputEventState *event, short filter)
{
    (void)filter;
    memset(event, 0, sizeof(*event));
    event->value = SCAN_ESCAPE;
    event->x = 140;
    event->y = 90;
    return cancelTransfer ? INPUT_EVENT_KEY_DOWN : INPUT_EVENT_BUTTON_DOWN;
}

static short TestConfirmationKey(void)
{
    return 'Y';
}

static short __stdcall TestRecRoomInput(InputEventState *event, short filter)
{
    const TitleMenuRegion *region;
    (void)filter;
    Require(recRoomChoice < 4, "rec room returns to barracks");
    region = &aRecRoomMenuRegions[recRoomChoice == 3 ? 4 : recRoomChoice];
    recRoomChoice++;
    memset(event, 0, sizeof(*event));
    event->x = (region->left + region->right) / 2;
    event->y = (region->top + region->bottom) / 2;
    return INPUT_EVENT_BUTTON_DOWN;
}

static short TestCampaignRoom(void)
{
    return 0;
}

static short TestCampaignBarracks(void)
{
    return 7;
}

static int TestCampaignFlight(short wave)
{
    int objective;
    (void)wave;
    Require(++campaignFlights <= 18, "campaign terminates");
    for (objective = 0; objective < MISSION_OBJECTIVE_COUNT; objective++)
        aMissionObjectivesArray[objective].flags = loseCampaign ? 0 : 3;
    nPlayerKillTotal = nWingmanKillCount = 1;
    nMissionMedalScoreValue = 100;
    set_up_action_sphere(aMissionShips[stCurrentMissionHeader.homeMissionShip].navPoint);
    nPlayerCollisionObject = find_ship_index(stCurrentMissionHeader.homeMissionShip);
    Require(nPlayerCollisionObject >= 0, "carrier present at mission start");
    nArcadeState = campaignFlights == 1 ? flightOutcome : 1;
    return nArcadeState;
}

static int TestRetryMission(void)
{
    int result;
    SDL_AtomicSet(&retryKey, acceptRetry ? SDL_SCANCODE_RETURN : SDL_SCANCODE_ESCAPE);
    result = SdlRetryMission();
    SDL_AtomicSet(&retryKey, 0);
    Require(result == acceptRetry, "retry prompt accepts scripted choice");
    return result;
}

static unsigned int TestEndScreen(short fireworks)
{
    unsigned int result;
    /* The medals viewer waits for input after the ending scene finishes. */
    SDL_AtomicSet(&retryKey, SDL_SCANCODE_SPACE);
    result = ShowTheEndScreen(fireworks);
    SDL_AtomicSet(&retryKey, 0);
    FlushInputEvents();
    return result;
}

static Uint32 SkipScene(Uint32 interval, void *unused)
{
    SDL_Event event = {0};
    int key = SDL_AtomicGet(&retryKey);
    (void)unused;
    if (fullScenes && !key)
        return interval;
    event.type = SDL_KEYDOWN;
    event.key.keysym.scancode = key ? key : SDL_SCANCODE_SPACE;
    event.key.keysym.sym = key == SDL_SCANCODE_RETURN ? SDLK_RETURN :
        key == SDL_SCANCODE_ESCAPE ? SDLK_ESCAPE : SDLK_SPACE;
    SDL_PushEvent(&event);
    event.type = SDL_KEYUP;
    SDL_PushEvent(&event);
    return interval;
}

int main(int argc, char **argv)
{
    SDL_Window *window;
    SaveGameRecord saved = {0}, loaded = {0};
    unsigned char *expected;
    int series, mission, count = 0, variant, nav, tick, object, sivar = 0;
    SDL_TimerID skip;
    FILE *completion;
    int campaign = argc > 2 ? 2 : 1;
    int seriesCount = campaign == 2 ? 9 : 8;
    int hasSm1;
    int flightTicks = getenv("WC1_CAMPAIGN_FLIGHT_TICKS") ?
        atoi(getenv("WC1_CAMPAIGN_FLIGHT_TICKS")) : 30;
    const char *campFile = campaign == 2 ? "CAMP.002" : "CAMP.001";
    const char *moduleFile = campaign == 2 ? "MODULE.002" : "MODULE.001";

    setvbuf(stdout, NULL, _IONBF, 0);
    fullScenes = getenv("WC1_CAMPAIGN_FULL_SCENES") != NULL;
    Require(flightTicks > 0 && flightTicks <= 3600, "flight tick count in 1..3600");
    Require(argc >= 2 && chdir(argv[1]) == 0, "enter scratch installation");
    SDL_SetMainReady();
    Require(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) == 0, "SDL init");
    window = SDL_CreateWindow("expansion test", 0, 0, 640, 400, 0);
    Require(window != NULL, "headless window");
    DIBinstall(window);
    hMainWindow = window;
    SdlStartEventPump();
    bIxAudioEnabled = 0;
    bCurrentRunningFromHardDiskFlag = 1;
    nMusicPlaybackMode = 0;
    Require(chdir("GAMEDAT") == 0, "enter assets");
    LoadInstallDat();
    hasSm1 = SdlHasSecretMissions(1);
    Require(SdlHasSecretMissions(campaign), "detect selected expansion");
    Require(chdir("..") == 0, "return to installation root");
    ResetCampaignData();
    InitGameClockEpoch();
    nVideoMode = 0x13;
    LoadOriginFxDrivers();
    nFrameIntervalMs = 0; /* Render cinematics without the presentation frame cap. */
    puts("PASS: resource initialization");
    CreateEmptySaveGameFile();
    nLaunchCampaign = campaign;
    CheckTitle(SCAN_C, SCAN_S, 0);
    Require(titleKeyIndex == 2, "unavailable Continue does not select hovered WC1");
    CheckTitle(-1, SCAN_S, 0);
    StartNewCampaign(campaign);
    Require(stCampaignState.campaignIndex == campaign && nCampaignDataSet == campaign &&
            stCampaignState.currentSeries == 1 && stCampaignState.currentMission == 0,
            "new expansion campaign identity");
    expected = PacketLoad(campFile, 0, NULL, 0, 0);
    Require(expected && !memcmp(expected, pConstellationDefinitionsPtr, 416), "expansion star map");
    ReleasePacketHandle(expected);
    if (campaign == 2) {
        Require(rename("TITLE1.VGA", "TITLE1.tmp") == 0, "hide SM2 title artwork");
        Require(!SdlHasSecretMissions(2) && SdlHasSecretMissions(1) == hasSm1,
                "SM2 requires original title artwork without affecting SM1");
        Require(rename("TITLE1.tmp", "TITLE1.VGA") == 0, "restore SM2 title artwork");
        Require(SdlHasSecretMissions(2), "SM2 available after restoring title artwork");
        Require(!strcmp(aPilotEntriesList[2].name, "COLSON") &&
                !strcmp(aPilotEntriesList[5].name, "MONTCLAIR") &&
                aPilotEntriesList[2].kills == 23 && aPilotEntriesList[5].rank == 2,
                "new SM2 roster");
        if (hasSm1) {
            Require(rename("MODULE.001", "MODULE.tmp") == 0, "hide SM1");
            Require(SdlHasSecretMissions(2) && !SdlHasSecretMissions(1), "SM2 does not require SM1");
            CheckTitle(-1, SCAN_S, 0);
            Require(rename("MODULE.tmp", "MODULE.001") == 0, "restore SM1");
        }
    }
    puts("PASS: title/startup");
    if (campaign == 1) {
        for (series = 1; series <= seriesCount; series++) {
            for (mission = 0; mission < pMissionCampaignInfo[(series - 1) * 90 + 2]; mission++) {
                stCampaignState.currentSeries = series;
                stCampaignState.currentMission = mission;
                nArcadeState = 0;
                Require(init_mission(series, mission) == 0, "initialize expansion flight");
                LaunchPlayerShip();
                Require(stCampaignState.playerShipType >= 0 && stCampaignState.playerShipType <= 3,
                        "supported player ship");
                nFrameSkipCounter = 1;
                Require(RenderSpaceViewFrame() != 0, "render initialized mission");
                for (nav = 0; nav < ACTIVE_MISSION_NAV_POINT_COUNT; nav++) {
                    if (nArcadeState) {
                        free_cockpit();
                        free_all_slots();
                        free_3Space();
                        nArcadeState = 0;
                        Require(init_mission(series, mission) == 0, "restart before next nav after flight ends");
                        LaunchPlayerShip();
                    }
                    if (aMissionNavPoints[nav].type < 0 || aMissionNavPoints[nav].type == 2)
                        continue;
                    aShipPositionsArray[0] = aMissionNavPoints[nav].position;
                    set_up_action_sphere(nav);
                    for (object = 1; object < 10; object++) {
                        if (aeObjectClass[object] >= OBJECT_CLASS_SHIP &&
                            aeObjectTypeList[object] == OBJECT_TYPE_SIVAR) {
                            int record = nShipMissionIndices[object];
                            /* The ship can still be invulnerable while arriving. */
                            if (explode(0, object)) {
                                Require(dead_ship(record), "Sivar destruction uses shared combat");
                                cleanup_objectives();
                                sivar++;
                            }
                        }
                    }
                    for (tick = 0; tick < flightTicks && !nArcadeState; tick++) {
                        Update_3Space();
                        RenderSpaceViewFrame();
                    }
                }
                free_cockpit();
                free_all_slots();
                free_3Space();
                count++;
            }
        }
        Require(count == 18, "all 18 missions initialize and render");
        Require(sivar > 0, "Sivar encounter exercised");
        printf("PASS: 18 flight missions (up to %d ticks per nav point)\n", flightTicks);
    }
    for (series = 1; series <= seriesCount; series++) {
        for (mission = 0; mission < pMissionCampaignInfo[(series - 1) * 90 + 2]; mission++) {
            stCampaignState.currentSeries = series;
            stCampaignState.currentMission = mission;
            skip = SDL_AddTimer(10, SkipScene, NULL);
            nDebriefingPersonality = *(short *)(pMissionCampaignInfo + (series - 1) * 90);
            recRoomChoice = 0;
            Require(RecRoom() == 4, "bar conversations and return hotspot");
            Briefing(series, mission);
            DeBriefing(series, mission);
            Require(!fullScenes || !bEscapePressedFlag, "scenes finish without skip input");
            SDL_RemoveTimer(skip);
            FlushInputEvents();
            printf("PASS: scenes %d/%d\n", series, mission);
        }
    }
    for (series = 1; series <= seriesCount; series++) {
        for (variant = 0; variant < 2; variant++) {
            ResetCampaignData();
            stCampaignState.campaignIndex = nCampaignDataSet = campaign;
            stCampaignState.currentSeries = series;
            stCampaignState.currentMission = pMissionCampaignInfo[(series - 1) * 90 + 2] - 1;
            stCampaignState.seriesScore = variant ? -10000 : 10000;
            memset(aMissionObjectivesArray, 0, sizeof(aMissionObjectivesArray));
            nPendingMedalIndex = -1;
            UpdateSeries();
            Require(stCampaignState.currentSeries == (signed char)pMissionCampaignInfo[(series - 1) * 90 + (variant ? 8 : 6)], "campaign branch");
            if (series >= seriesCount - 1)
                Require(stCampaignState.currentSeries == -1 && nPostSeriesSequence == (campaign == 2 ? 8 : 5), "terminal scene survives");
        }
    }
    puts("PASS: all branch edges and endings");
    for (loseCampaign = 0; loseCampaign < 2; loseCampaign++) {
        campaignFlights = 0;
        StartNewCampaign(campaign);
        SdlConfigureAutosave(1);
        SdlResumeAutosave(1);
        SdlAutosaveCampaign();
        skip = SDL_AddTimer(10, SkipScene, NULL);
        while (GameFlow()) {
            Require(LoadGame(SDL_AUTOSAVE_SLOT, &loaded) &&
                    loaded.campaign.campaignIndex == campaign &&
                    loaded.campaign.currentSeries == stCampaignState.currentSeries &&
                    loaded.campaign.currentMission == stCampaignState.currentMission,
                    "autosave follows campaign progression");
        }
        SDL_RemoveTimer(skip);
        FlushInputEvents();
        Require(campaignFlights == (loseCampaign ? 6 : 16) && !bCampaignActive,
                "complete winning/losing GameFlow path");
        Require(LoadGame(SDL_AUTOSAVE_SLOT, &loaded) &&
                loaded.campaign.currentSeries == (loseCampaign ? 8 : campaign == 2 ? 9 : 7) &&
                loaded.campaign.currentMission == 1, "ending preserves last checkpoint");
        SdlConfigureAutosave(0);
    }
    puts("PASS: complete winning/losing campaign flows and autosaves");
    loseCampaign = 0;
    for (flightOutcome = 2; flightOutcome <= 4; flightOutcome++) {
        for (acceptRetry = 0; acceptRetry <= 1; acceptRetry++) {
            int continues;
            campaignFlights = 0;
            StartNewCampaign(campaign);
            skip = SDL_AddTimer(10, SkipScene, NULL);
            continues = GameFlow();
            SDL_RemoveTimer(skip);
            FlushInputEvents();
            Require(continues == (acceptRetry || flightOutcome == 2), "postflight story outcome");
            Require(campaignFlights == 1 + acceptRetry, "retry repeats same mission");
            Require(stCampaignState.elapsedDate.year == (!acceptRetry && flightOutcome == 2),
                    "ejection history is restored on retry");
            if (continues)
                Require(stCampaignState.currentMission == 1, "survivor advances once");
        }
    }
    flightOutcome = 1;
    puts("PASS: ejection, stranding, death and retry/continue paths");
    StartNewCampaign(campaign);
    saved.occupied = 1;
    strcpy(saved.description, "expansion test");
    saved.campaign = stCampaignState;
    saved.campaign.currentPilot = 0;
    memcpy(saved.pilots, aPilotEntriesList, sizeof(saved.pilots));
    Require(SaveGame(0, &saved) && LoadGame(0, &loaded), "expansion disk save round trip");
    Require(loaded.campaign.campaignIndex == campaign && loaded.campaign.currentSeries == 1, "save campaign retained");
    CheckTitle(SCAN_C, SCAN_S, 1);
    CheckTitle(SCAN_S, SCAN_C, 0);
    CheckTitle(-1, SCAN_C, 0);
    LoadGameFromSlot(0, &stCampaignState, aPilotEntriesList, aMissionObjectivesArray);
    Require(nCampaignDataSet == campaign && nQueuedCampaignIndex == campaign, "load expansion campaign resources");
    Require(rename(moduleFile, "MODULE.tmp") == 0, "hide optional mission data");
    Require(!SdlHasSecretMissions(campaign) && !LoadGame(0, &loaded), "missing expansion rejects expansion saves");
    nLaunchCampaign = 0;
    CheckTitle(SCAN_2, SCAN_S, 0);
    Require(titleKeyIndex == 2, "campaign shortcuts belong to the launcher");
    Require(rename("MODULE.tmp", moduleFile) == 0, "restore mission data");
    nLaunchCampaign = campaign;
    saved.campaign.campaignIndex = 0;
    ResetCampaignData();
    memcpy(saved.pilots, aPilotEntriesList, sizeof(saved.pilots));
    saved.pilots[2].kills = 77;
    saved.pilots[5].missions = 45;
    Require(SaveGame(0, &saved) && LoadGame(0, &loaded), "WC1 saves still load");
    nLaunchCampaign = 0;
    CheckTitle(SCAN_C, SCAN_S, 1);
    CheckTitle(SCAN_T, SCAN_S, 0);
    Require(titleKeyIndex == 2, "WC1 title has no expansion transfer option");
    CheckTitle(SCAN_M, SCAN_S, 0);
    Require(titleKeyIndex == 2, "WC1 title has no expansion selection");
    nLaunchCampaign = campaign;
    saved.pilots[8].rank = 3;
    saved.pilots[8].kills = 42;
    saved.pilots[8].missions = 20;
    saved.campaign.medals[2] = saved.campaign.badges[10] = 1;
    saved.campaign.personalityDeathMission[2] = 5;
    saved.campaign.currentSeries = 13;
    saved.campaign.currentMission = 3;
    Require(SaveGame(0, &saved), "save WC1 transfer source");
    CheckTitle(SCAN_T, SCAN_S, 0);
    Require(titleKeyIndex == 2 && aTitleMenuRegions[2].frame == -1,
            "expansion title retains only original Start/Continue buttons, even with transferable saves");
    nPilotTransferCampaign = campaign;
    nPilotTransferState = 1;
    cancelTransfer = 1;
    Require(BarracksScreen() == 6 && nPilotTransferState == 1, "cancel transfer selection");
    cancelTransfer = 0;
    Require(BarracksScreen() == 7 && nPilotTransferState == 2, "select pilot in transfer barracks");
    nPilotTransferState = 0;
    Require(stCampaignState.campaignIndex == campaign && stCampaignState.currentSeries == 1 &&
            stCampaignState.currentMission == 0 && stCampaignState.seriesHistoryCount == 0 &&
            stCampaignState.personalityDeathMission[2] == 0, "transfer resets campaign progress");
    if (campaign == 2)
        SdlSetSm2Pilots(saved.pilots, 0);
    Require(!memcmp(saved.pilots, aPilotEntriesList, sizeof(saved.pilots)) &&
            stCampaignState.medals[2] == 1 && stCampaignState.badges[10] == 1,
            "transfer preserves roster and awards");
    Require(LoadGame(0, &loaded) && loaded.campaign.campaignIndex == 0 &&
            loaded.campaign.personalityDeathMission[2] == 5, "transfer preserves WC1 save");
    if (campaign == 2) {
        /* The SM2 loader also accepts original converted saves whose nine
         * stored pilots still have the WC1 names and accumulated statistics. */
        saved.campaign = stCampaignState;
        saved.campaign.currentPilot = 0;
        strcpy(saved.pilots[2].name, "CHEN");
        strcpy(saved.pilots[5].name, "TAGGART");
        Require(SaveGame(0, &saved) && LoadGame(0, &loaded) &&
                !strcmp(loaded.pilots[2].name, "COLSON") &&
                !strcmp(loaded.pilots[5].name, "MONTCLAIR") &&
                loaded.pilots[2].kills == 77 && loaded.pilots[5].missions == 45 &&
                loaded.campaign.personalityDeathMission[12] == 0,
                "restore expansion identities without overwriting statistics");
        if (hasSm1) {
            saved.campaign.campaignIndex = 1;
            Require(SaveGame(0, &saved) && SdlTransferPilot(0, 2) &&
                    aPilotEntriesList[2].kills == 77 && aPilotEntriesList[5].missions == 45 &&
                    LoadGame(0, &loaded) && loaded.campaign.campaignIndex == 1,
                    "SM1 to SM2 transfer preserves source save and statistics");
        }
    }
    puts("PASS: transfer UI, cancellation, original conversion rules and source preservation");
    saved.campaign = stCampaignState;
    saved.campaign.currentPilot = 0;
    saved.campaign.campaignIndex = 3;
    Require(SaveGame(0, &saved) && !LoadGame(0, &loaded), "invalid campaign rejected");
    saved.campaign.campaignIndex = campaign;
    Require(SaveGame(0, &saved) && LoadGame(0, &loaded), "restore valid slot before truncation");
    {
        unsigned char prefix[sizeof(SaveGameDiskRecord) - 1];
        FILE *file = fopen("SAVEGAME.WLD", "rb");
        Require(file && fread(prefix, 1, sizeof(prefix), file) == sizeof(prefix), "read saved slot");
        fclose(file);
        file = fopen("SAVEGAME.WLD", "wb");
        Require(file && fwrite(prefix, 1, sizeof(prefix), file) == sizeof(prefix), "truncate saved slot");
        fclose(file);
        Require(!LoadGame(0, &loaded), "truncated save rejected");
    }
    StartNewCampaign(0);
    expected = PacketLoad("CAMP.000", 0, NULL, 0, 0);
    Require(expected && !memcmp(expected, pConstellationDefinitionsPtr, 416), "switch back to WC1 star map");
    ReleasePacketHandle(expected);
    puts("PASS: save/load and WC1 return");
    for (series = campaign == 2 ? 6 : 4; series <= (campaign == 2 ? 8 : 5); series++) {
        for (variant = 0; variant < 2; variant++) {
            if ((series == 4 && variant == 0) || (series == 6 && variant == 1))
                continue;
            nCampaignDataSet = campaign;
            bEscapePressedFlag = 0;
            skip = SDL_AddTimer(100, SkipScene, NULL);
            ShowMeanwhileTransition(series, variant);
            SDL_RemoveTimer(skip);
            FlushInputEvents();
        }
    }
    puts(fullScenes ? "PASS: expansion interlude and endings without skip input (shortened reading pauses)" :
                      "PASS: expansion interlude and both ending scene paths (with skip input)");
    StartNewCampaign(campaign);
    bIxAudioEnabled = 1;
    nMusicPlaybackMode = 1;
    InitializeAudioSystem(window);
    InitializeAudioStreamer(window);
    SetSoundEffectsVolume(32000);
    for (variant = 0; variant < 3; variant++) {
        const int tracks[] = {23, 0, 34};
        spacetrack(tracks[variant], 0, 1);
        Require((dwIxStreamerStatus & 0x87) == 0x87,
                "preflight, flight and postflight streams open and play");
        SDL_Delay(150);
    }
    PlaySfxWaveFileByNumber(4, -1, 0);
    Require(FindWaveTableEntryByName("sfx03.wav") != NULL, "combat sound sample loaded");
    SDL_Delay(150);
    ShutdownAudioStreamer();
    ServiceAudioStream();
    puts("PASS: shared Kilrathi Saga music and sound playback on dummy audio device");
    completion = fopen("probe-complete", "w");
    Require(completion != NULL && fclose(completion) == 0, "record completed probe");
    return 0;
}
