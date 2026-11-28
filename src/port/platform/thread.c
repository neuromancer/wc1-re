#include "wc1sdl.h"

enum SdlHandleType {
    SDL_PORT_HANDLE_EVENT,
    SDL_PORT_HANDLE_THREAD
};

typedef struct SdlHandle {
    enum SdlHandleType type;
    SDL_mutex *mutex;
    SDL_cond *condition;
    SDL_Thread *thread;
    LPTHREAD_START_ROUTINE startRoutine;
    LPVOID parameter;
    DWORD result;
    BOOL manualReset;
    BOOL signaled;
    BOOL joined;
} SdlHandle;

static int SdlThreadEntry(void *parameter)
{
    SdlHandle *handle;

    handle = parameter;
    handle->result = handle->startRoutine(handle->parameter);
    return handle->result;
}

HANDLE CreateEventA(LPVOID security, BOOL manualReset, BOOL initialState,
                    const char *name)
{
    SdlHandle *handle;

    (void)security;
    (void)name;
    handle = (SdlHandle *)SDL_calloc(1, sizeof(*handle));
    if (handle == 0)
        return 0;
    handle->type = SDL_PORT_HANDLE_EVENT;
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
    SdlHandle *handle;

    (void)attributes;
    (void)stackSize;
    (void)creationFlags;
    handle = (SdlHandle *)SDL_calloc(1, sizeof(*handle));
    if (handle == 0)
        return 0;
    handle->type = SDL_PORT_HANDLE_THREAD;
    handle->startRoutine = startRoutine;
    handle->parameter = parameter;
    handle->thread = SDL_CreateThread(SdlThreadEntry, "wc1", handle);
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
    SdlHandle *event;

    if (eventHandle == 0 || eventHandle == INVALID_HANDLE_VALUE)
        return FALSE;
    event = (SdlHandle *)eventHandle;
    if (event->type != SDL_PORT_HANDLE_EVENT)
        return FALSE;
    SDL_LockMutex(event->mutex);
    event->signaled = TRUE;
    SDL_CondBroadcast(event->condition);
    SDL_UnlockMutex(event->mutex);
    return TRUE;
}

BOOL ResetEvent(HANDLE eventHandle)
{
    SdlHandle *event;

    if (eventHandle == 0 || eventHandle == INVALID_HANDLE_VALUE)
        return FALSE;
    event = (SdlHandle *)eventHandle;
    if (event->type != SDL_PORT_HANDLE_EVENT)
        return FALSE;
    SDL_LockMutex(event->mutex);
    event->signaled = FALSE;
    SDL_UnlockMutex(event->mutex);
    return TRUE;
}

DWORD WaitForSingleObject(HANDLE objectHandle, DWORD milliseconds)
{
    SdlHandle *object;
    int result;

    if (objectHandle == 0 || objectHandle == INVALID_HANDLE_VALUE)
        return WAIT_TIMEOUT;
    object = (SdlHandle *)objectHandle;
    if (object->type == SDL_PORT_HANDLE_THREAD) {
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
    SdlHandle *object;

    if (objectHandle == 0 || objectHandle == INVALID_HANDLE_VALUE)
        return FALSE;
    object = (SdlHandle *)objectHandle;
    if (object->type == SDL_PORT_HANDLE_THREAD && !object->joined) {
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
