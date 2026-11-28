/* FM Towns text and its input (towns_text.c). */
#ifndef SDL_PORT_TOWNS_TEXT_H
#define SDL_PORT_TOWNS_TEXT_H

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

int SdlSetTownsLanguage(int japanese);
int SdlUsingTownsJapanese(void);
const char *SdlTownsFilename(const char *path);
const char *SdlTranslateTownsText(const char *text);
int SdlMeasureTownsText(const char *text);
int SdlDrawTownsText(const char *text);
int SdlDrawTownsCharacter(unsigned char character);
short SdlReadTownsTextInput(char *destination, short maximumLength, short mode);
int SdlHandleTownsTextInputEvent(const SDL_Event *event);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_TOWNS_TEXT_H */
