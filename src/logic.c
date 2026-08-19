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

    for (weapon = 0;
         weapon < (signed char)aShipWeapons[obj][0];
         weapon++) {
        ShipWeaponSlot *weaponSlot =
            &((ShipWeaponSlot *)&aShipWeapons[obj][1])[weapon];

        if (weaponSlot->type == weaponType)
            return weapon;
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
    slot = (ShipWeaponSlot *)&aShipWeapons[ship][1];
    weaponCount = (signed char)aShipWeapons[ship][0];
    if (weaponCount > 0) {
        for (; weapon < weaponCount; weapon++, slot++) {
            if (aObjectTypeData[slot->type].objectClass ==
                    OBJECT_CLASS_MISSILE) {
                if (ship != 0)
                    return fire_weapon(ship, weapon);
                if (slot->disabled == 0) {
                    if ((slot->type == OBJECT_TYPE_HEAT_SEEKING_MISSILE ||
                         slot->type == OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE) &&
                        nTargetLockCountdown != 0) {
                        if (get_mode(0) == 1)
                            ShowComponentHitHudMessage(
                                szNeedLock,
                                cYellowColour, 3);
                        return -1;
                    }
                    return fire_weapon(0, weapon);
                }
            }
        }
    }
    return -1;
}

/* Function start: 0x421220 */
int fire_fixed_projectile_weapon(short obj)
{
    int loadoutOffset;
    short weapon;
    ShipWeaponSlot *slot;

    loadoutOffset = obj * sizeof(aShipWeapons[0]);
    weapon = 0;
    slot = (ShipWeaponSlot *)((unsigned char *)aShipWeapons +
                              loadoutOffset + 1);
    if (*(signed char *)((unsigned char *)aShipWeapons +
                         loadoutOffset) <= 0)
        return;
    do {
        if (aObjectTypeData[slot->type].objectClass ==
                OBJECT_CLASS_PROJECTILE &&
            slot->disabled == 0) {
            if (fire_weapon(obj, weapon) == -1)
                return -1;
        }
        weapon++;
        slot++;
    } while ((short)*(signed char *)((unsigned char *)aShipWeapons +
                                     loadoutOffset) > weapon);
    /* The original leaves EAX incidental on successful and empty paths. */
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
        &((ShipWeaponSlot *)&aShipWeapons[obj][1])[weapon];
    child_object(weaponSlot->hardpoint, mine, obj);
    if (lifetime == -1)
        lifetime = 20;
    acObjectCollisionGraceTicks[mine] =
        (signed char)lifetime;
    asObjectCounter[mine] = (short)(signed char)lifetime;
    if (obj == 0)
        RemovePlayerReleaseWeapon(weapon);
    else
        remove_weapon(obj, weapon);
    return mine;
}

/* Function start: 0x421350 */
void fire_afterburner(short obj, short time)
{
    short timer;
    long velocity;

    velocity = Vector_magnitude(&aShipVelocity[obj]);
    if (get_ship_max_velocity(obj) * 0x500L > velocity) {
        set_special(obj, SPECIAL_MANEUVER_AFTERBURNER);
        timer = 0;
        if (aeSpecialManeuver[obj] ==
            SPECIAL_MANEUVER_AFTERBURNER)
            timer = time;
        asShipAfterburnerTimer[obj] = timer;
    }
}

/* Function start: 0x4213B0 */
unsigned int fire_super_brake(short ship)
{
    asShipAfterburnerTimer[ship] = 10;
    set_special(ship, SPECIAL_MANEUVER_SUPER_BRAKE);
    return 0;
}

/* Function start: 0x4213D0 */
short flip_angle(short ship, short angle)
{
    short flip;

    flip = asObjectFlip[ship];
    if ((flip & 0x10) != 0)
        angle = (short)(180 - angle);
    if ((flip & 0x20) != 0)
        angle = (short)-angle;
    angle += asObjectScreenAngle[ship];
    angle %= 360;
    if (angle < 0)
        angle += 360;
    return angle;
}

/* Function start: 0x421430 */
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
        if (aeObjectClass[shipIndex] >= OBJECT_CLASS_MISSILE &&
            anShipSpeed[shipIndex] != 0 &&
            aeSpecialManeuver[shipIndex] !=
                SPECIAL_MANEUVER_KILL_ENGINES &&
            asObjectScreenX[shipIndex] != (short)0x8001) {
            animation = (short *)aObjectTypeData[
                aeObjectType[shipIndex]].animation;
            if (animation != 0) {
                object = animation[asObjectViewFrame[shipIndex]];
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
                        if (abShipExhaustHeat[shipIndex] == 0)
                            scale = (short)(scale - 32);
                        asObjectScale[object] = scale;
                        asObjectDistance[object] = *animation++;
                        asObjectScreenAngle[object] =
                            flip_angle(ship, *animation++);
                        asObjectFlip[object] = 0;
                        asObjectScreenX[object] = *animation++;
                        asObjectScreenY[object] = *animation++;
                        if (aeSpecialManeuver[shipIndex] ==
                                SPECIAL_MANEUVER_AFTERBURNER) {
                            asObjectViewFrame[object] =
                                (short)(frame * 3 +
                                        RandomInRange(0, 2));
                        } else {
                            asObjectViewFrame[object] =
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

/* Function start: 0x4215E0 */
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
        if (aeObjectClass[objectIndex] ==
                OBJECT_CLASS_FIXED_OBJECT) {
            parent = (short)acObjectOwner[objectIndex];
            if (aeObjectType[objectIndex] ==
                    OBJECT_TYPE_TURRET ||
                aeObjectType[objectIndex] ==
                    OBJECT_TYPE_THRUSTERS) {
                parentIndex = (int)parent;
                angle = asObjectScreenAngle[parentIndex];
                sine = SinFixed(angle);
                cosine = CosFixed(angle);
                parentScale =
                    asObjectScreenScale[parentIndex];
                right = (int)asObjectScreenX[objectIndex] *
                        (unsigned short)parentScale;
                asObjectDistance[objectIndex] +=
                    asObjectDistance[parentIndex];
                if ((asObjectFlip[parentIndex] & 0x10) != 0)
                    right = -right;
                up = (int)asObjectScreenY[objectIndex] *
                     (unsigned short)parentScale;
                if ((asObjectFlip[parentIndex] & 0x20) != 0)
                    up = -up;
                asObjectScreenX[objectIndex] = (short)(
                    (MultiplyFixed(right, (int)cosine) -
                     MultiplyFixed(up, (int)sine)) >> 8);
                asObjectScreenY[objectIndex] = (short)(
                    (MultiplyFixed(up, (int)cosine) +
                     MultiplyFixed(right, (int)sine)) >> 8);
                asObjectScreenX[objectIndex] +=
                    asObjectScreenX[parentIndex];
                asObjectScreenY[objectIndex] +=
                    asObjectScreenY[parentIndex];
#ifdef WC1_SDL
                if (aeObjectType[objectIndex] ==
                        OBJECT_TYPE_THRUSTERS) {
                    /* Match the anchor to the enhanced parent transform. */
                    parentScreenX =
                        (float)nViewCenterX +
                        (float)(((double)(nScreenWidth & ~1) *
                                 0.5 *
                                 aObjectViewPosition[parentIndex].x) /
                                aObjectViewPosition[parentIndex].z);
                    parentScreenY =
                        (float)nViewCenterY +
                        (float)(((double)(nScreenWidth & ~1) *
                                 0.5 *
                                 aObjectViewPosition[parentIndex].y) /
                                aObjectViewPosition[parentIndex].z);
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
            asObjectScreenScale[objectIndex] = (short)(
                (unsigned short)
                    asObjectScreenScale[parentIndex] *
                (unsigned short)
                    asObjectScale[objectIndex] >> 8);
        }
        object++;
    } while (object <= WC1_SPACE_LAST_MOVING_OBJECT);
    return 0;
}

/* Function start: 0x421760 */
unsigned int housekeep_power_plant_and_fuel(short ship)
{
    if (0 < anShipSpeed[ship])
        drain_fuel(ship, 5);
    return 0;
}

/* Function start: 0x421780 */
unsigned int replenish_shields(short ship)
{
    signed char shield;

    if (ship == 0 && acPlayerComponentDamage[1] > 0 &&
        nSpaceFrame %
            (acPlayerComponentDamage[1] + 1) != 0)
        return 0;
    shield = 0;
    do {
        short maximum;
        short current;

        maximum = aasShipMaximumShield[ship][shield];
        if (aasShipShield[ship][shield] > maximum)
            aasShipShield[ship][shield] = maximum;
        current = aasShipShield[ship][shield];
        if (current < maximum &&
            nSpaceFrame %
                aObjectTypeData[
                    aeObjectType[ship]].animationDelay == 0) {
            aasShipShield[ship][shield] =
                (short)(current + 1);
        }
        shield++;
    } while (shield <= 1);
    return 0;
}

/* Function start: 0x421830 */
unsigned int replenish_weapon_energy_bank(short ship)
{
    short energy;
    short shieldEnergy;
    short maximumShield;

    if (ship == 0 && acPlayerComponentDamage[1] != 0 &&
        (int)(unsigned short)RandomInRange(0, 4) <
            acPlayerComponentDamage[1])
        return 0;
    energy = asShipWeaponEnergy[ship];
    if (energy < 100) {
        shieldEnergy = aasShipShield[ship][1];
        maximumShield = aasShipMaximumShield[ship][1];
        shieldEnergy =
            (short)(shieldEnergy + aasShipShield[ship][0]);
        maximumShield =
            (short)(maximumShield +
                    aasShipMaximumShield[ship][0]);
        if (shieldEnergy < maximumShield) {
            asShipWeaponEnergy[ship] =
                MinShort((short)(energy + 1), 100);
        } else {
            asShipWeaponEnergy[ship] =
                MinShort((short)(energy + 2), 100);
        }
    }
    return 0;
}

/* Function start: 0x4218D0 */
void accelerate(short amount)
{
    if (malf(0) != 0) {
        amount = (short)(amount - 2);
        if (nSpaceFrame % 3 == 0)
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

    if (anShipFuel[0] <= 0)
        return;
    if (malf(0) != 0) {
        PlaySfxWaveFileByNumber(3, -1, 0);
        return;
    }
    if (aeSpecialManeuver[0] !=
            SPECIAL_MANEUVER_AFTERBURNER) {
        time = 8;
    } else if (asShipAfterburnerTimer[0] == 0) {
        time = 8;
    } else {
        if (asShipAfterburnerTimer[0] > 2)
            return;
        time = 2;
    }
    fire_afterburner(0, time);
    frame = (int)nSpaceFrame;
    nextSoundFrame = frame + 6;
    if (nextSoundFrame < nAfterburnerSoundDeadline)
        nAfterburnerSoundDeadline = 0;
    if (nAfterburnerSoundDeadline < frame) {
        nAfterburnerSoundDeadline = nextSoundFrame;
        PlaySfxWaveFileByNumber(12, -1, 0);
    }
}

/* Function start: 0x4219C0 */
unsigned int LoadGamePaletteFile(void)
{
    short index;

    PromptInsertNumberedDisk(0);
    switch ((int)(short)nVideoMode) {
    case 9:
    case 13:
        index = 0;
        do {
            ((unsigned char *)&cBlueColour)[index] =
                abLegacyVideoModeColours[index];
            index++;
        } while ((unsigned int)(int)index < 14);
        index = 0;
        do {
            asConversationTextColours[index] =
                asConversationTextColours[index + 12];
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

/* Function start: 0x421A40 */
unsigned int EMShutDown(void)
{
    if (nEventManagerActive != 0)
        ShutdownEventManager();
    return 0;
}

/* Function start: 0x421A60 */
unsigned short InitializeEventManagerResources(void)
{
    DAT_0059a9f0 = 20;
    nInputTickScale = 20;
    DAT_0059ab64 = 1;
    stMouseCursorState.shape = pMouseCursorResource =
        FetchDiskPacketRetrying(14, 0, 0x10);
    stMouseCursorState.frame = 0;
    stMouseCursorState.viewport = &stScreen;
    return 0;
}

/* Function start: 0x421AB0 */
unsigned int EMStartUp(void)
{
    PromptInsertNumberedDisk(0);
    RegisterEventManagerShutdown((void (*)(void))LogMemoryUsage);
    if (InitializeEventManager(20, InitializeEventManagerResources, 0) == 0)
        exit_squadron("EMStartUp Failed");
    ConfigureEventManagerPointer(stMouseCursorState.shape, 0);
    SetEventManagerPump(PollJoystickButtonEvents);
    nMenuInputRepeatDelay = 6;
    return 0;
}

/* Function start: 0x421B10 */
unsigned int LoadOriginFxDrivers(void)
{
    int memoryThreshold;
    int videoModeMemory;
    short requestedGraphicsMode;

    memoryThreshold = 100000;
    _chdir("gamedat");
    nNearHeapMaxDescriptors = 0x80;
    IsSoundHardwarePresent(8);
    if (bExpandedMemoryAvailable == 0)
        SystemDebugPrintf("No ");
    SystemDebugPrintf("Expanded Memory Detected.\n");
    bGraphicsActive = 0;
    PromptInsertNumberedDisk(0x38);
    if (GetMusicDriverPresent(bSlowSceneAnimation) == 0)
        exit_squadron("Failed to load Origin-FX drivers");
    RewriteDiskFileGraphicsExtensions(bSlowSceneAnimation);
    LoadJoystickCalibrationFile(9, 9, 1, 1);
    nInputDoubleClickInterval = 2;
    EMStartUp();
    GetFxDriverInitResult();
    dwOriginalFreeMemory = GetFixedOneMillionThunkAlt(0);
    if (nMusicPlaybackMode != 0 && nMusicPlaybackMode != 3)
        memoryThreshold = 210000;
    SetFrameTimerPeriodDirect(0x78);
    if ((int)GetFixedOneMillionThunk(4) > memoryThreshold) {
        nAvailableGameMemory =
            (int)dwOriginalFreeMemory -
            anExpandedMemoryReservationByVideoMode[
                bSlowSceneAnimation];
        if (nAvailableGameMemory < 0)
            exit_squadron(
                "You do not have enough memory to play Wing Commander.\n"
                "Refer to your reference guide for assistance.");
        nMemoryConfiguration = 2;
        SystemDebugPrintf("Expanded Memory fully used.\n");
    } else {
        nAvailableGameMemory =
            (int)dwOriginalFreeMemory -
            anBaseMemoryReservationByVideoMode[
                bSlowSceneAnimation];
        if (nAvailableGameMemory < 0)
            exit_squadron(
                "You do not have enough memory to play Wing Commander.\n"
                "Refer to your reference guide for assistance.");
        nMemoryConfiguration = 0;
        if (nMusicPlaybackMode == 1 || nMusicPlaybackMode == 2) {
            videoModeMemory =
                anFullMusicMemoryReservationByVideoMode[
                    bSlowSceneAnimation];
            if ((int)dwOriginalFreeMemory > videoModeMemory) {
                nMemoryConfiguration = 1;
                nAvailableGameMemory =
                    (int)dwOriginalFreeMemory - videoModeMemory;
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
        acGraphicsModeByAnimationSpeed[
            bSlowSceneAnimation]);
    requestedGraphicsMode =
        acGraphicsModeByAnimationSpeed[
            bSlowSceneAnimation];
    if (GetTargetColourIndex() != requestedGraphicsMode)
        exit_squadron("Requested graphics display mode not available.");
    LoadGamePaletteFile();
    InitializeGameTextContexts();
    InitializeDiskPromptTextContext();
    GetEventManagerStatus();
    nFrameSkip = 1;
    if (nMusicPlaybackMode != 0 && nMusicPlaybackMode != 3)
        GetFxDriverStatus();
    initialize_direction_view_frames();
    pConstellationDefinitions =
        LoadPacketAllocated(0x3a, 0);
    pMissionCampaignData = LoadPacketAllocated(0x3a, 1);
    SystemDebugPrintf("\n[SYSTEM] : Exiting initialize()\n");
    return 0;
}

/* Function start: 0x421D80 */
unsigned int InitializeGameTextContexts(void)
{
    InitializeDIBScreenViewport(&stScreen,
                                (unsigned char)cBlackColour);
    stModalSourceViewport = stScreen;
    stDefaultTextContext.viewport =
        &stModalSourceViewport;
    stDefaultTextContext.text =
        szDefaultTextBuffer;
    InitializeTextContextFromFont(&stDefaultTextContext, 1,
                                  (unsigned char)cPrimaryTextColour,
                                  cBlackColour);
    SetTextContext(&stDefaultTextContext);

    stHudMessageTextContext.viewport = &stSpaceBuffer;
    stHudMessageTextContext.text = szDefaultTextBuffer;
    stHudMessageTextContext.alignment = 2;
    InitializeTextContextFromFont(&stHudMessageTextContext, 1,
                                  (unsigned char)cRedColour, -1);
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
    aDirectionViewRightVector[index] = right;
    aDirectionViewUpVector[index] = up;
    aDirectionViewForwardVector[index] = forward;
    return 0;
}

/* Function start: 0x421EF0 */
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

/* Function start: 0x421F50 */
unsigned int LoadSpaceflightResources(void)
{
    unsigned char *debrisShapeSet;

    if (bExpandedMemoryAvailable == 0)
        return 0;
    if (LoadShapeSet(aCommon3SpaceResources, 4, -1) == 0)
        return 0;
    LoadShapeSet(aMissionResourceDescriptors, 4, -1);
    aObjectTypeData[OBJECT_TYPE_DEBRIS_WING].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet;
    debrisShapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_O_RING].shapeSet =
        debrisShapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_GLASS].shapeSet =
        debrisShapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_SHIP_TUBING].shapeSet =
        debrisShapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK].shapeSet =
        debrisShapeSet;
    LoadShapeSet(aCockpitResourceDescriptors, 4, -1);
    apCommPortraitShapes[8] = 0;
    apCommPortraitShapes[9] = 0;
    apCommPortraitShapes[10] = 0;
    apCommPortraitShapes[11] = 0;
    apCommPortraitShapes[13] = 0;
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
short ace_status(short ace, unsigned char bits)
{
    return (stCampaignState.aceFlags[ace] & bits) == bits;
}

/* Function start: 0x422030 */
void unflag_ace(short ace, unsigned char bits)
{
    stCampaignState.aceFlags[ace] &= ~bits;
}

/* Function start: 0x422050 */
void flag_ace(short ace, unsigned char bits)
{
    stCampaignState.aceFlags[ace] |= bits;
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
    short ace = (short)aiPilotLevel[obj] - 14;

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
    return RandomBelowOrEqual((short)(range * 2)) - range;
}

/* Function start: 0x422110 */
int alert_flag(short ship, unsigned int bits)
{
    return (anShipAlertFlags[ship] & bits) != 0;
}

/* Function start: 0x422130 */
/* NOT a constant return: `mov eax,0x59b430 / cmp eax,1 / sbb eax,eax / inc eax`
 * is the classic `>= 1` boolean idiom applied to the *address* 0x0059B430,
 * which is always non-zero -- hence Ghidra folding it to `return 1`.  Written
 * the way the original was, so the idiom survives. */
unsigned int HasSpeechBuffer(void)
{
    return (unsigned int)&anShipAlertFlags >= 1;
}

/* Function start: 0x422140 */
unsigned short set_alert(short i, unsigned int bits)
{
    anShipAlertFlags[i] |= bits;
    return 0;
}

/* Function start: 0x422160 */
void clear_alert(short i)
{
    asCollisionCountdown[i] = 0;
    anShipAlertFlags[i] = 0;
    abCollisionAlertTarget[i] = 0xff;
}

/* Function start: 0x422180 */
void start_collision_alert(short obj, short other)
{
    if ((short)(signed char)abCollisionAlertTarget[obj] != other) {
        abCollisionAlertTarget[obj] = (unsigned char)other;
        steady_object(obj);
    }
    asCollisionCountdown[obj] = 3;
    set_alert(obj, 1);
    asShipAfterburnerTimer[obj] = 0;
    set_special(obj, SPECIAL_MANEUVER_NONE);
}

/* Function start: 0x4221E0 */
void try2end_collision_alert(short obj)
{
    asCollisionCountdown[obj]--;
    if (asCollisionCountdown[obj] <= 0)
        clear_alert(obj);
    else
        set_alert(obj, 2);
}

/* Function start: 0x422220 */
short normal_speed(short obj)
{
    if (aeSpecialManeuver[obj] !=
            SPECIAL_MANEUVER_AFTERBURNER &&
        real_velocity(obj) <= asShipMaximumSpeed[obj])
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

    collisionRadius = asObjectCollisionRadius[obj];
    collisionRadius += asObjectCollisionRadius[other];
    collisionRadius += 30;
    collisionFound = 0;
    ComputeVectorDelta(&aShipPosition[obj],
                       &aShipPosition[other],
                       &relativePosition);
    distance = Vector_magnitude(&relativePosition);
    if (((long)collisionRadius + 1500) * 0x100 < distance)
        return 0x7fff;

    ComputeVectorDelta(&aShipVelocity[obj],
                       &aShipVelocity[other],
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

/* Function start: 0x422440 */
void clear_crash_cache(void)
{
    short i = 0;

    do {
        asCollisionPartner[i] = -1;
        i = i + 1;
    } while (i < 10);
}

/* Function start: 0x422460 */
short crash_time(short obj, short other)
{
    if (asCollisionPartner[obj] == other)
        return asCollisionTime[obj];
    if (other < 10 && asCollisionPartner[other] == obj)
        return asCollisionTime[other];
    if (aeObjectClass[other] == OBJECT_CLASS_ASTEROID &&
        (asObjectScreenX[other] == -0x7fff ||
         asObjectScreenX[obj] == -0x7fff))
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
    for (other = 0; other <= 60; other++) {
        if (other != obj &&
            aeObjectClass[other] >= OBJECT_CLASS_ASTEROID &&
            aeObjectClass[other] != OBJECT_CLASS_MISSILE) {
            time = crash_time(obj, other);
            if (closestTime > time) {
                closestTime = time;
                candidate = other;
            }
        }
    }
    if (candidate != -1) {
        asCollisionPartner[obj] = candidate;
        asCollisionTime[obj] = closestTime;
    }
    return candidate;
}

/* Function start: 0x422560 */
unsigned int unactive(short ship)
{
    if (ship != -1 &&
        aeObjectClass[ship] >= OBJECT_CLASS_SHIP &&
        aeShipManeuver[ship] != MANEUVER_HARD_BRAKE)
        return 0;
    return 1;
}

/* Function start: 0x422590 */
int are_alive(short obj)
{
    if (unactive(obj) == 0 &&
        aeShipObjective[obj] != OBJECTIVE_WANDER)
        return 1;
    return 0;
}

/* Function start: 0x4225C0 */
int trim_goals(short obj, short amount)
{
    short goal = anYawGoal[obj];

    if (amount < goal)
        anYawGoal[obj] = amount;
    else if (goal < -amount)
        anYawGoal[obj] = -amount;
    goal = anPitchGoal[obj];
    if (amount < goal)
        anPitchGoal[obj] = amount;
    else if (goal < -amount)
        anPitchGoal[obj] = -amount;
    return 0;
}

/* Function start: 0x422640 */
int report_kilrathi_rout(int mode)
{
    short obj;

    for (obj = 0; obj < 10; obj++) {
        if (aeObjectClass[obj] < OBJECT_CLASS_SHIP ||
            aeSpecialManeuver[obj] ==
                SPECIAL_MANEUVER_UNKNOWN_9)
            continue;
        switch (mode) {
        case 0:
            if (aeShipSide[obj] == SIDE_KILRATHI)
                return 1;
            break;
        case 1:
            if (aeShipSide[obj] == SIDE_KILRATHI &&
                aeShipMissionType[obj] != MISSION_TYPE_ROUT &&
                distance_from_object(0, obj) < 16000)
                return 1;
            break;
        case 2:
            if (aeShipSide[obj] == SIDE_KILRATHI &&
                (aeShipObjective[obj] ==
                     OBJECTIVE_ENGAGE_ENEMY ||
                 aeShipObjective[obj] ==
                     OBJECTIVE_DESTROY_SHIP))
                return 1;
            break;
        }
    }
    if (mode == 1 && nCurrentWave != -1)
        check_next_wave();
    return 0;
}

/* Function start: 0x422710 */
short find_ship_index(short missionShip)
{
    short obj;

    nLastFoundShip = -1;
    if (missionShip != -1) {
        for (obj = 0; obj < 10; obj++) {
            if (((aeObjectClass[obj] >= OBJECT_CLASS_SHIP &&
                  aeSpecialManeuver[obj] !=
                      SPECIAL_MANEUVER_UNKNOWN_9) ||
                 aeObjectClass[obj] == OBJECT_CLASS_FUTURION) &&
                nShipMissionIndices[obj] == missionShip) {
                nLastFoundShip = obj;
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
    if (nTrainSimActive != 0) {
        canContinue = 1;
    } else {
        other = 0;
        do {
            if (aeObjectClass[other] ==
                    OBJECT_CLASS_CAPITAL_SHIP &&
                aeSpecialManeuver[other] !=
                    SPECIAL_MANEUVER_UNKNOWN_9 &&
                aeShipSide[obj] ==
                    aeShipSide[other])
                canContinue = 1;
            other++;
        } while (other < 10);
    }
    if (canContinue != 0) {
        acShipStress[obj] = 0;
        maneuver_complete(obj);
    } else {
        reset_mission_type(obj, MISSION_TYPE_ROUT);
        if (obj == nYourWingman)
            send_message(obj, 9);
    }
    return canContinue == 0;
}

/* Function start: 0x422830 */
signed char no_goal(short ship)
{
    return (anYawGoal[ship] |
            anPitchGoal[ship] |
            anRollGoal[ship]) == 0;
}

/* Function start: 0x422860 */
int being_tailed(short obj, short other)
{
    ship_vs_ship(obj, other);
    return nFacingToTarget < -60 &&
           nTargetFacing > 85 &&
           nTargetRange < 7000;
}

/* Function start: 0x4228A0 */
int any_enemy_tail(short obj)
{
    short other;

    for (other = 0; other < 10; other++) {
        if (aeObjectClass[other] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuver[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            aeShipSide[obj] != aeShipSide[other] &&
            acShipTarget[other] == obj &&
            being_tailed(obj, other) != 0) {
            nTargetShip = other;
            return 1;
        }
    }
    nTargetShip = -1;
    return 0;
}

/* Function start: 0x422930 */
short detect_enemy_tail(short obj)
{
    short other;

    for (other = 0; other < 10; other++) {
        if (aeObjectClass[other] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuver[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            aeShipSide[obj] != aeShipSide[other] &&
            acShipTarget[other] == obj &&
            being_tailed(obj, other) != 0)
            return other;
    }
    return -1;
}

/* Function start: 0x4229B0 */
int is_ship_tailing_player_target(short obj)
{
    short target = acShipTarget[0];

    if (unactive(target) == 0 && being_tailed(target, obj) != 0)
        return 1;
    return 0;
}

/* Function start: 0x4229F0 */
int missile_on_tail(short obj)
{
    short other;

    for (other = 0; other < 10; other++) {
        if (aeObjectClass[other] == OBJECT_CLASS_MISSILE &&
            acShipTarget[other] == obj)
            return 1;
    }
    return 0;
}

/* Function start: 0x422A30 */
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

/* Function start: 0x422A70 */
unsigned int build_squad_list(short leader)
{
    short obj;
    short index = 1;

    acFormationMemberList[0] = (signed char)leader;
    acFormationMemberList[1] = -1;
    for (obj = 0; obj < 10; obj++) {
        if (asShipWingLeader[obj] == leader) {
            acFormationMemberList[index++] = (signed char)obj;
            acFormationMemberList[index] = -1;
        }
    }
    return 0;
}

/* Function start: 0x422AC0 */
unsigned int find_squad_center(FixedVector *center)
{
    short count = 0;

    zero_vector(center);
    while (acFormationMemberList[count] != -1) {
        AddFixedVectors(center, &aShipPosition[
                            (short)acFormationMemberList[count]],
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
    member = acFormationMemberList[0];
    while (member != -1) {
        destination = &aShipDestination[member];
        ComputeVectorDelta(&center, &aShipPosition[member],
                           destination);
        ScaleFixedVector(destination, 0xa00, destination);
        AddFixedVectors(destination, &aShipPosition[member],
                        destination);
        steady_object(member);
        reset_objective(member, OBJECTIVE_BREAK_FORMATION);
        member = acFormationMemberList[++index];
    }
    return 0;
}

/* Function start: 0x422BE0 */
unsigned int reset_mission_type(short obj,
                                enum ShipMissionType missionType)
{
    if (missionType == MISSION_TYPE_ROUT &&
        aeShipSide[obj] == SIDE_KILRATHI)
        report_kilrathi_rout(1);
    reset_objective(obj, OBJECTIVE_NONE);
    aeShipMissionType[obj] = missionType;
    return 0;
}

/* Function start: 0x422C30 */
unsigned int change_mission_type(short obj,
                                 enum ShipMissionType missionType)
{
    if (aeShipObjective[obj] == OBJECTIVE_ENGAGE_ENEMY)
        aeShipMissionType[obj] = missionType;
    else
        reset_mission_type(obj, missionType);
    return 0;
}

/* Function start: 0x422C70 */
unsigned int reset_objective(short ship, enum ShipObjective objective)
{
    steady_object(ship);
    reset_tactic(ship, TACTIC_NONE);
    aeShipObjective[ship] = objective;
    return 0;
}

/* Function start: 0x422CA0 */
unsigned int alter_objective(short ship, enum ShipObjective objective)
{
    steady_object(ship);
    alter_tactic(ship, TACTIC_NONE);
    aeShipObjective[ship] = objective;
    return 0;
}

/* Function start: 0x422CD0 */
unsigned int reset_tactic(short ship, enum ShipTactic tactic)
{
    reset_maneuver(ship, MANEUVER_NONE);
    aeShipTactic[ship] = tactic;
    acShipTarget[ship] = -1;
    return 0;
}

/* Function start: 0x422D00 */
unsigned int alter_tactic(short ship, enum ShipTactic tactic)
{
    reset_maneuver(ship, MANEUVER_NONE);
    aeShipTactic[ship] = tactic;
    return 0;
}

/* Function start: 0x422D30 */
void reset_maneuver(short ship, short maneuver)
{
    aeShipManeuver[ship] = (enum ShipManeuver)maneuver;
    asShipCount[ship] = 0;
    acShipSequence[ship] = 0;
}

/* Function start: 0x422D60 */
void try2reset_maneuver(short obj, short maneuver)
{
    if (aeShipManeuver[obj] != maneuver) {
        reset_maneuver(obj, maneuver);
        steady_object(obj);
    }
}

/* Function start: 0x422D90 */
unsigned int set_special(short ship, enum SpecialManeuver special)
{
    enum SpecialManeuver *currentState;
    enum SpecialManeuver current;

    currentState = &aeSpecialManeuver[ship];
    current = *currentState;
    if (current < SPECIAL_MANEUVER_LOST_CONTROL || special > current)
        *currentState = special;

    if (*currentState == SPECIAL_MANEUVER_BLOWING_UP &&
        (short)alert_flag(ship, 1))
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
    short speed = aObjectTypeData[
        aeObjectType[obj]].cruiseVelocity;

    approach_speed(obj, (int)(short)(speed & 0xfffe) << 7);
    return 0;
}

/* Function start: 0x422E50 */
unsigned int approach_cruise_speed(short ship)
{
    approach_speed(ship,
        (int)aObjectTypeData[
            aeObjectType[ship]].cruiseVelocity << 8);
    return 0;
}

/* Function start: 0x422E80 */
unsigned int approach_full_speed(short ship)
{
    approach_speed(ship,
                      (int)asShipMaximumSpeed[ship] << 8);
    return 0;
}

/* Function start: 0x422EA0 */
unsigned int approach_ship_speed(short obj, short other)
{
    approach_speed(obj, anShipSpeed[other]);
    return 0;
}

/* Function start: 0x422EC0 */
unsigned int get_front_spot(short obj, unsigned short distance,
                            FixedVector *point)
{
    ScaleFixedVector(&aShipForwardVector[obj],
                     (int)distance << 8, point);
    AddFixedVectors(&aShipPosition[obj], point, point);
    return 0;
}

/* Function start: 0x422F10 */
unsigned int get_rear_spot(short obj, unsigned short distance,
                           FixedVector *point)
{
    ScaleFixedVector(&aShipForwardVector[obj],
                     -((int)distance << 8), point);
    AddFixedVectors(&aShipPosition[obj], point, point);
    return 0;
}

/* Function start: 0x422F60 */
unsigned int close_behind(short range)
{
    if (nTargetRange < range &&
        nTargetFacing < -0x32)
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
    nTargetRange = 0;
    for (other = 0; other < 10; other++) {
        if (aeObjectClass[other] < OBJECT_CLASS_SHIP ||
            aeSpecialManeuver[other] ==
                SPECIAL_MANEUVER_UNKNOWN_9)
            continue;
        nTargetShip = target;
        if (aeShipSide[obj] == aeShipSide[other])
            continue;
        distance = distance_from_point(other, &aShipPosition[obj]);
        target = nTargetShip;
        if (distance < range &&
            (target == -1 ||
             distance < nTargetRange)) {
            target = other;
            nTargetRange = distance;
        }
    }
    if (target != -1) {
        nTargetShip = target;
        get_facing_range_from_object(obj, nTargetShip);
        nTargetRange =
            distance_from_object(obj, nTargetShip);
        target = nTargetShip;
    }
    nTargetShip = target;
    return target;
}

/* Function start: 0x423070 */
int any_enemy(short obj, short range)
{
    short other;

    nTargetShip = -1;
    for (other = 0; other < 10; other++) {
        if (aeObjectClass[other] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuver[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            aeShipSide[obj] != aeShipSide[other]) {
            nTargetRange = distance_from_object(obj, other);
            if (nTargetRange < range) {
                nTargetShip = other;
                return 1;
            }
        }
    }
    return 0;
}

/* Function start: 0x4230F0 */
short nearest_enemy_range(short obj)
{
    short other;
    short range;

    nTargetShip = -1;
    range = 0x7fff;
    other = 0;
    do {
        if (aeObjectClass[other] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuver[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            aeShipSide[obj] != aeShipSide[other])
            range = MinShort(range, distance_from_object(obj, other));
        other++;
    } while (other < 10);
    return range;
}

/* Function start: 0x423210 */
void fire_when_ready(short obj, short aimed)
{
    (void)aimed;
    if (nYourWingman == obj &&
        aiPilotLevel[obj] != 11) {
        ship_vs_ship(obj, 0);
        if (nFacingToTarget > 80)
            return;
    }
    fire(obj, acShipTarget[obj]);
}

/* Function start: 0x423260 */
unsigned int ships_within_range(short obj, short other, short range)
{
    FixedVector delta;

    ComputeVectorDelta(&aShipPosition[obj],
                       &aShipPosition[other], &delta);
    return IsVectorWithinRange(&delta, range);
}

/* Function start: 0x4232B0 */
int attacker_in_range(short obj, short range)
{
    short other;

    for (other = 0; other < 10; other++) {
        if (aeObjectClass[other] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuver[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            aeShipSide[obj] != aeShipSide[other] &&
            acShipTarget[other] == obj) {
            nTargetRange = distance_from_object(other, obj);
            if ((unsigned short)nTargetRange <
                (unsigned short)range) {
                nTargetShip = other;
                return 1;
            }
        }
    }
    nTargetShip = -1;
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
        if (aeObjectClass[other] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuver[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            aeShipSide[obj] != aeShipSide[other]) {
            nTargetShip = target;
            if (acShipTarget[other] != obj)
                continue;
            range = (unsigned short)distance_from_object(other, obj);
            target = nTargetShip;
            if (target == -1 || range < nTargetRange) {
                target = other;
                nTargetRange = (short)range;
            }
        }
    }
    nTargetShip = target;
    return target != -1;
}

/* Function start: 0x423400 */
unsigned int target_within_range(short obj)
{
    short target = acShipTarget[obj];

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
    for (other = 0; other < 10; other++) {
        if (aeObjectClass[other] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuver[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            aeShipSide[obj] != aeShipSide[other]) {
            distance = distance_from_object(obj, other);
            if (distance < range) {
                acFormationMemberList[count] =
                    (signed char)other;
                asTargetListRange[count] = distance;
                count++;
            }
        }
    }
    acFormationMemberList[count] = -1;
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
        target = (short)acFormationMemberList[index];
        if (target == -1)
            break;
    } while (in_danger(target) != 0);
    if (target == -1 && index > 0) {
        index--;
        target = (short)acFormationMemberList[
            RandomBelowOrEqual(index)];
    }
    nTargetShip = target;
    return target != -1;
}

/* Function start: 0x423530 */
void inherit_leader_mission(short obj)
{
    short leader = asShipWingLeader[obj];

    if (leader != -1 &&
        aeObjectClass[obj] >= OBJECT_CLASS_SHIP) {
        aeShipMissionType[obj] =
            aeShipMissionType[leader];
        anShipMissionShip[obj] =
            anShipMissionShip[leader];
        asShipWingLeader[obj] = -1;
        aShipMissionSpot[obj] =
            aShipMissionSpot[leader];
    }
}

/* Function start: 0x4235B0 */
void inherit_leader(short obj)
{
    short leader = asShipWingLeader[obj];
    short other;

    if (leader == -1 || aeObjectClass[obj] < OBJECT_CLASS_SHIP)
        return;
    inherit_leader_mission(obj);
    asShipWingLeader[obj] = asShipWingLeader[leader];
    for (other = 0; other < 10; other++) {
        if (asShipWingLeader[other] == leader)
            asShipWingLeader[other] = obj;
    }
}

/* Function start: 0x423610 */
unsigned int dead_ship(short i)
{
    if (i != -1 && aMissionShips[i].state != 3)
        return 0;
    return 1;
}

/* Function start: 0x423640 */
int gone_ship(short missionShip)
{
    if (missionShip != -1 &&
        aMissionShips[missionShip].state != 3 &&
        aMissionShips[missionShip].state != 2)
        return 0;
    return 1;
}

/* Function start: 0x423670 */
short skill_rating(short obj)
{
    int rating = aiPilotLevel[obj];

    if (rating <= 4)
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

    roll = RandomBelowOrEqual(MinShort(8, difficulty));
    return skill_rating(obj) > roll;
}

/* Function start: 0x4236F0 */
short find_ships_sphere(short missionShip)
{
    MissionNavPoint *navPoint;
    short fallback;
    short navIndex;
    short shipIndex;

    navIndex = 0;
    navPoint = aMissionNavPoints;
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
        if (aMissionNavPoints[navIndex].type > 1)
            do {
                navIndex--;
            } while (aMissionNavPoints[navIndex].type > 1);
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
        *point = aShipPosition[obj];
        return 1;
    }
    if (dead_ship(missionShip) != 0)
        return 0;
    navPoint = find_ships_sphere(missionShip);
    if (navPoint != -1) {
        AddFixedVectors(&aMissionNavPoints[navPoint].position,
                        &aMissionShips[missionShip].position,
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

    if (aeShipSide[obj] == SIDE_KILRATHI) {
        *point = aMissionNavPoints[
            anShipMissionShip[obj]].position;
        return 0;
    }
    pathIndex = (short)abShipNavPointIndex[obj];
    while (++pathIndex < 16) {
        objective = (short)abFlightPath[pathIndex];
        type = aMissionObjectives[objective].type;
        switch (type) {
        case 0:
            *point = aMissionObjectives[objective].position;
            abShipNavPointIndex[obj] = (signed char)pathIndex;
            return 0;
        case 1:
            missionShip = (short)aMissionObjectives[objective].index;
            objective = find_ship_index(missionShip);
            if (objective == -1)
                *point = aMissionNavPoints[missionShip].position;
            else
                *point = aShipPosition[objective];
            abShipNavPointIndex[obj] = (signed char)pathIndex;
            return 0;
        }
    }
    return 0;
}

/* Function start: 0x423930 */
unsigned int get_first_follow_point(short obj, FixedVector *point)
{
    if (aeShipSide[obj] == SIDE_IMPERIAL)
        abShipNavPointIndex[obj] =
            cCurrentNavPointIndex - 1;
    get_follow_point(obj, point);
    return 0;
}

/* Function start: 0x423970 */
int hostile_sphere(short obj, short navPoint)
{
    short index;
    short missionShip;
    short *ships = aMissionNavPoints[navPoint].missionShips;

    for (index = 0; index < 10; index++) {
        missionShip = ships[index];
        if (missionShip != -1 &&
            aeShipSide[obj] !=
                aMissionShips[missionShip].side)
            return 1;
    }
    return 0;
}

/* Function start: 0x4239D0 */
int abandoned(short obj, short other)
{
    short navPoint;

    if ((abShipTurn[obj] & 7) == 0 &&
        aeShipSide[obj] != SIDE_KILRATHI &&
        RandomBelowOrEqual(8) == 0) {
        navPoint = FindNearestNavPoint(obj);
        if (nCurrentNavPoint != navPoint &&
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
    if (aeShipObjective[obj] != objective) {
        reset_objective(obj, objective);
        if (acShipRating[obj] > 8 &&
            ace_status(
                (short)((short)aiPilotLevel[obj] - 14), 8) == 0)
            ace_greeting(obj);
    }
    acShipTarget[obj] = target;
}

/* Function start: 0x423AC0 */
int target_valid(short obj)
{
    return unactive(acShipTarget[obj]) == 0 &&
           aeShipSide[
               acShipTarget[obj]] !=
               aeShipSide[obj];
}

/* Function start: 0x423B00 */
unsigned int triumph(short obj)
{
    unsigned int result = 0;
    short objective;

    switch (aeShipMissionType[obj]) {
    case MISSION_TYPE_PATROL:
        objective = 0;
        while (abFlightPath[objective] != -1 &&
               visited(
                   (short)abFlightPath[objective]) != 0)
            objective++;
        return 1;
    case MISSION_TYPE_ESCORT:
    case MISSION_TYPE_DEFEND:
    case MISSION_TYPE_WINGMAN:
        return dead_ship(anShipMissionShip[obj]) == 0;
    case MISSION_TYPE_STRIKE:
        result = dead_ship(anShipMissionShip[obj]);
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
        &aObjectTypeData[aeObjectType[obj]];

    if (aeObjectClass[obj] < OBJECT_CLASS_SHIP)
        return 100;
    return (short)((acShipDamage[obj] * -26) /
                       typeData->damageCapacity +
                   (aasShipArmor[obj][1] * 27) /
                       typeData->armorRear +
                   (aasShipArmor[obj][0] * 23) /
                       typeData->armorFront +
                   (aasShipArmor[obj][2] * 12) /
                       typeData->armorLeft +
                   (aasShipArmor[obj][3] * 12) /
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
            *resources->resource = FetchDiskPacketRetrying(
                logicalFile, resources->section, flags);
            if (*resources->resource == 0)
                return 0;
        }
        resources++;
    }
    return 1;
}

/* Function start: 0x423D50 */
int FreeShapeSet(PacketResourceDescriptor *resources,
                 unsigned short releaseFlags)
{
    while (resources->resource != 0) {
        if (*resources->resource != 0)
            FreePacketAndClear(resources->resource, releaseFlags);
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
                pDiskFileRecords[
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
        memcpy(aPaletteFadeEntries[index], black,
               sizeof(black));
        SetPaletteEntry((short)(index + 185),
                        aPaletteFadeEntries[index]);
        index++;
    } while (index < 6);
    memcpy(asDamageFlashColour, black, sizeof(black));
    asDamageFlashColour[2] = 32;
    SetPaletteEntry((short)cPrimaryViewBufferColour,
                    asDamageFlashColour);
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

    if (bCockpitlessView == 0 && mode == cScreenViewportMode) {
        if (stSpaceBuffer.pixels != 0)
            ClearViewport(&stSpaceBuffer, cPrimaryViewBufferColour);
        else
            initialize_view_buffer();
        return 0;
    }

    GetScreenUpdateFlag();
    savedScreen = stScreen;
    stScreen.left = 0;
    stScreen.top = 0;
    stScreen.right = 319;
    stScreen.bottom = 199;
    ClearViewport(&stModalSourceViewport, cBlackColour);

    cCockpitLogicalFile =
        (unsigned char)(cCockpitView + 17);
    cScreenViewportMode = mode;
    switch (mode) {
    case 0:
        if (apCockpitShapes[0] != 0) {
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreen, 0, 0,
                                  apCockpitShapes[0], 0);
        } else {
            backdrop = FetchDiskPacketRetrying(
                (short)cCockpitLogicalFile, 0, 0);
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreen, 0, 0, backdrop, 0);
            ReleasePacketHandle(backdrop);
        }
        ResetCockpitPaletteEntries();
        if (bCockpitlessView == 0)
            explosion_draw();
        reset_cockpit();
        InvalidateVduMode(0);
        InvalidateVduMode(1);
        update_VDUs();
        clear_head_up_display();
        if (bCockpitlessView == 0)
            ResetPilotHandAnimation();
        set_up_screen_viewport(0);
        break;
    case 1:
        if (apCockpitShapes[1] != 0) {
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreen, 0, 0,
                                  apCockpitShapes[1], 0);
        } else {
            backdrop = FetchDiskPacketRetrying(
                (short)cCockpitLogicalFile, 1, 0);
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreen, 0, 0, backdrop, 0);
            ReleasePacketHandle(backdrop);
        }
        set_up_screen_viewport(1);
        break;
    case 2:
        if (apCockpitShapes[2] != 0) {
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreen, 0, 0,
                                  apCockpitShapes[2], 0);
        } else {
            backdrop = FetchDiskPacketRetrying(
                (short)cCockpitLogicalFile, 2, 0);
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreen, 0, 0, backdrop, 0);
            ReleasePacketHandle(backdrop);
        }
        set_up_screen_viewport(2);
        break;
    case 3:
        if (apCockpitShapes[3] != 0) {
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreen, 0, 0,
                                  apCockpitShapes[3], 0);
        } else {
            backdrop = FetchDiskPacketRetrying(
                (short)cCockpitLogicalFile, 3, 0);
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreen, 0, 0, backdrop, 0);
            ReleasePacketHandle(backdrop);
        }
        set_up_screen_viewport(3);
        break;
    case 4:
        if (bIntroSceneResourcesActive == 1) {
            if (pCinematicViewBackdrop != 0) {
                DrawSpriteDefault(&stScreen, 0, 0,
                                  pCinematicViewBackdrop, 0);
                if (bCockpitlessView < 1) {
                    DIBslam();
                    DIBslamReal();
                }
            } else {
                backdrop = FetchDiskPacketRetrying(8, 6, 0);
                if (backdrop != 0) {
                    DrawSpriteDefault(&stScreen, 0, 0, backdrop, 0);
                    if (bCockpitlessView < 1) {
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
        if (pRearViewBackdrop != 0) {
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreen, 0, 0,
                                  pRearViewBackdrop, 0);
        } else {
            backdrop = FetchDiskPacketRetrying(8, 7, 0);
            if (bCockpitlessView == 0)
                DrawSpriteDefault(&stScreen, 0, 0, backdrop, 0);
            if (backdrop != 0)
                ReleasePacketHandle(backdrop);
        }
        set_up_screen_viewport(0);
        break;
    }

    bViewBufferEnabled = 1;
    SetViewportRect(&stSpaceBuffer, 0, 0,
                    (unsigned short)(nScreenWidth - 1),
                    (unsigned short)(nScreenHeight - 1));
    result = initialize_view_buffer();
    stScreen = savedScreen;
    return result;
}

/* Function start: 0x4242D0 */
unsigned int InitializeConstellationObject(
    const ConstellationObjectDefinition *definition, short object)
{
    FixedVector position;

    aeObjectClass[object] = OBJECT_CLASS_PLANET;
    init_ijk(63);
    alter_yaw((short)-definition->yaw, 63);
    alter_pitch((short)-definition->pitch, 63);
    alter_roll(definition->roll, 63);
    ScaleFixedVector(&aShipForwardVector[63],
                     0x753000, &position);
    aShipPosition[object] = position;
    asObjectScreenScale[object] = 0xff;
    asObjectScreenAngle[object] = 0;
    asObjectViewFrame[object] = 0;
    aeObjectType[object] = OBJECT_TYPE_HORNET;
    apObjectShape[object] =
        FetchDiskPacketRetrying(
            12, (short)(definition->shapePacket + 1), 0);
    return 0;
}

/* Function start: 0x4243B0 */
unsigned int FreeConstellationObject(short object)
{
    FreePacketAndClear(&apObjectShape[object], 0);
    remove_object(object);
    return 0;
}

/* Function start: 0x4243E0 */
unsigned int init_constellation(short scene)
{
    short slot;
    short object;
    int definitionBase;

    if (pConstellationShape != 0)
        return 0;

    scene--;
    pConstellationShape =
        FetchDiskPacketRetrying(12, 0, 0);
    if (nTrainSimActive != 0 || scene < 0)
        return;

    definitionBase = (int)scene * 4;
    slot = 0;
    do {
        if (pConstellationDefinitions[
                definitionBase + slot].shapePacket != -1) {
            object = find_vacant_3d_object();
            if (object != -1) {
                InitializeConstellationObject(
                    &pConstellationDefinitions[
                        definitionBase + slot],
                    object);
            }
            asConstellationObjectIndices[slot] = object;
        } else {
            asConstellationObjectIndices[slot] = -1;
        }
        slot++;
    } while (slot < 4);
    /* The successful path returns the last expression left in EAX. */
}

/* Function start: 0x424490 */
unsigned int free_constellation(void)
{
    short object;
    short slot;

    slot = 0;
    FreePacketAndClear(&pConstellationShape, 0);
    do {
        object = asConstellationObjectIndices[slot];
        if (object != -1) {
            FreeConstellationObject(object);
            asConstellationObjectIndices[slot] = -1;
        }
        slot++;
    } while (slot < 4);
    return 0;
}

/* Function start: 0x4244E0 */
void init_vdus(void)
{
    stRightVdu = stScreen;
    stLeftVdu = stScreen;

    InitializeTextContextFromFont(
        &stLeftVduTextContext, 2,
        cPrimaryTextColour,
        (signed char)cBlackColour);
    stLeftVduTextContext.viewport = &stLeftVdu;
    stLeftVduTextContext.text = szDefaultTextBuffer;
    set_mode(0, 1);
    anVduModeCache[0] = 0;

    InitializeTextContextFromFont(
        &stRightVduTextContext, 2,
        cPrimaryTextColour,
        (signed char)cBlackColour);
    stRightVduTextContext.viewport = &stRightVdu;
    stRightVduTextContext.text = szDefaultTextBuffer;
    set_mode(1, nTrainSimActive == 0 ? 5 : 3);
    anVduModeCache[1] = 0;
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

    if (bCockpitResourcesActive == 1) {
        if (mode == cCockpitView)
            return 0;
        free_cockpit();
    }

    bCockpitResourcesActive = 1;
    cCockpitView = mode;
    cCockpitLogicalFile = (unsigned char)(mode + 17);
    clear_cockpit_damage();
    ClearHudGunReadouts();
    reset_cockpit();
    GetScreenUpdateFlag();

    LoadShapeSet(aCockpitPrimaryResources, 4,
                 (short)cCockpitLogicalFile);
    pScreenViewportPacket =
        LoadPacketAllocated(
            (short)cCockpitLogicalFile, 6);
    stCockpitBar = stScreen;
    init_vdus();

    InitializeTextContextFromFont(
        &stCockpitReadoutTextContext, 2,
        cPrimaryTextColour,
        (signed char)cBlackColour);
    stCockpitReadoutTextContext.viewport = &stScreen;
    SetTextContext(&stCockpitReadoutTextContext);

    layout = &stCockpitLayout;
    origin = &layout->readoutOrigins[0][(int)cCockpitView];
    SetTextCursor((unsigned short)origin->x, (unsigned short)origin->y);
    InitializeCockpitReadout(4, &stCockpitReadoutTextContext);
    origin = &layout->readoutOrigins[1][(int)cCockpitView];
    SetTextCursor((unsigned short)origin->x, (unsigned short)origin->y);
    InitializeCockpitReadout(5, &stCockpitReadoutTextContext);
    origin = &layout->readoutOrigins[2][(int)cCockpitView];
    SetTextCursor((unsigned short)origin->x, (unsigned short)origin->y);
    InitializeCockpitReadout(2, &stCockpitReadoutTextContext);
    origin = &layout->readoutOrigins[3][(int)cCockpitView];
    SetTextCursor((unsigned short)origin->x, (unsigned short)origin->y);
    InitializeCockpitReadout(3, &stCockpitReadoutTextContext);

    bounds = &layout->leftVduBounds[(int)cCockpitView];
    stLeftVdu.left = bounds->left;
    stLeftVdu.top = bounds->top;
    stLeftVdu.right = bounds->right;
    stLeftVdu.bottom = bounds->bottom;
    bounds = &layout->rightVduBounds[(int)cCockpitView];
    stRightVdu.left = bounds->left;
    stRightVdu.top = bounds->top;
    stRightVdu.right = bounds->right;
    stRightVdu.bottom = bounds->bottom;

    if (pPilotHandShape != 0) {
        stPilotHand = stScreen;
        bounds = &layout->pilotHandBounds[(int)cCockpitView];
        stPilotHand.left = bounds->left;
        stPilotHand.top = bounds->top;
        stPilotHand.right = bounds->right;
        stPilotHand.bottom = bounds->bottom;
        SetViewportRect(&stPilotHandComposite, 0, 0,
                        (unsigned short)(bounds->right - bounds->left),
                        (unsigned short)(bounds->bottom - bounds->top));
        stPilotHandBackdrop = stPilotHandComposite;
        AllocateViewport(&stPilotHandComposite, cBlackColour, 0);
        AllocateViewport(&stPilotHandBackdrop, cBlackColour, 0);
    }

    LoadShapeSet(aCockpitSecondaryResources, 0,
                 (short)cCockpitLogicalFile);
    backgroundSize = MeasureShapeFrameStorage(
        pTargetLockShape, 2);
    pScannerMarkerBackground =
        AllocateTaggedMemory(backgroundSize, 0);

    pReleaseWeaponDisplayBackground = 0;
    maximumSize = 0;
    for (weapon = 0;
         weapon < (signed char)aShipWeapons[0][0];
         weapon++) {
        weaponSlot = &((ShipWeaponSlot *)
            &aShipWeapons[0][1])[weapon];
        if (aObjectTypeData[weaponSlot->type].objectClass ==
                OBJECT_CLASS_MISSILE) {
            backgroundSize = MeasureShapeFrameStorage(
                pCockpitWeaponShape,
                (short)(weaponSlot->type * 2 - 0x2f));
            maximumSize = MaxShort(maximumSize, (short)backgroundSize);
        }
    }
    if (maximumSize != 0)
        pReleaseWeaponDisplayBackground =
            AllocateTaggedMemory(maximumSize, 0);

    pCockpitExplosionBackground = 0;
    if (pCockpitExplosionShape != 0) {
        maximumSize = 0;
        for (frame = 0; frame < 8; frame++) {
            backgroundSize = MeasureShapeFrameStorage(
                pCockpitExplosionShape, frame);
            maximumSize = MaxShort(maximumSize, (short)backgroundSize);
        }
        if (maximumSize != 0)
            pCockpitExplosionBackground =
                AllocateTaggedMemory(maximumSize, 0);
    }

    pDamageDisplayBackground = 0;
    maximumSize = 0;
    for (frame = 0; frame < 9; frame++) {
        backgroundSize = MeasureShapeFrameStorage(
            pCockpitWeaponShape, frame);
        maximumSize = MaxShort(maximumSize, (short)backgroundSize);
    }
    if (maximumSize != 0)
        pDamageDisplayBackground =
            AllocateTaggedMemory(maximumSize, 0);

    ResetScannerContacts();
    result = init_personalities();
    nCockpitExplosionFrame = 8;
    bRadioSilence = 0;
    if (nMemoryConfiguration == 2)
        bCommVideoEnabled = 1;
    else if (nSceneResourceBudget > 0x59d8)
        bCommVideoEnabled = 1;
    else
        bCommVideoEnabled = 0;
    return result;
}

/* Function start: 0x4249A0 */
unsigned int free_cockpit(void)
{
    if (bCockpitResourcesActive == 0)
        return 0;
    bCockpitResourcesActive = 0;
    GetScreenUpdateFlag();
    if (pScreenViewportPacket != 0) {
        ReleasePacketHandle(pScreenViewportPacket);
        pScreenViewportPacket = 0;
    }
    ReleaseTextFont(2);
    if (pPilotHandShape != 0) {
        free_viewport(&stPilotHandComposite);
        free_viewport(&stPilotHandBackdrop);
    }
    cCockpitLogicalFile =
        (signed char)(cCockpitView + 17);
    FreeShapeSet(aCockpitPrimaryResources, 4);
    FreeShapeSet(aCockpitSecondaryResources, 0);
    FreePacketAndClear(&pCockpitPilotShape, 0);
    FreePacketAndClear(&pReleaseWeaponDisplayBackground, 0);
    FreePacketAndClear(&pCockpitExplosionBackground, 0);
    FreePacketAndClear(&pDamageDisplayBackground, 0);
    FreeCommDisplayResources();
    ReleasePacketHandle(pScannerMarkerBackground);
}

/* Function start: 0x424A80 */
unsigned int init_3Space_objects(short scene)
{
    short slot;

    if (b3SpaceObjectsActive == 1)
        return 0;
    b3SpaceObjectsActive = 1;
    cScreenViewportMode = -1;
    remove_all_3d_objects();
    nExternalViewShip = -1;
    nRenderedSpaceFrame = 0;
    bScriptedView = 0;
    nSpaceFrame = 0;
    bMissileCameraEnabled = 0;
    nClosestVisibleObject = -1;
    nPlayerCollisionObject = -1;
    slot = 0;
    do {
        aObjectResourceSlots[slot].type = -1;
        slot++;
    } while (slot <= 3);
    init_constellation(scene);
    return load_common_3Space_objects();
}

/* Function start: 0x424B00 */
unsigned int load_common_3Space_objects(void)
{
    LoadShapeSet(aCommon3SpaceResources, 0, -1);
    aObjectTypeData[OBJECT_TYPE_DEBRIS_O_RING].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_GLASS].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_SHIP_TUBING].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_TURRET].shapeSet =
        aObjectTypeData[OBJECT_TYPE_LASER_CANNON].shapeSet;
    aObjectTypeData[OBJECT_TYPE_TURRET].animation =
        aObjectTypeData[OBJECT_TYPE_LASER_CANNON].animation;

    load_ship(OBJECT_TYPE_HEAT_SEEKING_MISSILE, 3);
    aObjectTypeData[OBJECT_TYPE_DUMB_FIRE_MISSILE].shapeSet =
        aObjectTypeData[OBJECT_TYPE_HEAT_SEEKING_MISSILE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE].shapeSet =
        aObjectTypeData[OBJECT_TYPE_HEAT_SEEKING_MISSILE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_FF_MISSILE].shapeSet =
        aObjectTypeData[OBJECT_TYPE_HEAT_SEEKING_MISSILE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_DUMB_FIRE_MISSILE].animation =
        aObjectTypeData[OBJECT_TYPE_HEAT_SEEKING_MISSILE].animation;
    aObjectTypeData[OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE].animation =
        aObjectTypeData[OBJECT_TYPE_HEAT_SEEKING_MISSILE].animation;
    aObjectTypeData[OBJECT_TYPE_FF_MISSILE].animation =
        aObjectTypeData[OBJECT_TYPE_HEAT_SEEKING_MISSILE].animation;
    return 0;
}

/* Function start: 0x424B80 */
unsigned int remove_all_3d_objects(void)
{
    short i = 0;

    do {
        remove_object(i);
        i = i + 1;
    } while (i < 0x40);
    return 0;
}

/* Function start: 0x424BA0 */
unsigned int free_3Space(void)
{
    if (b3SpaceObjectsActive == 0)
        return 0;
    b3SpaceObjectsActive = 0;
    GetScreenUpdateFlag();
    bViewBufferEnabled = 0;
    free_constellation();
    remove_all_hazards();
    remove_all_3d_objects();
    free_3Space_objects();
    return 0;
}

/* Function start: 0x424BE0 */
unsigned int free_3Space_objects(void)
{
    FreeShapeSet(aCommon3SpaceResources, 0);
    FreeShapeSet(aMissionResourceDescriptors, 0);
    free_ship(3);
    aObjectTypeData[OBJECT_TYPE_DUMB_FIRE_MISSILE].shapeSet = 0;
    aObjectTypeData[OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE].shapeSet = 0;
    aObjectTypeData[OBJECT_TYPE_FF_MISSILE].shapeSet = 0;
    aObjectTypeData[OBJECT_TYPE_DUMB_FIRE_MISSILE].animation = 0;
    aObjectTypeData[OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE].animation = 0;
    aObjectTypeData[OBJECT_TYPE_FF_MISSILE].animation = 0;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_O_RING].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_GLASS].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_SHIP_TUBING].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_WING].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet;
    return 0;
}

/* Function start: 0x424C60 */
unsigned int init_inflight_music(void)
{
    nCombatMusicActive = 0;
    nInFlightMusicActive = 1;
    nInitialFlightMusicPending = 1;
    return 0;
}

/* Function start: 0x424C80 */
unsigned int free_inflight_music(void)
{
    int slot;

    StopMusicUnlessSuppressed();
    nInFlightMusicActive = 0;
    if (nInFlightMusicSlotA != -1) {
        slot = nInFlightMusicSlotA;
        nInFlightMusicSlotA = -1;
        *(int *)(abSoundPlaybackSlots + slot * 6) = 0;
    }
    if (nInFlightMusicSlotB != -1) {
        slot = nInFlightMusicSlotB;
        nInFlightMusicSlotB = -1;
        *(int *)(abSoundPlaybackSlots + slot * 6) = 0;
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
unsigned short LoadSceneAnimationResources(short scene, short variant)
{
    unsigned char *packet;
    short logicalFile;

    logicalFile = asSceneAnimationLogicalFiles[scene];
    pSceneAnimationPrimaryShape =
        FetchDiskPacketRetrying(logicalFile, 0, 0);
    pSceneAnimationDefinitions =
        FetchDiskPacketRetrying(
            logicalFile, (short)(variant + 1), 0);
    pSceneAnimationSecondaryShape =
        FetchDiskPacketRetrying(
            logicalFile, (short)(variant + 3), 0);
    packet = FetchDiskPacketRetrying(
        logicalFile, (short)(variant + 5), 0);
    pSceneAnimationPacket = packet;
    pSceneAnimationSceneData =
        packet + *(unsigned int *)(packet + 0);
    pSceneAnimationTextData =
        packet + *(unsigned int *)(packet + 4);
    pSceneAnimationObjects =
        (SceneAnimationObject *)(pSceneAnimationDefinitions + 2);
    return 0;
}

/* Function start: 0x424DA0 */
void ReleaseSceneAnimationResources(void)
{
    ReleasePacketHandle(pSceneAnimationPrimaryShape);
    ReleasePacketHandle(pSceneAnimationPacket);
    ReleasePacketHandle(pSceneAnimationSecondaryShape);
    ReleasePacketHandle(pSceneAnimationDefinitions);
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
                if (object->layer != 2 && nFrameSkipCounter < 1)
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
                if (object->layer != 2 && nFrameSkipCounter < 1)
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
            objectCount = *(short *)pSceneAnimationDefinitions;
            objectIndex = (short)(objectCount * (short)*cursor++);
            objectIndex = (short)(objectIndex + (short)*cursor++);
            source = &pSceneAnimationObjects[objectIndex];
            object->x = source->x;
            object->y = source->y;
            object->rotation = source->rotation;
            object->scale = source->scale;
            object->frame = source->frame;
            break;

        case 'W':
            nSceneAnimationWaitFrames = *(short *)cursor;
            cursor += 2;
            bSceneAnimationWaitCommand = 1;
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

    nSceneAnimationWaitFrames = -1;
    complete = 0;
    bSceneAnimationWaitCommand = 0;
    AddPCName(text);
    ClearViewport(&stConversationTextViewport,
                  cBlackColour);
    FormatTextBufferFromStart(szSceneAnimationTextFormat,
                              0, 160,
                              nConversationTextColour,
                              szTextScratchBuffer);

    objectCount = *(short *)pSceneAnimationDefinitions;
    objects = pSceneAnimationObjects +
        (short)(objectCount * animation);
    remaining = objectCount;
    object = objects;
    while (remaining > 0) {
        if (object->layer == 0)
            object->shape = pSceneAnimationPrimaryShape;
        else
            object->shape = pSceneAnimationSecondaryShape;
        object->scriptStart =
            (signed char *)pSceneAnimationDefinitions +
            object->scriptOffset;
        object->scriptCursor = object->scriptStart;
        object++;
        remaining--;
    }

    nFrameSkipCounter = 1;
    bEscapePressed = 0;
    ClearInputKeyState();
    DIBslam();
    DIBslamReal();
    for (;;) {
        do {
            nFrameSkipCounter--;
            object = objects;
            remaining = objectCount;
            while (remaining > 0) {
                complete |= (unsigned short)
                    UpdateSceneAnimationObject(object, &stSceneBuffer);
                object++;
                remaining--;
            }
            if (nSceneAnimationWaitFrames != -1) {
                if (nSceneAnimationWaitFrames == 0)
                    complete++;
                else
                    nSceneAnimationWaitFrames--;
            }
            if (complete == 0) {
                RefreshMemoryStatusOverlay();
                DIBslam();
                DIBslamReal();
            }
            if (nFrameSkipCounter == 0) {
                nFrameSkipCounter = nFrameSkip;
                if (bSlowSceneAnimation != 0)
                    nFrameSkipCounter++;
            }
            if ((complete == 0 && CheckEscaped() != 0) ||
                bEscapePressed != 0) {
                if (nSceneAnimationWaitFrames == -1) {
                    while (complete == 0 &&
                           bSceneAnimationWaitCommand == 0) {
                        object = objects;
                        remaining = objectCount;
                        while (remaining > 0) {
                            nFrameSkipCounter = 2;
                            complete |= (unsigned short)
                                UpdateSceneAnimationObject(
                                    object, &stSceneBuffer);
                            object++;
                            remaining--;
                        }
                    }
                }
                nSceneAnimationWaitFrames = 0;
            }
        } while (complete == 0);

        if (nSceneAnimationWaitFrames == -1) {
            SetFrameTimerPeriodDirect((short)(duration / 2));
            do {
                if (IsFrameTickElapsed() != 0 || CheckEscaped() != 0)
                    break;
            } while (bEscapePressed == 0);
        }
        if (complete != 0)
            return;
    }
}
