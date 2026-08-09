/*
 *  On-screen message banners and the debug cheat keys.
 *
 *  Address range 0x428000-0x42afff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: ShowOnScreenMessage and its six callers; string band 0x46A24C-0x46A378.
 */
#include "wc1.h"

/* Function start: 0x428E70 */
/* Message dwell time: grows with text length, scaled by the speed setting. */
short MeasureMessageWidth(const char *text)
{
    short n = MinShort(5, DosStrlen(text) >> 1);

    return (n + 5) * ((char)g_bMessageSpeed_0046af68 + 1);
}

/* Function start: 0x428F80 */
void ReportOutOfMemoryAndExit(void)
{
    DoLocalFn8F20("ERROR: Out of memory for %s");
    HelperOf428F80A("You do not have enough memory to run Wing Commander.");
}

/* Function start: 0x4290D0 */
void ShowVersionBanner(void)
{
    ShowOnScreenMessage(1, 9999, "WING COMMANDER VER. %s");
}

/* Function start: 0x429E70 */
void ComputeArcadeTimeBonus(void) { DAT_005a7c2c = (DAT_00469e34 + 6) * 400; }

/* Function start: 0x42A0C0 */
void RefreshCockpitStatus(void)
{
    GetShipAiFn7C50();
    if (DAT_00469fb4 < 2)
        DrawStatusBarBackdrop();
    GetUiFn9DD0();
}

/* Function start: 0x42A170 */
unsigned int ReleaseStaleNavTarget(void)
{
    short v = LoopNavFnA120(0);

    if (DAT_0059df60 != v)
        ReleaseTbl0046c35fFnBFF0(v);
    return 0;
}

/* Function start: 0x42A670 */
void RedrawCommWindow(void) { GetUiFn60F0(DAT_0059ab19, 0); }

/* Function start: 0x42AFA0 */
void WaitForDebugStep(void) { while (TakeDebugStepFlag() == 0) ; }
