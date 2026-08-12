#include "wc1sdl.h"

int main(void)
{
    DWORD elapsed;
    DWORD start;

    if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0)
        return 1;
    start = GetTickCount();
    Sleep(2);
    elapsed = GetTickCount() - start;
    if (elapsed == 0) {
        SDL_Quit();
        return 1;
    }
    if (GetAsyncKeyState(-1) != 0) {
        SDL_Quit();
        return 1;
    }
    if (SetCursorPos(160, 100) != FALSE) {
        SDL_Quit();
        return 1;
    }
    SDL_Quit();
    return 0;
}
