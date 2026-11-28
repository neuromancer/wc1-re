/* The Win32 types, constants and functions the original game uses, as
 * the port provides them. */
#ifndef SDL_PORT_WIN32_H
#define SDL_PORT_WIN32_H

#include <SDL.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#ifdef _WIN32
#include <conio.h>
#include <direct.h>
#include <io.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Keep the widths of Win32 scalar types even on LP64 hosts. */
typedef uint8_t BYTE;
typedef uint8_t BOOLEAN;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t LONG;
typedef uint32_t UINT;
typedef uint32_t COLORREF;
typedef int BOOL;
typedef int HRESULT;

typedef uintptr_t WPARAM;
typedef intptr_t LPARAM;
typedef intptr_t LRESULT;

typedef void *HANDLE;
typedef void *HINSTANCE;
typedef void *HWND;
typedef void *HDC;
typedef void *HCURSOR;
typedef void *HHOOK;
typedef void *HKEY;
typedef void *LPVOID;
typedef void *LPDIRECTSOUND;
typedef void *LPDIRECTSOUNDBUFFER;

typedef char *LPSTR;
typedef const char *LPCSTR;
typedef BYTE *LPBYTE;
typedef void (*LPTIMECALLBACK)(UINT, UINT, DWORD, DWORD, DWORD);
typedef DWORD (*LPTHREAD_START_ROUTINE)(LPVOID);
typedef struct GUID {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t Data4[8];
} GUID;
typedef GUID *LPGUID;

typedef union LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    };
    int64_t QuadPart;
} LARGE_INTEGER;

typedef struct JOYINFO {
    DWORD wXpos;
    DWORD wYpos;
    DWORD wZpos;
    DWORD wButtons;
} JOYINFO;

typedef struct CRITICAL_SECTION {
    SDL_mutex *mutex;
} CRITICAL_SECTION;

/* DirectDraw objects become SDL-owned port objects in the native build. */
typedef struct SdlDirectDraw2 *LPDIRECTDRAW2;
typedef struct SdlDirectDrawSurface *LPDIRECTDRAWSURFACE;
typedef struct SdlDirectDrawPalette *LPDIRECTDRAWPALETTE;
/* The port's Win32 stand-ins take Sdl names, so they never collide with or
 * bind to the real Win32 API on Windows; the game's spellings map to them. */
#define CloseHandle SdlCloseHandle
#define CreateEventA SdlCreateEventA
#define CreateFileA SdlCreateFileA
#define CreateThread SdlCreateThread
#define DeviceIoControl SdlDeviceIoControl
#define GetCurrentDirectoryA SdlGetCurrentDirectoryA
#define GetCurrentThread SdlGetCurrentThread
#define GetDriveTypeA SdlGetDriveTypeA
#define GetVolumeInformationA SdlGetVolumeInformationA
#define MessageBoxA SdlMessageBoxA
#define QueryPerformanceCounter SdlQueryPerformanceCounter
#define ResetEvent SdlResetEvent
#define SetCurrentDirectoryA SdlSetCurrentDirectoryA
#define SetEvent SdlSetEvent
#define SetThreadPriority SdlSetThreadPriority
#define TextOutA SdlTextOutA
#define WaitForSingleObject SdlWaitForSingleObject
#define timeKillEvent SdlTimeKillEvent
#define timeSetEvent SdlTimeSetEvent
#define DeleteCriticalSection SdlDeleteCriticalSection
#define EnterCriticalSection SdlEnterCriticalSection
#define InitializeCriticalSection SdlInitializeCriticalSection
#define LeaveCriticalSection SdlLeaveCriticalSection
#define RegCloseKey SdlRegCloseKey
#define RegOpenKeyExA SdlRegOpenKeyExA
#define RegQueryValueExA SdlRegQueryValueExA
#define RegSetValueExA SdlRegSetValueExA
BOOL CloseHandle(HANDLE handle);
HANDLE CreateEventA(LPVOID security, BOOL manualReset, BOOL initialState,
                    const char *name);
HANDLE CreateFileA(const char *path, DWORD desiredAccess, DWORD shareMode,
                   LPVOID security, DWORD creationDisposition,
                   DWORD flags, HANDLE templateFile);
HANDLE CreateThread(LPVOID attributes, size_t stackSize,
                    LPTHREAD_START_ROUTINE startRoutine, LPVOID parameter,
                    DWORD creationFlags, DWORD *threadId);
BOOL DeviceIoControl(HANDLE device, DWORD controlCode, LPVOID input,
                     DWORD inputBytes, LPVOID output, DWORD outputBytes,
                     DWORD *returnedBytes, LPVOID overlapped);
DWORD GetCurrentDirectoryA(DWORD size, char *path);
UINT GetDriveTypeA(const char *root);
BOOL GetVolumeInformationA(const char *root, char *volume,
                           DWORD volumeSize, DWORD *serial,
                           DWORD *maximumComponentLength, DWORD *flags,
                           char *filesystem, DWORD filesystemSize);
int MessageBoxA(HWND window, const char *text, const char *title,
                UINT type);
BOOL QueryPerformanceCounter(LARGE_INTEGER *counter);
BOOL ResetEvent(HANDLE event);
BOOL SetCurrentDirectoryA(const char *path);
BOOL SetEvent(HANDLE event);
BOOL SetThreadPriority(HANDLE thread, int priority);
BOOL TextOutA(HDC dc, int x, int y, const char *text, int length);
DWORD WaitForSingleObject(HANDLE handle, DWORD milliseconds);
HANDLE GetCurrentThread(void);
UINT timeKillEvent(UINT timerId);
UINT timeSetEvent(UINT delay, UINT resolution, LPTIMECALLBACK callback,
                  DWORD user, UINT eventType);
void DeleteCriticalSection(CRITICAL_SECTION *criticalSection);
void EnterCriticalSection(CRITICAL_SECTION *criticalSection);
void InitializeCriticalSection(CRITICAL_SECTION *criticalSection);
void LeaveCriticalSection(CRITICAL_SECTION *criticalSection);

DWORD RegCloseKey(HKEY key);
DWORD RegOpenKeyExA(HKEY root, const char *subkey, DWORD options,
                    DWORD access, HKEY *result);
DWORD RegQueryValueExA(HKEY key, const char *name, DWORD *reserved,
                       DWORD *type, BYTE *data, DWORD *size);
DWORD RegSetValueExA(HKEY key, const char *name, DWORD reserved,
                     DWORD type, const BYTE *data, DWORD size);
DWORD SdlGetTicks(void);
int SdlGetAsyncKeyState(int virtualKey);
/* Windows developer-channel output stays off in the SDL2 port unless a
 * diagnostic build defines SDL_PORT_LEGACY_DEBUG_OUTPUT. */
void SdlOutputDebugString(const char *text);
int SdlSetCursorPosition(int x, int y);
void SdlSleep(DWORD milliseconds);
#define GetTickCount SdlGetTicks
#define GetAsyncKeyState SdlGetAsyncKeyState
#define OutputDebugString SdlOutputDebugString
#define OutputDebugStringA SdlOutputDebugString
#define SetCursorPos SdlSetCursorPosition
#define Sleep SdlSleep
#define timeGetTime SdlGetTicks
#define VK_BACK 0x08
#define VK_TAB 0x09
#define VK_CLEAR 0x0c
#define VK_RETURN 0x0d
#define VK_SHIFT 0x10
#define VK_CONTROL 0x11
#define VK_MENU 0x12
#define VK_ESCAPE 0x1b
#define VK_F1 0x70
#define VK_F12 0x7b
#define VK_PRIOR 0x21
#define VK_NEXT 0x22
#define VK_END 0x23
#define VK_HOME 0x24
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28
#define VK_INSERT 0x2d
#define VK_DELETE 0x2e
#define VK_OEM_COMMA 0xbc
#define VK_OEM_PERIOD 0xbe

#define KF_ALTDOWN 0x2000
#define KF_REPEAT 0x4000
#define JOY_BUTTON1 0x0001
#define JOY_BUTTON2 0x0002
#define KEY_STATE_DOWN 0x8000
#define CREATE_ALWAYS 2
#define DRIVE_CDROM 5
#define ERROR_FILE_NOT_FOUND 2
#define ERROR_SUCCESS 0
#define FILE_FLAG_DELETE_ON_CLOSE 0x04000000
#define HKEY_LOCAL_MACHINE ((HKEY)(uintptr_t)1)
#define IDCANCEL 2
#define IDOK 1
#define INVALID_HANDLE_VALUE ((HANDLE)(intptr_t)-1)
#define INFINITE 0xffffffffU
#define KEY_ALL_ACCESS 0x000f003f
#define MB_ICONERROR 0x00000010
#define MB_ICONEXCLAMATION 0x00000030
#define MB_ICONHAND MB_ICONERROR
#define MB_OKCANCEL 0x00000001
#define OPAQUE 2
#define REG_DWORD 4
#define THREAD_PRIORITY_TIME_CRITICAL 15
#define TRANSPARENT 1
#define WAIT_OBJECT_0 0
#define WAIT_TIMEOUT 258

#ifndef __cdecl
#define __cdecl
#endif

#ifndef __stdcall
#define __stdcall
#endif

#ifndef __fastcall
#define __fastcall
#endif

#ifndef WINAPI
#define WINAPI
#endif

#ifndef CALLBACK
#define CALLBACK
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* The C core uses Win32 min/max; leave C++ standard-library names intact. */
#ifndef __cplusplus
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_WIN32_H */
