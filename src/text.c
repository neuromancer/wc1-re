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
    DrawTextAt(&DAT_005a74f0, DAT_005a6b80.left, DAT_005a6b80.top,
               " INFO DISPLAY \n", 0);
    DrawFormattedText("Version: %s\n", g_pGameVersion_004693b4);
    DrawFormattedText("Series %d Misn %d\n",
                      (int)g_stCampaignState_0059ca50.currentSeries,
                      (int)g_stCampaignState_0059ca50.currentMission);
    DrawFormattedText("PC Kills   =%d\nWing Kills =%d\n",
                      (int)g_nPlayerKillCount_005a7c9c,
                      (int)g_asCollisionTime_005a7ca0[12]);
    DrawFormattedText("Series Score   =%d\n",
                      (int)g_stCampaignState_0059ca50.seriesScore);
    DrawFormattedText("Miss Score %d, Medal %d\n",
                      (int)g_stCampaignState_0059ca50.missionScore,
                      (int)g_nMissionMedalScore_005a8116);
    DrawFormattedText("Promotion Score=%d\n",
                      (int)g_stCampaignState_0059ca50.promotionScore);
    objective = 0;
    DrawFormattedText("OBJECTIVES\n");
    strcpy(objectiveSummary, "");
    do {
        marker = " ";
        if (objective < g_cMissionObjectiveCount_0059c46a) {
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
        objective++;
    } while (objective < 16);
    DrawFormattedText("%s\n", objectiveSummary);
}
