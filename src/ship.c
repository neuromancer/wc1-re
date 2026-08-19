/*
 *  Ship object, damage, explosion, and weapon logic from the Mac `ship`
 *  compilation unit.
 *
 *  Address range 41f000-420fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: the ordered Mac `ship` symbols map across the larger
 *  41dee0-42193f Win32 run; this file currently owns its central tranche.
 */
#include "wc1.h"

static const enum ObjectType aaeExplosionDebris[4][7] = { /* 0x004698e0 */
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

    if (obj != 0 && aeSpecialManeuver[obj] !=
                        SPECIAL_MANEUVER_UNKNOWN_9) {
        type = aeObjectType[obj];
        if (skill_check(
                obj,
                (short)((short)(abs(anObjectRollRotation[obj]) +
                                abs(anObjectYawRotation[obj]) +
                                abs(anObjectPitchRotation[obj])) /
                        (short)(aObjectTypeData[type].rollRate +
                                aObjectTypeData[type].yawRate +
                                aObjectTypeData[type].pitchRate))) ==
            0) {
            set_special(obj, SPECIAL_MANEUVER_BLOWING_UP);
            asObjectCounter[obj] =
                (short)(RandomBelowOrEqual(6) + 5);
        }
    }
    return 0;
}

/* Function start: 0x41E900 */
void send_appropriate_message(short attacker, short victim)
{
    if (aeObjectClass[attacker] >= OBJECT_CLASS_SHIP) {
        if (nYourWingman != -1 &&
            (short)acObjectOwner[attacker] ==
                nYourWingman &&
            nYourWingman != attacker &&
            aeShipSide[victim] == SIDE_KILRATHI) {
            if (RandomBelowOrEqual(100) < 50 &&
                aeSpecialManeuver[attacker] !=
                    SPECIAL_MANEUVER_UNKNOWN_9)
                send_message(nYourWingman, 5);
#ifdef WC1_SDL
        /* An unowned ship has owner -1.  The original indexes side[-1], which
           aliases the final roll-goal words and normally compares false. */
        } else if (acObjectOwner[attacker] != -1 &&
#else
        } else if (
#endif
                   aeShipSide[
                       (short)acObjectOwner[attacker]] ==
                       SIDE_KILRATHI &&
                   nYourWingman == victim) {
            send_message((short)acObjectOwner[attacker], 5);
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

    if (bPlayerVulnerable == 0 && victim == 0)
        return 0;
    if (damage == 0 ||
        aeSpecialManeuver[victim] ==
            SPECIAL_MANEUVER_UNKNOWN_9 ||
        aeObjectClass[victim] < OBJECT_CLASS_MISSILE)
        return 0;

    if (aeObjectClass[victim] < OBJECT_CLASS_SHIP) {
        asShipAccumulatedDamage[victim] = (short)(
            asShipAccumulatedDamage[victim] + damage);
        if (aObjectTypeData[
                aeObjectType[victim]].damageCapacity == -1)
            return 0;
        if (aObjectTypeData[
                aeObjectType[victim]].damageCapacity <=
            asShipAccumulatedDamage[victim])
            return explode(attacker, victim);
    } else {
        if (victim == 0) {
            TriggerPlayerHitPaletteFlash();
#ifdef WC1_SDL
            Wc1SdlQueueJoystickDamageRumble(damage);
#endif
        }

        if (attacker != -1 &&
            nYourWingman == victim &&
            acObjectOwner[attacker] == 0)
            send_message(victim, 10);

        quadrant = (short)(dot_product(impactDirection,
            &aShipForwardVector[victim]) > 0);
        damage = (short)(damage -
                         aasShipShield[victim][quadrant]);
        if (damage > 0) {
            aasShipShield[victim][quadrant] = 0;
            if (attacker != -1 &&
                aeObjectClass[attacker] ==
                    OBJECT_CLASS_PROJECTILE)
                PlaySfxWaveFileByNumber(9, victim, 0);

            sideDot = dot_product(impactDirection,
                                  &aShipRightVector[victim]);
            if (sideDot > 0xb5)
                quadrant = 3;
            else if (sideDot < -0xb5)
                quadrant = 2;
            damage = (short)(damage -
                             aasShipArmor[victim][quadrant]);
            if (damage > 0) {
                aasShipArmor[victim][quadrant] = 0;

                if (asObjectScreenX[victim] !=
                        (short)0x8001 &&
                    aeObjectClass[victim] !=
                        OBJECT_CLASS_CAPITAL_SHIP &&
                    RandomBelowOrEqual(1) == 0)
                    Create_ship_hit_debris(attacker, 1);
                if (RandomBelowOrEqual(99) == 0) {
                    if (attacker != 0 &&
                        attacker != nYourWingman &&
                        aeObjectClass[attacker] ==
                            OBJECT_CLASS_SHIP) {
                        if (aeShipSide[attacker] ==
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
            aasShipArmor[victim][quadrant] = (short)-damage;
        } else {
            aasShipShield[victim][quadrant] = (short)-damage;
            if (attacker != -1 &&
                aeObjectClass[attacker] ==
                    OBJECT_CLASS_PROJECTILE)
                PlaySfxWaveFileByNumber(10, victim, 0);
        }
    }
    return 0;
}

/* Function start: 0x41EC60 */
short pilot_hit(short obj)
{
    if (acPilotHitPoints[obj] > 0) {
        acPilotHitPoints[obj]--;
        if (acPilotHitPoints[obj] == 0) {
            if (obj == 0) {
                if (bPlayerVulnerable != 0)
                    nArcadeState = 4;
                return -1;
            }
        } else if (skill_check(obj, 9) == 0 && obj != 0) {
            asObjectCounter[obj] =
                (short)(RandomBelowOrEqual(20) + 30);
            set_special(obj, SPECIAL_MANEUVER_BLOWING_UP);
        }
    }
    return acPilotHitPoints[obj];
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
        asObjectScale[debris] <<= 2;
        asObjectCounter[debris] = 6;
        aShipVelocity[debris] =
            *(FixedVector *)(void *)
                ((unsigned char *)aShipVelocity +
                 objectOffset);
        MakeRandomVectorFixed(
            (short)(asObjectCollisionRadius[obj] >> 2),
            (short)(asObjectCollisionRadius[obj] >> 1),
            &offset);
        AddFixedVectors((FixedVector *)(void *)
                            ((unsigned char *)aShipPosition +
                             objectOffset),
                        &offset,
                        &aShipPosition[debris]);
    }
    return debris != -1;
}

/* Function start: 0x41EDB0 */
void call_enemy(short obj)
{
    short other;

    other = 0;
    for (; other < 10; other++) {
        if (aeObjectClass[other] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuver[other] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            aeShipSide[obj] != aeShipSide[other] &&
            RandomBelow(100) < 50)
            acShipTarget[other] = obj;
    }
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
    type = aeObjectType[victim];
    damageCapacity = aObjectTypeData[type].damageCapacity;
    if (aeObjectClass[victim] == OBJECT_CLASS_CAPITAL_SHIP) {
        if (aeShipSide[victim] == SIDE_KILRATHI) {
            events = MaxShort(1, (short)(damage >> 3));
            asShipAccumulatedDamage[victim] = (short)(
                asShipAccumulatedDamage[victim] + events);
            if (attacker != -1 && attacker < 10 &&
                any_enemy(attacker, 10000) == 0)
                call_enemy(attacker);
        } else {
            events = MaxShort(1, (short)(damage / 10));
            asShipAccumulatedDamage[victim] = (short)(
                asShipAccumulatedDamage[victim] + events);
            if (RandomBelowOrEqual(1000) < 35 && attacker != 0)
                send_message(victim, 4);
        }
        if (asShipAccumulatedDamage[victim] >=
            damageCapacity)
            return explode(attacker, victim);
        onboard_explosion(victim);
        return 0;
    }

    if (acShipRating[victim] != -1) {
        events = MaxShort(1, (short)(damage / 40));
        events = MinShort(RandomInRange(3, 4), events);
    } else {
        events = MaxShort(1, (short)(damage / 6));
    }
    asShipAccumulatedDamage[victim] = (short)(
        asShipAccumulatedDamage[victim] + events);

    while (events > 0) {
        if (events == 1 && acShipRating[victim] != -1)
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
            aasShipShield[victim][0] = 0;
            aasShipShield[victim][1] = 0;
            aasShipMaximumShield[victim][0] = 0;
            aasShipMaximumShield[victim][1] = 0;
            break;
        case 4:
            events--;
            acShipDamage[victim]++;
            if ((short)acShipDamage[victim] >
                damageCapacity)
                return explode(attacker, victim);
            break;
        case 5:
            if (quadrant == 0) {
                weaponCount = (short)(signed char)
                    aShipWeapons[victim][0];
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
                acShipDestroyedWeaponCount[victim] < 5) {
                events--;
                acShipDestroyedWeaponCount[victim]++;
            }
            break;
        case 7:
            if (quadrant == 1) {
                events--;
                drain_fuel(victim,
                           (short)(*(int *)&aObjectTypeData[
                                       type].lifetime / 4));
                if (RandomBelowOrEqual(1) != 0 ||
                    anShipFuel[victim] < 0)
                    return explode(attacker, victim);
            }
            break;
        case 8:
            if (quadrant == 0 &&
                acShipCommunicator[victim] != -1) {
                acShipCommunicator[victim] = -1;
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

    aasShipMaximumShield[obj][0] =
        MaxShort(0, (short)(aasShipMaximumShield[obj][0] -
            (aObjectTypeData[
                aeObjectType[obj]].shieldFore >> 2)));
    maximum = aasShipMaximumShield[obj][0];
    aasShipMaximumShield[obj][1] =
        MaxShort(0, (short)(maximum -
            (aObjectTypeData[
                aeObjectType[obj]].shieldAft >> 2)));
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

    attackerClass = aeObjectClass[attacker];
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
    playerType = aeObjectType[0];
    severity = (signed char)RandomBelowOrEqual(10);
    asShipAccumulatedDamage[0] = (short)(
        asShipAccumulatedDamage[0] + events);
    if (events > 1)
        place_damage_on_cockpit(RandomBelowOrEqual(3));

    while (events > 0) {
        events--;
        system = *(const signed char *)(const void *)
            &asPlayerDamageSystemTable[
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
            acShipDamage[0]++;
            if (acShipDamage[0] == 1) {
                pilot_hit(0);
                if (events > 0)
                    events--;
            } else if ((short)acShipDamage[0] >
                       aObjectTypeData[playerType].
                           damageCapacity) {
                return explode(attacker, 0);
            }
            break;
        case 5:
            if (quadrant != 0) {
                events++;
            } else {
                weaponCount = (short)(signed char)aShipWeapons[0][0];
                if (weaponCount > 0) {
                    remove_weapon(0,
                        RandomBelowOrEqual(
                            (short)(weaponCount - 1)));
                    ShowComponentHitHudMessage(
                        szWeaponDestroyed,
                        cRedColour, 8);
                }
            }
            break;
        case 6:
            if (quadrant != 0)
                events++;
            else if (acShipDestroyedWeaponCount[0] < 5) {
                    acShipDestroyedWeaponCount[0]++;
                    check_computer_damage();
                }
            break;
        case 7:
            drain_fuel(0,
                       (short)(*(int *)&aObjectTypeData[
                                   playerType].lifetime / 4));
            if (RandomBelowOrEqual(1) != 0 ||
                anShipFuel[0] < 0)
                return explode(attacker, 0);
            ShowComponentHitHudMessage(szFuelTanksHit,
                                       cRedColour, 8);
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
                    if (acPlayerComponentDamage[4] > 3)
                        acShipCommunicator[0] = -1;
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
        (short)acPlayerComponentDamage[component]) {
        acPlayerComponentDamage[component]--;
        sprintf(message, szComponentFixedFormat,
                apszComponentNames[component]);
        ShowComponentHitHudMessage(message, cRedColour, 8);
        return 1;
    }
    return 0;
}

/* Function start: 0x41F660 */
void repair_internal_damage(void)
{
    short repair;
#ifndef WC1_SDL
    short component;
#endif

#ifndef WC1_SDL
    if ((short)acPlayerComponentDamage[component] >= 4)
        return;
#endif
    if (RandomBelowOrEqual(500) >= 2)
        return;
    repair = RandomBelowOrEqual(2);
    switch (repair) {
    case 0:
#ifdef WC1_SDL
        /* The Mac body guards the selected component in each repair case.
           Retail Win32 instead reads an uninitialized component above. */
        if (acPlayerComponentDamage[2] >= 4)
            break;
#endif
        ReportComponentRepaired(2, 1);
        break;
    case 1:
#ifdef WC1_SDL
        if (acPlayerComponentDamage[0] >= 4)
            break;
#endif
        if (ReportComponentRepaired(0, 2) != 0)
            damage_ion_drive(0, -1, 3);
        break;
    case 2:
        if (aObjectTypeData[
                aeObjectType[0]].damageCapacity - 3 <
            (short)acShipDamage[0])
            acShipDamage[0]--;
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
            aeShipHitDebrisTypes[
                RandomBelowOrEqual(2)], -1);
        asObjectCounter[debris] = 40;
        FillFixedVectorWithRandomComponents(10, &offset);
        AddFixedVectors(&aShipPosition[obj], &offset,
                        &aShipPosition[debris]);
        FillFixedVectorWithRandomComponents(
            6, &aShipVelocity[debris]);
        created++;
    }
}

/* Function start: 0x41F7C0 */
void check_next_wave(void)
{
    short obj;

    if (nCurrentWave == -1)
        return;
    obj = 0;
    do {
        if (aeObjectClass[obj] == OBJECT_CLASS_SHIP &&
            aeShipSide[obj] == SIDE_KILRATHI)
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
    for (; index < 7; index++) {
        debris = find_vacant_3d_object();
        if (debris == -1)
            break;
        set_objects_data(debris,
                         aaeExplosionDebris[set][index], -1);
        asObjectCounter[debris] = 40;
        FillFixedVectorWithRandomComponents(50, &vector);
        AddFixedVectors(&aShipPosition[obj], &vector,
                        &aShipPosition[debris]);
        FillFixedVectorWithRandomComponents(
            25, &aShipVelocity[debris]);
        divide_vector(&aShipVelocity[obj], 0x200, &vector);
        AddFixedVectors(&vector,
                        &aShipVelocity[debris],
                        &aShipVelocity[debris]);
    }
    index = 0;
    for (; index < 8; index++) {
        debris = find_vacant_3d_object();
        if (debris == -1)
            break;
        FillFixedVectorWithRandomComponents(50, &vector);
        AddFixedVectors(&aShipPosition[obj], &vector,
                        &aShipPosition[debris]);
        FillFixedVectorWithRandomComponents(
            25, &aShipVelocity[debris]);
        divide_vector(&aShipVelocity[obj], 0x200, &vector);
        AddFixedVectors(&vector,
                        &aShipVelocity[debris],
                        &aShipVelocity[debris]);
        asObjectScreenAngle[debris] =
            (short)(RandomBelowOrEqual(3) + 0x10);
        asObjectCounter[debris] = 40;
        aeObjectClass[debris] = OBJECT_CLASS_DUST;
        aeObjectType[debris] = OBJECT_TYPE_DEBRIS_DUST;
    }
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
    stCampaignState.missionScore =
        (short)(stCampaignState.missionScore + score);
    if (pilot == 0) {
        nMissionMedalScore =
            (short)(nMissionMedalScore + score);
        nArcadeScore += score * 10;
    }
    return 0;
}

/* Function start: 0x41FA90 */
unsigned int score_for_kill(short pilot, short victim)
{
    int event;

    if (aeShipSide[victim] == SIDE_KILRATHI) {
        switch (aeObjectType[victim]) {
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

    enemy = aeShipSide[victim] !=
            aeShipSide[attacker];
    new_space_music_changes(attacker, victim);
    if (enemy) {
        send_message(attacker, 5);
        score_for_kill(attacker, victim);
        if (attacker == 0)
            nPlayerKillCount++;
        else if (nYourWingman == attacker)
            nWingmanKillCount++;
    }
    return 0;
}

/* Function start: 0x41FBC0 */
short ShipExplosion(short obj)
{
    unsigned short originalScale;
    short explosion;

    originalScale = (unsigned short)asObjectScale[obj];
    explosion = find_vacant_3d_object();
    if (explosion == -1) {
        if (aeObjectClass[obj] ==
            OBJECT_CLASS_CAPITAL_SHIP)
            FreePacketAndClear(&apObjectShape[obj], 0);
        asCapitalShipViewFrame[obj] = -1;
        explosion = obj;
    } else {
        copy_frame(obj, explosion);
        aShipPosition[explosion] =
            aShipPosition[obj];
        aShipVelocity[explosion] =
            aShipVelocity[obj];
        acObjectOwner[explosion] = (signed char)obj;
    }
    set_objects_data(explosion, OBJECT_TYPE_EXPLOSION1,
                     (short)acObjectOwner[explosion]);
    asObjectScale[explosion] = (short)(
        (unsigned short)asObjectScale[explosion] *
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

    objectClass = aeObjectClass[obj];
    explosion = obj;
    if (objectClass >= OBJECT_CLASS_SHIP) {
        missionShip = -1;
        if (objectClass == OBJECT_CLASS_CAPITAL_SHIP)
            missionShip = find_ship_index(obj);
        if (obj < 10 &&
            (acShipRating[obj] != -1 ||
             (missionShip != -1 &&
              anShipMissionShip[obj] == missionShip) ||
             RandomBelow(100) <= 2)) {
            send_message(obj, 7);
        }
        rating = acShipRating[obj];
        if (rating != -1 && rating != RATING_ACE_ICEMAN)
            personality_killed((short)rating);
        if (nYourWingman == obj) {
            missionShip = 0;
            nWingmanKilledThisMission = 1;
            for (; missionShip < 10; missionShip++) {
                if (acShipRating[missionShip] >
                    RATING_ACE_ICEMAN)
                    break;
            }
            if (missionShip < 10)
                send_message(missionShip, 5);
            nYourWingman = -1;
        }
        set_special(obj, SPECIAL_MANEUVER_UNKNOWN_9);
        asObjectCounter[obj] = 8;
        aMissionShips[
            nShipMissionIndices[obj]].state = 3;
        if (aeObjectClass[obj] ==
            OBJECT_CLASS_CAPITAL_SHIP) {
            short count = 4;

            for (; count != 0; count--) {
                onboard_explosion(obj);
            }
            asObjectCounter[obj] =
                (short)(aObjectTypeData[
                    aeObjectType[obj]].damageCapacity >> 2) + 8;
        } else {
            explosion = ShipExplosion(obj);
        }
    } else {
        explosionType = OBJECT_TYPE_EXPLOSION2;
        if (aeObjectType[obj] == OBJECT_TYPE_TURRET ||
            objectClass == OBJECT_CLASS_ASTEROID)
            explosionType = OBJECT_TYPE_EXPLOSION0;
        set_objects_data(obj, explosionType,
                         acObjectOwner[obj]);
        if (aeObjectClass[obj] == OBJECT_CLASS_ASTEROID)
            asObjectScale[obj] = 0x380;
    }
    if (objectClass != OBJECT_CLASS_CAPITAL_SHIP)
        explosion_shock_wave(
            obj, aObjectTypeData[
                aeObjectType[obj]].explosionDamage);
    if (asObjectScreenX[obj] != (short)0x8001)
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
        owner = acObjectOwner[obj];
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
    for (; other < 10; other++) {
        if (other != obj &&
            aeObjectClass[other] >= OBJECT_CLASS_SHIP) {
            ComputeVectorDelta(&aShipPosition[obj],
                               &aShipPosition[other], &delta);
            distance = FixedToShortSaturating(
                Vector_magnitude(&delta));
            distance = MaxShort(0,
                (short)(distance -
                        asObjectCollisionRadius[other]));
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

/* Function start: 0x420040 */
int explode(short attacker, short victim)
{
    short creator;

    if (victim < 10) {
        if (acShipRating[victim] != -1 &&
            acShipRating[victim] != RATING_ACE_ICEMAN) {
            if (acShipRating[victim] > RATING_ACE_ICEMAN) {
                if (ace_status(
                        (short)(acShipRating[victim] -
                                RATING_ACE_ANGEL),
                                      0x20) != 0) {
                    unflag_ace(
                        (short)(acShipRating[victim] -
                                RATING_ACE_ANGEL),
                        0x20);
                    acShipStress[victim] = -25;
                    reset_maneuver(victim, MANEUVER_OUTA_HERE);
                    acShipDamage[victim] =
                        (signed char)(acShipDamage[victim] / 2);
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
        aeSpecialManeuver[victim] ==
            SPECIAL_MANEUVER_UNKNOWN_9 &&
        aeObjectClass[victim] >= OBJECT_CLASS_SHIP)
#else
    if (aeSpecialManeuver[victim] ==
            SPECIAL_MANEUVER_UNKNOWN_9 &&
        aeObjectClass[victim] >= OBJECT_CLASS_SHIP)
#endif
        return 0;

    if (victim == 0) {
        if (bPlayerVulnerable == 0)
            return 0;
        bPlayerDestroyed = 1;
        nArcadeState = 4;
        return 1;
    }

    if (nExternalViewShip == victim)
        nExternalViewShip = -1;
    creator = the_creator(attacker);
    if (creator != -1 &&
        aeObjectClass[victim] >= OBJECT_CLASS_SHIP)
        analyze_kill(creator, victim);
    Explosion(victim);
    return 1;
}

/* Function start: 0x420190 */
int send_at_point(short obj, FixedVector *point, short speed)
{
    ComputeVectorDelta(&aShipPosition[obj], point,
                       &aShipVelocity[obj]);
    SetVectorFixedPoint((unsigned int *)&aShipVelocity[obj],
                        speed);
    return 0;
}

/* Function start: 0x4201D0 */
short find_child_object(short parent, enum ObjectClass objectClass)
{
    short obj;

    obj = 0;
    do {
        if ((short)acObjectOwner[obj] == parent &&
            aeObjectClass[obj] == objectClass)
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
        if ((short)acObjectOwner[obj] == parent &&
            aeObjectClass[obj] == objectClass &&
            (target == -1 ||
             acShipTarget[obj] == target))
            return obj;
        obj++;
    } while (obj < 10);
    return -1;
}

/* Function start: 0x420260 */
int launch_object(short parent, short child, FixedVector direction,
                  short speed)
{
    NormalizeFixedVector(&direction);
    vector_component_in_dir(&aShipVelocity[parent],
                            &direction,
                            &aShipVelocity[child]);
    ScaleFixedVector(&direction, (int)speed << 8, &direction);
    AddFixedVectors(&direction, &aShipVelocity[child],
                    &aShipVelocity[child]);
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

    canFire = asObjectCounter[obj] <= 0;
    get_facing_range_from_object(obj, target);
    range = nTargetRange;
    closingSpeed = (short)(((anShipSpeed[target] *
                             (int)nTargetFacing) / 100) >> 8);
    fireMissile = RandomBelowOrEqual(19) == 0 &&
                  RandomBelowOrEqual(7000) > range;
    if (fireMissile &&
        find_child_ship(obj, OBJECT_CLASS_MISSILE, target) != -1)
        fireMissile = 0;
    minePresent = 0;
    if (find_child_object(obj, OBJECT_CLASS_MINE) != -1)
        minePresent = 1;

    loadoutOffset = (int)obj * sizeof(aShipWeapons[0]);
    for (weapon = 0;
         weapon < *(signed char *)((unsigned char *)
             aShipWeapons + loadoutOffset);
         weapon++) {
        slot = (ShipWeaponSlot *)((unsigned char *)
            aShipWeapons + loadoutOffset +
            weapon * sizeof(ShipWeaponSlot) + 1);
        weaponType = slot->type;
        weaponData = &aObjectTypeData[weaponType];
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
                    shouldFire = nFacingToTarget > 70;
                    break;
                case OBJECT_TYPE_NEUTRON_PARTICLE_GUN:
                    shouldFire = nFacingToTarget > 80;
                    break;
                case OBJECT_TYPE_MASS_DRIVER_CANNON:
                    shouldFire = nFacingToTarget > 85;
                    break;
                case OBJECT_TYPE_TURRET:
                    shouldFire = nFacingToTarget > 10;
                    break;
                }
            }
            slot->disabled = (signed char)!shouldFire;
            if (shouldFire) {
                firedObject = (short)fire_weapon(obj, weapon);
                if (weaponType == OBJECT_TYPE_TURRET) {
                    ComputeVectorDelta(
                        &aShipPosition[firedObject],
                        &aShipPosition[target], &direction);
                    launch_object(obj, firedObject, direction,
                                  real_velocity(firedObject));
                }
            }
        } else if (weaponClass == OBJECT_CLASS_MINE) {
            /* The first angle test deliberately preserves the original
             * stack-local ordering recovered at 0x42056a. */
            if (!minePresent && weaponType == OBJECT_TYPE_SPACE_MINE &&
                anShipSpeed[obj] >= 0x500 &&
                velocityAngle >= 75 && range <= 2000 &&
                nFacingToTarget >= -50 &&
                nTargetFacing <= 90) {
                velocityAngle = vector_angle(
                    aShipVelocity[target],
                    aShipVelocity[obj]);
                predictionTime = (short)(900 /
                    (short)((anShipSpeed[obj] >> 8) + 20));
                predictedSeparation = (short)(predictionTime *
                    (short)(-20 - closingSpeed) + range);
                if (closingSpeed == -20)
                    mineTime = range;
                else
                    mineTime = (short)(range / (closingSpeed + 20));
                if (range < 2000 && nFacingToTarget < -50 &&
                    nTargetFacing > 90) {
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
                    ScaleFixedVector(&aShipVelocity[target],
                                     (int)mineTime, &direction);
                    interceptPoint = &aShipPosition[
                        WC1_SPACE_OBJECT_COUNT - 1];
                    AddFixedVectors(&aShipPosition[target],
                                    &direction, interceptPoint);
                    ComputeVectorDelta(
                        &aShipPosition[firedObject],
                        interceptPoint, &direction);
                    launch_object(obj, firedObject, direction, 20);
                }
                minePresent = 1;
            }
        } else if (weaponClass == OBJECT_CLASS_MISSILE) {
            if (fireMissile && targetInRange) {
                switch (weaponType) {
                case OBJECT_TYPE_DUMB_FIRE_MISSILE:
                    shouldFire = nFacingToTarget > 97;
                    break;
                case OBJECT_TYPE_HEAT_SEEKING_MISSILE:
                    shouldFire = nFacingToTarget > 40 &&
                                 nTargetFacing < -60;
                    break;
                case OBJECT_TYPE_FF_MISSILE:
                    shouldFire = 1;
                    break;
                case OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE:
                    shouldFire = nFacingToTarget > 40;
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
        aObjectTypeData[OBJECT_TYPE_TURRET].maximumVelocity;
    set_objects_data(explosion, OBJECT_TYPE_TURRET, owner);
    lifetime = (short)(range / projectileVelocity -
                       RandomBelowOrEqual(8) - 5);
    lifetime = MaxShort(5, lifetime);
    lifetime = MinShort(27, lifetime);
    asObjectCounter[explosion] = lifetime;
    send_at_point(explosion, aim, projectileVelocity);
    return 0;
}

/* Function start: 0x4208C0 */
short rnd_sign(short v)
{
    if (RandomBelowOrEqual(1) != 0)
        return v;
    return -v;
}

/* Function start: 0x4208E0 */
int rnd_aim(short radius, short speed, short maximum)
{
    short aim;

    aim = MinShort(maximum,
                   (short)(RandomBelowOrEqual(radius) + speed));
    return rnd_sign(aim) << 8;
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

    target = acShipTarget[obj];
    explosion = new_object(OBJECT_TYPE_EXPLOSION0, obj);
    if (explosion != -1) {
        aimRadius = MaxShort(400, (short)(range >> 2));
        targetSpeed = (short)(real_velocity(target) << 4);
        randomAim.x = rnd_aim(aimRadius, targetSpeed, 1500);
        randomAim.y = rnd_aim(aimRadius, targetSpeed, 1500);
        randomAim.z = rnd_aim(aimRadius, targetSpeed, 1500);
        AddFixedVectors(&aShipPosition[target], &randomAim,
                        &aimPoint);
        chance = RandomBelowOrEqual(100);
        if ((asObjectCounter[obj] != -1 || chance >= 40) &&
            chance >= 8) {
            aShipPosition[explosion] = aimPoint;
            explosion_shock_wave(
                explosion,
                aObjectTypeData[
                    OBJECT_TYPE_TURRET].explosionDamage);
            return explosion;
        }
        aShipPosition[explosion] = *hardpoint;
        fire_flack(obj, explosion, range, &aimPoint);
        asObjectCounter[obj] = (short)(RandomBelow(10) + 7);
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

    loadoutOffset = (int)obj * sizeof(aShipWeapons[0]);
    slot = (ShipWeaponSlot *)((unsigned char *)
        aShipWeapons + loadoutOffset + 1);
    for (weapon = 0;
         weapon < *(signed char *)((unsigned char *)
             aShipWeapons + loadoutOffset);
         weapon++, slot++) {
        if (RandomBelowOrEqual(2) == 0) {
            position_child(obj, slot->hardpoint, &hardpoint);
            startTarget = RandomBelowOrEqual(lastTarget);
            targetIndex = startTarget;
            do {
                target = (short)acFormationMemberList[
                    targetIndex];
                targetHemisphere = hemisphere(
                    &aShipPosition[target],
                    &aShipPosition[obj], &hardpoint);
                if (slot->type != OBJECT_TYPE_TURRET) {
                    if (targetHemisphere > 50 &&
                        RandomBelowOrEqual(14) == 0) {
                        acShipTarget[obj] =
                            (signed char)target;
                        fire_weapon(obj, weapon);
                    }
                    break;
                }
                if (targetHemisphere >= 25) {
                    acShipTarget[obj] = target;
                    pop_flack(obj,
                              asTargetListRange[targetIndex],
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

/* Function start: 0x420C20 */
int fire_weapon(short obj, short weapon)
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
    weaponOffset = (int)obj * sizeof(aShipWeapons[0]) +
                   (int)weapon * sizeof(ShipWeaponSlot);
#ifdef WC1_SDL
    weaponSlot = (ShipWeaponSlot *)((unsigned char *)aShipWeapons +
                                   weaponOffset + 1);
    weaponType = weaponSlot->type;
#else
    weaponType = *(enum ObjectType *)(void *)
        ((unsigned char *)aShipWeapons + weaponOffset + 1);
#endif
    weaponClass = aObjectTypeData[weaponType].objectClass;
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
        weaponData = &aObjectTypeData[weaponType];
        copy_frame(obj, projectile);
        if (weaponClass == OBJECT_CLASS_PROJECTILE) {
            asShipAccumulatedDamage[projectile] =
                weaponData->damageCapacity;
            projectileSpeed = aObjectTypeData[
                aeObjectType[projectile]].maximumVelocity;
            asShipWeaponEnergy[obj] =
                (short)(asShipWeaponEnergy[obj] -
                        weaponData->animationDelay);
        }
#ifdef WC1_SDL
        child_object(weaponSlot->hardpoint, projectile, obj);
#else
        child_object(*(short *)(void *)
                         ((unsigned char *)aShipWeapons +
                          weaponOffset + 5),
                     projectile, obj);
#endif
        asObjectCounter[projectile] =
            aObjectTypeData[weaponType].lifetime;
        vector_component_in_dir(&aShipVelocity[obj],
                                &aShipForwardVector[projectile],
                                &aShipVelocity[projectile]);
        if (weaponClass == OBJECT_CLASS_PROJECTILE) {
            ScaleFixedVector(&aShipForwardVector[obj],
                (int)(short)((weaponData->lifetime + 5) *
                             weaponData->maximumVelocity) << 8,
                &vector);
            AddFixedVectors(&aShipPosition[obj], &vector,
                            &vector);
            point_at(projectile, vector);
            if (bCockpitlessView != 0 && cCockpitView == 3) {
                ScaleFixedVector(&aShipUpVector[obj], 0x12200,
                                 &cockpitOffset);
                AddFixedVectors(&cockpitOffset, &vector, &vector);
                point_at(projectile, vector);
            }
        }
        ScaleFixedVector(&aShipForwardVector[projectile],
                         (int)projectileSpeed << 8, &vector);
        AddFixedVectors(&vector, &aShipVelocity[projectile],
                        &aShipVelocity[projectile]);
        if (weaponClass == OBJECT_CLASS_MISSILE) {
            ScaleFixedVector(&aShipUpVector[obj], 0xa00,
                             &vector);
            AddFixedVectors(&vector, &aShipVelocity[projectile],
                            &aShipVelocity[projectile]);
            if (obj == 0)
                RemovePlayerReleaseWeapon((signed char)weapon);
            else
                remove_weapon(obj, weapon);
            acObjectCollisionGraceTicks[projectile] = 20;
            aeSpecialManeuver[projectile] =
                SPECIAL_MANEUVER_NONE;
            aeShipManeuver[projectile] = MANEUVER_NONE;
            aeShipTactic[projectile] = TACTIC_SIT_STILL;
            asObjectCounter[projectile] = 5;
            switch (weaponType) {
            case OBJECT_TYPE_DUMB_FIRE_MISSILE:
                steady_object(projectile);
                asObjectCounter[projectile] = 1;
                acShipTarget[projectile] =
                    acShipTarget[obj];
                anShipSpeed[projectile] =
                    get_ship_max_velocity(projectile) << 8;
                if (acShipTarget[projectile] != -1) {
                    ComputeVectorDelta(&aShipPosition[obj],
                        &aShipPosition[
                            acShipTarget[projectile]], &vector);
                    range = Vector_magnitude(&vector);
                    ScaleFixedVector(&aShipVelocity[
                        acShipTarget[projectile]],
                        range / get_ship_max_velocity(projectile), &vector);
                    AddFixedVectors(&aShipPosition[
                        acShipTarget[projectile]], &vector,
                        &vector);
                    point_at(projectile, vector);
                }
                break;
            case OBJECT_TYPE_HEAT_SEEKING_MISSILE:
            case OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE:
                acShipTarget[projectile] =
                    acShipTarget[obj];
                break;
            case OBJECT_TYPE_FF_MISSILE:
                acShipTarget[projectile] = -1;
                asObjectCounter[projectile] = 15;
                break;
            }
        }
        if (obj == 0) {
            if (weaponClass == OBJECT_CLASS_PROJECTILE) {
                asObjectCounter[obj] =
                    acGunRefireDelay[
                        weaponType - OBJECT_TYPE_LASER_CANNON];
            }
        } else {
            asObjectCounter[obj] = 12;
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
