/*
 *  Disk data files and packet fetching with retry.
 *
 *  Address range 0x41d250-0x41efff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: OpenDiskDataFile/FetchDiskPacketRetrying/PromptInsertNumberedDisk.
 */
#include "wc1.h"

/* Function start: 0x41D120 */
void ReportPacketLoadError(void *packet, short logicalFile,
                           short retry, short section,
                           const char *sourceTag)
{
    short error;
    unsigned int packetSize;
    const char *operation;

    error = nPacketError;
    if ((packet == 0 || (error != 0 && error != 8)) &&
        (packet != 0 || error != 8)) {
        if (section != -1)
            packetSize = GetPacketSize(
                pDiskFileRecords[logicalFile].name, section);
        LogMemoryUsage();
        operation = "allocating memory";
        if (packet != 0 && section != -1)
            operation = "reading from disk";
        nPacketError = error;
        sprintf(szDefaultTextBuffer,
                "Sorry, an error has occured while %s.\n"
                "Please note the following information:\n"
                "%s #%d (ERR %d  PS%ld  LB%ld  FL%d) at %s\n"
                "Check your configuration.  If this problem persists, please\n"
                "call Origin Systems' service line.  We are sorry for the inconvenience.",
                operation,
                pDiskFileRecords[logicalFile].name,
                (int)section, (int)error, packetSize,
                GetFixedOneMillionThunkAlt(retry), (int)retry, sourceTag);
        FatalErrorAndExit(szDefaultTextBuffer);
    }
}

/* Function start: 0x41D200 */
void *LoadPacketIntoBuffer(short logicalFile, short section,
                           void *destination)
{
    void *packet;

    PromptInsertNumberedDisk(logicalFile);
    packet = PacketLoad(
        pDiskFileRecords[logicalFile].name,
        section, destination, 0, 0);
    ReportPacketLoadError(destination, logicalFile, 0, section, "RP");
    return packet;
}

/* Function start: 0x41D250 */
void *LoadPacketAllocated(short logicalFile, short section)
{
    unsigned int packetSize;
    void *packet;
    short retries;

    retries = 5;
    PromptInsertNumberedDisk(logicalFile);
    packetSize = GetPacketSize(
        pDiskFileRecords[logicalFile].name, section);
    packet = AllocateTaggedMemory((unsigned int)(short)packetSize, 0x40);
    if (packet != 0) {
        do {
            retries--;
            PacketLoad(pDiskFileRecords[logicalFile].name,
                       section, packet, 0, 0);
            if (retries <= 0 || nPacketError == 0)
                break;
        } while (nPacketError != 8);
    }
    ReportPacketLoadError(packet, logicalFile, 0, section, "LPN");
    return packet;
}

/* Function start: 0x41D2E0 */
void *FetchDiskPacketRetrying(short logicalFile, short section,
                              unsigned short flags)
{
    void *packet = 0;
    short retries = 5;
    const char *fileName;

    PromptInsertNumberedDisk(logicalFile);
    if (flags == 0) {
        if (GetPacketSize(
                pDiskFileRecords[logicalFile].name, section) >
            (int)GetFixedOneMillionThunkAlt(0)) {
            ReportPacketLoadError(0, logicalFile, 0, section, "LP1");
        }
    }
    fileName = pDiskFileRecords[logicalFile].name;

    do {
        retries--;
        FreePacketAndClear(&packet, flags);
        packet = PacketLoad(fileName, section, 0, flags, 0);
        if (retries < 1 || nPacketError == 0)
            break;
    } while (nPacketError != 8);

    if (packet == 0) {
        if (stSpaceBuffer.pixels != 0) {
            free_viewport(&stSpaceBuffer);
            do {
                retries--;
                FreePacketAndClear(&packet, flags);
                packet = PacketLoad(fileName, section, 0, flags, 0);
                if (retries < 1 || nPacketError == 0)
                    break;
            } while (nPacketError != 8);
            if (AllocateViewport(&stSpaceBuffer,
                                 (short)cPrimaryViewBufferColour, 0x20) == 0) {
                ReportPacketLoadError(0, logicalFile, flags, flags,
                                      "LP2");
            }
        }
        if (packet == 0 && stSceneBuffer.pixels != 0) {
            free_viewport(&stSceneBuffer);
            do {
                retries--;
                FreePacketAndClear(&packet, flags);
                packet = PacketLoad(fileName, section, 0, flags, 0);
                if (retries < 1 || nPacketError == 0)
                    break;
            } while (nPacketError != 8);
            if (AllocateViewport(&stSceneBuffer,
                                 (short)cBlackColour, 0) == 0) {
                ReportPacketLoadError(0, logicalFile, flags, section,
                                      "LP3");
            }
        }
    }
    if (packet == 0 && (flags & 4) == 0 &&
        nPacketError != 0 && nPacketError != 8) {
        ReportPacketLoadError(packet, logicalFile, flags, section, "LP4");
    }

    ClearInputKeyStatePreservingModifiers();
    return packet;
}

/* Function start: 0x41D510 */
unsigned int InitializeTextContextFromFont(TextContext *context,
                                           short fontIndex,
                                           unsigned char colour,
                                           signed char background)
{
    int index;

    index = fontIndex;
    if (apTextFonts[index] == 0) {
        if (fontIndex == 1) {
            apTextFonts[index] =
                FetchDiskPacketRetrying(0, fontIndex,
                                                         0x10);
        } else {
            apTextFonts[index] =
                FetchDiskPacketRetrying(0, fontIndex,
                                                         0);
        }
        apFontWorkspaces[index] =
            AllocateFontWorkspace((short)index);
    }
    context->font = apTextFonts[index];
    context->colour = colour;
    context->backgroundColour = (unsigned char)background;
    context->fontWorkspace = apFontWorkspaces[index];
    SetTextContext(context);
    return 0;
}

/* Function start: 0x41D590 */
unsigned int ReleaseTextFont(short fontIndex)
{
    int index;

    if (fontIndex == 1)
        return 0;
    index = fontIndex;
    if (apTextFonts[index] != 0) {
        ReleasePacketHandle(apTextFonts[index]);
        apTextFonts[index] = 0;
        FreeFontWorkspace(apFontWorkspaces[index]);
        apFontWorkspaces[index] = 0;
    }
    return 0;
}

/* Function start: 0x41D5F0 */
unsigned int DrawTextAt(TextContext *context, short x, short y,
                        const char *text, unsigned char alignment)
{
    char *savedText = context->text;
    unsigned char savedAlignment = context->alignment;

    SetTextContext(context);
    SetTextCursor((unsigned short)x, (unsigned short)y);
    context->text = (char *)text;
    context->alignment = alignment;
    DrawTextString(text);
    context->text = savedText;
    context->alignment = savedAlignment;
    if (context->viewport->pixels == stScreen.pixels)
        DIBslam();
    return 0;
}

/* Function start: 0x41D650 */
unsigned int SortSignedByteValuesAscending(signed char *values,
                                           short count)
{
    signed char value;
    short next;
    signed char outer;
    signed char inner;

    outer = 0;
    if (0 < count - 1) {
        do {
            inner = (signed char)(outer + 1);
            next = (short)inner;
            if (next < count) {
                do {
                    value = values[outer];
                    if (values[inner] < value) {
                        values[outer] = values[inner];
                        values[inner] = value;
                    }
                    inner++;
                } while ((short)inner < count);
            }
            outer++;
        } while ((int)next < count - 1);
    }
    return 0;
}

/* Function start: 0x41D6C0 */
short OpenDiskDataFile(short logicalFile)
{
    short file;

    FillGraphicSuffix(szDiskMarkerFile,
                      (unsigned char)
                          pDiskFileRecords[logicalFile].diskNumber,
                      3);
    file = OpenDataFileOrDie(szDiskMarkerFile);
    if (file != -1) {
        CloseDataFile((unsigned short)file);
        return 1;
    }
    if (DAT_0059ab34 != 0)
        return 1;
    if (GetCurrentDiskDriveHook() == 'A') {
        if (toupper((int)(signed char)
                        abDiskPromptDriveState[1]) == 'B') {
            SelectDiskDriveHook('B');
        } else {
            return 0;
        }
    } else {
        SelectDiskDriveHook('A');
    }
    file = OpenDataFileOrDie(szDiskMarkerFile);
    if (file != -1) {
        CloseDataFile((unsigned short)file);
        return 1;
    }
    return 0;
}

/* Function start: 0x41D760 */
void __stdcall PromptInsertNumberedDisk(short logicalFile)
{
    TextContext *savedTextContext;
    short backgroundColour;
    signed char diskReady;
    signed char savedViewportMode;
    signed char diskNumber;

    savedViewportMode = 2;
    diskReady = 0;
    if (OpenDiskDataFile(logicalFile) != 0)
        return;
    if (bGraphicsActive == 0) {
        diskNumber =
            pDiskFileRecords[logicalFile].diskNumber;
        do {
            DiskPromptDrawHook();
            ResetDiskPromptTimer();
            _cprintf("Please Insert Disk %d. Press any key to continue",
                     (int)diskNumber);
            WaitForInputKey();
        } while (OpenDiskDataFile(logicalFile) == 0);
        return;
    }

    savedTextContext = pCurrentTextContext;
    SetTextContext(&stDiskPromptTextContext);
    stDiskPromptBackgroundViewport.left =
        (short)dwDiskPromptTopLeft;
    stDiskPromptBackgroundViewport.top =
        (short)(dwDiskPromptTopLeft >> 16);
    stDiskPromptViewport.left =
        (short)dwDiskPromptTopLeft;
    stDiskPromptViewport.top =
        (short)(dwDiskPromptTopLeft >> 16);
    stDiskPromptBackgroundViewport.right =
        (short)dwDiskPromptBottomRight;
    stDiskPromptBackgroundViewport.bottom =
        (short)(dwDiskPromptBottomRight >> 16);
    stDiskPromptViewport.right =
        (short)dwDiskPromptBottomRight;
    stDiskPromptViewport.bottom =
        (short)(dwDiskPromptBottomRight >> 16);

    if (stSpaceBuffer.pixels != 0) {
        stDiskPromptBackgroundViewport = stSpaceBuffer;
        backgroundColour = (unsigned char)cPrimaryViewBufferColour;
    } else if (stSceneBuffer.pixels != 0) {
        stDiskPromptBackgroundViewport = stSceneBuffer;
        backgroundColour = (unsigned char)cBlackColour;
    } else {
        savedViewportMode = (signed char)AllocateViewport(
            &stDiskPromptBackgroundViewport, -1, 0);
        backgroundColour = (short)(unsigned int)savedTextContext;
    }

    stDiskPromptBackgroundViewport.left =
        (short)dwDiskPromptTopLeft;
    stDiskPromptBackgroundViewport.top =
        (short)(dwDiskPromptTopLeft >> 16);
    stDiskPromptBackgroundViewport.right =
        (short)dwDiskPromptBottomRight;
    stDiskPromptBackgroundViewport.bottom =
        (short)(dwDiskPromptBottomRight >> 16);
    if (savedViewportMode != 0) {
        CopyViewportContents(&stDiskPromptViewport,
                             &stDiskPromptBackgroundViewport);
    }

    do {
        ClearViewport(&stDiskPromptViewport,
                      cViewportClearColour);
        SetTextCursor(
            (unsigned short)(stDiskPromptViewport.left + 2),
            (unsigned short)(stDiskPromptViewport.top + 2));
        DrawViewportBorder(
            &stDiskPromptViewport,
            stDiskPromptViewport.left,
            stDiskPromptViewport.top,
            stDiskPromptViewport.right,
            stDiskPromptViewport.bottom,
            nDiskPromptBorderColour);
        FormatTextBufferFromStart(
            "Please insert disk %d\ninto any drive\nPress any key when ready.",
            (int)pDiskFileRecords[logicalFile].diskNumber);
        DrawTextString(szTextScratchBuffer);
        WaitForInputKey();
        if (OpenDiskDataFile(logicalFile) != 0)
            diskReady++;
        if (savedViewportMode != 0) {
            CopyViewportContents(
                &stDiskPromptBackgroundViewport,
                &stDiskPromptViewport);
        } else {
            ClearViewport(&stDiskPromptViewport,
                          backgroundColour);
        }
    } while (diskReady == 0);

    if (savedViewportMode == 1) {
        free_viewport(&stDiskPromptBackgroundViewport);
    } else if (savedViewportMode == 2) {
        ClearViewport(&stDiskPromptBackgroundViewport,
                      backgroundColour);
    }
    pCurrentTextContext = savedTextContext;
}

/* Function start: 0x41DA00 */
unsigned int GetZeroUnused(void)
{
    return 0;
}

/* Function start: 0x41DA10 */
short CheckEscaped(void)
{
    InputEventState event;
    short escaped;

    PumpWindowMessages();
    escaped = 0;
    if (IsInputEventQueued(10) != 0) {
        PeekInputEvent(&event, 10);
        escaped = (short)event.value + 1;
    } else if (IsInputEventQueued(2) != 0) {
        PeekInputEvent(&event, 2);
        escaped = (short)event.value + 1;
    } else if (IsInputEventQueued(3) != 0) {
        PeekInputEvent(&event, 3);
        escaped = (short)event.value + 1;
        while (PollInputEvent(&event, 0xff) != 0)
            ;
    }
    if (escaped != 0)
        FlushInputEvents();
    return escaped;
}

/* Function start: 0x41DAA0 */
short WaitForInputKey(void)
{
    InputEventState event;
    unsigned char savedMode;
    signed char key;

    key = 0;
    if (nEventManagerActive == 0)
        return (signed char)PumpMessagesDuringWait();

    savedMode = bInputMode;
    bInputMode = 1;
    do {
        switch (PollInputEvent(&event, 0xff)) {
        case 2:
        case 10:
            key = 0x1c;
            while (PollInputEvent(&event, 0xff) != 0)
                ;
            break;
        case 3:
        case 5:
            key = (signed char)event.value;
            if (key == 0x1d) {
                key = 0;
            } else {
                if (PollInputEvent(&event, 0xff) != 0) {
                    do {
                    } while (PollInputEvent(&event, 0xff) != 0);
                }
            }
            break;
        }
    } while (key == 0);
    ClearInputKeyStatePreservingModifiers();
    bInputMode = savedMode;
    FlushInputEvents();
    return key;
}

/* Function start: 0x41DBA0 */
void WaitForSceneAdvance(short duration, short unused)
{
    InputEventState event;
    unsigned char savedMode;
    short eventType;
    short escaped;
    short advanced;

    (void)unused;
    advanced = 0;
    savedMode = bInputMode;
    bInputMode = 1;
    if (duration != -1) {
        SetFrameTimerPeriodDirect(duration);
    } else {
        escaped = CheckEscaped();
        if (escaped != 0) {
            do {
                escaped = CheckEscaped();
            } while (escaped != 0);
            SetFrameTimerPeriodDirect(0);
        }
    }
    while ((short)IsFrameTickElapsed() == 0 && advanced == 0) {
        eventType = PollInputEvent(&event, 0xff);
        switch (eventType) {
        case 2:
        case 3:
        case 5:
        case 10:
            advanced++;
            bInputMode = savedMode;
            FlushInputEvents();
            do {
                eventType = PollInputEvent(&event, 0xff);
            } while (eventType != 0);
            ClearInputKeyStatePreservingModifiers();
            break;
        }
    }
}

/* Function start: 0x41DC70 */
void MoveMenuPointerFromKeyboard(InputEventState *event)
{
    int delta;
    int moved;

    delta = nKeyboardPointerStep * 2;
    moved = 0;
    if ((short)event->value == 0x4c) {
        if (nKeyboardPointerStep == 1)
            nKeyboardPointerStep = 4;
        else
            nKeyboardPointerStep = 1;
    } else {
        switch ((short)event->value) {
        case 0x47:
            stMouseCursorState.y -= delta;
            /* fall through */
        case 0x4b:
            stMouseCursorState.x -= delta;
            break;
        case 0x49:
            stMouseCursorState.x += delta;
            /* fall through */
        case 0x48:
            stMouseCursorState.y -= delta;
            break;
        case 0x4f:
            stMouseCursorState.x -= delta;
            /* fall through */
        case 0x50:
            stMouseCursorState.y += delta;
            break;
        case 0x51:
            stMouseCursorState.y += delta;
            /* fall through */
        case 0x4d:
            stMouseCursorState.x += delta;
            break;
        default:
            goto clamp_pointer;
        }
        moved = 1;
    }

clamp_pointer:
    if (stMouseCursorState.x < 0)
        stMouseCursorState.x = 0;
    else if (stMouseCursorState.x > 320)
        stMouseCursorState.x = 320;
    if (stMouseCursorState.y < 0)
        stMouseCursorState.y = 0;
    else if (stMouseCursorState.y > 320)
        stMouseCursorState.y = 320;

    stHostMouseState.x = stMouseCursorState.x;
    stHostMouseState.y = stMouseCursorState.y;
    if (moved != 0) {
        RetainInputEventsOfType(3);
        QueueInputEvent(13, (unsigned short)stMouseCursorState.x,
                        (unsigned short)stMouseCursorState.y,
                        0, 0, 0, 0);
        bPointerMovedByKeyboard = 1;
        SetMousePosition(stHostMouseState.x,
                         stHostMouseState.y);
    }
}

/* Function start: 0x41DDF0 */
void EraseLastTextInputCharacter(void)
{
    Viewport clearArea;
    char *text;
    short textWidth;
    short length;
    short characterWidth;

    text = pCurrentTextContext->text;
    textWidth = MeasureTextPixelWidthClamped(text);
    length = DosStrlen(text);
    if (length != 0) {
        characterWidth = (short)GetFontCharWidth(text[length - 1]);
        clearArea = *pCurrentTextContext->viewport;
        clearArea.left = (short)(clearArea.left +
                                 textWidth - characterWidth);
        clearArea.right = (short)(clearArea.left + characterWidth - 1);
        clearArea.top = pCurrentTextContext->cursorY;
        clearArea.bottom = (short)(clearArea.top +
            ReadWord((unsigned short *)
                pCurrentTextContext->font) - 1);
        LeaveAllocationScope();
        ClearViewport(&clearArea,
                      pCurrentTextContext->backgroundColour);
        EnterAllocationScope();
        pCurrentTextContext->cursorX = (short)(
            pCurrentTextContext->cursorX - characterWidth);
    }
}

/* Function start: 0x41DEB0 */
short WaitForStreamInputKey(void)
{
    unsigned int saved = bKeyEventQueueEnabled;
    short key;

    bKeyEventQueueEnabled = 1;
    do {
        key = WaitForInputKey();
    } while (key == 0);
    bKeyEventQueueEnabled = saved;
    return key;
}

/* Function start: 0x41DEE0 */
short initialize_object(short obj, enum ObjectType type, short owner)
{
    if (obj != -1) {
        set_objects_data(obj, type, owner);
        zero_vector(&aShipPosition[obj]);
        zero_vector(&aShipVelocity[obj]);
    }
    return obj;
}

/* Function start: 0x41DF40 */
short borrow_dust(void)
{
    short i = 0x22;

    do {
        if (aeObjectClass[i] == OBJECT_CLASS_DUST)
            return i;
        i = i + 1;
    } while (i < 0x2a);
    return -1;
}

/* Function start: 0x41DF70 */
short new_object(enum ObjectType type, short owner)
{
    short obj;

    obj = find_vacant_3d_object();
    if (obj == -1 && owner == 0)
        obj = borrow_dust();
    return initialize_object(obj, type, owner);
}

/* Function start: 0x41DFA0 */
short initialize_ship(enum ObjectType type, short owner)
{
    short obj = get_ship_slot();

    if (obj != -1) {
        initialize_object(obj, type, owner);
        aeShipSide[obj] = SIDE_NEUTRAL;
    }
    return obj;
}

/* Function start: 0x41DFE0 */
short any_selected(unsigned char *loadout, enum ObjectClass objectClass)
{
    enum ObjectClass selectedClass;
    short selected;
    short weapon;

    selectedClass = objectClass;
    selected = 0;
    weapon = 0;
    if ((signed char)loadout[0] > (signed char)selected)
        for (; (short)(signed char)loadout[0] > weapon; weapon++) {
            if (selected != 0)
                break;
            if (aObjectTypeData[
                    ((ShipWeaponSlot *)(loadout + weapon * 7 + 1))->type]
                    .objectClass ==
                    selectedClass &&
                ((ShipWeaponSlot *)(loadout + weapon * 7 + 1))->disabled == 0)
                selected = 1;
        }
    return selected;
}

/* Function start: 0x41E040 */
unsigned int remove_weapon(short obj, short weapon)
{
    short ship;
    short currentWeapon;
    int weaponOffset;
    enum ObjectType preferredType;
    enum ObjectClass objectClass;
    unsigned char *loadout;

    ship = obj;
    currentWeapon = weapon;
    weaponOffset = (int)currentWeapon * sizeof(ShipWeaponSlot);
    loadout = aShipWeapons[ship];
    preferredType =
        ((ShipWeaponSlot *)(loadout + weaponOffset + 1))->type;
    objectClass = aObjectTypeData[preferredType].objectClass;
    for (; currentWeapon < (signed char)loadout[0] - 1;
         currentWeapon++) {
        unsigned char *entry = loadout + currentWeapon * 7;

#ifdef WC1_SDL
        /* The seven-byte records are intentionally unaligned. */
        memcpy(entry + 1, entry + 8, sizeof(ShipWeaponSlot));
#else
        *(int *)(entry + 1) = *(int *)(entry + 8);
        *(short *)(entry + 5) = *(short *)(entry + 12);
        entry[7] = entry[14];
#endif
    }
    loadout[(signed char)loadout[0] * 7 + 7] = 1;
    loadout[0]--;
    if (ship == 0) {
        if (any_selected(loadout, objectClass) == 0) {
            if (objectClass == OBJECT_CLASS_PROJECTILE) {
                select_new_gun();
            } else {
                nSelectedReleaseWeaponIndex = -1;
                select_new_release_weapon(preferredType);
            }
        }
        if (get_mode(0) == 1)
            InvalidateVduMode(0);
    }
    return 0;
}

/* Function start: 0x41E120 */
void set_objects_data(short obj, enum ObjectType type, short owner)
{
    ObjectTypeData *typeData;
    unsigned char *loadout;
    enum ObjectClass objectClass;
    short value;
    short zero;
    short weapon;

    if (type == OBJECT_TYPE_SPACE_DUST) {
        aeObjectType[obj] = type;
        aeObjectClass[obj] = OBJECT_CLASS_DUST;
        return;
    }
    if (aObjectTypeData[type].shapeSet == 0) {
        switch (type) {
        case OBJECT_TYPE_ASTEROID2:
            type = OBJECT_TYPE_ASTEROID1;
            break;
        case OBJECT_TYPE_ASTEROID4:
            type = OBJECT_TYPE_ASTEROID3;
            break;
        case OBJECT_TYPE_ASTEROID6:
            type = OBJECT_TYPE_ASTEROID5;
            break;
        case OBJECT_TYPE_DEBRIS_METAL_SHEET:
            type = OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK;
            break;
        case OBJECT_TYPE_DEBRIS_WING:
            type = OBJECT_TYPE_DEBRIS_PIPE;
            break;
        case OBJECT_TYPE_EXPLOSION1:
        case OBJECT_TYPE_EXPLOSION2:
            type = OBJECT_TYPE_EXPLOSION0;
            break;
        }
    }
    typeData = &aObjectTypeData[type];
    aeObjectType[obj] = type;
    aeObjectClass[obj] = typeData->objectClass;
    if (type == OBJECT_TYPE_ROCK_CHUNK)
        apObjectShape[obj] =
            aObjectTypeData[OBJECT_TYPE_ASTEROID1].shapeSet;
    else
        apObjectShape[obj] = typeData->shapeSet;
    init_ijk(obj);
    asObjectCollisionRadius[obj] = typeData->collisionRadius;
    zero = 0;
    asObjectRadarRadius[obj] = typeData->radarRadius;
    asObjectScale[obj] = typeData->scale;
    asObjectAfterburnerVelocity[obj] =
        typeData->afterburnerVelocity;
    acObjectOwner[obj] = (signed char)owner;
    asShipAccumulatedDamage[obj] = zero;
    objectClass = aeObjectClass[obj];
    asObjectFlip[obj] = zero;
    acLastCollisionObject[obj] = -1;
    asObjectScreenAngle[obj] = zero;

    if (objectClass >= OBJECT_CLASS_MISSILE) {
        asObjectViewFrame[obj] = zero;
        acShipTarget[obj] = -1;
        if (objectClass >= OBJECT_CLASS_SHIP) {
            value = typeData->shieldFore;
            aasShipShield[obj][0] = value;
            aasShipMaximumShield[obj][0] = value;
            value = typeData->shieldAft;
            aasShipShield[obj][1] = value;
            aasShipMaximumShield[obj][1] = value;
            aasShipArmor[obj][0] = typeData->armorFront;
            aasShipArmor[obj][2] = typeData->armorLeft;
            aasShipArmor[obj][3] = typeData->armorRight;
            aasShipArmor[obj][1] = typeData->armorRear;
            anShipFuel[obj] = *(int *)&typeData->lifetime;
            acShipIonDriveDamage[obj] = (signed char)zero;
            acShipDamage[obj] = (signed char)zero;
            recalc_max_velocity(obj);
            acPilotHitPoints[obj] = 4;
            loadout = aShipWeapons[obj];
            memcpy(loadout, typeData->weaponLoadout,
                   sizeof(typeData->weaponLoadout));

            if (obj == 0) {
                nSelectedReleaseWeaponIndex = -1;
                eSelectedGunType = (enum ObjectType)-1;
                for (weapon = (short)(signed char)loadout[0];
                     weapon-- > 0;) {
                    ShipWeaponSlot *slot;

                    slot = (ShipWeaponSlot *)(loadout + weapon * 7 + 1);
                    if (slot->disabled == 0) {
                        if (aObjectTypeData[
                                slot->type].objectClass ==
                                OBJECT_CLASS_PROJECTILE)
                            eSelectedGunType = slot->type;
                        else
                            nSelectedReleaseWeaponIndex = weapon;
                    }
                }
            }
            acLastAttacker[obj] = -1;
            asShipWeaponEnergy[obj] = 100;
        }
        return;
    }

    if (typeData->animation == 0) {
        asObjectViewFrame[obj] = typeData->yawRate;
        return;
    }
    asObjectAnimationDelay[obj] = 1;
    asObjectAnimationIndex[obj] = 0;
    animate_shape(obj);
}

/* Function start: 0x41E400 */
unsigned int match_rotation_goal(short *rotation, short *goal,
                                 short totalError, short rate)
{
    short step;

    if (totalError != 0) {
        if (*goal > 180)
            *goal = *goal - 360;
        if (*goal < -180)
            *goal = *goal + 360;
        step = MaxShort(1, (short)(abs(*rotation - *goal) *
                                  rate / totalError));
        if (*goal != *rotation || step < abs(*rotation)) {
            if (*goal < 1) {
                *rotation = (short)(*rotation +
                    MinShort(
                        MaxShort(
                            (short)(MaxShort(*goal, (short)-step) -
                                    *rotation),
                            (short)-step),
                        step));
            } else {
                *rotation = (short)(*rotation +
                    MaxShort(
                        MinShort(
                            (short)(MinShort(*goal, step) - *rotation),
                            step),
                        (short)-step));
            }
        }
    }
    if (*goal != 0) {
        if (*goal > 0)
            *goal = MaxShort((short)(*goal - *rotation), 0);
        else
            *goal = MinShort((short)(*goal - *rotation), 0);
    }
    return 0;
}

/* Function start: 0x41E520 */
void rotate_object_to_goal(short obj)
{
    ObjectTypeData *typeData;
    short totalError;

    typeData = &aObjectTypeData[aeObjectType[obj]];
    if (aeSpecialManeuver[obj] ==
            SPECIAL_MANEUVER_BLOWING_UP) {
        if ((short)alert_flag(obj, 1) != 0) {
            set_special(obj, SPECIAL_MANEUVER_NONE);
        } else {
            if (asObjectCounter[obj] == -1 &&
                skill_check(obj, 7) != 0)
                aeSpecialManeuver[obj] =
                    SPECIAL_MANEUVER_NONE;
            return;
        }
    }
    totalError = (short)(abs(anObjectYawRotation[obj] -
                            anYawGoal[obj]) +
                         abs(anObjectPitchRotation[obj] -
                             anPitchGoal[obj]) +
                         abs(anObjectRollRotation[obj] -
                             anRollGoal[obj]));
    match_rotation_goal(&anObjectPitchRotation[obj],
                        &anPitchGoal[obj], totalError,
                        typeData->yawRate);
    match_rotation_goal(&anObjectYawRotation[obj],
                        &anYawGoal[obj], totalError,
                        typeData->pitchRate);
    match_rotation_goal(&anObjectRollRotation[obj],
                        &anRollGoal[obj], totalError,
                        typeData->rollRate);
}

/* Function start: 0x41E710 */
unsigned int celerate(short ship, int delta)
{
    int maximumSpeed = (int)asShipMaximumSpeed[ship] << 8;
    int speed;

    speed = anShipSpeed[ship] + delta;
    anShipSpeed[ship] = speed;
    if (speed > maximumSpeed)
        anShipSpeed[ship] = maximumSpeed;
    if (anShipSpeed[ship] < 0)
        anShipSpeed[ship] = 0;
    return 0;
}

/* Function start: 0x41E750 */
unsigned int approach_speed(short ship, int targetSpeed)
{
    int delta;
    int acceleration;

    acceleration = GetShipAccelerationRate(ship);
    delta = targetSpeed - anShipSpeed[ship];

    if ((short)alert_flag(ship, 1))
        acceleration += acceleration;
    if ((delta < 0 ? -delta : delta) > acceleration)
        delta = MultiplyFixed(SignFixed(delta), acceleration);
    celerate(ship, delta);
    return 0;
}

/* Function start: 0x41E7C0 */
unsigned int steady_object(short ship)
{
    anYawGoal[ship] = 0;
    anPitchGoal[ship] = 0;
    anRollGoal[ship] = 0;
    return 0;
}

/* Function start: 0x41E7F0 */
short real_velocity(short obj)
{
    return FixedToShortSaturating(
        Vector_magnitude(&aShipVelocity[obj]));
}

/* Function start: 0x41E820 */
unsigned int fix_velocity(short obj)
{
    ScaleFixedVector(&aShipForwardVector[obj],
                     anShipSpeed[obj],
                     &aShipVelocity[obj]);
    return 0;
}

/* Function start: 0x41E860 */
unsigned int sort_viable_target_list(void)
{
    short nextOuter;
    signed char target;
    short distance;
    short outer;
    short count;
    short inner;

    if (cViableTargetCount > 1) {
        count = (short)cViableTargetCount;
        outer = 0;
        if (count - 1 > 0) {
            do {
                nextOuter = outer + 1;
                inner = nextOuter;
                if (inner < count) {
                    for (; inner < count; inner++) {
                        distance =
                            asViableTargetDistance[outer];
                        if (asViableTargetDistance[inner] <
                            distance) {
                            asViableTargetDistance[outer] =
                                asViableTargetDistance[inner];
                            target = acViableTarget[outer];
                            asViableTargetDistance[inner] =
                                distance;
                            acViableTarget[outer] =
                                acViableTarget[inner];
                            acViableTarget[inner] = target;
                        }
                    }
                }
                outer = nextOuter;
            } while ((int)outer < count - 1);
        }
    }
    return 0;
}
