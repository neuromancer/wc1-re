/*
 *  Disk data files and packet fetching with retry.
 *
 *  Address range 0x41d250-0x41efff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: OpenDiskDataFile/FetchDiskPacketRetrying/PromptInsertNumberedDisk.
 */
#include "wc1.h"

/* Function start: 0x41DA00 */
unsigned int GetZeroUnused(void)
{
    return 0;
}

/* Function start: 0x41DEB0 */
void WaitForStreamIdle(void)
{
    unsigned int saved = DAT_0046505c;

    DAT_0046505c = 1;
    while (UiRoutine03() == 0)
        ;
    DAT_0046505c = saved;
}

/* Function start: 0x41DF40 */
short FindActiveShipSlot(void)
{
    short i = 0x22;

    do {
        if (g_abShipSlotState_0059d100[i] == 4)
            return i;
        i = i + 1;
    } while (i < 0x2a);
    return -1;
}

/* Function start: 0x41E7C0 */
unsigned int ShipAiRoutine08(short ship)
{
    DAT_0059c310[ship] = 0;
    DAT_0059d7a0[ship] = 0;
    DAT_0059d630[ship] = 0;
    return 0;
}

/* Function start: 0x41E7F0 */
void ShipAiRoutine09(void)
{
    FixedToShortSaturating((int)FloatToLongDirect());
}
