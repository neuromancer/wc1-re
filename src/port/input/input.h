/* Keyboard sampling for the game's input (input.c). */
#ifndef SDL_PORT_INPUT_H
#define SDL_PORT_INPUT_H

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Borrow one SDL keyboard sample for a simulation tick; NULL restores live polling. */
void SdlSetKeyboardSnapshot(const Uint8 *keys);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_INPUT_H */
