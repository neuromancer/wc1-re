/*
 *  Mouse, keyboard and clock services over the Win32 API.
 *
 *  Address range 0x402e00-0x4030ff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: contiguous run of Win32 input/timing wrappers with no string references.
 */
#include "wc1.h"

/* Function start: 0x402E80 */
void SetMousePosition(int x, int y)
{
    SetCursorPos(x, y);
}

/* Function start: 0x403060 */
int GetShiftKeyState(void)
{
    return GetAsyncKeyState(VK_SHIFT);
}

/* Function start: 0x403070 */
int GetControlKeyState(void)
{
    return GetAsyncKeyState(VK_CONTROL);
}

/* Function start: 0x403080 */
unsigned int GetKeyboardModifiers(void)
{
    return DAT_005a8964;
}

/* Function start: 0x403090 */
unsigned int GetGameClockTicks(void)
{
    return (GetTickCount() - g_dwGameClockBase_005a89a8) * 60 / 1000;
}

/* Function start: 0x4030B0 */
void InitGameClockEpoch(void)
{
    g_dwGameClockBase_005a89a8 = GetTickCount() + (rand() & 3600000);
}

/* Function start: 0x4030D0 */
void WriteDebugString(const char *s)
{
    OutputDebugString(s);
}

/* Function start: 0x4030E0 */
/* Byte-identical duplicate of SetMousePosition (0x00402E80); both exist because
 * VC++ 4.2's linker did no COMDAT folding. */
void SetMousePositionDuplicate(int x, int y)
{
    SetCursorPos(x, y);
}
