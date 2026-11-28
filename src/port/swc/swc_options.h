/* Super Wing Commander's Options menu (swc_options.c). */
#ifndef SDL_PORT_SWC_OPTIONS_H
#define SDL_PORT_SWC_OPTIONS_H

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SWC Options commands: Cmd (Ctrl on other hosts) with U, N, B or V; 1 when consumed. */
int SdlHandleSwcOptionKey(SDL_Window *window, const SDL_KeyboardEvent *key);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_SWC_OPTIONS_H */
