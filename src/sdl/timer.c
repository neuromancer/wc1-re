#include "wc1.h"

typedef struct Wc1SdlTimer {
    SDL_TimerID sdlTimer;
    LPTIMECALLBACK callback;
    DWORD user;
    UINT period;
    UINT id;
} Wc1SdlTimer;

static Wc1SdlTimer g_aSdlTimers[16];

static Uint32 Wc1SdlTimerCallback(Uint32 interval, void *parameter)
{
    Wc1SdlTimer *timer;

    timer = (Wc1SdlTimer *)parameter;
    timer->callback(timer->id, 0, timer->user, 0, 0);
    return timer->period;
}

UINT timeSetEvent(UINT delay, UINT resolution, LPTIMECALLBACK callback,
                  DWORD user, UINT eventType)
{
    UINT index;

    index = 0;
    while (index < 16 && g_aSdlTimers[index].sdlTimer != 0)
        index++;
    if (index == 16 || callback == 0)
        return 0;
    g_aSdlTimers[index].callback = callback;
    g_aSdlTimers[index].user = user;
    g_aSdlTimers[index].period = delay;
    g_aSdlTimers[index].id = index + 1;
    g_aSdlTimers[index].sdlTimer =
        SDL_AddTimer(delay, Wc1SdlTimerCallback, &g_aSdlTimers[index]);
    if (g_aSdlTimers[index].sdlTimer == 0)
        return 0;
    return index + 1;
}

UINT timeKillEvent(UINT timerId)
{
    Wc1SdlTimer *timer;

    if (timerId == 0 || timerId > 16)
        return 1;
    timer = &g_aSdlTimers[timerId - 1];
    if (timer->sdlTimer == 0)
        return 1;
    SDL_RemoveTimer(timer->sdlTimer);
    memset(timer, 0, sizeof(*timer));
    return 0;
}
