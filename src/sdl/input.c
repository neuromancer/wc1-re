#include "wc1sdl.h"

#include "video_internal.h"

#include <stdio.h>
#include <string.h>

HANDLE CreateFileA(const char *path, DWORD desiredAccess, DWORD shareMode,
                   LPVOID security, DWORD creationDisposition,
                   DWORD flags, HANDLE templateFile)
{
    return INVALID_HANDLE_VALUE;
}

BOOL DeviceIoControl(HANDLE device, DWORD controlCode, LPVOID input,
                     DWORD inputBytes, LPVOID output, DWORD outputBytes,
                     DWORD *returnedBytes, LPVOID overlapped)
{
    if (returnedBytes != 0)
        *returnedBytes = 0;
    return FALSE;
}

DWORD GetCurrentDirectoryA(DWORD size, char *path)
{
    if (getcwd(path, size) == 0)
        return 0;
    return (DWORD)strlen(path);
}

UINT GetDriveTypeA(const char *root)
{
    return 0;
}

BOOL GetVolumeInformationA(const char *root, char *volume,
                           DWORD volumeSize, DWORD *serial,
                           DWORD *maximumComponentLength, DWORD *flags,
                           char *filesystem, DWORD filesystemSize)
{
    return FALSE;
}

int MessageBoxA(HWND window, const char *text, const char *title, UINT type)
{
    Uint32 messageFlags;

    messageFlags = (type & MB_ICONERROR) != 0
                       ? SDL_MESSAGEBOX_ERROR
                       : SDL_MESSAGEBOX_WARNING;
    SDL_ShowSimpleMessageBox(messageFlags, title, text,
                             (SDL_Window *)window);
    return (type & MB_OKCANCEL) != 0 ? IDCANCEL : IDOK;
}

BOOL QueryPerformanceCounter(LARGE_INTEGER *counter)
{
    counter->QuadPart = (int64_t)SDL_GetPerformanceCounter();
    return TRUE;
}

BOOL SetCurrentDirectoryA(const char *path)
{
    return Wc1SdlChangeDirectory(path) == 0;
}

BOOL TextOutA(HDC dc, int x, int y, const char *text, int length)
{
    return TRUE;
}

void InitializeCriticalSection(CRITICAL_SECTION *criticalSection)
{
    criticalSection->mutex = SDL_CreateMutex();
}

void DeleteCriticalSection(CRITICAL_SECTION *criticalSection)
{
    if (criticalSection->mutex != 0) {
        SDL_DestroyMutex(criticalSection->mutex);
        criticalSection->mutex = 0;
    }
}

void EnterCriticalSection(CRITICAL_SECTION *criticalSection)
{
    SDL_LockMutex(criticalSection->mutex);
}

void LeaveCriticalSection(CRITICAL_SECTION *criticalSection)
{
    SDL_UnlockMutex(criticalSection->mutex);
}

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
    int windowX;
    int windowY;

    window = SDL_GetKeyboardFocus();
    if (window == 0)
        window = SDL_GetMouseFocus();
    if (window == 0)
        return FALSE;
    if (!Wc1SdlMapLogicalToWindow(
            window, x, y, &windowX, &windowY))
        return FALSE;
    SDL_WarpMouseInWindow(window, windowX, windowY);
    return TRUE;
}

void Wc1SdlSleep(DWORD milliseconds)
{
    SDL_Delay(milliseconds);
}
