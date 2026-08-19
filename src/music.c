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
    FixedVector vector;
    short obj;
    short command;

    if (*pViewScript == 13)
        return 0;
    while (*pViewScript != 14) {
        command = *pViewScript++;
        if (command == -1) {
            bScriptedView = 0;
            nScriptedViewObject = -1;
            return 0;
        }
        switch (command) {
        case 0:
#ifdef WC1_SDL
            aShipPosition[61].x =
                (int)*pViewScript++ * 0x100;
            aShipPosition[61].y =
                (int)*pViewScript++ * 0x100;
            aShipPosition[61].z =
                (int)*pViewScript++ * 0x100;
#else
            aShipPosition[61].x =
                (int)*pViewScript++ << 8;
            aShipPosition[61].y =
                (int)*pViewScript++ << 8;
            aShipPosition[61].z =
                (int)*pViewScript++ << 8;
#endif
            break;
        case 1:
            alter_yaw(*pViewScript++, 61);
            alter_pitch(*pViewScript++, 61);
            alter_roll(*pViewScript++, 61);
            break;
        case 2:
#ifdef WC1_SDL
            ScaleFixedVector(&aShipForwardVector[61],
                             (int)*pViewScript++ * 0x100,
                             &aShipVelocity[61]);
#else
            ScaleFixedVector(&aShipForwardVector[61],
                             (int)*pViewScript++ << 8,
                             &aShipVelocity[61]);
#endif
            break;
        case 3:
            force_view(*pViewScript++,
                       nScriptedViewObject);
            break;
        case 4:
            nEyePitchGoal =
                (short)-*pViewScript++;
            nEyePitchRate = *pViewScript++;
            break;
        case 5:
            nEyePitchGoal = *pViewScript++;
            nEyePitchRate = *pViewScript++;
            break;
        case 6:
            nEyeYawGoal = *pViewScript++;
            nEyeYawRate = *pViewScript++;
            break;
        case 7:
            nEyeYawGoal =
                (short)-*pViewScript++;
            nEyeYawRate = *pViewScript++;
            break;
        case 8:
            nEyeRollGoal = *pViewScript++;
            nEyeRollRate = *pViewScript++;
            break;
        case 9:
            copy_frame(61, 63);
            alter_yaw(*pViewScript++, 63);
            alter_pitch(*pViewScript++, 63);
            alter_roll(*pViewScript++, 63);
#ifdef WC1_SDL
            ScaleFixedVector(&aShipForwardVector[63],
                             (int)*pViewScript++ * 0x100,
                             &vector);
#else
            ScaleFixedVector(&aShipForwardVector[63],
                             (int)*pViewScript++ << 8,
                             &vector);
#endif
            AddFixedVectors(&aShipVelocity[61], &vector,
                            &aShipVelocity[61]);
            break;
        case 10:
            aShipVelocity[61] =
                aShipVelocity[nScriptedViewObject];
            break;
        case 11:
            copy_frame(nScriptedViewObject, 61);
            break;
        case 12:
            aShipPosition[61] =
                aShipPosition[nScriptedViewObject];
            break;
        case 15:
            ComputeVectorDelta(&aShipPosition[61],
                &aShipPosition[nScriptedViewObject],
                &vector);
            aShipForwardVector[61] = vector;
            fix_objects_ijk(61);
            break;
        case 16:
            obj = 0;
            while (obj < 10) {
                if (nShipMissionIndices[obj] ==
                    *pViewScript)
                    break;
                obj++;
            }
            if (obj < 10)
                nScriptedViewObject = obj;
            pViewScript++;
            break;
        }
        if (*pViewScript == 13)
            return 0;
    }
    asObjectCounter[61] = pViewScript[1];
    return 0;
}

/* Function start: 0x42D1C0 */
unsigned int update_scripted_view(void)
{
    short counter;
    int command;

    command = *pViewScript;
    switch (command) {
    case 13:
        if ((nEyeYawGoal == nEyePitchGoal) !=
            nEyeRollGoal) {
            pViewScript++;
            parse_view_script();
        }
        break;
    case 14:
        counter = asObjectCounter[61];
        asObjectCounter[61]--;
        if (counter < 1) {
            pViewScript += 2;
            parse_view_script();
        }
        break;
    }
    return 0;
}

/* Function start: 0x42D230 */
void initialize_scripted_view(const short *script)
{
    bScriptedView = 1;
    zero_vector(&aShipVelocity[61]);
    init_ijk(61);
    pViewScript = script;
    parse_view_script();
    asObjectCollisionRadius[61] = 100;
}

/* Function start: 0x42D270 */
unsigned int InitializeFireworks(void)
{
    int empty = -1;
    short i = 0;

    do {
        aFireworks[i].frame = (short)empty;
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
        if (aFireworks[index].frame == -1) {
            emptyCount++;
        } else {
            DrawSpriteDefault(
                viewport, aFireworks[index].x,
                aFireworks[index].y,
                pFireworkShape,
                (short)(aFireworks[index].frame +
                        aFireworks[index].variant * 8));
            if (aFireworks[index].frame++ == 7) {
                aFireworks[index].frame = -1;
                ((void (__cdecl *)(int, short))FlushSoundEffectsAndLog)(
                    aFireworks[index].soundHandle, index);
            } else if (aFireworks[index].frame == 1) {
                aFireworks[index].soundHandle =
                    ((unsigned int (__cdecl *)(
                        const unsigned char *, int, int, short, short,
                        int))SoundFxTick)(
                            abFireworkSoundDescriptor,
                            0, 127,
                            (signed char)(
                                127 -
                                (int)aFireworks[index].x *
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

    pConstellationViewport = viewport;
    nConstellationDirection = direction;
    width = (short)(viewport->right - viewport->left);
    height = (short)(viewport->bottom - viewport->top);
    nConstellationStarCount = (short)(density * 10 / 16);
    nConstellationParticleCount =
        (short)(density * 16 / 16);
    index = 0;
    while (index < nConstellationStarCount) {
        aConstellationStars[index].x =
            RandomInRange(0, width);
        aConstellationStars[index].y =
            RandomInRange(0, height);
        aConstellationStars[index].frame =
            (short)(RandomInRange(0, 5) + 32);
        index++;
    }
    index = 0;
    while (index < nConstellationParticleCount) {
        randomIndex = RandomInRange(0, 15);
        particleIndex = index;
        index++;
        aConstellationParticles[particleIndex].x =
            (short)(pConstellationViewport->left +
                    RandomInRange(0, width));
        aConstellationParticles[particleIndex].y =
            (short)(pConstellationViewport->top +
                    RandomInRange(0, height));
        aConstellationParticles[particleIndex].velocity =
            (short)(asConstellationVelocity[randomIndex] *
                    nConstellationDirection);
        aConstellationParticles[particleIndex].frame =
            (short)(asConstellationFrame[randomIndex] +
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

    height = (short)(pConstellationViewport->bottom -
                     pConstellationViewport->top);
    ClearViewport(pConstellationViewport, cPrimaryViewBufferColour);
    for (index = 0;
         index < nConstellationStarCount;
         index++) {
        DrawSpriteDefault(pConstellationViewport,
                          aConstellationStars[index].x,
                          aConstellationStars[index].y,
                          pConstellationShape,
                          aConstellationStars[index].frame);
    }
    for (index = 0;
         index < nConstellationParticleCount;
         index++) {
        particle = &aConstellationParticles[index];
        DrawSpriteDefault(pConstellationViewport,
                          particle->x, particle->y,
                          pConstellationShape,
                          particle->frame);
        particle->x = (short)(particle->x + particle->velocity);
        particle->frame = (short)(
            (particle->frame & 0xfc) + (particle->frame + 1) % 4);
        if (nConstellationDirection < 0) {
            if (particle->x < pConstellationViewport->left) {
                randomIndex = RandomInRange(0, 15);
                speed = asConstellationVelocity[randomIndex];
                particle->x = (short)(
                    pConstellationViewport->right -
                    RandomInRange(0, speed));
                particle->y = (short)(
                    pConstellationViewport->top +
                    RandomInRange(0, height));
                particle->velocity = (short)-speed;
            }
        } else if (particle->x >
                   pConstellationViewport->right) {
            randomIndex = RandomInRange(0, 15);
            speed = asConstellationVelocity[randomIndex];
            particle->velocity = speed;
            particle->x = (short)(
                pConstellationViewport->left +
                RandomInRange(0, speed));
            particle->y = (short)(
                pConstellationViewport->top +
                RandomInRange(0, height));
            particle->frame = (short)(
                asConstellationFrame[randomIndex] +
                RandomInRange(0, 3));
        }
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
        nPacketError = 3;
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
void __stdcall CloseDataFileByHandle(unsigned short *handle)
{
    CloseDataFile(*handle);
}

/* Function start: 0x42D880 */
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
    pLastPacketAllocation = 0;
    if (wPacketCompressionFormatFlags == 0) {
        if (handle->compression != 1) {
            nPacketError = 6;
            return 0;
        }
    } else if ((handle->compression & 0xc0) != 0) {
        nPacketError = 6;
        return 0;
    }

    if (decompressionWorkspace == 0) {
        if (pPacketDecompressionWorkspace != 0) {
            wPacketDecompressionInputSizeOverride = 0;
            decompressionWorkspace =
                pPacketDecompressionWorkspace;
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
                    nPacketError = 1;
                    return 0;
                }
                smallScratch = AllocateTaggedMemory(0x400, 0x22);
                if (smallScratch == 0) {
                    ReleasePacketHandle(largeScratch);
                    nPacketError = 2;
                    return 0;
                }
            }
            alignedWorkspace = IdentityHandle(largeScratch);
            pPacketDecompressInput = IdentityHandle(smallScratch);
            wPacketDecompressInputSize = 0x400;
            goto initializeDecompressor;
        }
    }

    alignedWorkspace = IdentityHandle(decompressionWorkspace);
    wPacketDecompressInputSize = 0x400;
    allocationSize = 0x3020;
    pPacketDecompressInput =
        ((PacketDecompressionWorkspace *)decompressionWorkspace)->input;
    pPacketDecompressInput =
        IdentityHandle(pPacketDecompressInput);
    largeScratch = (void *)allocationSize;
    smallScratch = (void *)allocationSize;
    if (wPacketDecompressionInputSizeOverride != 0)
        wPacketDecompressInputSize =
            wPacketDecompressionInputSizeOverride;

initializeDecompressor:
    nPacketDecompressSourceFile = handle->file;
    nPacketDecompressInputPosition = 0;
    nPacketDecompressPending = 0;
    nPacketDecompressWorkspaceSegment =
        (short)(unsigned int)alignedWorkspace;
    if (alignedWorkspace != 0)
        nPacketDecompressWorkspaceSegment++;

    if (SeekPacketSection(handle, 0, 0) == -1) {
        nPacketError = 5;
        return 0;
    }
    if (destination == 0) {
        if (ReadPacketSectionData(handle, &allocationSize, 4) != 0) {
            pLastPacketAllocation = AllocateTaggedMemory(
                allocationSize, flags);
            if (pLastPacketAllocation == 0)
                nPacketError = 4;
        }
    } else {
        if (SeekPacketSection(handle, 4, 0) == -1) {
            nPacketError = 5;
            return 0;
        }
        pLastPacketAllocation = destination;
    }

    if (nPacketError == 0) {
        packet = pLastPacketAllocation;
        if (ReadPacketSectionData(
                handle, pPacketDecompressInput,
                wPacketDecompressInputSize) == 0)
            packet = 0;
        nPacketDecompressResult = GetVideoReleaseResult();
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

/* Function start: 0x42DB70 */
short GetTargetColourIndex(void)
{
    short v = (short)nVideoMode;

    if ((short)nVideoMode == -1)
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

    DrawTextAt(&stRightVduTextContext,
               stRightVdu.left, stRightVdu.top,
               szEmptyTargetDisplayText, 2);
    if (nTargetLockMode != 0) {
        DrawFormattedText(szTextColourStringColourFormat,
                          (unsigned int)cRedColour,
                          szLockedTarget,
                          (unsigned int)cDefaultTextColour);
    } else {
        DrawFormattedText(szTextColourStringFormat,
                          (unsigned int)cDefaultTextColour,
                          szAutoTargetting);
    }
    target = acShipTarget[0];
    if (target != -1 &&
        (aeObjectClass[target] < OBJECT_CLASS_SHIP ||
         aeSpecialManeuver[target] ==
             SPECIAL_MANEUVER_UNKNOWN_9)) {
        target = -1;
        acShipTarget[0] = -1;
    }
    cTargetDisplayObject = (signed char)target;
    DrawFormattedText(szTargetLabel);
    if (target == -1) {
        DrawFormattedText(szNoTarget);
        return;
    }
    targetIndex = (int)target;
    objectType = aeObjectType[targetIndex];
    typeData = &aObjectTypeData[objectType];
    rating = acShipRating[targetIndex];
    if (rating >= 0 && rating <= 7) {
        DrawFormattedText(
            szWingmanTargetNameFormat,
            apWingmanPilots[(int)rating]->callsign);
    } else if (rating >= 9 && rating <= 12) {
        DrawFormattedText(
            szAceTargetNameFormat,
            apszKilrathiAceNames[(int)rating - 9]);
    } else {
        DrawFormattedText(szShipTargetNameFormat,
                          typeData->displayName);
    }
    DrawFormattedText(szRangeLabel);
    InitializeCockpitReadout(1, &stRightVduTextContext);
    if (asObjectScreenX[targetIndex] == (short)0x8001) {
        cTargetDisplayObject = -1;
        return;
    }

    x = (short)(stRightVdu.left + 0x25);
    y = (short)(stRightVdu.top + 0x26);
    frame = (short)((3 - MinShort(
        (short)((aasShipShield[targetIndex][1] * 6) /
                typeData->shieldAft), 3)) * 2);
    if (frame < 6)
        DrawSpriteDefault(&stRightVdu, x, y,
                          pCockpitIndicatorShape, frame);

    targetViewport = stRightVdu;
    maximumArmor = &typeData->armorFront;
    for (armor = 0; armor < 4; armor++) {
        targetViewport.left =
            (short)(aTargetArmorClipRects[armor].left + x);
        targetViewport.top =
            (short)(aTargetArmorClipRects[armor].top + y);
        targetViewport.right =
            (short)(aTargetArmorClipRects[armor].right + x);
        targetViewport.bottom =
            (short)(aTargetArmorClipRects[armor].bottom + y);
        if (aasShipArmor[targetIndex][armor] >
            (short)(maximumArmor[armor] >> 1)) {
            DrawSpriteDefault(&targetViewport, x, y,
                              typeData->shape, 0);
        } else {
            DrawSpriteDefault(&targetViewport, x, y,
                              typeData->shape, 1);
        }
    }

    DrawSpriteDefault(&stRightVdu, x, y, typeData->shape, 2);
    frame = (short)((3 - MinShort(
        (short)((aasShipShield[targetIndex][0] * 6) /
                typeData->shieldFore), 3)) * 2);
    if (frame < 6)
        DrawSpriteDefault(&stRightVdu, x, y,
                          pCockpitIndicatorShape,
                          (short)(frame + 1));
}

/* Function start: 0x42DEA0 */
void DrawTargetRangeReadout(void)
{
    short target;
    const char *rangeText;

    target = acShipTarget[0];
#ifdef WC1_SDL
    /* The original tests the table before the -1 sentinel.  At target -1 it
       reads the zero-filled gap at 0x0059c3bc, so the comparison is false. */
    if (target != -1 &&
#else
    if (
#endif
        aeSpecialManeuver[target] ==
        SPECIAL_MANEUVER_UNKNOWN_9) {
        acShipTarget[0] = -1;
        InvalidateVduMode(1);
        return;
    }
    if (target != -1 &&
        aeObjectClass[target] < OBJECT_CLASS_SHIP) {
        acShipTarget[0] = -1;
        target = -1;
    }
    if (cTargetDisplayObject != target ||
        (short)(nRenderedSpaceFrame % 8) == 0) {
        set_new_vdu(1);
        show_target_disp();
    }
    if (target == -1)
        return;

    if (asObjectScreenX[target] == (short)0x8001) {
        rangeText = szTargetOffscreenRange;
    } else if ((unsigned short)asObjectDistance[target] <=
               30000) {
        strcat(_itoa((unsigned short)asObjectDistance[target],
                     szTextScratchBuffer, 10), " m");
        goto draw_readout;
    } else {
        rangeText = szTargetTooFar;
    }
    memcpy(szTextScratchBuffer, rangeText, 8);

draw_readout:
    DrawCockpitReadout(1, szTextScratchBuffer);
    if (nTargetLockCountdown == 0) {
        if (bTargetLockAcquired == 1) {
            bTargetLockAcquired = 0;
            return;
        }
    } else if (bTargetLockReadoutDirty != 0) {
        EraseCockpitReadoutRegion(&stRightVdu,
                                  stRightVdu.left,
                                  (short)(stRightVdu.bottom - 6),
                                  stRightVdu.right,
                                  stRightVdu.bottom,
                                  (short)cBlackColour);
        bTargetLockReadoutDirty = 0;
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

/* Function start: 0x42E050 */
short __stdcall CalcRectangleArea(const Viewport *viewport)
{
    short height;
    short width;

    if (nVideoMode != 0x13)
        LogDisplayMode("not MCGA");
    height = (short)(viewport->bottom - viewport->top + 1);
    width = (short)(viewport->right - viewport->left + 1);
    return (short)(height * width);
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
    nAllocateViewportCalls++;
    if (nVideoMode != 0x13)
        LogDisplayMode("not MCGA");
    viewport->allocation = AllocateTaggedMemory(
        (unsigned int)width * height, (unsigned short)(flags + 2));
    apViewportAllocations[
        nViewportAllocationCount++] = viewport->allocation;
    if (viewport->allocation == 0)
        return 0;
    viewport->pixels = viewport->allocation;

    rowOffsets = AllocateTaggedMemory(
        (top + (unsigned int)height) * sizeof(unsigned short) + 4,
        0);
    viewport->rowOffsets = rowOffsets;
    if (rowOffsets == 0) {
        if (nVideoMode != 0x13)
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
void StopMusic(short unused)
{
    (void)unused;
    SoundDebugPrintf("StopMusic");
    nCurrentMusicTrack = -1;
    Streamer_stop();
    SoundDebugPrintf("");
}

/* Function start: 0x42E380 */
void SetMusBreakpt(int first, int second)
{
    (void)first;
    (void)second;
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

    if (streamSet == nMusicStreamSet)
        return;
    if (streamSet == -1) {
        if (nMusicStreamSet >= 0)
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
        nMusicStreamSet = streamSet;
    } else {
        Streamer_close();
        nMusicStreamSet = -1;
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
void ProcessMusicScriptCommand(int track, int command, short enabled)
{
    int streamerCommand;

    if (track == -1 || bMusicCommandSuppressed != 0)
        return;
    if (command == 4) {
        SoundDebugPrintf("queue_stop\n");
        StopMusic(enabled);
        nCurrentMusicTrack = -1;
        return;
    }

    SoundDebugPrintf("track_%02d ", track);
    if ((nCurrentMusicTrack == 25 && track == 25) ||
        (nCurrentMusicTrack == 38 && track == 38) ||
        (nCurrentMusicTrack == 39 && track == 39) ||
        (nCurrentMusicTrack == 40 && track == 40)) {
        SoundDebugPrintf("skipping for QA\n");
        return;
    }

    nCurrentMusicTrack = track;
    SelectFlightMusicTrack(track);
    if (nMusicStreamSet == 2) {
        if ((track >= 0 && track <= 5) ||
            (track >= 12 && track <= 18)) {
            SoundDebugPrintf("flight_intensity %d ", track);
            SetStreamerIntensity((unsigned char)track);
        } else {
            SoundDebugPrintf("flight_trigger %d ", track);
            Streamer_trigger(track);
        }
    } else {
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
    }
    SoundDebugPrintf("\n");
}

/* Function start: 0x42E880 */
unsigned int spacetrack(int track, int mode, short enabled)
{
    if (nMusicPlaybackMode != 0 && nMusicPlaybackMode != 3)
        ProcessMusicScriptCommand(track, mode, enabled);
    return 1;
}

/* Function start: 0x42E8B0 */
void StopMusicUnlessSuppressed(void)
{
    if (nMusicPlaybackMode != 0 && nMusicPlaybackMode != 3)
        StopMusic(0);
}

/* Function start: 0x42E8D0 */
unsigned short GetMusicMode(void)
{
    if (nMusicPlaybackMode != 0 && nMusicPlaybackMode != 3 &&
        nMusicTrackComplete != 0)
        return 1;
    return 0;
}

/* Function start: 0x42E900 */
void wait_for_end_of_music(void)
{
    if (nMusicPlaybackMode != 0 && nMusicPlaybackMode != 3) {
        if (nWaitForMusicEnabled == 0) {
            StopMusic(0);
            return;
        }
        SetMusBreakpt(0, 0);
        do {
            if (nMusicTrackComplete != 0)
                return;
        } while (bEscapePressed == 0 && CheckEscaped() == 0);
        StopMusic(0);
    }
}

/* Function start: 0x42E9E0 */
void new_space_music_changes(short attacker, short victim)
{
    enum Side side;

    if (nInFlightMusicActive != 0 &&
        nTrainSimActive == 0) {
        side = aeShipSide[victim];
        if (side == SIDE_KILRATHI) {
            if (report_kilrathi_rout(1) == 0) {
                spacetrack(10, 1, 0);
                return;
            }
            if (attacker == 0) {
                if (acShipRating[victim] == -1 &&
                    RandomInRange(0, 3) != 0) {
                    spacetrack(6, 3, 0);
                    return;
                }
                spacetrack(9, 3, 0);
                return;
            }
        } else {
            if (nYourWingman == victim) {
                spacetrack(8, 3, 0);
                return;
            }
            if (side == SIDE_IMPERIAL) {
                if ((aeShipMissionType[0] ==
                         MISSION_TYPE_DEFEND ||
                     aeShipMissionType[0] ==
                         MISSION_TYPE_ESCORT) &&
                    nShipMissionIndices[victim] ==
                        anShipMissionShip[0]) {
                    spacetrack(11, 3, 0);
                    return;
                }
                spacetrack(7, 3, 0);
            }
        }
    }
}

/* Function start: 0x42EAD0 */
int changetrack(void)
{
    int track;

    switch (aeShipMissionType[0]) {
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
    if (aMissionObjectives[
            cCurrentObjective].type == OBJECTIVE_HOME_BASE) {
        if (triumph(0) != 0) {
            if (aeShipMissionType[0] == MISSION_TYPE_PATROL)
                return 13;
            return 14;
        }
        track = 12;
    }
    return track;
}

/* Function start: 0x42EB60 */
void gametrack(void)
{
    int track;
    short damage;

    track = -1;
    if (nInFlightMusicActive != 0) {
        if (nTrainSimActive != 0) {
            if (nMusicStreamSet != 0 ||
                nCurrentMusicTrack != 20)
                spacetrack(20, 1, 0);
            SoundDebugPrintf("%d %d\n", nMusicStreamSet,
                             nCurrentMusicTrack);
            return;
        }
        if (nCombatMusicActive != 0) {
            if ((nSpaceFrame & 0xf) == 0 ||
                nMusicTrackComplete != 0) {
                if (nInitialFlightMusicPending != 0)
                    nInitialFlightMusicPending = 0;
                if (missile_on_tail(0) != 0) {
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
                    nCombatMusicActive = 0;
            }
        } else if ((nSpaceFrame & 0xf) == 0 ||
                   nMusicTrackComplete != 0) {
            track = changetrack();
            if (report_kilrathi_rout(2) != 0)
                nCombatMusicActive = 1;
        }
        spacetrack(track, 1, 0);
    }
}

/* Function start: 0x42ECB0 */
void servicetrack(void)
{
    short object;
    FixedVector futurePosition;
    FixedVector travel;

    gametrack();
    if (nFlightSoundEffectsEnabled != 0) {
        for (object = 0; object < WC1_SPACE_OBJECT_COUNT; object++) {
            if (object == nPassingShipSoundObject) {
                if (aeObjectClass[object] !=
                        OBJECT_CLASS_SHIP ||
                    aeObjectClass[object] !=
                        OBJECT_CLASS_CAPITAL_SHIP)
                    nPassingShipSoundObject = -1;
            }
            if (aeObjectClass[object] ==
                    OBJECT_CLASS_ASTEROID) {
                if (asObjectDistance[object] == 0 &&
                    (unsigned short)
                        asPreviousObjectDistance[object] < 50 &&
                    aiSoundEffectSourceActive[object + 1] == 0)
                    PlaySfxWaveFileByNumber(6, object, 0);
            } else if (aeObjectClass[object] >=
                           OBJECT_CLASS_SHIP &&
                       aeObjectClass[object] <=
                           OBJECT_CLASS_CAPITAL_SHIP &&
                       asObjectScreenX[object] !=
                           (short)0x8001 &&
                       (unsigned short)
                           asObjectDistance[object] < 0x55a) {
                if (nPassingShipSoundObject == -1) {
                    ScaleFixedVector(&aShipVelocity[object],
                                     0x1400, &travel);
                    AddFixedVectors(&aShipPosition[object],
                                    &travel, &futurePosition);
                    ComputeVectorDelta(
                        &aShipPosition[WC1_EYE_OBJECT],
                        &futurePosition, &travel);
                    ComputeVectorDelta(
                        &aShipPosition[WC1_EYE_OBJECT],
                        &aShipPosition[object],
                        &futurePosition);
                    if (dot_product(&travel, &futurePosition) < 0xdd) {
                        nPassingShipSoundCountdown = 10;
                        nPassingShipSoundObject = object;
                        if (nPassingShipSoundCooldown <
                            nSpaceFrame) {
                            nPassingShipSoundCooldown =
                                nSpaceFrame + 6;
                            PlaySfxWaveFileByNumber(2, object, 0);
                        }
                    }
                } else if (object ==
                               nPassingShipSoundObject) {
                    nPassingShipSoundCountdown--;
                    if (nPassingShipSoundCountdown == 0)
                        nPassingShipSoundObject = -1;
                }
            }
        }
    }
}

/* Function start: 0x42EE80 */
void ResetSoundState(void)
{
    FlushSoundEffects();
    bAfterburnerSfxActive = 0;
    nDamageAlarmSfxHandle = 0;
}

/* Function start: 0x42EEA0 */
void ResetSoundStateForScene(void)
{
    ResetSoundState();
    nFlightSoundEffectsEnabled = 0;
}

/* Function start: 0x42EEB0 */
void ResetSoundStateForFlight(void)
{
    ResetSoundState();
    nFlightSoundEffectsEnabled = 1;
}

/* Function start: 0x42EEE0 */
void EnableMusicForScene(void)
{
    nWaitForMusicEnabled = 1;
    SetMusicOn(1);
}

/* Function start: 0x42EF00 */
unsigned int SoundFxTick(void)
{
    WriteDebugString("soundFX");
    return 0;
}

/* Function start: 0x42EF10 */
void FlushSoundEffectsAndLog(void)
{
    FlushSoundEffects();
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

#ifdef WC1_SDL
    if (Wc1SdlUsingDosData()) {
        int volume;

        volume = 127;
        if (sourceObject != -1) {
            if (sourceObject < 0 ||
                sourceObject >= WC1_SPACE_OBJECT_COUNT)
                return;
            ComputeVectorDelta(
                &aShipPosition[WC1_EYE_OBJECT],
                &aShipPosition[sourceObject], &delta);
            distance = (int)((Vector_magnitude(&delta) / 500L) >> 8);
            volume -= distance;
            if (volume < 0)
                volume = 0;
        }
        if (volume >= 10 && Wc1SdlPlayDosSoundEffect(
                soundNumber, volume, 64, sourceObject, looping)) {
            aiSoundEffectSourceActive[sourceObject + 1] = 1;
            if (sourceObject == -1)
                bAfterburnerSfxActive = soundNumber == 12;
        }
        return;
    }
#endif

    if (sourceObject != -1) {
        ComputeVectorDelta(&aShipPosition[WC1_EYE_OBJECT],
                           &aShipPosition[sourceObject],
                           &delta);
        distance = Vector_magnitude(&delta);
        if (distance > 32000)
            distance = 32000;
    } else {
        distance = 32000;
    }
    if (distance >= 10) {
        aiSoundEffectSourceActive[sourceObject + 1] = 1;
        sprintf(szSfxWavePath, szSfxWaveFormat,
                soundNumber - 1);
        playWAVE(szSfxWavePath, looping, distance);
    }
}
