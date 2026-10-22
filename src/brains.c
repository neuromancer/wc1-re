/* NPC pilot intelligence (BRAINS.C).
 * The surviving WINGLEADER module and Mac brain symbol run corroborate the
 * cruise_home through FF_missile_intelligence group. See docs/ORDER.md.
 */
#define WC1_BRAINS_SOURCE
#include "wc1.h"
#ifndef SDL_PORT
#include <dos.h>
#endif
#include <sys/types.h>
#include <assert.h>

short nEnemySighting = 0x7fff; /* 0x00465c7c */

/* Linker commons defined here; LINK orders them by identifier hash. */
unsigned char *pScrambleDetailShape; /* 0x005a86bc */
short nScrambleCockpitAngle; /* 0x005a86c0 */
ShortPoint stScrambleCockpitDetail; /* 0x005a86c4 */
short nFuneralCasketPositionX; /* 0x005a86c8 */
short nFuneralCasketPosY; /* 0x005a86ca */

/* Function start: 0x409760 */
void cruise_home(short obj)
{
    short range;
    short objective;

    if (abandoned(obj, 0) != 0)
        return;

    if ((abShipTurnList[obj] & 7) == 5) {
        if (nYourWingman == obj &&
            distance_from_object(obj, 0) > 16000)
            remove_object(obj);

        if (aeObjectClass[obj] == OBJECT_CLASS_CAPITAL_SHIP)
            approach_cruise_speed(obj);
        else if (normal_speed(obj) != 0)
            fire_afterburner(obj, 10);

        if (no_goal(obj) != 0)
            point_ship_at_point(obj, &aShipDestinationsArray[obj]);
        range = distance_from_point(obj, &aShipDestinationsArray[obj]);

        if (equ_vector(&aShipDestinationsArray[obj],
                       &aShipMissionSpotArray[obj])) {
            if (range < 5000) {
                reset_tactic(obj, TACTIC_HEAD_HOME);
                set_special(obj, SPECIAL_MANEUVER_KILL_ENGINES);
                zero_vector(&aShipVelocity[obj]);
            }
        } else if (range < 1500) {
            objective = abFlightPathList[abShipNavPtIdx[obj]];
            if (aMissionObjectivesArray[objective].type != 1)
                flag_objective(objective, 1);
            get_follow_point(obj, &aShipDestinationsArray[obj]);
        }
    }
}

/* Function start: 0x4098C0 */
void fail(short obj)
{
    reset_objective(obj, OBJECTIVE_NONE);
}

/* Function start: 0x4098D0 */
void coming_home(short obj)
{
    short objective;

    switch (aeShipTacticArray[obj]) {
    case TACTIC_NONE:
        reset_tactic(obj, TACTIC_CRUISE);
        if (aeShipSide[obj] == SIDE_IMPERIAL &&
            aeShipMissionType[obj] == MISSION_TYPE_ROUT) {
            objective = find_objective(1, -1);
            aShipDestinationsArray[obj] =
                aMissionObjectivesArray[objective].position;
        } else {
            get_first_follow_point(obj, &aShipDestinationsArray[obj]);
        }
        break;
    case TACTIC_CRUISE:
        cruise_home(obj);
        break;
    case TACTIC_HEAD_HOME:
        if (no_goal(obj) != 0)
            point_parallel(obj, find_ship_index(stCurrentMissionHeader.homeMissionShip));
        break;
    default:
        fail(obj);
        break;
    }
}

/* Function start: 0x4099C0 */
void run_away(short obj)
{
    FixedVector direction;

    if (unactive(asShipWingLeaderArray[obj]) == 0 &&
        aeShipMissionType[
            asShipWingLeaderArray[obj]] == MISSION_TYPE_ROUT) {
        maintain_formation(obj);
        return;
    }
    if (aeShipSide[obj] == SIDE_IMPERIAL) {
        coming_home(obj);
        return;
    }

    zero_vector(&direction);
    direction.y = 0x100;
    if ((obj & 1) != 0)
        direction.y = -0x100;
    point_ship(obj, 0, &direction);
    if (normal_speed(obj) != 0 &&
        (RandomBelow(100) < 50 || any_enemy(obj, 16000) != 0))
        fire_afterburner(obj, 40);
    else
        approach_full_speed(obj);
    if (distance_from_object(obj, 0) > 16000)
        remove_object(obj);
}

/* Function start: 0x409AC0 */
short check_engage_target(short obj)
{
    short newTarget = detect_enemy_tail(obj);

    if (newTarget != -1 && newTarget != acShipTargetTable[obj])
        acShipTargetTable[obj] = newTarget;
    else if (target_valid(obj) == 0)
        select_target(obj);
    return acShipTargetTable[obj];
}

/* Function start: 0x409B10 */
short check_destroy_target(short obj)
{
    short destroyTarget = find_ship_index(anShipMissionShips[obj]);

    if (destroyTarget == -1) {
        acShipTargetTable[obj] = check_engage_target(obj);
    } else if (aeObjectClass[destroyTarget] ==
                   OBJECT_CLASS_FUTURION ||
               gone_ship(anShipMissionShips[obj]) != 0) {
        check_engage_target(obj);
    } else if (evaluate_damage(obj) > 70 - MaxShort(
                   0, MinShort(4, (short)aiPilotLevel[obj])) * 15) {
        acShipTargetTable[obj] = destroyTarget;
        if (aeShipSide[destroyTarget] ==
            aeShipSide[obj])
            acShipTargetTable[obj] = -1;
    } else if (target_valid(obj) != 0 && RandomBelow(100) > 3) {
        check_engage_target(obj);
    } else {
        acShipTargetTable[obj] = destroyTarget;
    }
    return acShipTargetTable[obj];
}

/* Function start: 0x409C20 */
void maneuvering(short obj, short newTarget)
{
    acShipTargetTable[obj] = newTarget;
    intelligence_events(obj);
    perform_maneuver(obj);
}

/* Function start: 0x409C50 */
void formation_burst(short obj)
{
    approach_full_speed(obj);
    if (no_goal(obj) != 0)
        point_ship(obj, 0, &aShipDestinationsArray[obj]);
    asShipTotal[obj]++;
    if (asShipTotal[obj] > 9) {
        if (aeShipMissionType[obj] == MISSION_TYPE_STRIKE)
            engage(obj, acShipTargetTable[obj],
                   OBJECTIVE_DESTROY_SHIP);
        else
            engage(obj, acShipTargetTable[obj],
                   OBJECTIVE_ENGAGE_ENEMY);
    }
}

/* Function start: 0x409CE0 */
void disallow_engage(void)
{
    bEngageAllowed = 0;
}

/* Function start: 0x409CF0 */
void allow_engage(void)
{
    bEngageAllowed = 1;
    nAutoEngageTimer = -1;
}

/* Function start: 0x409D10 */
void try2allow_engage(int pilotLevel)
{
    if (pilotLevel <= 4) {
        allow_engage();
        return;
    }
    if (pilotLevel == 8 || pilotLevel == 11 || pilotLevel == 6) {
        allow_engage();
        return;
    }
    if (pilotLevel == 5 && RandomBelowOrEqual(100) < 50) {
        allow_engage();
        return;
    }
    nAutoEngageTimer = -40;
}

/* Function start: 0x409D60 */
void imperial_formation(short obj)
{
    short leader = asShipWingLeaderArray[obj];
    short target;
    FixedVector dest;
    short closest;

    if (leader == -1)
        leader = obj;
    maintain_formation(obj);
    if (attacker_in_range(leader, 12000) != 0) {
        if (obj == nYourWingman ||
            nYourWingman == -1) {
            if (nAutoEngageTimer < -1)
                nAutoEngageTimer++;
            else if (nAutoEngageTimer != -1 &&
                     --nAutoEngageTimer == 0)
                try2allow_engage(aiPilotLevel[obj]);
        }
        if (bEngageAllowed != 0) {
            engage(obj, nTargetShip,
                   OBJECTIVE_ENGAGE_ENEMY);
        } else if (obj == nYourWingman &&
                   nAutoEngageTimer == -1) {
            send_message(obj, 3);
            nAutoEngageTimer = 40;
        }
    } else if (obj == nYourWingman &&
               nEnemySighting != nCurrentWave &&
               any_enemy(obj, 16000) != 0 &&
               message_showing() == 0 && nCameraViewMode == 0) {
        send_message(obj, 2);
        nEnemySighting = nCurrentWave;
    }

    if (aeSpecialManeuvers[obj] == SPECIAL_MANEUVER_NONE)
        if (distance_from_object(obj, leader) > 9000)
            if (facing_to_object(obj, &aShipPositionsArray[leader]) > 85 &&
                real_velocity(obj) < 110)
                fire_afterburner(obj, 10);
            else {
                point_ship_at_object(obj, leader);
                approach_ship_speed(obj, leader);
            }
}

/* Function start: 0x409F00 */
void formation_break(short obj)
{
    switch (acShipSequence[obj]) {
    case 0:
        steady_object(obj);
        anYawTarget[obj] = -30;
        anRollGoal[obj] = -45;
        anPitchGoal[obj] = -20;
        acShipSequence[obj]++;
        break;
    case 1:
        if (no_goal(obj) != 0)
            engage(obj, acShipTargetTable[obj],
                   OBJECTIVE_ENGAGE_ENEMY);
        break;
    default:
        acShipSequence[obj] = 0;
        break;
    }
}

/* Function start: 0x409F80 */
void imperial_wingman(short obj)
{
    switch (aeShipObjectiveList[obj]) {
    case OBJECTIVE_DESTROY_SHIP:
        maneuvering(obj, check_destroy_target(obj));
        break;
    case OBJECTIVE_ENGAGE_ENEMY:
        maneuvering(obj, check_engage_target(obj));
        break;
    case OBJECTIVE_HOLD_FORMATION:
        imperial_formation(obj);
        break;
    case OBJECTIVE_BREAK_FORMATION:
        formation_break(obj);
        break;
    case OBJECTIVE_NONE:
        reset_objective(obj, OBJECTIVE_HOLD_FORMATION);
        break;
    default:
        fail(obj);
        break;
    }
}

/* Function start: 0x40A030 */
void kilrathi_wingman(short obj)
{
    short leader = asShipWingLeaderArray[obj];
    enum ShipObjective objective;

    if (leader == -1) {
        change_mission_type(obj, MISSION_TYPE_PATROL);
        return;
    }
    if (unactive(leader) != 0) {
        inherit_leader(obj);
        return;
    }
    objective = aeShipObjectiveList[leader];
    if ((objective == OBJECTIVE_ENGAGE_ENEMY ||
         objective == OBJECTIVE_DESTROY_SHIP) &&
        aeShipObjectiveList[obj] != objective)
        engage(obj, acShipTargetTable[obj], objective);

    switch (aeShipObjectiveList[obj]) {
    case OBJECTIVE_DESTROY_SHIP:
    case OBJECTIVE_ENGAGE_ENEMY:
        maneuvering(obj, check_engage_target(obj));
        break;
    case OBJECTIVE_HOLD_FORMATION:
        maintain_formation(obj);
        break;
    case OBJECTIVE_BREAK_FORMATION:
        formation_burst(obj);
        break;
    case OBJECTIVE_NONE:
        reset_objective(obj, OBJECTIVE_HOLD_FORMATION);
        break;
    default:
        fail(obj);
        break;
    }
}

/* Function start: 0x40A130 */
void wingman_mission(short obj)
{
    if (aeShipSide[obj] == SIDE_IMPERIAL) {
        imperial_wingman(obj);
        return;
    }
    kilrathi_wingman(obj);
}

/* Function start: 0x40A160 */
short dist_from_home(short obj)
{
    return distance_from_point(obj, &aShipMissionSpotArray[obj]);
}

/* Function start: 0x40A180 */
short scan_and_lock(short obj, int scanRange, enum ShipTactic newTactic)
{
    acShipTargetTable[obj] = scan_for_enemy(obj, 14000);
    if (acShipTargetTable[obj] != -1)
        aeShipTacticArray[obj] = newTactic;
    return acShipTargetTable[obj] != -1;
}

/* Function start: 0x40A1C0 */
void patrol_area(short obj)
{
    short target = acShipTargetTable[obj];

    switch (aeShipTacticArray[obj]) {
    case TACTIC_HEAD_HOME:
        approach_cruise_speed(obj);
        if (scan_and_lock(obj, 14000, TACTIC_APPROACH_TARGET) == 0) {
            ship_vs_point(obj, &aShipMissionSpotArray[obj]);
            if (nTgtRange < 3000) {
                reset_tactic(obj, TACTIC_LOOK_OUT);
                return;
            }
            point_ship_at_point(obj, &aShipMissionSpotArray[obj]);
            trim_goals(obj, 7);
        }
        break;
    case TACTIC_LOOK_OUT:
        approach_cruise_speed(obj);
        if (scan_and_lock(obj, 14000, TACTIC_APPROACH_TARGET) == 0 &&
            dist_from_home(obj) > 8000)
            reset_tactic(obj, TACTIC_HEAD_HOME);
        break;
    case TACTIC_APPROACH_TARGET:
        approach_full_speed(obj);
        if (unactive(target) != 0) {
            if (scan_and_lock(obj, 14000,
                              TACTIC_APPROACH_TARGET) == 0)
                alter_tactic(obj, TACTIC_LOOK_OUT);
        } else {
            ship_vs_ship(obj, target);
            if (nTgtRange < 10000) {
                init_formation_burst(obj);
                return;
            }
            if (no_goal(obj) != 0)
                point_ship_at_object(obj, target);
        }
        break;
    case TACTIC_NONE:
        reset_tactic(obj, TACTIC_APPROACH_TARGET);
        break;
    default:
        fail(obj);
        break;
    }
}

/* Function start: 0x40A360 */
void kilrathi_patrol(short obj)
{
    switch (aeShipObjectiveList[obj]) {
    case OBJECTIVE_WANDER:
    case OBJECTIVE_HOLD_FORMATION:
        patrol_area(obj);
        break;
    case OBJECTIVE_ENGAGE_ENEMY:
        maneuvering(obj, check_engage_target(obj));
        break;
    case OBJECTIVE_BREAK_FORMATION:
        formation_burst(obj);
        break;
    case OBJECTIVE_NONE:
        aeShipObjectiveList[obj] = OBJECTIVE_WANDER;
        aeShipTacticArray[obj] = TACTIC_APPROACH_TARGET;
        break;
    default:
        fail(obj);
        break;
    }
}

/* Function start: 0x40A400 */
void imperial_wingleader(short obj)
{
    kilrathi_patrol(obj);
}

/* Function start: 0x40A410 */
void cruise_to_destination(short obj)
{
    short range;

    if (abandoned(obj, 0) != 0)
        return;

    if ((abShipTurnList[obj] & 7) == 6)
        acShipTargetTable[obj] = scan_for_enemy(obj, 15000);

    if (acShipTargetTable[obj] == -1) {
        approach_cruise_speed(obj);
    } else {
        get_facing_range_from_object(obj, acShipTargetTable[obj]);
        if (nFacingToTargetValue > 65)
            approach_half_speed(obj);
        else
            approach_full_speed(obj);
    }

    if ((abShipTurnList[obj] & 7) == 2) {
        if (no_goal(obj) != 0)
            point_ship_at_point(obj, &aShipDestinationsArray[obj]);
        range = distance_from_point(obj, &aShipDestinationsArray[obj]);
        if (range < 1500) {
            if (aeShipSide[obj] == SIDE_IMPERIAL)
                flag_reached(abFlightPathList[abShipNavPtIdx[obj]], 1);
            if (equ_vector(&aShipDestinationsArray[obj],
                           &aShipMissionSpotArray[obj])) {
                reset_tactic(obj, TACTIC_SIT_STILL);
                set_special(obj, SPECIAL_MANEUVER_KILL_ENGINES);
            } else {
                get_follow_point(obj, &aShipDestinationsArray[obj]);
            }
        }
    }
}

/* Function start: 0x40A540 */
void prepare_for_jump(short obj)
{
    if (anShipSpeed[obj] != 0) {
        set_special(obj, SPECIAL_MANEUVER_STOP_DRIFT);
    } else if (++asShipTotal[obj] >
               (aeShipSide[obj] == SIDE_KILRATHI ? 250 : 25)) {
        get_facing_range_from_object(obj, 0);
        if (nFacingToTargetValue > 90 && no_goal(obj) != 0) {
            anYawTarget[obj] = signed_random(30);
        } else if (asShipTotal[obj] >
                       (aeShipSide[obj] == SIDE_KILRATHI ? 270 : 45) ||
                   (nTargetFacingValue > 80 && nTgtRange < 6000)) {
            reset_tactic(obj, TACTIC_WARP_OUT);
            fire_afterburner(obj, 10);
        }
    }
}

/* Function start: 0x40A630 */
void accelerate_and_jump(short obj)
{
    approach_full_speed(obj);
    if (asShipTotal[obj]++ == 4)
        warp(obj);
}

/* Function start: 0x40A670 */
void reach_warp(short obj)
{
    switch (aeShipTacticArray[obj]) {
    case TACTIC_CRUISE:
        cruise_to_destination(obj);
        break;
    case TACTIC_SIT_STILL:
        prepare_for_jump(obj);
        break;
    case TACTIC_WARP_OUT:
        accelerate_and_jump(obj);
        break;
    case TACTIC_NONE:
        reset_tactic(obj, TACTIC_CRUISE);
        get_first_follow_point(obj, &aShipDestinationsArray[obj]);
        break;
    default:
        fail(obj);
        break;
    }
}

/* Function start: 0x40A710 */
void warp_arrival(short obj)
{
    if (aeShipTacticArray[obj] == TACTIC_WARP_IN)
        arrive_from_warp(obj);
    else
        reset_tactic(obj, TACTIC_WARP_IN);
}

/* Function start: 0x40A740 */
void return_to_buddy(short obj, short buddy)
{
    approach_cruise_speed(obj);
    if (no_goal(obj) != 0)
        point_ship_at_object(obj, buddy);
    if (distance_from_object(obj, buddy) < 1000) {
        reset_objective(obj, OBJECTIVE_WANDER);
        point_parallel(obj, buddy);
    }
}

/* Function start: 0x40A7A0 */
void escort_buddy(short obj, short buddy)
{
    approach_ship_speed(obj, buddy);
    if (no_goal(obj) != 0)
        point_parallel(obj, buddy);
}

/* Function start: 0x40A7D0 */
void escort_mission(short obj)
{
    short buddy = find_ship_index(anShipMissionShips[obj]);

    if (unactive(buddy) != 0) {
        change_mission_type(obj, MISSION_TYPE_PATROL);
        return;
    }
    if ((abShipTurnList[obj] & 3) == 0 && in_danger(buddy) != 0 &&
        nTgtRange < 3000)
        engage(obj, nTargetShip,
               OBJECTIVE_ENGAGE_ENEMY);
    if (aeShipObjectiveList[obj] != OBJECTIVE_HOME_BASE &&
        (abShipTurnList[obj] & 7) == 4 &&
        distance_from_object(obj, buddy) > 5000)
        reset_objective(obj, OBJECTIVE_HOME_BASE);

    switch (aeShipObjectiveList[obj]) {
    case OBJECTIVE_HOME_BASE:
        return_to_buddy(obj, buddy);
        break;
    case OBJECTIVE_WANDER:
        escort_buddy(obj, buddy);
        break;
    case OBJECTIVE_ENGAGE_ENEMY:
        maneuvering(obj, check_engage_target(obj));
        break;
    case OBJECTIVE_NONE:
        reset_objective(obj, OBJECTIVE_WANDER);
        break;
    default:
        fail(obj);
        break;
    }
}

/* Function start: 0x40A900 */
void check_goal(short obj)
{
    if (gone_ship(anShipMissionShips[obj]) != 0)
        reset_mission_type(obj, MISSION_TYPE_ROUT);
    else
        reset_mission_type(obj, MISSION_TYPE_PATROL);
}

/* Function start: 0x40A940 */
void streak_toward(short obj, short goal, short range)
{
    if (no_goal(obj) != 0) {
        if (RandomBelow(100) < 95)
            point_ship_at_object(obj, goal);
        else
            veer_random(obj, 20);
    }
    if (range > 2000 && normal_speed(obj) != 0)
        fire_afterburner(obj, 10);
    else
        approach_full_speed(obj);
}

/* Function start: 0x40A9B0 */
void approach_and_engage(short obj, short goal)
{
    unsigned short range = (unsigned short)distance_from_object(obj, goal);
    unsigned short possibleRange;
    short possibleTarget;

    if (aeObjectClass[goal] != OBJECT_CLASS_FUTURION &&
        evaluate_damage(obj) > 70 - MaxShort(
            0, MinShort(4, (short)aiPilotLevel[obj])) * 15 &&
        range > 5000) {
        streak_toward(obj, goal, (short)range);
        return;
    }
    possibleTarget = scan_for_enemy(obj, 10000);
    possibleRange = (unsigned short)nTgtRange;
    if (possibleTarget != -1 &&
        (possibleRange * 3 < range ||
         aeObjectClass[goal] == OBJECT_CLASS_FUTURION)) {
        init_formation_burst(obj);
        acShipTargetTable[obj] = possibleTarget;
    } else if (range < 5000) {
        engage(obj, goal, OBJECTIVE_DESTROY_SHIP);
    } else {
        streak_toward(obj, goal, (short)range);
    }
}

/* Function start: 0x40AAC0 */
void strike_mission(short obj)
{
    short goal = find_ship_index(anShipMissionShips[obj]);

#ifdef SDL_PORT
    /* The original source indexes class[-1] here.  In the Win32 layout that
       aliases the last two previous-distance words; the intended no-goal
       branch is check_goal. */
    if (goal == -1)
#else
    if (goal == -1 &&
        aeObjectClass[goal] != OBJECT_CLASS_FUTURION)
#endif
        check_goal(obj);
    switch (aeShipObjectiveList[obj]) {
    case OBJECTIVE_HOME_BASE:
    case OBJECTIVE_HOLD_FORMATION:
        approach_and_engage(obj, goal);
        break;
    case OBJECTIVE_DESTROY_SHIP:
        maneuvering(obj, check_destroy_target(obj));
        break;
    case OBJECTIVE_ENGAGE_ENEMY:
        maneuvering(obj, check_destroy_target(obj));
        break;
    case OBJECTIVE_BREAK_FORMATION:
        formation_burst(obj);
        break;
    case OBJECTIVE_NONE:
        reset_objective(obj, OBJECTIVE_HOME_BASE);
        break;
    default:
        fail(obj);
        break;
    }
}

/* Function start: 0x40ABB0 */
void return_to_master(short obj, short master)
{
    short range = distance_from_object(obj, master);

    streak_toward(obj, master, range);
    if (range < 5000) {
        reset_objective(obj, OBJECTIVE_WANDER);
        point_perpendicular(obj, master);
    }
}

/* Function start: 0x40AC00 */
void defend_mission(short obj)
{
    short master = find_ship_index(anShipMissionShips[obj]);

    if (master == -1) {
        change_mission_type(obj, MISSION_TYPE_PATROL);
        return;
    }

    if (abShipTurnList[obj] % 10 == 0)
        if (in_danger(master) != 0)
            if (nTgtRange < 6000) {
                if (aeShipObjectiveList[obj] != OBJECTIVE_ENGAGE_ENEMY)
                    engage(obj, nTargetShip, OBJECTIVE_ENGAGE_ENEMY);
            }

    if (aeShipObjectiveList[obj] != OBJECTIVE_HOME_BASE)
        if ((abShipTurnList[obj] & 7) == 4)
            if (distance_from_object(obj, master) > 10000)
                reset_objective(obj, OBJECTIVE_HOME_BASE);

    switch (aeShipObjectiveList[obj]) {
    case OBJECTIVE_WANDER:
        acShipTargetTable[obj] = scan_for_enemy(obj, 7000);
        if (acShipTargetTable[obj] != -1)
            engage(obj, acShipTargetTable[obj], OBJECTIVE_ENGAGE_ENEMY);
        else {
            approach_half_speed(obj);
            if (no_goal(obj) != 0)
                point_perpendicular(obj, master);
        }
        break;
    case OBJECTIVE_HOME_BASE:
        return_to_master(obj, master);
        break;
    case OBJECTIVE_ENGAGE_ENEMY:
        maneuvering(obj, check_engage_target(obj));
        break;
    case OBJECTIVE_NONE:
        reset_objective(obj, OBJECTIVE_WANDER);
        break;
    default:
        fail(obj);
    }
}

/* Function start: 0x40AD80 */
void rendezvous_mission(short obj)
{
    short goal = find_ship_index(anShipMissionShips[obj]);

    if (unactive(goal) != 0) {
        change_mission_type(obj, MISSION_TYPE_PATROL);
        return;
    }
    switch (aeShipObjectiveList[obj]) {
    case OBJECTIVE_REACH_SHIP:
        if (attacker_in_range(obj, 3500) != 0)
            engage(obj, nTargetShip,
                   OBJECTIVE_ENGAGE_ENEMY);
        if (distance_from_object(obj, goal) < 2500) {
            reset_mission_type(obj, MISSION_TYPE_DEFEND);
            return;
        }
        if (attacker_in_range(goal, 9000) != 0)
            approach_full_speed(obj);
        else
            approach_cruise_speed(obj);
        if (no_goal(obj) != 0)
            point_ship_at_object(obj, goal);
        break;
    case OBJECTIVE_ENGAGE_ENEMY:
        maneuvering(obj, check_engage_target(obj));
        break;
    default:
        reset_objective(obj, OBJECTIVE_REACH_SHIP);
        break;
    }
}

/* Function start: 0x40AE80 */
void ship_intelligence(short obj)
{
    short target;

    if (regulate_turn(obj) != 0)
        return;
    switch (aeShipMissionType[obj]) {
    case MISSION_TYPE_PATROL:
        /* The retail source tests the array address rather than this ship's
         * side, leaving the Imperial arm present but unreachable. */
#ifdef SDL_PORT
        /* SWC CODE_02 ship_intelligence +0x1c2c reads this ship's side. */
        if (SdlSwcFlightActive() ? aeShipSide[obj] == SIDE_IMPERIAL : aeShipSide == 0)
#else
        if (aeShipSide == 0)
#endif
            imperial_wingleader(obj);
        else
            kilrathi_patrol(obj);
        break;
    case MISSION_TYPE_ESCORT:
        escort_mission(obj);
        break;
    case MISSION_TYPE_STRIKE:
        strike_mission(obj);
        break;
    case MISSION_TYPE_DEFEND:
        defend_mission(obj);
        break;
    case MISSION_TYPE_WINGMAN:
        wingman_mission(obj);
        break;
    case MISSION_TYPE_ROUT:
        run_away(obj);
        break;
    case MISSION_TYPE_GOTO_WARP:
        reach_warp(obj);
        break;
    case MISSION_TYPE_WARP_ARRIVE:
        warp_arrival(obj);
        break;
    case MISSION_TYPE_RENDEZVOUS:
        rendezvous_mission(obj);
        break;
    case MISSION_TYPE_COME_HOME:
        coming_home(obj);
        break;
    case MISSION_TYPE_NONE:
        inherit_leader_mission(obj);
        break;
    default:
        fail(obj);
        break;
    }
    if (acShipAiCooldowns[obj] > 0)
        acShipAiCooldowns[obj]--;
}

/* Function start: 0x40AF70 */
void orbit_sphere(short obj)
{
    short radius = aMissionNavPoints[
        nCurrentNavigationPoint].proximityRadius >> 1;
    FixedVector center =
        aMissionNavPoints[nCurrentNavigationPoint].position;
    short range = distance_from_point(obj, &center);

    if (no_goal(obj) != 0)
        if (range > radius - 750)
            if (range > radius)
                point_ship_at_point(obj, &center);
            else
                point_perpendicular_to_point(obj, &center);
    trim_goals(obj, 10);
}

/* Function start: 0x40B010 */
void tanker_intelligence(short obj)
{
    if (attacker_in_range(obj, 3000) != 0) {
        approach_full_speed(obj);
        acShipTargetTable[obj] = nTargetShip;
        fire(obj, nTargetShip);
        if (no_goal(obj) != 0) {
            if (RandomBelowOrEqual(4) == 0) {
                anYawTarget[obj] = signed_random(90);
                anRollGoal[obj] = signed_random(90);
            } else {
                point_capital_ship_at_object(obj,
                                             nTargetShip);
            }
        }
        return;
    }
    approach_cruise_speed(obj);
    orbit_sphere(obj);
}

/* Function start: 0x40B0C0 */
void destroyer_intelligence(short obj)
{
    if (fire_turrets(obj) != 0) {
        acShipTargetTable[obj] = -1;
        approach_half_speed(obj);
    } else {
        approach_cruise_speed(obj);
    }
    orbit_sphere(obj);
}

/* Function start: 0x40B110 */
void stationary_intelligence(short obj)
{
    if (aeObjectTypeList[obj] == OBJECT_TYPE_KILRATHI_BASE) {
        anObjectYawRotation[obj] = 4;
        fire_turrets(obj);
    }
}

/* Function start: 0x40B140 */
void capital_ship_intelligence(short obj)
{
    if (regulate_turn(obj) != 0)
        return;
    switch (aeShipMissionType[obj]) {
    case MISSION_TYPE_ROUT:
        run_away(obj);
        return;
    case MISSION_TYPE_GOTO_WARP:
        reach_warp(obj);
        return;
    case MISSION_TYPE_WARP_ARRIVE:
        warp_arrival(obj);
        return;
    case MISSION_TYPE_COME_HOME:
        coming_home(obj);
        return;
    case MISSION_TYPE_NONE:
        stationary_intelligence(obj);
        return;
    default:
        break;
    }

    if (aeObjectTypeList[obj] == OBJECT_TYPE_DORKIR ||
        aeObjectTypeList[obj] == OBJECT_TYPE_LUMBARI) {
        tanker_intelligence(obj);
        return;
    }
    if (aeObjectTypeList[obj] == OBJECT_TYPE_SPIKERI ||
        aeObjectTypeList[obj] == OBJECT_TYPE_RALARI ||
        aeObjectTypeList[obj] == OBJECT_TYPE_FRALTHI ||
        aeObjectTypeList[obj] == OBJECT_TYPE_SNAKEIR ||
        aeObjectTypeList[obj] == OBJECT_TYPE_SIVAR ||
        aeObjectTypeList[obj] == OBJECT_TYPE_KILRATHI_BASE) {
        destroyer_intelligence(obj);
        return;
    }

    if (unactive(nTargetShip = acShipTargetTable[obj]) != 0)
        scan_for_enemy(obj, 15000);
    switch (aeShipTacticArray[obj]) {
    case TACTIC_SELF_DEFENSE:
        approach_full_speed(obj);
        if (unactive(acShipTargetTable[obj]) != 0) {
            select_target(obj);
            if (unactive(acShipTargetTable[obj]) != 0)
                reset_tactic(obj, TACTIC_NONE);
        } else
            fire_turrets(obj);
        break;
    default:
        if (nTargetShip != -1) {
            approach_full_speed(obj);
            aeShipTacticArray[obj] = TACTIC_SELF_DEFENSE;
            acShipTargetTable[obj] = (signed char)nTargetShip;
            fire_turrets(obj);
        } else
            approach_cruise_speed(obj);
    }
}

/* Function start: 0x40B320 */
void futurion_intelligence(short obj)
{
    ship_vs_ship(0, obj);

    asActionCounts[obj]++;
    if (nTgtRange > 1000 && asActionCounts[obj] > 1000)
        aeObjectClass[obj] = (enum ObjectClass)asObjectCounterArray[obj];
    else if (asActionCounts[obj] > 200 && nTgtRange < 4000 &&
             nTgtRange > 1000 && nFacingToTargetValue > 80)
        aeObjectClass[obj] = (enum ObjectClass)asObjectCounterArray[obj];
}

/* Function start: 0x40B3A0 */
void mine_intelligence(short obj)
{
    short other;
    short distance;
    int objectIndex = obj;

    if (asObjectCounterArray[objectIndex] != -1)
        return;
    for (other = 0; other <= 9; other++) {
        if (other == obj || aeObjectClass[other] <
                            OBJECT_CLASS_SHIP)
            continue;
        distance = distance_from_object(obj, other);
        if (distance < aObjectTypeData[
                           aeObjectTypeList[objectIndex]].collisionRadius ||
            (distance < 50 && RandomBelowOrEqual(7) == 0)) {
            explode(obj, obj);
            return;
        }
    }
}

/* Function start: 0x40B430 */
void heat_seeking_missile_intelligence(short obj)
{
    short exhaustHeat;
    short targetShip;
    short targ;

    if (nFacingToTargetValue < 0 || acShipTargetTable[obj] == -1) {
        acShipTargetTable[obj] = -1;
        cViableTargetCount = 0;
        for (targetShip = 0; targetShip <= 9; targetShip++) {
            if (obj != targetShip &&
                aeObjectClass[targetShip] >= OBJECT_CLASS_SHIP) {
                get_facing_range_from_object(obj, targetShip);
                if (nTgtRange < 9000 && nFacingToTargetValue > 0 &&
                    nTargetFacingValue < 0) {
                    asViableTgtDistList[cViableTargetCount] =
                        nTgtRange;
                    acViableTargetList[cViableTargetCount++] = targetShip;
                }
            }
        }
        sort_viable_target_list();
        if (cViableTargetCount > 0) {
            for (exhaustHeat = 3; exhaustHeat > 0; exhaustHeat--) {
                for (targ = 0; targ < cViableTargetCount; targ++) {
                    if (aeObjectClass[acViableTargetList[targ]] ==
                            OBJECT_CLASS_CAPITAL_SHIP ||
                        abShipExhaustHeatList[acViableTargetList[targ]] ==
                            exhaustHeat) {
                        acShipTargetTable[obj] = acViableTargetList[targ];
                        exhaustHeat = 0;
                        break;
                    }
                }
            }
        }
        if (acShipTargetTable[obj] == -1)
            explode(obj, obj);
    } else {
        point_ship(obj, 0, &vToTgt);
        anShipSpeed[obj] =
            (get_ship_max_velocity(obj) + 10) << 8;
    }
}

/* Function start: 0x40B570 */
void FF_missile_intelligence(short obj)
{
    short targetShip;

    if (aeShipTacticArray[obj] == TACTIC_RAM) {
        if (acShipTargetTable[obj] == -1) {
            cViableTargetCount = 0;
            for (targetShip = 0; targetShip <= 9; targetShip++) {
                if (obj != targetShip) {
                    if (aeObjectClass[targetShip] >= OBJECT_CLASS_SHIP) {
                        if (aeShipSide[targetShip] !=
                                aeShipSide[acObjOwnersTable[obj]] ||
                            acShipCommunicatorArray[targetShip] == -1) {
                            nTgtRange =
                                distance_from_object(obj, targetShip);
                            if (nTgtRange < 9000) {
                                asViableTgtDistList[
                                    cViableTargetCount] = nTgtRange;
                                acViableTargetList[cViableTargetCount++] =
                                    targetShip;
                            }
                        }
                    }
                }
            }
            sort_viable_target_list();
            if (cViableTargetCount > 0)
                acShipTargetTable[obj] = acViableTargetList[0];
        } else {
            point_ship(obj, 0, &vToTgt);
            anShipSpeed[obj] =
                (get_ship_max_velocity(obj) + 10) << 8;
        }
    }
}
