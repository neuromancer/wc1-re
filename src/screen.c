/*
 *  Screen scopes, prompts and the comm menu.
 *
 *  Address range 0x42f000-0x431fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: PushMemoryStackFrame/ShowChoosePrompt/ShowEnemyTargetSelectMenu.
 */
#include "wc1.h"

/* Function start: 0x42F1F0 */
int IsShipDestroyed(short i)
{
    return DAT_0059c3f0[i] == 5;
}

/* Function start: 0x42F730 */
unsigned short GetConversationState(void)
{
    return 0;
}

/* Function start: 0x42F7E0 */
/* Initialises a 320x200 viewport record (0x13F == 319, 199) then validates it. */
void InitFullScreenViewport(int *vp, short arg)
{
    DAT_005a6538 = vp;
    *(unsigned short *)(vp + 2) = 0;
    *(unsigned short *)(vp + 3) = 0x13f;
    *(unsigned short *)((int)vp + 0xe) = 199;
    *(unsigned short *)((int)vp + 10) = 0;
    CheckMcgaVideoMode(vp, arg, 0);
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

/* Function start: 0x42FB20 */
/* Tail-jump thunk to 0x004362E0, not a constant return -- Ghidra followed the
 * jump and folded the callee's body into the display. */
__declspec(naked) void GetFixedOneMillionThunk(void) { __asm { jmp GetFixedOneMillion } }

/* Function start: 0x42FB30 */
__declspec(naked) void GetFixedOneMillionThunkAlt(void) { __asm { jmp GetFixedOneMillionAlt } }

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
    return DAT_0046af60 == 0;
}

/* Function start: 0x430D30 */
void ShowCommMenuEntry(short i)
{
    CombatRoutine03((int)PTR_s_Never_mind____0046af90[i], i);
}

/* Function start: 0x430D50 */
void ShowCommMenuPrompt(unsigned int a, int b)
{
    ShowHudMessageUnlessDuplicate(b, DAT_004699a8, 0xff);
    DAT_0059e490 = a;
}

/* Function start: 0x430D80 */
int IsEjectPromptActive(void)
{
    return (short)GetSeriesStateWord(1) == 4;
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
    PushSeriesStateWord(1, 4);
    SetPendingMenuAction(1);
}

/* Function start: 0x430E10 */
int HasNoLockedTarget(void)
{
    return DAT_0046c04c == -1;
}

/* Function start: 0x430E30 */
int IsWingmanIdle(void)
{
    return ShipAiRoutine18((short)DAT_0059ce60[0]) == 0;
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
    if ((short)GetSeriesStateWord(1) == 6)
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
