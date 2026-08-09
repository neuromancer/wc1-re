/*
 *  Disk data files and packet fetching with retry.
 *
 *  Address range 0x41d250-0x41efff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: OpenDiskDataFile/FetchDiskPacketRetrying/PromptInsertNumberedDisk.
 */
#include "wc1.h"

/* Function start: 0x41D2E0 */
void *FetchDiskPacketRetrying(short logicalFile, short section,
                              unsigned short flags)
{
    const char *filename;
    void *packet = 0;
    short retries = 5;

    if (DAT_005a7cf0 == 0 || logicalFile < 0) {
        DAT_00465460 = 3;
        return 0;
    }
    filename = (const char *)(DAT_005a7cf0 + logicalFile * 16);
    do {
        FreePacketAndClear((int *)&packet);
        packet = PacketLoad(filename, section, 0, flags);
        retries--;
    } while (packet == 0 && retries > 0 && DAT_00465460 != 8);

    if (packet == 0 && DAT_00465460 == 4 && DAT_005a7510.pixels != 0) {
        free_viewport(&DAT_005a7510);
        do {
            FreePacketAndClear((int *)&packet);
            packet = PacketLoad(filename, section, 0, flags);
            retries--;
        } while (packet == 0 && retries > 0 && DAT_00465460 != 8);
        AllocateViewport(&DAT_005a7510, (short)DAT_004699d8, 0x20);
    }
    if (packet == 0 && DAT_00465460 == 4 && DAT_005a76b0.pixels != 0) {
        free_viewport(&DAT_005a76b0);
        do {
            FreePacketAndClear((int *)&packet);
            packet = PacketLoad(filename, section, 0, flags);
            retries--;
        } while (packet == 0 && retries > 0 && DAT_00465460 != 8);
        AllocateViewport(&DAT_005a76b0, (short)DAT_0046999c, 0);
    }
    return packet;
}

/* Function start: 0x41D5F0 */
unsigned int DrawTextAt(TextContext *context, short x, short y,
                        char *text, unsigned char alignment)
{
    char *savedText = context->text;
    unsigned char savedAlignment = context->alignment;

    SetTextContext(context);
    SetTextCursor((unsigned short)x, (unsigned short)y);
    context->text = text;
    context->alignment = alignment;
    DrawTextString(text);
    context->text = savedText;
    context->alignment = savedAlignment;
    if (context->viewport->pixels == DAT_005a6ba0.pixels)
        DIBslam();
    return 0;
}

/* Function start: 0x41DA00 */
unsigned int GetZeroUnused(void)
{
    return 0;
}

/* Function start: 0x41DEB0 */
void WaitForStreamIdle(void)
{
    unsigned int saved = DAT_0046505c;

    DAT_0046505c = 1;
    while (UiRoutine03() == 0)
        ;
    DAT_0046505c = saved;
}

/* Function start: 0x41DF40 */
short FindActiveShipSlot(void)
{
    short i = 0x22;

    do {
        if (g_aeObjectClass_0059d100[i] == OBJECT_CLASS_DUST)
            return i;
        i = i + 1;
    } while (i < 0x2a);
    return -1;
}

/* Function start: 0x41DFA0 */
short new_object(enum ObjectType type, signed char owner)
{
    short obj = get_ship_slot();

    if (obj != -1) {
        initialize_object(obj, type, owner);
        zero_vector(&g_aShipPosition_0059c490[obj]);
        zero_vector(&g_aShipVelocity_0059c010[obj]);
        g_aeShipSide_0059d650[obj] = SIDE_NEUTRAL;
    }
    return obj;
}

/* Function start: 0x41E120 */
void initialize_object(short obj, enum ObjectType type, short owner)
{
    ObjectTypeData *typeData;
    ObjectTypeData *resourceData;
    enum ObjectType resourceType;

    if (obj < 0 || obj >= WC1_SPACE_OBJECT_COUNT)
        return;
    if (type == OBJECT_TYPE_SPACE_DUST) {
        g_aeObjectType_0059b560[obj] = type;
        g_aeObjectClass_0059d100[obj] = OBJECT_CLASS_DUST;
        g_asObjectScreenX_0059d9b0[obj] = (short)0x8001;
        g_asObjectCounter_0059c330[obj] = 0;
        g_asObjectViewFrame_0059d230[obj] = 0;
        g_asObjectFlip_0059c870[obj] = 0;
        return;
    }
    if ((int)type < 0 || type >= OBJECT_TYPE_COUNT)
        return;

    typeData = &g_aObjectTypeData_0046645c[type];
    resourceType = type;
    if (typeData->shapeSet == 0) {
        switch (resourceType) {
        case OBJECT_TYPE_ASTEROID2:
            resourceType = OBJECT_TYPE_ASTEROID1;
            break;
        case OBJECT_TYPE_ASTEROID4:
            resourceType = OBJECT_TYPE_ASTEROID3;
            break;
        case OBJECT_TYPE_ASTEROID6:
            resourceType = OBJECT_TYPE_ASTEROID5;
            break;
        case OBJECT_TYPE_DEBRIS_METAL_SHEET:
            resourceType = OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK;
            break;
        case OBJECT_TYPE_DEBRIS_WING:
            resourceType = OBJECT_TYPE_DEBRIS_PIPE;
            break;
        case OBJECT_TYPE_EXPLOSION1:
        case OBJECT_TYPE_EXPLOSION2:
            resourceType = OBJECT_TYPE_EXPLOSION0;
            break;
        }
    }
    resourceData = &g_aObjectTypeData_0046645c[resourceType];
    type = resourceType;
    typeData = resourceData;

    g_aeObjectType_0059b560[obj] = type;
    g_aeObjectClass_0059d100[obj] = typeData->objectClass;
    g_apObjectShape_0059d2f0[obj] = resourceData->shapeSet;
    init_ijk(obj);
    g_asObjectCollisionRadius_0059d710[obj] = typeData->collisionRadius;
    g_asObjectScale_0059de40[obj] = typeData->scale;
    g_asObjectScreenScale_0059c950[obj] = typeData->scale;
    g_asObjectViewFrame_0059d230[obj] = 0;
    g_asObjectFlip_0059c870[obj] = 0;
    g_asObjectScreenAngle_0059cd90[obj] = 0;
    g_asObjectAnimationDelay_0059b660[obj] = 1;
    g_asObjectAnimationIndex_0059da30[obj] = 0;
    g_acObjectOwner_0059ce20[obj] = (signed char)owner;
    g_asObjectCounter_0059c330[obj] = -1;
    g_acShipTarget_0059ce60[obj] = -1;
    g_aeSpecialManeuver_0059c3c0[obj] = SPECIAL_MANEUVER_NONE;

    if (typeData->objectClass >= OBJECT_CLASS_SHIP) {
        g_asShipMaximumSpeed_0059c440[obj] = typeData->maximumVelocity;
        g_anShipSpeed_0059b320[obj] = 0;
        if (obj < 12) {
            g_aasShipShield_0059d5b0[obj][0] = typeData->shieldFore;
            g_aasShipMaximumShield_0059d6e0[obj][0] = typeData->shieldFore;
            g_aasShipShield_0059d5b0[obj][1] = typeData->shieldAft;
            g_aasShipMaximumShield_0059d6e0[obj][1] = typeData->shieldAft;
        }
        if (obj < 16)
            g_asShipWeaponEnergy_0059d470[obj] = 100;
        g_aeShipManeuver_0059dcb0[obj] = MANEUVER_NONE;
        g_aeShipTactic_0059d5e0[obj] = TACTIC_SIT_STILL;
        if (obj < 16)
            memcpy(g_aShipWeapons_0059cab0[obj],
                   typeData->weaponLoadout,
                   sizeof(g_aShipWeapons_0059cab0[obj]));
    }
}

/* Function start: 0x41E400 */
void match_rotation_goal(short *rotation, short *goal,
                         short totalError, short rate)
{
    short step;
    short value;

    if (totalError != 0) {
        if (*goal > 180)
            *goal = *goal - 360;
        if (*goal < -180)
            *goal = *goal + 360;
        step = MaxShort(1, (short)(AbsInt(*rotation - *goal) *
                                  rate / totalError));
        if (*goal != *rotation || step < AbsInt(*rotation)) {
            if (*goal < 1) {
                value = MaxShort(*goal, (short)-step);
                value = MaxShort((short)(value - *rotation),
                                 (short)-step);
                value = MinShort(value, step);
                *rotation = value + *rotation;
            } else {
                value = MinShort(*goal, step);
                value = MinShort((short)(value - *rotation), step);
                value = MaxShort(value, (short)-step);
                *rotation = value + *rotation;
            }
        }
    }
    if (*goal != 0) {
        if (*goal > 0)
            *goal = MaxShort((short)(*goal - *rotation), 0);
        else
            *goal = MinShort((short)(*goal - *rotation), 0);
    }
}

/* Function start: 0x41E520 */
void rotate_object_to_goal(short obj)
{
    ObjectTypeData *typeData;
    short totalError;

    typeData = &g_aObjectTypeData_0046645c[g_aeObjectType_0059b560[obj]];
    totalError = (short)(AbsInt(g_anObjectPitchRotation_0059b2a0[obj] -
                               g_anPitchGoal_0059d7a0[obj]) +
                         AbsInt(g_anObjectYawRotation_0059ce80[obj] -
                               g_anYawGoal_0059c310[obj]) +
                         AbsInt(g_anObjectRollRotation_0059d7e0[obj] -
                               g_anRollGoal_0059d630[obj]));
    match_rotation_goal(&g_anObjectPitchRotation_0059b2a0[obj],
                        &g_anPitchGoal_0059d7a0[obj], totalError,
                        typeData->pitchRate);
    match_rotation_goal(&g_anObjectYawRotation_0059ce80[obj],
                        &g_anYawGoal_0059c310[obj], totalError,
                        typeData->yawRate);
    match_rotation_goal(&g_anObjectRollRotation_0059d7e0[obj],
                        &g_anRollGoal_0059d630[obj], totalError,
                        typeData->rollRate);
}

/* Function start: 0x41E710 */
unsigned int AdjustShipSpeed(short ship, int delta)
{
    int maximumSpeed = (int)g_asShipMaximumSpeed_0059c440[ship] << 8;

    g_anShipSpeed_0059b320[ship] += delta;
    if (maximumSpeed < g_anShipSpeed_0059b320[ship])
        g_anShipSpeed_0059b320[ship] = maximumSpeed;
    if (g_anShipSpeed_0059b320[ship] < 0)
        g_anShipSpeed_0059b320[ship] = 0;
    return 0;
}

/* Function start: 0x41E750 */
unsigned int ApproachShipSpeed(short ship, int targetSpeed)
{
    int delta;
    int acceleration;

    acceleration = GetShipAccelerationRate(ship);
    delta = targetSpeed - g_anShipSpeed_0059b320[ship];

    if ((short)alert_flag(ship, 1))
        acceleration += acceleration;
    if ((delta < 0 ? -delta : delta) > acceleration)
        delta = MultiplyFixed(SignFixed(delta), acceleration);
    AdjustShipSpeed(ship, delta);
    return 0;
}

/* Function start: 0x41E7C0 */
unsigned int steady_object(short ship)
{
    DAT_0059c310[ship] = 0;
    DAT_0059d7a0[ship] = 0;
    DAT_0059d630[ship] = 0;
    return 0;
}

/* Function start: 0x41E7F0 */
short real_velocity(short obj)
{
    return FixedToShortSaturating(
        (int)ComputeFixedVectorMagnitude(&g_aShipVelocity_0059c010[obj]));
}

/* Function start: 0x41E820 */
unsigned int fix_velocity(short obj)
{
    ScaleFixedVector(&g_aShipForwardVector_0059bce0[obj],
                     g_anShipSpeed_0059b320[obj],
                     &g_aShipVelocity_0059c010[obj]);
    return 0;
}

/* Function start: 0x41E860 */
unsigned int sort_viable_target_list(void)
{
    short outer;
    short inner;
    short distance;
    signed char target;
    short count = (short)g_cViableTargetCount_0046c088;

    for (outer = 0; outer < count - 1; outer++) {
        for (inner = outer + 1; inner < count; inner++) {
            if (g_asViableTargetDistance_0059c470[inner] <
                g_asViableTargetDistance_0059c470[outer]) {
                distance = g_asViableTargetDistance_0059c470[outer];
                g_asViableTargetDistance_0059c470[outer] =
                    g_asViableTargetDistance_0059c470[inner];
                g_asViableTargetDistance_0059c470[inner] = distance;
                target = g_acViableTarget_0059c920[outer];
                g_acViableTarget_0059c920[outer] =
                    g_acViableTarget_0059c920[inner];
                g_acViableTarget_0059c920[inner] = target;
            }
        }
    }
    return 0;
}
