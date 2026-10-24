/* Carrier departure and cockpit launch sequences.
 * Address range 42b9a0-42c40f. The following installation helpers remain
 * a provisional internal boundary; see docs/ORDER.md.
 */
#define WC1_LAUNCH_SOURCE
#include "wc1.h"

/* 0x0046a550 */
static short asCarrierLaunchApproachDeltaY[24] = {
    -1, -1, -1, -2, -2, -2, -2, -3,
    -3, -3, -3, -3, -2, -2, -2, -2,
    -1, -1, -1, -1, -1, 0, 0, 0
};
/* 0x0046a580 */
static signed char acCarrierLaunchApproachFrames[24] = {
    36, 32, 32, 32, 32, 32, 32, 32,
    25, 25, 25, 25, 25, 25, 25, 25,
    18, 18, 18, 18, 18, 18, 18, 18
};
/* 0x0046a598 */
static ShortPoint aCarrierLaunchFighterPath[9] = {
    { -2, 1 }, { -1, 1 }, { -1, 1 },
    { -1, 0 }, { -1, 1 }, { -1, 0 },
    { -1, 1 }, { -1, 0 }, { 0, 0 }
};
static short asCarrierLaunchFighterDeltaY[14] = {
    3, 3, 3, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1
}; /* 0x0046a5c0 */

static short asCarrierLaunchViewData[14] = {
    12, 11, 1, 90, 90, 0, 9, -90, 0, 0, 20, 3, 15, -1
}; /* 0x0046a5e0 */


/* Linker commons defined here; LINK orders them by identifier hash. */
Viewport *pConstellationViewportPtr; /* 0x005a6aac */
short nViewConstellationStarCount; /* 0x005a6ab0 */
ConstellationParticle aConstellationParticlesList[16]; /* 0x005a6ac0 */
Viewport stConstellationViewport; /* 0x005a6b40 */
short nConstellationParticleTotal; /* 0x005a6b54 */
const short *pCurrentViewScriptPtr; /* 0x005a6b58 */
short nCurrentExternalViewDistance; /* 0x005a6b5c */
Viewport stPilotHandView; /* 0x005a6b60 */
Viewport stLeftVdu; /* 0x005a6b80 */
ScreenViewportPacket *pCurrentScreenViewportPkt; /* 0x005a6b94 */
Viewport stScreenView; /* 0x005a6ba0 */
TextContext stHudMessageTextContext; /* 0x005a6bc0 */
Viewport stCockpitBar; /* 0x005a6be0 */

/* Function start: 0x42B9A0 */
void DrawLaunchDoorFrame(short distance)
{
    short bounds[4];
    short scale;

    if (distance > 10) {
        scale = (short)(0x1a00L / distance);
        GetTransformedShapeBounds(
            &stSpaceBufferViewport,
            (short)((short)nScreenWidth >> 1),
            (short)((short)nScreenHeight >> 1),
            pLaunchDoorShape, 1, 0, scale, 0, bounds);
#ifdef SDL_PORT
        if (!SdlRecordSpaceSprite(
                &stSpaceBufferViewport, (short)(bounds[0] - 1),
                (short)((short)nScreenHeight >> 1),
                pLaunchDoorShape, 0, 0, scale, 0))
#endif
        DrawSpriteScaled(
            &stSpaceBufferViewport, (short)(bounds[0] - 1),
            (short)((short)nScreenHeight >> 1),
            pLaunchDoorShape, 0, 0, scale, 0);
#ifdef SDL_PORT
        if (!SdlRecordSpaceSprite(
                &stSpaceBufferViewport,
                (short)((short)nScreenWidth >> 1),
                (short)((short)nScreenHeight >> 1),
                pLaunchDoorShape, 1, 0, scale, 0))
#endif
        DrawSpriteScaled(
            &stSpaceBufferViewport,
            (short)((short)nScreenWidth >> 1),
            (short)((short)nScreenHeight >> 1),
            pLaunchDoorShape, 1, 0, scale, 0);
#ifdef SDL_PORT
        if (!SdlRecordSpaceSprite(
                &stSpaceBufferViewport, bounds[2],
                (short)((short)nScreenHeight >> 1),
                pLaunchDoorShape, 2, 0, scale, 0))
#endif
        DrawSpriteScaled(
            &stSpaceBufferViewport, bounds[2],
            (short)((short)nScreenHeight >> 1),
            pLaunchDoorShape, 2, 0, scale, 0);
    }
}

/* Function start: 0x42BA90 */
void LaunchPlayerShip(void)
{
    short doorDistances[4];
    signed char distanceStep;
    signed char frame;
    signed char door;

    doorDistances[0] = 50;
    doorDistances[1] = 40;
    doorDistances[2] = 30;
    doorDistances[3] = 20;
    distanceStep = 1;

    spacetrack(changetrack(), 1, 0);
    if (bEscapePressedFlag == 0) {
        pLaunchDoorShape =
            (unsigned char *)FetchDiskPacketRetrying(1, 7, 0);
        nCannedSceneMode = 1;
        force_view(0, 0);
        PlaySfxWaveFileByNumber(20, -1, 0);
        nFrameSkipCounter = 1;
        bEscapePressedFlag = 0;
        frame = 0;
        do {
            PumpWindowMessages();
            if (RefreshCockpitStatus() != 0) {
                door = 0;
                do {
                    DrawLaunchDoorFrame(doorDistances[door]);
                    doorDistances[door] =
                        (short)(doorDistances[door] - distanceStep);
                    door++;
                } while (door < 4);
                dump_buffer_to_screen();
                update_cockpit();
            }
            DIBslam();
            DIBslamReal();
            if (bEscapePressedFlag == 1)
                break;
            if (frame % 5 == 0)
                distanceStep++;
            frame++;
        } while (frame < 25);

        if (bEscapePressedFlag != 0) {
            StopMusicUnlessSuppressed();
            spacetrack(changetrack(), 1, 0);
        }
        ReleasePacketHandle(pLaunchDoorShape);
    } else {
        force_view(0, 0);
    }

    DIBslam();
    DIBslamReal();
    clear_view_buffer();
    nCannedSceneMode = 0;
    ResetSoundState();
    bEscapePressedFlag = 0;
}

/* Function start: 0x42BC00 */
unsigned int ShowCarrierLaunchSequence(signed char sceneObject)
{
    unsigned char *carrierShape;
    unsigned char *actorShape;
    unsigned char *fighterShape;
    short fighterScreenY;
    short approachScale;
    short approachDistance;
    short fighterScreenX;
    short carrierScreenX;
    short actorX;
    short object;
    short frame;

    bIntroSceneResourcesActive = 0;
    carrierScreenX = 180;
    free_ship(1);
    free_ship(2);
    free_ship(3);
    remove_nav_point_objects();
    ResetSoundState();
    PreloadMusicTrackHook(0x1c);
    spacetrack(0x1c, 2, 1);
    carrierShape =
        (unsigned char *)FetchDiskPacketRetrying(1, 8, 0);
    actorShape =
        (unsigned char *)FetchDiskPacketRetrying(1, 4, 0);
    pScrambleView = &stSpaceBufferViewport;
    object = (short)sceneObject;
    fighterScreenY = 64;
    fighterShape = aObjectTypeData[
        stCampaignState.playerShipType].shapeSet;
    nScriptedViewObject = object;
    fighterScreenX = 20;
    initialize_scripted_view(asCarrierLaunchViewData);
    nScrambleBackdropPositionY = 64;
    nScrambleBackgroundRightCoordX = 520;
    asObjectFlipArray[0] = 0;
    asObjectViewFrames[0] = 36;
    asObjScreenAngles[0] = 180;
    asObjDistancesTable[0] = 300;
    apObjShapeArray[0] = fighterShape;
    apObjShapeArray[object] = carrierShape;
    asObjectFlipArray[object] = 0;
    asObjectViewFrames[object] = 3;
    bEscapePressedFlag = 0;
    asObjScreenAngles[object] = 0;
    asObjScreenScalesArray[object] = 0x100;
    asObjDistancesTable[object] = 2000;
    nFrameSkipCounter = 1;
    frame = 0;
    approachDistance = 20;
    do {
        PumpWindowMessages();
        aeObjectClass[0] = OBJECT_CLASS_NULL;
        aeObjectClass[object] = OBJECT_CLASS_NULL;
        set_eye_direction_and_position();
        nFrameSkipCounter--;
        if (nFrameSkipCounter < 1) {
            nFrameSkipCounter = nFrameSkip;
            nRenderedSpaceFrame++;
            UpdateSpacePaletteFade();
            clear_view_buffer();
            house_keep_objects();
            update_objects_in_space();
            transform_objects_to_your_view();
            update_star_field();
            if (frame < 24) {
                fighterScreenY = (short)(fighterScreenY +
                    asCarrierLaunchApproachDeltaY[frame]);
                asObjectViewFrames[0] =
                    acCarrierLaunchApproachFrames[frame];
            } else if (frame < 48) {
                fighterScreenY = (short)(fighterScreenY -
                    asCarrierLaunchApproachDeltaY[47 - frame]);
                asObjectViewFrames[0] =
                    acCarrierLaunchApproachFrames[47 - frame];
            }
            aeObjectClass[0] = OBJECT_CLASS_SHIP;
            approachScale = (short)(
                ((int)awObjectScales[0]
                    << 4) / approachDistance);
            aeObjectClass[object] = OBJECT_CLASS_SHIP;
            asObjectScreenXList[0] =
                (short)(fighterScreenX - nCurrentViewCenterPosX);
            asObjectScreenYArray[0] =
                (short)(fighterScreenY - nCurrentViewCenterPosY);
            asObjScreenScalesArray[0] = approachScale;
            asObjectScreenXList[object] =
                (short)(nScrambleBackgroundRightCoordX -
                        nCurrentViewCenterPosX);
            asObjectScreenYArray[object] =
                (short)(nScrambleBackdropPositionY -
                        nCurrentViewCenterPosY);
            sort_object_depth();
#ifdef SDL_PORT
            SdlBeginSpaceFrame(
                pScreenViewGeometry,
                (int)cScreenViewMode,
                bCockpitlessView > 0,
                (unsigned char)cPrimaryViewBufferColour);
#endif
            draw_sorted_objects_to_buffer();
            dump_buffer_to_screen();
        }
        fighterScreenX = (short)(fighterScreenX + 2);
        nCurrentSpaceFrameValue++;
        nScrambleBackgroundRightCoordX =
            (short)(nScrambleBackgroundRightCoordX - 2);
        approachDistance = (short)(approachDistance + 2);
        asObjDistancesTable[0] =
            (short)(asObjDistancesTable[0] + 10);
        AddFixedVectors(&aShipPositionsArray[61],
                        &aShipVelocity[61],
                        &aShipPositionsArray[61]);
        if (bEscapePressedFlag == 1)
            break;
        frame++;
        DIBslam();
        DIBslamReal();
    } while (frame < 100);

    aeObjectClass[0] = OBJECT_CLASS_NULL;
    aeObjectClass[object] = OBJECT_CLASS_NULL;
    if (bEscapePressedFlag != 1) {
        short actorY;

        copy_frame(object, 61);
        aShipPositionsArray[61] = aShipPositionsArray[object];
        nScrambleBackgroundRightCoordX = 0;
        fighterScreenX = 200;
        fighterScreenY = 32;
        nScrambleBackdropPositionY = 0;
        asObjectCollisionRadius[61] =
            asObjectCollisionRadius[object];
        approachDistance = 100;
        ConfigureScrambleActor(100, 80, 1, 0, actorShape,
                               0x100, 0, 0, 0);
        ConfigureScrambleActor(116, 130, 0, 0, actorShape,
                               0x100, 0, 0, 1);
        ConfigureScrambleActor(300, 110, -4, 0, actorShape,
                               0xc0, 0, 0x10, 3);
        ConfigureScrambleActor(301, 110, -4, 0, actorShape,
                               0xc0, 0, 0x10, 4);
        PlaySfxWaveFileByNumber(18, -1, 0);
        frame = 0;
        actorX = 60;
        actorY = 10;
        nFrameSkipCounter = 1;
        do {
            PumpWindowMessages();
            alter_yaw(-1, 61);
            if (RefreshCockpitStatus() != 0) {
                DrawSpriteDefault(
                    &stSpaceBufferViewport,
                    (short)(nScrambleBackgroundRightCoordX + 239),
                    nScrambleBackdropPositionY,
                    carrierShape, 0);
                DrawSpriteDefault(
                    &stSpaceBufferViewport,
                    (short)(nScrambleBackgroundRightCoordX + 240),
                    nScrambleBackdropPositionY,
                    carrierShape, 1);
                DrawScrambleActor(0);
                approachScale = (short)(0x6000L / approachDistance);
                DrawSpriteScaled(&stSpaceBufferViewport, fighterScreenX,
                                 fighterScreenY, fighterShape, 16, 0,
                                 approachScale, 0);
                DrawScrambleActor(3);
                DrawScrambleActor(4);
                DrawScrambleActor(1);
                DrawSpriteDefault(
                    &stSpaceBufferViewport,
                    (short)(nScrambleBackgroundRightCoordX + 60),
                    (short)(nScrambleBackdropPositionY + 10),
                    actorShape, 16);
                DrawSpriteDefault(
                    &stSpaceBufferViewport,
                    (short)(nScrambleBackgroundRightCoordX + 80),
                    (short)(nScrambleBackdropPositionY + 134),
                    actorShape, 8);
                DrawSpriteDefault(&stSpaceBufferViewport, carrierScreenX,
                                  nScrambleBackdropPositionY,
                                  carrierShape, 2);
                WaitForVerticalBlankThunk();
                dump_buffer_to_screen();
            }
            nScrambleBackgroundRightCoordX =
                (short)(nScrambleBackgroundRightCoordX + 2);
            carrierScreenX = (short)(carrierScreenX + 4);
            fighterScreenX = (short)(fighterScreenX - 2);
            fighterScreenY++;
            approachDistance--;
            if (sceneObject != 0) {
                ((void (__cdecl *)(int, int, short, int))PaletteFadeHook)(
                    (int)sceneObject, 10, (short)(frame * 4), 0);
            }
            if (bEscapePressedFlag == 1)
                break;
            DIBslam();
            DIBslamReal();
            frame++;
        } while (frame < 35);

        if (bEscapePressedFlag != 1) {
            nFrameSkipCounter = 1;
            frame = 0;
            while (frame < 50) {
                PumpWindowMessages();
                if (RefreshCockpitStatus() != 0) {
                    DrawSpriteDefault(
                        &stSpaceBufferViewport,
                        (short)(nScrambleBackgroundRightCoordX + 239),
                        nScrambleBackdropPositionY,
                        carrierShape, 0);
                    DrawSpriteDefault(
                        &stSpaceBufferViewport,
                        (short)(nScrambleBackgroundRightCoordX + 240),
                        nScrambleBackdropPositionY,
                        carrierShape, 1);
                    DrawScrambleActor(0);
                    DrawSpriteScaled(&stSpaceBufferViewport, fighterScreenX,
                                     fighterScreenY, fighterShape, 16, 0,
                                     approachScale, 0);
                    DrawScrambleActor(3);
                    DrawScrambleActor(4);
                    DrawScrambleActor(1);
                    DrawSpriteDefault(
                        &stSpaceBufferViewport,
                        (short)(nScrambleBackgroundRightCoordX + actorX),
                        (short)(nScrambleBackdropPositionY + actorY),
                        actorShape, 16);
                    DrawSpriteDefault(
                        &stSpaceBufferViewport,
                        (short)(nScrambleBackgroundRightCoordX + 80),
                        (short)(nScrambleBackdropPositionY + 134),
                        actorShape, 8);
                    DrawSpriteDefault(&stSpaceBufferViewport, carrierScreenX,
                                      nScrambleBackdropPositionY,
                                      carrierShape, 2);
                    WaitForVerticalBlankThunk();
                    dump_buffer_to_screen();
                }
                frame++;
                if (frame < 9) {
                    fighterScreenX = (short)(fighterScreenX +
                        aCarrierLaunchFighterPath[frame].x);
                    fighterScreenY = (short)(fighterScreenY +
                        aCarrierLaunchFighterPath[frame].y);
                } else if (frame < 23) {
                    if (frame == 9)
                        PlaySfxWaveFileByNumber(11, -1, 0);
                    fighterScreenY = (short)(fighterScreenY +
                        asCarrierLaunchFighterDeltaY[frame - 9]);
                }
                if (frame == 23) {
                    FlushSoundEffectsAndLog((int)sceneObject, 0);
                    PlaySfxWaveFileByNumber(19, -1, 0);
                }
                actorX++;
                if (frame % 7 == 0)
                    actorY--;
                DIBslam();
                DIBslamReal();
                if (bEscapePressedFlag == 1)
                    break;
            }
        }
    }

    bEscapePressedFlag = 0;
    ResetSoundState();
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook(0x1c);
    free_ship(0);
    ReleasePacketHandle(carrierShape);
    ReleasePacketHandle(actorShape);
    bScriptedView = 0;
    bIntroSceneResourcesActive = 1;
    return 0;
}
