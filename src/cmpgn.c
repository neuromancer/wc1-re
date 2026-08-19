/*
 *  Campaign mission packet decoding (`cmpgn` in the Mac segment names).
 *
 *  Address range 0x404610-0x40609f (provisional -- see docs/ORDER.md).
 *  Boundary evidence: auto.c ends before 0x00404610 and brains.c begins at
 *  0x004060A0; LoadMissionData is the recovered function at 0x004059B0.
 */
#include "wc1.h"

#pragma pack(push, 1)
typedef struct MissionHeaderDisk {
    short entryNavPoint;
    short homeMissionShip;
    short playerMissionShip;
    short initialMissionShips[8];
    short field_16;
} MissionHeaderDisk;

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

typedef char MissionHeaderDisk_size_must_be_0x18[
    sizeof(MissionHeaderDisk) == 0x18 ? 1 : -1];
typedef char MissionNavPointDisk_size_must_be_0x4d[
    sizeof(MissionNavPointDisk) == 0x4d ? 1 : -1];
typedef char MissionObjectiveDisk_size_must_be_0x40[
    sizeof(MissionObjectiveDisk) == 0x40 ? 1 : -1];
typedef char MissionShipDisk_size_must_be_0x2a[
    sizeof(MissionShipDisk) == 0x2a ? 1 : -1];

/* Function start: 0x404610 */
unsigned short __stdcall LoadPaletteTripletsFile(const char *path)
{
    unsigned char *palette;
    FILE *file;
#ifdef WC1_SDL
    char resolvedPath[PATH_MAX];
#endif

    palette = AllocateTaggedMemory(0x300, 0);
    if (palette == 0)
        return 0;
#ifdef WC1_SDL
    if (Wc1SdlResolvePath(path, resolvedPath, sizeof(resolvedPath)))
        file = fopen(resolvedPath, "rb");
    else
        file = 0;
#else
    file = fopen(path, "rb");
#endif
    if (file != 0) {
        fseek(file, 0x30, SEEK_SET);
        fread(palette, 0x300, 1, file);
        SetWholePaletteFromTriplets(palette);
        fclose(file);
        ReleasePacketHandle(palette);
        return 1;
    }
    ReleasePacketHandle(palette);
    return 0;
}

/* Function start: 0x4046A0 */
unsigned int ejection_sequence(void)
{
    FixedVector viewOffset;
    unsigned char *background;
    unsigned char *ejectionShape;
    short frame;
    short y;
    short descentSpeed;
    short spriteFrame;

    free_all_slots();
    free_cockpit();
    PreloadMusicTrackHook(0x1f);
    spacetrack(0x1f, 2, 1);
    frame = 0;
    new_view(9, 0);
    background = FetchDiskPacketRetrying(
        (short)cCockpitLogicalFile, 3, 0);
    ejectionShape = FetchDiskPacketRetrying(2, 1, 0);
    PlaySfxWaveFileByNumber(0x21, -1, 0);
    y = 199;
    bEscapePressed = 0;
    descentSpeed = 4;
    nFrameSkipCounter = 1;
    do {
        if (RefreshCockpitStatus() != 0) {
            DrawSpriteDefault(&stSpaceBuffer, 0, 0, background, 0);
            spriteFrame = MinShort(frame, 4);
            DrawSpriteDefault(
                &stSpaceBuffer, 160, y, ejectionShape,
                asEjectionPrimaryFrames[spriteFrame]);
            if (asEjectionSecondaryFrames[spriteFrame] != -1)
                DrawSpriteDefault(
                    &stSpaceBuffer, 160, y, ejectionShape,
                    asEjectionSecondaryFrames[spriteFrame]);
            DrawSpriteDefault(&stSpaceBuffer, 160, (short)(y + 1),
                              ejectionShape, 5);
            dump_buffer_to_screen();
        }
        if (frame > 1) {
            y = (short)(y - descentSpeed);
            descentSpeed = MinShort((short)(descentSpeed + 4), 20);
        }
        if (bEscapePressed == 1)
            break;
        frame++;
        DIBslam();
        DIBslamReal();
    } while (frame < 10);

    ReleasePacketHandle(ejectionShape);
    ReleasePacketHandle(background);
    GetScreenUpdateFlag();
    if (bEscapePressed != 1) {
        PromptInsertNumberedDisk(8);
        pScreenViewportPacket =
            AllocateTaggedMemory(
                GetPacketSize(pDiskFileRecords[8].name, 8),
                0x40);
        if (pScreenViewportPacket == 0)
            ReportOutOfMemoryAndExit(szViewTemplates);
        else
            LoadPacketIntoBuffer(8, 8,
                                 pScreenViewportPacket);

        aObjectTypeData[OBJECT_TYPE_EJECTED_PILOT].shapeSet =
            FetchDiskPacketRetrying(2, 2, 0);
        nEjectedPilotObject = find_vacant_3d_object();
        set_objects_data(nEjectedPilotObject,
                         OBJECT_TYPE_EJECTED_PILOT, -1);
        asObjectCounter[nEjectedPilotObject] =
            32000;
        copy_frame(0, nEjectedPilotObject);
        aShipPosition[nEjectedPilotObject] =
            aShipPosition[0];
        ScaleFixedVector(
            &aShipUpVector[nEjectedPilotObject],
            -0x500,
            &aShipVelocity[nEjectedPilotObject]);
        AddFixedVectors(
            &aShipVelocity[nEjectedPilotObject],
            &aShipVelocity[0],
            &aShipVelocity[nEjectedPilotObject]);
        new_view(10, nEjectedPilotObject);

        background = FetchDiskPacketRetrying(
            (short)cCockpitLogicalFile, 0, 0);
        ejectionShape = FetchDiskPacketRetrying(
            (short)cCockpitLogicalFile, 5, 0);
        y = 40;
        frame = 0;
        PlaySfxWaveFileByNumber(0x22, -1, 0);
        nFrameSkipCounter = 1;
        do {
            if (RefreshCockpitStatus() != 0) {
                DrawSpriteDefault(&stSpaceBuffer, 0, y,
                                  background, 0);
                DrawSpriteDefault(&stSpaceBuffer, 0, (short)(y - 1),
                                  ejectionShape, 0);
                dump_buffer_to_screen();
            }
            if (bEscapePressed == 1)
                break;
            y = (short)(y + descentSpeed);
            DIBslam();
            frame++;
            DIBslamReal();
        } while (frame < 10);

        ReleasePacketHandle(ejectionShape);
        ReleasePacketHandle(background);
        if (bEscapePressed != 1) {
            load_all_slots();
            aShipForwardVector[WC1_EYE_OBJECT] =
                aShipUpVector[0];
            aShipRightVector[WC1_EYE_OBJECT] =
                aShipRightVector[0];
            aShipUpVector[WC1_EYE_OBJECT] =
                aShipForwardVector[0];
            negate_vector(&aShipUpVector[WC1_EYE_OBJECT]);
            ScaleFixedVector(
                &aShipUpVector[nEjectedPilotObject],
                -0x25800, &viewOffset);
            AddFixedVectors(
                &aShipPosition[nEjectedPilotObject],
                &viewOffset,
                &aShipPosition[WC1_EYE_OBJECT]);
            nScriptedViewObject =
                nEjectedPilotObject;
            initialize_scripted_view(asEjectionViewScript);
            frame = 0;
            nFrameSkipCounter = 1;
            SetMusBreakpt(0, 0);
            while (1) {
                alter_pitch(4, nEjectedPilotObject);
                if (RefreshCockpitStatus() != 0)
                    dump_buffer_to_screen();
                if (frame == 10) {
                    Explosion(0);
                    PlaySfxWaveFileByNumber(4, -1, 0);
                }
                frame++;
                if (frame > 200 || bEscapePressed != 0)
                    break;
                DIBslam();
                DIBslamReal();
            }
        }
    }

    bEscapePressed = 0;
    bScriptedView = 0;
    if (pScreenViewportPacket != 0) {
        ReleasePacketHandle(pScreenViewportPacket);
        pScreenViewportPacket = 0;
    }
    FadeViewportPaletteToColour(&stScreen, cBlackColour, 1);
    ClearViewport(&stScreen, cBlackColour);
    DIBslam();
    DIBslamReal();
    RestoreGamePalette();
    free_all_slots();
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook(0x1f);
    return 0;
}

/* Function start: 0x404BE0 */
void stranded_sequence(void)
{
    short frame;

    nCannedSceneMode = 1;
    free_cockpit();
    force_view(13, 0);
    pIntroFont = FetchDiskPacketRetrying(9, 1, 0);
    frame = 0;
    do {
        if (RefreshCockpitStatus() != 0) {
            if (frame >= 300)
                print_subtitle(&stSpaceBuffer, 56,
                               szStrandedTheEnd);
            else if (frame >= 160)
                print_subtitle(&stSpaceBuffer, 50,
                               pStrandedMessage);
            dump_buffer_to_screen();
        }
        if (bEscapePressed != 0)
            break;
        frame++;
        DIBslam();
        DIBslamReal();
    } while (frame < 400);
    free_all_slots();
    FreePacketAndClear(&pIntroFont, 0);
    stScreen.top = 0;
    stScreen.bottom = 199;
    FadeViewportPaletteToColour(&stScreen, cBlackColour, 1);
    ClearViewport(&stScreen, cBlackColour);
    RestoreGamePalette();
    bEscapePressed = 0;
}

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

    szTextScratchBuffer[0] = '\0';
    for (;;) {
        marker = DosStrchr(text, '$');
        if (marker == 0) {
            DosStrcat(szTextScratchBuffer, text);
            return szTextScratchBuffer;
        }
        output = DosStrchr(szTextScratchBuffer, 0);
        while (text != marker) {
            *output++ = *text++;
        }
        *output = '\0';
        marker++;
        switch (*marker++) {
        case 'A':
            DosStrcat(szTextScratchBuffer,
                      apszMedalNames[
                          nConversationMedalIndex]);
            break;
        case 'C':
            DosStrcat(szTextScratchBuffer,
                      stCampaignState.currentPilot->callsign);
            break;
        case 'D':
            sprintf(formatted, szCampaignDateFormat,
                    pCurrentCampaignDate->year,
                    pCurrentCampaignDate->day);
            DosStrcat(szTextScratchBuffer, formatted);
            break;
        case 'E':
            sprintf(formatted, szSavedCampaignDateFormat,
                    stSavedCampaignDate.year,
                    stSavedCampaignDate.day);
            DosStrcat(szTextScratchBuffer, formatted);
            break;
        case 'K':
            sprintf(formatted, szConversationIntegerFormat,
                    nPlayerKillCount);
            DosStrcat(szTextScratchBuffer, formatted);
            break;
        case 'L':
            sprintf(formatted,
                    szConversationIntegerFormatAlt,
                    nWingmanKillCount);
            DosStrcat(szTextScratchBuffer, formatted);
            break;
        case 'N':
        case 'P':
            DosStrcat(szTextScratchBuffer,
                      stCampaignState.currentPilot->name);
            break;
        case 'R':
            DosStrcat(szTextScratchBuffer,
                      apszPilotRankNames[
                          stCampaignState.currentPilot->rank]);
            length = DosStrlen(szTextScratchBuffer);
            if (szTextScratchBuffer[length - 1] == '.' &&
                *marker == '.')
                szTextScratchBuffer[
                    DosStrlen(szTextScratchBuffer) - 1] = '\0';
            break;
        case 'S':
            DosStrcat(szTextScratchBuffer,
                      (char *)abSeriesAuxData);
            break;
        case 'T':
            sprintf(formatted, szCampaignTimeFormat,
                    (int)((signed char *)pElapsedCampaignDate)[0],
                    (int)((signed char *)pElapsedCampaignDate)[1]);
            DosStrcat(szTextScratchBuffer, formatted);
            break;
        case 'W':
            DosStrcat(szTextScratchBuffer,
                      apWingmanPilots[*marker++ - '0']->name);
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
    case 3:
    case 11:
    case 13:
        nConversationBackdropFrame = 1;
        break;
    default:
        nConversationBackdropFrame = -1;
        break;
    }
    if (face != nTalkingHeadFace &&
        pTalkingHeadShape != 0)
        FreePacketAndClear(&pTalkingHeadShape, 0);
    if (pTalkingHeadShape == 0)
        pTalkingHeadShape =
            FetchDiskPacketRetrying(6, face, 0);
    nTalkingHeadFace = face;
    if (pConversationOverlayShape == 0)
        pConversationOverlayShape =
            FetchDiskPacketRetrying(6, 11, 0);
    nTalkingHeadFaceX =
        aTalkingHeadOrigins[face].faceX;
    nTalkingHeadFaceY =
        aTalkingHeadOrigins[face].faceY;
    nTalkingHeadMouthX =
        aTalkingHeadOrigins[face].mouthX;
    nTalkingHeadMouthY =
        aTalkingHeadOrigins[face].mouthY;
    CloseTalk(pTalkingHeadShape, -1, -1);
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
    faceFrame = 0;
    ClearViewport(&stConversationTextViewport,
                  cBlackColour);
    mouthFrame = 0;
    faceCountdown = 0;
    mouthCountdown = 0;
    FormatTextBufferFromStart(szConversationTextFormat,
                              0, 160,
                              nConversationTextColour,
                              szTextScratchBuffer);
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
    switch (nConversationSceneType) {
    case 0:
    case 1:
    case 2:
    case 4:
    case 5:
        if (nConversationBackdropFrame != -1) {
            DrawSpriteDefault(&stSceneBuffer, 0, 0,
                              pConversationBackdropShape,
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
                          nTalkingHeadFaceY,
                          talker, (short)(faceFrame + 11));
    if (mouthFrame > -1)
        DrawSpriteDefault(&stSceneBuffer,
                          nTalkingHeadMouthX,
                          nTalkingHeadMouthY,
                          talker, (short)(mouthFrame + 1));
    if (bConversationOverlay != 0)
        DrawSpriteDefault(&stSceneBuffer, 0, 0,
                          pConversationOverlayShape,
                          MinShort(nTalkingHeadFace, 1));
    switch (nConversationCharacter) {
    case 5:
        DrawSpriteDefault(&stSceneBuffer, 0, 0,
                          pConversationSpecialShape, 10);
        RefreshMemoryStatusOverlay();
        return 0;
    case 6:
        DrawSpriteDefault(&stSceneBuffer, 0, 0,
                          pConversationSpecialShape, 10);
        DrawSpriteDefault(&stSceneBuffer, 0, 0,
                          pConversationSpecialShape, 11);
        RefreshMemoryStatusOverlay();
        return 0;
    }
    RefreshMemoryStatusOverlay();
    return 0;
}

/* Function start: 0x405660 */
unsigned int Briefing(short series, short mission)
{
    bEscapePressed = 0;
    PreloadMusicTrackHook(0x18);
    PreloadMusicTrackHook(0x19);
    PreloadMusicTrackHook(0x1a);
    LoadMissionData(series, mission);
    LoadBriefingData(series, mission);
    if (bEscapePressed == 0) {
        Build_objective_list();
        LoadBriefingRoom();
    }
    bEscapePressed = 0;
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

    bEscapePressed = 0;
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
    ClearViewport(stConversationTextContext.viewport,
                  cBlackColour);
    SetTextContext(&stConversationTextContext);
    LoadBriefingData(series, mission);
    pConversationBackdropShape =
        FetchDiskPacketRetrying(4, 6, 0);
    SceneDirector(1, pDebriefingSceneData,
                  pDebriefingTextData);
    DIBslam();
    DIBslamReal();
    bEscapePressed = 0;
    ReleasePacketHandle(pConversationBackdropShape);
    pConversationBackdropShape = 0;
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

    bEscapePressed = 0;
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
    pConversationBackdropShape =
        FetchDiskPacketRetrying(4, 7, 0);
    SceneDirector(4, sceneData, textData);
    DIBslam();
    DIBslamReal();
    bEscapePressed = 0;
    ReleasePacketHandle(pConversationBackdropShape);
    pConversationBackdropShape = 0;
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
    pBriefingSceneData = pBriefingPacket +
        header->briefingScene;
    pBriefingTextData = pBriefingPacket +
        header->briefingText;
    pDebriefingSceneData = pBriefingPacket +
        header->debriefingScene;
    pDebriefingTextData = pBriefingPacket +
        header->debriefingText;
    apRecRoomSceneData[0] = pBriefingPacket +
        header->recRoomScene0;
    apRecRoomTextData[0] = pBriefingPacket +
        header->recRoomText0;
    apRecRoomSceneData[2] = pBriefingPacket +
        header->recRoomScene2;
    apRecRoomTextData[2] = pBriefingPacket +
        header->recRoomText2;
    apRecRoomSceneData[1] = pBriefingPacket +
        header->recRoomScene1;
    apRecRoomTextData[1] = pBriefingPacket +
        header->recRoomText1;
    return 0;
}

/* Function start: 0x4059B0 */
unsigned int LoadMissionData(short series, short mission)
{
    short logicalFile;
    int missionIndex;
    unsigned char *packet;
    MissionHeaderDisk *header;
    MissionNavPointDisk *diskNav;
    MissionObjectiveDisk *diskObjective;
    MissionShipDisk *diskShip;
    MissionShipRecord *ship;
    short *sourceInitialShip;
    short *initialShip;
    int index;
    int item;

    logicalFile = asMissionDataFiles[nCampaignDataSet];
    packet = FetchDiskPacketRetrying(logicalFile, 0, 0);
    missionIndex = (int)mission + (int)series * 4;
#ifdef WC1_SDL
    /* The original developer path assumes an occupied one-of-64 header.  An
     * empty header marks its player ship as -1, which cannot be represented
     * as a valid access to the 32 mission records loaded below. */
    if (missionIndex < 0 || missionIndex >= 64) {
        ReleasePacketHandle(packet);
        return 1;
    }
#endif
    header = (MissionHeaderDisk *)(packet + missionIndex * 0x18);
#ifdef WC1_SDL
    if (header->playerMissionShip < 0 ||
        header->playerMissionShip >= WC1_ACTIVE_MISSION_SHIP_COUNT) {
        ReleasePacketHandle(packet);
        return 1;
    }
#endif
    nMissionEntryNavPoint = header->entryNavPoint;
    nHomeMissionShipIndex = header->homeMissionShip;
    nPlayerMissionShipIndex = header->playerMissionShip;
    sourceInitialShip = header->initialMissionShips;
    initialShip = nInitialMissionShipIndices;
    for (; initialShip < &nInitialMissionShipIndices[8];
         initialShip++, sourceInitialShip++) {
        *initialShip = *sourceInitialShip;
    }
    DAT_005a86a6 = header->field_16;
    ReleasePacketHandle(packet);

    packet = FetchDiskPacketRetrying(logicalFile, 1, 0);
    diskNav = (MissionNavPointDisk *)(packet + missionIndex * 0x4d0);
    for (index = 0;
         index < WC1_ACTIVE_MISSION_NAV_POINT_COUNT;
         diskNav++, index++) {
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
    for (index = 0; index < WC1_MISSION_OBJECTIVE_COUNT;
         diskObjective++, index++) {
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
    DosMemcpy(abMissionAuxData,
              packet + missionIndex * sizeof(abMissionAuxData),
              sizeof(abMissionAuxData));
    ReleasePacketHandle(packet);

    packet = FetchDiskPacketRetrying(logicalFile, 5, 0);
    DosMemcpy(abSeriesAuxData,
              packet + series * sizeof(abSeriesAuxData),
              sizeof(abSeriesAuxData));
    ReleasePacketHandle(packet);
    return 0;
}

/* Function start: 0x405CC0 */
unsigned int UpdateMap(char *text, short duration)
{
    Viewport savedScreen;
    Viewport savedVirtualScreen;

    savedScreen = stScreen;
    savedVirtualScreen = stSceneBuffer;
    ClearViewport(&stScreen, cBlackColour);
    if (stSceneBuffer.pixels != 0)
        ClearViewport(&stSceneBuffer, cBlackColour);
    AddPCName(text);
    ClearViewport(&stConversationTextViewport,
                  cBlackColour);
    FormatTextBufferFromStart(szBriefingMapTextFormat,
                              0, 160,
                              nConversationTextColour,
                              szTextScratchBuffer);
    stScreen = savedScreen;
    stSceneBuffer = savedVirtualScreen;
    BriefingMap_DisplayMap();
    WaitForSceneAdvance(duration, 0);
    ClearViewport(&stScreen, cBlackColour);
    SetTextContext(&stConversationTextContext);
    ClearViewport(&stScreen, cBlackColour);
    return 0;
}

/* Function start: 0x405DE0 */
unsigned int CloseLook(unsigned char *shape, short shot,
                       short *animation, char *text, short duration,
                       short unused)
{
    short character;
    short countdown;
    short escaped;
    short finished;
    short frame;
    short sceneFrame;
    short *cursor;
    short *start;

    finished = 0;
    sceneFrame = 0;
    AddPCName(text);
    ClearViewport(&stConversationTextViewport,
                  cBlackColour);
    FormatTextBufferFromStart(szCloseLookTextFormat,
                              0, 160,
                              nConversationTextColour,
                              szTextScratchBuffer);
    nFrameSkipCounter = 1;
    cursor = animation;
    start = animation;
    if (shot == 2 || shot == 11) {
        if (*cursor != -1) {
            countdown = 0;
            do {
                if (countdown-- == 0) {
                    if (*cursor != -1)
                        cursor += 2;
                    if (*cursor == -2)
                        cursor = start;
                    else if (*cursor == -1) {
                        frame = -1;
                        if (finished == 0) {
                            finished = 1;
                            SetFrameTimerPeriodDirect(duration);
                        }
                    } else {
                        frame = *cursor;
                        countdown = (short)(cursor[1] * 2);
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
                                pConversationBackdropShape,
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
                if (finished != 0 && IsFrameTickElapsed() != 0)
                    return 0;
                DIBslam();
                DIBslamReal();
            } while (*cursor != -1);
        }
    } else if (shot == 0) {
        for (; sceneFrame < 22; sceneFrame++) {
            nFrameSkipCounter--;
            if (nFrameSkipCounter < 1) {
                nFrameSkipCounter = nFrameSkip;
                DrawSpriteDefault(
                    &stSceneBuffer, 0, 0,
                    pConversationBackdropShape, 0);
                DrawSpriteDefault(&stSceneBuffer, 241, 60,
                                  pBriefingAnimationShape,
                                  sceneFrame);
                DrawSpriteDefault(&stSceneBuffer, 241, 64,
                                  pBriefingAnimationShape, 22);
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
