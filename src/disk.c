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

    error = DAT_00465460;
    packetSize = (unsigned int)-1;
    if ((packet == 0 || (error != 0 && error != 8)) &&
        (packet != 0 || error != 8)) {
        if (section != -1)
            packetSize = GetPacketSize(
                (char *)(DAT_005a7cf0 + logicalFile * 16), section);
        LogMemoryUsage();
        operation = "allocating memory";
        if (packet != 0 && section != -1)
            operation = "reading from disk";
        DAT_00465460 = error;
        sprintf(g_szDefaultTextBuffer_005a7590,
                "Sorry, an error has occured while %s.\n"
                "Please note the following information:\n"
                "%s #%d (ERR %d  PS%ld  LB%ld  FL%d) at %s\n"
                "Check your configuration.  If this problem persists, please\n"
                "call Origin Systems' service line.  We are sorry for the inconvenience.",
                operation,
                (char *)(DAT_005a7cf0 + logicalFile * 16),
                (int)section, (int)error, packetSize,
                GetFixedOneMillionThunkAlt(), (int)retry, sourceTag);
        FatalErrorAndExit(g_szDefaultTextBuffer_005a7590);
    }
}

/* Function start: 0x41D200 */
void *LoadPacketIntoBuffer(short logicalFile, short section,
                           void *destination)
{
    void *packet;

    PromptInsertNumberedDisk(logicalFile);
    packet = PacketLoad(
        (char *)(DAT_005a7cf0 + logicalFile * 16),
        section, destination, 0);
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
        (char *)(DAT_005a7cf0 + logicalFile * 16), section);
    packet = AllocateTaggedMemory((unsigned int)(short)packetSize, 0x40);
    if (packet != 0) {
        do {
            retries--;
            PacketLoad((char *)(DAT_005a7cf0 + logicalFile * 16),
                       section, packet, 0);
            if (retries < 1 || DAT_00465460 == 0)
                break;
        } while (DAT_00465460 != 8);
    }
    ReportPacketLoadError(packet, logicalFile, 0, section, "LPN");
    return packet;
}

/* Function start: 0x41D2E0 */
void *FetchDiskPacketRetrying(short logicalFile, short section,
                              unsigned short flags)
{
    const char *filename;
    void *packet = 0;
    short retries = 5;

    if (DAT_005a7cf0 == 0 || logicalFile < 0) {
        DAT_00465460 = 3;
        return 0;
    }
    filename = (const char *)(DAT_005a7cf0 + logicalFile * 16);
    do {
        FreePacketAndClear((int *)&packet, flags);
        packet = PacketLoad(filename, section, 0, flags);
        retries--;
    } while (packet == 0 && retries > 0 && DAT_00465460 != 8);

    if (packet == 0 && DAT_00465460 == 4 && DAT_005a7510.pixels != 0) {
        free_viewport(&DAT_005a7510);
        do {
            FreePacketAndClear((int *)&packet, flags);
            packet = PacketLoad(filename, section, 0, flags);
            retries--;
        } while (packet == 0 && retries > 0 && DAT_00465460 != 8);
        AllocateViewport(&DAT_005a7510, (short)DAT_004699d8, 0x20);
    }
    if (packet == 0 && DAT_00465460 == 4 && DAT_005a76b0.pixels != 0) {
        free_viewport(&DAT_005a76b0);
        do {
            FreePacketAndClear((int *)&packet, flags);
            packet = PacketLoad(filename, section, 0, flags);
            retries--;
        } while (packet == 0 && retries > 0 && DAT_00465460 != 8);
        AllocateViewport(&DAT_005a76b0, (short)DAT_0046999c, 0);
    }
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
    if (g_apTextFonts_005a6c00[index] == 0) {
        if (index == 1) {
            g_apTextFonts_005a6c00[index] =
                (unsigned char *)FetchDiskPacketRetrying(0, (short)index,
                                                         0x10);
        } else {
            g_apTextFonts_005a6c00[index] =
                (unsigned char *)FetchDiskPacketRetrying(0, (short)index,
                                                         0);
        }
        g_apFontWorkspaces_005a6c10[index] =
            AllocateFontWorkspace((short)index);
    }
    context->font = g_apTextFonts_005a6c00[index];
    context->colour = colour;
    context->backgroundColour = (unsigned char)background;
    context->fontWorkspace = g_apFontWorkspaces_005a6c10[index];
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
    if (g_apTextFonts_005a6c00[index] != 0) {
        ReleasePacketHandle((int)g_apTextFonts_005a6c00[index]);
        g_apTextFonts_005a6c00[index] = 0;
        FreeFontWorkspace(g_apFontWorkspaces_005a6c10[index]);
        g_apFontWorkspaces_005a6c10[index] = 0;
    }
    return 0;
}

/* Function start: 0x41D5F0 */
unsigned int DrawTextAt(TextContext *context, short x, short y,
                        char *text, unsigned char alignment)
{
    char *savedText = context->text;
    unsigned char savedAlignment = context->alignment;

    SetTextContext(context);
    SetTextCursor((unsigned short)x, (unsigned short)y);
    context->text = text;
    context->alignment = alignment;
    DrawTextString(text);
    context->text = savedText;
    context->alignment = savedAlignment;
    if (context->viewport->pixels == DAT_005a6ba0.pixels)
        DIBslam();
    return 0;
}

/* Function start: 0x41D6C0 */
short OpenDiskDataFile(short logicalFile)
{
    short file;

    FillGraphicSuffix(g_szDiskMarkerFile_00469688,
                      (unsigned char)DAT_005a7cf0[logicalFile * 16 + 13],
                      3);
    file = OpenDataFileOrDie(g_szDiskMarkerFile_00469688);
    if (file != -1) {
        CloseDataFile((unsigned short)file);
        return 1;
    }
    if (DAT_0059ab34 != 0)
        return 1;
    if (GetCurrentDiskDriveHook() == 'A') {
        if (_toupper((int)g_cDiskPromptDriveLetter_005a7d21) != 'B')
            return 0;
        SelectDiskDriveHook('B');
    } else {
        SelectDiskDriveHook('A');
    }
    file = OpenDataFileOrDie(g_szDiskMarkerFile_00469688);
    if (file == -1)
        return 0;
    CloseDataFile((unsigned short)file);
    return 1;
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
    if (g_bGraphicsActive_00469a20 == 0) {
        diskNumber = (signed char)DAT_005a7cf0[logicalFile * 16 + 13];
        do {
            DiskPromptDrawHook();
            ResetDiskPromptTimer();
            _cprintf("Please Insert Disk %d. Press any key to continue",
                     (int)diskNumber);
            WaitForInputKey();
        } while (OpenDiskDataFile(logicalFile) == 0);
        return;
    }

    savedTextContext = g_pCurrentTextContext_0059af8c;
    SetTextContext(&g_stDiskPromptTextContext_005a7d60);
    g_stDiskPromptBackgroundViewport_005a7d00.left =
        (short)g_dwDiskPromptTopLeft_005a7d80;
    g_stDiskPromptBackgroundViewport_005a7d00.top =
        (short)(g_dwDiskPromptTopLeft_005a7d80 >> 16);
    g_stDiskPromptViewport_005a7d40.left =
        (short)g_dwDiskPromptTopLeft_005a7d80;
    g_stDiskPromptViewport_005a7d40.top =
        (short)(g_dwDiskPromptTopLeft_005a7d80 >> 16);
    g_stDiskPromptBackgroundViewport_005a7d00.right =
        (short)g_dwDiskPromptBottomRight_005a7d84;
    g_stDiskPromptBackgroundViewport_005a7d00.bottom =
        (short)(g_dwDiskPromptBottomRight_005a7d84 >> 16);
    g_stDiskPromptViewport_005a7d40.right =
        (short)g_dwDiskPromptBottomRight_005a7d84;
    g_stDiskPromptViewport_005a7d40.bottom =
        (short)(g_dwDiskPromptBottomRight_005a7d84 >> 16);

    if (DAT_005a7510.pixels != 0) {
        g_stDiskPromptBackgroundViewport_005a7d00 = DAT_005a7510;
        backgroundColour = (unsigned char)DAT_004699d8;
    } else if (DAT_005a76b0.pixels != 0) {
        g_stDiskPromptBackgroundViewport_005a7d00 = DAT_005a76b0;
        backgroundColour = (unsigned char)DAT_0046999c;
    } else {
        savedViewportMode = (signed char)AllocateViewport(
            &g_stDiskPromptBackgroundViewport_005a7d00, -1, 0);
        backgroundColour = (short)(unsigned int)savedTextContext;
    }

    g_stDiskPromptBackgroundViewport_005a7d00.left =
        (short)g_dwDiskPromptTopLeft_005a7d80;
    g_stDiskPromptBackgroundViewport_005a7d00.top =
        (short)(g_dwDiskPromptTopLeft_005a7d80 >> 16);
    g_stDiskPromptBackgroundViewport_005a7d00.right =
        (short)g_dwDiskPromptBottomRight_005a7d84;
    g_stDiskPromptBackgroundViewport_005a7d00.bottom =
        (short)(g_dwDiskPromptBottomRight_005a7d84 >> 16);
    if (savedViewportMode != 0) {
        CopyViewportContents(&g_stDiskPromptViewport_005a7d40,
                             &g_stDiskPromptBackgroundViewport_005a7d00);
    }

    do {
        ClearViewport(&g_stDiskPromptViewport_005a7d40,
                      g_cViewportClearColour_004699a0);
        SetTextCursor(
            (unsigned short)(g_stDiskPromptViewport_005a7d40.left + 2),
            (unsigned short)(g_stDiskPromptViewport_005a7d40.top + 2));
        DrawViewportBorder(
            &g_stDiskPromptViewport_005a7d40,
            g_stDiskPromptViewport_005a7d40.left,
            g_stDiskPromptViewport_005a7d40.top,
            g_stDiskPromptViewport_005a7d40.right,
            g_stDiskPromptViewport_005a7d40.bottom,
            g_nDiskPromptBorderColour_00469694);
        FormatTextBufferFromStart(
            "Please insert disk %d\ninto any drive\nPress any key when ready.",
            (int)(signed char)DAT_005a7cf0[logicalFile * 16 + 13]);
        DrawTextString(g_szTextScratchBuffer_00598b00);
        WaitForInputKey();
        if (OpenDiskDataFile(logicalFile) != 0)
            diskReady++;
        if (savedViewportMode == 0) {
            ClearViewport(&g_stDiskPromptViewport_005a7d40,
                          backgroundColour);
        } else {
            CopyViewportContents(
                &g_stDiskPromptBackgroundViewport_005a7d00,
                &g_stDiskPromptViewport_005a7d40);
        }
    } while (diskReady == 0);

    if (savedViewportMode == 1) {
        free_viewport(&g_stDiskPromptBackgroundViewport_005a7d00);
    } else if (savedViewportMode == 2) {
        ClearViewport(&g_stDiskPromptBackgroundViewport_005a7d00,
                      backgroundColour);
    }
    g_pCurrentTextContext_0059af8c = savedTextContext;
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
    if (g_nEventManagerActive_0059a850 == 0)
        return (signed char)PumpMessagesDuringWait();

    savedMode = g_bInputMode_0059a848;
    g_bInputMode_0059a848 = 1;
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
            switch (key) {
            case 0x1d:
                key = 0;
                break;
            default:
                while (PollInputEvent(&event, 0xff) != 0)
                    ;
                break;
            }
            break;
        }
    } while (key == 0);
    ClearInputKeyStatePreservingModifiers();
    g_bInputMode_0059a848 = savedMode;
    FlushInputEvents();
    return key;
}

/* Function start: 0x41DBA0 */
void WaitForSceneAdvance(short duration)
{
    InputEventState event;
    unsigned char savedMode;
    short eventType;
    short escaped;
    int advanced;

    savedMode = g_bInputMode_0059a848;
    advanced = 0;
    g_bInputMode_0059a848 = 1;
    if (duration == -1) {
        escaped = CheckEscaped();
        if (escaped == 0)
            goto wait_for_input;
        do {
            escaped = CheckEscaped();
        } while (escaped != 0);
        duration = 0;
    }
    SetFrameTimerPeriodDirect(duration);
wait_for_input:
    while (IsFrameTickElapsed() == 0 && advanced == 0) {
        eventType = PollInputEvent(&event, 0xff);
        switch (eventType) {
        case 2:
        case 3:
        case 5:
        case 10:
            advanced = 1;
            g_bInputMode_0059a848 = savedMode;
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
    short delta;
    short scanCode;
    int moved;

    delta = (short)(g_nKeyboardPointerStep_004696a4 * 2);
    scanCode = (short)event->value;
    moved = 0;
    if (scanCode == 0x4c) {
        if (g_nKeyboardPointerStep_004696a4 == 1)
            g_nKeyboardPointerStep_004696a4 = 4;
        else
            g_nKeyboardPointerStep_004696a4 = 1;
    } else {
        switch (scanCode) {
        case 0x47:
            g_nMouseY_0059ab12 -= delta;
            g_nMouseX_0059ab10 -= delta;
            moved = 1;
            break;
        case 0x48:
            g_nMouseY_0059ab12 -= delta;
            moved = 1;
            break;
        case 0x49:
            g_nMouseX_0059ab10 += delta;
            g_nMouseY_0059ab12 -= delta;
            moved = 1;
            break;
        case 0x4b:
            g_nMouseX_0059ab10 -= delta;
            moved = 1;
            break;
        case 0x4d:
            g_nMouseX_0059ab10 += delta;
            moved = 1;
            break;
        case 0x4f:
            g_nMouseX_0059ab10 -= delta;
            g_nMouseY_0059ab12 += delta;
            moved = 1;
            break;
        case 0x50:
            g_nMouseY_0059ab12 += delta;
            moved = 1;
            break;
        case 0x51:
            g_nMouseX_0059ab10 += delta;
            g_nMouseY_0059ab12 += delta;
            moved = 1;
            break;
        }
    }

    if (g_nMouseX_0059ab10 < 0)
        g_nMouseX_0059ab10 = 0;
    else if (g_nMouseX_0059ab10 > 320)
        g_nMouseX_0059ab10 = 320;
    if (g_nMouseY_0059ab12 < 0)
        g_nMouseY_0059ab12 = 0;
    else if (g_nMouseY_0059ab12 > 320)
        g_nMouseY_0059ab12 = 320;

    g_nHostMouseX_0059af70 = g_nMouseX_0059ab10;
    g_nHostMouseY_0059af72 = g_nMouseY_0059ab12;
    if (moved != 0) {
        RetainInputEventsOfType(3);
        QueueInputEvent(13, (unsigned short)g_nMouseX_0059ab10,
                        (unsigned short)g_nMouseY_0059ab12,
                        0, 0, 0, 0);
        g_bPointerMovedByKeyboard_005a7d54 = 1;
        SetMousePosition(g_nHostMouseX_0059af70,
                         g_nHostMouseY_0059af72);
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

    text = g_pCurrentTextContext_0059af8c->text;
    textWidth = MeasureTextPixelWidthClamped(text);
    length = DosStrlen(text);
    if (length != 0) {
        characterWidth = (short)GetFontCharWidth(text[length - 1]);
        clearArea = *g_pCurrentTextContext_0059af8c->viewport;
        clearArea.left = (short)(clearArea.left +
                                 textWidth - characterWidth);
        clearArea.right = (short)(clearArea.left + characterWidth - 1);
        clearArea.top = g_pCurrentTextContext_0059af8c->cursorY;
        clearArea.bottom = (short)(clearArea.top +
            ReadWord((unsigned short *)
                g_pCurrentTextContext_0059af8c->font) - 1);
        LeaveAllocationScope();
        ClearViewport(&clearArea,
                      g_pCurrentTextContext_0059af8c->backgroundColour);
        EnterAllocationScope();
        g_pCurrentTextContext_0059af8c->cursorX = (short)(
            g_pCurrentTextContext_0059af8c->cursorX - characterWidth);
    }
}

/* Function start: 0x41DEB0 */
short WaitForStreamInputKey(void)
{
    unsigned int saved = DAT_0046505c;
    short key;

    DAT_0046505c = 1;
    do {
        key = WaitForInputKey();
    } while (key == 0);
    DAT_0046505c = saved;
    return key;
}

/* Function start: 0x41DEE0 */
short initialize_object(short obj, enum ObjectType type, short owner)
{
    if (obj != -1) {
        set_objects_data(obj, type, owner);
        zero_vector(&g_aShipPosition_0059c490[obj]);
        zero_vector(&g_aShipVelocity_0059c010[obj]);
    }
    return obj;
}

/* Function start: 0x41DF40 */
short borrow_dust(void)
{
    short i = 0x22;

    do {
        if (g_aeObjectClass_0059d100[i] == OBJECT_CLASS_DUST)
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
        g_aeShipSide_0059d650[obj] = SIDE_NEUTRAL;
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
    if ((signed char)loadout[0] > 0)
        do {
            ShipWeaponSlot *slot;

            if (selected != 0)
                break;
            slot = (ShipWeaponSlot *)(loadout + weapon * 7 + 1);
            if (g_aObjectTypeData_00466458[slot->type].objectClass ==
                    selectedClass &&
                slot->disabled == 0)
                selected = 1;
            weapon++;
        } while ((short)(signed char)loadout[0] > weapon);
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
    loadout = g_aShipWeapons_0059cab0[ship];
    preferredType =
        *(enum ObjectType *)(loadout + weaponOffset + 1);
    objectClass = g_aObjectTypeData_00466458[preferredType].objectClass;
    while (currentWeapon < (signed char)loadout[0] - 1) {
        unsigned char *entry = loadout + currentWeapon * 7;

        *(int *)(entry + 1) = *(int *)(entry + 8);
        *(short *)(entry + 5) = *(short *)(entry + 12);
        entry[7] = entry[14];
        currentWeapon++;
    }
    loadout[(signed char)loadout[0] * 7 + 7] = 1;
    loadout[0]--;
    if (ship == 0) {
        if (any_selected(loadout, objectClass) == 0) {
            if (objectClass == OBJECT_CLASS_PROJECTILE) {
                select_new_gun();
            } else {
                g_nSelectedReleaseWeaponIndex_0046c058 = -1;
                select_new_release_weapon(preferredType);
            }
        }
        if ((short)get_mode(0) == 1)
            ClearMessageSlot(0);
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
        g_aeObjectType_0059b560[obj] = type;
        g_aeObjectClass_0059d100[obj] = OBJECT_CLASS_DUST;
        return;
    }
    if (g_aObjectTypeData_00466458[type].shapeSet == 0) {
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
    typeData = &g_aObjectTypeData_00466458[type];
    g_aeObjectType_0059b560[obj] = type;
    g_aeObjectClass_0059d100[obj] = typeData->objectClass;
    if (type == OBJECT_TYPE_ROCK_CHUNK)
        g_apObjectShape_0059d2f0[obj] =
            g_aObjectTypeData_00466458[OBJECT_TYPE_ASTEROID1].shapeSet;
    else
        g_apObjectShape_0059d2f0[obj] = typeData->shapeSet;
    init_ijk(obj);
    g_asObjectCollisionRadius_0059d710[obj] = typeData->collisionRadius;
    zero = 0;
    g_asObjectRadarRadius_0059c790[obj] = typeData->radarRadius;
    g_asObjectScale_0059de40[obj] = typeData->scale;
    g_asObjectAfterburnerVelocity_0059c9d0[obj] =
        typeData->afterburnerVelocity;
    g_acObjectOwner_0059ce20[obj] = (signed char)owner;
    g_asShipAccumulatedDamage_0059dee0[obj] = zero;
    objectClass = g_aeObjectClass_0059d100[obj];
    g_asObjectFlip_0059c870[obj] = zero;
    DAT_0059d6a0[obj] = -1;
    g_asObjectScreenAngle_0059cd90[obj] = zero;

    if (objectClass >= OBJECT_CLASS_MISSILE) {
        g_asObjectViewFrame_0059d230[obj] = zero;
        g_acShipTarget_0059ce60[obj] = -1;
        if (objectClass >= OBJECT_CLASS_SHIP) {
            value = typeData->shieldFore;
            g_aasShipShield_0059d5b0[obj][0] = value;
            g_aasShipMaximumShield_0059d6e0[obj][0] = value;
            value = typeData->shieldAft;
            g_aasShipShield_0059d5b0[obj][1] = value;
            g_aasShipMaximumShield_0059d6e0[obj][1] = value;
            g_aasShipArmor_0059d420[obj][0] = typeData->armorFront;
            g_aasShipArmor_0059d420[obj][2] = typeData->armorLeft;
            g_aasShipArmor_0059d420[obj][3] = typeData->armorRight;
            g_aasShipArmor_0059d420[obj][1] = typeData->armorRear;
            g_anShipFuel_0059b470[obj] = *(int *)&typeData->lifetime;
            g_acShipIonDriveDamage_0059d4a0[obj] = (signed char)zero;
            g_acShipDamage_0059c460[obj] = (signed char)zero;
            recalc_max_velocity(obj);
            DAT_0059cf00[obj] = 4;
            loadout = g_aShipWeapons_0059cab0[obj];
            memcpy(loadout, typeData->weaponLoadout,
                   sizeof(typeData->weaponLoadout));

            if (obj == 0) {
                g_nSelectedReleaseWeaponIndex_0046c058 = -1;
                g_eSelectedGunType_0046c054 = (enum ObjectType)-1;
                weapon = (short)(signed char)loadout[0];
                while (weapon-- > 0) {
                    ShipWeaponSlot *slot;

                    slot = (ShipWeaponSlot *)(loadout + weapon * 7 + 1);
                    if (slot->disabled == 0) {
                        if (g_aObjectTypeData_00466458[
                                slot->type].objectClass ==
                                OBJECT_CLASS_PROJECTILE)
                            g_eSelectedGunType_0046c054 = slot->type;
                        else
                            g_nSelectedReleaseWeaponIndex_0046c058 = weapon;
                    }
                }
            }
            DAT_0059c910[obj] = -1;
            g_asShipWeaponEnergy_0059d470[obj] = 100;
        }
        return;
    }

    if (typeData->animation == 0) {
        g_asObjectViewFrame_0059d230[obj] = typeData->yawRate;
        return;
    }
    g_asObjectAnimationDelay_0059b660[obj] = 1;
    g_asObjectAnimationIndex_0059da30[obj] = 0;
    animate_shape(obj);
}

/* Function start: 0x41E400 */
void match_rotation_goal(short *rotation, short *goal,
                         short totalError, short rate)
{
    short step;
    short value;

    if (totalError != 0) {
        if (*goal > 180)
            *goal = *goal - 360;
        if (*goal < -180)
            *goal = *goal + 360;
        step = MaxShort(1, (short)(AbsInt(*rotation - *goal) *
                                  rate / totalError));
        if (*goal != *rotation || step < AbsInt(*rotation)) {
            if (*goal < 1) {
                value = MaxShort(*goal, (short)-step);
                value = MaxShort((short)(value - *rotation),
                                 (short)-step);
                value = MinShort(value, step);
                *rotation = value + *rotation;
            } else {
                value = MinShort(*goal, step);
                value = MinShort((short)(value - *rotation), step);
                value = MaxShort(value, (short)-step);
                *rotation = value + *rotation;
            }
        }
    }
    if (*goal != 0) {
        if (*goal > 0)
            *goal = MaxShort((short)(*goal - *rotation), 0);
        else
            *goal = MinShort((short)(*goal - *rotation), 0);
    }
}

/* Function start: 0x41E520 */
void rotate_object_to_goal(short obj)
{
    ObjectTypeData *typeData;
    short totalError;

    typeData = &g_aObjectTypeData_00466458[g_aeObjectType_0059b560[obj]];
    totalError = (short)(AbsInt(g_anObjectPitchRotation_0059b2a0[obj] -
                               g_anPitchGoal_0059d7a0[obj]) +
                         AbsInt(g_anObjectYawRotation_0059ce80[obj] -
                               g_anYawGoal_0059c310[obj]) +
                         AbsInt(g_anObjectRollRotation_0059d7e0[obj] -
                               g_anRollGoal_0059d630[obj]));
    match_rotation_goal(&g_anObjectPitchRotation_0059b2a0[obj],
                        &g_anPitchGoal_0059d7a0[obj], totalError,
                        typeData->pitchRate);
    match_rotation_goal(&g_anObjectYawRotation_0059ce80[obj],
                        &g_anYawGoal_0059c310[obj], totalError,
                        typeData->yawRate);
    match_rotation_goal(&g_anObjectRollRotation_0059d7e0[obj],
                        &g_anRollGoal_0059d630[obj], totalError,
                        typeData->rollRate);
}

/* Function start: 0x41E710 */
unsigned int celerate(short ship, int delta)
{
    int maximumSpeed = (int)g_asShipMaximumSpeed_0059c440[ship] << 8;

    g_anShipSpeed_0059b320[ship] += delta;
    if (maximumSpeed < g_anShipSpeed_0059b320[ship])
        g_anShipSpeed_0059b320[ship] = maximumSpeed;
    if (g_anShipSpeed_0059b320[ship] < 0)
        g_anShipSpeed_0059b320[ship] = 0;
    return 0;
}

/* Function start: 0x41E750 */
unsigned int approach_speed(short ship, int targetSpeed)
{
    int delta;
    int acceleration;

    acceleration = GetShipAccelerationRate(ship);
    delta = targetSpeed - g_anShipSpeed_0059b320[ship];

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
    g_anYawGoal_0059c310[ship] = 0;
    g_anPitchGoal_0059d7a0[ship] = 0;
    g_anRollGoal_0059d630[ship] = 0;
    return 0;
}

/* Function start: 0x41E7F0 */
short real_velocity(short obj)
{
    return FixedToShortSaturating(
        (int)Vector_magnitude(&g_aShipVelocity_0059c010[obj]));
}

/* Function start: 0x41E820 */
unsigned int fix_velocity(short obj)
{
    ScaleFixedVector(&g_aShipForwardVector_0059bce0[obj],
                     g_anShipSpeed_0059b320[obj],
                     &g_aShipVelocity_0059c010[obj]);
    return 0;
}

/* Function start: 0x41E860 */
unsigned int sort_viable_target_list(void)
{
    short outer;
    short inner;
    short distance;
    signed char target;
    short count = (short)g_cViableTargetCount_0046c088;

    for (outer = 0; outer < count - 1; outer++) {
        for (inner = outer + 1; inner < count; inner++) {
            if (g_asViableTargetDistance_0059c470[inner] <
                g_asViableTargetDistance_0059c470[outer]) {
                distance = g_asViableTargetDistance_0059c470[outer];
                g_asViableTargetDistance_0059c470[outer] =
                    g_asViableTargetDistance_0059c470[inner];
                g_asViableTargetDistance_0059c470[inner] = distance;
                target = g_acViableTarget_0059c920[outer];
                g_acViableTarget_0059c920[outer] =
                    g_acViableTarget_0059c920[inner];
                g_acViableTarget_0059c920[inner] = target;
            }
        }
    }
    return 0;
}
