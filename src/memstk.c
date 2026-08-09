/*
 *  Tracked-allocation scopes and the frame timer.
 *
 *  Address range 0x435600-0x436fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: the DAT_0046da90 node list with its DAT_0046daa0 depth counter.
 */
#include "wc1.h"

/* Function start: 0x435CC0 */
void PumpMessagesAndDispatch(int a) { PumpWindowMessages(); DispatchTbl0046da90Fn59C0(a); }

/* Function start: 0x435DB0 */
void FreeAllTrackedAllocations(void)
{
    int node = DAT_0046da90;

    while (node != 0) {
        int next = *(int *)(node + 0x14);
        LoopInputFn5760(node);
        node = next;
    }
    DAT_0046da94 = 0;
    DAT_0046da90 = 0;
}

/* Function start: 0x435DC0 */
unsigned int ResetAllocationDepth(void) { DAT_0046daa0 = 0; return 1; }

/* Function start: 0x4360D0 */
void EnterAllocationScope(void) { DAT_0046daa0 = DAT_0046daa0 + 1; }

/* Function start: 0x4360E0 */
void LeaveAllocationScope(void) { DAT_0046daa0 = DAT_0046daa0 - 1; }

/* Function start: 0x436160 */
void SetMouseHomePosition(short x, short y)
{
    DAT_0059ab10 = x;
    DAT_0059ab12 = y;
}

/* Function start: 0x4361E0 */
void SetFrameTimerPeriod(short period) { SetMultimediaTimerCallback((int)period); }

/* Function start: 0x4361F0 */
void SetFrameTimerAndWait(short period) { SetFrameTimerPeriod(period); WaitForFrameTick(); }

/* Function start: 0x436210 */
void SetFrameTimerPeriodDirect(short p) { SetMultimediaTimerCallback((int)p); }

/* Function start: 0x436230 */
void WaitForFrameTick(void) { while (DAT_0059ab3c != 0) ; }

/* Function start: 0x436240 */
int IsFrameTickElapsed(void) { return DAT_0059ab3c == 0; }

/* Function start: 0x436260 */
unsigned short GetSoundHardwareFlag(void) { return 1; }

/* Function start: 0x4362A0 */
void TimerResetHook(void) {}

/* Function start: 0x4362B0 */
unsigned int GetVideoReleaseResult(void) { return 0; }

/* Function start: 0x4362C0 */
unsigned short IdentityWord(unsigned short v) { return v; }

/* Function start: 0x4362D0 */
void TimerStopHook(void) {}

/* Function start: 0x4362E0 */
unsigned int GetFixedOneMillion(void) { return 0x3e8000; }

/* Function start: 0x4362F0 */
unsigned int GetFixedOneMillionAlt(void) { return 0x3e8000; }

/* Function start: 0x4368C0 */
void MouseIdleHook(void) {}

/* Function start: 0x4368E0 */
unsigned short GetNavRangeSentinel(void) { return 0x8000; }

/* Function start: 0x4368F0 */
unsigned short GetOriginalFreeMemory(void) { return 0x8000; }

/* Function start: 0x436900 */
void StartupHook(void) {}

/* Function start: 0x436950 */
void FreeIfNotNull(void *p) { if (p != 0) _free_dbg(p, 1); }

/* Function start: 0x436970 */
unsigned int GetStartupErrorCode(void) { return 0; }

/* Function start: 0x436980 */
void ShutdownHook(void) {}

/* Function start: 0x4369A0 */
unsigned short GetDiskRetryCount(void) { return 0; }

/* Function start: 0x4369B0 */
unsigned short GetDiskChangeCount(void) { return 0; }

/* Function start: 0x4369C0 */
unsigned short GetShutdownErrorCode(void) { return 0; }

/* Function start: 0x4369D0 */
void VideoReleaseHook(void) {}

/* Function start: 0x4369E0 */
void ExitCleanupHook(void) {}
