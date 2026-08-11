/*
 *  Music state machine and the streaming music script.
 *
 *  Address range 0x42d000-0x42efff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: PROVEN by the names the routines print: StopMusic, FadeMusic, SetMusicOn, ....
 */
#include "wc1.h"

/* Function start: 0x42CDB0 */
unsigned int parse_view_script(void)
{
    const short *command;
    FixedVector direction;
    FixedVector velocity;
    short obj;

    if (*g_pViewScript_005a6b58 == 13)
        return 0;
    while (*g_pViewScript_005a6b58 != 14) {
        command = g_pViewScript_005a6b58++;
        switch (*command) {
        case -1:
            g_bScriptedView_0046a8d4 = 0;
            g_nScriptedViewObject_0046a8d0 = -1;
            return 0;
        case 0:
            g_aShipPosition_0059c490[61].x =
                (int)g_pViewScript_005a6b58[0] << 8;
            g_aShipPosition_0059c490[61].y =
                (int)g_pViewScript_005a6b58[1] << 8;
            g_aShipPosition_0059c490[61].z =
                (int)g_pViewScript_005a6b58[2] << 8;
            g_pViewScript_005a6b58 += 3;
            break;
        case 1:
            alter_yaw(g_pViewScript_005a6b58[0], 61);
            alter_pitch(g_pViewScript_005a6b58[1], 61);
            alter_roll(g_pViewScript_005a6b58[2], 61);
            g_pViewScript_005a6b58 += 3;
            break;
        case 2:
            ScaleFixedVector(&g_aShipForwardVector_0059bce0[61],
                             (int)*g_pViewScript_005a6b58 << 8,
                             &g_aShipVelocity_0059c010[61]);
            g_pViewScript_005a6b58++;
            break;
        case 3:
            force_view(*g_pViewScript_005a6b58++,
                       g_nScriptedViewObject_0046a8d0);
            break;
        case 4:
            g_nEyePitchGoal_0059d61c =
                (short)-g_pViewScript_005a6b58[0];
            g_nEyePitchRate_0046c004 = g_pViewScript_005a6b58[1];
            g_pViewScript_005a6b58 += 2;
            break;
        case 5:
            g_nEyePitchGoal_0059d61c = g_pViewScript_005a6b58[0];
            g_nEyePitchRate_0046c004 = g_pViewScript_005a6b58[1];
            g_pViewScript_005a6b58 += 2;
            break;
        case 6:
            g_nEyeYawGoal_0059c944 = g_pViewScript_005a6b58[0];
            g_nEyeYawRate_0046c008 = g_pViewScript_005a6b58[1];
            g_pViewScript_005a6b58 += 2;
            break;
        case 7:
            g_nEyeYawGoal_0059c944 =
                (short)-g_pViewScript_005a6b58[0];
            g_nEyeYawRate_0046c008 = g_pViewScript_005a6b58[1];
            g_pViewScript_005a6b58 += 2;
            break;
        case 8:
            g_nEyeRollGoal_0059c8f0 = g_pViewScript_005a6b58[0];
            g_nEyeRollRate_0046c00c = g_pViewScript_005a6b58[1];
            g_pViewScript_005a6b58 += 2;
            break;
        case 9:
            copy_frame(61, 63);
            alter_yaw(g_pViewScript_005a6b58[0], 63);
            alter_pitch(g_pViewScript_005a6b58[1], 63);
            alter_roll(g_pViewScript_005a6b58[2], 63);
            ScaleFixedVector(&g_aShipForwardVector_0059bce0[63],
                             (int)g_pViewScript_005a6b58[3] << 8,
                             &velocity);
            AddFixedVectors(&g_aShipVelocity_0059c010[61], &velocity,
                            &g_aShipVelocity_0059c010[61]);
            g_pViewScript_005a6b58 += 4;
            break;
        case 10:
            if (g_nScriptedViewObject_0046a8d0 >= 0)
                g_aShipVelocity_0059c010[61] =
                    g_aShipVelocity_0059c010[g_nScriptedViewObject_0046a8d0];
            break;
        case 11:
            if (g_nScriptedViewObject_0046a8d0 >= 0)
                copy_frame(g_nScriptedViewObject_0046a8d0, 61);
            break;
        case 12:
            if (g_nScriptedViewObject_0046a8d0 >= 0)
                g_aShipPosition_0059c490[61] =
                    g_aShipPosition_0059c490[g_nScriptedViewObject_0046a8d0];
            break;
        case 15:
            if (g_nScriptedViewObject_0046a8d0 >= 0) {
                ComputeVectorDelta(&g_aShipPosition_0059c490[61],
                    &g_aShipPosition_0059c490[g_nScriptedViewObject_0046a8d0],
                    &direction);
                if (NormalizeFixedVector(&direction) != 0) {
                    g_aShipForwardVector_0059bce0[61] = direction;
                    fix_objects_ijk(61);
                }
            }
            break;
        case 16:
            obj = 0;
            while (obj < 10) {
                if (g_nShipMissionIndices_0059c830[obj] ==
                    *g_pViewScript_005a6b58)
                    break;
                obj++;
            }
            if (obj < 10)
                g_nScriptedViewObject_0046a8d0 = obj;
            g_pViewScript_005a6b58++;
            break;
        }
        if (*g_pViewScript_005a6b58 == 13)
            return 0;
    }
    g_asObjectCounter_0059c330[61] = g_pViewScript_005a6b58[1];
    return 0;
}

/* Function start: 0x42D1C0 */
unsigned int update_scripted_view(void)
{
    short counter;
    int command;

    command = *g_pViewScript_005a6b58;
    switch (command) {
    case 13:
        if ((g_nEyeYawGoal_0059c944 == g_nEyePitchGoal_0059d61c) !=
            g_nEyeRollGoal_0059c8f0) {
            g_pViewScript_005a6b58++;
            parse_view_script();
        }
        break;
    case 14:
        counter = g_asObjectCounter_0059c330[61];
        g_asObjectCounter_0059c330[61]--;
        if (counter < 1) {
            g_pViewScript_005a6b58 += 2;
            parse_view_script();
        }
        break;
    }
    return 0;
}

/* Function start: 0x42D230 */
void initialize_scripted_view(const short *script)
{
    g_bScriptedView_0046a8d4 = 1;
    zero_vector(&g_aShipVelocity_0059c010[61]);
    init_ijk(61);
    g_pViewScript_005a6b58 = script;
    parse_view_script();
    g_asObjectCollisionRadius_0059d710[61] = 100;
}

/* Function start: 0x42D270 */
unsigned int InitializeFireworks(void)
{
    int empty = -1;
    short i = 0;

    do {
        g_aFireworks_005a6900[i].frame = (short)empty;
        i = i + 1;
    } while (i < 0x1e);
    return 0;
}

/* Function start: 0x42D2A0 */
short TheEndFireWorks(Viewport *viewport, short count)
{
    short index;
    short emptyCount;

    emptyCount = 0;
    index = count;
    while (--index >= 0) {
        if (g_aFireworks_005a6900[index].frame == -1) {
            emptyCount++;
        } else {
            DrawSpriteDefault(
                viewport, g_aFireworks_005a6900[index].x,
                g_aFireworks_005a6900[index].y,
                g_pFireworkShape_005a6a68,
                (short)(g_aFireworks_005a6900[index].frame +
                        g_aFireworks_005a6900[index].variant * 8));
            if (g_aFireworks_005a6900[index].frame++ == 7) {
                g_aFireworks_005a6900[index].frame = -1;
                ((void (__cdecl *)(int, short))FlushSoundEffectsAndLog)(
                    g_aFireworks_005a6900[index].soundHandle, index);
            } else if (g_aFireworks_005a6900[index].frame == 1) {
                g_aFireworks_005a6900[index].soundHandle =
                    ((unsigned int (__cdecl *)(
                        const unsigned char *, int, int, short, short,
                        int))SoundFxTick)(
                            g_abFireworkSoundDescriptor_0046ab70,
                            0, 127,
                            (signed char)(
                                127 -
                                (int)g_aFireworks_005a6900[index].x *
                                    127 / 319),
                            index, 1);
            }
        }
    }
    return emptyCount;
}

/* Function start: 0x42D390 */
unsigned int InitializeConstellationField(Viewport *viewport,
                                          short direction,
                                          short density)
{
    short height;
    short index;
    int particleIndex;
    short randomIndex;
    short width;

    g_pConstellationViewport_005a6aac = viewport;
    g_nConstellationDirection_0046a918 = direction;
    width = (short)(viewport->right - viewport->left);
    height = (short)(viewport->bottom - viewport->top);
    g_nConstellationStarCount_005a6ab0 = (short)(density * 10 / 16);
    g_nConstellationParticleCount_005a6b54 =
        (short)(density * 16 / 16);
    index = 0;
    while (index < g_nConstellationStarCount_005a6ab0) {
        g_aConstellationStars_005a6a70[index].x =
            RandomInRange(0, width);
        g_aConstellationStars_005a6a70[index].y =
            RandomInRange(0, height);
        g_aConstellationStars_005a6a70[index].frame =
            (short)(RandomInRange(0, 5) + 32);
        index++;
    }
    index = 0;
    while (index < g_nConstellationParticleCount_005a6b54) {
        randomIndex = RandomInRange(0, 15);
        particleIndex = index;
        index++;
        g_aConstellationParticles_005a6ac0[particleIndex].x =
            (short)(g_pConstellationViewport_005a6aac->left +
                    RandomInRange(0, width));
        g_aConstellationParticles_005a6ac0[particleIndex].y =
            (short)(g_pConstellationViewport_005a6aac->top +
                    RandomInRange(0, height));
        g_aConstellationParticles_005a6ac0[particleIndex].velocity =
            (short)(g_asConstellationVelocity_0046a8d8[randomIndex] *
                    g_nConstellationDirection_0046a918);
        g_aConstellationParticles_005a6ac0[particleIndex].frame =
            (short)(g_asConstellationFrame_0046a8f8[randomIndex] +
                    RandomInRange(0, 3));
    }
    return 0;
}

/* Function start: 0x42D500 */
unsigned int DrawConstellationField(void)
{
    ConstellationParticle *particle;
    short height;
    short index;
    short randomIndex;
    short speed;

    height = (short)(g_pConstellationViewport_005a6aac->bottom -
                     g_pConstellationViewport_005a6aac->top);
    ClearViewport(g_pConstellationViewport_005a6aac, DAT_004699d8);
    index = 0;
    while (index < g_nConstellationStarCount_005a6ab0) {
        DrawSpriteDefault(g_pConstellationViewport_005a6aac,
                          g_aConstellationStars_005a6a70[index].x,
                          g_aConstellationStars_005a6a70[index].y,
                          g_pConstellationShape_005a765c,
                          g_aConstellationStars_005a6a70[index].frame);
        index++;
    }
    index = 0;
    while (index < g_nConstellationParticleCount_005a6b54) {
        particle = &g_aConstellationParticles_005a6ac0[index];
        DrawSpriteDefault(g_pConstellationViewport_005a6aac,
                          particle->x, particle->y,
                          g_pConstellationShape_005a765c,
                          particle->frame);
        particle->x = (short)(particle->x + particle->velocity);
        particle->frame = (short)(
            (particle->frame & 0xfc) + (particle->frame + 1) % 4);
        if (g_nConstellationDirection_0046a918 < 0) {
            if (particle->x < g_pConstellationViewport_005a6aac->left) {
                randomIndex = RandomInRange(0, 15);
                speed = g_asConstellationVelocity_0046a8d8[randomIndex];
                particle->x = (short)(
                    g_pConstellationViewport_005a6aac->right -
                    RandomInRange(0, speed));
                particle->y = (short)(
                    g_pConstellationViewport_005a6aac->top +
                    RandomInRange(0, height));
                particle->velocity = (short)-speed;
                particle->frame = (short)(
                    g_asConstellationFrame_0046a8f8[randomIndex] +
                    RandomInRange(0, 3));
            }
        } else if (particle->x >
                   g_pConstellationViewport_005a6aac->right) {
            randomIndex = RandomInRange(0, 15);
            speed = g_asConstellationVelocity_0046a8d8[randomIndex];
            particle->velocity = speed;
            particle->x = (short)(
                g_pConstellationViewport_005a6aac->left +
                RandomInRange(0, speed));
            particle->y = (short)(
                g_pConstellationViewport_005a6aac->top +
                RandomInRange(0, height));
            particle->frame = (short)(
                g_asConstellationFrame_0046a8f8[randomIndex] +
                RandomInRange(0, 3));
        }
        index++;
    }
    return 0;
}

/* Function start: 0x42D730 */
short __stdcall OpenPacketSection(const char *filename, short section,
                                  PacketSectionHandle *handle)
{
    unsigned int sectionEntry;
    unsigned int fileSize;
    unsigned int nextEntry;
    int sectionIndex;
    unsigned int directorySize;
    short compression;
    short sectionCount;
    short file;

    file = OpenDataFileOrDie(filename);
    if (file == -1)
        goto failed;
    if (ReadDataFileAtOffset(file, 0, 4, &fileSize) == 0)
        goto failed;
    if (ReadDataFileAtOffset(file, 4, 4, &directorySize) == 0)
        goto failed;
    sectionCount = (short)(directorySize >> 2) - 1;
    if (section >= sectionCount) {
        DAT_00465460 = 3;
        goto failed;
    }
    sectionIndex = (int)section;
    if (ReadDataFileAtOffset(file, sectionIndex * 4 + 4, 4,
                             &sectionEntry) == 0)
        goto failed;
    handle->finalSection = 0;
    compression = (short)(sectionEntry >> 24);
    if (sectionCount - sectionIndex == 1) {
        handle->finalSection = 1;
        nextEntry = fileSize;
        sectionEntry &= 0x00ffffff;
    } else {
        sectionEntry &= 0x00ffffff;
        if (ReadDataFileAtOffset(file, sectionIndex * 4 + 8, 4,
                                 &nextEntry) == 0)
            goto failed;
        nextEntry &= 0x00ffffff;
    }
    fileSize = nextEntry - sectionEntry;
    if (SeekDataFile(file, sectionEntry, 0) != 0) {
        handle->file = file;
        handle->dataOffset = sectionEntry;
        handle->sectionCount = sectionCount;
        handle->compression = compression;
        handle->dataSize = fileSize;
        handle->position = 0;
        return 1;
    }
failed:
    CloseDataFile((unsigned short)file);
    return 0;
}

/* Function start: 0x42D870 */
void __stdcall CloseDataFileByHandle(unsigned short *p)
{
    CloseDataFile(*p);
}

/* Function start: 0x42DB70 */
short GetTargetColourIndex(void)
{
    short v = (short)DAT_0046b168;

    if ((short)DAT_0046b168 == -1)
        v = 0x13;
    return v;
}

/* Function start: 0x42DB90 */
void show_target_disp(void)
{
    short target;
    int targetIndex;
    ObjectTypeData *typeData;
    enum ObjectType objectType;
    signed char rating;
    short x;
    short y;
    short frame;
    short *maximumArmor;
    short armor;
    Viewport targetViewport;

    DrawTextAt(&DAT_005a7700, DAT_005a7530.left, DAT_005a7530.top,
               g_szEmptyTargetDisplayText_0046a948, 2);
    if (g_nTargetLockMode_0046c078 != 0) {
        DrawFormattedText(g_szTextColourStringColourFormat_0046a960,
                          (unsigned int)DAT_004699ac,
                          g_szLockedTarget_0046a94c,
                          (unsigned int)g_cDefaultTextColour_004699cc);
    } else {
        DrawFormattedText(g_szTextColourStringFormat_0046a97c,
                          (unsigned int)g_cDefaultTextColour_004699cc,
                          g_szAutoTargetting_0046a968);
    }
    target = (short)g_acShipTarget_0059ce60[0];
    if (target != -1 &&
        (g_aeObjectClass_0059d100[target] < OBJECT_CLASS_SHIP ||
         g_aeSpecialManeuver_0059c3c0[target] ==
             SPECIAL_MANEUVER_UNKNOWN_9)) {
        target = -1;
        g_acShipTarget_0059ce60[0] = -1;
    }
    g_cTargetDisplayObject_0046c06c = (signed char)target;
    DrawFormattedText(g_szTargetLabel_0046a984);
    if (target == -1) {
        DrawFormattedText(g_szNoTarget_0046a990);
        return;
    }
    targetIndex = (int)target;
    objectType = g_aeObjectType_0059b560[targetIndex];
    typeData = &g_aObjectTypeData_00466458[objectType];
    rating = g_acShipRating_0059cd80[targetIndex];
    if (rating >= 0 && rating <= 7) {
        DrawFormattedText(
            g_szWingmanTargetNameFormat_0046a998,
            g_apWingmanPilots_00598a30[(int)rating]->callsign);
    } else if (rating >= 9 && rating <= 12) {
        DrawFormattedText(
            g_szAceTargetNameFormat_0046a99c,
            g_apszKilrathiAceNames_0046af80[(int)rating - 9]);
    } else {
        DrawFormattedText(g_szShipTargetNameFormat_0046a9a0,
                          typeData->displayName);
    }
    DrawFormattedText(g_szRangeLabel_0046a9a4);
    InitializeCockpitReadout(1, &DAT_005a7700);
    if (g_asObjectScreenX_0059d9b0[targetIndex] == (short)0x8001) {
        g_cTargetDisplayObject_0046c06c = -1;
        return;
    }

    x = (short)(DAT_005a7530.left + 0x25);
    y = (short)(DAT_005a7530.top + 0x26);
    frame = (short)((3 - MinShort(
        (short)((g_aasShipShield_0059d5b0[targetIndex][1] * 6) /
                typeData->shieldAft), 3)) * 2);
    if (frame < 6)
        DrawSpriteDefault(&DAT_005a7530, x, y,
                          g_pCockpitIndicatorShape_005a7658, frame);

    targetViewport = DAT_005a7530;
    maximumArmor = &typeData->armorFront;
    armor = 0;
    do {
        targetViewport.left =
            (short)(g_aTargetArmorClipRects_0046a928[armor].left + x);
        targetViewport.top =
            (short)(g_aTargetArmorClipRects_0046a928[armor].top + y);
        targetViewport.right =
            (short)(g_aTargetArmorClipRects_0046a928[armor].right + x);
        targetViewport.bottom =
            (short)(g_aTargetArmorClipRects_0046a928[armor].bottom + y);
        if (g_aasShipArmor_0059d420[targetIndex][armor] >
            (short)(maximumArmor[armor] >> 1)) {
            DrawSpriteDefault(&targetViewport, x, y,
                              typeData->shape, 0);
        } else {
            DrawSpriteDefault(&targetViewport, x, y,
                              typeData->shape, 1);
        }
        armor++;
    } while (armor < 4);

    DrawSpriteDefault(&DAT_005a7530, x, y, typeData->shape, 2);
    frame = (short)((3 - MinShort(
        (short)((g_aasShipShield_0059d5b0[targetIndex][0] * 6) /
                typeData->shieldFore), 3)) * 2);
    if (frame < 6)
        DrawSpriteDefault(&DAT_005a7530, x, y,
                          g_pCockpitIndicatorShape_005a7658,
                          (short)(frame + 1));
}

/* Function start: 0x42DEA0 */
void DrawTargetRangeReadout(void)
{
    short target;
    const char *rangeText;

    target = (short)g_acShipTarget_0059ce60[0];
    if (g_aeSpecialManeuver_0059c3c0[target] ==
        SPECIAL_MANEUVER_UNKNOWN_9) {
        g_acShipTarget_0059ce60[0] = -1;
        InvalidateVduMode(1);
        return;
    }
    if (target != -1 &&
        g_aeObjectClass_0059d100[target] < OBJECT_CLASS_SHIP) {
        g_acShipTarget_0059ce60[0] = -1;
        target = -1;
    }
    if (g_cTargetDisplayObject_0046c06c != target ||
        (short)(g_nRenderedSpaceFrame_0059d61a % 8) == 0) {
        set_new_vdu(1);
        show_target_disp();
    }
    if (target == -1)
        return;

    if (g_asObjectScreenX_0059d9b0[target] == (short)0x8001) {
        rangeText = g_szTargetOffscreenRange_0046a9bc;
    } else if ((unsigned short)g_asObjectDistance_0059b4a0[target] <=
               30000) {
        strcat(_itoa((unsigned short)g_asObjectDistance_0059b4a0[target],
                     g_szTextScratchBuffer_00598b00, 10), " m");
        goto draw_readout;
    } else {
        rangeText = g_szTargetTooFar_0046a9b0;
    }
    memcpy(g_szTextScratchBuffer_00598b00, rangeText, 8);

draw_readout:
    DrawCockpitReadout(1, g_szTextScratchBuffer_00598b00);
    if (g_nTargetLockCountdown_0046c064 == 0) {
        if (g_bTargetLockAcquired_0046c074 == 1) {
            g_bTargetLockAcquired_0046c074 = 0;
            return;
        }
    } else if (g_bTargetLockReadoutDirty_0046c060 != 0) {
        EraseCockpitReadoutRegion(&DAT_005a7530,
                                  DAT_005a7530.left,
                                  (short)(DAT_005a7530.bottom - 6),
                                  DAT_005a7530.right,
                                  DAT_005a7530.bottom,
                                  (short)DAT_0046999c);
        g_bTargetLockReadoutDirty_0046c060 = 0;
    }
}

/* Function start: 0x42E020 */
void LogDisplayMode(const char *mode)
{
    SystemDebugPrintf("display_mode == '%s'\n", mode);
    ClearDebugPauseFlags();
    PumpMessagesDuringWait();
    exit(1);
}

/* Function start: 0x42E090 */
unsigned short __stdcall AllocateViewport(Viewport *viewport,
                                          short clearColour, short flags)
{
    unsigned short *rowOffsets;
    unsigned short top;
    short left;
    unsigned short width;
    unsigned short height;
    unsigned short row;
    short offset;

    top = (unsigned short)viewport->top;
    height = (unsigned short)(viewport->bottom - top + 1);
    left = viewport->left;
    width = (unsigned short)(viewport->right - left + 1);
    g_nAllocateViewportCalls_005a68ec++;
    if (DAT_0046b168 != 0x13)
        LogDisplayMode("not MCGA");
    viewport->allocation = (unsigned char *)AllocateTaggedMemory(
        (unsigned int)width * height, (unsigned short)(flags + 2));
    g_apViewportAllocations_005a7f10[
        g_nViewportAllocationCount_005a7f0c++] = viewport->allocation;
    if (viewport->allocation == 0)
        return 0;
    viewport->pixels = viewport->allocation;

    rowOffsets = (unsigned short *)AllocateTaggedMemory(
        (top + (unsigned int)height) * sizeof(unsigned short) + 4,
        0);
    viewport->rowOffsets = rowOffsets;
    if (rowOffsets == 0) {
        if (DAT_0046b168 != 0x13)
            ReleasePacketHandle((int)viewport->allocation);
        return 0;
    }

    row = 0;
    offset = 0;
    do {
        rowOffsets[top + row] = (unsigned short)(offset - left);
        row++;
        offset = (short)(offset + width);
    } while ((unsigned int)row < (unsigned int)height + 2);
    if (clearColour != -1)
        ClearViewport(viewport, (unsigned char)clearColour);
    return 1;
}

/* Function start: 0x42E1D0 */
void __stdcall AlignSpriteFrameToRectCorner(
    const ShortRect *rectangle, ShortPoint *position, short corner,
    unsigned char *shape, short frame)
{
    ShortRect frameBounds;
    short x;
    short y;

    switch (corner) {
    case 0:
        x = rectangle->left;
        y = rectangle->top;
        break;
    case 1:
        x = rectangle->right;
        y = rectangle->top;
        break;
    case 2:
        x = rectangle->left;
        y = rectangle->bottom;
        break;
    case 3:
        x = rectangle->right;
        y = rectangle->bottom;
        break;
    }

    GetShapeFrameBounds(&frameBounds.left, x, y, shape, frame);
    position->x = x;
    position->y = y;
    switch (corner) {
    case 0:
        position->x = (short)(x * 2 - frameBounds.left);
        position->y = (short)(y * 2 - frameBounds.top);
        return;
    case 1:
        position->x = (short)(x * 2 - frameBounds.right);
        position->y = (short)(y * 2 - frameBounds.top);
        return;
    case 2:
        position->x = (short)(x * 2 - frameBounds.left);
        position->y = (short)(y * 2 - frameBounds.bottom);
        return;
    case 3:
        position->x = (short)(x * 2 - frameBounds.right);
        position->y = (short)(y * 2 - frameBounds.bottom);
        return;
    }
}

/* Function start: 0x42E320 */
void FadeMusic(void)
{
    SoundDebugPrintf("FadeMusic");
}

/* Function start: 0x42E330 */
void SetMusicOn(short enabled)
{
    SoundDebugPrintf("SetMusicOn %d", (int)enabled);
}

/* Function start: 0x42E350 */
void StopMusic(void)
{
    SoundDebugPrintf("StopMusic");
    DAT_0046aa14 = 0xffffffff;
    Streamer_stop();
    SoundDebugPrintf("");
}

/* Function start: 0x42E380 */
void SetMusBreakpt(void)
{
    SoundDebugPrintf("SetMusBreakpt");
}

/* Function start: 0x42E390 */
void PaletteFadeHook(void)
{
}

/* Function start: 0x42E3A0 */
void FlushSoundEffect(void)
{
    stop_all_sounds();
    SoundDebugPrintf("FlushSoundEffect");
}

/* Function start: 0x42E3C0 */
void FlushSoundEffects(void)
{
    stop_all_sounds();
    SoundDebugPrintf("FlushSoundEffects");
}

/* Function start: 0x42E3E0 */
void SceneLeaveHook(void)
{
}

/* Function start: 0x42E3F0 */
void SelectFlightMusicTrack(int track)
{
    const char *streamName;
    int streamSet;

    switch (track) {
    case 0:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 27:
    case 31:
    case 32:
        streamSet = 2;
        break;
    case 1:
        streamSet = 2;
        SoundDebugPrintf("being tailed");
        break;
    case 19:
        SoundDebugPrintf("ofx music");
        break;
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 30:
    case 35:
        streamSet = 0;
        break;
    case 28:
    case 29:
    case 33:
    case 34:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
        streamSet = 1;
        break;
    default:
        streamSet = -1;
        break;
    }

    if (streamSet == g_nMusicStreamSet_0046aa18)
        return;
    if (streamSet == -1) {
        if (g_nMusicStreamSet_0046aa18 >= 0)
            Streamer_close();
        return;
    }

    if (streamSet == 0) {
        streamName = "preflite.str";
    } else if (streamSet == 1) {
        streamName = "posflite.str";
    } else if (streamSet == 2) {
        streamName = "mission.str";
        ClearStreamerTrigger();
    } else {
        streamName = 0;
    }

    if (streamName != 0) {
        Streamer_open(streamName);
        g_nMusicStreamSet_0046aa18 = streamSet;
    } else {
        Streamer_close();
        g_nMusicStreamSet_0046aa18 = -1;
    }
}

/* Function start: 0x42E520 */
int MapMusicTrackToStreamerCommand(int track)
{
    switch (track) {
    case 0:
        return 5;
    case 1:
        SoundDebugPrintf("being tailed");
        return 7;
    case 2:
        return 7;
    case 3:
        return 8;
    case 4:
        return 9;
    case 5:
        return 6;
    case 6:
        return 15;
    case 7:
        return 13;
    case 8:
        return 16;
    case 9:
        return 14;
    case 10:
        return 17;
    case 11:
        return 18;
    case 12:
        return 10;
    case 13:
        return 12;
    case 14:
        return 11;
    case 15:
        return 4;
    case 16:
        return 3;
    case 17:
        return 1;
    case 18:
        return 2;
    case 19:
        SoundDebugPrintf("ofx music");
        return -1;
    case 20:
        return 1;
    case 21:
        return 4;
    case 22:
        return 3;
    case 23:
        return -1;
    case 24:
        return 5;
    case 25:
        return 6;
    case 26:
        return 7;
    case 27:
    case 28:
        return -1;
    case 29:
    case 30:
        return 0;
    case 31:
        return 19;
    case 32:
        return 20;
    case 33:
        return 2;
    case 34:
        return 1;
    case 35:
        return 2;
    case 36:
        return 3;
    case 37:
        return 4;
    case 38:
        return 5;
    case 39:
        return 7;
    case 40:
        return 6;
    default:
        return -1;
    }
}

/* Function start: 0x42E6F0 */
void ProcessMusicScriptCommand(int track, int command, int enabled)
{
    int streamerCommand;

    (void)enabled;
    if (track == -1 || g_bMusicCommandSuppressed_0046a9fc != 0)
        return;
    if (command == 4) {
        SoundDebugPrintf("queue_stop\n");
        StopMusic();
        DAT_0046aa14 = 0xffffffff;
        return;
    }

    SoundDebugPrintf("track_%02d ", track);
    if ((DAT_0046aa14 == 25 && track == 25) ||
        (DAT_0046aa14 == 38 && track == 38) ||
        (DAT_0046aa14 == 39 && track == 39) ||
        (DAT_0046aa14 == 40 && track == 40)) {
        SoundDebugPrintf("skipping for QA\n");
        return;
    }

    DAT_0046aa14 = track;
    SelectFlightMusicTrack(track);
    if (g_nMusicStreamSet_0046aa18 == 2) {
        if ((track >= 0 && track <= 5) ||
            (track >= 12 && track <= 18)) {
            SoundDebugPrintf("flight_intensity %d ", track);
            SetStreamerIntensity((unsigned char)track);
        } else {
            SoundDebugPrintf("flight_trigger %d ", track);
            Streamer_trigger(track);
        }
        SoundDebugPrintf("\n");
        return;
    }

    switch (command) {
    case 0:
        SoundDebugPrintf(" queue_start ");
        streamerCommand = MapMusicTrackToStreamerCommand(track);
        Streamer_trigger(streamerCommand);
        break;
    case 1:
        SoundDebugPrintf(" queue_break ");
        streamerCommand = MapMusicTrackToStreamerCommand(track);
        ForceStreamerTrigger(streamerCommand);
        break;
    case 2:
        SoundDebugPrintf(" queue_switch ");
        streamerCommand = MapMusicTrackToStreamerCommand(track);
        Streamer_trigger(streamerCommand);
        break;
    case 3:
        SoundDebugPrintf(" queue_interrupt ");
        streamerCommand = MapMusicTrackToStreamerCommand(track);
        ForceStreamerTrigger(streamerCommand);
        break;
    }
    SoundDebugPrintf("\n");
}

/* Function start: 0x42E880 */
unsigned int StartMusicTrack(int track, int mode, int enabled)
{
    if (DAT_0046a9f8 != 0 && DAT_0046a9f8 != 3)
        ProcessMusicScriptCommand(track, mode, enabled);
    return 1;
}

/* Function start: 0x42E8B0 */
void StopMusicUnlessSuppressed(void)
{
    if (DAT_0046a9f8 != 0 && DAT_0046a9f8 != 3)
        StopMusic();
}

/* Function start: 0x42E8D0 */
unsigned short GetMusicMode(void)
{
    if (DAT_0046a9f8 != 0 && DAT_0046a9f8 != 3 && DAT_0046aa04 != 0)
        return 1;
    return 0;
}

/* Function start: 0x42E9E0 */
void new_space_music_changes(short attacker, short victim)
{
    enum Side side;

    if (DAT_0046aa40 != 0 && g_nTrainSimActive_00469e2c == 0) {
        side = g_aeShipSide_0059d650[victim];
        if (side == SIDE_KILRATHI) {
            if (report_kilrathi_rout(1) == 0) {
                StartMusicTrack(10, 1, 0);
                return;
            }
            if (attacker == 0) {
                if (g_acShipRating_0059cd80[victim] == -1 &&
                    (short)RandomInRange(0, 3) != 0) {
                    StartMusicTrack(6, 3, 0);
                    return;
                }
                StartMusicTrack(9, 3, 0);
                return;
            }
        } else {
            if (g_nYourWingman_0046c04c == victim) {
                StartMusicTrack(8, 3, 0);
                return;
            }
            if (side == SIDE_IMPERIAL) {
                if ((g_aeShipMissionType_0059c3f0[0] ==
                         MISSION_TYPE_DEFEND ||
                     g_aeShipMissionType_0059c3f0[0] ==
                         MISSION_TYPE_ESCORT) &&
                    g_nShipMissionIndices_0059c830[victim] ==
                        g_anShipMissionShip_0059d4b0[0]) {
                    StartMusicTrack(11, 3, 0);
                    return;
                }
                StartMusicTrack(7, 3, 0);
            }
        }
    }
}

/* Function start: 0x42EE80 */
void ResetSoundState(void)
{
    FlushSoundEffects();
    DAT_005a7cec = 0;
    DAT_005a7ec0 = 0;
}

/* Function start: 0x42EEA0 */
void ResetSoundStateForScene(void)
{
    ResetSoundState();
    DAT_0046aa34 = 0;
}

/* Function start: 0x42EEB0 */
void ResetSoundStateForFlight(void)
{
    ResetSoundState();
    DAT_0046aa34 = 1;
}

/* Function start: 0x42EEE0 */
void EnableMusicForScene(void)
{
    DAT_0046aa30 = 1;
    SetMusicOn(1);
}

/* Function start: 0x42EF00 */
unsigned int SoundFxTick(void)
{
    WriteDebugString("soundFX");
    return 0;
}

/* Function start: 0x42EF10 */
__declspec(naked) void FlushSoundEffectsAndLog(void)
{
    __asm { jmp FlushSoundEffects }
}

/* Function start: 0x42EF20 */
void sound_effect(void)
{
    WriteDebugString("sound_effect");
}

/* Function start: 0x42EF30 */
void PlaySfxWaveFileByNumber(int soundNumber, int sourceObject, int looping)
{
    FixedVector delta;
    int distance;

    if (sourceObject != -1) {
        ComputeVectorDelta(&g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                           &g_aShipPosition_0059c490[sourceObject],
                           &delta);
        distance = Vector_magnitude(&delta);
        if (distance > 32000)
            distance = 32000;
    } else {
        distance = 32000;
    }
    if (distance >= 10) {
        g_aiSoundEffectSourceActive_005a66ec[sourceObject + 1] = 1;
        sprintf(g_szSfxWavePath_00476558, g_szSfxWaveFormat_0046ad2c,
                soundNumber - 1);
        playWAVE((unsigned char *)g_szSfxWavePath_00476558,
                 looping, distance);
    }
}
