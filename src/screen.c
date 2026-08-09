/*
 *  Screen scopes, prompts and the comm menu.
 *
 *  Address range 0x42f000-0x431fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: PushMemoryStackFrame/ShowChoosePrompt/ShowEnemyTargetSelectMenu.
 */
#include "wc1.h"

/* Function start: 0x42F1F0 */
int IsShipQueuedOrderDefend(short i)
{
    return g_aeShipMissionType_0059c3f0[i] == MISSION_TYPE_ROUT;
}

/* Function start: 0x42F730 */
unsigned short GetConversationState(void)
{
    return 0;
}

/* Function start: 0x42F7E0 */
/* Initialises a 320x200 viewport record (0x13F == 319, 199) then validates it. */
void InitFullScreenViewport(int *record, short arg)
{
    Viewport *viewport = (Viewport *)record;

    DAT_005a6538 = record;
    viewport->left = 0;
    viewport->top = 0;
    viewport->right = 319;
    viewport->bottom = 199;
    AllocateViewport(viewport, arg, 0);
}

/* Function start: 0x42F930 */
void FrameStartHook(void)
{
}

/* Function start: 0x42F940 */
unsigned short IsSoundHardwarePresent(void)
{
    GetSoundHardwareFlag();
    return 1;
}

/* Function start: 0x42F950 */
void MessagePumpHook(void)
{
}

/* Function start: 0x42F960 */
int PushMemoryStackFrame(int memory, int offset)
{
    int index;

    if (offset != 0) {
        printf("push %p by %d\n", (void *)memory, offset);
        if (g_nPacketHandleCount_005a6530 == 0x1000)
            exit_squadron("qq mem push overflow");
        index = g_nPacketHandleCount_005a6530;
        g_aiPacketHandleOffsets_005a2530[index] = offset;
        if (offset < 0) {
            memory -= offset;
            g_aiPacketHandles_0059e530[index] = memory;
            g_nPacketHandleCount_005a6530 = index + 1;
            return memory;
        }
        memory += offset;
        g_aiPacketHandles_0059e530[index] = memory;
        g_nPacketHandleCount_005a6530 = index + 1;
        return memory;
    }
    return memory;
}

/* Function start: 0x42FA20 */
int MapPacketHandleToBlock(int handle)
{
    int count = g_nPacketHandleCount_005a6530;
    int bytes = count * 4;
    int i;
    int *entry;
    int offset;

    for (;;) {
        i = 0;
        g_nPacketHandleCount_005a6530 = count;
        if (bytes <= 0)
            return handle;
        entry = g_aiPacketHandles_0059e530;
        while (*entry != handle) {
            entry = entry + 1;
            i = i + 1;
            g_nPacketHandleCount_005a6530 = count;
            if (i >= count)
                return handle;
        }
        offset = g_aiPacketHandleOffsets_005a2530[i];
        if (offset < 0)
            handle = handle + offset;
        else
            handle = handle - offset;
        g_aiPacketHandles_0059e530[i] =
            *(int *)((unsigned char *)g_aiPacketHandles_0059e530 + bytes - 4);
        bytes = bytes - 4;
        count = count - 1;
        g_aiPacketHandleOffsets_005a2530[i] =
            *(int *)((unsigned char *)g_aiPacketHandleOffsets_005a2530 + bytes);
    }
}

/* Function start: 0x42FA90 */
void *AllocateTaggedMemory(unsigned int size, unsigned short flags)
{
    unsigned int *memory;
    unsigned short tagged;

    tagged = flags & 0x40;
    if (tagged != 0)
        size += 8;
    memory = (unsigned int *)AllocateGuardedMemory(size);
    if (tagged != 0) {
        memcpy(memory, g_abTaggedAllocationPrefix_0046ad88,
               sizeof(g_abTaggedAllocationPrefix_0046ad88));
        memory = (unsigned int *)PushMemoryStackFrame((int)memory, -8);
    }
    return memory;
}

/* Function start: 0x42FAE0 */
void ReleasePacketHandle(int handle)
{
    int group = 4;
    int *entry = g_aiPacketReferenceTable_00465c88;

    do {
        int i = 0x25;

        do {
            if (*entry == handle)
                *entry = 0;
            entry = entry + 1;
            i = i - 1;
        } while (i != 0);
        group = group - 1;
    } while (group != 0);
    FreeGuardedAllocation((void *)MapPacketHandleToBlock(handle));
}

/* Function start: 0x42FB20 */
/* Tail-jump thunk to 0x004362E0, not a constant return -- Ghidra followed the
 * jump and folded the callee's body into the display. */
__declspec(naked) unsigned int GetFixedOneMillionThunk(void) { __asm { jmp GetFixedOneMillion } }

/* Function start: 0x42FB30 */
__declspec(naked) unsigned int GetFixedOneMillionThunkAlt(void) { __asm { jmp GetFixedOneMillionAlt } }

/* Function start: 0x430BC0 */
short StepMenuSelection(short v, int flag)
{
    if (v == -1)
        return 0xd - (unsigned short)(flag == 0);
    if (flag == 1)
        v = v - 1;
    return v;
}

/* Function start: 0x430CA0 */
int IsCommMenuIdle(void)
{
    return g_nCommMenuChoiceCount_0046af60 == 0;
}

/* Function start: 0x430CB0 */
void AppendCommMenuChoice(char *text, short command)
{
    short index;

    if (g_nCommMenuReuseMode_0046af64 == 1) {
        index = g_nCommMenuChoiceCount_0046af60;
        if (g_apszCommMenuChoiceText_0059e4e0[index] != text ||
            g_abCommMenuChoiceCommand_0059e488[index] != command)
            g_nCommMenuReuseMode_0046af64 = 0;
    }
    index = g_nCommMenuChoiceCount_0046af60;
    g_apszCommMenuChoiceText_0059e4e0[index] = text;
    g_nCommMenuChoiceCount_0046af60 = index + 1;
    g_abCommMenuChoiceCommand_0059e488[index] = (signed char)command;
}

/* Function start: 0x430D30 */
void SendCommMenuChoice(short i)
{
    AppendCommMenuChoice(g_apszCommMenuText_0046af90[i], i);
}

/* Function start: 0x430D50 */
void OpenCommMenuForTarget(unsigned int a, int b)
{
    CockpitMessage((char *)b, DAT_004699a8, 0xff);
    DAT_0059e490 = a;
}

/* Function start: 0x430D80 */
int IsEjectPromptActive(void)
{
    return (short)get_mode(1) == 4;
}

/* Function start: 0x430DA0 */
short GetPendingMenuAction(void)
{
    return (short)DAT_0046af6c;
}

/* Function start: 0x430DB0 */
void SetPendingMenuAction(unsigned char v)
{
    DAT_0046af6c = v;
}

/* Function start: 0x430DC0 */
void RequestEjectPrompt(void)
{
    push_mode(1, 4);
    SetPendingMenuAction(1);
}

/* Function start: 0x430E10 */
int HasNoLockedTarget(void)
{
    return g_nYourWingman_0046c04c == -1;
}

/* Function start: 0x430E30 */
int IsWingmanIdle(void)
{
    return unactive((short)g_acShipTarget_0059ce60[0]) == 0;
}

/* Function start: 0x430E50 */
unsigned short IsCommMenuAvailable(void)
{
    if (IsWingmanIdle() == 0) {
        if (HasNoLockedTarget())
            return 0;
    }
    return 1;
}

/* Function start: 0x430E70 */
void RequestCommMenu(unsigned char v)
{
    DAT_0046afc4 = v;
    SetPendingMenuAction(2);
}

/* Function start: 0x4314C0 */
void EndCommMenu(void)
{
    ClearAutopilotFlag();
    if ((short)get_mode(1) == 6)
        EndCommSessionWithWingman();
    DAT_00469004 = 0;
}

/* Function start: 0x4314F0 */
void ShowCentredPrompt(char *text, unsigned short arg)
{
    DosStrcpy(DAT_0059e1c0, text);
    SetHudMessageText(DAT_0059e1c0, DAT_004699a8, arg);
}

/* Function start: 0x4318F0 */
void ShutdownVideoHook(void)
{
    ReleaseVideoResourcesHook();
}

/* Function start: 0x431F00 */
void ThrottleFrameAndDrawFps(HDC dc)
{
    DWORD now;

    if (DAT_00465070 != 0) {
        sprintf((char *)DAT_00476620, "%f", DAT_00486510);
        TextOutA(dc, 0, 0, (char *)DAT_00476620,
                 strlen((char *)DAT_00476620));
    }

    if (DAT_0059ab23->pixels == DAT_00476648) {
        while (timeGetTime() < (DWORD)DAT_0046b1bc) {
            Sleep(0);
            RefreshMouseCursorDisplay();
        }
    } else {
        while (timeGetTime() < (DWORD)DAT_0046b1bc)
            Sleep(0);
    }

    if (DAT_00465070 != 0) {
        if (DAT_0046b1c4 != 0) {
            now = timeGetTime();
            DAT_00486510 = 1000.0f / ((double)now - DAT_0046b1c4);
        }
        DAT_0046b1c4 = timeGetTime();
        sprintf((char *)DAT_00476620, "%f", DAT_00486510);
        TextOutA(dc, 0, 0, (char *)DAT_00476620,
                 strlen((char *)DAT_00476620));
    }

    DAT_0046b1bc = timeGetTime() + DAT_0046b1b8;
}
