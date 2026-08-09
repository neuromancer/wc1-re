/*
 *  Vector, angle and fixed-point geometry helpers.
 *
 *  Address range 0x418000-0x41a9ff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: 16-bit clamp/sign/wrap leaves with no string or global references.
 */
#include "wc1.h"

/* Function start: 0x418130 */
unsigned short GetMusicDriverPresent(void)
{
    return 1;
}

/* Function start: 0x4181C0 */
short get_ship_max_velocity(short obj)
{
    short velocity = g_aObjectTypeData_0046645c[
        g_aeObjectType_0059b560[obj]].maximumVelocity;

    if (obj < 10 && g_acShipRating_0059cd80[obj] > 8)
        return velocity + velocity / 3;
    return velocity;
}

/* Function start: 0x418280 */
void AddShipAiTimer(short i, short delta)
{
    DAT_0059b470[i] = DAT_0059b470[i] - (int)delta;
}

/* Function start: 0x4182F0 */
int GetShipAccelerationRate(short ship)
{
    int shipIndex = (int)ship;
    enum ObjectType objectType = g_aeObjectType_0059b560[shipIndex];
    int acceleration = g_aObjectTypeData_0046645c[objectType].acceleration;

    if (ship < 10 && g_acShipRating_0059cd80[shipIndex] > RATING_ACE_ICEMAN)
        return acceleration + acceleration / 3;
    return acceleration;
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
    if (v < 1)
        return 0;
    return 0x100;
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

/* Function start: 0x4185F0 */
void zero_vector(FixedVector *vector)
{
    vector->z = 0;
    vector->y = 0;
    vector->x = 0;
}

/* Function start: 0x418600 */
void ZeroVectorPtr(int *p)
{
    p[0] = -p[0];
    p[1] = -p[1];
    p[2] = -p[2];
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

/* Function start: 0x4189E0 */
int dot_product(const FixedVector *left, const FixedVector *right)
{
    return MultiplyFixed(left->x, right->x) +
           MultiplyFixed(left->y, right->y) +
           MultiplyFixed(left->z, right->z);
}

/* Function start: 0x418B10 */
short NormalizeFixedVector(FixedVector *vector)
{
    int magnitude = ComputeFixedVectorMagnitude(vector);

    if (magnitude != 0) {
        vector->x = DivideFixed(vector->x, magnitude);
        vector->y = DivideFixed(vector->y, magnitude);
        vector->z = DivideFixed(vector->z, magnitude);
        return 1;
    }
    return 0;
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

/* Function start: 0x4190B0 */
void transform_to_objects_frame(const FixedVector *source,
                                FixedVector *destination, short obj)
{
    destination->x = dot_product(source, &g_aShipRightVector_0059b6e0[obj]);
    destination->y = dot_product(source, &g_aShipUpVector_0059b9e0[obj]);
    destination->z = dot_product(source, &g_aShipForwardVector_0059bce0[obj]);
}

/* Function start: 0x419210 */
short distance_from_point(short obj, const FixedVector *point)
{
    long magnitude;

    ComputeVectorDelta(&g_aShipPosition_0059c490[obj],
                       (FixedVector *)point, &g_vToTarget_0059d4d0);
    magnitude = ComputeFixedVectorMagnitude(&g_vToTarget_0059d4d0);
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
        (short)(((unsigned short)dot_product(
            &g_aShipForwardVector_0059bce0[obj],
            &g_vNormalizedToTarget_005a7db0) * 100) >> 8);
}

/* Function start: 0x419310 */
void get_facing_range_from_object(short obj, short other)
{
    get_facing_range_from_point(obj, &g_aShipPosition_0059c490[other]);
    g_nTargetRange_0059ce10 -= g_asObjectCollisionRadius_0059d710[other];
    ZeroVectorPtr((int *)&g_vNormalizedToTarget_005a7db0);
    g_nTargetFacing_0059d52a =
        (short)(((unsigned short)dot_product(
            &g_aShipForwardVector_0059bce0[other],
            &g_vNormalizedToTarget_005a7db0) * 100) >> 8);
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
short facing_to_object(short obj, short other)
{
    FixedVector direction;

    ComputeVectorDelta(&g_aShipPosition_0059c490[obj],
                       &g_aShipPosition_0059c490[other], &direction);
    NormalizeFixedVector(&direction);
    g_nFacingToTarget_0059d920 =
        (short)(((unsigned short)dot_product(
            &g_aShipForwardVector_0059bce0[obj], &direction) * 100) >> 8);
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
    angle = (short)ArcCosFixed(roll.y);
    if (roll.x >= 0)
        angle = 360 - angle;
    return WrapDegrees(angle);
}

/* Function start: 0x4194D0 */
int set_ship_rotation_goals(short obj, short reference,
                            const FixedVector *direction,
                            short *yawGoal, short *pitchGoal)
{
    double horizontal;
    short yaw;
    short pitch;

    (void)obj;
    if (ComputeFixedVectorMagnitude(direction) == 0)
        return 1;
    horizontal = sqrt((double)direction->x * direction->x +
                      (double)direction->z * direction->z);
    yaw = (short)(atan2((double)direction->x,
                        (double)direction->z) / WC1_DEG2RAD);
    pitch = (short)(-atan2((double)direction->y,
                           horizontal) / WC1_DEG2RAD);
    if (reference == 1) {
        yaw += reference;
        pitch += reference;
    }
    *yawGoal = WrapDegrees(-yaw);
    *pitchGoal = WrapDegrees(-pitch);
    return 0;
}

/* Function start: 0x419620 */
void point_ship(short obj, short reference, const FixedVector *direction)
{
    set_ship_rotation_goals(obj, reference, direction,
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
            DAT_0059d9b0[i] = 0x8001;
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
    DAT_0059d9b0[obj] = 0x8001;
    g_abObjectField_0059b4a0[obj] = 0;
    if (obj == DAT_00469208)
        DAT_00469208 = -1;
    if (obj == g_nYourWingman_0046c04c)
        g_nYourWingman_0046c04c = -1;
    for (slot = 0; slot < 20; slot++) {
        if (g_abSoundEffectShips_0046c028[slot] == obj) {
            g_abSoundEffectShips_0046c028[slot] = -1;
            break;
        }
    }
    if (obj < 10) {
        if (g_aeObjectClass_0059d100[obj] == OBJECT_CLASS_CAPITAL_SHIP)
            FreePacketAndClear((int *)&g_aeShipObjective_0059d200[obj + 60]);
        g_acShipRating_0059cd80[obj] = -1;
        ((signed char *)g_aeShipObjective_0059d200)[obj + 0xc0] = -1;
        g_aeShipSide_0059d650[obj] = SIDE_NEUTRAL;
        g_aeShipManeuver_0059dcb0[obj] = MANEUVER_NONE;
        ResetShipStateRecord(obj);
        *(short *)((unsigned char *)&((FixedVector *)
            g_aShipMissionSpot_0059dd10)[10].z + obj * 2) = -1;
    }
    g_aeObjectClass_0059d100[obj] = OBJECT_CLASS_NULL;
    g_aeShipObjective_0059d200[obj + 60] = OBJECTIVE_NAV_POINT;
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
