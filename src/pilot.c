/*
 *  Pilot name entry, high scores and inter-scene transitions.
 *
 *  Address range 0x425000-0x426fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: EnterPilotNameAndCallsign/ShowTrainSimHighScores; string band 0x469D74-0x469F98.
 */
#include "wc1.h"

/* Function start: 0x425AF0 */
void SceneEnterHook(void)
{
}

/* Function start: 0x425B90 */
void GetMessagePumpInterval(void)
{
    void *p = (void *)DAT_004763f0;

    if (DAT_004763f0 != 0) {
        DebugOverlayThreadProc((int)DAT_004763f0);
        free(p);
    }
}

/* Function start: 0x425BB0 */
/* TODO: formatted debug-overlay output body is not yet recovered. */
void SystemDebugPrintf(const char *format, ...)
{
    (void)format;
}

/* Function start: 0x425BC0 */
void PumpMessagesDuringWait(void)
{
    PumpMessagesWhileDebugPaused(DAT_004763f0);
}

/* Function start: 0x425BD0 */
unsigned char TakeDebugStepFlag(void)
{
    unsigned char v = DAT_0046964c;

    DAT_0046964c = 0;
    return v;
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
    DAT_0046964c = 0;
    DAT_00469648 = 0;
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
    short k = (short)(char)DAT_005a7c30[i * 5];

    if (8 < k)
        return DAT_00469dbc[k];
    return &DAT_005988de[0] + k * 0x26;
}

/* Function start: 0x425E20 */
unsigned int GetHighScoreValue(short i)
{
    return *(unsigned int *)(&DAT_005a7c31[0] + i * 5);
}

/* Function start: 0x425E30 */
void SetHighScoreEntry(short i, unsigned char b, unsigned int v)
{
    DAT_005a7c30[i * 5] = b;
    *(unsigned int *)(&DAT_005a7c30[1] + i * 5) = v;
}

/* Function start: 0x425ED0 */
void ClearHighScoreTable(short v)
{
    short i = 5;

    do {
        if ((char)DAT_005a7c30[i * 5] == v)
            return;
        i = i - 1;
    } while (i != 0);
}

/* Function start: 0x425FE0 */
int IsHighScoreSlotUsed(short i)
{
    return DAT_005a7c30[i * 5] != 0xff;
}

/* Function start: 0x426C50 */
void LoadSceneBackdrop(char n)
{
    DAT_005a86b0 = n + 0x16;
    FetchDiskPacketRetrying((short)DAT_005a86b0, 1, 0);
}
