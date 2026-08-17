/*
 *  Mouse, keyboard and clock services over the Win32 API.
 *
 *  Address range 0x402e00-0x4030ff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: contiguous run of Win32 input/timing wrappers with no string references.
 */
#include "wc1.h"

/* Function start: 0x455A85 */
void SetMousePosition(int x, int y)
{
    SetCursorPos(x, y);
}

/* Function start: 0x455AC8 */
unsigned int PollKeyboardState(void)
{
    g_nClearedKeyboardState_00475b6c = 0;
    g_nHomeKeyState_00475bf4 = GetAsyncKeyState(VK_HOME);
    g_nUpArrowKeyState_00475c10 = GetAsyncKeyState(VK_UP);
    g_nPageUpKeyState_00475d18 = GetAsyncKeyState(VK_PRIOR);
    g_nLeftArrowKeyState_00475d1c = GetAsyncKeyState(VK_LEFT);
    g_nRightArrowKeyState_00475c00 = GetAsyncKeyState(VK_RIGHT);
    g_nEndKeyState_00475c04 = GetAsyncKeyState(VK_END);
    g_nDownArrowKeyState_00475c14 = GetAsyncKeyState(VK_DOWN);
    g_nPageDownKeyState_00475bf8 = GetAsyncKeyState(VK_NEXT);
    g_nClearKeyState_00475c08 = GetAsyncKeyState(VK_CLEAR);
    g_nOemPeriodKeyState_00475bf0 = GetAsyncKeyState(0xbe);
    g_nOemCommaKeyState_00475bfc = GetAsyncKeyState(0xbc);
    g_nInsertKeyState_00475b68 = GetAsyncKeyState(VK_INSERT);
    g_nDeleteKeyState_00475c0c = GetAsyncKeyState(VK_DELETE);

    if (g_nHomeKeyState_00475bf4 != 0)
        return 0x47;
    if (g_nPageUpKeyState_00475d18 != 0)
        return 0x49;
    if (g_nEndKeyState_00475c04 != 0)
        return 0x4f;
    if (g_nPageDownKeyState_00475bf8 != 0)
        return 0x51;
    if (g_nInsertKeyState_00475b68 != 0 ||
        g_nOemCommaKeyState_00475bfc != 0)
        return 0x52;
    if (g_nDeleteKeyState_00475c0c != 0 ||
        g_nOemPeriodKeyState_00475bf0 != 0)
        return 0x53;
    if (g_nClearKeyState_00475c08 != 0)
        return 0x4c;
    if (g_nUpArrowKeyState_00475c10 != 0) {
        if (g_nLeftArrowKeyState_00475d1c != 0)
            return 0x47;
        return g_nRightArrowKeyState_00475c00 != 0 ? 0x49 : 0x48;
    }
    if (g_nDownArrowKeyState_00475c14 != 0) {
        if (g_nLeftArrowKeyState_00475d1c != 0)
            return 0x4f;
        return g_nRightArrowKeyState_00475c00 != 0 ? 0x51 : 0x50;
    }
    if (g_nLeftArrowKeyState_00475d1c != 0)
        return 0x4b;
    return g_nRightArrowKeyState_00475c00 != 0 ? 0x4d : 0;
}

/* Function start: 0x455E8F */
int GetShiftKeyState(void)
{
    return GetAsyncKeyState(VK_SHIFT);
}

/* Function start: 0x455EBF */
int GetControlKeyState(void)
{
    return GetAsyncKeyState(VK_CONTROL);
}

/* Function start: 0x455EAA */
unsigned int GetKeyboardModifiers(void)
{
    return DAT_005a8964;
}

/* Function start: 0x455EF5 */
unsigned int GetGameClockTicks(void)
{
    return (GetTickCount() - g_dwGameClockBase_005d10e8) * 60 / 1000;
}

/* Function start: 0x455F23 */
void InitGameClockRandomEpoch(void)
{
    g_dwGameClockStart_005d12b8 = GetTickCount();
    g_dwGameClockBase_005d10e8 = GetTickCount() + (rand() & 3600000);
}

/* Function start: 0x45641B */
int TakeInputPressCount(void)
{
    int pressCount;

    pressCount = g_nInputPressCount_0049c258;
    g_nInputPressCount_0049c258 = 0;
    return pressCount;
}

/* Function start: 0x455F59 */
void WriteDebugString(const char *s)
{
#if 0
    OutputDebugString(s);
#else
    if (s != 0) {
        OutputDebugString(s);
        if (g_nDebugFileLoggingEnabled_0049c2d8 != 0) {
            if (g_pDebugLogFile_0049c2dc == 0)
                g_pDebugLogFile_0049c2dc = fopen("log.log", "wt");
            fprintf(g_pDebugLogFile_0049c2dc, "%s", s);
            fflush(g_pDebugLogFile_0049c2dc);
        }
    }
    return;
#endif
}

/* Function start: WC2_UNMAPPED */
/* Byte-identical duplicate of SetMousePosition (0x00402E80); both exist because
 * VC++ 4.2's linker did no COMDAT folding. */
void SetMousePositionDuplicate(int x, int y)
{
    SetCursorPos(x, y);
}
