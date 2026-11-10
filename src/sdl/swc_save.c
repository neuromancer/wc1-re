/* Retail CODE_03 SaveGame/LoadGame and CODE_12 preferences storage.
 * WC1 converts the campaign records; this SDL boundary owns SWC slots and
 * the replacement-pilot flag that lives outside the shared campaign record. */
#ifdef _WIN32
#include <wchar.h>
/* Declare before the host header replaces Win32 calling-convention macros. */
__declspec(dllimport) int __stdcall MoveFileExW(const wchar_t *, const wchar_t *, unsigned long);
#endif
#include "wc1.h"
#include "swc.h"

static SaveGameDiskRecord swcSaves[8];
static Uint32 swcSaveOrder[8];
static short swcSaveBossman[8];
static unsigned char swcSaveMissions[3][13];
static char swcSavePath[PATH_MAX];
static int swcSavesReady;
int nSwcContinueSlot = -1;
short nSwcBossmanDeath;

/* CheckValidZones 0x010356e0, with bounds needed before a host save can
 * reach the original, unchecked campaign and portrait table consumers. */
static int SwcCheckSavedGame(const SaveGameDiskRecord *save)
{
    const SaveGameDiskCampaignState *campaign = &save->campaign;
    const PilotRecord *pilot;
    int index;

    if (save->occupied == 0)
        return 1;
    if (save->occupied != 1 || memchr(save->description, 0, sizeof(save->description)) == NULL ||
        (unsigned int)campaign->campaignIndex >= 3 ||
        campaign->currentSeries < 1 || campaign->currentSeries > 13 ||
        campaign->currentMission < 0 || campaign->currentMission >=
            swcSaveMissions[campaign->campaignIndex][campaign->currentSeries - 1] ||
        (unsigned int)campaign->seriesHistoryCount > 8 ||
        (campaign->playerShipType != 10 && (unsigned int)campaign->playerShipType > 3) ||
        campaign->currentDate.day < 1 || campaign->currentDate.day > 365 ||
        campaign->currentDate.year < 0 || campaign->elapsedDate.year < 0)
        return 0;
    for (index = 0; index < campaign->seriesHistoryCount; index++) {
        if (campaign->seriesHistory[index] < 1 || campaign->seriesHistory[index] > 13)
            return 0;
    }
    for (index = 0; index < 8; index++) {
        if ((unsigned int)campaign->personalityDeathMission[index] > 55)
            return 0;
    }
    for (index = 0; index < 5; index++) {
        if (campaign->medals[index] > (index >= 3 ? 1 : 3))
            return 0;
    }
    for (index = 0; index < 9; index++) {
        pilot = &save->pilots[index];
        if (memchr(pilot->name, 0, sizeof(pilot->name)) == NULL ||
            memchr(pilot->callsign, 0, sizeof(pilot->callsign)) == NULL ||
            (unsigned int)pilot->rank > 4 || (unsigned int)pilot->portrait > 4 ||
            (unsigned int)pilot->personality > 4 || pilot->missions < 0 || pilot->kills < 0)
            return 0;
    }
    return 1;
}

/* LoadPreferences 0x010c3806. The versioned SDL file contains eight WC1
 * disk records, each preceded by a LE32 save order and LE16 Bossman status.
 * The original Mac preferences and WC1 SAVEGAME.WLD are independent files. */
int SdlLoadSwcSaves(const char *missionPath, int retail)
{
    SwcCmf cmf = {0};
    SwcBuffer table = {0};
    SDL_RWops *file = NULL;
    char *directory;
    char tag[5];
    char magic[8];
    Uint32 newest = 0;
    int dataset;
    int series;
    int slot;
    int result = -1;

    swcSavesReady = 0;
    nSwcContinueSlot = -1;
    memset(swcSaves, 0, sizeof(swcSaves));
    memset(swcSaveOrder, 0, sizeof(swcSaveOrder));
    memset(swcSaveBossman, 0, sizeof(swcSaveBossman));
    memset(swcSaveMissions, 0, sizeof(swcSaveMissions));
    if (SDL_BYTEORDER != SDL_LIL_ENDIAN || sizeof(SaveGameDiskRecord) != 828)
        return SDL_SetError("Unsupported SWC save record layout");
    directory = SDL_GetPrefPath("Origin Systems", "WC Kilrathi Saga");
    if (directory == NULL)
        return -1;
    result = SDL_snprintf(swcSavePath, sizeof(swcSavePath), "%s%s", directory,
        retail ? "swc-saves.dat" : "swc-demo-saves.dat");
    SDL_free(directory);
    if (result >= (int)sizeof(swcSavePath))
        return SDL_SetError("SWC save path is too long");
    result = -1;
    if (SwcCMOpen(missionPath, &cmf) != 0)
        goto done;
    for (dataset = 0; dataset < (retail ? 3 : 1); dataset++) {
        SDL_snprintf(tag, sizeof(tag), "CMP%d", dataset);
        if (SwcCMGetChunk(&cmf, tag, 2, &table) != 0)
            goto done;
        if (table.size < 13 * 128) {
            SDL_SetError("Truncated SWC campaign table");
            goto done;
        }
        for (series = 0; series < 13; series++)
            swcSaveMissions[dataset][series] = table.data[series * 128 + 7];
        SDL_free(table.data);
        SDL_zero(table);
    }
    if (!retail) {
        memset(swcSaveMissions, 0, sizeof(swcSaveMissions));
        swcSaveMissions[0][0] = 1;
    }
    errno = 0;
    file = SDL_RWFromFile(swcSavePath, "rb");
    if (file == NULL) {
#ifdef _WIN32
        /* SDL's native Windows file backend need not set errno. */
        wchar_t *filename = (wchar_t *)SDL_iconv_string("UTF-16LE", "UTF-8", swcSavePath, strlen(swcSavePath) + 1);
        int missing = filename && _waccess(filename, 0) != 0 && errno == ENOENT;

        SDL_free(filename);
        if (missing) {
#else
        if (errno == ENOENT) {
#endif
            SDL_ClearError();
            result = 0;
        }
        goto done;
    }
    if (SDL_RWsize(file) != 8 + 8 * (6 + sizeof(SaveGameDiskRecord)) ||
        SDL_RWread(file, magic, sizeof(magic), 1) != 1 || memcmp(magic, "SWCSAVE1", 8) != 0) {
        SDL_SetError("Invalid SWC save file; existing data has been preserved");
        goto done;
    }
    for (slot = 0; slot < 8; slot++) {
        swcSaveOrder[slot] = SDL_ReadLE32(file);
        swcSaveBossman[slot] = SDL_ReadLE16(file);
        if (SDL_RWread(file, &swcSaves[slot], sizeof(swcSaves[slot]), 1) != 1 ||
            !SwcCheckSavedGame(&swcSaves[slot]) || (unsigned int)swcSaveBossman[slot] > 55 ||
            (swcSaves[slot].occupied && swcSaveOrder[slot] == 0)) {
            SDL_SetError("Invalid SWC save slot %d; existing data has been preserved", slot + 1);
            goto done;
        }
        if (swcSaves[slot].occupied && swcSaveOrder[slot] > newest) {
            newest = swcSaveOrder[slot];
            nSwcContinueSlot = slot;
        }
    }
    result = 0;
done:
    if (file != NULL)
        SDL_RWclose(file);
    SwcCMClose(&cmf);
    SDL_free(table.data);
    swcSavesReady = result == 0;
    if (!swcSavesReady)
        nSwcContinueSlot = -1;
    return result;
}

int SdlReadSwcSaveGame(short slot, SaveGameDiskRecord *record)
{
    if (!swcSavesReady || (unsigned int)slot >= 8 || !swcSaves[slot].occupied)
        return 0;
    *record = swcSaves[slot];
    return 1;
}

/* SavePreferences 0x010c3cc8: commit a complete image. A failed write must
 * leave both the previous file and the in-memory slot list unchanged. */
int SdlWriteSwcSaveGame(short slot, const SaveGameDiskRecord *record)
{
    SDL_RWops *file;
    char temporary[PATH_MAX];
    Uint32 order = 0;
    Uint32 newest = 0;
    int index;
    int written = 1;

    if (!swcSavesReady || (unsigned int)slot >= 8) {
        SDL_SetError("SWC saves are unavailable");
        return 0;
    }
    if (!SwcCheckSavedGame(record) || (unsigned int)nSwcBossmanDeath > 55) {
        SDL_SetError("Invalid SWC campaign state");
        return 0;
    }
    for (index = 0; index < 8; index++)
        order = SDL_max(order, swcSaveOrder[index]);
    if (order == UINT32_MAX || SDL_snprintf(temporary, sizeof(temporary), "%s.tmp", swcSavePath) >= (int)sizeof(temporary)) {
        SDL_SetError("Cannot prepare SWC save file");
        return 0;
    }
    order++;
    file = SDL_RWFromFile(temporary, "wb");
    if (file == NULL)
        return 0;
    written = SDL_RWwrite(file, "SWCSAVE1", 8, 1) == 1;
    for (index = 0; index < 8 && written; index++) {
        written = SDL_WriteLE32(file, index == slot ? order : swcSaveOrder[index]) &&
            SDL_WriteLE16(file, index == slot ? nSwcBossmanDeath : swcSaveBossman[index]) &&
            SDL_RWwrite(file, index == slot ? record : &swcSaves[index], sizeof(*record), 1) == 1;
    }
    if (SDL_RWclose(file) != 0)
        written = 0;
#ifdef _WIN32
    {
        wchar_t *source = (wchar_t *)SDL_iconv_string("UTF-16LE", "UTF-8", temporary, strlen(temporary) + 1);
        wchar_t *destination = (wchar_t *)SDL_iconv_string("UTF-16LE", "UTF-8", swcSavePath, strlen(swcSavePath) + 1);

        if (written)
            written = source && destination && MoveFileExW(source, destination, 9); /* replace, write through */
        if (!written && source)
            _wremove(source);
        SDL_free(source);
        SDL_free(destination);
    }
#else
    if (written)
        written = rename(temporary, swcSavePath) == 0;
    if (!written)
        remove(temporary);
#endif
    if (!written) {
        SDL_SetError("Cannot replace SWC save file; the previous saves are unchanged");
        return 0;
    }
    swcSaves[slot] = *record;
    swcSaveOrder[slot] = order;
    swcSaveBossman[slot] = nSwcBossmanDeath;
    nSwcContinueSlot = -1;
    for (index = 0; index < 8; index++) {
        if (swcSaves[index].occupied && swcSaveOrder[index] > newest) {
            newest = swcSaveOrder[index];
            nSwcContinueSlot = index;
        }
    }
    return 1;
}

/* Retail SaveGame 0x01035898 captures the campaign and all nine pilots. */
int SdlSaveSwcGame(short slot)
{
    SaveGameRecord save = {0};

    save.occupied = 1;
    SDL_snprintf(save.description, sizeof(save.description), "%.12s %d",
        (const char *)abSeriesAuxData, stCampaignState.currentMission + 1);
    save.campaign = stCampaignState;
    save.campaign.currentPilot = NULL;
    memcpy(save.pilots, aPilotEntriesList, sizeof(save.pilots));
    /* Objectives and their pointers are rebuilt from MODn on load. */
    return SaveGame(slot, &save) ? 0 : -1;
}

/* Retail LoadGame 0x01035786 restores pilots before CorrectPointers. */
int SdlLoadSwcGame(short slot)
{
    SaveGameRecord save = {0};

    if (!LoadGame(slot, &save))
        return SDL_SetError("Cannot load SWC save slot %d", slot + 1);
    stCampaignState = save.campaign;
    memcpy(aPilotEntriesList, save.pilots, sizeof(save.pilots));
    CorrectPointers();
    nCampaignDataSet = stCampaignState.campaignIndex;
    nSwcBossmanDeath = swcSaveBossman[slot];
    bCampaignActive = 1;
    SdlResumeAutosave(slot == SDL_AUTOSAVE_SLOT);
    return 0;
}
