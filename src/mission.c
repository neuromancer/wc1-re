/*
 *  Mission setup: FX drivers, palette load, ship state bits.
 *
 *  Address range 0x421000-0x424fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: LoadOriginFxDrivers/EMStartUp; string band 0x469A28-0x469B9C.
 */
#include "wc1.h"

/* Function start: 0x421100 */
short find_weapon(short obj, enum ObjectType weaponType)
{
    short weapon;

    weapon = 0;
    while (weapon < (signed char)g_aShipWeapons_0059cab0[obj][0]) {
        if (*(enum ObjectType *)(
                g_aShipWeapons_0059cab0[obj] + 1 + weapon * 7) ==
            weaponType)
            return weapon;
        weapon++;
    }
    return -1;
}

/* Function start: 0x421350 */
void fire_afterburner(short obj, short time)
{
    long velocity = ComputeFixedVectorMagnitude(&g_aShipVelocity_0059c010[obj]);

    if (velocity < get_ship_max_velocity(obj) * 0x500L) {
        set_special(obj, SPECIAL_MANEUVER_AFTERBURNER);
        g_asShipAfterburnerTimer_0059c810[obj] =
            g_aeSpecialManeuver_0059c3c0[obj] ==
                SPECIAL_MANEUVER_AFTERBURNER ? time : 0;
    }
}

/* Function start: 0x4213B0 */
unsigned int ShipAiRoutine15(short ship)
{
    DAT_0059c810[ship] = 10;
    set_special(ship, SPECIAL_MANEUVER_SUPER_BRAKE);
    return 0;
}

/* Function start: 0x421760 */
unsigned int FlagShipAiIfPositive(short i)
{
    if (0 < g_anShipSpeed_0059b320[i])
        AddShipAiTimer(i, 5);
    return 0;
}

/* Function start: 0x421A40 */
unsigned int LeaveWaitCursorScope(void)
{
    if (DAT_0059a850 != 0)
        ClearWaitCursorFlag();
    return 0;
}

/* Function start: 0x421B10 */
unsigned int LoadOriginFxDrivers(void)
{
    FILE *paletteFile;
    unsigned char palette[0x300];
    unsigned short *rowOffsets;
    short row;

    /* The retail function leaves the process in GAMEDAT after bringing up the
     * Origin FX drivers.  The intro packet names are relative to that directory. */
    if (_chdir("gamedat") != 0)
        FatalErrorAndExit("Unable to enter GAMEDAT");

    if (DAT_005a6ba0.pixels == 0)
        FatalErrorAndExit("The DirectDraw frame buffer is not available");

    rowOffsets = (unsigned short *)malloc(202 * sizeof(unsigned short));
    if (rowOffsets == 0)
        FatalErrorAndExit("Unable to allocate the intro viewport");

    row = 0;
    do {
        rowOffsets[row] = (unsigned short)(row * 320);
        row = row + 1;
    } while (row < 202);

    DAT_005a6ba0.rowOffsets = rowOffsets;
    DAT_005a6ba0.left = 0;
    DAT_005a6ba0.top = 0;
    DAT_005a6ba0.right = 319;
    DAT_005a6ba0.bottom = 199;
    DAT_005a6ba0.allocation = DAT_005a6ba0.pixels;
    DAT_0059ab23 = &DAT_005a6ba0;
    DAT_005a6538 = (int *)&DAT_005a6ba0;

    DAT_005a7510.left = 0;
    DAT_005a7510.top = 0;
    DAT_005a7510.right = 319;
    DAT_005a7510.bottom = 199;
    if (!AllocateViewport(&DAT_005a7510, 0, 0x20))
        FatalErrorAndExit("Unable to allocate the intro back buffer");

    paletteFile = fopen("GAME.PAL", "rb");
    if (paletteFile == 0)
        FatalErrorAndExit("Unable to open GAME.PAL");
    if (fseek(paletteFile, 0x30, SEEK_SET) != 0 ||
        fread(palette, 1, sizeof(palette), paletteFile) != sizeof(palette)) {
        fclose(paletteFile);
        FatalErrorAndExit("Unable to read GAME.PAL");
    }
    fclose(paletteFile);

    row = 0;
    do {
        DAT_005a8a50[row] = palette[row];
        row = row + 1;
    } while (row < 0x300);
    DIBwholePaletteFromTriplets(palette);

    memset(DAT_005a6ba0.pixels, 0, 320 * 200);
    DAT_0046b1b8 = 0;
    DIBslam();
    DIBslamReal();
    g_nFrameSkip_00469fb8 = 1;
    return 0;
}

/* Function start: 0x421FE0 */
unsigned int GetFxDriverInitResult(void)
{
    return 0;
}

/* Function start: 0x421FF0 */
unsigned int GetMessagePumpResult(void)
{
    return 0;
}

/* Function start: 0x422000 */
unsigned int GetFxDriverStatus(void)
{
    return 0;
}

/* Function start: 0x422010 */
int TestShipFlags(short i, unsigned char bits)
{
    return (DAT_0059ca94[i] & bits) == bits;
}

/* Function start: 0x422030 */
void ClearShipFlags(short i, unsigned char bits)
{
    DAT_0059ca94[i] &= ~bits;
}

/* Function start: 0x422050 */
void SetShipFlags(short i, unsigned char bits)
{
    DAT_0059ca94[i] |= bits;
}

/* Function start: 0x422090 */
void prepare_ace_engagement(short obj)
{
    short ace = (short)g_aiPilotLevel_0059cf30[obj] - 14;

    send_message(obj, (signed char)(TestShipFlags(ace, 4) != 0));
    SetShipFlags(ace, 8);
}

/* Function start: 0x4220D0 */
void ClearShipTimer(short i)
{
    ClearShipFlags(i, 0x1a);
    SetShipFlags(i, 0x20);
}

/* Function start: 0x4220F0 */
short RandomCentred(short range)
{
    return (short)RandomBelowOrEqual(range * 2) - range;
}

/* Function start: 0x422110 */
int ShipAiRoutine16(short ship, unsigned int bits)
{
    return (DAT_0059b430[ship] & bits) != 0;
}

/* Function start: 0x422130 */
/* NOT a constant return: `mov eax,0x59b430 / cmp eax,1 / sbb eax,eax / inc eax`
 * is the classic `>= 1` boolean idiom applied to the *address* 0x0059B430,
 * which is always non-zero -- hence Ghidra folding it to `return 1`.  Written
 * the way the original was, so the idiom survives. */
unsigned int HasSpeechBuffer(void)
{
    return (unsigned int)&DAT_0059b430 >= 1;
}

/* Function start: 0x422140 */
unsigned int SetShipStateBits(short i, unsigned int bits)
{
    DAT_0059b430[i] |= bits;
    return bits & 0xffff0000;
}

/* Function start: 0x422160 */
void ResetShipStateRecord(short i)
{
    g_asCollisionCountdown_0059d2d0[i] = 0;
    DAT_0059b430[i] = 0;
    DAT_0059cf20[i] = 0xff;
}

/* Function start: 0x422180 */
void start_collision_avoidance(short obj, short other)
{
    if ((short)(signed char)DAT_0059cf20[obj] != other) {
        DAT_0059cf20[obj] = (unsigned char)other;
        steady_object(obj);
    }
    g_asCollisionCountdown_0059d2d0[obj] = 3;
    SetShipStateBits(obj, 1);
    g_asShipAfterburnerTimer_0059c810[obj] = 0;
    set_special(obj, SPECIAL_MANEUVER_NONE);
}

/* Function start: 0x4221E0 */
void advance_collision_avoidance(short obj)
{
    g_asCollisionCountdown_0059d2d0[obj]--;
    if (g_asCollisionCountdown_0059d2d0[obj] <= 0)
        ResetShipStateRecord(obj);
    else
        SetShipStateBits(obj, 2);
}

/* Function start: 0x422220 */
short normal_speed(short obj)
{
    if (g_aeSpecialManeuver_0059c3c0[obj] !=
            SPECIAL_MANEUVER_AFTERBURNER &&
        real_velocity(obj) <= g_asShipMaximumSpeed_0059c440[obj])
        return 1;
    return 0;
}

/* Function start: 0x422260 */
short predict_collision_time(short obj, short other)
{
    short collisionRadius;
    short time;
    short range;
    short elapsed;
    short step;
    int collisionFound;
    long distance;
    long relativeSpeed;
    FixedVector relativePosition;
    FixedVector relativeVelocity;
    FixedVector travel;
    FixedVector separation;

    collisionRadius = g_asObjectCollisionRadius_0059d710[obj];
    collisionRadius += g_asObjectCollisionRadius_0059d710[other];
    collisionRadius += 30;
    collisionFound = 0;
    ComputeVectorDelta(&g_aShipPosition_0059c490[obj],
                       &g_aShipPosition_0059c490[other],
                       &relativePosition);
    distance = ComputeFixedVectorMagnitude(&relativePosition);
    if (((long)collisionRadius + 1500) * 0x100 < distance)
        return 0x7fff;

    ComputeVectorDelta(&g_aShipVelocity_0059c010[obj],
                       &g_aShipVelocity_0059c010[other],
                       &relativeVelocity);
    relativeSpeed = ComputeFixedVectorMagnitude(&relativeVelocity);
    if (relativeSpeed == 0)
        return 0x7fbc;

    time = FixedToShortSaturating(
        DivideFixed((int)distance, (int)relativeSpeed));
    if (time >= 30)
        return time;

    ScaleFixedVector(&relativeVelocity, (int)time << 8, &travel);
    AddFixedVectors(&relativePosition, &travel, &separation);
    range = FixedToShortSaturating(
        (int)ComputeFixedVectorMagnitude(&separation));
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
            (int)ComputeFixedVectorMagnitude(&separation));
        if (collisionRadius >= range)
            collisionFound = 1;
    } while (collisionFound == 0);
    return elapsed;
}

/* Function start: 0x422440 */
void ClearWingmanSlots(void)
{
    short i = 0;

    do {
        g_asCollisionPartner_005a7cc0[i] = -1;
        i = i + 1;
    } while (i < 10);
}

/* Function start: 0x422460 */
short collision_distance(short obj, short other)
{
    if (g_asCollisionPartner_005a7cc0[obj] == other)
        return g_asCollisionTime_005a7ca0[obj];
    if (other < 10 && g_asCollisionPartner_005a7cc0[other] == obj)
        return g_asCollisionTime_005a7ca0[other];
    if (g_aeObjectClass_0059d100[other] == OBJECT_CLASS_ASTEROID &&
        ((short)DAT_0059d9b0[other] == -0x7fff ||
         (short)DAT_0059d9b0[obj] == -0x7fff))
        return 0x7fff;
    return predict_collision_time(obj, other);
}

/* Function start: 0x4224F0 */
short find_collision_object(short obj)
{
    short candidate;
    short closestTime;
    short other;
    short time;

    candidate = -1;
    closestTime = 30;
    other = 0;
    do {
        if (other != obj &&
            g_aeObjectClass_0059d100[other] >= OBJECT_CLASS_ASTEROID &&
            g_aeObjectClass_0059d100[other] != OBJECT_CLASS_MISSILE) {
            time = collision_distance(obj, other);
            if (closestTime > time) {
                closestTime = time;
                candidate = other;
            }
        }
        other++;
    } while (other <= 60);
    if (candidate != -1) {
        g_asCollisionPartner_005a7cc0[obj] = candidate;
        g_asCollisionTime_005a7ca0[obj] = closestTime;
    }
    return candidate;
}

/* Function start: 0x422560 */
unsigned int unactive(short ship)
{
    if (ship != -1 &&
        g_aeObjectClass_0059d100[ship] >= OBJECT_CLASS_SHIP &&
        g_aeShipManeuver_0059dcb0[ship] != MANEUVER_HARD_BRAKE)
        return 0;
    return 1;
}

/* Function start: 0x4225C0 */
void trim_goals(short obj, short amount)
{
    short goal = g_anYawGoal_0059c310[obj];

    if (goal > amount)
        g_anYawGoal_0059c310[obj] = amount;
    else if (goal <= -amount && goal != -amount)
        g_anYawGoal_0059c310[obj] = -amount;
    goal = g_anPitchGoal_0059d7a0[obj];
    if (goal > amount)
        g_anPitchGoal_0059d7a0[obj] = amount;
    else if (goal <= -amount && goal != -amount)
        g_anPitchGoal_0059d7a0[obj] = -amount;
}

/* Function start: 0x422640 */
int report_kilrathi_rout(short mode)
{
    short obj;

    for (obj = 0; obj < 10; obj++) {
        if (g_aeObjectClass_0059d100[obj] < OBJECT_CLASS_SHIP ||
            g_aeSpecialManeuver_0059c3c0[obj] ==
                SPECIAL_MANEUVER_UNKNOWN_9)
            continue;
        if (mode == 0 && g_aeShipSide_0059d650[obj] == SIDE_KILRATHI)
            return 1;
        if (mode == 1 && g_aeShipSide_0059d650[obj] == SIDE_KILRATHI &&
            g_aeShipMissionType_0059c3f0[obj] != MISSION_TYPE_ROUT &&
            distance_from_object(0, obj) < 16000)
            return 1;
        if (mode == 2 && g_aeShipSide_0059d650[obj] == SIDE_KILRATHI &&
            (g_aeShipObjective_0059d200[obj] == OBJECTIVE_ENGAGE_ENEMY ||
             g_aeShipObjective_0059d200[obj] == OBJECTIVE_DESTROY_SHIP))
            return 1;
    }
    return 0;
}

/* Function start: 0x422710 */
short find_ship_index(short missionShip)
{
    short obj;

    g_nLastFoundShip_005a7cba = -1;
    if (missionShip == -1)
        return -1;
    for (obj = 0; obj < 10; obj++) {
        if (((g_aeObjectClass_0059d100[obj] >= OBJECT_CLASS_SHIP &&
              g_aeSpecialManeuver_0059c3c0[obj] !=
                  SPECIAL_MANEUVER_UNKNOWN_9) ||
             g_aeObjectClass_0059d100[obj] == OBJECT_CLASS_FUTURION) &&
            g_acShipMissionIndex_0059c830[obj] == missionShip) {
            g_nLastFoundShip_005a7cba = obj;
            return obj;
        }
    }
    return -1;
}

/* Function start: 0x422830 */
unsigned char no_goal(short ship)
{
    return (g_anYawGoal_0059c310[ship] |
            g_anPitchGoal_0059d7a0[ship] |
            g_anRollGoal_0059d630[ship]) == 0;
}

/* Function start: 0x422860 */
int is_enemy_on_tail(short obj, short other)
{
    ship_vs_ship(obj, other);
    return g_nFacingToTarget_0059d920 < -60 &&
           g_nTargetFacing_0059d52a > 85 &&
           g_nTargetRange_0059ce10 < 7000;
}

/* Function start: 0x422930 */
short detect_enemy_tail(short obj)
{
    short other;

    for (other = 0; other < 10; other++) {
        if (g_aeObjectClass_0059d100[other] >= OBJECT_CLASS_SHIP &&
            g_aeSpecialManeuver_0059c3c0[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            g_aeShipSide_0059d650[obj] != g_aeShipSide_0059d650[other] &&
            g_acShipTarget_0059ce60[other] == obj &&
            is_enemy_on_tail(obj, other) != 0)
            return other;
    }
    return -1;
}

/* Function start: 0x4229F0 */
int object_requires_evasion(short obj)
{
    short other;

    for (other = 0; other < 10; other++) {
        if (g_aeObjectClass_0059d100[other] == OBJECT_CLASS_MISSILE &&
            g_acShipTarget_0059ce60[other] == obj)
            return 1;
    }
    return 0;
}

/* Function start: 0x422A70 */
unsigned int build_formation_member_list(short leader)
{
    short obj;
    short index = 1;

    g_acFormationMemberList_0059d490[0] = (signed char)leader;
    g_acFormationMemberList_0059d490[1] = -1;
    for (obj = 0; obj < 10; obj++) {
        if (g_asShipWingLeader_0059d400[obj] == leader) {
            g_acFormationMemberList_0059d490[index++] = (signed char)obj;
            g_acFormationMemberList_0059d490[index] = -1;
        }
    }
    return 0;
}

/* Function start: 0x422AC0 */
unsigned int get_formation_center(FixedVector *center)
{
    short count = 0;
    signed char member;

    zero_vector(center);
    member = g_acFormationMemberList_0059d490[0];
    while (member != -1) {
        AddFixedVectors(center, &g_aShipPosition_0059c490[member], center);
        member = g_acFormationMemberList_0059d490[++count];
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
    short index = 0;

    build_formation_member_list(obj);
    get_formation_center(&center);
    member = g_acFormationMemberList_0059d490[0];
    while (member != -1) {
        destination = &g_aShipDestination_0059d530[member];
        ComputeVectorDelta(&center, &g_aShipPosition_0059c490[member],
                           destination);
        ScaleFixedVector(destination, 0xa00, destination);
        AddFixedVectors(destination, &g_aShipPosition_0059c490[member],
                        destination);
        steady_object(member);
        reset_objective(member, OBJECTIVE_BREAK_FORMATION);
        member = g_acFormationMemberList_0059d490[++index];
    }
    return 0;
}

/* Function start: 0x422BE0 */
unsigned int reset_mission_type(short obj,
                                enum ShipMissionType missionType)
{
    if (missionType == MISSION_TYPE_ROUT &&
        g_aeShipSide_0059d650[obj] == SIDE_KILRATHI)
        report_kilrathi_rout(1);
    reset_objective(obj, OBJECTIVE_NONE);
    g_aeShipMissionType_0059c3f0[obj] = missionType;
    return 0;
}

/* Function start: 0x422C30 */
unsigned int change_mission_type(short obj,
                                 enum ShipMissionType missionType)
{
    if (g_aeShipObjective_0059d200[obj] == OBJECTIVE_ENGAGE_ENEMY)
        g_aeShipMissionType_0059c3f0[obj] = missionType;
    else
        reset_mission_type(obj, missionType);
    return 0;
}

/* Function start: 0x422C70 */
unsigned int reset_objective(short ship, enum ShipObjective objective)
{
    steady_object(ship);
    reset_tactic(ship, TACTIC_NONE);
    g_aeShipObjective_0059d200[ship] = objective;
    return 0;
}

/* Function start: 0x422CD0 */
unsigned int reset_tactic(short ship, enum ShipTactic tactic)
{
    set_maneuver(ship, MANEUVER_NONE);
    g_aeShipTactic_0059d5e0[ship] = tactic;
    g_acShipTarget_0059ce60[ship] = -1;
    return 0;
}

/* Function start: 0x422D00 */
unsigned int alter_tactic(short ship, enum ShipTactic tactic)
{
    set_maneuver(ship, MANEUVER_NONE);
    g_aeShipTactic_0059d5e0[ship] = tactic;
    return 0;
}

/* Function start: 0x422D30 */
void set_maneuver(short ship, enum ShipManeuver maneuver)
{
    g_aeShipManeuver_0059dcb0[ship] = maneuver;
    g_asShipCount_0059c420[ship] = 0;
    g_acShipSequence_0059d520[ship] = 0;
}

/* Function start: 0x422D60 */
void set_maneuver_range_state(short obj, short maneuver)
{
    if (g_aeShipManeuver_0059dcb0[obj] != maneuver) {
        set_maneuver(obj, (enum ShipManeuver)maneuver);
        steady_object(obj);
    }
}

/* Function start: 0x422D90 */
unsigned int set_special(short ship, enum SpecialManeuver special)
{
    enum SpecialManeuver *currentState = &g_aeSpecialManeuver_0059c3c0[ship];

    if (*currentState >= SPECIAL_MANEUVER_LOST_CONTROL && *currentState >= special)
        goto checkCancellation;
    *currentState = special;

checkCancellation:
    if (*currentState == SPECIAL_MANEUVER_BLOWING_UP && (short)ShipAiRoutine16(ship, 1))
        *currentState = SPECIAL_MANEUVER_NONE;
    return 0;
}

/* Function start: 0x422DD0 */
unsigned int ClearShipAiThrottle(short ship)
{
    ApproachShipSpeed(ship, 0);
    return 0;
}

/* Function start: 0x422DF0 */
unsigned int approach_half_speed(short obj)
{
    ApproachShipSpeed(obj, 0x500);
    return 0;
}

/* Function start: 0x422E50 */
unsigned int approach_cruise_speed(short ship)
{
    ApproachShipSpeed(ship,
        (int)*(short *)(&DAT_00466472[0] +
                        g_aeObjectType_0059b560[ship] * 0x87) << 8);
    return 0;
}

/* Function start: 0x422E80 */
unsigned int approach_full_speed(short ship)
{
    ApproachShipSpeed(ship,
                      (int)g_asShipMaximumSpeed_0059c440[ship] << 8);
    return 0;
}

/* Function start: 0x422EA0 */
unsigned int approach_ship_speed(short obj, short other)
{
    ApproachShipSpeed(obj, g_anShipSpeed_0059b320[other]);
    return 0;
}

/* Function start: 0x422F60 */
unsigned int IsBehindAndFalling(short ship)
{
    if (g_nTargetRange_0059ce10 < ship && g_nTargetFacing_0059d52a < -0x32)
        return 1;
    return 0;
}

/* Function start: 0x422F80 */
short scan_for_enemy(short obj, short range)
{
    short other;
    short distance;

    g_nTargetShip_0059c3b0 = -1;
    g_nTargetRange_0059ce10 = 0;
    for (other = 0; other < 10; other++) {
        if (g_aeObjectClass_0059d100[other] < OBJECT_CLASS_SHIP ||
            g_aeSpecialManeuver_0059c3c0[other] ==
                SPECIAL_MANEUVER_UNKNOWN_9 ||
            g_aeShipSide_0059d650[obj] == g_aeShipSide_0059d650[other])
            continue;
        distance = distance_from_point(other, &g_aShipPosition_0059c490[obj]);
        if ((unsigned short)distance < (unsigned short)range &&
            (g_nTargetShip_0059c3b0 == -1 ||
             distance < g_nTargetRange_0059ce10)) {
            g_nTargetRange_0059ce10 = distance;
            g_nTargetShip_0059c3b0 = other;
        }
    }
    if (g_nTargetShip_0059c3b0 != -1) {
        get_facing_range_from_object(obj, g_nTargetShip_0059c3b0);
        g_nTargetRange_0059ce10 =
            distance_from_object(obj, g_nTargetShip_0059c3b0);
    }
    return g_nTargetShip_0059c3b0;
}

/* Function start: 0x423070 */
int any_enemy(short obj, short range)
{
    short other;

    g_nTargetShip_0059c3b0 = -1;
    for (other = 0; other < 10; other++) {
        if (g_aeObjectClass_0059d100[other] >= OBJECT_CLASS_SHIP &&
            g_aeSpecialManeuver_0059c3c0[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            g_aeShipSide_0059d650[obj] != g_aeShipSide_0059d650[other]) {
            g_nTargetRange_0059ce10 = distance_from_object(obj, other);
            if (g_nTargetRange_0059ce10 < range) {
                g_nTargetShip_0059c3b0 = other;
                return 1;
            }
        }
    }
    return 0;
}

/* Function start: 0x4232B0 */
int attacker_in_range(short obj, short range)
{
    short other;

    for (other = 0; other < 10; other++) {
        if (g_aeObjectClass_0059d100[other] >= OBJECT_CLASS_SHIP &&
            g_aeSpecialManeuver_0059c3c0[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            g_aeShipSide_0059d650[obj] != g_aeShipSide_0059d650[other] &&
            g_acShipTarget_0059ce60[other] == obj) {
            g_nTargetRange_0059ce10 = distance_from_object(other, obj);
            if ((unsigned short)g_nTargetRange_0059ce10 <
                (unsigned short)range) {
                g_nTargetShip_0059c3b0 = other;
                return 1;
            }
        }
    }
    g_nTargetShip_0059c3b0 = -1;
    return 0;
}

/* Function start: 0x423350 */
int in_danger(short obj)
{
    short other;
    unsigned short range;

    g_nTargetShip_0059c3b0 = -1;
    for (other = 0; other < 10; other++) {
        if (g_aeObjectClass_0059d100[other] >= OBJECT_CLASS_SHIP &&
            g_aeSpecialManeuver_0059c3c0[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            g_aeShipSide_0059d650[obj] != g_aeShipSide_0059d650[other] &&
            g_acShipTarget_0059ce60[other] == obj) {
            range = (unsigned short)distance_from_object(other, obj);
            if (g_nTargetShip_0059c3b0 == -1 ||
                range < (unsigned short)g_nTargetRange_0059ce10) {
                g_nTargetShip_0059c3b0 = other;
                g_nTargetRange_0059ce10 = (short)range;
            }
        }
    }
    return g_nTargetShip_0059c3b0 != -1;
}

/* Function start: 0x423530 */
void inherit_leader_mission(short obj)
{
    short leader = g_asShipWingLeader_0059d400[obj];

    if (leader != -1 &&
        g_aeObjectClass_0059d100[obj] >= OBJECT_CLASS_SHIP) {
        g_aeShipMissionType_0059c3f0[obj] =
            g_aeShipMissionType_0059c3f0[leader];
        g_anShipMissionShip_0059d4b0[obj] =
            g_anShipMissionShip_0059d4b0[leader];
        g_asShipWingLeader_0059d400[obj] = -1;
        ((FixedVector *)g_aShipMissionSpot_0059dd10)[obj] =
            ((FixedVector *)g_aShipMissionSpot_0059dd10)[leader];
    }
}

/* Function start: 0x4235B0 */
void inherit_leader(short obj)
{
    short leader = g_asShipWingLeader_0059d400[obj];
    short other;

    if (leader == -1 || g_aeObjectClass_0059d100[obj] < OBJECT_CLASS_SHIP)
        return;
    inherit_leader_mission(obj);
    g_asShipWingLeader_0059d400[obj] = g_asShipWingLeader_0059d400[leader];
    for (other = 0; other < 10; other++) {
        if (g_asShipWingLeader_0059d400[other] == leader)
            g_asShipWingLeader_0059d400[other] = obj;
    }
}

/* Function start: 0x423610 */
unsigned int GetObjectRecordField(short i)
{
    if (i != -1 && DAT_0046c97a[i * 0x36] != 3)
        return 0;
    return 1;
}

/* Function start: 0x423640 */
int gone_ship(short missionShip)
{
    if (missionShip != -1 && DAT_0046c97a[missionShip * 0x36] != 3 &&
        DAT_0046c97a[missionShip * 0x36] != 2)
        return 0;
    return 1;
}

/* Function start: 0x423820 */
unsigned int get_follow_point(short obj, FixedVector *point)
{
    short pathIndex;
    short objective;
    short missionShip;
    int type;

    if (g_aeShipSide_0059d650[obj] == SIDE_KILRATHI) {
        *point = g_aMissionNavPoints_0046c2f0[
            g_anShipMissionShip_0059d4b0[obj]].position;
        return 0;
    }
    pathIndex = (short)g_abShipNavPointIndex_0059d7c0[obj];
    while (++pathIndex <= 15) {
        objective = (short)g_abFlightPath_0059c000[pathIndex];
        type = *(int *)((unsigned char *)g_abMissionObjectiveType_0059dac5 +
                        objective * 0x1f);
        if (type == 0) {
            *point = *(FixedVector *)((unsigned char *)
                g_abMissionObjectiveType_0059dac5 + objective * 0x1f + 0x0e);
            g_abShipNavPointIndex_0059d7c0[obj] = (signed char)pathIndex;
            return 0;
        }
        if (type == 1) {
            missionShip = (short)*(signed char *)((unsigned char *)
                g_abMissionObjectiveType_0059dac5 + objective * 0x1f + 4);
            objective = find_ship_index(missionShip);
            if (objective == -1)
                *point = g_aMissionNavPoints_0046c2f0[missionShip].position;
            else
                *point = g_aShipPosition_0059c490[objective];
            g_abShipNavPointIndex_0059d7c0[obj] = (signed char)pathIndex;
            return 0;
        }
    }
    return 0;
}

/* Function start: 0x423930 */
unsigned int get_first_follow_point(short obj, FixedVector *point)
{
    if (g_aeShipSide_0059d650[obj] == SIDE_IMPERIAL)
        g_abShipNavPointIndex_0059d7c0[obj] =
            g_cCurrentNavPointIndex_0059c86c - 1;
    return get_follow_point(obj, point);
}

/* Function start: 0x423970 */
int ship_can_reach_nav_point(short obj, short navPoint)
{
    short index;
    short missionShip;
    short *ships = (short *)((unsigned char *)&
        g_aMissionNavPoints_0046c2f0[navPoint] + 0x3d);

    for (index = 0; index < 10; index++) {
        missionShip = ships[index];
        if (missionShip != -1 &&
            g_aeShipSide_0059d650[obj] !=
                *(enum Side *)(&DAT_0046c97a[missionShip * 0x36] - 0x2e))
            return 1;
    }
    return 0;
}

/* Function start: 0x4239D0 */
int abandoned(short obj, short other)
{
    short navPoint;

    if ((g_abShipTurn_0059d860[obj] & 7) == 0 &&
        g_aeShipSide_0059d650[obj] != SIDE_KILRATHI &&
        RandomBelowOrEqual(8) == 0) {
        navPoint = FindNearestNavPoint(obj);
        if (g_nCurrentNavPoint_0059df60 != navPoint &&
            ship_can_reach_nav_point(obj, navPoint) != 0 &&
            distance_from_object(obj, other) > 10000) {
            explode(-1, obj);
            return 1;
        }
    }
    return 0;
}

/* Function start: 0x423A50 */
void engage(short obj, signed char target, enum ShipObjective objective)
{
    if (g_aeShipObjective_0059d200[obj] != objective) {
        reset_objective(obj, objective);
        if (g_acShipRating_0059cd80[obj] > 8 &&
            TestShipFlags((short)g_aiPilotLevel_0059cf30[obj] - 14, 8) == 0)
            prepare_ace_engagement(obj);
    }
    g_acShipTarget_0059ce60[obj] = target;
}

/* Function start: 0x423AC0 */
int target_valid(short obj)
{
    short target = (short)g_acShipTarget_0059ce60[obj];

    return unactive(target) == 0 &&
           g_aeShipSide_0059d650[target] != g_aeShipSide_0059d650[obj];
}

/* Function start: 0x423BA0 */
short InterpolateClamped(short inputMinimum, short inputMaximum,
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
    int typeOffset = g_aeObjectType_0059b560[obj] * 0x87;

    if (g_aeObjectClass_0059d100[obj] < OBJECT_CLASS_SHIP)
        return 100;
    return (short)((g_asShipDamage_0059c460[obj] * -26) /
                       *(short *)(g_aObjectTypeData_00466460 +
                                  typeOffset + 0x0c) +
                   (g_asShipWingLeader_0059d400[obj * 4 + 0x11] * 27) /
                       *(short *)(g_aObjectTypeData_00466460 +
                                  typeOffset + 0x71) +
                   (g_asShipWingLeader_0059d400[obj * 4 + 0x10] * 23) /
                       *(short *)(g_aObjectTypeData_00466460 +
                                  typeOffset + 0x6f) +
                   (g_asShipWingLeader_0059d400[obj * 4 + 0x12] * 12) /
                       *(short *)(g_aObjectTypeData_00466460 +
                                  typeOffset + 0x73) +
                   (g_asShipWingLeader_0059d400[obj * 4 + 0x13] * 12) /
                       *(short *)(g_aObjectTypeData_00466460 +
                                  typeOffset + 0x75) + 26);
}

/* Function start: 0x423CD0 */
short find_space_mine(short obj)
{
    return find_weapon(obj, OBJECT_TYPE_SPACE_MINE);
}

/* Function start: 0x424B80 */
unsigned int GetPilotNameLength(void)
{
    short i = 0;

    do {
        remove_object(i);
        i = i + 1;
    } while (i < 0x40);
    return 0;
}

/* Function start: 0x424BA0 */
unsigned int GetPilotRecordBase(void)
{
    if (DAT_00469d5c == 0)
        return 0;
    DAT_00469d5c = 0;
    GetScreenUpdateFlag();
    DAT_0046a004 = 0;
    ReleaseSceneActors();
    ReleaseAllSfxSlots();
    GetPilotNameLength();
    ResetSeriesProgress();
    return 0;
}

/* Function start: 0x424C60 */
unsigned int ResetSceneFlags(void)
{
    DAT_0046aa3c = 0;
    DAT_0046aa40 = 1;
    DAT_0046aa38 = 1;
    return 0;
}

/* Function start: 0x424CE0 */
unsigned int MouseHide(void)
{
    return 0;
}

/* Function start: 0x424CF0 */
unsigned int MouseShow(void)
{
    return 0;
}
