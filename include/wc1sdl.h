/* SDL2 host declarations used only by the native port. */
#ifndef WC1_SDL_H
#define WC1_SDL_H

#ifndef WC1_SDL
#error "wc1sdl.h is only for the WC1_SDL build"
#endif

#include <SDL.h>
#include <stdint.h>

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
