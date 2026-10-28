/*
 *  Kill board, conversation scenes, and initial campaign state.
 *
 *  The original function window is recorded in docs/ORDER.md.
 *  Room state precedes the rank, menu-label, and chalk-board literals.
 */
#define WC1_KILLBRD_SOURCE
#include "wc1.h"

/* 0x00470090 */
static const char *pszBlankRoomMenuLabel =
    " ";
/* 0x00470098 */
const char *apszPilotRankNames[5] = {
    "2ND LT.",
    "1ST LT.",
    "CAPTAIN",
    "MAJOR",
    "LT. COL."
};
/* 0x004700b0 */
CampaignState stInitialCampaignState = {
    0,
    OBJECT_TYPE_HORNET,
    {0, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    0,
    1,
    0,
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1},
    {110, 2654},
    {6, 0},
    0,
    0,
    0,
    0
};
/* 0x00470108 */
static PilotRecord aInitialPilotRecords[9] = {
    {"TANAKA",   "SPIRIT",  3, 1, 11, 14, 1},
    {"ST.JOHN",  "HUNTER",  4, 2, 25, 32, 4},
    {"CHEN",     "BOSSMAN", 1, 3, 35, 37, 2},
    {"CASEY",    "ICEMAN",  0, 3, 28, 43, 1},
    {"DEVEREAUX", "ANGEL",   0, 2, 22, 20, 1},
    {"TAGGART",  "PALADIN", 2, 3, 42, 34, 2},
    {"MARSHALL", "MANIAC",  4, 0, 5, 6, 1},
    {"KHUMALO",  "KNIGHT",  3, 2, 18, 23, 3},
    {"PELLEY",   "GOBLIN",  0, 0, 0, 0, 0}
};
/* 0x00470260 */
static signed char abSpiritRecRoomAnimation[48] = {
    0, 0, 0, 0, 1, 1, 2, 2, 1, 1, 2, 3, 4, 3, 3, 4,
    3, 4, 3, 4, 5, 3, 4, 5, 3, 4, 3, 4, 3, 4, 5, 3,
    2, 2, 2, 3, 2, 2, 2, 1, 1, 1, -1, 0, 0, 0, 0, 0
};
/* 0x00470290 */
static signed char abHunterRecRoomAnimation[40] = {
    0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 2, 2,
    3, 3, 4, 4, 5, 3, 3, 4, 4, 5, 5, 5, 2, 2, 1, 1,
    0, 0, -1, 0, 0, 0, 0, 0
};
/* 0x004702b8 */
static signed char abAngelRecRoomAnimation[48] = {
    3, 4, 3, 3, 4, 3, 4, 4, 3, 4, 3, 4, 0, 0, 0, 2,
    0, 2, 0, 1, 0, 2, 0, 0, 2, 0, 1, 0, 2, 3, 3, 4,
    3, 4, 4, 5, 5, 5, 5, 5, -1, 0, 0, 0, 0, 0, 0, 0
};
/* 0x004702e8 */
static signed char abKnightRecRoomAnimation[40] = {
    0, 0, 0, 0, 0, 1, 0, 1, 0, 5, 0, 0, 1, 2, 3, 4,
    4, 3, 2, 2, 2, 3, 4, 4, 5, 5, 0, 0, 1, 0, 1, 2,
    1, 0, 1, -1, 0, 0, 0, 0
};
/* 0x00470310 */
static signed char abIcemanRecRoomAnimation[40] = {
    0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 3, 3, 3, 3,
    4, 4, 4, 5, 4, 4, 5, 4, 4, 5, 4, 4, 5, 4, 4, 3,
    3, 2, 2, 1, 0, 0, -1, 0
};
/* 0x00470338 */
static signed char abManiacRecRoomAnimation[24] = {
    0, 1, 2, 0, 1, 2, 0, 3, 2, 0, 1, 1,
    0, 4, 2, 0, 1, 2, 0, 5, 5, 5, 5, -1
};
/* 0x00470350 */
static signed char abPaladinRecRoomAnimation[56] = {
    0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 2, 2, 2, 3, 2,
    2, 3, 2, 3, 2, 3, 2, 4, 4, 2, 3, 2, 3, 2, 3, 2,
    3, 2, 3, 4, 4, 5, 5, 4, 4, 4, 4, 5, 5, 4, 2, 2,
    2, -1, 0, 0, 0, 0, 0, 0
};
/* 0x00470388 */
static signed char abBossmanRecRoomAnimation[48] = {
    3, 3, 3, 2, 3, 2, 3, 2, 3, 3, 2, 3, 1, 3, 1, 3,
    0, 3, 0, 3, 0, 2, 1, 2, 1, 3, 3, 4, 5, 5, 4, 5,
    4, 5, 1, 3, 1, 3, 1, 3, -1, 0, 0, 0, 0, 0, 0, 0
};
/* 0x004703b8 */
static signed char abShotglassIdleAnimation[24] = {
    0, 0, 0, 0, 0, 1, 1, 0, 0, 2, 2, 0,
    0, 3, 3, 3, -1, 0, 0, 0, 0, 0, 0, 0
};
/* 0x004703d0 */
static signed char abShotglassGlassAnimation[32] = {
    3, 6, 7, 8, 8, 9, 9, 10, 10, 10, 12, 11, 10, 12, 11, 10,
    12, 11, 10, 12, 11, 10, 13, 13, 13, 10, 9, 8, 8, 3, 3, -1
};
/* 0x004703f0 */
static signed char abShotglassPourAnimation[80] = {
    3, 3, 6, 6, 7, 8, 8, 14, 15, 15, 16, 17, 18, 18, 18, 18,
    17, 19, 19, 20, 20, 21, 20, 21, 20, 21, 20, 21, 20, 21, 20, 21,
    22, 22, 23, 24, 24, 25, 25, 26, 26, 27, 28, 28, 28, 29, 30, 31,
    31, 31, 31, 31, 32, 32, 31, 31, 31, 33, 33, 32, 34, 34, 35, 35,
    36, 36, 37, 37, 37, 38, 38, 39, 40, 3, 3, 3, -1, 0, 0, 0
};
/* 0x00470440 */
static signed char abShotglassWipeAnimation[24] = {
    3, 4, 3, 4, 5, 3, 4, 3, 5, 6, 3, 3,
    3, 3, 4, 3, 4, 3, -1, 0, 0, 0, 0, 0
};
/* 0x00470458 */
static signed char *apRecRoomAnimations[14] = {
    abSpiritRecRoomAnimation,
    abHunterRecRoomAnimation,
    abBossmanRecRoomAnimation,
    abIcemanRecRoomAnimation,
    abAngelRecRoomAnimation,
    abPaladinRecRoomAnimation,
    abManiacRecRoomAnimation,
    abKnightRecRoomAnimation,
    0,
    abShotglassIdleAnimation,
    abShotglassGlassAnimation,
    abShotglassPourAnimation,
    abShotglassWipeAnimation,
    0
};
/* 0x00470490 */
static ShortPoint aRecRoomCharacterOrigins[3] = {
    {94, 59}, {161, 79}, {202, 79}
};
/* 0x0047049c */
static ShortPoint stRecRoomPanelOrigin = {158, 28};
/* 0x004704a0 */
static TitleMenuRegion aRecRoomMenuRegions[7] = {
    { 1, 94, 59, 130, 95 },
    { 1, 161, 79, 180, 95 },
    { 1, 210, 79, 240, 95 },
    { 1, 180, 50, 250, 75 },
    { 1, 275, 50, 319, 135 },
    { 1, 0, 100, 120, 190 },
    { -1, 0, 0, 0, 0 }
};
/* 0x004704e8 */
static char *apszRecRoomBaseLabels[3] = {
    "Talk to SHOTGLASS.",
    "Talk to ??????????????",
    "Talk to ??????????????"
};
/* 0x004704f8 */
static char *apszRecRoomMenuLabels[6] = {
    0, 0, 0,
    "Check pilot scores",
    "Enter barracks",
    "Fly training mission"
};
/* 0x00470510 */
int bPanRoomTransition = 0;
/* 0x00470514 */
static CampaignDate stChalkBoardDate = {-1, -1};
/* 0x00470518 */
static short asChalkBoardPilotOrder[9] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8
};

unsigned char *pRecRoomBgShape; /* 0x00598a50 */
Viewport stRoomDisplayView; /* 0x00598a60 */
Viewport stCurrentRoomMouseView; /* 0x00598a80 */
unsigned char *apRecRoomTxtDataArray[3]; /* 0x00598aa0 */
RoomMenu stCurrentRoomMenu; /* 0x00598ab0 */
unsigned char *apRecRoomSceneDataList[3]; /* 0x00598ae0 */
unsigned char *pBriefingPacket; /* 0x00598aec */

/* Function start: 0x43F640 */
void CorrectPointers(void)
{
    short pilot;

    pilot = 0;
    do {
        apWingmanPilotsList[pilot] =
            &aPilotEntriesList[pilot];
        pilot++;
    } while (pilot < 8);
    stCampaignState.currentPilot =
        &aPilotEntriesList[8];
    pCurrentCampaignDate =
        &stCampaignState.currentDate;
    pElapsedCampaignDatePtr =
        &stCampaignState.elapsedDate;
}

/* Function start: 0x43F690 */
void ClearRoomMenuLabel(void)
{
    stCurrentRoomMenu.currentLabel = 0;
}

/* Function start: 0x43F6A0 */
char IsRoomMenuLabelEmpty(void)
{
    return stCurrentRoomMenu.currentLabel == 0;
}

/* Function start: 0x43F6B0 */
void DrawRoomMenuLabel(TextContext *context, const char *label)
{
    DosStrcpy(szTxtWorkspaceBuffer, label);
    SetTextContext(context);
    FormatTextBufferFromStart("%X%Y%s%P",
                              0, 188,
                              szTxtWorkspaceBuffer);
}

/* Function start: 0x43F6F0 */
void RefreshRoomMenuLabel(void)
{
    if (IsRoomMenuLabelEmpty())
        stCurrentRoomMenu.currentLabel =
            pszBlankRoomMenuLabel;
    DrawRoomMenuLabel(&stCurrentRoomMenu.text,
                      stCurrentRoomMenu.currentLabel);
}

/* Function start: 0x43F720 */
void ClearRoomMenuCursorFrame(void)
{
    stCurrentRoomMenu.cursorFrame = 0;
}

/* Function start: 0x43F730 */
void SelectRoomMenuLabel(short i)
{
    const char *label;

    label = stCurrentRoomMenu.labels[i];
    if (label != 0)
        stCurrentRoomMenu.currentLabel = label;
}

/* Function start: 0x43F750 */
void InitializeRoomMenu(const TitleMenuRegion *regions, char **labels,
                        Viewport *viewport, char *text,
                        unsigned char alignment)
{
    stCurrentRoomMenu.regions = regions;
    stCurrentRoomMenu.labels = labels;
    stCurrentRoomMenu.currentLabel =
        pszBlankRoomMenuLabel;
    stCurrentRoomMenu.text.viewport = viewport;
    stCurrentRoomMenu.text.text = text;
    stCurrentRoomMenu.text.textCursor = text;
    stCurrentRoomMenu.text.alignment = alignment;
    *text = ""[0];
    InitializeTextContextFromFont(&stCurrentRoomMenu.text,
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
    stRoomScreenViewport = stScreenView;
    stSceneBuffer.right = 319;
    stSceneBuffer.bottom = 199;
    stSceneBuffer.left = 0;
    stSceneBuffer.top = 0;
    AllocateViewport(&stSceneBuffer, cBlackColour, 0);
    stCurrentRoomMouseView = stSceneBuffer;
    stRoomDisplayView = stRoomScreenViewport;
    SetEventManagerPump(PollMenuInputDevices);
    EventManagerHook(UpdateRoomMenuCursor);
    nSavedRoomCtrlX =
        nMenuInputRepeatDelay;
    nMenuInputRepeatDelay = 6;
}

/* Empty hidden routines of the event manager (eventmgr.c and strdos.c). */
void RefreshMouseCursorHook(void);  /* 0x00435DF0 */
void ReleaseEventManagerPointerHook(void);  /* 0x004355B0 */

/* Function start: 0x43F8B0 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
void DrawMouseCursorIntoViewport(Viewport *viewport)
{
    if (stMouseCursorState.shapeChanged != 0) {
        stMouseCursorState.viewport = viewport;
        RefreshMouseCursorHook();
        CheckCursor();
        stMouseCursorState.viewport = &stRoomScreenViewport;
        return;
    }
    CaptureSpriteBackground(
        viewport,
        (unsigned char *)stMouseCursorState.reservedAfterFrame,
        stMouseCursorState.x, stMouseCursorState.y,
        stMouseCursorState.shape, stMouseCursorState.frame);
    DrawSpriteDefault(viewport,
                      stMouseCursorState.x, stMouseCursorState.y,
                      stMouseCursorState.shape, stMouseCursorState.frame);
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
    signed char animationId;
    unsigned char clicked;
    unsigned char firstFrame;
    short characterMask;
    short eventType;
    short index;
    short personality;
    short refreshRoom;
    short region;
    short result;
    int lastChalkboardTick;

    result = 0;
    lastChalkboardTick = 0;
    firstFrame = 0;
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
    apRecRoomFigureShapes[2] = 0;
    apRecRoomFigureShapes[1] = 0;
    apRecRoomFigureShapes[0] = 0;
    aRecRoomMenuRegions[0].left = 400;
    aRecRoomMenuRegions[0].top = 400;
    aRecRoomMenuRegions[0].right = 401;
    aRecRoomMenuRegions[0].bottom = 401;
    *(ShortRect *)&aRecRoomMenuRegions[1].left =
        *(ShortRect *)&aRecRoomMenuRegions[0].left;
    *(ShortRect *)&aRecRoomMenuRegions[2].left =
        *(ShortRect *)&aRecRoomMenuRegions[0].left;

    apRecRoomFigureShapes[0] =
        FetchDiskPacketRetrying(5, 11, 0);
    GetShapeFrameBounds(&aRecRoomMenuRegions[0].left,
                        aRecRoomCharacterOrigins[0].x,
                        aRecRoomCharacterOrigins[0].y,
                        apRecRoomFigureShapes[0], 0);

    animationIds[1] = (signed char)pRecRoomRoster[
        (stCampaignState.currentSeries * 4 +
         stCampaignState.currentMission) * 2 - 8];
    if (animationIds[1] != -1) {
        personality = (short)animationIds[1];
        if (stCampaignState
                .personalityDeathMission[personality] == 0) {
            apRecRoomFigureShapes[1] =
                FetchDiskPacketRetrying(
                    5, (short)(personality + 3), 0);
            GetShapeFrameBounds(
                &aRecRoomMenuRegions[1].left,
                aRecRoomCharacterOrigins[1].x,
                aRecRoomCharacterOrigins[1].y,
                apRecRoomFigureShapes[1], 0);
            sprintf(apszRecRoomBaseLabels[1],
                    "Talk to %s.",
                    apWingmanPilotsList[personality]->callsign);
        } else {
            sprintf(apszRecRoomBaseLabels[1],
                    "");
        }
    }

    animationIds[2] = (signed char)pRecRoomRoster[
        (stCampaignState.currentSeries * 4 +
         stCampaignState.currentMission) * 2 - 7];
    /* The original tests the first pilot's sentinel here too. */
    if (animationIds[1] != -1) {
        personality = (short)animationIds[2];
        if (stCampaignState
                .personalityDeathMission[personality] == 0) {
            apRecRoomFigureShapes[2] =
                FetchDiskPacketRetrying(
                    5, (short)(personality + 3), 0);
            GetShapeFrameBounds(
                &aRecRoomMenuRegions[2].left,
                aRecRoomCharacterOrigins[2].x,
                aRecRoomCharacterOrigins[2].y,
                apRecRoomFigureShapes[2], 0);
            sprintf(apszRecRoomBaseLabels[2],
                    "Talk to %s.",
                    apWingmanPilotsList[personality]->callsign);
        } else {
            sprintf(apszRecRoomBaseLabels[2],
                    "");
        }
    }

    characterMask = 0;
    ReleasePacketHandle(pRecRoomRoster);
    if (apRecRoomFigureShapes[2] != 0)
        characterMask = 1;
    if (apRecRoomFigureShapes[1] != 0)
        characterMask = 2;
    if (apRecRoomFigureShapes[1] != 0 &&
        apRecRoomFigureShapes[2] != 0)
        characterMask = 3;

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
                       szDefaultTxtBuf, 2);

    bottomSource = stSceneBuffer;
    SetViewportRect(&bottomSource, 0, 187, 319, 199);
    bottomDestination = stRoomScreenViewport;
    SetViewportRect(&bottomDestination, 0, 187, 319, 199);
    pRecRoomBgShape =
        FetchDiskPacketRetrying(5, 0, 0);
    nMenuPointerSpeed = 1;
    bInputModeState = 1;
    stMouseCursorState.viewport = &stRoomDisplayView;

    pilotWork = stSceneBuffer;
    shotglassWork = stSceneBuffer;
    pilotDestination = stRoomScreenViewport;
    shotglassDestination = stRoomScreenViewport;

    if (apRecRoomFigureShapes[2] != 0) {
        GetShapeFrameBounds(
            &secondPilotBounds.left,
            aRecRoomCharacterOrigins[2].x,
            aRecRoomCharacterOrigins[2].y,
            apRecRoomFigureShapes[2],
            (short)*animations[2]);
    }
    if (apRecRoomFigureShapes[1] != 0) {
        GetShapeFrameBounds(
            &firstPilotBounds.left,
            aRecRoomCharacterOrigins[1].x,
            aRecRoomCharacterOrigins[1].y,
            apRecRoomFigureShapes[1],
            (short)*animations[1]);
    }
    if (apRecRoomFigureShapes[1] != 0) {
        *(ShortRect *)&pilotWork.left = firstPilotBounds;
        if (apRecRoomFigureShapes[2] != 0) {
            UnionRectBounds((ShortRect *)&pilotWork.left,
                            &firstPilotBounds, &secondPilotBounds);
        }
    } else if (apRecRoomFigureShapes[2] != 0) {
        *(ShortRect *)&pilotWork.left = secondPilotBounds;
    }
    *(ShortRect *)&pilotDestination.left =
        *(ShortRect *)&pilotWork.left;

    WarpMouseTo(160, 100);
    GetShapeFrameBounds(
        &shotglassWork.left,
        aRecRoomCharacterOrigins[0].x,
        aRecRoomCharacterOrigins[0].y,
        apRecRoomFigureShapes[0],
        (short)*animations[0]);

    while (result == 0) {
        if (firstFrame == 0) {
            DrawSpriteDefault(&stSceneBuffer, 0, 0,
                              pRecRoomBgShape, 0);
            if (characterMask != 0) {
                DrawSpriteDefault(&stSceneBuffer, 158, 128,
                                  pRecRoomBgShape,
                                  characterMask);
            }
            SetFrameTimerPeriodDirect(0);
        }

        if ((short)IsFrameTickElapsed() != 0) {
            DrawSpriteDefault(&pilotWork, 0, 0,
                              pRecRoomBgShape, 0);
            for (index = 0; index < 3; index++) {
                if (apRecRoomFigureShapes[index] != 0) {
                    if (*animations[index] == -1) {
                        if (index == 0) {
                            animationId =
                                (signed char)(RandomInRange(0, 3) + 9);
                            if (animationId == 11 &&
                                RandomInRange(0, 3) != 0)
                                animationId--;
                        } else {
                            animationId = animationIds[index];
                        }
                        animations[index] =
                            apRecRoomAnimations[(int)animationId];
                    }

                    if (index > 0) {
                        DrawSpriteDefault(
                            &pilotWork,
                            aRecRoomCharacterOrigins[index].x,
                            aRecRoomCharacterOrigins[index].y,
                            apRecRoomFigureShapes[index], 0);
                        DrawSpriteDefault(
                            &pilotWork,
                            aRecRoomCharacterOrigins[index].x,
                            aRecRoomCharacterOrigins[index].y,
                            apRecRoomFigureShapes[index],
                            (short)*animations[index]++);
                    } else {
                        UnionRectBounds(
                            (ShortRect *)&shotglassWork.left,
                            (ShortRect *)&shotglassWork.left,
                            (ShortRect *)&stConstellationViewport.left);
                        DrawConstellationField();
                        DrawSpriteDefault(
                            &shotglassWork, 0, 0,
                            pRecRoomBgShape, 0);
                        DrawSpriteDefault(
                            &shotglassWork,
                            aRecRoomCharacterOrigins[index].x,
                            aRecRoomCharacterOrigins[index].y,
                            apRecRoomFigureShapes[index],
                            (short)*animations[index]++);
                        GetShapeFrameBounds(
                            &nextFrameBounds.left,
                            aRecRoomCharacterOrigins[index].x,
                            aRecRoomCharacterOrigins[index].y,
                            apRecRoomFigureShapes[index],
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
            } else if (apRecRoomFigureShapes[1] != 0 ||
                       apRecRoomFigureShapes[2] != 0) {
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

        eventType = PollInputEvent(&event, 0xff);
        switch (eventType) {
        case 3:
        case 5:
            clicked = 0;
            ClearInputKeyStatePreservingModifiers();
            switch ((short)event.value) {
            case 0x1c:
            case 0x39:
                clicked = 1;
                break;
            default:
                MoveMenuPointerFromKeyboard(&event);
                break;
            }
            if (clicked == 0)
                break;
            /* Fall through to the shared mouse/keyboard activation. */
        case 2:
        case 10:
            refreshRoom = 1;
            region = FindMenuRegionAtPoint(
                event.x, event.y, aRecRoomMenuRegions);
            LeaveAllocationScope();
            switch (region) {
            case 1:
            case 2:
                if (apRecRoomFigureShapes[region] == 0)
                    break;
                /* Shotglass enters this path without the shape check. */
            case 0:
                free_constellation();
                ReleasePacketHandle(pRecRoomBgShape);
                stSceneBuffer.bottom = 127;
                stScreenView.top = 24;
                stScreenView.bottom = 151;
                InitializeConversationText();
                ClearViewport(&stRoomScreenViewport,
                              cBlackColour);
                pCurrentConversationBackdropShapePtr =
                    FetchDiskPacketRetrying(
                        5, 1, 0);
                SceneDirector(
                    2,
                    apRecRoomSceneDataList[region],
                    apRecRoomTxtDataArray[region]);
                bEscapePressedFlag = 0;
                SetEventManagerPump(PollMenuInputDevices);
                FreePacketAndClear(
                    &pCurrentConversationBackdropShapePtr, 0);
                SetFrameTimerPeriodDirect(1);
                stScreenView.top = 0;
                stScreenView.bottom = 199;
                stSceneBuffer.bottom = 199;
                stConstellationViewport =
                    stSceneBuffer;
                stConstellationViewport.left = 54;
                stConstellationViewport.right = 146;
                stConstellationViewport.top = 35;
                stConstellationViewport.bottom = 72;
                init_constellation(0);
                InitializeConstellationField(
                    &stConstellationViewport, -1, 6);
                pRecRoomBgShape =
                    FetchDiskPacketRetrying(
                        5, 0, 0);
                ClearViewport(&stRoomScreenViewport,
                              cBlackColour);
                break;
            case 3:
                FlushInputEvents();
                if ((int)(nTickCount60Hz - lastChalkboardTick) >
                    nInputTickScale) {
                    ShowChalkBoard();
                    ClearViewport(&stSceneBuffer, cBlackColour);
                    lastChalkboardTick = (int)nTickCount60Hz;
                }
                break;
            case 4:
            case 5:
                result = region;
                break;
            default:
                refreshRoom = 0;
                EnterAllocationScope();
                break;
            }

            stCurrentRoomMouseView = stScreenView;
            stMouseCursorState.viewport = &stCurrentRoomMouseView;
            bInputModeState = 1;
            if (refreshRoom != 0)
                firstFrame = 0;
            break;
        case 13:
            UpdateRoomMenuCursor();
            break;
        }

        ShowMemoryStatusDebug();
        DIBslam();
        DIBslamReal();
    }

    nMenuPointerSpeed = 2;
    nMenuInputRepeatDelay =
        nSavedRoomCtrlX;
    EventManagerHook(0);
    ReleasePacketHandle(apRecRoomFigureShapes[0]);
    ReleasePacketHandle(apRecRoomFigureShapes[1]);
    ReleasePacketHandle(apRecRoomFigureShapes[2]);
    free_constellation();
    ReleasePacketHandle(pRecRoomBgShape);
    ReleaseTextFont(0);
    ReleasePacketHandle(pBriefingPacket);
    ClearViewport(&stScreenView, cBlackColour);
    free_viewport(&stSceneBuffer);
    bEscapePressedFlag = 0;
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook(30);
    return result;
}

/* Function start: 0x4404E0 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
short SuspendMouseCursor(void)
{
    LeaveAllocationScope();
    ReleaseEventManagerPointerHook();
    return 0;
}

/* Function start: 0x4404F0 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
short InitializeEventManagerTiming(void)
{
    nUnusedInputTimerFlag = 1;
    DAT_0059a9f0 = 20;
    nInputTickScale = 20;
    return 1;
}

/* Function start: 0x440510 */
void ShowChalkBoard(void)
{
    InputEventState event;
    TextContext context;
    TextContext *previousContext;
    unsigned char *background;
    unsigned char done;
    unsigned char savedInputMode;
    char pilotName[20];
    short other;
    short index;
    short pilot;
    short rowY;
    short swap;

    done = 0;
    for (index = 0; index < 9; index++) {
        for (other = index; other < 9; other++) {
            if (aPilotEntriesList[asChalkBoardPilotOrder[index]].kills * 1000 -
                    aPilotEntriesList[asChalkBoardPilotOrder[index]].missions + 1 <
                aPilotEntriesList[asChalkBoardPilotOrder[other]].kills * 1000 -
                    aPilotEntriesList[asChalkBoardPilotOrder[other]].missions + 1) {
                swap = asChalkBoardPilotOrder[other];
                asChalkBoardPilotOrder[other] =
                    asChalkBoardPilotOrder[index];
                asChalkBoardPilotOrder[index] = swap;
            }
        }
    }

    context.colour = 0;
    context.backgroundColour = 0;
    stChalkBoardDate = *pCurrentCampaignDate;
    background = FetchDiskPacketRetrying(5, 2, 0);
    previousContext = pCurTextContext;
    context.viewport = &stModalSrcViewport;
    InitializeTextContextFromFont(&context, 3,
                                  cViewportClearColour, -1);
    context.text = szDefaultTxtBuf;
    savedInputMode = bInputModeState;
    bInputModeState = 1;
    do {
        PumpWindowMessages();
        if (PeekInputEvent(&event, 10) != 0 ||
            PeekInputEvent(&event, 2) != 0 ||
            PeekInputEvent(&event, 3) != 0)
            done = 1;

        DrawSpriteDefault(&stModalSrcViewport,
                          0, 0, background, 0);
        context.alignment = 2;
        SetTextContext(&context);
        FormatTextBufferFromStart("%X%YCARRIER - TIGER'S CLAW%P",
                                  0, 10);
        DrawFormattedText("%X%YPILOT",
                          60, 24);
        DrawFormattedText("%X%YSORTIES KILLS",
                          185, 24);
        rowY = 46;
        index = 0;
        for (; index < 9; index++, rowY = (short)(rowY + 16)) {
            pilot = asChalkBoardPilotOrder[index];
            DosStrcpy(pilotName, aPilotEntriesList[pilot].name);
            strupr(pilotName);
            DrawFormattedText("%X%Y%s %s",
                              10, rowY,
                              apszPilotRankNames[aPilotEntriesList[pilot].rank],
                              pilotName);
            if (pilot == 8 ||
                stCampaignState
                    .personalityDeathMission[pilot] == 0) {
                DrawFormattedText("%X%Y%d%X%d",
                                  230, rowY,
                                  aPilotEntriesList[pilot].missions,
                                  280,
                                  aPilotEntriesList[pilot].kills);
            } else {
                DrawFormattedText("%X%Y     %s",
                                  230, rowY,
                                  "KIA");
            }
        }
        DIBslam();
        DIBslamReal();
    } while (done == 0);

    ReleaseTextFont(3);
    ReleasePacketHandle(background);
    pCurTextContext = previousContext;
    ClearInputKeyStatePreservingModifiers();
    bInputModeState = savedInputMode;
    FlushInputEvents();
}

/* Function start: 0x440800 */
void ResetCampaignData(void)
{
    memcpy(&stCampaignState,
           &stInitialCampaignState,
           sizeof(stCampaignState));
    memcpy(aPilotEntriesList,
           aInitialPilotRecords,
           sizeof(aPilotEntriesList));
    InitializeTrainSimHighScores();
    CorrectPointers();
}

/* Function start: 0x440840 */
short __stdcall ReadPacketSectionData(PacketSectionHandle *handle,
                                      void *destination,
                                      unsigned int length)
{
    int end;
    int offset;

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
