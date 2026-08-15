/*
 *  Event manager, mouse pointer, and frame timer.
 *
 *  Address range 0x4355f0-0x436fff (provisional -- see docs/ORDER.md).
 *  The FM Towns executable preserves the original source name
 *  "source\\eventmgr.c" for this unit.
 */
#include "wc1.h"

/* Function start: 0x4355F0 */
void TranslatePolledInputEvent(unsigned short type, unsigned int value)
{
    unsigned int state;

    switch (type) {
    case 2:
        state = (value >> 2) |
                (g_bHostSecondaryMouseButton_005a899c << 1) |
                g_bHostPrimaryMouseButton_005a8998;
        QueueInputEvent(type,
                        (short)g_nHostMouseMessageX_005a8990,
                        (short)g_nHostMouseMessageY_005a8994,
                        0,
                        g_bHostPrimaryMouseButton_005a8998,
                        g_bHostSecondaryMouseButton_005a899c,
                        state);
        return;
    case 6:
    {
        InputDeviceSample *sample =
            &g_aInputDeviceSamples_005a81f0[g_nActiveInputDevice_005a819c];

        QueueInputEvent(type, (short)sample->x, (short)sample->y,
                        0, 0, 0, 0);
        return;
    }
    case 13:
        QueueInputEvent(type,
                        (short)g_nHostMouseMessageX_005a8990,
                        (short)g_nHostMouseMessageY_005a8994,
                        0, 0, 0, 0);
        return;
    }
}

/* Function start: 0x4356A0 */
void QueueInputEventAtCursor(unsigned int type, short primaryButton,
                             short secondaryButton)
{
    /* Preserve the original 16-bit event ID and sample each volatile axis. */
    unsigned int eventType = type & 0xffff;
    int x = g_stMouseCursorState_0059ab10.x;
    int y = g_stMouseCursorState_0059ab10.y;

    QueueInputEvent((unsigned short)eventType, (unsigned short)x,
                    (unsigned short)y, 0,
                    primaryButton, secondaryButton, 0);
}

/* Function start: 0x4356E0 */
InputEvent *AllocateInputEvent(void)
{
    int *used;
    int index;

    if (g_bInputEventPoolInitialized_0046da98 != 0) {
        index = 0;
        used = g_aiInputEventSlotUsed_0059ab70;
        do {
            if (*used == 0) {
                g_aiInputEventSlotUsed_0059ab70[index] = 1;
                return &g_aInputEventPool_00598c40[index];
            }
            used++;
            index++;
        } while (used < &g_aiInputEventSlotUsed_0059ab70[0x100]);
        return 0;
    }
    memset(g_aInputEventPool_00598c40, 0,
           sizeof(g_aInputEventPool_00598c40));
    memset(g_aiInputEventSlotUsed_0059ab70, 0,
           sizeof(g_aiInputEventSlotUsed_0059ab70));
    g_bInputEventPoolInitialized_0046da98 = 1;
    g_aiInputEventSlotUsed_0059ab70[0] = 1;
    return &g_aInputEventPool_00598c40[0];
}

/* Function start: 0x435760 */
void ReleaseInputEvent(InputEvent *event)
{
    int slot;

    for (slot = 0; slot < 0x100; slot++) {
        if (&g_aInputEventPool_00598c40[slot] == event)
            g_aiInputEventSlotUsed_0059ab70[slot] = 0;
    }
}

/* Function start: 0x435790 */
void QueueInputEvent(unsigned short type, unsigned short x,
                     unsigned short y, unsigned short value,
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

    if (g_pInputEventHead_0046da90 == 0) {
        g_pInputEventHead_0046da90 = AllocateInputEvent();
        if (g_pInputEventHead_0046da90 == 0) {
            ReleaseInputEventQueue();
            return;
        }
        g_pInputEventTail_0046da94 = g_pInputEventHead_0046da90;
        g_pInputEventHead_0046da90->next = 0;
        g_pInputEventTail_0046da94->previous = 0;
    } else {
        event = AllocateInputEvent();
        g_pInputEventTail_0046da94->next = event;
        if (g_pInputEventTail_0046da94->next == 0) {
            ReleaseInputEventQueue();
            return;
        }
        g_pInputEventTail_0046da94->next->previous =
            g_pInputEventTail_0046da94;
        g_pInputEventTail_0046da94 = g_pInputEventTail_0046da94->next;
        g_pInputEventTail_0046da94->next = 0;
    }
    g_pInputEventTail_0046da94->type = type;
    g_pInputEventTail_0046da94->modifiers = modifiers;
    g_pInputEventTail_0046da94->x = x;
    g_pInputEventTail_0046da94->y = y;
    g_pInputEventTail_0046da94->value = value;
    g_pInputEventTail_0046da94->primaryButton = (short)primaryButton;
    g_pInputEventTail_0046da94->secondaryButton = (short)secondaryButton;
}

/* Function start: 0x4358B0 */
void ReleaseInputEventQueue(void)
{
    InputEvent *event = g_pInputEventHead_0046da90;

    while (event != 0) {
        InputEvent *next = event->next;

        ReleaseInputEvent(event);
        event = next;
    }
    g_pInputEventTail_0046da94 = 0;
    g_pInputEventHead_0046da90 = 0;
}

/* Function start: 0x4358E0 */
void RetainInputEventsOfType(int type)
{
    InputEvent *event = g_pInputEventHead_0046da90;

    while (event != 0) {
        InputEvent *next = event->next;

        if (event->type != type) {
            if (event->previous != 0)
                event->previous->next = next;
            else
                g_pInputEventHead_0046da90 = next;
            if (event->next != 0)
                event->next->previous = event->previous;
            else
                g_pInputEventTail_0046da94 = event->previous;
            ReleaseInputEvent(event);
        }
        event = next;
    }
}

/* Function start: 0x435940 */
void RemoveInputEvent(InputEvent *event)
{
    InputEvent *previous = event->previous;
    InputEvent *next = event->next;

    if (previous != 0) {
        if (next != 0) {
            previous->next = next;
            event->next->previous = event->previous;
            ReleaseInputEvent(event);
            return;
        }
        previous->next = 0;
        g_pInputEventTail_0046da94 = event->previous;
        ReleaseInputEvent(event);
        return;
    }
    if (next != 0) {
        g_pInputEventHead_0046da90 = next;
        event->next->previous = 0;
        ReleaseInputEvent(event);
        return;
    }
    g_pInputEventTail_0046da94 = 0;
    g_pInputEventHead_0046da90 = 0;
    ReleaseInputEvent(event);
}

/* Function start: 0x4359C0 */
short __stdcall GetNextInputEvent(InputEventState *state)
{
    short *eventX;
    int eventY;
    int type;

    type = 0;
    if (g_pInputEventHead_0046da90 != 0) {
        eventX = &g_pInputEventHead_0046da90->x;
        eventY = (int)g_pInputEventHead_0046da90->y;
        if ((int)g_stMouseCursorState_0059ab10.viewport->left > (int)*eventX)
            *eventX = g_stMouseCursorState_0059ab10.viewport->left;
        else if ((int)g_stMouseCursorState_0059ab10.viewport->right < (int)*eventX)
            *eventX = g_stMouseCursorState_0059ab10.viewport->right;
        if ((int)g_stMouseCursorState_0059ab10.viewport->top > eventY)
            g_pInputEventHead_0046da90->y = g_stMouseCursorState_0059ab10.viewport->top;
        else if ((int)g_stMouseCursorState_0059ab10.viewport->bottom <
                 eventY)
            g_pInputEventHead_0046da90->y =
                g_stMouseCursorState_0059ab10.viewport->bottom;

        state->modifiers =
            (short)g_pInputEventHead_0046da90->modifiers;
        switch (g_pInputEventHead_0046da90->type) {
        case 1:
            g_stMouseCursorState_0059ab10.x = g_pInputEventHead_0046da90->x;
            g_stMouseCursorState_0059ab10.y = g_pInputEventHead_0046da90->y;
            g_stMouseCursorState_0059ab10.primaryButton = 0;
            state->x = g_pInputEventHead_0046da90->x;
            state->y = g_pInputEventHead_0046da90->y;
            type = 1;
            break;
        case 2:
            g_stMouseCursorState_0059ab10.x = g_pInputEventHead_0046da90->x;
            g_stMouseCursorState_0059ab10.y = g_pInputEventHead_0046da90->y;
            g_stMouseCursorState_0059ab10.primaryButton =
                (unsigned char)g_pInputEventHead_0046da90->primaryButton;
            g_stMouseCursorState_0059ab10.secondaryButton =
                (unsigned char)g_pInputEventHead_0046da90->secondaryButton;
            state->x = g_pInputEventHead_0046da90->x;
            state->y = g_pInputEventHead_0046da90->y;
            type = 2;
            state->value =
                (int)g_pInputEventHead_0046da90->secondaryButton * 2 |
                (int)g_pInputEventHead_0046da90->primaryButton;
            break;
        case 3:
            type = 3;
            state->value = g_pInputEventHead_0046da90->value;
            state->x = g_stMouseCursorState_0059ab10.x;
            state->y = g_stMouseCursorState_0059ab10.y;
            break;
        case 4:
            type = 4;
            state->x = g_pInputEventHead_0046da90->value;
            state->x = g_stMouseCursorState_0059ab10.x;
            state->y = g_stMouseCursorState_0059ab10.y;
            break;
        case 5:
            type = 5;
            state->x = g_pInputEventHead_0046da90->value;
            break;
        case 6:
            type = 6;
            state->x = g_pInputEventHead_0046da90->x;
            state->y = g_pInputEventHead_0046da90->y;
            break;
        case 7:
            type = 7;
            state->x = g_pInputEventHead_0046da90->x;
            state->y = g_pInputEventHead_0046da90->y;
            break;
        case 8:
            type = 8;
            state->x = g_pInputEventHead_0046da90->x;
            state->y = g_pInputEventHead_0046da90->y;
            break;
        case 9:
            type = 9;
            state->x = g_pInputEventHead_0046da90->x;
            state->y = g_pInputEventHead_0046da90->y;
            break;
        case 10:
            type = 10;
            state->x = g_pInputEventHead_0046da90->x;
            state->y = g_pInputEventHead_0046da90->y;
            break;
        case 13:
            g_stMouseCursorState_0059ab10.x = g_pInputEventHead_0046da90->x;
            g_stMouseCursorState_0059ab10.y = g_pInputEventHead_0046da90->y;
            state->x = g_pInputEventHead_0046da90->x;
            state->y = g_pInputEventHead_0046da90->y;
            type = 13;
            break;
        }
        RemoveInputEvent(g_pInputEventHead_0046da90);
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

    event = g_pInputEventHead_0046da90;
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
    InputEvent *event = g_pInputEventHead_0046da90;

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
    DAT_0046daa0 = 0;
    return 1;
}

/* Function start: 0x435DE0 */
void CheckCursor(void)
{
}

/* Function start: 0x435E20 */
void CaptureMouseCursorBackground(void)
{
    int x;
    int y;

    if (DAT_0046daa0 == 0 ||
        g_stMouseCursorState_0059ab10.viewport == 0 ||
        g_stMouseCursorState_0059ab10.shape == 0)
        return;

    CaptureSpriteBackground(g_stMouseCursorState_0059ab10.viewport,
                            DAT_004865a8,
                            g_stMouseCursorState_0059ab10.x,
                            g_stMouseCursorState_0059ab10.y,
                            g_stMouseCursorState_0059ab10.shape,
                            g_stMouseCursorState_0059ab10.frame);
    x = g_stMouseCursorState_0059ab10.x;
    y = g_stMouseCursorState_0059ab10.y;
    if (DAT_0059ab5c > x - 16)
        DAT_0059ab5c = x - 16;
    DAT_0059a8e4 = x;
    if (DAT_0059ab44 < x + 16)
        DAT_0059ab44 = x + 16;
    if (DAT_0059ab60 > y - 16)
        DAT_0059ab60 = y - 16;
    DAT_0059a8e0 = y;
    if (DAT_0059ab48 < y + 16)
        DAT_0059ab48 = y + 16;
    DAT_0059ab40 = 1;
    DAT_0059a84c = 1;
}

/* Function start: 0x435EF0 */
void DrawMouseCursor(void)
{
    int x;
    int y;

    if (DAT_0046daa0 == 0 ||
        g_stMouseCursorState_0059ab10.viewport == 0 ||
        g_stMouseCursorState_0059ab10.shape == 0)
        return;

    DrawSpriteDefault(g_stMouseCursorState_0059ab10.viewport,
                      g_stMouseCursorState_0059ab10.x,
                      g_stMouseCursorState_0059ab10.y,
                      g_stMouseCursorState_0059ab10.shape,
                      g_stMouseCursorState_0059ab10.frame);
    x = g_stMouseCursorState_0059ab10.x;
    if (DAT_0059ab5c > x - 16)
        DAT_0059ab5c = x - 16;
    if (DAT_0059ab44 < x + 16)
        DAT_0059ab44 = x + 16;
    y = g_stMouseCursorState_0059ab10.y;
    if (DAT_0059ab60 > y - 16)
        DAT_0059ab60 = y - 16;
    if (DAT_0059ab48 < y + 16)
        DAT_0059ab48 = y + 16;
    DAT_0059ab40 = 1;
}

/* Function start: 0x435FA0 */
void RestoreMouseCursorBackground(void)
{
    int x;
    int y;

    if (DAT_0046daa0 == 0 ||
        g_stMouseCursorState_0059ab10.viewport == 0 ||
        g_stMouseCursorState_0059ab10.shape == 0 ||
        DAT_0059a84c == 0)
        return;

    RestoreSpriteBackground(g_stMouseCursorState_0059ab10.viewport,
                            DAT_004865a8,
                            (short)DAT_0059a8e4, (short)DAT_0059a8e0,
                            g_stMouseCursorState_0059ab10.shape,
                            g_stMouseCursorState_0059ab10.frame);
    x = DAT_0059a8e4;
    if (DAT_0059ab5c > x - 16)
        DAT_0059ab5c = x - 16;
    if (DAT_0059ab44 < x + 16)
        DAT_0059ab44 = x + 16;
    y = DAT_0059a8e0;
    if (DAT_0059ab60 > y - 16)
        DAT_0059ab60 = y - 16;
    if (DAT_0059ab48 < y + 16)
        DAT_0059ab48 = y + 16;
    DAT_0059a84c = 0;
}

/* Function start: 0x436060 */
void RefreshMouseCursorDisplay(void)
{
    DAT_0059ab5c = 319;
    DAT_0059ab60 = 199;
    DAT_0059ab44 = 0;
    DAT_0059ab48 = 0;
    DAT_0059ab40 = 0;
    DAT_0059a84c = 0;
    CaptureMouseCursorBackground();
    DrawMouseCursor();
    DIBupdate(DAT_0059ab5c, DAT_0059ab60,
              DAT_0059ab44, DAT_0059ab48);
    RestoreMouseCursorBackground();
}

/* Function start: 0x4360D0 */
void EnterAllocationScope(void)
{
    DAT_0046daa0 = DAT_0046daa0 + 1;
}

/* Function start: 0x4360E0 */
void LeaveAllocationScope(void)
{
    DAT_0046daa0 = DAT_0046daa0 - 1;
}

/* Function start: 0x4360F0 */
void __stdcall SetMouseCursorShape(unsigned char *shape, short frame)
{
    g_stMouseCursorState_0059ab10.shapeChanged = 1;
    if (g_stMouseCursorState_0059ab10.viewport != 0 && DAT_0046daa0 > 0 &&
        g_pDrawnMouseCursorShape_0046da9c != 0) {
        RestoreSpriteBackground(g_stMouseCursorState_0059ab10.viewport, DAT_004865a8,
                                (short)g_nMouseCursorSavedX_0059a844,
                                (short)g_nMouseCursorSavedY_0059a840,
                                g_pDrawnMouseCursorShape_0046da9c,
                                (short)g_stMouseCursorState_0059ab10.frame);
        g_pDrawnMouseCursorShape_0046da9c = 0;
    }
    g_stMouseCursorState_0059ab10.frame = frame;
    g_stMouseCursorState_0059ab10.shape = shape;
}

/* Function start: 0x436160 */
void __stdcall SetMouseHomePosition(short x, short y)
{
    g_stMouseCursorState_0059ab10.x = x;
    g_stMouseCursorState_0059ab10.y = y;
    SetMousePositionDuplicate(x, y);
}

/* Function start: 0x436190 */
void __stdcall ApplyPackedMousePosition(ShortPoint point)
{
    g_stMouseCursorState_0059ab10.x = point.x;
    g_stMouseCursorState_0059ab10.y = point.y;
    SetMousePositionDuplicate(point.x, point.y);
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
    while (DAT_0059ab3c != 0) ;
}

/* Function start: 0x436240 */
int IsFrameTickElapsed(void)
{
    return DAT_0059ab3c == 0;
}

/* Function start: 0x436260 */
unsigned short GetSoundHardwareFlag(void)
{
    return 1;
}

/* Function start: 0x4362A0 */
void TimerResetHook(void)
{
}

/* Function start: 0x4362B0 */
unsigned int GetVideoReleaseResult(void)
{
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

/* Function start: 0x4363A0 */
void ClearInputKeyStatePreservingModifiers(void)
{
    unsigned int control = g_abInputKeyState_0059a860[0x1d];
    unsigned int alt = g_abInputKeyState_0059a860[0x38];

    memset(g_abInputKeyState_0059a860, 0,
           sizeof(g_abInputKeyState_0059a860));
    g_abInputKeyState_0059a860[0x1d] = control;
    g_abInputKeyState_0059a860[0x38] = alt;
    ClearDebugPauseFlags();
}

/* Function start: 0x4363E0 */
void ClearInputKeyState(void)
{
    memset(g_abInputKeyState_0059a860, 0,
           sizeof(g_abInputKeyState_0059a860));
    ClearDebugPauseFlags();
}

/* Function start: 0x436420 */
void SetInputKeyState(int scanCode, unsigned char pressed)
{
    if (scanCode >= 0 && scanCode < 0x80) {
        g_abInputKeyState_0059a860[scanCode] = pressed;
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
    unsigned int distance;
    int previous;
    int best;
    int bestObject;
    int obj;
    int screenOffset;
    int *placed;
    int *sortedEntry;
    int sorted;

    previous = -999999999;
    bestObject = -1;
    memset(g_anObjectDepthPlaced_0059a8f0, 0,
           sizeof(g_anObjectDepthPlaced_0059a8f0));
    obj = 0;
    do {
        distance = (unsigned short)g_asObjectDistance_0059b4a0[obj];
        if (previous < (int)distance) {
            previous = (int)distance;
            bestObject = obj;
        }
        obj++;
    } while (obj < WC1_SPACE_OBJECT_COUNT);
    sorted = 0;
    sortedEntry = g_anSortedObject_0059aa00;
    do {
        best = -1;
        *sortedEntry = bestObject;
        if (bestObject == -1)
            return;
        screenOffset = 0;
        obj = 0;
        g_anObjectDepthPlaced_0059a8f0[bestObject] = 1;
        bestObject = -1;
        placed = g_anObjectDepthPlaced_0059a8f0;
        do {
            if (*placed == 0 &&
                *(short *)((unsigned char *)g_asObjectScreenX_0059d9b0 +
                           screenOffset) != (short)0x8001) {
                distance = *(unsigned short *)(
                    (unsigned char *)g_asObjectDistance_0059b4a0 +
                    screenOffset);
                if (best < (int)distance && previous >= (int)distance) {
                    bestObject = obj;
                    best = (int)distance;
                }
            }
            screenOffset += sizeof(short);
            placed++;
            obj++;
        } while (placed < g_anObjectDepthPlaced_0059a8f0 +
                            WC1_SPACE_OBJECT_COUNT);
        sortedEntry++;
        sorted++;
    } while (sorted < WC1_SPACE_OBJECT_COUNT);
}

/* Function start: 0x436520 */
void draw_sorted_objects_to_buffer(void)
{
    int *sortedEntry;
    int obj;
    int objectClass;
    unsigned char *shape;
    short screenX;
    short screenY;
    int specialObject;
#ifdef WC1_SDL
    float enhancedScreenX;
    float enhancedScreenY;
    short projectedScreenX;
    short projectedScreenY;
#endif

    sortedEntry = g_anSortedObject_0059aa00;
    do {
        obj = *sortedEntry;
        if (obj < 0)
            return;
        if ((int)g_aeObjectType_0059b560[obj] < 0)
            return;
        objectClass = g_aeObjectClass_0059d100[obj];
#ifdef WC1_SDL
        enhancedScreenX = (float)(short)(
            g_asObjectScreenX_0059d9b0[obj] + g_nViewCenterX_0059a852);
        enhancedScreenY = (float)(short)(
            g_asObjectScreenY_0059d930[obj] + g_nViewCenterY_0059a854);
        if (objectClass != OBJECT_CLASS_NULL &&
            objectClass != OBJECT_CLASS_FIXED_OBJECT &&
            obj != DAT_00469208 &&
            g_aObjectViewPosition_0059afa0[obj].z != 0) {
            projectedScreenX = (short)(DivideFixed(
                MultiplyFixed(
                    (short)(g_nScreenWidth_0046daa4 & ~1) << 7,
                    g_aObjectViewPosition_0059afa0[obj].x),
                g_aObjectViewPosition_0059afa0[obj].z) >> 8);
            projectedScreenY = (short)(DivideFixed(
                MultiplyFixed(
                    (short)(g_nScreenWidth_0046daa4 & ~1) << 7,
                    g_aObjectViewPosition_0059afa0[obj].y),
                g_aObjectViewPosition_0059afa0[obj].z) >> 8);
            if (projectedScreenX == g_asObjectScreenX_0059d9b0[obj] &&
                projectedScreenY == g_asObjectScreenY_0059d930[obj]) {
                enhancedScreenX =
                    (float)g_nViewCenterX_0059a852 +
                    (float)(((double)(g_nScreenWidth_0046daa4 & ~1) * 0.5 *
                             g_aObjectViewPosition_0059afa0[obj].x) /
                            g_aObjectViewPosition_0059afa0[obj].z);
                enhancedScreenY =
                    (float)g_nViewCenterY_0059a854 +
                    (float)(((double)(g_nScreenWidth_0046daa4 & ~1) * 0.5 *
                             g_aObjectViewPosition_0059afa0[obj].y) /
                            g_aObjectViewPosition_0059afa0[obj].z);
            }
        }
#endif
        if (objectClass != OBJECT_CLASS_NULL) {
            switch (objectClass) {
            default:
                screenY = g_asObjectScreenY_0059d930[obj];
                screenX = (short)(g_asObjectScreenX_0059d9b0[obj] +
                                  g_nViewCenterX_0059a852);
                shape = g_apObjectShape_0059d2f0[obj];
                g_asObjectDrawX_0059d000[obj] = screenX;
                screenY = (short)(screenY + g_nViewCenterY_0059a854);
                g_asObjectDrawY_0059cf80[obj] = screenY;
                if (shape != 0) {
#ifdef WC1_SDL
                    if (!Wc1SdlRecordSpaceSprite(
                            &DAT_005a7510, enhancedScreenX, enhancedScreenY,
                            shape,
                            g_asObjectViewFrame_0059d230[obj],
                            g_asObjectScreenAngle_0059cd90[obj],
                            g_asObjectScreenScale_0059c950[obj],
                            g_asObjectFlip_0059c870[obj]))
#endif
                    DrawSpriteScaled(
                        &DAT_005a7510, screenX, screenY, shape,
                        g_asObjectViewFrame_0059d230[obj],
                        g_asObjectScreenAngle_0059cd90[obj],
                        g_asObjectScreenScale_0059c950[obj],
                        g_asObjectFlip_0059c870[obj]);
                }
                break;
            case OBJECT_CLASS_STAR:
#ifdef WC1_SDL
                /* WCDX fix: planets use the per-object scaled path above. */
#else
            case OBJECT_CLASS_PLANET:
#endif
            case OBJECT_CLASS_DUST:
                specialObject = (int)DAT_00469208;
                screenY = g_asObjectScreenY_0059d930[obj];
                screenX = (short)(g_asObjectScreenX_0059d9b0[obj] +
                                  g_nViewCenterX_0059a852);
                g_asObjectDrawX_0059d000[obj] = screenX;
                screenY = (short)(screenY + g_nViewCenterY_0059a854);
                g_asObjectDrawY_0059cf80[obj] = screenY;
                if (specialObject == obj)
                    shape = g_apObjectShape_0059d2f0[obj];
                else
                    shape = g_pConstellationShape_005a765c;
#ifdef WC1_SDL
                if (!Wc1SdlRecordSpaceSprite(
                        &DAT_005a7510, enhancedScreenX, enhancedScreenY, shape,
                        g_asObjectViewFrame_0059d230[obj], 0, 0x100, 0))
#endif
                DrawSpriteDefault(&DAT_005a7510, screenX, screenY, shape,
                                  g_asObjectViewFrame_0059d230[obj]);
                break;
            }
        }
        sortedEntry++;
    } while (sortedEntry < g_anSortedObject_0059aa00 +
                           WC1_SPACE_OBJECT_COUNT);
}

/* Function start: 0x436650 */
void intro_drawbackgroundships(void)
{
    unsigned char *shape;
    int objectClass;
    int obj;
    int dwordOffset;
    int shortOffset;
    int zero;

    obj = 0;
    shortOffset = 0;
    zero = 0;
    dwordOffset = 0;
    do {
        if (*(enum ObjectType *)((unsigned char *)g_aeObjectType_0059b560 +
                                 dwordOffset) < zero)
            return;
        objectClass = *(enum ObjectClass *)(
            (unsigned char *)g_aeObjectClass_0059d100 + dwordOffset);
        if (objectClass != OBJECT_CLASS_NULL) {
            switch (objectClass) {
            default:
#ifdef WC1_SDL
                shape = g_apObjectShape_0059d2f0[obj];
#else
                shape = *(unsigned char **)(
                    (unsigned char *)g_apObjectShape_0059d2f0 +
                    dwordOffset);
#endif
                if (shape != 0) {
                    DrawSolidColourSpriteScaled(
                        &DAT_005a7510,
                        *(short *)((unsigned char *)g_asObjectDrawX_0059d000 +
                                   shortOffset),
                        *(short *)((unsigned char *)g_asObjectDrawY_0059cf80 +
                                   shortOffset),
                        shape,
                        *(short *)((unsigned char *)g_asObjectViewFrame_0059d230 +
                                   shortOffset),
                        *(short *)((unsigned char *)g_asObjectScreenAngle_0059cd90 +
                                   shortOffset),
                        *(short *)((unsigned char *)g_asObjectScreenScale_0059c950 +
                                   shortOffset),
                        *(short *)((unsigned char *)g_asObjectFlip_0059c870 +
                                   shortOffset),
                        DAT_004699d8);
                }
                break;
            case OBJECT_CLASS_STAR:
#ifdef WC1_SDL
                /* Erase planets with the same scaled geometry used to draw. */
#else
            case OBJECT_CLASS_PLANET:
#endif
            case OBJECT_CLASS_DUST:
                if (obj == DAT_00469208)
#ifdef WC1_SDL
                    shape = g_apObjectShape_0059d2f0[obj];
#else
                    shape = *(unsigned char **)(
                        (unsigned char *)g_apObjectShape_0059d2f0 +
                        dwordOffset);
#endif
                else
                    shape = g_pConstellationShape_005a765c;
                DrawSolidColourSprite(
                    &DAT_005a7510,
                    *(short *)((unsigned char *)g_asObjectDrawX_0059d000 +
                               shortOffset),
                    *(short *)((unsigned char *)g_asObjectDrawY_0059cf80 +
                               shortOffset),
                    shape,
                    *(short *)((unsigned char *)g_asObjectViewFrame_0059d230 +
                               shortOffset),
                    DAT_004699d8);
                break;
            }
        }
        shortOffset += sizeof(short);
        dwordOffset += sizeof(int);
        obj++;
    } while (dwordOffset < WC1_SPACE_OBJECT_COUNT * (int)sizeof(int));
}

/* Function start: 0x436740 */
void set_up_screen_viewport(signed char mode)
{
    int modeIndex;
    short viewportHeight;
    short viewportWidth;
    const ScreenViewportGeometry *viewportGeometry;

    g_cScreenViewportMode_0059a9f2 = mode;
    modeIndex = (int)mode;
    if (modeIndex >= 4) {
        if (modeIndex <= 5)
            goto static_geometry;
    }

    g_pScreenViewportGeometry_0059a9f4 =
        (const ScreenViewportGeometry *)(
            (const unsigned char *)g_pScreenViewportPacket_005a6b94 +
            g_pScreenViewportPacket_005a6b94
                ->geometryOffsets[modeIndex]);
    goto geometry_ready;

static_geometry:
    g_pScreenViewportGeometry_0059a9f4 =
        &g_aScreenViewportGeometry_0046dab8[modeIndex];

geometry_ready:

    if (DAT_0046a008 != 0 && DAT_0046a008 != -2) {
        viewportWidth = g_pScreenViewportGeometry_0059a9f4->width;
        viewportGeometry = g_pScreenViewportGeometry_0059a9f4;
        *(short *)&g_nScreenWidth_0046daa4 = viewportWidth;
        g_nViewCenterX_0059a852 = (short)(viewportWidth / 2);
        viewportHeight = viewportGeometry->height;
        g_nViewCenterY_0059a854 = (short)(viewportHeight / 2);
        *(short *)&g_nScreenHeight_0046daa8 = viewportHeight;
        g_nViewportOriginX_0059ab52 = viewportGeometry->originX;
        g_nViewportOriginY_0059ab50 = viewportGeometry->originY;
        switch ((int)g_cCockpitView_0059dab0) {
        case 0:
            g_nViewportOriginY_0059ab50 += 10;
            g_nViewCenterY_0059a854 += 10;
            break;
        case 1:
            g_nViewportOriginY_0059ab50 += 25;
            g_nViewCenterY_0059a854 += 25;
            break;
        case 2:
            g_nViewportOriginY_0059ab50 += 50;
            g_nViewCenterY_0059a854 += 50;
            break;
        }
        *(short *)&g_nScreenWidth_0046daa4 = 320;
        *(short *)&g_nScreenHeight_0046daa8 = 200;
        return;
    }

    viewportWidth = g_pScreenViewportGeometry_0059a9f4->width;
    viewportGeometry = g_pScreenViewportGeometry_0059a9f4;
    *(short *)&g_nScreenWidth_0046daa4 = viewportWidth;
    g_nViewCenterX_0059a852 = (short)(viewportWidth / 2);
    viewportHeight = viewportGeometry->height;
    g_nViewCenterY_0059a854 = (short)(viewportHeight / 2);
    *(short *)&g_nScreenHeight_0046daa8 = viewportHeight;
    g_nViewportOriginX_0059ab52 = viewportGeometry->originX;
    g_nViewportOriginY_0059ab50 = viewportGeometry->originY;
}

/* Function start: 0x4368C0 */
void MouseIdleHook(void)
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
void VideoReleaseHook(void)
{
}

/* Function start: 0x4369E0 */
void ExitCleanupHook(void)
{
}

/* Function start: 0x436A00 */
unsigned int IsVectorWithinRange(FixedVector *vector, short range)
{
    int magnitude = Vector_magnitude(vector);
    int fixedRange = abs((int)range << 8);

    if (fixedRange >= magnitude)
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
    unsigned short fraction;
    unsigned int integerPart;
    short signedIntegerPart;

    value = *component / 2;
    *component = value;
    fraction = (unsigned short)value;
    integerPart = (unsigned int)value >> 16;
    signedIntegerPart = (short)integerPart;
    if (signedIntegerPart == 0)
        return fraction > 0x0f00;
    if (signedIntegerPart == -1)
        return fraction < 0xf100;
    if (signedIntegerPart != 0)
        integerPart = 1;
    return integerPart;
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
