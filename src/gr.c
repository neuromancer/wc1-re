/*
 *  Rasteriser primitives and screen-space effects.
 *
 *  Address range 0x440c00-0x44274f (provisional -- see docs/ORDER.md).
 *  Boundary evidence: PROVEN by name: shadow_draw, fizzle_fade, snow_viewport.
 */
#include "wc1.h"

/* Function start: 0x440CF0 */
void ClipViewportToScreen(int *p)
{
    ValidateViewportBounds(p, (int *)&DAT_004875a8, &DAT_00496fc0);
}

/* Function start: 0x441140 */
void RasterLineHook(void)
{
}

/* Function start: 0x4413C0 */
void SetPaletteEntry(short a, unsigned short *b)
{
    SetPaletteEntryFromWords(a, b);
}
