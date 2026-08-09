/*
 *  Text formatting and the DirectDraw error-string table.
 *
 *  Address range 0x433400-0x4348ff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: DirectDrawResultToText holds 122 string references, the largest single block.
 */
#include "wc1.h"

/* Function start: 0x4345D0 */
int GetShipIfArmorBelow15(short i)
{
    if ((char)DAT_0059d620[i] < 0x0f)
        return 0;
    return i;
}
