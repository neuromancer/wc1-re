/*
 *  Ship AI: dispatch-table states and the behaviour routines.
 *
 *  Address range 0x404000-0x40cfff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: the 47-slot dispatch table at 0x004656a8 targets this range almost exclusively.
 */
#include "wc1.h"

/* Function start: 0x4060A0 */
void SetShipAiScratchWord(unsigned short v) { DAT_00475e78 = v; }

/* Function start: 0x4060B0 */
void SelectNewShipAiBehavior(short ship)
{
    ShipAiRoutine21(ship, -1);
    ShipAiRoutine20(ship, -1);
}

/* Function start: 0x4061E0 */
void ShipAiRoutine01(short ship)
{
    if (DAT_0059dcb0[ship] != 0xff)
        DAT_0059d520[ship] = DAT_0059d520[ship] + 1;
}

/* Function start: 0x407030 */
void ShipAiState30(short ship, short target)
{
    if (DAT_0059d52a < 0x50) {
        ShipAiState29(ship, target);
        return;
    }
    ShipAiState40(ship, target);
}

/* Function start: 0x4070D0 */
void ShipAiState31(short ship, short target)
{
    if (0 < DAT_0059c330[ship]) {
        DAT_0059d630[ship] = 0x2d;
        return;
    }
    ShipAiState29(ship, target);
}

/* Function start: 0x407350 */
void ShipAiState24(short ship, unsigned int arg) { ShipAiRoutine02(ship, arg, 0); }

/* Function start: 0x407370 */
void ShipAiState34(short ship, unsigned int arg) { ShipAiRoutine02(ship, arg, 0x23); }

/* Function start: 0x407560 */
void ShipAiState44(short ship) { DAT_0059d620[ship] = 0; SelectNewShipAiBehavior(ship); }

/* Function start: 0x407580 */
void ShipAiState27(short ship, short target) { ShipAiState30(ship, target); }

/* Function start: 0x4075A0 */
void ShipAiState21(short ship) { ClearShipAiOrders(ship); }

/* Function start: 0x4075B0 */
/* Empty in the original: dispatch-table slots 0 and 1 (no-op / invalid state). */
void ShipAiStateNoOp(void) {}

/* Function start: 0x4075C0 */
void ShipAiState03(short ship) { SelectNewShipAiBehavior(ship); }

/* Function start: 0x4098C0 */
void ClearShipTarget(short a) { DoShipAiFn2C70(a, -1); }

/* Function start: 0x409CE0 */
void DisableInputCapture(void) { DAT_0046c080 = 0; }

/* Function start: 0x409CF0 */
void EnableInputCapture(void) { DAT_0046c080 = 1; DAT_0046c084 = 0xffff; }

/* Function start: 0x40A130 */
void ClearNavSelection(short i)
{
    if (DAT_0059d650[i] == 0) {
        DispatchDebugFn9F80(i);
        return;
    }
    DispatchTbl0059d400FnA030(i);
}

/* Function start: 0x40A160 */
void UpdateShipAiVector(short ship)
{
    GetTbl0059d710Fn9210(ship, &DAT_0059dd10[0] + ship * 0xc);
}

/* Function start: 0x40A400 */
void RunShipAiDispatch(short a) { DispatchTbl0059d5e0FnA360(a); }

/* Function start: 0x40B110 */
void PromoteShipAiState15(short i)
{
    if (DAT_0059b560[i] == 0x15) {
        DAT_0059ce80[i] = 4;
        ScanTbl0059cab0Fn0AA0(i);
    }
}

/* Function start: 0x40B700 */
unsigned int GetShipMode(short i)
{
    unsigned int prev;

    if (DAT_0059c3f0[i] == 7) {
        prev = DAT_0059d100[i];
        DAT_0059d100[i] = 1;
        DAT_0059c330[i] = (short)prev;
    }
    return 0;
}

/* Function start: 0x40BEA0 */
void ResetNavCursor(void)
{
    short i = 0;

    do {
        ReleaseTbl0046c028Fn9BD0(i);
        i = i + 1;
    } while (i < 10);
}

/* Function start: 0x40C350 */
unsigned int GetObjectVisible(void) { return 1; }

/* Function start: 0x40CBB0 */
void SetNavCursorIndex(unsigned short v) { DAT_00468660 = v; }

/* Function start: 0x40CBC0 */
void ObjectDrawHook(short *p) { *p = (short)((int)*p / (DAT_00468664 * 100)); }
