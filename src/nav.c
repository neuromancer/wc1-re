/*
 *  Nav map, location readouts and the virtual screen.
 *
 *  Address range 0x40d000-0x40ffff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: DrawNav* family; string band 0x4687AC-0x4688F4.
 */
#include "wc1.h"

/* Function start: 0x40D1D0 */
void ClearNavHazardFlag(void)
{
    DAT_00468710 = 0;
}

/* Function start: 0x40D240 */
void ClearNavLegendFlag(void)
{
    DAT_0046870c = 0;
}

/* Function start: 0x40D8C0 */
void SetScreenClipRect(unsigned short a, unsigned short b,
                       unsigned short c, unsigned short d)
{
    SetRectBounds((int)&DAT_005a76b0, a, b, c, d);
}

/* Function start: 0x40DE70 */
void FormatNavCoordinates(unsigned char *out)
{
    unsigned char tmp[4];

    SplitGameClockTicks(tmp);
    out[1] = tmp[2];
    out[0] = tmp[3];
}

/* Function start: 0x40DF50 */
char *GetNavNameSkippingMarker(short i)
{
    char *p = g_aMissionObjectives_0059dac5[i].name;

    if (*p == '?')
        p = p + 1;
    return p;
}

/* Function start: 0x40EFE0 */
void add_statistics(short pilot, short missions, short kills)
{
    g_apWingmanPilots_00598a30[pilot]->missions += missions;
    g_apWingmanPilots_00598a30[pilot]->kills += kills;
}

/* Function start: 0x40F010 */
void PostMission(void)
{
    short oldKills;
    short pilot;
    short missions;
    short kills;

    oldKills = g_stCampaignState_0059ca50.currentPilot->kills;
    if (oldKills < 5 && oldKills + g_nPlayerKillCount_005a7c9c > 4)
        g_stCampaignState_0059ca50.fiveKillFlag = 1;
    else if (oldKills < 25 &&
             oldKills + g_nPlayerKillCount_005a7c9c > 24)
        g_stCampaignState_0059ca50.twentyFiveKillFlag = 1;

    if (g_stCampaignState_0059ca50.shipFlown[
            g_stCampaignState_0059ca50.playerShipType] == 0)
        g_stCampaignState_0059ca50.shipFlown[
            g_stCampaignState_0059ca50.playerShipType] = 1;

    g_stCampaignState_0059ca50.currentPilot->missions++;
    switch (g_stCampaignState_0059ca50.currentPilot->missions) {
    case 1:
        g_stCampaignState_0059ca50.firstMissionFlag = 1;
        /* The retail switch deliberately falls through. */
    case 5:
        g_stCampaignState_0059ca50.fiveMissionFlag = 1;
        break;
    case 10:
        g_stCampaignState_0059ca50.tenMissionFlag = 1;
        break;
    case 15:
        g_stCampaignState_0059ca50.fifteenMissionFlag = 1;
        break;
    }

    g_stCampaignState_0059ca50.currentPilot->kills +=
        g_nPlayerKillCount_005a7c9c;
    if (oldKills / 5 <
        g_stCampaignState_0059ca50.currentPilot->kills / 5)
        g_stCampaignState_0059ca50.promotionScore++;

    pilot = 0;
    do {
        if (g_nYourWingman_0046c04c == -1 ||
            g_acShipRating_0059cd80[g_nYourWingman_0046c04c] != pilot) {
            if (g_stCampaignState_0059ca50.personalityDeathMission[
                    pilot] != 0) {
                pilot++;
                continue;
            }
            missions = (short)RandomInRange(0, 2);
            if (missions == 0)
                kills = 0;
            else
                kills = (short)RandomInRange(
                    0, g_nPlayerKillCount_005a7c9c);
        } else {
            missions = 1;
            kills = g_asCollisionTime_005a7ca0[12];
        }
        add_statistics(pilot, missions, kills);
        pilot++;
    } while (pilot < 8);
}

/* Function start: 0x40F190 */
int FullMissionScore(void)
{
    signed char *scores;
    short objective;
    short score;

    scores = (signed char *)(g_pMissionCampaignData_005988bc +
        (int)g_stCampaignState_0059ca50.currentSeries * 0x5a +
        (int)g_stCampaignState_0059ca50.currentMission * 0x14 - 0x4c);
    score = 0;
    objective = 0;
    do {
        score = (short)(score + scores[objective]);
        objective++;
    } while (objective < 16);
    return score;
}

/* Function start: 0x40F1E0 */
int PlayersMissionScore(void)
{
    signed char *scores;
    short objective;
    short score;

    scores = (signed char *)(g_pMissionCampaignData_005988bc +
        (int)g_stCampaignState_0059ca50.currentSeries * 0x5a +
        (int)g_stCampaignState_0059ca50.currentMission * 0x14 - 0x4c);
    score = 0;
    objective = 0;
    do {
        if (achieved(objective) != 0)
            score = (short)(score + scores[objective]);
        objective++;
    } while (objective < 16);
    return score;
}

/* Function start: 0x40F240 */
unsigned int UpdateSeries(void)
{
    unsigned char *seriesData;
    short *medalData;
    short fullScore;
    short playerScore;
    int failed;

    g_stSavedCampaignDate_0046e188 = *g_pCurrentCampaignDate_005a86a8;
    seriesData = g_pMissionCampaignData_005988bc +
        (int)g_stCampaignState_0059ca50.currentSeries * 0x5a - 0x5a;
    medalData = (short *)(g_pMissionCampaignData_005988bc +
        (int)g_stCampaignState_0059ca50.currentSeries * 0x5a +
        (int)g_stCampaignState_0059ca50.currentMission * 0x14 - 0x50);

    fullScore = (short)FullMissionScore();
    playerScore = (short)PlayersMissionScore();
    if (playerScore == fullScore)
        g_stCampaignState_0059ca50.promotionScore++;
    g_stCampaignState_0059ca50.seriesScore = (short)(
        g_stCampaignState_0059ca50.seriesScore + playerScore);
    g_stCampaignState_0059ca50.currentMission++;

    if (g_stCampaignState_0059ca50.currentMission >=
        (signed char)seriesData[2]) {
        DAT_004688dc =
            (short)g_stCampaignState_0059ca50.playerShipType;
        DAT_004688e8 = (short)(signed char)seriesData[5];
        g_stCampaignState_0059ca50.seriesHistory[
            g_stCampaignState_0059ca50.seriesHistoryCount] =
            g_stCampaignState_0059ca50.currentSeries;
        g_stCampaignState_0059ca50.seriesHistoryCount++;
        failed = g_stCampaignState_0059ca50.seriesScore <
            *(short *)(seriesData + 3);
        if (failed != 0) {
            g_stCampaignState_0059ca50.currentSeries =
                (signed char)seriesData[8];
            g_stCampaignState_0059ca50.playerShipType =
                (enum ObjectType)(signed char)seriesData[9];
        } else {
            g_stCampaignState_0059ca50.currentSeries =
                (signed char)seriesData[6];
            g_stCampaignState_0059ca50.playerShipType =
                (enum ObjectType)(signed char)seriesData[7];
        }
        DAT_004688ec = (unsigned short)failed;
        if (DAT_004688dc !=
            (short)g_stCampaignState_0059ca50.playerShipType) {
            DAT_004688d8 = 1;
            DAT_004688cc = 1;
        }
        g_stCampaignState_0059ca50.seriesScore = 0;
        g_stCampaignState_0059ca50.currentMission = 0;
        if ((signed char)g_pMissionCampaignData_005988bc[
                (int)g_stCampaignState_0059ca50.currentSeries *
                0x5a + 5] ==
                DAT_004688e8 &&
            DAT_004688e8 < 0x40)
            DAT_004688e8 = -1;
    }

    if (*(int *)&g_asCollisionTime_005a7ca0[10] != 0)
        g_nMissionMedalScore_005a8116 =
            MaxShort(0, (short)(
                g_stCampaignState_0059ca50.missionScore - 15));
    if (medalData[1] <= g_nMissionMedalScore_005a8116 &&
        DAT_004688e4 == -1) {
        g_stSavedCampaignDate_0046e188 =
            *g_pCurrentCampaignDate_005a86a8;
        DAT_004688e4 = medalData[0];
    }
    return 0;
}

/* Function start: 0x40F3F0 */
unsigned int MoveNewCampaign(void)
{
    short days;

    if (g_stCampaignState_0059ca50.currentMission == 0)
        days = (short)(RandomInRange(0, 1) + 5);
    else
        days = (short)RandomInRange(0, 1);
    g_pCurrentCampaignDate_005a86a8->day =
        (short)(g_pCurrentCampaignDate_005a86a8->day + days);
    if (g_pCurrentCampaignDate_005a86a8->day > 365) {
        g_pCurrentCampaignDate_005a86a8->day =
            (short)(g_pCurrentCampaignDate_005a86a8->day - 365);
        g_pCurrentCampaignDate_005a86a8->year++;
    }
    return 0;
}

/* Function start: 0x40F440 */
unsigned int StartNewCampaign(short campaign)
{
    DAT_004688f0 = 1;
    ResetCampaignData();
    DAT_004688e0 = 1;
    RunTrainSim();
    g_stCampaignState_0059ca50.campaignIndex = campaign;
    g_nCampaignDataSet_005a8118 = campaign;
    DAT_004688e0 = 0;
    LoadPacketIntoBuffer(g_asCampaignPilotFiles_00469450[campaign], 1,
                         g_pMissionCampaignData_005988bc);
    DAT_00470510 = 0;
    DAT_005a8114 = -1;
    return 0;
}

/* Function start: 0x40F4B0 */
short GameFlow(void)
{
    short savedSeries;
    short savedMission;
    int flightResult;

    FrameStartHook(0);
    if (DAT_005a8114 != -1) {
        g_stCampaignState_0059ca50.campaignIndex = DAT_005a8114;
        g_nCampaignDataSet_005a8118 = DAT_005a8114;
    }

    DAT_0046505c = 0;
    DAT_004688d4 = 0;
    DAT_004688e8 = -1;
    DAT_004688d0 = 0;
    DAT_004688e4 = -1;
    DAT_004688cc = 0;
    DAT_004688d8 = 0;
    DAT_00470510 = 0;

    /* RecRoom/BarracksScreen and the briefing presentation belong to separate
     * compilation units that are not reconstructed yet.  Retain the original
     * campaign state transition and enter the selected mission directly;
     * init_mission performs the mission-data load used by that path. */
    DAT_004688e0 = 0;
    PumpWindowMessages();
    DAT_0046505c = 1;
    init_mission((short)g_stCampaignState_0059ca50.currentSeries,
                 (short)g_stCampaignState_0059ca50.currentMission);
    flightResult = RunSpaceFlight(-1);

    if (flightResult == 2) {
        check_stranded();
        if (g_nArcadeState_00469fb0 == 3) {
            free_cockpit();
            free_all_slots();
            free_3Space();
            return 0;
        }
        g_nArcadeState_00469fb0 = 0;
        DAT_004688d4 = 1;
        g_stCampaignState_0059ca50.promotionScore = MaxShort(
            0, (short)(g_stCampaignState_0059ca50.promotionScore - 1));
        g_stCampaignState_0059ca50.elapsedDate.year++;
        if (g_stCampaignState_0059ca50.elapsedDate.year == 1)
            DAT_004688e4 = 3;
        DAT_004688cc = 1;
    } else if (flightResult != 1) {
        free_cockpit();
        free_all_slots();
        free_3Space();
        DAT_004688f0 = 0;
        return 0;
    }

    free_cockpit();
    g_nArcadeState_00469fb0 = 0;
    g_nPlayerCollisionObject_0046c050 = -1;
    free_all_slots();
    free_3Space();

    PostMission();
    UpdateSeries();
    savedSeries = (short)g_stCampaignState_0059ca50.currentSeries;
    savedMission = (short)g_stCampaignState_0059ca50.currentMission;

    if (DAT_004688d4 == 0) {
        if ((unsigned short)RandomInRange(0, 5) +
                g_stCampaignState_0059ca50.promotionScore > 7) {
            g_stCampaignState_0059ca50.promotionScore = 0;
            if (g_nCampaignDataSet_005a8118 == 0) {
                DAT_004688d0 =
                    g_stCampaignState_0059ca50.currentPilot->rank < 3;
            } else if (g_nCampaignDataSet_005a8118 > 0) {
                DAT_004688d0 =
                    g_stCampaignState_0059ca50.currentPilot->rank < 4;
            } else {
                DAT_004688d0 = 0;
            }
            DAT_004688cc =
                DAT_004688cc != 0 || DAT_004688d0 != 0;
        }
        if (DAT_004688d0 != 0)
            g_stCampaignState_0059ca50.currentPilot->rank++;
    }

    if (savedSeries == -1) {
        DAT_004688f0 = 0;
        return 0;
    }

    g_stCampaignState_0059ca50.currentSeries = (signed char)savedSeries;
    g_stCampaignState_0059ca50.currentMission = (signed char)savedMission;
    MoveNewCampaign();
    DAT_00470510 = 1;
    return 1;
}

/* Function start: 0x40F940 */
void __stdcall free_viewport(Viewport *viewport)
{
    unsigned char *allocation;
    int i;
    int oldCount;

    g_nFreeViewportCalls_005a8110++;
    allocation = viewport->allocation;
    if (allocation != 0) {
        i = 0;
        while (i < g_nViewportAllocationCount_005a7f0c) {
            if (g_apViewportAllocations_005a7f10[i] == allocation) {
                oldCount = g_nViewportAllocationCount_005a7f0c;
                g_nViewportAllocationCount_005a7f0c--;
                g_apViewportAllocations_005a7f10[i] =
                    g_apViewportAllocations_005a7f10[oldCount];
                break;
            }
            i++;
        }
        if (viewport->rowOffsets != 0) {
            ReleasePacketHandle((int)viewport->rowOffsets);
            viewport->rowOffsets = 0;
        }
        if (DAT_0046b168 != 0x13)
            printf("free_viewport not mcga\n");
        ReleasePacketHandle((int)allocation);
        viewport->pixels = 0;
        viewport->allocation = 0;
        if (DAT_005a6ba0.pixels == allocation)
            DAT_005a6ba0.pixels = 0;
        if (DAT_005a6ba0.allocation == allocation)
            DAT_005a6ba0.allocation = 0;
        if (DAT_005a76b0.pixels == allocation)
            DAT_005a76b0.pixels = 0;
        if (DAT_005a76b0.allocation == allocation)
            DAT_005a76b0.allocation = 0;
    }
}

/* Function start: 0x40FA30 */
unsigned short GetPaletteReadyUnused(void)
{
    return 1;
}

/* Function start: 0x40FA40 */
void DrawTitleLogo(short distance, short y)
{
    short bounds[4];
    short scale;
    short centre;

    if (distance <= 10 || g_pTitleShape_005a7f08 == 0)
        return;
    scale = (short)(0x1000 / distance);
    centre = (short)((DAT_005a7510.left + DAT_005a7510.right + 1) / 2);
    GetTransformedShapeBounds(&DAT_005a7510, centre, y,
                              g_pTitleShape_005a7f08, 1, 0, scale, 0,
                              bounds);
    DrawSpriteTransformed(&DAT_005a7510, (short)(bounds[0] - 1), y,
                          g_pTitleShape_005a7f08, 0, 0,
                          scale, scale, 0, 0);
    DrawSpriteTransformed(&DAT_005a7510, centre, y,
                          g_pTitleShape_005a7f08, 1, 0,
                          scale, scale, 0, 0);
    DrawSpriteTransformed(&DAT_005a7510, bounds[2], y,
                          g_pTitleShape_005a7f08, 2, 0,
                          scale, scale, 0, 0);
}

/* Function start: 0x40FB10 */
void UpdateTitleMenuCursor(void)
{
    TitleMenuRegion *region;
    short frame;

    frame = 0;
    region = g_aTitleMenuRegions_00468a88;
    while (region->frame != -1) {
        if (IsPointInRect(g_nHostMouseX_0059af70,
                          g_nHostMouseY_0059af72,
                          &region->left) != 0)
            frame = region->frame;
        region++;
    }
    SetMouseCursorShape(DAT_0059ab19, frame);
}

/* Function start: 0x40FB70 */
int Title_Sequence(void)
{
    short frame;
    short credit;
    short titleDistance;
    short missionShip;
    short menuIndex;
    int optionCount;
    short eventType;
    short menuOptions[4];
    unsigned char *menuShape;
    unsigned char *alternateMenuShape;
    InputEventState event;
    int activate;
    signed char state;
    signed char selectedIndex;

    state = 0;
    if (DAT_0046506c != 0)
        g_nIntroCreditCount_00468a30 += 9;
    if (DAT_0059ab58 == 0) {
        PreloadMusicTrackHook(0x17);
        SetEventManagerPump(PollJoystickButtonEvents);
        g_bIntroSceneResourcesActive_00469d48 = 0;
        init_3Space_objects(0);
        g_nCannedSceneMode_00469fac = 2;
        g_pIntroFont_005a8960 =
            (unsigned char *)FetchDiskPacketRetrying(9, 1, 0);
        g_nSceneResourceBudget_005a7ce4 = 0x3e8000;
        g_nSceneResourceBudget_005a7ce4 = LoadPacketResourceList(
            g_aIntroResourceDescriptors_00468ac0, 0, 0x3e8000);
        g_pIntroSceneResourceMirror_00467c0b =
            g_pIntroSceneResource_00467b84;
        ClearInputKeyStatePreservingModifiers();
        FlushInputEvents();
        DAT_0059ab58 = 0;

        while (state == 0) {
            PumpWindowMessages();
            missionShip = 32;
            do {
                g_aMissionShips_0046c948[missionShip].state = 0;
                missionShip++;
            } while (missionShip < 46);
            titleDistance = 200;
            remove_all_hazards();
            g_bIntroSecondaryScene_0046c024 = 0;
            set_up_action_sphere(16);
            g_pTitleShape_005a7f08 =
                (unsigned char *)FetchDiskPacketRetrying(9, 0, 0);
            StartMusicTrack(0x17, 2, 1);
            initialize_scripted_view(g_asIntroCameraSequence_0046c090);
            DAT_00469fb4 = 1;

            frame = 0;
            do {
                Update_3Space();
                if (Draw_3Space_Frame() != 0) {
                    print_subtitle(&DAT_005a7510, 0x32,
                                   g_pszIntroOpeningText_00468910);
                    dump_buffer_to_screen();
                    DIBslam();
                    DIBslamReal();
                    intro_drawbackgroundships();
                    if (CheckEscaped() != 0) {
                        state++;
                        break;
                    }
                }
                frame++;
            } while (frame < 25);
            clear_view_buffer();
            if (state != 0)
                break;

            frame = 0;
            do {
                Update_3Space();
                RenderSpaceViewFrame();
                if (CheckEscaped() != 0) {
                    state++;
                    break;
                }
                frame++;
                DIBslam();
                DIBslamReal();
            } while (frame < 110);
            if (state != 0)
                break;

            frame = 0;
            do {
                Update_3Space();
                if (Draw_3Space_Frame() != 0) {
                    DrawTitleLogo(titleDistance,
                                  (short)(g_nViewCenterY_0059a854 - 6));
                    dump_buffer_to_screen();
                    DIBslam();
                    DIBslamReal();
                    clear_view_buffer();
                }
                if (titleDistance > 16)
                    titleDistance -= 4;
                if (CheckEscaped() != 0) {
                    state++;
                    break;
                }
                frame++;
            } while (frame < 100);
            FreePacketAndClear((int *)&g_pTitleShape_005a7f08, 0);
            if (state != 0)
                break;

            ScaleFixedVector(&g_aShipForwardVector_0059bce0[61], 0x9600,
                             &g_aShipVelocity_0059c010[61]);
            set_up_action_sphere(17);
            g_bIntroSecondaryScene_0046c024 = 1;
            g_anObjectPitchRotation_0059b2a0[0] = 0;
            g_anObjectYawRotation_0059ce80[0] = 0;
            g_anObjectRollRotation_0059d7e0[0] = 0;
            start_hazard_field(0);

            credit = 0;
            while (credit < g_nIntroCreditCount_00468a30) {
                frame = 0;
                do {
                    Update_3Space();
                    if (Draw_3Space_Frame() != 0) {
                        print_subtitle(&DAT_005a7510, 0x32,
                            g_apszIntroCredits_00468a38[credit]);
                        dump_buffer_to_screen();
                        DIBslam();
                        DIBslamReal();
                        clear_view_buffer();
                    }
                    if (CheckEscaped() != 0) {
                        state++;
                        break;
                    }
                    frame++;
                } while (frame < 70);
                if (state != 0)
                    break;

                frame = 0;
                do {
                    Update_3Space();
                    RenderSpaceViewFrame();
                    DIBslam();
                    DIBslamReal();
                    if (CheckEscaped() != 0) {
                        state++;
                        break;
                    }
                    frame++;
                } while (frame < 40);
                credit++;
            }
            if (state != 0)
                break;

            frame = 0;
            do {
                Update_3Space();
                RenderSpaceViewFrame();
                DIBslam();
                DIBslamReal();
                if (CheckEscaped() != 0) {
                    state++;
                    break;
                }
                frame++;
            } while (frame < 150);
        }

        state = 0;
        StopMusicUnlessSuppressed();
        ResetSoundState();
        ReleasePacketHandle((int)g_pIntroFont_005a8960);
        ReleasePacketHandle((int)g_pTitleShape_005a7f08);
        ReleasePacketResourceList(g_aIntroResourceDescriptors_00468ac0, 0);
        g_pIntroSceneResourceMirror_00467c0b =
            g_pIntroSceneResource_00467b84;
        free_all_slots();
        free_3Space();
        g_bIntroSecondaryScene_0046c024 = 0;
        g_nCannedSceneMode_00469fac = 0;
        g_bScriptedView_0046a8d4 = 0;
        g_bIntroSceneResourcesActive_00469d48 = 1;
        ReleaseMusicTrackHook(0x17);
    }

    DAT_0059ab58 = 0;
    if (g_bTitleMenuSceneInitialized_00468ad8 == 0) {
        SceneEnterHook();
        g_bTitleMenuSceneInitialized_00468ad8 = 1;
    }
    menuShape = (unsigned char *)FetchDiskPacketRetrying(9, 4, 0);
    optionCount = 1;
    alternateMenuShape =
        (unsigned char *)FetchDiskPacketRetrying(0x4b, 0, 0);
    menuOptions[0] = 0;
    if (AnySavedGames() != 0) {
        optionCount = 2;
        menuOptions[1] = 1;
    }
    if (optionCount < 4) {
        short *fillWord;
        unsigned int fillCount;

        fillWord = &menuOptions[optionCount];
        fillCount = 4 - optionCount;
        do {
            *fillWord++ = -1;
            fillCount--;
        } while (fillCount != 0);
    }

    menuIndex = 0;
    do {
        if (menuOptions[menuIndex] == -1) {
            g_aTitleMenuRegions_00468a88[menuIndex].frame = -1;
        } else {
            g_aTitleMenuRegions_00468a88[menuIndex].frame = 1;
            if (menuOptions[menuIndex] < 3) {
                GetShapeFrameBounds(
                    &g_aTitleMenuRegions_00468a88[menuIndex].left,
                    g_aTitleMenuRegions_00468a88[menuIndex].left,
                    g_aTitleMenuRegions_00468a88[menuIndex].top,
                    menuShape, menuOptions[menuIndex]);
            } else {
                GetShapeFrameBounds(
                    &g_aTitleMenuRegions_00468a88[menuIndex].left,
                    g_aTitleMenuRegions_00468a88[menuIndex].left,
                    g_aTitleMenuRegions_00468a88[menuIndex].top,
                    alternateMenuShape, 0);
            }
        }
        menuIndex++;
    } while (menuIndex < 4);

    ClearViewport(&DAT_005a6ba0, DAT_0046999c);
    menuIndex = 0;
    do {
        if (menuOptions[menuIndex] != -1) {
            if (menuOptions[menuIndex] < 3) {
                DrawSpriteDefault(&DAT_005a6ba0,
                    g_aTitleMenuRegions_00468a88[menuIndex].left,
                    g_aTitleMenuRegions_00468a88[menuIndex].top,
                    menuShape, menuOptions[menuIndex]);
            } else {
                DrawSpriteDefault(&DAT_005a6ba0,
                    g_aTitleMenuRegions_00468a88[menuIndex].left,
                    g_aTitleMenuRegions_00468a88[menuIndex].top,
                    alternateMenuShape, 0);
            }
        }
        menuIndex++;
    } while (menuIndex < 4);
    DIBslam();
    DIBslamReal();

    DAT_0059ab23 = &DAT_005a6ba0;
    SetEventManagerPump(PollMenuInputDevices);
    *(short *)&g_aInputDeviceSamples_005a81f0[2].x = 6;
    WarpMouseTo(160, 100);
    EnterAllocationScope();
    g_bInputMode_0059a848 = 1;
    DAT_0046505c = 0;
    while (state == 0) {
        selectedIndex = -1;
        activate = 0;
        UpdateTitleMenuCursor();
        eventType = PollInputEvent(&event, 0xff);
        if (eventType == 2) {
            activate = 1;
        } else if (eventType == 3 || eventType == 5) {
            ClearInputKeyStatePreservingModifiers();
            switch ((short)event.value) {
            case 0x1c:
            case 0x1f:
            case 0x2e:
            case 0x39:
                if ((short)event.value == 0x1f)
                    selectedIndex = 0;
                if ((short)event.value == 0x2e &&
                    menuOptions[2] != -1)
                    selectedIndex = 1;
                if ((short)event.value == 0x32 &&
                    menuOptions[2] != -1)
                    selectedIndex = 2;
                activate = 1;
                break;
            case 0x24:
                CalibrateJoystickInteractive(9, 9, 1, 1);
                break;
            default:
                MoveMenuPointerFromKeyboard(&event);
                break;
            }
        }
        if (activate != 0) {
            if (selectedIndex == -1)
                selectedIndex = FindMenuRegionAtPoint(
                    event.x, event.y, g_aTitleMenuRegions_00468a88);
            if (selectedIndex < 0 || selectedIndex > 3)
                state = 0;
            else
                state = (signed char)(menuOptions[selectedIndex] + 1);
        }
        DIBslam();
        DIBslamReal();
    }

    DAT_0046505c = 1;
    ClearDebugPauseFlags();
    ReleasePacketHandle((int)menuShape);
    ReleasePacketHandle((int)alternateMenuShape);
    SetEventManagerPump(0);
    EventManagerHook(0);
    LeaveAllocationScope();
    FadeViewportPaletteToColour(&DAT_005a6ba0, DAT_0046999c, 1);
    ClearViewport(&DAT_005a6ba0, DAT_0046999c);
    DIBslam();
    DIBslamReal();
    RestoreGamePalette();
    return state - 1;
}
