/*
 *  Ship damage and component repair reporting.
 *
 *  Address range 0x41f000-0x420fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: ReportShipSystemDamage/ReportComponentRepaired; string band 0x469960-0x469984.
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

/* Function start: 0x41ECE0 */
int Create_ship_hit_debris(short obj)
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

/* Function start: 0x41F5D0 */
unsigned int ReportShieldHit(void)
{
    damage_your_component(3, 1, 3);
    return 0;
}

/* Function start: 0x41F7C0 */
void check_next_wave(void)
{
    short obj;

    if (g_nCannedSceneMode_00469fac == 2 ||
        g_nCurrentWave_0046c01c == -1)
        return;
    obj = 0;
    do {
        if (g_aeObjectClass_0059d100[obj] == OBJECT_CLASS_SHIP &&
            g_aeShipSide_0059d650[obj] == SIDE_KILRATHI)
            return;
        obj++;
    } while (obj < 10);
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
        set_special(obj, SPECIAL_MANEUVER_UNKNOWN_9);
        g_asObjectCounter_0059c330[obj] = 8;
        if (objectClass == OBJECT_CLASS_CAPITAL_SHIP) {
            short count = 4;

            do {
                Create_ship_hit_debris(obj);
                count--;
            } while (count != 0);
            g_asObjectCounter_0059c330[obj] =
                (short)(g_aObjectTypeData_00466458[
                    g_aeObjectType_0059b560[obj]].lifetime / 4 + 8);
        } else {
            explosion = ShipExplosion(obj);
        }
    }
    if (g_asObjectScreenX_0059d9b0[obj] != (short)0x8001)
        PlaySfxWaveFileByNumber(4, obj, 0);
    return explosion;
}

/* Function start: 0x420040 */
int explode(short attacker, short victim)
{
    (void)attacker;
    if (victim < 0 || g_aeObjectClass_0059d100[victim] == OBJECT_CLASS_NULL)
        return 0;
    if (g_aeSpecialManeuver_0059c3c0[victim] ==
            SPECIAL_MANEUVER_UNKNOWN_9 &&
        g_aeObjectClass_0059d100[victim] >= OBJECT_CLASS_SHIP)
        return 0;
    Explosion(victim);
    return 1;
}

/* Function start: 0x4202D0 */
void fire_capital_weapon(short obj, short target)
{
    get_facing_range_from_object(obj, target);
    g_acShipTarget_0059ce60[obj] = (signed char)target;
}

/* Function start: 0x4208C0 */
short RandomlyNegate(short v)
{
    if ((short)RandomBelowOrEqual(1) != 0)
        return v;
    return -v;
}

/* Function start: 0x420AA0 */
int fire_turrets(short obj)
{
    if (attacker_in_range(obj, 5000) != 0) {
        g_acShipTarget_0059ce60[obj] =
            (signed char)g_nTargetShip_0059c3b0;
        fire_capital_weapon(obj, g_nTargetShip_0059c3b0);
        return 1;
    }
    return 0;
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
        DAT_0059dee0[projectile] = weaponData->damageCapacity;
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
                range = (int)ComputeFixedVectorMagnitude(&vector);
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
