/*
 *  Campaign victory, retreat, and ending scenes.
 *
 *  Address range 42fa90-43070f (see docs/ORDER.md).
 *  The adjacent tagged-allocation helpers remain in this provisional unit.
 */
#define WC1_ENDING_SOURCE
#include "wc1.h"
#include <sys/types.h>

/* 0x0046ad88 */
static unsigned char abTaggedAllocationPrefix[8] = {
    'j', 'e', 'f', 'f', 0, 0, 0, 0
};
/* Linker commons defined here; LINK orders them by identifier hash. */
signed char acShipStressList[16]; /* 0x0059d620 */
short anRollGoal[16]; /* 0x0059d630 */
enum Side aeShipSide[12]; /* 0x0059d650 */
signed char acShipAiCooldowns[16]; /* 0x0059d680 */
FixedVector vCollisionDelta; /* 0x0059d690 */
signed char acLastCollisionObjects[SPACE_OBJECT_COUNT]; /* 0x0059d6a0 */
short aasShipMaximumShieldTable[10][2]; /* 0x0059d6e0 */
short asObjectCollisionRadius[64]; /* 0x0059d710 */
signed char acShipPointingModeList[16]; /* 0x0059d790 */

/* Function start: 0x42FA90 */
void *AllocateTaggedMemory(unsigned int size, unsigned short flags)
{
    unsigned int *memory;
    unsigned short tagged;

    tagged = flags & 0x40;
#ifdef SDL_PORT
    if (tagged != 0)
        size += 8 + sizeof(unsigned char *);
#else
    if (tagged != 0)
        size += 8;
#endif
    memory = AllocateGuardedMemory(size);
    if (tagged != 0) {
#ifdef SDL_PORT
        *(unsigned char **)memory = 0;
        memcpy((unsigned char *)memory + sizeof(unsigned char *),
               abTaggedAllocationPrefix,
               sizeof(abTaggedAllocationPrefix));
        memory = PushMemoryStackFrame(
            memory, -(int)(8 + sizeof(unsigned char *)));
#else
        memcpy(memory, abTaggedAllocationPrefix,
               sizeof(abTaggedAllocationPrefix));
        memory = PushMemoryStackFrame(memory, -8);
#endif
    }
    return memory;
}

/* Function start: 0x42FAE0 */
void ReleasePacketHandle(void *handle)
{
    void **reference;
    int group;
    int i;

    reference = (void **)aapPacketReferences;
    for (group = 0; group < 4; group++) {
        for (i = 0; i < 0x25; i++) {
            if (*reference == handle)
                *reference = 0;
            reference++;
        }
    }
    FreeGuardedAllocation(MapPacketHandleToBlock(handle));
}

/* Function start: 0x42FB20 */
/* Tail-jump thunk to 0x004362E0, not a constant return -- Ghidra followed the
 * jump and folded the callee's body into the display.  The optimized compiler
 * emits the tail jump from this forwarding return. */
unsigned int GetFixedOneMillionThunk(short memoryType)
{
    (void)memoryType;
    return GetFixedOneMillion();
}

/* Function start: 0x42FB30 */
unsigned int GetFixedOneMillionThunkAlt(short memoryType)
{
    (void)memoryType;
    return GetFixedOneMillionAlt();
}

/* Function start: 0x42FB40 */
void CreateCannedSceneObject(short *object, short yaw, short unusedPitch,
                             short distance, unsigned char *shape,
                             short frame, short type, short scale)
{
    (void)unusedPitch;
    *object = find_vacant_3d_object();
    if (*object != -1) {
        aeObjectClass[*object] = OBJECT_CLASS_PLANET;
        init_ijk(63);
        alter_yaw(yaw, 63);
        ScaleFixedVector(&aShipFwdVec[63],
                         (int)distance << 8,
                         &aShipPositionsArray[*object]);
        asObjectViewFrames[*object] = frame;
        asObjScreenAngles[*object] = type;
        aeObjectTypeList[*object] =
            (enum ObjectType)asObjScreenAngles[*object];
        asObjScreenScalesArray[*object] = scale;
        apObjShapeArray[*object] = shape;
    }
}

/* 0x0046ad90 */
static const char *apszCampaignVictoryText[4] = {
    "Destroying the remains of the Kilrathi naval power in the sector...",
    "The Tiger's Claw closes in for the kill...",
    "And the last Kilrathi planet in the sector falls!",
    0
};
/* 0x0046ada0 */
static const char *pszTigerClawEscapeOpening =
    "Fleeing from the overwelming Kilrathi forces in the sector...";
/* 0x0046ada4 */
static const char *pszTigerClawEscapeJump =
    "The Tiger's Claw manages to jump out. Barely.";
/* 0x0046ada8 */
static const char *pszTigerClawEscapeClosing =
    "There'll be other sectors, other battles...";
/* 0x0046adb0 */
static ShortVector aCampaignVictoryProjectileOrigins[4] = {
    { -55, 42, 0 }, { -68, 46, 0 },
    { 60, 42, 16 }, { 73, 46, 16 }
};
static const char *pszTheEnd = "The End"; /* 0x0046adc8 */
static const char *pszForNow = "For Now..."; /* 0x0046adcc */

/* Function start: 0x42FC00 */
unsigned int ShowCampaignVictorySequence(void)
{
    short slot;
    CampaignVictoryProjectile projectiles[16];
    unsigned char *planetShape;
    unsigned char *projectileShape;
    const ShortVector *origin;
    short planetObject;
    short spawnCountdown;
    short vacant[2];
    short vacantCount;
    short textIndex;
    CampaignVictoryProjectile *projectile;
    short frame;
    short elapsed;
    short animationFrame;
    int planetScale;
    int verticalOffset;
    int planetDepth;
    int projectedY;

    PreloadMusicTrackHook(0x21);
    spacetrack(0x21, 2, 1);
    InitializeConversationText();
    init_3Space_objects(0);
    nCannedSceneMode = 2;
    bIntroSceneResourcesActive = 0;
    set_up_action_sphere(0x12);
    planetShape =
        FetchDiskPacketRetrying(9, 3, 0);
    projectileShape =
        FetchDiskPacketRetrying(9, 2, 0);
    CreateCannedSceneObject(&planetObject, -4, 0, 30000,
                            planetShape, 0, 0, 0x50);
    nScriptedViewObject = 1;
    initialize_scripted_view(asCampaignVictoryViewScript);
    {
        short initializationCount;

        initializationCount = 16;
        projectile = projectiles;
        do {
            projectile->scale = -1;
            projectile++;
            initializationCount--;
        } while (initializationCount != 0);
    }

    planetDepth = -1500;
    frame = 0;
    bEscapePressedFlag = 0;
    verticalOffset = -70000;
    nFrameSkipCounter = 1;
    do {
        switch (frame) {
        case 0:
            textIndex = 0;
            break;
        case 100:
            textIndex = 1;
            break;
        case 180:
            textIndex = 2;
            break;
        default:
            textIndex = -1;
            break;
        }
        if (textIndex != -1) {
            ClearViewport(&stCurrentConvTextViewport,
                          cBlackColour);
            SetTextContext(&stCurrentConvTxtCtx);
            FormatTextBufferFromStart(
                "%X%Y%s%P", 0, 160,
                apszCampaignVictoryText[textIndex]);
        }

        Update_3Space();
        if (Draw_3Space_Frame() != 0) {
            if (frame > 90)
                asObjScreenScalesArray[planetObject]++;
            if (asObjectCollisionRadius[61] < planetDepth) {
                slot = 0;
                projectile = projectiles;
                do {
                    if (projectile->scale != -1 &&
                        asObjectCollisionRadius[61] <
                            projectile->depth) {
                        projectile->screenX =
                            (short)(projectile->x / projectile->depth);
                        projectile->screenY =
                            (short)(projectile->y / projectile->depth);
                        projectile->scale = 0x10000L / projectile->depth;
                        if (projectile->scale < 16) {
                            projectile->scale = -1;
                        } else {
                            DrawSpriteScaled(
                                &stSpaceBufferViewport,
                                (short)(projectile->screenX +
                                        nCurrentViewCenterPosX),
                                (short)(projectile->screenY +
                                        nCurrentViewCenterPosY),
                                projectileShape, 1, 0,
                                (short)projectile->scale,
                                projectile->flip);
                            projectile->depth += 100;
                            projectile->y += 4000;
                        }
                    }
                    projectile++;
                    slot++;
                } while (slot < 16);

                if (frame < 170 && --spawnCountdown < 1) {
                    projectile = projectiles;
                    vacantCount = 0;
                    slot = 0;
                    do {
                        if (projectile->scale == -1) {
                            vacant[vacantCount] = slot;
                            vacantCount++;
                            if (vacantCount == 2)
                                break;
                        }
                        slot++;
                        projectile++;
                    } while (slot < 16);

                    if (vacantCount > 1) {
                        origin =
                            &aCampaignVictoryProjectileOrigins[
                                RandomBelowOrEqual(3)];
                        projectile = &projectiles[vacant[0]];
                        projectile->depth = planetDepth;
                        projectile->x =
                            ((origin->x * planetScale) >> 8) * planetDepth;
                        projectile->y =
                            ((origin->y * planetScale) >> 8) *
                                projectile->depth + verticalOffset;
                        projectile->scale = 0x100;
                        projectile->flip = origin->z;
                        projectile->depth += 40;

                        projectile = &projectiles[vacant[1]];
                        projectile->depth = planetDepth;
                        projectile->x =
                            (((origin->x - 4) * planetScale) >> 8) *
                                planetDepth;
                        projectile->y =
                            ((origin->y * planetScale) >> 8) *
                                projectile->depth + verticalOffset;
                        projectile->scale = 0x100;
                        projectile->flip = origin->z;
                        projectile->depth += 40;
                    }
                    spawnCountdown = 8;
                }

                planetScale = 0x40000L / planetDepth;
                projectedY = verticalOffset / planetDepth;
                DrawSpriteScaled(
                    &stSpaceBufferViewport, nCurrentViewCenterPosX,
                    (short)(nCurrentViewCenterPosY + projectedY),
                    projectileShape, 0, 0, (short)planetScale, 0);
                verticalOffset += 200;
            }
            dump_buffer_to_screen();
            clear_view_buffer();
            DIBslam();
            DIBslamReal();
        }
        planetDepth += 15;
        if (bEscapePressedFlag == 1)
            break;
        DIBslam();
        DIBslamReal();
        frame++;
    } while (frame < 250);

    ReleasePacketHandle(projectileShape);
    ReleasePacketHandle(planetShape);
    free_all_slots();
    ReleaseTextFont(0);
    free_3Space();
    if (bEscapePressedFlag != 1) {
        planetShape =
            FetchDiskPacketRetrying(9, 5, 0);
        ClearViewport(&stScreenView, cBlackColour);
        animationFrame = 1;
        WaitForVerticalBlankThunk();
        DrawSpriteDefault(&stScreenView, 0, 0, planetShape, 0);
        WaitForSceneAdvance(14, 0);
        elapsed = 0;
        do {
            SetFrameTimerPeriodDirect(8);
            DrawSpriteDefault(&stScreenView, 0, 0, planetShape,
                              animationFrame++);
            if (animationFrame > 17)
                animationFrame = 12;
            while ((short)IsFrameTickElapsed() == 0) {
                if (bEscapePressedFlag != 0 || CheckEscaped() != 0) {
                    elapsed = 1000;
                    break;
                }
            }
            elapsed++;
            DIBslam();
            DIBslamReal();
        } while (elapsed < 40);
        ReleasePacketHandle(planetShape);
        FadeViewportPaletteToColour(&stModalSrcViewport,
                                    cBlackColour, 1);
        ClearViewport(&stModalSrcViewport,
                      cBlackColour);
        DIBslam();
        DIBslamReal();
        RestoreGamePalette();
    }

    bEscapePressedFlag = 0;
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook(0x21);
    bScriptedView = 0;
    bIntroSceneResourcesActive = 1;
    nCannedSceneMode = 0;
    FadeViewportPaletteToColour(&stModalSrcViewport,
                                cBlackColour, 1);
    ClearViewport(&stModalSrcViewport,
                  cBlackColour);
    DIBslam();
    DIBslamReal();
    RestoreGamePalette();
    return 0;
}

/* Function start: 0x430150 */
unsigned int ShowTigerClawEscapeScene(void)
{
    unsigned char *escapeShape;
    FixedVector jumpOffset;
    short approachStep;
    short frame;
    short effect;
    int verticalOffset;
    int depth;
    int scale;
    int projectedY;

    approachStep = 15;
    PreloadMusicTrackHook(0x22);
    spacetrack(0x22, 2, 1);
    init_3Space_objects((short)stCampaignState.currentSeries);
    nCannedSceneMode = 2;
    bIntroSceneResourcesActive = 0;
    InitializeConversationText();
    set_up_action_sphere(0x13);
    escapeShape =
        FetchDiskPacketRetrying(9, 2, 0);
    if (aObjectTypeData[
            OBJECT_TYPE_HYPERSPACE_JUMP_FLASH].shapeSet == 0) {
        aObjectTypeData[
            OBJECT_TYPE_HYPERSPACE_JUMP_FLASH].shapeSet =
                FetchDiskPacketRetrying(3, 14, 0);
    }
    nScriptedViewObject = 1;
    initialize_scripted_view(asTigerClawEscapeViewScript);
    ClearViewport(&stCurrentConvTextViewport,
                  cBlackColour);
    SetTextContext(&stCurrentConvTxtCtx);
    FormatTextBufferFromStart(
        "%X%Y%s%P", 0, 160,
        pszTigerClawEscapeOpening);
    depth = -1000;
    verticalOffset = -70000;
    nFrameSkipCounter = 1;
    bEscapePressedFlag = 0;
    frame = 0;
    do {
        Update_3Space();
        if (Draw_3Space_Frame() != 0) {
            if (asObjectCollisionRadius[61] < depth &&
                frame < 198) {
                scale = 0x40000L / depth;
                projectedY = verticalOffset / depth;
                DrawSpriteScaled(
                    &stSpaceBufferViewport, nCurrentViewCenterPosX,
                    (short)(nCurrentViewCenterPosY + projectedY),
                    escapeShape, 0, 0, (short)scale, 0);
            }
            dump_buffer_to_screen();
            clear_view_buffer();
        }
        if (asObjectCollisionRadius[61] < depth)
            verticalOffset += 400;
        depth += approachStep;
        if (frame > 170)
            approachStep = (short)(approachStep + 10);

        switch (frame) {
        case 150:
            ClearViewport(&stCurrentConvTextViewport,
                          cBlackColour);
            SetTextContext(&stCurrentConvTxtCtx);
            FormatTextBufferFromStart(
                "%X%Y%s%P", 0, 160,
                pszTigerClawEscapeJump);
            break;
        case 190:
            effect = find_vacant_3d_object();
            if (effect != -1) {
                set_objects_data(effect,
                                 OBJECT_TYPE_HYPERSPACE_JUMP_FLASH,
                                 -1);
                ScaleFixedVector(&aShipFwdVec[61],
                                 0x271000, &jumpOffset);
                awObjectScales[effect] =
                    (short)(awObjectScales[effect] << 2);
                zero_vector(&aShipVelocity[effect]);
                AddFixedVectors(&aShipPositionsArray[61],
                                &jumpOffset,
                                &aShipPositionsArray[effect]);
            }
            break;
        case 198:
            ClearViewport(&stSpaceBufferViewport,
                          cViewportClearColour);
            bViewportDirty = 1;
            break;
        case 210:
            ClearViewport(&stCurrentConvTextViewport,
                          cBlackColour);
            SetTextContext(&stCurrentConvTxtCtx);
            FormatTextBufferFromStart(
                "%X%Y%s%P", 0, 160,
                pszTigerClawEscapeClosing);
            break;
        }
        if (bEscapePressedFlag == 1)
            break;
        frame++;
        DIBslam();
        DIBslamReal();
    } while (frame < 260);

    ReleasePacketHandle(aObjectTypeData[
        OBJECT_TYPE_HYPERSPACE_JUMP_FLASH].shapeSet);
    aObjectTypeData[
        OBJECT_TYPE_HYPERSPACE_JUMP_FLASH].shapeSet = 0;
    ReleasePacketHandle(escapeShape);
    free_all_slots();
    ReleaseTextFont(0);
    free_3Space();
    ClearViewport(&stCurrentConvTextViewport,
                  cBlackColour);
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook(0x22);
    bScriptedView = 0;
    nCannedSceneMode = 0;
    bIntroSceneResourcesActive = 1;
    return 0;
}

/* Function start: 0x4304F0 */
unsigned int ShowTheEndScreen(short enableFireworks)
{
    short activeFireworks;
    short frame;
    short slot;

    activeFireworks = 0;
    SetEventManagerPump(get_player_input);
    PreloadMusicTrackHook(0x17);
    spacetrack(0x17, 2, 1);
    InitializeConversationViewport();
    ViewMedals();
    ReleaseTextFont(0);
    ClearViewport(&stModalSrcViewport, cBlackColour);
    ClearViewport(&stSceneBuffer, cBlackColour);
    InitializeFireworks();
    pFireworkShape =
        FetchDiskPacketRetrying(9, 0x11, 0);
    pIntroFont =
        FetchDiskPacketRetrying(9, 1, 0);
    print_subtitle(&stSceneBuffer, 0x3a, pszTheEnd);
    PanToScreen(&stSceneBuffer, &stScreenView);
    bEscapePressedFlag = 0;
    frame = 0;
    nFrameSkipCounter = 1;
    do {
        ClearViewport(&stSceneBuffer, cBlackColour);
        if (enableFireworks != 0 && activeFireworks != 0 &&
            (RandomBelowOrEqual(100) < 40 || frame > 280)) {
            slot = 0;
            do {
                if (aFireworksList[slot].frame == -1) {
                    aFireworksList[slot].frame = 0;
                    aFireworksList[slot].x =
                        RandomInRange(0, stSpaceBufferViewport.right);
                    aFireworksList[slot].y =
                        RandomInRange(0, stSpaceBufferViewport.bottom);
                    aFireworksList[slot].variant = RandomInRange(0, 2);
                    break;
                }
                slot++;
            } while (slot < 30);
        }
        activeFireworks = TheEndFireWorks(&stSceneBuffer, 30);
        if (frame < 160) {
            print_subtitle(&stSceneBuffer, 0x3a,
                           pszTheEnd);
        } else if (frame > 190) {
            StopMusic((short)(320 - frame));
            print_subtitle(&stSceneBuffer, 0x3a,
                           pszForNow);
        }
        frame++;
        RefreshMemoryStatusOverlay();
        DIBslam();
        DIBslamReal();
    } while (frame < 320);

    ReleasePacketHandle(pFireworkShape);
    ReleasePacketHandle(pIntroFont);
    ResetScreenClipToFullHeight();
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook(0x17);
    return 0;
}
