/*
 *  Mission setup: FX drivers, palette load, ship state bits.
 *
 *  Address range 0x421000-0x424fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: LoadOriginFxDrivers/StartExtendedMemoryManager; string band 0x469A28-0x469B9C.
 */
#include "wc1.h"

/* Function start: 0x4213B0 */
unsigned int ShipAiRoutine15(short ship)
{
    DAT_0059c810[ship] = 10;
    ShipAiRoutine21(ship, 3);
    return 0;
}

/* Function start: 0x421760 */
unsigned int FlagShipAiIfPositive(short i)
{
    if (0 < DAT_0059b320[i])
        AddShipAiTimer(i, 5);
    return 0;
}

/* Function start: 0x421A40 */
unsigned int LeaveWaitCursorScope(void)
{
    if (DAT_0059a850 != 0)
        ClearWaitCursorFlag();
    return 0;
}

/* Function start: 0x421FE0 */
unsigned int GetFxDriverInitResult(void)
{
    return 0;
}

/* Function start: 0x421FF0 */
unsigned int GetMessagePumpResult(void)
{
    return 0;
}

/* Function start: 0x422000 */
unsigned int GetFxDriverStatus(void)
{
    return 0;
}

/* Function start: 0x422010 */
int TestShipFlags(short i, unsigned char bits)
{
    return (DAT_0059ca94[i] & bits) == bits;
}

/* Function start: 0x422030 */
void ClearShipFlags(short i, unsigned char bits)
{
    DAT_0059ca94[i] &= ~bits;
}

/* Function start: 0x422050 */
void SetShipFlags(short i, unsigned char bits)
{
    DAT_0059ca94[i] |= bits;
}

/* Function start: 0x4220D0 */
void ClearShipTimer(short i)
{
    ClearShipFlags(i, 0x1a);
    SetShipFlags(i, 0x20);
}

/* Function start: 0x4220F0 */
short RandomCentred(short range)
{
    return (short)RandomBelowOrEqual(range * 2) - range;
}

/* Function start: 0x422110 */
int ShipAiRoutine16(short ship, unsigned int bits)
{
    return (DAT_0059b430[ship] & bits) != 0;
}

/* Function start: 0x422130 */
/* NOT a constant return: `mov eax,0x59b430 / cmp eax,1 / sbb eax,eax / inc eax`
 * is the classic `>= 1` boolean idiom applied to the *address* 0x0059B430,
 * which is always non-zero -- hence Ghidra folding it to `return 1`.  Written
 * the way the original was, so the idiom survives. */
unsigned int HasSpeechBuffer(void)
{
    return (unsigned int)&DAT_0059b430 >= 1;
}

/* Function start: 0x422140 */
unsigned int SetShipStateBits(short i, unsigned int bits)
{
    DAT_0059b430[i] |= bits;
    return bits & 0xffff0000;
}

/* Function start: 0x422160 */
void ResetShipStateRecord(short i)
{
    DAT_0059d2d0[i] = 0;
    DAT_0059b430[i] = 0;
    DAT_0059cf20[i] = 0xff;
}

/* Function start: 0x422440 */
void ClearWingmanSlots(void)
{
    short i = 0;

    do {
        DAT_005a7cc0[i] = 0xffff;
        i = i + 1;
    } while (i < 10);
}

/* Function start: 0x422830 */
unsigned int ShipAiRoutine19(short ship)
{
    return DAT_0059c310[ship] == 0 && DAT_0059d7a0[ship] == 0 && DAT_0059d630[ship] == 0;
}

/* Function start: 0x422CD0 */
unsigned int SetShipOrderWord(short i, unsigned int v)
{
    ShipAiRoutine20(i, -1);
    DAT_0059d5e0[i] = v;
    DAT_0059ce60[i] = 0xff;
    return 0;
}

/* Function start: 0x422D00 */
unsigned int ToggleShipOrderWord(short i, unsigned int v)
{
    ShipAiRoutine20(i, -1);
    DAT_0059d5e0[i] = v;
    return 0;
}

/* Function start: 0x422D30 */
void ShipAiRoutine20(short ship, short state)
{
    DAT_0059dcb0[ship] = (unsigned char)state;
    DAT_0059c420[ship] = 0;
    DAT_0059d520[ship] = 0;
}

/* Function start: 0x422DD0 */
unsigned int ClearShipAiThrottle(short ship)
{
    ShipAiRoutine07(ship, 0);
    return 0;
}

/* Function start: 0x422DF0 */
unsigned int SetShipAiThrottleHalf(short ship)
{
    ShipAiRoutine07(ship, 0x500);
    return 0;
}

/* Function start: 0x422E50 */
unsigned int ShipAiRoutine22(short ship)
{
    ShipAiRoutine07(ship,
        (int)*(short *)(&DAT_00466472[0] + DAT_0059b560[ship] * 0x87) << 8);
    return 0;
}

/* Function start: 0x422E80 */
unsigned int ShipAiRoutine23(short ship)
{
    ShipAiRoutine07(ship, (int)DAT_0059c440[ship] << 8);
    return 0;
}

/* Function start: 0x422EA0 */
unsigned int SetShipAiThrottleFromTable(short ship, short i)
{
    ShipAiRoutine07(ship, DAT_0059b320[i]);
    return 0;
}

/* Function start: 0x422F60 */
unsigned int IsBehindAndFalling(short ship)
{
    if (DAT_0059ce10 < ship && DAT_0059d52a < -0x32)
        return 1;
    return 0;
}

/* Function start: 0x423610 */
unsigned int GetObjectRecordField(short i)
{
    if (i != -1 && DAT_0046c97a[i * 0x36] != 3)
        return 0;
    return 1;
}

/* Function start: 0x423CD0 */
void IssueShipAiOrder21(short a)
{
    FindShipCommEntry(a, 0x21);
}

/* Function start: 0x424B80 */
unsigned int GetPilotNameLength(void)
{
    short i = 0;

    do {
        DestroyShip(i);
        i = i + 1;
    } while (i < 0x40);
    return 0;
}

/* Function start: 0x424BA0 */
unsigned int GetPilotRecordBase(void)
{
    if (DAT_00469d5c == 0)
        return 0;
    DAT_00469d5c = 0;
    GetScreenUpdateFlag();
    DAT_0046a004 = 0;
    ReleaseSceneActors();
    ReleaseAllSfxSlots();
    GetPilotNameLength();
    ResetSeriesProgress();
    return 0;
}

/* Function start: 0x424C60 */
unsigned int ResetSceneFlags(void)
{
    DAT_0046aa3c = 0;
    DAT_0046aa40 = 1;
    DAT_0046aa38 = 1;
    return 0;
}

/* Function start: 0x424CE0 */
unsigned int MouseHide(void)
{
    return 0;
}

/* Function start: 0x424CF0 */
unsigned int MouseShow(void)
{
    return 0;
}
