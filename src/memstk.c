/*
 *  Tracked-allocation scopes and the frame timer.
 *
 *  Address range 0x435600-0x436fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: the DAT_0046da90 node list with its DAT_0046daa0 depth counter.
 */
#include "wc1.h"

/* Function start: 0x435760 */
void ClearEventSlotByAddress(void *event)
{
    int *used = g_aiInputEventSlotUsed_0059ab70;
    int offset = 0;

    do {
        if (&g_aInputEventSlots_00598c40[offset] == event)
            *used = 0;
        used = used + 1;
        offset = offset + 0x1c;
    } while (offset < 0x1c00);
}

/* Function start: 0x435CC0 */
void PumpMessagesAndDispatch(int a)
{
    PumpWindowMessages();
    DispatchPendingEvents(a);
}

/* Function start: 0x435D80 */
short __stdcall IsInputEventQueued(short code)
{
    int event = DAT_0046da90;

    while (event != 0 && *(short *)event != code)
        event = *(int *)(event + 0x14);
    return event != 0;
}

/* Function start: 0x435DB0 */
void FreeAllTrackedAllocations(void)
{
    int node = DAT_0046da90;

    while (node != 0) {
        int next = *(int *)(node + 0x14);
        ClearEventSlotByAddress((void *)node);
        node = next;
    }
    DAT_0046da94 = 0;
    DAT_0046da90 = 0;
}

/* Function start: 0x435DC0 */
unsigned int ResetAllocationDepth(void)
{
    DAT_0046daa0 = 0;
    return 1;
}

/* Function start: 0x435E20 */
void CaptureMouseCursorBackground(void)
{
    int x;
    int y;

    if (DAT_0046daa0 == 0 || DAT_0059ab23 == 0 || DAT_0059ab19 == 0)
        return;

    CaptureSpriteBackground(DAT_0059ab23, DAT_004865a8,
                            DAT_0059ab10, DAT_0059ab12,
                            DAT_0059ab19, DAT_0059ab1d);
    x = DAT_0059ab10;
    y = DAT_0059ab12;
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

    if (DAT_0046daa0 == 0 || DAT_0059ab23 == 0 || DAT_0059ab19 == 0)
        return;

    DrawSpriteDefault(DAT_0059ab23, DAT_0059ab10, DAT_0059ab12,
                      DAT_0059ab19, DAT_0059ab1d);
    x = DAT_0059ab10;
    if (DAT_0059ab5c > x - 16)
        DAT_0059ab5c = x - 16;
    if (DAT_0059ab44 < x + 16)
        DAT_0059ab44 = x + 16;
    y = DAT_0059ab12;
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

    if (DAT_0046daa0 == 0 || DAT_0059ab23 == 0 || DAT_0059ab19 == 0 ||
        DAT_0059a84c == 0)
        return;

    RestoreSpriteBackground(DAT_0059ab23, DAT_004865a8,
                            (short)DAT_0059a8e4, (short)DAT_0059a8e0,
                            DAT_0059ab19, DAT_0059ab1d);
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
    g_bMouseCursorShapeChanged_0059ab2b = 1;
    if (DAT_0059ab23 != 0 && DAT_0046daa0 > 0 &&
        g_pDrawnMouseCursorShape_0046da9c != 0) {
        RestoreSpriteBackground(DAT_0059ab23, DAT_004865a8,
                                g_nMouseCursorSavedX_0059a844,
                                g_nMouseCursorSavedY_0059a840,
                                g_pDrawnMouseCursorShape_0046da9c,
                                DAT_0059ab1d);
        g_pDrawnMouseCursorShape_0046da9c = 0;
    }
    DAT_0059ab1d = frame;
    DAT_0059ab19 = shape;
}

/* Function start: 0x436160 */
void SetMouseHomePosition(short x, short y)
{
    DAT_0059ab10 = x;
    DAT_0059ab12 = y;
}

/* Function start: 0x4361E0 */
void SetFrameTimerPeriod(short period)
{
    SetMultimediaTimerCallback((int)period);
}

/* Function start: 0x4361F0 */
void SetFrameTimerAndWait(short period)
{
    SetFrameTimerPeriod(period);
    WaitForFrameTick();
}

/* Function start: 0x436210 */
void SetFrameTimerPeriodDirect(short p)
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

/* Function start: 0x436460 */
void sort_object_depth(void)
{
    unsigned char used[WC1_SPACE_OBJECT_COUNT];
    unsigned short farthestDistance;
    short farthestObject;
    short sorted;
    short obj;

    memset(used, 0, sizeof(used));
    sorted = 0;
    while (sorted < WC1_SPACE_OBJECT_COUNT) {
        farthestDistance = 0;
        farthestObject = -1;
        obj = 0;
        while (obj < WC1_SPACE_OBJECT_COUNT) {
            if (used[obj] == 0 &&
                g_asObjectScreenX_0059d9b0[obj] != (short)0x8001 &&
                (farthestObject == -1 ||
                 (unsigned short)g_asObjectDistance_0059b4a0[obj] >
                     farthestDistance)) {
                farthestDistance =
                    (unsigned short)g_asObjectDistance_0059b4a0[obj];
                farthestObject = obj;
            }
            obj++;
        }
        g_anSortedObject_0059aa00[sorted] = farthestObject;
        if (farthestObject == -1)
            return;
        used[farthestObject] = 1;
        sorted++;
    }
}

/* Function start: 0x436520 */
void draw_sorted_objects_to_buffer(void)
{
    int obj;
    short screenX;
    short screenY;
    short sorted;

    sorted = 0;
    while (sorted < WC1_SPACE_OBJECT_COUNT) {
        obj = g_anSortedObject_0059aa00[sorted];
        if (obj < 0)
            return;
        if (g_aeObjectClass_0059d100[obj] != OBJECT_CLASS_NULL &&
            (int)g_aeObjectType_0059b560[obj] >= 0) {
            screenX = (short)(g_asObjectScreenX_0059d9b0[obj] +
                (DAT_005a7510.left + DAT_005a7510.right + 1) / 2);
            screenY = (short)(g_asObjectScreenY_0059d930[obj] +
                (DAT_005a7510.top + DAT_005a7510.bottom + 1) / 2);
            if (g_aeObjectClass_0059d100[obj] >= OBJECT_CLASS_STAR &&
                g_aeObjectClass_0059d100[obj] <= OBJECT_CLASS_DUST) {
                DrawSpriteDefault(&DAT_005a7510, screenX, screenY,
                                  g_pConstellationShape_005a765c,
                                  g_asObjectViewFrame_0059d230[obj]);
            } else if (g_apObjectShape_0059d2f0[obj] != 0) {
                DrawSpriteTransformed(&DAT_005a7510, screenX, screenY,
                                      g_apObjectShape_0059d2f0[obj],
                                      g_asObjectViewFrame_0059d230[obj],
                                      g_asObjectScreenAngle_0059cd90[obj],
                                      g_asObjectScreenScale_0059c950[obj],
                                      g_asObjectScreenScale_0059c950[obj],
                                      g_asObjectFlip_0059c870[obj], 0);
            }
        }
        sorted++;
    }
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
void StartupHook(void)
{
}

/* Function start: 0x436910 */
unsigned int JoystickEdgeHook(int button)
{
    (void)button;
}

/* Function start: 0x436950 */
void FreeIfNotNull(void *p)
{
    if (p != 0) _free_dbg(p, 1);
}

/* Function start: 0x436970 */
unsigned int GetStartupErrorCode(void)
{
    return 0;
}

/* Function start: 0x436980 */
void ShutdownHook(void)
{
}

/* Function start: 0x4369A0 */
unsigned short GetDiskRetryCount(void)
{
    return 0;
}

/* Function start: 0x4369B0 */
unsigned short GetDiskChangeCount(void)
{
    return 0;
}

/* Function start: 0x4369C0 */
unsigned short GetShutdownErrorCode(void)
{
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
    int magnitude = ComputeFixedVectorMagnitude(vector);
    int fixedRange = abs((int)range << 8);

    if (fixedRange >= magnitude)
        return 1;
    return 0;
}
