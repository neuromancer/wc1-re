#include "wc1sdl.h"

#include <stdio.h>

DWORD Wc1SdlGetTicks(void)
{
    return SDL_GetTicks();
}

int Wc1SdlGetAsyncKeyState(int virtualKey)
{
    const Uint8 *keys;
    SDL_Scancode scanCode;

    SDL_PumpEvents();
    keys = SDL_GetKeyboardState(0);
    switch (virtualKey) {
    case VK_SHIFT:
        return (keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT])
            ? 0x8000 : 0;
    case VK_CONTROL:
        return (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL])
            ? 0x8000 : 0;
    case VK_CLEAR:
        scanCode = SDL_SCANCODE_KP_5;
        break;
    case VK_PRIOR:
        scanCode = SDL_SCANCODE_PAGEUP;
        break;
    case VK_NEXT:
        scanCode = SDL_SCANCODE_PAGEDOWN;
        break;
    case VK_END:
        scanCode = SDL_SCANCODE_END;
        break;
    case VK_HOME:
        scanCode = SDL_SCANCODE_HOME;
        break;
    case VK_LEFT:
        scanCode = SDL_SCANCODE_LEFT;
        break;
    case VK_UP:
        scanCode = SDL_SCANCODE_UP;
        break;
    case VK_RIGHT:
        scanCode = SDL_SCANCODE_RIGHT;
        break;
    case VK_DOWN:
        scanCode = SDL_SCANCODE_DOWN;
        break;
    case VK_INSERT:
        scanCode = SDL_SCANCODE_INSERT;
        break;
    case VK_DELETE:
        scanCode = SDL_SCANCODE_DELETE;
        break;
    case 0xbc:
        scanCode = SDL_SCANCODE_COMMA;
        break;
    case 0xbe:
        scanCode = SDL_SCANCODE_PERIOD;
        break;
    default:
        return 0;
    }
    return keys[scanCode] ? 0x8000 : 0;
}

void Wc1SdlOutputDebugString(const char *text)
{
    fputs(text, stderr);
    fflush(stderr);
}

int Wc1SdlSetCursorPosition(int x, int y)
{
    SDL_Window *window;
    int height;
    int width;

    window = SDL_GetKeyboardFocus();
    if (window == 0)
        window = SDL_GetMouseFocus();
    if (window == 0)
        return FALSE;
    SDL_GetWindowSize(window, &width, &height);
    SDL_WarpMouseInWindow(window, x * width / 320, y * height / 200);
    return TRUE;
}

void Wc1SdlSleep(DWORD milliseconds)
{
    SDL_Delay(milliseconds);
}
