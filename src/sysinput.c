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

/* Function start: 0x402EA0 */
unsigned int PollKeyboardState(void)
{
    nClearedKeyboardState = 0;
    nHomeKeyState = GetAsyncKeyState(VK_HOME);
    nUpArrowKeyState = GetAsyncKeyState(VK_UP);
    nPageUpKeyState = GetAsyncKeyState(VK_PRIOR);
    nLeftArrowKeyState = GetAsyncKeyState(VK_LEFT);
    nRightArrowKeyState = GetAsyncKeyState(VK_RIGHT);
    nEndKeyState = GetAsyncKeyState(VK_END);
    nDownArrowKeyState = GetAsyncKeyState(VK_DOWN);
    nPageDownKeyState = GetAsyncKeyState(VK_NEXT);
    nClearKeyState = GetAsyncKeyState(VK_CLEAR);
    nOemPeriodKeyState = GetAsyncKeyState(0xbe);
    nOemCommaKeyState = GetAsyncKeyState(0xbc);
    nInsertKeyState = GetAsyncKeyState(VK_INSERT);
    nDeleteKeyState = GetAsyncKeyState(VK_DELETE);

    if (nHomeKeyState != 0)
        return 0x47;
    if (nPageUpKeyState != 0)
        return 0x49;
    if (nEndKeyState != 0)
        return 0x4f;
    if (nPageDownKeyState != 0)
        return 0x51;
    if (nInsertKeyState != 0 ||
        nOemCommaKeyState != 0)
        return 0x52;
    if (nDeleteKeyState != 0 ||
        nOemPeriodKeyState != 0)
        return 0x53;
    if (nClearKeyState != 0)
        return 0x4c;
    if (nUpArrowKeyState != 0) {
        if (nLeftArrowKeyState != 0)
            return 0x47;
        return nRightArrowKeyState != 0 ? 0x49 : 0x48;
    }
    if (nDownArrowKeyState != 0) {
        if (nLeftArrowKeyState != 0)
            return 0x4f;
        return nRightArrowKeyState != 0 ? 0x51 : 0x50;
    }
    if (nLeftArrowKeyState != 0)
        return 0x4b;
    return nRightArrowKeyState != 0 ? 0x4d : 0;
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
    return nSystemKeyDown;
}

/* Function start: 0x403090 */
unsigned int GetGameClockTicks(void)
{
    return (GetTickCount() - dwGameClockBase) * 60 / 1000;
}

/* Function start: 0x4030B0 */
void InitGameClockEpoch(void)
{
    dwGameClockBase = GetTickCount() + (rand() & 3600000);
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
