/* Key and mouse rebinding (keymap.c). */
#ifndef SDL_PORT_KEYMAP_H
#define SDL_PORT_KEYMAP_H

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Keyboard rebinding (keymap.c): the launcher's changed bindings, one
 * "action=bindings" line each. */
void SdlSetKeyBindings(const char *bindings);
void SdlKeymapInvalidate(void);
/* The key the game should see for a key event; SDL_SCANCODE_UNKNOWN drops it. */
SDL_Scancode SdlKeymapTranslateKey(SDL_Scancode key, Uint16 modifiers, int pressed);
/* The keys the game should see held, given the keys that are. */
const Uint8 *SdlKeymapKeyboardState(const Uint8 *keys);
/* 1 or 0 when the full-screen keys were changed, -1 when they were not. */
int SdlKeymapFullscreenShortcut(SDL_Scancode key, Uint16 modifiers);
/* In flight, whether an action the player bound takes this key press. */
int SdlKeymapBindsKey(SDL_Scancode key, Uint16 modifiers);
/* In flight with changed mouse bindings: which of SDL's buttons hold the
 * game's guns (primary) and afterburner (secondary) buttons, and the key a
 * bound mouse button or wheel step presses (SDL_SCANCODE_UNKNOWN for none). */
int SdlKeymapMouseActive(void);
int SdlKeymapIsGameButton(Uint8 button);
void SdlKeymapGameButtons(Uint32 buttons, int *primary, int *secondary);
SDL_Scancode SdlKeymapMouseKey(Uint8 button);
SDL_Scancode SdlKeymapWheelKey(int direction);
/* SWC's rebound flight inputs: events rewritten into the keys SWC reads (type
 * 0 drops one), and the held keys and buttons SwcFlightTick sees. Option keys
 * return 1 when rewritten, -1 to drop, 0 to keep. */
void SdlSwcKeymapTranslateEvent(SDL_Event *event);
int SdlSwcKeymapTranslateOptionKey(SDL_KeyboardEvent *key);
const Uint8 *SdlSwcKeymapKeyboardState(const Uint8 *keys);
Uint32 SdlSwcKeymapMouseButtons(Uint32 buttons);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_KEYMAP_H */
