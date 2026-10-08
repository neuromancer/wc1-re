/* SDL2 host declarations used only by the native port. */
#ifndef SDL_PORT_H
#define SDL_PORT_H

#ifndef SDL_PORT
#error "wc1sdl.h is only for the SDL_PORT build"
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
typedef void (*SdlAudioMixer)(void *, unsigned int);

struct DiskFileRecord;
struct ScreenViewportGeometry;
struct Viewport;
typedef struct SdlOriginFxPlayer SdlOriginFxPlayer;

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

/* The port stands in for a slice of the Win32 API with SDL.  A native Windows
 * host already has kernel32, user32, gdi32, advapi32 and winmm exporting those
 * same names, and its idea of a HANDLE, an HWND or a CRITICAL_SECTION is not
 * the port's, so defining our own would either collide at link time or bind
 * the game to a real entry point that does not understand these structures.
 * Give the port's versions their own names on every host and let the game's
 * Win32 spellings reach them through these macros; the shims below are
 * declared after the renaming, so they carry the prefixed names too. */
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

DWORD SdlGetTicks(void);
/* Borrow one SDL keyboard sample for a simulation tick; NULL restores live polling. */
void SdlSetKeyboardSnapshot(const Uint8 *keys);
int SdlGetAsyncKeyState(int virtualKey);
int SdlStartAudio(SdlAudioMixer mixer,
                     CRITICAL_SECTION *criticalSection,
                     unsigned int *tick);
void SdlStopAudio(void);
/* Windows developer-channel output stays off in the SDL2 port unless a
 * diagnostic build defines SDL_PORT_LEGACY_DEBUG_OUTPUT. */
void SdlOutputDebugString(const char *text);
void SdlPumpEvents(void);
void SdlSetMouseGrab(int enabled);
void SdlDrawViewportStatic(struct Viewport *viewport, int effect,
                              unsigned short colour);
void SdlSuspendMouseGrab(void);
void SdlResumeMouseGrab(void);
int SdlInitializeVideo(SDL_Window *window);
int SdlPresentIndexedFrame(const unsigned char *pixels,
                              const unsigned char *palette);
void SdlBeginSpaceFrame(
    const struct ScreenViewportGeometry *geometry, int viewportMode,
    int fullViewportCopy, unsigned char clearColour);
void SdlCompleteSpaceFrame(void);
void SdlCancelSpaceFrame(void);
void SdlSetThrusterScreenPosition(short object, float x, float y);
void SdlGetThrusterScreenPosition(short object, float *x, float *y);
int SdlRecordSpaceSprite(
    const struct Viewport *viewport, float x, float y,
    unsigned char *shape, short frame, short angle, short scale,
    short flip);
int SdlSetCursorPosition(int x, int y);
BOOL SdlReadJoystick(unsigned int device, JOYINFO *information);
BOOL SdlReadJoystickAxisRange(unsigned int device,
                                 unsigned int *xMinimum,
                                 unsigned int *xMaximum,
                                 unsigned int *yMinimum,
                                 unsigned int *yMaximum);
int SdlSetJoystickMode(const char *name);
int SdlSetJoystickAxesMode(const char *name);
void SdlEnableJoystickDebug(void);
void SdlEnableJoystickRumble(void);
void SdlLogJoystickEvent(const SDL_Event *event);
int SdlGetCommunicationMenuSelection(void);
void SdlQueueJoystickWeaponRumble(int weaponType);
void SdlQueueJoystickDamageRumble(int damage);
void SdlQueueJoystickCollisionRumble(int collisionSpeed);
void SdlApplyJoystickFlightControls(void);
void SdlEndJoystickSpaceflight(void);
void SdlHandleJoystickButtonEvent(SDL_JoystickID instanceId,
                                      int button, int pressed,
                                      int controllerEvent);
void SdlHandleJoystickHatEvent(SDL_JoystickID instanceId,
                                  Uint8 hat, Uint8 value);
void SdlHandleJoystickDeviceEvent(Uint32 type, Sint32 which);
void SdlSleep(DWORD milliseconds);
void SdlStartEventPump(void);
void SdlShutdownJoysticks(void);
void SdlShutdownVideo(void);
int SdlTranslateScanCode(SDL_Scancode scanCode);
void SdlWaitForVerticalBlank(void);
int SdlUsingDosData(void);
int SdlFindSwcMissionData(char *path, unsigned long capacity);
int SdlLoadSwcMissionData(const char *path, short series, short mission);
int SdlRunSwcMission(const char *missionPath, int checkOnly, int cockpitless);
int SdlSwcFlightActive(void);
void SdlPlayDosStartupIntro(void);
int SdlDecompressOriginLzw(const unsigned char *source,
                              size_t sourceSize,
                              unsigned char *destination,
                              size_t destinationSize,
                              size_t *writtenSize);
int SdlExtractOriginPacketSection(const unsigned char *archive,
                                     size_t archiveSize,
                                     unsigned int sectionIndex,
                                     unsigned char **section,
                                     size_t *sectionSize);
SdlOriginFxPlayer *SdlCreateOriginFxPlayer(
    const unsigned char *midi, size_t midiSize,
    const unsigned char *timbres, size_t timbreSize);
SdlOriginFxPlayer *SdlCreateOriginFxSoundPlayer(
    const unsigned char *timbres, size_t timbreSize);
void SdlDestroyOriginFxPlayer(SdlOriginFxPlayer *player);
int SdlOriginFxPlayerFinished(const SdlOriginFxPlayer *player);
unsigned int SdlOriginFxPlayerSequencePosition(
    const SdlOriginFxPlayer *player);
int SdlPlayOriginFxSoundEffect(
    SdlOriginFxPlayer *player, unsigned int soundNumber,
    int volume, int pan, int tag, int priority);
void SdlStopOriginFxSoundEffects(SdlOriginFxPlayer *player);
void SdlMixOriginFxSoundEffects(
    SdlOriginFxPlayer *player, short *samples,
    unsigned int frameCount, unsigned int gain);
void SdlRenderOriginFxPlayer(SdlOriginFxPlayer *player,
                                short *samples,
                                unsigned int frameCount,
                                unsigned int gain);
void SdlMixOriginFxPlayer(SdlOriginFxPlayer *player,
                             short *samples,
                             unsigned int frameCount,
                             unsigned int gain);
int SdlInitializeOriginFxAudio(int useStandaloneAudio);
int SdlGetOriginFxMusicSequencePosition(void);
void SdlMixOriginFxMusic(short *samples, unsigned int frameCount);
int SdlPlayDosSoundEffect(int soundNumber, int volume, int pan,
                             int tag, int priority);
int SdlHandlesGameSoundEffects(void);
int SdlPlayGameSoundEffect(int soundNumber, int sourceObject,
                           int looping);
void SdlPlayWaveWithPan(const char *filename, int looping,
                        int volume, int pan);
void SdlServiceOriginFxMusic(void);
void SdlStopDosSoundEffects(void);
void SdlShutdownOriginFxAudio(void);
void SdlCompleteDosInstallTable(struct DiskFileRecord *records);

#define GetTickCount SdlGetTicks
#define GetAsyncKeyState SdlGetAsyncKeyState
#define OutputDebugString SdlOutputDebugString
#define OutputDebugStringA SdlOutputDebugString
#define SetCursorPos SdlSetCursorPosition
#define Sleep SdlSleep
#define timeGetTime SdlGetTicks

int SdlChangeDirectory(const char *path);
int SdlResolvePath(const char *path, char *resolved,
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
int SdlOpen(const char *path, int flags, ...);
long SdlFileLength(int file);
char *SdlItoa(int value, char *text, int radix);
char *SdlLtoa(long value, char *text, int radix);
char *SdlUltoa(unsigned long value, char *text, int radix);
char *SdlStrupr(char *text);

#endif

#ifdef __cplusplus
}
#endif

#ifndef _WIN32
#define _open SdlOpen
#define _close close
#define _read read
#define _write write
#define _lseek lseek
#define _filelength SdlFileLength
#define _unlink unlink
#define _chdir SdlChangeDirectory
#define _cprintf printf
#define _itoa SdlItoa
#define _ltoa SdlLtoa
#define _ultoa SdlUltoa
#define _strupr SdlStrupr
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

#endif /* SDL_PORT_H */
