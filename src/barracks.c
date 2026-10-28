/*
 *  Barracks and campaign save/load room.
 *
 *  Address range 41a9d0-41c50f (see docs/ORDER.md).
 *  Boundary evidence: the menu tables precede the modal renderer's "%P"
 *  literal, then the SAVEGAME.WLD literals; the following palette flag
 *  starts a separate unit after this file's string pool.
 */
#define WC1_BARRACKS_SOURCE
#include "wc1.h"
#ifndef SDL_PORT
#include <dos.h>
#include <process.h>
#endif
#include <sys/stat.h>
#include <assert.h>

const TitleMenuRegion aBarracksMenuRegions[21] = {
    { 1, 137, 88, 149, 94 },
    { 1, 110, 88, 136, 94 },
    { 1, 172, 88, 184, 94 },
    { 1, 185, 88, 210, 94 },
    { 1, 133, 98, 146, 107 },
    { 1, 100, 98, 132, 107 },
    { 1, 174, 98, 189, 107 },
    { 1, 190, 98, 220, 107 },
    { 1, 124, 114, 142, 128 },
    { 1, 81, 114, 123, 128 },
    { 1, 178, 114, 197, 128 },
    { 1, 198, 114, 238, 128 },
    { 1, 109, 141, 135, 164 },
    { 1, 50, 141, 108, 164 },
    { 1, 185, 141, 213, 164 },
    { 1, 214, 141, 268, 164 },
    { 1, 288, 39, 311, 85 },
    { 1, 9, 33, 39, 95 },
    { 1, 218, 37, 248, 78 },
    { 1, 86, 44, 181, 78 },
    { -1, 0, 0, 0, 0 }
}; /* 0x00463008 */

ShortPoint aBarracksBunkOrigins[8] = {
    {109, 86}, {170, 86}, {98, 95}, {173, 95},
    {78, 110}, {176, 110}, {42, 136}, {183, 136}
}; /* 0x004693c8 */
const char *apszSaveCampaignMenuLabels[2] = {
    "Save this campaign  ",
    "Save this campaign  "
}; /* 0x004693e8 */
char *apszBarracksMenuLabels[20] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    "Mission Hangar",
    "Return to the Bar",
    "Quit Wing Commander",
    "View your medals"
}; /* 0x004693f0 */
ShortRect stModalTextBounds = { 24, 40, 296, 60 }; /* 0x00469440 */
ModalTextPanel *pModalTextPanel = 0; /* 0x00469448 */
short asCampaignPilotFiles[3] = {58, 61, 74}; /* 0x00469450 */
short asCampaignBriefingFiles[3] = {10, 62, 73}; /* 0x00469458 */
short asMissionDataFiles[3] = {15, 52, 72}; /* 0x00469460 */

/* Linker commons defined here; LINK orders them by identifier hash. */
int DAT_005a7d9c;

/* Function start: 0x41A9D0 */
short InitializeModalTextPanel(ModalTextPanel *panel, short fontIndex,
                               ShortRect bounds,
                               short clearColour,
                               short backgroundColour,
                               short borderColour)
{
    *(ShortRect *)&panel->left = bounds;
    panel->previousContext = pCurTextContext;
    pCurTextContext = &panel->context;
    panel->context = stDefaultTxtContext;
    if (fontIndex == -1)
        fontIndex = 1;
    InitializeTextContextFromFont(&panel->context, fontIndex,
                                  (unsigned char)clearColour,
                                  (signed char)backgroundColour);
    panel->viewport = stModalSrcViewport;
    *(ShortRect *)&panel->viewport.left =
        *(ShortRect *)&panel->savedBackground.left =
        *(ShortRect *)&panel->left;
    if (AllocateViewport(&panel->savedBackground, clearColour, 0) == 0)
        return 0;
    CopyViewportContents(&panel->viewport, &panel->savedBackground);
    panel->context.text = szTxtWorkspaceBuffer;
    panel->context.viewport = &panel->viewport;
    ResetStringBuilder(&panel->context);
    EraseTextContextBackground(&panel->context);
    DrawViewportBorder(&panel->viewport, panel->left, panel->top,
                       panel->right, panel->bottom, borderColour);
    return 1;
}

/* Function start: 0x41AAE0 */
void DrawModalTextPanel(ModalTextPanel *panel, short x, short y,
                        unsigned char alignment,
                        const char *format, ...)
{
    char text[84];

#ifdef SDL_PORT
    va_list arguments;

    va_start(arguments, format);
    vsprintf(text, format, arguments);
    va_end(arguments);
#else
    vsprintf(text, format, (char *)(&format + 1));
#endif
    SetTextCursor((unsigned short)(panel->left + x),
                  (unsigned short)(panel->top + y));
    panel->context.alignment = alignment;
    strcat(text, "%P");
    FormatTextBufferFromStart(text);
}

/* Function start: 0x41AB60 */
void RestoreModalTextPanel(ModalTextPanel *panel)
{
    CopyViewportContents(&panel->savedBackground, &panel->viewport);
    free_viewport(&panel->savedBackground);
    pCurTextContext = panel->previousContext;
}

/* Function start: 0x41AB90 */
short ShowModalTextPanel(short fontIndex, const char *format, ...)
{
    ShortRect bounds;
    short halfWidth;
    char text[52];

#ifdef SDL_PORT
    va_list arguments;

    va_start(arguments, format);
    vsprintf(text, format, arguments);
    va_end(arguments);
#else
    vsprintf(text, format, (char *)(&format + 1));
#endif
    bounds = stModalTextBounds;
    if (pModalTextPanel == 0) {
        pModalTextPanel = AllocateTaggedMemory(sizeof(ModalTextPanel), 0);
    }
    if (pModalTextPanel == 0)
        return 0;
    if (InitializeModalTextPanel(pModalTextPanel, fontIndex,
                                 stModalTextBounds,
                                 cBlackColour, cBlackColour,
                                 cBlackColour) == 0) {
        ReleasePacketHandle(pModalTextPanel);
        pModalTextPanel = 0;
        return 0;
    }
    halfWidth = MeasureTextPixelWidthClamped(text);
    halfWidth = (short)((int)halfWidth * 8 / 16);
    RestoreModalTextPanel(pModalTextPanel);
    bounds.left = (short)(159 - halfWidth);
    bounds.right = (short)(161 + halfWidth);
    if (InitializeModalTextPanel(pModalTextPanel, fontIndex,
                                 bounds, cViewportClearColour,
                                 cBlueColour, cRedColour) == 0) {
        ReleasePacketHandle(pModalTextPanel);
        pModalTextPanel = 0;
        return 0;
    }
    DrawModalTextPanel(pModalTextPanel, 0, 6, 2, text);
    DIBslam();
    DIBslamReal();
    return 1;
}

/* Function start: 0x41AD10 */
void ReleaseModalTextPanel(void)
{
    if (pModalTextPanel != 0) {
        RestoreModalTextPanel(pModalTextPanel);
        ReleasePacketHandle(pModalTextPanel);
        pModalTextPanel = 0;
        DIBslam();
        DIBslamReal();
    }
}

/* Function start: 0x41AD50 */
short AnySavedGames(void)
{
    SaveGameRecord gameRecord;
    short slot;
    short found;

    found = 0;
    slot = 0;
    do {
        if (LoadGame(slot, &gameRecord) != 0) {
            found = 1;
            if (gameRecord.campaign.campaignIndex > 0)
                DAT_005a7d9c = 1;
        }
        slot++;
    } while (slot < 8);
    return found;
}

/* Function start: 0x41ADA0 */
void CreateEmptySaveGameFile(void)
{
    SaveGameDiskRecord diskRecord;
    SaveGameRecord gameRecord;
    CampaignState *gameCampaign = &gameRecord.campaign;
    SaveGameDiskObjective *diskObjective;
    MissionObjective *gameObjective;
    unsigned char *diskBytes;
    unsigned char *gameBytes;
    short *diskDeathMission;
    int *gameDeathMission;
    short file;
    short slot;
    int index;
    SaveGameDiskCampaignState *diskCampaign = &diskRecord.campaign;

    slot = 0;
    gameRecord.occupied = 0;
    PromptInsertNumberedDisk(0x10);
    file = CreateDataFile("SAVEGAME.WLD");
    if (file >= 0) {
        for (; slot < 8; slot++) {
            sprintf(gameRecord.description, "game %d", (int)slot + 1);
            diskBytes = (unsigned char *)diskRecord.description;
            gameBytes = (unsigned char *)gameRecord.description;
            memcpy(diskBytes, gameBytes, sizeof(diskRecord.description));
            diskRecord.occupied = gameRecord.occupied;
            memcpy(diskRecord.pilots, gameRecord.pilots,
                   sizeof(diskRecord.pilots));
            diskCampaign->currentPilot =
                (short)(int)gameCampaign->currentPilot;
            diskCampaign->playerShipType =
                (short)gameCampaign->playerShipType;

            diskBytes = diskCampaign->medals;
            gameBytes = gameCampaign->medals;
            for (index = 0; index < 5; index++) {
                diskBytes[index] = gameBytes[index];
            }

            diskBytes = diskCampaign->badges;
            gameBytes = gameCampaign->badges;
            for (index = 0; index < 12; index++) {
                diskBytes[index] = gameBytes[index];
            }

            diskCampaign->currentMission =
                gameCampaign->currentMission;
            diskCampaign->currentSeries =
                gameCampaign->currentSeries;
            diskCampaign->seriesHistoryCount =
                gameCampaign->seriesHistoryCount;
            diskBytes = (unsigned char *)
                diskCampaign->seriesHistory;
            gameBytes = (unsigned char *)
                gameCampaign->seriesHistory;
            for (index = 0; index < 8; index++) {
                diskBytes[index] = gameBytes[index];
            }

            gameDeathMission =
                gameCampaign->personalityDeathMission;
            diskDeathMission =
                diskCampaign->personalityDeathMission;
            for (; diskDeathMission <
                       diskCampaign->personalityDeathMission + 8;
                 gameDeathMission++, diskDeathMission++) {
                *diskDeathMission = (short)*gameDeathMission;
            }

            diskBytes = diskCampaign->aceFlags;
            gameBytes = gameCampaign->aceFlags;
            for (index = 0; index < 4; index++) {
                diskBytes[index] = gameBytes[index];
            }
            diskCampaign->currentDate =
                gameCampaign->currentDate;
            diskCampaign->elapsedDate.day =
                gameCampaign->elapsedDate.day;
            diskCampaign->elapsedDate.year =
                gameCampaign->elapsedDate.year;
            diskCampaign->promotionScore =
                gameCampaign->promotionScore;
            diskCampaign->missionScore =
                gameCampaign->missionScore;
            diskCampaign->seriesScore =
                gameCampaign->seriesScore;
            diskCampaign->campaignIndex =
                gameCampaign->campaignIndex;

            gameObjective = gameRecord.objectives;
            diskObjective = diskRecord.objectives;
            for (; diskObjective < diskRecord.objectives + 16;
                 gameObjective++, diskObjective++) {
                diskObjective->mapX = gameObjective->mapX;
                diskObjective->mapY = gameObjective->mapY;
                diskObjective->field_4 = gameObjective->field_4;
                diskObjective->type = (short)gameObjective->type;
                diskObjective->index = gameObjective->index;
                diskObjective->flags = gameObjective->flags;
                diskObjective->displayName =
                    (short)(int)gameObjective->displayName;
                diskObjective->name = (short)(int)gameObjective->name;
                gameBytes = (unsigned char *)&gameObjective->position;
                diskBytes = (unsigned char *)&diskObjective->position;
                ((FixedVector *)diskBytes)->x = ((FixedVector *)gameBytes)->x;
                ((FixedVector *)diskBytes)->y = ((FixedVector *)gameBytes)->y;
                ((FixedVector *)diskBytes)->z = ((FixedVector *)gameBytes)->z;
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
    length = filelength((int)file);
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
void SetAwakenBarracksMenuLabel(char **label, short series, short mission,
                                char *description)
{
    FreeBarracksMenuLabel(label);
#ifdef SDL_PORT
    /* MSVC 4.20 accepts %Fs as its legacy far-string conversion. */
    sprintf(szTxtWorkspaceBuffer, "Awaken %s.", description);
#else
    sprintf(szTxtWorkspaceBuffer, "Awaken %Fs.", description);
#endif
    *label = AllocateTaggedMemory(
        strlen(szTxtWorkspaceBuffer) + 2, 0);
    DosMemcpy(*label, szTxtWorkspaceBuffer,
              strlen(szTxtWorkspaceBuffer) + 2);
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
    stCurrentRoomMenu.currentLabel = 0;
}

/* Function start: 0x41B1E0 */
short SaveGame(short slot, SaveGameRecord *gameRecord)
{
    int index;
    SaveGameDiskRecord diskRecord;
    SaveGameDiskCampaignState *diskCampaign = &diskRecord.campaign;
    SaveGameDiskObjective *diskObjective;
    MissionObjective *gameObjective;
    unsigned char *diskBytes;
    short *diskDeathMission;
    int *gameDeathMission;
    short file;
    int written;

    PromptInsertNumberedDisk(0x10);
    diskBytes = (unsigned char *)diskRecord.description;
    memcpy(diskBytes, gameRecord->description,
           sizeof(diskRecord.description));
    diskRecord.occupied = gameRecord->occupied;
    memcpy(diskRecord.pilots, gameRecord->pilots,
           sizeof(diskRecord.pilots));
    diskCampaign->currentPilot =
        (short)(int)gameRecord->campaign.currentPilot;
    diskCampaign->playerShipType =
        (short)gameRecord->campaign.playerShipType;
    diskBytes = diskCampaign->medals;
    for (index = 0; index < 5; index++) {
        diskBytes[index] = gameRecord->campaign.medals[index];
    }

    diskBytes = diskCampaign->badges;
    for (index = 0; index < 12; index++) {
        diskBytes[index] = gameRecord->campaign.badges[index];
    }
    diskCampaign->currentMission =
        gameRecord->campaign.currentMission;
    diskCampaign->currentSeries =
        gameRecord->campaign.currentSeries;
    diskCampaign->seriesHistoryCount =
        gameRecord->campaign.seriesHistoryCount;
    diskBytes = (unsigned char *)diskCampaign->seriesHistory;
    for (index = 0; index < 8; index++) {
        diskBytes[index] =
            (unsigned char)gameRecord->campaign.seriesHistory[index];
    }

    gameDeathMission = gameRecord->campaign.personalityDeathMission;
    diskDeathMission = diskCampaign->personalityDeathMission;
    for (; diskDeathMission <
               diskCampaign->personalityDeathMission + 8;
         gameDeathMission++, diskDeathMission++) {
        *diskDeathMission = (short)*gameDeathMission;
    }

    diskBytes = diskCampaign->aceFlags;
    for (index = 0; index < 4; index++) {
        diskBytes[index] = gameRecord->campaign.aceFlags[index];
    }
    diskCampaign->currentDate =
        gameRecord->campaign.currentDate;
    diskCampaign->elapsedDate.day =
        gameRecord->campaign.elapsedDate.day;
    diskCampaign->elapsedDate.year =
        gameRecord->campaign.elapsedDate.year;
    diskCampaign->promotionScore =
        gameRecord->campaign.promotionScore;
    diskCampaign->missionScore =
        gameRecord->campaign.missionScore;
    diskCampaign->seriesScore =
        gameRecord->campaign.seriesScore;
    diskCampaign->campaignIndex =
        gameRecord->campaign.campaignIndex;

    gameObjective = gameRecord->objectives;
    diskObjective = diskRecord.objectives;
    for (; diskObjective < diskRecord.objectives + 16;
         gameObjective++, diskObjective++) {
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
    return ((written != 0) & (file >= 0)) & 1;
}

/* Function start: 0x41B420 */
short PromptForTextInput(short x, short y, const char *prompt,
                         char *destination, short maximumLength,
                         short inputMode)
{
    ModalTextPanel panel;
    ShortRect bounds;
    short result;

    result = 0;
    bounds.top = y;
    bounds.left = x;
    bounds.bottom = (short)(y + 20);
    InitializeModalTextPanel(&panel, 0,
                             stModalTextBounds,
                             cBlackColour, cBlackColour,
                             cBlackColour);
    bounds.right = (short)(x +
        ((int)MeasureTextPixelWidthClamped("M") * (int)maximumLength +
         (int)MeasureTextPixelWidthClamped(prompt)) * 16 / 15);
    RestoreModalTextPanel(&panel);
    if (InitializeModalTextPanel(&panel, 0,
                                 bounds,
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
        gameRecord.campaign = *campaign;
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
    int index;
    SaveGameDiskRecord diskRecord;
    SaveGameDiskObjective *diskObjective;
    MissionObjective *gameObjective;
    unsigned char *diskBytes;
    int *gameDeathMission;
    short *diskDeathMission;
    short file;
    short fileOpen;
    int read;
    SaveGameDiskCampaignState *diskCampaign = &diskRecord.campaign;

    PromptInsertNumberedDisk(0x10);
    file = OpenDataFileOrDie("SAVEGAME.WLD");
    fileOpen = (file >= 0) & 1;
    read = ReadDataFileAtOffset((unsigned short)file,
                                (int)slot * 0x33c, 0x33c,
                                &diskRecord);
    diskBytes = (unsigned char *)diskRecord.description;
    memcpy(gameRecord->description, diskBytes,
           sizeof(gameRecord->description));
    gameRecord->occupied = ((SaveGameDiskRecord *)diskBytes)->occupied;
    memcpy(gameRecord->pilots, diskRecord.pilots,
           sizeof(gameRecord->pilots));
    gameRecord->campaign.currentPilot =
        (PilotRecord *)(int)diskCampaign->currentPilot;
    gameRecord->campaign.playerShipType =
        (enum ObjectType)diskCampaign->playerShipType;
    diskBytes = diskCampaign->medals;
    for (index = 0; index < 5; index++) {
        gameRecord->campaign.medals[index] = diskBytes[index];
    }

    diskBytes = diskCampaign->badges;
    for (index = 0; index < 12; index++) {
        gameRecord->campaign.badges[index] = diskBytes[index];
    }
    gameRecord->campaign.currentMission =
        diskCampaign->currentMission;
    gameRecord->campaign.currentSeries =
        diskCampaign->currentSeries;
    gameRecord->campaign.seriesHistoryCount =
        diskCampaign->seriesHistoryCount;
    diskBytes = (unsigned char *)diskCampaign->seriesHistory;
    for (index = 0; index < 8; index++) {
        gameRecord->campaign.seriesHistory[index] = diskBytes[index];
    }

    diskDeathMission = diskCampaign->personalityDeathMission;
    gameDeathMission = gameRecord->campaign.personalityDeathMission;
    for (; diskDeathMission <
               diskCampaign->personalityDeathMission + 8;
         diskDeathMission++, gameDeathMission++) {
        *gameDeathMission = (int)*diskDeathMission;
    }

    diskBytes = diskCampaign->aceFlags;
    for (index = 0; index < 4; index++) {
        gameRecord->campaign.aceFlags[index] = diskBytes[index];
    }
    gameRecord->campaign.currentDate =
        diskCampaign->currentDate;
    gameRecord->campaign.elapsedDate.day =
        diskCampaign->elapsedDate.day;
    gameRecord->campaign.elapsedDate.year =
        diskCampaign->elapsedDate.year;
    gameRecord->campaign.promotionScore =
        diskCampaign->promotionScore;
    gameRecord->campaign.missionScore =
        diskCampaign->missionScore;
    gameRecord->campaign.seriesScore =
        diskCampaign->seriesScore;
    gameRecord->campaign.campaignIndex =
        diskCampaign->campaignIndex;

    diskObjective = diskRecord.objectives;
    gameObjective = gameRecord->objectives;
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
    short loaded;
    SaveGameRecord gameRecord;

    LeaveAllocationScope();
    if (ShowModalTextPanel(0, "Loading Game...") != 0) {
        loaded = LoadGame(slot, &gameRecord);
        if (loaded != 0) {
            memcpy(pilots, gameRecord.pilots,
                   sizeof(gameRecord.pilots));
            *campaign = gameRecord.campaign;
            nCampaignDataSet = campaign->campaignIndex;
            nQueuedCampaignIndex = nCampaignDataSet;
            LoadPacketIntoBuffer(
                asCampaignPilotFiles[nCampaignDataSet], 0,
                pConstellationDefinitionsPtr);
            LoadPacketIntoBuffer(
                asCampaignPilotFiles[
                    nCampaignDataSet],
                1, pMissionCampaignInfo);
            DosMemcpy(objectives, gameRecord.objectives, 0x1f0);
            CorrectPointers();
            bCampaignActive = 1;
        }
        ReleaseModalTextPanel();
        if (loaded == 0) {
            ShowModalMessage("Error: Game %d not loaded.", (int)slot);
        } else if (nOriginDevUnlock != 0) {
            strcpy(campaign->currentPilot->callsign, "CHEATER");
        }
    }
    EnterAllocationScope();
}

/* Function start: 0x41BAD0 */
void SetBunkMenuLabel(short occupied, short side, char **label,
                      short series, short mission, char *description)
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
            gameRecord.campaign.currentSeries,
            gameRecord.campaign.currentMission,
            gameRecord.description);
        SetBunkMenuLabel(
            state->bunks[bunk].occupied, 1,
            &apszBarracksMenuLabels[bunk * 2 + 1],
            gameRecord.campaign.currentSeries,
            gameRecord.campaign.currentMission,
            gameRecord.description);
        bunk++;
    } while (bunk < 8);
}

/* Function start: 0x41BBD0 */
void DrawBarracksBunks(Viewport *viewport, unsigned char *shape,
                       BarracksAnimationState *state)
{
    short frame;
    short bunk;

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
                          (short)((short)(bunk / 2) * 5 + 167),
                          shape, frame);
        bunk++;
    } while (bunk < 8);
    CopyViewportContents(&stSceneBuffer, &stRoomScreenViewport);
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
            {
                short animationFrame;

                animationFrame = bunkState->animationFrame;
                if (animationFrame < 13)
                    frame = (short)(24 - animationFrame);
                else
                    frame = 24;
            }
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
        DrawSpriteDefault(viewport, 45, 0, shape, 49);
    else
        DrawSpriteDefault(viewport, 45, 0, shape, 26);
    CheckCursor();
    if (state->menuLabel != stCurrentRoomMenu.currentLabel) {
        state->menuLabel = stCurrentRoomMenu.currentLabel;
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
        strcpy(gameRecord.description, "FAULTY DATA");
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
                    aPilotEntriesList,
                    aMissionObjectivesArray);
            }
            goto refresh;
        }
        if (ConfirmReplaceFaultyData(slot) == 0)
            goto refresh;
    }
    SaveGameWithNamePrompt(slot, &stCampaignState,
                           aPilotEntriesList,
                           aMissionObjectivesArray);

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
    int lastMedalsTick;
    short clicked;
    short result;
    unsigned char *background;
    short eventType;
    short region;

    result = 0;
    lastMedalsTick = 0;
    PreloadMusicTrackHook(35);
    spacetrack(35, 2, 1);
    InitializeRoomViewports();
    background = FetchDiskPacketRetrying(5, 12, 0);
    InitializeRoomMenu(aBarracksMenuRegions,
                       apszBarracksMenuLabels,
                       &stRoomScreenViewport,
                       szDefaultTxtBuf, 2);
    EnsureSaveGameFile();
    InitializeBarracksAnimation(&animation);
    GetBunkInfo(&animation);
    DrawBarracksBunks(&stSceneBuffer, background, &animation);
    stMouseCursorState.viewport = &stRoomScreenViewport;
    WarpMouseTo(160, 100);
    EnterAllocationScope();
    SetFrameTimerPeriodDirect(0);
    FlushInputEvents();
    bInputModeState = 1;
    nMenuPointerSpeed = 1;
    nSavedRoomCtrlX =
        nMenuInputRepeatDelay;
    nMenuInputRepeatDelay = 2;

    while (result == 0) {
        if ((short)IsFrameTickElapsed() != 0) {
            UpdateBarracksScreen(&stRoomScreenViewport,
                                 background, &animation);
            SetFrameTimerPeriodDirect(2);
        }
        eventType = PollInputEvent(&event, 0xff);
        switch (eventType) {
        case 3:
        case 5:
            clicked = 0;
            ClearInputKeyStatePreservingModifiers();
            switch ((short)event.value) {
            case 0x1c:
            case 0x39:
                clicked = 1;
                break;
            case 0x24:
                CalibrateJoystickInteractive();
                break;
            default:
                MoveMenuPointerFromKeyboard(&event);
                break;
            }
            if (clicked == 0)
                break;
            /* Fall through to the shared mouse/keyboard activation. */
        case 2:
        case 10:
            region = FindMenuRegionAtPoint(
                event.x, event.y, aBarracksMenuRegions);
            if (region >= 0 && region < 16) {
                HandleBarracksBunkSelection(
                    &stSceneBuffer, background, &animation, region);
            } else {
                switch (region) {
                case 16:
                    if (bCampaignActive == 0)
                        WarnLoadGameFirst();
                    else
                        result = 7;
                    break;
                case 17:
                    if (bCampaignActive == 0)
                        WarnLoadGameFirst();
                    else
                        result = 8;
                    break;
                case 18:
                    if (ConfirmQuitWingCommander() != 0) {
                        ShutdownEventManager();
                        exit_squadron(
                            "You step out of the airlock and into...");
                    }
                    break;
                case 19:
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
                            stScreenView.top = 24;
                            stScreenView.bottom = 151;
                            ViewMedals();
                            lastMedalsTick = (int)nTickCount60Hz;
                            ClearViewport(&stRoomScreenViewport,
                                          cBlackColour);
                            stScreenView.top = 0;
                            stScreenView.bottom = 199;
                            stSceneBuffer.bottom = 199;
                            DrawBarracksBunks(&stSceneBuffer, background,
                                              &animation);
                            EnterAllocationScope();
                            UpdateBarracksScreen(
                                &stRoomScreenViewport,
                                background, &animation);
                        }
                    }
                    break;
                }
            }
            break;
        case 13:
            UpdateRoomMenuCursor();
            break;
        }
        ShowMemoryStatusDebug();
        DIBslam();
        DIBslamReal();
    }

    LeaveAllocationScope();
    nMenuPointerSpeed = 2;
    nMenuInputRepeatDelay =
        nSavedRoomCtrlX;
    EventManagerHook(0);
    FreeBarracksMenuLabels();
    ReleasePacketHandle(background);
    ReleaseTextFont(0);
    free_viewport(&stSceneBuffer);
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook(35);
    return result;
}
