/* SDL2 host declarations used only by the native port. */
#ifndef WC1_SDL_H
#define WC1_SDL_H

#ifndef WC1_SDL
#error "wc1sdl.h is only for the WC1_SDL build"
#endif

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
typedef void (*Wc1SdlAudioMixer)(void *, unsigned int);

struct DiskFileRecord;
typedef struct Wc1SdlOriginFxPlayer Wc1SdlOriginFxPlayer;

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
typedef struct Wc1SdlDirectDraw2 *LPDIRECTDRAW2;
typedef struct Wc1SdlDirectDrawSurface *LPDIRECTDRAWSURFACE;
typedef struct Wc1SdlDirectDrawPalette *LPDIRECTDRAWPALETTE;

#ifdef __cplusplus
extern "C" {
#endif

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

DWORD Wc1SdlGetTicks(void);
int Wc1SdlGetAsyncKeyState(int virtualKey);
int Wc1SdlStartAudio(Wc1SdlAudioMixer mixer,
                     CRITICAL_SECTION *criticalSection,
                     unsigned int *tick);
void Wc1SdlStopAudio(void);
void Wc1SdlOutputDebugString(const char *text);
void Wc1SdlPumpEvents(void);
int Wc1SdlInitializeVideo(SDL_Window *window);
int Wc1SdlPresentIndexedFrame(const unsigned char *pixels,
                              const unsigned char *palette);
int Wc1SdlSetCursorPosition(int x, int y);
BOOL Wc1SdlReadJoystick(unsigned int device, JOYINFO *information);
BOOL Wc1SdlReadJoystickAxisRange(unsigned int device,
                                 unsigned int *xMinimum,
                                 unsigned int *xMaximum,
                                 unsigned int *yMinimum,
                                 unsigned int *yMaximum);
void Wc1SdlSleep(DWORD milliseconds);
void Wc1SdlStartEventPump(void);
void Wc1SdlShutdownJoysticks(void);
void Wc1SdlShutdownVideo(void);
int Wc1SdlTranslateScanCode(SDL_Scancode scanCode);
void Wc1SdlWaitForVerticalBlank(void);
int Wc1SdlUsingDosData(void);
int Wc1SdlDecompressOriginLzw(const unsigned char *source,
                              size_t sourceSize,
                              unsigned char *destination,
                              size_t destinationSize,
                              size_t *writtenSize);
int Wc1SdlExtractOriginPacketSection(const unsigned char *archive,
                                     size_t archiveSize,
                                     unsigned int sectionIndex,
                                     unsigned char **section,
                                     size_t *sectionSize);
Wc1SdlOriginFxPlayer *Wc1SdlCreateOriginFxPlayer(
    const unsigned char *midi, size_t midiSize,
    const unsigned char *timbres, size_t timbreSize);
void Wc1SdlDestroyOriginFxPlayer(Wc1SdlOriginFxPlayer *player);
int Wc1SdlOriginFxPlayerFinished(const Wc1SdlOriginFxPlayer *player);
void Wc1SdlRenderOriginFxPlayer(Wc1SdlOriginFxPlayer *player,
                                short *samples,
                                unsigned int frameCount,
                                unsigned int gain);
int Wc1SdlInitializeDosAdlibMusic(void);
void Wc1SdlServiceDosAdlibMusic(void);
void Wc1SdlShutdownDosAdlibMusic(void);
void Wc1SdlCompleteDosInstallTable(struct DiskFileRecord *records);

#define GetTickCount Wc1SdlGetTicks
#define GetAsyncKeyState Wc1SdlGetAsyncKeyState
#define OutputDebugString Wc1SdlOutputDebugString
#define OutputDebugStringA Wc1SdlOutputDebugString
#define SetCursorPos Wc1SdlSetCursorPosition
#define Sleep Wc1SdlSleep
#define timeGetTime Wc1SdlGetTicks

int Wc1SdlChangeDirectory(const char *path);
int Wc1SdlResolvePath(const char *path, char *resolved,
                      unsigned long resolvedSize);

#define VK_CLEAR 0x0c
#define VK_SHIFT 0x10
#define VK_CONTROL 0x11
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

#ifndef _WIN32
int Wc1SdlOpen(const char *path, int flags, ...);
long Wc1SdlFileLength(int file);
char *Wc1SdlItoa(int value, char *text, int radix);
char *Wc1SdlLtoa(long value, char *text, int radix);
char *Wc1SdlUltoa(unsigned long value, char *text, int radix);
char *Wc1SdlStrupr(char *text);

#endif

#ifdef __cplusplus
}
#endif

#ifndef _WIN32
#define _open Wc1SdlOpen
#define _close close
#define _read read
#define _write write
#define _lseek lseek
#define _filelength Wc1SdlFileLength
#define _unlink unlink
#define _chdir Wc1SdlChangeDirectory
#define _cprintf printf
#define _itoa Wc1SdlItoa
#define _ltoa Wc1SdlLtoa
#define _ultoa Wc1SdlUltoa
#define _strupr Wc1SdlStrupr
#endif

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

#endif /* WC1_SDL_H */
