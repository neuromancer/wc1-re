#include "wc1.h"

#include <string.h>

int main(void)
{
    InputEventState input;
    SDL_Event event;
    SDL_Window *window;
    Viewport viewport;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0)
        return 1;
    window =
        SDL_CreateWindow("WC1 input test", 0, 0, 640, 400, SDL_WINDOW_HIDDEN);
    if (window == 0) {
        SDL_Quit();
        return 1;
    }
    memset(&viewport, 0, sizeof(viewport));
    viewport.right = 319;
    viewport.bottom = 199;
    g_stMouseCursorState_0059ab10.viewport = &viewport;
    Wc1SdlStartEventPump();

    memset(&event, 0, sizeof(event));
    event.type = SDL_KEYDOWN;
    event.key.windowID = SDL_GetWindowID(window);
    event.key.keysym.scancode = SDL_SCANCODE_A;
    event.key.keysym.sym = SDLK_a;
    if (SDL_PushEvent(&event) != 1 || PumpWindowMessages() == 0)
        return 1;
    if (g_abInputKeyState_0059a860[0x1e] != 1)
        return 1;
    if (GetNextInputEvent(&input) != 3 || input.value != 0x1e)
        return 1;

    event.type = SDL_KEYUP;
    if (SDL_PushEvent(&event) != 1 || PumpWindowMessages() == 0)
        return 1;
    if (g_abInputKeyState_0059a860[0x1e] != 0)
        return 1;
    if (GetNextInputEvent(&input) != 4)
        return 1;
    if (g_dwDebugOverlayKey_00469648 != 'A')
        return 1;

    memset(&event, 0, sizeof(event));
    event.type = SDL_MOUSEMOTION;
    event.motion.windowID = SDL_GetWindowID(window);
    event.motion.x = 320;
    event.motion.y = 200;
    if (SDL_PushEvent(&event) != 1 || PumpWindowMessages() == 0)
        return 1;
    if (GetNextInputEvent(&input) != 13 || input.x != 160 || input.y != 100)
        return 1;

    memset(&event, 0, sizeof(event));
    event.type = SDL_QUIT;
    if (SDL_PushEvent(&event) != 1 || PumpWindowMessages() != 0)
        return 1;

    FlushInputEvents();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
