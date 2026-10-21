/*
 *  Autopilot travel and interstitial flight sequences.
 *
 *  Address range 403e50-40469f (provisional -- see docs/ORDER.md).
 *  Boundary evidence: the Mac auto unit preserves visit_the_cinema,
 *  player_wingman, set_speed, auto_position, and auto_pilot_sequence in this
 *  exact order. The palette-file loader follows in this unit: its "rb"
 *  literal follows the initialized formation count in the original data.
 */
#include "wc1.h"

/* Linker commons defined here; LINK orders them by identifier hash. */
unsigned int nSystemKeyDown; /* 0x005a8964 */
JOYINFO aJoystickInfos[2]; /* 0x005a8970 */
int nHostMouseMessageX; /* 0x005a8990 */

/* Function start: 0x403E50 */
void visit_the_cinema(int view, int obj, short frames)
{
    int savedOriginUnlock;
    int savedInputFlag;
    int savedKeyboardFlag;

#ifdef SDL_PORT
    /* SWC's travel state is shared below; its movie/space presentation is
       owned by the SDL host, not the WC1 viewport renderer. */
    if (SdlSwcFlightActive()) {
        nSwcCinemaResult = SdlSwcVisitCinema(view, obj, frames);
        return;
    }
#endif
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
    if (obj != -1 && asShipWingLeaderArray[obj] == 0)
        return 1;
    return 0;
}

/* Function start: 0x403F10 */
void set_speed(short obj, short speed)
{
    anShipSpeed[obj] = (int)speed << 8;
    fix_velocity(obj);
}

static short nAutopilotFormationShipCount = 0; /* 0x00465544 */

/* Function start: 0x403F40 */
void auto_position(short obj, short *formationSlot)
{
    short ship = obj;
    short lateral;
    short vertical;
    short forward;

    if (player_wingman(ship) != 0) {
        position_relative_ijk(&aShipPositionsArray[ship], 0,
                              aShipFormationDeltaList[ship].x,
                              aShipFormationDeltaList[ship].y,
                              aShipFormationDeltaList[ship].z);
        return;
    }

    (*formationSlot)++;
    lateral = (*formationSlot & 1) ? 650 : -650;
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
            aShipFormationDeltaList[nYourWingman].z -
            forward);
        if (radii > separation)
            vertical = 500;
    }
    position_relative_ijk(&aShipPositionsArray[ship], 0,
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
    short other;
    short destinationRange;
    short cruiseSpeed;
    short nearestShipRange;
#ifdef SDL_PORT
    FixedVector previousPosition;
#endif

    savedCannedSceneMode = (short)nCannedSceneMode;
    destination = aMissionObjectivesArray[
        abFlightPathList[cCurrentNavPtIdx]].position;
    formationSlot = 0;
    leaveCurrentNavPoint = 1;

    if (auto_pilot_valid(1) != 0) {
        if (distance_between_points(
                &aMissionObjectivesArray[
                    cCurrentObjective].position,
                &aMissionNavPoints[
                    nCurrentNavigationPoint].position) <
            aMissionNavPoints[
                nCurrentNavigationPoint].proximityRadius + 25)
            leaveCurrentNavPoint = 0;

        clean_up_cockpit();
        ResetSoundState();
        nAutopilotFormationShipCount = 0;

        for (ship = 0; ship < 10; ship++) {
            travelMode[ship] = 0;
            anShipSpeed[ship] = 0;
            zero_vector(&aShipVelocity[ship]);
            if (aeObjectClass[ship] >= OBJECT_CLASS_SHIP &&
                aeSpecialManeuvers[ship] !=
                    SPECIAL_MANEUVER_UNKNOWN_9 &&
                aeShipSide[ship] == SIDE_IMPERIAL) {
                if (is_team_member(
                        nShipMissionIndices[ship]) != 0) {
                    if (kilrathi_near(ship, 10000) == 0) {
                        travelMode[ship] = -1;
                        aeSpecialManeuvers[ship] =
                            SPECIAL_MANEUVER_NONE;
                        anRollGoal[ship] = 0;
                        anObjectRollRotationList[ship] = 0;
                        anPitchGoal[ship] = 0;
                        anObjPitchRot[ship] = 0;
                        anYawTarget[ship] = 0;
                        anObjectYawRotation[ship] = 0;
#ifdef SDL_PORT
                        /* CODE_14 +0x03a8: release the player's gun timer. */
                        if (SdlSwcFlightActive() && ship == 0)
                            asObjectCounterArray[0] = -1;
#endif
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
                                 &aShipDestinationsArray[ship],
                                 &destination) != 0)
                        travelMode[ship] = 2;
                    else
                        travelMode[ship] = 3;
                } else {
                    if (player_wingman(ship) != 0 ||
                        equ_vector(&aShipDestinationsArray[ship],
                                   &destination) != 0)
                        travelMode[ship] = 1;
                    else
                        travelMode[ship] = 3;
                }
                if (travelMode[ship] == 1) {
                    auto_position(ship, &formationSlot);
                    ScaleFixedVector(&aShipFwdVec[0],
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
#ifdef SDL_PORT
        if (SdlSwcFlightActive() && nSwcCinemaResult != 0) {
            nCannedSceneMode = savedCannedSceneMode;
            return;
        }
#endif
        while (nCannedSceneMode == 4) {
            ComputeVectorDelta(&aShipPositionsArray[0],
                               &destination, &travelStep);
            NormalizeFixedVector(&travelStep);
            ScaleFixedVector(&travelStep, 0x19000, &travelStep);
            AddFixedVectors(&aShipPositionsArray[0], &travelStep,
                            &aShipPositionsArray[0]);
            ReleaseStaleNavTarget();
            check_hazards();

            destinationRange = distance_from_point(0, &destination);
            nearestShipRange = 0x7fff;
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

#ifdef SDL_PORT
        previousPosition = aShipPositionsArray[0];
#endif
        SubtractFixedVectors(&aShipPositionsArray[0], &travelStep,
                             &aShipPositionsArray[0]);
#ifdef SDL_PORT
        /* CODE_14 +0x0648: retain the arrival position when reversing the
           last step would disagree with the active navigation sphere. */
        if (SdlSwcFlightActive() && FindNearestNavPoint(0) != nCurrentNavigationPoint)
            aShipPositionsArray[0] = previousPosition;
#endif
        cruiseSpeed = asShipMaximumSpeedArray[0];
        for (ship = 0; ship < 10; ship++) {
            if (travelMode[ship] != 0) {
                short shipCruiseSpeed;

                shipCruiseSpeed = aObjectTypeData[
                    aeObjectTypeList[ship]].cruiseVelocity;
                if (shipCruiseSpeed < cruiseSpeed)
                    cruiseSpeed = shipCruiseSpeed;
            }
        }

        formationSlot = 0;
        for (ship = 0; ship < 10; ship++) {
            if (travelMode[ship] != 0) {
                set_speed(ship, cruiseSpeed);
                if (ship != 0) {
                    switch (travelMode[ship]) {
                    case 1:
                    case 2:
                        auto_position(ship, &formationSlot);
                        break;
                    case 3:
                        if (distance_from_point(
                                ship,
                                &aShipDestinationsArray[ship]) <
                            initialDistance)
                            aShipPositionsArray[ship] =
                                aShipDestinationsArray[ship];
                        break;
                    }
                }
            }
        }

#ifdef SDL_PORT
        if (SdlSwcFlightActive()) {
            /* CODE_14 +0x077e advances one final frame after travel. */
            SdlSwcUpdateSpace();
            return;
        }
#endif
        Update_3Space();
        if (bCockpitlessView == 0) {
            force_view(0, 0);
            SetMousePosition(
                (stSpaceBufferViewport.right - stSpaceBufferViewport.left) / 2 + 1,
                (stSpaceBufferViewport.bottom - stSpaceBufferViewport.top) / 2);
        } else {
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0,
                            (unsigned short)(nScreenWidth - 1),
                            (unsigned short)(nScreenHeight - 1));
            initialize_view_buffer();
            bCockpitlessView = 1;
            force_view(0, 0);
            bCockpitlessView = 1;
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
            initialize_view_buffer();
            SetMousePosition(
                (stSpaceBufferViewport.right - stSpaceBufferViewport.left) / 2,
                nCurrentViewCenterPosY);
        }
    }
}

/* Function start: 0x404610 */
unsigned short __stdcall LoadPaletteTripletsFile(const char *path)
{
    unsigned char *palette;
    FILE *file;
#ifdef SDL_PORT
    char resolvedPath[PATH_MAX];
#endif

    palette = AllocateTaggedMemory(0x300, 0);
    if (palette == 0)
        return 0;
#ifdef SDL_PORT
    if (SdlResolvePath(path, resolvedPath, sizeof(resolvedPath)))
        file = fopen(resolvedPath, "rb");
    else
        file = 0;
#else
    file = fopen(path, "rb");
#endif
    if (file != 0) {
        fseek(file, 0x30, SEEK_SET);
        fread(palette, 0x300, 1, file);
        SetWholePaletteFromTriplets(palette);
        fclose(file);
        ReleasePacketHandle(palette);
        return 1;
    }
    ReleasePacketHandle(palette);
    return 0;
}
