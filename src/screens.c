/*
 *  Full-screen presentation screens.
 *
 *  Address range 0x437000-0x43cfff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: unbroken run of Blit* screens plus ShowGetReady/Victory/GameOver.
 */
#include "wc1.h"

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
    WaitForSceneAdvance(duration);
    switch (g_nConversationMedalIndex_00598c08) {
    case 0:
    case 1:
        StartMusicTrack(39, 1, -1);
        break;
    case 2:
    case 4:
        StartMusicTrack(40, 1, -1);
        break;
    case 3:
        StartMusicTrack(38, 1, -1);
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
    WaitForSceneAdvance(duration);
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
    WaitForSceneAdvance(duration);
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
    WaitForSceneAdvance(duration);
    return 0;
}

/* Function start: 0x4375C0 */
unsigned int DrawMedals(void)
{
    unsigned char *campaign;
    short badge;
    short medal;
    short rowY;
    short stack;
    short x;

    campaign = (unsigned char *)&g_stCampaignState_0059ca50;
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
        if (campaign[0x0d + badge] != 0) {
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
        if (campaign[8 + medal] != 0) {
            x = g_asMedalDisplayX_0046e2d0[medal];
            stack = rowY;
            if (medal < 3) {
                badge = 0;
                while (badge < (signed char)campaign[8 + medal]) {
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
            (unsigned char *)FetchDiskPacketRetrying(4, 4, 0);
    if (g_pBriefingPortraitShape_00598c24 == 0)
        g_pBriefingPortraitShape_00598c24 =
            (unsigned char *)FetchDiskPacketRetrying(4, 5, 0);
    AddPCName(text);
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    frame = 0;
    FormatTextBufferFromStart(g_szEstablishingShotTextFormat_0046e640,
                              0, 160,
                              g_nConversationTextColour_00598c10,
                              g_szTextScratchBuffer_00598b00);
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
    WaitForSceneAdvance(duration);
    StartMusicTrack(25, 1, -1);
    return 0;
}

/* Function start: 0x4378D0 */
unsigned int DrawBriefingLongShot(void)
{
    short character;

    if (g_pBriefingBodyShape_00598c1c == 0)
        g_pBriefingBodyShape_00598c1c =
            (unsigned char *)FetchDiskPacketRetrying(4, 4, 0);
    if (g_pBriefingPortraitShape_00598c24 == 0)
        g_pBriefingPortraitShape_00598c24 =
            (unsigned char *)FetchDiskPacketRetrying(4, 5, 0);
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

    StartMusicTrack(26, 1, -1);
    if (g_pBriefingBodyShape_00598c1c == 0)
        g_pBriefingBodyShape_00598c1c =
            (unsigned char *)FetchDiskPacketRetrying(4, 4, 0);
    if (g_pBriefingPortraitShape_00598c24 == 0)
        g_pBriefingPortraitShape_00598c24 =
            (unsigned char *)FetchDiskPacketRetrying(4, 5, 0);
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
                (short)RandomBelowOrEqual(5) == 0)
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
    WaitForSceneAdvance(duration);
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
        FreePacketAndClear((int *)&g_pTalkingHeadShape_00598c0c, 0);
    if (g_pBriefingBodyShape_00598c1c != 0)
        FreePacketAndClear((int *)&g_pBriefingBodyShape_00598c1c, 0);
    if (g_pBriefingPortraitShape_00598c24 != 0)
        FreePacketAndClear((int *)&g_pBriefingPortraitShape_00598c24, 0);
    AddPCName(text);
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    FormatTextBufferFromStart(g_szDismissedTextFormat_0046e658,
                              0, 160,
                              g_nConversationTextColour_00598c10,
                              g_szTextScratchBuffer_00598b00);
    frame = 0;
    g_pTalkingHeadShape_00598c0c =
        (unsigned char *)FetchDiskPacketRetrying(4, 2, 0);
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
    WaitForSceneAdvance(duration);
    FreePacketAndClear((int *)&g_pTalkingHeadShape_00598c0c, 0);
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
    WaitForSceneAdvance(duration);
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
    if (currentMission == deathMission)
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
            if ((short)wing_status(first) != 3)
                return sceneData + second;
            break;
        case 5:
            first = int_value(&test);
            second = int_value(&test);
            if ((short)wing_status(first) == 3)
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
            if (g_asCollisionTime_005a7ca0[12] == 0)
                return sceneData + first;
            break;
        case 9:
            first = int_value(&test);
            if (g_asCollisionTime_005a7ca0[12] != 0)
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
        case 27:
            first = int_value(&test);
            second = int_value(&test);
            if (sighted(first) != 0)
                return sceneData + second;
            break;
        case 28:
            first = int_value(&test);
            second = int_value(&test);
            if (sighted(first) == 0)
                return sceneData + second;
            break;
        case 29:
            first = int_value(&test);
            second = int_value(&test);
            if ((short)wing_status(first) == 2)
                return sceneData + second;
            break;
        case 30:
            first = int_value(&test);
            second = int_value(&test);
            if ((short)wing_status(first) == 1)
                return sceneData + second;
            break;
        case 31:
            first = int_value(&test);
            second = int_value(&test);
            if ((short)ace_status(first, 1) == 0)
                return sceneData + second;
            break;
        case 32:
            first = int_value(&test);
            second = int_value(&test);
            if ((short)ace_status(first, 1) != 0)
                return sceneData + second;
            break;
        case 33:
            first = int_value(&test);
            second = int_value(&test);
            if ((short)ace_status(first, 2) == 0 &&
                (short)ace_status(first, 1) == 0)
                return sceneData + second;
            break;
        case 34:
            first = int_value(&test);
            second = int_value(&test);
            if ((short)ace_status(first, 2) != 0)
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
        (short *)AllocateTaggedMemory(0x140, 0);
    g_pMouthAnimationCommands_00598af4 =
        (short *)AllocateTaggedMemory(0x140, 0);
    return 0;
}

/* Function start: 0x438BC0 */
unsigned int FreeTalker(void)
{
    FreePacketAndClear((int *)&g_pConversationOverlayShape_00598c30, 0);
    FreePacketAndClear((int *)&g_pTalkingHeadShape_00598c0c, 0);
    FreePacketAndClear((int *)&g_pMouthAnimationCommands_00598af4, 0);
    FreePacketAndClear((int *)&g_pFaceAnimationCommands_00598c18, 0);
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
            if (record->testsOffset == 0)
                break;
            selected = ParseTests(record,
                                  (ConversationSceneRecord *)sceneBytes,
                                  textData);
            if (record == selected)
                break;
            record = selected;
        } while (1);
        if (selected->talker != -2)
            g_nConversationCharacter_0046e580 = selected->talker;
        duration = selected->duration;
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
                shot = 1;
                DrawBriefingLongShot();
                g_nTalkingHeadFace_0046e584 = -1;
                previousShot = shot;
            }
            break;
        case 2:
            if (previousShot != 2) {
                shot = 2;
                DrawPodiumShot();
                g_nTalkingHeadFace_0046e584 = -1;
                previousShot = shot;
            }
            break;
        case 4:
            shot = 4;
            if (g_nConversationCharacter_0046e580 < 0)
                g_cCurrentObjective_0046c020 =
                    (signed char)-g_nConversationCharacter_0046e580;
            else
                g_cCurrentObjective_0046c020 =
                    (signed char)g_nConversationCharacter_0046e580;
            g_nTalkingHeadFace_0046e584 = -1;
            previousShot = shot;
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
            shot = 50;
            g_nTalkingHeadFace_0046e584 = -1;
            previousShot = shot;
            break;
        }
        if (previousColour != (short)selected->textColour &&
            selected->textColour != -1) {
            g_nConversationTextColour_00598c10 =
                g_asConversationTextColours_004699f0[
                    (short)selected->textColour];
            previousColour = selected->textColour;
        }
        g_pMouthAnimationCommands_00598af4[0] = -1;
        if (textData[selected->mouthAnimationOffset] != '\0')
            ParseMouthAnimation((char *)textData +
                                    selected->mouthAnimationOffset,
                                g_pMouthAnimationCommands_00598af4);
        g_pFaceAnimationCommands_00598c18[0] = -1;
        if (textData[selected->faceAnimationOffset] != '\0')
            ParseFaceAnimation((char *)textData +
                               selected->faceAnimationOffset,
                               g_pFaceAnimationCommands_00598c18);
        FlushInputEvents();
        text = (char *)textData + selected->textOffset;
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
            default:
                LongTalk(g_pTalkingHeadShape_00598c0c, text,
                         g_pMouthAnimationCommands_00598af4,
                         g_pFaceAnimationCommands_00598c18,
                         duration);
                break;
            }
        }
        record = selected + 1;
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
        FreePacketAndClear((int *)&g_pTalkingHeadShape_00598c0c, 0);
    if (g_pBriefingBodyShape_00598c1c != 0)
        FreePacketAndClear((int *)&g_pBriefingBodyShape_00598c1c, 0);
    if (g_pBriefingPortraitShape_00598c24 != 0)
        FreePacketAndClear((int *)&g_pBriefingPortraitShape_00598c24, 0);
    g_pTalkingHeadShape_00598c0c =
        (unsigned char *)FetchDiskPacketRetrying(4, 2, 0);
    DrawSpriteDefault(&DAT_005a76b0, -96, 0,
                      g_pConversationBackdropShape_00598c04, 1);
    DrawSpriteDefault(&DAT_005a76b0, 224, 0,
                      g_pConversationBackdropShape_00598c04, 2);
    DrawSpriteDefault(&DAT_005a76b0, 240, 127,
                      g_pTalkingHeadShape_00598c0c, 0);
    DrawSpriteDefault(&DAT_005a76b0, 252, 127,
                      g_pConversationBackdropShape_00598c04, 3);
    RefreshMemoryStatusOverlay();
    FreePacketAndClear((int *)&g_pTalkingHeadShape_00598c0c, 0);
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
        WaitForSceneAdvance(duration);
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
        indices = (unsigned char *)AllocateTaggedMemory(256, 0);
        if (indices == 0)
            return;
        memset(indices, 0, 256);
        activeCount = CollectActivePaletteIndices(source, indices, 256);
        paletteBytes = (unsigned int)(activeCount * 6);
        originalPalette =
            (short *)AllocateTaggedMemory(paletteBytes, 0);
        transitionPalette =
            (short *)AllocateTaggedMemory(paletteBytes, 0);
        if (originalPalette == 0 || transitionPalette == 0) {
            ReleasePacketHandle((int)indices);
            if (originalPalette != 0)
                ReleasePacketHandle((int)originalPalette);
            if (transitionPalette != 0)
                ReleasePacketHandle((int)transitionPalette);
            return;
        }

        memset(originalPalette, 0, paletteBytes);
        memset(transitionPalette, 0, paletteBytes);
        GetPaletteEntry(
            (short)GetViewportPixel(destination,
                                    destination->left,
                                    destination->top),
            target);
        index = 0;
        while (index < activeCount) {
            GetPaletteEntry(
                (short)indices[index],
                (unsigned short *)&originalPalette[index * 3]);
            CachePaletteEntryFromWords((short)indices[index], target);
            memcpy(&transitionPalette[index * 3], target, 6);
            index++;
        }

        WaitForVerticalBlankThunk();
        DIBramPalette();
        CopyViewportContents(source, destination);
        DIBslam();
        DIBslamReal();

        while (StepPaletteTransition(
                   transitionPalette, originalPalette,
                   (short)(activeCount * 3)) != 0) {
            index = 0;
            while (index < activeCount) {
                CachePaletteEntryFromWords(
                    (short)indices[index],
                    (unsigned short *)&transitionPalette[index * 3]);
                index++;
            }
            WaitForVerticalBlankThunk();
            DIBramPalette();
        }

        ReleasePacketHandle((int)transitionPalette);
        ReleasePacketHandle((int)originalPalette);
        ReleasePacketHandle((int)indices);
    } else {
        CopyViewportContents(source, destination);
    }
    DIBslam();
    DIBslamReal();
}

/* Function start: 0x439840 */
unsigned int ShowGetReadyScreen(void)
{
    short frame;
    short distance;

    frame = 0;
    g_pIntroFont_005a8960 =
        (unsigned char *)FetchDiskPacketRetrying(9, 1, 0);
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
    ReleasePacketHandle((int)g_pIntroFont_005a8960);
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
        (unsigned char *)FetchDiskPacketRetrying(9, 17, 0);
    distance = 500;
    g_pIntroFont_005a8960 =
        (unsigned char *)FetchDiskPacketRetrying(9, 1, 0);
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
    ReleasePacketHandle((int)g_pIntroFont_005a8960);
    ReleasePacketHandle((int)g_pFireworkShape_005a6a68);
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
        (unsigned char *)FetchDiskPacketRetrying(9, 1, 0);
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
    StartMusicTrack(22, 2, 1);
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
    ReleasePacketHandle((int)g_pIntroFont_005a8960);
    return 0;
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
unsigned int DrawClippedLine(RasterClip *clip, int x1, int y1, int x2, int y2,
                             int mode, int colour)
{
    RasterSurface *surface;
    unsigned char *pixel;
    int stride;
    int clipLeft;
    int clipTop;
    int clipRight;
    int clipBottom;
    int deltaX;
    int deltaY;
    int stepX;
    int stepY;
    int error;
    int doubled;

    surface = clip->surface;
    if (surface == 0 || surface->pixels == 0 ||
        surface->maximumX < 0 || surface->maximumY < 0)
        return 0xffffffff;
    clipLeft = clip->left < 0 ? 0 : clip->left;
    clipTop = clip->top < 0 ? 0 : clip->top;
    clipRight = clip->right < surface->maximumX ?
        clip->right : surface->maximumX;
    clipBottom = clip->bottom < surface->maximumY ?
        clip->bottom : surface->maximumY;
    if (clipRight < clipLeft || clipBottom < clipTop)
        return 0xfffffffe;
    x1 += clip->left;
    x2 += clip->left;
    y1 += clip->top;
    y2 += clip->top;
    stride = surface->maximumX + 1;
    deltaX = AbsInt(x2 - x1);
    deltaY = AbsInt(y2 - y1);
    stepX = x1 < x2 ? 1 : -1;
    stepY = y1 < y2 ? 1 : -1;
    error = deltaX - deltaY;
    for (;;) {
        if (clipLeft <= x1 && x1 <= clipRight &&
            clipTop <= y1 && y1 <= clipBottom) {
            pixel = surface->pixels + y1 * stride + x1;
            if (mode == 0)
                *pixel = (unsigned char)colour;
            else if (mode == 1)
                *pixel = *(unsigned char *)(colour + *pixel);
        }
        if (x1 == x2 && y1 == y2)
            break;
        doubled = error * 2;
        if (doubled > -deltaY) {
            error -= deltaY;
            x1 += stepX;
        }
        if (doubled < deltaX) {
            error += deltaX;
            y1 += stepY;
        }
    }
    return 1;
}

/* Function start: 0x43A974 */
int DrawRLEImage(RasterClip *clip, unsigned char *shape, int frame,
                 int x, int y)
{
    RasterSurface *surface;
    RLEFrameHeader *frameHeader;
    unsigned char *stream;
    unsigned char *destination;
    unsigned char code;
    unsigned char colour;
    int stride;
    int clipLeft;
    int clipTop;
    int clipRight;
    int clipBottom;
    int imageLeft;
    int imageTop;
    int imageRight;
    int imageBottom;
    int drawX;
    int drawY;
    int runLength;
    int runLeft;
    int runRight;
    int sourceSkip;
    int copyLength;

    surface = clip->surface;
    if (surface == 0 || surface->pixels == 0 ||
        surface->maximumX < 0 || surface->maximumY < 0)
        return -1;
    clipLeft = clip->left < 0 ? 0 : clip->left;
    clipTop = clip->top < 0 ? 0 : clip->top;
    clipRight = clip->right < surface->maximumX ?
        clip->right : surface->maximumX;
    clipBottom = clip->bottom < surface->maximumY ?
        clip->bottom : surface->maximumY;
    if (clipRight < clipLeft || clipBottom < clipTop)
        return -2;
    if (shape == 0 || frame < 0 || frame >= *(int *)(shape + 4))
        return -4;

    frameHeader = (RLEFrameHeader *)(shape +
        *(int *)(shape + 8 + frame * 8));
    imageLeft = frameHeader->left + x + clip->left;
    imageTop = frameHeader->top + y + clip->top;
    imageRight = frameHeader->right + x + clip->left;
    imageBottom = frameHeader->bottom + y + clip->top;
    if (imageRight < imageLeft || imageBottom < imageTop)
        return -4;
    if (imageRight < clipLeft || clipRight < imageLeft ||
        imageBottom < clipTop || clipBottom < imageTop)
        return -3;
    if (clipLeft <= imageLeft && imageRight <= clipRight &&
        clipTop <= imageTop && imageBottom <= clipBottom)
        return DrawRLEImageUnclipped(clip, frameHeader, x, y);

    stride = surface->maximumX + 1;
    stream = (unsigned char *)frameHeader + sizeof(RLEFrameHeader);
    drawY = imageTop;
    while (drawY <= imageBottom) {
        drawX = imageLeft;
        for (;;) {
            code = *stream++;
            runLength = code >> 1;
            if ((code & 1) != 0) {
                if (runLength == 0) {
                    drawX += *stream++;
                } else {
                    runLeft = drawX;
                    runRight = drawX + runLength - 1;
                    if (drawY >= clipTop && drawY <= clipBottom &&
                        runLeft <= clipRight && runRight >= clipLeft) {
                        sourceSkip = clipLeft > runLeft ?
                            clipLeft - runLeft : 0;
                        copyLength = runLength - sourceSkip;
                        if (runRight > clipRight)
                            copyLength -= runRight - clipRight;
                        destination = surface->pixels + drawY * stride +
                                      runLeft + sourceSkip;
                        memcpy(destination, stream + sourceSkip,
                               copyLength);
                    }
                    stream += runLength;
                    drawX += runLength;
                }
            } else {
                if (runLength == 0)
                    break;
                colour = *stream++;
                runLeft = drawX;
                runRight = drawX + runLength - 1;
                if (drawY >= clipTop && drawY <= clipBottom &&
                    runLeft <= clipRight && runRight >= clipLeft) {
                    sourceSkip = clipLeft > runLeft ?
                        clipLeft - runLeft : 0;
                    copyLength = runLength - sourceSkip;
                    if (runRight > clipRight)
                        copyLength -= runRight - clipRight;
                    destination = surface->pixels + drawY * stride +
                                  runLeft + sourceSkip;
                    memset(destination, colour, copyLength);
                }
                drawX += runLength;
            }
        }
        drawY++;
    }
    return 0;
}

/* Function start: 0x43AD78 */
/* This is one of the original hand-written raster loops: it establishes ES,
 * decodes each prepared scan line, and uses REP stores/copies for the runs. */
#pragma optimize("", off)
__declspec(naked) int DrawRLEImageUnclipped(
    RasterClip *clip, RLEFrameHeader *frameHeader, int x, int y)
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
int DrawRLEImageColor(RasterClip *clip, unsigned char *shape, int frame,
                      int x, int y)
{
    RasterSurface *surface;
    RLEFrameHeader *frameHeader;
    unsigned char *stream;
    unsigned char *destination;
    unsigned char code;
    unsigned char colour;
    int stride;
    int clipLeft;
    int clipTop;
    int clipRight;
    int clipBottom;
    int imageLeft;
    int imageTop;
    int imageRight;
    int imageBottom;
    int drawX;
    int drawY;
    int runLength;
    int runLeft;
    int runRight;
    int sourceSkip;
    int copyLength;
    int pixel;

    surface = clip->surface;
    if (surface == 0 || surface->pixels == 0 ||
        surface->maximumX < 0 || surface->maximumY < 0)
        return -1;
    clipLeft = clip->left < 0 ? 0 : clip->left;
    clipTop = clip->top < 0 ? 0 : clip->top;
    clipRight = clip->right < surface->maximumX ?
        clip->right : surface->maximumX;
    clipBottom = clip->bottom < surface->maximumY ?
        clip->bottom : surface->maximumY;
    if (clipRight < clipLeft || clipBottom < clipTop)
        return -2;
    if (shape == 0 || frame < 0 || frame >= *(int *)(shape + 4))
        return -4;

    frameHeader = (RLEFrameHeader *)(shape +
        *(int *)(shape + 8 + frame * 8));
    imageLeft = frameHeader->left + x + clip->left;
    imageTop = frameHeader->top + y + clip->top;
    imageRight = frameHeader->right + x + clip->left;
    imageBottom = frameHeader->bottom + y + clip->top;
    if (imageRight < imageLeft || imageBottom < imageTop)
        return -4;
    if (imageRight < clipLeft || clipRight < imageLeft ||
        imageBottom < clipTop || clipBottom < imageTop)
        return -3;
    if (clipLeft <= imageLeft && imageRight <= clipRight &&
        clipTop <= imageTop && imageBottom <= clipBottom)
        return DrawRLEImageColorUnclipped(clip, frameHeader, x, y);

    stride = surface->maximumX + 1;
    stream = (unsigned char *)frameHeader + sizeof(RLEFrameHeader);
    drawY = imageTop;
    while (drawY <= imageBottom) {
        drawX = imageLeft;
        for (;;) {
            code = *stream++;
            runLength = code >> 1;
            if ((code & 1) != 0) {
                if (runLength == 0) {
                    drawX += *stream++;
                } else {
                    runLeft = drawX;
                    runRight = drawX + runLength - 1;
                    if (drawY >= clipTop && drawY <= clipBottom &&
                        runLeft <= clipRight && runRight >= clipLeft) {
                        sourceSkip = clipLeft > runLeft ?
                            clipLeft - runLeft : 0;
                        copyLength = runLength - sourceSkip;
                        if (runRight > clipRight)
                            copyLength -= runRight - clipRight;
                        destination = surface->pixels + drawY * stride +
                                      runLeft + sourceSkip;
                        pixel = 0;
                        while (pixel < copyLength) {
                            destination[pixel] =
                                g_abRasterPaletteTranslation_0046ff2c[
                                    stream[sourceSkip + pixel]];
                            pixel++;
                        }
                    }
                    stream += runLength;
                    drawX += runLength;
                }
            } else {
                if (runLength == 0)
                    break;
                colour = g_abRasterPaletteTranslation_0046ff2c[*stream++];
                runLeft = drawX;
                runRight = drawX + runLength - 1;
                if (drawY >= clipTop && drawY <= clipBottom &&
                    runLeft <= clipRight && runRight >= clipLeft) {
                    sourceSkip = clipLeft > runLeft ?
                        clipLeft - runLeft : 0;
                    copyLength = runLength - sourceSkip;
                    if (runRight > clipRight)
                        copyLength -= runRight - clipRight;
                    destination = surface->pixels + drawY * stride +
                                  runLeft + sourceSkip;
                    memset(destination, colour, copyLength);
                }
                drawX += runLength;
            }
        }
        drawY++;
    }
    return 0;
}

/* Function start: 0x43B336 */
/* Palette-translated counterpart to the original hand-written loop above. */
#pragma optimize("", off)
__declspec(naked) int DrawRLEImageColorUnclipped(
    RasterClip *clip, RLEFrameHeader *frameHeader, int x, int y)
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
int RotateRLEImage(RasterClip *clip, unsigned char *shape, int frame,
                   int x, int y, unsigned char *scratch,
                   unsigned int angleTenths, int scaleX, int scaleY,
                   unsigned int flags)
{
    RasterClip sourceClip;
    RasterSurface sourceSurface;
    RasterSurface *destinationSurface;
    unsigned int packedSize;
    unsigned int packedOrigin;
    int origin[2];
    int point[2];
    int transformed[2];
    int width;
    int height;
    int destinationStride;
    int clipLeft;
    int clipTop;
    int clipRight;
    int clipBottom;
    int minimumX;
    int minimumY;
    int maximumX;
    int maximumY;
    int corner;
    int destinationX;
    int destinationY;
    int deltaX;
    int deltaY;
    int sourceX;
    int sourceY;
    int sine;
    int cosine;
    unsigned char colour;
    __int64 numerator;

    if (angleTenths == 0 && scaleX == 0x10000 && scaleY == 0x10000) {
        if ((flags & 1) != 0)
            return DrawRLEImageColor(clip, shape, frame, x, y);
        return DrawRLEImage(clip, shape, frame, x, y);
    }
    if (shape == 0 || frame < 0 || frame >= *(int *)(shape + 4) ||
        scratch == 0 || scaleX == 0 || scaleY == 0)
        return -4;

    packedSize = GetRLEImageSize(shape, frame);
    width = (short)(packedSize >> 16);
    height = (short)packedSize;
    if (width <= 0 || height <= 0 || width * height > 0xfa00)
        return -4;
    packedOrigin = GetRLEImageOrigin(shape, frame);
    origin[0] = -(short)(packedOrigin >> 16);
    origin[1] = -(short)packedOrigin;

    sourceSurface.pixels = scratch;
    sourceSurface.maximumX = width - 1;
    sourceSurface.maximumY = height - 1;
    sourceSurface.field_C = 0;
    sourceSurface.field_10 = 0;
    sourceClip.surface = &sourceSurface;
    sourceClip.left = 0;
    sourceClip.top = 0;
    sourceClip.right = width - 1;
    sourceClip.bottom = height - 1;
    if ((flags & 2) == 0) {
        FillRasterClip(&sourceClip, 0xff);
        if ((flags & 1) != 0)
            DrawRLEImageColor(&sourceClip, shape, frame,
                              origin[0], origin[1]);
        else
            DrawRLEImage(&sourceClip, shape, frame,
                         origin[0], origin[1]);
    }

    destinationSurface = clip->surface;
    if (destinationSurface == 0 || destinationSurface->pixels == 0 ||
        destinationSurface->maximumX < 0 ||
        destinationSurface->maximumY < 0)
        return -1;
    clipLeft = clip->left < 0 ? 0 : clip->left;
    clipTop = clip->top < 0 ? 0 : clip->top;
    clipRight = clip->right < destinationSurface->maximumX ?
        clip->right : destinationSurface->maximumX;
    clipBottom = clip->bottom < destinationSurface->maximumY ?
        clip->bottom : destinationSurface->maximumY;
    if (clipRight < clipLeft || clipBottom < clipTop)
        return -2;

    minimumX = 0x7fffffff;
    minimumY = 0x7fffffff;
    maximumX = -0x7fffffff;
    maximumY = -0x7fffffff;
    corner = 0;
    while (corner < 4) {
        point[0] = (corner == 1 || corner == 2) ? width - 1 : 0;
        point[1] = (corner >= 2) ? height - 1 : 0;
        TransformRLEPoint(point, transformed, origin,
                          angleTenths, scaleX, scaleY);
        transformed[0] += x - origin[0];
        transformed[1] += y - origin[1];
        if (transformed[0] < minimumX)
            minimumX = transformed[0];
        if (transformed[0] > maximumX)
            maximumX = transformed[0];
        if (transformed[1] < minimumY)
            minimumY = transformed[1];
        if (transformed[1] > maximumY)
            maximumY = transformed[1];
        corner++;
    }
    if (minimumX < clipLeft)
        minimumX = clipLeft;
    if (minimumY < clipTop)
        minimumY = clipTop;
    if (maximumX > clipRight)
        maximumX = clipRight;
    if (maximumY > clipBottom)
        maximumY = clipBottom;
    if (maximumX < minimumX || maximumY < minimumY)
        return -3;

    sine = (int)SinFixed((short)(angleTenths / 10));
    cosine = (int)CosFixed((short)(angleTenths / 10));
    destinationStride = destinationSurface->maximumX + 1;
    destinationY = minimumY;
    while (destinationY <= maximumY) {
        destinationX = minimumX;
        while (destinationX <= maximumX) {
            deltaX = destinationX - x;
            deltaY = destinationY - y;
            numerator = ((__int64)deltaX * cosine +
                         (__int64)deltaY * sine) << 8;
            sourceX = origin[0] + (int)(numerator / scaleX);
            numerator = ((-(__int64)deltaX * sine +
                          (__int64)deltaY * cosine) << 8);
            sourceY = origin[1] + (int)(numerator / scaleY);
            if (sourceX >= 0 && sourceX < width &&
                sourceY >= 0 && sourceY < height) {
                colour = scratch[sourceY * width + sourceX];
                if (colour != 0xff)
                    destinationSurface->pixels[
                        destinationY * destinationStride + destinationX] =
                            colour;
            }
            destinationX++;
        }
        destinationY++;
    }
    return 0;
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

/* Function start: 0x43E3B1 */
void TransformRLEPoint(int *point, int *result, int *origin,
                       unsigned int angleTenths, int scaleX,
                       int scaleY)
{
    int sine;
    int cosine;
    int relativeX;
    int relativeY;
    int scaledX;
    int scaledY;
    __int64 value;

    sine = (int)SinFixed((short)(angleTenths / 10));
    cosine = (int)CosFixed((short)(angleTenths / 10));
    relativeX = point[0] - origin[0];
    relativeY = point[1] - origin[1];
    value = (__int64)relativeX * scaleX;
    scaledX = (int)((value + (value < 0 ? -0x8000 : 0x8000)) >> 16);
    value = (__int64)relativeY * scaleY;
    scaledY = (int)((value + (value < 0 ? -0x8000 : 0x8000)) >> 16);
    value = (__int64)scaledX * cosine - (__int64)scaledY * sine;
    result[0] = origin[0] +
        (int)((value + (value < 0 ? -0x80 : 0x80)) >> 8);
    value = (__int64)scaledY * cosine + (__int64)scaledX * sine;
    result[1] = origin[1] +
        (int)((value + (value < 0 ? -0x80 : 0x80)) >> 8);
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
