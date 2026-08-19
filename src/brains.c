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
    if ((aeSpecialManeuver[target] ==
         SPECIAL_MANEUVER_NORMAL) == 9) {
        maneuver_complete(ship);
        return;
    }
    if (no_goal(ship) != 0) {
        point_ship_at_point(ship, &aShipPosition[target]);
        anRollGoal[ship] = 360;
    }
}

/* Function start: 0x406130 */
void Mwabble(short ship)
{
    short choice;

    asShipCount[ship]++;
    if (asShipCount[ship] > 20) {
        maneuver_complete(ship);
        return;
    }

    approach_full_speed(ship);
    if (no_goal(ship) != 0) {
        choice = RandomBelowOrEqual(2);
        switch (choice) {
        case 0:
            anYawGoal[ship] = signed_random(6) * 5;
            break;
        case 1:
            anPitchGoal[ship] = signed_random(6) * 5;
            break;
        default:
            anRollGoal[ship] = signed_random(10) * 5;
            break;
        }
    }
}

/* Function start: 0x4061E0 */
void advance(short ship)
{
    if (aeShipManeuver[ship] != MANEUVER_NONE)
        acShipSequence[ship] = acShipSequence[ship] + 1;
}

/* Function start: 0x406200 */
void ShipAiState35(short ship, short target)
{
    (void)target;

    switch (acShipSequence[ship]) {
    case 0:
        if (nTargetRange < 750 ||
            ++asShipCount[ship] > 10) {
            advance(ship);
            asShipCount[ship] = 0;
        }
        break;
    case 1:
        veer_random(ship, 45);
        advance(ship);
        break;
    case 2:
        if (++asShipCount[ship] > 5) {
            acShipSequence[ship] = 0;
            asShipCount[ship] = 0;
        }
        if (nTargetFacing < 75)
            advance(ship);
        break;
    case 3:
        point_ship(ship, 0, &vToTarget);
        approach_speed(ship, 0x500);
        if (nFacingToTarget > 10)
            reset_maneuver(ship, MANEUVER_TAIL_FIRE);
        if (nTargetRange > 1500 ||
            nTargetFacing > 80)
            maneuver_complete(ship);
        break;
    }
}

/* Function start: 0x406310 */
void Mfull_ahead(short ship)
{
    short count;

    approach_full_speed(ship);
    count = asShipCount[ship];
    asShipCount[ship] = count - 1;
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
        reset_maneuver(ship, acShipSequence[ship]);
}

/* Function start: 0x4063B0 */
void Mdrop_a_mine(short ship)
{
    short weapon;

    weapon = -1;
    if (nTargetRange > 1500)
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
    if (asShipCount[ship] == 0)
        asShipCount[ship] = 2;
    if (--asShipCount[ship] <= 1)
        maneuver_complete(ship);
}

/* Function start: 0x406440 */
void Mtight_loop(short ship)
{
    approach_cruise_speed(ship);
    switch (acShipSequence[ship]) {
    case 0:
        anPitchGoal[ship] = 180;
        advance(ship);
        return;
    case 1:
        if (no_goal(ship) != 0)
            advance(ship);
        approach_cruise_speed(ship);
        break;
    case 2:
        anPitchGoal[ship] = 180;
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
    switch (acShipSequence[ship]) {
    case 0:
        set_special(ship, SPECIAL_MANEUVER_KILL_ENGINES);
        break;
    case 1:
        advanceSequence = ++asShipCount[ship] > 3;
        if (advanceSequence != 0)
            asShipCount[ship] = 0;
        break;
    case 2:
        fire_super_brake(ship);
        break;
    case 3:
        advanceSequence = ++asShipCount[ship] > 3;
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
    switch (acShipSequence[ship]) {
    case 0:
        if (++asShipCount[ship] < 4) {
            approach_speed(ship, anShipSpeed[target]);
            ScaleFixedVector(&aShipForwardVector[target],
                             nTargetRange * 2, &destination);
            AddFixedVectors(&aShipPosition[target],
                            &destination, &destination);
            advanceSequence = 0;
            point_ship_at_point(ship, &destination);
        } else {
            asShipCount[ship] = 0;
        }
        break;
    case 1:
        steady_object(ship);
        if (close_behind(1000) == 0) {
            advanceSequence = 0;
            reset_maneuver(ship, MANEUVER_CHILL);
            acShipSequence[ship] = 10;
        }
        break;
    case 2:
        break;
    case 3:
        if (abs(anShipSpeed[ship] -
                anShipSpeed[target]) < 0x200) {
            advanceSequence = 0;
            approach_speed(ship, anShipSpeed[target]);
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
        if (nFacingToTarget > 85)
            fire(ship, target);
        if (++asShipCount[ship] < 6)
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
    switch (acShipSequence[ship]) {
    case 0:
        veer_random(ship, 90);
        break;
    case 1:
        ++asShipCount[ship];
        advanceSequence = 1;
        if (asShipCount[ship] <= 2)
            advanceSequence = 0;
        break;
    case 2:
        advanceSequence = nTargetFacing <= 80;
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

    switch (acShipSequence[ship]) {
    case 0:
        fire_afterburner(ship, 10);
        advance(ship);
        break;
    case 1:
        if (aeSpecialManeuver[ship] == SPECIAL_MANEUVER_NONE) {
            anYawGoal[ship] = 180;
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
    switch (acShipSequence[ship]) {
    case 0:
        fire_afterburner(ship, 10);
        advance(ship);
        break;
    default:
        if (aeSpecialManeuver[ship] ==
            SPECIAL_MANEUVER_NONE)
            maneuver_complete(ship);
        break;
    }
}

/* Function start: 0x406910 */
void Mturn_n_kick(short ship)
{
    switch (acShipSequence[ship]) {
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
        if (aeSpecialManeuver[ship] == SPECIAL_MANEUVER_NONE)
            maneuver_complete(ship);
        break;
    }
}

/* Function start: 0x406990 */
void Mroll_over(short ship)
{
    if (acShipSequence[ship] == 0) {
        advance(ship);
        anRollGoal[ship] =
            RandomBelowOrEqual(2) * 360 - 180;
    } else if (anRollGoal[ship] == 0) {
        maneuver_complete(ship);
    } else {
        approach_full_speed(ship);
    }
}

/* Function start: 0x4069F0 */
void Mhard_turn(short ship)
{
    approach_full_speed(ship);
    switch (acShipSequence[ship]) {
    case 0:
        anYawGoal[ship] =
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
    switch (acShipSequence[ship]) {
    case 0:
        anYawGoal[ship] =
            RandomBelowOrEqual(2) * 240 - 120;
        fire_afterburner(ship, 5);
        break;
    case 1:
        advanceSequence = no_goal(ship);
        approach_full_speed(ship);
        break;
    case 2:
        fire_super_brake(ship);
        anYawGoal[ship] =
            RandomBelowOrEqual(2) * 90 - 45;
        break;
    case 3:
        advanceSequence =
            aObjectTypeData[
                aeObjectType[ship]].cruiseVelocity >=
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
    switch (acShipSequence[ship]) {
    case 0:
        anYawGoal[ship] = 90;
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
    switch (acShipSequence[ship]) {
    case 0:
        anYawGoal[ship] = -90;
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
    switch (acShipSequence[ship]) {
    case 0:
        set_special(ship, SPECIAL_MANEUVER_KILL_ENGINES);
        advance(ship);
        break;
    case 1:
        anPitchGoal[ship] = 15;
        if (RandomBelow(100) < 50)
            advance(ship);
        break;
    default:
        anPitchGoal[ship] = -30;
        if (RandomBelow(100) < 50) {
            if (++asShipCount[ship] < 10)
                acShipSequence[ship] = 1;
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
        (short)((asObjectCollisionRadius[target] +
                 asObjectCollisionRadius[ship] * 6) >> 1));
    fire_when_ready(ship, 1);
}

/* Function start: 0x406D80 */
void Mzip_past(short ship, short target)
{
    if (unactive(target) == 0) {
        if (close_behind(
                asObjectCollisionRadius[target] + 2000) != 0) {
            Mtail_fire(ship, target);
            return;
        }
        approach_full_speed(ship);
        if (no_goal(ship) != 0) {
            if (nTargetFacing > 80)
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
        if (acObjectOwner[object] == ship &&
            aeObjectClass[object] == OBJECT_CLASS_MISSILE) {
            reset_maneuver(ship, MANEUVER_STRAFE_ENEMY);
            return;
        }
        object++;
    }

    if (nFacingToTarget > 85 &&
        nTargetRange < 6000 &&
        (nTargetFacing > 80 ||
         nTargetFacing < -80) &&
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
    if (nFacingToTarget > 75 &&
        nTargetRange < 6000) {
        fire_missile(ship);
        maneuver_complete(ship);
    }
}

/* Function start: 0x406F20 */
void Mbuzz_debris(short ship)
{
    switch (acShipSequence[ship]) {
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
        if (aeSpecialManeuver[ship] ==
            SPECIAL_MANEUVER_NONE) {
            maneuver_complete(ship);
        } else if (nFacingToTarget > 95) {
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
    if (abs(anPitchGoal[ship]) != 0 ||
        abs(anYawGoal[ship]) != 0)
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
    if (nTargetFacing < 0x50) {
        Mstrafe_enemy(ship, target);
        return;
    }
    Mzip_past(ship, target);
}

/* Function start: 0x407060 */
void Msit_n_fire(short ship, short target)
{
    if (aeObjectClass[target] == OBJECT_CLASS_CAPITAL_SHIP) {
        Mbest_strafe(ship, target);
        return;
    }
    if (no_goal(ship) != 0)
        point_ship_at_object(ship, target);
    if (nTargetRange > 3000)
        approach_cruise_speed(ship);
    else
        approach_zero_speed(ship);
    fire_when_ready(ship, 1);
}

/* Function start: 0x4070D0 */
void Mstrafe_n_roll(short ship, short target)
{
    if (0 < asObjectCounter[ship]) {
        anRollGoal[ship] = 0x2d;
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
    ship_vs_ship(ship, nTargetShip);
    if (nFacingToTarget < 0) {
        reset_maneuver(ship, MANEUVER_FISH_HOOK);
        Mfish_hook(ship, target);
        return;
    }
    if (nFacingToTarget < 80) {
        reset_maneuver(ship, MANEUVER_BURNOUT);
        Mburnout(ship, target);
        return;
    }
    point_ship_at_object(ship, nTargetShip);
    if (nTargetRange < 8000)
        fire(ship, acShipTarget[ship]);
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

    if (nTargetRange > 2000) {
        maneuver_complete(ship);
        return;
    }
    if (nTargetRange < 700 && normal_speed(ship) != 0)
        fire_afterburner(ship, 10);
    else
        approach_full_speed(ship);
    if (nFacingToTarget > 0 && no_goal(ship) != 0) {
        amount = MinShort(20, nFacingToTarget);
        steer_away_from_object(ship, target, amount);
    }
}

/* Function start: 0x407270 */
void general_zig(short ship, unsigned int target, short pitch)
{
    short complete = 1;

    (void)target;
    approach_full_speed(ship);
    switch (acShipSequence[ship] % 6) {
    case 0:
        anYawGoal[ship] = -35;
        anPitchGoal[ship] = pitch;
        break;
    case 1:
    case 4:
        complete = no_goal(ship);
        asShipCount[ship] = 0;
        break;
    case 2:
    case 5:
        complete = ++asShipCount[ship] >= 4;
        break;
    case 3:
        pitch = -pitch;
        anYawGoal[ship] = 35;
        anPitchGoal[ship] = pitch;
        break;
    }
    if (acShipSequence[ship] >= 12)
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
        if (--asShipCount[ship] <= 0) {
            switch (acShipSequence[ship] % 4) {
            case 0:
                anYawGoal[ship] = -20;
                break;
            case 1:
            case 3:
                anRollGoal[ship] = 20;
                break;
            case 2:
                anYawGoal[ship] = 20;
                break;
            }
            asShipCount[ship] = 4;
            advance(ship);
        }
    }
    if (acShipSequence[ship] > 8)
        maneuver_complete(ship);
}

/* Function start: 0x407450 */
void Mveer_away(short ship, short target)
{
    switch (acShipSequence[ship]) {
    case 0:
        if (nFacingToTarget > 80)
            steer_away_from_object(ship, target, 40);
        else
            steer_away_from_object(ship, target, 10);
        advance(ship);
        return;
    default:
        break;
    }
    if (asObjectCollisionRadius[target] * 3 <
        nTargetRange) {
        veer_random(ship, 8);
        maneuver_complete(ship);
        return;
    }
    if (no_goal(ship) != 0) {
        if (nFacingToTarget > 80) {
            steer_away_from_object(ship, target, 40);
        } else if (nFacingToTarget < -65 ||
                   RandomBelowOrEqual(100) < 4) {
            veer_random(ship, 16);
        }
    }
    if ((DAT_00475e78 >= nTargetRange ||
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
    acShipStress[ship] = 0;
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
    short target = acShipTarget[obj];
    enum ShipManeuver previous = aeShipManeuver[obj];
    short range;
    int maneuverWeight;

#ifdef WC1_SDL
    /* The original performs this lookup before validating the maneuver.
       MANEUVER_NONE reads the zero alignment byte at 0x00465677, immediately
       before the table.  Other invalid values are reset before the result is
       used.  Preserve those results without an invalid C array access. */
    if (previous < MANEUVER_WARPING_IN ||
        previous > MANEUVER_UNKNOWN_46)
        bCurrentManeuverReroll = 0;
    else
#endif
        bCurrentManeuverReroll =
            abManeuverRerollChance[previous];
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
    range = nTargetRange;
    if (nTargetFacing < 0)
        maneuverWeight = asObjectCollisionRadius[target] +
                         asObjectCollisionRadius[obj] * 4;
    else
        maneuverWeight = asObjectCollisionRadius[target] +
                         asObjectCollisionRadius[obj] * 6;
    SetShipAiScratchWord((unsigned short)(maneuverWeight >> 1));

    if (unactive(target) != 0) {
        if (aeShipManeuver[obj] == MANEUVER_VEER_AWAY) {
            Mveer_away(obj, target);
        } else if (aeShipManeuver[obj] == MANEUVER_GLOAT) {
#ifdef WC1_SDL
            Mgloat(obj);
#else
            ((void (__cdecl *)(short, short))Mgloat)(obj, target);
#endif
        } else if (aeShipManeuver[obj] ==
                   MANEUVER_LINE_UP_DROP) {
            Mline_up_drop(obj, target);
        } else {
            maneuver_complete(obj);
        }
    } else if (aeShipManeuver[obj] >= 0 &&
               aeShipManeuver[obj] < 47) {
#ifdef WC1_SDL
        /* The original x86 dispatcher pushes both values for every handler.
           Several handlers consume only the ship, and two consume an unsigned
           target.  Call those through their real C types in the native port. */
        switch (aeShipManeuver[obj]) {
        case MANEUVER_WARPING_IN:
        case MANEUVER_WARPING_OUT:
            Mnone();
            break;
        case MANEUVER_DRIFT:
        case MANEUVER_UNKNOWN_46:
            Mreset(obj);
            break;
        case MANEUVER_FULL_AHEAD:
            Mfull_ahead(obj);
            break;
        case MANEUVER_THINKING:
            Mthink(obj);
            break;
        case MANEUVER_KICK_STOP:
        case MANEUVER_TURN_N_KICK:
            Mturn_n_kick(obj);
            break;
        case MANEUVER_TIGHT_LOOP:
            Mtight_loop(obj);
            break;
        case MANEUVER_HARD_BRAKE:
            Mhard_break(obj);
            break;
        case MANEUVER_WABBLE:
            Mwabble(obj);
            break;
        case MANEUVER_ROLL_OVER:
            Mroll_over(obj);
            break;
        case MANEUVER_HARD_TURN:
            Mhard_turn(obj);
            break;
        case MANEUVER_SPLIT_LEFT:
            Msplit_left(obj);
            break;
        case MANEUVER_KICKIT:
            Mkickit(obj);
            break;
        case MANEUVER_OUTA_HERE:
            Mrout_me(obj);
            break;
        case MANEUVER_DROP_A_MINE:
            Mdrop_a_mine(obj);
            break;
        case MANEUVER_SPLIT_RIGHT:
            Msplit_right(obj);
            break;
        case MANEUVER_ZIG_ZAG:
            Mzig_zag(obj, (unsigned int)target);
            break;
        case MANEUVER_GLOAT:
            Mgloat(obj);
            break;
        case MANEUVER_SAFE_BRAKE:
            Mzig_zag_pitch(obj, (unsigned int)target);
            break;
        case MANEUVER_INTERCEPT:
            Mcorkscrew(obj);
            break;
        case MANEUVER_BUZZ_DEBRIS:
            Mbuzz_debris(obj);
            break;
        case MANEUVER_UNKNOWN_44:
            ShipAiState44(obj);
            break;
        default:
            apShipAiManeuverHandlers[
                aeShipManeuver[obj]](obj, target);
            break;
        }
#else
        apShipAiManeuverHandlers[
            aeShipManeuver[obj]](obj, target);
#endif
    } else {
        maneuver_complete(obj);
    }

    if (range < DAT_00475e78) {
        try2reset_maneuver(obj, 2);
    } else if (aeShipManeuver[obj] == previous &&
               RandomBelowOrEqual(100) <
                   (short)bCurrentManeuverReroll) {
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

    nFrameSkipCounter = 1;
    elapsed = 0;
    if (ticks > 0) {
        do {
            PumpWindowMessages();
            nFrameSkipCounter--;
            if (nFrameSkipCounter < 1) {
                nFrameSkipCounter = nFrameSkip;
                DrawSpriteDefault(&stSceneBuffer,
                                  nScrambleBackgroundX, 0,
                                  pScrambleHangarShape, 0);
                DrawSpriteDefault(&stSceneBuffer,
                                  (short)(nScrambleBackgroundX +
                                          320),
                                  0, pScrambleHangarShape, 1);
                DrawSpriteDefault(&stSceneBuffer,
                                  (short)(nScrambleBackgroundX +
                                          640),
                                  0, pScrambleHangarShape, 0);
                DrawSpriteDefault(&stSceneBuffer,
                                  (short)(nScrambleBackgroundX +
                                          960),
                                  0, pScrambleHangarShape, 1);

                nScrambleWalkerY =
                    127 - GetShapeFrameExtent(
                        0, 0, pScrambleHangarShape,
                        cScrambleLeftWalkerFrame, 3);
                DrawSpriteDefault(&stSceneBuffer,
                                  nScrambleLeftWalkerX,
                                  nScrambleWalkerY,
                                  pScrambleHangarShape,
                                  cScrambleLeftWalkerFrame);
                DrawSpriteDefault(
                    &stSceneBuffer, nScrambleLeftWalkerX,
                    nScrambleWalkerY,
                    pScrambleHangarShape,
                    acScrambleWalkerOverlayFrames[
                        cScrambleWalkerPair * 2]);

                nScrambleWalkerY =
                    137 - GetShapeFrameExtent(
                        0, 0, pScrambleHangarShape,
                        cScrambleRightWalkerFrame, 3);
                DrawSpriteDefault(&stSceneBuffer,
                                  nScrambleRightWalkerX,
                                  nScrambleWalkerY,
                                  pScrambleHangarShape,
                                  cScrambleRightWalkerFrame);
                DrawSpriteDefault(
                    &stSceneBuffer, nScrambleRightWalkerX,
                    nScrambleWalkerY,
                    pScrambleHangarShape,
                    acScrambleWalkerOverlayFrames[
                        cScrambleWalkerPair * 2 + 1]);
                RefreshMemoryStatusOverlay();
                DIBslam();
                DIBslamReal();

                cScrambleLeftWalkerFrame++;
                if (cScrambleLeftWalkerFrame > 19)
                    cScrambleLeftWalkerFrame = 7;
                cScrambleRightWalkerFrame++;
                if (cScrambleRightWalkerFrame > 19)
                    cScrambleRightWalkerFrame = 7;
            }
            nScrambleBackgroundX =
                nScrambleBackgroundX - 12;
            nScrambleRightWalkerX =
                nScrambleRightWalkerX + 3;
            nScrambleLeftWalkerX =
                nScrambleLeftWalkerX + 2;
            if (bEscapePressed == 1)
                break;
            elapsed++;
        } while (elapsed < ticks);
    }
    return 0;
}

/* Function start: 0x4079C0 */
unsigned int PlayScrambleHangarScene(void)
{
    if (nMemoryConfiguration == 1)
        SceneLeaveHook();
    else
        PreloadMusicTrackHook(0x1b);
    spacetrack(0x1b, 1, -1);

    InitializeConversationViewport();
    cScrambleLeftWalkerFrame = 7;
    nScrambleBackgroundX = 0;
    cScrambleRightWalkerFrame = 10;
    nScrambleLeftWalkerX = 70;
    nScrambleRightWalkerX = 170;
    pScrambleHangarShape =
        FetchDiskPacketRetrying(1, 0, 0);
    cScrambleWalkerPair =
        (signed char)(((unsigned short)rand() + 3) & 3);
    PlaySfxWaveFileByNumber(14, -1, 0);
    bEscapePressed = 0;

    AnimateScrambleWalk(24);
    if (bEscapePressed != 1) {
        cScrambleLeftWalkerFrame = 21;
        cScrambleRightWalkerFrame = 24;
        nScrambleLeftWalkerX = 90;
        nScrambleRightWalkerX = 200;
        nScrambleWalkerY = -14;
        nFrameSkipCounter = 1;
        for (cScrambleWalkTicks = 0;
             cScrambleWalkTicks < 24;
             cScrambleWalkTicks++) {
            PumpWindowMessages();
            nFrameSkipCounter--;
            if (nFrameSkipCounter < 1) {
                nFrameSkipCounter = nFrameSkip;
                DrawSpriteDefault(&stSceneBuffer,
                                  nScrambleBackgroundX, 0,
                                  pScrambleHangarShape, 2);
                DrawSpriteDefault(&stSceneBuffer,
                                  (short)(nScrambleBackgroundX +
                                          320),
                                  0, pScrambleHangarShape, 2);
                DrawSpriteDefault(&stSceneBuffer,
                                  (short)(nScrambleBackgroundX +
                                          640),
                                  0, pScrambleHangarShape, 2);
                DrawSpriteDefault(&stSceneBuffer,
                                  nScrambleLeftWalkerX,
                                  nScrambleWalkerY,
                                  pScrambleHangarShape,
                                  cScrambleLeftWalkerFrame);
                DrawSpriteDefault(&stSceneBuffer,
                                  nScrambleRightWalkerX,
                                  nScrambleWalkerY,
                                  pScrambleHangarShape,
                                  cScrambleRightWalkerFrame);
                RefreshMemoryStatusOverlay();
                DIBslam();
                DIBslamReal();

                cScrambleLeftWalkerFrame++;
                if (cScrambleLeftWalkerFrame > 26)
                    cScrambleLeftWalkerFrame = 21;
                cScrambleRightWalkerFrame++;
                if (cScrambleRightWalkerFrame > 26)
                    cScrambleRightWalkerFrame = 21;
            }
            nScrambleBackgroundX =
                (short)(nScrambleBackgroundX - 12);
            nScrambleLeftWalkerX =
                (short)(nScrambleLeftWalkerX + 2);
            nScrambleRightWalkerX =
                (short)(nScrambleRightWalkerX + 3);
            if (bEscapePressed == 1)
                break;
        }

        if (bEscapePressed != 1) {
            cScrambleLeftWalkerFrame = 7;
            cScrambleRightWalkerFrame = 10;
            AnimateScrambleWalk(24);
        }
    }

    FlushSoundEffectsAndLog();
    ReleasePacketHandle(pScrambleHangarShape);
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
    if ((animationFrame = aScrambleAnimationActors[
             actorIndex].animationFrame) != -1) {
        if (aScrambleAnimationActors[
                actorIndex].animationState != 0xa000) {
            animationFrame++;
            do {
                frame = aScrambleAnimationActors[
                    actorIndex].animation[animationFrame];
                control = (unsigned char)frame & 0xc0;
                switch (control) {
                    case 0:
                        aScrambleAnimationActors[
                            actorIndex].animationFrame++;
                        break;
                    case 0x40:
                        aScrambleAnimationActors[
                            actorIndex].animationState = 0;
                        animationFrame--;
                        break;
                    case 0x80:
                        animationFrame = frame & 0x3f;
                        aScrambleAnimationActors[
                            actorIndex].animationFrame = animationFrame;
                        break;
                }
            } while (control != 0);
        } else
            frame = animationFrame;
    }

    x = aScrambleAnimationActors[actorIndex].deltaX +
        aScrambleAnimationActors[actorIndex].x;
    aScrambleAnimationActors[actorIndex].x = x;
    y = aScrambleAnimationActors[actorIndex].deltaY +
        aScrambleAnimationActors[actorIndex].y;
    aScrambleAnimationActors[actorIndex].y = y;
    DrawSpriteScaled(
        pScrambleViewport,
        (short)(nScrambleBackgroundRightX + x),
        (short)(nScrambleBackgroundY + y),
        aScrambleAnimationActors[actorIndex].shape,
        (short)(frame +
                aScrambleAnimationActors[actorIndex].baseFrame),
        aScrambleAnimationActors[actorIndex].angle,
        aScrambleAnimationActors[actorIndex].scale,
        aScrambleAnimationActors[actorIndex].flip);
    return 0;
}

/* Function start: 0x407D90 */
unsigned int ConfigureScrambleActor(short x, short y, short deltaX,
                                    short deltaY, unsigned char *shape,
                                    short scale, short angle,
                                    signed char flip, short actorIndex)
{
    ScrambleAnimationActor *actor;

    actor = &aScrambleAnimationActors[actorIndex];
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

    nFrameSkipCounter--;
    if (nFrameSkipCounter > 0)
        return 0;
    nFrameSkipCounter = nFrameSkip;

    DrawSpriteDefault(&stSceneBuffer,
                      (short)(nScrambleBackgroundRightX - 1),
                      nScrambleBackgroundY,
                      pScrambleBackgroundShape, 0);
    DrawSpriteDefault(&stSceneBuffer,
                      nScrambleBackgroundRightX,
                      nScrambleBackgroundY,
                      pScrambleBackgroundShape, 1);
    DrawScrambleActor(0);
    DrawScrambleActor(3);
    DrawScrambleActor(4);
    DrawScrambleActor(2);

    if (bScrambleCanopyClosed == 0) {
        DrawSpriteDefault(&stSceneBuffer,
                          (short)(nScrambleShipX + 40),
                          (short)(nScrambleShipY - 40),
                          pScrambleCanopyShape,
                          nScrambleCanopyOffset);
    }

    switch (stCampaignState.playerShipType) {
    case OBJECT_TYPE_HORNET:
        DrawSpriteDefault(&stSceneBuffer,
                          (short)(nScrambleShipX + 10),
                          (short)(nScrambleShipY - 25),
                          pScrambleShipShape,
                          nScrambleCanopyFrame);
        DrawSpriteDefault(&stSceneBuffer, nScrambleShipX,
                          nScrambleShipY,
                          pScrambleCockpitShape, 0);
        DrawSpriteScaled(&stSceneBuffer,
                         nScrambleCockpitDetailX,
                         nScrambleCockpitDetailY,
                         pScrambleCockpitShape, 1, 0,
                         nScrambleCockpitScale, 0);
        break;
    case OBJECT_TYPE_RAPIER:
        DrawSpriteDefault(&stSceneBuffer,
                          (short)(nScrambleShipX + 10),
                          (short)(nScrambleShipY - 16),
                          pScrambleShipShape,
                          nScrambleCanopyFrame);
        DrawSpriteDefault(&stSceneBuffer, nScrambleShipX,
                          nScrambleShipY,
                          pScrambleCockpitShape, 0);
        DrawSpriteDefault(&stSceneBuffer,
                          (short)(nScrambleShipX + 153),
                          (short)(nScrambleShipY + 5),
                          pScrambleCockpitShape, 2);
        DrawSpriteDefault(&stSceneBuffer,
                          nScrambleCockpitDetailX,
                          nScrambleCockpitDetailY,
                          pScrambleCockpitShape, 1);
        break;
    case OBJECT_TYPE_SCIMITAR:
        DrawSpriteDefault(&stSceneBuffer,
                          (short)(nScrambleShipX + 10),
                          (short)(nScrambleShipY - 15),
                          pScrambleShipShape,
                          nScrambleCanopyFrame);
        DrawSpriteDefault(&stSceneBuffer, nScrambleShipX,
                          nScrambleShipY,
                          pScrambleCockpitShape, 0);
        DrawSpriteDefault(&stSceneBuffer,
                          (short)(nScrambleShipX + 148),
                          nScrambleShipY,
                          pScrambleCockpitShape, 2);
        DrawSpriteDefault(&stSceneBuffer,
                          nScrambleCockpitDetailX,
                          nScrambleCockpitDetailY,
                          pScrambleCockpitShape, 1);
        break;
    case OBJECT_TYPE_RAPTOR:
        DrawSpriteDefault(&stSceneBuffer,
                          (short)(nScrambleShipX + 10),
                          (short)(nScrambleShipY - 11),
                          pScrambleShipShape,
                          nScrambleCanopyFrame);
        DrawSpriteDefault(&stSceneBuffer, nScrambleShipX,
                          nScrambleShipY,
                          pScrambleCockpitShape, 0);
        DrawSpriteDefault(&stSceneBuffer,
                          (short)(nScrambleShipX + 158),
                          (short)(nScrambleShipY + 6),
                          pScrambleCockpitShape, 2);
        DrawSpriteDefault(&stSceneBuffer,
                          nScrambleCockpitDetailX,
                          nScrambleCockpitDetailY,
                          pScrambleCockpitShape, 1);
        break;
    }

    for (detailIndex = 0;
         detailIndex < nScrambleShipDetailCount;
         detailIndex++) {
        detailSlot = acScrambleShipDetailIndices[detailIndex];
        detail = &aaScrambleShipDetails[
            stCampaignState.playerShipType][detailSlot];
        DrawSpriteDefault(&stSceneBuffer,
                          (short)(nScrambleShipX + detail->x),
                          (short)(nScrambleShipY + detail->y),
                          pScrambleDetailShape,
                          detail->frame);
    }

    if (bScrambleCanopyClosed == 1) {
        if (nScrambleOverlayX != -1000) {
            DrawSpriteDefault(&stSceneBuffer,
                              nScrambleOverlayX,
                              nScrambleOverlayY,
                              pScrambleOverlayShape, 0);
        }
        DrawSpriteDefault(&stSceneBuffer, 100, 127,
                          pScrambleCanopyShape,
                          nScrambleCanopyOffset);
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

    nScrambleCanopyOffset = 0;
    nScrambleOverlayX = -1000;
    nScrambleCanopyFrame = 0;
    nScrambleShipDetailCount = 0;
    if (bEscapePressed == 0) {
        InitializeConversationViewport();
        cCockpitLogicalFile =
            (signed char)(stCampaignState.playerShipType + 17);
        pScrambleCockpitShape = FetchDiskPacketRetrying(
            cCockpitLogicalFile, 8, 0);
        pScrambleBackgroundShape =
            FetchDiskPacketRetrying(1, 1, 0);
        bScrambleCanopyClosed = 0;
        pScrambleCanopyShape =
            FetchDiskPacketRetrying(1, 2, 0);
        pScrambleShipShape =
            FetchDiskPacketRetrying(1, 3, 0);
        actorShape = FetchDiskPacketRetrying(1, 4, 0);
        nScrambleBackgroundY = 0;
        pScrambleViewport = &stSceneBuffer;
        nScrambleBackgroundRightX = 64;

        ConfigureScrambleActor(130, 94, 1, 0, actorShape,
                               0x100, 0, 0, 0);
        ConfigureScrambleActor(160, 120, 0, 0, actorShape,
                               0x100, 0, 0, 2);
        ConfigureScrambleActor(260, 100, -3, 0, actorShape,
                               0xff, 0, 0x10, 3);
        ConfigureScrambleActor(260, 100, -3, 0, actorShape,
                               0xff, 0, 0x10, 4);
        PlaySfxWaveFileByNumber(17, -1, 0);
        bEscapePressed = 0;

        switch (stCampaignState.playerShipType) {
        case OBJECT_TYPE_HORNET:
            nScrambleShipX = -40;
            nScrambleShipY = 96;
            nScrambleCockpitDetailX = -95;
            nScrambleCockpitDetailY = 71;
            nScrambleCockpitScale = 316;
            break;
        case OBJECT_TYPE_RAPIER:
            nScrambleShipX = -30;
            nScrambleShipY = 80;
            nScrambleCockpitDetailX = -15;
            nScrambleCockpitDetailY = 76;
            break;
        case OBJECT_TYPE_SCIMITAR:
            nScrambleShipX = -40;
            nScrambleShipY = 86;
            nScrambleCockpitDetailX = 4;
            nScrambleCockpitDetailY = 83;
            break;
        case OBJECT_TYPE_RAPTOR:
            nScrambleShipX = -40;
            nScrambleShipY = 80;
            nScrambleCockpitDetailX = -22;
            nScrambleCockpitDetailY = 67;
            break;
        }

        nFrameSkipCounter = 1;
        for (frame = 0; frame < 10; frame++) {
            PumpWindowMessages();
            DrawScrambleFrame();
            nScrambleBackgroundRightX--;
            nScrambleShipY =
                (short)(nScrambleShipY - 2);
            nScrambleCockpitDetailY =
                (short)(nScrambleCockpitDetailY - 2);
            nScrambleShipX =
                (short)(nScrambleShipX + 4);
            nScrambleCockpitDetailX =
                (short)(nScrambleCockpitDetailX + 4);
            if (bEscapePressed == 1)
                break;
        }

        if (bEscapePressed != 1) {
            nFrameSkipCounter = 1;
            for (frame = 0; frame < 27; frame++) {
                PumpWindowMessages();
                DrawScrambleFrame();
                nScrambleCanopyOffset++;
                nScrambleShipX =
                    (short)(nScrambleShipX + 4);
                nScrambleCockpitDetailX =
                    (short)(nScrambleCockpitDetailX + 4);
                nScrambleBackgroundRightX--;
                if (nScrambleCanopyFrame < 25)
                    nScrambleCanopyFrame++;
                if (bEscapePressed == 1)
                    break;
            }
        }

        if (bEscapePressed != 1) {
            FlushSoundEffectsAndLog();
            nScrambleCanopyOffset--;
            PlaySfxWaveFileByNumber(15, -1, 0);
            nFrameSkipCounter = 1;
            for (frame = 0; frame < 23; frame++) {
                PumpWindowMessages();
                if (frame == 22)
                    nFrameSkipCounter = 1;
                DrawScrambleFrame();
                switch (stCampaignState.playerShipType) {
                case OBJECT_TYPE_HORNET:
                    nScrambleCockpitScale =
                        (short)(nScrambleCockpitScale + 2);
                    break;
                case OBJECT_TYPE_RAPIER:
                case OBJECT_TYPE_RAPTOR:
                    nScrambleCockpitDetailX =
                        (short)(nScrambleCockpitDetailX - 2);
                    break;
                case OBJECT_TYPE_SCIMITAR:
                    if (frame == 21)
                        nScrambleCockpitDetailY++;
                    else
                        nScrambleCockpitDetailX =
                            (short)(nScrambleCockpitDetailX - 2);
                    break;
                }
                if (nScrambleCanopyFrame < 35)
                    nScrambleCanopyFrame++;
                if (bEscapePressed == 1)
                    break;
            }

            FlushSoundEffectsAndLog();
            PlaySfxWaveFileByNumber(16, -1, 0);
            if (bEscapePressed != 1)
                WaitForSceneAdvance(60, 0);
        }

        bEscapePressed = 0;
        ReleasePacketHandle(pScrambleCockpitShape);
        ReleasePacketHandle(pScrambleBackgroundShape);
        ReleasePacketHandle(pScrambleCanopyShape);
        ReleasePacketHandle(pScrambleShipShape);
        ReleasePacketHandle(actorShape);
        FlushSoundEffects();
        ResetScreenClipToFullHeight();
    }

    if (nMemoryConfiguration == 0) {
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
    nScrambleShipDetailCount =
        anLandingDamageDetailCounts[damageLevel];
    for (frame = 0;
         frame < nScrambleShipDetailCount;
         frame++) {
        do {
            detail = RandomInRange(0, 31);
            for (prior = 0; prior < frame; prior++) {
                if (acScrambleShipDetailIndices[prior] ==
                    detail)
                    break;
            }
        } while (prior < frame);
        acScrambleShipDetailIndices[frame] = detail;
    }

    cCockpitLogicalFile =
        (signed char)(stCampaignState.playerShipType + 17);
    pScrambleCockpitShape =
        FetchDiskPacketRetrying(
            cCockpitLogicalFile, 8, 0);
    pScrambleBackgroundShape =
        FetchDiskPacketRetrying(1, 1, 0);
    pScrambleShipShape =
        FetchDiskPacketRetrying(1, 3, 0);
    actorShape = FetchDiskPacketRetrying(1, 4, 0);
    pScrambleDetailShape =
        FetchDiskPacketRetrying(1, 9, 0);
    pScrambleOverlayShape =
        FetchDiskPacketRetrying(1, 5, 0);
    bScrambleCanopyClosed = 1;
    pScrambleCanopyShape =
        FetchDiskPacketRetrying(1, 6, 0);
    InitializeConversationText();
    nScrambleBackgroundY = 0;
    pScrambleViewport = &stSceneBuffer;
    nScrambleBackgroundRightX = 32;
    ConfigureScrambleActor(140, 88, 2, 0, actorShape,
                           0x80, 0, 0, 3);
    ConfigureScrambleActor(139, 88, 2, 0, actorShape,
                           0x80, 0, 0, 4);
    ConfigureScrambleActor(240, 94, -1, 0, actorShape,
                           0x100, 0, 0x10, 0);
    ConfigureScrambleActor(160, 120, 0, 0, actorShape,
                           0x100, 0, 0, 2);
    nScrambleCanopyOffset = 0;
    nScrambleCanopyFrame = 34;
    nScrambleOverlayX = -1000;

    switch (stCampaignState.playerShipType) {
    case OBJECT_TYPE_HORNET:
        nScrambleShipX = 124;
        nScrambleShipY = 140;
        nScrambleCockpitDetailX = 69;
        nScrambleCockpitDetailY = 115;
        nScrambleCockpitScale = 360;
        break;
    case OBJECT_TYPE_RAPIER:
        nScrambleShipX = 124;
        nScrambleShipY = 130;
        nScrambleCockpitDetailX = 94;
        nScrambleCockpitDetailY = 125;
        break;
    case OBJECT_TYPE_SCIMITAR:
        nScrambleShipY = 134;
        nScrambleCockpitDetailY = 132;
        nScrambleShipX = 124;
        nScrambleCockpitDetailX = 124;
        break;
    case OBJECT_TYPE_RAPTOR:
        nScrambleShipX = 124;
        nScrambleShipY = 126;
        nScrambleCockpitDetailX = 96;
        nScrambleCockpitDetailY = 113;
        break;
    }

    frame = 0;
    bEscapePressed = 0;
    PlaySfxWaveFileByNumber(17, -1, 0);
    nFrameSkipCounter = 1;
    for (; frame < 30; frame++) {
        PumpWindowMessages();
        DrawScrambleFrame();
        nScrambleShipY =
            (short)(nScrambleShipY - 2);
        nScrambleCockpitDetailY =
            (short)(nScrambleCockpitDetailY - 2);
        if (bEscapePressed == 1)
            break;
    }

    ((void (__cdecl *)(int, int))FlushSoundEffectsAndLog)(
        damageOffset, 0);
    if (bEscapePressed != 1) {
        nScrambleOverlayX =
            (short)(nScrambleShipX + 180);
        canopyFrames = *(const signed char **)
            ((unsigned char *)apLandingCanopyFrames +
             damageOffset);
        nScrambleOverlayY =
            (short)(nScrambleShipY + 50);
        frame = 0;
        PlaySfxWaveFileByNumber(15, -1, 0);
        nFrameSkipCounter = 1;
        for (; frame < 30; frame++) {
            PumpWindowMessages();
            if (nRenderedSpaceFrame == 29)
                nFrameSkipCounter = 1;
            DrawScrambleFrame();
            nScrambleOverlayY--;
            nScrambleOverlayX =
                (short)(nScrambleOverlayX - 4);
            switch (stCampaignState.playerShipType) {
            case OBJECT_TYPE_HORNET:
                nScrambleCockpitScale =
                    (short)(nScrambleCockpitScale - 2);
                break;
            case OBJECT_TYPE_RAPIER:
            case OBJECT_TYPE_RAPTOR:
                nScrambleCockpitDetailX =
                    (short)(nScrambleCockpitDetailX + 2);
                break;
            case OBJECT_TYPE_SCIMITAR:
                if (frame == 0)
                    nScrambleCockpitDetailY--;
                else
                    nScrambleCockpitDetailX =
                        (short)(nScrambleCockpitDetailX + 2);
                break;
            }
            if (frame > 6 && *canopyFrames != 0x40) {
                nScrambleCanopyOffset = *canopyFrames;
                canopyFrames++;
            }
            if (bEscapePressed == 1)
                break;
        }

        ((void (__cdecl *)(int, int))FlushSoundEffectsAndLog)(
            damageOffset, 0);
        if (bEscapePressed != 1) {
            ClearViewport(&stConversationTextViewport,
                          cBlackColour);
            FormatTextBufferFromStart(
                szLandingCommentFormat, 0, 160,
                cBlueColour,
                *(const char **)
                    ((unsigned char *)apszLandingDamageComments +
                     damageOffset));
            DIBslam();
            DIBslamReal();
            ReleaseTextFont(0);
            WaitForSceneAdvance(300, 0);
        }
    }

    bEscapePressed = 0;
    ReleasePacketHandle(pScrambleCockpitShape);
    ReleasePacketHandle(pScrambleBackgroundShape);
    ReleasePacketHandle(pScrambleShipShape);
    ReleasePacketHandle(actorShape);
    ReleasePacketHandle(pScrambleDetailShape);
    ReleasePacketHandle(pScrambleOverlayShape);
    ReleasePacketHandle(pScrambleCanopyShape);
    ResetScreenClipToFullHeight();
    ClearViewport(&stConversationTextViewport,
                  cBlackColour);
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

    nFrameSkipCounter--;
    if (nFrameSkipCounter < 1) {
        nFrameSkipCounter = nFrameSkip;
        DrawConstellationField();
        nFuneralMainScale =
            (short)(0x7000L / nFuneralMainDistance);
        DrawSpriteDefault(&stSceneBuffer,
                          nFuneralBaseX,
                          nFuneralBaseY,
                          pConversationSpecialShape, 0);
        DrawSpriteScaled(&stSceneBuffer,
                         nFuneralCasketX,
                         nFuneralCasketY,
                         pConversationSpecialShape,
                         8, 0, nFuneralMainScale, 0);
        DrawSpriteDefault(&stSceneBuffer,
                          nFuneralBaseX,
                          nFuneralBaseY,
                          pConversationSpecialShape, 1);
        DrawSpriteDefault(&stSceneBuffer,
                          nFuneralBaseX,
                          nFuneralBaseY,
                          pConversationSpecialShape,
                          nFuneralGuardFrame);
        DrawSpriteDefault(&stSceneBuffer,
                          nFuneralBaseX,
                          nFuneralBaseY,
                          pConversationSpecialShape,
                          nFuneralRifleFrame);

        index = 0;
        nFuneralParticleScale =
            (short)(0x1000L / nFuneralParticleDistance);
        shape = pConversationSpecialShape;
        for (; index < 7; index++) {
            if (aFuneralParticles[index].x != 0) {
                DrawSpriteScaled(
                    &stSceneBuffer,
                    aFuneralParticles[index].x,
                    aFuneralParticles[index].y,
                    shape, 9, 0, nFuneralParticleScale, 0);
                shape = pConversationSpecialShape;
                aFuneralParticles[index].x -= 6;
                previousY = aFuneralParticles[index].y;
                aFuneralParticles[index].y =
                    (short)(previousY - 6);
                if (stSceneBuffer.top >
                    aFuneralParticles[index].y)
                    aFuneralParticles[index].x = 0;
            }
        }

        DrawSpriteDefault(&stSceneBuffer,
                          nFuneralForegroundX,
                          nFuneralForegroundY,
                          shape, 7);
        DrawSpriteDefault(&stSceneBuffer,
                          (short)(nFuneralForegroundX + 180),
                          nFuneralForegroundY,
                          pConversationSpecialShape, 6);
        if (bFuneralShowTheEnd != 0)
            print_subtitle(&stSceneBuffer, 56,
                           szFuneralTheEnd);
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
    ClearViewport(&stConversationTextViewport,
                  cBlackColour);
    FormatTextBufferFromStart(szFuneralTextFormat,
                              0, 160,
                              nConversationTextColour,
                              szTextScratchBuffer);
    nFrameSkipCounter = 1;
    SetFrameTimerPeriodDirect(duration);
    while ((short)IsFrameTickElapsed() == 0) {
        PumpWindowMessages();
        funeral_player();
        if (bEscapePressed == 1)
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
    nFuneralSequenceActive = 1;
    spacetrack(0x20, 1, 0);
    packet = FetchDiskPacketRetrying(
        asCampaignBriefingFiles[nCampaignDataSet],
        0, 0);
    bFuneralShowTheEnd = 0;

    if (playerFuneral != 0) {
        scenePair = (short)(
            asFuneralSceneBySeries[
                stCampaignState.currentSeries] * 2);
        followupSceneData = (unsigned char *)packet + packet[0];
        sceneData = (unsigned char *)packet + packet[scenePair + 2];
        textData = (unsigned char *)packet + packet[scenePair + 3];
        followupTextData = (unsigned char *)packet + packet[1];
        pIntroFont =
            FetchDiskPacketRetrying(9, 1, 0);
    } else {
        followupSceneData = (unsigned char *)packet + packet[10];
        sceneData = (unsigned char *)packet + packet[12];
        textData = (unsigned char *)packet + packet[13];
        followupTextData = (unsigned char *)packet + packet[11];
    }

    InitializeConversationViewport();
    nFuneralCasketX = 180;
    nFuneralCasketY = 70;
    nFuneralForegroundX = 30;
    nFuneralMainDistance = 112;
    nFuneralParticleDistance = 16;
    nFuneralGuardFrame = 2;
    nFuneralRifleFrame = 4;
    nFuneralBaseY = 0;
    nFuneralBaseX = 0;
    nFuneralForegroundY = 0;
    for (particle = 0; particle < 7; particle++) {
        aFuneralParticles[particle].x = 0;
    }

    frame = 0;
    InitializeConversationText();
    init_constellation(0);
    pConversationSpecialShape =
        FetchDiskPacketRetrying(4, 9, 0);
    ClearViewport(&stSceneBuffer, cPrimaryViewBufferColour);
    InitializeConstellationField(&stSceneBuffer, -1, 16);
    bEscapePressed = 0;
    PumpWindowMessages();
    SceneDirector(3, sceneData, textData);

    if (bEscapePressed != 1) {
        ClearViewport(&stSceneBuffer, cPrimaryViewBufferColour);
        ClearViewport(&stConversationTextViewport,
                      cBlackColour);
        nFrameSkipCounter = 1;
        for (; frame < 10; frame++) {
            PumpWindowMessages();
            funeral_player();
            if (bEscapePressed == 1)
                break;
        }

        if (bEscapePressed != 1) {
            frame = 0;
            FormatTextBufferFromStart(
                szFuneralCompanyCommand, 0, 160,
                cBlueColour);
            nFrameSkipCounter = 1;
            for (; frame < 15; frame++) {
                PumpWindowMessages();
                funeral_player();
                if (bEscapePressed == 1)
                    break;
            }
            ClearViewport(&stConversationTextViewport,
                          cBlackColour);

            if (bEscapePressed != 1) {
                frame = 0;
                nFrameSkipCounter = 1;
                FormatTextBufferFromStart(
                    szFuneralAttentionCommand, 0, 160);
                for (; frame < 10; frame++) {
                    PumpWindowMessages();
                    funeral_player();
                    if (frame == 0)
                        PlaySfxWaveFileByNumber(0x24, -1, 0);
                    if (bEscapePressed == 1)
                        break;
                }

                if (bEscapePressed != 1) {
                    nFuneralGuardFrame = 3;
                    frame = 0;
                    nFrameSkipCounter = 1;
                    for (; frame < 10; frame++) {
                        PumpWindowMessages();
                        funeral_player();
                        if (bEscapePressed == 1)
                            break;
                    }

                    if (bEscapePressed != 1) {
                        ClearViewport(
                            &stConversationTextViewport,
                            cBlackColour);
                        frame = 10;
                        nFrameSkipCounter = 1;
                        FormatTextBufferFromStart(
                            szFuneralPrepareArmsCommand,
                            0, 160);
                        for (; frame != 0; frame--) {
                            PumpWindowMessages();
                            funeral_player();
                        }

                        frame = 0;
                        nFuneralRifleFrame = 5;
                        nFrameSkipCounter = 1;
                        for (; frame < 10; frame++) {
                            PumpWindowMessages();
                            funeral_player();
                            if (frame == 0)
                                PlaySfxWaveFileByNumber(0x1f, -1, 0);
                            if (bEscapePressed == 1)
                                break;
                        }

                        if (bEscapePressed != 1) {
                            SceneDirector(3, followupSceneData,
                                          followupTextData);
                            ClearViewport(&stSceneBuffer,
                                          cPrimaryViewBufferColour);

                            if (bEscapePressed != 1) {
                                for (volley = 0; volley < 3; volley++) {
                                    ClearViewport(
                                        &stConversationTextViewport,
                                        cBlackColour);
                                    FormatTextBufferFromStart(
                                        szFuneralFireCommand,
                                        0, 160, cBlueColour);
                                    if (volley == 1)
                                        PlaySfxWaveFileByNumber(
                                            0x1e, -1, 0);

                                    frame = 0;
                                    nFrameSkipCounter = 1;
                                    for (; frame < 10; frame++) {
                                        PumpWindowMessages();
                                        funeral_player();
                                        if (volley > 0) {
                                            nFuneralCasketX--;
                                            if (nFuneralCasketX %
                                                    2 == 0)
                                                nFuneralCasketY--;
                                            nFuneralMainDistance++;
                                            if (nFuneralCasketX <
                                                160) {
                                                nFuneralForegroundX +=
                                                    2;
                                                nFuneralBaseX++;
                                            }
                                        }
                                        if (bEscapePressed == 1)
                                            break;
                                    }

                                    ClearViewport(
                                        &stConversationTextViewport,
                                        cBlackColour);
                                    if (bEscapePressed == 1)
                                        break;

                                    nFuneralParticleDistance = 16;
                                    for (particle = 0;
                                         particle < 7;
                                         particle++) {
                                        aFuneralParticles[particle].x =
                                            (short)(
                                                aFuneralParticleOrigins[
                                                    particle].x +
                                                nFuneralBaseX);
                                        aFuneralParticles[particle].y =
                                            (short)(
                                                aFuneralParticleOrigins[
                                                    particle].y +
                                                nFuneralBaseY);
                                    }

                                    frame = 0;
                                    PlaySfxWaveFileByNumber(0x1d, -1, 0);
                                    nFrameSkipCounter = 1;
                                    for (; frame < 24; frame++) {
                                        PumpWindowMessages();
                                        funeral_player();
                                        if (volley > 0) {
                                            nFuneralCasketX--;
                                            if (nFuneralCasketX %
                                                    2 == 0)
                                                nFuneralCasketY--;
                                            nFuneralMainDistance++;
                                            if (nFuneralCasketX <
                                                160) {
                                                nFuneralForegroundX +=
                                                    2;
                                                nFuneralBaseX++;
                                            }
                                        }
                                        nFuneralParticleDistance++;
                                        if (bEscapePressed == 1)
                                            break;
                                    }

                                    if (bEscapePressed == 1)
                                        break;
                                }

                                if (bEscapePressed != 1) {
                                    frame = 0;
                                    nFrameSkipCounter = 1;
                                    SetMusBreakpt(0, 0);
                                    while (bEscapePressed == 0) {
                                        PumpWindowMessages();
                                        funeral_player();
                                        nFuneralCasketX--;
                                        if (nFuneralCasketX % 2 ==
                                            0)
                                            nFuneralCasketY--;
                                        frame++;
                                        nFuneralMainDistance++;
                                        nFuneralBaseX++;
                                        nFuneralForegroundX += 2;
                                        nFuneralParticleDistance++;
                                        if (frame == 110 &&
                                            playerFuneral != 0)
                                            bFuneralShowTheEnd = 1;

                                        if (nMusicPlaybackMode == 0 ||
                                            nWaitForMusicEnabled == 0 ||
                                            nMusicPlaybackMode == 3) {
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
        FreePacketAndClear(&pIntroFont, 0);
    ReleasePacketHandle(packet);
    ReleasePacketHandle(pConversationSpecialShape);
    free_constellation();
    ReleaseTextFont(0);
    ResetScreenClipToFullHeight();
    FadeViewportPaletteToColour(&stScreen, cBlackColour, 1);
    ClearViewport(&stScreen, cBlackColour);
    RestoreGamePalette();
    bEscapePressed = 0;
    ClearInputKeyStatePreservingModifiers();
    FlushInputEvents();
    nFuneralSequenceActive = 0;
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
        init_3Space_objects((short)stCampaignState.currentSeries);
        InitializeCockpitResources(0);
        death_sequence();
        WaitForInputKey();
        break;
    case 1:
        init_3Space_objects((short)stCampaignState.currentSeries);
        InitializeCockpitResources(0);
        LaunchPlayerShip();
        WaitForInputKey();
        break;
    case 2:
        Briefing((short)stCampaignState.currentSeries,
                 (short)stCampaignState.currentMission);
        break;
    case 3:
        PlayScrambleHangarScene();
        break;
    case 4:
        scramble();
        WaitForInputKey();
        break;
    case 5:
        init_3Space_objects((short)stCampaignState.currentSeries);
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
        DeBriefing((short)stCampaignState.currentSeries,
                   (short)stCampaignState.currentMission);
        break;
    case 11:
        funeral_sequence(1);
        break;
    case 12:
        AwardCampaignMedal(nConversationMedalIndex);
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

    if (abandoned(obj, 0) != 0 || (abShipTurn[obj] & 7) != 5)
        return;

    if (obj == nYourWingman &&
        distance_from_object(obj, 0) > 16000)
        remove_object(obj);

    if (aeObjectClass[obj] == OBJECT_CLASS_CAPITAL_SHIP)
        approach_cruise_speed(obj);
    else if (normal_speed(obj) != 0)
        fire_afterburner(obj, 10);

    destination = &aShipDestination[obj];
    if (no_goal(obj) != 0)
        point_ship_at_point(obj, destination);
    range = distance_from_point(obj, destination);

    if (equ_vector(destination, &aShipMissionSpot[obj])) {
        if (range < 5000) {
            reset_tactic(obj, TACTIC_HEAD_HOME);
            set_special(obj, SPECIAL_MANEUVER_KILL_ENGINES);
            zero_vector(&aShipVelocity[obj]);
        }
        return;
    }

    if (range < 1500) {
        objective = abFlightPath[
            abShipNavPointIndex[obj]];
        if (aMissionObjectives[objective].type != 1)
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

    switch (aeShipTactic[obj]) {
    case TACTIC_NONE:
        reset_tactic(obj, TACTIC_CRUISE);
        if (aeShipSide[obj] == SIDE_IMPERIAL &&
            aeShipMissionType[obj] == MISSION_TYPE_ROUT) {
            objective = find_objective(1, -1);
            aShipDestination[obj] =
                aMissionObjectives[objective].position;
        } else {
            get_first_follow_point(obj, &aShipDestination[obj]);
        }
        break;
    case TACTIC_CRUISE:
        cruise_home(obj);
        break;
    case TACTIC_HEAD_HOME:
        if (no_goal(obj) != 0)
            point_parallel(obj, find_ship_index(nHomeMissionShipIndex));
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

    if (unactive(asShipWingLeader[obj]) == 0 &&
        aeShipMissionType[
            asShipWingLeader[obj]] == MISSION_TYPE_ROUT) {
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

    if (newTarget != -1 && newTarget != acShipTarget[obj])
        acShipTarget[obj] = newTarget;
    else if (target_valid(obj) == 0)
        select_target(obj);
    return acShipTarget[obj];
}

/* Function start: 0x409B10 */
short check_destroy_target(short obj)
{
    short destroyTarget = find_ship_index(anShipMissionShip[obj]);
    int determination;

    if (destroyTarget == -1) {
        acShipTarget[obj] = check_engage_target(obj);
    } else if (aeObjectClass[destroyTarget] ==
                   OBJECT_CLASS_FUTURION ||
               gone_ship(anShipMissionShip[obj]) != 0) {
        check_engage_target(obj);
    } else {
        determination = 70;
        determination -= MaxShort(
            0, MinShort(4, (short)aiPilotLevel[obj])) * 15;
        if (evaluate_damage(obj) > determination) {
            acShipTarget[obj] = destroyTarget;
            if (aeShipSide[destroyTarget] ==
                aeShipSide[obj])
                acShipTarget[obj] = -1;
        } else if (target_valid(obj) != 0 && RandomBelow(100) > 3) {
            check_engage_target(obj);
        } else {
            acShipTarget[obj] = destroyTarget;
        }
    }
    return acShipTarget[obj];
}

/* Function start: 0x409C20 */
void maneuvering(short obj, short newTarget)
{
    acShipTarget[obj] = newTarget;
    intelligence_events(obj);
    perform_maneuver(obj);
}

/* Function start: 0x409C50 */
void formation_burst(short obj)
{
    approach_full_speed(obj);
    if (no_goal(obj) != 0)
        point_ship(obj, 0, &aShipDestination[obj]);
    asShipCount[obj]++;
    if (asShipCount[obj] > 9) {
        if (aeShipMissionType[obj] == MISSION_TYPE_STRIKE)
            engage(obj, acShipTarget[obj],
                   OBJECTIVE_DESTROY_SHIP);
        else
            engage(obj, acShipTarget[obj],
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
    short leader = asShipWingLeader[obj];

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

    if (aeSpecialManeuver[obj] == SPECIAL_MANEUVER_NONE &&
        distance_from_object(obj, leader) > 9000) {
        if (facing_to_object(obj, &aShipPosition[leader]) > 85 &&
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
    switch (acShipSequence[obj]) {
    case 0:
        steady_object(obj);
        anYawGoal[obj] = -30;
        anRollGoal[obj] = -45;
        anPitchGoal[obj] = -20;
        acShipSequence[obj]++;
        break;
    case 1:
        if (no_goal(obj) != 0)
            engage(obj, acShipTarget[obj],
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
    switch (aeShipObjective[obj]) {
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
    short leader = asShipWingLeader[obj];
    enum ShipObjective objective;

    if (leader == -1) {
        change_mission_type(obj, MISSION_TYPE_PATROL);
        return;
    }
    if (unactive(leader) != 0) {
        inherit_leader(obj);
        return;
    }
    objective = aeShipObjective[leader];
    if ((objective == OBJECTIVE_ENGAGE_ENEMY ||
         objective == OBJECTIVE_DESTROY_SHIP) &&
        aeShipObjective[obj] != objective)
        engage(obj, acShipTarget[obj], objective);

    switch (aeShipObjective[obj]) {
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
    return distance_from_point(obj, &aShipMissionSpot[obj]);
}

/* Function start: 0x40A180 */
short scan_and_lock(short obj, int scanRange, enum ShipTactic newTactic)
{
    acShipTarget[obj] = scan_for_enemy(obj, 14000);
    if (acShipTarget[obj] != -1)
        aeShipTactic[obj] = newTactic;
    return acShipTarget[obj] != -1;
}

/* Function start: 0x40A1C0 */
void patrol_area(short obj)
{
    short target = acShipTarget[obj];

    switch (aeShipTactic[obj]) {
    case TACTIC_HEAD_HOME:
        approach_cruise_speed(obj);
        if (scan_and_lock(obj, 14000, TACTIC_APPROACH_TARGET) == 0) {
            ship_vs_point(obj, &aShipMissionSpot[obj]);
            if (nTargetRange < 3000) {
                reset_tactic(obj, TACTIC_LOOK_OUT);
                return;
            }
            point_ship_at_point(obj, &aShipMissionSpot[obj]);
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
            if (nTargetRange < 10000) {
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
    switch (aeShipObjective[obj]) {
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
        aeShipObjective[obj] = OBJECTIVE_WANDER;
        aeShipTactic[obj] = TACTIC_APPROACH_TARGET;
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
    if ((abShipTurn[obj] & 7) == 6)
        acShipTarget[obj] = scan_for_enemy(obj, 15000);

    if (acShipTarget[obj] == -1) {
        approach_cruise_speed(obj);
    } else {
        get_facing_range_from_object(obj,
            acShipTarget[obj]);
        if (nFacingToTarget <= 65)
            approach_full_speed(obj);
        else
            approach_half_speed(obj);
    }

    if ((abShipTurn[obj] & 7) != 2)
        return;
    destination = &aShipDestination[obj];
    if (no_goal(obj) != 0)
        point_ship_at_point(obj, destination);
    range = distance_from_point(obj, destination);
    if (range < 1500) {
        if (aeShipSide[obj] == SIDE_IMPERIAL)
            flag_reached((short)abFlightPath[
                abShipNavPointIndex[obj]], 1);
        if (equ_vector(destination, &aShipMissionSpot[obj])) {
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

    if (anShipSpeed[obj] != 0) {
        set_special(obj, SPECIAL_MANEUVER_STOP_DRIFT);
        return;
    }
    count = ++asShipCount[obj];
    delay = aeShipSide[obj] == SIDE_KILRATHI ? 250 : 25;
    if (count <= delay)
        return;

    get_facing_range_from_object(obj, 0);
    if (nFacingToTarget > 90 && no_goal(obj) != 0) {
        anYawGoal[obj] = signed_random(30);
        return;
    }
    delay = aeShipSide[obj] == SIDE_KILRATHI ? 270 : 45;
    if (asShipCount[obj] > delay ||
        (nTargetFacing > 80 && nTargetRange < 6000)) {
        reset_tactic(obj, TACTIC_WARP_OUT);
        fire_afterburner(obj, 10);
    }
}

/* Function start: 0x40A630 */
void accelerate_and_jump(short obj)
{
    approach_full_speed(obj);
    if (asShipCount[obj]++ == 4)
        warp(obj);
}

/* Function start: 0x40A670 */
void reach_warp(short obj)
{
    switch (aeShipTactic[obj]) {
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
        get_first_follow_point(obj, &aShipDestination[obj]);
        break;
    default:
        fail(obj);
        break;
    }
}

/* Function start: 0x40A710 */
void warp_arrival(short obj)
{
    if (aeShipTactic[obj] == TACTIC_WARP_IN)
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
    short buddy = find_ship_index(anShipMissionShip[obj]);

    if (unactive(buddy) != 0) {
        change_mission_type(obj, MISSION_TYPE_PATROL);
        return;
    }
    if ((abShipTurn[obj] & 3) == 0 && in_danger(buddy) != 0 &&
        nTargetRange < 3000)
        engage(obj, nTargetShip,
               OBJECTIVE_ENGAGE_ENEMY);
    if (aeShipObjective[obj] != OBJECTIVE_HOME_BASE &&
        (abShipTurn[obj] & 7) == 4 &&
        distance_from_object(obj, buddy) > 5000)
        reset_objective(obj, OBJECTIVE_HOME_BASE);

    switch (aeShipObjective[obj]) {
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
    if (gone_ship(anShipMissionShip[obj]) != 0)
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

    if (aeObjectClass[goal] != OBJECT_CLASS_FUTURION) {
        determination = 70;
        determination -= MaxShort(
            0, MinShort(4, (short)aiPilotLevel[obj])) * 15;
        if (evaluate_damage(obj) > determination && range > 5000) {
            streak_toward(obj, goal, (short)range);
            return;
        }
    }
    possibleTarget = scan_for_enemy(obj, 10000);
    possibleRange = (unsigned short)nTargetRange;
    if (possibleTarget != -1 &&
        (possibleRange * 3 < range ||
         aeObjectClass[goal] == OBJECT_CLASS_FUTURION)) {
        init_formation_burst(obj);
        acShipTarget[obj] = possibleTarget;
    } else if (range < 5000) {
        engage(obj, goal, OBJECTIVE_DESTROY_SHIP);
    } else {
        streak_toward(obj, goal, (short)range);
    }
}

/* Function start: 0x40AAC0 */
void strike_mission(short obj)
{
    short goal = find_ship_index(anShipMissionShip[obj]);

#ifdef WC1_SDL
    /* The original source indexes class[-1] here.  In the Win32 layout that
       aliases the last two previous-distance words; the intended no-goal
       branch is check_goal. */
    if (goal == -1)
#else
    if (goal == -1 &&
        aeObjectClass[goal] != OBJECT_CLASS_FUTURION)
#endif
        check_goal(obj);
    switch (aeShipObjective[obj]) {
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
    short master = find_ship_index(anShipMissionShip[obj]);
    short target;

    if (master == -1) {
        change_mission_type(obj, MISSION_TYPE_PATROL);
        return;
    }
    if (abShipTurn[obj] % 10 == 0 &&
        in_danger(master) != 0 && nTargetRange < 6000 &&
        aeShipObjective[obj] != OBJECTIVE_ENGAGE_ENEMY)
        engage(obj, nTargetShip,
               OBJECTIVE_ENGAGE_ENEMY);
    if (aeShipObjective[obj] != OBJECTIVE_HOME_BASE &&
        (abShipTurn[obj] & 7) == 4 &&
        distance_from_object(obj, master) > 10000)
        reset_objective(obj, OBJECTIVE_HOME_BASE);

    switch (aeShipObjective[obj]) {
    case OBJECTIVE_HOME_BASE:
        return_to_master(obj, master);
        break;
    case OBJECTIVE_WANDER:
        target = scan_for_enemy(obj, 7000);
        acShipTarget[obj] = target;
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
    short goal = find_ship_index(anShipMissionShip[obj]);

    if (unactive(goal) != 0) {
        change_mission_type(obj, MISSION_TYPE_PATROL);
        return;
    }
    switch (aeShipObjective[obj]) {
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
    if (regulate_turn(obj) != 0)
        return;
    switch (aeShipMissionType[obj]) {
    case MISSION_TYPE_PATROL:
        /* The retail source tests the array address rather than this ship's
         * side, leaving the Imperial arm present but unreachable. */
        if (aeShipSide == 0)
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
    if (acShipAiCooldown[obj] > 0)
        acShipAiCooldown[obj]--;
}

/* Function start: 0x40AF70 */
void orbit_sphere(short obj)
{
    short radius = aMissionNavPoints[
        nCurrentNavPoint].proximityRadius >> 1;
    FixedVector center =
        aMissionNavPoints[nCurrentNavPoint].position;
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
        acShipTarget[obj] = nTargetShip;
        fire(obj, nTargetShip);
        if (no_goal(obj) != 0) {
            if (RandomBelowOrEqual(4) == 0) {
                anYawGoal[obj] = signed_random(90);
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
        acShipTarget[obj] = -1;
        approach_half_speed(obj);
    } else {
        approach_cruise_speed(obj);
    }
    orbit_sphere(obj);
}

/* Function start: 0x40B110 */
void stationary_intelligence(short obj)
{
    if (aeObjectType[obj] == OBJECT_TYPE_KILRATHI_BASE) {
        anObjectYawRotation[obj] = 4;
        fire_turrets(obj);
    }
}

/* Function start: 0x40B140 */
void capital_ship_intelligence(short obj)
{
    enum ObjectType type;

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

    type = aeObjectType[obj];
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

    nTargetShip = acShipTarget[obj];
    if (unactive(nTargetShip) != 0)
        scan_for_enemy(obj, 15000);
    if (aeShipTactic[obj] != TACTIC_SELF_DEFENSE) {
        if (nTargetShip != -1) {
            approach_full_speed(obj);
            aeShipTactic[obj] = TACTIC_SELF_DEFENSE;
            acShipTarget[obj] =
                (signed char)nTargetShip;
            fire_turrets(obj);
        } else {
            approach_cruise_speed(obj);
        }
        return;
    }

    approach_full_speed(obj);
    if (unactive(acShipTarget[obj]) != 0) {
        select_target(obj);
        if (unactive(acShipTarget[obj]) != 0)
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
    range = nTargetRange;
    count = ++asActionCount[obj];
    if (range > 1000 && count > 1000) {
        aeObjectClass[obj] =
            (enum ObjectClass)asObjectCounter[obj];
        return;
    }
    if (count > 200 && range < 4000 && range > 1000 &&
        nFacingToTarget > 80)
        aeObjectClass[obj] =
            (enum ObjectClass)asObjectCounter[obj];
}

/* Function start: 0x40B3A0 */
void mine_intelligence(short obj)
{
    short other;
    short distance;

    if (asObjectCounter[obj] != -1)
        return;
    for (other = 0; other <= 9; other++) {
        if (other == obj || aeObjectClass[other] <
                            OBJECT_CLASS_SHIP)
            continue;
        distance = distance_from_object(obj, other);
        if (distance < aObjectTypeData[
                           aeObjectType[obj]].collisionRadius ||
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

    if (nFacingToTarget >= 0 &&
        acShipTarget[obj] != -1) {
        point_ship(obj, 0, &vToTarget);
        anShipSpeed[obj] =
            (get_ship_max_velocity(obj) + 10) << 8;
        return;
    }

    target = &acShipTarget[obj];
    other = 0;
    cViableTargetCount = 0;
    *target = -1;
    for (; other <= 9; other++) {
        if (other == obj ||
            aeObjectClass[other] < OBJECT_CLASS_SHIP) {
            continue;
        }
        get_facing_range_from_object(obj, other);
        range = nTargetRange;
        facing = nFacingToTarget;
        viableIndex = cViableTargetCount;
        targetFacing = nTargetFacing;
        if (range < 9000 && facing > 0 && targetFacing < 0) {
            asViableTargetDistance[viableIndex] = range;
            acViableTarget[viableIndex] = other;
            cViableTargetCount = ++viableIndex;
        }
    }
    sort_viable_target_list();
    targetCount = cViableTargetCount;
    if (targetCount > 0) {
        for (heat = 3; heat > 0; heat--) {
            for (candidate = 0; candidate < targetCount; candidate++) {
                if (aeObjectClass[
                        acViableTarget[candidate]] ==
                        OBJECT_CLASS_CAPITAL_SHIP ||
                    abShipExhaustHeat[
                        acViableTarget[candidate]] == heat) {
                    *target = acViableTarget[candidate];
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

    if (aeShipTactic[obj] == TACTIC_RAM) {
        if (acShipTarget[obj] == -1) {
            cViableTargetCount = 0;
            for (other = 0; other < 10; other++) {
                if (other == obj ||
                    aeObjectClass[other] < OBJECT_CLASS_SHIP)
                    continue;
                if (aeShipSide[
                        acObjectOwner[obj]] ==
                        aeShipSide[other] &&
                    acShipCommunicator[other] != -1)
                    continue;
                nTargetRange = distance_from_object(obj, other);
                if (nTargetRange < 9000) {
                    candidate = cViableTargetCount++;
                    asViableTargetDistance[candidate] =
                        nTargetRange;
                    acViableTarget[candidate] = other;
                }
            }
            sort_viable_target_list();
            if (cViableTargetCount > 0)
                acShipTarget[obj] =
                    acViableTarget[0];
        } else {
            point_ship(obj, 0, &vToTarget);
            anShipSpeed[obj] =
                (get_ship_max_velocity(obj) + 10) << 8;
        }
    }
}

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
        asObjectCounter[i] = prev;
    }
    return 0;
}

/* Function start: 0x40B730 */
unsigned int init_mission(short series, short mission)
{
#ifdef WC1_SDL
    if (LoadMissionData(series, mission) != 0)
        return 1;
#else
    LoadMissionData(series, mission);
#endif
    init_3Space_objects(series);
    nSceneResourceBudget = LoadPacketResourceList(
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
    MissionShipRecord *playerRecord;
    short initial;
    short missionShip;
    int pilot;

    stCampaignState.missionScore = 0;
    nWingmanKilledThisMission = 0;
    bPlayerDestroyed = 0;
    nWingmanKillCount = 0;
    nMissionMedalScore = 0;
    nPlayerKillCount = 0;

    playerRecord =
        &aMissionShips[nPlayerMissionShipIndex];
    stCampaignState.playerShipType = playerRecord->type;
    load_ship(stCampaignState.playerShipType, 0);
    set_objects_data(0, stCampaignState.playerShipType, -1);
    playerRecord->navPoint = nMissionEntryNavPoint;
    if (nTrainSimActive == 0 && nStartNavPointOverride != -1)
        playerRecord->navPoint = nStartNavPointOverride;
    Set_up_ship_info(0, nPlayerMissionShipIndex, -1);

    memset(acPlayerComponentDamage, 0,
           sizeof(acPlayerComponentDamage));
    bInitialFormationSetup = 1;
    nYourWingman = -1;
    initial = 0;
    do {
        missionShip = nInitialMissionShipIndices[initial];
        if (missionShip != -1) {
            pilot = aMissionShips[missionShip].behaviour.pilot;
            if (is_alive(pilot) != 0 &&
                find_ships_sphere(missionShip) == -1) {
                init_ship(missionShip, nMissionEntryNavPoint);
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
    nCarrierMissionShipIndex = missionShip;
#ifdef WC1_SDL
    /* The original scans to 64 even though the initialized table ends after
     * record 47.  Valid missions place the carrier in the loaded first 32;
     * stop at the proven storage boundary in the sanitizer-enabled port. */
    while (missionShip < WC1_MISSION_SHIP_STORAGE_COUNT &&
           aMissionShips[missionShip].type !=
               OBJECT_TYPE_TIGERS_CLAW) {
#else
    while (aMissionShips[missionShip].type !=
               OBJECT_TYPE_TIGERS_CLAW &&
           missionShip < WC1_MISSION_SHIP_SCAN_LIMIT) {
#endif
        missionShip++;
        nCarrierMissionShipIndex = missionShip;
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
            FreePacketAndClear(&apObjectShape[obj], 0);
            asCapitalShipViewFrame[obj] = -1;
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
            if (aeObjectType[obj] == type) {
                FreePacketAndClear(&apObjectShape[obj], 0);
                asCapitalShipViewFrame[obj] = -1;
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
        if (aObjectResourceSlots[slot].shapeSet == 0) {
            aObjectResourceSlots[slot].type = type;
            cObjectResourceLogicalFile =
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
                aObjectResourceSlots[slot].shapeSet =
                    aObjectTypeData[
                        OBJECT_TYPE_ASTEROID5].shapeSet;
                if (nMemoryConfiguration == 2) {
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
                        apObjectShape[obj] =
                            aObjectTypeData[
                                aeObjectType[obj]].shapeSet;
                    }
                    obj++;
                } while (obj <= WC1_SPACE_LAST_MOVING_OBJECT);
                return 0;
            }
            if (type != OBJECT_TYPE_MINE_FIELD) {
                if (aObjectTypeData[type].objectClass !=
                        OBJECT_CLASS_SHIP &&
                    aObjectTypeData[type].objectClass !=
                        OBJECT_CLASS_MISSILE) {
                    if (bExpandedMemoryAvailable != 0) {
                        section = 0;
                        do {
                            aapPacketReferences[slot]
                                [section] =
                                FetchDiskPacketRetrying(
                                    (short)
                                        cObjectResourceLogicalFile,
                                    section, 4);
                            if (aapPacketReferences[slot]
                                    [section] == 0)
                                break;
                            section++;
                        } while (section < 0x25);
                    }
                    aObjectResourceSlots[slot].shape =
                        FetchDiskPacketRetrying(
                            (short)cObjectResourceLogicalFile,
                            0x25, 0);
                    aObjectTypeData[type].shape =
                        aObjectResourceSlots[slot].shape;
                    obj = 1;
                    do {
                        if (aeObjectType[obj] == type) {
                            FreePacketAndClear(
                                &apObjectShape[obj], 0);
                            asCapitalShipViewFrame[obj] = -1;
                        }
                        obj++;
                    } while (obj < 10);
                    return 0;
                }

                aObjectResourceSlots[slot].shapeSet =
                    FetchDiskPacketRetrying(
                        (short)cObjectResourceLogicalFile,
                        0, 0);
                aObjectTypeData[type].shapeSet =
                    aObjectResourceSlots[slot].shapeSet;
                aObjectResourceSlots[slot].animation =
                    FetchDiskPacketRetrying(
                        (short)cObjectResourceLogicalFile,
                        2, 0);
                aObjectTypeData[type].animation =
                    aObjectResourceSlots[slot].animation;
                aObjectResourceSlots[slot].shape =
                    FetchDiskPacketRetrying(
                        (short)cObjectResourceLogicalFile,
                        1, 0);
                aObjectTypeData[type].shape =
                    aObjectResourceSlots[slot].shape;
                obj = 0;
                do {
                    if (aeObjectClass[obj] >=
                            OBJECT_CLASS_MISSILE &&
                        aeObjectType[obj] == type) {
                        apObjectShape[obj] =
                            aObjectResourceSlots[slot]
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

    resource = &aObjectResourceSlots[slot];
    type = (enum ObjectType)resource->type;
    typeData = &aObjectTypeData[type];

    if (typeData->objectClass == OBJECT_CLASS_CAPITAL_SHIP) {
        release_capital_ship_shapes(type);
        if (bExpandedMemoryAvailable != 0) {
            section = 0;
            do {
                FreePacketAndClear(
                    &aapPacketReferences[slot][section],
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
            if (aeObjectType[obj] == OBJECT_TYPE_ROCK_CHUNK)
                remove_object(obj);
            else if (aeObjectClass[obj] ==
                     OBJECT_CLASS_ASTEROID)
                apObjectShape[obj] = 0;
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
            if (aeObjectClass[obj] >=
                    OBJECT_CLASS_MISSILE &&
                aeObjectType[obj] == type)
                apObjectShape[obj] = 0;
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
        if (aObjectResourceSlots[slot].type != -1)
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
            aObjectResourceSlots[slot].type;
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
        if (aObjectResourceSlots[slot].type == -1)
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
                aObjectResourceSlots[slot].type == type)
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
        resource = &aObjectResourceSlots[slot];
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

    nCurrentNavPoint = navPoint;
    nav = &aMissionNavPoints[navPoint];
    nCurrentWave =
        (short)((((aMissionNavPoints[navPoint + 1].type == 2) ?
                  -1 : 0) & 3) - 1);
    nEnemySighting = 0x7fff;

    obj = 1;
    do {
        if (aeObjectClass[obj] != OBJECT_CLASS_NULL &&
            acShipSpawnNavPoint[obj] != -1) {
            if (aeObjectClass[obj] >= OBJECT_CLASS_SHIP &&
                aeShipMissionType[obj] == MISSION_TYPE_ROUT) {
                aMissionShips[
                    nShipMissionIndices[obj]].state = 3;
            }
            if (aeObjectClass[obj] ==
                OBJECT_CLASS_CAPITAL_SHIP) {
                FreePacketAndClear(&apObjectShape[obj], 0);
            }
            if (asObjectScreenX[obj] != -0x7fff)
                explode(-1, obj);
            else
                remove_object(obj);
        }
        obj++;
    } while (obj < 10);
    remove_all_hazards();
    nHazardFieldCount = 0;
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
    while (objective < cMissionObjectiveCount) {
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
        if (aapszPilotSpeech[personality][line] != 0) {
            FreePacketAndClear(
                &aapszPilotSpeech[personality][line], 0);
        }
        if (apCommPortraitShapes[personality] != 0) {
            FreePacketAndClear(
                &apCommPortraitShapes[personality], 8);
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
    file = OpenDataFileOrDie(pDiskFileRecords[13].name);
    loaded = (short)(file >= 0);
    line = 0;
    for (; line < 11; line++) {
        loaded = (short)(loaded &
            (ReadDataFileAtOffset(
                (unsigned short)file,
                (personality * 11 + line) * 80,
                80, speech) == 1));
        if (loaded != 0) {
            aapszPilotSpeech[personality][line] =
                AllocateTaggedMemory(strlen(speech) + 1, 0);
            DosStrcpy(aapszPilotSpeech[personality][line], speech);
        }
    }

    CloseDataFile((unsigned short)file);
    if (loaded == 0)
        ShowModalMessage(szErrorLoadingPilotSpeech);
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
        random_radial(&aShipPosition[0], maximum,
                      &aShipPosition[obj]);
    } while (room_for_me(obj, minimum) == 0);
}

/* Function start: 0x40C3C0 */
void set_up_next_wave(void)
{
    MissionNavPoint *waveNav;
    short previousWave;
    short entry;

    if (nTrainSimActive != 0) {
        spacetrack(21, 2, 0);
        nArcadeBonusCountdown = 60;
        if (nCurrentWave != -1)
            nArcadeBonusCountdown = 30;
        GetArcadeBonus();
        FigureArcadeTime();
    }

    if (nCurrentWave == -1 ||
        nCannedSceneMode != 0)
        return;

    /* The original indexes through a base biased one MissionNavPoint before
     * aMissionNavPoints.  Preserve that -1 when expressing it as a typed
     * array index: arcade wave 2 is stored in nav record 1. */
    waveNav = &aMissionNavPoints[
        nCurrentNavPoint + nCurrentWave - 1];
    previousWave = nCurrentWave;
    nCurrentWave++;
    if (waveNav->type == (signed char)previousWave) {
        new_sphere_shapes(waveNav);
        waveNav->type = -1;
        entry = 0;
        do {
            approve_xyz(
                init_ship(waveNav->missionShips[entry],
                          nCurrentNavPoint),
                5000, 10000);
            entry++;
        } while (entry < 10);
        return;
    }
    nCurrentWave = -1;
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
        leaderRecord = &aMissionShips[
            leaderRecord->leaderMissionIndex];
    }

    sub_int_vector(
        &aaFormationPositions[record->formationIndex]
                                               [record->formationSpot],
        &aaFormationPositions[leaderRecord->formationIndex]
                                               [leaderRecord->formationSpot],
        &aShipFormationOffset[obj]);
    if (source == 0 && bInitialFormationSetup == 0)
        return 0;

    copy_frame(source, obj);
    set_sphere_point(leaderRecord, &aShipPosition[obj]);
    offset_location(obj, &aShipFormationOffset[obj],
                    &aShipPosition[obj]);
    anShipSpeed[obj] = leaderRecord->speed << 8;
    return 0;
}

/* Function start: 0x40C5E0 */
void Set_up_ship_info(short obj, short missionShip, signed char navPoint)
{
    MissionShipRecord *record = &aMissionShips[missionShip];

    asCapitalShipViewFrame[obj] = -1;
    acWingmanMessageState[obj] = -1;
    acLastAttacker[obj] = -1;
    asActionCount[obj] = 0;
    abShipExhaustHeat[obj] = 0;
    asShipAccumulatedDamage[obj] = 0;
    acShipDamage[obj] = 0;
    asCannedCommand[obj] = 0;
    acShipIonDriveDamage[obj] = 0;
    acShipDestroyedWeaponCount[obj] = 0;
    acShipCommunicator[obj] = 0;
    apCannedSequence[obj] = 0;
    acShipSpawnNavPoint[obj] = navPoint;
    nShipMissionIndices[obj] = missionShip;
    acShipPointingMode[obj] = 1;

    set_sphere_point(record, &aShipPosition[obj]);
    alter_yaw((short)-record->pitch, obj);
    alter_pitch((short)-record->yaw, obj);
    alter_roll(record->roll, obj);
    aeShipSide[obj] = record->side;
    anShipSpeed[obj] = record->speed << 8;
    aiPilotLevel[obj] = record->behaviour.pilot;
    reset_mission_type(obj, record->missionType);
    anShipMissionShip[obj] = record->targetMissionIndex;
    asShipWingLeader[obj] =
        find_ship_index(record->leaderMissionIndex);
    set_formation_position(obj, record);
    zero_vector(&aShipVelocity[obj]);
    init_intelligence_data(obj);
}

/* Function start: 0x40C740 */
unsigned int is_team_member(short missionShip)
{
    short index;

    if (nPlayerMissionShipIndex == missionShip)
        return 1;
    index = 0;
    do {
        if (nInitialMissionShipIndices[index] == missionShip)
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
    acTurnRegulator[objIndex] = 1;
    interval = anPilotTurnInterval[
        aiPilotLevel[objIndex]];
    acTurnInterval[objIndex] = interval;
    do {
        if (aeObjectClass[other] == OBJECT_CLASS_SHIP &&
            other != obj &&
            acTurnRegulator[other] ==
                acTurnRegulator[objIndex] &&
            acTurnInterval[other] == interval) {
            other = 1;
            acTurnRegulator[objIndex]++;
            if (interval < acTurnRegulator[objIndex])
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
    record = &aMissionShips[missionShip];
    if (record->type == OBJECT_TYPE_ASTEROID_FIELD ||
        record->type == OBJECT_TYPE_MINE_FIELD) {
        AddFixedVectors(&aMissionNavPoints[navPoint].position,
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

    abShipTurn[obj] = 0;
    clear_alert(obj);
    missionTarget = anShipMissionShip[obj];
    missionSpot = &aShipMissionSpot[obj];
    aeSpecialManeuver[obj] = SPECIAL_MANEUVER_NONE;
    *missionSpot = aMissionNavPoints[
        nCurrentNavPoint].position;

    switch (aeShipMissionType[obj]) {
    case MISSION_TYPE_ESCORT:
    case MISSION_TYPE_STRIKE:
    case MISSION_TYPE_DEFEND:
    case MISSION_TYPE_WINGMAN:
        anShipMissionShip[obj] = missionTarget;
        break;
    case MISSION_TYPE_GOTO_WARP:
        *missionSpot =
            aMissionNavPoints[missionTarget].position;
        break;
    case MISSION_TYPE_WARP_ARRIVE:
        aeShipTactic[obj] = TACTIC_WARP_IN;
        aeShipManeuver[obj] = MANEUVER_WARPING_IN;
        if (aeShipSide[obj] == SIDE_KILRATHI)
            break;
        /* fall through */
    case MISSION_TYPE_COME_HOME:
        locate_ship(nHomeMissionShipIndex, missionSpot);
        break;
    case MISSION_TYPE_CANNED_SEQUENCE:
        apCannedSequence[obj] =
            aMissionShips[
                nShipMissionIndices[obj]].behaviour.cannedSequence;
        aiPilotLevel[obj] = 2;
        advance_canned_sequence(obj);
        break;
    default:
        break;
    }

    if (aiPilotLevel[obj] < 5)
        acShipRating[obj] =
            (signed char)~RATING_PROVINCIAL;
    else
        acShipRating[obj] =
            (signed char)(aiPilotLevel[obj] -
                          RATING_ACE_SPIRIT);
    acShipStress[obj] = 0;
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
        nActiveInputDevice = -1;
    return result;
}

/* Function start: 0x40CB30 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
void SampleActiveJoystickDevice(void)
{
    short device;

    device = nActiveInputDevice;
    if (device != -1)
        SampleJoystickDevice(&aInputDeviceSamples[device],
                             device, 0);
}

/* Function start: 0x40CB60 */
void DrawNavTextLine(unsigned char alignment, unsigned short colour,
                     const char *format, ...)
{
    pCurrentTextContext->colour = colour;
    pCurrentTextContext->alignment = alignment;
    pCurrentTextContext->textCursor =
        pCurrentTextContext->text;
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
    DrawTextString(pCurrentTextContext->text);
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
    *y = (short)((nNavMapCentreY - mapY) /
                 nNavMapScale + 67);
}

/* Function start: 0x40CC30 */
void nav_getxy(short *x, short *y, int worldX, int worldZ)
{
    short mapY;

    *x = (short)((worldX / 100) >> 8);
    mapY = (short)((worldZ / 100) >> 8);
    *y = mapY;
    if (nNavMapCoordinateScaling != 0)
        ScaleNavMapCoordinates(x, y, *x, mapY);
}

/* Function start: 0x40CC80 */
void CheckPoint(short x, short y)
{
    nNavMapMinimumX =
        MinShort(nNavMapMinimumX, x);
    nNavMapMaximumX =
        MaxShort(nNavMapMaximumX, x);
    nNavMapMinimumY =
        MinShort(nNavMapMinimumY, y);
    nNavMapMaximumY =
        MaxShort(nNavMapMaximumY, y);
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
    nNavMapMinimumX = aMissionObjectives[0].mapX;
    nNavMapMaximumX = aMissionObjectives[0].mapX;
    nNavMapMinimumY = aMissionObjectives[0].mapY;
    nNavMapMaximumY = aMissionObjectives[0].mapY;
    objectiveIndex = 0;
    while (objectiveIndex < (short)cMissionObjectiveCount) {
        if (mobile_objective(objectiveIndex) != 0) {
            ship = find_ship_index((short)
                aMissionObjectives[objectiveIndex].index);
            if (ship != -1)
                aMissionObjectives[objectiveIndex].position =
                    aShipPosition[ship];
        }
        nav_getxy(
            &aMissionObjectives[objectiveIndex].mapX,
            &aMissionObjectives[objectiveIndex].mapY,
            aMissionObjectives[objectiveIndex].position.x,
            aMissionObjectives[objectiveIndex].position.z);
        CheckPoint(aMissionObjectives[objectiveIndex].mapX,
                   aMissionObjectives[objectiveIndex].mapY);
        objectiveIndex++;
    }
    IncludeNavMapWorldPoint(aShipPosition[0].x,
                            aShipPosition[0].z);
    width = (short)(nNavMapMaximumX -
                    nNavMapMinimumX);
    height = (short)(nNavMapMaximumY -
                     nNavMapMinimumY);
    halfWidth = (short)(width / 2);
    nNavMapCentreX =
        (short)(nNavMapMinimumX + halfWidth);
    halfHeight = (short)(height / 2);
    nNavMapCentreY =
        (short)(nNavMapMinimumY + halfHeight);
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
    const char *displayName;
    int type;
    short flightPathCount;

    flightPathCount = 0;
    SetNavMapCoordinateScaling(0);
    cMissionObjectiveCount = 0;
    source = aMissionObjectiveSources;
    type = source->type;
    for (; type != -1; source++, type = source->type) {
        aMissionObjectives[
            cMissionObjectiveCount].flags = 0;
        if (type == 0) {
            position = aMissionNavPoints[
                source->index].position;
            displayName = aMissionNavPoints[
                source->index].name;
            abFlightPath[flightPathCount++] =
                cMissionObjectiveCount;
        } else if (type >= 1 && type <= 4) {
            ship = &aMissionShips[source->index];
            displayName =
                aObjectTypeData[ship->type].displayName;
            set_sphere_point(ship, &position);
            abFlightPath[flightPathCount++] =
                cMissionObjectiveCount;
        }

        aMissionObjectives[
            cMissionObjectiveCount].type = type;
        aMissionObjectives[
            cMissionObjectiveCount].index = source->index;
        aMissionObjectives[
            cMissionObjectiveCount].name = source->description;
        aMissionObjectives[
            cMissionObjectiveCount].position = position;
        nav_getxy(
            &aMissionObjectives[
                cMissionObjectiveCount].mapX,
            &aMissionObjectives[
                cMissionObjectiveCount].mapY,
            position.x, position.z);
        aMissionObjectives[
            cMissionObjectiveCount].displayName = displayName;
        cMissionObjectiveCount++;
    }

    abFlightPath[flightPathCount] = -1;
    aMissionObjectives[
        (unsigned char)cMissionObjectiveCount].type = -1;
    cCurrentNavPointIndex = 0;
    cCurrentObjective = 0;
    if (cMissionObjectiveCount != 0) {
        while (set_new_objective(
                   (short)cCurrentNavPointIndex) == 0)
            cCurrentNavPointIndex++;
    }
}
