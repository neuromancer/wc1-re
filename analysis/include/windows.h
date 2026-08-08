#ifndef TEACHER_ANALYSIS_WINDOWS_H
#define TEACHER_ANALYSIS_WINDOWS_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
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
#define WINAPI __stdcall
#endif
#ifndef CALLBACK
#define CALLBACK __stdcall
#endif
#ifndef APIENTRY
#define APIENTRY WINAPI
#endif

typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef short SHORT;
typedef unsigned short USHORT;
typedef unsigned int DWORD;
typedef unsigned int UINT;
typedef int LONG;
typedef uintptr_t WPARAM;
typedef intptr_t LPARAM;
typedef intptr_t LRESULT;
typedef char* LPSTR;
typedef const char* LPCSTR;
typedef void* LPVOID;
typedef void* HANDLE;
typedef HANDLE HWND;
typedef HANDLE HINSTANCE;
typedef HANDLE HDC;
typedef HANDLE HGDIOBJ;
typedef HANDLE HGLOBAL;
typedef HANDLE HPALETTE;
typedef HANDLE HBITMAP;
typedef HANDLE HFONT;
typedef HANDLE HMODULE;
typedef HANDLE HICON;
typedef HANDLE HCURSOR;
typedef HANDLE HMENU;
typedef HANDLE HBRUSH;
typedef HANDLE LPSECURITY_ATTRIBUTES;
typedef void* FARPROC;

#ifndef NULL
#define NULL 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#define INVALID_HANDLE_VALUE ((HANDLE)(intptr_t)-1)

typedef struct tagPOINT {
    LONG x;
    LONG y;
} POINT, *LPPOINT;

typedef struct tagRECT {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECT, *LPRECT;

typedef struct tagSIZE {
    LONG cx;
    LONG cy;
} SIZE, *LPSIZE;

typedef struct tagTEXTMETRICA {
    LONG tmHeight;
    LONG tmAscent;
    LONG tmDescent;
    LONG tmInternalLeading;
    LONG tmExternalLeading;
    LONG tmAveCharWidth;
    LONG tmMaxCharWidth;
    LONG tmWeight;
    LONG tmOverhang;
    LONG tmDigitizedAspectX;
    LONG tmDigitizedAspectY;
    BYTE tmFirstChar;
    BYTE tmLastChar;
    BYTE tmDefaultChar;
    BYTE tmBreakChar;
    BYTE tmItalic;
    BYTE tmUnderlined;
    BYTE tmStruckOut;
    BYTE tmPitchAndFamily;
    BYTE tmCharSet;
} TEXTMETRICA;

typedef struct tagMSG {
    HWND hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD time;
    POINT pt;
} MSG, *LPMSG;

typedef LRESULT (CALLBACK *WNDPROC)(HWND, UINT, WPARAM, LPARAM);

typedef struct tagWNDCLASSEXA {
    UINT cbSize;
    UINT style;
    WNDPROC lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    LPCSTR lpszMenuName;
    LPCSTR lpszClassName;
    HICON hIconSm;
} WNDCLASSEXA, *LPWNDCLASSEXA;

typedef struct tagPALETTEENTRY {
    BYTE peRed;
    BYTE peGreen;
    BYTE peBlue;
    BYTE peFlags;
} PALETTEENTRY, *LPPALETTEENTRY;

typedef struct tagLOGPALETTE {
    WORD palVersion;
    WORD palNumEntries;
    PALETTEENTRY palPalEntry[1];
} LOGPALETTE, *LPLOGPALETTE;

typedef struct tagRGBQUAD {
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPINFO {
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD bmiColors[1];
} BITMAPINFO;

typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME;

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME;

typedef struct _WAVEFORMATEX {
    WORD wFormatTag;
    WORD nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD nBlockAlign;
    WORD wBitsPerSample;
    WORD cbSize;
} WAVEFORMATEX;

typedef struct pcmwaveformat_tag {
    WAVEFORMATEX wf;
    WORD wBitsPerSample;
} PCMWAVEFORMAT;

typedef struct _OFSTRUCT {
    BYTE cBytes;
    BYTE fFixedDisk;
    WORD nErrCode;
    WORD Reserved1;
    WORD Reserved2;
    char szPathName[128];
} OFSTRUCT;

#define WM_CREATE 0x0001
#define WM_DESTROY 0x0002
#define WM_PAINT 0x000f
#define WM_CLOSE 0x0010
#define WM_QUERYNEWPALETTE 0x030f
#define WM_PALETTECHANGED 0x0311
#define WM_KEYDOWN 0x0100
#define PM_REMOVE 0x0001
#define BI_RGB 0
#define DIB_RGB_COLORS 0
#define SRCCOPY 0x00cc0020
#define STOCK_LAST 19

UINT RegisterClassExA(const WNDCLASSEXA*);
HWND CreateWindowExA(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
BOOL ShowWindow(HWND, int);
BOOL UpdateWindow(HWND);
HDC GetDC(HWND);
int ReleaseDC(HWND, HDC);
BOOL DeleteDC(HDC);
BOOL DeleteObject(HGDIOBJ);
HGLOBAL GlobalAlloc(UINT, size_t);
LPVOID GlobalLock(HGLOBAL);
BOOL GlobalUnlock(HGLOBAL);
HGLOBAL GlobalFree(HGLOBAL);
HGDIOBJ SelectObject(HDC, HGDIOBJ);
int SetDIBitsToDevice(HDC, int, int, DWORD, DWORD, int, int, UINT, UINT, const void*, const BITMAPINFO*, UINT);
int StretchDIBits(HDC, int, int, int, int, int, int, int, int, const void*, const BITMAPINFO*, UINT, DWORD);
BOOL InvalidateRect(HWND, const RECT*, BOOL);
LRESULT DefWindowProcA(HWND, UINT, WPARAM, LPARAM);
void PostQuitMessage(int);
BOOL PeekMessageA(MSG*, HWND, UINT, UINT, UINT);
BOOL TranslateMessage(const MSG*);
LRESULT DispatchMessageA(const MSG*);
HWND GetActiveWindow(void);
BOOL GetClientRect(HWND, RECT*);
BOOL GetCursorPos(POINT*);
BOOL ScreenToClient(HWND, POINT*);
HCURSOR LoadCursorA(HINSTANCE, LPCSTR);
HICON LoadIconA(HINSTANCE, LPCSTR);
HGDIOBJ GetStockObject(int);
HPALETTE SelectPalette(HDC, HPALETTE, BOOL);
UINT RealizePalette(HDC);
HPALETTE CreatePalette(const LOGPALETTE*);
UINT GetSystemPaletteEntries(HDC, UINT, UINT, PALETTEENTRY*);
DWORD SetTextColor(HDC, DWORD);
int SetBkMode(HDC, int);
UINT SetTextAlign(HDC, UINT);
BOOL GetTextExtentPointA(HDC, LPCSTR, int, SIZE*);
BOOL TextOutA(HDC, int, int, LPCSTR, int);
HANDLE CreateFileA(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
BOOL SetFileTime(HANDLE, const FILETIME*, const FILETIME*, const FILETIME*);
BOOL CloseHandle(HANDLE);
void GetLocalTime(SYSTEMTIME*);
void GetSystemTime(SYSTEMTIME*);
BOOL SystemTimeToFileTime(const SYSTEMTIME*, FILETIME*);
BOOL GetDiskFreeSpaceA(LPCSTR, DWORD*, DWORD*, DWORD*, DWORD*);
HMODULE LoadLibraryA(LPCSTR);
FARPROC GetProcAddress(HMODULE, LPCSTR);
BOOL FreeLibrary(HMODULE);
UINT GetSystemDirectoryA(LPSTR, UINT);
int MessageBoxA(HWND, LPCSTR, LPCSTR, UINT);
short GetAsyncKeyState(int);
HWND SetActiveWindow(HWND);
int GetSystemMetrics(int);
WORD GetWindowWord(HWND, int);
DWORD GetVersion(void);
int GetDeviceCaps(HDC, int);
BOOL GetTextMetricsA(HDC, TEXTMETRICA*);
BOOL AnimatePalette(HPALETTE, UINT, UINT, const PALETTEENTRY*);
BOOL SetCursorPos(int, int);
int ShowCursor(BOOL);
DWORD GetCurrentDirectoryA(DWORD, LPSTR);
unsigned long __stdcall timeGetTime(void);

#ifdef __cplusplus
}
#endif

#endif
