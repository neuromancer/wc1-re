/*
 *  Ship AI: dispatch-table states and the behaviour routines.
 *
 *  Address range 0x4060a0-0x40cfff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: the 47-slot dispatch table at 0x004656a8 targets this
 *  range almost exclusively.  The nested Mac brain compilation unit maps
 *  exactly to 0x409760-0x40b66f, with one additional Win32 split helper.
 */
#include "wc1.h"

/* Function start: 0x4060A0 */
void SetShipAiScratchWord(unsigned short v)
{
    DAT_00475e78 = v;
}

/* Function start: 0x4060B0 */
void SelectNewShipAiBehavior(short ship)
{
    set_special(ship, SPECIAL_MANEUVER_NONE);
    reset_maneuver(ship, MANEUVER_NONE);
}

/* Function start: 0x4060D0 */
void ShipAiState42(short ship, short target)
{
    if (g_aeSpecialManeuver_0059c3c0[target] ==
        SPECIAL_MANEUVER_NORMAL) {
        SelectNewShipAiBehavior(ship);
        return;
    }
    if (no_goal(ship) != 0) {
        point_ship_at_point(ship, &g_aShipPosition_0059c490[target]);
        g_anRollGoal_0059d630[ship] = 360;
    }
}

/* Function start: 0x4061E0 */
void ShipAiRoutine01(short ship)
{
    if (g_aeShipManeuver_0059dcb0[ship] != MANEUVER_NONE)
        g_acShipSequence_0059d520[ship] = g_acShipSequence_0059d520[ship] + 1;
}

/* Function start: 0x406C70 */
void ShipAiState25(short ship)
{
    if (g_acShipSequence_0059d520[ship] == 0) {
        set_special(ship, SPECIAL_MANEUVER_KILL_ENGINES);
        ShipAiRoutine01(ship);
    } else if (g_acShipSequence_0059d520[ship] == 1) {
        g_anPitchGoal_0059d7a0[ship] = 15;
        if (RandomBelow(100) < 50)
            ShipAiRoutine01(ship);
    } else {
        g_anPitchGoal_0059d7a0[ship] = -30;
        if (RandomBelow(100) < 50) {
            if (++g_asShipCount_0059c420[ship] < 10)
                g_acShipSequence_0059d520[ship] = 1;
            else
                SelectNewShipAiBehavior(ship);
        }
    }
}

/* Function start: 0x406D20 */
void Mtail_fire(short ship, short target)
{
    if (no_goal(ship) != 0)
        point_ship_at_object(ship, target);
    chase_speed(ship,
        (short)((g_asObjectCollisionRadius_0059d710[target] +
                 g_asObjectCollisionRadius_0059d710[ship] * 6) >> 1));
    fire_when_ready(ship, 1);
}

/* Function start: 0x406D80 */
void Mzip_past(short ship, short target)
{
    if (unactive(target) == 0) {
        if (close_behind(
                g_asObjectCollisionRadius_0059d710[target] + 2000) != 0) {
            Mtail_fire(ship, target);
            return;
        }
        approach_full_speed(ship);
        if (no_goal(ship) != 0) {
            if (g_nTargetFacing_0059d52a > 80)
                point_ship_below_object(ship, target);
            else
                point_ship_behind_object(ship, target);
        }
    } else {
        SelectNewShipAiBehavior(ship);
    }
}

/* Function start: 0x406FB0 */
void Mstrafe_enemy(short ship, short target)
{
    short aimed;

    approach_cruise_speed(ship);
    if (abs(g_anPitchGoal_0059d7a0[ship]) != 0 ||
        abs(g_anYawGoal_0059c310[ship]) != 0)
        aimed = 0;
    else
        aimed = 1;
    if (aimed != 0) {
        ship_vs_ship(ship, target);
        point_ship_at_object(ship, target);
    }
    fire_when_ready(ship, aimed == 0);
}

/* Function start: 0x407030 */
void Mbest_strafe(short ship, short target)
{
    if (g_nTargetFacing_0059d52a < 0x50) {
        Mstrafe_enemy(ship, target);
        return;
    }
    Mzip_past(ship, target);
}

/* Function start: 0x4070D0 */
void Mstrafe_n_roll(short ship, short target)
{
    if (0 < g_asObjectCounter_0059c330[ship]) {
        g_anRollGoal_0059d630[ship] = 0x2d;
        return;
    }
    Mstrafe_enemy(ship, target);
}

/* Function start: 0x407270 */
void general_zig(short ship, unsigned int target, short pitch)
{
    short complete = 1;

    (void)target;
    approach_full_speed(ship);
    switch (g_acShipSequence_0059d520[ship] % 6) {
    case 0:
        g_anYawGoal_0059c310[ship] = -35;
        g_anPitchGoal_0059d7a0[ship] = pitch;
        break;
    case 1:
    case 4:
        complete = no_goal(ship);
        g_asShipCount_0059c420[ship] = 0;
        break;
    case 2:
    case 5:
        complete = 1;
        if (++g_asShipCount_0059c420[ship] < 4)
            complete = 0;
        break;
    case 3:
        pitch = -pitch;
        g_anYawGoal_0059c310[ship] = 35;
        g_anPitchGoal_0059d7a0[ship] = pitch;
        break;
    }
    if (g_acShipSequence_0059d520[ship] >= 12)
        SelectNewShipAiBehavior(ship);
    if (complete != 0)
        ShipAiRoutine01(ship);
}

/* Function start: 0x407350 */
void Mzig_zag(short ship, unsigned int target)
{
    general_zig(ship, target, 0);
}

/* Function start: 0x407370 */
void Mzig_zag_pitch(short ship, unsigned int target)
{
    general_zig(ship, target, 0x23);
}

/* Function start: 0x407450 */
void ShipAiState02(short ship, short target)
{
    if (g_acShipSequence_0059d520[ship] == 0) {
        steer_away_from_object(ship, target,
            g_nFacingToTarget_0059d920 < 81 ? 10 : 40);
        ShipAiRoutine01(ship);
        return;
    }
    if (g_asObjectCollisionRadius_0059d710[target] * 3 <
        g_nTargetRange_0059ce10) {
        veer_random(ship, 8);
        SelectNewShipAiBehavior(ship);
        return;
    }
    if (no_goal(ship) != 0) {
        if (g_nFacingToTarget_0059d920 < 81) {
            if (g_nFacingToTarget_0059d920 <= -66 ||
                RandomBelowOrEqual(100) <= 3)
                veer_random(ship, 16);
        } else {
            steer_away_from_object(ship, target, 40);
        }
    }
    if ((DAT_00475e78 >= g_nTargetRange_0059ce10 ||
         RandomBelowOrEqual(100) <= 9) && normal_speed(ship) != 0) {
        fire_afterburner(ship, 10);
        return;
    }
    approach_full_speed(ship);
}

/* Function start: 0x407560 */
void ShipAiState44(short ship)
{
    g_acShipStress_0059d620[ship] = 0;
    SelectNewShipAiBehavior(ship);
}

/* Function start: 0x407580 */
void ShipAiState27(short ship, short target)
{
    Mbest_strafe(ship, target);
}

/* Function start: 0x4075A0 */
void ShipAiState21(short ship)
{
    try2rout(ship);
}

/* Function start: 0x4075B0 */
/* Empty in the original: dispatch-table slots 0 and 1 (no-op / invalid state). */
void ShipAiStateNoOp(void)
{
}

/* Function start: 0x4075C0 */
void ShipAiState03(short ship)
{
    SelectNewShipAiBehavior(ship);
}

/* Function start: 0x4075D0 */
void perform_maneuver(short obj)
{
    short target = (short)g_acShipTarget_0059ce60[obj];
    enum ShipManeuver previous = g_aeShipManeuver_0059dcb0[obj];
    short range;
    int maneuverWeight;

    g_bCurrentManeuverReroll_00475e7c =
        g_abManeuverRerollChance_00465678[previous];
    ship_vs_ship(obj, target);
    range = g_nTargetRange_0059ce10;
    if (g_nTargetFacing_0059d52a < 0)
        maneuverWeight = g_asObjectCollisionRadius_0059d710[target] +
                         g_asObjectCollisionRadius_0059d710[obj] * 4;
    else
        maneuverWeight = g_asObjectCollisionRadius_0059d710[target] +
                         g_asObjectCollisionRadius_0059d710[obj] * 6;
    SetShipAiScratchWord((unsigned short)(maneuverWeight >> 1));

    if (unactive(target) != 0) {
        if (g_aeShipManeuver_0059dcb0[obj] == MANEUVER_VEER_AWAY) {
            ShipAiState02(obj, target);
        } else if (g_aeShipManeuver_0059dcb0[obj] == MANEUVER_GLOAT) {
            ((void (__cdecl *)(short, short))ShipAiState25)(obj, target);
        } else if (g_aeShipManeuver_0059dcb0[obj] ==
                   MANEUVER_LINE_UP_DROP) {
            ShipAiState42(obj, target);
        } else {
            SelectNewShipAiBehavior(obj);
        }
    } else if ((int)g_aeShipManeuver_0059dcb0[obj] >= 0 &&
               g_aeShipManeuver_0059dcb0[obj] < 47) {
        g_apShipAiManeuverHandlers_004656a8[
            g_aeShipManeuver_0059dcb0[obj]](obj, target);
    } else {
        SelectNewShipAiBehavior(obj);
    }

    if (range < (short)DAT_00475e78) {
        try2reset_maneuver(obj, 2);
    } else if (g_aeShipManeuver_0059dcb0[obj] == previous &&
               RandomBelowOrEqual(100) <
                   (short)g_bCurrentManeuverReroll_00475e7c) {
        SelectNewShipAiBehavior(obj);
    }
}

/* Function start: 0x407710 */
short __stdcall GetShapeFrameExtent(short x, short y,
                                    unsigned char *shape, short frame,
                                    short extent)
{
    short bounds[4];

    GetShapeFrameBounds(bounds, x, y, shape, frame);
    return bounds[extent];
}

/* Function start: 0x409760 */
void cruise_home(short obj)
{
    FixedVector *destination;
    short range;
    short objective;

    if (abandoned(obj, 0) != 0 || (g_abShipTurn_0059d860[obj] & 7) != 5)
        return;

    if (obj == g_nYourWingman_0046c04c &&
        distance_from_object(obj, 0) > 16000)
        remove_object(obj);

    if (g_aeObjectClass_0059d100[obj] == OBJECT_CLASS_CAPITAL_SHIP)
        approach_cruise_speed(obj);
    else if (normal_speed(obj) != 0)
        fire_afterburner(obj, 10);

    destination = &g_aShipDestination_0059d530[obj];
    if (no_goal(obj) != 0)
        point_ship_at_point(obj, destination);
    range = distance_from_point(obj, destination);

    if (equ_vector(destination,
                   &((FixedVector *)g_aShipMissionSpot_0059dd10)[obj])) {
        if (range < 5000) {
            reset_tactic(obj, TACTIC_HEAD_HOME);
            set_special(obj, SPECIAL_MANEUVER_KILL_ENGINES);
            zero_vector(&g_aShipVelocity_0059c010[obj]);
        }
        return;
    }

    if (range < 1500) {
        objective = (short)g_abFlightPath_0059c000[
            g_abShipNavPointIndex_0059d7c0[obj]];
        if (g_aMissionObjectives_0059dac5[objective].type != 1)
            flag_objective(objective, 1);
        get_follow_point(obj, destination);
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

    switch (g_aeShipTactic_0059d5e0[obj]) {
    case TACTIC_NONE:
        reset_tactic(obj, TACTIC_CRUISE);
        if (g_aeShipSide_0059d650[obj] == SIDE_IMPERIAL &&
            g_aeShipMissionType_0059c3f0[obj] == MISSION_TYPE_ROUT) {
            objective = find_objective(1, -1);
            g_aShipDestination_0059d530[obj] =
                g_aMissionObjectives_0059dac5[objective].position;
        } else {
            get_first_follow_point(obj, &g_aShipDestination_0059d530[obj]);
        }
        break;
    case TACTIC_CRUISE:
        cruise_home(obj);
        break;
    case TACTIC_HEAD_HOME:
        if (no_goal(obj) != 0)
            point_parallel(obj, find_ship_index(DAT_005a8692));
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

    if (unactive(g_asShipWingLeader_0059d400[obj]) == 0 &&
        g_aeShipMissionType_0059c3f0[
            g_asShipWingLeader_0059d400[obj]] == MISSION_TYPE_ROUT) {
        maintain_formation(obj);
        return;
    }
    if (g_aeShipSide_0059d650[obj] == SIDE_IMPERIAL) {
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

    if (newTarget != -1 && newTarget != g_acShipTarget_0059ce60[obj])
        g_acShipTarget_0059ce60[obj] = (signed char)newTarget;
    else if (target_valid(obj) == 0)
        select_target(obj);
    return (short)g_acShipTarget_0059ce60[obj];
}

/* Function start: 0x409B10 */
short check_destroy_target(short obj)
{
    short destroyTarget = find_ship_index(g_anShipMissionShip_0059d4b0[obj]);
    int determination;

    if (destroyTarget == -1) {
        g_acShipTarget_0059ce60[obj] = (signed char)check_engage_target(obj);
    } else if (g_aeObjectClass_0059d100[destroyTarget] ==
                   OBJECT_CLASS_FUTURION ||
               gone_ship(g_anShipMissionShip_0059d4b0[obj]) != 0) {
        check_engage_target(obj);
    } else {
        determination = 70;
        determination -= MaxShort(
            0, MinShort(4, (short)g_aiPilotLevel_0059cf30[obj])) * 15;
        if (evaluate_damage(obj) > determination) {
            g_acShipTarget_0059ce60[obj] = (signed char)destroyTarget;
            if (g_aeShipSide_0059d650[destroyTarget] ==
                g_aeShipSide_0059d650[obj])
                g_acShipTarget_0059ce60[obj] = -1;
        } else if (target_valid(obj) != 0 && RandomBelow(100) > 3) {
            check_engage_target(obj);
        } else {
            g_acShipTarget_0059ce60[obj] = (signed char)destroyTarget;
        }
    }
    return (short)g_acShipTarget_0059ce60[obj];
}

/* Function start: 0x409C20 */
void maneuvering(short obj, short newTarget)
{
    g_acShipTarget_0059ce60[obj] = (signed char)newTarget;
    intelligence_events(obj);
    perform_maneuver(obj);
}

/* Function start: 0x409C50 */
void formation_burst(short obj)
{
    approach_full_speed(obj);
    if (no_goal(obj) != 0)
        point_ship(obj, 0, &g_aShipDestination_0059d530[obj]);
    g_asShipCount_0059c420[obj]++;
    if (g_asShipCount_0059c420[obj] > 9) {
        if (g_aeShipMissionType_0059c3f0[obj] == MISSION_TYPE_STRIKE)
            engage(obj, (short)g_acShipTarget_0059ce60[obj],
                   OBJECTIVE_DESTROY_SHIP);
        else
            engage(obj, (short)g_acShipTarget_0059ce60[obj],
                   OBJECTIVE_ENGAGE_ENEMY);
    }
}

/* Function start: 0x409CE0 */
void disallow_engage(void)
{
    g_bEngageAllowed_0046c080 = 0;
}

/* Function start: 0x409CF0 */
void allow_engage(void)
{
    g_bEngageAllowed_0046c080 = 1;
    g_nAutoEngageTimer_0046c084 = -1;
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
    g_nAutoEngageTimer_0046c084 = -40;
}

/* Function start: 0x409D60 */
void imperial_formation(short obj)
{
    short leader = g_asShipWingLeader_0059d400[obj];

    if (leader == -1)
        leader = obj;
    maintain_formation(obj);
    if (attacker_in_range(leader, 12000) != 0) {
        if (obj == g_nYourWingman_0046c04c ||
            g_nYourWingman_0046c04c == -1) {
            if (g_nAutoEngageTimer_0046c084 < -1)
                g_nAutoEngageTimer_0046c084++;
            else if (g_nAutoEngageTimer_0046c084 != -1 &&
                     --g_nAutoEngageTimer_0046c084 == 0)
                try2allow_engage(g_aiPilotLevel_0059cf30[obj]);
        }
        if (g_bEngageAllowed_0046c080 != 0) {
            engage(obj, g_nTargetShip_0059c3b0,
                   OBJECTIVE_ENGAGE_ENEMY);
        } else if (obj == g_nYourWingman_0046c04c &&
                   g_nAutoEngageTimer_0046c084 == -1) {
            send_message(obj, 3);
            g_nAutoEngageTimer_0046c084 = 40;
        }
    } else if (obj == g_nYourWingman_0046c04c &&
               g_nEnemySighting_00465c7c != g_nCurrentWave_0046c01c &&
               any_enemy(obj, 16000) != 0 &&
               IsAutopilotEngaged() == 0 && DAT_0046c03c == 0) {
        send_message(obj, 2);
        g_nEnemySighting_00465c7c = g_nCurrentWave_0046c01c;
    }

    if (g_aeSpecialManeuver_0059c3c0[obj] == SPECIAL_MANEUVER_NONE &&
        distance_from_object(obj, leader) > 9000) {
        if (facing_to_object(obj, leader) > 85 && real_velocity(obj) < 110) {
            fire_afterburner(obj, 10);
            return;
        }
        point_ship_at_object(obj, leader);
        approach_ship_speed(obj, leader);
    }
}

/* Function start: 0x409F00 */
void formation_break(short obj)
{
    switch (g_acShipSequence_0059d520[obj]) {
    case 0:
        steady_object(obj);
        g_anYawGoal_0059c310[obj] = -30;
        g_anRollGoal_0059d630[obj] = -45;
        g_anPitchGoal_0059d7a0[obj] = -20;
        g_acShipSequence_0059d520[obj]++;
        break;
    case 1:
        if (no_goal(obj) != 0)
            engage(obj, (short)g_acShipTarget_0059ce60[obj],
                   OBJECTIVE_ENGAGE_ENEMY);
        break;
    default:
        g_acShipSequence_0059d520[obj] = 0;
        break;
    }
}

/* Function start: 0x409F80 */
void imperial_wingman(short obj)
{
    switch (g_aeShipObjective_0059d200[obj]) {
    case OBJECTIVE_DESTROY_SHIP:
        maneuvering(obj, (short)check_destroy_target(obj));
        break;
    case OBJECTIVE_ENGAGE_ENEMY:
        maneuvering(obj, (short)check_engage_target(obj));
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
    short leader = g_asShipWingLeader_0059d400[obj];
    enum ShipObjective objective;

    if (leader == -1) {
        change_mission_type(obj, MISSION_TYPE_PATROL);
        return;
    }
    if (unactive(leader) != 0) {
        inherit_leader(obj);
        return;
    }
    objective = g_aeShipObjective_0059d200[leader];
    if ((objective == OBJECTIVE_ENGAGE_ENEMY ||
         objective == OBJECTIVE_DESTROY_SHIP) &&
        g_aeShipObjective_0059d200[obj] != objective)
        engage(obj, (short)g_acShipTarget_0059ce60[obj], objective);

    switch (g_aeShipObjective_0059d200[obj]) {
    case OBJECTIVE_DESTROY_SHIP:
    case OBJECTIVE_ENGAGE_ENEMY:
        maneuvering(obj, (short)check_engage_target(obj));
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
    if (g_aeShipSide_0059d650[obj] == SIDE_IMPERIAL) {
        imperial_wingman(obj);
        return;
    }
    kilrathi_wingman(obj);
}

/* Function start: 0x40A160 */
short dist_from_home(short obj)
{
    return distance_from_point(
        obj, (FixedVector *)&g_aShipMissionSpot_0059dd10[
                 (int)obj * sizeof(FixedVector)]);
}

/* Function start: 0x40A180 */
short scan_and_lock(short obj, int scanRange, enum ShipTactic newTactic)
{
    g_acShipTarget_0059ce60[obj] = (signed char)scan_for_enemy(obj, 14000);
    if (g_acShipTarget_0059ce60[obj] != -1)
        g_aeShipTactic_0059d5e0[obj] = newTactic;
    return g_acShipTarget_0059ce60[obj] != -1;
}

/* Function start: 0x40A1C0 */
void patrol_area(short obj)
{
    short target = (short)g_acShipTarget_0059ce60[obj];

    switch (g_aeShipTactic_0059d5e0[obj]) {
    case TACTIC_HEAD_HOME:
        approach_cruise_speed(obj);
        if (scan_and_lock(obj, 14000, TACTIC_APPROACH_TARGET) == 0) {
            ship_vs_point(obj,
                &((FixedVector *)g_aShipMissionSpot_0059dd10)[obj]);
            if (g_nTargetRange_0059ce10 < 3000) {
                reset_tactic(obj, TACTIC_LOOK_OUT);
                return;
            }
            point_ship_at_point(obj,
                &((FixedVector *)g_aShipMissionSpot_0059dd10)[obj]);
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
            if (g_nTargetRange_0059ce10 < 10000) {
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
    switch (g_aeShipObjective_0059d200[obj]) {
    case OBJECTIVE_WANDER:
    case OBJECTIVE_HOLD_FORMATION:
        patrol_area(obj);
        break;
    case OBJECTIVE_ENGAGE_ENEMY:
        maneuvering(obj, (short)check_engage_target(obj));
        break;
    case OBJECTIVE_BREAK_FORMATION:
        formation_burst(obj);
        break;
    case OBJECTIVE_NONE:
        g_aeShipObjective_0059d200[obj] = OBJECTIVE_WANDER;
        g_aeShipTactic_0059d5e0[obj] = TACTIC_APPROACH_TARGET;
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
    FixedVector *destination;
    short range;

    if (abandoned(obj, 0) != 0)
        return;
    if ((g_abShipTurn_0059d860[obj] & 7) == 6)
        g_acShipTarget_0059ce60[obj] =
            (signed char)scan_for_enemy(obj, 15000);

    if (g_acShipTarget_0059ce60[obj] == -1) {
        approach_cruise_speed(obj);
    } else {
        get_facing_range_from_object(obj,
            (short)g_acShipTarget_0059ce60[obj]);
        if (g_nFacingToTarget_0059d920 <= 65)
            approach_full_speed(obj);
        else
            approach_half_speed(obj);
    }

    if ((g_abShipTurn_0059d860[obj] & 7) != 2)
        return;
    destination = &g_aShipDestination_0059d530[obj];
    if (no_goal(obj) != 0)
        point_ship_at_point(obj, destination);
    range = distance_from_point(obj, destination);
    if (range < 1500) {
        if (g_aeShipSide_0059d650[obj] == SIDE_IMPERIAL)
            flag_reached((short)g_abFlightPath_0059c000[
                g_abShipNavPointIndex_0059d7c0[obj]], 1);
        if (equ_vector(destination,
                       &((FixedVector *)g_aShipMissionSpot_0059dd10)[obj])) {
            reset_tactic(obj, TACTIC_SIT_STILL);
            set_special(obj, SPECIAL_MANEUVER_KILL_ENGINES);
        } else {
            get_follow_point(obj, destination);
        }
    }
}

/* Function start: 0x40A540 */
void prepare_for_jump(short obj)
{
    short count;
    short delay;

    if (g_anShipSpeed_0059b320[obj] != 0) {
        set_special(obj, SPECIAL_MANEUVER_STOP_DRIFT);
        return;
    }
    count = ++g_asShipCount_0059c420[obj];
    delay = g_aeShipSide_0059d650[obj] == SIDE_KILRATHI ? 250 : 25;
    if (count <= delay)
        return;

    get_facing_range_from_object(obj, 0);
    if (g_nFacingToTarget_0059d920 > 90 && no_goal(obj) != 0) {
        g_anYawGoal_0059c310[obj] = signed_random(30);
        return;
    }
    delay = g_aeShipSide_0059d650[obj] == SIDE_KILRATHI ? 270 : 45;
    if (g_asShipCount_0059c420[obj] > delay ||
        (g_nTargetFacing_0059d52a > 80 && g_nTargetRange_0059ce10 < 6000)) {
        reset_tactic(obj, TACTIC_WARP_OUT);
        fire_afterburner(obj, 10);
    }
}

/* Function start: 0x40A630 */
void accelerate_and_jump(short obj)
{
    approach_full_speed(obj);
    if (g_asShipCount_0059c420[obj]++ == 4)
        warp(obj);
}

/* Function start: 0x40A670 */
void reach_warp(short obj)
{
    switch (g_aeShipTactic_0059d5e0[obj]) {
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
        get_first_follow_point(obj, &g_aShipDestination_0059d530[obj]);
        break;
    default:
        fail(obj);
        break;
    }
}

/* Function start: 0x40A710 */
void warp_arrival(short obj)
{
    if (g_aeShipTactic_0059d5e0[obj] == TACTIC_WARP_IN)
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
    short buddy = find_ship_index(g_anShipMissionShip_0059d4b0[obj]);

    if (unactive(buddy) != 0) {
        change_mission_type(obj, MISSION_TYPE_PATROL);
        return;
    }
    if ((g_abShipTurn_0059d860[obj] & 3) == 0 && in_danger(buddy) != 0 &&
        g_nTargetRange_0059ce10 < 3000)
        engage(obj, g_nTargetShip_0059c3b0,
               OBJECTIVE_ENGAGE_ENEMY);
    if (g_aeShipObjective_0059d200[obj] != OBJECTIVE_HOME_BASE &&
        (g_abShipTurn_0059d860[obj] & 7) == 4 &&
        distance_from_object(obj, buddy) > 5000)
        reset_objective(obj, OBJECTIVE_HOME_BASE);

    switch (g_aeShipObjective_0059d200[obj]) {
    case OBJECTIVE_HOME_BASE:
        return_to_buddy(obj, buddy);
        break;
    case OBJECTIVE_WANDER:
        escort_buddy(obj, buddy);
        break;
    case OBJECTIVE_ENGAGE_ENEMY:
        maneuvering(obj, (short)check_engage_target(obj));
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
    if (gone_ship(g_anShipMissionShip_0059d4b0[obj]) != 0)
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
    int determination;

    if (g_aeObjectClass_0059d100[goal] != OBJECT_CLASS_FUTURION) {
        determination = 70;
        determination -= MaxShort(
            0, MinShort(4, (short)g_aiPilotLevel_0059cf30[obj])) * 15;
        if (evaluate_damage(obj) > determination && range > 5000) {
            streak_toward(obj, goal, (short)range);
            return;
        }
    }
    possibleTarget = scan_for_enemy(obj, 10000);
    possibleRange = (unsigned short)g_nTargetRange_0059ce10;
    if (possibleTarget != -1 &&
        (possibleRange * 3 < range ||
         g_aeObjectClass_0059d100[goal] == OBJECT_CLASS_FUTURION)) {
        init_formation_burst(obj);
        g_acShipTarget_0059ce60[obj] = (signed char)possibleTarget;
    } else if (range < 5000) {
        engage(obj, goal, OBJECTIVE_DESTROY_SHIP);
    } else {
        streak_toward(obj, goal, (short)range);
    }
}

/* Function start: 0x40AAC0 */
void strike_mission(short obj)
{
    short goal = find_ship_index(g_anShipMissionShip_0059d4b0[obj]);

    if (goal == -1 &&
        g_aeObjectClass_0059d100[-1] != OBJECT_CLASS_FUTURION)
        check_goal(obj);
    switch (g_aeShipObjective_0059d200[obj]) {
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
    short master = find_ship_index(g_anShipMissionShip_0059d4b0[obj]);
    short target;

    if (master == -1) {
        change_mission_type(obj, MISSION_TYPE_PATROL);
        return;
    }
    if ((signed char)g_abShipTurn_0059d860[obj] % 10 == 0 &&
        in_danger(master) != 0 && g_nTargetRange_0059ce10 < 6000 &&
        g_aeShipObjective_0059d200[obj] != OBJECTIVE_ENGAGE_ENEMY)
        engage(obj, g_nTargetShip_0059c3b0,
               OBJECTIVE_ENGAGE_ENEMY);
    if (g_aeShipObjective_0059d200[obj] != OBJECTIVE_HOME_BASE &&
        (g_abShipTurn_0059d860[obj] & 7) == 4 &&
        distance_from_object(obj, master) > 10000)
        reset_objective(obj, OBJECTIVE_HOME_BASE);

    switch (g_aeShipObjective_0059d200[obj]) {
    case OBJECTIVE_HOME_BASE:
        return_to_master(obj, master);
        break;
    case OBJECTIVE_WANDER:
        target = scan_for_enemy(obj, 7000);
        g_acShipTarget_0059ce60[obj] = (signed char)target;
        if (target != -1) {
            engage(obj, target, OBJECTIVE_ENGAGE_ENEMY);
        } else {
            approach_half_speed(obj);
            if (no_goal(obj) != 0)
                point_perpendicular(obj, master);
        }
        break;
    case OBJECTIVE_ENGAGE_ENEMY:
        maneuvering(obj, (short)check_engage_target(obj));
        break;
    case OBJECTIVE_NONE:
        reset_objective(obj, OBJECTIVE_WANDER);
        break;
    default:
        fail(obj);
        break;
    }
}

/* Function start: 0x40AD80 */
void rendezvous_mission(short obj)
{
    short goal = find_ship_index(g_anShipMissionShip_0059d4b0[obj]);

    if (unactive(goal) != 0) {
        change_mission_type(obj, MISSION_TYPE_PATROL);
        return;
    }
    switch (g_aeShipObjective_0059d200[obj]) {
    case OBJECTIVE_REACH_SHIP:
        if (attacker_in_range(obj, 3500) != 0)
            engage(obj, g_nTargetShip_0059c3b0,
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
    if (regulate_turn(obj) != 0)
        return;
    switch (g_aeShipMissionType_0059c3f0[obj]) {
    case MISSION_TYPE_PATROL:
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
    if (((signed char *)g_aeShipSide_0059d650)[obj + 0x30] > 0)
        ((signed char *)g_aeShipSide_0059d650)[obj + 0x30]--;
}

/* Function start: 0x40AF70 */
void orbit_sphere(short obj)
{
    short radius = g_aMissionNavPoints_0046c2f0[
        g_nCurrentNavPoint_0059df60].proximityRadius >> 1;
    FixedVector center =
        g_aMissionNavPoints_0046c2f0[g_nCurrentNavPoint_0059df60].position;
    short range = distance_from_point(obj, &center);

    if (no_goal(obj) != 0 && range > radius - 750) {
        if (range > radius)
            point_ship_at_point(obj, &center);
        else
            point_perpendicular_to_point(obj, &center);
    }
    trim_goals(obj, 10);
}

/* Function start: 0x40B010 */
void tanker_intelligence(short obj)
{
    if (attacker_in_range(obj, 3000) != 0) {
        approach_full_speed(obj);
        g_acShipTarget_0059ce60[obj] =
            (signed char)g_nTargetShip_0059c3b0;
        fire_capital_weapon(obj, g_nTargetShip_0059c3b0);
        if (no_goal(obj) != 0) {
            if (RandomBelowOrEqual(4) == 0) {
                g_anYawGoal_0059c310[obj] = signed_random(90);
                g_anRollGoal_0059d630[obj] = signed_random(90);
            } else {
                point_capital_ship_at_object(obj,
                                             g_nTargetShip_0059c3b0);
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
        g_acShipTarget_0059ce60[obj] = -1;
        approach_half_speed(obj);
    } else {
        approach_cruise_speed(obj);
    }
    orbit_sphere(obj);
}

/* Function start: 0x40B110 */
void stationary_intelligence(short obj)
{
    if (g_aeObjectType_0059b560[obj] == OBJECT_TYPE_KILRATHI_BASE) {
        g_anObjectYawRotation_0059ce80[obj] = 4;
        fire_turrets(obj);
    }
}

/* Function start: 0x40B140 */
void capital_ship_intelligence(short obj)
{
    enum ObjectType type;

    if (regulate_turn(obj) != 0)
        return;
    switch (g_aeShipMissionType_0059c3f0[obj]) {
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

    type = g_aeObjectType_0059b560[obj];
    if (type == OBJECT_TYPE_DORKIR || type == OBJECT_TYPE_LUMBARI) {
        tanker_intelligence(obj);
        return;
    }
    if (type == OBJECT_TYPE_SPIKERI || type == OBJECT_TYPE_RALARI ||
        type == OBJECT_TYPE_FRALTHI || type == OBJECT_TYPE_SNAKEIR ||
        type == OBJECT_TYPE_SIVAR || type == OBJECT_TYPE_KILRATHI_BASE) {
        destroyer_intelligence(obj);
        return;
    }

    g_nTargetShip_0059c3b0 = (short)g_acShipTarget_0059ce60[obj];
    if (unactive(g_nTargetShip_0059c3b0) != 0)
        scan_for_enemy(obj, 15000);
    if (g_aeShipTactic_0059d5e0[obj] != TACTIC_SELF_DEFENSE) {
        if (g_nTargetShip_0059c3b0 != -1) {
            approach_full_speed(obj);
            g_aeShipTactic_0059d5e0[obj] = TACTIC_SELF_DEFENSE;
            g_acShipTarget_0059ce60[obj] =
                (signed char)g_nTargetShip_0059c3b0;
            fire_turrets(obj);
        } else {
            approach_cruise_speed(obj);
        }
        return;
    }

    approach_full_speed(obj);
    if (unactive((short)g_acShipTarget_0059ce60[obj]) != 0) {
        select_target(obj);
        if (unactive((short)g_acShipTarget_0059ce60[obj]) != 0)
            reset_tactic(obj, TACTIC_NONE);
    } else {
        fire_turrets(obj);
    }
}

/* Function start: 0x40B320 */
void futurion_intelligence(short obj)
{
    short range;
    short count;

    ship_vs_ship(0, obj);
    range = g_nTargetRange_0059ce10;
    count = ++g_asActionCount_0059c930[obj];
    if (range > 1000 && count > 1000) {
        g_aeObjectClass_0059d100[obj] =
            (enum ObjectClass)g_asObjectCounter_0059c330[obj];
        return;
    }
    if (count > 200 && range < 4000 && range > 1000 &&
        g_nFacingToTarget_0059d920 > 80)
        g_aeObjectClass_0059d100[obj] =
            (enum ObjectClass)g_asObjectCounter_0059c330[obj];
}

/* Function start: 0x40B3A0 */
void mine_intelligence(short obj)
{
    short other;
    short distance;

    if (g_asObjectCounter_0059c330[obj] != -1)
        return;
    for (other = 0; other < 10; other++) {
        if (other == obj || g_aeObjectClass_0059d100[other] <
                            OBJECT_CLASS_SHIP)
            continue;
        distance = distance_from_object(obj, other);
        if (distance < g_aObjectTypeData_0046645c[
                           g_aeObjectType_0059b560[obj]].collisionRadius ||
            (distance < 50 && RandomBelowOrEqual(7) == 0)) {
            explode(obj, obj);
            return;
        }
    }
}

/* Function start: 0x40B430 */
void heat_seeking_missile_intelligence(short obj)
{
    short other;
    short heat;
    short candidate;
    short range;
    short facing;
    short targetFacing;
    signed char viableIndex;
    signed char targetCount;
    signed char *target;

    if (g_nFacingToTarget_0059d920 >= 0 &&
        g_acShipTarget_0059ce60[obj] != -1) {
        point_ship(obj, 0, &g_vToTarget_0059d4d0);
        g_anShipSpeed_0059b320[obj] =
            (get_ship_max_velocity(obj) + 10) << 8;
        return;
    }

    target = &g_acShipTarget_0059ce60[obj];
    other = 0;
    g_cViableTargetCount_0046c088 = 0;
    *target = -1;
    do {
        if (other == obj ||
            g_aeObjectClass_0059d100[other] < OBJECT_CLASS_SHIP) {
            other++;
            continue;
        }
        get_facing_range_from_object(obj, other);
        range = g_nTargetRange_0059ce10;
        facing = g_nFacingToTarget_0059d920;
        viableIndex = g_cViableTargetCount_0046c088;
        targetFacing = g_nTargetFacing_0059d52a;
        if (range < 9000 && facing > 0 && targetFacing < 0) {
            g_asViableTargetDistance_0059c470[(short)viableIndex] = range;
            g_acViableTarget_0059c920[(short)viableIndex] =
                (signed char)other;
            g_cViableTargetCount_0046c088 = ++viableIndex;
        }
        other++;
    } while (other <= 9);
    sort_viable_target_list();
    targetCount = g_cViableTargetCount_0046c088;
    if (targetCount > 0) {
        for (heat = 3; heat > 0; heat--) {
            for (candidate = 0; candidate < targetCount; candidate++) {
                if (g_aeObjectClass_0059d100[
                        (short)g_acViableTarget_0059c920[candidate]] ==
                        OBJECT_CLASS_CAPITAL_SHIP ||
                    g_abShipExhaustHeat_0059d610[
                        (short)g_acViableTarget_0059c920[candidate]] == heat) {
                    *target = g_acViableTarget_0059c920[candidate];
                    heat = 0;
                    break;
                }
            }
        }
    }
    if (*target == -1)
        explode(obj, obj);
}

/* Function start: 0x40B570 */
void FF_missile_intelligence(short obj)
{
    short other;
    short candidate;

    if (g_aeShipTactic_0059d5e0[obj] == TACTIC_RAM) {
        if (g_acShipTarget_0059ce60[obj] == -1) {
            g_cViableTargetCount_0046c088 = 0;
            for (other = 0; other < 10; other++) {
                if (other == obj ||
                    g_aeObjectClass_0059d100[other] < OBJECT_CLASS_SHIP)
                    continue;
                if (g_aeShipSide_0059d650[
                        g_acObjectOwner_0059ce20[obj]] ==
                        g_aeShipSide_0059d650[other] &&
                    g_acShipCommunicator_0059c850[other] != -1)
                    continue;
                g_nTargetRange_0059ce10 = distance_from_object(obj, other);
                if (g_nTargetRange_0059ce10 < 9000) {
                    candidate = g_cViableTargetCount_0046c088++;
                    g_asViableTargetDistance_0059c470[candidate] =
                        g_nTargetRange_0059ce10;
                    g_acViableTarget_0059c920[candidate] =
                        (signed char)other;
                }
            }
            sort_viable_target_list();
            if (g_cViableTargetCount_0046c088 > 0)
                g_acShipTarget_0059ce60[obj] =
                    g_acViableTarget_0059c920[0];
        } else {
            point_ship(obj, 0, &g_vToTarget_0059d4d0);
            g_anShipSpeed_0059b320[obj] =
                (get_ship_max_velocity(obj) + 10) << 8;
        }
    }
}

/* Function start: 0x40B700 */
unsigned int GetShipSlotState(short i)
{
    unsigned int prev;

    if (g_aeShipMissionType_0059c3f0[i] == MISSION_TYPE_WARP_ARRIVE) {
        prev = g_aeObjectClass_0059d100[i];
        g_aeObjectClass_0059d100[i] = OBJECT_CLASS_FUTURION;
        g_asObjectCounter_0059c330[i] = (short)prev;
    }
    return 0;
}

/* Function start: 0x40B990 */
void release_capital_ship_shapes(enum ObjectType type)
{
    short obj;

    if (g_aObjectTypeData_0046645c[type].objectClass !=
        OBJECT_CLASS_CAPITAL_SHIP)
        return;
    obj = 1;
    do {
        if (g_aeObjectType_0059b560[obj] == type)
            g_apObjectShape_0059d2f0[obj] = 0;
        obj++;
    } while (obj < 10);
}

/* Function start: 0x40B9F0 */
void load_object_resources(enum ObjectType type, short slot)
{
    ObjectResourceSlot *resource;
    ObjectTypeData *typeData;
    short logicalFile;
    short asteroidType;

    if ((int)type < 0 || type >= OBJECT_TYPE_COUNT ||
        slot < 0 || slot >= 4)
        return;
    resource = &g_aObjectResourceSlots_0059ddf0[slot];
    if (resource->shapeSet != 0)
        return;

    resource->type = (signed char)type;
    if (type == OBJECT_TYPE_ASTEROID_FIELD) {
        resource->animation =
            (unsigned char *)FetchDiskPacketRetrying(3, 13, 0);
        resource->shapeSet =
            (unsigned char *)FetchDiskPacketRetrying(3, 16, 0);
        resource->shape = 0;
        asteroidType = OBJECT_TYPE_ASTEROID1;
        do {
            typeData = &g_aObjectTypeData_0046645c[asteroidType];
            typeData->objectClass = OBJECT_CLASS_ASTEROID;
            typeData->collisionRadius = 100;
            typeData->scale = 640;
            typeData->shapeSet = resource->shapeSet;
            asteroidType++;
        } while (asteroidType <= OBJECT_TYPE_ASTEROID6);
        return;
    }
    if (type == OBJECT_TYPE_MINE_FIELD)
        return;

    typeData = &g_aObjectTypeData_0046645c[type];
    logicalFile = (short)type + 22;
    resource->shapeSet =
        (unsigned char *)FetchDiskPacketRetrying(logicalFile, 0, 0);
    resource->animation =
        (unsigned char *)FetchDiskPacketRetrying(logicalFile, 2, 0);
    resource->shape =
        (unsigned char *)FetchDiskPacketRetrying(logicalFile, 1, 0);
    typeData->shapeSet = resource->shapeSet;
    typeData->animation = resource->animation;
    typeData->shape = resource->shape;

    for (asteroidType = 0; asteroidType < WC1_SPACE_OBJECT_COUNT;
         asteroidType++) {
        if (g_aeObjectType_0059b560[asteroidType] == type &&
            g_aeObjectClass_0059d100[asteroidType] >= OBJECT_CLASS_SHIP)
            g_apObjectShape_0059d2f0[asteroidType] = resource->shapeSet;
    }
}

/* Function start: 0x40BC70 */
void release_object_resources(short slot)
{
    ObjectResourceSlot *resource;
    ObjectTypeData *typeData;
    enum ObjectType type;
    short asteroidType;

    if (slot < 0 || slot >= 4)
        return;
    resource = &g_aObjectResourceSlots_0059ddf0[slot];
    type = (enum ObjectType)resource->type;
    if ((int)type < 0 || type >= OBJECT_TYPE_COUNT)
        return;

    if (type == OBJECT_TYPE_ASTEROID_FIELD) {
        FreePacketAndClear((int *)&resource->animation, 0);
        FreePacketAndClear((int *)&resource->shapeSet, 0);
        asteroidType = OBJECT_TYPE_ASTEROID1;
        do {
            g_aObjectTypeData_0046645c[asteroidType].shapeSet = 0;
            asteroidType++;
        } while (asteroidType <= OBJECT_TYPE_ASTEROID6);
    } else if (type != OBJECT_TYPE_MINE_FIELD) {
        typeData = &g_aObjectTypeData_0046645c[type];
        release_capital_ship_shapes(type);
        FreePacketAndClear((int *)&resource->shapeSet, 0);
        FreePacketAndClear((int *)&resource->animation, 0);
        FreePacketAndClear((int *)&resource->shape, 0);
        typeData->shapeSet = 0;
        typeData->animation = 0;
        typeData->shape = 0;
    }
    resource->shapeSet = 0;
    resource->animation = 0;
    resource->shape = 0;
    resource->type = -1;
}

/* Function start: 0x40BE20 */
void free_nav_object_resources(void)
{
    short slot = 0;

    do {
        if (g_aObjectResourceSlots_0059ddf0[slot].type != -1)
            release_object_resources(slot);
        slot++;
    } while (slot < 3);
    initialize_view_buffer();
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
short find_free_object_resource_slot(void)
{
    short slot = 0;

    do {
        if (g_aObjectResourceSlots_0059ddf0[slot].type == -1)
            return slot;
        slot++;
    } while (slot < 4);
    return -1;
}

/* Function start: 0x40BEF0 */
int object_resources_loaded(enum ObjectType type)
{
    short slot = 0;

    do {
        if (g_aObjectResourceSlots_0059ddf0[slot].type == (signed char)type)
            return 1;
        slot++;
    } while (slot < 4);
    return 0;
}

/* Function start: 0x40BF20 */
int nav_point_uses_object_type(const MissionNavPoint *navPoint,
                               enum ObjectType type)
{
    short preload;

    if ((int)type < 0)
        return 0;
    preload = 0;
    do {
        if (navPoint->preloadObjectTypes[preload] == type)
            return 1;
        preload++;
    } while (preload < 2);
    return 0;
}

/* Function start: 0x40BF50 */
void cache_nav_point_resources(MissionNavPoint *navPoint)
{
    short slot;
    short preload;
    enum ObjectType type;

    slot = 1;
    do {
        type = (enum ObjectType)g_aObjectResourceSlots_0059ddf0[slot].type;
        if ((int)type >= 0 &&
            !nav_point_uses_object_type(navPoint, type))
            release_object_resources(slot);
        slot++;
    } while (slot < 3);

    preload = 0;
    do {
        type = navPoint->preloadObjectTypes[preload];
        if ((int)type >= 0 && !object_resources_loaded(type)) {
            slot = find_free_object_resource_slot();
            if (slot != -1)
                load_object_resources(type, slot);
        }
        preload++;
    } while (preload < 2);
    initialize_view_buffer();
}

/* Function start: 0x40BFF0 */
void EnterNavPoint(short navPoint)
{
    MissionNavPoint *nav;
    short obj;
    short entry;

    if (navPoint < 0 || navPoint >= WC1_MISSION_NAV_POINT_COUNT)
        return;
    g_nCurrentNavPoint_0059df60 = navPoint;
    nav = &g_aMissionNavPoints_0046c2f0[navPoint];

    obj = 1;
    do {
        if (g_aeObjectClass_0059d100[obj] != OBJECT_CLASS_NULL)
            remove_object(obj);
        obj++;
    } while (obj < 10);
    remove_all_hazards();
    cache_nav_point_resources(nav);

    entry = 0;
    do {
        if (nav->missionShips[entry] != -1)
            spawn_mission_ship(nav->missionShips[entry], navPoint);
        entry++;
    } while (entry < 10);

    entry = 0;
    do {
        if (nav->triggers[entry][0] != -1) {
            signed char target = nav->triggers[entry][1];

            if (target >= 0 && target < WC1_MISSION_NAV_POINT_COUNT)
                g_aMissionNavPoints_0046c2f0[target].type =
                    nav->triggers[entry][0];
        }
        entry++;
    } while (entry < 4);
    clean_up_cockpit();
}

/* Function start: 0x40C350 */
unsigned int GetObjectVisible(void)
{
    return 1;
}

/* Function start: 0x40C360 */
void place_ship_near_player_until_valid(short obj, int minimum,
                                        short maximum)
{
    (void)minimum;
    if (obj != -1) {
        g_aShipPosition_0059c490[obj] = g_aShipPosition_0059c490[0];
        g_aShipPosition_0059c490[obj].z += (int)maximum << 8;
    }
}

/* Function start: 0x40C5E0 */
void initialize_mission_ship(short obj, short missionShip,
                             signed char navPoint)
{
    MissionShipRecord *record = &g_aMissionShips_0046c948[missionShip];
    MissionNavPoint *nav = &g_aMissionNavPoints_0046c2f0[record->navPoint];

    g_nShipMissionIndices_0059c830[obj] = missionShip;
    g_aShipPosition_0059c490[obj].x = nav->position.x + record->position.x;
    g_aShipPosition_0059c490[obj].y = nav->position.y + record->position.y;
    g_aShipPosition_0059c490[obj].z = nav->position.z + record->position.z;
    alter_yaw((short)-record->pitch, obj);
    alter_pitch((short)-record->yaw, obj);
    alter_roll(record->roll, obj);
    g_aeShipSide_0059d650[obj] = record->side;
    g_anShipSpeed_0059b320[obj] = (int)record->speed << 8;
    g_aiPilotLevel_0059cf30[obj] = (int)record->cannedSequence;
    reset_mission_type(obj, record->missionType);
    g_anShipMissionShip_0059d4b0[obj] = record->targetMissionIndex;
    g_asShipWingLeader_0059d400[obj] =
        find_ship_index(record->leaderMissionIndex);
    zero_vector(&g_aShipVelocity_0059c010[obj]);
    g_abShipTurn_0059d860[obj] = 0;
    g_aeSpecialManeuver_0059c3c0[obj] = SPECIAL_MANEUVER_NONE;
    ((FixedVector *)g_aShipMissionSpot_0059dd10)[obj] = nav->position;

    if (record->missionType == MISSION_TYPE_CANNED_SEQUENCE) {
        g_apCannedSequence_0059dce0[obj] = record->cannedSequence;
        g_aiPilotLevel_0059cf30[obj] = 2;
        advance_canned_sequence(obj);
    }
    g_acShipRating_0059cd80[obj] =
        g_aiPilotLevel_0059cf30[obj] < 5 ? -1 :
        (signed char)(g_aiPilotLevel_0059cf30[obj] - 5);
    g_acShipStress_0059d620[obj] = 0;
    (void)navPoint;
}

/* Function start: 0x40C740 */
unsigned int IsInitialMissionShip(short missionShip)
{
    short index;

    if (g_nPlayerMissionShipIndex_005a8694 == missionShip)
        return 1;
    index = 0;
    do {
        if (g_nInitialMissionShipIndices_005a8696[index] == missionShip)
            return 1;
        index++;
    } while (index < 8);
    return 0;
}

/* Function start: 0x40C800 */
short spawn_mission_ship(short missionShip, short navPoint)
{
    MissionShipRecord *record;
    short obj;

    if (missionShip < 0 || missionShip >= WC1_MISSION_SHIP_COUNT)
        return -1;
    record = &g_aMissionShips_0046c948[missionShip];
    if (record->type == OBJECT_TYPE_ASTEROID_FIELD ||
        record->type == OBJECT_TYPE_MINE_FIELD)
        return -1;
    obj = find_ship_index(missionShip);
    if (obj != -1 || record->state != 0)
        return -1;
    record->navPoint = (signed char)navPoint;
    obj = initialize_ship(record->type, -1);
    if (obj != -1) {
        initialize_mission_ship(obj, missionShip, (signed char)navPoint);
        GetShipSlotState(obj);
    }
    return obj;
}

/* Function start: 0x40CBB0 */
void SetNavCursorIndex(unsigned short v)
{
    DAT_00468660 = v;
}

/* Function start: 0x40CBC0 */
void ObjectDrawHook(short *p)
{
    *p = (short)((int)*p / (DAT_00468664 * 100));
}
