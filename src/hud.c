/*
 *  Cockpit HUD: weapon, damage, target and message displays.
 *
 *  Address range 0x413000-0x417fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: Draw*Panel/Report* family; string band 0x4692B8-0x4693A4.
 */
#include "wc1.h"

/* Function start: 0x413D20 */
int HasFreeMessageSlot(void)
{
    return DAT_00469068 < 8;
}

/* Function start: 0x413F70 */
unsigned int GetSeriesRecordField(char slot, int rec)
{
    int k = slot * 10;

    *(int *)(&DAT_005a7e30[0] + k) = rec;
    *(unsigned short *)(&DAT_005a7e30[4] + k) = *(unsigned short *)(rec + 4);
    *(unsigned short *)(&DAT_005a7e30[6] + k) = *(unsigned short *)(rec + 6);
    return 0;
}

/* Function start: 0x4141B0 */
void ClearHudMessageIfMatching(int *p, int v)
{
    if (*(int *)((int)p + 0xd) == v)
        ClearHudMessageSlot(p);
}

/* Function start: 0x4141D0 */
void ClearHudGunReadouts(void)
{
    DAT_005a7ddd = 0;
    DAT_005a7ddc = 0;
    DAT_005a7dee = 0;
    DAT_005a7ded = 0;
}

/* Function start: 0x4142E0 */
void ShowHudMessageIfCurrent(int v)
{
    ClearHudMessageIfMatching(&DAT_005a7de1, v);
}

/* Function start: 0x414410 */
void *ClearHudTargetVectors(void)
{
    DAT_005a7eb8 = 0;
    DAT_005a7e70 = 0;
    DAT_005a7ebc = 0;
    DAT_005a7e74 = 0;
    DAT_005a7ebe = 0;
    DAT_005a7e76 = 0;
    return &DAT_005a7eb8;
}

/* Function start: 0x4147E0 */
unsigned short GetSeriesStateWord(short i)
{
    return DAT_0059d500[((int)DAT_0059dec0[i] + i * 4) * 2];
}

/* Function start: 0x414890 */
int GetSeriesFlag(short i)
{
    return (char)DAT_0059dec0[i];
}

/* Function start: 0x4148E0 */
void ClearSeriesFlagEntry(short i)
{
    ClearHudMessageSlot((int *)(&DAT_005a7dd0[0] + i * 0x11));
    DAT_0059dec0[i] = DAT_0059dec0[i] - 1;
}

/* Function start: 0x4149C0 */
void ClearMessageSlot(short i)
{
    DAT_0059ce18[i] = 0;
}

/* Function start: 0x4149E0 */
void ClearAutopilotFlag(void)
{
    DAT_005a7dca = 0;
}

/* Function start: 0x4149F0 */
int IsAutopilotEngaged(void)
{
    return 0 < DAT_005a7dca;
}

/* Function start: 0x414A10 */
unsigned short SetAutopilotFlag(unsigned short v)
{
    DAT_005a7dca = (unsigned char)v;
    return 0;
}

/* Function start: 0x414A20 */
void RefreshAutopilotHud(void)
{
    if (IsAutopilotEngaged() && (DAT_005a7dca = DAT_005a7dca - 1, DAT_005a7dca < 1))
        EndCommMenu();
}

/* Function start: 0x414AD0 */
void PlayTargetLockSfx(void)
{
    PlaySfxWaveFileByNumber(0x18, -1, 0);
}

/* Function start: 0x414AE0 */
void PlayShieldHitSfx(void)
{
    PlaySfxWaveFileByNumber(0x1f, -1, 0);
}

/* Function start: 0x415040 */
void InputFilterHook(void)
{
}

/* Function start: 0x415050 */
int GetNavRecordField50(short i)
{
    return (DAT_0059daca[i * 0x1f] & 4) == 4;
}

/* Function start: 0x415070 */
int GetNavRecordField70(short i)
{
    return (DAT_0059daca[i * 0x1f] & 1) == 1;
}

/* Function start: 0x415090 */
int GetNavRecordField90(short i)
{
    return (DAT_0059daca[i * 0x1f] & 2) == 2;
}

/* Function start: 0x4150B0 */
void SetNavRecordFlags(short i, unsigned char bits)
{
    DAT_0059daca[i * 0x1f] |= bits;
}

/* Function start: 0x415510 */
unsigned int GetShipAiScratch(void)
{
    if (DAT_0059c3f0[0] != 1) {
        if (AnyShipReadyToJump() == 0)
            return 0;
    }
    return 1;
}

/* Function start: 0x415A70 */
void ClearWeaponHardpoints(void)
{
    short i = 10;

    do {
        DAT_005a7ea0[i] = 0;
        i = i - 1;
    } while (i != 0);
    DAT_00469090 = 0xffff;
}

/* Function start: 0x415FC0 */
void BeginMissileLockWarning(unsigned short v)
{
    DAT_0046c060 = 0;
    DAT_0046c064 = v;
    DAT_0046c068 = (short)RandomBelowOrEqual(0x167);
}

/* Function start: 0x415FF0 */
unsigned int GetHudMessageSlot(unsigned short v)
{
    if (DAT_0046c064 == -1) {
        BeginMissileLockWarning(v);
        return 1;
    }
    return 0;
}

/* Function start: 0x416010 */
void EndMissileLockWarning(void)
{
    if (DAT_0046c064 >= 0)
        DAT_0046c060 = 1;
    ShowHudMessageIfCurrent((int)PTR_s_MISSILE_LOCKED_004691d4[0]);
    DAT_0046c064 = -1;
}

/* Function start: 0x416220 */
void SetRectBounds(int p, unsigned short a, unsigned short b,
                   unsigned short c, unsigned short d)
{
    *(unsigned short *)(p + 8) = a;
    *(unsigned short *)(p + 10) = b;
    *(unsigned short *)(p + 12) = c;
    *(unsigned short *)(p + 14) = d;
}

/* Function start: 0x416250 */
short GetRectHeight(int p)
{
    return *(short *)(p + 0xc) - *(short *)(p + 8);
}

/* Function start: 0x416460 */
void ShowHudTextLine(char *s, unsigned char b)
{
    DAT_00469004 = s;
    DrawWrappedCommText(s, b);
}

/* Function start: 0x416480 */
void SetHudTextColour(short v)
{
    if (v != 0)
        EndCommMenu();
    DrawWrappedCommText(DAT_00469008, (unsigned char)DAT_004699d8);
}

/* Function start: 0x4168A0 */
void ReleaseCurrentTargetLock(void)
{
    if (DAT_00469208 != -1)
        DestroyShip(DAT_00469208);
}

/* Function start: 0x416C90 */
void DrawHudMessagesIfEnabled(void)
{
    if (HasFreeMessageSlot() && DAT_00469060 != 0)
        DrawHudMessageList();
}

/* Function start: 0x4173C0 */
void RefreshDamageDisplay(void)
{
    if (DAT_005a7684 != 0) {
        ComputeStickIndicatorFrame();
        if (DAT_0046900c != DAT_005a7dc8)
            DrawStickIndicator();
    }
}

/* Function start: 0x4173F0 */
void ForceRefreshDamageDisplay(void)
{
    if (DAT_005a7684 != 0) {
        DAT_0046900c = 0xff;
        CopyViewportContents(&DAT_005a6b60, &DAT_005a7550);
        RefreshDamageDisplay();
    }
}

/* Function start: 0x417610 */
void ClearHudTargetBox(void)
{
    DAT_005a7ef0 = 0;
    DAT_005a7ef4 = 0;
    DAT_005a7ef8 = 0;
    DAT_005a7efc = 0;
}

/* Function start: 0x417B10 */
void ShowDamageMessage(short a)
{
    CombatRoutine02(a, 1, DAT_004699b0, 0x17, 0);
}

/* Function start: 0x417F00 */
void PlayMissileLaunchSfx(void)
{
    PlaySfxWaveFileByNumber(0x19, -1, 0);
}
