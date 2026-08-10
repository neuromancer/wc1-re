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

/* Function start: 0x430710 */
short __stdcall UpdateInputDeviceTransitions(short raw)
{
    InputDeviceSample *sample;
    unsigned int now;
    unsigned short buttons;
    unsigned short changed;
    unsigned short previous;
    short device;
    int threshold;

    device = g_nActiveInputDevice_005a819c;
    if (raw == 0)
        ReadCalibratedJoystick();
    else
        SampleJoystickDevice(&g_aInputDeviceSamples_005a81f0[device],
                             device, 0);

    sample = &g_aInputDeviceSamples_005a81f0[device];
    g_asInputButton2DoubleClick_0059e520[device] = 0;
    g_asInputButton1DoubleClick_0059e508[device] = 0;
    g_asInputButton2Changed_0059e510[device] = 0;
    g_asInputButton1Changed_0059e50c[device] = 0;
    buttons = (unsigned short)sample->buttons;
    previous = (unsigned short)g_asPreviousInputButtons_0059e514[device];
    if (buttons != previous) {
        changed = buttons ^ previous;
        now = DAT_0059ab54;
        threshold = g_nInputDoubleClickInterval_0046af54 *
                    g_nInputTickScale_0059af90;
        if ((changed & 1) != 0) {
            g_asInputButton1Changed_0059e50c[device] = 1;
            if ((buttons & 1) != 0) {
                if ((int)(now -
                    g_anInputButton1PressTime_0059e518[device]) <=
                        threshold)
                    g_asInputButton1DoubleClick_0059e508[device] = 1;
                g_anInputButton1PressTime_0059e518[device] = now;
            }
        }
        if ((changed & 2) != 0) {
            g_asInputButton2Changed_0059e510[device] = 1;
            if ((buttons & 2) != 0) {
                if ((int)(now -
                    g_anInputButton2PressTime_0059e500[device]) <=
                        threshold)
                    g_asInputButton2DoubleClick_0059e520[device] = 1;
                g_anInputButton2PressTime_0059e500[device] = now;
            }
        }
        g_asPreviousInputButtons_0059e514[device] = (short)buttons;
    }
    return 1;
}

/* Function start: 0x430840 */
void PollJoystickButtonEvents(void)
{
    InputDeviceSample *sample;
    short device;
    short doubleClick;

    device = g_nActiveInputDevice_005a819c;
    if (device == -1 || g_bInputPollingGuard_0046a01c != 0)
        return;
    g_bInputPollingGuard_0046a01c++;
    UpdateInputDeviceTransitions(1);
    sample = &g_aInputDeviceSamples_005a81f0[device];
    if (g_asInputButton1Changed_0059e50c[device] != 0) {
        doubleClick = g_asInputButton1DoubleClick_0059e508[device] != 0 ?
                      3 : 0;
        QueueInputEventAtCursor(((unsigned short)sample->buttons & 1) + 1,
                                0, doubleClick);
    }
    if (g_asInputButton2Changed_0059e510[device] != 0) {
        doubleClick = g_asInputButton2DoubleClick_0059e520[device] != 0 ?
                      3 : 0;
        QueueInputEventAtCursor(
            (((unsigned short)sample->buttons >> 1) & 1) + 1,
            1, doubleClick);
    }
    g_bInputPollingGuard_0046a01c--;
    g_bPreviousPrimaryInputButton_0059af74 =
        (unsigned char)sample->buttons;
    g_bPreviousSecondaryInputButton_0059af75 =
        (unsigned char)sample->buttons;
}

/* Function start: 0x430920 */
void PollMenuInputDevices(void)
{
    InputDeviceSample *sample;
    short device;
    short doubleClick;
    int buttonChanges;

    buttonChanges = 0;
    device = g_nActiveInputDevice_005a819c;
    if (device == -1)
        return;
    g_nHostMouseX_0059af70 = g_nMouseX_0059ab10;
    g_nHostMouseY_0059af72 = g_nMouseY_0059ab12;
    if (g_bInputPollingGuard_0046a01c != 0)
        return;
    g_bInputPollingGuard_0046a01c++;
    UpdateInputDeviceTransitions(0);
    sample = &g_aInputDeviceSamples_005a81f0[device];

    if (g_asInputButton1Changed_0059e50c[device] != 0) {
        g_nHostMouseX_0059af70 +=
            (short)(sample->x * g_nMenuPointerSpeed_0046af58);
        g_nHostMouseY_0059af72 +=
            (short)(sample->y * g_nMenuPointerSpeed_0046af58);
        doubleClick = g_asInputButton1DoubleClick_0059e508[device] != 0 ?
                      3 : 0;
        QueueInputEventAtCursor(((unsigned short)sample->buttons & 1) + 1,
                                0, doubleClick);
        buttonChanges = 1;
    }
    if (g_asInputButton2Changed_0059e510[device] != 0) {
        g_nHostMouseX_0059af70 +=
            (short)(sample->x * g_nMenuPointerSpeed_0046af58);
        g_nHostMouseY_0059af72 +=
            (short)(sample->y * g_nMenuPointerSpeed_0046af58);
        doubleClick = g_asInputButton1DoubleClick_0059e508[device] != 0 ?
                      3 : 0;
        QueueInputEventAtCursor(
            (((unsigned short)sample->buttons >> 1) & 1) + 1,
            1, doubleClick);
        buttonChanges++;
    }
    if (buttonChanges == 0) {
        g_nHostMouseX_0059af70 +=
            (short)(sample->x * g_nMenuPointerSpeed_0046af58);
        g_nHostMouseY_0059af72 +=
            (short)(sample->y * g_nMenuPointerSpeed_0046af58);
        if ((signed char)sample->x != 0 ||
            (signed char)sample->y != 0) {
            FlushInputEvents();
            if (g_nHostMouseX_0059af70 < 0)
                g_nHostMouseX_0059af70 = 0;
            else if (g_nHostMouseX_0059af70 > 319)
                g_nHostMouseX_0059af70 = 319;
            if (g_nHostMouseY_0059af72 < 0)
                g_nHostMouseY_0059af72 = 0;
            else if (g_nHostMouseY_0059af72 > 199)
                g_nHostMouseY_0059af72 = 199;
            SetMousePosition(g_nHostMouseX_0059af70,
                             g_nHostMouseY_0059af72);
        }
    }

    g_bPreviousPrimaryInputButton_0059af74 =
        (unsigned char)sample->buttons;
    g_bPreviousSecondaryInputButton_0059af75 =
        (unsigned char)sample->buttons;
    if (g_nHostMouseX_0059af70 < 0)
        g_nHostMouseX_0059af70 = 0;
    else if (g_nHostMouseX_0059af70 > 319)
        g_nHostMouseX_0059af70 = 319;
    if (g_nHostMouseY_0059af72 < 0)
        g_nHostMouseY_0059af72 = 0;
    else if (g_nHostMouseY_0059af72 > 199)
        g_nHostMouseY_0059af72 = 199;
    if (buttonChanges != 0) {
        LeaveAllocationScope();
        g_bPrimaryMouseButton_0059ab14 =
            g_bPreviousPrimaryInputButton_0059af74;
        g_nMouseX_0059ab10 = g_nHostMouseX_0059af70;
        g_nMouseY_0059ab12 = g_nHostMouseY_0059af72;
        g_bSecondaryMouseButton_0059ab15 =
            g_bPreviousSecondaryInputButton_0059af75;
        g_wInputFlags_0059ab17 = g_wPreviousMenuInputFlags_0059af77;
        EnterAllocationScope();
    }
    g_bInputPollingGuard_0046a01c--;
}

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

/* Function start: 0x431410 */
void FreeCommDisplayResources(void)
{
    FreePacketAndClear((int *)&g_apCommPortraitShapes_0059e180[
        g_nCommPortraitIndex_0046afd0], 0);
    FreePacketAndClear((int *)&g_pConfedCommBackground_00469278, 0);
    FreePacketAndClear((int *)&g_pKilrathiCommBackground_00469280, 0);
    FreePacketAndClear((int *)&g_pCommStaticShape_0046927c, 0);
    g_nCommSpeakerRating_0046afcc = -1;
    g_nCommSpeakerObject_0046afc8 = -1;
    g_nCommPortraitIndex_0046afd0 = -1;
}

/* Function start: 0x431470 */
void EndCommSessionWithWingman(void)
{
    if (g_apCommPortraitShapes_0059e180[g_nCommPortraitIndex_0046afd0] != 0)
        malf_noise(1, 1, 12, 23, 1);
    FreeCommDisplayResources();
    if ((short)get_mode(1) == 6)
        pop_mode(1);
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

/* Function start: 0x431D20 */
short ReadCalibratedJoystick(void)
{
    InputDeviceSample *sample;
    int rawX;
    int rawY;
    int normalizedX;
    int normalizedY;
    short device;

    device = g_nActiveInputDevice_005a819c;
    if (device == -1)
        return 0;
    sample = &g_aInputDeviceSamples_005a81f0[device];
    if (SampleJoystickDevice(sample, device,
                             g_nJoystickFailureValue_005a81e0) != 0 ||
        sample->x == g_nJoystickFailureValue_005a81e0 ||
        sample->y == g_nJoystickFailureValue_005a81e0) {
        g_nActiveInputDevice_005a819c = -1;
        sample->x = 0;
        sample->y = 0;
        sample->buttons = 0;
        return 0;
    }

    rawX = sample->x;
    rawY = sample->y;
    g_nJoystickRawX_005a81c0 = rawX;
    g_nJoystickRawY_005a81c4 = rawY;
    if (sample->x < g_nJoystickMinimumX_005a81b8)
        sample->x = g_nJoystickMinimumX_005a81b8;
    if (sample->x > g_nJoystickMaximumX_005a81b0)
        sample->x = g_nJoystickMaximumX_005a81b0;
    if (sample->y < g_nJoystickMinimumY_005a81bc)
        sample->y = g_nJoystickMinimumY_005a81bc;
    if (sample->y > g_nJoystickMaximumY_005a81b4)
        sample->y = g_nJoystickMaximumY_005a81b4;

    normalizedX = 0;
    if (sample->x < g_nJoystickCentreX_005a81dc &&
        g_nJoystickLeftScale_005a81ac != 0) {
        normalizedX = (g_nJoystickCentreX_005a81dc - sample->x) /
                      g_nJoystickLeftScale_005a81ac;
        if (normalizedX > g_nJoystickHorizontalDeadZone_005a81a4)
            normalizedX = -normalizedX;
        else
            normalizedX = 0;
    } else if (sample->x > g_nJoystickCentreX_005a81dc &&
               g_nJoystickRightScale_005a81d0 != 0) {
        normalizedX = (sample->x - g_nJoystickCentreX_005a81dc) /
                      g_nJoystickRightScale_005a81d0;
        if (normalizedX <= g_nJoystickHorizontalDeadZone_005a81a4)
            normalizedX = 0;
    }

    normalizedY = 0;
    if (sample->y < g_nJoystickCentreY_005a81d8 &&
        g_nJoystickUpScale_005a81a8 != 0) {
        normalizedY = (g_nJoystickCentreY_005a81d8 - sample->y) /
                      g_nJoystickUpScale_005a81a8;
        if (normalizedY > g_nJoystickVerticalDeadZone_005a81a0)
            normalizedY = -normalizedY;
        else
            normalizedY = 0;
    } else if (sample->y > g_nJoystickCentreY_005a81d8 &&
               g_nJoystickDownScale_005a81d4 != 0) {
        normalizedY = (sample->y - g_nJoystickCentreY_005a81d8) /
                      g_nJoystickDownScale_005a81d4;
        if (normalizedY <= g_nJoystickVerticalDeadZone_005a81a0)
            normalizedY = 0;
    }
    sample->x = (short)normalizedX;
    sample->y = (short)normalizedY;
    return 1;
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
