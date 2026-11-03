/**
 * @file
 * @ingroup data
 * @brief Integer min and max, a packet-release helper, and the
 * single-precision angular constants.
 *
 * Address range 41d0c0-41d11f. MinShort and MaxShort have 94 call sites
 * across the game core. The constants' read-only data follows the barracks
 * menu regions and precedes the DirectDraw GUIDs, as only a unit linked
 * between those two can place it.
 */
#include "wc1.h"

/* Nothing in the shipped executable references these three values. */
const float fPi = 3.1415927410125732421875f; /* 0x004630dc */
const float fDegreesPerRadian = 57.295780181884765625f; /* 0x004630e0 */
const float fRadiansPerDegree = 0.01745329238474369049072265625f; /* 0x004630e4 */

/* Function start: 0x41D0C0 */
short MinShort(short a, short b)
{
    if (a < b)
        return a;
    return b;
}

/* One cockpit sizing call supplies a third argument; only the first two are read. */
/* Function start: 0x41D0E0 */
short MaxShort(short a, short b, ...)
{
    if (a > b)
        return a;
    return b;
}

/* Function start: 0x41D100 */
void FreePacketAndClear(void *slot, unsigned short releaseFlags)
{
#ifdef SDL_PORT
    void *packet;

    memcpy(&packet, slot, sizeof(packet));
    if (packet != 0) {
        ReleasePacketHandle(packet);
        packet = 0;
        memcpy(slot, &packet, sizeof(packet));
    }
#else
    void **p;

    p = slot;
    if (*p != 0) {
        ReleasePacketHandle(*p);
        *p = 0;
    }
#endif
}
