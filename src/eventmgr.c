/*
 *  Event manager, mouse pointer, and frame timer.
 *
 *  The provisional address range is recorded in docs/ORDER.md.
 *  The FM Towns executable preserves the original source name
 *  "source\\eventmgr.c" for this unit.
 */
#include "wc1.h"
#ifndef SDL_PORT
#include <malloc.h>
#endif
#include <sys/types.h>

/* The SDL renderer also reads the saved cursor background. */
#ifndef SDL_PORT
static
#endif
unsigned char abCursorSaveArea[0x1000]; /* 0x004865a8 */

/* Linker commons defined here; LINK orders them by identifier hash. */
InputEvent aInputEvtPool[0x100]; /* 0x00598c40 */
int nMouseCursorSavedCoordY; /* 0x0059a840 */
int nMouseCursorSavedX; /* 0x0059a844 */
unsigned char bInputModeState; /* 0x0059a848 */
unsigned char bFilteredKeyWaitActive; /* 0x0059a849 */
int bEventMouseCursorDrawnFlag; /* 0x0059a84c */
short nEventManagerActive; /* 0x0059a850 */
short nCurrentViewCenterPosX; /* 0x0059a852 */
short nCurrentViewCenterPosY; /* 0x0059a854 */
short bCurrentExpandedMemoryAvail; /* 0x0059a856 */
short nPktDecompressSourceFile; /* 0x0059a858 */
unsigned char abInputKeyStateArray[0x80]; /* 0x0059a860 */
int nMousePointerDrawnY; /* 0x0059a8e0 */
int nMousePointerDrawnX; /* 0x0059a8e4 */
int anEventObjectDepthPlacedList[SPACE_OBJECT_COUNT]; /* 0x0059a8f0 */
short DAT_0059a9f0;

/* Function start: 0x4355F0 */
void TranslatePolledInputEvent(unsigned short type, unsigned int value)
{
    InputDeviceSample *samples;
    unsigned int state;

    switch (type) {
    case 2:
        state = (value >> 2) |
                (bHostSecondaryMouseButton << 1) |
                bHostPrimaryMouseButton;
        QueueInputEvent(type, nHostMouseMessageX, nHostMouseMessageY, 0,
                        bHostPrimaryMouseButton, bHostSecondaryMouseButton,
                        state);
        return;
    case 6:
        samples = aInputDevSamples;
        QueueInputEvent(type, samples[nCurrentActiveInputDevice].x,
                        samples[nCurrentActiveInputDevice].y, 0, 0, 0, 0);
        return;
    case 13:
        QueueInputEvent(type, nHostMouseMessageX, nHostMouseMessageY,
                        0, 0, 0, 0);
        return;
    }
}

/* Function start: 0x4356A0 */
void QueueInputEventAtCursor(unsigned short type, short primaryButton,
                             short secondaryButton)
{
    int eventType = type;

    QueueInputEvent(eventType, stMouseCursorState.x, stMouseCursorState.y, 0,
                    primaryButton, secondaryButton, 0);
}

/* Function start: 0x4356E0 */
InputEvent *AllocateInputEvent(void)
{
    int *used;
    int index;

    if (bInputEventPoolInitialized != 0) {
        index = 0;
        used = aiInputEventSlotUsedTable;
        do {
            if (*used == 0) {
                aiInputEventSlotUsedTable[index] = 1;
                return &aInputEvtPool[index];
            }
            used++;
            index++;
        } while (used < &aiInputEventSlotUsedTable[0x100]);
        return 0;
    }
    memset(aInputEvtPool, 0,
           sizeof(aInputEvtPool));
    memset(aiInputEventSlotUsedTable, 0,
           sizeof(aiInputEventSlotUsedTable));
    bInputEventPoolInitialized = 1;
    aiInputEventSlotUsedTable[0] = 1;
    return &aInputEvtPool[0];
}

/* Function start: 0x435760 */
void ReleaseInputEvent(InputEvent *event)
{
    int *used;
    int slot;

    used = aiInputEventSlotUsedTable;
    for (slot = 0; slot < 0x100; used++, slot++) {
        if (&aInputEvtPool[slot] == event)
            *used = 0;
    }
}

/* Function start: 0x435790 */
void QueueInputEvent(int type, int x, int y, int value,
                     int primaryButton, int secondaryButton,
                     unsigned int timestamp)
{
    unsigned int modifiers;
    InputEvent *event;

    modifiers = 0;
    if (GetShiftKeyState() != 0)
        modifiers = 0xe0;
    if (GetControlKeyState() != 0)
        modifiers |= 0x2000;
    if (GetKeyboardModifiers() != 0)
        modifiers |= 0x700;
    if (primaryButton != 0)
        modifiers |= 2;
    if (secondaryButton != 0)
        modifiers |= 4;

    if (pInputEventHead == 0) {
        pInputEventHead = AllocateInputEvent();
        if (pInputEventHead == 0) {
            ReleaseInputEventQueue();
            return;
        }
        pInputEventTail = pInputEventHead;
        pInputEventHead->next = 0;
        pInputEventTail->previous = 0;
    } else {
        event = AllocateInputEvent();
        pInputEventTail->next = event;
        if (pInputEventTail->next == 0) {
            ReleaseInputEventQueue();
            return;
        }
        pInputEventTail->next->previous =
            pInputEventTail;
        pInputEventTail = pInputEventTail->next;
        pInputEventTail->next = 0;
    }
    pInputEventTail->type = type;
    pInputEventTail->modifiers = modifiers;
    pInputEventTail->x = x;
    pInputEventTail->y = y;
    pInputEventTail->value = value;
    pInputEventTail->primaryButton = (short)primaryButton;
    pInputEventTail->secondaryButton = (short)secondaryButton;
}

/* Function start: 0x4358B0 */
void ReleaseInputEventQueue(void)
{
    InputEvent *event = pInputEventHead;

    while (event != 0) {
        InputEvent *next = event->next;

        ReleaseInputEvent(event);
        event = next;
    }
    pInputEventTail = 0;
    pInputEventHead = 0;
}

/* Function start: 0x4358E0 */
void RetainInputEventsOfType(int type)
{
    InputEvent *event = pInputEventHead;

    while (event != 0) {
        InputEvent *next = event->next;

        if (event->type != type) {
            if (event->previous != 0)
                event->previous->next = next;
            else
                pInputEventHead = next;
            if (event->next != 0)
                event->next->previous = event->previous;
            else
                pInputEventTail = event->previous;
            ReleaseInputEvent(event);
        }
        event = next;
    }
}

/* Function start: 0x435940 */
void RemoveInputEvent(InputEvent *event)
{
    if (event->previous != 0) {
        if (event->next != 0) {
            event->previous->next = event->next;
            event->next->previous = event->previous;
            ReleaseInputEvent(event);
            return;
        }
        event->previous->next = 0;
        pInputEventTail = event->previous;
        ReleaseInputEvent(event);
        return;
    }
    if (event->next != 0) {
        pInputEventHead = event->next;
        event->next->previous = 0;
        ReleaseInputEvent(event);
        return;
    }
    pInputEventTail = 0;
    pInputEventHead = 0;
    ReleaseInputEvent(event);
}

/* Function start: 0x4359C0 */
short __stdcall GetNextInputEvent(InputEventState *state)
{
    int eventX;
    int eventY;
    short limit;
    int type;

    type = 0;
    if (pInputEventHead != 0) {
        eventX = pInputEventHead->x;
        eventY = pInputEventHead->y;
        limit = stMouseCursorState.viewport->left;
        if (limit > eventX)
            pInputEventHead->x = limit;
        else {
            limit = stMouseCursorState.viewport->right;
            if (limit < eventX)
                pInputEventHead->x = limit;
        }
        limit = stMouseCursorState.viewport->top;
        if (limit > eventY)
            pInputEventHead->y = limit;
        else {
            limit = stMouseCursorState.viewport->bottom;
            if (limit < eventY)
                pInputEventHead->y = limit;
        }

        state->modifiers =
            (short)pInputEventHead->modifiers;
        switch (pInputEventHead->type) {
        case 1:
            stMouseCursorState.x = pInputEventHead->x;
            stMouseCursorState.y = pInputEventHead->y;
            stMouseCursorState.primaryButton = 0;
            state->x = pInputEventHead->x;
            state->y = pInputEventHead->y;
            type = 1;
            break;
        case 2:
            stMouseCursorState.x = pInputEventHead->x;
            stMouseCursorState.y = pInputEventHead->y;
            stMouseCursorState.primaryButton =
                (unsigned char)pInputEventHead->primaryButton;
            stMouseCursorState.secondaryButton =
                (unsigned char)pInputEventHead->secondaryButton;
            state->x = pInputEventHead->x;
            state->y = pInputEventHead->y;
            type = 2;
            state->value =
                (int)pInputEventHead->secondaryButton * 2 |
                (int)pInputEventHead->primaryButton;
            break;
        case 3:
            state->value = pInputEventHead->value;
            state->x = stMouseCursorState.x;
            state->y = stMouseCursorState.y;
            type = 3;
            break;
        case 4:
            state->x = pInputEventHead->value;
            state->x = stMouseCursorState.x;
            state->y = stMouseCursorState.y;
            type = 4;
            break;
        case 5:
            state->x = pInputEventHead->value;
            type = 5;
            break;
        case 6:
            state->x = pInputEventHead->x;
            state->y = pInputEventHead->y;
            type = 6;
            break;
        case 7:
            state->x = pInputEventHead->x;
            state->y = pInputEventHead->y;
            type = 7;
            break;
        case 8:
            state->x = pInputEventHead->x;
            state->y = pInputEventHead->y;
            type = 8;
            break;
        case 9:
            state->x = pInputEventHead->x;
            state->y = pInputEventHead->y;
            type = 9;
            break;
        case 10:
            state->x = pInputEventHead->x;
            state->y = pInputEventHead->y;
            type = 10;
            break;
        case 13:
            stMouseCursorState.x = pInputEventHead->x;
            stMouseCursorState.y = pInputEventHead->y;
            state->x = pInputEventHead->x;
            state->y = pInputEventHead->y;
            type = 13;
            break;
        }
        RemoveInputEvent(pInputEventHead);
    }
    return type;
}

/* Function start: 0x435CC0 */
short __stdcall PollInputEvent(InputEventState *event, short filter)
{
    PumpWindowMessages();
    return GetNextInputEvent(event);
}

/* Function start: 0x435CE0 */
short __stdcall PeekInputEvent(InputEventState *state, short type)
{
    InputEvent *event;
    int eventType;
    int modifiers;

    event = pInputEventHead;
    modifiers = 0;
    while (event != 0 && event->type != type)
        event = event->next;
    if (event != 0) {
        state->type = event->type;
        state->value = event->modifiers;
        state->timestamp = event->timestamp;
        eventType = (int)event->type;
        if (eventType == 2 || eventType == 1)
            modifiers = 1;
        modifiers |= ((unsigned short)event->primaryButton < 1 ? 0 : -1) & 2;
        modifiers |= ((unsigned short)event->secondaryButton < 1 ? 0 : -1) & 4;
        state->modifiers = (short)modifiers;
        state->x = event->x;
        state->y = event->y;
        return 1;
    }
    return 0;
}

/* Function start: 0x435D80 */
short __stdcall IsInputEventQueued(short type)
{
    InputEvent *event = pInputEventHead;

    while (event != 0 && event->type != type)
        event = event->next;
    return event != 0;
}

/* Function start: 0x435DB0 */
void FlushInputEvents(void)
{
    ReleaseInputEventQueue();
}

/* Function start: 0x435DC0 */
short __stdcall ResetAllocationDepth(int x, int y)
{
    (void)x;
    (void)y;
    nMouseCursorShowCount = 0;
    return 1;
}

/* Function start: 0x435DD0 */
/* Returns 1. Nothing in the shipped executable calls it; reporting an
 * installed mouse is inferred only from its place after the mouse-reset
 * stand-in at 0x435DC0, which also returns 1. */
short IsMouseInstalled(void)
{
    return 1;
}

/* Function start: 0x435DE0 */
void CheckCursor(void)
{
}

/* Function start: 0x435DF0 */
/* Empty in the Win32 build; the hidden kill-board code at 0x43F8B0 calls it. */
void RefreshMouseCursorHook(void)
{
}

/* Function start: 0x435E00 */
/* Empty. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it sits between the cursor hooks and the cursor
 * drawing routines. */
void MouseCursorRedrawHook(void)
{
}

/* Function start: 0x435E10 */
/* Empty. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it immediately precedes the cursor background
 * capture. */
void MouseCursorCaptureHook(void)
{
}

/* Function start: 0x435E20 */
void CaptureMouseCursorBackground(void)
{
    int x;
    int y;

    if (nMouseCursorShowCount == 0 ||
        stMouseCursorState.viewport == 0 ||
        stMouseCursorState.shape == 0)
        return;

    CaptureSpriteBackground(stMouseCursorState.viewport,
                            abCursorSaveArea,
                            stMouseCursorState.x,
                            stMouseCursorState.y,
                            stMouseCursorState.shape,
                            stMouseCursorState.frame);
    x = stMouseCursorState.x;
    y = stMouseCursorState.y;
    if (nMouseCursorDamageLeft > x - 16)
        nMouseCursorDamageLeft = x - 16;
    nMousePointerDrawnX = x;
    if (nMouseCursorDamageRight < x + 16)
        nMouseCursorDamageRight = x + 16;
    if (nMousePointerDamageTopValue > y - 16)
        nMousePointerDamageTopValue = y - 16;
    nMousePointerDrawnY = y;
    if (nEventMousePointerDamageBottom < y + 16)
        nEventMousePointerDamageBottom = y + 16;
    bMouseCursorDamagePending = 1;
    bEventMouseCursorDrawnFlag = 1;
}

/* Function start: 0x435EF0 */
void DrawMouseCursor(void)
{
    int x;
    int y;

    if (nMouseCursorShowCount == 0 ||
        stMouseCursorState.viewport == 0 ||
        stMouseCursorState.shape == 0)
        return;

    DrawSpriteDefault(stMouseCursorState.viewport,
                      stMouseCursorState.x,
                      stMouseCursorState.y,
                      stMouseCursorState.shape,
                      stMouseCursorState.frame);
    x = stMouseCursorState.x;
    if (nMouseCursorDamageLeft > x - 16)
        nMouseCursorDamageLeft = x - 16;
    if (nMouseCursorDamageRight < x + 16)
        nMouseCursorDamageRight = x + 16;
    y = stMouseCursorState.y;
    if (nMousePointerDamageTopValue > y - 16)
        nMousePointerDamageTopValue = y - 16;
    if (nEventMousePointerDamageBottom < y + 16)
        nEventMousePointerDamageBottom = y + 16;
    bMouseCursorDamagePending = 1;
}

/* Function start: 0x435FA0 */
void RestoreMouseCursorBackground(void)
{
    int x;
    int y;

    if (nMouseCursorShowCount == 0 ||
        stMouseCursorState.viewport == 0 ||
        stMouseCursorState.shape == 0 ||
        bEventMouseCursorDrawnFlag == 0)
        return;

    RestoreSpriteBackground(stMouseCursorState.viewport,
                            abCursorSaveArea,
                            (short)nMousePointerDrawnX,
                            (short)nMousePointerDrawnY,
                            stMouseCursorState.shape,
                            stMouseCursorState.frame);
    x = nMousePointerDrawnX;
    if (nMouseCursorDamageLeft > x - 16)
        nMouseCursorDamageLeft = x - 16;
    if (nMouseCursorDamageRight < x + 16)
        nMouseCursorDamageRight = x + 16;
    y = nMousePointerDrawnY;
    if (nMousePointerDamageTopValue > y - 16)
        nMousePointerDamageTopValue = y - 16;
    if (nEventMousePointerDamageBottom < y + 16)
        nEventMousePointerDamageBottom = y + 16;
    bEventMouseCursorDrawnFlag = 0;
}

/* Function start: 0x436060 */
void RefreshMouseCursorDisplay(void)
{
    nMouseCursorDamageLeft = 319;
    nMousePointerDamageTopValue = 199;
    nMouseCursorDamageRight = 0;
    nEventMousePointerDamageBottom = 0;
    bMouseCursorDamagePending = 0;
    bEventMouseCursorDrawnFlag = 0;
    CaptureMouseCursorBackground();
    DrawMouseCursor();
    DIBupdate(nMouseCursorDamageLeft,
              nMousePointerDamageTopValue,
              nMouseCursorDamageRight,
              nEventMousePointerDamageBottom);
    RestoreMouseCursorBackground();
}

/* Function start: 0x4360C0 */
/* Empty. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it follows the cursor display refresh. */
void MouseCursorDisplayHook(void)
{
}

/* Function start: 0x4360D0 */
void EnterAllocationScope(void)
{
    nMouseCursorShowCount = nMouseCursorShowCount + 1;
}

/* Function start: 0x4360E0 */
void LeaveAllocationScope(void)
{
    nMouseCursorShowCount = nMouseCursorShowCount - 1;
}

/* Function start: 0x4360F0 */
void __stdcall SetMouseCursorShape(unsigned char *shape, short frame)
{
    stMouseCursorState.shapeChanged = 1;
    if (stMouseCursorState.viewport != 0 &&
        nMouseCursorShowCount > 0 &&
        pDrawnMouseCursorShape != 0) {
        RestoreSpriteBackground(stMouseCursorState.viewport,
                                abCursorSaveArea,
                                (short)nMouseCursorSavedX,
                                (short)nMouseCursorSavedCoordY,
                                pDrawnMouseCursorShape,
                                (short)stMouseCursorState.frame);
        pDrawnMouseCursorShape = 0;
    }
    stMouseCursorState.shape = shape;
    stMouseCursorState.frame = frame;
}

/* Function start: 0x436160 */
void __stdcall SetMouseHomePosition(short x, short y)
{
    stMouseCursorState.x = x;
    stMouseCursorState.y = y;
    SetMousePositionDuplicate(x, y);
}

/* Function start: 0x436190 */
void __stdcall ApplyPackedMousePosition(ShortPoint point)
{
    stMouseCursorState.x = point.x;
    stMouseCursorState.y = point.y;
    SetMousePositionDuplicate(point.x, point.y);
}

/* Function start: 0x4361C0 */
void __stdcall GetMouseCursorPosition(ShortPoint *point)
{
    point->x = stMouseCursorState.x;
    point->y = stMouseCursorState.y;
}

/* Function start: 0x4361E0 */
void SetFrameTimerPeriod(short period)
{
    SetMultimediaTimerCallback((int)period);
}

/* Function start: 0x4361F0 */
void __stdcall SetFrameTimerAndWait(short period)
{
    SetFrameTimerPeriod(period);
    WaitForFrameTick();
}

/* Function start: 0x436210 */
void __stdcall SetFrameTimerPeriodDirect(short p)
{
    SetMultimediaTimerCallback((int)p);
}

/* Function start: 0x436230 */
void WaitForFrameTick(void)
{
    while (bFrameTickPendingFlag != 0) ;
}

/* Function start: 0x436240 */
int IsFrameTickElapsed(void)
{
    return bFrameTickPendingFlag == 0;
}

/* Function start: 0x436250 */
/* Empty. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it follows the frame-tick queries. */
void FrameTickHook(void)
{
}

/* Function start: 0x436260 */
unsigned short GetSoundHardwareFlag(void)
{
    return 1;
}

/* Function start: 0x436270 */
void *AllocateUntaggedMemory(unsigned int size)
{
    return AllocateTaggedMemory(size, 0);
}

/* Function start: 0x436280 */
/* Returns 0. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it sits between the untagged allocation and release
 * stand-ins. */
int UntaggedMemoryHook(void)
{
    return 0;
}

/* Function start: 0x436290 */
void ReleaseUntaggedMemory(void *memory)
{
    ReleasePacketHandle(memory);
}

/* Function start: 0x4362A0 */
void TimerResetHook(void)
{
}

/* Function start: 0x4362B0 */
unsigned int GetPacketDecompressionResult(void *packet)
{
    (void)packet;
    return 0;
}

/* Function start: 0x4362C0 */
unsigned short IdentityWord(unsigned short v)
{
    return v;
}

/* Function start: 0x4362D0 */
void TimerStopHook(void)
{
}

/* Function start: 0x4362E0 */
unsigned int GetFixedOneMillion(void)
{
    return 0x3e8000;
}

/* Function start: 0x4362F0 */
unsigned int GetFixedOneMillionAlt(void)
{
    return 0x3e8000;
}

/* Function start: 0x436300 */
/* Returns 0. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it follows the free-memory stand-ins that the memory
 * manager reaches through 0x42FB20 and 0x42FB30. The role words of this
 * and the following eight names only distinguish the entries. */
int __stdcall MemoryServiceQueryHook(int value)
{
    (void)value;
    return 0;
}

/* Function start: 0x436310 */
/* Empty. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it sits among the memory-service stand-ins. */
void __stdcall MemoryServiceReleaseHook(int value)
{
    (void)value;
}

/* Function start: 0x436320 */
/* Returns 0. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it sits among the memory-service stand-ins. */
int __stdcall MemoryServiceTransferHook(int first, int second)
{
    (void)first;
    (void)second;
    return 0;
}

/* Function start: 0x436330 */
/* Returns 0. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it sits among the memory-service stand-ins. */
int __stdcall MemoryServiceMapHook(int value)
{
    (void)value;
    return 0;
}

/* Function start: 0x436340 */
/* Returns 0. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it sits among the memory-service stand-ins. */
short MemoryServiceVersionHook(void)
{
    return 0;
}

/* Function start: 0x436350 */
/* Empty. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it sits among the memory-service stand-ins. */
void MemoryServiceShutdownHook(void)
{
}

/* Function start: 0x436360 */
/* Returns 0. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it sits among the memory-service stand-ins. */
int MemoryServiceStatusHook(void)
{
    return 0;
}

/* Function start: 0x436370 */
/* Returns 0. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it sits among the memory-service stand-ins. */
short MemoryServiceErrorHook(void)
{
    return 0;
}

/* Function start: 0x436380 */
/* Returns 0. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it is the last of the memory-service stand-ins. */
short MemoryServiceResetHook(void)
{
    return 0;
}

/* Function start: 0x436390 */
void ResetInputKeyStatePreservingModifiers(void)
{
    ClearInputKeyStatePreservingModifiers();
}

/* Function start: 0x4363A0 */
void ClearInputKeyStatePreservingModifiers(void)
{
    unsigned int control = abInputKeyStateArray[0x1d];
    unsigned int alt = abInputKeyStateArray[0x38];

    memset(abInputKeyStateArray, 0,
           sizeof(abInputKeyStateArray));
    abInputKeyStateArray[0x1d] = control;
    abInputKeyStateArray[0x38] = alt;
    ClearDebugPauseFlags();
}

/* Function start: 0x4363E0 */
void ClearInputKeyState(void)
{
    memset(abInputKeyStateArray, 0,
           sizeof(abInputKeyStateArray));
    ClearDebugPauseFlags();
}

/* Function start: 0x436400 */
/* Empty. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it follows the key-state clearing routines. */
void KeyboardStateHook(void)
{
}

/* Function start: 0x436410 */
/* Empty. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it precedes the key-state setter. */
void KeyboardScanHook(void)
{
}

/* Function start: 0x436420 */
void SetInputKeyState(int scanCode, unsigned char pressed)
{
    if (scanCode >= 0 && scanCode < 0x80) {
        abInputKeyStateArray[scanCode] = pressed;
        return;
    }
    SystemDebugPrintf("keyboard almost messed up\n");
    ClearDebugPauseFlags();
    PumpMessagesDuringWait();
    exit(1);
}

/* Function start: 0x436460 */
void sort_object_depth(void)
{
    int previous;
    unsigned int distance;
    int bestObject;
    int obj;
    int *placed;
    int screenOffset;
    int best;
    int *sortedEntry;
    int sorted;

    previous = -999999999;
    bestObject = -1;
    memset(anEventObjectDepthPlacedList, 0,
           sizeof(anEventObjectDepthPlacedList));
    obj = 0;
    for (; obj < SPACE_OBJECT_COUNT; obj++) {
        distance = (unsigned short)asObjDistancesTable[obj];
        if (previous < (int)distance) {
            previous = (int)distance;
            bestObject = obj;
        }
    }
    sorted = 0;
    sortedEntry = anSortedObjects;
    for (; sorted < SPACE_OBJECT_COUNT; sorted++, sortedEntry++) {
        *sortedEntry = bestObject;
        best = -1;
        if (bestObject == -1)
            return;
        screenOffset = 0;
        obj = 0;
        anEventObjectDepthPlacedList[bestObject] = 1;
        bestObject = -1;
        placed = anEventObjectDepthPlacedList;
        for (; placed < anEventObjectDepthPlacedList +
                           SPACE_OBJECT_COUNT;
             screenOffset += sizeof(short), placed++, obj++) {
            if (*placed == 0 &&
                *(short *)((unsigned char *)asObjectScreenXList +
                           screenOffset) != (short)0x8001) {
                distance = *(unsigned short *)(
                    (unsigned char *)asObjDistancesTable +
                    screenOffset);
                if (best < (int)distance && previous >= (int)distance) {
                    bestObject = obj;
                    best = (int)distance;
                }
            }
        }
    }
}

/* Function start: 0x436520 */
void draw_sorted_objects_to_buffer(void)
{
    int *sortedEntry;
    int obj;
    int objectClass;
    unsigned char *shape;
    short screenY;
    short screenX;
    int specialObject;
#ifdef SDL_PORT
    float enhancedScreenX;
    float enhancedScreenY;
    short projectedScreenX;
    short projectedScreenY;
#endif

    sortedEntry = anSortedObjects;
    do {
        obj = *sortedEntry;
        if (obj < 0)
            return;
        if ((int)aeObjectTypeList[obj] < 0)
            return;
        objectClass = aeObjectClass[obj];
#ifdef SDL_PORT
        enhancedScreenX = (float)(short)(
            asObjectScreenXList[obj] + nCurrentViewCenterPosX);
        enhancedScreenY = (float)(short)(
            asObjectScreenYArray[obj] + nCurrentViewCenterPosY);
        if (objectClass != OBJECT_CLASS_NULL &&
            objectClass != OBJECT_CLASS_FIXED_OBJECT &&
            obj != nNavPointerObject &&
            aObjectViewPositionsList[obj].z != 0) {
            projectedScreenX = (short)(DivideFixed(
                MultiplyFixed(
                    (short)(nScreenWidth & ~1) << 7,
                    aObjectViewPositionsList[obj].x),
                aObjectViewPositionsList[obj].z) >> 8);
            projectedScreenY = (short)(DivideFixed(
                MultiplyFixed(
                    (short)(nScreenWidth & ~1) << 7,
                    aObjectViewPositionsList[obj].y),
                aObjectViewPositionsList[obj].z) >> 8);
            if (projectedScreenX == asObjectScreenXList[obj] &&
                projectedScreenY == asObjectScreenYArray[obj]) {
                enhancedScreenX =
                    (float)nCurrentViewCenterPosX +
                    (float)(((double)(nScreenWidth & ~1) * 0.5 *
                             aObjectViewPositionsList[obj].x) /
                            aObjectViewPositionsList[obj].z);
                enhancedScreenY =
                    (float)nCurrentViewCenterPosY +
                    (float)(((double)(nScreenWidth & ~1) * 0.5 *
                             aObjectViewPositionsList[obj].y) /
                            aObjectViewPositionsList[obj].z);
            }
        } else if (objectClass == OBJECT_CLASS_FIXED_OBJECT &&
                   aeObjectTypeList[obj] == OBJECT_TYPE_THRUSTERS) {
            SdlGetThrusterScreenPosition(
                (short)obj, &enhancedScreenX, &enhancedScreenY);
        }
#endif
        if (objectClass != OBJECT_CLASS_NULL) {
            switch (objectClass) {
            default: {
                short objectX;
                short objectY;
                unsigned char *objectShape;

                objectX = asObjectScreenXList[obj];
                objectY = *(short volatile *)&asObjectScreenYArray[obj];
                objectX = (short)(objectX +
                                  nCurrentViewCenterPosX);
                objectShape = *(unsigned char *volatile *)&apObjShapeArray[obj];
                asObjectDrawPosX[obj] = objectX;
                objectY = (short)(objectY +
                                  nCurrentViewCenterPosY);
                asObjectDrawPosY[obj] = objectY;
                if (objectShape != 0) {
#ifdef SDL_PORT
                    if (!SdlRecordSpaceSprite(
                            &stSpaceBufferViewport, enhancedScreenX, enhancedScreenY,
                            objectShape,
                            asObjectViewFrames[obj],
                            asObjScreenAngles[obj],
                            asObjScreenScalesArray[obj],
                            asObjectFlipArray[obj]))
#endif
                    DrawSpriteScaled(
                        &stSpaceBufferViewport, objectX, objectY, objectShape,
                        asObjectViewFrames[obj],
                        asObjScreenAngles[obj],
                        asObjScreenScalesArray[obj],
                        asObjectFlipArray[obj]);
                }
                break;
            }
            case OBJECT_CLASS_STAR:
#ifdef SDL_PORT
                /* WCDX fix: planets use the per-object scaled path above. */
#else
            case OBJECT_CLASS_PLANET:
#endif
            case OBJECT_CLASS_DUST:
                specialObject = (int)nNavPointerObject;
                screenY = asObjectScreenYArray[obj];
                screenX = (short)(asObjectScreenXList[obj] +
                                  nCurrentViewCenterPosX);
                asObjectDrawPosX[obj] = screenX;
                screenY = (short)(screenY + nCurrentViewCenterPosY);
                asObjectDrawPosY[obj] = screenY;
                if (specialObject == obj) {
                    shape = apObjShapeArray[obj];
#ifdef SDL_PORT
                    if (!SdlRecordSpaceSprite(
                            &stSpaceBufferViewport, enhancedScreenX,
                            enhancedScreenY, shape,
                            asObjectViewFrames[obj], 0, 0x100, 0))
#endif
                    DrawSpriteDefault(&stSpaceBufferViewport, screenX, screenY, shape,
                                      asObjectViewFrames[obj]);
                } else {
                    shape = pCurrentConstellationShape;
#ifdef SDL_PORT
                    if (!SdlRecordSpaceSprite(
                            &stSpaceBufferViewport, enhancedScreenX,
                            enhancedScreenY, shape,
                            asObjectViewFrames[obj], 0, 0x100, 0))
#endif
                    DrawSpriteDefault(&stSpaceBufferViewport, screenX, screenY, shape,
                                      asObjectViewFrames[obj]);
                }
                break;
            }
        }
        sortedEntry++;
    } while (sortedEntry < anSortedObjects +
                           SPACE_OBJECT_COUNT);
}

/* Function start: 0x436650 */
void intro_drawbackgroundships(void)
{
    unsigned char *shape;
    int objectClass;
    int obj;

    for (obj = 0; obj < SPACE_OBJECT_COUNT; obj++) {
        if (aeObjectTypeList[obj] < 0)
            return;
        objectClass = aeObjectClass[obj];
        if (objectClass != OBJECT_CLASS_NULL) {
            switch (objectClass) {
            default:
                shape = apObjShapeArray[obj];
                if (shape != 0) {
                    DrawSolidColourSpriteScaled(
                        &stSpaceBufferViewport,
                        asObjectDrawPosX[obj],
                        asObjectDrawPosY[obj],
                        shape,
                        asObjectViewFrames[obj],
                        asObjScreenAngles[obj],
                        asObjScreenScalesArray[obj],
                        asObjectFlipArray[obj],
                        cPrimaryViewBufferColour);
                }
                break;
            case OBJECT_CLASS_STAR:
#ifdef SDL_PORT
                /* Erase planets with the same scaled geometry used to draw. */
#else
            case OBJECT_CLASS_PLANET:
#endif
            case OBJECT_CLASS_DUST:
                if (obj == nNavPointerObject) {
                    DrawSolidColourSprite(
                        &stSpaceBufferViewport,
                        asObjectDrawPosX[obj],
                        asObjectDrawPosY[obj],
                        apObjShapeArray[obj],
                        asObjectViewFrames[obj],
                        cPrimaryViewBufferColour);
                } else {
                    DrawSolidColourSprite(
                        &stSpaceBufferViewport,
                        asObjectDrawPosX[obj],
                        asObjectDrawPosY[obj],
                        pCurrentConstellationShape,
                        asObjectViewFrames[obj],
                        cPrimaryViewBufferColour);
                }
                break;
            }
        }
    }
}

/* Function start: 0x436740 */
void set_up_screen_viewport(signed char mode)
{
    int modeIndex;

    cScreenViewMode = mode;
    modeIndex = (int)mode;
    switch (modeIndex) {
    case 4:
    case 5:
        pScreenViewGeometry =
            &aScreenViewportGeometry[modeIndex];
        break;
    default:
        pScreenViewGeometry =
            (const ScreenViewportGeometry *)(
                (const unsigned char *)pCurrentScreenViewportPkt +
                pCurrentScreenViewportPkt
                    ->geometryOffsets[modeIndex]);
        break;
    }

    if (bCockpitlessView != 0 && bCockpitlessView != -2) {
        nScreenWidth = pScreenViewGeometry->width;
        nCurrentViewCenterPosX = (short)(nScreenWidth / 2);
        nScreenHeight = pScreenViewGeometry->height;
        nCurrentViewCenterPosY = (short)(nScreenHeight / 2);
        nViewportOriginPositionX = pScreenViewGeometry->originX;
        nViewportOriginPositionY = pScreenViewGeometry->originY;
        switch ((int)cCurrentCockpitView) {
        case 0:
            nViewportOriginPositionY += 10;
            nCurrentViewCenterPosY += 10;
            break;
        case 1:
            nViewportOriginPositionY += 25;
            nCurrentViewCenterPosY += 25;
            break;
        case 2:
            nViewportOriginPositionY += 50;
            nCurrentViewCenterPosY += 50;
            break;
        }
        nScreenWidth = 320;
        nScreenHeight = 200;
        return;
    }

    nScreenWidth = pScreenViewGeometry->width;
    nCurrentViewCenterPosX = (short)(nScreenWidth / 2);
    nScreenHeight = pScreenViewGeometry->height;
    nCurrentViewCenterPosY = (short)(nScreenHeight / 2);
    nViewportOriginPositionX = pScreenViewGeometry->originX;
    nViewportOriginPositionY = pScreenViewGeometry->originY;
}

/* Function start: 0x436890 */
/* Returns 0. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it follows set_up_screen_viewport. */
int ScreenViewportHook(void)
{
    return 0;
}

/* Function start: 0x4368A0 */
/* Empty. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it follows set_up_screen_viewport. */
void ScreenViewportResetHook(void)
{
}

/* Function start: 0x4368B0 */
/* Empty. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it precedes the mouse idle hook. */
void MouseActivityHook(void)
{
}

/* Function start: 0x4368C0 */
void MouseIdleHook(void)
{
}

/* The startup code installs this distinct empty entry for vector 0x21. */
/* Function start: 0x4368D0 */
void DosInterruptHandlerHook(void)
{
}

/* Function start: 0x4368E0 */
unsigned short GetNavRangeSentinel(void)
{
    return 0x8000;
}

/* Function start: 0x4368F0 */
unsigned short GetOriginalFreeMemory(void)
{
    return 0x8000;
}

/* Function start: 0x436900 */
void StartupHook(unsigned int (*callback)(unsigned int, short))
{
    (void)callback;
}

/* Function start: 0x436910 */
unsigned int JoystickEdgeHook(int button)
{
    (void)button;
}

/* Function start: 0x436920 */
/* Returns 0. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it follows the joystick edge hook. */
short JoystickStatusHook(void)
{
    return 0;
}

/* Function start: 0x436930 */
/* Empty. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it follows the joystick edge hook. */
void JoystickResetHook(void)
{
}

/* Function start: 0x436940 */
void *AllocateSmallMemoryBlock(short size)
{
    return malloc(size);
}

/* Function start: 0x436950 */
void FreeIfNotNull(void *p)
{
    if (p != 0)
        free(p);
}

/* Function start: 0x436970 */
unsigned int GetStartupErrorCode(int vector)
{
    (void)vector;
    return 0;
}

/* Function start: 0x436980 */
void ShutdownHook(int vector, void *handler)
{
    (void)vector;
    (void)handler;
}

/* Function start: 0x436990 */
/* Empty. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it follows the vector-restoring shutdown hook. */
void InterruptVectorHook(void)
{
}

/* Function start: 0x4369A0 */
unsigned short __stdcall SelectDiskDriveHook(short drive)
{
    return 0;
}

/* Function start: 0x4369B0 */
short GetCurrentDiskDriveHook(void)
{
    return 0;
}

/* Function start: 0x4369C0 */
unsigned short __stdcall GetShutdownErrorCode(unsigned char *driveState)
{
    (void)driveState;
    return 0;
}

/* Function start: 0x4369D0 */
void FinishPacketDecompressionHook(void)
{
}

/* Function start: 0x4369E0 */
void ExitCleanupHook(void)
{
}

/* Function start: 0x4369F0 */
/* Returns 0. Nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it follows the exit cleanup hook. */
short ExitStatusHook(void)
{
    return 0;
}

/* Function start: 0x436A00 */
unsigned int IsVectorWithinRange(FixedVector *vector, short range)
{
    int magnitude = Vector_magnitude(vector);

    if (abs((int)range << 8) >= magnitude)
        return 1;
    return 0;
}

/* Function start: 0x436A30 */
unsigned int shrink_vector(FixedVector *vector)
{
    unsigned int shrinking;

    do {
        shrinking = shrink(&vector->x);
        shrinking |= shrink(&vector->y);
        shrinking |= shrink(&vector->z);
    } while (shrinking != 0);
    return 0;
}

/* Function start: 0x436A70 */
unsigned int shrink(int *component)
{
    int value;
    unsigned int fraction;
    unsigned int integerPart;
    short signedIntegerPart;

    value = *component / 2;
    *component = value;
    fraction = value;
    integerPart = (unsigned int)value >> 16;
    signedIntegerPart = (short)integerPart;
    if (signedIntegerPart == 0)
        return (unsigned short)fraction > 0x0f00;
    if (signedIntegerPart == -1)
        return (unsigned short)fraction < 0xf100;
    if (signedIntegerPart != 0)
        integerPart = 1;
    return integerPart;
}

/* Function start: 0x436AC0 */
short __stdcall GetViewportIntersection(ShortRect *intersection,
                                         const ShortRect *first,
                                         const ShortRect *second)
{
    int pointCount;
    int pointIndex;
    int xCoordinates[16];
    int yCoordinates[16];
    int inside[4];
    int x;
    int insideCount;
    int y;

    pointCount = 0;
    xCoordinates[pointCount] = first->left;
    yCoordinates[pointCount] = first->top;
    pointCount++;
    xCoordinates[pointCount] = first->left;
    yCoordinates[pointCount] = first->bottom;
    pointCount++;
    xCoordinates[pointCount] = first->right;
    yCoordinates[pointCount] = first->top;
    pointCount++;
    xCoordinates[pointCount] = first->right;
    yCoordinates[pointCount] = first->bottom;
    pointCount++;
    xCoordinates[pointCount] = second->left;
    yCoordinates[pointCount] = second->top;
    pointCount++;
    xCoordinates[pointCount] = second->left;
    yCoordinates[pointCount] = second->bottom;
    pointCount++;
    xCoordinates[pointCount] = second->right;
    yCoordinates[pointCount] = second->top;
    pointCount++;
    xCoordinates[pointCount] = second->right;
    yCoordinates[pointCount] = second->bottom;
    pointCount++;
    xCoordinates[pointCount] = first->left;
    yCoordinates[pointCount] = second->top;
    pointCount++;
    xCoordinates[pointCount] = first->left;
    yCoordinates[pointCount] = second->bottom;
    pointCount++;
    xCoordinates[pointCount] = first->right;
    yCoordinates[pointCount] = second->top;
    pointCount++;
    xCoordinates[pointCount] = first->right;
    yCoordinates[pointCount] = second->bottom;
    pointCount++;
    xCoordinates[pointCount] = second->left;
    yCoordinates[pointCount] = first->top;
    pointCount++;
    xCoordinates[pointCount] = second->left;
    yCoordinates[pointCount] = first->bottom;
    pointCount++;
    xCoordinates[pointCount] = second->right;
    yCoordinates[pointCount] = first->top;
    pointCount++;
    xCoordinates[pointCount] = second->right;
    yCoordinates[pointCount] = first->bottom;
    pointCount++;

    /* The original tests the postincrement and discards the comparison.
     * insideCount and inside remain uninitialized when this loop exits. */
    for (pointIndex = 0; pointIndex++; pointIndex < pointCount) {
        x = xCoordinates[pointIndex];
        y = yCoordinates[pointIndex];
        if (first->left <= x && x <= first->right &&
            second->left <= x && x <= second->right &&
            first->top <= y && y <= first->bottom &&
            second->top <= y && y <= second->bottom) {
            if (insideCount == 4)
                return 0;
            inside[insideCount] = 1;
            insideCount++;
        } else {
            if (insideCount == 4)
                return 0;
            inside[insideCount] = 0;
            insideCount++;
        }
    }
    if (insideCount != 4)
        return 0;

    intersection->left = MinShort(
        (short)xCoordinates[inside[0]],
        MinShort((short)xCoordinates[inside[1]],
                 (short)xCoordinates[inside[2]]));
    intersection->top = MinShort(
        (short)yCoordinates[inside[0]],
        MinShort((short)yCoordinates[inside[1]],
                 (short)yCoordinates[inside[2]]));
    intersection->left = MaxShort(
        (short)xCoordinates[inside[0]],
        MaxShort((short)xCoordinates[inside[1]],
                 (short)xCoordinates[inside[2]]));
    intersection->top = MaxShort(
        (short)yCoordinates[inside[0]],
        MaxShort((short)yCoordinates[inside[1]],
                 (short)yCoordinates[inside[2]]));
    return 1;
}

/* Function start: 0x436C70 */
void __stdcall FillGraphicSuffix(char *path, short number, short digits)
{
    char *dot;
    char *suffix;

    suffix = DosStrchr(path, '.');
    dot = suffix;
    suffix++;
    if (dot == 0)
        suffix = path;
    suffix[digits] = '\0';
    ConvertChar_Int(suffix, number, digits);
}

/* Function start: 0x436CB0 */
void __stdcall ConvertChar_Int(char *text, short number, short digits)
{
    while (digits--) {
        text[digits] = (char)(number % 10) + '0';
        number /= 10;
    }
}
