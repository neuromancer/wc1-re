/*
 *  On-screen message banners and the debug cheat keys.
 *
 *  Address range 0x428000-0x42afff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: ShowOnScreenMessage and its six callers; string band 0x46A24C-0x46A378.
 *  The Mac CODE 4 `targ` symbols prove the nested 0x42A8F0-0x42ACFF unit.
 */
#include "wc1.h"

/* Function start: 0x428E70 */
/* Message dwell time: grows with text length, scaled by the speed setting. */
short MeasureMessageWidth(const char *text)
{
    short n = MinShort(5, DosStrlen(text) >> 1);

    return (n + 5) * ((char)g_bMessageSpeed_0046af68 + 1);
}

/* Function start: 0x428F80 */
void ReportOutOfMemoryAndExit(void)
{
    ShowModalMessage("ERROR: Out of memory for %s");
    FatalErrorAndExit("You do not have enough memory to run Wing Commander.");
}

/* Function start: 0x4290D0 */
void ShowVersionBanner(void)
{
    ShowOnScreenMessage(1, 9999, "WING COMMANDER VER. %s");
}

/* Function start: 0x429E70 */
void ComputeArcadeTimeBonus(void)
{
    DAT_005a7c2c = (DAT_00469e34 + 6) * 400;
}

/* Function start: 0x42A0C0 */
void RefreshCockpitStatus(void)
{
    RunSimulationTick();
    if (DAT_00469fb4 < 2)
        DrawStatusBarBackdrop();
    RunFrameUpdate();
}

/* Function start: 0x42A0E0 */
short GetShipDistanceToNavPoint(short ship, MissionNavPoint *navPoint)
{
    FixedVector delta;

    ComputeVectorDelta(&g_aShipPosition_0059c490[ship],
                       &navPoint->position, &delta);
    return FixedToShortSaturating((int)ComputeFixedVectorMagnitude(&delta));
}

/* Function start: 0x42A120 */
short FindNearestNavPoint(short ship)
{
    short navPointIndex = 0;
    MissionNavPoint *navPoint = g_aMissionNavPoints_0046c2f0;

    do {
        if (navPoint->type == 1 &&
            GetShipDistanceToNavPoint(ship, navPoint) < navPoint->proximityRadius)
            return navPointIndex;
        navPointIndex++;
        navPoint++;
    } while (navPointIndex < WC1_MISSION_NAV_POINT_COUNT);

    return g_nCurrentNavPoint_0059df60;
}

/* Function start: 0x42A170 */
unsigned int ReleaseStaleNavTarget(void)
{
    short v = FindNearestNavPoint(0);

    if (g_nCurrentNavPoint_0059df60 != v)
        EnterNavPoint(v);
    return 0;
}

/* Function start: 0x42A670 */
void RedrawCommWindow(void)
{
    SetMouseCursorShape(DAT_0059ab19, 0);
}

/* Function start: 0x42A8F0 */
short find_objective(int type, short index)
{
    short objective;

    objective = 0;
    while (objective < (short)g_cMissionObjectiveCount_0059c46a) {
        if (g_aMissionObjectives_0059dac5[objective].type == type) {
            if (index == -1)
                return objective;
            if (g_aMissionObjectives_0059dac5[objective].index == index)
                return objective;
        }
        objective++;
    }
    return -1;
}

/* Function start: 0x42A950 */
void arrive_from_warp(short obj)
{
    short objective = find_objective(0, g_nCurrentNavPoint_0059df60);

    if (objective != -1) {
        if (g_aMissionObjectives_0059dac5[
                g_abFlightPath_0059c000[objective]].type != 1)
            visit(objective, 1);
        if (g_cCurrentObjective_0046c020 == objective)
            set_next_destination();
    }
    place_ship_near_player_until_valid(obj, 2000, 5000);
    unwarp(obj);
    g_anShipSpeed_0059b320[obj] =
        (int)g_asShipMaximumSpeed_0059c440[obj] << 8;
    fix_velocity(obj);
    if (g_aeShipSide_0059d650[obj] == SIDE_IMPERIAL)
        reset_mission_type(obj, MISSION_TYPE_COME_HOME);
    else
        reset_mission_type(obj, MISSION_TYPE_PATROL);
}

/* Function start: 0x42AA10 */
unsigned int unwarp(short obj)
{
    short effect;

    ClearViewport(&DAT_005a7510, g_cViewportClearColour_004699a0);
    g_bViewportDirty_00469fc4 = 1;
    effect = find_vacant_3d_object();
    if (effect != -1) {
        initialize_object(effect, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH, obj);
        g_aShipPosition_0059c490[effect] = g_aShipPosition_0059c490[obj];
        g_aShipVelocity_0059c010[effect] = g_aShipVelocity_0059c010[obj];
        g_aeShipManeuver_0059dcb0[obj] = MANEUVER_NONE;
        g_asObjectCounter_0059c330[obj] = 6;
        return 0;
    }
    g_abShipNavPointIndex_0059d7c0[obj] =
        (signed char)g_aeObjectType_0059b560[obj];
    initialize_object(obj, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH, obj);
    return 0;
}

/* Function start: 0x42AAF0 */
unsigned int warp(short obj)
{
    short effect;

    ClearViewport(&DAT_005a7510, g_cViewportClearColour_004699a0);
    g_bViewportDirty_00469fc4 = 1;
    effect = find_vacant_3d_object();
    if (effect != -1) {
        initialize_object(effect, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH,
                          g_acObjectOwner_0059ce20[obj]);
        g_aShipPosition_0059c490[effect] = g_aShipPosition_0059c490[obj];
        g_aShipVelocity_0059c010[effect] = g_aShipVelocity_0059c010[obj];
        g_aeShipManeuver_0059dcb0[obj] = MANEUVER_WARPING_OUT;
        g_asObjectCounter_0059c330[obj] = 6;
        return 0;
    }
    initialize_object(obj, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH,
                      g_acObjectOwner_0059ce20[obj]);
    return 0;
}

/* Function start: 0x42ABD0 */
int drop_player_mine(short obj)
{
    short weapon;

    weapon = 0;
    while (weapon < (signed char)g_aShipWeapons_0059cab0[obj][0]) {
        ShipWeaponSlot *weaponSlot =
            &((ShipWeaponSlot *)&g_aShipWeapons_0059cab0[obj][1])[weapon];
        enum ObjectType type = weaponSlot->type;

        if (g_aObjectTypeData_0046645c[type].objectClass ==
                OBJECT_CLASS_MINE &&
            weaponSlot->disabled == 0)
            return drop_mine(obj, weapon, type, 20);
        weapon++;
    }
    return -1;
}

/* Function start: 0x42AC50 */
unsigned int personality_killed(short personality)
{
    if (personality < 8) {
        g_aiPersonalityDeathMission_0059ca74[personality] =
            (int)g_cCurrentMission_0059ca69 +
            (int)g_cCurrentSeries_0059ca6a * 4;
        g_nPromotionScore_0059caa0 =
            MaxShort(0, g_nPromotionScore_0059caa0 - 1);
        return 0;
    }
    kill_ace(personality - 9);
    g_nPromotionScore_0059caa0++;
    g_nMissionScore_0059caa2 += 25;
    return 0;
}

/* Function start: 0x42ACC0 */
void clean_up_cockpit(void)
{
    short wingman = g_nYourWingman_0046c04c;

    g_acShipTarget_0059ce60[0] = -1;
    g_nTargetLockMode_0046c078 = 0;
    if (wingman != -1) {
        g_nAutoEngageTimer_0046c084 = -1;
        g_acShipTarget_0059ce60[wingman] = -1;
        reset_objective(wingman, OBJECTIVE_HOLD_FORMATION);
    }
    ClearHudGunReadouts();
}

/* Function start: 0x42AFA0 */
void WaitForDebugStep(void)
{
    while (TakeDebugStepFlag() == 0) ;
}

/* Function start: 0x42AFB0 */
void CALLBACK FrameTimerCallback(UINT timerId, UINT message,
                                 DWORD user, DWORD first, DWORD second)
{
    (void)timerId;
    (void)message;
    (void)user;
    (void)first;
    (void)second;
    DAT_0059ab3c = 0;
}

/* Function start: 0x42AFC0 */
void SetMultimediaTimerCallback(int period)
{
    int milliseconds = period * 1000 / 60;

    if (period == 0) {
        DAT_0059ab3c = 0;
        if (DAT_005a77ec != 0) {
            timeKillEvent(DAT_005a77ec);
            DAT_005a77ec = 0;
        }
    } else {
        if (DAT_005a77ec != 0) {
            timeKillEvent(DAT_005a77ec);
            DAT_005a77ec = 0;
        }
        DAT_0059ab3c = 1;
        DAT_005a77ec = timeSetEvent(milliseconds, milliseconds,
                                    FrameTimerCallback, 0, 0);
    }
}
