/*
 *  Game startup, graphics defaults, and Origin-FX initialization.
 *
 *  Address range 4219c0-42200f (see docs/ORDER.md). The initialized
 *  colour and memory tables precede this unit's startup string pool.
 */
#define WC1_INIT_SOURCE
#include "wc1.h"

short bSlowSceneAnimation = 0; /* 0x00469998 */
unsigned char cBlackColour = 0; /* 0x0046999c */
unsigned char cViewportClearColour = 15; /* 0x004699a0 */
unsigned char cBlueColour = 0x25; /* 0x004699a4 */
unsigned char cYellowColour = 0x47; /* 0x004699a8 */
unsigned char cRedColour = 0x50; /* 0x004699ac */
unsigned char cDarkGreenColour = 0xaa; /* 0x004699b0 */
unsigned char cPrimaryTextColour = 0xa6; /* 0x004699b4 */
unsigned char cOrangeColour = 0x85; /* 0x004699b8 */
unsigned char cDarkBlueColour = 0x27; /* 0x004699bc */
unsigned char cDarkGreyColour = 7; /* 0x004699c0 */
unsigned char cLightGreyColour = 0x0b; /* 0x004699c4 */
unsigned char cMagentaColour = 0xb6; /* 0x004699c8 */
unsigned char cDefaultTextColour = 0xa8; /* 0x004699cc */
unsigned char cBrownColour = 0xfd; /* 0x004699d0 */
unsigned char cAsteroidColour = 0xf5; /* 0x004699d4 */
unsigned char cPrimaryViewBufferColour = 0xbf; /* 0x004699d8 */
unsigned char abLegacyVideoModeColours[14] = {
    9, 14, 4, 2, 10, 12, 15, 8, 7, 5, 2, 6, 7, 0
}; /* 0x004699e0 */
short asConversationTextColours[24] = {
    0x25, 0xb6, 0x9a, 0x50, 0x94, 0x85, 0x27, 0xa6,
    0xfd, 0x47, 0xaa, 0x0b, 0x09, 0x0d, 0x03, 0x04,
    0x0b, 0x0c, 0x01, 0x0a, 0x06, 0x0e, 0x02, 0x07
}; /* 0x004699f0 */
int bGraphicsActive = 0; /* 0x00469a20 */
char szInsufficientMemory[100] =
    "You do not have enough memory to play Wing Commander.\n"
    "Refer to your reference guide for assistance."; /* 0x00469a28 */
int anBaseMemoryReservationByVideoMode[4] = {
    280000, 260000, 280000, 0
}; /* 0x00469a90 */
int anFullMusicMemoryReservationByVideoMode[4] = {
    306000, 286000, 306000, 0
}; /* 0x00469aa0 */
int anExpandedMemoryReservationByVideoMode[3] = {
    258000, 238000, 258000
}; /* 0x00469ab0 */


/* Linker commons defined here; LINK orders them by identifier hash. */
short asCollisionTimes[10]; /* 0x005a7ca0 */
int nCurrentWingmanKilledThisMission; /* 0x005a7cb4 */
short nWingmanKillCount; /* 0x005a7cb8 */
short nLogicLastFoundShipValue; /* 0x005a7cba */
short asCollisionPartnerTable[10]; /* 0x005a7cc0 */

/* Function start: 0x4219C0 */
unsigned int LoadGamePaletteFile(void)
{
    short index;

    PromptInsertNumberedDisk(0);
    switch (nVideoMode) {
    case 9:
    case 13:
        index = 0;
        do {
            ((unsigned char *)&cBlueColour)[index] =
                abLegacyVideoModeColours[index];
            index++;
        } while ((unsigned int)(int)index < 14);
        index = 0;
        do {
            asConversationTextColours[index] =
                asConversationTextColours[index + 12];
            index++;
        } while ((unsigned int)(int)index < 12);
        return 0;
    case 0x13:
        LoadPaletteTripletsFile("game.pal");
        ResetCockpitPaletteEntries();
        SaveGamePalette();
        return 0;
    }
    return 0;
}

/* Function start: 0x421A40 */
unsigned int EMShutDown(void)
{
    if (nEventManagerActive != 0)
        ShutdownEventManager();
    return 0;
}

/* Function start: 0x421A60 */
unsigned short InitializeEventManagerResources(void)
{
    DAT_0059a9f0 = 20;
    nInputTickScale = 20;
    nUnusedInputTimerFlag = 1;
    *(unsigned char *volatile *)&stMouseCursorState.shape = pMouseCursorResource =
        FetchDiskPacketRetrying(14, 0, 0x10);
    *(unsigned short volatile *)&stMouseCursorState.frame = 0;
    *(Viewport *volatile *)&stMouseCursorState.viewport = &stScreenView;
    return 0;
}

/* Function start: 0x421AB0 */
unsigned int EMStartUp(void)
{
    PromptInsertNumberedDisk(0);
    RegisterEventManagerShutdown((void (*)(void))LogMemoryUsage);
    if (InitializeEventManager(20, InitializeEventManagerResources, 0) == 0)
        exit_squadron("EMStartUp Failed");
    ConfigureEventManagerPointer(stMouseCursorState.shape, 0);
    SetEventManagerPump(get_player_input);
    nMenuInputRepeatDelay = 6;
    return 0;
}

/* Function start: 0x421B10 */
unsigned int LoadOriginFxDrivers(void)
{
    int memoryThreshold;
    int videoModeMemory;

    memoryThreshold = 100000;
    chdir("gamedat");
    nNearHeapMaxDescriptors = 0x80;
    IsSoundHardwarePresent(8);
    if (bCurrentExpandedMemoryAvail == 0)
        SystemDebugPrintf("No ");
    SystemDebugPrintf("Expanded Memory Detected.\n");
    bGraphicsActive = 0;
    PromptInsertNumberedDisk(0x38);
    if (GetMusicDriverPresent(bSlowSceneAnimation) == 0)
        exit_squadron("Failed to load Origin-FX drivers");
    RewriteDiskFileGraphicsExtensions(bSlowSceneAnimation);
    LoadJoystickCalibrationFile(9, 9, 1, 1);
    nInputDoubleClickInterval = 2;
    EMStartUp();
    GetFxDriverInitResult();
    dwOriginalFreeMem = GetFixedOneMillionThunkAlt(0);
    if (nMusicPlaybackMode != 0 && nMusicPlaybackMode != 3)
        memoryThreshold = 210000;
    SetFrameTimerPeriodDirect(0x78);
    if ((int)GetFixedOneMillionThunk(4) > memoryThreshold) {
        nAvailableGameMemory =
            (int)dwOriginalFreeMem -
            anExpandedMemoryReservationByVideoMode[
                bSlowSceneAnimation];
        if (nAvailableGameMemory < 0)
            exit_squadron(szInsufficientMemory);
        nMemoryConfig = 2;
        SystemDebugPrintf("Expanded Memory fully used.\n");
    } else {
        nAvailableGameMemory =
            (int)dwOriginalFreeMem -
            anBaseMemoryReservationByVideoMode[
                bSlowSceneAnimation];
        if (nAvailableGameMemory < 0)
            exit_squadron(szInsufficientMemory);
        nMemoryConfig = 0;
        if (nMusicPlaybackMode == 1 || nMusicPlaybackMode == 2) {
            videoModeMemory =
                anFullMusicMemoryReservationByVideoMode[
                    bSlowSceneAnimation];
            if ((int)dwOriginalFreeMem > videoModeMemory) {
                nMemoryConfig = 1;
                nAvailableGameMemory =
                    (int)dwOriginalFreeMem - videoModeMemory;
                SystemDebugPrintf("Full");
            } else {
                SystemDebugPrintf("Limited");
            }
            SystemDebugPrintf(" music will play.");
        }
    }

    LoadSpaceflightResources();
    FxDriverShutdownHook();
    SetEventManagerPump(PollJoystickButtonEvents);
    PromptInsertNumberedDisk(0);
    ShutdownVideoHook(
        acGraphicsModeByAnimationSpeed[
            bSlowSceneAnimation]);
    if (acGraphicsModeByAnimationSpeed[bSlowSceneAnimation] !=
        GetTargetColourIndex())
        exit_squadron("Requested graphics display mode not available.");
    LoadGamePaletteFile();
    InitializeGameTextContexts();
    InitializeDiskPromptTextContext();
    GetEventManagerStatus();
    nFrameSkip = 1;
    if (nMusicPlaybackMode != 0 && nMusicPlaybackMode != 3)
        GetFxDriverStatus();
    initialize_direction_view_frames();
    pConstellationDefinitionsPtr =
        LoadPacketAllocated(0x3a, 0);
    pMissionCampaignInfo = LoadPacketAllocated(0x3a, 1);
    SystemDebugPrintf("\n[SYSTEM] : Exiting initialize()\n");
    return 0;
}

/* Function start: 0x421D80 */
unsigned int InitializeGameTextContexts(void)
{
    InitializeDIBScreenViewport(&stScreenView,
                                (unsigned char)cBlackColour);
    stModalSrcViewport = stScreenView;
    stDefaultTxtContext.viewport =
        &stModalSrcViewport;
    stDefaultTxtContext.text =
        szDefaultTxtBuf;
    InitializeTextContextFromFont(&stDefaultTxtContext, 1,
                                  (unsigned char)cPrimaryTextColour,
                                  cBlackColour);
    SetTextContext(&stDefaultTxtContext);

    stHudMessageTextContext.viewport = &stSpaceBufferViewport;
    stHudMessageTextContext.text = szDefaultTxtBuf;
    stHudMessageTextContext.alignment = 2;
    InitializeTextContextFromFont(&stHudMessageTextContext, 1,
                                  (unsigned char)cRedColour, -1);
    return 0;
}

/* Function start: 0x421E20 */
unsigned int initialize_direction_view_frame(short yaw, short pitch,
                                             signed char frame)
{
    FixedVector right = { 0x100, 0, 0 };
    FixedVector up = { 0, 0x100, 0 };
    FixedVector forward = { 0, 0, 0x100 };
    short index;

    rotate_about_j(yaw, &right, &forward);
    rotate_about_i(pitch, &up, &forward);
    index = (short)frame;
    aDirectionViewRightVector[index] = right;
    aDirectionViewUpVectorArray[index] = up;
    aDirectionViewForwardVector[index] = forward;
    return 0;
}

/* Function start: 0x421EF0 */
unsigned int initialize_direction_view_frames(void)
{
    signed char frame;
    signed char pitchBand;
    signed char yawSector;
    short yaw;
    short pitch;

    frame = 1;
    pitch = 90;
    initialize_direction_view_frame(0, pitch, 0);
    for (pitchBand = 0; pitchBand < 5; pitchBand++) {
        pitch -= 30;
        yaw = 0;
        for (yawSector = 0; yawSector < 12; yawSector++) {
            initialize_direction_view_frame(yaw, pitch, frame++);
            yaw += 30;
        }
    }
    initialize_direction_view_frame(0, -90, frame);
    return 0;
}

/* Function start: 0x421F50 */
unsigned int LoadSpaceflightResources(void)
{
    unsigned char *debrisShapeSet;

    if (bCurrentExpandedMemoryAvail == 0)
        return 0;
    if (LoadShapeSet(aCommon3SpaceResources, 4, -1) == 0)
        return 0;
    LoadShapeSet(aMissionResourceDescriptors, 4, -1);
    aObjectTypeData[OBJECT_TYPE_DEBRIS_WING].shapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet;
    debrisShapeSet =
        aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_O_RING].shapeSet =
        debrisShapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_GLASS].shapeSet =
        debrisShapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_SHIP_TUBING].shapeSet =
        debrisShapeSet;
    aObjectTypeData[OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK].shapeSet =
        debrisShapeSet;
    LoadShapeSet(aCockpitResourceDescriptors, 4, -1);
    apCommunicationPortraitShapesTable[8] = 0;
    apCommunicationPortraitShapesTable[9] = 0;
    apCommunicationPortraitShapesTable[10] = 0;
    apCommunicationPortraitShapesTable[11] = 0;
    apCommunicationPortraitShapesTable[13] = 0;
    return 0;
}

/* Function start: 0x421FE0 */
unsigned int GetFxDriverInitResult(void)
{
    return 0;
}

/* Function start: 0x421FF0 */
unsigned int GetMessagePumpResult(void)
{
    return 0;
}

/* Function start: 0x422000 */
unsigned int GetFxDriverStatus(void)
{
    return 0;
}
