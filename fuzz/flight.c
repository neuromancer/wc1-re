/* libFuzzer entry points shared by the flight fuzzers (see flight.h): mission
 * selection, seed inputs, flight-path travel and the action loop. */
#include "flight.h"

/* Escort waits per input: about three minutes of flight. */
#define ESCORT_WAIT_TICKS 3600

static const char *const campaignNames[] = {"wc1", "sm1", "sm2", "sim"};
/* Per campaign and series; the simulator's missions are its series 0. */
static unsigned char missionCounts[FLIGHT_SIMULATOR + 1][14];
static int campaignFilter = -1, seriesFilter = -1, missionFilter = -1;
static int tickLimit = 600;
static int inputActive;
static int traceInputs;

void Require(int condition, const char *message)
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

static int FlightCaseSelected(int campaign, int series, int mission)
{
    return campaign <= FLIGHT_SIMULATOR && series < 14 && mission < missionCounts[campaign][series] &&
           (campaignFilter < 0 || campaignFilter == campaign) &&
           (seriesFilter < 0 || seriesFilter == series) &&
           (missionFilter < 0 || missionFilter == mission);
}

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

    while (legs-- > 0 && FlightContinues() && cCurrentNavPtIdx >= 0 &&
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
            ViewFlightArrival();
            ReleaseStaleNavTarget();
        }
        /* update_objective_location checks one objective per tick. */
        for (tick = 0; tick <= cMissionObjectiveCountValue && FlightContinues(); tick++)
            FlightStep(-1);
        /* Wait there like the player: flag_reached (SdlSwcFlagReached in SWC)
           holds the leg while an escorted ship is on its way, and that ship
           registers the arrival itself (cruise_to_destination's
           flag_reached(..., 1)). */
        for (tick = 0; waited < ESCORT_WAIT_TICKS && cCurrentNavPtIdx == previousLeg &&
             escorting_a_ship() && find_ship_index(anShipMissionShips[0]) != -1 &&
             FlightContinues(); tick++, waited++)
            FlightStep(-1);
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

static int ActionIndex(const FlightAction *action)
{
    size_t index;

    for (index = 0; index < flightActionCount; index++) {
        if (flightActions[index].kind == action->kind && flightActions[index].key == action->key &&
            flightActions[index].x == action->x && flightActions[index].y == action->y)
            return (int)index;
    }
    Require(0, "seed action");
    return 0;
}

static void WriteSeed(const char *directory, int campaign, int series, int mission, int legs)
{
    char path[PATH_MAX];
    FILE *file;
    unsigned char seed[8 + 2 * 64] = {
        (unsigned char)campaign, (unsigned char)series, (unsigned char)mission,
        (unsigned char)legs, 0, 0, 0, 0
    };
    size_t size = 8;
    size_t step;

    Require(flightSeedLength <= 64, "seed length");
    for (step = 0; step < flightSeedLength; step++) {
        seed[size++] = (unsigned char)(ActionIndex(&flightSeed[step].action) |
                                       (flightSeed[step].keepHeld ? 0x40 : 0));
        seed[size++] = flightSeed[step].hold;
    }
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
    const char *directory = getenv("WC1_FLIGHT_DATA");
    const char *campaignName = getenv("WC1_FLIGHT_CAMPAIGN");
    const char *filter = getenv("WC1_FLIGHT_CASE");
    const char *ticks = getenv("WC1_FLIGHT_TICKS");
    const char *seeds = getenv("WC1_FLIGHT_SEEDS");
    const char *keyBindings = getenv("WC1_FLIGHT_KEYS");
    int list = getenv("WC1_FLIGHT_LIST") != NULL;
    int campaign, series, mission, legs, cases = 0;
    (void)argc;
    (void)argv;

    Require(atexit(CheckFuzzExit) == 0, "register early-exit check");
    inputActive = 1;
    traceInputs = getenv("WC1_FLIGHT_TRACE") != NULL;
    if (keyBindings != NULL)
        SdlSetKeyBindings(keyBindings);
    if (campaignName != NULL)
        campaignFilter = atoi(campaignName);
    if (filter != NULL)
        Require(sscanf(filter, "%d:%d:%d", &campaignFilter, &seriesFilter, &missionFilter) == 3,
                "campaign:series:mission filter");
    if (ticks != NULL)
        tickLimit = atoi(ticks);
    Require(campaignFilter >= -1 && campaignFilter <= FLIGHT_SIMULATOR && tickLimit > 0 &&
            tickLimit <= 3600, "valid campaign and tick budget");
    Require(directory != NULL && chdir(directory) == 0,
            "set WC1_FLIGHT_DATA to the game data; fuzz/runFuzzer.py does");
    StartFlightGame();
    for (campaign = 0; campaign <= FLIGHT_SIMULATOR; campaign++) {
        if (campaign == FLIGHT_SIMULATOR)
            missionCounts[campaign][0] = 4;
        else if (!LoadFlightCampaign(campaign, missionCounts[campaign]))
            continue;
        if (list)
            printf("INSTALLED %d\n", campaign);
        for (series = 0; series < 14; series++) {
            for (mission = 0; mission < missionCounts[campaign][series]; mission++) {
                if (!FlightCaseSelected(campaign, series, mission))
                    continue;
                if (list)
                    printf("CASE %d %d %d\n", campaign, series, mission);
                /* The simulator has no flight path. */
                for (legs = 0; seeds != NULL && legs < (campaign == FLIGHT_SIMULATOR ? 1 : 4); legs++)
                    WriteSeed(seeds, campaign, series, mission, legs);
                cases++;
            }
        }
    }
    inputActive = 0;
    if (list) {
        puts("COMPLETE");
        exit(0);
    }
    Require(cases > 0, "matching missions installed");
    return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    int campaign, series, mission, legs, tick = 0, hold, frame;
    int startLeg, reachedLeg;
    size_t position;

    if (size < 10 || !FlightCaseSelected(data[0], data[1], data[2]))
        return -1;
    campaign = data[0];
    series = data[1];
    mission = data[2];
    legs = campaign == FLIGHT_SIMULATOR ? 0 : data[3] & 15;
    inputActive = 1;
    srand((unsigned int)data[4] | ((unsigned int)data[5] << 8) |
          ((unsigned int)data[6] << 16) | ((unsigned int)data[7] << 24));
    BeginFlight(campaign, series, mission);
    startLeg = cCurrentNavPtIdx;
    TravelFlightPath(legs);
    reachedLeg = cCurrentNavPtIdx;
    for (position = 8; position + 1 < size && tick < tickLimit && FlightContinues(); position += 2) {
        if (!(data[position] & 0x40))
            ReleaseFlightControls();
        hold = (1 + (data[position + 1] & 63)) << (data[position + 1] >> 6);
        ApplyFlightAction(&flightActions[(data[position] & 63) % flightActionCount]);
        for (frame = 0; frame < hold && tick < tickLimit && FlightContinues(); frame++, tick++)
            FlightStep(frame);
    }
    if (traceInputs)
        fprintf(stderr, "TRACE campaign %d series %d mission %d: nav %d -> %d (%d legs asked) "
                "-> %d, %d ticks, arcade state %d, destroyed %d, speed %d at %d %d %d\n",
                campaign, series, mission, startLeg, reachedLeg, legs, cCurrentNavPtIdx, tick,
                nArcadeState, bCurrentPlayerDestroyed, anShipSpeed[0], aShipPositionsArray[0].x,
                aShipPositionsArray[0].y, aShipPositionsArray[0].z);
    ReleaseFlightControls();
    EndFlight();
    nTrainSimActive = 0;
    inputActive = 0;
    return 0;
}
