/* Flight maneuver handlers and their dispatch state.
 * The Mac fl symbol run corroborates this group; its dispatch tables precede
 * the cutscene data. Exact original file boundaries remain provisional.
 * Placement evidence is recorded in docs/ORDER.md.
 */
#include "wc1.h"

typedef void (*ShipManeuverHandler)(short, short);

/* 0x00465678 */
static unsigned char abManeuverRerollChance[47] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 3, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0
};
static ShipManeuverHandler apShipAiManeuverHandlers[47] = {
    /* 00 */ (void (*)(short, short))Mnone,
    /* 01 */ (void (*)(short, short))Mnone,
    /* 02 */ Mveer_away,
    /* 03 */ (void (*)(short, short))Mreset,
    /* 04 */ (void (*)(short, short))Mfull_ahead,
    /* 05 */ (void (*)(short, short))Mthink,
    /* 06 */ Mram_missile,
    /* 07 */ (void (*)(short, short))Mturn_n_kick,
    /* 08 */ (void (*)(short, short))Mtight_loop,
    /* 09 */ (void (*)(short, short))Mhard_break,
    /* 10 */ Msit_n_spin,
    /* 11 */ Mturn_n_spin,
    /* 12 */ Mburnout,
    /* 13 */ (void (*)(short, short))Mwabble,
    /* 14 */ (void (*)(short, short))Mroll_over,
    /* 15 */ (void (*)(short, short))Mhard_turn,
    /* 16 */ Mfish_hook,
    /* 17 */ (void (*)(short, short))Msplit_left,
    /* 18 */ Msit_n_fire,
    /* 19 */ (void (*)(short, short))Mkickit,
    /* 20 */ (void (*)(short, short))Mturn_n_kick,
    /* 21 */ (void (*)(short, short))Mrout_me,
    /* 22 */ (void (*)(short, short))Mdrop_a_mine,
    /* 23 */ (void (*)(short, short))Msplit_right,
    /* 24 */ Mzig_zag,
    /* 25 */ Mgloat,
    /* 26 */ Mtail_fire,
    /* 27 */ Mtarget_laser,
    /* 28 */ Mtarget_missile,
    /* 29 */ Mstrafe_enemy,
    /* 30 */ Mbest_strafe,
    /* 31 */ Mstrafe_n_roll,
    /* 32 */ Mkill_missile,
    /* 33 */ Msuicide_run,
    /* 34 */ Mzig_zag_pitch,
    /* 35 */ ShipAiState35,
    /* 36 */ Mbest_strafe,
    /* 37 */ Mget_distance,
    /* 38 */ (void (*)(short, short))Mcorkscrew,
    /* 39 */ Mtry2tail,
    /* 40 */ Mzip_past,
    /* 41 */ (void (*)(short, short))Mbuzz_debris,
    /* 42 */ Mline_up_drop,
    /* 43 */ Mchill,
    /* 44 */ (void (*)(short, short))ShipAiState44,
    /* 45 */ Mbest_strafe,
    /* 46 */ (void (*)(short, short))Mreset
}; /* 0x004656a8 */

static short nManeuverProximityRange; /* 0x00475e78 */
static unsigned char bCurrentManeuverReroll; /* 0x00475e7c */

/* Linker commons defined here; LINK orders them by identifier hash. */
short nScrambleCanopyOffset; /* 0x005a8736 */
short nFuneralMainDist; /* 0x005a8738 */
signed char bScrambleCanopyClosed; /* 0x005a873a */
short nCutsceneFuneralGuardFrame; /* 0x005a873c */
ShortPoint stScrambleOverlay; /* 0x005a8740 */
unsigned char *pScrambleOverlayShape; /* 0x005a8744 */

/* Function start: 0x4060A0 */
void SetManeuverProximityRange(unsigned short v)
{
    nManeuverProximityRange = v;
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
    if ((aeSpecialManeuvers[target] ==
         SPECIAL_MANEUVER_NORMAL) == 9) {
        maneuver_complete(ship);
        return;
    }
    if (no_goal(ship) != 0) {
        point_ship_at_point(ship, &aShipPositionsArray[target]);
        anRollGoal[ship] = 360;
    }
}

/* Function start: 0x406130 */
void Mwabble(short ship)
{
    short choice;

    asShipTotal[ship]++;
    if (asShipTotal[ship] > 20) {
        maneuver_complete(ship);
        return;
    }

    approach_full_speed(ship);
    if (no_goal(ship) != 0) {
        choice = RandomBelowOrEqual(2);
        switch (choice) {
        case 0:
            anYawTarget[ship] = signed_random(6) * 5;
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
    if (aeShipManeuversTable[ship] != MANEUVER_NONE)
        acShipSequence[ship] = acShipSequence[ship] + 1;
}

/* Function start: 0x406200 */
void ShipAiState35(short ship, short target)
{
    (void)target;

    switch (acShipSequence[ship]) {
    case 0:
        if (nTgtRange < 750 ||
            ++asShipTotal[ship] > 10) {
            advance(ship);
            asShipTotal[ship] = 0;
        }
        break;
    case 1:
        veer_random(ship, 45);
        advance(ship);
        break;
    case 2:
        if (++asShipTotal[ship] > 5) {
            acShipSequence[ship] = 0;
            asShipTotal[ship] = 0;
        }
        if (nTargetFacingValue < 75)
            advance(ship);
        break;
    case 3:
        point_ship(ship, 0, &vToTgt);
        approach_speed(ship, 0x500);
        if (nFacingToTargetValue > 10)
            reset_maneuver(ship, MANEUVER_TAIL_FIRE);
        if (nTgtRange > 1500 ||
            nTargetFacingValue > 80)
            maneuver_complete(ship);
        break;
    }
}

/* Function start: 0x406310 */
void Mfull_ahead(short ship)
{
    short *counter;
    short count;

    approach_full_speed(ship);
    counter = &asShipTotal[ship];
    count = *counter;
    *counter = count - 1;
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
    if (nTgtRange > 1500)
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
    if (asShipTotal[ship] == 0)
        asShipTotal[ship] = 2;
    if (--asShipTotal[ship] <= 1)
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
        advanceSequence = ++asShipTotal[ship] > 3;
        if (advanceSequence != 0)
            asShipTotal[ship] = 0;
        break;
    case 2:
        fire_super_brake(ship);
        break;
    case 3:
        advanceSequence = ++asShipTotal[ship] > 3;
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
        if (++asShipTotal[ship] < 4) {
            approach_speed(ship, anShipSpeed[target]);
            ScaleFixedVector(&aShipFwdVec[target],
                             nTgtRange * 2, &destination);
            AddFixedVectors(&aShipPositionsArray[target],
                            &destination, &destination);
            point_ship_at_point(ship, &destination);
            advanceSequence = 0;
        } else {
            asShipTotal[ship] = 0;
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
            approach_speed(ship, anShipSpeed[target]);
            advanceSequence = 0;
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
        if (nFacingToTargetValue > 85)
            fire(ship, target);
        if (++asShipTotal[ship] <= 5)
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
        advanceSequence = ++asShipTotal[ship] > 2;
        break;
    case 2:
        advanceSequence = nTargetFacingValue <= 80;
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
        if (aeSpecialManeuvers[ship] == SPECIAL_MANEUVER_NONE) {
            anYawTarget[ship] = 180;
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
        if (aeSpecialManeuvers[ship] ==
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
        if (aeSpecialManeuvers[ship] == SPECIAL_MANEUVER_NONE)
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
        anYawTarget[ship] =
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
        anYawTarget[ship] =
            RandomBelowOrEqual(2) * 240 - 120;
        fire_afterburner(ship, 5);
        break;
    case 1:
        advanceSequence = no_goal(ship);
        approach_full_speed(ship);
        break;
    case 2:
        fire_super_brake(ship);
        anYawTarget[ship] =
            RandomBelowOrEqual(2) * 90 - 45;
        break;
    case 3:
        advanceSequence =
            aObjectTypeData[
                aeObjectTypeList[ship]].cruiseVelocity >=
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
        anYawTarget[ship] = 90;
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
        anYawTarget[ship] = -90;
        advance(ship);
        break;
    default:
        if (no_goal(ship) != 0)
            maneuver_complete(ship);
        break;
    }
}

/* Function start: 0x406C70 */
void Mgloat(short ship, short target)
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
            if (++asShipTotal[ship] < 10)
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
            if (nTargetFacingValue > 80)
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
    int objectIndex;
    short object;

    if (no_goal(ship) != 0)
        point_ship_at_object(ship, target);
    approach_cruise_speed(ship);

    object = 0;
    while (object < 10) {
        objectIndex = object;
        if (acObjOwnersTable[objectIndex] == ship &&
            aeObjectClass[objectIndex] == OBJECT_CLASS_MISSILE) {
            reset_maneuver(ship, MANEUVER_STRAFE_ENEMY);
            return;
        }
        object++;
    }

    if (nFacingToTargetValue > 85 &&
        nTgtRange < 6000 &&
        (nTargetFacingValue > 80 ||
         nTargetFacingValue < -80) &&
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
    if (nFacingToTargetValue > 75 &&
        nTgtRange < 6000) {
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
        if (aeSpecialManeuvers[ship] ==
            SPECIAL_MANEUVER_NONE) {
            maneuver_complete(ship);
        } else if (nFacingToTargetValue > 95) {
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
        abs(anYawTarget[ship]) != 0)
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
    if (nTargetFacingValue < 0x50) {
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
    if (nTgtRange > 3000)
        approach_cruise_speed(ship);
    else
        approach_zero_speed(ship);
    fire_when_ready(ship, 1);
}

/* Function start: 0x4070D0 */
void Mstrafe_n_roll(short ship, short target)
{
    if (0 < asObjectCounterArray[ship]) {
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
    if (nFacingToTargetValue < 0) {
        reset_maneuver(ship, MANEUVER_FISH_HOOK);
        Mfish_hook(ship, target);
        return;
    }
    if (nFacingToTargetValue < 80) {
        reset_maneuver(ship, MANEUVER_BURNOUT);
        Mburnout(ship, target);
        return;
    }
    point_ship_at_object(ship, nTargetShip);
    if (nTgtRange < 8000)
        fire(ship, acShipTargetTable[ship]);
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

    amount = nTgtRange;
    if (amount > 2000) {
        maneuver_complete(ship);
        return;
    }
    if (amount < 700 && normal_speed(ship) != 0)
        fire_afterburner(ship, 10);
    else
        approach_full_speed(ship);
    if (nFacingToTargetValue > 0 && no_goal(ship) != 0) {
        amount = MinShort(20, nFacingToTargetValue);
        steer_away_from_object(ship, target, amount);
    }
}

/* Function start: 0x407270 */
void general_zig(short ship, short target, short pitch)
{
    short complete = 1;

    (void)target;
    approach_full_speed(ship);
    switch (acShipSequence[ship] % 6) {
    case 0:
        anYawTarget[ship] = -35;
        anPitchGoal[ship] = pitch;
        break;
    case 1:
    case 4:
        complete = no_goal(ship);
        asShipTotal[ship] = 0;
        break;
    case 2:
    case 5:
        complete = ++asShipTotal[ship] >= 4;
        break;
    case 3:
        pitch = -pitch;
        anYawTarget[ship] = 35;
        anPitchGoal[ship] = pitch;
        break;
    }
    if (acShipSequence[ship] >= 12)
        maneuver_complete(ship);
    if (complete != 0)
        advance(ship);
}

/* Function start: 0x407350 */
void Mzig_zag(short ship, short target)
{
    general_zig(ship, target, 0);
}

/* Function start: 0x407370 */
void Mzig_zag_pitch(short ship, short target)
{
    general_zig(ship, target, 0x23);
}

/* Function start: 0x407390 */
void Mcorkscrew(short ship)
{
    approach_full_speed(ship);
    if (no_goal(ship) != 0) {
        if (--asShipTotal[ship] <= 0) {
            switch (acShipSequence[ship] % 4) {
            case 0:
                anYawTarget[ship] = -20;
                break;
            case 1:
            case 3:
                anRollGoal[ship] = 20;
                break;
            case 2:
                anYawTarget[ship] = 20;
                break;
            }
            asShipTotal[ship] = 4;
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
        if (nFacingToTargetValue > 80)
            steer_away_from_object(ship, target, 40);
        else
            steer_away_from_object(ship, target, 10);
        advance(ship);
        return;
    default:
        break;
    }
    if (asObjectCollisionRadius[target] * 3 <
        nTgtRange) {
        veer_random(ship, 8);
        maneuver_complete(ship);
        return;
    }
    if (no_goal(ship) != 0) {
        if (nFacingToTargetValue > 80) {
            steer_away_from_object(ship, target, 40);
        } else if (nFacingToTargetValue < -65 ||
                   RandomBelowOrEqual(100) < 4) {
            veer_random(ship, 16);
        }
    }
    if ((nManeuverProximityRange >= nTgtRange ||
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
    acShipStressList[ship] = 0;
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
    short target = acShipTargetTable[obj];
    enum ShipManeuver previous = aeShipManeuversTable[obj];
    short range;
    int proximityLimit;

#ifdef SDL_PORT
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
#ifdef SDL_PORT
    /* The original reaches the same completion path only after calculating
       geometry for target -1.  Those reads alias the globals immediately
       before three object tables in the Win32 image. */
    if (target == -1) {
        maneuver_complete(obj);
        return;
    }
#endif
    ship_vs_ship(obj, target);
    range = nTgtRange;
    if (nTargetFacingValue < 0)
        SetManeuverProximityRange((unsigned short)((asObjectCollisionRadius[target] +
                                   asObjectCollisionRadius[obj] * 4) >> 1));
    else
        SetManeuverProximityRange((unsigned short)((asObjectCollisionRadius[target] +
                                   asObjectCollisionRadius[obj] * 6) >> 1));

    if (unactive(target) != 0) {
        switch (aeShipManeuversTable[obj]) {
        case MANEUVER_VEER_AWAY:
            Mveer_away(obj, target);
            break;
        case MANEUVER_GLOAT:
            Mgloat(obj, target);
            break;
        case MANEUVER_LINE_UP_DROP:
            Mline_up_drop(obj, target);
            break;
        default:
            maneuver_complete(obj);
            break;
        }
    } else if (aeShipManeuversTable[obj] >= 0 &&
               aeShipManeuversTable[obj] < 47) {
#ifdef SDL_PORT
        /* The original x86 dispatcher pushes both values for every handler.
           Several handlers consume fewer arguments. Call those through their
           real C types in the native port. */
        switch (aeShipManeuversTable[obj]) {
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
        case MANEUVER_GLOAT:
            Mgloat(obj, target);
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
                aeShipManeuversTable[obj]](obj, target);
            break;
        }
#else
        apShipAiManeuverHandlers[
            aeShipManeuversTable[obj]](obj, target);
#endif
    } else {
        maneuver_complete(obj);
    }

    proximityLimit = *(volatile short *)&nManeuverProximityRange;
    if ((short)proximityLimit > *(volatile short *)&range) {
        try2reset_maneuver(obj, 2);
    } else if (aeShipManeuversTable[obj] == previous &&
               RandomBelowOrEqual(100) <
                   bCurrentManeuverReroll) {
        maneuver_complete(obj);
    }
}
