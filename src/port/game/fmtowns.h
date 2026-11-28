/* FM Towns data and audio (fmtowns.c). */
#ifndef SDL_PORT_FMTOWNS_H
#define SDL_PORT_FMTOWNS_H

#ifdef __cplusplus
extern "C" {
#endif

int SdlUsingTownsData(void);
void SdlLoadTownsInstallTable(void);
int SdlInitializeTownsAudio(void);
void SdlShutdownTownsAudio(void);
void SdlServiceTownsAudio(void);
void SdlTownsMusicCommand(int track, int command);
int SdlPlayTownsEffect(int effect, int volume, int pan, int source, int looping);
void SdlStopTownsEffects(void);
int SdlStopTownsEffect(int handle);
int SdlPlayTownsRadio(short speaker, short record, short rank);
int SdlTownsRadioPlaying(void);
void SdlStopTownsRadio(void);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_FMTOWNS_H */
