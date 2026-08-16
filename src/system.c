/*
 *  Process-level services: memory reporting and exit.
 *
 *  Address range 0x427000-0x4274df (provisional -- see docs/ORDER.md).
 *  Boundary evidence: exit_squadron/ShowMemoryStatusDebug; string band 0x46A064-0x46A10C.
 */
#include "wc1.h"

/* Function start: WC2_UNMAPPED */
void RunTrainSim(void)
{
    unsigned int savedFrameState;
    short savedCampaign;
    short savedDataSet;
    short proceed;
    int result;

    proceed = 1;
    g_nArcadeWave_00469e34 = 0;
    g_nTrainSimMission_00469e30 = 0;
    g_bInputMode_0059a848 = 1;
    SetEventManagerPump(PollMenuInputDevices);
    g_nArcadeWave_00469e34 = 0;
    g_nArcadeScore_005a7bc4 = 0;
    g_nArcadeBonusCountdown_0046a014 = 0;
    g_cCockpitView_0059dab0 = 4;
    g_cCockpitLogicalFile_005a7c74 = 21;

    if (DAT_004688e0 == 0) {
        ShowTrainSimHighScores();
        proceed = SelectTrainSimMission(&g_nTrainSimMission_00469e30);
    } else {
        g_nArcadeScore_005a7bc4 = 4000;
        g_nTrainSimMission_00469e30 = 2;
    }

    if (proceed != 0) {
        g_nCannedSceneMode_00469fac = 0;
        g_nTrainSimActive_00469e2c = 1;
        PreloadMusicTrackHook(20);
        PreloadMusicTrackHook(21);
        PreloadMusicTrackHook(22);
        ResetStringBuilder(&DAT_005a6bc0);
        savedDataSet = g_nCampaignDataSet_005a8118;
        savedCampaign = g_stCampaignState_0059ca50.campaignIndex;
        g_stCampaignState_0059ca50.campaignIndex = 0;
        g_nCampaignDataSet_005a8118 = 0;

        while (g_nTrainSimMission_00469e30 < 4) {
            g_nTrainSimActive_00469e2c = 1;
            FigureArcadeTime();
            init_mission(0, g_nTrainSimMission_00469e30);
            ShowGetReadyScreen();

            if (DAT_004688e0 != 0) {
                g_aasShipShield_00495518[0][0] = 0;
                g_aasShipMaximumShield_004954f0[0][0] = 0;
                g_acPlayerComponentDamage_00493470[2] = 4;
                g_aasShipShield_00495518[0][1] = 0;
                g_aasShipMaximumShield_004954f0[0][1] = 0;
                g_nArcadeTimeRemaining_005a7c2c = 100;
                g_nCurrentWave_0046c01c = 2;
                g_acShipDamage_0059c460[0] = (signed char)(
                    g_aObjectTypeData_00496d30[
                        g_acObjectType_00493980[0]].damageCapacity + 1);
                set_up_next_wave();
                g_nArcadeTimeRemaining_005a7c2c = 25;
            }

            InvalidateVduMode(0);
            InvalidateVduMode(1);
            DIBslam();
            DIBslamReal();
            savedFrameState = DAT_0046505c;
            DAT_0046505c = 1;
            result = RunSpaceFlight(g_nArcadeWave_00469e34);
            if (result == 1) {
                if (g_nTrainSimMission_00469e30 < 3)
                    g_nArcadeWave_00469e34 = 0;
                else
                    ShowVictoryScreen();
                g_nTrainSimMission_00469e30++;
            } else {
                g_nArcadeState_00469fb0 = 4;
                ShowGameOverScreen();
                g_nTrainSimMission_00469e30 = 4;
            }
            DAT_0046505c = savedFrameState;
        }

        g_stCampaignState_0059ca50.campaignIndex = savedCampaign;
        g_nCampaignDataSet_005a8118 = savedDataSet;
        free_all_slots();
        free_cockpit();
        free_3Space();
        ReleaseMusicTrackHook(20);
        ReleaseMusicTrackHook(21);
        ReleaseMusicTrackHook(22);
        UpdateTrainSimHighScores(g_nArcadeScore_005a7bc4);
        ShowTrainSimHighScores();
    }
    g_nTrainSimActive_00469e2c = 0;
}

/* Function start: 0x46579D */
short LogMemoryUsage(void)
{
    ShutdownHook(0x21, (void *)DAT_0059ab4c);
    EMShutDown();
    GetMessagePumpResult();
    _chdir("..");
    ShutdownVideoHook(3);
    if (g_nOriginDevUnlock_0049d774 != 0) {
        SystemDebugPrintf(
            "Original FMem %lu.  Current FMem %lu.  Current NMem %u.\n",
            g_dwOriginalFreeMemory_005a7cd8,
            GetFixedOneMillionThunkAlt(0),
            (unsigned int)(int)(short)GetOriginalFreeMemory());
    }
    MouseIdleHook();
    MessagePumpHook(8);
    return 0;
}

/* Function start: 0x437AB4 */
void exit_squadron(const char *msg)
{
    LogMemoryUsage();
    SystemDebugPrintf(msg);
    SystemDebugPrintf("[SYSTEM]: Exit_squadron\n");
    WriteDebugString("[SYSTEM] exit_squadron");
    WriteDebugString(msg);
    ClearDebugPauseFlags();
    AbortToDesktop();
    exit(0);
}

/* Function start: 0x437AEC */
unsigned int ShowMemoryStatusDebug(void)
{
    TextContext savedContext;
    TextContext *previousContext;
    char value[60];

    previousContext = g_pCurrentTextContext_005c8d1c;
    savedContext = g_stDefaultTextContext_005a7740;
    if (g_nShowMemoryStatus_0049d784 != 0) {
        InitializeTextContextFromFont(
            &g_stDefaultTextContext_005a7740, 1,
            (unsigned char)g_cViewportClearColour_004699a0,
            DAT_0046999c);
        SetTextContext(&g_stDefaultTextContext_005a7740);
        DrawFormattedText("%X%YCurrent NMem %d.",
                          0, 176, (int)(short)GetOriginalFreeMemory());
        sprintf(value, "%ld", GetFixedOneMillionThunkAlt(0));
        DrawFormattedText("%X%YCurrent FMem %s.", 0, 184, value);
        sprintf(value, "%ld", g_dwOriginalFreeMemory_005a7cd8);
        DrawFormattedText("%X%YOriginal FMem %s.", 0, 0, value);
        g_stDefaultTextContext_005a7740 = savedContext;
    }
    g_pCurrentTextContext_005c8d1c = previousContext;
    return 0;
}

/* Function start: WC2_UNMAPPED */
unsigned int GetJoystickButtonEdge(unsigned int a, short b)
{
    (void)a;
    if (b < 0)
        return JoystickEdgeHook(-1);
    return 0;
}
