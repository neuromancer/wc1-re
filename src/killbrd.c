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
void ClearRoomMenuLabel(void)
{
    g_pszCurrentRoomMenuLabel_00598aba = 0;
}

/* Function start: 0x43F6A0 */
int IsRoomMenuLabelEmpty(void)
{
    return g_pszCurrentRoomMenuLabel_00598aba == 0;
}

/* Function start: 0x43F6B0 */
void DrawRoomMenuLabel(TextContext *context, char *label)
{
    DosStrcpy(g_szTextScratchBuffer_00598b00, label);
    SetTextContext(context);
    FormatTextBufferFromStart(g_szRoomMenuLabelFormat_004705dc,
                              0, 188,
                              g_szTextScratchBuffer_00598b00);
}

/* Function start: 0x43F6F0 */
void RefreshRoomMenuLabel(void)
{
    if (IsRoomMenuLabelEmpty())
        g_pszCurrentRoomMenuLabel_00598aba =
            (char *)g_pszBlankRoomMenuLabel_00470090;
    DrawRoomMenuLabel(&g_stRoomMenuTextContext_00598abe,
                      g_pszCurrentRoomMenuLabel_00598aba);
}

/* Function start: 0x43F720 */
void ClearRoomMenuCursorFrame(void)
{
    g_nRoomMenuCursorFrame_00598ab0 = 0;
}

/* Function start: 0x43F730 */
void SelectRoomMenuLabel(short i)
{
    char *label;

    label = g_ppszRoomMenuLabels_00598ab6[i];
    if (label != 0)
        g_pszCurrentRoomMenuLabel_00598aba = label;
}

/* Function start: 0x43F750 */
void InitializeRoomMenu(TitleMenuRegion *regions, char **labels,
                        Viewport *viewport, char *text,
                        unsigned char alignment)
{
    g_pRoomMenuRegions_00598ab2 = regions;
    g_ppszRoomMenuLabels_00598ab6 = labels;
    g_pszCurrentRoomMenuLabel_00598aba =
        (char *)g_pszBlankRoomMenuLabel_00470090;
    g_stRoomMenuTextContext_00598abe.viewport = viewport;
    g_stRoomMenuTextContext_00598abe.text = text;
    g_stRoomMenuTextContext_00598abe.textCursor = text;
    g_stRoomMenuTextContext_00598abe.alignment = alignment;
    *text = (char)g_dwRoomMenuStringPadding_004705e8;
    InitializeTextContextFromFont(&g_stRoomMenuTextContext_00598abe,
                                  0, g_cViewportClearColour_004699a0,
                                  -1);
    ClearRoomMenuCursorFrame();
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

/* Function start: 0x43F810 */
void InitializeRoomViewports(void)
{
    g_stRoomScreenViewport_005988a0 = DAT_005a6ba0;
    DAT_005a76b0.right = 319;
    DAT_005a76b0.bottom = 199;
    DAT_005a76b0.left = 0;
    DAT_005a76b0.top = 0;
    AllocateViewport(&DAT_005a76b0, DAT_0046999c, 0);
    g_stRoomMouseViewport_00598a80 = DAT_005a76b0;
    g_stRoomDisplayViewport_00598a60 = g_stRoomScreenViewport_005988a0;
    SetEventManagerPump(PollMenuInputDevices);
    EventManagerHook(UpdateRoomMenuCursor);
    g_nSavedRoomControllerX_005988b4 =
        (short)g_aInputDeviceSamples_005a81f0[2].x;
    *(short *)&g_aInputDeviceSamples_005a81f0[2].x = 6;
}

/* Function start: 0x43F940 */
short RecRoom(void)
{
    InputEventState event;
    ShortRect firstPilotBounds;
    ShortRect nextFrameBounds;
    ShortRect secondPilotBounds;
    Viewport bottomDestination;
    Viewport bottomSource;
    Viewport pilotDestination;
    Viewport pilotWork;
    Viewport shotglassDestination;
    Viewport shotglassWork;
    signed char animationIds[3];
    signed char *animations[3];
    unsigned char clicked;
    unsigned char firstFrame;
    short characterMask;
    short eventType;
    short frame;
    short index;
    short region;
    short result;
    int lastChalkboardTick;
    int rosterOffset;
    int personality;

    result = 0;
    lastChalkboardTick = 0;
    firstFrame = 0;
    characterMask = 0;
    g_apszRecRoomMenuLabels_004704f8[0] =
        g_apszRecRoomBaseLabels_004704e8[0];
    g_apszRecRoomMenuLabels_004704f8[1] =
        g_apszRecRoomBaseLabels_004704e8[1];
    g_apszRecRoomMenuLabels_004704f8[2] =
        g_apszRecRoomBaseLabels_004704e8[2];

    PreloadMusicTrackHook(30);
    StartMusicTrack(30, 2, 1);
    FlushInputEvents();
    LoadBriefingData((short)g_stCampaignState_0059ca50.currentSeries,
                     (short)g_stCampaignState_0059ca50.currentMission);
    g_pRecRoomRoster_005988b8 = (unsigned char *)LoadPacketAllocated(
        g_asCampaignPilotFiles_00469450[g_nCampaignDataSet_005a8118], 2);

    animationIds[0] = (signed char)(RandomInRange(0, 3) + 9);
    g_apRecRoomCharacterShapes_005988c0[0] = 0;
    g_apRecRoomCharacterShapes_005988c0[1] = 0;
    g_apRecRoomCharacterShapes_005988c0[2] = 0;
    g_aRecRoomMenuRegions_004704a0[0].left = 400;
    g_aRecRoomMenuRegions_004704a0[0].top = 400;
    g_aRecRoomMenuRegions_004704a0[0].right = 401;
    g_aRecRoomMenuRegions_004704a0[0].bottom = 401;
    *(ShortRect *)&g_aRecRoomMenuRegions_004704a0[1].left =
        *(ShortRect *)&g_aRecRoomMenuRegions_004704a0[0].left;
    *(ShortRect *)&g_aRecRoomMenuRegions_004704a0[2].left =
        *(ShortRect *)&g_aRecRoomMenuRegions_004704a0[0].left;

    g_apRecRoomCharacterShapes_005988c0[0] =
        (unsigned char *)FetchDiskPacketRetrying(5, 11, 0);
    GetShapeFrameBounds(&g_aRecRoomMenuRegions_004704a0[0].left,
                        g_aRecRoomCharacterOrigins_00470490[0].x,
                        g_aRecRoomCharacterOrigins_00470490[0].y,
                        g_apRecRoomCharacterShapes_005988c0[0], 0);

    rosterOffset = ((int)g_stCampaignState_0059ca50.currentMission +
                    (int)g_stCampaignState_0059ca50.currentSeries * 4) *
                       2 - 8;
    animationIds[1] =
        (signed char)g_pRecRoomRoster_005988b8[rosterOffset];
    if (animationIds[1] != -1) {
        personality = (int)animationIds[1];
        if (g_stCampaignState_0059ca50
                .personalityDeathMission[personality] == 0) {
            g_apRecRoomCharacterShapes_005988c0[1] =
                (unsigned char *)FetchDiskPacketRetrying(
                    5, (short)(personality + 3), 0);
            GetShapeFrameBounds(
                &g_aRecRoomMenuRegions_004704a0[1].left,
                g_aRecRoomCharacterOrigins_00470490[1].x,
                g_aRecRoomCharacterOrigins_00470490[1].y,
                g_apRecRoomCharacterShapes_005988c0[1], 0);
            sprintf(g_szTalkToFirstPilot_00470570,
                    g_szTalkToPilotFormat1_004705ec,
                    g_apWingmanPilots_00598a30[personality]->callsign);
        } else {
            sprintf(g_szTalkToFirstPilot_00470570,
                    g_szTalkToPilotFormat1_004705ec + 12);
        }
    }

    animationIds[2] =
        (signed char)g_pRecRoomRoster_005988b8[rosterOffset + 1];
    if (animationIds[1] != -1) {
        personality = (int)animationIds[2];
        if (g_stCampaignState_0059ca50
                .personalityDeathMission[personality] == 0) {
            g_apRecRoomCharacterShapes_005988c0[2] =
                (unsigned char *)FetchDiskPacketRetrying(
                    5, (short)(personality + 3), 0);
            GetShapeFrameBounds(
                &g_aRecRoomMenuRegions_004704a0[2].left,
                g_aRecRoomCharacterOrigins_00470490[2].x,
                g_aRecRoomCharacterOrigins_00470490[2].y,
                g_apRecRoomCharacterShapes_005988c0[2], 0);
            sprintf(g_szTalkToSecondPilot_00470588,
                    g_szTalkToPilotFormat2_004705fc,
                    g_apWingmanPilots_00598a30[personality]->callsign);
        } else {
            sprintf(g_szTalkToSecondPilot_00470588,
                    g_szTalkToPilotFormat2_004705fc + 12);
        }
    }

    ReleasePacketHandle((int)g_pRecRoomRoster_005988b8);
    if (g_apRecRoomCharacterShapes_005988c0[2] != 0)
        characterMask = 1;
    if (g_apRecRoomCharacterShapes_005988c0[1] != 0) {
        characterMask = 2;
        if (g_apRecRoomCharacterShapes_005988c0[2] != 0)
            characterMask = 3;
    }

    SetViewportRect(&DAT_005a76b0, 0, 0, 319, 199);
    InitializeRoomViewports();
    init_constellation(0);
    g_stConstellationViewport_005a6b40 = DAT_005a76b0;
    SetViewportRect(&g_stConstellationViewport_005a6b40,
                    54, 35, 146, 72);
    InitializeConstellationField(&g_stConstellationViewport_005a6b40,
                                 -1, 6);

    animations[0] = g_abShotglassIdleAnimation_004703b8;
    animations[1] =
        g_apRecRoomAnimations_00470458[(int)animationIds[1]];
    animations[2] =
        g_apRecRoomAnimations_00470458[(int)animationIds[2]];
    InitializeRoomMenu(g_aRecRoomMenuRegions_004704a0,
                       g_apszRecRoomMenuLabels_004704f8,
                       &g_stRoomScreenViewport_005988a0,
                       g_szDefaultTextBuffer_005a7590, 2);

    bottomSource = DAT_005a76b0;
    SetViewportRect(&bottomSource, 0, 187, 319, 199);
    bottomDestination = g_stRoomScreenViewport_005988a0;
    SetViewportRect(&bottomDestination, 0, 187, 319, 199);
    g_pRecRoomBackgroundShape_00598a50 =
        (unsigned char *)FetchDiskPacketRetrying(5, 0, 0);
    g_nMenuPointerSpeed_0046af58 = 1;
    g_bInputMode_0059a848 = 1;
    DAT_0059ab23 = &g_stRoomDisplayViewport_00598a60;

    pilotWork = DAT_005a76b0;
    shotglassWork = DAT_005a76b0;
    pilotDestination = g_stRoomScreenViewport_005988a0;
    shotglassDestination = g_stRoomScreenViewport_005988a0;

    if (g_apRecRoomCharacterShapes_005988c0[2] != 0) {
        GetShapeFrameBounds(
            &secondPilotBounds.left,
            g_aRecRoomCharacterOrigins_00470490[2].x,
            g_aRecRoomCharacterOrigins_00470490[2].y,
            g_apRecRoomCharacterShapes_005988c0[2],
            (short)*animations[2]);
    }
    if (g_apRecRoomCharacterShapes_005988c0[1] != 0) {
        GetShapeFrameBounds(
            &firstPilotBounds.left,
            g_aRecRoomCharacterOrigins_00470490[1].x,
            g_aRecRoomCharacterOrigins_00470490[1].y,
            g_apRecRoomCharacterShapes_005988c0[1],
            (short)*animations[1]);
        *(ShortRect *)&pilotWork.left = firstPilotBounds;
        if (g_apRecRoomCharacterShapes_005988c0[2] != 0) {
            UnionRectBounds((ShortRect *)&pilotWork.left,
                            &firstPilotBounds, &secondPilotBounds);
        }
    } else if (g_apRecRoomCharacterShapes_005988c0[2] != 0) {
        *(ShortRect *)&pilotWork.left = secondPilotBounds;
    }
    if (g_apRecRoomCharacterShapes_005988c0[1] != 0 ||
        g_apRecRoomCharacterShapes_005988c0[2] != 0) {
        *(ShortRect *)&pilotDestination.left =
            *(ShortRect *)&pilotWork.left;
    }

    WarpMouseTo(160, 100);
    GetShapeFrameBounds(
        &shotglassWork.left,
        g_aRecRoomCharacterOrigins_00470490[0].x,
        g_aRecRoomCharacterOrigins_00470490[0].y,
        g_apRecRoomCharacterShapes_005988c0[0],
        (short)*animations[0]);

    while (result == 0) {
        if (firstFrame == 0) {
            DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                              g_pRecRoomBackgroundShape_00598a50, 0);
            if (characterMask != 0) {
                DrawSpriteDefault(&DAT_005a76b0, 158, 128,
                                  g_pRecRoomBackgroundShape_00598a50,
                                  characterMask);
            }
            SetFrameTimerPeriodDirect(0);
        }

        if (IsFrameTickElapsed() != 0) {
            DrawSpriteDefault(&pilotWork, 0, 0,
                              g_pRecRoomBackgroundShape_00598a50, 0);
            index = 0;
            do {
                if (g_apRecRoomCharacterShapes_005988c0[index] != 0) {
                    if (*animations[index] == -1) {
                        if (index == 0) {
                            animationIds[0] =
                                (signed char)(RandomInRange(0, 3) + 9);
                            if (animationIds[0] == 11 &&
                                RandomInRange(0, 3) != 0)
                                animationIds[0]--;
                        }
                        animations[index] =
                            g_apRecRoomAnimations_00470458[
                                (int)animationIds[index]];
                    }

                    if (index > 0) {
                        DrawSpriteDefault(
                            &pilotWork,
                            g_aRecRoomCharacterOrigins_00470490[index].x,
                            g_aRecRoomCharacterOrigins_00470490[index].y,
                            g_apRecRoomCharacterShapes_005988c0[index], 0);
                        frame = (short)*animations[index]++;
                        DrawSpriteDefault(
                            &pilotWork,
                            g_aRecRoomCharacterOrigins_00470490[index].x,
                            g_aRecRoomCharacterOrigins_00470490[index].y,
                            g_apRecRoomCharacterShapes_005988c0[index],
                            frame);
                    } else {
                        UnionRectBounds(
                            (ShortRect *)&shotglassWork.left,
                            (ShortRect *)&shotglassWork.left,
                            (ShortRect *)&g_stConstellationViewport_005a6b40.left);
                        DrawConstellationField();
                        DrawSpriteDefault(
                            &shotglassWork, 0, 0,
                            g_pRecRoomBackgroundShape_00598a50, 0);
                        frame = (short)*animations[index]++;
                        DrawSpriteDefault(
                            &shotglassWork,
                            g_aRecRoomCharacterOrigins_00470490[index].x,
                            g_aRecRoomCharacterOrigins_00470490[index].y,
                            g_apRecRoomCharacterShapes_005988c0[index],
                            frame);
                        GetShapeFrameBounds(
                            &nextFrameBounds.left,
                            g_aRecRoomCharacterOrigins_00470490[index].x,
                            g_aRecRoomCharacterOrigins_00470490[index].y,
                            g_apRecRoomCharacterShapes_005988c0[index],
                            (short)*animations[index]);
                        if (firstFrame != 0) {
                            *(ShortRect *)&shotglassDestination.left =
                                *(ShortRect *)&shotglassWork.left;
                            if (ShouldSuspendCursorForRect(
                                    &nextFrameBounds) != 0) {
                                LeaveAllocationScope();
                                CopyViewportContents(
                                    &shotglassWork,
                                    &shotglassDestination);
                                EnterAllocationScope();
                            } else {
                                CopyViewportContents(
                                    &shotglassWork,
                                    &shotglassDestination);
                            }
                        }
                    }
                }
                index++;
            } while (index < 3);

            if (firstFrame == 0) {
                firstFrame = 1;
                if (DAT_00470510 != 0) {
                    PanToScreen(&DAT_005a76b0,
                                &g_stRoomScreenViewport_005988a0);
                    DAT_00470510 = 0;
                } else {
                    CopyViewportContents(
                        &DAT_005a76b0,
                        &g_stRoomScreenViewport_005988a0);
                }
                EnterAllocationScope();
            } else if (g_apRecRoomCharacterShapes_005988c0[1] != 0 ||
                       g_apRecRoomCharacterShapes_005988c0[2] != 0) {
                if (ShouldSuspendCursorForRect(
                        (ShortRect *)&pilotWork.left) != 0) {
                    LeaveAllocationScope();
                    CopyViewportContents(&pilotWork,
                                         &pilotDestination);
                    EnterAllocationScope();
                } else {
                    CopyViewportContents(&pilotWork,
                                         &pilotDestination);
                }
            }

            LeaveAllocationScope();
            CopyViewportContents(&bottomSource, &bottomDestination);
            RefreshRoomMenuLabel();
            EnterAllocationScope();
            SetFrameTimerPeriodDirect(9);
        }

        clicked = 0;
        eventType = PollInputEvent(&event, 0xff);
        if (eventType == 3 || eventType == 5) {
            ClearInputKeyStatePreservingModifiers();
            if ((short)event.value == 0x1c ||
                (short)event.value == 0x39) {
                clicked = 1;
            } else {
                MoveMenuPointerFromKeyboard(&event);
            }
        } else if (eventType == 2 || eventType == 10) {
            clicked = 1;
        } else if (eventType == 13) {
            UpdateRoomMenuCursor();
        }

        if (clicked != 0) {
            region = FindMenuRegionAtPoint(
                event.x, event.y, g_aRecRoomMenuRegions_004704a0);
            LeaveAllocationScope();
            if (region >= 0 && region <= 2) {
                if (g_apRecRoomCharacterShapes_005988c0[region] != 0) {
                    free_constellation();
                    ReleasePacketHandle(
                        (int)g_pRecRoomBackgroundShape_00598a50);
                    DAT_005a76b0.bottom = 127;
                    DAT_005a6ba0.top = 24;
                    DAT_005a6ba0.bottom = 151;
                    InitializeConversationText();
                    ClearViewport(&g_stRoomScreenViewport_005988a0,
                                  DAT_0046999c);
                    g_pConversationBackdropShape_00598c04 =
                        (unsigned char *)FetchDiskPacketRetrying(
                            5, 1, 0);
                    SceneDirector(
                        2,
                        g_apRecRoomSceneData_00598ae0[region],
                        g_apRecRoomTextData_00598aa0[region]);
                    DAT_0059ab58 = 0;
                    SetEventManagerPump(PollMenuInputDevices);
                    FreePacketAndClear(
                        (int *)&g_pConversationBackdropShape_00598c04,
                        0);
                    SetFrameTimerPeriodDirect(1);
                    DAT_005a6ba0.top = 0;
                    DAT_005a6ba0.bottom = 199;
                    DAT_005a76b0.bottom = 199;
                    g_stConstellationViewport_005a6b40 =
                        DAT_005a76b0;
                    g_stConstellationViewport_005a6b40.left = 54;
                    g_stConstellationViewport_005a6b40.top = 35;
                    g_stConstellationViewport_005a6b40.right = 146;
                    g_stConstellationViewport_005a6b40.bottom = 72;
                    init_constellation(0);
                    InitializeConstellationField(
                        &g_stConstellationViewport_005a6b40, -1, 6);
                    g_pRecRoomBackgroundShape_00598a50 =
                        (unsigned char *)FetchDiskPacketRetrying(
                            5, 0, 0);
                    ClearViewport(&g_stRoomScreenViewport_005988a0,
                                  DAT_0046999c);
                }
            } else if (region == 3) {
                FlushInputEvents();
                if ((int)(DAT_0059ab54 - lastChalkboardTick) >
                    g_nInputTickScale_0059af90) {
                    ShowChalkBoard();
                    ClearViewport(&DAT_005a76b0, DAT_0046999c);
                    lastChalkboardTick = (int)DAT_0059ab54;
                }
            } else if (region == 4 || region == 5) {
                result = region;
            } else {
                clicked = 0;
                EnterAllocationScope();
            }

            g_stRoomMouseViewport_00598a80 = DAT_005a6ba0;
            DAT_0059ab23 = &g_stRoomMouseViewport_00598a80;
            g_bInputMode_0059a848 = 1;
            if (clicked != 0)
                firstFrame = 0;
        }

        ShowMemoryStatusDebug();
        DIBslam();
        DIBslamReal();
    }

    g_nMenuPointerSpeed_0046af58 = 2;
    *(short *)&g_aInputDeviceSamples_005a81f0[2].x =
        g_nSavedRoomControllerX_005988b4;
    EventManagerHook(0);
    ReleasePacketHandle(
        (int)g_apRecRoomCharacterShapes_005988c0[0]);
    ReleasePacketHandle(
        (int)g_apRecRoomCharacterShapes_005988c0[1]);
    ReleasePacketHandle(
        (int)g_apRecRoomCharacterShapes_005988c0[2]);
    free_constellation();
    ReleasePacketHandle((int)g_pRecRoomBackgroundShape_00598a50);
    ReleaseTextFont(0);
    ReleasePacketHandle((int)g_pBriefingPacket_00598aec);
    ClearViewport(&DAT_005a6ba0, DAT_0046999c);
    free_viewport(&DAT_005a76b0);
    DAT_0059ab58 = 0;
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook(30);
    return result;
}

/* Function start: 0x440510 */
void ShowChalkBoard(void)
{
    InputEventState event;
    TextContext context;
    TextContext *previousContext;
    unsigned char *background;
    unsigned char savedInputMode;
    char pilotName[20];
    short done;
    short index;
    short other;
    short pilot;
    short rank;
    short rowY;
    short swap;
    int score;
    int otherScore;

    index = 0;
    do {
        other = index;
        do {
            pilot = g_asChalkBoardPilotOrder_00470518[index];
            swap = g_asChalkBoardPilotOrder_00470518[other];
            score = g_aPilotRecords_005988d0[pilot].kills * 1000 -
                    g_aPilotRecords_005988d0[pilot].missions + 1;
            otherScore = g_aPilotRecords_005988d0[swap].kills * 1000 -
                         g_aPilotRecords_005988d0[swap].missions + 1;
            if (score < otherScore) {
                g_asChalkBoardPilotOrder_00470518[index] = swap;
                g_asChalkBoardPilotOrder_00470518[other] = pilot;
            }
            other++;
        } while (other < 9);
        index++;
    } while (index < 9);

    memset(&context, 0, sizeof(context));
    g_stChalkBoardDate_00470514 = *g_pCurrentCampaignDate_005a86a8;
    background = (unsigned char *)FetchDiskPacketRetrying(5, 2, 0);
    previousContext = g_pCurrentTextContext_0059af8c;
    g_stModalSourceViewport_005a7670 = DAT_005a6ba0;
    context.viewport = &g_stModalSourceViewport_005a7670;
    context.text = g_szDefaultTextBuffer_005a7590;
    context.textCursor = g_szDefaultTextBuffer_005a7590;
    InitializeTextContextFromFont(&context, 3,
                                  g_cViewportClearColour_004699a0, -1);
    savedInputMode = g_bInputMode_0059a848;
    g_bInputMode_0059a848 = 1;
    done = 0;
    do {
        PumpWindowMessages();
        if (PeekInputEvent(&event, 10) != 0 ||
            PeekInputEvent(&event, 2) != 0 ||
            PeekInputEvent(&event, 3) != 0 ||
            PeekInputEvent(&event, 5) != 0)
            done = 1;

        DrawSpriteDefault(&g_stModalSourceViewport_005a7670,
                          0, 0, background, 0);
        context.alignment = 2;
        SetTextContext(&context);
        FormatTextBufferFromStart(g_szChalkBoardHeading_0047060c,
                                  0, 10);
        DrawFormattedText(g_szChalkBoardPilotHeading_0047062c,
                          60, 24);
        DrawFormattedText(g_szChalkBoardScoreHeading_00470638,
                          185, 24);
        rowY = 46;
        index = 0;
        do {
            pilot = g_asChalkBoardPilotOrder_00470518[index];
            DosStrcpy(pilotName, g_aPilotRecords_005988d0[pilot].name);
            _strupr(pilotName);
            rank = g_aPilotRecords_005988d0[pilot].rank;
            if (rank < 0)
                rank = 0;
            if (rank > 4)
                rank = 4;
            DrawFormattedText(g_szChalkBoardPilotFormat_0047064c,
                              10, rowY,
                              g_apszPilotRankNames_00470098[rank],
                              pilotName);
            if (pilot == 8 ||
                g_stCampaignState_0059ca50
                    .personalityDeathMission[pilot] == 0) {
                DrawFormattedText(g_szChalkBoardScoreFormat_00470658,
                                  230, rowY,
                                  g_aPilotRecords_005988d0[pilot].missions,
                                  280,
                                  g_aPilotRecords_005988d0[pilot].kills);
            } else {
                DrawFormattedText(g_szChalkBoardKiaFormat_00470668,
                                  230, rowY,
                                  g_szKilledInAction_00470664);
            }
            index++;
            rowY = (short)(rowY + 16);
        } while (index < 9);
        DIBslam();
        DIBslamReal();
    } while (done == 0);

    ReleaseTextFont(3);
    FreePacketAndClear((int *)&background, 0);
    g_pCurrentTextContext_0059af8c = previousContext;
    ClearInputKeyStatePreservingModifiers();
    g_bInputMode_0059a848 = savedInputMode;
    FlushInputEvents();
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
short __stdcall ReadPacketSectionData(PacketSectionHandle *handle,
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
void CheckHeapBlockSignature(unsigned char *shape)
{
    if (*(int *)(shape - 8) != 0x6666656a)
        exit_squadron(g_szInvalidShapeAllocation_00470d18);
}

/* Function start: 0x4408C0 */
unsigned char *GetPreparedShapeData(unsigned char *shape)
{
    return *(unsigned char **)(shape - 4);
}

/* Function start: 0x4408D0 */
short __stdcall GetShapeFrameCount(unsigned char *shape)
{
    CheckHeapBlockSignature(shape);
    return (short)((*(unsigned short *)(shape + 4) >> 2) - 1);
}

/* Function start: 0x4408F0 */
void GetShapeFrameExtents(unsigned char *shape, short frame,
                          short *width, short *height,
                          short *leftExtent, short *topExtent)
{
    short *frameHeader;
    int frameOffset;
    int rightExtent;
    int left;
    int top;
    int bottom;

    frameOffset = (int)(short)(frame * 4 + 4);
    if (frameOffset < (int)*(unsigned short *)(shape + 4)) {
        frameHeader = (short *)(shape + *(int *)(shape + frameOffset));
        rightExtent = *frameHeader++;
        left = *frameHeader++;
        top = *frameHeader++;
        bottom = *frameHeader;
        *width = (short)(left + rightExtent + 1);
        *height = (short)(top + bottom + 1);
        *leftExtent = (short)left;
        *topExtent = (short)top;
    }
}

/* Function start: 0x440960 */
void DecodeShapeFrame(unsigned char *shape, short frame,
                      unsigned char *bitmap, int width, short height,
                      int leftExtent, int topExtent)
{
    unsigned char *commands;
    unsigned char *runData;
    unsigned char *destination;
    unsigned char code;
    unsigned char colour;
    volatile unsigned short rowCode;
    unsigned short runLength;
    unsigned short copyLength;
    short maximumX;
    short maximumY;
    int frameOffset;
    int x;
    int y;
    int runRight;
    int skip;

    if (shape == 0 || frame < 0)
        return;
    frameOffset = (int)(short)(frame * 4 + 4);
    if (frameOffset >= (int)*(unsigned short *)(shape + 4))
        return;

    maximumX = (short)(width - 1);
    commands = shape + *(int *)(shape + frameOffset) + 8;
    maximumY = (short)(height - 1);
    rowCode = *(unsigned short *)commands;
    commands += 2;
    while (rowCode != 0) {
        x = leftExtent + *(short *)commands;
        y = topExtent + *(short *)(commands + 2);
        destination = bitmap + y * width + x;
        commands += 4;
        if ((rowCode & 1) != 0) {
            rowCode >>= 1;
            while (rowCode != 0) {
                code = *commands;
                commands++;
                if ((code & 1) != 0) {
                    code >>= 1;
                    colour = *commands;
                    commands++;
                    runLength = code;
                    rowCode = (unsigned short)(rowCode - runLength);
                    if (y >= 0 && y <= maximumY) {
                        runRight = x + runLength - 1;
                        if (x <= maximumX && runRight >= 0) {
                            skip = 0;
                            copyLength = runLength;
                            if (x < 0) {
                                skip = -x;
                                copyLength =
                                    (unsigned short)(copyLength + x);
                            }
                            if (maximumX < runRight)
                                copyLength = (unsigned short)(copyLength -
                                                       runRight + maximumX);
                            memset(destination + skip, colour,
                                   (short)copyLength);
                        }
                    }
                } else {
                    code >>= 1;
                    runLength = code;
                    rowCode = (unsigned short)(rowCode - runLength);
                    runData = commands;
                    if (y >= 0 && y <= maximumY) {
                        runRight = x + runLength - 1;
                        if (x <= maximumX && runRight >= 0) {
                            skip = 0;
                            copyLength = runLength;
                            if (x < 0) {
                                skip = -x;
                                copyLength =
                                    (unsigned short)(copyLength + x);
                            }
                            if (maximumX < runRight)
                                copyLength = (unsigned short)(copyLength -
                                                       runRight + maximumX);
                            memcpy(destination + skip, runData + skip,
                                   (short)copyLength);
                        }
                    }
                    commands = runData + runLength;
                }
                x += runLength;
                destination += runLength;
            }
        } else {
            rowCode >>= 1;
            if (y >= 0 && y <= maximumY) {
                runRight = x + rowCode - 1;
                if (x <= maximumX && runRight >= 0) {
                    skip = 0;
                    copyLength = rowCode;
                    if (x < 0) {
                        skip = -x;
                        copyLength = (unsigned short)(copyLength + x);
                    }
                    if (maximumX < runRight)
                        copyLength = (unsigned short)(copyLength -
                                                     runRight + maximumX);
                    memcpy(destination + skip, commands + skip,
                           (short)copyLength);
                }
            }
            commands += rowCode;
        }
        rowCode = *(unsigned short *)commands;
        commands += 2;
    }
}

/* Function start: 0x440BE0 */
unsigned int SignExtendClipCoord(volatile int v)
{
    if ((unsigned short)v < 0xfdc0)
        return (unsigned short)v;
    return (int)(short)v;
}
