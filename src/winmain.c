/*
 *  Win32 shell: window creation, message loop, exit paths.
 *
 *  Address range 0x401000-0x402dff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: CreateMainWindow/MainWindowProc/AbortToDesktop cluster; string band 0x465048-0x465354.
 */
#include "wc1.h"

/* Function start: 0x401020 */
void RestoreGamePalette(void)
{
    ReleaseDirectDrawPalette();
    DIBwholePaletteFromWords();
}

/* Function start: 0x401250 */
unsigned int GetSfxDistanceFromCamera(void)
{
    return abs(0x19 - (int)DAT_00465040);
}

/* Function start: 0x401270 */
void PickRandomTauntDelay(void)
{
    MinShort(0x14, (short)RandomBelowOrEqual(7) + 10);
}

/* Function start: 0x401390 */
/* Divides in place and returns the quotient: *p keeps only the multiple of n. */
short SumShortArray(short *p, short n)
{
    short v = *p;

    *p = v - v % n;
    return v / n;
}

/* Function start: 0x401680 */
void TransformObjectVector(int p, int *q)
{
    ScaleDeltaToRange((int *)(p + 4), q, *(short *)(p + 0x10) + 0x10cc);
}

/* Function start: 0x4016A0 */
void TransformObjectVectorAlt(int p, int *q)
{
    ScaleDeltaToRange((int *)(p + 4), q, *(short *)(p + 0x10));
}

/* Function start: 0x401870 */
void PlayEngineRumble(void)
{
    InterpolateClamped(0, 0x14, DAT_00465040, 0x10cc, 0xc1c);
}

/* Function start: 0x401930 */
void ClearShipMode4(short i)
{
    if (DAT_0059d100[i] == 4)
        DAT_0059d100[i] = 0;
}

/* Function start: 0x401CE0 */
void WarpMouseTo(short x, short y)
{
    DAT_0059af70 = x;
    DAT_0059af72 = y;
    DAT_0059ab10 = x;
    DAT_0059ab12 = y;
    SetMouseHomePosition(x, y);
}

/* Function start: 0x4020E0 */
void ShowNoticeMessageBox(const char *text)
{
    DIBpositionWindow();
    MessageBoxA(0, text, "NOTICE", 0x10);
    exit(0);
}

/* Function start: 0x402520 */
unsigned int GetWindowHandleWord(void)
{
    return DAT_004650ac;
}

/* Function start: 0x402AC0 */
short GetJoystickButtons(void)
{
    return ((short)DAT_005a898c << 2) | DAT_005a897c;
}

/* Function start: 0x402B80 */
unsigned int GetMouseButtonState(void)
{
    return DAT_005a8a40;
}
