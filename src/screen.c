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
    home = find_ship_index(g_nHomeMissionShipIndex_005a8692);
    proximity = g_aMissionNavPoints_0046c2f0[
        g_nMissionEntryNavPoint_005a8690].proximityRadius;
    objectiveType = g_aMissionObjectives_0059dac0[objective].type;
    while (objectiveType != -1) {
        if (achieved(objective) == 0) {
            index = (short)g_aMissionObjectives_0059dac0[objective].index;
            switch (objectiveType) {
            case 0:
            case 1:
            case 3:
                if (visited(objective) != 0)
                    flag_objective(objective, 2);
                break;
            case 2:
                if (g_aMissionShips_0046c948[index].missionType ==
                        MISSION_TYPE_GOTO_WARP) {
                    if (g_aMissionShips_0046c948[index].state == 2) {
                        flag_objective(objective, 2);
                        if (g_aMissionShips_0046c948[index].type ==
                                OBJECT_TYPE_HORNET ||
                            g_aMissionShips_0046c948[index].type ==
                                OBJECT_TYPE_DRAYMAN)
                            affect_mission_score(0, 5, -1);
                        else
                            affect_mission_score(0, 9, -1);
                    }
                } else if (g_aMissionShips_0046c948[index].missionType ==
                               MISSION_TYPE_WARP_ARRIVE ||
                           g_aMissionShips_0046c948[index].missionType ==
                               MISSION_TYPE_COME_HOME) {
                    object = find_ship_index(index);
                    if (object != -1 && home != -1 &&
                        distance_from_object(object, home) < proximity)
                        g_aMissionShips_0046c948[index].state = 1;
                    if (g_aMissionShips_0046c948[index].state == 1 &&
                        sighted(objective) != 0) {
                        flag_objective(objective, 2);
                        if (g_aMissionShips_0046c948[index].type ==
                                OBJECT_TYPE_HORNET ||
                            g_aMissionShips_0046c948[index].type ==
                                OBJECT_TYPE_DRAYMAN)
                            affect_mission_score(0, 5, -1);
                        else
                            affect_mission_score(0, 9, -1);
                    }
                } else if (g_aMissionShips_0046c948[index].state == 0 &&
                           sighted(objective) != 0) {
                    flag_objective(objective, 2);
                    if (g_aMissionShips_0046c948[index].type ==
                            OBJECT_TYPE_HORNET ||
                        g_aMissionShips_0046c948[index].type ==
                            OBJECT_TYPE_DRAYMAN)
                        affect_mission_score(0, 5, -1);
                    else
                        affect_mission_score(0, 9, -1);
                }
                break;
            case 4:
                if (g_aMissionShips_0046c948[index].state == 3)
                    flag_objective(objective, 2);
                break;
            }
        }
        objective++;
        objectiveType = g_aMissionObjectives_0059dac0[objective].type;
    }
}

/* Function start: 0x42F1F0 */
int too_busy(short ship)
{
    return g_aeShipMissionType_0059c3f0[ship] == MISSION_TYPE_ROUT;
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
    switch (g_aiPilotLevel_0059cf30[ship]) {
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
        g_aeShipSide_0059d650[target] != g_aeShipSide_0059d650[ship] &&
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
        if (evaluate_damage(0) < 50 || g_nPlayerKillCount_005a7c9c > 0 ||
            g_anShipFuel_0059b470[0] < 1000)
            result = 1;
        index = 0;
        if (g_cMissionObjectiveCount_0059c46a > 0) {
            do {
                objectiveType =
                    &g_aMissionObjectives_0059dac0[index].type;
                if (*objectiveType != 1) {
                    if (achieved(index) == 0) {
                        if (visited(index) == 0 || *objectiveType == 2)
                            goto next_objective;
                    }
                    result = 1;
                }
next_objective:
                index++;
            } while (index < g_cMissionObjectiveCount_0059c46a);
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
        return g_aeShipMissionType_0059c3f0[ship] ==
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

    requesterTarget = &g_acShipTarget_0059ce60[requester];

retry_request:
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
        object = 0;
        do {
            if (g_aeObjectClass_0059d100[object] >= OBJECT_CLASS_SHIP &&
                g_aeSpecialManeuver_0059c3c0[object] !=
                    SPECIAL_MANEUVER_UNKNOWN_9 &&
                g_aeShipSide_0059d650[ship] !=
                    g_aeShipSide_0059d650[object] &&
                g_acShipTarget_0059ce60[object] == requester) {
                target = object;
                break;
            }
            object++;
        } while (object < 10);
        if (target == -1) {
            command = 9;
            goto retry_request;
        }
        engage(ship, target, OBJECTIVE_ENGAGE_ENEMY);
        reply(ship, 1);
        return;
    case 3:
        if (i_wanna_rout(ship, g_aiPilotLevel_0059cf30[ship]) != 0 &&
            try2rout(ship) != 0) {
            g_bEngageAllowed_0046c080 = 0;
            reply(ship, 1);
            return;
        }
        reply(ship, 0);
        return;
    case 4:
    case 5:
    case 6:
        if (RandomBelow(100) < 70 ||
            ((signed char)g_acShipRating_0059cd80[ship] > 8 &&
             (signed char)g_acShipRating_0059cd80[ship] < 13))
            send_message(ship, (signed char)(command - 2));
        if (g_acShipTarget_0059ce60[ship] != requester &&
            too_busy(ship) == 0) {
            engage(ship, requester, OBJECTIVE_ENGAGE_ENEMY);
            return;
        }
        break;
    case 7:
        allow_engage();
        if (g_aeShipObjective_0059d200[ship] ==
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
        g_nAutoEngageTimer_0046c084 = -150;
        reply(ship, 1);
        return;
    case 9:
        disallow_engage();
        if (disobey_formation(ship) != 0) {
            reply(ship, 0);
            return;
        }
        reset_objective(ship, OBJECTIVE_HOLD_FORMATION);
        g_nAutoEngageTimer_0046c084 = -150;
        reply(ship, 1);
        return;
    case 10:
    case 11:
        g_bRadioSilence_0046af70 = 0;
        reply(ship, 1);
        g_bRadioSilence_0046af70 = command == 10;
        return;
    case 12:
        cleanup_objectives();
        if (can_land() != 0) {
            g_bLandingAuthorized_00468ff8 = 1;
            send_message(ship, 8);
            return;
        }
        send_message(ship, 9);
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
    g_nScreenAllocationState_005a66e0 = 0;
    g_pAllocatedScreenViewport_005a6534 = viewport;
    g_pAllocatedScreenViewportMirror_005a66e4 = viewport;
    g_aiSoundEffectSourceActive_005a66ec[0] =
        (int)&g_nScreenAllocationState_005a66e0;
    viewport->right = 319;
    viewport->bottom = 199;
    viewport->left = 0;
    viewport->top = 0;
    if (DAT_0046b168 != 0x13) {
        SystemDebugPrintf("== BAD alloc_screen == : type: '%d'\n",
                          (int)(short)DAT_0046b168);
        _exit(1);
    }
    viewport->pixels = GetDIBPixelBuffer();
    viewport->rowOffsets = g_awScreenRowOffsets_005a6540;
    row = 0;
    offset = 0;
    do {
        g_awScreenRowOffsets_005a6540[row] = (unsigned short)offset;
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
            if (g_nPacketError_00465460 != 0)
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
        g_nNearHeapBase_005a8120 + g_nNearHeapSize_005a811c - 8;
    while (descriptorAddress >= g_nNearHeapFirstDescriptor_005a8124) {
        block = DosNearPtrToFar(descriptorAddress);
        if ((block->sizeAndFlags & 0x80000000) == 0)
            freeBytes += block->sizeAndFlags & 0xfffff;
        descriptorAddress -= 8;
    }
    return freeBytes;
}

/* Function start: 0x42F930 */
void FrameStartHook(int mode)
{
    TimerResetHook();
}

/* Function start: 0x42F940 */
unsigned short IsSoundHardwarePresent(void)
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
        if (g_nPacketHandleCount_005a6530 == 0x1000)
            exit_squadron("qq mem push overflow");
        index = g_nPacketHandleCount_005a6530;
        g_aiPacketHandleOffsets_005a2530[index] = offset;
        if (offset < 0) {
            memory = (unsigned char *)memory - offset;
            g_apPacketHandles_0059e530[index] = memory;
            g_nPacketHandleCount_005a6530 = index + 1;
            return memory;
        }
        memory = (unsigned char *)memory + offset;
        g_apPacketHandles_0059e530[index] = memory;
        g_nPacketHandleCount_005a6530 = index + 1;
        return memory;
    }
    return memory;
}

/* Function start: 0x42F9E0 */
int IsPushedPacketHandle(void *handle)
{
    int index;

    index = 0;
    if (g_nPacketHandleCount_005a6530 > 0) {
        do {
            if (g_apPacketHandles_0059e530[index] == handle) {
                if (g_aiPacketHandleOffsets_005a2530[index] < 0)
                    return 1;
                return 0;
            }
            index++;
        } while (index < g_nPacketHandleCount_005a6530);
    }
    return 0;
}

/* Function start: 0x42FA20 */
void *MapPacketHandleToBlock(void *handle)
{
    int count = g_nPacketHandleCount_005a6530;
    int bytes = count * 4;
    int i;
    void **entry;
    int offset;

    for (;;) {
        i = 0;
        g_nPacketHandleCount_005a6530 = count;
        if (bytes <= 0)
            return handle;
        entry = g_apPacketHandles_0059e530;
        while (*entry != handle) {
            entry = entry + 1;
            i = i + 1;
            g_nPacketHandleCount_005a6530 = count;
            if (i >= count)
                return handle;
        }
        offset = g_aiPacketHandleOffsets_005a2530[i];
        if (offset < 0)
            handle = (unsigned char *)handle + offset;
        else
            handle = (unsigned char *)handle - offset;
#ifdef WC1_SDL
        g_apPacketHandles_0059e530[i] =
            g_apPacketHandles_0059e530[count - 1];
#else
        g_apPacketHandles_0059e530[i] =
            *(void **)((unsigned char *)g_apPacketHandles_0059e530 + bytes - 4);
#endif
        bytes = bytes - 4;
        count = count - 1;
        g_aiPacketHandleOffsets_005a2530[i] =
            *(int *)((unsigned char *)g_aiPacketHandleOffsets_005a2530 + bytes);
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
               g_abTaggedAllocationPrefix_0046ad88,
               sizeof(g_abTaggedAllocationPrefix_0046ad88));
        memory = PushMemoryStackFrame(
            memory, -(int)(8 + sizeof(unsigned char *)));
#else
        memcpy(memory, g_abTaggedAllocationPrefix_0046ad88,
               sizeof(g_abTaggedAllocationPrefix_0046ad88));
        memory = PushMemoryStackFrame(memory, -8);
#endif
    }
    return memory;
}

/* Function start: 0x42FAE0 */
void ReleasePacketHandle(void *handle)
{
    int group = 4;
    void **entry = &g_aapPacketReferences_00465c88[0][0];

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
unsigned int GetFixedOneMillionThunk(void)
{
    return GetFixedOneMillion();
}

/* Function start: 0x42FB30 */
unsigned int GetFixedOneMillionThunkAlt(void)
{
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
        g_aeObjectClass_0059d100[*object] = OBJECT_CLASS_PLANET;
        init_ijk(63);
        alter_yaw(yaw, 63);
        ScaleFixedVector(&g_aShipForwardVector_0059bce0[63],
                         (int)distance << 8,
                         &g_aShipPosition_0059c490[*object]);
        g_asObjectViewFrame_0059d230[*object] = frame;
        g_asObjectScreenAngle_0059cd90[*object] = type;
        g_aeObjectType_0059b560[*object] =
            (enum ObjectType)g_asObjectScreenAngle_0059cd90[*object];
        g_asObjectScreenScale_0059c950[*object] = scale;
        g_apObjectShape_0059d2f0[*object] = shape;
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
    g_nCannedSceneMode_00469fac = 2;
    g_bIntroSceneResourcesActive_00469d48 = 0;
    set_up_action_sphere(0x12);
    planetShape =
        FetchDiskPacketRetrying(9, 3, 0);
    projectileShape =
        FetchDiskPacketRetrying(9, 2, 0);
    CreateCannedSceneObject(&planetObject, -4, 0, 30000,
                            planetShape, 0, 0, 0x50);
    g_nScriptedViewObject_0046a8d0 = 1;
    initialize_scripted_view(g_asCampaignVictoryViewScript_0046c160);
    slot = 16;
    projectile = projectiles;
    do {
        projectile->scale = -1;
        projectile++;
        slot--;
    } while (slot != 0);

    planetDepth = -1500;
    frame = 0;
    DAT_0059ab58 = 0;
    verticalOffset = -70000;
    DAT_00469fb4 = 1;
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
            ClearViewport(&g_stConversationTextViewport_005a7570,
                          DAT_0046999c);
            SetTextContext(&g_stConversationTextContext_005a7760);
            FormatTextBufferFromStart(
                g_szCampaignVictoryTextFormat_0046af24, 0, 160,
                g_apszCampaignVictoryText_0046ad90[textIndex]);
        }

        Update_3Space();
        if (Draw_3Space_Frame() != 0) {
            if (frame > 90)
                g_asObjectScreenScale_0059c950[planetObject]++;
            if (g_asObjectCollisionRadius_0059d710[61] < planetDepth) {
                slot = 0;
                projectile = projectiles;
                do {
                    if (projectile->scale != -1 &&
                        g_asObjectCollisionRadius_0059d710[61] <
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
                                &DAT_005a7510,
                                (short)(projectile->screenX +
                                        g_nViewCenterX_0059a852),
                                (short)(projectile->screenY +
                                        g_nViewCenterY_0059a854),
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
                            &g_aCampaignVictoryProjectileOrigins_0046adb0[
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
                    &DAT_005a7510, g_nViewCenterX_0059a852,
                    (short)(g_nViewCenterY_0059a854 +
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
        if (DAT_0059ab58 == 1)
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
    if (DAT_0059ab58 != 1) {
        planetShape =
            FetchDiskPacketRetrying(9, 5, 0);
        animationFrame = 1;
        ClearViewport(&DAT_005a6ba0, DAT_0046999c);
        WaitForVerticalBlankThunk();
        DrawSpriteDefault(&DAT_005a6ba0, 0, 0, planetShape, 0);
        elapsed = 0;
        WaitForSceneAdvance(14, 0);
        do {
            SetFrameTimerPeriodDirect(8);
            DrawSpriteDefault(&DAT_005a6ba0, 0, 0, planetShape,
                              animationFrame++);
            if (animationFrame > 17)
                animationFrame = 12;
            while ((short)IsFrameTickElapsed() == 0) {
                if (DAT_0059ab58 != 0 || CheckEscaped() != 0) {
                    elapsed = 1000;
                    break;
                }
            }
            elapsed++;
            DIBslam();
            DIBslamReal();
        } while (elapsed < 40);
        ReleasePacketHandle(planetShape);
        FadeViewportPaletteToColour(&g_stModalSourceViewport_005a7670,
                                    DAT_0046999c, 1);
        ClearViewport(&g_stModalSourceViewport_005a7670,
                      DAT_0046999c);
        DIBslam();
        DIBslamReal();
        RestoreGamePalette();
    }

    DAT_0059ab58 = 0;
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook(0x21);
    g_bScriptedView_0046a8d4 = 0;
    g_bIntroSceneResourcesActive_00469d48 = 1;
    g_nCannedSceneMode_00469fac = 0;
    FadeViewportPaletteToColour(&g_stModalSourceViewport_005a7670,
                                DAT_0046999c, 1);
    ClearViewport(&g_stModalSourceViewport_005a7670,
                  DAT_0046999c);
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
    init_3Space_objects((short)g_stCampaignState_0059ca50.currentSeries);
    g_nCannedSceneMode_00469fac = 2;
    g_bIntroSceneResourcesActive_00469d48 = 0;
    InitializeConversationText();
    set_up_action_sphere(0x13);
    escapeShape =
        FetchDiskPacketRetrying(9, 2, 0);
    if (g_aObjectTypeData_00466458[
            OBJECT_TYPE_HYPERSPACE_JUMP_FLASH].shapeSet == 0) {
        g_aObjectTypeData_00466458[
            OBJECT_TYPE_HYPERSPACE_JUMP_FLASH].shapeSet =
                FetchDiskPacketRetrying(3, 14, 0);
    }
    g_nScriptedViewObject_0046a8d0 = 1;
    initialize_scripted_view(g_asTigerClawEscapeViewScript_0046c238);
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    SetTextContext(&g_stConversationTextContext_005a7760);
    FormatTextBufferFromStart(
        g_szTigerClawEscapeOpeningFormat_0046af30, 0, 160,
        g_pszTigerClawEscapeOpening_0046ada0);
    DAT_00469fb4 = 1;
    DAT_0059ab58 = 0;
    depth = -1000;
    verticalOffset = -70000;
    frame = 0;
    do {
        Update_3Space();
        if (Draw_3Space_Frame() != 0) {
            if (g_asObjectCollisionRadius_0059d710[61] < depth &&
                frame < 198) {
                DrawSpriteScaled(
                    &DAT_005a7510, g_nViewCenterX_0059a852,
                    (short)(g_nViewCenterY_0059a854 +
                            verticalOffset / depth),
                    escapeShape, 0, 0, (short)(0x40000L / depth), 0);
            }
            dump_buffer_to_screen();
            clear_view_buffer();
        }
        if (g_asObjectCollisionRadius_0059d710[61] < depth)
            verticalOffset += 400;
        depth += approachStep;
        if (frame > 170)
            approachStep = (short)(approachStep + 10);

        switch (frame) {
        case 150:
            ClearViewport(&g_stConversationTextViewport_005a7570,
                          DAT_0046999c);
            SetTextContext(&g_stConversationTextContext_005a7760);
            FormatTextBufferFromStart(
                g_szTigerClawEscapeJumpFormat_0046af3c, 0, 160,
                g_pszTigerClawEscapeJump_0046ada4);
            break;
        case 190:
            effect = find_vacant_3d_object();
            if (effect != -1) {
                set_objects_data(effect,
                                 OBJECT_TYPE_HYPERSPACE_JUMP_FLASH,
                                 -1);
                ScaleFixedVector(&g_aShipForwardVector_0059bce0[61],
                                 0x271000, &jumpOffset);
                g_asObjectScale_0059de40[effect] =
                    (short)(g_asObjectScale_0059de40[effect] << 2);
                zero_vector(&g_aShipVelocity_0059c010[effect]);
                AddFixedVectors(&g_aShipPosition_0059c490[61],
                                &jumpOffset,
                                &g_aShipPosition_0059c490[effect]);
            }
            break;
        case 198:
            ClearViewport(&DAT_005a7510,
                          g_cViewportClearColour_004699a0);
            g_bViewportDirty_00469fc4 = 1;
            break;
        case 210:
            ClearViewport(&g_stConversationTextViewport_005a7570,
                          DAT_0046999c);
            SetTextContext(&g_stConversationTextContext_005a7760);
            FormatTextBufferFromStart(
                g_szTigerClawEscapeClosingFormat_0046af48, 0, 160,
                g_pszTigerClawEscapeClosing_0046ada8);
            break;
        }
        if (DAT_0059ab58 == 1)
            break;
        frame++;
        DIBslam();
        DIBslamReal();
    } while (frame < 260);

    ReleasePacketHandle(g_aObjectTypeData_00466458[
        OBJECT_TYPE_HYPERSPACE_JUMP_FLASH].shapeSet);
    g_aObjectTypeData_00466458[
        OBJECT_TYPE_HYPERSPACE_JUMP_FLASH].shapeSet = 0;
    ReleasePacketHandle(escapeShape);
    free_all_slots();
    ReleaseTextFont(0);
    free_3Space();
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook(0x22);
    g_bScriptedView_0046a8d4 = 0;
    g_nCannedSceneMode_00469fac = 0;
    g_bIntroSceneResourcesActive_00469d48 = 1;
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
    ClearViewport(&g_stModalSourceViewport_005a7670, DAT_0046999c);
    ClearViewport(&DAT_005a76b0, DAT_0046999c);
    InitializeFireworks();
    g_pFireworkShape_005a6a68 =
        FetchDiskPacketRetrying(9, 0x11, 0);
    g_pIntroFont_005a8960 =
        FetchDiskPacketRetrying(9, 1, 0);
    print_subtitle(&DAT_005a76b0, 0x3a, g_pszTheEnd_0046adc8);
    PanToScreen(&DAT_005a76b0, &DAT_005a6ba0);
    DAT_0059ab58 = 0;
    DAT_00469fb4 = 1;
    activeFireworks = 0;
    frame = 0;
    do {
        ClearViewport(&DAT_005a76b0, DAT_0046999c);
        if (enableFireworks != 0 && activeFireworks != 0 &&
            (RandomBelowOrEqual(100) < 40 || frame > 280)) {
            slot = 0;
            do {
                firework = &g_aFireworks_005a6900[slot];
                if (firework->frame == -1) {
                    firework->frame = 0;
                    firework->x = RandomInRange(0, DAT_005a7510.right);
                    firework->y = RandomInRange(0, DAT_005a7510.bottom);
                    firework->variant = RandomInRange(0, 2);
                    break;
                }
                slot++;
            } while (slot < 30);
        }
        activeFireworks = TheEndFireWorks(&DAT_005a76b0, 30);
        if (frame < 160) {
            print_subtitle(&DAT_005a76b0, 0x3a,
                           g_pszTheEnd_0046adc8);
        } else if (frame > 190) {
            StopMusic((short)(320 - frame));
            print_subtitle(&DAT_005a76b0, 0x3a,
                           g_pszForNow_0046adcc);
        }
        frame++;
        RefreshMemoryStatusOverlay();
        DIBslam();
        DIBslamReal();
    } while (frame < 320);

    ReleasePacketHandle(g_pFireworkShape_005a6a68);
    ReleasePacketHandle(g_pIntroFont_005a8960);
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

    device = g_nActiveInputDevice_005a819c;
    if (raw == 0)
        ReadCalibratedJoystick();
    else
        SampleJoystickDevice(&g_aInputDeviceSamples_005a81f0[device],
                             device, 0);

    g_asInputButton2DoubleClick_0059e520[device] = 0;
    g_asInputButton1DoubleClick_0059e508[device] = 0;
    g_asInputButton2Changed_0059e510[device] = 0;
    g_asInputButton1Changed_0059e50c[device] = 0;
    buttons = (unsigned short)g_aInputDeviceSamples_005a81f0[device].buttons;
    previous = (unsigned short)g_asPreviousInputButtons_0059e514[device];
    if (buttons != previous) {
        changed = buttons ^ previous;
        now = DAT_0059ab54;
        if ((changed & 1) != 0) {
            g_asInputButton1Changed_0059e50c[device] = 1;
            if ((buttons & 1) != 0) {
                if ((int)(now -
                    g_anInputButton1PressTime_0059e518[device]) <=
                        g_nInputDoubleClickInterval_0046af54 *
                        g_nInputTickScale_0059af90)
                    g_asInputButton1DoubleClick_0059e508[device] = 1;
                g_anInputButton1PressTime_0059e518[device] = now;
            }
        }
        if ((changed & 2) != 0) {
            g_asInputButton2Changed_0059e510[device] = 1;
            if ((buttons & 2) != 0) {
                if ((int)(now -
                    g_anInputButton2PressTime_0059e500[device]) <=
                        g_nInputDoubleClickInterval_0046af54 *
                        g_nInputTickScale_0059af90)
                    g_asInputButton2DoubleClick_0059e520[device] = 1;
                g_anInputButton2PressTime_0059e500[device] = now;
            }
        }
        g_asPreviousInputButtons_0059e514[device] = (short)buttons;
    }
    return 1;
}

/* Function start: 0x430840 */
void PollJoystickButtonEvents(void)
{
    short doubleClick;

    if (g_nActiveInputDevice_005a819c == -1 ||
        g_bInputPollingGuard_0046a01c != 0)
        return;
    g_bInputPollingGuard_0046a01c++;
    UpdateInputDeviceTransitions(1);
    if (g_asInputButton1Changed_0059e50c
            [g_nActiveInputDevice_005a819c] != 0) {
        doubleClick = 0;
        if (g_asInputButton1DoubleClick_0059e508
                [g_nActiveInputDevice_005a819c] != 0)
            doubleClick = 3;
        QueueInputEventAtCursor(
            ((unsigned short)g_aInputDeviceSamples_005a81f0
                [g_nActiveInputDevice_005a819c].buttons & 1) + 1,
            0, doubleClick);
    }
    if (g_asInputButton2Changed_0059e510
            [g_nActiveInputDevice_005a819c] != 0) {
        doubleClick = 0;
        if (g_asInputButton2DoubleClick_0059e520
                [g_nActiveInputDevice_005a819c] != 0)
            doubleClick = 3;
        QueueInputEventAtCursor(
            (((unsigned short)g_aInputDeviceSamples_005a81f0
                [g_nActiveInputDevice_005a819c].buttons >> 1) & 1) + 1,
            1, doubleClick);
    }
    g_bInputPollingGuard_0046a01c--;
    g_stHostMouseState_0059af70.primaryButton =
        (unsigned char)g_aInputDeviceSamples_005a81f0
            [g_nActiveInputDevice_005a819c].buttons;
    g_stHostMouseState_0059af70.secondaryButton =
        g_stHostMouseState_0059af70.primaryButton;
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
    if (g_nActiveInputDevice_005a819c == -1)
        return;
    g_stHostMouseState_0059af70.x =
        g_stMouseCursorState_0059ab10.x;
    g_stHostMouseState_0059af70.y =
        g_stMouseCursorState_0059ab10.y;
    if (g_bInputPollingGuard_0046a01c != 0)
        return;
    g_bInputPollingGuard_0046a01c++;
    UpdateInputDeviceTransitions(0);

    deviceIndex = (int)g_nActiveInputDevice_005a819c;
    if (g_asInputButton1Changed_0059e50c[deviceIndex] != 0) {
        g_stHostMouseState_0059af70.x +=
            (short)g_aInputDeviceSamples_005a81f0
                [deviceIndex].x *
            g_nMenuPointerSpeed_0046af58;
        g_stHostMouseState_0059af70.y +=
            (short)g_aInputDeviceSamples_005a81f0
                [deviceIndex].y *
            g_nMenuPointerSpeed_0046af58;
        if (g_asInputButton1DoubleClick_0059e508[deviceIndex] != 0)
            changes = 3;
        QueueInputEventAtCursor(
            ((unsigned short)g_aInputDeviceSamples_005a81f0
                [deviceIndex].buttons & 1) + 1,
            0, (short)(signed char)changes);
        changes = 1;
    }
    deviceIndex = (int)g_nActiveInputDevice_005a819c;
    if (g_asInputButton2Changed_0059e510[deviceIndex] != 0) {
        g_stHostMouseState_0059af70.x +=
            (short)g_aInputDeviceSamples_005a81f0
                [deviceIndex].x *
            g_nMenuPointerSpeed_0046af58;
        g_stHostMouseState_0059af70.y +=
            (short)g_aInputDeviceSamples_005a81f0
                [deviceIndex].y *
            g_nMenuPointerSpeed_0046af58;
        doubleClick = 0;
        if (g_asInputButton1DoubleClick_0059e508[deviceIndex] != 0)
            doubleClick = 3;
        QueueInputEventAtCursor(
            (((unsigned short)g_aInputDeviceSamples_005a81f0
                [deviceIndex].buttons >> 1) & 1) + 1,
            1, (short)doubleClick);
        changes++;
    }
    if (changes == 0) {
        movementX = (short)g_aInputDeviceSamples_005a81f0
            [g_nActiveInputDevice_005a819c].x;
        movementY = (short)g_aInputDeviceSamples_005a81f0
            [g_nActiveInputDevice_005a819c].y;
        g_stHostMouseState_0059af70.x +=
            g_nMenuPointerSpeed_0046af58 * movementX;
        g_stHostMouseState_0059af70.y +=
            g_nMenuPointerSpeed_0046af58 * movementY;
        changes = (unsigned char)movementY | (unsigned char)movementX;
        if (changes != 0) {
            FlushInputEvents();
            if (g_stHostMouseState_0059af70.x <= 0)
                g_stHostMouseState_0059af70.x = 0;
            if (g_stHostMouseState_0059af70.x >= 319)
                g_stHostMouseState_0059af70.x = 319;
            if (g_stHostMouseState_0059af70.y <= 0)
                g_stHostMouseState_0059af70.y = 0;
            if (g_stHostMouseState_0059af70.y >= 199)
                g_stHostMouseState_0059af70.y = 199;
            SetMousePosition(g_stHostMouseState_0059af70.x,
                             g_stHostMouseState_0059af70.y);
        }
    }

    g_stHostMouseState_0059af70.primaryButton =
        (unsigned char)g_aInputDeviceSamples_005a81f0
            [g_nActiveInputDevice_005a819c].buttons;
    g_stHostMouseState_0059af70.secondaryButton =
        g_stHostMouseState_0059af70.primaryButton;
    if (g_stHostMouseState_0059af70.x <= 0)
        g_stHostMouseState_0059af70.x = 0;
    if (g_stHostMouseState_0059af70.x >= 319)
        g_stHostMouseState_0059af70.x = 319;
    if (g_stHostMouseState_0059af70.y <= 0)
        g_stHostMouseState_0059af70.y = 0;
    if (g_stHostMouseState_0059af70.y >= 199)
        g_stHostMouseState_0059af70.y = 199;
    if (changes != 0) {
        LeaveAllocationScope();
        g_stMouseCursorState_0059ab10.primaryButton =
            g_stHostMouseState_0059af70.primaryButton;
        g_stMouseCursorState_0059ab10.x =
            g_stHostMouseState_0059af70.x;
        g_stMouseCursorState_0059ab10.y =
            g_stHostMouseState_0059af70.y;
        g_stMouseCursorState_0059ab10.secondaryButton =
            g_stHostMouseState_0059af70.secondaryButton;
        g_stMouseCursorState_0059ab10.flags =
            g_stHostMouseState_0059af70.flags;
        EnterAllocationScope();
    }
    g_bInputPollingGuard_0046a01c--;
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
        g_apCommPortraitShapes_0059e180[face] =
            FetchDiskPacketRetrying(11, section,
                                                     (short)alternate);
}

/* Function start: 0x430C50 */
void ResetCommMenuChoices(short reuse)
{
    short choice;

    if (reuse == 0) {
        memset(g_abCommMenuChoiceCommand_0059e488, -1,
               sizeof(g_abCommMenuChoiceCommand_0059e488));
        choice = 0;
        do {
            g_apszCommMenuChoiceText_0059e4e0[choice] = 0;
            choice++;
        } while (choice < 7);
    }
    g_nCommMenuChoiceCount_0046af60 = 0;
    g_nCommMenuReuseMode_0046af64 = reuse;
}

/* Function start: 0x430CA0 */
int IsCommMenuIdle(void)
{
    return g_nCommMenuChoiceCount_0046af60 == 0;
}

/* Function start: 0x430CB0 */
void AppendCommMenuChoice(const char *text, short command)
{
    short index;

    if (g_nCommMenuReuseMode_0046af64 == 1) {
        index = g_nCommMenuChoiceCount_0046af60;
        if (g_apszCommMenuChoiceText_0059e4e0[index] != text ||
            g_abCommMenuChoiceCommand_0059e488[index] != command)
            g_nCommMenuReuseMode_0046af64 = 0;
    }
    index = g_nCommMenuChoiceCount_0046af60;
    g_apszCommMenuChoiceText_0059e4e0[index] = text;
    g_nCommMenuChoiceCount_0046af60 = index + 1;
    g_abCommMenuChoiceCommand_0059e488[index] = (signed char)command;
}

/* Function start: 0x430D30 */
void SendCommMenuChoice(short i)
{
    AppendCommMenuChoice(g_apszCommMenuText_0046af90[i], i);
}

/* Function start: 0x430D50 */
void OpenCommMenuForTarget(const char *heading, const char *message)
{
    CockpitMessage(message, DAT_004699a8, -1);
    g_pszCommMenuHeading_0059e490 = heading;
}

/* Function start: 0x430D80 */
int IsCommChoiceMenuOpen(void)
{
    return get_mode(1) == 4;
}

/* Function start: 0x430DA0 */
short GetPendingMenuAction(void)
{
    return (short)g_cPendingCommMenuAction_0046af6c;
}

/* Function start: 0x430DB0 */
void SetPendingMenuAction(unsigned char v)
{
    g_cPendingCommMenuAction_0046af6c = (signed char)v;
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
    return g_nYourWingman_0046c04c == -1;
}

/* Function start: 0x430E30 */
short have_target(void)
{
    return unactive(g_acShipTarget_0059ce60[0]) == 0;
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
    g_cCommMenuRecipient_0046afc4 = recipient;
    SetPendingMenuAction(2);
}

/* Function start: 0x430E90 */
void BuildCommunicationRecipientMenu(void)
{
    short target;
    short command;
    const char *text;

    ResetCommMenuChoices(g_nCommMenuReuseMode_0046af64);
    OpenCommMenuForTarget("VID-COM SYSTEM\n\nSend message to?\n\n",
                          "SELECT");
    if (wingman_dead() != 0) {
        SelectCommRecipient(g_acShipTarget_0059ce60[0]);
        return;
    }
    if (have_target() == 0 ||
        g_acShipTarget_0059ce60[0] == g_nYourWingman_0046c04c) {
        SelectCommRecipient(g_nYourWingman_0046c04c);
        return;
    }
    AppendCommMenuChoice(
        g_apWingmanPilots_00598a30[
            (signed char)g_acShipRating_0059cd80[
                g_nYourWingman_0046c04c]]->callsign,
        1);
    target = g_acShipTarget_0059ce60[0];
    if (target != -1) {
        if (g_aeShipSide_0059d650[target] == SIDE_KILRATHI &&
            g_aeObjectClass_0059d100[target] == OBJECT_CLASS_SHIP) {
            command = 2;
            text = "ENEMY TARGET";
        } else {
            if (g_aeShipSide_0059d650[target] != SIDE_IMPERIAL ||
                ((g_aeObjectClass_0059d100[target] != OBJECT_CLASS_SHIP ||
                  any_enemy(0, 14000) == 0) &&
                 g_aeObjectType_0059b560[target] !=
                     OBJECT_TYPE_TIGERS_CLAW)) {
                goto finish_recipient_menu;
            }
            command = 3;
            text = g_aObjectTypeData_00466458[
                g_aeObjectType_0059b560[target]].displayName;
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

    ResetCommMenuChoices(g_nCommMenuReuseMode_0046af64);
    if (g_cCommMenuRecipient_0046afc4 == g_nYourWingman_0046c04c) {
        if (g_aeShipObjective_0059d200[g_nYourWingman_0046c04c] ==
                OBJECTIVE_HOLD_FORMATION &&
            any_enemy(0, 14000) != 0) {
            SendCommMenuChoice(7);
        }
        if (g_nAutoEngageTimer_0046c084 == -1) {
            if (g_aeShipObjective_0059d200[g_nYourWingman_0046c04c] !=
                    OBJECTIVE_HOLD_FORMATION)
                SendCommMenuChoice(9);
        } else {
            SendCommMenuChoice(8);
        }
        if (g_bRadioSilence_0046af70 != 0)
            SendCommMenuChoice(11);
        else
            SendCommMenuChoice(10);
    }
    if (g_aeShipSide_0059d650[g_cCommMenuRecipient_0046afc4] ==
            g_aeShipSide_0059d650[0]) {
        if (g_aeObjectType_0059b560[g_cCommMenuRecipient_0046afc4] ==
                OBJECT_TYPE_TIGERS_CLAW &&
            g_bLandingAuthorized_00468ff8 == 0) {
            SendCommMenuChoice(12);
        }
        if (have_target() != 0 &&
            g_aeShipSide_0059d650[g_acShipTarget_0059ce60[0]] ==
                SIDE_KILRATHI) {
            SendCommMenuChoice(1);
        }
        if (evaluate_damage(0) < 50 && any_enemy(0, 14000) != 0)
            SendCommMenuChoice(2);
    }
    if (g_cCommMenuRecipient_0046afc4 == g_nYourWingman_0046c04c)
        SendCommMenuChoice(3);
    if (g_aeShipSide_0059d650[g_cCommMenuRecipient_0046afc4] ==
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
        strcpy(g_szCommMenuHeadingBuffer_0059e4a0,
               "VID-COM SYSTEM\n\nTo: ");
        rating = g_acShipRating_0059cd80[g_cCommMenuRecipient_0046afc4];
        if (rating == -1) {
            name = g_aObjectTypeData_00466458[
                g_aeObjectType_0059b560[
                    g_cCommMenuRecipient_0046afc4]].displayName;
        } else if (rating < 8) {
            name = g_apWingmanPilots_00598a30[rating]->callsign;
        } else {
            name = g_apszKilrathiAceNames_0046af80[rating - 9];
        }
        strcat(g_szCommMenuHeadingBuffer_0059e4a0, name);
        strcat(g_szCommMenuHeadingBuffer_0059e4a0, "\n");
        OpenCommMenuForTarget(g_szCommMenuHeadingBuffer_0059e4a0,
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
        if (g_nCommMenuReuseMode_0046af64 == 0)
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

    if (IsCommChoiceMenuOpen() == 0)
        HandleCommunicationMenuRequest();
    if (IsCommChoiceMenuOpen() != 0) {
        set_new_vdu(1);
        choice = 0;
        DrawTextAt(&DAT_005a7700, DAT_005a7530.left,
                   DAT_005a7530.top, g_pszCommMenuHeading_0059e490, 2);
        if (g_nCommMenuChoiceCount_0046af60 > 0) {
            do {
                DrawFormattedText("\n%d %s", (int)choice + 1,
                                  g_apszCommMenuChoiceText_0059e4e0[
                                      (int)choice]);
                choice++;
            } while (choice < g_nCommMenuChoiceCount_0046af60);
        }
        DrawSpriteDefault(&DAT_005a7530,
                          (short)(DAT_005a7530.left + 36),
                          (short)(DAT_005a7530.top + 10),
                          g_pCommMenuCursorShape_005a7660, 0x19);
        g_nCommMenuReuseMode_0046af64 = 1;
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
        if (g_abCommMenuChoiceCommand_0059e488[choice] == 0) {
            CloseCommChoiceMenu();
            return 0;
        }
        if (g_abCommMenuChoiceCommand_0059e488[choice] == 1) {
            SelectCommRecipient(g_nYourWingman_0046c04c);
            RefreshCommunicationMenu();
            return 0;
        }
        SelectCommRecipient(g_acShipTarget_0059ce60[0]);
        RefreshCommunicationMenu();
        return 0;
    case 2:
        CloseCommChoiceMenu();
        request(0, (short)g_cCommMenuRecipient_0046afc4,
                (short)g_abCommMenuChoiceCommand_0059e488[choice]);
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
    if (g_nCommPortraitIndex_0046afd0 != -1)
#endif
    FreePacketAndClear(&g_apCommPortraitShapes_0059e180[
        g_nCommPortraitIndex_0046afd0], 0);
    FreePacketAndClear(&g_pConfedCommBackground_00469278, 0);
    FreePacketAndClear(&g_pKilrathiCommBackground_00469280, 0);
    FreePacketAndClear(&g_pCommStaticShape_0046927c, 0);
    g_nCommSpeakerRating_0046afcc = -1;
    g_nCommSpeakerObject_0046afc8 = -1;
    g_nCommPortraitIndex_0046afd0 = -1;
}

/* Function start: 0x431470 */
void EndCommSessionWithWingman(void)
{
#ifdef WC1_SDL
    if (g_nCommPortraitIndex_0046afd0 != -1 &&
#else
    if (
#endif
        g_apCommPortraitShapes_0059e180[g_nCommPortraitIndex_0046afd0] != 0)
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
    DAT_00469004 = 0;
}

/* Function start: 0x4314F0 */
void ShowCentredPrompt(char *text, unsigned short arg)
{
    DosStrcpy(g_szHudMessageBuffer_0059e1c0, text);
    SetHudMessageText(g_szHudMessageBuffer_0059e1c0,
                      DAT_004699a8, arg);
}

/* Function start: 0x431520 */
short LoadCommDisplayResources(short rating, enum Side side)
{
    short loaded;

    loaded = 1;
    switch (side) {
    case SIDE_IMPERIAL:
        if (g_pConfedCommBackground_00469278 == 0)
            g_pConfedCommBackground_00469278 =
                FetchDiskPacketRetrying(11, 0, 0);
        loaded = g_pConfedCommBackground_00469278 != 0;
        break;
    case SIDE_KILRATHI:
        if (g_pKilrathiCommBackground_00469280 == 0)
            g_pKilrathiCommBackground_00469280 =
                FetchDiskPacketRetrying(11, 9, 0);
        loaded = g_pKilrathiCommBackground_00469280 != 0;
        break;
    }
    if (g_pCommStaticShape_0046927c == 0)
        g_pCommStaticShape_0046927c =
            FetchDiskPacketRetrying(11, 11, 0);
    if (loaded != 0 && g_pCommStaticShape_0046927c != 0)
        return 1;
    return 0;
}

/* Function start: 0x4315C0 */
char *ExpandCommMessageTokens(const char *text)
{
    char *destination;
    const char *marker;
    short length;

    g_szTextScratchBuffer_00598b00[0] = '\0';
    for (;;) {
        marker = DosStrchr(text, '$');
        if (marker == 0) {
            DosStrcat(g_szTextScratchBuffer_00598b00, text);
            return g_szTextScratchBuffer_00598b00;
        }
        destination = DosStrchr(g_szTextScratchBuffer_00598b00, '\0');
        while (marker != text)
            *destination++ = *text++;
        *destination = '\0';
        text = marker + 2;
        switch (marker[1]) {
        case 'C':
            DosStrcat(
                g_szTextScratchBuffer_00598b00,
                g_stCampaignState_0059ca50.currentPilot->callsign);
            break;
        case 'N':
        case 'P':
            DosStrcat(g_szTextScratchBuffer_00598b00,
                      g_stCampaignState_0059ca50.currentPilot->name);
            break;
        case 'R':
            DosStrcat(g_szTextScratchBuffer_00598b00,
                      g_apszPilotRankNames_00470098[
                          g_stCampaignState_0059ca50.currentPilot->rank]);
            length = DosStrlen(g_szTextScratchBuffer_00598b00);
            if (g_szTextScratchBuffer_00598b00[length - 1] == '.' &&
                *text == '.') {
                length = DosStrlen(g_szTextScratchBuffer_00598b00);
                g_szTextScratchBuffer_00598b00[length - 1] = '\0';
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

    g_nCommSpeakerObject_0046afc8 = obj;
    g_nCommSpeakerRating_0046afcc =
        (short)g_acShipRating_0059cd80[obj];
    g_nCommPortraitIndex_0046afd0 =
        get_face(g_nCommSpeakerRating_0046afcc,
                 g_aeShipSide_0059d650[obj]);
    if (g_nCommPortraitIndex_0046afd0 == -1)
        return;
    objectOffset = (int)obj * sizeof(enum ObjectType);
    if (DAT_0046af78 != 0 && g_bVideoImagesSuppressed_0046af74 == 0) {
        if (g_apCommPortraitShapes_0059e180[
                g_nCommPortraitIndex_0046afd0] == 0)
            LoadCommPortraitShape(g_nCommPortraitIndex_0046afd0, 0);
        if (g_apCommPortraitShapes_0059e180[
                g_nCommPortraitIndex_0046afd0] != 0 &&
            LoadCommDisplayResources(g_nCommSpeakerRating_0046afcc,
                g_aeShipSide_0059d650[
                    g_nCommSpeakerObject_0046afc8]) != 0) {
            push_mode(1, 6);
            malf_noise(1, 3, 12, 23, 1);
            DrawSpriteDefault(
                &DAT_005a7530, DAT_005a7530.left, DAT_005a7530.top,
                g_aeShipSide_0059d650[
                    g_nCommSpeakerObject_0046afc8] == SIDE_IMPERIAL ?
                    g_pConfedCommBackground_00469278 :
                    g_pKilrathiCommBackground_00469280,
                0);
            DrawSpriteDefault(
                &DAT_005a7530, DAT_005a7530.left, DAT_005a7530.top,
                g_apCommPortraitShapes_0059e180[
                    g_nCommPortraitIndex_0046afd0],
                0);
        }
    }
    speech = g_aapszPilotSpeech_0059e220[
        g_nCommPortraitIndex_0046afd0][message];
    if (g_nCommSpeakerRating_0046afcc >= 0 &&
        g_nCommSpeakerRating_0046afcc <= 7) {
#ifdef WC1_SDL
        /* MSVC 4.20 accepts %Fs as its legacy far-string conversion. */
        sprintf(text, "%s: %s",
                g_apWingmanPilots_00598a30[
                    g_nCommSpeakerRating_0046afcc]->callsign,
                speech);
#else
        sprintf(text, g_szConfedCommFormat_0046b150,
                g_apWingmanPilots_00598a30[
                    g_nCommSpeakerRating_0046afcc]->callsign,
                speech);
#endif
    } else if (g_nCommSpeakerRating_0046afcc >= 9 &&
               g_nCommSpeakerRating_0046afcc <= 12) {
#ifdef WC1_SDL
        sprintf(text, "%s: %s",
                g_apszKilrathiAceNames_0046af80[
                    g_nCommSpeakerRating_0046afcc - 9],
                speech);
#else
        sprintf(text, g_szKilrathiAceCommFormat_0046b158,
                g_apszKilrathiAceNames_0046af80[
                    g_nCommSpeakerRating_0046afcc - 9],
                speech);
#endif
    } else {
#ifdef WC1_SDL
        sprintf(text, "%s: %s",
                g_aObjectTypeData_00466458[
                    *(enum ObjectType *)(void *)
                        ((unsigned char *)g_aeObjectType_0059b560 +
                         objectOffset)].displayName,
                speech);
#else
        sprintf(text, g_szShipCommFormat_0046b160,
                g_aObjectTypeData_00466458[
                    *(enum ObjectType *)(void *)
                        ((unsigned char *)g_aeObjectType_0059b560 +
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
        if (g_awPaletteEntryAllocation_0059df80[entry] != 0) {
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
        g_awPaletteEntryAllocation_0059df80[firstEntry + fillEntry] =
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
    entryCount = g_awPaletteEntryAllocation_0059df80[firstEntry];
    while (entry < entryCount) {
        g_awPaletteEntryAllocation_0059df80[firstEntry + entry] = 0;
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
            printf("%c", g_awPaletteEntryAllocation_0059df80[index++] < 1 ?
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
    g_nJoystickFailureValue_005a81e0 = -1;
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
            g_nJoystickCentreX_005a81dc = (unsigned int)storedCentreX;
            g_nJoystickCentreY_005a81d8 = (unsigned int)storedCentreY;
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
            centreX = g_nJoystickCentreX_005a81dc;
            centreY = g_nJoystickCentreY_005a81d8;
        }

        g_nJoystickCalibrationMinimumX_0059df68 = centreX;
        g_nJoystickCalibrationMaximumX_0059df6c = centreX;
        if (centreX > 10) {
            g_nJoystickCalibrationMinimumX_0059df68 = centreX - 10;
            g_nJoystickCalibrationMaximumX_0059df6c = centreX + 10;
        }
        g_nJoystickCalibrationMinimumY_0059df64 = centreY;
        g_nJoystickCalibrationMaximumY_0059df70 = centreY;
        if (centreY > 10) {
            g_nJoystickCalibrationMinimumY_0059df64 = centreY - 10;
            g_nJoystickCalibrationMaximumY_0059df70 = centreY + 10;
        }

        g_nJoystickLeftScale_005a81ac =
            g_nJoystickCalibrationMinimumX_0059df68 /
            (int)horizontalRange;
        g_nJoystickUpScale_005a81a8 =
            g_nJoystickCalibrationMinimumY_0059df64 /
            (int)verticalRange;
        if (g_nJoystickLeftScale_005a81ac == 0)
            g_nJoystickLeftScale_005a81ac = 1;
        if (g_nJoystickUpScale_005a81a8 == 0)
            g_nJoystickUpScale_005a81a8 = 1;
        g_nJoystickRightScale_005a81d0 =
            g_nJoystickCalibrationMinimumX_0059df68 /
            (int)horizontalRange;
        if (g_nJoystickRightScale_005a81d0 == 0)
            g_nJoystickRightScale_005a81d0 = 1;
        g_nJoystickDownScale_005a81d4 =
            g_nJoystickCalibrationMinimumY_0059df64 /
            (int)verticalRange;
        if (g_nJoystickDownScale_005a81d4 == 0)
            g_nJoystickDownScale_005a81d4 = 1;

        g_nJoystickCentreX_005a81dc = centreX;
        g_nJoystickCentreY_005a81d8 = centreY;
        g_nJoystickMinimumX_005a81b8 =
            centreX - horizontalRange * g_nJoystickLeftScale_005a81ac;
        g_nJoystickMinimumY_005a81bc =
            centreY - verticalRange * g_nJoystickUpScale_005a81a8;
        g_nJoystickMaximumX_005a81b0 =
            horizontalRange * g_nJoystickLeftScale_005a81ac + centreX;
        g_nJoystickMaximumY_005a81b4 =
            verticalRange * g_nJoystickUpScale_005a81a8 + centreY;
        g_nJoystickHorizontalDeadZone_005a81a4 = horizontalDeadZone;
        g_nJoystickVerticalDeadZone_005a81a0 = verticalDeadZone;
    }
    g_nActiveInputDevice_005a819c = activeDevice;
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

    device = g_nActiveInputDevice_005a819c;
    if (device == -1)
        return 0;
    deviceIndex = (int)device;
    sample = &g_aInputDeviceSamples_005a81f0[deviceIndex];
    sampleResult = SampleJoystickDevice(
        sample, device,
        g_nJoystickFailureValue_005a81e0);
    rawX = sample->x;
    rawY = g_aInputDeviceSamples_005a81f0[deviceIndex].y;
    if (rawX == g_nJoystickFailureValue_005a81e0 ||
        rawY == g_nJoystickFailureValue_005a81e0 ||
        sampleResult != 0) {
        g_nActiveInputDevice_005a819c = -1;
        g_aInputDeviceSamples_005a81f0[deviceIndex].buttons = 0;
        g_aInputDeviceSamples_005a81f0[deviceIndex].y = 0;
        sample->x = 0;
        return 0;
    }

    g_nJoystickRawX_005a81c0 = rawX;
    g_nJoystickRawY_005a81c4 = rawY;
    if (rawX < g_nJoystickMinimumX_005a81b8)
        sample->x = g_nJoystickMinimumX_005a81b8;
    if (sample->x > g_nJoystickMaximumX_005a81b0)
        sample->x = g_nJoystickMaximumX_005a81b0;
    if (g_aInputDeviceSamples_005a81f0[deviceIndex].y <
        g_nJoystickMinimumY_005a81bc)
        g_aInputDeviceSamples_005a81f0[deviceIndex].y =
            g_nJoystickMinimumY_005a81bc;
    if (g_aInputDeviceSamples_005a81f0[deviceIndex].y >
        g_nJoystickMaximumY_005a81b4)
        g_aInputDeviceSamples_005a81f0[deviceIndex].y =
            g_nJoystickMaximumY_005a81b4;

    normalizedY = 0;
    normalizedX = 0;
    rawX = sample->x;
    if (g_nJoystickCentreX_005a81dc > rawX) {
        normalizedX = (short)((g_nJoystickCentreX_005a81dc - rawX) /
                              g_nJoystickLeftScale_005a81ac);
        if (g_nJoystickHorizontalDeadZone_005a81a4 < normalizedX) {
            if (normalizedX != 0)
                normalizedX = (short)-normalizedX;
        } else {
            normalizedX = 0;
        }
    } else if (g_nJoystickCentreX_005a81dc < rawX) {
        normalizedX = (short)((rawX - g_nJoystickCentreX_005a81dc) /
                              g_nJoystickRightScale_005a81d0);
        if (normalizedX <= g_nJoystickHorizontalDeadZone_005a81a4)
            normalizedX = 0;
    }

    rawY = g_aInputDeviceSamples_005a81f0[deviceIndex].y;
    if (g_nJoystickCentreY_005a81d8 > rawY) {
        normalizedY = (short)((g_nJoystickCentreY_005a81d8 - rawY) /
                              g_nJoystickUpScale_005a81a8);
        if (g_nJoystickVerticalDeadZone_005a81a0 < normalizedY) {
            if (normalizedY != 0)
                normalizedY = (short)-normalizedY;
        } else {
            normalizedY = 0;
        }
    } else if (g_nJoystickCentreY_005a81d8 < rawY) {
        normalizedY = (short)((rawY - g_nJoystickCentreY_005a81d8) /
                              g_nJoystickDownScale_005a81d4);
        if (normalizedY <= g_nJoystickVerticalDeadZone_005a81a0)
            normalizedY = 0;
    }
    sample->x = (int)normalizedX;
    g_aInputDeviceSamples_005a81f0[deviceIndex].y = (int)normalizedY;
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

    if (DAT_00465070 != 0) {
        sprintf((char *)DAT_00476620, "%f", DAT_00486510);
        TextOutA(dc, 0, 0, (char *)DAT_00476620,
                 strlen((char *)DAT_00476620));
    }

    if (g_stMouseCursorState_0059ab10.viewport->pixels == DAT_00476648) {
        while (timeGetTime() < (DWORD)DAT_0046b1bc) {
            Sleep(0);
            RefreshMouseCursorDisplay();
        }
    } else {
        while (timeGetTime() < (DWORD)DAT_0046b1bc)
            Sleep(0);
    }

    if (DAT_00465070 != 0) {
        if (DAT_0046b1c4 != 0) {
            now = timeGetTime();
            DAT_00486510 = 1000.0f / ((double)now - DAT_0046b1c4);
        }
        DAT_0046b1c4 = timeGetTime();
        sprintf((char *)DAT_00476620, "%f", DAT_00486510);
        TextOutA(dc, 0, 0, (char *)DAT_00476620,
                 strlen((char *)DAT_00476620));
    }

    DAT_0046b1bc = timeGetTime() + DAT_0046b1b8;
}
