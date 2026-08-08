/*
 *  Recovered global state definitions.
 *
 *  Declaration order fixes the data layout, which binary-comp compares against
 *  the original.  Do not reorder without checking `make globals-data`.
 *
 *  This file is deliberately near-empty: nothing should be added here that has
 *  not been located in the binary.  Use `make missing-data` to list nonzero
 *  data slots in the game's own static range (0x00465000-0x004751ff) that are
 *  still unaccounted for.
 */
#include "globals.h"

short g_nFrameSkip_00469fb8 = 1;              /* int _FrameSkip = 1; */

unsigned char g_bCurrentKey_0046c014 = 0;
unsigned char g_bMessageSpeed_0046af68 = 0;
short g_nOriginDevUnlock_00469ff4 = 0;
unsigned long g_dwGameClockBase_005a89a8 = 0;
