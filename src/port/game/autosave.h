/* Autosave and mission retry (autosave.c). */
#ifndef SDL_PORT_AUTOSAVE_H
#define SDL_PORT_AUTOSAVE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Slot 8 is always reserved in the SDL barracks, even with autosave off. */
#define SDL_AUTOSAVE_SLOT 7
void SdlConfigureAutosave(int enabled);
void SdlResetAutosave(void);
void SdlResumeAutosave(int resumeAutosave);
void SdlAutosaveCampaign(void);
void SdlCaptureMissionCheckpoint(void);
void SdlRestoreMissionCheckpoint(void);
int SdlRetryMission(void);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_AUTOSAVE_H */
