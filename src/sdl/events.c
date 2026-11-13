#include "wc1.h"

#include "video_internal.h"

static int SdlTranslateVirtualKey(SDL_Keycode key)
{
    if (key >= SDLK_a && key <= SDLK_z)
        return key - SDLK_a + 'A';
    if (key >= SDLK_F1 && key <= SDLK_F12)
        return key - SDLK_F1 + VK_F1;
    switch (key) {
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
        return VK_RETURN;
    case SDLK_ESCAPE:
        return VK_ESCAPE;
    case SDLK_BACKSPACE:
        return VK_BACK;
    case SDLK_TAB:
        return VK_TAB;
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:
        return VK_SHIFT;
    case SDLK_LCTRL:
    case SDLK_RCTRL:
        return VK_CONTROL;
    case SDLK_LALT:
    case SDLK_RALT:
        return VK_MENU;
    case SDLK_PAGEUP:
        return VK_PRIOR;
    case SDLK_PAGEDOWN:
        return VK_NEXT;
    case SDLK_END:
        return VK_END;
    case SDLK_HOME:
        return VK_HOME;
    case SDLK_LEFT:
        return VK_LEFT;
    case SDLK_UP:
        return VK_UP;
    case SDLK_RIGHT:
        return VK_RIGHT;
    case SDLK_DOWN:
        return VK_DOWN;
    case SDLK_INSERT:
        return VK_INSERT;
    case SDLK_DELETE:
        return VK_DELETE;
    case SDLK_COMMA:
        return VK_OEM_COMMA;
    case SDLK_PERIOD:
        return VK_OEM_PERIOD;
    default:
        return key >= 0 && key <= 0xffff ? key : 0;
    }
}

int SdlTranslateScanCode(SDL_Scancode scanCode)
{
    switch (scanCode) {
    case SDL_SCANCODE_ESCAPE:
        return SCAN_ESCAPE;
    case SDL_SCANCODE_1:
        return SCAN_1;
    case SDL_SCANCODE_2:
        return SCAN_2;
    case SDL_SCANCODE_3:
        return SCAN_3;
    case SDL_SCANCODE_4:
        return SCAN_4;
    case SDL_SCANCODE_5:
        return SCAN_5;
    case SDL_SCANCODE_6:
        return SCAN_6;
    case SDL_SCANCODE_7:
        return SCAN_7;
    case SDL_SCANCODE_8:
        return SCAN_8;
    case SDL_SCANCODE_9:
        return SCAN_9;
    case SDL_SCANCODE_0:
        return SCAN_0;
    case SDL_SCANCODE_MINUS:
        return SCAN_MINUS;
    case SDL_SCANCODE_EQUALS:
        return SCAN_EQUALS;
    case SDL_SCANCODE_BACKSPACE:
        return SCAN_BACKSPACE;
    case SDL_SCANCODE_TAB:
        return SCAN_TAB;
    case SDL_SCANCODE_Q:
        return SCAN_Q;
    case SDL_SCANCODE_W:
        return SCAN_W;
    case SDL_SCANCODE_E:
        return SCAN_E;
    case SDL_SCANCODE_R:
        return SCAN_R;
    case SDL_SCANCODE_T:
        return SCAN_T;
    case SDL_SCANCODE_Y:
        return SCAN_Y;
    case SDL_SCANCODE_U:
        return SCAN_U;
    case SDL_SCANCODE_I:
        return SCAN_I;
    case SDL_SCANCODE_O:
        return SCAN_O;
    case SDL_SCANCODE_P:
        return SCAN_P;
    case SDL_SCANCODE_LEFTBRACKET:
        return SCAN_LEFTBRACKET;
    case SDL_SCANCODE_RIGHTBRACKET:
        return SCAN_RIGHTBRACKET;
    case SDL_SCANCODE_RETURN:
    case SDL_SCANCODE_KP_ENTER:
        return SCAN_RETURN;
    case SDL_SCANCODE_LCTRL:
    case SDL_SCANCODE_RCTRL:
        return SCAN_CONTROL;
    case SDL_SCANCODE_A:
        return SCAN_A;
    case SDL_SCANCODE_S:
        return SCAN_S;
    case SDL_SCANCODE_D:
        return SCAN_D;
    case SDL_SCANCODE_F:
        return SCAN_F;
    case SDL_SCANCODE_G:
        return SCAN_G;
    case SDL_SCANCODE_H:
        return SCAN_H;
    case SDL_SCANCODE_J:
        return SCAN_J;
    case SDL_SCANCODE_K:
        return SCAN_K;
    case SDL_SCANCODE_L:
        return SCAN_L;
    case SDL_SCANCODE_SEMICOLON:
        return SCAN_SEMICOLON;
    case SDL_SCANCODE_APOSTROPHE:
        return SCAN_APOSTROPHE;
    case SDL_SCANCODE_GRAVE:
        return SCAN_GRAVE;
    case SDL_SCANCODE_LSHIFT:
        return SCAN_LSHIFT;
    case SDL_SCANCODE_BACKSLASH:
        return SCAN_BACKSLASH;
    case SDL_SCANCODE_Z:
        return SCAN_Z;
    case SDL_SCANCODE_X:
        return SCAN_X;
    case SDL_SCANCODE_C:
        return SCAN_C;
    case SDL_SCANCODE_V:
        return SCAN_V;
    case SDL_SCANCODE_B:
        return SCAN_B;
    case SDL_SCANCODE_N:
        return SCAN_N;
    case SDL_SCANCODE_M:
        return SCAN_M;
    case SDL_SCANCODE_COMMA:
        return SCAN_COMMA;
    case SDL_SCANCODE_PERIOD:
        return SCAN_PERIOD;
    case SDL_SCANCODE_SLASH:
        return SCAN_SLASH;
    case SDL_SCANCODE_RSHIFT:
        return SCAN_RSHIFT;
    case SDL_SCANCODE_KP_MULTIPLY:
        return SCAN_KP_MULTIPLY;
    case SDL_SCANCODE_LALT:
    case SDL_SCANCODE_RALT:
        return SCAN_ALT;
    case SDL_SCANCODE_SPACE:
        return SCAN_SPACE;
    case SDL_SCANCODE_CAPSLOCK:
        return SCAN_CAPSLOCK;
    case SDL_SCANCODE_F1:
        return SCAN_F1;
    case SDL_SCANCODE_F2:
        return SCAN_F2;
    case SDL_SCANCODE_F3:
        return SCAN_F3;
    case SDL_SCANCODE_F4:
        return SCAN_F4;
    case SDL_SCANCODE_F5:
        return SCAN_F5;
    case SDL_SCANCODE_F6:
        return SCAN_F6;
    case SDL_SCANCODE_F7:
        return SCAN_F7;
    case SDL_SCANCODE_F8:
        return SCAN_F8;
    case SDL_SCANCODE_F9:
        return SCAN_F9;
    case SDL_SCANCODE_F10:
        return SCAN_F10;
    case SDL_SCANCODE_NUMLOCKCLEAR:
        return SCAN_NUMLOCK;
    case SDL_SCANCODE_SCROLLLOCK:
        return SCAN_SCROLLLOCK;
    case SDL_SCANCODE_HOME:
    case SDL_SCANCODE_KP_7:
        return SCAN_HOME;
    case SDL_SCANCODE_UP:
    case SDL_SCANCODE_KP_8:
        return SCAN_UP;
    case SDL_SCANCODE_PAGEUP:
    case SDL_SCANCODE_KP_9:
        return SCAN_PAGEUP;
    case SDL_SCANCODE_KP_MINUS:
        return SCAN_KP_MINUS;
    case SDL_SCANCODE_LEFT:
    case SDL_SCANCODE_KP_4:
        return SCAN_LEFT;
    case SDL_SCANCODE_KP_5:
        return SCAN_KP_5;
    case SDL_SCANCODE_RIGHT:
    case SDL_SCANCODE_KP_6:
        return SCAN_RIGHT;
    case SDL_SCANCODE_KP_PLUS:
        return SCAN_KP_PLUS;
    case SDL_SCANCODE_END:
    case SDL_SCANCODE_KP_1:
        return SCAN_END;
    case SDL_SCANCODE_DOWN:
    case SDL_SCANCODE_KP_2:
        return SCAN_DOWN;
    case SDL_SCANCODE_PAGEDOWN:
    case SDL_SCANCODE_KP_3:
        return SCAN_PAGEDOWN;
    case SDL_SCANCODE_INSERT:
    case SDL_SCANCODE_KP_0:
        return SCAN_INSERT;
    case SDL_SCANCODE_DELETE:
    case SDL_SCANCODE_KP_PERIOD:
        return SCAN_DELETE;
    case SDL_SCANCODE_NONUSBACKSLASH:
        return SCAN_NONUS_BACKSLASH;
    case SDL_SCANCODE_F11:
        return SCAN_F11;
    case SDL_SCANCODE_F12:
        return SCAN_F12;
    default:
        return SCAN_NONE;
    }
}

static void SdlQueueMouseMotion(unsigned short x, unsigned short y,
                                   int primaryButton,
                                   int secondaryButton)
{
    InputEvent *queued;

    queued = pInputEventTail;
    if (queued != 0 && queued->type == INPUT_EVENT_MOUSE_MOVE) {
        queued->x = x;
        queued->y = y;
        queued->primaryButton = primaryButton;
        queued->secondaryButton = secondaryButton;
        queued->modifiers &= ~(unsigned int)(INPUT_MOD_PRIMARY_BUTTON |
                                            INPUT_MOD_SECONDARY_BUTTON);
        if (primaryButton != 0)
            queued->modifiers |= INPUT_MOD_PRIMARY_BUTTON;
        if (secondaryButton != 0)
            queued->modifiers |= INPUT_MOD_SECONDARY_BUTTON;
        return;
    }
    QueueInputEvent(INPUT_EVENT_MOUSE_MOVE, x, y, 0,
                    primaryButton, secondaryButton, 0);
}

/* The pointer is confined only while spaceflight is running, unpaused, and
 * focused.  Mouse flight steers by the pointer's distance from the viewport
 * centre, so a hard turn parks it against the window edge and needs the
 * confinement; anywhere else it would only trap the pointer. */
static int g_bSdlMouseGrabRequested;
static int g_bSdlWindowFocused = 1;
static int g_nSdlMouseGrabSuspendDepth;

static void SdlApplyMouseGrab(void)
{
    SDL_Window *window;

    window = hMainWindow;
    if (window == 0)
        return;
    SDL_SetWindowMouseGrab(
        window,
        g_bSdlMouseGrabRequested && g_bSdlWindowFocused &&
                g_nSdlMouseGrabSuspendDepth == 0
            ? SDL_TRUE
            : SDL_FALSE);
}

void SdlSetMouseGrab(int enabled)
{
    g_bSdlMouseGrabRequested = enabled;
    SdlApplyMouseGrab();
}

/* Modal waits release the pointer without disturbing whether spaceflight
 * wants it, so a wait entered outside flight cannot switch the grab on.
 * The depth keeps nested waits honest. */
void SdlSuspendMouseGrab(void)
{
    g_nSdlMouseGrabSuspendDepth++;
    SdlApplyMouseGrab();
}

void SdlResumeMouseGrab(void)
{
    if (g_nSdlMouseGrabSuspendDepth > 0)
        g_nSdlMouseGrabSuspendDepth--;
    SdlApplyMouseGrab();
}

static int SdlHandleWindowEvent(const SDL_WindowEvent *event)
{
    SDL_Window *window;

    if (event->event == SDL_WINDOWEVENT_CLOSE) {
        ShutdownGameWindow();
        return 1;
    }
    if (event->event == SDL_WINDOWEVENT_FOCUS_LOST ||
        event->event == SDL_WINDOWEVENT_FOCUS_GAINED) {
        g_bSdlWindowFocused =
            event->event == SDL_WINDOWEVENT_FOCUS_GAINED;
        SdlApplyMouseGrab();
        return 0;
    }
    if (event->event != SDL_WINDOWEVENT_SIZE_CHANGED &&
        event->event != SDL_WINDOWEVENT_DISPLAY_CHANGED)
        return 0;
    window = SDL_GetWindowFromID(event->windowID);
    if (window != 0 && SDL_GetWindowMouseGrab(window)) {
        SDL_SetWindowMouseGrab(window, SDL_FALSE);
        SDL_SetWindowMouseGrab(window, SDL_TRUE);
    }
    return 0;
}

static SDL_Window *SdlGetFullscreenWindow(Uint32 windowId)
{
    SDL_Window *window;

    window = SDL_GetWindowFromID(windowId);
    if (window == 0)
        window = SDL_GetKeyboardFocus();
    if (window == 0)
        window = hDIBParentWindow;
    if (window == 0)
        window = hMainWindow;
    return window;
}

static int SdlIsFullscreenShortcut(const SDL_KeyboardEvent *event)
{
    int enterPressed;
    int modifierPressed;
    int changed;

    changed = SdlKeymapFullscreenShortcut(event->keysym.scancode,
                                          event->keysym.mod | SDL_GetModState());
    if (changed >= 0)
        return changed;
    enterPressed = event->keysym.sym == SDLK_RETURN ||
        event->keysym.sym == SDLK_KP_ENTER ||
        event->keysym.scancode == SDL_SCANCODE_RETURN ||
        event->keysym.scancode == SDL_SCANCODE_KP_ENTER;
#ifdef __APPLE__
    modifierPressed = (event->keysym.mod & KMOD_GUI) != 0 ||
        (SDL_GetModState() & KMOD_GUI) != 0;
#else
    modifierPressed = (event->keysym.mod & KMOD_ALT) != 0 ||
        (SDL_GetModState() & KMOD_ALT) != 0 ||
        abInputKeyStateArray[SCAN_ALT] != 0;
#endif
    return enterPressed && modifierPressed;
}

static void SdlToggleFullscreen(Uint32 windowId)
{
    SDL_Window *window;
    SDL_bool mouseGrabbed;
    Uint32 fullscreenFlags;
    Uint32 windowFlags;

    window = SdlGetFullscreenWindow(windowId);
    if (window == 0)
        return;
    windowFlags = SDL_GetWindowFlags(window);
    fullscreenFlags = (windowFlags & SDL_WINDOW_FULLSCREEN) != 0
        ? 0
        : SDL_WINDOW_FULLSCREEN_DESKTOP;
    mouseGrabbed = SDL_GetWindowMouseGrab(window);
    if (mouseGrabbed)
        SDL_SetWindowMouseGrab(window, SDL_FALSE);
    if (SDL_SetWindowFullscreen(window, fullscreenFlags) != 0) {
        fprintf(stderr, "Unable to toggle fullscreen: %s\n",
                SDL_GetError());
    }
    if (mouseGrabbed)
        SDL_SetWindowMouseGrab(window, SDL_TRUE);
}

int SdlHandleSwcEvent(SDL_Window *window, SDL_Event *event, int flight)
{
    SdlLogJoystickEvent(event);
    switch (event->type) {
    case SDL_JOYDEVICEADDED:
    case SDL_JOYDEVICEREMOVED:
        SdlHandleJoystickDeviceEvent(event->type, event->jdevice.which);
        return 1;
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
    case SDL_JOYHATMOTION:
        if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS))
            return 1;
        SdlTranslateSwcJoystickEvent(event, flight);
        return event->type == 0;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        if (SdlIsFullscreenShortcut(&event->key)) {
            if (event->type == SDL_KEYDOWN && !event->key.repeat)
                SdlToggleFullscreen(SDL_GetWindowID(window));
            return 1;
        }
        /* Rebound option keys apply on every SWC screen. */
        if (SdlSwcKeymapTranslateOptionKey(&event->key) < 0)
            return 1;
        if (SdlHandleSwcOptionKey(window, &event->key))
            return 1;
        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_q &&
            (event->key.keysym.mod & KMOD_GUI))
            event->type = SDL_QUIT;
        break;
    default:
        break;
    }
    return 0;
}

/* Hands the game one key transition, as WC1's keyboard reads it. */
static void SdlQueueGameKey(SDL_Scancode key, SDL_Keycode symbol, Uint16 modifiers,
                            int pressed, int repeat)
{
    int scanCode;
    int virtualKey;

    scanCode = SdlTranslateScanCode(key);
    virtualKey = SdlTranslateVirtualKey(symbol);
    if ((modifiers & KMOD_ALT) != 0 ||
        key == SDL_SCANCODE_LALT ||
        key == SDL_SCANCODE_RALT)
        nSystemKeyDown = pressed ? (unsigned int)virtualKey : 0;
    if (key == SDL_SCANCODE_F1)
        bF1KeyLatch = pressed && repeat == 0;
    if (pressed && scanCode == SCAN_ESCAPE)
        bEscapePressedFlag = 1;
    if (scanCode != SCAN_NONE) {
        if (bKeyEventQueueEnabled != 0) {
            QueueInputEvent(pressed ? INPUT_EVENT_KEY_DOWN : INPUT_EVENT_KEY_UP,
                            0, 0,
                            (unsigned short)virtualKey, 0, 0, 0);
        }
        QueueInputEvent(pressed ? INPUT_EVENT_KEY_DOWN : INPUT_EVENT_KEY_UP,
                        0, 0,
                        (unsigned short)scanCode, 0, 0, 0);
        SetInputKeyState(scanCode, pressed);
    }
    if (!pressed) {
        dwDebugOverlayKey = virtualKey;
        dwDebugOverlayKeyLatch = virtualKey;
    }
}

/* Mouse buttons bound to an action press its key, repeating like a held
 * key; each records the key it pressed so the release matches it. */
#define SDL_MOUSE_KEY_DELAY 250
#define SDL_MOUSE_KEY_REPEAT 33

static SDL_Scancode g_aSdlMouseKeys[8];
static Uint32 g_adwSdlMouseKeyRepeat[8];

static void SdlPressMouseKey(Uint8 button, SDL_Scancode key, int pressed)
{
    if (button >= sizeof(g_aSdlMouseKeys) / sizeof(g_aSdlMouseKeys[0]))
        return;
    if (pressed) {
        if (key == SDL_SCANCODE_UNKNOWN)
            return;
        g_aSdlMouseKeys[button] = key;
        g_adwSdlMouseKeyRepeat[button] = SDL_GetTicks() + SDL_MOUSE_KEY_DELAY;
        SdlQueueGameKey(key, SDL_GetKeyFromScancode(key), 0, 1, 0);
    } else if (g_aSdlMouseKeys[button] != SDL_SCANCODE_UNKNOWN) {
        key = g_aSdlMouseKeys[button];
        g_aSdlMouseKeys[button] = SDL_SCANCODE_UNKNOWN;
        SdlQueueGameKey(key, SDL_GetKeyFromScancode(key), 0, 0, 0);
    }
}

static void SdlRepeatMouseKeys(void)
{
    Uint32 now = SDL_GetTicks();
    size_t button;

    for (button = 0; button < sizeof(g_aSdlMouseKeys) / sizeof(g_aSdlMouseKeys[0]); button++) {
        SDL_Scancode key = g_aSdlMouseKeys[button];

        if (key == SDL_SCANCODE_UNKNOWN || (Sint32)(now - g_adwSdlMouseKeyRepeat[button]) < 0)
            continue;
        g_adwSdlMouseKeyRepeat[button] = now + SDL_MOUSE_KEY_REPEAT;
        SdlQueueGameKey(key, SDL_GetKeyFromScancode(key), 0, 1, 1);
    }
}

static int SdlHandleKeyboardEvent(const SDL_KeyboardEvent *event)
{
    int pressed;
    SDL_Scancode key;
    SDL_Keycode symbol;

    pressed = event->type == SDL_KEYDOWN;
    if (pressed && event->repeat == 0 && event->keysym.sym == SDLK_q &&
        (event->keysym.mod & KMOD_GUI) != 0) {
        ShutdownGameWindow();
        return 1;
    }
    if (SdlIsFullscreenShortcut(event)) {
        if (pressed && event->repeat == 0)
            SdlToggleFullscreen(event->windowID);
        return 0;
    }
    /* In flight, rebound keys stand in for the keys the game reads. */
    key = SdlKeymapTranslateKey(event->keysym.scancode, event->keysym.mod, pressed);
    if (key == SDL_SCANCODE_UNKNOWN)
        return 0;
    symbol = key == event->keysym.scancode ? event->keysym.sym : SDL_GetKeyFromScancode(key);
    SdlQueueGameKey(key, symbol, event->keysym.mod, pressed, event->repeat);
    return 0;
}

static void SdlHandleMouseWheelEvent(const SDL_MouseWheelEvent *event)
{
    int scanCode;
    int wheelY;

    wheelY = event->y;
    if (event->direction == SDL_MOUSEWHEEL_FLIPPED)
        wheelY = -wheelY;
    if (wheelY == 0)
        return;
    scanCode = wheelY > 0 ? SCAN_EQUALS : SCAN_MINUS;
    if (SdlKeymapMouseActive()) {
        /* In flight, the wheel presses the key of the action bound to it. */
        SDL_Scancode key = SdlKeymapWheelKey(wheelY);

        if (key == SDL_SCANCODE_UNKNOWN)
            return;
        scanCode = SdlTranslateScanCode(key);
        if (scanCode == SCAN_NONE)
            return;
    }
    /* player_input samples one transition before consuming the remaining
       queue, so lead with the release for this impulse. */
    QueueInputEvent(INPUT_EVENT_KEY_UP, 0, 0, (unsigned short)scanCode, 0, 0, 0);
    QueueInputEvent(INPUT_EVENT_KEY_DOWN, 0, 0, (unsigned short)scanCode, 0, 0, 0);
}

static void SdlHandleMouseEvent(const SDL_Event *event)
{
    SDL_Window *window;
    Uint32 buttons;
    int mouseX;
    int mouseY;
    int primaryButton;
    int secondaryButton;

    window = SDL_GetWindowFromID(event->type == SDL_MOUSEMOTION
                                     ? event->motion.windowID
                                     : event->button.windowID);
    if (event->type == SDL_MOUSEMOTION) {
        mouseX = event->motion.x;
        mouseY = event->motion.y;
        buttons = event->motion.state;
    } else {
        mouseX = event->button.x;
        mouseY = event->button.y;
        buttons = SDL_GetMouseState(0, 0);
        if (event->type == SDL_MOUSEBUTTONDOWN)
            buttons |= SDL_BUTTON(event->button.button);
        else
            buttons &= ~(Uint32)SDL_BUTTON(event->button.button);
    }
    SdlMapWindowToLogical(window, mouseX, mouseY, &mouseX, &mouseY);
    if (mouseX < 0)
        mouseX = 0;
    else if (mouseX > 319)
        mouseX = 319;
    if (mouseY < 0)
        mouseY = 0;
    else if (mouseY > 199)
        mouseY = 199;
    primaryButton = (buttons & SDL_BUTTON_LMASK) != 0;
    secondaryButton = (buttons & SDL_BUTTON_RMASK) != 0;
    if (SdlKeymapMouseActive()) {
        /* In flight, the bound buttons stand in for the game's two buttons,
           and other bound buttons press their action's key. */
        SdlKeymapGameButtons(buttons, &primaryButton, &secondaryButton);
        if (event->type != SDL_MOUSEMOTION && !SdlKeymapIsGameButton(event->button.button)) {
            SdlPressMouseKey(event->button.button,
                             SdlKeymapMouseKey(event->button.button),
                             event->type == SDL_MOUSEBUTTONDOWN);
            nHostMouseMessageX = mouseX;
            nHostMouseMessageY = mouseY;
            bHostPrimaryMouseButton = primaryButton;
            bHostSecondaryMouseButton = secondaryButton;
            return;
        }
    } else if (event->type == SDL_MOUSEBUTTONUP) {
        /* Release a key that a button pressed during flight. */
        SdlPressMouseKey(event->button.button, SDL_SCANCODE_UNKNOWN, 0);
    }

    if (event->type == SDL_MOUSEMOTION) {
        if (bPtrMovedByKeyboardState != 0) {
            bPtrMovedByKeyboardState = 0;
            return;
        }
        SdlQueueMouseMotion(mouseX,
                               mouseY,
                               primaryButton, secondaryButton);
    } else {
        QueueInputEvent(event->type == SDL_MOUSEBUTTONDOWN ?
                        INPUT_EVENT_BUTTON_DOWN : INPUT_EVENT_BUTTON_UP,
                        (unsigned short)mouseX, (unsigned short)mouseY,
                        0, primaryButton, secondaryButton, 0);
    }
    nHostMouseMessageX = mouseX;
    nHostMouseMessageY = mouseY;
    bHostPrimaryMouseButton = primaryButton;
    bHostSecondaryMouseButton = secondaryButton;
}

void SdlStartEventPump(void)
{
    bMainWindowAlive = 1;
}

void SdlPumpEvents(void)
{
    SDL_Event event;

    /* The SWC host owns its SDL window and event loop. Its calls into WC1's
       player_input consume only the flight events it has already queued. */
    if (SdlSwcFlightActive())
        return;

    SdlServiceOriginFxMusic();
    SdlRepeatMouseKeys();
    while (SDL_PollEvent(&event)) {
        if (SdlHandleTownsTextInputEvent(&event))
            continue;
        SdlLogJoystickEvent(&event);
        switch (event.type) {
        case SDL_QUIT:
            ShutdownGameWindow();
            return;
        case SDL_WINDOWEVENT:
            if (SdlHandleWindowEvent(&event.window))
                return;
            break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            if (SdlHandleKeyboardEvent(&event.key))
                return;
            break;
        case SDL_MOUSEWHEEL:
            SdlHandleMouseWheelEvent(&event.wheel);
            break;
        case SDL_KEYMAPCHANGED:
            SdlKeymapInvalidate();
            break;
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            SdlHandleMouseEvent(&event);
            break;
        case SDL_JOYDEVICEADDED:
        case SDL_JOYDEVICEREMOVED:
            SdlHandleJoystickDeviceEvent(event.type,
                                             event.jdevice.which);
            break;
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            SdlHandleJoystickButtonEvent(event.cbutton.which,
                                             event.cbutton.button,
                                             event.type ==
                                                 SDL_CONTROLLERBUTTONDOWN,
                                             1);
            break;
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
            SdlHandleJoystickButtonEvent(event.jbutton.which,
                                             event.jbutton.button,
                                             event.type ==
                                                 SDL_JOYBUTTONDOWN,
                                             0);
            break;
        case SDL_JOYHATMOTION:
            SdlHandleJoystickHatEvent(event.jhat.which,
                                         event.jhat.hat,
                                         event.jhat.value);
            break;
        default:
            break;
        }
    }
}
