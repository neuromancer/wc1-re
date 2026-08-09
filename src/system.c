/*
 *  Process-level services: memory reporting and exit.
 *
 *  Address range 0x427000-0x4274df (provisional -- see docs/ORDER.md).
 *  Boundary evidence: exit_squadron/ShowMemoryStatusDebug; string band 0x46A064-0x46A10C.
 */
#include "wc1.h"

/* Function start: 0x4272F0 */
short LogMemoryUsage(void)
{
    ((void (__cdecl *)(int, int))ShutdownHook)(0x21, DAT_0059ab4c);
    LeaveWaitCursorScope();
    GetMessagePumpResult();
    _chdir("..");
    ((void (__cdecl *)(int))ShutdownVideoHook)(3);
    if (g_nOriginDevUnlock_00469ff4 != 0) {
        SystemDebugPrintf(
            "Original FMem %lu.  Current FMem %lu.  Current NMem %u.\n",
            g_dwOriginalFreeMemory_005a7cd8,
            ((unsigned int (__cdecl *)(int))
                GetFixedOneMillionThunkAlt)(0),
            (unsigned int)(int)(short)GetOriginalFreeMemory());
    }
    MouseIdleHook();
    ((void (__cdecl *)(int))MessagePumpHook)(8);
    return 0;
}

/* Function start: 0x427370 */
void exit_squadron(const char *msg)
{
    LogMemoryUsage();
    SystemDebugPrintf(msg);
    SystemDebugPrintf("[SYSTEM]: Exit_squadron\n");
    WriteDebugString("[SYSTEM] exit_squadron");
    WriteDebugString(msg);
    ClearDebugPauseFlags();
    AbortToDesktop();
    exit(0);
}

/* Function start: 0x4273C0 */
unsigned int ShowMemoryStatusDebug(void)
{
    TextContext savedContext;
    TextContext *previousContext;
    char value[60];

    previousContext = g_pCurrentTextContext_0059af8c;
    savedContext = g_stDefaultTextContext_005a7740;
    if (g_nShowMemoryStatus_0046a00c != 0) {
        InitializeTextContextFromFont(
            &g_stDefaultTextContext_005a7740, 1,
            (unsigned char)g_cViewportClearColour_004699a0,
            DAT_0046999c);
        SetTextContext(&g_stDefaultTextContext_005a7740);
        DrawFormattedText("%X%YCurrent NMem %d.",
                          0, 176, (int)(short)GetOriginalFreeMemory());
        sprintf(value, "%ld",
                ((unsigned int (__cdecl *)(int))
                    GetFixedOneMillionThunkAlt)(0));
        DrawFormattedText("%X%YCurrent FMem %s.", 0, 184, value);
        sprintf(value, "%ld", g_dwOriginalFreeMemory_005a7cd8);
        DrawFormattedText("%X%YOriginal FMem %s.", 0, 0, value);
        g_stDefaultTextContext_005a7740 = savedContext;
    }
    g_pCurrentTextContext_0059af8c = previousContext;
    return 0;
}

/* Function start: 0x4274C0 */
unsigned int GetJoystickButtonEdge(unsigned int a, short b)
{
    (void)a;
    if (b < 0)
        return JoystickEdgeHook(-1);
    return 0;
}
