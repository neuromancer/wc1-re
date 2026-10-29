/*
 * Campaign scoring and mission flow, followed by viewport disposal.
 * The viewport helper remains here provisionally.
 * Placement and boundary evidence are recorded in docs/ORDER.md.
 */
#define WC1_GAMEFLOW_SOURCE
#include "wc1.h"

int bOfficeVisitPending = 0; /* 0x004688cc */
int bPromotionPending = 0; /* 0x004688d0 */
int bPlayerEjectedThisMission = 0; /* 0x004688d4 */
int bPlayerShipTypeChanged = 0; /* 0x004688d8 */
short nPreviousPlayerShipType = 0; /* 0x004688dc */
int bCampaignStartupMode = 0; /* 0x004688e0 */
#ifndef SDL_PORT
static
#endif
short nPendingMedalIndex = -1; /* 0x004688e4 */
#ifndef SDL_PORT
static
#endif
short nPostSeriesSequence = -1; /* 0x004688e8 */
static unsigned short bSeriesFailed = 1; /* 0x004688ec */
int bCampaignActive = 0; /* 0x004688f0 */

/* Linker commons defined here; LINK orders them by identifier hash. */
int nViewAllocTotal; /* 0x005a7f0c */
unsigned char *apViewportAllocations[128]; /* 0x005a7f10 */
int nFreeViewCallsValue; /* 0x005a8110 */
short nQueuedCampaignIndex; /* 0x005a8114 */
short nMissionMedalScoreValue; /* 0x005a8116 */
short nCampaignDataSet; /* 0x005a8118 */

/* Function start: 0x40EFE0 */
void add_statistics(short pilot, short missions, short kills)
{
    apWingmanPilotsList[pilot]->missions += missions;
    apWingmanPilotsList[pilot]->kills += kills;
}

/* Function start: 0x40F010 */
void PostMission(void)
{
    short oldKills;
    short pilot;
    short missions;

    oldKills = stCampaignState.currentPilot->kills;
    if (oldKills < 5 && oldKills + nPlayerKillTotal >= 5)
        stCampaignState.badges[
            CAMPAIGN_BADGE_FIVE_KILLS] = 1;
    else if (oldKills < 25 &&
             oldKills + nPlayerKillTotal >= 25)
        stCampaignState.badges[
            CAMPAIGN_BADGE_TWENTY_FIVE_KILLS] = 1;

    if (stCampaignState.badges[
            CAMPAIGN_BADGE_SHIP_TYPE_BASE +
            stCampaignState.playerShipType] == 0)
        stCampaignState.badges[
            CAMPAIGN_BADGE_SHIP_TYPE_BASE +
            stCampaignState.playerShipType] = 1;

    oldKills /= 5;
    stCampaignState.currentPilot->missions++;
    switch (stCampaignState.currentPilot->missions) {
    case 1:
        stCampaignState.badges[
            CAMPAIGN_BADGE_FIRST_MISSION] = 1;
        /* The retail switch deliberately falls through. */
    case 5:
        stCampaignState.badges[
            CAMPAIGN_BADGE_FIVE_MISSIONS] = 1;
        break;
    case 10:
        stCampaignState.badges[
            CAMPAIGN_BADGE_TEN_MISSIONS] = 1;
        break;
    case 15:
        stCampaignState.badges[
            CAMPAIGN_BADGE_FIFTEEN_MISSIONS] = 1;
        break;
    }

    stCampaignState.currentPilot->kills +=
        nPlayerKillTotal;
    if (oldKills <
        stCampaignState.currentPilot->kills / 5)
        stCampaignState.promotionScore++;

    for (pilot = 0; pilot < 8; pilot++) {
        if (nYourWingman != -1 &&
            acShipRating[nYourWingman] == pilot) {
            add_statistics(pilot, 1, nWingmanKillCount);
        } else if (stCampaignState.personalityDeathMission[
                       pilot] == 0) {
            missions = RandomInRange(0, 2);
            add_statistics(pilot, missions,
                           missions == 0 ? 0 :
                           (unsigned short)RandomInRange(
                               0, nPlayerKillTotal));
        }
    }
}

/* Function start: 0x40F190 */
int FullMissionScore(void)
{
    short objective;
    signed char *scores;
    short score;

    score = 0;
    scores = (signed char *)(pMissionCampaignInfo +
        (int)stCampaignState.currentMission * 0x14 +
        (int)stCampaignState.currentSeries * 0x5a - 0x50);
    objective = 0;
    do {
        score = (short)(score + scores[objective + 4]);
        objective++;
    } while (objective < 16);
    return score;
}

/* Function start: 0x40F1E0 */
int PlayersMissionScore(void)
{
    short objective;
    signed char *scores;
    short score;

    score = 0;
    scores = (signed char *)(pMissionCampaignInfo +
        (int)stCampaignState.currentSeries * 0x5a +
        (int)stCampaignState.currentMission * 0x14 - 0x50);
    for (objective = 0; objective < 16; objective++) {
        if (achieved(objective) != 0)
            score = (short)(score + scores[objective + 4]);
    }
    return score;
}

/* Function start: 0x40F240 */
unsigned int UpdateSeries(void)
{
    unsigned char *seriesData;
    short *medalData;
    short playerScore;

    stSavedCampaignDate = *pCurrentCampaignDate;
    seriesData = pMissionCampaignInfo +
        (int)stCampaignState.currentSeries * 0x5a - 0x5a;
    medalData = (short *)(pMissionCampaignInfo +
        (int)stCampaignState.currentSeries * 0x5a +
        (int)stCampaignState.currentMission * 0x14 - 0x50);

    playerScore = (short)PlayersMissionScore();
    if ((short)FullMissionScore() == playerScore)
        stCampaignState.promotionScore++;
    stCampaignState.seriesScore = (short)(
        stCampaignState.seriesScore + playerScore);
    stCampaignState.currentMission++;

    if (stCampaignState.currentMission >=
        (signed char)seriesData[2]) {
        nPreviousPlayerShipType =
            (short)stCampaignState.playerShipType;
        nPostSeriesSequence =
            (short)(signed char)seriesData[5];
        stCampaignState.seriesHistory[
            stCampaignState.seriesHistoryCount] =
            stCampaignState.currentSeries;
        stCampaignState.seriesHistoryCount++;
        if (stCampaignState.seriesScore >=
            *(short *)(seriesData + 3)) {
            stCampaignState.currentSeries =
                (signed char)seriesData[6];
            stCampaignState.playerShipType =
                (enum ObjectType)(signed char)seriesData[7];
            bSeriesFailed = 0;
        } else {
            stCampaignState.currentSeries =
                (signed char)seriesData[8];
            stCampaignState.playerShipType =
                (enum ObjectType)(signed char)seriesData[9];
            bSeriesFailed = 1;
        }
        if (nPreviousPlayerShipType !=
            stCampaignState.playerShipType) {
            bPlayerShipTypeChanged = 1;
            bOfficeVisitPending = 1;
        }
        stCampaignState.seriesScore = 0;
        stCampaignState.currentMission = 0;
        if ((signed char)pMissionCampaignInfo[
                (int)stCampaignState.currentSeries *
                0x5a + 5] ==
                nPostSeriesSequence &&
            nPostSeriesSequence < 0x40)
            nPostSeriesSequence = -1;
    }

    if (nCurrentWingmanKilledThisMission != 0)
        nMissionMedalScoreValue =
            MaxShort(0, (short)(
                stCampaignState.missionScore - 15));
    if (medalData[1] <= nMissionMedalScoreValue &&
        nPendingMedalIndex == -1) {
        stSavedCampaignDate =
            *pCurrentCampaignDate;
        nPendingMedalIndex = medalData[0];
    }
    return 0;
}

/* Function start: 0x40F3F0 */
unsigned int MoveNewCampaign(void)
{
    if (stCampaignState.currentMission != 0)
        pCurrentCampaignDate->day += RandomInRange(0, 1);
    else
        pCurrentCampaignDate->day += RandomInRange(0, 1) + 5;
    if (pCurrentCampaignDate->day >= 366) {
        pCurrentCampaignDate->day -= 365;
        pCurrentCampaignDate->year++;
    }
    return 0;
}

/* Function start: 0x40F440 */
unsigned int StartNewCampaign(short campaign)
{
    bCampaignActive = 1;
    ResetCampaignData();
    bCampaignStartupMode = 1;
    RunTrainSim();
    stCampaignState.campaignIndex = campaign;
    nCampaignDataSet = campaign;
    bCampaignStartupMode = 0;
    LoadPacketIntoBuffer(asCampaignPilotFiles[campaign], 1,
                         pMissionCampaignInfo);
    bPanRoomTransition = 0;
    nQueuedCampaignIndex = -1;
    return 0;
}

/* Function start: 0x40F4B0 */
short GameFlow(void)
{
    short roomSelection;
    int barracksSelection;
    short launchMission;
    short flownSeries;
    short flownMission;
    short nextSeries;
    short nextMission;
    int flightResult;

    launchMission = 0;
    FrameStartHook(0);
    if (nQueuedCampaignIndex != -1) {
        stCampaignState.campaignIndex = nQueuedCampaignIndex;
        nCampaignDataSet = nQueuedCampaignIndex;
    }

    bKeyEventQueueEnabled = 0;
    do {
        roomSelection = 0;
        nPostSeriesSequence = -1;
        bPlayerEjectedThisMission = 0;
        bPromotionPending = 0;
        nPendingMedalIndex = -1;
        bOfficeVisitPending = 0;
        bPlayerShipTypeChanged = 0;
        if (bCampaignStartupMode == 0)
            roomSelection = RecRoom();
        bPanRoomTransition = 0;
        if (roomSelection == 5) {
            RunTrainSim();
        } else {
            barracksSelection = BarracksScreen();
            bCampaignStartupMode = 0;
            if (barracksSelection == 6)
                return 0;
            if (barracksSelection == 7)
                launchMission++;
        }
        PumpWindowMessages();
    } while (launchMission == 0);

    bKeyEventQueueEnabled = 1;
    nDebriefingPersonality = *(short *)(
        pMissionCampaignInfo +
        (short)stCampaignState.currentSeries * 0x5a - 0x5a);
    Briefing((short)stCampaignState.currentSeries,
             (short)stCampaignState.currentMission);
    PlayScrambleHangarScene();
    stCampaignState.playerShipType =
        aMissionShips[
            stCurrentMissionHeader.playerMissionShip].type;
    scramble();
    flownSeries = (short)stCampaignState.currentSeries;
    flownMission = (short)stCampaignState.currentMission;
    init_mission(flownSeries, flownMission);
    LaunchPlayerShip();
    flightResult = RunSpaceFlight(-1);

    switch (flightResult) {
    case 1:
        free_cockpit();
        ShowCarrierLaunchSequence(
            (signed char)nPlayerCollisionObject);
        nArcadeState = 0;
        nPlayerCollisionObject = -1;
        free_3Space();
        flightResult = calculate_damage_level();
        landing((signed char)flightResult);
        break;
    case 2:
        ejection_sequence();
        check_stranded();
        if (nArcadeState == 3)
            stranded_sequence();
        free_3Space();
        if (nArcadeState == 3)
            return 0;
        nArcadeState = 0;
        bPlayerEjectedThisMission = 1;
        stCampaignState.promotionScore = MaxShort(
            0, (short)(stCampaignState.promotionScore - 1));
        stCampaignState.elapsedDate.year++;
        if (stCampaignState.elapsedDate.year == 1)
            nPendingMedalIndex = 3;
        bOfficeVisitPending = 1;
        break;
    case 3:
        stranded_sequence();
        free_3Space();
        return 0;
    case 4:
        death_sequence();
        free_3Space();
        funeral_sequence(1);
        bCampaignActive = 0;
        return 0;
    default:
        free_cockpit();
        free_all_slots();
        free_3Space();
        return 0;
    }

    PostMission();
    UpdateSeries();
    nextSeries = (short)stCampaignState.currentSeries;
    nextMission = (short)stCampaignState.currentMission;
    stCampaignState.currentSeries = (signed char)flownSeries;
    stCampaignState.currentMission = (signed char)flownMission;

    if (bPlayerEjectedThisMission == 0) {
        if ((unsigned short)RandomInRange(0, 5) +
                stCampaignState.promotionScore > 7) {
            stCampaignState.promotionScore = 0;
            bPromotionPending =
                (nCampaignDataSet == 0 &&
                 stCampaignState.currentPilot->rank < 3) ||
                (nCampaignDataSet > 0 &&
                 stCampaignState.currentPilot->rank < 4);
            bOfficeVisitPending =
                bOfficeVisitPending != 0 ||
                bPromotionPending != 0;
        }
        DeBriefing(flownSeries, flownMission);
        if (bPromotionPending != 0)
            stCampaignState.currentPilot->rank++;
    }

    if (nextSeries == -1) {
        if (nPendingMedalIndex != -1)
            AwardCampaignMedal(nPendingMedalIndex);

        switch (nPostSeriesSequence) {
        case -1:
            ShowTheEndScreen(0);
            break;
        case 0x40:
            ShowCampaignVictorySequence();
            ShowTheEndScreen(1);
            break;
        case 0x41:
            ShowTigerClawEscapeScene();
            ShowTheEndScreen(0);
            break;
        default:
            ShowMeanwhileTransition(nPostSeriesSequence,
                                    (short)bSeriesFailed);
            ShowTheEndScreen(!bSeriesFailed);
            break;
        }
        bCampaignActive = 0;
        return 0;
    }

    if (nCurrentWingmanKilledThisMission != 0)
        funeral_sequence(0);
    if (bOfficeVisitPending == 1)
        Office();
    if (nPendingMedalIndex != -1) {
        AwardCampaignMedal(nPendingMedalIndex);
        nPendingMedalIndex = -1;
    }
    if (nPostSeriesSequence != -1)
        ShowMeanwhileTransition(nPostSeriesSequence,
                                (short)bSeriesFailed);
    stCampaignState.currentSeries = (signed char)nextSeries;
    stCampaignState.currentMission = (signed char)nextMission;
    MoveNewCampaign();
    AddRandomTrainSimHighScores();
    bPanRoomTransition = 1;
    return 1;
}

/* Function start: 0x40F940 */
void __stdcall free_viewport(Viewport *viewport)
{
    int i;
    unsigned char *allocation;

    nFreeViewCallsValue++;
    allocation = viewport->allocation;
    if (allocation != 0) {
        i = 0;
        while (i < nViewAllocTotal) {
            if (apViewportAllocations[i] == viewport->allocation) {
                apViewportAllocations[i] =
                    apViewportAllocations[nViewAllocTotal--];
                break;
            }
            i++;
        }
        if (viewport->rowOffsets != 0) {
            ReleasePacketHandle(viewport->rowOffsets);
            viewport->rowOffsets = 0;
        }
#if !defined(SDL_PORT) || defined(SDL_PORT_LEGACY_DEBUG_OUTPUT)
        if (nVideoMode != 0x13)
            printf("free_viewport not mcga\n");
#endif
        ReleasePacketHandle(allocation);
        viewport->pixels = 0;
        viewport->allocation = 0;
        if (stScreenView.pixels == allocation)
            stScreenView.pixels = 0;
        if (stScreenView.allocation == allocation)
            stScreenView.allocation = 0;
        if (stSceneBuffer.pixels == allocation)
            stSceneBuffer.pixels = 0;
        if (stSceneBuffer.allocation == allocation)
            stSceneBuffer.allocation = 0;
    }
}
