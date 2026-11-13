/* The WC1 flight inputs that the launcher's keyboard and mouse table can
 * rebind, shared by src/sdl/keymap.c and the launcher check. Each entry
 * names the input as the launcher settings and the table do, the key and
 * modifier that the game's own handler reads, and the original bindings: a
 * key by position, its other side for modifier keys, a modifier, a mouse
 * button, and a wheel direction. The arrows steer like a stick, so up
 * pitches the nose down. Full screen, a port shortcut, and the mouse buttons
 * that fire the guns and light the afterburner are handled separately, for
 * both games. */
#ifndef WC1_SDL_KEYMAP_ACTIONS_H
#define WC1_SDL_KEYMAP_ACTIONS_H

#define SDL_KEYMAP_ACTION_LIST(ACTION) \
    ACTION("pitch-down", SDL_SCANCODE_UP, 0, {SDL_SCANCODE_UP}) \
    ACTION("pitch-up", SDL_SCANCODE_DOWN, 0, {SDL_SCANCODE_DOWN}) \
    ACTION("yaw-left", SDL_SCANCODE_LEFT, 0, {SDL_SCANCODE_LEFT}) \
    ACTION("yaw-right", SDL_SCANCODE_RIGHT, 0, {SDL_SCANCODE_RIGHT}) \
    ACTION("pitch-down-yaw-left", SDL_SCANCODE_HOME, 0, {SDL_SCANCODE_HOME}) \
    ACTION("pitch-down-yaw-right", SDL_SCANCODE_PAGEUP, 0, {SDL_SCANCODE_PAGEUP}) \
    ACTION("pitch-up-yaw-left", SDL_SCANCODE_END, 0, {SDL_SCANCODE_END}) \
    ACTION("pitch-up-yaw-right", SDL_SCANCODE_PAGEDOWN, 0, {SDL_SCANCODE_PAGEDOWN}) \
    ACTION("roll-left", SDL_SCANCODE_COMMA, 0, {SDL_SCANCODE_COMMA}, {SDL_SCANCODE_INSERT}) \
    ACTION("roll-right", SDL_SCANCODE_PERIOD, 0, {SDL_SCANCODE_PERIOD}, {SDL_SCANCODE_DELETE}) \
    ACTION("full-turn-rate", SDL_SCANCODE_LSHIFT, 0, {SDL_SCANCODE_LSHIFT, SDL_SCANCODE_RSHIFT}) \
    ACTION("speed-up", SDL_SCANCODE_EQUALS, 0, {SDL_SCANCODE_EQUALS}, \
           {SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, 0, 0, 1}) \
    ACTION("slow-down", SDL_SCANCODE_MINUS, 0, {SDL_SCANCODE_MINUS}, \
           {SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, 0, 0, -1}) \
    ACTION("full-speed", SDL_SCANCODE_BACKSLASH, 0, {SDL_SCANCODE_BACKSLASH}) \
    ACTION("stop", SDL_SCANCODE_BACKSPACE, 0, {SDL_SCANCODE_BACKSPACE}) \
    ACTION("afterburner", SDL_SCANCODE_TAB, 0, {SDL_SCANCODE_TAB}) \
    ACTION("fire-guns", SDL_SCANCODE_SPACE, 0, {SDL_SCANCODE_SPACE}) \
    ACTION("fire-missile", SDL_SCANCODE_RETURN, 0, {SDL_SCANCODE_RETURN}) \
    ACTION("next-gun", SDL_SCANCODE_G, 0, {SDL_SCANCODE_G}) \
    ACTION("next-missile", SDL_SCANCODE_W, 0, {SDL_SCANCODE_W}) \
    ACTION("next-target", SDL_SCANCODE_T, 0, {SDL_SCANCODE_T}) \
    ACTION("lock-target", SDL_SCANCODE_L, 0, {SDL_SCANCODE_L}) \
    ACTION("damage-display", SDL_SCANCODE_D, 0, {SDL_SCANCODE_D}) \
    ACTION("navigation", SDL_SCANCODE_N, 0, {SDL_SCANCODE_N}) \
    ACTION("autopilot", SDL_SCANCODE_A, 0, {SDL_SCANCODE_A}) \
    ACTION("eject", SDL_SCANCODE_E, KMOD_CTRL, {SDL_SCANCODE_E, SDL_SCANCODE_UNKNOWN, KMOD_CTRL}) \
    ACTION("communications", SDL_SCANCODE_C, 0, {SDL_SCANCODE_C}) \
    ACTION("break-and-attack", SDL_SCANCODE_B, 0, {SDL_SCANCODE_B}) \
    ACTION("form-on-wing", SDL_SCANCODE_H, 0, {SDL_SCANCODE_H}) \
    ACTION("front-view", SDL_SCANCODE_F1, 0, {SDL_SCANCODE_F1}) \
    ACTION("left-view", SDL_SCANCODE_F2, 0, {SDL_SCANCODE_F2}) \
    ACTION("right-view", SDL_SCANCODE_F3, 0, {SDL_SCANCODE_F3}) \
    ACTION("rear-view", SDL_SCANCODE_F4, 0, {SDL_SCANCODE_F4}) \
    ACTION("chase-view", SDL_SCANCODE_F5, 0, {SDL_SCANCODE_F5}) \
    ACTION("battle-view", SDL_SCANCODE_F6, 0, {SDL_SCANCODE_F6}) \
    ACTION("tactical-view", SDL_SCANCODE_F7, 0, {SDL_SCANCODE_F7}) \
    ACTION("missile-camera", SDL_SCANCODE_F8, 0, {SDL_SCANCODE_F8}) \
    ACTION("other-ships", SDL_SCANCODE_F9, 0, {SDL_SCANCODE_F9}) \
    ACTION("pause", SDL_SCANCODE_P, 0, {SDL_SCANCODE_P}) \
    ACTION("message-speed", SDL_SCANCODE_M, 0, {SDL_SCANCODE_M}) \
    ACTION("comm-videos", SDL_SCANCODE_V, 0, {SDL_SCANCODE_V}) \
    ACTION("music", SDL_SCANCODE_M, KMOD_CTRL, {SDL_SCANCODE_M, SDL_SCANCODE_UNKNOWN, KMOD_CTRL}) \
    ACTION("sound", SDL_SCANCODE_S, KMOD_CTRL, {SDL_SCANCODE_S, SDL_SCANCODE_UNKNOWN, KMOD_CTRL})

/* SWC's rebindable inputs, as the Mac demo reads them (src/sdl/swc_flight.c
 * and swc_options.c). Each names the input, the key SWC reads (its keycode,
 * its position or SDL_SCANCODE_UNKNOWN to look the keycode up on the layout,
 * and its modifier), whether it is a menu option that every SWC screen reads,
 * and the original bindings by name. The menu shortcut is Command on macOS
 * and Control elsewhere. The digits 8, 2, 4 and 6, the keypad and the Actions
 * and Views menus' shortcuts are not listed: they always work. */
#ifdef __APPLE__
#define SDL_KEYMAP_MENU_MODIFIER KMOD_GUI
#define SDL_KEYMAP_MENU_NAME "meta"
#else
#define SDL_KEYMAP_MENU_MODIFIER KMOD_CTRL
#define SDL_KEYMAP_MENU_NAME "ctrl"
#endif

#define SDL_KEYMAP_SWC_ACTION_LIST(ACTION) \
    ACTION("swc-pitch-down", SDLK_UP, SDL_SCANCODE_UP, 0, 0, "up", "i") \
    ACTION("swc-pitch-up", SDLK_DOWN, SDL_SCANCODE_DOWN, 0, 0, "down", "k") \
    ACTION("swc-yaw-left", SDLK_LEFT, SDL_SCANCODE_LEFT, 0, 0, "left", "j") \
    ACTION("swc-yaw-right", SDLK_RIGHT, SDL_SCANCODE_RIGHT, 0, 0, "right", "l") \
    ACTION("swc-pitch-down-yaw-left", SDLK_KP_7, SDL_SCANCODE_KP_7, 0, 0, "7") \
    ACTION("swc-pitch-down-yaw-right", SDLK_KP_9, SDL_SCANCODE_KP_9, 0, 0, "9") \
    ACTION("swc-pitch-up-yaw-left", SDLK_KP_1, SDL_SCANCODE_KP_1, 0, 0, "1") \
    ACTION("swc-pitch-up-yaw-right", SDLK_KP_3, SDL_SCANCODE_KP_3, 0, 0, "3") \
    ACTION("swc-roll-left", SDLK_COMMA, SDL_SCANCODE_UNKNOWN, 0, 0, ",", "<") \
    ACTION("swc-roll-right", SDLK_KP_PERIOD, SDL_SCANCODE_KP_PERIOD, 0, 0, ".", ">") \
    ACTION("swc-speed-up", SDLK_KP_PLUS, SDL_SCANCODE_KP_PLUS, 0, 0, "+", "wheel-up") \
    ACTION("swc-slow-down", SDLK_KP_MINUS, SDL_SCANCODE_KP_MINUS, 0, 0, "-", "wheel-down") \
    ACTION("swc-afterburner", SDLK_TAB, SDL_SCANCODE_TAB, 0, 0, "tab") \
    ACTION("swc-fire-guns", SDLK_SPACE, SDL_SCANCODE_SPACE, 0, 0, "space") \
    ACTION("swc-fire-missile", SDLK_RETURN, SDL_SCANCODE_RETURN, 0, 0, "enter") \
    ACTION("swc-next-gun", SDLK_g, SDL_SCANCODE_UNKNOWN, 0, 0, "g") \
    ACTION("swc-next-missile", SDLK_w, SDL_SCANCODE_UNKNOWN, 0, 0, "w") \
    ACTION("swc-next-target", SDLK_t, SDL_SCANCODE_UNKNOWN, 0, 0, "t") \
    ACTION("swc-autopilot", SDLK_a, SDL_SCANCODE_UNKNOWN, 0, 0, "a") \
    ACTION("swc-computer", SDLK_c, SDL_SCANCODE_UNKNOWN, 0, 0, "c") \
    ACTION("swc-break-and-attack", SDLK_b, SDL_SCANCODE_UNKNOWN, 0, 0, "b") \
    ACTION("swc-request-landing", SDLK_r, SDL_SCANCODE_UNKNOWN, 0, 0, "r") \
    ACTION("swc-help", SDLK_h, SDL_SCANCODE_UNKNOWN, 0, 0, "h") \
    ACTION("swc-taunt", SDLK_e, SDL_SCANCODE_UNKNOWN, 0, 0, "e") \
    ACTION("swc-cockpit-view", SDLK_F1, SDL_SCANCODE_F1, 0, 0, "f1") \
    ACTION("swc-battle-view", SDLK_F2, SDL_SCANCODE_F2, 0, 0, "f2") \
    ACTION("swc-chase-view", SDLK_F3, SDL_SCANCODE_F3, 0, 0, "f3") \
    ACTION("swc-cockpitless", SDLK_F1, SDL_SCANCODE_F1, KMOD_CTRL, 0, "ctrl+f1") \
    ACTION("swc-pause", SDLK_p, SDL_SCANCODE_UNKNOWN, SDL_KEYMAP_MENU_MODIFIER, 0, SDL_KEYMAP_MENU_NAME "+p") \
    ACTION("swc-music", SDLK_u, SDL_SCANCODE_UNKNOWN, SDL_KEYMAP_MENU_MODIFIER, 1, SDL_KEYMAP_MENU_NAME "+u") \
    ACTION("swc-sound", SDLK_n, SDL_SCANCODE_UNKNOWN, SDL_KEYMAP_MENU_MODIFIER, 1, SDL_KEYMAP_MENU_NAME "+n") \
    ACTION("swc-text", SDLK_b, SDL_SCANCODE_UNKNOWN, SDL_KEYMAP_MENU_MODIFIER, 1, SDL_KEYMAP_MENU_NAME "+b") \
    ACTION("swc-volume", SDLK_v, SDL_SCANCODE_UNKNOWN, SDL_KEYMAP_MENU_MODIFIER, 1, SDL_KEYMAP_MENU_NAME "+v")

#endif
