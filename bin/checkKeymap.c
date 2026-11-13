/* Checks the keyboard rebinding in src/sdl/keymap.c: in flight, changed keys
 * stand in for the keys the game reads, the keys they replaced stop working,
 * and every other key keeps its meaning. Run with SDL's dummy video driver,
 * whose keyboard layout is US English. */
#include "wc1sdl.h"

#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
#define SDL_KEYMAP_TEST_MENU KMOD_GUI
#else
#define SDL_KEYMAP_TEST_MENU KMOD_CTRL
#endif

static int g_bFlight;
static int g_nFailures;
static Uint32 g_dwMouseButtons;

int SdlSpaceflightInputActive(void)
{
    return g_bFlight;
}

/* The buttons the keymap sees held. */
Uint32 SDL_GetMouseState(int *x, int *y)
{
    if (x != NULL)
        *x = 0;
    if (y != NULL)
        *y = 0;
    return g_dwMouseButtons;
}

static void Expect(int condition, const char *what)
{
    if (!condition) {
        fprintf(stderr, "FAIL: %s\n", what);
        g_nFailures++;
    }
}

/* Presses and releases a key; the press's translation, which the release
 * must repeat. */
static SDL_Scancode Tap(SDL_Scancode key, Uint16 modifiers)
{
    SDL_Scancode pressed = SdlKeymapTranslateKey(key, modifiers, 1);

    if (SdlKeymapTranslateKey(key, modifiers, 0) != pressed) {
        fprintf(stderr, "FAIL: release of scancode %d differs from its press\n", key);
        g_nFailures++;
    }
    return pressed;
}

/* What the game sees held while exactly these keys are. */
static const Uint8 *Hold(SDL_Scancode first, SDL_Scancode second)
{
    static Uint8 keys[SDL_NUM_SCANCODES];

    memset(keys, 0, sizeof(keys));
    keys[first] = 1;
    keys[second] = 1;
    return SdlKeymapKeyboardState(keys);
}

int main(void)
{
    const Uint8 *held;

    SDL_setenv("SDL_VIDEODRIVER", "dummy", 1);
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "FAIL: SDL: %s\n", SDL_GetError());
        return 1;
    }

    /* Without changes nothing is touched, in flight or not. */
    SdlSetKeyBindings("");
    g_bFlight = 1;
    Expect(Tap(SDL_SCANCODE_UP, 0) == SDL_SCANCODE_UP, "original keys pass unchanged");
    held = Hold(SDL_SCANCODE_UP, SDL_SCANCODE_UNKNOWN);
    Expect(held[SDL_SCANCODE_UP], "original keys stay held");
    Expect(SdlKeymapFullscreenShortcut(SDL_SCANCODE_RETURN, KMOD_LALT) == -1,
           "full screen keeps Alt-Enter when unchanged");

    SdlSetKeyBindings("pitch-down=k\n"
                      "pitch-up=up\n"
                      "eject=j\n"
                      "message-speed=ctrl+k\n"
                      "full-turn-rate=caps-lock\n"
                      "speed-up=%2b,%c3%b1\n"
                      "fire-guns=%21\n"
                      "pause=o,escape\n"
                      "lock-target=\n"
                      "full-screen=f11\n"
                      "unknown-action=q\n");

    /* Outside flight, keys keep their meaning. */
    g_bFlight = 0;
    Expect(Tap(SDL_SCANCODE_K, 0) == SDL_SCANCODE_K, "menus see K");
    Expect(Tap(SDL_SCANCODE_DOWN, 0) == SDL_SCANCODE_DOWN, "menus see Down");
    held = Hold(SDL_SCANCODE_K, SDL_SCANCODE_UNKNOWN);
    Expect(held[SDL_SCANCODE_K] && !held[SDL_SCANCODE_UP], "menus see K held");

    g_bFlight = 1;
    /* A new key stands in for the original one, which stops working. */
    Expect(Tap(SDL_SCANCODE_K, 0) == SDL_SCANCODE_UP, "K pitches down");
    Expect(Tap(SDL_SCANCODE_UP, 0) == SDL_SCANCODE_DOWN, "Up pitches up");
    Expect(Tap(SDL_SCANCODE_DOWN, 0) == SDL_SCANCODE_UNKNOWN, "Down no longer pitches up");
    held = Hold(SDL_SCANCODE_K, SDL_SCANCODE_UNKNOWN);
    Expect(held[SDL_SCANCODE_UP] && !held[SDL_SCANCODE_K], "holding K holds Up");
    held = Hold(SDL_SCANCODE_UP, SDL_SCANCODE_UNKNOWN);
    Expect(held[SDL_SCANCODE_DOWN] && !held[SDL_SCANCODE_UP], "holding Up holds Down");
    held = Hold(SDL_SCANCODE_DOWN, SDL_SCANCODE_UNKNOWN);
    Expect(!held[SDL_SCANCODE_DOWN] && !held[SDL_SCANCODE_UP], "holding Down holds nothing");

    /* A key for an action that needs Ctrl holds Ctrl for it. */
    Expect(Tap(SDL_SCANCODE_J, 0) == SDL_SCANCODE_E, "J ejects");
    held = Hold(SDL_SCANCODE_J, SDL_SCANCODE_UNKNOWN);
    Expect(held[SDL_SCANCODE_E] && held[SDL_SCANCODE_LCTRL], "J holds Ctrl+E");
    Expect(Tap(SDL_SCANCODE_E, KMOD_LCTRL) == SDL_SCANCODE_UNKNOWN, "Ctrl+E no longer ejects");
    Expect(Tap(SDL_SCANCODE_E, 0) == SDL_SCANCODE_E, "E alone keeps its meaning");

    /* A modifier in a binding stands only for that binding, and a binding
     * with a held modifier wins over the plain key. */
    Expect(Tap(SDL_SCANCODE_K, KMOD_LCTRL) == SDL_SCANCODE_M, "Ctrl+K sets message speed");
    held = Hold(SDL_SCANCODE_K, SDL_SCANCODE_LCTRL);
    Expect(held[SDL_SCANCODE_M] && !held[SDL_SCANCODE_LCTRL] && !held[SDL_SCANCODE_UP],
           "Ctrl+K holds M without Ctrl");
    Expect(Tap(SDL_SCANCODE_M, 0) == SDL_SCANCODE_UNKNOWN, "M no longer sets message speed");
    Expect(Tap(SDL_SCANCODE_M, KMOD_LCTRL) == SDL_SCANCODE_M, "Ctrl+M still toggles music");
    held = Hold(SDL_SCANCODE_M, SDL_SCANCODE_LCTRL);
    Expect(held[SDL_SCANCODE_M] && held[SDL_SCANCODE_LCTRL], "Ctrl+M stays held for music");

    /* A modifier can be rebound like any key. */
    held = Hold(SDL_SCANCODE_CAPSLOCK, SDL_SCANCODE_UNKNOWN);
    Expect(held[SDL_SCANCODE_LSHIFT], "Caps Lock gives the full turn rate");
    held = Hold(SDL_SCANCODE_LSHIFT, SDL_SCANCODE_UNKNOWN);
    Expect(!held[SDL_SCANCODE_LSHIFT] && !held[SDL_SCANCODE_RSHIFT], "Shift no longer does");

    /* "+" is not an unshifted key on a US layout: it stays on the key the
     * game has always read. A key this layout cannot type is ignored. */
    Expect(Tap(SDL_SCANCODE_EQUALS, 0) == SDL_SCANCODE_EQUALS, "+ keeps its key");
    Expect(Tap(SDL_SCANCODE_SEMICOLON, 0) == SDL_SCANCODE_SEMICOLON, "a US keyboard has no Ñ");

    /* A symbol typed with Shift needs Shift. */
    Expect(Tap(SDL_SCANCODE_1, KMOD_LSHIFT) == SDL_SCANCODE_SPACE, "! fires guns");
    Expect(Tap(SDL_SCANCODE_1, 0) == SDL_SCANCODE_1, "1 still chooses a message");
    Expect(Tap(SDL_SCANCODE_SPACE, 0) == SDL_SCANCODE_UNKNOWN, "Space no longer fires");

    /* Escape always keeps its meanings; an action may lose every key. */
    Expect(Tap(SDL_SCANCODE_O, 0) == SDL_SCANCODE_P, "O pauses");
    Expect(Tap(SDL_SCANCODE_P, 0) == SDL_SCANCODE_UNKNOWN, "P no longer pauses");
    Expect(Tap(SDL_SCANCODE_ESCAPE, 0) == SDL_SCANCODE_ESCAPE, "Escape still pauses");
    Expect(Tap(SDL_SCANCODE_L, 0) == SDL_SCANCODE_UNKNOWN, "L no longer locks targets");

    /* Unchanged actions keep their keys, including the keypad. */
    Expect(Tap(SDL_SCANCODE_G, 0) == SDL_SCANCODE_G, "G still cycles guns");
    held = Hold(SDL_SCANCODE_KP_8, SDL_SCANCODE_UNKNOWN);
    Expect(held[SDL_SCANCODE_KP_8], "keypad 8 still pitches down");

    /* A press translated in flight is released the same way afterwards. */
    Expect(SdlKeymapTranslateKey(SDL_SCANCODE_K, 0, 1) == SDL_SCANCODE_UP, "press K in flight");
    g_bFlight = 0;
    Expect(SdlKeymapTranslateKey(SDL_SCANCODE_K, 0, 0) == SDL_SCANCODE_UP,
           "releasing K after flight releases Up");

    /* Full screen follows its keys everywhere. */
    Expect(SdlKeymapFullscreenShortcut(SDL_SCANCODE_F11, 0) == 1, "F11 toggles full screen");
    Expect(SdlKeymapFullscreenShortcut(SDL_SCANCODE_RETURN, KMOD_LALT) == 0,
           "Alt-Enter no longer does");

    /* Keys alone leave the mouse as it is. */
    g_bFlight = 1;
    SdlSetKeyBindings("pitch-down=k\n");
    Expect(!SdlKeymapMouseActive(), "changed keys leave the mouse alone");

    /* The game's two buttons can swap or move, and other buttons and the
     * wheel press the keys of the actions bound to them. */
    SdlSetKeyBindings("mouse-guns=mouse-right\n"
                      "mouse-afterburner=mouse-left\n"
                      "lock-target=l,mouse-back\n"
                      "pitch-down=up,mouse-middle\n"
                      "slow-down=-,wheel-up\n"
                      "eject=mouse-forward\n");
    {
        int primary;
        int secondary;

        Expect(SdlKeymapMouseActive(), "the mouse is remapped in flight");
        SdlKeymapGameButtons(SDL_BUTTON_RMASK, &primary, &secondary);
        Expect(primary && !secondary, "the right button fires the guns");
        SdlKeymapGameButtons(SDL_BUTTON_LMASK, &primary, &secondary);
        Expect(!primary && secondary, "the left button lights the afterburner");
        SdlKeymapGameButtons(SDL_BUTTON_MMASK, &primary, &secondary);
        Expect(!primary && !secondary, "the middle button is neither");
    }
    Expect(SdlKeymapIsGameButton(SDL_BUTTON_LEFT) && SdlKeymapIsGameButton(SDL_BUTTON_RIGHT) &&
           !SdlKeymapIsGameButton(SDL_BUTTON_MIDDLE), "the game's buttons");
    Expect(SdlKeymapMouseKey(SDL_BUTTON_X1) == SDL_SCANCODE_L, "button 4 locks targets");
    Expect(SdlKeymapMouseKey(SDL_BUTTON_MIDDLE) == SDL_SCANCODE_UP, "the middle button pitches down");
    Expect(SdlKeymapMouseKey(SDL_BUTTON_LEFT) == SDL_SCANCODE_UNKNOWN, "a game button presses no key");
    Expect(SdlKeymapWheelKey(1) == SDL_SCANCODE_MINUS, "wheel up now slows down");
    Expect(SdlKeymapWheelKey(-1) == SDL_SCANCODE_UNKNOWN, "wheel down is unbound");
    g_dwMouseButtons = SDL_BUTTON(SDL_BUTTON_MIDDLE);
    held = Hold(SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN);
    Expect(held[SDL_SCANCODE_UP], "holding the middle button holds Up");
    g_dwMouseButtons = SDL_BUTTON(SDL_BUTTON_X2);
    held = Hold(SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN);
    Expect(held[SDL_SCANCODE_E] && held[SDL_SCANCODE_LCTRL], "button 5 holds Ctrl+E to eject");
    g_dwMouseButtons = SDL_BUTTON(SDL_BUTTON_LEFT);
    held = Hold(SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN);
    Expect(!held[SDL_SCANCODE_UP] && !held[SDL_SCANCODE_LCTRL], "a game button holds no key");
    g_dwMouseButtons = 0;
    Expect(Tap(SDL_SCANCODE_UP, 0) == SDL_SCANCODE_UP, "Up still pitches down");
    g_bFlight = 0;
    Expect(!SdlKeymapMouseActive(), "menus keep the mouse as it is");

    /* SWC: rebound inputs become the keys the Mac demo reads. */
    {
        SDL_Event event;
        Uint8 keys[SDL_NUM_SCANCODES];
        const Uint8 *swcHeld;

        SdlSetKeyBindings("swc-next-target=y\n"
                          "swc-pitch-down=w\n"
                          "swc-pause=f12\n"
                          "swc-music=f11\n");
        SDL_zero(event);
        event.type = SDL_KEYDOWN;
        event.key.keysym.scancode = SDL_SCANCODE_Y;
        event.key.keysym.sym = SDLK_y;
        SdlSwcKeymapTranslateEvent(&event);
        Expect(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_t &&
               event.key.keysym.scancode == SDL_SCANCODE_T && event.key.keysym.mod == 0,
               "SWC: Y cycles targets");
        SDL_zero(event);
        event.type = SDL_KEYUP;
        event.key.keysym.scancode = SDL_SCANCODE_Y;
        event.key.keysym.sym = SDLK_y;
        SdlSwcKeymapTranslateEvent(&event);
        Expect(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_t, "SWC: Y's release releases T");
        SDL_zero(event);
        event.type = SDL_KEYDOWN;
        event.key.keysym.scancode = SDL_SCANCODE_T;
        event.key.keysym.sym = SDLK_t;
        SdlSwcKeymapTranslateEvent(&event);
        Expect(event.type == 0, "SWC: plain T no longer cycles targets");
        SDL_zero(event);
        event.type = SDL_KEYDOWN;
        event.key.keysym.scancode = SDL_SCANCODE_T;
        event.key.keysym.sym = SDLK_t;
        event.key.keysym.mod = KMOD_LGUI | KMOD_LCTRL;
        SdlSwcKeymapTranslateEvent(&event);
        Expect(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_t,
               "SWC: the menu shortcut still cycles targets");
        SDL_zero(event);
        event.type = SDL_KEYDOWN;
        event.key.keysym.scancode = SDL_SCANCODE_F12;
        event.key.keysym.sym = SDLK_F12;
        SdlSwcKeymapTranslateEvent(&event);
        Expect(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p &&
               event.key.keysym.mod == SDL_KEYMAP_TEST_MENU, "SWC: F12 pauses through the menu");
        SDL_zero(event);
        event.type = SDL_KEYDOWN;
        event.key.keysym.scancode = SDL_SCANCODE_P;
        event.key.keysym.sym = SDLK_p;
        event.key.keysym.mod = SDL_KEYMAP_TEST_MENU;
        SdlSwcKeymapTranslateEvent(&event);
        Expect(event.type == 0, "SWC: the pause shortcut moved to F12");
        SDL_zero(event);
        event.type = SDL_KEYDOWN;
        event.key.keysym.scancode = SDL_SCANCODE_F11;
        event.key.keysym.sym = SDLK_F11;
        Expect(SdlSwcKeymapTranslateOptionKey(&event.key) == 1 && event.key.keysym.sym == SDLK_u &&
               event.key.keysym.mod == SDL_KEYMAP_TEST_MENU, "SWC: F11 toggles music on any screen");
        event.key.keysym.scancode = SDL_SCANCODE_U;
        event.key.keysym.sym = SDLK_u;
        event.key.keysym.mod = SDL_KEYMAP_TEST_MENU;
        Expect(SdlSwcKeymapTranslateOptionKey(&event.key) == -1, "SWC: the music shortcut moved to F11");
        memset(keys, 0, sizeof(keys));
        keys[SDL_SCANCODE_W] = 1;
        swcHeld = SdlSwcKeymapKeyboardState(keys);
        Expect(swcHeld[SDL_SCANCODE_UP] && !swcHeld[SDL_SCANCODE_W], "SWC: holding W holds Up");
        memset(keys, 0, sizeof(keys));
        keys[SDL_SCANCODE_UP] = 1;
        keys[SDL_SCANCODE_I] = 1;
        keys[SDL_SCANCODE_8] = 1;
        swcHeld = SdlSwcKeymapKeyboardState(keys);
        Expect(!swcHeld[SDL_SCANCODE_UP] && !swcHeld[SDL_SCANCODE_I] && swcHeld[SDL_SCANCODE_8],
               "SWC: Up and I no longer pitch down, and 8 still does");

        SdlSetKeyBindings("swc-mouse-guns=mouse-right\n"
                          "swc-mouse-afterburner=mouse-left\n"
                          "swc-next-gun=g,mouse-middle\n"
                          "swc-speed-up=%2b\n"
                          "swc-slow-down=-,wheel-up\n");
        SDL_zero(event);
        event.type = SDL_MOUSEBUTTONDOWN;
        event.button.button = SDL_BUTTON_RIGHT;
        SdlSwcKeymapTranslateEvent(&event);
        Expect(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT,
               "SWC: the right button fires the guns");
        SDL_zero(event);
        event.type = SDL_MOUSEBUTTONDOWN;
        event.button.button = SDL_BUTTON_MIDDLE;
        SdlSwcKeymapTranslateEvent(&event);
        Expect(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_g, "SWC: the middle button cycles guns");
        SDL_zero(event);
        event.type = SDL_MOUSEBUTTONUP;
        event.button.button = SDL_BUTTON_MIDDLE;
        SdlSwcKeymapTranslateEvent(&event);
        Expect(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_g, "SWC: and releases G");
        SDL_zero(event);
        event.type = SDL_MOUSEWHEEL;
        event.wheel.y = 1;
        SdlSwcKeymapTranslateEvent(&event);
        Expect(event.type == SDL_MOUSEWHEEL && event.wheel.y == -1, "SWC: wheel up slows down");
        SDL_zero(event);
        event.type = SDL_MOUSEWHEEL;
        event.wheel.y = -1;
        SdlSwcKeymapTranslateEvent(&event);
        Expect(event.type == 0, "SWC: wheel down is unbound");
        Expect(SdlSwcKeymapMouseButtons(SDL_BUTTON_RMASK) == SDL_BUTTON_LMASK &&
               SdlSwcKeymapMouseButtons(SDL_BUTTON_LMASK) == SDL_BUTTON_RMASK,
               "SWC: the held buttons swap too");

        SdlSetKeyBindings("");
        SDL_zero(event);
        event.type = SDL_KEYDOWN;
        event.key.keysym.scancode = SDL_SCANCODE_T;
        event.key.keysym.sym = SDLK_t;
        SdlSwcKeymapTranslateEvent(&event);
        Expect(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_t, "SWC: unchanged keys pass");
        Expect(SdlSwcKeymapMouseButtons(SDL_BUTTON_LMASK) == SDL_BUTTON_LMASK, "SWC: unchanged buttons pass");
    }

    SDL_Quit();
    if (g_nFailures != 0)
        return 1;
    puts("PASS: rebound WC1 and SWC keys and mouse inputs stand in for the original ones, only in flight.");
    return 0;
}
