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
void CalibrateJoystickInteractive(short logicalFile, short section,
                                   short flags, short mode)
{
    InputDeviceSample samples[2];
    short calibration[6];
    short device;
    short shown;
    int file;
    int failed;

    (void)logicalFile;
    (void)section;
    (void)flags;
    (void)mode;
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
void set_eye_direction_and_position(void)
{
    if (g_bScriptedView_0046a8d4 == 1)
        update_scripted_view();

    switch (DAT_0046c03c) {
    case 0:
        copy_frame(0, 61);
        g_aShipVelocity_0059c010[61] = g_aShipVelocity_0059c010[0];
        g_aShipPosition_0059c490[61] = g_aShipPosition_0059c490[0];
        break;
    case 15:
        AddFixedVectors(&g_aShipPosition_0059c490[61],
                        &g_aShipVelocity_0059c010[61],
                        &g_aShipPosition_0059c490[61]);
        rotate_object(61);
        rotate_eye_to_goal();
        break;
    }
}

/* Function start: 0x4117B0 */
unsigned int force_view(int view, int obj)
{
    DAT_0046c03c = -1;
    return new_view(view, obj);
}

/* Function start: 0x4117D0 */
unsigned int new_view(int view, int obj)
{
    int scripted;

    if ((int)DAT_0046c03c == view)
        return 0;
    DAT_0046c03c = view;
    if (obj == -1)
        g_asObjectCollisionRadius_0059d710[WC1_EYE_OBJECT] = 10;
    else
        g_asObjectCollisionRadius_0059d710[WC1_EYE_OBJECT] =
            MaxShort(10, g_asObjectCollisionRadius_0059d710[obj]);
    switch (view) {
    case 0:
        initialize_cockpit(0);
        copy_frame(0, WC1_EYE_OBJECT);
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
        if (g_bScriptedView_0046a8d4 == 0 && obj >= 0) {
            FixedVector offset;

            ScaleFixedVector(&g_aShipForwardVector_0059bce0[obj],
                             -1200 << 8, &offset);
            AddFixedVectors(&g_aShipPosition_0059c490[obj], &offset,
                            &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
            g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT] =
                g_aShipUpVector_0059b9e0[obj];
            g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] = offset;
            fix_objects_ijk(WC1_EYE_OBJECT);
            zero_vector(&g_aShipVelocity_0059c010[WC1_EYE_OBJECT]);
        }
        break;
    case 5:
    case 6:
        initialize_cockpit(4);
        break;
    case 7:
        initialize_cockpit(4);
        copy_frame(0, WC1_EYE_OBJECT);
        zero_vector(&g_aShipVelocity_0059c010[WC1_EYE_OBJECT]);
        break;
    case 8:
        initialize_cockpit(4);
        if (obj >= 0) {
            g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT] =
                g_aShipRightVector_0059b6e0[obj];
            g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT] =
                g_aShipForwardVector_0059bce0[obj];
            ZeroVectorPtr((int *)&g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT]);
            g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] =
                g_aShipUpVector_0059b9e0[obj];
            fix_objects_ijk(WC1_EYE_OBJECT);
            zero_vector(&g_aShipVelocity_0059c010[WC1_EYE_OBJECT]);
        }
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
                              0, 0, 0, 0);
        zero_vector(&g_aShipVelocity_0059c010[WC1_EYE_OBJECT]);
        look_at(g_aShipPosition_0059c490[0]);
        break;
    case 13:
    case 14:
        initialize_cockpit(4);
        break;
    }
    scripted = g_bScriptedView_0046a8d4;
    if (scripted == 1)
        g_bScriptedView_0046a8d4 = 0;
    set_eye_direction_and_position();
    g_bScriptedView_0046a8d4 = scripted;
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
void house_keep_objects(void)
{
    short obj = 0;

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
            if ((short)count_down(obj) == 0)
                remove_object(obj);
            break;
        case OBJECT_CLASS_MINE:
        case OBJECT_CLASS_MISSILE:
            if ((short)count_down(obj) == 0)
                explode(obj, obj);
            break;
        case OBJECT_CLASS_SHIP:
        case OBJECT_CLASS_CAPITAL_SHIP:
            if ((short)count_down(obj) < 1 &&
                g_asObjectCounter_0059c330[obj] == 0 &&
                g_aeSpecialManeuver_0059c3c0[obj] ==
                    SPECIAL_MANEUVER_UNKNOWN_9)
                Create_explosion_debris(obj);
            break;
        }
        obj++;
    } while (obj <= WC1_SPACE_LAST_MOVING_OBJECT);
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
void accelerate_and_move_object(short obj)
{
    FixedVector desired;
    FixedVector delta;
    FixedVector accelerationVector;
    int magnitude;
    int acceleration;
    int direction;
    int scale;

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
    }
    if (g_aeObjectClass_0059d100[obj] >= OBJECT_CLASS_MISSILE &&
        g_aeSpecialManeuver_0059c3c0[obj] <
            SPECIAL_MANEUVER_KILL_ENGINES &&
        g_aeShipTactic_0059d5e0[obj] != TACTIC_SIT_STILL) {
        zero_vector(&accelerationVector);
        if (g_aeSpecialManeuver_0059c3c0[obj] ==
                SPECIAL_MANEUVER_AFTERBURNER) {
            g_asShipAfterburnerTimer_0059c810[obj]--;
            if (g_asShipAfterburnerTimer_0059c810[obj] == 0) {
                g_aeSpecialManeuver_0059c3c0[obj] =
                    SPECIAL_MANEUVER_NONE;
                ScaleFixedVector(&g_aShipForwardVector_0059bce0[obj],
                                 g_anShipSpeed_0059b320[obj], &desired);
            } else {
                ScaleFixedVector(&g_aShipForwardVector_0059bce0[obj],
                    (g_aObjectTypeData_00466458[
                        g_aeObjectType_0059b560[obj]].maximumVelocity + 20) *
                        0x200,
                    &desired);
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
                                 g_anShipSpeed_0059b320[obj], &desired);
            } else {
                zero_vector(&desired);
                drain_fuel(obj, 140);
            }
        } else {
            ScaleFixedVector(&g_aShipForwardVector_0059bce0[obj],
                             g_anShipSpeed_0059b320[obj], &desired);
        }
        ComputeVectorDelta(&g_aShipVelocity_0059c010[obj], &desired,
                           &delta);
        magnitude = (int)Vector_magnitude(&delta);
        if (magnitude > 0) {
            acceleration = GetShipAccelerationRate(obj);
            if (alert_flag(obj, 1) != 0 && acceleration < 0x500)
                acceleration = 0x500;
            if (g_aeSpecialManeuver_0059c3c0[obj] ==
                    SPECIAL_MANEUVER_AFTERBURNER ||
                g_aeSpecialManeuver_0059c3c0[obj] ==
                    SPECIAL_MANEUVER_SUPER_BRAKE)
                acceleration = acceleration * 2;
            direction = (int)DivideFixed(
                dot_product(&delta, &g_aShipForwardVector_0059bce0[obj]),
                magnitude);
            acceleration = (int)MultiplyFixed(
                acceleration, direction + 0x200);
            scale = MinInt((int)DivideFixed(acceleration >> 1, magnitude),
                           0x100);
            ScaleFixedVector(&delta, scale, &accelerationVector);
            if (g_aeSpecialManeuver_0059c3c0[obj] !=
                    SPECIAL_MANEUVER_AFTERBURNER)
                g_abShipExhaustHeat_0059d610[obj] = 2;
        }
        AddFixedVectors(&accelerationVector,
                        &g_aShipVelocity_0059c010[obj],
                        &g_aShipVelocity_0059c010[obj]);
    }
    AddFixedVectors(&g_aShipPosition_0059c490[obj],
                    &g_aShipVelocity_0059c010[obj],
                    &g_aShipPosition_0059c490[obj]);
}

/* Function start: 0x412CD0 */
void animate_shape(short obj)
{
    ObjectTypeData *typeData;
    unsigned short command;

    typeData = &g_aObjectTypeData_00466458[g_aeObjectType_0059b560[obj]];
    if (typeData->animation == 0)
        return;
    if (--g_asObjectAnimationDelay_0059b660[obj] > 0)
        return;
    g_asObjectAnimationDelay_0059b660[obj] = typeData->yawRate;
    command = *(unsigned short *)(typeData->animation +
              g_asObjectAnimationIndex_0059da30[obj] * 4);
    if ((command & 0xf000) == 0x9000) {
        g_asObjectAnimationIndex_0059da30[obj] = command & 0x0fff;
        command = *(unsigned short *)(typeData->animation +
                  g_asObjectAnimationIndex_0059da30[obj] * 4);
    } else if ((command & 0xf000) == 0xa000) {
        remove_object(obj);
        return;
    }
    if ((command & 0x0c00) == 0x0400) {
        g_asObjectScale_0059de40[obj] +=
            (command & 0x3f) * (g_asObjectScale_0059de40[obj] >> 6);
    } else if ((command & 0x0c00) == 0x0800) {
        g_asObjectScale_0059de40[obj] -=
            (command & 0x3f) * (g_asObjectScale_0059de40[obj] >> 6);
    } else {
        g_asObjectViewFrame_0059d230[obj] = command & 0x3f;
    }
    g_asObjectFlip_0059c870[obj] = (command & 0xc0) >> 2;
    g_asObjectAnimationIndex_0059da30[obj]++;
}

/* Function start: 0x412E30 */
void animate_object(short obj)
{
    switch (g_aeObjectClass_0059d100[obj]) {
    case OBJECT_CLASS_EXPLOSION:
        if (g_aObjectTypeData_00466458[
                g_aeObjectType_0059b560[obj]].animation == 0) {
            g_asObjectScale_0059de40[obj] +=
                g_asObjectScale_0059de40[obj] >> 3;
            break;
        }
    case OBJECT_CLASS_DEBRIS:
    case OBJECT_CLASS_FIXED_OBJECT:
    case OBJECT_CLASS_ASTEROID:
    case OBJECT_CLASS_MINE:
        animate_shape(obj);
        break;
    }
}

/* Function start: 0x413880 */
void object_intelligence(short obj)
{
    if (g_nCannedSceneMode_00469fac == 4)
        return;
    if (g_nCannedSceneMode_00469fac == 2 &&
        g_aeObjectClass_0059d100[obj] > OBJECT_CLASS_MISSILE) {
        update_canned_sequence(obj);
        return;
    }
    switch (g_aeObjectClass_0059d100[obj]) {
    case OBJECT_CLASS_FUTURION:
        futurion_intelligence(obj);
        break;
    case OBJECT_CLASS_MINE:
        mine_intelligence(obj);
        break;
    case OBJECT_CLASS_SHIP:
        ship_intelligence(obj);
        break;
    case OBJECT_CLASS_CAPITAL_SHIP:
        capital_ship_intelligence(obj);
        break;
    }
}
