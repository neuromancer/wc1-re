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
        apWingmanPilots[pilot] =
            &aPilotRecords[pilot];
        pilot++;
    } while (pilot < 8);
    stCampaignState.currentPilot =
        &aPilotRecords[8];
    pCurrentCampaignDate =
        &stCampaignState.currentDate;
    pElapsedCampaignDate =
        &stCampaignState.elapsedDate;
}

/* Function start: 0x43F690 */
void ClearRoomMenuLabel(void)
{
    pszCurrentRoomMenuLabel = 0;
}

/* Function start: 0x43F6A0 */
int IsRoomMenuLabelEmpty(void)
{
    return pszCurrentRoomMenuLabel == 0;
}

/* Function start: 0x43F6B0 */
void DrawRoomMenuLabel(TextContext *context, const char *label)
{
    DosStrcpy(szTextScratchBuffer, label);
    SetTextContext(context);
    FormatTextBufferFromStart(szRoomMenuLabelFormat,
                              0, 188,
                              szTextScratchBuffer);
}

/* Function start: 0x43F6F0 */
void RefreshRoomMenuLabel(void)
{
    if (IsRoomMenuLabelEmpty())
        pszCurrentRoomMenuLabel =
            pszBlankRoomMenuLabel;
    DrawRoomMenuLabel(&stRoomMenuTextContext,
                      pszCurrentRoomMenuLabel);
}

/* Function start: 0x43F720 */
void ClearRoomMenuCursorFrame(void)
{
    nRoomMenuCursorFrame = 0;
}

/* Function start: 0x43F730 */
void SelectRoomMenuLabel(short i)
{
    const char *label;

    label = ppszRoomMenuLabels[i];
    if (label != 0)
        pszCurrentRoomMenuLabel = label;
}

/* Function start: 0x43F750 */
void InitializeRoomMenu(TitleMenuRegion *regions, char **labels,
                        Viewport *viewport, char *text,
                        unsigned char alignment)
{
    pRoomMenuRegions = regions;
    ppszRoomMenuLabels = labels;
    pszCurrentRoomMenuLabel =
        pszBlankRoomMenuLabel;
    stRoomMenuTextContext.viewport = viewport;
    stRoomMenuTextContext.text = text;
    stRoomMenuTextContext.textCursor = text;
    stRoomMenuTextContext.alignment = alignment;
    *text = szEmptyRoomMenuLabel[0];
    InitializeTextContextFromFont(&stRoomMenuTextContext,
                                  0, cViewportClearColour,
                                  -1);
    ClearRoomMenuCursorFrame();
}

/* Function start: 0x43F7C0 */
int FindMenuRegionAtPoint(short x, short y,
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
    stRoomScreenViewport = stScreen;
    stSceneBuffer.right = 319;
    stSceneBuffer.bottom = 199;
    stSceneBuffer.left = 0;
    stSceneBuffer.top = 0;
    AllocateViewport(&stSceneBuffer, cBlackColour, 0);
    stRoomMouseViewport = stSceneBuffer;
    stRoomDisplayViewport = stRoomScreenViewport;
    SetEventManagerPump(PollMenuInputDevices);
    EventManagerHook(UpdateRoomMenuCursor);
    nSavedRoomControllerX =
        nMenuInputRepeatDelay;
    nMenuInputRepeatDelay = 6;
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
    apszRecRoomMenuLabels[0] =
        apszRecRoomBaseLabels[0];
    apszRecRoomMenuLabels[1] =
        apszRecRoomBaseLabels[1];
    apszRecRoomMenuLabels[2] =
        apszRecRoomBaseLabels[2];

    PreloadMusicTrackHook(30);
    spacetrack(30, 2, 1);
    FlushInputEvents();
    LoadBriefingData((short)stCampaignState.currentSeries,
                     (short)stCampaignState.currentMission);
    pRecRoomRoster = LoadPacketAllocated(
        asCampaignPilotFiles[nCampaignDataSet], 2);

    animationIds[0] = (signed char)(RandomInRange(0, 3) + 9);
    apRecRoomCharacterShapes[0] = 0;
    apRecRoomCharacterShapes[1] = 0;
    apRecRoomCharacterShapes[2] = 0;
    aRecRoomMenuRegions[0].left = 400;
    aRecRoomMenuRegions[0].top = 400;
    aRecRoomMenuRegions[0].right = 401;
    aRecRoomMenuRegions[0].bottom = 401;
    *(ShortRect *)&aRecRoomMenuRegions[1].left =
        *(ShortRect *)&aRecRoomMenuRegions[0].left;
    *(ShortRect *)&aRecRoomMenuRegions[2].left =
        *(ShortRect *)&aRecRoomMenuRegions[0].left;

    apRecRoomCharacterShapes[0] =
        FetchDiskPacketRetrying(5, 11, 0);
    GetShapeFrameBounds(&aRecRoomMenuRegions[0].left,
                        aRecRoomCharacterOrigins[0].x,
                        aRecRoomCharacterOrigins[0].y,
                        apRecRoomCharacterShapes[0], 0);

    rosterOffset = ((int)stCampaignState.currentMission +
                    (int)stCampaignState.currentSeries * 4) *
                       2 - 8;
    animationIds[1] =
        (signed char)pRecRoomRoster[rosterOffset];
    if (animationIds[1] != -1) {
        personality = (int)animationIds[1];
        if (stCampaignState
                .personalityDeathMission[personality] == 0) {
            apRecRoomCharacterShapes[1] =
                FetchDiskPacketRetrying(
                    5, (short)(personality + 3), 0);
            GetShapeFrameBounds(
                &aRecRoomMenuRegions[1].left,
                aRecRoomCharacterOrigins[1].x,
                aRecRoomCharacterOrigins[1].y,
                apRecRoomCharacterShapes[1], 0);
            sprintf(szTalkToFirstPilot,
                    szTalkToPilotFormat1,
                    apWingmanPilots[personality]->callsign);
        } else {
            sprintf(szTalkToFirstPilot,
                    szTalkToPilotFormat1 + 12);
        }
    }

    animationIds[2] =
        (signed char)pRecRoomRoster[rosterOffset + 1];
    if (animationIds[1] != -1) {
        personality = (int)animationIds[2];
        if (stCampaignState
                .personalityDeathMission[personality] == 0) {
            apRecRoomCharacterShapes[2] =
                FetchDiskPacketRetrying(
                    5, (short)(personality + 3), 0);
            GetShapeFrameBounds(
                &aRecRoomMenuRegions[2].left,
                aRecRoomCharacterOrigins[2].x,
                aRecRoomCharacterOrigins[2].y,
                apRecRoomCharacterShapes[2], 0);
            sprintf(szTalkToSecondPilot,
                    szTalkToPilotFormat2,
                    apWingmanPilots[personality]->callsign);
        } else {
            sprintf(szTalkToSecondPilot,
                    szTalkToPilotFormat2 + 12);
        }
    }

    ReleasePacketHandle(pRecRoomRoster);
    if (apRecRoomCharacterShapes[2] != 0)
        characterMask = 1;
    if (apRecRoomCharacterShapes[1] != 0) {
        characterMask = 2;
        if (apRecRoomCharacterShapes[2] != 0)
            characterMask = 3;
    }

    SetViewportRect(&stSceneBuffer, 0, 0, 319, 199);
    InitializeRoomViewports();
    init_constellation(0);
    stConstellationViewport = stSceneBuffer;
    SetViewportRect(&stConstellationViewport,
                    54, 35, 146, 72);
    InitializeConstellationField(&stConstellationViewport,
                                 -1, 6);

    animations[0] = abShotglassIdleAnimation;
    animations[1] =
        apRecRoomAnimations[(int)animationIds[1]];
    animations[2] =
        apRecRoomAnimations[(int)animationIds[2]];
    InitializeRoomMenu(aRecRoomMenuRegions,
                       apszRecRoomMenuLabels,
                       &stRoomScreenViewport,
                       szDefaultTextBuffer, 2);

    bottomSource = stSceneBuffer;
    SetViewportRect(&bottomSource, 0, 187, 319, 199);
    bottomDestination = stRoomScreenViewport;
    SetViewportRect(&bottomDestination, 0, 187, 319, 199);
    pRecRoomBackgroundShape =
        FetchDiskPacketRetrying(5, 0, 0);
    nMenuPointerSpeed = 1;
    bInputMode = 1;
    stMouseCursorState.viewport = &stRoomDisplayViewport;

    pilotWork = stSceneBuffer;
    shotglassWork = stSceneBuffer;
    pilotDestination = stRoomScreenViewport;
    shotglassDestination = stRoomScreenViewport;

    if (apRecRoomCharacterShapes[2] != 0) {
        GetShapeFrameBounds(
            &secondPilotBounds.left,
            aRecRoomCharacterOrigins[2].x,
            aRecRoomCharacterOrigins[2].y,
            apRecRoomCharacterShapes[2],
            (short)*animations[2]);
    }
    if (apRecRoomCharacterShapes[1] != 0) {
        GetShapeFrameBounds(
            &firstPilotBounds.left,
            aRecRoomCharacterOrigins[1].x,
            aRecRoomCharacterOrigins[1].y,
            apRecRoomCharacterShapes[1],
            (short)*animations[1]);
        *(ShortRect *)&pilotWork.left = firstPilotBounds;
        if (apRecRoomCharacterShapes[2] != 0) {
            UnionRectBounds((ShortRect *)&pilotWork.left,
                            &firstPilotBounds, &secondPilotBounds);
        }
    } else if (apRecRoomCharacterShapes[2] != 0) {
        *(ShortRect *)&pilotWork.left = secondPilotBounds;
    }
    if (apRecRoomCharacterShapes[1] != 0 ||
        apRecRoomCharacterShapes[2] != 0) {
        *(ShortRect *)&pilotDestination.left =
            *(ShortRect *)&pilotWork.left;
    }

    WarpMouseTo(160, 100);
    GetShapeFrameBounds(
        &shotglassWork.left,
        aRecRoomCharacterOrigins[0].x,
        aRecRoomCharacterOrigins[0].y,
        apRecRoomCharacterShapes[0],
        (short)*animations[0]);

    while (result == 0) {
        if (firstFrame == 0) {
            DrawSpriteDefault(&stSceneBuffer, 0, 0,
                              pRecRoomBackgroundShape, 0);
            if (characterMask != 0) {
                DrawSpriteDefault(&stSceneBuffer, 158, 128,
                                  pRecRoomBackgroundShape,
                                  characterMask);
            }
            SetFrameTimerPeriodDirect(0);
        }

        if (IsFrameTickElapsed() != 0) {
            DrawSpriteDefault(&pilotWork, 0, 0,
                              pRecRoomBackgroundShape, 0);
            for (index = 0; index < 3; index++) {
                if (apRecRoomCharacterShapes[index] != 0) {
                    if (*animations[index] == -1) {
                        if (index == 0) {
                            animationIds[0] =
                                (signed char)(RandomInRange(0, 3) + 9);
                            if (animationIds[0] == 11 &&
                                RandomInRange(0, 3) != 0)
                                animationIds[0]--;
                        }
                        animations[index] =
                            apRecRoomAnimations[
                                (int)animationIds[index]];
                    }

                    if (index > 0) {
                        DrawSpriteDefault(
                            &pilotWork,
                            aRecRoomCharacterOrigins[index].x,
                            aRecRoomCharacterOrigins[index].y,
                            apRecRoomCharacterShapes[index], 0);
                        frame = (short)*animations[index]++;
                        DrawSpriteDefault(
                            &pilotWork,
                            aRecRoomCharacterOrigins[index].x,
                            aRecRoomCharacterOrigins[index].y,
                            apRecRoomCharacterShapes[index],
                            frame);
                    } else {
                        UnionRectBounds(
                            (ShortRect *)&shotglassWork.left,
                            (ShortRect *)&shotglassWork.left,
                            (ShortRect *)&stConstellationViewport.left);
                        DrawConstellationField();
                        DrawSpriteDefault(
                            &shotglassWork, 0, 0,
                            pRecRoomBackgroundShape, 0);
                        frame = (short)*animations[index]++;
                        DrawSpriteDefault(
                            &shotglassWork,
                            aRecRoomCharacterOrigins[index].x,
                            aRecRoomCharacterOrigins[index].y,
                            apRecRoomCharacterShapes[index],
                            frame);
                        GetShapeFrameBounds(
                            &nextFrameBounds.left,
                            aRecRoomCharacterOrigins[index].x,
                            aRecRoomCharacterOrigins[index].y,
                            apRecRoomCharacterShapes[index],
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
            }

            if (firstFrame == 0) {
                firstFrame = 1;
                if (bPanRoomTransition != 0) {
                    PanToScreen(&stSceneBuffer,
                                &stRoomScreenViewport);
                    bPanRoomTransition = 0;
                } else {
                    CopyViewportContents(
                        &stSceneBuffer,
                        &stRoomScreenViewport);
                }
                EnterAllocationScope();
            } else if (apRecRoomCharacterShapes[1] != 0 ||
                       apRecRoomCharacterShapes[2] != 0) {
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
                event.x, event.y, aRecRoomMenuRegions);
            LeaveAllocationScope();
            if (region >= 0 && region <= 2) {
                if (apRecRoomCharacterShapes[region] != 0) {
                    free_constellation();
                    ReleasePacketHandle(pRecRoomBackgroundShape);
                    stSceneBuffer.bottom = 127;
                    stScreen.top = 24;
                    stScreen.bottom = 151;
                    InitializeConversationText();
                    ClearViewport(&stRoomScreenViewport,
                                  cBlackColour);
                    pConversationBackdropShape =
                        FetchDiskPacketRetrying(
                            5, 1, 0);
                    SceneDirector(
                        2,
                        apRecRoomSceneData[region],
                        apRecRoomTextData[region]);
                    bEscapePressed = 0;
                    SetEventManagerPump(PollMenuInputDevices);
                    FreePacketAndClear(
                        &pConversationBackdropShape, 0);
                    SetFrameTimerPeriodDirect(1);
                    stScreen.top = 0;
                    stScreen.bottom = 199;
                    stSceneBuffer.bottom = 199;
                    stConstellationViewport =
                        stSceneBuffer;
                    stConstellationViewport.left = 54;
                    stConstellationViewport.top = 35;
                    stConstellationViewport.right = 146;
                    stConstellationViewport.bottom = 72;
                    init_constellation(0);
                    InitializeConstellationField(
                        &stConstellationViewport, -1, 6);
                    pRecRoomBackgroundShape =
                        FetchDiskPacketRetrying(
                            5, 0, 0);
                    ClearViewport(&stRoomScreenViewport,
                                  cBlackColour);
                }
            } else if (region == 3) {
                FlushInputEvents();
                if ((int)(nTickCount60Hz - lastChalkboardTick) >
                    nInputTickScale) {
                    ShowChalkBoard();
                    ClearViewport(&stSceneBuffer, cBlackColour);
                    lastChalkboardTick = (int)nTickCount60Hz;
                }
            } else if (region == 4 || region == 5) {
                result = region;
            } else {
                clicked = 0;
                EnterAllocationScope();
            }

            stRoomMouseViewport = stScreen;
            stMouseCursorState.viewport = &stRoomMouseViewport;
            bInputMode = 1;
            if (clicked != 0)
                firstFrame = 0;
        }

        ShowMemoryStatusDebug();
        DIBslam();
        DIBslamReal();
    }

    nMenuPointerSpeed = 2;
    nMenuInputRepeatDelay =
        nSavedRoomControllerX;
    EventManagerHook(0);
    ReleasePacketHandle(apRecRoomCharacterShapes[0]);
    ReleasePacketHandle(apRecRoomCharacterShapes[1]);
    ReleasePacketHandle(apRecRoomCharacterShapes[2]);
    free_constellation();
    ReleasePacketHandle(pRecRoomBackgroundShape);
    ReleaseTextFont(0);
    ReleasePacketHandle(pBriefingPacket);
    ClearViewport(&stScreen, cBlackColour);
    free_viewport(&stSceneBuffer);
    bEscapePressed = 0;
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

    for (index = 0; index < 9; index++) {
        for (other = index; other < 9; other++) {
            pilot = asChalkBoardPilotOrder[index];
            swap = asChalkBoardPilotOrder[other];
            score = aPilotRecords[pilot].kills * 1000 -
                    aPilotRecords[pilot].missions + 1;
            otherScore = aPilotRecords[swap].kills * 1000 -
                         aPilotRecords[swap].missions + 1;
            if (score < otherScore) {
                asChalkBoardPilotOrder[index] = swap;
                asChalkBoardPilotOrder[other] = pilot;
            }
        }
    }

    memset(&context, 0, sizeof(context));
    stChalkBoardDate = *pCurrentCampaignDate;
    background = FetchDiskPacketRetrying(5, 2, 0);
    previousContext = pCurrentTextContext;
    stModalSourceViewport = stScreen;
    context.viewport = &stModalSourceViewport;
    context.text = szDefaultTextBuffer;
    context.textCursor = szDefaultTextBuffer;
    InitializeTextContextFromFont(&context, 3,
                                  cViewportClearColour, -1);
    savedInputMode = bInputMode;
    bInputMode = 1;
    done = 0;
    do {
        PumpWindowMessages();
        if (PeekInputEvent(&event, 10) != 0 ||
            PeekInputEvent(&event, 2) != 0 ||
            PeekInputEvent(&event, 3) != 0)
            done = 1;

        DrawSpriteDefault(&stModalSourceViewport,
                          0, 0, background, 0);
        context.alignment = 2;
        SetTextContext(&context);
        FormatTextBufferFromStart(szChalkBoardHeading,
                                  0, 10);
        DrawFormattedText(szChalkBoardPilotHeading,
                          60, 24);
        DrawFormattedText(szChalkBoardScoreHeading,
                          185, 24);
        rowY = 46;
        index = 0;
        for (; index < 9; index++, rowY = (short)(rowY + 16)) {
            pilot = asChalkBoardPilotOrder[index];
            DosStrcpy(pilotName, aPilotRecords[pilot].name);
            _strupr(pilotName);
            rank = aPilotRecords[pilot].rank;
            if (rank < 0)
                rank = 0;
            if (rank > 4)
                rank = 4;
            DrawFormattedText(szChalkBoardPilotFormat,
                              10, rowY,
                              apszPilotRankNames[rank],
                              pilotName);
            if (pilot == 8 ||
                stCampaignState
                    .personalityDeathMission[pilot] == 0) {
                DrawFormattedText(szChalkBoardScoreFormat,
                                  230, rowY,
                                  aPilotRecords[pilot].missions,
                                  280,
                                  aPilotRecords[pilot].kills);
            } else {
                DrawFormattedText(szChalkBoardKiaFormat,
                                  230, rowY,
                                  szKilledInAction);
            }
        }
        DIBslam();
        DIBslamReal();
    } while (done == 0);

    ReleaseTextFont(3);
    ReleasePacketHandle(background);
    pCurrentTextContext = previousContext;
    ClearInputKeyStatePreservingModifiers();
    bInputMode = savedInputMode;
    FlushInputEvents();
}

/* Function start: 0x440800 */
void ResetCampaignData(void)
{
    memcpy(&stCampaignState,
           &stInitialCampaignState,
           sizeof(stCampaignState));
    memcpy(aPilotRecords,
           aInitialPilotRecords,
           sizeof(aPilotRecords));
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
        exit_squadron(szInvalidShapeAllocation);
}

/* Function start: 0x4408C0 */
unsigned char *GetPreparedShapeData(unsigned char *shape)
{
#ifdef WC1_SDL
    return *(unsigned char **)(shape - 8 - sizeof(unsigned char *));
#else
    return *(unsigned char **)(shape - 4);
#endif
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
    int rightExtent;
#ifndef WC1_SDL
    short *frameHeader;
#endif
    int frameOffset;
    int left;
    int top;
    int bottom;
#ifdef WC1_SDL
    short frameExtents[4];
#endif

    frameOffset = (int)(short)(frame * 4 + 4);
    if (frameOffset < (int)*(unsigned short *)(shape + 4)) {
#ifdef WC1_SDL
        memcpy(frameExtents, shape + *(int *)(shape + frameOffset),
               sizeof(frameExtents));
        rightExtent = frameExtents[0];
        left = frameExtents[1];
        top = frameExtents[2];
        bottom = frameExtents[3];
#else
        frameHeader = (short *)(shape + *(int *)(shape + frameOffset));
        rightExtent = *frameHeader++;
        left = *frameHeader++;
        top = *frameHeader++;
        bottom = *frameHeader;
#endif
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
#ifdef WC1_SDL
    short coordinate;
#endif

    if (shape == 0 || frame < 0)
        return;
    frameOffset = (int)(short)(frame * 4 + 4);
    if (frameOffset >= (int)*(unsigned short *)(shape + 4))
        return;

    maximumX = (short)(width - 1);
    commands = shape + *(int *)(shape + frameOffset) + 8;
    maximumY = (short)(height - 1);
#ifdef WC1_SDL
    memcpy((void *)&rowCode, commands, sizeof(rowCode));
#else
    rowCode = *(unsigned short *)commands;
#endif
    commands += 2;
    while (rowCode != 0) {
#ifdef WC1_SDL
        memcpy(&coordinate, commands, sizeof(coordinate));
        x = leftExtent + coordinate;
        memcpy(&coordinate, commands + 2, sizeof(coordinate));
        y = topExtent + coordinate;
#else
        x = leftExtent + *(short *)commands;
        y = topExtent + *(short *)(commands + 2);
#endif
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
#ifdef WC1_SDL
        memcpy((void *)&rowCode, commands, sizeof(rowCode));
#else
        rowCode = *(unsigned short *)commands;
#endif
        commands += 2;
    }
}

/* Function start: 0x440BE0 */
int SignExtendClipCoord(volatile short v)
{
    if ((unsigned short)v < 0xfdc0)
        return (unsigned short)v;
    return (int)(short)v;
}
