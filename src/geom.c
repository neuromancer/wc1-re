/*
 *  Vector, angle and fixed-point geometry helpers.
 *
 *  Address range 0x418000-0x41ad4f (provisional -- see docs/ORDER.md).
 *  Boundary evidence: geometry helpers followed by the contiguous modal-panel
 *  drawing block, before the save/load tranche at 0x41ada0.
 */
#include "wc1.h"

/* Function start: 0x418080 */
short __stdcall MeasureTextPixelWidthClamped(const char *text)
{
    short width = 0;
    const char *scan = text;

    while (*scan != 0) {
        width = (short)(width + GetFontCharWidth(*scan++));
        if (width >= 320)
            break;
    }
    if (*scan-- != 0) {
        width = (short)(width - GetFontCharWidth(*scan));
    }
    return width;
}

/* Function start: 0x4180C0 */
int __stdcall SeekPacketSection(PacketSectionHandle *handle, int offset,
                                short origin)
{
    int position;
    int sectionEnd;
    int result;

    sectionEnd = (int)(handle->dataOffset + handle->dataSize);
    switch (origin) {
    case 0:
        position = (int)handle->dataOffset;
        break;
    case 1:
        position = (int)(handle->dataOffset + handle->position);
        break;
    case 2:
        position = sectionEnd;
        break;
    }
    position += offset;
    if (position < (int)handle->dataOffset)
        position = (int)handle->dataOffset;
    if (position > sectionEnd && handle->finalSection == 0)
        position = sectionEnd;
    result = SeekDataFile((unsigned short)handle->file, position, 0);
    if (result != -1)
        handle->position = (unsigned int)(result - handle->dataOffset);
}

/* Function start: 0x418130 */
unsigned short GetMusicDriverPresent(void)
{
    return 1;
}

/* Function start: 0x418140 */
short __stdcall CollectActivePaletteIndices(Viewport *viewport,
                                             unsigned char *indices,
                                             short capacity)
{
    unsigned char *active;
    short count;
    short index;

    count = 0;
    active = (unsigned char *)AllocateTaggedMemory((unsigned int)capacity, 0);
    if (active == 0)
        return 0;

    memset(active, 0, (unsigned int)capacity);
    MarkActivePaletteEntries(viewport, active);
    index = 0;
    if (capacity > 0) {
        do {
            if (active[index] != 0)
                indices[count++] = (unsigned char)index;
            index++;
        } while (index < capacity);
    }
    ReleasePacketHandle((int)active);
    return count;
}

/* Function start: 0x4181C0 */
short get_ship_max_velocity(short obj)
{
    short velocity = g_aObjectTypeData_00466458[
        g_aeObjectType_0059b560[obj]].maximumVelocity;

    if (obj < 10 && g_acShipRating_0059cd80[obj] > 8)
        return velocity + velocity / 3;
    return velocity;
}

/* Function start: 0x418210 */
short recalc_max_velocity(short ship)
{
    short oldVelocity;
    short maximumVelocity;

    oldVelocity = g_asShipMaximumSpeed_0059c440[ship];
    if (g_anShipFuel_0059b470[ship] <= 0) {
        g_asShipMaximumSpeed_0059c440[ship] = 5;
    } else {
        maximumVelocity = get_ship_max_velocity(ship);
        g_asShipMaximumSpeed_0059c440[ship] =
            (short)(((int)maximumVelocity *
                     (4 - (int)g_acShipIonDriveDamage_0059d4a0[ship])) >> 2);
    }
    if (g_asShipMaximumSpeed_0059c440[ship] != oldVelocity)
        celerate(ship, 0);
    return 0;
}

/* Function start: 0x418280 */
void drain_fuel(short ship, short amount)
{
    g_anShipFuel_0059b470[ship] -= (int)amount;
    if (g_anShipFuel_0059b470 == 0)
        recalc_max_velocity(ship);
}

/* Function start: 0x4182B0 */
void damage_ion_drive(short ship, short amount, short maximum)
{
    volatile signed char *ionDriveDamage;
    int damage;

    ionDriveDamage = &g_acShipIonDriveDamage_0059d4a0[ship];
    damage = (int)*ionDriveDamage;
    damage += amount;
    if (damage >= maximum)
        damage = maximum;
    if (damage <= 0)
        damage = 0;
    *ionDriveDamage = (signed char)damage;
    recalc_max_velocity(ship);
}

/* Function start: 0x4182F0 */
int GetShipAccelerationRate(short ship)
{
    int shipIndex = (int)ship;
    enum ObjectType objectType = g_aeObjectType_0059b560[shipIndex];
    int acceleration = g_aObjectTypeData_00466458[objectType].acceleration;

    if (ship < 10 && g_acShipRating_0059cd80[shipIndex] > RATING_ACE_ICEMAN)
        return acceleration + acceleration / 3;
    return acceleration;
}

/* Function start: 0x418330 */
void point_at(short obj, FixedVector point)
{
    FixedVector direction;

    ComputeVectorDelta(&g_aShipPosition_0059c490[obj], &point, &direction);
    shrink_vector(&direction);
    g_aShipForwardVector_0059bce0[obj] = direction;
    fix_objects_ijk(obj);
}

/* Function start: 0x4183A0 */
void look_at(short obj)
{
    point_at(WC1_EYE_OBJECT, g_aShipPosition_0059c490[obj]);
}

/* Function start: 0x4183D0 */
void position_relative(FixedVector *position, FixedVector direction,
                       short distance)
{
    if (distance != 0) {
        NormalizeFixedVector(&direction);
        ScaleFixedVector(&direction, (int)distance << 8, &direction);
        AddFixedVectors(position, &direction, position);
    }
}

/* Function start: 0x418420 */
void position_relative_ijk(FixedVector *position, short obj,
                           short right, short up, short forward)
{
    *position = g_aShipPosition_0059c490[obj];
    position_relative(position, g_aShipRightVector_0059b6e0[obj], right);
    position_relative(position, g_aShipUpVector_0059b9e0[obj], up);
    position_relative(position, g_aShipForwardVector_0059bce0[obj], forward);
}

/* Function start: 0x4184C0 */
short FixedToShortSaturating(int value)
{
    if (value < -0x7fff00)
        return -0x7fff;
    if (value > 0x7fff00)
        return 0x7fff;
    return (short)(value >> 8);
}

/* Function start: 0x4184E0 */
int MinInt(int a, int b)
{
    if (a <= b) b = a;
    return b;
}

/* Function start: 0x4184F0 */
int MaxInt(int a, int b)
{
    if (b <= a) b = a;
    return b;
}

/* Function start: 0x418500 */
int AbsInt(int v)
{
    if (v < 0)
        v = -v;
    return v;
}

/* Function start: 0x418510 */
/* Exact Mac symbol: intfract_sign. No inbound reference is known in the
 * shipped executable; this routine is believed unreachable. */
int intfract_sign(int sign, int magnitude)
{
    int result;

    result = magnitude;
    if (sign >= 0)
        return result;
    return -result;
}

/* Function start: 0x418520 */
unsigned short SignShort(short v)
{
    if (v < 0)
        return 0xffff;
    return (unsigned short)(0 < v);
}

/* Function start: 0x418540 */
unsigned int SignFixed(int v)
{
    if (v < 0)
        return 0xffffff00;
    return v > 0 ? 0x100 : 0;
}

/* Function start: 0x418560 */
/* Normalise a degree value into (-180, 180]. 0x168 == 360, 0xB4 == 180. */
short WrapDegrees(short degrees)
{
    short v = degrees % 0x168;

    if (v < -0xb4)
        v = v + 0x168;
    if (0xb4 < v)
        v = v - 0x168;
    return v;
}

/* Function start: 0x418590 */
int equ_vector(const FixedVector *left, const FixedVector *right)
{
    return left->x == right->x && left->y == right->y &&
           left->z == right->z;
}

/* Function start: 0x4185C0 */
int IsPairEqualityDifferentFromFlag(const unsigned int *values)
{
    return ((values[1] == values[0]) == values[2]) == 0;
}

/* Function start: 0x4185F0 */
void zero_vector(FixedVector *vector)
{
    vector->z = 0;
    vector->y = 0;
    vector->x = 0;
}

/* Function start: 0x418600 */
void negate_vector(FixedVector *vector)
{
    vector->x = -vector->x;
    vector->y = -vector->y;
    vector->z = -vector->z;
}

/* Function start: 0x418620 */
void AddFixedVectors(const FixedVector *left, const FixedVector *right,
                     FixedVector *sum)
{
    sum->x = right->x + left->x;
    sum->y = right->y + left->y;
    sum->z = right->z + left->z;
}

/* Function start: 0x418650 */
void SubtractFixedVectors(FixedVector *left, FixedVector *right,
                          FixedVector *difference)
{
    difference->x = left->x - right->x;
    difference->y = left->y - right->y;
    difference->z = left->z - right->z;
}

/* Function start: 0x418680 */
void ComputeVectorDelta(FixedVector *from, FixedVector *to, FixedVector *delta)
{
    delta->x = to->x - from->x;
    delta->y = to->y - from->y;
    delta->z = to->z - from->z;
}

/* Function start: 0x4186B0 */
void ScaleFixedVector(FixedVector *vector, int scale, FixedVector *result)
{
    result->x = MultiplyFixed(vector->x, scale);
    result->y = MultiplyFixed(vector->y, scale);
    result->z = MultiplyFixed(vector->z, scale);
}

/* Function start: 0x418700 */
void divide_vector(FixedVector *vector, int divisor, FixedVector *result)
{
    result->x = DivideFixed(vector->x, divisor);
    result->y = DivideFixed(vector->y, divisor);
    result->z = DivideFixed(vector->z, divisor);
}

/* Function start: 0x418750 */
short ChooseRandomSignedMagnitude(short minimum, short maximum,
                                  int allowNegative)
{
    short value = RandomInRange(minimum, maximum);

    if (allowNegative != 0 && RandomInRange(0, 1) != 0)
        value = -value;
    return value;
}

/* Function start: 0x418780 */
void MakeRandomVectorFixed(short minimum, short maximum, FixedVector *vector)
{
    vector->x = (int)ChooseRandomSignedMagnitude(minimum, maximum, 1) << 8;
    vector->y = (int)ChooseRandomSignedMagnitude(minimum, maximum, 1) << 8;
    vector->z = (int)ChooseRandomSignedMagnitude(minimum, maximum, 1) << 8;
}

/* Function start: 0x4187E0 */
void FillFixedVectorWithRandomComponents(short limit, FixedVector *vector)
{
    MakeRandomVectorFixed(0, limit, vector);
}

/* Function start: 0x418800 */
void random_radial(const FixedVector *center, short radius,
                   FixedVector *position)
{
    FixedVector offset;

    FillFixedVectorWithRandomComponents(
        (short)RandomBelowOrEqual(radius), &offset);
    AddFixedVectors(center, &offset, position);
}

/* Function start: 0x418840 */
void MakeRandomNormalizedVector(FixedVector *vector)
{
    vector->x = (unsigned short)RandomInRange(0x40, 0xff);
    vector->y = (unsigned short)RandomInRange(0x40, 0xff);
    vector->z = (unsigned short)RandomInRange(0x40, 0xff);
    NormalizeFixedVector(vector);
}

/* Function start: 0x418890 */
void rectangular_to_spherical(const FixedVector *rectangular,
                              SphericalVector *spherical)
{
    int horizontalLength;
    int z;

    spherical->radius = (int)Vector_magnitude(rectangular);
    if (spherical->radius == 0)
        return;
    z = rectangular->z;
    horizontalLength = (int)PlanarMagnitude(rectangular->x, z);
    spherical->yaw = (short)ArcCos(
        DivideFixed(z, horizontalLength));
    if (rectangular->x < 0)
        spherical->yaw = -spherical->yaw;
    spherical->pitch = (short)(ArcCos(
        DivideFixed(rectangular->y, spherical->radius)) - 90);
}

/* Function start: 0x418980 */
void ConvertShortVectorToFixedVector(const ShortVector *source,
                                     FixedVector *destination)
{
    destination->x = (int)source->x << 8;
    destination->y = (int)source->y << 8;
    destination->z = (int)source->z << 8;
}

/* Function start: 0x4189B0 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
void ConvertFixedVectorToShortVector(const FixedVector *source,
                                     ShortVector *destination)
{
    destination->x = (short)(source->x >> 8);
    destination->y = (short)(source->y >> 8);
    destination->z = (short)(source->z >> 8);
}

/* Function start: 0x4189E0 */
int dot_product(const FixedVector *left, const FixedVector *right)
{
    return MultiplyFixed(left->x, right->x) +
           MultiplyFixed(left->y, right->y) +
           MultiplyFixed(left->z, right->z);
}

/* Function start: 0x418A30 */
short vector_angle(FixedVector left, FixedVector right)
{
    NormalizeFixedVector(&left);
    NormalizeFixedVector(&right);
    return (short)(((short)dot_product(&left, &right) * 100) / 0x100);
}

/* Function start: 0x418A80 */
void vector_cross_product(const FixedVector *left, const FixedVector *right,
                          FixedVector *product)
{
    product->x = MultiplyFixed(left->y, right->z) -
                 MultiplyFixed(left->z, right->y);
    product->y = MultiplyFixed(left->z, right->x) -
                 MultiplyFixed(left->x, right->z);
    product->z = MultiplyFixed(left->x, right->y) -
                 MultiplyFixed(left->y, right->x);
}

/* Function start: 0x418B10 */
short NormalizeFixedVector(FixedVector *vector)
{
    int magnitude = Vector_magnitude(vector);

    if (magnitude != 0) {
        vector->x = DivideFixed(vector->x, magnitude);
        vector->y = DivideFixed(vector->y, magnitude);
        vector->z = DivideFixed(vector->z, magnitude);
        return 1;
    }
    return 0;
}

/* Function start: 0x418B60 */
int vector_length_in_dir(const FixedVector *vector,
                         const FixedVector *direction)
{
    FixedVector normalized;

    normalized = *vector;
    NormalizeFixedVector(&normalized);
    return (int)MultiplyFixed(
        Vector_magnitude(vector), dot_product(direction, &normalized));
}

/* Function start: 0x418BB0 */
void vector_component_in_dir(const FixedVector *vector,
                             const FixedVector *direction,
                             FixedVector *component)
{
    int length;

    length = vector_length_in_dir(vector, direction);
    ScaleFixedVector((FixedVector *)direction, length, component);
}

/* Function start: 0x418BE0 */
void rotate_about_i(short angle, FixedVector *j, FixedVector *k)
{
    int old;
    int cosine;
    int sine;

    cosine = (int)CosFixed(angle);
    sine = (int)SinFixed(angle);
    old = j->x;
    j->x = MultiplyFixed(old, cosine) - MultiplyFixed(k->x, sine);
    k->x = MultiplyFixed(old, sine) + MultiplyFixed(k->x, cosine);
    old = j->y;
    j->y = MultiplyFixed(old, cosine) - MultiplyFixed(k->y, sine);
    k->y = MultiplyFixed(old, sine) + MultiplyFixed(k->y, cosine);
    old = j->z;
    j->z = MultiplyFixed(old, cosine) - MultiplyFixed(k->z, sine);
    k->z = MultiplyFixed(old, sine) + MultiplyFixed(k->z, cosine);
}

/* Function start: 0x418D00 */
void rotate_about_j(short angle, FixedVector *i, FixedVector *k)
{
    int cosine;
    int sine;
    int old;

    cosine = (int)CosFixed(angle);
    sine = (int)SinFixed(angle);
    old = i->x;
    i->x = MultiplyFixed(k->x, sine) + MultiplyFixed(old, cosine);
    k->x = MultiplyFixed(k->x, cosine) - MultiplyFixed(old, sine);
    old = i->y;
    i->y = MultiplyFixed(k->y, sine) + MultiplyFixed(old, cosine);
    k->y = MultiplyFixed(k->y, cosine) - MultiplyFixed(old, sine);
    old = i->z;
    i->z = MultiplyFixed(k->z, sine) + MultiplyFixed(old, cosine);
    k->z = MultiplyFixed(k->z, cosine) - MultiplyFixed(old, sine);
}

/* Function start: 0x418E40 */
void rotate_about_k(short angle, FixedVector *i, FixedVector *j)
{
    int old;
    int cosine;
    int sine;

    cosine = (int)CosFixed(angle);
    sine = (int)SinFixed(angle);
    old = i->x;
    i->x = MultiplyFixed(old, cosine) - MultiplyFixed(j->x, sine);
    j->x = MultiplyFixed(old, sine) + MultiplyFixed(j->x, cosine);
    old = i->y;
    i->y = MultiplyFixed(old, cosine) - MultiplyFixed(j->y, sine);
    j->y = MultiplyFixed(old, sine) + MultiplyFixed(j->y, cosine);
    old = i->z;
    i->z = MultiplyFixed(old, cosine) - MultiplyFixed(j->z, sine);
    j->z = MultiplyFixed(old, sine) + MultiplyFixed(j->z, cosine);
}

/* Function start: 0x418F60 */
void init_ijk(short obj)
{
    g_aShipForwardVector_0059bce0[obj].z = 0x100;
    g_aShipUpVector_0059b9e0[obj].y = 0x100;
    g_aShipRightVector_0059b6e0[obj].x = 0x100;
    g_aShipForwardVector_0059bce0[obj].y = 0;
    g_aShipUpVector_0059b9e0[obj].z = 0;
    g_aShipForwardVector_0059bce0[obj].x = 0;
    g_aShipUpVector_0059b9e0[obj].x = 0;
    g_aShipRightVector_0059b6e0[obj].z = 0;
    g_aShipRightVector_0059b6e0[obj].y = 0;
    g_anObjectRollRotation_0059d7e0[obj] = 0;
    g_anObjectYawRotation_0059ce80[obj] = 0;
    g_anObjectPitchRotation_0059b2a0[obj] = 0;
}

/* Function start: 0x418FD0 */
void copy_frame(short source, short destination)
{
    g_aShipRightVector_0059b6e0[destination] =
        g_aShipRightVector_0059b6e0[source];
    g_aShipUpVector_0059b9e0[destination] =
        g_aShipUpVector_0059b9e0[source];
    g_aShipForwardVector_0059bce0[destination] =
        g_aShipForwardVector_0059bce0[source];
}

/* Function start: 0x419050 */
void fix_objects_ijk(short obj)
{
    vector_cross_product(&g_aShipUpVector_0059b9e0[obj],
                         &g_aShipForwardVector_0059bce0[obj],
                         &g_aShipRightVector_0059b6e0[obj]);
    vector_cross_product(&g_aShipForwardVector_0059bce0[obj],
                         &g_aShipRightVector_0059b6e0[obj],
                         &g_aShipUpVector_0059b9e0[obj]);
    NormalizeFixedVector(&g_aShipRightVector_0059b6e0[obj]);
    NormalizeFixedVector(&g_aShipUpVector_0059b9e0[obj]);
    NormalizeFixedVector(&g_aShipForwardVector_0059bce0[obj]);
}

/* Function start: 0x4190B0 */
void transform_to_objects_frame(const FixedVector *source,
                                FixedVector *destination, short obj)
{
    destination->x = dot_product(source, &g_aShipRightVector_0059b6e0[obj]);
    destination->y = dot_product(source, &g_aShipUpVector_0059b9e0[obj]);
    destination->z = dot_product(source, &g_aShipForwardVector_0059bce0[obj]);
}

/* Function start: 0x419110 */
void alter_pitch(short angle, short obj)
{
    rotate_about_i(angle, &g_aShipUpVector_0059b9e0[obj],
                   &g_aShipForwardVector_0059bce0[obj]);
    fix_objects_ijk(obj);
}

/* Function start: 0x419150 */
void alter_yaw(short angle, short obj)
{
    rotate_about_j(angle, &g_aShipRightVector_0059b6e0[obj],
                   &g_aShipForwardVector_0059bce0[obj]);
    fix_objects_ijk(obj);
}

/* Function start: 0x419190 */
void alter_roll(short angle, short obj)
{
    rotate_about_k(angle, &g_aShipRightVector_0059b6e0[obj],
                   &g_aShipUpVector_0059b9e0[obj]);
    fix_objects_ijk(obj);
}

/* Function start: 0x4191D0 */
short distance_between_points(const FixedVector *from,
                              const FixedVector *to)
{
    FixedVector delta;
    long magnitude;

    ComputeVectorDelta((FixedVector *)from, (FixedVector *)to, &delta);
    magnitude = Vector_magnitude(&delta);
    return FixedToShortSaturating((int)magnitude);
}

/* Function start: 0x419210 */
short distance_from_point(short obj, const FixedVector *point)
{
    long magnitude;

    ComputeVectorDelta(&g_aShipPosition_0059c490[obj],
                       (FixedVector *)point, &g_vToTarget_0059d4d0);
    magnitude = Vector_magnitude(&g_vToTarget_0059d4d0);
    return FixedToShortSaturating((int)magnitude) -
           g_asObjectCollisionRadius_0059d710[obj];
}

/* Function start: 0x419260 */
short distance_from_object(short obj, short other)
{
    return distance_from_point(obj, &g_aShipPosition_0059c490[other]) -
           g_asObjectCollisionRadius_0059d710[other];
}

/* Function start: 0x419290 */
void get_facing_range_from_point(short obj, const FixedVector *point)
{
    g_nTargetRange_0059ce10 =
        distance_from_point(obj, point) -
        g_asObjectCollisionRadius_0059d710[obj];
    g_vNormalizedToTarget_005a7db0 = g_vToTarget_0059d4d0;
    NormalizeFixedVector(&g_vNormalizedToTarget_005a7db0);
    g_nFacingToTarget_0059d920 =
        (short)(((short)dot_product(
            &g_vNormalizedToTarget_005a7db0,
            &g_aShipForwardVector_0059bce0[obj]) * 100) >> 8);
}

/* Function start: 0x419310 */
void get_facing_range_from_object(short obj, short other)
{
    get_facing_range_from_point(obj, &g_aShipPosition_0059c490[other]);
    g_nTargetRange_0059ce10 -= g_asObjectCollisionRadius_0059d710[other];
    negate_vector(&g_vNormalizedToTarget_005a7db0);
    g_nTargetFacing_0059d52a =
        (short)(((short)dot_product(
            &g_vNormalizedToTarget_005a7db0,
            &g_aShipForwardVector_0059bce0[other]) * 100) >> 8);
}

/* Function start: 0x419390 */
void ship_vs_point(short obj, const FixedVector *point)
{
    get_facing_range_from_point(obj, point);
}

/* Function start: 0x4193B0 */
void ship_vs_ship(short obj, short other)
{
    get_facing_range_from_object(obj, other);
}

/* Function start: 0x4193D0 */
short facing_to_object(short obj, FixedVector *point)
{
    FixedVector direction;

    ComputeVectorDelta(&g_aShipPosition_0059c490[obj],
                       point, &direction);
    NormalizeFixedVector(&direction);
    g_nFacingToTarget_0059d920 =
        (short)(((short)dot_product(
            &direction, &g_aShipForwardVector_0059bce0[obj]) * 100) >> 8);
    return g_nFacingToTarget_0059d920;
}

/* Function start: 0x419440 */
short match_roll_orientation(short obj, short reference)
{
    FixedVector roll;
    short angle;

    roll.x = dot_product(&g_aShipUpVector_0059b9e0[obj],
                         &g_aShipRightVector_0059b6e0[reference]);
    roll.y = dot_product(&g_aShipUpVector_0059b9e0[obj],
                         &g_aShipUpVector_0059b9e0[reference]);
    roll.z = 0;
    NormalizeFixedVector(&roll);
    angle = (short)ArcCos(roll.y);
    if (roll.x >= 0)
        angle = 360 - angle;
    return WrapDegrees(angle);
}

/* Function start: 0x4194D0 */
int set_ship_rotation_goals(short obj, short turnRate,
                            const FixedVector *direction,
                            short pointingMode,
                            short *yawGoal, short *pitchGoal)
{
    SphericalVector spherical;
    FixedVector localDirection;
    int magnitude;

    transform_to_objects_frame(direction, &localDirection, obj);
    if (pointingMode == 1) {
        rectangular_to_spherical(&localDirection, &spherical);
        if (spherical.radius == 0)
            return 1;
        if (spherical.yaw <= 0)
            spherical.yaw = (short)(spherical.yaw + turnRate);
        else
            spherical.yaw = (short)(spherical.yaw - turnRate);
    } else {
        magnitude = (int)Vector_magnitude(&localDirection);
        if (magnitude == 0)
            return 1;
        spherical.yaw = (short)ArcSin(DivideFixed(
            localDirection.x,
            (int)PlanarMagnitude(localDirection.x, localDirection.z)));
        spherical.pitch = (short)-ArcSin(
            DivideFixed(localDirection.y, magnitude));
        if (localDirection.z < 0) {
            spherical.yaw = (short)-spherical.yaw;
            if (spherical.pitch <= 0)
                spherical.pitch = (short)(spherical.pitch - 180);
            else
                spherical.pitch = (short)(spherical.pitch + 180);
        }
        if (spherical.pitch <= 0)
            spherical.pitch = (short)(spherical.pitch + turnRate);
        else
            spherical.pitch = (short)(spherical.pitch - turnRate);
    }
    *yawGoal = WrapDegrees((short)-spherical.yaw);
    *pitchGoal = WrapDegrees((short)-spherical.pitch);
    return 0;
}

/* Function start: 0x419620 */
void point_ship(short obj, short turnRate, const FixedVector *direction)
{
    set_ship_rotation_goals(obj, turnRate, direction,
                            g_acShipPointingMode_0059d790[obj],
                            &g_anYawGoal_0059c310[obj],
                            &g_anPitchGoal_0059d7a0[obj]);
}

/* Function start: 0x419660 */
void point_ship_at_point(short obj, const FixedVector *point)
{
    FixedVector direction;

    ComputeVectorDelta(&g_aShipPosition_0059c490[obj],
                       (FixedVector *)point, &direction);
    point_ship(obj, 0, &direction);
}

/* Function start: 0x4196A0 */
void point_ship_at_object(short obj, short other)
{
    point_ship_at_point(obj, &g_aShipPosition_0059c490[other]);
}

/* Function start: 0x4196C0 */
void point_capital_ship_at_object(short obj, short other)
{
    FixedVector direction;

    ComputeVectorDelta(&g_aShipPosition_0059c490[other],
                       &g_aShipPosition_0059c490[obj], &direction);
    point_ship(obj, 0, &direction);
}

/* Function start: 0x419710 */
void point_ship_behind_object(short obj, short other)
{
    FixedVector point = g_aShipPosition_0059c490[other];

    position_relative(&point, g_aShipForwardVector_0059bce0[other],
                      (short)(-500 -
                          g_asObjectCollisionRadius_0059d710[other]));
    point_ship_at_point(obj, &point);
}

/* Function start: 0x419790 */
void point_ship_below_object(short obj, short other)
{
    FixedVector point = g_aShipPosition_0059c490[other];

    position_relative(&point, g_aShipUpVector_0059b9e0[other],
                      (short)(g_asObjectCollisionRadius_0059d710[other] +
                              500));
    point_ship_at_point(obj, &point);
}

/* Function start: 0x419810 */
void point_perpendicular_to_point(short obj, const FixedVector *point)
{
    point_ship_at_point(obj, point);
    if (g_anYawGoal_0059c310[obj] < 0)
        g_anYawGoal_0059c310[obj] += 90;
    else
        g_anYawGoal_0059c310[obj] -= 90;
}

/* Function start: 0x419850 */
void point_perpendicular(short obj, short other)
{
    point_perpendicular_to_point(obj, &g_aShipPosition_0059c490[other]);
}

/* Function start: 0x419870 */
void point_parallel(short obj, short other)
{
    if (other != -1)
        point_ship(obj, 0, &g_aShipForwardVector_0059bce0[other]);
}

/* Function start: 0x4198A0 */
void MoveObjectAlongDirection(short obj, const FixedVector *direction,
                              short distance)
{
    FixedVector offset;

    offset = *direction;
    SetVectorFixedPoint((unsigned int *)&offset, distance);
    AddFixedVectors(&g_aShipPosition_0059c490[obj], &offset,
                    &g_aShipPosition_0059c490[obj]);
}

/* Function start: 0x419950 */
void NormalizeAndScaleVector(FixedVector *vector, int scale)
{
    NormalizeFixedVector(vector);
    ScaleFixedVector(vector, scale, vector);
}

/* Function start: 0x419970 */
void SetVectorFixedPoint(unsigned int *p, short v)
{
    NormalizeAndScaleVector((FixedVector *)p, (int)v << 8);
}

/* Function start: 0x419990 */
unsigned int IsPointWithinRange(FixedVector *from, FixedVector *to, short range)
{
    FixedVector delta;

    ComputeVectorDelta(from, to, &delta);
    return IsVectorWithinRange(&delta, range);
}

/* Function start: 0x4199C0 */
short check_for_collision(short obj)
{
    FixedVector *objectPosition;
    FixedVector *position;
    int objectIndex;
    short other;
    short range;

    objectIndex = (int)obj;
    objectPosition = &g_aShipPosition_0059c490[objectIndex];
    other = 0;
    position = g_aShipPosition_0059c490;
    do {
        if (other != obj &&
            g_aeObjectClass_0059d100[(int)other] >=
                OBJECT_CLASS_PROJECTILE) {
            ComputeVectorDelta(objectPosition, position,
                               &g_vCollisionDelta_0059d690);
            range = (short)(
                g_asObjectCollisionRadius_0059d710[(int)other] +
                g_asObjectCollisionRadius_0059d710[objectIndex]);
            if (g_aeObjectClass_0059d100[(int)other] ==
                    OBJECT_CLASS_SHIP &&
                g_aeObjectClass_0059d100[objectIndex] == OBJECT_CLASS_SHIP)
                range >>= 1;
            if (IsVectorWithinRange(&g_vCollisionDelta_0059d690,
                                    range) != 0)
                return other;
        }
        other++;
        position++;
    } while (other <= WC1_SPACE_LAST_MOVING_OBJECT);
    return -1;
}

/* Function start: 0x419A70 */
void position_child(short parent, short hardpoint, FixedVector *position)
{
    const ShortVector *offset = &g_aChildOffsets_004682f0[hardpoint];

    position->x = g_aShipForwardVector_0059bce0[parent].x * offset->z +
                  g_aShipUpVector_0059b9e0[parent].x * offset->y +
                  g_aShipRightVector_0059b6e0[parent].x * offset->x +
                  g_aShipPosition_0059c490[parent].x;
    position->y = g_aShipForwardVector_0059bce0[parent].y * offset->z +
                  g_aShipUpVector_0059b9e0[parent].y * offset->y +
                  g_aShipRightVector_0059b6e0[parent].y * offset->x +
                  g_aShipPosition_0059c490[parent].y;
    position->z = g_aShipForwardVector_0059bce0[parent].z * offset->z +
                  g_aShipUpVector_0059b9e0[parent].z * offset->y +
                  g_aShipRightVector_0059b6e0[parent].z * offset->x +
                  g_aShipPosition_0059c490[parent].z;
}

/* Function start: 0x419B40 */
void child_object(short hardpoint, short child, short parent)
{
    position_child(parent, hardpoint, &g_aShipPosition_0059c490[child]);
    g_acObjectOwner_0059ce20[child] = (signed char)parent;
}

/* Function start: 0x419B70 */
short get_ship_slot(void)
{
    short slot = 1;

    do {
        if (g_aeObjectClass_0059d100[slot] == OBJECT_CLASS_NULL) {
            DAT_0046c010 = slot;
            return slot;
        }
        slot++;
    } while (slot <= 9);
    DAT_0046c010 = -1;
    return -1;
}

/* Function start: 0x419BA0 */
short find_vacant_3d_object(void)
{
    short i = 10;

    do {
        if (g_aeObjectClass_0059d100[i] == OBJECT_CLASS_NULL) {
            g_asObjectScreenX_0059d9b0[i] = (short)0x8001;
            return i;
        }
        i = i + 1;
    } while (i <= 0x3c);
    return -1;
}

/* Function start: 0x419BD0 */
void remove_object(short obj)
{
    short slot;

    if (obj == -1)
        return;
    g_asObjectScreenX_0059d9b0[obj] = (short)0x8001;
    g_asObjectDistance_0059b4a0[obj] = 0;
    if (obj == DAT_00469208)
        DAT_00469208 = -1;
    if (obj == g_nYourWingman_0046c04c)
        g_nYourWingman_0046c04c = -1;
    for (slot = 0; slot < 20; slot++) {
        if (g_abHazardObjects_0046c028[slot] == obj) {
            g_abHazardObjects_0046c028[slot] = -1;
            break;
        }
    }
    if (obj < 10) {
        if (g_aeObjectClass_0059d100[obj] == OBJECT_CLASS_CAPITAL_SHIP)
            FreePacketAndClear((int *)&g_aeShipObjective_0059d200[obj + 60], 0);
        g_acShipRating_0059cd80[obj] = -1;
        g_acWingmanMessageState_0059d2c0[obj] = -1;
        g_aeShipSide_0059d650[obj] = SIDE_NEUTRAL;
        g_aeShipManeuver_0059dcb0[obj] = MANEUVER_NONE;
        clear_alert(obj);
        g_asCapitalShipViewFrame_0059dd90[obj] = -1;
    }
    g_aeObjectClass_0059d100[obj] = OBJECT_CLASS_NULL;
    g_aeShipObjective_0059d200[obj + 60] = OBJECTIVE_NAV_POINT;
}

/* Function start: 0x419CC0 */
void apply_force_to_objects_center(FixedVector *force, short obj)
{
    FixedVector acceleration;

    divide_vector(force,
                  (unsigned short)g_asObjectRadarRadius_0059c790[obj]
                      << 8,
                  &acceleration);
    AddFixedVectors(&g_aShipVelocity_0059c010[obj], &acceleration,
                    &g_aShipVelocity_0059c010[obj]);
}

/* Function start: 0x419D10 */
void apply_force_to_object(FixedVector *point, FixedVector *force,
                           short obj)
{
    FixedVector localPoint;
    FixedVector localForce;
    FixedVector acceleration;
    int rotationalMass;
    int mass;
    int value;

    transform_to_objects_frame(force, &localForce, obj);
    transform_to_objects_frame(point, &localPoint, obj);
    rotationalMass = DivideFixed(
        (unsigned short)g_asObjectAfterburnerVelocity_0059c9d0[obj]
            << 8,
        (int)g_asObjectCollisionRadius_0059d710[obj] << 8);

    value = DivideFixed(
        MultiplyFixed(localPoint.x, localForce.y) -
            MultiplyFixed(localPoint.y, localForce.x),
        rotationalMass);
    g_anObjectRollRotation_0059d7e0[obj] += (short)(value >> 8);
    value = DivideFixed(
        MultiplyFixed(localPoint.x, localForce.z) -
            MultiplyFixed(localPoint.z, localForce.x),
        rotationalMass);
    g_anObjectYawRotation_0059ce80[obj] += (short)(value >> 8);
    value = DivideFixed(
        MultiplyFixed(localPoint.z, localForce.y) -
            MultiplyFixed(localPoint.y, localForce.z),
        rotationalMass);
    g_anObjectPitchRotation_0059b2a0[obj] += (short)(value >> 8);
    ClampTo30(&g_anObjectPitchRotation_0059b2a0[obj]);
    ClampTo30(&g_anObjectYawRotation_0059ce80[obj]);
    ClampTo30(&g_anObjectRollRotation_0059d7e0[obj]);

    mass = (unsigned short)g_asObjectRadarRadius_0059c790[obj] << 8;
    acceleration.x = DivideFixed(
        MultiplyFixed(0x16a -
            (int)PlanarMagnitude(localForce.y, localForce.z),
            localForce.x),
        MultiplyFixed(0x16a, mass));
    acceleration.y = DivideFixed(
        MultiplyFixed(0x16a -
            (int)PlanarMagnitude(localForce.x, localForce.z),
            localForce.y),
        MultiplyFixed(0x16a, mass));
    acceleration.z = DivideFixed(
        MultiplyFixed(0x16a -
            (int)PlanarMagnitude(localForce.x, localForce.y),
            localForce.z),
        MultiplyFixed(0x16a, mass));
    AddFixedVectors(&g_aShipVelocity_0059c010[obj], &acceleration,
                    &g_aShipVelocity_0059c010[obj]);
    if (g_aeObjectClass_0059d100[obj] == OBJECT_CLASS_SHIP)
        check_for_lost_control(obj);
}

/* Function start: 0x419F70 */
void rotational_acceleration(FixedVector *point, FixedVector *force,
                             short obj)
{
    FixedVector localPoint;
    FixedVector localForce;
    int denominator;
    int value;

    transform_to_objects_frame(force, &localForce, obj);
    transform_to_objects_frame(point, &localPoint, obj);
    denominator = DivideFixed(
        (unsigned short)g_asObjectAfterburnerVelocity_0059c9d0[obj]
            << 8,
        MultiplyFixed(
            (int)g_asObjectCollisionRadius_0059d710[obj] << 8,
            0x123c));

    value = DivideFixed(
        MultiplyFixed(localPoint.x, localForce.y) -
            MultiplyFixed(localPoint.y, localForce.x),
        denominator);
    g_anObjectRollRotation_0059d7e0[obj] += (short)(value >> 8);
    value = DivideFixed(
        MultiplyFixed(localPoint.x, localForce.z) -
            MultiplyFixed(localPoint.z, localForce.x),
        denominator);
    g_anObjectYawRotation_0059ce80[obj] += (short)(value >> 8);
    value = DivideFixed(
        MultiplyFixed(localPoint.z, localForce.y) -
            MultiplyFixed(localPoint.y, localForce.z),
        denominator);
    g_anObjectPitchRotation_0059b2a0[obj] += (short)(value >> 8);
    ClampTo30(&g_anObjectPitchRotation_0059b2a0[obj]);
    ClampTo30(&g_anObjectYawRotation_0059ce80[obj]);
    ClampTo30(&g_anObjectRollRotation_0059d7e0[obj]);
    if (g_aeObjectClass_0059d100[obj] == OBJECT_CLASS_SHIP)
        check_for_lost_control(obj);
}

/* Function start: 0x41A0F0 */
void ClampVectorTo30(short *p)
{
    short v = *p;

    if (v < 0) {
        *p = v + 1;
        return;
    }
    if (0 < v)
        *p = v - 1;
}

/* Function start: 0x41A110 */
void ClampTo30(short *p)
{
    if (*p > 0x1e) {
        *p = 0x1e;
        return;
    }
    if (*p < -0x1e)
        *p = -0x1e;
}

/* Function start: 0x41A130 */
unsigned short IsPointWithinEyeViewCone(const FixedVector *point)
{
    FixedVector direction;
    FixedVector viewPosition;
    int distance;
    long projection;
    unsigned short visible;

    ComputeVectorDelta(&g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                       (FixedVector *)point, &direction);
    distance = (int)Vector_magnitude(&direction);
    if (g_asObjectCollisionRadius_0059d710[WC1_EYE_OBJECT] * 0x100 >
        distance)
        return 0;
    transform_to_objects_frame(&direction, &viewPosition,
                               WC1_EYE_OBJECT);
    if (g_asObjectCollisionRadius_0059d710[WC1_EYE_OBJECT] * 0x100 >
        viewPosition.z)
        return 0;
    projection = DivideFixed(viewPosition.z, distance);
    visible = 0;
    if (projection >= 0x94)
        visible = 1;
    return visible;
}

/* Function start: 0x41A1D0 */
void transform_objects_to_your_view(void)
{
    FixedVector direction;
    int distance;
    int objectIndex;
    int objectRadius;
    int scaleFactor;
    short dustSize;
    short obj;

    g_nClosestVisibleObject_0046c048 = -1;
    draw_nav_pointer();
    obj = 0;
    do {
        objectIndex = (int)obj;
        if (g_aeObjectClass_0059d100[objectIndex] != OBJECT_CLASS_NULL &&
            g_aeObjectClass_0059d100[objectIndex] !=
                OBJECT_CLASS_FIXED_OBJECT &&
            obj != DAT_00469208) {
            g_asPreviousObjectDistance_0059d080[objectIndex] =
                g_asObjectDistance_0059b4a0[objectIndex];
            g_asObjectDistance_0059b4a0[objectIndex] = 0;
            if (g_aeObjectClass_0059d100[objectIndex] ==
                OBJECT_CLASS_FUTURION) {
                g_asObjectScreenX_0059d9b0[objectIndex] = (short)0x8001;
                obj++;
                continue;
            }
            if (g_aeObjectClass_0059d100[objectIndex] ==
                    OBJECT_CLASS_PLANET ||
                g_aeObjectClass_0059d100[objectIndex] == OBJECT_CLASS_STAR) {
                direction = g_aShipPosition_0059c490[objectIndex];
            } else {
                ComputeVectorDelta(&g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                                   &g_aShipPosition_0059c490[objectIndex],
                                   &direction);
            }
            distance = (int)Vector_magnitude(&direction);
            if (distance <
                g_asObjectCollisionRadius_0059d710[WC1_EYE_OBJECT] * 0x100) {
                g_asObjectScreenX_0059d9b0[objectIndex] = (short)0x8001;
                goto next_object;
            }
            if (g_aeObjectClass_0059d100[objectIndex] == OBJECT_CLASS_DUST &&
                distance > (1400 << 8)) {
                g_asObjectScreenX_0059d9b0[objectIndex] = (short)0x8001;
                goto next_object;
            }
            transform_to_objects_frame(&direction,
                                       &g_aObjectViewPosition_0059afa0[
                                           objectIndex],
                                       WC1_EYE_OBJECT);
            if (g_aObjectViewPosition_0059afa0[objectIndex].z <
                g_asObjectCollisionRadius_0059d710[WC1_EYE_OBJECT] * 0x100) {
                g_asObjectScreenX_0059d9b0[objectIndex] = (short)0x8001;
                goto next_object;
            }
            if (DivideFixed(g_aObjectViewPosition_0059afa0[objectIndex].z,
                            distance) < 0x94) {
                g_asObjectScreenX_0059d9b0[objectIndex] = (short)0x8001;
                goto next_object;
            }
            objectRadius =
                g_asObjectCollisionRadius_0059d710[objectIndex] * 0x100;
            if (distance <= objectRadius)
                distance = objectRadius + 1;
            if (g_aeObjectClass_0059d100[objectIndex] > OBJECT_CLASS_DUST) {
                scaleFactor = (int)DivideFixed(
                    (short)(g_nScreenWidth_0046daa4 & ~1) << 15,
                    distance - objectRadius);
                g_asObjectScreenScale_0059c950[objectIndex] = (short)(
                    MultiplyFixed((unsigned short)
                                      g_asObjectScale_0059de40[objectIndex],
                                  scaleFactor) >> 8);
                if ((unsigned short)g_asObjectScreenScale_0059c950[
                        objectIndex] >
                    0x1fff)
                    g_asObjectScreenScale_0059c950[objectIndex] = 0x2000;
                if ((unsigned short)g_asObjectScreenScale_0059c950[
                        objectIndex] < 5) {
                    g_asObjectScreenX_0059d9b0[objectIndex] =
                        (short)0x8001;
                    goto next_object;
                }
            }
            g_asObjectDistance_0059b4a0[objectIndex] =
                (short)(distance >> 8);
            g_asObjectScreenX_0059d9b0[objectIndex] = (short)(DivideFixed(
                (int)MultiplyFixed(
                    (short)(g_nScreenWidth_0046daa4 & ~1) << 7,
                    g_aObjectViewPosition_0059afa0[objectIndex].x),
                g_aObjectViewPosition_0059afa0[objectIndex].z) >> 8);
            g_asObjectScreenY_0059d930[objectIndex] = (short)(DivideFixed(
                (int)MultiplyFixed(
                    (short)(g_nScreenWidth_0046daa4 & ~1) << 7,
                    g_aObjectViewPosition_0059afa0[objectIndex].y),
                g_aObjectViewPosition_0059afa0[objectIndex].z) >> 8);
            switch (g_aeObjectClass_0059d100[objectIndex]) {
            case OBJECT_CLASS_PLANET:
                if (g_asObjectScreenScale_0059c950[objectIndex] == 0xff)
                    set_background_objects_rotation(obj, &direction);
                break;
            case OBJECT_CLASS_DUST:
                dustSize = (short)(MultiplyFixed(
                    0x900, (int)DivideFixed(
                        g_asObjectCollisionRadius_0059d710[WC1_EYE_OBJECT] << 8,
                        distance)) >> 8);
                if (dustSize > 3)
                    dustSize = 3;
                g_asObjectViewFrame_0059d230[objectIndex] =
                    (short)(((g_asObjectCounter_0059c330[objectIndex] +
                              g_nSpaceFrame_0059b420) & 3) +
                            (g_asObjectScreenAngle_0059cd90[objectIndex] &
                             0x10) +
                            (3 - dustSize) * 4);
                break;
            case OBJECT_CLASS_MISSILE:
            case OBJECT_CLASS_SHIP:
            case OBJECT_CLASS_CAPITAL_SHIP:
                get_right_shape(obj, &direction);
                break;
            }
        }
next_object:
        obj++;
    } while (obj <= WC1_SPACE_LAST_MOVING_OBJECT);
}

/* Function start: 0x41A530 */
void set_background_objects_rotation(short obj, FixedVector *direction)
{
    SphericalVector spherical;
    FixedVector projectedUp;
    short angle;

    negate_vector(direction);
    rectangular_to_spherical(direction, &spherical);
    init_ijk(63);
    alter_yaw((short)-spherical.yaw, 63);
    alter_pitch((short)-spherical.pitch, 63);
    projectedUp.x = dot_product(&g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT],
                                &g_aShipRightVector_0059b6e0[63]);
    projectedUp.y = dot_product(&g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT],
                                &g_aShipUpVector_0059b9e0[63]);
    projectedUp.z = 0;
    NormalizeFixedVector(&projectedUp);
    angle = (short)ArcCos(projectedUp.y);
    if (projectedUp.x >= 0)
        angle = 360 - angle;
    g_asObjectScreenAngle_0059cd90[obj] = angle;
    g_asObjectScreenScale_0059c950[obj] = 0xff;
}

/* Function start: 0x41A610 */
void get_right_shape(short obj, FixedVector *direction)
{
    FixedVector right = { 0x100, 0, 0 };
    FixedVector up = { 0, 0x100, 0 };
    FixedVector forward = { 0, 0, 0x100 };
    FixedVector objectForward;
    FixedVector eyeUp;
    FixedVector projectedUp;
    SphericalVector spherical;
    enum ObjectClass objectClass;
    enum ObjectType type;
    short pitchBand;
    short yawSector;
    short directionIndex;
    short frame;
    short remainder;
    short angle;
    short slot;

    negate_vector(direction);
    rectangular_to_spherical(direction, &spherical);
    rotate_about_j((short)-spherical.yaw, &right, &forward);
    rotate_about_i((short)-spherical.pitch, &up, &forward);
    NormalizeFixedVector(&up);
    NormalizeFixedVector(&forward);
    transform_to_objects_frame(&forward, &objectForward, obj);
    transform_to_objects_frame(&g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT],
                               &eyeUp, obj);
    rectangular_to_spherical(&objectForward, &spherical);
    pitchBand = (short)(spherical.pitch / 30 + 3);
    remainder = (short)(spherical.pitch % 30);
    if (remainder >= 16) {
        pitchBand++;
        if (pitchBand > 5)
            pitchBand = 6;
    } else if (remainder < -15) {
        pitchBand--;
        if (pitchBand < 1)
            pitchBand = 0;
    }
    yawSector = (short)((12 - spherical.yaw / 30) % 12);
    remainder = (short)(spherical.yaw % 30);
    if (remainder >= 16)
        yawSector = (short)((yawSector + 11) % 12);
    else if (remainder < -15)
        yawSector = (short)((yawSector + 1) % 12);
    if (yawSector < 0)
        yawSector += 12;
    if (pitchBand == 0)
        directionIndex = 0;
    else if (pitchBand == 6)
        directionIndex = 61;
    else
        directionIndex = (short)(pitchBand * 12 + yawSector - 11);

    projectedUp.x = dot_product(
        &eyeUp, &g_aDirectionViewRightVector_005a6c20[directionIndex]);
    projectedUp.y = dot_product(
        &eyeUp, &g_aDirectionViewUpVector_005a6f10[directionIndex]);
    projectedUp.z = 0;
    NormalizeFixedVector(&projectedUp);
    angle = (short)ArcCos(projectedUp.y);
    if (projectedUp.x >= 0)
        angle = (short)(360 - angle);

    objectClass = g_aeObjectClass_0059d100[obj];
    type = g_aeObjectType_0059b560[obj];
    if (objectClass == OBJECT_CLASS_MISSILE ||
        type == OBJECT_TYPE_TURRET) {
        directionIndex += WC1_DIRECTION_VIEW_COUNT;
    } else if (type == OBJECT_TYPE_KILRATHI_BASE) {
        directionIndex += WC1_DIRECTION_VIEW_COUNT * 2;
    }
    frame = g_acDirectionShapeFrame_0046db28[directionIndex];
    if (frame == 0)
        angle += 90;
    if (frame == 36 &&
        objectClass != OBJECT_CLASS_MISSILE)
        angle -= 90;
    g_asObjectFlip_0059c870[obj] =
        (short)(g_acDirectionShapeFlip_0046dbe8[directionIndex] << 4);
    angle %= 360;
    if (angle < 0)
        angle += 360;
    g_asObjectScreenAngle_0059cd90[obj] = angle;

    if (objectClass == OBJECT_CLASS_CAPITAL_SHIP) {
        if (g_asCapitalShipViewFrame_0059dd90[obj] != frame) {
            slot = 1;
            do {
                if (g_aObjectResourceSlots_0059ddf0[slot].type ==
                    (signed char)type) {
                    break;
                }
                slot++;
            } while (slot < 3);
            g_asObjectViewFrame_0059d230[obj] = 0;
            g_asCapitalShipViewFrame_0059dd90[obj] = frame;
            if (DAT_005a7510.pixels != 0 &&
                IdentityWord(
                    (unsigned short)g_apObjectShape_0059d2f0[obj]) == 0) {
                GetScreenUpdateFlag();
            }
            if (g_aiPacketReferenceTable_00465c88[slot][frame] != 0) {
                g_apObjectShape_0059d2f0[obj] = (unsigned char *)
                    g_aiPacketReferenceTable_00465c88[slot][frame];
            } else {
                if (DAT_005a7510.pixels != 0)
                    GetScreenUpdateFlag();
                g_cCapitalShipLogicalFile_005a7da0 =
                    (signed char)(type + 22);
                g_apObjectShape_0059d2f0[obj] =
                    (unsigned char *)FetchDiskPacketRetrying(
                        (short)g_cCapitalShipLogicalFile_005a7da0,
                        g_asCapitalShipViewFrame_0059dd90[obj], 0);
            }
            initialize_view_buffer();
        }
    } else {
        g_asObjectViewFrame_0059d230[obj] = frame;
    }
}

/* Function start: 0x41A9D0 */
short InitializeModalTextPanel(ModalTextPanel *panel, short fontIndex,
                               unsigned int topLeft,
                               unsigned int bottomRight,
                               short clearColour,
                               unsigned char backgroundColour,
                               short borderColour)
{
    memcpy(&panel->left, &topLeft, sizeof(topLeft));
    memcpy(&panel->right, &bottomRight, sizeof(bottomRight));
    panel->previousContext = g_pCurrentTextContext_0059af8c;
    g_pCurrentTextContext_0059af8c = &panel->context;
    panel->context = g_stDefaultTextContext_005a7740;
    if (fontIndex == -1)
        fontIndex = 1;
    InitializeTextContextFromFont(&panel->context, fontIndex,
                                  (unsigned char)clearColour,
                                  backgroundColour);
    panel->viewport = g_stModalSourceViewport_005a7670;
    memcpy(&panel->savedBackground.left, &panel->left,
           sizeof(topLeft) + sizeof(bottomRight));
    memcpy(&panel->viewport.left, &panel->left,
           sizeof(topLeft) + sizeof(bottomRight));
    if (AllocateViewport(&panel->savedBackground, clearColour, 0) == 0)
        return 0;
    CopyViewportContents(&panel->viewport, &panel->savedBackground);
    panel->context.text = g_szTextScratchBuffer_00598b00;
    panel->context.viewport = &panel->viewport;
    ResetStringBuilder(&panel->context);
    EraseTextContextBackground(&panel->context);
    DrawViewportBorder(&panel->viewport, panel->left, panel->top,
                       panel->right, panel->bottom, borderColour);
    return 1;
}

/* Function start: 0x41AAE0 */
void DrawModalTextPanel(ModalTextPanel *panel, short x, short y,
                        unsigned char alignment,
                        const char *format, ...)
{
    char text[84];

    vsprintf(text, format, (char *)(&format + 1));
    SetTextCursor((unsigned short)(panel->left + x),
                  (unsigned short)(panel->top + y));
    panel->context.alignment = alignment;
    strcat(text, "%P");
    FormatTextBufferFromStart(text);
}

/* Function start: 0x41AB60 */
void RestoreModalTextPanel(ModalTextPanel *panel)
{
    CopyViewportContents(&panel->savedBackground, &panel->viewport);
    free_viewport(&panel->savedBackground);
    g_pCurrentTextContext_0059af8c = panel->previousContext;
}

/* Function start: 0x41AB90 */
short ShowModalTextPanel(short fontIndex, const char *format, ...)
{
    unsigned int topLeft;
    unsigned int bottomRight;
    short halfWidth;
    char text[52];

    vsprintf(text, format, (char *)(&format + 1));
    topLeft = g_dwModalBoundsTopLeft_00469440;
    bottomRight = g_dwModalBoundsBottomRight_00469444;
    if (g_pModalTextPanel_00469448 == 0) {
        g_pModalTextPanel_00469448 = (ModalTextPanel *)
            AllocateTaggedMemory(sizeof(ModalTextPanel), 0);
    }
    if (g_pModalTextPanel_00469448 == 0)
        return 0;
    if (InitializeModalTextPanel(g_pModalTextPanel_00469448, fontIndex,
                                 topLeft, bottomRight,
                                 DAT_0046999c, DAT_0046999c,
                                 DAT_0046999c) == 0) {
        ReleasePacketHandle((int)g_pModalTextPanel_00469448);
        g_pModalTextPanel_00469448 = 0;
        return 0;
    }
    halfWidth = MeasureTextPixelWidthClamped(text);
    halfWidth = (short)(((int)halfWidth * 8 +
        (((int)halfWidth * 8 >> 31) & 15)) >> 4);
    RestoreModalTextPanel(g_pModalTextPanel_00469448);
    *(short *)&topLeft = (short)(159 - halfWidth);
    *(short *)&bottomRight = (short)(161 + halfWidth);
    if (InitializeModalTextPanel(g_pModalTextPanel_00469448, fontIndex,
                                 topLeft, bottomRight,
                                 g_cViewportClearColour_004699a0,
                                 DAT_004699a4, DAT_004699ac) == 0) {
        ReleasePacketHandle((int)g_pModalTextPanel_00469448);
        g_pModalTextPanel_00469448 = 0;
        return 0;
    }
    DrawModalTextPanel(g_pModalTextPanel_00469448, 0, 6, 2, text);
    DIBslam();
    DIBslamReal();
    return 1;
}

/* Function start: 0x41AD10 */
void ReleaseModalTextPanel(void)
{
    if (g_pModalTextPanel_00469448 != 0) {
        RestoreModalTextPanel(g_pModalTextPanel_00469448);
        ReleasePacketHandle((int)g_pModalTextPanel_00469448);
        g_pModalTextPanel_00469448 = 0;
        DIBslam();
        DIBslamReal();
    }
}

/* Function start: 0x41AD50 */
short AnySavedGames(void)
{
    SaveGameRecord gameRecord;
    short slot;
    short found;

    found = 0;
    slot = 0;
    do {
        if (LoadGame(slot, &gameRecord) != 0) {
            found = 1;
            if (gameRecord.campaign.campaignIndex > 0)
                DAT_005a7d9c = 1;
        }
        slot++;
    } while (slot < 8);
    return found;
}
