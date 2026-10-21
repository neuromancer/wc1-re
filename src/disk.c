/*
 *  Disk data files and packet fetching with retry.
 *
 *  Address range 41d120-41dedf (provisional -- see docs/ORDER.md).
 *  Boundary evidence: OpenDiskDataFile/FetchDiskPacketRetrying/PromptInsertNumberedDisk;
 *  the Mac `ship` unit begins with initialize_object at 41dee0.
 */
#include "wc1.h"

const char *apszGraphicsModeNames[4] = {
    "vga",
    "ega",
    "cga",
    "tga"
}; /* 0x00469668 */
const char *pszPacketReadOperation = "reading from disk"; /* 0x00469678 */
const char *pszPacketAllocationOperation = "allocating memory"; /* 0x0046967c */
/* 0x00469680 */
const char *pszPacketLoadErrorFormat =
    "Sorry, an error has occured while %s.\n"
    "Please note the following information:\n"
    "%s #%d (ERR %d  PS%ld  LB%ld  FL%d) at %s\n"
    "Check your configuration.  If this problem persists, please\n"
    "call Origin Systems' service line.  We are sorry for the inconvenience.";
char szDiskMarkerFile[9] = "DISK.000"; /* 0x00469688 */
short nDiskPromptBorderColour = 0x50; /* 0x00469694 */
/* Unreferenced pointer sequence: two strings followed by a null entry. */
const char *DAT_00469698[3] = {
    "(OVR)", "", 0
}; /* 0x00469698 */
short nKeyboardPointerStep = 4; /* 0x004696a4 */

/* Linker commons defined here; LINK orders them by identifier hash. */
DiskFileRecord *pDiskFileRecords; /* 0x005a7cf0 */
Viewport stDiskPromptBgView; /* 0x005a7d00 */
/* GetShutdownErrorCode's buffer.  The prompt viewport that follows it at
 * 0x005A7D40 bounds it to 17-32 bytes; the DOS hook that filled it is an
 * empty stub, so its true length is unknown. */
unsigned char abDiskPromptDriveStates[0x20]; /* 0x005a7d20 */
Viewport stDiskPromptViewport; /* 0x005a7d40 */
int bPtrMovedByKeyboardState; /* 0x005a7d54 */
TextContext stDiskPromptTextContext; /* 0x005a7d60 */
ShortRect stDiskPromptBounds; /* 0x005a7d80 */

/* Function start: 0x41D120 */
void ReportPacketLoadError(void *packet, short logicalFile,
                           short retry, short section,
                           const char *sourceTag)
{
    unsigned int packetSize;
    short error;
    const char *operation;

    if (packet != 0 && (nPacketError == 0 || nPacketError == 8))
        return;
    if (packet == 0 && nPacketError == 8)
        return;
    error = nPacketError;
    if (section != -1)
        packetSize = GetPacketSize(
            pDiskFileRecords[logicalFile].name, section);
    LogMemoryUsage();
    nPacketError = error;
    if (packet != 0 && section != -1)
        operation = pszPacketReadOperation;
    else
        operation = pszPacketAllocationOperation;
    sprintf(szDefaultTxtBuf,
            pszPacketLoadErrorFormat,
            operation,
            pDiskFileRecords[logicalFile].name,
            (int)section, (int)error, packetSize,
            GetFixedOneMillionThunkAlt(retry), (int)retry, sourceTag);
    FatalErrorAndExit(szDefaultTxtBuf);
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
    int fileIndex;
    DiskFileRecord *files;
    short error;

    retries = 5;
    PromptInsertNumberedDisk(logicalFile);
    fileIndex = logicalFile;
    packetSize = GetPacketSize(
        pDiskFileRecords[fileIndex].name, section);
    packet = AllocateTaggedMemory((unsigned int)(short)packetSize, 0x40);
    if (packet != 0) {
        files = pDiskFileRecords;
        do {
            retries--;
            PacketLoad(files[fileIndex].name,
                       section, packet, 0, 0);
            error = nPacketError;
            files = pDiskFileRecords;
            if (retries <= 0 || error == 0)
                break;
        } while (error != 8);
    }
    ReportPacketLoadError(packet, logicalFile, 0, section, "LPN");
    return packet;
}

/* Function start: 0x41D2E0 */
void *FetchDiskPacketRetrying(short logicalFile, short section,
                              short flags)
{
    void *packet = 0;
    short retries = 5;

    PromptInsertNumberedDisk(logicalFile);
    if (flags == 0) {
        if ((int)GetPacketSize(
                pDiskFileRecords[logicalFile].name, section) >
            (int)GetFixedOneMillionThunkAlt(0)) {
            ReportPacketLoadError(0, logicalFile, flags, section, "LP1");
        }
    }

    do {
        retries--;
        FreePacketAndClear(&packet, flags);
        packet = PacketLoad(pDiskFileRecords[logicalFile].name,
                            section, 0, flags, 0);
        if (retries <= 0 || nPacketError == 0)
            break;
    } while (nPacketError != 8);

    if (packet == 0 && stSpaceBufferViewport.pixels != 0) {
        free_viewport(&stSpaceBufferViewport);
        do {
            retries--;
            FreePacketAndClear(&packet, flags);
            packet = PacketLoad(pDiskFileRecords[logicalFile].name,
                                section, 0, flags, 0);
            if (retries <= 0 || nPacketError == 0)
                break;
        } while (nPacketError != 8);
        if (AllocateViewport(&stSpaceBufferViewport,
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
            packet = PacketLoad(pDiskFileRecords[logicalFile].name,
                                section, 0, flags, 0);
            if (retries <= 0 || nPacketError == 0)
                break;
        } while (nPacketError != 8);
        if (AllocateViewport(&stSceneBuffer,
                             (short)cBlackColour, 0) == 0) {
            ReportPacketLoadError(0, logicalFile, flags, section,
                                  "LP3");
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
    if (context->viewport->pixels == stScreenView.pixels)
        DIBslam();
    return 0;
}

/* Function start: 0x41D650 */
unsigned int SortSignedByteValuesAscending(signed char *values,
                                           short count)
{
    short next;
    signed char outer;
    signed char inner;

    outer = 0;
    if (0 < count - 1) {
        do {
            inner = (signed char)(outer + 1);
            next = (short)inner;
            if ((short)inner < count) {
                do {
                    signed char value = values[outer];

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
    if (bCurrentRunningFromHardDiskFlag != 0)
        return 1;
    if (GetCurrentDiskDriveHook() == 'A') {
        if (toupper((int)(signed char)
                        abDiskPromptDriveStates[1]) == 'B') {
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
            ResetDiskPromptTimerHook(2, 10);
            cprintf("Please Insert Disk %d. Press any key to continue",
                    (int)diskNumber);
            WaitForInputKey();
        } while (OpenDiskDataFile(logicalFile) == 0);
        return;
    }

    savedTextContext = pCurTextContext;
    SetTextContext(&stDiskPromptTextContext);
    *(ShortRect *)&stDiskPromptBgView.left = stDiskPromptBounds;
    *(ShortRect *)&stDiskPromptViewport.left = stDiskPromptBounds;

    if (stSpaceBufferViewport.pixels != 0) {
        stDiskPromptBgView = stSpaceBufferViewport;
        backgroundColour = (unsigned char)cPrimaryViewBufferColour;
    } else if (stSceneBuffer.pixels != 0) {
        stDiskPromptBgView = stSceneBuffer;
        backgroundColour = (unsigned char)cBlackColour;
    } else {
        savedViewportMode = (signed char)AllocateViewport(
            &stDiskPromptBgView, -1, 0);
        backgroundColour = (short)(unsigned int)savedTextContext;
    }

    *(ShortRect *)&stDiskPromptBgView.left = stDiskPromptBounds;
    if (savedViewportMode != 0) {
        CopyViewportContents(&stDiskPromptViewport,
                             &stDiskPromptBgView);
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
        DrawTextString(szTxtWorkspaceBuffer);
        WaitForInputKey();
        if (OpenDiskDataFile(logicalFile) != 0)
            diskReady++;
        if (savedViewportMode != 0) {
            CopyViewportContents(
                &stDiskPromptBgView,
                &stDiskPromptViewport);
        } else {
            ClearViewport(&stDiskPromptViewport,
                          backgroundColour);
        }
    } while (diskReady == 0);

    if (savedViewportMode == 1) {
        free_viewport(&stDiskPromptBgView);
    } else if (savedViewportMode == 2) {
        ClearViewport(&stDiskPromptBgView,
                      backgroundColour);
    }
    pCurTextContext = savedTextContext;
}

/* Function start: 0x41D9B0 */
unsigned int SetDiskFileNumber(char *fileName, short diskNumber)
{
    char *number;

    number = strchr(fileName, '.');
    if (number++ == 0) {
        *fileName = *apszGraphicsModeNames[bSlowSceneAnimation];
        number = fileName + 1;
    } else {
        number++;
    }
    number[2] = '\0';
    ConvertChar_Int(number, diskNumber, 2);
    return 0;
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

    savedMode = bInputModeState;
    bInputModeState = 1;
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
                if (PollInputEvent(&event, 0xff) != 0) {
                    do {
                    } while (PollInputEvent(&event, 0xff) != 0);
                }
                break;
            }
            break;
        }
    } while (key == 0);
    ClearInputKeyStatePreservingModifiers();
    bInputModeState = savedMode;
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
    savedMode = bInputModeState;
    bInputModeState = 1;
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
            bInputModeState = savedMode;
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
        QueueInputEvent(13, stMouseCursorState.x,
                        stMouseCursorState.y,
                        0, 0, 0, 0);
        bPtrMovedByKeyboardState = 1;
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

    text = pCurTextContext->text;
    textWidth = MeasureTextPixelWidthClamped(text);
    length = DosStrlen(text);
    if (length != 0) {
        characterWidth = (short)GetFontCharWidth(text[length - 1]);
        clearArea = *pCurTextContext->viewport;
        clearArea.left = (short)(clearArea.left +
                                 textWidth - characterWidth);
        clearArea.right = (short)(clearArea.left + characterWidth - 1);
        clearArea.top = pCurTextContext->cursorY;
        clearArea.bottom = (short)(clearArea.top +
            ReadWord((unsigned short *)
                pCurTextContext->font) - 1);
        LeaveAllocationScope();
        ClearViewport(&clearArea,
                      pCurTextContext->backgroundColour);
        EnterAllocationScope();
        pCurTextContext->cursorX = (short)(
            pCurTextContext->cursorX - characterWidth);
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
