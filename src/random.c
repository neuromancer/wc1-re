/*
 *  Random helpers.
 *
 *  MODULE BOUNDARY UNVERIFIED.  Adjacent to the CRT-matched _rand at 0x00434D10.
 *
 *  RandomInRange ends in `RET 8` -- callee cleanup -- so it is __stdcall in the
 *  original, while RandomBelowOrEqual ends in a bare `RET` and is __cdecl.
 *  Calling conventions are not uniform in this program; see docs/PATTERNS.md.
 */
#include "wc1.h"
#include <stdlib.h>

/* Function start: 0x434D20 */
short __stdcall RandomInRange(short lo, short hi)
{
    short span = hi - lo;

    if (span == 0)
        span = 1;
    return lo + rand() % (span + 1);
}

/* Function start: 0x434D50 */
short RandomBelowOrEqual(short n)
{
    if (n != -1 && n != 0)
        return (short)(rand() % (n + 1));
    return 0;
}
