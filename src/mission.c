/* Mission setup, ship resources, formation placement, and pilot speech.
 * The following joystick and navigation helpers remain here provisionally;
 * their exact original object boundaries are unresolved. See docs/ORDER.md.
 */
#include "wc1.h"

short nDebriefingPersonality = 0; /* 0x00465c80 */
#ifndef SDL_PORT
static
#endif
int bInitialFormationSetup = 0; /* 0x00465c84 */
void *aapPacketReferences[4][0x25] = {0}; /* 0x00465c88 */
#ifndef SDL_PORT
static
#endif
/* 0x00465ed8 */
ShortVector aaFormationPositions[5][8] = {
    {
        { 0, 0, 0 }, { -750, 0, 0 }, { 750, 0, 0 }, { 0, 0, -750 },
        { 0, 0, 750 }, { -750, 0, -750 }, { 750, 0, -750 },
        { 0, 0, -1500 }
    },
    {
        { 0, 0, 0 }, { 750, 0, 0 }, { -750, -100, -250 },
        { 1500, -100, -250 }, { -1500, -200, -500 },
        { -2250, -300, -750 }, { 2250, -200, -500 },
        { 3000, -300, -750 }
    },
    {
        { 0, 0, 0 }, { 750, 0, -500 }, { -750, 0, -500 },
        { 0, 0, -1000 }, { -1500, 0, -1000 }, { -750, 0, -1500 },
        { 1500, 0, -1000 }, { 750, 0, -1500 }
    },
    {
        { 0, 0, 0 }, { 750, 0, -250 }, { 0, 325, -500 },
        { 750, -325, -750 }, { 0, 325, -500 }, { 750, -325, -750 },
        { 0, 0, -1000 }, { 750, 0, -1250 }
    },
    {
        { 0, 0, 0 }, { 0, 0, -750 }, { -750, 0, -500 },
        { -750, 0, -1250 }, { 750, 0, -500 }, { 750, 0, -1250 },
        { 0, 500, -500 }, { 0, 500, -1250 }
    }
};
#ifndef SDL_PORT
static
#endif
/* 0x00465fc8 */
int anPilotTurnInterval[18] = {
    5, 5, 4, 4, 3, 3, 3, 2, 2, 1, 3, 3, 3, 3, 2, 2, 2, 1
};

/* Linker commons defined here; LINK orders them by identifier hash. */
unsigned char abSeriesAuxData[0x28]; /* 0x005a8240 */
MissionObjectiveSource aMissionObjectiveSources[16]; /* 0x005a8270 */
MissionHeaderDisk stCurrentMissionHeader; /* 0x005a8690 */
CampaignDate *pCurrentCampaignDate; /* 0x005a86a8 */
CampaignDate *pElapsedCampaignDatePtr; /* 0x005a86ac */
signed char cCurrentObjResourceLogicalFile; /* 0x005a86b0 */
Viewport *pScrambleView; /* 0x005a86b4 */
short nSceneScrambleCanopyFrameValue; /* 0x005a86b8 */

/* Function start: 0x40B670 */
void set_sphere_point(const MissionShipRecord *record,
                      FixedVector *position)
{
    AddFixedVectors(
        &aMissionNavPoints[record->navPoint].position,
        &record->position, position);
}

/* Function start: 0x40B6A0 */
unsigned int is_alive(int pilot)
{
    if (pilot <= 4)
        return 1;
    if (pilot == 13)
        return nArcadeState != 4;
    if (pilot >= 5 && pilot <= 12)
        return stCampaignState.personalityDeathMission[
            pilot - 5] == 0;
    if (pilot >= 14 && pilot <= 17)
        return (unsigned int)ace_status((short)(pilot - 14), 1);
    return 0;
}

/* Function start: 0x40B700 */
unsigned int check_futurion(short i)
{
    unsigned int prev;

    if (aeShipMissionType[i] == MISSION_TYPE_WARP_ARRIVE) {
        prev = aeObjectClass[i];
        aeObjectClass[i] = OBJECT_CLASS_FUTURION;
        asObjectCounterArray[i] = prev;
    }
    return 0;
}

/* Function start: 0x40B730 */
unsigned int init_mission(short series, short mission)
{
#ifdef SDL_PORT
    if (LoadMissionData(series, mission) != 0)
        return 1;
#else
    LoadMissionData(series, mission);
#endif
    init_3Space_objects(series);
    nSceneResourceBudgetValue = LoadPacketResourceList(
        aMissionResourceDescriptors, 0,
        nAvailableGameMemory);
    aObjectTypeData[OBJECT_TYPE_DEBRIS_WING].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet;
    prepare_mission();
    InitializeCockpitResources(
        (signed char)(series == 0 ? 4 :
            stCampaignState.playerShipType));
    return 0;
}

/* Function start: 0x40B7A0 */
void prepare_mission(void)
{
    short playerShip;
    short initial;
    short missionShip;
    short pilot;

    *(volatile short *)&stCampaignState.missionScore = 0;
    *(volatile int *)&nCurrentWingmanKilledThisMission = 0;
    playerShip = *(volatile short *)&stCurrentMissionHeader.playerMissionShip;
    *(volatile int *)&bCurrentPlayerDestroyed = 0;
    *(volatile short *)&nMissionMedalScoreValue = 0;
    *(volatile short *)&nWingmanKillCount = 0;
    nPlayerKillTotal = 0;

    stCampaignState.playerShipType = aMissionShips[playerShip].type;
    load_ship(stCampaignState.playerShipType, 0);
    set_objects_data(0, stCampaignState.playerShipType, -1);
    aMissionShips[playerShip].navPoint = stCurrentMissionHeader.entryNavPoint;
    if (nTrainSimActive == 0 && nStartNavPointOverride != -1)
        aMissionShips[playerShip].navPoint = nStartNavPointOverride;
    Set_up_ship_info(0, playerShip, -1);

    memset(acPlayerComponentDamageList, 0,
           sizeof(acPlayerComponentDamageList));
    bInitialFormationSetup = 1;
    nYourWingman = -1;
    initial = 0;
    do {
        missionShip = stCurrentMissionHeader.initialMissionShips[initial];
        if (missionShip != -1) {
            pilot = (short)aMissionShips[missionShip].behaviour.pilot;
            if (is_alive(pilot) != 0 &&
                find_ships_sphere(missionShip) == -1) {
                init_ship(missionShip, stCurrentMissionHeader.entryNavPoint);
                if (nYourWingman == -1 &&
                    pilot > 4 && pilot < 14)
                    nYourWingman = DAT_0046c010;
            }
        }
        initial++;
    } while (initial < 8);
    bInitialFormationSetup = 0;

    Build_objective_list();
    missionShip = 0;
    nCarrierMissionShipIndexValue = missionShip;
#ifdef SDL_PORT
    /* The original scans to 64 even though the initialized table ends after
     * record 47.  Valid missions place the carrier in the loaded first 32;
     * stop at the proven storage boundary in the sanitizer-enabled port. */
    while (missionShip < MISSION_SHIP_STORAGE_COUNT &&
           aMissionShips[missionShip].type !=
               OBJECT_TYPE_TIGERS_CLAW) {
#else
    while (aMissionShips[missionShip].type !=
               OBJECT_TYPE_TIGERS_CLAW &&
           (nCarrierMissionShipIndexValue = missionShip) <
               MISSION_SHIP_SCAN_LIMIT) {
#endif
        missionShip++;
        nCarrierMissionShipIndexValue = missionShip;
    }
    nTargetLockMode = 0;
    bLandingAuthorized = 0;
}

/* Function start: 0x40B940 */
int release_all_capital_ship_shapes(void)
{
    short obj;

    obj = 0;
    do {
        if (aeObjectClass[obj] ==
            OBJECT_CLASS_CAPITAL_SHIP) {
            FreePacketAndClear(&apObjShapeArray[obj], 0);
            asCapitalShipViewFramesTable[obj] = -1;
        }
        obj++;
    } while (obj < 10);
    return 0;
}

/* Function start: 0x40B990 */
int release_capital_ship_shapes(enum ObjectType type)
{
    short obj;

    if (aObjectTypeData[type].objectClass ==
        OBJECT_CLASS_CAPITAL_SHIP) {
        obj = 1;
        do {
            if (aeObjectTypeList[obj] == type) {
                FreePacketAndClear(&apObjShapeArray[obj], 0);
                asCapitalShipViewFramesTable[obj] = -1;
            }
            obj++;
        } while (obj < 10);
    }
    return 0;
}

/* Function start: 0x40B9F0 */
int load_ship(enum ObjectType type, short slot)
{
    short obj;
    short section;

    if (type != -1) {
        if (aObjectResourceSlotsArray[slot].shapeSet == 0) {
            aObjectResourceSlotsArray[slot].type = type;
            cCurrentObjResourceLogicalFile =
                (signed char)(type + 22);
            if (type == OBJECT_TYPE_ASTEROID_FIELD) {
                aObjectTypeData[
                    OBJECT_TYPE_ROCK_CHUNK].shapeSet =
                    FetchDiskPacketRetrying(3, 13, 0);
                aObjectTypeData[
                    OBJECT_TYPE_ASTEROID5].shapeSet =
                    FetchDiskPacketRetrying(3, 16, 0);
                aObjectTypeData[
                    OBJECT_TYPE_ASTEROID3].shapeSet =
                    aObjectTypeData[
                        OBJECT_TYPE_ASTEROID5].shapeSet;
                aObjectTypeData[
                    OBJECT_TYPE_ASTEROID1].shapeSet =
                    aObjectTypeData[
                        OBJECT_TYPE_ASTEROID5].shapeSet;
                aObjectResourceSlotsArray[slot].shapeSet =
                    aObjectTypeData[
                        OBJECT_TYPE_ASTEROID5].shapeSet;
                if (nMemoryConfig == 2) {
                    aObjectTypeData[
                        OBJECT_TYPE_ASTEROID6].shapeSet =
                        FetchDiskPacketRetrying(
                            3, 17, 0);
                    aObjectTypeData[
                        OBJECT_TYPE_ASTEROID4].shapeSet =
                        aObjectTypeData[
                            OBJECT_TYPE_ASTEROID6].shapeSet;
                    aObjectTypeData[
                        OBJECT_TYPE_ASTEROID2].shapeSet =
                        aObjectTypeData[
                            OBJECT_TYPE_ASTEROID6].shapeSet;
                }
                obj = 10;
                do {
                    if (aeObjectClass[obj] ==
                        OBJECT_CLASS_ASTEROID) {
                        apObjShapeArray[obj] =
                            aObjectTypeData[
                                aeObjectTypeList[obj]].shapeSet;
                    }
                    obj++;
                } while (obj <= SPACE_LAST_MOVING_OBJECT);
                return 0;
            }
            if (type != OBJECT_TYPE_MINE_FIELD) {
                if (aObjectTypeData[type].objectClass !=
                        OBJECT_CLASS_SHIP &&
                    aObjectTypeData[type].objectClass !=
                        OBJECT_CLASS_MISSILE) {
                    if (bCurrentExpandedMemoryAvail != 0) {
                        section = 0;
                        do {
                            aapPacketReferences[slot]
                                [section] =
                                FetchDiskPacketRetrying(
                                    (short)
                                        cCurrentObjResourceLogicalFile,
                                    section, 4);
                            if (aapPacketReferences[slot]
                                    [section] == 0)
                                break;
                            section++;
                        } while (section < 0x25);
                    }
                    aObjectResourceSlotsArray[slot].shape =
                        FetchDiskPacketRetrying(
                            (short)cCurrentObjResourceLogicalFile,
                            0x25, 0);
                    aObjectTypeData[type].shape =
                        aObjectResourceSlotsArray[slot].shape;
                    obj = 1;
                    do {
                        if (aeObjectTypeList[obj] == type) {
                            FreePacketAndClear(
                                &apObjShapeArray[obj], 0);
                            asCapitalShipViewFramesTable[obj] = -1;
                        }
                        obj++;
                    } while (obj < 10);
                    return 0;
                }

                aObjectResourceSlotsArray[slot].shapeSet =
                    FetchDiskPacketRetrying(
                        (short)cCurrentObjResourceLogicalFile,
                        0, 0);
                aObjectTypeData[type].shapeSet =
                    aObjectResourceSlotsArray[slot].shapeSet;
                aObjectResourceSlotsArray[slot].animation =
                    FetchDiskPacketRetrying(
                        (short)cCurrentObjResourceLogicalFile,
                        2, 0);
                aObjectTypeData[type].animation =
                    aObjectResourceSlotsArray[slot].animation;
                aObjectResourceSlotsArray[slot].shape =
                    FetchDiskPacketRetrying(
                        (short)cCurrentObjResourceLogicalFile,
                        1, 0);
                aObjectTypeData[type].shape =
                    aObjectResourceSlotsArray[slot].shape;
                obj = 0;
                do {
                    if (aeObjectClass[obj] >=
                            OBJECT_CLASS_MISSILE &&
                        aeObjectTypeList[obj] == type) {
                        apObjShapeArray[obj] =
                            aObjectResourceSlotsArray[slot]
                                .shapeSet;
                    }
                    obj++;
                } while (obj < 10);
            }
        }
    }
    return 0;
}

/* Function start: 0x40BC70 */
int free_ship(short slot)
{
    enum ObjectType type;
    short obj;
    short section;

#ifdef SDL_PORT
    /* The carrier-return sequence also frees empty slots. Their -1 type
     * becomes an unsigned enum index on native builds. */
    if (aObjectResourceSlotsArray[slot].type == -1)
        return 0;
#endif
    type = (enum ObjectType)aObjectResourceSlotsArray[slot].type;

    if (aObjectTypeData[type].objectClass == OBJECT_CLASS_CAPITAL_SHIP) {
        release_capital_ship_shapes(type);
        if (bCurrentExpandedMemoryAvail != 0) {
            section = 0;
            do {
                FreePacketAndClear(
                    &aapPacketReferences[slot][section],
                    4);
                section++;
            } while (section < 0x25);
        }
        FreePacketAndClear(&aObjectResourceSlotsArray[slot].shape, 0);
        aObjectTypeData[type].shape = 0;
    }
    if (aObjectResourceSlotsArray[slot].shapeSet == 0)
        return 0;

    FreePacketAndClear(&aObjectResourceSlotsArray[slot].shapeSet, 0);
    if (type == OBJECT_TYPE_ASTEROID_FIELD) {
        FreePacketAndClear(
            &aObjectTypeData[OBJECT_TYPE_ROCK_CHUNK].shapeSet,
            0);
        FreePacketAndClear(
            &aObjectTypeData[OBJECT_TYPE_ASTEROID2].shapeSet,
            0);
        aObjectTypeData[OBJECT_TYPE_ASTEROID6].shapeSet = 0;
        aObjectTypeData[OBJECT_TYPE_ASTEROID5].shapeSet = 0;
        aObjectTypeData[OBJECT_TYPE_ASTEROID4].shapeSet = 0;
        aObjectTypeData[OBJECT_TYPE_ASTEROID3].shapeSet = 0;
        aObjectTypeData[OBJECT_TYPE_ASTEROID1].shapeSet = 0;
        obj = 10;
        do {
            if (aeObjectTypeList[obj] == OBJECT_TYPE_ROCK_CHUNK)
                remove_object(obj);
            else if (aeObjectClass[obj] ==
                     OBJECT_CLASS_ASTEROID)
                apObjShapeArray[obj] = 0;
            obj++;
        } while (obj <= SPACE_LAST_MOVING_OBJECT);
        return 0;
    }

    if (type != OBJECT_TYPE_MINE_FIELD) {
        aObjectTypeData[type].shapeSet = 0;
        FreePacketAndClear(&aObjectResourceSlotsArray[slot].animation, 0);
        aObjectTypeData[type].animation = 0;
        FreePacketAndClear(&aObjectResourceSlotsArray[slot].shape, 0);
        aObjectTypeData[type].shape = 0;
        obj = 0;
        do {
            if (aeObjectClass[obj] >=
                    OBJECT_CLASS_MISSILE &&
                aeObjectTypeList[obj] == type)
                apObjShapeArray[obj] = 0;
            obj++;
        } while (obj < 10);
    }
    return 0;
}

/* Function start: 0x40BE20 */
int free_all_slots(void)
{
    short slot = 0;

    GetScreenUpdateFlag();
    release_all_capital_ship_shapes();
    do {
        if (aObjectResourceSlotsArray[slot].type != -1)
            free_ship(slot);
        slot++;
    } while (slot < 3);
    initialize_view_buffer();
    return 0;
}

/* Function start: 0x40BE60 */
int load_all_slots(void)
{
    enum ObjectType type;
    short slot;

    GetScreenUpdateFlag();
    slot = 0;
    release_all_capital_ship_shapes();
    do {
        type = (enum ObjectType)
            aObjectResourceSlotsArray[slot].type;
        if (type != -1)
            load_ship(type, slot);
        slot++;
    } while (slot < 3);
    return 0;
}

/* Function start: 0x40BEA0 */
void remove_nav_point_objects(void)
{
    short i = 0;

    do {
        remove_object(i);
        i = i + 1;
    } while (i < 10);
}

/* Function start: 0x40BEC0 */
short get_shape_slot(void)
{
    short slot = 0;

    do {
        if (aObjectResourceSlotsArray[slot].type == -1)
            return slot;
        slot++;
    } while (slot < 4);
    return -1;
}

/* Function start: 0x40BEF0 */
int shape_loaded(enum ObjectType type)
{
    short slot = 0;

    do {
        if ((enum ObjectType)
                aObjectResourceSlotsArray[slot].type == type)
            return 1;
        slot++;
    } while (slot < 4);
    return 0;
}

/* Function start: 0x40BF20 */
int shape_needed(const MissionNavPoint *navPoint, enum ObjectType type)
{
    short preload;

    if (type != (enum ObjectType)-1) {
        preload = 0;
        do {
            if (navPoint->preloadObjectTypes[preload] == type)
                return 1;
            preload++;
        } while (preload < 2);
    }
    return 0;
}

/* Function start: 0x40BF50 */
int new_sphere_shapes(MissionNavPoint *navPoint)
{
    ObjectResourceSlot *resource;
    short slot;
    short preload;
    enum ObjectType type;

#ifdef SDL_PORT
    /* The SWC renderer owns CMF textures and loads each active type on demand.
       WC1's disk packet cache and viewport setup have no SDL resource here. */
    if (SdlSwcFlightActive())
        return 0;
#endif
    GetScreenUpdateFlag();
    slot = 1;
    release_all_capital_ship_shapes();
    do {
        resource = &aObjectResourceSlotsArray[slot];
        if (resource->type != -1 &&
            !shape_needed(navPoint, (enum ObjectType)resource->type)) {
            free_ship(slot);
            resource->type = -1;
        }
        slot++;
    } while (slot < 3);

    preload = 0;
    do {
        type = navPoint->preloadObjectTypes[preload];
        if (type != (enum ObjectType)-1) {
            if (!shape_loaded(type)) {
                slot = get_shape_slot();
                if (slot != -1)
                    load_ship(type, slot);
            }
        }
        preload++;
    } while (preload < 2);
    initialize_view_buffer();
    return 0;
}

/* Function start: 0x40BFF0 */
int set_up_action_sphere(short navPoint)
{
    MissionNavPoint *nav;
    short obj;
    short entry;
    short trigger;
    short objective;
    signed char triggerType;

#ifdef SDL_PORT
    /* SWC CODE_04 +0x0c9e clears glare on entering an action sphere. */
    if (SdlSwcFlightActive()) {
        nSwcSpaceGlareTicks = 0;
        bSwcExplosionGlare = 0;
    }
#endif
    nCurrentNavigationPoint = navPoint;
    nav = &aMissionNavPoints[navPoint];
    nCurrentWave =
        aMissionNavPoints[navPoint + 1].type == 2 ? 2 : -1;
    nEnemySighting = 0x7fff;

    obj = 1;
    do {
        if (aeObjectClass[obj] != OBJECT_CLASS_NULL &&
            acShipSpawnNavPt[obj] != -1) {
            if (aeObjectClass[obj] >= OBJECT_CLASS_SHIP &&
                aeShipMissionType[obj] == MISSION_TYPE_ROUT) {
                aMissionShips[
                    nShipMissionIndices[obj]].state = 3;
            }
            if (aeObjectClass[obj] ==
                OBJECT_CLASS_CAPITAL_SHIP) {
                FreePacketAndClear(&apObjShapeArray[obj], 0);
            }
            if (asObjectScreenXList[obj] != -0x7fff)
                explode(-1, obj);
            else
                remove_object(obj);
        }
        obj++;
    } while (obj < 10);
    remove_all_hazards();
    nCurrentHazardFieldTotal = 0;
    new_sphere_shapes(nav);

    entry = 0;
    do {
        if (nav->missionShips[entry] != -1)
            init_ship(nav->missionShips[entry], navPoint);
        entry++;
    } while (entry < 10);

    trigger = 0;
    do {
        triggerType = ((signed char *)nav->triggers)[trigger];
        if (triggerType != -1) {
            aMissionNavPoints[
                ((signed char *)nav->triggers)[trigger + 1]].type =
                triggerType;
        }
        trigger += 2;
    } while (trigger < 8);

    objective = 0;
    while (objective < cMissionObjectiveCountValue) {
        LocateMobileObjective(objective);
        objective++;
    }
    clean_up_cockpit();
    bLandingAuthorized = 0;
    return 0;
}

/* Function start: 0x40C150 */
void free_pilot_talk(short personality)
{
    short line;

    line = 0;
    do {
        if (aapszPilotSpeeches[personality][line] != 0) {
            FreePacketAndClear(
                &aapszPilotSpeeches[personality][line], 0);
        }
        if (apCommunicationPortraitShapesTable[personality] != 0) {
            FreePacketAndClear(
                &apCommunicationPortraitShapesTable[personality], 8);
        }
        line++;
    } while (line < 11);
}

/* Function start: 0x40C1A0 */
/* Nothing in the shipped executable calls this. */
void FreeAllPilotTalk(void)
{
    short personality;

    for (personality = 0; personality < 14; personality++) {
        free_pilot_talk(personality);
    }
}

/* Function start: 0x40C1C0 */
void get_pilot_talk(short personality)
{
    char speech[84];
    short file;
    short line;
    short loaded;

    free_pilot_talk(personality);
    PromptInsertNumberedDisk(13);
    loaded = 1;
    file = OpenDataFileOrDie(pDiskFileRecords[13].name);
    loaded &= (file >= 0);
    for (line = 0; line < 11; line++) {
        loaded &= (ReadDataFileAtOffset(file,
                                        (personality * 11 + line) * 80,
                                        80, speech) != 0);
        if (loaded != 0) {
            aapszPilotSpeeches[personality][line] =
                AllocateTaggedMemory(strlen(speech) + 1, 0);
            DosStrcpy(aapszPilotSpeeches[personality][line], speech);
        }
    }

    CloseDataFile(file);
    if (loaded == 0)
        ShowModalMessage("Error loading pilot speech");
    LoadCommPortraitShape(personality, 4);
}

/* Function start: 0x40C2B0 */
unsigned int init_personalities(void)
{
    short missionShip;
    short personality;
    short face;

    missionShip = 0;
    do {
        personality = (short)
            aMissionShips[missionShip].behaviour.pilot - 5;
        if (personality >= 0 && personality < 8) {
            face = get_face(personality, SIDE_IMPERIAL);
            get_pilot_talk(face);
        }
        if (personality > 8) {
            face = get_face(personality, SIDE_KILRATHI);
            get_pilot_talk(face);
            prepare_ace((short)(personality - 9));
        }
        missionShip++;
    } while (missionShip < ACTIVE_MISSION_SHIP_COUNT);

    face = get_face(-1, SIDE_KILRATHI);
    get_pilot_talk(face);
    face = get_face(-1, SIDE_IMPERIAL);
    get_pilot_talk(face);
    return 0;
}

/* Function start: 0x40C350 */
unsigned int room_for_me(short obj, short minimum)
{
    return 1;
}

/* Function start: 0x40C360 */
void approve_xyz(short obj, short minimum, short maximum)
{
    if (obj == -1 || room_for_me(obj, minimum) != 0)
        return;
    do {
        random_radial(&aShipPositionsArray[0], maximum,
                      &aShipPositionsArray[obj]);
    } while (room_for_me(obj, minimum) == 0);
}

/* Function start: 0x40C3C0 */
void set_up_next_wave(void)
{
    MissionNavPoint *waveNav;
    short entry;

    if (nTrainSimActive != 0) {
        spacetrack(21, 2, 0);
        nArcadeBonusCountdown = 60;
        if (nCurrentWave != -1)
            nArcadeBonusCountdown = 30;
        GetArcadeBonus();
        FigureArcadeTime();
    }

    if (nCurrentWave == -1 || nCannedSceneMode != 0)
        return;

    /* The original indexes through a base biased one MissionNavPoint before
     * aMissionNavPoints.  Preserve that -1 when expressing it as a typed
     * array index: arcade wave 2 is stored in nav record 1. */
    waveNav = &aMissionNavPoints[nCurrentWave + nCurrentNavigationPoint - 1];
    if (waveNav->type == nCurrentWave++) {
        new_sphere_shapes(waveNav);
        waveNav->type = -1;
        for (entry = 0; entry < 10; entry++) {
            approve_xyz(init_ship(waveNav->missionShips[entry],
                                  nCurrentNavigationPoint),
                        5000, 10000);
        }
    } else {
        nCurrentWave = -1;
    }
}

/* Function start: 0x40C4A0 */
unsigned int sub_int_vector(const ShortVector *left,
                            const ShortVector *right,
                            ShortVector *difference)
{
    difference->x = left->x - right->x;
    difference->y = left->y - right->y;
    difference->z = left->z - right->z;
    return 0;
}

/* Function start: 0x40C4E0 */
unsigned int set_formation_position(short obj,
                                    const MissionShipRecord *record)
{
    const MissionShipRecord *leaderRecord;
    short source;

    if (record->formationIndex != -1) {
        source = obj;
        for (leaderRecord = record;
             leaderRecord->leaderMissionIndex != -1;
             leaderRecord = &aMissionShips[
                 leaderRecord->leaderMissionIndex])
            source = find_ship_index(leaderRecord->leaderMissionIndex);

        sub_int_vector(
            &aaFormationPositions[record->formationIndex]
                                 [record->formationSpot],
            &aaFormationPositions[leaderRecord->formationIndex]
                                 [leaderRecord->formationSpot],
            &aShipFormationDeltaList[obj]);
        if (source == 0 && bInitialFormationSetup == 0)
            return 0;
        copy_frame(source, obj);
        set_sphere_point(leaderRecord, &aShipPositionsArray[obj]);
        offset_location(obj, &aShipFormationDeltaList[obj],
                        &aShipPositionsArray[obj]);
        anShipSpeed[obj] = leaderRecord->speed << 8;
    }
    return 0;
}

/* Function start: 0x40C5E0 */
unsigned int Set_up_ship_info(short obj, short missionShip,
                              signed char navPoint)
{
    MissionShipRecord *record;

    asCapitalShipViewFramesTable[obj] = -1;
    acWingmanMessageStatesArray[obj] = -1;
    acPreviousAttackers[obj] = -1;
    asActionCounts[obj] = 0;
    abShipExhaustHeatList[obj] = 0;
    asShipAccumulatedDmg[obj] = 0;
    acShipDmg[obj] = 0;
    asCannedCommands[obj] = 0;
    acShipIonDriveDmg[obj] = 0;
    acShipDestroyedWeaponCountArray[obj] = 0;
    acShipCommunicatorArray[obj] = 0;
    apCannedSequenceList[obj] = 0;
    acShipSpawnNavPt[obj] = navPoint;
    nShipMissionIndices[obj] = missionShip;
    acShipPointingModeList[obj] = 1;

    record = &aMissionShips[missionShip];
    set_sphere_point(record, &aShipPositionsArray[obj]);
    alter_yaw((short)-record->pitch, obj);
    alter_pitch((short)-record->yaw, obj);
    alter_roll(record->roll, obj);
    aeShipSide[obj] = record->side;
    anShipSpeed[obj] = record->speed << 8;
    aiPilotLevel[obj] = record->behaviour.pilot;
    reset_mission_type(obj, record->missionType);
    anShipMissionShips[obj] = record->targetMissionIndex;
    asShipWingLeaderArray[obj] =
        find_ship_index(record->leaderMissionIndex);
    set_formation_position(obj, record);
    zero_vector(&aShipVelocity[obj]);
    init_intelligence_data(obj);
    return 0;
}

/* Function start: 0x40C740 */
unsigned int is_team_member(short missionShip)
{
    short index;

    if (missionShip == stCurrentMissionHeader.playerMissionShip)
        return 1;
    index = 0;
    while (index < 8) {
        if (missionShip == stCurrentMissionHeader.initialMissionShips[index])
            return 1;
        index++;
    }
    return 0;
}

/* Function start: 0x40C780 */
unsigned int find_next_ship_turn_slot(short obj)
{
    int objectIndex;
    short other;

    objectIndex = obj;
    other = 1;
    acTurnRegulatorsArray[objectIndex] = 1;
    acTurnIntervalArray[objectIndex] = anPilotTurnInterval[aiPilotLevel[objectIndex]];
    do {
        if (aeObjectClass[other] == OBJECT_CLASS_SHIP &&
            other != obj &&
            acTurnRegulatorsArray[other] == acTurnRegulatorsArray[objectIndex] &&
            acTurnIntervalArray[other] == acTurnIntervalArray[objectIndex]) {
            other = 1;
            acTurnRegulatorsArray[objectIndex]++;
            if (acTurnIntervalArray[objectIndex] < acTurnRegulatorsArray[objectIndex])
                break;
        }
        other++;
    } while (other <= 9);
    return 0;
}

/* Function start: 0x40C800 */
short init_ship(short missionShip, short navPoint)
{
    MissionShipRecord *record;
    enum ObjectType type;
    FixedVector center;
    short obj;

    if (missionShip == -1)
        return -1;
    record = &aMissionShips[missionShip];
    type = record->type;
    if (type == OBJECT_TYPE_ASTEROID_FIELD ||
        type == OBJECT_TYPE_MINE_FIELD) {
        AddFixedVectors(&aMissionNavPoints[navPoint].position,
                        &record->position, &center);
        add_hazard_field(type, center,
                         (short)(record->speed + 3000),
                         (short)record->behaviour.pilot);
#ifdef SDL_PORT
        return -1;
#endif
    } else {
        obj = find_ship_index(missionShip);
        if (obj != -1 || record->state != 0)
            return -1;
        if (record->missionType != MISSION_TYPE_CANNED_SEQUENCE &&
            is_alive(record->behaviour.pilot) == 0) {
            if (record->behaviour.pilot <= 8)
                return -1;
            record->behaviour.pilot = 3;
        }
        record->navPoint = navPoint;
        if (is_team_member(missionShip) != 0)
            navPoint = -1;
        obj = initialize_ship(type, -1);
        if (obj != -1) {
            Set_up_ship_info(obj, missionShip, (signed char)navPoint);
            find_next_ship_turn_slot(obj);
            check_futurion(obj);
            return obj;
        }
    }
    /* The original hazard path returns the still-uninitialized object slot. */
    return obj;
}

/* Function start: 0x40C950 */
unsigned int init_intelligence_data(short obj)
{
    FixedVector *missionSpot;
    short missionTarget;

    abShipTurnList[obj] = 0;
    clear_alert(obj);
    missionTarget = *(short volatile *)&anShipMissionShips[obj];
    missionSpot = &aShipMissionSpotArray[obj];
    *(enum SpecialManeuver volatile *)&aeSpecialManeuvers[obj] =
        SPECIAL_MANEUVER_NONE;
    *missionSpot = aMissionNavPoints[
        nCurrentNavigationPoint].position;

    switch (aeShipMissionType[obj]) {
    case MISSION_TYPE_ESCORT:
    case MISSION_TYPE_STRIKE:
    case MISSION_TYPE_DEFEND:
    case MISSION_TYPE_WINGMAN:
        anShipMissionShips[obj] = missionTarget;
        break;
    case MISSION_TYPE_GOTO_WARP:
        *missionSpot =
            aMissionNavPoints[missionTarget].position;
        break;
    case MISSION_TYPE_WARP_ARRIVE:
        aeShipTacticArray[obj] = TACTIC_WARP_IN;
        aeShipManeuversTable[obj] = MANEUVER_WARPING_IN;
        if (aeShipSide[obj] == SIDE_KILRATHI)
            break;
        /* fall through */
    case MISSION_TYPE_COME_HOME:
        locate_ship(stCurrentMissionHeader.homeMissionShip, missionSpot);
        break;
    case MISSION_TYPE_CANNED_SEQUENCE:
#ifdef SDL_PORT
        /* The native pointer does not fit in the legacy 32-bit AI slot. */
        apCannedSequenceList[obj] =
            aMissionShips[
                nShipMissionIndices[obj]].behaviour.cannedSequence;
#else
        apCannedSequenceList[obj] = (const short *)aiPilotLevel[obj];
#endif
        aiPilotLevel[obj] = 2;
        advance_canned_sequence(obj);
        break;
    default:
        break;
    }

    if (aiPilotLevel[obj] > 4)
        acShipRating[obj] =
            (signed char)(aiPilotLevel[obj] -
                          RATING_ACE_SPIRIT);
    else
        acShipRating[obj] =
            (signed char)~RATING_PROVINCIAL;
    acShipStressList[obj] = 0;
    return 0;
}

/* Function start: 0x40CAA0 */
short __stdcall SampleBothJoysticks(InputDeviceSample *samples,
                                    short fallback)
{
    GetJoystickPosition((unsigned int *)&samples[0].x,
                        (unsigned int *)&samples[0].y,
                        &samples[0].buttons, 0, fallback);
    GetJoystickPosition((unsigned int *)&samples[1].x,
                        (unsigned int *)&samples[1].y,
                        &samples[1].buttons, 1, fallback);
    return 1;
}

/* Function start: 0x40CAE0 */
int __stdcall SampleJoystickDevice(InputDeviceSample *samples,
                                   short joystick,
                                   unsigned short fallback)
{
    InputDeviceSample *sample;
    int result;
    short sampleIndex;

    if (joystick != 0)
        sampleIndex = 1;
    else
        sampleIndex = 0;
    sample = &samples[sampleIndex];
    result = GetJoystickPosition((unsigned int *)&sample->x,
                                 (unsigned int *)&sample->y,
                                 &sample->buttons, joystick, fallback);
    if (result != 0)
        nCurrentActiveInputDevice = -1;
    return result;
}

/* Function start: 0x40CB20 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
short ReadJoystickButtons(void)
{
    return GetJoystickButtons();
}

/* Function start: 0x40CB30 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
void SampleActiveJoystickDevice(void)
{
    short device;

    device = nCurrentActiveInputDevice;
    if (device != -1)
        SampleJoystickDevice(&aInputDevSamples[device],
                             device, 0);
}

/* Function start: 0x40CB60 */
void DrawNavTextLine(unsigned char alignment, unsigned short colour,
                     const char *format, ...)
{
    pCurTextContext->colour = colour;
    pCurTextContext->alignment = alignment;
    pCurTextContext->textCursor =
        pCurTextContext->text;
#ifdef SDL_PORT
    {
        va_list arguments;

        va_start(arguments, format);
        FormatTextTokens((void (__stdcall *)(int))AppendTextCharacter,
                         format, &arguments);
        va_end(arguments);
    }
#else
    FormatTextTokens((void (__stdcall *)(int))AppendTextCharacter,
                     format, (va_list)(&format + 1));
#endif
    DrawTextString(pCurTextContext->text);
}

/* Function start: 0x40CBB0 */
void SetNavMapCoordinateScaling(short enabled)
{
    nNavMapCoordinateScaling = enabled;
}

/* Function start: 0x40CBC0 */
void ScaleNavMapMarkerSize(short *size)
{
    *size = (short)(*size / (nNavMapScale * 100));
}

/* Function start: 0x40CBE0 */
void ScaleNavMapCoordinates(short *x, short *y, short mapX, short mapY)
{
    *x = (short)((mapX - nNavMapCentreX) /
                 nNavMapScale + 75);
    *y = (short)((nNavMapCenterPositionY - mapY) /
                 nNavMapScale + 67);
}

/* Function start: 0x40CC30 */
void nav_getxy(short *x, short *y, int worldX, int worldZ)
{
#ifdef SDL_PORT
    short mapX;
    short mapY;

    /* MissionObjective is packed; its coordinate fields may be unaligned. */
    mapX = (short)((worldX / 100) >> 8);
    mapY = (short)((worldZ / 100) >> 8);
    if (nNavMapCoordinateScaling != 0)
        ScaleNavMapCoordinates(&mapX, &mapY, mapX, mapY);
    memcpy((void *)x, &mapX, sizeof(mapX));
    memcpy((void *)y, &mapY, sizeof(mapY));
#else
    short mapY;

    *x = (short)((worldX / 100) >> 8);
    mapY = (short)((worldZ / 100) >> 8);
    *y = mapY;
    if (nNavMapCoordinateScaling != 0)
        ScaleNavMapCoordinates(x, y, *x, mapY);
#endif
}

/* Function start: 0x40CC80 */
void CheckPoint(short x, short y)
{
    nNavigationMapMinimumCoordX =
        MinShort(nNavigationMapMinimumCoordX, x);
    nNavMapMaximumPositionX =
        MaxShort(nNavMapMaximumPositionX, x);
    nNavigationMapMinimumPositionY =
        MinShort(nNavigationMapMinimumPositionY, y);
    nNavMapMaximumPosY =
        MaxShort(nNavMapMaximumPosY, y);
}

/* Function start: 0x40CCF0 */
void IncludeNavMapWorldPoint(int worldX, int worldZ)
{
    short x;
    short y;

    nav_getxy(&x, &y, worldX, worldZ);
    CheckPoint(x, y);
}

/* Function start: 0x40CD30 */
void SetScale(void)
{
    short objectiveIndex;
    short ship;
    short halfHeight;
    short halfWidth;
    short height;
    short width;

    SetNavMapCoordinateScaling(0);
    nNavigationMapMinimumCoordX = aMissionObjectivesArray[0].mapX;
    nNavMapMaximumPositionX = aMissionObjectivesArray[0].mapX;
    nNavigationMapMinimumPositionY = aMissionObjectivesArray[0].mapY;
    nNavMapMaximumPosY = aMissionObjectivesArray[0].mapY;
    objectiveIndex = 0;
    while (objectiveIndex < (short)cMissionObjectiveCountValue) {
        if (mobile_objective(objectiveIndex) != 0) {
            ship = find_ship_index((short)
                aMissionObjectivesArray[objectiveIndex].index);
            if (ship != -1)
                aMissionObjectivesArray[objectiveIndex].position =
                    aShipPositionsArray[ship];
        }
        nav_getxy(
            &aMissionObjectivesArray[objectiveIndex].mapX,
            &aMissionObjectivesArray[objectiveIndex].mapY,
            aMissionObjectivesArray[objectiveIndex].position.x,
            aMissionObjectivesArray[objectiveIndex].position.z);
        CheckPoint(aMissionObjectivesArray[objectiveIndex].mapX,
                   aMissionObjectivesArray[objectiveIndex].mapY);
        objectiveIndex++;
    }
    IncludeNavMapWorldPoint(aShipPositionsArray[0].x,
                            aShipPositionsArray[0].z);
    width = (short)(nNavMapMaximumPositionX -
                    nNavigationMapMinimumCoordX);
    height = (short)(nNavMapMaximumPosY -
                     nNavigationMapMinimumPositionY);
    halfWidth = (short)(width / 2);
    nNavMapCentreX =
        (short)(nNavigationMapMinimumCoordX + halfWidth);
    halfHeight = (short)(height / 2);
    nNavMapCenterPositionY =
        (short)(nNavigationMapMinimumPositionY + halfHeight);
    nNavMapScale = MaxShort(
        (short)((width + halfWidth) / 150),
        (short)((halfHeight + height) / 135));
    if (nNavMapScale == 0)
        nNavMapScale = 100;
    SetNavMapCoordinateScaling(1);
}

/* Function start: 0x40CED0 */
void Build_objective_list(void)
{
    MissionObjectiveSource *source;
    MissionShipRecord *ship;
    FixedVector position;
    int type;
    const char *displayName;
    short flightPathCount;
    short sourceIndex;

    flightPathCount = 0;
    SetNavMapCoordinateScaling(0);
    cMissionObjectiveCountValue = 0;
    source = aMissionObjectiveSources;
    type = source->type;
    for (; type != -1; source++, type = source->type) {
        sourceIndex = source->index;
        aMissionObjectivesArray[
            cMissionObjectiveCountValue].flags = 0;
        switch (type) {
        case 0:
            position = aMissionNavPoints[
                sourceIndex].position;
            displayName = aMissionNavPoints[
                sourceIndex].name;
            abFlightPathList[flightPathCount++] =
                cMissionObjectiveCountValue;
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            ship = &aMissionShips[sourceIndex];
            displayName =
                aObjectTypeData[ship->type].displayName;
            set_sphere_point(ship, &position);
            abFlightPathList[flightPathCount++] =
                cMissionObjectiveCountValue;
            break;
        }

        aMissionObjectivesArray[
            cMissionObjectiveCountValue].type = type;
        aMissionObjectivesArray[
            cMissionObjectiveCountValue].index = source->index;
        aMissionObjectivesArray[
            cMissionObjectiveCountValue].name = source->description;
        aMissionObjectivesArray[
            cMissionObjectiveCountValue].position = position;
        nav_getxy(
            &aMissionObjectivesArray[
                cMissionObjectiveCountValue].mapX,
            &aMissionObjectivesArray[
                cMissionObjectiveCountValue].mapY,
            position.x, position.z);
        aMissionObjectivesArray[
            cMissionObjectiveCountValue].displayName = displayName;
        cMissionObjectiveCountValue++;
    }

    abFlightPathList[flightPathCount] = -1;
    aMissionObjectivesArray[
        cMissionObjectiveCountValue].type = -1;
    cCurrentNavPtIdx = 0;
    cCurrentObjective = 0;
    if (cMissionObjectiveCountValue != 0) {
        while (set_new_objective(
                   (short)cCurrentNavPtIdx) == 0)
            cCurrentNavPtIdx++;
    }
}
