/*
 *  Text formatting and the DirectDraw error-string table.
 *
 *  Address range 0x433400-0x4348ff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: DirectDrawResultToText holds 122 string references, the largest single block.
 */
#include "wc1.h"

/* Function start: 0x433AC0 */
void steer_away_from_object(short obj, short other, short amount)
{
    FixedVector predicted;
    FixedVector direction;

    g_anRollGoal_0059d630[obj] = 0;
    AddFixedVectors(&g_aShipPosition_0059c490[other],
                    &g_aShipVelocity_0059c010[other], &predicted);
    ComputeVectorDelta(&g_aShipPosition_0059c490[obj],
                       &predicted, &direction);
    if (AbsInt(direction.x) < AbsInt(direction.y)) {
        if (direction.x > 0)
            amount = -amount;
        g_anYawGoal_0059c310[obj] = amount;
    } else {
        if (direction.y > 0)
            amount = -amount;
        g_anPitchGoal_0059d7a0[obj] = amount;
    }
}

/* Function start: 0x433D90 */
int check_turn_delay(short obj)
{
    return ShipAiRoutine16(obj, 1);
}

/* Function start: 0x433DE0 */
int regulate_turn(short obj)
{
    signed char count;

    if (g_aeSpecialManeuver_0059c3c0[obj] ==
        SPECIAL_MANEUVER_UNKNOWN_9)
        return 1;
    if (g_aeObjectClass_0059d100[obj] != OBJECT_CLASS_CAPITAL_SHIP) {
        if (check_turn_delay(obj) != 0)
            return 1;
    }
    count = --g_acTurnRegulator_0059cf10[obj];
    if (count > 0)
        return 1;
    g_abShipTurn_0059d860[obj]++;
    g_acTurnRegulator_0059cf10[obj] = g_acTurnInterval_0059d7d0[obj];
    return 0;
}

/* Function start: 0x433E50 */
void select_target(short obj)
{
    if (g_aeShipSide_0059d650[obj] == SIDE_KILRATHI &&
        RandomBelow(100) < 50 && attacker_in_range(0, 16000) == 0 &&
        distance_from_object(obj, 0) < 5000) {
        g_acShipTarget_0059ce60[obj] = 0;
        return;
    }
    g_acShipTarget_0059ce60[obj] =
        (signed char)scan_for_enemy(obj, 16000);
}

/* Function start: 0x433EC0 */
unsigned int veer_random(short obj, short amount)
{
    switch (RandomBelowOrEqual(3)) {
    case 0:
        g_anYawGoal_0059c310[obj] = amount;
        break;
    case 1:
        g_anYawGoal_0059c310[obj] = -amount;
        break;
    case 2:
        g_anPitchGoal_0059d7a0[obj] = amount;
        break;
    case 3:
        g_anPitchGoal_0059d7a0[obj] = -amount;
        break;
    }
    return 0;
}

/* Function start: 0x433FF0 */
unsigned int compute_formation_destination(short leader,
                                           const FixedVector *offset,
                                           FixedVector *destination)
{
    *destination = g_aShipPosition_0059c490[leader];
    AddFixedVectors(destination, (FixedVector *)offset, destination);
    AddFixedVectors(destination, &g_aShipVelocity_0059c010[leader],
                    destination);
    AddFixedVectors(destination, &g_aShipVelocity_0059c010[leader],
                    destination);
    AddFixedVectors(destination, &g_aShipVelocity_0059c010[leader],
                    destination);
    return 0;
}

/* Function start: 0x434360 */
unsigned int approach_formation_destination(short obj,
                                            const FixedVector *destination,
                                            short leader)
{
    FixedVector projected;
    short forwardFacing;

    AddFixedVectors(&g_aShipPosition_0059c490[obj],
                    &g_aShipForwardVector_0059bce0[leader], &projected);
    get_facing_range_from_point(obj, &projected);
    forwardFacing = g_nFacingToTarget_0059d920;
    get_facing_range_from_point(obj, destination);
    if (g_nFacingToTarget_0059d920 < 40) {
        ClearShipAiThrottle(obj);
    } else if (g_nTargetRange_0059ce10 < 2001) {
        if (g_nTargetRange_0059ce10 < 201)
            approach_ship_speed(obj, leader);
        else
            ApproachShipSpeed(obj, g_anShipSpeed_0059b320[leader] +
                                   (g_nTargetRange_0059ce10 << 4));
    } else {
        approach_full_speed(obj);
        if (g_nTargetRange_0059ce10 > 3000 &&
            g_nFacingToTarget_0059d920 > 70 && normal_speed(obj) != 0)
            fire_afterburner(obj, 5);
    }
    if (g_nTargetRange_0059ce10 < 200) {
        steady_object(obj);
        if (forwardFacing > 90)
            point_perpendicular(obj, leader);
        else
            point_parallel(obj, leader);
    } else if (no_goal(obj) != 0) {
        point_ship(obj, 0, &g_vToTarget_0059d4d0);
    }
    if (g_nTargetRange_0059ce10 < 700)
        trim_goals(obj, 10);
    return 0;
}

/* Function start: 0x4344E0 */
unsigned int maintain_formation(short obj)
{
    short leader = g_asShipWingLeader_0059d400[obj];
    FixedVector destination;

    compute_formation_destination(leader,
                                  &g_aShipFormationOffset_0059b520[obj],
                                  &destination);
    g_aShipDestination_0059d530[obj] = destination;
    approach_formation_destination(obj, &destination, leader);
    return 0;
}

/* Function start: 0x434550 */
unsigned int reset_intelligence_state(short obj)
{
    short damage;

    if (obj > 11) {
        if (g_acPilotAggression_0046d9a0[g_aiPilotLevel_0059cf30[obj]] == 0)
            damage = 100;
        else
            damage = evaluate_damage(obj);
        if (damage < 30)
            DAT_0059d620[obj] = 30;
        else if (damage < 75)
            DAT_0059d620[obj] =
                (unsigned char)InterpolateClamped(30, 74, damage, 29, 15);
        else
            DAT_0059d620[obj] =
                (unsigned char)InterpolateClamped(75, 100, damage, 14, 0);
    }
    return 0;
}

/* Function start: 0x4345D0 */
int GetShipIfArmorBelow15(short i)
{
    if ((char)DAT_0059d620[i] < 0x0f)
        return 0;
    return i;
}

/* Function start: 0x434900 */
int select_maneuver_for_event(short obj, int event)
{
    (void)event;
    SelectNewShipAiBehavior(obj);
    return event;
}

/* Function start: 0x434980 */
void classify_intelligence_event(short obj, int event)
{
    signed char aggression =
        g_acPilotAggression_0046d9a0[g_aiPilotLevel_0059cf30[obj]];
    short damage;
    short limit;

    switch (event) {
    case 3:
    case 4:
    case 7:
        DAT_0059d620[obj] += aggression;
        break;
    case 5:
        DAT_0059d620[obj] -= aggression;
        break;
    case 6:
        DAT_0059d620[obj] += aggression * 2;
        break;
    case 8:
        DAT_0059d620[obj] /= 2;
        break;
    case -1:
    case 2:
        DAT_0059d620[obj] -=
            g_acPilotRecovery_0046d9b8[g_aiPilotLevel_0059cf30[obj]];
        break;
    }
    damage = evaluate_damage(obj);
    if (damage < 40) {
        DAT_0059d620[obj] += aggression * 2;
    } else {
        if (damage < 75) {
            limit = 28;
            damage = (short)(signed char)DAT_0059d620[obj] + aggression;
        } else {
            limit = 7;
            damage = (short)(signed char)DAT_0059d620[obj];
        }
        DAT_0059d620[obj] = (unsigned char)MinShort(damage, limit);
    }
    if (event == 6 && (signed char)DAT_0059d620[obj] > 29)
        DAT_0059d620[obj] = 29;
    if ((signed char)DAT_0059d620[obj] < 0)
        DAT_0059d620[obj] = 0;
}
