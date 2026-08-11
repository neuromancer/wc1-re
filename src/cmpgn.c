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
    frame = 0;
    StartMusicTrack(0x1f, 2, 1);
    new_view(9, 0);
    background = FetchDiskPacketRetrying(
        (short)g_cCockpitLogicalFile_005a7c74, 3, 0);
    ejectionShape = FetchDiskPacketRetrying(2, 1, 0);
    y = 199;
    PlaySfxWaveFileByNumber(0x21, -1, 0);
    DAT_0059ab58 = 0;
    descentSpeed = 4;
    DAT_00469fb4 = 1;
    do {
        if (RefreshCockpitStatus() != 0) {
            DrawSpriteDefault(&DAT_005a7510, 0, 0, background, 0);
            spriteFrame = MinShort(frame, 4);
            DrawSpriteDefault(
                &DAT_005a7510, 160, y, ejectionShape,
                g_asEjectionPrimaryFrames_00465550[spriteFrame]);
            if (g_asEjectionSecondaryFrames_00465560[spriteFrame] != -1)
                DrawSpriteDefault(
                    &DAT_005a7510, 160, y, ejectionShape,
                    g_asEjectionSecondaryFrames_00465560[spriteFrame]);
            DrawSpriteDefault(&DAT_005a7510, 160, (short)(y + 1),
                              ejectionShape, 5);
            dump_buffer_to_screen();
        }
        if (frame > 1) {
            y = (short)(y - descentSpeed);
            descentSpeed = MinShort((short)(descentSpeed + 4), 20);
        }
        if (DAT_0059ab58 == 1)
            break;
        frame++;
        DIBslam();
        DIBslamReal();
    } while (frame < 10);

    ReleasePacketHandle((int)ejectionShape);
    ReleasePacketHandle((int)background);
    GetScreenUpdateFlag();
    if (DAT_0059ab58 != 1) {
        PromptInsertNumberedDisk(8);
        g_pScreenViewportPacket_005a6b94 =
            (unsigned char *)AllocateTaggedMemory(
                GetPacketSize((char *)(DAT_005a7cf0 + 0x80), 8), 0x40);
        if (g_pScreenViewportPacket_005a6b94 == 0)
            ReportOutOfMemoryAndExit(g_szViewTemplates_004655d4);
        else
            LoadPacketIntoBuffer(8, 8,
                                 g_pScreenViewportPacket_005a6b94);

        g_aObjectTypeData_00466458[OBJECT_TYPE_EJECTED_PILOT].shapeSet =
            FetchDiskPacketRetrying(2, 2, 0);
        g_nEjectedPilotObject_0046c044 = find_vacant_3d_object();
        set_objects_data(g_nEjectedPilotObject_0046c044,
                         OBJECT_TYPE_EJECTED_PILOT, -1);
        g_asObjectCounter_0059c330[g_nEjectedPilotObject_0046c044] =
            32000;
        copy_frame(0, g_nEjectedPilotObject_0046c044);
        g_aShipPosition_0059c490[g_nEjectedPilotObject_0046c044] =
            g_aShipPosition_0059c490[0];
        ScaleFixedVector(
            &g_aShipUpVector_0059b9e0[g_nEjectedPilotObject_0046c044],
            -0x500,
            &g_aShipVelocity_0059c010[g_nEjectedPilotObject_0046c044]);
        AddFixedVectors(
            &g_aShipVelocity_0059c010[g_nEjectedPilotObject_0046c044],
            &g_aShipVelocity_0059c010[0],
            &g_aShipVelocity_0059c010[g_nEjectedPilotObject_0046c044]);
        new_view(10, g_nEjectedPilotObject_0046c044);

        background = FetchDiskPacketRetrying(
            (short)g_cCockpitLogicalFile_005a7c74, 0, 0);
        ejectionShape = FetchDiskPacketRetrying(
            (short)g_cCockpitLogicalFile_005a7c74, 5, 0);
        y = 40;
        frame = 0;
        PlaySfxWaveFileByNumber(0x22, -1, 0);
        DAT_00469fb4 = 1;
        do {
            if (RefreshCockpitStatus() != 0) {
                DrawSpriteDefault(&DAT_005a7510, 0, y,
                                  background, 0);
                DrawSpriteDefault(&DAT_005a7510, 0, (short)(y - 1),
                                  ejectionShape, 0);
                dump_buffer_to_screen();
            }
            if (DAT_0059ab58 == 1)
                break;
            y = (short)(y + descentSpeed);
            DIBslam();
            frame++;
            DIBslamReal();
        } while (frame < 10);

        ReleasePacketHandle((int)ejectionShape);
        ReleasePacketHandle((int)background);
        if (DAT_0059ab58 != 1) {
            load_all_slots();
            g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] =
                g_aShipUpVector_0059b9e0[0];
            g_aShipRightVector_0059b6e0[WC1_EYE_OBJECT] =
                g_aShipRightVector_0059b6e0[0];
            g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT] =
                g_aShipForwardVector_0059bce0[0];
            negate_vector(&g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT]);
            ScaleFixedVector(
                &g_aShipUpVector_0059b9e0[g_nEjectedPilotObject_0046c044],
                -0x25800, &viewOffset);
            AddFixedVectors(
                &g_aShipPosition_0059c490[g_nEjectedPilotObject_0046c044],
                &viewOffset,
                &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
            g_nScriptedViewObject_0046a8d0 =
                g_nEjectedPilotObject_0046c044;
            initialize_scripted_view(g_asEjectionViewScript_00465570);
            frame = 0;
            DAT_00469fb4 = 1;
            SetMusBreakpt();
            while (1) {
                alter_pitch(4, g_nEjectedPilotObject_0046c044);
                if (RefreshCockpitStatus() != 0)
                    dump_buffer_to_screen();
                if (frame == 10) {
                    Explosion(0);
                    PlaySfxWaveFileByNumber(4, -1, 0);
                }
                frame++;
                if (frame > 200 || DAT_0059ab58 != 0)
                    break;
                DIBslam();
                DIBslamReal();
            }
        }
    }

    DAT_0059ab58 = 0;
    g_bScriptedView_0046a8d4 = 0;
    if (g_pScreenViewportPacket_005a6b94 != 0) {
        ReleasePacketHandle((int)g_pScreenViewportPacket_005a6b94);
        g_pScreenViewportPacket_005a6b94 = 0;
    }
    FadeViewportPaletteToColour(&DAT_005a6ba0, DAT_0046999c, 1);
    ClearViewport(&DAT_005a6ba0, DAT_0046999c);
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

    g_nCannedSceneMode_00469fac = 1;
    free_cockpit();
    force_view(13, 0);
    frame = 0;
    g_pIntroFont_005a8960 = FetchDiskPacketRetrying(9, 1, 0);
    do {
        if (RefreshCockpitStatus() != 0) {
            if (frame >= 300)
                print_subtitle(&DAT_005a7510, 56,
                               g_szStrandedTheEnd_004655e4);
            else if (frame >= 160)
                print_subtitle(&DAT_005a7510, 50,
                               g_pStrandedMessage_00465588);
            dump_buffer_to_screen();
        }
        if (DAT_0059ab58 != 0)
            break;
        frame++;
        DIBslam();
        DIBslamReal();
    } while (frame < 400);
    free_all_slots();
    FreePacketAndClear((int *)&g_pIntroFont_005a8960, 0);
    DAT_005a6ba0.top = 0;
    DAT_005a6ba0.bottom = 199;
    FadeViewportPaletteToColour(&DAT_005a6ba0, DAT_0046999c, 1);
    ClearViewport(&DAT_005a6ba0, DAT_0046999c);
    RestoreGamePalette();
    DAT_0059ab58 = 0;
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
            commands += 2;
            sequenceIndex++;
            *durationCursor = '\0';
            commands[-1] = (short)atoi(duration);
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

    character = *text;
    for (;;) {
        if (character == '\0') {
            *commands = -1;
            return 0;
        }
        text++;
        if (character == '$') {
            *commands = 9;
        } else if (character <= 'z') {
            if (character < 'a') {
                character = *text;
                continue;
            }
            *commands = g_asMouthFramesByPhoneme_004655f0[
                character - 'a'];
        } else {
            character = *text;
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
        character = *text;
    }
}

/* Function start: 0x404E10 */
char *AddPCName(char *text)
{
    char formatted[12];
    char *marker;
    char *output;
    short length;

    g_szTextScratchBuffer_00598b00[0] = '\0';
    for (;;) {
        marker = DosStrchr(text, '$');
        if (marker == 0) {
            DosStrcat(g_szTextScratchBuffer_00598b00, text);
            return g_szTextScratchBuffer_00598b00;
        }
        output = DosStrchr(g_szTextScratchBuffer_00598b00, 0);
        while (text != marker) {
            *output = *text;
            output++;
            text++;
        }
        *output = '\0';
        text = marker + 2;
        switch (marker[1]) {
        case 'A':
            DosStrcat(g_szTextScratchBuffer_00598b00,
                      g_apszMedalNames_0046e2e0[
                          g_nConversationMedalIndex_00598c08]);
            break;
        case 'C':
            DosStrcat(g_szTextScratchBuffer_00598b00,
                      g_stCampaignState_0059ca50.currentPilot->callsign);
            break;
        case 'D':
            sprintf(formatted, g_szCampaignDateFormat_00465630,
                    g_pCurrentCampaignDate_005a86a8->year,
                    g_pCurrentCampaignDate_005a86a8->day);
            DosStrcat(g_szTextScratchBuffer_00598b00, formatted);
            break;
        case 'E':
            sprintf(formatted, g_szSavedCampaignDateFormat_0046563c,
                    g_stSavedCampaignDate_0046e188.year,
                    g_stSavedCampaignDate_0046e188.day);
            DosStrcat(g_szTextScratchBuffer_00598b00, formatted);
            break;
        case 'K':
            sprintf(formatted, g_szConversationIntegerFormat_00465628,
                    g_nPlayerKillCount_005a7c9c);
            DosStrcat(g_szTextScratchBuffer_00598b00, formatted);
            break;
        case 'L':
            sprintf(formatted,
                    g_szConversationIntegerFormatAlt_0046562c,
                    g_asCollisionTime_005a7ca0[12]);
            DosStrcat(g_szTextScratchBuffer_00598b00, formatted);
            break;
        case 'N':
        case 'P':
            DosStrcat(g_szTextScratchBuffer_00598b00,
                      g_stCampaignState_0059ca50.currentPilot->name);
            break;
        case 'R':
            DosStrcat(g_szTextScratchBuffer_00598b00,
                      g_apszPilotRankNames_00470098[
                          g_stCampaignState_0059ca50.currentPilot->rank]);
            length = DosStrlen(g_szTextScratchBuffer_00598b00);
            if (length != 0 &&
                g_szTextScratchBuffer_00598b00[length - 1] == '.' &&
                *text == '.')
                g_szTextScratchBuffer_00598b00[length - 1] = '\0';
            break;
        case 'S':
            DosStrcat(g_szTextScratchBuffer_00598b00,
                      (char *)g_abSeriesAuxData_005a8240);
            break;
        case 'T':
            sprintf(formatted, g_szCampaignTimeFormat_00465648,
                    (int)((signed char *)g_pElapsedCampaignDate_005a86ac)[0],
                    (int)((signed char *)g_pElapsedCampaignDate_005a86ac)[1]);
            DosStrcat(g_szTextScratchBuffer_00598b00, formatted);
            break;
        case 'W':
            DosStrcat(g_szTextScratchBuffer_00598b00,
                      g_apWingmanPilots_00598a30[*text - '0']->name);
            text++;
            break;
        }
    }
}

/* Function start: 0x4050B0 */
unsigned int LoadFace(short face)
{
    const TalkingHeadOrigin *origin;

    switch (g_nConversationCharacter_0046e580) {
    case 0:
        g_nConversationBackdropFrame_0046e588 = 4;
        break;
    case 1:
        g_nConversationBackdropFrame_0046e588 = 5;
        break;
    case 2:
        g_nConversationBackdropFrame_0046e588 = 0;
        init_constellation(0);
        g_stConstellationViewport_005a6b40 = DAT_005a76b0;
        g_stConstellationViewport_005a6b40.bottom = 76;
        InitializeConstellationField(&g_stConstellationViewport_005a6b40,
                                     -1, 16);
        g_bConversationConstellation_0046e58c = 1;
        break;
    case 3:
    case 11:
    case 13:
        g_nConversationBackdropFrame_0046e588 = 1;
        break;
    case 4:
        g_nConversationBackdropFrame_0046e588 = 2;
        break;
    case 8:
        g_nConversationBackdropFrame_0046e588 = 1;
        init_constellation(0);
        InitializeConstellationField(&DAT_005a76b0, -1, 16);
        g_bConversationConstellation_0046e58c = 1;
        break;
    case 9:
        g_nConversationBackdropFrame_0046e588 = 2;
        break;
    case 10:
    case 12:
        g_nConversationBackdropFrame_0046e588 = 0;
        break;
    default:
        g_nConversationBackdropFrame_0046e588 = -1;
        break;
    }
    if (face != g_nTalkingHeadFace_0046e584 &&
        g_pTalkingHeadShape_00598c0c != 0)
        FreePacketAndClear((int *)&g_pTalkingHeadShape_00598c0c, 0);
    if (g_pTalkingHeadShape_00598c0c == 0)
        g_pTalkingHeadShape_00598c0c =
            (unsigned char *)FetchDiskPacketRetrying(6, face, 0);
    g_nTalkingHeadFace_0046e584 = face;
    if (g_pConversationOverlayShape_00598c30 == 0)
        g_pConversationOverlayShape_00598c30 =
            (unsigned char *)FetchDiskPacketRetrying(6, 11, 0);
    origin = &g_aTalkingHeadOrigins_0046e190[face];
    g_nTalkingHeadFaceX_005a8754 = origin->faceX;
    g_nTalkingHeadFaceY_005a8756 = origin->faceY;
    g_nTalkingHeadMouthX_005a875a = origin->mouthX;
    g_nTalkingHeadMouthY_005a8758 = origin->mouthY;
    CloseTalk(g_pTalkingHeadShape_00598c0c, -1, -1);
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
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    mouthFrame = 0;
    faceCountdown = 0;
    mouthCountdown = 0;
    FormatTextBufferFromStart(g_szConversationTextFormat_00465654,
                              0, 160,
                              g_nConversationTextColour_00598c10,
                              g_szTextScratchBuffer_00598b00);
    DAT_00469fb4 = 1;
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
        DAT_00469fb4--;
        if (DAT_00469fb4 < 1) {
            DAT_00469fb4 = g_nFrameSkip_00469fb8;
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
    unsigned char clearColour;

    if (g_bConversationConstellation_0046e58c == 1)
        DrawConstellationField();
    clearColour = DAT_0046999c;
    switch (g_nConversationSceneType_00598c0a) {
    case 0:
    case 1:
    case 2:
    case 4:
    case 5:
        if (g_nConversationBackdropFrame_0046e588 != -1) {
            DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                              g_pConversationBackdropShape_00598c04,
                              g_nConversationBackdropFrame_0046e588);
            break;
        }
        ClearViewport(&DAT_005a76b0, clearColour);
        break;
    case 3:
        ClearViewport(&DAT_005a76b0, DAT_004699d8);
        break;
    default:
        ClearViewport(&DAT_005a76b0, clearColour);
        break;
    }
    DrawSpriteDefault(&DAT_005a76b0, 0, 0, talker, 0);
    if (faceFrame >= 0)
        DrawSpriteDefault(&DAT_005a76b0,
                          g_nTalkingHeadFaceX_005a8754,
                          g_nTalkingHeadFaceY_005a8756,
                          talker, (short)(faceFrame + 11));
    if (mouthFrame >= 0)
        DrawSpriteDefault(&DAT_005a76b0,
                          g_nTalkingHeadMouthX_005a875a,
                          g_nTalkingHeadMouthY_005a8758,
                          talker, (short)(mouthFrame + 1));
    if (g_bConversationOverlay_0046e590 != 0)
        DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                          g_pConversationOverlayShape_00598c30,
                          MinShort(g_nTalkingHeadFace_0046e584, 1));
    switch (g_nConversationCharacter_0046e580) {
    case 5:
        DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                          g_pConversationSpecialShape_005a86ec, 10);
        RefreshMemoryStatusOverlay();
        return 0;
    case 6:
        DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                          g_pConversationSpecialShape_005a86ec, 10);
        DrawSpriteDefault(&DAT_005a76b0, 0, 0,
                          g_pConversationSpecialShape_005a86ec, 11);
        RefreshMemoryStatusOverlay();
        return 0;
    }
    RefreshMemoryStatusOverlay();
    return 0;
}

/* Function start: 0x405660 */
unsigned int Briefing(short series, short mission)
{
    DAT_0059ab58 = 0;
    PreloadMusicTrackHook(0x18);
    PreloadMusicTrackHook(0x19);
    PreloadMusicTrackHook(0x1a);
    LoadMissionData(series, mission);
    LoadBriefingData(series, mission);
    if (DAT_0059ab58 == 0) {
        Build_objective_list();
        LoadBriefingRoom();
    }
    DAT_0059ab58 = 0;
    ReleasePacketHandle((int)g_pBriefingPacket_00598aec);
    g_pBriefingPacket_00598aec = 0;
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

    DAT_0059ab58 = 0;
    fullScore = (short)FullMissionScore();
    playerScore = (short)PlayersMissionScore();
    if (fullScore == 0) {
        PreloadMusicTrackHook(0x21);
        StartMusicTrack(0x21, 2, 1);
    } else if ((playerScore * 100) / fullScore > 70) {
        PreloadMusicTrackHook(0x21);
        StartMusicTrack(0x21, 2, 1);
    } else {
        PreloadMusicTrackHook(0x22);
        StartMusicTrack(0x22, 2, 1);
    }
    LoadMissionData(series, mission);
    InitializeConversationViewport();
    InitializeConversationText();
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    SetTextContext(&g_stConversationTextContext_005a7760);
    LoadBriefingData(series, mission);
    g_pConversationBackdropShape_00598c04 =
        (unsigned char *)FetchDiskPacketRetrying(4, 6, 0);
    SceneDirector(1, g_pDebriefingSceneData_00598afc,
                  g_pDebriefingTextData_00598c28);
    DIBslam();
    DIBslamReal();
    DAT_0059ab58 = 0;
    ReleasePacketHandle((int)g_pConversationBackdropShape_00598c04);
    g_pConversationBackdropShape_00598c04 = 0;
    ReleasePacketHandle((int)g_pBriefingPacket_00598aec);
    g_pBriefingPacket_00598aec = 0;
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
    unsigned char *packet;
    unsigned char *sceneData;
    unsigned char *textData;

    DAT_0059ab58 = 0;
    PreloadMusicTrackHook(0x24);
    StartMusicTrack(0x24, 2, 1);
    InitializeConversationViewport();
    InitializeConversationText();
    packet = (unsigned char *)FetchDiskPacketRetrying(
        g_asCampaignBriefingFiles_00469458[g_nCampaignDataSet_005a8118],
        1, 0);
    sceneData = packet + *(unsigned int *)(packet + 0);
    textData = packet + *(unsigned int *)(packet + 4);
    g_pConversationBackdropShape_00598c04 =
        (unsigned char *)FetchDiskPacketRetrying(4, 7, 0);
    SceneDirector(4, sceneData, textData);
    DIBslam();
    DIBslamReal();
    DAT_0059ab58 = 0;
    ReleasePacketHandle((int)g_pConversationBackdropShape_00598c04);
    g_pConversationBackdropShape_00598c04 = 0;
    ReleasePacketHandle((int)packet);
    ReleaseTextFont(0);
    ResetScreenClipToFullHeight();
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook(0x24);
    return 0;
}

/* Function start: 0x405910 */
unsigned int LoadBriefingData(short series, short mission)
{
    g_pBriefingPacket_00598aec = (unsigned char *)FetchDiskPacketRetrying(
        g_asCampaignBriefingFiles_00469458[g_nCampaignDataSet_005a8118],
        (short)(mission + series * 4), 0);
    g_pBriefingSceneData_00598c00 = g_pBriefingPacket_00598aec +
        *(unsigned int *)(g_pBriefingPacket_00598aec + 0x00);
    g_pBriefingTextData_00598af0 = g_pBriefingPacket_00598aec +
        *(unsigned int *)(g_pBriefingPacket_00598aec + 0x04);
    g_pDebriefingSceneData_00598afc = g_pBriefingPacket_00598aec +
        *(unsigned int *)(g_pBriefingPacket_00598aec + 0x08);
    g_pDebriefingTextData_00598c28 = g_pBriefingPacket_00598aec +
        *(unsigned int *)(g_pBriefingPacket_00598aec + 0x0c);
    g_apRecRoomSceneData_00598ae0[0] = g_pBriefingPacket_00598aec +
        *(unsigned int *)(g_pBriefingPacket_00598aec + 0x10);
    g_apRecRoomTextData_00598aa0[0] = g_pBriefingPacket_00598aec +
        *(unsigned int *)(g_pBriefingPacket_00598aec + 0x14);
    g_apRecRoomSceneData_00598ae0[2] = g_pBriefingPacket_00598aec +
        *(unsigned int *)(g_pBriefingPacket_00598aec + 0x18);
    g_apRecRoomTextData_00598aa0[2] = g_pBriefingPacket_00598aec +
        *(unsigned int *)(g_pBriefingPacket_00598aec + 0x1c);
    g_apRecRoomSceneData_00598ae0[1] = g_pBriefingPacket_00598aec +
        *(unsigned int *)(g_pBriefingPacket_00598aec + 0x20);
    g_apRecRoomTextData_00598aa0[1] = g_pBriefingPacket_00598aec +
        *(unsigned int *)(g_pBriefingPacket_00598aec + 0x24);
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
    MissionNavPoint *nav;
    MissionObjectiveSource *objective;
    MissionShipRecord *ship;
    short *sourceInitialShip;
    short *initialShip;
    int index;
    int item;

    logicalFile = g_asMissionDataFiles_00469460[g_nCampaignDataSet_005a8118];
    packet = (unsigned char *)FetchDiskPacketRetrying(logicalFile, 0, 0);
    missionIndex = (int)mission + (int)series * 4;
    header = (MissionHeaderDisk *)(packet + missionIndex * 0x18);
    g_nMissionEntryNavPoint_005a8690 = header->entryNavPoint;
    g_nHomeMissionShipIndex_005a8692 = header->homeMissionShip;
    g_nPlayerMissionShipIndex_005a8694 = header->playerMissionShip;
    sourceInitialShip = header->initialMissionShips;
    initialShip = g_nInitialMissionShipIndices_005a8696;
    do {
        *initialShip++ = *sourceInitialShip++;
    } while (initialShip < &g_nInitialMissionShipIndices_005a8696[8]);
    DAT_005a86a6 = header->field_16;
    ReleasePacketHandle((int)packet);

    packet = (unsigned char *)FetchDiskPacketRetrying(logicalFile, 1, 0);
    diskNav = (MissionNavPointDisk *)(packet + missionIndex * 0x4d0);
    index = 0;
    do {
        nav = &g_aMissionNavPoints_0046c2f0[index];
        memcpy(nav->name, diskNav->name, sizeof(nav->name));
        nav->type = diskNav->type;
        nav->position = diskNav->position;
        nav->proximityRadius = diskNav->proximityRadius;
        item = 0;
        do {
            ((signed char *)nav->triggers)[item] =
                ((signed char *)diskNav->triggers)[item];
            item++;
        } while (item < 8);
        item = 0;
        do {
            nav->preloadObjectTypes[item] =
                (enum ObjectType)diskNav->preloadObjectTypes[item];
            item++;
        } while (item < 2);
        item = 0;
        do {
            nav->missionShips[item] = diskNav->missionShips[item];
            item++;
        } while (item < 10);
        diskNav++;
        index++;
    } while (index < WC1_ACTIVE_MISSION_NAV_POINT_COUNT);
    ReleasePacketHandle((int)packet);

    packet = (unsigned char *)FetchDiskPacketRetrying(logicalFile, 2, 0);
    diskObjective =
        (MissionObjectiveDisk *)(packet + missionIndex * 0x400);
    index = 0;
    do {
        objective = &g_aMissionObjectiveSources_005a8270[index];
        objective->type = diskObjective->type;
        objective->index = diskObjective->index;
        item = 0;
        do {
            objective->description[item] = diskObjective->description[item];
            item++;
        } while (item < 60);
        diskObjective++;
        index++;
    } while (index < WC1_MISSION_OBJECTIVE_COUNT);
    ReleasePacketHandle((int)packet);

    packet = (unsigned char *)FetchDiskPacketRetrying(logicalFile, 3, 0);
    diskShip = (MissionShipDisk *)(packet + missionIndex * 0x540);
    ship = g_aMissionShips_0046c948;
    do {
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
        diskShip++;
        ship++;
    } while (ship < &g_aMissionShips_0046c948[32]);
    ReleasePacketHandle((int)packet);

    packet = (unsigned char *)FetchDiskPacketRetrying(logicalFile, 4, 0);
    DosMemcpy(g_abMissionAuxData_005a8210,
              packet + missionIndex * sizeof(g_abMissionAuxData_005a8210),
              sizeof(g_abMissionAuxData_005a8210));
    ReleasePacketHandle((int)packet);

    packet = (unsigned char *)FetchDiskPacketRetrying(logicalFile, 5, 0);
    DosMemcpy(g_abSeriesAuxData_005a8240,
              packet + series * sizeof(g_abSeriesAuxData_005a8240),
              sizeof(g_abSeriesAuxData_005a8240));
    ReleasePacketHandle((int)packet);
    return 0;
}

/* Function start: 0x405CC0 */
unsigned int UpdateMap(char *text, short duration)
{
    Viewport savedScreen;
    Viewport savedVirtualScreen;

    savedScreen = DAT_005a6ba0;
    savedVirtualScreen = DAT_005a76b0;
    ClearViewport(&DAT_005a6ba0, DAT_0046999c);
    if (DAT_005a76b0.pixels != 0)
        ClearViewport(&DAT_005a76b0, DAT_0046999c);
    AddPCName(text);
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    FormatTextBufferFromStart(g_szBriefingMapTextFormat_00465660,
                              0, 160,
                              g_nConversationTextColour_00598c10,
                              g_szTextScratchBuffer_00598b00);
    DAT_005a6ba0 = savedScreen;
    DAT_005a76b0 = savedVirtualScreen;
    BriefingMap_DisplayMap();
    WaitForSceneAdvance(duration, 0);
    ClearViewport(&DAT_005a6ba0, DAT_0046999c);
    SetTextContext(&g_stConversationTextContext_005a7760);
    ClearViewport(&DAT_005a6ba0, DAT_0046999c);
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
    ClearViewport(&g_stConversationTextViewport_005a7570,
                  DAT_0046999c);
    FormatTextBufferFromStart(g_szCloseLookTextFormat_0046566c,
                              0, 160,
                              g_nConversationTextColour_00598c10,
                              g_szTextScratchBuffer_00598b00);
    DAT_00469fb4 = 1;
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
                DAT_00469fb4--;
                if (DAT_00469fb4 < 1) {
                    DAT_00469fb4 = g_nFrameSkip_00469fb8;
                    if (shot == 11) {
                        DrawDebriefingLongShot();
                        if (frame > -1)
                            DrawSpriteDefault(
                                &DAT_005a76b0,
                                g_nDebriefingPodiumX_0046e57c, 53,
                                g_pConversationBackdropShape_00598c04,
                                (short)(frame + 17));
                    } else if (frame > -1) {
                        DrawSpriteDefault(&DAT_005a76b0, 225, 34,
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
        do {
            DAT_00469fb4--;
            if (DAT_00469fb4 < 1) {
                DAT_00469fb4 = g_nFrameSkip_00469fb8;
                DrawSpriteDefault(
                    &DAT_005a76b0, 0, 0,
                    g_pConversationBackdropShape_00598c04, 0);
                DrawSpriteDefault(&DAT_005a76b0, 241, 60,
                                  g_pBriefingAnimationShape_00598c14,
                                  sceneFrame);
                DrawSpriteDefault(&DAT_005a76b0, 241, 64,
                                  g_pBriefingAnimationShape_00598c14, 22);
                character = 0;
                do {
                    DrawBriefingCharacter(
                        character, 0,
                        g_aBriefingCharacters_0046e218[character]
                            .animation[sceneFrame],
                        0, 0);
                    character++;
                } while (character < 14);
                RefreshMemoryStatusOverlay();
                DIBslam();
                DIBslamReal();
            }
            if (CheckEscaped() != 0)
                sceneFrame = 20;
            if (sceneFrame == 20)
                DAT_00469fb4 = 1;
            sceneFrame++;
            DIBslam();
            DIBslamReal();
        } while (sceneFrame < 22);
    }
    DIBslam();
    DIBslamReal();
    WaitForSceneAdvance(duration, unused);
    return 0;
}
