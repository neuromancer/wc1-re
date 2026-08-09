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
    initialize_object(debris, OBJECT_TYPE_EXPLOSION2, obj);
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
        initialize_object(debris, g_aaeExplosionDebris_004698e0[set][index],
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
    initialize_object(explosion, OBJECT_TYPE_EXPLOSION1, obj);
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
        initialize_object(obj, explosionType, g_acObjectOwner_0059ce20[obj]);
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
                (short)(g_aObjectTypeData_0046645c[
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
    FixedVector convergence;
    FixedVector forwardVelocity;
    short projectileSpeed;
    short projectile;

    if (obj < 0 || obj >= WC1_SPACE_OBJECT_COUNT ||
        weapon < 0 || weapon >= (signed char)g_aShipWeapons_0059cab0[obj][0])
        return -1;
    slot = &((ShipWeaponSlot *)&g_aShipWeapons_0059cab0[obj][1])[weapon];
    weaponType = slot->type;
    if ((int)weaponType < 0 || weaponType >= OBJECT_TYPE_COUNT)
        return -1;
    weaponClass = g_aObjectTypeData_0046645c[weaponType].objectClass;
    if (weaponType == OBJECT_TYPE_TURRET)
        weaponType = OBJECT_TYPE_LASER_CANNON;

    projectile = find_vacant_3d_object();
    if (projectile == -1)
        return -1;
    initialize_object(projectile, weaponType, obj);
    copy_frame(obj, projectile);
    weaponData = &g_aObjectTypeData_0046645c[weaponType];
    child_object(slot->hardpoint, projectile, obj);
    g_asObjectCounter_0059c330[projectile] = weaponData->lifetime;
    vector_component_in_dir(&g_aShipVelocity_0059c010[obj],
                            &g_aShipForwardVector_0059bce0[projectile],
                            &g_aShipVelocity_0059c010[projectile]);
    projectileSpeed = 10;
    if (weaponClass == OBJECT_CLASS_PROJECTILE) {
        projectileSpeed = weaponData->maximumVelocity;
        ScaleFixedVector(&g_aShipForwardVector_0059bce0[obj],
            (int)(short)((weaponData->damageCapacity + 5) *
                         weaponData->maximumVelocity) << 8,
            &convergence);
        AddFixedVectors(&g_aShipPosition_0059c490[obj], &convergence,
                        &convergence);
        point_at(projectile, convergence);
    }
    ScaleFixedVector(&g_aShipForwardVector_0059bce0[projectile],
                     (int)projectileSpeed << 8, &forwardVelocity);
    AddFixedVectors(&g_aShipVelocity_0059c010[projectile], &forwardVelocity,
                    &g_aShipVelocity_0059c010[projectile]);
    g_asObjectViewFrame_0059d230[projectile] = 0;
    if (obj < 10)
        g_asObjectCounter_0059c330[obj] = 12;
    return projectile;
}
