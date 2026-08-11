/*
 *  Screen scopes, prompts and the comm menu.
 *
 *  Address range 0x42efc0-0x431fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: PushMemoryStackFrame/ShowChoosePrompt/ShowEnemyTargetSelectMenu.
 */
#include "wc1.h"

/* Function start: 0x42EFC0 */
void cleanup_objectives(void)
{
    MissionObjective *entry;
    MissionShipRecord *missionShip;
    short objective;
    short home;
    short proximity;
    short index;
    short object;

    objective = 0;
    home = find_ship_index(g_nHomeMissionShipIndex_005a8692);
    proximity = g_aMissionNavPoints_0046c2f0[
        g_nMissionEntryNavPoint_005a8690].proximityRadius;
    entry = &g_aMissionObjectives_0059dac0[0];
    while (entry->type != -1) {
        if (achieved(objective) == 0) {
            index = (short)entry->index;
            switch (entry->type) {
            case 0:
            case 1:
            case 3:
                if (visited(objective) != 0)
                    flag_objective(objective, 2);
                break;
            case 2:
                missionShip = &g_aMissionShips_0046c948[index];
                if (missionShip->type == OBJECT_TYPE_DRAYMAN) {
                    if (missionShip->state == 2) {
                        flag_objective(objective, 2);
                        if (missionShip->type == OBJECT_TYPE_HORNET ||
                            missionShip->type == OBJECT_TYPE_DRAYMAN)
                            affect_mission_score(0, 5, -1);
                        else
                            affect_mission_score(0, 9, -1);
                    }
                } else if (missionShip->type == OBJECT_TYPE_EXETER ||
                           missionShip->type == OBJECT_TYPE_DRALTHI) {
                    object = find_ship_index(index);
                    if (object != -1 && home != -1 &&
                        distance_from_object(object, home) < proximity)
                        missionShip->state = 1;
                    if (missionShip->state == 1 && sighted(objective) != 0) {
                        flag_objective(objective, 2);
                        if (missionShip->type == OBJECT_TYPE_HORNET ||
                            missionShip->type == OBJECT_TYPE_DRAYMAN)
                            affect_mission_score(0, 5, -1);
                        else
                            affect_mission_score(0, 9, -1);
                    }
                } else if (missionShip->state == 0 &&
                           sighted(objective) != 0) {
                    flag_objective(objective, 2);
                    if (missionShip->type == OBJECT_TYPE_HORNET ||
                        missionShip->type == OBJECT_TYPE_DRAYMAN)
                        affect_mission_score(0, 5, -1);
                    else
                        affect_mission_score(0, 9, -1);
                }
                break;
            case 4:
                if (g_aMissionShips_0046c948[index].state == 3)
                    flag_objective(objective, 2);
                break;
            }
        }
        objective++;
        entry++;
    }
}

/* Function start: 0x42F1F0 */
int too_busy(short ship)
{
    return g_aeShipMissionType_0059c3f0[ship] == MISSION_TYPE_ROUT;
}

/* Function start: 0x42F210 */
void reply(short ship, short accepted)
{
    if (accepted == 1) {
        send_message(ship, 0);
        return;
    }
    send_message(ship, 1);
}

/* Function start: 0x42F240 */
int disobey_formation(short ship)
{
    switch (g_aiPilotLevel_0059cf30[ship]) {
    case 10:
        return any_enemy_tail(0);
    case 11:
        return report_kilrathi_rout(0);
    }
    return 0;
}

/* Function start: 0x42F270 */
int bad_target(short ship, short target)
{
    if (target != ship &&
        g_aeShipSide_0059d650[target] != g_aeShipSide_0059d650[ship] &&
        too_busy(ship) == 0)
        return 0;
    return 1;
}

/* Function start: 0x42F2B0 */
short can_land(void)
{
    MissionObjective *objective;
    short result;
    short index;

    result = 0;
    if (any_enemy(0, 20000) == 0) {
        if (evaluate_damage(0) < 50 || g_nPlayerKillCount_005a7c9c > 0 ||
            g_anShipFuel_0059b470[0] < 1000)
            result = 1;
        index = 0;
        if (g_cMissionObjectiveCount_0059c46a > 0) {
            do {
                objective = &g_aMissionObjectives_0059dac0[index];
                if (objective->type != 1) {
                    if (achieved(index) == 0) {
                        if (visited(index) == 0 || objective->type == 2)
                            goto next_objective;
                    }
                    result = 1;
                }
next_objective:
                index++;
            } while (index < g_cMissionObjectiveCount_0059c46a);
        }
    }
    return result;
}

/* Function start: 0x42F350 */
short i_wanna_rout(short ship, int pilot)
{
    if (pilot <= 4)
        return 1;
    switch (pilot) {
    case 6:
        return any_enemy(ship, 5000) == 0;
    case 7:
    case 11:
        return 0;
    case 8:
        return g_aeShipMissionType_0059c3f0[ship] ==
               MISSION_TYPE_CANNED_SEQUENCE;
    case 9:
        return (short)triumph(0);
    case 10:
        return any_enemy(0, 10000) == 0;
    }
    return 1;
}

/* Function start: 0x42F3F0 */
void request(short requester, short ship, short command)
{
    signed char *requesterTarget;
    short target;
    short object;

    requesterTarget = &g_acShipTarget_0059ce60[requester];

retry_request:
    target = (short)*requesterTarget;
    switch (command) {
    case 1:
        allow_engage();
        if (bad_target(ship, target) == 0) {
            engage(ship, target, OBJECTIVE_ENGAGE_ENEMY);
            reply(ship, 1);
            return;
        }
        reply(ship, 0);
        return;
    case 2:
        target = -1;
        object = 0;
        do {
            if (g_aeObjectClass_0059d100[object] >= OBJECT_CLASS_SHIP &&
                g_aeSpecialManeuver_0059c3c0[object] !=
                    SPECIAL_MANEUVER_UNKNOWN_9 &&
                g_aeShipSide_0059d650[ship] !=
                    g_aeShipSide_0059d650[object] &&
                g_acShipTarget_0059ce60[object] == requester) {
                target = object;
                break;
            }
            object++;
        } while (object < 10);
        if (target == -1) {
            command = 9;
            goto retry_request;
        }
        allow_engage();
        engage(ship, target, OBJECTIVE_ENGAGE_ENEMY);
        reply(ship, 1);
        return;
    case 3:
        if (i_wanna_rout(ship, g_aiPilotLevel_0059cf30[ship]) != 0 &&
            try2rout(ship) != 0) {
            g_bEngageAllowed_0046c080 = 0;
            reply(ship, 1);
            return;
        }
        reply(ship, 0);
        return;
    case 4:
    case 5:
    case 6:
        if (RandomBelow(100) < 70 ||
            ((signed char)g_acShipRating_0059cd80[ship] > 8 &&
             (signed char)g_acShipRating_0059cd80[ship] < 13))
            send_message(ship, (signed char)(command - 2));
        if (g_acShipTarget_0059ce60[ship] != requester &&
            too_busy(ship) == 0) {
            engage(ship, requester, OBJECTIVE_ENGAGE_ENEMY);
            return;
        }
        break;
    case 7:
        allow_engage();
        if (g_aeShipObjective_0059d200[ship] ==
                OBJECTIVE_HOLD_FORMATION) {
            reset_objective(ship, OBJECTIVE_BREAK_FORMATION);
            reply(ship, 1);
            return;
        }
        reply(ship, 0);
        return;
    case 8:
        disallow_engage();
        if (disobey_formation(ship) != 0) {
            alter_objective(ship, OBJECTIVE_BREAK_FORMATION);
            reply(ship, 0);
            return;
        }
        g_nAutoEngageTimer_0046c084 = -150;
        reply(ship, 1);
        return;
    case 9:
        disallow_engage();
        if (disobey_formation(ship) != 0) {
            reply(ship, 0);
            return;
        }
        reset_objective(ship, OBJECTIVE_HOLD_FORMATION);
        g_nAutoEngageTimer_0046c084 = -150;
        reply(ship, 1);
        return;
    case 10:
    case 11:
        g_bRadioSilence_0046af70 = 0;
        reply(ship, 1);
        g_bRadioSilence_0046af70 = command == 10;
        return;
    case 12:
        cleanup_objectives();
        if (can_land() != 0) {
            g_bLandingAuthorized_00468ff8 = 1;
            send_message(ship, 8);
            return;
        }
        send_message(ship, 9);
        return;
    }
}

/* Function start: 0x42F730 */
unsigned short __stdcall ShouldSuspendCursorForRect(const ShortRect *bounds)
{
    (void)bounds;
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

/* Function start: 0x42F810 */
unsigned int __stdcall GetPacketSize(const char *filename, short section)
{
    PacketSectionHandle handle;
    unsigned int size;

    size = (unsigned int)-1;
    if (OpenPacketSection(filename, section, &handle) != 0) {
        switch (handle.compression) {
        case 1:
            ReadPacketSectionData(&handle, &size, 4);
            if (DAT_00465460 != 0)
                size = (unsigned int)-1;
            break;
        case 2:
        default:
            size = handle.dataSize;
            break;
        }
        CloseDataFileByHandle((unsigned short *)&handle);
    }
    return size;
}

/* Function start: 0x42F930 */
__declspec(naked) void FrameStartHook(int mode)
{
    __asm { jmp TimerResetHook }
}

/* Function start: 0x42F940 */
unsigned short IsSoundHardwarePresent(void)
{
    GetSoundHardwareFlag();
    return 1;
}

/* Function start: 0x42F950 */
__declspec(naked) void MessagePumpHook(int mode)
{
    __asm { jmp TimerStopHook }
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
    unsigned int now;
    unsigned short buttons;
    unsigned short changed;
    unsigned short previous;
    short device;

    device = g_nActiveInputDevice_005a819c;
    if (raw == 0)
        ReadCalibratedJoystick();
    else
        SampleJoystickDevice(&g_aInputDeviceSamples_005a81f0[device],
                             device, 0);

    g_asInputButton2DoubleClick_0059e520[device] = 0;
    g_asInputButton1DoubleClick_0059e508[device] = 0;
    g_asInputButton2Changed_0059e510[device] = 0;
    g_asInputButton1Changed_0059e50c[device] = 0;
    buttons = (unsigned short)g_aInputDeviceSamples_005a81f0[device].buttons;
    previous = (unsigned short)g_asPreviousInputButtons_0059e514[device];
    if (buttons != previous) {
        changed = buttons ^ previous;
        now = DAT_0059ab54;
        if ((changed & 1) != 0) {
            g_asInputButton1Changed_0059e50c[device] = 1;
            if ((buttons & 1) != 0) {
                if ((int)(now -
                    g_anInputButton1PressTime_0059e518[device]) <=
                        g_nInputDoubleClickInterval_0046af54 *
                        g_nInputTickScale_0059af90)
                    g_asInputButton1DoubleClick_0059e508[device] = 1;
                g_anInputButton1PressTime_0059e518[device] = now;
            }
        }
        if ((changed & 2) != 0) {
            g_asInputButton2Changed_0059e510[device] = 1;
            if ((buttons & 2) != 0) {
                if ((int)(now -
                    g_anInputButton2PressTime_0059e500[device]) <=
                        g_nInputDoubleClickInterval_0046af54 *
                        g_nInputTickScale_0059af90)
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
    short doubleClick;

    if (g_nActiveInputDevice_005a819c == -1 ||
        g_bInputPollingGuard_0046a01c != 0)
        return;
    g_bInputPollingGuard_0046a01c++;
    UpdateInputDeviceTransitions(1);
    if (g_asInputButton1Changed_0059e50c
            [g_nActiveInputDevice_005a819c] != 0) {
        doubleClick = 0;
        if (g_asInputButton1DoubleClick_0059e508
                [g_nActiveInputDevice_005a819c] != 0)
            doubleClick = 3;
        QueueInputEventAtCursor(
            ((unsigned short)g_aInputDeviceSamples_005a81f0
                [g_nActiveInputDevice_005a819c].buttons & 1) + 1,
            0, doubleClick);
    }
    if (g_asInputButton2Changed_0059e510
            [g_nActiveInputDevice_005a819c] != 0) {
        doubleClick = 0;
        if (g_asInputButton2DoubleClick_0059e520
                [g_nActiveInputDevice_005a819c] != 0)
            doubleClick = 3;
        QueueInputEventAtCursor(
            (((unsigned short)g_aInputDeviceSamples_005a81f0
                [g_nActiveInputDevice_005a819c].buttons >> 1) & 1) + 1,
            1, doubleClick);
    }
    g_bInputPollingGuard_0046a01c--;
    g_bPreviousPrimaryInputButton_0059af74 =
        (unsigned char)g_aInputDeviceSamples_005a81f0
            [g_nActiveInputDevice_005a819c].buttons;
    g_bPreviousSecondaryInputButton_0059af75 =
        g_bPreviousPrimaryInputButton_0059af74;
}

/* Function start: 0x430920 */
void PollMenuInputDevices(void)
{
    unsigned char changes;
    char doubleClick;
    short movementX;
    short movementY;
    int deviceIndex;

    changes = 0;
    if (g_nActiveInputDevice_005a819c == -1)
        return;
    g_nHostMouseX_0059af70 = g_nMouseX_0059ab10;
    g_nHostMouseY_0059af72 = g_nMouseY_0059ab12;
    if (g_bInputPollingGuard_0046a01c != 0)
        return;
    g_bInputPollingGuard_0046a01c++;
    UpdateInputDeviceTransitions(0);

    deviceIndex = (int)g_nActiveInputDevice_005a819c;
    if (g_asInputButton1Changed_0059e50c[deviceIndex] != 0) {
        g_nHostMouseX_0059af70 +=
            (short)g_aInputDeviceSamples_005a81f0
                [deviceIndex].x *
            g_nMenuPointerSpeed_0046af58;
        g_nHostMouseY_0059af72 +=
            (short)g_aInputDeviceSamples_005a81f0
                [deviceIndex].y *
            g_nMenuPointerSpeed_0046af58;
        if (g_asInputButton1DoubleClick_0059e508[deviceIndex] != 0)
            changes = 3;
        QueueInputEventAtCursor(
            ((unsigned short)g_aInputDeviceSamples_005a81f0
                [deviceIndex].buttons & 1) + 1,
            0, (short)(signed char)changes);
        changes = 1;
    }
    deviceIndex = (int)g_nActiveInputDevice_005a819c;
    if (g_asInputButton2Changed_0059e510[deviceIndex] != 0) {
        g_nHostMouseX_0059af70 +=
            (short)g_aInputDeviceSamples_005a81f0
                [deviceIndex].x *
            g_nMenuPointerSpeed_0046af58;
        g_nHostMouseY_0059af72 +=
            (short)g_aInputDeviceSamples_005a81f0
                [deviceIndex].y *
            g_nMenuPointerSpeed_0046af58;
        doubleClick = 0;
        if (g_asInputButton1DoubleClick_0059e508[deviceIndex] != 0)
            doubleClick = 3;
        QueueInputEventAtCursor(
            (((unsigned short)g_aInputDeviceSamples_005a81f0
                [deviceIndex].buttons >> 1) & 1) + 1,
            1, (short)doubleClick);
        changes++;
    }
    if (changes == 0) {
        movementX = (short)g_aInputDeviceSamples_005a81f0
            [g_nActiveInputDevice_005a819c].x;
        movementY = (short)g_aInputDeviceSamples_005a81f0
            [g_nActiveInputDevice_005a819c].y;
        g_nHostMouseX_0059af70 +=
            g_nMenuPointerSpeed_0046af58 * movementX;
        g_nHostMouseY_0059af72 +=
            g_nMenuPointerSpeed_0046af58 * movementY;
        changes = (unsigned char)movementY | (unsigned char)movementX;
        if (changes != 0) {
            FlushInputEvents();
            if (g_nHostMouseX_0059af70 <= 0)
                g_nHostMouseX_0059af70 = 0;
            if (g_nHostMouseX_0059af70 >= 319)
                g_nHostMouseX_0059af70 = 319;
            if (g_nHostMouseY_0059af72 <= 0)
                g_nHostMouseY_0059af72 = 0;
            if (g_nHostMouseY_0059af72 >= 199)
                g_nHostMouseY_0059af72 = 199;
            SetMousePosition(g_nHostMouseX_0059af70,
                             g_nHostMouseY_0059af72);
        }
    }

    g_bPreviousPrimaryInputButton_0059af74 =
        (unsigned char)g_aInputDeviceSamples_005a81f0
            [g_nActiveInputDevice_005a819c].buttons;
    g_bPreviousSecondaryInputButton_0059af75 =
        g_bPreviousPrimaryInputButton_0059af74;
    if (g_nHostMouseX_0059af70 <= 0)
        g_nHostMouseX_0059af70 = 0;
    if (g_nHostMouseX_0059af70 >= 319)
        g_nHostMouseX_0059af70 = 319;
    if (g_nHostMouseY_0059af72 <= 0)
        g_nHostMouseY_0059af72 = 0;
    if (g_nHostMouseY_0059af72 >= 199)
        g_nHostMouseY_0059af72 = 199;
    if (changes != 0) {
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
short get_face(short rating, enum Side side)
{
    if (rating == -1)
        return 12 + (unsigned short)(side != SIDE_IMPERIAL);
    if (side == SIDE_KILRATHI)
        rating--;
    return rating;
}

/* Function start: 0x430BF0 */
void LoadCommPortraitShape(short face, signed char alternate)
{
    short section;

    if (face >= 0 && face < 8)
        section = face + 1;
    else if ((face >= 8 && face <= 11) || face == 13)
        section = 10;
    else
        section = -1;
    if (section != -1)
        g_apCommPortraitShapes_0059e180[face] =
            (unsigned char *)FetchDiskPacketRetrying(11, section,
                                                     (short)alternate);
}

/* Function start: 0x430C50 */
void ResetCommMenuChoices(short reuse)
{
    short choice;

    choice = 0;
    if (reuse == 0) {
        memset(g_abCommMenuChoiceCommand_0059e488, -1,
               sizeof(g_abCommMenuChoiceCommand_0059e488));
        do {
            g_apszCommMenuChoiceText_0059e4e0[choice] = 0;
            choice++;
        } while (choice < 7);
    }
    g_nCommMenuChoiceCount_0046af60 = 0;
    g_nCommMenuReuseMode_0046af64 = reuse;
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
void OpenCommMenuForTarget(char *heading, char *message)
{
    CockpitMessage(message, DAT_004699a8, 0xff);
    g_pszCommMenuHeading_0059e490 = heading;
}

/* Function start: 0x430D80 */
int IsCommChoiceMenuOpen(void)
{
    return (short)get_mode(1) == 4;
}

/* Function start: 0x430DA0 */
short GetPendingMenuAction(void)
{
    return (short)g_cPendingCommMenuAction_0046af6c;
}

/* Function start: 0x430DB0 */
void SetPendingMenuAction(unsigned char v)
{
    g_cPendingCommMenuAction_0046af6c = (signed char)v;
}

/* Function start: 0x430DC0 */
void OpenCommRecipientMenu(void)
{
    push_mode(1, 4);
    SetPendingMenuAction(1);
}

/* Function start: 0x430DE0 */
void CloseCommChoiceMenu(void)
{
    if ((short)get_mode(1) == 4) {
        pop_mode(1);
        return;
    }
    ShutdownEventManager();
    exit_squadron("stop");
}

/* Function start: 0x430E10 */
int wingman_dead(void)
{
    return g_nYourWingman_0046c04c == -1;
}

/* Function start: 0x430E30 */
int have_target(void)
{
    return unactive((short)g_acShipTarget_0059ce60[0]) == 0;
}

/* Function start: 0x430E50 */
unsigned short CanOpenCommMenu(void)
{
    if (have_target() == 0) {
        if (wingman_dead())
            return 0;
    }
    return 1;
}

/* Function start: 0x430E70 */
void SelectCommRecipient(signed char recipient)
{
    g_cCommMenuRecipient_0046afc4 = recipient;
    SetPendingMenuAction(2);
}

/* Function start: 0x430E90 */
void BuildCommunicationRecipientMenu(void)
{
    short target;
    short command;
    char *text;

    ResetCommMenuChoices(g_nCommMenuReuseMode_0046af64);
    OpenCommMenuForTarget("VID-COM SYSTEM\n\nSend message to?\n\n",
                          "SELECT");
    if (wingman_dead() != 0) {
        SelectCommRecipient(g_acShipTarget_0059ce60[0]);
        return;
    }
    if (have_target() == 0 ||
        g_acShipTarget_0059ce60[0] == g_nYourWingman_0046c04c) {
        SelectCommRecipient((signed char)g_nYourWingman_0046c04c);
        return;
    }
    AppendCommMenuChoice(
        g_apWingmanPilots_00598a30[
            (signed char)g_acShipRating_0059cd80[
                g_nYourWingman_0046c04c]]->callsign,
        1);
    target = (short)g_acShipTarget_0059ce60[0];
    if (target != -1) {
        if (g_aeShipSide_0059d650[target] == SIDE_KILRATHI &&
            g_aeObjectClass_0059d100[target] == OBJECT_CLASS_SHIP) {
            command = 2;
            text = "ENEMY TARGET";
        } else {
            if (g_aeShipSide_0059d650[target] != SIDE_IMPERIAL ||
                ((g_aeObjectClass_0059d100[target] != OBJECT_CLASS_SHIP ||
                  any_enemy(0, 14000) == 0) &&
                 g_aeObjectType_0059b560[target] !=
                     OBJECT_TYPE_TIGERS_CLAW)) {
                goto finish_recipient_menu;
            }
            command = 3;
            text = (char *)g_aObjectTypeData_00466458[
                g_aeObjectType_0059b560[target]].displayName;
        }
        AppendCommMenuChoice(text, command);
    }

finish_recipient_menu:
    SendCommMenuChoice(0);
}

/* Function start: 0x430FC0 */
void BuildCommunicationCommandMenu(void)
{
    signed char rating;
    char *name;

    ResetCommMenuChoices(g_nCommMenuReuseMode_0046af64);
    if (g_cCommMenuRecipient_0046afc4 == g_nYourWingman_0046c04c) {
        if (g_aeShipObjective_0059d200[g_nYourWingman_0046c04c] ==
                OBJECTIVE_HOLD_FORMATION &&
            any_enemy(0, 14000) != 0) {
            SendCommMenuChoice(7);
        }
        if (g_nAutoEngageTimer_0046c084 == -1) {
            if (g_aeShipObjective_0059d200[g_nYourWingman_0046c04c] !=
                    OBJECTIVE_HOLD_FORMATION)
                SendCommMenuChoice(9);
        } else {
            SendCommMenuChoice(8);
        }
        if (g_bRadioSilence_0046af70 == 0)
            SendCommMenuChoice(10);
        else
            SendCommMenuChoice(11);
    }
    if (g_aeShipSide_0059d650[g_cCommMenuRecipient_0046afc4] ==
            g_aeShipSide_0059d650[0]) {
        if (g_aeObjectType_0059b560[g_cCommMenuRecipient_0046afc4] ==
                OBJECT_TYPE_TIGERS_CLAW &&
            g_bLandingAuthorized_00468ff8 == 0) {
            SendCommMenuChoice(12);
        }
        if (have_target() != 0 &&
            g_aeShipSide_0059d650[g_acShipTarget_0059ce60[0]] ==
                SIDE_KILRATHI) {
            SendCommMenuChoice(1);
        }
        if (evaluate_damage(0) < 50 && any_enemy(0, 14000) != 0)
            SendCommMenuChoice(2);
    }
    if (g_cCommMenuRecipient_0046afc4 == g_nYourWingman_0046c04c)
        SendCommMenuChoice(3);
    if (g_aeShipSide_0059d650[g_cCommMenuRecipient_0046afc4] ==
            SIDE_KILRATHI) {
        SendCommMenuChoice(4);
        SendCommMenuChoice(5);
        SendCommMenuChoice(6);
    }
    if (IsCommMenuIdle() == 0)
        SendCommMenuChoice(0);
    else
        CloseCommChoiceMenu();

    if (IsCommChoiceMenuOpen() != 0) {
        strcpy(g_szCommMenuHeadingBuffer_0059e4a0,
               "VID-COM SYSTEM\n\nTo: ");
        rating = g_acShipRating_0059cd80[g_cCommMenuRecipient_0046afc4];
        if (rating == -1) {
            name = (char *)g_aObjectTypeData_00466458[
                g_aeObjectType_0059b560[
                    g_cCommMenuRecipient_0046afc4]].displayName;
        } else if (rating < 8) {
            name = g_apWingmanPilots_00598a30[rating]->callsign;
        } else {
            name = (char *)g_apszKilrathiAceNames_0046af80[rating - 9];
        }
        strcat(g_szCommMenuHeadingBuffer_0059e4a0, name);
        strcat(g_szCommMenuHeadingBuffer_0059e4a0, "\n");
        OpenCommMenuForTarget(g_szCommMenuHeadingBuffer_0059e4a0,
                              "CHOOSE");
    }
}

/* Function start: 0x431200 */
void RefreshCommunicationMenu(void)
{
    if (IsCommChoiceMenuOpen() != 0) {
        if (GetPendingMenuAction() == 1)
            BuildCommunicationRecipientMenu();
        if (GetPendingMenuAction() == 2)
            BuildCommunicationCommandMenu();
        if (g_nCommMenuReuseMode_0046af64 == 0)
            InvalidateVduMode(1);
    }
}

/* Function start: 0x431240 */
void HandleCommunicationMenuRequest(void)
{
    if (IsCommChoiceMenuOpen() != 0)
        CloseCommChoiceMenu();
    if ((short)message_showing() == 0 &&
        IsCommChoiceMenuOpen() == 0 && CanOpenCommMenu() != 0) {
        OpenCommRecipientMenu();
        ResetCommMenuChoices(0);
        RefreshCommunicationMenu();
    }
}

/* Function start: 0x431290 */
void show_communications_disp(void)
{
    signed char choice;

    if (IsCommChoiceMenuOpen() == 0)
        HandleCommunicationMenuRequest();
    if (IsCommChoiceMenuOpen() != 0) {
        set_new_vdu(1);
        choice = 0;
        DrawTextAt(&DAT_005a7700, DAT_005a7530.left,
                   DAT_005a7530.top, g_pszCommMenuHeading_0059e490, 2);
        if (g_nCommMenuChoiceCount_0046af60 > 0) {
            do {
                DrawFormattedText("%d %s", (int)choice + 1,
                                  g_apszCommMenuChoiceText_0059e4e0[
                                      (int)choice]);
                choice++;
            } while (choice < g_nCommMenuChoiceCount_0046af60);
        }
        DrawSpriteDefault(&DAT_005a7530,
                          (short)(DAT_005a7530.left + 36),
                          (short)(DAT_005a7530.top + 10),
                          g_pCommMenuCursorShape_005a7660, 0x19);
        g_nCommMenuReuseMode_0046af64 = 1;
    }
}

/* Function start: 0x431350 */
unsigned int Chosen_communicate_option(short choice)
{
    int action;

    PlaySfxWaveFileByNumber(0x19, -1, 0);
    action = GetPendingMenuAction();
    switch (action) {
    case 0:
        CloseCommChoiceMenu();
        return 0;
    case 1:
        if (g_abCommMenuChoiceCommand_0059e488[choice] == 0) {
            CloseCommChoiceMenu();
            return 0;
        }
        if (g_abCommMenuChoiceCommand_0059e488[choice] == 1) {
            SelectCommRecipient((signed char)g_nYourWingman_0046c04c);
            RefreshCommunicationMenu();
            return 0;
        }
        SelectCommRecipient(g_acShipTarget_0059ce60[0]);
        RefreshCommunicationMenu();
        return 0;
    case 2:
        CloseCommChoiceMenu();
        request(0, (short)g_cCommMenuRecipient_0046afc4,
                (short)g_abCommMenuChoiceCommand_0059e488[choice]);
        return 0;
    }
    return 0;
}

/* Function start: 0x431400 */
void talk_equiv(void)
{
    RefreshCommunicationMenu();
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
    clear_message_time();
    if ((short)get_mode(1) == 6)
        EndCommSessionWithWingman();
    DAT_00469004 = 0;
}

/* Function start: 0x4314F0 */
void ShowCentredPrompt(char *text, unsigned short arg)
{
    DosStrcpy(g_szHudMessageBuffer_0059e1c0, text);
    SetHudMessageText(g_szHudMessageBuffer_0059e1c0,
                      DAT_004699a8, arg);
}

/* Function start: 0x431520 */
short LoadCommDisplayResources(short rating, enum Side side)
{
    short loaded;

    loaded = 1;
    if (side == SIDE_IMPERIAL) {
        if (g_pConfedCommBackground_00469278 == 0)
            g_pConfedCommBackground_00469278 =
                (unsigned char *)FetchDiskPacketRetrying(11, 0, 0);
        loaded = g_pConfedCommBackground_00469278 != 0;
    } else if (side == SIDE_KILRATHI) {
        if (g_pKilrathiCommBackground_00469280 == 0)
            g_pKilrathiCommBackground_00469280 =
                (unsigned char *)FetchDiskPacketRetrying(11, 9, 0);
        loaded = g_pKilrathiCommBackground_00469280 != 0;
    }
    if (g_pCommStaticShape_0046927c == 0)
        g_pCommStaticShape_0046927c =
            (unsigned char *)FetchDiskPacketRetrying(11, 11, 0);
    if (loaded != 0 && g_pCommStaticShape_0046927c != 0)
        return 1;
    return 0;
}

/* Function start: 0x4315C0 */
char *ExpandCommMessageTokens(char *text)
{
    char *destination;
    char *marker;
    short length;

    g_szTextScratchBuffer_00598b00[0] = '\0';
    for (;;) {
        marker = DosStrchr(text, '$');
        if (marker == 0) {
            DosStrcat(g_szTextScratchBuffer_00598b00, text);
            return g_szTextScratchBuffer_00598b00;
        }
        destination = DosStrchr(g_szTextScratchBuffer_00598b00, '\0');
        while (marker != text)
            *destination++ = *text++;
        *destination = '\0';
        text = marker + 2;
        switch (marker[1]) {
        case 'C':
            DosStrcat(
                g_szTextScratchBuffer_00598b00,
                g_stCampaignState_0059ca50.currentPilot->callsign);
            break;
        case 'N':
        case 'P':
            DosStrcat(g_szTextScratchBuffer_00598b00,
                      g_stCampaignState_0059ca50.currentPilot->name);
            break;
        case 'R':
            DosStrcat(g_szTextScratchBuffer_00598b00,
                      g_apszPilotRankNames_00470098[
                          g_stCampaignState_0059ca50.currentPilot->rank]);
            length = DosStrlen(g_szTextScratchBuffer_00598b00);
            if (g_szTextScratchBuffer_00598b00[length - 1] == '.' &&
                *text == '.') {
                length = DosStrlen(g_szTextScratchBuffer_00598b00);
                g_szTextScratchBuffer_00598b00[length - 1] = '\0';
            }
            break;
        }
    }
}

/* Function start: 0x4316E0 */
void real_vid_transmit(short obj, short message)
{
    char text[84];
    char *expandedText;
    char *speech;
    enum Side side;
    int objectOffset;

    g_nCommSpeakerObject_0046afc8 = obj;
    g_nCommSpeakerRating_0046afcc =
        (short)g_acShipRating_0059cd80[obj];
    side = g_aeShipSide_0059d650[obj];
    g_nCommPortraitIndex_0046afd0 =
        get_face(g_nCommSpeakerRating_0046afcc, side);
    if (g_nCommPortraitIndex_0046afd0 == -1)
        return;
    objectOffset = (int)obj * sizeof(enum ObjectType);
    if (DAT_0046af78 != 0 && g_bVideoImagesSuppressed_0046af74 == 0) {
        if (g_apCommPortraitShapes_0059e180[
                g_nCommPortraitIndex_0046afd0] == 0)
            LoadCommPortraitShape(g_nCommPortraitIndex_0046afd0, 0);
        if (g_apCommPortraitShapes_0059e180[
                g_nCommPortraitIndex_0046afd0] != 0 &&
            LoadCommDisplayResources(g_nCommSpeakerRating_0046afcc,
                                     side) != 0) {
            push_mode(1, 6);
            malf_noise(1, 3, 12, 23, 1);
            DrawSpriteDefault(
                &DAT_005a7530, DAT_005a7530.left, DAT_005a7530.top,
                side == SIDE_IMPERIAL ?
                    g_pConfedCommBackground_00469278 :
                    g_pKilrathiCommBackground_00469280,
                0);
            DrawSpriteDefault(
                &DAT_005a7530, DAT_005a7530.left, DAT_005a7530.top,
                g_apCommPortraitShapes_0059e180[
                    g_nCommPortraitIndex_0046afd0],
                0);
        }
    }
    speech = g_aapszPilotSpeech_0059e220[
        g_nCommPortraitIndex_0046afd0][message];
    if (g_nCommSpeakerRating_0046afcc >= 0 &&
        g_nCommSpeakerRating_0046afcc <= 7) {
        sprintf(text, g_szConfedCommFormat_0046b150,
                g_apWingmanPilots_00598a30[
                    g_nCommSpeakerRating_0046afcc]->callsign,
                speech);
    } else if (g_nCommSpeakerRating_0046afcc >= 9 &&
               g_nCommSpeakerRating_0046afcc <= 12) {
        sprintf(text, g_szKilrathiAceCommFormat_0046b158,
                g_apszKilrathiAceNames_0046af80[
                    g_nCommSpeakerRating_0046afcc - 9],
                speech);
    } else {
        sprintf(text, g_szShipCommFormat_0046b160,
                g_aObjectTypeData_00466458[
                    *(enum ObjectType *)(void *)
                        ((unsigned char *)g_aeObjectType_0059b560 +
                         objectOffset)].displayName,
                speech);
    }
    expandedText = ExpandCommMessageTokens(text);
    ShowCentredPrompt(expandedText, (unsigned short)MeasureMessageWidth(text));
}

/* Function start: 0x4318F0 */
__declspec(naked) void __stdcall ShutdownVideoHook(int mode)
{
    __asm {
        call ReleaseVideoResourcesHook
        ret 4
    }
}

/* Function start: 0x431D20 */
short ReadCalibratedJoystick(void)
{
    InputDeviceSample *sample;
    int sampleResult;
    int rawX;
    int rawY;
    int deviceIndex;
    short normalizedX;
    short normalizedY;
    short device;

    device = g_nActiveInputDevice_005a819c;
    if (device == -1)
        return 0;
    deviceIndex = (int)device;
    sample = &g_aInputDeviceSamples_005a81f0[deviceIndex];
    sampleResult = SampleJoystickDevice(
        sample, device,
        g_nJoystickFailureValue_005a81e0);
    rawX = sample->x;
    rawY = g_aInputDeviceSamples_005a81f0[deviceIndex].y;
    if (rawX == g_nJoystickFailureValue_005a81e0 ||
        rawY == g_nJoystickFailureValue_005a81e0 ||
        sampleResult != 0) {
        g_nActiveInputDevice_005a819c = -1;
        g_aInputDeviceSamples_005a81f0[deviceIndex].buttons = 0;
        g_aInputDeviceSamples_005a81f0[deviceIndex].y = 0;
        sample->x = 0;
        return 0;
    }

    g_nJoystickRawX_005a81c0 = rawX;
    g_nJoystickRawY_005a81c4 = rawY;
    if (rawX < g_nJoystickMinimumX_005a81b8)
        sample->x = g_nJoystickMinimumX_005a81b8;
    if (sample->x > g_nJoystickMaximumX_005a81b0)
        sample->x = g_nJoystickMaximumX_005a81b0;
    if (g_aInputDeviceSamples_005a81f0[deviceIndex].y <
        g_nJoystickMinimumY_005a81bc)
        g_aInputDeviceSamples_005a81f0[deviceIndex].y =
            g_nJoystickMinimumY_005a81bc;
    if (g_aInputDeviceSamples_005a81f0[deviceIndex].y >
        g_nJoystickMaximumY_005a81b4)
        g_aInputDeviceSamples_005a81f0[deviceIndex].y =
            g_nJoystickMaximumY_005a81b4;

    normalizedY = 0;
    normalizedX = 0;
    rawX = sample->x;
    if (g_nJoystickCentreX_005a81dc > rawX) {
        normalizedX = (short)((g_nJoystickCentreX_005a81dc - rawX) /
                              g_nJoystickLeftScale_005a81ac);
        if (g_nJoystickHorizontalDeadZone_005a81a4 < normalizedX) {
            if (normalizedX != 0)
                normalizedX = (short)-normalizedX;
        } else {
            normalizedX = 0;
        }
    } else if (g_nJoystickCentreX_005a81dc < rawX) {
        normalizedX = (short)((rawX - g_nJoystickCentreX_005a81dc) /
                              g_nJoystickRightScale_005a81d0);
        if (normalizedX <= g_nJoystickHorizontalDeadZone_005a81a4)
            normalizedX = 0;
    }

    rawY = g_aInputDeviceSamples_005a81f0[deviceIndex].y;
    if (g_nJoystickCentreY_005a81d8 > rawY) {
        normalizedY = (short)((g_nJoystickCentreY_005a81d8 - rawY) /
                              g_nJoystickUpScale_005a81a8);
        if (g_nJoystickVerticalDeadZone_005a81a0 < normalizedY) {
            if (normalizedY != 0)
                normalizedY = (short)-normalizedY;
        } else {
            normalizedY = 0;
        }
    } else if (g_nJoystickCentreY_005a81d8 < rawY) {
        normalizedY = (short)((rawY - g_nJoystickCentreY_005a81d8) /
                              g_nJoystickDownScale_005a81d4);
        if (normalizedY <= g_nJoystickVerticalDeadZone_005a81a0)
            normalizedY = 0;
    }
    sample->x = (int)normalizedX;
    g_aInputDeviceSamples_005a81f0[deviceIndex].y = (int)normalizedY;
    return 1;
}

/* Function start: 0x431EA0 */
void __stdcall UnionRectBounds(ShortRect *destination,
                               const ShortRect *first,
                               const ShortRect *second)
{
    destination->left = first->left < second->left ?
        first->left : second->left;
    destination->top = first->top < second->top ?
        first->top : second->top;
    destination->right = first->right > second->right ?
        first->right : second->right;
    destination->bottom = first->bottom > second->bottom ?
        first->bottom : second->bottom;
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
