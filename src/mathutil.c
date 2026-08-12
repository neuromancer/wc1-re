/*
 *  Integer min/max used across the game core.
 *
 *  Address range 0x41d000-0x41d24f (provisional -- see docs/ORDER.md).
 *  Boundary evidence: MinShort/MaxShort pair, 94 call sites, no other content in the gap.
 */
#include "wc1.h"

/* Function start: 0x41D0C0 */
short MinShort(short a, short b)
{
    if (a < b)
        return a;
    return b;
}

/* Function start: 0x41D0E0 */
short MaxShort(short a, short b)
{
    if (a > b)
        return a;
    return b;
}

/* Function start: 0x41D100 */
void FreePacketAndClear(void *slot, unsigned short releaseFlags)
{
    void **p;

    p = (void **)slot;
    if (*p != 0) {
        ReleasePacketHandle(*p);
        *p = 0;
    }
}
