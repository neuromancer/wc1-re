/* Installation-file loading, startup arguments, and disk-prompt setup.
 * Address range 42c410-42c7ff. The leading empty shutdown hook's exact
 * original ownership remains provisional; see docs/ORDER.md.
 */
#include "wc1.h"

/* Linker commons defined here; LINK orders them by identifier hash. */
ScreenAllocation *pScreenAllocPtr; /* 0x005a66ec */
int aiSoundEffectSourceActiveArray[0x40]; /* 0x005a66f0 */
unsigned char abAudioPlaybackSlots[0xf8]; /* 0x005a67f0 */
int nPassingShipAudioCooldown; /* 0x005a68e8 */
int nAllocateViewportCallsValue; /* 0x005a68ec */
void *pLastPacketAllocation; /* 0x005a68f0 */
FireworkState aFireworksList[30]; /* 0x005a6900 */
unsigned char *pFireworkShape; /* 0x005a6a68 */
ConstellationStar aConstellationStarsTable[10]; /* 0x005a6a70 */

/* Function start: 0x42C410 */
void FxDriverShutdownHook(void)
{
}

/* Function start: 0x42C420 */
unsigned short InitializeDiskPromptTextContext(void)
{
    short textWidth;
    short textHeight;

    nDiskPromptBorderColour = 0x50;
    if (stScreenView.pixels == 0)
        InitializeGameTextContexts();
    stDiskPromptTextContext = stDefaultTxtContext;
    stDiskPromptTextContext.viewport =
        &stDiskPromptViewport;
    stDiskPromptTextContext.text =
        szTxtWorkspaceBuffer;
    stDiskPromptTextContext.alignment = 2;
    stDiskPromptViewport = stScreenView;
    textWidth =
        (short)(MeasureTextPixelWidthClamped(
                    "Press any key when ready") +
                10);
    textHeight =
        (short)((ReadWord((unsigned short *)
                              stDiskPromptTextContext.font) +
                 1) *
                3);
    stDiskPromptBounds.top =
        (short)(100 - textHeight / 2);
    stDiskPromptBounds.left =
        (short)(160 - textWidth / 2);
    stDiskPromptBounds.bottom =
        (short)(stDiskPromptBounds.top + textHeight);
    stDiskPromptBounds.right =
        (short)(stDiskPromptBounds.left + textWidth);
    InitializeTextContextFromFont(&stDiskPromptTextContext, 1,
                                  cBlackColour,
                                  (signed char)cViewportClearColour);
    bGraphicsActive = 1;
    return 0;
}

/* Function start: 0x42C510 */
unsigned short RewriteDiskFileGraphicsExtensions(short videoMode)
{
    DiskFileRecord *record;
    char *extensionPosition;
    char extension;

    record = pDiskFileRecords;
    switch (videoMode) {
    case 0:
        extension = 'v';
        break;
    case 1:
        extension = 'e';
        break;
    case 3:
        extension = 't';
        break;
    }

    while (record->name[0] != '\0') {
        extensionPosition = strchr(record->name, '.');
#ifdef SDL_PORT
        if (extensionPosition != 0) {
            extensionPosition++;
            if (toupper((int)*extensionPosition) == 'V')
                *extensionPosition = extension;
        }
#else
        if (extensionPosition++ != 0 &&
            toupper((int)*extensionPosition) == 'V')
            *extensionPosition = extension;
#endif
        record++;
    }
    return 0;
}

/* Function start: 0x42C580 */
short LoadWingCmdrCfgFile(short argc, char **argv)
{
    FILE *file;
    char *destination;
    short argumentCount;
    short argumentIndex;
#ifdef SDL_PORT
    char resolvedPath[PATH_MAX];
#endif

    argumentIndex = 1;
    argumentCount = 0;
    destination = szTxtWorkspaceBuffer;
#ifdef SDL_PORT
    if (SdlResolvePath("WINGCMDR.CFG", resolvedPath,
                          sizeof(resolvedPath)))
        file = fopen(resolvedPath, "rt");
    else
        file = 0;
#else
    file = fopen("WINGCMDR.CFG", "rt");
#endif
    while (file != 0) {
        if ((short)fscanf(file, "%s", destination) == -1) {
            fclose(file);
            break;
        }
        pCurrentStartupArgumentsPtr[argumentCount++] =
            destination;
        destination = strchr(destination, 0) + 1;
    }

    while (argc-- != 0) {
        strcpy(destination, argv[argumentIndex]);
        pCurrentStartupArgumentsPtr[argumentCount++] = destination;
        argumentIndex++;
        destination = strchr(destination, 0) + 1;
    }
    return (short)(argumentCount - 1);
}

/* Function start: 0x42C660 */
unsigned short LoadInstallDat(void)
{
    DiskFileRecord *records;
    DiskFileRecord *record;
    DiskFileRecord *entry;
    unsigned int size;
    short file;
    short maximumId;

    maximumId = 0;
    SystemDebugPrintf("Loading INSTALL.DAT\n");
    file = OpenDataFileOrDie("install.dat");
    if (file == -1) {
        SystemDebugPrintf("Unable to open INSTALL.DAT\n");
        SystemDebugPrintf(
            "[SYSTEM]: Exiting Prematurely (LoadInstallData)\n");
        ClearDebugPauseFlags();
        PumpMessagesDuringWait();
        exit(0);
    }
    size = (unsigned int)filelength(file);
    records = (DiskFileRecord *)AllocateTaggedMemory(size, 0);
    if (records == 0) {
        SystemDebugPrintf("Unable to load INSTALL.DAT\n");
        SystemDebugPrintf(
            "[SYSTEM]: Exiting Prematurely (LoadInstallData)\n");
        ClearDebugPauseFlags();
        PumpMessagesDuringWait();
        exit(0);
    }
    ReadDataFileAtOffset(file, 0, size, records);
    CloseDataFile(file);

    record = records;
    while (record->name[0] != 0) {
        if (maximumId < record->logicalFile &&
            record->logicalFile != 0xff)
            maximumId = record->logicalFile;
        record++;
    }
    maximumId++;

#ifdef SDL_PORT
    pDiskFileRecords =
        (DiskFileRecord *)AllocateTaggedMemory(
            sizeof(DiskFileRecord) * 78, 0);
    if (pDiskFileRecords != 0)
        memset(pDiskFileRecords, 0,
               sizeof(DiskFileRecord) * 78);
#else
    pDiskFileRecords =
        (DiskFileRecord *)AllocateTaggedMemory(0x4b0, 0);
    memset(pDiskFileRecords, 0,
           (maximumId + 1) * sizeof(DiskFileRecord));
#endif
    if (pDiskFileRecords == 0) {
        SystemDebugPrintf("Unable to copy INSTALL.DAT\n");
        SystemDebugPrintf(
            "[SYSTEM]: Exiting Prematurely (LoadInstallData)\n");
        ClearDebugPauseFlags();
        PumpMessagesDuringWait();
        exit(0);
    }

    entry = pDiskFileRecords;
    while (maximumId > 0) {
        entry->name[0] = ' ';
        entry++;
        maximumId--;
    }

    record = records;
    while (record->name[0] != 0) {
        if (record->logicalFile != 0xff)
            pDiskFileRecords[record->logicalFile] = *record;
        record++;
    }
    ReleasePacketHandle(records);
    pDiskFileRecords++;
#ifdef SDL_PORT
    if (SdlUsingDosData())
        SdlCompleteDosInstallTable(pDiskFileRecords);
#endif
    return 0;
}
