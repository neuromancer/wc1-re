/*
 *  Full-screen presentation screens.
 *
 *  The original function window is recorded in docs/ORDER.md.
 *  Briefing and medal state precedes this unit's text-format literals;
 *  the hand-written raster library in src/raster.asm follows.
 */
#define WC1_SCREENS_SOURCE
#include "wc1.h"

/* 0x0046e188 */
CampaignDate stSavedCampaignDate = {20, 340};
/* 0x0046e190 */
TalkingHeadOrigin aTalkingHeadOrigins[11] = {
    {161, 60, 161, 90},
    {161, 60, 161, 87},
    {160, 60, 159, 90},
    {161, 60, 161, 90},
    {161, 60, 161, 90},
    {161, 60, 161, 90},
    {161, 60, 161, 90},
    {161, 60, 161, 90},
    {161, 60, 161, 90},
    {161, 60, 161, 90},
    {160, 53, 160, 88},
};
/* 0x0046e1e8 */
static signed char abBriefingSmallCharacterAnimation[24] = {
    1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0
};
/* 0x0046e200 */
static signed char abBriefingLargeCharacterAnimation[24] = {
    0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0
};
/* Eight populated records in ten slots; see docs/ORDER.md for the
 * activity-array and data-layout evidence for the zero-initialized tail. */
/* 0x0046e218 */
BriefingCharacterLayout aBriefingCharacters[10] = {
    { 60, 123,  10, 95, 176, 1, 0, 2,
      abBriefingSmallCharacterAnimation, 0},
    {316, 123, 264, 94, 176, 1, 2, 1,
      abBriefingSmallCharacterAnimation, 0},
    {193, 123, 141, 95, 176, 1, 3, 1,
      abBriefingSmallCharacterAnimation, 0},
    {250, 124, 199, 93, 176, 1, 4, 1,
      abBriefingSmallCharacterAnimation, 0},
    {124, 123,  71, 94, 176, 1, 5, 1,
      abBriefingSmallCharacterAnimation, 0},
    {103, 122,  29, 76, 256, 1, 6, 2,
      abBriefingLargeCharacterAnimation, 0},
    {191, 122, 118, 76, 256, 1, 8, 1,
      abBriefingLargeCharacterAnimation, 0},
    {287, 122, 212, 76, 256, 1, 9, 1,
      abBriefingLargeCharacterAnimation, 0}
};
/* 0x0046e2d0 */
static short asMedalDisplayX[5] = {191, 199, 207, 216, 228};
/* 0x0046e2e0 */
const char *apszMedalNames[5] = {
    "Bronze Star",
    "Silver Star",
    "Gold Star",
    "Golden Sun",
    "Terran Medal of Valor"
};
/* 0x0046e2f4 */
static unsigned char *pMedalSceneShape = 0;
/* 0x0046e2f8 */
static const char *pszMedalsPilotSummary =
    "$R $N, aka $C.\n$S system, dateline $D.";
/* 0x0046e300 */
static signed char aBriefingPortraitOffsetX[8][12] = {
    {  0,  0, -2, -2, -1, -2, -4, -3, -5, -7, -7, -4},
    { -1,  0, -2, -3, -3, -4, -3, -4, -6, -8, -7, -6},
    {  0,  1,  0,  0,  0,  0,  0, -1, -2, -4, -5, -3},
    {  0, -1, -2, -3, -1, -3, -2, -3, -4, -4, -5, -4},
    {  2,  0,  0,  0,  0,  0,  0,  0, -2, -3, -4, -2},
    {  1,  1,  1,  0,  1,  0, -6, -4, -4, -9, -8, -6},
    { -2, -1, -4, -4, -4, -7, -8, -7, -5, -8, -9, -7},
    {  0,  0,  0,  0,  0, -3, -3, -7, -5, -7, -8, -7}
};
/* 0x0046e360 */
static signed char aBriefingPortraitOffsetY[8][12] = {
    { -6, -2, -3, -1, -4,-10,-12,-20,-32,-38,-42,-43},
    { -5,  0, -4, -4, -7,-12,-16,-23,-32,-37,-42,-42},
    { -5, -3,  0,  1,  0, -7,-13,-23,-33,-38,-42,-42},
    { -3, -2, -1,  0, -5,-10,-16,-25,-33,-38,-41,-41},
    { -6,  0,  2,  0, -6,-11,-18,-25,-33,-39,-42,-42},
    { -8, -2,  1, -3, -6,-16,-23,-35,-46,-55,-61,-61},
    { -6, -4, -3, -5, -9,-14,-23,-34,-49,-57,-61,-62},
    {-10, -5,  0,  0, -3, -7,-24,-32,-47,-55,-59,-60}
};
/* 0x0046e3c0 */
static short aBriefingPortraitScale[8][12] = {
    {357, 357, 357, 359, 355,   0, 352, 355, 353, 353, 355,   1},
    {355,   0, 354, 354, 352, 354, 355, 355, 352, 352, 353, 350},
    {354, 354, 359,   0, 358, 357, 357, 356, 357,   0,   0,   2},
    {354, 354, 353, 356, 358, 356, 356, 355, 356,   0, 357, 357},
    {  0,   0,   0,   0, 358,   0, 358,   0,   0,   4,   0, 359},
    {  0, 358, 358,   0, 357,   0, 350, 354, 357, 354,   0, 358},
    {356, 356, 352, 354, 352, 349, 350, 352,   0,   0,   0,   0},
    {  0,   0,   0, 359,   0, 350, 354, 350, 356, 357, 357, 357}
};
/* 0x0046e480 */
static int aiBriefingLeftPanelVelocity[12] = {
    1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4
};
/* 0x0046e4b0 */
static int aiBriefingPodiumVelocity[12] = {
    1, 2, 2, 3, 3, 4, 4, 4, 5, 5, 5, 6
};
/* 0x0046e4e0 */
static int aiBriefingRightPanelVelocity[12] = {
    2, 2, 3, 4, 4, 4, 5, 5, 5, 6, 7, 8
};
/* 0x0046e510 */
static signed char abBriefingPodiumFrames[40] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
    10, 11, 12, 13, 14, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 14, 13, 12, 11, 10,  9,
     8,  7,  6,  5,  4,  0,  0,  0,  0,  0
};
/* The shot uses 48 samples; the original table retains two more -2 samples. */
/* 0x0046e538 */
static signed char abDebriefingEstablishDeltas[50] = {
    -2, -2, -1, -1,  0,  0,  1,  0,
     1,  0,  1,  0,  1,  0,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,
     1,  1,  1,  1,  0,  1,  0,  1,
     0,  1,  0,  1,  0,  0, -1, -1, -2, -2
};
/* 0x0046e56c */
static short nDebriefingLeftX = 0;
/* 0x0046e570 */
static short nDebriefingPilotX = 80;
/* 0x0046e574 */
static short nDebriefingRightX = 278;
/* 0x0046e578 */
static short nDebriefingOfficerX = 200;
/* 0x0046e57c */
short nDebriefingPodiumX = 344;
/* 0x0046e580 */
short nConversationCharacter = -1;
/* 0x0046e584 */
short nTalkingHeadFace = -1;
/* 0x0046e588 */
short nConversationBackdropFrame = -1;
/* 0x0046e58c */
int bConversationConstellation = 0;
/* 0x0046e590 */
int bConversationOverlay = 0;

/* Linker commons defined here; LINK orders them by identifier hash. */
unsigned char *pCurrentBriefingTextData; /* 0x00598af0 */
short *pSceneMouthAnimCmds; /* 0x00598af4 */
unsigned char *pRoomMedalTextData; /* 0x00598af8 */
unsigned char *pDebriefingSceneDataPtr; /* 0x00598afc */
char szTxtWorkspaceBuffer[256]; /* 0x00598b00 */
unsigned char *pBriefingSceneInfo; /* 0x00598c00 */
unsigned char *pCurrentConversationBackdropShapePtr; /* 0x00598c04 */
short nCurrentConversationMedalIndex; /* 0x00598c08 */
short nTalkSceneType; /* 0x00598c0a */
unsigned char *pTalkHeadShape; /* 0x00598c0c */
short nConvTextColorValue; /* 0x00598c10 */
unsigned char *pCurrentBriefingAnimShape; /* 0x00598c14 */
short *pFaceAnimationCmds; /* 0x00598c18 */
unsigned char *pSceneBriefingBodyShape; /* 0x00598c1c */
unsigned char *pMedalSceneData; /* 0x00598c20 */
unsigned char *pSceneBriefingPortraitShape; /* 0x00598c24 */
unsigned char *pDebriefingTxtInfo; /* 0x00598c28 */
unsigned char *pBriefingCloseupShape; /* 0x00598c2c */
unsigned char *pConvOverlayShapePtr; /* 0x00598c30 */
unsigned char bFilteredKeyDelayStartedState; /* 0x00598c34 */

/* Function start: 0x436D00 */
unsigned int LoadBriefingRoom(void)
{
    pCurrentConversationBackdropShapePtr = 0;
    pCurrentBriefingAnimShape = 0;
    pBriefingCloseupShape = 0;
    pSceneBriefingBodyShape = 0;
    pSceneBriefingPortraitShape = 0;
    InitializeConversationViewport();
    InitializeConversationText();
    SetTextContext(&stCurrentConvTxtCtx);
    spacetrack(0x19, 2, 1);
    pCurrentConversationBackdropShapePtr =
        FetchDiskPacketRetrying(4, 0, 0);
    pCurrentBriefingAnimShape =
        FetchDiskPacketRetrying(4, 1, 0);
    pBriefingCloseupShape =
        FetchDiskPacketRetrying(4, 3, 0);
    pSceneBriefingBodyShape =
        FetchDiskPacketRetrying(4, 4, 0);
    pSceneBriefingPortraitShape =
        FetchDiskPacketRetrying(4, 5, 0);
    SceneDirector(0, pBriefingSceneInfo,
                  pCurrentBriefingTextData);
    FreePacketAndClear(&pSceneBriefingPortraitShape, 8);
    FreePacketAndClear(&pSceneBriefingBodyShape, 8);
    FreePacketAndClear(&pBriefingCloseupShape, 8);
    FreePacketAndClear(&pCurrentBriefingAnimShape, 8);
    FreePacketAndClear(&pCurrentConversationBackdropShapePtr, 8);
    ReleaseTextFont(0);
    ResetScreenClipToFullHeight();
    if (bEscapePressedFlag != 0) {
        StopMusicUnlessSuppressed();
        bEscapePressedFlag = 0;
    }
    return 0;
}

/* Function start: 0x436E30 */
unsigned int ViewMedals(void)
{
    InputEventState event;
    unsigned char clicked;
    unsigned char savedInputMode;

    clicked = 0;
    pMedalSceneShape =
        FetchDiskPacketRetrying(4, 8, 0);
    pCurrentConversationBackdropShapePtr = 0;
    InitializeConversationText();
    ClearViewport(&stSceneBuffer, cBlackColour);
    savedInputMode = bInputModeState;
    bInputModeState = 1;
    do {
        PumpWindowMessages();
        if (PeekInputEvent(&event, 10) != 0 ||
            PeekInputEvent(&event, 2) != 0 ||
            PeekInputEvent(&event, 3) != 0)
            clicked = 1;
        DrawMedals();
        AddPCName(pszMedalsPilotSummary);
        RefreshMemoryStatusOverlay();
        ClearViewport(&stCurrentConvTextViewport,
                      cBlackColour);
        FormatTextBufferFromStart(
            "%X%Y%F%s%P", 0, 160,
            cViewportClearColour,
            szTxtWorkspaceBuffer);
        DIBslam();
        DIBslamReal();
        if (clicked != 0) {
            FreePacketAndClear(&pMedalSceneShape, 8);
            WaitForInputKey();
            ClearInputKeyStatePreservingModifiers();
            bInputModeState = savedInputMode;
            FlushInputEvents();
            return 0;
        }
    } while (1);
}

/* Function start: 0x436F50 */
unsigned int AwardCampaignMedal(short medal)
{
    int medalIndex;
    int *packet;

    if (medal == 3 &&
        (signed char)stCampaignState.medals[3] > 0)
        return 0;

    PreloadMusicTrackHook(0x25);
    PreloadMusicTrackHook(0x26);
    PreloadMusicTrackHook(0x27);
    PreloadMusicTrackHook(0x28);

    medalIndex = medal;
    switch (medalIndex) {
    case 0:
    case 1:
        spacetrack(0x27, 1, -1);
        break;
    case 2:
    case 4:
        spacetrack(0x28, 1, -1);
        break;
    case 3:
        spacetrack(0x26, 1, -1);
        break;
    }

    packet = FetchDiskPacketRetrying(
        asCampaignBriefingFiles[nCampaignDataSet],
        2, 0);
    pMedalSceneData = (unsigned char *)packet + packet[0];
    pRoomMedalTextData = (unsigned char *)packet + packet[1];
    InitializeConversationViewport();
    InitializeConversationText();
    pMedalSceneShape =
        FetchDiskPacketRetrying(4, 8, 0);
    pCurrentConversationBackdropShapePtr =
        FetchDiskPacketRetrying(4, 10, 0);
    nCurrentConversationMedalIndex = medal;
    stCampaignState.medals[medalIndex]++;
    SceneDirector(5, pMedalSceneData,
                  pRoomMedalTextData);
    bEscapePressedFlag = 0;
    FreePacketAndClear(&pCurrentConversationBackdropShapePtr, 8);
    FreePacketAndClear(&pMedalSceneShape, 8);
    ReleasePacketHandle(packet);
    StopMusicUnlessSuppressed();
    ReleaseTextFont(0);
    ResetScreenClipToFullHeight();
    ReleaseMusicTrackHook(0x25);
    ReleaseMusicTrackHook(0x26);
    ReleaseMusicTrackHook(0x27);
    ReleaseMusicTrackHook(0x28);
    return 0;
}

/* Function start: 0x4370D0 */
unsigned int DrawMedalChest(char *text, short duration)
{
    short escaped;
    short offset;

    AddPCName(text);
    ClearViewport(&stCurrentConvTextViewport,
                  cBlackColour);
    FormatTextBufferFromStart("%X%Y%F%s%P",
                              0, 160,
                              nConvTextColorValue,
                              szTxtWorkspaceBuffer);
    offset = 0;
    do {
        DrawSpriteDefault(&stSceneBuffer, 0, 0,
                          pMedalSceneShape, 41);
        DrawSpriteDefault(&stSceneBuffer, (short)(92 - offset), 64,
                          pMedalSceneShape, 43);
        DrawSpriteDefault(&stSceneBuffer, (short)(228 + offset), 64,
                          pMedalSceneShape, 44);
        DrawSpriteDefault(&stSceneBuffer, 0, 64,
                          pMedalSceneShape, 42);
        DrawSpriteScaled(&stSceneBuffer, 319, 64,
                         pMedalSceneShape, 42,
                         0, 256, 16);
        RefreshMemoryStatusOverlay();
        escaped = CheckEscaped();
        if (escaped != 0) {
            duration = -1;
            break;
        }
        offset = (short)(offset + 2);
        DIBslam();
        DIBslamReal();
    } while (offset < 162);
    WaitForSceneAdvance(duration, 0);
    switch (nCurrentConversationMedalIndex) {
    case 0:
    case 1:
        spacetrack(39, 1, -1);
        break;
    case 2:
    case 4:
        spacetrack(40, 1, -1);
        break;
    case 3:
        spacetrack(38, 1, -1);
        break;
    }
    return 0;
}

/* Function start: 0x437250 */
unsigned int DrawMedalLongShot(short *animation, char *text,
                               short duration)
{
    short *start;
    short countdown;
    short escaped;
    short frame;

    countdown = 0;
    AddPCName(text);
    ClearViewport(&stCurrentConvTextViewport,
                  cBlackColour);
    FormatTextBufferFromStart("%X%Y%F%s%P",
                              0, 160,
                              nConvTextColorValue,
                              szTxtWorkspaceBuffer);
    start = animation;
    while (*animation != -1) {
        if (countdown-- == 0) {
            if (*animation != -1)
                animation += 2;
            switch (*animation) {
            case -2:
                animation = start;
                /* fall through */
            default:
                frame = *animation;
                countdown = (short)(animation[1] * 2);
                break;
            case -1:
                frame = -1;
                break;
            }
        }
        DrawSpriteDefault(&stSceneBuffer, 0, 0,
                          pCurrentConversationBackdropShapePtr, 0);
        DrawSpriteDefault(&stSceneBuffer, 0, 0,
                          pMedalSceneShape, 0);
        if (frame > -1)
            DrawSpriteDefault(&stSceneBuffer, 121, 8,
                              pMedalSceneShape,
                              (short)(frame + 1));
        RefreshMemoryStatusOverlay();
        escaped = CheckEscaped();
        if (escaped != 0) {
            duration = -1;
            break;
        }
        DIBslam();
        DIBslamReal();
    }
    DrawSpriteDefault(&stSceneBuffer, 0, 0,
                      pCurrentConversationBackdropShapePtr, 0);
    DrawSpriteDefault(&stSceneBuffer, 0, 0,
                      pMedalSceneShape, 0);
    RefreshMemoryStatusOverlay();
    DIBslam();
    DIBslamReal();
    WaitForSceneAdvance(duration, 0);
    return 0;
}

/* Function start: 0x4373E0 */
unsigned int MedalEstablish(char *text, short duration)
{
    int distance;
    short escaped;
    short frame;
    short scale;
    int x;
    int y;

    distance = 200;
    AddPCName(text);
    ClearViewport(&stCurrentConvTextViewport,
                  cBlackColour);
    FormatTextBufferFromStart("%X%Y%F%s%P",
                              0, 160,
                              nConvTextColorValue,
                              szTxtWorkspaceBuffer);
    x = asMedalDisplayX[nCurrentConversationMedalIndex];
    y = 87;
    for (frame = 0; frame < 32; frame++) {
        DrawMedals();
        scale = (short)(0xc800L / distance);
        DrawSpriteScaled(&stSceneBuffer, (short)x, (short)y,
                         pMedalSceneShape, 12,
                         0, scale, 0);
        distance--;
        x--;
        y += 2;
        RefreshMemoryStatusOverlay();
        escaped = CheckEscaped();
        if (escaped != 0) {
            duration = -1;
            break;
        }
        DIBslam();
        DIBslamReal();
    }
    WaitForSceneAdvance(duration, 0);
    return 0;
}

/* Function start: 0x4374B0 */
unsigned int PinMedal(char *text, short duration)
{
    short escaped;
    short frame;

    frame = 0;
    AddPCName(text);
    ClearViewport(&stCurrentConvTextViewport,
                  cBlackColour);
    FormatTextBufferFromStart("%X%Y%F%s%P",
                              0, 160,
                              nConvTextColorValue,
                              szTxtWorkspaceBuffer);
    ClearViewport(&stSceneBuffer, cBlackColour);
    SetFrameTimerPeriodDirect(duration);
    escaped = IsFrameTickElapsed();
    while (escaped == 0) {
        frame++;
        DrawSpriteDefault(&stSceneBuffer, 0, 0,
                          pCurrentConversationBackdropShapePtr, 0);
        DrawSpriteDefault(&stSceneBuffer, 0, 0,
                          pMedalSceneShape, 0);
        DrawSpriteDefault(&stSceneBuffer, 0, 0,
                          pMedalSceneShape,
                          (short)(frame % 3 + 38));
        RefreshMemoryStatusOverlay();
        escaped = CheckEscaped();
        if (escaped != 0) {
            duration = -1;
            break;
        }
        DIBslam();
        DIBslamReal();
        escaped = IsFrameTickElapsed();
    }
    WaitForSceneAdvance(duration, 0);
    return 0;
}

/* Function start: 0x4375C0 */
unsigned int DrawMedals(void)
{
    short badge;
    short medal;
    short rowY;
    short medalRowY;
    short stack;
    short x;

    rowY = 78;
    x = 188;
    DrawSpriteDefault(&stSceneBuffer, 0, 0,
                      pCurrentConversationBackdropShapePtr, 1);
    DrawSpriteDefault(&stSceneBuffer, 0, 0,
                      pMedalSceneShape, 11);
    DrawSpriteDefault(&stSceneBuffer, 253, 38,
                      pMedalSceneShape,
                      (short)(stCampaignState.currentPilot->rank +
                              33));
    DrawSpriteScaled(&stSceneBuffer, 67, 38,
                     pMedalSceneShape,
                     (short)(stCampaignState.currentPilot->rank +
                             33),
                     0, 255, 16);
    for (badge = 0; badge < 12; badge++) {
        if (stCampaignState.badges[badge] != 0) {
            if (x >= 232) {
                rowY = (short)(rowY + 3);
                x = 188;
            }
            DrawSpriteDefault(&stSceneBuffer, x, rowY,
                              pMedalSceneShape,
                              (short)(badge + 13));
            x = (short)(x + 11);
        }
    }
    medalRowY = (short)(rowY + 5);
    for (medal = 0; medal < 5; medal++) {
        if (stCampaignState.medals[medal] != 0) {
            x = asMedalDisplayX[medal];
            stack = medalRowY;
            if (medal < 3) {
                short medalCount;

                medalCount = 0;
                while (medalCount < (signed char)
                                   stCampaignState.medals[medal]) {
                    medalCount++;
                    DrawSpriteDefault(&stSceneBuffer, x, stack,
                                      pMedalSceneShape,
                                      (short)(medal + 25));
                    stack = (short)(stack + 2);
                }
            }
            DrawSpriteDefault(&stSceneBuffer, x, stack,
                              pMedalSceneShape,
                              (short)(medal + 28));
        }
    }
    DIBslam();
    DIBslamReal();
    return 0;
}

/* Function start: 0x437770 */
unsigned int EstablishingShot(char *text, short duration)
{
    int frameIndex;
    short character;
    short escaped;
    short frame;

    if (pSceneBriefingBodyShape == 0)
        pSceneBriefingBodyShape =
            FetchDiskPacketRetrying(4, 4, 0);
    if (pSceneBriefingPortraitShape == 0)
        pSceneBriefingPortraitShape =
            FetchDiskPacketRetrying(4, 5, 0);
    AddPCName(text);
    ClearViewport(&stCurrentConvTextViewport,
                  cBlackColour);
    FormatTextBufferFromStart("%X%Y%F%s%P",
                              0, 160,
                              nConvTextColorValue,
                              szTxtWorkspaceBuffer);
    frame = 0;
    FlushInputEvents();
    do {
        DrawSpriteDefault(&stSceneBuffer, 0, 0,
                          pCurrentConversationBackdropShapePtr, 0);
        DrawSpriteDefault(&stSceneBuffer, 241, 60,
                          pCurrentBriefingAnimShape, frame);
        DrawSpriteDefault(&stSceneBuffer, 241, 64,
                          pCurrentBriefingAnimShape, 22);
        frameIndex = frame;
        for (character = 0; character < 8; character++) {
            DrawBriefingCharacter(
                character, 0,
                aBriefingCharacters[character]
                    .animation[frameIndex],
                0, 0);
        }
        RefreshMemoryStatusOverlay();
        escaped = CheckEscaped();
        if (escaped != 0) {
            frame = 21;
            duration = -1;
        }
        frame++;
        DIBslam();
        DIBslamReal();
    } while (frame < 22);
    WaitForSceneAdvance(duration, 0);
    spacetrack(25, 1, -1);
    return 0;
}

/* Function start: 0x4378D0 */
unsigned int DrawBriefingLongShot(void)
{
    short character;

    if (pSceneBriefingBodyShape == 0)
        pSceneBriefingBodyShape =
            FetchDiskPacketRetrying(4, 4, 0);
    if (pSceneBriefingPortraitShape == 0)
        pSceneBriefingPortraitShape =
            FetchDiskPacketRetrying(4, 5, 0);
    DrawSpriteDefault(&stSceneBuffer, 0, 0,
                      pCurrentConversationBackdropShapePtr, 0);
    DrawSpriteDefault(&stSceneBuffer, 241, 60,
                      pCurrentBriefingAnimShape, 21);
    DrawSpriteDefault(&stSceneBuffer, 241, 64,
                      pCurrentBriefingAnimShape, 22);
    for (character = 0; character < 8; character++) {
        DrawBriefingCharacter(character, 0, 0, 0, 0);
    }
    RefreshMemoryStatusOverlay();
    return 0;
}

/* Function start: 0x437980 */
unsigned int ReturnToBriefingLongShot(char *text, short duration)
{
    char active[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    signed char *phase;
    char *characterActive;
    short character;
    short escaped;
    short frame;

    spacetrack(26, 1, -1);
    if (pSceneBriefingBodyShape == 0)
        pSceneBriefingBodyShape =
            FetchDiskPacketRetrying(4, 4, 0);
    if (pSceneBriefingPortraitShape == 0)
        pSceneBriefingPortraitShape =
            FetchDiskPacketRetrying(4, 5, 0);
    AddPCName(text);
    ClearViewport(&stCurrentConvTextViewport,
                  cBlackColour);
    FormatTextBufferFromStart("%X%Y%F%s%P",
                              0, 160,
                              nConvTextColorValue,
                              szTxtWorkspaceBuffer);
    for (character = 0; character < 8; character++) {
        aBriefingCharacters[character].animationPhase = 0;
    }
    frame = 0;
    for (;;) {
        DrawSpriteDefault(&stSceneBuffer, 0, 0,
                          pCurrentConversationBackdropShapePtr, 0);
        DrawSpriteDefault(&stSceneBuffer, 241, 60,
                          pCurrentBriefingAnimShape, 0);
        DrawSpriteDefault(&stSceneBuffer, 241, 64,
                          pCurrentBriefingAnimShape, 22);
        for (character = 0; character < 8; character++) {
            characterActive = &active[character];
            if (*characterActive == 0 &&
                RandomBelowOrEqual(5) == 0)
                *characterActive = 1;
            phase = &aBriefingCharacters[character].animationPhase;
            DrawBriefingCharacter(
                character, *phase, 0,
                (const signed char *)aBriefingPortraitOffsetX +
                    *phase * 12,
                (const signed char *)aBriefingPortraitOffsetY +
                    *phase * 12);
            if (*characterActive == 1 && *phase < 11)
                (*phase)++;
        }
        RefreshMemoryStatusOverlay();
        escaped = CheckEscaped();
        if (escaped != 0) {
            duration = 0;
            StopMusicUnlessSuppressed();
            break;
        }
        frame++;
        DIBslam();
        DIBslamReal();
        if (frame >= 40)
            break;
    }
    WaitForSceneAdvance(duration, 0);
    ClearViewport(&stCurrentConvTextViewport,
                  cBlackColour);
    return 0;
}

/* Function start: 0x437B80 */
unsigned int Dismissed(char *text, short duration)
{
    short frame;
    short leftDelta;
    short podiumFrame;
    short leftX;
    short podiumDelta;
    short podiumX;
    short rightDelta;
    short rightX;

    podiumFrame = 0;
    leftX = -96;
    podiumX = 240;
    rightX = 252;
    if (pTalkHeadShape != 0)
        FreePacketAndClear(&pTalkHeadShape, 0);
    if (pSceneBriefingBodyShape != 0)
        FreePacketAndClear(&pSceneBriefingBodyShape, 0);
    if (pSceneBriefingPortraitShape != 0)
        FreePacketAndClear(&pSceneBriefingPortraitShape, 0);
    AddPCName(text);
    ClearViewport(&stCurrentConvTextViewport,
                  cBlackColour);
    FormatTextBufferFromStart("%X%Y%F%s%P",
                              0, 160,
                              nConvTextColorValue,
                              szTxtWorkspaceBuffer);
    pTalkHeadShape =
        FetchDiskPacketRetrying(4, 2, 0);
    nFrameSkipCounter = 1;
    frame = 0;
    do {
        if (CheckEscaped() != 0) {
            rightX = 348;
            leftX = 0;
            frame = 31;
            podiumFrame = 34;
            podiumX = 336;
        }
        nFrameSkipCounter--;
        if (nFrameSkipCounter < 1) {
            nFrameSkipCounter = nFrameSkip;
            DrawSpriteDefault(&stSceneBuffer, leftX, 0,
                              pCurrentConversationBackdropShapePtr, 1);
            DrawSpriteDefault(&stSceneBuffer, (short)(leftX + 320), 0,
                              pCurrentConversationBackdropShapePtr, 2);
            DrawSpriteDefault(&stSceneBuffer, podiumX, 127,
                              pTalkHeadShape,
                              abBriefingPodiumFrames[
                                  podiumFrame]);
            DrawSpriteDefault(&stSceneBuffer, rightX, 127,
                              pCurrentConversationBackdropShapePtr, 3);
            RefreshMemoryStatusOverlay();
        }
        if (podiumFrame < 34)
            podiumFrame++;
        if (frame < 12) {
            leftDelta =
                (short)aiBriefingLeftPanelVelocity[frame];
            podiumDelta =
                (short)aiBriefingPodiumVelocity[frame];
            rightDelta =
                (short)aiBriefingRightPanelVelocity[frame];
        }
        if (frame > 24)
            leftDelta = (short)aiBriefingLeftPanelVelocity[
                11 - (frame - 24)];
        podiumX = (short)(podiumX + podiumDelta);
        rightX = (short)(rightX + rightDelta);
        frame++;
        leftX = (short)(leftX + leftDelta);
        DIBslam();
        DIBslamReal();
    } while (frame < 32);
    WaitForSceneAdvance(duration, 0);
    FreePacketAndClear(&pTalkHeadShape, 0);
    SetTextContext(&stCurrentConvTxtCtx);
    return 0;
}

/* Function start: 0x437DC0 */
unsigned int DrawDebriefingLongShot(void)
{
    DrawSpriteDefault(&stSceneBuffer, nDebriefingLeftX, 0,
                      pCurrentConversationBackdropShapePtr, 2);
    DrawSpriteDefault(&stSceneBuffer,
                      (short)(nDebriefingLeftX + 320), 0,
                      pCurrentConversationBackdropShapePtr, 3);
    DrawSpriteDefault(&stSceneBuffer,
                      (short)(nDebriefingPilotX - 1), 127,
                      pCurrentConversationBackdropShapePtr, 4);
    DrawSpriteDefault(&stSceneBuffer, nDebriefingPilotX, 127,
                      pCurrentConversationBackdropShapePtr, 5);
    DrawSpriteDefault(&stSceneBuffer, nDebriefingPodiumX, 127,
                      pCurrentConversationBackdropShapePtr, 8);
    if (stCampaignState.personalityDeathMission[
            nDebriefingPersonality] == 0) {
        if (nDebriefingPersonality != 0)
            DrawSpriteDefault(
                &stSceneBuffer, nDebriefingOfficerX, 32,
                pCurrentConversationBackdropShapePtr,
                (short)(nDebriefingPersonality + 9));
        DrawSpriteDefault(&stSceneBuffer,
                          nDebriefingOfficerX, 32,
                          pCurrentConversationBackdropShapePtr, 6);
        if (nDebriefingPersonality == 0)
            DrawSpriteDefault(
                &stSceneBuffer, nDebriefingOfficerX, 32,
                pCurrentConversationBackdropShapePtr,
                (short)(nDebriefingPersonality + 9));
    }
    DrawSpriteDefault(&stSceneBuffer, nDebriefingRightX, 127,
                      pCurrentConversationBackdropShapePtr, 7);
    DIBslam();
    DIBslamReal();
    return 0;
}

/* Function start: 0x437F20 */
unsigned int DebriefingEstablishingShot(char *text, short duration)
{
    const signed char *delta;
    short escaped;
    short frame;
    short skip;

    nDebriefingPilotX = 80;
    nDebriefingRightX = 278;
    frame = 0;
    nDebriefingLeftX = 0;
    nDebriefingOfficerX = 200;
    nDebriefingPodiumX = 344;
    AddPCName(text);
    ClearViewport(&stCurrentConvTextViewport,
                  cBlackColour);
    FormatTextBufferFromStart("%X%Y%F%s%P",
                              0, 160,
                              nConvTextColorValue,
                              szTxtWorkspaceBuffer);
    nFrameSkipCounter = 1;
    for (;;) {
        delta = &abDebriefingEstablishDeltas[frame];
        nDebriefingLeftX =
            (short)(nDebriefingLeftX -
                    MaxShort((short)delta[0], 0));
        nDebriefingPilotX =
            (short)(nDebriefingPilotX -
                    MaxShort((short)(delta[0] + 1), 0));
        nDebriefingPodiumX =
            (short)(nDebriefingPodiumX -
                    MaxShort((short)(delta[0] + 2), 0));
        nDebriefingOfficerX =
            (short)(nDebriefingOfficerX -
                    MaxShort((short)(delta[0] + 3), 0));
        nDebriefingRightX =
            (short)(nDebriefingRightX -
                    MaxShort((short)(delta[0] + 3), 0));
        skip = nFrameSkipCounter;
        if (frame == 47)
            skip = 1;
        skip--;
        nFrameSkipCounter = skip;
        if (skip < 1) {
            nFrameSkipCounter = nFrameSkip;
            DrawDebriefingLongShot();
            RefreshMemoryStatusOverlay();
            DIBslam();
            DIBslamReal();
        }
        escaped = CheckEscaped();
        if (escaped != 0) {
            duration = -1;
            break;
        }
        frame++;
        if (frame >= 48)
            break;
    }
    WaitForSceneAdvance(duration, 0);
    return 0;
}

/* Function start: 0x438090 */
short no_objectives_achieved(void)
{
    short objective;

    objective = 0;
    while (objective < cMissionObjectiveCountValue) {
        if (achieved(objective) != 0)
            break;
        objective++;
    }
    return objective >= cMissionObjectiveCountValue;
}

/* Function start: 0x4380D0 */
short wing_status(short personality)
{
    int deathMission;
    int currentMission;

    deathMission =
        stCampaignState.personalityDeathMission[personality];
    if (deathMission == 0)
        return 3;
    currentMission = stCampaignState.currentMission +
                     stCampaignState.currentSeries * 4;
    if (currentMission == deathMission)
        return 1;
    if (currentMission > deathMission)
        return 2;
}

/* Function start: 0x438110 */
short int_value(char **text)
{
    char number[8];
    char *destination;
    short character;

    destination = number;
    character = (short)**text;
    while (character != ',' && character != ')') {
        (*text)++;
        *destination = character;
        destination++;
        character = **text;
    }
    *destination = '\0';
    (*text)++;
    return (short)atoi(number);
}

/* Function start: 0x438160 */
ConversationSceneRecord *ParseTests(ConversationSceneRecord *record,
                                    ConversationSceneRecord *sceneData,
                                    unsigned char *textData)
{
    char *test;
    short first;
    short second;
    short testCode;

    test = (char *)textData + record->testsOffset;
    while ((testCode = (short)*test++) != 0) {
        switch (testCode) {
        case 1:
            return sceneData + int_value(&test);
        case 2:
            first = int_value(&test);
            second = int_value(&test);
            if (stCampaignState.missionScore < first)
                return sceneData + second;
            break;
        case 3:
            first = int_value(&test);
            second = int_value(&test);
            if (first <= stCampaignState.missionScore)
                return sceneData + second;
            break;
        case 4:
            first = int_value(&test);
            second = int_value(&test);
            if (wing_status(first) != 3)
                return sceneData + second;
            break;
        case 5:
            first = int_value(&test);
            second = int_value(&test);
            if (wing_status(first) == 3)
                return sceneData + second;
            break;
        case 6:
            first = int_value(&test);
            if (nPlayerKillTotal == 0)
                return sceneData + first;
            break;
        case 7:
            first = int_value(&test);
            if (nPlayerKillTotal != 0)
                return sceneData + first;
            break;
        case 8:
            first = int_value(&test);
            if (nWingmanKillCount == 0)
                return sceneData + first;
            break;
        case 9:
            first = int_value(&test);
            if (nWingmanKillCount != 0)
                return sceneData + first;
            break;
        case 10:
            first = int_value(&test);
            if (bOfficeVisitPending == 0)
                return sceneData + first;
            break;
        case 11:
            first = int_value(&test);
            second = int_value(&test);
            if (achieved(first) == 0)
                return sceneData + second;
            break;
        case 12:
            first = int_value(&test);
            second = int_value(&test);
            if (achieved(first) != 0)
                return sceneData + second;
            break;
        case 27:
            first = int_value(&test);
            second = int_value(&test);
            if (sighted(first) != 0)
                return sceneData + second;
            break;
        case 29:
            first = int_value(&test);
            second = int_value(&test);
            if (wing_status(first) == 2)
                return sceneData + second;
            break;
        case 30:
            first = int_value(&test);
            second = int_value(&test);
            if (wing_status(first) == 1)
                return sceneData + second;
            break;
        case 31:
            first = int_value(&test);
            second = int_value(&test);
            if (ace_status(first, 1) == 0)
                return sceneData + second;
            break;
        case 32:
            first = int_value(&test);
            second = int_value(&test);
            if (ace_status(first, 1) != 0)
                return sceneData + second;
            break;
        case 33:
            first = int_value(&test);
            second = int_value(&test);
            if (ace_status(first, 2) == 0 &&
                ace_status(first, 1) == 0)
                return sceneData + second;
            break;
        case 34:
            first = int_value(&test);
            second = int_value(&test);
            if (ace_status(first, 2) != 0)
                return sceneData + second;
            break;
        }
        switch (testCode) {
        case 13:
            first = int_value(&test);
            if (nCurrentConversationMedalIndex == 4)
                return sceneData + first;
            break;
        case 14:
            first = int_value(&test);
            if (nCurrentConversationMedalIndex < 3)
                return sceneData + first;
            break;
        case 15:
            first = int_value(&test);
            if (nCurrentConversationMedalIndex == 3)
                return sceneData + first;
            break;
        case 16:
            first = int_value(&test);
            if (bPromotionPending != 1)
                return sceneData + first;
            break;
        case 17:
            first = int_value(&test);
            if (bPlayerEjectedThisMission == 0)
                return sceneData + first;
            break;
        case 18:
            first = int_value(&test);
            if (bPlayerEjectedThisMission == 1 &&
                stCampaignState.elapsedDate.year == 1)
                return sceneData + first;
            break;
        case 19:
            first = int_value(&test);
            if (bPlayerShipTypeChanged != 1)
                return sceneData + first;
            break;
        case 20:
            first = int_value(&test);
            if (stCampaignState.playerShipType != 0)
                return sceneData + first;
            break;
        case 21:
            first = int_value(&test);
            if (stCampaignState.playerShipType != 2)
                return sceneData + first;
            break;
        case 22:
            first = int_value(&test);
            if (stCampaignState.playerShipType != 3)
                return sceneData + first;
            break;
        case 23:
            first = int_value(&test);
            if (stCampaignState.playerShipType != 1)
                return sceneData + first;
            break;
        case 24:
            first = int_value(&test);
            if (stCampaignState.playerShipType != 1 &&
                stCampaignState.playerShipType <
                    nPreviousPlayerShipType)
                return sceneData + first;
            break;
        case 25:
            first = int_value(&test);
            if (stCampaignState.playerShipType == 1 ||
                stCampaignState.playerShipType >=
                    nPreviousPlayerShipType)
                return sceneData + first;
            break;
        case 26:
            first = int_value(&test);
            if (bPlayerEjectedThisMission == 1 &&
                stCampaignState.elapsedDate.year > 1)
                return sceneData + first;
            break;
        case 28:
            first = int_value(&test);
            second = int_value(&test);
            if (sighted(first) == 0)
                return sceneData + second;
            break;
        case 35:
            first = int_value(&test);
            if (PlayersMissionScore() == FullMissionScore())
                return sceneData + first;
            break;
        case 36:
            first = int_value(&test);
            if (PlayersMissionScore() < FullMissionScore())
                return sceneData + first;
            break;
        case 37:
            first = int_value(&test);
            if (no_objectives_achieved() != 0)
                return sceneData + first;
            break;
        case 38:
            first = int_value(&test);
            if (no_objectives_achieved() == 0)
                return sceneData + first;
            break;
        }
    }
    return record;
}

/* Function start: 0x438B90 */
unsigned int TalkerInit(void)
{
    pFaceAnimationCmds =
        AllocateTaggedMemory(0x140, 0);
    pSceneMouthAnimCmds =
        AllocateTaggedMemory(0x140, 0);
    return 0;
}

/* Function start: 0x438BC0 */
unsigned int FreeTalker(void)
{
    FreePacketAndClear(&pConvOverlayShapePtr, 0);
    FreePacketAndClear(&pTalkHeadShape, 0);
    FreePacketAndClear(&pSceneMouthAnimCmds, 0);
    FreePacketAndClear(&pFaceAnimationCmds, 0);
    return 0;
}

/* Function start: 0x438C00 */
unsigned int SceneDirector(short sceneType, unsigned char *sceneBytes,
                           unsigned char *textData)
{
    ConversationSceneRecord *record;
    ConversationSceneRecord *selected;
    short duration;
    short previousColour;
    short shot;
    short previousShot;
    char *text;

    previousShot = -2;
    previousColour = -2;
    record = (ConversationSceneRecord *)sceneBytes;
    nTalkSceneType = sceneType;
    TalkerInit();
    bInputModeState = 1;
    ClearInputKeyState();
    FlushInputEvents();
    SetEventManagerPump(PollJoystickButtonEvents);
    bEscapePressedFlag = 0;
    for (;;) {
        shot = (short)record->shot;
        if (shot == -2)
            goto scene_complete;
        if (shot != -1) {
            if ((shot & 0x40) != 0) {
                bConversationOverlay = 1;
                shot &= 0x3f;
            } else {
                bConversationOverlay = 0;
            }
        }
        if (record->testsOffset != 0) {
            selected = record;
            record = ParseTests(record,
                                (ConversationSceneRecord *)sceneBytes,
                                textData);
            if (selected != record)
                continue;
        }
        if (record->talker != -2)
            nConversationCharacter = record->talker;
        duration = record->duration;
        switch (shot & 0x3f) {
        case 0:
        case 3:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 16:
        case 17:
            previousShot = shot;
            nTalkingHeadFace = -1;
            break;
        case 1:
            if (previousShot != 1) {
                previousShot = 1;
                DrawBriefingLongShot();
                nTalkingHeadFace = -1;
            }
            break;
        case 2:
            if (previousShot != 2) {
                previousShot = 2;
                DrawPodiumShot();
                nTalkingHeadFace = -1;
            }
            break;
        case 4:
            previousShot = 4;
            cCurrentObjective = (signed char)abs(nConversationCharacter);
            nTalkingHeadFace = -1;
            break;
        case 12:
        case 13:
        case 14:
        case 15:
            if (previousShot != shot) {
                init_constellation(0);
                stConstellationViewport = stSceneBuffer;
                InitializeConstellationField(
                    &stConstellationViewport, -1, 16);
                bConversationConstellation = 1;
                nTalkingHeadFace = -1;
                previousShot = shot;
            }
            break;
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
            if (previousShot != shot) {
                LoadFace((short)(shot - 20));
                previousShot = shot;
            }
            break;
        case 50:
        case 51:
        case 52:
        case 53:
        case 54:
        case 55:
        case 56:
        case 57:
        case 58:
        case 59:
            previousShot = 50;
            nTalkingHeadFace = -1;
            break;
        }
        if (previousColour != (short)record->textColour &&
            record->textColour != -1) {
            previousColour = record->textColour;
            nConvTextColorValue =
                asConversationTextColours[previousColour];
        }
        pSceneMouthAnimCmds[0] = -1;
        if (textData[record->mouthAnimationOffset] != '\0')
            ParseMouthAnimation((char *)textData +
                                    record->mouthAnimationOffset,
                                pSceneMouthAnimCmds);
        pFaceAnimationCmds[0] = -1;
        if (textData[record->faceAnimationOffset] != '\0')
            ParseFaceAnimation((char *)textData +
                               record->faceAnimationOffset,
                               pFaceAnimationCmds);
        FlushInputEvents();
        text = (char *)textData + record->textOffset;
        if (*text != '\0') {
            switch (previousShot) {
            case 0:
                EstablishingShot(text, duration);
                break;
            case 1:
            case 2:
            case 11:
                CloseLook(pBriefingCloseupShape,
                          previousShot,
                          pSceneMouthAnimCmds,
                          text, duration, 0);
                break;
            case 3:
                previousShot = 4;
                Dismissed(text, duration);
                break;
            case 4:
                UpdateMap(text, duration);
                break;
            case 5:
                previousShot = 1;
                ReturnToBriefingLongShot(text, duration);
                break;
            case 6:
                DrawMedalLongShot(pSceneMouthAnimCmds,
                                  text, duration);
                break;
            case 7:
                MedalEstablish(text, duration);
                break;
            case 8:
                PinMedal(text, duration);
                break;
            case 9:
            case 12:
            case 13:
            case 14:
            case 15:
                DrawFuneralLongShot(previousShot, text, duration);
                break;
            case 10:
                DebriefingEstablishingShot(text, duration);
                break;
            case 16:
                DrawMedalChest(text, duration);
                break;
            case 17:
                funeral_wingman(text, duration);
                break;
            case 50:
                PlaySceneAnimation(text, (short)(shot - 50), duration);
                break;
            default:
                LongTalk(pTalkHeadShape, text,
                         pSceneMouthAnimCmds,
                         pFaceAnimationCmds,
                         duration);
                break;
            }
        }
        record++;
        if (bEscapePressedFlag == 1)
            break;
    }
scene_complete:
    ClearViewport(&stCurrentConvTextViewport,
                  cBlackColour);
    FreeTalker();
    SetEventManagerPump(0);
    if (bConversationConstellation == 1) {
        free_constellation();
        bConversationConstellation = 0;
    }
    return 0;
}

/* Function start: 0x439070 */
unsigned int DrawPodiumShot(void)
{
    if (pTalkHeadShape != 0)
        FreePacketAndClear(&pTalkHeadShape, 0);
    if (pSceneBriefingBodyShape != 0)
        FreePacketAndClear(&pSceneBriefingBodyShape, 0);
    if (pSceneBriefingPortraitShape != 0)
        FreePacketAndClear(&pSceneBriefingPortraitShape, 0);
    pTalkHeadShape =
        FetchDiskPacketRetrying(4, 2, 0);
    DrawSpriteDefault(&stSceneBuffer, -96, 0,
                      pCurrentConversationBackdropShapePtr, 1);
    DrawSpriteDefault(&stSceneBuffer, 224, 0,
                      pCurrentConversationBackdropShapePtr, 2);
    DrawSpriteDefault(&stSceneBuffer, 240, 127,
                      pTalkHeadShape, 0);
    DrawSpriteDefault(&stSceneBuffer, 252, 127,
                      pCurrentConversationBackdropShapePtr, 3);
    RefreshMemoryStatusOverlay();
    FreePacketAndClear(&pTalkHeadShape, 0);
    return 0;
}

/* Function start: 0x439150 */
unsigned int DrawBriefingCharacter(short character, short pose,
                                   short animationFrame,
                                   const signed char *unusedXOffsets,
                                   const signed char *unusedYOffsets)
{
    short frame;

    (void)unusedXOffsets;
    (void)unusedYOffsets;
    if (aBriefingCharacters[character].visible != 0) {
        frame = aBriefingCharacters[character].firstPortraitFrame;
        if (animationFrame <
            aBriefingCharacters[character].portraitFrameCount)
            frame = (short)(frame + animationFrame);
        DrawSpriteScaled(
            &stSceneBuffer,
            (short)(aBriefingCharacters[character].portraitX +
                    aBriefingPortraitOffsetX[character][pose]),
            (short)(aBriefingCharacters[character].portraitY +
                    aBriefingPortraitOffsetY[character][pose]),
            pSceneBriefingPortraitShape, frame,
            aBriefingPortraitScale[character][pose],
            aBriefingCharacters[character].scale, 0);
        DrawSpriteScaled(&stSceneBuffer,
                         aBriefingCharacters[character].bodyX,
                         (short)(aBriefingCharacters[character].bodyY +
                                 10),
                         pSceneBriefingBodyShape, pose,
                         0, aBriefingCharacters[character].scale, 0);
    }
    return 0;
}

/* Function start: 0x439220 */
unsigned int DrawFuneralLongShot(short shot, char *text, short duration)
{
    short escaped;

    AddPCName(text);
    ClearViewport(&stCurrentConvTextViewport,
                  cBlackColour);
    FormatTextBufferFromStart("%X%Y%F%s%P",
                              0, 160,
                              nConvTextColorValue,
                              szTxtWorkspaceBuffer);
    if (shot == 9) {
        DrawSpriteDefault(&stSceneBuffer, 0, 0,
                          pCurrentConversationBackdropShapePtr, 0);
        RefreshMemoryStatusOverlay();
        DIBslam();
        DIBslamReal();
        WaitForSceneAdvance(duration, 0);
        return 0;
    }
    SetFrameTimerPeriodDirect(duration);
    for (;;) {
        escaped = IsFrameTickElapsed();
        if (escaped != 0)
            break;
        if (bConversationConstellation == 1)
            DrawConstellationField();
        DrawSpriteDefault(&stSceneBuffer, 0, 0,
                          pCurrentConversationBackdropShapePtr, 3);
        DrawSpriteDefault(&stSceneBuffer, 0, 0,
                          pCurrentConversationBackdropShapePtr,
                          (short)(shot - 8));
        DrawSpriteDefault(&stSceneBuffer, 80, 127,
                          pCurrentConversationBackdropShapePtr, 8);
        RefreshMemoryStatusOverlay();
        DIBslam();
        DIBslamReal();
        escaped = CheckEscaped();
        if (escaped != 0)
            break;
    }
    return 0;
}

/* Function start: 0x439350 */
/* Nothing in the shipped executable calls this.  It shows the off-screen
 * buffer the DOS way: CGA, Hercules, Tandy and VGA copy it to the screen,
 * while EGA (mode 0Dh) swaps the two display pages. */
void ShowOffscreenBuffer(void)
{
    unsigned short pixels;
    unsigned short *rowOffsets;

    switch (nVideoMode) {
    case 4:
    case 7:
    case 9:
    case 0x13:
        CopyViewportContents((Viewport *)pFullScreenRecord,
                             pAllocatedScreenViewport);
        break;
    case 0xd:
        pixels = (unsigned short)((Viewport *)pFullScreenRecord)->pixels;
        rowOffsets = ((Viewport *)pFullScreenRecord)->rowOffsets;
        ((Viewport *)pFullScreenRecord)->pixels =
            pAllocatedScreenViewport->pixels;
        ((Viewport *)pFullScreenRecord)->rowOffsets =
            pAllocatedScreenViewport->rowOffsets;
        pAllocatedScreenViewport->pixels = (unsigned char *)pixels;
        pAllocatedScreenViewport->rowOffsets = rowOffsets;
        WaitForVerticalBlankThunk();
        break;
    }
}

/* Function start: 0x439400 */
void __stdcall SetViewportRect(Viewport *viewport, unsigned short left,
                               unsigned short top, unsigned short right,
                               unsigned short bottom)
{
    viewport->left = (short)left;
    viewport->top = (short)top;
    viewport->right = (short)right;
    viewport->bottom = (short)bottom;
}

/* Function start: 0x439430 */
void __stdcall PanToScreen(Viewport *source, Viewport *destination)
{
    unsigned short target[3];
    short *originalPalette;
    short *transitionPalette;
    unsigned int paletteBytes;
    short index;
    short activeCount;
    unsigned char *indices;

    if (nVideoMode == 0x13) {
        indices = AllocateTaggedMemory(256, 0);
        if (indices == 0)
            return;
        memset(indices, 0, 256);
        activeCount = CollectActivePaletteIndices(source, indices, 256);
        paletteBytes = (unsigned int)(activeCount * 6);
        originalPalette =
            AllocateTaggedMemory(paletteBytes, 0);
        transitionPalette =
            AllocateTaggedMemory(paletteBytes, 0);
        if (originalPalette == 0 || transitionPalette == 0) {
            ReleasePacketHandle(indices);
            if (originalPalette != 0)
                ReleasePacketHandle(originalPalette);
            if (transitionPalette != 0)
                ReleasePacketHandle(transitionPalette);
            return;
        }

        memset(originalPalette, 0, paletteBytes);
        memset(transitionPalette, 0, paletteBytes);
        GetPaletteEntry(
            (short)GetViewportPixel(destination,
                                    destination->left,
                                    destination->top),
            target);
        for (index = 0; index < activeCount; index++) {
            GetPaletteEntry(
                (short)indices[index],
                (unsigned short *)&originalPalette[index * 3]);
            CachePaletteEntryFromWords((short)indices[index], target);
            memcpy(&transitionPalette[index * 3], target, 6);
        }

        WaitForVerticalBlankThunk();
        DIBramPalette();
        CopyViewportContents(source, destination);
        DIBslam();
        DIBslamReal();

        while (StepPaletteTransition(
                   transitionPalette, originalPalette,
                   (short)(activeCount * 3)) != 0) {
            for (index = 0; index < activeCount; index++) {
                CachePaletteEntryFromWords(
                    (short)indices[index],
                    (unsigned short *)&transitionPalette[index * 3]);
            }
            WaitForVerticalBlankThunk();
            DIBramPalette();
        }

        ReleasePacketHandle(transitionPalette);
        ReleasePacketHandle(originalPalette);
        ReleasePacketHandle(indices);
    } else {
        CopyViewportContents(source, destination);
    }
    DIBslam();
    DIBslamReal();
}

/* Function start: 0x439660 (Mac symbol: death_sequence) */
unsigned int death_sequence(void)
{
    unsigned char *deathShape;
    unsigned char *cockpitBackground;
    signed char frame;

    nCannedSceneMode = 1;
    free_all_slots();
    free_cockpit();
    StopMusicUnlessSuppressed();
    if (nMemoryConfig == 1)
        SceneLeaveHook(0x20);

    spacetrack(0x20, 2, 1);
    frame = 0;
    deathShape = FetchDiskPacketRetrying(2, 0, 0);
    cockpitBackground = FetchDiskPacketRetrying(
        (short)cCockpitLogicalFile, 3, 0);
    PlaySfxWaveFileByNumber(4, -1, 0);
    new_view(9, 0);
    bEscapePressedFlag = 0;
    nFrameSkipCounter = 1;
    for (; frame < 8; frame++) {
        if (frame == 7) {
            ClearViewport(&stSpaceBufferViewport,
                          cViewportClearColour);
        } else {
            RefreshCockpitStatus();
            DrawSpriteDefault(&stSpaceBufferViewport, 0, 0,
                              cockpitBackground, 0);
        }
        DrawSpriteDefault(&stSpaceBufferViewport, 160, 199,
                          deathShape, (short)frame);
        dump_buffer_to_screen();
        DIBslam();
        DIBslamReal();
        if (bEscapePressedFlag == 1)
            break;
    }

    GetScreenUpdateFlag();
    FreePacketAndClear(&cockpitBackground, 0);
    FreePacketAndClear(&deathShape, 0);
    if (bEscapePressedFlag != 1) {
        frame = 0;
        load_all_slots();
        new_view(4, 0);
        nFrameSkipCounter = 1;
        do {
            if (RefreshCockpitStatus() != 0)
                dump_buffer_to_screen();
            switch (frame) {
            case 2:
                Explosion(0);
                break;
            }
            if (bEscapePressedFlag == 1)
                break;
            frame++;
            DIBslam();
            DIBslamReal();
        } while (frame < 60);
    }

    bEscapePressedFlag = 0;
    free_all_slots();
    stScreenView.top = 0;
    stScreenView.bottom = 199;
    FadeViewportPaletteToColour(&stScreenView, cBlackColour, 1);
    ClearViewport(&stScreenView, cBlackColour);
    RestoreGamePalette();
    return 0;
}

/* Function start: 0x439840 */
unsigned int ShowGetReadyScreen(void)
{
    short frame;
    short distance;

    frame = 0;
    pIntroFont =
        FetchDiskPacketRetrying(9, 1, 0);
    nCannedSceneMode = 1;
    force_view(0, 0);
    distance = 400;
    nFrameSkipCounter = 1;
    bEscapePressedFlag = 0;
    do {
        if (RefreshCockpitStatus() != 0) {
            DrawCenteredScaledIntroText(
                "Get Ready", nCurrentViewCenterPosX,
                nCurrentViewCenterPosY,
                (short)(0xc800 / (int)distance));
            dump_buffer_to_screen();
        }
        if (distance > 100)
            distance = (short)(distance - 10);
        if (bEscapePressedFlag == 1)
            break;
        frame++;
        DIBslam();
        DIBslamReal();
    } while (frame < 40);
    bEscapePressedFlag = 0;
    ReleasePacketHandle(pIntroFont);
    clear_view_buffer();
    nCannedSceneMode = 0;
    ResetSoundState();
    return 0;
}

/* Function start: 0x439910 */
unsigned int ShowVictoryScreen(void)
{
    short distance;
    short emptyCount;
    short frame;
    short index;

    emptyCount = 0;
    InitializeFireworks();
    pFireworkShape =
        FetchDiskPacketRetrying(9, 17, 0);
    pIntroFont =
        FetchDiskPacketRetrying(9, 1, 0);
    distance = 500;
    bEscapePressedFlag = 0;
    nFrameSkipCounter = 1;
    frame = 0;
    do {
        if (RandomBelowOrEqual(7) == 0 && emptyCount != 0) {
            for (index = 0; index < 30; index++) {
                if (aFireworksList[index].frame == -1) {
                    aFireworksList[index].frame = 0;
                    aFireworksList[index].x =
                        RandomInRange(0, stSpaceBufferViewport.right);
                    aFireworksList[index].y =
                        RandomInRange(0, stSpaceBufferViewport.bottom);
                    aFireworksList[index].variant =
                        RandomInRange(0, 2);
                    break;
                }
            }
        }
        if (RefreshCockpitStatus() != 0) {
            emptyCount = TheEndFireWorks(&stSpaceBufferViewport, 30);
            DrawCenteredScaledIntroText(
                "Victory", nCurrentViewCenterPosX,
                nCurrentViewCenterPosY,
                (short)(0xc800 / (int)distance));
            dump_buffer_to_screen();
        }
        if (bEscapePressedFlag == 1)
            break;
        if (distance > 100)
            distance = (short)(distance - 10);
        frame++;
        DIBslam();
        DIBslamReal();
    } while (frame < 80);
    bEscapePressedFlag = 0;
    ReleasePacketHandle(pIntroFont);
    ReleasePacketHandle(pFireworkShape);
    return 0;
}

/* Function start: 0x439A80 */
unsigned int ShowGameOverScreen(void)
{
    FixedVector cameraOffset;
    short frame;
    short distance;

    frame = 0;
    pIntroFont =
        FetchDiskPacketRetrying(9, 1, 0);
    cViewObject = (signed char)Explosion(0);
    nCameraViewMode = 4;
    asObjectCollisionRadius[EYE_OBJECT] = 100;
    ScaleFixedVector(
        &aShipFwdVec[cViewObject],
        -0x12c00, &cameraOffset);
    AddFixedVectors(
        &aShipPositionsArray[cViewObject],
        &cameraOffset, &aShipPositionsArray[EYE_OBJECT]);
    aShipUpVector[EYE_OBJECT] =
        aShipUpVector[cViewObject];
    aShipFwdVec[EYE_OBJECT] = cameraOffset;
    fix_objects_ijk(EYE_OBJECT);
    zero_vector(&aShipVelocity[EYE_OBJECT]);
    set_eye_direction_and_position();
    distance = 700;
    generate_stars();
    spacetrack(22, 2, 1);
    bEscapePressedFlag = 0;
    nFrameSkipCounter = 1;
    do {
        if (RefreshCockpitStatus() != 0) {
            if (frame > 20)
                DrawCenteredScaledIntroText(
                    "Game Over", nCurrentViewCenterPosX,
                    nCurrentViewCenterPosY,
                    (short)(0xc800 / (int)distance));
            dump_buffer_to_screen();
        }
        if (bEscapePressedFlag == 1)
            break;
        if (distance > 100)
            distance = (short)(distance - 10);
        frame++;
        DIBslam();
        DIBslamReal();
    } while (frame < 80);
    StopMusicUnlessSuppressed();
    bEscapePressedFlag = 0;
    ReleasePacketHandle(pIntroFont);
    return 0;
}

/* Function start: 0x439C10 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable.  It is compiled C, CL's inline memset expansion, and
 * the padding after it brings the raster library to 0x439C40. */
void __stdcall FillRasterBytes(void *destination, unsigned int length,
                               short value)
{
    memset(destination, value, length);
}

/* The raster library (0x439C40-0x43F635) was hand-written assembly; the Win32
 * build assembles it from src/raster.asm. */
#ifdef SDL_PORT
#include "screens_portable.inc"
#endif
