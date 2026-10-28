/* Scramble, landing, funeral scenes, and the animation demo.
 * The initialized scene tables precede their diagnostic and subtitle literals,
 * which end before the following AI state. See docs/ORDER.md for boundaries.
 */
#define WC1_CUTSCENE_SOURCE
#ifndef SDL_PORT
#include <signal.h>
#include <dos.h>
#endif
#include <sys/types.h>
#include "wc1.h"
#include <memory.h>

static signed char cScrambleLeftWalkerFrame = 7; /* 0x00465768 */
static signed char cScrambleRightWalkerFrame = 10; /* 0x0046576c */
/* 0x00465770 */
static signed char acScrambleWalkerOverlayFrames[8] = {
    3, 4, 5, 3, 4, 6, 6, 3
};
static short nScrambleLeftWalkerX = 70; /* 0x00465778 */
static short nScrambleRightWalkerX = 170; /* 0x0046577c */
static short nScrambleBackgroundX = 0; /* 0x00465780 */
/* 0x00465788 */
static unsigned short ausScrambleActorAnimationA[8] = {
    0, 1, 2, 3, 4, 5, 0x80, 0
};
/* 0x00465798 */
static unsigned short ausScrambleActorAnimationB[11] = {
    0, 0, 1, 2, 2, 2, 2, 2, 1, 1, 0x80
};
/* 0x004657b0 */
static ScrambleAnimationActor aScrambleAnimationActors[5] = {
    { 0, 0, 0, 0,  2,  0, 0,
      ausScrambleActorAnimationA, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 10,  0, 0,
      ausScrambleActorAnimationB, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 13,  0, 0,
      ausScrambleActorAnimationB, 0, 0, 0, 0 },
    { 0, 0, 0, 0,  0, -1, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0,  1, -1, 0, 0, 0, 0, 0, 0 }
};
/* 0x00465828 */
static ScrambleShipDetail aaScrambleShipDetails[4][32] = {
    {
        {  2,   23,   5 }, {  3, -112,  16 },
        {  6,  -94, -23 }, {  6,  -26,   5 },
        {  7,  -58, -11 }, {  7,  -98,   5 },
        {  7,  -14,  13 }, {  7,   60,   6 },
        {  7,  149,  24 }, {  9, -112, -24 },
        {  9,  -99,  -9 }, {  9, -116,  18 },
        {  9,  -81,  15 }, {  9,  -41,   9 },
        {  9,  -54,   1 }, {  9,  -12, -14 },
        {  9,   45,  18 }, {  9,   75,  11 },
        {  9,  130,  14 }, {  9,  166,  24 },
        {  0,  -68, -35 }, {  0,  -19,  -9 },
        {  0,   68,  13 }, {  8,  -76, -18 },
        {  8,  -33,  -1 }, {  8,   97,  31 },
        { 10,  -86,  10 }, { 10, -117,  47 },
        { 10,    9,   0 }, {  1, -111,  52 },
        {  1,  -50,  15 }, {  1,   75,   5 }
    },
    {
        {  3,  -88,  20 }, {  6,  -57,  29 },
        {  7, -139,  63 }, {  7,  -72,   5 },
        {  7,  -14,  28 }, {  9,  -91, -34 },
        {  9, -113, -14 }, {  9, -132,  33 },
        {  9,  -95,  34 }, {  9,  -60,  12 },
        {  9, -155,  52 }, {  9,  -66,  51 },
        {  9,  -56,  44 }, {  9,   44,  35 },
        {  9,   57,  25 }, {  9,   59,  -1 },
        {  9,  123,  41 }, {  9,  148,  30 },
        {  0, -135,  -2 }, {  0, -100,  59 },
        {  8,  -40,  11 }, { 10, -157, -19 },
        { 10,  -77,  35 }, {  6,  248,  17 },
        {  7,  300,  41 }, {  9,  172,  27 },
        {  9,  194,  12 }, {  9,  291,  17 },
        {  9,  290,  35 }, {  0,  233,  20 },
        {  8,  206,  17 }, { 10,  187,  42 }
    },
    {
        {  3,  -97,  -6 }, {  3,   89,   9 },
        {  6,  151,  -4 }, {  7,  -91, -39 },
        {  7,  -78, -16 }, {  7,  -56,  41 },
        {  7,  124,   1 }, {  7, -118,   0 },
        {  9, -107, -37 }, {  9, -127,  38 },
        {  9, -102,  21 }, {  9,  -67,  35 },
        {  9,  -54, -14 }, {  9,   12,  34 },
        {  9,   35,  39 }, {  9,   31,  21 },
        {  9,  130,  35 }, {  9,  185,  10 },
        {  0,  -42,  -3 }, {  0,   75,  35 },
        {  0,  177,  42 }, {  8, -126, -39 },
        { 10,  -16,  37 }, { 10,  148,   5 },
        {  1,   -3,  45 }, {  6,  160,  38 },
        {  7,  154,  16 }, {  9,  188,   3 },
        {  9,  150,  37 }, {  9,  214,  31 },
        {  9,  302,  16 }, {  8,  200,  15 }
    },
    {
        {  6,  103,  18 }, {  7, -114,   4 },
        {  7, -142,  39 }, {  7,   48,  23 },
        {  7,   88,  44 }, {  9,  -67, -30 },
        {  9, -134, -24 }, {  9,  -82,  -1 },
        {  9,  -45,  19 }, {  9, -153,  46 },
        {  9,   -8,  48 }, {  9,   11,  24 },
        {  9,   64,  27 }, {  9,  141,  38 },
        {  9,  154,  12 }, {  0, -105, -20 },
        {  0,   18,  14 }, {  0,  134,  20 },
        {  8, -153, -11 }, { 10,  -76,  64 },
        { 10,   33,  18 }, {  1,  -85, -28 },
        {  1, -154,  14 }, {  1,  148,  21 },
        {  3,  198,  23 }, {  6,  260,  14 },
        {  7,  238,  19 }, {  9,  165,  14 },
        {  9,  221,  22 }, {  0,  246,  16 },
        {  8,  221,  33 }, {  1,  282,  22 }
    }
};
/* 0x00465aa8 */
static int anLandingDamageDetailCounts[4] = {
    0, 8, 16, 24
};
/* 0x00465ab8 */
static const char *apszLandingDamageComments[4] = {
    "You got away pretty clean, sir!",
    "Looks like it got a little hot out there, sir!",
    "You sure got yourself shot up, sir!",
    "Glad to see you made it back alive, sir."
};
/* 0x00465ac8 */
static signed char acLandingCanopyFramesHeavy[24] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10,
    10, 10, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0x40
};
/* 0x00465ae0 */
static signed char acLandingCanopyFramesLight[24] = {
    0, 1, 2, 3, 11, 12, 13, 14, 14, 14, 14, 14,
    14, 14, 13, 12, 11, 3, 2, 1, 0, 0x40, 0, 0
};
/* 0x00465af8 */
static signed char acLandingCanopyFramesModerate[16] = {
    0, 15, 16, 17, 18, 19, 19, 18,
    17, 16, 15, 0, 0x40, 0, 0, 0
};
/* 0x00465b08 */
static const signed char *apLandingCanopyFrames[4] = {
    acLandingCanopyFramesLight,
    acLandingCanopyFramesModerate,
    acLandingCanopyFramesHeavy,
    acLandingCanopyFramesHeavy
};
/* 0x00465b18 */
static ShortPoint aFuneralParticleOrigins[7] = {
    { 234, 83 }, { 248, 85 }, { 260, 80 }, { 273, 78 },
    { 286, 75 }, { 299, 76 }, { 310, 74 }
};
/* 0x00465b38 */
static short asFuneralSceneBySeries[13] = {
    0, 1, 1, 1, 1, 1, 2, 3, 2, 3, 3, 2, 3
};
static int bFuneralShowTheEnd = 0; /* 0x00465b54 */

short nSceneScrambleShipDetailCountValue; /* 0x005a86cc */
signed char acScrambleShipDetailSlots[24]; /* 0x005a86d0 */
signed char cCutsceneScrambleWalkTime; /* 0x005a86e8 */
unsigned char *pCurrentConversationSpecialShape; /* 0x005a86ec */
ShortPoint aFuneralParticlesArray[7]; /* 0x005a86f0 */
unsigned char *pSceneScrambleBgShapePtr; /* 0x005a870c */
short nFuneralParticleDistance; /* 0x005a8710 */
short nScrambleBackdropPositionY; /* 0x005a8712 */
short nScrambleBackgroundRightCoordX; /* 0x005a8714 */
ShortPoint stCutsceneFuneralForeground; /* 0x005a8718 */
short nSceneFuneralMainScale; /* 0x005a871c */
short nFuneralRifleFrame; /* 0x005a871e */
short nCutsceneFuneralBaseX; /* 0x005a8720 */
short nCutsceneFuneralBasePosY; /* 0x005a8722 */
short nCurrentScrambleShipPositionX; /* 0x005a8724 */
short nScrambleShipY; /* 0x005a8726 */
short nFuneralParticleScale; /* 0x005a8728 */
unsigned char *pCurrentScrambleHangarShape; /* 0x005a872c */
unsigned char *pScrambleCockpitShapePtr; /* 0x005a8730 */
short nSceneScrambleWalkerPositionY; /* 0x005a8734 */

/* Function start: 0x407710 */
short __stdcall GetShapeFrameExtent(short x, short y,
                                    unsigned char *shape, short frame,
                                    short extent)
{
    short bounds[4];

    GetShapeFrameBounds(bounds, x, y, shape, frame);
    return bounds[extent];
}

/* Function start: 0x407750 */
unsigned int AnimateScrambleWalk(short ticks)
{
    signed char elapsed;

    nFrameSkipCounter = 1;
    elapsed = 0;
    if (ticks > 0) {
        do {
            PumpWindowMessages();
            nFrameSkipCounter--;
            if (nFrameSkipCounter < 1) {
                nFrameSkipCounter = nFrameSkip;
                DrawSpriteDefault(&stSceneBuffer,
                                  nScrambleBackgroundX, 0,
                                  pCurrentScrambleHangarShape, 0);
                DrawSpriteDefault(&stSceneBuffer,
                                  (short)(nScrambleBackgroundX +
                                          320),
                                  0, pCurrentScrambleHangarShape, 1);
                DrawSpriteDefault(&stSceneBuffer,
                                  (short)(nScrambleBackgroundX +
                                          640),
                                  0, pCurrentScrambleHangarShape, 0);
                DrawSpriteDefault(&stSceneBuffer,
                                  (short)(nScrambleBackgroundX +
                                          960),
                                  0, pCurrentScrambleHangarShape, 1);

                nSceneScrambleWalkerPositionY =
                    127 - GetShapeFrameExtent(
                        0, 0, pCurrentScrambleHangarShape,
                        cScrambleLeftWalkerFrame, 3);
                DrawSpriteDefault(&stSceneBuffer,
                                  nScrambleLeftWalkerX,
                                  nSceneScrambleWalkerPositionY,
                                  pCurrentScrambleHangarShape,
                                  cScrambleLeftWalkerFrame);
                DrawSpriteDefault(
                    &stSceneBuffer, nScrambleLeftWalkerX,
                    nSceneScrambleWalkerPositionY,
                    pCurrentScrambleHangarShape,
                    acScrambleWalkerOverlayFrames[
                        cScrambleWalkerPair * 2]);

                nSceneScrambleWalkerPositionY =
                    137 - GetShapeFrameExtent(
                        0, 0, pCurrentScrambleHangarShape,
                        cScrambleRightWalkerFrame, 3);
                DrawSpriteDefault(&stSceneBuffer,
                                  nScrambleRightWalkerX,
                                  nSceneScrambleWalkerPositionY,
                                  pCurrentScrambleHangarShape,
                                  cScrambleRightWalkerFrame);
                DrawSpriteDefault(
                    &stSceneBuffer, nScrambleRightWalkerX,
                    nSceneScrambleWalkerPositionY,
                    pCurrentScrambleHangarShape,
                    acScrambleWalkerOverlayFrames[
                        cScrambleWalkerPair * 2 + 1]);
                RefreshMemoryStatusOverlay();
                DIBslam();
                DIBslamReal();

                cScrambleLeftWalkerFrame++;
                if (cScrambleLeftWalkerFrame > 19)
                    cScrambleLeftWalkerFrame = 7;
                cScrambleRightWalkerFrame++;
                if (cScrambleRightWalkerFrame > 19)
                    cScrambleRightWalkerFrame = 7;
            }
            nScrambleBackgroundX =
                nScrambleBackgroundX - 12;
            nScrambleRightWalkerX =
                nScrambleRightWalkerX + 3;
            nScrambleLeftWalkerX =
                nScrambleLeftWalkerX + 2;
            if (bEscapePressedFlag == 1)
                break;
            elapsed++;
        } while (elapsed < ticks);
    }
    return 0;
}

/* Function start: 0x4079C0 */
unsigned int PlayScrambleHangarScene(void)
{
    int soundHandle; /* Unset in the original; ignored by the Win32 callee. */

#ifdef SDL_PORT
    soundHandle = 0;
#endif
    if (nMemoryConfig == 1)
        SceneLeaveHook(0x1b);
    else
        PreloadMusicTrackHook(0x1b);
    spacetrack(0x1b, 1, -1);

    InitializeConversationViewport();
    cScrambleLeftWalkerFrame = 7;
    cScrambleRightWalkerFrame = 10;
    nScrambleLeftWalkerX = 70;
    nScrambleBackgroundX = 0;
    nScrambleRightWalkerX = 170;
    pCurrentScrambleHangarShape =
        FetchDiskPacketRetrying(1, 0, 0);
    cScrambleWalkerPair =
        (signed char)((RandomWord() + 3) % 4);
    PlaySfxWaveFileByNumber(14, -1, 0);
    bEscapePressedFlag = 0;

    AnimateScrambleWalk(24);
    if (bEscapePressedFlag != 1) {
        cScrambleLeftWalkerFrame = 21;
        cScrambleRightWalkerFrame = 24;
        nScrambleLeftWalkerX = 90;
        nScrambleRightWalkerX = 200;
        nSceneScrambleWalkerPositionY = -14;
        nFrameSkipCounter = 1;
        for (cCutsceneScrambleWalkTime = 0;
             cCutsceneScrambleWalkTime < 24;
             cCutsceneScrambleWalkTime++) {
            PumpWindowMessages();
            nFrameSkipCounter--;
            if (nFrameSkipCounter < 1) {
                nFrameSkipCounter = nFrameSkip;
                DrawSpriteDefault(&stSceneBuffer,
                                  nScrambleBackgroundX, 0,
                                  pCurrentScrambleHangarShape, 2);
                DrawSpriteDefault(&stSceneBuffer,
                                  (short)(nScrambleBackgroundX +
                                          320),
                                  0, pCurrentScrambleHangarShape, 2);
                DrawSpriteDefault(&stSceneBuffer,
                                  (short)(nScrambleBackgroundX +
                                          640),
                                  0, pCurrentScrambleHangarShape, 2);
                DrawSpriteDefault(&stSceneBuffer,
                                  nScrambleLeftWalkerX,
                                  nSceneScrambleWalkerPositionY,
                                  pCurrentScrambleHangarShape,
                                  cScrambleLeftWalkerFrame);
                DrawSpriteDefault(&stSceneBuffer,
                                  nScrambleRightWalkerX,
                                  nSceneScrambleWalkerPositionY,
                                  pCurrentScrambleHangarShape,
                                  cScrambleRightWalkerFrame);
                RefreshMemoryStatusOverlay();
                DIBslam();
                DIBslamReal();

                cScrambleLeftWalkerFrame++;
                if (cScrambleLeftWalkerFrame > 26)
                    cScrambleLeftWalkerFrame = 21;
                cScrambleRightWalkerFrame++;
                if (cScrambleRightWalkerFrame > 26)
                    cScrambleRightWalkerFrame = 21;
            }
            nScrambleBackgroundX =
                (short)(nScrambleBackgroundX - 12);
            nScrambleLeftWalkerX =
                (short)(nScrambleLeftWalkerX + 2);
            nScrambleRightWalkerX =
                (short)(nScrambleRightWalkerX + 3);
            if (bEscapePressedFlag == 1)
                break;
        }

        if (bEscapePressedFlag != 1) {
            cScrambleLeftWalkerFrame = 7;
            cScrambleRightWalkerFrame = 10;
            AnimateScrambleWalk(24);
        }
    }

    FlushSoundEffectsAndLog(soundHandle, 10);
    ReleasePacketHandle(pCurrentScrambleHangarShape);
    ResetScreenClipToFullHeight();
    return 0;
}

/* Function start: 0x407C90 */
unsigned int DrawScrambleActor(short actorIndex)
{
    signed char frame;
    short x;
    unsigned char control;
    signed char animationFrame;
    short y;

    frame = 0;
    if ((animationFrame = aScrambleAnimationActors[
             actorIndex].animationFrame) != -1) {
        if (aScrambleAnimationActors[
                actorIndex].animationState != 0xa000) {
            animationFrame++;
            do {
                frame = aScrambleAnimationActors[
                    actorIndex].animation[animationFrame];
                control = (unsigned char)frame & 0xc0;
                switch (control) {
                    case 0:
                        aScrambleAnimationActors[
                            actorIndex].animationFrame++;
                        break;
                    case 0x40:
                        aScrambleAnimationActors[
                            actorIndex].animationState = 0;
                        animationFrame--;
                        break;
                    case 0x80:
                        animationFrame = frame & 0x3f;
                        aScrambleAnimationActors[
                            actorIndex].animationFrame = animationFrame;
                        break;
                }
            } while (control != 0);
        } else
            frame = animationFrame;
    }

    frame += aScrambleAnimationActors[actorIndex].baseFrame;
    x = nScrambleBackgroundRightCoordX +
        (aScrambleAnimationActors[actorIndex].x +=
         aScrambleAnimationActors[actorIndex].deltaX);
    y = nScrambleBackdropPositionY +
        (aScrambleAnimationActors[actorIndex].y +=
         aScrambleAnimationActors[actorIndex].deltaY);
    DrawSpriteScaled(
        pScrambleView,
        x,
        y,
        aScrambleAnimationActors[actorIndex].shape,
        (short)frame,
        aScrambleAnimationActors[actorIndex].angle,
        aScrambleAnimationActors[actorIndex].scale,
        aScrambleAnimationActors[actorIndex].flip);
    return 0;
}

/* Function start: 0x407D90 */
unsigned int ConfigureScrambleActor(short x, short y, short deltaX,
                                    short deltaY, unsigned char *shape,
                                    short scale, short angle,
                                    signed char flip, short actorIndex)
{
    aScrambleAnimationActors[actorIndex].x = x;
    aScrambleAnimationActors[actorIndex].y = y;
    aScrambleAnimationActors[actorIndex].deltaX = deltaX;
    aScrambleAnimationActors[actorIndex].deltaY = deltaY;
    aScrambleAnimationActors[actorIndex].shape = shape;
    if (aScrambleAnimationActors[actorIndex].animationFrame != -1)
        aScrambleAnimationActors[actorIndex].animationFrame = 0;
    aScrambleAnimationActors[actorIndex].scale = scale;
    aScrambleAnimationActors[actorIndex].angle = angle;
    aScrambleAnimationActors[actorIndex].flip = flip;
    return 0;
}

/* Function start: 0x407E10 */
unsigned int DrawScrambleFrame(void)
{
    short detailIndex;
    short detailSlot;
    const ScrambleShipDetail *detail;

    nFrameSkipCounter--;
    if (nFrameSkipCounter < 1) {
        nFrameSkipCounter = nFrameSkip;

        DrawSpriteDefault(&stSceneBuffer,
                          (short)(nScrambleBackgroundRightCoordX - 1),
                          nScrambleBackdropPositionY,
                          pSceneScrambleBgShapePtr, 0);
        DrawSpriteDefault(&stSceneBuffer,
                          nScrambleBackgroundRightCoordX,
                          nScrambleBackdropPositionY,
                          pSceneScrambleBgShapePtr, 1);
        DrawScrambleActor(0);
        DrawScrambleActor(3);
        DrawScrambleActor(4);
        DrawScrambleActor(2);

        if (bScrambleCanopyClosed == 0) {
            DrawSpriteDefault(&stSceneBuffer,
                              (short)(nCurrentScrambleShipPositionX + 40),
                              (short)(nScrambleShipY - 40),
                              pScrambleCanopyShape,
                              nScrambleCanopyOffset);
        }

        switch (stCampaignState.playerShipType) {
        case OBJECT_TYPE_HORNET:
            DrawSpriteDefault(&stSceneBuffer,
                              (short)(nCurrentScrambleShipPositionX + 10),
                              (short)(nScrambleShipY - 25),
                              pScrambleShipShape,
                              nSceneScrambleCanopyFrameValue);
            DrawSpriteDefault(&stSceneBuffer, nCurrentScrambleShipPositionX,
                              nScrambleShipY,
                              pScrambleCockpitShapePtr, 0);
            DrawSpriteScaled(&stSceneBuffer,
                             stScrambleCockpitDetail.x,
                             stScrambleCockpitDetail.y,
                             pScrambleCockpitShapePtr, 1,
                             nScrambleCockpitAngle, 255, 0);
            break;
        case OBJECT_TYPE_RAPIER:
            DrawSpriteDefault(&stSceneBuffer,
                              (short)(nCurrentScrambleShipPositionX + 10),
                              (short)(nScrambleShipY - 16),
                              pScrambleShipShape,
                              nSceneScrambleCanopyFrameValue);
            DrawSpriteDefault(&stSceneBuffer, nCurrentScrambleShipPositionX,
                              nScrambleShipY,
                              pScrambleCockpitShapePtr, 0);
            DrawSpriteDefault(&stSceneBuffer,
                              (short)(nCurrentScrambleShipPositionX + 153),
                              (short)(nScrambleShipY + 5),
                              pScrambleCockpitShapePtr, 2);
            DrawSpriteDefault(&stSceneBuffer,
                              stScrambleCockpitDetail.x,
                              stScrambleCockpitDetail.y,
                              pScrambleCockpitShapePtr, 1);
            break;
        case OBJECT_TYPE_SCIMITAR:
            DrawSpriteDefault(&stSceneBuffer,
                              (short)(nCurrentScrambleShipPositionX + 10),
                              (short)(nScrambleShipY - 15),
                              pScrambleShipShape,
                              nSceneScrambleCanopyFrameValue);
            DrawSpriteDefault(&stSceneBuffer, nCurrentScrambleShipPositionX,
                              nScrambleShipY,
                              pScrambleCockpitShapePtr, 0);
            DrawSpriteDefault(&stSceneBuffer,
                              (short)(nCurrentScrambleShipPositionX + 148),
                              nScrambleShipY,
                              pScrambleCockpitShapePtr, 2);
            DrawSpriteDefault(&stSceneBuffer,
                              stScrambleCockpitDetail.x,
                              stScrambleCockpitDetail.y,
                              pScrambleCockpitShapePtr, 1);
            break;
        case OBJECT_TYPE_RAPTOR:
            DrawSpriteDefault(&stSceneBuffer,
                              (short)(nCurrentScrambleShipPositionX + 10),
                              (short)(nScrambleShipY - 11),
                              pScrambleShipShape,
                              nSceneScrambleCanopyFrameValue);
            DrawSpriteDefault(&stSceneBuffer, nCurrentScrambleShipPositionX,
                              nScrambleShipY,
                              pScrambleCockpitShapePtr, 0);
            DrawSpriteDefault(&stSceneBuffer,
                              (short)(nCurrentScrambleShipPositionX + 158),
                              (short)(nScrambleShipY + 6),
                              pScrambleCockpitShapePtr, 2);
            DrawSpriteDefault(&stSceneBuffer,
                              stScrambleCockpitDetail.x,
                              stScrambleCockpitDetail.y,
                              pScrambleCockpitShapePtr, 1);
            break;
        }

        if (nSceneScrambleShipDetailCountValue > 0) {
            detailIndex = 0;
            do {
                detailSlot = acScrambleShipDetailSlots[detailIndex];
                detail = &aaScrambleShipDetails[
                    stCampaignState.playerShipType][detailSlot];
                DrawSpriteDefault(&stSceneBuffer,
                                  (short)(nCurrentScrambleShipPositionX + detail->x),
                                  (short)(nScrambleShipY + detail->y),
                                  pScrambleDetailShape,
                                  detail->frame);
                detailIndex++;
            } while (detailIndex < nSceneScrambleShipDetailCountValue);
        }

        if (bScrambleCanopyClosed == 1) {
            if (stScrambleOverlay.x != -1000) {
                DrawSpriteDefault(&stSceneBuffer,
                                  stScrambleOverlay.x,
                                  stScrambleOverlay.y,
                                  pScrambleOverlayShape, 0);
            }
            DrawSpriteDefault(&stSceneBuffer, 100, 127,
                              pScrambleCanopyShape,
                              nScrambleCanopyOffset);
        }
        RefreshMemoryStatusOverlay();
        DIBslam();
        DIBslamReal();
    }
    return 0;
}

/* Preserve the scramble actor and sound declaration context. */
unsigned int ConfigureScrambleActor(short x, short y, short deltaX,
                                    short deltaY, unsigned char *shape,
                                    short scale, short angle,
                                    signed char flip, short actorIndex);
void FlushSoundEffectsAndLog(int soundHandle, short fadeTicks);

/* Function start: 0x408200 (Mac symbol: scramble) */
unsigned int scramble(void)
{
    unsigned char *actorShape;
    short frame;
    int scrambleSound;
    int canopySound; /* Unset after the Win32 sound-call replacements. */

#ifdef SDL_PORT
    scrambleSound = 0;
    canopySound = 0;
#endif
    nScrambleCanopyOffset = 0;
    nSceneScrambleCanopyFrameValue = 0;
    stScrambleOverlay.x = -1000;
    nSceneScrambleShipDetailCountValue = 0;
    if (bEscapePressedFlag == 0) {
        InitializeConversationViewport();
        cCockpitLogicalFile =
            (signed char)(stCampaignState.playerShipType + 17);
        pScrambleCockpitShapePtr = FetchDiskPacketRetrying(
            cCockpitLogicalFile, 8, 0);
        pSceneScrambleBgShapePtr =
            FetchDiskPacketRetrying(1, 1, 0);
        bScrambleCanopyClosed = 0;
        pScrambleCanopyShape =
            FetchDiskPacketRetrying(1, 2, 0);
        pScrambleShipShape =
            FetchDiskPacketRetrying(1, 3, 0);
        actorShape = FetchDiskPacketRetrying(1, 4, 0);
        nScrambleBackdropPositionY = 0;
        pScrambleView = &stSceneBuffer;
        nScrambleBackgroundRightCoordX = 64;

        ConfigureScrambleActor(130, 94, 1, 0, actorShape,
                               0x100, 0, 0, 0);
        ConfigureScrambleActor(160, 120, 0, 0, actorShape,
                               0x100, 0, 0, 2);
        ConfigureScrambleActor(260, 100, -3, 0, actorShape,
                               0xff, 0, 0x10, 3);
        ConfigureScrambleActor(260, 100, -3, 0, actorShape,
                               0xff, 0, 0x10, 4);
        PlaySfxWaveFileByNumber(17, -1, 0);
        bEscapePressedFlag = 0;

        switch (stCampaignState.playerShipType) {
        case OBJECT_TYPE_HORNET:
            *(volatile short *)&nCurrentScrambleShipPositionX = -40;
            *(volatile short *)&nScrambleShipY = 96;
            stScrambleCockpitDetail.x = nCurrentScrambleShipPositionX - 55;
            stScrambleCockpitDetail.y = nScrambleShipY - 25;
            nScrambleCockpitAngle = 316;
            break;
        case OBJECT_TYPE_RAPIER:
            nCurrentScrambleShipPositionX = -30;
            nScrambleShipY = 80;
            stScrambleCockpitDetail.x = nCurrentScrambleShipPositionX + 15;
            stScrambleCockpitDetail.y = nScrambleShipY - 4;
            break;
        case OBJECT_TYPE_SCIMITAR:
            nCurrentScrambleShipPositionX = -40;
            nScrambleShipY = 86;
            stScrambleCockpitDetail.x = nCurrentScrambleShipPositionX + 44;
            stScrambleCockpitDetail.y = nScrambleShipY - 3;
            break;
        case OBJECT_TYPE_RAPTOR:
            nCurrentScrambleShipPositionX = -40;
            nScrambleShipY = 80;
            stScrambleCockpitDetail.x = nCurrentScrambleShipPositionX + 18;
            stScrambleCockpitDetail.y = nScrambleShipY - 13;
            break;
        }

        nFrameSkipCounter = 1;
        for (frame = 0; frame < 10; frame++) {
            PumpWindowMessages();
            DrawScrambleFrame();
            nScrambleShipY =
                (short)(nScrambleShipY - 2);
            stScrambleCockpitDetail.y =
                (short)(stScrambleCockpitDetail.y - 2);
            nCurrentScrambleShipPositionX =
                (short)(nCurrentScrambleShipPositionX + 4);
            stScrambleCockpitDetail.x =
                (short)(stScrambleCockpitDetail.x + 4);
            nScrambleBackgroundRightCoordX--;
            if (bEscapePressedFlag == 1)
                goto releaseScrambleResources;
        }

        if (bEscapePressedFlag != 1) {
            nFrameSkipCounter = 1;
            for (frame = 0; frame < 27; frame++) {
                PumpWindowMessages();
                DrawScrambleFrame();
                nCurrentScrambleShipPositionX += 4;
                stScrambleCockpitDetail.x += 4;
                nScrambleBackgroundRightCoordX--;
                nScrambleCanopyOffset++;
                if (nSceneScrambleCanopyFrameValue < 25)
                    nSceneScrambleCanopyFrameValue++;
                if (bEscapePressedFlag == 1)
                    break;
            }
        }

        if (bEscapePressedFlag != 1) {
            FlushSoundEffectsAndLog(scrambleSound, 0);
            nScrambleCanopyOffset--;
            PlaySfxWaveFileByNumber(15, -1, 0);
            nFrameSkipCounter = 1;
            for (frame = 0; frame < 23; frame++) {
                PumpWindowMessages();
                if (frame == 22)
                    nFrameSkipCounter = 1;
                DrawScrambleFrame();
                switch (stCampaignState.playerShipType) {
                case OBJECT_TYPE_HORNET:
                    nScrambleCockpitAngle =
                        (short)(nScrambleCockpitAngle + 2);
                    break;
                case OBJECT_TYPE_RAPIER:
                case OBJECT_TYPE_RAPTOR:
                    stScrambleCockpitDetail.x =
                        (short)(stScrambleCockpitDetail.x - 2);
                    break;
                case OBJECT_TYPE_SCIMITAR:
                    if (frame == 21)
                        stScrambleCockpitDetail.y++;
                    else
                        stScrambleCockpitDetail.x =
                            (short)(stScrambleCockpitDetail.x - 2);
                    break;
                }
                if (nSceneScrambleCanopyFrameValue < 35)
                    nSceneScrambleCanopyFrameValue++;
                if (bEscapePressedFlag == 1)
                    break;
            }

            FlushSoundEffectsAndLog(canopySound, 0);
            PlaySfxWaveFileByNumber(16, -1, 0);
            if (bEscapePressedFlag != 1)
                WaitForSceneAdvance(60, 0);
        }

releaseScrambleResources:
        bEscapePressedFlag = 0;
        ReleasePacketHandle(pScrambleCockpitShapePtr);
        ReleasePacketHandle(pSceneScrambleBgShapePtr);
        ReleasePacketHandle(pScrambleCanopyShape);
        ReleasePacketHandle(pScrambleShipShape);
        ReleasePacketHandle(actorShape);
        FlushSoundEffects();
        ResetScreenClipToFullHeight();
    }

    if (nMemoryConfig == 0) {
        StopMusicUnlessSuppressed();
        ReleaseMusicTrackHook(0x1b);
    }
    return 0;
}

/* Preserve the landing resource and event declaration context. */
void *FetchDiskPacketRetrying(short logicalFile, short section, short flags);
unsigned int spacetrack(int track, int mode, short enabled);
void __stdcall SetEventManagerPump(void (*pump)(void));

/* Function start: 0x408650 (Mac symbol: landing) */
unsigned int landing(signed char damageLevel)
{
    unsigned char *actorShape;
    short frame;
    signed char detail;
    signed char prior;
    /* The native routine saves this table byte offset in its four-byte frame. */
    volatile int damageOffset;

    PreloadMusicTrackHook(0x1d);
    spacetrack(0x1d, 2, 1);
    SetEventManagerPump(PollJoystickButtonEvents);
    InitializeConversationViewport();
    damageOffset = damageLevel * 4;
    nSceneScrambleShipDetailCountValue =
        anLandingDamageDetailCounts[damageLevel];
    for (frame = 0;
         frame < nSceneScrambleShipDetailCountValue;
         frame++) {
        do {
            detail = RandomInRange(0, 31);
            for (prior = 0; prior < frame; prior++) {
                if (acScrambleShipDetailSlots[prior] ==
                    detail)
                    break;
            }
        } while (prior < frame);
        acScrambleShipDetailSlots[frame] = detail;
    }

    cCockpitLogicalFile =
        (signed char)(stCampaignState.playerShipType + 17);
    pScrambleCockpitShapePtr =
        FetchDiskPacketRetrying(
            cCockpitLogicalFile, 8, 0);
    pSceneScrambleBgShapePtr =
        FetchDiskPacketRetrying(1, 1, 0);
    pScrambleShipShape =
        FetchDiskPacketRetrying(1, 3, 0);
    actorShape = FetchDiskPacketRetrying(1, 4, 0);
    pScrambleDetailShape =
        FetchDiskPacketRetrying(1, 9, 0);
    pScrambleOverlayShape =
        FetchDiskPacketRetrying(1, 5, 0);
    bScrambleCanopyClosed = 1;
    pScrambleCanopyShape =
        FetchDiskPacketRetrying(1, 6, 0);
    InitializeConversationText();
    nScrambleBackdropPositionY = 0;
    pScrambleView = &stSceneBuffer;
    nScrambleBackgroundRightCoordX = 32;
    ConfigureScrambleActor(140, 88, 2, 0, actorShape,
                           0x80, 0, 0, 3);
    ConfigureScrambleActor(139, 88, 2, 0, actorShape,
                           0x80, 0, 0, 4);
    ConfigureScrambleActor(240, 94, -1, 0, actorShape,
                           0x100, 0, 0x10, 0);
    ConfigureScrambleActor(160, 120, 0, 0, actorShape,
                           0x100, 0, 0, 2);
    nScrambleCanopyOffset = 0;
    nSceneScrambleCanopyFrameValue = 34;
    stScrambleOverlay.x = -1000;

    switch (stCampaignState.playerShipType) {
    case OBJECT_TYPE_HORNET:
        nCurrentScrambleShipPositionX = 124;
        nScrambleShipY = 140;
        stScrambleCockpitDetail.x = 69;
        stScrambleCockpitDetail.y = 115;
        nScrambleCockpitAngle = 360;
        break;
    case OBJECT_TYPE_RAPIER:
        nCurrentScrambleShipPositionX = 124;
        nScrambleShipY = 130;
        stScrambleCockpitDetail.x = 94;
        stScrambleCockpitDetail.y = 125;
        break;
    case OBJECT_TYPE_SCIMITAR:
        nScrambleShipY = 134;
        stScrambleCockpitDetail.y = 132;
        nCurrentScrambleShipPositionX = 124;
        stScrambleCockpitDetail.x = 124;
        break;
    case OBJECT_TYPE_RAPTOR:
        nCurrentScrambleShipPositionX = 124;
        nScrambleShipY = 126;
        stScrambleCockpitDetail.x = 96;
        stScrambleCockpitDetail.y = 113;
        break;
    }

    frame = 0;
    bEscapePressedFlag = 0;
    PlaySfxWaveFileByNumber(17, -1, 0);
    nFrameSkipCounter = 1;
    for (; frame < 30; frame++) {
        PumpWindowMessages();
        DrawScrambleFrame();
        nScrambleShipY =
            (short)(nScrambleShipY - 2);
        stScrambleCockpitDetail.y =
            (short)(stScrambleCockpitDetail.y - 2);
        if (bEscapePressedFlag == 1)
            break;
    }

    FlushSoundEffectsAndLog(damageOffset, 0);
    if (bEscapePressedFlag != 1) {
        short canopyFrame;
        const signed char *canopyFrames;

        stScrambleOverlay.x =
            (short)(nCurrentScrambleShipPositionX + 180);
#ifdef SDL_PORT
        canopyFrames = apLandingCanopyFrames[damageLevel];
#else
        canopyFrames = *(const signed char **)
            ((unsigned char *)apLandingCanopyFrames +
             damageOffset);
#endif
        stScrambleOverlay.y =
            (short)(nScrambleShipY + 50);
        PlaySfxWaveFileByNumber(15, -1, 0);
        nFrameSkipCounter = 1;
        for (canopyFrame = 0; canopyFrame < 30; canopyFrame++) {
            PumpWindowMessages();
            if (nRenderedSpaceFrame == 29)
                nFrameSkipCounter = 1;
            DrawScrambleFrame();
            stScrambleOverlay.y--;
            stScrambleOverlay.x =
                (short)(stScrambleOverlay.x - 4);
            switch (stCampaignState.playerShipType) {
            case OBJECT_TYPE_HORNET:
                nScrambleCockpitAngle =
                    (short)(nScrambleCockpitAngle - 2);
                break;
            case OBJECT_TYPE_RAPIER:
            case OBJECT_TYPE_RAPTOR:
                stScrambleCockpitDetail.x =
                    (short)(stScrambleCockpitDetail.x + 2);
                break;
            case OBJECT_TYPE_SCIMITAR:
                if (canopyFrame == 0)
                    stScrambleCockpitDetail.y--;
                else
                    stScrambleCockpitDetail.x =
                        (short)(stScrambleCockpitDetail.x + 2);
                break;
            }
            if (canopyFrame > 6 && *canopyFrames != 0x40) {
                nScrambleCanopyOffset = *canopyFrames;
                canopyFrames++;
            }
            if (bEscapePressedFlag == 1)
                break;
        }

        FlushSoundEffectsAndLog(damageOffset, 0);
        if (bEscapePressedFlag != 1) {
            ClearViewport(&stCurrentConvTextViewport,
                          cBlackColour);
            FormatTextBufferFromStart(
                "%X%Y%F%s%P", 0, 160,
                cBlueColour,
#ifdef SDL_PORT
                apszLandingDamageComments[damageLevel]);
#else
                *(const char **)
                    ((unsigned char *)apszLandingDamageComments +
                     damageOffset));
#endif
            DIBslam();
            DIBslamReal();
            ReleaseTextFont(0);
            WaitForSceneAdvance(300, 0);
        }
    }

    bEscapePressedFlag = 0;
    ReleasePacketHandle(pScrambleCockpitShapePtr);
    ReleasePacketHandle(pSceneScrambleBgShapePtr);
    ReleasePacketHandle(pScrambleShipShape);
    ReleasePacketHandle(actorShape);
    ReleasePacketHandle(pScrambleDetailShape);
    ReleasePacketHandle(pScrambleOverlayShape);
    ReleasePacketHandle(pScrambleCanopyShape);
    ResetScreenClipToFullHeight();
    ClearViewport(&stCurrentConvTextViewport,
                  cBlackColour);
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook(0x1d);
    return 0;
}

/* Function start: 0x408B30 (Mac symbol: funeral_player) */
unsigned int funeral_player(void)
{
    short index;

    nFrameSkipCounter--;
    if (nFrameSkipCounter < 1) {
        nFrameSkipCounter = nFrameSkip;
        DrawConstellationField();
        nSceneFuneralMainScale =
            (short)(0x7000L / nFuneralMainDist);
        DrawSpriteDefault(&stSceneBuffer,
                          nCutsceneFuneralBaseX,
                          nCutsceneFuneralBasePosY,
                          pCurrentConversationSpecialShape, 0);
        DrawSpriteScaled(&stSceneBuffer,
                         nFuneralCasketPositionX,
                         nFuneralCasketPosY,
                         pCurrentConversationSpecialShape,
                         8, 0, nSceneFuneralMainScale, 0);
        DrawSpriteDefault(&stSceneBuffer,
                          nCutsceneFuneralBaseX,
                          nCutsceneFuneralBasePosY,
                          pCurrentConversationSpecialShape, 1);
        DrawSpriteDefault(&stSceneBuffer,
                          nCutsceneFuneralBaseX,
                          nCutsceneFuneralBasePosY,
                          pCurrentConversationSpecialShape,
                          nCutsceneFuneralGuardFrame);
        DrawSpriteDefault(&stSceneBuffer,
                          nCutsceneFuneralBaseX,
                          nCutsceneFuneralBasePosY,
                          pCurrentConversationSpecialShape,
                          nFuneralRifleFrame);

        nFuneralParticleScale =
            (short)(0x1000L / nFuneralParticleDistance);
        for (index = 0; index < 7; index++) {
            if (aFuneralParticlesArray[index].x != 0) {
                DrawSpriteScaled(
                    &stSceneBuffer,
                    aFuneralParticlesArray[index].x,
                    aFuneralParticlesArray[index].y,
                    pCurrentConversationSpecialShape, 9, 0,
                    nFuneralParticleScale, 0);
                aFuneralParticlesArray[index].x -= 6;
                aFuneralParticlesArray[index].y -= 6;
                if (stSceneBuffer.top >
                    aFuneralParticlesArray[index].y)
                    aFuneralParticlesArray[index].x = 0;
            }
        }

        DrawSpriteDefault(&stSceneBuffer,
                          stCutsceneFuneralForeground.x,
                          stCutsceneFuneralForeground.y,
                          pCurrentConversationSpecialShape, 7);
        DrawSpriteDefault(&stSceneBuffer,
                          (short)(stCutsceneFuneralForeground.x + 180),
                          stCutsceneFuneralForeground.y,
                          pCurrentConversationSpecialShape, 6);
        if (bFuneralShowTheEnd != 0)
            print_subtitle(&stSceneBuffer, 56,
                           "THE END");
        RefreshMemoryStatusOverlay();
        DIBslam();
        DIBslamReal();
    }
    return 0;
}

/* Function start: 0x408D50 (Mac symbol: funeral_wingman) */
unsigned int funeral_wingman(char *text, short duration)
{
    AddPCName(text);
    ClearViewport(&stCurrentConvTextViewport,
                  cBlackColour);
    FormatTextBufferFromStart("%X%Y%F%s%P",
                              0, 160,
                              nConvTextColorValue,
                              szTxtWorkspaceBuffer);
    nFrameSkipCounter = 1;
    SetFrameTimerPeriodDirect(duration);
    while ((short)IsFrameTickElapsed() == 0) {
        PumpWindowMessages();
        funeral_player();
        if (bEscapePressedFlag == 1)
            break;
        if (CheckEscaped() != 0)
            break;
    }
    return 0;
}

/* Preserve the funeral resource and scene declaration context. */
void *FetchDiskPacketRetrying(short logicalFile, short section, short flags);
void FreePacketAndClear(void *slot, unsigned short releaseFlags);
unsigned int InitializeConstellationField(Viewport *viewport,
                                          short direction, short density);
void __stdcall FadeViewportPaletteToColour(Viewport *viewport,
                                           unsigned short colour,
                                           short enabled);
unsigned int SceneDirector(short sceneType, unsigned char *sceneData,
                           unsigned char *textData);

/* Function start: 0x408DE0 (Mac symbol: funeral_sequence) */
unsigned int funeral_sequence(int playerFuneral)
{
    int *packet;
    unsigned char *sceneData;
    unsigned char *textData;
    unsigned char *followupSceneData;
    short frame;
    unsigned char *followupTextData;
    short volley;
    short particle;
    short scenePair;

    PreloadMusicTrackHook(0x20);
    nFuneralSequenceActive = 1;
    spacetrack(0x20, 1, 0);
    packet = FetchDiskPacketRetrying(
        asCampaignBriefingFiles[nCampaignDataSet],
        0, 0);
    bFuneralShowTheEnd = 0;

    if (playerFuneral != 0) {
#ifdef SDL_PORT
        /* The original reads zero alignment words for series 0 and 14.
         * Keep those results without indexing outside the native table. */
        if (stCampaignState.currentSeries == 0 ||
            stCampaignState.currentSeries == 14)
            scenePair = 0;
        else
#endif
            scenePair = (short)(
                asFuneralSceneBySeries[
                    stCampaignState.currentSeries - 1] * 2);
        followupSceneData = (unsigned char *)packet + packet[0];
        followupTextData = (unsigned char *)packet + packet[1];
        sceneData = (unsigned char *)packet + packet[scenePair + 2];
        textData = (unsigned char *)packet + packet[scenePair + 3];
        pIntroFont =
            FetchDiskPacketRetrying(9, 1, 0);
    } else {
        followupSceneData = (unsigned char *)packet + packet[10];
        followupTextData = (unsigned char *)packet + packet[11];
        sceneData = (unsigned char *)packet + packet[12];
        textData = (unsigned char *)packet + packet[13];
    }

    InitializeConversationViewport();
    nFuneralCasketPositionX = 180;
    nFuneralCasketPosY = 70;
    stCutsceneFuneralForeground.x = 30;
    nFuneralMainDist = 112;
    nFuneralParticleDistance = 16;
    nCutsceneFuneralGuardFrame = 2;
    nFuneralRifleFrame = 4;
    nCutsceneFuneralBasePosY = 0;
    nCutsceneFuneralBaseX = 0;
    stCutsceneFuneralForeground.y = 0;
    for (particle = 0; particle < 7; particle++) {
        aFuneralParticlesArray[particle].x = 0;
    }

    frame = 0;
    InitializeConversationText();
    init_constellation(0);
    pCurrentConversationSpecialShape =
        FetchDiskPacketRetrying(4, 9, 0);
    ClearViewport(&stSceneBuffer, cPrimaryViewBufferColour);
    InitializeConstellationField(&stSceneBuffer, -1, 16);
    bEscapePressedFlag = 0;
    PumpWindowMessages();
    SceneDirector(3, sceneData, textData);

    if (bEscapePressedFlag != 1) {
        ClearViewport(&stSceneBuffer, cPrimaryViewBufferColour);
        ClearViewport(&stCurrentConvTextViewport,
                      cBlackColour);
        nFrameSkipCounter = 1;
        for (; frame < 10; frame++) {
            PumpWindowMessages();
            funeral_player();
            if (bEscapePressedFlag == 1)
                break;
        }

        if (bEscapePressedFlag != 1) {
            FormatTextBufferFromStart(
                "%X%Y%FCompany...%P", 0, 160,
                cBlueColour);
            nFrameSkipCounter = 1;
            for (frame = 0; frame < 15; frame++) {
                PumpWindowMessages();
                funeral_player();
                if (bEscapePressedFlag == 1)
                    break;
            }
            ClearViewport(&stCurrentConvTextViewport,
                          cBlackColour);

            if (bEscapePressedFlag != 1) {
                frame = 0;
                nFrameSkipCounter = 1;
                FormatTextBufferFromStart(
                    "%X%YAtten-SHUN!%P", 0, 160);
                for (; frame < 10; frame++) {
                    PumpWindowMessages();
                    funeral_player();
                    if (frame == 0)
                        PlaySfxWaveFileByNumber(0x24, -1, 0);
                    if (bEscapePressedFlag == 1)
                        break;
                }

                if (bEscapePressedFlag != 1) {
                    nCutsceneFuneralGuardFrame = 3;
                    frame = 0;
                    nFrameSkipCounter = 1;
                    for (; frame < 10; frame++) {
                        PumpWindowMessages();
                        funeral_player();
                        if (bEscapePressedFlag == 1)
                            break;
                    }

                    if (bEscapePressedFlag != 1) {
                        ClearViewport(
                            &stCurrentConvTextViewport,
                            cBlackColour);
                        nFrameSkipCounter = 1;
                        FormatTextBufferFromStart(
                            "%X%YPrepare arms!%P",
                            0, 160);
                        for (frame = 10; frame != 0; frame--) {
                            PumpWindowMessages();
                            funeral_player();
                        }

                        frame = 0;
                        nFuneralRifleFrame = 5;
                        nFrameSkipCounter = 1;
                        for (; frame < 10; frame++) {
                            PumpWindowMessages();
                            funeral_player();
                            if (frame == 0)
                                PlaySfxWaveFileByNumber(0x1f, -1, 0);
                            if (bEscapePressedFlag == 1)
                                break;
                        }

                        if (bEscapePressedFlag != 1) {
                            SceneDirector(3, followupSceneData,
                                          followupTextData);
                            ClearViewport(&stSceneBuffer,
                                          cPrimaryViewBufferColour);

                            if (bEscapePressedFlag != 1) {
                                for (volley = 0; volley < 3; volley++) {
                                    ClearViewport(
                                        &stCurrentConvTextViewport,
                                        cBlackColour);
                                    FormatTextBufferFromStart(
                                        "%X%Y%FFire!%P",
                                        0, 160, cBlueColour);
                                    if (volley == 1)
                                        PlaySfxWaveFileByNumber(
                                            0x1e, -1, 0);

                                    frame = 0;
                                    nFrameSkipCounter = 1;
                                    for (; frame < 10; frame++) {
                                        PumpWindowMessages();
                                        funeral_player();
                                        if (volley > 0) {
                                            nFuneralCasketPositionX--;
                                            if (nFuneralCasketPositionX %
                                                    2 == 0)
                                                nFuneralCasketPosY--;
                                            nFuneralMainDist++;
                                            if (nFuneralCasketPositionX <
                                                160) {
                                                stCutsceneFuneralForeground.x +=
                                                    2;
                                                nCutsceneFuneralBaseX++;
                                            }
                                        }
                                        if (bEscapePressedFlag == 1)
                                            break;
                                    }

                                    ClearViewport(
                                        &stCurrentConvTextViewport,
                                        cBlackColour);
                                    if (bEscapePressedFlag == 1)
                                        break;

                                    nFuneralParticleDistance = 16;
                                    for (particle = 0;
                                         particle < 7;
                                         particle++) {
                                        aFuneralParticlesArray[particle].x =
                                            (short)(
                                                aFuneralParticleOrigins[
                                                    particle].x +
                                                nCutsceneFuneralBaseX);
                                        aFuneralParticlesArray[particle].y =
                                            (short)(
                                                aFuneralParticleOrigins[
                                                    particle].y +
                                                nCutsceneFuneralBasePosY);
                                    }

                                    frame = 0;
                                    PlaySfxWaveFileByNumber(0x1d, -1, 0);
                                    nFrameSkipCounter = 1;
                                    for (; frame < 24; frame++) {
                                        PumpWindowMessages();
                                        funeral_player();
                                        if (volley > 0) {
                                            nFuneralCasketPositionX--;
                                            if (nFuneralCasketPositionX %
                                                    2 == 0)
                                                nFuneralCasketPosY--;
                                            nFuneralMainDist++;
                                            if (nFuneralCasketPositionX <
                                                160) {
                                                stCutsceneFuneralForeground.x +=
                                                    2;
                                                nCutsceneFuneralBaseX++;
                                            }
                                        }
                                        nFuneralParticleDistance++;
                                        if (bEscapePressedFlag == 1)
                                            break;
                                    }

                                    if (bEscapePressedFlag == 1)
                                        break;
                                }

                                if (bEscapePressedFlag != 1) {
                                    frame = 0;
                                    nFrameSkipCounter = 1;
                                    SetMusBreakpt(0, 0);
                                    while (bEscapePressedFlag == 0) {
                                        PumpWindowMessages();
                                        funeral_player();
                                        nFuneralCasketPositionX--;
                                        if (nFuneralCasketPositionX % 2 ==
                                            0)
                                            nFuneralCasketPosY--;
                                        frame++;
                                        nFuneralMainDist++;
                                        nCutsceneFuneralBaseX++;
                                        stCutsceneFuneralForeground.x += 2;
                                        nFuneralParticleDistance++;
                                        if (frame == 110 &&
                                            playerFuneral != 0)
                                            bFuneralShowTheEnd = 1;

                                        if (nMusicPlaybackMode == 0 ||
                                            nWaitForMusicEnabled == 0 ||
                                            nMusicPlaybackMode == 3) {
                                            if (frame > 160)
                                                break;
                                        } else if (GetMusicMode() != 0) {
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (playerFuneral != 0)
        FreePacketAndClear(&pIntroFont, 0);
    ReleasePacketHandle(packet);
    ReleasePacketHandle(pCurrentConversationSpecialShape);
    free_constellation();
    ReleaseTextFont(0);
    ResetScreenClipToFullHeight();
    FadeViewportPaletteToColour(&stScreenView, cBlackColour, 1);
    ClearViewport(&stScreenView, cBlackColour);
    RestoreGamePalette();
    bEscapePressedFlag = 0;
    ClearInputKeyStatePreservingModifiers();
    FlushInputEvents();
    nFuneralSequenceActive = 0;
    StopMusicUnlessSuppressed();
    free_inflight_music();
    ReleaseMusicTrackHook(0x20);
    return 0;
}

/* Function start: 0x4094E0 */
unsigned int RunAnimationDemoLoop(signed char animation)
{
    switch (animation) {
    case 0:
        init_3Space_objects((short)stCampaignState.currentSeries);
        InitializeCockpitResources(0);
        death_sequence();
        WaitForInputKey();
        break;
    case 1:
        init_3Space_objects((short)stCampaignState.currentSeries);
        InitializeCockpitResources(0);
        LaunchPlayerShip();
        WaitForInputKey();
        break;
    case 2:
        Briefing((short)stCampaignState.currentSeries,
                 (short)stCampaignState.currentMission);
        break;
    case 3:
        PlayScrambleHangarScene();
        break;
    case 4:
        scramble();
        WaitForInputKey();
        break;
    case 5:
        init_3Space_objects((short)stCampaignState.currentSeries);
        ShowCarrierLaunchSequence(1);
        free_constellation();
        free_3Space();
        WaitForInputKey();
        break;
    case 6:
        landing(3);
        WaitForInputKey();
        break;
    case 7:
        Title_Sequence();
        break;
    case 8:
        RecRoom();
        break;
    case 10:
        DeBriefing((short)stCampaignState.currentSeries,
                   (short)stCampaignState.currentMission);
        break;
    case 11:
        funeral_sequence(1);
        break;
    case 12:
        AwardCampaignMedal(nCurrentConversationMedalIndex);
        break;
    case 13:
        BarracksScreen();
        break;
    }

    switch (animation) {
    case 14:
        landing((signed char)(animation - 14));
        WaitForInputKey();
        break;
    case 15:
        Office();
        break;
    case 16:
        ShowCampaignVictorySequence();
        break;
    case 17:
        ShowTigerClawEscapeScene();
        break;
    case 18:
        ShowMeanwhileTransition(0, 0);
        WaitForInputKey();
        ShowMeanwhileTransition(0, 1);
        WaitForInputKey();
        break;
    case 19:
        ShowMeanwhileTransition(1, 0);
        WaitForInputKey();
        ShowMeanwhileTransition(1, 1);
        WaitForInputKey();
        break;
    case 20:
        ShowMeanwhileTransition(2, 0);
        WaitForInputKey();
        ShowMeanwhileTransition(2, 1);
        WaitForInputKey();
        break;
    case 21:
        ShowMeanwhileTransition(3, 0);
        WaitForInputKey();
        ShowMeanwhileTransition(3, 1);
        WaitForInputKey();
        break;
    case 22:
        funeral_sequence(0);
        break;
    case 23:
        ShowMeanwhileTransition(4, 1);
        WaitForInputKey();
        break;
    case 24:
        ShowMeanwhileTransition(5, 0);
        WaitForInputKey();
        ShowMeanwhileTransition(5, 1);
        WaitForInputKey();
        break;
    case 25:
        ShowMeanwhileTransition(6, 0);
        WaitForInputKey();
        break;
    }
    exit_squadron("Animation demo over.");
    return 0;
}
