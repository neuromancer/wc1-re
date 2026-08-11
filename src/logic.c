/*
 *  Ship mission logic (Mac `logic` compilation unit), with adjacent Win32
 *  ship and game-mode initialization routines.
 *
 *  Address range 0x421000-0x424fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: exact nested Mac CODE 5 `logic` unit at
 *  0x422010-0x423cdf; LoadOriginFxDrivers/EMStartUp and string band
 *  0x469A28-0x469B9C anchor the provisional enclosing file.
 */
#include "wc1.h"

/* Function start: 0x421100 */
short find_weapon(short obj, enum ObjectType weaponType)
{
    short weapon;

    weapon = 0;
    while (weapon < (signed char)g_aShipWeapons_0059cab0[obj][0]) {
        ShipWeaponSlot *weaponSlot =
            &((ShipWeaponSlot *)&g_aShipWeapons_0059cab0[obj][1])[weapon];

        if (weaponSlot->type == weaponType)
            return weapon;
        weapon++;
    }
    return -1;
}

/* Function start: 0x421150 */
int fire_missile(short ship)
{
    short weapon;
    ShipWeaponSlot *slot;
    signed char weaponCount;

    weapon = 0;
    slot = (ShipWeaponSlot *)&g_aShipWeapons_0059cab0[ship][1];
    weaponCount = (signed char)g_aShipWeapons_0059cab0[ship][0];
    if (weaponCount > 0) {
        do {
            if (g_aObjectTypeData_00466458[slot->type].objectClass ==
                    OBJECT_CLASS_MISSILE) {
                if (ship != 0)
                    return fire_weapon(ship, weapon);
                if (slot->disabled == 0) {
                    if ((slot->type == OBJECT_TYPE_HEAT_SEEKING_MISSILE ||
                         slot->type == OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE) &&
                        g_nTargetLockCountdown_0046c064 != 0) {
                        if ((short)get_mode(0) == 1)
                            ShowComponentHitHudMessage(
                                (char *)g_szNeedLock_0046998c,
                                DAT_004699a8, 3);
                        return -1;
                    }
                    return fire_weapon(0, weapon);
                }
            }
            weapon++;
            slot++;
        } while (weapon < weaponCount);
    }
    return -1;
}

/* Function start: 0x421220 */
short fire_fixed_projectile_weapon(short obj)
{
    ShipWeaponSlot *slot;
    int loadoutOffset;
    short ship;
    short weapon;
    int result;

    ship = obj;
    loadoutOffset = ship * sizeof(g_aShipWeapons_0059cab0[0]);
    weapon = 0;
    slot = (ShipWeaponSlot *)((unsigned char *)g_aShipWeapons_0059cab0 +
                              loadoutOffset + 1);
    if (*(signed char *)((unsigned char *)g_aShipWeapons_0059cab0 +
                         loadoutOffset) <= 0)
        return ship;
    do {
        if (g_aObjectTypeData_00466458[slot->type].objectClass ==
                OBJECT_CLASS_PROJECTILE &&
            slot->disabled == 0) {
            result = fire_weapon(ship, weapon);
            if (result == -1)
                return -1;
        }
        weapon++;
        slot++;
    } while ((short)*(signed char *)((unsigned char *)g_aShipWeapons_0059cab0 +
                                     loadoutOffset) > weapon);
    return *(signed char *)((unsigned char *)g_aShipWeapons_0059cab0 +
                            loadoutOffset);
}

/* Function start: 0x4212A0 */
int drop_mine(short obj, signed char weapon, enum ObjectType type,
              short lifetime)
{
    ShipWeaponSlot *weaponSlot;
    short mine;

    mine = new_object(type, obj);
    if (mine == -1)
        return -1;
    copy_frame(obj, mine);
    weaponSlot =
        &((ShipWeaponSlot *)&g_aShipWeapons_0059cab0[obj][1])[weapon];
    child_object(weaponSlot->hardpoint, mine, obj);
    if (lifetime == -1)
        lifetime = 20;
    g_acObjectCollisionGraceTicks_0059ddb0[mine] =
        (signed char)lifetime;
    g_asObjectCounter_0059c330[mine] = (short)(signed char)lifetime;
    if (obj == 0)
        RemovePlayerReleaseWeapon(weapon);
    else
        remove_weapon(obj, weapon);
    return mine;
}

/* Function start: 0x421350 */
void fire_afterburner(short obj, short time)
{
    long velocity = Vector_magnitude(&g_aShipVelocity_0059c010[obj]);

    if (velocity < get_ship_max_velocity(obj) * 0x500L) {
        set_special(obj, SPECIAL_MANEUVER_AFTERBURNER);
        g_asShipAfterburnerTimer_0059c810[obj] =
            g_aeSpecialManeuver_0059c3c0[obj] ==
                SPECIAL_MANEUVER_AFTERBURNER ? time : 0;
    }
}

/* Function start: 0x4213B0 */
unsigned int fire_super_brake(short ship)
{
    g_asShipAfterburnerTimer_0059c810[ship] = 10;
    set_special(ship, SPECIAL_MANEUVER_SUPER_BRAKE);
    return 0;
}

/* Function start: 0x4213D0 */
short flip_angle(short ship, short angle)
{
    if ((g_asObjectFlip_0059c870[ship] & 0x10) != 0)
        angle = (short)(180 - angle);
    if ((g_asObjectFlip_0059c870[ship] & 0x20) != 0)
        angle = (short)-angle;
    angle = (short)((angle + g_asObjectScreenAngle_0059cd90[ship]) % 360);
    if (angle < 0)
        angle = (short)(angle + 360);
    return angle;
}

/* Function start: 0x421430 */
void place_exhaust_on_ships(void)
{
    short ship;

    ship = 0;
    do {
        if (g_aeObjectClass_0059d100[ship] > OBJECT_CLASS_MINE &&
            g_anShipSpeed_0059b320[ship] != 0 &&
            g_aeSpecialManeuver_0059c3c0[ship] !=
                SPECIAL_MANEUVER_KILL_ENGINES &&
            g_asObjectScreenX_0059d9b0[ship] != (short)0x8001) {
            ObjectTypeData *typeData;
            short *animation;
            short offset;

            typeData = &g_aObjectTypeData_00466458[
                g_aeObjectType_0059b560[ship]];
            animation = (short *)typeData->animation;
            if (animation != 0) {
                offset = animation[g_asObjectViewFrame_0059d230[ship]];
                if (offset != -1) {
                    short *exhaust = (short *)((unsigned char *)animation +
                                               (unsigned short)offset);

                    while (exhaust[0] != -1) {
                        short object;
                        short scale;

                        object = find_vacant_3d_object();
                        if (object == -1)
                            return;
                        set_objects_data(object, OBJECT_TYPE_THRUSTERS,
                                         ship);
                        scale = (short)(exhaust[1] - RandomInRange(0, 32));
                        if (g_abShipExhaustHeat_0059d610[ship] == 0)
                            scale = (short)(scale - 32);
                        g_asObjectScale_0059de40[object] = scale;
                        g_asObjectDistance_0059b4a0[object] = exhaust[2];
                        g_asObjectScreenAngle_0059cd90[object] =
                            flip_angle(ship, exhaust[3]);
                        g_asObjectFlip_0059c870[object] = 0;
                        g_asObjectScreenX_0059d9b0[object] = exhaust[4];
                        g_asObjectScreenY_0059d930[object] = exhaust[5];
                        if (g_aeSpecialManeuver_0059c3c0[ship] ==
                                SPECIAL_MANEUVER_AFTERBURNER) {
                            g_asObjectViewFrame_0059d230[object] =
                                (short)(exhaust[0] * 3 +
                                        RandomInRange(0, 2));
                        } else {
                            g_asObjectViewFrame_0059d230[object] =
                                (short)(exhaust[0] * 2 + 12 +
                                        RandomInRange(0, 1));
                        }
                        exhaust += 6;
                    }
                }
            }
        }
        ship++;
    } while (ship < 10);
}

/* Function start: 0x4215E0 */
void reposition_fixed_child_objects(void)
{
    short object;

    object = 10;
    do {
        if (g_aeObjectClass_0059d100[object] ==
                OBJECT_CLASS_FIXED_OBJECT) {
            short parent;

            parent = (short)g_acObjectOwner_0059ce20[object];
            if (g_aeObjectType_0059b560[object] == OBJECT_TYPE_TURRET ||
                g_aeObjectType_0059b560[object] == OBJECT_TYPE_THRUSTERS) {
                int right;
                int up;
                long sine;
                long cosine;
                short parentScale;

                sine = SinFixed(g_asObjectScreenAngle_0059cd90[parent]);
                cosine = CosFixed(g_asObjectScreenAngle_0059cd90[parent]);
                parentScale = g_asObjectScreenScale_0059c950[parent];
                right = (int)g_asObjectScreenX_0059d9b0[object] *
                        (unsigned short)parentScale;
                g_asObjectDistance_0059b4a0[object] = (short)(
                    g_asObjectDistance_0059b4a0[object] +
                    g_asObjectDistance_0059b4a0[parent]);
                if ((g_asObjectFlip_0059c870[parent] & 0x10) != 0)
                    right = -right;
                up = (int)g_asObjectScreenY_0059d930[object] *
                     (unsigned short)parentScale;
                if ((g_asObjectFlip_0059c870[parent] & 0x20) != 0)
                    up = -up;
                g_asObjectScreenX_0059d9b0[object] = (short)(
                    ((MultiplyFixed(right, (int)cosine) -
                      MultiplyFixed(up, (int)sine)) >> 8) +
                    g_asObjectScreenX_0059d9b0[parent]);
                g_asObjectScreenY_0059d930[object] = (short)(
                    ((MultiplyFixed(up, (int)cosine) +
                      MultiplyFixed(right, (int)sine)) >> 8) +
                    g_asObjectScreenY_0059d930[parent]);
            }
            g_asObjectScreenScale_0059c950[object] = (short)(
                (unsigned int)(unsigned short)
                    g_asObjectScreenScale_0059c950[parent] *
                (unsigned int)(unsigned short)
                    g_asObjectScale_0059de40[object] >> 8);
        }
        object++;
    } while (object <= WC1_SPACE_LAST_MOVING_OBJECT);
}

/* Function start: 0x421760 */
unsigned int housekeep_power_plant_and_fuel(short ship)
{
    if (0 < g_anShipSpeed_0059b320[ship])
        drain_fuel(ship, 5);
    return 0;
}

/* Function start: 0x421780 */
unsigned int replenish_shields(short ship)
{
    signed char shield;

    if (ship == 0 && g_cPlayerPowerDamage_0059bff1 > 0 &&
        g_nSpaceFrame_0059b420 % (g_cPlayerPowerDamage_0059bff1 + 1) != 0)
        return 0;
    shield = 0;
    do {
        short current;
        short maximum;

        current = g_aasShipShield_0059d5b0[ship][shield];
        maximum = g_aasShipMaximumShield_0059d6e0[ship][shield];
        if (maximum < current) {
            g_aasShipShield_0059d5b0[ship][shield] = maximum;
            current = maximum;
        }
        if (current < maximum &&
            g_nSpaceFrame_0059b420 %
                g_aObjectTypeData_00466458[
                    g_aeObjectType_0059b560[ship]].animationDelay == 0) {
            g_aasShipShield_0059d5b0[ship][shield] =
                (short)(current + 1);
        }
        shield++;
    } while (shield < 2);
    return 0;
}

/* Function start: 0x421830 */
unsigned int replenish_weapon_energy_bank(short ship)
{
    short energy;

    if (ship == 0 && g_cPlayerPowerDamage_0059bff1 != 0 &&
        (int)(unsigned short)RandomInRange(0, 4) <
            g_cPlayerPowerDamage_0059bff1)
        return 0;
    energy = g_asShipWeaponEnergy_0059d470[ship];
    if (energy < 100) {
        if ((short)(g_aasShipShield_0059d5b0[ship][0] +
                    g_aasShipShield_0059d5b0[ship][1]) <
            (short)(g_aasShipMaximumShield_0059d6e0[ship][0] +
                    g_aasShipMaximumShield_0059d6e0[ship][1])) {
            energy = (short)(energy + 1);
        } else {
            energy = (short)(energy + 2);
        }
        g_asShipWeaponEnergy_0059d470[ship] = MinShort(energy, 100);
    }
    return 0;
}

/* Function start: 0x4218D0 */
void accelerate(short amount)
{
    if ((short)malf(0) != 0) {
        amount = (short)(amount - 2);
        if (g_nSpaceFrame_0059b420 % 3 == 0)
            PlaySfxWaveFileByNumber(3, -1, 0);
    }
    celerate(0, (int)amount << 8);
}

/* Function start: 0x421920 */
void your_afterburner(void)
{
    short time;
    int frame;
    int nextSoundFrame;

    if (g_anShipFuel_0059b470[0] <= 0)
        return;
    if ((short)malf(0) != 0) {
        PlaySfxWaveFileByNumber(3, -1, 0);
        return;
    }
    if (g_aeSpecialManeuver_0059c3c0[0] !=
            SPECIAL_MANEUVER_AFTERBURNER) {
        time = 8;
    } else if (g_asShipAfterburnerTimer_0059c810[0] == 0) {
        time = 8;
    } else {
        if (g_asShipAfterburnerTimer_0059c810[0] > 2)
            return;
        time = 2;
    }
    fire_afterburner(0, time);
    frame = (int)g_nSpaceFrame_0059b420;
    nextSoundFrame = frame + 6;
    if (nextSoundFrame < g_nAfterburnerSoundDeadline_005a7ce8)
        g_nAfterburnerSoundDeadline_005a7ce8 = 0;
    if (g_nAfterburnerSoundDeadline_005a7ce8 < frame) {
        g_nAfterburnerSoundDeadline_005a7ce8 = nextSoundFrame;
        PlaySfxWaveFileByNumber(12, -1, 0);
    }
}

/* Function start: 0x421A40 */
unsigned int EMShutDown(void)
{
    if (g_nEventManagerActive_0059a850 != 0)
        ShutdownEventManager();
    return 0;
}

/* Function start: 0x421A60 */
unsigned int InitializeEventManagerResources(void)
{
    g_nInputTickScale_0059af90 = 20;
    g_pMouseCursorResource_005a7cdc =
        (unsigned char *)FetchDiskPacketRetrying(14, 0, 0x10);
    DAT_0059ab19 = g_pMouseCursorResource_005a7cdc;
    DAT_0059ab1d = 0;
    DAT_0059ab23 = &DAT_005a6ba0;
    return 0;
}

/* Function start: 0x421AB0 */
unsigned int EMStartUp(void)
{
    PromptInsertNumberedDisk(0);
    RegisterEventManagerShutdown((void (*)(void))LogMemoryUsage);
    if (InitializeEventManager(20, InitializeEventManagerResources, 0) == 0)
        exit_squadron("EMStartUp Failed");
    ConfigureEventManagerPointer(DAT_0059ab19, 0);
    SetEventManagerPump(PollJoystickButtonEvents);
    g_aInputDeviceSamples_005a81f0[2].x = 6;
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
    g_nInputDoubleClickInterval_0046af54 = 2;
    g_nJoystickFailureValue_005a81e0 = -1;
    EMStartUp();
    g_dwOriginalFreeMemory_005a7cd8 = GetFixedOneMillionThunkAlt();
    g_nMemoryConfiguration_005a7cd4 = 0;
    g_nAvailableGameMemory_005a7ce0 =
        (int)g_dwOriginalFreeMemory_005a7cd8;
    InitializeGameTextContexts();

    memset(DAT_005a6ba0.pixels, 0, 320 * 200);
    DIBslam();
    DIBslamReal();
    g_nFrameSkip_00469fb8 = 1;
    initialize_direction_view_frames();
    g_pConstellationDefinitions_00598a28 =
        (ConstellationObjectDefinition *)LoadPacketAllocated(0x3a, 0);
    g_pMissionCampaignData_005988bc = LoadPacketAllocated(0x3a, 1);
    return 0;
}

/* Function start: 0x421D80 */
unsigned int InitializeGameTextContexts(void)
{
    g_stModalSourceViewport_005a7670 = DAT_005a6ba0;
    g_stDefaultTextContext_005a7740.viewport =
        &g_stModalSourceViewport_005a7670;
    g_stDefaultTextContext_005a7740.text =
        g_szDefaultTextBuffer_005a7590;
    InitializeTextContextFromFont(&g_stDefaultTextContext_005a7740, 1,
                                  (unsigned char)DAT_004699b4,
                                  DAT_0046999c);
    SetTextContext(&g_stDefaultTextContext_005a7740);

    DAT_005a6bc0.viewport = &DAT_005a7510;
    DAT_005a6bc0.text = g_szDefaultTextBuffer_005a7590;
    DAT_005a6bc0.alignment = 2;
    InitializeTextContextFromFont(&DAT_005a6bc0, 1,
                                  (unsigned char)DAT_004699ac, -1);
    return 0;
}

/* Function start: 0x421E20 */
unsigned int initialize_direction_view_frame(short yaw, short pitch,
                                             signed char frame)
{
    FixedVector right = { 0x100, 0, 0 };
    FixedVector up = { 0, 0x100, 0 };
    FixedVector forward = { 0, 0, 0x100 };
    short index;

    rotate_about_j(yaw, &right, &forward);
    rotate_about_i(pitch, &up, &forward);
    index = (short)frame;
    g_aDirectionViewRightVector_005a6c20[index] = right;
    g_aDirectionViewUpVector_005a6f10[index] = up;
    g_aDirectionViewForwardVector_005a7200[index] = forward;
    return 0;
}

/* Function start: 0x421EF0 */
unsigned int initialize_direction_view_frames(void)
{
    signed char frame;
    signed char pitchBands;
    signed char yawSectors;
    short pitch;
    short yaw;

    frame = 1;
    pitch = 90;
    initialize_direction_view_frame(0, pitch, 0);
    pitchBands = 5;
    do {
        pitch -= 30;
        yaw = 0;
        yawSectors = 12;
        do {
            initialize_direction_view_frame(yaw, pitch, frame++);
            yaw += 30;
            yawSectors--;
        } while (yawSectors != 0);
        pitchBands--;
    } while (pitchBands != 0);
    initialize_direction_view_frame(0, -90, frame);
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
int ace_status(short ace, unsigned char bits)
{
    return (g_stCampaignState_0059ca50.aceFlags[ace] & bits) == bits;
}

/* Function start: 0x422030 */
void unflag_ace(short ace, unsigned char bits)
{
    g_stCampaignState_0059ca50.aceFlags[ace] &= ~bits;
}

/* Function start: 0x422050 */
void flag_ace(short ace, unsigned char bits)
{
    g_stCampaignState_0059ca50.aceFlags[ace] |= bits;
}

/* Function start: 0x422060 */
void kill_ace(short ace)
{
    if (ace_status(ace, 1) != 0) {
        unflag_ace(ace, 1);
        flag_ace(ace, 2);
    }
}

/* Function start: 0x422090 */
void ace_greeting(short obj)
{
    short ace = (short)g_aiPilotLevel_0059cf30[obj] - 14;

    send_message(obj, (signed char)(ace_status(ace, 4) != 0));
    flag_ace(ace, 8);
}

/* Function start: 0x4220D0 */
void prepare_ace(short ace)
{
    unflag_ace(ace, 0x1a);
    flag_ace(ace, 0x20);
}

/* Function start: 0x4220F0 */
short signed_random(short range)
{
    return (short)RandomBelowOrEqual(range * 2) - range;
}

/* Function start: 0x422110 */
int alert_flag(short ship, unsigned int bits)
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
unsigned int set_alert(short i, unsigned int bits)
{
    DAT_0059b430[i] |= bits;
    return bits & 0xffff0000;
}

/* Function start: 0x422160 */
void clear_alert(short i)
{
    g_asCollisionCountdown_0059d2d0[i] = 0;
    DAT_0059b430[i] = 0;
    DAT_0059cf20[i] = 0xff;
}

/* Function start: 0x422180 */
void start_collision_alert(short obj, short other)
{
    if ((short)(signed char)DAT_0059cf20[obj] != other) {
        DAT_0059cf20[obj] = (unsigned char)other;
        steady_object(obj);
    }
    g_asCollisionCountdown_0059d2d0[obj] = 3;
    set_alert(obj, 1);
    g_asShipAfterburnerTimer_0059c810[obj] = 0;
    set_special(obj, SPECIAL_MANEUVER_NONE);
}

/* Function start: 0x4221E0 */
void try2end_collision_alert(short obj)
{
    g_asCollisionCountdown_0059d2d0[obj]--;
    if (g_asCollisionCountdown_0059d2d0[obj] <= 0)
        clear_alert(obj);
    else
        set_alert(obj, 2);
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
short real_crash_time(short obj, short other)
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
    distance = Vector_magnitude(&relativePosition);
    if (((long)collisionRadius + 1500) * 0x100 < distance)
        return 0x7fff;

    ComputeVectorDelta(&g_aShipVelocity_0059c010[obj],
                       &g_aShipVelocity_0059c010[other],
                       &relativeVelocity);
    relativeSpeed = Vector_magnitude(&relativeVelocity);
    if (relativeSpeed == 0)
        return 0x7fbc;

    time = FixedToShortSaturating(
        DivideFixed((int)distance, (int)relativeSpeed));
    if (time >= 30)
        return time;

    ScaleFixedVector(&relativeVelocity, (int)time << 8, &travel);
    AddFixedVectors(&relativePosition, &travel, &separation);
    range = FixedToShortSaturating(
        (int)Vector_magnitude(&separation));
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
            (int)Vector_magnitude(&separation));
        if (collisionRadius >= range)
            collisionFound = 1;
    } while (collisionFound == 0);
    return elapsed;
}

/* Function start: 0x422440 */
void clear_crash_cache(void)
{
    short i = 0;

    do {
        g_asCollisionPartner_005a7cc0[i] = -1;
        i = i + 1;
    } while (i < 10);
}

/* Function start: 0x422460 */
short crash_time(short obj, short other)
{
    if (g_asCollisionPartner_005a7cc0[obj] == other)
        return g_asCollisionTime_005a7ca0[obj];
    if (other < 10 && g_asCollisionPartner_005a7cc0[other] == obj)
        return g_asCollisionTime_005a7ca0[other];
    if (g_aeObjectClass_0059d100[other] == OBJECT_CLASS_ASTEROID &&
        (g_asObjectScreenX_0059d9b0[other] == -0x7fff ||
         g_asObjectScreenX_0059d9b0[obj] == -0x7fff))
        return 0x7fff;
    return real_crash_time(obj, other);
}

/* Function start: 0x4224F0 */
short detect_collisions(short obj)
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
            time = crash_time(obj, other);
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

/* Function start: 0x422590 */
int are_alive(short obj)
{
    if (unactive(obj) == 0 &&
        g_aeShipObjective_0059d200[obj] != OBJECTIVE_WANDER)
        return 1;
    return 0;
}

/* Function start: 0x4225C0 */
void trim_goals(short obj, short amount)
{
    short goal = g_anYawGoal_0059c310[obj];

    if (goal > amount)
        g_anYawGoal_0059c310[obj] = amount;
    else if (goal < -amount)
        g_anYawGoal_0059c310[obj] = -amount;
    goal = g_anPitchGoal_0059d7a0[obj];
    if (goal > amount)
        g_anPitchGoal_0059d7a0[obj] = amount;
    else if (goal < -amount)
        g_anPitchGoal_0059d7a0[obj] = -amount;
}

/* Function start: 0x422640 */
int report_kilrathi_rout(int mode)
{
    short obj;

    for (obj = 0; obj < 10; obj++) {
        if (g_aeObjectClass_0059d100[obj] < OBJECT_CLASS_SHIP ||
            g_aeSpecialManeuver_0059c3c0[obj] ==
                SPECIAL_MANEUVER_UNKNOWN_9)
            continue;
        switch (mode) {
        case 0:
            if (g_aeShipSide_0059d650[obj] == SIDE_KILRATHI)
                return 1;
            break;
        case 1:
            if (g_aeShipSide_0059d650[obj] == SIDE_KILRATHI &&
                g_aeShipMissionType_0059c3f0[obj] != MISSION_TYPE_ROUT &&
                distance_from_object(0, obj) < 16000)
                return 1;
            break;
        case 2:
            if (g_aeShipSide_0059d650[obj] == SIDE_KILRATHI &&
                (g_aeShipObjective_0059d200[obj] ==
                     OBJECTIVE_ENGAGE_ENEMY ||
                 g_aeShipObjective_0059d200[obj] ==
                     OBJECTIVE_DESTROY_SHIP))
                return 1;
            break;
        }
    }
    if (mode == 1 && g_nCurrentWave_0046c01c != -1)
        check_next_wave();
    return 0;
}

/* Function start: 0x422710 */
short find_ship_index(short missionShip)
{
    short obj;

    g_nLastFoundShip_005a7cba = -1;
    if (missionShip != -1) {
        for (obj = 0; obj < 10; obj++) {
            if (((g_aeObjectClass_0059d100[obj] >= OBJECT_CLASS_SHIP &&
                  g_aeSpecialManeuver_0059c3c0[obj] !=
                      SPECIAL_MANEUVER_UNKNOWN_9) ||
                 g_aeObjectClass_0059d100[obj] == OBJECT_CLASS_FUTURION) &&
                g_nShipMissionIndices_0059c830[obj] == missionShip) {
                g_nLastFoundShip_005a7cba = obj;
                return obj;
            }
        }
    }
    return -1;
}

/* Function start: 0x422780 */
int try2rout(short obj)
{
    short canContinue;
    short other;

    canContinue = 0;
    if (g_nTrainSimActive_00469e2c != 0) {
        canContinue = 1;
    } else {
        other = 0;
        do {
            if (g_aeObjectClass_0059d100[other] ==
                    OBJECT_CLASS_CAPITAL_SHIP &&
                g_aeSpecialManeuver_0059c3c0[other] !=
                    SPECIAL_MANEUVER_UNKNOWN_9 &&
                g_aeShipSide_0059d650[obj] ==
                    g_aeShipSide_0059d650[other])
                canContinue = 1;
            other++;
        } while (other < 10);
    }
    if (canContinue != 0) {
        g_acShipStress_0059d620[obj] = 0;
        maneuver_complete(obj);
    } else {
        reset_mission_type(obj, MISSION_TYPE_ROUT);
        if (obj == g_nYourWingman_0046c04c)
            send_message(obj, 9);
    }
    return canContinue == 0;
}

/* Function start: 0x422830 */
unsigned char no_goal(short ship)
{
    return (g_anYawGoal_0059c310[ship] |
            g_anPitchGoal_0059d7a0[ship] |
            g_anRollGoal_0059d630[ship]) == 0;
}

/* Function start: 0x422860 */
int being_tailed(short obj, short other)
{
    ship_vs_ship(obj, other);
    return g_nFacingToTarget_0059d920 < -60 &&
           g_nTargetFacing_0059d52a > 85 &&
           g_nTargetRange_0059ce10 < 7000;
}

/* Function start: 0x4228A0 */
int any_enemy_tail(short obj)
{
    short other;

    other = 0;
    do {
        if (g_aeObjectClass_0059d100[other] >= OBJECT_CLASS_SHIP &&
            g_aeSpecialManeuver_0059c3c0[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            g_aeShipSide_0059d650[obj] != g_aeShipSide_0059d650[other] &&
            g_acShipTarget_0059ce60[other] == obj &&
            being_tailed(obj, other) != 0) {
            g_nTargetShip_0059c3b0 = other;
            return 1;
        }
        other++;
    } while (other < 10);
    g_nTargetShip_0059c3b0 = -1;
    return 0;
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
            being_tailed(obj, other) != 0)
            return other;
    }
    return -1;
}

/* Function start: 0x4229B0 */
int is_ship_tailing_player_target(short obj)
{
    short target = (short)g_acShipTarget_0059ce60[0];

    if (unactive(target) == 0 && being_tailed(target, obj) != 0)
        return 1;
    return 0;
}

/* Function start: 0x4229F0 */
int missile_on_tail(short obj)
{
    short other;

    for (other = 0; other < 10; other++) {
        if (g_aeObjectClass_0059d100[other] == OBJECT_CLASS_MISSILE &&
            g_acShipTarget_0059ce60[other] == obj)
            return 1;
    }
    return 0;
}

/* Function start: 0x422A30 */
short select_weighted_value(short *choices)
{
    short roll;

    roll = (short)RandomBelowOrEqual(100) + 1;
    roll -= choices[0];
    while (roll > 0) {
        if (choices[0] == -1)
            return -1;
        choices += 2;
        roll -= choices[0];
    }
    if (choices[0] == -1)
        return -1;
    return choices[1];
}

/* Function start: 0x422A70 */
unsigned int build_squad_list(short leader)
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
unsigned int find_squad_center(FixedVector *center)
{
    short count = 0;

    zero_vector(center);
    while (g_acFormationMemberList_0059d490[count] != -1) {
        AddFixedVectors(center, &g_aShipPosition_0059c490[
                            (short)g_acFormationMemberList_0059d490[count]],
                        center);
        count++;
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

    build_squad_list(obj);
    find_squad_center(&center);
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

/* Function start: 0x422CA0 */
unsigned int alter_objective(short ship, enum ShipObjective objective)
{
    steady_object(ship);
    alter_tactic(ship, TACTIC_NONE);
    g_aeShipObjective_0059d200[ship] = objective;
    return 0;
}

/* Function start: 0x422CD0 */
unsigned int reset_tactic(short ship, enum ShipTactic tactic)
{
    reset_maneuver(ship, MANEUVER_NONE);
    g_aeShipTactic_0059d5e0[ship] = tactic;
    g_acShipTarget_0059ce60[ship] = -1;
    return 0;
}

/* Function start: 0x422D00 */
unsigned int alter_tactic(short ship, enum ShipTactic tactic)
{
    reset_maneuver(ship, MANEUVER_NONE);
    g_aeShipTactic_0059d5e0[ship] = tactic;
    return 0;
}

/* Function start: 0x422D30 */
void reset_maneuver(short ship, short maneuver)
{
    g_aeShipManeuver_0059dcb0[ship] = (enum ShipManeuver)maneuver;
    g_asShipCount_0059c420[ship] = 0;
    g_acShipSequence_0059d520[ship] = 0;
}

/* Function start: 0x422D60 */
void try2reset_maneuver(short obj, short maneuver)
{
    if (g_aeShipManeuver_0059dcb0[obj] != maneuver) {
        reset_maneuver(obj, (enum ShipManeuver)maneuver);
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
    if (*currentState == SPECIAL_MANEUVER_BLOWING_UP && (short)alert_flag(ship, 1))
        *currentState = SPECIAL_MANEUVER_NONE;
    return 0;
}

/* Function start: 0x422DD0 */
unsigned int approach_zero_speed(short ship)
{
    approach_speed(ship, 0);
    return 0;
}

/* Function start: 0x422DF0 */
unsigned int approach_min_speed(short obj)
{
    approach_speed(obj, 0x500);
    return 0;
}

/* Function start: 0x422E10 */
unsigned int approach_half_speed(short obj)
{
    short speed = g_aObjectTypeData_00466458[
        g_aeObjectType_0059b560[obj]].cruiseVelocity;

    approach_speed(obj, (int)(short)(speed & 0xfffe) << 7);
    return 0;
}

/* Function start: 0x422E50 */
unsigned int approach_cruise_speed(short ship)
{
    approach_speed(ship,
        (int)g_aObjectTypeData_00466458[
            g_aeObjectType_0059b560[ship]].cruiseVelocity << 8);
    return 0;
}

/* Function start: 0x422E80 */
unsigned int approach_full_speed(short ship)
{
    approach_speed(ship,
                      (int)g_asShipMaximumSpeed_0059c440[ship] << 8);
    return 0;
}

/* Function start: 0x422EA0 */
unsigned int approach_ship_speed(short obj, short other)
{
    approach_speed(obj, g_anShipSpeed_0059b320[other]);
    return 0;
}

/* Function start: 0x422EC0 */
unsigned int get_front_spot(short obj, unsigned short distance,
                            FixedVector *point)
{
    ScaleFixedVector(&g_aShipForwardVector_0059bce0[obj],
                     (int)distance << 8, point);
    AddFixedVectors(&g_aShipPosition_0059c490[obj], point, point);
    return 0;
}

/* Function start: 0x422F10 */
unsigned int get_rear_spot(short obj, unsigned short distance,
                           FixedVector *point)
{
    ScaleFixedVector(&g_aShipForwardVector_0059bce0[obj],
                     -((int)distance << 8), point);
    AddFixedVectors(&g_aShipPosition_0059c490[obj], point, point);
    return 0;
}

/* Function start: 0x422F60 */
unsigned int close_behind(short range)
{
    if (g_nTargetRange_0059ce10 < range &&
        g_nTargetFacing_0059d52a < -0x32)
        return 1;
    return 0;
}

/* Function start: 0x422F80 */
short scan_for_enemy(short obj, unsigned short range)
{
    short target;
    short other;
    short distance;

    target = -1;
    g_nTargetRange_0059ce10 = 0;
    for (other = 0; other < 10; other++) {
        if (g_aeObjectClass_0059d100[other] < OBJECT_CLASS_SHIP ||
            g_aeSpecialManeuver_0059c3c0[other] ==
                SPECIAL_MANEUVER_UNKNOWN_9)
            continue;
        g_nTargetShip_0059c3b0 = target;
        if (g_aeShipSide_0059d650[obj] == g_aeShipSide_0059d650[other])
            continue;
        distance = distance_from_point(other, &g_aShipPosition_0059c490[obj]);
        target = g_nTargetShip_0059c3b0;
        if (distance < range &&
            (target == -1 ||
             distance < g_nTargetRange_0059ce10)) {
            target = other;
            g_nTargetRange_0059ce10 = distance;
        }
    }
    if (target != -1) {
        g_nTargetShip_0059c3b0 = target;
        get_facing_range_from_object(obj, g_nTargetShip_0059c3b0);
        g_nTargetRange_0059ce10 =
            distance_from_object(obj, g_nTargetShip_0059c3b0);
        target = g_nTargetShip_0059c3b0;
    }
    g_nTargetShip_0059c3b0 = target;
    return target;
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

/* Function start: 0x4230F0 */
short nearest_enemy_range(short obj)
{
    short range;
    short other;

    g_nTargetShip_0059c3b0 = -1;
    range = 0x7fff;
    other = 0;
    do {
        if (g_aeObjectClass_0059d100[other] >= OBJECT_CLASS_SHIP &&
            g_aeSpecialManeuver_0059c3c0[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            g_aeShipSide_0059d650[obj] != g_aeShipSide_0059d650[other])
            range = MinShort(range, distance_from_object(obj, other));
        other++;
    } while (other < 10);
    return range;
}

/* Function start: 0x423210 */
void fire_when_ready(short obj, short aimed)
{
    (void)aimed;
    if (g_nYourWingman_0046c04c == obj &&
        g_aiPilotLevel_0059cf30[obj] != 11) {
        ship_vs_ship(obj, 0);
        if (g_nFacingToTarget_0059d920 > 80)
            return;
    }
    fire(obj, (short)g_acShipTarget_0059ce60[obj]);
}

/* Function start: 0x423260 */
unsigned int ships_within_range(short obj, short other, short range)
{
    FixedVector delta;

    ComputeVectorDelta(&g_aShipPosition_0059c490[obj],
                       &g_aShipPosition_0059c490[other], &delta);
    return IsVectorWithinRange(&delta, range);
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
    short target;
    short other;
    unsigned short range;

    target = -1;
    for (other = 0; other < 10; other++) {
        if (g_aeObjectClass_0059d100[other] >= OBJECT_CLASS_SHIP &&
            g_aeSpecialManeuver_0059c3c0[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            g_aeShipSide_0059d650[obj] != g_aeShipSide_0059d650[other]) {
            g_nTargetShip_0059c3b0 = target;
            if (g_acShipTarget_0059ce60[other] != obj)
                continue;
            range = (unsigned short)distance_from_object(other, obj);
            target = g_nTargetShip_0059c3b0;
            if (target == -1 || range < g_nTargetRange_0059ce10) {
                target = other;
                g_nTargetRange_0059ce10 = (short)range;
            }
        }
    }
    g_nTargetShip_0059c3b0 = target;
    return target != -1;
}

/* Function start: 0x423400 */
unsigned int target_within_range(short obj)
{
    short target = (short)g_acShipTarget_0059ce60[obj];

    if (unactive(target) != 0)
        return 0;
    return ships_within_range(obj, target, 7000);
}

/* Function start: 0x423440 */
short build_target_list(short obj, short range)
{
    short count;
    short other;
    short distance;

    count = 0;
    other = 0;
    do {
        if (g_aeObjectClass_0059d100[other] >= OBJECT_CLASS_SHIP &&
            g_aeSpecialManeuver_0059c3c0[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            g_aeShipSide_0059d650[obj] != g_aeShipSide_0059d650[other]) {
            distance = distance_from_object(obj, other);
            if (distance < range) {
                g_acFormationMemberList_0059d490[count] =
                    (signed char)other;
                g_asTargetListRange_0059cf60[count] = distance;
                count++;
            }
        }
        other++;
    } while (other < 10);
    g_acFormationMemberList_0059d490[count] = -1;
    return count;
}

/* Function start: 0x4234C0 */
int select_safe_target(short obj)
{
    short target;
    short index;

    build_target_list(obj, 7000);
    index = -1;
    do {
        index++;
        target = (short)g_acFormationMemberList_0059d490[index];
        if (target == -1)
            break;
    } while (in_danger(target) != 0);
    if (target == -1 && index > 0) {
        index--;
        target = (short)g_acFormationMemberList_0059d490[
            (short)RandomBelowOrEqual(index)];
    }
    g_nTargetShip_0059c3b0 = target;
    return target != -1;
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
        g_aShipMissionSpot_0059dd10[obj] =
            g_aShipMissionSpot_0059dd10[leader];
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
unsigned int dead_ship(short i)
{
    if (i != -1 && g_aMissionShips_0046c948[i].state != 3)
        return 0;
    return 1;
}

/* Function start: 0x423640 */
int gone_ship(short missionShip)
{
    if (missionShip != -1 &&
        g_aMissionShips_0046c948[missionShip].state != 3 &&
        g_aMissionShips_0046c948[missionShip].state != 2)
        return 0;
    return 1;
}

/* Function start: 0x423670 */
short skill_rating(short obj)
{
    int rating = g_aiPilotLevel_0059cf30[obj];

    if (rating < 5)
        return MaxShort(2, (short)rating);
    if (rating == 13)
        return 5;
    if (rating < 14)
        return (short)((rating - 5 >> 1) + 4);
    return (short)rating - 10;
}

/* Function start: 0x4236B0 */
int skill_check(short obj, short difficulty)
{
    short roll;

    roll = (short)RandomBelowOrEqual(MinShort(8, difficulty));
    return roll < skill_rating(obj);
}

/* Function start: 0x4236F0 */
short find_ships_sphere(short missionShip)
{
    MissionNavPoint *navPoint;
    short fallback;
    short navIndex;
    short shipIndex;

    navIndex = 0;
    navPoint = g_aMissionNavPoints_0046c2f0;
    fallback = -1;
    do {
        shipIndex = 0;
        do {
            if (navPoint->missionShips[shipIndex] == missionShip) {
                if (navPoint->type == 1)
                    return navIndex;
                if (fallback == -1)
                    fallback = navIndex;
            }
            shipIndex++;
        } while (shipIndex < 10);
        navIndex++;
        navPoint++;
    } while (navIndex < WC1_ACTIVE_MISSION_NAV_POINT_COUNT);
    if (fallback != -1) {
        navIndex = fallback;
        if (g_aMissionNavPoints_0046c2f0[navIndex].type > 1)
            do {
                navIndex--;
            } while (g_aMissionNavPoints_0046c2f0[navIndex].type > 1);
    } else {
        navIndex = -1;
    }
    return navIndex;
}

/* Function start: 0x423780 */
int locate_ship(short missionShip, FixedVector *point)
{
    short navPoint;
    short obj;

    obj = find_ship_index(missionShip);
    if (obj != -1) {
        *point = g_aShipPosition_0059c490[obj];
        return 1;
    }
    if (dead_ship(missionShip) != 0)
        return 0;
    navPoint = find_ships_sphere(missionShip);
    if (navPoint != -1) {
        AddFixedVectors(&g_aMissionNavPoints_0046c2f0[navPoint].position,
                        &g_aMissionShips_0046c948[missionShip].position,
                        point);
        return 1;
    }
    zero_vector(point);
    return 0;
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
    while (++pathIndex < 16) {
        objective = (short)g_abFlightPath_0059c000[pathIndex];
        type = g_aMissionObjectives_0059dac0[objective].type;
        switch (type) {
        case 0:
            *point = g_aMissionObjectives_0059dac0[objective].position;
            g_abShipNavPointIndex_0059d7c0[obj] = (signed char)pathIndex;
            return 0;
        case 1:
            missionShip = (short)g_aMissionObjectives_0059dac0[objective].index;
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
int hostile_sphere(short obj, short navPoint)
{
    short index;
    short missionShip;
    short *ships = g_aMissionNavPoints_0046c2f0[navPoint].missionShips;

    for (index = 0; index < 10; index++) {
        missionShip = ships[index];
        if (missionShip != -1 &&
            g_aeShipSide_0059d650[obj] !=
                g_aMissionShips_0046c948[missionShip].side)
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
            hostile_sphere(obj, navPoint) != 0 &&
            distance_from_object(obj, other) > 10000) {
            explode(-1, obj);
            return 1;
        }
    }
    return 0;
}

/* Function start: 0x423A50 */
void engage(short obj, short target, enum ShipObjective objective)
{
    if (g_aeShipObjective_0059d200[obj] != objective) {
        reset_objective(obj, objective);
        if (g_acShipRating_0059cd80[obj] > 8 &&
            ace_status((short)g_aiPilotLevel_0059cf30[obj] - 14, 8) == 0)
            ace_greeting(obj);
    }
    g_acShipTarget_0059ce60[obj] = target;
}

/* Function start: 0x423AC0 */
int target_valid(short obj)
{
    return unactive((short)g_acShipTarget_0059ce60[obj]) == 0 &&
           g_aeShipSide_0059d650[
               (short)g_acShipTarget_0059ce60[obj]] !=
               g_aeShipSide_0059d650[obj];
}

/* Function start: 0x423B00 */
unsigned int triumph(short obj)
{
    unsigned int result = 0;
    short objective;

    switch (g_aeShipMissionType_0059c3f0[obj]) {
    case MISSION_TYPE_PATROL:
        objective = 0;
        while (g_abFlightPath_0059c000[objective] != -1 &&
               (short)visited(
                   (short)g_abFlightPath_0059c000[objective]) != 0)
            objective++;
        return 1;
    case MISSION_TYPE_ESCORT:
    case MISSION_TYPE_DEFEND:
    case MISSION_TYPE_WINGMAN:
        return dead_ship(g_anShipMissionShip_0059d4b0[obj]) == 0;
    case MISSION_TYPE_STRIKE:
        result = dead_ship(g_anShipMissionShip_0059d4b0[obj]);
    }
    return result;
}

/* Function start: 0x423BA0 */
short find_ratio(short inputMinimum, short inputMaximum,
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
    ObjectTypeData *typeData =
        &g_aObjectTypeData_00466458[g_aeObjectType_0059b560[obj]];

    if (g_aeObjectClass_0059d100[obj] < OBJECT_CLASS_SHIP)
        return 100;
    return (short)((g_acShipDamage_0059c460[obj] * -26) /
                       typeData->damageCapacity +
                   (g_aasShipArmor_0059d420[obj][1] * 27) /
                       typeData->armorRear +
                   (g_aasShipArmor_0059d420[obj][0] * 23) /
                       typeData->armorFront +
                   (g_aasShipArmor_0059d420[obj][2] * 12) /
                       typeData->armorLeft +
                   (g_aasShipArmor_0059d420[obj][3] * 12) /
                       typeData->armorRight + 26);
}

/* Function start: 0x423CD0 */
short mine_available(short obj)
{
    return find_weapon(obj, OBJECT_TYPE_SPACE_MINE);
}

/* Function start: 0x423CE0 */
int LoadShapeSet(PacketResourceDescriptor *resources,
                 unsigned short flags, short defaultLogicalFile)
{
    short logicalFile;

    while (resources->resource != 0) {
        if (*resources->resource == 0) {
            logicalFile = resources->logicalFile;
            if (logicalFile == -1)
                logicalFile = defaultLogicalFile;
            *resources->resource = (unsigned char *)FetchDiskPacketRetrying(
                logicalFile, resources->section, flags);
            if (*resources->resource == 0)
                return 0;
        }
        resources++;
    }
    return 1;
}

/* Function start: 0x423D50 */
int FreeShapeSet(PacketResourceDescriptor *resources, short releaseFlags)
{
    while (resources->resource != 0) {
        if (*resources->resource != 0)
            FreePacketAndClear((int *)resources->resource, releaseFlags);
        resources++;
    }
    return 0;
}

/* Function start: 0x423D80 */
int LoadPacketResourceList(PacketResourceDescriptor *resources,
                           short flags, int availableBytes)
{
    int packetSize;

    while (resources->resource != 0) {
        if (*resources->resource == 0) {
            PromptInsertNumberedDisk(resources->logicalFile);
            packetSize = (int)GetPacketSize(
                g_pDiskFileRecords_005a7cf0[
                    resources->logicalFile].name,
                resources->section);
            if (packetSize < availableBytes) {
                *resources->resource =
                    (unsigned char *)FetchDiskPacketRetrying(
                        resources->logicalFile, resources->section,
                        (unsigned short)flags);
                if (*resources->resource == 0)
                    return availableBytes;
                availableBytes -= packetSize;
            }
        }
        resources++;
    }
    return availableBytes;
}

/* Function start: 0x423E10 */
unsigned int ResetCockpitPaletteEntries(void)
{
    short black[3];
    short index;

    black[0] = 0;
    black[1] = 0;
    black[2] = 0;
    index = 0;
    do {
        memcpy(g_aPaletteFadeEntries_005a76d0[index], black,
               sizeof(black));
        SetPaletteEntry((short)(index + 185),
                        g_aPaletteFadeEntries_005a76d0[index]);
        index++;
    } while (index < 6);
    memcpy(DAT_005a7780, black, sizeof(black));
    DAT_005a7780[2] = 32;
    SetPaletteEntry((short)DAT_004699d8, DAT_005a7780);
    return 0;
}

/* Function start: 0x423E90 */
unsigned int initialize_cockpit(signed char mode)
{
    Viewport savedScreen;
    unsigned char *backdrop;
    unsigned int result;

    if (message_showing())
        EndCommMenu();

    if (DAT_0046a008 == 0 && mode == g_cScreenViewportMode_0059a9f2) {
        if (DAT_005a7510.pixels != 0)
            ClearViewport(&DAT_005a7510, DAT_004699d8);
        else
            initialize_view_buffer();
        return 0;
    }

    GetScreenUpdateFlag();
    savedScreen = DAT_005a6ba0;
    DAT_005a6ba0.left = 0;
    DAT_005a6ba0.top = 0;
    DAT_005a6ba0.right = 319;
    DAT_005a6ba0.bottom = 199;
    ClearViewport(&g_stModalSourceViewport_005a7670, DAT_0046999c);

    g_cCockpitLogicalFile_005a7c74 =
        (unsigned char)(g_cCockpitView_0059dab0 + 17);
    g_cScreenViewportMode_0059a9f2 = mode;
    switch (mode) {
    case 0:
        if (g_apCockpitShapes_005a7c80[0] != 0) {
            if (DAT_0046a008 == 0)
                DrawSpriteDefault(&DAT_005a6ba0, 0, 0,
                                  g_apCockpitShapes_005a7c80[0], 0);
        } else {
            backdrop = (unsigned char *)FetchDiskPacketRetrying(
                (short)g_cCockpitLogicalFile_005a7c74, 0, 0);
            if (DAT_0046a008 == 0)
                DrawSpriteDefault(&DAT_005a6ba0, 0, 0, backdrop, 0);
            ReleasePacketHandle((int)backdrop);
        }
        ResetCockpitPaletteEntries();
        if (DAT_0046a008 == 0)
            explosion_draw();
        reset_cockpit();
        InvalidateVduMode(0);
        InvalidateVduMode(1);
        update_VDUs();
        clear_head_up_display();
        if (DAT_0046a008 == 0)
            ResetPilotHandAnimation();
        set_up_screen_viewport(0);
        break;
    case 1:
        if (g_apCockpitShapes_005a7c80[1] != 0) {
            if (DAT_0046a008 == 0)
                DrawSpriteDefault(&DAT_005a6ba0, 0, 0,
                                  g_apCockpitShapes_005a7c80[1], 0);
        } else {
            backdrop = (unsigned char *)FetchDiskPacketRetrying(
                (short)g_cCockpitLogicalFile_005a7c74, 1, 0);
            if (DAT_0046a008 == 0)
                DrawSpriteDefault(&DAT_005a6ba0, 0, 0, backdrop, 0);
            ReleasePacketHandle((int)backdrop);
        }
        set_up_screen_viewport(1);
        break;
    case 2:
        if (g_apCockpitShapes_005a7c80[2] != 0) {
            if (DAT_0046a008 == 0)
                DrawSpriteDefault(&DAT_005a6ba0, 0, 0,
                                  g_apCockpitShapes_005a7c80[2], 0);
        } else {
            backdrop = (unsigned char *)FetchDiskPacketRetrying(
                (short)g_cCockpitLogicalFile_005a7c74, 2, 0);
            if (DAT_0046a008 == 0)
                DrawSpriteDefault(&DAT_005a6ba0, 0, 0, backdrop, 0);
            ReleasePacketHandle((int)backdrop);
        }
        set_up_screen_viewport(2);
        break;
    case 3:
        if (g_apCockpitShapes_005a7c80[3] != 0) {
            if (DAT_0046a008 == 0)
                DrawSpriteDefault(&DAT_005a6ba0, 0, 0,
                                  g_apCockpitShapes_005a7c80[3], 0);
        } else {
            backdrop = (unsigned char *)FetchDiskPacketRetrying(
                (short)g_cCockpitLogicalFile_005a7c74, 3, 0);
            if (DAT_0046a008 == 0)
                DrawSpriteDefault(&DAT_005a6ba0, 0, 0, backdrop, 0);
            ReleasePacketHandle((int)backdrop);
        }
        set_up_screen_viewport(3);
        break;
    case 4:
        if (g_bIntroSceneResourcesActive_00469d48 == 1) {
            if (g_pCinematicViewBackdrop_005a7c90 != 0) {
                DrawSpriteDefault(&DAT_005a6ba0, 0, 0,
                                  g_pCinematicViewBackdrop_005a7c90, 0);
                if (DAT_0046a008 < 1) {
                    DIBslam();
                    DIBslamReal();
                }
            } else {
                backdrop = (unsigned char *)FetchDiskPacketRetrying(8, 6, 0);
                if (backdrop != 0) {
                    DrawSpriteDefault(&DAT_005a6ba0, 0, 0, backdrop, 0);
                    if (DAT_0046a008 < 1) {
                        DIBslam();
                        DIBslamReal();
                    }
                    ReleasePacketHandle((int)backdrop);
                }
            }
        }
        set_up_screen_viewport(4);
        break;
    case 5:
        set_up_screen_viewport(4);
        break;
    case 6:
        set_up_screen_viewport(5);
        break;
    case 7:
        if (g_pRearViewBackdrop_005a7c94 != 0) {
            if (DAT_0046a008 == 0)
                DrawSpriteDefault(&DAT_005a6ba0, 0, 0,
                                  g_pRearViewBackdrop_005a7c94, 0);
        } else {
            backdrop = (unsigned char *)FetchDiskPacketRetrying(8, 7, 0);
            if (DAT_0046a008 == 0)
                DrawSpriteDefault(&DAT_005a6ba0, 0, 0, backdrop, 0);
            if (backdrop != 0)
                ReleasePacketHandle((int)backdrop);
        }
        set_up_screen_viewport(0);
        break;
    }

    DAT_0046a004 = 1;
    SetViewportRect(&DAT_005a7510, 0, 0,
                    (unsigned short)(g_nScreenWidth_0046daa4 - 1),
                    (unsigned short)(g_nScreenHeight_0046daa8 - 1));
    result = initialize_view_buffer();
    DAT_005a6ba0 = savedScreen;
    return result;
}

/* Function start: 0x4242D0 */
unsigned int InitializeConstellationObject(
    const ConstellationObjectDefinition *definition, short object)
{
    FixedVector position;

    g_aeObjectClass_0059d100[object] = OBJECT_CLASS_PLANET;
    init_ijk(63);
    alter_yaw((short)-definition->yaw, 63);
    alter_pitch((short)-definition->pitch, 63);
    alter_roll(definition->roll, 63);
    ScaleFixedVector(&g_aShipForwardVector_0059bce0[63],
                     0x753000, &position);
    g_aShipPosition_0059c490[object] = position;
    g_asObjectScreenScale_0059c950[object] = 0xff;
    g_asObjectScreenAngle_0059cd90[object] = 0;
    g_asObjectViewFrame_0059d230[object] = 0;
    g_aeObjectType_0059b560[object] = OBJECT_TYPE_HORNET;
    g_apObjectShape_0059d2f0[object] =
        (unsigned char *)FetchDiskPacketRetrying(
            12, (short)(definition->shapePacket + 1), 0);
    return 0;
}

/* Function start: 0x4243B0 */
unsigned int FreeConstellationObject(short object)
{
    FreePacketAndClear((int *)&g_apObjectShape_0059d2f0[object], 0);
    remove_object(object);
    return 0;
}

/* Function start: 0x4243E0 */
void init_constellation(short scene)
{
    short constellation;
    short object;
    short slot;
    int definitionBase;

    if (g_pConstellationShape_005a765c == 0)
        goto load_constellation;
    return;

load_constellation:
    constellation = scene;
    constellation--;
    g_pConstellationShape_005a765c =
        (unsigned char *)FetchDiskPacketRetrying(12, 0, 0);
    if (g_nTrainSimActive_00469e2c != 0 || constellation < 0)
        return;

    definitionBase = (int)constellation * 4;
    slot = 0;
    do {
        if (g_pConstellationDefinitions_00598a28[
                definitionBase + slot].shapePacket != -1 &&
            (object = find_vacant_3d_object()) != -1) {
            InitializeConstellationObject(
                &g_pConstellationDefinitions_00598a28[
                    definitionBase + slot],
                object);
            g_asConstellationObjectIndices_00469d50[slot] = object;
        } else {
            g_asConstellationObjectIndices_00469d50[slot] = -1;
        }
        slot++;
    } while (slot < 4);
}

/* Function start: 0x424490 */
unsigned int free_constellation(void)
{
    short object;
    short slot;

    slot = 0;
    FreePacketAndClear((int *)&g_pConstellationShape_005a765c, 0);
    do {
        object = g_asConstellationObjectIndices_00469d50[slot];
        if (object != -1) {
            FreeConstellationObject(object);
            g_asConstellationObjectIndices_00469d50[slot] = -1;
        }
        slot++;
    } while (slot < 4);
    return 0;
}

/* Function start: 0x4244E0 */
void init_vdus(void)
{
    DAT_005a7530 = DAT_005a6ba0;
    DAT_005a6b80 = DAT_005a6ba0;

    InitializeTextContextFromFont(
        &DAT_005a74f0, 2, DAT_004699b4, (signed char)DAT_0046999c);
    DAT_005a74f0.viewport = &DAT_005a6b80;
    DAT_005a74f0.text = g_szDefaultTextBuffer_005a7590;
    set_mode(0, 1);
    DAT_0059ce18[0] = 0;

    InitializeTextContextFromFont(
        &DAT_005a7700, 2, DAT_004699b4, (signed char)DAT_0046999c);
    DAT_005a7700.viewport = &DAT_005a7530;
    DAT_005a7700.text = g_szDefaultTextBuffer_005a7590;
    set_mode(1, g_nTrainSimActive_00469e2c == 0 ? 5 : 3);
    DAT_0059ce18[1] = 0;
}

/* Function start: 0x4245B0 */
unsigned int InitializeCockpitResources(signed char mode)
{
    const CockpitLayout *layout;
    const ShortPoint *origin;
    const ShortRect *bounds;
    ShipWeaponSlot *weaponSlot;
    int backgroundSize;
    unsigned int result;
    short maximumSize;
    short frame;
    short weapon;

    if (g_bCockpitResourcesActive_00469d58 == 1) {
        if (mode == g_cCockpitView_0059dab0)
            return 0;
        free_cockpit();
    }

    g_bCockpitResourcesActive_00469d58 = 1;
    g_cCockpitView_0059dab0 = mode;
    g_cCockpitLogicalFile_005a7c74 = (unsigned char)(mode + 17);
    clear_cockpit_damage();
    ClearHudGunReadouts();
    reset_cockpit();
    GetScreenUpdateFlag();

    LoadShapeSet(g_aCockpitPrimaryResources_00469d08, 4,
                 (short)g_cCockpitLogicalFile_005a7c74);
    g_pScreenViewportPacket_005a6b94 =
        (ScreenViewportPacket *)LoadPacketAllocated(
            (short)g_cCockpitLogicalFile_005a7c74, 6);
    DAT_005a6be0 = DAT_005a6ba0;
    init_vdus();

    InitializeTextContextFromFont(
        &DAT_005a7720, 2, DAT_004699b4, (signed char)DAT_0046999c);
    DAT_005a7720.viewport = &DAT_005a6ba0;
    SetTextContext(&DAT_005a7720);

    layout = &g_stCockpitLayout_0046e000;
    origin = &layout->readoutOrigins[0][(int)g_cCockpitView_0059dab0];
    SetTextCursor((unsigned short)origin->x, (unsigned short)origin->y);
    InitializeCockpitReadout(4, &DAT_005a7720);
    origin = &layout->readoutOrigins[1][(int)g_cCockpitView_0059dab0];
    SetTextCursor((unsigned short)origin->x, (unsigned short)origin->y);
    InitializeCockpitReadout(5, &DAT_005a7720);
    origin = &layout->readoutOrigins[2][(int)g_cCockpitView_0059dab0];
    SetTextCursor((unsigned short)origin->x, (unsigned short)origin->y);
    InitializeCockpitReadout(2, &DAT_005a7720);
    origin = &layout->readoutOrigins[3][(int)g_cCockpitView_0059dab0];
    SetTextCursor((unsigned short)origin->x, (unsigned short)origin->y);
    InitializeCockpitReadout(3, &DAT_005a7720);

    bounds = &layout->leftVduBounds[(int)g_cCockpitView_0059dab0];
    DAT_005a6b80.left = bounds->left;
    DAT_005a6b80.top = bounds->top;
    DAT_005a6b80.right = bounds->right;
    DAT_005a6b80.bottom = bounds->bottom;
    bounds = &layout->rightVduBounds[(int)g_cCockpitView_0059dab0];
    DAT_005a7530.left = bounds->left;
    DAT_005a7530.top = bounds->top;
    DAT_005a7530.right = bounds->right;
    DAT_005a7530.bottom = bounds->bottom;

    if (g_pPilotHandShape_005a7684 != 0) {
        DAT_005a6b60 = DAT_005a6ba0;
        bounds = &layout->pilotHandBounds[(int)g_cCockpitView_0059dab0];
        DAT_005a6b60.left = bounds->left;
        DAT_005a6b60.top = bounds->top;
        DAT_005a6b60.right = bounds->right;
        DAT_005a6b60.bottom = bounds->bottom;
        SetViewportRect(&DAT_005a7690, 0, 0,
                        (unsigned short)(bounds->right - bounds->left),
                        (unsigned short)(bounds->bottom - bounds->top));
        DAT_005a7550 = DAT_005a7690;
        AllocateViewport(&DAT_005a7690, DAT_0046999c, 0);
        AllocateViewport(&DAT_005a7550, DAT_0046999c, 0);
    }

    LoadShapeSet(g_aCockpitSecondaryResources_00469ce0, 0,
                 (short)g_cCockpitLogicalFile_005a7c74);
    backgroundSize = MeasureShapeFrameStorage(
        g_pTargetLockShape_005a6bf4, 2);
    g_pScannerMarkerBackground_005a7dc4 =
        (unsigned char *)AllocateTaggedMemory(backgroundSize, 0);

    g_pReleaseWeaponDisplayBackground_0046906c = 0;
    maximumSize = 0;
    weapon = 0;
    while (weapon < (signed char)g_aShipWeapons_0059cab0[0][0]) {
        weaponSlot = &((ShipWeaponSlot *)
            &g_aShipWeapons_0059cab0[0][1])[weapon];
        if (g_aObjectTypeData_00466458[weaponSlot->type].objectClass ==
                OBJECT_CLASS_MISSILE) {
            backgroundSize = MeasureShapeFrameStorage(
                g_pCockpitWeaponShape_005a7564,
                (short)(weaponSlot->type * 2 - 0x2f));
            maximumSize = MaxShort(maximumSize, (short)backgroundSize);
        }
        weapon++;
    }
    if (maximumSize != 0)
        g_pReleaseWeaponDisplayBackground_0046906c =
            (unsigned char *)AllocateTaggedMemory(maximumSize, 0);

    g_pCockpitExplosionBackground_00469060 = 0;
    if (g_pCockpitExplosionShape_00469064 != 0) {
        maximumSize = 0;
        frame = 0;
        do {
            backgroundSize = MeasureShapeFrameStorage(
                g_pCockpitExplosionShape_00469064, frame);
            maximumSize = MaxShort(maximumSize, (short)backgroundSize);
            frame++;
        } while (frame < 8);
        if (maximumSize != 0)
            g_pCockpitExplosionBackground_00469060 =
                (unsigned char *)AllocateTaggedMemory(maximumSize, 0);
    }

    g_pDamageDisplayBackground_0046a748 = 0;
    maximumSize = 0;
    frame = 0;
    do {
        backgroundSize = MeasureShapeFrameStorage(
            g_pCockpitWeaponShape_005a7564, frame);
        maximumSize = MaxShort(maximumSize, (short)backgroundSize);
        frame++;
    } while (frame < 9);
    if (maximumSize != 0)
        g_pDamageDisplayBackground_0046a748 =
            (unsigned char *)AllocateTaggedMemory(maximumSize, 0);

    ResetScannerContacts();
    result = init_personalities();
    g_nCockpitExplosionFrame_00469068 = 8;
    g_bRadioSilence_0046af70 = 0;
    if (g_nMemoryConfiguration_005a7cd4 == 2)
        DAT_0046af78 = 1;
    else if (g_nSceneResourceBudget_005a7ce4 > 0x59d8)
        DAT_0046af78 = 1;
    else
        DAT_0046af78 = 0;
    return result;
}

/* Function start: 0x4249A0 */
unsigned int free_cockpit(void)
{
    if (g_bCockpitResourcesActive_00469d58 == 0)
        return 0;
    g_bCockpitResourcesActive_00469d58 = 0;
    GetScreenUpdateFlag();
    if (g_pScreenViewportPacket_005a6b94 != 0) {
        ReleasePacketHandle((int)g_pScreenViewportPacket_005a6b94);
        g_pScreenViewportPacket_005a6b94 = 0;
    }
    ReleaseTextFont(2);
    if (g_pPilotHandShape_005a7684 != 0) {
        free_viewport(&DAT_005a7690);
        free_viewport(&DAT_005a7550);
    }
    g_cCockpitLogicalFile_005a7c74 =
        (signed char)(g_cCockpitView_0059dab0 + 17);
    FreeShapeSet(g_aCockpitPrimaryResources_00469d08, 4);
    FreeShapeSet(g_aCockpitSecondaryResources_00469ce0, 0);
    FreePacketAndClear((int *)&g_pCockpitPilotShape_0046905c, 0);
    FreePacketAndClear(
        (int *)&g_pReleaseWeaponDisplayBackground_0046906c, 0);
    FreePacketAndClear((int *)&g_pCockpitExplosionBackground_00469060, 0);
    FreePacketAndClear((int *)&g_pDamageDisplayBackground_0046a748, 0);
    FreeCommDisplayResources();
    ReleasePacketHandle((int)g_pScannerMarkerBackground_005a7dc4);
    return 0;
}

/* Function start: 0x424A80 */
void init_3Space_objects(short scene)
{
    short slot;

    if (DAT_00469d5c == 1)
        return;
    DAT_00469d5c = 1;
    g_cScreenViewportMode_0059a9f2 = -1;
    remove_all_3d_objects();
    g_nExternalViewShip_0046c040 = -1;
    g_nRenderedSpaceFrame_0059d61a = 0;
    g_bScriptedView_0046a8d4 = 0;
    g_nSpaceFrame_0059b420 = 0;
    g_bMissileCameraEnabled_0046c07c = 0;
    g_nClosestVisibleObject_0046c048 = -1;
    g_nPlayerCollisionObject_0046c050 = -1;
    slot = 0;
    do {
        g_aObjectResourceSlots_0059ddf0[slot].type = -1;
        slot++;
    } while (slot < 4);
    init_constellation(scene);
    load_common_3Space_objects();
}

/* Function start: 0x424B00 */
unsigned int load_common_3Space_objects(void)
{
    ObjectTypeData *types = g_aObjectTypeData_00466458;
    unsigned char *debrisShapeSet;
    unsigned char *turretShapeSet;
    unsigned char *turretAnimation;
    unsigned char *missileShapeSet;
    unsigned char *missileAnimation;

    LoadShapeSet(g_aCommon3SpaceResources_00469bc0, 0, -1);
    debrisShapeSet = types[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    turretShapeSet = types[OBJECT_TYPE_LASER_CANNON].shapeSet;
    types[OBJECT_TYPE_DEBRIS_O_RING].shapeSet = debrisShapeSet;
    turretAnimation = types[OBJECT_TYPE_LASER_CANNON].animation;
    types[OBJECT_TYPE_DEBRIS_GLASS].shapeSet = debrisShapeSet;
    types[OBJECT_TYPE_DEBRIS_SHIP_TUBING].shapeSet = debrisShapeSet;
    types[OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK].shapeSet = debrisShapeSet;
    types[OBJECT_TYPE_TURRET].shapeSet = turretShapeSet;
    types[OBJECT_TYPE_TURRET].animation = turretAnimation;

    load_ship(OBJECT_TYPE_HEAT_SEEKING_MISSILE, 3);
    missileShapeSet = types[OBJECT_TYPE_HEAT_SEEKING_MISSILE].shapeSet;
    missileAnimation = types[OBJECT_TYPE_HEAT_SEEKING_MISSILE].animation;
    types[OBJECT_TYPE_DUMB_FIRE_MISSILE].shapeSet = missileShapeSet;
    types[OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE].shapeSet = missileShapeSet;
    types[OBJECT_TYPE_FF_MISSILE].shapeSet = missileShapeSet;
    types[OBJECT_TYPE_DUMB_FIRE_MISSILE].animation = missileAnimation;
    types[OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE].animation = missileAnimation;
    types[OBJECT_TYPE_FF_MISSILE].animation = missileAnimation;
    return 0;
}

/* Function start: 0x424B80 */
void remove_all_3d_objects(void)
{
    short i = 0;

    do {
        remove_object(i);
        i = i + 1;
    } while (i < 0x40);
}

/* Function start: 0x424BA0 */
unsigned int free_3Space(void)
{
    if (DAT_00469d5c == 0)
        return 0;
    DAT_00469d5c = 0;
    GetScreenUpdateFlag();
    DAT_0046a004 = 0;
    free_constellation();
    remove_all_hazards();
    remove_all_3d_objects();
    free_3Space_objects();
    return 0;
}

/* Function start: 0x424BE0 */
unsigned int free_3Space_objects(void)
{
    ObjectTypeData *types = g_aObjectTypeData_00466458;

    FreeShapeSet(g_aCommon3SpaceResources_00469bc0, 0);
    FreeShapeSet(g_aMissionResourceDescriptors_00469c20, 0);
    free_ship(3);
    types[OBJECT_TYPE_DUMB_FIRE_MISSILE].shapeSet = 0;
    types[OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE].shapeSet = 0;
    types[OBJECT_TYPE_FF_MISSILE].shapeSet = 0;
    types[OBJECT_TYPE_DUMB_FIRE_MISSILE].animation = 0;
    types[OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE].animation = 0;
    types[OBJECT_TYPE_FF_MISSILE].animation = 0;
    types[OBJECT_TYPE_DEBRIS_O_RING].shapeSet =
        types[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    types[OBJECT_TYPE_DEBRIS_GLASS].shapeSet =
        types[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    types[OBJECT_TYPE_DEBRIS_SHIP_TUBING].shapeSet =
        types[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    types[OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK].shapeSet =
        types[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    types[OBJECT_TYPE_DEBRIS_WING].shapeSet =
        types[OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet;
    return 0;
}

/* Function start: 0x424C60 */
unsigned int init_inflight_music(void)
{
    g_nCombatMusicActive_0046aa3c = 0;
    g_nInFlightMusicActive_0046aa40 = 1;
    g_nInitialFlightMusicPending_0046aa38 = 1;
    return 0;
}

/* Function start: 0x424C80 */
unsigned int free_inflight_music(void)
{
    int slot;

    StopMusicUnlessSuppressed();
    g_nInFlightMusicActive_0046aa40 = 0;
    if (g_nInFlightMusicSlotA_0046aa24 != -1) {
        slot = g_nInFlightMusicSlotA_0046aa24;
        g_nInFlightMusicSlotA_0046aa24 = -1;
        *(int *)(g_abSoundPlaybackSlots_005a67f0 + slot * 6) = 0;
    }
    if (g_nInFlightMusicSlotB_0046aa2c != -1) {
        slot = g_nInFlightMusicSlotB_0046aa2c;
        g_nInFlightMusicSlotB_0046aa2c = -1;
        *(int *)(g_abSoundPlaybackSlots_005a67f0 + slot * 6) = 0;
    }
    return 0;
}

/* Function start: 0x424CE0 */
unsigned int PreloadMusicTrackHook(short track)
{
    (void)track;
    return 0;
}

/* Function start: 0x424CF0 */
unsigned int ReleaseMusicTrackHook(short track)
{
    (void)track;
    return 0;
}

/* Function start: 0x424D00 */
unsigned int LoadSceneAnimationResources(short scene, short variant)
{
    short logicalFile;

    logicalFile = g_asSceneAnimationLogicalFiles_00469d60[scene];
    g_pSceneAnimationPrimaryShape_005a7c58 =
        (unsigned char *)FetchDiskPacketRetrying(logicalFile, 0, 0);
    g_pSceneAnimationDefinitions_005a7c6c =
        (unsigned char *)FetchDiskPacketRetrying(
            logicalFile, (short)(variant + 1), 0);
    g_pSceneAnimationSecondaryShape_005a7c70 =
        (unsigned char *)FetchDiskPacketRetrying(
            logicalFile, (short)(variant + 3), 0);
    g_pSceneAnimationPacket_005a7c60 =
        (unsigned char *)FetchDiskPacketRetrying(
            logicalFile, (short)(variant + 5), 0);
    g_pSceneAnimationSceneData_005a7c54 =
        g_pSceneAnimationPacket_005a7c60 +
        *(unsigned int *)(g_pSceneAnimationPacket_005a7c60 + 0);
    g_pSceneAnimationTextData_005a7c5c =
        g_pSceneAnimationPacket_005a7c60 +
        *(unsigned int *)(g_pSceneAnimationPacket_005a7c60 + 4);
    g_pSceneAnimationObjects_005a7c64 =
        (SceneAnimationObject *)(g_pSceneAnimationDefinitions_005a7c6c + 2);
    return (unsigned int)g_pSceneAnimationPacket_005a7c60 & 0xffff0000;
}

/* Function start: 0x424DA0 */
void ReleaseSceneAnimationResources(void)
{
    ReleasePacketHandle((int)g_pSceneAnimationPrimaryShape_005a7c58);
    ReleasePacketHandle((int)g_pSceneAnimationPacket_005a7c60);
    ReleasePacketHandle((int)g_pSceneAnimationSecondaryShape_005a7c70);
    ReleasePacketHandle((int)g_pSceneAnimationDefinitions_005a7c6c);
}

/* Function start: 0x424DE0 */
signed char *__stdcall FindSceneAnimationCommand(
    signed char *script, signed char command)
{
    signed char opcode;

    while (*script != 0) {
        opcode = *script++;
        if (opcode == command) {
            script--;
            break;
        }
        switch (opcode) {
        case 'A':
        case 'L':
        case 'Q':
            script++;
        case 'B':
        case 'G':
        case 'J':
        case 'R':
        case 'W':
            script += 2;
            break;
        case 'D':
            while (*script++ != -1) {
            }
            break;
        case 'E':
        case 'P':
        case 'S':
            script++;
            break;
        case 'X':
            script += 10;
            break;
        }
    }
    return *script != 0 ? script : 0;
}

/* Function start: 0x424EA0 */
short __stdcall SceneAnimationGoalReached(short delta, short current,
                                          short goal)
{
    if (delta < 0) {
        if (current >= goal)
            return 1;
    } else if (delta == 0) {
        if (current == goal)
            return 1;
    } else if (current <= goal) {
        return 1;
    }
    return 0;
}

/* Function start: 0x424EF0 */
unsigned int __stdcall UpdateSceneAnimationObject(
    SceneAnimationObject *object, Viewport *viewport)
{
    SceneAnimationObject *source;
    signed char *commandStart;
    signed char *cursor;
    signed char *label;
    signed char opcode;
    signed char property;
    unsigned short complete;
    unsigned short goalFlags;
    short delay;
    short value;
    short frame;
    short xOffset;
    short labelNumber;
    short objectIndex;
    short objectCount;
    signed char stop;

    complete = 0;
    delay = object->delay;
    stop = 0;
    if (delay != 0)
        cursor = object->repeatCursor;
    else
        cursor = object->scriptCursor;

    while (*cursor != 0 && stop == 0) {
        opcode = *cursor++;
        switch (opcode) {
        case 'A':
            property = *cursor++;
            value = *(short *)cursor;
            cursor += 2;
            switch (property) {
            case 'F':
                object->frame = (short)(object->frame + value);
                object->deltaFrame = value;
                break;
            case 'R':
                object->rotation = (short)(object->rotation + value);
                object->deltaRotation = value;
                if (object->rotation < 0)
                    object->rotation = (short)(
                        ((unsigned short)(0x167 - object->rotation) /
                         0x168) * 0x168 + object->rotation);
                if (object->rotation > 0x167)
                    object->rotation = (short)(
                        object->rotation -
                        ((unsigned short)object->rotation / 0x168) *
                            0x168);
                break;
            case 'S':
                object->scale = (short)(object->scale + value);
                object->deltaScale = value;
                if (object->scale < 0x40)
                    object->scale = 0x40;
                else if (object->scale > 0x1fff)
                    object->scale = 0x1fff;
                break;
            case 'T':
                delay = (short)(delay + value);
                break;
            case 'X':
                object->x = (short)(object->x + value);
                object->deltaX = value;
                break;
            case 'Y':
                object->y = (short)(object->y + value);
                object->deltaY = value;
                break;
            }
            break;

        case 'B':
            cursor += 2;
            break;

        case 'D':
            commandStart = cursor - 1;
            object->repeatCursor = commandStart;
            xOffset = 0;
            frame = (short)*cursor++;
            while (frame != -1) {
                if (object->layer != 2 && DAT_00469fb4 < 1)
                    DrawSpriteScaled(
                        viewport, (short)(object->x + xOffset), object->y,
                        object->shape, frame, object->rotation,
                        object->scale, object->frame);
                if (object->layer == 0)
                    xOffset = (short)(xOffset + 320);
                frame = (short)*cursor++;
            }
            stop = 1;
            break;

        case 'E':
            commandStart = cursor - 1;
            xOffset = 0;
            complete = 1;
            frame = (short)*cursor++;
            while (frame != -1) {
                if (object->layer != 2 && DAT_00469fb4 < 1)
                    DrawSpriteScaled(
                        viewport, (short)(object->x + xOffset), object->y,
                        object->shape, frame, object->rotation,
                        object->scale, object->frame);
                xOffset = (short)(xOffset + 320);
                frame = (short)*cursor++;
            }
            cursor = commandStart;
            stop = 1;
            break;

        case 'G':
        case 'J':
            if (opcode == 'J')
                stop = 1;
            labelNumber = *(short *)cursor;
            label = object->scriptStart;
            do {
                label = FindSceneAnimationCommand(label, 'B');
                cursor = label + 3;
                value = *(short *)(label + 1);
                label = cursor;
            } while (value != labelNumber);
            break;

        case 'L':
            property = *cursor++;
            value = *(short *)cursor;
            cursor += 2;
            switch (property) {
            case 'F':
                object->frame = value;
                break;
            case 'R':
                object->rotation = value;
                if (object->rotation < 0)
                    object->rotation = (short)(
                        ((unsigned short)(0x167 - object->rotation) /
                         0x168) * 0x168 + object->rotation);
                if (object->rotation > 0x167)
                    object->rotation = (short)(
                        object->rotation -
                        ((unsigned short)object->rotation / 0x168) *
                            0x168);
                break;
            case 'S':
                object->scale = value;
                if (object->scale < 0x40)
                    object->scale = 0x40;
                else if (object->scale > 0x1fff)
                    object->scale = 0x1fff;
                break;
            case 'T':
                delay = value;
                break;
            case 'X':
                object->x = value;
                break;
            case 'Y':
                object->y = value;
                break;
            }
            break;

        case 'P':
            stop = 1;
            break;

        case 'Q':
            property = *cursor++;
            value = *(short *)cursor;
            cursor += 2;
            switch (property) {
            case 'F':
                object->goalFlags |= 0x10;
                object->goalFrame = value;
                break;
            case 'R':
                object->goalFlags |= 1;
                object->goalRotation = value;
                break;
            case 'S':
                object->goalFlags |= 2;
                object->goalScale = value;
                break;
            case 'X':
                object->goalFlags |= 4;
                object->goalX = value;
                break;
            case 'Y':
                object->goalFlags |= 8;
                object->goalY = value;
                break;
            }
            break;

        case 'R':
            objectCount = *(short *)g_pSceneAnimationDefinitions_005a7c6c;
            objectIndex = (short)(objectCount * (short)*cursor++);
            objectIndex = (short)(objectIndex + (short)*cursor++);
            source = &g_pSceneAnimationObjects_005a7c64[objectIndex];
            object->x = source->x;
            object->y = source->y;
            object->rotation = source->rotation;
            object->scale = source->scale;
            object->frame = source->frame;
            break;

        case 'W':
            g_nSceneAnimationWaitFrames_005a7c68 = *(short *)cursor;
            cursor += 2;
            g_bSceneAnimationWaitCommand_00469d70 = 1;
            break;

        case 'X':
            object->x = *(short *)cursor;
            cursor += 2;
            object->y = *(short *)cursor;
            cursor += 2;
            object->rotation = *(short *)cursor;
            cursor += 2;
            object->scale = *(short *)cursor;
            cursor += 2;
            object->frame = *(short *)cursor;
            cursor += 2;
            break;
        }
    }

    if (object->delay != 0) {
        object->delay--;
        return 0;
    }

    object->scriptCursor = cursor;
    goalFlags = object->goalFlags;
    object->delay = delay;
    if (complete == 0 && goalFlags != 0) {
        if ((goalFlags & 0x10) != 0)
            complete = SceneAnimationGoalReached(
                object->deltaFrame, object->frame, object->goalFrame);
        if ((goalFlags & 4) != 0)
            complete |= SceneAnimationGoalReached(
                object->deltaX, object->x, object->goalX);
        if ((goalFlags & 8) != 0)
            complete |= SceneAnimationGoalReached(
                object->deltaY, object->y, object->goalY);
        if ((goalFlags & 2) != 0)
            complete |= SceneAnimationGoalReached(
                object->deltaScale, object->scale, object->goalScale);
        if ((goalFlags & 1) != 0)
            complete |= SceneAnimationGoalReached(
                object->deltaRotation, object->rotation,
                object->goalRotation);
    }
    return complete;
}

/* Function start: 0x425500 */
void PlaySceneAnimation(char *text, short animation, short duration)
{
    SceneAnimationObject *object;
    SceneAnimationObject *objects;
    unsigned short complete;
    short objectCount;
    short remaining;

    g_nSceneAnimationWaitFrames_005a7c68 = -1;
    complete = 0;
    g_bSceneAnimationWaitCommand_00469d70 = 0;
    AddPCName(text);
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    FormatTextBufferFromStart(g_szSceneAnimationTextFormat_00469d74,
                              0, 160,
                              g_nConversationTextColour_00598c10,
                              g_szTextScratchBuffer_00598b00);

    objectCount = *(short *)g_pSceneAnimationDefinitions_005a7c6c;
    objects = g_pSceneAnimationObjects_005a7c64 +
        (short)(objectCount * animation);
    remaining = objectCount;
    object = objects;
    while (remaining > 0) {
        if (object->layer == 0)
            object->shape = g_pSceneAnimationPrimaryShape_005a7c58;
        else
            object->shape = g_pSceneAnimationSecondaryShape_005a7c70;
        object->scriptStart =
            (signed char *)g_pSceneAnimationDefinitions_005a7c6c +
            object->scriptOffset;
        object->scriptCursor = object->scriptStart;
        object++;
        remaining--;
    }

    DAT_00469fb4 = 1;
    DAT_0059ab58 = 0;
    ClearInputKeyState();
    DIBslam();
    DIBslamReal();
    for (;;) {
        do {
            DAT_00469fb4--;
            object = objects;
            remaining = objectCount;
            while (remaining > 0) {
                complete |= (unsigned short)
                    UpdateSceneAnimationObject(object, &DAT_005a76b0);
                object++;
                remaining--;
            }
            if (g_nSceneAnimationWaitFrames_005a7c68 != -1) {
                if (g_nSceneAnimationWaitFrames_005a7c68 == 0)
                    complete++;
                else
                    g_nSceneAnimationWaitFrames_005a7c68--;
            }
            if (complete == 0) {
                RefreshMemoryStatusOverlay();
                DIBslam();
                DIBslamReal();
            }
            if (DAT_00469fb4 == 0) {
                DAT_00469fb4 = g_nFrameSkip_00469fb8;
                if (g_bSlowSceneAnimation_00469998 != 0)
                    DAT_00469fb4++;
            }
            if ((complete == 0 && CheckEscaped() != 0) ||
                DAT_0059ab58 != 0) {
                if (g_nSceneAnimationWaitFrames_005a7c68 == -1) {
                    while (complete == 0 &&
                           g_bSceneAnimationWaitCommand_00469d70 == 0) {
                        object = objects;
                        remaining = objectCount;
                        while (remaining > 0) {
                            DAT_00469fb4 = 2;
                            complete |= (unsigned short)
                                UpdateSceneAnimationObject(
                                    object, &DAT_005a76b0);
                            object++;
                            remaining--;
                        }
                    }
                }
                g_nSceneAnimationWaitFrames_005a7c68 = 0;
            }
        } while (complete == 0);

        if (g_nSceneAnimationWaitFrames_005a7c68 == -1) {
            SetFrameTimerPeriodDirect((short)(duration / 2));
            do {
                if (IsFrameTickElapsed() != 0 || CheckEscaped() != 0)
                    break;
            } while (DAT_0059ab58 == 0);
        }
        if (complete != 0)
            return;
    }
}
