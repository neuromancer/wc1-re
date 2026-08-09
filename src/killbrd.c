/*
 *  Kill board, conversation scenes and save-slot flags.
 *
 *  Address range 0x43c000-0x440bff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: ShowTigersClawKillBoard/RunConversationScene; string band 0x4705DC-0x470668.
 */
#include "wc1.h"

/* Function start: 0x43F690 */
void ClearSaveSlotFlag(void)
{
    DAT_00598aba = 0;
}

/* Function start: 0x43F6A0 */
int IsSaveSlotFree(void)
{
    return DAT_00598aba == 0;
}

/* Function start: 0x43F720 */
void ClearLoadSlotFlag(void)
{
    DAT_00598ab0 = 0;
}

/* Function start: 0x43F730 */
void SelectSaveSlot(short i)
{
    int v = *(int *)(DAT_00598ab6 + i * 4);

    if (v != 0)
        DAT_00598aba = (unsigned char)v;
}

/* Function start: 0x4408A0 */
void CheckHeapBlockSignature(int p)
{
    if (*(int *)(p - 8) != 0x6666656a)
        exit_squadron("not jefftep");
}

/* Function start: 0x4408C0 */
unsigned int GetHeapBlockSize(int p)
{
    return *(unsigned int *)(p - 4);
}

/* Function start: 0x440BE0 */
unsigned int SignExtendClipCoord(unsigned short v)
{
    if (v < 0xfdc0)
        return v;
    return (int)(short)v;
}
