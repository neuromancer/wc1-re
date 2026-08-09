/*
 *  Full-screen presentation screens.
 *
 *  Address range 0x437000-0x43bfff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: unbroken run of Blit* screens plus ShowGetReady/Victory/GameOver.
 */
#include "wc1.h"

/* Function start: 0x438B90 */
unsigned int BeginBriefingScene(void)
{
    DAT_00598c18 = GetG0046ad88FnFA90(0x140, 0);
    DAT_00598af4 = GetG0046ad88FnFA90(0x140, 0);
    return 0;
}

/* Function start: 0x439400 */
void DebugNoBodyStubv1(int p, unsigned short a, unsigned short b,
                       unsigned short c, unsigned short d)
{
    *(unsigned short *)(p + 8) = a;
    *(unsigned short *)(p + 10) = b;
    *(unsigned short *)(p + 0xc) = c;
    *(unsigned short *)(p + 0xe) = d;
}
