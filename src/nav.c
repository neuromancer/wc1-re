/*
 *  Nav map, location readouts and the virtual screen.
 *
 *  Address range 0x40d000-0x40ffff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: DrawNav* family; string band 0x4687AC-0x4688F4.
 */
#include "wc1.h"

/* Function start: 0x40D1D0 */
void ClearNavHazardFlag(void)
{
    DAT_00468710 = 0;
}

/* Function start: 0x40D240 */
void ClearNavLegendFlag(void)
{
    DAT_0046870c = 0;
}

/* Function start: 0x40D8C0 */
void SetScreenClipRect(unsigned short a, unsigned short b, unsigned short c, unsigned short d)
{
    SetRectBounds((int)&DAT_005a76b0, a, b, c, d);
}

/* Function start: 0x40DE70 */
void FormatNavCoordinates(unsigned char *out)
{
    unsigned char tmp[4];

    SplitGameClockToHms(tmp);
    out[1] = tmp[2];
    out[0] = tmp[3];
}

/* Function start: 0x40DF50 */
char *GetNavNameSkippingMarker(short i)
{
    char *p = *(char **)(&DAT_0059dacf[0] + i * 0x1f);

    if (*p == '?')
        p = p + 1;
    return p;
}

/* Function start: 0x40EFE0 */
void NudgeObjectX(short i, short dx, short dy)
{
    short *p = (short *)(DAT_00598a30[i] + 0x20);

    (void)dy;
    *p = *p + dx;
}

/* Function start: 0x40FA30 */
unsigned short GetPaletteReadyUnused(void)
{
    return 1;
}
