/*
 * Space-object simulation and the fleet overview camera.
 * The Mac spc symbol run identifies rotate_eye_to_goal through
 * object_intelligence. Placement evidence is recorded in docs/ORDER.md.
 */
#include <signal.h>
#include <sys/stat.h>
#include "wc1.h"

int nCapitalShipViewDistance = 0x7d000; /* 0x00468ff4 */
unsigned char bLandingAuthorized = 0; /* 0x00468ff8 */
#ifndef SDL_PORT
static
#endif
int bAlternateChaseView = 0; /* 0x00468ffc */

/* Linker commons defined here; LINK orders them by identifier hash. */
short asScannerObjectY[11]; /* 0x005a7e80 */
volatile ShortPoint stCockpitExplosionPosition; /* 0x005a7e98 */
short asScannerObjectPositionX[11]; /* 0x005a7ea0 */
unsigned char abCockpitLightGoalArray[7]; /* 0x005a7eb8 */
int nCurrentDamageAlarmSfxHandle; /* 0x005a7ec0 */
short asScannerBackgroundColour[10]; /* 0x005a7ed0 */
unsigned int anCockpitDamageState[4]; /* 0x005a7ef0 */
unsigned short nPendingHudMessageColour; /* 0x005a7f00 */
short nCurrentStarFieldJRotationValue; /* 0x005a7f02 */
short nStarFieldIRotValue; /* 0x005a7f04 */

/* Function start: 0x410740 */
void SetFleetOverviewView(int initializeCockpit)
{
    FixedVector centre;
    FixedVector orientation;
    FixedVector offset;
    int shipCount;
    int range;
    int maximumRange;
    int playerRange;
    int cameraDistance;
    int object;

    orientation.x = 0xff;
    orientation.y = 0xff;
    orientation.z = 0xff;
    shipCount = 0;
    if (initializeCockpit != 0) {
        GetScreenUpdateFlag();
        if ((int)GetFixedOneMillionThunkAlt(0) < 66000)
            initialize_cockpit(4);
        else
            initialize_cockpit(6);
        zero_vector(&aShipVelocity[EYE_OBJECT]);
    }

    object = 0;
    do {
        if (aeObjectClass[object] >= OBJECT_CLASS_SHIP)
            shipCount++;
        object++;
    } while (object < 10);

    if (shipCount > 1) {
        zero_vector(&centre);
        object = 0;
        do {
            if (aeObjectClass[object] >= OBJECT_CLASS_SHIP) {
                centre.x += aShipPositionsArray[object].x / shipCount;
                centre.y += aShipPositionsArray[object].y / shipCount;
                centre.z += aShipPositionsArray[object].z / shipCount;
            }
            object++;
        } while (object < 10);

        maximumRange = 0x4b000;
#ifdef SDL_PORT
        /* The native compiler overlays this range slot with orientation.x. */
        playerRange = orientation.x;
#endif
        object = 9;
        do {
            if (aeObjectClass[object] >= OBJECT_CLASS_SHIP) {
                ComputeVectorDelta(&centre,
                                   &aShipPositionsArray[object],
                                   &offset);
                range = Vector_magnitude(&offset);
                if (maximumRange < range)
                    maximumRange = range;
                if (object == 0)
                    playerRange = range;
            }
            object--;
        } while (object >= 0);
        if (maximumRange <= 0x1f4000)
            playerRange = maximumRange;
    } else {
        playerRange = 0x4b000;
        maximumRange = playerRange;
        position_relative_ijk(&centre, 0, 400, 400, 400);
    }

    cameraDistance = (playerRange >> 3) * 9 + 0x2bc00;
    aShipPositionsArray[63] = centre;
    aShipRightVector[63] = orientation;
    aShipUpVector[63] = orientation;
    point_at(63, aShipPositionsArray[0]);

    ScaleFixedVector(&aShipRightVector[63],
                     cameraDistance >> 2, &offset);
    AddFixedVectors(&aShipPositionsArray[63], &offset,
                    &aShipPositionsArray[63]);
    ScaleFixedVector(&aShipUpVector[63], 0x9600, &offset);
    AddFixedVectors(&aShipPositionsArray[63], &offset,
                    &aShipPositionsArray[63]);
    ScaleFixedVector(&aShipFwdVec[63],
                     cameraDistance, &offset);
    AddFixedVectors(&aShipPositionsArray[63], &offset,
                    &aShipPositionsArray[63]);

    aShipPositionsArray[EYE_OBJECT] =
        aShipPositionsArray[63];
    aShipRightVector[EYE_OBJECT] = orientation;
    aShipUpVector[EYE_OBJECT] = orientation;
    if (maximumRange < 0x271000)
        point_at(EYE_OBJECT, centre);
    else
        point_at(EYE_OBJECT, aShipPositionsArray[0]);
}

/* Function start: 0x410A30 */
unsigned int rotate_eye_to_goal(void)
{
    short totalError;

    totalError = (short)(abs(anObjPitchRot[61] -
                            nEyePitchTargetValue) +
                         abs(anObjectYawRotation[61] -
                            nEyeYawGoalValue) +
                         abs(anObjectRollRotationList[61] -
                            nEyeRollTargetValue));
    match_rotation_goal(&anObjPitchRot[61],
                        &nEyePitchTargetValue, totalError,
                        nEyePitchRate);
    match_rotation_goal(&anObjectYawRotation[61],
                        &nEyeYawGoalValue, totalError,
                        nEyeYawRate);
    match_rotation_goal(&anObjectRollRotationList[61],
                        &nEyeRollTargetValue, totalError,
                        nEyeRollRate);
    return 0;
}

/* Function start: 0x410AD0 */
short GetVectorMagnitude(const FixedVector *vector)
{
    return FixedToShortSaturating(Vector_magnitude(vector));
}

/* Function start: 0x410AF0 */
unsigned int set_eye_direction_and_position(void)
{
    FixedVector vector;
    FixedVector adjustment;
    FixedVector desiredPosition;
    FixedVector positionDelta;
    FixedVector viewDirection;
    int magnitude;

    if (bScriptedView == 1)
        update_scripted_view();

    switch (nCameraViewMode) {
    case 0:
        copy_frame(0, EYE_OBJECT);
        aShipVelocity[EYE_OBJECT] =
            aShipVelocity[0];
        aShipPositionsArray[EYE_OBJECT] =
            aShipPositionsArray[0];
        return 0;
    case 1:
        aShipRightVector[EYE_OBJECT] =
            aShipFwdVec[0];
        negate_vector(&aShipRightVector[EYE_OBJECT]);
        aShipUpVector[EYE_OBJECT] =
            aShipUpVector[0];
        aShipFwdVec[EYE_OBJECT] =
            aShipRightVector[0];
        aShipVelocity[EYE_OBJECT] =
            aShipVelocity[0];
        aShipPositionsArray[EYE_OBJECT] =
            aShipPositionsArray[0];
        return 0;
    case 2:
        aShipRightVector[EYE_OBJECT] =
            aShipFwdVec[0];
        aShipUpVector[EYE_OBJECT] =
            aShipUpVector[0];
        aShipFwdVec[EYE_OBJECT] =
            aShipRightVector[0];
        negate_vector(&aShipFwdVec[EYE_OBJECT]);
        aShipVelocity[EYE_OBJECT] =
            aShipVelocity[0];
        aShipPositionsArray[EYE_OBJECT] =
            aShipPositionsArray[0];
        return 0;
    case 3:
        copy_frame(0, EYE_OBJECT);
        negate_vector(&aShipRightVector[EYE_OBJECT]);
        negate_vector(&aShipFwdVec[EYE_OBJECT]);
        aShipVelocity[EYE_OBJECT] =
            aShipVelocity[0];
        aShipPositionsArray[EYE_OBJECT] =
            aShipPositionsArray[0];
        return 0;
    case 4:
        if (aeObjectClass[cViewObject] !=
            OBJECT_CLASS_NULL) {
            ComputeVectorDelta(
                &aShipPositionsArray[EYE_OBJECT],
                &aShipPositionsArray[cViewObject],
                &viewDirection);
            desiredPosition = aShipFwdVec[0];
            SetVectorFixedPoint(
                (unsigned int *)&desiredPosition,
                (short)(bAlternateChaseView == 0 ? -700 : -500));
            AddFixedVectors(
                &desiredPosition,
                &aShipPositionsArray[cViewObject],
                &desiredPosition);
            ComputeVectorDelta(
                &aShipPositionsArray[EYE_OBJECT],
                &desiredPosition, &positionDelta);
            vector = positionDelta;
            divide_vector(
                &vector,
                (bAlternateChaseView == 0 ? 25 : 7) << 8,
                &vector);
            aShipVelocity[EYE_OBJECT] = vector;
            aShipFwdVec[EYE_OBJECT] = viewDirection;
            shrink_vector(
                &aShipFwdVec[EYE_OBJECT]);
            fix_objects_ijk(EYE_OBJECT);
            nEyeRollTargetValue = match_roll_orientation(
                EYE_OBJECT, (short)cViewObject);
            if (nEyeRollTargetValue != 0) {
                if (abs((int)nEyeRollTargetValue) > 4) {
                    nEyeRollRate = 4;
                    rotate_eye_to_goal();
                    rotate_object(EYE_OBJECT);
                } else {
                    aShipUpVector[EYE_OBJECT] =
                        aShipUpVector[0];
                    nEyeRollTargetValue = 0;
                }
            }
        }
        AddFixedVectors(&aShipPositionsArray[EYE_OBJECT],
                        &aShipVelocity[EYE_OBJECT],
                        &aShipPositionsArray[EYE_OBJECT]);
        return 0;
    case 5:
        zero_vector(&aShipVelocity[EYE_OBJECT]);
        ComputeVectorDelta(
            &aShipPositionsArray[EYE_OBJECT],
            &aShipPositionsArray[cViewObject], &vector);
        magnitude = Vector_magnitude(&vector);
        if (magnitude > 0x7d000) {
            vector = aShipVelocity[cViewObject];
            NormalizeFixedVector(&vector);
            aShipUpVector[EYE_OBJECT] =
                aShipUpVector[cViewObject];
            if (vector.x == aShipUpVector[EYE_OBJECT].x &&
                vector.y == aShipUpVector[EYE_OBJECT].y &&
                vector.z == aShipUpVector[EYE_OBJECT].z) {
                aShipUpVector[EYE_OBJECT] =
                    aShipRightVector[cViewObject];
            }
            ScaleFixedVector(&vector, -0x12430, &vector);
            AddFixedVectors(
                &aShipPositionsArray[cViewObject], &vector,
                &aShipPositionsArray[EYE_OBJECT]);
            aShipFwdVec[EYE_OBJECT] = vector;
            fix_objects_ijk(EYE_OBJECT);
            ScaleFixedVector(&aShipRightVector[EYE_OBJECT],
                             0x19000, &vector);
            AddFixedVectors(&aShipPositionsArray[EYE_OBJECT], &vector,
                            &aShipPositionsArray[EYE_OBJECT]);
            ScaleFixedVector(&aShipUpVector[EYE_OBJECT],
                             0x6400, &vector);
            AddFixedVectors(&aShipPositionsArray[EYE_OBJECT], &vector,
                            &aShipPositionsArray[EYE_OBJECT]);
            ComputeVectorDelta(
                &aShipPositionsArray[EYE_OBJECT],
                &aShipPositionsArray[cViewObject], &vector);
            aShipFwdVec[EYE_OBJECT] = vector;
            fix_objects_ijk(EYE_OBJECT);
            generate_stars();
            return 0;
        }
        aShipFwdVec[EYE_OBJECT] = vector;
        fix_objects_ijk(EYE_OBJECT);
        return 0;
    case 6:
        if (nExternalViewShip == -1) {
            zero_vector(&aShipVelocity[EYE_OBJECT]);
            if (nExternalViewAngle++ > 20) {
                if (bCockpitlessView == 0) {
                    new_view(0, 0);
                } else {
                    GetScreenUpdateFlag();
                    SetViewportRect(
                        &stSpaceBufferViewport, 0, 0,
                        (unsigned short)(nScreenWidth - 1),
                        (unsigned short)(nScreenHeight - 1));
                    initialize_view_buffer();
                    new_view(0, 0);
                    GetScreenUpdateFlag();
                    SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
                    initialize_view_buffer();
                }
            }
        } else {
            ComputeVectorDelta(
                &aShipPositionsArray[EYE_OBJECT],
                &aShipPositionsArray[nExternalViewShip],
                &vector);
            if (aeShipTacticArray[nExternalViewShip] !=
                TACTIC_CRUISE) {
                magnitude = Vector_magnitude(&vector);
                if (magnitude > 0xfa00) {
                    divide_vector(
                        &vector,
                        (int)(short)(nCurrentExternalViewDistance &
                                     0xfffe) << 7,
                        &aShipVelocity[EYE_OBJECT]);
                    nCurrentExternalViewDistance = MaxShort(
                        (short)(nCurrentExternalViewDistance - 1), 8);
                } else {
                    adjustment = vector;
                    NormalizeFixedVector(&adjustment);
                    ScaleFixedVector(&adjustment, -64000, &adjustment);
                    AddFixedVectors(
                        &vector, &adjustment,
                        &aShipVelocity[EYE_OBJECT]);
                }
            }
            aShipFwdVec[EYE_OBJECT] = vector;
            fix_objects_ijk(EYE_OBJECT);
        }
        AddFixedVectors(&aShipPositionsArray[EYE_OBJECT],
                        &aShipVelocity[EYE_OBJECT],
                        &aShipPositionsArray[EYE_OBJECT]);
        return 0;
    case 7:
        if (acShipTargetTable[0] != -1) {
            ScaleFixedVector(&aShipRightVector[0],
                             0x12c00, &vector);
            AddFixedVectors(&aShipPositionsArray[0], &vector,
                            &aShipPositionsArray[EYE_OBJECT]);
            ComputeVectorDelta(
                &aShipPositionsArray[EYE_OBJECT],
                &aShipPositionsArray[
                    acShipTargetTable[0]],
                &vector);
            NormalizeFixedVector(&vector);
            aShipFwdVec[EYE_OBJECT] = vector;
            ScaleFixedVector(&vector, -0x25800, &vector);
            AddFixedVectors(&aShipPositionsArray[EYE_OBJECT],
                            &vector,
                            &aShipPositionsArray[EYE_OBJECT]);
            fix_objects_ijk(EYE_OBJECT);
            return 0;
        }
        if (bCockpitlessView == 0) {
            new_view(0, 0);
            return 0;
        }
        GetScreenUpdateFlag();
        SetViewportRect(
            &stSpaceBufferViewport, 0, 0,
            (unsigned short)(nScreenWidth - 1),
            (unsigned short)(nScreenHeight - 1));
        initialize_view_buffer();
        bCockpitlessView = 0;
        new_view(0, 0);
        bCockpitlessView = 1;
        GetScreenUpdateFlag();
        SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
        initialize_view_buffer();
        return 0;
    case 8:
        ScaleFixedVector(&aShipFwdVec[EYE_OBJECT],
                         nCapitalShipViewDistance, &vector);
        SubtractFixedVectors(
            &aShipPositionsArray[cViewObject], &vector,
            &aShipPositionsArray[EYE_OBJECT]);
        if (Vector_magnitude(
                &aShipVelocity[EYE_OBJECT]) != 0) {
            AddFixedVectors(&aShipPositionsArray[EYE_OBJECT],
                            &aShipVelocity[EYE_OBJECT],
                            &aShipPositionsArray[EYE_OBJECT]);
            return 0;
        }
        break;
    case 9:
        copy_frame(0, EYE_OBJECT);
        negate_vector(&aShipRightVector[EYE_OBJECT]);
        negate_vector(&aShipFwdVec[EYE_OBJECT]);
        aShipVelocity[EYE_OBJECT] =
            aShipVelocity[0];
        aShipPositionsArray[EYE_OBJECT] =
            aShipPositionsArray[0];
        return 0;
    case 10:
        copy_frame((short)cViewObject, EYE_OBJECT);
        aShipVelocity[EYE_OBJECT] =
            aShipVelocity[cViewObject];
        aShipPositionsArray[EYE_OBJECT] =
            aShipPositionsArray[cViewObject];
        return 0;
    case 11:
        ComputeVectorDelta(
            &aShipPositionsArray[EYE_OBJECT],
            &aShipPositionsArray[cViewObject], &vector);
        magnitude = Vector_magnitude(&vector);
        if (magnitude < 0x25800) {
            adjustment = vector;
            NormalizeFixedVector(&adjustment);
            ScaleFixedVector(&adjustment, -0x25800, &adjustment);
            AddFixedVectors(&vector, &adjustment, &adjustment);
            AddFixedVectors(&aShipPositionsArray[EYE_OBJECT],
                            &adjustment,
                            &aShipPositionsArray[EYE_OBJECT]);
        }
        aShipFwdVec[EYE_OBJECT] = vector;
        fix_objects_ijk(EYE_OBJECT);
        AddFixedVectors(&aShipPositionsArray[EYE_OBJECT],
                        &aShipVelocity[EYE_OBJECT],
                        &aShipPositionsArray[EYE_OBJECT]);
        return 0;
    case 12:
        ScaleFixedVector(&aShipRightVector[EYE_OBJECT],
                         -0xa00, &vector);
        AddFixedVectors(&aShipPositionsArray[EYE_OBJECT], &vector,
                        &aShipPositionsArray[EYE_OBJECT]);
        AddFixedVectors(&aShipPositionsArray[EYE_OBJECT],
                        &aShipVelocity[EYE_OBJECT],
                        &aShipPositionsArray[EYE_OBJECT]);
        look_at(0);
        return 0;
    case 13:
        AddFixedVectors(&aShipPositionsArray[EYE_OBJECT],
                        &aShipVelocity[EYE_OBJECT],
                        &aShipPositionsArray[EYE_OBJECT]);
        look_at(0);
        return 0;
    case 14:
        SetFleetOverviewView(0);
        return 0;
    case 15:
        AddFixedVectors(&aShipPositionsArray[EYE_OBJECT],
                        &aShipVelocity[EYE_OBJECT],
                        &aShipPositionsArray[EYE_OBJECT]);
        rotate_object(EYE_OBJECT);
        rotate_eye_to_goal();
        break;
    }
    return 0;
}

/* Function start: 0x4117B0 */
unsigned int force_view(int view, short obj)
{
    nCameraViewMode = -1;
    new_view(view, obj);
    return 0;
}

/* Function start: 0x4117D0 */
unsigned int new_view(int view, short obj)
{
    short carrier;

    if (nCameraViewMode == view) {
        if (view == 4)
            bAlternateChaseView =
                1 - bAlternateChaseView;
        return 0;
    }
    if (view == 6 && nExternalViewShip == -1)
        return 0;

    cViewObject = (signed char)obj;
    if (view == 4 &&
        aeObjectClass[cViewObject] ==
            OBJECT_CLASS_CAPITAL_SHIP)
        view = 8;
    nCameraViewMode = view;
    if (obj != -1)
        asObjectCollisionRadius[EYE_OBJECT] =
            MaxShort(10, asObjectCollisionRadius[obj]);
    else
        asObjectCollisionRadius[EYE_OBJECT] = 10;
    switch (view) {
        FixedVector vector;

    case 0:
        initialize_cockpit(0);
        break;
    case 1:
        initialize_cockpit(1);
        break;
    case 2:
        initialize_cockpit(2);
        break;
    case 3:
        initialize_cockpit(3);
        break;
    case 4:
        initialize_cockpit(4);
        if (bScriptedView == 0) {
            ScaleFixedVector(
                             &aShipFwdVec[
                                 cViewObject],
#ifdef SDL_PORT
                             -1200 * 0x100, &vector);
#else
                             -1200 << 8, &vector);
#endif
            AddFixedVectors(
                            &aShipPositionsArray[
                                cViewObject],
                            &vector,
                            &aShipPositionsArray[EYE_OBJECT]);
            aShipUpVector[EYE_OBJECT] =
                aShipUpVector[cViewObject];
            aShipFwdVec[EYE_OBJECT] = vector;
            fix_objects_ijk(EYE_OBJECT);
            zero_vector(&aShipVelocity[EYE_OBJECT]);
        }
        break;
    case 5:
        initialize_cockpit(4);
        vector = aShipVelocity[cViewObject];
        if (NormalizeFixedVector(&vector) == 0)
            vector =
                aShipFwdVec[cViewObject];
        aShipUpVector[EYE_OBJECT] =
            aShipUpVector[cViewObject];
        if (vector.x ==
                aShipUpVector[EYE_OBJECT].x &&
            vector.y ==
                aShipUpVector[EYE_OBJECT].y &&
            vector.z ==
                aShipUpVector[EYE_OBJECT].z) {
            aShipUpVector[EYE_OBJECT] =
                aShipRightVector[cViewObject];
        }
        ScaleFixedVector(&vector, -0x12430, &vector);
        AddFixedVectors(
            &aShipPositionsArray[cViewObject],
            &vector, &aShipPositionsArray[EYE_OBJECT]);
        aShipFwdVec[EYE_OBJECT] = vector;
        fix_objects_ijk(EYE_OBJECT);
        zero_vector(&aShipVelocity[EYE_OBJECT]);
        ScaleFixedVector(&aShipRightVector[EYE_OBJECT],
                         0x19000, &vector);
        AddFixedVectors(&aShipPositionsArray[EYE_OBJECT],
                        &vector,
                        &aShipPositionsArray[EYE_OBJECT]);
        ScaleFixedVector(&aShipUpVector[EYE_OBJECT],
                         0x6400, &vector);
        AddFixedVectors(&aShipPositionsArray[EYE_OBJECT],
                        &vector,
                        &aShipPositionsArray[EYE_OBJECT]);
        ComputeVectorDelta(
            &aShipPositionsArray[EYE_OBJECT],
            &aShipPositionsArray[cViewObject],
            &vector);
        aShipFwdVec[EYE_OBJECT] = vector;
        fix_objects_ijk(EYE_OBJECT);
        break;
    case 6:
        initialize_cockpit(4);
        ScaleFixedVector(
            &aShipRightVector[nExternalViewShip],
            0x25800, &vector);
        AddFixedVectors(
            &aShipPositionsArray[nExternalViewShip],
            &vector, &aShipPositionsArray[EYE_OBJECT]);
        ComputeVectorDelta(
            &aShipPositionsArray[EYE_OBJECT],
            &aShipPositionsArray[nExternalViewShip],
            &vector);
        aShipUpVector[EYE_OBJECT] =
            aShipUpVector[0];
        aShipFwdVec[EYE_OBJECT] = vector;
        fix_objects_ijk(EYE_OBJECT);
        zero_vector(&aShipVelocity[EYE_OBJECT]);
        nCurrentExternalViewDistance = 0x20;
        nExternalViewAngle = 0;
        break;
    case 7:
        initialize_cockpit(4);
        copy_frame(0, EYE_OBJECT);
        zero_vector(&aShipVelocity[EYE_OBJECT]);
        break;
    case 8:
        initialize_cockpit(4);
        aShipRightVector[EYE_OBJECT] =
            aShipRightVector[cViewObject];
        aShipUpVector[EYE_OBJECT] =
            aShipFwdVec[cViewObject];
        negate_vector(&aShipUpVector[EYE_OBJECT]);
        aShipFwdVec[EYE_OBJECT] =
            aShipUpVector[cViewObject];
        fix_objects_ijk(EYE_OBJECT);
        zero_vector(&aShipVelocity[EYE_OBJECT]);
        break;
    case 9:
        initialize_cockpit(6);
        break;
    case 10:
        initialize_cockpit(7);
        break;
    case 11:
    case 15:
        initialize_cockpit(4);
        nEyeRollTargetValue = 0;
        nEyeYawGoalValue = 0;
        nEyePitchTargetValue = 0;
        nEyeRollRate = 1;
        nEyeYawRate = 1;
        nEyePitchRate = 1;
        break;
    case 12:
        initialize_cockpit(4);
        copy_frame(0, EYE_OBJECT);
        position_relative_ijk(&aShipPositionsArray[EYE_OBJECT],
                              0, 500, 0, 2000);
        zero_vector(&aShipVelocity[EYE_OBJECT]);
        look_at(0);
        break;
    case 13:
        initialize_cockpit(4);
        zero_vector(&aShipVelocity[0]);
        if (nYourWingman != -1)
            zero_vector(
                &aShipVelocity[nYourWingman]);
        copy_frame(cViewObject, EYE_OBJECT);
        carrier = find_ship_index(nCarrierMissionShipIndexValue);
        look_at(carrier);
        position_relative_ijk(
            &aShipPositionsArray[EYE_OBJECT],
            EYE_OBJECT, 0, -10, -400);
        aShipVelocity[EYE_OBJECT] =
            aShipFwdVec[EYE_OBJECT];
        ScaleFixedVector(&aShipVelocity[EYE_OBJECT],
                         -0x2300,
                         &aShipVelocity[EYE_OBJECT]);
        break;
    case 14:
        SetFleetOverviewView(1);
        break;
    }

    if (bScriptedView == 1) {
        bScriptedView = 0;
        set_eye_direction_and_position();
        bScriptedView = 1;
    } else {
        set_eye_direction_and_position();
    }
    generate_stars();
    return 0;
}

/* Function start: 0x411EC0 */
unsigned int start_dust(short obj, FixedVector origin,
                        short forwardDistance, int rightOffset,
                        int upOffset)
{
    FixedVector offset;
    short streak;

    set_objects_data(obj, OBJECT_TYPE_SPACE_DUST, -1);
    ScaleFixedVector(&aShipFwdVec[EYE_OBJECT],
                     (int)forwardDistance << 8, &offset);
    AddFixedVectors(&origin, &offset, &origin);
    ScaleFixedVector(&aShipRightVector[EYE_OBJECT],
                     rightOffset, &offset);
    AddFixedVectors(&offset, &origin, &origin);
    ScaleFixedVector(&aShipUpVector[EYE_OBJECT],
                     upOffset, &offset);
    AddFixedVectors(&offset, &origin, &origin);
    AddFixedVectors(&origin,
                    &aShipPositionsArray[EYE_OBJECT],
                    &aShipPositionsArray[obj]);
    FillFixedVectorWithRandomComponents(2, &aShipVelocity[obj]);
    streak = MaxShort((short)(1 - RandomInRange(0, 3)), 0);
    streak <<= 4;
    streak += RandomInRange(0, 3);
    asObjScreenAngles[obj] = streak;
    return 0;
}

/* Function start: 0x411FE0 */
unsigned int generate_stars(void)
{
    FixedVector origin;
    short distance;
    short obj;

    origin.x = 0;
    origin.y = 0;
    origin.z = 0;
    obj = 34;
    do {
        if (obj < 42) {
            distance = RandomInRange(0, 1400);
#ifdef SDL_PORT
            start_dust(obj, origin, distance,
                       signed_random(distance) * 0x100,
                       signed_random(distance) * 0x100);
#else
            start_dust(obj, origin, distance,
                       signed_random(distance) << 8,
                       signed_random(distance) << 8);
#endif
        } else {
            aeObjectClass[obj] = OBJECT_CLASS_STAR;
            nStarFieldIRotValue = signed_random(45);
            nCurrentStarFieldJRotationValue = signed_random(45);
            copy_frame(EYE_OBJECT, 63);
            rotate_about_j(nCurrentStarFieldJRotationValue,
                           &aShipRightVector[63],
                           &aShipFwdVec[63]);
            rotate_about_i(nStarFieldIRotValue,
                           &aShipUpVector[63],
                           &aShipFwdVec[63]);
            ScaleFixedVector(&aShipFwdVec[63],
                             15000 << 8,
                             &aShipPositionsArray[obj]);
            asObjectViewFrames[obj] =
                (short)(RandomInRange(0, 5) + 32);
        }
        obj++;
    } while (obj < 49);
    return 0;
}

/* Function start: 0x412100 */
unsigned int update_star_field(void)
{
    FixedVector cameraMotion;
    FixedVector viewMotion;
    FixedVector origin;
    int distance;
    short randomChoice;
    short rightRandom;
    short upRandom;
    unsigned char shift;
    int rightOffset;
    int upOffset;
    int objectIndex;
    int objectOffset;
    short obj;
    unsigned int hazardActive;

    hazardActive = pActiveHazardField != 0;
    vPreviousStarFieldMotion = vStarFieldMotion;
    ScaleFixedVector(&aShipFwdVec[EYE_OBJECT],
                     200 << 8, &cameraMotion);
    ScaleFixedVector(&aShipVelocity[EYE_OBJECT],
                     20 << 8, &vStarFieldMotion);
    AddFixedVectors(&cameraMotion, &vStarFieldMotion,
                    &vStarFieldMotion);
    ComputeVectorDelta(&vPreviousStarFieldMotion,
                       &vStarFieldMotion, &origin);
    for (obj = 34; obj < 49; obj++) {
        objectIndex = (int)obj;
        objectOffset = objectIndex * sizeof(short);
        if (*(short *)((unsigned char *)asObjectScreenXList +
                       objectOffset) == (short)0x8001) {
            randomChoice = RandomInRange(0, 7);
            if (hazardActive == 0) {
                if (aeObjectClass[objectIndex] ==
                        OBJECT_CLASS_ASTEROID ||
                    (int)aeObjectClass[objectIndex] == 0x21 ||
                    aeObjectClass[objectIndex] ==
                        OBJECT_CLASS_NULL) {
                    set_objects_data(obj, OBJECT_TYPE_SPACE_DUST, -1);
                    randomChoice = 0;
                }
            } else if (obj < 42) {
                extra_hazard(obj);
            }
            if (aeObjectClass[objectIndex] == OBJECT_CLASS_STAR &&
                randomChoice == 0) {
                if ((anObjectYawRotation[0] |
                     anObjPitchRot[0]) == 0)
                    continue;
                copy_frame(EYE_OBJECT, 63);
                if (anObjPitchRot[0] != 0) {
                    nStarFieldIRotValue =
                        anObjPitchRot[0] < 0 ?
                            -45 : 45;
                    nCurrentStarFieldJRotationValue = signed_random(45);
                }
                if (anObjectYawRotation[0] != 0 &&
                    (anObjPitchRot[0] == 0 ||
                     RandomInRange(0, 1) != 0)) {
                    nCurrentStarFieldJRotationValue =
                        anObjectYawRotation[0] < 0 ? -45 : 45;
                    nStarFieldIRotValue = signed_random(45);
                }
                rotate_about_i(nStarFieldIRotValue,
                               &aShipUpVector[63],
                               &aShipFwdVec[63]);
                rotate_about_j(nCurrentStarFieldJRotationValue,
                               &aShipRightVector[63],
                               &aShipFwdVec[63]);
                ScaleFixedVector(&aShipFwdVec[63],
                                 15000 << 8,
                                 &aShipPositionsArray[objectIndex]);
                *(short *)((unsigned char *)asObjectViewFrames +
                           objectOffset) =
                    (short)(RandomInRange(0, 5) + 32);
                break;
            }
            if (aeObjectClass[objectIndex] == OBJECT_CLASS_DUST &&
                randomChoice < 2) {
                transform_to_objects_frame(
                    &aShipVelocity[EYE_OBJECT],
                    &viewMotion, EYE_OBJECT);
                ScaleFixedVector(&viewMotion, 10 << 8, &viewMotion);
                if (viewMotion.z >= 0) {
                    distance =
                        (unsigned short)RandomInRange(
                            0, (short)(viewMotion.z >> 8));
                    distance += asObjectCollisionRadius[EYE_OBJECT];
                    distance *= 2;
                    distance += (unsigned short)RandomInRange(0, 350);
                } else {
                    distance = (unsigned short)RandomInRange(0, 40) +
                        asObjectCollisionRadius[EYE_OBJECT];
                }
                rightRandom = signed_random((short)(distance >> 1));
                upRandom = signed_random((short)(distance >> 1));
                shift = (unsigned char)(viewMotion.z <= 0 ? 9 : 8);
#ifdef SDL_PORT
                rightOffset =
                    viewMotion.x + (int)rightRandom * (1 << shift);
                upOffset =
                    viewMotion.y + (int)upRandom * (1 << shift);
#else
                rightOffset =
                    viewMotion.x + ((int)rightRandom << shift);
                upOffset = viewMotion.y + ((int)upRandom << shift);
#endif
                start_dust(obj, origin, distance, rightOffset, upOffset);
                break;
            }
        }
    }
    if (pActiveHazardField != 0)
        update_hazards();
    return 0;
}

/* Function start: 0x412410 */
short count_down(short obj)
{
    if (asObjectCounterArray[obj] != -1)
        asObjectCounterArray[obj]--;
    return asObjectCounterArray[obj];
}

/* Function start: 0x412430 */
unsigned int house_keep_objects(void)
{
    short obj;

    for (obj = 0; obj <= SPACE_LAST_MOVING_OBJECT; obj++) {
        switch (aeObjectClass[obj]) {
        case OBJECT_CLASS_DUST:
            if (aeObjectTypeList[obj] == OBJECT_TYPE_DEBRIS_DUST &&
                count_down(obj) == -1 &&
                asObjectScreenXList[obj] == (short)0x8001)
                remove_object(obj);
            break;
        case OBJECT_CLASS_DEBRIS:
            if (count_down(obj) == -1)
                remove_object(obj);
            break;
        case OBJECT_CLASS_FIXED_OBJECT:
            if (aeObjectTypeList[obj] == OBJECT_TYPE_TURRET ||
                aeObjectTypeList[obj] == OBJECT_TYPE_THRUSTERS)
                remove_object(obj);
            break;
        case OBJECT_CLASS_PROJECTILE:
            if (count_down(obj) == 0) {
                if (aeObjectTypeList[obj] == OBJECT_TYPE_TURRET)
                    explode((short)acObjOwnersTable[obj], obj);
                else
                    remove_object(obj);
            }
            break;
        case OBJECT_CLASS_MINE:
            if (acObjCollisionGraceTicks[obj] > 0)
                acObjCollisionGraceTicks[obj]--;
            if (count_down(obj) == 0)
                explode(obj, obj);
            break;
        case OBJECT_CLASS_MISSILE:
            abShipExhaustHeatList[obj] = 0;
            if (acObjCollisionGraceTicks[obj] > 0)
                acObjCollisionGraceTicks[obj]--;
            if (aeShipTacticArray[obj] == TACTIC_SIT_STILL) {
                if (count_down(obj) <= 0) {
                    aeShipTacticArray[obj] = TACTIC_RAM;
                    asObjectCounterArray[obj] =
                        aObjectTypeData[
                            aeObjectTypeList[obj]].lifetime;
                    if (aeObjectTypeList[obj] ==
                            OBJECT_TYPE_DUMB_FIRE_MISSILE) {
                        vector_component_in_dir(
                            &aShipVelocity[obj],
                            &aShipFwdVec[obj],
                            &aShipVelocity[obj]);
                    }
                }
            } else if (count_down(obj) <= 0) {
                explode(obj, obj);
            }
            break;
        case OBJECT_CLASS_SHIP:
        case OBJECT_CLASS_CAPITAL_SHIP:
            abShipExhaustHeatList[obj] = 0;
            if (count_down(obj) > 0) {
                if (aeShipManeuversTable[obj] ==
                        MANEUVER_WARPING_OUT)
                    awObjectScales[obj] >>= 1;
                if (aeSpecialManeuvers[obj] ==
                        SPECIAL_MANEUVER_UNKNOWN_9 &&
                    aeObjectClass[obj] ==
                        OBJECT_CLASS_CAPITAL_SHIP) {
                    if (asObjectCounterArray[obj] == 7) {
                        ShipExplosion(obj);
                        explosion_shock_wave(
                            obj, aObjectTypeData[
                                aeObjectTypeList[obj]].
                                    explosionDamage);
                    } else {
                        while ((unsigned short)RandomInRange(0, 100) < 50)
                            onboard_explosion(obj);
                    }
                }
            } else if (asObjectCounterArray[obj] == 0) {
                if (aeSpecialManeuvers[obj] ==
                        SPECIAL_MANEUVER_UNKNOWN_9) {
                    if (nYourWingman != -1 &&
                        acPreviousAttackers[obj] == 0 &&
                        aeShipSide[obj] == SIDE_KILRATHI &&
                        RandomBelowOrEqual(100) < 10) {
                        send_message(nYourWingman, 6);
                    }
                    Create_explosion_debris(obj);
                    break;
                }
                switch (aeShipManeuversTable[obj]) {
                case MANEUVER_WARPING_IN:
                    if (aeShipTacticArray[obj] != TACTIC_WARP_IN) {
                        if ((short)acObjOwnersTable[obj] == obj) {
                            set_objects_data(
                                obj,
                                (enum ObjectType)
                                    abShipNavPtIdx[obj],
                                -1);
                            reset_maneuver(obj, -1);
                        } else {
                            remove_object(obj);
                        }
                    }
                    break;
                case MANEUVER_WARPING_OUT:
                    if (aeShipSide[obj] != SIDE_NEUTRAL) {
                        aMissionShips[
                            nShipMissionIndices[obj]].state = 2;
                        remove_object(obj);
                    }
                    break;
                }
            }
            if (aeObjectTypeList[obj] ==
                    OBJECT_TYPE_TIGERS_CLAW &&
                bPlayerCollisionsEnabled != 0 &&
                bLandingAuthorized != 0 &&
                normal_speed(0) != 0) {
                get_facing_range_from_object(0, obj);
#ifdef SDL_PORT
                /* SWC CODE_02 +0x414a checks range alone. Keep the existing
                   SDL WC1 approach check for non-SWC flights. */
                if (nTgtRange < 700 &&
                    (SdlSwcFlightActive() || nFacingToTargetValue > 75)) {
#else
                if (nTgtRange < 700 &&
                    nFacingToTargetValue > 75 &&
                    nTargetFacingValue > 70) {
#endif
                    nArcadeState = 1;
                    nPlayerCollisionObject = obj;
                }
            }
            break;
        }
    }
    return 0;
}

/* Function start: 0x412820 */
unsigned int update_objects_in_space(void)
{
    short obj;

    clear_crash_cache();
    obj = 0;
    do {
        if (aeObjectClass[obj] == OBJECT_CLASS_FUTURION) {
            futurion_intelligence(obj);
        } else if (aeObjectClass[obj] > OBJECT_CLASS_PLANET) {
            animate_object(obj);
            if (aeObjectClass[obj] != OBJECT_CLASS_NULL &&
                aeObjectClass[obj] >= OBJECT_CLASS_PROJECTILE) {
                object_collision(obj);
                rotate_object(obj);
                if (obj >= 10 ||
                    aeSpecialManeuvers[obj] !=
                        SPECIAL_MANEUVER_UNKNOWN_9) {
                    if (obj != 0)
                        object_intelligence(obj);
                    if (obj < 10 &&
                        aeObjectClass[obj] >=
                            OBJECT_CLASS_MISSILE) {
                        if (obj != 0)
                            rotate_object_to_goal(obj);
                        if (aeObjectClass[obj] ==
                                OBJECT_CLASS_SHIP)
                            replenish_weapon_energy_bank(obj);
                    }
                }
            }
        }
        obj++;
    } while (obj <= SPACE_LAST_MOVING_OBJECT);

    obj = 0;
    do {
        if (aeObjectClass[obj] > OBJECT_CLASS_PLANET) {
            accelerate_and_move_object(obj);
            if (aeObjectClass[obj] >= OBJECT_CLASS_SHIP) {
                replenish_shields(obj);
                housekeep_power_plant_and_fuel(obj);
            }
        }
        obj++;
    } while (obj <= SPACE_LAST_MOVING_OBJECT);
    return 0;
}

/* Function start: 0x412920 */
unsigned int rotate_object(short obj)
{
    if (anObjPitchRot[obj] != 0) {
        alter_pitch(anObjPitchRot[obj], obj);
        ClampVectorTo30(&anObjPitchRot[obj]);
    }
    if (anObjectYawRotation[obj] != 0) {
        alter_yaw(anObjectYawRotation[obj], obj);
        ClampVectorTo30(&anObjectYawRotation[obj]);
    }
    if (anObjectRollRotationList[obj] != 0) {
        alter_roll(anObjectRollRotationList[obj], obj);
        ClampVectorTo30(&anObjectRollRotationList[obj]);
    }
    return 0;
}

/* Function start: 0x4129A0 */
unsigned int accelerate_and_move_object(short obj)
{
    FixedVector delta;
    FixedVector accelerationVector;
    int acceleration;
    int magnitude;
    int afterburnerSpeed;

    if (aeObjectClass[obj] >= OBJECT_CLASS_MISSILE) {
        switch (aeSpecialManeuvers[obj]) {
        case SPECIAL_MANEUVER_KILL_ENGINES:
            abShipExhaustHeatList[obj] = 0;
            if (RandomBelowOrEqual(100) < 10)
                set_special(obj, SPECIAL_MANEUVER_NONE);
            break;
        case SPECIAL_MANEUVER_STOP_DRIFT:
            approach_zero_speed(obj);
            NormalizeAndScaleVector(&aShipVelocity[obj],
                                    anShipSpeed[obj]);
            if (anShipSpeed[obj] == 0)
                set_special(obj, SPECIAL_MANEUVER_NONE);
            break;
        }
        if (aeSpecialManeuvers[obj] <
                SPECIAL_MANEUVER_KILL_ENGINES &&
            aeShipTacticArray[obj] != TACTIC_SIT_STILL) {
            zero_vector(&accelerationVector);
            switch (aeSpecialManeuvers[obj]) {
            case SPECIAL_MANEUVER_AFTERBURNER:
                asShipAfterburnerTimer[obj]--;
                if (asShipAfterburnerTimer[obj] == 0) {
                    aeSpecialManeuvers[obj] =
                        SPECIAL_MANEUVER_NONE;
                    if (bAfterburnerSfxEnabled != 0) {
                        FlushSoundEffectsAndLog(
                            bAfterburnerSfxEnabled, 0);
                        bAfterburnerSfxEnabled = 0;
                    }
                    ScaleFixedVector(&aShipFwdVec[obj],
                                     anShipSpeed[obj], &delta);
                } else {
                    afterburnerSpeed =
                        (aObjectTypeData[
                            aeObjectTypeList[obj]].maximumVelocity +
                         20) * 0x200;
                    ScaleFixedVector(
                        &aShipFwdVec[obj],
                        afterburnerSpeed,
                        &delta);
                    drain_fuel(obj, 200);
                    abShipExhaustHeatList[obj] = 3;
                }
                break;
            case SPECIAL_MANEUVER_SUPER_BRAKE:
                asShipAfterburnerTimer[obj]--;
                if (asShipAfterburnerTimer[obj] == 0) {
                    aeSpecialManeuvers[obj] =
                        SPECIAL_MANEUVER_NONE;
                    ScaleFixedVector(&aShipFwdVec[obj],
                                     anShipSpeed[obj], &delta);
                } else {
                    zero_vector(&delta);
                    drain_fuel(obj, 140);
                }
                break;
            default:
                ScaleFixedVector(&aShipFwdVec[obj],
                                 anShipSpeed[obj], &delta);
                break;
            }
            ComputeVectorDelta(&aShipVelocity[obj], &delta,
                               &delta);
            magnitude = Vector_magnitude(&delta);
            if (magnitude > 0) {
                acceleration = GetShipAccelerationRate(obj);
                if ((short)alert_flag(obj, 1) != 0 && acceleration < 0x500)
                    acceleration = 0x500;
                if (aeSpecialManeuvers[obj] ==
                        SPECIAL_MANEUVER_AFTERBURNER ||
                    aeSpecialManeuvers[obj] ==
                        SPECIAL_MANEUVER_SUPER_BRAKE) {
                    acceleration *= 2;
                }
                acceleration = MultiplyFixed(
                    acceleration,
                    DivideFixed(
                        dot_product(
                            &delta,
                            &aShipFwdVec[obj]),
                        magnitude) + 0x200);
                acceleration = MinInt(
                    DivideFixed(acceleration >> 1, magnitude), 0x100);
                ScaleFixedVector(&delta, acceleration, &accelerationVector);
                if (aeSpecialManeuvers[obj] !=
                        SPECIAL_MANEUVER_AFTERBURNER) {
                    abShipExhaustHeatList[obj] = 2;
                }
            }
            AddFixedVectors(&accelerationVector,
                            &aShipVelocity[obj],
                            &aShipVelocity[obj]);
            if (obj == 0)
                vPlayerAccel = accelerationVector;
        }
    }
    AddFixedVectors(&aShipPositionsArray[obj],
                    &aShipVelocity[obj],
                    &aShipPositionsArray[obj]);
    return 0;
}

/* Function start: 0x412CD0 */
unsigned int animate_shape(short obj)
{
    enum ObjectType type;
    unsigned char *animation;
    short command;

    type = aeObjectTypeList[obj];
    animation = aObjectTypeData[type].animation;
    if (animation == 0)
        return 0;
    if (--asObjectAnimationDelay[obj] > 0)
        return 0;
    asObjectAnimationDelay[obj] = aObjectTypeData[type].yawRate;
    command = *(unsigned short *)(animation + asObjectAnimIndexes[obj] * 4);
#ifdef SDL_PORT
    /* SWC CODE_02 animate_shape +0x4832: final visible type-49 frame
       starts the eight-color cockpit glare sequence. */
    if (SdlSwcFlightActive() && type == OBJECT_TYPE_EXPLOSION1 &&
        asObjectScreenXList[obj] != (short)0x8001 &&
        asObjectAnimIndexes[obj] == 19) {
        bSwcExplosionGlare = 1;
        nSwcSpaceGlareTicks = 8;
    }
#endif
    switch (command & 0xf000) {
    case 0x9000:
        asObjectAnimIndexes[obj] = command & 0x0fff;
        command = *(unsigned short *)(animation +
                  asObjectAnimIndexes[obj] * 4);
        if (asObjectScreenXList[obj] != (short)0x8001 &&
            (type == OBJECT_TYPE_DEBRIS_WING ||
             type == OBJECT_TYPE_DEBRIS_METAL_SHEET)) {
            PlaySfxWaveFileByNumber(13, obj, 0);
        }
        break;
    case 0xa000:
        remove_object(obj);
        return 0;
    }

    switch (command & 0x0c00) {
    case 0x0400:
        awObjectScales[obj] +=
            (command & 0x3f) * ((unsigned short)awObjectScales[obj] >> 6);
        break;
    case 0x0800:
        awObjectScales[obj] -=
            (command & 0x3f) * ((unsigned short)awObjectScales[obj] >> 6);
        break;
    default:
        command &= 0x3f;
        asObjectViewFrames[obj] = command;
        break;
    }
    asObjectFlipArray[obj] = (command & 0xc0) >> 2;
    asObjectAnimIndexes[obj]++;
    return 0;
}

/* Function start: 0x412E30 */
unsigned int animate_object(short obj)
{
    FixedVector offset;
    short effect;

    switch (aeObjectClass[obj]) {
    case OBJECT_CLASS_EXPLOSION:
        animate_shape(obj);
        break;
    case OBJECT_CLASS_DEBRIS:
        animate_shape(obj);
        break;
    case OBJECT_CLASS_FIXED_OBJECT:
        animate_shape(obj);
        break;
    case OBJECT_CLASS_ASTEROID:
        animate_shape(obj);
        break;
    case OBJECT_CLASS_MINE:
        animate_shape(obj);
        break;
    case OBJECT_CLASS_SHIP:
        if (asObjectScreenXList[obj] == (short)0x8001 ||
            (nRenderedSpaceFrame & 3) != 0 ||
            (aObjectTypeData[
                 aeObjectTypeList[obj]].damageCapacity >> 1) - 1 >
                asShipAccumulatedDmg[obj]) {
            break;
        }
        effect = find_vacant_3d_object();
        if (effect == -1)
            break;
        ScaleFixedVector(&aShipFwdVec[obj],
                         -((int)asObjectCollisionRadius[obj]
                             << 8),
                         &offset);
        AddFixedVectors(&aShipPositionsArray[obj], &offset,
                        &aShipPositionsArray[effect]);
        FillFixedVectorWithRandomComponents(20, &offset);
        AddFixedVectors(&aShipPositionsArray[effect], &offset,
                        &aShipPositionsArray[effect]);
        zero_vector(&aShipVelocity[effect]);
        set_objects_data(
            effect,
            (enum ObjectType)((unsigned short)RandomInRange(0, 2) +
                              OBJECT_TYPE_RED_SPARK),
            obj);
        awObjectScales[effect] =
            awObjectScales[obj];
        if (RandomInRange(0, 3) == 0)
            PlaySfxWaveFileByNumber(7, obj, 0, 0);
        break;
    }
    return 0;
}

/* Function start: 0x413030 */
unsigned int hit_asteroid(short asteroid, short destructionChance)
{
    short sourceAsteroid;

    if (RandomBelowOrEqual((short)(destructionChance - 1)) == 0) {
        /* Reuse the consumed parameters for the fragment count. */
        destructionChance = (short)(RandomBelowOrEqual(1) + 2);
        sourceAsteroid = asteroid;
        asteroid = destructionChance;
        while (asteroid > 0) {
            make_shard(sourceAsteroid, aShipVelocity[sourceAsteroid]);
            asteroid--;
        }
        explode(-1, sourceAsteroid);
    } else if (RandomBelowOrEqual(7) == 0) {
        sourceAsteroid = asteroid;
        make_shard(sourceAsteroid, vCollisionDelta);
    }
    return 0;
}

/* Function start: 0x4130D0 */
int object_collision(short obj)
{
    SphericalVector impact;
    FixedVector relativeVelocity;
    FixedVector response;
    FixedVector partnerComponent;
    FixedVector componentDelta;
    FixedVector tangent;
    int separationScale;
    int responseScale;
    int forceMagnitude;
    int totalMass;
    int objectIndex;
    int partnerIndex;
    short partner;
    short projectileDamage;
    short damage;
    short savedScale;

    partner = check_for_collision(obj);
    if (partner != -1) {
        if (bPlayerCollisionResponse == 0 && (partner == 0 || obj == 0))
            return 0;

        NormalizeFixedVector(&vCollisionDelta);
        partnerIndex = partner;
        objectIndex = obj;
        ComputeVectorDelta(&aShipVelocity[partnerIndex],
                           &aShipVelocity[objectIndex],
                           &relativeVelocity);
        NormalizeFixedVector(&relativeVelocity);
        switch (aeObjectClass[objectIndex]) {
        case OBJECT_CLASS_PROJECTILE:
            if (acObjOwnersTable[objectIndex] == partner)
                break;
            if (aeObjectClass[partnerIndex] >= OBJECT_CLASS_MISSILE) {
                if (partner == 0) {
                    rectangular_to_spherical(
                        &aObjectViewPositionsList[objectIndex], &impact);
                    if (abs(impact.pitch) < 45) {
                        if (abs(impact.yaw) < 45)
                            aPaletteFadeEntries[1][0] = 0x38;
                        else if (abs(impact.yaw) > 135)
                            aPaletteFadeEntries[0][0] = 0x38;
                        else {
                            if (impact.yaw < 0)
                                aPaletteFadeEntries[3][0] = 0x38;
                            else
                                aPaletteFadeEntries[5][0] = 0x38;
                        }
                    } else if (impact.pitch < 0)
                        aPaletteFadeEntries[2][0] = 0x38;
                    else
                        aPaletteFadeEntries[4][0] = 0x38;
                }
                acPreviousAttackers[partnerIndex] = acObjOwnersTable[objectIndex];
                acShipAiCooldowns[partnerIndex] += 4;
                projectileDamage = (short)(asShipAccumulatedDmg[objectIndex] -
                                           asObjectCounterArray[objectIndex] / 2);
                response = aShipVelocity[objectIndex];
                NormalizeFixedVector(&response);
                ScaleFixedVector(&response, (int)projectileDamage << 8,
                                 &response);
                negate_vector(&vCollisionDelta);
                apply_force_to_object(&vCollisionDelta,
                                      &response, partner);
                inflict_damage(obj, partner, projectileDamage,
                               &relativeVelocity);
            }
            savedScale = awObjectScales[objectIndex];
            set_objects_data(obj, OBJECT_TYPE_LASER_SPARK,
                             acObjOwnersTable[objectIndex]);
            awObjectScales[objectIndex] = (short)(savedScale * 2);
            aShipVelocity[objectIndex] =
                aShipVelocity[partnerIndex];
            if (aeObjectClass[partnerIndex] == OBJECT_CLASS_ASTEROID) {
                hit_asteroid(partner, 3);
                return 0;
            }
            break;

        case OBJECT_CLASS_ASTEROID:
            if (aeObjectClass[partnerIndex] ==
                    OBJECT_CLASS_ASTEROID) {
                if (asObjectScreenXList[objectIndex] == (short)0x8001) {
                    remove_object(obj);
                    return 0;
                }
                hit_asteroid(obj, 0);
                return 0;
            }
            break;

        case OBJECT_CLASS_MINE:
            if (acObjOwnersTable[objectIndex] == partner ||
                acObjCollisionGraceTicks[objectIndex] > 0)
                return 0;
            if (easy2see(obj) == 0 &&
                (nCameraViewMode == 0 || partner != 0)) {
                remove_object(obj);
                return 0;
            }
            explode(obj, obj);
            return 0;

        case OBJECT_CLASS_MISSILE:
            if (acObjOwnersTable[objectIndex] != partner ||
                acObjCollisionGraceTicks[objectIndex] <= 0) {
                ScaleFixedVector(&aShipVelocity[objectIndex],
                                 awObjMassArray[objectIndex] << 8,
                                 &response);
                negate_vector(&vCollisionDelta);
                apply_force_to_object(&vCollisionDelta,
                                      &response, partner);
                explode(obj, obj);
                zero_vector(&aShipVelocity[objectIndex]);
                return 0;
            }
            break;

        case OBJECT_CLASS_SHIP:
        case OBJECT_CLASS_CAPITAL_SHIP:
            if ((aeObjectClass[partnerIndex] == OBJECT_CLASS_ASTEROID ||
                 (aeObjectClass[partnerIndex] == OBJECT_CLASS_MINE &&
                  acObjOwnersTable[partnerIndex] == -1)) &&
                easy2see(partner) == 0 &&
                (nCameraViewMode == 0 || obj != 0)) {
                remove_object(partner);
            }
            if ((aeObjectClass[partnerIndex] == OBJECT_CLASS_ASTEROID ||
                 aeObjectClass[partnerIndex] == OBJECT_CLASS_SHIP ||
                 aeObjectClass[partnerIndex] == OBJECT_CLASS_CAPITAL_SHIP) &&
                acLastCollisionObjects[objectIndex] != partner) {
                PlaySfxWaveFileByNumber(0x1c, obj, 0, 0);
                acLastCollisionObjects[partnerIndex] =
                    (signed char)obj;
                acLastCollisionObjects[objectIndex] =
                    (signed char)partner;

                separationScale = DivideFixed(
                    (asObjectCollisionRadius[objectIndex] +
                     asObjectCollisionRadius[partnerIndex]) << 8,
                    Vector_magnitude(&vCollisionDelta));
                separationScale = MinInt(separationScale, 0x7d000);
                ScaleFixedVector(&vCollisionDelta,
                                 separationScale, &response);
                AddFixedVectors(&aShipPositionsArray[objectIndex], &response,
                                &aShipPositionsArray[partnerIndex]);

                vector_component_in_dir(
                    &aShipVelocity[objectIndex],
                    &vCollisionDelta, &response);
                SubtractFixedVectors(&aShipVelocity[objectIndex],
                                     &response, &tangent);
                vector_component_in_dir(
                    &aShipVelocity[partnerIndex],
                    &vCollisionDelta, &partnerComponent);
                ComputeVectorDelta(&partnerComponent, &response,
                                   &componentDelta);
                damage = (short)(Vector_magnitude(&componentDelta) >> 8);
#ifdef SDL_PORT
                if (obj == 0 || partner == 0)
                    SdlQueueJoystickCollisionRumble(damage);
#endif
                damage = (short)((damage * damage) >> 1);

                totalMass =
                    awObjMassArray[objectIndex] + awObjMassArray[partnerIndex];
                responseScale =
                    ((awObjMassArray[objectIndex] -
                      awObjMassArray[partnerIndex]) * 256) /
                    totalMass;
                responseScale = MinInt(MaxInt(0x40, responseScale), 0x400);
                ScaleFixedVector(&componentDelta, responseScale, &response);
                AddFixedVectors(&response, &partnerComponent, &response);
                forceMagnitude = MultiplyFixed(
                    awObjMassArray[objectIndex] * 0x600,
                    Vector_magnitude(&response)) + 0xa00;
                AddFixedVectors(&response, &aShipVelocity[objectIndex],
                                &aShipVelocity[objectIndex]);

                if (aeObjectClass[objectIndex] == OBJECT_CLASS_SHIP) {
                    NormalizeFixedVector(&tangent);
                    negate_vector(&tangent);
                    ScaleFixedVector(&tangent, forceMagnitude, &tangent);
                    rotational_acceleration(&vCollisionDelta,
                                            &tangent, obj);
                    negate_vector(&relativeVelocity);
                    inflict_damage(partner, obj, damage, &relativeVelocity);
                    negate_vector(&relativeVelocity);
                }

                SubtractFixedVectors(&aShipVelocity[partnerIndex],
                                     &partnerComponent, &tangent);
                responseScale = MinInt(MaxInt(0x40,
                    (awObjMassArray[objectIndex] << 9) / totalMass),
                    0x400);
                ScaleFixedVector(&componentDelta, responseScale, &response);
                AddFixedVectors(&response, &partnerComponent, &response);
                AddFixedVectors(&response,
                                &aShipVelocity[partnerIndex],
                                &aShipVelocity[partnerIndex]);
                if (aeObjectClass[partnerIndex] == OBJECT_CLASS_SHIP) {
                    NormalizeFixedVector(&tangent);
                    negate_vector(&tangent);
                    ScaleFixedVector(&tangent, forceMagnitude, &tangent);
                    negate_vector(&vCollisionDelta);
                    rotational_acceleration(&vCollisionDelta,
                                            &tangent, partner);
                    inflict_damage(obj, partner, damage, &relativeVelocity);
                }
                if (aeObjectClass[partnerIndex] == OBJECT_CLASS_CAPITAL_SHIP) {
                    SubtractFixedVectors(&aShipPositionsArray[objectIndex],
                                         &aShipVelocity[objectIndex],
                                         &aShipPositionsArray[objectIndex]);
                    anShipSpeed[objectIndex] = 0;
                    aShipVelocity[objectIndex] =
                        aShipVelocity[partnerIndex];
                }
            }
            break;
        }
    } else {
        acLastCollisionObjects[obj] = -1;
    }
    return 0;
}

/* Function start: 0x413880 */
unsigned int object_intelligence(short obj)
{
    short target;

    if (nCannedSceneMode == 4)
        return 0;
    if (nCannedSceneMode == 2 &&
        aeObjectClass[obj] >= OBJECT_CLASS_SHIP) {
        update_canned_sequence(obj);
        return 0;
    }
    switch (aeObjectClass[obj]) {
    case OBJECT_CLASS_FUTURION:
        futurion_intelligence(obj);
        break;
    case OBJECT_CLASS_MINE:
        mine_intelligence(obj);
        break;
    case OBJECT_CLASS_MISSILE:
        if ((short)(nCurrentSpaceFrameValue % 4) != 0 &&
            nExternalViewShip != obj) {
            break;
        }
        target = acShipTargetTable[obj];
        if (target != -1) {
            get_facing_range_from_object(obj, target);
        }
        switch (aeObjectTypeList[obj]) {
        case OBJECT_TYPE_DUMB_FIRE_MISSILE:
            anShipSpeed[obj] =
                (get_ship_max_velocity(obj) + 10) * 0x100;
            break;
        case OBJECT_TYPE_HEAT_SEEKING_MISSILE:
            heat_seeking_missile_intelligence(obj);
            break;
        case OBJECT_TYPE_FF_MISSILE:
            FF_missile_intelligence(obj);
            break;
        case OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE:
            point_ship(obj, 0, &vToTgt);
            anShipSpeed[obj] =
                (get_ship_max_velocity(obj) + 10) * 0x100;
            break;
        }
        break;
    case OBJECT_CLASS_SHIP:
        ship_intelligence(obj);
        break;
    case OBJECT_CLASS_CAPITAL_SHIP:
        capital_ship_intelligence(obj);
        break;
    }
    return 0;
}
