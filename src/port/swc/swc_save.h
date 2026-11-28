/* Super Wing Commander's saved games (swc_save.c). */
#ifndef SDL_PORT_SWC_SAVE_H
#define SDL_PORT_SWC_SAVE_H

#ifdef __cplusplus
extern "C" {
#endif

/* SWC storage uses the original SaveGame/LoadGame record conversion. */
struct SaveGameDiskRecord;
int SdlReadSwcSaveGame(short slot, struct SaveGameDiskRecord *record);
int SdlWriteSwcSaveGame(short slot, const struct SaveGameDiskRecord *record);
int SdlSaveSwcGame(short slot);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_SWC_SAVE_H */
