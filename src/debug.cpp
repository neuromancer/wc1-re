/*
 *  Win32 developer overlay console.
 *
 *  Address range 41c760-41d0bf (exact).  The new/delete call sites,
 *  implicit ECX object parameter, and callee-cleanup member arguments prove
 *  this isolated utility was compiled as C++ despite the surrounding game
 *  sources being C.
 */
#ifndef SDL_PORT
#include <malloc.h>
#include <locale.h>
#endif
#include "wc1.h"
#include <stdarg.h>
#include <signal.h>

int nDebugOverlayConsoleCount = 0; /* 0x00469644 */
DWORD dwDebugOverlayKey = 0; /* 0x00469648 */
DWORD dwDebugOverlayKeyLatch = 0; /* 0x0046964c */
HHOOK hDebugKeyboardHook = 0; /* 0x00469650 */

/* Function start: 0x41C760 */
DebugOverlayConsole::DebugOverlayConsole(HINSTANCE module,
                                         HWND targetWindow,
                                         int columnCount,
                                         int rowCount,
                                         int waitMode)
{
#ifndef SDL_PORT
    TEXTMETRICA metrics;
    HDC deviceContext;
#endif

    busyWait = waitMode;
    nDebugOverlayConsoleCount++;
    window = targetWindow;
    columns = columnCount;
    cursorRow = 0;
    rows = rowCount;
    cursorColumn = 0;
    textBuffer = (char *)malloc(rows * columns);
    dirtyLines = (unsigned char *)malloc(rows);
    memset(textBuffer, ' ', rows * columns);
    memset(dirtyLines, 1, rows);

#ifdef SDL_PORT
    (void)module;
    characterWidth = 8;
    characterHeight = 10;
#else
    deviceContext = GetDC(window);
    SelectObject(deviceContext,
                 CreateFontA(10, 10, 0, 0, 400, 0, 0, 0, 0, 2, 0, 0,
                             0x30, "Courier"));
#endif
    backgroundColor = 0;
    textColor = 0xffffff;
    backgroundMode = OPAQUE;
#ifndef SDL_PORT
    GetTextMetricsA(deviceContext, &metrics);
    SetTextColor(deviceContext, 0xffffff);
    SetBkColor(deviceContext, 0);
    ReleaseDC(window, deviceContext);
    characterWidth = metrics.tmMaxCharWidth;
    characterHeight = metrics.tmHeight;

    if (hDebugKeyboardHook == 0) {
        hDebugKeyboardHook =
            SetWindowsHookExA(WH_KEYBOARD, (HOOKPROC)DebugKeyboardHookProc,
                              module, 0);
    }
#endif
    reverseVideo = 0;
#ifdef SDL_PORT
    mutex = 0;
#else
    mutex = CreateMutexA(0, FALSE, 0);
#endif
    spinnerIndex = 0;
    animationState = 1;
    spinnerCharacters = (char *)malloc(5);
    strcpy(spinnerCharacters, "-\\|/");
}

/* Function start: 0x41C910 */
DebugOverlayConsole::~DebugOverlayConsole()
{
    animationState = 2;
#ifdef SDL_PORT
    nDebugOverlayConsoleCount--;
#else
    if (--nDebugOverlayConsoleCount == 0)
        UnhookWindowsHookEx(hDebugKeyboardHook);
#endif
    free(textBuffer);
    free(dirtyLines);
    free(spinnerCharacters);
}

/* Function start: 0x41C960 */
extern "C" DWORD WINAPI DebugOverlayWorkerProc(void *parameter)
{
    DebugOverlayConsole *console;
#ifndef SDL_PORT
    /* Retail compares against this stack slot before its first assignment. */
    int timer;
    DWORD waitResult;
    HDC deviceContext;
#endif

    console = (DebugOverlayConsole *)parameter;
#ifdef SDL_PORT
    while (console->animationState != 2)
        SDL_Delay(10);
    return 0;
#else
    for (;;) {
        if (console->animationState == 2) {
            ExitThread(0);
            return 0;
        }
        if ((int)timeGetTime() > timer + 500) {
            waitResult = WaitForSingleObject(console->mutex, 500);
            if (waitResult == WAIT_FAILED) {
                ExitThread(1);
            } else if (waitResult == WAIT_TIMEOUT) {
                continue;
            }
            if (console->animationState != 0) {
                deviceContext = GetDC(console->window);
                TextOutA(deviceContext,
                         console->characterWidth * console->cursorColumn,
                         console->characterHeight * console->cursorRow,
                         console->spinnerCharacters + console->spinnerIndex,
                         1);
                console->spinnerIndex++;
                if ((int)strlen(console->spinnerCharacters) ==
                    console->spinnerIndex) {
                    console->spinnerIndex = 0;
                }
                ReleaseDC(console->window, deviceContext);
            }
            ReleaseMutex(console->mutex);
            timer = timeGetTime();
        }
    }
#endif
}

/* Function start: 0x41CA60 */
extern "C" LRESULT CALLBACK DebugKeyboardHookProc(int code, WPARAM key,
                                                    LPARAM flags)
{
#ifdef SDL_PORT
    (void)code;
    if ((flags & 0x40000000) != 0) {
        dwDebugOverlayKey = (DWORD)key;
        dwDebugOverlayKeyLatch = (DWORD)key;
    }
    return 0;
#else
    if (code < 0)
        return CallNextHookEx(hDebugKeyboardHook,
                              code, key, flags);
    if ((flags & 0x40000000) != 0) {
        dwDebugOverlayKey = key;
        dwDebugOverlayKeyLatch = key;
    }
    return CallNextHookEx(hDebugKeyboardHook,
                          code, key, flags);
#endif
}

/* Function start: 0x41CAB0 */
extern "C" void DebugOverlayPrintf(DebugOverlayConsole *console,
                                     const char *format, ...)
{
    va_list arguments;
    int index;
    int length;
    signed char character;

    if (format != 0) {
        va_start(arguments, format);
        vsprintf(console->formatBuffer, format, arguments);
        va_end(arguments);
    } else {
        strcpy(console->formatBuffer, "\n");
    }
    length = strlen(console->formatBuffer);
    index = 0;
    while (index < length) {
        console->dirtyLines[console->cursorRow] = 1;
        character = console->formatBuffer[index];
        if (character >= ' ' && character <= '~') {
            console->textBuffer[console->cursorRow * console->columns +
                                console->cursorColumn] = character;
            console->cursorColumn++;
            if (console->cursorColumn == console->columns) {
                console->cursorColumn = 0;
                console->cursorRow++;
                if (console->cursorRow == console->rows)
                    console->Scroll();
            }
        } else {
            switch (character) {
            case '\a':
#ifdef SDL_PORT
                fputc('\a', stderr);
#else
                Beep(0, 0);
#endif
                break;
            case '\b':
                console->cursorColumn--;
                if (console->cursorColumn < 0) {
                    console->cursorColumn = 0;
                    console->cursorRow--;
                    if (console->cursorRow < 0)
                        console->cursorRow = 0;
                }
                console->textBuffer[
                    console->cursorRow * console->columns +
                    console->cursorColumn] = ' ';
                break;
            case '\n':
            case '\r':
                console->cursorColumn = 0;
                console->cursorRow++;
                if (console->cursorRow == console->rows)
                    console->Scroll();
                break;
            }
        }
        index++;
    }
    console->DrawPendingLines();
}

/* Function start: 0x41CC00 */
void DebugOverlayConsole::Clear(void)
{
    cursorRow = 0;
    cursorColumn = 0;
    memset(textBuffer, ' ', rows * columns);
    memset(dirtyLines, 1, rows);
}

/* Function start: 0x41CC50 */
void DebugOverlayConsole::Scroll(void)
{
    memmove(textBuffer, textBuffer + columns, (rows - 1) * columns);
    cursorRow = rows - 1;
    memset(textBuffer + cursorRow * columns, ' ', columns);
    memset(dirtyLines, 1, rows);
}

/* Function start: 0x41CCC0 */
void DebugOverlayConsole::DrawPendingLines(void)
{
#ifndef SDL_PORT
    int row;
    HDC deviceContext;

    deviceContext = GetDC(window);
    row = 0;
    while (row < rows) {
        BOOL (WINAPI *drawText)(HDC, int, int, LPCSTR, int) = TextOutA;

        if (dirtyLines[row] != 0) {
            drawText(deviceContext, 0, characterHeight * row,
                     textBuffer + row * columns, columns);
        }
        row++;
    }
    ReleaseDC(window, deviceContext);
#endif
    memset(dirtyLines, 0, rows);
}

/* Function start: 0x41CD40 */
char DebugOverlayConsole::WaitForKey(void)
{
#ifdef SDL_PORT
    char key;

    while (dwDebugOverlayKey == 0 &&
           PumpWindowMessages() != 0) {
        if (busyWait == 0)
            SDL_Delay(1);
    }
    if (dwDebugOverlayKey == 0)
        return 0x1b;
    key = (char)dwDebugOverlayKey;
    dwDebugOverlayKey = 0;
    return key;
#else
    RECT clip;
    MSG message;
    HANDLE process;
    int (WINAPI *showCursor)(BOOL);
    BOOL (WINAPI *setPriorityClass)(HANDLE, DWORD);
    int minimized;
    int complete;

    minimized = 0;
    complete = 0;
    if (busyWait == 0) {
        while (dwDebugOverlayKey == 0) {
            showCursor = ShowCursor;
            setPriorityClass = SetPriorityClass;
            while (complete == 0) {
                if (minimized != 0) {
                    if (GetMessageA(&message, 0, 0, 0) != 0) {
                        TranslateMessage(&message);
                        DispatchMessageA(&message);
                        complete = 1;
                    } else {
                        ShutdownGameWindow();
                    }
                    if (IsIconic(hMainWindow) == 0)
                        minimized = 0;
                    if (minimized == 0) {
                        clip.left = 0;
                        clip.top = 0;
                        clip.right = 320;
                        clip.bottom = 200;
                        ClipCursor(&clip);
                        showCursor(0);
                        process = GetCurrentProcess();
                        setPriorityClass(process, HIGH_PRIORITY_CLASS);
                        SetActiveWindow(hMainWindow);
                        SetForegroundWindow(hMainWindow);
                        DIBreInstall();
                        DIBslam();
                        DIBslamReal();
                    }
                } else {
                    if (PeekMessageA(&message, 0, 0, 0, PM_NOREMOVE) != 0) {
                        if (GetMessageA(&message, 0, 0, 0) != 0) {
                            TranslateMessage(&message);
                            DispatchMessageA(&message);
                            complete = 0;
                        } else {
                            ShutdownGameWindow();
                            complete = 0;
                        }
                    } else {
                        complete = 1;
                    }
                }
                if (IsIconic(hMainWindow) != 0) {
                    if (minimized == 0) {
                        ClipCursor(0);
                        showCursor(1);
                        process = GetCurrentProcess();
                        setPriorityClass(process, 0x40);
                    }
                    minimized = 1;
                }
                if (minimized != 0)
                    complete = 0;
            }
        }
    } else {
        while (dwDebugOverlayKey == 0)
            ;
    }
    {
        char key = (char)dwDebugOverlayKey;

        dwDebugOverlayKey = 0;
        return key;
    }
#endif
}

/* Function start: 0x41CF00 */
void DebugOverlayConsole::EnableReverseVideo(void)
{
#ifndef SDL_PORT
    HDC deviceContext;
#endif

    if (reverseVideo == 0) {
#ifndef SDL_PORT
        deviceContext = GetDC(window);
        SetBkColor(deviceContext, textColor);
        SetTextColor(deviceContext, backgroundColor);
#endif
        reverseVideo = 1;
#ifndef SDL_PORT
        ReleaseDC(window, deviceContext);
#endif
    }
}

/* Function start: 0x41CF50 */
void DebugOverlayConsole::DisableReverseVideo(void)
{
#ifndef SDL_PORT
    HDC deviceContext;
#endif

    if (reverseVideo != 0) {
#ifndef SDL_PORT
        deviceContext = GetDC(window);
        SetTextColor(deviceContext, textColor);
        SetBkColor(deviceContext, backgroundColor);
#endif
        reverseVideo = 0;
#ifndef SDL_PORT
        ReleaseDC(window, deviceContext);
#endif
    }
}

/* Function start: 0x41CFA0 */
void DebugOverlayConsole::SetOverlayTextColor(int red, int green, int blue)
{
#ifndef SDL_PORT
    HDC deviceContext;

    deviceContext = GetDC(window);
#endif
    textColor = red + (blue * 0x100 + green) * 0x100;
#ifndef SDL_PORT
    SetTextColor(deviceContext, textColor);
    ReleaseDC(window, deviceContext);
#endif
}

/* Function start: 0x41CFF0 */
void DebugOverlayConsole::SetOverlayBackgroundColor(int red, int green,
                                                    int blue)
{
#ifndef SDL_PORT
    HDC deviceContext;

    deviceContext = GetDC(window);
#endif
    backgroundColor = red + (blue * 0x100 + green) * 0x100;
#ifndef SDL_PORT
    SetBkColor(deviceContext, backgroundColor);
    ReleaseDC(window, deviceContext);
#endif
}

/* Function start: 0x41D040 */
void DebugOverlayConsole::SetTransparentBackground(void)
{
#ifndef SDL_PORT
    HDC deviceContext;
#endif

    backgroundMode = TRANSPARENT;
#ifndef SDL_PORT
    deviceContext = GetDC(window);
    SetBkMode(deviceContext, backgroundMode);
    ReleaseDC(window, deviceContext);
#endif
}

/* Function start: 0x41D080 */
void DebugOverlayConsole::SetOpaqueBackground(void)
{
#ifndef SDL_PORT
    HDC deviceContext;
#endif

    backgroundMode = OPAQUE;
#ifndef SDL_PORT
    deviceContext = GetDC(window);
    SetBkMode(deviceContext, backgroundMode);
    ReleaseDC(window, deviceContext);
#endif
}
