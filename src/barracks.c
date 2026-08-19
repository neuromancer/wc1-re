/*
 *  Barracks and campaign save/load room.
 *
 *  Address range 0x41ada0-0x41c75f (provisional -- see docs/ORDER.md).
 *  Boundary evidence: contiguous SAVEGAME.WLD helpers ending at BarracksScreen;
 *  the Win32 debug-overlay compilation unit begins at 0x41c760.
 */
#include "wc1.h"

/* Function start: 0x41ADA0 */
void CreateEmptySaveGameFile(void)
{
    SaveGameDiskRecord diskRecord;
    SaveGameRecord gameRecord;
    SaveGameDiskObjective *diskObjective;
    MissionObjective *gameObjective;
    unsigned char *diskBytes;
    unsigned char *gameBytes;
    short *diskDeathMission;
    int *gameDeathMission;
    short file;
    short slot;
    int index;

    slot = 0;
    gameRecord.occupied = 0;
    PromptInsertNumberedDisk(0x10);
    file = CreateDataFile("SAVEGAME.WLD");
    if (file >= 0) {
        for (; slot < 8; slot++) {
            sprintf(gameRecord.description, "game %d", (int)slot + 1);
            memcpy(diskRecord.description, gameRecord.description,
                   sizeof(diskRecord.description));
            diskRecord.occupied = gameRecord.occupied;
            memcpy(diskRecord.pilots, gameRecord.pilots,
                   sizeof(diskRecord.pilots));
            diskRecord.campaign.currentPilot =
                (short)(int)gameRecord.campaign.currentPilot;
            diskRecord.campaign.playerShipType =
                (short)gameRecord.campaign.playerShipType;

            diskBytes = diskRecord.campaign.medals;
            gameBytes = gameRecord.campaign.medals;
            for (index = 0; index < 5; index++) {
                diskBytes[index] = gameBytes[index];
            }

            diskBytes = diskRecord.campaign.badges;
            gameBytes = gameRecord.campaign.badges;
            for (index = 0; index < 12; index++) {
                diskBytes[index] = gameBytes[index];
            }

            diskRecord.campaign.currentMission =
                gameRecord.campaign.currentMission;
            diskRecord.campaign.currentSeries =
                gameRecord.campaign.currentSeries;
            diskRecord.campaign.seriesHistoryCount =
                gameRecord.campaign.seriesHistoryCount;
            diskBytes = (unsigned char *)
                diskRecord.campaign.seriesHistory;
            gameBytes = (unsigned char *)
                gameRecord.campaign.seriesHistory;
            for (index = 0; index < 8; index++) {
                diskBytes[index] = gameBytes[index];
            }

            gameDeathMission =
                gameRecord.campaign.personalityDeathMission;
            diskDeathMission =
                diskRecord.campaign.personalityDeathMission;
            for (; diskDeathMission <
                       diskRecord.campaign.personalityDeathMission + 8;
                 diskDeathMission++, gameDeathMission++) {
                *diskDeathMission = (short)*gameDeathMission;
            }

            diskBytes = diskRecord.campaign.aceFlags;
            gameBytes = gameRecord.campaign.aceFlags;
            for (index = 0; index < 4; index++) {
                diskBytes[index] = gameBytes[index];
            }
            diskRecord.campaign.currentDate =
                gameRecord.campaign.currentDate;
            diskRecord.campaign.elapsedDate.day =
                gameRecord.campaign.elapsedDate.day;
            diskRecord.campaign.elapsedDate.year =
                gameRecord.campaign.elapsedDate.year;
            diskRecord.campaign.promotionScore =
                gameRecord.campaign.promotionScore;
            diskRecord.campaign.missionScore =
                gameRecord.campaign.missionScore;
            diskRecord.campaign.seriesScore =
                gameRecord.campaign.seriesScore;
            diskRecord.campaign.campaignIndex =
                gameRecord.campaign.campaignIndex;

            gameObjective = gameRecord.objectives;
            diskObjective = diskRecord.objectives;
            for (; diskObjective < diskRecord.objectives + 16;
                 diskObjective++, gameObjective++) {
                diskObjective->mapX = gameObjective->mapX;
                diskObjective->mapY = gameObjective->mapY;
                diskObjective->field_4 = gameObjective->field_4;
                diskObjective->type = (short)gameObjective->type;
                diskObjective->index = gameObjective->index;
                diskObjective->flags = gameObjective->flags;
                diskObjective->displayName =
                    (short)(int)gameObjective->displayName;
                diskObjective->name = (short)(int)gameObjective->name;
                diskObjective->position = gameObjective->position;
            }

            WriteDataFileAtOffset((unsigned short)file,
                                  (int)slot * 0x33c, 0x33c,
                                  &diskRecord);
        }
        CloseDataFile((unsigned short)file);
    }
}

/* Function start: 0x41B020 */
void EnsureSaveGameFile(void)
{
    short file;
    long length;

    PromptInsertNumberedDisk(0x10);
    file = OpenDataFileOrDie("SAVEGAME.WLD");
    if (file < 0) {
        CreateEmptySaveGameFile();
        return;
    }
    length = _filelength((int)file);
    CloseDataFile((unsigned short)file);
    if (length != 0x19e0)
        CreateEmptySaveGameFile();
}

/* Function start: 0x41B070 */
void InitializeBarracksAnimation(BarracksAnimationState *state)
{
    short bunk;

    bunk = 0;
    do {
        state->bunks[bunk].animationFrame =
            RandomInRange(0, 13);
        state->bunks[bunk].animationPeriod =
            (short)(RandomInRange(0, 12) + 13);
        state->bunks[bunk].animationTick = 0;
        bunk++;
    } while (bunk < 8);
    state->fallingY = -99;
    state->fallingDelay = 20;
    state->impactFrame = 49;
    state->blinkDelay = 0;
    state->fallingVelocity = 0;
    state->animationTick = 0;
    state->menuLabel = 0;
    state->eyesOpen = 1;
}

/* Function start: 0x41B0E0 */
void FreeBarracksMenuLabel(char **label)
{
    if (*label != 0 &&
        *label != apszSaveCampaignMenuLabels[0] &&
        *label != apszSaveCampaignMenuLabels[1]) {
        ReleasePacketHandle(*label);
        *label = 0;
    }
}

/* Function start: 0x41B110 */
void SetAwakenBarracksMenuLabel(char **label, int series, int mission,
                                char *description)
{
    FreeBarracksMenuLabel(label);
#ifdef WC1_SDL
    /* MSVC 4.20 accepts %Fs as its legacy far-string conversion. */
    sprintf(szTextScratchBuffer, "Awaken %s.", description);
#else
    sprintf(szTextScratchBuffer, "Awaken %Fs.", description);
#endif
    *label = AllocateTaggedMemory(
        strlen(szTextScratchBuffer) + 2, 0);
    DosMemcpy(*label, szTextScratchBuffer,
              strlen(szTextScratchBuffer) + 2);
}

/* Function start: 0x41B180 */
void FreeBarracksMenuLabels(void)
{
    const char *saveCampaignLabel;
    short bunk;

    saveCampaignLabel = apszSaveCampaignMenuLabels[0];
    bunk = 0;
    do {
        if (apszBarracksMenuLabels[bunk * 2] !=
            saveCampaignLabel) {
            FreeBarracksMenuLabel(
                &apszBarracksMenuLabels[bunk * 2]);
            FreeBarracksMenuLabel(
                &apszBarracksMenuLabels[bunk * 2 + 1]);
            saveCampaignLabel =
                apszSaveCampaignMenuLabels[0];
        }
        bunk++;
    } while (bunk < 8);
    pszCurrentRoomMenuLabel = 0;
}

/* Function start: 0x41B1E0 */
short SaveGame(short slot, SaveGameRecord *gameRecord)
{
    SaveGameDiskRecord diskRecord;
    SaveGameDiskObjective *diskObjective;
    MissionObjective *gameObjective;
    unsigned char *diskBytes;
    short *diskDeathMission;
    int *gameDeathMission;
    short file;
    short written;
    int index;

    PromptInsertNumberedDisk(0x10);
    memcpy(diskRecord.description, gameRecord->description,
           sizeof(diskRecord.description));
    diskRecord.occupied = gameRecord->occupied;
    memcpy(diskRecord.pilots, gameRecord->pilots,
           sizeof(diskRecord.pilots));
    diskRecord.campaign.currentPilot =
        (short)(int)gameRecord->campaign.currentPilot;
    diskRecord.campaign.playerShipType =
        (short)gameRecord->campaign.playerShipType;
    diskBytes = diskRecord.campaign.medals;
    for (index = 0; index < 5; index++) {
        diskBytes[index] = gameRecord->campaign.medals[index];
    }

    diskBytes = diskRecord.campaign.badges;
    for (index = 0; index < 12; index++) {
        diskBytes[index] = gameRecord->campaign.badges[index];
    }
    diskRecord.campaign.currentMission =
        gameRecord->campaign.currentMission;
    diskRecord.campaign.currentSeries =
        gameRecord->campaign.currentSeries;
    diskRecord.campaign.seriesHistoryCount =
        gameRecord->campaign.seriesHistoryCount;
    diskBytes = (unsigned char *)diskRecord.campaign.seriesHistory;
    for (index = 0; index < 8; index++) {
        diskBytes[index] =
            (unsigned char)gameRecord->campaign.seriesHistory[index];
    }

    gameDeathMission = gameRecord->campaign.personalityDeathMission;
    diskDeathMission = diskRecord.campaign.personalityDeathMission;
    for (; diskDeathMission <
               diskRecord.campaign.personalityDeathMission + 8;
         diskDeathMission++, gameDeathMission++) {
        *diskDeathMission = (short)*gameDeathMission;
    }

    diskBytes = diskRecord.campaign.aceFlags;
    for (index = 0; index < 4; index++) {
        diskBytes[index] = gameRecord->campaign.aceFlags[index];
    }
    diskRecord.campaign.currentDate = gameRecord->campaign.currentDate;
    diskRecord.campaign.elapsedDate.day =
        gameRecord->campaign.elapsedDate.day;
    diskRecord.campaign.elapsedDate.year =
        gameRecord->campaign.elapsedDate.year;
    diskRecord.campaign.promotionScore =
        gameRecord->campaign.promotionScore;
    diskRecord.campaign.missionScore = gameRecord->campaign.missionScore;
    diskRecord.campaign.seriesScore = gameRecord->campaign.seriesScore;
    diskRecord.campaign.campaignIndex = gameRecord->campaign.campaignIndex;

    gameObjective = gameRecord->objectives;
    diskObjective = diskRecord.objectives;
    for (; diskObjective < diskRecord.objectives + 16;
         diskObjective++, gameObjective++) {
        diskObjective->mapX = gameObjective->mapX;
        diskObjective->mapY = gameObjective->mapY;
        diskObjective->field_4 = gameObjective->field_4;
        diskObjective->type = (short)gameObjective->type;
        diskObjective->index = gameObjective->index;
        diskObjective->flags = gameObjective->flags;
        diskObjective->displayName =
            (short)(int)gameObjective->displayName;
        diskObjective->name = (short)(int)gameObjective->name;
        diskObjective->position = gameObjective->position;
    }

    file = OpenDataFileOrDie("SAVEGAME.WLD");
    written = WriteDataFileAtOffset((unsigned short)file,
                                    (int)slot * 0x33c, 0x33c,
                                    &diskRecord);
    CloseDataFile((unsigned short)file);
    return (written != 0) & (file >= 0);
}

/* Function start: 0x41B420 */
short PromptForTextInput(short x, short y, const char *prompt,
                         char *destination, short maximumLength,
                         short inputMode)
{
    ModalTextPanel panel;
    volatile unsigned int bounds[2];
    short promptWidth;
    short result;
    int widestCharacter;

    result = 0;
    ((short *)&bounds[0])[0] = x;
    ((short *)&bounds[0])[1] = y;
    ((short *)&bounds[1])[1] = (short)(y + 20);
    InitializeModalTextPanel(&panel, 0,
                             dwModalBoundsTopLeft,
                             dwModalBoundsBottomRight,
                             cBlackColour, cBlackColour,
                             cBlackColour);
    widestCharacter = MeasureTextPixelWidthClamped("M");
    widestCharacter *= (int)maximumLength;
    promptWidth = MeasureTextPixelWidthClamped(prompt);
    widestCharacter += (int)promptWidth;
    ((short *)&bounds[1])[0] = (short)(x +
        (widestCharacter * 16) / 15);
    RestoreModalTextPanel(&panel);
    if (InitializeModalTextPanel(&panel, 0,
                                 (unsigned int)bounds[0],
                                 (unsigned int)bounds[1],
                                 cViewportClearColour,
                                 cBlueColour, cRedColour) != 0) {
        DrawModalTextPanel(&panel, 3, 6, 0, prompt);
        DIBslam();
        DIBslamReal();
        if (ReadTextInput(destination, maximumLength, inputMode) != 0)
            result = 1;
        RestoreModalTextPanel(&panel);
    }
    return result;
}

/* Function start: 0x41B550 */
int WarnLoadGameFirst(void)
{
    InputEventState event;
    short key;

    LeaveAllocationScope();
    key = 0;
    if (ShowModalTextPanel(0, "Load a game first.") != 0) {
        while (PollInputEvent(&event, 0xff) != 0) {
        }
        key = WaitForInputKey();
        while (PollInputEvent(&event, 0xff) != 0) {
        }
        ReleaseModalTextPanel();
    }
    EnterAllocationScope();
    return (int)key;
}

/* Function start: 0x41B5C0 */
void SaveGameWithNamePrompt(short slot, CampaignState *campaign,
                            PilotRecord *pilots,
                            MissionObjective *objectives)
{
    SaveGameRecord gameRecord;
    char oldLabel[40];
    char *separator;

    if (bCampaignActive == 0) {
        WarnLoadGameFirst();
        return;
    }
    DosStrcpy(oldLabel,
              apszBarracksMenuLabels[slot * 2]);
    if (strcmp(oldLabel,
               apszSaveCampaignMenuLabels[0]) == 0)
        oldLabel[0] = 0;
    separator = DosStrchr(oldLabel, ' ');
    if (separator != 0)
        DosStrcpy(oldLabel, separator + 1);
    separator = DosStrchr(oldLabel, '.');
    if (separator != 0)
        *separator = 0;
    DosStrcpy(gameRecord.description, oldLabel);
    if (PromptForTextInput(40, 24, "Game Name: ",
                           gameRecord.description, 16, 1) != 0) {
        gameRecord.occupied = 1;
        memcpy(&gameRecord.campaign, campaign,
               sizeof(gameRecord.campaign));
        memcpy(gameRecord.pilots, pilots,
               sizeof(gameRecord.pilots));
        DosMemcpy(gameRecord.objectives, objectives, 0x1f0);
        if (SaveGame(slot, &gameRecord) == 0)
            ShowModalMessage("Error: Game %s not saved.",
                             gameRecord.description);
    }
}

/* Function start: 0x41B710 */
int LoadGame(short slot, SaveGameRecord *gameRecord)
{
    SaveGameDiskRecord diskRecord;
    SaveGameDiskObjective *diskObjective;
    MissionObjective *gameObjective;
    unsigned char *diskBytes;
    unsigned char *gameBytes;
    short *diskDeathMission;
    int *gameDeathMission;
    short file;
    short fileOpen;
    int index;
    int read;

    PromptInsertNumberedDisk(0x10);
    file = OpenDataFileOrDie("SAVEGAME.WLD");
    fileOpen = file >= 0;
    read = ReadDataFileAtOffset((unsigned short)file,
                                (int)slot * 0x33c, 0x33c,
                                &diskRecord);
    memcpy(gameRecord->description, diskRecord.description,
           sizeof(gameRecord->description));
    gameRecord->occupied = diskRecord.occupied;
    memcpy(gameRecord->pilots, diskRecord.pilots,
           sizeof(gameRecord->pilots));
    gameRecord->campaign.currentPilot =
        (PilotRecord *)(int)diskRecord.campaign.currentPilot;
    gameRecord->campaign.playerShipType =
        (enum ObjectType)diskRecord.campaign.playerShipType;
    gameBytes = gameRecord->campaign.medals;
    diskBytes = diskRecord.campaign.medals;
    for (index = 0; index < 5; index++) {
        gameBytes[index] = diskBytes[index];
    }

    gameBytes = gameRecord->campaign.badges;
    diskBytes = diskRecord.campaign.badges;
    for (index = 0; index < 12; index++) {
        gameBytes[index] = diskBytes[index];
    }
    gameRecord->campaign.currentMission =
        diskRecord.campaign.currentMission;
    gameRecord->campaign.currentSeries =
        diskRecord.campaign.currentSeries;
    gameRecord->campaign.seriesHistoryCount =
        diskRecord.campaign.seriesHistoryCount;
    gameBytes = (unsigned char *)gameRecord->campaign.seriesHistory;
    diskBytes = (unsigned char *)diskRecord.campaign.seriesHistory;
    for (index = 0; index < 8; index++) {
        gameBytes[index] = diskBytes[index];
    }

    gameDeathMission = gameRecord->campaign.personalityDeathMission;
    diskDeathMission = diskRecord.campaign.personalityDeathMission;
    for (; diskDeathMission <
               diskRecord.campaign.personalityDeathMission + 8;
         diskDeathMission++, gameDeathMission++) {
        *gameDeathMission = (int)*diskDeathMission;
    }

    gameBytes = gameRecord->campaign.aceFlags;
    diskBytes = diskRecord.campaign.aceFlags;
    for (index = 0; index < 4; index++) {
        gameBytes[index] = diskBytes[index];
    }
    gameRecord->campaign.currentDate = diskRecord.campaign.currentDate;
    gameRecord->campaign.elapsedDate.day =
        diskRecord.campaign.elapsedDate.day;
    gameRecord->campaign.elapsedDate.year =
        diskRecord.campaign.elapsedDate.year;
    gameRecord->campaign.promotionScore =
        diskRecord.campaign.promotionScore;
    gameRecord->campaign.missionScore = diskRecord.campaign.missionScore;
    gameRecord->campaign.seriesScore = diskRecord.campaign.seriesScore;
    gameRecord->campaign.campaignIndex =
        diskRecord.campaign.campaignIndex;

    gameObjective = gameRecord->objectives;
    diskObjective = diskRecord.objectives;
    for (; diskObjective < diskRecord.objectives + 16;
         diskObjective++, gameObjective++) {
        gameObjective->mapX = diskObjective->mapX;
        gameObjective->mapY = diskObjective->mapY;
        gameObjective->field_4 = diskObjective->field_4;
        gameObjective->type = (int)diskObjective->type;
        gameObjective->index = diskObjective->index;
        gameObjective->flags = diskObjective->flags;
        gameObjective->displayName =
            (const char *)(int)diskObjective->displayName;
        gameObjective->name = (char *)(int)diskObjective->name;
        gameObjective->position = diskObjective->position;
    }

    fileOpen &= read != 0;
    CloseDataFile((unsigned short)file);
    if (fileOpen != 0 && gameRecord->occupied != 0)
        return 1;
    return 0;
}

/* Function start: 0x41B980 */
void LoadGameFromSlot(short slot, CampaignState *campaign,
                      PilotRecord *pilots,
                      MissionObjective *objectives)
{
    SaveGameRecord gameRecord;
    int loaded;

    LeaveAllocationScope();
    if (ShowModalTextPanel(0, "Loading Game...") != 0) {
        loaded = LoadGame(slot, &gameRecord);
        if (loaded != 0) {
            memcpy(pilots, gameRecord.pilots,
                   sizeof(gameRecord.pilots));
            memcpy(campaign, &gameRecord.campaign,
                   sizeof(gameRecord.campaign));
            nPendingCampaignIndex = campaign->campaignIndex;
            nCampaignDataSet = nPendingCampaignIndex;
            LoadPacketIntoBuffer(
                asCampaignPilotFiles[nPendingCampaignIndex], 0,
                pConstellationDefinitions);
            LoadPacketIntoBuffer(
                asCampaignPilotFiles[
                    nCampaignDataSet],
                1, pMissionCampaignData);
            DosMemcpy(objectives, gameRecord.objectives, 0x1f0);
            CorrectPointers();
            bCampaignActive = 1;
        }
        ReleaseModalTextPanel();
        if (loaded == 0) {
            ShowModalMessage("Error: Game %d not loaded.", (int)slot);
        } else if (nOriginDevUnlock != 0) {
            memcpy(campaign->currentPilot->callsign, "CHEATER", 8);
        }
    }
    EnterAllocationScope();
}

/* Function start: 0x41BAD0 */
void SetBunkMenuLabel(short occupied, short side, char **label,
                      int series, int mission, char *description)
{
    if (occupied == 0) {
        *label = (char *)apszSaveCampaignMenuLabels[0];
        return;
    }
    if (side == 0) {
        SetAwakenBarracksMenuLabel(label, series, mission, description);
        return;
    }
    *label = (char *)apszSaveCampaignMenuLabels[1];
}

/* Function start: 0x41BB20 */
void GetBunkInfo(BarracksAnimationState *state)
{
    SaveGameRecord gameRecord;
    short bunk;

    FreeBarracksMenuLabels();
    bunk = 0;
    do {
        state->bunks[bunk].occupied =
            (short)LoadGame(bunk, &gameRecord);
        SetBunkMenuLabel(
            state->bunks[bunk].occupied, 0,
            &apszBarracksMenuLabels[bunk * 2],
            (int)gameRecord.campaign.currentSeries,
            (int)gameRecord.campaign.currentMission,
            gameRecord.description);
        SetBunkMenuLabel(
            state->bunks[bunk].occupied, 1,
            &apszBarracksMenuLabels[bunk * 2 + 1],
            (int)gameRecord.campaign.currentSeries,
            (int)gameRecord.campaign.currentMission,
            gameRecord.pilots[0].name + 6);
        bunk++;
    } while (bunk < 8);
}

/* Function start: 0x41BBD0 */
void DrawBarracksBunks(Viewport *viewport, unsigned char *shape,
                       BarracksAnimationState *state)
{
    short bunk;
    short frame;

    DrawSpriteDefault(viewport, 0, 0, shape, 0);
    bunk = 0;
    do {
        frame = 10;
        if (state->bunks[bunk].occupied != 0) {
            DrawSpriteDefault(
                viewport, aBarracksBunkOrigins[bunk].x,
                aBarracksBunkOrigins[bunk].y,
                shape, (short)(bunk + 1));
            frame = 9;
        }
        DrawSpriteDefault(viewport,
                          (short)((bunk % 2) * 31 + 143),
                          (short)((bunk / 2) * 5 + 167),
                          shape, frame);
        bunk++;
    } while (bunk < 8);
    CopyViewportContents(&stSceneBuffer,
                         &stRoomScreenViewport);
}

/* Function start: 0x41BC90 */
void DrawBarracksStaticDetails(Viewport *viewport,
                               unsigned char *shape)
{
    DrawSpriteDefault(viewport, 147, 167, shape, 25);
    CheckCursor();
    DrawSpriteDefault(viewport, 304, 144, shape, 36);
    CheckCursor();
}

/* Function start: 0x41BCE0 */
void AnimateBarracks(Viewport *viewport, unsigned char *shape,
                     BarracksAnimationState *state)
{
    BarracksBunkState *bunkState;
    int frameTick;
    int bunkTick;
    short bunk;
    short frame;

    frameTick = (int)nTickCount60Hz / 3;
    for (bunk = 0; bunk < 8; bunk++) {
        frame = 11;
        bunkState = &state->bunks[bunk];
        if (bunkState->occupied != 0) {
            bunkTick = frameTick / 4;
            if (bunkState->animationTick != bunkTick) {
                bunkState->animationTick = bunkTick;
                bunkState->animationFrame++;
                if (bunkState->animationTick %
                        (int)bunkState->animationPeriod == 0)
                    bunkState->animationFrame = 0;
            }
            frame = 24;
            if (bunkState->animationFrame < 13)
                frame = (short)(24 - bunkState->animationFrame);
        }
        DrawSpriteDefault(viewport,
                          (short)((bunk % 2) * 14 + 148),
                          (short)((bunk / 2) * 5 + 166),
                          shape, frame);
    }

    if ((int)state->animationTick != frameTick) {
        state->animationTick = (short)frameTick;
        state->fallingDelay--;
        if (state->fallingDelay == 0) {
            state->fallingY = -5;
            state->fallingVelocity = 3;
        }
        if (state->fallingY != -99) {
            state->fallingY += state->fallingVelocity;
            state->fallingVelocity++;
            if (state->fallingY > 115) {
                state->fallingY = -99;
                state->impactFrame = 37;
                state->fallingDelay = 20;
                PlaySfxWaveFileByNumber(35, -1, 0);
            }
        }
    }
    if (state->fallingY != -99) {
        DrawSpriteDefault(viewport, 298, (short)state->fallingY,
                          shape,
                          (short)(state->fallingY / 40 + 27));
        DrawSpriteDefault(viewport, 305, 46, shape, 35);
    }
    frame = state->impactFrame;
    if (frame != 49) {
        state->impactFrame++;
        DrawSpriteDefault(viewport, 298, 139, shape, frame);
    }

    if (state->blinkDelay != 0) {
        state->eyesOpen = (short)(state->eyesOpen == 0);
        state->blinkDelay--;
        if (state->blinkDelay == 0 &&
            (unsigned short)RandomInRange(0, 100) < 90)
            state->eyesOpen = 1;
    } else if (RandomInRange(0, 70) == 0) {
        state->blinkDelay = (short)(RandomInRange(0, 15) + 2);
    }
    if (state->eyesOpen != 0)
        frame = 49;
    else
        frame = 26;
    DrawSpriteDefault(viewport, 45, 0, shape, frame);
    CheckCursor();
    if (state->menuLabel != pszCurrentRoomMenuLabel) {
        state->menuLabel = pszCurrentRoomMenuLabel;
        DrawSpriteDefault(viewport, 319, 199, shape, 50);
    }
}

/* Function start: 0x41BF10 */
int ConfirmQuitWingCommander(void)
{
    short confirmed;

    LeaveAllocationScope();
    confirmed = 0;
    if (ShowModalTextPanel(0, "Quit Wing Commander? (Y/N)") != 0) {
        confirmed = (short)((short)toupper(
            WaitForStreamInputKey()) == 'Y');
        ReleaseModalTextPanel();
    }
    EnterAllocationScope();
    return (int)confirmed;
}

/* Function start: 0x41BF60 */
int ConfirmAwakenAfterBadData(short slot)
{
    SaveGameRecord gameRecord;
    short confirmed;

    confirmed = 0;
    if (LoadGame(slot, &gameRecord) == 0)
        ShowModalMessage("Error: data may be bad.");
    LeaveAllocationScope();
    if (ShowModalTextPanel(0, "Awaken %s? (Y/N)",
                           gameRecord.description) != 0) {
        confirmed = (short)((short)toupper(
            WaitForStreamInputKey()) == 'Y');
        ReleaseModalTextPanel();
    }
    EnterAllocationScope();
    return (int)confirmed;
}

/* Function start: 0x41BFE0 */
int ConfirmReplaceFaultyData(short slot)
{
    SaveGameRecord gameRecord;
    short confirmed;

    confirmed = 0;
    if (bCampaignActive == 0) {
        WarnLoadGameFirst();
        return 0;
    }
    if (LoadGame(slot, &gameRecord) == 0)
        memcpy(gameRecord.description, "FAULTY DATA", 12);
    LeaveAllocationScope();
    if (ShowModalTextPanel(0, "Replace %s? (Y/N)",
                           gameRecord.description) != 0) {
        confirmed = (short)((short)toupper(
            WaitForStreamInputKey()) == 'Y');
        ReleaseModalTextPanel();
    }
    EnterAllocationScope();
    return (int)confirmed;
}

/* Function start: 0x41C090 */
void HandleBarracksBunkSelection(Viewport *viewport,
                                  unsigned char *shape,
                                  BarracksAnimationState *state,
                                  short region)
{
    short slot;

    LeaveAllocationScope();
    slot = (short)(region / 2);
    if (state->bunks[slot].occupied != 0) {
        if (region % 2 == 0) {
            if (ConfirmAwakenAfterBadData(slot) != 0) {
                LoadGameFromSlot(
                    slot, &stCampaignState,
                    aPilotRecords,
                    aMissionObjectives);
            }
            goto refresh;
        }
        if (ConfirmReplaceFaultyData(slot) == 0)
            goto refresh;
    }
    SaveGameWithNamePrompt(slot, &stCampaignState,
                           aPilotRecords,
                           aMissionObjectives);

refresh:
    GetBunkInfo(state);
    DrawBarracksBunks(viewport, shape, state);
    EnterAllocationScope();
}

/* Function start: 0x41C140 */
void UpdateBarracksScreen(Viewport *viewport, unsigned char *shape,
                          BarracksAnimationState *state)
{
    DrawBarracksStaticDetails(viewport, shape);
    AnimateBarracks(viewport, shape, state);
    RefreshRoomMenuLabel();
}

/* Function start: 0x41C170 */
short BarracksScreen(void)
{
    InputEventState event;
    BarracksAnimationState animation;
    unsigned char *background;
    int lastMedalsTick;
    short clicked;
    short eventType;
    short region;
    short result;

    result = 0;
    lastMedalsTick = 0;
    PreloadMusicTrackHook(35);
    spacetrack(35, 2, 1);
    InitializeRoomViewports();
    background = FetchDiskPacketRetrying(5, 12, 0);
    InitializeRoomMenu(aBarracksMenuRegions,
                       apszBarracksMenuLabels,
                       &stRoomScreenViewport,
                       szDefaultTextBuffer, 2);
    EnsureSaveGameFile();
    InitializeBarracksAnimation(&animation);
    GetBunkInfo(&animation);
    DrawBarracksBunks(&stSceneBuffer, background, &animation);
    stMouseCursorState.viewport = &stRoomScreenViewport;
    WarpMouseTo(160, 100);
    EnterAllocationScope();
    SetFrameTimerPeriodDirect(0);
    FlushInputEvents();
    nSavedRoomControllerX =
        nMenuInputRepeatDelay;
    bInputMode = 1;
    nMenuPointerSpeed = 1;
    nMenuInputRepeatDelay = 2;

    while (result == 0) {
        if (IsFrameTickElapsed() != 0) {
            UpdateBarracksScreen(&stRoomScreenViewport,
                                 background, &animation);
            SetFrameTimerPeriodDirect(2);
        }
        eventType = PollInputEvent(&event, 0xff);
        clicked = 0;
        if (eventType == 3 || eventType == 5) {
            ClearInputKeyStatePreservingModifiers();
            if ((short)event.value == 0x1c ||
                (short)event.value == 0x39)
                clicked = 1;
            else if ((short)event.value == 0x24)
                CalibrateJoystickInteractive();
            else
                MoveMenuPointerFromKeyboard(&event);
        } else if (eventType == 2 || eventType == 10) {
            clicked = 1;
        } else if (eventType == 13) {
            UpdateRoomMenuCursor();
        }

        if (clicked != 0) {
            region = FindMenuRegionAtPoint(
                event.x, event.y, aBarracksMenuRegions);
            if (region >= 0 && region < 16) {
                HandleBarracksBunkSelection(
                    &stSceneBuffer, background, &animation, region);
            } else if (region == 16) {
                if (bCampaignActive == 0)
                    WarnLoadGameFirst();
                else
                    result = 7;
            } else if (region == 17) {
                if (bCampaignActive == 0)
                    WarnLoadGameFirst();
                else
                    result = 8;
            } else if (region == 18) {
                if (ConfirmQuitWingCommander() != 0) {
                    ShutdownEventManager();
                    exit_squadron(
                        "You step out of the airlock and into...");
                }
            } else if (region == 19) {
                if (bCampaignActive == 0) {
                    WarnLoadGameFirst();
                } else {
                    LoadMissionData(
                        (short)stCampaignState.currentSeries,
                        (short)stCampaignState.currentMission);
                    FlushInputEvents();
                    if ((int)(nTickCount60Hz - lastMedalsTick) >
                        nInputTickScale) {
                        LeaveAllocationScope();
                        ClearViewport(&stRoomScreenViewport,
                                      cBlackColour);
                        stSceneBuffer.bottom = 127;
                        stScreen.top = 24;
                        stScreen.bottom = 151;
                        ViewMedals();
                        lastMedalsTick = (int)nTickCount60Hz;
                        ClearViewport(&stRoomScreenViewport,
                                      cBlackColour);
                        stScreen.top = 0;
                        stScreen.bottom = 199;
                        stSceneBuffer.bottom = 199;
                        DrawBarracksBunks(&stSceneBuffer, background,
                                          &animation);
                        EnterAllocationScope();
                        UpdateBarracksScreen(
                            &stRoomScreenViewport,
                            background, &animation);
                    }
                }
            }
        }
        ShowMemoryStatusDebug();
        DIBslam();
        DIBslamReal();
    }

    LeaveAllocationScope();
    nMenuPointerSpeed = 2;
    nMenuInputRepeatDelay =
        nSavedRoomControllerX;
    EventManagerHook(0);
    FreeBarracksMenuLabels();
    ReleasePacketHandle(background);
    ReleaseTextFont(0);
    free_viewport(&stSceneBuffer);
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook(35);
    return result;
}

/* Function start: 0x41C510 */
unsigned short __stdcall StepPaletteTransition(short *current,
                                                const short *target,
                                                short componentCount)
{
    unsigned int byteCount;
    short difference;
    short index;
    short previousCountdown;

    if (nPaletteTransitionInitialise != 0) {
        byteCount = (unsigned int)(componentCount * 2);
        pPaletteTransitionAccumulator =
            AllocateTaggedMemory(byteCount, 0);
        pPaletteTransitionDelta =
            AllocateTaggedMemory(byteCount, 0);
        pPaletteTransitionDirection =
            AllocateTaggedMemory(byteCount, 0);
        if (pPaletteTransitionAccumulator == 0 ||
            pPaletteTransitionDelta == 0 ||
            pPaletteTransitionDirection == 0) {
            if (pPaletteTransitionAccumulator != 0)
                ReleasePacketHandle(pPaletteTransitionAccumulator);
            if (pPaletteTransitionDelta != 0)
                ReleasePacketHandle(pPaletteTransitionDelta);
            if (pPaletteTransitionDirection != 0)
                ReleasePacketHandle(pPaletteTransitionDirection);
            return 0;
        }

        nPaletteTransitionMaxDelta = 0;
        for (index = 0; index < componentCount; index++) {
            difference = (short)(current[index] - target[index]);
            if (difference < 0) {
                difference = (short)-difference;
                pPaletteTransitionDirection[index] = 4;
            } else {
                pPaletteTransitionDirection[index] = -4;
            }
            pPaletteTransitionDelta[index] = difference;
            if (nPaletteTransitionMaxDelta < difference)
                nPaletteTransitionMaxDelta = difference;
        }

        for (index = 0; index < componentCount; index++) {
            pPaletteTransitionAccumulator[index] =
                (short)(nPaletteTransitionMaxDelta / 4);
        }
        nPaletteTransitionInitialise = 0;
        nPaletteTransitionCountdown =
            (short)(nPaletteTransitionMaxDelta / 4);
    }

    previousCountdown = nPaletteTransitionCountdown;
    nPaletteTransitionCountdown--;
    if (previousCountdown == 0) {
        ReleasePacketHandle(pPaletteTransitionAccumulator);
        ReleasePacketHandle(pPaletteTransitionDelta);
        ReleasePacketHandle(pPaletteTransitionDirection);
        nPaletteTransitionInitialise = 1;
        return 0;
    }

    for (index = 0; index < componentCount; index++) {
        pPaletteTransitionAccumulator[index] =
            (short)(pPaletteTransitionAccumulator[index] +
                    pPaletteTransitionDelta[index]);
        if (pPaletteTransitionAccumulator[index] >
            nPaletteTransitionMaxDelta) {
            pPaletteTransitionAccumulator[index] =
                (short)(pPaletteTransitionAccumulator[index] -
                        nPaletteTransitionMaxDelta);
            current[index] =
                (short)(current[index] +
                        pPaletteTransitionDirection[index]);
        }
    }
    return 1;
}

/* Function start: 0x41C740 */
char *__stdcall DosStrcat(char *destination, const char *source)
{
    char *end;

    end = DosStrchr(destination, 0);
    DosStrcpy(end, source);
    return destination;
}
