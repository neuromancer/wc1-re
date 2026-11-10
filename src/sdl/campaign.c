/* SDL campaign selection and Win32 Secret Missions compatibility. */
#include "wc1.h"

int nSm1TransferState;

int SdlUsingSm2Data(void)
{
    return nCampaignDataSet == 2 && !SdlSwcFlightActive() &&
        !SdlUsingDosData() && !SdlUsingTownsData();
}

void SdlSetCampaignShips(void)
{
    /* SM2.EXE object records 10 and 14: the only changed flight definitions.
     * Preserve loaded resources and restore WC1's definitions on exit. */
    static const unsigned char dralthiWeapons[0x47] = {
        6, 26, 0, 0, 0, 61, 0, 0, 26, 0, 0, 0, 62, 0, 0,
        28, 0, 0, 0, 59, 0, 0, 28, 0, 0, 0, 60, 0, 1,
        29, 0, 0, 0, 56, 0, 1, 29, 0, 0, 0, 58, 0, 1
    };
    static const ObjectTypeData hriss = {
        "Hriss", OBJECT_CLASS_SHIP, 140, 140, 1024, 4, 200000, 6, 6000,
        33, 20, NULL, 716, 8, 8, 16, 1200,
        {6, 25, 0, 0, 0, 41, 0, 0, 25, 0, 0, 0, 44, 0, 0,
         26, 0, 0, 0, 21, 0, 0, 26, 0, 0, 0, 30, 0, 0,
         29, 0, 0, 0, 31, 0, 0, 29, 0, 0, 0, 31, 0, 1},
        150, 180, 160, 170, 150, 150, NULL, NULL
    };
    static unsigned char originalWeapons[0x47];
    static ObjectTypeData originalSpikeri;
    static int active;
    ObjectTypeData *ship = &aObjectTypeData[OBJECT_TYPE_SPIKERI];
    unsigned char *animation = ship->animation;
    int sm2 = SdlUsingSm2Data();

    if (SdlSwcFlightActive() || sm2 == active)
        return;
    if (sm2) {
        originalSpikeri = *ship;
        memcpy(originalWeapons, aObjectTypeData[OBJECT_TYPE_DRALTHI].weaponLoadout,
               sizeof(originalWeapons));
    }
    memcpy(ship, sm2 ? &hriss : &originalSpikeri, offsetof(ObjectTypeData, shapeSet));
    ship->animation = animation;
    memcpy(aObjectTypeData[OBJECT_TYPE_DRALTHI].weaponLoadout,
           sm2 ? dralthiWeapons : originalWeapons, sizeof(originalWeapons));
    active = sm2;
}

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
