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
    enum ObjectType type;

    if (obj != 0 && g_aeSpecialManeuver_0059c3c0[obj] !=
                        SPECIAL_MANEUVER_UNKNOWN_9) {
        type = g_aeObjectType_0059b560[obj];
        if (skill_check(
                obj,
                (short)((short)(abs(g_anObjectRollRotation_0059d7e0[obj]) +
                                abs(g_anObjectYawRotation_0059ce80[obj]) +
                                abs(g_anObjectPitchRotation_0059b2a0[obj])) /
                        (short)(g_aObjectTypeData_00466458[type].rollRate +
                                g_aObjectTypeData_00466458[type].yawRate +
                                g_aObjectTypeData_00466458[type].pitchRate))) ==
            0) {
            set_special(obj, SPECIAL_MANEUVER_BLOWING_UP);
            g_asObjectCounter_0059c330[obj] =
                (short)(RandomBelowOrEqual(6) + 5);
        }
    }
    return 0;
}

/* Function start: 0x41E900 */
void send_appropriate_message(short attacker, short victim)
{
    if (g_aeObjectClass_0059d100[attacker] >= OBJECT_CLASS_SHIP) {
        if (g_nYourWingman_0046c04c != -1 &&
            (short)g_acObjectOwner_0059ce20[attacker] ==
                g_nYourWingman_0046c04c &&
            g_nYourWingman_0046c04c != attacker &&
            g_aeShipSide_0059d650[victim] == SIDE_KILRATHI) {
            if ((short)RandomBelowOrEqual(100) < 50 &&
                g_aeSpecialManeuver_0059c3c0[attacker] !=
                    SPECIAL_MANEUVER_UNKNOWN_9)
                send_message(g_nYourWingman_0046c04c, 5);
        } else if (g_aeShipSide_0059d650[
                       (short)g_acObjectOwner_0059ce20[attacker]] ==
                       SIDE_KILRATHI &&
                   g_nYourWingman_0046c04c == victim) {
            send_message((short)g_acObjectOwner_0059ce20[attacker], 5);
        }
    }
}

/* Function start: 0x41E9B0 */
int inflict_damage(short attacker, short victim, short damage,
                   const FixedVector *impactDirection)
{
    unsigned short quadrant;
    short destroyed;
    int sideDot;

    if (DAT_00469ffc == 0 && victim == 0)
        return 0;
    if (damage == 0 ||
        g_aeSpecialManeuver_0059c3c0[victim] ==
            SPECIAL_MANEUVER_UNKNOWN_9 ||
        g_aeObjectClass_0059d100[victim] < OBJECT_CLASS_MISSILE)
        return 0;

    if (g_aeObjectClass_0059d100[victim] < OBJECT_CLASS_SHIP) {
        g_asShipAccumulatedDamage_0059dee0[victim] = (short)(
            g_asShipAccumulatedDamage_0059dee0[victim] + damage);
        if (g_aObjectTypeData_00466458[
                g_aeObjectType_0059b560[victim]].damageCapacity == -1)
            return 0;
        if (g_aObjectTypeData_00466458[
                g_aeObjectType_0059b560[victim]].damageCapacity <=
            g_asShipAccumulatedDamage_0059dee0[victim])
            return explode(attacker, victim);
    } else {
        if (victim == 0)
            TriggerPlayerHitPaletteFlash();

        if (attacker != -1 &&
            g_nYourWingman_0046c04c == victim &&
            g_acObjectOwner_0059ce20[attacker] == 0)
            send_message(victim, 10);

        quadrant = (short)(dot_product(impactDirection,
            &g_aShipForwardVector_0059bce0[victim]) > 0);
        damage = (short)(damage -
                         g_aasShipShield_0059d5b0[victim][quadrant]);
        if (damage > 0) {
            g_aasShipShield_0059d5b0[victim][quadrant] = 0;
            if (attacker != -1 &&
                g_aeObjectClass_0059d100[attacker] ==
                    OBJECT_CLASS_PROJECTILE)
                PlaySfxWaveFileByNumber(9, victim, 0);

            sideDot = dot_product(impactDirection,
                                  &g_aShipRightVector_0059b6e0[victim]);
            if (sideDot > 0xb5)
                quadrant = 3;
            else if (sideDot < -0xb5)
                quadrant = 2;
            damage = (short)(damage -
                             g_aasShipArmor_0059d420[victim][quadrant]);
            if (damage > 0) {
                g_aasShipArmor_0059d420[victim][quadrant] = 0;

                if (g_asObjectScreenX_0059d9b0[victim] !=
                        (short)0x8001 &&
                    g_aeObjectClass_0059d100[victim] !=
                        OBJECT_CLASS_CAPITAL_SHIP &&
                    (short)RandomBelowOrEqual(1) == 0)
                    Create_ship_hit_debris(attacker, 1);
                if ((short)RandomBelowOrEqual(99) == 0) {
                    if (attacker != 0 &&
                        attacker != g_nYourWingman_0046c04c &&
                        g_aeObjectClass_0059d100[attacker] ==
                            OBJECT_CLASS_SHIP) {
                        if (g_aeShipSide_0059d650[attacker] ==
                            SIDE_KILRATHI)
                            send_message(attacker, 6);
                        destroyed = explode(attacker, victim);
                    }
                } else {
                    destroyed = internal_damage(attacker, victim, damage,
                                                quadrant);
                }
                if (destroyed == 1)
                    send_appropriate_message(attacker, victim);
                return destroyed;
            }
            g_aasShipArmor_0059d420[victim][quadrant] = (short)-damage;
        } else {
            g_aasShipShield_0059d5b0[victim][quadrant] = (short)-damage;
            if (attacker != -1 &&
                g_aeObjectClass_0059d100[attacker] ==
                    OBJECT_CLASS_PROJECTILE)
                PlaySfxWaveFileByNumber(10, victim, 0);
        }
    }
    return 0;
}

/* Function start: 0x41EC60 */
short pilot_hit(short obj)
{
    if (DAT_0059cf00[obj] > 0) {
        DAT_0059cf00[obj]--;
        if (DAT_0059cf00[obj] == 0) {
            if (obj == 0) {
                if (DAT_00469ffc != 0)
                    g_nArcadeState_00469fb0 = 4;
                return -1;
            }
        } else if (skill_check(obj, 9) == 0 && obj != 0) {
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
    int objectOffset;
    short debris;
    FixedVector offset;

    debris = find_vacant_3d_object();
    if (debris != -1) {
        set_objects_data(debris, OBJECT_TYPE_EXPLOSION2, obj);
        objectOffset = (int)obj * sizeof(FixedVector);
        g_asObjectScale_0059de40[debris] <<= 2;
        g_asObjectCounter_0059c330[debris] = 6;
        g_aShipVelocity_0059c010[debris] =
            *(FixedVector *)(void *)
                ((unsigned char *)g_aShipVelocity_0059c010 +
                 objectOffset);
        MakeRandomVectorFixed(
            (short)(g_asObjectCollisionRadius_0059d710[obj] >> 2),
            (short)(g_asObjectCollisionRadius_0059d710[obj] >> 1),
            &offset);
        AddFixedVectors((FixedVector *)(void *)
                            ((unsigned char *)g_aShipPosition_0059c490 +
                             objectOffset),
                        &offset,
                        &g_aShipPosition_0059c490[debris]);
    }
    return debris != -1;
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
            (short)RandomBelow(100) < 50)
            g_acShipTarget_0059ce60[other] = (signed char)obj;
        other++;
    } while (other < 10);
}

/* Function start: 0x41EE20 */
int internal_damage(short attacker, short victim, short damage,
                    short quadrant)
{
    enum ObjectType type;
    short damageCapacity;
    short events;
    short system;
    short weaponCount;

    if (victim == 0)
        return your_internal_damage(attacker, damage, quadrant);
    type = g_aeObjectType_0059b560[victim];
    damageCapacity = g_aObjectTypeData_00466458[type].damageCapacity;
    if (g_aeObjectClass_0059d100[victim] == OBJECT_CLASS_CAPITAL_SHIP) {
        if (g_aeShipSide_0059d650[victim] == SIDE_KILRATHI) {
            events = MaxShort(1, (short)(damage >> 3));
            g_asShipAccumulatedDamage_0059dee0[victim] = (short)(
                g_asShipAccumulatedDamage_0059dee0[victim] + events);
            if (attacker != -1 && attacker < 10 &&
                any_enemy(attacker, 10000) == 0)
                call_enemy(attacker);
        } else {
            events = MaxShort(1, (short)(damage / 10));
            g_asShipAccumulatedDamage_0059dee0[victim] = (short)(
                g_asShipAccumulatedDamage_0059dee0[victim] + events);
            if ((short)RandomBelowOrEqual(1000) < 35 && attacker != 0)
                send_message(victim, 4);
        }
        if (g_asShipAccumulatedDamage_0059dee0[victim] >=
            damageCapacity)
            return explode(attacker, victim);
        onboard_explosion(victim);
        return 0;
    }

    if (g_acShipRating_0059cd80[victim] != -1) {
        events = MaxShort(1, (short)(damage / 40));
        events = MinShort((short)RandomInRange(3, 4), events);
    } else {
        events = MaxShort(1, (short)(damage / 6));
    }
    g_asShipAccumulatedDamage_0059dee0[victim] = (short)(
        g_asShipAccumulatedDamage_0059dee0[victim] + events);

    while (events > 0) {
        if (events == 1 && g_acShipRating_0059cd80[victim] != -1)
            system = 4;
        else
            system = (short)RandomBelowOrEqual(9);
        switch (system) {
        case 0:
            events--;
            pilot_hit(victim);
            break;
        case 1:
            if (quadrant == 1) {
                events--;
                damage_ion_drive(victim, 1, 3);
            }
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
            events--;
            g_acShipDamage_0059c460[victim]++;
            if ((short)g_acShipDamage_0059c460[victim] >
                damageCapacity)
                return explode(attacker, victim);
            break;
        case 5:
            if (quadrant == 0) {
                weaponCount = (short)(signed char)
                    g_aShipWeapons_0059cab0[victim][0];
                if (weaponCount > 0) {
                    events--;
                    remove_weapon(victim,
                        (short)RandomBelowOrEqual(
                            (short)(weaponCount - 1)));
                }
            }
            break;
        case 6:
            if (quadrant == 0 &&
                g_acShipDestroyedWeaponCount_0059de30[victim] < 5) {
                events--;
                g_acShipDestroyedWeaponCount_0059de30[victim]++;
            }
            break;
        case 7:
            if (quadrant == 1) {
                events--;
                drain_fuel(victim,
                           (short)(*(int *)&g_aObjectTypeData_00466458[
                                       type].lifetime / 4));
                if ((short)RandomBelowOrEqual(1) != 0 ||
                    g_anShipFuel_0059b470[victim] < 0)
                    return explode(attacker, victim);
            }
            break;
        case 8:
            if (quadrant == 0 &&
                g_acShipCommunicator_0059c850[victim] != -1) {
                g_acShipCommunicator_0059c850[victim] = -1;
                events--;
            }
            break;
        }
    }
    return 0;
}

/* Function start: 0x41F1A0 */
void revise_shields(short obj)
{
    short maximum;

    maximum = MaxShort(0, (short)(g_aasShipMaximumShield_0059d6e0[obj][0] -
        (g_aObjectTypeData_00466458[
            g_aeObjectType_0059b560[obj]].shieldFore >> 2)));
    g_aasShipMaximumShield_0059d6e0[obj][0] = maximum;
    g_aasShipMaximumShield_0059d6e0[obj][1] =
        MaxShort(0, (short)(maximum -
            (g_aObjectTypeData_00466458[
                g_aeObjectType_0059b560[obj]].shieldAft >> 2)));
}

/* Function start: 0x41F220 */
int your_internal_damage(short attacker, short damage, short quadrant)
{
    enum ObjectClass attackerClass;
    enum ObjectType playerType;
    short tableGroup;
    short events;
    short system;
    signed char severity;
    signed char component;
    signed char amount;
    short weaponCount;

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
    playerType = g_aeObjectType_0059b560[0];
    severity = (signed char)RandomBelowOrEqual(10);
    g_asShipAccumulatedDamage_0059dee0[0] = (short)(
        g_asShipAccumulatedDamage_0059dee0[0] + events);
    if (events > 1)
        place_damage_on_cockpit((short)RandomBelowOrEqual(3));

    while (events > 0) {
        events--;
        system = *(const signed char *)(const void *)
            &g_asPlayerDamageSystemTable_00469878[
                tableGroup * 10 + (short)RandomBelowOrEqual(9)];
        switch (system) {
        case 0:
            if (severity < 4)
                pilot_hit(0);
            else {
                if (severity < 7) {
                    amount = 2;
                    component = 7;
                } else {
                    amount = 4;
                    component = 6;
                }
                goto damage_component;
            }
            break;
        case 1:
            if (quadrant == 1) {
                damage_your_component(0, 1, 3);
                damage_ion_drive(0, 1, 3);
            } else
                events++;
            break;
        case 2:
            if (quadrant == 1) {
                if ((short)RandomBelowOrEqual(3) == 0) {
                    return explode(attacker, 0);
                }
                if (damage_your_component(1, 1, 4) == 4)
                    return explode(attacker, 0);
            } else
                events++;
            break;
        case 3:
            if (severity > 8) {
                amount = 2;
                component = 8;
                goto damage_component;
            } else {
                damage_your_component(2, 1, 4);
                revise_shields(0);
            }
            break;
        case 4:
            g_acShipDamage_0059c460[0]++;
            if (g_acShipDamage_0059c460[0] == 1) {
                pilot_hit(0);
                if (events > 0)
                    events--;
            } else if ((short)g_acShipDamage_0059c460[0] >
                       g_aObjectTypeData_00466458[playerType].
                           damageCapacity) {
                return explode(attacker, 0);
            }
            break;
        case 5:
            if (quadrant != 0) {
                events++;
            } else {
                weaponCount = (short)(signed char)g_aShipWeapons_0059cab0[0][0];
                if (weaponCount > 0) {
                    remove_weapon(0,
                        (short)RandomBelowOrEqual(
                            (short)(weaponCount - 1)));
                    ShowComponentHitHudMessage(
                        (char *)g_szWeaponDestroyed_00469960,
                        DAT_004699ac, 8);
                }
            }
            break;
        case 6:
            if (quadrant != 0)
                events++;
            else if (g_acShipDestroyedWeaponCount_0059de30[0] < 5) {
                    g_acShipDestroyedWeaponCount_0059de30[0]++;
                    check_computer_damage();
                }
            break;
        case 7:
            drain_fuel(0,
                       (short)(*(int *)&g_aObjectTypeData_00466458[
                                   playerType].lifetime / 4));
            if ((short)RandomBelowOrEqual(1) != 0 ||
                g_anShipFuel_0059b470[0] < 0)
                return explode(attacker, 0);
            ShowComponentHitHudMessage((char *)g_szFuelTanksHit_00469974,
                                       DAT_004699ac, 8);
            break;
        case 8:
            if (quadrant != 0) {
                events++;
            } else {
                if (severity > 6) {
                    amount = 4;
                    component = 5;
                    goto damage_component;
                } else {
                    damage_your_component(4, 2, 3);
                    if (g_acPlayerComponentDamage_0059bff0[4] > 3)
                        g_acShipCommunicator_0059c850[0] = -1;
                }
            }
            break;
        }
        continue;
damage_component:
        damage_your_component(component, amount, 4);
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

    if (minimumDamage <
        (short)g_acPlayerComponentDamage_0059bff0[component]) {
        g_acPlayerComponentDamage_0059bff0[component]--;
        sprintf(message, g_szComponentFixedFormat_00469984,
                g_apszComponentNames_0046a778[component]);
        ShowComponentHitHudMessage(message, DAT_004699ac, 8);
        return 1;
    }
    return 0;
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
unsigned int Create_explosion_debris(short obj)
{
    FixedVector vector;
    short debris;
    short index;
    short set;

    remove_object(obj);
    index = 0;
    check_next_wave();
    set = RandomBelowOrEqual(3);
    do {
        debris = find_vacant_3d_object();
        if (debris == -1)
            break;
        set_objects_data(debris,
                         g_aaeExplosionDebris_004698e0[set][index++], -1);
        g_asObjectCounter_0059c330[debris] = 40;
        FillFixedVectorWithRandomComponents(50, &vector);
        AddFixedVectors(&g_aShipPosition_0059c490[obj], &vector,
                        &g_aShipPosition_0059c490[debris]);
        FillFixedVectorWithRandomComponents(
            25, &g_aShipVelocity_0059c010[debris]);
        divide_vector(&g_aShipVelocity_0059c010[obj], 0x200, &vector);
        AddFixedVectors(&vector,
                        &g_aShipVelocity_0059c010[debris],
                        &g_aShipVelocity_0059c010[debris]);
    } while (index < 7);
    index = 0;
    do {
        debris = find_vacant_3d_object();
        if (debris == -1)
            break;
        index++;
        FillFixedVectorWithRandomComponents(50, &vector);
        AddFixedVectors(&g_aShipPosition_0059c490[obj], &vector,
                        &g_aShipPosition_0059c490[debris]);
        FillFixedVectorWithRandomComponents(
            25, &g_aShipVelocity_0059c010[debris]);
        divide_vector(&g_aShipVelocity_0059c010[obj], 0x200, &vector);
        AddFixedVectors(&vector,
                        &g_aShipVelocity_0059c010[debris],
                        &g_aShipVelocity_0059c010[debris]);
        g_asObjectScreenAngle_0059cd90[debris] =
            (short)(RandomBelowOrEqual(3) + 0x10);
        g_asObjectCounter_0059c330[debris] = 40;
        g_aeObjectClass_0059d100[debris] = OBJECT_CLASS_DUST;
        g_aeObjectType_0059b560[debris] = OBJECT_TYPE_DEBRIS_DUST;
    } while (index < 8);
    return 0;
}

/* Function start: 0x41F9E0 */
unsigned int affect_mission_score(short pilot, int event, short amount)
{
    short score;

    switch (event) {
    case 0:
        score = amount;
        break;
    case 1:
        score = 7;
        break;
    case 2:
        score = 10;
        break;
    case 3:
    case 4:
        score = 15;
        break;
    case 5:
    case 6:
        score = 25;
        break;
    case 7:
        score = 50;
        break;
    case 8:
        score = 75;
        break;
    case 9:
    case 10:
    case 11:
        score = 25;
        break;
    case 12:
        score = (short)(amount * 2);
        break;
    default:
        score = amount;
        break;
    }
    g_stCampaignState_0059ca50.missionScore =
        (short)(g_stCampaignState_0059ca50.missionScore + score);
    if (pilot == 0) {
        g_nMissionMedalScore_005a8116 =
            (short)(g_nMissionMedalScore_005a8116 + score);
        g_nArcadeScore_005a7bc4 += score * 10;
    }
    return 0;
}

/* Function start: 0x41FA90 */
unsigned int score_for_kill(short pilot, short victim)
{
    int event;

    if (g_aeShipSide_0059d650[victim] == SIDE_KILRATHI) {
        switch (g_aeObjectType_0059b560[victim]) {
        case OBJECT_TYPE_SALTHI:
            event = 1;
            break;
        case OBJECT_TYPE_DRALTHI:
        case OBJECT_TYPE_KRANT:
            event = 2;
            break;
        case OBJECT_TYPE_GRATHA:
        case OBJECT_TYPE_JALTHI:
            event = 3;
            break;
        case OBJECT_TYPE_SPIKERI:
        case OBJECT_TYPE_RALARI:
            event = 6;
            break;
        case OBJECT_TYPE_DORKIR:
        case OBJECT_TYPE_LUMBARI:
            event = 4;
            break;
        case OBJECT_TYPE_FRALTHI:
        case OBJECT_TYPE_SNAKEIR:
            event = 7;
            break;
        case OBJECT_TYPE_SIVAR:
        case OBJECT_TYPE_KILRATHI_BASE:
            event = 8;
            break;
        }
        affect_mission_score(pilot, event, -1);
    }
    return 0;
}

/* Function start: 0x41FB40 */
unsigned int analyze_kill(short attacker, short victim)
{
    int enemy;

    enemy = g_aeShipSide_0059d650[victim] !=
            g_aeShipSide_0059d650[attacker];
    new_space_music_changes(attacker, victim);
    if (enemy) {
        send_message(attacker, 5);
        score_for_kill(attacker, victim);
        if (attacker == 0)
            g_nPlayerKillCount_005a7c9c++;
        else if (g_nYourWingman_0046c04c == attacker)
            g_nWingmanKillCount_005a7cb8++;
    }
    return 0;
}

/* Function start: 0x41FBC0 */
short ShipExplosion(short obj)
{
    unsigned short originalScale;
    short explosion;

    originalScale = (unsigned short)g_asObjectScale_0059de40[obj];
    explosion = find_vacant_3d_object();
    if (explosion == -1) {
        if (g_aeObjectClass_0059d100[obj] ==
            OBJECT_CLASS_CAPITAL_SHIP)
            FreePacketAndClear((int *)&g_apObjectShape_0059d2f0[obj], 0);
        g_asCapitalShipViewFrame_0059dd90[obj] = -1;
        explosion = obj;
    } else {
        copy_frame(obj, explosion);
        g_aShipPosition_0059c490[explosion] =
            g_aShipPosition_0059c490[obj];
        g_aShipVelocity_0059c010[explosion] =
            g_aShipVelocity_0059c010[obj];
        g_acObjectOwner_0059ce20[explosion] = (signed char)obj;
    }
    set_objects_data(explosion, OBJECT_TYPE_EXPLOSION1,
                     (short)g_acObjectOwner_0059ce20[explosion]);
    g_asObjectScale_0059de40[explosion] = (short)(
        (unsigned short)g_asObjectScale_0059de40[explosion] *
        originalScale >> 8);
    return explosion;
}

/* Function start: 0x41FCD0 */
short Explosion(short obj)
{
    enum ObjectClass objectClass;
    enum ObjectType explosionType;
    signed char rating;
    short missionShip;
    short explosion;

    objectClass = g_aeObjectClass_0059d100[obj];
    explosion = obj;
    if (objectClass >= OBJECT_CLASS_SHIP) {
        missionShip = -1;
        if (objectClass == OBJECT_CLASS_CAPITAL_SHIP)
            missionShip = find_ship_index(obj);
        if (obj < 10) {
            if (g_acShipRating_0059cd80[obj] == -1 &&
                (missionShip == -1 ||
                 g_anShipMissionShip_0059d4b0[obj] != missionShip) &&
                (short)RandomBelow(100) > 2)
                goto message_finished;
            send_message(obj, 7);
        }
message_finished:
        rating = g_acShipRating_0059cd80[obj];
        if (rating != -1 && rating != RATING_ACE_ICEMAN)
            personality_killed((short)rating);
        if (g_nYourWingman_0046c04c == obj) {
            missionShip = 0;
            g_nWingmanKilledThisMission_005a7cb4 = 1;
            do {
                if (g_acShipRating_0059cd80[missionShip] >
                    RATING_ACE_ICEMAN)
                    break;
                missionShip++;
            } while (missionShip < 10);
            if (missionShip < 10)
                send_message(missionShip, 5);
            g_nYourWingman_0046c04c = -1;
        }
        set_special(obj, SPECIAL_MANEUVER_UNKNOWN_9);
        g_asObjectCounter_0059c330[obj] = 8;
        g_aMissionShips_0046c948[
            g_nShipMissionIndices_0059c830[obj]].state = 3;
        if (g_aeObjectClass_0059d100[obj] ==
            OBJECT_CLASS_CAPITAL_SHIP) {
            short count = 4;

            do {
                onboard_explosion(obj);
                count--;
            } while (count != 0);
            g_asObjectCounter_0059c330[obj] =
                (short)(g_aObjectTypeData_00466458[
                    g_aeObjectType_0059b560[obj]].damageCapacity >> 2) + 8;
        } else {
            explosion = ShipExplosion(obj);
        }
    } else {
        explosionType = OBJECT_TYPE_EXPLOSION2;
        if (g_aeObjectType_0059b560[obj] == OBJECT_TYPE_TURRET ||
            objectClass == OBJECT_CLASS_ASTEROID)
            explosionType = OBJECT_TYPE_EXPLOSION0;
        set_objects_data(obj, explosionType,
                         g_acObjectOwner_0059ce20[obj]);
        if (objectClass == OBJECT_CLASS_ASTEROID)
            g_asObjectScale_0059de40[obj] = 0x380;
    }
    if (objectClass != OBJECT_CLASS_CAPITAL_SHIP)
        explosion_shock_wave(
            obj, g_aObjectTypeData_00466458[
                g_aeObjectType_0059b560[obj]].explosionDamage);
    if (g_asObjectScreenX_0059d9b0[obj] != (short)0x8001)
        PlaySfxWaveFileByNumber(4, obj, 0);
    return explosion;
}

/* Function start: 0x41FEB0 */
short the_creator(short obj)
{
    signed char owner;

    for (;;) {
        if (obj == -1)
            return -1;
        owner = g_acObjectOwner_0059ce20[obj];
        if (obj == owner || owner == -1)
            return obj;
        obj = owner;
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
            if (distance > 1000) {
                damage = 0;
            } else {
                if (distance > 750) {
                    divisor = 40;
                } else {
                    divisor = 30;
                    if (distance <= 500)
                        divisor = find_ratio(0, 500, distance, 8, 25);
                }
                damage = (short)((blastDamage / divisor) / divisor);
                if (other == 0)
                    damage = MaxShort(1,
                        (short)(damage * 3 >> 2));
            }
            if (damage > 1) {
                NormalizeFixedVector(&delta);
                ScaleFixedVector(&delta, (int)damage << 8, &force);
                apply_force_to_objects_center(&force, other);
                attacker = the_creator(obj);
                inflict_damage(attacker, other,
                               MinShort(100, damage), &delta);
            }
        }
        other++;
    } while (other < 10);
    return 0;
}

/* Function start: 0x420040 */
int explode(short attacker, short victim)
{
    short creator;

    if (victim < 10) {
        if (g_acShipRating_0059cd80[victim] != -1 &&
            g_acShipRating_0059cd80[victim] != RATING_ACE_ICEMAN) {
            if (g_acShipRating_0059cd80[victim] > RATING_ACE_ICEMAN) {
                if ((short)ace_status(
                        (short)(g_acShipRating_0059cd80[victim] -
                                RATING_ACE_ANGEL),
                                      0x20) != 0) {
                    unflag_ace(
                        (short)(g_acShipRating_0059cd80[victim] -
                                RATING_ACE_ANGEL),
                        0x20);
                    g_acShipStress_0059d620[victim] = -25;
                    reset_maneuver(victim, MANEUVER_OUTA_HERE);
                    g_acShipDamage_0059c460[victim] =
                        (signed char)(g_acShipDamage_0059c460[victim] / 2);
                    send_message(victim, 6);
                    return 0;
                }
                if ((short)RandomBelowOrEqual(1) == 0)
                    return 0;
            } else if ((short)RandomBelowOrEqual(1) == 0)
                return 0;
        }
    }
    if (g_aeSpecialManeuver_0059c3c0[victim] ==
            SPECIAL_MANEUVER_UNKNOWN_9 &&
        g_aeObjectClass_0059d100[victim] >= OBJECT_CLASS_SHIP)
        return 0;

    if (victim == 0) {
        if (DAT_00469ffc == 0)
            return 0;
        g_bPlayerDestroyed_005a7c98 = 1;
        g_nArcadeState_00469fb0 = 4;
        return 1;
    }

    if (g_nExternalViewShip_0046c040 == victim)
        g_nExternalViewShip_0046c040 = -1;
    creator = the_creator(attacker);
    if (creator != -1 &&
        g_aeObjectClass_0059d100[victim] >= OBJECT_CLASS_SHIP)
        analyze_kill(creator, victim);
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
                    OBJECT_TYPE_TURRET].explosionDamage);
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
    ObjectTypeData *weaponData;
    enum ObjectType weaponType;
    enum ObjectClass weaponClass;
    FixedVector vector;
    FixedVector cockpitOffset;
    int weaponOffset;
    int range;
    int sound;
    short projectileSpeed;
    short projectile;

    projectileSpeed = 10;
    weaponOffset = (int)obj * sizeof(g_aShipWeapons_0059cab0[0]) +
                   (int)weapon * sizeof(ShipWeaponSlot);
    weaponType = *(enum ObjectType *)(void *)
        ((unsigned char *)g_aShipWeapons_0059cab0 + weaponOffset + 1);
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
    if (projectile != -1) {
        weaponData = &g_aObjectTypeData_00466458[weaponType];
        copy_frame(obj, projectile);
        if (weaponClass == OBJECT_CLASS_PROJECTILE) {
            g_asShipAccumulatedDamage_0059dee0[projectile] =
                weaponData->damageCapacity;
            projectileSpeed = g_aObjectTypeData_00466458[
                g_aeObjectType_0059b560[projectile]].maximumVelocity;
            g_asShipWeaponEnergy_0059d470[obj] =
                (short)(g_asShipWeaponEnergy_0059d470[obj] -
                        weaponData->animationDelay);
        }
        child_object(*(short *)(void *)
                         ((unsigned char *)g_aShipWeapons_0059cab0 +
                          weaponOffset + 5),
                     projectile, obj);
        g_asObjectCounter_0059c330[projectile] =
            g_aObjectTypeData_00466458[weaponType].lifetime;
        vector_component_in_dir(&g_aShipVelocity_0059c010[obj],
                                &g_aShipForwardVector_0059bce0[projectile],
                                &g_aShipVelocity_0059c010[projectile]);
        if (weaponClass == OBJECT_CLASS_PROJECTILE) {
            ScaleFixedVector(&g_aShipForwardVector_0059bce0[obj],
                (int)(short)((weaponData->lifetime + 5) *
                             weaponData->maximumVelocity) << 8,
                &vector);
            AddFixedVectors(&g_aShipPosition_0059c490[obj], &vector,
                            &vector);
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
            ScaleFixedVector(&g_aShipUpVector_0059b9e0[obj], 0xa00,
                             &vector);
            AddFixedVectors(&vector, &g_aShipVelocity_0059c010[projectile],
                            &g_aShipVelocity_0059c010[projectile]);
            if (obj == 0)
                RemovePlayerReleaseWeapon((signed char)weapon);
            else
                remove_weapon(obj, weapon);
            g_acObjectCollisionGraceTicks_0059ddb0[projectile] = 20;
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
                        g_acShipTarget_0059ce60[projectile]], &vector,
                        &vector);
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
            sound = 8;
            break;
        case OBJECT_TYPE_MASS_DRIVER_CANNON:
        case OBJECT_TYPE_TURRET:
            sound = 5;
            break;
        case OBJECT_TYPE_DUMB_FIRE_MISSILE:
        case OBJECT_TYPE_HEAT_SEEKING_MISSILE:
        case OBJECT_TYPE_FF_MISSILE:
        case OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE:
            sound = 1;
            break;
        default:
            return projectile;
        }
        PlaySfxWaveFileByNumber(sound, projectile, 0);
    }
    return projectile;
}
