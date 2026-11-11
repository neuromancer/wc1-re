/* Win32 SM2 flight integration; run with disposable user-supplied assets. */
#include "wc1.h"
static void Require(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "FAIL: %s (%s)\n", message, SDL_GetError());
        exit(1);
    }
}

static Uint32 SkipScene(Uint32 interval, void *unused)
{
    SDL_Event event = {0};
    (void)unused;
    event.type = SDL_KEYDOWN;
    event.key.keysym.scancode = SDL_SCANCODE_SPACE;
    event.key.keysym.sym = SDLK_SPACE;
    SDL_PushEvent(&event);
    event.type = SDL_KEYUP;
    SDL_PushEvent(&event);
    return interval;
}

int main(int argc, char **argv)
{
    SDL_Window *window;
    ObjectTypeData originalDralthi = aObjectTypeData[OBJECT_TYPE_DRALTHI];
    ObjectTypeData originalSpikeri = aObjectTypeData[OBJECT_TYPE_SPIKERI];
    int series, mission, nav, tick, view, mode, damage, count = 0, dralthi = 0;
    int hriss = 0, ship, personality, line;
    short file;
    char speech[80];
    SDL_TimerID skip;
    FILE *completion;
    int flightTicks = getenv("WC1_CAMPAIGN_FLIGHT_TICKS") ?
        atoi(getenv("WC1_CAMPAIGN_FLIGHT_TICKS")) : 30;

    setvbuf(stdout, NULL, _IONBF, 0);
    Require(flightTicks > 0 && flightTicks <= 3600, "flight tick count in 1..3600");
    Require(argc >= 2 && chdir(argv[1]) == 0, "enter scratch installation");
    SDL_SetMainReady();
    Require(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) == 0, "SDL init");
    window = SDL_CreateWindow("SM2 flight test", 0, 0, 640, 400, 0);
    Require(window != NULL, "headless window");
    DIBinstall(window);
    hMainWindow = window;
    SdlStartEventPump();
    bIxAudioEnabled = 0;
    bCurrentRunningFromHardDiskFlag = 1;
    nMusicPlaybackMode = 0;
    Require(chdir("GAMEDAT") == 0, "enter assets");
    LoadInstallDat();
    Require(chdir("..") == 0, "return to installation root");
    Require(!SdlUsingDosData() && !SdlUsingTownsData(), "Kilrathi Saga data");
    ResetCampaignData();
    InitGameClockEpoch();
    nVideoMode = 0x13;
    LoadOriginFxDrivers();
    nFrameIntervalMs = 0;
    memcpy(aShipWeaponsTable[8], aObjectTypeData[OBJECT_TYPE_RAPTOR].weaponLoadout,
           sizeof(aShipWeaponsTable[8]));
    memcpy(aShipWeaponsTable[9], aObjectTypeData[OBJECT_TYPE_HORNET].weaponLoadout,
           sizeof(aShipWeaponsTable[9]));
    remove_weapon(8, 4);
    Require(aShipWeaponsTable[8][0] == 9 &&
            !memcmp(aShipWeaponsTable[9], aObjectTypeData[OBJECT_TYPE_HORNET].weaponLoadout,
                    sizeof(aShipWeaponsTable[9])),
            "firing from a full Raptor loadout preserves the neighboring ship's weapons");
    stCampaignState.campaignIndex = nCampaignDataSet = 2;
    SdlSetSm2Pilots(aPilotEntriesList, 1);
    LoadPacketIntoBuffer(asCampaignPilotFiles[2], 0, pConstellationDefinitionsPtr);
    LoadPacketIntoBuffer(asCampaignPilotFiles[2], 1, pMissionCampaignInfo);
    for (series = 1; series <= 9; series++) {
        for (mission = 0; mission < pMissionCampaignInfo[(series - 1) * 90 + 2]; mission++) {
            stCampaignState.currentSeries = series;
            stCampaignState.currentMission = mission;
            nArcadeState = 0;
            Require(init_mission(series, mission) == 0, "initialize SM2 flight");
            set_up_action_sphere(stCurrentMissionHeader.entryNavPoint);
            bEscapePressedFlag = 0;
            LaunchPlayerShip();
            copy_frame(0, 62);
            Require(aObjectTypeData[OBJECT_TYPE_SPIKERI].objectClass == OBJECT_CLASS_SHIP &&
                    aObjectTypeData[OBJECT_TYPE_SPIKERI].shieldFore == 150 &&
                    aObjectTypeData[OBJECT_TYPE_SPIKERI].maximumVelocity == 33,
                    "Hriss fighter definition");
            for (ship = 0; ship < ACTIVE_MISSION_SHIP_COUNT; ship++)
                hriss += aMissionShips[ship].type == OBJECT_TYPE_SPIKERI;
            nFrameSkipCounter = 1;
            Require(RenderSpaceViewFrame() != 0, "render initialized mission");
            update_cockpit();
            if (stCampaignState.playerShipType == OBJECT_TYPE_DRALTHI) {
                if (dralthi == 0 && getenv("WC1_SM2_CAPTURE")) {
                    SDL_Surface *frame = SDL_CreateRGBSurfaceWithFormat(0, 640, 400, 32,
                                                                       SDL_PIXELFORMAT_ARGB8888);
                    Require(frame != NULL, "allocate cockpit capture");
                    DIBslam();
                    DIBslamReal();
                    Require(SDL_RenderReadPixels(SDL_GetRenderer(window), NULL, frame->format->format,
                                                  frame->pixels, frame->pitch) == 0 &&
                            SDL_SaveBMP(frame, getenv("WC1_SM2_CAPTURE")) == 0, "capture cockpit");
                    SDL_FreeSurface(frame);
                }
                Require(cCurrentCockpitView == OBJECT_TYPE_DRALTHI && cCockpitLogicalFile == 81,
                        "Dralthi cockpit archive");
                Require(aShipWeaponsTable[0][0] == 6 &&
                        ((ShipWeaponSlot *)&aShipWeaponsTable[0][1])[0].type == OBJECT_TYPE_MASS_DRIVER_CANNON,
                        "Dralthi weapons");
                for (view = 0; view < 4; view++) {
                    new_view(view, 0);
                    RenderSpaceViewFrame();
                }
                new_view(0, 0);
                for (mode = 0; mode < 3; mode++) {
                    static const int leftModes[] = {1, 2, 8};
                    static const int rightModes[] = {3, 5, 4};
                    if (mode == 2) {
                        ResetCommMenuChoices(0);
                        OpenCommMenuForTarget("Test", "Test");
                        SendCommMenuChoice(0);
                    }
                    set_mode(0, leftModes[mode]);
                    set_mode(1, rightModes[mode]);
                    RenderSpaceViewFrame();
                    update_cockpit();
                }
                EndCommMenu();
                set_mode(0, 1);
                set_mode(1, 5);
                acPlayerComponentDamageList[0] = 1;
                show_damage_disp();
                for (damage = 0; damage < 4; damage++)
                    place_damage_on_cockpit(damage);
                for (view = 0; view <= 1; view++) {
                    bCockpitlessView = view;
                    cScreenViewMode = -1;
                    initialize_cockpit(0);
                    print_message_text("SM2 cockpit check", cPrimaryTextColour);
                    RenderSpaceViewFrame();
                }
                bCockpitlessView = 0;
                cScreenViewMode = -1;
                initialize_cockpit(0);
                dralthi++;
            }
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
                for (tick = 0; tick < flightTicks && !nArcadeState; tick++) {
                    Update_3Space();
                    RenderSpaceViewFrame();
                    update_cockpit();
                }
            }
            free_cockpit();
            free_all_slots();
            free_3Space();
            printf("PASS: flight %d/%d (ship %d)\n", series, mission,
                   stCampaignState.playerShipType);
            count++;
        }
    }
    Require(count == 18 && dralthi == 4 && hriss > 0, "18 missions, four Dralthi flights, Hriss encounters");
    printf("PASS: up to %d flight ticks per nav point\n", flightTicks);
    file = OpenDataFileOrDie(pDiskFileRecords[82].name);
    for (personality = 0; personality < 14; personality++) {
        get_pilot_talk(personality);
        for (line = 0; line < 11; line++) {
            Require(ReadDataFileAtOffset(file, (personality * 11 + line) * 80, 80, speech),
                    "read expected SM2 radio line");
            Require(!strcmp(speech, aapszPilotSpeeches[personality][line]), "SM2 radio text");
        }
        if (personality == 2 || personality == 5) {
            void *portrait = FetchDiskPacketRetrying(83, personality == 5 ? 0 : 1, 0);
            Require(!memcmp(portrait, apCommunicationPortraitShapesTable[personality],
                            GetPacketSize(pDiskFileRecords[83].name, personality == 5 ? 0 : 1)),
                    "expansion pilot portrait");
            ReleasePacketHandle(portrait);
        }
        free_pilot_talk(personality);
    }
    CloseDataFile(file);
    puts("PASS: all SM2 radio lines and expansion portraits");
    stCampaignState.playerShipType = OBJECT_TYPE_DRALTHI;
    skip = SDL_AddTimer(10, SkipScene, NULL);
    bEscapePressedFlag = 0;
    scramble();
    for (damage = 0; damage < 4; damage++)
        landing(damage);
    SDL_RemoveTimer(skip);
    FlushInputEvents();
    puts("PASS: Dralthi launch and landing at all damage levels");
    for (mode = 0; mode < 2; mode++) {
        stCampaignState.campaignIndex = nCampaignDataSet = mode;
        LoadPacketIntoBuffer(asCampaignPilotFiles[mode], 0, pConstellationDefinitionsPtr);
        Require(init_mission(1, 0) == 0, "return to WC1/SM1 flight");
        nCameraViewMode = -1;
        new_view(0, 0);
        Require(!memcmp(aObjectTypeData[OBJECT_TYPE_DRALTHI].weaponLoadout,
                        originalDralthi.weaponLoadout, sizeof(originalDralthi.weaponLoadout)) &&
                aObjectTypeData[OBJECT_TYPE_SPIKERI].objectClass == originalSpikeri.objectClass &&
                aObjectTypeData[OBJECT_TYPE_SPIKERI].shieldFore == originalSpikeri.shieldFore,
                "WC1/SM1 definitions restored");
        Require(RenderSpaceViewFrame() != 0, "render WC1/SM1 after SM2");
        free_cockpit();
        free_all_slots();
        free_3Space();
    }
    puts("PASS: campaign isolation");
    completion = fopen("probe-complete", "w");
    Require(completion != NULL && fclose(completion) == 0, "record completed probe");
    return 0;
}
