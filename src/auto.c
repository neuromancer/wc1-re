/*
 *  Autopilot travel and interstitial flight sequences.
 *
 *  Address range 0x403e50-0x40460f (provisional -- see docs/ORDER.md).
 *  Boundary evidence: the Mac auto unit preserves visit_the_cinema,
 *  player_wingman, set_speed, auto_position, and auto_pilot_sequence in this
 *  exact order; the next Win32 function is a palette-file loader in another
 *  compilation unit.
 */
#include "wc1.h"

/* Function start: 0x403E50 */
void visit_the_cinema(int view, int obj, short frames)
{
    int savedOriginUnlock;
    int savedInputFlag;
    int savedKeyboardFlag;

    savedOriginUnlock = g_nOriginDevUnlock_00469ff4;
    savedInputFlag = DAT_00469ffc;
    savedKeyboardFlag = DAT_0046a000;
    g_nOriginDevUnlock_00469ff4 = 1;
    DAT_00469ffc = 0;
    DAT_0046a000 = 0;
    force_view(view, obj);
    while (frames-- > 0) {
        Update_3Space();
        RenderSpaceViewFrame();
        DIBslam();
        DIBslamReal();
    }
    g_nOriginDevUnlock_00469ff4 = savedOriginUnlock;
    DAT_00469ffc = savedInputFlag;
    DAT_0046a000 = savedKeyboardFlag;
}

/* Function start: 0x403EE0 */
unsigned int player_wingman(short obj)
{
    if (obj != -1 && g_asShipWingLeader_0059d400[obj] == 0)
        return 1;
    return 0;
}

/* Function start: 0x403F10 */
void set_speed(short obj, short speed)
{
    g_anShipSpeed_0059b320[obj] = (int)speed << 8;
    fix_velocity(obj);
}

/* Function start: 0x403F40 */
void auto_position(short obj, short *formationSlot)
{
    short ship = obj;
    short lateral;
    short vertical;
    short forward;

    if (player_wingman(ship) != 0) {
        position_relative_ijk(&g_aShipPosition_0059c490[ship], 0,
                              g_aShipFormationOffset_0059b520[ship].x,
                              g_aShipFormationOffset_0059b520[ship].y,
                              g_aShipFormationOffset_0059b520[ship].z);
        return;
    }

    (*formationSlot)++;
    lateral = ((*formationSlot & 1) >= 1) ? 650 : -650;
    if (g_nAutopilotFormationShipCount_00465544 == *formationSlot)
        lateral = 0;
    vertical = 0;
    forward = MaxShort(1, (short)(*formationSlot >> 1));
    forward = (short)(forward * -1800);
    if (g_nYourWingman_0046c04c != -1) {
        int radii;
        int separation;

        radii = g_asObjectCollisionRadius_0059d710[ship] +
                g_asObjectCollisionRadius_0059d710[g_nYourWingman_0046c04c];
        separation = abs(
            g_aShipFormationOffset_0059b520[g_nYourWingman_0046c04c].z -
            forward);
        if (radii > separation)
            vertical = 500;
    }
    position_relative_ijk(&g_aShipPosition_0059c490[ship], 0,
                          lateral, vertical, forward);
}

/* Function start: 0x404050 */
void auto_pilot_sequence(void)
{
    short savedCannedSceneMode;
    short leaveCurrentNavPoint;
    short formationSlot;
    FixedVector destination;
    signed char travelMode[10];
    FixedVector travelStep;
    short ship;
    short initialDistance;
    short destinationRange;
    short nearestShipRange;
    short cruiseSpeed;
    short other;

    savedCannedSceneMode = (short)g_nCannedSceneMode_00469fac;
    formationSlot = 0;
    destination = g_aMissionObjectives_0059dac5[
        g_abFlightPath_0059c000[g_cCurrentNavPointIndex_0059c86c]].position;
    leaveCurrentNavPoint = 1;

    if (CanEngageAutopilot(1) != 0) {
        if (distance_between_points(
                &g_aMissionObjectives_0059dac5[
                    g_cCurrentObjective_0046c020].position,
                &g_aMissionNavPoints_0046c2f0[
                    g_nCurrentNavPoint_0059df60].position) <
            g_aMissionNavPoints_0046c2f0[
                g_nCurrentNavPoint_0059df60].proximityRadius + 25)
            leaveCurrentNavPoint = 0;

        clean_up_cockpit();
        ResetSoundState();
        g_nAutopilotFormationShipCount_00465544 = 0;

        ship = 0;
        do {
            travelMode[ship] = 0;
            g_anShipSpeed_0059b320[ship] = 0;
            zero_vector(&g_aShipVelocity_0059c010[ship]);
            if (g_aeObjectClass_0059d100[ship] >= OBJECT_CLASS_SHIP &&
                g_aeSpecialManeuver_0059c3c0[ship] !=
                    SPECIAL_MANEUVER_UNKNOWN_9 &&
                g_aeShipSide_0059d650[ship] == SIDE_IMPERIAL) {
                if (IsInitialMissionShip(
                        g_nShipMissionIndices_0059c830[ship]) != 0) {
                    if (KilrathiShipWithinRange(ship, 10000) == 0) {
                        travelMode[ship] = -1;
                        g_aeSpecialManeuver_0059c3c0[ship] =
                            SPECIAL_MANEUVER_NONE;
                        g_anRollGoal_0059d630[ship] = 0;
                        g_anObjectRollRotation_0059d7e0[ship] = 0;
                        g_anPitchGoal_0059d7a0[ship] = 0;
                        g_anObjectPitchRotation_0059b2a0[ship] = 0;
                        g_anYawGoal_0059c310[ship] = 0;
                        g_anObjectYawRotation_0059ce80[ship] = 0;
                        if (ship != 0 &&
                            ship != g_nYourWingman_0046c04c)
                            g_nAutopilotFormationShipCount_00465544++;
                    }
                } else if (leaveCurrentNavPoint != 0) {
                    remove_object(ship);
                }
            }
            ship++;
        } while (ship < 10);

        initialDistance = distance_from_point(0, &destination);
        point_at(0, destination);
        set_speed(0, 60);
        travelMode[0] = 1;
        formationSlot = 0;

        ship = 0;
        do {
            if (travelMode[ship] == -1 && ship != 0) {
                if (distance_from_object(ship, 0) > 20000) {
                    if (player_wingman(ship) != 0)
                        travelMode[ship] = 1;
                    else if (equ_vector(
                                 &g_aShipDestination_0059d530[ship],
                                 &destination) != 0)
                        travelMode[ship] = 2;
                    else
                        travelMode[ship] = 3;
                } else {
                    if (player_wingman(ship) != 0 ||
                        equ_vector(&g_aShipDestination_0059d530[ship],
                                   &destination) != 0)
                        travelMode[ship] = 1;
                    else
                        travelMode[ship] = 3;
                }
                if (travelMode[ship] == 1) {
                    auto_position(ship, &formationSlot);
                    ScaleFixedVector(&g_aShipForwardVector_0059bce0[0],
                                     0x3c00,
                                     &g_aShipVelocity_0059c010[ship]);
                    copy_frame(0, ship);
                    g_anShipSpeed_0059b320[ship] =
                        g_anShipSpeed_0059b320[0];
                    g_aShipVelocity_0059c010[ship] =
                        g_aShipVelocity_0059c010[0];
                }
            }
            ship++;
        } while (ship < 10);

        g_nCannedSceneMode_00469fac = 4;
        visit_the_cinema(12, 0, 120);
        while (g_nCannedSceneMode_00469fac == 4) {
            ComputeVectorDelta(&g_aShipPosition_0059c490[0],
                               &destination, &travelStep);
            NormalizeFixedVector(&travelStep);
            ScaleFixedVector(&travelStep, 0x19000, &travelStep);
            AddFixedVectors(&g_aShipPosition_0059c490[0], &travelStep,
                            &g_aShipPosition_0059c490[0]);
            ReleaseStaleNavTarget();
            check_hazards();

            nearestShipRange = 0x7fff;
            destinationRange = distance_from_point(0, &destination);
            other = 0;
            do {
                if (g_aeObjectClass_0059d100[other] >= OBJECT_CLASS_SHIP &&
                    travelMode[other] == 0)
                    nearestShipRange = MinShort(
                        nearestShipRange, distance_from_object(0, other));
                other++;
            } while (other < 10);

            if ((unsigned short)destinationRange < 1000 ||
                g_pActiveHazardField_0059bfe0 != 0 ||
                nearestShipRange < 4000 ||
                report_kilrathi_rout(1) != 0)
                g_nCannedSceneMode_00469fac = savedCannedSceneMode;
        }

        SubtractFixedVectors(&g_aShipPosition_0059c490[0], &travelStep,
                             &g_aShipPosition_0059c490[0]);
        cruiseSpeed = g_asShipMaximumSpeed_0059c440[0];
        ship = 0;
        do {
            if (travelMode[ship] != 0 &&
                g_aObjectTypeData_00466458[
                    g_aeObjectType_0059b560[ship]].cruiseVelocity <
                    cruiseSpeed)
                cruiseSpeed = g_aObjectTypeData_00466458[
                    g_aeObjectType_0059b560[ship]].cruiseVelocity;
            ship++;
        } while (ship < 10);

        formationSlot = 0;
        ship = 0;
        do {
            if (travelMode[ship] != 0) {
                set_speed(ship, cruiseSpeed);
                if (ship != 0 && travelMode[ship] >= 1) {
                    switch (travelMode[ship]) {
                    case 1:
                    case 2:
                        auto_position(ship, &formationSlot);
                        break;
                    case 3:
                        if (distance_from_point(
                                ship,
                                &g_aShipDestination_0059d530[ship]) <
                            initialDistance)
                            g_aShipPosition_0059c490[ship] =
                                g_aShipDestination_0059d530[ship];
                        break;
                    }
                }
            }
            ship++;
        } while (ship < 10);

        Update_3Space();
        if (DAT_0046a008 == 0) {
            force_view(0, 0);
            SetMousePosition(
                (DAT_005a7510.right - DAT_005a7510.left) / 2 + 1,
                (DAT_005a7510.bottom - DAT_005a7510.top) / 2);
        } else {
            GetScreenUpdateFlag();
            SetViewportRect(&DAT_005a7510, 0, 0,
                            (unsigned short)(g_nScreenWidth_0046daa4 - 1),
                            (unsigned short)(g_nScreenHeight_0046daa8 - 1));
            initialize_view_buffer();
            DAT_0046a008 = 1;
            force_view(0, 0);
            DAT_0046a008 = 1;
            GetScreenUpdateFlag();
            SetViewportRect(&DAT_005a7510, 0, 0, 319, 199);
            initialize_view_buffer();
            SetMousePosition(
                (DAT_005a7510.right - DAT_005a7510.left) / 2,
                g_nViewCenterY_0059a854);
        }
    }
}
