/*
 *  Screen scopes, prompts and the comm menu.
 *
 *  Address range 0x42efc0-0x431fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: PushMemoryStackFrame/ShowChoosePrompt/ShowEnemyTargetSelectMenu.
 */
#include "wc1.h"

/* Function start: 0x42EFC0 */
void cleanup_objectives(void)
{
    int objectiveType;
    short objective;
    short home;
    short proximity;
    short index;
    short object;

    objective = 0;
    home = find_ship_index(nHomeMissionShipIndex);
    proximity = aMissionNavPoints[
        nMissionEntryNavPoint].proximityRadius;
    objectiveType = aMissionObjectives[objective].type;
    for (; objectiveType != -1;
         objective++,
         objectiveType = aMissionObjectives[objective].type) {
        if (achieved(objective) == 0) {
            index = (short)aMissionObjectives[objective].index;
            switch (objectiveType) {
            case 0:
            case 1:
            case 3:
                if (visited(objective) != 0)
                    flag_objective(objective, 2);
                break;
            case 2:
                if (aMissionShips[index].missionType ==
                        MISSION_TYPE_GOTO_WARP) {
                    if (aMissionShips[index].state == 2) {
                        flag_objective(objective, 2);
                        if (aMissionShips[index].type ==
                                OBJECT_TYPE_HORNET ||
                            aMissionShips[index].type ==
                                OBJECT_TYPE_DRAYMAN)
                            affect_mission_score(0, 5, -1);
                        else
                            affect_mission_score(0, 9, -1);
                    }
                } else if (aMissionShips[index].missionType ==
                               MISSION_TYPE_WARP_ARRIVE ||
                           aMissionShips[index].missionType ==
                               MISSION_TYPE_COME_HOME) {
                    object = find_ship_index(index);
                    if (object != -1 && home != -1 &&
                        distance_from_object(object, home) < proximity)
                        aMissionShips[index].state = 1;
                    if (aMissionShips[index].state == 1 &&
                        sighted(objective) != 0) {
                        flag_objective(objective, 2);
                        if (aMissionShips[index].type ==
                                OBJECT_TYPE_HORNET ||
                            aMissionShips[index].type ==
                                OBJECT_TYPE_DRAYMAN)
                            affect_mission_score(0, 5, -1);
                        else
                            affect_mission_score(0, 9, -1);
                    }
                } else if (aMissionShips[index].state == 0 &&
                           sighted(objective) != 0) {
                    flag_objective(objective, 2);
                    if (aMissionShips[index].type ==
                            OBJECT_TYPE_HORNET ||
                        aMissionShips[index].type ==
                            OBJECT_TYPE_DRAYMAN)
                        affect_mission_score(0, 5, -1);
                    else
                        affect_mission_score(0, 9, -1);
                }
                break;
            case 4:
                if (aMissionShips[index].state == 3)
                    flag_objective(objective, 2);
                break;
            }
        }
    }
}

/* Function start: 0x42F1F0 */
int too_busy(short ship)
{
    return aeShipMissionType[ship] == MISSION_TYPE_ROUT;
}

/* Function start: 0x42F210 */
void reply(short ship, short accepted)
{
    if (accepted == 1) {
        send_message(ship, 0);
        return;
    }
    send_message(ship, 1);
}

/* Function start: 0x42F240 */
int disobey_formation(short ship)
{
    switch (aiPilotLevel[ship]) {
    case 10:
        return any_enemy_tail(0);
    case 11:
        return report_kilrathi_rout(0);
    }
    return 0;
}

/* Function start: 0x42F270 */
int bad_target(short ship, short target)
{
    if (target != ship &&
        aeShipSide[target] != aeShipSide[ship] &&
        too_busy(ship) == 0)
        return 0;
    return 1;
}

/* Function start: 0x42F2B0 */
short can_land(void)
{
    int *objectiveType;
    short result;
    short index;

    result = 0;
    if (any_enemy(0, 20000) == 0) {
        if (evaluate_damage(0) < 50 || nPlayerKillCount > 0 ||
            anShipFuel[0] < 1000)
            result = 1;
        index = 0;
        if (cMissionObjectiveCount > 0) {
            do {
                objectiveType =
                    &aMissionObjectives[index].type;
                if (*objectiveType != 1) {
                    if (achieved(index) == 0) {
                        if (visited(index) == 0 || *objectiveType == 2)
                            goto next_objective;
                    }
                    result = 1;
                }
next_objective:
                index++;
            } while (index < cMissionObjectiveCount);
        }
    }
    return result;
}

/* Function start: 0x42F350 */
short i_wanna_rout(short ship, int pilot)
{
    if (pilot <= 4)
        return 1;
    switch (pilot) {
    case 6:
        return any_enemy(ship, 5000) == 0;
    case 7:
    case 11:
        return 0;
    case 8:
        return aeShipMissionType[ship] ==
               MISSION_TYPE_CANNED_SEQUENCE;
    case 9:
        return (short)triumph(0);
    case 10:
        return any_enemy(0, 10000) == 0;
    }
    return 1;
}

/* Function start: 0x42F3F0 */
void request(short requester, short ship, short command)
{
    signed char *requesterTarget;
    short target;
    short object;

    requesterTarget = &acShipTarget[requester];

    for (;;) {
        target = (short)*requesterTarget;
        switch (command) {
        case 1:
            allow_engage();
            if (bad_target(ship, target) == 0) {
                engage(ship, target, OBJECTIVE_ENGAGE_ENEMY);
                reply(ship, 1);
                return;
            }
            reply(ship, 0);
            return;
        case 2:
            allow_engage();
            target = -1;
            for (object = 0; object < 10; object++) {
                if (aeObjectClass[object] >=
                        OBJECT_CLASS_SHIP &&
                    aeSpecialManeuver[object] !=
                        SPECIAL_MANEUVER_UNKNOWN_9 &&
                    aeShipSide[ship] !=
                        aeShipSide[object] &&
                    acShipTarget[object] == requester) {
                    target = object;
                    break;
                }
            }
            if (target == -1) {
                command = 9;
                continue;
            }
            engage(ship, target, OBJECTIVE_ENGAGE_ENEMY);
            reply(ship, 1);
            return;
        case 3:
            if (i_wanna_rout(ship, aiPilotLevel[ship]) != 0 &&
                try2rout(ship) != 0) {
                bEngageAllowed = 0;
                reply(ship, 1);
                return;
            }
            reply(ship, 0);
            return;
        case 4:
        case 5:
        case 6:
            if (RandomBelow(100) < 70 ||
                ((signed char)acShipRating[ship] > 8 &&
                 (signed char)acShipRating[ship] < 13))
                send_message(ship, (signed char)(command - 2));
            if (acShipTarget[ship] != requester &&
                too_busy(ship) == 0) {
                engage(ship, requester, OBJECTIVE_ENGAGE_ENEMY);
                return;
            }
            break;
        case 7:
            allow_engage();
            if (aeShipObjective[ship] ==
                    OBJECTIVE_HOLD_FORMATION) {
                reset_objective(ship, OBJECTIVE_BREAK_FORMATION);
                reply(ship, 1);
                return;
            }
            reply(ship, 0);
            return;
        case 8:
            disallow_engage();
            if (disobey_formation(ship) != 0) {
                alter_objective(ship, OBJECTIVE_BREAK_FORMATION);
                reply(ship, 0);
                return;
            }
            nAutoEngageTimer = -150;
            reply(ship, 1);
            return;
        case 9:
            disallow_engage();
            if (disobey_formation(ship) != 0) {
                reply(ship, 0);
                return;
            }
            reset_objective(ship, OBJECTIVE_HOLD_FORMATION);
            nAutoEngageTimer = -150;
            reply(ship, 1);
            return;
        case 10:
        case 11:
            bRadioSilence = 0;
            reply(ship, 1);
            bRadioSilence = command == 10;
            return;
        case 12:
            cleanup_objectives();
            if (can_land() != 0) {
                bLandingAuthorized = 1;
                send_message(ship, 8);
                return;
            }
            send_message(ship, 9);
            return;
        }
        return;
    }
}

/* Function start: 0x42F730 */
unsigned short __stdcall ShouldSuspendCursorForRect(const ShortRect *bounds)
{
    (void)bounds;
    return 0;
}

/* Function start: 0x42F740 */
unsigned short __stdcall InitializeDIBScreenViewport(
    Viewport *viewport, unsigned short colour)
{
    short row;
    int offset;

    (void)colour;
    nScreenAllocationState = 0;
    pAllocatedScreenViewport = viewport;
    pAllocatedScreenViewportMirror = viewport;
    aiSoundEffectSourceActive[0] =
        (int)&nScreenAllocationState;
    viewport->right = 319;
    viewport->bottom = 199;
    viewport->left = 0;
    viewport->top = 0;
    if (nVideoMode != 0x13) {
        SystemDebugPrintf("== BAD alloc_screen == : type: '%d'\n",
                          (int)(short)nVideoMode);
        _exit(1);
    }
    viewport->pixels = GetDIBPixelBuffer();
    viewport->rowOffsets = awScreenRowOffsets;
    row = 0;
    offset = 0;
    do {
        awScreenRowOffsets[row] = (unsigned short)offset;
        offset = (short)offset + 320;
        row++;
    } while (row < 202);
    return 1;
}

/* Function start: 0x42F7E0 */
/* Initialises a 320x200 viewport record (0x13F == 319, 199) then validates it. */
void InitFullScreenViewport(int *record, short arg)
{
    Viewport *viewport = (Viewport *)record;

    DAT_005a6538 = record;
    viewport->left = 0;
    viewport->top = 0;
    viewport->right = 319;
    viewport->bottom = 199;
    AllocateViewport(viewport, arg, 0);
}

/* Function start: 0x42F810 */
unsigned int __stdcall GetPacketSize(const char *filename, short section)
{
    PacketSectionHandle handle;
    unsigned int size;

    size = (unsigned int)-1;
    if (OpenPacketSection(filename, section, &handle) != 0) {
        switch (handle.compression) {
        case 1:
            ReadPacketSectionData(&handle, &size, 4);
            if (nPacketError != 0)
                size = (unsigned int)-1;
            break;
        case 2:
            size = handle.dataSize;
            break;
        default:
            size = handle.dataSize;
            break;
        }
        CloseDataFileByHandle((unsigned short *)&handle);
    }
    return size;
}

/* Function start: 0x42F890 */
int GetFreeNearHeapBytes(void)
{
    NearHeapBlock *block;
    int descriptorAddress;
    int freeBytes;

    freeBytes = 0;
    descriptorAddress =
        nNearHeapBase + nNearHeapSize - 8;
    for (; descriptorAddress >= nNearHeapFirstDescriptor;
         descriptorAddress -= 8) {
        block = DosNearPtrToFar(descriptorAddress);
        if ((block->sizeAndFlags & 0x80000000) == 0)
            freeBytes += block->sizeAndFlags & 0xfffff;
    }
    return freeBytes;
}

/* Function start: 0x42F930 */
void FrameStartHook(int mode)
{
    TimerResetHook();
}

/* Function start: 0x42F940 */
unsigned short IsSoundHardwarePresent(short device)
{
    GetSoundHardwareFlag();
    return 1;
}

/* Function start: 0x42F950 */
void MessagePumpHook(int mode)
{
    TimerStopHook();
}

/* Function start: 0x42F960 */
void *PushMemoryStackFrame(void *memory, int offset)
{
    int index;

    if (offset != 0) {
        printf("push %p by %d\n", memory, offset);
        if (nPacketHandleCount == 0x1000)
            exit_squadron("qq mem push overflow");
        index = nPacketHandleCount;
        aiPacketHandleOffsets[index] = offset;
        if (offset < 0) {
            memory = (unsigned char *)memory - offset;
            apPacketHandles[index] = memory;
            nPacketHandleCount = index + 1;
            return memory;
        }
        memory = (unsigned char *)memory + offset;
        apPacketHandles[index] = memory;
        nPacketHandleCount = index + 1;
        return memory;
    }
    return memory;
}

/* Function start: 0x42F9E0 */
int IsPushedPacketHandle(void *handle)
{
    int index;

    index = 0;
    if (nPacketHandleCount > 0) {
        do {
            if (apPacketHandles[index] == handle) {
                if (aiPacketHandleOffsets[index] < 0)
                    return 1;
                return 0;
            }
            index++;
        } while (index < nPacketHandleCount);
    }
    return 0;
}

/* Function start: 0x42FA20 */
void *MapPacketHandleToBlock(void *handle)
{
    int count = nPacketHandleCount;
    int bytes = count * 4;
    int i;
    void **entry;
    int offset;

    for (;;) {
        i = 0;
        nPacketHandleCount = count;
        if (bytes <= 0)
            return handle;
        entry = apPacketHandles;
        while (*entry != handle) {
            entry = entry + 1;
            i = i + 1;
            nPacketHandleCount = count;
            if (i >= count)
                return handle;
        }
        offset = aiPacketHandleOffsets[i];
        if (offset < 0)
            handle = (unsigned char *)handle + offset;
        else
            handle = (unsigned char *)handle - offset;
#ifdef WC1_SDL
        apPacketHandles[i] =
            apPacketHandles[count - 1];
#else
        apPacketHandles[i] =
            *(void **)((unsigned char *)apPacketHandles + bytes - 4);
#endif
        bytes = bytes - 4;
        count = count - 1;
        aiPacketHandleOffsets[i] =
            *(int *)((unsigned char *)aiPacketHandleOffsets + bytes);
    }
}

/* Function start: 0x42FA90 */
void *AllocateTaggedMemory(unsigned int size, unsigned short flags)
{
    unsigned int *memory;
    unsigned short tagged;

    tagged = flags & 0x40;
#ifdef WC1_SDL
    if (tagged != 0)
        size += 8 + sizeof(unsigned char *);
#else
    if (tagged != 0)
        size += 8;
#endif
    memory = AllocateGuardedMemory(size);
    if (tagged != 0) {
#ifdef WC1_SDL
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
    int group = 4;
    void **entry = &aapPacketReferences[0][0];

    do {
        int i = 0x25;

        do {
            if (*entry == handle)
                *entry = 0;
            entry = entry + 1;
            i = i - 1;
        } while (i != 0);
        group = group - 1;
    } while (group != 0);
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
        ScaleFixedVector(&aShipForwardVector[63],
                         (int)distance << 8,
                         &aShipPosition[*object]);
        asObjectViewFrame[*object] = frame;
        asObjectScreenAngle[*object] = type;
        aeObjectType[*object] =
            (enum ObjectType)asObjectScreenAngle[*object];
        asObjectScreenScale[*object] = scale;
        apObjectShape[*object] = shape;
    }
}

/* Function start: 0x42FC00 */
unsigned int ShowCampaignVictorySequence(void)
{
    CampaignVictoryProjectile projectiles[16];
    unsigned char *planetShape;
    unsigned char *projectileShape;
    const ShortVector *origin;
    CampaignVictoryProjectile *projectile;
    short planetObject;
    short spawnCountdown;
    short vacant[2];
    short vacantCount;
    short textIndex;
    short slot;
    volatile short frame;
    short animationFrame;
    short elapsed;
    int planetScale;
    int verticalOffset;
    int planetDepth;

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
    slot = 16;
    projectile = projectiles;
    do {
        projectile->scale = -1;
        projectile++;
        slot--;
    } while (slot != 0);

    planetDepth = -1500;
    frame = 0;
    bEscapePressed = 0;
    verticalOffset = -70000;
    nFrameSkipCounter = 1;
    do {
        if (frame == 0)
            textIndex = 0;
        else if (frame == 100)
            textIndex = 1;
        else if (frame == 180)
            textIndex = 2;
        else
            textIndex = -1;
        if (textIndex != -1) {
            ClearViewport(&stConversationTextViewport,
                          cBlackColour);
            SetTextContext(&stConversationTextContext);
            FormatTextBufferFromStart(
                szCampaignVictoryTextFormat, 0, 160,
                apszCampaignVictoryText[textIndex]);
        }

        Update_3Space();
        if (Draw_3Space_Frame() != 0) {
            if (frame > 90)
                asObjectScreenScale[planetObject]++;
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
                                &stSpaceBuffer,
                                (short)(projectile->screenX +
                                        nViewCenterX),
                                (short)(projectile->screenY +
                                        nViewCenterY),
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
                    vacantCount = 0;
                    slot = 0;
                    projectile = projectiles;
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
                DrawSpriteScaled(
                    &stSpaceBuffer, nViewCenterX,
                    (short)(nViewCenterY +
                            verticalOffset / planetDepth),
                    planetShape, 0, 0, (short)planetScale, 0);
                verticalOffset += 200;
            }
            dump_buffer_to_screen();
            clear_view_buffer();
            DIBslam();
            DIBslamReal();
        }
        planetDepth += 15;
        if (bEscapePressed == 1)
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
    if (bEscapePressed != 1) {
        planetShape =
            FetchDiskPacketRetrying(9, 5, 0);
        animationFrame = 1;
        ClearViewport(&stScreen, cBlackColour);
        WaitForVerticalBlankThunk();
        DrawSpriteDefault(&stScreen, 0, 0, planetShape, 0);
        elapsed = 0;
        WaitForSceneAdvance(14, 0);
        do {
            SetFrameTimerPeriodDirect(8);
            DrawSpriteDefault(&stScreen, 0, 0, planetShape,
                              animationFrame++);
            if (animationFrame > 17)
                animationFrame = 12;
            while ((short)IsFrameTickElapsed() == 0) {
                if (bEscapePressed != 0 || CheckEscaped() != 0) {
                    elapsed = 1000;
                    break;
                }
            }
            elapsed++;
            DIBslam();
            DIBslamReal();
        } while (elapsed < 40);
        ReleasePacketHandle(planetShape);
        FadeViewportPaletteToColour(&stModalSourceViewport,
                                    cBlackColour, 1);
        ClearViewport(&stModalSourceViewport,
                      cBlackColour);
        DIBslam();
        DIBslamReal();
        RestoreGamePalette();
    }

    bEscapePressed = 0;
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook(0x21);
    bScriptedView = 0;
    bIntroSceneResourcesActive = 1;
    nCannedSceneMode = 0;
    FadeViewportPaletteToColour(&stModalSourceViewport,
                                cBlackColour, 1);
    ClearViewport(&stModalSourceViewport,
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
    int depth;
    int verticalOffset;

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
    ClearViewport(&stConversationTextViewport,
                  cBlackColour);
    SetTextContext(&stConversationTextContext);
    FormatTextBufferFromStart(
        szTigerClawEscapeOpeningFormat, 0, 160,
        pszTigerClawEscapeOpening);
    nFrameSkipCounter = 1;
    bEscapePressed = 0;
    depth = -1000;
    verticalOffset = -70000;
    frame = 0;
    do {
        Update_3Space();
        if (Draw_3Space_Frame() != 0) {
            if (asObjectCollisionRadius[61] < depth &&
                frame < 198) {
                DrawSpriteScaled(
                    &stSpaceBuffer, nViewCenterX,
                    (short)(nViewCenterY +
                            verticalOffset / depth),
                    escapeShape, 0, 0, (short)(0x40000L / depth), 0);
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
            ClearViewport(&stConversationTextViewport,
                          cBlackColour);
            SetTextContext(&stConversationTextContext);
            FormatTextBufferFromStart(
                szTigerClawEscapeJumpFormat, 0, 160,
                pszTigerClawEscapeJump);
            break;
        case 190:
            effect = find_vacant_3d_object();
            if (effect != -1) {
                set_objects_data(effect,
                                 OBJECT_TYPE_HYPERSPACE_JUMP_FLASH,
                                 -1);
                ScaleFixedVector(&aShipForwardVector[61],
                                 0x271000, &jumpOffset);
                asObjectScale[effect] =
                    (short)(asObjectScale[effect] << 2);
                zero_vector(&aShipVelocity[effect]);
                AddFixedVectors(&aShipPosition[61],
                                &jumpOffset,
                                &aShipPosition[effect]);
            }
            break;
        case 198:
            ClearViewport(&stSpaceBuffer,
                          cViewportClearColour);
            bViewportDirty = 1;
            break;
        case 210:
            ClearViewport(&stConversationTextViewport,
                          cBlackColour);
            SetTextContext(&stConversationTextContext);
            FormatTextBufferFromStart(
                szTigerClawEscapeClosingFormat, 0, 160,
                pszTigerClawEscapeClosing);
            break;
        }
        if (bEscapePressed == 1)
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
    ClearViewport(&stConversationTextViewport,
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
    FireworkState *firework;
    short activeFireworks;
    short frame;
    short slot;

    SetEventManagerPump(get_player_input);
    PreloadMusicTrackHook(0x17);
    spacetrack(0x17, 2, 1);
    InitializeConversationViewport();
    ViewMedals();
    ReleaseTextFont(0);
    ClearViewport(&stModalSourceViewport, cBlackColour);
    ClearViewport(&stSceneBuffer, cBlackColour);
    InitializeFireworks();
    pFireworkShape =
        FetchDiskPacketRetrying(9, 0x11, 0);
    pIntroFont =
        FetchDiskPacketRetrying(9, 1, 0);
    print_subtitle(&stSceneBuffer, 0x3a, pszTheEnd);
    PanToScreen(&stSceneBuffer, &stScreen);
    bEscapePressed = 0;
    nFrameSkipCounter = 1;
    activeFireworks = 0;
    frame = 0;
    do {
        ClearViewport(&stSceneBuffer, cBlackColour);
        if (enableFireworks != 0 && activeFireworks != 0 &&
            (RandomBelowOrEqual(100) < 40 || frame > 280)) {
            slot = 0;
            do {
                firework = &aFireworks[slot];
                if (firework->frame == -1) {
                    firework->frame = 0;
                    firework->x = RandomInRange(0, stSpaceBuffer.right);
                    firework->y = RandomInRange(0, stSpaceBuffer.bottom);
                    firework->variant = RandomInRange(0, 2);
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

/* Function start: 0x430710 */
short __stdcall UpdateInputDeviceTransitions(short raw)
{
    unsigned int now;
    unsigned short buttons;
    unsigned short changed;
    unsigned short previous;
    short device;

    device = nActiveInputDevice;
    if (raw == 0)
        ReadCalibratedJoystick();
    else
        SampleJoystickDevice(&aInputDeviceSamples[device],
                             device, 0);

    asInputButton2DoubleClick[device] = 0;
    asInputButton1DoubleClick[device] = 0;
    asInputButton2Changed[device] = 0;
    asInputButton1Changed[device] = 0;
    buttons = (unsigned short)aInputDeviceSamples[device].buttons;
    previous = (unsigned short)asPreviousInputButtons[device];
    if (buttons != previous) {
        changed = buttons ^ previous;
        now = nTickCount60Hz;
        if ((changed & 1) != 0) {
            asInputButton1Changed[device] = 1;
            if ((buttons & 1) != 0) {
                if ((int)(now -
                    anInputButton1PressTime[device]) <=
                        nInputDoubleClickInterval *
                        nInputTickScale)
                    asInputButton1DoubleClick[device] = 1;
                anInputButton1PressTime[device] = now;
            }
        }
        if ((changed & 2) != 0) {
            asInputButton2Changed[device] = 1;
            if ((buttons & 2) != 0) {
                if ((int)(now -
                    anInputButton2PressTime[device]) <=
                        nInputDoubleClickInterval *
                        nInputTickScale)
                    asInputButton2DoubleClick[device] = 1;
                anInputButton2PressTime[device] = now;
            }
        }
        asPreviousInputButtons[device] = (short)buttons;
    }
    return 1;
}

/* Function start: 0x430840 */
void PollJoystickButtonEvents(void)
{
    short doubleClick;

    if (nActiveInputDevice == -1 ||
        bInputPollingGuard != 0)
        return;
    bInputPollingGuard++;
    UpdateInputDeviceTransitions(1);
    if (asInputButton1Changed
            [nActiveInputDevice] != 0) {
        doubleClick = 0;
        if (asInputButton1DoubleClick
                [nActiveInputDevice] != 0)
            doubleClick = 3;
        QueueInputEventAtCursor(
            ((unsigned short)aInputDeviceSamples
                [nActiveInputDevice].buttons & 1) + 1,
            0, doubleClick);
    }
    if (asInputButton2Changed
            [nActiveInputDevice] != 0) {
        doubleClick = 0;
        if (asInputButton2DoubleClick
                [nActiveInputDevice] != 0)
            doubleClick = 3;
        QueueInputEventAtCursor(
            (((unsigned short)aInputDeviceSamples
                [nActiveInputDevice].buttons >> 1) & 1) + 1,
            1, doubleClick);
    }
    bInputPollingGuard--;
    stHostMouseState.primaryButton =
        (unsigned char)aInputDeviceSamples
            [nActiveInputDevice].buttons;
    stHostMouseState.secondaryButton =
        stHostMouseState.primaryButton;
}

/* Function start: 0x430920 */
void PollMenuInputDevices(void)
{
    unsigned char changes;
    char doubleClick;
    short movementX;
    short movementY;
    int deviceIndex;

    changes = 0;
    if (nActiveInputDevice == -1)
        return;
    stHostMouseState.x =
        stMouseCursorState.x;
    stHostMouseState.y =
        stMouseCursorState.y;
    if (bInputPollingGuard != 0)
        return;
    bInputPollingGuard++;
    UpdateInputDeviceTransitions(0);

    deviceIndex = (int)nActiveInputDevice;
    if (asInputButton1Changed[deviceIndex] != 0) {
        stHostMouseState.x +=
            (short)aInputDeviceSamples
                [deviceIndex].x *
            nMenuPointerSpeed;
        stHostMouseState.y +=
            (short)aInputDeviceSamples
                [deviceIndex].y *
            nMenuPointerSpeed;
        if (asInputButton1DoubleClick[deviceIndex] != 0)
            changes = 3;
        QueueInputEventAtCursor(
            ((unsigned short)aInputDeviceSamples
                [deviceIndex].buttons & 1) + 1,
            0, (short)(signed char)changes);
        changes = 1;
    }
    deviceIndex = (int)nActiveInputDevice;
    if (asInputButton2Changed[deviceIndex] != 0) {
        stHostMouseState.x +=
            (short)aInputDeviceSamples
                [deviceIndex].x *
            nMenuPointerSpeed;
        stHostMouseState.y +=
            (short)aInputDeviceSamples
                [deviceIndex].y *
            nMenuPointerSpeed;
        doubleClick = 0;
        if (asInputButton1DoubleClick[deviceIndex] != 0)
            doubleClick = 3;
        QueueInputEventAtCursor(
            (((unsigned short)aInputDeviceSamples
                [deviceIndex].buttons >> 1) & 1) + 1,
            1, (short)doubleClick);
        changes++;
    }
    if (changes == 0) {
        movementX = (short)aInputDeviceSamples
            [nActiveInputDevice].x;
        movementY = (short)aInputDeviceSamples
            [nActiveInputDevice].y;
        stHostMouseState.x +=
            nMenuPointerSpeed * movementX;
        stHostMouseState.y +=
            nMenuPointerSpeed * movementY;
        changes = (unsigned char)movementY | (unsigned char)movementX;
        if (changes != 0) {
            FlushInputEvents();
            if (stHostMouseState.x <= 0)
                stHostMouseState.x = 0;
            if (stHostMouseState.x >= 319)
                stHostMouseState.x = 319;
            if (stHostMouseState.y <= 0)
                stHostMouseState.y = 0;
            if (stHostMouseState.y >= 199)
                stHostMouseState.y = 199;
            SetMousePosition(stHostMouseState.x,
                             stHostMouseState.y);
        }
    }

    stHostMouseState.primaryButton =
        (unsigned char)aInputDeviceSamples
            [nActiveInputDevice].buttons;
    stHostMouseState.secondaryButton =
        stHostMouseState.primaryButton;
    if (stHostMouseState.x <= 0)
        stHostMouseState.x = 0;
    if (stHostMouseState.x >= 319)
        stHostMouseState.x = 319;
    if (stHostMouseState.y <= 0)
        stHostMouseState.y = 0;
    if (stHostMouseState.y >= 199)
        stHostMouseState.y = 199;
    if (changes != 0) {
        LeaveAllocationScope();
        stMouseCursorState.primaryButton =
            stHostMouseState.primaryButton;
        stMouseCursorState.x =
            stHostMouseState.x;
        stMouseCursorState.y =
            stHostMouseState.y;
        stMouseCursorState.secondaryButton =
            stHostMouseState.secondaryButton;
        stMouseCursorState.flags =
            stHostMouseState.flags;
        EnterAllocationScope();
    }
    bInputPollingGuard--;
}

/* Function start: 0x430BC0 */
short get_face(short rating, unsigned int side)
{
    if (rating == -1)
        return 13 + (side < SIDE_KILRATHI ? -1 : 0);
    if (side == SIDE_KILRATHI)
        rating--;
    return rating;
}

/* Function start: 0x430BF0 */
void LoadCommPortraitShape(short face, signed char alternate)
{
    short section;

    if (face >= 0 && face <= 7)
        section = face + 1;
    else if ((face > 7 && face < 12) || face == 13)
        section = 10;
    else
        section = -1;
    if (section != -1)
        apCommPortraitShapes[face] =
            FetchDiskPacketRetrying(11, section,
                                                     (short)alternate);
}

/* Function start: 0x430C50 */
void ResetCommMenuChoices(short reuse)
{
    short choice;

    if (reuse == 0) {
        memset(abCommMenuChoiceCommand, -1,
               sizeof(abCommMenuChoiceCommand));
        choice = 0;
        do {
            apszCommMenuChoiceText[choice] = 0;
            choice++;
        } while (choice < 7);
    }
    nCommMenuChoiceCount = 0;
    nCommMenuReuseMode = reuse;
}

/* Function start: 0x430CA0 */
int IsCommMenuIdle(void)
{
    return nCommMenuChoiceCount == 0;
}

/* Function start: 0x430CB0 */
void AppendCommMenuChoice(const char *text, short command)
{
    short index;

    if (nCommMenuReuseMode == 1) {
        index = nCommMenuChoiceCount;
        if (apszCommMenuChoiceText[index] != text ||
            abCommMenuChoiceCommand[index] != command)
            nCommMenuReuseMode = 0;
    }
    index = nCommMenuChoiceCount;
    apszCommMenuChoiceText[index] = text;
    nCommMenuChoiceCount = index + 1;
    abCommMenuChoiceCommand[index] = (signed char)command;
}

/* Function start: 0x430D30 */
void SendCommMenuChoice(short i)
{
    AppendCommMenuChoice(apszCommMenuText[i], i);
}

/* Function start: 0x430D50 */
void OpenCommMenuForTarget(const char *heading, const char *message)
{
    CockpitMessage(message, cYellowColour, -1);
    pszCommMenuHeading = heading;
}

/* Function start: 0x430D80 */
int IsCommChoiceMenuOpen(void)
{
    return get_mode(1) == 4;
}

/* Function start: 0x430DA0 */
short GetPendingMenuAction(void)
{
    return (short)cPendingCommMenuAction;
}

/* Function start: 0x430DB0 */
void SetPendingMenuAction(unsigned char v)
{
    cPendingCommMenuAction = (signed char)v;
}

/* Function start: 0x430DC0 */
void OpenCommRecipientMenu(void)
{
    push_mode(1, 4);
    SetPendingMenuAction(1);
}

/* Function start: 0x430DE0 */
void CloseCommChoiceMenu(void)
{
    if (get_mode(1) == 4) {
        pop_mode(1);
        return;
    }
    ShutdownEventManager();
    exit_squadron("!stop");
}

/* Function start: 0x430E10 */
int wingman_dead(void)
{
    return nYourWingman == -1;
}

/* Function start: 0x430E30 */
short have_target(void)
{
    return unactive(acShipTarget[0]) == 0;
}

/* Function start: 0x430E50 */
unsigned short CanOpenCommMenu(void)
{
    if (have_target() == 0) {
        if (wingman_dead())
            return 0;
    }
    return 1;
}

/* Function start: 0x430E70 */
void SelectCommRecipient(short recipient)
{
    cCommMenuRecipient = recipient;
    SetPendingMenuAction(2);
}

/* Function start: 0x430E90 */
void BuildCommunicationRecipientMenu(void)
{
    short target;
    short command;
    const char *text;

    ResetCommMenuChoices(nCommMenuReuseMode);
    OpenCommMenuForTarget("VID-COM SYSTEM\n\nSend message to?\n\n",
                          "SELECT");
    if (wingman_dead() != 0) {
        SelectCommRecipient(acShipTarget[0]);
        return;
    }
    if (have_target() == 0 ||
        acShipTarget[0] == nYourWingman) {
        SelectCommRecipient(nYourWingman);
        return;
    }
    AppendCommMenuChoice(
        apWingmanPilots[
            (signed char)acShipRating[
                nYourWingman]]->callsign,
        1);
    target = acShipTarget[0];
    if (target != -1) {
        if (aeShipSide[target] == SIDE_KILRATHI &&
            aeObjectClass[target] == OBJECT_CLASS_SHIP) {
            command = 2;
            text = "ENEMY TARGET";
        } else {
            if (aeShipSide[target] != SIDE_IMPERIAL ||
                ((aeObjectClass[target] != OBJECT_CLASS_SHIP ||
                  any_enemy(0, 14000) == 0) &&
                 aeObjectType[target] !=
                     OBJECT_TYPE_TIGERS_CLAW)) {
                goto finish_recipient_menu;
            }
            command = 3;
            text = aObjectTypeData[
                aeObjectType[target]].displayName;
        }
        AppendCommMenuChoice(text, command);
    }

finish_recipient_menu:
    SendCommMenuChoice(0);
}

/* Function start: 0x430FC0 */
void BuildCommunicationCommandMenu(void)
{
    signed char rating;
    const char *name;

    ResetCommMenuChoices(nCommMenuReuseMode);
    if (cCommMenuRecipient == nYourWingman) {
        if (aeShipObjective[nYourWingman] ==
                OBJECTIVE_HOLD_FORMATION &&
            any_enemy(0, 14000) != 0) {
            SendCommMenuChoice(7);
        }
        if (nAutoEngageTimer == -1) {
            if (aeShipObjective[nYourWingman] !=
                    OBJECTIVE_HOLD_FORMATION)
                SendCommMenuChoice(9);
        } else {
            SendCommMenuChoice(8);
        }
        if (bRadioSilence != 0)
            SendCommMenuChoice(11);
        else
            SendCommMenuChoice(10);
    }
    if (aeShipSide[cCommMenuRecipient] ==
            aeShipSide[0]) {
        if (aeObjectType[cCommMenuRecipient] ==
                OBJECT_TYPE_TIGERS_CLAW &&
            bLandingAuthorized == 0) {
            SendCommMenuChoice(12);
        }
        if (have_target() != 0 &&
            aeShipSide[acShipTarget[0]] ==
                SIDE_KILRATHI) {
            SendCommMenuChoice(1);
        }
        if (evaluate_damage(0) < 50 && any_enemy(0, 14000) != 0)
            SendCommMenuChoice(2);
    }
    if (cCommMenuRecipient == nYourWingman)
        SendCommMenuChoice(3);
    if (aeShipSide[cCommMenuRecipient] ==
            SIDE_KILRATHI) {
        SendCommMenuChoice(4);
        SendCommMenuChoice(5);
        SendCommMenuChoice(6);
    }
    if (IsCommMenuIdle() == 0)
        SendCommMenuChoice(0);
    else
        CloseCommChoiceMenu();

    if (IsCommChoiceMenuOpen() != 0) {
        strcpy(szCommMenuHeadingBuffer,
               "VID-COM SYSTEM\n\nTo: ");
        rating = acShipRating[cCommMenuRecipient];
        if (rating == -1) {
            name = aObjectTypeData[
                aeObjectType[
                    cCommMenuRecipient]].displayName;
        } else if (rating < 8) {
            name = apWingmanPilots[rating]->callsign;
        } else {
            name = apszKilrathiAceNames[rating - 9];
        }
        strcat(szCommMenuHeadingBuffer, name);
        strcat(szCommMenuHeadingBuffer, "\n");
        OpenCommMenuForTarget(szCommMenuHeadingBuffer,
                              "CHOOSE");
    }
}

/* Function start: 0x431200 */
void RefreshCommunicationMenu(void)
{
    if (IsCommChoiceMenuOpen() != 0) {
        if (GetPendingMenuAction() == 1)
            BuildCommunicationRecipientMenu();
        if (GetPendingMenuAction() == 2)
            BuildCommunicationCommandMenu();
        if (nCommMenuReuseMode == 0)
            InvalidateVduMode(1);
    }
}

/* Function start: 0x431240 */
void HandleCommunicationMenuRequest(void)
{
    if (IsCommChoiceMenuOpen() != 0)
        CloseCommChoiceMenu();
    if (message_showing() == 0 &&
        IsCommChoiceMenuOpen() == 0 && CanOpenCommMenu() != 0) {
        OpenCommRecipientMenu();
        ResetCommMenuChoices(0);
        RefreshCommunicationMenu();
    }
}

/* Function start: 0x431290 */
void show_communications_disp(void)
{
    signed char choice;
#ifdef WC1_SDL
    int selectedChoice;
    unsigned char normalColour;
#endif

    if (IsCommChoiceMenuOpen() == 0)
        HandleCommunicationMenuRequest();
    if (IsCommChoiceMenuOpen() != 0) {
        set_new_vdu(1);
        choice = 0;
        DrawTextAt(&stRightVduTextContext, stRightVdu.left,
                   stRightVdu.top, pszCommMenuHeading, 2);
#ifdef WC1_SDL
        selectedChoice = Wc1SdlGetCommunicationMenuSelection();
        normalColour = stRightVduTextContext.colour;
#endif
        if (nCommMenuChoiceCount > 0) {
            do {
#ifdef WC1_SDL
                if ((int)choice == selectedChoice)
                    stRightVduTextContext.colour = cYellowColour;
#endif
                DrawFormattedText("\n%d %s", (int)choice + 1,
                                  apszCommMenuChoiceText[
                                      (int)choice]);
#ifdef WC1_SDL
                stRightVduTextContext.colour = normalColour;
#endif
                choice++;
            } while (choice < nCommMenuChoiceCount);
        }
        DrawSpriteDefault(&stRightVdu,
                          (short)(stRightVdu.left + 36),
                          (short)(stRightVdu.top + 10),
                          pCommMenuCursorShape, 0x19);
        nCommMenuReuseMode = 1;
    }
}

/* Function start: 0x431350 */
unsigned int Chosen_communicate_option(short choice)
{
    int action;

    PlaySfxWaveFileByNumber(0x19, -1, 0);
    action = GetPendingMenuAction();
    switch (action) {
    case 0:
        CloseCommChoiceMenu();
        return 0;
    case 1:
        if (abCommMenuChoiceCommand[choice] == 0) {
            CloseCommChoiceMenu();
            return 0;
        }
        if (abCommMenuChoiceCommand[choice] == 1) {
            SelectCommRecipient(nYourWingman);
            RefreshCommunicationMenu();
            return 0;
        }
        SelectCommRecipient(acShipTarget[0]);
        RefreshCommunicationMenu();
        return 0;
    case 2:
        CloseCommChoiceMenu();
        request(0, (short)cCommMenuRecipient,
                (short)abCommMenuChoiceCommand[choice]);
        return 0;
    }
    return 0;
}

/* Function start: 0x431400 */
void talk_equiv(void)
{
    RefreshCommunicationMenu();
}

/* Function start: 0x431410 */
void FreeCommDisplayResources(void)
{
#ifdef WC1_SDL
    /* The original indexes the portrait table with its -1 inactive sentinel,
       aliasing the final two palette-allocation words at 0x0059E17C. */
    if (nCommPortraitIndex != -1)
#endif
    FreePacketAndClear(&apCommPortraitShapes[
        nCommPortraitIndex], 0);
    FreePacketAndClear(&pConfedCommBackground, 0);
    FreePacketAndClear(&pKilrathiCommBackground, 0);
    FreePacketAndClear(&pCommStaticShape, 0);
    nCommSpeakerRating = -1;
    nCommSpeakerObject = -1;
    nCommPortraitIndex = -1;
}

/* Function start: 0x431470 */
void EndCommSessionWithWingman(void)
{
#ifdef WC1_SDL
    if (nCommPortraitIndex != -1 &&
#else
    if (
#endif
        apCommPortraitShapes[nCommPortraitIndex] != 0)
        malf_noise(1, 1, 12, 23, 1);
    FreeCommDisplayResources();
    if (get_mode(1) == 6)
        pop_mode(1);
}

/* Function start: 0x4314C0 */
void EndCommMenu(void)
{
    clear_message_time();
    if (get_mode(1) == 6)
        EndCommSessionWithWingman();
    pszPendingHudMessage = 0;
}

/* Function start: 0x4314F0 */
void ShowCentredPrompt(char *text, unsigned short arg)
{
    DosStrcpy(szHudMessageBuffer, text);
    SetHudMessageText(szHudMessageBuffer,
                      cYellowColour, arg);
}

/* Function start: 0x431520 */
short LoadCommDisplayResources(short rating, enum Side side)
{
    short loaded;

    loaded = 1;
    switch (side) {
    case SIDE_IMPERIAL:
        if (pConfedCommBackground == 0)
            pConfedCommBackground =
                FetchDiskPacketRetrying(11, 0, 0);
        loaded = pConfedCommBackground != 0;
        break;
    case SIDE_KILRATHI:
        if (pKilrathiCommBackground == 0)
            pKilrathiCommBackground =
                FetchDiskPacketRetrying(11, 9, 0);
        loaded = pKilrathiCommBackground != 0;
        break;
    }
    if (pCommStaticShape == 0)
        pCommStaticShape =
            FetchDiskPacketRetrying(11, 11, 0);
    if (loaded != 0 && pCommStaticShape != 0)
        return 1;
    return 0;
}

/* Function start: 0x4315C0 */
char *ExpandCommMessageTokens(const char *text)
{
    char *destination;
    const char *marker;
    short length;

    szTextScratchBuffer[0] = '\0';
    for (;;) {
        marker = DosStrchr(text, '$');
        if (marker == 0) {
            DosStrcat(szTextScratchBuffer, text);
            return szTextScratchBuffer;
        }
        destination = DosStrchr(szTextScratchBuffer, '\0');
        while (marker != text)
            *destination++ = *text++;
        *destination = '\0';
        text = marker + 2;
        switch (marker[1]) {
        case 'C':
            DosStrcat(
                szTextScratchBuffer,
                stCampaignState.currentPilot->callsign);
            break;
        case 'N':
        case 'P':
            DosStrcat(szTextScratchBuffer,
                      stCampaignState.currentPilot->name);
            break;
        case 'R':
            DosStrcat(szTextScratchBuffer,
                      apszPilotRankNames[
                          stCampaignState.currentPilot->rank]);
            length = DosStrlen(szTextScratchBuffer);
            if (szTextScratchBuffer[length - 1] == '.' &&
                *text == '.') {
                length = DosStrlen(szTextScratchBuffer);
                szTextScratchBuffer[length - 1] = '\0';
            }
            break;
        }
    }
}

/* Function start: 0x4316E0 */
void real_vid_transmit(short obj, short message)
{
    char text[84];
    char *expandedText;
    char *speech;
    int objectOffset;

    nCommSpeakerObject = obj;
    nCommSpeakerRating =
        (short)acShipRating[obj];
    nCommPortraitIndex =
        get_face(nCommSpeakerRating,
                 aeShipSide[obj]);
    if (nCommPortraitIndex == -1)
        return;
    objectOffset = (int)obj * sizeof(enum ObjectType);
    if (bCommVideoEnabled != 0 && bVideoImagesSuppressed == 0) {
        if (apCommPortraitShapes[
                nCommPortraitIndex] == 0)
            LoadCommPortraitShape(nCommPortraitIndex, 0);
        if (apCommPortraitShapes[
                nCommPortraitIndex] != 0 &&
            LoadCommDisplayResources(nCommSpeakerRating,
                aeShipSide[
                    nCommSpeakerObject]) != 0) {
            push_mode(1, 6);
            malf_noise(1, 3, 12, 23, 1);
            DrawSpriteDefault(
                &stRightVdu, stRightVdu.left,
                stRightVdu.top,
                aeShipSide[
                    nCommSpeakerObject] == SIDE_IMPERIAL ?
                    pConfedCommBackground :
                    pKilrathiCommBackground,
                0);
            DrawSpriteDefault(
                &stRightVdu, stRightVdu.left,
                stRightVdu.top,
                apCommPortraitShapes[
                    nCommPortraitIndex],
                0);
        }
    }
    speech = aapszPilotSpeech[
        nCommPortraitIndex][message];
    if (nCommSpeakerRating >= 0 &&
        nCommSpeakerRating <= 7) {
#ifdef WC1_SDL
        /* MSVC 4.20 accepts %Fs as its legacy far-string conversion. */
        sprintf(text, "%s: %s",
                apWingmanPilots[
                    nCommSpeakerRating]->callsign,
                speech);
#else
        sprintf(text, szConfedCommFormat,
                apWingmanPilots[
                    nCommSpeakerRating]->callsign,
                speech);
#endif
    } else if (nCommSpeakerRating >= 9 &&
               nCommSpeakerRating <= 12) {
#ifdef WC1_SDL
        sprintf(text, "%s: %s",
                apszKilrathiAceNames[
                    nCommSpeakerRating - 9],
                speech);
#else
        sprintf(text, szKilrathiAceCommFormat,
                apszKilrathiAceNames[
                    nCommSpeakerRating - 9],
                speech);
#endif
    } else {
#ifdef WC1_SDL
        sprintf(text, "%s: %s",
                aObjectTypeData[
                    *(enum ObjectType *)(void *)
                        ((unsigned char *)aeObjectType +
                         objectOffset)].displayName,
                speech);
#else
        sprintf(text, szShipCommFormat,
                aObjectTypeData[
                    *(enum ObjectType *)(void *)
                        ((unsigned char *)aeObjectType +
                         objectOffset)].displayName,
                speech);
#endif
    }
    expandedText = ExpandCommMessageTokens(text);
    ShowCentredPrompt(expandedText, (unsigned short)MeasureMessageWidth(text));
}

/* Function start: 0x4318F0 */
void __stdcall ShutdownVideoHook(short mode)
{
    ReleaseVideoResourcesHook();
}

/* Function start: 0x431900 */
short __stdcall ReserveContiguousPaletteEntries(short entryCount)
{
    short entry;
    short freeEntries;
    short firstEntry;
    short fillEntry;

    freeEntries = 0;
    entry = 0;
    firstEntry = 0;
    for (;;) {
        if (awPaletteEntryAllocation[entry] != 0) {
            freeEntries = 0;
            firstEntry = (short)(entry + 1);
        } else
            freeEntries++;
        if (freeEntries == entryCount)
            break;
        entry++;
        if (entry >= 256)
            return -1;
    }

    fillEntry = 0;
    while (fillEntry < entryCount) {
        awPaletteEntryAllocation[firstEntry + fillEntry] =
            entryCount;
        fillEntry++;
    }
    return firstEntry;
}

/* Function start: 0x431970 */
void __stdcall ReleaseContiguousPaletteEntries(short firstEntry)
{
    short entry;
    short entryCount;

    entry = 0;
    entryCount = awPaletteEntryAllocation[firstEntry];
    while (entry < entryCount) {
        awPaletteEntryAllocation[firstEntry + entry] = 0;
        entry++;
    }
}

/* Function start: 0x4319B0 */
void PrintPaletteAllocationMap(void)
{
    short index;
    short row;
    short column;

    index = 0;
    row = 4;
    do {
        column = 0x40;
        do {
            printf("%c", awPaletteEntryAllocation[index++] < 1 ?
                   '_' : '.');
            column--;
        } while (column != 0);
        printf("\n");
        row--;
    } while (row != 0);
}

/* Function start: 0x431A10 */
void LoadJoystickCalibrationFile(short horizontalRange,
                                 short verticalRange,
                                 short horizontalDeadZone,
                                 short verticalDeadZone)
{
    unsigned short storedCentreX;
    unsigned short storedCentreY;
    unsigned short minimumX;
    unsigned short maximumX;
    unsigned short maximumY;
    unsigned short minimumY;
    short activeDevice;
    InputDeviceSample samples[2];
    int file;
    int failed;
    int centreX;
    int centreY;

    activeDevice = -1;
    failed = 1;
    if (horizontalRange == 0)
        horizontalRange += (short)failed;
    if (verticalRange == 0)
        verticalRange++;
    nJoystickFailureValue = -1;
    SampleBothJoysticks(samples, 0xffff);
    if (samples[0].x != -1 && samples[0].y != -1)
        activeDevice = 0;
    else if (samples[1].x != -1 && samples[1].y != -1)
        activeDevice = 1;

    if (activeDevice != -1) {
        file = _open("j.cal", 0x8000);
        if (file != -1) {
            failed = _read(file, &activeDevice, 2) <= 0;
            if (failed == 0)
                failed = _read(file, &minimumX, 2) <= 0;
            if (failed == 0)
                failed = _read(file, &minimumY, 2) <= 0;
            if (failed == 0)
                failed = _read(file, &maximumX, 2) <= 0;
            if (failed == 0)
                failed = _read(file, &maximumY, 2) <= 0;
            if (failed == 0)
                failed = _read(file, &storedCentreX, 2) <= 0;
            if (failed == 0)
                failed = _read(file, &storedCentreY, 2) <= 0;
            nJoystickCentreX = (unsigned int)storedCentreX;
            nJoystickCentreY = (unsigned int)storedCentreY;
            _close(file);
            if (failed != 0)
                _unlink("j.cal");
        }

        if (failed != 0) {
            GetJoystickDevCaps(activeDevice,
                               (short *)&minimumX,
                               (short *)&maximumX,
                               (short *)&minimumY,
                               (short *)&maximumY);
            centreX = ((int)minimumX + (int)maximumX) / 2;
            centreY = ((int)maximumY + (int)minimumY) / 2;
        } else {
            centreX = nJoystickCentreX;
            centreY = nJoystickCentreY;
        }

        nJoystickCalibrationMinimumX = centreX;
        nJoystickCalibrationMaximumX = centreX;
        if (centreX > 10) {
            nJoystickCalibrationMinimumX = centreX - 10;
            nJoystickCalibrationMaximumX = centreX + 10;
        }
        nJoystickCalibrationMinimumY = centreY;
        nJoystickCalibrationMaximumY = centreY;
        if (centreY > 10) {
            nJoystickCalibrationMinimumY = centreY - 10;
            nJoystickCalibrationMaximumY = centreY + 10;
        }

        nJoystickLeftScale =
            nJoystickCalibrationMinimumX /
            (int)horizontalRange;
        nJoystickUpScale =
            nJoystickCalibrationMinimumY /
            (int)verticalRange;
        if (nJoystickLeftScale == 0)
            nJoystickLeftScale = 1;
        if (nJoystickUpScale == 0)
            nJoystickUpScale = 1;
        nJoystickRightScale =
            nJoystickCalibrationMinimumX /
            (int)horizontalRange;
        if (nJoystickRightScale == 0)
            nJoystickRightScale = 1;
        nJoystickDownScale =
            nJoystickCalibrationMinimumY /
            (int)verticalRange;
        if (nJoystickDownScale == 0)
            nJoystickDownScale = 1;

        nJoystickCentreX = centreX;
        nJoystickCentreY = centreY;
        nJoystickMinimumX =
            centreX - horizontalRange * nJoystickLeftScale;
        nJoystickMinimumY =
            centreY - verticalRange * nJoystickUpScale;
        nJoystickMaximumX =
            horizontalRange * nJoystickLeftScale + centreX;
        nJoystickMaximumY =
            verticalRange * nJoystickUpScale + centreY;
        nJoystickHorizontalDeadZone = horizontalDeadZone;
        nJoystickVerticalDeadZone = verticalDeadZone;
    }
    nActiveInputDevice = activeDevice;
}

/* Function start: 0x431D20 */
short ReadCalibratedJoystick(void)
{
    InputDeviceSample *sample;
    int sampleResult;
    int rawX;
    int rawY;
    int deviceIndex;
    short normalizedX;
    short normalizedY;
    short device;

    device = nActiveInputDevice;
    if (device == -1)
        return 0;
    deviceIndex = (int)device;
    sample = &aInputDeviceSamples[deviceIndex];
    sampleResult = SampleJoystickDevice(
        sample, device,
        nJoystickFailureValue);
    rawX = sample->x;
    rawY = aInputDeviceSamples[deviceIndex].y;
    if (rawX == nJoystickFailureValue ||
        rawY == nJoystickFailureValue ||
        sampleResult != 0) {
        nActiveInputDevice = -1;
        aInputDeviceSamples[deviceIndex].buttons = 0;
        aInputDeviceSamples[deviceIndex].y = 0;
        sample->x = 0;
        return 0;
    }

    nJoystickRawX = rawX;
    nJoystickRawY = rawY;
    if (rawX < nJoystickMinimumX)
        sample->x = nJoystickMinimumX;
    if (sample->x > nJoystickMaximumX)
        sample->x = nJoystickMaximumX;
    if (aInputDeviceSamples[deviceIndex].y <
        nJoystickMinimumY)
        aInputDeviceSamples[deviceIndex].y =
            nJoystickMinimumY;
    if (aInputDeviceSamples[deviceIndex].y >
        nJoystickMaximumY)
        aInputDeviceSamples[deviceIndex].y =
            nJoystickMaximumY;

    normalizedY = 0;
    normalizedX = 0;
    rawX = sample->x;
    if (nJoystickCentreX > rawX) {
        normalizedX = (short)((nJoystickCentreX - rawX) /
                              nJoystickLeftScale);
        if (nJoystickHorizontalDeadZone < normalizedX) {
            if (normalizedX != 0)
                normalizedX = (short)-normalizedX;
        } else {
            normalizedX = 0;
        }
    } else if (nJoystickCentreX < rawX) {
        normalizedX = (short)((rawX - nJoystickCentreX) /
                              nJoystickRightScale);
        if (normalizedX <= nJoystickHorizontalDeadZone)
            normalizedX = 0;
    }

    rawY = aInputDeviceSamples[deviceIndex].y;
    if (nJoystickCentreY > rawY) {
        normalizedY = (short)((nJoystickCentreY - rawY) /
                              nJoystickUpScale);
        if (nJoystickVerticalDeadZone < normalizedY) {
            if (normalizedY != 0)
                normalizedY = (short)-normalizedY;
        } else {
            normalizedY = 0;
        }
    } else if (nJoystickCentreY < rawY) {
        normalizedY = (short)((rawY - nJoystickCentreY) /
                              nJoystickDownScale);
        if (normalizedY <= nJoystickVerticalDeadZone)
            normalizedY = 0;
    }
    sample->x = (int)normalizedX;
    aInputDeviceSamples[deviceIndex].y = (int)normalizedY;
    return 1;
}

/* Function start: 0x431EA0 */
void __stdcall UnionRectBounds(ShortRect *destination,
                               const ShortRect *first,
                               const ShortRect *second)
{
    destination->left = first->left < second->left ?
        first->left : second->left;
    destination->top = first->top < second->top ?
        first->top : second->top;
    destination->right = first->right > second->right ?
        first->right : second->right;
    destination->bottom = first->bottom > second->bottom ?
        first->bottom : second->bottom;
}

/* Function start: 0x431F00 */
void ThrottleFrameAndDrawFps(HDC dc)
{
    DWORD now;

    if (bShowFrameRate != 0) {
        sprintf((char *)szFrameRateText, "%f",
                fMeasuredFrameRate);
        TextOutA(dc, 0, 0, (char *)szFrameRateText,
                 strlen((char *)szFrameRateText));
    }

    if (stMouseCursorState.viewport->pixels ==
            pDIBPixelBuffer) {
        while (timeGetTime() < (DWORD)nFrameDeadlineMs) {
            Sleep(0);
            RefreshMouseCursorDisplay();
        }
    } else {
        while (timeGetTime() < (DWORD)nFrameDeadlineMs)
            Sleep(0);
    }

    if (bShowFrameRate != 0) {
        if (nPreviousFrameTimeMs != 0) {
            now = timeGetTime();
            fMeasuredFrameRate =
                1000.0f / ((double)now - nPreviousFrameTimeMs);
        }
        nPreviousFrameTimeMs = timeGetTime();
        sprintf((char *)szFrameRateText, "%f",
                fMeasuredFrameRate);
        TextOutA(dc, 0, 0, (char *)szFrameRateText,
                 strlen((char *)szFrameRateText));
    }

    nFrameDeadlineMs = timeGetTime() + nFrameIntervalMs;
}
