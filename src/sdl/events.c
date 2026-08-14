#include "wc1.h"

#include "video_internal.h"

static int Wc1SdlTranslateVirtualKey(SDL_Keycode key)
{
    if (key >= SDLK_a && key <= SDLK_z)
        return (int)(key - SDLK_a + 'A');
    if (key >= SDLK_F1 && key <= SDLK_F12)
        return (int)(key - SDLK_F1 + VK_F1);
    switch (key) {
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
        return 0x0d;
    case SDLK_ESCAPE:
        return 0x1b;
    case SDLK_BACKSPACE:
        return 0x08;
    case SDLK_TAB:
        return 0x09;
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:
        return VK_SHIFT;
    case SDLK_LCTRL:
    case SDLK_RCTRL:
        return VK_CONTROL;
    case SDLK_LALT:
    case SDLK_RALT:
        return 0x12;
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
        return 0xbc;
    case SDLK_PERIOD:
        return 0xbe;
    default:
        return key >= 0 && key <= 0xffff ? (int)key : 0;
    }
}

int Wc1SdlTranslateScanCode(SDL_Scancode scanCode)
{
    switch (scanCode) {
    case SDL_SCANCODE_ESCAPE:
        return 0x01;
    case SDL_SCANCODE_1:
        return 0x02;
    case SDL_SCANCODE_2:
        return 0x03;
    case SDL_SCANCODE_3:
        return 0x04;
    case SDL_SCANCODE_4:
        return 0x05;
    case SDL_SCANCODE_5:
        return 0x06;
    case SDL_SCANCODE_6:
        return 0x07;
    case SDL_SCANCODE_7:
        return 0x08;
    case SDL_SCANCODE_8:
        return 0x09;
    case SDL_SCANCODE_9:
        return 0x0a;
    case SDL_SCANCODE_0:
        return 0x0b;
    case SDL_SCANCODE_MINUS:
        return 0x0c;
    case SDL_SCANCODE_EQUALS:
        return 0x0d;
    case SDL_SCANCODE_BACKSPACE:
        return 0x0e;
    case SDL_SCANCODE_TAB:
        return 0x0f;
    case SDL_SCANCODE_Q:
        return 0x10;
    case SDL_SCANCODE_W:
        return 0x11;
    case SDL_SCANCODE_E:
        return 0x12;
    case SDL_SCANCODE_R:
        return 0x13;
    case SDL_SCANCODE_T:
        return 0x14;
    case SDL_SCANCODE_Y:
        return 0x15;
    case SDL_SCANCODE_U:
        return 0x16;
    case SDL_SCANCODE_I:
        return 0x17;
    case SDL_SCANCODE_O:
        return 0x18;
    case SDL_SCANCODE_P:
        return 0x19;
    case SDL_SCANCODE_LEFTBRACKET:
        return 0x1a;
    case SDL_SCANCODE_RIGHTBRACKET:
        return 0x1b;
    case SDL_SCANCODE_RETURN:
    case SDL_SCANCODE_KP_ENTER:
        return 0x1c;
    case SDL_SCANCODE_LCTRL:
    case SDL_SCANCODE_RCTRL:
        return 0x1d;
    case SDL_SCANCODE_A:
        return 0x1e;
    case SDL_SCANCODE_S:
        return 0x1f;
    case SDL_SCANCODE_D:
        return 0x20;
    case SDL_SCANCODE_F:
        return 0x21;
    case SDL_SCANCODE_G:
        return 0x22;
    case SDL_SCANCODE_H:
        return 0x23;
    case SDL_SCANCODE_J:
        return 0x24;
    case SDL_SCANCODE_K:
        return 0x25;
    case SDL_SCANCODE_L:
        return 0x26;
    case SDL_SCANCODE_SEMICOLON:
        return 0x27;
    case SDL_SCANCODE_APOSTROPHE:
        return 0x28;
    case SDL_SCANCODE_GRAVE:
        return 0x29;
    case SDL_SCANCODE_LSHIFT:
        return 0x2a;
    case SDL_SCANCODE_BACKSLASH:
        return 0x2b;
    case SDL_SCANCODE_Z:
        return 0x2c;
    case SDL_SCANCODE_X:
        return 0x2d;
    case SDL_SCANCODE_C:
        return 0x2e;
    case SDL_SCANCODE_V:
        return 0x2f;
    case SDL_SCANCODE_B:
        return 0x30;
    case SDL_SCANCODE_N:
        return 0x31;
    case SDL_SCANCODE_M:
        return 0x32;
    case SDL_SCANCODE_COMMA:
        return 0x33;
    case SDL_SCANCODE_PERIOD:
        return 0x34;
    case SDL_SCANCODE_SLASH:
        return 0x35;
    case SDL_SCANCODE_RSHIFT:
        return 0x36;
    case SDL_SCANCODE_KP_MULTIPLY:
        return 0x37;
    case SDL_SCANCODE_LALT:
    case SDL_SCANCODE_RALT:
        return 0x38;
    case SDL_SCANCODE_SPACE:
        return 0x39;
    case SDL_SCANCODE_CAPSLOCK:
        return 0x3a;
    case SDL_SCANCODE_F1:
        return 0x3b;
    case SDL_SCANCODE_F2:
        return 0x3c;
    case SDL_SCANCODE_F3:
        return 0x3d;
    case SDL_SCANCODE_F4:
        return 0x3e;
    case SDL_SCANCODE_F5:
        return 0x3f;
    case SDL_SCANCODE_F6:
        return 0x40;
    case SDL_SCANCODE_F7:
        return 0x41;
    case SDL_SCANCODE_F8:
        return 0x42;
    case SDL_SCANCODE_F9:
        return 0x43;
    case SDL_SCANCODE_F10:
        return 0x44;
    case SDL_SCANCODE_NUMLOCKCLEAR:
        return 0x45;
    case SDL_SCANCODE_SCROLLLOCK:
        return 0x46;
    case SDL_SCANCODE_HOME:
    case SDL_SCANCODE_KP_7:
        return 0x47;
    case SDL_SCANCODE_UP:
    case SDL_SCANCODE_KP_8:
        return 0x48;
    case SDL_SCANCODE_PAGEUP:
    case SDL_SCANCODE_KP_9:
        return 0x49;
    case SDL_SCANCODE_KP_MINUS:
        return 0x4a;
    case SDL_SCANCODE_LEFT:
    case SDL_SCANCODE_KP_4:
        return 0x4b;
    case SDL_SCANCODE_KP_5:
        return 0x4c;
    case SDL_SCANCODE_RIGHT:
    case SDL_SCANCODE_KP_6:
        return 0x4d;
    case SDL_SCANCODE_KP_PLUS:
        return 0x4e;
    case SDL_SCANCODE_END:
    case SDL_SCANCODE_KP_1:
        return 0x4f;
    case SDL_SCANCODE_DOWN:
    case SDL_SCANCODE_KP_2:
        return 0x50;
    case SDL_SCANCODE_PAGEDOWN:
    case SDL_SCANCODE_KP_3:
        return 0x51;
    case SDL_SCANCODE_INSERT:
    case SDL_SCANCODE_KP_0:
        return 0x52;
    case SDL_SCANCODE_DELETE:
    case SDL_SCANCODE_KP_PERIOD:
        return 0x53;
    case SDL_SCANCODE_NONUSBACKSLASH:
        return 0x56;
    case SDL_SCANCODE_F11:
        return 0x57;
    case SDL_SCANCODE_F12:
        return 0x58;
    default:
        return 0;
    }
}

static void Wc1SdlQueueMouseMotion(unsigned short x, unsigned short y,
                                   int primaryButton,
                                   int secondaryButton)
{
    InputEvent *queued;

    queued = g_pInputEventTail_0046da94;
    if (queued != 0 && queued->type == 13) {
        queued->x = (short)x;
        queued->y = (short)y;
        queued->primaryButton = (short)primaryButton;
        queued->secondaryButton = (short)secondaryButton;
        queued->modifiers &= ~6U;
        if (primaryButton != 0)
            queued->modifiers |= 2;
        if (secondaryButton != 0)
            queued->modifiers |= 4;
        return;
    }
    QueueInputEvent(13, x, y, 0, primaryButton, secondaryButton, 0);
}

void Wc1SdlStartEventPump(void)
{
    DAT_005a8a3c = 1;
}

void Wc1SdlPumpEvents(void)
{
    SDL_Event event;

    Wc1SdlServiceDosAdlibMusic();
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            ShutdownGameWindow();
            return;
        } else if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                ShutdownGameWindow();
                return;
            } else if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
                       event.window.event == SDL_WINDOWEVENT_DISPLAY_CHANGED) {
                SDL_Window *window;

                window = SDL_GetWindowFromID(event.window.windowID);
                if (window != 0 && SDL_GetWindowMouseGrab(window)) {
                    SDL_SetWindowMouseGrab(window, SDL_FALSE);
                    SDL_SetWindowMouseGrab(window, SDL_TRUE);
                }
            }
        } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            SDL_Window *window;
            SDL_bool mouseGrabbed;
            Uint32 fullscreenFlags;
            Uint32 windowFlags;
            int enterPressed;
            int fullscreenModifierPressed;
            int fullscreenShortcut;
            int pressed;
            int scanCode;
            int virtualKey;

            pressed = event.type == SDL_KEYDOWN;
            if (pressed && event.key.repeat == 0 &&
                event.key.keysym.sym == SDLK_q &&
                (event.key.keysym.mod & KMOD_GUI) != 0) {
                ShutdownGameWindow();
                return;
            }
            enterPressed =
                event.key.keysym.sym == SDLK_RETURN ||
                event.key.keysym.sym == SDLK_KP_ENTER ||
                event.key.keysym.scancode == SDL_SCANCODE_RETURN ||
                event.key.keysym.scancode == SDL_SCANCODE_KP_ENTER;
#ifdef __APPLE__
            fullscreenModifierPressed =
                (event.key.keysym.mod & KMOD_GUI) != 0 ||
                (SDL_GetModState() & KMOD_GUI) != 0;
#else
            fullscreenModifierPressed =
                (event.key.keysym.mod & KMOD_ALT) != 0 ||
                (SDL_GetModState() & KMOD_ALT) != 0 ||
                g_abInputKeyState_0059a860[0x38] != 0;
#endif
            fullscreenShortcut =
                enterPressed && fullscreenModifierPressed;
            if (fullscreenShortcut) {
                if (pressed && event.key.repeat == 0) {
                    window = SDL_GetWindowFromID(event.key.windowID);
                    if (window == 0)
                        window = SDL_GetKeyboardFocus();
                    if (window == 0)
                        window = (SDL_Window *)DAT_00486074;
                    if (window == 0)
                        window = (SDL_Window *)DAT_005a89a0;
                    if (window != 0) {
                        windowFlags = SDL_GetWindowFlags(window);
                        fullscreenFlags =
                            (windowFlags & SDL_WINDOW_FULLSCREEN) != 0
                                ? 0
                                : SDL_WINDOW_FULLSCREEN_DESKTOP;
                        mouseGrabbed = SDL_GetWindowMouseGrab(window);
                        if (mouseGrabbed)
                            SDL_SetWindowMouseGrab(window, SDL_FALSE);
                        if (SDL_SetWindowFullscreen(window,
                                                    fullscreenFlags) != 0) {
                            fprintf(stderr,
                                    "Unable to toggle fullscreen: %s\n",
                                    SDL_GetError());
                        }
                        if (mouseGrabbed)
                            SDL_SetWindowMouseGrab(window, SDL_TRUE);
                    }
                }
                continue;
            }
            scanCode = Wc1SdlTranslateScanCode(event.key.keysym.scancode);
            virtualKey = Wc1SdlTranslateVirtualKey(event.key.keysym.sym);
            if ((event.key.keysym.mod & KMOD_ALT) != 0 ||
                event.key.keysym.scancode == SDL_SCANCODE_LALT ||
                event.key.keysym.scancode == SDL_SCANCODE_RALT)
                DAT_005a8964 = pressed ? (unsigned int)virtualKey : 0;
            if (event.key.keysym.scancode == SDL_SCANCODE_F1)
                DAT_004650ac = pressed && event.key.repeat == 0;
            if (pressed && scanCode == 1)
                DAT_0059ab58 = 1;
            if (scanCode != 0) {
                if (DAT_0046505c != 0)
                    QueueInputEvent(pressed ? 3 : 4, 0, 0,
                                    (unsigned short)virtualKey, 0, 0, 0);
                QueueInputEvent(pressed ? 3 : 4, 0, 0, (unsigned short)scanCode,
                                0, 0, 0);
                SetInputKeyState(scanCode, (unsigned char)pressed);
            }
            if (!pressed) {
                g_dwDebugOverlayKey_00469648 = (DWORD)virtualKey;
                g_dwDebugOverlayKeyLatch_0046964c = (DWORD)virtualKey;
            }
        } else if (event.type == SDL_MOUSEMOTION ||
                   event.type == SDL_MOUSEBUTTONDOWN ||
                   event.type == SDL_MOUSEBUTTONUP) {
            SDL_Window *window;
            Uint32 buttons;
            int mouseX;
            int mouseY;
            int primaryButton;
            int secondaryButton;

            window = SDL_GetWindowFromID(event.type == SDL_MOUSEMOTION
                                             ? event.motion.windowID
                                             : event.button.windowID);
            buttons = SDL_GetMouseState(&mouseX, &mouseY);
            if (event.type != SDL_MOUSEMOTION) {
                if (event.type == SDL_MOUSEBUTTONDOWN)
                    buttons |= SDL_BUTTON(event.button.button);
                else
                    buttons &= ~SDL_BUTTON(event.button.button);
            }
            Wc1SdlMapWindowToLogical(
                window, mouseX, mouseY, &mouseX, &mouseY);
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

            if (event.type == SDL_MOUSEMOTION) {
                if (g_bPointerMovedByKeyboard_005a7d54 != 0) {
                    g_bPointerMovedByKeyboard_005a7d54 = 0;
                    continue;
                }
                Wc1SdlQueueMouseMotion(
                    (unsigned short)mouseX, (unsigned short)mouseY,
                    primaryButton, secondaryButton);
            } else {
                QueueInputEvent(event.type == SDL_MOUSEBUTTONDOWN ? 2 : 1,
                                (unsigned short)mouseX, (unsigned short)mouseY,
                                0, primaryButton, secondaryButton, 0);
            }
            g_nHostMouseMessageX_005a8990 = mouseX;
            g_nHostMouseMessageY_005a8994 = mouseY;
            g_bHostPrimaryMouseButton_005a8998 = primaryButton;
            g_bHostSecondaryMouseButton_005a899c = secondaryButton;
        }
    }
}
