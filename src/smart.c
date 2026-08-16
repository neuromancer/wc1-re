/*
 *  Ship collision avoidance, formation following, and maneuver selection.
 *
 *  Address range 0x433ac0-0x434ccf.  The boundary is corroborated by the
 *  ordered MacsBug symbols in the Super Wing Commander `smart` unit: its
 *  final routine is chase_speed, immediately followed by the random unit.
 */
#include "wc1.h"

/* Function start: 0x41E4E0 */
unsigned int steer_away_from_object(short obj, short other, short amount)
{
    FixedVector predicted;
    FixedVector difference;
    FixedVector relative;

    g_anRollGoal_0059d630[obj] = 0;
    AddFixedVectors(&g_aShipPosition_0059c490[other],
                    &g_aShipVelocity_0059c010[other], &predicted);
    ComputeVectorDelta(&g_aShipPosition_0059c490[obj],
                       &predicted, &difference);
    transform_to_objects_frame(&difference, &relative, obj);
    if (abs(relative.x) < abs(relative.y)) {
        if (relative.x > 0)
            amount = -amount;
        g_anYawGoal_0059c310[obj] = amount;
    } else {
        if (relative.y > 0)
            amount = -amount;
        g_anPitchGoal_0059d7a0[obj] = amount;
    }
    return 0;
}

/* Function start: 0x41E5C8 */
void steer_away_from_predicted_object(short obj, short other,
                                      short predictionTicks, short amount)
{
    FixedVector predicted;
    FixedVector difference;
    FixedVector relative;

    g_anRollGoal_0059d630[obj] = 0;
    ScaleFixedVector(&g_aShipVelocity_0059c010[other],
                     (int)predictionTicks << 8, &predicted);
    AddFixedVectors(&g_aShipPosition_0059c490[other],
                    &predicted, &predicted);
    ComputeVectorDelta(&g_aShipPosition_0059c490[obj],
                       &predicted, &difference);
    transform_to_objects_frame(&difference, &relative, obj);
    if (abs(relative.x) < abs(relative.y)) {
        if (relative.x < 0)
            amount = -amount;
        g_anYawGoal_0059c310[obj] = amount;
    } else {
        if (relative.y > 0)
            amount = -amount;
        g_anPitchGoal_0059d7a0[obj] = amount;
    }
}

/* Function start: 0x41E6C8 */
void prevent_collision(short obj)
{
    short other;
    short collisionTime;
    short amount;
    short facing;

    other = (short)(signed char)DAT_0059cf20[obj];
    if (other == -1) {
        clear_alert(obj);
        return;
    }
    collisionTime = crash_time(obj, other);
    if (collisionTime >= 30) {
        approach_full_speed(obj);
        try2end_collision_alert(obj);
        return;
    }
    ship_vs_point(obj, &g_aShipPosition_0059c490[other]);
    facing = g_nFacingToTarget_0059d920;
    if (facing > 75) {
        if (g_nTargetFacing_0059d52a < -70)
            approach_zero_speed(obj);
        else
            approach_full_speed(obj);
    } else if (facing < -70 && normal_speed(obj) != 0) {
        fire_afterburner(obj, 8);
    } else {
        approach_full_speed(obj);
    }
    if (no_goal(obj) != 0) {
        facing = g_nFacingToTarget_0059d920;
        if (facing < -60 &&
            g_nTargetFacing_0059d52a > 60) {
            veer_random(obj, 14);
            return;
        }
        amount = MinShort(facing, 25);
        amount = MaxShort(0, amount);
        steer_away_from_predicted_object(obj, other,
                                         (short)(collisionTime >> 1), amount);
    }
}

/* Function start: 0x41E860 */
int handle_collisions(short obj)
{
    short other;

    other = detect_collisions(obj);
    if (other != -1)
        start_collision_alert(obj, other);
    if (alert_flag(obj, 1) != 0)
        prevent_collision(obj);
    return alert_flag(obj, 1);
}

/* Function start: 0x421CF6 */
int regulate_turn(short obj)
{
    signed char count;

    if (g_aeSpecialManeuver_0059c3c0[obj] ==
        SPECIAL_MANEUVER_UNKNOWN_9)
        return 1;
    if (g_aeObjectClass_0059d100[obj] != OBJECT_CLASS_CAPITAL_SHIP &&
        handle_collisions(obj) != 0)
        return 1;
    count = --g_acTurnRegulator_0059cf10[obj];
    if (count > 0)
        return 1;
    g_abShipTurn_0059d860[obj]++;
    g_acTurnRegulator_0059cf10[obj] = g_acTurnInterval_0059d7d0[obj];
    return 0;
}

/* Function start: 0x41EA86 */
unsigned int select_target(short obj)
{
    if (g_aeShipSide_0059d650[obj] == SIDE_KILRATHI) {
        if (RandomBelow(100) < 50) {
            if (attacker_in_range(0, 16000) == 0) {
                if (distance_from_object(obj, 0) < 5000) {
                    g_acShipTarget_0059ce60[obj] = 0;
                    return 0;
                }
            }
        }
    }
    g_acShipTarget_0059ce60[obj] =
        (signed char)scan_for_enemy(obj, 16000);
    return 0;
}

/* Function start: 0x41EB1D */
unsigned int veer_random(short obj, short amount)
{
    switch (RandomBelowOrEqual(3)) {
    case 0:
        g_anYawGoal_0059c310[obj] = amount;
        return 0;
    case 1:
        g_anYawGoal_0059c310[obj] = -amount;
        return 0;
    case 2:
        g_anPitchGoal_0059d7a0[obj] = amount;
        return 0;
    case 3:
        g_anPitchGoal_0059d7a0[obj] = -amount;
        break;
    }
    return 0;
}

/* Function start: 0x41EBC1 */
unsigned int offset_location(short obj, const ShortVector *offset,
                             FixedVector *location)
{
    *location = g_aShipPosition_0059c490[obj];
    position_relative(location, g_aShipForwardVector_0059bce0[obj],
                      offset->z);
    position_relative(location, g_aShipUpVector_0059b9e0[obj],
                      offset->y);
    position_relative(location, g_aShipRightVector_0059b6e0[obj],
                      offset->x);
    return 0;
}

/* Function start: 0x41EC73 */
unsigned int compute_formation_destination(short leader,
                                           const ShortVector *offset,
                                           FixedVector *destination)
{
    offset_location(leader, offset, destination);
    AddFixedVectors(destination, &g_aShipVelocity_0059c010[leader],
                    destination);
    AddFixedVectors(destination, &g_aShipVelocity_0059c010[leader],
                    destination);
    AddFixedVectors(destination, &g_aShipVelocity_0059c010[leader],
                    destination);
    return 0;
}

/* Function start: 0x41ECEF */
unsigned int control_speed(short obj, unsigned short range,
                           int desiredSpeed)
{
    unsigned short travelTime;
    int brakingMargin;

    travelTime = range /
        MaxShort(FixedToShortSaturating(g_anShipSpeed_0059b320[obj]), 1);
    brakingMargin = GetShipAccelerationRate(obj) * travelTime;
    brakingMargin -=
        AbsInt(desiredSpeed - g_anShipSpeed_0059b320[obj]);
    if (brakingMargin <= 0) {
        approach_speed(obj, desiredSpeed);
        return 0;
    }
    if (brakingMargin < 12800) {
        celerate(obj, 0x100);
        return 0;
    }
    celerate(obj, GetShipAccelerationRate(obj));
    return 0;
}

/* Function start: 0x41EDCC */
unsigned int chase_location(short obj, const FixedVector *destination,
                            short reference)
{
    int pointAtDestination;
    int desiredSpeed;
    short forwardFacing;
    short speed;
    FixedVector forwardTravel;
    FixedVector projectedDestination;

    pointAtDestination = 0;
    desiredSpeed = g_anShipSpeed_0059b320[reference] - 0x200;
    if (desiredSpeed < 0)
        desiredSpeed = 0;
    ScaleFixedVector(&g_aShipForwardVector_0059bce0[reference],
                     g_anShipSpeed_0059b320[reference] * 15,
                     &forwardTravel);
    AddFixedVectors(destination, &forwardTravel, &projectedDestination);
    get_facing_range_from_point(obj, &projectedDestination);
    forwardFacing = g_nFacingToTarget_0059d920;
    get_facing_range_from_point(obj, destination);
    if (g_nFacingToTarget_0059d920 < 0) {
        approach_speed(obj, 0);
        speed = FixedToShortSaturating(g_anShipSpeed_0059b320[reference]);
        speed = MaxShort(speed, 1);
        if (g_nTargetRange_0059ce10 / speed > 49)
            pointAtDestination = 1;
    } else if (g_nTargetRange_0059ce10 > 175 && no_goal(obj) != 0) {
        pointAtDestination = 1;
    }
    control_speed(obj, (unsigned short)g_nTargetRange_0059ce10,
                  desiredSpeed);
    if (g_nFacingToTarget_0059d920 > 85 &&
        g_nTargetRange_0059ce10 > 175)
        g_anRollGoal_0059d630[obj] =
            match_roll_orientation(obj, reference);
    if (pointAtDestination != 0)
        point_ship(obj, 0, &g_vToTarget_0059d4d0);
    if (g_nTargetRange_0059ce10 < 175) {
        if (forwardFacing > 90) {
            copy_frame(reference, obj);
            steady_object(obj);
            approach_speed(obj, g_anShipSpeed_0059b320[reference]);
            return 0;
        }
        point_ship(obj, 0, &forwardTravel);
    }
    if (g_nTargetRange_0059ce10 < 600)
        trim_goals(obj, 10);
    return 0;
}

/* Function start: 0x41EFFE */
unsigned int goto_location(short obj, const FixedVector *destination)
{
    short range;
    short speed;

    ship_vs_point(obj, destination);
    if (g_nFacingToTarget_0059d920 < 51) {
        approach_half_speed(obj);
        if (no_goal(obj) != 0) {
            point_ship_at_point(obj, destination);
            return 0;
        }
    } else {
        range = g_nTargetRange_0059ce10;
        if (range <= 3000) {
            speed = FixedToShortSaturating(g_anShipSpeed_0059b320[obj]);
            speed = MaxShort(speed, 1);
            if (range / speed <= 50) {
                approach_cruise_speed(obj);
                return 0;
            }
        }
        approach_full_speed(obj);
    }
    return 0;
}

/* Function start: 0x41F0C5 */
unsigned int goto_formation(short obj, const FixedVector *destination,
                            short leader)
{
    short forwardFacing;
    FixedVector projected;

    AddFixedVectors(&g_aShipPosition_0059c490[obj],
                    &g_aShipForwardVector_0059bce0[leader], &projected);
    get_facing_range_from_point(obj, &projected);
    forwardFacing = g_nFacingToTarget_0059d920;
    get_facing_range_from_point(obj, destination);
    if (g_nFacingToTarget_0059d920 < 40) {
        approach_min_speed(obj);
    } else if (g_nTargetRange_0059ce10 > 2000) {
        approach_full_speed(obj);
        if (g_nTargetRange_0059ce10 > 3000 &&
            g_nFacingToTarget_0059d920 > 70 && normal_speed(obj) != 0)
            fire_afterburner(obj, 5);
    } else if (g_nTargetRange_0059ce10 > 200) {
        control_speed(obj, (unsigned short)g_nTargetRange_0059ce10,
                      g_anShipSpeed_0059b320[leader]);
    } else {
        approach_ship_speed(obj, leader);
    }
    if (g_nTargetRange_0059ce10 < 200) {
        steady_object(obj);
        if (forwardFacing > 90) {
            copy_frame(leader, obj);
            return 0;
        }
        point_parallel(obj, leader);
    } else if (no_goal(obj) != 0) {
        point_ship(obj, 0, &g_vToTarget_0059d4d0);
    }
    if (g_nTargetRange_0059ce10 < 700) {
        trim_goals(obj, 10);
        if (forwardFacing > 90)
            g_anRollGoal_0059d630[obj] =
                match_roll_orientation(obj, leader);
    }
    return 0;
}

/* Function start: 0x41F2C5 */
unsigned int maintain_formation(short obj)
{
    short leader;
    FixedVector destination;

    leader = g_asShipWingLeader_0059d400[obj];
    compute_formation_destination(leader,
                                  &g_aShipFormationOffset_0059b520[obj],
                                  &destination);
    g_aShipDestination_0059d530[obj] = destination;
    goto_formation(obj, &destination, leader);
    return 0;
}

/* Function start: 0x41F337 */
unsigned int reset_stress(short obj)
{
    short damage;

    if (obj >= 12) {
        if (g_acPilotAggression_0046d9a0[g_aiPilotLevel_0059cf30[obj]] == 0)
            damage = 100;
        else
            damage = evaluate_damage(obj);
        if (damage < 30) {
            damage = 30;
        } else if (damage < 75) {
            damage = find_ratio(30, 74, damage, 29, 15);
        } else {
            damage = find_ratio(75, 100, damage, 14, 0);
        }
        g_acShipStress_0059d620[obj] = (signed char)damage;
    }
    return 0;
}

/* Function start: 0x41F3EF */
short stress_morale(short obj)
{
    if (g_acShipStress_0059d620[obj] < 15)
        return 0;
    if (g_acShipStress_0059d620[obj] < 30)
        return 1;
    return 2;
}

/* Function start: WC2_UNMAPPED */
enum ShipManeuver any_defense(short obj)
{
    signed char *maneuvers;
    short count;

    count = 0;
    maneuvers = g_apDefenseManeuvers_0046d3d0[
        g_aiPilotLevel_0059cf30[obj]];
    while (maneuvers[count] != -1)
        count++;
    return (enum ShipManeuver)maneuvers[RandomBelowOrEqual(count)];
}

/* Function start: 0x41F4C6 */
enum ShipManeuver pick_regular_maneuver(short obj, int event)
{
    short reroll;
    short morale;

    reroll = RandomBelowOrEqual(100) < 3 ||
             g_aeShipManeuver_0059dcb0[obj] == MANEUVER_NONE;
    morale = stress_morale(obj);
    if (morale == 2)
        return MANEUVER_OUTA_HERE;
    if ((g_aiIntelligenceEvent_0046d368[obj] == event ||
         RandomBelowOrEqual(100) < 20) &&
        (event == 0 || event == 3 || event == 4 || event == 7) &&
        reroll == 0)
        return g_aeShipManeuver_0059dcb0[obj];

    switch (event) {
    case 0:
        if (g_aeObjectClass_0059d100[
                g_acShipTarget_0059ce60[obj]] ==
            OBJECT_CLASS_CAPITAL_SHIP)
            return MANEUVER_STRAFE_ENEMY;
        if (RandomBelow(100) <
            g_aiPilotLevel_0059cf30[obj] * 5 + 60)
            return MANEUVER_ZIP_PAST;
        return any_defense(obj);
    case 2:
        return MANEUVER_TRY2TAIL;
    case 3:
        if (mine_available(obj) != -1 && RandomBelow(100) < 10)
            return MANEUVER_DROP_A_MINE;
        return any_defense(obj);
    case 4:
        if (RandomBelowOrEqual(100) >=
            g_aiPilotLevel_0059cf30[obj] * 20 + 30)
            return any_defense(obj);
        return MANEUVER_STRAFE_ENEMY;
    case 5:
        return MANEUVER_TAIL_FIRE;
    case 6:
        if (g_aiPilotLevel_0059cf30[obj] >= 2)
            return MANEUVER_HARD_TURN;
        return MANEUVER_WABBLE;
    case 7:
        return any_defense(obj);
    case 8:
        return MANEUVER_LINE_UP_DROP;
    default:
        return MANEUVER_ROLL_OVER;
    }
}

/* Function start: 0x41F714 */
enum ShipManeuver pick_from_list(const ManeuverChoice *choice, short obj)
{
    enum ShipManeuver maneuver;
    short chooseAgain;

    maneuver = g_aeShipManeuver_0059dcb0[obj];
    chooseAgain = maneuver == MANEUVER_NONE;
    if (choice->primary != maneuver && choice->secondary != maneuver &&
        RandomBelow(100) < 10 &&
        choice->primary < MANEUVER_UNKNOWN_45 &&
        choice->secondary < MANEUVER_UNKNOWN_45)
        chooseAgain = 1;
    if (chooseAgain == 0) {
        if (RandomBelowOrEqual(100) < 5)
            chooseAgain = 1;
    }
    if (chooseAgain != 0) {
        if (RandomBelowOrEqual(100) >= choice->threshold)
            maneuver = (enum ShipManeuver)choice->secondary;
        else
            maneuver = (enum ShipManeuver)choice->primary;
    }
    return maneuver;
}

/* Function start: 0x41F83F */
enum ShipManeuver pick_kilrathi_maneuver(short obj, int event)
{
    const ManeuverChoice *choice;
    enum ShipManeuver maneuver;

    choice =
        &g_aKilrathiManeuverChoices_0046d808[
            g_aiPilotLevel_0059cf30[obj]][event][stress_morale(obj)];
    maneuver = pick_from_list(choice, obj);
    switch (maneuver) {
    case MANEUVER_UNKNOWN_45:
        return MANEUVER_STRAFE_ENEMY;
    case MANEUVER_UNKNOWN_46:
        return any_defense(obj);
    default:
        return maneuver;
    }
}

/* Function start: WC2_UNMAPPED */
unsigned int process_maneuver_node(short obj, int event)
{
    const ManeuverChoice *choice;
    short rating;
    short morale;
    enum ShipManeuver maneuver;

    rating = (short)g_acShipRating_0059cd80[obj];
    if (rating == -1) {
        if (g_aeShipSide_0059d650[obj] == SIDE_KILRATHI)
            maneuver = pick_kilrathi_maneuver(obj, event);
        else
            maneuver = pick_regular_maneuver(obj, event);
    } else {
        morale = stress_morale(obj);
        choice =
            &g_aRatedManeuverChoices_0046d3e8[rating][event][morale];
        maneuver = pick_from_list(choice, obj);
    }
    if (g_aeShipManeuver_0059dcb0[obj] != maneuver)
        reset_maneuver(obj, (short)maneuver);
    return 0;
}

/* Function start: 0x41FD2B */
void handle_stress(short obj, int event)
{
    short aggression;
    short damage;

    aggression =
        g_acPilotAggression_0046d9a0[g_aiPilotLevel_0059cf30[obj]];
    switch (event) {
    case 3:
    case 4:
    case 7:
        g_acShipStress_0059d620[obj] += aggression;
        break;
    case 5:
        g_acShipStress_0059d620[obj] -= aggression;
        break;
    case 6:
        g_acShipStress_0059d620[obj] += aggression * 2;
        break;
    case 8:
        g_acShipStress_0059d620[obj] /= 2;
        break;
    case -1:
    case 2:
        g_acShipStress_0059d620[obj] -=
            g_acPilotRecovery_0046d9b8[g_aiPilotLevel_0059cf30[obj]];
        break;
    }
    damage = evaluate_damage(obj);
    if (damage < 40) {
        g_acShipStress_0059d620[obj] += aggression * 2;
    } else if (damage < 75) {
        g_acShipStress_0059d620[obj] =
            (signed char)MinShort(
                (short)((short)g_acShipStress_0059d620[obj] + aggression),
                28);
    } else {
        g_acShipStress_0059d620[obj] =
            (signed char)MinShort(g_acShipStress_0059d620[obj], 7);
    }
    if (event == 6 && g_acShipStress_0059d620[obj] >= 30)
        g_acShipStress_0059d620[obj] = 29;
    if (g_acShipStress_0059d620[obj] < 0)
        g_acShipStress_0059d620[obj] = 0;
}

/* Function start: 0x41FF37 */
void intelligence_events(short obj)
{
    int event;
    short targetGone;
    short target;
    short previousStress;
    short playerDamage;

    event = -1;
    targetGone = 0;
    target = g_acShipTarget_0059ce60[obj];
    previousStress = (short)g_acShipStress_0059d620[obj];
    if (missile_on_tail(obj) != 0) {
        event = 6;
    } else if (unactive(target) != 0) {
        targetGone = 1;
    } else if (g_aeSpecialManeuver_0059c3c0[
                   g_acShipTarget_0059ce60[obj]] ==
               SPECIAL_MANEUVER_UNKNOWN_9) {
        event = 8;
    } else {
        event = 0;
        ship_vs_ship(obj, target);
        if (g_nTargetRange_0059ce10 > 8000) {
            event = 2;
        } else if (g_acShipAiCooldown_0059d680[obj] > 0) {
            event = 7;
        } else if (g_nFacingToTarget_0059d920 > 55 &&
                   g_nTargetFacing_0059d52a < -55) {
            event = 5;
        } else if (g_nFacingToTarget_0059d920 > 75 &&
                   g_nTargetFacing_0059d52a > 75) {
            event = 4;
        } else if (g_nFacingToTarget_0059d920 < -60 &&
                   g_nTargetFacing_0059d52a > 85 &&
                   g_nTargetRange_0059ce10 < 7000) {
            event = 3;
        } else if (g_anShipSpeed_0059b320[target] < 20) {
            event = 1;
        }
    }

    handle_stress(obj, event);
    if (event != -1)
        process_maneuver_node(obj, event);
    if (event == -1 && targetGone != 0) {
        if (any_enemy(obj, 16000) == 0)
            reset_objective(obj, OBJECTIVE_NONE);
        else
            select_target(obj);
        reset_stress(obj);
    }

    if (g_nYourWingman_0046c04c == obj &&
        g_aeObjectClass_0059d100[0] == OBJECT_CLASS_SHIP &&
        g_acWingmanMessageState_0059d2c0[g_nYourWingman_0046c04c] == -1) {
        if (previousStress < 15 && g_acShipStress_0059d620[obj] >= 15) {
            send_message(obj, 4);
        } else {
            playerDamage = evaluate_damage(0);
            if (RandomBelow(1000) < 4 && playerDamage < 35) {
                if (evaluate_damage(obj) > playerDamage)
                    send_message(obj, 8);
                else
                    send_message(obj, 4);
            }
        }
    }
    g_aiIntelligenceEvent_0046d368[obj] = event;
}

/* Function start: 0x440571 */
unsigned int chase_speed(short obj, short range)
{
    short targetRange = g_nTargetRange_0059ce10;

    if (range < targetRange) {
        approach_full_speed(obj);
        return 0;
    }
    if (range > targetRange) {
        approach_zero_speed(obj);
        return 0;
    }
    approach_speed(obj,
                      g_anShipSpeed_0059b320[
                          g_acShipTarget_0059ce60[obj]]);
    return 0;
}
