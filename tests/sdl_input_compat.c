#include "wc1sdl.h"

static SDL_atomic_t g_nTimerCallbackCount;
static SDL_atomic_t g_dwTimerCallbackUser;

static void TimerCallback(UINT timerId, UINT message, DWORD user,
                          DWORD first, DWORD second)
{
    (void)timerId;
    (void)message;
    (void)first;
    (void)second;
    SDL_AtomicSet(&g_dwTimerCallbackUser, (int)user);
    SDL_AtomicAdd(&g_nTimerCallbackCount, 1);
}

int main(int argumentCount, char **arguments)
{
    DWORD elapsed;
    DWORD start;
    UINT timerId;

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
    timerId = timeSetEvent(5, 5, TimerCallback, 0x12345678U, 0);
    if (timerId == 0) {
        SDL_Quit();
        return 1;
    }
    start = GetTickCount();
    while (SDL_AtomicGet(&g_nTimerCallbackCount) == 0 &&
           GetTickCount() - start < 250)
        SDL_Delay(1);
    if (SDL_AtomicGet(&g_nTimerCallbackCount) != 1 ||
        (DWORD)SDL_AtomicGet(&g_dwTimerCallbackUser) != 0x12345678U) {
        timeKillEvent(timerId);
        SDL_Quit();
        return 1;
    }
    SDL_Delay(20);
    if (SDL_AtomicGet(&g_nTimerCallbackCount) != 1) {
        timeKillEvent(timerId);
        SDL_Quit();
        return 1;
    }
    timeKillEvent(timerId);
    SDL_Quit();
    return 0;
}
