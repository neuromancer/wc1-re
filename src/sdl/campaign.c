/* SDL campaign selection and Win32 Secret Missions compatibility. */
#include "wc1.h"

int nPilotTransferState;
int nPilotTransferCampaign = 1;
int nLaunchCampaign;

/* SM2 plays the same whichever release supplied its files; DOS installs
 * take its Kilrathi Saga logical files 80-84 from SdlCompleteDosInstallTable. */
int SdlPlayingSm2(void)
{
    return nCampaignDataSet == 2 && !SdlSwcFlightActive();
}

/* The DOS SM2.EXE keeps its own saves in CRUSADE.WLD (CreateSaveFile,
 * 147:0308), so a DOS install's SM2 games carry over. WC.EXE and the Kilrathi
 * Saga's executables share SAVEGAME.WLD, where the transfer barracks also
 * finds the WC1 and SM1 pilots that SM2 can take over. */
const char *SdlSaveGameFileName(void)
{
    return SdlPlayingSm2() && SdlUsingDosData() && nPilotTransferState != 1 ?
        "CRUSADE.WLD" : "SAVEGAME.WLD";
}

/* SM2's captured-Dralthi disguise, as the DOS SM2.EXE plays it (issue #31);
 * the Kilrathi Saga's SM2.EXE kept WC1's code. A nav sphere with a wave link
 * back to itself (EnterSphere, 142:09b5) lets a player flying the Dralthi
 * pass as Kilrathi there. Kilrathi ships then overlook Confederation
 * Dralthis, until one of their gun shots hits a Kilrathi ship
 * (HandleCollisions, 138:209d). The DOS flag g_kilrathiMayTarget is the
 * opposite of bSdlSm2Disguised. */
static int bSdlSm2Disguised;

/* Each sphere starts without the disguise. */
void SdlSm2ClearDisguise(void)
{
    bSdlSm2Disguised = 0;
}

/* Whether the sphere's wave link of this type to linkedNav starts the
 * disguise, instead of activating its sphere as in WC1. */
int SdlSm2StartDisguise(short navPoint, short linkedNav, short type)
{
    /* g_playerShip: the type of the player's mission ship. */
    if (!SdlPlayingSm2() || linkedNav != navPoint || type == 0 ||
        aMissionShips[stCurrentMissionHeader.playerMissionShip].type !=
            OBJECT_TYPE_DRALTHI)
        return 0;
    bSdlSm2Disguised = 1;
    return 1;
}

int SdlSm2Disguised(void)
{
    return bSdlSm2Disguised;
}

/* Whether the ship scanner, looking for enemies, overlooks other: a Kilrathi
 * ship passes over a Dralthi on the other side while the disguise holds. */
int SdlSm2Overlooks(short scanner, short other)
{
    return bSdlSm2Disguised && aeObjectTypeList[other] == OBJECT_TYPE_DRALTHI &&
        aeShipSide[scanner] == SIDE_KILRATHI;
}

/* A gun shot that a Confederation Dralthi fired and that hit a Kilrathi
 * ship ends the disguise. */
void SdlSm2DisguiseHit(short shooter, short victim)
{
    if (bSdlSm2Disguised && shooter >= 0 && shooter < SPACE_OBJECT_COUNT &&
        aeShipSide[victim] == SIDE_KILRATHI &&
        aeObjectTypeList[shooter] == OBJECT_TYPE_DRALTHI &&
        aeShipSide[shooter] == SIDE_IMPERIAL)
        bSdlSm2Disguised = 0;
}

void SdlSetSm2Pilots(PilotRecord *pilots, int fresh)
{
    /* SM2.EXE initial roster (0x4a92d8); LoadGame (0x4686bc) changes
     * only names, retaining the statistics of transferred/saved pilots. */
    static const PilotRecord jazz = {"COLSON", "JAZZ", 3, 2, 18, 23, 3};
    static const PilotRecord doomsday = {"MONTCLAIR", "DOOMSDAY", 3, 2, 18, 23, 3};
    size_t size = fresh ? sizeof(PilotRecord) : offsetof(PilotRecord, portrait);

    memcpy(&pilots[2], &jazz, size);
    memcpy(&pilots[5], &doomsday, size);
}

void SdlSetCampaignShips(void)
{
    /* SM2.EXE object records 10 and 14 and its ace names (0x4a63c4, all
     * "Drakhai"): the only changed flight definitions. Preserve loaded
     * resources and restore WC1's definitions on exit. */
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
    static const char *originalAces[4];
    static int active;
    ObjectTypeData *ship = &aObjectTypeData[OBJECT_TYPE_SPIKERI];
    unsigned char *animation = ship->animation;
    int sm2 = SdlPlayingSm2();
    int ace;

    if (SdlSwcFlightActive() || sm2 == active)
        return;
    if (sm2) {
        originalSpikeri = *ship;
        memcpy(originalWeapons, aObjectTypeData[OBJECT_TYPE_DRALTHI].weaponLoadout,
               sizeof(originalWeapons));
        memcpy(originalAces, apszKilrathiAceNames, sizeof(originalAces));
    }
    memcpy(ship, sm2 ? &hriss : &originalSpikeri, offsetof(ObjectTypeData, shapeSet));
    ship->animation = animation;
    memcpy(aObjectTypeData[OBJECT_TYPE_DRALTHI].weaponLoadout,
           sm2 ? dralthiWeapons : originalWeapons, sizeof(originalWeapons));
    for (ace = 0; ace < 4; ace++)
        apszKilrathiAceNames[ace] = sm2 ? "Drakhai" : originalAces[ace];
    active = sm2;
}

int SdlValidateExpansionSave(const CampaignState *campaign)
{
    unsigned char *series;
    int valid;
    int sm2 = campaign->campaignIndex == 2;
    short logicalFile;

    if (campaign->campaignIndex < 1 || campaign->campaignIndex > 2)
        return 0;
    logicalFile = asCampaignPilotFiles[campaign->campaignIndex];

    if (!SdlHasSecretMissions(campaign->campaignIndex) || campaign->currentSeries < 1 ||
        campaign->currentSeries > (sm2 ? 9 : 8) || campaign->currentMission < 0 ||
        campaign->seriesHistoryCount < 0 || campaign->seriesHistoryCount >= 8 ||
        campaign->playerShipType < OBJECT_TYPE_HORNET ||
        (campaign->playerShipType > OBJECT_TYPE_RAPTOR &&
         !(sm2 && campaign->playerShipType == OBJECT_TYPE_DRALTHI)) ||
        GetPacketSize(pDiskFileRecords[logicalFile].name, 1) != 1170)
        return 0;
    series = PacketLoad(pDiskFileRecords[logicalFile].name, 1, NULL, 0, 0);
    if (!series)
        return 0;
    valid = campaign->currentMission < series[(campaign->currentSeries - 1) * 90 + 2];
    ReleasePacketHandle(series);
    return valid;
}

int SdlTransferPilot(short slot, short campaign)
{
    SaveGameRecord source;

    if (!SdlHasSecretMissions(campaign) || !LoadGame(slot, &source) ||
        source.campaign.campaignIndex >= campaign)
        return 0;
    /* TRANSFER.EXE 0x417bc7: initial campaign + all nine pilots, medals and
     * badges. Deaths, aces, dates and mission progress use the initial state.
     * Start in memory; the existing save/autosave UI chooses a destination. */
    ResetCampaignData();
    memcpy(aPilotEntriesList, source.pilots, sizeof(source.pilots));
    memcpy(stCampaignState.medals, source.campaign.medals, sizeof(stCampaignState.medals));
    memcpy(stCampaignState.badges, source.campaign.badges, sizeof(stCampaignState.badges));
    stCampaignState.campaignIndex = nCampaignDataSet = campaign;
    if (campaign == 2)
        SdlSetSm2Pilots(aPilotEntriesList, 0);
    CorrectPointers();
    LoadPacketIntoBuffer(asCampaignPilotFiles[campaign], 0, pConstellationDefinitionsPtr);
    LoadPacketIntoBuffer(asCampaignPilotFiles[campaign], 1, pMissionCampaignInfo);
    LoadMissionData(1, 0);
    Build_objective_list();
    bCampaignActive = 1;
    bCampaignStartupMode = bPanRoomTransition = 0;
    nQueuedCampaignIndex = -1;
    /* Done transferring: the autosave goes to the new campaign's save file. */
    nPilotTransferState = 2;
    SdlResetAutosave();
    SdlAutosaveCampaign();
    return 1;
}
