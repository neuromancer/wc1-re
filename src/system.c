/*
 *  Process-level services: memory reporting and exit.
 *
 *  Address range 0x427000-0x4274df (provisional -- see docs/ORDER.md).
 *  Boundary evidence: exit_squadron/ShowMemoryStatusDebug; string band 0x46A064-0x46A10C.
 */
#include "wc1.h"

/* Function start: 0x427080 */
void RunTrainSim(void)
{
    unsigned int savedFrameState;
    short savedCampaign;
    short savedDataSet;
    short proceed;
    int result;

    proceed = 1;
    nArcadeWave = 0;
    nTrainSimMission = 0;
    bInputMode = 1;
    SetEventManagerPump(PollMenuInputDevices);
    nArcadeWave = 0;
    nArcadeScore = 0;
    nArcadeBonusCountdown = 0;
    cCockpitView = 4;
    cCockpitLogicalFile = 21;

    if (bCampaignStartupMode == 0) {
        ShowTrainSimHighScores();
        proceed = SelectTrainSimMission(&nTrainSimMission);
    } else {
        nArcadeScore = 4000;
        nTrainSimMission = 2;
    }

    if (proceed != 0) {
        nCannedSceneMode = 0;
        nTrainSimActive = 1;
        PreloadMusicTrackHook(20);
        PreloadMusicTrackHook(21);
        PreloadMusicTrackHook(22);
        ResetStringBuilder(&stHudMessageTextContext);
        savedDataSet = nCampaignDataSet;
        savedCampaign = stCampaignState.campaignIndex;
        stCampaignState.campaignIndex = 0;
        nCampaignDataSet = 0;

        while (nTrainSimMission < 4) {
            nTrainSimActive = 1;
            FigureArcadeTime();
            init_mission(0, nTrainSimMission);
            ShowGetReadyScreen();

            if (bCampaignStartupMode != 0) {
                aasShipShield[0][0] = 0;
                aasShipMaximumShield[0][0] = 0;
                acPlayerComponentDamage[2] = 4;
                aasShipShield[0][1] = 0;
                aasShipMaximumShield[0][1] = 0;
                nArcadeTimeRemaining = 100;
                nCurrentWave = 2;
                acShipDamage[0] = (signed char)(
                    aObjectTypeData[
                        aeObjectType[0]].damageCapacity + 1);
                set_up_next_wave();
                nArcadeTimeRemaining = 25;
            }

            InvalidateVduMode(0);
            InvalidateVduMode(1);
            DIBslam();
            DIBslamReal();
            savedFrameState = bKeyEventQueueEnabled;
            bKeyEventQueueEnabled = 1;
            result = RunSpaceFlight(nArcadeWave);
            if (result == 1) {
                if (nTrainSimMission < 3)
                    nArcadeWave = 0;
                else
                    ShowVictoryScreen();
                nTrainSimMission++;
            } else {
                nArcadeState = 4;
                ShowGameOverScreen();
                nTrainSimMission = 4;
            }
            bKeyEventQueueEnabled = savedFrameState;
        }

        stCampaignState.campaignIndex = savedCampaign;
        nCampaignDataSet = savedDataSet;
        free_all_slots();
        free_cockpit();
        free_3Space();
        ReleaseMusicTrackHook(20);
        ReleaseMusicTrackHook(21);
        ReleaseMusicTrackHook(22);
        UpdateTrainSimHighScores(nArcadeScore);
        ShowTrainSimHighScores();
    }
    nTrainSimActive = 0;
}

/* Function start: 0x4272F0 */
short LogMemoryUsage(void)
{
    ShutdownHook(0x21, (void *)DAT_0059ab4c);
    EMShutDown();
    GetMessagePumpResult();
    _chdir("..");
    ShutdownVideoHook(3);
    if (nOriginDevUnlock != 0) {
        SystemDebugPrintf(
            "Original FMem %lu.  Current FMem %lu.  Current NMem %u.\n",
            dwOriginalFreeMemory,
            GetFixedOneMillionThunkAlt(0),
            (unsigned int)(int)(short)GetOriginalFreeMemory());
    }
    MouseIdleHook();
    MessagePumpHook(8);
    return 0;
}

/* Function start: 0x427370 */
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

/* Function start: 0x4273C0 */
unsigned int ShowMemoryStatusDebug(void)
{
    TextContext savedContext;
    TextContext *previousContext;
    char value[60];

    previousContext = pCurrentTextContext;
    savedContext = stDefaultTextContext;
    if (nShowMemoryStatus != 0) {
        InitializeTextContextFromFont(
            &stDefaultTextContext, 1,
            (unsigned char)cViewportClearColour,
            cBlackColour);
        SetTextContext(&stDefaultTextContext);
        DrawFormattedText("%X%YCurrent NMem %d.",
                          0, 176, (int)(short)GetOriginalFreeMemory());
        sprintf(value, "%ld", GetFixedOneMillionThunkAlt(0));
        DrawFormattedText("%X%YCurrent FMem %s.", 0, 184, value);
        sprintf(value, "%ld", dwOriginalFreeMemory);
        DrawFormattedText("%X%YOriginal FMem %s.", 0, 0, value);
        stDefaultTextContext = savedContext;
    }
    pCurrentTextContext = previousContext;
    return 0;
}

/* Function start: 0x4274C0 */
unsigned int GetJoystickButtonEdge(unsigned int a, short b)
{
    (void)a;
    if (b < 0)
        return JoystickEdgeHook(-1);
    return 0;
}
