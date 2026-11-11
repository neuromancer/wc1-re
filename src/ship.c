/**
 * @file
 * @ingroup flight
 * @brief Ships: damage, explosions and weapons.
 *
 * Address range 41dee0-4219bf, the Mac ship compilation unit: its symbols,
 * initialize_object through your_afterburner, map in order onto this range.
 */
#include "wc1.h"
#include <sys/stat.h>
#include <setjmp.h>

short asPlayerDamageSystemTable[50] = {
    0, 8, 6, 5, 0, 3, 5, 5, 7, 6,
    0, 8, 6, 5, 4, 3, 4, 0, 4, 4,
    1, 2, 5, 2, 7, 3, 4, 7, 5, 1,
    1, 4, 1, 5, 2, 3, 4, 7, 2, 1,
    4, 4, 4, 4, 0, 8, 6, 5, 4, 0
}; /* 0x00469878 */

static enum ObjectType aaeExplosionDebris[4][7] = {
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
}; /* 0x004698e0 */

enum ObjectType aeShipHitDebrisTypes[3] = {
    OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK,
    OBJECT_TYPE_DEBRIS_SHIP_TUBING,
    OBJECT_TYPE_DEBRIS_O_RING
}; /* 0x00469950 */

signed char acGunRefireDelay[4] = {6, 10, 4, 0}; /* 0x0046995c */

/* Linker commons defined here; LINK orders them by identifier hash. */
int nMemoryConfig; /* 0x005a7cd4 */
int dwOriginalFreeMem; /* 0x005a7cd8 */
struct Shape *pMouseCursorResource; /* 0x005a7cdc */
int nAvailableGameMemory; /* 0x005a7ce0 */
int nSceneResourceBudgetValue; /* 0x005a7ce4 */
int nAfterburnerSoundDeadline; /* 0x005a7ce8 */
int bAfterburnerSfxEnabled; /* 0x005a7cec */

/* Function start: 0x41DEE0 */
short initialize_object(short obj, enum ObjectType type, short owner)
{
    if (obj != -1) {
        set_objects_data(obj, type, owner);
        zero_vector(&aShipPositionsArray[obj]);
        zero_vector(&aShipVelocity[obj]);
    }
    return obj;
}

/* Function start: 0x41DF40 */
short borrow_dust(void)
{
    short i = 0x22;

    do {
        if (aeObjectClass[i] == OBJECT_CLASS_DUST)
            return i;
        i = i + 1;
    } while (i < 0x2a);
    return -1;
}

/* Function start: 0x41DF70 */
short new_object(enum ObjectType type, short owner)
{
    short obj;

    obj = find_vacant_3d_object();
    if (obj == -1 && owner == 0)
        obj = borrow_dust();
    return initialize_object(obj, type, owner);
}

/* Function start: 0x41DFA0 */
short initialize_ship(enum ObjectType type, short owner)
{
    short obj = get_ship_slot();

    if (obj != -1) {
        initialize_object(obj, type, owner);
        aeShipSide[obj] = SIDE_NEUTRAL;
    }
    return obj;
}

/* Function start: 0x41DFE0 */
short any_selected(signed char *loadout, enum ObjectClass objectClass)
{
    short selected;
    short weapon;

    selected = 0;
    for (weapon = 0; weapon < loadout[0] && selected == 0;
         weapon++) {
        if (aObjectTypeData[
                ((ShipWeaponSlot *)(loadout + 1))[weapon].type]
                .objectClass == objectClass &&
            ((ShipWeaponSlot *)(loadout + 1))[weapon].disabled == 0)
            selected = 1;
    }
    return selected;
}

/* Function start: 0x41E040 */
unsigned int remove_weapon(short obj, short weapon)
{
    enum ObjectType preferredType;
    enum ObjectClass objectClass;
    signed char *loadout;
    short slot;

#ifdef SDL_PORT
    if (SdlSwcFlightActive()) {
        /* CODE_05 remove_weapon +0x019a consumes the native slot's
           quantity before removing it. WC1 has no quantity byte. */
        if (obj < 0 || obj >= 10 || weapon < 0 ||
            aShipWeaponsTable[obj][0] > 10 ||
            weapon >= aShipWeaponsTable[obj][0])
            return 0;
        if (aSwcWeaponAmmo[obj][weapon] > 1) {
            aSwcWeaponAmmo[obj][weapon]--;
            return 0;
        }
        memmove(&aSwcWeaponAmmo[obj][weapon], &aSwcWeaponAmmo[obj][weapon + 1],
                aShipWeaponsTable[obj][0] - weapon - 1);
        aSwcWeaponAmmo[obj][aShipWeaponsTable[obj][0] - 1] = 0;
    }
#endif
    loadout = aShipWeaponsTable[obj];
    preferredType =
        ((ShipWeaponSlot *)&aShipWeaponsTable[obj][1])[weapon].type;
    objectClass = aObjectTypeData[preferredType].objectClass;
    if (weapon < loadout[0] - 1) {
        for (slot = weapon; slot < loadout[0] - 1; slot++) {
            signed char *entry = loadout + slot * 7;

            ((ShipWeaponSlot *)(entry + 1))[0].type =
                ((ShipWeaponSlot *)(entry + 1))[1].type;
            ((ShipWeaponSlot *)(entry + 1))[0].hardpoint =
                ((ShipWeaponSlot *)(entry + 1))[1].hardpoint;
            ((ShipWeaponSlot *)(entry + 1))[0].disabled =
                ((ShipWeaponSlot *)(entry + 1))[1].disabled;
        }
#ifdef SDL_PORT
        /* The original spare-slot write corrupts the next ship's first
           hardpoint when all ten slots are occupied. SWC also guards it. */
        if (loadout[0] < 10)
#endif
            loadout[loadout[0] * 7 + 7] = 1;
    }
    loadout[0]--;
    if (obj == 0) {
        if (any_selected(loadout, objectClass) == 0) {
            if (objectClass == OBJECT_CLASS_PROJECTILE) {
                select_new_gun();
            } else {
                nSelectedReleaseWeaponIndex = -1;
                select_new_release_weapon(preferredType);
            }
        }
        if (get_mode(0) == 1)
            InvalidateVduMode(0);
    }
    return 0;
}

/* Function start: 0x41E120 */
void set_objects_data(short obj, enum ObjectType type, short owner)
{
    ObjectTypeData *typeData;
    enum ObjectClass objectClass;
    short value;
    short weapon;

    if (type == OBJECT_TYPE_SPACE_DUST) {
        aeObjectTypeList[obj] = OBJECT_TYPE_SPACE_DUST;
        aeObjectClass[obj] = OBJECT_CLASS_DUST;
        return;
    }
    if (aObjectTypeData[type].shapeSet == 0
#ifdef SDL_PORT
        /* SWC owns CMF textures instead of WC1 disk packets. */
        && !SdlSwcFlightActive()
#endif
    ) {
        switch (type) {
        case OBJECT_TYPE_ASTEROID2:
            type = OBJECT_TYPE_ASTEROID1;
            break;
        case OBJECT_TYPE_ASTEROID4:
            type = OBJECT_TYPE_ASTEROID3;
            break;
        case OBJECT_TYPE_ASTEROID6:
            type = OBJECT_TYPE_ASTEROID5;
            break;
        case OBJECT_TYPE_DEBRIS_METAL_SHEET:
            type = OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK;
            break;
        case OBJECT_TYPE_DEBRIS_WING:
            type = OBJECT_TYPE_DEBRIS_PIPE;
            break;
        case OBJECT_TYPE_EXPLOSION1:
            type = OBJECT_TYPE_EXPLOSION0;
            break;
        case OBJECT_TYPE_EXPLOSION2:
            type = OBJECT_TYPE_EXPLOSION0;
            break;
        }
    }
    typeData = &aObjectTypeData[type];
    aeObjectTypeList[obj] = type;
    aeObjectClass[obj] = typeData->objectClass;
    if (type == OBJECT_TYPE_ROCK_CHUNK)
        apObjShapeArray[obj] =
            aObjectTypeData[OBJECT_TYPE_ASTEROID1].shapeSet;
    else
        apObjShapeArray[obj] = typeData->shapeSet;
    init_ijk(obj);
    asObjectCollisionRadius[obj] = typeData->collisionRadius;
    awObjMassArray[obj] = typeData->mass;
    awObjectScales[obj] = typeData->scale;
    asObjAfterburnerVelocities[obj] =
        typeData->afterburnerVelocity;
    asShipAccumulatedDmg[obj] = 0;
    asObjectFlipArray[obj] = asShipAccumulatedDmg[obj];
    asObjScreenAngles[obj] = asObjectFlipArray[obj];
    acObjOwnersTable[obj] = owner;
    acLastCollisionObjects[obj] = -1;
    objectClass = aeObjectClass[obj];

    if (objectClass >= OBJECT_CLASS_MISSILE) {
        asObjectViewFrames[obj] = 0;
        acShipTargetTable[obj] = -1;
        if (objectClass >= OBJECT_CLASS_SHIP) {
            value = typeData->shieldFore;
            aasShipShield[obj][0] = value;
            aasShipMaximumShieldTable[obj][0] = value;
            value = typeData->shieldAft;
            aasShipShield[obj][1] = value;
            aasShipMaximumShieldTable[obj][1] = value;
            aasShipArmor[obj][0] = typeData->armorFront;
            aasShipArmor[obj][2] = typeData->armorLeft;
            aasShipArmor[obj][3] = typeData->armorRight;
            aasShipArmor[obj][1] = typeData->armorRear;
            anShipFuelsTable[obj] = typeData->lifetime;
            acShipIonDriveDmg[obj] = 0;
            acShipDmg[obj] = 0;
            recalc_max_velocity(obj);
            acPilotHitPointsArray[obj] = 4;
            memcpy(aShipWeaponsTable[obj], typeData->weaponLoadout,
                   sizeof(typeData->weaponLoadout));
#ifdef SDL_PORT
            if (SdlSwcFlightActive() && obj >= 0 && obj < 10 && type >= 0 && type < 22)
                memcpy(aSwcWeaponAmmo[obj], aSwcInitialWeaponAmmo[type],
                        sizeof(aSwcWeaponAmmo[obj]));
#endif

            if (obj == 0) {
                nSelectedReleaseWeaponIndex = -1;
                eSelectedGunType = (enum ObjectType)-1;
                for (weapon = aShipWeaponsTable[obj][0];
                     weapon-- > 0;) {
                    if (((ShipWeaponSlot *)&aShipWeaponsTable[obj][1])[weapon]
                            .disabled == 0) {
                        if (aObjectTypeData[((ShipWeaponSlot *)
                                &aShipWeaponsTable[obj][1])[weapon].type]
                                .objectClass == OBJECT_CLASS_PROJECTILE)
                            eSelectedGunType = ((ShipWeaponSlot *)
                                &aShipWeaponsTable[obj][1])[weapon].type;
                        else
                            nSelectedReleaseWeaponIndex = weapon;
                    }
                }
            }
            acPreviousAttackers[obj] = -1;
            asShipWpnEnergyList[obj] = 100;
        }
        return;
    }

    if (typeData->animation == 0) {
        asObjectViewFrames[obj] = typeData->yawRate;
        return;
    }
    asObjectAnimationDelay[obj] = 1;
    asObjectAnimIndexes[obj] = 0;
    animate_shape(obj);
}

/* Function start: 0x41E400 */
unsigned int match_rotation_goal(short *rotation, short *goal,
                                 short totalError, short rate)
{
    short step;

    if (totalError != 0) {
        if (*goal > 180)
            *goal = *goal - 360;
        if (*goal < -180)
            *goal = *goal + 360;
        step = MaxShort(1, (abs(*rotation - *goal) *
                                  rate / totalError));
        if (*goal != *rotation || step < abs(*rotation)) {
            if (*goal > 0) {
                *rotation = (*rotation +
                    MaxShort(
                        MinShort(
                            (MinShort(*goal, step) - *rotation),
                            step),
                        -step));
            } else {
                *rotation = (*rotation +
                    MinShort(
                        MaxShort(
                            (MaxShort(*goal, -step) -
                                    *rotation),
                            -step),
                        step));
            }
        }
    }
    if (*goal != 0) {
        if (*goal > 0)
            *goal = MaxShort((*goal - *rotation), 0);
        else
            *goal = MinShort((*goal - *rotation), 0);
    }
    return 0;
}

/* Function start: 0x41E520 */
int rotate_object_to_goal(short obj)
{
    ObjectTypeData *typeData;
    short totalError;

    typeData = &aObjectTypeData[aeObjectTypeList[obj]];
    if (aeSpecialManeuvers[obj] ==
            SPECIAL_MANEUVER_BLOWING_UP) {
        if ((short)alert_flag(obj, 1) != 0) {
            set_special(obj, SPECIAL_MANEUVER_NONE);
        } else {
            if (asObjectCounterArray[obj] == -1 &&
                skill_check(obj, 7) != 0)
                aeSpecialManeuvers[obj] =
                    SPECIAL_MANEUVER_NONE;
            return 0;
        }
    }
    totalError = (abs(anObjectYawRotation[obj] -
                            anYawTarget[obj]) +
                         abs(anObjPitchRot[obj] -
                             anPitchGoal[obj]) +
                         abs(anObjectRollRotationList[obj] -
                             anRollGoal[obj]));
    match_rotation_goal(&anObjPitchRot[obj],
                        &anPitchGoal[obj], totalError,
                        typeData->yawRate);
    match_rotation_goal(&anObjectYawRotation[obj],
                        &anYawTarget[obj], totalError,
                        typeData->pitchRate);
    match_rotation_goal(&anObjectRollRotationList[obj],
                        &anRollGoal[obj], totalError,
                        typeData->rollRate);
}

/* Function start: 0x41E650 */
unsigned int check_for_lost_control(short obj)
{
    short overshoot;
    short turnRate;

    if (obj != 0 && aeSpecialManeuvers[obj] !=
                        SPECIAL_MANEUVER_UNKNOWN_9) {
        turnRate = aObjectTypeData[aeObjectTypeList[obj]].pitchRate;
        turnRate = (aObjectTypeData[aeObjectTypeList[obj]].yawRate +
                           turnRate);
        turnRate = (aObjectTypeData[aeObjectTypeList[obj]].rollRate +
                           turnRate);
        overshoot = (abs(anObjPitchRot[obj]) +
                            abs(anObjectRollRotationList[obj]) +
                            abs(anObjectYawRotation[obj]));
        overshoot = (overshoot / turnRate);
        if (skill_check(obj, overshoot) == 0) {
            set_special(obj, SPECIAL_MANEUVER_BLOWING_UP);
            asObjectCounterArray[obj] =
                (RandomBelowOrEqual(6) + 5);
        }
    }
    return 0;
}

/* Function start: 0x41E710 */
unsigned int celerate(short ship, int delta)
{
    int maximumSpeed = asShipMaximumSpeedArray[ship] << 8;
    int speed;

    speed = anShipSpeed[ship] + delta;
    anShipSpeed[ship] = speed;
    if (speed > maximumSpeed)
        anShipSpeed[ship] = maximumSpeed;
    if (anShipSpeed[ship] < 0)
        anShipSpeed[ship] = 0;
    return 0;
}

/* Function start: 0x41E750 */
unsigned int approach_speed(short ship, int targetSpeed)
{
    int delta;
    int acceleration;

    acceleration = GetShipAccelerationRate(ship);
    delta = targetSpeed - anShipSpeed[ship];

    if ((short)alert_flag(ship, 1))
        acceleration += acceleration;
    if ((delta < 0 ? -delta : delta) > acceleration)
        delta = MultiplyFixed(SignFixed(delta), acceleration);
    celerate(ship, delta);
    return 0;
}

/* Function start: 0x41E7C0 */
unsigned int steady_object(short ship)
{
    anYawTarget[ship] = 0;
    anPitchGoal[ship] = 0;
    anRollGoal[ship] = 0;
    return 0;
}

/* Function start: 0x41E7F0 */
short real_velocity(short obj)
{
    return FixedToShortSaturating(
        Vector_magnitude(&aShipVelocity[obj]));
}

/* Function start: 0x41E820 */
unsigned int fix_velocity(short obj)
{
    ScaleFixedVector(&aShipFwdVec[obj],
                     anShipSpeed[obj],
                     &aShipVelocity[obj]);
    return 0;
}

/* Function start: 0x41E860 */
unsigned int sort_viable_target_list(void)
{
    short outer;
    signed char target;
    short innerDistance;
    short count;
    short inner;

    if (cViableTargetCount > 1) {
        count = cViableTargetCount;
        for (outer = 0; outer < count - 1; outer++) {
            for (inner = outer + 1; inner < count; inner++) {
                short distance;
                int outerIndex;

                outerIndex = outer;
                distance = asViableTgtDistList[outerIndex];
                innerDistance = asViableTgtDistList[inner];
                if (distance > innerDistance) {
                    target = acViableTargetList[outerIndex];
                    asViableTgtDistList[outerIndex] = innerDistance;
                    acViableTargetList[outerIndex] =
                        acViableTargetList[inner];
                    asViableTgtDistList[inner] = distance;
                    acViableTargetList[inner] = target;
                }
            }
        }
    }
    return 0;
}

/* Function start: 0x41E900 */
void send_appropriate_message(short attacker, short victim)
{
    if (aeObjectClass[attacker] >= OBJECT_CLASS_SHIP) {
        if (nYourWingman != -1 &&
            (short)acObjOwnersTable[attacker] ==
                nYourWingman &&
            nYourWingman != attacker &&
            aeShipSide[victim] == SIDE_KILRATHI) {
            if (RandomBelowOrEqual(100) < 50 &&
                aeSpecialManeuvers[attacker] !=
                    SPECIAL_MANEUVER_UNKNOWN_9)
                send_message(nYourWingman, 5);
#ifdef SDL_PORT
        /* An unowned ship has owner -1.  The original indexes side[-1], which
           aliases the final roll-goal words and normally compares false. */
        } else if (acObjOwnersTable[attacker] != -1 &&
#else
        } else if (
#endif
                   aeShipSide[
                       (short)acObjOwnersTable[attacker]] ==
                       SIDE_KILRATHI &&
                   nYourWingman == victim) {
            send_message(acObjOwnersTable[attacker], 5);
        }
    }
}

/* Function start: 0x41E9B0 */
int inflict_damage(short attacker, short victim, short damage,
                   const FixedVector *impactDirection)
{
    short *strength;
    short quadrant;
    short destroyed;
    int sideDot;

    if (bPlayerVulnerable == 0 && victim == 0)
        return 0;
    if (damage == 0 ||
        aeSpecialManeuvers[victim] ==
            SPECIAL_MANEUVER_UNKNOWN_9 ||
        aeObjectClass[victim] < OBJECT_CLASS_MISSILE)
        return 0;

    if (aeObjectClass[victim] < OBJECT_CLASS_SHIP) {
        asShipAccumulatedDmg[victim] = (
            asShipAccumulatedDmg[victim] + damage);
        if (aObjectTypeData[
                aeObjectTypeList[victim]].damageCapacity == -1)
            return 0;
        if (aObjectTypeData[
                aeObjectTypeList[victim]].damageCapacity <=
            asShipAccumulatedDmg[victim])
            return explode(attacker, victim);
#ifdef SDL_PORT
        return 0;
#else
        /* The original survivor path returns without clearing EAX. */
        return;
#endif
    } else {
        if (victim == 0) {
            TriggerPlayerHitPaletteFlash();
#ifdef SDL_PORT
            SdlQueueJoystickDamageRumble(damage);
#endif
        }

        if (attacker != -1 &&
            nYourWingman == victim &&
            acObjOwnersTable[attacker] == 0)
            send_message(victim, 10);

        quadrant = (dot_product(impactDirection,
            &aShipFwdVec[victim]) > 0);
        strength = &aasShipShield[victim][quadrant];
        damage = (damage - *strength);
        if (damage > 0) {
            *strength = 0;
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
            strength = &aasShipArmor[victim][quadrant];
            damage = (damage - *strength);
            if (damage > 0) {
                *strength = 0;

                if (asObjectScreenXList[victim] !=
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
            *strength = -damage;
        } else {
            *strength = -damage;
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
    if (acPilotHitPointsArray[obj] > 0) {
        acPilotHitPointsArray[obj]--;
        if (acPilotHitPointsArray[obj] == 0) {
            if (obj == 0) {
                if (bPlayerVulnerable != 0)
                    nArcadeState = 4;
                return -1;
            }
        } else if (skill_check(obj, 9) == 0 && obj != 0) {
            asObjectCounterArray[obj] =
                (RandomBelowOrEqual(20) + 30);
            set_special(obj, SPECIAL_MANEUVER_BLOWING_UP);
        }
    }
    return acPilotHitPointsArray[obj];
}

/* Function start: 0x41ECE0 */
int onboard_explosion(short obj)
{
    short debris;
    FixedVector offset;

    debris = find_vacant_3d_object();
    if (debris != -1) {
        set_objects_data(debris, OBJECT_TYPE_EXPLOSION2, obj);
        awObjectScales[debris] <<= 2;
        aShipVelocity[debris] = aShipVelocity[obj];
        asObjectCounterArray[debris] = 6;
        MakeRandomVectorFixed(
            (asObjectCollisionRadius[obj] >> 2),
            (asObjectCollisionRadius[obj] >> 1),
            &offset);
        AddFixedVectors(&aShipPositionsArray[obj], &offset,
                        &aShipPositionsArray[debris]);
    }
    return debris != -1;
}

/* Function start: 0x41EDB0 */
void call_enemy(short obj)
{
    int otherIndex;
    short other;

    other = 0;
    for (; other < 10; other++) {
        otherIndex = other;
        if (aeObjectClass[otherIndex] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuvers[otherIndex] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            aeShipSide[obj] != aeShipSide[otherIndex] &&
            RandomBelow(100) < 50)
            acShipTargetTable[otherIndex] = obj;
    }
}

/* Function start: 0x41EE20 */
int internal_damage(short attacker, short victim, short damage,
                    short quadrant)
{
    enum ObjectType type;
    short damageCapacity;
    int victimIndex;
    short system;
    signed char weaponCount;

    if (victim == 0)
        return your_internal_damage(attacker, damage, quadrant);
    victimIndex = victim;
    type = aeObjectTypeList[victimIndex];
    damageCapacity = aObjectTypeData[type].damageCapacity;
    if (aeObjectClass[victimIndex] == OBJECT_CLASS_CAPITAL_SHIP) {
        if (aeShipSide[victimIndex] == SIDE_KILRATHI) {
            asShipAccumulatedDmg[victimIndex] = (
                asShipAccumulatedDmg[victimIndex] +
                MaxShort(1, (damage >> 3)));
            if (attacker != -1 && attacker < 10 &&
                any_enemy(attacker, 10000) == 0)
                call_enemy(attacker);
        } else {
            asShipAccumulatedDmg[victimIndex] = (
                asShipAccumulatedDmg[victimIndex] +
                MaxShort(1, (damage / 10)));
            if (RandomBelowOrEqual(1000) < 35 && attacker != 0)
                send_message(victim, 4);
        }
        if (asShipAccumulatedDmg[victimIndex] >= damageCapacity)
            return explode(attacker, victim);
        else
            onboard_explosion(victim);
        return 0;
    }

    if (acShipRating[victimIndex] != -1) {
        damage = MaxShort(1, (damage / 40));
        damage = MinShort(RandomInRange(3, 4), damage);
    } else {
        damage = MaxShort(1, (damage / 6));
    }
    asShipAccumulatedDmg[victimIndex] = (
        asShipAccumulatedDmg[victimIndex] + damage);

    while (damage > 0) {
        if (damage == 1 && acShipRating[victimIndex] != -1)
            system = 4;
        else
            system = RandomBelowOrEqual(9);
        switch (system) {
        case 0:
            damage--;
            pilot_hit(victim);
            break;
        case 1:
            if (quadrant == 1) {
                damage--;
                damage_ion_drive(victim, 1, 3);
            }
            break;
        case 2:
            if (quadrant == 1)
                return explode(attacker, victim);
            break;
        case 3:
            aasShipMaximumShieldTable[victimIndex][0] = 0;
            aasShipShield[victimIndex][0] = 0;
            aasShipMaximumShieldTable[victimIndex][1] = 0;
            aasShipShield[victimIndex][1] = 0;
            break;
        case 4:
            damage--;
            acShipDmg[victimIndex]++;
            if ((short)acShipDmg[victimIndex] >
                damageCapacity)
                return explode(attacker, victim);
            break;
        case 5:
            if (quadrant == 0) {
                weaponCount = aShipWeaponsTable[victimIndex][0];
                if (weaponCount > 0) {
                    system = (weaponCount - 1);
                    damage--;
                    remove_weapon(victim,
                        (signed char)RandomBelowOrEqual(system));
                }
            }
            break;
        case 6:
            if (quadrant == 0 &&
                acShipDestroyedWeaponCountArray[victimIndex] < 5) {
                damage--;
                acShipDestroyedWeaponCountArray[victimIndex]++;
            }
            break;
        case 7:
            if (quadrant == 1) {
                damage--;
                drain_fuel(victim,
                           (aObjectTypeData[
                                       type].lifetime / 4));
                if (RandomBelowOrEqual(1) != 0 ||
                    anShipFuelsTable[victimIndex] < 0)
                    return explode(attacker, victim);
            }
            break;
        case 8:
            if (quadrant == 0 &&
                acShipCommunicatorArray[victimIndex] != -1) {
                acShipCommunicatorArray[victimIndex] = -1;
                damage--;
            }
            break;
        }
    }
#ifdef SDL_PORT
    return 0;
#else
    /* The original leaves the return register unset on this path. */
    return;
#endif
}

/* Function start: 0x41F1A0 */
void revise_shields(short obj)
{
    aasShipMaximumShieldTable[obj][0] =
        MaxShort(0, (aasShipMaximumShieldTable[obj][0] -
            (aObjectTypeData[
                aeObjectTypeList[obj]].shieldFore >> 2)));
    aasShipMaximumShieldTable[obj][1] =
        MaxShort(0, (aasShipMaximumShieldTable[obj][0] -
            (aObjectTypeData[
                aeObjectTypeList[obj]].shieldAft >> 2)));
}

#ifdef SDL_PORT
/* Mac last_gun, CODE_05 +0x16e6: missiles do not count as spare guns. */
static short SwcIsOnlyShipGun(short ship, short weapon)
{
    ShipWeaponSlot *weapons = (ShipWeaponSlot *)&aShipWeaponsTable[ship][1];
    short candidate;

    if (aObjectTypeData[weapons[weapon].type].objectClass !=
        OBJECT_CLASS_PROJECTILE)
        return 0;
    for (candidate = 0; candidate < aShipWeaponsTable[ship][0]; candidate++) {
        if (candidate != weapon &&
            aObjectTypeData[weapons[candidate].type].objectClass ==
                OBJECT_CLASS_PROJECTILE)
            return 0;
    }
    return 1;
}
#endif

/* Function start: 0x41F220 */
int your_internal_damage(short attacker, short damage, short quadrant)
{
    enum ObjectType playerType;
    short tableGroup;
    short events;
    signed char severity;

    if (aeObjectClass[attacker] == OBJECT_CLASS_PROJECTILE) {
        tableGroup = quadrant == 1 ? 2 : 0;
        events = MaxShort(1, (damage >> 4));
    } else if (aeObjectClass[attacker] == OBJECT_CLASS_ASTEROID ||
               aeObjectClass[attacker] >= OBJECT_CLASS_SHIP) {
        tableGroup = 4;
        events = MaxShort(1, (damage >> 7));
    } else {
        tableGroup = quadrant == 1 ? 3 : 1;
        events = MaxShort(1, (damage >> 5));
    }
    playerType = aeObjectTypeList[0];
    /* Mac CODE_05 +0x179e uses exclusive random_number bounds. */
    severity = RandomBelowOrEqual(
#ifdef SDL_PORT
        SdlSwcFlightActive() ? 9 :
#endif
        10);
    asShipAccumulatedDmg[0] = (
        asShipAccumulatedDmg[0] + events);
    if (events > 1)
#ifdef SDL_PORT
        /* SWC CODE_05 +0x184a uses random_number(3), upper bound exclusive:
           CODE_04 dp_random +0x5d44. Its cockpit has three damage regions. */
        if (SdlSwcFlightActive())
            place_damage_on_cockpit(RandomBelowOrEqual(2));
        else
#endif
        place_damage_on_cockpit(RandomBelowOrEqual(3));

    while (events > 0) {
        events--;
        switch ((short)(signed char)asPlayerDamageSystemTable[
                tableGroup * 10 + RandomBelowOrEqual(
#ifdef SDL_PORT
                    SdlSwcFlightActive() ? 8 :
#endif
                    9)]) {
        case 0:
            if (severity < 4)
                pilot_hit(0);
            else if (severity < 7)
                damage_your_component(7, 2, 4);
            else
                damage_your_component(6, 4, 4);
            break;
        case 1:
            if (quadrant != 1) {
                events++;
                break;
            }
            damage_your_component(0, 1, 3);
            damage_ion_drive(0, 1, 3);
            break;
        case 2:
            if (quadrant != 1) {
                events++;
                break;
            }
            if (RandomBelowOrEqual(
#ifdef SDL_PORT
                SdlSwcFlightActive() ? 2 :
#endif
                3) == 0)
                return explode(attacker, 0);
            if (damage_your_component(1, 1, 4) == 4)
                return explode(attacker, 0);
            break;
        case 3:
            if (severity < 9) {
                damage_your_component(2, 1, 4);
                revise_shields(0);
            } else
                damage_your_component(8, 2, 4);
            break;
        case 4:
            acShipDmg[0]++;
            if (acShipDmg[0] == 1) {
                events--;
                pilot_hit(0);
            } else if ((short)acShipDmg[0] >
                       aObjectTypeData[playerType].
                           damageCapacity) {
                return explode(attacker, 0);
            }
            break;
        case 5:
            if (quadrant != 0) {
                events++;
            } else {
                if (aShipWeaponsTable[0][0] > 0) {
#ifdef SDL_PORT
                    if (SdlSwcFlightActive()) {
                        short weapon = RandomBelowOrEqual(
                            MaxShort(0, aShipWeaponsTable[0][0] - 2));

                        if (SwcIsOnlyShipGun(0, weapon))
                            break;
                        remove_weapon(0, weapon);
                    } else
#endif
                    remove_weapon(0,
                        (signed char)RandomBelowOrEqual(
                            (aShipWeaponsTable[0][0] - 1)));
                    ShowComponentHitHudMessage(
                        _("Weapon destroyed"),
                        cRedColour, 8);
                }
            }
            break;
        case 6:
            if (quadrant != 0)
                events++;
            else if (acShipDestroyedWeaponCountArray[0] < 5) {
                    acShipDestroyedWeaponCountArray[0]++;
                    check_computer_damage();
                }
            break;
        case 7:
            drain_fuel(0,
                       (aObjectTypeData[
                                   playerType].lifetime / 4));
            if (RandomBelowOrEqual(
#ifdef SDL_PORT
                SdlSwcFlightActive() ? 0 :
#endif
                1) != 0 ||
                anShipFuelsTable[0] < 0)
                return explode(attacker, 0);
            ShowComponentHitHudMessage(_("Fuel tanks hit"),
                                       cRedColour, 8);
            break;
        case 8:
            if (quadrant != 0) {
                events++;
            } else {
                if (severity < 7) {
                    damage_your_component(4, 2, 3);
                    if (acPlayerComponentDamageList[4] >= 4)
                        acShipCommunicatorArray[0] = -1;
                } else
                    damage_your_component(5, 4, 4);
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

    if (minimumDamage <
        (short)acPlayerComponentDamageList[component]) {
        acPlayerComponentDamageList[component] =
            (acPlayerComponentDamageList[component] - 1);
        sprintf(message, _("%s FIXD"),
                _(apszComponentNames[component]));
        ShowComponentHitHudMessage(message, cRedColour, 8);
        return 1;
    }
    return 0;
}

/* Function start: 0x41F660 */
void repair_internal_damage(void)
{
    short repair;
#ifndef SDL_PORT
    short component;
#endif

#ifndef SDL_PORT
    if ((short)acPlayerComponentDamageList[component] >= 4)
        return;
#endif
    if (RandomBelowOrEqual(
#ifdef SDL_PORT
        SdlSwcFlightActive() ? 499 :
#endif
        500) >= 2)
        return;
    repair = RandomBelowOrEqual(
#ifdef SDL_PORT
        SdlSwcFlightActive() ? 1 :
#endif
        2);
    switch (repair) {
    case 0:
#ifdef SDL_PORT
        /* The Mac body guards the selected component in each repair case.
           Retail Win32 instead reads an uninitialized component above. */
        if (acPlayerComponentDamageList[2] >= 4)
            break;
#endif
        ReportComponentRepaired(2, 1);
        break;
    case 1:
#ifdef SDL_PORT
        if (acPlayerComponentDamageList[0] >= 4)
            break;
#endif
        if (ReportComponentRepaired(0, 2) != 0)
            damage_ion_drive(0, -1, 3);
        break;
    case 2:
        if (aObjectTypeData[
                aeObjectTypeList[0]].damageCapacity - 3 <
            (short)acShipDmg[0])
            acShipDmg[0]--;
        break;
    }
}

/* Function start: 0x41F700 */
void Create_ship_hit_debris(short obj, short count)
{
    FixedVector offset;
    short created;
    short debris;

    created = 0;
    if (created >= count)
        return;
    do {
        debris = find_vacant_3d_object();
        if (debris == -1)
            return;
        set_objects_data(debris,
            aeShipHitDebrisTypes[
                RandomBelowOrEqual(2)], -1);
        asObjectCounterArray[debris] = 40;
        FillFixedVectorWithRandomComponents(10, &offset);
        AddFixedVectors(&aShipPositionsArray[obj], &offset,
                        &aShipPositionsArray[debris]);
        FillFixedVectorWithRandomComponents(
            6, &aShipVelocity[debris]);
        created++;
    } while (created < count);
}

/* Function start: 0x41F7C0 */
void check_next_wave(void)
{
    short obj;

    if (nCurrentWave == -1)
        return;
    for (obj = 0; obj < 10; obj++) {
        if (aeObjectClass[obj] == OBJECT_CLASS_SHIP &&
            aeShipSide[obj] == SIDE_KILRATHI)
            return;
    }
    set_up_next_wave();
}

/* Function start: 0x41F800 */
unsigned int Create_explosion_debris(short obj)
{
    FixedVector vector;
    short debris;
    short index;
    short set;
#ifdef SDL_PORT
    enum ObjectType wreckage = (enum ObjectType)(OBJECT_TYPE_SWC_WRECKAGE_BASE + aeObjectTypeList[obj]);
#endif

    remove_object(obj);
    check_next_wave();
#ifdef SDL_PORT
    /* Mac Create_explosion_debris +0x1dde emits the ship's DBRS wreck
       before the common debris and dust. */
    if (SdlSwcFlightActive() && wreckage >= OBJECT_TYPE_SWC_WRECKAGE_BASE &&
        wreckage < OBJECT_TYPE_COUNT) {
        debris = find_vacant_3d_object();
        if (debris != -1) {
            set_objects_data(debris, wreckage, -1);
            asObjectCounterArray[debris] = 40;
            FillFixedVectorWithRandomComponents(1, &vector);
            AddFixedVectors(&aShipPositionsArray[obj], &vector, &aShipPositionsArray[debris]);
            FillFixedVectorWithRandomComponents(5, &aShipVelocity[debris]);
            ScaleFixedVector(&aShipVelocity[obj], 435, &vector);
            AddFixedVectors(&vector, &aShipVelocity[debris], &aShipVelocity[debris]);
        }
    }
#endif
    set = RandomBelowOrEqual(
#ifdef SDL_PORT
        SdlSwcFlightActive() ? 2 :
#endif
        3);
    for (index = 0; index < 7; index++) {
        debris = find_vacant_3d_object();
        if (debris == -1)
            break;
        set_objects_data(debris,
                         aaeExplosionDebris[set][index], -1);
        asObjectCounterArray[debris] = 40;
        FillFixedVectorWithRandomComponents(50, &vector);
        AddFixedVectors(&aShipPositionsArray[obj], &vector,
                        &aShipPositionsArray[debris]);
        FillFixedVectorWithRandomComponents(
            25, &aShipVelocity[debris]);
        divide_vector(&aShipVelocity[obj], 0x200, &vector);
        AddFixedVectors(&vector,
                        &aShipVelocity[debris],
                        &aShipVelocity[debris]);
    }
    for (index = 0; index < 8; index++) {
        debris = find_vacant_3d_object();
        if (debris == -1)
            break;
        FillFixedVectorWithRandomComponents(50, &vector);
        AddFixedVectors(&aShipPositionsArray[obj], &vector,
                        &aShipPositionsArray[debris]);
        FillFixedVectorWithRandomComponents(
            25, &aShipVelocity[debris]);
        divide_vector(&aShipVelocity[obj], 0x200, &vector);
        AddFixedVectors(&vector,
                        &aShipVelocity[debris],
                        &aShipVelocity[debris]);
        asObjScreenAngles[debris] =
            (RandomBelowOrEqual(
#ifdef SDL_PORT
                SdlSwcFlightActive() ? 2 :
#endif
                3) + 0x10);
        asObjectCounterArray[debris] = 40;
        aeObjectClass[debris] = OBJECT_CLASS_DUST;
        aeObjectTypeList[debris] = OBJECT_TYPE_DEBRIS_DUST;
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
        score = (amount * 2);
        break;
    default:
        score = amount;
        break;
    }
    stCampaignState.missionScore =
        (stCampaignState.missionScore + score);
    if (pilot == 0) {
        nMissionMedalScoreValue =
            (nMissionMedalScoreValue + score);
        nCurrentArcadeScore += score * 10;
    }
    return 0;
}

/* Function start: 0x41FA90 */
unsigned int score_for_kill(short pilot, short victim)
{
    int event;

    if (aeShipSide[victim] == SIDE_KILRATHI) {
        switch (aeObjectTypeList[victim]) {
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
            nPlayerKillTotal++;
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

    originalScale = awObjectScales[obj];
    explosion = find_vacant_3d_object();
    if (explosion == -1) {
        if (aeObjectClass[obj] ==
            OBJECT_CLASS_CAPITAL_SHIP)
            FreePacketAndClear(&apObjShapeArray[obj], 0);
        asCapitalShipViewFramesTable[obj] = -1;
        explosion = obj;
    } else {
        copy_frame(obj, explosion);
        aShipPositionsArray[explosion] =
            aShipPositionsArray[obj];
        aShipVelocity[explosion] =
            aShipVelocity[obj];
        acObjOwnersTable[explosion] = obj;
    }
#ifdef SDL_PORT
    /* SWC CODE_05 ShipExplosion +0x2302: fighters use type 50; only
       capital-ship type 49 carries the final-frame glare trigger. */
    if (SdlSwcFlightActive() && aeObjectClass[obj] < OBJECT_CLASS_CAPITAL_SHIP)
        set_objects_data(explosion, OBJECT_TYPE_EXPLOSION2,
                         acObjOwnersTable[explosion]);
    else
#endif
    set_objects_data(explosion, OBJECT_TYPE_EXPLOSION1,
                     acObjOwnersTable[explosion]);
    awObjectScales[explosion] = (
        awObjectScales[explosion] *
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

    objectClass = aeObjectClass[obj];
    explosion = obj;
    if (objectClass >= OBJECT_CLASS_SHIP) {
        missionShip = -1;
        if (objectClass == OBJECT_CLASS_CAPITAL_SHIP)
            missionShip = find_ship_index(obj);
        if (obj < 10 &&
            (acShipRating[obj] != -1 ||
             (missionShip != -1 &&
              anShipMissionShips[obj] == missionShip) ||
             RandomBelow(100) < 3)) {
            send_message(obj, 7);
        }
        if (acShipRating[obj] != -1 &&
            acShipRating[obj] != RATING_ACE_ICEMAN)
            personality_killed(acShipRating[obj]);
        if (nYourWingman == obj) {
            nCurrentWingmanKilledThisMission = 1;
            missionShip = 0;
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
        asObjectCounterArray[obj] = 8;
        aMissionShips[
            nShipMissionIndices[obj]].state = 3;
        if (aeObjectClass[obj] ==
            OBJECT_CLASS_CAPITAL_SHIP) {
            short count = 4;

            for (; count != 0; count--) {
                onboard_explosion(obj);
            }
            asObjectCounterArray[obj] =
                (short)(aObjectTypeData[
                    aeObjectTypeList[obj]].damageCapacity >> 2) + 8;
        } else {
            explosion = ShipExplosion(obj);
            explosion_shock_wave(
                obj, aObjectTypeData[
                    aeObjectTypeList[obj]].explosionDamage);
        }
    } else {
#ifdef SDL_PORT
        /* Mac Explosion +0x2564: substantial wreckage explodes at its
           squared Q8 scale; the smaller DBRS fragments simply expire. */
        if (SdlSwcFlightActive() && aeObjectTypeList[obj] >= OBJECT_TYPE_SWC_WRECKAGE_BASE) {
            unsigned int scale = awObjectScales[obj];

            explosion = obj;
            set_objects_data(obj, OBJECT_TYPE_EXPLOSION0, acObjOwnersTable[obj]);
            awObjectScales[obj] = (scale * scale) >> 8;
        } else {
#endif
        if (aeObjectTypeList[obj] == OBJECT_TYPE_TURRET ||
            objectClass == OBJECT_CLASS_ASTEROID)
            explosionType = OBJECT_TYPE_EXPLOSION0;
        else
            explosionType = OBJECT_TYPE_EXPLOSION2;
        explosion = obj;
        set_objects_data(obj, explosionType,
                         acObjOwnersTable[obj]);
        if (aeObjectClass[obj] == OBJECT_CLASS_ASTEROID)
            awObjectScales[obj] = 0x380;
#ifdef SDL_PORT
        }
#endif
        explosion_shock_wave(
            obj, aObjectTypeData[
                aeObjectTypeList[obj]].explosionDamage);
    }
    if (asObjectScreenXList[obj] != (short)0x8001)
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
        owner = acObjOwnersTable[obj];
        if (obj == owner || owner == -1)
            return obj;
        obj = owner;
    }
    return -1;
}

/* Function start: 0x41FEE0 */
int explosion_shock_wave(short obj, short blastDamage)
{
    short other;
    FixedVector delta;
    FixedVector force;
    short distance;
    short divisor;
    short damage;

    other = 0;
    for (; other < 10; other++) {
        if (other != obj &&
            aeObjectClass[other] >= OBJECT_CLASS_SHIP) {
            ComputeVectorDelta(&aShipPositionsArray[obj],
                               &aShipPositionsArray[other], &delta);
            distance = FixedToShortSaturating(
                Vector_magnitude(&delta));
            distance = MaxShort(0,
                (distance -
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
                damage = ((blastDamage / divisor) / divisor);
                if (other == 0)
                    damage = MaxShort(1,
                        (damage * 3 >> 2));
            }
            if (damage > 1) {
                NormalizeFixedVector(&delta);
                ScaleFixedVector(&delta, damage << 8, &force);
                apply_force_to_objects_center(&force, other);
                inflict_damage(the_creator(obj), other,
                               MinShort(100, damage), &delta);
            }
        }
    }
    return 0;
}

/* Function start: 0x420040 */
int explode(short attacker, short victim)
{
    if (victim < 10 && acShipRating[victim] != -1 &&
        acShipRating[victim] != RATING_ACE_ICEMAN) {
        if (acShipRating[victim] > RATING_ACE_ICEMAN) {
            if (ace_status(
                    (acShipRating[victim] -
                            RATING_ACE_ANGEL),
                    0x20) != 0) {
                unflag_ace(
                    (acShipRating[victim] -
                            RATING_ACE_ANGEL),
                    0x20);
                acShipStressList[victim] = -25;
                reset_maneuver(victim, MANEUVER_OUTA_HERE);
                acShipDmg[victim] =
                    (acShipDmg[victim] / 2);
                send_message(victim, 6);
                return 0;
            }
            if (RandomBelowOrEqual(1) == 0)
                return 0;
        } else if (RandomBelowOrEqual(1) == 0)
            return 0;
    }
#ifdef SDL_PORT
    /* Special-maneuver state exists only for the twelve ship slots.  The
       original evaluates this lookup before rejecting non-ship objects, so
       exploding a projectile in a later object slot reads the following
       ship-mission table. */
    if (victim >= 0 && victim < 12 &&
        aeSpecialManeuvers[victim] ==
            SPECIAL_MANEUVER_UNKNOWN_9 &&
        aeObjectClass[victim] >= OBJECT_CLASS_SHIP)
#else
    if (aeSpecialManeuvers[victim] ==
            SPECIAL_MANEUVER_UNKNOWN_9 &&
        aeObjectClass[victim] >= OBJECT_CLASS_SHIP)
#endif
        return 0;

    if (victim == 0) {
        if (bPlayerVulnerable == 0)
            return 0;
        bCurrentPlayerDestroyed = 1;
        nArcadeState = 4;
        return 1;
    }

    if (nExternalViewShip == victim)
        nExternalViewShip = -1;
    attacker = the_creator(attacker);
    if (attacker != -1 &&
        aeObjectClass[victim] >= OBJECT_CLASS_SHIP)
        analyze_kill(attacker, victim);
    Explosion(victim);
    return 1;
}

/* Function start: 0x420190 */
int send_at_point(short obj, FixedVector *point, short speed)
{
    ComputeVectorDelta(&aShipPositionsArray[obj], point,
                       &aShipVelocity[obj]);
    SetVectorFixedPoint(&aShipVelocity[obj],
                        speed);
    return 0;
}

/* Function start: 0x4201D0 */
short find_child_object(short parent, enum ObjectClass objectClass)
{
    short obj = 0;
    short wantedParent = parent;
    enum ObjectClass wantedClass = objectClass;

    do {
        if ((short)acObjOwnersTable[obj] == wantedParent &&
            aeObjectClass[obj] == wantedClass)
            return obj;
        obj++;
    } while (obj < SPACE_OBJECT_COUNT);
    return -1;
}

/* Function start: 0x420210 */
short find_child_ship(short parent, enum ObjectClass objectClass,
                      short target)
{
    short obj;

    for (obj = 0; obj < 10; obj++) {
        if ((short)acObjOwnersTable[obj] == parent &&
            aeObjectClass[obj] == objectClass &&
            (target == -1 ||
             acShipTargetTable[obj] == target))
            return obj;
    }
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
    ScaleFixedVector(&direction, speed << 8, &direction);
    AddFixedVectors(&direction, &aShipVelocity[child],
                    &aShipVelocity[child]);
    return 0;
}

/* Function start: 0x4202D0 */
void fire(short obj, short target)
{
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
    int minePresent = 0;
    int targetInRange;

    canFire = asObjectCounterArray[obj] <= 0;
    get_facing_range_from_object(obj, target);
    range = nTgtRange;
    closingSpeed = (((anShipSpeed[target] *
                             nTargetFacingValue) / 100) >> 8);
    fireMissile = RandomBelowOrEqual(19) == 0 &&
                  RandomBelowOrEqual(7000) > range;
    if (fireMissile &&
        find_child_ship(obj, OBJECT_CLASS_MISSILE, target) != -1)
        fireMissile = 0;
    if (find_child_object(obj, OBJECT_CLASS_MINE) != -1)
        minePresent = 1;

    for (weapon = 0; weapon < aShipWeaponsTable[obj][0]; weapon++) {
        weaponType =
            ((ShipWeaponSlot *)&aShipWeaponsTable[obj][1])[weapon].type;
        weaponData = &aObjectTypeData[weaponType];
        weaponVelocity = weaponData->maximumVelocity;
        if (closingSpeed < 0)
            weaponVelocity = (weaponVelocity + closingSpeed / 100);
        targetInRange =
            (short)(weaponData->lifetime * weaponVelocity) > range;
        shouldFire = 0;
        weaponClass = weaponData->objectClass;

        switch (weaponClass) {
        case OBJECT_CLASS_PROJECTILE:
            if (canFire && targetInRange) {
                switch (weaponType) {
                case OBJECT_TYPE_LASER_CANNON:
                    shouldFire = nFacingToTargetValue > 70;
                    break;
                case OBJECT_TYPE_NEUTRON_PARTICLE_GUN:
                    shouldFire = nFacingToTargetValue > 80;
                    break;
                case OBJECT_TYPE_MASS_DRIVER_CANNON:
                    shouldFire = nFacingToTargetValue > 85;
                    break;
                case OBJECT_TYPE_TURRET:
                    shouldFire = nFacingToTargetValue > 10;
                    break;
                }
            }
            ((ShipWeaponSlot *)&aShipWeaponsTable[obj][1])[weapon].disabled =
                !shouldFire;
            if (shouldFire) {
                firedObject = fire_weapon(obj, weapon);
                if (weaponType == OBJECT_TYPE_TURRET) {
                    ComputeVectorDelta(
                        &aShipPositionsArray[firedObject],
                        &aShipPositionsArray[target], &direction);
                    launch_object(obj, firedObject, direction,
                                  real_velocity(firedObject));
                }
            }
            break;
        case OBJECT_CLASS_MINE:
            /* The first angle test deliberately preserves the original
             * stack-local ordering recovered at 0x42056a. */
            if (!minePresent && weaponType == OBJECT_TYPE_SPACE_MINE &&
                anShipSpeed[obj] >= 0x500 &&
                velocityAngle >= 75 && range <= 2000 &&
                nFacingToTargetValue >= -50 &&
                nTargetFacingValue <= 90) {
                velocityAngle = vector_angle(
                    aShipVelocity[obj],
                    aShipVelocity[target]);
                predictionTime = (900 /
                    (short)((anShipSpeed[obj] >> 8) + 20));
                predictedSeparation = (predictionTime *
                    (short)(-20 - closingSpeed) + range);
                if (closingSpeed == -20)
                    mineTime = range;
                else
                    mineTime = (range / (closingSpeed + 20));
                shouldFire = range < 2000 && nFacingToTargetValue < -50 &&
                             nTargetFacingValue > 90 && predictedSeparation > 50;
            }
            if (shouldFire) {
                firedObject = drop_mine(
                    obj, weapon, weaponType,
                    (mineTime + 15));
                if (firedObject != -1) {
                    ScaleFixedVector(&aShipVelocity[target],
                                     mineTime, &direction);
                    interceptPoint = &aShipPositionsArray[
                        SPACE_OBJECT_COUNT - 1];
                    AddFixedVectors(&aShipPositionsArray[target],
                                    &direction, interceptPoint);
                    ComputeVectorDelta(
                        &aShipPositionsArray[firedObject],
                        interceptPoint, &direction);
                    launch_object(obj, firedObject, direction, 20);
                }
                minePresent = 1;
            }
            break;
        case OBJECT_CLASS_MISSILE:
            if (fireMissile && targetInRange) {
                switch (weaponType) {
                case OBJECT_TYPE_DUMB_FIRE_MISSILE:
                    shouldFire = nFacingToTargetValue > 97;
                    break;
                case OBJECT_TYPE_HEAT_SEEKING_MISSILE:
                    shouldFire = nFacingToTargetValue > 40 &&
                                 nTargetFacingValue < -60;
                    break;
                case OBJECT_TYPE_FF_MISSILE:
                    shouldFire = 1;
                    break;
                case OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE:
                    shouldFire = nFacingToTargetValue > 40;
                    break;
                }
            }
            if (shouldFire) {
                fire_weapon(obj, weapon);
                fireMissile = 0;
            }
            break;
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
    return vector_angle(parentFromHardpoint, parentFromTarget);
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
    lifetime = (range / projectileVelocity -
                       RandomBelowOrEqual(8) - 5);
    lifetime = MaxShort(5, lifetime);
    lifetime = MinShort(27, lifetime);
    asObjectCounterArray[explosion] = lifetime;
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
                   (RandomBelowOrEqual(radius) + speed));
#ifdef SDL_PORT
    return rnd_sign(aim) * 256;
#else
    return rnd_sign(aim) << 8;
#endif
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

    target = acShipTargetTable[obj];
    explosion = new_object(OBJECT_TYPE_EXPLOSION0, obj);
    if (explosion != -1) {
        aimRadius = MaxShort(400, (range >> 2));
        targetSpeed = (real_velocity(target) << 4);
        randomAim.x = rnd_aim(aimRadius, targetSpeed, 1500);
        randomAim.y = rnd_aim(aimRadius, targetSpeed, 1500);
        randomAim.z = rnd_aim(aimRadius, targetSpeed, 1500);
        AddFixedVectors(&aShipPositionsArray[target], &randomAim,
                        &aimPoint);
        chance = RandomBelowOrEqual(100);
        if ((asObjectCounterArray[obj] != -1 || chance >= 40) &&
            chance >= 8) {
            aShipPositionsArray[explosion] = aimPoint;
            explosion_shock_wave(
                explosion,
                aObjectTypeData[
                    OBJECT_TYPE_TURRET].explosionDamage);
            return explosion;
        }
        aShipPositionsArray[explosion] = *hardpoint;
        fire_flack(obj, explosion, range, &aimPoint);
        asObjectCounterArray[obj] = (RandomBelow(10) + 7);
    }
    return explosion;
}

/* Turret target-list callee. */
short build_target_list(short obj, short range);

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

    lastTarget = build_target_list(obj, 5000);
    if (lastTarget-- == 0)
        return 0;

    slot = (ShipWeaponSlot *)&aShipWeaponsTable[obj][1];
    for (weapon = 0;
         weapon < aShipWeaponsTable[obj][0];
         weapon++, slot++) {
        if (RandomBelowOrEqual(2) == 0) {
            position_child(obj, slot->hardpoint, &hardpoint);
            startTarget = RandomBelowOrEqual(lastTarget);
            targetIndex = startTarget;
            do {
                target = acFormationMemberLists[
                    targetIndex];
                targetHemisphere = hemisphere(
                    &aShipPositionsArray[target],
                    &aShipPositionsArray[obj], &hardpoint);
                if (slot->type != OBJECT_TYPE_TURRET) {
                    if (targetHemisphere > 50 &&
                        RandomBelowOrEqual(14) == 0) {
                        acShipTargetTable[obj] =
                            target;
                        fire_weapon(obj, weapon);
                    }
                    break;
                }
                if (targetHemisphere >= 25) {
                    acShipTargetTable[obj] = target;
                    pop_flack(obj,
                              asTargetListRangesArray[targetIndex],
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
    weaponOffset = obj * sizeof(aShipWeaponsTable[0]) +
                   weapon * sizeof(ShipWeaponSlot);
    weaponType = ((ShipWeaponSlot *)((unsigned char *)
        aShipWeaponsTable + weaponOffset + 1))->type;
    weaponClass = aObjectTypeData[weaponType].objectClass;
    if (weaponType == OBJECT_TYPE_TURRET) {
        weaponClass = OBJECT_CLASS_PROJECTILE;
        weaponType = OBJECT_TYPE_LASER_CANNON;
    }
    if (weaponClass == OBJECT_CLASS_MINE)
        return drop_mine(obj, weapon, weaponType, -1);
    if (weaponClass == OBJECT_CLASS_MISSILE)
        projectile = initialize_ship(weaponType, obj);
    else
        projectile = new_object(weaponType, obj);
    if (projectile != -1) {
        weaponData = &aObjectTypeData[weaponType];
        copy_frame(obj, projectile);
        if (weaponClass == OBJECT_CLASS_PROJECTILE) {
            asShipAccumulatedDmg[projectile] =
                weaponData->damageCapacity;
            projectileSpeed = aObjectTypeData[
                aeObjectTypeList[projectile]].maximumVelocity;
            asShipWpnEnergyList[obj] =
                (asShipWpnEnergyList[obj] -
                        weaponData->animationDelay);
        }
        child_object(
            ((ShipWeaponSlot *)((unsigned char *)aShipWeaponsTable +
                               weaponOffset + 1))->hardpoint,
            projectile, obj);
        asObjectCounterArray[projectile] =
            aObjectTypeData[weaponType].lifetime;
        vector_component_in_dir(&aShipVelocity[obj],
                                &aShipFwdVec[projectile],
                                &aShipVelocity[projectile]);
        if (weaponClass == OBJECT_CLASS_PROJECTILE) {
            range = (short)((weaponData->lifetime + 5) *
                             weaponData->maximumVelocity);
            ScaleFixedVector(&aShipFwdVec[obj],
                             range << 8, &vector);
            AddFixedVectors(&aShipPositionsArray[obj], &vector,
                            &vector);
            point_at(projectile, vector);
#ifdef SDL_PORT
            /* SWC CODE_05 +0x34c8..+0x3532 offsets the player's convergence
               point along up: 300 units for neutron/mass driver, 400 otherwise. */
            if (SdlSwcFlightActive()) {
                if (obj == 0) {
                    ScaleFixedVector(&aShipUpVector[obj],
                        (weaponType == OBJECT_TYPE_NEUTRON_PARTICLE_GUN ||
                         weaponType == OBJECT_TYPE_MASS_DRIVER_CANNON ? 300 : 400) * 256,
                        &cockpitOffset);
                    AddFixedVectors(&cockpitOffset, &vector, &vector);
                    point_at(projectile, vector);
                }
            } else
#endif
            if (bCockpitlessView != 0 && cCurrentCockpitView == 3) {
                ScaleFixedVector(&aShipUpVector[obj], 0x12200,
                                 &cockpitOffset);
                AddFixedVectors(&cockpitOffset, &vector, &vector);
                point_at(projectile, vector);
            }
        }
        ScaleFixedVector(&aShipFwdVec[projectile],
                         projectileSpeed << 8, &vector);
        AddFixedVectors(&vector, &aShipVelocity[projectile],
                        &aShipVelocity[projectile]);
        if (weaponClass == OBJECT_CLASS_MISSILE) {
            ScaleFixedVector(&aShipUpVector[obj], 0xa00,
                             &vector);
            AddFixedVectors(&vector, &aShipVelocity[projectile],
                            &aShipVelocity[projectile]);
            if (obj == 0)
                RemovePlayerReleaseWeapon(weapon);
            else
                remove_weapon(obj, weapon);
            acObjCollisionGraceTicks[projectile] = 20;
            aeSpecialManeuvers[projectile] =
                SPECIAL_MANEUVER_NONE;
            aeShipManeuversTable[projectile] = MANEUVER_NONE;
            aeShipTacticArray[projectile] = TACTIC_SIT_STILL;
            asObjectCounterArray[projectile] = 5;
            switch (weaponType) {
            case OBJECT_TYPE_DUMB_FIRE_MISSILE:
                steady_object(projectile);
                asObjectCounterArray[projectile] = 1;
                acShipTargetTable[projectile] =
                    acShipTargetTable[obj];
                anShipSpeed[projectile] =
                    get_ship_max_velocity(projectile) << 8;
                if (acShipTargetTable[projectile] != -1) {
                    ComputeVectorDelta(&aShipPositionsArray[obj],
                        &aShipPositionsArray[
                            acShipTargetTable[projectile]], &vector);
                    range = Vector_magnitude(&vector);
                    range /= get_ship_max_velocity(projectile);
                    ScaleFixedVector(&aShipVelocity[
                        acShipTargetTable[projectile]],
                        range, &vector);
                    AddFixedVectors(&aShipPositionsArray[
                        acShipTargetTable[projectile]], &vector,
                        &vector);
                    point_at(projectile, vector);
                }
                break;
            case OBJECT_TYPE_HEAT_SEEKING_MISSILE:
            case OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE:
                acShipTargetTable[projectile] =
                    acShipTargetTable[obj];
                break;
            case OBJECT_TYPE_FF_MISSILE:
                acShipTargetTable[projectile] = -1;
                asObjectCounterArray[projectile] = 15;
                break;
            }
        }
        if (obj == 0) {
            if (weaponClass == OBJECT_CLASS_PROJECTILE) {
                asObjectCounterArray[obj] =
                    acGunRefireDelay[
                        weaponType - OBJECT_TYPE_LASER_CANNON];
#ifdef SDL_PORT
                /* SWC DATA/0 +0x31f2 is {4,10,4,0}; only lasers differ. */
                if (SdlSwcFlightActive() && weaponType == OBJECT_TYPE_LASER_CANNON)
                    asObjectCounterArray[obj] = 4;
#endif
            }
        } else {
            asObjectCounterArray[obj] = 12;
        }
#ifdef SDL_PORT
        if (obj == 0)
            SdlQueueJoystickWeaponRumble(weaponType);
#endif
        switch (weaponType) {
        case OBJECT_TYPE_LASER_CANNON:
        case OBJECT_TYPE_NEUTRON_PARTICLE_GUN:
            sound = 8;
            PlaySfxWaveFileByNumber(sound, projectile, 0);
            break;
        case OBJECT_TYPE_MASS_DRIVER_CANNON:
        case OBJECT_TYPE_TURRET:
            sound = 5;
            PlaySfxWaveFileByNumber(sound, projectile, 0);
            break;
        case OBJECT_TYPE_DUMB_FIRE_MISSILE:
        case OBJECT_TYPE_HEAT_SEEKING_MISSILE:
        case OBJECT_TYPE_FF_MISSILE:
        case OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE:
            sound = 1;
            PlaySfxWaveFileByNumber(sound, projectile, 0);
            break;
        }
    }
    return projectile;
}

/* Function start: 0x421100 */
short find_weapon(short obj, enum ObjectType weaponType)
{
    short weapon;

    for (weapon = 0;
         weapon < aShipWeaponsTable[obj][0];
         weapon++) {
        ShipWeaponSlot *weaponSlot =
            &((ShipWeaponSlot *)&aShipWeaponsTable[obj][1])[weapon];

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

    slot = (ShipWeaponSlot *)&aShipWeaponsTable[ship][1];
    for (weapon = 0; weapon < aShipWeaponsTable[ship][0];
         weapon++, slot++) {
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
                            _("Need Lock"),
                            cYellowColour, 3);
                    return -1;
                }
                return fire_weapon(ship, weapon);
            }
        }
    }
    return -1;
}

/* Function start: 0x421220 */
int fire_fixed_projectile_weapon(short obj)
{
    short weapon;
    ShipWeaponSlot *slot;

    slot = (ShipWeaponSlot *)&aShipWeaponsTable[obj][1];
    for (weapon = 0; weapon < aShipWeaponsTable[obj][0]; weapon++, slot++)
        if (aObjectTypeData[slot->type].objectClass ==
                OBJECT_CLASS_PROJECTILE &&
            slot->disabled == 0)
            if (fire_weapon(obj, weapon) == -1)
                return -1;
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
        &((ShipWeaponSlot *)&aShipWeaponsTable[obj][1])[weapon];
    child_object(weaponSlot->hardpoint, mine, obj);
    if (lifetime == -1)
        lifetime = 20;
    acObjCollisionGraceTicks[mine] = lifetime;
    asObjectCounterArray[mine] = (signed char)lifetime;
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

    if (Vector_magnitude(&aShipVelocity[obj]) <
        get_ship_max_velocity(obj) * 0x500L) {
        set_special(obj, SPECIAL_MANEUVER_AFTERBURNER);
        if (aeSpecialManeuvers[obj] != SPECIAL_MANEUVER_AFTERBURNER)
            timer = 0;
        else
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

    flip = asObjectFlipArray[ship];
    if ((flip & 0x10) != 0)
        angle = (180 - angle);
    if ((flip & 0x20) != 0)
        angle = -angle;
    angle += asObjScreenAngles[ship];
    angle %= 360;
    if (angle < 0)
        angle += 360;
    return angle;
}

/* Function start: 0x421430 */
unsigned int place_exhaust_on_ships(void)
{
    short *animation;
    short frame;
    short scale;
    short object;
    short ship;

    for (ship = 0; ship < 10; ship++) {
        if (aeObjectClass[ship] >= OBJECT_CLASS_MISSILE &&
            (anShipSpeed[ship] != 0
#ifdef SDL_PORT
             || (SdlSwcFlightActive() && aeSpecialManeuvers[ship] == SPECIAL_MANEUVER_AFTERBURNER)
#endif
            ) &&
            aeSpecialManeuvers[ship] !=
                SPECIAL_MANEUVER_KILL_ENGINES &&
            asObjectScreenXList[ship] != (short)0x8001) {
            animation = (short *)aObjectTypeData[
                aeObjectTypeList[ship]].animation;
            if (animation != 0) {
                object = animation[asObjectViewFrames[ship]];
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
                        scale -= RandomInRange(0,
#ifdef SDL_PORT
                            SdlSwcFlightActive() ? 31 :
#endif
                            32);
                        if (abShipExhaustHeatList[ship] == 0)
                            scale = (scale - 32);
                        awObjectScales[object] = scale;
                        asObjDistancesTable[object] = *animation++;
                        asObjScreenAngles[object] =
                            flip_angle(ship, *animation++);
                        asObjectFlipArray[object] = 0;
                        asObjectScreenXList[object] = *animation++;
                        asObjectScreenYArray[object] = *animation++;
                        if (aeSpecialManeuvers[ship] ==
                                SPECIAL_MANEUVER_AFTERBURNER
#ifdef SDL_PORT
                            || (SdlSwcFlightActive() && aeObjectClass[ship] == OBJECT_CLASS_MISSILE &&
                                acObjOwnersTable[ship] >= 0 && acObjOwnersTable[ship] < 10 &&
                                aeShipSide[acObjOwnersTable[ship]] == SIDE_KILRATHI)
#endif
                        ) {
                            asObjectViewFrames[object] =
                                (frame * 3 +
                                        RandomInRange(0,
#ifdef SDL_PORT
                                            SdlSwcFlightActive() ? 1 :
#endif
                                            2));
                        } else {
                            asObjectViewFrames[object] =
                                (frame * 2 + 12
#ifdef SDL_PORT
                                 + (SdlSwcFlightActive() ? 0 : RandomInRange(0, 1))
#else
                                 + RandomInRange(0, 1)
#endif
                                );
                        }
                    }
                }
            }
        }
    }
    /* The original leaves EAX incidental after scanning every ship. */
}

/* Function start: 0x4215E0 */
unsigned int reposition_fixed_child_objects(void)
{
    int right;
    int up;
    long sine;
    long cosine;
    short angle;
    short parent;
    short object;
#ifdef SDL_PORT
    int fixedCosine;
    int fixedSine;
    float attachmentRight;
    float attachmentUp;
    float cosineFloat;
    float parentScreenX;
    float parentScreenY;
    float sineFloat;
#endif

    for (object = 10; object <= SPACE_LAST_MOVING_OBJECT; object++) {
        if (aeObjectClass[object] ==
                OBJECT_CLASS_FIXED_OBJECT) {
            parent = acObjOwnersTable[object];
            if (aeObjectTypeList[object] ==
                    OBJECT_TYPE_TURRET ||
                aeObjectTypeList[object] ==
                    OBJECT_TYPE_THRUSTERS) {
                angle = asObjScreenAngles[parent];
                sine = SinFixed(angle);
                cosine = CosFixed(angle);
#ifdef SDL_PORT
                if (SdlSwcFlightActive()) {
                    sine = SdlSwcCos(90 - angle);
                    cosine = SdlSwcCos(angle);
                }
#endif
                right = asObjectScreenXList[object] *
                        (unsigned short)asObjScreenScalesArray[parent];
                asObjDistancesTable[object] +=
                    asObjDistancesTable[parent];
                if ((asObjectFlipArray[parent] & 0x10) != 0)
                    right = -right;
                up = asObjectScreenYArray[object] *
                     (unsigned short)asObjScreenScalesArray[parent];
                if ((asObjectFlipArray[parent] & 0x20) != 0)
                    up = -up;
#ifdef SDL_PORT
                if (SdlSwcFlightActive()) {
                    /* Mac keeps both signed 64-bit products through each
                       Q8 shift; rounding toward zero moves some anchors. */
                    asObjectScreenXList[object] =
                        ((((int64_t)right * cosine >> 8) - ((int64_t)up * sine >> 8)) >> 8);
                    asObjectScreenYArray[object] =
                        ((((int64_t)up * cosine >> 8) + ((int64_t)right * sine >> 8)) >> 8);
                } else {
#endif
                asObjectScreenXList[object] = (
                    (MultiplyFixed(right, cosine) -
                     MultiplyFixed(up, sine)) >> 8);
                asObjectScreenYArray[object] = (
                    (MultiplyFixed(up, cosine) +
                     MultiplyFixed(right, sine)) >> 8);
#ifdef SDL_PORT
                }
#endif
                asObjectScreenXList[object] +=
                    asObjectScreenXList[parent];
                asObjectScreenYArray[object] +=
                    asObjectScreenYArray[parent];
#ifdef SDL_PORT
                if (aeObjectTypeList[object] ==
                        OBJECT_TYPE_THRUSTERS && !SdlSwcFlightActive()) {
                    /* Match the anchor to the enhanced parent transform. */
                    parentScreenX =
                        (float)nCurrentViewCenterPosX +
                        (float)(((double)(nScreenWidth & ~1) *
                                 0.5 *
                                 aObjectViewPositionsList[parent].x) /
                                aObjectViewPositionsList[parent].z);
                    parentScreenY =
                        (float)nCurrentViewCenterPosY +
                        (float)(((double)(nScreenWidth & ~1) *
                                 0.5 *
                                 aObjectViewPositionsList[parent].y) /
                                aObjectViewPositionsList[parent].z);
                    GetRLETransformTrig(angle * 10,
                                        &fixedCosine, &fixedSine);
                    cosineFloat = (float)fixedCosine / 65536.0f;
                    sineFloat = (float)fixedSine / 65536.0f;
                    attachmentRight = (float)right / 256.0f;
                    attachmentUp = (float)up / 256.0f;
                    SdlSetThrusterScreenPosition(
                        object,
                        parentScreenX + attachmentRight * cosineFloat -
                            attachmentUp * sineFloat,
                        parentScreenY + attachmentRight * sineFloat +
                            attachmentUp * cosineFloat);
                }
#endif
            }
            asObjScreenScalesArray[object] = (
                (unsigned short)
                    asObjScreenScalesArray[parent] *
                
                    awObjectScales[object] >> 8);
        }
    }
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

    if (ship == 0 && acPlayerComponentDamageList[1] > 0 &&
        nCurrentSpaceFrameValue %
            (acPlayerComponentDamageList[1] + 1) != 0)
        return 0;
    for (shield = 0; shield <= 1; shield++) {
        if (aasShipShield[ship][shield] >
                aasShipMaximumShieldTable[ship][shield])
            aasShipShield[ship][shield] =
                aasShipMaximumShieldTable[ship][shield];
        if (aasShipShield[ship][shield] <
                aasShipMaximumShieldTable[ship][shield] &&
            nCurrentSpaceFrameValue %
                aObjectTypeData[aeObjectTypeList[ship]].animationDelay == 0)
            aasShipShield[ship][shield]++;
    }
    return 0;
}

/* Function start: 0x421830 */
unsigned int replenish_weapon_energy_bank(short ship)
{
    short energy;
    short shieldEnergy;
    short maximumShield;

    if (ship == 0 && acPlayerComponentDamageList[1] != 0 &&
        (unsigned short)RandomInRange(0, 4) <
            acPlayerComponentDamageList[1])
        return 0;
    energy = asShipWpnEnergyList[ship];
    if (energy < 100) {
        shieldEnergy = aasShipShield[ship][0];
        maximumShield = aasShipMaximumShieldTable[ship][0];
        shieldEnergy =
            (shieldEnergy + aasShipShield[ship][1]);
        maximumShield =
            (maximumShield +
                    aasShipMaximumShieldTable[ship][1]);
        if (shieldEnergy < maximumShield) {
            asShipWpnEnergyList[ship] =
                MinShort((energy + 1), 100);
        } else {
            asShipWpnEnergyList[ship] =
                MinShort((energy + 2), 100);
        }
    }
    return 0;
}

/* Function start: 0x4218D0 */
void accelerate(short amount)
{
    if (malf(0) != 0) {
        amount = (amount - 2);
        if (nCurrentSpaceFrameValue % 3 == 0)
            PlaySfxWaveFileByNumber(3, -1, 0);
    }
#ifdef SDL_PORT
    /* Negative throttle steps are valid; shifting them is undefined in C. */
    celerate(0, amount * 256);
#else
    celerate(0, amount << 8);
#endif
}

/* Function start: 0x421920 */
void your_afterburner(void)
{
    short time;

    if (anShipFuelsTable[0] <= 0)
        return;
    if (malf(0) != 0) {
        PlaySfxWaveFileByNumber(3, -1, 0);
        return;
    }
    if (aeSpecialManeuvers[0] !=
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
    if (nCurrentSpaceFrameValue + 6 < nAfterburnerSoundDeadline)
        nAfterburnerSoundDeadline = 0;
    if (nCurrentSpaceFrameValue > nAfterburnerSoundDeadline) {
        nAfterburnerSoundDeadline = nCurrentSpaceFrameValue + 6;
        PlaySfxWaveFileByNumber(12, -1, 0);
    }
}
