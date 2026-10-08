#include "wc1sdl.h"

#include "video_internal.h"

#include <stdio.h>
#include <string.h>

HANDLE CreateFileA(const char *path, DWORD desiredAccess, DWORD shareMode,
                   LPVOID security, DWORD creationDisposition,
                   DWORD flags, HANDLE templateFile)
{
    (void)path;
    (void)desiredAccess;
    (void)shareMode;
    (void)security;
    (void)creationDisposition;
    (void)flags;
    (void)templateFile;
    return INVALID_HANDLE_VALUE;
}

BOOL DeviceIoControl(HANDLE device, DWORD controlCode, LPVOID input,
                     DWORD inputBytes, LPVOID output, DWORD outputBytes,
                     DWORD *returnedBytes, LPVOID overlapped)
{
    (void)device;
    (void)controlCode;
    (void)input;
    (void)inputBytes;
    (void)output;
    (void)outputBytes;
    (void)overlapped;
    if (returnedBytes != 0)
        *returnedBytes = 0;
    return FALSE;
}

DWORD GetCurrentDirectoryA(DWORD size, char *path)
{
    if (path == 0 || size == 0 || getcwd(path, size) == 0)
        return 0;
    return (DWORD)strlen(path);
}

UINT GetDriveTypeA(const char *root)
{
    (void)root;
    return 0;
}

BOOL GetVolumeInformationA(const char *root, char *volume,
                           DWORD volumeSize, DWORD *serial,
                           DWORD *maximumComponentLength, DWORD *flags,
                           char *filesystem, DWORD filesystemSize)
{
    (void)root;
    (void)volume;
    (void)volumeSize;
    (void)serial;
    (void)maximumComponentLength;
    (void)flags;
    (void)filesystem;
    (void)filesystemSize;
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
    return SdlChangeDirectory(path) == 0;
}

BOOL TextOutA(HDC dc, int x, int y, const char *text, int length)
{
    (void)dc;
    (void)x;
    (void)y;
    (void)text;
    (void)length;
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

DWORD SdlGetTicks(void)
{
    return SDL_GetTicks();
}

static const Uint8 *sdlKeyboardSnapshot;

void SdlSetKeyboardSnapshot(const Uint8 *keys)
{
    sdlKeyboardSnapshot = keys;
}

int SdlGetAsyncKeyState(int virtualKey)
{
    const Uint8 *keys;
    SDL_Scancode scanCode;
    SDL_Scancode keypadCode = SDL_SCANCODE_UNKNOWN;

    keys = sdlKeyboardSnapshot;
    if (keys == NULL) {
        SDL_PumpEvents();
        keys = SDL_GetKeyboardState(0);
    }
    switch (virtualKey) {
    case VK_SHIFT:
        return (keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT])
            ? 0x8000 : 0;
    case VK_CONTROL:
        if (!keys[SDL_SCANCODE_LCTRL] && !keys[SDL_SCANCODE_RCTRL])
            return 0;
        /* The SDL2 port balances audio itself and retains only Ctrl+S and
           Ctrl+M as audio controls.  Let Ctrl+direction keys continue to
           behave as flight directions instead of changing stored levels. */
        if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_RIGHT] ||
            keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_DOWN] ||
            keys[SDL_SCANCODE_HOME] || keys[SDL_SCANCODE_PAGEUP] ||
            keys[SDL_SCANCODE_END] || keys[SDL_SCANCODE_PAGEDOWN] ||
            keys[SDL_SCANCODE_KP_1] || keys[SDL_SCANCODE_KP_2] ||
            keys[SDL_SCANCODE_KP_3] || keys[SDL_SCANCODE_KP_4] ||
            keys[SDL_SCANCODE_KP_6] || keys[SDL_SCANCODE_KP_7] ||
            keys[SDL_SCANCODE_KP_8] || keys[SDL_SCANCODE_KP_9])
            return 0;
        return 0x8000;
    case VK_CLEAR:
        scanCode = SDL_SCANCODE_KP_5;
        break;
    case VK_PRIOR:
        scanCode = SDL_SCANCODE_PAGEUP;
        keypadCode = SDL_SCANCODE_KP_9;
        break;
    case VK_NEXT:
        scanCode = SDL_SCANCODE_PAGEDOWN;
        keypadCode = SDL_SCANCODE_KP_3;
        break;
    case VK_END:
        scanCode = SDL_SCANCODE_END;
        keypadCode = SDL_SCANCODE_KP_1;
        break;
    case VK_HOME:
        scanCode = SDL_SCANCODE_HOME;
        keypadCode = SDL_SCANCODE_KP_7;
        break;
    case VK_LEFT:
        scanCode = SDL_SCANCODE_LEFT;
        keypadCode = SDL_SCANCODE_KP_4;
        break;
    case VK_UP:
        scanCode = SDL_SCANCODE_UP;
        keypadCode = SDL_SCANCODE_KP_8;
        break;
    case VK_RIGHT:
        scanCode = SDL_SCANCODE_RIGHT;
        keypadCode = SDL_SCANCODE_KP_6;
        break;
    case VK_DOWN:
        scanCode = SDL_SCANCODE_DOWN;
        keypadCode = SDL_SCANCODE_KP_2;
        break;
    case VK_INSERT:
        scanCode = SDL_SCANCODE_INSERT;
        keypadCode = SDL_SCANCODE_KP_0;
        break;
    case VK_DELETE:
        scanCode = SDL_SCANCODE_DELETE;
        keypadCode = SDL_SCANCODE_KP_PERIOD;
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
    return (keys[scanCode] ||
            (keypadCode != SDL_SCANCODE_UNKNOWN && keys[keypadCode]))
        ? 0x8000 : 0;
}

void SdlOutputDebugString(const char *text)
{
#ifdef SDL_PORT_LEGACY_DEBUG_OUTPUT
    fputs(text, stderr);
    fflush(stderr);
#else
    (void)text;
#endif
}

int SdlSetCursorPosition(int x, int y)
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    int logicalWidth = 0;
    int logicalHeight = 0;
    int windowX;
    int windowY;

    window = SDL_GetKeyboardFocus();
    if (window == 0)
        window = SDL_GetMouseFocus();
    if (window == 0)
        return FALSE;
    renderer = SDL_GetRenderer(window);
    if (renderer != NULL)
        SDL_RenderGetLogicalSize(renderer, &logicalWidth, &logicalHeight);
    if (logicalWidth > 0 && logicalHeight > 0) {
        /* SWC uses SDL's 320x240 logical renderer, including letterboxing.
           WC1's renderer keeps its existing 320x200 coordinate conversion. */
        SDL_RenderLogicalToWindow(renderer, (float)x, (float)y, &windowX, &windowY);
    } else if (!SdlMapLogicalToWindow(
            window, x, y, &windowX, &windowY))
        return FALSE;
    SDL_WarpMouseInWindow(window, windowX, windowY);
    return TRUE;
}

void SdlSleep(DWORD milliseconds)
{
    SDL_Delay(milliseconds);
}
