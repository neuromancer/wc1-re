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
    ShortRect currentBounds;
    ShortRect dirtyBounds[3];
    ShortRect previousBounds[3];
    Viewport dirtyDestination;
    Viewport dirtySource;
    signed char characterIds[2];
    signed char *animations[3];
    unsigned char savedInputMode;
    short characterMask;
    short eventType;
    short frame;
    short index;
    short region;
    short result;
    int firstFrame;
    int rosterOffset;
    int personality;

    result = 0;
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

    g_apRecRoomCharacterShapes_005988c0[0] =
        (unsigned char *)FetchDiskPacketRetrying(5, 11, 0);
    g_apRecRoomCharacterShapes_005988c0[1] = 0;
    g_apRecRoomCharacterShapes_005988c0[2] = 0;
    g_aRecRoomMenuRegions_004704a0[0].left = 94;
    g_aRecRoomMenuRegions_004704a0[0].top = 59;
    g_aRecRoomMenuRegions_004704a0[0].right = 94;
    g_aRecRoomMenuRegions_004704a0[0].bottom = 59;
    g_aRecRoomMenuRegions_004704a0[1].left = 161;
    g_aRecRoomMenuRegions_004704a0[1].top = 79;
    g_aRecRoomMenuRegions_004704a0[1].right = 161;
    g_aRecRoomMenuRegions_004704a0[1].bottom = 79;
    g_aRecRoomMenuRegions_004704a0[2].left = 202;
    g_aRecRoomMenuRegions_004704a0[2].top = 79;
    g_aRecRoomMenuRegions_004704a0[2].right = 202;
    g_aRecRoomMenuRegions_004704a0[2].bottom = 79;
    GetShapeFrameBounds(&g_aRecRoomMenuRegions_004704a0[0].left,
                        g_aRecRoomCharacterOrigins_00470490[0].x,
                        g_aRecRoomCharacterOrigins_00470490[0].y,
                        g_apRecRoomCharacterShapes_005988c0[0], 0);

    rosterOffset = ((int)g_stCampaignState_0059ca50.currentMission +
                    (int)g_stCampaignState_0059ca50.currentSeries * 4) * 2 - 8;
    characterIds[0] = (signed char)g_pRecRoomRoster_005988b8[rosterOffset];
    characterIds[1] = (signed char)g_pRecRoomRoster_005988b8[rosterOffset + 1];
    index = 0;
    do {
        personality = characterIds[index];
        if (personality >= 0 && personality < 8 &&
            g_stCampaignState_0059ca50
                .personalityDeathMission[personality] == 0) {
            g_apRecRoomCharacterShapes_005988c0[index + 1] =
                (unsigned char *)FetchDiskPacketRetrying(
                    5, (short)(personality + 3), 0);
            GetShapeFrameBounds(
                &g_aRecRoomMenuRegions_004704a0[index + 1].left,
                g_aRecRoomCharacterOrigins_00470490[index + 1].x,
                g_aRecRoomCharacterOrigins_00470490[index + 1].y,
                g_apRecRoomCharacterShapes_005988c0[index + 1], 0);
            sprintf(index == 0 ? g_szTalkToFirstPilot_00470570 :
                                 g_szTalkToSecondPilot_00470588,
                    index == 0 ? g_szTalkToPilotFormat1_004705ec :
                                 g_szTalkToPilotFormat2_004705fc,
                    g_apWingmanPilots_00598a30[personality]->callsign);
            characterMask |= (short)(2 >> index);
        } else {
            *(index == 0 ? g_szTalkToFirstPilot_00470570 :
                           g_szTalkToSecondPilot_00470588) = '\0';
        }
        index++;
    } while (index < 2);
    FreePacketAndClear((int *)&g_pRecRoomRoster_005988b8, 0);

    InitializeRoomViewports();
    InitializeRoomMenu(g_aRecRoomMenuRegions_004704a0,
                       g_apszRecRoomMenuLabels_004704f8,
                       &g_stRoomScreenViewport_005988a0,
                       g_szDefaultTextBuffer_005a7590, 2);
    g_pRecRoomBackgroundShape_00598a50 =
        (unsigned char *)FetchDiskPacketRetrying(5, 0, 0);
    init_constellation(0);
    g_stConstellationViewport_005a6b40 = DAT_005a76b0;
    SetViewportRect(&g_stConstellationViewport_005a6b40,
                    54, 35, 146, 72);
    InitializeConstellationField(&g_stConstellationViewport_005a6b40,
                                 -1, 6);
    animations[0] = g_apRecRoomAnimations_00470458[9];
    animations[1] = characterIds[0] >= 0 && characterIds[0] < 8 ?
        g_apRecRoomAnimations_00470458[characterIds[0]] : 0;
    animations[2] = characterIds[1] >= 0 && characterIds[1] < 8 ?
        g_apRecRoomAnimations_00470458[characterIds[1]] : 0;
    index = 0;
    do {
        previousBounds[index].left =
            g_aRecRoomMenuRegions_004704a0[index].left;
        previousBounds[index].top =
            g_aRecRoomMenuRegions_004704a0[index].top;
        previousBounds[index].right =
            g_aRecRoomMenuRegions_004704a0[index].right;
        previousBounds[index].bottom =
            g_aRecRoomMenuRegions_004704a0[index].bottom;
        dirtyBounds[index] = previousBounds[index];
        index++;
    } while (index < 3);
    firstFrame = 1;

    savedInputMode = g_bInputMode_0059a848;
    g_bInputMode_0059a848 = 1;
    g_nMenuPointerSpeed_0046af58 = 1;
    DAT_0059ab23 = &g_stRoomDisplayViewport_00598a60;
    EnterAllocationScope();
    WarpMouseTo(160, 100);
    SetFrameTimerPeriodDirect(0);
    FlushInputEvents();

    while (result == 0) {
        PumpWindowMessages();
        if (IsFrameTickElapsed() != 0) {
            DrawConstellationField();
            DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                              g_pRecRoomBackgroundShape_00598a50, 0);
            if (characterMask != 0)
                DrawSpriteDefault(&DAT_005a76b0, 158, 128,
                                  g_pRecRoomBackgroundShape_00598a50,
                                  characterMask);
            index = 0;
            do {
                if (g_apRecRoomCharacterShapes_005988c0[index] != 0 &&
                    animations[index] != 0) {
                    if (*animations[index] == -1) {
                        if (index == 0) {
                            frame = (short)(RandomInRange(0, 3) + 9);
                            if (frame == 11 && RandomInRange(0, 3) != 0)
                                frame = 10;
                            animations[index] =
                                g_apRecRoomAnimations_00470458[frame];
                        } else {
                            animations[index] =
                                g_apRecRoomAnimations_00470458[
                                    characterIds[index - 1]];
                        }
                    }
                    frame = (short)*animations[index];
                    animations[index]++;
                    DrawSpriteDefault(
                        &DAT_005a76b0,
                        g_aRecRoomCharacterOrigins_00470490[index].x,
                        g_aRecRoomCharacterOrigins_00470490[index].y,
                        g_apRecRoomCharacterShapes_005988c0[index], frame);
                    GetShapeFrameBounds(
                        &currentBounds.left,
                        g_aRecRoomCharacterOrigins_00470490[index].x,
                        g_aRecRoomCharacterOrigins_00470490[index].y,
                        g_apRecRoomCharacterShapes_005988c0[index], frame);
                    dirtyBounds[index].left = MinShort(
                        previousBounds[index].left, currentBounds.left);
                    dirtyBounds[index].top = MinShort(
                        previousBounds[index].top, currentBounds.top);
                    dirtyBounds[index].right = MaxShort(
                        previousBounds[index].right, currentBounds.right);
                    dirtyBounds[index].bottom = MaxShort(
                        previousBounds[index].bottom, currentBounds.bottom);
                    previousBounds[index] = currentBounds;
                }
                index++;
            } while (index < 3);
            if (firstFrame != 0) {
                CopyViewportContents(&DAT_005a76b0,
                                     &g_stRoomScreenViewport_005988a0);
                firstFrame = 0;
            } else {
                dirtySource = DAT_005a76b0;
                dirtyDestination = g_stRoomScreenViewport_005988a0;
                SetViewportRect(&dirtySource, 54, 35, 146, 72);
                SetViewportRect(&dirtyDestination, 54, 35, 146, 72);
                CopyViewportContents(&dirtySource, &dirtyDestination);
                index = 0;
                do {
                    if (g_apRecRoomCharacterShapes_005988c0[index] != 0) {
                        dirtySource = DAT_005a76b0;
                        dirtyDestination =
                            g_stRoomScreenViewport_005988a0;
                        SetViewportRect(
                            &dirtySource,
                            (unsigned short)dirtyBounds[index].left,
                            (unsigned short)dirtyBounds[index].top,
                            (unsigned short)dirtyBounds[index].right,
                            (unsigned short)dirtyBounds[index].bottom);
                        SetViewportRect(
                            &dirtyDestination,
                            (unsigned short)dirtyBounds[index].left,
                            (unsigned short)dirtyBounds[index].top,
                            (unsigned short)dirtyBounds[index].right,
                            (unsigned short)dirtyBounds[index].bottom);
                        CopyViewportContents(&dirtySource,
                                             &dirtyDestination);
                    }
                    index++;
                } while (index < 3);
            }
            RefreshRoomMenuLabel();
            SetFrameTimerPeriodDirect(9);
        }

        region = -1;
        eventType = PollInputEvent(&event, 0xff);
        if (eventType == 13) {
            UpdateRoomMenuCursor();
        } else if (eventType == 2 || eventType == 10) {
            region = FindMenuRegionAtPoint(
                event.x, event.y, g_aRecRoomMenuRegions_004704a0);
            if (region == 3) {
                FlushInputEvents();
                ShowChalkBoard();
                SetFrameTimerPeriodDirect(0);
            } else if (region == 4 || region == 5) {
                result = region;
            }
        } else if (eventType == 3 || eventType == 5) {
            ClearInputKeyStatePreservingModifiers();
            if ((short)event.value == 0x1c ||
                (short)event.value == 0x39) {
                region = FindMenuRegionAtPoint(
                    g_nMouseX_0059ab10, g_nMouseY_0059ab12,
                    g_aRecRoomMenuRegions_004704a0);
                if (region == 3) {
                    FlushInputEvents();
                    ShowChalkBoard();
                    SetFrameTimerPeriodDirect(0);
                } else if (region == 4 || region == 5) {
                    result = region;
                }
            } else {
                MoveMenuPointerFromKeyboard(&event);
            }
        }
        if (region >= 0 && region < 3 &&
            g_apRecRoomCharacterShapes_005988c0[region] != 0) {
            FlushInputEvents();
            free_constellation();
            FreePacketAndClear(
                (int *)&g_pRecRoomBackgroundShape_00598a50, 0);
            DAT_005a76b0.bottom = 127;
            DAT_005a6baa = 24;
            DAT_005a6bae = 151;
            InitializeConversationText();
            ClearViewport(&g_stRoomScreenViewport_005988a0,
                          DAT_0046999c);
            g_pConversationBackdropShape_00598c04 =
                (unsigned char *)FetchDiskPacketRetrying(5, 1, 0);
            SceneDirector(2,
                          g_apRecRoomSceneData_00598ae0[region],
                          g_apRecRoomTextData_00598aa0[region]);
            DAT_0059ab58 = 0;
            SetEventManagerPump(PollMenuInputDevices);
            FreePacketAndClear(
                (int *)&g_pConversationBackdropShape_00598c04, 0);
            SetFrameTimerPeriodDirect(1);
            DAT_005a76b0.bottom = 199;
            DAT_005a6baa = 0;
            DAT_005a6bae = 199;
            init_constellation(0);
            g_stConstellationViewport_005a6b40 = DAT_005a76b0;
            SetViewportRect(&g_stConstellationViewport_005a6b40,
                            54, 35, 146, 72);
            InitializeConstellationField(
                &g_stConstellationViewport_005a6b40, -1, 6);
            g_pRecRoomBackgroundShape_00598a50 =
                (unsigned char *)FetchDiskPacketRetrying(5, 0, 0);
            ClearViewport(&g_stRoomScreenViewport_005988a0,
                          DAT_0046999c);
            firstFrame = 1;
        }
        ShowMemoryStatusDebug();
        DIBslam();
        DIBslamReal();
    }

    LeaveAllocationScope();
    *(short *)&g_aInputDeviceSamples_005a81f0[2].x =
        g_nSavedRoomControllerX_005988b4;
    EventManagerHook(0);
    SetEventManagerPump(0);
    index = 0;
    do {
        FreePacketAndClear(
            (int *)&g_apRecRoomCharacterShapes_005988c0[index], 0);
        index++;
    } while (index < 3);
    FreePacketAndClear((int *)&g_pRecRoomBackgroundShape_00598a50, 0);
    free_constellation();
    ReleaseTextFont(0);
    FreePacketAndClear((int *)&g_pBriefingPacket_00598aec, 0);
    free_viewport(&DAT_005a76b0);
    g_bInputMode_0059a848 = savedInputMode;
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
unsigned int ReadPacketSectionData(PacketSectionHandle *handle,
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
void CheckHeapBlockSignature(int p)
{
    if (*(int *)(p - 8) != 0x6666656a)
        exit_squadron("not jefftep");
}

/* Function start: 0x4408C0 */
unsigned int GetHeapBlockSize(int p)
{
    return *(unsigned int *)(p - 4);
}

/* Function start: 0x440BE0 */
unsigned int SignExtendClipCoord(unsigned short v)
{
    if (v < 0xfdc0)
        return v;
    return (int)(short)v;
}
