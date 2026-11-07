/* SDL campaign checkpoints. The existing SaveGame serializer keeps slot 8
 * in the original 828-byte SAVEGAME.WLD format. */
#include "wc1.h"

#include <errno.h>

static int autosaveEnabled;
/* 0: ask before replacing an occupied slot; 1: this campaign owns it;
 * -1: disabled for the current game, including any saves loaded within it. */
static int autosavePermission;

void SdlConfigureAutosave(int enabled)
{
    autosaveEnabled = enabled != 0;
    autosavePermission = 0;
}

void SdlResetAutosave(void)
{
    /* A return to the title screen or a new campaign starts a fresh game.
     * Keep the launcher's persistent enabled/disabled preference intact. */
    autosavePermission = 0;
}

void SdlResumeAutosave(int resumeAutosave)
{
    /* Loading another save must not undo a declined overwrite. Otherwise,
     * only loading the autosave itself permits continuing it without a prompt. */
    if (autosavePermission < 0)
        return;
    autosavePermission = resumeAutosave ? 1 : 0;
}

void SdlAutosaveCampaign(void)
{
    SaveGameDiskRecord previous;
    SaveGameRecord checkpoint = {0};
    short file;
    int valid;

    if (!autosaveEnabled || autosavePermission < 0 || !bCampaignActive ||
        nTrainSimActive || stCampaignState.currentSeries <= 0)
        return;

    PromptInsertNumberedDisk(0x10);
    file = OpenDataFileOrDie("SAVEGAME.WLD");
    if (file < 0 && errno == ENOENT) {
        CreateEmptySaveGameFile();
        file = OpenDataFileOrDie("SAVEGAME.WLD");
    }
    if (file < 0)
        goto failed;
    /* Unlike EnsureSaveGameFile, never recreate an existing file of an
     * unexpected size. Also distinguish an unreadable slot from an empty one. */
    valid = filelength(file) == 8 * sizeof(SaveGameDiskRecord) &&
        _lseek(file, SDL_AUTOSAVE_SLOT * sizeof(previous), SEEK_SET) != -1 &&
        _read(file, &previous, sizeof(previous)) == sizeof(previous);
    CloseDataFile(file);
    if (!valid)
        goto failed;

    if (autosavePermission == 0 && previous.occupied != 0) {
        int confirmed = 0;
        int savedCursorShowCount = nMouseCursorShowCount;

        /* The cursor can already be hidden after the initial simulator.
         * Preserve its visibility across the same text panel used by saves. */
        nMouseCursorShowCount = 0;
        SdlSuspendMouseGrab();
        if (ShowModalTextPanel(0, "Overwrite autosave? (Y/N)") != 0) {
            FlushInputEvents();
            confirmed = toupper((unsigned char)WaitForStreamInputKey()) == 'Y';
            ReleaseModalTextPanel();
        }
        SdlResumeMouseGrab();
        nMouseCursorShowCount = savedCursorShowCount;
        if (!confirmed) {
            autosavePermission = -1;
            return;
        }
    }

    strcpy(checkpoint.description, "autosave");
    checkpoint.occupied = 1;
    checkpoint.campaign = stCampaignState;
    memcpy(checkpoint.pilots, aPilotEntriesList, sizeof(checkpoint.pilots));
    memcpy(checkpoint.objectives, aMissionObjectivesArray,
           sizeof(checkpoint.objectives));
    if (!SaveGame(SDL_AUTOSAVE_SLOT, &checkpoint))
        goto failed;
    autosavePermission = 1;
    return;

failed:
    autosavePermission = -1;
    fprintf(stderr, "Cannot autosave to slot 8 in SAVEGAME.WLD.\n");
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Autosave failed",
        "Could not read or write SAVEGAME.WLD.\n"
        "Autosave is off for this game. Check the file and its permissions.\n"
        "You can still use manual save slots 1-7.", hMainWindow);
}
