/*
 *  Space-object simulation (Mac `spc` compilation unit), preceded by the
 *  Win32 joystick calibration/input helpers in this provisional range.
 *
 *  Address range 0x410000-0x412fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: the Mac `spc` symbol run maps rotate_eye_to_goal through
 *  object_intelligence to 0x410a30-0x4139ff.
 */
#include "wc1.h"

/* Function start: 0x4102B0 */
void CalibrateJoystickInteractive()
{
    InputDeviceSample samples[2];
    short calibration[6];
    short device;
    short shown;
    int file;
    int failed;

    device = g_nActiveInputDevice_005a819c;
    if (device == -1)
        return;

    g_stDefaultTextContext_005a7740.alignment = 2;
    InitializeTextContextFromFont(&g_stDefaultTextContext_005a7740, 1,
        g_cViewportClearColour_004699a0, (signed char)DAT_0046999c);
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
        SampleJoystickDevice(samples, device, 0x7fff);
        g_nJoystickMinimumX_005a81b8 = samples[device].x;
        g_nJoystickMinimumY_005a81bc = samples[device].y;
        calibration[0] = (short)samples[device].x;
        calibration[1] = (short)samples[device].y;
        WaitForJoystickButtonRelease();
        ReleaseModalTextPanel();
    }

    shown = ShowModalTextPanel(1,
        "Move stick to the LOWER RIGHT, press a button");
    if (shown != 0) {
        DIBslamReal();
        WaitForJoystickButtonPress();
        SampleJoystickDevice(samples, device, 0x7fff);
        g_nJoystickMaximumX_005a81b0 = samples[device].x;
        g_nJoystickMaximumY_005a81b4 = samples[device].y;
        calibration[2] = (short)samples[device].x;
        calibration[3] = (short)samples[device].y;
        WaitForJoystickButtonRelease();
        ReleaseModalTextPanel();
    }

    shown = ShowModalTextPanel(1,
        "Center Joystick, press a button");
    if (shown != 0) {
        DIBslamReal();
        WaitForJoystickButtonPress();
        SampleJoystickDevice(samples, device, 0x7fff);
        g_nJoystickCentreX_005a81dc = samples[device].x;
        g_nJoystickCentreY_005a81d8 = samples[device].y;
        calibration[4] = (short)samples[device].x;
        calibration[5] = (short)samples[device].y;
        WaitForJoystickButtonRelease();
        ReleaseModalTextPanel();
    }

    if (g_nJoystickHorizontalRange_005a81cc == 0)
        g_nJoystickHorizontalRange_005a81cc = 9;
    if (g_nJoystickVerticalRange_005a81c8 == 0)
        g_nJoystickVerticalRange_005a81c8 = 9;
    g_nJoystickLeftScale_005a81ac =
        (g_nJoystickCentreX_005a81dc - g_nJoystickMinimumX_005a81b8) /
        g_nJoystickHorizontalRange_005a81cc;
    g_nJoystickRightScale_005a81d0 =
        (g_nJoystickMaximumX_005a81b0 - g_nJoystickCentreX_005a81dc) /
        g_nJoystickHorizontalRange_005a81cc;
    g_nJoystickUpScale_005a81a8 =
        (g_nJoystickCentreY_005a81d8 - g_nJoystickMinimumY_005a81bc) /
        g_nJoystickVerticalRange_005a81c8;
    g_nJoystickDownScale_005a81d4 =
        (g_nJoystickMaximumY_005a81b4 - g_nJoystickCentreY_005a81d8) /
        g_nJoystickVerticalRange_005a81c8;
    if (g_nJoystickLeftScale_005a81ac == 0)
        g_nJoystickLeftScale_005a81ac = 1;
    if (g_nJoystickRightScale_005a81d0 == 0)
        g_nJoystickRightScale_005a81d0 = 1;
    if (g_nJoystickUpScale_005a81a8 == 0)
        g_nJoystickUpScale_005a81a8 = 1;
    if (g_nJoystickDownScale_005a81d4 == 0)
        g_nJoystickDownScale_005a81d4 = 1;

    g_nJoystickMinimumX_005a81b8 = g_nJoystickCentreX_005a81dc -
        g_nJoystickLeftScale_005a81ac *
            g_nJoystickHorizontalRange_005a81cc;
    g_nJoystickMinimumY_005a81bc = g_nJoystickCentreY_005a81d8 -
        g_nJoystickUpScale_005a81a8 * g_nJoystickVerticalRange_005a81c8;
    g_nJoystickMaximumX_005a81b0 = g_nJoystickCentreX_005a81dc +
        g_nJoystickRightScale_005a81d0 *
            g_nJoystickHorizontalRange_005a81cc;
    g_nJoystickMaximumY_005a81b4 = g_nJoystickCentreY_005a81d8 +
        g_nJoystickDownScale_005a81d4 * g_nJoystickVerticalRange_005a81c8;
    g_nJoystickFailureValue_005a81e0 = g_nJoystickMaximumX_005a81b0 * 2;

    if (g_nJoystickMaximumX_005a81b0 <= g_nJoystickMinimumX_005a81b8 ||
        g_nJoystickMaximumY_005a81b4 <= g_nJoystickMinimumY_005a81bc) {
        shown = ShowModalTextPanel(1,
            "FAILED! Center Joystick, press a button");
        if (shown != 0) {
            DIBslamReal();
            WaitForJoystickButtonPress();
            WaitForJoystickButtonRelease();
            ReleaseModalTextPanel();
        }
        _unlink("j.cal");
        return;
    }

    file = _open("j.cal", 0x8301, 0x180);
    if (file == -1)
        return;
    failed = _write(file, &device, 2) != 2;
    if (failed == 0)
        failed = _write(file, calibration, sizeof(calibration)) !=
                 sizeof(calibration);
    _close(file);
    if (failed != 0)
        _unlink("j.cal");
}

/* Function start: 0x4106C0 */
void WaitForJoystickButtonRelease(void)
{
    short device;

    device = g_nActiveInputDevice_005a819c;
    if (device == -1)
        return;
    do {
        PumpWindowMessages();
        SampleJoystickDevice(g_aInputDeviceSamples_005a81f0, device, 0);
    } while (g_nActiveInputDevice_005a819c != -1 &&
             g_aInputDeviceSamples_005a81f0[device].buttons != 0);
}

/* Function start: 0x410700 */
void WaitForJoystickButtonPress(void)
{
    short device;

    device = g_nActiveInputDevice_005a819c;
    if (device == -1)
        return;
    do {
        PumpWindowMessages();
        SampleJoystickDevice(g_aInputDeviceSamples_005a81f0, device, 0);
    } while (g_nActiveInputDevice_005a819c != -1 &&
             g_aInputDeviceSamples_005a81f0[device].buttons == 0);
}

/* Function start: 0x410740 */
unsigned int SetFleetOverviewView(int initializeCockpit)
{
    FixedVector centre;
    FixedVector offset;
    FixedVector orientation;
    int shipCount;
    int maximumRange;
    int playerRange;
    int cameraDistance;
    int range;
    short object;

    orientation.x = 0xff;
    orientation.y = 0xff;
    orientation.z = 0xff;
    shipCount = 0;
    if (initializeCockpit != 0) {
        GetScreenUpdateFlag();
        if ((int)((unsigned int (__cdecl *)(int))
                GetFixedOneMillionThunkAlt)(0) < 66000)
            initialize_cockpit(4);
        else
            initialize_cockpit(6);
        zero_vector(&g_aShipVelocity_0059c010[WC1_EYE_OBJECT]);
    }

    object = 0;
    do {
        if (g_aeObjectClass_0059d100[object] >= OBJECT_CLASS_SHIP)
            shipCount++;
        object++;
    } while (object < 10);

    if (shipCount < 2) {
        maximumRange = 0x4b000;
        playerRange = 0x4b000;
        position_relative_ijk(&centre, 0, 400, 400, 400);
    } else {
        zero_vector(&centre);
        object = 0;
        do {
            if (g_aeObjectClass_0059d100[object] >= OBJECT_CLASS_SHIP) {
                centre.x += g_aShipPosition_0059c490[object].x / shipCount;
                centre.y += g_aShipPosition_0059c490[object].y / shipCount;
                centre.z += g_aShipPosition_0059c490[object].z / shipCount;
            }
            object++;
        } while (object < 10);

        maximumRange = 0x4b000;
        playerRange = 0xff;
        object = 9;
        do {
            if (g_aeObjectClass_0059d100[object] >= OBJECT_CLASS_SHIP) {
                ComputeVectorDelta(&centre,
                                   &g_aShipPosition_0059c490[object],
                                   &offset);
                range = (int)Vector_magnitude(&offset);
                if (maximumRange < range)
                    maximumRange = range;
                if (object == 0)
                    playerRange = range;
            }
            object--;
        } while (object >= 0);
        if (maximumRange <= 0x1f4000)
            playerRange = maximumRange;
    }

    cameraDistance = (playerRange >> 3) * 9 + 0x2bc00;
    g_aShipPosition_0059c490[63] = centre;
    g_aShipUpVector_0059b9e0[63] = orientation;
    g_aShipForwardVector_0059bce0[63] = orientation;
    point_at(63, g_aShipPosition_0059c490[0]);

    ScaleFixedVector(&g_aShipRightVector_0059b6e0[63],
                     cameraDistance >> 2, &offset);
    AddFixedVectors(&g_aShipPosition_0059c490[63], &offset,
                    &g_aShipPosition_0059c490[63]);
    ScaleFixedVector(&g_aShipUpVector_0059b9e0[63], 0x9600, &offset);
    AddFixedVectors(&g_aShipPosition_0059c490[63], &offset,
                    &g_aShipPosition_0059c490[63]);
    ScaleFixedVector(&g_aShipForwardVector_0059bce0[63],
                     cameraDistance, &offset);
    AddFixedVectors(&g_aShipPosition_0059c490[63], &offset,
                    &g_aShipPosition_0059c490[63]);

    g_aShipPosition_0059c490[WC1_EYE_OBJECT] =
        g_aShipPosition_0059c490[63];
    g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT] = orientation;
    g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] = orientation;
    if (maximumRange < 0x271000)
        point_at(WC1_EYE_OBJECT, centre);
    else
        point_at(WC1_EYE_OBJECT, g_aShipPosition_0059c490[0]);
    return 0;
}

/* Function start: 0x410A30 */
void rotate_eye_to_goal(void)
{
    short totalError;

    totalError = (short)(AbsInt(g_anObjectPitchRotation_0059b2a0[61] -
                               g_nEyePitchGoal_0059d61c) +
                         AbsInt(g_anObjectYawRotation_0059ce80[61] -
                               g_nEyeYawGoal_0059c944) +
                         AbsInt(g_anObjectRollRotation_0059d7e0[61] -
                               g_nEyeRollGoal_0059c8f0));
    match_rotation_goal(&g_anObjectPitchRotation_0059b2a0[61],
                        &g_nEyePitchGoal_0059d61c, totalError,
                        g_nEyePitchRate_0046c004);
    match_rotation_goal(&g_anObjectYawRotation_0059ce80[61],
                        &g_nEyeYawGoal_0059c944, totalError,
                        g_nEyeYawRate_0046c008);
    match_rotation_goal(&g_anObjectRollRotation_0059d7e0[61],
                        &g_nEyeRollGoal_0059c8f0, totalError,
                        g_nEyeRollRate_0046c00c);
}

/* Function start: 0x410AD0 */
short GetVectorMagnitude(const FixedVector *vector)
{
    return FixedToShortSaturating((int)Vector_magnitude(vector));
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

    if (g_bScriptedView_0046a8d4 == 1)
        update_scripted_view();

    switch (DAT_0046c03c) {
    case 0:
        copy_frame(0, WC1_EYE_OBJECT);
        g_aShipVelocity_0059c010[WC1_EYE_OBJECT] =
            g_aShipVelocity_0059c010[0];
        g_aShipPosition_0059c490[WC1_EYE_OBJECT] =
            g_aShipPosition_0059c490[0];
        return 0;
    case 1:
        g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT] =
            g_aShipForwardVector_0059bce0[0];
        negate_vector(&g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT]);
        g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT] =
            g_aShipUpVector_0059b9e0[0];
        g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] =
            g_aShipRightVector_0059b6e0[0];
        g_aShipVelocity_0059c010[WC1_EYE_OBJECT] =
            g_aShipVelocity_0059c010[0];
        g_aShipPosition_0059c490[WC1_EYE_OBJECT] =
            g_aShipPosition_0059c490[0];
        return 0;
    case 2:
        g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT] =
            g_aShipForwardVector_0059bce0[0];
        g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT] =
            g_aShipUpVector_0059b9e0[0];
        g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] =
            g_aShipRightVector_0059b6e0[0];
        negate_vector(&g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT]);
        g_aShipVelocity_0059c010[WC1_EYE_OBJECT] =
            g_aShipVelocity_0059c010[0];
        g_aShipPosition_0059c490[WC1_EYE_OBJECT] =
            g_aShipPosition_0059c490[0];
        return 0;
    case 3:
        copy_frame(0, WC1_EYE_OBJECT);
        negate_vector(&g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT]);
        negate_vector(&g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT]);
        g_aShipVelocity_0059c010[WC1_EYE_OBJECT] =
            g_aShipVelocity_0059c010[0];
        g_aShipPosition_0059c490[WC1_EYE_OBJECT] =
            g_aShipPosition_0059c490[0];
        return 0;
    case 4:
        if (g_aeObjectClass_0059d100[g_cViewObject_0046c000] !=
            OBJECT_CLASS_NULL) {
            ComputeVectorDelta(
                &g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                &g_aShipPosition_0059c490[g_cViewObject_0046c000],
                &viewDirection);
            desiredPosition = g_aShipForwardVector_0059bce0[0];
            SetVectorFixedPoint(
                (unsigned int *)&desiredPosition,
                (short)(g_bAlternateChaseView_00468ffc == 0 ? -700 : -500));
            AddFixedVectors(
                &desiredPosition,
                &g_aShipPosition_0059c490[g_cViewObject_0046c000],
                &desiredPosition);
            ComputeVectorDelta(
                &g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                &desiredPosition, &positionDelta);
            vector = positionDelta;
            divide_vector(
                &vector,
                (g_bAlternateChaseView_00468ffc == 0 ? 25 : 7) << 8,
                &vector);
            g_aShipVelocity_0059c010[WC1_EYE_OBJECT] = vector;
            g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] = viewDirection;
            shrink_vector(
                &g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT]);
            fix_objects_ijk(WC1_EYE_OBJECT);
            g_nEyeRollGoal_0059c8f0 = match_roll_orientation(
                WC1_EYE_OBJECT, (short)g_cViewObject_0046c000);
            if (g_nEyeRollGoal_0059c8f0 != 0) {
                if (abs((int)g_nEyeRollGoal_0059c8f0) < 5) {
                    g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT] =
                        g_aShipUpVector_0059b9e0[0];
                    g_nEyeRollGoal_0059c8f0 = 0;
                } else {
                    g_nEyeRollRate_0046c00c = 4;
                    rotate_eye_to_goal();
                    rotate_object(WC1_EYE_OBJECT);
                }
            }
        }
        AddFixedVectors(&g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                        &g_aShipVelocity_0059c010[WC1_EYE_OBJECT],
                        &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
        return 0;
    case 5:
        zero_vector(&g_aShipVelocity_0059c010[WC1_EYE_OBJECT]);
        ComputeVectorDelta(
            &g_aShipPosition_0059c490[WC1_EYE_OBJECT],
            &g_aShipPosition_0059c490[g_cViewObject_0046c000], &vector);
        magnitude = (int)Vector_magnitude(&vector);
        if (magnitude < 0x7d001) {
            g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] = vector;
            fix_objects_ijk(WC1_EYE_OBJECT);
            return 0;
        }
        vector = g_aShipVelocity_0059c010[g_cViewObject_0046c000];
        NormalizeFixedVector(&vector);
        g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT] =
            g_aShipUpVector_0059b9e0[g_cViewObject_0046c000];
        if (vector.x == g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT].x &&
            vector.y == g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT].y &&
            vector.z == g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT].z) {
            g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT] =
                g_aShipRightVector_0059b6e0[g_cViewObject_0046c000];
        }
        ScaleFixedVector(&vector, -0x12430, &vector);
        AddFixedVectors(
            &g_aShipPosition_0059c490[g_cViewObject_0046c000], &vector,
            &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
        g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] = vector;
        fix_objects_ijk(WC1_EYE_OBJECT);
        ScaleFixedVector(&g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT],
                         0x19000, &vector);
        AddFixedVectors(&g_aShipPosition_0059c490[WC1_EYE_OBJECT], &vector,
                        &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
        ScaleFixedVector(&g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT],
                         0x6400, &vector);
        AddFixedVectors(&g_aShipPosition_0059c490[WC1_EYE_OBJECT], &vector,
                        &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
        ComputeVectorDelta(
            &g_aShipPosition_0059c490[WC1_EYE_OBJECT],
            &g_aShipPosition_0059c490[g_cViewObject_0046c000], &vector);
        g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] = vector;
        fix_objects_ijk(WC1_EYE_OBJECT);
        generate_stars();
        return 0;
    case 6:
        if (g_nExternalViewShip_0046c040 == -1) {
            zero_vector(&g_aShipVelocity_0059c010[WC1_EYE_OBJECT]);
            if (g_nExternalViewAngle_005a7f06++ > 20) {
                if (DAT_0046a008 == 0) {
                    new_view(0, 0);
                } else {
                    GetScreenUpdateFlag();
                    SetViewportRect(
                        &DAT_005a7510, 0, 0,
                        (unsigned short)(g_nScreenWidth_0046daa4 - 1),
                        (unsigned short)(g_nScreenHeight_0046daa8 - 1));
                    initialize_view_buffer();
                    new_view(0, 0);
                    GetScreenUpdateFlag();
                    SetViewportRect(&DAT_005a7510, 0, 0, 319, 199);
                    initialize_view_buffer();
                }
            }
        } else {
            ComputeVectorDelta(
                &g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                &g_aShipPosition_0059c490[g_nExternalViewShip_0046c040],
                &vector);
            if (g_aeShipTactic_0059d5e0[g_nExternalViewShip_0046c040] !=
                TACTIC_CRUISE) {
                magnitude = (int)Vector_magnitude(&vector);
                if (magnitude < 0xfa01) {
                    adjustment = vector;
                    NormalizeFixedVector(&adjustment);
                    ScaleFixedVector(&adjustment, -64000, &adjustment);
                    AddFixedVectors(
                        &vector, &adjustment,
                        &g_aShipVelocity_0059c010[WC1_EYE_OBJECT]);
                } else {
                    divide_vector(
                        &vector,
                        (int)(short)(g_nExternalViewDistance_005a6b5c &
                                     0xfffe) << 7,
                        &g_aShipVelocity_0059c010[WC1_EYE_OBJECT]);
                    g_nExternalViewDistance_005a6b5c = MaxShort(
                        (short)(g_nExternalViewDistance_005a6b5c - 1), 8);
                }
            }
            g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] = vector;
            fix_objects_ijk(WC1_EYE_OBJECT);
        }
        AddFixedVectors(&g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                        &g_aShipVelocity_0059c010[WC1_EYE_OBJECT],
                        &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
        return 0;
    case 7:
        if (g_acShipTarget_0059ce60[0] != -1) {
            ScaleFixedVector(&g_aShipRightVector_0059b6e0[0],
                             0x12c00, &vector);
            AddFixedVectors(&g_aShipPosition_0059c490[0], &vector,
                            &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
            ComputeVectorDelta(
                &g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                &g_aShipPosition_0059c490[
                    (short)g_acShipTarget_0059ce60[0]],
                &vector);
            NormalizeFixedVector(&vector);
            g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] = vector;
            ScaleFixedVector(&vector, -0x25800, &vector);
            AddFixedVectors(&g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                            &vector,
                            &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
            fix_objects_ijk(WC1_EYE_OBJECT);
            return 0;
        }
        if (DAT_0046a008 != 0) {
            GetScreenUpdateFlag();
            SetViewportRect(
                &DAT_005a7510, 0, 0,
                (unsigned short)(g_nScreenWidth_0046daa4 - 1),
                (unsigned short)(g_nScreenHeight_0046daa8 - 1));
            initialize_view_buffer();
            DAT_0046a008 = 0;
            new_view(0, 0);
            DAT_0046a008 = 1;
            GetScreenUpdateFlag();
            SetViewportRect(&DAT_005a7510, 0, 0, 319, 199);
            initialize_view_buffer();
            return 0;
        }
        new_view(0, 0);
        return 0;
    case 8:
        ScaleFixedVector(&g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT],
                         g_nCapitalShipViewDistance_00468ff4, &vector);
        SubtractFixedVectors(
            &g_aShipPosition_0059c490[g_cViewObject_0046c000], &vector,
            &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
        if (Vector_magnitude(
                &g_aShipVelocity_0059c010[WC1_EYE_OBJECT]) != 0) {
            AddFixedVectors(&g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                            &g_aShipVelocity_0059c010[WC1_EYE_OBJECT],
                            &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
            return 0;
        }
        break;
    case 9:
        copy_frame(0, WC1_EYE_OBJECT);
        negate_vector(&g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT]);
        negate_vector(&g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT]);
        g_aShipVelocity_0059c010[WC1_EYE_OBJECT] =
            g_aShipVelocity_0059c010[0];
        g_aShipPosition_0059c490[WC1_EYE_OBJECT] =
            g_aShipPosition_0059c490[0];
        return 0;
    case 10:
        copy_frame((short)g_cViewObject_0046c000, WC1_EYE_OBJECT);
        g_aShipVelocity_0059c010[WC1_EYE_OBJECT] =
            g_aShipVelocity_0059c010[g_cViewObject_0046c000];
        g_aShipPosition_0059c490[WC1_EYE_OBJECT] =
            g_aShipPosition_0059c490[g_cViewObject_0046c000];
        return 0;
    case 11:
        ComputeVectorDelta(
            &g_aShipPosition_0059c490[WC1_EYE_OBJECT],
            &g_aShipPosition_0059c490[g_cViewObject_0046c000], &vector);
        magnitude = (int)Vector_magnitude(&vector);
        if (magnitude < 0x25800) {
            adjustment = vector;
            NormalizeFixedVector(&adjustment);
            ScaleFixedVector(&adjustment, -0x25800, &adjustment);
            AddFixedVectors(&vector, &adjustment, &adjustment);
            AddFixedVectors(&g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                            &adjustment,
                            &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
        }
        g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] = vector;
        fix_objects_ijk(WC1_EYE_OBJECT);
        AddFixedVectors(&g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                        &g_aShipVelocity_0059c010[WC1_EYE_OBJECT],
                        &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
        return 0;
    case 12:
        ScaleFixedVector(&g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT],
                         -0xa00, &vector);
        AddFixedVectors(&g_aShipPosition_0059c490[WC1_EYE_OBJECT], &vector,
                        &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
        AddFixedVectors(&g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                        &g_aShipVelocity_0059c010[WC1_EYE_OBJECT],
                        &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
        look_at(0);
        return 0;
    case 13:
        AddFixedVectors(&g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                        &g_aShipVelocity_0059c010[WC1_EYE_OBJECT],
                        &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
        look_at(0);
        return 0;
    case 14:
        SetFleetOverviewView(0);
        return 0;
    case 15:
        AddFixedVectors(&g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                        &g_aShipVelocity_0059c010[WC1_EYE_OBJECT],
                        &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
        rotate_object(WC1_EYE_OBJECT);
        rotate_eye_to_goal();
        break;
    }
    return 0;
}

/* Function start: 0x4117B0 */
unsigned int force_view(int view, short obj)
{
    DAT_0046c03c = -1;
    return new_view(view, obj);
}

/* Function start: 0x4117D0 */
unsigned int new_view(int view, short obj)
{
    FixedVector direction;
    FixedVector offset;
    short carrier;

    if (DAT_0046c03c == view) {
        if (view == 4)
            g_bAlternateChaseView_00468ffc =
                1 - g_bAlternateChaseView_00468ffc;
        return 0;
    }
    if (view == 6 && g_nExternalViewShip_0046c040 == -1)
        return 0;

    g_cViewObject_0046c000 = (signed char)obj;
    if (view == 4 &&
        g_aeObjectClass_0059d100[g_cViewObject_0046c000] ==
            OBJECT_CLASS_CAPITAL_SHIP)
        view = 8;
    DAT_0046c03c = view;
    if (obj == -1)
        g_asObjectCollisionRadius_0059d710[WC1_EYE_OBJECT] = 10;
    else
        g_asObjectCollisionRadius_0059d710[WC1_EYE_OBJECT] =
            MaxShort(10, g_asObjectCollisionRadius_0059d710[obj]);
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
        if (g_bScriptedView_0046a8d4 == 0) {
            ScaleFixedVector(
                             &g_aShipForwardVector_0059bce0[
                                 g_cViewObject_0046c000],
                             -1200 << 8, &offset);
            AddFixedVectors(
                            &g_aShipPosition_0059c490[
                                g_cViewObject_0046c000],
                            &offset,
                            &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
            g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT] =
                g_aShipUpVector_0059b9e0[g_cViewObject_0046c000];
            g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] = offset;
            fix_objects_ijk(WC1_EYE_OBJECT);
            zero_vector(&g_aShipVelocity_0059c010[WC1_EYE_OBJECT]);
        }
        break;
    case 5:
        initialize_cockpit(4);
        direction = g_aShipVelocity_0059c010[g_cViewObject_0046c000];
        if (NormalizeFixedVector(&direction) == 0)
            direction =
                g_aShipForwardVector_0059bce0[g_cViewObject_0046c000];
        g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT] =
            g_aShipUpVector_0059b9e0[g_cViewObject_0046c000];
        if (equ_vector(&direction,
                       &g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT])) {
            g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT] =
                g_aShipRightVector_0059b6e0[g_cViewObject_0046c000];
        }
        ScaleFixedVector(&direction, -0x12430, &direction);
        AddFixedVectors(
            &g_aShipPosition_0059c490[g_cViewObject_0046c000],
            &direction, &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
        g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] = direction;
        fix_objects_ijk(WC1_EYE_OBJECT);
        zero_vector(&g_aShipVelocity_0059c010[WC1_EYE_OBJECT]);
        ScaleFixedVector(&g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT],
                         0x19000, &offset);
        AddFixedVectors(&g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                        &offset,
                        &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
        ScaleFixedVector(&g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT],
                         0x6400, &offset);
        AddFixedVectors(&g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                        &offset,
                        &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
        ComputeVectorDelta(
            &g_aShipPosition_0059c490[WC1_EYE_OBJECT],
            &g_aShipPosition_0059c490[g_cViewObject_0046c000],
            &direction);
        g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] = direction;
        fix_objects_ijk(WC1_EYE_OBJECT);
        break;
    case 6:
        initialize_cockpit(4);
        ScaleFixedVector(
            &g_aShipRightVector_0059b6e0[g_nExternalViewShip_0046c040],
            0x25800, &offset);
        AddFixedVectors(
            &g_aShipPosition_0059c490[g_nExternalViewShip_0046c040],
            &offset, &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
        ComputeVectorDelta(
            &g_aShipPosition_0059c490[WC1_EYE_OBJECT],
            &g_aShipPosition_0059c490[g_nExternalViewShip_0046c040],
            &direction);
        g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT] =
            g_aShipUpVector_0059b9e0[0];
        g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] = direction;
        fix_objects_ijk(WC1_EYE_OBJECT);
        zero_vector(&g_aShipVelocity_0059c010[WC1_EYE_OBJECT]);
        g_nExternalViewDistance_005a6b5c = 0x20;
        g_nExternalViewAngle_005a7f06 = 0;
        break;
    case 7:
        initialize_cockpit(4);
        copy_frame(0, WC1_EYE_OBJECT);
        zero_vector(&g_aShipVelocity_0059c010[WC1_EYE_OBJECT]);
        break;
    case 8:
        initialize_cockpit(4);
        g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT] =
            g_aShipRightVector_0059b6e0[g_cViewObject_0046c000];
        g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT] =
            g_aShipForwardVector_0059bce0[g_cViewObject_0046c000];
        negate_vector(&g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT]);
        g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] =
            g_aShipUpVector_0059b9e0[g_cViewObject_0046c000];
        fix_objects_ijk(WC1_EYE_OBJECT);
        zero_vector(&g_aShipVelocity_0059c010[WC1_EYE_OBJECT]);
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
        g_nEyePitchGoal_0059d61c = 0;
        g_nEyeYawGoal_0059c944 = 0;
        g_nEyeRollGoal_0059c8f0 = 0;
        g_nEyePitchRate_0046c004 = 1;
        g_nEyeYawRate_0046c008 = 1;
        g_nEyeRollRate_0046c00c = 1;
        break;
    case 12:
        initialize_cockpit(4);
        copy_frame(0, WC1_EYE_OBJECT);
        position_relative_ijk(&g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                              0, 500, 0, 2000);
        zero_vector(&g_aShipVelocity_0059c010[WC1_EYE_OBJECT]);
        look_at(0);
        break;
    case 13:
        initialize_cockpit(4);
        zero_vector(&g_aShipVelocity_0059c010[0]);
        if (g_nYourWingman_0046c04c != -1)
            zero_vector(
                &g_aShipVelocity_0059c010[g_nYourWingman_0046c04c]);
        copy_frame(g_cViewObject_0046c000, WC1_EYE_OBJECT);
        carrier = find_ship_index(g_nCarrierMissionShipIndex_005a7e2a);
        look_at(carrier);
        position_relative_ijk(
            &g_aShipPosition_0059c490[WC1_EYE_OBJECT],
            WC1_EYE_OBJECT, 0, -10, -400);
        g_aShipVelocity_0059c010[WC1_EYE_OBJECT] =
            g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT];
        ScaleFixedVector(&g_aShipVelocity_0059c010[WC1_EYE_OBJECT],
                         -0x2300,
                         &g_aShipVelocity_0059c010[WC1_EYE_OBJECT]);
        break;
    case 14:
        SetFleetOverviewView(1);
        break;
    }

    if (g_bScriptedView_0046a8d4 == 1) {
        g_bScriptedView_0046a8d4 = 0;
        set_eye_direction_and_position();
        g_bScriptedView_0046a8d4 = 1;
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
    ScaleFixedVector(&g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT],
                     (int)forwardDistance << 8, &offset);
    AddFixedVectors(&origin, &offset, &origin);
    ScaleFixedVector(&g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT],
                     rightOffset, &offset);
    AddFixedVectors(&origin, &offset, &origin);
    ScaleFixedVector(&g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT],
                     upOffset, &offset);
    AddFixedVectors(&origin, &offset, &origin);
    AddFixedVectors(&origin,
                    &g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                    &g_aShipPosition_0059c490[obj]);
    FillFixedVectorWithRandomComponents(2, &g_aShipVelocity_0059c010[obj]);
    streak = MaxShort((short)(1 - RandomInRange(0, 3)), 0);
    g_asObjectScreenAngle_0059cd90[obj] =
        (short)(streak * 0x10 + RandomInRange(0, 3));
    return 0;
}

/* Function start: 0x411FE0 */
unsigned int generate_stars(void)
{
    FixedVector origin;
    short distance;
    short pitch;
    short yaw;
    short obj;

    zero_vector(&origin);
    obj = 34;
    do {
        if (obj < 42) {
            distance = RandomInRange(0, 1400);
            start_dust(obj, origin, distance,
                       (int)signed_random(distance) << 8,
                       (int)signed_random(distance) << 8);
        } else {
            g_aeObjectClass_0059d100[obj] = OBJECT_CLASS_STAR;
            pitch = signed_random(45);
            yaw = signed_random(45);
            copy_frame(WC1_EYE_OBJECT, 63);
            alter_yaw(yaw, 63);
            alter_pitch(pitch, 63);
            ScaleFixedVector(&g_aShipForwardVector_0059bce0[63],
                             15000 << 8,
                             &g_aShipPosition_0059c490[obj]);
            g_asObjectViewFrame_0059d230[obj] =
                (short)(RandomInRange(0, 5) + 32);
            g_asObjectScreenScale_0059c950[obj] = 0xff;
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
    short distance;
    short randomChoice;
    short pitch;
    short yaw;
    short obj;

    g_vPreviousStarFieldMotion_0059c900 = g_vStarFieldMotion_0059c860;
    ScaleFixedVector(&g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT],
                     200 << 8, &cameraMotion);
    ScaleFixedVector(&g_aShipVelocity_0059c010[WC1_EYE_OBJECT],
                     20 << 8, &g_vStarFieldMotion_0059c860);
    AddFixedVectors(&cameraMotion, &g_vStarFieldMotion_0059c860,
                    &g_vStarFieldMotion_0059c860);
    ComputeVectorDelta(&g_vPreviousStarFieldMotion_0059c900,
                       &g_vStarFieldMotion_0059c860, &origin);
    obj = 34;
    do {
        if (g_asObjectScreenX_0059d9b0[obj] == (short)0x8001) {
            randomChoice = RandomInRange(0, 7);
            if (g_pActiveHazardField_0059bfe0 == 0) {
                if (g_aeObjectClass_0059d100[obj] == OBJECT_CLASS_ASTEROID ||
                    (int)g_aeObjectClass_0059d100[obj] == 0x21 ||
                    g_aeObjectClass_0059d100[obj] == OBJECT_CLASS_NULL) {
                    set_objects_data(obj, OBJECT_TYPE_SPACE_DUST, -1);
                    randomChoice = 0;
                }
            } else if (obj < 42) {
                extra_hazard(obj);
            }
            if (g_aeObjectClass_0059d100[obj] == OBJECT_CLASS_STAR &&
                randomChoice == 0 &&
                (g_anObjectYawRotation_0059ce80[0] != 0 ||
                 g_anObjectPitchRotation_0059b2a0[0] != 0)) {
                copy_frame(WC1_EYE_OBJECT, 63);
                if (g_anObjectPitchRotation_0059b2a0[0] != 0) {
                    pitch = g_anObjectPitchRotation_0059b2a0[0] < 0 ?
                        -45 : 45;
                    yaw = signed_random(45);
                } else {
                    pitch = signed_random(45);
                    yaw = 0;
                }
                if (g_anObjectYawRotation_0059ce80[0] != 0 &&
                    (g_anObjectPitchRotation_0059b2a0[0] == 0 ||
                     RandomInRange(0, 1) != 0)) {
                    yaw = g_anObjectYawRotation_0059ce80[0] < 0 ? -45 : 45;
                    pitch = signed_random(45);
                }
                alter_pitch(pitch, 63);
                alter_yaw(yaw, 63);
                ScaleFixedVector(&g_aShipForwardVector_0059bce0[63],
                                 15000 << 8,
                                 &g_aShipPosition_0059c490[obj]);
                g_asObjectViewFrame_0059d230[obj] =
                    (short)(RandomInRange(0, 5) + 32);
                break;
            }
            if (g_aeObjectClass_0059d100[obj] == OBJECT_CLASS_DUST &&
                randomChoice < 2) {
                transform_to_objects_frame(
                    &g_aShipVelocity_0059c010[WC1_EYE_OBJECT],
                    &viewMotion, WC1_EYE_OBJECT);
                ScaleFixedVector(&viewMotion, 10 << 8, &viewMotion);
                if (viewMotion.z < 0) {
                    distance = (short)((RandomInRange(0, 40) +
                        g_asObjectCollisionRadius_0059d710[
                            WC1_EYE_OBJECT]) >> 1);
                } else {
                    distance = (short)(RandomInRange(
                        0, (short)(viewMotion.z >> 8)) +
                        g_asObjectCollisionRadius_0059d710[
                            WC1_EYE_OBJECT] + RandomInRange(0, 350) / 2);
                }
                start_dust(obj, origin, distance,
                           (int)signed_random(distance) << 8,
                           (int)signed_random(distance) << 8);
                break;
            }
        }
        obj++;
    } while (obj < 49);
    if (g_pActiveHazardField_0059bfe0 != 0)
        update_hazards();
    return 0;
}

/* Function start: 0x412410 */
unsigned short count_down(short obj)
{
    if (g_asObjectCounter_0059c330[obj] != -1)
        g_asObjectCounter_0059c330[obj]--;
    return (unsigned short)g_asObjectCounter_0059c330[obj];
}

/* Function start: 0x412430 */
unsigned int house_keep_objects(void)
{
    short obj;

    obj = 0;

    do {
        switch (g_aeObjectClass_0059d100[obj]) {
        case OBJECT_CLASS_DUST:
            if (g_aeObjectType_0059b560[obj] == OBJECT_TYPE_DEBRIS_DUST &&
                (short)count_down(obj) == -1 &&
                g_asObjectScreenX_0059d9b0[obj] == (short)0x8001)
                remove_object(obj);
            break;
        case OBJECT_CLASS_DEBRIS:
            if ((short)count_down(obj) == -1)
                remove_object(obj);
            break;
        case OBJECT_CLASS_FIXED_OBJECT:
            if (g_aeObjectType_0059b560[obj] == OBJECT_TYPE_TURRET ||
                g_aeObjectType_0059b560[obj] == OBJECT_TYPE_THRUSTERS)
                remove_object(obj);
            break;
        case OBJECT_CLASS_PROJECTILE:
            if ((short)count_down(obj) == 0) {
                if (g_aeObjectType_0059b560[obj] == OBJECT_TYPE_TURRET)
                    explode((short)g_acObjectOwner_0059ce20[obj], obj);
                else
                    remove_object(obj);
            }
            break;
        case OBJECT_CLASS_MINE:
            if (g_acObjectCollisionGraceTicks_0059ddb0[obj] > 0)
                g_acObjectCollisionGraceTicks_0059ddb0[obj]--;
            if ((short)count_down(obj) == 0)
                explode(obj, obj);
            break;
        case OBJECT_CLASS_MISSILE:
            g_abShipExhaustHeat_0059d610[obj] = 0;
            if (g_acObjectCollisionGraceTicks_0059ddb0[obj] > 0)
                g_acObjectCollisionGraceTicks_0059ddb0[obj]--;
            if (g_aeShipTactic_0059d5e0[obj] == TACTIC_SIT_STILL) {
                if ((short)count_down(obj) <= 0) {
                    g_aeShipTactic_0059d5e0[obj] = TACTIC_RAM;
                    g_asObjectCounter_0059c330[obj] =
                        g_aObjectTypeData_00466458[
                            g_aeObjectType_0059b560[obj]].lifetime;
                    if (g_aeObjectType_0059b560[obj] ==
                            OBJECT_TYPE_DUMB_FIRE_MISSILE) {
                        vector_component_in_dir(
                            &g_aShipVelocity_0059c010[obj],
                            &g_aShipForwardVector_0059bce0[obj],
                            &g_aShipVelocity_0059c010[obj]);
                    }
                }
            } else if ((short)count_down(obj) <= 0) {
                explode(obj, obj);
            }
            break;
        case OBJECT_CLASS_SHIP:
        case OBJECT_CLASS_CAPITAL_SHIP:
            g_abShipExhaustHeat_0059d610[obj] = 0;
            if ((short)count_down(obj) <= 0) {
                if (g_asObjectCounter_0059c330[obj] == 0 &&
                    g_aeSpecialManeuver_0059c3c0[obj] ==
                        SPECIAL_MANEUVER_UNKNOWN_9) {
                    if (g_nYourWingman_0046c04c != -1 &&
                        DAT_0059c910[obj] == 0 &&
                        g_aeShipSide_0059d650[obj] == SIDE_KILRATHI &&
                        (short)RandomBelowOrEqual(100) < 10) {
                        send_message(g_nYourWingman_0046c04c, 6);
                    }
                    Create_explosion_debris(obj);
                    break;
                }
                if (g_asObjectCounter_0059c330[obj] == 0) {
                    if (g_aeShipManeuver_0059dcb0[obj] ==
                            MANEUVER_WARPING_IN) {
                        if (g_aeShipTactic_0059d5e0[obj] !=
                                TACTIC_WARP_IN) {
                            if ((short)g_acObjectOwner_0059ce20[obj] == obj) {
                                set_objects_data(
                                    obj,
                                    (enum ObjectType)
                                        g_abShipNavPointIndex_0059d7c0[obj],
                                    -1);
                                reset_maneuver(obj, -1);
                            } else {
                                remove_object(obj);
                            }
                        }
                    } else if (g_aeShipManeuver_0059dcb0[obj] ==
                                   MANEUVER_WARPING_OUT &&
                               g_aeShipSide_0059d650[obj] != SIDE_NEUTRAL) {
                        g_aMissionShips_0046c948[
                            g_nShipMissionIndices_0059c830[obj]].state = 2;
                        remove_object(obj);
                    }
                }
            } else if (g_aeSpecialManeuver_0059c3c0[obj] ==
                           SPECIAL_MANEUVER_UNKNOWN_9 &&
                       g_aeObjectClass_0059d100[obj] ==
                           OBJECT_CLASS_CAPITAL_SHIP) {
                if (g_aeShipManeuver_0059dcb0[obj] ==
                        MANEUVER_WARPING_OUT)
                    g_asObjectScale_0059de40[obj] >>= 1;
                if (g_asObjectCounter_0059c330[obj] == 7) {
                    ShipExplosion(obj);
                    explosion_shock_wave(
                        obj, g_aObjectTypeData_00466458[
                            g_aeObjectType_0059b560[obj]].fuelCapacity);
                } else {
                    while ((unsigned short)RandomInRange(0, 100) < 50)
                        onboard_explosion(obj);
                }
            } else if (g_aeShipManeuver_0059dcb0[obj] ==
                           MANEUVER_WARPING_OUT) {
                g_asObjectScale_0059de40[obj] >>= 1;
            }
            if (g_aeObjectType_0059b560[obj] ==
                    OBJECT_TYPE_TIGERS_CLAW &&
                g_bPlayerCollisionsEnabled_00469ff8 != 0 &&
                g_bLandingAuthorized_00468ff8 != 0 &&
                normal_speed(0) != 0) {
                get_facing_range_from_object(0, obj);
                if (g_nTargetRange_0059ce10 < 700 &&
                    g_nFacingToTarget_0059d920 > 75 &&
                    g_nTargetFacing_0059d52a > 70) {
                    g_nArcadeState_00469fb0 = 1;
                    g_nPlayerCollisionObject_0046c050 = obj;
                }
            }
            break;
        }
        obj++;
    } while (obj <= WC1_SPACE_LAST_MOVING_OBJECT);
    return 0;
}

/* Function start: 0x412820 */
void update_objects_in_space(void)
{
    short obj;

    clear_crash_cache();
    obj = 0;
    do {
        if (g_aeObjectClass_0059d100[obj] == OBJECT_CLASS_FUTURION) {
            futurion_intelligence(obj);
        } else if (g_aeObjectClass_0059d100[obj] > OBJECT_CLASS_PLANET) {
            animate_object(obj);
            object_collision(obj);
            if (g_aeObjectClass_0059d100[obj] != OBJECT_CLASS_NULL &&
                g_aeObjectClass_0059d100[obj] >= OBJECT_CLASS_PROJECTILE) {
                rotate_object(obj);
                if (obj >= 10 ||
                    g_aeSpecialManeuver_0059c3c0[obj] !=
                        SPECIAL_MANEUVER_UNKNOWN_9) {
                    if (obj != 0)
                        object_intelligence(obj);
                    if (obj < 10 &&
                        g_aeObjectClass_0059d100[obj] >=
                            OBJECT_CLASS_MISSILE) {
                        if (obj != 0)
                            rotate_object_to_goal(obj);
                        if (g_aeObjectClass_0059d100[obj] ==
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
        if (g_aeObjectClass_0059d100[obj] > OBJECT_CLASS_PLANET) {
            accelerate_and_move_object(obj);
            if (g_aeObjectClass_0059d100[obj] >= OBJECT_CLASS_SHIP) {
                replenish_shields(obj);
                housekeep_power_plant_and_fuel(obj);
            }
        }
        obj++;
    } while (obj <= WC1_SPACE_LAST_MOVING_OBJECT);
}

/* Function start: 0x412920 */
void rotate_object(short obj)
{
    if (g_anObjectPitchRotation_0059b2a0[obj] != 0) {
        alter_pitch(g_anObjectPitchRotation_0059b2a0[obj], obj);
        ClampVectorTo30(&g_anObjectPitchRotation_0059b2a0[obj]);
    }
    if (g_anObjectYawRotation_0059ce80[obj] != 0) {
        alter_yaw(g_anObjectYawRotation_0059ce80[obj], obj);
        ClampVectorTo30(&g_anObjectYawRotation_0059ce80[obj]);
    }
    if (g_anObjectRollRotation_0059d7e0[obj] != 0) {
        alter_roll(g_anObjectRollRotation_0059d7e0[obj], obj);
        ClampVectorTo30(&g_anObjectRollRotation_0059d7e0[obj]);
    }
}

/* Function start: 0x4129A0 */
unsigned int accelerate_and_move_object(short obj)
{
    FixedVector delta;
    FixedVector accelerationVector;
    int acceleration;
    int magnitude;

    if (g_aeObjectClass_0059d100[obj] >= OBJECT_CLASS_MISSILE) {
        if (g_aeSpecialManeuver_0059c3c0[obj] ==
                SPECIAL_MANEUVER_KILL_ENGINES) {
            g_abShipExhaustHeat_0059d610[obj] = 0;
            if ((short)RandomBelowOrEqual(100) < 10)
                set_special(obj, SPECIAL_MANEUVER_NONE);
        } else if (g_aeSpecialManeuver_0059c3c0[obj] ==
                       SPECIAL_MANEUVER_STOP_DRIFT) {
            approach_zero_speed(obj);
            NormalizeAndScaleVector(&g_aShipVelocity_0059c010[obj],
                                    g_anShipSpeed_0059b320[obj]);
            if (g_anShipSpeed_0059b320[obj] == 0)
                set_special(obj, SPECIAL_MANEUVER_NONE);
        }
        if (g_aeSpecialManeuver_0059c3c0[obj] <
                SPECIAL_MANEUVER_KILL_ENGINES &&
            g_aeShipTactic_0059d5e0[obj] != TACTIC_SIT_STILL) {
            zero_vector(&accelerationVector);
            if (g_aeSpecialManeuver_0059c3c0[obj] ==
                    SPECIAL_MANEUVER_AFTERBURNER) {
                g_asShipAfterburnerTimer_0059c810[obj]--;
                if (g_asShipAfterburnerTimer_0059c810[obj] == 0) {
                    g_aeSpecialManeuver_0059c3c0[obj] =
                        SPECIAL_MANEUVER_NONE;
                    if (DAT_005a7cec != 0) {
                        FlushSoundEffectsAndLog();
                        DAT_005a7cec = 0;
                    }
                    ScaleFixedVector(&g_aShipForwardVector_0059bce0[obj],
                                     g_anShipSpeed_0059b320[obj], &delta);
                } else {
                    ScaleFixedVector(
                        &g_aShipForwardVector_0059bce0[obj],
                        (g_aObjectTypeData_00466458[
                            g_aeObjectType_0059b560[obj]].maximumVelocity +
                         20) * 0x200,
                        &delta);
                    drain_fuel(obj, 200);
                    g_abShipExhaustHeat_0059d610[obj] = 3;
                }
            } else if (g_aeSpecialManeuver_0059c3c0[obj] ==
                           SPECIAL_MANEUVER_SUPER_BRAKE) {
                g_asShipAfterburnerTimer_0059c810[obj]--;
                if (g_asShipAfterburnerTimer_0059c810[obj] == 0) {
                    g_aeSpecialManeuver_0059c3c0[obj] =
                        SPECIAL_MANEUVER_NONE;
                    ScaleFixedVector(&g_aShipForwardVector_0059bce0[obj],
                                     g_anShipSpeed_0059b320[obj], &delta);
                } else {
                    zero_vector(&delta);
                    drain_fuel(obj, 140);
                }
            } else {
                ScaleFixedVector(&g_aShipForwardVector_0059bce0[obj],
                                 g_anShipSpeed_0059b320[obj], &delta);
            }
            ComputeVectorDelta(&g_aShipVelocity_0059c010[obj], &delta,
                               &delta);
            magnitude = (int)Vector_magnitude(&delta);
            if (magnitude > 0) {
                acceleration = GetShipAccelerationRate(obj);
                if (alert_flag(obj, 1) != 0 && acceleration < 0x500)
                    acceleration = 0x500;
                if (g_aeSpecialManeuver_0059c3c0[obj] ==
                        SPECIAL_MANEUVER_AFTERBURNER ||
                    g_aeSpecialManeuver_0059c3c0[obj] ==
                        SPECIAL_MANEUVER_SUPER_BRAKE) {
                    acceleration *= 2;
                }
                acceleration = (int)MultiplyFixed(
                    acceleration,
                    (int)DivideFixed(
                        dot_product(
                            &delta,
                            &g_aShipForwardVector_0059bce0[obj]),
                        magnitude) + 0x200);
                ScaleFixedVector(
                    &delta,
                    MinInt((int)DivideFixed(acceleration >> 1, magnitude),
                           0x100),
                    &accelerationVector);
                if (g_aeSpecialManeuver_0059c3c0[obj] !=
                        SPECIAL_MANEUVER_AFTERBURNER) {
                    g_abShipExhaustHeat_0059d610[obj] = 2;
                }
            }
            AddFixedVectors(&accelerationVector,
                            &g_aShipVelocity_0059c010[obj],
                            &g_aShipVelocity_0059c010[obj]);
            if (obj == 0)
                g_vPlayerAcceleration_0059b460 = accelerationVector;
        }
    }
    AddFixedVectors(&g_aShipPosition_0059c490[obj],
                    &g_aShipVelocity_0059c010[obj],
                    &g_aShipPosition_0059c490[obj]);
    return 0;
}

/* Function start: 0x412CD0 */
unsigned int animate_shape(short obj)
{
    unsigned char *animation;
    enum ObjectType type;
    short command;

    type = g_aeObjectType_0059b560[obj];
    animation = g_aObjectTypeData_00466458[type].animation;
    if (animation == 0)
        return 0;
    if (--g_asObjectAnimationDelay_0059b660[obj] > 0)
        return 0;
    g_asObjectAnimationDelay_0059b660[obj] =
        g_aObjectTypeData_00466458[type].yawRate;
    command = *(unsigned short *)(animation +
              g_asObjectAnimationIndex_0059da30[obj] * 4);
    switch (command & 0xf000) {
    case 0x9000:
        command &= 0x0fff;
        g_asObjectAnimationIndex_0059da30[obj] = command;
        command = *(unsigned short *)(animation + command * 4);
        if (g_asObjectScreenX_0059d9b0[obj] != (short)0x8001 &&
            (type == OBJECT_TYPE_DEBRIS_WING ||
             type == OBJECT_TYPE_DEBRIS_METAL_SHEET)) {
            PlaySfxWaveFileByNumber(13, obj, 0);
        }
        break;
    case 0xa000:
        remove_object(obj);
        return 0;
    }

    if ((command & 0x0c00) == 0x0400)
        goto grow_object;
    if ((command & 0x0c00) == 0x0800)
        goto shrink_object;
    command &= 0x3f;
    g_asObjectViewFrame_0059d230[obj] = command;

finish_command:
    g_asObjectFlip_0059c870[obj] = (command & 0xc0) >> 2;
    g_asObjectAnimationIndex_0059da30[obj]++;
    return 0;

grow_object:
    g_asObjectScale_0059de40[obj] +=
        (command & 0x3f) * (g_asObjectScale_0059de40[obj] >> 6);
    goto finish_command;

shrink_object:
    g_asObjectScale_0059de40[obj] -=
        (command & 0x3f) * (g_asObjectScale_0059de40[obj] >> 6);
    goto finish_command;
}

/* Function start: 0x412E30 */
unsigned int animate_object(short obj)
{
    FixedVector offset;
    short effect;

    switch (g_aeObjectClass_0059d100[obj]) {
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
        if (g_asObjectScreenX_0059d9b0[obj] == (short)0x8001 ||
            (g_nRenderedSpaceFrame_0059d61a & 3) != 0 ||
            (g_aObjectTypeData_00466458[
                 g_aeObjectType_0059b560[obj]].damageCapacity >> 1) - 1 >
                g_asShipAccumulatedDamage_0059dee0[obj]) {
            break;
        }
        effect = find_vacant_3d_object();
        if (effect == -1)
            break;
        ScaleFixedVector(&g_aShipForwardVector_0059bce0[obj],
                         -((int)g_asObjectCollisionRadius_0059d710[obj]
                             << 8),
                         &offset);
        AddFixedVectors(&g_aShipPosition_0059c490[obj], &offset,
                        &g_aShipPosition_0059c490[effect]);
        FillFixedVectorWithRandomComponents(20, &offset);
        AddFixedVectors(&g_aShipPosition_0059c490[effect], &offset,
                        &g_aShipPosition_0059c490[effect]);
        zero_vector(&g_aShipVelocity_0059c010[effect]);
        set_objects_data(
            effect,
            (enum ObjectType)(RandomInRange(0, 2) + OBJECT_TYPE_RED_SPARK),
            obj);
        g_asObjectScale_0059de40[effect] =
            g_asObjectScale_0059de40[obj];
        if ((short)RandomInRange(0, 3) == 0)
            PlaySfxWaveFileByNumber(7, obj, 0);
        break;
    }
    return 0;
}

/* Function start: 0x413030 */
unsigned int hit_asteroid(short asteroid, short destructionChance)
{
    short fragments;

    if ((short)RandomBelowOrEqual((short)(destructionChance - 1)) == 0) {
        fragments = (short)(RandomBelowOrEqual(1) + 2);
        while (fragments > 0) {
            make_shard(asteroid, g_aShipVelocity_0059c010[asteroid]);
            fragments--;
        }
        explode(-1, asteroid);
    } else if ((short)RandomBelowOrEqual(7) == 0) {
        make_shard(asteroid, g_vCollisionDelta_0059d690);
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
        g_acLastCollisionObject_0059d6a0[obj] = -1;
        return 0;
    }
    if (DAT_0046a000 == 0 && (obj == 0 || partner == 0))
        return 0;

    NormalizeFixedVector(&g_vCollisionDelta_0059d690);
    ComputeVectorDelta(&g_aShipVelocity_0059c010[partner],
                       &g_aShipVelocity_0059c010[obj],
                       &relativeVelocity);
    NormalizeFixedVector(&relativeVelocity);
    owner = g_acObjectOwner_0059ce20[obj];
    switch (g_aeObjectClass_0059d100[obj]) {
    case OBJECT_CLASS_PROJECTILE:
        if (owner == partner)
            break;
        partnerClass = g_aeObjectClass_0059d100[partner];
        if (partnerClass > OBJECT_CLASS_MINE) {
            if (partner == 0) {
                rectangular_to_spherical(
                    &g_aObjectViewPosition_0059afa0[obj], &impact);
                if (abs(impact.pitch) < 45) {
                    if (abs(impact.yaw) < 45)
                        g_aPaletteFadeEntries_005a76d0[1][0] = 0x38;
                    else if (abs(impact.yaw) < 136) {
                        if (impact.yaw < 0)
                            g_aPaletteFadeEntries_005a76d0[3][0] = 0x38;
                        else
                            g_aPaletteFadeEntries_005a76d0[5][0] = 0x38;
                    }
                    else
                        g_aPaletteFadeEntries_005a76d0[0][0] = 0x38;
                } else if (impact.pitch < 0)
                    g_aPaletteFadeEntries_005a76d0[2][0] = 0x38;
                else
                    g_aPaletteFadeEntries_005a76d0[4][0] = 0x38;
            }
            DAT_0059c910[partner] = owner;
            g_acShipAiCooldown_0059d680[partner] += 4;
            damage = (short)(g_asShipAccumulatedDamage_0059dee0[obj] -
                             g_asObjectCounter_0059c330[obj] / 2);
            force = g_aShipVelocity_0059c010[obj];
            NormalizeFixedVector(&force);
            ScaleFixedVector(&force, (int)damage << 8, &force);
            negate_vector(&g_vCollisionDelta_0059d690);
            apply_force_to_object(&g_vCollisionDelta_0059d690,
                                  &force, partner);
            inflict_damage(obj, partner, damage, &relativeVelocity);
        }
        savedScale = g_asObjectScale_0059de40[obj];
        set_objects_data(obj, OBJECT_TYPE_LASER_SPARK, owner);
        g_asObjectScale_0059de40[obj] = (short)(savedScale * 2);
        g_aShipVelocity_0059c010[obj] =
            g_aShipVelocity_0059c010[partner];
        if (partnerClass == OBJECT_CLASS_ASTEROID)
            hit_asteroid(partner, 3);
        break;

    case OBJECT_CLASS_ASTEROID:
        if (g_aeObjectClass_0059d100[partner] ==
                OBJECT_CLASS_ASTEROID) {
            if (g_asObjectScreenX_0059d9b0[obj] == (short)0x8001)
                remove_object(obj);
            else
                hit_asteroid(obj, 0);
        }
        break;

    case OBJECT_CLASS_MINE:
        if (owner == partner ||
            g_acObjectCollisionGraceTicks_0059ddb0[obj] > 0)
            return 0;
        if (easy2see(obj) == 0 &&
            (DAT_0046c03c == 0 || partner != 0)) {
            remove_object(obj);
            return 0;
        }
        explode(obj, obj);
        return 0;

    case OBJECT_CLASS_MISSILE:
        if (owner != partner ||
            g_acObjectCollisionGraceTicks_0059ddb0[obj] < 1) {
            ScaleFixedVector(&g_aShipVelocity_0059c010[obj],
                (unsigned short)g_asObjectRadarRadius_0059c790[obj]
                    << 8, &force);
            negate_vector(&g_vCollisionDelta_0059d690);
            apply_force_to_object(&g_vCollisionDelta_0059d690,
                                  &force, partner);
            explode(obj, obj);
            zero_vector(&g_aShipVelocity_0059c010[obj]);
            return 0;
        }
        break;

    case OBJECT_CLASS_SHIP:
    case OBJECT_CLASS_CAPITAL_SHIP:
        partnerClass = g_aeObjectClass_0059d100[partner];
        if ((partnerClass == OBJECT_CLASS_ASTEROID ||
             (partnerClass == OBJECT_CLASS_MINE &&
              g_acObjectOwner_0059ce20[partner] == -1)) &&
            easy2see(partner) == 0 &&
            (DAT_0046c03c == 0 || obj != 0)) {
            remove_object(partner);
        }
        partnerClass = g_aeObjectClass_0059d100[partner];
        if ((partnerClass == OBJECT_CLASS_ASTEROID ||
             partnerClass == OBJECT_CLASS_SHIP ||
             partnerClass == OBJECT_CLASS_CAPITAL_SHIP) &&
            g_acLastCollisionObject_0059d6a0[obj] != partner) {
            PlaySfxWaveFileByNumber(0x1c, obj, 0);
            g_acLastCollisionObject_0059d6a0[obj] =
                (signed char)partner;
            g_acLastCollisionObject_0059d6a0[partner] =
                (signed char)obj;

            separationScale = (int)DivideFixed(
                (g_asObjectCollisionRadius_0059d710[obj] +
                 g_asObjectCollisionRadius_0059d710[partner]) << 8,
                (int)Vector_magnitude(&g_vCollisionDelta_0059d690));
            separationScale = MinInt(separationScale, 0x7d000);
            ScaleFixedVector(&g_vCollisionDelta_0059d690,
                             separationScale, &separation);
            AddFixedVectors(&g_aShipPosition_0059c490[obj], &separation,
                            &g_aShipPosition_0059c490[partner]);

            vector_component_in_dir(
                &g_aShipVelocity_0059c010[obj],
                &g_vCollisionDelta_0059d690, &objectComponent);
            SubtractFixedVectors(&g_aShipVelocity_0059c010[obj],
                                 &objectComponent, &tangent);
            vector_component_in_dir(
                &g_aShipVelocity_0059c010[partner],
                &g_vCollisionDelta_0059d690, &partnerComponent);
            ComputeVectorDelta(&partnerComponent, &objectComponent,
                               &componentDelta);
            collisionSpeed =
                (short)((unsigned int)Vector_magnitude(&componentDelta) >> 8);
            damage = (short)((collisionSpeed * collisionSpeed) >> 1);

            objectMass = (unsigned short)
                g_asObjectRadarRadius_0059c790[obj];
            partnerMass = (unsigned short)
                g_asObjectRadarRadius_0059c790[partner];
            totalMass = objectMass + partnerMass;
            responseScale = ((objectMass - partnerMass) << 8) /
                            totalMass;
            responseScale = MaxInt(0x40, responseScale);
            responseScale = MinInt(responseScale, 0x400);
            ScaleFixedVector(&componentDelta, responseScale, &impulse);
            AddFixedVectors(&impulse, &partnerComponent, &impulse);
            forceMagnitude = (int)MultiplyFixed(
                objectMass * 0x600,
                (int)Vector_magnitude(&impulse)) + 0xa00;
            AddFixedVectors(&impulse, &g_aShipVelocity_0059c010[obj],
                            &g_aShipVelocity_0059c010[obj]);

            if (g_aeObjectClass_0059d100[obj] == OBJECT_CLASS_SHIP) {
                NormalizeFixedVector(&tangent);
                negate_vector(&tangent);
                ScaleFixedVector(&tangent, forceMagnitude, &tangent);
                rotational_acceleration(&g_vCollisionDelta_0059d690,
                                        &tangent, obj);
                negate_vector(&relativeVelocity);
                inflict_damage(partner, obj, damage, &relativeVelocity);
                negate_vector(&relativeVelocity);
            }

            SubtractFixedVectors(&g_aShipVelocity_0059c010[partner],
                                 &partnerComponent, &partnerTangent);
            responseScale = (objectMass << 9) / totalMass;
            responseScale = MaxInt(0x40, responseScale);
            responseScale = MinInt(responseScale, 0x400);
            ScaleFixedVector(&componentDelta, responseScale, &impulse);
            AddFixedVectors(&impulse, &partnerComponent, &impulse);
            AddFixedVectors(&impulse,
                            &g_aShipVelocity_0059c010[partner],
                            &g_aShipVelocity_0059c010[partner]);
            if (partnerClass == OBJECT_CLASS_SHIP) {
                NormalizeFixedVector(&partnerTangent);
                negate_vector(&partnerTangent);
                ScaleFixedVector(&partnerTangent, forceMagnitude,
                                 &partnerTangent);
                negate_vector(&g_vCollisionDelta_0059d690);
                rotational_acceleration(&g_vCollisionDelta_0059d690,
                                        &partnerTangent, partner);
                inflict_damage(obj, partner, damage, &relativeVelocity);
            }
            if (partnerClass == OBJECT_CLASS_CAPITAL_SHIP) {
                SubtractFixedVectors(&g_aShipPosition_0059c490[obj],
                                     &g_aShipVelocity_0059c010[obj],
                                     &g_aShipPosition_0059c490[obj]);
                g_anShipSpeed_0059b320[obj] = 0;
                g_aShipVelocity_0059c010[obj] =
                    g_aShipVelocity_0059c010[partner];
            }
        }
        break;
    }
    return 0;
}

/* Function start: 0x413880 */
unsigned int object_intelligence(short obj)
{
    if (g_nCannedSceneMode_00469fac == 4)
        return 0;
    if (g_nCannedSceneMode_00469fac == 2 &&
        g_aeObjectClass_0059d100[obj] > OBJECT_CLASS_MISSILE) {
        update_canned_sequence(obj);
        return 0;
    }
    switch (g_aeObjectClass_0059d100[obj]) {
    case OBJECT_CLASS_FUTURION:
        futurion_intelligence(obj);
        break;
    case OBJECT_CLASS_MINE:
        mine_intelligence(obj);
        break;
    case OBJECT_CLASS_MISSILE:
        if ((abs((int)g_nSpaceFrame_0059b420) & 3) != 0 &&
            g_nExternalViewShip_0046c040 != obj) {
            break;
        }
        if (g_acShipTarget_0059ce60[obj] != -1) {
            get_facing_range_from_object(
                obj, (short)g_acShipTarget_0059ce60[obj]);
        }
        switch (g_aeObjectType_0059b560[obj]) {
        case OBJECT_TYPE_DUMB_FIRE_MISSILE:
            g_anShipSpeed_0059b320[obj] =
                (get_ship_max_velocity(obj) + 10) * 0x100;
            break;
        case OBJECT_TYPE_HEAT_SEEKING_MISSILE:
            heat_seeking_missile_intelligence(obj);
            break;
        case OBJECT_TYPE_FF_MISSILE:
            FF_missile_intelligence(obj);
            break;
        case OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE:
            point_ship(obj, 0, &g_vToTarget_0059d4d0);
            g_anShipSpeed_0059b320[obj] =
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
