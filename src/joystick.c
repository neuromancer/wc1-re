/*
 *  Joystick calibration and input dispatch.
 *
 *  Address range 0x410000-0x412fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: CalibrateJoystickInteractive; string band 0x468F04-0x468FEC.
 */
#include "wc1.h"

/* Function start: 0x410AD0 */
void ApplyPitchFromFloat(void)
{
    FixedToShortSaturating((int)FloatToLongDirect());
}

/* Function start: 0x4117B0 */
unsigned int ResetCommTarget(void *p, unsigned int arg)
{
    DAT_0046c03c = 0xff;
    DispatchCommMenuCommand(p, (int)p, (short)arg);
    return 0;
}

/* Function start: 0x412410 */
unsigned short TickShipAiCountdown(short ship)
{
    if (DAT_0059c330[ship] != -1)
        DAT_0059c330[ship] = DAT_0059c330[ship] - 1;
    return ship;
}
