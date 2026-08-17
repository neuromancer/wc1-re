/*
 *  Disk data files and packet fetching with retry.
 *
 *  Address range 0x41d250-0x41efff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: OpenDiskDataFile/FetchDiskPacketRetrying/PromptInsertNumberedDisk.
 */
#include "wc1.h"

/* Function start: 0x409120 */
short WaitForSceneAdvance(void *scenePacket)
{
    unsigned char *cursor;
    unsigned char *nextForm;
    short hotspotCount;
    unsigned int formType;

    if (g_pActiveScenePacket_00492654 != scenePacket) {
        ReleasePacketSlot(&g_stSceneHotspotTable_005d3bf8.data);
        nextForm = (unsigned char *)scenePacket +
            ((ScenePacketHeader *)scenePacket)->formOffset;
        do {
            cursor = nextForm;
            formType = ReadNextSceneForm(&cursor, &nextForm);
            if (formType == 0x52544f48) {
                DecodeSceneResourceChunk(
                    &cursor, &g_stSceneHotspotTable_005d3bf8);
                if (g_stSceneHotspotTable_005d3bf8.type != 0)
                    ReleasePacketSlot(
                        &g_stSceneHotspotTable_005d3bf8.data);
            } else if (formType == 0x54585448) {
                DecodeSceneResourceChunk(
                    &cursor, &g_stSceneTextTable_005d3c00);
                if (g_stSceneTextTable_005d3c00.type == 2)
                    break;
                ReleasePacketSlot(&g_stSceneTextTable_005d3c00.data);
            }
        } while (formType != 0);
        g_pActiveScenePacket_00492654 = scenePacket;
    }
    hotspotCount = g_stSceneHotspotTable_005d3bf8.count;
    g_pActiveScenePacket_00492654 = 0;
    ReleasePacketSlot(&g_stSceneHotspotTable_005d3bf8.data);
    ReleasePacketSlot(&g_stSceneTextTable_005d3c00.data);
    if (hotspotCount <= 2)
        return 0;
    else
        return 1;
}

/* Function start: 0x409417 */
short FindSceneHotspotAtPosition(void *scenePacket, short offsetX,
                                 short offsetY, short x, short y)
{
    ShortRect bounds;
    short selection;
    unsigned char *cursor;
    SceneHotspot *hotspot;
    short index;
    unsigned char *nextForm;
    unsigned int formType;
    ScenePacketHeader *packet;
    SceneHotspot **hotspots;
    char **textEntries;

    selection = 0;
    g_pszPersonnelFooter_00492658 = 0;
    packet = (ScenePacketHeader *)scenePacket;
    if (g_pActiveScenePacket_00492654 != scenePacket) {
        ReleasePacketSlot(&g_stSceneHotspotTable_005d3bf8.data);
        nextForm = (unsigned char *)packet + packet->formOffset;
        do {
            cursor = nextForm;
            formType = ReadNextSceneForm(&cursor, &nextForm);
            if (formType == 0x52544f48) {
                DecodeSceneResourceChunk(
                    &cursor, &g_stSceneHotspotTable_005d3bf8);
                if (g_stSceneHotspotTable_005d3bf8.type != 0)
                    ReleasePacketSlot(
                        &g_stSceneHotspotTable_005d3bf8.data);
            } else if (formType == 0x54585448) {
                DecodeSceneResourceChunk(
                    &cursor, &g_stSceneTextTable_005d3c00);
                if (g_stSceneTextTable_005d3c00.type == 2)
                    break;
                ReleasePacketSlot(&g_stSceneTextTable_005d3c00.data);
            }
        } while (formType != 0);
        g_pActiveScenePacket_00492654 = scenePacket;
    }

    hotspots = (SceneHotspot **)g_stSceneHotspotTable_005d3bf8.data;
    index = 0;
    while (index < g_stSceneHotspotTable_005d3bf8.count) {
        hotspot = hotspots[index];
        OffsetSceneHotspotBounds(&bounds, hotspot, offsetX, offsetY);
        if (IsPointInRect(x, y, &bounds.left) != 0) {
            selection = hotspot->selection;
            textEntries = (char **)g_stSceneTextTable_005d3c00.data;
            if (textEntries != 0) {
                g_pszPersonnelFooter_00492658 = textEntries[0];
                if (index == g_stSceneHotspotTable_005d3bf8.count - 1) {
                    g_pszPersonnelFooter_00492658 = 0;
                } else {
                    index = (short)(selection - 1);
                    while (index-- != 0) {
                        g_pszPersonnelFooter_00492658 =
                            DosStrchr(g_pszPersonnelFooter_00492658, 0) + 1;
                    }
                    if (*g_pszPersonnelFooter_00492658 == 0)
                        g_pszPersonnelFooter_00492658 = 0;
                }
            }
            break;
        }
        index++;
    }
    return selection;
}

/* Function start: 0x40963B */
short PollSceneHotspotInput(void *scenePacket, short offsetX,
                            short offsetY, short dismissOnBackground,
                            short maximumSelection)
{
    short selection;
    InputEventState event;
    InputEventState *inputEvent;
    short controlPressed;
    short shiftPressed;

    selection = 0;
    ServiceInputDevices(15);
    inputEvent = FindQueuedInputEvent(4);
    if (inputEvent != 0) {
        controlPressed = (short)GetControlKeyState();
        shiftPressed = (short)GetShiftKeyState();
        if ((controlPressed != 0 && shiftPressed != 0) ||
            g_bJoystickCalibrationHotkey_005d1284 != 0) {
            controlPressed = 0;
            shiftPressed = 0;
            g_bJoystickCalibrationHotkey_005d1284 = 0;
            CalibrateJoystickInteractive();
            ReleaseInputEventQueue();
        }
    }
    inputEvent = FindQueuedInputEvent(1);
    if (inputEvent == 0)
        inputEvent = &event;
    GetNextInputEvent(&event);
    selection = FindSceneHotspotAtPosition(
        scenePacket, offsetX, offsetY, inputEvent->x, inputEvent->y);
    if (maximumSelection != 0 && selection > maximumSelection)
        selection = 0;
    if (g_bDisableChalkboardReplay_0049ca58 != 0 && selection == 2) {
        selection = 0;
        g_pszPersonnelFooter_00492658 = 0;
    }
    if (selection != 0) {
        SetMouseCursorShape(g_pPersonnelCursor_005c8464->shape, 1);
    } else if (g_bPersonnelMenuDrawing_0049a6c0 != 0) {
        SetMouseCursorShape(g_pPersonnelCursor_005c8464->shape, 0);
    }
    g_bSceneBackgroundClicked_005c9018 = 0;
    if (inputEvent->type != 1) {
        selection = 0;
    } else if (selection == 0 && dismissOnBackground != 0) {
        g_bSceneBackgroundClicked_005c9018 = 1;
    }
    if (selection != 0 ||
        (dismissOnBackground != 0 &&
         g_bSceneBackgroundClicked_005c9018 != 0)) {
        g_pActiveScenePacket_00492654 = 0;
        ReleasePacketSlot(&g_stSceneHotspotTable_005d3bf8.data);
        ReleasePacketSlot(&g_stSceneTextTable_005d3c00.data);
    }
    return selection;
}

/* Function start: 0x40D6E1 */
void SwapSceneChunkSizeEndian(int *value)
{
    char *bytes;

    bytes = (char *)value;
    bytes[0] ^= bytes[3];
    bytes[3] ^= bytes[0];
    bytes[0] ^= bytes[3];
    bytes[1] ^= bytes[2];
    bytes[2] ^= bytes[1];
    bytes[1] ^= bytes[2];
}

/* Function start: 0x40D812 */
unsigned int ReadNextSceneForm(unsigned char **cursor,
                               unsigned char **nextForm)
{
    unsigned int chunkSize;
    unsigned int formType;
    unsigned char *data;

    data = *cursor;
    formType = 0;
    if (*(unsigned int *)data == 0x4d524f46) {
        data += 4;
        chunkSize = *(unsigned int *)data;
        data += 4;
        SwapSceneChunkSizeEndian((int *)&chunkSize);
        *nextForm = data + ((chunkSize + 1) & ~1);
        formType = *(unsigned int *)data;
        data += 4;
        *cursor = IdentityHandle(data);
    }
    return formType;
}

/* Function start: 0x40D9AD */
void *AllocateScenePointerTable(int count, short elementSize,
                                unsigned short flags,
                                const char *errorMessage)
{
    void *allocation;

    allocation = AllocateZeroedRecords(count, elementSize, flags);
    if (count * elementSize <= 4)
        flags = 0;
    if (allocation == 0)
        FatalErrorAndExit("%s", errorMessage);
    return allocation;
}

/* Function start: 0x40E130 */
signed char DecodeSceneResourceChunk(unsigned char **cursor,
                                     SceneResourceTable *resource)
{
    unsigned char *chunk;
    signed char decoded;
    unsigned int chunkType;

    chunk = *cursor;
    decoded = 0;
    chunkType = *(unsigned int *)chunk;
    switch (chunkType) {
    case 0x43525453:
        decoded = DecodeSceneStructChunk(&chunk, &resource);
        break;
    case 0x5453464f:
        decoded = DecodeSceneOffsetChunk(&chunk, &resource);
        break;
    case 0x424d5953:
        decoded = DecodeSceneSymbolChunk(&chunk, &resource);
        break;
    case 0x454c4946:
        decoded = DecodeSceneFileChunk(&chunk, &resource);
        break;
    case 0x4d524f46:
        break;
    }
    *cursor = IdentityHandle(chunk);
    return decoded;
}

/* Function start: 0x40F0D2 */
void ReportPacketLoadError(void *packet, char *fileName,
                           short retry, short section,
                           const char *sourceTag)
{
    short error;
    const char *operation;
    unsigned int packetSize;

#if 0
    error = g_nPacketError_0049ca90;
    if ((packet == 0 || (error != 0 && error != 8)) &&
        (packet != 0 || error != 8)) {
        if (section != -1)
            packetSize = GetPacketSize(
                g_pDiskFileRecords_005a7cf0[logicalFile].name, section);
        LogMemoryUsage();
        operation = "allocating memory";
        if (packet != 0 && section != -1)
            operation = "reading from disk";
        g_nPacketError_0049ca90 = error;
        sprintf(g_szDefaultTextBuffer_005d2b80,
                "Sorry, an error has occured while %s.\n"
                "Please note the following information:\n"
                "%s #%d (ERR %d  PS%ld  LB%ld  FL%d) at %s\n"
                "Check your configuration.  If this problem persists, please\n"
                "call Origin Systems' service line.  We are sorry for the inconvenience.",
                operation,
                g_pDiskFileRecords_005a7cf0[logicalFile].name,
                (int)section, (int)error, packetSize,
                GetLargestFreeMemoryBlockByType(retry), (int)retry, sourceTag);
        FatalErrorAndExit(g_szDefaultTextBuffer_005d2b80);
    }
#else
    if ((packet == 0 ||
         (g_nPacketError_0049ca90 != 0 &&
          g_nPacketError_0049ca90 != 8)) &&
        (packet != 0 || g_nPacketError_0049ca90 != 8)) {
        error = g_nPacketError_0049ca90;
        if (section != -1)
            packetSize = GetNamedPacketSize(fileName, section);
        LogMemoryUsage();
        g_nPacketError_0049ca90 = error;
        if (g_nOriginDevUnlock_0049d774 == 0) {
            if (packet == 0 || section == -1)
                operation = g_pszPacketAllocateOperation_00492b10;
            else
                operation = g_pszPacketReadOperation_00492b0c;
            sprintf(g_szDefaultTextBuffer_005d2b80,
                    g_pszPacketLoadErrorFormat_00492b14,
                    operation, fileName, (int)section, (int)error,
                    packetSize,
                    GetLargestFreeMemoryBlockByType(retry), (int)retry,
                    sourceTag);
            FatalErrorAndExit(g_szDefaultTextBuffer_005d2b80);
        } else {
            printf("Error: %Fs, packet %d\n", fileName, (int)section);
            printf("packet size: %ld\n", packetSize);
            printf("largest block: %ld  FREE: %ld\n",
                   GetLargestFreeMemoryBlockByType(retry),
                   GetAvailableFarMemoryByType(retry));
            printf("MAIN: %ld   EMS: %ld\n",
                   PreloadMusicTrackHook(), GetAvailableFarMemory());
            exit(1);
        }
    }
#endif
}

#ifndef WC1_SDL
#pragma function(strlen, strcat)
#endif

/* Function start: 0x40F266 */
void AppendPacketLoadDebugLog(char *fileName, short section,
                              void *packet)
{
    short file;
    char truncatedFileName[36];
    char logLine[80];

    file = (short)_open("logfile.txt", 0x4109, 0x80);
    strncpy(truncatedFileName, fileName, strlen(truncatedFileName));
    sprintf(logLine, "\n%s, #%d, %Fp", truncatedFileName,
            (int)section, packet);
    if (g_nShowMemoryStatus_0049d784 != 0) {
        _ltoa((long)GetNamedPacketSize(fileName, section),
              truncatedFileName, 10);
        strcat(logLine, ",Size:");
        strcat(logLine, truncatedFileName);
        _ltoa((long)ReleaseMusicTrackHook(), truncatedFileName, 10);
        strcat(logLine, ",LB:");
        strcat(logLine, truncatedFileName);
        _ltoa((long)PreloadMusicTrackHook(), truncatedFileName, 10);
        strcat(logLine, ",Free:");
        strcat(logLine, truncatedFileName);
    }
    _write((int)file, logLine, strlen(logLine));
    _close((int)file);
}

#ifndef WC1_SDL
#pragma intrinsic(strlen, strcat)
#endif

/* Function start: 0x40F3AE */
void RewritePacketFilenameForInstalledData(char *fileName)
{
    char *extension;
    char *extensionStart;

    if (g_bRewritePacketExtensions_0049cb48 == 1) {
        extension = DosStrchr(fileName, '.');
        extensionStart = extension;
        extension++;
        if (extensionStart != 0 && toupper(*extension) == 'V')
            *extension = 'e';
    }
}

/* Function start: WC2_UNMAPPED */
void *LoadWc1PacketIntoBuffer(short logicalFile, short section,
                              void *destination)
{
    void *packet;

    PromptInsertNumberedDisk(logicalFile);
    packet = PacketLoad(
        g_pDiskFileRecords_005a7cf0[logicalFile].name,
        section, destination, 0, 0, 1);
    ReportPacketLoadError(destination, logicalFile, 0, section, "RP");
    return packet;
}

/* Function start: 0x40F40E */
void *LoadPacketIntoBuffer(char *fileName, short section,
                           void *destination, short registerHandle)
{
    void *packet;

    if (g_bRewritePacketExtensions_0049cb48 == 1)
        RewritePacketFilenameForInstalledData(fileName);
    packet = LoadNamedPacket(fileName, section, destination, 0, 0,
                             registerHandle);
    ReportPacketLoadError(destination, fileName, 0, section, "RP");
    if (g_nOriginDevUnlock_0049d774 != 0)
        AppendPacketLoadDebugLog(fileName, section, packet);
    return packet;
}

/* Function start: 0x40F49D */
void *LoadPacketAllocated(char *fileName, short section)
{
#if 0
    unsigned int packetSize;
    void *packet;
    short retries;

    retries = 5;
    PromptInsertNumberedDisk(logicalFile);
    packetSize = GetPacketSize(
        g_pDiskFileRecords_005a7cf0[logicalFile].name, section);
    packet = AllocateTaggedMemory((unsigned int)(short)packetSize, 0x40);
    if (packet != 0) {
        do {
            retries--;
            PacketLoad(g_pDiskFileRecords_005a7cf0[logicalFile].name,
                       section, packet, 0, 0, 1);
            if (retries <= 0 || g_nPacketError_0049ca90 == 0)
                break;
        } while (g_nPacketError_0049ca90 != 8);
    }
    ReportPacketLoadError(packet, logicalFile, 0, section, "LPN");
    return packet;
#else
    short retries;
    char errorText[40];
    void *packet;
    int packetSize;

    retries = 5;
    if (g_bRewritePacketExtensions_0049cb48 == 1)
        RewritePacketFilenameForInstalledData(fileName);
    packetSize = (int)GetNamedPacketSize(fileName, section);
    packet = AllocateTaggedMemory((int)(short)packetSize, 0x40);
    if (packet != 0) {
        do {
            LoadNamedPacket(fileName, section, packet, 0, 0, 1);
            retries--;
            if (retries <= 0 || g_nPacketError_0049ca90 == 0)
                break;
        } while (g_nPacketError_0049ca90 != 8);
    } else {
        sprintf(errorText, "Error in LPN:  %Fs, #%d\n",
                fileName, (int)section);
        exit_squadron(errorText);
    }
    ReportPacketLoadError(packet, fileName, 0, section, "LPN");
    if (g_nOriginDevUnlock_0049d774 != 0)
        AppendPacketLoadDebugLog(fileName, section, packet);
    return packet;
#endif
}

/* Function start: 0x40F5B6 */
#if 0
void *FetchDiskPacketRetrying(short logicalFile, short section,
                              unsigned short flags)
{
    void *packet = 0;
    short retries = 5;
    const char *fileName;

    PromptInsertNumberedDisk(logicalFile);
    if (flags == 0) {
        if (GetPacketSize(
                g_pDiskFileRecords_005a7cf0[logicalFile].name, section) >
            (int)GetLargestFreeMemoryBlockByType(0)) {
            ReportPacketLoadError(0, logicalFile, 0, section, "LP1");
        }
    }
    fileName = g_pDiskFileRecords_005a7cf0[logicalFile].name;

    do {
        retries--;
        FreePacketAndClear(&packet, flags);
        packet = PacketLoad(fileName, section, 0, flags, 0, 1);
        if (retries < 1 || g_nPacketError_0049ca90 == 0)
            break;
    } while (g_nPacketError_0049ca90 != 8);

    if (packet == 0) {
        if (g_stViewBuffer_005d2b00.pixels != 0) {
            free_viewport(&g_stViewBuffer_005d2b00);
            do {
                retries--;
                FreePacketAndClear(&packet, flags);
                packet = PacketLoad(fileName, section, 0, flags, 0, 1);
                if (retries < 1 || g_nPacketError_0049ca90 == 0)
                    break;
            } while (g_nPacketError_0049ca90 != 8);
            if (AllocateViewport(&g_stViewBuffer_005d2b00,
                                 (short)g_cPrimaryViewBufferColour_0049cb88, 0x20) == 0) {
                ReportPacketLoadError(0, logicalFile, flags, flags,
                                      "LP2");
            }
        }
        if (packet == 0 && g_stSecondaryViewBuffer_005d2c90.pixels != 0) {
            free_viewport(&g_stSecondaryViewBuffer_005d2c90);
            do {
                retries--;
                FreePacketAndClear(&packet, flags);
                packet = PacketLoad(fileName, section, 0, flags, 0, 1);
                if (retries < 1 || g_nPacketError_0049ca90 == 0)
                    break;
            } while (g_nPacketError_0049ca90 != 8);
            if (AllocateViewport(&g_stSecondaryViewBuffer_005d2c90,
                                 (short)g_cSecondaryViewBufferColour_0049cb4c, 0) == 0) {
                ReportPacketLoadError(0, logicalFile, flags, section,
                                      "LP3");
            }
        }
    }
    if (packet == 0 && (flags & 4) == 0 &&
        g_nPacketError_0049ca90 != 0 && g_nPacketError_0049ca90 != 8) {
        ReportPacketLoadError(packet, logicalFile, flags, section, "LP4");
    }

    ClearInputKeyStatePreservingModifiers();
    return packet;
}
#else
void *FetchDiskPacketRetrying(char *fileName, short section,
                              unsigned short flags)
{
    void *packet;
    unsigned int availableMemory;
    short retries;
    unsigned int largestBlock;

    packet = 0;
    retries = 5;
    if (g_bRewritePacketExtensions_0049cb48 == 1)
        RewritePacketFilenameForInstalledData(fileName);
    do {
        FreePacketAndClear(&packet, flags);
        largestBlock = ReleaseMusicTrackHook();
        availableMemory = PreloadMusicTrackHook();
        packet = LoadNamedPacket(fileName, section, 0, flags, 0, 1);
        largestBlock = ReleaseMusicTrackHook();
        availableMemory = PreloadMusicTrackHook();
        retries--;
        if (retries < 1 || g_nPacketError_0049ca90 == 0)
            break;
    } while (g_nPacketError_0049ca90 != 8);

    if (packet == 0 && g_stViewBuffer_005d2b00.pixels != 0) {
        free_viewport(&g_stViewBuffer_005d2b00);
        do {
            FreePacketAndClear(&packet, flags);
            largestBlock = ReleaseMusicTrackHook();
            availableMemory = PreloadMusicTrackHook();
            packet = LoadNamedPacket(fileName, section, 0, flags, 0, 1);
            largestBlock = ReleaseMusicTrackHook();
            availableMemory = PreloadMusicTrackHook();
            retries--;
            if (retries < 1 || g_nPacketError_0049ca90 == 0)
                break;
        } while (g_nPacketError_0049ca90 != 8);
        if (AllocateViewport(&g_stViewBuffer_005d2b00,
                             g_cPrimaryViewBufferColour_0049cb88,
                             0x20) == 0) {
            ReportPacketLoadError(0, fileName, flags, flags, "LP2");
        }
    }
    if (packet == 0 &&
        g_stSecondaryViewBuffer_005d2c90.pixels != 0) {
        free_viewport(&g_stSecondaryViewBuffer_005d2c90);
        do {
            FreePacketAndClear(&packet, flags);
            packet = LoadNamedPacket(fileName, section, 0, flags, 0, 1);
            retries--;
            if (retries < 1 || g_nPacketError_0049ca90 == 0)
                break;
        } while (g_nPacketError_0049ca90 != 8);
        if (AllocateViewport(&g_stSecondaryViewBuffer_005d2c90,
                             g_cSecondaryViewBufferColour_0049cb4c,
                             0) == 0) {
            ReportPacketLoadError(0, fileName, flags, section, "LP3");
        }
    }
    if (packet == 0 && (flags & 4) == 0 &&
        g_nPacketError_0049ca90 != 0 && g_nPacketError_0049ca90 != 8) {
        ReportPacketLoadError(0, fileName, flags, section, "LP4");
    }
    if (g_nOriginDevUnlock_0049d774 != 0) {
        AppendPacketLoadDebugLog(fileName, section, packet);
    }
    return packet;
}
#endif

/* Function start: 0x40F882 */
void InitializeTextContextFromFont(TextContext *context, short fontIndex,
                                   unsigned char colour,
                                   signed char background)
{
#if 0
    int index;

    index = fontIndex;
    if (g_apTextFonts_005d2200[index] == 0) {
        if (fontIndex == 1) {
            g_apTextFonts_005d2200[index] =
                FetchDiskPacketRetrying(0, fontIndex,
                                                         0x10);
        } else {
            g_apTextFonts_005d2200[index] =
                FetchDiskPacketRetrying(0, fontIndex,
                                                         0);
        }
        g_apFontWorkspaces_005a6c10[index] =
            AllocateFontWorkspace((short)index);
    }
    context->font = g_apTextFonts_005d2200[index];
    context->colour = colour;
    context->backgroundColour = (unsigned char)background;
    context->fontWorkspace = g_apFontWorkspaces_005a6c10[index];
    SetTextContext(context);
    return 0;
#else
    if (g_apTextFonts_005d2200[fontIndex] == 0) {
        if (fontIndex == 1) {
            g_apTextFonts_005d2200[fontIndex] =
                FetchDiskPacketRetrying("fonts.fnt", fontIndex, 0x10);
        } else {
            g_apTextFonts_005d2200[fontIndex] =
                FetchDiskPacketRetrying("fonts.fnt", fontIndex, 0);
        }
    }
    context->font = g_apTextFonts_005d2200[fontIndex];
    context->colour = colour;
    context->backgroundColour = (unsigned char)background;
    SetTextContext(context);
#endif
}

/* Function start: WC2_UNMAPPED */
unsigned int ReleaseTextFont(short fontIndex)
{
    int index;

    if (fontIndex == 1)
        return 0;
    index = fontIndex;
    if (g_apTextFonts_005d2200[index] != 0) {
        ReleasePacketHandle(g_apTextFonts_005d2200[index]);
        g_apTextFonts_005d2200[index] = 0;
        FreeFontWorkspace(g_apFontWorkspaces_005a6c10[index]);
        g_apFontWorkspaces_005a6c10[index] = 0;
    }
    return 0;
}

/* Function start: 0x40F96E */
void DrawTextAt(TextContext *context, short x, short y,
                const char *text, unsigned char alignment)
{
#if 0
    char *savedText = context->text;
    unsigned char savedAlignment = context->alignment;

    SetTextContext(context);
    SetTextCursor((unsigned short)x, (unsigned short)y);
    context->text = (char *)text;
    context->alignment = alignment;
    DrawTextString(text);
    context->text = savedText;
    context->alignment = savedAlignment;
    if (context->viewport->pixels == g_stScreenViewport_005d21a0.pixels)
        DIBslam();
    return 0;
#else
    char *savedText;
    unsigned char savedAlignment;

    savedText = context->text;
    savedAlignment = context->alignment;
    if (x < 0 || y < 0)
        return;
    SetTextContext(context);
    SetTextCursor(x, y);
    context->text = (char *)text;
    context->alignment = alignment;
    DrawTextString(text);
    context->text = savedText;
    context->alignment = savedAlignment;
#endif
}

/* Function start: WC2_UNMAPPED */
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

/* Function start: WC2_UNMAPPED */
short CheckWc1DiskAvailable(short logicalFile)
{
    short file;

    FillGraphicSuffix(g_szDiskMarkerFile_00469688,
                      (unsigned char)
                          g_pDiskFileRecords_005a7cf0[logicalFile].diskNumber,
                      3);
    file = OpenDataFileOrDie(g_szDiskMarkerFile_00469688);
    if (file != -1) {
        CloseDataFile((unsigned short)file);
        return 1;
    }
    if (DAT_0059ab34 != 0)
        return 1;
    if (GetCurrentDiskDriveHook() == 'A') {
        if (toupper((int)(signed char)
                        g_abDiskPromptDriveState_005a7d20[1]) == 'B') {
            SelectDiskDriveHook('B');
        } else {
            return 0;
        }
    } else {
        SelectDiskDriveHook('A');
    }
    file = OpenDataFileOrDie(g_szDiskMarkerFile_00469688);
    if (file != -1) {
        CloseDataFile((unsigned short)file);
        return 1;
    }
    return 0;
}

/* Function start: WC2_UNMAPPED */
void __stdcall PromptInsertNumberedDisk(short logicalFile)
{
    TextContext *savedTextContext;
    short backgroundColour;
    signed char diskReady;
    signed char savedViewportMode;
    signed char diskNumber;

    savedViewportMode = 2;
    diskReady = 0;
    if (CheckWc1DiskAvailable(logicalFile) != 0)
        return;
    if (g_bGraphicsActive_00469a20 == 0) {
        diskNumber =
            g_pDiskFileRecords_005a7cf0[logicalFile].diskNumber;
        do {
#if 0
            DiskPromptDrawHook();
            ResetDiskPromptTimer();
#endif
            _cprintf("Please Insert Disk %d. Press any key to continue",
                     (int)diskNumber);
            WaitForInputKey();
        } while (CheckWc1DiskAvailable(logicalFile) == 0);
        return;
    }

    savedTextContext = g_pCurrentTextContext_005c8d1c;
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

    if (g_stViewBuffer_005d2b00.pixels != 0) {
        g_stDiskPromptBackgroundViewport_005a7d00 = g_stViewBuffer_005d2b00;
        backgroundColour = (unsigned char)g_cPrimaryViewBufferColour_0049cb88;
    } else if (g_stSecondaryViewBuffer_005d2c90.pixels != 0) {
        g_stDiskPromptBackgroundViewport_005a7d00 = g_stSecondaryViewBuffer_005d2c90;
        backgroundColour = (unsigned char)g_cSecondaryViewBufferColour_0049cb4c;
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
            (int)g_pDiskFileRecords_005a7cf0[logicalFile].diskNumber);
        DrawTextString(g_szTextScratchBuffer_005d1c40);
        WaitForInputKey();
        if (CheckWc1DiskAvailable(logicalFile) != 0)
            diskReady++;
        if (savedViewportMode != 0) {
            CopyViewportContents(
                &g_stDiskPromptBackgroundViewport_005a7d00,
                &g_stDiskPromptViewport_005a7d40);
        } else {
            ClearViewport(&g_stDiskPromptViewport_005a7d40,
                          backgroundColour);
        }
    } while (diskReady == 0);

    if (savedViewportMode == 1) {
        free_viewport(&g_stDiskPromptBackgroundViewport_005a7d00);
    } else if (savedViewportMode == 2) {
        ClearViewport(&g_stDiskPromptBackgroundViewport_005a7d00,
                      backgroundColour);
    }
    g_pCurrentTextContext_005c8d1c = savedTextContext;
}

/* Function start: WC2_UNMAPPED */
unsigned int GetZeroUnused(void)
{
    return 0;
}

/* Function start: WC2_UNMAPPED */
short CheckEscaped(void)
{
    InputEventState event;
    short escaped;

    PumpWindowMessages();
    escaped = 0;
    if (FindQueuedInputEvent(10) != 0) {
        PeekInputEvent(&event, 10);
        escaped = (short)event.value + 1;
    } else if (FindQueuedInputEvent(2) != 0) {
        PeekInputEvent(&event, 2);
        escaped = (short)event.value + 1;
    } else if (FindQueuedInputEvent(3) != 0) {
        PeekInputEvent(&event, 3);
        escaped = (short)event.value + 1;
        while (PollInputEvent(&event) != 0)
            ;
    }
    if (escaped != 0)
        FlushInputEvents();
    return escaped;
}

/* Function start: 0x40F9F7 */
void FlushPendingInputPresses(void)
{
    PumpWindowMessages();
    while (TakeInputPressCount() != 0)
        ServiceInputDevices(15);
}

/* Function start: 0x40FA2C */
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
        switch (PollInputEvent(&event)) {
        case 2:
        case 10:
            key = 0x1c;
            while (PollInputEvent(&event) != 0)
                ;
            break;
        case 3:
        case 5:
            key = (signed char)event.value;
            if (key == 0x1d) {
                key = 0;
            } else {
                if (PollInputEvent(&event) != 0) {
                    do {
                    } while (PollInputEvent(&event) != 0);
                }
            }
            break;
        }
    } while (key == 0);
    ClearInputKeyStatePreservingModifiers();
    g_bInputMode_0059a848 = savedMode;
    FlushInputEvents();
    return key;
}

/* Function start: WC2_UNMAPPED */
void WaitForWc1SceneAdvance(short duration, short unused)
{
    InputEventState event;
    unsigned char savedMode;
    short eventType;
    short escaped;
    short advanced;

    (void)unused;
    advanced = 0;
    savedMode = g_bInputMode_0059a848;
    g_bInputMode_0059a848 = 1;
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
        eventType = PollInputEvent(&event);
        switch (eventType) {
        case 2:
        case 3:
        case 5:
        case 10:
            advanced++;
            g_bInputMode_0059a848 = savedMode;
            FlushInputEvents();
            do {
                eventType = PollInputEvent(&event);
            } while (eventType != 0);
            ClearInputKeyStatePreservingModifiers();
            break;
        }
    }
}

/* Function start: WC2_UNMAPPED */
void MoveMenuPointerFromKeyboard(InputEventState *event)
{
    int delta;
    int moved;

    delta = g_nKeyboardPointerStep_004696a4 * 2;
    moved = 0;
    if ((short)event->value == 0x4c) {
        if (g_nKeyboardPointerStep_004696a4 == 1)
            g_nKeyboardPointerStep_004696a4 = 4;
        else
            g_nKeyboardPointerStep_004696a4 = 1;
    } else {
        switch ((short)event->value) {
        case 0x47:
            g_stMouseCursorState_0059ab10.y -= delta;
            /* fall through */
        case 0x4b:
            g_stMouseCursorState_0059ab10.x -= delta;
            break;
        case 0x49:
            g_stMouseCursorState_0059ab10.x += delta;
            /* fall through */
        case 0x48:
            g_stMouseCursorState_0059ab10.y -= delta;
            break;
        case 0x4f:
            g_stMouseCursorState_0059ab10.x -= delta;
            /* fall through */
        case 0x50:
            g_stMouseCursorState_0059ab10.y += delta;
            break;
        case 0x51:
            g_stMouseCursorState_0059ab10.y += delta;
            /* fall through */
        case 0x4d:
            g_stMouseCursorState_0059ab10.x += delta;
            break;
        default:
            goto clamp_pointer;
        }
        moved = 1;
    }

clamp_pointer:
    if (g_stMouseCursorState_0059ab10.x < 0)
        g_stMouseCursorState_0059ab10.x = 0;
    else if (g_stMouseCursorState_0059ab10.x > 320)
        g_stMouseCursorState_0059ab10.x = 320;
    if (g_stMouseCursorState_0059ab10.y < 0)
        g_stMouseCursorState_0059ab10.y = 0;
    else if (g_stMouseCursorState_0059ab10.y > 320)
        g_stMouseCursorState_0059ab10.y = 320;

    g_stHostMouseState_0059af70.x = g_stMouseCursorState_0059ab10.x;
    g_stHostMouseState_0059af70.y = g_stMouseCursorState_0059ab10.y;
    if (moved != 0) {
        RetainInputEventsOfType(3);
        QueueInputEvent(13, (unsigned short)g_stMouseCursorState_0059ab10.x,
                        (unsigned short)g_stMouseCursorState_0059ab10.y,
                        0, 0, 0, 0, 0, 0);
        g_bPointerMovedByKeyboard_005a7d54 = 1;
        SetMousePosition(g_stHostMouseState_0059af70.x,
                         g_stHostMouseState_0059af70.y);
    }
}

/* Function start: 0x4345A1 */
void EraseLastTextInputCharacter(void)
{
    Viewport clearArea;
    char *text;
    short textWidth;
    short length;
    short characterWidth;

    text = g_pCurrentTextContext_005c8d1c->text;
    textWidth = MeasureTextPixelWidthClamped(text);
    length = DosStrlen(text);
    if (length != 0) {
        characterWidth = (short)GetFontCharWidth(text[length - 1]);
        clearArea = *g_pCurrentTextContext_005c8d1c->viewport;
        clearArea.left = (short)(clearArea.left +
                                 textWidth - characterWidth);
        clearArea.right = (short)(clearArea.left + characterWidth - 1);
        clearArea.top = g_pCurrentTextContext_005c8d1c->cursorY;
        clearArea.bottom = (short)(clearArea.top +
            ReadWord((unsigned short *)
                g_pCurrentTextContext_005c8d1c->font) - 1);
        SuspendWc1MouseCursor();
        ClearViewport(&clearArea,
                      g_pCurrentTextContext_005c8d1c->backgroundColour);
        ResumeMouseCursor();
        g_pCurrentTextContext_005c8d1c->cursorX = (short)(
            g_pCurrentTextContext_005c8d1c->cursorX - characterWidth);
    }
}

/* Function start: WC2_UNMAPPED */
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

/* Function start: 0x410102 */
short CanShipWeaponDamageTarget(short ship, short target)
{
    if (g_aeObjectClass_00495328[ship] == OBJECT_CLASS_SHIP) {
        if (g_aasShipShield_00495518[target][0] >= 1000 ||
            g_aasShipShield_00495518[target][1] >= 1000)
            return 0;
    }
    return 1;
}

/* Function start: 0x410215 */
short ShipHasTorpedo(short ship)
{
    unsigned char *loadout;
    short weapon;

    loadout = g_aShipWeapons_004956b0[ship];
    weapon = 0;
    for (; (short)(signed char)loadout[0] > weapon; weapon++) {
        if (((ShipWeaponSlot *)(loadout + 1))[weapon].type == 0x13)
            return 1;
    }
    return 0;
}

/* Function start: 0x4103A6 */
short initialize_object(short obj, enum ObjectType type, short owner)
{
    if (obj != -1) {
        set_objects_data(obj, type, owner);
        zero_vector(&g_aShipPosition_00494550[obj]);
        zero_vector(&g_aShipVelocity_0059c010[obj]);
    }
    return obj;
}

/* Function start: 0x41040D */
short borrow_dust(void)
{
    short i;
    signed char found;

    found = 0;
    for (i = 10; i <= 66; i++) {
        if (g_aeObjectClass_00495328[i] == OBJECT_CLASS_DUST) {
            found++;
            break;
        }
    }
    if (!found) {
        for (i = 10; i <= 66; i++) {
            if (g_aeObjectClass_00495328[i] == OBJECT_CLASS_DEBRIS) {
                found++;
                break;
            }
        }
    }
    if (!found) {
        for (i = 10; i <= 66; i++) {
            if (g_aeObjectClass_00495328[i] == OBJECT_CLASS_PROJECTILE &&
                g_acObjectOwner_00495208[i] != 0 &&
                g_asObjectScreenX_00493598[i] == (short)0x8001) {
                found++;
                break;
            }
        }
    }
    if (!found) {
        for (i = 10; i <= 66; i++) {
            if ((g_aeObjectClass_00495328[i] == OBJECT_CLASS_ASTEROID ||
                 g_aeObjectClass_00495328[i] == OBJECT_CLASS_MINE) &&
                g_asObjectScreenX_00493598[i] == (short)0x8001) {
                found++;
                break;
            }
        }
    }
    if (!found) {
        i = -1;
    } else {
        g_asObjectScreenX_00493598[i] = (short)0x8001;
    }
    return i;

#if 0
    short i = 0x22;

    do {
        if (g_aeObjectClass_00495328[i] == OBJECT_CLASS_DUST)
            return i;
        i = i + 1;
    } while (i < 0x2a);
    return -1;
#endif
}

/* Function start: 0x4105BF */
short new_object(enum ObjectType type, short owner)
{
    short obj;

    obj = find_vacant_3d_object();
    if (obj == -1 && owner == 0)
        obj = borrow_dust();
    return initialize_object(obj, type, owner);
}

/* Function start: 0x41062D */
short initialize_ship(enum ObjectType type, short owner)
{
    short obj = get_ship_slot();

    if (obj != -1) {
        initialize_object(obj, type, owner);
        g_asShipSide_004955d0[obj] = SIDE_NEUTRAL;
    }
    return obj;
}

/* Function start: 0x410680 */
short any_selected(unsigned char *loadout, short objectClass)
{
#if 0
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
            if (g_aObjectTypeData_00496d30[
                    ((ShipWeaponSlot *)(loadout + weapon * 7 + 1))->type]
                    .objectClass ==
                    selectedClass &&
                ((ShipWeaponSlot *)(loadout + weapon * 7 + 1))->disabled == 0)
                selected = 1;
        }
    return selected;
#else
    short selected;
    short weapon;

    selected = 0;
    weapon = 0;
    for (; (short)(signed char)loadout[0] > weapon && selected == 0;
         weapon++) {
        if (g_aObjectTypeData_00496d30[
                ((ShipWeaponSlot *)(loadout + 1))[weapon].weaponType]
                .objectClass == objectClass &&
            ((ShipWeaponSlot *)(loadout + 1))[weapon].disabled == 0)
            selected = 1;
    }
    return selected;
#endif
}

/* Function start: 0x410715 */
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
        ((ShipWeaponSlot *)(loadout + weaponOffset + 1))->type;
    objectClass = g_aObjectTypeData_00496d30[preferredType].objectClass;
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
                g_nSelectedReleaseWeaponIndex_004934e0 = -1;
                select_new_release_weapon(preferredType);
            }
        }
        if (get_mode(0) == 1)
            InvalidateVduMode(0);
    }
    return 0;
}

/* Function start: 0x410999 */
void set_objects_data(short obj, enum ObjectType type, short owner)
{
    ObjectTypeData *typeData;
    unsigned char *loadout;
    enum ObjectClass objectClass;
    short value;
    short zero;
    short weapon;

    if (type == OBJECT_TYPE_SPACE_DUST) {
        g_acObjectType_00493980[obj] = type;
        g_aeObjectClass_00495328[obj] = OBJECT_CLASS_DUST;
        return;
    }
    if (g_aObjectTypeData_00496d30[type].shapeSet == 0) {
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
    typeData = &g_aObjectTypeData_00496d30[type];
    g_acObjectType_00493980[obj] = type;
    g_aeObjectClass_00495328[obj] = typeData->objectClass;
    if (type == OBJECT_TYPE_ROCK_CHUNK)
        g_apObjectShape_0059d2f0[obj] =
            g_aObjectTypeData_00496d30[OBJECT_TYPE_ASTEROID1].shapeSet;
    else
        g_apObjectShape_0059d2f0[obj] = typeData->shapeSet;
    init_ijk(obj);
    g_asObjectCollisionRadius_0059d710[obj] = typeData->collisionRadius;
    zero = 0;
    g_asObjectRadarRadius_0059c790[obj] = typeData->radarRadius;
    g_asObjectScale_0059de40[obj] = typeData->scale;
    g_asObjectAfterburnerVelocity_0059c9d0[obj] =
        typeData->afterburnerVelocity;
    g_acObjectOwner_00495208[obj] = (signed char)owner;
    g_asShipAccumulatedDamage_0059dee0[obj] = zero;
    objectClass = g_aeObjectClass_00495328[obj];
    g_asObjectFlip_0059c870[obj] = zero;
    g_acLastCollisionObject_0059d6a0[obj] = -1;
    g_asObjectScreenAngle_0059cd90[obj] = zero;

    if (objectClass >= OBJECT_CLASS_MISSILE) {
        g_asObjectViewFrame_0059d230[obj] = zero;
        g_acShipTarget_00495f20[obj] = -1;
        if (objectClass >= OBJECT_CLASS_SHIP) {
            value = typeData->shieldFore;
            g_aasShipShield_00495518[obj][0] = value;
            g_aasShipMaximumShield_004954f0[obj][0] = value;
            value = typeData->shieldAft;
            g_aasShipShield_00495518[obj][1] = value;
            g_aasShipMaximumShield_004954f0[obj][1] = value;
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
                g_nSelectedReleaseWeaponIndex_004934e0 = -1;
                g_eSelectedGunType_0046c054 = (enum ObjectType)-1;
                for (weapon = (short)(signed char)loadout[0];
                     weapon-- > 0;) {
                    ShipWeaponSlot *slot;

                    slot = (ShipWeaponSlot *)(loadout + weapon * 7 + 1);
                    if (slot->disabled == 0) {
                        if (g_aObjectTypeData_00496d30[
                                slot->type].objectClass ==
                                OBJECT_CLASS_PROJECTILE)
                            g_eSelectedGunType_0046c054 = slot->type;
                        else
                            g_nSelectedReleaseWeaponIndex_004934e0 = weapon;
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

/* Function start: 0x411172 */
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

/* Function start: 0x41133D */
void rotate_object_to_goal(short obj)
{
    ObjectTypeData *typeData;
    short totalError;

    typeData = &g_aObjectTypeData_00496d30[g_acObjectType_00493980[obj]];
    if (g_aeSpecialManeuver_00495600[obj] ==
            SPECIAL_MANEUVER_BLOWING_UP) {
        if ((short)alert_flag(obj, 1) != 0) {
            set_special(obj, SPECIAL_MANEUVER_NONE);
        } else {
            if (g_asObjectCounter_00494be0[obj] == -1 &&
                skill_check(obj) != 0)
                g_aeSpecialManeuver_00495600[obj] =
                    SPECIAL_MANEUVER_NONE;
            return;
        }
    }
    totalError = (short)(abs(g_anObjectYawRotation_0059ce80[obj] -
                            g_anYawGoal_004954c0[obj]) +
                         abs(g_anObjectPitchRotation_0059b2a0[obj] -
                             g_anPitchGoal_004954a8[obj]) +
                         abs(g_anObjectRollRotation_0059d7e0[obj] -
                             g_anRollGoal_004954d8[obj]));
    match_rotation_goal(&g_anObjectPitchRotation_0059b2a0[obj],
                        &g_anPitchGoal_004954a8[obj], totalError,
                        typeData->yawRate);
    match_rotation_goal(&g_anObjectYawRotation_0059ce80[obj],
                        &g_anYawGoal_004954c0[obj], totalError,
                        typeData->pitchRate);
    match_rotation_goal(&g_anObjectRollRotation_0059d7e0[obj],
                        &g_anRollGoal_004954d8[obj], totalError,
                        typeData->rollRate);
}

/* Function start: 0x4117AC */
unsigned int celerate(short ship, int delta)
{
    int maximumSpeed = (int)g_asShipMaximumSpeed_0059c440[ship] << 8;
    int speed;

    speed = g_anShipSpeed_0059b320[ship] + delta;
    g_anShipSpeed_0059b320[ship] = speed;
    if (speed > maximumSpeed)
        g_anShipSpeed_0059b320[ship] = maximumSpeed;
    if (g_anShipSpeed_0059b320[ship] < 0)
        g_anShipSpeed_0059b320[ship] = 0;
    return 0;
}

/* Function start: 0x41181D */
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

/* Function start: 0x4118A9 */
unsigned int steady_object(short ship)
{
    g_anYawGoal_004954c0[ship] = 0;
    g_anPitchGoal_004954a8[ship] = 0;
    g_anRollGoal_004954d8[ship] = 0;
    return 0;
}

/* Function start: 0x4118F2 */
short real_velocity(short obj)
{
    return FixedToShortSaturating(
        Vector_magnitude(&g_aShipVelocity_0059c010[obj]));
}

/* Function start: 0x411922 */
unsigned int fix_velocity(short obj)
{
    ScaleFixedVector(&g_aShipForwardVector_00494208[obj],
                     g_anShipSpeed_0059b320[obj],
                     &g_aShipVelocity_0059c010[obj]);
    return 0;
}

/* Function start: 0x43CE8F */
unsigned int sort_viable_target_list(void)
{
    short nextOuter;
    signed char target;
    short distance;
    short outer;
    short count;
    short inner;

    if (g_cViableTargetCount_0046c088 > 1) {
        count = (short)g_cViableTargetCount_0046c088;
        outer = 0;
        if (count - 1 > 0) {
            do {
                nextOuter = outer + 1;
                inner = nextOuter;
                if (inner < count) {
                    for (; inner < count; inner++) {
                        distance =
                            g_asViableTargetDistance_0059c470[outer];
                        if (g_asViableTargetDistance_0059c470[inner] <
                            distance) {
                            g_asViableTargetDistance_0059c470[outer] =
                                g_asViableTargetDistance_0059c470[inner];
                            target = g_acViableTarget_0059c920[outer];
                            g_asViableTargetDistance_0059c470[inner] =
                                distance;
                            g_acViableTarget_0059c920[outer] =
                                g_acViableTarget_0059c920[inner];
                            g_acViableTarget_0059c920[inner] = target;
                        }
                    }
                }
                outer = nextOuter;
            } while ((int)outer < count - 1);
        }
    }
    return 0;
}
