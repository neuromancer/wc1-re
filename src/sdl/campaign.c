/* SDL campaign selection and Win32 Secret Missions compatibility. */
#include "wc1.h"

int nSm1TransferState;

int SdlValidateSm1Save(const CampaignState *campaign)
{
    unsigned char *series;
    int valid;

    if (!SdlHasSecretMissions1() || campaign->currentSeries < 1 ||
        campaign->currentSeries > 8 || campaign->currentMission < 0 ||
        campaign->seriesHistoryCount < 0 || campaign->seriesHistoryCount >= 8 ||
        campaign->playerShipType < OBJECT_TYPE_HORNET ||
        campaign->playerShipType > OBJECT_TYPE_RAPTOR ||
        GetPacketSize(pDiskFileRecords[61].name, 1) != 1170)
        return 0;
    series = PacketLoad(pDiskFileRecords[61].name, 1, NULL, 0, 0);
    if (!series)
        return 0;
    valid = campaign->currentMission < series[(campaign->currentSeries - 1) * 90 + 2];
    ReleasePacketHandle(series);
    return valid;
}

int SdlTransferSm1Pilot(short slot)
{
    SaveGameRecord source;

    if (!SdlHasSecretMissions1() || !LoadGame(slot, &source) ||
        source.campaign.campaignIndex != 0)
        return 0;
    /* TRANSFER.EXE 0x417bc7: initial campaign + all nine pilots, medals and
     * badges. Deaths, aces, dates and mission progress use the initial state.
     * Start in memory; the existing save/autosave UI chooses a destination. */
    ResetCampaignData();
    memcpy(aPilotEntriesList, source.pilots, sizeof(source.pilots));
    memcpy(stCampaignState.medals, source.campaign.medals, sizeof(stCampaignState.medals));
    memcpy(stCampaignState.badges, source.campaign.badges, sizeof(stCampaignState.badges));
    stCampaignState.campaignIndex = nCampaignDataSet = 1;
    CorrectPointers();
    LoadPacketIntoBuffer(asCampaignPilotFiles[1], 0, pConstellationDefinitionsPtr);
    LoadPacketIntoBuffer(asCampaignPilotFiles[1], 1, pMissionCampaignInfo);
    LoadMissionData(1, 0);
    Build_objective_list();
    bCampaignActive = 1;
    bCampaignStartupMode = bPanRoomTransition = 0;
    nQueuedCampaignIndex = -1;
    SdlResetAutosave();
    SdlAutosaveCampaign();
    return 1;
}
