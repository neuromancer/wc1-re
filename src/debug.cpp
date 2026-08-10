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
    TEXTMETRICA metrics;
    HDC deviceContext;

    busyWait = waitMode;
    g_nDebugOverlayConsoleCount_00469644++;
    window = targetWindow;
    columns = columnCount;
    cursorRow = 0;
    rows = rowCount;
    cursorColumn = 0;
    textBuffer = (char *)malloc(rows * columns);
    dirtyLines = (unsigned char *)malloc(rows);
    memset(textBuffer, ' ', rows * columns);
    memset(dirtyLines, 1, rows);

    deviceContext = GetDC(window);
    SelectObject(deviceContext,
                 CreateFontA(10, 10, 0, 0, 400, 0, 0, 0, 0, 2, 0, 0,
                             0x30, g_szDebugOverlayFontName_00469654));
    backgroundColor = 0;
    textColor = 0xffffff;
    backgroundMode = OPAQUE;
    GetTextMetricsA(deviceContext, &metrics);
    SetTextColor(deviceContext, 0xffffff);
    SetBkColor(deviceContext, 0);
    ReleaseDC(window, deviceContext);
    characterWidth = metrics.tmMaxCharWidth;
    characterHeight = metrics.tmHeight;

    if (g_hDebugKeyboardHook_00469650 == 0) {
        g_hDebugKeyboardHook_00469650 =
            SetWindowsHookExA(WH_KEYBOARD, (HOOKPROC)DebugKeyboardHookProc,
                              module, 0);
    }
    reverseVideo = 0;
    mutex = CreateMutexA(0, FALSE, 0);
    spinnerIndex = 0;
    animationState = 1;
    spinnerCharacters = (char *)malloc(5);
    strcpy(spinnerCharacters, g_szDebugOverlaySpinner_0046965c);
}

/* Function start: 0x41C910 */
DebugOverlayConsole::~DebugOverlayConsole()
{
    animationState = 2;
    if (--g_nDebugOverlayConsoleCount_00469644 == 0)
        UnhookWindowsHookEx(g_hDebugKeyboardHook_00469650);
    free(textBuffer);
    free(dirtyLines);
    free(spinnerCharacters);
}

/* Function start: 0x41C960 */
extern "C" DWORD WINAPI DebugOverlayWorkerProc(void *parameter)
{
    DebugOverlayConsole *console;
    DWORD timer;
    DWORD waitResult;
    HDC deviceContext;

    console = (DebugOverlayConsole *)parameter;
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
}

/* Function start: 0x41CA60 */
extern "C" LRESULT CALLBACK DebugKeyboardHookProc(int code, WPARAM key,
                                                    LPARAM flags)
{
    if (code < 0)
        return CallNextHookEx(g_hDebugKeyboardHook_00469650,
                              code, key, flags);
    if ((flags & 0x40000000) != 0) {
        g_dwDebugOverlayKey_00469648 = key;
        g_dwDebugOverlayKeyLatch_0046964c = key;
    }
    return CallNextHookEx(g_hDebugKeyboardHook_00469650,
                          code, key, flags);
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
        strcpy(console->formatBuffer, g_szDebugOverlayNewline_00469664);
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
                Beep(0, 0);
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
    memset(dirtyLines, 0, rows);
}

/* Function start: 0x41CD40 */
char DebugOverlayConsole::WaitForKey(void)
{
    RECT clip;
    MSG message;
    HANDLE process;
    int minimized;
    int complete;

    minimized = 0;
    complete = 0;
    if (busyWait == 0) {
        while (g_dwDebugOverlayKey_00469648 == 0) {
            while (complete == 0) {
                if (minimized != 0) {
                    if (GetMessageA(&message, 0, 0, 0) != 0) {
                        complete = 1;
                        TranslateMessage(&message);
                        DispatchMessageA(&message);
                    } else {
                        ShutdownGameWindow();
                    }
                    if (IsIconic(DAT_005a89a0) == 0)
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
                        SetActiveWindow(DAT_005a89a0);
                        SetForegroundWindow(DAT_005a89a0);
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
                if (IsIconic(DAT_005a89a0) != 0) {
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
        while (g_dwDebugOverlayKey_00469648 == 0)
            ;
    }
    {
        char key = (char)g_dwDebugOverlayKey_00469648;

        g_dwDebugOverlayKey_00469648 = 0;
        return key;
    }
}

/* Function start: 0x41CF00 */
void DebugOverlayConsole::EnableReverseVideo(void)
{
    HDC deviceContext;

    if (reverseVideo == 0) {
        deviceContext = GetDC(window);
        SetBkColor(deviceContext, textColor);
        SetTextColor(deviceContext, backgroundColor);
        reverseVideo = 1;
        ReleaseDC(window, deviceContext);
    }
}

/* Function start: 0x41CF50 */
void DebugOverlayConsole::DisableReverseVideo(void)
{
    HDC deviceContext;

    if (reverseVideo != 0) {
        deviceContext = GetDC(window);
        SetTextColor(deviceContext, textColor);
        SetBkColor(deviceContext, backgroundColor);
        reverseVideo = 0;
        ReleaseDC(window, deviceContext);
    }
}

/* Function start: 0x41CFA0 */
void DebugOverlayConsole::SetOverlayTextColor(int red, int green, int blue)
{
    HDC deviceContext;

    deviceContext = GetDC(window);
    textColor = red + (blue * 0x100 + green) * 0x100;
    SetTextColor(deviceContext, textColor);
    ReleaseDC(window, deviceContext);
}

/* Function start: 0x41CFF0 */
void DebugOverlayConsole::SetOverlayBackgroundColor(int red, int green,
                                                    int blue)
{
    HDC deviceContext;

    deviceContext = GetDC(window);
    backgroundColor = red + (blue * 0x100 + green) * 0x100;
    SetBkColor(deviceContext, backgroundColor);
    ReleaseDC(window, deviceContext);
}

/* Function start: 0x41D040 */
void DebugOverlayConsole::SetTransparentBackground(void)
{
    HDC deviceContext;

    backgroundMode = TRANSPARENT;
    deviceContext = GetDC(window);
    SetBkMode(deviceContext, backgroundMode);
    ReleaseDC(window, deviceContext);
}

/* Function start: 0x41D080 */
void DebugOverlayConsole::SetOpaqueBackground(void)
{
    HDC deviceContext;

    backgroundMode = OPAQUE;
    deviceContext = GetDC(window);
    SetBkMode(deviceContext, backgroundMode);
    ReleaseDC(window, deviceContext);
}
