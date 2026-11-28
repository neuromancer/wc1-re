/* Audio output and voice playback (audio.c). */
#ifndef SDL_PORT_AUDIO_H
#define SDL_PORT_AUDIO_H

#include "platform/win32.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*SdlAudioMixer)(void *, unsigned int);
int SdlStartAudio(SdlAudioMixer mixer,
                     CRITICAL_SECTION *criticalSection,
                     unsigned int *tick);
void SdlStopAudio(void);
int SdlPlayVoiceWav(const char *path);
int SdlVoicePlaying(void);
short SdlVoiceMouthFrame(const short *commands, short fallback);
void SdlStopVoice(void);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_AUDIO_H */
