/* Music and sound effects: the device choice and playback (music.c). */
#ifndef SDL_PORT_MUSIC_H
#define SDL_PORT_MUSIC_H

#ifdef __cplusplus
extern "C" {
#endif

int SdlInitializeOriginFxAudio(int useStandaloneAudio);
int SdlGetOriginFxMusicSequencePosition(void);
void SdlMixOriginFxMusic(short *samples, unsigned int frameCount);
int SdlPlayDosSoundEffect(int soundNumber, int volume, int pan,
                             int tag, int priority);
int SdlHandlesGameSoundEffects(void);
struct FixedVector;
int SdlSoundEffectPan(const struct FixedVector *direction);
int SdlPlayGameSoundEffect(int soundNumber, int sourceObject,
                           int looping);
void SdlServiceOriginFxMusic(void);
/* The music the player chose: the installation's own (Kilrathi Saga
 * streams, or DOS AdLib), or the DOS scores on an AdLib or an MT-32. */
enum {
    SDL_PORT_MUSIC_INSTALLED = 0,
    SDL_PORT_MUSIC_ADLIB = 1,
    SDL_PORT_MUSIC_MT32 = 2
};
void SdlSetMusicDevice(int device);
int SdlGetMusicDevice(void);
/* The sound effects the player chose, apart from the music: the
 * installation's own (Kilrathi Saga's digital effects, or DOS AdLib), or
 * the DOS effects on an AdLib or an MT-32. */
enum {
    SDL_PORT_SOUND_INSTALLED = 0,
    SDL_PORT_SOUND_ADLIB = 1,
    SDL_PORT_SOUND_MT32 = 2
};
void SdlSetSoundDevice(int device);
int SdlUsingOriginFxMusic(void);
int SdlUsingOriginFxSoundEffects(void);
int SdlOriginFxMusicCommand(int track, int command);
void SdlOriginFxMusicBreakpoint(void);
void SdlStopOriginFxMusic(void);
void SdlStopDosSoundEffects(void);
void SdlShutdownOriginFxAudio(void);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_MUSIC_H */
