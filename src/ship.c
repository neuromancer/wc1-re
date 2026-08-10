/*
 *  Ship object, damage, explosion, and weapon logic from the Mac `ship`
 *  compilation unit.
 *
 *  Address range 0x41f000-0x420fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: the ordered Mac `ship` symbols map across the larger
 *  0x41dee0-0x42193f Win32 run; this file currently owns its central tranche.
 */
#include "wc1.h"

static const enum ObjectType g_aaeExplosionDebris_004698e0[4][7] = {
    {
        OBJECT_TYPE_DEBRIS_PIPE, OBJECT_TYPE_DEBRIS_O_RING,
        OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK,
        OBJECT_TYPE_DEBRIS_SHIP_TUBING, OBJECT_TYPE_DEBRIS_METAL_SHEET,
        OBJECT_TYPE_DEBRIS_WING, OBJECT_TYPE_DEBRIS_GLASS
    },
    {
        OBJECT_TYPE_DEBRIS_O_RING, OBJECT_TYPE_DEBRIS_O_RING,
        OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK,
        OBJECT_TYPE_DEBRIS_SHIP_TUBING, OBJECT_TYPE_DEBRIS_METAL_SHEET,
        OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK,
        OBJECT_TYPE_DEBRIS_GLASS
    },
    {
        OBJECT_TYPE_DEBRIS_PIPE, OBJECT_TYPE_DEBRIS_O_RING,
        OBJECT_TYPE_DEBRIS_METAL_SHEET,
        OBJECT_TYPE_DEBRIS_SHIP_TUBING, OBJECT_TYPE_DEBRIS_METAL_SHEET,
        OBJECT_TYPE_DEBRIS_WING, OBJECT_TYPE_DEBRIS_SHIP_TUBING
    },
    {
        OBJECT_TYPE_DEBRIS_GLASS, OBJECT_TYPE_DEBRIS_SHIP_TUBING,
        OBJECT_TYPE_DEBRIS_METAL_SHEET, OBJECT_TYPE_DEBRIS_WING,
        OBJECT_TYPE_DEBRIS_PIPE, OBJECT_TYPE_DEBRIS_O_RING,
        OBJECT_TYPE_DEBRIS_GLASS
    }
};

/* Function start: 0x41E650 */
unsigned int check_for_lost_control(short obj)
{
    ObjectTypeData *typeData;
    short control;
    short divisor;

    if (obj == 0 || g_aeSpecialManeuver_0059c3c0[obj] ==
                        SPECIAL_MANEUVER_UNKNOWN_9)
        return 0;
    typeData = &g_aObjectTypeData_00466458[g_aeObjectType_0059b560[obj]];
    divisor = (short)(typeData->rollRate + typeData->yawRate +
                      typeData->pitchRate);
    if (divisor == 0)
        return 0;
    control = (short)((abs(g_anObjectRollRotation_0059d7e0[obj]) +
                       abs(g_anObjectYawRotation_0059ce80[obj]) +
                       abs(g_anObjectPitchRotation_0059b2a0[obj])) /
                      divisor);
    if (skill_check(obj, control) == 0) {
        set_special(obj, SPECIAL_MANEUVER_BLOWING_UP);
        g_asObjectCounter_0059c330[obj] =
            (short)(RandomBelowOrEqual(6) + 5);
    }
    return 0;
}

/* Function start: 0x41E900 */
void send_appropriate_message(short attacker, short victim)
{
    short owner;

    if (attacker < 0 || attacker >= WC1_SPACE_OBJECT_COUNT ||
        g_aeObjectClass_0059d100[attacker] < OBJECT_CLASS_SHIP)
        return;
    owner = (short)g_acObjectOwner_0059ce20[attacker];
    if (g_nYourWingman_0046c04c != -1 &&
        owner == g_nYourWingman_0046c04c &&
        g_nYourWingman_0046c04c != attacker &&
        g_aeShipSide_0059d650[victim] == SIDE_KILRATHI) {
        if ((short)RandomBelowOrEqual(100) < 50 &&
            g_aeSpecialManeuver_0059c3c0[attacker] !=
                SPECIAL_MANEUVER_UNKNOWN_9)
            send_message(g_nYourWingman_0046c04c, 5);
    } else if (owner >= 0 && owner < WC1_SPACE_OBJECT_COUNT &&
               g_aeShipSide_0059d650[owner] == SIDE_KILRATHI &&
               g_nYourWingman_0046c04c == victim) {
        send_message(owner, 5);
    }
}

/* Function start: 0x41E9B0 */
int inflict_damage(short attacker, short victim, short damage)
{
    enum ObjectClass objectClass;
    ObjectTypeData *typeData;
    short shield;
    short quadrant;
    short remaining;
    int sideDot;
    int destroyed;

    if ((DAT_00469ffc == 0 && victim == 0) || damage == 0)
        return 0;
    if (victim < 0 || victim >= WC1_SPACE_OBJECT_COUNT)
        return 0;
    if (g_aeSpecialManeuver_0059c3c0[victim] ==
            SPECIAL_MANEUVER_UNKNOWN_9)
        return 0;
    objectClass = g_aeObjectClass_0059d100[victim];
    if (objectClass < OBJECT_CLASS_MISSILE)
        return 0;

    if (objectClass < OBJECT_CLASS_SHIP) {
        typeData = &g_aObjectTypeData_00466458[
            g_aeObjectType_0059b560[victim]];
        g_asShipAccumulatedDamage_0059dee0[victim] = (short)(
            g_asShipAccumulatedDamage_0059dee0[victim] + damage);
        if (typeData->damageCapacity != -1 &&
            g_asShipAccumulatedDamage_0059dee0[victim] >=
                typeData->damageCapacity)
            return explode(attacker, victim);
        return 0;
    }

    if (victim == 0)
        TriggerPlayerHitPaletteFlash();

    if (attacker >= 0 && attacker < WC1_SPACE_OBJECT_COUNT &&
        g_nYourWingman_0046c04c == victim &&
        g_acObjectOwner_0059ce20[attacker] == 0)
        send_message(victim, 10);

    shield = (short)(dot_product(&g_vCollisionDelta_0059d690,
        &g_aShipForwardVector_0059bce0[victim]) > 0);
    remaining = (short)(damage - g_aasShipShield_0059d5b0[victim][shield]);
    if (remaining < 1) {
        g_aasShipShield_0059d5b0[victim][shield] = (short)-remaining;
        if (attacker >= 0 &&
            g_aeObjectClass_0059d100[attacker] == OBJECT_CLASS_PROJECTILE)
            PlaySfxWaveFileByNumber(10, victim, 0);
        return 0;
    }
    g_aasShipShield_0059d5b0[victim][shield] = 0;
    if (attacker >= 0 &&
        g_aeObjectClass_0059d100[attacker] == OBJECT_CLASS_PROJECTILE)
        PlaySfxWaveFileByNumber(9, victim, 0);

    quadrant = shield;
    sideDot = dot_product(&g_vCollisionDelta_0059d690,
                          &g_aShipRightVector_0059b6e0[victim]);
    if (sideDot >= 0xb6)
        quadrant = 3;
    else if (sideDot < -0xb5)
        quadrant = 2;
    remaining = (short)(remaining -
                        g_aasShipArmor_0059d420[victim][quadrant]);
    if (remaining < 1) {
        g_aasShipArmor_0059d420[victim][quadrant] = (short)-remaining;
        return 0;
    }
    g_aasShipArmor_0059d420[victim][quadrant] = 0;

    if (g_asObjectScreenX_0059d9b0[victim] != (short)0x8001 &&
        objectClass != OBJECT_CLASS_CAPITAL_SHIP &&
        (short)RandomBelowOrEqual(1) == 0)
        Create_ship_hit_debris(attacker, 1);
    destroyed = 0;
    if ((short)RandomBelowOrEqual(99) == 0) {
        if (attacker != 0 && attacker != g_nYourWingman_0046c04c &&
            attacker >= 0 && attacker < WC1_SPACE_OBJECT_COUNT &&
            g_aeObjectClass_0059d100[attacker] == OBJECT_CLASS_SHIP) {
            if (g_aeShipSide_0059d650[attacker] == SIDE_KILRATHI)
                send_message(attacker, 6);
            destroyed = explode(attacker, victim);
        }
    } else {
        destroyed = internal_damage(attacker, victim, remaining, quadrant);
    }
    if (destroyed == 1)
        send_appropriate_message(attacker, victim);
    return destroyed;
}

/* Function start: 0x41EC60 */
short pilot_hit(short obj)
{
    if (DAT_0059cf00[obj] > 0) {
        DAT_0059cf00[obj]--;
        if (DAT_0059cf00[obj] == 0 && obj == 0) {
            if (DAT_00469ffc != 0)
                g_nArcadeState_00469fb0 = 4;
            return -1;
        }
        if (DAT_0059cf00[obj] != 0 && obj != 0 &&
            skill_check(obj, 9) == 0) {
            g_asObjectCounter_0059c330[obj] =
                (short)(RandomBelowOrEqual(20) + 30);
            set_special(obj, SPECIAL_MANEUVER_BLOWING_UP);
        }
    }
    return DAT_0059cf00[obj];
}

/* Function start: 0x41ECE0 */
int onboard_explosion(short obj)
{
    FixedVector offset;
    short debris;

    debris = find_vacant_3d_object();
    if (debris == -1)
        return 0;
    set_objects_data(debris, OBJECT_TYPE_EXPLOSION2, obj);
    g_asObjectScale_0059de40[debris] <<= 2;
    g_asObjectCounter_0059c330[debris] = 6;
    g_aShipVelocity_0059c010[debris] = g_aShipVelocity_0059c010[obj];
    MakeRandomVectorFixed(
        (short)(g_asObjectCollisionRadius_0059d710[obj] >> 2),
        (short)(g_asObjectCollisionRadius_0059d710[obj] >> 1), &offset);
    AddFixedVectors(&g_aShipPosition_0059c490[obj], &offset,
                    &g_aShipPosition_0059c490[debris]);
    return 1;
}

/* Function start: 0x41EDB0 */
void call_enemy(short obj)
{
    short other;

    other = 0;
    do {
        if (g_aeObjectClass_0059d100[other] >= OBJECT_CLASS_SHIP &&
            g_aeSpecialManeuver_0059c3c0[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            g_aeShipSide_0059d650[obj] != g_aeShipSide_0059d650[other] &&
            (short)RandomBelowOrEqual(100) < 50)
            g_acShipTarget_0059ce60[other] = (signed char)obj;
        other++;
    } while (other < 10);
}

/* Function start: 0x41EE20 */
int internal_damage(short attacker, short victim, short damage,
                    short quadrant)
{
    ObjectTypeData *typeData;
    short events;
    short system;
    short weaponCount;

    if (victim == 0) {
        your_internal_damage(attacker, damage, quadrant);
        return g_nArcadeState_00469fb0 == 4;
    }
    typeData = &g_aObjectTypeData_00466458[
        g_aeObjectType_0059b560[victim]];
    if (g_aeObjectClass_0059d100[victim] == OBJECT_CLASS_CAPITAL_SHIP) {
        if (g_aeShipSide_0059d650[victim] == SIDE_KILRATHI) {
            events = MaxShort(1, (short)(damage >> 3));
            if (attacker >= 0 && attacker < 10 &&
                any_enemy(attacker, 10000) == 0)
                call_enemy(attacker);
        } else {
            events = MaxShort(1, (short)(damage / 10));
            if ((short)RandomBelowOrEqual(1000) < 35 && attacker != 0)
                send_message(victim, 4);
        }
        g_asShipAccumulatedDamage_0059dee0[victim] = (short)(
            g_asShipAccumulatedDamage_0059dee0[victim] + events);
        if (g_asShipAccumulatedDamage_0059dee0[victim] >=
            typeData->damageCapacity)
            return explode(attacker, victim);
        onboard_explosion(victim);
        return 0;
    }

    if (g_acShipRating_0059cd80[victim] == -1) {
        events = MaxShort(1, (short)(damage / 6));
    } else {
        events = MaxShort(1, (short)(damage / 40));
        events = MinShort((short)RandomInRange(3, 4), events);
    }
    g_asShipAccumulatedDamage_0059dee0[victim] = (short)(
        g_asShipAccumulatedDamage_0059dee0[victim] + events);

    while (events > 0) {
        events--;
        system = (short)RandomBelowOrEqual(9);
        switch (system) {
        case 0:
            pilot_hit(victim);
            break;
        case 1:
            if (quadrant == 1)
                damage_ion_drive(victim, 1, 3);
            break;
        case 2:
            if (quadrant == 1)
                return explode(attacker, victim);
            break;
        case 3:
            g_aasShipShield_0059d5b0[victim][0] = 0;
            g_aasShipShield_0059d5b0[victim][1] = 0;
            g_aasShipMaximumShield_0059d6e0[victim][0] = 0;
            g_aasShipMaximumShield_0059d6e0[victim][1] = 0;
            break;
        case 4:
            g_acShipDamage_0059c460[victim]++;
            if ((short)g_acShipDamage_0059c460[victim] >
                typeData->damageCapacity)
                return explode(attacker, victim);
            break;
        case 5:
            if (quadrant == 0) {
                weaponCount = (short)(signed char)
                    g_aShipWeapons_0059cab0[victim][0];
                if (weaponCount > 0)
                    remove_weapon(victim,
                        (short)RandomBelowOrEqual(
                            (short)(weaponCount - 1)));
            }
            break;
        case 6:
            if (quadrant == 0 &&
                g_acShipDestroyedWeaponCount_0059de30[victim] < 5)
                g_acShipDestroyedWeaponCount_0059de30[victim]++;
            break;
        case 7:
            if (quadrant == 1) {
                drain_fuel(victim, (short)(typeData->fuelCapacity / 4));
                if ((short)RandomBelowOrEqual(1) != 0 ||
                    g_anShipFuel_0059b470[victim] < 0)
                    return explode(attacker, victim);
            }
            break;
        case 8:
            if (quadrant == 0 &&
                g_acShipCommunicator_0059c850[victim] != -1)
                g_acShipCommunicator_0059c850[victim] = -1;
            break;
        }
    }
    return 0;
}

/* Function start: 0x41F1A0 */
void revise_shields(short obj)
{
    ObjectTypeData *typeData;
    short maximum;

    typeData = &g_aObjectTypeData_00466458[
        g_aeObjectType_0059b560[obj]];
    maximum = MaxShort(0, (short)(g_aasShipMaximumShield_0059d6e0[obj][0] -
                                 (typeData->shieldFore >> 2)));
    g_aasShipMaximumShield_0059d6e0[obj][0] = maximum;
    g_aasShipMaximumShield_0059d6e0[obj][1] =
        MaxShort(0, (short)(maximum - (typeData->shieldAft >> 2)));
}

/* Function start: 0x41F220 */
void your_internal_damage(short attacker, short damage, short quadrant)
{
    enum ObjectClass attackerClass;
    ObjectTypeData *typeData;
    short tableGroup;
    short events;
    short system;
    short severity;
    short weaponCount;

    attackerClass = OBJECT_CLASS_MISSILE;
    if (attacker >= 0 && attacker < WC1_SPACE_OBJECT_COUNT)
        attackerClass = g_aeObjectClass_0059d100[attacker];
    if (attackerClass == OBJECT_CLASS_PROJECTILE) {
        tableGroup = quadrant == 1 ? 2 : 0;
        events = MaxShort(1, (short)(damage >> 4));
    } else if (attackerClass == OBJECT_CLASS_ASTEROID ||
               attackerClass >= OBJECT_CLASS_SHIP) {
        tableGroup = 4;
        events = MaxShort(1, (short)(damage >> 7));
    } else {
        tableGroup = (short)((quadrant == 1 ? 2 : 0) + 1);
        events = MaxShort(1, (short)(damage >> 5));
    }
    typeData = &g_aObjectTypeData_00466458[g_aeObjectType_0059b560[0]];
    severity = (short)RandomBelowOrEqual(10);
    g_asShipAccumulatedDamage_0059dee0[0] = (short)(
        g_asShipAccumulatedDamage_0059dee0[0] + events);

    while (events > 0) {
        events--;
        system = g_asPlayerDamageSystemTable_00469878[
            tableGroup * 10 + (short)RandomBelowOrEqual(9)];
        switch (system) {
        case 0:
            if (severity < 4)
                pilot_hit(0);
            else if (severity < 7)
                damage_your_component(5, 2, 4);
            else
                damage_your_component(4, 2, 4);
            break;
        case 1:
            if (quadrant == 1) {
                damage_your_component(0, 1, 3);
                damage_ion_drive(0, 1, 3);
            }
            break;
        case 2:
            if (quadrant == 1) {
                if ((short)RandomBelowOrEqual(3) == 0 ||
                    damage_your_component(1, 1, 4) == 4) {
                    explode(attacker, 0);
                    return;
                }
            }
            break;
        case 3:
            damage_your_component(2, 1, 4);
            revise_shields(0);
            break;
        case 4:
            g_acShipDamage_0059c460[0]++;
            if (g_acShipDamage_0059c460[0] == 1) {
                pilot_hit(0);
                if (events > 0)
                    events--;
            } else if ((short)g_acShipDamage_0059c460[0] >
                       typeData->damageCapacity) {
                explode(attacker, 0);
                return;
            }
            break;
        case 5:
            if (quadrant == 0) {
                weaponCount = (short)(signed char)g_aShipWeapons_0059cab0[0][0];
                if (weaponCount > 0) {
                    remove_weapon(0,
                        (short)RandomBelowOrEqual(
                            (short)(weaponCount - 1)));
                    ShowComponentHitHudMessage(
                        (char *)g_szWeaponDestroyed_00469960,
                        DAT_004699ac, 11);
                }
            }
            break;
        case 6:
            if (quadrant == 0 &&
                g_acShipDestroyedWeaponCount_0059de30[0] < 5) {
                g_acShipDestroyedWeaponCount_0059de30[0]++;
                check_computer_damage();
            }
            break;
        case 7:
            drain_fuel(0, (short)(typeData->fuelCapacity / 4));
            if ((short)RandomBelowOrEqual(1) != 0 ||
                g_anShipFuel_0059b470[0] < 0) {
                explode(attacker, 0);
                return;
            }
            ShowComponentHitHudMessage((char *)g_szFuelTanksHit_00469974,
                                       DAT_004699ac, 11);
            break;
        case 8:
            if (quadrant == 0) {
                if (severity > 6)
                    damage_your_component(5, 4, 4);
                else
                    damage_your_component(4, 2, 3);
                if (g_acPlayerComponentDamage_0059bff0[4] > 3)
                    g_acShipCommunicator_0059c850[0] = -1;
            }
            break;
        }
    }
}

/* Function start: 0x41F5D0 */
unsigned int check_computer_damage(void)
{
    damage_your_component(3, 1, 3);
    return 0;
}

/* Function start: 0x41F5F0 */
short ReportComponentRepaired(short component, short minimumDamage)
{
    char message[80];

    if ((short)g_acPlayerComponentDamage_0059bff0[component] <=
        minimumDamage)
        return 0;
    g_acPlayerComponentDamage_0059bff0[component]--;
    sprintf(message, g_szComponentFixedFormat_00469984,
            g_apszComponentNames_0046a778[component]);
    ShowComponentHitHudMessage(message, DAT_004699ac, 8);
    return 1;
}

/* Function start: 0x41F660 */
void repair_internal_damage(void)
{
    short repair;

    /* The retail Win32 function reads an uninitialised stack word for its
     * outer component-damage guard.  Attempting the repair is equivalent for
     * valid undestroyed state; the individual repair paths retain their real
     * damage thresholds without risking an out-of-bounds array access. */
    if ((short)RandomBelowOrEqual(500) >= 2)
        return;
    repair = (short)RandomBelowOrEqual(2);
    switch (repair) {
    case 0:
        ReportComponentRepaired(2, 1);
        break;
    case 1:
        if (ReportComponentRepaired(0, 2) != 0)
            damage_ion_drive(0, -1, 3);
        break;
    case 2:
        if (g_aObjectTypeData_00466458[
                g_aeObjectType_0059b560[0]].damageCapacity - 3 <
            (short)g_acShipDamage_0059c460[0])
            g_acShipDamage_0059c460[0]--;
        break;
    }
}

/* Function start: 0x41F700 */
void Create_ship_hit_debris(short obj, short count)
{
    FixedVector offset;
    short debris;
    short created;

    created = 0;
    while (created < count) {
        debris = find_vacant_3d_object();
        if (debris == -1)
            return;
        set_objects_data(debris,
            g_aeShipHitDebrisTypes_00469950[
                (short)RandomBelowOrEqual(2)], -1);
        g_asObjectCounter_0059c330[debris] = 40;
        FillFixedVectorWithRandomComponents(10, &offset);
        AddFixedVectors(&g_aShipPosition_0059c490[obj], &offset,
                        &g_aShipPosition_0059c490[debris]);
        FillFixedVectorWithRandomComponents(
            6, &g_aShipVelocity_0059c010[debris]);
        created++;
    }
}

/* Function start: 0x41F7C0 */
void check_next_wave(void)
{
    short obj;

    if (g_nCurrentWave_0046c01c == -1)
        return;
    obj = 0;
    do {
        if (g_aeObjectClass_0059d100[obj] == OBJECT_CLASS_SHIP &&
            g_aeShipSide_0059d650[obj] == SIDE_KILRATHI)
            return;
        obj++;
    } while (obj < 10);
    set_up_next_wave();
}

/* Function start: 0x41F800 */
void Create_explosion_debris(short obj)
{
    FixedVector position;
    FixedVector velocity;
    FixedVector offset;
    FixedVector inheritedVelocity;
    short debris;
    short set;
    short index;

    position = g_aShipPosition_0059c490[obj];
    velocity = g_aShipVelocity_0059c010[obj];
    remove_object(obj);
    check_next_wave();
    set = RandomBelowOrEqual(3);
    index = 0;
    do {
        debris = find_vacant_3d_object();
        if (debris == -1)
            break;
        set_objects_data(debris, g_aaeExplosionDebris_004698e0[set][index],
                         -1);
        g_asObjectCounter_0059c330[debris] = 40;
        FillFixedVectorWithRandomComponents(50, &offset);
        AddFixedVectors(&position, &offset,
                        &g_aShipPosition_0059c490[debris]);
        FillFixedVectorWithRandomComponents(
            25, &g_aShipVelocity_0059c010[debris]);
        divide_vector(&velocity, 0x200, &inheritedVelocity);
        AddFixedVectors(&inheritedVelocity,
                        &g_aShipVelocity_0059c010[debris],
                        &g_aShipVelocity_0059c010[debris]);
        index++;
    } while (index < 7);
    index = 0;
    do {
        debris = find_vacant_3d_object();
        if (debris == -1)
            return;
        FillFixedVectorWithRandomComponents(50, &offset);
        AddFixedVectors(&position, &offset,
                        &g_aShipPosition_0059c490[debris]);
        FillFixedVectorWithRandomComponents(
            25, &g_aShipVelocity_0059c010[debris]);
        divide_vector(&velocity, 0x200, &inheritedVelocity);
        AddFixedVectors(&inheritedVelocity,
                        &g_aShipVelocity_0059c010[debris],
                        &g_aShipVelocity_0059c010[debris]);
        g_asObjectScreenAngle_0059cd90[debris] =
            (short)(RandomBelowOrEqual(3) + 0x10);
        g_asObjectCounter_0059c330[debris] = 40;
        g_aeObjectClass_0059d100[debris] = OBJECT_CLASS_DUST;
        g_aeObjectType_0059b560[debris] = OBJECT_TYPE_DEBRIS_DUST;
        index++;
    } while (index < 8);
}

/* Function start: 0x41FBC0 */
short ShipExplosion(short obj)
{
    unsigned short originalScale;
    FixedVector position;
    FixedVector velocity;
    short explosion;

    originalScale = (unsigned short)g_asObjectScale_0059de40[obj];
    position = g_aShipPosition_0059c490[obj];
    velocity = g_aShipVelocity_0059c010[obj];
    explosion = find_vacant_3d_object();
    if (explosion == -1)
        explosion = obj;
    else
        copy_frame(obj, explosion);
    set_objects_data(explosion, OBJECT_TYPE_EXPLOSION1, obj);
    g_aShipPosition_0059c490[explosion] = position;
    g_aShipVelocity_0059c010[explosion] = velocity;
    g_asObjectScale_0059de40[explosion] = (short)(
        (unsigned int)(unsigned short)g_asObjectScale_0059de40[explosion] *
        originalScale >> 8);
    return explosion;
}

/* Function start: 0x41FCD0 */
short Explosion(short obj)
{
    enum ObjectClass objectClass;
    enum ObjectType explosionType;
    short missionShip;
    short explosion;

    objectClass = g_aeObjectClass_0059d100[obj];
    explosion = obj;
    if (objectClass < OBJECT_CLASS_SHIP) {
        explosionType = OBJECT_TYPE_EXPLOSION2;
        if (g_aeObjectType_0059b560[obj] == OBJECT_TYPE_TURRET ||
            objectClass == OBJECT_CLASS_ASTEROID)
            explosionType = OBJECT_TYPE_EXPLOSION0;
        set_objects_data(obj, explosionType, g_acObjectOwner_0059ce20[obj]);
        if (objectClass == OBJECT_CLASS_ASTEROID)
            g_asObjectScale_0059de40[obj] = 0x380;
    } else {
        if (obj < 10)
            send_message(obj, 7);
        set_special(obj, SPECIAL_MANEUVER_UNKNOWN_9);
        g_asObjectCounter_0059c330[obj] = 8;
        missionShip = g_nShipMissionIndices_0059c830[obj];
        if (missionShip >= 0 && missionShip < WC1_MISSION_SHIP_COUNT)
            g_aMissionShips_0046c948[missionShip].state = 3;
        if (objectClass == OBJECT_CLASS_CAPITAL_SHIP) {
            short count = 4;

            do {
                onboard_explosion(obj);
                count--;
            } while (count != 0);
            g_asObjectCounter_0059c330[obj] =
                (short)(g_aObjectTypeData_00466458[
                    g_aeObjectType_0059b560[obj]].lifetime / 4 + 8);
        } else {
            explosion = ShipExplosion(obj);
        }
    }
    if (objectClass != OBJECT_CLASS_CAPITAL_SHIP)
        explosion_shock_wave(
            obj, g_aObjectTypeData_00466458[
                g_aeObjectType_0059b560[obj]].fuelCapacity);
    if (g_asObjectScreenX_0059d9b0[obj] != (short)0x8001)
        PlaySfxWaveFileByNumber(4, obj, 0);
    return explosion;
}

/* Function start: 0x41FEB0 */
short the_creator(short obj)
{
    short owner;
    short steps;

    steps = 0;
    while (obj != -1 && obj >= 0 && obj < WC1_SPACE_OBJECT_COUNT &&
           steps < WC1_SPACE_OBJECT_COUNT) {
        owner = (short)g_acObjectOwner_0059ce20[obj];
        if (owner == obj || owner == -1)
            return obj;
        obj = owner;
        steps++;
    }
    return -1;
}

/* Function start: 0x41FEE0 */
int explosion_shock_wave(short obj, short blastDamage)
{
    FixedVector delta;
    FixedVector force;
    short other;
    short distance;
    short divisor;
    short damage;
    short attacker;

    other = 0;
    do {
        if (other != obj &&
            g_aeObjectClass_0059d100[other] >= OBJECT_CLASS_SHIP) {
            ComputeVectorDelta(&g_aShipPosition_0059c490[obj],
                               &g_aShipPosition_0059c490[other], &delta);
            distance = FixedToShortSaturating(
                (int)Vector_magnitude(&delta));
            distance = MaxShort(0,
                (short)(distance -
                        g_asObjectCollisionRadius_0059d710[other]));
            damage = 0;
            if (distance < 1000) {
                if (distance < 501)
                    divisor = find_ratio(0, 500, distance, 8, 25);
                else if (distance < 751)
                    divisor = 30;
                else
                    divisor = 40;
                damage = (short)((blastDamage / divisor) / divisor);
                if (other == 0)
                    damage = MaxShort(1,
                        (short)(damage * 3 >> 2));
            }
            if (damage > 1) {
                NormalizeFixedVector(&delta);
                ScaleFixedVector(&delta, (int)damage << 8, &force);
                apply_force_to_objects_center(&force, other);
                g_vCollisionDelta_0059d690 = delta;
                attacker = the_creator(obj);
                inflict_damage(attacker, other,
                               MinShort(100, damage));
            }
        }
        other++;
    } while (other < 10);
    return 0;
}

/* Function start: 0x420040 */
int explode(short attacker, short victim)
{
    if (victim < 0 || g_aeObjectClass_0059d100[victim] == OBJECT_CLASS_NULL)
        return 0;
    if (g_aeSpecialManeuver_0059c3c0[victim] ==
            SPECIAL_MANEUVER_UNKNOWN_9 &&
        g_aeObjectClass_0059d100[victim] >= OBJECT_CLASS_SHIP)
        return 0;
    if (victim == 0) {
        if (DAT_00469ffc == 0)
            return 0;
        g_nArcadeState_00469fb0 = 4;
        return 1;
    }
    Explosion(victim);
    return 1;
}

/* Function start: 0x420190 */
int send_at_point(short obj, FixedVector *point, short speed)
{
    ComputeVectorDelta(&g_aShipPosition_0059c490[obj], point,
                       &g_aShipVelocity_0059c010[obj]);
    SetVectorFixedPoint((unsigned int *)&g_aShipVelocity_0059c010[obj],
                        speed);
    return 0;
}

/* Function start: 0x4201D0 */
short find_child_object(short parent, enum ObjectClass objectClass)
{
    short obj;

    obj = 0;
    do {
        if ((short)g_acObjectOwner_0059ce20[obj] == parent &&
            g_aeObjectClass_0059d100[obj] == objectClass)
            return obj;
        obj++;
    } while (obj < WC1_SPACE_OBJECT_COUNT);
    return -1;
}

/* Function start: 0x420210 */
short find_child_ship(short parent, enum ObjectClass objectClass,
                      short target)
{
    short obj;

    obj = 0;
    do {
        if ((short)g_acObjectOwner_0059ce20[obj] == parent &&
            g_aeObjectClass_0059d100[obj] == objectClass &&
            (target == -1 ||
             (short)g_acShipTarget_0059ce60[obj] == target))
            return obj;
        obj++;
    } while (obj < 10);
    return -1;
}

/* Function start: 0x420260 */
int launch_object(short parent, short child, FixedVector direction,
                  short speed)
{
    FixedVector launchVelocity;

    NormalizeFixedVector(&direction);
    vector_component_in_dir(&g_aShipVelocity_0059c010[parent],
                            &direction,
                            &g_aShipVelocity_0059c010[child]);
    ScaleFixedVector(&direction, (int)speed << 8, &launchVelocity);
    AddFixedVectors(&launchVelocity, &g_aShipVelocity_0059c010[child],
                    &g_aShipVelocity_0059c010[child]);
    return 0;
}

/* Function start: 0x4202D0 */
void fire(short obj, short target)
{
    ShipWeaponSlot *slot;
    ObjectTypeData *weaponData;
    enum ObjectType weaponType;
    enum ObjectClass weaponClass;
    FixedVector direction;
    FixedVector *interceptPoint;
    short velocityAngle;
    short mineTime;
    short closingSpeed;
    short weaponVelocity;
    short weapon;
    short range;
    short firedObject;
    short predictionTime;
    short predictedSeparation;
    short shouldFire;
    int canFire;
    int fireMissile;
    int minePresent;
    int targetInRange;
    int loadoutOffset;

    canFire = g_asObjectCounter_0059c330[obj] <= 0;
    get_facing_range_from_object(obj, target);
    range = g_nTargetRange_0059ce10;
    closingSpeed = (short)(((g_anShipSpeed_0059b320[target] *
                             (int)g_nTargetFacing_0059d52a) / 100) >> 8);
    fireMissile = (short)RandomBelowOrEqual(19) == 0 &&
                  (short)RandomBelowOrEqual(7000) > range;
    if (fireMissile &&
        find_child_ship(obj, OBJECT_CLASS_MISSILE, target) != -1)
        fireMissile = 0;
    minePresent = 0;
    if (find_child_object(obj, OBJECT_CLASS_MINE) != -1)
        minePresent = 1;

    loadoutOffset = (int)obj * sizeof(g_aShipWeapons_0059cab0[0]);
    weapon = 0;
    while (weapon < *(signed char *)((unsigned char *)
               g_aShipWeapons_0059cab0 + loadoutOffset)) {
        slot = (ShipWeaponSlot *)((unsigned char *)
            g_aShipWeapons_0059cab0 + loadoutOffset +
            weapon * sizeof(ShipWeaponSlot) + 1);
        weaponType = slot->type;
        weaponData = &g_aObjectTypeData_00466458[weaponType];
        weaponVelocity = weaponData->maximumVelocity;
        if (closingSpeed < 0)
            weaponVelocity = (short)(weaponVelocity + closingSpeed / 100);
        targetInRange =
            (short)(weaponData->lifetime * weaponVelocity) > range;
        shouldFire = 0;
        weaponClass = weaponData->objectClass;

        if (weaponClass == OBJECT_CLASS_PROJECTILE) {
            if (canFire && targetInRange) {
                switch (weaponType) {
                case OBJECT_TYPE_LASER_CANNON:
                    shouldFire = g_nFacingToTarget_0059d920 > 70;
                    break;
                case OBJECT_TYPE_NEUTRON_PARTICLE_GUN:
                    shouldFire = g_nFacingToTarget_0059d920 > 80;
                    break;
                case OBJECT_TYPE_MASS_DRIVER_CANNON:
                    shouldFire = g_nFacingToTarget_0059d920 > 85;
                    break;
                case OBJECT_TYPE_TURRET:
                    shouldFire = g_nFacingToTarget_0059d920 > 10;
                    break;
                }
            }
            slot->disabled = (signed char)!shouldFire;
            if (shouldFire) {
                firedObject = (short)fire_weapon(obj, weapon);
                if (weaponType == OBJECT_TYPE_TURRET) {
                    ComputeVectorDelta(
                        &g_aShipPosition_0059c490[firedObject],
                        &g_aShipPosition_0059c490[target], &direction);
                    launch_object(obj, firedObject, direction,
                                  real_velocity(firedObject));
                }
            }
        } else if (weaponClass == OBJECT_CLASS_MINE) {
            /* The first angle test deliberately preserves the original
             * stack-local ordering recovered at 0x42056a. */
            if (!minePresent && weaponType == OBJECT_TYPE_SPACE_MINE &&
                g_anShipSpeed_0059b320[obj] >= 0x500 &&
                velocityAngle >= 75 && range <= 2000 &&
                g_nFacingToTarget_0059d920 >= -50 &&
                g_nTargetFacing_0059d52a <= 90) {
                velocityAngle = vector_angle(
                    g_aShipVelocity_0059c010[target],
                    g_aShipVelocity_0059c010[obj]);
                predictionTime = (short)(900 /
                    (short)((g_anShipSpeed_0059b320[obj] >> 8) + 20));
                predictedSeparation = (short)(predictionTime *
                    (short)(-20 - closingSpeed) + range);
                if (closingSpeed == -20)
                    mineTime = range;
                else
                    mineTime = (short)(range / (closingSpeed + 20));
                if (range < 2000 && g_nFacingToTarget_0059d920 < -50 &&
                    g_nTargetFacing_0059d52a > 90) {
                    shouldFire = 1;
                    if (predictedSeparation <= 50)
                        shouldFire = 0;
                }
            }
            if (shouldFire) {
                firedObject = (short)drop_mine(
                    obj, (signed char)weapon, weaponType,
                    (short)(mineTime + 15));
                if (firedObject != -1) {
                    ScaleFixedVector(&g_aShipVelocity_0059c010[target],
                                     (int)mineTime, &direction);
                    interceptPoint = &g_aShipPosition_0059c490[
                        WC1_SPACE_OBJECT_COUNT - 1];
                    AddFixedVectors(&g_aShipPosition_0059c490[target],
                                    &direction, interceptPoint);
                    ComputeVectorDelta(
                        &g_aShipPosition_0059c490[firedObject],
                        interceptPoint, &direction);
                    launch_object(obj, firedObject, direction, 20);
                }
                minePresent = 1;
            }
        } else if (weaponClass == OBJECT_CLASS_MISSILE) {
            if (fireMissile && targetInRange) {
                switch (weaponType) {
                case OBJECT_TYPE_DUMB_FIRE_MISSILE:
                    shouldFire = g_nFacingToTarget_0059d920 > 97;
                    break;
                case OBJECT_TYPE_HEAT_SEEKING_MISSILE:
                    shouldFire = g_nFacingToTarget_0059d920 > 40 &&
                                 g_nTargetFacing_0059d52a < -60;
                    break;
                case OBJECT_TYPE_FF_MISSILE:
                    shouldFire = 1;
                    break;
                case OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE:
                    shouldFire = g_nFacingToTarget_0059d920 > 40;
                    break;
                }
            }
            if (shouldFire) {
                fire_weapon(obj, weapon);
                fireMissile = 0;
            }
        }
        weapon++;
    }
}

/* Function start: 0x4207E0 */
short hemisphere(FixedVector *target, FixedVector *parent,
                 FixedVector *hardpoint)
{
    FixedVector parentFromHardpoint;
    FixedVector parentFromTarget;

    ComputeVectorDelta(hardpoint, parent, &parentFromHardpoint);
    ComputeVectorDelta(target, parent, &parentFromTarget);
    return vector_angle(parentFromTarget, parentFromHardpoint);
}

/* Function start: 0x420840 */
int fire_flack(short owner, short explosion, short range,
               FixedVector *aim)
{
    short projectileVelocity;
    short lifetime;

    projectileVelocity =
        g_aObjectTypeData_00466458[OBJECT_TYPE_TURRET].maximumVelocity;
    set_objects_data(explosion, OBJECT_TYPE_TURRET, owner);
    lifetime = (short)(range / projectileVelocity -
                       (short)RandomBelowOrEqual(8) - 5);
    lifetime = MaxShort(5, lifetime);
    lifetime = MinShort(27, lifetime);
    g_asObjectCounter_0059c330[explosion] = lifetime;
    send_at_point(explosion, aim, projectileVelocity);
    return 0;
}

/* Function start: 0x4208C0 */
short rnd_sign(short v)
{
    if ((short)RandomBelowOrEqual(1) != 0)
        return v;
    return -v;
}

/* Function start: 0x4208E0 */
int rnd_aim(short radius, short speed, short maximum)
{
    short aim;

    aim = MinShort(maximum,
                   (short)((short)RandomBelowOrEqual(radius) + speed));
    return (int)rnd_sign(aim) << 8;
}

/* Function start: 0x420920 */
short pop_flack(short obj, short range, FixedVector *hardpoint)
{
    FixedVector randomAim;
    FixedVector aimPoint;
    short target;
    short explosion;
    short aimRadius;
    short targetSpeed;
    short chance;

    target = (short)g_acShipTarget_0059ce60[obj];
    explosion = new_object(OBJECT_TYPE_EXPLOSION0, obj);
    if (explosion != -1) {
        aimRadius = MaxShort(400, (short)(range >> 2));
        targetSpeed = (short)(real_velocity(target) << 4);
        randomAim.x = rnd_aim(aimRadius, targetSpeed, 1500);
        randomAim.y = rnd_aim(aimRadius, targetSpeed, 1500);
        randomAim.z = rnd_aim(aimRadius, targetSpeed, 1500);
        AddFixedVectors(&g_aShipPosition_0059c490[target], &randomAim,
                        &aimPoint);
        chance = (short)RandomBelowOrEqual(100);
        if ((g_asObjectCounter_0059c330[obj] != -1 || chance > 39) &&
            chance > 7) {
            g_aShipPosition_0059c490[explosion] = aimPoint;
            explosion_shock_wave(
                explosion,
                g_aObjectTypeData_00466458[
                    OBJECT_TYPE_TURRET].fuelCapacity);
            return explosion;
        }
        g_aShipPosition_0059c490[explosion] = *hardpoint;
        fire_flack(obj, explosion, range, &aimPoint);
        g_asObjectCounter_0059c330[obj] = (short)(RandomBelow(10) + 7);
    }
    return explosion;
}

/* Function start: 0x420AA0 */
int fire_turrets(short obj)
{
    ShipWeaponSlot *slot;
    FixedVector hardpoint;
    short lastTarget;
    short weapon;
    short startTarget;
    short targetIndex;
    short target;
    short targetHemisphere;
    int loadoutOffset;

    lastTarget = build_target_list(obj, 5000);
    lastTarget--;
    if (lastTarget == -1)
        return 0;

    loadoutOffset = (int)obj * sizeof(g_aShipWeapons_0059cab0[0]);
    slot = (ShipWeaponSlot *)((unsigned char *)
        g_aShipWeapons_0059cab0 + loadoutOffset + 1);
    weapon = 0;
    while (weapon < *(signed char *)((unsigned char *)
               g_aShipWeapons_0059cab0 + loadoutOffset)) {
        if ((short)RandomBelowOrEqual(2) == 0) {
            position_child(obj, slot->hardpoint, &hardpoint);
            startTarget = (short)RandomBelowOrEqual(lastTarget);
            targetIndex = startTarget;
            do {
                target = (short)g_acFormationMemberList_0059d490[
                    targetIndex];
                targetHemisphere = hemisphere(
                    &g_aShipPosition_0059c490[target],
                    &g_aShipPosition_0059c490[obj], &hardpoint);
                if (slot->type != OBJECT_TYPE_TURRET) {
                    if (targetHemisphere > 50 &&
                        (short)RandomBelowOrEqual(14) == 0) {
                        g_acShipTarget_0059ce60[obj] =
                            (signed char)target;
                        fire_weapon(obj, weapon);
                    }
                    break;
                }
                if (targetHemisphere >= 25) {
                    g_acShipTarget_0059ce60[obj] = (signed char)target;
                    pop_flack(obj,
                              g_asTargetListRange_0059cf60[targetIndex],
                              &hardpoint);
                    break;
                }
                targetIndex++;
                if (targetIndex > lastTarget)
                    targetIndex = 0;
            } while (targetIndex != startTarget);
        }
        weapon++;
        slot++;
    }
    return 1;
}

/* Function start: 0x420C20 */
int fire_weapon(short obj, short weapon)
{
    ShipWeaponSlot *slot;
    ObjectTypeData *weaponData;
    enum ObjectType weaponType;
    enum ObjectClass weaponClass;
    FixedVector vector;
    FixedVector cockpitOffset;
    int range;
    short projectileSpeed;
    short projectile;

    slot = &((ShipWeaponSlot *)&g_aShipWeapons_0059cab0[obj][1])[weapon];
    weaponType = slot->type;
    weaponClass = g_aObjectTypeData_00466458[weaponType].objectClass;
    if (weaponType == OBJECT_TYPE_TURRET) {
        weaponClass = OBJECT_CLASS_PROJECTILE;
        weaponType = OBJECT_TYPE_LASER_CANNON;
    }
    if (weaponClass == OBJECT_CLASS_MINE)
        return drop_mine(obj, (signed char)weapon, weaponType, -1);
    if (weaponClass == OBJECT_CLASS_MISSILE)
        projectile = initialize_ship(weaponType, obj);
    else
        projectile = new_object(weaponType, obj);
    if (projectile == -1)
        return projectile;
    weaponData = &g_aObjectTypeData_00466458[weaponType];
    copy_frame(obj, projectile);
    projectileSpeed = 10;
    if (weaponClass == OBJECT_CLASS_PROJECTILE) {
        g_asShipAccumulatedDamage_0059dee0[projectile] =
            weaponData->damageCapacity;
        projectileSpeed = g_aObjectTypeData_00466458[
            g_aeObjectType_0059b560[projectile]].maximumVelocity;
        g_asShipWeaponEnergy_0059d470[obj] =
            (short)(g_asShipWeaponEnergy_0059d470[obj] -
                    weaponData->animationDelay);
    }
    child_object(slot->hardpoint, projectile, obj);
    g_asObjectCounter_0059c330[projectile] = weaponData->lifetime;
    vector_component_in_dir(&g_aShipVelocity_0059c010[obj],
                            &g_aShipForwardVector_0059bce0[projectile],
                            &g_aShipVelocity_0059c010[projectile]);
    if (weaponClass == OBJECT_CLASS_PROJECTILE) {
        ScaleFixedVector(&g_aShipForwardVector_0059bce0[obj],
            (int)(short)((weaponData->damageCapacity + 5) *
                         weaponData->maximumVelocity) << 8,
            &vector);
        AddFixedVectors(&g_aShipPosition_0059c490[obj], &vector, &vector);
        point_at(projectile, vector);
        if (DAT_0046a008 != 0 && g_cCockpitView_0059dab0 == 3) {
            ScaleFixedVector(&g_aShipUpVector_0059b9e0[obj], 0x12200,
                             &cockpitOffset);
            AddFixedVectors(&cockpitOffset, &vector, &vector);
            point_at(projectile, vector);
        }
    }
    ScaleFixedVector(&g_aShipForwardVector_0059bce0[projectile],
                     (int)projectileSpeed << 8, &vector);
    AddFixedVectors(&vector, &g_aShipVelocity_0059c010[projectile],
                    &g_aShipVelocity_0059c010[projectile]);
    if (weaponClass == OBJECT_CLASS_MISSILE) {
        ScaleFixedVector(&g_aShipUpVector_0059b9e0[obj], 0xa00, &vector);
        AddFixedVectors(&vector, &g_aShipVelocity_0059c010[projectile],
                        &g_aShipVelocity_0059c010[projectile]);
        if (obj == 0)
            RemovePlayerReleaseWeapon((signed char)weapon);
        else
            remove_weapon(obj, weapon);
        g_aShipMissionSpot_0059dd10[0xa0 + projectile] = 20;
        g_aeSpecialManeuver_0059c3c0[projectile] =
            SPECIAL_MANEUVER_NONE;
        g_aeShipManeuver_0059dcb0[projectile] = MANEUVER_NONE;
        g_aeShipTactic_0059d5e0[projectile] = TACTIC_SIT_STILL;
        g_asObjectCounter_0059c330[projectile] = 5;
        switch (weaponType) {
        case OBJECT_TYPE_DUMB_FIRE_MISSILE:
            steady_object(projectile);
            g_asObjectCounter_0059c330[projectile] = 1;
            g_acShipTarget_0059ce60[projectile] =
                g_acShipTarget_0059ce60[obj];
            g_anShipSpeed_0059b320[projectile] =
                (int)get_ship_max_velocity(projectile) << 8;
            if (g_acShipTarget_0059ce60[projectile] != -1) {
                ComputeVectorDelta(&g_aShipPosition_0059c490[obj],
                    &g_aShipPosition_0059c490[
                        g_acShipTarget_0059ce60[projectile]], &vector);
                range = (int)Vector_magnitude(&vector);
                ScaleFixedVector(&g_aShipVelocity_0059c010[
                    g_acShipTarget_0059ce60[projectile]],
                    range / get_ship_max_velocity(projectile), &vector);
                AddFixedVectors(&g_aShipPosition_0059c490[
                    g_acShipTarget_0059ce60[projectile]], &vector, &vector);
                point_at(projectile, vector);
            }
            break;
        case OBJECT_TYPE_HEAT_SEEKING_MISSILE:
        case OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE:
            g_acShipTarget_0059ce60[projectile] =
                g_acShipTarget_0059ce60[obj];
            break;
        case OBJECT_TYPE_FF_MISSILE:
            g_acShipTarget_0059ce60[projectile] = -1;
            g_asObjectCounter_0059c330[projectile] = 15;
            break;
        }
    }
    if (obj == 0) {
        if (weaponClass == OBJECT_CLASS_PROJECTILE) {
            g_asObjectCounter_0059c330[obj] =
                g_acGunRefireDelay_0046995c[
                    weaponType - OBJECT_TYPE_LASER_CANNON];
        }
    } else {
        g_asObjectCounter_0059c330[obj] = 12;
    }
    switch (weaponType) {
    case OBJECT_TYPE_LASER_CANNON:
    case OBJECT_TYPE_NEUTRON_PARTICLE_GUN:
        PlaySfxWaveFileByNumber(8, projectile, 0);
        break;
    case OBJECT_TYPE_MASS_DRIVER_CANNON:
    case OBJECT_TYPE_TURRET:
        PlaySfxWaveFileByNumber(5, projectile, 0);
        break;
    case OBJECT_TYPE_DUMB_FIRE_MISSILE:
    case OBJECT_TYPE_HEAT_SEEKING_MISSILE:
    case OBJECT_TYPE_FF_MISSILE:
    case OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE:
        PlaySfxWaveFileByNumber(1, projectile, 0);
        break;
    }
    return projectile;
}
