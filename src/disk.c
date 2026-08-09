/*
 *  Disk data files and packet fetching with retry.
 *
 *  Address range 0x41d250-0x41efff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: OpenDiskDataFile/FetchDiskPacketRetrying/PromptInsertNumberedDisk.
 */
#include "wc1.h"

/* Function start: 0x41D2E0 */
void *FetchDiskPacketRetrying(short logicalFile, short section,
                              unsigned short flags)
{
    const char *filename;
    void *packet = 0;
    short retries = 5;

    if (DAT_005a7cf0 == 0 || logicalFile < 0) {
        DAT_00465460 = 3;
        return 0;
    }
    filename = (const char *)(DAT_005a7cf0 + logicalFile * 16);
    do {
        FreePacketAndClear((int *)&packet);
        packet = PacketLoad(filename, section, 0, flags);
        retries--;
    } while (packet == 0 && retries > 0 && DAT_00465460 != 8);

    if (packet == 0 && DAT_00465460 == 4 && DAT_005a7510.pixels != 0) {
        free_viewport(&DAT_005a7510);
        do {
            FreePacketAndClear((int *)&packet);
            packet = PacketLoad(filename, section, 0, flags);
            retries--;
        } while (packet == 0 && retries > 0 && DAT_00465460 != 8);
        AllocateViewport(&DAT_005a7510, (short)DAT_004699d8, 0x20);
    }
    if (packet == 0 && DAT_00465460 == 4 && DAT_005a76b0.pixels != 0) {
        free_viewport(&DAT_005a76b0);
        do {
            FreePacketAndClear((int *)&packet);
            packet = PacketLoad(filename, section, 0, flags);
            retries--;
        } while (packet == 0 && retries > 0 && DAT_00465460 != 8);
        AllocateViewport(&DAT_005a76b0, (short)DAT_0046999c, 0);
    }
    return packet;
}

/* Function start: 0x41DA00 */
unsigned int GetZeroUnused(void)
{
    return 0;
}

/* Function start: 0x41DEB0 */
void WaitForStreamIdle(void)
{
    unsigned int saved = DAT_0046505c;

    DAT_0046505c = 1;
    while (UiRoutine03() == 0)
        ;
    DAT_0046505c = saved;
}

/* Function start: 0x41DF40 */
short FindActiveShipSlot(void)
{
    short i = 0x22;

    do {
        if (g_aeObjectClass_0059d100[i] == OBJECT_CLASS_DUST)
            return i;
        i = i + 1;
    } while (i < 0x2a);
    return -1;
}

/* Function start: 0x41E710 */
unsigned int AdjustShipSpeed(short ship, int delta)
{
    int maximumSpeed = (int)g_asShipMaximumSpeed_0059c440[ship] << 8;

    g_anShipSpeed_0059b320[ship] += delta;
    if (maximumSpeed < g_anShipSpeed_0059b320[ship])
        g_anShipSpeed_0059b320[ship] = maximumSpeed;
    if (g_anShipSpeed_0059b320[ship] < 0)
        g_anShipSpeed_0059b320[ship] = 0;
    return 0;
}

/* Function start: 0x41E750 */
unsigned int ApproachShipSpeed(short ship, int targetSpeed)
{
    int delta;
    int acceleration;

    acceleration = GetShipAccelerationRate(ship);
    delta = targetSpeed - g_anShipSpeed_0059b320[ship];

    if ((short)ShipAiRoutine16(ship, 1))
        acceleration += acceleration;
    if ((delta < 0 ? -delta : delta) > acceleration)
        delta = MultiplyFixed(SignFixed(delta), acceleration);
    AdjustShipSpeed(ship, delta);
    return 0;
}

/* Function start: 0x41E7C0 */
unsigned int steady_object(short ship)
{
    DAT_0059c310[ship] = 0;
    DAT_0059d7a0[ship] = 0;
    DAT_0059d630[ship] = 0;
    return 0;
}

/* Function start: 0x41E7F0 */
short real_velocity(short obj)
{
    return FixedToShortSaturating(
        (int)ComputeFixedVectorMagnitude(&g_aShipVelocity_0059c010[obj]));
}

/* Function start: 0x41E820 */
unsigned int fix_velocity(short obj)
{
    ScaleFixedVector(&g_aShipForwardVector_0059bce0[obj],
                     g_anShipSpeed_0059b320[obj],
                     &g_aShipVelocity_0059c010[obj]);
    return 0;
}

/* Function start: 0x41E860 */
unsigned int sort_viable_target_list(void)
{
    short outer;
    short inner;
    short distance;
    signed char target;
    short count = (short)g_cViableTargetCount_0046c088;

    for (outer = 0; outer < count - 1; outer++) {
        for (inner = outer + 1; inner < count; inner++) {
            if (g_asViableTargetDistance_0059c470[inner] <
                g_asViableTargetDistance_0059c470[outer]) {
                distance = g_asViableTargetDistance_0059c470[outer];
                g_asViableTargetDistance_0059c470[outer] =
                    g_asViableTargetDistance_0059c470[inner];
                g_asViableTargetDistance_0059c470[inner] = distance;
                target = g_acViableTarget_0059c920[outer];
                g_acViableTarget_0059c920[outer] =
                    g_acViableTarget_0059c920[inner];
                g_acViableTarget_0059c920[inner] = target;
            }
        }
    }
    return 0;
}
