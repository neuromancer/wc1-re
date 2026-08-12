#include "wc1.h"

#include <stdio.h>
#include <string.h>

int main(int argumentCount, char **arguments)
{
    SDL_Window *window;
    Uint32 windowFlags;
    int checkOnly;

    checkOnly = argumentCount == 2 && strcmp(arguments[1], "--check") == 0;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER |
                 SDL_INIT_JOYSTICK) != 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    windowFlags = SDL_WINDOW_RESIZABLE;
    if (checkOnly)
        windowFlags |= SDL_WINDOW_HIDDEN;
    window =
        SDL_CreateWindow("Wing Commander SDL2 port", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, 960, 600, windowFlags);
    if (window == 0) {
        fprintf(stderr, "SDL window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    DIBinstall((HWND)window);
    DAT_005a89a0 = (HWND)window;
    Wc1SdlStartEventPump();
    if (checkOnly) {
        g_acShipTarget_0059ce60[0] = 0;
        g_aeShipManeuver_0059dcb0[0] = MANEUVER_NONE;
        perform_maneuver(0);
    } else {
        MonoDebug_install();
        InitializeAudioSystem((HWND)window);
        InitializeAudioStreamer((HWND)window);
        srand((unsigned int)time(0));
        InitGameClockEpoch();
        CreateDebugOverlayConsole(0, (HWND)window, 60, 20);
        DAT_005a8a44 = (unsigned int)time(0);
        DAT_0059ab2c = 0;
        SDL_SetWindowMouseGrab(window, SDL_TRUE);
        SDL_ShowCursor(SDL_DISABLE);
        Wc1GameMain((short)(argumentCount - 1), arguments);
        SDL_SetWindowMouseGrab(window, SDL_FALSE);
        SDL_ShowCursor(SDL_ENABLE);
        DestroyGlobalDebugOverlayConsole();
        if ((g_dwStreamerState_00597cd0 & 1) != 0)
            ix_streamer_destroy();
        ServiceAudioStream();
    }

    DIBunInstall();
    Wc1SdlShutdownJoysticks();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
