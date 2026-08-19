#include "wc1.h"

#include <string.h>

int main(int argumentCount, char **arguments)
{
    InputEventState input;
    SDL_Event event;
    SDL_Event fullscreenEvent;
    SDL_Window *window;
    Viewport viewport;
    int windowHeight;
    int windowWidth;
    int index;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0)
        return 1;
    window =
        SDL_CreateWindow("WC1 input test", 0, 0, 800, 400, SDL_WINDOW_HIDDEN);
    if (window == 0) {
        SDL_Quit();
        return 1;
    }
    if (Wc1SdlInitializeVideo(window) == 0)
        return 1;
    memset(&viewport, 0, sizeof(viewport));
    viewport.right = 319;
    viewport.bottom = 199;
    stMouseCursorState.viewport = &viewport;
    Wc1SdlStartEventPump();

    memset(&event, 0, sizeof(event));
    event.type = SDL_KEYDOWN;
    event.key.windowID = SDL_GetWindowID(window);
    event.key.keysym.scancode = SDL_SCANCODE_A;
    event.key.keysym.sym = SDLK_a;
    if (SDL_PushEvent(&event) != 1 || PumpWindowMessages() == 0)
        return 1;
    if (abInputKeyState[0x1e] != 1)
        return 1;
    if (GetNextInputEvent(&input) != 3 || input.value != 0x1e)
        return 1;

    event.type = SDL_KEYUP;
    if (SDL_PushEvent(&event) != 1 || PumpWindowMessages() == 0)
        return 1;
    if (abInputKeyState[0x1e] != 0)
        return 1;
    if (GetNextInputEvent(&input) != 4)
        return 1;
    if (dwDebugOverlayKey != 'A')
        return 1;

    FlushInputEvents();
    memset(&fullscreenEvent, 0, sizeof(fullscreenEvent));
    fullscreenEvent.type = SDL_KEYDOWN;
    fullscreenEvent.key.windowID = SDL_GetWindowID(window);
    fullscreenEvent.key.keysym.scancode = SDL_SCANCODE_RETURN;
    fullscreenEvent.key.keysym.sym = SDLK_RETURN;
#ifdef __APPLE__
    fullscreenEvent.key.keysym.mod = KMOD_GUI;
#else
    fullscreenEvent.key.keysym.mod = KMOD_ALT;
#endif
    if (SDL_PushEvent(&fullscreenEvent) != 1 ||
        PumpWindowMessages() == 0)
        return 1;
    if ((SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) == 0)
        return 1;
    if (GetNextInputEvent(&input) != 0)
        return 1;

    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    SDL_WarpMouseInWindow(window, windowWidth / 2, windowHeight / 2);
    SDL_PumpEvents();
    SDL_FlushEvent(SDL_MOUSEMOTION);
    memset(&event, 0, sizeof(event));
    event.type = SDL_MOUSEMOTION;
    event.motion.windowID = SDL_GetWindowID(window);
    if (SDL_PushEvent(&event) != 1 || PumpWindowMessages() == 0)
        return 1;
    if (GetNextInputEvent(&input) != 13 || input.x != 160 || input.y != 100)
        return 1;

    fullscreenEvent.type = SDL_KEYUP;
    if (SDL_PushEvent(&fullscreenEvent) != 1 ||
        PumpWindowMessages() == 0)
        return 1;
    if ((SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) == 0)
        return 1;
    if (GetNextInputEvent(&input) != 0)
        return 1;

    fullscreenEvent.type = SDL_KEYDOWN;
    if (SDL_PushEvent(&fullscreenEvent) != 1 ||
        PumpWindowMessages() == 0)
        return 1;
    if ((SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) != 0)
        return 1;
    if (GetNextInputEvent(&input) != 0)
        return 1;

    FlushInputEvents();
    memset(&event, 0, sizeof(event));
    event.type = SDL_KEYDOWN;
    event.key.windowID = SDL_GetWindowID(window);
    event.key.keysym.scancode = SDL_SCANCODE_F;
    event.key.keysym.sym = SDLK_f;
    event.key.keysym.mod = KMOD_NONE;
    pEventManagerPump = get_player_input;
    if (SDL_PushEvent(&event) != 1)
        return 1;
    Wc1SdlPumpEvents();
    pEventManagerPump = 0;
    if ((SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) != 0)
        return 1;
    if (GetNextInputEvent(&input) != 3 || input.value != 0x21)
        return 1;

    event.type = SDL_KEYUP;
    pEventManagerPump = get_player_input;
    if (SDL_PushEvent(&event) != 1)
        return 1;
    Wc1SdlPumpEvents();
    pEventManagerPump = 0;
    if ((SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) != 0)
        return 1;
    if (GetNextInputEvent(&input) != 4 || input.value != 0x21)
        return 1;

    FlushInputEvents();
    SDL_FlushEvent(SDL_MOUSEMOTION);
    SDL_WarpMouseInWindow(window, 400, 200);
    SDL_PumpEvents();
    SDL_FlushEvent(SDL_MOUSEMOTION);
    memset(&event, 0, sizeof(event));
    event.type = SDL_MOUSEMOTION;
    event.motion.windowID = SDL_GetWindowID(window);
    if (SDL_PushEvent(&event) != 1 || PumpWindowMessages() == 0)
        return 1;
    if (GetNextInputEvent(&input) != 13 || input.x != 160 || input.y != 100)
        return 1;

    SDL_WarpMouseInWindow(window, 80, 0);
    SDL_PumpEvents();
    SDL_FlushEvent(SDL_MOUSEMOTION);
    if (SDL_PushEvent(&event) != 1 || PumpWindowMessages() == 0)
        return 1;
    if (GetNextInputEvent(&input) != 13 || input.x != 0 || input.y != 0)
        return 1;

    FlushInputEvents();
    SDL_FlushEvent(SDL_MOUSEMOTION);
    SDL_WarpMouseInWindow(window, 400, 200);
    SDL_PumpEvents();
    SDL_FlushEvent(SDL_MOUSEMOTION);
    index = 0;
    while (index < 300) {
        if (SDL_PushEvent(&event) != 1)
            return 1;
        index++;
    }
    if (PumpWindowMessages() == 0)
        return 1;
    if (GetNextInputEvent(&input) != 13 || input.x != 160 || input.y != 100)
        return 1;
    if (GetNextInputEvent(&input) != 0)
        return 1;

    memset(&event, 0, sizeof(event));
    event.type = SDL_KEYDOWN;
    event.key.windowID = SDL_GetWindowID(window);
    event.key.keysym.scancode = SDL_SCANCODE_Q;
    event.key.keysym.sym = SDLK_q;
    event.key.keysym.mod = KMOD_GUI;
    hMainWindow = (HWND)window;
    if (SDL_PushEvent(&event) != 1)
        return 1;
    PumpWindowMessages();
    return 1;
}
