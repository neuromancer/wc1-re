/*
 *  Music state machine and the streaming music script.
 *
 *  Address range 0x42d000-0x42efff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: PROVEN by the names the routines print: StopMusic, FadeMusic, SetMusicOn, ....
 */
#include "wc1.h"

static int g_nActiveMusicStreamMask_0049be9c = -1;
static signed char g_acMusicTrackStreamFlags_0049beb8[72] = {
    6, 6, 6, 6, 6, 6, 10, 10, 10, 10, 10, 10,
    6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0,
    0, 0, 0, 6, 16, 16, 0, 6, 6, 16, 16, 0,
    16, 16, 16, 16, 16, 6, 10, 1, 1, 1, 1, 7,
    1, 1, 1, 1, 1, 1, 1, 6, 6, 6, 1, 16,
    16, 1, 1, 16, 16, 16, 0, 0, 0, 0, 0, 0
};

/* Function start: WC2_UNMAPPED */
unsigned int parse_view_script(void)
{
    FixedVector vector;
    short obj;
    short command;

    if (*g_pViewScript_005a6b58 == 13)
        return 0;
    while (*g_pViewScript_005a6b58 != 14) {
        command = *g_pViewScript_005a6b58++;
        if (command == -1) {
            g_bScriptedView_0046a8d4 = 0;
            g_nScriptedViewObject_0046a8d0 = -1;
            return 0;
        }
        switch (command) {
        case 0:
#ifdef WC1_SDL
            g_aShipPosition_00494550[61].x =
                (int)*g_pViewScript_005a6b58++ * 0x100;
            g_aShipPosition_00494550[61].y =
                (int)*g_pViewScript_005a6b58++ * 0x100;
            g_aShipPosition_00494550[61].z =
                (int)*g_pViewScript_005a6b58++ * 0x100;
#else
            g_aShipPosition_00494550[61].x =
                (int)*g_pViewScript_005a6b58++ << 8;
            g_aShipPosition_00494550[61].y =
                (int)*g_pViewScript_005a6b58++ << 8;
            g_aShipPosition_00494550[61].z =
                (int)*g_pViewScript_005a6b58++ << 8;
#endif
            break;
        case 1:
            alter_yaw(*g_pViewScript_005a6b58++, 61);
            alter_pitch(*g_pViewScript_005a6b58++, 61);
            alter_roll(*g_pViewScript_005a6b58++, 61);
            break;
        case 2:
#ifdef WC1_SDL
            ScaleFixedVector(&g_aShipForwardVector_00494208[61],
                             (int)*g_pViewScript_005a6b58++ * 0x100,
                             &g_aShipVelocity_0059c010[61]);
#else
            ScaleFixedVector(&g_aShipForwardVector_00494208[61],
                             (int)*g_pViewScript_005a6b58++ << 8,
                             &g_aShipVelocity_0059c010[61]);
#endif
            break;
        case 3:
            force_view(*g_pViewScript_005a6b58++,
                       g_nScriptedViewObject_0046a8d0);
            break;
        case 4:
            g_nEyePitchGoal_0059d61c =
                (short)-*g_pViewScript_005a6b58++;
            g_nEyePitchRate_0046c004 = *g_pViewScript_005a6b58++;
            break;
        case 5:
            g_nEyePitchGoal_0059d61c = *g_pViewScript_005a6b58++;
            g_nEyePitchRate_0046c004 = *g_pViewScript_005a6b58++;
            break;
        case 6:
            g_nEyeYawGoal_0059c944 = *g_pViewScript_005a6b58++;
            g_nEyeYawRate_0046c008 = *g_pViewScript_005a6b58++;
            break;
        case 7:
            g_nEyeYawGoal_0059c944 =
                (short)-*g_pViewScript_005a6b58++;
            g_nEyeYawRate_0046c008 = *g_pViewScript_005a6b58++;
            break;
        case 8:
            g_nEyeRollGoal_0059c8f0 = *g_pViewScript_005a6b58++;
            g_nEyeRollRate_0046c00c = *g_pViewScript_005a6b58++;
            break;
        case 9:
            copy_frame(61, 63);
            alter_yaw(*g_pViewScript_005a6b58++, 63);
            alter_pitch(*g_pViewScript_005a6b58++, 63);
            alter_roll(*g_pViewScript_005a6b58++, 63);
#ifdef WC1_SDL
            ScaleFixedVector(&g_aShipForwardVector_00494208[63],
                             (int)*g_pViewScript_005a6b58++ * 0x100,
                             &vector);
#else
            ScaleFixedVector(&g_aShipForwardVector_00494208[63],
                             (int)*g_pViewScript_005a6b58++ << 8,
                             &vector);
#endif
            AddFixedVectors(&g_aShipVelocity_0059c010[61], &vector,
                            &g_aShipVelocity_0059c010[61]);
            break;
        case 10:
            g_aShipVelocity_0059c010[61] =
                g_aShipVelocity_0059c010[g_nScriptedViewObject_0046a8d0];
            break;
        case 11:
            copy_frame(g_nScriptedViewObject_0046a8d0, 61);
            break;
        case 12:
            g_aShipPosition_00494550[61] =
                g_aShipPosition_00494550[g_nScriptedViewObject_0046a8d0];
            break;
        case 15:
            ComputeVectorDelta(&g_aShipPosition_00494550[61],
                &g_aShipPosition_00494550[g_nScriptedViewObject_0046a8d0],
                &vector);
            g_aShipForwardVector_00494208[61] = vector;
            fix_objects_ijk(61);
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
    g_asObjectCounter_00494be0[61] = g_pViewScript_005a6b58[1];
    return 0;
}

/* Function start: WC2_UNMAPPED */
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
        counter = g_asObjectCounter_00494be0[61];
        g_asObjectCounter_00494be0[61]--;
        if (counter < 1) {
            g_pViewScript_005a6b58 += 2;
            parse_view_script();
        }
        break;
    }
    return 0;
}

/* Function start: WC2_UNMAPPED */
void initialize_scripted_view(const short *script)
{
    g_bScriptedView_0046a8d4 = 1;
    zero_vector(&g_aShipVelocity_0059c010[61]);
    init_ijk(61);
    g_pViewScript_005a6b58 = script;
    parse_view_script();
    g_asObjectCollisionRadius_0059d710[61] = 100;
}

/* Function start: 0x45A2C0 */
void InitializeFireworks(void)
{
#if 0
    int empty = -1;
    short i = 0;

    do {
        g_aFireworks_005c8df0[i].frame = (short)empty;
        i = i + 1;
    } while (i < 0x1e);
    return 0;
#else
    short i;

    for (i = 0; i < 0x1e; i++)
        g_aFireworks_005c8df0[i].frame = -1;
#endif
}

/* Function start: 0x45A300 */
short TheEndFireWorks(Viewport *viewport, short count)
{
#if 0
    short index;
    short emptyCount;

    emptyCount = 0;
    index = count;
    while (--index >= 0) {
        if (g_aFireworks_005c8df0[index].frame == -1) {
            emptyCount++;
        } else {
            DrawSpriteDefault(
                viewport, g_aFireworks_005c8df0[index].x,
                g_aFireworks_005c8df0[index].y,
                g_pFireworkShape_005a6a68,
                (short)(g_aFireworks_005c8df0[index].frame +
                        g_aFireworks_005c8df0[index].variant * 8));
            if (g_aFireworks_005c8df0[index].frame++ == 7) {
                g_aFireworks_005c8df0[index].frame = -1;
                ((void (__cdecl *)(int, short))FlushSoundEffectsAndLog)(
                    g_aFireworks_005c8df0[index].soundHandle, index);
            } else if (g_aFireworks_005c8df0[index].frame == 1) {
                g_aFireworks_005c8df0[index].soundHandle =
                    ((unsigned int (__cdecl *)(
                        const unsigned char *, int, int, short, short,
                        int))SoundFxTick)(
                            g_abFireworkSoundDescriptor_0046ab70,
                            0, 127,
                            (signed char)(
                                127 -
                                (int)g_aFireworks_005c8df0[index].x *
                                    127 / 319),
                            index, 1);
            }
        }
    }
    return emptyCount;
#else
    short emptyCount;
    short index;

    emptyCount = 0;
    for (index = (short)(count - 1); index >= 0; index--) {
        if (g_aFireworks_005c8df0[index].frame == -1) {
            emptyCount++;
        } else {
            DrawSpriteDefault(
                viewport, g_aFireworks_005c8df0[index].x,
                g_aFireworks_005c8df0[index].y,
                g_pFireworkShape_005a6a68,
                (short)(g_aFireworks_005c8df0[index].frame +
                        g_aFireworks_005c8df0[index].variant * 8));
            if (g_aFireworks_005c8df0[index].frame++ == 7) {
                g_aFireworks_005c8df0[index].frame = -1;
                ((void (__cdecl *)(int, short))FlushSoundEffectsAndLog)(
                    g_aFireworks_005c8df0[index].soundHandle, index);
            } else if (g_aFireworks_005c8df0[index].frame == 1) {
                PlaySfxWaveFileByNumber(0x25, -1, 0);
            }
        }
    }
    return emptyCount;
#endif
}

/* Function start: 0x45A441 */
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

/* Function start: 0x45A634 */
unsigned int DrawConstellationField(void)
{
    ConstellationParticle *particle;
    short height;
    short index;
    short randomIndex;
    short speed;

    height = (short)(g_pConstellationViewport_005a6aac->bottom -
                     g_pConstellationViewport_005a6aac->top);
    ClearViewport(g_pConstellationViewport_005a6aac, g_cPrimaryViewBufferColour_0049cb88);
    for (index = 0;
         index < g_nConstellationStarCount_005a6ab0;
         index++) {
        DrawSpriteDefault(g_pConstellationViewport_005a6aac,
                          g_aConstellationStars_005a6a70[index].x,
                          g_aConstellationStars_005a6a70[index].y,
                          g_pConstellationShape_005a765c,
                          g_aConstellationStars_005a6a70[index].frame);
    }
    for (index = 0;
         index < g_nConstellationParticleCount_005a6b54;
         index++) {
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
    }
    return 0;
}

/* Function start: 0x44F010 */
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
        g_nPacketError_0049ca90 = 3;
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

/* Function start: 0x44F1C7 */
void __stdcall CloseDataFileByHandle(unsigned short *handle)
{
    CloseDataFile(*handle);
}

/* Function start: WC2_UNMAPPED */
void * __stdcall DecompressPacketSection(
    PacketSectionHandle *handle, void *destination, unsigned short flags,
    void *decompressionWorkspace)
{
    unsigned int allocationSize;
    void *packet;
    void *alignedWorkspace;
    void *largeScratch;
    void *smallScratch;
    int fallbackAllocations;

    fallbackAllocations = 0;
    packet = 0;
    g_pLastPacketAllocation_005a68f0 = 0;
    if (g_wPacketCompressionFormatFlags_0046a924 == 0) {
        if (handle->compression != 1) {
            g_nPacketError_0049ca90 = 6;
            return 0;
        }
    } else if ((handle->compression & 0xc0) != 0) {
        g_nPacketError_0049ca90 = 6;
        return 0;
    }

    if (decompressionWorkspace == 0) {
        if (g_pPacketDecompressionWorkspace_0046a91c != 0) {
            g_wPacketDecompressionInputSizeOverride_0046a920 = 0;
            decompressionWorkspace =
                g_pPacketDecompressionWorkspace_0046a91c;
        }
        if (decompressionWorkspace == 0) {
            largeScratch = AllocateTaggedMemory(0x3020, 0);
            smallScratch = AllocateTaggedMemory(0x410, 0);
            if (largeScratch == 0 || smallScratch == 0) {
                if (largeScratch != 0)
                    ReleasePacketHandle(largeScratch);
                if (smallScratch != 0)
                    ReleasePacketHandle(smallScratch);
                fallbackAllocations = 1;
                largeScratch = AllocateTaggedMemory(0x3000, 0x22);
                if (largeScratch == 0) {
                    g_nPacketError_0049ca90 = 1;
                    return 0;
                }
                smallScratch = AllocateTaggedMemory(0x400, 0x22);
                if (smallScratch == 0) {
                    ReleasePacketHandle(largeScratch);
                    g_nPacketError_0049ca90 = 2;
                    return 0;
                }
            }
            alignedWorkspace = IdentityHandle(largeScratch);
            g_pPacketDecompressInput_0059ab04 = IdentityHandle(smallScratch);
            g_wPacketDecompressInputSize_0059ab38 = 0x400;
            goto initializeDecompressor;
        }
    }

    alignedWorkspace = IdentityHandle(decompressionWorkspace);
    g_wPacketDecompressInputSize_0059ab38 = 0x400;
    allocationSize = 0x3020;
    g_pPacketDecompressInput_0059ab04 =
        ((PacketDecompressionWorkspace *)decompressionWorkspace)->input;
    g_pPacketDecompressInput_0059ab04 =
        IdentityHandle(g_pPacketDecompressInput_0059ab04);
    largeScratch = (void *)allocationSize;
    smallScratch = (void *)allocationSize;
    if (g_wPacketDecompressionInputSizeOverride_0046a920 != 0)
        g_wPacketDecompressInputSize_0059ab38 =
            g_wPacketDecompressionInputSizeOverride_0046a920;

initializeDecompressor:
    g_nPacketDecompressSourceFile_0059a858 = handle->file;
    g_nPacketDecompressInputPosition_0059ab00 = 0;
    g_nPacketDecompressPending_0059ab36 = 0;
    g_nPacketDecompressWorkspaceSegment_0059ab3a =
        (short)(unsigned int)alignedWorkspace;
    if (alignedWorkspace != 0)
        g_nPacketDecompressWorkspaceSegment_0059ab3a++;

    if (SeekPacketSection(handle, 0, 0) == -1) {
        g_nPacketError_0049ca90 = 5;
        return 0;
    }
    if (destination == 0) {
        if (ReadPacketSectionData(handle, &allocationSize, 4) != 0) {
            g_pLastPacketAllocation_005a68f0 = AllocateTaggedMemory(
                allocationSize, flags);
            if (g_pLastPacketAllocation_005a68f0 == 0)
                g_nPacketError_0049ca90 = 4;
        }
    } else {
        if (SeekPacketSection(handle, 4, 0) == -1) {
            g_nPacketError_0049ca90 = 5;
            return 0;
        }
        g_pLastPacketAllocation_005a68f0 = destination;
    }

    if (g_nPacketError_0049ca90 == 0) {
        packet = g_pLastPacketAllocation_005a68f0;
        if (ReadPacketSectionData(
                handle, g_pPacketDecompressInput_0059ab04,
                g_wPacketDecompressInputSize_0059ab38) == 0)
            packet = 0;
        g_nPacketDecompressResult_0059ab30 = GetVideoReleaseResult();
        if (packet != 0)
            VideoReleaseHook();
    }
    if (decompressionWorkspace == 0) {
        ReleasePacketHandle(smallScratch);
        if (fallbackAllocations != 0)
            ReleasePacketHandle(largeScratch);
        else
            ReleasePacketHandle(largeScratch);
    }
    return packet;
}

/* Function start: WC2_UNMAPPED */
short GetTargetColourIndex(void)
{
    short v = (short)g_nSpacePaletteFadeMode_004901e8;

    if ((short)g_nSpacePaletteFadeMode_004901e8 == -1)
        v = 0x13;
    return v;
}

/* Function start: 0x43FAC0 */
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
    target = g_acShipTarget_00495f20[0];
    if (target != -1 &&
        (g_aeObjectClass_00495328[target] < OBJECT_CLASS_SHIP ||
         g_aeSpecialManeuver_00495600[target] ==
             SPECIAL_MANEUVER_UNKNOWN_9)) {
        target = -1;
        g_acShipTarget_00495f20[0] = -1;
    }
    g_cTargetDisplayObject_0046c06c = (signed char)target;
    DrawFormattedText(g_szTargetLabel_0046a984);
    if (target == -1) {
        DrawFormattedText(g_szNoTarget_0046a990);
        return;
    }
    targetIndex = (int)target;
    objectType = g_acObjectType_00493980[targetIndex];
    typeData = &g_aObjectTypeData_00496d30[objectType];
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
    if (g_asObjectScreenX_00493598[targetIndex] == (short)0x8001) {
        g_cTargetDisplayObject_0046c06c = -1;
        return;
    }

    x = (short)(DAT_005a7530.left + 0x25);
    y = (short)(DAT_005a7530.top + 0x26);
    frame = (short)((3 - MinShort(
        (short)((g_aasShipShield_00495518[targetIndex][1] * 6) /
                typeData->shieldAft), 3)) * 2);
    if (frame < 6)
        DrawSpriteDefault(&DAT_005a7530, x, y,
                          g_pCockpitIndicatorShape_005a7658, frame);

    targetViewport = DAT_005a7530;
    maximumArmor = &typeData->armorFront;
    for (armor = 0; armor < 4; armor++) {
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
    }

    DrawSpriteDefault(&DAT_005a7530, x, y, typeData->shape, 2);
    frame = (short)((3 - MinShort(
        (short)((g_aasShipShield_00495518[targetIndex][0] * 6) /
                typeData->shieldFore), 3)) * 2);
    if (frame < 6)
        DrawSpriteDefault(&DAT_005a7530, x, y,
                          g_pCockpitIndicatorShape_005a7658,
                          (short)(frame + 1));
}

/* Function start: 0x43FF40 */
void DrawTargetRangeReadout(void)
{
    short target;
    const char *rangeText;

    target = g_acShipTarget_00495f20[0];
#ifdef WC1_SDL
    /* The original tests the table before the -1 sentinel.  At target -1 it
       reads the zero-filled gap at 0x0059c3bc, so the comparison is false. */
    if (target != -1 &&
#else
    if (
#endif
        g_aeSpecialManeuver_00495600[target] ==
        SPECIAL_MANEUVER_UNKNOWN_9) {
        g_acShipTarget_00495f20[0] = -1;
        InvalidateVduMode(1);
        return;
    }
    if (target != -1 &&
        g_aeObjectClass_00495328[target] < OBJECT_CLASS_SHIP) {
        g_acShipTarget_00495f20[0] = -1;
        target = -1;
    }
    if (g_cTargetDisplayObject_0046c06c != target ||
        (short)(g_nRenderedSpaceFrame_00493138 % 8) == 0) {
        set_new_vdu(1);
        show_target_disp();
    }
    if (target == -1)
        return;

    if (g_asObjectScreenX_00493598[target] == (short)0x8001) {
        rangeText = g_szTargetOffscreenRange_0046a9bc;
    } else if ((unsigned short)g_asObjectDistance_0059b4a0[target] <=
               30000) {
        strcat(_itoa((unsigned short)g_asObjectDistance_0059b4a0[target],
                     g_szTextScratchBuffer_005d1c40, 10), " m");
        goto draw_readout;
    } else {
        rangeText = g_szTargetTooFar_0046a9b0;
    }
    memcpy(g_szTextScratchBuffer_005d1c40, rangeText, 8);

draw_readout:
    DrawCockpitReadout(1, g_szTextScratchBuffer_005d1c40);
    if (g_nTargetLockCountdown_004934ec == 0) {
        if (g_bTargetLockAcquired_0046c074 == 1) {
            g_bTargetLockAcquired_0046c074 = 0;
            return;
        }
    } else if (g_bTargetLockReadoutDirty_004934e8 != 0) {
        EraseCockpitReadoutRegion(&DAT_005a7530,
                                  DAT_005a7530.left,
                                  (short)(DAT_005a7530.bottom - 6),
                                  DAT_005a7530.right,
                                  DAT_005a7530.bottom,
                                  (short)g_cSecondaryViewBufferColour_0049cb4c);
        g_bTargetLockReadoutDirty_004934e8 = 0;
    }
}

/* Function start: 0x45CA50 */
void LogDisplayMode(const char *mode)
{
    SystemDebugPrintf("display_mode == '%s'\n", mode);
    ClearDebugPauseFlags();
    PumpMessagesDuringWait();
    exit(1);
}

/* Function start: 0x45CA80 */
short CalcRectangleArea(const Viewport *viewport)
{
    short height;
    short width;

    if (g_nSpacePaletteFadeMode_004901e8 != 0x13)
        LogDisplayMode("not MCGA");
    height = (short)(viewport->bottom - viewport->top + 1);
    width = (short)(viewport->right - viewport->left + 1);
    return (short)(height * width);
}

/* Function start: 0x45CAD2 */
unsigned short AllocateViewport(Viewport *viewport,
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
    if (g_nSpacePaletteFadeMode_004901e8 != 0x13)
        LogDisplayMode("not MCGA");
    viewport->allocation = AllocateTaggedMemory(
        (unsigned int)width * height, (unsigned short)(flags + 2));
    g_apViewportAllocations_005a7f10[
        g_nViewportAllocationCount_005a7f0c++] = viewport->allocation;
    if (viewport->allocation == 0)
        return 0;
    viewport->pixels = viewport->allocation;

    rowOffsets = AllocateTaggedMemory(
        (top + (unsigned int)height) * sizeof(unsigned short) + 4,
        0);
    viewport->rowOffsets = rowOffsets;
    if (rowOffsets == 0) {
        if (g_nSpacePaletteFadeMode_004901e8 != 0x13)
            ReleasePacketHandle(viewport->allocation);
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

/* Function start: 0x46505E */
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

/* Function start: 0x452AE5 */
void FadeMusic(int duration)
{
#if 0
    SoundDebugPrintf("FadeMusic");
#endif
}

/* Function start: WC2_UNMAPPED */
void SetMusicOn(short enabled)
{
    SoundDebugPrintf("SetMusicOn %d", (int)enabled);
}

/* Function start: 0x452A00 */
void StopMusicStream(void)
{
    Streamer_stop();
}

/* Function start: 0x452A26 */
void StopMusic(int enabled)
{
#if 0
    (void)unused;
    SoundDebugPrintf("StopMusic");
    g_nCurrentMusicTrack_0049be98 = -1;
    Streamer_stop();
    SoundDebugPrintf("");
#endif
    g_nCurrentMusicTrack_0049be98 = -1;
    Streamer_stop();
}

/* Function start: 0x452A10 */
void SetMusBreakpt(int first, int second)
{
#if 0
    (void)first;
    (void)second;
    SoundDebugPrintf("SetMusBreakpt");
#endif
}

/* Function start: 0x452A99 */
unsigned int PaletteFadeHook(void)
{
    return 0;
}

/* Function start: 0x452AAB */
void FlushSoundEffect(void)
{
#if 0
    stop_all_sounds();
    SoundDebugPrintf("FlushSoundEffect");
#else
    SoundDebugPrintf("FLUSH_EFFECT");
    stop_all_sounds();
#endif
}

/* Function start: 0x452AC8 */
void FlushSoundEffects(void)
{
#if 0
    stop_all_sounds();
    SoundDebugPrintf("FlushSoundEffects");
#else
    SoundDebugPrintf("FLUSH_EFFECTS");
    stop_all_sounds();
#endif
}

/* Function start: 0x452AF0 */
void SceneLeaveHook(void)
{
}

/* Function start: WC2_UNMAPPED */
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
        SoundDebugPrintf("being tailed");
        streamSet = 2;
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

    switch (streamSet) {
    case 0:
        streamName = "preflite.str";
        break;
    case 1:
        streamName = "posflite.str";
        break;
    case 2:
        streamName = "mission.str";
        ClearStreamerTrigger();
        break;
    default:
        streamName = 0;
        break;
    }

    if (streamName != 0) {
        Streamer_open(streamName);
        g_nMusicStreamSet_0046aa18 = streamSet;
    } else {
        Streamer_close();
        g_nMusicStreamSet_0046aa18 = -1;
    }
}

/* Function start: WC2_UNMAPPED */
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

/* Function start: 0x402258 */
void RecordMusicCommandHook(int track, int command, int enabled)
{
}

/* Function start: 0x452B03 */
unsigned short ProcessMusicScriptCommand(int track, int command, int enabled)
{
#if 0
    int streamerCommand;

    if (track == -1 || g_bMusicCommandSuppressed_0046a9fc != 0)
        return;
    if (command == 4) {
        SoundDebugPrintf("queue_stop\n");
        StopMusic();
        g_nCurrentMusicTrack_0049be98 = -1;
        return;
    }

    SoundDebugPrintf("track_%02d ", track);
    if ((g_nCurrentMusicTrack_0049be98 == 25 && track == 25) ||
        (g_nCurrentMusicTrack_0049be98 == 38 && track == 38) ||
        (g_nCurrentMusicTrack_0049be98 == 39 && track == 39) ||
        (g_nCurrentMusicTrack_0049be98 == 40 && track == 40)) {
        SoundDebugPrintf("skipping for QA\n");
        return;
    }

    g_nCurrentMusicTrack_0049be98 = track;
    SelectFlightMusicTrack(track);
    if (g_nMusicStreamSet_0046aa18 == 2) {
        if ((track >= 0 && track <= 5) ||
            (track >= 12 && track <= 18)) {
            SoundDebugPrintf("flight_intensity %d ", track);
            SetStreamerIntensity((unsigned char)track);
        } else {
            SoundDebugPrintf("flight_trigger %d ", track);
            SetStreamerTrigger(track);
        }
    } else {
        switch (command) {
        case 0:
            SoundDebugPrintf(" queue_start ");
            streamerCommand = MapMusicTrackToStreamerCommand(track);
            SetStreamerTrigger(streamerCommand);
            break;
        case 1:
            SoundDebugPrintf(" queue_break ");
            streamerCommand = MapMusicTrackToStreamerCommand(track);
            ForceStreamerTrigger(streamerCommand);
            break;
        case 2:
            SoundDebugPrintf(" queue_switch ");
            streamerCommand = MapMusicTrackToStreamerCommand(track);
            SetStreamerTrigger(streamerCommand);
            break;
        case 3:
            SoundDebugPrintf(" queue_interrupt ");
            streamerCommand = MapMusicTrackToStreamerCommand(track);
            ForceStreamerTrigger(streamerCommand);
            break;
        }
    }
    SoundDebugPrintf("\n");
#else
    int streamFlags;
    int streamerCommand;

    if (track == -1)
        return 0;

    streamFlags = g_acMusicTrackStreamFlags_0049beb8[track];
    SoundDebugPrintf("queue_next_music %d %x\n", track, streamFlags);
    if (track >= 0x40)
        streamerCommand = track - 0x2c;
    else
        streamerCommand = track;

    if (command == 4) {
        SoundDebugPrintf("queue_stop\n");
        StopMusic(enabled);
        g_nCurrentMusicTrack_0049be98 = -1;
        return;
    }

    if ((streamFlags & g_nActiveMusicStreamMask_0049be9c) == 0) {
        if ((streamFlags & 1) != 0) {
            Streamer_open("gameflow.str");
            g_nActiveMusicStreamMask_0049be9c = 1;
        } else if ((streamFlags & 2) != 0) {
            Streamer_open("spaceflt.str");
            g_nActiveMusicStreamMask_0049be9c = 2;
        } else if ((streamFlags & 0x10) != 0) {
            Streamer_open("gametwo.str");
            g_nActiveMusicStreamMask_0049be9c = 0x10;
        } else {
            SoundDebugPrintf("**** unknown stream for track %d\n", track);
            StopMusic(0);
            return;
        }
        SetStreamerIntensity(0x32);
    } else {
        g_nCurrentMusicTrack_0049be98 = track;
    }

    if (g_nActiveMusicStreamMask_0049be9c == 2) {
        if ((streamFlags & 4) != 0) {
            SetStreamerIntensity(streamerCommand);
        } else if ((streamFlags & 8) != 0) {
            SetStreamerTrigger(streamerCommand);
        } else {
            SoundDebugPrintf(
                "unknown int/trig stream for track %d\n", track);
            exit_squadron("unknown track %d", track);
        }
    } else {
        Streamer_stop();
        ForceStreamerTrigger(streamerCommand);
    }

    if (g_nCannedSceneMode_0049021c == 0)
        RecordMusicCommandHook(track, command, enabled);
    return 1;
#endif
}

/* Function start: 0x452CF4 */
void spacetrack(int track, int mode, int enabled)
{
    if (g_nMusicDriverMode_0049be8c != 0 &&
        g_nMusicDriverMode_0049be8c != 3)
        ProcessMusicScriptCommand(track, mode, enabled);
#if 0
    return 1;
#endif
}

/* Function start: 0x452D32 */
void StopMusicUnlessSuppressed(void)
{
    if (g_nMusicDriverMode_0049be8c != 0 &&
        g_nMusicDriverMode_0049be8c != 3)
        StopMusic(0);
}

/* Function start: 0x452D66 */
unsigned short IsMusicTrackComplete(void)
{
    if (g_nMusicDriverMode_0049be8c != 0 &&
        g_nMusicDriverMode_0049be8c != 3 &&
        g_nMusicTrackComplete_0049be88 != 0)
        return 1;
    return 0;
}

/* Function start: 0x452DB0 */
void wait_for_end_of_music(void)
{
    if (g_nMusicDriverMode_0049be8c != 0 &&
        g_nMusicDriverMode_0049be8c != 3) {
        if (g_nWaitForMusicEnabled_0049beac == 0) {
            StopMusic(0);
            return;
        }
        SetMusBreakpt(0, 0);
#if 0
        do {
            if (g_nMusicTrackComplete_0049be88 != 0)
                return;
        } while (g_bSceneEscapeRequested_0049d4b0 == 0 &&
                 CheckEscaped() == 0);
        StopMusic(0);
#else
        while (g_nMusicTrackComplete_0049be88 == 0) {
            if (g_bSceneEscapeRequested_0049d4b0 != 0 ||
                CheckEscaped() != 0) {
                StopMusic(0);
                return;
            }
        }
#endif
    }
}

/* Function start: 0x452E46 */
void new_space_music_changes(short attacker, short victim)
{
    enum Side side;

    if (g_nInFlightMusicActive_0049bf08 != 0 &&
        g_nTrainSimActive_00469e2c == 0) {
        side = g_asShipSide_004955d0[victim];
        if (side == SIDE_KILRATHI) {
            if (report_kilrathi_rout(1) == 0) {
                spacetrack(10, 1, 0);
                return;
            }
            if (attacker == 0) {
                if (g_acShipRating_0059cd80[victim] == -1 &&
                    RandomInRange(0, 3) != 0) {
                    spacetrack(6, 3, 0);
                    return;
                }
                spacetrack(9, 3, 0);
                return;
            }
        } else {
            if (g_nYourWingman_0049346c == victim) {
                spacetrack(8, 3, 0);
                return;
            }
            if (side == SIDE_IMPERIAL) {
                if ((g_asShipMissionType_00495de8[0] ==
                         MISSION_TYPE_DEFEND ||
                     g_asShipMissionType_00495de8[0] ==
                         MISSION_TYPE_ESCORT) &&
                    g_nShipMissionIndices_0059c830[victim] ==
                        g_anShipMissionShip_00495e00[0]) {
                    spacetrack(11, 3, 0);
                    return;
                }
                spacetrack(7, 3, 0);
            }
        }
    }
}

/* Function start: 0x452F90 */
int changetrack(void)
{
    int track;

    switch (g_asShipMissionType_00495de8[0]) {
    case MISSION_TYPE_ESCORT:
        track = 18;
        break;
    case MISSION_TYPE_STRIKE:
        track = 17;
        break;
    case MISSION_TYPE_DEFEND:
    case MISSION_TYPE_RENDEZVOUS:
        track = 16;
        break;
    default:
        track = 15;
        break;
    }
    if (g_aMissionObjectives_004932a8[
            g_cCurrentObjective_004931cc].type == OBJECTIVE_HOME_BASE) {
        if (triumph(0) != 0) {
            if (g_asShipMissionType_00495de8[0] == MISSION_TYPE_PATROL)
                return 13;
            return 14;
        }
        track = 12;
    }
    return track;
}

/* Function start: 0x453085 */
void gametrack(void)
{
    int track;
    short damage;

    track = -1;
    if (g_nInFlightMusicActive_0049bf08 != 0) {
        if (g_nTrainSimActive_00469e2c != 0) {
            if (g_nMusicStreamSet_0046aa18 != 0 ||
                g_nCurrentMusicTrack_0049be98 != 20)
                spacetrack(20, 1, 0);
            SoundDebugPrintf("%d %d\n", g_nMusicStreamSet_0046aa18,
                             g_nCurrentMusicTrack_0049be98);
            return;
        }
        if (g_nCombatMusicActive_0049bf04 != 0) {
            if ((g_nSpaceFrame_00493134 & 0xf) == 0 ||
                g_nMusicTrackComplete_0049be88 != 0) {
                if (g_nInitialFlightMusicPending_0049bf00 != 0)
                    g_nInitialFlightMusicPending_0049bf00 = 0;
                if (FindMissileTargetingObject(0) != 0) {
                    track = 3;
                } else if (any_enemy_tail(0) != 0) {
                    track = 1;
                } else if (is_ship_tailing_player_target(0) != 0) {
                    track = 2;
                } else {
                    damage = (short)calculate_damage_level();
                    if (damage < 2)
                        track = damage == 1 ? 5 : 0;
                    else
                        track = 4;
                }
                if (report_kilrathi_rout(1) == 0)
                    g_nCombatMusicActive_0049bf04 = 0;
            }
        } else if ((g_nSpaceFrame_00493134 & 0xf) == 0 ||
                   g_nMusicTrackComplete_0049be88 != 0) {
            track = changetrack();
            if (report_kilrathi_rout(2) != 0)
                g_nCombatMusicActive_0049bf04 = 1;
        }
        spacetrack(track, 1, 0);
    }
}

/* Function start: 0x453240 */
void servicetrack(void)
{
    short object;
    FixedVector futurePosition;
    FixedVector travel;

    gametrack();
    if (g_nFlightSoundEffectsEnabled_0046aa34 != 0) {
        for (object = 0; object < WC2_SPACE_OBJECT_COUNT; object++) {
            if (object == g_nPassingShipSoundObject_0046aa48) {
                if (g_aeObjectClass_00495328[object] !=
                        OBJECT_CLASS_SHIP ||
                    g_aeObjectClass_00495328[object] !=
                        OBJECT_CLASS_CAPITAL_SHIP)
                    g_nPassingShipSoundObject_0046aa48 = -1;
            }
            if (g_aeObjectClass_00495328[object] ==
                    OBJECT_CLASS_ASTEROID) {
                if (g_asObjectDistance_0059b4a0[object] == 0 &&
                    (unsigned short)
                        g_asPreviousObjectDistance_0059d080[object] < 50 &&
                    g_aiSoundEffectSourceActive_005a66ec[object + 1] == 0)
                    PlaySfxWaveFileByNumber(6, object, 0);
            } else if (g_aeObjectClass_00495328[object] >=
                           OBJECT_CLASS_SHIP &&
                       g_aeObjectClass_00495328[object] <=
                           OBJECT_CLASS_CAPITAL_SHIP &&
                       g_asObjectScreenX_00493598[object] !=
                           (short)0x8001 &&
                       (unsigned short)
                           g_asObjectDistance_0059b4a0[object] < 0x55a) {
                if (g_nPassingShipSoundObject_0046aa48 == -1) {
                    ScaleFixedVector(&g_aShipVelocity_0059c010[object],
                                     0x1400, &travel);
                    AddFixedVectors(&g_aShipPosition_00494550[object],
                                    &travel, &futurePosition);
                    ComputeVectorDelta(
                        &g_aShipPosition_00494550[WC1_EYE_OBJECT],
                        &futurePosition, &travel);
                    ComputeVectorDelta(
                        &g_aShipPosition_00494550[WC1_EYE_OBJECT],
                        &g_aShipPosition_00494550[object],
                        &futurePosition);
                    if (dot_product(&travel, &futurePosition) < 0xdd) {
                        g_nPassingShipSoundCountdown_0046aa4c = 10;
                        g_nPassingShipSoundObject_0046aa48 = object;
                        if (g_nPassingShipSoundCooldown_005a68e8 <
                            g_nSpaceFrame_00493134) {
                            g_nPassingShipSoundCooldown_005a68e8 =
                                g_nSpaceFrame_00493134 + 6;
                            PlaySfxWaveFileByNumber(2, object, 0);
                        }
                    }
                } else if (object ==
                               g_nPassingShipSoundObject_0046aa48) {
                    g_nPassingShipSoundCountdown_0046aa4c--;
                    if (g_nPassingShipSoundCountdown_0046aa4c == 0)
                        g_nPassingShipSoundObject_0046aa48 = -1;
                }
            }
        }
    }
}

/* Function start: 0x453496 */
void ResetSoundState(void)
{
    FlushSoundEffects();
    DAT_005a7cec = 0;
    DAT_005a7ec0 = 0;
}

/* Function start: 0x4534BA */
void ResetSoundStateForScene(void)
{
    ResetSoundState();
    g_nFlightSoundEffectsEnabled_0046aa34 = 0;
}

/* Function start: 0x4534D3 */
void ResetSoundStateForFlight(void)
{
    ResetSoundState();
    g_nFlightSoundEffectsEnabled_0046aa34 = 1;
}

/* Function start: 0x4534FC */
void EnableMusicForScene(void)
{
    g_nWaitForMusicEnabled_0049beac = 1;
    SetMusicOn(1);
}

/* Function start: WC2_UNMAPPED */
unsigned int SoundFxTick(void)
{
    WriteDebugString("soundFX");
    return 0;
}

/* Function start: 0x45351A */
int LogUnknownSoundEffect(const unsigned char *definition,
                          int sourceObject, int pan, int volume,
                          int looping, int priority)
{
    SoundDebugPrintf("Playing SFX <UNKNOWN>\n");
    return 0;
}

/* Function start: 0x45357E */
void FlushSoundEffectsAndLog(void)
{
#if 0
    FlushSoundEffects();
#else
    FlushSoundEffect();
    SoundDebugPrintf("flushFX\n");
    return;
#endif
}

/* Function start: 0x4535BB */
void sound_effect(signed char soundNumber, short sourceObject, short looping)
{
#if 0
    WriteDebugString("sound_effect");
#else
    short volume;
    short effectHandle = 0;
    int distance;
    FixedVector delta;
    int stereoOffset;
    short sampleNumber;
    short pan;

    SoundDebugPrintf("Playing SFX #%d\n", soundNumber);
    return;
#endif
}

/* Function start: 0x45373B */
void PlaySfxWaveFileByNumber(int soundNumber, int sourceObject, int looping)
{
#if 0
    FixedVector delta;
    int distance;

#ifdef WC1_SDL
    if (Wc1SdlUsingDosData()) {
        int volume;

        volume = 127;
        if (sourceObject != -1) {
            if (sourceObject < 0 ||
                sourceObject >= WC2_SPACE_OBJECT_COUNT)
                return;
            ComputeVectorDelta(
                &g_aShipPosition_00494550[WC1_EYE_OBJECT],
                &g_aShipPosition_00494550[sourceObject], &delta);
            distance = (int)((Vector_magnitude(&delta) / 500L) >> 8);
            volume -= distance;
            if (volume < 0)
                volume = 0;
        }
        if (volume >= 10 && Wc1SdlPlayDosSoundEffect(
                soundNumber, volume, 64, sourceObject, looping)) {
            g_aiSoundEffectSourceActive_005a66ec[sourceObject + 1] = 1;
            if (sourceObject == -1)
                DAT_005a7cec = soundNumber == 12;
        }
        return;
    }
#endif

    if (sourceObject != -1) {
        ComputeVectorDelta(&g_aShipPosition_00494550[WC1_EYE_OBJECT],
                           &g_aShipPosition_00494550[sourceObject],
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
        playWAVE(g_szSfxWavePath_00476558, looping, distance);
    }
#else
    FixedVector delta;
    int magnitude;
    int distance;

    if (sourceObject != -1) {
        ComputeVectorDelta(&g_aShipPosition_00494550[WC2_EYE_OBJECT],
                           &g_aShipPosition_00494550[sourceObject],
                           &delta);
        magnitude = Vector_magnitude(&delta);
        distance = magnitude;
        if (distance > 32000)
            distance = 32000;
        g_aiSoundEffectSourceActive_005d12c0[sourceObject] = 1;
    } else {
        distance = 72000;
    }
    if (distance < 10)
        return;
    soundNumber--;
    SoundDebugPrintf(
        "Playing SFX #%d on obj: %d with volume of %d\n",
        soundNumber, sourceObject, distance);
    sprintf(g_szSfxWavePath_005b3650, "sfx%02i.wav", soundNumber);
    playWAVE(g_szSfxWavePath_005b3650, looping, distance);
#endif
}
