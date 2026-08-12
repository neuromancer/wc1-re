/* SDL2 host declarations used only by the native port. */
#ifndef WC1_SDL_H
#define WC1_SDL_H

#ifndef WC1_SDL
#error "wc1sdl.h is only for the WC1_SDL build"
#endif

#include <SDL.h>
#include <stdint.h>

#ifdef _WIN32
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

typedef char *LPSTR;
typedef const char *LPCSTR;
typedef BYTE *LPBYTE;

typedef struct GUID {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t Data4[8];
} GUID;

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

DWORD Wc1SdlGetTicks(void);
int Wc1SdlGetAsyncKeyState(int virtualKey);
void Wc1SdlOutputDebugString(const char *text);
int Wc1SdlSetCursorPosition(int x, int y);
void Wc1SdlSleep(DWORD milliseconds);

#define GetTickCount Wc1SdlGetTicks
#define GetAsyncKeyState Wc1SdlGetAsyncKeyState
#define OutputDebugString Wc1SdlOutputDebugString
#define OutputDebugStringA Wc1SdlOutputDebugString
#define SetCursorPos Wc1SdlSetCursorPosition
#define Sleep Wc1SdlSleep
#define timeGetTime Wc1SdlGetTicks

#define VK_CLEAR 0x0c
#define VK_SHIFT 0x10
#define VK_CONTROL 0x11
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

#define _open Wc1SdlOpen
#define _close close
#define _read read
#define _write write
#define _lseek lseek
#define _filelength Wc1SdlFileLength
#define _unlink unlink
#define _chdir chdir
#define _cprintf printf
#define _itoa Wc1SdlItoa
#define _ltoa Wc1SdlLtoa
#define _ultoa Wc1SdlUltoa
#define _strupr Wc1SdlStrupr
#endif

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
