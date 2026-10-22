/*
 *  Ship mission logic, resource ownership, and scene animation.
 *
 *  Address range 422010-42572f (see docs/ORDER.md). Interior boundaries
 *  remain provisional; the preceding startup unit has a separate pool
 *  of initialized data and string literals.
 */
#define WC1_LOGIC_SOURCE
#include "wc1.h"
#ifndef SDL_PORT
#include <crtdbg.h>
#endif

PacketResourceDescriptor aCommon3SpaceResources[12] = {
    { &aObjectTypeData[OBJECT_TYPE_THRUSTERS].shapeSet, 3, 0 },
    { &aObjectTypeData[OBJECT_TYPE_EXPLOSION0].shapeSet, 3, 1 },
    { &aObjectTypeData[OBJECT_TYPE_LASER_CANNON].shapeSet, 3, 6 },
    { &aObjectTypeData[OBJECT_TYPE_MASS_DRIVER_CANNON].shapeSet,
      3, 7 },
    { &aObjectTypeData[OBJECT_TYPE_NEUTRON_PARTICLE_GUN].shapeSet,
      3, 8 },
    { &aObjectTypeData[OBJECT_TYPE_LASER_SPARK].shapeSet, 3, 9 },
    { &aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet, 3, 4 },
    { &aObjectTypeData[OBJECT_TYPE_BLUE_SPARK].shapeSet, 3, 10 },
    { &aObjectTypeData[OBJECT_TYPE_RED_SPARK].shapeSet, 3, 11 },
    { &aObjectTypeData[OBJECT_TYPE_SPARK_TRAIL].shapeSet, 3, 12 },
    { &aObjectTypeData[OBJECT_TYPE_SPACE_MINE].shapeSet, 3, 15 },
    { 0, 0, 0 }
}; /* 0x00469bc0 */
PacketResourceDescriptor aMissionResourceDescriptors[5] = {
    { &aObjectTypeData[OBJECT_TYPE_HYPERSPACE_JUMP_FLASH].shapeSet,
      3, 14 },
    { &aObjectTypeData[OBJECT_TYPE_EXPLOSION1].shapeSet, 3, 2 },
    { &aObjectTypeData[OBJECT_TYPE_EXPLOSION2].shapeSet, 3, 3 },
    { &aObjectTypeData[OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet,
      3, 5 },
    { 0, 0, 0 }
}; /* 0x00469c20 */
PacketResourceDescriptor aCockpitResourceDescriptors[19] = {
    { &pTgtLockShapePtr, 8, 0 },
    { &pCurrentPilotHandShapePtr, 2, 3 },
    { &pConfedCommBackground, 11, 0 },
    { &pCommStaticShape, 11, 11 },
    { &pKilrathiCommBackground, 11, 9 },
    { (unsigned char **)&pCockpitExplosionShape, 8, 5 },
    { &pCockpitIndicatorShape, 8, 4 },
    { &pCinematicViewBackdropPtr, 8, 6 },
    { &pLogicRearViewBackdropPtr, 8, 7 },
    { &apCommunicationPortraitShapesTable[0], 11, 1 },
    { &apCommunicationPortraitShapesTable[1], 11, 2 },
    { &apCommunicationPortraitShapesTable[2], 11, 3 },
    { &apCommunicationPortraitShapesTable[3], 11, 4 },
    { &apCommunicationPortraitShapesTable[4], 11, 5 },
    { &apCommunicationPortraitShapesTable[5], 11, 6 },
    { &apCommunicationPortraitShapesTable[6], 11, 7 },
    { &apCommunicationPortraitShapesTable[7], 11, 8 },
    { &apCommunicationPortraitShapesTable[8], 11, 10 },
    { 0, 0, 0 }
}; /* 0x00469c48 */
PacketResourceDescriptor aCockpitSecondaryResources[5] = {
    { &pCockpitDamageShape, -1, 7 },
    { &pCockpitWeaponShapePtr, -1, 9 },
    { &pCockpitIndicatorShape, 8, 4 },
    { &pTgtLockShapePtr, 8, 0 },
    { 0, 0, 0 }
}; /* 0x00469ce0 */
PacketResourceDescriptor aCockpitPrimaryResources[8] = {
    { &pCockpitPilotShape, -1, 4 },
    { &pCockpitDamageShape, -1, 7 },
    { &pCockpitWeaponShapePtr, -1, 9 },
    { &apCockpitShapesList[0], -1, 0 },
    { &apCockpitShapesList[1], -1, 1 },
    { &apCockpitShapesList[2], -1, 2 },
    { &apCockpitShapesList[3], -1, 3 },
    { 0, 0, 0 }
}; /* 0x00469d08 */
unsigned int bIntroSceneResourcesActive = 1; /* 0x00469d48 */
short asConstellationObjectIndices[4] = {
    -1, -1, -1, -1
}; /* 0x00469d50 */
int bCockpitResourcesActive = 0; /* 0x00469d58 */
int b3SpaceObjectsActive = 0; /* 0x00469d5c */
short asSceneAnimationLogicalFiles[8] = {
    63, 64, 65, 66, 67, 68, 69, 70
}; /* 0x00469d60 */
short bSceneAnimationWaitCommand = 0; /* 0x00469d70 */

/* Linker commons defined here; LINK orders them by identifier hash. */
unsigned char *pSceneAnimationSceneDataPtr; /* 0x005a7c54 */
unsigned char *pCurrentSceneAnimationPrimaryShape; /* 0x005a7c58 */
unsigned char *pSceneAnimationTextData; /* 0x005a7c5c */
unsigned char *pSceneAnimationPacketPtr; /* 0x005a7c60 */
SceneAnimationObject *pSceneAnimationObjects; /* 0x005a7c64 */
short nSceneAnimationDelayFrames; /* 0x005a7c68 */
unsigned char *pSceneAnimationDefinitionsPtr; /* 0x005a7c6c */
unsigned char *pSceneAnimationSecondaryShape; /* 0x005a7c70 */
signed char cCockpitLogicalFile; /* 0x005a7c74 */
unsigned char *apCockpitShapesList[4]; /* 0x005a7c80 */
unsigned char *pCinematicViewBackdropPtr; /* 0x005a7c90 */
unsigned char *pLogicRearViewBackdropPtr; /* 0x005a7c94 */
int bCurrentPlayerDestroyed; /* 0x005a7c98 */
short nPlayerKillTotal; /* 0x005a7c9c */

/* Function start: 0x422010 */
short ace_status(short ace, unsigned char bits)
{
    return (stCampaignState.aceFlags[ace] & bits) == bits;
}

/* Function start: 0x422030 */
void unflag_ace(short ace, unsigned char bits)
{
    stCampaignState.aceFlags[ace] &= ~bits;
}

/* Function start: 0x422050 */
void flag_ace(short ace, unsigned char bits)
{
    stCampaignState.aceFlags[ace] |= bits;
}

/* Function start: 0x422060 */
void kill_ace(short ace)
{
    if (ace_status(ace, 1) != 0) {
        unflag_ace(ace, 1);
        flag_ace(ace, 2);
    }
}

/* Function start: 0x422090 */
void ace_greeting(short obj)
{
    short ace = (short)aiPilotLevel[obj] - 14;
    int message;

    message = ace_status(ace, 4) != 0;
    send_message(obj, message);
    flag_ace(ace, 8);
}

/* Function start: 0x4220D0 */
void prepare_ace(short ace)
{
    unflag_ace(ace, 0x1a);
    flag_ace(ace, 0x20);
}

/* Function start: 0x4220F0 */
short signed_random(short range)
{
    return RandomBelowOrEqual((short)(range * 2)) - range;
}

/* Function start: 0x422110 */
int alert_flag(short ship, unsigned int bits)
{
    return (anShipAlertFlags[ship] & bits) != 0;
}

/* Function start: 0x422130 */
/* NOT a constant return: `mov eax,0x59b430 / cmp eax,1 / sbb eax,eax / inc eax`
 * is the classic `>= 1` boolean idiom applied to the *address* 0x0059B430,
 * which is always non-zero -- hence Ghidra folding it to `return 1`.  Written
 * the way the original was, so the idiom survives. */
unsigned int HasSpeechBuffer(void)
{
    return (unsigned int)&anShipAlertFlags >= 1;
}

/* Function start: 0x422140 */
unsigned short set_alert(short i, unsigned int bits)
{
    anShipAlertFlags[i] |= bits;
    return 0;
}

/* Function start: 0x422160 */
void clear_alert(short i)
{
    asCollisionDelayList[i] = 0;
    anShipAlertFlags[i] = 0;
    abCollisionAlertTargets[i] = 0xff;
}

/* Function start: 0x422180 */
void start_collision_alert(short obj, short other)
{
    if ((short)(signed char)abCollisionAlertTargets[obj] != other) {
        abCollisionAlertTargets[obj] = (unsigned char)other;
        steady_object(obj);
    }
    asCollisionDelayList[obj] = 3;
    set_alert(obj, 1);
    asShipAfterburnerTimer[obj] = 0;
    set_special(obj, SPECIAL_MANEUVER_NONE);
}

/* Function start: 0x4221E0 */
void try2end_collision_alert(short obj)
{
    asCollisionDelayList[obj]--;
    if (asCollisionDelayList[obj] <= 0)
        clear_alert(obj);
    else
        set_alert(obj, 2);
}

/* Function start: 0x422220 */
short normal_speed(short obj)
{
    if (aeSpecialManeuvers[obj] !=
            SPECIAL_MANEUVER_AFTERBURNER &&
        real_velocity(obj) <= asShipMaximumSpeedArray[obj])
        return 1;
    return 0;
}

/* Function start: 0x422260 */
short real_crash_time(short obj, short other)
{
    short range;
    short collisionRadius;
    short time;
    short elapsed;
    short step;
    int collisionFound;
    long distance;
    long relativeSpeed;
    FixedVector relativePosition;
    FixedVector relativeVelocity;
    FixedVector travel;
    FixedVector separation;

    collisionRadius = asObjectCollisionRadius[obj];
    collisionRadius += asObjectCollisionRadius[other];
    collisionRadius += 30;
    collisionFound = 0;
    ComputeVectorDelta(&aShipPositionsArray[obj],
                       &aShipPositionsArray[other],
                       &relativePosition);
    distance = Vector_magnitude(&relativePosition);
    if (((long)collisionRadius + 1500) * 0x100 < distance)
        return 0x7fff;

    ComputeVectorDelta(&aShipVelocity[obj],
                       &aShipVelocity[other],
                       &relativeVelocity);
    relativeSpeed = Vector_magnitude(&relativeVelocity);
    if (relativeSpeed == 0)
        return 0x7fbc;

    time = FixedToShortSaturating(
        DivideFixed((int)distance, (int)relativeSpeed));
    if (time >= 30)
        return time;

    ScaleFixedVector(&relativeVelocity, (int)time << 8, &travel);
    AddFixedVectors(&relativePosition, &travel, &separation);
    range = FixedToShortSaturating(
        Vector_magnitude(&separation));
    if (range > collisionRadius) {
        if ((collisionRadius * 2 >> 4) > range)
            return 25;
        return 32000;
    }

    elapsed = 0;
    do {
        if (elapsed >= time)
            break;
        step = MaxShort(1, (short)((time - elapsed) >> 1));
        elapsed += step;
        ScaleFixedVector(&relativeVelocity, (int)elapsed << 8, &travel);
        AddFixedVectors(&relativePosition, &travel, &separation);
        range = FixedToShortSaturating(
            Vector_magnitude(&separation));
        if (collisionRadius >= range)
            collisionFound = 1;
    } while (collisionFound == 0);
    return elapsed;
}

/* Function start: 0x422440 */
void clear_crash_cache(void)
{
    short i = 0;

    do {
        asCollisionPartnerTable[i] = -1;
        i = i + 1;
    } while (i < 10);
}

/* Function start: 0x422460 */
short crash_time(short obj, short other)
{
    if (asCollisionPartnerTable[obj] == other)
        return asCollisionTimes[obj];
    if (other < 10 && asCollisionPartnerTable[other] == obj)
        return asCollisionTimes[other];
    if (aeObjectClass[other] == OBJECT_CLASS_ASTEROID &&
        (asObjectScreenXList[other] == -0x7fff ||
         asObjectScreenXList[obj] == -0x7fff))
        return 0x7fff;
    return real_crash_time(obj, other);
}

/* Function start: 0x4224F0 */
short detect_collisions(short obj)
{
    short candidate;
    short other;
    short time;
    short closestTime;

    candidate = -1;
    closestTime = 30;
    for (other = 0; other <= 60; other++) {
        if (other != obj &&
            aeObjectClass[other] >= OBJECT_CLASS_ASTEROID &&
            aeObjectClass[other] != OBJECT_CLASS_MISSILE) {
            time = crash_time(obj, other);
            if (closestTime > time) {
                closestTime = time;
                candidate = other;
            }
        }
    }
    if (candidate != -1) {
        asCollisionPartnerTable[obj] = candidate;
        asCollisionTimes[obj] = closestTime;
    }
    return candidate;
}

/* Function start: 0x422560 */
unsigned int unactive(short ship)
{
    if (ship != -1 &&
        aeObjectClass[ship] >= OBJECT_CLASS_SHIP &&
        aeShipManeuversTable[ship] != MANEUVER_HARD_BRAKE)
        return 0;
    return 1;
}

/* Function start: 0x422590 */
int are_alive(short obj)
{
    if (unactive(obj) == 0 &&
        aeShipObjectiveList[obj] != OBJECTIVE_WANDER)
        return 1;
    return 0;
}

/* Function start: 0x4225C0 */
int trim_goals(short obj, short amount)
{
    short goal = anYawTarget[obj];

    if (amount < goal)
        anYawTarget[obj] = amount;
    else if (goal < -amount)
        anYawTarget[obj] = -amount;
    goal = anPitchGoal[obj];
    if (amount < goal)
        anPitchGoal[obj] = amount;
    else if (goal < -amount)
        anPitchGoal[obj] = -amount;
    return 0;
}

/* Function start: 0x422640 */
int report_kilrathi_rout(int mode)
{
    short obj;

    for (obj = 0; obj < 10; obj++) {
        if (aeObjectClass[obj] < OBJECT_CLASS_SHIP ||
            aeSpecialManeuvers[obj] ==
                SPECIAL_MANEUVER_UNKNOWN_9)
            continue;
        switch (mode) {
        case 0:
            if (aeShipSide[obj] == SIDE_KILRATHI)
                return 1;
            break;
        case 1:
            if (aeShipSide[obj] == SIDE_KILRATHI &&
                aeShipMissionType[obj] != MISSION_TYPE_ROUT &&
                distance_from_object(0, obj) < 16000)
                return 1;
            break;
        case 2:
            if (aeShipSide[obj] == SIDE_KILRATHI &&
                (aeShipObjectiveList[obj] ==
                     OBJECTIVE_ENGAGE_ENEMY ||
                 aeShipObjectiveList[obj] ==
                     OBJECTIVE_DESTROY_SHIP))
                return 1;
            break;
        }
    }
    if (mode == 1 && nCurrentWave != -1)
        check_next_wave();
    return 0;
}

/* Function start: 0x422710 */
short find_ship_index(short missionShip)
{
    short obj;

    nLogicLastFoundShipValue = -1;
    if (missionShip != -1) {
        for (obj = 0; obj < 10; obj++) {
            if (((aeObjectClass[obj] >= OBJECT_CLASS_SHIP &&
                  aeSpecialManeuvers[obj] !=
                      SPECIAL_MANEUVER_UNKNOWN_9) ||
                 aeObjectClass[obj] == OBJECT_CLASS_FUTURION) &&
                nShipMissionIndices[obj] == missionShip) {
                nLogicLastFoundShipValue = obj;
                return obj;
            }
        }
    }
    return -1;
}

/* Function start: 0x422780 */
short try2rout(short obj)
{
    short canContinue;
    short other;

    canContinue = 0;
    if (nTrainSimActive != 0) {
        canContinue = 1;
    } else {
        other = 0;
        do {
            if (aeObjectClass[other] ==
                    OBJECT_CLASS_CAPITAL_SHIP &&
                aeSpecialManeuvers[other] !=
                    SPECIAL_MANEUVER_UNKNOWN_9 &&
                aeShipSide[obj] ==
                    aeShipSide[other])
                canContinue = 1;
            other++;
        } while (other < 10);
    }
    if (canContinue != 0) {
        acShipStressList[obj] = 0;
        maneuver_complete(obj);
    } else {
        reset_mission_type(obj, MISSION_TYPE_ROUT);
        if (obj == nYourWingman)
            send_message(obj, 9);
    }
    return canContinue == 0;
}

/* Function start: 0x422830 */
signed char no_goal(short ship)
{
    return (anYawTarget[ship] |
            anPitchGoal[ship] |
            anRollGoal[ship]) == 0;
}

/* Function start: 0x422860 */
int being_tailed(short obj, short other)
{
    ship_vs_ship(obj, other);
    return nFacingToTargetValue < -60 &&
           nTargetFacingValue > 85 &&
           nTgtRange < 7000;
}

/* Function start: 0x4228A0 */
int any_enemy_tail(short obj)
{
    short other;

    for (other = 0; other < 10; other++) {
        if (aeObjectClass[other] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuvers[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            aeShipSide[obj] != aeShipSide[other] &&
            acShipTargetTable[other] == obj &&
            being_tailed(obj, other) != 0) {
            nTargetShip = other;
            return 1;
        }
    }
    nTargetShip = -1;
    return 0;
}

/* Function start: 0x422930 */
short detect_enemy_tail(short obj)
{
    short other;

    for (other = 0; other < 10; other++) {
        if (aeObjectClass[other] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuvers[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            aeShipSide[obj] != aeShipSide[other] &&
            acShipTargetTable[other] == obj &&
            being_tailed(obj, other) != 0)
            return other;
    }
    return -1;
}

/* Function start: 0x4229B0 */
int is_ship_tailing_player_target(short obj)
{
    short target = acShipTargetTable[0];

    if (unactive(target) == 0 && being_tailed(target, obj) != 0)
        return 1;
    return 0;
}

/* Function start: 0x4229F0 */
int missile_on_tail(short obj)
{
    short other;

    for (other = 0; other < 10; other++) {
        if (aeObjectClass[other] == OBJECT_CLASS_MISSILE &&
            acShipTargetTable[other] == obj)
            return 1;
    }
    return 0;
}

/* Function start: 0x422A30 */
short select_weighted_value(short *choices)
{
    short roll;

    roll = RandomBelowOrEqual(100) + 1;
    roll -= choices[0];
    while (roll > 0) {
        if (choices[0] == -1)
            break;
        choices += 2;
        roll -= choices[0];
    }
    if (choices[0] == -1)
        return -1;
    return choices[1];
}

/* Function start: 0x422A70 */
unsigned int build_squad_list(short leader)
{
    short obj;
    short index = 1;

    acFormationMemberLists[0] = (signed char)leader;
    acFormationMemberLists[1] = -1;
    for (obj = 0; obj < 10; obj++) {
        if (asShipWingLeaderArray[obj] == leader) {
            acFormationMemberLists[index++] = (signed char)obj;
            acFormationMemberLists[index] = -1;
        }
    }
    return 0;
}

/* Function start: 0x422AC0 */
unsigned int find_squad_center(FixedVector *center)
{
    short count = 0;

    zero_vector(center);
    while (acFormationMemberLists[count] != -1) {
        AddFixedVectors(center, &aShipPositionsArray[
                            (short)acFormationMemberLists[count]],
                        center);
        count++;
    }
    if (count != 0)
        divide_vector(center, (int)count << 8, center);
    return 0;
}

/* Function start: 0x422B30 */
unsigned int init_formation_burst(short obj)
{
    FixedVector center;
    FixedVector *destination;
    short member;
    short index;

    build_squad_list(obj);
    find_squad_center(&center);
    index = 0;
    member = acFormationMemberLists[0];
    while (member != -1) {
        destination = &aShipDestinationsArray[member];
        ComputeVectorDelta(&center, &aShipPositionsArray[member],
                           destination);
        ScaleFixedVector(destination, 0xa00, destination);
        AddFixedVectors(destination, &aShipPositionsArray[member],
                        destination);
        steady_object(member);
        reset_objective(member, OBJECTIVE_BREAK_FORMATION);
        member = acFormationMemberLists[++index];
    }
    return 0;
}

/* Function start: 0x422BE0 */
unsigned int reset_mission_type(short obj,
                                enum ShipMissionType missionType)
{
    if (missionType == MISSION_TYPE_ROUT &&
        aeShipSide[obj] == SIDE_KILRATHI)
        report_kilrathi_rout(1);
    reset_objective(obj, OBJECTIVE_NONE);
    aeShipMissionType[obj] = missionType;
    return 0;
}

/* Function start: 0x422C30 */
unsigned int change_mission_type(short obj,
                                 enum ShipMissionType missionType)
{
    if (aeShipObjectiveList[obj] == OBJECTIVE_ENGAGE_ENEMY)
        aeShipMissionType[obj] = missionType;
    else
        reset_mission_type(obj, missionType);
    return 0;
}

/* Function start: 0x422C70 */
unsigned int reset_objective(short ship, enum ShipObjective objective)
{
    steady_object(ship);
    reset_tactic(ship, TACTIC_NONE);
    aeShipObjectiveList[ship] = objective;
    return 0;
}

/* Function start: 0x422CA0 */
unsigned int alter_objective(short ship, enum ShipObjective objective)
{
    steady_object(ship);
    alter_tactic(ship, TACTIC_NONE);
    aeShipObjectiveList[ship] = objective;
    return 0;
}

/* Function start: 0x422CD0 */
unsigned int reset_tactic(short ship, enum ShipTactic tactic)
{
    reset_maneuver(ship, MANEUVER_NONE);
    aeShipTacticArray[ship] = tactic;
    acShipTargetTable[ship] = -1;
    return 0;
}

/* Function start: 0x422D00 */
unsigned int alter_tactic(short ship, enum ShipTactic tactic)
{
    reset_maneuver(ship, MANEUVER_NONE);
    aeShipTacticArray[ship] = tactic;
    return 0;
}

/* Function start: 0x422D30 */
void reset_maneuver(short ship, short maneuver)
{
    aeShipManeuversTable[ship] = (enum ShipManeuver)maneuver;
    asShipTotal[ship] = 0;
    acShipSequence[ship] = 0;
}

/* Function start: 0x422D60 */
void try2reset_maneuver(short obj, short maneuver)
{
    if (aeShipManeuversTable[obj] != maneuver) {
        reset_maneuver(obj, maneuver);
        steady_object(obj);
    }
}

/* Function start: 0x422D90 */
unsigned int set_special(short ship, enum SpecialManeuver special)
{
    enum SpecialManeuver *currentState;
    enum SpecialManeuver current;

    currentState = &aeSpecialManeuvers[ship];
    current = *currentState;
    if (current < SPECIAL_MANEUVER_LOST_CONTROL || special > current)
        *currentState = special;

    if (*currentState == SPECIAL_MANEUVER_BLOWING_UP &&
        (short)alert_flag(ship, 1))
        *currentState = SPECIAL_MANEUVER_NONE;
    return 0;
}

/* Function start: 0x422DD0 */
unsigned int approach_zero_speed(short ship)
{
    approach_speed(ship, 0);
    return 0;
}

/* Function start: 0x422DF0 */
unsigned int approach_min_speed(short obj)
{
    approach_speed(obj, 0x500);
    return 0;
}

/* Function start: 0x422E10 */
unsigned int approach_half_speed(short obj)
{
    short speed = aObjectTypeData[
        aeObjectTypeList[obj]].cruiseVelocity;

    approach_speed(obj, (int)(short)(speed & 0xfffe) << 7);
    return 0;
}

/* Function start: 0x422E50 */
unsigned int approach_cruise_speed(short ship)
{
    approach_speed(ship,
        (int)aObjectTypeData[
            aeObjectTypeList[ship]].cruiseVelocity << 8);
    return 0;
}

/* Function start: 0x422E80 */
unsigned int approach_full_speed(short ship)
{
    approach_speed(ship,
                      (int)asShipMaximumSpeedArray[ship] << 8);
    return 0;
}

/* Function start: 0x422EA0 */
unsigned int approach_ship_speed(short obj, short other)
{
    approach_speed(obj, anShipSpeed[other]);
    return 0;
}

/* Function start: 0x422EC0 */
unsigned int get_front_spot(short obj, unsigned short distance,
                            FixedVector *point)
{
    ScaleFixedVector(&aShipFwdVec[obj],
                     (int)distance << 8, point);
    AddFixedVectors(&aShipPositionsArray[obj], point, point);
    return 0;
}

/* Function start: 0x422F10 */
unsigned int get_rear_spot(short obj, unsigned short distance,
                           FixedVector *point)
{
    ScaleFixedVector(&aShipFwdVec[obj],
                     -((int)distance << 8), point);
    AddFixedVectors(&aShipPositionsArray[obj], point, point);
    return 0;
}

/* Function start: 0x422F60 */
unsigned int close_behind(volatile short range)
{
    if (nTgtRange < range &&
        nTargetFacingValue < -0x32)
        return 1;
    return 0;
}

/* Function start: 0x422F80 */
short scan_for_enemy(short obj, unsigned short range)
{
    short other;
    short distance;

    nTargetShip = -1;
    nTgtRange = 0;
    for (other = 0; other < 10; other++) {
        if (aeObjectClass[other] < OBJECT_CLASS_SHIP ||
            aeSpecialManeuvers[other] ==
                SPECIAL_MANEUVER_UNKNOWN_9)
            continue;
        if (aeShipSide[obj] == aeShipSide[other])
            continue;
        distance = distance_from_point(other, &aShipPositionsArray[obj]);
        if (distance < range &&
            (nTargetShip == -1 ||
             nTgtRange > distance)) {
            nTargetShip = other;
            nTgtRange = distance;
        }
    }
    if (nTargetShip != -1) {
        get_facing_range_from_object(obj, nTargetShip);
        nTgtRange =
            distance_from_object(obj, nTargetShip);
    }
    return nTargetShip;
}

/* Function start: 0x423070 */
int any_enemy(short obj, short range)
{
    short other;

    nTargetShip = -1;
    for (other = 0; other < 10; other++) {
        if (aeObjectClass[other] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuvers[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            aeShipSide[obj] != aeShipSide[other]) {
            nTgtRange = distance_from_object(obj, other);
            if (nTgtRange < range) {
                nTargetShip = other;
                return 1;
            }
        }
    }
    return 0;
}

/* Function start: 0x4230F0 */
short nearest_enemy_range(short obj)
{
    short other;
    short range;

    nTargetShip = -1;
    range = 0x7fff;
    other = 0;
    do {
        if (aeObjectClass[other] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuvers[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            aeShipSide[obj] != aeShipSide[other])
            range = MinShort(range, distance_from_object(obj, other));
        other++;
    } while (other < 10);
    return range;
}

/* Function start: 0x423160 */
/* Nothing in the shipped executable calls this function. */
int FindEnemyAimingWithinRange(short obj, short range)
{
    short other;

    for (other = 0; other < 10; other++) {
        if (aeObjectClass[other] < OBJECT_CLASS_SHIP ||
            aeSpecialManeuvers[other] ==
                SPECIAL_MANEUVER_UNKNOWN_9 ||
            aeShipSide[obj] == aeShipSide[other])
            continue;
        get_facing_range_from_object(obj, other);
        if (range > nTgtRange && nTargetFacingValue > 80) {
            nTargetShip = other;
            return 1;
        }
    }
    return 0;
}

/* Function start: 0x4231E0 */
/* Nothing in the shipped executable calls this function. */
unsigned int FireOnAlternateTurns(short obj)
{
    if ((abShipTurnList[obj] & 1) == 0)
        fire(obj, acShipTargetTable[obj]);
    return 0;
}

/* Function start: 0x423210 */
void fire_when_ready(short obj, short aimed)
{
    (void)aimed;
    if (nYourWingman == obj &&
        aiPilotLevel[obj] != 11) {
        ship_vs_ship(obj, 0);
        if (nFacingToTargetValue > 80)
            return;
    }
    fire(obj, acShipTargetTable[obj]);
}

/* Function start: 0x423260 */
unsigned int ships_within_range(short obj, short other, short range)
{
    FixedVector delta;

    ComputeVectorDelta(&aShipPositionsArray[obj],
                       &aShipPositionsArray[other], &delta);
    return IsVectorWithinRange(&delta, range);
}

/* Function start: 0x4232B0 */
int attacker_in_range(short obj, unsigned short range)
{
    short other;

    for (other = 0; other < 10; other++) {
        if (aeObjectClass[other] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuvers[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            aeShipSide[obj] != aeShipSide[other] &&
            acShipTargetTable[other] == obj) {
            nTgtRange = distance_from_object(other, obj);
            if (range > nTgtRange) {
                nTargetShip = other;
                return 1;
            }
        }
    }
    nTargetShip = -1;
    return 0;
}

/* Function start: 0x423350 */
int in_danger(short obj)
{
    short target;
    short other;
    unsigned short range;

    target = -1;
    for (other = 0; other < 10; other++) {
        if (aeObjectClass[other] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuvers[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            aeShipSide[obj] != aeShipSide[other]) {
            nTargetShip = target;
            if (acShipTargetTable[other] != obj)
                continue;
            range = (unsigned short)distance_from_object(other, obj);
            target = nTargetShip;
            if (target == -1 || range < nTgtRange) {
                target = other;
                nTgtRange = (short)range;
            }
        }
    }
    nTargetShip = target;
    return target != -1;
}

/* Function start: 0x423400 */
unsigned int target_within_range(short obj)
{
    short target = acShipTargetTable[obj];

    if (unactive(target) != 0)
        return 0;
    return ships_within_range(obj, target, 7000);
}

/* Function start: 0x423440 */
short build_target_list(short obj, short range)
{
    short count;
    short other;
    short distance;

    count = 0;
    for (other = 0; other < 10; other++) {
        if (aeObjectClass[other] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuvers[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            aeShipSide[obj] != aeShipSide[other]) {
            distance = distance_from_object(obj, other);
            if (distance < range) {
                acFormationMemberLists[count] =
                    (signed char)other;
                asTargetListRangesArray[count] = distance;
                count++;
            }
        }
    }
    acFormationMemberLists[count] = -1;
    return count;
}

/* Function start: 0x4234C0 */
int select_safe_target(short obj)
{
    short target;
    short index;

    build_target_list(obj, 7000);
    index = -1;
    do {
        index++;
        target = (short)acFormationMemberLists[index];
        if (target == -1)
            break;
    } while (in_danger(target) != 0);
    if (target == -1 && index > 0) {
        index--;
        target = (short)acFormationMemberLists[
            RandomBelowOrEqual(index)];
    }
    nTargetShip = target;
    return target != -1;
}

/* Function start: 0x423530 */
void inherit_leader_mission(short obj)
{
    short leader = asShipWingLeaderArray[obj];

    if (leader != -1 &&
        aeObjectClass[obj] >= OBJECT_CLASS_SHIP) {
        aeShipMissionType[obj] =
            aeShipMissionType[leader];
        anShipMissionShips[obj] =
            anShipMissionShips[leader];
        aShipMissionSpotArray[obj] =
            aShipMissionSpotArray[leader];
        asShipWingLeaderArray[obj] = -1;
    }
}

/* Function start: 0x4235B0 */
void inherit_leader(short obj)
{
    short leader = asShipWingLeaderArray[obj];
    short other;

    if (leader == -1 || aeObjectClass[obj] < OBJECT_CLASS_SHIP)
        return;
    inherit_leader_mission(obj);
    asShipWingLeaderArray[obj] = asShipWingLeaderArray[leader];
    for (other = 0; other < 10; other++) {
        if (asShipWingLeaderArray[other] == leader)
            asShipWingLeaderArray[other] = obj;
    }
}

/* Function start: 0x423610 */
unsigned int dead_ship(short i)
{
    if (i != -1 && aMissionShips[i].state != 3)
        return 0;
    return 1;
}

/* Function start: 0x423640 */
int gone_ship(short missionShip)
{
    if (missionShip != -1 &&
        aMissionShips[missionShip].state != 3 &&
        aMissionShips[missionShip].state != 2)
        return 0;
    return 1;
}

/* Function start: 0x423670 */
short skill_rating(short obj)
{
    int rating = aiPilotLevel[obj];

    if (rating <= 4)
        return MaxShort(2, (short)rating);
    if (rating == 13)
        return 5;
    if (rating < 14)
        return (short)((rating - 5 >> 1) + 4);
    return (short)rating - 10;
}

/* Function start: 0x4236B0 */
int skill_check(short obj, short difficulty)
{
    return RandomBelowOrEqual(MinShort(8, difficulty)) <
           skill_rating(obj);
}

/* Function start: 0x4236F0 */
short find_ships_sphere(short missionShip)
{
    short shipIndex;
    MissionNavPoint *navPoint;
    short fallback;
    short navIndex;

    navIndex = 0;
    navPoint = aMissionNavPoints;
    fallback = -1;
    for (; navIndex < ACTIVE_MISSION_NAV_POINT_COUNT;
         navIndex++, navPoint++) {
        for (shipIndex = 0; shipIndex < 10; shipIndex++) {
            if (navPoint->missionShips[shipIndex] == missionShip) {
                if (navPoint->type == 1)
                    return navIndex;
                if (fallback == -1)
                    fallback = navIndex;
            }
        }
    }
    if (fallback != -1) {
        navIndex = fallback;
        if (aMissionNavPoints[navIndex].type > 1)
            do {
                navIndex--;
            } while (aMissionNavPoints[navIndex].type > 1);
    } else {
        navIndex = -1;
    }
    return navIndex;
}

/* Function start: 0x423780 */
int locate_ship(short missionShip, FixedVector *point)
{
    short navPoint;
    short obj;

    obj = find_ship_index(missionShip);
    if (obj != -1) {
        *point = aShipPositionsArray[obj];
        return 1;
    }
    if (dead_ship(missionShip) != 0)
        return 0;
    navPoint = find_ships_sphere(missionShip);
    if (navPoint != -1) {
        AddFixedVectors(&aMissionNavPoints[navPoint].position,
                        &aMissionShips[missionShip].position,
                        point);
        return 1;
    }
    zero_vector(point);
    return 0;
}

/* Function start: 0x423820 */
unsigned int get_follow_point(short obj, FixedVector *point)
{
    short pathIndex;
    short objective;
    short missionShip;
    int type;

    if (aeShipSide[obj] == SIDE_KILRATHI) {
        *point = aMissionNavPoints[
            anShipMissionShips[obj]].position;
        return 0;
    }
    pathIndex = (short)abShipNavPtIdx[obj];
    while (++pathIndex < 16) {
        objective = (short)abFlightPathList[pathIndex];
        type = aMissionObjectivesArray[objective].type;
        switch (type) {
        case 0:
            *point = aMissionObjectivesArray[objective].position;
            abShipNavPtIdx[obj] = (signed char)pathIndex;
            return 0;
        case 1:
            missionShip = (short)aMissionObjectivesArray[objective].index;
            objective = find_ship_index(missionShip);
            if (objective == -1)
                *point = aMissionNavPoints[missionShip].position;
            else
                *point = aShipPositionsArray[objective];
            abShipNavPtIdx[obj] = (signed char)pathIndex;
            return 0;
        }
    }
    return 0;
}

/* Function start: 0x423930 */
unsigned int get_first_follow_point(short obj, FixedVector *point)
{
    if (aeShipSide[obj] == SIDE_IMPERIAL)
        abShipNavPtIdx[obj] =
            cCurrentNavPtIdx - 1;
    get_follow_point(obj, point);
    return 0;
}

/* Function start: 0x423970 */
int hostile_sphere(short obj, short navPoint)
{
    short missionShip;
    short index;

    for (index = 0; index < 10; index++) {
        missionShip =
            aMissionNavPoints[navPoint].missionShips[index];
        if (missionShip != -1 &&
            aeShipSide[obj] !=
                aMissionShips[missionShip].side)
            return 1;
    }
    return 0;
}

/* Function start: 0x4239D0 */
int abandoned(short obj, short other)
{
    short navPoint;

    if ((abShipTurnList[obj] & 7) == 0 &&
        aeShipSide[obj] != SIDE_KILRATHI &&
        RandomBelowOrEqual(8) == 0) {
        navPoint = FindNearestNavPoint(obj);
        if (nCurrentNavigationPoint != navPoint &&
            hostile_sphere(obj, navPoint) != 0 &&
            distance_from_object(obj, other) > 10000) {
            explode(-1, obj);
            return 1;
        }
        return 0;
    }
    return 0;
}

/* Function start: 0x423A50 */
void engage(short obj, short target, enum ShipObjective objective)
{
    if (aeShipObjectiveList[obj] != objective) {
        reset_objective(obj, objective);
        if (acShipRating[obj] > 8 &&
            ace_status(
                (short)((short)aiPilotLevel[obj] - 14), 8) == 0)
            ace_greeting(obj);
    }
    acShipTargetTable[obj] = target;
}

/* Function start: 0x423AC0 */
int target_valid(short obj)
{
    return unactive(acShipTargetTable[obj]) == 0 &&
           aeShipSide[
               acShipTargetTable[obj]] !=
               aeShipSide[obj];
}

/* Function start: 0x423B00 */
unsigned int triumph(short obj)
{
    unsigned int result = 0;
    short objective;

    switch (aeShipMissionType[obj]) {
    case MISSION_TYPE_PATROL:
        objective = 0;
        while (abFlightPathList[objective] != -1 &&
               visited(
                   (short)abFlightPathList[objective]) != 0)
            objective++;
        return 1;
    case MISSION_TYPE_ESCORT:
    case MISSION_TYPE_DEFEND:
    case MISSION_TYPE_WINGMAN:
        return dead_ship(anShipMissionShips[obj]) == 0;
    case MISSION_TYPE_STRIKE:
        result = dead_ship(anShipMissionShips[obj]);
    }
    return result;
}

/* Function start: 0x423BA0 */
short find_ratio(short inputMinimum, short inputMaximum,
                         short input, short outputMinimum,
                         short outputMaximum)
{
    if (input < inputMinimum)
        return outputMinimum;
    if (input > inputMaximum)
        return outputMaximum;
    return (short)(((int)(outputMaximum - outputMinimum) *
                   (int)(input - inputMinimum)) /
                   (int)(inputMaximum - inputMinimum)) + outputMinimum;
}

/* Function start: 0x423C00 */
short evaluate_damage(short obj)
{
    ObjectTypeData *typeData =
        &aObjectTypeData[aeObjectTypeList[obj]];

    if (aeObjectClass[obj] < OBJECT_CLASS_SHIP)
        return 100;
    return (short)((acShipDmg[obj] * -26) /
                       typeData->damageCapacity +
                   (aasShipArmor[obj][1] * 27) /
                       typeData->armorRear +
                   (aasShipArmor[obj][0] * 23) /
                       typeData->armorFront +
                   (aasShipArmor[obj][2] * 12) /
                       typeData->armorLeft +
                   (aasShipArmor[obj][3] * 12) /
                       typeData->armorRight + 26);
}

/* Function start: 0x423CD0 */
short mine_available(short obj)
{
    return find_weapon(obj, OBJECT_TYPE_SPACE_MINE);
}

/* Function start: 0x423CE0 */
int LoadShapeSet(PacketResourceDescriptor *resources,
                 unsigned short flags, short defaultLogicalFile)
{
    short logicalFile;

    while (resources->resource != 0) {
        if (*resources->resource == 0) {
            logicalFile = resources->logicalFile;
            if (logicalFile == -1)
                *resources->resource = FetchDiskPacketRetrying(
                    defaultLogicalFile, resources->section, flags);
            else
                *resources->resource = FetchDiskPacketRetrying(
                    logicalFile, resources->section, flags);
            if (*resources->resource == 0)
                return 0;
        }
        resources++;
    }
    return 1;
}

/* Function start: 0x423D50 */
int FreeShapeSet(PacketResourceDescriptor *resources,
                 unsigned short releaseFlags)
{
    while (resources->resource != 0) {
        if (*resources->resource != 0)
            FreePacketAndClear(resources->resource, releaseFlags);
        resources++;
    }
    return 0;
}

/* Function start: 0x423D80 */
int LoadPacketResourceList(PacketResourceDescriptor *resources,
                           short flags, int availableBytes)
{
    int packetSize;

    while (resources->resource != 0) {
        if (*resources->resource == 0) {
            PromptInsertNumberedDisk(resources->logicalFile);
            packetSize = (int)GetPacketSize(
                pDiskFileRecords[
                    resources->logicalFile].name,
                resources->section);
            if (packetSize < availableBytes) {
                *resources->resource =
                    FetchDiskPacketRetrying(
                        resources->logicalFile, resources->section,
                        (unsigned short)flags);
                if (*resources->resource == 0)
                    return availableBytes;
                availableBytes -= packetSize;
            }
        }
        resources++;
    }
    return availableBytes;
}

/* Function start: 0x423E10 */
unsigned int ResetCockpitPaletteEntries(void)
{
    short black[3];
    short index;

    black[0] = 0;
    black[1] = 0;
    black[2] = 0;
    index = 0;
    do {
        memcpy(aPaletteFadeEntries[index], black,
               sizeof(black));
        SetPaletteEntry((short)(index + 185),
                        aPaletteFadeEntries[index]);
        index++;
    } while (index < 6);
    memcpy(asDamageFlashColour, black, sizeof(black));
    asDamageFlashColour[2] = 32;
    SetPaletteEntry((short)cPrimaryViewBufferColour,
                    asDamageFlashColour);
    return 0;
}

/* Function start: 0x423E90 */
unsigned int initialize_cockpit(int requestedMode)
{
    Viewport savedScreen;
    unsigned char *backdrop;
    unsigned int result;
    signed char mode;

    if (message_showing())
        EndCommMenu();

    mode = (signed char)requestedMode;

    if (bCockpitlessView == 0 && mode == cScreenViewMode) {
        if (stSpaceBufferViewport.pixels != 0)
            ClearViewport(&stSpaceBufferViewport, cPrimaryViewBufferColour);
        else
            initialize_view_buffer();
        return 0;
    }

    GetScreenUpdateFlag();
    savedScreen = stScreenView;
    stScreenView.top = 0;
    stScreenView.left = 0;
    stScreenView.right = 319;
    stScreenView.bottom = 199;
    ClearViewport(&stModalSrcViewport, cBlackColour);

    cCockpitLogicalFile =
        (unsigned char)(cCurrentCockpitView + 17);
    cScreenViewMode = mode;
    switch (mode) {
    case 0:
        if (apCockpitShapesList[0] != 0) {
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreenView, 0, 0,
                                  apCockpitShapesList[0], 0);
        } else {
            backdrop = FetchDiskPacketRetrying(
                (short)cCockpitLogicalFile, 0, 0);
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreenView, 0, 0, backdrop, 0);
            ReleasePacketHandle(backdrop);
        }
        ResetCockpitPaletteEntries();
        if (bCockpitlessView == 0)
            explosion_draw();
        reset_cockpit();
        InvalidateVduMode(0);
        InvalidateVduMode(1);
        update_VDUs();
        clear_head_up_display();
        if (bCockpitlessView == 0)
            ResetPilotHandAnimation();
        set_up_screen_viewport(0);
        break;
    case 1:
        if (apCockpitShapesList[1] != 0) {
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreenView, 0, 0,
                                  apCockpitShapesList[1], 0);
        } else {
            backdrop = FetchDiskPacketRetrying(
                (short)cCockpitLogicalFile, 1, 0);
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreenView, 0, 0, backdrop, 0);
            ReleasePacketHandle(backdrop);
        }
        set_up_screen_viewport(1);
        break;
    case 2:
        if (apCockpitShapesList[2] != 0) {
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreenView, 0, 0,
                                  apCockpitShapesList[2], 0);
        } else {
            backdrop = FetchDiskPacketRetrying(
                (short)cCockpitLogicalFile, 2, 0);
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreenView, 0, 0, backdrop, 0);
            ReleasePacketHandle(backdrop);
        }
        set_up_screen_viewport(2);
        break;
    case 3:
        if (apCockpitShapesList[3] != 0) {
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreenView, 0, 0,
                                  apCockpitShapesList[3], 0);
        } else {
            backdrop = FetchDiskPacketRetrying(
                (short)cCockpitLogicalFile, 3, 0);
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreenView, 0, 0, backdrop, 0);
            ReleasePacketHandle(backdrop);
        }
        set_up_screen_viewport(3);
        break;
    case 4:
        if (bIntroSceneResourcesActive == 1) {
            if (pCinematicViewBackdropPtr != 0) {
                DrawSpriteDefault(&stScreenView, 0, 0,
                                  pCinematicViewBackdropPtr, 0);
                if (bCockpitlessView < 1) {
                    DIBslam();
                    DIBslamReal();
                }
            } else {
                unsigned char *cinematicBackdrop;

                cinematicBackdrop = FetchDiskPacketRetrying(8, 6, 0);
                DrawSpriteDefault(&stScreenView, 0, 0, cinematicBackdrop, 0);
                if (bCockpitlessView < 1) {
                    DIBslam();
                    DIBslamReal();
                }
                ReleasePacketHandle(cinematicBackdrop);
            }
        }
        set_up_screen_viewport(4);
        break;
    case 5:
        set_up_screen_viewport(4);
        break;
    case 6:
        set_up_screen_viewport(5);
        break;
    case 7:
        if (pLogicRearViewBackdropPtr != 0) {
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreenView, 0, 0,
                                  pLogicRearViewBackdropPtr, 0);
        } else {
            unsigned char *rearBackdrop;

            rearBackdrop = FetchDiskPacketRetrying(8, 7, 0);
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreenView, 0, 0, rearBackdrop, 0);
            ReleasePacketHandle(rearBackdrop);
        }
        set_up_screen_viewport(0);
        break;
    }

    bViewBufferEnabled = 1;
    SetViewportRect(&stSpaceBufferViewport, 0, 0,
                    (unsigned short)(nScreenWidth - 1),
                    (unsigned short)(nScreenHeight - 1));
    result = initialize_view_buffer();
    stScreenView = savedScreen;
    return result;
}

/* Function start: 0x4242D0 */
unsigned int InitializeConstellationObject(
    const ConstellationObjectDefinition *definition, short object)
{
    FixedVector position;

    aeObjectClass[object] = OBJECT_CLASS_PLANET;
    init_ijk(63);
    alter_yaw((short)-definition->yaw, 63);
    alter_pitch((short)-definition->pitch, 63);
    alter_roll(definition->roll, 63);
    ScaleFixedVector(&aShipFwdVec[63],
                     0x753000, &position);
    aShipPositionsArray[object] = position;
    asObjScreenScalesArray[object] = 0xff;
    asObjScreenAngles[object] = 0;
    asObjectViewFrames[object] = 0;
    aeObjectTypeList[object] = OBJECT_TYPE_HORNET;
    apObjShapeArray[object] =
        FetchDiskPacketRetrying(
            12, (short)(definition->shapePacket + 1), 0);
    return 0;
}

/* Function start: 0x4243B0 */
unsigned int FreeConstellationObject(short object)
{
    FreePacketAndClear(&apObjShapeArray[object], 0);
    remove_object(object);
    return 0;
}

/* Function start: 0x4243E0 */
unsigned int init_constellation(short scene)
{
    short slot;
    short object;

    if (pCurrentConstellationShape != 0)
        return 0;

    scene--;
    pCurrentConstellationShape =
        FetchDiskPacketRetrying(12, 0, 0);
    if (nTrainSimActive != 0 || scene < 0)
        return;

    slot = 0;
    do {
        if (pConstellationDefinitionsPtr[scene * 4 + slot].shapePacket !=
            -1) {
            object = find_vacant_3d_object();
            if (object != -1) {
                InitializeConstellationObject(
                    &pConstellationDefinitionsPtr[scene * 4 + slot],
                    object);
            }
            asConstellationObjectIndices[slot] = object;
        } else {
            asConstellationObjectIndices[slot] = -1;
        }
        slot++;
    } while (slot < 4);
    /* The successful path returns the last expression left in EAX. */
}

/* Function start: 0x424490 */
unsigned int free_constellation(void)
{
    short object;
    short slot;

    slot = 0;
    FreePacketAndClear(&pCurrentConstellationShape, 0);
    do {
        object = asConstellationObjectIndices[slot];
        if (object != -1) {
            FreeConstellationObject(object);
            asConstellationObjectIndices[slot] = -1;
        }
        slot++;
    } while (slot < 4);
    return 0;
}

/* Function start: 0x4244E0 */
void init_vdus(void)
{
    stRightVduViewport = stScreenView;
    stLeftVdu = stScreenView;

    InitializeTextContextFromFont(
        &stLeftVduTextContext, 2,
        cPrimaryTextColour,
        (signed char)cBlackColour);
    stLeftVduTextContext.viewport = &stLeftVdu;
    stLeftVduTextContext.text = szDefaultTxtBuf;
    set_mode(0, 1);
    anVduModeCache[0] = 0;

    InitializeTextContextFromFont(
        &stRightVduTxtContext, 2,
        cPrimaryTextColour,
        (signed char)cBlackColour);
    stRightVduTxtContext.viewport = &stRightVduViewport;
    stRightVduTxtContext.text = szDefaultTxtBuf;
    set_mode(1, nTrainSimActive == 0 ? 5 : 3);
    anVduModeCache[1] = 0;
}

/* Function start: 0x4245B0 */
unsigned int InitializeCockpitResources(signed char mode)
{
    int backgroundSize;
    unsigned int result;
    short maximumSize;
    short frame;
    short weapon;

    if (bCockpitResourcesActive == 1) {
        if (mode == cCurrentCockpitView)
            return 0;
        free_cockpit();
    }

    bCockpitResourcesActive = 1;
    cCurrentCockpitView = mode;
    cCockpitLogicalFile = (unsigned char)(mode + 17);
    clear_cockpit_damage();
    ClearHudGunReadouts();
    reset_cockpit();
    GetScreenUpdateFlag();

    LoadShapeSet(aCockpitPrimaryResources, 4,
                 (short)cCockpitLogicalFile);
    pCurrentScreenViewportPkt =
        LoadPacketAllocated(
            (short)cCockpitLogicalFile, 6);
    stCockpitBar = stScreenView;
    init_vdus();

    InitializeTextContextFromFont(
        &stCurrentCockpitReadoutTextCtx, 2,
        cPrimaryTextColour,
        (signed char)cBlackColour);
    stCurrentCockpitReadoutTextCtx.viewport = &stScreenView;
    SetTextContext(&stCurrentCockpitReadoutTextCtx);

    SetTextCursor(
        (unsigned short)stCockpitLayout.readoutOrigins[0][(int)cCurrentCockpitView].x,
        (unsigned short)stCockpitLayout.readoutOrigins[0][(int)cCurrentCockpitView].y);
    InitializeCockpitReadout(4, &stCurrentCockpitReadoutTextCtx);
    SetTextCursor(
        (unsigned short)stCockpitLayout.readoutOrigins[1][(int)cCurrentCockpitView].x,
        (unsigned short)stCockpitLayout.readoutOrigins[1][(int)cCurrentCockpitView].y);
    InitializeCockpitReadout(5, &stCurrentCockpitReadoutTextCtx);
    SetTextCursor(
        (unsigned short)stCockpitLayout.readoutOrigins[2][(int)cCurrentCockpitView].x,
        (unsigned short)stCockpitLayout.readoutOrigins[2][(int)cCurrentCockpitView].y);
    InitializeCockpitReadout(2, &stCurrentCockpitReadoutTextCtx);
    SetTextCursor(
        (unsigned short)stCockpitLayout.readoutOrigins[3][(int)cCurrentCockpitView].x,
        (unsigned short)stCockpitLayout.readoutOrigins[3][(int)cCurrentCockpitView].y);
    InitializeCockpitReadout(3, &stCurrentCockpitReadoutTextCtx);

    *(ShortRect *)&stLeftVdu.left =
        stCockpitLayout.leftVduBounds[(int)cCurrentCockpitView];
    *(ShortRect *)&stRightVduViewport.left =
        stCockpitLayout.rightVduBounds[(int)cCurrentCockpitView];

    if (pCurrentPilotHandShapePtr != 0) {
        stPilotHandView = stScreenView;
        *(ShortRect *)&stPilotHandView.left =
            stCockpitLayout.pilotHandBounds[(int)cCurrentCockpitView];
        SetViewportRect(&stPilotHandComposite, 0, 0,
            (unsigned short)(
                stCockpitLayout.pilotHandBounds[(int)cCurrentCockpitView].right -
                stCockpitLayout.pilotHandBounds[(int)cCurrentCockpitView].left),
            (unsigned short)(
                stCockpitLayout.pilotHandBounds[(int)cCurrentCockpitView].bottom -
                stCockpitLayout.pilotHandBounds[(int)cCurrentCockpitView].top));
        stPilotHandBackdropView = stPilotHandComposite;
        AllocateViewport(&stPilotHandComposite, cBlackColour, 0);
        AllocateViewport(&stPilotHandBackdropView, cBlackColour, 0);
    }

    LoadShapeSet(aCockpitSecondaryResources, 0,
                 (short)cCockpitLogicalFile);
    pRadarMarkerBackground =
        AllocateTaggedMemory(MeasureShapeFrameStorage(pTgtLockShapePtr, 2), 0);

    pReleaseWeaponDisplayBackground = 0;
    maximumSize = 0;
    for (weapon = 0;
         weapon < (signed char)aShipWeaponsTable[0][0];
         weapon++) {
        if (aObjectTypeData[((ShipWeaponSlot *)&aShipWeaponsTable[0][1])[weapon].type].objectClass ==
                OBJECT_CLASS_MISSILE) {
            backgroundSize = MeasureShapeFrameStorage(
                pCockpitWeaponShapePtr,
                (short)(((ShipWeaponSlot *)&aShipWeaponsTable[0][1])[weapon].type * 2 - 0x2f));
            maximumSize = MaxShort(maximumSize, (short)backgroundSize);
        }
    }
    if (maximumSize != 0)
        pReleaseWeaponDisplayBackground =
            AllocateTaggedMemory(maximumSize, 0);

    pCockpitExplosionBackground = 0;
    if (pCockpitExplosionShape != 0) {
        maximumSize = 0;
        for (frame = 0; frame < 8; frame++) {
            backgroundSize = MeasureShapeFrameStorage(
                pCockpitExplosionShape, frame);
            maximumSize = MaxShort(maximumSize, (short)backgroundSize);
        }
        if (maximumSize != 0)
            pCockpitExplosionBackground =
                AllocateTaggedMemory(maximumSize, 0);
    }

    pDamageDisplayBackground = 0;
    maximumSize = 0;
    for (frame = 0; frame < 9; frame++) {
        /* The original passes an unused third zero to MaxShort. */
        maximumSize = MaxShort(maximumSize,
            (short)MeasureShapeFrameStorage(pCockpitWeaponShapePtr, frame), 0);
    }
    if (maximumSize != 0)
        pDamageDisplayBackground =
            AllocateTaggedMemory(maximumSize, 0);

    ResetScannerContacts();
    result = init_personalities();
    nCockpitExplosionFrame = 8;
    bRadioSilence = 0;
    if (nMemoryConfig == 2)
        bCommVideoEnabled = 1;
    else if (nSceneResourceBudgetValue > 0x59d8)
        bCommVideoEnabled = 1;
    else
        bCommVideoEnabled = 0;
    return result;
}

/* Function start: 0x4249A0 */
unsigned int free_cockpit(void)
{
    if (bCockpitResourcesActive == 0)
        return 0;
    bCockpitResourcesActive = 0;
    GetScreenUpdateFlag();
    if (pCurrentScreenViewportPkt != 0) {
        ReleasePacketHandle(pCurrentScreenViewportPkt);
        pCurrentScreenViewportPkt = 0;
    }
    ReleaseTextFont(2);
    if (pCurrentPilotHandShapePtr != 0) {
        free_viewport(&stPilotHandComposite);
        free_viewport(&stPilotHandBackdropView);
    }
    cCockpitLogicalFile =
        (signed char)(cCurrentCockpitView + 17);
    FreeShapeSet(aCockpitPrimaryResources, 4);
    FreeShapeSet(aCockpitSecondaryResources, 0);
    FreePacketAndClear(&pCockpitPilotShape, 0);
    FreePacketAndClear(&pReleaseWeaponDisplayBackground, 0);
    FreePacketAndClear(&pCockpitExplosionBackground, 0);
    FreePacketAndClear(&pDamageDisplayBackground, 0);
    FreeCommDisplayResources();
    ReleasePacketHandle(pRadarMarkerBackground);
}

/* Function start: 0x424A80 */
unsigned int init_3Space_objects(short scene)
{
    short slot;

    if (b3SpaceObjectsActive == 1)
        return 0;
    b3SpaceObjectsActive = 1;
    cScreenViewMode = -1;
    remove_all_3d_objects();
    nExternalViewShip = -1;
    nRenderedSpaceFrame = 0;
    bScriptedView = 0;
    nCurrentSpaceFrameValue = 0;
    bMissileCameraEnabled = 0;
    nClosestVisibleObject = -1;
    nPlayerCollisionObject = -1;
    slot = 0;
    do {
        aObjectResourceSlotsArray[slot].type = -1;
        slot++;
    } while (slot <= 3);
    init_constellation(scene);
    return load_common_3Space_objects();
}

/* Function start: 0x424B00 */
unsigned int load_common_3Space_objects(void)
{
    LoadShapeSet(aCommon3SpaceResources, 0, -1);
    aObjectTypeData[OBJECT_TYPE_DEBRIS_O_RING].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_GLASS].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_SHIP_TUBING].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_TURRET].shapeSet =
        aObjectTypeData[OBJECT_TYPE_LASER_CANNON].shapeSet;
    aObjectTypeData[OBJECT_TYPE_TURRET].animation =
        aObjectTypeData[OBJECT_TYPE_LASER_CANNON].animation;

    load_ship(OBJECT_TYPE_HEAT_SEEKING_MISSILE, 3);
    aObjectTypeData[OBJECT_TYPE_DUMB_FIRE_MISSILE].shapeSet =
        aObjectTypeData[OBJECT_TYPE_HEAT_SEEKING_MISSILE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE].shapeSet =
        aObjectTypeData[OBJECT_TYPE_HEAT_SEEKING_MISSILE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_FF_MISSILE].shapeSet =
        aObjectTypeData[OBJECT_TYPE_HEAT_SEEKING_MISSILE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_DUMB_FIRE_MISSILE].animation =
        aObjectTypeData[OBJECT_TYPE_HEAT_SEEKING_MISSILE].animation;
    aObjectTypeData[OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE].animation =
        aObjectTypeData[OBJECT_TYPE_HEAT_SEEKING_MISSILE].animation;
    aObjectTypeData[OBJECT_TYPE_FF_MISSILE].animation =
        aObjectTypeData[OBJECT_TYPE_HEAT_SEEKING_MISSILE].animation;
    return 0;
}

/* Function start: 0x424B80 */
unsigned int remove_all_3d_objects(void)
{
    short i = 0;

    do {
        remove_object(i);
        i = i + 1;
    } while (i < 0x40);
    return 0;
}

/* Function start: 0x424BA0 */
unsigned int free_3Space(void)
{
    if (b3SpaceObjectsActive == 0)
        return 0;
    b3SpaceObjectsActive = 0;
    GetScreenUpdateFlag();
    bViewBufferEnabled = 0;
    free_constellation();
    remove_all_hazards();
    remove_all_3d_objects();
    free_3Space_objects();
    return 0;
}

/* Function start: 0x424BE0 */
unsigned int free_3Space_objects(void)
{
    FreeShapeSet(aCommon3SpaceResources, 0);
    FreeShapeSet(aMissionResourceDescriptors, 0);
    free_ship(3);
    aObjectTypeData[OBJECT_TYPE_DUMB_FIRE_MISSILE].shapeSet = 0;
    aObjectTypeData[OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE].shapeSet = 0;
    aObjectTypeData[OBJECT_TYPE_FF_MISSILE].shapeSet = 0;
    aObjectTypeData[OBJECT_TYPE_DUMB_FIRE_MISSILE].animation = 0;
    aObjectTypeData[OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE].animation = 0;
    aObjectTypeData[OBJECT_TYPE_FF_MISSILE].animation = 0;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_O_RING].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_GLASS].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_SHIP_TUBING].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_WING].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet;
    return 0;
}

/* Function start: 0x424C60 */
unsigned int init_inflight_music(void)
{
    nCombatMusicActive = 0;
    nInFlightMusicActive = 1;
    nInitialFlightMusicPending = 1;
    return 0;
}

/* Function start: 0x424C80 */
unsigned int free_inflight_music(void)
{
    int *record;

    StopMusicUnlessSuppressed();
    nInFlightMusicActive = 0;
    if (nInFlightMusicSlotA != -1) {
        record = (int *)(abAudioPlaybackSlots + nInFlightMusicSlotA * 6);
        nInFlightMusicSlotA = -1;
        *record = 0;
    }
    if (nInFlightMusicSlotB != -1) {
        record = (int *)(abAudioPlaybackSlots + nInFlightMusicSlotB * 6);
        nInFlightMusicSlotB = -1;
        *record = 0;
    }
    return 0;
}

/* Function start: 0x424CE0 */
unsigned int PreloadMusicTrackHook(short track)
{
    (void)track;
    return 0;
}

/* Function start: 0x424CF0 */
unsigned int ReleaseMusicTrackHook(short track)
{
    (void)track;
    return 0;
}

/* Function start: 0x424D00 */
unsigned short LoadSceneAnimationResources(short scene, short variant)
{
    unsigned char *packet;
    short logicalFile;

    logicalFile = asSceneAnimationLogicalFiles[scene];
    pCurrentSceneAnimationPrimaryShape =
        FetchDiskPacketRetrying(logicalFile, 0, 0);
    pSceneAnimationDefinitionsPtr =
        FetchDiskPacketRetrying(
            logicalFile, (short)(variant + 1), 0);
    pSceneAnimationSecondaryShape =
        FetchDiskPacketRetrying(
            logicalFile, (short)(variant + 3), 0);
    packet = FetchDiskPacketRetrying(
        logicalFile, (short)(variant + 5), 0);
    pSceneAnimationPacketPtr = packet;
    pSceneAnimationSceneDataPtr =
        packet + *(unsigned int *)(packet + 0);
    pSceneAnimationTextData =
        packet + *(unsigned int *)(packet + 4);
    pSceneAnimationObjects =
        (SceneAnimationObject *)(pSceneAnimationDefinitionsPtr + 2);
    return 0;
}

/* Function start: 0x424DA0 */
void ReleaseSceneAnimationResources(void)
{
    ReleasePacketHandle(pCurrentSceneAnimationPrimaryShape);
    ReleasePacketHandle(pSceneAnimationPacketPtr);
    ReleasePacketHandle(pSceneAnimationSecondaryShape);
    ReleasePacketHandle(pSceneAnimationDefinitionsPtr);
}

/* Function start: 0x424DE0 */
signed char *__stdcall FindSceneAnimationCommand(
    signed char *script, signed char command)
{
    signed char opcode;

    while (*script != 0) {
        opcode = *script++;
        if (opcode == command) {
            script--;
            break;
        }
        switch (opcode) {
        case 'A':
        case 'L':
        case 'Q':
            script++;
        case 'B':
        case 'G':
        case 'J':
        case 'R':
        case 'W':
            script += 2;
            break;
        case 'D':
            while (*script++ != -1) {
            }
            break;
        case 'E':
        case 'P':
        case 'S':
            script++;
            break;
        case 'X':
            script += 10;
            break;
        }
    }
    return *script != 0 ? script : 0;
}

/* Function start: 0x424EA0 */
short __stdcall SceneAnimationGoalReached(short delta, short current,
                                          short goal)
{
    if (delta < 0) {
        if (current <= goal)
            return 1;
    } else if (delta > 0) {
        if (current >= goal)
            return 1;
    } else if (current == goal) {
        return 1;
    }
    return 0;
}

/* Function start: 0x424EF0 */
unsigned short __stdcall UpdateSceneAnimationObject(
    SceneAnimationObject *object, Viewport *viewport)
{
    SceneAnimationObject *source;
    signed char *commandStart;
    signed char *cursor;
    signed char *label;
    signed char property;
    signed char frame;
    unsigned short complete;
    unsigned short goalFlags;
    short delay;
    short value;
    short xOffset;
    short layer;
    short labelNumber;
    short objectIndex;
    short objectSet;
    short objectCount;
    signed char stop;

    complete = 0;
    delay = object->delay;
    stop = 0;
    if (delay != 0)
        cursor = object->repeatCursor;
    else
        cursor = object->scriptCursor;

    while (*cursor != 0 && stop == 0) {
        switch (*cursor++) {
        case 'A':
            property = *cursor++;
            value = *(short *)cursor;
            cursor += 2;
            switch (property) {
            case 'F':
                object->frame = (short)(object->frame + value);
                object->deltaFrame = value;
                break;
            case 'R':
                object->rotation = (short)(object->rotation + value);
                object->deltaRotation = value;
                if (object->rotation < 0)
                    object->rotation = (short)(
                        ((unsigned short)(0x167 - object->rotation) /
                         0x168) * 0x168 + object->rotation);
                if (object->rotation >= 0x168)
                    object->rotation = (short)(
                        object->rotation -
                        ((unsigned short)object->rotation / 0x168) *
                            0x168);
                break;
            case 'S':
                object->scale = (short)(object->scale + value);
                object->deltaScale = value;
                if (object->scale < 0x40)
                    object->scale = 0x40;
                else if (object->scale > 0x1fff)
                    object->scale = 0x1fff;
                break;
            case 'T':
                delay = (short)(delay + value);
                break;
            case 'X':
                object->x = (short)(object->x + value);
                object->deltaX = value;
                break;
            case 'Y':
                object->y = (short)(object->y + value);
                object->deltaY = value;
                break;
            }
            break;

        case 'B':
            cursor += 2;
            break;

        case 'D':
            commandStart = cursor - 1;
            object->repeatCursor = commandStart;
            xOffset = 0;
            layer = object->layer;
            frame = *cursor++;
            while (frame != -1) {
                if (layer != 2 && nFrameSkipCounter <= 0)
                    DrawSpriteScaled(
                        viewport, (short)(object->x + xOffset), object->y,
                        object->shape, frame, object->rotation,
                        object->scale, object->frame);
                if (layer == 0)
                    xOffset = (short)(xOffset + 320);
                frame = *cursor++;
            }
            stop++;
            break;

        case 'E':
            commandStart = cursor - 1;
            xOffset = 0;
            layer = object->layer;
            complete = 1;
            frame = *cursor++;
            while (frame != -1) {
                if (layer != 2 && nFrameSkipCounter <= 0)
                    DrawSpriteScaled(
                        viewport, (short)(object->x + xOffset), object->y,
                        object->shape, frame, object->rotation,
                        object->scale, object->frame);
                xOffset = (short)(xOffset + 320);
                frame = *cursor++;
            }
            cursor = commandStart;
            stop++;
            break;

        case 'J':
            stop++;
            /* Fall through to the label search. */
        case 'G':
            labelNumber = *(short *)cursor;
            label = object->scriptStart;
            do {
                label = FindSceneAnimationCommand(label, 'B');
                label++;
                value = *(short *)label;
                label += 2;
            } while (value != labelNumber);
            cursor = label;
            break;

        case 'L':
            property = *cursor++;
            value = *(short *)cursor;
            cursor += 2;
            switch (property) {
            case 'F':
                object->frame = value;
                break;
            case 'R':
                object->rotation = value;
                if (object->rotation < 0)
                    object->rotation = (short)(
                        ((unsigned short)(0x167 - object->rotation) /
                         0x168) * 0x168 + object->rotation);
                if (object->rotation >= 0x168)
                    object->rotation = (short)(
                        object->rotation -
                        ((unsigned short)object->rotation / 0x168) *
                            0x168);
                break;
            case 'S':
                object->scale = value;
                if (object->scale < 0x40)
                    object->scale = 0x40;
                else if (object->scale > 0x1fff)
                    object->scale = 0x1fff;
                break;
            case 'T':
                delay = value;
                break;
            case 'X':
                object->x = value;
                break;
            case 'Y':
                object->y = value;
                break;
            }
            break;

        case 'P':
            stop++;
            break;

        case 'Q':
            property = *cursor++;
            value = *(short *)cursor;
            cursor += 2;
            switch (property) {
            case 'F':
                object->goalFlags |= 0x10;
                object->goalFrame = value;
                break;
            case 'R':
                object->goalFlags |= 1;
                object->goalRotation = value;
                break;
            case 'S':
                object->goalFlags |= 2;
                object->goalScale = value;
                break;
            case 'X':
                object->goalFlags |= 4;
                object->goalX = value;
                break;
            case 'Y':
                object->goalFlags |= 8;
                object->goalY = value;
                break;
            }
            break;

        case 'R':
            objectCount = *(short *)pSceneAnimationDefinitionsPtr;
            objectSet = (short)*cursor++;
            objectIndex = (short)*cursor++;
            source = &pSceneAnimationObjects[
                (short)(objectCount * objectSet) + objectIndex];
            object->x = source->x;
            object->y = source->y;
            object->rotation = source->rotation;
            object->scale = source->scale;
            object->frame = source->frame;
            break;

        case 'W':
            nSceneAnimationDelayFrames = *(short *)cursor;
            cursor += 2;
            bSceneAnimationWaitCommand = 1;
            break;

        case 'X':
            object->x = *(short *)cursor;
            cursor += 2;
            object->y = *(short *)cursor;
            cursor += 2;
            object->rotation = *(short *)cursor;
            cursor += 2;
            object->scale = *(short *)cursor;
            cursor += 2;
            object->frame = *(short *)cursor;
            cursor += 2;
            break;
        }
    }

    if (object->delay != 0) {
        object->delay--;
        return 0;
    }

    object->scriptCursor = cursor;
    goalFlags = object->goalFlags;
    object->delay = delay;
    if (complete == 0 && goalFlags != 0) {
        if ((goalFlags & 0x10) != 0)
            complete |= SceneAnimationGoalReached(
                object->deltaFrame, object->frame, object->goalFrame);
        if ((goalFlags & 4) != 0)
            complete |= SceneAnimationGoalReached(
                object->deltaX, object->x, object->goalX);
        if ((goalFlags & 8) != 0)
            complete |= SceneAnimationGoalReached(
                object->deltaY, object->y, object->goalY);
        if ((goalFlags & 2) != 0)
            complete |= SceneAnimationGoalReached(
                object->deltaScale, object->scale, object->goalScale);
        if ((goalFlags & 1) != 0)
            complete |= SceneAnimationGoalReached(
                object->deltaRotation, object->rotation,
                object->goalRotation);
    }
    return complete;
}

/* Function start: 0x425500 */
void PlaySceneAnimation(char *text, short animation, short duration)
{
    SceneAnimationObject *object;
    SceneAnimationObject *objects;
    unsigned short complete;
    short objectCount;
    short remaining;

    nSceneAnimationDelayFrames = -1;
    complete = 0;
    bSceneAnimationWaitCommand = 0;
    AddPCName(text);
    ClearViewport(&stCurrentConvTextViewport,
                  cBlackColour);
    FormatTextBufferFromStart("%X%Y%F%s%P",
                              0, 160,
                              nConvTextColorValue,
                              szTxtWorkspaceBuffer);

    objectCount = *(short *)pSceneAnimationDefinitionsPtr;
    objects = pSceneAnimationObjects +
        (short)(objectCount * animation);
    object = objects;
    if (objectCount > 0) {
        remaining = objectCount;
        do {
            if (object->layer == 0)
                object->shape = pCurrentSceneAnimationPrimaryShape;
            else
                object->shape = pSceneAnimationSecondaryShape;
            object->scriptStart = (signed char *)pSceneAnimationDefinitionsPtr;
            object->scriptStart += object->scriptOffset;
            object->scriptCursor = object->scriptStart;
            object++;
        } while (--remaining != 0);
    }

    nFrameSkipCounter = 1;
    bEscapePressedFlag = 0;
    ClearInputKeyState();
    DIBslam();
    DIBslamReal();
    for (;;) {
        do {
            nFrameSkipCounter--;
            object = objects;
            if (objectCount > 0) {
                remaining = objectCount;
                do {
                    complete |= UpdateSceneAnimationObject(
                        object, &stSceneBuffer);
                    object++;
                } while (--remaining != 0);
            }
            if (nSceneAnimationDelayFrames != -1) {
                if (nSceneAnimationDelayFrames != 0)
                    nSceneAnimationDelayFrames--;
                else
                    complete++;
            }
            if (complete == 0) {
                RefreshMemoryStatusOverlay();
                DIBslam();
                DIBslamReal();
            }
            if (nFrameSkipCounter == 0) {
                nFrameSkipCounter = nFrameSkip;
                if (bSlowSceneAnimation != 0)
                    nFrameSkipCounter++;
            }
            if ((complete == 0 && CheckEscaped() != 0) ||
                bEscapePressedFlag != 0) {
                if (nSceneAnimationDelayFrames == -1) {
                    while (complete == 0 &&
                           bSceneAnimationWaitCommand == 0) {
                        object = objects;
                        if (objectCount > 0) {
                            remaining = objectCount;
                            do {
                                nFrameSkipCounter = 2;
                                complete |= UpdateSceneAnimationObject(
                                    object, &stSceneBuffer);
                                object++;
                            } while (--remaining != 0);
                        }
                    }
                }
                nSceneAnimationDelayFrames = 0;
            }
        } while (complete == 0);

        if (nSceneAnimationDelayFrames == -1) {
            SetFrameTimerPeriodDirect((short)(duration / 2));
            do {
                if ((short)IsFrameTickElapsed() != 0 || CheckEscaped() != 0)
                    break;
            } while (bEscapePressedFlag == 0);
        }
        if (complete != 0)
            return;
    }
}
