/*
 *  Scripted views, constellation effects, and cockpit target rendering.
 *
 *  Address range 42cdb0-42e31f (see docs/ORDER.md).
 *  Packet and viewport helpers remain in this provisional contiguous unit.
 */
#define WC1_VIEW_SOURCE
#include "wc1.h"

short nScriptedViewObject = -1; /* 0x0046a8d0 */
int bScriptedView = 0; /* 0x0046a8d4 */
static short asConstellationVelocity[16] = { /* 0x0046a8d8 */
    8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1
};
static short asConstellationFrame[16] = { /* 0x0046a8f8 */
    0, 16, 16, 0, 4, 4, 20, 20, 24, 8, 8, 24, 28, 12, 12, 28
};
static short nConstellationDirection = -1; /* 0x0046a918 */
static unsigned char *pPacketDecompressionWorkspace = 0; /* 0x0046a91c */
static unsigned short wPacketDecompressionInputSizeOverride = 0; /* 0x0046a920 */
static unsigned short wPacketCompressionFormatFlags = 0; /* 0x0046a924 */
static ShortRect aTargetArmorClipRects[4] = { /* 0x0046a928 */
    { 12, -20, 29, 20 },
    { -11, 1, 11, 20 },
    { -11, -20, 11, 0 },
    { -29, -20, -12, 20 }
};


/* Linker commons defined here; LINK orders them by identifier hash. */
const char *pszCommMenuHeading; /* 0x0059e490 */
char szCommMenuHeadingBuf[0x40]; /* 0x0059e4a0 */
const char *apszCommMenuChoiceTextTable[7]; /* 0x0059e4e0 */
int anInputButton2PressTime[2]; /* 0x0059e500 */
short asInputButton1DoubleClicksList[2]; /* 0x0059e508 */
short asInputBtn1ChangedTable[2]; /* 0x0059e50c */
short asInputButton2ChangedTable[2]; /* 0x0059e510 */
short asPreviousInputButtons[2]; /* 0x0059e514 */
int anInputButton1PressTimes[2]; /* 0x0059e518 */

/* Function start: 0x42CDB0 */
unsigned int parse_view_script(void)
{
    FixedVector vector;
    short obj;
    short missionIndex;

    if (*pCurrentViewScriptPtr == 13)
        goto done;
    for (;;) {
        if (*pCurrentViewScriptPtr == -1) {
            bScriptedView = 0;
            nScriptedViewObject = -1;
            return 0;
        }
        if (*pCurrentViewScriptPtr == 14)
            break;
        switch (*pCurrentViewScriptPtr++) {
        case 0:
#ifdef SDL_PORT
            aShipPositionsArray[61].x =
                (int)*pCurrentViewScriptPtr++ * 0x100;
            aShipPositionsArray[61].y =
                (int)*pCurrentViewScriptPtr++ * 0x100;
            aShipPositionsArray[61].z =
                (int)*pCurrentViewScriptPtr++ * 0x100;
#else
            aShipPositionsArray[61].x =
                (int)*pCurrentViewScriptPtr++ << 8;
            aShipPositionsArray[61].y =
                (int)*pCurrentViewScriptPtr++ << 8;
            aShipPositionsArray[61].z =
                (int)*pCurrentViewScriptPtr++ << 8;
#endif
            break;
        case 1:
            alter_yaw(*pCurrentViewScriptPtr++, 61);
            alter_pitch(*pCurrentViewScriptPtr++, 61);
            alter_roll(*pCurrentViewScriptPtr++, 61);
            break;
        case 2:
#ifdef SDL_PORT
            ScaleFixedVector(&aShipFwdVec[61],
                             (int)*pCurrentViewScriptPtr++ * 0x100,
                             &aShipVelocity[61]);
#else
            ScaleFixedVector(&aShipFwdVec[61],
                             (int)*pCurrentViewScriptPtr++ << 8,
                             &aShipVelocity[61]);
#endif
            break;
        case 3:
            force_view(*pCurrentViewScriptPtr++,
                       nScriptedViewObject);
            break;
        case 4:
            nEyePitchTargetValue =
                (short)-*pCurrentViewScriptPtr++;
            nEyePitchRate = *pCurrentViewScriptPtr++;
            break;
        case 5:
            nEyePitchTargetValue = *pCurrentViewScriptPtr++;
            nEyePitchRate = *pCurrentViewScriptPtr++;
            break;
        case 6:
            nEyeYawGoalValue = *pCurrentViewScriptPtr++;
            nEyeYawRate = *pCurrentViewScriptPtr++;
            break;
        case 7:
            nEyeYawGoalValue =
                (short)-*pCurrentViewScriptPtr++;
            nEyeYawRate = *pCurrentViewScriptPtr++;
            break;
        case 8:
            nEyeRollTargetValue = *pCurrentViewScriptPtr++;
            nEyeRollRate = *pCurrentViewScriptPtr++;
            break;
        case 9:
            copy_frame(61, 63);
            alter_yaw(*pCurrentViewScriptPtr++, 63);
            alter_pitch(*pCurrentViewScriptPtr++, 63);
            alter_roll(*pCurrentViewScriptPtr++, 63);
#ifdef SDL_PORT
            ScaleFixedVector(&aShipFwdVec[63],
                             (int)*pCurrentViewScriptPtr++ * 0x100,
                             &vector);
#else
            ScaleFixedVector(&aShipFwdVec[63],
                             (int)*pCurrentViewScriptPtr++ << 8,
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
            aShipPositionsArray[61] =
                aShipPositionsArray[nScriptedViewObject];
            break;
        case 15:
            ComputeVectorDelta(&aShipPositionsArray[61],
                &aShipPositionsArray[nScriptedViewObject],
                &vector);
            aShipFwdVec[61] = vector;
            fix_objects_ijk(61);
            break;
        case 16:
            obj = 0;
            missionIndex = *pCurrentViewScriptPtr;
            while (obj < 10) {
                if (nShipMissionIndices[obj] == missionIndex) {
                    nScriptedViewObject = obj;
                    break;
                }
                obj++;
            }
            pCurrentViewScriptPtr++;
            break;
        }
        if (*pCurrentViewScriptPtr == 13)
            return 0;
    }
    asObjectCounterArray[61] = pCurrentViewScriptPtr[1];
done:
    return 0;
}

/* Function start: 0x42D1C0 */
unsigned int update_scripted_view(void)
{
    short counter;
    int command;

    command = *pCurrentViewScriptPtr;
    switch (command) {
    case 13:
        if ((nEyeYawGoalValue == nEyePitchTargetValue) !=
            nEyeRollTargetValue) {
            pCurrentViewScriptPtr++;
            parse_view_script();
        }
        break;
    case 14:
        counter = asObjectCounterArray[61];
        asObjectCounterArray[61]--;
        if (counter < 1) {
            pCurrentViewScriptPtr += 2;
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
    pCurrentViewScriptPtr = script;
    parse_view_script();
    asObjectCollisionRadius[61] = 100;
}

/* Function start: 0x42D270 */
unsigned int InitializeFireworks(void)
{
    short i = 0;

    do {
        aFireworksList[i].frame = -1;
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
        if (aFireworksList[index].frame == -1) {
            emptyCount++;
        } else {
            DrawSpriteDefault(
                viewport, aFireworksList[index].x,
                aFireworksList[index].y,
                pFireworkShape,
                (short)(aFireworksList[index].frame +
                        aFireworksList[index].variant * 8));
            if (aFireworksList[index].frame++ == 7) {
                aFireworksList[index].frame = -1;
                FlushSoundEffectsAndLog(
                    aFireworksList[index].soundHandle, index);
            } else if (aFireworksList[index].frame == 1) {
                aFireworksList[index].soundHandle =
                    ((unsigned int (__cdecl *)(
                        const unsigned char *, int, int, short, short,
                        int))SoundFxTick)(
                            aabSoundEffectDescriptors[36],
                            0, 127,
                            (signed char)(
                                127 -
                                (int)aFireworksList[index].x *
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
    short viewportHeight;
    unsigned short starHeight;
    short index;
    int particleOffset;
    short randomIndex;
    short width;
    int population;

    pConstellationViewportPtr = viewport;
    nConstellationDirection = direction;
    width = (short)(viewport->right - viewport->left);
    viewportHeight = (short)(viewport->bottom - viewport->top);
    population = density;
    nViewConstellationStarCount = (short)(population * 10 / 16);
    population *= 16;
    nConstellationParticleTotal = (short)(population / 16);
    index = 0;
    if (index < nViewConstellationStarCount) {
        starHeight = viewportHeight;
        do {
            aConstellationStarsTable[index].x =
                RandomInRange(0, width);
            aConstellationStarsTable[index].y =
                RandomInRange(0, starHeight);
            aConstellationStarsTable[index].frame =
                (short)(RandomInRange(0, 5) + 32);
            index++;
        } while (index < nViewConstellationStarCount);
    }
    index = 0;
    while (index < nConstellationParticleTotal) {
        randomIndex = RandomInRange(0, 15);
        particleOffset = (int)index * sizeof(ConstellationParticle);
        index++;
        ((ConstellationParticle *)((unsigned char *)aConstellationParticlesList +
                                  particleOffset))->x =
            (short)(pConstellationViewportPtr->left +
                    RandomInRange(0, width));
        ((ConstellationParticle *)((unsigned char *)aConstellationParticlesList +
                                  particleOffset))->y =
            (short)(pConstellationViewportPtr->top +
                    RandomInRange(0, viewportHeight));
        ((ConstellationParticle *)((unsigned char *)aConstellationParticlesList +
                                  particleOffset))->velocity =
            (short)(asConstellationVelocity[randomIndex] *
                    nConstellationDirection);
        ((ConstellationParticle *)((unsigned char *)aConstellationParticlesList +
                                  particleOffset))->frame =
            (short)(asConstellationFrame[randomIndex] +
                    RandomInRange(0, 3));
    }
    return 0;
}

/* Function start: 0x42D500 */
unsigned int DrawConstellationField(void)
{
    short height;
    short index;
    int tableOffset;
    short speed;

    height = (short)(pConstellationViewportPtr->bottom -
                     pConstellationViewportPtr->top);
    ClearViewport(pConstellationViewportPtr, cPrimaryViewBufferColour);
    for (index = 0;
         index < nViewConstellationStarCount;
         index++) {
        DrawSpriteDefault(pConstellationViewportPtr,
                          aConstellationStarsTable[index].x,
                          aConstellationStarsTable[index].y,
                          pCurrentConstellationShape,
                          aConstellationStarsTable[index].frame);
    }
    for (index = 0;
         index < nConstellationParticleTotal;
         index++) {
        DrawSpriteDefault(pConstellationViewportPtr,
                          aConstellationParticlesList[index].x,
                          aConstellationParticlesList[index].y,
                          pCurrentConstellationShape,
                          aConstellationParticlesList[index].frame);
        aConstellationParticlesList[index].x = (short)(
            aConstellationParticlesList[index].x +
            aConstellationParticlesList[index].velocity);
        aConstellationParticlesList[index].frame = (short)(
            (aConstellationParticlesList[index].frame & 0xfc) +
            (aConstellationParticlesList[index].frame + 1) % 4);
        if (nConstellationDirection < 0) {
            if (aConstellationParticlesList[index].x >=
                pConstellationViewportPtr->left)
                continue;
            tableOffset = (int)RandomInRange(0, 15) * sizeof(short);
            speed = *(short *)((unsigned char *)asConstellationVelocity +
                              tableOffset);
            aConstellationParticlesList[index].x = (short)(
                pConstellationViewportPtr->right -
                RandomInRange(0, speed));
            aConstellationParticlesList[index].y = (short)(
                pConstellationViewportPtr->top +
                RandomInRange(0, height));
            aConstellationParticlesList[index].velocity = (short)-speed;
        } else {
            if (aConstellationParticlesList[index].x <=
                pConstellationViewportPtr->right)
                continue;
            tableOffset = (int)RandomInRange(0, 15) * sizeof(short);
            speed = *(short *)((unsigned char *)asConstellationVelocity +
                              tableOffset);
            aConstellationParticlesList[index].velocity = speed;
            aConstellationParticlesList[index].x = (short)(
                pConstellationViewportPtr->left +
                RandomInRange(0, speed));
            aConstellationParticlesList[index].y = (short)(
                pConstellationViewportPtr->top +
                RandomInRange(0, height));
        }
        aConstellationParticlesList[index].frame = (short)(
            *(short *)((unsigned char *)asConstellationFrame +
                       tableOffset) +
            RandomInRange(0, 3));
    }
    return 0;
}

/* Function start: 0x42D730 */
short __stdcall OpenPacketSection(const char *filename, short section,
                                  PacketSectionHandle *handle)
{
    unsigned int sectionEntry;
    unsigned int sectionEnd;
    int sectionIndex;
    int directoryOffset;
    unsigned int directorySize;
    short compression;
    short sectionCount;
    short file;

    {
        unsigned int fileSize;

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
        directoryOffset = sectionIndex * 4 + 4;
        if (ReadDataFileAtOffset(file, directoryOffset, 4,
                                 &sectionEntry) == 0)
            goto failed;
        compression = (short)(sectionEntry >> 24) & 0xff;
        handle->finalSection = 0;
        sectionEntry &= 0x00ffffff;
        if (sectionCount - sectionIndex == 1) {
            handle->finalSection = 1;
            sectionEnd = fileSize;
        } else {
            directoryOffset += 4;
            if (ReadDataFileAtOffset(file, directoryOffset, 4,
                                     &sectionEnd) == 0)
                goto failed;
            /* Discard the next directory entry's compression byte. */
            ((unsigned char *)&sectionEnd)[3] = 0;
        }
    }
    {
        unsigned int sectionSize;

        sectionSize = sectionEnd - sectionEntry;
        if (SeekDataFile(file, sectionEntry, 0) != 0) {
            handle->file = file;
            handle->sectionCount = sectionCount;
            handle->compression = compression;
            handle->dataOffset = sectionEntry;
            handle->position = 0;
            handle->dataSize = *(volatile unsigned int *)&sectionSize;
            return 1;
        }
    }
failed:
    CloseDataFile((unsigned short)file);
    return 0;
}

/* Function start: 0x42D870 */
void __stdcall CloseDataFileByHandle(PacketSectionHandle *handle)
{
    CloseDataFile(handle->file);
}

/* Function start: 0x42D880 */
void * __stdcall DecompressPacketSection(
    PacketSectionHandle *handle, void *destination, unsigned short flags,
    void *decompressionWorkspace)
{
    unsigned int allocationSize;
    void *packet;
    void *workspace;
    void *largeScratch;
    void *smallScratch;
    unsigned char fallbackAllocations;

    fallbackAllocations = 0;
    packet = 0;
    pLastPacketAllocation = 0;
    if (wPacketCompressionFormatFlags != 0) {
        if ((handle->compression & 0xc0) != 0) {
            nPacketError = 6;
            return 0;
        }
    } else if (handle->compression != 1) {
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
            workspace = IdentityHandle(largeScratch);
            pPktDecompInput = IdentityHandle(smallScratch);
            wPacketDecompressInputSize = 0x400;
            goto initializeDecompressor;
        }
    }

    workspace = IdentityHandle(decompressionWorkspace);
    wPacketDecompressInputSize = 0x400;
    allocationSize = 0x3020;
    pPktDecompInput =
        ((PacketDecompressionWorkspace *)decompressionWorkspace)->input;
    pPktDecompInput =
        IdentityHandle(pPktDecompInput);
    if (wPacketDecompressionInputSizeOverride != 0)
        wPacketDecompressInputSize =
            wPacketDecompressionInputSizeOverride;

initializeDecompressor:
    nPktDecompressSourceFile = handle->file;
    nPacketDecompressInputPositionValue = 0;
    nPacketDecompressPending = 0;
    nCurrentPacketDecompressScratchSegment =
        (short)(unsigned int)workspace;
    if (workspace != 0)
        nCurrentPacketDecompressScratchSegment++;

    if (SeekPacketSection(handle, 0, 0) == -1) {
        nPacketError = 5;
        return 0;
    }
    if (destination == 0) {
        if (ReadPacketSectionData(handle, (void *)&allocationSize, 4) != 0) {
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
                handle, pPktDecompInput,
                wPacketDecompressInputSize) == 0)
            packet = 0;
        nPacketDecompressResult = GetPacketDecompressionResult(packet);
        if (packet != 0)
            FinishPacketDecompressionHook();
    }
    if (decompressionWorkspace == 0) {
        /* DOS near and far frees both map to ReleasePacketHandle in Win32. */
        if (fallbackAllocations != 0) {
            ReleasePacketHandle(smallScratch);
            ReleasePacketHandle(largeScratch);
        } else {
            ReleasePacketHandle(smallScratch);
            ReleasePacketHandle(largeScratch);
        }
    }
    return packet;
}

/* Function start: 0x42DB70 */
short GetTargetColourIndex(void)
{
    short v = nVideoMode;

    if (nVideoMode == -1)
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

    DrawTextAt(&stRightVduTxtContext,
               stRightVduViewport.left, stRightVduViewport.top,
               "", 2);
    if (nTargetLockMode != 0) {
        DrawFormattedText("%F%s%F",
                          (unsigned int)cRedColour,
                          "   LOCKED TARGET",
                          (unsigned int)cDefaultTextColour);
    } else {
        DrawFormattedText("%F%s",
                          (unsigned int)cDefaultTextColour,
                          "  AUTO TARGETTING");
    }
    target = acShipTargetTable[0];
    if (target != -1 &&
        (aeObjectClass[target] < OBJECT_CLASS_SHIP ||
         aeSpecialManeuvers[target] ==
             SPECIAL_MANEUVER_UNKNOWN_9)) {
        target = -1;
        acShipTargetTable[0] = -1;
    }
    cTargetDisplayObject = (signed char)target;
    DrawFormattedText("\nTarget:");
    if (target == -1) {
        DrawFormattedText(" None");
        return;
    }
    targetIndex = (int)target;
    objectType = aeObjectTypeList[targetIndex];
    typeData = &aObjectTypeData[objectType];
    rating = acShipRating[targetIndex];
    if (rating >= 0 && rating <= 7) {
        DrawFormattedText(
            " %s",
            apWingmanPilotsList[(int)rating]->callsign);
    } else if (rating >= 9 && rating <= 12) {
        DrawFormattedText(
            " %s",
            apszKilrathiAceNames[(int)rating - 9]);
    } else {
        DrawFormattedText(" %s",
                          typeData->displayName);
    }
    DrawFormattedText("\nRange : ");
    InitializeCockpitReadout(1, &stRightVduTxtContext);
    if (asObjectScreenXList[targetIndex] == (short)0x8001) {
        cTargetDisplayObject = -1;
        return;
    }

    x = (short)(stRightVduViewport.left + 0x25);
    y = (short)(stRightVduViewport.top + 0x26);
    frame = (short)((aasShipShield[targetIndex][1] * 6) /
                    typeData->shieldAft);
    frame = (short)((3 - MinShort(frame, 3)) * 2);
    if (frame < 6)
        DrawSpriteDefault(&stRightVduViewport, x, y,
                          pCockpitIndicatorShape, frame);

    targetViewport = stRightVduViewport;
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

    DrawSpriteDefault(&stRightVduViewport, x, y, typeData->shape, 2);
    frame = (short)((aasShipShield[targetIndex][0] * 6) /
                    typeData->shieldFore);
    frame = (short)((3 - MinShort(frame, 3)) * 2);
    if (frame < 6)
        DrawSpriteDefault(&stRightVduViewport, x, y,
                          pCockpitIndicatorShape,
                          (short)(frame + 1));
}

/* Function start: 0x42DEA0 */
void DrawTargetRangeReadout(void)
{
    short target;

    target = acShipTargetTable[0];
#ifdef SDL_PORT
    /* The original tests the table before the -1 sentinel.  At target -1 it
       reads the zero-filled gap at 0x0059c3bc, so the comparison is false. */
    if (target != -1 &&
#else
    if (
#endif
        aeSpecialManeuvers[target] ==
        SPECIAL_MANEUVER_UNKNOWN_9) {
        acShipTargetTable[0] = -1;
        InvalidateVduMode(1);
        return;
    }
    if (target != -1 &&
        aeObjectClass[target] < OBJECT_CLASS_SHIP) {
        acShipTargetTable[0] = -1;
        target = -1;
    }
    if (cTargetDisplayObject != target ||
        (short)(nRenderedSpaceFrame % 8) == 0) {
        set_new_vdu(1);
        show_target_disp();
    }
    if (target == -1)
        return;

    if (asObjectScreenXList[target] != (short)0x8001) {
        if ((unsigned short)asObjDistancesTable[target] <= 30000)
            goto format_range;
        strcpy(szTxtWorkspaceBuffer, "TOO FAR");
        goto range_ready;
format_range:
        strcpy(szTxtWorkspaceBuffer + strlen(
            itoa((unsigned short)asObjDistancesTable[target],
                 szTxtWorkspaceBuffer, 10)), " m");
    } else {
        strcpy(szTxtWorkspaceBuffer, "----- m");
    }

range_ready:
    DrawCockpitReadout(1, szTxtWorkspaceBuffer);
    if (nTargetLockCountdown == 0) {
        if (bTargetLockAcquired == 1) {
            bTargetLockAcquired = 0;
            return;
        }
    } else if (bTargetLockReadoutDirty != 0) {
        EraseCockpitReadoutRegion(&stRightVduViewport,
                                  stRightVduViewport.left,
                                  (short)(stRightVduViewport.bottom - 6),
                                  stRightVduViewport.right,
                                  stRightVduViewport.bottom,
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
    unsigned short top;
    short left;
    unsigned short width;
    unsigned short height;
    unsigned short row;
    short offset;
    int rowCount;

    top = viewport->top;
    height = viewport->bottom - top + 1;
    width = viewport->right - viewport->left + 1;
    left = viewport->left;
    nAllocateViewportCallsValue++;
    if (nVideoMode != 0x13)
        LogDisplayMode("not MCGA");
    viewport->allocation = AllocateTaggedMemory(
        (unsigned int)width * height, (unsigned short)(flags + 2));
    apViewportAllocations[
        nViewAllocTotal++] = viewport->allocation;
    if (viewport->allocation == 0)
        return 0;
    viewport->pixels = viewport->allocation;

    offset = 0;
    viewport->rowOffsets = AllocateTaggedMemory(
        (top + (unsigned int)height) * sizeof(unsigned short) + 4,
        0);
    if (viewport->rowOffsets == 0) {
        if (nVideoMode != 0x13)
            ReleasePacketHandle(viewport->allocation);
        return 0;
    }

    rowCount = height + 2;
    row = 0;
    while ((int)row < rowCount) {
        viewport->rowOffsets[top + row] = offset - left;
        offset += width;
        row++;
    }
    if (clearColour != -1)
        ClearViewport(viewport, clearColour);
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
