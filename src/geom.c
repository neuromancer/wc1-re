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

/* Function start: 0x418280 */
void AddShipAiTimer(short i, short delta)
{
    DAT_0059b470[i] = DAT_0059b470[i] - (int)delta;
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

/* Function start: 0x4185F0 */
void ZeroVector(unsigned int *p)
{
    p[2] = 0;
    p[1] = 0;
    p[0] = 0;
}

/* Function start: 0x418600 */
void ZeroVectorPtr(int *p)
{
    p[0] = -p[0];
    p[1] = -p[1];
    p[2] = -p[2];
}

/* Function start: 0x4187E0 */
void ScaleVectorByZero(short a, int *p)
{
    MakeRandomVectorFixed(0, a, p);
}

/* Function start: 0x419260 */
short GetShipVectorComponent(short a, short i)
{
    return GetHeadingErrorToPoint(a, &DAT_0059c490[0] + i * 3) - DAT_0059d710[i];
}

/* Function start: 0x419390 */
void StoreShipVector(short a, int *p)
{
    UpdateAimSolution(a, p);
}

/* Function start: 0x4193B0 */
void SetShipAiOrder(short a, short b)
{
    ShipAiRoutine04(a, b);
}

/* Function start: 0x4196A0 */
void ReadShipRotationRow(short a, short i)
{
    AimShipAtPoint(a, &DAT_0059c490[0] + i * 3);
}

/* Function start: 0x419850 */
void WriteShipRotationRow(short a, short i)
{
    AimShipAtPointOffset90(a, &DAT_0059c490[0] + i * 3);
}

/* Function start: 0x419970 */
void SetVectorFixedPoint(unsigned int *p, short v)
{
    NormaliseAndScaleVector(p, (int)v << 8);
}

/* Function start: 0x419B70 */
short FindShipInMode1(void)
{
    DAT_0046c010 = 1;
    do {
        if (g_abShipSlotState_0059d100[DAT_0046c010] == 0)
            return DAT_0046c010;
        DAT_0046c010 = DAT_0046c010 + 1;
    } while (DAT_0046c010 < 10);
    DAT_0046c010 = -1;
    return -1;
}

/* Function start: 0x419BA0 */
short FindShipInMode2(void)
{
    short i = 10;

    do {
        if (g_abShipSlotState_0059d100[i] == 0) {
            DAT_0059d9b0[i] = 0x8001;
            return i;
        }
        i = i + 1;
    } while (i < 0x3d);
    return -1;
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
