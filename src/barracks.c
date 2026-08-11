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
        do {
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
            index = 0;
            do {
                diskBytes[index] = gameBytes[index];
                index++;
            } while (index < 5);

            diskBytes = diskRecord.campaign.badges;
            gameBytes = gameRecord.campaign.badges;
            index = 0;
            do {
                diskBytes[index] = gameBytes[index];
                index++;
            } while (index < 12);

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
            index = 0;
            do {
                diskBytes[index] = gameBytes[index];
                index++;
            } while (index < 8);

            gameDeathMission =
                gameRecord.campaign.personalityDeathMission;
            diskDeathMission =
                diskRecord.campaign.personalityDeathMission;
            do {
                *diskDeathMission++ = (short)*gameDeathMission++;
            } while (diskDeathMission <
                     diskRecord.campaign.personalityDeathMission + 8);

            diskBytes = diskRecord.campaign.aceFlags;
            gameBytes = gameRecord.campaign.aceFlags;
            index = 0;
            do {
                diskBytes[index] = gameBytes[index];
                index++;
            } while (index < 4);
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
            do {
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
                diskObjective++;
                gameObjective++;
            } while (diskObjective < diskRecord.objectives + 16);

            WriteDataFileAtOffset((unsigned short)file,
                                  (int)slot * 0x33c, 0x33c,
                                  &diskRecord);
            slot++;
        } while (slot < 8);
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
            (short)RandomInRange(0, 13);
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
        *label != g_apszSaveCampaignMenuLabels_004693e8[0] &&
        *label != g_apszSaveCampaignMenuLabels_004693e8[1]) {
        ReleasePacketHandle((int)*label);
        *label = 0;
    }
}

/* Function start: 0x41B110 */
void SetAwakenBarracksMenuLabel(char **label, int series, int mission,
                                char *description)
{
    FreeBarracksMenuLabel(label);
    sprintf(g_szTextScratchBuffer_00598b00, "Awaken %Fs.", description);
    *label = (char *)AllocateTaggedMemory(
        strlen(g_szTextScratchBuffer_00598b00) + 1, 0);
    DosMemcpy(*label, g_szTextScratchBuffer_00598b00,
              strlen(g_szTextScratchBuffer_00598b00) + 1);
}

/* Function start: 0x41B180 */
void FreeBarracksMenuLabels(void)
{
    short bunk;

    bunk = 0;
    do {
        if (g_apszBarracksMenuLabels_004693f0[bunk * 2] !=
            g_apszSaveCampaignMenuLabels_004693e8[0]) {
            FreeBarracksMenuLabel(
                &g_apszBarracksMenuLabels_004693f0[bunk * 2]);
            FreeBarracksMenuLabel(
                &g_apszBarracksMenuLabels_004693f0[bunk * 2 + 1]);
        }
        bunk++;
    } while (bunk < 8);
    g_pszCurrentRoomMenuLabel_00598aba = 0;
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
    index = 0;
    do {
        diskBytes[index] = gameRecord->campaign.medals[index];
        index++;
    } while (index < 5);

    diskBytes = diskRecord.campaign.badges;
    index = 0;
    do {
        diskBytes[index] = gameRecord->campaign.badges[index];
        index++;
    } while (index < 12);
    diskRecord.campaign.currentMission =
        gameRecord->campaign.currentMission;
    diskRecord.campaign.currentSeries =
        gameRecord->campaign.currentSeries;
    diskRecord.campaign.seriesHistoryCount =
        gameRecord->campaign.seriesHistoryCount;
    diskBytes = (unsigned char *)diskRecord.campaign.seriesHistory;
    index = 0;
    do {
        diskBytes[index] =
            (unsigned char)gameRecord->campaign.seriesHistory[index];
        index++;
    } while (index < 8);

    gameDeathMission = gameRecord->campaign.personalityDeathMission;
    diskDeathMission = diskRecord.campaign.personalityDeathMission;
    do {
        *diskDeathMission++ = (short)*gameDeathMission++;
    } while (diskDeathMission <
             diskRecord.campaign.personalityDeathMission + 8);

    diskBytes = diskRecord.campaign.aceFlags;
    index = 0;
    do {
        diskBytes[index] = gameRecord->campaign.aceFlags[index];
        index++;
    } while (index < 4);
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
    do {
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
        diskObjective++;
        gameObjective++;
    } while (diskObjective < diskRecord.objectives + 16);

    file = OpenDataFileOrDie("SAVEGAME.WLD");
    written = WriteDataFileAtOffset((unsigned short)file,
                                    (int)slot * 0x33c, 0x33c,
                                    &diskRecord);
    CloseDataFile((unsigned short)file);
    return (written != 0) & (file >= 0);
}

/* Function start: 0x41B420 */
short PromptForTextInput(short x, short y, char *prompt,
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
                             g_dwModalBoundsTopLeft_00469440,
                             g_dwModalBoundsBottomRight_00469444,
                             DAT_0046999c, DAT_0046999c,
                             DAT_0046999c);
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
                                 g_cViewportClearColour_004699a0,
                                 DAT_004699a4, DAT_004699ac) != 0) {
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

    if (DAT_004688f0 == 0) {
        WarnLoadGameFirst();
        return;
    }
    DosStrcpy(oldLabel,
              g_apszBarracksMenuLabels_004693f0[slot * 2]);
    if (strcmp(oldLabel,
               g_apszSaveCampaignMenuLabels_004693e8[0]) == 0)
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
    index = 0;
    do {
        gameBytes[index] = diskBytes[index];
        index++;
    } while (index < 5);

    gameBytes = gameRecord->campaign.badges;
    diskBytes = diskRecord.campaign.badges;
    index = 0;
    do {
        gameBytes[index] = diskBytes[index];
        index++;
    } while (index < 12);
    gameRecord->campaign.currentMission =
        diskRecord.campaign.currentMission;
    gameRecord->campaign.currentSeries =
        diskRecord.campaign.currentSeries;
    gameRecord->campaign.seriesHistoryCount =
        diskRecord.campaign.seriesHistoryCount;
    gameBytes = (unsigned char *)gameRecord->campaign.seriesHistory;
    diskBytes = (unsigned char *)diskRecord.campaign.seriesHistory;
    index = 0;
    do {
        gameBytes[index] = diskBytes[index];
        index++;
    } while (index < 8);

    gameDeathMission = gameRecord->campaign.personalityDeathMission;
    diskDeathMission = diskRecord.campaign.personalityDeathMission;
    do {
        *gameDeathMission++ = (int)*diskDeathMission++;
    } while (diskDeathMission <
             diskRecord.campaign.personalityDeathMission + 8);

    gameBytes = gameRecord->campaign.aceFlags;
    diskBytes = diskRecord.campaign.aceFlags;
    index = 0;
    do {
        gameBytes[index] = diskBytes[index];
        index++;
    } while (index < 4);
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
    do {
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
        diskObjective++;
        gameObjective++;
    } while (diskObjective < diskRecord.objectives + 16);

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
            DAT_005a8114 = campaign->campaignIndex;
            g_nCampaignDataSet_005a8118 = DAT_005a8114;
            LoadPacketIntoBuffer(
                g_asCampaignPilotFiles_00469450[DAT_005a8114], 0,
                g_pConstellationDefinitions_00598a28);
            LoadPacketIntoBuffer(
                g_asCampaignPilotFiles_00469450[
                    g_nCampaignDataSet_005a8118],
                1, g_pMissionCampaignData_005988bc);
            DosMemcpy(objectives, gameRecord.objectives, 0x1f0);
            CorrectPointers();
            DAT_004688f0 = 1;
        }
        ReleaseModalTextPanel();
        if (loaded == 0) {
            ShowModalMessage("Error: Game %d not loaded.", (int)slot);
        } else if (g_nOriginDevUnlock_00469ff4 != 0) {
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
        *label = (char *)g_apszSaveCampaignMenuLabels_004693e8[0];
        return;
    }
    if (side == 0) {
        SetAwakenBarracksMenuLabel(label, series, mission, description);
        return;
    }
    *label = (char *)g_apszSaveCampaignMenuLabels_004693e8[1];
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
            &g_apszBarracksMenuLabels_004693f0[bunk * 2],
            (int)gameRecord.campaign.currentSeries,
            (int)gameRecord.campaign.currentMission,
            gameRecord.description);
        SetBunkMenuLabel(
            state->bunks[bunk].occupied, 1,
            &g_apszBarracksMenuLabels_004693f0[bunk * 2 + 1],
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

    bunk = 0;
    DrawSpriteDefault(viewport, 0, 0, shape, 0);
    do {
        frame = 10;
        if (state->bunks[bunk].occupied != 0) {
            DrawSpriteDefault(
                viewport, g_aBarracksBunkOrigins_004693c8[bunk].x,
                g_aBarracksBunkOrigins_004693c8[bunk].y,
                shape, (short)(bunk + 1));
            frame = 9;
        }
        DrawSpriteDefault(viewport,
                          (short)((bunk % 2) * 31 + 143),
                          (short)((bunk / 2) * 5 + 167),
                          shape, frame);
        bunk++;
    } while (bunk < 8);
    CopyViewportContents(&DAT_005a76b0,
                         &g_stRoomScreenViewport_005988a0);
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

    frameTick = (int)DAT_0059ab54 / 3;
    bunk = 0;
    do {
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
        bunk++;
    } while (bunk < 8);

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
        if (state->blinkDelay == 0 && RandomInRange(0, 100) < 90)
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
    if (state->menuLabel != g_pszCurrentRoomMenuLabel_00598aba) {
        state->menuLabel = g_pszCurrentRoomMenuLabel_00598aba;
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
        confirmed = (short)(toupper((int)WaitForStreamInputKey()) == 'Y');
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
        confirmed = (short)(toupper((int)WaitForStreamInputKey()) == 'Y');
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
    if (DAT_004688f0 == 0) {
        WarnLoadGameFirst();
        return 0;
    }
    if (LoadGame(slot, &gameRecord) == 0)
        memcpy(gameRecord.description, "FAULTY DATA", 12);
    LeaveAllocationScope();
    if (ShowModalTextPanel(0, "Replace %s? (Y/N)",
                           gameRecord.description) != 0) {
        confirmed = (short)(toupper((int)WaitForStreamInputKey()) == 'Y');
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
                    slot, &g_stCampaignState_0059ca50,
                    g_aPilotRecords_005988d0,
                    g_aMissionObjectives_0059dac0);
            }
            goto refresh;
        }
        if (ConfirmReplaceFaultyData(slot) == 0)
            goto refresh;
    }
    SaveGameWithNamePrompt(slot, &g_stCampaignState_0059ca50,
                           g_aPilotRecords_005988d0,
                           g_aMissionObjectives_0059dac0);

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
    StartMusicTrack(35, 2, 1);
    InitializeRoomViewports();
    background = (unsigned char *)FetchDiskPacketRetrying(5, 12, 0);
    InitializeRoomMenu(g_aBarracksMenuRegions_00463008,
                       g_apszBarracksMenuLabels_004693f0,
                       &g_stRoomScreenViewport_005988a0,
                       g_szDefaultTextBuffer_005a7590, 2);
    EnsureSaveGameFile();
    InitializeBarracksAnimation(&animation);
    GetBunkInfo(&animation);
    DrawBarracksBunks(&DAT_005a76b0, background, &animation);
    DAT_0059ab23 = &g_stRoomScreenViewport_005988a0;
    WarpMouseTo(160, 100);
    EnterAllocationScope();
    SetFrameTimerPeriodDirect(0);
    FlushInputEvents();
    g_nSavedRoomControllerX_005988b4 =
        *(short *)&g_aInputDeviceSamples_005a81f0[2].x;
    g_bInputMode_0059a848 = 1;
    g_nMenuPointerSpeed_0046af58 = 1;
    *(short *)&g_aInputDeviceSamples_005a81f0[2].x = 2;

    while (result == 0) {
        if (IsFrameTickElapsed() != 0) {
            UpdateBarracksScreen(&g_stRoomScreenViewport_005988a0,
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
                event.x, event.y, g_aBarracksMenuRegions_00463008);
            if (region >= 0 && region < 16) {
                HandleBarracksBunkSelection(
                    &DAT_005a76b0, background, &animation, region);
            } else if (region == 16) {
                if (DAT_004688f0 == 0)
                    WarnLoadGameFirst();
                else
                    result = 7;
            } else if (region == 17) {
                if (DAT_004688f0 == 0)
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
                if (DAT_004688f0 == 0) {
                    WarnLoadGameFirst();
                } else {
                    LoadMissionData(
                        (short)g_stCampaignState_0059ca50.currentSeries,
                        (short)g_stCampaignState_0059ca50.currentMission);
                    FlushInputEvents();
                    if ((int)(DAT_0059ab54 - lastMedalsTick) >
                        g_nInputTickScale_0059af90) {
                        LeaveAllocationScope();
                        ClearViewport(&g_stRoomScreenViewport_005988a0,
                                      DAT_0046999c);
                        DAT_005a76b0.bottom = 127;
                        DAT_005a6ba0.top = 24;
                        DAT_005a6ba0.bottom = 151;
                        ViewMedals();
                        lastMedalsTick = (int)DAT_0059ab54;
                        ClearViewport(&g_stRoomScreenViewport_005988a0,
                                      DAT_0046999c);
                        DAT_005a6ba0.top = 0;
                        DAT_005a6ba0.bottom = 199;
                        DAT_005a76b0.bottom = 199;
                        DrawBarracksBunks(&DAT_005a76b0, background,
                                          &animation);
                        EnterAllocationScope();
                        UpdateBarracksScreen(
                            &g_stRoomScreenViewport_005988a0,
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
    g_nMenuPointerSpeed_0046af58 = 2;
    *(short *)&g_aInputDeviceSamples_005a81f0[2].x =
        g_nSavedRoomControllerX_005988b4;
    EventManagerHook(0);
    FreeBarracksMenuLabels();
    ReleasePacketHandle((int)background);
    ReleaseTextFont(0);
    free_viewport(&DAT_005a76b0);
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

    if (g_nPaletteTransitionInitialise_00469640 != 0) {
        byteCount = (unsigned int)(componentCount * 2);
        g_pPaletteTransitionAccumulator_005a7d94 =
            (short *)AllocateTaggedMemory(byteCount, 0);
        g_pPaletteTransitionDelta_005a7d8c =
            (short *)AllocateTaggedMemory(byteCount, 0);
        g_pPaletteTransitionDirection_005a7d88 =
            (short *)AllocateTaggedMemory(byteCount, 0);
        if (g_pPaletteTransitionAccumulator_005a7d94 == 0 ||
            g_pPaletteTransitionDelta_005a7d8c == 0 ||
            g_pPaletteTransitionDirection_005a7d88 == 0) {
            if (g_pPaletteTransitionAccumulator_005a7d94 != 0)
                ReleasePacketHandle(
                    (int)g_pPaletteTransitionAccumulator_005a7d94);
            if (g_pPaletteTransitionDelta_005a7d8c != 0)
                ReleasePacketHandle((int)g_pPaletteTransitionDelta_005a7d8c);
            if (g_pPaletteTransitionDirection_005a7d88 != 0)
                ReleasePacketHandle(
                    (int)g_pPaletteTransitionDirection_005a7d88);
            return 0;
        }

        g_nPaletteTransitionMaxDelta_005a7d90 = 0;
        index = 0;
        while (index < componentCount) {
            difference = (short)(current[index] - target[index]);
            if (difference < 0) {
                difference = (short)-difference;
                g_pPaletteTransitionDirection_005a7d88[index] = 4;
            } else {
                g_pPaletteTransitionDirection_005a7d88[index] = -4;
            }
            g_pPaletteTransitionDelta_005a7d8c[index] = difference;
            if (g_nPaletteTransitionMaxDelta_005a7d90 < difference)
                g_nPaletteTransitionMaxDelta_005a7d90 = difference;
            index++;
        }

        index = 0;
        while (index < componentCount) {
            g_pPaletteTransitionAccumulator_005a7d94[index] =
                (short)(g_nPaletteTransitionMaxDelta_005a7d90 / 4);
            index++;
        }
        g_nPaletteTransitionInitialise_00469640 = 0;
        g_nPaletteTransitionCountdown_005a7d98 =
            (short)(g_nPaletteTransitionMaxDelta_005a7d90 / 4);
    }

    previousCountdown = g_nPaletteTransitionCountdown_005a7d98;
    g_nPaletteTransitionCountdown_005a7d98--;
    if (previousCountdown == 0) {
        ReleasePacketHandle((int)g_pPaletteTransitionAccumulator_005a7d94);
        ReleasePacketHandle((int)g_pPaletteTransitionDelta_005a7d8c);
        ReleasePacketHandle((int)g_pPaletteTransitionDirection_005a7d88);
        g_nPaletteTransitionInitialise_00469640 = 1;
        return 0;
    }

    index = 0;
    while (index < componentCount) {
        g_pPaletteTransitionAccumulator_005a7d94[index] =
            (short)(g_pPaletteTransitionAccumulator_005a7d94[index] +
                    g_pPaletteTransitionDelta_005a7d8c[index]);
        if (g_pPaletteTransitionAccumulator_005a7d94[index] >
            g_nPaletteTransitionMaxDelta_005a7d90) {
            g_pPaletteTransitionAccumulator_005a7d94[index] =
                (short)(g_pPaletteTransitionAccumulator_005a7d94[index] -
                        g_nPaletteTransitionMaxDelta_005a7d90);
            current[index] =
                (short)(current[index] +
                        g_pPaletteTransitionDirection_005a7d88[index]);
        }
        index++;
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
