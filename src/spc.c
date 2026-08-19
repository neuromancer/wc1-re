/*
 *  Space-object simulation (Mac `spc` compilation unit), preceded by the
 *  Win32 joystick calibration/input helpers in this provisional range.
 *
 *  Address range 410000-412fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: the Mac `spc` symbol run maps rotate_eye_to_goal through
 *  object_intelligence to 410a30-4139ff.
 */
#include "wc1.h"

int nCapitalShipViewDistance = 0x7d000; /* 0x00468ff4 */
unsigned char bLandingAuthorized = 0; /* 0x00468ff8 */
int bAlternateChaseView = 0; /* 0x00468ffc */
extern short nStarFieldJRotation; /* 0x005a7f02 */
extern short nStarFieldIRotation; /* 0x005a7f04 */

/* Function start: 0x4102B0 */
void CalibrateJoystickInteractive()
{
    InputDeviceSample samples[2];
    short calibration[6];
    short shown;
    int file;
    int failed;
    int fontLoaded;

    fontLoaded = 0;
    if (nActiveInputDevice == -1)
        return;
    if (apTextFonts[1] != 0)
        fontLoaded = 1;

    stDefaultTextContext.alignment = 2;
    InitializeTextContextFromFont(&stDefaultTextContext, 1,
        cViewportClearColour, (signed char)cBlackColour);
    shown = ShowModalTextPanel(1,
        "Turn AUTO FIRE off if present, press a button");
    if (shown != 0) {
        DIBslamReal();
        WaitForJoystickButtonPress();
        WaitForJoystickButtonRelease();
        ReleaseModalTextPanel();
    }

    shown = ShowModalTextPanel(1,
        "Move stick to the UPPER LEFT, press a button");
    if (shown != 0) {
        DIBslamReal();
        WaitForJoystickButtonPress();
        SampleJoystickDevice(samples, nActiveInputDevice, 0x7fff);
        nJoystickMinimumX = samples[nActiveInputDevice].x;
        nJoystickMinimumY = samples[nActiveInputDevice].y;
        calibration[0] = (short)samples[nActiveInputDevice].x;
        calibration[1] = (short)samples[nActiveInputDevice].y;
        WaitForJoystickButtonRelease();
        ReleaseModalTextPanel();
    }

    shown = ShowModalTextPanel(1,
        "Move stick to the LOWER RIGHT, press a button");
    if (shown != 0) {
        DIBslamReal();
        WaitForJoystickButtonPress();
        SampleJoystickDevice(samples, nActiveInputDevice, 0x7fff);
        nJoystickMaximumX = samples[nActiveInputDevice].x;
        nJoystickMaximumY = samples[nActiveInputDevice].y;
        calibration[2] = (short)samples[nActiveInputDevice].x;
        calibration[3] = (short)samples[nActiveInputDevice].y;
        WaitForJoystickButtonRelease();
        ReleaseModalTextPanel();
    }

    shown = ShowModalTextPanel(1,
        "Center Joystick, press a button");
    if (shown != 0) {
        DIBslamReal();
        WaitForJoystickButtonPress();
        SampleJoystickDevice(samples, nActiveInputDevice, 0x7fff);
        nJoystickCentreX = samples[nActiveInputDevice].x;
        nJoystickCentreY = samples[nActiveInputDevice].y;
        calibration[4] = (short)samples[nActiveInputDevice].x;
        calibration[5] = (short)samples[nActiveInputDevice].y;
        WaitForJoystickButtonRelease();
        ReleaseModalTextPanel();
    }

    if (fontLoaded == 0)
        ReleaseTextFont(1);

    if (nJoystickHorizontalRange == 0)
        nJoystickHorizontalRange = 9;
    if (nJoystickVerticalRange == 0)
        nJoystickVerticalRange = 9;
    nJoystickLeftScale =
        (nJoystickCentreX - nJoystickMinimumX) /
        nJoystickHorizontalRange;
    nJoystickRightScale =
        (nJoystickMaximumX - nJoystickCentreX) /
        nJoystickHorizontalRange;
    nJoystickUpScale =
        (nJoystickCentreY - nJoystickMinimumY) /
        nJoystickVerticalRange;
    nJoystickDownScale =
        (nJoystickMaximumY - nJoystickCentreY) /
        nJoystickVerticalRange;
    if (nJoystickLeftScale == 0)
        nJoystickLeftScale = 1;
    if (nJoystickRightScale == 0)
        nJoystickRightScale = 1;
    if (nJoystickUpScale == 0)
        nJoystickUpScale = 1;
    if (nJoystickDownScale == 0)
        nJoystickDownScale = 1;

    nJoystickMinimumX = nJoystickCentreX -
        nJoystickLeftScale *
            nJoystickHorizontalRange;
    nJoystickMinimumY = nJoystickCentreY -
        nJoystickUpScale * nJoystickVerticalRange;
    nJoystickMaximumX = nJoystickCentreX +
        nJoystickRightScale *
            nJoystickHorizontalRange;
    nJoystickMaximumY = nJoystickCentreY +
        nJoystickDownScale * nJoystickVerticalRange;
    nJoystickFailureValue = nJoystickMaximumX * 2;

    if (nJoystickMaximumX <= nJoystickMinimumX ||
        nJoystickMaximumY <= nJoystickMinimumY) {
        shown = ShowModalTextPanel(1,
            "FAILED! Center Joystick, press a button");
        if (shown != 0) {
            DIBslamReal();
            WaitForJoystickButtonPress();
            WaitForJoystickButtonRelease();
            LoadJoystickCalibrationFile(8, 8, 1, 1);
            ReleaseModalTextPanel();
        }
        _unlink("j.cal");
        return;
    }

    file = _open("j.cal", 0x8301, 0x180);
    if (file == -1)
        return;
    failed = _write(file, &nActiveInputDevice, 2) == -1;
    if (failed == 0)
        failed = _write(file, &calibration[0], 2) == -1;
    if (failed == 0)
        failed = _write(file, &calibration[1], 2) == -1;
    if (failed == 0)
        failed = _write(file, &calibration[2], 2) == -1;
    if (failed == 0)
        failed = _write(file, &calibration[3], 2) == -1;
    if (failed == 0)
        failed = _write(file, &calibration[4], 2) == -1;
    if (failed == 0)
        failed = _write(file, &calibration[5], 2) == -1;
    _close(file);
    if (failed != 0)
        _unlink("j.cal");
}

/* Function start: 0x4106C0 */
void WaitForJoystickButtonRelease(void)
{
    do {
        SampleJoystickDevice(
            &aInputDeviceSamples[
                nActiveInputDevice],
            nActiveInputDevice, 0);
    } while (aInputDeviceSamples[
                 nActiveInputDevice].buttons != 0);
}

/* Function start: 0x410700 */
void WaitForJoystickButtonPress(void)
{
    do {
        SampleJoystickDevice(
            &aInputDeviceSamples[
                nActiveInputDevice],
            nActiveInputDevice, 0);
    } while (aInputDeviceSamples[
                 nActiveInputDevice].buttons == 0);
}

/* Function start: 0x410740 */
void SetFleetOverviewView(int initializeCockpit)
{
    FixedVector centre;
    FixedVector orientation;
    FixedVector offset;
    int shipCount;
    int maximumRange;
    int playerRange;
    int cameraDistance;
    int range;
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
        zero_vector(&aShipVelocity[WC1_EYE_OBJECT]);
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
                centre.x += aShipPosition[object].x / shipCount;
                centre.y += aShipPosition[object].y / shipCount;
                centre.z += aShipPosition[object].z / shipCount;
            }
            object++;
        } while (object < 10);

        maximumRange = 0x4b000;
        playerRange = 0xff;
        object = 9;
        do {
            if (aeObjectClass[object] >= OBJECT_CLASS_SHIP) {
                ComputeVectorDelta(&centre,
                                   &aShipPosition[object],
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
        maximumRange = 0x4b000;
        playerRange = 0x4b000;
        position_relative_ijk(&centre, 0, 400, 400, 400);
    }

    cameraDistance = (playerRange >> 3) * 9 + 0x2bc00;
    aShipPosition[63] = centre;
    aShipRightVector[63] = orientation;
    aShipUpVector[63] = orientation;
    point_at(63, aShipPosition[0]);

    ScaleFixedVector(&aShipRightVector[63],
                     cameraDistance >> 2, &offset);
    AddFixedVectors(&aShipPosition[63], &offset,
                    &aShipPosition[63]);
    ScaleFixedVector(&aShipUpVector[63], 0x9600, &offset);
    AddFixedVectors(&aShipPosition[63], &offset,
                    &aShipPosition[63]);
    ScaleFixedVector(&aShipForwardVector[63],
                     cameraDistance, &offset);
    AddFixedVectors(&aShipPosition[63], &offset,
                    &aShipPosition[63]);

    aShipPosition[WC1_EYE_OBJECT] =
        aShipPosition[63];
    aShipRightVector[WC1_EYE_OBJECT] = orientation;
    aShipUpVector[WC1_EYE_OBJECT] = orientation;
    if (maximumRange < 0x271000)
        point_at(WC1_EYE_OBJECT, centre);
    else
        point_at(WC1_EYE_OBJECT, aShipPosition[0]);
}

/* Function start: 0x410A30 */
unsigned int rotate_eye_to_goal(void)
{
    short totalError;

    totalError = (short)(abs(anObjectPitchRotation[61] -
                            nEyePitchGoal) +
                         abs(anObjectYawRotation[61] -
                            nEyeYawGoal) +
                         abs(anObjectRollRotation[61] -
                            nEyeRollGoal));
    match_rotation_goal(&anObjectPitchRotation[61],
                        &nEyePitchGoal, totalError,
                        nEyePitchRate);
    match_rotation_goal(&anObjectYawRotation[61],
                        &nEyeYawGoal, totalError,
                        nEyeYawRate);
    match_rotation_goal(&anObjectRollRotation[61],
                        &nEyeRollGoal, totalError,
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
        copy_frame(0, WC1_EYE_OBJECT);
        aShipVelocity[WC1_EYE_OBJECT] =
            aShipVelocity[0];
        aShipPosition[WC1_EYE_OBJECT] =
            aShipPosition[0];
        return 0;
    case 1:
        aShipRightVector[WC1_EYE_OBJECT] =
            aShipForwardVector[0];
        negate_vector(&aShipRightVector[WC1_EYE_OBJECT]);
        aShipUpVector[WC1_EYE_OBJECT] =
            aShipUpVector[0];
        aShipForwardVector[WC1_EYE_OBJECT] =
            aShipRightVector[0];
        aShipVelocity[WC1_EYE_OBJECT] =
            aShipVelocity[0];
        aShipPosition[WC1_EYE_OBJECT] =
            aShipPosition[0];
        return 0;
    case 2:
        aShipRightVector[WC1_EYE_OBJECT] =
            aShipForwardVector[0];
        aShipUpVector[WC1_EYE_OBJECT] =
            aShipUpVector[0];
        aShipForwardVector[WC1_EYE_OBJECT] =
            aShipRightVector[0];
        negate_vector(&aShipForwardVector[WC1_EYE_OBJECT]);
        aShipVelocity[WC1_EYE_OBJECT] =
            aShipVelocity[0];
        aShipPosition[WC1_EYE_OBJECT] =
            aShipPosition[0];
        return 0;
    case 3:
        copy_frame(0, WC1_EYE_OBJECT);
        negate_vector(&aShipRightVector[WC1_EYE_OBJECT]);
        negate_vector(&aShipForwardVector[WC1_EYE_OBJECT]);
        aShipVelocity[WC1_EYE_OBJECT] =
            aShipVelocity[0];
        aShipPosition[WC1_EYE_OBJECT] =
            aShipPosition[0];
        return 0;
    case 4:
        if (aeObjectClass[cViewObject] !=
            OBJECT_CLASS_NULL) {
            ComputeVectorDelta(
                &aShipPosition[WC1_EYE_OBJECT],
                &aShipPosition[cViewObject],
                &viewDirection);
            desiredPosition = aShipForwardVector[0];
            SetVectorFixedPoint(
                (unsigned int *)&desiredPosition,
                (short)(bAlternateChaseView == 0 ? -700 : -500));
            AddFixedVectors(
                &desiredPosition,
                &aShipPosition[cViewObject],
                &desiredPosition);
            ComputeVectorDelta(
                &aShipPosition[WC1_EYE_OBJECT],
                &desiredPosition, &positionDelta);
            vector = positionDelta;
            divide_vector(
                &vector,
                (bAlternateChaseView == 0 ? 25 : 7) << 8,
                &vector);
            aShipVelocity[WC1_EYE_OBJECT] = vector;
            aShipForwardVector[WC1_EYE_OBJECT] = viewDirection;
            shrink_vector(
                &aShipForwardVector[WC1_EYE_OBJECT]);
            fix_objects_ijk(WC1_EYE_OBJECT);
            nEyeRollGoal = match_roll_orientation(
                WC1_EYE_OBJECT, (short)cViewObject);
            if (nEyeRollGoal != 0) {
                if (abs((int)nEyeRollGoal) < 5) {
                    aShipUpVector[WC1_EYE_OBJECT] =
                        aShipUpVector[0];
                    nEyeRollGoal = 0;
                } else {
                    nEyeRollRate = 4;
                    rotate_eye_to_goal();
                    rotate_object(WC1_EYE_OBJECT);
                }
            }
        }
        AddFixedVectors(&aShipPosition[WC1_EYE_OBJECT],
                        &aShipVelocity[WC1_EYE_OBJECT],
                        &aShipPosition[WC1_EYE_OBJECT]);
        return 0;
    case 5:
        zero_vector(&aShipVelocity[WC1_EYE_OBJECT]);
        ComputeVectorDelta(
            &aShipPosition[WC1_EYE_OBJECT],
            &aShipPosition[cViewObject], &vector);
        magnitude = Vector_magnitude(&vector);
        if (magnitude < 0x7d001) {
            aShipForwardVector[WC1_EYE_OBJECT] = vector;
            fix_objects_ijk(WC1_EYE_OBJECT);
            return 0;
        }
        vector = aShipVelocity[cViewObject];
        NormalizeFixedVector(&vector);
        aShipUpVector[WC1_EYE_OBJECT] =
            aShipUpVector[cViewObject];
        if (vector.x == aShipUpVector[WC1_EYE_OBJECT].x &&
            vector.y == aShipUpVector[WC1_EYE_OBJECT].y &&
            vector.z == aShipUpVector[WC1_EYE_OBJECT].z) {
            aShipUpVector[WC1_EYE_OBJECT] =
                aShipRightVector[cViewObject];
        }
        ScaleFixedVector(&vector, -0x12430, &vector);
        AddFixedVectors(
            &aShipPosition[cViewObject], &vector,
            &aShipPosition[WC1_EYE_OBJECT]);
        aShipForwardVector[WC1_EYE_OBJECT] = vector;
        fix_objects_ijk(WC1_EYE_OBJECT);
        ScaleFixedVector(&aShipRightVector[WC1_EYE_OBJECT],
                         0x19000, &vector);
        AddFixedVectors(&aShipPosition[WC1_EYE_OBJECT], &vector,
                        &aShipPosition[WC1_EYE_OBJECT]);
        ScaleFixedVector(&aShipUpVector[WC1_EYE_OBJECT],
                         0x6400, &vector);
        AddFixedVectors(&aShipPosition[WC1_EYE_OBJECT], &vector,
                        &aShipPosition[WC1_EYE_OBJECT]);
        ComputeVectorDelta(
            &aShipPosition[WC1_EYE_OBJECT],
            &aShipPosition[cViewObject], &vector);
        aShipForwardVector[WC1_EYE_OBJECT] = vector;
        fix_objects_ijk(WC1_EYE_OBJECT);
        generate_stars();
        return 0;
    case 6:
        if (nExternalViewShip == -1) {
            zero_vector(&aShipVelocity[WC1_EYE_OBJECT]);
            if (nExternalViewAngle++ > 20) {
                if (bCockpitlessView == 0) {
                    new_view(0, 0);
                } else {
                    GetScreenUpdateFlag();
                    SetViewportRect(
                        &stSpaceBuffer, 0, 0,
                        (unsigned short)(nScreenWidth - 1),
                        (unsigned short)(nScreenHeight - 1));
                    initialize_view_buffer();
                    new_view(0, 0);
                    GetScreenUpdateFlag();
                    SetViewportRect(&stSpaceBuffer, 0, 0, 319, 199);
                    initialize_view_buffer();
                }
            }
        } else {
            ComputeVectorDelta(
                &aShipPosition[WC1_EYE_OBJECT],
                &aShipPosition[nExternalViewShip],
                &vector);
            if (aeShipTactic[nExternalViewShip] !=
                TACTIC_CRUISE) {
                magnitude = Vector_magnitude(&vector);
                if (magnitude < 0xfa01) {
                    adjustment = vector;
                    NormalizeFixedVector(&adjustment);
                    ScaleFixedVector(&adjustment, -64000, &adjustment);
                    AddFixedVectors(
                        &vector, &adjustment,
                        &aShipVelocity[WC1_EYE_OBJECT]);
                } else {
                    divide_vector(
                        &vector,
                        (int)(short)(nExternalViewDistance &
                                     0xfffe) << 7,
                        &aShipVelocity[WC1_EYE_OBJECT]);
                    nExternalViewDistance = MaxShort(
                        (short)(nExternalViewDistance - 1), 8);
                }
            }
            aShipForwardVector[WC1_EYE_OBJECT] = vector;
            fix_objects_ijk(WC1_EYE_OBJECT);
        }
        AddFixedVectors(&aShipPosition[WC1_EYE_OBJECT],
                        &aShipVelocity[WC1_EYE_OBJECT],
                        &aShipPosition[WC1_EYE_OBJECT]);
        return 0;
    case 7:
        if (acShipTarget[0] != -1) {
            ScaleFixedVector(&aShipRightVector[0],
                             0x12c00, &vector);
            AddFixedVectors(&aShipPosition[0], &vector,
                            &aShipPosition[WC1_EYE_OBJECT]);
            ComputeVectorDelta(
                &aShipPosition[WC1_EYE_OBJECT],
                &aShipPosition[
                    acShipTarget[0]],
                &vector);
            NormalizeFixedVector(&vector);
            aShipForwardVector[WC1_EYE_OBJECT] = vector;
            ScaleFixedVector(&vector, -0x25800, &vector);
            AddFixedVectors(&aShipPosition[WC1_EYE_OBJECT],
                            &vector,
                            &aShipPosition[WC1_EYE_OBJECT]);
            fix_objects_ijk(WC1_EYE_OBJECT);
            return 0;
        }
        if (bCockpitlessView != 0) {
            GetScreenUpdateFlag();
            SetViewportRect(
                &stSpaceBuffer, 0, 0,
                (unsigned short)(nScreenWidth - 1),
                (unsigned short)(nScreenHeight - 1));
            initialize_view_buffer();
            bCockpitlessView = 0;
            new_view(0, 0);
            bCockpitlessView = 1;
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBuffer, 0, 0, 319, 199);
            initialize_view_buffer();
            return 0;
        }
        new_view(0, 0);
        return 0;
    case 8:
        ScaleFixedVector(&aShipForwardVector[WC1_EYE_OBJECT],
                         nCapitalShipViewDistance, &vector);
        SubtractFixedVectors(
            &aShipPosition[cViewObject], &vector,
            &aShipPosition[WC1_EYE_OBJECT]);
        if (Vector_magnitude(
                &aShipVelocity[WC1_EYE_OBJECT]) != 0) {
            AddFixedVectors(&aShipPosition[WC1_EYE_OBJECT],
                            &aShipVelocity[WC1_EYE_OBJECT],
                            &aShipPosition[WC1_EYE_OBJECT]);
            return 0;
        }
        break;
    case 9:
        copy_frame(0, WC1_EYE_OBJECT);
        negate_vector(&aShipRightVector[WC1_EYE_OBJECT]);
        negate_vector(&aShipForwardVector[WC1_EYE_OBJECT]);
        aShipVelocity[WC1_EYE_OBJECT] =
            aShipVelocity[0];
        aShipPosition[WC1_EYE_OBJECT] =
            aShipPosition[0];
        return 0;
    case 10:
        copy_frame((short)cViewObject, WC1_EYE_OBJECT);
        aShipVelocity[WC1_EYE_OBJECT] =
            aShipVelocity[cViewObject];
        aShipPosition[WC1_EYE_OBJECT] =
            aShipPosition[cViewObject];
        return 0;
    case 11:
        ComputeVectorDelta(
            &aShipPosition[WC1_EYE_OBJECT],
            &aShipPosition[cViewObject], &vector);
        magnitude = Vector_magnitude(&vector);
        if (magnitude < 0x25800) {
            adjustment = vector;
            NormalizeFixedVector(&adjustment);
            ScaleFixedVector(&adjustment, -0x25800, &adjustment);
            AddFixedVectors(&vector, &adjustment, &adjustment);
            AddFixedVectors(&aShipPosition[WC1_EYE_OBJECT],
                            &adjustment,
                            &aShipPosition[WC1_EYE_OBJECT]);
        }
        aShipForwardVector[WC1_EYE_OBJECT] = vector;
        fix_objects_ijk(WC1_EYE_OBJECT);
        AddFixedVectors(&aShipPosition[WC1_EYE_OBJECT],
                        &aShipVelocity[WC1_EYE_OBJECT],
                        &aShipPosition[WC1_EYE_OBJECT]);
        return 0;
    case 12:
        ScaleFixedVector(&aShipRightVector[WC1_EYE_OBJECT],
                         -0xa00, &vector);
        AddFixedVectors(&aShipPosition[WC1_EYE_OBJECT], &vector,
                        &aShipPosition[WC1_EYE_OBJECT]);
        AddFixedVectors(&aShipPosition[WC1_EYE_OBJECT],
                        &aShipVelocity[WC1_EYE_OBJECT],
                        &aShipPosition[WC1_EYE_OBJECT]);
        look_at(0);
        return 0;
    case 13:
        AddFixedVectors(&aShipPosition[WC1_EYE_OBJECT],
                        &aShipVelocity[WC1_EYE_OBJECT],
                        &aShipPosition[WC1_EYE_OBJECT]);
        look_at(0);
        return 0;
    case 14:
        SetFleetOverviewView(0);
        return 0;
    case 15:
        AddFixedVectors(&aShipPosition[WC1_EYE_OBJECT],
                        &aShipVelocity[WC1_EYE_OBJECT],
                        &aShipPosition[WC1_EYE_OBJECT]);
        rotate_object(WC1_EYE_OBJECT);
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
    FixedVector vector;
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
        asObjectCollisionRadius[WC1_EYE_OBJECT] =
            MaxShort(10, asObjectCollisionRadius[obj]);
    else
        asObjectCollisionRadius[WC1_EYE_OBJECT] = 10;
    switch (view) {
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
                             &aShipForwardVector[
                                 cViewObject],
#ifdef WC1_SDL
                             -1200 * 0x100, &vector);
#else
                             -1200 << 8, &vector);
#endif
            AddFixedVectors(
                            &aShipPosition[
                                cViewObject],
                            &vector,
                            &aShipPosition[WC1_EYE_OBJECT]);
            aShipUpVector[WC1_EYE_OBJECT] =
                aShipUpVector[cViewObject];
            aShipForwardVector[WC1_EYE_OBJECT] = vector;
            fix_objects_ijk(WC1_EYE_OBJECT);
            zero_vector(&aShipVelocity[WC1_EYE_OBJECT]);
        }
        break;
    case 5:
        initialize_cockpit(4);
        vector = aShipVelocity[cViewObject];
        if (NormalizeFixedVector(&vector) == 0)
            vector =
                aShipForwardVector[cViewObject];
        aShipUpVector[WC1_EYE_OBJECT] =
            aShipUpVector[cViewObject];
        if (vector.x ==
                aShipUpVector[WC1_EYE_OBJECT].x &&
            vector.y ==
                aShipUpVector[WC1_EYE_OBJECT].y &&
            vector.z ==
                aShipUpVector[WC1_EYE_OBJECT].z) {
            aShipUpVector[WC1_EYE_OBJECT] =
                aShipRightVector[cViewObject];
        }
        ScaleFixedVector(&vector, -0x12430, &vector);
        AddFixedVectors(
            &aShipPosition[cViewObject],
            &vector, &aShipPosition[WC1_EYE_OBJECT]);
        aShipForwardVector[WC1_EYE_OBJECT] = vector;
        fix_objects_ijk(WC1_EYE_OBJECT);
        zero_vector(&aShipVelocity[WC1_EYE_OBJECT]);
        ScaleFixedVector(&aShipRightVector[WC1_EYE_OBJECT],
                         0x19000, &vector);
        AddFixedVectors(&aShipPosition[WC1_EYE_OBJECT],
                        &vector,
                        &aShipPosition[WC1_EYE_OBJECT]);
        ScaleFixedVector(&aShipUpVector[WC1_EYE_OBJECT],
                         0x6400, &vector);
        AddFixedVectors(&aShipPosition[WC1_EYE_OBJECT],
                        &vector,
                        &aShipPosition[WC1_EYE_OBJECT]);
        ComputeVectorDelta(
            &aShipPosition[WC1_EYE_OBJECT],
            &aShipPosition[cViewObject],
            &vector);
        aShipForwardVector[WC1_EYE_OBJECT] = vector;
        fix_objects_ijk(WC1_EYE_OBJECT);
        break;
    case 6:
        initialize_cockpit(4);
        ScaleFixedVector(
            &aShipRightVector[nExternalViewShip],
            0x25800, &vector);
        AddFixedVectors(
            &aShipPosition[nExternalViewShip],
            &vector, &aShipPosition[WC1_EYE_OBJECT]);
        ComputeVectorDelta(
            &aShipPosition[WC1_EYE_OBJECT],
            &aShipPosition[nExternalViewShip],
            &vector);
        aShipUpVector[WC1_EYE_OBJECT] =
            aShipUpVector[0];
        aShipForwardVector[WC1_EYE_OBJECT] = vector;
        fix_objects_ijk(WC1_EYE_OBJECT);
        zero_vector(&aShipVelocity[WC1_EYE_OBJECT]);
        nExternalViewDistance = 0x20;
        nExternalViewAngle = 0;
        break;
    case 7:
        initialize_cockpit(4);
        copy_frame(0, WC1_EYE_OBJECT);
        zero_vector(&aShipVelocity[WC1_EYE_OBJECT]);
        break;
    case 8:
        initialize_cockpit(4);
        aShipRightVector[WC1_EYE_OBJECT] =
            aShipRightVector[cViewObject];
        aShipUpVector[WC1_EYE_OBJECT] =
            aShipForwardVector[cViewObject];
        negate_vector(&aShipUpVector[WC1_EYE_OBJECT]);
        aShipForwardVector[WC1_EYE_OBJECT] =
            aShipUpVector[cViewObject];
        fix_objects_ijk(WC1_EYE_OBJECT);
        zero_vector(&aShipVelocity[WC1_EYE_OBJECT]);
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
        nEyePitchGoal = 0;
        nEyeYawGoal = 0;
        nEyeRollGoal = 0;
        nEyePitchRate = 1;
        nEyeYawRate = 1;
        nEyeRollRate = 1;
        break;
    case 12:
        initialize_cockpit(4);
        copy_frame(0, WC1_EYE_OBJECT);
        position_relative_ijk(&aShipPosition[WC1_EYE_OBJECT],
                              0, 500, 0, 2000);
        zero_vector(&aShipVelocity[WC1_EYE_OBJECT]);
        look_at(0);
        break;
    case 13:
        initialize_cockpit(4);
        zero_vector(&aShipVelocity[0]);
        if (nYourWingman != -1)
            zero_vector(
                &aShipVelocity[nYourWingman]);
        copy_frame(cViewObject, WC1_EYE_OBJECT);
        carrier = find_ship_index(nCarrierMissionShipIndex);
        look_at(carrier);
        position_relative_ijk(
            &aShipPosition[WC1_EYE_OBJECT],
            WC1_EYE_OBJECT, 0, -10, -400);
        aShipVelocity[WC1_EYE_OBJECT] =
            aShipForwardVector[WC1_EYE_OBJECT];
        ScaleFixedVector(&aShipVelocity[WC1_EYE_OBJECT],
                         -0x2300,
                         &aShipVelocity[WC1_EYE_OBJECT]);
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
    ScaleFixedVector(&aShipForwardVector[WC1_EYE_OBJECT],
                     (int)forwardDistance << 8, &offset);
    AddFixedVectors(&origin, &offset, &origin);
    ScaleFixedVector(&aShipRightVector[WC1_EYE_OBJECT],
                     rightOffset, &offset);
    AddFixedVectors(&origin, &offset, &origin);
    ScaleFixedVector(&aShipUpVector[WC1_EYE_OBJECT],
                     upOffset, &offset);
    AddFixedVectors(&origin, &offset, &origin);
    AddFixedVectors(&origin,
                    &aShipPosition[WC1_EYE_OBJECT],
                    &aShipPosition[obj]);
    FillFixedVectorWithRandomComponents(2, &aShipVelocity[obj]);
    streak = MaxShort((short)(1 - RandomInRange(0, 3)), 0);
    asObjectScreenAngle[obj] =
        (short)(streak * 0x10 + RandomInRange(0, 3));
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
#ifdef WC1_SDL
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
            nStarFieldIRotation = signed_random(45);
            nStarFieldJRotation = signed_random(45);
            copy_frame(WC1_EYE_OBJECT, 63);
            rotate_about_j(nStarFieldJRotation,
                           &aShipRightVector[63],
                           &aShipForwardVector[63]);
            rotate_about_i(nStarFieldIRotation,
                           &aShipUpVector[63],
                           &aShipForwardVector[63]);
            ScaleFixedVector(&aShipForwardVector[63],
                             15000 << 8,
                             &aShipPosition[obj]);
            asObjectViewFrame[obj] =
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
    ScaleFixedVector(&aShipForwardVector[WC1_EYE_OBJECT],
                     200 << 8, &cameraMotion);
    ScaleFixedVector(&aShipVelocity[WC1_EYE_OBJECT],
                     20 << 8, &vStarFieldMotion);
    AddFixedVectors(&cameraMotion, &vStarFieldMotion,
                    &vStarFieldMotion);
    ComputeVectorDelta(&vPreviousStarFieldMotion,
                       &vStarFieldMotion, &origin);
    for (obj = 34; obj < 49; obj++) {
        objectIndex = (int)obj;
        objectOffset = objectIndex * sizeof(short);
        if (*(short *)((unsigned char *)asObjectScreenX +
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
                randomChoice == 0 &&
                (anObjectYawRotation[0] |
                 anObjectPitchRotation[0]) != 0) {
                copy_frame(WC1_EYE_OBJECT, 63);
                if (anObjectPitchRotation[0] != 0) {
                    nStarFieldIRotation =
                        anObjectPitchRotation[0] < 0 ?
                            -45 : 45;
                    nStarFieldJRotation = signed_random(45);
                }
                if (anObjectYawRotation[0] != 0 &&
                    (anObjectPitchRotation[0] == 0 ||
                     RandomInRange(0, 1) != 0)) {
                    nStarFieldJRotation =
                        anObjectYawRotation[0] < 0 ? -45 : 45;
                    nStarFieldIRotation = signed_random(45);
                }
                rotate_about_i(nStarFieldIRotation,
                               &aShipUpVector[63],
                               &aShipForwardVector[63]);
                rotate_about_j(nStarFieldJRotation,
                               &aShipRightVector[63],
                               &aShipForwardVector[63]);
                ScaleFixedVector(&aShipForwardVector[63],
                                 15000 << 8,
                                 &aShipPosition[objectIndex]);
                *(short *)((unsigned char *)asObjectViewFrame +
                           objectOffset) =
                    (short)(RandomInRange(0, 5) + 32);
                break;
            }
            if (aeObjectClass[objectIndex] == OBJECT_CLASS_DUST &&
                randomChoice < 2) {
                transform_to_objects_frame(
                    &aShipVelocity[WC1_EYE_OBJECT],
                    &viewMotion, WC1_EYE_OBJECT);
                ScaleFixedVector(&viewMotion, 10 << 8, &viewMotion);
                if (viewMotion.z >= 0) {
                    distance =
                        (unsigned short)RandomInRange(
                            0, (short)(viewMotion.z >> 8)) +
                        asObjectCollisionRadius[WC1_EYE_OBJECT];
                    distance = distance * 2 +
                        (unsigned short)RandomInRange(0, 350);
                } else {
                    distance = (unsigned short)RandomInRange(0, 40) +
                        asObjectCollisionRadius[WC1_EYE_OBJECT];
                }
                rightRandom = signed_random((short)(distance >> 1));
                upRandom = signed_random((short)(distance >> 1));
                shift = (unsigned char)(viewMotion.z <= 0 ? 9 : 8);
#ifdef WC1_SDL
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
    if (asObjectCounter[obj] != -1)
        asObjectCounter[obj]--;
    return asObjectCounter[obj];
}

/* Function start: 0x412430 */
unsigned int house_keep_objects(void)
{
    short obj;

    for (obj = 0; obj <= WC1_SPACE_LAST_MOVING_OBJECT; obj++) {
        switch (aeObjectClass[obj]) {
        case OBJECT_CLASS_DUST:
            if (aeObjectType[obj] == OBJECT_TYPE_DEBRIS_DUST &&
                count_down(obj) == -1 &&
                asObjectScreenX[obj] == (short)0x8001)
                remove_object(obj);
            break;
        case OBJECT_CLASS_DEBRIS:
            if (count_down(obj) == -1)
                remove_object(obj);
            break;
        case OBJECT_CLASS_FIXED_OBJECT:
            if (aeObjectType[obj] == OBJECT_TYPE_TURRET ||
                aeObjectType[obj] == OBJECT_TYPE_THRUSTERS)
                remove_object(obj);
            break;
        case OBJECT_CLASS_PROJECTILE:
            if (count_down(obj) == 0) {
                if (aeObjectType[obj] == OBJECT_TYPE_TURRET)
                    explode((short)acObjectOwner[obj], obj);
                else
                    remove_object(obj);
            }
            break;
        case OBJECT_CLASS_MINE:
            if (acObjectCollisionGraceTicks[obj] > 0)
                acObjectCollisionGraceTicks[obj]--;
            if (count_down(obj) == 0)
                explode(obj, obj);
            break;
        case OBJECT_CLASS_MISSILE:
            abShipExhaustHeat[obj] = 0;
            if (acObjectCollisionGraceTicks[obj] > 0)
                acObjectCollisionGraceTicks[obj]--;
            if (aeShipTactic[obj] == TACTIC_SIT_STILL) {
                if (count_down(obj) <= 0) {
                    aeShipTactic[obj] = TACTIC_RAM;
                    asObjectCounter[obj] =
                        aObjectTypeData[
                            aeObjectType[obj]].lifetime;
                    if (aeObjectType[obj] ==
                            OBJECT_TYPE_DUMB_FIRE_MISSILE) {
                        vector_component_in_dir(
                            &aShipVelocity[obj],
                            &aShipForwardVector[obj],
                            &aShipVelocity[obj]);
                    }
                }
            } else if (count_down(obj) <= 0) {
                explode(obj, obj);
            }
            break;
        case OBJECT_CLASS_SHIP:
        case OBJECT_CLASS_CAPITAL_SHIP:
            abShipExhaustHeat[obj] = 0;
            if (count_down(obj) > 0) {
                if (aeShipManeuver[obj] ==
                        MANEUVER_WARPING_OUT)
                    asObjectScale[obj] >>= 1;
                if (aeSpecialManeuver[obj] ==
                        SPECIAL_MANEUVER_UNKNOWN_9 &&
                    aeObjectClass[obj] ==
                        OBJECT_CLASS_CAPITAL_SHIP) {
                    if (asObjectCounter[obj] == 7) {
                        ShipExplosion(obj);
                        explosion_shock_wave(
                            obj, aObjectTypeData[
                                aeObjectType[obj]].
                                    explosionDamage);
                    } else {
                        while ((unsigned short)RandomInRange(0, 100) < 50)
                            onboard_explosion(obj);
                    }
                }
            } else if (asObjectCounter[obj] == 0 &&
                       aeSpecialManeuver[obj] ==
                           SPECIAL_MANEUVER_UNKNOWN_9) {
                if (nYourWingman != -1 &&
                    acLastAttacker[obj] == 0 &&
                    aeShipSide[obj] == SIDE_KILRATHI &&
                    RandomBelowOrEqual(100) < 10) {
                    send_message(nYourWingman, 6);
                }
                Create_explosion_debris(obj);
                break;
            } else if (asObjectCounter[obj] == 0) {
                if (aeShipManeuver[obj] ==
                        MANEUVER_WARPING_IN) {
                    if (aeShipTactic[obj] != TACTIC_WARP_IN) {
                        if ((short)acObjectOwner[obj] == obj) {
                            set_objects_data(
                                obj,
                                (enum ObjectType)
                                    abShipNavPointIndex[obj],
                                -1);
                            reset_maneuver(obj, -1);
                        } else {
                            remove_object(obj);
                        }
                    }
                } else if (aeShipManeuver[obj] ==
                               MANEUVER_WARPING_OUT &&
                           aeShipSide[obj] != SIDE_NEUTRAL) {
                    aMissionShips[
                        nShipMissionIndices[obj]].state = 2;
                    remove_object(obj);
                }
            }
            if (aeObjectType[obj] ==
                    OBJECT_TYPE_TIGERS_CLAW &&
                bPlayerCollisionsEnabled != 0 &&
                bLandingAuthorized != 0 &&
                normal_speed(0) != 0) {
                get_facing_range_from_object(0, obj);
#ifdef WC1_SDL
                /* WC2 lands from any bearing; the original also required the
                   Claw's bow to face the player. */
                if (nTargetRange < 700 &&
                    nFacingToTarget > 75) {
#else
                if (nTargetRange < 700 &&
                    nFacingToTarget > 75 &&
                    nTargetFacing > 70) {
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
                    aeSpecialManeuver[obj] !=
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
    } while (obj <= WC1_SPACE_LAST_MOVING_OBJECT);

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
    } while (obj <= WC1_SPACE_LAST_MOVING_OBJECT);
    return 0;
}

/* Function start: 0x412920 */
unsigned int rotate_object(short obj)
{
    if (anObjectPitchRotation[obj] != 0) {
        alter_pitch(anObjectPitchRotation[obj], obj);
        ClampVectorTo30(&anObjectPitchRotation[obj]);
    }
    if (anObjectYawRotation[obj] != 0) {
        alter_yaw(anObjectYawRotation[obj], obj);
        ClampVectorTo30(&anObjectYawRotation[obj]);
    }
    if (anObjectRollRotation[obj] != 0) {
        alter_roll(anObjectRollRotation[obj], obj);
        ClampVectorTo30(&anObjectRollRotation[obj]);
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

    if (aeObjectClass[obj] >= OBJECT_CLASS_MISSILE) {
        if (aeSpecialManeuver[obj] ==
                SPECIAL_MANEUVER_KILL_ENGINES) {
            abShipExhaustHeat[obj] = 0;
            if (RandomBelowOrEqual(100) < 10)
                set_special(obj, SPECIAL_MANEUVER_NONE);
        } else if (aeSpecialManeuver[obj] ==
                       SPECIAL_MANEUVER_STOP_DRIFT) {
            approach_zero_speed(obj);
            NormalizeAndScaleVector(&aShipVelocity[obj],
                                    anShipSpeed[obj]);
            if (anShipSpeed[obj] == 0)
                set_special(obj, SPECIAL_MANEUVER_NONE);
        }
        if (aeSpecialManeuver[obj] <
                SPECIAL_MANEUVER_KILL_ENGINES &&
            aeShipTactic[obj] != TACTIC_SIT_STILL) {
            zero_vector(&accelerationVector);
            switch (aeSpecialManeuver[obj]) {
            case SPECIAL_MANEUVER_AFTERBURNER:
                asShipAfterburnerTimer[obj]--;
                if (asShipAfterburnerTimer[obj] == 0) {
                    aeSpecialManeuver[obj] =
                        SPECIAL_MANEUVER_NONE;
                    if (bAfterburnerSfxActive != 0) {
                        FlushSoundEffectsAndLog();
                        bAfterburnerSfxActive = 0;
                    }
                    ScaleFixedVector(&aShipForwardVector[obj],
                                     anShipSpeed[obj], &delta);
                } else {
                    ScaleFixedVector(
                        &aShipForwardVector[obj],
                        (aObjectTypeData[
                            aeObjectType[obj]].maximumVelocity +
                         20) * 0x200,
                        &delta);
                    drain_fuel(obj, 200);
                    abShipExhaustHeat[obj] = 3;
                }
                break;
            case SPECIAL_MANEUVER_SUPER_BRAKE:
                asShipAfterburnerTimer[obj]--;
                if (asShipAfterburnerTimer[obj] == 0) {
                    aeSpecialManeuver[obj] =
                        SPECIAL_MANEUVER_NONE;
                    ScaleFixedVector(&aShipForwardVector[obj],
                                     anShipSpeed[obj], &delta);
                } else {
                    zero_vector(&delta);
                    drain_fuel(obj, 140);
                }
                break;
            default:
                ScaleFixedVector(&aShipForwardVector[obj],
                                 anShipSpeed[obj], &delta);
                break;
            }
            ComputeVectorDelta(&aShipVelocity[obj], &delta,
                               &delta);
            magnitude = Vector_magnitude(&delta);
            if (magnitude > 0) {
                acceleration = GetShipAccelerationRate(obj);
                if (alert_flag(obj, 1) != 0 && acceleration < 0x500)
                    acceleration = 0x500;
                if (aeSpecialManeuver[obj] ==
                        SPECIAL_MANEUVER_AFTERBURNER ||
                    aeSpecialManeuver[obj] ==
                        SPECIAL_MANEUVER_SUPER_BRAKE) {
                    acceleration *= 2;
                }
                acceleration = MultiplyFixed(
                    acceleration,
                    DivideFixed(
                        dot_product(
                            &delta,
                            &aShipForwardVector[obj]),
                        magnitude) + 0x200);
                ScaleFixedVector(
                    &delta,
                    MinInt(DivideFixed(acceleration >> 1, magnitude),
                           0x100),
                    &accelerationVector);
                if (aeSpecialManeuver[obj] !=
                        SPECIAL_MANEUVER_AFTERBURNER) {
                    abShipExhaustHeat[obj] = 2;
                }
            }
            AddFixedVectors(&accelerationVector,
                            &aShipVelocity[obj],
                            &aShipVelocity[obj]);
            if (obj == 0)
                vPlayerAcceleration = accelerationVector;
        }
    }
    AddFixedVectors(&aShipPosition[obj],
                    &aShipVelocity[obj],
                    &aShipPosition[obj]);
    return 0;
}

/* Function start: 0x412CD0 */
unsigned int animate_shape(short obj)
{
    unsigned char *animation;
    enum ObjectType type;
    short command;

    type = aeObjectType[obj];
    animation = aObjectTypeData[type].animation;
    if (animation == 0)
        return 0;
    if (--asObjectAnimationDelay[obj] > 0)
        return 0;
    asObjectAnimationDelay[obj] =
        aObjectTypeData[type].yawRate;
    command = *(unsigned short *)(animation +
              asObjectAnimationIndex[obj] * 4);
    switch (command & 0xf000) {
    case 0x9000:
        command &= 0x0fff;
        asObjectAnimationIndex[obj] = command;
        command = *(unsigned short *)(animation + command * 4);
        if (asObjectScreenX[obj] != (short)0x8001 &&
            (type == OBJECT_TYPE_DEBRIS_WING ||
             type == OBJECT_TYPE_DEBRIS_METAL_SHEET)) {
            PlaySfxWaveFileByNumber(13, obj, 0);
        }
        break;
    case 0xa000:
        remove_object(obj);
        return 0;
    }

    if ((command & 0x0c00) == 0x0400) {
        asObjectScale[obj] +=
            (command & 0x3f) * (asObjectScale[obj] >> 6);
    } else if ((command & 0x0c00) == 0x0800) {
        asObjectScale[obj] -=
            (command & 0x3f) * (asObjectScale[obj] >> 6);
    } else {
        command &= 0x3f;
        asObjectViewFrame[obj] = command;
    }
    asObjectFlip[obj] = (command & 0xc0) >> 2;
    asObjectAnimationIndex[obj]++;
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
        if (asObjectScreenX[obj] == (short)0x8001 ||
            (nRenderedSpaceFrame & 3) != 0 ||
            (aObjectTypeData[
                 aeObjectType[obj]].damageCapacity >> 1) - 1 >
                asShipAccumulatedDamage[obj]) {
            break;
        }
        effect = find_vacant_3d_object();
        if (effect == -1)
            break;
        ScaleFixedVector(&aShipForwardVector[obj],
                         -((int)asObjectCollisionRadius[obj]
                             << 8),
                         &offset);
        AddFixedVectors(&aShipPosition[obj], &offset,
                        &aShipPosition[effect]);
        FillFixedVectorWithRandomComponents(20, &offset);
        AddFixedVectors(&aShipPosition[effect], &offset,
                        &aShipPosition[effect]);
        zero_vector(&aShipVelocity[effect]);
        set_objects_data(
            effect,
            (enum ObjectType)((unsigned short)RandomInRange(0, 2) +
                              OBJECT_TYPE_RED_SPARK),
            obj);
        asObjectScale[effect] =
            asObjectScale[obj];
        if (RandomInRange(0, 3) == 0)
            PlaySfxWaveFileByNumber(7, obj, 0, 0);
        break;
    }
    return 0;
}

/* Function start: 0x413030 */
unsigned int hit_asteroid(short asteroid, short destructionChance)
{
    short fragments;

    if (RandomBelowOrEqual((short)(destructionChance - 1)) == 0) {
        fragments = (short)(RandomBelowOrEqual(1) + 2);
        while (fragments > 0) {
            make_shard(asteroid, aShipVelocity[asteroid]);
            fragments--;
        }
        explode(-1, asteroid);
    } else if (RandomBelowOrEqual(7) == 0) {
        make_shard(asteroid, vCollisionDelta);
    }
    return 0;
}

/* Function start: 0x4130D0 */
int object_collision(short obj)
{
    SphericalVector impact;
    FixedVector relativeVelocity;
    FixedVector separation;
    FixedVector objectComponent;
    FixedVector partnerComponent;
    FixedVector componentDelta;
    FixedVector tangent;
    FixedVector partnerTangent;
    FixedVector impulse;
    FixedVector force;
    enum ObjectClass partnerClass;
    int separationScale;
    int responseScale;
    int forceMagnitude;
    int totalMass;
    int objectMass;
    int partnerMass;
    int collisionSpeed;
    short partner;
    short damage;
    short savedScale;
    signed char owner;

    partner = check_for_collision(obj);
    if (partner == -1) {
        acLastCollisionObject[obj] = -1;
        return 0;
    }
    if (bPlayerCollisionResponse == 0 && (obj == 0 || partner == 0))
        return 0;

    NormalizeFixedVector(&vCollisionDelta);
    ComputeVectorDelta(&aShipVelocity[partner],
                       &aShipVelocity[obj],
                       &relativeVelocity);
    NormalizeFixedVector(&relativeVelocity);
    owner = acObjectOwner[obj];
    switch (aeObjectClass[obj]) {
    case OBJECT_CLASS_PROJECTILE:
        if (owner == partner)
            break;
        partnerClass = aeObjectClass[partner];
        if (partnerClass > OBJECT_CLASS_MINE) {
            if (partner == 0) {
                rectangular_to_spherical(
                    &aObjectViewPosition[obj], &impact);
                if (abs(impact.pitch) < 45) {
                    if (abs(impact.yaw) < 45)
                        aPaletteFadeEntries[1][0] = 0x38;
                    else if (abs(impact.yaw) < 136) {
                        if (impact.yaw < 0)
                            aPaletteFadeEntries[3][0] = 0x38;
                        else
                            aPaletteFadeEntries[5][0] = 0x38;
                    }
                    else
                        aPaletteFadeEntries[0][0] = 0x38;
                } else if (impact.pitch < 0)
                    aPaletteFadeEntries[2][0] = 0x38;
                else
                    aPaletteFadeEntries[4][0] = 0x38;
            }
            acLastAttacker[partner] = owner;
            acShipAiCooldown[partner] += 4;
            damage = (short)(asShipAccumulatedDamage[obj] -
                             asObjectCounter[obj] / 2);
            force = aShipVelocity[obj];
            NormalizeFixedVector(&force);
            ScaleFixedVector(&force, (int)damage << 8, &force);
            negate_vector(&vCollisionDelta);
            apply_force_to_object(&vCollisionDelta,
                                  &force, partner);
            inflict_damage(obj, partner, damage, &relativeVelocity);
        }
        savedScale = asObjectScale[obj];
        set_objects_data(obj, OBJECT_TYPE_LASER_SPARK, owner);
        asObjectScale[obj] = (short)(savedScale * 2);
        aShipVelocity[obj] =
            aShipVelocity[partner];
        if (partnerClass == OBJECT_CLASS_ASTEROID)
            hit_asteroid(partner, 3);
        return 0;

    case OBJECT_CLASS_ASTEROID:
        if (aeObjectClass[partner] ==
                OBJECT_CLASS_ASTEROID) {
            if (asObjectScreenX[obj] == (short)0x8001) {
                remove_object(obj);
                return 0;
            }
            hit_asteroid(obj, 0);
            return 0;
        }
        break;

    case OBJECT_CLASS_MINE:
        if (owner == partner ||
            acObjectCollisionGraceTicks[obj] > 0)
            return 0;
        if (easy2see(obj) == 0 &&
            (nCameraViewMode == 0 || partner != 0)) {
            remove_object(obj);
            return 0;
        }
        explode(obj, obj);
        return 0;

    case OBJECT_CLASS_MISSILE:
        if (owner != partner ||
            acObjectCollisionGraceTicks[obj] < 1) {
            ScaleFixedVector(&aShipVelocity[obj],
                (unsigned short)asObjectRadarRadius[obj]
                    << 8, &force);
            negate_vector(&vCollisionDelta);
            apply_force_to_object(&vCollisionDelta,
                                  &force, partner);
            explode(obj, obj);
            zero_vector(&aShipVelocity[obj]);
            return 0;
        }
        break;

    case OBJECT_CLASS_SHIP:
    case OBJECT_CLASS_CAPITAL_SHIP:
        partnerClass = aeObjectClass[partner];
        if ((partnerClass == OBJECT_CLASS_ASTEROID ||
             (partnerClass == OBJECT_CLASS_MINE &&
              acObjectOwner[partner] == -1)) &&
            easy2see(partner) == 0 &&
            (nCameraViewMode == 0 || obj != 0)) {
            remove_object(partner);
        }
        partnerClass = aeObjectClass[partner];
        if ((partnerClass == OBJECT_CLASS_ASTEROID ||
             partnerClass == OBJECT_CLASS_SHIP ||
             partnerClass == OBJECT_CLASS_CAPITAL_SHIP) &&
            acLastCollisionObject[obj] != partner) {
            PlaySfxWaveFileByNumber(0x1c, obj, 0);
            acLastCollisionObject[obj] =
                (signed char)partner;
            acLastCollisionObject[partner] =
                (signed char)obj;

            separationScale = DivideFixed(
                (asObjectCollisionRadius[obj] +
                 asObjectCollisionRadius[partner]) << 8,
                Vector_magnitude(&vCollisionDelta));
            separationScale = MinInt(separationScale, 0x7d000);
            ScaleFixedVector(&vCollisionDelta,
                             separationScale, &separation);
            AddFixedVectors(&aShipPosition[obj], &separation,
                            &aShipPosition[partner]);

            vector_component_in_dir(
                &aShipVelocity[obj],
                &vCollisionDelta, &objectComponent);
            SubtractFixedVectors(&aShipVelocity[obj],
                                 &objectComponent, &tangent);
            vector_component_in_dir(
                &aShipVelocity[partner],
                &vCollisionDelta, &partnerComponent);
            ComputeVectorDelta(&partnerComponent, &objectComponent,
                               &componentDelta);
            collisionSpeed =
                (short)((unsigned int)Vector_magnitude(&componentDelta) >> 8);
            damage = (short)((collisionSpeed * collisionSpeed) >> 1);
#ifdef WC1_SDL
            if (obj == 0 || partner == 0)
                Wc1SdlQueueJoystickCollisionRumble(collisionSpeed);
#endif

            objectMass = (unsigned short)
                asObjectRadarRadius[obj];
            partnerMass = (unsigned short)
                asObjectRadarRadius[partner];
            totalMass = objectMass + partnerMass;
            responseScale = ((objectMass - partnerMass) * 256) /
                            totalMass;
            responseScale = MaxInt(0x40, responseScale);
            responseScale = MinInt(responseScale, 0x400);
            ScaleFixedVector(&componentDelta, responseScale, &impulse);
            AddFixedVectors(&impulse, &partnerComponent, &impulse);
            forceMagnitude = MultiplyFixed(
                objectMass * 0x600,
                Vector_magnitude(&impulse)) + 0xa00;
            AddFixedVectors(&impulse, &aShipVelocity[obj],
                            &aShipVelocity[obj]);

            if (aeObjectClass[obj] == OBJECT_CLASS_SHIP) {
                NormalizeFixedVector(&tangent);
                negate_vector(&tangent);
                ScaleFixedVector(&tangent, forceMagnitude, &tangent);
                rotational_acceleration(&vCollisionDelta,
                                        &tangent, obj);
                negate_vector(&relativeVelocity);
                inflict_damage(partner, obj, damage, &relativeVelocity);
                negate_vector(&relativeVelocity);
            }

            SubtractFixedVectors(&aShipVelocity[partner],
                                 &partnerComponent, &partnerTangent);
            responseScale = (objectMass << 9) / totalMass;
            responseScale = MaxInt(0x40, responseScale);
            responseScale = MinInt(responseScale, 0x400);
            ScaleFixedVector(&componentDelta, responseScale, &impulse);
            AddFixedVectors(&impulse, &partnerComponent, &impulse);
            AddFixedVectors(&impulse,
                            &aShipVelocity[partner],
                            &aShipVelocity[partner]);
            if (partnerClass == OBJECT_CLASS_SHIP) {
                NormalizeFixedVector(&partnerTangent);
                negate_vector(&partnerTangent);
                ScaleFixedVector(&partnerTangent, forceMagnitude,
                                 &partnerTangent);
                negate_vector(&vCollisionDelta);
                rotational_acceleration(&vCollisionDelta,
                                        &partnerTangent, partner);
                inflict_damage(obj, partner, damage, &relativeVelocity);
            }
            if (partnerClass == OBJECT_CLASS_CAPITAL_SHIP) {
                SubtractFixedVectors(&aShipPosition[obj],
                                     &aShipVelocity[obj],
                                     &aShipPosition[obj]);
                anShipSpeed[obj] = 0;
                aShipVelocity[obj] =
                    aShipVelocity[partner];
            }
        }
        break;
    }
    return 0;
}

/* Function start: 0x413880 */
unsigned int object_intelligence(short obj)
{
    if (nCannedSceneMode == 4)
        return 0;
    if (nCannedSceneMode == 2 &&
        aeObjectClass[obj] > OBJECT_CLASS_MISSILE) {
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
        if ((abs((int)nSpaceFrame) & 3) != 0 &&
            nExternalViewShip != obj) {
            break;
        }
        if (acShipTarget[obj] != -1) {
            get_facing_range_from_object(
                obj, acShipTarget[obj]);
        }
        switch (aeObjectType[obj]) {
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
            point_ship(obj, 0, &vToTarget);
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
