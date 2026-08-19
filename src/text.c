/*
 *  Cockpit information display.
 *
 *  Address range 0x433910-0x433abf (provisional -- see docs/ORDER.md).
 *  The DirectDraw error switch and its generated lookup tables end at
 *  0x43390f; the Mac symbol order proves that the ship-AI `smart` unit starts
 *  at 0x433ac0.
 */
#include "wc1.h"

/* Function start: 0x433910 */
void show_info_disp(void)
{
    char objectiveMarker[2];
    char objectiveSummary[20];
    short objective;
    char *marker;

    set_new_vdu(0);
    DrawTextAt(&stLeftVduTextContext,
               stLeftVdu.left, stLeftVdu.top,
               " INFO DISPLAY \n", 0);
    DrawFormattedText("Version: %s\n", pGameVersion);
    DrawFormattedText("Series %d Misn %d\n",
                      (int)stCampaignState.currentSeries,
                      (int)stCampaignState.currentMission);
    DrawFormattedText("PC Kills   =%d\nWing Kills =%d\n",
                      (int)nPlayerKillCount,
                      (int)nWingmanKillCount);
    DrawFormattedText("Series Score   =%d\n",
                      (int)stCampaignState.seriesScore);
    DrawFormattedText("Miss Score %d, Medal %d\n",
                      (int)stCampaignState.missionScore,
                      (int)nMissionMedalScore);
    DrawFormattedText("Promotion Score=%d\n",
                      (int)stCampaignState.promotionScore);
    objective = 0;
    DrawFormattedText("OBJECTIVES\n");
    strcpy(objectiveSummary, "");
    for (; objective < 16; objective++) {
        marker = " ";
        if (objective < cMissionObjectiveCount) {
            sprintf(objectiveMarker, "%c", objective % 10 + '1');
            marker = objectiveMarker;
            if (sighted(objective) != 0)
                marker = "S";
            if (visited(objective) != 0)
                marker = "V";
            if (achieved(objective) != 0)
                marker = "A";
        }
        strcat(objectiveSummary, marker);
    }
    DrawFormattedText("%s\n", objectiveSummary);
}
