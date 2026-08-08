/*
 *  Win32 input, cursor and timing shims.
 *
 *  MODULE BOUNDARY UNVERIFIED.  Contiguous run 0x00402E80-0x004030E0; the CD /
 *  disc-location code continues at 0x004031C0 and may belong to the same unit.
 */
#include "wc1.h"
#include "globals.h"

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

/* Function start: 0x403090 */
unsigned int GetGameClockTicks(void)
{
    return (GetTickCount() - g_dwGameClockBase_005a89a8) * 60 / 1000;
}

/* Function start: 0x4030D0 */
void WriteDebugString(const char *s)
{
    OutputDebugString(s);
}
