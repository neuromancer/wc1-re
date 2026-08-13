#include "wc1.h"

#define WC1_SDL_TIME_PERIODIC 1U

typedef struct Wc1SdlTimer {
    SDL_TimerID sdlTimer;
    LPTIMECALLBACK callback;
    DWORD user;
    UINT period;
    UINT eventType;
    UINT id;
} Wc1SdlTimer;

static Wc1SdlTimer g_aSdlTimers[16];
static void *g_pSdlTimerFallback;
static SDL_SpinLock g_nSdlTimerLock;

static Uint32 Wc1SdlTimerCallback(Uint32 interval, void *parameter)
{
    LPTIMECALLBACK callback;
    Wc1SdlTimer *timer;
    DWORD user;
    UINT eventType;
    UINT id;
    UINT period;

    (void)interval;
    timer = (Wc1SdlTimer *)parameter;
    if (timer == 0)
        timer = (Wc1SdlTimer *)SDL_AtomicGetPtr(
            &g_pSdlTimerFallback);
    if (timer == 0)
        return 0;
    SDL_AtomicLock(&g_nSdlTimerLock);
    callback = timer->callback;
    id = timer->id;
    user = timer->user;
    period = timer->period;
    eventType = timer->eventType;
    SDL_AtomicUnlock(&g_nSdlTimerLock);
    if (callback == 0)
        return 0;
    callback(id, 0, user, 0, 0);
    return (eventType & WC1_SDL_TIME_PERIODIC) != 0 ? period : 0;
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
    g_aSdlTimers[index].eventType = eventType;
    g_aSdlTimers[index].id = index + 1;
    SDL_AtomicSetPtr(&g_pSdlTimerFallback, &g_aSdlTimers[index]);
    g_aSdlTimers[index].sdlTimer =
        SDL_AddTimer(delay, Wc1SdlTimerCallback, &g_aSdlTimers[index]);
    if (g_aSdlTimers[index].sdlTimer == 0) {
        SDL_AtomicLock(&g_nSdlTimerLock);
        if (SDL_AtomicGetPtr(&g_pSdlTimerFallback) ==
            &g_aSdlTimers[index])
            SDL_AtomicSetPtr(&g_pSdlTimerFallback, 0);
        memset(&g_aSdlTimers[index], 0, sizeof(g_aSdlTimers[index]));
        SDL_AtomicUnlock(&g_nSdlTimerLock);
        return 0;
    }
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
    SDL_AtomicLock(&g_nSdlTimerLock);
    if (SDL_AtomicGetPtr(&g_pSdlTimerFallback) == timer)
        SDL_AtomicSetPtr(&g_pSdlTimerFallback, 0);
    memset(timer, 0, sizeof(*timer));
    SDL_AtomicUnlock(&g_nSdlTimerLock);
    return 0;
}
