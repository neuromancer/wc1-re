/*
 *  Full-screen presentation screens.
 *
 *  Address range 0x437000-0x43cfff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: unbroken run of Blit* screens plus ShowGetReady/Victory/GameOver.
 */
#include "wc1.h"

/* Function start: 0x436D00 */
unsigned int LoadBriefingRoom(void)
{
    g_pConversationBackdropShape_00598c04 = 0;
    g_pBriefingAnimationShape_00598c14 = 0;
    g_pBriefingCloseupShape_00598c2c = 0;
    g_pBriefingBodyShape_00598c1c = 0;
    g_pBriefingPortraitShape_00598c24 = 0;
    InitializeConversationViewport();
    InitializeConversationText();
    SetTextContext(&g_stConversationTextContext_005a7760);
    spacetrack(0x19, 2, 1);
    g_pConversationBackdropShape_00598c04 =
        FetchDiskPacketRetrying(4, 0, 0);
    g_pBriefingAnimationShape_00598c14 =
        FetchDiskPacketRetrying(4, 1, 0);
    g_pBriefingCloseupShape_00598c2c =
        FetchDiskPacketRetrying(4, 3, 0);
    g_pBriefingBodyShape_00598c1c =
        FetchDiskPacketRetrying(4, 4, 0);
    g_pBriefingPortraitShape_00598c24 =
        FetchDiskPacketRetrying(4, 5, 0);
    SceneDirector(0, g_pBriefingSceneData_00598c00,
                  g_pBriefingTextData_00598af0);
    FreePacketAndClear(&g_pBriefingPortraitShape_00598c24, 8);
    FreePacketAndClear(&g_pBriefingBodyShape_00598c1c, 8);
    FreePacketAndClear(&g_pBriefingCloseupShape_00598c2c, 8);
    FreePacketAndClear(&g_pBriefingAnimationShape_00598c14, 8);
    FreePacketAndClear(&g_pConversationBackdropShape_00598c04, 8);
    ReleaseTextFont(0);
    ResetScreenClipToFullHeight();
    if (DAT_0059ab58 != 0) {
        StopMusicUnlessSuppressed();
        DAT_0059ab58 = 0;
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
    g_pMedalSceneShape_0046e2f4 =
        FetchDiskPacketRetrying(4, 8, 0);
    g_pConversationBackdropShape_00598c04 = 0;
    InitializeConversationText();
    ClearViewport(&DAT_005a76b0, DAT_0046999c);
    savedInputMode = g_bInputMode_0059a848;
    g_bInputMode_0059a848 = 1;
    do {
        PumpWindowMessages();
        if (PeekInputEvent(&event, 10) != 0 ||
            PeekInputEvent(&event, 2) != 0 ||
            PeekInputEvent(&event, 3) != 0)
            clicked = 1;
        DrawMedals();
        AddPCName(g_pszMedalsPilotSummary_0046e2f8);
        RefreshMemoryStatusOverlay();
        ClearViewport(&g_stConversationTextViewport_005a7570,
                      DAT_0046999c);
        FormatTextBufferFromStart(
            g_szViewMedalsTextFormat_0046e604, 0, 160,
            g_cViewportClearColour_004699a0,
            g_szTextScratchBuffer_00598b00);
        DIBslam();
        DIBslamReal();
        if (clicked != 0) {
            FreePacketAndClear(&g_pMedalSceneShape_0046e2f4, 8);
            WaitForInputKey();
            ClearInputKeyStatePreservingModifiers();
            g_bInputMode_0059a848 = savedInputMode;
            FlushInputEvents();
            return 0;
        }
    } while (1);
}

/* Function start: 0x436F50 */
unsigned int AwardCampaignMedal(short medal)
{
    int *packet;

    if (medal == 3 &&
        (signed char)g_stCampaignState_0059ca50.medals[3] > 0)
        return 0;

    PreloadMusicTrackHook(0x25);
    PreloadMusicTrackHook(0x26);
    PreloadMusicTrackHook(0x27);
    PreloadMusicTrackHook(0x28);

    switch (medal) {
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
        g_asCampaignBriefingFiles_00469458[g_nCampaignDataSet_005a8118],
        2, 0);
    g_pMedalSceneData_00598c20 = (unsigned char *)packet + packet[0];
    g_pMedalTextData_00598af8 = (unsigned char *)packet + packet[1];
    InitializeConversationViewport();
    InitializeConversationText();
    g_pMedalSceneShape_0046e2f4 =
        FetchDiskPacketRetrying(4, 8, 0);
    g_pConversationBackdropShape_00598c04 =
        FetchDiskPacketRetrying(4, 10, 0);
    g_nConversationMedalIndex_00598c08 = medal;
    g_stCampaignState_0059ca50.medals[medal]++;
    SceneDirector(5, g_pMedalSceneData_00598c20,
                  g_pMedalTextData_00598af8);
    DAT_0059ab58 = 0;
    FreePacketAndClear(&g_pConversationBackdropShape_00598c04, 8);
    FreePacketAndClear(&g_pMedalSceneShape_0046e2f4, 8);
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
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    offset = 0;
    FormatTextBufferFromStart(g_szMedalChestTextFormat_0046e610,
                              0, 160,
                              g_nConversationTextColour_00598c10,
                              g_szTextScratchBuffer_00598b00);
    do {
        DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                          g_pMedalSceneShape_0046e2f4, 41);
        DrawSpriteDefault(&DAT_005a76b0, (short)(92 - offset), 64,
                          g_pMedalSceneShape_0046e2f4, 43);
        DrawSpriteDefault(&DAT_005a76b0, (short)(228 + offset), 64,
                          g_pMedalSceneShape_0046e2f4, 44);
        DrawSpriteDefault(&DAT_005a76b0, 0, 64,
                          g_pMedalSceneShape_0046e2f4, 42);
        DrawSpriteScaled(&DAT_005a76b0, 319, 64,
                         g_pMedalSceneShape_0046e2f4, 42,
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
    switch (g_nConversationMedalIndex_00598c08) {
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
    short countdown;
    short escaped;
    short frame;
    short *cursor;
    short *start;

    countdown = 0;
    AddPCName(text);
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    FormatTextBufferFromStart(g_szMedalLongShotTextFormat_0046e61c,
                              0, 160,
                              g_nConversationTextColour_00598c10,
                              g_szTextScratchBuffer_00598b00);
    cursor = animation;
    start = animation;
    if (*cursor != -1) {
        for (;;) {
            if (countdown-- == 0) {
                if (*cursor != -1)
                    cursor += 2;
                if (*cursor == -2)
                    cursor = start;
                else if (*cursor == -1)
                    frame = -1;
                else {
                    frame = *cursor;
                    countdown = (short)(cursor[1] * 2);
                }
            }
            DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                              g_pConversationBackdropShape_00598c04, 0);
            DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                              g_pMedalSceneShape_0046e2f4, 0);
            if (frame > -1)
                DrawSpriteDefault(&DAT_005a76b0, 121, 8,
                                  g_pMedalSceneShape_0046e2f4,
                                  (short)(frame + 1));
            RefreshMemoryStatusOverlay();
            escaped = CheckEscaped();
            if (escaped != 0) {
                duration = -1;
                break;
            }
            DIBslam();
            DIBslamReal();
            if (*cursor == -1)
                break;
        }
    }
    DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                      g_pConversationBackdropShape_00598c04, 0);
    DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                      g_pMedalSceneShape_0046e2f4, 0);
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
    short x;
    short y;

    distance = 200;
    AddPCName(text);
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    frame = 0;
    FormatTextBufferFromStart(g_szMedalEstablishTextFormat_0046e628,
                              0, 160,
                              g_nConversationTextColour_00598c10,
                              g_szTextScratchBuffer_00598b00);
    x = g_asMedalDisplayX_0046e2d0[g_nConversationMedalIndex_00598c08];
    y = 87;
    do {
        DrawMedals();
        DrawSpriteScaled(&DAT_005a76b0, x, y,
                         g_pMedalSceneShape_0046e2f4, 12,
                         0, (short)(0xc800L / distance), 0);
        distance--;
        x--;
        y = (short)(y + 2);
        RefreshMemoryStatusOverlay();
        escaped = CheckEscaped();
        if (escaped != 0) {
            duration = -1;
            break;
        }
        frame++;
        DIBslam();
        DIBslamReal();
    } while (frame < 32);
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
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    FormatTextBufferFromStart(g_szPinMedalTextFormat_0046e634,
                              0, 160,
                              g_nConversationTextColour_00598c10,
                              g_szTextScratchBuffer_00598b00);
    ClearViewport(&DAT_005a76b0, DAT_0046999c);
    SetFrameTimerPeriodDirect(duration);
    escaped = IsFrameTickElapsed();
    while (escaped == 0) {
        frame++;
        DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                          g_pConversationBackdropShape_00598c04, 0);
        DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                          g_pMedalSceneShape_0046e2f4, 0);
        DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                          g_pMedalSceneShape_0046e2f4,
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
    short stack;
    short x;

    rowY = 78;
    x = 188;
    DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                      g_pConversationBackdropShape_00598c04, 1);
    DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                      g_pMedalSceneShape_0046e2f4, 11);
    DrawSpriteDefault(&DAT_005a76b0, 253, 38,
                      g_pMedalSceneShape_0046e2f4,
                      (short)(g_stCampaignState_0059ca50.currentPilot->rank +
                              33));
    DrawSpriteScaled(&DAT_005a76b0, 67, 38,
                     g_pMedalSceneShape_0046e2f4,
                     (short)(g_stCampaignState_0059ca50.currentPilot->rank +
                             33),
                     0, 255, 16);
    badge = 0;
    do {
        if (g_stCampaignState_0059ca50.badges[badge] != 0) {
            if (x > 231) {
                rowY = (short)(rowY + 3);
                x = 188;
            }
            DrawSpriteDefault(&DAT_005a76b0, x, rowY,
                              g_pMedalSceneShape_0046e2f4,
                              (short)(badge + 13));
            x = (short)(x + 11);
        }
        badge++;
    } while (badge < 12);
    rowY = (short)(rowY + 5);
    medal = 0;
    do {
        if (g_stCampaignState_0059ca50.medals[medal] != 0) {
            x = g_asMedalDisplayX_0046e2d0[medal];
            stack = rowY;
            if (medal < 3) {
                badge = 0;
                while (badge < (signed char)
                                   g_stCampaignState_0059ca50.medals[medal]) {
                    badge++;
                    DrawSpriteDefault(&DAT_005a76b0, x, stack,
                                      g_pMedalSceneShape_0046e2f4,
                                      (short)(medal + 25));
                    stack = (short)(stack + 2);
                }
            }
            DrawSpriteDefault(&DAT_005a76b0, x, stack,
                              g_pMedalSceneShape_0046e2f4,
                              (short)(medal + 28));
        }
        medal++;
    } while (medal < 5);
    DIBslam();
    DIBslamReal();
    return 0;
}

/* Function start: 0x437770 */
unsigned int EstablishingShot(char *text, short duration)
{
    short character;
    short escaped;
    short frame;

    if (g_pBriefingBodyShape_00598c1c == 0)
        g_pBriefingBodyShape_00598c1c =
            FetchDiskPacketRetrying(4, 4, 0);
    if (g_pBriefingPortraitShape_00598c24 == 0)
        g_pBriefingPortraitShape_00598c24 =
            FetchDiskPacketRetrying(4, 5, 0);
    AddPCName(text);
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    FormatTextBufferFromStart(g_szEstablishingShotTextFormat_0046e640,
                              0, 160,
                              g_nConversationTextColour_00598c10,
                              g_szTextScratchBuffer_00598b00);
    frame = 0;
    FlushInputEvents();
    do {
        DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                          g_pConversationBackdropShape_00598c04, 0);
        DrawSpriteDefault(&DAT_005a76b0, 241, 60,
                          g_pBriefingAnimationShape_00598c14, frame);
        DrawSpriteDefault(&DAT_005a76b0, 241, 64,
                          g_pBriefingAnimationShape_00598c14, 22);
        character = 0;
        do {
            DrawBriefingCharacter(
                character, 0,
                g_aBriefingCharacters_0046e218[character]
                    .animation[frame],
                0, 0);
            character++;
        } while (character < 8);
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

    if (g_pBriefingBodyShape_00598c1c == 0)
        g_pBriefingBodyShape_00598c1c =
            FetchDiskPacketRetrying(4, 4, 0);
    if (g_pBriefingPortraitShape_00598c24 == 0)
        g_pBriefingPortraitShape_00598c24 =
            FetchDiskPacketRetrying(4, 5, 0);
    DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                      g_pConversationBackdropShape_00598c04, 0);
    DrawSpriteDefault(&DAT_005a76b0, 241, 60,
                      g_pBriefingAnimationShape_00598c14, 21);
    DrawSpriteDefault(&DAT_005a76b0, 241, 64,
                      g_pBriefingAnimationShape_00598c14, 22);
    character = 0;
    do {
        DrawBriefingCharacter(character, 0, 0, 0, 0);
        character++;
    } while (character < 8);
    RefreshMemoryStatusOverlay();
    return 0;
}

/* Function start: 0x437980 */
unsigned int ReturnToBriefingLongShot(char *text, short duration)
{
    char active[10] = "";
    BriefingCharacterLayout *layout;
    short character;
    short escaped;
    short frame;

    spacetrack(26, 1, -1);
    if (g_pBriefingBodyShape_00598c1c == 0)
        g_pBriefingBodyShape_00598c1c =
            FetchDiskPacketRetrying(4, 4, 0);
    if (g_pBriefingPortraitShape_00598c24 == 0)
        g_pBriefingPortraitShape_00598c24 =
            FetchDiskPacketRetrying(4, 5, 0);
    AddPCName(text);
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    FormatTextBufferFromStart(g_szBriefingReturnTextFormat_0046e64c,
                              0, 160,
                              g_nConversationTextColour_00598c10,
                              g_szTextScratchBuffer_00598b00);
    character = 0;
    do {
        g_aBriefingCharacters_0046e218[character].animationPhase = 0;
        character++;
    } while (character < 8);
    frame = 0;
    for (;;) {
        DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                          g_pConversationBackdropShape_00598c04, 0);
        DrawSpriteDefault(&DAT_005a76b0, 241, 60,
                          g_pBriefingAnimationShape_00598c14, 0);
        DrawSpriteDefault(&DAT_005a76b0, 241, 64,
                          g_pBriefingAnimationShape_00598c14, 22);
        character = 0;
        do {
            layout = &g_aBriefingCharacters_0046e218[character];
            if (active[character] == 0 &&
                RandomBelowOrEqual(5) == 0)
                active[character] = 1;
            DrawBriefingCharacter(
                character, layout->animationPhase, 0,
                (const signed char *)g_aBriefingPortraitOffsetX_0046e300 +
                    layout->animationPhase * 12,
                (const signed char *)g_aBriefingPortraitOffsetY_0046e360 +
                    layout->animationPhase * 12);
            if (active[character] == 1 && layout->animationPhase < 11)
                layout->animationPhase++;
            character++;
        } while (character < 8);
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
        if (frame > 39)
            break;
    }
    WaitForSceneAdvance(duration, 0);
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    return 0;
}

/* Function start: 0x437B80 */
unsigned int Dismissed(char *text, short duration)
{
    short frame;
    short leftDelta;
    short leftX;
    short podiumDelta;
    short podiumFrame;
    short podiumX;
    short rightDelta;
    short rightX;

    podiumFrame = 0;
    rightX = 252;
    leftX = -96;
    podiumX = 240;
    if (g_pTalkingHeadShape_00598c0c != 0)
        FreePacketAndClear(&g_pTalkingHeadShape_00598c0c, 0);
    if (g_pBriefingBodyShape_00598c1c != 0)
        FreePacketAndClear(&g_pBriefingBodyShape_00598c1c, 0);
    if (g_pBriefingPortraitShape_00598c24 != 0)
        FreePacketAndClear(&g_pBriefingPortraitShape_00598c24, 0);
    AddPCName(text);
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    FormatTextBufferFromStart(g_szDismissedTextFormat_0046e658,
                              0, 160,
                              g_nConversationTextColour_00598c10,
                              g_szTextScratchBuffer_00598b00);
    frame = 0;
    g_pTalkingHeadShape_00598c0c =
        FetchDiskPacketRetrying(4, 2, 0);
    DAT_00469fb4 = 1;
    do {
        if (CheckEscaped() != 0) {
            rightX = 348;
            leftX = 0;
            frame = 31;
            podiumFrame = 34;
            podiumX = 336;
        }
        DAT_00469fb4--;
        if (DAT_00469fb4 < 1) {
            DAT_00469fb4 = g_nFrameSkip_00469fb8;
            DrawSpriteDefault(&DAT_005a76b0, leftX, 0,
                              g_pConversationBackdropShape_00598c04, 1);
            DrawSpriteDefault(&DAT_005a76b0, (short)(leftX + 320), 0,
                              g_pConversationBackdropShape_00598c04, 2);
            DrawSpriteDefault(&DAT_005a76b0, podiumX, 127,
                              g_pTalkingHeadShape_00598c0c,
                              g_abBriefingPodiumFrames_0046e510[
                                  podiumFrame]);
            DrawSpriteDefault(&DAT_005a76b0, rightX, 127,
                              g_pConversationBackdropShape_00598c04, 3);
            RefreshMemoryStatusOverlay();
        }
        if (podiumFrame < 34)
            podiumFrame++;
        if (frame < 12) {
            leftDelta =
                (short)g_aiBriefingLeftPanelVelocity_0046e480[frame];
            podiumDelta =
                (short)g_aiBriefingPodiumVelocity_0046e4b0[frame];
            rightDelta =
                (short)g_aiBriefingRightPanelVelocity_0046e4e0[frame];
        }
        if (frame > 24)
            leftDelta = (short)g_aiBriefingLeftPanelVelocity_0046e480[
                11 - (frame - 25)];
        podiumX = (short)(podiumX + podiumDelta);
        frame++;
        leftX = (short)(leftX + leftDelta);
        rightX = (short)(rightX + rightDelta);
        DIBslam();
        DIBslamReal();
    } while (frame < 32);
    WaitForSceneAdvance(duration, 0);
    FreePacketAndClear(&g_pTalkingHeadShape_00598c0c, 0);
    SetTextContext(&g_stConversationTextContext_005a7760);
    return 0;
}

/* Function start: 0x437DC0 */
unsigned int DrawDebriefingLongShot(void)
{
    DrawSpriteDefault(&DAT_005a76b0, g_nDebriefingLeftX_0046e56c, 0,
                      g_pConversationBackdropShape_00598c04, 2);
    DrawSpriteDefault(&DAT_005a76b0,
                      (short)(g_nDebriefingLeftX_0046e56c + 320), 0,
                      g_pConversationBackdropShape_00598c04, 3);
    DrawSpriteDefault(&DAT_005a76b0,
                      (short)(g_nDebriefingPilotX_0046e570 - 1), 127,
                      g_pConversationBackdropShape_00598c04, 4);
    DrawSpriteDefault(&DAT_005a76b0, g_nDebriefingPilotX_0046e570, 127,
                      g_pConversationBackdropShape_00598c04, 5);
    DrawSpriteDefault(&DAT_005a76b0, g_nDebriefingPodiumX_0046e57c, 127,
                      g_pConversationBackdropShape_00598c04, 8);
    if (g_stCampaignState_0059ca50.personalityDeathMission[
            g_nDebriefingPersonality_00465c80] == 0) {
        if (g_nDebriefingPersonality_00465c80 != 0)
            DrawSpriteDefault(
                &DAT_005a76b0, g_nDebriefingOfficerX_0046e578, 32,
                g_pConversationBackdropShape_00598c04,
                (short)(g_nDebriefingPersonality_00465c80 + 9));
        DrawSpriteDefault(&DAT_005a76b0,
                          g_nDebriefingOfficerX_0046e578, 32,
                          g_pConversationBackdropShape_00598c04, 6);
        if (g_nDebriefingPersonality_00465c80 == 0)
            DrawSpriteDefault(&DAT_005a76b0,
                              g_nDebriefingOfficerX_0046e578, 32,
                              g_pConversationBackdropShape_00598c04, 9);
    }
    DrawSpriteDefault(&DAT_005a76b0, g_nDebriefingRightX_0046e574, 127,
                      g_pConversationBackdropShape_00598c04, 7);
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

    g_nDebriefingPilotX_0046e570 = 80;
    g_nDebriefingRightX_0046e574 = 278;
    frame = 0;
    g_nDebriefingLeftX_0046e56c = 0;
    g_nDebriefingOfficerX_0046e578 = 200;
    g_nDebriefingPodiumX_0046e57c = 344;
    AddPCName(text);
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    FormatTextBufferFromStart(g_szDebriefEstablishTextFormat_0046e664,
                              0, 160,
                              g_nConversationTextColour_00598c10,
                              g_szTextScratchBuffer_00598b00);
    DAT_00469fb4 = 1;
    for (;;) {
        delta = &g_abDebriefingEstablishDeltas_0046e538[frame];
        g_nDebriefingLeftX_0046e56c =
            (short)(g_nDebriefingLeftX_0046e56c -
                    MaxShort((short)delta[0], 0));
        g_nDebriefingPilotX_0046e570 =
            (short)(g_nDebriefingPilotX_0046e570 -
                    MaxShort((short)(delta[0] + 1), 0));
        g_nDebriefingPodiumX_0046e57c =
            (short)(g_nDebriefingPodiumX_0046e57c -
                    MaxShort((short)(delta[0] + 2), 0));
        g_nDebriefingOfficerX_0046e578 =
            (short)(g_nDebriefingOfficerX_0046e578 -
                    MaxShort((short)(delta[0] + 3), 0));
        g_nDebriefingRightX_0046e574 =
            (short)(g_nDebriefingRightX_0046e574 -
                    MaxShort((short)(delta[0] + 3), 0));
        if (frame == 47)
            DAT_00469fb4 = 1;
        DAT_00469fb4--;
        if (DAT_00469fb4 < 1) {
            DAT_00469fb4 = g_nFrameSkip_00469fb8;
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
int no_objectives_achieved(void)
{
    short objective;

    objective = 0;
    while (objective < g_cMissionObjectiveCount_0059c46a) {
        if (achieved(objective) != 0)
            break;
        objective++;
    }
    return objective >= g_cMissionObjectiveCount_0059c46a;
}

/* Function start: 0x4380D0 */
short wing_status(short personality)
{
    int currentMission;
    int deathMission;

    deathMission =
        g_stCampaignState_0059ca50.personalityDeathMission[personality];
    if (deathMission == 0)
        return 3;
    currentMission = (int)g_stCampaignState_0059ca50.currentMission +
        (int)g_stCampaignState_0059ca50.currentSeries * 4;
    if (deathMission == currentMission)
        return 1;
    if (currentMission > deathMission)
        return 2;
    return currentMission;
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
    for (;;) {
        testCode = (short)*test++;
        if (testCode == 0)
            return record;
        switch (testCode) {
        case 1:
            first = int_value(&test);
            return sceneData + first;
        case 2:
            first = int_value(&test);
            second = int_value(&test);
            if (g_stCampaignState_0059ca50.missionScore < first)
                return sceneData + second;
            break;
        case 3:
            first = int_value(&test);
            second = int_value(&test);
            if (first <= g_stCampaignState_0059ca50.missionScore)
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
            if (g_nPlayerKillCount_005a7c9c == 0)
                return sceneData + first;
            break;
        case 7:
            first = int_value(&test);
            if (g_nPlayerKillCount_005a7c9c != 0)
                return sceneData + first;
            break;
        case 8:
            first = int_value(&test);
            if (g_nWingmanKillCount_005a7cb8 == 0)
                return sceneData + first;
            break;
        case 9:
            first = int_value(&test);
            if (g_nWingmanKillCount_005a7cb8 != 0)
                return sceneData + first;
            break;
        case 10:
            first = int_value(&test);
            if (DAT_004688cc == 0)
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
            if (g_nConversationMedalIndex_00598c08 == 4)
                return sceneData + first;
            break;
        case 14:
            first = int_value(&test);
            if (g_nConversationMedalIndex_00598c08 < 3)
                return sceneData + first;
            break;
        case 15:
            first = int_value(&test);
            if (g_nConversationMedalIndex_00598c08 == 3)
                return sceneData + first;
            break;
        case 16:
            first = int_value(&test);
            if (DAT_004688d0 != 1)
                return sceneData + first;
            break;
        case 17:
            first = int_value(&test);
            if (DAT_004688d4 == 0)
                return sceneData + first;
            break;
        case 18:
            first = int_value(&test);
            if (DAT_004688d4 == 1 &&
                g_stCampaignState_0059ca50.elapsedDate.year == 1)
                return sceneData + first;
            break;
        case 19:
            first = int_value(&test);
            if (DAT_004688d8 != 1)
                return sceneData + first;
            break;
        case 20:
            first = int_value(&test);
            if (g_stCampaignState_0059ca50.playerShipType != 0)
                return sceneData + first;
            break;
        case 21:
            first = int_value(&test);
            if (g_stCampaignState_0059ca50.playerShipType != 2)
                return sceneData + first;
            break;
        case 22:
            first = int_value(&test);
            if (g_stCampaignState_0059ca50.playerShipType != 3)
                return sceneData + first;
            break;
        case 23:
            first = int_value(&test);
            if (g_stCampaignState_0059ca50.playerShipType != 1)
                return sceneData + first;
            break;
        case 24:
            first = int_value(&test);
            if (g_stCampaignState_0059ca50.playerShipType != 1 &&
                g_stCampaignState_0059ca50.playerShipType < DAT_004688dc)
                return sceneData + first;
            break;
        case 25:
            first = int_value(&test);
            if (g_stCampaignState_0059ca50.playerShipType == 1 ||
                g_stCampaignState_0059ca50.playerShipType >= DAT_004688dc)
                return sceneData + first;
            break;
        case 26:
            first = int_value(&test);
            if (DAT_004688d4 == 1 &&
                g_stCampaignState_0059ca50.elapsedDate.year > 1)
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
}

/* Function start: 0x438B90 */
unsigned int TalkerInit(void)
{
    g_pFaceAnimationCommands_00598c18 =
        AllocateTaggedMemory(0x140, 0);
    g_pMouthAnimationCommands_00598af4 =
        AllocateTaggedMemory(0x140, 0);
    return 0;
}

/* Function start: 0x438BC0 */
unsigned int FreeTalker(void)
{
    FreePacketAndClear(&g_pConversationOverlayShape_00598c30, 0);
    FreePacketAndClear(&g_pTalkingHeadShape_00598c0c, 0);
    FreePacketAndClear(&g_pMouthAnimationCommands_00598af4, 0);
    FreePacketAndClear(&g_pFaceAnimationCommands_00598c18, 0);
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
    short previousShot;
    short shot;
    char *text;

    g_nConversationSceneType_00598c0a = sceneType;
    previousShot = -2;
    previousColour = -2;
    TalkerInit();
    g_bInputMode_0059a848 = 1;
    ClearInputKeyState();
    FlushInputEvents();
    SetEventManagerPump(PollJoystickButtonEvents);
    DAT_0059ab58 = 0;
    record = (ConversationSceneRecord *)sceneBytes;
    do {
        do {
            shot = (short)record->shot;
            if (shot == -2)
                goto scene_complete;
            if (shot != -1) {
                if ((shot & 0x40) != 0) {
                    g_bConversationOverlay_0046e590 = 1;
                    shot &= 0x3f;
                } else {
                    g_bConversationOverlay_0046e590 = 0;
                }
            }
            selected = record;
            if (record->testsOffset != 0)
                record = ParseTests(record,
                                    (ConversationSceneRecord *)sceneBytes,
                                    textData);
        } while (selected != record);
        if (record->talker != -2)
            g_nConversationCharacter_0046e580 = record->talker;
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
            g_nTalkingHeadFace_0046e584 = -1;
            previousShot = shot;
            break;
        case 1:
            if (previousShot != 1) {
                previousShot = 1;
                DrawBriefingLongShot();
                g_nTalkingHeadFace_0046e584 = -1;
            }
            break;
        case 2:
            if (previousShot != 2) {
                previousShot = 2;
                DrawPodiumShot();
                g_nTalkingHeadFace_0046e584 = -1;
            }
            break;
        case 4:
            previousShot = 4;
            if (g_nConversationCharacter_0046e580 < 0)
                g_cCurrentObjective_0046c020 =
                    (signed char)-g_nConversationCharacter_0046e580;
            else
                g_cCurrentObjective_0046c020 =
                    (signed char)g_nConversationCharacter_0046e580;
            g_nTalkingHeadFace_0046e584 = -1;
            break;
        case 12:
        case 13:
        case 14:
        case 15:
            if (previousShot != shot) {
                init_constellation(0);
                g_stConstellationViewport_005a6b40 = DAT_005a76b0;
                InitializeConstellationField(
                    &g_stConstellationViewport_005a6b40, -1, 16);
                g_bConversationConstellation_0046e58c = 1;
                g_nTalkingHeadFace_0046e584 = -1;
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
            g_nTalkingHeadFace_0046e584 = -1;
            break;
        }
        if (previousColour != (short)record->textColour &&
            record->textColour != -1) {
            g_nConversationTextColour_00598c10 =
                g_asConversationTextColours_004699f0[
                    (short)record->textColour];
            previousColour = record->textColour;
        }
        g_pMouthAnimationCommands_00598af4[0] = -1;
        if (textData[record->mouthAnimationOffset] != '\0')
            ParseMouthAnimation((char *)textData +
                                    record->mouthAnimationOffset,
                                g_pMouthAnimationCommands_00598af4);
        g_pFaceAnimationCommands_00598c18[0] = -1;
        if (textData[record->faceAnimationOffset] != '\0')
            ParseFaceAnimation((char *)textData +
                               record->faceAnimationOffset,
                               g_pFaceAnimationCommands_00598c18);
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
                CloseLook(g_pBriefingCloseupShape_00598c2c,
                          previousShot,
                          g_pMouthAnimationCommands_00598af4,
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
                DrawMedalLongShot(g_pMouthAnimationCommands_00598af4,
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
                LongTalk(g_pTalkingHeadShape_00598c0c, text,
                         g_pMouthAnimationCommands_00598af4,
                         g_pFaceAnimationCommands_00598c18,
                         duration);
                break;
            }
        }
        record++;
    } while (DAT_0059ab58 != 1);
scene_complete:
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    FreeTalker();
    SetEventManagerPump(0);
    if (g_bConversationConstellation_0046e58c == 1) {
        free_constellation();
        g_bConversationConstellation_0046e58c = 0;
    }
    return 0;
}

/* Function start: 0x439070 */
unsigned int DrawPodiumShot(void)
{
    if (g_pTalkingHeadShape_00598c0c != 0)
        FreePacketAndClear(&g_pTalkingHeadShape_00598c0c, 0);
    if (g_pBriefingBodyShape_00598c1c != 0)
        FreePacketAndClear(&g_pBriefingBodyShape_00598c1c, 0);
    if (g_pBriefingPortraitShape_00598c24 != 0)
        FreePacketAndClear(&g_pBriefingPortraitShape_00598c24, 0);
    g_pTalkingHeadShape_00598c0c =
        FetchDiskPacketRetrying(4, 2, 0);
    DrawSpriteDefault(&DAT_005a76b0, -96, 0,
                      g_pConversationBackdropShape_00598c04, 1);
    DrawSpriteDefault(&DAT_005a76b0, 224, 0,
                      g_pConversationBackdropShape_00598c04, 2);
    DrawSpriteDefault(&DAT_005a76b0, 240, 127,
                      g_pTalkingHeadShape_00598c0c, 0);
    DrawSpriteDefault(&DAT_005a76b0, 252, 127,
                      g_pConversationBackdropShape_00598c04, 3);
    RefreshMemoryStatusOverlay();
    FreePacketAndClear(&g_pTalkingHeadShape_00598c0c, 0);
    return 0;
}

/* Function start: 0x439150 */
unsigned int DrawBriefingCharacter(short character, short pose,
                                   short animationFrame,
                                   const signed char *unusedXOffsets,
                                   const signed char *unusedYOffsets)
{
    BriefingCharacterLayout *layout;
    int offsetIndex;
    short frame;

    (void)unusedXOffsets;
    (void)unusedYOffsets;
    layout = &g_aBriefingCharacters_0046e218[character];
    if (layout->visible != 0) {
        frame = layout->firstPortraitFrame;
        if (animationFrame < layout->portraitFrameCount)
            frame = (short)(frame + animationFrame);
        offsetIndex = (int)character * 12 + (int)pose;
        DrawSpriteScaled(
            &DAT_005a76b0,
            (short)(layout->portraitX +
                    ((const signed char *)
                         g_aBriefingPortraitOffsetX_0046e300)[offsetIndex]),
            (short)(layout->portraitY +
                    ((const signed char *)
                         g_aBriefingPortraitOffsetY_0046e360)[offsetIndex]),
            g_pBriefingPortraitShape_00598c24, frame,
            ((const short *)g_aBriefingPortraitScale_0046e3c0)[offsetIndex],
            layout->scale, 0);
        DrawSpriteScaled(&DAT_005a76b0,
                         layout->bodyX, (short)(layout->bodyY + 10),
                         g_pBriefingBodyShape_00598c1c, pose,
                         0, layout->scale, 0);
    }
    return 0;
}

/* Function start: 0x439220 */
unsigned int DrawFuneralLongShot(short shot, char *text, short duration)
{
    short escaped;

    AddPCName(text);
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    FormatTextBufferFromStart(g_szFuneralLongShotTextFormat_0046e670,
                              0, 160,
                              g_nConversationTextColour_00598c10,
                              g_szTextScratchBuffer_00598b00);
    if (shot == 9) {
        DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                          g_pConversationBackdropShape_00598c04, 0);
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
            return 0;
        if (g_bConversationConstellation_0046e58c == 1)
            DrawConstellationField();
        DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                          g_pConversationBackdropShape_00598c04, 3);
        DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                          g_pConversationBackdropShape_00598c04,
                          (short)(shot - 8));
        DrawSpriteDefault(&DAT_005a76b0, 80, 127,
                          g_pConversationBackdropShape_00598c04, 8);
        RefreshMemoryStatusOverlay();
        DIBslam();
        DIBslamReal();
        escaped = CheckEscaped();
        if (escaped != 0)
            return 0;
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
    unsigned char *indices;
    unsigned short target[3];
    short *originalPalette;
    short *transitionPalette;
    unsigned int paletteBytes;
    short activeCount;
    short index;

    if (DAT_0046b168 == 0x13) {
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

    g_nCannedSceneMode_00469fac = 1;
    free_all_slots();
    free_cockpit();
    StopMusicUnlessSuppressed();
    if (g_nMemoryConfiguration_005a7cd4 == 1)
        SceneLeaveHook(0x20);

    frame = 0;
    spacetrack(0x20, 2, 1);
    deathShape = FetchDiskPacketRetrying(2, 0, 0);
    cockpitBackground = FetchDiskPacketRetrying(
        (short)g_cCockpitLogicalFile_005a7c74, 3, 0);
    PlaySfxWaveFileByNumber(4, -1, 0);
    new_view(9, 0);
    DAT_0059ab58 = 0;
    DAT_00469fb4 = 1;
    do {
        if (frame == 7) {
            ClearViewport(&DAT_005a7510,
                          g_cViewportClearColour_004699a0);
        } else {
            RefreshCockpitStatus();
            DrawSpriteDefault(&DAT_005a7510, 0, 0,
                              cockpitBackground, 0);
        }
        DrawSpriteDefault(&DAT_005a7510, 160, 199,
                          deathShape, (short)frame);
        dump_buffer_to_screen();
        DIBslam();
        DIBslamReal();
        if (DAT_0059ab58 == 1)
            break;
        frame++;
    } while (frame < 8);

    GetScreenUpdateFlag();
    FreePacketAndClear(&cockpitBackground, 0);
    FreePacketAndClear(&deathShape, 0);
    if (DAT_0059ab58 != 1) {
        frame = 0;
        load_all_slots();
        new_view(4, 0);
        DAT_00469fb4 = 1;
        do {
            if (RefreshCockpitStatus() != 0)
                dump_buffer_to_screen();
            if (frame == 2)
                Explosion(0);
            if (DAT_0059ab58 == 1)
                break;
            frame++;
            DIBslam();
            DIBslamReal();
        } while (frame < 60);
    }

    DAT_0059ab58 = 0;
    free_all_slots();
    DAT_005a6ba0.top = 0;
    DAT_005a6ba0.bottom = 199;
    FadeViewportPaletteToColour(&DAT_005a6ba0, DAT_0046999c, 1);
    ClearViewport(&DAT_005a6ba0, DAT_0046999c);
    RestoreGamePalette();
    return 0;
}

/* Function start: 0x439840 */
unsigned int ShowGetReadyScreen(void)
{
    short frame;
    short distance;

    frame = 0;
    g_pIntroFont_005a8960 =
        FetchDiskPacketRetrying(9, 1, 0);
    distance = 400;
    g_nCannedSceneMode_00469fac = 1;
    force_view(0, 0);
    DAT_00469fb4 = 1;
    DAT_0059ab58 = 0;
    do {
        if (RefreshCockpitStatus() != 0) {
            DrawCenteredScaledIntroText(
                "Get Ready", g_nViewCenterX_0059a852,
                g_nViewCenterY_0059a854,
                (short)(0xc800 / (int)distance));
            dump_buffer_to_screen();
        }
        if (distance > 100)
            distance = (short)(distance - 10);
        if (DAT_0059ab58 == 1)
            break;
        frame++;
        DIBslam();
        DIBslamReal();
    } while (frame < 40);
    DAT_0059ab58 = 0;
    ReleasePacketHandle(g_pIntroFont_005a8960);
    clear_view_buffer();
    g_nCannedSceneMode_00469fac = 0;
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
    g_pFireworkShape_005a6a68 =
        FetchDiskPacketRetrying(9, 17, 0);
    distance = 500;
    g_pIntroFont_005a8960 =
        FetchDiskPacketRetrying(9, 1, 0);
    frame = 0;
    DAT_0059ab58 = 0;
    DAT_00469fb4 = 1;
    do {
        if (RandomBelowOrEqual(7) == 0 && emptyCount != 0) {
            index = 0;
            do {
                if (g_aFireworks_005a6900[index].frame == -1) {
                    g_aFireworks_005a6900[index].frame = 0;
                    g_aFireworks_005a6900[index].x =
                        RandomInRange(0, DAT_005a7510.right);
                    g_aFireworks_005a6900[index].y =
                        RandomInRange(0, DAT_005a7510.bottom);
                    g_aFireworks_005a6900[index].variant =
                        RandomInRange(0, 2);
                    break;
                }
                index++;
            } while (index < 30);
        }
        if (RefreshCockpitStatus() != 0) {
            emptyCount = TheEndFireWorks(&DAT_005a7510, 30);
            DrawCenteredScaledIntroText(
                "Victory", g_nViewCenterX_0059a852,
                g_nViewCenterY_0059a854,
                (short)(0xc800 / (int)distance));
            dump_buffer_to_screen();
        }
        if (DAT_0059ab58 == 1)
            break;
        if (distance > 100)
            distance = (short)(distance - 10);
        frame++;
        DIBslam();
        DIBslamReal();
    } while (frame < 80);
    DAT_0059ab58 = 0;
    ReleasePacketHandle(g_pIntroFont_005a8960);
    ReleasePacketHandle(g_pFireworkShape_005a6a68);
    return 0;
}

/* Function start: 0x439A80 */
unsigned int ShowGameOverScreen(void)
{
    FixedVector cameraOffset;
    short frame;
    short distance;

    frame = 0;
    g_pIntroFont_005a8960 =
        FetchDiskPacketRetrying(9, 1, 0);
    g_cViewObject_0046c000 = (signed char)Explosion(0);
    DAT_0046c03c = 4;
    g_asObjectCollisionRadius_0059d710[WC1_EYE_OBJECT] = 100;
    ScaleFixedVector(
        &g_aShipForwardVector_0059bce0[g_cViewObject_0046c000],
        -0x12c00, &cameraOffset);
    AddFixedVectors(
        &g_aShipPosition_0059c490[g_cViewObject_0046c000],
        &cameraOffset, &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
    g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT] =
        g_aShipUpVector_0059b9e0[g_cViewObject_0046c000];
    g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] = cameraOffset;
    fix_objects_ijk(WC1_EYE_OBJECT);
    zero_vector(&g_aShipVelocity_0059c010[WC1_EYE_OBJECT]);
    set_eye_direction_and_position();
    distance = 700;
    generate_stars();
    spacetrack(22, 2, 1);
    DAT_0059ab58 = 0;
    DAT_00469fb4 = 1;
    do {
        if (RefreshCockpitStatus() != 0) {
            if (frame > 20)
                DrawCenteredScaledIntroText(
                    "Game Over", g_nViewCenterX_0059a852,
                    g_nViewCenterY_0059a854,
                    (short)(0xc800 / (int)distance));
            dump_buffer_to_screen();
        }
        if (DAT_0059ab58 == 1)
            break;
        if (distance > 100)
            distance = (short)(distance - 10);
        frame++;
        DIBslam();
        DIBslamReal();
    } while (frame < 80);
    StopMusicUnlessSuppressed();
    DAT_0059ab58 = 0;
    ReleasePacketHandle(g_pIntroFont_005a8960);
    return 0;
}

#ifndef WC1_SDL

/* Function start: 0x439C0E */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* The hot-patch entry followed by an unframed REP STOS implementation
 * identifies this raster-library primitive as hand-written assembly. */
__declspec(naked) void __stdcall FillRasterBytes(
    void *destination, unsigned int length, short value)
{
#ifdef _MSC_VER
#include "screens_fill_raster_bytes.inc"
#else
    return;
#endif
}

/* Function start: 0x439C3E */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* Preserve the explicit ES setup used by the hand-written driver glue. */
__declspec(naked) char *CopyRasterDriverName(void *const *callbacks)
{
#ifdef _MSC_VER
#include "screens_copy_raster_driver_name.inc"
#else
    return 0;
#endif
}

/* Function start: 0x439C69 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* Preserve the explicit ES setup used by the hand-written driver glue. */
__declspec(naked) void InstallRasterDriverCallbacks(
    void *const *callbacks)
{
#ifdef _MSC_VER
#include "screens_install_raster_driver_callbacks.inc"
#else
    return;
#endif
}

/* Function start: 0x439C88 */
/* The explicit segment setup and symmetric read primitive at 0x439D63 show
 * that this raster-library pixel writer was hand-written assembly. */
__declspec(naked) unsigned int SetRasterClipPixel(
    RasterClip *clip, int x, int y, int colour)
{
    __asm {
        push ebp
        mov ebp, esp
        add esp, -20h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp + 8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx + 4]
        inc eax
        mov dword ptr [ebp - 18h], eax
        jle write_invalid_surface
        mov eax, dword ptr [ebx + 8]
        inc eax
        mov ecx, eax
        jle write_invalid_surface
        mov eax, dword ptr [esi + 4]
        mov dword ptr [ebp - 1ch], eax
        cmp eax, 0
        jg write_left_clipped
        mov eax, 0
write_left_clipped:
        mov dword ptr [ebp - 4], eax
        mov eax, dword ptr [esi + 8]
        mov dword ptr [ebp - 20h], eax
        cmp eax, 0
        jg write_top_clipped
        mov eax, 0
write_top_clipped:
        mov dword ptr [ebp - 8], eax
        mov eax, dword ptr [esi + 0ch]
        mov edx, dword ptr [ebp - 18h]
        dec edx
        cmp eax, edx
        jl write_right_clipped
        mov eax, edx
write_right_clipped:
        mov dword ptr [ebp - 0ch], eax
        mov eax, dword ptr [esi + 10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl write_bottom_clipped
        mov eax, edx
write_bottom_clipped:
        mov dword ptr [ebp - 10h], eax
        mov eax, dword ptr [ebp - 0ch]
        cmp eax, dword ptr [ebp - 4]
        jl write_invalid_clip
        mov eax, dword ptr [ebp - 10h]
        cmp eax, dword ptr [ebp - 8]
        jl write_invalid_clip
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp - 14h], eax
        jmp write_point
write_invalid_surface:
        mov eax, 0ffffffffh
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
write_invalid_clip:
        mov eax, 0fffffffeh
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
write_point:
        mov ecx, dword ptr [ebp + 0ch]
        mov ebx, dword ptr [ebp + 10h]
        add ecx, dword ptr [ebp - 1ch]
        add ebx, dword ptr [ebp - 20h]
        cmp ecx, dword ptr [ebp - 4]
        jl write_point_outside
        cmp ecx, dword ptr [ebp - 0ch]
        jg write_point_outside
        cmp ebx, dword ptr [ebp - 8]
        jl write_point_outside
        cmp ebx, dword ptr [ebp - 10h]
        jg write_point_outside
        mov eax, ebx
        imul dword ptr [ebp - 18h]
        add eax, dword ptr [ebp - 14h]
        add eax, ecx
        mov ebx, eax
        xor eax, eax
        mov al, byte ptr [ebx]
        mov dl, byte ptr [ebp + 14h]
        mov byte ptr [ebx], dl
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
write_point_outside:
        mov eax, 0fffffffdh
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
    }
}

/* Function start: 0x439D63 */
__declspec(naked) int ReadRasterClipPixel(RasterClip *clip, int x, int y)
{
    __asm {
        push ebp
        mov ebp, esp
        add esp, -20h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp + 8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx + 4]
        inc eax
        mov dword ptr [ebp - 18h], eax
        jle invalid_surface
        mov eax, dword ptr [ebx + 8]
        inc eax
        mov ecx, eax
        jle invalid_surface
        mov eax, dword ptr [esi + 4]
        mov dword ptr [ebp - 1ch], eax
        cmp eax, 0
        jg left_clipped
        mov eax, 0
left_clipped:
        mov dword ptr [ebp - 4], eax
        mov eax, dword ptr [esi + 8]
        mov dword ptr [ebp - 20h], eax
        cmp eax, 0
        jg top_clipped
        mov eax, 0
top_clipped:
        mov dword ptr [ebp - 8], eax
        mov eax, dword ptr [esi + 0ch]
        mov edx, dword ptr [ebp - 18h]
        dec edx
        cmp eax, edx
        jl right_clipped
        mov eax, edx
right_clipped:
        mov dword ptr [ebp - 0ch], eax
        mov eax, dword ptr [esi + 10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl bottom_clipped
        mov eax, edx
bottom_clipped:
        mov dword ptr [ebp - 10h], eax
        mov eax, dword ptr [ebp - 0ch]
        cmp eax, dword ptr [ebp - 4]
        jl invalid_clip
        mov eax, dword ptr [ebp - 10h]
        cmp eax, dword ptr [ebp - 8]
        jl invalid_clip
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp - 14h], eax
        jmp read_point
invalid_surface:
        mov eax, 0ffffffffh
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
invalid_clip:
        mov eax, 0fffffffeh
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
read_point:
        mov ecx, dword ptr [ebp + 0ch]
        mov ebx, dword ptr [ebp + 10h]
        add ecx, dword ptr [ebp - 1ch]
        add ebx, dword ptr [ebp - 20h]
        cmp ecx, dword ptr [ebp - 4]
        jl point_outside
        cmp ecx, dword ptr [ebp - 0ch]
        jg point_outside
        cmp ebx, dword ptr [ebp - 8]
        jl point_outside
        cmp ebx, dword ptr [ebp - 10h]
        jg point_outside
        mov eax, ebx
        imul dword ptr [ebp - 18h]
        add eax, dword ptr [ebp - 14h]
        add eax, ecx
        mov ebx, eax
        xor eax, eax
        mov al, byte ptr [ebx]
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
point_outside:
        mov eax, 0fffffffdh
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
    }
}

/* Function start: 0x439E39 */
/* The segment setup, fixed-point clipping, and unrolled drawing paths identify
 * the original as hand-written raster assembly.  Preserve those paths exactly;
 * mode 0 writes a colour, mode 1 translates through a colour table, and higher
 * modes call the callback passed in the colour argument. */
#pragma optimize("", off)
__declspec(naked) unsigned int DrawClippedLine(
    RasterClip *clip, int x1, int y1, int x2, int y2, int mode, int colour)
{
#ifdef _MSC_VER
#include "screens_draw_clipped_line.inc"
#else
    return 0;
#endif
}
#pragma optimize("", on)

/* Function start: 0x43A83B */
/* Original hand-written alternating-pixel rectangle fill used for
 * checkerboard shading inside a clipped raster surface. */
#pragma optimize("", off)
__declspec(naked) int FillRasterClipCheckerboard(
    RasterClip *clip, int left, int top, int right, int bottom,
    unsigned char colour)
{
    __asm {
        push ebp
        mov ebp,esp
        add esp,-0x20
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi,dword ptr [ebp + 0x8]
        mov ebx,dword ptr [esi]
        mov eax,dword ptr [ebx + 0x4]
        inc eax
        mov dword ptr [ebp - 0x18],eax
        jle checker_43a8ba
        mov eax,dword ptr [ebx + 0x8]
        inc eax
        mov ecx,eax
        jle checker_43a8ba
        mov eax,dword ptr [esi + 0x4]
        mov dword ptr [ebp - 0x1c],eax
        cmp eax,0x0
        jg checker_43a86e
        mov eax,0x0
checker_43a86e:
        mov dword ptr [ebp - 0x4],eax
        mov eax,dword ptr [esi + 0x8]
        mov dword ptr [ebp - 0x20],eax
        cmp eax,0x0
        jg checker_43a881
        mov eax,0x0
checker_43a881:
        mov dword ptr [ebp - 0x8],eax
        mov eax,dword ptr [esi + 0xc]
        mov edx,dword ptr [ebp - 0x18]
        dec edx
        cmp eax,edx
        jl checker_43a891
        mov eax,edx
checker_43a891:
        mov dword ptr [ebp - 0xc],eax
        mov eax,dword ptr [esi + 0x10]
        mov edx,ecx
        dec edx
        cmp eax,edx
        jl checker_43a8a0
        mov eax,edx
checker_43a8a0:
        mov dword ptr [ebp - 0x10],eax
        mov eax,dword ptr [ebp - 0xc]
        cmp eax,dword ptr [ebp - 0x4]
        jl checker_43a8c5
        mov eax,dword ptr [ebp - 0x10]
        cmp eax,dword ptr [ebp - 0x8]
        jl checker_43a8c5
        mov eax,dword ptr [ebx]
        mov dword ptr [ebp - 0x14],eax
        jmp checker_43a8d0
checker_43a8ba:
        mov eax,0xffffffff
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
checker_43a8c5:
        mov eax,0xfffffffe
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
checker_43a8d0:
        mov eax,dword ptr [ebp - 0x1c]
        add dword ptr [ebp + 0xc],eax
        add dword ptr [ebp + 0x14],eax
        mov eax,dword ptr [ebp - 0x20]
        add dword ptr [ebp + 0x10],eax
        add dword ptr [ebp + 0x18],eax
        mov eax,dword ptr [ebp - 0x4]
        cmp dword ptr [ebp + 0xc],eax
        jg checker_43a8ed
        mov dword ptr [ebp + 0xc],eax
checker_43a8ed:
        mov eax,dword ptr [ebp - 0x8]
        cmp dword ptr [ebp + 0x10],eax
        jg checker_43a8f8
        mov dword ptr [ebp + 0x10],eax
checker_43a8f8:
        mov eax,dword ptr [ebp - 0xc]
        cmp dword ptr [ebp + 0x14],eax
        jl checker_43a903
        mov dword ptr [ebp + 0x14],eax
checker_43a903:
        mov eax,dword ptr [ebp - 0x10]
        cmp dword ptr [ebp + 0x18],eax
        jl checker_43a90e
        mov dword ptr [ebp + 0x18],eax
checker_43a90e:
        mov ecx,dword ptr [ebp + 0x14]
        sub ecx,dword ptr [ebp + 0xc]
        jl checker_43a969
        inc ecx
        mov eax,dword ptr [ebp + 0x10]
        imul dword ptr [ebp - 0x18]
        add eax,dword ptr [ebp - 0x14]
        add eax,dword ptr [ebp + 0xc]
        mov edi,eax
        mov edx,dword ptr [ebp + 0x18]
        sub edx,dword ptr [ebp + 0x10]
        jl checker_43a969
        mov eax,dword ptr [ebp + 0x1c]
        mov esi,edi
        mov ebx,ecx
        jmp checker_43a93d
checker_43a936:
        add esi,dword ptr [ebp - 0x18]
        mov edi,esi
        mov ecx,ebx
checker_43a93d:
        push edx
        and edx,0x1
        jz checker_43a948
        pop edx
        inc edi
        dec ecx
        jmp checker_43a949
checker_43a948:
        pop edx
checker_43a949:
        mov byte ptr [edi],al
        add edi,0x2
        sub ecx,0x2
        jg checker_43a949
        dec edx
        jns checker_43a936
        xor eax,eax
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
checker_43a969:
        mov eax,0xfffffffc
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
    }
}
#pragma optimize("", on)

/* Function start: 0x43A974 */
/* The segment setup, outcode construction, and REP run decoders identify
 * this as an original hand-written clipped raster routine. */
#pragma optimize("", off)
__declspec(naked) int DrawRLEImage(RasterClip *clip, unsigned char *shape,
                                    int frame, int x, int y)
{
    __asm {
        push ebp
        mov ebp,esp
        add esp,-0x50
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi,dword ptr [ebp + 0x8]
        mov ebx,dword ptr [esi]
        mov eax,dword ptr [ebx + 0x4]
        inc eax
        mov dword ptr [ebp - 0x48],eax
        jle plain_43a9f3
        mov eax,dword ptr [ebx + 0x8]
        inc eax
        mov ecx,eax
        jle plain_43a9f3
        mov eax,dword ptr [esi + 0x4]
        mov dword ptr [ebp - 0x4c],eax
        cmp eax,0x0
        jg plain_43a9a7
        mov eax,0x0
plain_43a9a7:
        mov dword ptr [ebp - 0x34],eax
        mov eax,dword ptr [esi + 0x8]
        mov dword ptr [ebp - 0x50],eax
        cmp eax,0x0
        jg plain_43a9ba
        mov eax,0x0
plain_43a9ba:
        mov dword ptr [ebp - 0x38],eax
        mov eax,dword ptr [esi + 0xc]
        mov edx,dword ptr [ebp - 0x48]
        dec edx
        cmp eax,edx
        jl plain_43a9ca
        mov eax,edx
plain_43a9ca:
        mov dword ptr [ebp - 0x3c],eax
        mov eax,dword ptr [esi + 0x10]
        mov edx,ecx
        dec edx
        cmp eax,edx
        jl plain_43a9d9
        mov eax,edx
plain_43a9d9:
        mov dword ptr [ebp - 0x40],eax
        mov eax,dword ptr [ebp - 0x3c]
        cmp eax,dword ptr [ebp - 0x34]
        jl plain_43a9fe
        mov eax,dword ptr [ebp - 0x40]
        cmp eax,dword ptr [ebp - 0x38]
        jl plain_43a9fe
        mov eax,dword ptr [ebx]
        mov dword ptr [ebp - 0x44],eax
        jmp plain_43aa09
plain_43a9f3:
        mov eax,0xffffffff
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
plain_43a9fe:
        mov eax,0xfffffffe
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
plain_43aa09:
        mov eax,dword ptr [ebp - 0x4c]
        add dword ptr [ebp + 0x14],eax
        mov eax,dword ptr [ebp - 0x50]
        add dword ptr [ebp + 0x18],eax
        mov esi,dword ptr [ebp + 0x10]
        shl esi,0x3
        add esi,0x8
        add esi,dword ptr [ebp + 0xc]
        mov esi,dword ptr [esi]
        add esi,dword ptr [ebp + 0xc]
        mov dword ptr [ebp - 0x30],esi
        mov eax,dword ptr [esi + 0x8]
        add eax,dword ptr [ebp + 0x14]
        mov dword ptr [ebp - 0xc],eax
        mov eax,dword ptr [esi + 0xc]
        add eax,dword ptr [ebp + 0x18]
        mov dword ptr [ebp - 0x10],eax
        mov eax,dword ptr [esi + 0x10]
        add eax,dword ptr [ebp + 0x14]
        mov dword ptr [ebp - 0x14],eax
        mov eax,dword ptr [esi + 0x14]
        add eax,dword ptr [ebp + 0x18]
        mov dword ptr [ebp - 0x18],eax
        add esi,0x18
        mov eax,dword ptr [ebp - 0x14]
        cmp eax,dword ptr [ebp - 0xc]
        jl plain_43ad6d
        mov eax,dword ptr [ebp - 0x18]
        cmp eax,dword ptr [ebp - 0x10]
        jl plain_43ad6d
        xor edx,edx
        mov eax,dword ptr [ebp - 0xc]
        sub eax,dword ptr [ebp - 0x34]
        shl eax,0x1
        adc dl,dl
        mov eax,dword ptr [ebp - 0x3c]
        sub eax,dword ptr [ebp - 0xc]
        shl eax,0x1
        adc dl,dl
        mov eax,dword ptr [ebp - 0x10]
        sub eax,dword ptr [ebp - 0x38]
        shl eax,0x1
        adc dl,dl
        mov eax,dword ptr [ebp - 0x40]
        sub eax,dword ptr [ebp - 0x10]
        shl eax,0x1
        adc dl,dl
        mov eax,dword ptr [ebp - 0x14]
        sub eax,dword ptr [ebp - 0x34]
        shl eax,0x1
        adc dh,dh
        mov eax,dword ptr [ebp - 0x3c]
        sub eax,dword ptr [ebp - 0x14]
        shl eax,0x1
        adc dh,dh
        mov eax,dword ptr [ebp - 0x18]
        sub eax,dword ptr [ebp - 0x38]
        shl eax,0x1
        adc dh,dh
        mov eax,dword ptr [ebp - 0x40]
        sub eax,dword ptr [ebp - 0x18]
        shl eax,0x1
        adc dh,dh
        mov dword ptr [ebp - 0x1c],edx
        test dh,dl
        jnz plain_43ad62
        or dl,dh
        jnz plain_43aaf4
        mov esi,dword ptr [ebp + 0x8]
        mov eax,dword ptr [esi + 0x4]
        sub dword ptr [ebp + 0x14],eax
        mov eax,dword ptr [esi + 0x8]
        sub dword ptr [ebp + 0x18],eax
        push dword ptr [ebp - 0x48]
        push dword ptr [ebp + 0x18]
        push dword ptr [ebp + 0x14]
        push dword ptr [ebp - 0x30]
        push dword ptr [ebp + 0x8]
        call DrawRLEImageUnclipped
        add esp,0x14
        jmp plain_43ad5a
plain_43aaf4:
        mov eax,dword ptr [ebp - 0x10]
        imul dword ptr [ebp - 0x48]
        add eax,dword ptr [ebp - 0x44]
        add eax,dword ptr [ebp - 0xc]
        mov edi,eax
        mov ecx,dword ptr [ebp - 0x10]
        mov dword ptr [ebp - 0x20],ecx
        jmp plain_43ab20
plain_43ab0a:
        movzx eax,al
        add esi,eax
        dec esi
plain_43ab10:
        inc esi
plain_43ab11:
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja plain_43ab10
        jnz plain_43ab0a
        jc plain_43ab10
        add edi,dword ptr [ebp - 0x48]
        inc ecx
plain_43ab20:
        cmp ecx,dword ptr [ebp - 0x38]
        jl plain_43ab11
        mov dword ptr [ebp - 0x24],edi
        mov dword ptr [ebp - 0x20],ecx
        mov eax,edi
        sub eax,dword ptr [ebp - 0xc]
        add eax,dword ptr [ebp - 0x34]
        mov dword ptr [ebp - 0x28],eax
        mov eax,edi
        sub eax,dword ptr [ebp - 0xc]
        add eax,dword ptr [ebp - 0x3c]
        mov dword ptr [ebp - 0x2c],eax
        jmp plain_43ad4e
plain_43ab46:
        mov eax,dword ptr [ebp - 0x20]
        cmp eax,dword ptr [ebp - 0x40]
        jg plain_43ad5a
        mov edi,dword ptr [ebp - 0x24]
        test dword ptr [ebp - 0x1c],0x8
        jnz plain_43abd3
        test dword ptr [ebp - 0x1c],0x400
        jnz plain_43ac5d
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja plain_43ab8b
        jnz plain_43abb4
        jnc plain_43abce
plain_43ab76:
        mov al,byte ptr [esi]
        inc esi
        movzx ecx,al
        add edi,ecx
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja plain_43ab8b
        jnz plain_43abb4
        jc plain_43ab76
        jnc plain_43abce
plain_43ab8b:
        movzx ecx,al
plain_43ab8e:
        mov al,byte ptr [esi]
        inc esi
        push ecx
        and ecx,0x3
        rep stosb
        mov ah,al
        rol eax,0x8
        mov al,ah
        rol eax,0x8
        mov al,ah
        pop ecx
        shr ecx,0x2
        rep stosd
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja plain_43ab8b
        jnc plain_43abce
        jz plain_43ab76
plain_43abb4:
        movzx ecx,al
plain_43abb7:
        push ecx
        and ecx,0x3
        rep movsb
        pop ecx
        shr ecx,0x2
        rep movsd
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja plain_43ab8b
        jnz plain_43abb4
        jc plain_43ab76
plain_43abce:
        jmp plain_43ad3f
plain_43abd3:
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja plain_43abf3
        jnz plain_43ac24
        jnc plain_43ac58
plain_43abde:
        mov al,byte ptr [esi]
        inc esi
        movzx ecx,al
        add edi,ecx
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja plain_43abf3
        jnz plain_43ac24
        jc plain_43abde
        jnc plain_43ac58
plain_43abf3:
        movzx ecx,al
        mov eax,dword ptr [ebp - 0x28]
        sub eax,edi
        cmp eax,ecx
        jge plain_43ac16
        or eax,eax
        js plain_43ac07
        add edi,eax
        sub ecx,eax
plain_43ac07:
        test dword ptr [ebp - 0x1c],0x400
        jz plain_43ab8e
        jnz plain_43ac84
plain_43ac16:
        add edi,ecx
        inc esi
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja plain_43abf3
        jnc plain_43ac58
        jz plain_43abde
plain_43ac24:
        movzx ecx,al
        mov eax,dword ptr [ebp - 0x28]
        sub eax,edi
        cmp eax,ecx
        jge plain_43ac49
        or eax,eax
        js plain_43ac3a
        add edi,eax
        sub ecx,eax
        add esi,eax
plain_43ac3a:
        test dword ptr [ebp - 0x1c],0x400
        jz plain_43abb7
        jnz plain_43acc7
plain_43ac49:
        add edi,ecx
        add esi,ecx
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja plain_43abf3
        jnz plain_43ac24
        jc plain_43abde
plain_43ac58:
        jmp plain_43ad3f
plain_43ac5d:
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja plain_43ac81
        jnz plain_43acc4
        jnc plain_43acfa
plain_43ac6c:
        mov al,byte ptr [esi]
        inc esi
        movzx ecx,al
        add edi,ecx
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja plain_43ac81
        jnz plain_43acc4
        jc plain_43ac6c
        jnc plain_43acfa
plain_43ac81:
        movzx ecx,al
plain_43ac84:
        cmp edi,dword ptr [ebp - 0x2c]
        jg plain_43ad1f
        mov eax,edi
        add eax,ecx
        dec eax
        sub eax,dword ptr [ebp - 0x2c]
        cdq
        not edx
        and edx,eax
        sub ecx,edx
        mov al,byte ptr [esi]
        inc esi
        push ecx
        and ecx,0x3
        rep stosb
        mov ah,al
        rol eax,0x8
        mov al,ah
        rol eax,0x8
        mov al,ah
        pop ecx
        shr ecx,0x2
        rep stosd
        add edi,edx
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja plain_43ac81
        jnc plain_43acfa
        jz plain_43ac6c
plain_43acc4:
        movzx ecx,al
plain_43acc7:
        cmp edi,dword ptr [ebp - 0x2c]
        jg plain_43ad30
        mov eax,edi
        add eax,ecx
        dec eax
        sub eax,dword ptr [ebp - 0x2c]
        cdq
        not edx
        and edx,eax
        sub ecx,edx
        push ecx
        and ecx,0x3
        rep movsb
        pop ecx
        shr ecx,0x2
        rep movsd
        add edi,edx
        add esi,edx
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja plain_43ac81
        jnz plain_43acc4
        jc plain_43ac6c
plain_43acfa:
        jmp plain_43ad3f
plain_43ad07:
        mov al,byte ptr [esi]
        inc esi
        movzx ecx,al
        add edi,ecx
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja plain_43ad1c
        jnz plain_43ad2d
        jc plain_43ad07
        jnc plain_43ad3f
plain_43ad1c:
        movzx ecx,al
plain_43ad1f:
        add edi,ecx
        inc esi
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja plain_43ad1c
        jnc plain_43ad3f
        jz plain_43ad07
plain_43ad2d:
        movzx ecx,al
plain_43ad30:
        add edi,ecx
        add esi,ecx
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja plain_43ad1c
        jnz plain_43ad2d
        jc plain_43ad07
plain_43ad3f:
        mov eax,dword ptr [ebp - 0x48]
        add dword ptr [ebp - 0x24],eax
        add dword ptr [ebp - 0x28],eax
        add dword ptr [ebp - 0x2c],eax
        inc dword ptr [ebp - 0x20]
plain_43ad4e:
        mov eax,dword ptr [ebp - 0x20]
        cmp eax,dword ptr [ebp - 0x18]
        jle plain_43ab46
plain_43ad5a:
        xor eax,eax
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
plain_43ad62:
        mov eax,0xfffffffd
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
plain_43ad6d:
        mov eax,0xfffffffc
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
    }
}
#pragma optimize("", on)

/* Function start: 0x43AD78 */
/* This is one of the original hand-written raster loops: it establishes ES,
 * decodes each prepared scan line, and uses REP stores/copies for the runs. */
#pragma optimize("", off)
__declspec(naked) int DrawRLEImageUnclipped(
    RasterClip *clip, RLEFrameHeader *frameHeader, int x, int y,
    int strideScratch)
{
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp + 8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [esi + 4]
        add dword ptr [ebp + 0x10], eax
        mov eax, dword ptr [esi + 8]
        add dword ptr [ebp + 0x14], eax
        mov eax, dword ptr [ebx + 4]
        inc eax
        mov dword ptr [ebp + 0x18], eax
        jle plain_done
        mov esi, dword ptr [ebp + 0xc]
        mov edi, dword ptr [ebx]
        mov eax, dword ptr [esi + 8]
        add eax, dword ptr [ebp + 0x10]
        add edi, eax
        mov eax, dword ptr [esi + 0xc]
        mov ebx, eax
        add eax, dword ptr [ebp + 0x14]
        mul dword ptr [ebp + 0x18]
        add edi, eax
        mov edx, edi
        mov eax, dword ptr [esi + 0x10]
        mov eax, dword ptr [esi + 0x14]
        inc eax
        sub eax, ebx
        mov ebx, eax
        jle plain_done
        add esi, 0x18
plain_next_row:
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja plain_solid
        jne plain_literal
        jae plain_row_done
plain_skip:
        mov al, byte ptr [esi]
        inc esi
        movzx ecx, al
        add edi, ecx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja plain_solid
        jne plain_literal
        jb plain_skip
        jae plain_row_done
plain_solid:
        movzx ecx, al
        mov al, byte ptr [esi]
        inc esi
        push ecx
        and ecx, 3
        rep stosb
        mov ah, al
        rol eax, 8
        mov al, ah
        rol eax, 8
        mov al, ah
        pop ecx
        shr ecx, 2
        rep stosd
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja plain_solid
        jae plain_row_done
        je plain_skip
plain_literal:
        movzx ecx, al
        push ecx
        and ecx, 3
        rep movsb
        pop ecx
        shr ecx, 2
        rep movsd
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja plain_solid
        jne plain_literal
        jb plain_skip
plain_row_done:
        add edx, dword ptr [ebp + 0x18]
        mov edi, edx
        dec ebx
        jne plain_next_row
plain_done:
        xor eax, eax
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
    }
}
#pragma optimize("", on)

/* Function start: 0x43AE3F */
/* The segment-register setup and fixed-size REP copy identify this as one of
 * the original hand-written raster support routines. */
__declspec(naked) void SetPaletteTranslationTable(
    const unsigned char *translation)
{
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp + 8]
        mov edi, offset g_abRasterPaletteTranslation_0046ff2c
        mov ecx, 0x40
        rep movsd
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
    }
}

/* Function start: 0x43AE5E */
/* Palette-translated counterpart to the original hand-written clipped
 * raster routine above. */
#pragma optimize("", off)
__declspec(naked) int DrawRLEImageColor(RasterClip *clip,
                                         unsigned char *shape,
                                         int frame, int x, int y)
{
    __asm {
        push ebp
        mov ebp,esp
        add esp,-0x50
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi,dword ptr [ebp + 0x8]
        mov ebx,dword ptr [esi]
        mov eax,dword ptr [ebx + 0x4]
        inc eax
        mov dword ptr [ebp - 0x48],eax
        jle color_43aedd
        mov eax,dword ptr [ebx + 0x8]
        inc eax
        mov ecx,eax
        jle color_43aedd
        mov eax,dword ptr [esi + 0x4]
        mov dword ptr [ebp - 0x4c],eax
        cmp eax,0x0
        jg color_43ae91
        mov eax,0x0
color_43ae91:
        mov dword ptr [ebp - 0x34],eax
        mov eax,dword ptr [esi + 0x8]
        mov dword ptr [ebp - 0x50],eax
        cmp eax,0x0
        jg color_43aea4
        mov eax,0x0
color_43aea4:
        mov dword ptr [ebp - 0x38],eax
        mov eax,dword ptr [esi + 0xc]
        mov edx,dword ptr [ebp - 0x48]
        dec edx
        cmp eax,edx
        jl color_43aeb4
        mov eax,edx
color_43aeb4:
        mov dword ptr [ebp - 0x3c],eax
        mov eax,dword ptr [esi + 0x10]
        mov edx,ecx
        dec edx
        cmp eax,edx
        jl color_43aec3
        mov eax,edx
color_43aec3:
        mov dword ptr [ebp - 0x40],eax
        mov eax,dword ptr [ebp - 0x3c]
        cmp eax,dword ptr [ebp - 0x34]
        jl color_43aee8
        mov eax,dword ptr [ebp - 0x40]
        cmp eax,dword ptr [ebp - 0x38]
        jl color_43aee8
        mov eax,dword ptr [ebx]
        mov dword ptr [ebp - 0x44],eax
        jmp color_43aef3
color_43aedd:
        mov eax,0xffffffff
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
color_43aee8:
        mov eax,0xfffffffe
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
color_43aef3:
        mov eax,dword ptr [ebp - 0x4c]
        add dword ptr [ebp + 0x14],eax
        mov eax,dword ptr [ebp - 0x50]
        add dword ptr [ebp + 0x18],eax
        mov esi,dword ptr [ebp + 0x10]
        shl esi,0x3
        add esi,0x8
        add esi,dword ptr [ebp + 0xc]
        mov esi,dword ptr [esi]
        add esi,dword ptr [ebp + 0xc]
        mov dword ptr [ebp - 0x30],esi
        mov eax,dword ptr [esi + 0x8]
        add eax,dword ptr [ebp + 0x14]
        mov dword ptr [ebp - 0xc],eax
        mov eax,dword ptr [esi + 0xc]
        add eax,dword ptr [ebp + 0x18]
        mov dword ptr [ebp - 0x10],eax
        mov eax,dword ptr [esi + 0x10]
        add eax,dword ptr [ebp + 0x14]
        mov dword ptr [ebp - 0x14],eax
        mov eax,dword ptr [esi + 0x14]
        add eax,dword ptr [ebp + 0x18]
        mov dword ptr [ebp - 0x18],eax
        add esi,0x18
        mov eax,dword ptr [ebp - 0x14]
        cmp eax,dword ptr [ebp - 0xc]
        jl color_43b32b
        mov eax,dword ptr [ebp - 0x18]
        cmp eax,dword ptr [ebp - 0x10]
        jl color_43b32b
        xor edx,edx
        mov eax,dword ptr [ebp - 0xc]
        sub eax,dword ptr [ebp - 0x34]
        shl eax,0x1
        adc dl,dl
        mov eax,dword ptr [ebp - 0x3c]
        sub eax,dword ptr [ebp - 0xc]
        shl eax,0x1
        adc dl,dl
        mov eax,dword ptr [ebp - 0x10]
        sub eax,dword ptr [ebp - 0x38]
        shl eax,0x1
        adc dl,dl
        mov eax,dword ptr [ebp - 0x40]
        sub eax,dword ptr [ebp - 0x10]
        shl eax,0x1
        adc dl,dl
        mov eax,dword ptr [ebp - 0x14]
        sub eax,dword ptr [ebp - 0x34]
        shl eax,0x1
        adc dh,dh
        mov eax,dword ptr [ebp - 0x3c]
        sub eax,dword ptr [ebp - 0x14]
        shl eax,0x1
        adc dh,dh
        mov eax,dword ptr [ebp - 0x18]
        sub eax,dword ptr [ebp - 0x38]
        shl eax,0x1
        adc dh,dh
        mov eax,dword ptr [ebp - 0x40]
        sub eax,dword ptr [ebp - 0x18]
        shl eax,0x1
        adc dh,dh
        mov dword ptr [ebp - 0x1c],edx
        test dh,dl
        jnz color_43b320
        or dl,dh
        jnz color_43afde
        mov esi,dword ptr [ebp + 0x8]
        mov eax,dword ptr [esi + 0x4]
        sub dword ptr [ebp + 0x14],eax
        mov eax,dword ptr [esi + 0x8]
        sub dword ptr [ebp + 0x18],eax
        push dword ptr [ebp - 0x48]
        push dword ptr [ebp + 0x18]
        push dword ptr [ebp + 0x14]
        push dword ptr [ebp - 0x30]
        push dword ptr [ebp + 0x8]
        call DrawRLEImageColorUnclipped
        add esp,0x14
        jmp color_43b318
color_43afde:
        mov eax,dword ptr [ebp - 0x10]
        imul dword ptr [ebp - 0x48]
        add eax,dword ptr [ebp - 0x44]
        add eax,dword ptr [ebp - 0xc]
        mov edi,eax
        mov ecx,dword ptr [ebp - 0x10]
        mov dword ptr [ebp - 0x20],ecx
        jmp color_43b00a
color_43aff4:
        movzx eax,al
        add esi,eax
        dec esi
color_43affa:
        inc esi
color_43affb:
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja color_43affa
        jnz color_43aff4
        jc color_43affa
        add edi,dword ptr [ebp - 0x48]
        inc ecx
color_43b00a:
        cmp ecx,dword ptr [ebp - 0x38]
        jl color_43affb
        mov dword ptr [ebp - 0x24],edi
        mov dword ptr [ebp - 0x20],ecx
        mov eax,edi
        sub eax,dword ptr [ebp - 0xc]
        add eax,dword ptr [ebp - 0x34]
        mov dword ptr [ebp - 0x28],eax
        mov eax,edi
        sub eax,dword ptr [ebp - 0xc]
        add eax,dword ptr [ebp - 0x3c]
        mov dword ptr [ebp - 0x2c],eax
        jmp color_43b30c
color_43b030:
        mov eax,dword ptr [ebp - 0x20]
        cmp eax,dword ptr [ebp - 0x40]
        jg color_43b318
        mov edi,dword ptr [ebp - 0x24]
        test dword ptr [ebp - 0x1c],0x8
        jnz color_43b125
        test dword ptr [ebp - 0x1c],0x400
        jnz color_43b1b3
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja color_43b081
        jnz color_43b0b2
        jnc color_43b120
color_43b068:
        mov al,byte ptr [esi]
        inc esi
        movzx ecx,al
        add edi,ecx
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja color_43b081
        jnz color_43b0b2
        jc color_43b068
        jnc color_43b120
color_43b081:
        movzx ecx,al
color_43b084:
        xor eax,eax
        mov al,byte ptr [esi]
        inc esi
        mov al,byte ptr g_abRasterPaletteTranslation_0046ff2c[eax]
        push ecx
        and ecx,0x3
        rep stosb
        mov ah,al
        rol eax,0x8
        mov al,ah
        rol eax,0x8
        mov al,ah
        pop ecx
        shr ecx,0x2
        rep stosd
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja color_43b081
        jnc color_43b120
        jz color_43b068
color_43b0b2:
        movzx ecx,al
color_43b0b5:
        xor eax,eax
        or ecx,ecx
        jz color_43b10d
        cmp ecx,0x4
        jl color_43b0fe
color_43b0c0:
        mov al,byte ptr [esi]
        mov al,byte ptr g_abRasterPaletteTranslation_0046ff2c[eax]
        mov byte ptr [edi],al
        mov al,byte ptr [esi + 0x1]
        mov al,byte ptr g_abRasterPaletteTranslation_0046ff2c[eax]
        mov byte ptr [edi + 0x1],al
        mov al,byte ptr [esi + 0x2]
        mov al,byte ptr g_abRasterPaletteTranslation_0046ff2c[eax]
        mov byte ptr [edi + 0x2],al
        mov al,byte ptr [esi + 0x3]
        mov al,byte ptr g_abRasterPaletteTranslation_0046ff2c[eax]
        mov byte ptr [edi + 0x3],al
        add esi,0x4
        add edi,0x4
        sub ecx,0x4
        jz color_43b10d
        cmp ecx,0x4
        jge color_43b0c0
color_43b0fe:
        mov al,byte ptr [esi]
        mov al,byte ptr g_abRasterPaletteTranslation_0046ff2c[eax]
        mov byte ptr [edi],al
        inc esi
        inc edi
        dec ecx
        jnz color_43b0fe
color_43b10d:
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja color_43b081
        jnz color_43b0b2
        jc color_43b068
color_43b120:
        jmp color_43b2fd
color_43b125:
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja color_43b145
        jnz color_43b176
        jnc color_43b1ae
color_43b130:
        mov al,byte ptr [esi]
        inc esi
        movzx ecx,al
        add edi,ecx
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja color_43b145
        jnz color_43b176
        jc color_43b130
        jnc color_43b1ae
color_43b145:
        movzx ecx,al
        mov eax,dword ptr [ebp - 0x28]
        sub eax,edi
        cmp eax,ecx
        jge color_43b168
        or eax,eax
        js color_43b159
        add edi,eax
        sub ecx,eax
color_43b159:
        test dword ptr [ebp - 0x1c],0x400
        jz color_43b084
        jnz color_43b1de
color_43b168:
        add edi,ecx
        inc esi
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja color_43b145
        jnc color_43b1ae
        jz color_43b130
color_43b176:
        movzx ecx,al
        mov eax,dword ptr [ebp - 0x28]
        sub eax,edi
        cmp eax,ecx
        jge color_43b19f
        or eax,eax
        js color_43b18c
        add edi,eax
        sub ecx,eax
        add esi,eax
color_43b18c:
        test dword ptr [ebp - 0x1c],0x400
        jz color_43b0b5
        jnz color_43b22d
color_43b19f:
        add edi,ecx
        add esi,ecx
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja color_43b145
        jnz color_43b176
        jc color_43b130
color_43b1ae:
        jmp color_43b2fd
color_43b1b3:
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja color_43b1db
        jnz color_43b22a
        jnc color_43b2b8
color_43b1c2:
        mov al,byte ptr [esi]
        inc esi
        movzx ecx,al
        add edi,ecx
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja color_43b1db
        jnz color_43b22a
        jc color_43b1c2
        jnc color_43b2b8
color_43b1db:
        movzx ecx,al
color_43b1de:
        cmp edi,dword ptr [ebp - 0x2c]
        jg color_43b2dd
        mov eax,edi
        add eax,ecx
        dec eax
        sub eax,dword ptr [ebp - 0x2c]
        cdq
        not edx
        and edx,eax
        sub ecx,edx
        xor eax,eax
        mov al,byte ptr [esi]
        inc esi
        mov al,byte ptr g_abRasterPaletteTranslation_0046ff2c[eax]
        push ecx
        and ecx,0x3
        rep stosb
        mov ah,al
        rol eax,0x8
        mov al,ah
        rol eax,0x8
        mov al,ah
        pop ecx
        shr ecx,0x2
        rep stosd
        add edi,edx
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja color_43b1db
        jnc color_43b2b8
        jz color_43b1c2
color_43b22a:
        movzx ecx,al
color_43b22d:
        cmp edi,dword ptr [ebp - 0x2c]
        jg color_43b2ee
        mov eax,edi
        add eax,ecx
        dec eax
        sub eax,dword ptr [ebp - 0x2c]
        cdq
        not edx
        and edx,eax
        sub ecx,edx
        xor eax,eax
        or ecx,ecx
        jz color_43b29d
        cmp ecx,0x4
        jl color_43b28e
color_43b250:
        mov al,byte ptr [esi]
        mov al,byte ptr g_abRasterPaletteTranslation_0046ff2c[eax]
        mov byte ptr [edi],al
        mov al,byte ptr [esi + 0x1]
        mov al,byte ptr g_abRasterPaletteTranslation_0046ff2c[eax]
        mov byte ptr [edi + 0x1],al
        mov al,byte ptr [esi + 0x2]
        mov al,byte ptr g_abRasterPaletteTranslation_0046ff2c[eax]
        mov byte ptr [edi + 0x2],al
        mov al,byte ptr [esi + 0x3]
        mov al,byte ptr g_abRasterPaletteTranslation_0046ff2c[eax]
        mov byte ptr [edi + 0x3],al
        add esi,0x4
        add edi,0x4
        sub ecx,0x4
        jz color_43b29d
        cmp ecx,0x4
        jge color_43b250
color_43b28e:
        mov al,byte ptr [esi]
        mov al,byte ptr g_abRasterPaletteTranslation_0046ff2c[eax]
        mov byte ptr [edi],al
        inc esi
        inc edi
        dec ecx
        jnz color_43b28e
color_43b29d:
        add edi,edx
        add esi,edx
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja color_43b1db
        jnz color_43b22a
        jc color_43b1c2
color_43b2b8:
        jmp color_43b2fd
color_43b2c5:
        mov al,byte ptr [esi]
        inc esi
        movzx ecx,al
        add edi,ecx
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja color_43b2da
        jnz color_43b2eb
        jc color_43b2c5
        jnc color_43b2fd
color_43b2da:
        movzx ecx,al
color_43b2dd:
        add edi,ecx
        inc esi
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja color_43b2da
        jnc color_43b2fd
        jz color_43b2c5
color_43b2eb:
        movzx ecx,al
color_43b2ee:
        add edi,ecx
        add esi,ecx
        mov al,byte ptr [esi]
        inc esi
        shr al,0x1
        ja color_43b2da
        jnz color_43b2eb
        jc color_43b2c5
color_43b2fd:
        mov eax,dword ptr [ebp - 0x48]
        add dword ptr [ebp - 0x24],eax
        add dword ptr [ebp - 0x28],eax
        add dword ptr [ebp - 0x2c],eax
        inc dword ptr [ebp - 0x20]
color_43b30c:
        mov eax,dword ptr [ebp - 0x20]
        cmp eax,dword ptr [ebp - 0x18]
        jle color_43b030
color_43b318:
        xor eax,eax
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
color_43b320:
        mov eax,0xfffffffd
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
color_43b32b:
        mov eax,0xfffffffc
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
    }
}
#pragma optimize("", on)

/* Function start: 0x43B336 */
/* Palette-translated counterpart to the original hand-written loop above. */
#pragma optimize("", off)
__declspec(naked) int DrawRLEImageColorUnclipped(
    RasterClip *clip, RLEFrameHeader *frameHeader, int x, int y,
    int strideScratch)
{
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp + 8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [esi + 4]
        add dword ptr [ebp + 0x10], eax
        mov eax, dword ptr [esi + 8]
        add dword ptr [ebp + 0x14], eax
        mov eax, dword ptr [ebx + 4]
        inc eax
        mov dword ptr [ebp + 0x18], eax
        jle color_done
        mov esi, dword ptr [ebp + 0xc]
        mov edi, dword ptr [ebx]
        mov eax, dword ptr [esi + 8]
        add eax, dword ptr [ebp + 0x10]
        add edi, eax
        mov eax, dword ptr [esi + 0xc]
        mov ebx, eax
        add eax, dword ptr [ebp + 0x14]
        mul dword ptr [ebp + 0x18]
        add edi, eax
        mov edx, edi
        mov eax, dword ptr [esi + 0x10]
        mov eax, dword ptr [esi + 0x14]
        inc eax
        sub eax, ebx
        mov ebx, eax
        jle color_done
        add esi, 0x18
color_next_row:
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja color_solid
        jne color_literal
        jae color_row_done
color_skip:
        mov al, byte ptr [esi]
        inc esi
        movzx ecx, al
        add edi, ecx
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja color_solid
        jne color_literal
        jb color_skip
        jae color_row_done
color_solid:
        movzx ecx, al
        xor eax, eax
        mov al, byte ptr [esi]
        inc esi
        mov al, byte ptr g_abRasterPaletteTranslation_0046ff2c[eax]
        push ecx
        and ecx, 3
        rep stosb
        mov ah, al
        rol eax, 8
        mov al, ah
        rol eax, 8
        mov al, ah
        pop ecx
        shr ecx, 2
        rep stosd
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja color_solid
        jae color_row_done
        je color_skip
color_literal:
        movzx ecx, al
        xor eax, eax
        or ecx, ecx
        je color_literal_done
        cmp ecx, 4
        jl color_literal_tail
color_literal_four:
        mov al, byte ptr [esi]
        mov al, byte ptr g_abRasterPaletteTranslation_0046ff2c[eax]
        mov byte ptr [edi], al
        mov al, byte ptr [esi + 1]
        mov al, byte ptr g_abRasterPaletteTranslation_0046ff2c[eax]
        mov byte ptr [edi + 1], al
        mov al, byte ptr [esi + 2]
        mov al, byte ptr g_abRasterPaletteTranslation_0046ff2c[eax]
        mov byte ptr [edi + 2], al
        mov al, byte ptr [esi + 3]
        mov al, byte ptr g_abRasterPaletteTranslation_0046ff2c[eax]
        mov byte ptr [edi + 3], al
        add esi, 4
        add edi, 4
        sub ecx, 4
        je color_literal_done
        cmp ecx, 4
        jge color_literal_four
color_literal_tail:
        mov al, byte ptr [esi]
        mov al, byte ptr g_abRasterPaletteTranslation_0046ff2c[eax]
        mov byte ptr [edi], al
        inc esi
        inc edi
        dec ecx
        jne color_literal_tail
color_literal_done:
        mov al, byte ptr [esi]
        inc esi
        shr al, 1
        ja color_solid
        jne color_literal
        jb color_skip
color_row_done:
        add edx, dword ptr [ebp + 0x18]
        mov edi, edx
        dec ebx
        jne color_next_row
color_done:
        xor eax, eax
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
    }
}
#pragma optimize("", on)

/* Function start: 0x43B469 */
/* The retail routine is a hand-written fixed-point scanline texture mapper.
 * It transforms a four-corner workspace, clips both polygon edges, and walks
 * the decoded RLE image with direction-specific source increments. */
#pragma optimize("", off)
__declspec(naked) int RotateRLEImage(
    RasterClip *clip, unsigned char *shape, int frame, int x, int y,
    unsigned char *scratch, unsigned int angleTenths, int scaleX, int scaleY,
    unsigned int flags)
{
#ifdef _MSC_VER
#include "screens_rotate_rle_image.inc"
#else
    return 0;
#endif
}
#pragma optimize("", on)

/* Function start: 0x43C015 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* The command-stream walk retains the raster library's explicit ES setup. */
#pragma optimize("", off)
__declspec(naked) unsigned int GetRLEFrameBounds(
    unsigned char *shape, int frame, int x, int y, unsigned int flags,
    int *bounds)
{
#ifdef _MSC_VER
#include "screens_get_rle_frame_bounds.inc"
#else
    return 0;
#endif
}
#pragma optimize("", on)

/* Function start: 0x43C18D */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* The paired forward/backward scans and explicit segment state belong to the
 * hand-written RLE encoder. */
__declspec(naked) int EncodeRasterClipToRLEFrame(
    RasterClip *clip, unsigned char transparentColour,
    int originX, int originY, unsigned char *output)
{
#ifdef _MSC_VER
#include "screens_encode_raster_clip_to_rle_frame.inc"
#else
    return 0;
#endif
}

/* Function start: 0x43C410 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* Preserve the hand-written RLE command-stream traversal. */
#pragma optimize("", off)
__declspec(naked) unsigned int TranslateRLEFramePalette(
    unsigned char *shape, int frame)
{
#ifdef _MSC_VER
#include "screens_translate_rle_frame_palette.inc"
#else
    return 0;
#endif
}
#pragma optimize("", on)

/* Function start: 0x43C4A2 */
/* The register-paired run classifier and explicit ES setup identify the
 * original scanline encoder as hand-written raster assembly. */
__declspec(naked) void EncodeRLEScanline(
    int pixelCount, unsigned char transparentColour, int sourceX)
{
    __asm {
        push ebp
        mov ebp, esp
        add esp, -4
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr g_pRLEScanlineStart_0046e6e5
        mov dword ptr g_pRLEScanCursor_0046e6e9, esi
        push dword ptr [ebp + 10h]
        push 0
        push 0
        call EmitRLEScanlineRun
        add esp, 0ch
        mov dword ptr [ebp - 4], 5
        mov ecx, dword ptr [ebp + 8]
        or ecx, ecx
        jz encode_finish_line
        mov al, byte ptr [esi]
        inc esi
        dec ecx
        mov ah, al
encode_classify:
        cmp ah, byte ptr [ebp + 0ch]
        jz encode_transparent_run
        mov dword ptr [ebp - 4], 1
        or ecx, ecx
        jz encode_finish_line
        mov al, byte ptr [esi]
        inc esi
        dec ecx
        xor al, ah
        xor ah, al
        or al, al
        jz encode_solid_run
        cmp ah, byte ptr [ebp + 0ch]
        jnz encode_literal_scan
        mov dword ptr g_pRLEScanCursor_0046e6e9, esi
        push dword ptr [ebp + 10h]
        push 1
        push 1
        call EmitRLEScanlineRun
        add esp, 0ch
        jmp encode_transparent_run
encode_literal_scan:
        or ecx, ecx
        jz encode_finish_line
        mov al, byte ptr [esi]
        inc esi
        dec ecx
        xor al, ah
        xor ah, al
        cmp ah, byte ptr [ebp + 0ch]
        jnz encode_literal_pair
        mov dword ptr g_pRLEScanCursor_0046e6e9, esi
        push dword ptr [ebp + 10h]
        push 1
        push 1
        call EmitRLEScanlineRun
        add esp, 0ch
        jmp encode_transparent_run
encode_literal_pair:
        or al, al
        jnz encode_literal_scan
        or ecx, ecx
        jz encode_finish_line
        mov al, byte ptr [esi]
        inc esi
        dec ecx
        xor al, ah
        xor ah, al
        cmp ah, byte ptr [ebp + 0ch]
        jnz encode_literal_triplet
        mov dword ptr g_pRLEScanCursor_0046e6e9, esi
        push dword ptr [ebp + 10h]
        push 1
        push 1
        call EmitRLEScanlineRun
        add esp, 0ch
        jmp encode_transparent_run
encode_literal_triplet:
        or al, al
        jnz encode_literal_scan
        mov dword ptr g_pRLEScanCursor_0046e6e9, esi
        push dword ptr [ebp + 10h]
        push 3
        push 1
        call EmitRLEScanlineRun
        add esp, 0ch
        mov dword ptr [ebp - 4], 2
encode_solid_run:
        or ecx, ecx
        jz encode_finish_line
        mov al, byte ptr [esi]
        inc esi
        dec ecx
        xor al, ah
        jz encode_solid_run
        xor ah, al
        mov dword ptr g_pRLEScanCursor_0046e6e9, esi
        push dword ptr [ebp + 10h]
        push 1
        push 2
        call EmitRLEScanlineRun
        add esp, 0ch
        cmp ah, byte ptr [ebp + 0ch]
        jnz encode_classify
        mov dword ptr [ebp - 4], 3
encode_transparent_run:
        or ecx, ecx
        jz encode_finish_line
        mov al, byte ptr [esi]
        inc esi
        dec ecx
        xor al, ah
        jz encode_transparent_run
        xor ah, al
        mov dword ptr g_pRLEScanCursor_0046e6e9, esi
        push dword ptr [ebp + 10h]
        push 1
        push 3
        call EmitRLEScanlineRun
        add esp, 0ch
        jmp encode_classify
encode_finish_line:
        mov dword ptr g_pRLEScanCursor_0046e6e9, esi
        push dword ptr [ebp + 10h]
        push 0
        push dword ptr [ebp - 4]
        call EmitRLEScanlineRun
        add esp, 0ch
        push dword ptr [ebp + 10h]
        push 0
        push 4
        call EmitRLEScanlineRun
        add esp, 0ch
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
    }
}

/* Function start: 0x43C62B */
/* Companion command emitter for the hand-written RLE scanline encoder. */
__declspec(naked) void EmitRLEScanlineRun(
    int runType, int trailingCount, int sourceX)
{
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        push eax
        push ecx
        mov esi, dword ptr g_pRLELiteralStart_0046e6f1
        mov edi, dword ptr g_pRLEOutputCursor_0046e6ed
        mov eax, dword ptr [ebp + 8]
        cmp eax, 2
        jz emit_solid
        cmp eax, 1
        jz emit_literal
        cmp eax, 3
        jz emit_defer_skip
        cmp eax, 4
        jz emit_end_line
        cmp eax, 0
        jnz emit_done
        xor eax, eax
        mov dword ptr g_nRLEPendingSkip_0046e6e1, eax
        mov esi, dword ptr g_pRLEScanCursor_0046e6e9
        mov dword ptr g_pRLELiteralStart_0046e6f1, esi
        jmp emit_done
emit_solid:
        mov ebx, dword ptr g_nRLEPendingSkip_0046e6e1
        or ebx, ebx
        jz emit_solid_bounds
emit_solid_skip_chunks:
        mov ecx, ebx
        cmp ecx, 0ffh
        jl emit_solid_skip_ready
        mov ecx, 0ffh
emit_solid_skip_ready:
        sub ebx, ecx
        cmp dword ptr g_pRLEEncodeBuffer_0046e6dd, 0
        jz emit_solid_skip_advance
        mov al, 1
        mov byte ptr [edi], al
        inc edi
        mov al, cl
        mov byte ptr [edi], al
        inc edi
        jmp emit_solid_skip_input
emit_solid_skip_advance:
        add edi, 2
emit_solid_skip_input:
        add esi, ecx
        or ebx, ebx
        jnz emit_solid_skip_chunks
        mov dword ptr g_nRLEPendingSkip_0046e6e1, ebx
emit_solid_bounds:
        mov ebx, dword ptr g_pRLEScanCursor_0046e6e9
        sub ebx, esi
        sub ebx, dword ptr [ebp + 0ch]
        mov eax, dword ptr [ebp + 10h]
        add eax, esi
        sub eax, dword ptr g_pRLEScanlineStart_0046e6e5
        cmp eax, dword ptr g_nRLEEncodedMinimumX_0046e705
        jge emit_solid_maximum
        mov dword ptr g_nRLEEncodedMinimumX_0046e705, eax
emit_solid_maximum:
        add eax, ebx
        dec eax
        cmp eax, dword ptr g_nRLEEncodedMaximumX_0046e70d
        jle emit_solid_chunks
        mov dword ptr g_nRLEEncodedMaximumX_0046e70d, eax
        jmp emit_solid_chunks
emit_solid_next_chunk:
        mov ecx, ebx
        cmp ecx, 7fh
        jl emit_solid_chunk_ready
        mov ecx, 7fh
emit_solid_chunk_ready:
        cmp dword ptr g_pRLEEncodeBuffer_0046e6dd, 0
        jz emit_solid_chunk_advance
        mov al, cl
        add al, al
        mov byte ptr [edi], al
        inc edi
        mov al, byte ptr [esi]
        mov byte ptr [edi], al
        inc edi
        jmp emit_solid_chunk_input
emit_solid_chunk_advance:
        add edi, 2
emit_solid_chunk_input:
        add esi, ecx
        sub ebx, ecx
emit_solid_chunks:
        or ebx, ebx
        jnz emit_solid_next_chunk
        jmp emit_done
emit_literal:
        mov ebx, dword ptr g_nRLEPendingSkip_0046e6e1
        or ebx, ebx
        jz emit_literal_bounds
emit_literal_skip_chunks:
        mov ecx, ebx
        cmp ecx, 0ffh
        jl emit_literal_skip_ready
        mov ecx, 0ffh
emit_literal_skip_ready:
        sub ebx, ecx
        cmp dword ptr g_pRLEEncodeBuffer_0046e6dd, 0
        jz emit_literal_skip_advance
        mov al, 1
        mov byte ptr [edi], al
        inc edi
        mov al, cl
        mov byte ptr [edi], al
        inc edi
        jmp emit_literal_skip_input
emit_literal_skip_advance:
        add edi, 2
emit_literal_skip_input:
        add esi, ecx
        or ebx, ebx
        jnz emit_literal_skip_chunks
        mov dword ptr g_nRLEPendingSkip_0046e6e1, ebx
emit_literal_bounds:
        mov ebx, dword ptr g_pRLEScanCursor_0046e6e9
        sub ebx, esi
        sub ebx, dword ptr [ebp + 0ch]
        mov eax, dword ptr [ebp + 10h]
        add eax, esi
        sub eax, dword ptr g_pRLEScanlineStart_0046e6e5
        cmp eax, dword ptr g_nRLEEncodedMinimumX_0046e705
        jge emit_literal_maximum
        mov dword ptr g_nRLEEncodedMinimumX_0046e705, eax
emit_literal_maximum:
        add eax, ebx
        dec eax
        cmp eax, dword ptr g_nRLEEncodedMaximumX_0046e70d
        jle emit_literal_chunks
        mov dword ptr g_nRLEEncodedMaximumX_0046e70d, eax
        jmp emit_literal_chunks
emit_literal_next_chunk:
        mov ecx, ebx
        cmp ecx, 7fh
        jl emit_literal_chunk_ready
        mov ecx, 7fh
emit_literal_chunk_ready:
        mov edx, ecx
        mov al, cl
        add al, al
        inc al
        cmp dword ptr g_pRLEEncodeBuffer_0046e6dd, 0
        jz emit_literal_chunk_advance
        mov byte ptr [edi], al
        inc edi
        rep movsb
        jmp emit_literal_chunk_input
emit_literal_chunk_advance:
        inc edi
        add esi, ecx
        add edi, ecx
emit_literal_chunk_input:
        sub ebx, edx
emit_literal_chunks:
        or ebx, ebx
        jnz emit_literal_next_chunk
        jmp emit_done
emit_defer_skip:
        mov ebx, dword ptr g_pRLEScanCursor_0046e6e9
        sub ebx, esi
        sub ebx, dword ptr [ebp + 0ch]
        mov dword ptr g_nRLEPendingSkip_0046e6e1, ebx
        jmp emit_done
emit_end_line:
        xor eax, eax
        cmp dword ptr g_pRLEEncodeBuffer_0046e6dd, 0
        jz emit_end_advance
        mov byte ptr [edi], al
        inc edi
        jmp emit_done
emit_end_advance:
        inc edi
emit_done:
        mov dword ptr g_pRLEOutputCursor_0046e6ed, edi
        mov dword ptr g_pRLELiteralStart_0046e6f1, esi
        pop ecx
        pop eax
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    }
}

/* Function start: 0x43C808 */
/* The explicit ES setup and REP stores identify the original as hand-written
 * raster assembly, so preserve that implementation rather than asking the C
 * compiler to synthesize a materially different fill loop. */
__declspec(naked) int FillRasterClip(RasterClip *clip, int colour)
{
    __asm {
        push ebp
        mov ebp, esp
        add esp, -20h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp + 8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx + 4]
        inc eax
        mov dword ptr [ebp - 18h], eax
        jle invalid_surface
        mov eax, dword ptr [ebx + 8]
        inc eax
        mov ecx, eax
        jle invalid_surface
        mov eax, dword ptr [esi + 4]
        mov dword ptr [ebp - 1ch], eax
        cmp eax, 0
        jg left_clipped
        mov eax, 0
left_clipped:
        mov dword ptr [ebp - 4], eax
        mov eax, dword ptr [esi + 8]
        mov dword ptr [ebp - 20h], eax
        cmp eax, 0
        jg top_clipped
        mov eax, 0
top_clipped:
        mov dword ptr [ebp - 8], eax
        mov eax, dword ptr [esi + 0ch]
        mov edx, dword ptr [ebp - 18h]
        dec edx
        cmp eax, edx
        jl right_clipped
        mov eax, edx
right_clipped:
        mov dword ptr [ebp - 0ch], eax
        mov eax, dword ptr [esi + 10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl bottom_clipped
        mov eax, edx
bottom_clipped:
        mov dword ptr [ebp - 10h], eax
        mov eax, dword ptr [ebp - 0ch]
        cmp eax, dword ptr [ebp - 4]
        jl empty_clip
        mov eax, dword ptr [ebp - 10h]
        cmp eax, dword ptr [ebp - 8]
        jl empty_clip
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp - 14h], eax
        jmp begin_fill
invalid_surface:
        mov eax, -1
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
empty_clip:
        mov eax, -2
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
begin_fill:
        mov eax, dword ptr [ebp - 8]
        imul dword ptr [ebp - 18h]
        add eax, dword ptr [ebp - 14h]
        add eax, dword ptr [ebp - 4]
        mov edi, eax
        mov ebx, dword ptr [ebp - 0ch]
        inc ebx
        sub ebx, dword ptr [ebp - 4]
        mov esi, dword ptr [ebp - 18h]
        sub esi, ebx
        mov al, byte ptr [ebp + 0ch]
        mov ah, al
        shl eax, 10h
        mov al, byte ptr [ebp + 0ch]
        mov ah, al
        mov edx, dword ptr [ebp - 8]
        jmp test_row
fill_row:
        mov ecx, ebx
        and ecx, 3
        rep stosb
        mov ecx, ebx
        shr ecx, 2
        rep stosd
        add edi, esi
        inc edx
test_row:
        cmp edx, dword ptr [ebp - 10h]
        jle fill_row
        xor eax, eax
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
    }
}

/* Function start: 0x43C8E7 */
/* This is the companion hand-written clipped blitter.  Direction-flag and
 * segment-register handling are part of its overlap-safe behavior. */
__declspec(naked) int BlitRasterClip(
    RasterClip *source, int sourceX, int sourceY,
    RasterClip *destination, int destinationX,
    int destinationY, unsigned int colour)
{
    __asm {
        push ebp
        mov ebp, esp
        add esp, -9ch
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp + 8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx + 4]
        inc eax
        mov dword ptr [ebp - 70h], eax
        jle blit_invalid_source
        mov eax, dword ptr [ebx + 8]
        inc eax
        mov ecx, eax
        jle blit_invalid_source
        mov eax, dword ptr [esi + 4]
        mov dword ptr [ebp - 78h], eax
        cmp eax, 0
        jg blit_source_left
        mov eax, 0
blit_source_left:
        mov dword ptr [ebp - 60h], eax
        mov eax, dword ptr [esi + 8]
        mov dword ptr [ebp - 7ch], eax
        cmp eax, 0
        jg blit_source_top
        mov eax, 0
blit_source_top:
        mov dword ptr [ebp - 64h], eax
        mov eax, dword ptr [esi + 0ch]
        mov edx, dword ptr [ebp - 70h]
        dec edx
        cmp eax, edx
        jl blit_source_right
        mov eax, edx
blit_source_right:
        mov dword ptr [ebp - 68h], eax
        mov eax, dword ptr [esi + 10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl blit_source_bottom
        mov eax, edx
blit_source_bottom:
        mov dword ptr [ebp - 6ch], eax
        mov eax, dword ptr [ebp - 68h]
        cmp eax, dword ptr [ebp - 60h]
        jl blit_empty_source
        mov eax, dword ptr [ebp - 6ch]
        cmp eax, dword ptr [ebp - 64h]
        jl blit_empty_source
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp - 74h], eax
        jmp blit_source_ready
blit_invalid_source:
        mov eax, -1
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
blit_empty_source:
        mov eax, -2
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
blit_source_ready:
        mov eax, dword ptr [ebp - 60h]
        mov dword ptr [ebp - 2ch], eax
        mov eax, dword ptr [ebp - 64h]
        mov dword ptr [ebp - 30h], eax
        mov eax, dword ptr [ebp - 68h]
        mov dword ptr [ebp - 34h], eax
        mov eax, dword ptr [ebp - 6ch]
        mov dword ptr [ebp - 38h], eax
        mov eax, dword ptr [ebp - 78h]
        sub dword ptr [ebp - 2ch], eax
        sub dword ptr [ebp - 34h], eax
        mov eax, dword ptr [ebp - 7ch]
        sub dword ptr [ebp - 30h], eax
        sub dword ptr [ebp - 38h], eax
        mov esi, dword ptr [ebp + 14h]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx + 4]
        inc eax
        mov dword ptr [ebp - 94h], eax
        jle blit_invalid_destination
        mov eax, dword ptr [ebx + 8]
        inc eax
        mov ecx, eax
        jle blit_invalid_destination
        mov eax, dword ptr [esi + 4]
        mov dword ptr [ebp - 98h], eax
        cmp eax, 0
        jg blit_destination_left
        mov eax, 0
blit_destination_left:
        mov dword ptr [ebp - 80h], eax
        mov eax, dword ptr [esi + 8]
        mov dword ptr [ebp - 9ch], eax
        cmp eax, 0
        jg blit_destination_top
        mov eax, 0
blit_destination_top:
        mov dword ptr [ebp - 84h], eax
        mov eax, dword ptr [esi + 0ch]
        mov edx, dword ptr [ebp - 94h]
        dec edx
        cmp eax, edx
        jl blit_destination_right
        mov eax, edx
blit_destination_right:
        mov dword ptr [ebp - 88h], eax
        mov eax, dword ptr [esi + 10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl blit_destination_bottom
        mov eax, edx
blit_destination_bottom:
        mov dword ptr [ebp - 8ch], eax
        mov eax, dword ptr [ebp - 88h]
        cmp eax, dword ptr [ebp - 80h]
        jl blit_empty_destination
        mov eax, dword ptr [ebp - 8ch]
        cmp eax, dword ptr [ebp - 84h]
        jl blit_empty_destination
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp - 90h], eax
        jmp blit_destination_ready
blit_invalid_destination:
        mov eax, -1
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
blit_empty_destination:
        mov eax, -2
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
blit_destination_ready:
        mov eax, dword ptr [ebp - 80h]
        mov dword ptr [ebp - 40h], eax
        mov eax, dword ptr [ebp - 84h]
        mov dword ptr [ebp - 44h], eax
        mov eax, dword ptr [ebp - 88h]
        mov dword ptr [ebp - 48h], eax
        mov eax, dword ptr [ebp - 8ch]
        mov dword ptr [ebp - 4ch], eax
        mov eax, dword ptr [ebp - 98h]
        sub dword ptr [ebp - 40h], eax
        sub dword ptr [ebp - 48h], eax
        mov eax, dword ptr [ebp - 9ch]
        sub dword ptr [ebp - 44h], eax
        sub dword ptr [ebp - 4ch], eax
        mov eax, dword ptr [ebp + 0ch]
        sub eax, dword ptr [ebp + 18h]
        mov dword ptr [ebp - 24h], eax
        mov eax, dword ptr [ebp + 10h]
        sub eax, dword ptr [ebp + 1ch]
        mov dword ptr [ebp - 28h], eax
        mov eax, dword ptr [ebp - 2ch]
        mov edx, dword ptr [ebp - 40h]
        add edx, dword ptr [ebp - 24h]
        cmp eax, edx
        jg blit_copy_left
        mov eax, edx
blit_copy_left:
        mov dword ptr [ebp - 4], eax
        mov eax, dword ptr [ebp - 30h]
        mov edx, dword ptr [ebp - 44h]
        add edx, dword ptr [ebp - 28h]
        cmp eax, edx
        jg blit_copy_top
        mov eax, edx
blit_copy_top:
        mov dword ptr [ebp - 8], eax
        mov eax, dword ptr [ebp - 34h]
        mov edx, dword ptr [ebp - 48h]
        add edx, dword ptr [ebp - 24h]
        cmp eax, edx
        jl blit_copy_right
        mov eax, edx
blit_copy_right:
        mov dword ptr [ebp - 0ch], eax
        mov eax, dword ptr [ebp - 38h]
        mov edx, dword ptr [ebp - 4ch]
        add edx, dword ptr [ebp - 28h]
        cmp eax, edx
        jl blit_copy_bottom
        mov eax, edx
blit_copy_bottom:
        mov dword ptr [ebp - 10h], eax
        mov eax, dword ptr [ebp - 0ch]
        cmp eax, dword ptr [ebp - 4]
        jl blit_no_overlap
        mov eax, dword ptr [ebp - 10h]
        cmp eax, dword ptr [ebp - 8]
        jl blit_no_overlap
        mov eax, dword ptr [ebp - 40h]
        mov edx, dword ptr [ebp - 2ch]
        sub edx, dword ptr [ebp - 24h]
        cmp eax, edx
        jg blit_destination_copy_left
        mov eax, edx
blit_destination_copy_left:
        mov dword ptr [ebp - 14h], eax
        mov eax, dword ptr [ebp - 44h]
        mov edx, dword ptr [ebp - 30h]
        sub edx, dword ptr [ebp - 28h]
        cmp eax, edx
        jg blit_destination_copy_top
        mov eax, edx
blit_destination_copy_top:
        mov dword ptr [ebp - 18h], eax
        mov eax, dword ptr [ebp - 48h]
        mov edx, dword ptr [ebp - 34h]
        sub edx, dword ptr [ebp - 24h]
        cmp eax, edx
        jl blit_destination_copy_right
        mov eax, edx
blit_destination_copy_right:
        mov dword ptr [ebp - 1ch], eax
        mov eax, dword ptr [ebp - 4ch]
        mov edx, dword ptr [ebp - 38h]
        sub edx, dword ptr [ebp - 28h]
        cmp eax, edx
        jl blit_destination_copy_bottom
        mov eax, edx
blit_destination_copy_bottom:
        mov dword ptr [ebp - 20h], eax
        mov eax, dword ptr [ebp - 0ch]
        inc eax
        sub eax, dword ptr [ebp - 4]
        mov dword ptr [ebp - 58h], eax
        mov eax, dword ptr [ebp - 10h]
        inc eax
        sub eax, dword ptr [ebp - 8]
        mov dword ptr [ebp - 54h], eax
        mov eax, dword ptr [ebp - 7ch]
        imul dword ptr [ebp - 70h]
        add eax, dword ptr [ebp - 74h]
        add eax, dword ptr [ebp - 78h]
        mov esi, eax
        mov eax, dword ptr [ebp - 9ch]
        imul dword ptr [ebp - 94h]
        add eax, dword ptr [ebp - 90h]
        add eax, dword ptr [ebp - 98h]
        mov edi, eax
        mov eax, dword ptr [ebp - 8]
        mov ebx, dword ptr [ebp - 18h]
        cmp eax, ebx
        jle blit_bottom_up
        mul dword ptr [ebp - 70h]
        add esi, eax
        mov eax, ebx
        mul dword ptr [ebp - 94h]
        add edi, eax
        mov eax, dword ptr [ebp - 70h]
        mov dword ptr [ebp - 3ch], eax
        mov eax, dword ptr [ebp - 94h]
        mov dword ptr [ebp - 50h], eax
        jmp blit_vertical_ready
blit_bottom_up:
        mov eax, dword ptr [ebp - 10h]
        mul dword ptr [ebp - 70h]
        add esi, eax
        mov eax, dword ptr [ebp - 20h]
        mul dword ptr [ebp - 94h]
        add edi, eax
        mov eax, dword ptr [ebp - 70h]
        neg eax
        mov dword ptr [ebp - 3ch], eax
        mov eax, dword ptr [ebp - 94h]
        neg eax
        mov dword ptr [ebp - 50h], eax
blit_vertical_ready:
        mov ecx, dword ptr [ebp - 58h]
        mov eax, dword ptr [ebp - 4]
        mov ebx, dword ptr [ebp - 14h]
        cmp eax, ebx
        jle blit_right_to_left
        add esi, eax
        add edi, ebx
        sub dword ptr [ebp - 3ch], ecx
        sub dword ptr [ebp - 50h], ecx
        cld
        mov dword ptr [ebp - 5ch], 0
        jmp blit_direction_ready
blit_right_to_left:
        add esi, dword ptr [ebp - 0ch]
        add edi, dword ptr [ebp - 1ch]
        add dword ptr [ebp - 3ch], ecx
        add dword ptr [ebp - 50h], ecx
        std
        mov dword ptr [ebp - 5ch], 3
blit_direction_ready:
        mov eax, dword ptr [ebp + 20h]
        test eax, 0ffffff00h
        jz blit_fill
        mov edx, dword ptr [ebp - 54h]
        mov eax, dword ptr [ebp - 3ch]
        mov ebx, dword ptr [ebp - 50h]
blit_copy_row:
        mov ecx, dword ptr [ebp - 58h]
        and ecx, 3
        rep movsb
        mov ecx, dword ptr [ebp - 58h]
        shr ecx, 2
        sub esi, dword ptr [ebp - 5ch]
        sub edi, dword ptr [ebp - 5ch]
        rep movsd
        add esi, dword ptr [ebp - 5ch]
        add edi, dword ptr [ebp - 5ch]
        add esi, eax
        add edi, ebx
        dec edx
        jnz blit_copy_row
        cld
        jmp blit_success
blit_fill:
        mov dl, al
        mov ah, al
        shl eax, 10h
        mov al, dl
        mov ah, al
        mov edx, dword ptr [ebp - 54h]
        mov ebx, dword ptr [ebp - 50h]
blit_fill_row:
        mov ecx, dword ptr [ebp - 58h]
        and ecx, 3
        rep stosb
        mov ecx, dword ptr [ebp - 58h]
        shr ecx, 2
        sub edi, dword ptr [ebp - 5ch]
        rep stosd
        add edi, dword ptr [ebp - 5ch]
        add edi, ebx
        dec edx
        jnz blit_fill_row
        cld
blit_success:
        xor eax, eax
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
blit_no_overlap:
        mov eax, -3
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
    }
}

/* Function start: 0x43CC83 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* Preserve the raster library's explicit segment setup and wrapped nine-blit
 * decomposition. */
__declspec(naked) int ScrollRasterClipWrapped(
    RasterClip *clip, int deltaX, int deltaY, int mode,
    unsigned int fillOrScratch)
{
#ifdef _MSC_VER
#include "screens_scroll_raster_clip_wrapped.inc"
#else
    return 0;
#endif
}

/* Function start: 0x43CE80 */
/* The segment setup, explicit clip arithmetic, and symmetric pixel stores
 * identify this as the retail hand-written midpoint ellipse rasterizer. */
#pragma optimize("", off)
__declspec(naked) unsigned int DrawRasterEllipse(
    RasterClip *clip, int x, int y, int horizontalRadius,
    int verticalRadius, int colour)
{
#ifdef _MSC_VER
#include "screens_draw_raster_ellipse.inc"
#else
    return 0;
#endif
}
#pragma optimize("", on)

/* Function start: 0x43D1C1 */
/* Filled counterpart to the hand-written midpoint ellipse outline routine. */
__declspec(naked) unsigned int FillRasterEllipse(
    RasterClip *clip, int x, int y, int horizontalRadius,
    int verticalRadius, int colour)
{
    __asm {
        push ebp
        mov ebp, esp
        add esp, -54h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        cmp dword ptr [ebp + 14h], 0
        jz fill_ellipse_line
        cmp dword ptr [ebp + 18h], 0
        jnz fill_ellipse_surface
fill_ellipse_line:
        mov eax, dword ptr [ebp + 10h]
        add eax, dword ptr [ebp + 18h]
        mov ebx, dword ptr [ebp + 0ch]
        add ebx, dword ptr [ebp + 14h]
        mov ecx, dword ptr [ebp + 10h]
        sub ecx, dword ptr [ebp + 18h]
        mov edx, dword ptr [ebp + 0ch]
        sub edx, dword ptr [ebp + 14h]
        push dword ptr [ebp + 1ch]
        push 0
        push eax
        push ebx
        push ecx
        push edx
        push dword ptr [ebp + 8]
        call DrawClippedLine
        add esp, 1ch
        jmp fill_ellipse_done
fill_ellipse_surface:
        mov esi, dword ptr [ebp + 8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx + 4]
        inc eax
        mov dword ptr [ebp - 4ch], eax
        jle fill_ellipse_invalid_surface
        mov eax, dword ptr [ebx + 8]
        inc eax
        mov ecx, eax
        jle fill_ellipse_invalid_surface
        mov eax, dword ptr [esi + 4]
        mov dword ptr [ebp - 50h], eax
        cmp eax, 0
        jg fill_ellipse_left_ready
        mov eax, 0
fill_ellipse_left_ready:
        mov dword ptr [ebp - 38h], eax
        mov eax, dword ptr [esi + 8]
        mov dword ptr [ebp - 54h], eax
        cmp eax, 0
        jg fill_ellipse_top_ready
        mov eax, 0
fill_ellipse_top_ready:
        mov dword ptr [ebp - 3ch], eax
        mov eax, dword ptr [esi + 0ch]
        mov edx, dword ptr [ebp - 4ch]
        dec edx
        cmp eax, edx
        jl fill_ellipse_right_ready
        mov eax, edx
fill_ellipse_right_ready:
        mov dword ptr [ebp - 40h], eax
        mov eax, dword ptr [esi + 10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl fill_ellipse_bottom_ready
        mov eax, edx
fill_ellipse_bottom_ready:
        mov dword ptr [ebp - 44h], eax
        mov eax, dword ptr [ebp - 40h]
        cmp eax, dword ptr [ebp - 38h]
        jl fill_ellipse_empty_clip
        mov eax, dword ptr [ebp - 44h]
        cmp eax, dword ptr [ebp - 3ch]
        jl fill_ellipse_empty_clip
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp - 48h], eax
        jmp fill_ellipse_begin
fill_ellipse_invalid_surface:
        mov eax, -1
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
fill_ellipse_empty_clip:
        mov eax, -2
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
fill_ellipse_begin:
        mov eax, dword ptr [ebp + 1ch]
        mov ah, al
        mov dword ptr [ebp + 1ch], eax
        mov word ptr [ebp + 1eh], ax
        mov eax, dword ptr [ebp - 50h]
        add dword ptr [ebp + 0ch], eax
        mov eax, dword ptr [ebp - 54h]
        add dword ptr [ebp + 10h], eax
        mov eax, dword ptr [ebp + 0ch]
        mov dword ptr [ebp - 4], eax
        mov eax, dword ptr [ebp + 10h]
        mov dword ptr [ebp - 8], eax
        mov dword ptr [ebp - 0ch], 0
        mov eax, dword ptr [ebp + 18h]
        mov dword ptr [ebp - 10h], eax
        mul eax
        mov dword ptr [ebp - 1ch], eax
        shl eax, 1
        mov dword ptr [ebp - 20h], eax
        mov eax, dword ptr [ebp + 14h]
        mul eax
        mov dword ptr [ebp - 14h], eax
        shl eax, 1
        mov dword ptr [ebp - 18h], eax
        mov dword ptr [ebp - 24h], 0
        mov eax, dword ptr [ebp - 18h]
        mul dword ptr [ebp + 18h]
        mov dword ptr [ebp - 28h], eax
        mov eax, dword ptr [ebp - 14h]
        shr eax, 2
        add eax, dword ptr [ebp - 1ch]
        mov dword ptr [ebp - 2ch], eax
        mov eax, dword ptr [ebp - 14h]
        mul dword ptr [ebp + 18h]
        sub dword ptr [ebp - 2ch], eax
        mov ebx, dword ptr [ebp + 18h]
fill_ellipse_region_one_test:
        mov eax, dword ptr [ebp - 24h]
        sub eax, dword ptr [ebp - 28h]
        jns fill_ellipse_region_two_setup
        mov edi, dword ptr [ebp - 4]
        add edi, dword ptr [ebp - 0ch]
        cmp edi, dword ptr [ebp - 38h]
        jl fill_ellipse_region_one_step
        cmp edi, dword ptr [ebp - 40h]
        jl fill_ellipse_region_one_right_ready
        mov edi, dword ptr [ebp - 40h]
fill_ellipse_region_one_right_ready:
        mov dword ptr [ebp - 34h], edi
        mov edi, dword ptr [ebp - 4]
        sub edi, dword ptr [ebp - 0ch]
        cmp edi, dword ptr [ebp - 40h]
        jg fill_ellipse_region_one_step
        cmp edi, dword ptr [ebp - 38h]
        jg fill_ellipse_region_one_left_ready
        mov edi, dword ptr [ebp - 38h]
fill_ellipse_region_one_left_ready:
        mov dword ptr [ebp - 30h], edi
        mov edx, dword ptr [ebp - 8]
        add edx, dword ptr [ebp - 10h]
        cmp edx, dword ptr [ebp - 3ch]
        jl fill_ellipse_region_one_step
        cmp edx, dword ptr [ebp - 44h]
        jg fill_ellipse_region_one_lower
        mov eax, edx
        imul dword ptr [ebp - 4ch]
        add eax, dword ptr [ebp - 48h]
        add eax, edi
        mov edi, eax
        mov ecx, dword ptr [ebp - 34h]
        sub ecx, dword ptr [ebp - 30h]
        inc ecx
        mov eax, dword ptr [ebp + 1ch]
        mov edx, ecx
        and edx, 3
        shr ecx, 2
        rep stosd
        mov ecx, edx
        rep stosb
fill_ellipse_region_one_lower:
        mov edi, dword ptr [ebp - 30h]
        mov edx, dword ptr [ebp - 8]
        sub edx, dword ptr [ebp - 10h]
        cmp edx, dword ptr [ebp - 3ch]
        jl fill_ellipse_region_one_step
        cmp edx, dword ptr [ebp - 44h]
        jg fill_ellipse_region_one_step
        mov eax, edx
        imul dword ptr [ebp - 4ch]
        add eax, dword ptr [ebp - 48h]
        add eax, edi
        mov edi, eax
        mov ecx, dword ptr [ebp - 34h]
        sub ecx, dword ptr [ebp - 30h]
        inc ecx
        mov eax, dword ptr [ebp + 1ch]
        mov edx, ecx
        and edx, 3
        shr ecx, 2
        rep stosd
        mov ecx, edx
        rep stosb
fill_ellipse_region_one_step:
        cmp dword ptr [ebp - 2ch], 0
        js fill_ellipse_region_one_advance_x
        dec dword ptr [ebp - 10h]
        dec ebx
        mov eax, dword ptr [ebp - 28h]
        sub eax, dword ptr [ebp - 18h]
        mov dword ptr [ebp - 28h], eax
        sub dword ptr [ebp - 2ch], eax
fill_ellipse_region_one_advance_x:
        inc dword ptr [ebp - 0ch]
        mov eax, dword ptr [ebp - 24h]
        add eax, dword ptr [ebp - 20h]
        mov dword ptr [ebp - 24h], eax
        add eax, dword ptr [ebp - 1ch]
        add dword ptr [ebp - 2ch], eax
        jmp fill_ellipse_region_one_test
fill_ellipse_region_two_setup:
        mov eax, dword ptr [ebp - 14h]
        sub eax, dword ptr [ebp - 1ch]
        mov edx, eax
        sar eax, 1
        add eax, edx
        sub eax, dword ptr [ebp - 24h]
        sub eax, dword ptr [ebp - 28h]
        sar eax, 1
        add dword ptr [ebp - 2ch], eax
fill_ellipse_region_two:
        mov edi, dword ptr [ebp - 4]
        add edi, dword ptr [ebp - 0ch]
        cmp edi, dword ptr [ebp - 38h]
        jl fill_ellipse_region_two_step
        cmp edi, dword ptr [ebp - 40h]
        jl fill_ellipse_region_two_right_ready
        mov edi, dword ptr [ebp - 40h]
fill_ellipse_region_two_right_ready:
        mov dword ptr [ebp - 34h], edi
        mov edi, dword ptr [ebp - 4]
        sub edi, dword ptr [ebp - 0ch]
        cmp edi, dword ptr [ebp - 40h]
        jg fill_ellipse_region_two_step
        cmp edi, dword ptr [ebp - 38h]
        jg fill_ellipse_region_two_left_ready
        mov edi, dword ptr [ebp - 38h]
fill_ellipse_region_two_left_ready:
        mov dword ptr [ebp - 30h], edi
        mov edx, dword ptr [ebp - 8]
        add edx, dword ptr [ebp - 10h]
        cmp edx, dword ptr [ebp - 3ch]
        jl fill_ellipse_region_two_step
        cmp edx, dword ptr [ebp - 44h]
        jg fill_ellipse_region_two_lower
        mov eax, edx
        imul dword ptr [ebp - 4ch]
        add eax, dword ptr [ebp - 48h]
        add eax, edi
        mov edi, eax
        mov ecx, dword ptr [ebp - 34h]
        sub ecx, dword ptr [ebp - 30h]
        inc ecx
        mov eax, dword ptr [ebp + 1ch]
        mov edx, ecx
        and edx, 3
        shr ecx, 2
        rep stosd
        mov ecx, edx
        rep stosb
fill_ellipse_region_two_lower:
        mov edi, dword ptr [ebp - 30h]
        mov edx, dword ptr [ebp - 8]
        sub edx, dword ptr [ebp - 10h]
        cmp edx, dword ptr [ebp - 3ch]
        jl fill_ellipse_region_two_step
        cmp edx, dword ptr [ebp - 44h]
        jg fill_ellipse_region_two_step
        mov eax, edx
        imul dword ptr [ebp - 4ch]
        add eax, dword ptr [ebp - 48h]
        add eax, edi
        mov edi, eax
        mov ecx, dword ptr [ebp - 34h]
        sub ecx, dword ptr [ebp - 30h]
        inc ecx
        mov eax, dword ptr [ebp + 1ch]
        mov edx, ecx
        and edx, 3
        shr ecx, 2
        rep stosd
        mov ecx, edx
        rep stosb
fill_ellipse_region_two_step:
        cmp dword ptr [ebp - 2ch], 0
        jns fill_ellipse_region_two_advance_y
        inc dword ptr [ebp - 0ch]
        mov eax, dword ptr [ebp - 24h]
        add eax, dword ptr [ebp - 20h]
        mov dword ptr [ebp - 24h], eax
        add dword ptr [ebp - 2ch], eax
fill_ellipse_region_two_advance_y:
        dec dword ptr [ebp - 10h]
        mov eax, dword ptr [ebp - 28h]
        sub eax, dword ptr [ebp - 18h]
        mov dword ptr [ebp - 28h], eax
        sub eax, dword ptr [ebp - 14h]
        sub dword ptr [ebp - 2ch], eax
        dec ebx
        js fill_ellipse_done
        jmp fill_ellipse_region_two
fill_ellipse_done:
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    }
}

/* Function start: 0x43E2D3 */
/* Segment preservation and the in-text lookup table identify this as part of
 * the original hand-written raster assembly. */
__declspec(naked) void GetRLETransformTrig(int angleTenths, int *cosine,
                                           int *sine)
{
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov ebx, dword ptr [ebp + 8]
        and ebx, ebx
        jns rle_trig_reduce_high
rle_trig_reduce_low:
        add ebx, 0e10h
        js rle_trig_reduce_low
        jmp rle_trig_reduce_high
rle_trig_subtract_turn:
        sub ebx, 0e10h
rle_trig_reduce_high:
        cmp ebx, 0e10h
        jg rle_trig_subtract_turn
        cmp ebx, 708h
        ja rle_trig_lower_half
        cmp ebx, 384h
        ja rle_trig_second_quadrant
        shl ebx, 2
        mov eax, dword ptr g_anRLEQuarterCosine_0043d4bf[ebx]
        neg ebx
        mov edx, dword ptr g_anRLEQuarterCosine_0043d4bf[ebx + 0e10h]
        jmp rle_trig_store
rle_trig_second_quadrant:
        neg ebx
        add ebx, 708h
        shl ebx, 2
        mov eax, dword ptr g_anRLEQuarterCosine_0043d4bf[ebx]
        neg eax
        neg ebx
        mov edx, dword ptr g_anRLEQuarterCosine_0043d4bf[ebx + 0e10h]
        jmp rle_trig_store
rle_trig_lower_half:
        neg ebx
        add ebx, 0e10h
        cmp ebx, 384h
        ja rle_trig_fourth_quadrant
        shl ebx, 2
        mov eax, dword ptr g_anRLEQuarterCosine_0043d4bf[ebx]
        neg ebx
        mov edx, dword ptr g_anRLEQuarterCosine_0043d4bf[ebx + 0e10h]
        neg edx
        jmp rle_trig_store
rle_trig_fourth_quadrant:
        neg ebx
        add ebx, 708h
        shl ebx, 2
        mov eax, dword ptr g_anRLEQuarterCosine_0043d4bf[ebx]
        neg eax
        neg ebx
        mov edx, dword ptr g_anRLEQuarterCosine_0043d4bf[ebx + 0e10h]
        neg edx
rle_trig_store:
        mov ebx, dword ptr [ebp + 0ch]
        mov dword ptr [ebx], eax
        mov ebx, dword ptr [ebp + 10h]
        mov dword ptr [ebx], edx
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
    }
}

/* Function start: 0x43E38B */
__declspec(naked) void CalculateRoundedRLEFixedProduct(int left, int right,
                                                       int *result)
{
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov eax, dword ptr [ebp + 8]
        imul dword ptr [ebp + 0ch]
        add eax, 8000h
        adc edx, 0
        mov ax, dx
        ror eax, 10h
        mov edi, dword ptr [ebp + 10h]
        mov dword ptr [edi], eax
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
    }
}

/* Function start: 0x43E3B1 */
__declspec(naked) void TransformRLEPoint(int *point, int *result,
                                         int *origin,
                                         unsigned int angleTenths,
                                         int scaleX, int scaleY)
{
    __asm {
        push ebp
        mov ebp, esp
        add esp, -20h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        lea eax, [ebp - 8]
        push eax
        lea eax, [ebp - 4]
        push eax
        push dword ptr [ebp + 14h]
        call GetRLETransformTrig
        add esp, 0ch
        mov esi, dword ptr [ebp + 8]
        mov edi, dword ptr [ebp + 10h]
        mov eax, dword ptr [esi]
        sub eax, dword ptr [edi]
        shl eax, 10h
        imul dword ptr [ebp + 18h]
        add eax, 8000h
        adc edx, 0
        mov ebx, edx
        mov eax, ebx
        imul dword ptr [ebp - 4]
        add eax, 8000h
        adc edx, 0
        mov ax, dx
        ror eax, 10h
        mov dword ptr [ebp - 0ch], eax
        mov eax, ebx
        imul dword ptr [ebp - 8]
        add eax, 8000h
        adc edx, 0
        mov ax, dx
        ror eax, 10h
        mov dword ptr [ebp - 14h], eax
        mov eax, dword ptr [esi + 4]
        sub eax, dword ptr [edi + 4]
        shl eax, 10h
        imul dword ptr [ebp + 1ch]
        add eax, 8000h
        adc edx, 0
        mov ecx, edx
        mov eax, ecx
        imul dword ptr [ebp - 4]
        add eax, 8000h
        adc edx, 0
        mov ax, dx
        ror eax, 10h
        mov dword ptr [ebp - 18h], eax
        mov eax, ecx
        imul dword ptr [ebp - 8]
        add eax, 8000h
        adc edx, 0
        mov ax, dx
        ror eax, 10h
        mov dword ptr [ebp - 10h], eax
        mov esi, dword ptr [ebp + 0ch]
        mov edx, dword ptr [ebp - 0ch]
        sub edx, dword ptr [ebp - 10h]
        add edx, dword ptr [edi]
        mov dword ptr [esi], edx
        mov edx, dword ptr [ebp - 18h]
        add edx, dword ptr [ebp - 14h]
        add edx, dword ptr [edi + 4]
        mov dword ptr [esi + 4], edx
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
    }
}

/* Function start: 0x43E478 */
/* The preserved segment register identifies this as another handwritten
 * raster-library accessor.  Offset 8 is the row count used by the raw-frame
 * blitter below this interface. */
__declspec(naked) unsigned int GetRawImageHeight(unsigned char *shape)
{
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov esi, dword ptr [ebp + 8]
        mov eax, dword ptr [esi + 8]
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
    }
}

/* Function start: 0x43E48B */
__declspec(naked) unsigned int GetRawFrameWidth(unsigned char *shape,
                                                 int frame)
{
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov eax, dword ptr [ebp + 0ch]
        shl eax, 2
        add eax, dword ptr [ebp + 8]
        add eax, 10h
        mov esi, dword ptr [eax]
        add esi, dword ptr [ebp + 8]
        mov eax, dword ptr [esi]
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
    }
}

/* Function start: 0x43E4AB */
/* This clipped raw-frame blitter preserves the original hand-written segment
 * setup, REP copies, and XLAT palette-translation path. */
__declspec(naked) int BlitRawFrame(
    RasterClip *clip, int x, int y, unsigned char *shape,
    int frame, unsigned char *translation)
{
    __asm {
        push ebp
        mov ebp, esp
        add esp, -30h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp + 8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx + 4]
        inc eax
        mov dword ptr [ebp - 28h], eax
        jle raw_frame_invalid_surface
        mov eax, dword ptr [ebx + 8]
        inc eax
        mov ecx, eax
        jle raw_frame_invalid_surface
        mov eax, dword ptr [esi + 4]
        mov dword ptr [ebp - 2ch], eax
        cmp eax, 0
        jg raw_frame_left_ready
        mov eax, 0
raw_frame_left_ready:
        mov dword ptr [ebp - 14h], eax
        mov eax, dword ptr [esi + 8]
        mov dword ptr [ebp - 30h], eax
        cmp eax, 0
        jg raw_frame_top_ready
        mov eax, 0
raw_frame_top_ready:
        mov dword ptr [ebp - 18h], eax
        mov eax, dword ptr [esi + 0ch]
        mov edx, dword ptr [ebp - 28h]
        dec edx
        cmp eax, edx
        jl raw_frame_right_ready
        mov eax, edx
raw_frame_right_ready:
        mov dword ptr [ebp - 1ch], eax
        mov eax, dword ptr [esi + 10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl raw_frame_bottom_ready
        mov eax, edx
raw_frame_bottom_ready:
        mov dword ptr [ebp - 20h], eax
        mov eax, dword ptr [ebp - 1ch]
        cmp eax, dword ptr [ebp - 14h]
        jl raw_frame_empty_clip
        mov eax, dword ptr [ebp - 20h]
        cmp eax, dword ptr [ebp - 18h]
        jl raw_frame_empty_clip
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp - 24h], eax
        jmp raw_frame_begin
raw_frame_invalid_surface:
        mov eax, -1
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
raw_frame_empty_clip:
        mov eax, -2
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
raw_frame_begin:
        mov eax, dword ptr [ebp - 2ch]
        add dword ptr [ebp + 0ch], eax
        mov eax, dword ptr [ebp - 30h]
        add dword ptr [ebp + 10h], eax
        mov esi, dword ptr [ebp + 14h]
        mov edx, dword ptr [esi + 8]
        mov eax, dword ptr [ebp + 18h]
        shl eax, 2
        add eax, dword ptr [ebp + 14h]
        add eax, 10h
        mov esi, dword ptr [eax]
        add esi, dword ptr [ebp + 14h]
        mov dword ptr [ebp - 8], 0
        mov ecx, dword ptr [esi]
        mov dword ptr [ebp - 4], ecx
        cmp ecx, 0
        jz raw_frame_done
        add esi, 4
        mov edi, dword ptr [ebp + 8]
        mov eax, dword ptr [ebp - 1ch]
        inc eax
        sub eax, ecx
        sub eax, dword ptr [ebp + 0ch]
        jns raw_frame_clip_left
        add ecx, eax
        jle raw_frame_done
raw_frame_clip_left:
        mov eax, dword ptr [ebp + 0ch]
        sub eax, dword ptr [ebp - 14h]
        jns raw_frame_clip_bottom
        add ecx, eax
        jle raw_frame_done
        sub esi, eax
        sub dword ptr [ebp + 0ch], eax
raw_frame_clip_bottom:
        mov eax, dword ptr [ebp - 20h]
        inc eax
        sub eax, edx
        sub eax, dword ptr [ebp + 10h]
        jns raw_frame_clip_top
        add edx, eax
        jle raw_frame_done
raw_frame_clip_top:
        mov eax, dword ptr [ebp + 10h]
        sub eax, dword ptr [ebp - 18h]
        jns raw_frame_position
        add edx, eax
        jle raw_frame_done
        sub dword ptr [ebp + 10h], eax
        imul eax, dword ptr [ebp - 4]
        sub esi, eax
raw_frame_position:
        mov dword ptr [ebp - 10h], edx
        mov eax, dword ptr [ebp + 10h]
        imul dword ptr [ebp - 28h]
        add eax, dword ptr [ebp - 24h]
        add eax, dword ptr [ebp + 0ch]
        mov edi, eax
        mov dword ptr [ebp - 8], ecx
        sub dword ptr [ebp - 4], ecx
        mov eax, dword ptr [ebp - 28h]
        sub eax, ecx
        mov dword ptr [ebp - 0ch], eax
        mov edx, dword ptr [ebp - 10h]
        cmp dword ptr [ebp + 1ch], 0
        jnz raw_frame_translate
raw_frame_copy_row:
        shr ecx, 1
        rep movsw
        adc ecx, 0
        rep movsb
        mov ecx, dword ptr [ebp - 8]
        add esi, dword ptr [ebp - 4]
        add edi, dword ptr [ebp - 0ch]
        dec edx
        jnz raw_frame_copy_row
        mov eax, dword ptr [ebp - 4]
        add eax, dword ptr [ebp - 8]
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
raw_frame_translate:
        jecxz raw_frame_done
        mov ebx, dword ptr [ebp + 1ch]
raw_frame_translate_pixel:
        mov al, byte ptr [esi]
        xlatb
        cmp al, 0ffh
        jz raw_frame_skip_translated
        mov byte ptr [edi], al
raw_frame_skip_translated:
        inc esi
        inc edi
        loop raw_frame_translate_pixel
        mov ecx, dword ptr [ebp - 8]
        add esi, dword ptr [ebp - 4]
        add edi, dword ptr [ebp - 0ch]
        dec edx
        jnz raw_frame_translate_pixel
        mov eax, dword ptr [ebp - 4]
        add eax, dword ptr [ebp - 8]
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
raw_frame_done:
        mov eax, dword ptr [ebp - 4]
        add eax, dword ptr [ebp - 8]
        pop es
        pop edi
        pop esi
        pop ebx
        leave
        ret
    }
}

/* Function start: 0x43E63E */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* Preserve the segment state used by the adjacent hand-written raw blitters. */
__declspec(naked) void BlitSelectedRawFrames(
    RasterClip *clip, int x, int y, unsigned char *shape,
    const unsigned char *frames, unsigned char *translation)
{
#ifdef _MSC_VER
#include "screens_blit_selected_raw_frames.inc"
#else
    return;
#endif
}

/* Function start: 0x43E675 */
/* Shared by the ILBM, PCX, and GIF decoders.  The segment preservation and
 * width-first REP copy identify this as a hand-written scanline blitter. */
__declspec(naked) int BlitRawScanline(RasterClip *clip, int y,
                                      const unsigned char *pixels,
                                      int width)
{
#ifdef _MSC_VER
#include "screens_blit_raw_scanline.inc"
#else
    return 0;
#endif
}

/* Function start: 0x43E784 */
/* The adjacent BMHD/CMAP/BODY identifiers and big-endian chunk length identify
 * this as the common IFF chunk-data locator. */
__declspec(naked) unsigned char *FindIFFChunkData(
    const char *chunkId, const unsigned char *iffData)
{
#ifdef _MSC_VER
#include "screens_find_iff_chunk_data.inc"
#else
    return 0;
#endif
}

/* Function start: 0x43E7C6 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* Preserve the hand-written ByteRun1 and planar-to-chunky decoder loops. */
__declspec(naked) unsigned int DecodeIFFImage(
    RasterClip *clip, const unsigned char *iffData)
{
#ifdef _MSC_VER
#include "screens_decode_iff_image.inc"
#else
    return 0;
#endif
}

/* Function start: 0x43E98D */
__declspec(naked) void CopyILBMPalette(const unsigned char *iffData,
                                       unsigned char *palette)
{
#ifdef _MSC_VER
#include "screens_copy_ilbm_palette.inc"
#else
    return;
#endif
}

/* Function start: 0x43E9BE */
__declspec(naked) unsigned int GetILBMImageSize(
    const unsigned char *iffData)
{
#ifdef _MSC_VER
#include "screens_get_ilbm_image_size.inc"
#else
    return 0;
#endif
}

/* Function start: 0x43E9EB */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* Preserve the hand-written PCX run decoder and its ES scanline writes. */
__declspec(naked) unsigned int DecodePCXImage(
    RasterClip *clip, const unsigned char *pcxData)
{
#ifdef _MSC_VER
#include "screens_decode_pcx_image.inc"
#else
    return 0;
#endif
}

/* Function start: 0x43EA6D */
/* A PCX file stores its 256 RGB palette entries in the final 768 bytes;
 * raster palette components are converted from eight to six bits here. */
__declspec(naked) void CopyPCXPaletteFromFileTail(
    const unsigned char *fileData, unsigned int fileSize,
    unsigned char *palette)
{
#ifdef _MSC_VER
#include "screens_copy_pcx_palette_from_file_tail.inc"
#else
    return;
#endif
}

/* Function start: 0x43EA98 */
__declspec(naked) unsigned int GetPCXImageSize(const unsigned char *header)
{
#ifdef _MSC_VER
#include "screens_get_pcx_image_size.inc"
#else
    return 0;
#endif
}

/* Function start: 0x43EABF */
/* These five helpers form the register-calling-convention core of the GIF LZW
 * decoder at 0x0043EC29.  EDI holds its workspace throughout the group. */
__declspec(naked) void ResetGIFLZWDictionary(void)
{
#ifdef _MSC_VER
#include "screens_reset_gif_lzw_dictionary.inc"
#else
    return;
#endif
}

/* Function start: 0x43EB07 */
__declspec(naked) unsigned int ReadGIFDataSubBlockByte(void)
{
#ifdef _MSC_VER
#include "screens_read_gif_data_sub_block_byte.inc"
#else
    return 0;
#endif
}

/* Function start: 0x43EB20 */
__declspec(naked) unsigned int ReadGIFLZWCode(void)
{
#ifdef _MSC_VER
#include "screens_read_gif_lzw_code.inc"
#else
    return 0;
#endif
}

/* Function start: 0x43EB66 */
__declspec(naked) void AppendGIFLZWDictionaryEntry(void)
{
#ifdef _MSC_VER
#include "screens_append_gif_lzw_dictionary_entry.inc"
#else
    return;
#endif
}

/* Function start: 0x43EBAC */
__declspec(naked) void EmitGIFDecodedPixel(void)
{
#ifdef _MSC_VER
#include "screens_emit_gif_decoded_pixel.inc"
#else
    return;
#endif
}

/* Function start: 0x43EC29 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* The decoder uses the register-convention GIF helpers and explicit segment
 * state of the hand-written raster library. */
__declspec(naked) unsigned int ExpandGIFLZWImage(
    RasterClip *clip, const unsigned char *gifData, void *workspace)
{
#ifdef _MSC_VER
#include "screens_expand_gif_lzw_image.inc"
#else
    return 0;
#endif
}

/* Function start: 0x43EE42 */
/* GIF global and local color tables use the same packed size field; a local
 * table, when present, replaces the global palette copied first. */
__declspec(naked) void CopyGIFPalette(const unsigned char *gifData,
                                      unsigned char *palette)
{
#ifdef _MSC_VER
#include "screens_copy_gif_palette.inc"
#else
    return;
#endif
}

/* Function start: 0x43EEA3 */
__declspec(naked) unsigned int GetGIFImageSize(
    const unsigned char *gifData)
{
#ifdef _MSC_VER
#include "screens_get_gif_image_size.inc"
#else
    return 0;
#endif
}

/* Function start: 0x43EEDB */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* Retain the segment-preserving raster-library accessor verbatim. */
__declspec(naked) unsigned int GetRLEFrameDimensions(
    unsigned char *shape, int frame)
{
#ifdef _MSC_VER
#include "screens_get_rle_frame_dimensions.inc"
#else
    return 0;
#endif
}

/* Function start: 0x43EEFD */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* Retain the segment-preserving raster-library accessor verbatim. */
__declspec(naked) unsigned int GetRLEFrameExtents(
    unsigned char *shape, int frame)
{
#ifdef _MSC_VER
#include "screens_get_rle_frame_extents.inc"
#else
    return 0;
#endif
}

/* Function start: 0x43EF20 */
/* The ES save with no C-visible use identifies this pair as hand-written
 * raster-library accessors, so retain the original instruction sequence. */
__declspec(naked) unsigned int GetRLEImageSize(unsigned char *shape, int frame)
{
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov esi, dword ptr [ebp + 8]
        add esi, 8
        mov eax, dword ptr [ebp + 0ch]
        shl eax, 3
        add esi, eax
        mov esi, dword ptr [esi]
        add esi, dword ptr [ebp + 8]
        mov eax, dword ptr [esi + 10h]
        sub eax, dword ptr [esi + 8]
        inc eax
        mov ebx, dword ptr [esi + 14h]
        sub ebx, dword ptr [esi + 0ch]
        inc ebx
        shl eax, 10h
        mov ax, bx
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
    }
}

/* Function start: 0x43EF54 */
__declspec(naked) unsigned int GetRLEImageOrigin(unsigned char *shape,
                                                  int frame)
{
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov esi, dword ptr [ebp + 8]
        add esi, 8
        mov eax, dword ptr [ebp + 0ch]
        shl eax, 3
        add esi, eax
        mov esi, dword ptr [esi]
        add esi, dword ptr [ebp + 8]
        mov eax, dword ptr [esi + 8]
        shl eax, 10h
        mov ax, word ptr [esi + 0ch]
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
    }
}

/* Function start: 0x43EF7E */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* Preserve the hand-written palette-delta stream traversal. */
__declspec(naked) void ApplyRLEFramePalette(
    unsigned char *shape, int frame, unsigned char *palette)
{
#ifdef _MSC_VER
#include "screens_apply_rle_frame_palette.inc"
#else
    return;
#endif
}

/* Function start: 0x43EFC9 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* Preserve the hand-written palette-delta stream copy. */
__declspec(naked) unsigned int CopyRLEFramePalette(
    unsigned char *shape, int frame, unsigned char *entries)
{
#ifdef _MSC_VER
#include "screens_copy_rle_frame_palette.inc"
#else
    return 0;
#endif
}

/* Function start: 0x43F011 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* Preserve the hand-written palette-delta stream copy. */
__declspec(naked) unsigned int SetRLEFramePalette(
    unsigned char *shape, int frame, const unsigned char *entries)
{
#ifdef _MSC_VER
#include "screens_set_rle_frame_palette.inc"
#else
    return 0;
#endif
}

/* Function start: 0x43F05B */
__declspec(naked) unsigned int GetRLEFrameCount(const unsigned char *shape)
{
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        mov esi, dword ptr [ebp + 8]
        mov eax, dword ptr [esi + 4]
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0c9h
        ret
    }
}

/* Function start: 0x43F06E */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* Retain the segment-preserving directory scan from the raster library. */
__declspec(naked) int CollectUniqueRLEImageFrames(
    const unsigned char *shape, unsigned int *frames)
{
#ifdef _MSC_VER
#include "screens_collect_unique_rle_image_frames.inc"
#else
    return 0;
#endif
}

/* Function start: 0x43F0D0 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* Retain the segment-preserving directory scan from the raster library. */
__declspec(naked) int CollectUniqueRLEPaletteFrames(
    const unsigned char *shape, unsigned int *frames)
{
#ifdef _MSC_VER
#include "screens_collect_unique_rle_palette_frames.inc"
#else
    return 0;
#endif
}

/* Function start: 0x43F425 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* Preserve the callback-driven, fixed-point palette fade implemented by the
 * hand-written raster library. */
__declspec(naked) void FadeRasterPaletteToPalette(
    RasterSurface *surface, const unsigned char *targetPalette,
    unsigned int duration)
{
#ifdef _MSC_VER
#include "screens_fade_raster_palette_to_palette.inc"
#else
    return;
#endif
}

/* Function start: 0x43F5A9 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
/* Preserve the segment-based colour-set scan used by the raster library. */
__declspec(naked) int CollectRasterClipColours(
    RasterClip *clip, unsigned int *colours)
{
#ifdef _MSC_VER
#include "screens_collect_raster_clip_colours.inc"
#else
    return 0;
#endif
}

#else
#include "screens_portable.inc"
#endif /* !WC1_SDL */
