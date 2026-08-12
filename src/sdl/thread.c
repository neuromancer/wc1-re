#include "wc1sdl.h"

enum Wc1SdlHandleType {
    WC1_SDL_HANDLE_EVENT,
    WC1_SDL_HANDLE_THREAD
};

typedef struct Wc1SdlHandle {
    enum Wc1SdlHandleType type;
    SDL_mutex *mutex;
    SDL_cond *condition;
    SDL_Thread *thread;
    LPTHREAD_START_ROUTINE startRoutine;
    LPVOID parameter;
    DWORD result;
    BOOL manualReset;
    BOOL signaled;
    BOOL joined;
} Wc1SdlHandle;

static int Wc1SdlThreadEntry(void *parameter)
{
    Wc1SdlHandle *handle;

    handle = (Wc1SdlHandle *)parameter;
    handle->result = handle->startRoutine(handle->parameter);
    return (int)handle->result;
}

HANDLE CreateEventA(LPVOID security, BOOL manualReset, BOOL initialState,
                    const char *name)
{
    Wc1SdlHandle *handle;

    (void)security;
    (void)name;
    handle = (Wc1SdlHandle *)SDL_calloc(1, sizeof(*handle));
    if (handle == 0)
        return 0;
    handle->type = WC1_SDL_HANDLE_EVENT;
    handle->manualReset = manualReset;
    handle->signaled = initialState;
    handle->mutex = SDL_CreateMutex();
    handle->condition = SDL_CreateCond();
    if (handle->mutex == 0 || handle->condition == 0) {
        CloseHandle(handle);
        return 0;
    }
    return handle;
}

HANDLE CreateThread(LPVOID attributes, size_t stackSize,
                    LPTHREAD_START_ROUTINE startRoutine, LPVOID parameter,
                    DWORD creationFlags, DWORD *threadId)
{
    Wc1SdlHandle *handle;

    (void)attributes;
    (void)stackSize;
    (void)creationFlags;
    handle = (Wc1SdlHandle *)SDL_calloc(1, sizeof(*handle));
    if (handle == 0)
        return 0;
    handle->type = WC1_SDL_HANDLE_THREAD;
    handle->startRoutine = startRoutine;
    handle->parameter = parameter;
    handle->thread = SDL_CreateThread(Wc1SdlThreadEntry, "wc1", handle);
    if (handle->thread == 0) {
        SDL_free(handle);
        return 0;
    }
    if (threadId != 0)
        *threadId = (DWORD)SDL_GetThreadID(handle->thread);
    return handle;
}

BOOL SetEvent(HANDLE eventHandle)
{
    Wc1SdlHandle *event;

    if (eventHandle == 0 || eventHandle == INVALID_HANDLE_VALUE)
        return FALSE;
    event = (Wc1SdlHandle *)eventHandle;
    if (event->type != WC1_SDL_HANDLE_EVENT)
        return FALSE;
    SDL_LockMutex(event->mutex);
    event->signaled = TRUE;
    SDL_CondBroadcast(event->condition);
    SDL_UnlockMutex(event->mutex);
    return TRUE;
}

BOOL ResetEvent(HANDLE eventHandle)
{
    Wc1SdlHandle *event;

    if (eventHandle == 0 || eventHandle == INVALID_HANDLE_VALUE)
        return FALSE;
    event = (Wc1SdlHandle *)eventHandle;
    if (event->type != WC1_SDL_HANDLE_EVENT)
        return FALSE;
    SDL_LockMutex(event->mutex);
    event->signaled = FALSE;
    SDL_UnlockMutex(event->mutex);
    return TRUE;
}

DWORD WaitForSingleObject(HANDLE objectHandle, DWORD milliseconds)
{
    Wc1SdlHandle *object;
    int result;

    if (objectHandle == 0 || objectHandle == INVALID_HANDLE_VALUE)
        return WAIT_TIMEOUT;
    object = (Wc1SdlHandle *)objectHandle;
    if (object->type == WC1_SDL_HANDLE_THREAD) {
        if (!object->joined) {
            SDL_WaitThread(object->thread, 0);
            object->joined = TRUE;
            object->thread = 0;
        }
        return WAIT_OBJECT_0;
    }
    SDL_LockMutex(object->mutex);
    result = 0;
    while (!object->signaled && result == 0) {
        if (milliseconds == INFINITE)
            result = SDL_CondWait(object->condition, object->mutex);
        else
            result = SDL_CondWaitTimeout(object->condition, object->mutex,
                                         milliseconds);
    }
    if (result == 0 && !object->manualReset)
        object->signaled = FALSE;
    SDL_UnlockMutex(object->mutex);
    return result == 0 ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
}

BOOL CloseHandle(HANDLE objectHandle)
{
    Wc1SdlHandle *object;

    if (objectHandle == 0 || objectHandle == INVALID_HANDLE_VALUE)
        return FALSE;
    object = (Wc1SdlHandle *)objectHandle;
    if (object->type == WC1_SDL_HANDLE_THREAD && !object->joined) {
        SDL_WaitThread(object->thread, 0);
        object->joined = TRUE;
        object->thread = 0;
    }
    if (object->condition != 0)
        SDL_DestroyCond(object->condition);
    if (object->mutex != 0)
        SDL_DestroyMutex(object->mutex);
    SDL_free(object);
    return TRUE;
}

HANDLE GetCurrentThread(void)
{
    return 0;
}

BOOL SetThreadPriority(HANDLE thread, int priority)
{
    (void)thread;
    (void)priority;
    return SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH) == 0;
}
