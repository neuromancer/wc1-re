/* Translations and the German fan translation (translation.c). */
#ifndef SDL_PORT_TRANSLATION_H
#define SDL_PORT_TRANSLATION_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 0 = English, 1 = Spanish; catalogs live beside the executable. */
int SdlSetLanguage(int language);
/* gettext-compatible return type; callers must treat the text as read-only. */
char *SdlTranslateText(const char *context, const char *source);
int SdlLoadTranslationCatalog(const char *path);
void SdlShutdownTranslations(void);
/* A player's German fan translation of the DOS WC.EXE B2.4. */
int SdlLoadGermanText(const char *path);
int SdlCountGermanText(const char *path);
int SdlUsingGermanText(void);
void SdlShutdownGermanText(void);
int SdlTranslatePacketData(const char *filename, short section,
                          const unsigned char *source, size_t size,
                          unsigned char **output, size_t *outputSize);
void SdlTranslateRadio(const char *filename, short speaker, short line,
                       char *text, size_t capacity);
struct PacketSectionHandle;
int SdlTranslatePacketSection(const char *filename, short section,
                              struct PacketSectionHandle *handle);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_TRANSLATION_H */
