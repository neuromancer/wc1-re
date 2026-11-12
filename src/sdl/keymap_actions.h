/* The WC1 flight inputs that the launcher's keyboard and mouse table can
 * rebind, shared by src/sdl/keymap.c and the launcher check. Each entry
 * names the input as the launcher settings and the table do, the key and
 * modifier that the game's own handler reads, and the original bindings: a
 * key by position, its other side for modifier keys, a modifier, a mouse
 * button, and a wheel direction. The arrows steer like a stick, so up
 * pitches the nose down. Full screen, a port shortcut, and the mouse buttons
 * that fire the guns and light the afterburner are handled separately. */
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

#endif
