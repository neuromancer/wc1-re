/* Voice packs: recordings for briefings, scenes and the radio (speech.c). */
#ifndef SDL_PORT_SPEECH_H
#define SDL_PORT_SPEECH_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int SdlLoadVoiceCatalog(const char *path);
int SdlFindVoiceDirectory(const char *directory, char *root, unsigned long capacity);
int SdlLoadInstalledVoices(const char *directory);
void SdlShutdownVoices(void);
void SdlRegisterVoicePacket(const char *filename, short section,
                            const unsigned char *data, size_t size);
void SdlForgetVoicePacket(const void *data);
int SdlPlaySceneVoice(const unsigned char *scene, const unsigned char *record, int listening);
int SdlPlayRadioVoice(const char *filename, short speaker, short record, short rank, const char *text);
int SdlSceneVoicePlaying(void);
int SdlListeningCloseup(void);
int SdlRadioVoicePlaying(void);
void SdlStopRadioVoice(void);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_SPEECH_H */
