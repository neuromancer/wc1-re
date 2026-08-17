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

static char g_szTemporaryCampaignGlobalsFile_00499bc0[] = "TEMPGLOB.000";

/* Function start: WC2_UNMAPPED */
short find_weapon(short obj, enum ObjectType weaponType)
{
    short weapon;

    for (weapon = 0;
         weapon < (signed char)g_aShipWeapons_0059cab0[obj][0];
         weapon++) {
        ShipWeaponSlot *weaponSlot =
            &((ShipWeaponSlot *)&g_aShipWeapons_0059cab0[obj][1])[weapon];

        if (weaponSlot->type == weaponType)
            return weapon;
    }
    return -1;
}

/* Function start: 0x41670E */
short fire_missile(short ship)
{
    ShipWeaponSlot *slot;
    short weapon;

    slot = (ShipWeaponSlot *)&g_aShipWeapons_0059cab0[ship][1];
    weapon = 0;
    while (weapon < (signed char)g_aShipWeapons_0059cab0[ship][0]) {
        if (g_aObjectTypeData_00496d30[slot->weaponType].objectClass ==
                OBJECT_CLASS_MISSILE) {
            if (ship != 0) {
                if (slot->type != 0x13 ||
                    IsCapitalShipObject(g_acShipTarget_00495f20[ship]) != 0)
                    return fire_weapon(ship, weapon);
            } else if (slot->disabled == 0) {
                if ((slot->type == 0x10 || slot->type == 0x12 ||
                     slot->type == 0x13) &&
                    g_nTargetLockCountdown_004934ec != 0) {
                    if (get_mode(0) == 1)
                        ShowComponentHitHudMessage(
                            g_szNeedLock_0046998c,
                            g_ucHudHighlightColour_0049cb58, 3);
                    return -1;
                }
                return fire_weapon(0, weapon);
            }
        }
        weapon++;
        slot++;
    }
    return -1;
}

/* Function start: 0x41687F */
void fire_fixed_projectile_weapon(short obj)
{
    ShipWeaponSlot *slot;
    short weapon;

    slot = (ShipWeaponSlot *)&g_aShipWeapons_0059cab0[obj][1];
    weapon = 0;
    for (;
         weapon < (signed char)g_aShipWeapons_0059cab0[obj][0] &&
         (g_aObjectTypeData_00496d30[slot->weaponType].objectClass !=
              OBJECT_CLASS_PROJECTILE ||
          slot->type == 0x0b || slot->disabled != 0 ||
          fire_weapon(obj, weapon) != -1);
         weapon++, slot++)
        ;
}

/* Function start: WC2_UNMAPPED */
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
    g_asObjectCounter_00494be0[mine] = (short)(signed char)lifetime;
    if (obj == 0)
        RemovePlayerReleaseWeapon(weapon);
    else
        remove_weapon(obj, weapon);
    return mine;
}

/* Function start: 0x41693A */
void fire_afterburner(short obj, short time)
{
    register short maximumVelocity;

    if (g_acObjectType_00493980[obj] == 0x33)
        return;
    maximumVelocity = get_ship_max_velocity(obj);
    if (maximumVelocity * 0x500L >
        Vector_magnitude(&g_aShipVelocity_0059c010[obj])) {
        set_special(obj, SPECIAL_MANEUVER_AFTERBURNER);
        if (g_aeSpecialManeuver_00495600[obj] !=
            SPECIAL_MANEUVER_AFTERBURNER)
            time = 0;
        g_asShipAfterburnerTimer_0059c810[obj] = time;
    }
}

/* Function start: 0x4169CC */
unsigned int fire_super_brake(short ship)
{
    g_asShipAfterburnerTimer_0059c810[ship] = 10;
    set_special(ship, SPECIAL_MANEUVER_SUPER_BRAKE);
    return 0;
}

/* Function start: 0x4169F3 */
short flip_angle(short ship, short angle)
{
    short flip;

    flip = g_asObjectFlip_0059c870[ship];
    if ((flip & 0x10) != 0)
        angle = (short)(180 - angle);
    if ((flip & 0x20) != 0)
        angle = (short)-angle;
    angle += g_asObjectScreenAngle_0059cd90[ship];
    angle %= 360;
    if (angle < 0)
        angle += 360;
    return angle;
}

/* Function start: 0x416A87 */
unsigned int place_exhaust_on_ships(void)
{
    int shipIndex;
    short *animation;
    short frame;
    short scale;
    short object;
    short ship;

    ship = 0;
    do {
        shipIndex = (int)ship;
        if (g_aeObjectClass_00495328[shipIndex] >= OBJECT_CLASS_MISSILE &&
            g_anShipSpeed_0059b320[shipIndex] != 0 &&
            g_aeSpecialManeuver_00495600[shipIndex] !=
                SPECIAL_MANEUVER_KILL_ENGINES &&
            g_asObjectScreenX_00493598[shipIndex] != (short)0x8001) {
            animation = (short *)g_aObjectTypeData_00496d30[
                g_acObjectType_00493980[shipIndex]].animation;
            if (animation != 0) {
                object = animation[g_asObjectViewFrame_0059d230[shipIndex]];
                if (object != -1) {
                    animation = (short *)((unsigned char *)animation + object);
                    while (*animation != -1) {
                        object = find_vacant_3d_object();
                        if (object == -1)
                            return 0;
                        set_objects_data(object, OBJECT_TYPE_THRUSTERS,
                                         ship);
                        frame = *animation++;
                        scale = *animation++;
                        scale -= RandomInRange(0, 32);
                        if (g_abShipExhaustHeat_0059d610[shipIndex] == 0)
                            scale = (short)(scale - 32);
                        g_asObjectScale_0059de40[object] = scale;
                        g_asObjectDistance_0059b4a0[object] = *animation++;
                        g_asObjectScreenAngle_0059cd90[object] =
                            flip_angle(ship, *animation++);
                        g_asObjectFlip_0059c870[object] = 0;
                        g_asObjectScreenX_00493598[object] = *animation++;
                        g_asObjectScreenY_00493628[object] = *animation++;
                        if (g_aeSpecialManeuver_00495600[shipIndex] ==
                                SPECIAL_MANEUVER_AFTERBURNER) {
                            g_asObjectViewFrame_0059d230[object] =
                                (short)(frame * 3 +
                                        RandomInRange(0, 2));
                        } else {
                            g_asObjectViewFrame_0059d230[object] =
                                (short)(frame * 2 + 12 +
                                        RandomInRange(0, 1));
                        }
                    }
                }
            }
        }
        ship++;
    } while (ship < 10);
    /* The original leaves EAX incidental after scanning every ship. */
}

/* Function start: 0x416DC3 */
unsigned int reposition_fixed_child_objects(void)
{
    int objectIndex;
    int parentIndex;
    int right;
    int up;
    long sine;
    long cosine;
    short parentScale;
    short angle;
    short parent;
    short object;
#ifdef WC1_SDL
    int fixedCosine;
    int fixedSine;
    float attachmentRight;
    float attachmentUp;
    float cosineFloat;
    float parentScreenX;
    float parentScreenY;
    float sineFloat;
#endif

    object = 10;
    do {
        objectIndex = (int)object;
        if (g_aeObjectClass_00495328[objectIndex] ==
                OBJECT_CLASS_FIXED_OBJECT) {
            parent = (short)g_acObjectOwner_00495208[objectIndex];
            if (g_acObjectType_00493980[objectIndex] ==
                    OBJECT_TYPE_TURRET ||
                g_acObjectType_00493980[objectIndex] ==
                    OBJECT_TYPE_THRUSTERS) {
                parentIndex = (int)parent;
                angle = g_asObjectScreenAngle_0059cd90[parentIndex];
                sine = SinFixed(angle);
                cosine = CosFixed(angle);
                parentScale =
                    g_asObjectScreenScale_0059c950[parentIndex];
                right = (int)g_asObjectScreenX_00493598[objectIndex] *
                        (unsigned short)parentScale;
                g_asObjectDistance_0059b4a0[objectIndex] +=
                    g_asObjectDistance_0059b4a0[parentIndex];
                if ((g_asObjectFlip_0059c870[parentIndex] & 0x10) != 0)
                    right = -right;
                up = (int)g_asObjectScreenY_00493628[objectIndex] *
                     (unsigned short)parentScale;
                if ((g_asObjectFlip_0059c870[parentIndex] & 0x20) != 0)
                    up = -up;
                g_asObjectScreenX_00493598[objectIndex] = (short)(
                    (MultiplyFixed(right, (int)cosine) -
                     MultiplyFixed(up, (int)sine)) >> 8);
                g_asObjectScreenY_00493628[objectIndex] = (short)(
                    (MultiplyFixed(up, (int)cosine) +
                     MultiplyFixed(right, (int)sine)) >> 8);
                g_asObjectScreenX_00493598[objectIndex] +=
                    g_asObjectScreenX_00493598[parentIndex];
                g_asObjectScreenY_00493628[objectIndex] +=
                    g_asObjectScreenY_00493628[parentIndex];
#ifdef WC1_SDL
                if (g_acObjectType_00493980[objectIndex] ==
                        OBJECT_TYPE_THRUSTERS) {
                    /* Match the anchor to the enhanced parent transform. */
                    parentScreenX =
                        (float)g_nViewCenterX_0059a852 +
                        (float)(((double)(g_nScreenWidth_0046daa4 & ~1) *
                                 0.5 *
                                 g_aObjectViewPosition_0059afa0[parentIndex].x) /
                                g_aObjectViewPosition_0059afa0[parentIndex].z);
                    parentScreenY =
                        (float)g_nViewCenterY_0059a854 +
                        (float)(((double)(g_nScreenWidth_0046daa4 & ~1) *
                                 0.5 *
                                 g_aObjectViewPosition_0059afa0[parentIndex].y) /
                                g_aObjectViewPosition_0059afa0[parentIndex].z);
                    GetRLETransformTrig((int)angle * 10,
                                        &fixedCosine, &fixedSine);
                    cosineFloat = (float)fixedCosine / 65536.0f;
                    sineFloat = (float)fixedSine / 65536.0f;
                    attachmentRight = (float)right / 256.0f;
                    attachmentUp = (float)up / 256.0f;
                    Wc1SdlSetThrusterScreenPosition(
                        object,
                        parentScreenX + attachmentRight * cosineFloat -
                            attachmentUp * sineFloat,
                        parentScreenY + attachmentRight * sineFloat +
                            attachmentUp * cosineFloat);
                }
#endif
            }
            parentIndex = (int)parent;
            g_asObjectScreenScale_0059c950[objectIndex] = (short)(
                (unsigned short)
                    g_asObjectScreenScale_0059c950[parentIndex] *
                (unsigned short)
                    g_asObjectScale_0059de40[objectIndex] >> 8);
        }
        object++;
    } while (object <= WC1_SPACE_LAST_MOVING_OBJECT);
    return 0;
}

/* Function start: 0x4170F9 */
unsigned int housekeep_power_plant_and_fuel(short ship)
{
    if (0 < g_anShipSpeed_0059b320[ship])
        drain_fuel(ship, 5);
    return 0;
}

/* Function start: 0x417124 */
void replenish_shields(short ship)
{
    signed char shield;

    if (ship == 0 && g_acPlayerComponentDamage_00493470[1] > 0 &&
        g_nSpaceFrame_00493134 %
            (g_acPlayerComponentDamage_00493470[1] + 1) != 0)
        return;
    shield = 0;
    do {
        if (g_aasShipShield_00495518[ship][shield] >
            g_aasShipMaximumShield_004954f0[ship][shield])
            g_aasShipShield_00495518[ship][shield] =
                g_aasShipMaximumShield_004954f0[ship][shield];
        if (g_aasShipShield_00495518[ship][shield] <
                g_aasShipMaximumShield_004954f0[ship][shield] &&
            g_nSpaceFrame_00493134 %
                g_aObjectTypeData_00496d30[
                    g_acObjectType_00493980[ship]].animationDelay == 0) {
            g_aasShipShield_00495518[ship][shield]++;
        }
        shield++;
    } while (shield <= 1);
}

/* Function start: 0x41724A */
unsigned int replenish_weapon_energy_bank(short ship)
{
    short energy;
    short shieldEnergy;
    short maximumShield;

    if (ship == 0 && g_acPlayerComponentDamage_00493470[1] != 0 &&
        (int)(unsigned short)RandomInRange(0, 4) <
            g_acPlayerComponentDamage_00493470[1])
        return 0;
    energy = g_asShipWeaponEnergy_0059d470[ship];
    if (energy < 100) {
        shieldEnergy = g_aasShipShield_00495518[ship][1];
        maximumShield = g_aasShipMaximumShield_004954f0[ship][1];
        shieldEnergy =
            (short)(shieldEnergy + g_aasShipShield_00495518[ship][0]);
        maximumShield =
            (short)(maximumShield +
                    g_aasShipMaximumShield_004954f0[ship][0]);
        if (shieldEnergy < maximumShield) {
            g_asShipWeaponEnergy_0059d470[ship] =
                MinShort((short)(energy + 1), 100);
        } else {
            g_asShipWeaponEnergy_0059d470[ship] =
                MinShort((short)(energy + 2), 100);
        }
    }
    return 0;
}

/* Function start: 0x41740F */
void accelerate(short amount)
{
    if (malf(0) != 0) {
        amount = (short)(amount - 2);
        if (g_nSpaceFrame_00493134 % 3 == 0)
            PlaySfxWaveFileByNumber(3, -1, 0);
    }
    celerate(0, (int)amount << 8);
}

/* Function start: 0x417471 */
void your_afterburner(void)
{
    short time;
    int frame;
    int nextSoundFrame;

    if (g_anShipFuel_0059b470[0] <= 0)
        return;
    if (malf(0) != 0) {
        PlaySfxWaveFileByNumber(3, -1, 0);
        return;
    }
    if (g_aeSpecialManeuver_00495600[0] !=
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
    frame = (int)g_nSpaceFrame_00493134;
    nextSoundFrame = frame + 6;
    if (nextSoundFrame < g_nAfterburnerSoundDeadline_005a7ce8)
        g_nAfterburnerSoundDeadline_005a7ce8 = 0;
    if (g_nAfterburnerSoundDeadline_005a7ce8 < frame) {
        g_nAfterburnerSoundDeadline_005a7ce8 = nextSoundFrame;
        PlaySfxWaveFileByNumber(12, -1, 0);
    }
}

/* Function start: 0x45B810 */
unsigned int LoadGamePaletteFile(void)
{
    short index;

    PromptInsertNumberedDisk(0);
    switch ((int)(short)g_nSpacePaletteFadeMode_004901e8) {
    case 9:
    case 13:
        index = 0;
        do {
            ((unsigned char *)&DAT_004699a4)[index] =
                g_abLegacyVideoModeColours_004699e0[index];
            index++;
        } while ((unsigned int)(int)index < 14);
        index = 0;
        do {
            g_asConversationTextColours_004699f0[index] =
                g_asConversationTextColours_004699f0[index + 12];
            index++;
        } while ((unsigned int)(int)index < 12);
        return 0;
    case 0x13:
        LoadPaletteTripletsFile("game.pal");
        ResetCockpitPaletteEntries();
        SaveGamePalette();
        return 0;
    }
    return 0;
}

/* Function start: 0x45B905 */
unsigned int EMShutDown(void)
{
    if (g_nEventManagerActive_0059a850 != 0)
        ShutdownEventManager();
    return 0;
}

/* Function start: WC2_UNMAPPED */
unsigned short InitializeEventManagerResources(void)
{
    DAT_0059a9f0 = 20;
    g_nInputTickScale_0059af90 = 20;
    DAT_0059ab64 = 1;
    g_stMouseCursorState_0059ab10.shape = g_pMouseCursorResource_005a7cdc =
        FetchDiskPacketRetrying(14, 0, 0x10);
    g_stMouseCursorState_0059ab10.frame = 0;
    g_stMouseCursorState_0059ab10.viewport = &g_stScreenViewport_005d21a0;
    return 0;
}

/* Function start: 0x45B924 */
unsigned int EMStartUp(void)
{
    PromptInsertNumberedDisk(0);
    RegisterEventManagerShutdown((void (*)(void))LogMemoryUsage);
    if (InitializeEventManager(20, InitializeEventManagerResources, 0) == 0)
        exit_squadron("EMStartUp Failed");
    ConfigureEventManagerPointer(g_stMouseCursorState_0059ab10.shape, 0);
    SetEventManagerPump(PollJoystickButtonEvents);
    g_nMenuInputRepeatDelay_005a8208 = 6;
    return 0;
}

/* Function start: 0x45B9D3 */
unsigned int LoadOriginFxDrivers(void)
{
    int memoryThreshold;
    int videoModeMemory;
    short requestedGraphicsMode;

    memoryThreshold = 100000;
    _chdir("gamedat");
    g_nNearHeapMaxDescriptors_004688c4 = 0x80;
    IsSoundHardwarePresent(8);
    if (DAT_0059a856 == 0)
        SystemDebugPrintf("No ");
    SystemDebugPrintf("Expanded Memory Detected.\n");
    g_bGraphicsActive_00469a20 = 0;
    PromptInsertNumberedDisk(0x38);
    if (GetMusicDriverPresent(g_bSlowSceneAnimation_00469998) == 0)
        exit_squadron("Failed to load Origin-FX drivers");
#if 0
    RewriteDiskFileGraphicsExtensions(g_bSlowSceneAnimation_00469998);
#endif
    LoadJoystickCalibrationFile(9, 9, 1, 1);
    g_nInputDoubleClickInterval_0046af54 = 2;
    EMStartUp();
    GetFxDriverInitResult();
    g_dwOriginalFreeMemory_005a7cd8 = GetLargestFreeMemoryBlockByType(0);
    if (g_nMusicDriverMode_0049be8c != 0 &&
        g_nMusicDriverMode_0049be8c != 3)
        memoryThreshold = 210000;
    SetFrameTimerPeriodDirect(0x78);
    if ((int)GetAvailableFarMemoryByType(4) > memoryThreshold) {
        g_nAvailableGameMemory_005a7ce0 =
            (int)g_dwOriginalFreeMemory_005a7cd8 -
            g_anExpandedMemoryReservationByVideoMode_00469ab0[
                g_bSlowSceneAnimation_00469998];
        if (g_nAvailableGameMemory_005a7ce0 < 0)
            exit_squadron(
                "You do not have enough memory to play Wing Commander.\n"
                "Refer to your reference guide for assistance.");
        g_nMemoryConfiguration_005a7cd4 = 2;
        SystemDebugPrintf("Expanded Memory fully used.\n");
    } else {
        g_nAvailableGameMemory_005a7ce0 =
            (int)g_dwOriginalFreeMemory_005a7cd8 -
            g_anBaseMemoryReservationByVideoMode_00469a90[
                g_bSlowSceneAnimation_00469998];
        if (g_nAvailableGameMemory_005a7ce0 < 0)
            exit_squadron(
                "You do not have enough memory to play Wing Commander.\n"
                "Refer to your reference guide for assistance.");
        g_nMemoryConfiguration_005a7cd4 = 0;
        if (g_nMusicDriverMode_0049be8c == 1 ||
            g_nMusicDriverMode_0049be8c == 2) {
            videoModeMemory =
                g_anFullMusicMemoryReservationByVideoMode_00469aa0[
                    g_bSlowSceneAnimation_00469998];
            if ((int)g_dwOriginalFreeMemory_005a7cd8 > videoModeMemory) {
                g_nMemoryConfiguration_005a7cd4 = 1;
                g_nAvailableGameMemory_005a7ce0 =
                    (int)g_dwOriginalFreeMemory_005a7cd8 - videoModeMemory;
                SystemDebugPrintf("Full");
            } else {
                SystemDebugPrintf("Limited");
            }
            SystemDebugPrintf(" music will play.");
        }
    }

    LoadSpaceflightResources();
    FxDriverShutdownHook();
    SetEventManagerPump(PollJoystickButtonEvents);
    PromptInsertNumberedDisk(0);
    ShutdownVideoHook(
        g_acGraphicsModeByAnimationSpeed_0046b178[
            g_bSlowSceneAnimation_00469998]);
    requestedGraphicsMode =
        g_acGraphicsModeByAnimationSpeed_0046b178[
            g_bSlowSceneAnimation_00469998];
    if (GetTargetColourIndex() != requestedGraphicsMode)
        exit_squadron("Requested graphics display mode not available.");
    LoadGamePaletteFile();
    InitializeGameTextContexts();
    InitializeDiskPromptTextContext();
    GetEventManagerStatus();
    g_nFrameSkip_0049d764 = 1;
    if (g_nMusicDriverMode_0049be8c != 0 &&
        g_nMusicDriverMode_0049be8c != 3)
        GetFxDriverStatus();
    initialize_direction_view_frames();
    g_pConstellationDefinitions_00598a28 =
        LoadPacketAllocated(0x3a, 0);
    g_pMissionCampaignData_005988bc = LoadPacketAllocated(0x3a, 1);
    SystemDebugPrintf("\n[SYSTEM] : Exiting initialize()\n");
    return 0;
}

/* Function start: 0x45C088 */
void InitializeGameTextContexts(void)
{
    InitializeDIBScreenViewport(&g_stScreenViewport_005d21a0,
                                (unsigned char)g_cSecondaryViewBufferColour_0049cb4c);
    g_stModalSourceViewport_005d2c50 = g_stScreenViewport_005d21a0;
    g_stDefaultTextContext_005d2d20.viewport =
        &g_stModalSourceViewport_005d2c50;
    g_stDefaultTextContext_005d2d20.text =
        g_szDefaultTextBuffer_005d2b80;
    InitializeTextContextFromFont(&g_stDefaultTextContext_005d2d20, 1,
                                  (unsigned char)g_ucPrimaryTextColour_0049cb64,
                                  g_cSecondaryViewBufferColour_0049cb4c);
    SetTextContext(&g_stDefaultTextContext_005d2d20);

    g_stSpaceTextContext_005d21c0.viewport = &g_stViewBuffer_005d2b00;
    g_stSpaceTextContext_005d21c0.text = g_szDefaultTextBuffer_005d2b80;
    g_stSpaceTextContext_005d21c0.alignment = 2;
    InitializeTextContextFromFont(&g_stSpaceTextContext_005d21c0, 1,
                                  g_ucSpaceClearColour_0049cb5c, -1);
}

/* Function start: 0x45C1A1 */
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

/* Function start: 0x45C279 */
unsigned int initialize_direction_view_frames(void)
{
    signed char frame;
    signed char pitchBands;
    signed char yawSectors;
    short yaw;
    short pitch;

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

/* Function start: 0x420B12 */
unsigned int LoadSpaceflightResources(void)
{
    unsigned char *debrisShapeSet;

    if (DAT_0059a856 == 0)
        return 0;
    if (LoadShapeSet(g_aCommon3SpaceResources_0049c728, 4, -1) == 0)
        return 0;
    LoadShapeSet(g_aMissionResourceDescriptors_0049c798, 4, -1);
    g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_WING].shapeSet =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet;
    debrisShapeSet =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_O_RING].shapeSet =
        debrisShapeSet;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_GLASS].shapeSet =
        debrisShapeSet;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_SHIP_TUBING].shapeSet =
        debrisShapeSet;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK].shapeSet =
        debrisShapeSet;
    LoadShapeSet(g_aCockpitResourceDescriptors_00469c48, 4, -1);
    g_apCommPortraitShapes_0059e180[8] = 0;
    g_apCommPortraitShapes_0059e180[9] = 0;
    g_apCommPortraitShapes_0059e180[10] = 0;
    g_apCommPortraitShapes_0059e180[11] = 0;
    g_apCommPortraitShapes_0059e180[13] = 0;
    return 0;
}

/* Function start: WC2_UNMAPPED */
unsigned int GetFxDriverInitResult(void)
{
    return 0;
}

/* Function start: WC2_UNMAPPED */
unsigned int GetMessagePumpResult(void)
{
    return 0;
}

/* Function start: WC2_UNMAPPED */
unsigned int GetFxDriverStatus(void)
{
    return 0;
}

/* Function start: WC2_UNMAPPED */
short ace_status(short ace, unsigned char bits)
{
    return (g_stCampaignState_0059ca50.aceFlags[ace] & bits) == bits;
}

/* Function start: WC2_UNMAPPED */
void unflag_ace(short ace, unsigned char bits)
{
    g_stCampaignState_0059ca50.aceFlags[ace] &= ~bits;
}

/* Function start: WC2_UNMAPPED */
void flag_ace(short ace, unsigned char bits)
{
    g_stCampaignState_0059ca50.aceFlags[ace] |= bits;
}

/* Function start: WC2_UNMAPPED */
void kill_ace(short ace)
{
    if (ace_status(ace, 1) != 0) {
        unflag_ace(ace, 1);
        flag_ace(ace, 2);
    }
}

/* Function start: 0x429550 */
void SendKilrathiAceGreetingOnce(short obj)
{
#if 0
    short ace = (short)g_asPilotLevel_00495d60[obj] - 14;

    send_message(obj, (signed char)(ace_status(ace, 4) != 0));
    flag_ace(ace, 8);
#else
    if (g_bKilrathiAceGreetingSent_00499bf4 == 0) {
        send_message(obj, 0x17);
        g_bKilrathiAceGreetingSent_00499bf4 = 1;
    }
#endif
}

/* Function start: WC2_UNMAPPED */
void prepare_ace(short ace)
{
    unflag_ace(ace, 0x1a);
    flag_ace(ace, 0x20);
}

/* Function start: 0x4293F9 */
void LoadTemporaryCampaignGlobals(void)
{
    ReleasePacketSlot(&g_pCampaignGlobals_00499c94);
    g_pCampaignGlobals_00499c94 =
        LoadWholeDataFile(g_szTemporaryCampaignGlobalsFile_00499bc0);
}

/* Function start: 0x429423 */
void SaveAndFreeTemporaryCampaignGlobals(void)
{
    short file;

    if (g_pCampaignGlobals_00499c94 != 0) {
        file = CreateDataFile(g_szTemporaryCampaignGlobalsFile_00499bc0);
        if (file >= 0) {
            WriteDataFileAtOffset(
                (unsigned short)file, 0,
                (unsigned int)*(unsigned short *)g_pCampaignGlobals_00499c94 *
                    2,
                g_pCampaignGlobals_00499c94);
            CloseDataFile((unsigned short)file);
        }
        ReleasePacketSlot(&g_pCampaignGlobals_00499c94);
    }
}

/* Function start: 0x42949F */
void *LoadWholeDataFile(const char *path)
{
    void *data;
    short file;
    unsigned int size;

    data = 0;
    file = OpenDataFileOrDie(path);
    if (file >= 0) {
        size = (unsigned int)_filelength(file);
        if (size == (unsigned int)-1) {
            CloseDataFile((unsigned short)file);
            return 0;
        }
        data = AllocateTaggedMemory(size, 0);
        if (data != 0)
            ReadDataFileAtOffset((unsigned short)file, 0, size, data);
        CloseDataFile((unsigned short)file);
    }
    return data;
}

/* Function start: 0x429581 */
short signed_random(short range)
{
    return RandomBelowOrEqual((short)(range * 2)) - range;
}

/* Function start: 0x4295A9 */
int alert_flag(short ship, unsigned int bits)
{
    return (DAT_0059b430[ship] & bits) != 0;
}

/* Function start: WC2_UNMAPPED */
/* NOT a constant return: `mov eax,0x59b430 / cmp eax,1 / sbb eax,eax / inc eax`
 * is the classic `>= 1` boolean idiom applied to the *address* 0x0059B430,
 * which is always non-zero -- hence Ghidra folding it to `return 1`.  Written
 * the way the original was, so the idiom survives. */
unsigned int HasSpeechBuffer(void)
{
    return (unsigned int)&DAT_0059b430 >= 1;
}

/* Function start: 0x4295D9 */
unsigned short set_alert(short i, unsigned int bits)
{
    DAT_0059b430[i] |= bits;
    return 0;
}

/* Function start: 0x4295F2 */
void clear_alert(short i)
{
    g_asCollisionCountdown_0059d2d0[i] = 0;
    DAT_0059b430[i] = 0;
    DAT_0059cf20[i] = 0xff;
}

/* Function start: 0x429625 */
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

/* Function start: 0x429698 */
void try2end_collision_alert(short obj)
{
    g_asCollisionCountdown_0059d2d0[obj]--;
    if (g_asCollisionCountdown_0059d2d0[obj] <= 0)
        clear_alert(obj);
    else
        set_alert(obj, 2);
}

/* Function start: 0x4296E2 */
short normal_speed(short obj)
{
    if (g_aeSpecialManeuver_00495600[obj] !=
            SPECIAL_MANEUVER_AFTERBURNER &&
        real_velocity(obj) <= g_asShipMaximumSpeed_0059c440[obj])
        return 1;
    return 0;
}

/* Function start: 0x429736 */
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
    ComputeVectorDelta(&g_aShipPosition_00494550[obj],
                       &g_aShipPosition_00494550[other],
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
        Vector_magnitude(&separation));
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
            Vector_magnitude(&separation));
        if (collisionRadius >= range)
            collisionFound = 1;
    } while (collisionFound == 0);
    return elapsed;
}

/* Function start: WC2_UNMAPPED */
void clear_crash_cache(void)
{
    short i = 0;

    do {
        g_asCollisionPartner_005a7cc0[i] = -1;
        i = i + 1;
    } while (i < 10);
}

/* Function start: 0x429958 */
short crash_time(short obj, short other)
{
#if 0
    if (g_asCollisionPartner_005a7cc0[obj] == other)
        return g_asCollisionTime_005a7ca0[obj];
    if (other < 10 && g_asCollisionPartner_005a7cc0[other] == obj)
        return g_asCollisionTime_005a7ca0[other];
#endif
    if (g_aeObjectClass_00495328[other] == OBJECT_CLASS_ASTEROID &&
        (g_asObjectScreenX_00493598[other] == -0x7fff ||
         g_asObjectScreenX_00493598[obj] == -0x7fff))
        return 0x7fff;
    return real_crash_time(obj, other);
}

/* Function start: 0x4299C9 */
short detect_collisions(short obj)
{
    short candidate;
    short closestTime;
    short other;
    short time;

    candidate = -1;
    closestTime = 30;
    for (other = 0; other <= 60; other++) {
        if (other != obj &&
            g_aeObjectClass_00495328[other] >= OBJECT_CLASS_ASTEROID &&
            g_aeObjectClass_00495328[other] != OBJECT_CLASS_MISSILE) {
            time = crash_time(obj, other);
            if (closestTime > time) {
                closestTime = time;
                candidate = other;
            }
        }
    }
    if (candidate != -1) {
        g_asCollisionPartner_005a7cc0[obj] = candidate;
        g_asCollisionTime_005a7ca0[obj] = closestTime;
    }
    return candidate;
}

/* Function start: 0x429B55 */
unsigned int unactive(short ship)
{
    if (ship != -1 &&
        g_aeObjectClass_00495328[ship] >= OBJECT_CLASS_SHIP &&
        g_asShipManeuver_00495f48[ship] != MANEUVER_HARD_BRAKE)
        return 0;
    return 1;
}

/* Function start: 0x429BA8 */
int are_alive(short obj)
{
    if (unactive(obj) == 0 &&
        g_aeShipObjective_00495f08[obj] != OBJECTIVE_WANDER)
        return 1;
    return 0;
}

/* Function start: 0x429BF0 */
void trim_goals(short obj, short amount)
{
#if 0
    short goal = g_anYawGoal_004954c0[obj];

    if (amount < goal)
        g_anYawGoal_004954c0[obj] = amount;
    else if (goal < -amount)
        g_anYawGoal_004954c0[obj] = -amount;
    goal = g_anPitchGoal_004954a8[obj];
    if (amount < goal)
        g_anPitchGoal_004954a8[obj] = amount;
    else if (goal < -amount)
        g_anPitchGoal_004954a8[obj] = -amount;
    return 0;
#else
    if (g_anYawGoal_004954c0[obj] > amount)
        g_anYawGoal_004954c0[obj] = amount;
    else if (g_anYawGoal_004954c0[obj] < -amount)
        g_anYawGoal_004954c0[obj] = -amount;
    if (g_anPitchGoal_004954a8[obj] > amount)
        g_anPitchGoal_004954a8[obj] = amount;
    else if (g_anPitchGoal_004954a8[obj] < -amount)
        g_anPitchGoal_004954a8[obj] = -amount;
#endif
}

/* Function start: 0x429CAD */
int report_kilrathi_rout(int mode)
{
    short obj;

    for (obj = 0; obj < 10; obj++) {
        if (g_aeObjectClass_00495328[obj] < OBJECT_CLASS_SHIP ||
            g_aeSpecialManeuver_00495600[obj] ==
                SPECIAL_MANEUVER_UNKNOWN_9)
            continue;
        switch (mode) {
        case 0:
            if (g_asShipSide_004955d0[obj] == SIDE_KILRATHI)
                return 1;
            break;
        case 1:
            if (g_asShipSide_004955d0[obj] == SIDE_KILRATHI &&
                g_asShipMissionType_00495de8[obj] != MISSION_TYPE_ROUT &&
                distance_from_object(0, obj) < 16000)
                return 1;
            break;
        case 2:
            if (g_asShipSide_004955d0[obj] == SIDE_KILRATHI &&
                (g_aeShipObjective_00495f08[obj] ==
                     OBJECTIVE_ENGAGE_ENEMY ||
                 g_aeShipObjective_00495f08[obj] ==
                     OBJECTIVE_DESTROY_SHIP))
                return 1;
            break;
        }
    }
    if (mode == 1 && g_nCurrentWave_0046c01c != -1)
        check_next_wave();
    return 0;
}

/* Function start: 0x429E24 */
short find_ship_index(short missionShip)
{
    short obj;

    g_nLastFoundShip_005a7cba = -1;
    if (missionShip != -1) {
        for (obj = 0; obj < 10; obj++) {
            if (((g_aeObjectClass_00495328[obj] >= OBJECT_CLASS_SHIP &&
                  g_aeSpecialManeuver_00495600[obj] !=
                      SPECIAL_MANEUVER_UNKNOWN_9) ||
                 g_aeObjectClass_00495328[obj] == OBJECT_CLASS_FUTURION) &&
                g_nShipMissionIndices_0059c830[obj] == missionShip) {
                g_nLastFoundShip_005a7cba = obj;
                return obj;
            }
        }
    }
    return -1;
}

/* Function start: 0x429EE0 */
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
            if (g_aeObjectClass_00495328[other] ==
                    OBJECT_CLASS_CAPITAL_SHIP &&
                g_aeSpecialManeuver_00495600[other] !=
                    SPECIAL_MANEUVER_UNKNOWN_9 &&
                g_asShipSide_004955d0[obj] ==
                    g_asShipSide_004955d0[other])
                canContinue = 1;
            other++;
        } while (other < 10);
    }
    if (canContinue != 0) {
        g_acShipStress_0059d620[obj] = 0;
        maneuver_complete(obj);
    } else {
        reset_mission_type(obj, MISSION_TYPE_ROUT);
        if (obj == g_nYourWingman_0049346c)
            send_message(obj, 9);
    }
    return canContinue == 0;
}

/* Function start: 0x42A003 */
signed char CanSetNewShipTurnGoal(short ship)
{
    if (GetAdaptiveTurnRate() > 120)
        return 1;
    return (g_anYawGoal_004954c0[ship] |
            g_anPitchGoal_004954a8[ship] |
            g_anRollGoal_004954d8[ship]) == 0;
}

/* Function start: 0x42A062 */
signed char no_goal(short ship)
{
    return (g_anYawGoal_004954c0[ship] |
            g_anPitchGoal_004954a8[ship] |
            g_anRollGoal_004954d8[ship]) == 0;
}

/* Function start: 0x42A0A9 */
int being_tailed(short obj, short other)
{
    ship_vs_ship(obj, other);
    return g_nFacingToTarget_00493194 < -60 &&
           g_nTargetFacing_0059d52a > 85 &&
           g_nTargetRange_0059ce10 < 7000;
}

/* Function start: WC2_UNMAPPED */
int any_enemy_tail(short obj)
{
    short other;

    for (other = 0; other < 10; other++) {
        if (g_aeObjectClass_00495328[other] >= OBJECT_CLASS_SHIP &&
            g_aeSpecialManeuver_00495600[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            g_asShipSide_004955d0[obj] != g_asShipSide_004955d0[other] &&
            g_acShipTarget_00495f20[other] == obj &&
            being_tailed(obj, other) != 0) {
            g_nTargetShip_0059c3b0 = other;
            return 1;
        }
    }
    g_nTargetShip_0059c3b0 = -1;
    return 0;
}

/* Function start: 0x42A108 */
short detect_enemy_tail(short obj)
{
    short other;

    for (other = 0; other < 10; other++) {
        if (g_aeObjectClass_00495328[other] >= OBJECT_CLASS_SHIP &&
            g_aeSpecialManeuver_00495600[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            g_asShipSide_004955d0[obj] != g_asShipSide_004955d0[other] &&
            g_acShipTarget_00495f20[other] == obj &&
            being_tailed(obj, other) != 0)
            return other;
    }
    return -1;
}

/* Function start: 0x42A1C5 */
int is_ship_tailing_player_target(short obj)
{
    short target = g_acShipTarget_00495f20[0];

    if (unactive(target) == 0 && being_tailed(target, obj) != 0)
        return 1;
    return 0;
}

/* Function start: 0x42A222 */
short FindMissileTargetingObject(short obj)
{
    short other;

    for (other = 0; other < 10; other++) {
        if (g_aeObjectClass_00495328[other] == OBJECT_CLASS_MISSILE &&
            g_acShipTarget_00495f20[other] == obj)
#if 0
            return 1;
#else
            return other;
#endif
    }
    return 0;
}

/* Function start: WC2_UNMAPPED */
short select_weighted_value(short *choices)
{
    short roll;

    roll = RandomBelowOrEqual(100) + 1;
    roll -= choices[0];
    while (roll > 0) {
        if (choices[0] == -1)
            break;
        choices += 2;
        roll -= choices[0];
    }
    if (choices[0] == -1)
        return -1;
    return choices[1];
}

/* Function start: 0x42A28E */
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

/* Function start: 0x42A313 */
unsigned int find_squad_center(FixedVector *center)
{
    short count = 0;

    zero_vector(center);
    while (g_acFormationMemberList_0059d490[count] != -1) {
        AddFixedVectors(center, &g_aShipPosition_00494550[
                            (short)g_acFormationMemberList_0059d490[count]],
                        center);
        count++;
    }
    if (count != 0)
        divide_vector(center, (int)count << 8, center);
    return 0;
}

/* Function start: 0x42A39A */
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
        destination = &g_aShipDestination_004953f0[member];
        ComputeVectorDelta(&center, &g_aShipPosition_00494550[member],
                           destination);
        ScaleFixedVector(destination, 0xa00, destination);
        AddFixedVectors(destination, &g_aShipPosition_00494550[member],
                        destination);
        steady_object(member);
        reset_objective(member, OBJECTIVE_BREAK_FORMATION);
        member = g_acFormationMemberList_0059d490[++index];
    }
    return 0;
}

/* Function start: 0x42A490 */
unsigned int reset_mission_type(short obj,
                                enum ShipMissionType missionType)
{
    if (missionType == MISSION_TYPE_ROUT &&
        g_asShipSide_004955d0[obj] == SIDE_KILRATHI)
        report_kilrathi_rout(1);
    reset_objective(obj, OBJECTIVE_NONE);
    g_asShipMissionType_00495de8[obj] = missionType;
    return 0;
}

/* Function start: 0x42A4E5 */
unsigned int change_mission_type(short obj,
                                 enum ShipMissionType missionType)
{
    if (g_aeShipObjective_00495f08[obj] == OBJECTIVE_ENGAGE_ENEMY)
        g_asShipMissionType_00495de8[obj] = missionType;
    else
        reset_mission_type(obj, missionType);
    return 0;
}

/* Function start: 0x42A52A */
unsigned int reset_objective(short ship, enum ShipObjective objective)
{
    steady_object(ship);
    reset_tactic(ship, TACTIC_NONE);
    g_aeShipObjective_00495f08[ship] = objective;
    return 0;
}

/* Function start: 0x42A55F */
unsigned int alter_objective(short ship, enum ShipObjective objective)
{
    steady_object(ship);
    alter_tactic(ship, TACTIC_NONE);
    g_aeShipObjective_00495f08[ship] = objective;
    return 0;
}

/* Function start: 0x42A594 */
unsigned int reset_tactic(short ship, enum ShipTactic tactic)
{
    reset_maneuver(ship, MANEUVER_NONE);
    g_aeShipTactic_0059d5e0[ship] = tactic;
    g_acShipTarget_00495f20[ship] = -1;
    return 0;
}

/* Function start: 0x42A5C8 */
unsigned int alter_tactic(short ship, enum ShipTactic tactic)
{
    reset_maneuver(ship, MANEUVER_NONE);
    g_aeShipTactic_0059d5e0[ship] = tactic;
    return 0;
}

/* Function start: 0x42A5F1 */
void reset_maneuver(short ship, short maneuver)
{
    g_asShipManeuver_00495f48[ship] = (enum ShipManeuver)maneuver;
    g_asShipCount_0059c420[ship] = 0;
    g_acShipSequence_00495fe8[ship] = 0;
}

/* Function start: 0x42A625 */
void try2reset_maneuver(short obj, short maneuver)
{
    if (g_asShipManeuver_00495f48[obj] != maneuver) {
        reset_maneuver(obj, maneuver);
        steady_object(obj);
    }
}

/* Function start: 0x42A664 */
void set_special(short ship, short special)
{
#if 0
    short *currentState;
    short current;
#endif

#if 0
    /* Retained WC1 implementation. */
    currentState = &g_aeSpecialManeuver_00495600[ship];
    current = *currentState;
    if (current < SPECIAL_MANEUVER_LOST_CONTROL || special > current)
        *currentState = special;

    if (*currentState == SPECIAL_MANEUVER_BLOWING_UP &&
        (short)alert_flag(ship, 1))
        *currentState = SPECIAL_MANEUVER_NONE;
    return 0;
#else
    if (g_aeSpecialManeuver_00495600[ship] <
            SPECIAL_MANEUVER_LOST_CONTROL ||
        special > g_aeSpecialManeuver_00495600[ship])
        g_aeSpecialManeuver_00495600[ship] = special;

    if (g_aeSpecialManeuver_00495600[ship] ==
            SPECIAL_MANEUVER_BLOWING_UP &&
        (short)alert_flag(ship, 1) != 0)
        g_aeSpecialManeuver_00495600[ship] = SPECIAL_MANEUVER_NONE;
#endif
}

/* Function start: 0x42A6E8 */
unsigned int approach_zero_speed(short ship)
{
    approach_speed(ship, 0);
    return 0;
}

/* Function start: 0x42A701 */
unsigned int approach_min_speed(short obj)
{
    approach_speed(obj, 0x500);
    return 0;
}

/* Function start: 0x42A71D */
void approach_half_speed(short obj)
{
#if 0
    short speed = g_aObjectTypeData_00496d30[
        g_acObjectType_00493980[obj]].cruiseVelocity;

    approach_speed(obj, (int)(short)(speed & 0xfffe) << 7);
    return 0;
#else
    approach_speed(
        obj,
        (int)(short)(g_aObjectTypeData_00496d30[
            g_acObjectType_00493980[obj]].cruiseVelocity & 0xfffe) << 7);
#endif
}

/* Function start: 0x42A754 */
unsigned int approach_cruise_speed(short ship)
{
    approach_speed(ship,
        (int)g_aObjectTypeData_00496d30[
            g_acObjectType_00493980[ship]].cruiseVelocity << 8);
    return 0;
}

/* Function start: 0x42A788 */
unsigned int approach_full_speed(short ship)
{
    approach_speed(ship,
                      (int)g_asShipMaximumSpeed_0059c440[ship] << 8);
    return 0;
}

/* Function start: 0x42A7AF */
unsigned int approach_ship_speed(short obj, short other)
{
    approach_speed(obj, g_anShipSpeed_0059b320[other]);
    return 0;
}

/* Function start: 0x42A7D2 */
void ComputePointAheadOfObject(short obj, unsigned short distance,
                               FixedVector *point)
{
    ScaleFixedVector(&g_aShipForwardVector_00494208[obj],
                     (int)distance << 8, point);
    AddFixedVectors(&g_aShipPosition_00494550[obj], point, point);
}

/* Function start: 0x42A823 */
void ComputePointBehindObject(short obj, unsigned short distance,
                              FixedVector *point)
{
    ScaleFixedVector(&g_aShipForwardVector_00494208[obj],
                     -((int)distance << 8), point);
    AddFixedVectors(&g_aShipPosition_00494550[obj], point, point);
}

/* Function start: 0x42A876 */
unsigned int close_behind(short range)
{
    if (g_nTargetRange_0059ce10 < range &&
        g_nTargetFacing_0059d52a < -0x32)
        return 1;
    return 0;
}

/* Function start: 0x42A8B5 */
short scan_for_enemy(short obj, unsigned short range)
{
    short target;
    short other;
    short distance;

    target = -1;
    g_nTargetRange_0059ce10 = 0;
    for (other = 0; other < 10; other++) {
        if (g_aeObjectClass_00495328[other] < OBJECT_CLASS_SHIP ||
            g_aeSpecialManeuver_00495600[other] ==
                SPECIAL_MANEUVER_UNKNOWN_9)
            continue;
        g_nTargetShip_0059c3b0 = target;
        if (g_asShipSide_004955d0[obj] == g_asShipSide_004955d0[other])
            continue;
        distance = distance_from_point(other, &g_aShipPosition_00494550[obj]);
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

/* Function start: 0x42AA0D */
short any_enemy(short obj, short range)
{
    short other;

    g_nTargetShip_0059c3b0 = -1;
    for (other = 0; other < 10; other++) {
        if (g_aeObjectClass_00495328[other] >= OBJECT_CLASS_SHIP &&
            g_aeSpecialManeuver_00495600[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            g_asShipSide_004955d0[obj] != g_asShipSide_004955d0[other]) {
            g_nTargetRange_0059ce10 = distance_from_object(obj, other);
            if (g_nTargetRange_0059ce10 < range) {
                g_nTargetShip_0059c3b0 = other;
                return 1;
            }
        }
    }
    return 0;
}

/* Function start: WC2_UNMAPPED */
short nearest_enemy_range(short obj)
{
    short other;
    short range;

    g_nTargetShip_0059c3b0 = -1;
    range = 0x7fff;
    other = 0;
    do {
        if (g_aeObjectClass_00495328[other] >= OBJECT_CLASS_SHIP &&
            g_aeSpecialManeuver_00495600[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            g_asShipSide_004955d0[obj] != g_asShipSide_004955d0[other])
            range = MinShort(range, distance_from_object(obj, other));
        other++;
    } while (other < 10);
    return range;
}

/* Function start: WC2_UNMAPPED */
void fire_when_ready(short obj, short aimed)
{
    (void)aimed;
    if (g_nYourWingman_0049346c == obj &&
        g_asPilotLevel_00495d60[obj] != 11) {
        ship_vs_ship(obj, 0);
        if (g_nFacingToTarget_00493194 > 80)
            return;
    }
    fire(obj, g_acShipTarget_00495f20[obj]);
}

/* Function start: 0x42AB81 */
unsigned int ships_within_range(short obj, short other, short range)
{
    FixedVector delta;

    ComputeVectorDelta(&g_aShipPosition_00494550[obj],
                       &g_aShipPosition_00494550[other], &delta);
    return IsVectorWithinRange(&delta, range);
}

/* Function start: 0x42ABCE */
int attacker_in_range(short obj, short range)
{
    short other;

    for (other = 0; other < 10; other++) {
        if (g_aeObjectClass_00495328[other] >= OBJECT_CLASS_SHIP &&
            g_aeSpecialManeuver_00495600[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            g_asShipSide_004955d0[obj] != g_asShipSide_004955d0[other] &&
            g_acShipTarget_00495f20[other] == obj) {
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

/* Function start: 0x42ACAF */
int in_danger(short obj)
{
    short target;
    short other;
    unsigned short range;

    target = -1;
    for (other = 0; other < 10; other++) {
        if (g_aeObjectClass_00495328[other] >= OBJECT_CLASS_SHIP &&
            g_aeSpecialManeuver_00495600[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            g_asShipSide_004955d0[obj] != g_asShipSide_004955d0[other]) {
            g_nTargetShip_0059c3b0 = target;
            if (g_acShipTarget_00495f20[other] != obj)
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

/* Function start: 0x42ADB8 */
short target_within_range(short obj)
{
    short target = g_acShipTarget_00495f20[obj];

    if (unactive(target) != 0)
        return 0;
    if (target_valid(obj, target) == 0)
        return 0;
    return ships_within_range(obj, target, 7000);
}

/* Function start: 0x42AE32 */
short build_target_list(short obj, short range)
{
    short count;
    short other;
    short distance;

    count = 0;
    for (other = 0; other < 10; other++) {
        if (g_aeObjectClass_00495328[other] >= OBJECT_CLASS_SHIP &&
            g_aeSpecialManeuver_00495600[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            g_asShipSide_004955d0[obj] != g_asShipSide_004955d0[other]) {
            distance = distance_from_object(obj, other);
            if (distance < range) {
                g_acFormationMemberList_0059d490[count] =
                    (signed char)other;
                g_asTargetListRange_0059cf60[count] = distance;
                count++;
            }
        }
    }
    g_acFormationMemberList_0059d490[count] = -1;
    return count;
}

/* Function start: WC2_UNMAPPED */
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
            RandomBelowOrEqual(index)];
    }
    g_nTargetShip_0059c3b0 = target;
    return target != -1;
}

/* Function start: 0x42AF60 */
void inherit_leader_mission(short obj)
{
    short leader = g_asShipWingLeader_0059d400[obj];

    if (leader != -1 &&
        g_aeObjectClass_00495328[obj] >= OBJECT_CLASS_SHIP) {
        g_asShipMissionType_00495de8[obj] =
            g_asShipMissionType_00495de8[leader];
        g_anShipMissionShip_00495e00[obj] =
            g_anShipMissionShip_00495e00[leader];
        g_asShipWingLeader_0059d400[obj] = -1;
        g_aShipMissionSpot_00495e18[obj] =
            g_aShipMissionSpot_00495e18[leader];
    }
}

/* Function start: 0x42B00A */
void inherit_leader(short obj)
{
    short leader = g_asShipWingLeader_0059d400[obj];
    short other;

    if (leader == -1 || g_aeObjectClass_00495328[obj] < OBJECT_CLASS_SHIP)
        return;
    inherit_leader_mission(obj);
    g_asShipWingLeader_0059d400[obj] = g_asShipWingLeader_0059d400[leader];
    for (other = 0; other < 10; other++) {
        if (g_asShipWingLeader_0059d400[other] == leader)
            g_asShipWingLeader_0059d400[other] = obj;
    }
}

/* Function start: 0x42B0B7 */
short dead_ship(short i)
{
#if 0
    if (i != -1 && g_aMissionShips_00492290[i].state != 3)
        return 0;
#else
    return i == -1 || g_aMissionShips_00492290[i].state == 3;
#endif
#if 0
    return 1;
#endif
}

/* Function start: 0x42B0FB */
int gone_ship(short missionShip)
{
    if (missionShip != -1 &&
        g_aMissionShips_00492290[missionShip].state != 3 &&
        g_aMissionShips_00492290[missionShip].state != 2)
        return 0;
    return 1;
}

/* Function start: WC2_UNMAPPED */
short skill_rating(short obj)
{
    int rating = g_asPilotLevel_00495d60[obj];

    if (rating <= 4)
        return MaxShort(2, (short)rating);
    if (rating == 13)
        return 5;
    if (rating < 14)
        return (short)((rating - 5 >> 1) + 4);
    return (short)rating - 10;
}

/* Function start: 0x42B15A */
short skill_check(short obj)
{
#if 0
    short roll;

    roll = RandomBelowOrEqual(MinShort(8, difficulty));
    return skill_rating(obj) > roll;
#else
    if (g_asPilotLevel_00495d60[obj] < 5 &&
        RandomBelowOrEqual((short)(g_asPilotLevel_00495d60[obj] + 4)) == 0)
        return 0;
    return 1;
#endif
}

/* Function start: 0x42B1AE */
short find_ships_sphere(short missionShip)
{
    MissionNavPoint *navPoint;
    short fallback;
    short navIndex;
    short shipIndex;

    navIndex = 0;
    navPoint = g_aMissionNavPoints_00491e98;
    fallback = -1;
    for (; navIndex < WC1_ACTIVE_MISSION_NAV_POINT_COUNT;
         navIndex++, navPoint++) {
        for (shipIndex = 0; shipIndex < 10; shipIndex++) {
            if (navPoint->missionShips[shipIndex] == missionShip) {
                if (navPoint->type == 1)
                    return navIndex;
                if (fallback == -1)
                    fallback = navIndex;
            }
        }
    }
    if (fallback != -1) {
        navIndex = fallback;
        if (g_aMissionNavPoints_00491e98[navIndex].type > 1)
            do {
                navIndex--;
            } while (g_aMissionNavPoints_00491e98[navIndex].type > 1);
    } else {
        navIndex = -1;
    }
    return navIndex;
}

/* Function start: 0x42B2A7 */
int locate_ship(short missionShip, FixedVector *point)
{
    short navPoint;
    short obj;

    obj = find_ship_index(missionShip);
    if (obj != -1) {
        *point = g_aShipPosition_00494550[obj];
        return 1;
    }
    if (dead_ship(missionShip) != 0)
        return 0;
    navPoint = find_ships_sphere(missionShip);
    if (navPoint != -1) {
        AddFixedVectors(&g_aMissionNavPoints_00491e98[navPoint].position,
                        &g_aMissionShips_00492290[missionShip].position,
                        point);
        return 1;
    }
    zero_vector(point);
    return 0;
}

/* Function start: 0x42B38F */
unsigned int get_follow_point(short obj, FixedVector *point)
{
    short pathIndex;
    short objective;
    short missionShip;
    int type;

    if (g_asShipSide_004955d0[obj] == SIDE_KILRATHI) {
        *point = g_aMissionNavPoints_00491e98[
            g_anShipMissionShip_00495e00[obj]].position;
        return 0;
    }
    pathIndex = (short)g_abShipNavPointIndex_00495f60[obj];
    while (++pathIndex < 16) {
        objective = (short)g_abFlightPath_004932a0[pathIndex];
        type = g_aMissionObjectives_004932a8[objective].type;
        switch (type) {
        case 0:
            *point = g_aMissionObjectives_004932a8[objective].position;
            g_abShipNavPointIndex_00495f60[obj] = (signed char)pathIndex;
            return 0;
        case 1:
            missionShip = (short)g_aMissionObjectives_004932a8[objective].index;
            objective = find_ship_index(missionShip);
            if (objective == -1)
                *point = g_aMissionNavPoints_00491e98[missionShip].position;
            else
                *point = g_aShipPosition_00494550[objective];
            g_abShipNavPointIndex_00495f60[obj] = (signed char)pathIndex;
            return 0;
        }
    }
    return 0;
}

/* Function start: 0x42B524 */
unsigned int get_first_follow_point(short obj, FixedVector *point)
{
    if (g_asShipSide_004955d0[obj] == SIDE_IMPERIAL)
        g_abShipNavPointIndex_00495f60[obj] =
            g_cCurrentNavPointIndex_00493298 - 1;
    get_follow_point(obj, point);
    return 0;
}

/* Function start: 0x42B565 */
short hostile_sphere(short obj, short navPoint)
{
#if 0
    short index;
    short missionShip;
    short *ships = g_aMissionNavPoints_00491e98[navPoint].missionShips;

    for (index = 0; index < 10; index++) {
        missionShip = ships[index];
        if (missionShip != -1 &&
            g_asShipSide_004955d0[obj] !=
                g_aMissionShips_00492290[missionShip].side)
            return 1;
    }
    return 0;
#else
    short index;
    short missionShip;

    for (index = 0; index < 10; index++) {
        missionShip =
            g_aMissionNavPoints_00491e98[navPoint].missionShips[index];
        if (missionShip != -1 &&
            g_aMissionShips_00492290[missionShip].side !=
                g_asShipSide_004955d0[obj])
            return 1;
    }
    return 0;
#endif
}

/* Function start: 0x42B5F7 */
short abandoned(short obj, short other)
{
#if 0
    short navPoint;

    if ((g_abShipTurn_00495fd8[obj] & 7) == 0 &&
        g_asShipSide_004955d0[obj] != SIDE_KILRATHI &&
        RandomBelowOrEqual(8) == 0) {
        navPoint = FindNearestNavPoint(obj);
        if (g_nCurrentNavPoint_004931bc != navPoint &&
            hostile_sphere(obj, navPoint) != 0 &&
            distance_from_object(obj, other) > 10000) {
            explode(-1, obj);
            return 1;
        }
    }
    return 0;
#else
    short navPoint;

    if ((g_abShipTurn_00495fd8[obj] & 7) == 0 &&
        g_asShipSide_004955d0[obj] != SIDE_KILRATHI) {
        if (RandomBelowOrEqual(8) != 0)
            return 0;
        navPoint = FindNearestNavPoint(obj);
        if (g_nCurrentNavPoint_004931bc != navPoint &&
            hostile_sphere(obj, navPoint) != 0 &&
            distance_from_object(obj, other) > 10000) {
            explode(-1, obj);
            return 1;
        }
    }
    return 0;
#endif
}

/* Function start: 0x42B6C5 */
void engage(short obj, short target, enum ShipObjective objective)
{
    if (g_aeShipObjective_00495f08[obj] != objective) {
        reset_objective(obj, objective);
#if 0
        if (g_acShipRating_0059cd80[obj] > 8 &&
            ace_status(
                (short)((short)g_asPilotLevel_00495d60[obj] - 14), 8) == 0)
            ace_greeting(obj);
#else
        if (g_asPilotLevel_00495d60[obj] == 5 &&
            g_asShipSide_004955d0[obj] == SIDE_KILRATHI)
            SendKilrathiAceGreetingOnce(obj);
#endif
    }
    g_acShipTarget_00495f20[obj] = target;
}

/* Function start: 0x42B73B */
short target_valid(short obj, short target)
{
#if 0
    return unactive(g_acShipTarget_00495f20[obj]) == 0 &&
           g_asShipSide_004955d0[g_acShipTarget_00495f20[obj]] !=
               g_asShipSide_004955d0[obj];
#else
    if (target == -1)
        return 0;
    if (g_anShipCloakState_00496020[target] == 1 &&
        g_asShipCloakElapsedFrames_00496060[target] >= 40)
        return 0;
    if (ShipHasTorpedo(obj) == 0 &&
        CanShipWeaponDamageTarget(obj, target) == 0)
        return 0;
    if (unactive(target) == 0 &&
        g_asShipSide_004955d0[obj] != g_asShipSide_004955d0[target])
        return 1;
    return 0;
#endif
}

/* Function start: 0x42B80C */
short triumph(short obj)
{
#if 0
    unsigned int result = 0;
    short objective;

    switch (g_asShipMissionType_00495de8[obj]) {
    case MISSION_TYPE_PATROL:
        objective = 0;
        while (g_abFlightPath_004932a0[objective] != -1 &&
               visited(
                   (short)g_abFlightPath_004932a0[objective]) != 0)
            objective++;
        return 1;
    case MISSION_TYPE_ESCORT:
    case MISSION_TYPE_DEFEND:
    case MISSION_TYPE_WINGMAN:
        return dead_ship(g_anShipMissionShip_00495e00[obj]) == 0;
    case MISSION_TYPE_STRIKE:
        result = dead_ship(g_anShipMissionShip_00495e00[obj]);
    }
    return result;
#else
    short result;
    short objective;

    result = 0;
    switch (g_asShipMissionType_00495de8[obj]) {
    case MISSION_TYPE_PATROL:
        objective = 0;
        while (g_abFlightPath_004932a0[objective] != -1) {
            if (visited((short)g_abFlightPath_004932a0[objective]) != 0)
                objective++;
            else
                break;
        }
        result = 1;
        break;
    case MISSION_TYPE_ESCORT:
    case MISSION_TYPE_DEFEND:
    case MISSION_TYPE_WINGMAN:
        result = dead_ship(g_anShipMissionShip_00495e00[obj]) == 0;
        break;
    case MISSION_TYPE_STRIKE:
        result = dead_ship(g_anShipMissionShip_00495e00[obj]);
        break;
    }
    return result;
#endif
}

/* Function start: WC2_UNMAPPED */
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

/* Function start: 0x42B985 */
short evaluate_damage(short obj)
{
    ObjectTypeData *typeData =
        &g_aObjectTypeData_00496d30[g_acObjectType_00493980[obj]];

    if (g_aeObjectClass_00495328[obj] < OBJECT_CLASS_SHIP)
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

/* Function start: WC2_UNMAPPED */
short mine_available(short obj)
{
    return find_weapon(obj, OBJECT_TYPE_SPACE_MINE);
}

/* Function start: 0x4568E0 */
short LoadShapeSet(PacketResourceDescriptor *resources,
                   short flags, char *defaultFileName)
{
#if 0
    short logicalFile;

    while (resources->resource != 0) {
        if (*resources->resource == 0) {
            logicalFile = resources->logicalFile;
            if (logicalFile == -1)
                logicalFile = defaultLogicalFile;
            *resources->resource = FetchDiskPacketRetrying(
                logicalFile, resources->section, flags);
            if (*resources->resource == 0)
                return 0;
        }
        resources++;
    }
    return 1;
#else
    while (resources->resource != 0) {
        if (*resources->resource == 0) {
            if (resources->fileName == 0) {
                *resources->resource = FetchDiskPacketRetrying(
                    defaultFileName, resources->section,
                    (unsigned short)(flags | 0x40));
            } else {
                *resources->resource = FetchDiskPacketRetrying(
                    resources->fileName, resources->section,
                    (unsigned short)(flags | 0x40));
            }
            if (*resources->resource == 0)
                return 0;
        }
        resources++;
    }
    return 1;
#endif
}

/* Function start: 0x456988 */
void FreeShapeSet(PacketResourceDescriptor *resources,
                  unsigned short releaseFlags)
{
#if 0
    while (resources->resource != 0) {
        if (*resources->resource != 0)
            FreePacketAndClear(resources->resource, releaseFlags);
        resources++;
    }
    return 0;
#else
    while (resources->resource != 0) {
        if (*resources->resource != 0)
            FreePacketAndClear(resources->resource, releaseFlags);
        resources++;
    }
#endif
}

/* Function start: 0x4569C8 */
int LoadPacketResourceList(PacketResourceDescriptor *resources,
                           short flags, int availableBytes,
                           char *defaultFileName)
{
    int packetSize;

#if 0
    while (resources->resource != 0) {
        if (*resources->resource == 0) {
            PromptInsertNumberedDisk(resources->logicalFile);
            packetSize = (int)GetPacketSize(
                g_pDiskFileRecords_005a7cf0[
                    resources->logicalFile].name,
                resources->section);
            if (packetSize < availableBytes) {
                *resources->resource =
                    FetchDiskPacketRetrying(
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
#else
    while (resources->resource != 0) {
        if (*resources->resource == 0) {
            packetSize = (int)GetNamedPacketSize(
                defaultFileName, resources->section);
            if (packetSize < availableBytes) {
                *resources->resource = FetchDiskPacketRetrying(
                    defaultFileName, resources->section, flags);
                if (*resources->resource == 0)
                    return availableBytes;
                availableBytes -= packetSize;
            }
        }
        resources++;
    }
    return availableBytes;
#endif
}

/* Function start: 0x456A68 */
void ResetCockpitPaletteEntries(void)
{
#if 0
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
    memcpy(g_asSpacePaletteFade_005d2d60, black, sizeof(black));
    g_asSpacePaletteFade_005d2d60[2] = 32;
    SetPaletteEntry((short)g_cPrimaryViewBufferColour_0049cb88, g_asSpacePaletteFade_005d2d60);
    return 0;
#else
    short black[3];
    short index;

    black[0] = 0;
    black[1] = 0;
    black[2] = 0;
    for (index = 0; index < 6; index++) {
        memcpy(g_aPaletteFadeEntries_005a76d0[index], black,
               sizeof(black));
        SetPaletteEntry((short)(index + 0xf9),
                        g_aPaletteFadeEntries_005a76d0[index]);
    }
    memcpy(g_asSpacePaletteFade_005d2d60, black, sizeof(black));
    g_asSpacePaletteFade_005d2d60[2] = 32;
    SetPaletteEntry((short)g_cPrimaryViewBufferColour_0049cb88,
                    g_asSpacePaletteFade_005d2d60);
#endif
}

/* Function start: 0x456B1A */
void initialize_cockpit(signed char mode)
{
    Viewport savedScreen;
    unsigned char *backdrop;
#if 0
    unsigned int result;
#endif

    if (message_showing())
        EndCommMenu();

    if (g_nCockpitDisplayMode_0049d71c == 0 && mode == g_cScreenViewportMode_005c82a6) {
        if (g_stViewBuffer_005d2b00.pixels != 0)
            ClearViewport(&g_stViewBuffer_005d2b00, g_cPrimaryViewBufferColour_0049cb88);
        else
            initialize_view_buffer();
#if 0
        return 0;
#else
        return;
#endif
    }

    free_view_buffer();
    savedScreen = g_stScreenViewport_005d21a0;
    g_stScreenViewport_005d21a0.left = 0;
    g_stScreenViewport_005d21a0.top = 0;
    g_stScreenViewport_005d21a0.right = 319;
    g_stScreenViewport_005d21a0.bottom = 199;
    ClearViewport(&g_stModalSourceViewport_005d2c50, g_cSecondaryViewBufferColour_0049cb4c);

    g_cCockpitLogicalFile_005a7c74 =
        (unsigned char)(g_cCockpitView_0059dab0 + 17);
    g_cScreenViewportMode_005c82a6 = mode;
    switch (mode) {
    case 0:
        if (g_apCockpitShapes_005a7c80[0] != 0) {
            if (g_nCockpitDisplayMode_0049d71c == 0)
                DrawSpriteDefault(&g_stScreenViewport_005d21a0, 0, 0,
                                  g_apCockpitShapes_005a7c80[0], 0);
        } else {
            backdrop = FetchDiskPacketRetrying(
                (short)g_cCockpitLogicalFile_005a7c74, 0, 0);
            if (g_nCockpitDisplayMode_0049d71c == 0)
                DrawSpriteDefault(&g_stScreenViewport_005d21a0, 0, 0, backdrop, 0);
            ReleasePacketHandle(backdrop);
        }
        ResetCockpitPaletteEntries();
        if (g_nCockpitDisplayMode_0049d71c == 0)
            explosion_draw();
        reset_cockpit();
        InvalidateVduMode(0);
        InvalidateVduMode(1);
        update_VDUs();
        clear_head_up_display();
        if (g_nCockpitDisplayMode_0049d71c == 0)
            ResetPilotHandAnimation();
        set_up_screen_viewport(0);
        break;
    case 1:
        if (g_apCockpitShapes_005a7c80[1] != 0) {
            if (g_nCockpitDisplayMode_0049d71c == 0)
                DrawSpriteDefault(&g_stScreenViewport_005d21a0, 0, 0,
                                  g_apCockpitShapes_005a7c80[1], 0);
        } else {
            backdrop = FetchDiskPacketRetrying(
                (short)g_cCockpitLogicalFile_005a7c74, 1, 0);
            if (g_nCockpitDisplayMode_0049d71c == 0)
                DrawSpriteDefault(&g_stScreenViewport_005d21a0, 0, 0, backdrop, 0);
            ReleasePacketHandle(backdrop);
        }
        set_up_screen_viewport(1);
        break;
    case 2:
        if (g_apCockpitShapes_005a7c80[2] != 0) {
            if (g_nCockpitDisplayMode_0049d71c == 0)
                DrawSpriteDefault(&g_stScreenViewport_005d21a0, 0, 0,
                                  g_apCockpitShapes_005a7c80[2], 0);
        } else {
            backdrop = FetchDiskPacketRetrying(
                (short)g_cCockpitLogicalFile_005a7c74, 2, 0);
            if (g_nCockpitDisplayMode_0049d71c == 0)
                DrawSpriteDefault(&g_stScreenViewport_005d21a0, 0, 0, backdrop, 0);
            ReleasePacketHandle(backdrop);
        }
        set_up_screen_viewport(2);
        break;
    case 3:
        if (g_apCockpitShapes_005a7c80[3] != 0) {
            if (g_nCockpitDisplayMode_0049d71c == 0)
                DrawSpriteDefault(&g_stScreenViewport_005d21a0, 0, 0,
                                  g_apCockpitShapes_005a7c80[3], 0);
        } else {
            backdrop = FetchDiskPacketRetrying(
                (short)g_cCockpitLogicalFile_005a7c74, 3, 0);
            if (g_nCockpitDisplayMode_0049d71c == 0)
                DrawSpriteDefault(&g_stScreenViewport_005d21a0, 0, 0, backdrop, 0);
            ReleasePacketHandle(backdrop);
        }
        set_up_screen_viewport(3);
        break;
    case 4:
        if (g_bIntroSceneResourcesActive_00469d48 == 1) {
            if (g_pCinematicViewBackdrop_005a7c90 != 0) {
                DrawSpriteDefault(&g_stScreenViewport_005d21a0, 0, 0,
                                  g_pCinematicViewBackdrop_005a7c90, 0);
                if (g_nCockpitDisplayMode_0049d71c < 1) {
                    DIBslam();
                    DIBslamReal();
                }
            } else {
                backdrop = FetchDiskPacketRetrying(8, 6, 0);
                if (backdrop != 0) {
                    DrawSpriteDefault(&g_stScreenViewport_005d21a0, 0, 0, backdrop, 0);
                    if (g_nCockpitDisplayMode_0049d71c < 1) {
                        DIBslam();
                        DIBslamReal();
                    }
                    ReleasePacketHandle(backdrop);
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
            if (g_nCockpitDisplayMode_0049d71c == 0)
                DrawSpriteDefault(&g_stScreenViewport_005d21a0, 0, 0,
                                  g_pRearViewBackdrop_005a7c94, 0);
        } else {
            backdrop = FetchDiskPacketRetrying(8, 7, 0);
            if (g_nCockpitDisplayMode_0049d71c == 0)
                DrawSpriteDefault(&g_stScreenViewport_005d21a0, 0, 0, backdrop, 0);
            if (backdrop != 0)
                ReleasePacketHandle(backdrop);
        }
        set_up_screen_viewport(0);
        break;
    }

    g_bSpaceViewBufferEnabled_0049d7a4 = 1;
    SetViewportRect(&g_stViewBuffer_005d2b00, 0, 0,
                    (unsigned short)(g_nScreenWidth_0046daa4 - 1),
                    (unsigned short)(g_nScreenHeight_0046daa8 - 1));
#if 0
    result = initialize_view_buffer();
    g_stScreenViewport_005d21a0 = savedScreen;
    return result;
#else
    initialize_view_buffer();
    g_stScreenViewport_005d21a0 = savedScreen;
#endif
}

/* Function start: 0x457434 */
unsigned int InitializeConstellationObject(
    const ConstellationObjectDefinition *definition, short object)
{
    FixedVector position;

    g_aeObjectClass_00495328[object] = OBJECT_CLASS_PLANET;
    init_ijk(63);
    alter_yaw((short)-definition->yaw, 63);
    alter_pitch((short)-definition->pitch, 63);
    alter_roll(definition->roll, 63);
    ScaleFixedVector(&g_aShipForwardVector_00494208[63],
                     0x753000, &position);
    g_aShipPosition_00494550[object] = position;
    g_asObjectScreenScale_0059c950[object] = 0xff;
    g_asObjectScreenAngle_0059cd90[object] = 0;
    g_asObjectViewFrame_0059d230[object] = 0;
    g_acObjectType_00493980[object] = OBJECT_TYPE_HORNET;
    g_apObjectShape_0059d2f0[object] =
        FetchDiskPacketRetrying(
            12, (short)(definition->shapePacket + 1), 0);
    return 0;
}

/* Function start: 0x457587 */
void FreeConstellationObject(short object)
{
#if 0
    FreePacketAndClear(&g_apObjectShape_0059d2f0[object], 0);
    remove_object(object);
    return 0;
#else
    FreePacketAndClear(&g_apObjectShape_0059d2f0[object], 0);
    remove_object(object);
#endif
}

/* Function start: 0x4575B4 */
unsigned int init_constellation(short scene)
{
    short slot;
    short object;
    int definitionBase;

    if (g_pConstellationShape_005a765c != 0)
        return 0;

    scene--;
    g_pConstellationShape_005a765c =
        FetchDiskPacketRetrying(12, 0, 0);
    if (g_nTrainSimActive_00469e2c != 0 || scene < 0)
        return;

    definitionBase = (int)scene * 4;
    slot = 0;
    do {
        if (g_pConstellationDefinitions_00598a28[
                definitionBase + slot].shapePacket != -1) {
            object = find_vacant_3d_object();
            if (object != -1) {
                InitializeConstellationObject(
                    &g_pConstellationDefinitions_00598a28[
                        definitionBase + slot],
                    object);
            }
            g_asConstellationObjectIndices_00469d50[slot] = object;
        } else {
            g_asConstellationObjectIndices_00469d50[slot] = -1;
        }
        slot++;
    } while (slot < 4);
    /* The successful path returns the last expression left in EAX. */
}

/* Function start: 0x4576AB */
void free_constellation(void)
{
    short object;
    short slot;

#if 0
    slot = 0;
    FreePacketAndClear(&g_pConstellationShape_005a765c, 0);
    do {
        object = g_asConstellationObjectIndices_00469d50[slot];
        if (object != -1) {
            FreeConstellationObject(object);
            g_asConstellationObjectIndices_00469d50[slot] = -1;
        }
        slot++;
    } while (slot < 4);
    return 0;
#else
    FreePacketAndClear(&g_pConstellationShape_005a765c, 0);
    for (slot = 0; slot < 4; slot++) {
        object = g_asConstellationObjectIndices_00469d50[slot];
        if (object != -1) {
            FreeConstellationObject(object);
            g_asConstellationObjectIndices_00469d50[slot] = -1;
        }
    }
#endif
}

/* Function start: 0x457720 */
void init_vdus(void)
{
    DAT_005a7530 = g_stScreenViewport_005d21a0;
    DAT_005a6b80 = g_stScreenViewport_005d21a0;

    InitializeTextContextFromFont(
        &DAT_005a74f0, 2, g_ucPrimaryTextColour_0049cb64, (signed char)g_cSecondaryViewBufferColour_0049cb4c);
    DAT_005a74f0.viewport = &DAT_005a6b80;
    DAT_005a74f0.text = g_szDefaultTextBuffer_005d2b80;
    set_mode(0, 1);
    DAT_0059ce18[0] = 0;

    InitializeTextContextFromFont(
        &DAT_005a7700, 2, g_ucPrimaryTextColour_0049cb64, (signed char)g_cSecondaryViewBufferColour_0049cb4c);
    DAT_005a7700.viewport = &DAT_005a7530;
    DAT_005a7700.text = g_szDefaultTextBuffer_005d2b80;
    set_mode(1, g_nTrainSimActive_00469e2c == 0 ? 5 : 3);
    DAT_0059ce18[1] = 0;
}

/* Function start: 0x4577D7 */
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
    free_view_buffer();

    LoadShapeSet(g_aCockpitPrimaryResources_00469d08, 4,
                 (short)g_cCockpitLogicalFile_005a7c74);
    g_pScreenViewportPacket_005a6b94 =
        LoadPacketAllocated(
            (short)g_cCockpitLogicalFile_005a7c74, 6);
    DAT_005a6be0 = g_stScreenViewport_005d21a0;
    init_vdus();

    InitializeTextContextFromFont(
        &DAT_005a7720, 2, g_ucPrimaryTextColour_0049cb64, (signed char)g_cSecondaryViewBufferColour_0049cb4c);
    DAT_005a7720.viewport = &g_stScreenViewport_005d21a0;
    SetTextContext(&DAT_005a7720);

    layout = &g_stCockpitLayout_0046e008;
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
        DAT_005a6b60 = g_stScreenViewport_005d21a0;
        bounds = &layout->pilotHandBounds[(int)g_cCockpitView_0059dab0];
        DAT_005a6b60.left = bounds->left;
        DAT_005a6b60.top = bounds->top;
        DAT_005a6b60.right = bounds->right;
        DAT_005a6b60.bottom = bounds->bottom;
        SetViewportRect(&DAT_005a7690, 0, 0,
                        (unsigned short)(bounds->right - bounds->left),
                        (unsigned short)(bounds->bottom - bounds->top));
        DAT_005a7550 = DAT_005a7690;
        AllocateViewport(&DAT_005a7690, g_cSecondaryViewBufferColour_0049cb4c, 0);
        AllocateViewport(&DAT_005a7550, g_cSecondaryViewBufferColour_0049cb4c, 0);
    }

    LoadShapeSet(g_aCockpitSecondaryResources_00469ce0, 0,
                 (short)g_cCockpitLogicalFile_005a7c74);
    backgroundSize = MeasureShapeFrameStorage(
        g_pTargetLockShape_005a6bf4, 2);
    g_pScannerMarkerBackground_005a7dc4 =
        AllocateTaggedMemory(backgroundSize, 0);

    g_pReleaseWeaponDisplayBackground_0046906c = 0;
    maximumSize = 0;
    for (weapon = 0;
         weapon < (signed char)g_aShipWeapons_0059cab0[0][0];
         weapon++) {
        weaponSlot = &((ShipWeaponSlot *)
            &g_aShipWeapons_0059cab0[0][1])[weapon];
        if (g_aObjectTypeData_00496d30[weaponSlot->type].objectClass ==
                OBJECT_CLASS_MISSILE) {
            backgroundSize = MeasureShapeFrameStorage(
                g_pCockpitWeaponShape_005a7564,
                (short)(weaponSlot->type * 2 - 0x2f));
            maximumSize = MaxShort(maximumSize, (short)backgroundSize);
        }
    }
    if (maximumSize != 0)
        g_pReleaseWeaponDisplayBackground_0046906c =
            AllocateTaggedMemory(maximumSize, 0);

    g_pCockpitExplosionBackground_00469060 = 0;
    if (g_pCockpitExplosionShape_00469064 != 0) {
        maximumSize = 0;
        for (frame = 0; frame < 8; frame++) {
            backgroundSize = MeasureShapeFrameStorage(
                g_pCockpitExplosionShape_00469064, frame);
            maximumSize = MaxShort(maximumSize, (short)backgroundSize);
        }
        if (maximumSize != 0)
            g_pCockpitExplosionBackground_00469060 =
                AllocateTaggedMemory(maximumSize, 0);
    }

    g_pDamageDisplayBackground_0046a748 = 0;
    maximumSize = 0;
    for (frame = 0; frame < 9; frame++) {
        backgroundSize = MeasureShapeFrameStorage(
            g_pCockpitWeaponShape_005a7564, frame);
        maximumSize = MaxShort(maximumSize, (short)backgroundSize);
    }
    if (maximumSize != 0)
        g_pDamageDisplayBackground_0046a748 =
            AllocateTaggedMemory(maximumSize, 0);

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

/* Function start: 0x458196 */
unsigned int free_cockpit(void)
{
    if (g_bCockpitResourcesActive_00469d58 == 0)
        return 0;
    g_bCockpitResourcesActive_00469d58 = 0;
    free_view_buffer();
    if (g_pScreenViewportPacket_005a6b94 != 0) {
        ReleasePacketHandle(g_pScreenViewportPacket_005a6b94);
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
    FreePacketAndClear(&g_pCockpitPilotShape_0046905c, 0);
    FreePacketAndClear(&g_pReleaseWeaponDisplayBackground_0046906c, 0);
    FreePacketAndClear(&g_pCockpitExplosionBackground_00469060, 0);
    FreePacketAndClear(&g_pDamageDisplayBackground_0046a748, 0);
    FreeCommDisplayResources();
    ReleasePacketHandle(g_pScannerMarkerBackground_005a7dc4);
}

/* Function start: 0x458467 */
void init_3Space_objects(short scene)
{
    short slot;

#if 0
    if (g_b3SpaceObjectsActive_0049c8ec == 1)
        return 0;
    g_b3SpaceObjectsActive_0049c8ec = 1;
    g_cScreenViewportMode_005c82a6 = -1;
    remove_all_3d_objects();
    g_nExternalViewShip_00493468 = -1;
    g_nRenderedSpaceFrame_00493138 = 0;
    g_bScriptedView_0046a8d4 = 0;
    g_nSpaceFrame_00493134 = 0;
    g_bMissileCameraEnabled_00493504 = 0;
    g_nClosestVisibleObject_0046c048 = -1;
    g_nPlayerCollisionObject_00493480 = -1;
    slot = 0;
    do {
        g_aObjectResourceSlots_00493398[slot].type = -1;
        slot++;
    } while (slot <= 3);
    init_constellation(scene);
    return load_common_3Space_objects();
#else
    if (g_b3SpaceObjectsActive_0049c8ec == 1)
        return;
    g_b3SpaceObjectsActive_0049c8ec = 1;
    g_cScreenViewportMode_005c82a6 = -1;
    remove_all_3d_objects();
    g_nSpaceFrame_00493134 =
        g_nRenderedSpaceFrame_00493138 =
            (short)(g_nNextAfterburnerSfxFrame_005d3820 =
                g_nNextMissileWarningSfxFrame_005d156c = 0);
    g_bMissileCameraEnabled_00493504 = 0;
    g_nExternalViewShip_00493468 = -1;
    g_nPlayerCollisionObject_00493480 = -1;
    for (slot = 0; slot <= 4; slot++)
        g_aObjectResourceSlots_00493398[slot].type = -1;
    init_constellation(scene);
    load_common_3Space_objects();
#endif
}

/* Function start: 0x458532 */
unsigned int load_common_3Space_objects(void)
{
    LoadShapeSet(g_aCommon3SpaceResources_0049c728, 0, -1);
    g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_O_RING].shapeSet =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_GLASS].shapeSet =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_SHIP_TUBING].shapeSet =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK].shapeSet =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_TURRET].shapeSet =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_LASER_CANNON].shapeSet;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_TURRET].animation =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_LASER_CANNON].animation;

    load_ship(OBJECT_TYPE_HEAT_SEEKING_MISSILE, 0, 0, 3);
    g_aObjectTypeData_00496d30[OBJECT_TYPE_DUMB_FIRE_MISSILE].shapeSet =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_HEAT_SEEKING_MISSILE].shapeSet;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE].shapeSet =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_HEAT_SEEKING_MISSILE].shapeSet;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_FF_MISSILE].shapeSet =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_HEAT_SEEKING_MISSILE].shapeSet;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_DUMB_FIRE_MISSILE].animation =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_HEAT_SEEKING_MISSILE].animation;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE].animation =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_HEAT_SEEKING_MISSILE].animation;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_FF_MISSILE].animation =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_HEAT_SEEKING_MISSILE].animation;
    return 0;
}

/* Function start: 0x45865D */
void remove_all_3d_objects(void)
{
    short i;

#if 0
    i = 0;
    do {
        remove_object(i);
        i = i + 1;
    } while (i < 0x40);
    return 0;
#else
    for (i = 0; i < 70; i++)
        remove_object(i);
#endif
}

/* Function start: 0x458698 */
void free_3Space(void)
{
#if 0
    if (g_b3SpaceObjectsActive_0049c8ec == 0)
        return 0;
    g_b3SpaceObjectsActive_0049c8ec = 0;
    free_view_buffer();
    g_bSpaceViewBufferEnabled_0049d7a4 = 0;
    free_constellation();
    remove_all_hazards();
    remove_all_3d_objects();
    free_3Space_objects();
    return 0;
#else
    if (g_b3SpaceObjectsActive_0049c8ec == 0)
        return;
    g_b3SpaceObjectsActive_0049c8ec = 0;
    free_view_buffer();
    g_bSpaceViewBufferEnabled_0049d7a4 = 0;
    free_constellation();
    remove_all_hazards();
    free_3Space_objects();
    FreeShapeSet(g_aMissionResourceDescriptors_0049c798, 0);
    if (g_pNavLocationText_0049bc54 != 0) {
        ReleasePacketHandle(g_pNavLocationText_0049bc54);
        g_pNavLocationText_0049bc54 = 0;
    }
#endif
}

/* Function start: 0x458716 */
unsigned int free_3Space_objects(void)
{
    FreeShapeSet(g_aCommon3SpaceResources_0049c728, 0);
    FreeShapeSet(g_aMissionResourceDescriptors_0049c798, 0);
    free_ship(3);
    g_aObjectTypeData_00496d30[OBJECT_TYPE_DUMB_FIRE_MISSILE].shapeSet = 0;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE].shapeSet = 0;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_FF_MISSILE].shapeSet = 0;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_DUMB_FIRE_MISSILE].animation = 0;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE].animation = 0;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_FF_MISSILE].animation = 0;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_O_RING].shapeSet =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_GLASS].shapeSet =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_SHIP_TUBING].shapeSet =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK].shapeSet =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_WING].shapeSet =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet;
    return 0;
}

/* Function start: 0x4587E0 */
void init_inflight_music(void)
{
    g_nInFlightMusicActive_0049bf08 = 1;
    g_nInitialFlightMusicPending_0049bf00 = 1;
    g_nCombatMusicActive_0049bf04 = 0;

#if 0
    g_nCombatMusicActive_0049bf04 = 0;
    g_nInFlightMusicActive_0049bf08 = 1;
    g_nInitialFlightMusicPending_0049bf00 = 1;
    return 0;
#endif
}

/* Function start: 0x458806 */
void free_inflight_music(void)
{
#if 0
    int slot;

    StopMusicUnlessSuppressed();
    g_nInFlightMusicActive_0049bf08 = 0;
    if (g_nInFlightMusicSlotA_0049bea4 != -1) {
        slot = g_nInFlightMusicSlotA_0049bea4;
        g_nInFlightMusicSlotA_0049bea4 = -1;
        *(int *)(g_abSoundPlaybackSlots_005d13e0 + slot * 6) = 0;
    }
    if (g_nInFlightMusicSlotB_0049beaa != -1) {
        slot = g_nInFlightMusicSlotB_0049beaa;
        g_nInFlightMusicSlotB_0049beaa = -1;
        *(int *)(g_abSoundPlaybackSlots_005d13e0 + slot * 6) = 0;
    }
    return 0;
#else
    StopMusicUnlessSuppressed();
    g_nInFlightMusicActive_0049bf08 = 0;
    if (g_nInFlightMusicSlotA_0049bea4 != -1) {
        *(int *)(g_abSoundPlaybackSlots_005d13e0 +
                 g_nInFlightMusicSlotA_0049bea4 * 6) = 0;
        g_nInFlightMusicSlotA_0049bea4 = -1;
    }
    if (g_nInFlightMusicSlotB_0049beaa != -1) {
        *(int *)(g_abSoundPlaybackSlots_005d13e0 +
                 g_nInFlightMusicSlotB_0049beaa * 6) = 0;
        g_nInFlightMusicSlotB_0049beaa = -1;
    }
#endif
}

/* Function start: 0x469AD0 */
unsigned int PreloadMusicTrackHook(short track)
{
    (void)track;
    return 0;
}

/* Function start: 0x469B49 */
unsigned int ReleaseMusicTrackHook(short track)
{
    (void)track;
    return 0;
}

/* Function start: WC2_UNMAPPED */
unsigned short LoadSceneAnimationResources(short scene, short variant)
{
    unsigned char *packet;
    short logicalFile;

    logicalFile = g_asSceneAnimationLogicalFiles_00469d60[scene];
    g_pSceneAnimationPrimaryShape_005a7c58 =
        FetchDiskPacketRetrying(logicalFile, 0, 0);
    g_pSceneAnimationDefinitions_005a7c6c =
        FetchDiskPacketRetrying(
            logicalFile, (short)(variant + 1), 0);
    g_pSceneAnimationSecondaryShape_005a7c70 =
        FetchDiskPacketRetrying(
            logicalFile, (short)(variant + 3), 0);
    packet = FetchDiskPacketRetrying(
        logicalFile, (short)(variant + 5), 0);
    g_pSceneAnimationPacket_005a7c60 = packet;
    g_pSceneAnimationSceneData_005a7c54 =
        packet + *(unsigned int *)(packet + 0);
    g_pSceneAnimationTextData_005a7c5c =
        packet + *(unsigned int *)(packet + 4);
    g_pSceneAnimationObjects_005a7c64 =
        (SceneAnimationObject *)(g_pSceneAnimationDefinitions_005a7c6c + 2);
    return 0;
}

/* Function start: WC2_UNMAPPED */
void ReleaseSceneAnimationResources(void)
{
    ReleasePacketHandle(g_pSceneAnimationPrimaryShape_005a7c58);
    ReleasePacketHandle(g_pSceneAnimationPacket_005a7c60);
    ReleasePacketHandle(g_pSceneAnimationSecondaryShape_005a7c70);
    ReleasePacketHandle(g_pSceneAnimationDefinitions_005a7c6c);
}

/* Function start: WC2_UNMAPPED */
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

/* Function start: WC2_UNMAPPED */
short __stdcall SceneAnimationGoalReached(short delta, short current,
                                          short goal)
{
    if (delta < 0) {
        if (current <= goal)
            return 1;
    } else if (delta > 0) {
        if (current >= goal)
            return 1;
    } else if (current == goal) {
        return 1;
    }
    return 0;
}

/* Function start: WC2_UNMAPPED */
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
                if (object->layer != 2 && g_nFrameSkipCountdown_0049d760 < 1)
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
                if (object->layer != 2 && g_nFrameSkipCountdown_0049d760 < 1)
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

/* Function start: WC2_UNMAPPED */
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
                  g_cSecondaryViewBufferColour_0049cb4c);
    FormatTextBufferFromStart(g_szSceneAnimationTextFormat_00469d74,
                              0, 160,
                              g_nConversationTextColour_00598c10,
                              g_szTextScratchBuffer_005d1c40);

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

    g_nFrameSkipCountdown_0049d760 = 1;
    g_bSceneEscapeRequested_0049d4b0 = 0;
    ClearInputKeyState();
    DIBslam();
    DIBslamReal();
    for (;;) {
        do {
            g_nFrameSkipCountdown_0049d760--;
            object = objects;
            remaining = objectCount;
            while (remaining > 0) {
                complete |= (unsigned short)
                    UpdateSceneAnimationObject(object, &g_stSecondaryViewBuffer_005d2c90);
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
            if (g_nFrameSkipCountdown_0049d760 == 0) {
                g_nFrameSkipCountdown_0049d760 = g_nFrameSkip_0049d764;
                if (g_bSlowSceneAnimation_00469998 != 0)
                    g_nFrameSkipCountdown_0049d760++;
            }
            if ((complete == 0 && CheckEscaped() != 0) ||
                g_bSceneEscapeRequested_0049d4b0 != 0) {
                if (g_nSceneAnimationWaitFrames_005a7c68 == -1) {
                    while (complete == 0 &&
                           g_bSceneAnimationWaitCommand_00469d70 == 0) {
                        object = objects;
                        remaining = objectCount;
                        while (remaining > 0) {
                            g_nFrameSkipCountdown_0049d760 = 2;
                            complete |= (unsigned short)
                                UpdateSceneAnimationObject(
                                    object, &g_stSecondaryViewBuffer_005d2c90);
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
            } while (g_bSceneEscapeRequested_0049d4b0 == 0);
        }
        if (complete != 0)
            return;
    }
}
