/*
 *  Text formatting and the DirectDraw error-string table.
 *
 *  Address range 0x433400-0x433abf (provisional -- see docs/ORDER.md).
 *  DirectDrawResultToText at 0x4331f0 remains in the preceding DIB tranche;
 *  the Mac symbol order proves that the ship-AI `smart` unit starts at
 *  0x433ac0.
 */
#include "wc1.h"
