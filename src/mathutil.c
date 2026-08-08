/*
 *  Shared short-integer helpers.
 *
 *  MODULE BOUNDARY UNVERIFIED.  These two are adjacent at 0x0041D0C0/0x0041D0E0
 *  and FreePacketAndClear follows at 0x0041D100, so they are probably all one
 *  original compilation unit.  Split or merge once `make order` says so.
 *
 *  Both take and return `short`: the core was ported from 16-bit DOS C where
 *  `int` was 16 bits.  Using `int` here emits 32-bit compares and does not match.
 *
 *  NOTE the early-return shape.  Writing these the other obvious way --
 *      if (b <= a) a = b; return a;
 *  -- compiles to `mov ax, cx` where the original has the one-byte-shorter
 *  `mov eax, ecx`.  Two `return` statements reproduce the original exactly.
 *  See docs/PATTERNS.md.
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
