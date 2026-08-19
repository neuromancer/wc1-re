/*
 *  Win32 developer overlay console.
 *
 *  Address range 0x41c760-0x41d0bf (exact).  The new/delete call sites,
 *  implicit ECX object parameter, and callee-cleanup member arguments prove
 *  this isolated utility was compiled as C++ despite the surrounding game
 *  sources being C.
 */
#include "wc1.h"
#include <stdarg.h>

#pragma function(memcpy)

/* Function start: 0x41C760 */
DebugOverlayConsole::DebugOverlayConsole(HINSTANCE module,
                                         HWND targetWindow,
                                         int columnCount,
                                         int rowCount,
                                         int waitMode)
{
#ifndef WC1_SDL
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

#ifdef WC1_SDL
    (void)module;
    characterWidth = 8;
    characterHeight = 10;
#else
    deviceContext = GetDC(window);
    SelectObject(deviceContext,
                 CreateFontA(10, 10, 0, 0, 400, 0, 0, 0, 0, 2, 0, 0,
                             0x30, szDebugOverlayFontName));
#endif
    backgroundColor = 0;
    textColor = 0xffffff;
    backgroundMode = OPAQUE;
#ifndef WC1_SDL
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
#ifdef WC1_SDL
    mutex = 0;
#else
    mutex = CreateMutexA(0, FALSE, 0);
#endif
    spinnerIndex = 0;
    animationState = 1;
    spinnerCharacters = (char *)malloc(5);
    strcpy(spinnerCharacters, szDebugOverlaySpinner);
}

/* Function start: 0x41C910 */
DebugOverlayConsole::~DebugOverlayConsole()
{
    animationState = 2;
#ifdef WC1_SDL
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
#ifndef WC1_SDL
    /* Retail compares against this stack slot before its first assignment. */
    DWORD timer;
    DWORD waitResult;
    HDC deviceContext;
#endif

    console = (DebugOverlayConsole *)parameter;
#ifdef WC1_SDL
    while (console->animationState != 2)
        SDL_Delay(10);
    return 0;
#else
    while (console->animationState != 2) {
        if (timeGetTime() > timer + 500) {
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
    ExitThread(0);
    return 0;
#endif
}

/* Function start: 0x41CA60 */
extern "C" LRESULT CALLBACK DebugKeyboardHookProc(int code, WPARAM key,
                                                    LPARAM flags)
{
#ifdef WC1_SDL
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
    int length;
    int index;
    signed char character;

    if (format != 0) {
        va_start(arguments, format);
        vsprintf(console->formatBuffer, format, arguments);
        va_end(arguments);
    } else {
        strcpy(console->formatBuffer, szDebugOverlayNewline);
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
#ifdef WC1_SDL
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
    memcpy(textBuffer, textBuffer + columns, (rows - 1) * columns);
    cursorRow = rows - 1;
    memset(textBuffer + cursorRow * columns, ' ', columns);
    memset(dirtyLines, 1, rows);
}

/* Function start: 0x41CCC0 */
void DebugOverlayConsole::DrawPendingLines(void)
{
#ifndef WC1_SDL
    HDC deviceContext;
    int row;

    deviceContext = GetDC(window);
    row = 0;
    while (row < rows) {
        if (dirtyLines[row] != 0) {
            TextOutA(deviceContext, 0, characterHeight * row,
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
#ifdef WC1_SDL
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
    int minimized;
    int complete;

    minimized = 0;
    complete = 0;
    if (busyWait == 0) {
        while (dwDebugOverlayKey == 0) {
            while (complete == 0) {
                if (minimized != 0) {
                    if (GetMessageA(&message, 0, 0, 0) != 0) {
                        complete = 1;
                        TranslateMessage(&message);
                        DispatchMessageA(&message);
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
                        ShowCursor(0);
                        process = GetCurrentProcess();
                        SetPriorityClass(process, HIGH_PRIORITY_CLASS);
                        SetActiveWindow(hMainWindow);
                        SetForegroundWindow(hMainWindow);
                        DIBreInstall();
                        DIBslam();
                        DIBslamReal();
                    }
                } else {
                    if (PeekMessageA(&message, 0, 0, 0, PM_NOREMOVE) != 0) {
                        if (GetMessageA(&message, 0, 0, 0) != 0) {
                            complete = 0;
                            TranslateMessage(&message);
                            DispatchMessageA(&message);
                        } else {
                            complete = 0;
                            ShutdownGameWindow();
                        }
                    } else {
                        complete = 1;
                    }
                }
                if (IsIconic(hMainWindow) != 0) {
                    if (minimized == 0) {
                        ClipCursor(0);
                        ShowCursor(1);
                        process = GetCurrentProcess();
                        SetPriorityClass(process, 0x40);
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
#ifndef WC1_SDL
    HDC deviceContext;
#endif

    if (reverseVideo == 0) {
#ifndef WC1_SDL
        deviceContext = GetDC(window);
        SetBkColor(deviceContext, textColor);
        SetTextColor(deviceContext, backgroundColor);
#endif
        reverseVideo = 1;
#ifndef WC1_SDL
        ReleaseDC(window, deviceContext);
#endif
    }
}

/* Function start: 0x41CF50 */
void DebugOverlayConsole::DisableReverseVideo(void)
{
#ifndef WC1_SDL
    HDC deviceContext;
#endif

    if (reverseVideo != 0) {
#ifndef WC1_SDL
        deviceContext = GetDC(window);
        SetTextColor(deviceContext, textColor);
        SetBkColor(deviceContext, backgroundColor);
#endif
        reverseVideo = 0;
#ifndef WC1_SDL
        ReleaseDC(window, deviceContext);
#endif
    }
}

/* Function start: 0x41CFA0 */
void DebugOverlayConsole::SetOverlayTextColor(int red, int green, int blue)
{
#ifndef WC1_SDL
    HDC deviceContext;

    deviceContext = GetDC(window);
#endif
    textColor = red + (blue * 0x100 + green) * 0x100;
#ifndef WC1_SDL
    SetTextColor(deviceContext, textColor);
    ReleaseDC(window, deviceContext);
#endif
}

/* Function start: 0x41CFF0 */
void DebugOverlayConsole::SetOverlayBackgroundColor(int red, int green,
                                                    int blue)
{
#ifndef WC1_SDL
    HDC deviceContext;

    deviceContext = GetDC(window);
#endif
    backgroundColor = red + (blue * 0x100 + green) * 0x100;
#ifndef WC1_SDL
    SetBkColor(deviceContext, backgroundColor);
    ReleaseDC(window, deviceContext);
#endif
}

/* Function start: 0x41D040 */
void DebugOverlayConsole::SetTransparentBackground(void)
{
#ifndef WC1_SDL
    HDC deviceContext;
#endif

    backgroundMode = TRANSPARENT;
#ifndef WC1_SDL
    deviceContext = GetDC(window);
    SetBkMode(deviceContext, backgroundMode);
    ReleaseDC(window, deviceContext);
#endif
}

/* Function start: 0x41D080 */
void DebugOverlayConsole::SetOpaqueBackground(void)
{
#ifndef WC1_SDL
    HDC deviceContext;
#endif

    backgroundMode = OPAQUE;
#ifndef WC1_SDL
    deviceContext = GetDC(window);
    SetBkMode(deviceContext, backgroundMode);
    ReleaseDC(window, deviceContext);
#endif
}
