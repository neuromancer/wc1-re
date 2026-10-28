/*
 *  Conversation animation and campaign mission packet decoding.
 *
 *  Address range 404cd0-40609f (see docs/ORDER.md). The mouth-animation
 *  table follows eject.c's literals; fl.c begins at 4060a0. LoadMissionData
 *  at 4059b0 is corroborated by the Mac cmpgn segment names.
 */
#include "wc1.h"

static short asMouthFramesByPhoneme[26] = {
    0, 5, 4, 4, 1, 8, 4, 7, 0, 4, 4, 7, 5,
    4, 2, 5, 6, 4, 4, 4, 3, 4, 6, 4, 4, 4
}; /* 0x004655f0 */

/* Linker commons defined here; LINK orders them by identifier hash. */
signed char cScrambleWalkerPair; /* 0x005a8748 */
unsigned char *pScrambleCanopyShape; /* 0x005a874c */
unsigned char *pScrambleShipShape; /* 0x005a8750 */
short nTalkingHeadFaceX; /* 0x005a8754 */
short nTalkingHeadFaceCoordY; /* 0x005a8756 */
short nTalkingHeadMouthY; /* 0x005a8758 */
short nTalkingHeadMouthX; /* 0x005a875a */

/* Function start: 0x404CD0 */
unsigned int ParseFaceAnimation(char *text, short *commands)
{
    char duration[8];
    char *durationCursor;
    short frame;
    short sequenceIndex;

    sequenceIndex = 0;
    frame = (short)*text++;
    while (frame != 0) {
        if (frame == 'R') {
            *commands = -2;
            commands += 2;
            commands[-1] = sequenceIndex;
        } else {
            if (frame >= 'A' && frame <= 'F')
                frame = (short)(frame - 'A' + 10);
            else
                frame = (short)(frame - '0');
            *commands = frame;
            durationCursor = duration;
            frame = (short)*text++;
            while (frame != ',') {
                *durationCursor++ = (char)frame;
                frame = (short)*text++;
            }
            *durationCursor = '\0';
            commands[1] = (short)atoi(duration);
            commands += 2;
            sequenceIndex++;
        }
        frame = (short)*text++;
    }
    *commands = -1;
    return 0;
}

/* Function start: 0x404D70 */
unsigned int ParseMouthAnimation(char *text, short *commands)
{
    char character;
    char duration[5] = "";
    short durationLength;
    short ticks;

    while ((character = *text++) != '\0') {
        if (character == '$') {
            *commands = 9;
        } else if (character <= 'z' && character >= 'a') {
            *commands = asMouthFramesByPhoneme[
                character - 'a'];
        } else {
            continue;
        }
        ticks = 1;
        durationLength = 0;
        while (*text <= '9') {
            character = *text;
            if (character < '0')
                break;
            duration[durationLength] = character;
            durationLength++;
            text++;
            duration[durationLength] = '\0';
            ticks = (short)atoi(duration);
            character = *text;
        }
        commands[1] = ticks;
        commands += 2;
    }
    *commands = -1;
    return 0;
}

/* Function start: 0x404E10 */
char *AddPCName(const char *text)
{
    char formatted[12];
    const char *marker;
    char *output;
    short length;

    szTxtWorkspaceBuffer[0] = '\0';
    for (;;) {
        marker = DosStrchr(text, '$');
        if (marker == 0) {
            DosStrcat(szTxtWorkspaceBuffer, text);
            return szTxtWorkspaceBuffer;
        }
        output = DosStrchr(szTxtWorkspaceBuffer, 0);
        while (text != marker) {
            *output++ = *text++;
        }
        *output = '\0';
        marker++;
        switch (*marker++) {
        case 'A':
            DosStrcat(szTxtWorkspaceBuffer,
                      apszMedalNames[
                          nCurrentConversationMedalIndex]);
            break;
        case 'C':
            DosStrcat(szTxtWorkspaceBuffer,
                      stCampaignState.currentPilot->callsign);
            break;
        case 'K':
            sprintf(formatted, "%d",
                    nPlayerKillTotal);
            DosStrcat(szTxtWorkspaceBuffer, formatted);
            break;
        case 'L':
            sprintf(formatted,
                    "%d",
                    nWingmanKillCount);
            DosStrcat(szTxtWorkspaceBuffer, formatted);
            break;
        case 'D':
            sprintf(formatted, "%03d.%03d",
                    pCurrentCampaignDate->year,
                    pCurrentCampaignDate->day);
            DosStrcat(szTxtWorkspaceBuffer, formatted);
            break;
        case 'E':
            sprintf(formatted, "%03d.%03d",
                    stSavedCampaignDate.year,
                    stSavedCampaignDate.day);
            DosStrcat(szTxtWorkspaceBuffer, formatted);
            break;
        case 'N':
        case 'P':
            DosStrcat(szTxtWorkspaceBuffer,
                      stCampaignState.currentPilot->name);
            break;
        case 'R':
            DosStrcat(szTxtWorkspaceBuffer,
                      apszPilotRankNames[
                          stCampaignState.currentPilot->rank]);
            length = DosStrlen(szTxtWorkspaceBuffer);
            if (szTxtWorkspaceBuffer[length - 1] == '.' &&
                *marker == '.')
                szTxtWorkspaceBuffer[
                    DosStrlen(szTxtWorkspaceBuffer) - 1] = '\0';
            break;
        case 'S':
            DosStrcat(szTxtWorkspaceBuffer,
                      (char *)abSeriesAuxData);
            break;
        case 'T':
            sprintf(formatted, "%02d:%02d",
                    (int)((signed char *)pElapsedCampaignDatePtr)[0],
                    (int)((signed char *)pElapsedCampaignDatePtr)[1]);
            DosStrcat(szTxtWorkspaceBuffer, formatted);
            break;
        case 'W':
            DosStrcat(szTxtWorkspaceBuffer,
                      apWingmanPilotsList[*marker++ - '0']->name);
            break;
        }
        text = marker;
    }
}

/* Function start: 0x4050B0 */
unsigned int LoadFace(short face)
{
    switch (nConversationCharacter) {
    case 0:
        nConversationBackdropFrame = 4;
        break;
    case 1:
        nConversationBackdropFrame = 5;
        break;
    case 2:
        nConversationBackdropFrame = 0;
        init_constellation(0);
        stConstellationViewport = stSceneBuffer;
        stConstellationViewport.bottom = 76;
        InitializeConstellationField(&stConstellationViewport,
                                     -1, 16);
        bConversationConstellation = 1;
        break;
    case 3:
        nConversationBackdropFrame = 1;
        break;
    case 4:
        nConversationBackdropFrame = 2;
        break;
    case 8:
        nConversationBackdropFrame = 1;
        init_constellation(0);
        InitializeConstellationField(&stSceneBuffer, -1, 16);
        bConversationConstellation = 1;
        break;
    case 9:
        nConversationBackdropFrame = 2;
        break;
    case 10:
    case 12:
        nConversationBackdropFrame = 0;
        break;
    case 11:
    case 13:
        nConversationBackdropFrame = 1;
        break;
    default:
        nConversationBackdropFrame = -1;
        break;
    }
    if (face != nTalkingHeadFace) {
        if (pTalkHeadShape != 0)
            FreePacketAndClear(&pTalkHeadShape, 0);
        pTalkHeadShape =
            FetchDiskPacketRetrying(6, face, 0);
    } else if (pTalkHeadShape == 0)
        pTalkHeadShape =
            FetchDiskPacketRetrying(6, face, 0);
    nTalkingHeadFace = face;
    if (pConvOverlayShapePtr == 0)
        pConvOverlayShapePtr =
            FetchDiskPacketRetrying(6, 11, 0);
    nTalkingHeadFaceX =
        aTalkingHeadOrigins[face].faceX;
    nTalkingHeadFaceCoordY =
        aTalkingHeadOrigins[face].faceY;
    nTalkingHeadMouthX =
        aTalkingHeadOrigins[face].mouthX;
    nTalkingHeadMouthY =
        aTalkingHeadOrigins[face].mouthY;
    CloseTalk(pTalkHeadShape, -1, -1);
    return 0;
}

/* Function start: 0x405290 */
unsigned int LongTalk(unsigned char *talker, char *text,
                      short *mouthCommands, short *faceCommands,
                      short duration)
{
    short *faceStart;
    short *mouthStart;
    short faceCountdown;
    short faceFrame;
    short mouthCountdown;
    short mouthFrame;
    short waiting;

    waiting = 0;
    AddPCName(text);
    faceStart = faceCommands;
    mouthStart = mouthCommands;
    ClearViewport(&stCurrentConvTextViewport,
                  cBlackColour);
    faceFrame = 0;
    mouthFrame = 0;
    faceCountdown = 0;
    mouthCountdown = 0;
    FormatTextBufferFromStart("%X%Y%F%s%P",
                              0, 160,
                              nConvTextColorValue,
                              szTxtWorkspaceBuffer);
    nFrameSkipCounter = 1;
    for (;;) {
        if (*mouthCommands == -1 && *faceCommands == -1) {
            if (waiting == 0) {
                CloseTalk(talker, -1, -1);
                DIBslam();
                DIBslamReal();
                WaitForSceneAdvance(duration, 0);
                return 0;
            }
            IsFrameTickElapsed();
            return 0;
        }
        if (mouthCountdown-- == 0) {
            if (*mouthCommands != -1)
                mouthCommands += 2;
            switch (*mouthCommands) {
            case -2:
                mouthCommands = mouthStart;
            default:
                mouthFrame = *mouthCommands;
                mouthCountdown = (short)(mouthCommands[1] * 2);
                break;
            case -1:
                mouthFrame = -1;
                if (waiting == 0) {
                    waiting++;
                    SetFrameTimerPeriodDirect(duration);
                }
                break;
            }
        }
        if (faceCountdown-- == 0) {
            if (*faceCommands != -1)
                faceCommands += 2;
            switch (*faceCommands) {
            case -2:
                faceCommands = faceStart;
            default:
                faceFrame = *faceCommands;
                if (faceFrame == 10)
                    faceFrame = -1;
                faceCountdown = (short)(faceCommands[1] * 2);
                break;
            case -1:
                faceFrame = -1;
                break;
            }
        }
        nFrameSkipCounter--;
        if (nFrameSkipCounter < 1) {
            nFrameSkipCounter = nFrameSkip;
            CloseTalk(talker, mouthFrame, faceFrame);
            DIBslam();
            DIBslamReal();
        }
        if (CheckEscaped() != 0)
            break;
        if (waiting != 0 && (short)IsFrameTickElapsed() != 0)
            return 0;
    }
    do {
    } while (CheckEscaped() != 0);
    return 0;
}

/* Function start: 0x4054B0 */
unsigned int CloseTalk(unsigned char *talker, short mouthFrame,
                       short faceFrame)
{
    if (bConversationConstellation == 1)
        DrawConstellationField();
    switch (nTalkSceneType) {
    case 0:
    case 1:
    case 2:
    case 4:
    case 5:
        if (nConversationBackdropFrame != -1) {
            DrawSpriteDefault(&stSceneBuffer, 0, 0,
                              pCurrentConversationBackdropShapePtr,
                              nConversationBackdropFrame);
            break;
        }
        ClearViewport(&stSceneBuffer, cBlackColour);
        break;
    case 3:
        ClearViewport(&stSceneBuffer, cPrimaryViewBufferColour);
        break;
    default:
        ClearViewport(&stSceneBuffer, cBlackColour);
        break;
    }
    DrawSpriteDefault(&stSceneBuffer, 0, 0, talker, 0);
    if (faceFrame > -1)
        DrawSpriteDefault(&stSceneBuffer,
                          nTalkingHeadFaceX,
                          nTalkingHeadFaceCoordY,
                          talker, (short)(faceFrame + 11));
    if (mouthFrame > -1)
        DrawSpriteDefault(&stSceneBuffer,
                          nTalkingHeadMouthX,
                          nTalkingHeadMouthY,
                          talker, (short)(mouthFrame + 1));
    if (bConversationOverlay != 0)
        DrawSpriteDefault(&stSceneBuffer, 0, 0,
                          pConvOverlayShapePtr,
                          MinShort(nTalkingHeadFace, 1));
    switch (nConversationCharacter) {
    case 5:
        DrawSpriteDefault(&stSceneBuffer, 0, 0,
                          pCurrentConversationSpecialShape, 10);
        RefreshMemoryStatusOverlay();
        return 0;
    case 6:
        DrawSpriteDefault(&stSceneBuffer, 0, 0,
                          pCurrentConversationSpecialShape, 10);
        DrawSpriteDefault(&stSceneBuffer, 0, 0,
                          pCurrentConversationSpecialShape, 11);
        RefreshMemoryStatusOverlay();
        return 0;
    }
    RefreshMemoryStatusOverlay();
    return 0;
}

/* Function start: 0x405660 */
unsigned int Briefing(short series, short mission)
{
    bEscapePressedFlag = 0;
    PreloadMusicTrackHook(0x18);
    PreloadMusicTrackHook(0x19);
    PreloadMusicTrackHook(0x1a);
    LoadMissionData(series, mission);
    LoadBriefingData(series, mission);
    if (bEscapePressedFlag == 0) {
        Build_objective_list();
        LoadBriefingRoom();
    }
    bEscapePressedFlag = 0;
    ReleasePacketHandle(pBriefingPacket);
    ReleaseMusicTrackHook(0x18);
    ReleaseMusicTrackHook(0x19);
    ReleaseMusicTrackHook(0x1a);
    return 0;
}

/* Function start: 0x4056F0 */
unsigned int DeBriefing(short series, short mission)
{
    short fullScore;
    short playerScore;

    bEscapePressedFlag = 0;
    fullScore = (short)FullMissionScore();
    playerScore = (short)PlayersMissionScore();
    if (fullScore == 0) {
        PreloadMusicTrackHook(0x21);
        spacetrack(0x21, 2, 1);
    } else if ((playerScore * 100) / fullScore > 70) {
        PreloadMusicTrackHook(0x21);
        spacetrack(0x21, 2, 1);
    } else {
        PreloadMusicTrackHook(0x22);
        spacetrack(0x22, 2, 1);
    }
    LoadMissionData(series, mission);
    InitializeConversationViewport();
    InitializeConversationText();
    ClearViewport(stCurrentConvTxtCtx.viewport,
                  cBlackColour);
    SetTextContext(&stCurrentConvTxtCtx);
    LoadBriefingData(series, mission);
    pCurrentConversationBackdropShapePtr =
        FetchDiskPacketRetrying(4, 6, 0);
    SceneDirector(1, pDebriefingSceneDataPtr,
                  pDebriefingTxtInfo);
    DIBslam();
    DIBslamReal();
    bEscapePressedFlag = 0;
    ReleasePacketHandle(pCurrentConversationBackdropShapePtr);
    pCurrentConversationBackdropShapePtr = 0;
    ReleasePacketHandle(pBriefingPacket);
    ReleaseTextFont(0);
    ResetScreenClipToFullHeight();
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook(0x21);
    ReleaseMusicTrackHook(0x22);
    return 0;
}

/* Function start: 0x405840 */
unsigned int Office(void)
{
    BriefingPacketHeader *header;
    unsigned char *packet;
    unsigned char *sceneData;
    unsigned char *textData;

    bEscapePressedFlag = 0;
    PreloadMusicTrackHook(0x24);
    spacetrack(0x24, 2, 1);
    InitializeConversationViewport();
    InitializeConversationText();
    packet = FetchDiskPacketRetrying(
        asCampaignBriefingFiles[nCampaignDataSet],
        1, 0);
    header = (BriefingPacketHeader *)packet;
    sceneData = packet + header->briefingScene;
    textData = packet + header->briefingText;
    pCurrentConversationBackdropShapePtr =
        FetchDiskPacketRetrying(4, 7, 0);
    SceneDirector(4, sceneData, textData);
    DIBslam();
    DIBslamReal();
    bEscapePressedFlag = 0;
    ReleasePacketHandle(pCurrentConversationBackdropShapePtr);
    pCurrentConversationBackdropShapePtr = 0;
    ReleasePacketHandle(packet);
    ReleaseTextFont(0);
    ResetScreenClipToFullHeight();
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook(0x24);
    return 0;
}

/* Function start: 0x405910 */
unsigned int LoadBriefingData(short series, short mission)
{
    BriefingPacketHeader *header;

    pBriefingPacket = FetchDiskPacketRetrying(
        asCampaignBriefingFiles[nCampaignDataSet],
        (short)(mission + series * 4), 0);
    header = (BriefingPacketHeader *)pBriefingPacket;
    pBriefingSceneInfo = pBriefingPacket +
        header->briefingScene;
    pCurrentBriefingTextData = pBriefingPacket +
        header->briefingText;
    pDebriefingSceneDataPtr = pBriefingPacket +
        header->debriefingScene;
    pDebriefingTxtInfo = pBriefingPacket +
        header->debriefingText;
    apRecRoomSceneDataList[0] = pBriefingPacket +
        header->recRoomScene0;
    apRecRoomTxtDataArray[0] = pBriefingPacket +
        header->recRoomText0;
    apRecRoomSceneDataList[2] = pBriefingPacket +
        header->recRoomScene2;
    apRecRoomTxtDataArray[2] = pBriefingPacket +
        header->recRoomText2;
    apRecRoomSceneDataList[1] = pBriefingPacket +
        header->recRoomScene1;
    apRecRoomTxtDataArray[1] = pBriefingPacket +
        header->recRoomText1;
    return 0;
}

#pragma pack(push, 1)
typedef struct MissionNavPointDisk {
    char name[30];
    signed char type;
    FixedVector position;
    unsigned short proximityRadius;
    signed char triggers[4][2];
    short preloadObjectTypes[2];
    short missionShips[10];
} MissionNavPointDisk;

typedef struct MissionObjectiveDisk {
    short type;
    short index;
    char description[60];
} MissionObjectiveDisk;

typedef struct MissionShipDisk {
    short type;
    short side;
    signed char leader;
    signed char field_5;
    short missionType;
    signed char navPoint;
    FixedVector position;
    short pitch;
    short yaw;
    short roll;
    signed char formationSpot;
    short speed;
    short rating;
    short pilot;
    short field_2c;
    short field_2e;
    signed char state;
    signed char leaderMissionIndex;
    signed char formationIndex;
    signed char targetMissionIndex;
} MissionShipDisk;
#pragma pack(pop)

/* Function start: 0x4059B0 */
unsigned int LoadMissionData(short series, short mission)
{
    short logicalFile;
    int missionIndex;
    unsigned char *packet;
    MissionHeaderDisk *header;
    MissionNavPointDisk *diskNav;
    int index;
    MissionObjectiveDisk *diskObjective;
    MissionShipRecord *ship;
    MissionShipDisk *diskShip;
    short *initialShip;
    short *sourceInitialShip;
    int item;
    unsigned char *auxiliary;

    logicalFile = asMissionDataFiles[nCampaignDataSet];
    packet = FetchDiskPacketRetrying(logicalFile, 0, 0);
    missionIndex = (int)mission + (int)series * 4;
#ifdef SDL_PORT
    /* The original developer path assumes an occupied one-of-64 header.  An
     * empty header marks its player ship as -1, which cannot be represented
     * as a valid access to the 32 mission records loaded below. */
    if (missionIndex < 0 || missionIndex >= 64) {
        ReleasePacketHandle(packet);
        return 1;
    }
#endif
    header = (MissionHeaderDisk *)(packet + missionIndex * 0x18);
#ifdef SDL_PORT
    if (header->playerMissionShip < 0 ||
        header->playerMissionShip >= ACTIVE_MISSION_SHIP_COUNT) {
        ReleasePacketHandle(packet);
        return 1;
    }
#endif
    stCurrentMissionHeader.entryNavPoint = header->entryNavPoint;
    stCurrentMissionHeader.homeMissionShip = header->homeMissionShip;
    stCurrentMissionHeader.playerMissionShip = header->playerMissionShip;
    sourceInitialShip = header->initialMissionShips;
    initialShip = stCurrentMissionHeader.initialMissionShips;
    while (initialShip < &stCurrentMissionHeader.initialMissionShips[8]) {
        *initialShip++ = *sourceInitialShip++;
    }
    stCurrentMissionHeader.field_16 = header->field_16;
    ReleasePacketHandle(packet);

    packet = FetchDiskPacketRetrying(logicalFile, 1, 0);
    diskNav = (MissionNavPointDisk *)(packet + missionIndex * 0x4d0);
    for (index = 0;
         index < ACTIVE_MISSION_NAV_POINT_COUNT;
         index++, diskNav++) {
        memcpy(aMissionNavPoints[index].name,
               diskNav->name,
               sizeof(aMissionNavPoints[index].name));
        aMissionNavPoints[index].type = diskNav->type;
        aMissionNavPoints[index].position = diskNav->position;
        aMissionNavPoints[index].proximityRadius =
            diskNav->proximityRadius;
        for (item = 0; item < 8; item++) {
            ((signed char *)aMissionNavPoints[index].triggers)[item] =
                ((signed char *)diskNav->triggers)[item];
        }
        for (item = 0; item < 2; item++) {
            aMissionNavPoints[index].preloadObjectTypes[item] =
                (enum ObjectType)diskNav->preloadObjectTypes[item];
        }
        for (item = 0; item < 10; item++) {
            aMissionNavPoints[index].missionShips[item] =
                diskNav->missionShips[item];
        }
    }
    ReleasePacketHandle(packet);

    packet = FetchDiskPacketRetrying(logicalFile, 2, 0);
    diskObjective =
        (MissionObjectiveDisk *)(packet + missionIndex * 0x400);
    for (index = 0; index < MISSION_OBJECTIVE_COUNT;
         index++, diskObjective++) {
        aMissionObjectiveSources[index].type =
            diskObjective->type;
        aMissionObjectiveSources[index].index =
            diskObjective->index;
        for (item = 0; item < 60; item++) {
            aMissionObjectiveSources[index].description[item] =
                diskObjective->description[item];
        }
    }
    ReleasePacketHandle(packet);

    packet = FetchDiskPacketRetrying(logicalFile, 3, 0);
    diskShip = (MissionShipDisk *)(packet + missionIndex * 0x540);
    ship = aMissionShips;
    for (; ship < &aMissionShips[32];
         diskShip++, ship++) {
        ship->type = (enum ObjectType)diskShip->type;
        ship->side = (enum Side)diskShip->side;
        ship->leader = diskShip->leader;
        ship->field_9 = diskShip->field_5;
        ship->missionType = (enum ShipMissionType)diskShip->missionType;
        ship->navPoint = diskShip->navPoint;
        ship->position = diskShip->position;
        memcpy(&ship->pitch, &diskShip->pitch, 6);
        ship->formationSpot = diskShip->formationSpot;
        ship->speed = diskShip->speed;
        ship->rating = diskShip->rating;
        ship->behaviour.pilot = diskShip->pilot;
        ship->field_2c = diskShip->field_2c;
        ship->field_2e = diskShip->field_2e;
        ship->state = diskShip->state;
        ship->leaderMissionIndex = diskShip->leaderMissionIndex;
        ship->formationIndex = diskShip->formationIndex;
        ship->targetMissionIndex = diskShip->targetMissionIndex;
    }
    ReleasePacketHandle(packet);

    packet = FetchDiskPacketRetrying(logicalFile, 4, 0);
    auxiliary = packet + missionIndex * sizeof(abMissionAuxData);
    DosMemcpy(abMissionAuxData,
              auxiliary,
              sizeof(abMissionAuxData));
    ReleasePacketHandle(packet);

    packet = FetchDiskPacketRetrying(logicalFile, 5, 0);
    auxiliary = packet + series * sizeof(abSeriesAuxData);
    DosMemcpy(abSeriesAuxData,
              auxiliary,
              sizeof(abSeriesAuxData));
    ReleasePacketHandle(packet);
    return 0;
}

/* Function start: 0x405CC0 */
unsigned int UpdateMap(char *text, short duration)
{
    Viewport savedScreen;
    Viewport savedVirtualScreen;

    savedScreen = stScreenView;
    savedVirtualScreen = stSceneBuffer;
    ClearViewport(&stScreenView, cBlackColour);
    if (stSceneBuffer.pixels != 0)
        ClearViewport(&stSceneBuffer, cBlackColour);
    AddPCName(text);
    ClearViewport(&stCurrentConvTextViewport,
                  cBlackColour);
    FormatTextBufferFromStart("%X%Y%F%s%P",
                              0, 160,
                              nConvTextColorValue,
                              szTxtWorkspaceBuffer);
    stScreenView = savedScreen;
    stSceneBuffer = savedVirtualScreen;
    BriefingMap_DisplayMap();
    WaitForSceneAdvance(duration, 0);
    ClearViewport(&stScreenView, cBlackColour);
    SetTextContext(&stCurrentConvTxtCtx);
    ClearViewport(&stScreenView, cBlackColour);
    return 0;
}

/* Function start: 0x405DE0 */
unsigned int CloseLook(unsigned char *shape, short shot,
                       short *animation, char *text, short duration,
                       int unused)
{
    short character;
    short escaped;
    short finished;
    short frame;
    short frameDelay;
    short *start;

    frameDelay = 0;
    finished = 0;
    AddPCName(text);
    ClearViewport(&stCurrentConvTextViewport,
                  cBlackColour);
    FormatTextBufferFromStart("%X%Y%F%s%P",
                              0, 160,
                              nConvTextColorValue,
                              szTxtWorkspaceBuffer);
    start = animation;
    nFrameSkipCounter = 1;
    if (shot == 2 || shot == 11) {
        if (*animation != -1) {
            do {
                if (frameDelay-- == 0) {
                    if (*animation != -1)
                        animation += 2;
                    switch (*animation) {
                    case -2:
                        animation = start;
                        /* Restart also loads the first frame and its delay. */
                    default:
                        frame = *animation;
                        frameDelay = (short)(animation[1] * 2);
                        break;
                    case -1:
                        frame = -1;
                        if (finished == 0) {
                            finished++;
                            SetFrameTimerPeriodDirect(duration);
                        }
                        break;
                    }
                }
                nFrameSkipCounter--;
                if (nFrameSkipCounter < 1) {
                    nFrameSkipCounter = nFrameSkip;
                    if (shot == 11) {
                        DrawDebriefingLongShot();
                        if (frame > -1)
                            DrawSpriteDefault(
                                &stSceneBuffer,
                                nDebriefingPodiumX, 53,
                                pCurrentConversationBackdropShapePtr,
                                (short)(frame + 17));
                    } else if (frame > -1) {
                        DrawSpriteDefault(&stSceneBuffer, 225, 34,
                                          shape, frame);
                    }
                    RefreshMemoryStatusOverlay();
                    DIBslam();
                    DIBslamReal();
                }
                escaped = CheckEscaped();
                if (escaped != 0) {
                    do {
                        escaped = CheckEscaped();
                    } while (escaped != 0);
                    return 0;
                }
                if (finished != 0 && (short)IsFrameTickElapsed() != 0)
                    return 0;
                DIBslam();
                DIBslamReal();
            } while (*animation != -1);
        }
    } else if (shot == 0) {
        short sceneFrame;

        for (sceneFrame = 0; sceneFrame < 22; sceneFrame++) {
            nFrameSkipCounter--;
            if (nFrameSkipCounter < 1) {
                nFrameSkipCounter = nFrameSkip;
                DrawSpriteDefault(
                    &stSceneBuffer, 0, 0,
                    pCurrentConversationBackdropShapePtr, 0);
                DrawSpriteDefault(&stSceneBuffer, 241, 60,
                                  pCurrentBriefingAnimShape,
                                  sceneFrame);
                DrawSpriteDefault(&stSceneBuffer, 241, 64,
                                  pCurrentBriefingAnimShape, 22);
                for (character = 0; character < 14; character++) {
                    DrawBriefingCharacter(
                        character, 0,
                        aBriefingCharacters[character]
                            .animation[sceneFrame],
                        0, 0);
                }
                RefreshMemoryStatusOverlay();
                DIBslam();
                DIBslamReal();
            }
            if (CheckEscaped() != 0)
                sceneFrame = 20;
            if (sceneFrame == 20)
                nFrameSkipCounter = 1;
            DIBslam();
            DIBslamReal();
        }
    }
    DIBslam();
    DIBslamReal();
    WaitForSceneAdvance(duration, unused);
    return 0;
}
