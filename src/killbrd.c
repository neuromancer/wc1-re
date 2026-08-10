/*
 *  Kill board, conversation scenes and save-slot flags.
 *
 *  Address range 0x43c000-0x440bff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: ShowTigersClawKillBoard/RunConversationScene; string band 0x4705DC-0x470668.
 */
#include "wc1.h"

/* Function start: 0x43F640 */
void CorrectPointers(void)
{
    short pilot;

    pilot = 0;
    do {
        g_apWingmanPilots_00598a30[pilot] =
            &g_aPilotRecords_005988d0[pilot];
        pilot++;
    } while (pilot < 8);
    g_stCampaignState_0059ca50.currentPilot =
        &g_aPilotRecords_005988d0[8];
    g_pCurrentCampaignDate_005a86a8 =
        &g_stCampaignState_0059ca50.currentDate;
    g_pElapsedCampaignDate_005a86ac =
        &g_stCampaignState_0059ca50.elapsedDate;
}

/* Function start: 0x43F690 */
void ClearSaveSlotFlag(void)
{
    DAT_00598aba = 0;
}

/* Function start: 0x43F6A0 */
int IsSaveSlotFree(void)
{
    return DAT_00598aba < 1;
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
        DAT_00598aba = v;
}

/* Function start: 0x43F7C0 */
short FindMenuRegionAtPoint(short x, short y,
                            const TitleMenuRegion *regions)
{
    short index;

    index = 0;
    while (regions->frame != -1) {
        if (IsPointInRect(x, y, &regions->left) != 0)
            return index;
        regions++;
        index++;
    }
    return -1;
}

/* Function start: 0x440800 */
void ResetCampaignData(void)
{
    memcpy(&g_stCampaignState_0059ca50,
           &g_stInitialCampaignState_004700b0,
           sizeof(g_stCampaignState_0059ca50));
    memcpy(g_aPilotRecords_005988d0,
           g_aInitialPilotRecords_00470108,
           sizeof(g_aPilotRecords_005988d0));
    InitializeTrainSimHighScores();
    CorrectPointers();
}

/* Function start: 0x440840 */
unsigned int ReadPacketSectionData(PacketSectionHandle *handle,
                                   void *destination,
                                   unsigned int length)
{
    int offset;
    int end;

    offset = (int)(handle->dataOffset + handle->position);
    end = (int)(handle->dataOffset + handle->dataSize);
    if (destination == 0)
        return 0;
    if (end < (int)(offset + length) || length == (unsigned int)-1)
        length = (unsigned int)(end - offset);
    if (ReadDataFileAtOffset((unsigned short)handle->file, offset,
                             length, destination) == 0)
        return 0;
    handle->position += length;
    return 1;
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
