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

    savedOriginUnlock = nOriginDevUnlock;
    savedInputFlag = bPlayerVulnerable;
    savedKeyboardFlag = bPlayerCollisionResponse;
    nOriginDevUnlock = 1;
    bPlayerVulnerable = 0;
    bPlayerCollisionResponse = 0;
    /* The retail call converts both int parameters implicitly.  Making the
     * narrowing explicit changes the register setup and loses an exact match. */
    force_view(view, obj);
    while (frames-- > 0) {
        Update_3Space();
        RenderSpaceViewFrame();
        DIBslam();
        DIBslamReal();
    }
    nOriginDevUnlock = savedOriginUnlock;
    bPlayerVulnerable = savedInputFlag;
    bPlayerCollisionResponse = savedKeyboardFlag;
}

/* Function start: 0x403EE0 */
unsigned int player_wingman(short obj)
{
    if (obj != -1 && asShipWingLeader[obj] == 0)
        return 1;
    return 0;
}

/* Function start: 0x403F10 */
void set_speed(short obj, short speed)
{
    anShipSpeed[obj] = (int)speed << 8;
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
        position_relative_ijk(&aShipPosition[ship], 0,
                              aShipFormationOffset[ship].x,
                              aShipFormationOffset[ship].y,
                              aShipFormationOffset[ship].z);
        return;
    }

    (*formationSlot)++;
    lateral = ((*formationSlot & 1) >= 1) ? 650 : -650;
    if (nAutopilotFormationShipCount == *formationSlot)
        lateral = 0;
    vertical = 0;
    forward = MaxShort(1, (short)(*formationSlot >> 1));
    forward = (short)(forward * -1800);
    if (nYourWingman != -1) {
        int radii;
        int separation;

        radii = asObjectCollisionRadius[ship] +
                asObjectCollisionRadius[nYourWingman];
        separation = abs(
            aShipFormationOffset[nYourWingman].z -
            forward);
        if (radii > separation)
            vertical = 500;
    }
    position_relative_ijk(&aShipPosition[ship], 0,
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

    savedCannedSceneMode = (short)nCannedSceneMode;
    formationSlot = 0;
    destination = aMissionObjectives[
        abFlightPath[cCurrentNavPointIndex]].position;
    leaveCurrentNavPoint = 1;

    if (auto_pilot_valid(1) != 0) {
        if (distance_between_points(
                &aMissionObjectives[
                    cCurrentObjective].position,
                &aMissionNavPoints[
                    nCurrentNavPoint].position) <
            aMissionNavPoints[
                nCurrentNavPoint].proximityRadius + 25)
            leaveCurrentNavPoint = 0;

        clean_up_cockpit();
        ResetSoundState();
        nAutopilotFormationShipCount = 0;

        for (ship = 0; ship < 10; ship++) {
            travelMode[ship] = 0;
            anShipSpeed[ship] = 0;
            zero_vector(&aShipVelocity[ship]);
            if (aeObjectClass[ship] >= OBJECT_CLASS_SHIP &&
                aeSpecialManeuver[ship] !=
                    SPECIAL_MANEUVER_UNKNOWN_9 &&
                aeShipSide[ship] == SIDE_IMPERIAL) {
                if (is_team_member(
                        nShipMissionIndices[ship]) != 0) {
                    if (kilrathi_near(ship, 10000) == 0) {
                        travelMode[ship] = -1;
                        aeSpecialManeuver[ship] =
                            SPECIAL_MANEUVER_NONE;
                        anRollGoal[ship] = 0;
                        anObjectRollRotation[ship] = 0;
                        anPitchGoal[ship] = 0;
                        anObjectPitchRotation[ship] = 0;
                        anYawGoal[ship] = 0;
                        anObjectYawRotation[ship] = 0;
                        if (ship != 0 &&
                            ship != nYourWingman)
                            nAutopilotFormationShipCount++;
                    }
                } else if (leaveCurrentNavPoint != 0) {
                    remove_object(ship);
                }
            }
        }

        initialDistance = distance_from_point(0, &destination);
        point_at(0, destination);
        set_speed(0, 60);
        travelMode[0] = 1;
        formationSlot = 0;

        for (ship = 0; ship < 10; ship++) {
            if (travelMode[ship] == -1 && ship != 0) {
                if (distance_from_object(ship, 0) > 20000) {
                    if (player_wingman(ship) != 0)
                        travelMode[ship] = 1;
                    else if (equ_vector(
                                 &aShipDestination[ship],
                                 &destination) != 0)
                        travelMode[ship] = 2;
                    else
                        travelMode[ship] = 3;
                } else {
                    if (player_wingman(ship) != 0 ||
                        equ_vector(&aShipDestination[ship],
                                   &destination) != 0)
                        travelMode[ship] = 1;
                    else
                        travelMode[ship] = 3;
                }
                if (travelMode[ship] == 1) {
                    auto_position(ship, &formationSlot);
                    ScaleFixedVector(&aShipForwardVector[0],
                                     0x3c00,
                                     &aShipVelocity[ship]);
                    copy_frame(0, ship);
                    anShipSpeed[ship] =
                        anShipSpeed[0];
                    aShipVelocity[ship] =
                        aShipVelocity[0];
                }
            }
        }

        nCannedSceneMode = 4;
        visit_the_cinema(12, 0, 120);
        while (nCannedSceneMode == 4) {
            ComputeVectorDelta(&aShipPosition[0],
                               &destination, &travelStep);
            NormalizeFixedVector(&travelStep);
            ScaleFixedVector(&travelStep, 0x19000, &travelStep);
            AddFixedVectors(&aShipPosition[0], &travelStep,
                            &aShipPosition[0]);
            ReleaseStaleNavTarget();
            check_hazards();

            nearestShipRange = 0x7fff;
            destinationRange = distance_from_point(0, &destination);
            for (other = 0; other < 10; other++) {
                if (aeObjectClass[other] >= OBJECT_CLASS_SHIP &&
                    travelMode[other] == 0)
                    nearestShipRange = MinShort(
                        nearestShipRange, distance_from_object(0, other));
            }

            if ((unsigned short)destinationRange < 1000 ||
                pActiveHazardField != 0 ||
                nearestShipRange < 4000 ||
                report_kilrathi_rout(1) != 0)
                nCannedSceneMode = savedCannedSceneMode;
        }

        SubtractFixedVectors(&aShipPosition[0], &travelStep,
                             &aShipPosition[0]);
        cruiseSpeed = asShipMaximumSpeed[0];
        for (ship = 0; ship < 10; ship++) {
            if (travelMode[ship] != 0 &&
                aObjectTypeData[
                    aeObjectType[ship]].cruiseVelocity <
                    cruiseSpeed)
                cruiseSpeed = aObjectTypeData[
                    aeObjectType[ship]].cruiseVelocity;
        }

        formationSlot = 0;
        for (ship = 0; ship < 10; ship++) {
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
                                &aShipDestination[ship]) <
                            initialDistance)
                            aShipPosition[ship] =
                                aShipDestination[ship];
                        break;
                    }
                }
            }
        }

        Update_3Space();
        if (bCockpitlessView == 0) {
            force_view(0, 0);
            SetMousePosition(
                (stSpaceBuffer.right - stSpaceBuffer.left) / 2 + 1,
                (stSpaceBuffer.bottom - stSpaceBuffer.top) / 2);
        } else {
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBuffer, 0, 0,
                            (unsigned short)(nScreenWidth - 1),
                            (unsigned short)(nScreenHeight - 1));
            initialize_view_buffer();
            bCockpitlessView = 1;
            force_view(0, 0);
            bCockpitlessView = 1;
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBuffer, 0, 0, 319, 199);
            initialize_view_buffer();
            SetMousePosition(
                (stSpaceBuffer.right - stSpaceBuffer.left) / 2,
                nViewCenterY);
        }
    }
}
