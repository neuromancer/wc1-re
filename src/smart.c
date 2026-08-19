/*
 *  Ship collision avoidance, formation following, and maneuver selection.
 *
 *  Address range 0x433ac0-0x434ccf.  The boundary is corroborated by the
 *  ordered MacsBug symbols in the Super Wing Commander `smart` unit: its
 *  final routine is chase_speed, immediately followed by the random unit.
 */
#include "wc1.h"

/* Function start: 0x433AC0 */
unsigned int steer_away_from_object(short obj, short other, short amount)
{
    FixedVector predicted;
    FixedVector difference;
    FixedVector relative;

    anRollGoal[obj] = 0;
    AddFixedVectors(&aShipPosition[other],
                    &aShipVelocity[other], &predicted);
    ComputeVectorDelta(&aShipPosition[obj],
                       &predicted, &difference);
    transform_to_objects_frame(&difference, &relative, obj);
    if (abs(relative.x) < abs(relative.y)) {
        if (relative.x > 0)
            amount = -amount;
        anYawGoal[obj] = amount;
    } else {
        if (relative.y > 0)
            amount = -amount;
        anPitchGoal[obj] = amount;
    }
    return 0;
}

/* Function start: 0x433B90 */
void steer_away_from_predicted_object(short obj, short other,
                                      short predictionTicks, short amount)
{
    FixedVector predicted;
    FixedVector difference;
    FixedVector relative;

    anRollGoal[obj] = 0;
    ScaleFixedVector(&aShipVelocity[other],
                     (int)predictionTicks << 8, &predicted);
    AddFixedVectors(&aShipPosition[other],
                    &predicted, &predicted);
    ComputeVectorDelta(&aShipPosition[obj],
                       &predicted, &difference);
    transform_to_objects_frame(&difference, &relative, obj);
    if (abs(relative.x) < abs(relative.y)) {
        if (relative.x < 0)
            amount = -amount;
        anYawGoal[obj] = amount;
    } else {
        if (relative.y > 0)
            amount = -amount;
        anPitchGoal[obj] = amount;
    }
}

/* Function start: 0x433C80 */
void prevent_collision(short obj)
{
    short other;
    short collisionTime;
    short amount;
    short facing;

    other = (short)(signed char)abCollisionAlertTarget[obj];
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
    ship_vs_point(obj, &aShipPosition[other]);
    facing = nFacingToTarget;
    if (facing > 75) {
        if (nTargetFacing < -70)
            approach_zero_speed(obj);
        else
            approach_full_speed(obj);
    } else if (facing < -70 && normal_speed(obj) != 0) {
        fire_afterburner(obj, 8);
    } else {
        approach_full_speed(obj);
    }
    if (no_goal(obj) != 0) {
        facing = nFacingToTarget;
        if (facing < -60 &&
            nTargetFacing > 60) {
            veer_random(obj, 14);
            return;
        }
        amount = MinShort(facing, 25);
        amount = MaxShort(0, amount);
        steer_away_from_predicted_object(obj, other,
                                         (short)(collisionTime >> 1), amount);
    }
}

/* Function start: 0x433D90 */
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

/* Function start: 0x433DE0 */
int regulate_turn(short obj)
{
    signed char count;

    if (aeSpecialManeuver[obj] ==
        SPECIAL_MANEUVER_UNKNOWN_9)
        return 1;
    if (aeObjectClass[obj] != OBJECT_CLASS_CAPITAL_SHIP &&
        handle_collisions(obj) != 0)
        return 1;
    count = --acTurnRegulator[obj];
    if (count > 0)
        return 1;
    abShipTurn[obj]++;
    acTurnRegulator[obj] = acTurnInterval[obj];
    return 0;
}

/* Function start: 0x433E50 */
unsigned int select_target(short obj)
{
    if (aeShipSide[obj] == SIDE_KILRATHI) {
        if (RandomBelow(100) < 50) {
            if (attacker_in_range(0, 16000) == 0) {
                if (distance_from_object(obj, 0) < 5000) {
                    acShipTarget[obj] = 0;
                    return 0;
                }
            }
        }
    }
    acShipTarget[obj] =
        (signed char)scan_for_enemy(obj, 16000);
    return 0;
}

/* Function start: 0x433EC0 */
unsigned int veer_random(short obj, short amount)
{
    switch (RandomBelowOrEqual(3)) {
    case 0:
        anYawGoal[obj] = amount;
        return 0;
    case 1:
        anYawGoal[obj] = -amount;
        return 0;
    case 2:
        anPitchGoal[obj] = amount;
        return 0;
    case 3:
        anPitchGoal[obj] = -amount;
        break;
    }
    return 0;
}

/* Function start: 0x433F50 */
unsigned int offset_location(short obj, const ShortVector *offset,
                             FixedVector *location)
{
    *location = aShipPosition[obj];
    position_relative(location, aShipForwardVector[obj],
                      offset->z);
    position_relative(location, aShipUpVector[obj],
                      offset->y);
    position_relative(location, aShipRightVector[obj],
                      offset->x);
    return 0;
}

/* Function start: 0x433FF0 */
unsigned int compute_formation_destination(short leader,
                                           const ShortVector *offset,
                                           FixedVector *destination)
{
    offset_location(leader, offset, destination);
    AddFixedVectors(destination, &aShipVelocity[leader],
                    destination);
    AddFixedVectors(destination, &aShipVelocity[leader],
                    destination);
    AddFixedVectors(destination, &aShipVelocity[leader],
                    destination);
    return 0;
}

/* Function start: 0x434040 */
unsigned int control_speed(short obj, unsigned short range,
                           int desiredSpeed)
{
    unsigned short travelTime;
    int brakingMargin;

    travelTime = range /
        MaxShort(FixedToShortSaturating(anShipSpeed[obj]), 1);
    brakingMargin = GetShipAccelerationRate(obj) * travelTime;
    brakingMargin -=
        AbsInt(desiredSpeed - anShipSpeed[obj]);
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

/* Function start: 0x4340F0 */
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
    desiredSpeed = anShipSpeed[reference] - 0x200;
    if (desiredSpeed < 0)
        desiredSpeed = 0;
    ScaleFixedVector(&aShipForwardVector[reference],
                     anShipSpeed[reference] * 15,
                     &forwardTravel);
    AddFixedVectors(destination, &forwardTravel, &projectedDestination);
    get_facing_range_from_point(obj, &projectedDestination);
    forwardFacing = nFacingToTarget;
    get_facing_range_from_point(obj, destination);
    if (nFacingToTarget < 0) {
        approach_speed(obj, 0);
        speed = FixedToShortSaturating(anShipSpeed[reference]);
        speed = MaxShort(speed, 1);
        if (nTargetRange / speed > 49)
            pointAtDestination = 1;
    } else if (nTargetRange > 175 && no_goal(obj) != 0) {
        pointAtDestination = 1;
    }
    control_speed(obj, (unsigned short)nTargetRange,
                  desiredSpeed);
    if (nFacingToTarget > 85 &&
        nTargetRange > 175)
        anRollGoal[obj] =
            match_roll_orientation(obj, reference);
    if (pointAtDestination != 0)
        point_ship(obj, 0, &vToTarget);
    if (nTargetRange < 175) {
        if (forwardFacing > 90) {
            copy_frame(reference, obj);
            steady_object(obj);
            approach_speed(obj, anShipSpeed[reference]);
            return 0;
        }
        point_ship(obj, 0, &forwardTravel);
    }
    if (nTargetRange < 600)
        trim_goals(obj, 10);
    return 0;
}

/* Function start: 0x4342C0 */
unsigned int goto_location(short obj, const FixedVector *destination)
{
    short range;
    short speed;

    ship_vs_point(obj, destination);
    if (nFacingToTarget < 51) {
        approach_half_speed(obj);
        if (no_goal(obj) != 0) {
            point_ship_at_point(obj, destination);
            return 0;
        }
    } else {
        range = nTargetRange;
        if (range <= 3000) {
            speed = FixedToShortSaturating(anShipSpeed[obj]);
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

/* Function start: 0x434360 */
unsigned int goto_formation(short obj, const FixedVector *destination,
                            short leader)
{
    short forwardFacing;
    FixedVector projected;

    AddFixedVectors(&aShipPosition[obj],
                    &aShipForwardVector[leader], &projected);
    get_facing_range_from_point(obj, &projected);
    forwardFacing = nFacingToTarget;
    get_facing_range_from_point(obj, destination);
    if (nFacingToTarget < 40) {
        approach_min_speed(obj);
    } else if (nTargetRange > 2000) {
        approach_full_speed(obj);
        if (nTargetRange > 3000 &&
            nFacingToTarget > 70 && normal_speed(obj) != 0)
            fire_afterburner(obj, 5);
    } else if (nTargetRange > 200) {
        control_speed(obj, (unsigned short)nTargetRange,
                      anShipSpeed[leader]);
    } else {
        approach_ship_speed(obj, leader);
    }
    if (nTargetRange < 200) {
        steady_object(obj);
        if (forwardFacing > 90) {
            copy_frame(leader, obj);
            return 0;
        }
        point_parallel(obj, leader);
    } else if (no_goal(obj) != 0) {
        point_ship(obj, 0, &vToTarget);
    }
    if (nTargetRange < 700) {
        trim_goals(obj, 10);
        if (forwardFacing > 90)
            anRollGoal[obj] =
                match_roll_orientation(obj, leader);
    }
    return 0;
}

/* Function start: 0x4344E0 */
unsigned int maintain_formation(short obj)
{
    short leader;
    FixedVector destination;

    leader = asShipWingLeader[obj];
    compute_formation_destination(leader,
                                  &aShipFormationOffset[obj],
                                  &destination);
    aShipDestination[obj] = destination;
    goto_formation(obj, &destination, leader);
    return 0;
}

/* Function start: 0x434550 */
unsigned int reset_stress(short obj)
{
    short damage;

    if (obj >= 12) {
        if (acPilotAggression[aiPilotLevel[obj]] == 0)
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
        acShipStress[obj] = (signed char)damage;
    }
    return 0;
}

/* Function start: 0x4345D0 */
short stress_morale(short obj)
{
    if (acShipStress[obj] < 15)
        return 0;
    if (acShipStress[obj] < 30)
        return 1;
    return 2;
}

/* Function start: 0x4345F0 */
enum ShipManeuver any_defense(short obj)
{
    signed char *maneuvers;
    short count;

    count = 0;
    maneuvers = apDefenseManeuvers[
        aiPilotLevel[obj]];
    while (maneuvers[count] != -1)
        count++;
    return (enum ShipManeuver)maneuvers[RandomBelowOrEqual(count)];
}

/* Function start: 0x434630 */
enum ShipManeuver pick_regular_maneuver(short obj, int event)
{
    short reroll;
    short morale;

    reroll = RandomBelowOrEqual(100) < 3 ||
             aeShipManeuver[obj] == MANEUVER_NONE;
    morale = stress_morale(obj);
    if (morale == 2)
        return MANEUVER_OUTA_HERE;
    if ((aiIntelligenceEvent[obj] == event ||
         RandomBelowOrEqual(100) < 20) &&
        (event == 0 || event == 3 || event == 4 || event == 7) &&
        reroll == 0)
        return aeShipManeuver[obj];

    switch (event) {
    case 0:
        if (aeObjectClass[
                acShipTarget[obj]] ==
            OBJECT_CLASS_CAPITAL_SHIP)
            return MANEUVER_STRAFE_ENEMY;
        if (RandomBelow(100) <
            aiPilotLevel[obj] * 5 + 60)
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
            aiPilotLevel[obj] * 20 + 30)
            return any_defense(obj);
        return MANEUVER_STRAFE_ENEMY;
    case 5:
        return MANEUVER_TAIL_FIRE;
    case 6:
        if (aiPilotLevel[obj] >= 2)
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

/* Function start: 0x434800 */
enum ShipManeuver pick_from_list(const ManeuverChoice *choice, short obj)
{
    enum ShipManeuver maneuver;
    short chooseAgain;

    maneuver = aeShipManeuver[obj];
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

/* Function start: 0x4348A0 */
enum ShipManeuver pick_kilrathi_maneuver(short obj, int event)
{
    const ManeuverChoice *choice;
    enum ShipManeuver maneuver;

    choice =
        &aKilrathiManeuverChoices[
            aiPilotLevel[obj]][event][stress_morale(obj)];
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

/* Function start: 0x434900 */
unsigned int process_maneuver_node(short obj, int event)
{
    const ManeuverChoice *choice;
    short rating;
    short morale;
    enum ShipManeuver maneuver;

    rating = (short)acShipRating[obj];
    if (rating == -1) {
        if (aeShipSide[obj] == SIDE_KILRATHI)
            maneuver = pick_kilrathi_maneuver(obj, event);
        else
            maneuver = pick_regular_maneuver(obj, event);
    } else {
        morale = stress_morale(obj);
        choice =
            &aRatedManeuverChoices[rating][event][morale];
        maneuver = pick_from_list(choice, obj);
    }
    if (aeShipManeuver[obj] != maneuver)
        reset_maneuver(obj, (short)maneuver);
    return 0;
}

/* Function start: 0x434980 */
void handle_stress(short obj, int event)
{
    short aggression;
    short damage;

    aggression =
        acPilotAggression[aiPilotLevel[obj]];
    switch (event) {
    case 3:
    case 4:
    case 7:
        acShipStress[obj] += aggression;
        break;
    case 5:
        acShipStress[obj] -= aggression;
        break;
    case 6:
        acShipStress[obj] += aggression * 2;
        break;
    case 8:
        acShipStress[obj] /= 2;
        break;
    case -1:
    case 2:
        acShipStress[obj] -=
            acPilotRecovery[aiPilotLevel[obj]];
        break;
    }
    damage = evaluate_damage(obj);
    if (damage < 40) {
        acShipStress[obj] += aggression * 2;
    } else if (damage < 75) {
        acShipStress[obj] =
            (signed char)MinShort(
                (short)((short)acShipStress[obj] + aggression),
                28);
    } else {
        acShipStress[obj] =
            (signed char)MinShort(acShipStress[obj], 7);
    }
    if (event == 6 && acShipStress[obj] >= 30)
        acShipStress[obj] = 29;
    if (acShipStress[obj] < 0)
        acShipStress[obj] = 0;
}

/* Function start: 0x434A80 */
void intelligence_events(short obj)
{
    int event;
    short targetGone;
    short target;
    short previousStress;
    short playerDamage;

    event = -1;
    targetGone = 0;
    target = acShipTarget[obj];
    previousStress = (short)acShipStress[obj];
    if (missile_on_tail(obj) != 0) {
        event = 6;
    } else if (unactive(target) != 0) {
        targetGone = 1;
    } else if (aeSpecialManeuver[
                   acShipTarget[obj]] ==
               SPECIAL_MANEUVER_UNKNOWN_9) {
        event = 8;
    } else {
        event = 0;
        ship_vs_ship(obj, target);
        if (nTargetRange > 8000) {
            event = 2;
        } else if (acShipAiCooldown[obj] > 0) {
            event = 7;
        } else if (nFacingToTarget > 55 &&
                   nTargetFacing < -55) {
            event = 5;
        } else if (nFacingToTarget > 75 &&
                   nTargetFacing > 75) {
            event = 4;
        } else if (nFacingToTarget < -60 &&
                   nTargetFacing > 85 &&
                   nTargetRange < 7000) {
            event = 3;
        } else if (anShipSpeed[target] < 20) {
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

    if (nYourWingman == obj &&
        aeObjectClass[0] == OBJECT_CLASS_SHIP &&
        acWingmanMessageState[nYourWingman] == -1) {
        if (previousStress < 15 && acShipStress[obj] >= 15) {
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
    aiIntelligenceEvent[obj] = event;
}

/* Function start: 0x434C70 */
unsigned int chase_speed(short obj, short range)
{
    short targetRange = nTargetRange;

    if (range < targetRange) {
        approach_full_speed(obj);
        return 0;
    }
    if (range > targetRange) {
        approach_zero_speed(obj);
        return 0;
    }
    approach_speed(obj,
                      anShipSpeed[
                          acShipTarget[obj]]);
    return 0;
}
