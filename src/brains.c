/*
 *  NPC pilot intelligence (BRAINS.C), with adjacent Win32 flight and mission
 *  routines whose exact compilation-unit boundaries are still provisional.
 *
 *  Address range 0x4060a0-0x40cfff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: the surviving WINGLEADER BRAINS.C and the Mac `brain`
 *  symbol run both map cruise_home through FF_missile_intelligence exactly to
 *  0x409760-0x40b66f.  Mac CODE 6 also maps the adjacent `fl` maneuver family
 *  across 0x4060a0-0x4075cf; the enclosing Win32 boundaries remain provisional.
 */
#include "wc1.h"

/* Function start: 0x4060A0 */
void SetShipAiScratchWord(unsigned short v)
{
    DAT_00475e78 = v;
}

/* Function start: 0x4060B0 */
void maneuver_complete(short ship)
{
    set_special(ship, SPECIAL_MANEUVER_NONE);
    reset_maneuver(ship, MANEUVER_NONE);
}

/* Function start: 0x4060D0 */
void Mline_up_drop(short ship, short target)
{
    /* The shipped comparison normalizes this test to 0/1, then compares it
       with 9.  We therefore believe this completion path is unreachable. */
    if ((g_aeSpecialManeuver_0059c3c0[target] ==
         SPECIAL_MANEUVER_NORMAL) == 9) {
        maneuver_complete(ship);
        return;
    }
    if (no_goal(ship) != 0) {
        point_ship_at_point(ship, &g_aShipPosition_0059c490[target]);
        g_anRollGoal_0059d630[ship] = 360;
    }
}

/* Function start: 0x406130 */
void Mwabble(short ship)
{
    short choice;

    g_asShipCount_0059c420[ship]++;
    if (g_asShipCount_0059c420[ship] > 20) {
        maneuver_complete(ship);
        return;
    }

    approach_full_speed(ship);
    if (no_goal(ship) != 0) {
        choice = RandomBelowOrEqual(2);
        switch (choice) {
        case 0:
            g_anYawGoal_0059c310[ship] = signed_random(6) * 5;
            break;
        case 1:
            g_anPitchGoal_0059d7a0[ship] = signed_random(6) * 5;
            break;
        default:
            g_anRollGoal_0059d630[ship] = signed_random(10) * 5;
            break;
        }
    }
}

/* Function start: 0x4061E0 */
void advance(short ship)
{
    if (g_aeShipManeuver_0059dcb0[ship] != MANEUVER_NONE)
        g_acShipSequence_0059d520[ship] = g_acShipSequence_0059d520[ship] + 1;
}

/* Function start: 0x406200 */
void ShipAiState35(short ship, short target)
{
    (void)target;

    switch (g_acShipSequence_0059d520[ship]) {
    case 0:
        if (g_nTargetRange_0059ce10 < 750 ||
            ++g_asShipCount_0059c420[ship] > 10) {
            advance(ship);
            g_asShipCount_0059c420[ship] = 0;
        }
        break;
    case 1:
        veer_random(ship, 45);
        advance(ship);
        break;
    case 2:
        if (++g_asShipCount_0059c420[ship] > 5) {
            g_acShipSequence_0059d520[ship] = 0;
            g_asShipCount_0059c420[ship] = 0;
        }
        if (g_nTargetFacing_0059d52a < 75)
            advance(ship);
        break;
    case 3:
        point_ship(ship, 0, &g_vToTarget_0059d4d0);
        approach_speed(ship, 0x500);
        if (g_nFacingToTarget_0059d920 > 10)
            reset_maneuver(ship, MANEUVER_TAIL_FIRE);
        if (g_nTargetRange_0059ce10 > 1500 ||
            g_nTargetFacing_0059d52a > 80)
            maneuver_complete(ship);
        break;
    }
}

/* Function start: 0x406310 */
void Mfull_ahead(short ship)
{
    short count;

    approach_full_speed(ship);
    count = g_asShipCount_0059c420[ship];
    g_asShipCount_0059c420[ship] = count - 1;
    if (count < 1)
        maneuver_complete(ship);
}

/* Function start: 0x406350 */
void Mchill(short ship, short target)
{
    FixedVector destination;

    get_front_spot(target, 900, &destination);
    chase_location(ship, &destination, target);
    if (close_behind(1000) != 0)
        reset_maneuver(ship, g_acShipSequence_0059d520[ship]);
}

/* Function start: 0x4063B0 */
void Mdrop_a_mine(short ship)
{
    short weapon;

    weapon = -1;
    if (g_nTargetRange_0059ce10 > 1500)
        weapon = mine_available(ship);
    if (weapon != -1)
        fire_weapon(ship, weapon);
    approach_full_speed(ship);
    maneuver_complete(ship);
}

/* Function start: 0x406400 */
void Mthink(short ship)
{
    approach_cruise_speed(ship);
    if (g_asShipCount_0059c420[ship] == 0)
        g_asShipCount_0059c420[ship] = 2;
    if (--g_asShipCount_0059c420[ship] <= 1)
        maneuver_complete(ship);
}

/* Function start: 0x406440 */
void Mtight_loop(short ship)
{
    approach_cruise_speed(ship);
    switch (g_acShipSequence_0059d520[ship]) {
    case 0:
        g_anPitchGoal_0059d7a0[ship] = 180;
        advance(ship);
        return;
    case 1:
        if (no_goal(ship) != 0)
            advance(ship);
        approach_cruise_speed(ship);
        break;
    case 2:
        g_anPitchGoal_0059d7a0[ship] = 180;
        advance(ship);
        return;
    case 3:
        if (no_goal(ship) != 0)
            maneuver_complete(ship);
        break;
    default:
        maneuver_complete(ship);
        break;
    }
}

/* Function start: 0x4064F0 */
void Mhard_break(short ship)
{
    short advanceSequence;

    advanceSequence = 1;
    switch (g_acShipSequence_0059d520[ship]) {
    case 0:
        set_special(ship, SPECIAL_MANEUVER_KILL_ENGINES);
        break;
    case 1:
        advanceSequence = ++g_asShipCount_0059c420[ship] > 3;
        if (advanceSequence != 0)
            g_asShipCount_0059c420[ship] = 0;
        break;
    case 2:
        fire_super_brake(ship);
        break;
    case 3:
        advanceSequence = ++g_asShipCount_0059c420[ship] > 3;
        if (advanceSequence != 0)
            maneuver_complete(ship);
        break;
    }
    if (advanceSequence != 0)
        advance(ship);
}

/* Function start: 0x4065A0 */
void Msit_n_spin(short ship, short target)
{
    short advanceSequence;
    FixedVector destination;

    advanceSequence = 1;
    switch (g_acShipSequence_0059d520[ship]) {
    case 0:
        if (++g_asShipCount_0059c420[ship] < 4) {
            approach_speed(ship, g_anShipSpeed_0059b320[target]);
            ScaleFixedVector(&g_aShipForwardVector_0059bce0[target],
                             g_nTargetRange_0059ce10 * 2, &destination);
            AddFixedVectors(&g_aShipPosition_0059c490[target],
                            &destination, &destination);
            advanceSequence = 0;
            point_ship_at_point(ship, &destination);
        } else {
            g_asShipCount_0059c420[ship] = 0;
        }
        break;
    case 1:
        steady_object(ship);
        if (close_behind(1000) == 0) {
            advanceSequence = 0;
            reset_maneuver(ship, MANEUVER_CHILL);
            g_acShipSequence_0059d520[ship] = 10;
        }
        break;
    case 2:
        break;
    case 3:
        if (abs(g_anShipSpeed_0059b320[ship] -
                g_anShipSpeed_0059b320[target]) < 0x200) {
            advanceSequence = 0;
            approach_speed(ship, g_anShipSpeed_0059b320[target]);
        }
        break;
    case 4:
        set_special(ship, SPECIAL_MANEUVER_KILL_ENGINES);
        break;
    case 5:
        steady_object(ship);
        point_ship_at_object(ship, target);
        break;
    case 6:
        advanceSequence = no_goal(ship);
        break;
    case 7:
        if (g_nFacingToTarget_0059d920 > 85)
            fire(ship, target);
        if (++g_asShipCount_0059c420[ship] < 6)
            advanceSequence = 0;
        break;
    case 8:
        veer_random(ship, 35);
        break;
    case 9:
        advanceSequence = no_goal(ship);
        break;
    case 10:
        set_special(ship, SPECIAL_MANEUVER_NONE);
        reset_maneuver(ship, MANEUVER_ROLL_OVER);
        break;
    }
    if (advanceSequence != 0)
        advance(ship);
}

/* Function start: 0x4067A0 */
void Mturn_n_spin(short ship, short target)
{
    short advanceSequence;

    advanceSequence = 1;
    switch (g_acShipSequence_0059d520[ship]) {
    case 0:
        veer_random(ship, 90);
        break;
    case 1:
        ++g_asShipCount_0059c420[ship];
        advanceSequence = 1;
        if (g_asShipCount_0059c420[ship] <= 2)
            advanceSequence = 0;
        break;
    case 2:
        advanceSequence = g_nTargetFacing_0059d52a <= 80;
        if (advanceSequence != 0) {
            set_special(ship, SPECIAL_MANEUVER_KILL_ENGINES);
            point_ship_at_object(ship, target);
        } else {
            maneuver_complete(ship);
        }
        break;
    case 3:
        if (no_goal(ship) != 0)
            maneuver_complete(ship);
        break;
    }
    if (advanceSequence != 0)
        advance(ship);
}

/* Function start: 0x406860 */
void Mburnout(short ship, short target)
{
    (void)target;

    switch (g_acShipSequence_0059d520[ship]) {
    case 0:
        fire_afterburner(ship, 10);
        advance(ship);
        break;
    case 1:
        if (g_aeSpecialManeuver_0059c3c0[ship] == SPECIAL_MANEUVER_NONE) {
            g_anYawGoal_0059c310[ship] = 180;
            advance(ship);
        }
        break;
    default:
        if (no_goal(ship) != 0)
            maneuver_complete(ship);
        break;
    }
}

/* Function start: 0x4068D0 */
void Mkickit(short ship)
{
    switch (g_acShipSequence_0059d520[ship]) {
    case 0:
        fire_afterburner(ship, 10);
        advance(ship);
        break;
    default:
        if (g_aeSpecialManeuver_0059c3c0[ship] ==
            SPECIAL_MANEUVER_NONE)
            maneuver_complete(ship);
        break;
    }
}

/* Function start: 0x406910 */
void Mturn_n_kick(short ship)
{
    switch (g_acShipSequence_0059d520[ship]) {
    case 0:
        veer_random(ship, 90);
        advance(ship);
        break;
    case 1:
        if (no_goal(ship) != 0) {
            fire_afterburner(ship, 10);
            advance(ship);
        } else {
            approach_full_speed(ship);
        }
        break;
    case 2:
        if (g_aeSpecialManeuver_0059c3c0[ship] == SPECIAL_MANEUVER_NONE)
            maneuver_complete(ship);
        break;
    }
}

/* Function start: 0x406990 */
void Mroll_over(short ship)
{
    if (g_acShipSequence_0059d520[ship] == 0) {
        advance(ship);
        g_anRollGoal_0059d630[ship] =
            RandomBelowOrEqual(2) * 360 - 180;
    } else if (g_anRollGoal_0059d630[ship] == 0) {
        maneuver_complete(ship);
    } else {
        approach_full_speed(ship);
    }
}

/* Function start: 0x4069F0 */
void Mhard_turn(short ship)
{
    approach_full_speed(ship);
    switch (g_acShipSequence_0059d520[ship]) {
    case 0:
        g_anYawGoal_0059c310[ship] =
            RandomBelowOrEqual(2) * 360 - 180;
        advance(ship);
        break;
    default:
        if (no_goal(ship) != 0)
            maneuver_complete(ship);
        break;
    }
}

/* Function start: 0x406A50 */
void Mfish_hook(short ship, short target)
{
    short advanceSequence = 1;

    (void)target;
    switch (g_acShipSequence_0059d520[ship]) {
    case 0:
        g_anYawGoal_0059c310[ship] =
            RandomBelowOrEqual(2) * 240 - 120;
        fire_afterburner(ship, 5);
        break;
    case 1:
        advanceSequence = no_goal(ship);
        approach_full_speed(ship);
        break;
    case 2:
        fire_super_brake(ship);
        g_anYawGoal_0059c310[ship] =
            RandomBelowOrEqual(2) * 90 - 45;
        break;
    case 3:
        advanceSequence =
            g_aObjectTypeData_00466458[
                g_aeObjectType_0059b560[ship]].cruiseVelocity >=
            real_velocity(ship);
        break;
    case 4:
        advanceSequence = no_goal(ship);
        break;
    case 5:
        fire_afterburner(ship, 10);
        break;
    default:
        if (normal_speed(ship) != 0)
            maneuver_complete(ship);
        break;
    }
    if (advanceSequence != 0)
        advance(ship);
}

/* Function start: 0x406B60 */
void Mtry2tail(short ship, short target)
{
    if (unactive(target) == 0) {
        approach_full_speed(ship);
        if (no_goal(ship) != 0)
            point_ship_at_object(ship, target);
        if (RandomBelowOrEqual(100) < 4)
            veer_random(ship, 5);
    } else {
        maneuver_complete(ship);
    }
}

/* Function start: 0x406BD0 */
void Msplit_left(short ship)
{
    switch (g_acShipSequence_0059d520[ship]) {
    case 0:
        g_anYawGoal_0059c310[ship] = 90;
        advance(ship);
        break;
    default:
        if (no_goal(ship) != 0)
            maneuver_complete(ship);
        break;
    }
}

/* Function start: 0x406C20 */
void Msplit_right(short ship)
{
    switch (g_acShipSequence_0059d520[ship]) {
    case 0:
        g_anYawGoal_0059c310[ship] = -90;
        advance(ship);
        break;
    default:
        if (no_goal(ship) != 0)
            maneuver_complete(ship);
        break;
    }
}

/* Function start: 0x406C70 */
void Mgloat(short ship)
{
    switch (g_acShipSequence_0059d520[ship]) {
    case 0:
        set_special(ship, SPECIAL_MANEUVER_KILL_ENGINES);
        advance(ship);
        break;
    case 1:
        g_anPitchGoal_0059d7a0[ship] = 15;
        if (RandomBelow(100) < 50)
            advance(ship);
        break;
    default:
        g_anPitchGoal_0059d7a0[ship] = -30;
        if (RandomBelow(100) < 50) {
            if (++g_asShipCount_0059c420[ship] < 10)
                g_acShipSequence_0059d520[ship] = 1;
            else
                maneuver_complete(ship);
        }
        break;
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
        maneuver_complete(ship);
    }
}

/* Function start: 0x406E10 */
void Mtarget_missile(short ship, short target)
{
    short object;

    if (no_goal(ship) != 0)
        point_ship_at_object(ship, target);
    approach_cruise_speed(ship);

    object = 0;
    while (object < 10) {
        if (g_acObjectOwner_0059ce20[object] == ship &&
            g_aeObjectClass_0059d100[object] == OBJECT_CLASS_MISSILE) {
            reset_maneuver(ship, MANEUVER_STRAFE_ENEMY);
            return;
        }
        object++;
    }

    if (g_nFacingToTarget_0059d920 > 85 &&
        g_nTargetRange_0059ce10 < 6000 &&
        (g_nTargetFacing_0059d52a > 80 ||
         g_nTargetFacing_0059d52a < -80) &&
        RandomBelowOrEqual(5) == 0) {
        fire_missile(ship);
        maneuver_complete(ship);
    }
}

/* Function start: 0x406EC0 */
void Mram_missile(short ship, short target)
{
    approach_full_speed(ship);
    if (no_goal(ship) != 0)
        point_ship_at_object(ship, target);
    if (g_nFacingToTarget_0059d920 > 75 &&
        g_nTargetRange_0059ce10 < 6000) {
        fire_missile(ship);
        maneuver_complete(ship);
    }
}

/* Function start: 0x406F20 */
void Mbuzz_debris(short ship)
{
    switch (g_acShipSequence_0059d520[ship]) {
    case 0:
        veer_random(ship, 10);
        advance(ship);
        break;
    case 1:
        if (no_goal(ship) != 0) {
            fire_afterburner(ship, 10);
            advance(ship);
        } else {
            approach_full_speed(ship);
        }
        break;
    default:
        if (g_aeSpecialManeuver_0059c3c0[ship] ==
            SPECIAL_MANEUVER_NONE) {
            maneuver_complete(ship);
        } else if (g_nFacingToTarget_0059d920 > 95) {
            veer_random(ship, 10);
        }
        break;
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
    fire_when_ready(ship, (short)(aimed == 0));
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

/* Function start: 0x407060 */
void Msit_n_fire(short ship, short target)
{
    if (g_aeObjectClass_0059d100[target] == OBJECT_CLASS_CAPITAL_SHIP) {
        Mbest_strafe(ship, target);
        return;
    }
    if (no_goal(ship) != 0)
        point_ship_at_object(ship, target);
    if (g_nTargetRange_0059ce10 > 3000)
        approach_cruise_speed(ship);
    else
        approach_zero_speed(ship);
    fire_when_ready(ship, 1);
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

/* Function start: 0x407100 */
void Mkill_missile(short ship, short target)
{
    if (missile_on_tail(ship) == 0) {
        maneuver_complete(ship);
        return;
    }
    ship_vs_ship(ship, g_nTargetShip_0059c3b0);
    if (g_nFacingToTarget_0059d920 < 0) {
        reset_maneuver(ship, MANEUVER_FISH_HOOK);
        Mfish_hook(ship, target);
        return;
    }
    if (g_nFacingToTarget_0059d920 < 80) {
        reset_maneuver(ship, MANEUVER_BURNOUT);
        Mburnout(ship, target);
        return;
    }
    point_ship_at_object(ship, g_nTargetShip_0059c3b0);
    if (g_nTargetRange_0059ce10 < 8000)
        fire(ship, g_acShipTarget_0059ce60[ship]);
}

/* Function start: 0x4071B0 */
void Msuicide_run(short ship, short target)
{
    approach_full_speed(ship);
    if (no_goal(ship) != 0)
        point_ship_at_object(ship, target);
}

/* Function start: 0x4071E0 */
void Mget_distance(short ship, short target)
{
    short amount;

    if (g_nTargetRange_0059ce10 > 2000) {
        maneuver_complete(ship);
        return;
    }
    if (g_nTargetRange_0059ce10 < 700 && normal_speed(ship) != 0)
        fire_afterburner(ship, 10);
    else
        approach_full_speed(ship);
    if (g_nFacingToTarget_0059d920 > 0 && no_goal(ship) != 0) {
        amount = MinShort(20, g_nFacingToTarget_0059d920);
        steer_away_from_object(ship, target, amount);
    }
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
        complete = ++g_asShipCount_0059c420[ship] >= 4;
        break;
    case 3:
        pitch = -pitch;
        g_anYawGoal_0059c310[ship] = 35;
        g_anPitchGoal_0059d7a0[ship] = pitch;
        break;
    }
    if (g_acShipSequence_0059d520[ship] >= 12)
        maneuver_complete(ship);
    if (complete != 0)
        advance(ship);
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

/* Function start: 0x407390 */
void Mcorkscrew(short ship)
{
    approach_full_speed(ship);
    if (no_goal(ship) != 0) {
        if (--g_asShipCount_0059c420[ship] <= 0) {
            switch (g_acShipSequence_0059d520[ship] % 4) {
            case 0:
                g_anYawGoal_0059c310[ship] = -20;
                break;
            case 1:
            case 3:
                g_anRollGoal_0059d630[ship] = 20;
                break;
            case 2:
                g_anYawGoal_0059c310[ship] = 20;
                break;
            }
            g_asShipCount_0059c420[ship] = 4;
            advance(ship);
        }
    }
    if (g_acShipSequence_0059d520[ship] > 8)
        maneuver_complete(ship);
}

/* Function start: 0x407450 */
void Mveer_away(short ship, short target)
{
    switch (g_acShipSequence_0059d520[ship]) {
    case 0:
        if (g_nFacingToTarget_0059d920 > 80)
            steer_away_from_object(ship, target, 40);
        else
            steer_away_from_object(ship, target, 10);
        advance(ship);
        return;
    default:
        break;
    }
    if (g_asObjectCollisionRadius_0059d710[target] * 3 <
        g_nTargetRange_0059ce10) {
        veer_random(ship, 8);
        maneuver_complete(ship);
        return;
    }
    if (no_goal(ship) != 0) {
        if (g_nFacingToTarget_0059d920 > 80) {
            steer_away_from_object(ship, target, 40);
        } else if (g_nFacingToTarget_0059d920 < -65 ||
                   RandomBelowOrEqual(100) < 4) {
            veer_random(ship, 16);
        }
    }
    if ((DAT_00475e78 >= g_nTargetRange_0059ce10 ||
         RandomBelowOrEqual(100) < 10) &&
        normal_speed(ship) != 0) {
        fire_afterburner(ship, 10);
        return;
    }
    approach_full_speed(ship);
}

/* Function start: 0x407560 */
void ShipAiState44(short ship)
{
    g_acShipStress_0059d620[ship] = 0;
    maneuver_complete(ship);
}

/* Function start: 0x407580 */
void Mtarget_laser(short ship, short target)
{
    Mbest_strafe(ship, target);
}

/* Function start: 0x4075A0 */
void Mrout_me(short ship)
{
    try2rout(ship);
}

/* Function start: 0x4075B0 */
/* Empty in the original: dispatch-table slots 0 and 1 (no-op / invalid state). */
void Mnone(void)
{
}

/* Function start: 0x4075C0 */
void Mreset(short ship)
{
    maneuver_complete(ship);
}

/* Function start: 0x4075D0 */
void perform_maneuver(short obj)
{
    short target = g_acShipTarget_0059ce60[obj];
    enum ShipManeuver previous = g_aeShipManeuver_0059dcb0[obj];
    short range;
    int maneuverWeight;

#ifdef WC1_SDL
    /* The original performs this lookup before validating the maneuver.
       MANEUVER_NONE reads the zero alignment byte at 0x00465677, immediately
       before the table.  Other invalid values are reset before the result is
       used.  Preserve those results without an invalid C array access. */
    if (previous < MANEUVER_WARPING_IN ||
        previous > MANEUVER_UNKNOWN_46)
        g_bCurrentManeuverReroll_00475e7c = 0;
    else
#endif
        g_bCurrentManeuverReroll_00475e7c =
            g_abManeuverRerollChance_00465678[previous];
#ifdef WC1_SDL
    /* The original reaches the same completion path only after calculating
       geometry for target -1.  Those reads alias the globals immediately
       before three object tables in the Win32 image. */
    if (target == -1) {
        maneuver_complete(obj);
        return;
    }
#endif
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
            Mveer_away(obj, target);
        } else if (g_aeShipManeuver_0059dcb0[obj] == MANEUVER_GLOAT) {
            ((void (__cdecl *)(short, short))Mgloat)(obj, target);
        } else if (g_aeShipManeuver_0059dcb0[obj] ==
                   MANEUVER_LINE_UP_DROP) {
            Mline_up_drop(obj, target);
        } else {
            maneuver_complete(obj);
        }
    } else if (g_aeShipManeuver_0059dcb0[obj] >= 0 &&
               g_aeShipManeuver_0059dcb0[obj] < 47) {
        g_apShipAiManeuverHandlers_004656a8[
            g_aeShipManeuver_0059dcb0[obj]](obj, target);
    } else {
        maneuver_complete(obj);
    }

    if (range < DAT_00475e78) {
        try2reset_maneuver(obj, 2);
    } else if (g_aeShipManeuver_0059dcb0[obj] == previous &&
               RandomBelowOrEqual(100) <
                   (short)g_bCurrentManeuverReroll_00475e7c) {
        maneuver_complete(obj);
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

/* Function start: 0x407750 */
unsigned int AnimateScrambleWalk(short ticks)
{
    signed char elapsed;

    DAT_00469fb4 = 1;
    elapsed = 0;
    if (ticks > 0) {
        do {
            PumpWindowMessages();
            DAT_00469fb4--;
            if (DAT_00469fb4 < 1) {
                DAT_00469fb4 = g_nFrameSkip_00469fb8;
                DrawSpriteDefault(&DAT_005a76b0,
                                  g_nScrambleBackgroundX_00465780, 0,
                                  g_pScrambleHangarShape_005a872c, 0);
                DrawSpriteDefault(&DAT_005a76b0,
                                  (short)(g_nScrambleBackgroundX_00465780 +
                                          320),
                                  0, g_pScrambleHangarShape_005a872c, 1);
                DrawSpriteDefault(&DAT_005a76b0,
                                  (short)(g_nScrambleBackgroundX_00465780 +
                                          640),
                                  0, g_pScrambleHangarShape_005a872c, 0);
                DrawSpriteDefault(&DAT_005a76b0,
                                  (short)(g_nScrambleBackgroundX_00465780 +
                                          960),
                                  0, g_pScrambleHangarShape_005a872c, 1);

                g_nScrambleWalkerY_005a8734 =
                    127 - GetShapeFrameExtent(
                        0, 0, g_pScrambleHangarShape_005a872c,
                        g_cScrambleLeftWalkerFrame_00465768, 3);
                DrawSpriteDefault(&DAT_005a76b0,
                                  g_nScrambleLeftWalkerX_00465778,
                                  g_nScrambleWalkerY_005a8734,
                                  g_pScrambleHangarShape_005a872c,
                                  g_cScrambleLeftWalkerFrame_00465768);
                DrawSpriteDefault(
                    &DAT_005a76b0, g_nScrambleLeftWalkerX_00465778,
                    g_nScrambleWalkerY_005a8734,
                    g_pScrambleHangarShape_005a872c,
                    g_acScrambleWalkerOverlayFrames_00465770[
                        g_cScrambleWalkerPair_005a8748 * 2]);

                g_nScrambleWalkerY_005a8734 =
                    137 - GetShapeFrameExtent(
                        0, 0, g_pScrambleHangarShape_005a872c,
                        g_cScrambleRightWalkerFrame_0046576c, 3);
                DrawSpriteDefault(&DAT_005a76b0,
                                  g_nScrambleRightWalkerX_0046577c,
                                  g_nScrambleWalkerY_005a8734,
                                  g_pScrambleHangarShape_005a872c,
                                  g_cScrambleRightWalkerFrame_0046576c);
                DrawSpriteDefault(
                    &DAT_005a76b0, g_nScrambleRightWalkerX_0046577c,
                    g_nScrambleWalkerY_005a8734,
                    g_pScrambleHangarShape_005a872c,
                    g_acScrambleWalkerOverlayFrames_00465770[
                        g_cScrambleWalkerPair_005a8748 * 2 + 1]);
                RefreshMemoryStatusOverlay();
                DIBslam();
                DIBslamReal();

                g_cScrambleLeftWalkerFrame_00465768++;
                if (g_cScrambleLeftWalkerFrame_00465768 > 19)
                    g_cScrambleLeftWalkerFrame_00465768 = 7;
                g_cScrambleRightWalkerFrame_0046576c++;
                if (g_cScrambleRightWalkerFrame_0046576c > 19)
                    g_cScrambleRightWalkerFrame_0046576c = 7;
            }
            g_nScrambleBackgroundX_00465780 =
                g_nScrambleBackgroundX_00465780 - 12;
            g_nScrambleRightWalkerX_0046577c =
                g_nScrambleRightWalkerX_0046577c + 3;
            g_nScrambleLeftWalkerX_00465778 =
                g_nScrambleLeftWalkerX_00465778 + 2;
            if (DAT_0059ab58 == 1)
                break;
            elapsed++;
        } while (elapsed < ticks);
    }
    return 0;
}

/* Function start: 0x4079C0 */
unsigned int PlayScrambleHangarScene(void)
{
    if (g_nMemoryConfiguration_005a7cd4 == 1)
        SceneLeaveHook();
    else
        PreloadMusicTrackHook(0x1b);
    spacetrack(0x1b, 1, -1);

    InitializeConversationViewport();
    g_cScrambleLeftWalkerFrame_00465768 = 7;
    g_nScrambleBackgroundX_00465780 = 0;
    g_cScrambleRightWalkerFrame_0046576c = 10;
    g_nScrambleLeftWalkerX_00465778 = 70;
    g_nScrambleRightWalkerX_0046577c = 170;
    g_pScrambleHangarShape_005a872c =
        FetchDiskPacketRetrying(1, 0, 0);
    g_cScrambleWalkerPair_005a8748 =
        (signed char)(((unsigned short)rand() + 3) & 3);
    PlaySfxWaveFileByNumber(14, -1, 0);
    DAT_0059ab58 = 0;

    AnimateScrambleWalk(24);
    if (DAT_0059ab58 != 1) {
        g_cScrambleLeftWalkerFrame_00465768 = 21;
        g_cScrambleRightWalkerFrame_0046576c = 24;
        g_nScrambleLeftWalkerX_00465778 = 90;
        g_nScrambleRightWalkerX_0046577c = 200;
        g_nScrambleWalkerY_005a8734 = -14;
        DAT_00469fb4 = 1;
        g_cScrambleWalkTicks_005a86e8 = 0;
        do {
            PumpWindowMessages();
            DAT_00469fb4--;
            if (DAT_00469fb4 < 1) {
                DAT_00469fb4 = g_nFrameSkip_00469fb8;
                DrawSpriteDefault(&DAT_005a76b0,
                                  g_nScrambleBackgroundX_00465780, 0,
                                  g_pScrambleHangarShape_005a872c, 2);
                DrawSpriteDefault(&DAT_005a76b0,
                                  (short)(g_nScrambleBackgroundX_00465780 +
                                          320),
                                  0, g_pScrambleHangarShape_005a872c, 2);
                DrawSpriteDefault(&DAT_005a76b0,
                                  (short)(g_nScrambleBackgroundX_00465780 +
                                          640),
                                  0, g_pScrambleHangarShape_005a872c, 2);
                DrawSpriteDefault(&DAT_005a76b0,
                                  g_nScrambleLeftWalkerX_00465778,
                                  g_nScrambleWalkerY_005a8734,
                                  g_pScrambleHangarShape_005a872c,
                                  g_cScrambleLeftWalkerFrame_00465768);
                DrawSpriteDefault(&DAT_005a76b0,
                                  g_nScrambleRightWalkerX_0046577c,
                                  g_nScrambleWalkerY_005a8734,
                                  g_pScrambleHangarShape_005a872c,
                                  g_cScrambleRightWalkerFrame_0046576c);
                RefreshMemoryStatusOverlay();
                DIBslam();
                DIBslamReal();

                g_cScrambleLeftWalkerFrame_00465768++;
                if (g_cScrambleLeftWalkerFrame_00465768 > 26)
                    g_cScrambleLeftWalkerFrame_00465768 = 21;
                g_cScrambleRightWalkerFrame_0046576c++;
                if (g_cScrambleRightWalkerFrame_0046576c > 26)
                    g_cScrambleRightWalkerFrame_0046576c = 21;
            }
            g_nScrambleBackgroundX_00465780 =
                (short)(g_nScrambleBackgroundX_00465780 - 12);
            g_nScrambleLeftWalkerX_00465778 =
                (short)(g_nScrambleLeftWalkerX_00465778 + 2);
            g_nScrambleRightWalkerX_0046577c =
                (short)(g_nScrambleRightWalkerX_0046577c + 3);
            if (DAT_0059ab58 == 1)
                break;
            g_cScrambleWalkTicks_005a86e8++;
        } while (g_cScrambleWalkTicks_005a86e8 < 24);

        if (DAT_0059ab58 != 1) {
            g_cScrambleLeftWalkerFrame_00465768 = 7;
            g_cScrambleRightWalkerFrame_0046576c = 10;
            AnimateScrambleWalk(24);
        }
    }

    FlushSoundEffectsAndLog();
    ReleasePacketHandle(g_pScrambleHangarShape_005a872c);
    ResetScreenClipToFullHeight();
    return 0;
}

/* Function start: 0x407C90 */
unsigned int DrawScrambleActor(short actorIndex)
{
    signed char frame;
    int control;
    signed char animationFrame;
    short x;
    short y;

    frame = 0;
    if ((animationFrame = g_aScrambleAnimationActors_004657b0[
             actorIndex].animationFrame) != -1) {
        if (g_aScrambleAnimationActors_004657b0[
                actorIndex].animationState != 0xa000) {
            animationFrame++;
            do {
                frame = g_aScrambleAnimationActors_004657b0[
                    actorIndex].animation[animationFrame];
                control = (unsigned char)frame & 0xc0;
                switch (control) {
                    case 0:
                        g_aScrambleAnimationActors_004657b0[
                            actorIndex].animationFrame++;
                        break;
                    case 0x40:
                        g_aScrambleAnimationActors_004657b0[
                            actorIndex].animationState = 0;
                        animationFrame--;
                        break;
                    case 0x80:
                        animationFrame = frame & 0x3f;
                        g_aScrambleAnimationActors_004657b0[
                            actorIndex].animationFrame = animationFrame;
                        break;
                }
            } while (control != 0);
        } else
            frame = animationFrame;
    }

    x = g_aScrambleAnimationActors_004657b0[actorIndex].deltaX +
        g_aScrambleAnimationActors_004657b0[actorIndex].x;
    g_aScrambleAnimationActors_004657b0[actorIndex].x = x;
    y = g_aScrambleAnimationActors_004657b0[actorIndex].deltaY +
        g_aScrambleAnimationActors_004657b0[actorIndex].y;
    g_aScrambleAnimationActors_004657b0[actorIndex].y = y;
    DrawSpriteScaled(
        g_pScrambleViewport_005a86b4,
        (short)(g_nScrambleBackgroundRightX_005a8714 + x),
        (short)(g_nScrambleBackgroundY_005a8712 + y),
        g_aScrambleAnimationActors_004657b0[actorIndex].shape,
        (short)(frame +
                g_aScrambleAnimationActors_004657b0[actorIndex].baseFrame),
        g_aScrambleAnimationActors_004657b0[actorIndex].angle,
        g_aScrambleAnimationActors_004657b0[actorIndex].scale,
        g_aScrambleAnimationActors_004657b0[actorIndex].flip);
    return 0;
}

/* Function start: 0x407D90 */
unsigned int ConfigureScrambleActor(short x, short y, short deltaX,
                                    short deltaY, unsigned char *shape,
                                    short scale, short angle,
                                    signed char flip, short actorIndex)
{
    ScrambleAnimationActor *actor;

    actor = &g_aScrambleAnimationActors_004657b0[actorIndex];
    actor->x = x;
    actor->y = y;
    actor->deltaX = deltaX;
    actor->deltaY = deltaY;
    actor->shape = shape;
    if (actor->animationFrame != -1)
        actor->animationFrame = 0;
    actor->scale = scale;
    actor->angle = angle;
    actor->flip = flip;
    return 0;
}

/* Function start: 0x407E10 */
unsigned int DrawScrambleFrame(void)
{
    short detailIndex;
    short detailSlot;
    const ScrambleShipDetail *detail;

    DAT_00469fb4--;
    if (DAT_00469fb4 > 0)
        return 0;
    DAT_00469fb4 = g_nFrameSkip_00469fb8;

    DrawSpriteDefault(&DAT_005a76b0,
                      (short)(g_nScrambleBackgroundRightX_005a8714 - 1),
                      g_nScrambleBackgroundY_005a8712,
                      g_pScrambleBackgroundShape_005a870c, 0);
    DrawSpriteDefault(&DAT_005a76b0,
                      g_nScrambleBackgroundRightX_005a8714,
                      g_nScrambleBackgroundY_005a8712,
                      g_pScrambleBackgroundShape_005a870c, 1);
    DrawScrambleActor(0);
    DrawScrambleActor(3);
    DrawScrambleActor(4);
    DrawScrambleActor(2);

    if (g_bScrambleCanopyClosed_005a873a == 0) {
        DrawSpriteDefault(&DAT_005a76b0,
                          (short)(g_nScrambleShipX_005a8724 + 40),
                          (short)(g_nScrambleShipY_005a8726 - 40),
                          g_pScrambleCanopyShape_005a874c,
                          g_nScrambleCanopyOffset_005a8736);
    }

    switch (g_stCampaignState_0059ca50.playerShipType) {
    case OBJECT_TYPE_HORNET:
        DrawSpriteDefault(&DAT_005a76b0,
                          (short)(g_nScrambleShipX_005a8724 + 10),
                          (short)(g_nScrambleShipY_005a8726 - 25),
                          g_pScrambleShipShape_005a8750,
                          g_nScrambleCanopyFrame_005a86b8);
        DrawSpriteDefault(&DAT_005a76b0, g_nScrambleShipX_005a8724,
                          g_nScrambleShipY_005a8726,
                          g_pScrambleCockpitShape_005a8730, 0);
        DrawSpriteScaled(&DAT_005a76b0,
                         g_nScrambleCockpitDetailX_005a86c4,
                         g_nScrambleCockpitDetailY_005a86c6,
                         g_pScrambleCockpitShape_005a8730, 1, 0,
                         g_nScrambleCockpitScale_005a86c0, 0);
        break;
    case OBJECT_TYPE_RAPIER:
        DrawSpriteDefault(&DAT_005a76b0,
                          (short)(g_nScrambleShipX_005a8724 + 10),
                          (short)(g_nScrambleShipY_005a8726 - 16),
                          g_pScrambleShipShape_005a8750,
                          g_nScrambleCanopyFrame_005a86b8);
        DrawSpriteDefault(&DAT_005a76b0, g_nScrambleShipX_005a8724,
                          g_nScrambleShipY_005a8726,
                          g_pScrambleCockpitShape_005a8730, 0);
        DrawSpriteDefault(&DAT_005a76b0,
                          (short)(g_nScrambleShipX_005a8724 + 153),
                          (short)(g_nScrambleShipY_005a8726 + 5),
                          g_pScrambleCockpitShape_005a8730, 2);
        DrawSpriteDefault(&DAT_005a76b0,
                          g_nScrambleCockpitDetailX_005a86c4,
                          g_nScrambleCockpitDetailY_005a86c6,
                          g_pScrambleCockpitShape_005a8730, 1);
        break;
    case OBJECT_TYPE_SCIMITAR:
        DrawSpriteDefault(&DAT_005a76b0,
                          (short)(g_nScrambleShipX_005a8724 + 10),
                          (short)(g_nScrambleShipY_005a8726 - 15),
                          g_pScrambleShipShape_005a8750,
                          g_nScrambleCanopyFrame_005a86b8);
        DrawSpriteDefault(&DAT_005a76b0, g_nScrambleShipX_005a8724,
                          g_nScrambleShipY_005a8726,
                          g_pScrambleCockpitShape_005a8730, 0);
        DrawSpriteDefault(&DAT_005a76b0,
                          (short)(g_nScrambleShipX_005a8724 + 148),
                          g_nScrambleShipY_005a8726,
                          g_pScrambleCockpitShape_005a8730, 2);
        DrawSpriteDefault(&DAT_005a76b0,
                          g_nScrambleCockpitDetailX_005a86c4,
                          g_nScrambleCockpitDetailY_005a86c6,
                          g_pScrambleCockpitShape_005a8730, 1);
        break;
    case OBJECT_TYPE_RAPTOR:
        DrawSpriteDefault(&DAT_005a76b0,
                          (short)(g_nScrambleShipX_005a8724 + 10),
                          (short)(g_nScrambleShipY_005a8726 - 11),
                          g_pScrambleShipShape_005a8750,
                          g_nScrambleCanopyFrame_005a86b8);
        DrawSpriteDefault(&DAT_005a76b0, g_nScrambleShipX_005a8724,
                          g_nScrambleShipY_005a8726,
                          g_pScrambleCockpitShape_005a8730, 0);
        DrawSpriteDefault(&DAT_005a76b0,
                          (short)(g_nScrambleShipX_005a8724 + 158),
                          (short)(g_nScrambleShipY_005a8726 + 6),
                          g_pScrambleCockpitShape_005a8730, 2);
        DrawSpriteDefault(&DAT_005a76b0,
                          g_nScrambleCockpitDetailX_005a86c4,
                          g_nScrambleCockpitDetailY_005a86c6,
                          g_pScrambleCockpitShape_005a8730, 1);
        break;
    }

    detailIndex = 0;
    while (detailIndex < g_nScrambleShipDetailCount_005a86cc) {
        detailSlot = g_acScrambleShipDetailIndices_005a86d0[detailIndex];
        detail = &g_aaScrambleShipDetails_00465828[
            g_stCampaignState_0059ca50.playerShipType][detailSlot];
        DrawSpriteDefault(&DAT_005a76b0,
                          (short)(g_nScrambleShipX_005a8724 + detail->x),
                          (short)(g_nScrambleShipY_005a8726 + detail->y),
                          g_pScrambleDetailShape_005a86bc,
                          detail->frame);
        detailIndex++;
    }

    if (g_bScrambleCanopyClosed_005a873a == 1) {
        if (g_nScrambleOverlayX_005a8740 != -1000) {
            DrawSpriteDefault(&DAT_005a76b0,
                              g_nScrambleOverlayX_005a8740,
                              g_nScrambleOverlayY_005a8742,
                              g_pScrambleOverlayShape_005a8744, 0);
        }
        DrawSpriteDefault(&DAT_005a76b0, 100, 127,
                          g_pScrambleCanopyShape_005a874c,
                          g_nScrambleCanopyOffset_005a8736);
    }
    RefreshMemoryStatusOverlay();
    DIBslam();
    DIBslamReal();
    return 0;
}

/* Function start: 0x408200 (Mac symbol: scramble) */
unsigned int scramble(void)
{
    unsigned char *actorShape;
    short frame;

    g_nScrambleCanopyOffset_005a8736 = 0;
    g_nScrambleOverlayX_005a8740 = -1000;
    g_nScrambleCanopyFrame_005a86b8 = 0;
    g_nScrambleShipDetailCount_005a86cc = 0;
    if (DAT_0059ab58 == 0) {
        InitializeConversationViewport();
        g_cCockpitLogicalFile_005a7c74 =
            (signed char)(g_stCampaignState_0059ca50.playerShipType + 17);
        g_pScrambleCockpitShape_005a8730 = FetchDiskPacketRetrying(
            g_cCockpitLogicalFile_005a7c74, 8, 0);
        g_pScrambleBackgroundShape_005a870c =
            FetchDiskPacketRetrying(1, 1, 0);
        g_bScrambleCanopyClosed_005a873a = 0;
        g_pScrambleCanopyShape_005a874c =
            FetchDiskPacketRetrying(1, 2, 0);
        g_pScrambleShipShape_005a8750 =
            FetchDiskPacketRetrying(1, 3, 0);
        actorShape = FetchDiskPacketRetrying(1, 4, 0);
        g_nScrambleBackgroundY_005a8712 = 0;
        g_pScrambleViewport_005a86b4 = &DAT_005a76b0;
        g_nScrambleBackgroundRightX_005a8714 = 64;

        ConfigureScrambleActor(130, 94, 1, 0, actorShape,
                               0x100, 0, 0, 0);
        ConfigureScrambleActor(160, 120, 0, 0, actorShape,
                               0x100, 0, 0, 2);
        ConfigureScrambleActor(260, 100, -3, 0, actorShape,
                               0xff, 0, 0x10, 3);
        ConfigureScrambleActor(260, 100, -3, 0, actorShape,
                               0xff, 0, 0x10, 4);
        PlaySfxWaveFileByNumber(17, -1, 0);
        DAT_0059ab58 = 0;

        switch (g_stCampaignState_0059ca50.playerShipType) {
        case OBJECT_TYPE_HORNET:
            g_nScrambleShipX_005a8724 = -40;
            g_nScrambleShipY_005a8726 = 96;
            g_nScrambleCockpitDetailX_005a86c4 = -95;
            g_nScrambleCockpitDetailY_005a86c6 = 71;
            g_nScrambleCockpitScale_005a86c0 = 316;
            break;
        case OBJECT_TYPE_RAPIER:
            g_nScrambleShipX_005a8724 = -30;
            g_nScrambleShipY_005a8726 = 80;
            g_nScrambleCockpitDetailX_005a86c4 = -15;
            g_nScrambleCockpitDetailY_005a86c6 = 76;
            break;
        case OBJECT_TYPE_SCIMITAR:
            g_nScrambleShipX_005a8724 = -40;
            g_nScrambleShipY_005a8726 = 86;
            g_nScrambleCockpitDetailX_005a86c4 = 4;
            g_nScrambleCockpitDetailY_005a86c6 = 83;
            break;
        case OBJECT_TYPE_RAPTOR:
            g_nScrambleShipX_005a8724 = -40;
            g_nScrambleShipY_005a8726 = 80;
            g_nScrambleCockpitDetailX_005a86c4 = -22;
            g_nScrambleCockpitDetailY_005a86c6 = 67;
            break;
        }

        frame = 0;
        DAT_00469fb4 = 1;
        do {
            PumpWindowMessages();
            DrawScrambleFrame();
            g_nScrambleBackgroundRightX_005a8714--;
            g_nScrambleShipY_005a8726 =
                (short)(g_nScrambleShipY_005a8726 - 2);
            g_nScrambleCockpitDetailY_005a86c6 =
                (short)(g_nScrambleCockpitDetailY_005a86c6 - 2);
            g_nScrambleShipX_005a8724 =
                (short)(g_nScrambleShipX_005a8724 + 4);
            g_nScrambleCockpitDetailX_005a86c4 =
                (short)(g_nScrambleCockpitDetailX_005a86c4 + 4);
            if (DAT_0059ab58 == 1)
                break;
            frame++;
        } while (frame < 10);

        if (DAT_0059ab58 != 1) {
            frame = 0;
            DAT_00469fb4 = 1;
            do {
                PumpWindowMessages();
                DrawScrambleFrame();
                g_nScrambleCanopyOffset_005a8736++;
                g_nScrambleShipX_005a8724 =
                    (short)(g_nScrambleShipX_005a8724 + 4);
                g_nScrambleCockpitDetailX_005a86c4 =
                    (short)(g_nScrambleCockpitDetailX_005a86c4 + 4);
                g_nScrambleBackgroundRightX_005a8714--;
                if (g_nScrambleCanopyFrame_005a86b8 < 25)
                    g_nScrambleCanopyFrame_005a86b8++;
                if (DAT_0059ab58 == 1)
                    break;
                frame++;
            } while (frame < 27);
        }

        if (DAT_0059ab58 != 1) {
            FlushSoundEffectsAndLog();
            g_nScrambleCanopyOffset_005a8736--;
            frame = 0;
            PlaySfxWaveFileByNumber(15, -1, 0);
            DAT_00469fb4 = 1;
            do {
                PumpWindowMessages();
                if (frame == 22)
                    DAT_00469fb4 = 1;
                DrawScrambleFrame();
                switch (g_stCampaignState_0059ca50.playerShipType) {
                case OBJECT_TYPE_HORNET:
                    g_nScrambleCockpitScale_005a86c0 =
                        (short)(g_nScrambleCockpitScale_005a86c0 + 2);
                    break;
                case OBJECT_TYPE_RAPIER:
                case OBJECT_TYPE_RAPTOR:
                    g_nScrambleCockpitDetailX_005a86c4 =
                        (short)(g_nScrambleCockpitDetailX_005a86c4 - 2);
                    break;
                case OBJECT_TYPE_SCIMITAR:
                    if (frame == 21)
                        g_nScrambleCockpitDetailY_005a86c6++;
                    else
                        g_nScrambleCockpitDetailX_005a86c4 =
                            (short)(g_nScrambleCockpitDetailX_005a86c4 - 2);
                    break;
                }
                if (g_nScrambleCanopyFrame_005a86b8 < 35)
                    g_nScrambleCanopyFrame_005a86b8++;
                if (DAT_0059ab58 == 1)
                    break;
                frame++;
            } while (frame < 23);

            FlushSoundEffectsAndLog();
            PlaySfxWaveFileByNumber(16, -1, 0);
            if (DAT_0059ab58 != 1)
                WaitForSceneAdvance(60, 0);
        }

        DAT_0059ab58 = 0;
        ReleasePacketHandle(g_pScrambleCockpitShape_005a8730);
        ReleasePacketHandle(g_pScrambleBackgroundShape_005a870c);
        ReleasePacketHandle(g_pScrambleCanopyShape_005a874c);
        ReleasePacketHandle(g_pScrambleShipShape_005a8750);
        ReleasePacketHandle(actorShape);
        FlushSoundEffects();
        ResetScreenClipToFullHeight();
    }

    if (g_nMemoryConfiguration_005a7cd4 == 0) {
        StopMusicUnlessSuppressed();
        ReleaseMusicTrackHook(0x1b);
    }
    return 0;
}

/* Function start: 0x408650 (Mac symbol: landing) */
unsigned int landing(signed char damageLevel)
{
    unsigned char *actorShape;
    const signed char *canopyFrames;
    short frame;
    signed char detail;
    signed char prior;
    int damageOffset;

    PreloadMusicTrackHook(0x1d);
    spacetrack(0x1d, 2, 1);
    SetEventManagerPump(PollJoystickButtonEvents);
    InitializeConversationViewport();
    damageOffset = damageLevel * 4;
    g_nScrambleShipDetailCount_005a86cc =
        g_anLandingDamageDetailCounts_00465aa8[damageLevel];
    frame = 0;
    while (frame < g_nScrambleShipDetailCount_005a86cc) {
        do {
            detail = RandomInRange(0, 31);
            for (prior = 0; prior < frame; prior++) {
                if (g_acScrambleShipDetailIndices_005a86d0[prior] ==
                    detail)
                    break;
            }
        } while (prior < frame);
        g_acScrambleShipDetailIndices_005a86d0[frame] = detail;
        frame++;
    }

    g_cCockpitLogicalFile_005a7c74 =
        (signed char)(g_stCampaignState_0059ca50.playerShipType + 17);
    g_pScrambleCockpitShape_005a8730 =
        FetchDiskPacketRetrying(
            g_cCockpitLogicalFile_005a7c74, 8, 0);
    g_pScrambleBackgroundShape_005a870c =
        FetchDiskPacketRetrying(1, 1, 0);
    g_pScrambleShipShape_005a8750 =
        FetchDiskPacketRetrying(1, 3, 0);
    actorShape = FetchDiskPacketRetrying(1, 4, 0);
    g_pScrambleDetailShape_005a86bc =
        FetchDiskPacketRetrying(1, 9, 0);
    g_pScrambleOverlayShape_005a8744 =
        FetchDiskPacketRetrying(1, 5, 0);
    g_bScrambleCanopyClosed_005a873a = 1;
    g_pScrambleCanopyShape_005a874c =
        FetchDiskPacketRetrying(1, 6, 0);
    InitializeConversationText();
    g_nScrambleBackgroundY_005a8712 = 0;
    g_pScrambleViewport_005a86b4 = &DAT_005a76b0;
    g_nScrambleBackgroundRightX_005a8714 = 32;
    ConfigureScrambleActor(140, 88, 2, 0, actorShape,
                           0x80, 0, 0, 3);
    ConfigureScrambleActor(139, 88, 2, 0, actorShape,
                           0x80, 0, 0, 4);
    ConfigureScrambleActor(240, 94, -1, 0, actorShape,
                           0x100, 0, 0x10, 0);
    ConfigureScrambleActor(160, 120, 0, 0, actorShape,
                           0x100, 0, 0, 2);
    g_nScrambleCanopyOffset_005a8736 = 0;
    g_nScrambleCanopyFrame_005a86b8 = 34;
    g_nScrambleOverlayX_005a8740 = -1000;

    switch (g_stCampaignState_0059ca50.playerShipType) {
    case OBJECT_TYPE_HORNET:
        g_nScrambleShipX_005a8724 = 124;
        g_nScrambleShipY_005a8726 = 140;
        g_nScrambleCockpitDetailX_005a86c4 = 69;
        g_nScrambleCockpitDetailY_005a86c6 = 115;
        g_nScrambleCockpitScale_005a86c0 = 360;
        break;
    case OBJECT_TYPE_RAPIER:
        g_nScrambleShipX_005a8724 = 124;
        g_nScrambleShipY_005a8726 = 130;
        g_nScrambleCockpitDetailX_005a86c4 = 94;
        g_nScrambleCockpitDetailY_005a86c6 = 125;
        break;
    case OBJECT_TYPE_SCIMITAR:
        g_nScrambleShipY_005a8726 = 134;
        g_nScrambleCockpitDetailY_005a86c6 = 132;
        g_nScrambleShipX_005a8724 = 124;
        g_nScrambleCockpitDetailX_005a86c4 = 124;
        break;
    case OBJECT_TYPE_RAPTOR:
        g_nScrambleShipX_005a8724 = 124;
        g_nScrambleShipY_005a8726 = 126;
        g_nScrambleCockpitDetailX_005a86c4 = 96;
        g_nScrambleCockpitDetailY_005a86c6 = 113;
        break;
    }

    frame = 0;
    DAT_0059ab58 = 0;
    PlaySfxWaveFileByNumber(17, -1, 0);
    DAT_00469fb4 = 1;
    do {
        PumpWindowMessages();
        DrawScrambleFrame();
        g_nScrambleShipY_005a8726 =
            (short)(g_nScrambleShipY_005a8726 - 2);
        g_nScrambleCockpitDetailY_005a86c6 =
            (short)(g_nScrambleCockpitDetailY_005a86c6 - 2);
        if (DAT_0059ab58 == 1)
            break;
        frame++;
    } while (frame < 30);

    ((void (__cdecl *)(int, int))FlushSoundEffectsAndLog)(
        damageOffset, 0);
    if (DAT_0059ab58 != 1) {
        g_nScrambleOverlayX_005a8740 =
            (short)(g_nScrambleShipX_005a8724 + 180);
        canopyFrames = *(const signed char **)
            ((unsigned char *)g_apLandingCanopyFrames_00465b08 +
             damageOffset);
        g_nScrambleOverlayY_005a8742 =
            (short)(g_nScrambleShipY_005a8726 + 50);
        frame = 0;
        PlaySfxWaveFileByNumber(15, -1, 0);
        DAT_00469fb4 = 1;
        do {
            PumpWindowMessages();
            if (g_nRenderedSpaceFrame_0059d61a == 29)
                DAT_00469fb4 = 1;
            DrawScrambleFrame();
            g_nScrambleOverlayY_005a8742--;
            g_nScrambleOverlayX_005a8740 =
                (short)(g_nScrambleOverlayX_005a8740 - 4);
            switch (g_stCampaignState_0059ca50.playerShipType) {
            case OBJECT_TYPE_HORNET:
                g_nScrambleCockpitScale_005a86c0 =
                    (short)(g_nScrambleCockpitScale_005a86c0 - 2);
                break;
            case OBJECT_TYPE_RAPIER:
            case OBJECT_TYPE_RAPTOR:
                g_nScrambleCockpitDetailX_005a86c4 =
                    (short)(g_nScrambleCockpitDetailX_005a86c4 + 2);
                break;
            case OBJECT_TYPE_SCIMITAR:
                if (frame == 0)
                    g_nScrambleCockpitDetailY_005a86c6--;
                else
                    g_nScrambleCockpitDetailX_005a86c4 =
                        (short)(g_nScrambleCockpitDetailX_005a86c4 + 2);
                break;
            }
            if (frame > 6 && *canopyFrames != 0x40) {
                g_nScrambleCanopyOffset_005a8736 = *canopyFrames;
                canopyFrames++;
            }
            if (DAT_0059ab58 == 1)
                break;
            frame++;
        } while (frame < 30);

        ((void (__cdecl *)(int, int))FlushSoundEffectsAndLog)(
            damageOffset, 0);
        if (DAT_0059ab58 != 1) {
            ClearViewport(&g_stConversationTextViewport_005a7570,
                          DAT_0046999c);
            FormatTextBufferFromStart(
                g_szLandingCommentFormat_00465bf8, 0, 160,
                DAT_004699a4,
                *(const char **)
                    ((unsigned char *)g_apszLandingDamageComments_00465ab8 +
                     damageOffset));
            DIBslam();
            DIBslamReal();
            ReleaseTextFont(0);
            WaitForSceneAdvance(300, 0);
        }
    }

    DAT_0059ab58 = 0;
    ReleasePacketHandle(g_pScrambleCockpitShape_005a8730);
    ReleasePacketHandle(g_pScrambleBackgroundShape_005a870c);
    ReleasePacketHandle(g_pScrambleShipShape_005a8750);
    ReleasePacketHandle(actorShape);
    ReleasePacketHandle(g_pScrambleDetailShape_005a86bc);
    ReleasePacketHandle(g_pScrambleOverlayShape_005a8744);
    ReleasePacketHandle(g_pScrambleCanopyShape_005a874c);
    ResetScreenClipToFullHeight();
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook(0x1d);
    return 0;
}

/* Function start: 0x408B30 (Mac symbol: funeral_player) */
unsigned int funeral_player(void)
{
    short index;
    short previousY;
    unsigned char *shape;

    DAT_00469fb4--;
    if (DAT_00469fb4 < 1) {
        DAT_00469fb4 = g_nFrameSkip_00469fb8;
        DrawConstellationField();
        g_nFuneralMainScale_005a871c =
            (short)(0x7000L / g_nFuneralMainDistance_005a8738);
        DrawSpriteDefault(&DAT_005a76b0,
                          g_nFuneralBaseX_005a8720,
                          g_nFuneralBaseY_005a8722,
                          g_pConversationSpecialShape_005a86ec, 0);
        DrawSpriteScaled(&DAT_005a76b0,
                         g_nFuneralCasketX_005a86c8,
                         g_nFuneralCasketY_005a86ca,
                         g_pConversationSpecialShape_005a86ec,
                         8, 0, g_nFuneralMainScale_005a871c, 0);
        DrawSpriteDefault(&DAT_005a76b0,
                          g_nFuneralBaseX_005a8720,
                          g_nFuneralBaseY_005a8722,
                          g_pConversationSpecialShape_005a86ec, 1);
        DrawSpriteDefault(&DAT_005a76b0,
                          g_nFuneralBaseX_005a8720,
                          g_nFuneralBaseY_005a8722,
                          g_pConversationSpecialShape_005a86ec,
                          g_nFuneralGuardFrame_005a873c);
        DrawSpriteDefault(&DAT_005a76b0,
                          g_nFuneralBaseX_005a8720,
                          g_nFuneralBaseY_005a8722,
                          g_pConversationSpecialShape_005a86ec,
                          g_nFuneralRifleFrame_005a871e);

        index = 0;
        g_nFuneralParticleScale_005a8728 =
            (short)(0x1000L / g_nFuneralParticleDistance_005a8710);
        shape = g_pConversationSpecialShape_005a86ec;
        do {
            if (g_aFuneralParticles_005a86f0[index].x != 0) {
                DrawSpriteScaled(
                    &DAT_005a76b0,
                    g_aFuneralParticles_005a86f0[index].x,
                    g_aFuneralParticles_005a86f0[index].y,
                    shape, 9, 0, g_nFuneralParticleScale_005a8728, 0);
                shape = g_pConversationSpecialShape_005a86ec;
                g_aFuneralParticles_005a86f0[index].x -= 6;
                previousY = g_aFuneralParticles_005a86f0[index].y;
                g_aFuneralParticles_005a86f0[index].y =
                    (short)(previousY - 6);
                if (DAT_005a76b0.top >
                    g_aFuneralParticles_005a86f0[index].y)
                    g_aFuneralParticles_005a86f0[index].x = 0;
            }
            index++;
        } while (index < 7);

        DrawSpriteDefault(&DAT_005a76b0,
                          g_nFuneralForegroundX_005a8718,
                          g_nFuneralForegroundY_005a871a,
                          shape, 7);
        DrawSpriteDefault(&DAT_005a76b0,
                          (short)(g_nFuneralForegroundX_005a8718 + 180),
                          g_nFuneralForegroundY_005a871a,
                          g_pConversationSpecialShape_005a86ec, 6);
        if (g_bFuneralShowTheEnd_00465b54 != 0)
            print_subtitle(&DAT_005a76b0, 56,
                           g_szTheEnd_00465c04);
        RefreshMemoryStatusOverlay();
        DIBslam();
        DIBslamReal();
    }
    return 0;
}

/* Function start: 0x408D50 (Mac symbol: funeral_wingman) */
unsigned int funeral_wingman(char *text, short duration)
{
    AddPCName(text);
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    FormatTextBufferFromStart(g_szFuneralTextFormat_00465c0c,
                              0, 160,
                              g_nConversationTextColour_00598c10,
                              g_szTextScratchBuffer_00598b00);
    DAT_00469fb4 = 1;
    SetFrameTimerPeriodDirect(duration);
    while ((short)IsFrameTickElapsed() == 0) {
        PumpWindowMessages();
        funeral_player();
        if (DAT_0059ab58 == 1)
            break;
        if (CheckEscaped() != 0)
            break;
    }
    return 0;
}

/* Function start: 0x408DE0 (Mac symbol: funeral_sequence) */
unsigned int funeral_sequence(int playerFuneral)
{
    int *packet;
    unsigned char *sceneData;
    unsigned char *textData;
    unsigned char *followupSceneData;
    unsigned char *followupTextData;
    short frame;
    short volley;
    short particle;
    short scenePair;

    PreloadMusicTrackHook(0x20);
    g_nFuneralSequenceActive_0046aa10 = 1;
    spacetrack(0x20, 1, 0);
    packet = FetchDiskPacketRetrying(
        g_asCampaignBriefingFiles_00469458[g_nCampaignDataSet_005a8118],
        0, 0);
    g_bFuneralShowTheEnd_00465b54 = 0;

    if (playerFuneral != 0) {
        scenePair = (short)(
            g_asFuneralSceneBySeries_00465b36[
                g_stCampaignState_0059ca50.currentSeries] * 2);
        followupSceneData = (unsigned char *)packet + packet[0];
        sceneData = (unsigned char *)packet + packet[scenePair + 2];
        textData = (unsigned char *)packet + packet[scenePair + 3];
        followupTextData = (unsigned char *)packet + packet[1];
        g_pIntroFont_005a8960 =
            FetchDiskPacketRetrying(9, 1, 0);
    } else {
        followupSceneData = (unsigned char *)packet + packet[10];
        sceneData = (unsigned char *)packet + packet[12];
        textData = (unsigned char *)packet + packet[13];
        followupTextData = (unsigned char *)packet + packet[11];
    }

    InitializeConversationViewport();
    g_nFuneralCasketX_005a86c8 = 180;
    g_nFuneralCasketY_005a86ca = 70;
    g_nFuneralForegroundX_005a8718 = 30;
    g_nFuneralMainDistance_005a8738 = 112;
    g_nFuneralParticleDistance_005a8710 = 16;
    g_nFuneralGuardFrame_005a873c = 2;
    g_nFuneralRifleFrame_005a871e = 4;
    g_nFuneralBaseY_005a8722 = 0;
    g_nFuneralBaseX_005a8720 = 0;
    g_nFuneralForegroundY_005a871a = 0;
    particle = 0;
    do {
        g_aFuneralParticles_005a86f0[particle].x = 0;
        particle++;
    } while (particle < 7);

    frame = 0;
    InitializeConversationText();
    init_constellation(0);
    g_pConversationSpecialShape_005a86ec =
        FetchDiskPacketRetrying(4, 9, 0);
    ClearViewport(&DAT_005a76b0, DAT_004699d8);
    InitializeConstellationField(&DAT_005a76b0, -1, 16);
    DAT_0059ab58 = 0;
    PumpWindowMessages();
    SceneDirector(3, sceneData, textData);

    if (DAT_0059ab58 != 1) {
        ClearViewport(&DAT_005a76b0, DAT_004699d8);
        ClearViewport(&g_stConversationTextViewport_005a7570,
                      DAT_0046999c);
        DAT_00469fb4 = 1;
        do {
            PumpWindowMessages();
            funeral_player();
            if (DAT_0059ab58 == 1)
                break;
            frame++;
        } while (frame < 10);

        if (DAT_0059ab58 != 1) {
            frame = 0;
            FormatTextBufferFromStart(
                g_szFuneralCompanyCommand_00465c18, 0, 160,
                DAT_004699a4);
            DAT_00469fb4 = 1;
            do {
                PumpWindowMessages();
                funeral_player();
                if (DAT_0059ab58 == 1)
                    break;
                frame++;
            } while (frame < 15);
            ClearViewport(&g_stConversationTextViewport_005a7570,
                          DAT_0046999c);

            if (DAT_0059ab58 != 1) {
                frame = 0;
                DAT_00469fb4 = 1;
                FormatTextBufferFromStart(
                    g_szFuneralAttentionCommand_00465c2c, 0, 160);
                do {
                    PumpWindowMessages();
                    funeral_player();
                    if (frame == 0)
                        PlaySfxWaveFileByNumber(0x24, -1, 0);
                    if (DAT_0059ab58 == 1)
                        break;
                    frame++;
                } while (frame < 10);

                if (DAT_0059ab58 != 1) {
                    g_nFuneralGuardFrame_005a873c = 3;
                    frame = 0;
                    DAT_00469fb4 = 1;
                    do {
                        PumpWindowMessages();
                        funeral_player();
                        if (DAT_0059ab58 == 1)
                            break;
                        frame++;
                    } while (frame < 10);

                    if (DAT_0059ab58 != 1) {
                        ClearViewport(
                            &g_stConversationTextViewport_005a7570,
                            DAT_0046999c);
                        frame = 10;
                        DAT_00469fb4 = 1;
                        FormatTextBufferFromStart(
                            g_szFuneralPrepareArmsCommand_00465c40,
                            0, 160);
                        do {
                            PumpWindowMessages();
                            funeral_player();
                            frame--;
                        } while (frame != 0);

                        frame = 0;
                        g_nFuneralRifleFrame_005a871e = 5;
                        DAT_00469fb4 = 1;
                        do {
                            PumpWindowMessages();
                            funeral_player();
                            if (frame == 0)
                                PlaySfxWaveFileByNumber(0x1f, -1, 0);
                            if (DAT_0059ab58 == 1)
                                break;
                            frame++;
                        } while (frame < 10);

                        if (DAT_0059ab58 != 1) {
                            SceneDirector(3, followupSceneData,
                                          followupTextData);
                            ClearViewport(&DAT_005a76b0,
                                          DAT_004699d8);

                            if (DAT_0059ab58 != 1) {
                                volley = 0;
                                do {
                                    ClearViewport(
                                        &g_stConversationTextViewport_005a7570,
                                        DAT_0046999c);
                                    FormatTextBufferFromStart(
                                        g_szFuneralFireCommand_00465c54,
                                        0, 160, DAT_004699a4);
                                    if (volley == 1)
                                        PlaySfxWaveFileByNumber(
                                            0x1e, -1, 0);

                                    frame = 0;
                                    DAT_00469fb4 = 1;
                                    do {
                                        PumpWindowMessages();
                                        funeral_player();
                                        if (volley > 0) {
                                            g_nFuneralCasketX_005a86c8--;
                                            if (g_nFuneralCasketX_005a86c8 %
                                                    2 == 0)
                                                g_nFuneralCasketY_005a86ca--;
                                            g_nFuneralMainDistance_005a8738++;
                                            if (g_nFuneralCasketX_005a86c8 <
                                                160) {
                                                g_nFuneralForegroundX_005a8718 +=
                                                    2;
                                                g_nFuneralBaseX_005a8720++;
                                            }
                                        }
                                        if (DAT_0059ab58 == 1)
                                            break;
                                        frame++;
                                    } while (frame < 10);

                                    ClearViewport(
                                        &g_stConversationTextViewport_005a7570,
                                        DAT_0046999c);
                                    if (DAT_0059ab58 == 1)
                                        break;

                                    g_nFuneralParticleDistance_005a8710 = 16;
                                    particle = 0;
                                    do {
                                        g_aFuneralParticles_005a86f0[particle].x =
                                            (short)(
                                                g_aFuneralParticleOrigins_00465b18[
                                                    particle].x +
                                                g_nFuneralBaseX_005a8720);
                                        g_aFuneralParticles_005a86f0[particle].y =
                                            (short)(
                                                g_aFuneralParticleOrigins_00465b18[
                                                    particle].y +
                                                g_nFuneralBaseY_005a8722);
                                        particle++;
                                    } while (particle < 7);

                                    frame = 0;
                                    PlaySfxWaveFileByNumber(0x1d, -1, 0);
                                    DAT_00469fb4 = 1;
                                    do {
                                        PumpWindowMessages();
                                        funeral_player();
                                        if (volley > 0) {
                                            g_nFuneralCasketX_005a86c8--;
                                            if (g_nFuneralCasketX_005a86c8 %
                                                    2 == 0)
                                                g_nFuneralCasketY_005a86ca--;
                                            g_nFuneralMainDistance_005a8738++;
                                            if (g_nFuneralCasketX_005a86c8 <
                                                160) {
                                                g_nFuneralForegroundX_005a8718 +=
                                                    2;
                                                g_nFuneralBaseX_005a8720++;
                                            }
                                        }
                                        g_nFuneralParticleDistance_005a8710++;
                                        if (DAT_0059ab58 == 1)
                                            break;
                                        frame++;
                                    } while (frame < 24);

                                    if (DAT_0059ab58 == 1)
                                        break;
                                    volley++;
                                } while (volley < 3);

                                if (DAT_0059ab58 != 1) {
                                    frame = 0;
                                    DAT_00469fb4 = 1;
                                    SetMusBreakpt(0, 0);
                                    while (DAT_0059ab58 == 0) {
                                        PumpWindowMessages();
                                        funeral_player();
                                        g_nFuneralCasketX_005a86c8--;
                                        if (g_nFuneralCasketX_005a86c8 % 2 ==
                                            0)
                                            g_nFuneralCasketY_005a86ca--;
                                        frame++;
                                        g_nFuneralMainDistance_005a8738++;
                                        g_nFuneralBaseX_005a8720++;
                                        g_nFuneralForegroundX_005a8718 += 2;
                                        g_nFuneralParticleDistance_005a8710++;
                                        if (frame == 110 &&
                                            playerFuneral != 0)
                                            g_bFuneralShowTheEnd_00465b54 = 1;

                                        if (DAT_0046a9f8 == 0 ||
                                            g_nWaitForMusicEnabled_0046aa30 == 0 ||
                                            DAT_0046a9f8 == 3) {
                                            if (frame > 160)
                                                break;
                                        } else if (GetMusicMode() != 0) {
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (playerFuneral != 0)
        FreePacketAndClear(&g_pIntroFont_005a8960, 0);
    ReleasePacketHandle(packet);
    ReleasePacketHandle(g_pConversationSpecialShape_005a86ec);
    free_constellation();
    ReleaseTextFont(0);
    ResetScreenClipToFullHeight();
    FadeViewportPaletteToColour(&DAT_005a6ba0, DAT_0046999c, 1);
    ClearViewport(&DAT_005a6ba0, DAT_0046999c);
    RestoreGamePalette();
    DAT_0059ab58 = 0;
    ClearInputKeyStatePreservingModifiers();
    FlushInputEvents();
    g_nFuneralSequenceActive_0046aa10 = 0;
    StopMusicUnlessSuppressed();
    free_inflight_music();
    ReleaseMusicTrackHook(0x20);
    return 0;
}

/* Function start: 0x4094E0 */
unsigned int RunAnimationDemoLoop(signed char animation)
{
    switch (animation) {
    case 0:
        init_3Space_objects((short)g_stCampaignState_0059ca50.currentSeries);
        InitializeCockpitResources(0);
        death_sequence();
        WaitForInputKey();
        break;
    case 1:
        init_3Space_objects((short)g_stCampaignState_0059ca50.currentSeries);
        InitializeCockpitResources(0);
        LaunchPlayerShip();
        WaitForInputKey();
        break;
    case 2:
        Briefing((short)g_stCampaignState_0059ca50.currentSeries,
                 (short)g_stCampaignState_0059ca50.currentMission);
        break;
    case 3:
        PlayScrambleHangarScene();
        break;
    case 4:
        scramble();
        WaitForInputKey();
        break;
    case 5:
        init_3Space_objects((short)g_stCampaignState_0059ca50.currentSeries);
        ShowCarrierLaunchSequence(1);
        free_constellation();
        free_3Space();
        WaitForInputKey();
        break;
    case 6:
        landing(3);
        WaitForInputKey();
        break;
    case 7:
        Title_Sequence();
        break;
    case 8:
        RecRoom();
        break;
    case 10:
        DeBriefing((short)g_stCampaignState_0059ca50.currentSeries,
                   (short)g_stCampaignState_0059ca50.currentMission);
        break;
    case 11:
        funeral_sequence(1);
        break;
    case 12:
        AwardCampaignMedal(g_nConversationMedalIndex_00598c08);
        break;
    case 13:
        BarracksScreen();
        break;
    }

    switch (animation) {
    case 14:
        landing((signed char)(animation - 14));
        WaitForInputKey();
        break;
    case 15:
        Office();
        break;
    case 16:
        ShowCampaignVictorySequence();
        break;
    case 17:
        ShowTigerClawEscapeScene();
        break;
    case 18:
        ShowMeanwhileTransition(0, 0);
        WaitForInputKey();
        ShowMeanwhileTransition(0, 1);
        WaitForInputKey();
        break;
    case 19:
        ShowMeanwhileTransition(1, 0);
        WaitForInputKey();
        ShowMeanwhileTransition(1, 1);
        WaitForInputKey();
        break;
    case 20:
        ShowMeanwhileTransition(2, 0);
        WaitForInputKey();
        ShowMeanwhileTransition(2, 1);
        WaitForInputKey();
        break;
    case 21:
        ShowMeanwhileTransition(3, 0);
        WaitForInputKey();
        ShowMeanwhileTransition(3, 1);
        WaitForInputKey();
        break;
    case 22:
        funeral_sequence(0);
        break;
    case 23:
        ShowMeanwhileTransition(4, 1);
        WaitForInputKey();
        break;
    case 24:
        ShowMeanwhileTransition(5, 0);
        WaitForInputKey();
        ShowMeanwhileTransition(5, 1);
        WaitForInputKey();
        break;
    case 25:
        ShowMeanwhileTransition(6, 0);
        WaitForInputKey();
        break;
    }
    exit_squadron("Animation demo over.");
    return 0;
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

    if (equ_vector(destination, &g_aShipMissionSpot_0059dd10[obj])) {
        if (range < 5000) {
            reset_tactic(obj, TACTIC_HEAD_HOME);
            set_special(obj, SPECIAL_MANEUVER_KILL_ENGINES);
            zero_vector(&g_aShipVelocity_0059c010[obj]);
        }
        return;
    }

    if (range < 1500) {
        objective = g_abFlightPath_0059c000[
            g_abShipNavPointIndex_0059d7c0[obj]];
        if (g_aMissionObjectives_0059dac0[objective].type != 1)
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
                g_aMissionObjectives_0059dac0[objective].position;
        } else {
            get_first_follow_point(obj, &g_aShipDestination_0059d530[obj]);
        }
        break;
    case TACTIC_CRUISE:
        cruise_home(obj);
        break;
    case TACTIC_HEAD_HOME:
        if (no_goal(obj) != 0)
            point_parallel(obj, find_ship_index(g_nHomeMissionShipIndex_005a8692));
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
        g_acShipTarget_0059ce60[obj] = newTarget;
    else if (target_valid(obj) == 0)
        select_target(obj);
    return g_acShipTarget_0059ce60[obj];
}

/* Function start: 0x409B10 */
short check_destroy_target(short obj)
{
    short destroyTarget = find_ship_index(g_anShipMissionShip_0059d4b0[obj]);
    int determination;

    if (destroyTarget == -1) {
        g_acShipTarget_0059ce60[obj] = check_engage_target(obj);
    } else if (g_aeObjectClass_0059d100[destroyTarget] ==
                   OBJECT_CLASS_FUTURION ||
               gone_ship(g_anShipMissionShip_0059d4b0[obj]) != 0) {
        check_engage_target(obj);
    } else {
        determination = 70;
        determination -= MaxShort(
            0, MinShort(4, (short)g_aiPilotLevel_0059cf30[obj])) * 15;
        if (evaluate_damage(obj) > determination) {
            g_acShipTarget_0059ce60[obj] = destroyTarget;
            if (g_aeShipSide_0059d650[destroyTarget] ==
                g_aeShipSide_0059d650[obj])
                g_acShipTarget_0059ce60[obj] = -1;
        } else if (target_valid(obj) != 0 && RandomBelow(100) > 3) {
            check_engage_target(obj);
        } else {
            g_acShipTarget_0059ce60[obj] = destroyTarget;
        }
    }
    return g_acShipTarget_0059ce60[obj];
}

/* Function start: 0x409C20 */
void maneuvering(short obj, short newTarget)
{
    g_acShipTarget_0059ce60[obj] = newTarget;
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
            engage(obj, g_acShipTarget_0059ce60[obj],
                   OBJECTIVE_DESTROY_SHIP);
        else
            engage(obj, g_acShipTarget_0059ce60[obj],
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
               message_showing() == 0 && DAT_0046c03c == 0) {
        send_message(obj, 2);
        g_nEnemySighting_00465c7c = g_nCurrentWave_0046c01c;
    }

    if (g_aeSpecialManeuver_0059c3c0[obj] == SPECIAL_MANEUVER_NONE &&
        distance_from_object(obj, leader) > 9000) {
        if (facing_to_object(obj, &g_aShipPosition_0059c490[leader]) > 85 &&
            real_velocity(obj) < 110) {
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
            engage(obj, g_acShipTarget_0059ce60[obj],
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
        engage(obj, g_acShipTarget_0059ce60[obj], objective);

    switch (g_aeShipObjective_0059d200[obj]) {
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
    if (g_aeShipSide_0059d650[obj] == SIDE_IMPERIAL) {
        imperial_wingman(obj);
        return;
    }
    kilrathi_wingman(obj);
}

/* Function start: 0x40A160 */
short dist_from_home(short obj)
{
    return distance_from_point(obj, &g_aShipMissionSpot_0059dd10[obj]);
}

/* Function start: 0x40A180 */
short scan_and_lock(short obj, int scanRange, enum ShipTactic newTactic)
{
    g_acShipTarget_0059ce60[obj] = scan_for_enemy(obj, 14000);
    if (g_acShipTarget_0059ce60[obj] != -1)
        g_aeShipTactic_0059d5e0[obj] = newTactic;
    return g_acShipTarget_0059ce60[obj] != -1;
}

/* Function start: 0x40A1C0 */
void patrol_area(short obj)
{
    short target = g_acShipTarget_0059ce60[obj];

    switch (g_aeShipTactic_0059d5e0[obj]) {
    case TACTIC_HEAD_HOME:
        approach_cruise_speed(obj);
        if (scan_and_lock(obj, 14000, TACTIC_APPROACH_TARGET) == 0) {
            ship_vs_point(obj, &g_aShipMissionSpot_0059dd10[obj]);
            if (g_nTargetRange_0059ce10 < 3000) {
                reset_tactic(obj, TACTIC_LOOK_OUT);
                return;
            }
            point_ship_at_point(obj, &g_aShipMissionSpot_0059dd10[obj]);
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
        maneuvering(obj, check_engage_target(obj));
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
        g_acShipTarget_0059ce60[obj] = scan_for_enemy(obj, 15000);

    if (g_acShipTarget_0059ce60[obj] == -1) {
        approach_cruise_speed(obj);
    } else {
        get_facing_range_from_object(obj,
            g_acShipTarget_0059ce60[obj]);
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
        if (equ_vector(destination, &g_aShipMissionSpot_0059dd10[obj])) {
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
        g_acShipTarget_0059ce60[obj] = possibleTarget;
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
        g_aeObjectClass_0059d100[goal] != OBJECT_CLASS_FUTURION)
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
    if (g_abShipTurn_0059d860[obj] % 10 == 0 &&
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
        g_acShipTarget_0059ce60[obj] = target;
        if (target != -1) {
            engage(obj, target, OBJECTIVE_ENGAGE_ENEMY);
        } else {
            approach_half_speed(obj);
            if (no_goal(obj) != 0)
                point_perpendicular(obj, master);
        }
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
        /* The retail source tests the array address rather than this ship's
         * side, leaving the Imperial arm present but unreachable. */
        if (g_aeShipSide_0059d650 == 0)
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
    if (g_acShipAiCooldown_0059d680[obj] > 0)
        g_acShipAiCooldown_0059d680[obj]--;
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
        g_acShipTarget_0059ce60[obj] = g_nTargetShip_0059c3b0;
        fire(obj, g_nTargetShip_0059c3b0);
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

    g_nTargetShip_0059c3b0 = g_acShipTarget_0059ce60[obj];
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
    if (unactive(g_acShipTarget_0059ce60[obj]) != 0) {
        select_target(obj);
        if (unactive(g_acShipTarget_0059ce60[obj]) != 0)
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
    for (other = 0; other <= 9; other++) {
        if (other == obj || g_aeObjectClass_0059d100[other] <
                            OBJECT_CLASS_SHIP)
            continue;
        distance = distance_from_object(obj, other);
        if (distance < g_aObjectTypeData_00466458[
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
            g_asViableTargetDistance_0059c470[viableIndex] = range;
            g_acViableTarget_0059c920[viableIndex] = other;
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
                        g_acViableTarget_0059c920[candidate]] ==
                        OBJECT_CLASS_CAPITAL_SHIP ||
                    g_abShipExhaustHeat_0059d610[
                        g_acViableTarget_0059c920[candidate]] == heat) {
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
                    g_acViableTarget_0059c920[candidate] = other;
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

/* Function start: 0x40B670 */
void set_sphere_point(const MissionShipRecord *record,
                      FixedVector *position)
{
    AddFixedVectors(
        &g_aMissionNavPoints_0046c2f0[record->navPoint].position,
        &record->position, position);
}

/* Function start: 0x40B6A0 */
unsigned int is_alive(int pilot)
{
    if (pilot <= 4)
        return 1;
    if (pilot == 13)
        return g_nArcadeState_00469fb0 != 4;
    if (pilot >= 5 && pilot <= 12)
        return g_stCampaignState_0059ca50.personalityDeathMission[
            pilot - 5] == 0;
    if (pilot >= 14 && pilot <= 17)
        return (unsigned int)ace_status((short)(pilot - 14), 1);
    return 0;
}

/* Function start: 0x40B700 */
unsigned int check_futurion(short i)
{
    unsigned int prev;

    if (g_aeShipMissionType_0059c3f0[i] == MISSION_TYPE_WARP_ARRIVE) {
        prev = g_aeObjectClass_0059d100[i];
        g_aeObjectClass_0059d100[i] = OBJECT_CLASS_FUTURION;
        g_asObjectCounter_0059c330[i] = prev;
    }
    return 0;
}

/* Function start: 0x40B730 */
unsigned int init_mission(short series, short mission)
{
    LoadMissionData(series, mission);
    init_3Space_objects(series);
    g_nSceneResourceBudget_005a7ce4 = LoadPacketResourceList(
        g_aMissionResourceDescriptors_00469c20, 0,
        g_nAvailableGameMemory_005a7ce0);
    g_aObjectTypeData_00466458[OBJECT_TYPE_DEBRIS_WING].shapeSet =
        g_aObjectTypeData_00466458[OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet;
    prepare_mission();
    InitializeCockpitResources(
        (signed char)(series == 0 ? 4 :
            g_stCampaignState_0059ca50.playerShipType));
    return 0;
}

/* Function start: 0x40B7A0 */
void prepare_mission(void)
{
    MissionShipRecord *playerRecord;
    short initial;
    short missionShip;
    int pilot;

    g_stCampaignState_0059ca50.missionScore = 0;
    g_nWingmanKilledThisMission_005a7cb4 = 0;
    g_bPlayerDestroyed_005a7c98 = 0;
    g_nWingmanKillCount_005a7cb8 = 0;
    g_nMissionMedalScore_005a8116 = 0;
    g_nPlayerKillCount_005a7c9c = 0;

    playerRecord =
        &g_aMissionShips_0046c948[g_nPlayerMissionShipIndex_005a8694];
    g_stCampaignState_0059ca50.playerShipType = playerRecord->type;
    load_ship(g_stCampaignState_0059ca50.playerShipType, 0);
    set_objects_data(0, g_stCampaignState_0059ca50.playerShipType, -1);
    playerRecord->navPoint = g_nMissionEntryNavPoint_005a8690;
    if (g_nTrainSimActive_00469e2c == 0 && DAT_0046a010 != -1)
        playerRecord->navPoint = DAT_0046a010;
    Set_up_ship_info(0, g_nPlayerMissionShipIndex_005a8694, -1);

    memset(g_acPlayerComponentDamage_0059bff0, 0,
           sizeof(g_acPlayerComponentDamage_0059bff0));
    DAT_00465c84 = 1;
    g_nYourWingman_0046c04c = -1;
    initial = 0;
    do {
        missionShip = g_nInitialMissionShipIndices_005a8696[initial];
        if (missionShip != -1) {
            pilot = g_aMissionShips_0046c948[missionShip].behaviour.pilot;
            if (is_alive(pilot) != 0 &&
                find_ships_sphere(missionShip) == -1) {
                init_ship(missionShip, g_nMissionEntryNavPoint_005a8690);
                if (g_nYourWingman_0046c04c == -1 &&
                    pilot > 4 && pilot < 14)
                    g_nYourWingman_0046c04c = DAT_0046c010;
            }
        }
        initial++;
    } while (initial < 8);
    DAT_00465c84 = 0;

    Build_objective_list();
    missionShip = 0;
    g_nCarrierMissionShipIndex_005a7e2a = missionShip;
#ifdef WC1_SDL
    /* The original tests the limit after reading each record.  If no carrier
     * exists, it increments to 64 and reads one record beyond the declared
     * table before leaving the index at 64.  Preserve that result without the
     * invalid read in the sanitizer-enabled port. */
    while (missionShip < WC1_MISSION_SHIP_COUNT &&
           g_aMissionShips_0046c948[missionShip].type !=
               OBJECT_TYPE_TIGERS_CLAW) {
#else
    while (g_aMissionShips_0046c948[missionShip].type !=
               OBJECT_TYPE_TIGERS_CLAW &&
           missionShip < WC1_MISSION_SHIP_COUNT) {
#endif
        missionShip++;
        g_nCarrierMissionShipIndex_005a7e2a = missionShip;
    }
    g_nTargetLockMode_0046c078 = 0;
    g_bLandingAuthorized_00468ff8 = 0;
}

/* Function start: 0x40B940 */
int release_all_capital_ship_shapes(void)
{
    short obj;

    obj = 0;
    do {
        if (g_aeObjectClass_0059d100[obj] ==
            OBJECT_CLASS_CAPITAL_SHIP) {
            FreePacketAndClear(&g_apObjectShape_0059d2f0[obj], 0);
            g_asCapitalShipViewFrame_0059dd90[obj] = -1;
        }
        obj++;
    } while (obj < 10);
    return 0;
}

/* Function start: 0x40B990 */
int release_capital_ship_shapes(enum ObjectType type)
{
    short obj;

    if (g_aObjectTypeData_00466458[type].objectClass ==
        OBJECT_CLASS_CAPITAL_SHIP) {
        obj = 1;
        do {
            if (g_aeObjectType_0059b560[obj] == type) {
                FreePacketAndClear(&g_apObjectShape_0059d2f0[obj], 0);
                g_asCapitalShipViewFrame_0059dd90[obj] = -1;
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
        if (g_aObjectResourceSlots_0059ddf0[slot].shapeSet == 0) {
            g_aObjectResourceSlots_0059ddf0[slot].type = type;
            g_cObjectResourceLogicalFile_005a86b0 =
                (signed char)(type + 22);
            if (type == OBJECT_TYPE_ASTEROID_FIELD) {
                g_aObjectTypeData_00466458[
                    OBJECT_TYPE_ROCK_CHUNK].shapeSet =
                    FetchDiskPacketRetrying(3, 13, 0);
                g_aObjectTypeData_00466458[
                    OBJECT_TYPE_ASTEROID5].shapeSet =
                    FetchDiskPacketRetrying(3, 16, 0);
                g_aObjectTypeData_00466458[
                    OBJECT_TYPE_ASTEROID3].shapeSet =
                    g_aObjectTypeData_00466458[
                        OBJECT_TYPE_ASTEROID5].shapeSet;
                g_aObjectTypeData_00466458[
                    OBJECT_TYPE_ASTEROID1].shapeSet =
                    g_aObjectTypeData_00466458[
                        OBJECT_TYPE_ASTEROID5].shapeSet;
                g_aObjectResourceSlots_0059ddf0[slot].shapeSet =
                    g_aObjectTypeData_00466458[
                        OBJECT_TYPE_ASTEROID5].shapeSet;
                if (g_nMemoryConfiguration_005a7cd4 == 2) {
                    g_aObjectTypeData_00466458[
                        OBJECT_TYPE_ASTEROID6].shapeSet =
                        FetchDiskPacketRetrying(
                            3, 17, 0);
                    g_aObjectTypeData_00466458[
                        OBJECT_TYPE_ASTEROID4].shapeSet =
                        g_aObjectTypeData_00466458[
                            OBJECT_TYPE_ASTEROID6].shapeSet;
                    g_aObjectTypeData_00466458[
                        OBJECT_TYPE_ASTEROID2].shapeSet =
                        g_aObjectTypeData_00466458[
                            OBJECT_TYPE_ASTEROID6].shapeSet;
                }
                obj = 10;
                do {
                    if (g_aeObjectClass_0059d100[obj] ==
                        OBJECT_CLASS_ASTEROID) {
                        g_apObjectShape_0059d2f0[obj] =
                            g_aObjectTypeData_00466458[
                                g_aeObjectType_0059b560[obj]].shapeSet;
                    }
                    obj++;
                } while (obj <= WC1_SPACE_LAST_MOVING_OBJECT);
                return 0;
            }
            if (type != OBJECT_TYPE_MINE_FIELD) {
                if (g_aObjectTypeData_00466458[type].objectClass !=
                        OBJECT_CLASS_SHIP &&
                    g_aObjectTypeData_00466458[type].objectClass !=
                        OBJECT_CLASS_MISSILE) {
                    if (DAT_0059a856 != 0) {
                        section = 0;
                        do {
                            g_aapPacketReferences_00465c88[slot]
                                [section] =
                                FetchDiskPacketRetrying(
                                    (short)
                                        g_cObjectResourceLogicalFile_005a86b0,
                                    section, 4);
                            if (g_aapPacketReferences_00465c88[slot]
                                    [section] == 0)
                                break;
                            section++;
                        } while (section < 0x25);
                    }
                    g_aObjectResourceSlots_0059ddf0[slot].shape =
                        FetchDiskPacketRetrying(
                            (short)g_cObjectResourceLogicalFile_005a86b0,
                            0x25, 0);
                    g_aObjectTypeData_00466458[type].shape =
                        g_aObjectResourceSlots_0059ddf0[slot].shape;
                    obj = 1;
                    do {
                        if (g_aeObjectType_0059b560[obj] == type) {
                            FreePacketAndClear(
                                &g_apObjectShape_0059d2f0[obj], 0);
                            g_asCapitalShipViewFrame_0059dd90[obj] = -1;
                        }
                        obj++;
                    } while (obj < 10);
                    return 0;
                }

                g_aObjectResourceSlots_0059ddf0[slot].shapeSet =
                    FetchDiskPacketRetrying(
                        (short)g_cObjectResourceLogicalFile_005a86b0,
                        0, 0);
                g_aObjectTypeData_00466458[type].shapeSet =
                    g_aObjectResourceSlots_0059ddf0[slot].shapeSet;
                g_aObjectResourceSlots_0059ddf0[slot].animation =
                    FetchDiskPacketRetrying(
                        (short)g_cObjectResourceLogicalFile_005a86b0,
                        2, 0);
                g_aObjectTypeData_00466458[type].animation =
                    g_aObjectResourceSlots_0059ddf0[slot].animation;
                g_aObjectResourceSlots_0059ddf0[slot].shape =
                    FetchDiskPacketRetrying(
                        (short)g_cObjectResourceLogicalFile_005a86b0,
                        1, 0);
                g_aObjectTypeData_00466458[type].shape =
                    g_aObjectResourceSlots_0059ddf0[slot].shape;
                obj = 0;
                do {
                    if (g_aeObjectClass_0059d100[obj] >=
                            OBJECT_CLASS_MISSILE &&
                        g_aeObjectType_0059b560[obj] == type) {
                        g_apObjectShape_0059d2f0[obj] =
                            g_aObjectResourceSlots_0059ddf0[slot]
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
    ObjectResourceSlot *resource;
    ObjectTypeData *typeData;
    enum ObjectType type;
    short obj;
    short section;

    resource = &g_aObjectResourceSlots_0059ddf0[slot];
    type = (enum ObjectType)resource->type;
    typeData = &g_aObjectTypeData_00466458[type];

    if (typeData->objectClass == OBJECT_CLASS_CAPITAL_SHIP) {
        release_capital_ship_shapes(type);
        if (DAT_0059a856 != 0) {
            section = 0;
            do {
                FreePacketAndClear(
                    &g_aapPacketReferences_00465c88[slot][section],
                    4);
                section++;
            } while (section < 0x25);
        }
        FreePacketAndClear(&resource->shape, 0);
        typeData->shape = 0;
    }
    if (resource->shapeSet == 0)
        return 0;

    FreePacketAndClear(&resource->shapeSet, 0);
    if (type == OBJECT_TYPE_ASTEROID_FIELD) {
        FreePacketAndClear(
            &g_aObjectTypeData_00466458[OBJECT_TYPE_ROCK_CHUNK].shapeSet,
            0);
        FreePacketAndClear(
            &g_aObjectTypeData_00466458[OBJECT_TYPE_ASTEROID2].shapeSet,
            0);
        g_aObjectTypeData_00466458[OBJECT_TYPE_ASTEROID6].shapeSet = 0;
        g_aObjectTypeData_00466458[OBJECT_TYPE_ASTEROID5].shapeSet = 0;
        g_aObjectTypeData_00466458[OBJECT_TYPE_ASTEROID4].shapeSet = 0;
        g_aObjectTypeData_00466458[OBJECT_TYPE_ASTEROID3].shapeSet = 0;
        g_aObjectTypeData_00466458[OBJECT_TYPE_ASTEROID1].shapeSet = 0;
        obj = 10;
        do {
            if (g_aeObjectType_0059b560[obj] == OBJECT_TYPE_ROCK_CHUNK)
                remove_object(obj);
            else if (g_aeObjectClass_0059d100[obj] ==
                     OBJECT_CLASS_ASTEROID)
                g_apObjectShape_0059d2f0[obj] = 0;
            obj++;
        } while (obj <= WC1_SPACE_LAST_MOVING_OBJECT);
        return 0;
    }

    if (type != OBJECT_TYPE_MINE_FIELD) {
        typeData->shapeSet = 0;
        FreePacketAndClear(&resource->animation, 0);
        typeData->animation = 0;
        FreePacketAndClear(&resource->shape, 0);
        typeData->shape = 0;
        obj = 0;
        do {
            if (g_aeObjectClass_0059d100[obj] >=
                    OBJECT_CLASS_MISSILE &&
                g_aeObjectType_0059b560[obj] == type)
                g_apObjectShape_0059d2f0[obj] = 0;
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
        if (g_aObjectResourceSlots_0059ddf0[slot].type != -1)
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
            g_aObjectResourceSlots_0059ddf0[slot].type;
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
        if (g_aObjectResourceSlots_0059ddf0[slot].type == -1)
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
                g_aObjectResourceSlots_0059ddf0[slot].type == type)
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

    GetScreenUpdateFlag();
    slot = 1;
    release_all_capital_ship_shapes();
    do {
        resource = &g_aObjectResourceSlots_0059ddf0[slot];
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

    g_nCurrentNavPoint_0059df60 = navPoint;
    nav = &g_aMissionNavPoints_0046c2f0[navPoint];
    g_nCurrentWave_0046c01c =
        (short)((((g_aMissionNavPoints_0046c2f0[navPoint + 1].type == 2) ?
                  -1 : 0) & 3) - 1);
    g_nEnemySighting_00465c7c = 0x7fff;

    obj = 1;
    do {
        if (g_aeObjectClass_0059d100[obj] != OBJECT_CLASS_NULL &&
            g_acShipSpawnNavPoint_0059ded0[obj] != -1) {
            if (g_aeObjectClass_0059d100[obj] >= OBJECT_CLASS_SHIP &&
                g_aeShipMissionType_0059c3f0[obj] == MISSION_TYPE_ROUT) {
                g_aMissionShips_0046c948[
                    g_nShipMissionIndices_0059c830[obj]].state = 3;
            }
            if (g_aeObjectClass_0059d100[obj] ==
                OBJECT_CLASS_CAPITAL_SHIP) {
                FreePacketAndClear(&g_apObjectShape_0059d2f0[obj], 0);
            }
            if (g_asObjectScreenX_0059d9b0[obj] != -0x7fff)
                explode(-1, obj);
            else
                remove_object(obj);
        }
        obj++;
    } while (obj < 10);
    remove_all_hazards();
    g_nHazardFieldCount_0059c90c = 0;
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
            g_aMissionNavPoints_0046c2f0[
                ((signed char *)nav->triggers)[trigger + 1]].type =
                triggerType;
        }
        trigger += 2;
    } while (trigger < 8);

    objective = 0;
    while (objective < g_cMissionObjectiveCount_0059c46a) {
        LocateMobileObjective(objective);
        objective++;
    }
    clean_up_cockpit();
    g_bLandingAuthorized_00468ff8 = 0;
    return 0;
}

/* Function start: 0x40C150 */
void free_pilot_talk(short personality)
{
    short line;

    line = 0;
    do {
        if (g_aapszPilotSpeech_0059e220[personality][line] != 0) {
            FreePacketAndClear(
                &g_aapszPilotSpeech_0059e220[personality][line], 0);
        }
        if (g_apCommPortraitShapes_0059e180[personality] != 0) {
            FreePacketAndClear(
                &g_apCommPortraitShapes_0059e180[personality], 8);
        }
        line++;
    } while (line < 11);
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
    file = OpenDataFileOrDie(g_pDiskFileRecords_005a7cf0[13].name);
    loaded = (short)(file >= 0);
    line = 0;
    do {
        loaded = (short)(loaded &
            (ReadDataFileAtOffset(
                (unsigned short)file,
                (personality * 11 + line) * 80,
                80, speech) == 1));
        if (loaded != 0) {
            g_aapszPilotSpeech_0059e220[personality][line] =
                AllocateTaggedMemory(strlen(speech) + 1, 0);
            DosStrcpy(g_aapszPilotSpeech_0059e220[personality][line], speech);
        }
        line++;
    } while (line < 11);

    CloseDataFile((unsigned short)file);
    if (loaded == 0)
        ShowModalMessage(g_szErrorLoadingPilotSpeech_00466010);
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
            g_aMissionShips_0046c948[missionShip].behaviour.pilot - 5;
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
    } while (missionShip < WC1_ACTIVE_MISSION_SHIP_COUNT);

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
        random_radial(&g_aShipPosition_0059c490[0], maximum,
                      &g_aShipPosition_0059c490[obj]);
    } while (room_for_me(obj, minimum) == 0);
}

/* Function start: 0x40C3C0 */
void set_up_next_wave(void)
{
    MissionNavPoint *waveNav;
    short previousWave;
    short entry;

    if (g_nTrainSimActive_00469e2c != 0) {
        spacetrack(21, 2, 0);
        g_nArcadeBonusCountdown_0046a014 = 60;
        if (g_nCurrentWave_0046c01c != -1)
            g_nArcadeBonusCountdown_0046a014 = 30;
        GetArcadeBonus();
        FigureArcadeTime();
    }

    if (g_nCurrentWave_0046c01c == -1 ||
        g_nCannedSceneMode_00469fac != 0)
        return;

    /* The original indexes through a base biased one MissionNavPoint before
     * g_aMissionNavPoints.  Preserve that -1 when expressing it as a typed
     * array index: arcade wave 2 is stored in nav record 1. */
    waveNav = &g_aMissionNavPoints_0046c2f0[
        g_nCurrentNavPoint_0059df60 + g_nCurrentWave_0046c01c - 1];
    previousWave = g_nCurrentWave_0046c01c;
    g_nCurrentWave_0046c01c++;
    if (waveNav->type == (signed char)previousWave) {
        new_sphere_shapes(waveNav);
        waveNav->type = -1;
        entry = 0;
        do {
            approve_xyz(
                init_ship(waveNav->missionShips[entry],
                          g_nCurrentNavPoint_0059df60),
                5000, 10000);
            entry++;
        } while (entry < 10);
        return;
    }
    g_nCurrentWave_0046c01c = -1;
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

    if (record->formationIndex == -1)
        return 0;

    source = obj;
    leaderRecord = record;
    while (leaderRecord->leaderMissionIndex != -1) {
        source = find_ship_index(leaderRecord->leaderMissionIndex);
        leaderRecord = &g_aMissionShips_0046c948[
            leaderRecord->leaderMissionIndex];
    }

    sub_int_vector(
        &g_aaFormationPositions_00465ed8[record->formationIndex]
                                               [record->formationSpot],
        &g_aaFormationPositions_00465ed8[leaderRecord->formationIndex]
                                               [leaderRecord->formationSpot],
        &g_aShipFormationOffset_0059b520[obj]);
    if (source == 0 && DAT_00465c84 == 0)
        return 0;

    copy_frame(source, obj);
    set_sphere_point(leaderRecord, &g_aShipPosition_0059c490[obj]);
    offset_location(obj, &g_aShipFormationOffset_0059b520[obj],
                    &g_aShipPosition_0059c490[obj]);
    g_anShipSpeed_0059b320[obj] = leaderRecord->speed << 8;
    return 0;
}

/* Function start: 0x40C5E0 */
void Set_up_ship_info(short obj, short missionShip, signed char navPoint)
{
    MissionShipRecord *record = &g_aMissionShips_0046c948[missionShip];

    g_asCapitalShipViewFrame_0059dd90[obj] = -1;
    g_acWingmanMessageState_0059d2c0[obj] = -1;
    DAT_0059c910[obj] = -1;
    g_asActionCount_0059c930[obj] = 0;
    g_abShipExhaustHeat_0059d610[obj] = 0;
    g_asShipAccumulatedDamage_0059dee0[obj] = 0;
    g_acShipDamage_0059c460[obj] = 0;
    g_asCannedCommand_0059d4e0[obj] = 0;
    g_acShipIonDriveDamage_0059d4a0[obj] = 0;
    g_acShipDestroyedWeaponCount_0059de30[obj] = 0;
    g_acShipCommunicator_0059c850[obj] = 0;
    g_apCannedSequence_0059dce0[obj] = 0;
    g_acShipSpawnNavPoint_0059ded0[obj] = navPoint;
    g_nShipMissionIndices_0059c830[obj] = missionShip;
    g_acShipPointingMode_0059d790[obj] = 1;

    set_sphere_point(record, &g_aShipPosition_0059c490[obj]);
    alter_yaw((short)-record->pitch, obj);
    alter_pitch((short)-record->yaw, obj);
    alter_roll(record->roll, obj);
    g_aeShipSide_0059d650[obj] = record->side;
    g_anShipSpeed_0059b320[obj] = record->speed << 8;
    g_aiPilotLevel_0059cf30[obj] = record->behaviour.pilot;
    reset_mission_type(obj, record->missionType);
    g_anShipMissionShip_0059d4b0[obj] = record->targetMissionIndex;
    g_asShipWingLeader_0059d400[obj] =
        find_ship_index(record->leaderMissionIndex);
    set_formation_position(obj, record);
    zero_vector(&g_aShipVelocity_0059c010[obj]);
    init_intelligence_data(obj);
}

/* Function start: 0x40C740 */
unsigned int is_team_member(short missionShip)
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

/* Function start: 0x40C780 */
unsigned int find_next_ship_turn_slot(short obj)
{
    signed char interval;
    int objIndex;
    short other;

    objIndex = obj;
    other = 1;
    g_acTurnRegulator_0059cf10[objIndex] = 1;
    interval = g_anPilotTurnInterval_00465fc8[
        g_aiPilotLevel_0059cf30[objIndex]];
    g_acTurnInterval_0059d7d0[objIndex] = interval;
    do {
        if (g_aeObjectClass_0059d100[other] == OBJECT_CLASS_SHIP &&
            other != obj &&
            g_acTurnRegulator_0059cf10[other] ==
                g_acTurnRegulator_0059cf10[objIndex] &&
            g_acTurnInterval_0059d7d0[other] == interval) {
            other = 1;
            g_acTurnRegulator_0059cf10[objIndex]++;
            if (interval < g_acTurnRegulator_0059cf10[objIndex])
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
    FixedVector center;
    short obj;

    if (missionShip == -1)
        return -1;
    record = &g_aMissionShips_0046c948[missionShip];
    if (record->type == OBJECT_TYPE_ASTEROID_FIELD ||
        record->type == OBJECT_TYPE_MINE_FIELD) {
        AddFixedVectors(&g_aMissionNavPoints_0046c2f0[navPoint].position,
                        &record->position, &center);
        add_hazard_field(record->type, center,
                         (short)(record->speed + 3000),
                         (short)record->behaviour.pilot);
        return -1;
    }
    obj = find_ship_index(missionShip);
    if (obj != -1 || record->state != 0)
        return -1;
    if (record->missionType != MISSION_TYPE_CANNED_SEQUENCE &&
        is_alive(record->behaviour.pilot) == 0) {
        if (record->behaviour.pilot < 9)
            return -1;
        record->behaviour.pilot = 3;
    }
    record->navPoint = navPoint;
    if (is_team_member(missionShip) != 0)
        navPoint = -1;
    obj = initialize_ship(record->type, -1);
    if (obj != -1) {
        Set_up_ship_info(obj, missionShip, (signed char)navPoint);
        find_next_ship_turn_slot(obj);
        check_futurion(obj);
    }
    return obj;
}

/* Function start: 0x40C950 */
unsigned int init_intelligence_data(short obj)
{
    FixedVector *missionSpot;
    short missionTarget;

    g_abShipTurn_0059d860[obj] = 0;
    clear_alert(obj);
    missionTarget = g_anShipMissionShip_0059d4b0[obj];
    missionSpot = &g_aShipMissionSpot_0059dd10[obj];
    g_aeSpecialManeuver_0059c3c0[obj] = SPECIAL_MANEUVER_NONE;
    *missionSpot = g_aMissionNavPoints_0046c2f0[
        g_nCurrentNavPoint_0059df60].position;

    switch (g_aeShipMissionType_0059c3f0[obj]) {
    case MISSION_TYPE_ESCORT:
    case MISSION_TYPE_STRIKE:
    case MISSION_TYPE_DEFEND:
    case MISSION_TYPE_WINGMAN:
        g_anShipMissionShip_0059d4b0[obj] = missionTarget;
        break;
    case MISSION_TYPE_GOTO_WARP:
        *missionSpot =
            g_aMissionNavPoints_0046c2f0[missionTarget].position;
        break;
    case MISSION_TYPE_WARP_ARRIVE:
        g_aeShipTactic_0059d5e0[obj] = TACTIC_WARP_IN;
        g_aeShipManeuver_0059dcb0[obj] = MANEUVER_WARPING_IN;
        if (g_aeShipSide_0059d650[obj] == SIDE_KILRATHI)
            break;
        /* fall through */
    case MISSION_TYPE_COME_HOME:
        locate_ship(g_nHomeMissionShipIndex_005a8692, missionSpot);
        break;
    case MISSION_TYPE_CANNED_SEQUENCE:
        g_apCannedSequence_0059dce0[obj] =
            g_aMissionShips_0046c948[
                g_nShipMissionIndices_0059c830[obj]].behaviour.cannedSequence;
        g_aiPilotLevel_0059cf30[obj] = 2;
        advance_canned_sequence(obj);
        break;
    default:
        break;
    }

    if (g_aiPilotLevel_0059cf30[obj] < 5)
        g_acShipRating_0059cd80[obj] =
            (signed char)~RATING_PROVINCIAL;
    else
        g_acShipRating_0059cd80[obj] =
            (signed char)(g_aiPilotLevel_0059cf30[obj] -
                          RATING_ACE_SPIRIT);
    g_acShipStress_0059d620[obj] = 0;
    return 0;
}

/* Function start: 0x40CAA0 */
short __stdcall SampleBothJoysticks(InputDeviceSample *samples,
                                    unsigned int fallback)
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
                                   unsigned int fallback)
{
    InputDeviceSample *sample;
    int result;
    short sampleIndex;

    sampleIndex = (short)(joystick != 0);
    sample = &samples[sampleIndex];
    result = GetJoystickPosition((unsigned int *)&sample->x,
                                 (unsigned int *)&sample->y,
                                 &sample->buttons, joystick, fallback);
    if (result != 0)
        g_nActiveInputDevice_005a819c = -1;
    return result;
}

/* Function start: 0x40CB30 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
void SampleActiveJoystickDevice(void)
{
    short device;

    device = g_nActiveInputDevice_005a819c;
    if (device != -1)
        SampleJoystickDevice(&g_aInputDeviceSamples_005a81f0[device],
                             device, 0);
}

/* Function start: 0x40CB60 */
void DrawNavTextLine(unsigned char alignment, unsigned short colour,
                     const char *format, ...)
{
    g_pCurrentTextContext_0059af8c->colour = colour;
    g_pCurrentTextContext_0059af8c->alignment = alignment;
    g_pCurrentTextContext_0059af8c->textCursor =
        g_pCurrentTextContext_0059af8c->text;
#ifdef WC1_SDL
    {
        va_list arguments;

        va_start(arguments, format);
        FormatTextTokens((void (__stdcall *)(int))AppendTextCharacter,
                         format, &arguments);
        va_end(arguments);
    }
#else
    FormatTextTokens((void (__stdcall *)(int))AppendTextCharacter,
                     format, (unsigned long *)(&format + 1));
#endif
    DrawTextString(g_pCurrentTextContext_0059af8c->text);
}

/* Function start: 0x40CBB0 */
void SetNavMapCoordinateScaling(short enabled)
{
    g_nNavMapCoordinateScaling_00468660 = enabled;
}

/* Function start: 0x40CBC0 */
void ScaleNavMapMarkerSize(short *size)
{
    *size = (short)(*size / (g_nNavMapScale_00468664 * 100));
}

/* Function start: 0x40CBE0 */
void ScaleNavMapCoordinates(short *x, short *y, short mapX, short mapY)
{
    *x = (short)((mapX - g_nNavMapCentreX_005a8152) /
                 g_nNavMapScale_00468664 + 75);
    *y = (short)((g_nNavMapCentreY_005a817c - mapY) /
                 g_nNavMapScale_00468664 + 67);
}

/* Function start: 0x40CC30 */
void nav_getxy(short *x, short *y, int worldX, int worldZ)
{
    short mapY;

    *x = (short)((worldX / 100) >> 8);
    mapY = (short)((worldZ / 100) >> 8);
    *y = mapY;
    if (g_nNavMapCoordinateScaling_00468660 != 0)
        ScaleNavMapCoordinates(x, y, *x, mapY);
}

/* Function start: 0x40CC80 */
void CheckPoint(short x, short y)
{
    g_nNavMapMinimumX_005a812e =
        MinShort(g_nNavMapMinimumX_005a812e, x);
    g_nNavMapMaximumX_005a812c =
        MaxShort(g_nNavMapMaximumX_005a812c, x);
    g_nNavMapMinimumY_005a8154 =
        MinShort(g_nNavMapMinimumY_005a8154, y);
    g_nNavMapMaximumY_005a8150 =
        MaxShort(g_nNavMapMaximumY_005a8150, y);
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
    g_nNavMapMinimumX_005a812e = g_aMissionObjectives_0059dac0[0].mapX;
    g_nNavMapMaximumX_005a812c = g_aMissionObjectives_0059dac0[0].mapX;
    g_nNavMapMinimumY_005a8154 = g_aMissionObjectives_0059dac0[0].mapY;
    g_nNavMapMaximumY_005a8150 = g_aMissionObjectives_0059dac0[0].mapY;
    objectiveIndex = 0;
    while (objectiveIndex < (short)g_cMissionObjectiveCount_0059c46a) {
        if (mobile_objective(objectiveIndex) != 0) {
            ship = find_ship_index((short)
                g_aMissionObjectives_0059dac0[objectiveIndex].index);
            if (ship != -1)
                g_aMissionObjectives_0059dac0[objectiveIndex].position =
                    g_aShipPosition_0059c490[ship];
        }
        nav_getxy(
            &g_aMissionObjectives_0059dac0[objectiveIndex].mapX,
            &g_aMissionObjectives_0059dac0[objectiveIndex].mapY,
            g_aMissionObjectives_0059dac0[objectiveIndex].position.x,
            g_aMissionObjectives_0059dac0[objectiveIndex].position.z);
        CheckPoint(g_aMissionObjectives_0059dac0[objectiveIndex].mapX,
                   g_aMissionObjectives_0059dac0[objectiveIndex].mapY);
        objectiveIndex++;
    }
    IncludeNavMapWorldPoint(g_aShipPosition_0059c490[0].x,
                            g_aShipPosition_0059c490[0].z);
    width = (short)(g_nNavMapMaximumX_005a812c -
                    g_nNavMapMinimumX_005a812e);
    height = (short)(g_nNavMapMaximumY_005a8150 -
                     g_nNavMapMinimumY_005a8154);
    halfWidth = (short)(width / 2);
    g_nNavMapCentreX_005a8152 =
        (short)(g_nNavMapMinimumX_005a812e + halfWidth);
    halfHeight = (short)(height / 2);
    g_nNavMapCentreY_005a817c =
        (short)(g_nNavMapMinimumY_005a8154 + halfHeight);
    g_nNavMapScale_00468664 = MaxShort(
        (short)((width + halfWidth) / 150),
        (short)((halfHeight + height) / 135));
    if (g_nNavMapScale_00468664 == 0)
        g_nNavMapScale_00468664 = 100;
    SetNavMapCoordinateScaling(1);
}

/* Function start: 0x40CED0 */
void Build_objective_list(void)
{
    MissionObjectiveSource *source;
    MissionShipRecord *ship;
    FixedVector position;
    const char *displayName;
    int type;
    short flightPathCount;

    flightPathCount = 0;
    SetNavMapCoordinateScaling(0);
    g_cMissionObjectiveCount_0059c46a = 0;
    source = g_aMissionObjectiveSources_005a8270;
    type = source->type;
    while (type != -1) {
        g_aMissionObjectives_0059dac0[
            g_cMissionObjectiveCount_0059c46a].flags = 0;
        if (type == 0) {
            position = g_aMissionNavPoints_0046c2f0[
                source->index].position;
            displayName = g_aMissionNavPoints_0046c2f0[
                source->index].name;
            g_abFlightPath_0059c000[flightPathCount++] =
                g_cMissionObjectiveCount_0059c46a;
        } else if (type >= 1 && type <= 4) {
            ship = &g_aMissionShips_0046c948[source->index];
            displayName =
                g_aObjectTypeData_00466458[ship->type].displayName;
            set_sphere_point(ship, &position);
            g_abFlightPath_0059c000[flightPathCount++] =
                g_cMissionObjectiveCount_0059c46a;
        }

        g_aMissionObjectives_0059dac0[
            g_cMissionObjectiveCount_0059c46a].type = type;
        g_aMissionObjectives_0059dac0[
            g_cMissionObjectiveCount_0059c46a].index = source->index;
        g_aMissionObjectives_0059dac0[
            g_cMissionObjectiveCount_0059c46a].name = source->description;
        g_aMissionObjectives_0059dac0[
            g_cMissionObjectiveCount_0059c46a].position = position;
        nav_getxy(
            &g_aMissionObjectives_0059dac0[
                g_cMissionObjectiveCount_0059c46a].mapX,
            &g_aMissionObjectives_0059dac0[
                g_cMissionObjectiveCount_0059c46a].mapY,
            position.x, position.z);
        g_aMissionObjectives_0059dac0[
            g_cMissionObjectiveCount_0059c46a].displayName = displayName;
        g_cMissionObjectiveCount_0059c46a++;
        source++;
        type = source->type;
    }

    g_abFlightPath_0059c000[flightPathCount] = -1;
    g_aMissionObjectives_0059dac0[
        (unsigned char)g_cMissionObjectiveCount_0059c46a].type = -1;
    g_cCurrentNavPointIndex_0059c86c = 0;
    g_cCurrentObjective_0046c020 = 0;
    if (g_cMissionObjectiveCount_0059c46a != 0) {
        while (set_new_objective(
                   (short)g_cCurrentNavPointIndex_0059c86c) == 0)
            g_cCurrentNavPointIndex_0059c86c++;
    }
}
