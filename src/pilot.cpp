/*
 *  Pilot name entry, high scores, inter-scene transitions and ownership of
 *  the Win32 developer overlay console.
 *
 *  Address range 0x425000-0x426fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: EnterPilotNameAndCallsign/ShowTrainSimHighScores; string band 0x469D74-0x469F98.
 */
#include "wc1.h"

/* Function start: 0x425AF0 */
void SceneEnterHook(void)
{
}

/* Function start: 0x425B00 */
void CreateDebugOverlayConsole(HINSTANCE module, HWND window,
                               short columns, short rows)
{
    g_pDebugOverlay_004763f0 =
        new DebugOverlayConsole(module, window, columns, rows, 0);
}

/* Function start: 0x425B90 */
void DestroyGlobalDebugOverlayConsole(void)
{
    delete g_pDebugOverlay_004763f0;
}

/* Function start: 0x425BB0 */
void SystemDebugPrintf(const char *, ...)
{
}

/* Function start: 0x425BC0 */
char PumpMessagesDuringWait(void)
{
    return g_pDebugOverlay_004763f0->WaitForKey();
}

/* Function start: 0x425BD0 */
unsigned char TakeDebugStepFlag(void)
{
    unsigned char value = (unsigned char)g_dwDebugOverlayKeyLatch_0046964c;

    g_dwDebugOverlayKeyLatch_0046964c = 0;
    return value;
}

/* Function start: 0x425BE0 */
void ResetDiskPromptTimer(void)
{
}

/* Function start: 0x425C10 */
void DiskPromptDrawHook(void)
{
}

/* Function start: 0x425C20 */
void ClearDebugPauseFlags(void)
{
    g_dwDebugOverlayKeyLatch_0046964c = 0;
    g_dwDebugOverlayKey_00469648 = 0;
}

/* Function start: 0x425C30 */
unsigned int GetDebugKeyState(unsigned int *p)
{
    char c = *(char *)((int)p + 0xd);

    if (*(char *)((int)p + 0xd) == -1)
        c = DAT_0046999c;
    ClearViewport((Viewport *)*p, (unsigned char)c);
    return 0;
}

/* Function start: 0x425DF0 */
unsigned char *GetHighScoreEntry(short i)
{
    short k = (short)g_aHighScoreEntries_005a7c30[i].pilotIndex;

    if (8 < k)
        return g_apszBuiltInHighScoreNames_00469de0[k - 9];
    return &DAT_005988de[0] + k * 0x26;
}

/* Function start: 0x425E20 */
unsigned int GetHighScoreValue(short i)
{
    return g_aHighScoreEntries_005a7c30[i].score;
}

/* Function start: 0x425E30 */
void SetHighScoreEntry(short i, unsigned char b, unsigned int v)
{
    g_aHighScoreEntries_005a7c30[i].pilotIndex = (signed char)b;
    g_aHighScoreEntries_005a7c30[i].score = v;
}

/* Function start: 0x425ED0 */
void ClearHighScoreTable(short v)
{
    short i = 5;

    do {
        if (g_aHighScoreEntries_005a7c30[i].pilotIndex == v)
            return;
        i = i - 1;
    } while (i != 0);
}

/* Function start: 0x425FE0 */
int IsHighScoreSlotUsed(short i)
{
    return g_aHighScoreEntries_005a7c30[i].pilotIndex != -1;
}

/* Function start: 0x426C50 */
void LoadSceneBackdrop(char n)
{
    DAT_005a86b0 = n + 0x16;
    FetchDiskPacketRetrying((short)DAT_005a86b0, 1, 0);
}
