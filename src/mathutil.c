/*
 *  Integer min/max used across the game core.
 *
 *  Address range 0x41d000-0x41d24f (provisional -- see docs/ORDER.md).
 *  Boundary evidence: MinShort/MaxShort pair, 94 call sites, no other content in the gap.
 */
#include "wc1.h"

/* Function start: 0x46F3D0 */
short MinShort(short a, short b)
{
    if (a < b)
        return a;
    return b;
}

/* Function start: 0x40F072 */
short MaxShort(short a, short b)
{
    if (a > b)
        return a;
    return b;
}

/* Function start: 0x40F0A4 */
void FreePacketAndClear(void *slot, unsigned short releaseFlags)
{
#ifdef WC1_SDL
    void *packet;

    memcpy(&packet, slot, sizeof(packet));
    if (packet != 0) {
        ReleasePacketHandle(packet);
        packet = 0;
        memcpy(slot, &packet, sizeof(packet));
    }
#else
    void **p;

    p = (void **)slot;
    if (*p != 0) {
        ReleasePacketHandle(*p);
        *p = 0;
    }
#endif
}
