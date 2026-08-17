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

/* Function start: WC2_UNMAPPED */
unsigned int check_for_lost_control(short obj)
{
    if (obj != 0 && g_aeSpecialManeuver_00495600[obj] !=
                        SPECIAL_MANEUVER_UNKNOWN_9) {
        if (skill_check(obj) == 0) {
            set_special(obj, SPECIAL_MANEUVER_BLOWING_UP);
            g_asObjectCounter_00494be0[obj] =
                (short)(RandomBelowOrEqual(6) + 5);
        }
    }
    return 0;
}

/* Function start: 0x411A50 */
void send_appropriate_message(short attacker, short victim)
{
    if (g_aeObjectClass_00495328[attacker] >= OBJECT_CLASS_SHIP) {
        if (g_nYourWingman_0049346c != -1 &&
            (short)g_acObjectOwner_00495208[attacker] ==
                g_nYourWingman_0049346c &&
            g_nYourWingman_0049346c != attacker &&
            g_asShipSide_004955d0[victim] == SIDE_KILRATHI) {
            if (RandomBelowOrEqual(100) < 50 &&
                g_aeSpecialManeuver_00495600[attacker] !=
                    SPECIAL_MANEUVER_UNKNOWN_9)
                send_message(g_nYourWingman_0049346c, 5);
#ifdef WC1_SDL
        /* An unowned ship has owner -1.  The original indexes side[-1], which
           aliases the final roll-goal words and normally compares false. */
        } else if (g_acObjectOwner_00495208[attacker] != -1 &&
#else
        } else if (
#endif
                   g_asShipSide_004955d0[
                       (short)g_acObjectOwner_00495208[attacker]] ==
                       SIDE_KILRATHI &&
                   g_nYourWingman_0049346c == victim) {
            send_message((short)g_acObjectOwner_00495208[attacker], 5);
        }
    }
}

/* Function start: 0x411C72 */
int inflict_damage(short attacker, short victim, short damage,
                   const FixedVector *impactDirection)
{
    unsigned short quadrant;
    short destroyed;
    int sideDot;

    if (DAT_00469ffc == 0 && victim == 0)
        return 0;
    if (damage == 0 ||
        g_aeSpecialManeuver_00495600[victim] ==
            SPECIAL_MANEUVER_UNKNOWN_9 ||
        g_aeObjectClass_00495328[victim] < OBJECT_CLASS_MISSILE)
        return 0;

    if (g_aeObjectClass_00495328[victim] < OBJECT_CLASS_SHIP) {
        g_asShipAccumulatedDamage_0059dee0[victim] = (short)(
            g_asShipAccumulatedDamage_0059dee0[victim] + damage);
        if (g_aObjectTypeData_00496d30[
                g_acObjectType_00493980[victim]].damageCapacity == -1)
            return 0;
        if (g_aObjectTypeData_00496d30[
                g_acObjectType_00493980[victim]].damageCapacity <=
            g_asShipAccumulatedDamage_0059dee0[victim])
            return explode(attacker, victim);
    } else {
        if (victim == 0) {
            TriggerPlayerHitPaletteFlash();
#ifdef WC1_SDL
            Wc1SdlQueueJoystickDamageRumble(damage);
#endif
        }

        if (attacker != -1 &&
            g_nYourWingman_0049346c == victim &&
            g_acObjectOwner_00495208[attacker] == 0)
            send_message(victim, 10);

        quadrant = (short)(dot_product(impactDirection,
            &g_aShipForwardVector_00494208[victim]) > 0);
        damage = (short)(damage -
                         g_aasShipShield_00495518[victim][quadrant]);
        if (damage > 0) {
            g_aasShipShield_00495518[victim][quadrant] = 0;
            if (attacker != -1 &&
                g_aeObjectClass_00495328[attacker] ==
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

                if (g_asObjectScreenX_00493598[victim] !=
                        (short)0x8001 &&
                    g_aeObjectClass_00495328[victim] !=
                        OBJECT_CLASS_CAPITAL_SHIP &&
                    RandomBelowOrEqual(1) == 0)
                    Create_ship_hit_debris(attacker, 1);
                if (RandomBelowOrEqual(99) == 0) {
                    if (attacker != 0 &&
                        attacker != g_nYourWingman_0049346c &&
                        g_aeObjectClass_00495328[attacker] ==
                            OBJECT_CLASS_SHIP) {
                        if (g_asShipSide_004955d0[attacker] ==
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
            g_aasShipShield_00495518[victim][quadrant] = (short)-damage;
            if (attacker != -1 &&
                g_aeObjectClass_00495328[attacker] ==
                    OBJECT_CLASS_PROJECTILE)
                PlaySfxWaveFileByNumber(10, victim, 0);
        }
    }
    return 0;
}

/* Function start: 0x41262D */
short pilot_hit(short obj)
{
    if (DAT_0059cf00[obj] > 0) {
        DAT_0059cf00[obj]--;
        if (DAT_0059cf00[obj] == 0) {
            if (obj == 0) {
                if (DAT_00469ffc != 0)
                    g_nArcadeState_0049d75c = 4;
                return -1;
            }
        } else if (skill_check(obj) == 0 && obj != 0) {
            g_asObjectCounter_00494be0[obj] =
                (short)(RandomBelowOrEqual(20) + 30);
            set_special(obj, SPECIAL_MANEUVER_BLOWING_UP);
        }
    }
    return DAT_0059cf00[obj];
}

/* Function start: 0x41270A */
short onboard_explosion(short obj)
{
#if 0
    int objectOffset;
    short debris;
    FixedVector offset;

    debris = find_vacant_3d_object();
    if (debris != -1) {
        set_objects_data(debris, OBJECT_TYPE_EXPLOSION2, obj);
        objectOffset = (int)obj * sizeof(FixedVector);
        g_asObjectScale_0059de40[debris] <<= 2;
        g_asObjectCounter_00494be0[debris] = 6;
        g_aShipVelocity_0059c010[debris] =
            *(FixedVector *)(void *)
                ((unsigned char *)g_aShipVelocity_0059c010 +
                 objectOffset);
        MakeRandomVectorFixed(
            (short)(g_asObjectCollisionRadius_0059d710[obj] >> 2),
            (short)(g_asObjectCollisionRadius_0059d710[obj] >> 1),
            &offset);
        AddFixedVectors((FixedVector *)(void *)
                            ((unsigned char *)g_aShipPosition_00494550 +
                             objectOffset),
                        &offset,
                        &g_aShipPosition_00494550[debris]);
    }
    return debris != -1;
#else
    short debris;
    FixedVector offset;

    debris = find_vacant_3d_object();
    if (debris != -1) {
        ((void (__cdecl *)(short, enum ObjectType, short, short))
            set_objects_data)(debris, (enum ObjectType)0x26, obj, 0);
        g_asObjectScale_0059de40[debris] <<= 1;
        g_aShipVelocity_0059c010[debris] =
            g_aShipVelocity_0059c010[obj];
        g_asObjectCounter_00494be0[debris] = 6;
        MakeRandomVectorFixed(
            (short)(g_asObjectCollisionRadius_0059d710[obj] >> 2),
            (short)(g_asObjectCollisionRadius_0059d710[obj] >> 1),
            &offset);
        AddFixedVectors(&g_aShipPosition_00494550[obj], &offset,
                        &g_aShipPosition_00494550[debris]);
    }
    return debris != -1;
#endif
}

/* Function start: 0x41280B */
void call_enemy(short obj)
{
    short other;

    other = 0;
    for (; other < 10; other++) {
        if (g_aeObjectClass_00495328[other] >= OBJECT_CLASS_SHIP &&
            g_aeSpecialManeuver_00495600[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            g_asShipSide_004955d0[obj] != g_asShipSide_004955d0[other] &&
            RandomBelow(100) < 50)
            g_acShipTarget_00495f20[other] = obj;
    }
}

/* Function start: 0x4128A7 */
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
    type = g_acObjectType_00493980[victim];
    damageCapacity = g_aObjectTypeData_00496d30[type].damageCapacity;
    if (g_aeObjectClass_00495328[victim] == OBJECT_CLASS_CAPITAL_SHIP) {
        if (g_asShipSide_004955d0[victim] == SIDE_KILRATHI) {
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
            if (RandomBelowOrEqual(1000) < 35 && attacker != 0)
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
        events = MinShort(RandomInRange(3, 4), events);
    } else {
        events = MaxShort(1, (short)(damage / 6));
    }
    g_asShipAccumulatedDamage_0059dee0[victim] = (short)(
        g_asShipAccumulatedDamage_0059dee0[victim] + events);

    while (events > 0) {
        if (events == 1 && g_acShipRating_0059cd80[victim] != -1)
            system = 4;
        else
            system = RandomBelowOrEqual(9);
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
            g_aasShipShield_00495518[victim][0] = 0;
            g_aasShipShield_00495518[victim][1] = 0;
            g_aasShipMaximumShield_004954f0[victim][0] = 0;
            g_aasShipMaximumShield_004954f0[victim][1] = 0;
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
                        RandomBelowOrEqual(
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
                           (short)(*(int *)&g_aObjectTypeData_00496d30[
                                       type].lifetime / 4));
                if (RandomBelowOrEqual(1) != 0 ||
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

/* Function start: 0x412F03 */
void revise_shields(short obj)
{
    short maximum;

    g_aasShipMaximumShield_004954f0[obj][0] =
        MaxShort(0, (short)(g_aasShipMaximumShield_004954f0[obj][0] -
            (g_aObjectTypeData_00496d30[
                g_acObjectType_00493980[obj]].shieldFore >> 2)));
    maximum = g_aasShipMaximumShield_004954f0[obj][0];
    g_aasShipMaximumShield_004954f0[obj][1] =
        MaxShort(0, (short)(maximum -
            (g_aObjectTypeData_00496d30[
                g_acObjectType_00493980[obj]].shieldAft >> 2)));
}

/* Function start: 0x413099 */
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

    attackerClass = g_aeObjectClass_00495328[attacker];
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
    playerType = g_acObjectType_00493980[0];
    severity = (signed char)RandomBelowOrEqual(10);
    g_asShipAccumulatedDamage_0059dee0[0] = (short)(
        g_asShipAccumulatedDamage_0059dee0[0] + events);
    if (events > 1)
        place_damage_on_cockpit(RandomBelowOrEqual(3));

    while (events > 0) {
        events--;
        system = *(const signed char *)(const void *)
            &g_asPlayerDamageSystemTable_00469878[
                tableGroup * 10 + RandomBelowOrEqual(9)];
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
                if (RandomBelowOrEqual(3) == 0) {
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
                       g_aObjectTypeData_00496d30[playerType].
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
                        RandomBelowOrEqual(
                            (short)(weaponCount - 1)));
                    ShowComponentHitHudMessage(
                        g_szWeaponDestroyed_00469960,
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
                       (short)(*(int *)&g_aObjectTypeData_00496d30[
                                   playerType].lifetime / 4));
            if (RandomBelowOrEqual(1) != 0 ||
                g_anShipFuel_0059b470[0] < 0)
                return explode(attacker, 0);
            ShowComponentHitHudMessage(g_szFuelTanksHit_00469974,
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
                    if (g_acPlayerComponentDamage_00493470[4] > 3)
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

/* Function start: 0x412F90 */
unsigned int check_computer_damage(void)
{
    damage_your_component(3, 1, 3);
    return 0;
}

/* Function start: 0x413578 */
short ReportComponentRepaired(short component, short minimumDamage)
{
    char message[80];

    if (minimumDamage <
        (short)g_acPlayerComponentDamage_00493470[component]) {
        g_acPlayerComponentDamage_00493470[component]--;
        sprintf(message, g_szComponentFixedFormat_00469984,
                g_apszComponentNames_0046a778[component]);
        ShowComponentHitHudMessage(message, DAT_004699ac, 8);
        return 1;
    }
    return 0;
}

/* Function start: 0x4135F1 */
void repair_internal_damage(void)
{
#if 0
    short repair;
    short component;

    if ((short)g_acPlayerComponentDamage_00493470[component] >= 4)
        return;
#else
    int repair;
#endif
    if (RandomBelowOrEqual(500) >= 2)
        return;
    repair = RandomBelowOrEqual(2);
    switch (repair) {
    case 0:
#if 0
        /* The Mac body guards the selected component in each repair case.
           Retail Win32 instead reads an uninitialized component above. */
#endif
        if (g_acPlayerComponentDamage_00493470[2] >= 4)
            break;
        ReportComponentRepaired(2, 1);
        break;
    case 1:
        if (g_acPlayerComponentDamage_00493470[0] >= 4)
            break;
        if (ReportComponentRepaired(0, 2) != 0)
            damage_ion_drive(0, -1, 3);
        break;
    case 2:
        if (g_aObjectTypeData_00496d30[
                g_acObjectType_00493980[0]].damageCapacity - 3 <
            (short)g_acShipDamage_0059c460[0])
            g_acShipDamage_0059c460[0]--;
        break;
    }
}

/* Function start: 0x4136E2 */
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
                RandomBelowOrEqual(2)], -1);
        g_asObjectCounter_00494be0[debris] = 40;
        FillFixedVectorWithRandomComponents(10, &offset);
        AddFixedVectors(&g_aShipPosition_00494550[obj], &offset,
                        &g_aShipPosition_00494550[debris]);
        FillFixedVectorWithRandomComponents(
            6, &g_aShipVelocity_0059c010[debris]);
        created++;
    }
}

/* Function start: 0x4137C2 */
void check_next_wave(void)
{
#if 0
    short obj;

    if (g_nCurrentWave_0046c01c == -1)
        return;
    obj = 0;
    do {
        if (g_aeObjectClass_00495328[obj] == OBJECT_CLASS_SHIP &&
            g_asShipSide_004955d0[obj] == SIDE_KILRATHI)
            return;
        obj++;
    } while (obj < 10);
    set_up_next_wave();
#else
    {
        short obj;

        for (obj = 0; obj < 10; obj++) {
            if (g_aeObjectClass_00495328[obj] >= OBJECT_CLASS_SHIP &&
                g_asShipSide_004955d0[obj] == SIDE_KILRATHI)
                return;
        }
    }
    if (g_nEscapedEnemyCount_004962e8 != 0)
        return;
    {
        MissionObjective *objectiveData;
        short objective;

        for (objective = 0;
             objective < (short)g_cMissionObjectiveCount_00493294;
             objective++) {
            objectiveData = &g_aMissionObjectives_004932a8[objective];
            if (objectiveData->type == 5 &&
                objectiveData->index == g_nCurrentNavPoint_004931bc) {
                flag_objective(objective, 2);
                return;
            }
        }
    }
#endif
}

/* Function start: 0x413A3B */
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
    for (; index < 7; index++) {
        debris = find_vacant_3d_object();
        if (debris == -1)
            break;
        set_objects_data(debris,
                         g_aaeExplosionDebris_004698e0[set][index], -1);
        g_asObjectCounter_00494be0[debris] = 40;
        FillFixedVectorWithRandomComponents(50, &vector);
        AddFixedVectors(&g_aShipPosition_00494550[obj], &vector,
                        &g_aShipPosition_00494550[debris]);
        FillFixedVectorWithRandomComponents(
            25, &g_aShipVelocity_0059c010[debris]);
        divide_vector(&g_aShipVelocity_0059c010[obj], 0x200, &vector);
        AddFixedVectors(&vector,
                        &g_aShipVelocity_0059c010[debris],
                        &g_aShipVelocity_0059c010[debris]);
    }
    index = 0;
    for (; index < 8; index++) {
        debris = find_vacant_3d_object();
        if (debris == -1)
            break;
        FillFixedVectorWithRandomComponents(50, &vector);
        AddFixedVectors(&g_aShipPosition_00494550[obj], &vector,
                        &g_aShipPosition_00494550[debris]);
        FillFixedVectorWithRandomComponents(
            25, &g_aShipVelocity_0059c010[debris]);
        divide_vector(&g_aShipVelocity_0059c010[obj], 0x200, &vector);
        AddFixedVectors(&vector,
                        &g_aShipVelocity_0059c010[debris],
                        &g_aShipVelocity_0059c010[debris]);
        g_asObjectScreenAngle_0059cd90[debris] =
            (short)(RandomBelowOrEqual(3) + 0x10);
        g_asObjectCounter_00494be0[debris] = 40;
        g_aeObjectClass_00495328[debris] = OBJECT_CLASS_DUST;
        g_acObjectType_00493980[debris] = OBJECT_TYPE_DEBRIS_DUST;
    }
    return 0;
}

/* Function start: 0x413D61 */
void affect_mission_score(short pilot, int event, short amount)
{
#if 0
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
    g_nMissionScore_00493462 =
        (short)(g_nMissionScore_00493462 + score);
    if (pilot == 0) {
        g_nMissionMedalScore_005a8116 =
            (short)(g_nMissionMedalScore_005a8116 + score);
        g_nArcadeScore_005a7bc4 += score * 10;
    }
    return 0;
#else
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
        score = 25;
        break;
    case 6:
        score = 50;
        break;
    case 7:
        score = 75;
        break;
    case 8:
    case 9:
    case 10:
        score = 25;
        break;
    case 11:
        score = (short)(amount * 2);
        break;
    default:
        score = amount;
        break;
    }
    g_nMissionScore_00493462 += score;
#endif
}

/* Function start: 0x413E4C */
unsigned int score_for_kill(short pilot, short victim)
{
    int event;

    if (g_asShipSide_004955d0[victim] == SIDE_KILRATHI) {
        switch (g_acObjectType_00493980[victim]) {
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

/* Function start: 0x413F58 */
void analyze_kill(short attacker, short victim)
{
#if 0
    int enemy;

    enemy = g_asShipSide_004955d0[victim] !=
            g_asShipSide_004955d0[attacker];
    new_space_music_changes(attacker, victim);
    if (enemy) {
        send_message(attacker, 5);
        score_for_kill(attacker, victim);
        if (attacker == 0)
            g_cPlayerKillCount_005d2fa8++;
        else if (g_nYourWingman_0049346c == attacker)
            g_nWingmanKillCount_005a7cb8++;
    }
    return 0;
#else
    short enemy;
    short initialShip;

    enemy = g_asShipSide_004955d0[attacker] !=
            g_asShipSide_004955d0[victim];
    new_space_music_changes(attacker, victim);
    if (enemy != 0) {
        score_for_kill(attacker, victim);
        if (attacker == 0) {
            g_cPlayerKillCount_005d2fa8++;
        } else {
            initialShip = 0;
            while (initialShip < 8) {
                if (g_stMissionHeader_005d3e70
                        .initialMissionShips[initialShip] ==
                    g_asShipMissionIndex_00495d00[attacker])
                    g_acInitialShipKillCount_005d2fc0[initialShip]++;
                initialShip++;
            }
        }
    }
#endif
}

/* Function start: 0x4142C8 */
short ShipExplosion(short obj)
{
    unsigned short originalScale;
    short explosion;

    originalScale = (unsigned short)g_asObjectScale_0059de40[obj];
    explosion = find_vacant_3d_object();
    if (explosion == -1) {
        if (g_aeObjectClass_00495328[obj] ==
            OBJECT_CLASS_CAPITAL_SHIP)
            FreePacketAndClear(&g_apObjectShape_0059d2f0[obj], 0);
        g_asCapitalShipViewFrame_0059dd90[obj] = -1;
        explosion = obj;
    } else {
        copy_frame(obj, explosion);
        g_aShipPosition_00494550[explosion] =
            g_aShipPosition_00494550[obj];
        g_aShipVelocity_0059c010[explosion] =
            g_aShipVelocity_0059c010[obj];
        g_acObjectOwner_00495208[explosion] = (signed char)obj;
    }
    set_objects_data(explosion, OBJECT_TYPE_EXPLOSION1,
                     (short)g_acObjectOwner_00495208[explosion]);
    g_asObjectScale_0059de40[explosion] = (short)(
        (unsigned short)g_asObjectScale_0059de40[explosion] *
        originalScale >> 8);
    return explosion;
}

/* Function start: 0x41444C */
short Explosion(short obj)
{
    enum ObjectClass objectClass;
    enum ObjectType explosionType;
    signed char rating;
    short missionShip;
    short explosion;

    objectClass = g_aeObjectClass_00495328[obj];
    explosion = obj;
    if (objectClass >= OBJECT_CLASS_SHIP) {
        missionShip = -1;
        if (objectClass == OBJECT_CLASS_CAPITAL_SHIP)
            missionShip = find_ship_index(obj);
        if (obj < 10 &&
            (g_acShipRating_0059cd80[obj] != -1 ||
             (missionShip != -1 &&
              g_anShipMissionShip_00495e00[obj] == missionShip) ||
             RandomBelow(100) <= 2)) {
            send_message(obj, 7);
        }
        rating = g_acShipRating_0059cd80[obj];
        if (rating != -1 && rating != RATING_ACE_ICEMAN)
            personality_killed((short)rating);
        if (g_nYourWingman_0049346c == obj) {
            missionShip = 0;
            g_bWingmanKilledThisMission_005d2fb8 = 1;
            for (; missionShip < 10; missionShip++) {
                if (g_acShipRating_0059cd80[missionShip] >
                    RATING_ACE_ICEMAN)
                    break;
            }
            if (missionShip < 10)
                send_message(missionShip, 5);
            g_nYourWingman_0049346c = -1;
        }
        set_special(obj, SPECIAL_MANEUVER_UNKNOWN_9);
        g_asObjectCounter_00494be0[obj] = 8;
        g_aMissionShips_00492290[
            g_nShipMissionIndices_0059c830[obj]].state = 3;
        if (g_aeObjectClass_00495328[obj] ==
            OBJECT_CLASS_CAPITAL_SHIP) {
            short count = 4;

            for (; count != 0; count--) {
                onboard_explosion(obj);
            }
            g_asObjectCounter_00494be0[obj] =
                (short)(g_aObjectTypeData_00496d30[
                    g_acObjectType_00493980[obj]].damageCapacity >> 2) + 8;
        } else {
            explosion = ShipExplosion(obj);
        }
    } else {
        explosionType = OBJECT_TYPE_EXPLOSION2;
        if (g_acObjectType_00493980[obj] == OBJECT_TYPE_TURRET ||
            objectClass == OBJECT_CLASS_ASTEROID)
            explosionType = OBJECT_TYPE_EXPLOSION0;
        set_objects_data(obj, explosionType,
                         g_acObjectOwner_00495208[obj]);
        if (g_aeObjectClass_00495328[obj] == OBJECT_CLASS_ASTEROID)
            g_asObjectScale_0059de40[obj] = 0x380;
    }
    if (objectClass != OBJECT_CLASS_CAPITAL_SHIP)
        explosion_shock_wave(
            obj, g_aObjectTypeData_00496d30[
                g_acObjectType_00493980[obj]].explosionDamage);
    if (g_asObjectScreenX_00493598[obj] != (short)0x8001)
        PlaySfxWaveFileByNumber(4, obj, 0);
    return explosion;
}

/* Function start: WC2_UNMAPPED */
short the_creator(short obj)
{
    signed char owner;

    for (;;) {
        if (obj == -1)
            return -1;
        owner = g_acObjectOwner_00495208[obj];
        if (obj == owner || owner == -1)
            return obj;
        obj = owner;
    }
    return -1;
}

/* Function start: 0x414025 */
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
    for (; other < 10; other++) {
        if (other != obj &&
            g_aeObjectClass_00495328[other] >= OBJECT_CLASS_SHIP) {
            ComputeVectorDelta(&g_aShipPosition_00494550[obj],
                               &g_aShipPosition_00494550[other], &delta);
            distance = FixedToShortSaturating(
                Vector_magnitude(&delta));
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
    }
    return 0;
}

/* Function start: 0x414835 */
int explode(short attacker, short victim)
{
    short creator;

    if (victim < 10) {
        if (g_acShipRating_0059cd80[victim] != -1 &&
            g_acShipRating_0059cd80[victim] != RATING_ACE_ICEMAN) {
            if (g_acShipRating_0059cd80[victim] > RATING_ACE_ICEMAN) {
                if (ace_status(
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
                if (RandomBelowOrEqual(1) == 0)
                    return 0;
            } else if (RandomBelowOrEqual(1) == 0)
                return 0;
        }
    }
#ifdef WC1_SDL
    /* Special-maneuver state exists only for the twelve ship slots.  The
       original evaluates this lookup before rejecting non-ship objects, so
       exploding a projectile in a later object slot reads the following
       ship-mission table. */
    if (victim >= 0 && victim < 12 &&
        g_aeSpecialManeuver_00495600[victim] ==
            SPECIAL_MANEUVER_UNKNOWN_9 &&
        g_aeObjectClass_00495328[victim] >= OBJECT_CLASS_SHIP)
#else
    if (g_aeSpecialManeuver_00495600[victim] ==
            SPECIAL_MANEUVER_UNKNOWN_9 &&
        g_aeObjectClass_00495328[victim] >= OBJECT_CLASS_SHIP)
#endif
        return 0;

    if (victim == 0) {
        if (DAT_00469ffc == 0)
            return 0;
        g_bPlayerDestroyed_005d2fa4 = 1;
        g_nArcadeState_0049d75c = 4;
        return 1;
    }

    if (g_nExternalViewShip_00493468 == victim)
        g_nExternalViewShip_00493468 = -1;
    creator = the_creator(attacker);
    if (creator != -1 &&
        g_aeObjectClass_00495328[victim] >= OBJECT_CLASS_SHIP)
        analyze_kill(creator, victim);
    Explosion(victim);
    return 1;
}

/* Function start: 0x414BE9 */
int send_at_point(short obj, FixedVector *point, short speed)
{
    ComputeVectorDelta(&g_aShipPosition_00494550[obj], point,
                       &g_aShipVelocity_0059c010[obj]);
    SetVectorFixedPoint((unsigned int *)&g_aShipVelocity_0059c010[obj],
                        speed);
    return 0;
}

/* Function start: 0x414C39 */
short find_child_object(short parent, short objectClass)
{
    short obj;

    for (obj = 0; obj < WC2_SPACE_OBJECT_COUNT; obj++) {
        if ((short)g_acObjectOwner_00495208[obj] == parent &&
            g_aeObjectClass_00495328[obj] == objectClass)
            return obj;
    }
    return -1;
}

/* Function start: 0x410161 */
short IsCapitalShipObject(short obj)
{
    if (g_aeObjectClass_00495328[obj] >= OBJECT_CLASS_CAPITAL_SHIP)
        return 1;
    return 0;
}

/* Function start: 0x410289 */
short GetAdaptiveTurnRate(void)
{
    short turnRate;

    turnRate = g_aasSeriesMissionTurnRate_004930a0[
        g_nCurrentSeries_005c5870][g_nCurrentMission_005c5878];
    turnRate = (short)((int)turnRate * g_nAdaptiveDifficulty_005d3844 / 5);
    if (turnRate == 0)
        turnRate = 1;
    if (turnRate > 200)
        turnRate = 200;
    return turnRate;
}

/* Function start: 0x414CA9 */
short find_child_ship(short parent, short objectClass,
                      short target)
{
    short obj;

    for (obj = 0; obj < 10; obj++) {
        if ((short)g_acObjectOwner_00495208[obj] == parent &&
            g_aeObjectClass_00495328[obj] == objectClass &&
            (target == -1 ||
             g_acShipTarget_00495f20[obj] == target))
            return obj;
    }
    return -1;
}

/* Function start: 0x414D3D */
int launch_object(short parent, short child, FixedVector direction,
                  short speed)
{
    NormalizeFixedVector(&direction);
    vector_component_in_dir(&g_aShipVelocity_0059c010[parent],
                            &direction,
                            &g_aShipVelocity_0059c010[child]);
    ScaleFixedVector(&direction, (int)speed << 8, &direction);
    AddFixedVectors(&direction, &g_aShipVelocity_0059c010[child],
                    &g_aShipVelocity_0059c010[child]);
    return 0;
}

/* Function start: 0x414DC0 */
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

    canFire = g_asObjectCounter_00494be0[obj] <= 0;
    get_facing_range_from_object(obj, target);
    range = g_nTargetRange_0059ce10;
    closingSpeed = (short)(((g_anShipSpeed_0059b320[target] *
                             (int)g_nTargetFacing_0059d52a) / 100) >> 8);
    fireMissile = RandomBelowOrEqual(19) == 0 &&
                  RandomBelowOrEqual(7000) > range;
    if (fireMissile &&
        find_child_ship(obj, OBJECT_CLASS_MISSILE, target) != -1)
        fireMissile = 0;
    minePresent = 0;
    if (find_child_object(obj, OBJECT_CLASS_MINE) != -1)
        minePresent = 1;

    loadoutOffset = (int)obj * sizeof(g_aShipWeapons_0059cab0[0]);
    for (weapon = 0;
         weapon < *(signed char *)((unsigned char *)
             g_aShipWeapons_0059cab0 + loadoutOffset);
         weapon++) {
        slot = (ShipWeaponSlot *)((unsigned char *)
            g_aShipWeapons_0059cab0 + loadoutOffset +
            weapon * sizeof(ShipWeaponSlot) + 1);
        weaponType = slot->type;
        weaponData = &g_aObjectTypeData_00496d30[weaponType];
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
                    shouldFire = g_nFacingToTarget_00493194 > 70;
                    break;
                case OBJECT_TYPE_NEUTRON_PARTICLE_GUN:
                    shouldFire = g_nFacingToTarget_00493194 > 80;
                    break;
                case OBJECT_TYPE_MASS_DRIVER_CANNON:
                    shouldFire = g_nFacingToTarget_00493194 > 85;
                    break;
                case OBJECT_TYPE_TURRET:
                    shouldFire = g_nFacingToTarget_00493194 > 10;
                    break;
                }
            }
            slot->disabled = (signed char)!shouldFire;
            if (shouldFire) {
                firedObject = (short)fire_weapon(obj, weapon);
                if (weaponType == OBJECT_TYPE_TURRET) {
                    ComputeVectorDelta(
                        &g_aShipPosition_00494550[firedObject],
                        &g_aShipPosition_00494550[target], &direction);
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
                g_nFacingToTarget_00493194 >= -50 &&
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
                if (range < 2000 && g_nFacingToTarget_00493194 < -50 &&
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
                    interceptPoint = &g_aShipPosition_00494550[
                        WC2_SPACE_OBJECT_COUNT - 1];
                    AddFixedVectors(&g_aShipPosition_00494550[target],
                                    &direction, interceptPoint);
                    ComputeVectorDelta(
                        &g_aShipPosition_00494550[firedObject],
                        interceptPoint, &direction);
                    launch_object(obj, firedObject, direction, 20);
                }
                minePresent = 1;
            }
        } else if (weaponClass == OBJECT_CLASS_MISSILE) {
            if (fireMissile && targetInRange) {
                switch (weaponType) {
                case OBJECT_TYPE_DUMB_FIRE_MISSILE:
                    shouldFire = g_nFacingToTarget_00493194 > 97;
                    break;
                case OBJECT_TYPE_HEAT_SEEKING_MISSILE:
                    shouldFire = g_nFacingToTarget_00493194 > 40 &&
                                 g_nTargetFacing_0059d52a < -60;
                    break;
                case OBJECT_TYPE_FF_MISSILE:
                    shouldFire = 1;
                    break;
                case OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE:
                    shouldFire = g_nFacingToTarget_00493194 > 40;
                    break;
                }
            }
            if (shouldFire) {
                fire_weapon(obj, weapon);
                fireMissile = 0;
            }
        }
    }
}

/* Function start: 0x415508 */
short hemisphere(FixedVector *target, FixedVector *parent,
                 FixedVector *hardpoint)
{
    FixedVector parentFromHardpoint;
    FixedVector parentFromTarget;

    ComputeVectorDelta(hardpoint, parent, &parentFromHardpoint);
    ComputeVectorDelta(target, parent, &parentFromTarget);
    return vector_angle(parentFromTarget, parentFromHardpoint);
}

/* Function start: 0x415563 */
int fire_flack(short owner, short explosion, short range,
               FixedVector *aim)
{
    short projectileVelocity;
    short lifetime;

    projectileVelocity =
        g_aObjectTypeData_00496d30[OBJECT_TYPE_TURRET].maximumVelocity;
    set_objects_data(explosion, OBJECT_TYPE_TURRET, owner);
    lifetime = (short)(range / projectileVelocity -
                       RandomBelowOrEqual(8) - 5);
    lifetime = MaxShort(5, lifetime);
    lifetime = MinShort(27, lifetime);
    g_asObjectCounter_00494be0[explosion] = lifetime;
    send_at_point(explosion, aim, projectileVelocity);
    return 0;
}

/* Function start: 0x415625 */
short RandomSign(short v)
{
    if (RandomBelowOrEqual(1) != 0)
        return v;
    return -v;
}

/* Function start: 0x41565E */
int RandomFixedAimComponent(short radius, short speed, short maximum)
{
    short aim;

    aim = MinShort(maximum,
                   (short)(RandomBelowOrEqual(radius) + speed));
    return RandomSign(aim) << 8;
}

/* Function start: 0x4156A7 */
short SpawnFlakBurst(short obj, short range, FixedVector *hardpoint)
{
    FixedVector randomAim;
    FixedVector aimPoint;
    short target;
    short explosion;
    short aimRadius;
    short targetSpeed;
    short chance;

    target = g_acShipTarget_00495f20[obj];
    explosion = new_object(OBJECT_TYPE_EXPLOSION0, obj);
    if (explosion != -1) {
        aimRadius = MaxShort(400, (short)(range >> 2));
        targetSpeed = (short)(real_velocity(target) << 4);
        randomAim.x = RandomFixedAimComponent(aimRadius, targetSpeed, 1500);
        randomAim.y = RandomFixedAimComponent(aimRadius, targetSpeed, 1500);
        randomAim.z = RandomFixedAimComponent(aimRadius, targetSpeed, 1500);
        AddFixedVectors(&g_aShipPosition_00494550[target], &randomAim,
                        &aimPoint);
        chance = RandomBelowOrEqual(100);
        if ((g_asObjectCounter_00494be0[obj] != -1 || chance >= 40) &&
            chance >= 8) {
            g_aShipPosition_00494550[explosion] = aimPoint;
            explosion_shock_wave(
                explosion,
                g_aObjectTypeData_00496d30[
                    OBJECT_TYPE_TURRET].explosionDamage);
            return explosion;
        }
        g_aShipPosition_00494550[explosion] = *hardpoint;
        fire_flack(obj, explosion, range, &aimPoint);
        g_asObjectCounter_00494be0[obj] = (short)(RandomBelow(10) + 7);
    }
    return explosion;
}

/* Function start: 0x4159DB */
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
    for (weapon = 0;
         weapon < *(signed char *)((unsigned char *)
             g_aShipWeapons_0059cab0 + loadoutOffset);
         weapon++, slot++) {
        if (RandomBelowOrEqual(2) == 0) {
            position_child(obj, slot->hardpoint, &hardpoint);
            startTarget = RandomBelowOrEqual(lastTarget);
            targetIndex = startTarget;
            do {
                target = (short)g_acFormationMemberList_0059d490[
                    targetIndex];
                targetHemisphere = hemisphere(
                    &g_aShipPosition_00494550[target],
                    &g_aShipPosition_00494550[obj], &hardpoint);
                if (slot->type != OBJECT_TYPE_TURRET) {
                    if (targetHemisphere > 50 &&
                        RandomBelowOrEqual(14) == 0) {
                        g_acShipTarget_00495f20[obj] =
                            (signed char)target;
                        fire_weapon(obj, weapon);
                    }
                    break;
                }
                if (targetHemisphere >= 25) {
                    g_acShipTarget_00495f20[obj] = target;
                    SpawnFlakBurst(obj,
                              g_asTargetListRange_0059cf60[targetIndex],
                              &hardpoint);
                    break;
                }
                targetIndex++;
                if (targetIndex > lastTarget)
                    targetIndex = 0;
            } while (targetIndex != startTarget);
        }
    }
    return 1;
}

/* Function start: 0x415E2C */
short fire_weapon(short obj, short weapon)
{
    ObjectTypeData *weaponData;
#ifdef WC1_SDL
    ShipWeaponSlot *weaponSlot;
#endif
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
#ifdef WC1_SDL
    weaponSlot = (ShipWeaponSlot *)((unsigned char *)g_aShipWeapons_0059cab0 +
                                   weaponOffset + 1);
    weaponType = weaponSlot->type;
#else
    weaponType = *(enum ObjectType *)(void *)
        ((unsigned char *)g_aShipWeapons_0059cab0 + weaponOffset + 1);
#endif
    weaponClass = g_aObjectTypeData_00496d30[weaponType].objectClass;
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
        weaponData = &g_aObjectTypeData_00496d30[weaponType];
        copy_frame(obj, projectile);
        if (weaponClass == OBJECT_CLASS_PROJECTILE) {
            g_asShipAccumulatedDamage_0059dee0[projectile] =
                weaponData->damageCapacity;
            projectileSpeed = g_aObjectTypeData_00496d30[
                g_acObjectType_00493980[projectile]].maximumVelocity;
            g_asShipWeaponEnergy_0059d470[obj] =
                (short)(g_asShipWeaponEnergy_0059d470[obj] -
                        weaponData->animationDelay);
        }
#ifdef WC1_SDL
        child_object(weaponSlot->hardpoint, projectile, obj);
#else
        child_object(*(short *)(void *)
                         ((unsigned char *)g_aShipWeapons_0059cab0 +
                          weaponOffset + 5),
                     projectile, obj);
#endif
        g_asObjectCounter_00494be0[projectile] =
            g_aObjectTypeData_00496d30[weaponType].lifetime;
        vector_component_in_dir(&g_aShipVelocity_0059c010[obj],
                                &g_aShipForwardVector_00494208[projectile],
                                &g_aShipVelocity_0059c010[projectile]);
        if (weaponClass == OBJECT_CLASS_PROJECTILE) {
            ScaleFixedVector(&g_aShipForwardVector_00494208[obj],
                (int)(short)((weaponData->lifetime + 5) *
                             weaponData->maximumVelocity) << 8,
                &vector);
            AddFixedVectors(&g_aShipPosition_00494550[obj], &vector,
                            &vector);
            point_at(projectile, vector);
            if (g_nCockpitDisplayMode_0049d71c != 0 && g_cCockpitView_0059dab0 == 3) {
                ScaleFixedVector(&g_aShipUpVector_0059b9e0[obj], 0x12200,
                                 &cockpitOffset);
                AddFixedVectors(&cockpitOffset, &vector, &vector);
                point_at(projectile, vector);
            }
        }
        ScaleFixedVector(&g_aShipForwardVector_00494208[projectile],
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
            g_aeSpecialManeuver_00495600[projectile] =
                SPECIAL_MANEUVER_NONE;
            g_asShipManeuver_00495f48[projectile] = MANEUVER_NONE;
            g_aeShipTactic_0059d5e0[projectile] = TACTIC_SIT_STILL;
            g_asObjectCounter_00494be0[projectile] = 5;
            switch (weaponType) {
            case OBJECT_TYPE_DUMB_FIRE_MISSILE:
                steady_object(projectile);
                g_asObjectCounter_00494be0[projectile] = 1;
                g_acShipTarget_00495f20[projectile] =
                    g_acShipTarget_00495f20[obj];
                g_anShipSpeed_0059b320[projectile] =
                    get_ship_max_velocity(projectile) << 8;
                if (g_acShipTarget_00495f20[projectile] != -1) {
                    ComputeVectorDelta(&g_aShipPosition_00494550[obj],
                        &g_aShipPosition_00494550[
                            g_acShipTarget_00495f20[projectile]], &vector);
                    range = Vector_magnitude(&vector);
                    ScaleFixedVector(&g_aShipVelocity_0059c010[
                        g_acShipTarget_00495f20[projectile]],
                        range / get_ship_max_velocity(projectile), &vector);
                    AddFixedVectors(&g_aShipPosition_00494550[
                        g_acShipTarget_00495f20[projectile]], &vector,
                        &vector);
                    point_at(projectile, vector);
                }
                break;
            case OBJECT_TYPE_HEAT_SEEKING_MISSILE:
            case OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE:
                g_acShipTarget_00495f20[projectile] =
                    g_acShipTarget_00495f20[obj];
                break;
            case OBJECT_TYPE_FF_MISSILE:
                g_acShipTarget_00495f20[projectile] = -1;
                g_asObjectCounter_00494be0[projectile] = 15;
                break;
            }
        }
        if (obj == 0) {
            if (weaponClass == OBJECT_CLASS_PROJECTILE) {
                g_asObjectCounter_00494be0[obj] =
                    g_acGunRefireDelay_0046995c[
                        weaponType - OBJECT_TYPE_LASER_CANNON];
            }
        } else {
            g_asObjectCounter_00494be0[obj] = 12;
        }
#ifdef WC1_SDL
        if (obj == 0)
            Wc1SdlQueueJoystickWeaponRumble(weaponType);
#endif
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
