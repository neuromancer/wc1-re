#include "wc1.h"

#include <stdarg.h>

void SystemDebugPrintf(const char *format, ...)
{
    va_list arguments;

    va_start(arguments, format);
    vfprintf(stderr, format, arguments);
    va_end(arguments);
}

char PumpMessagesDuringWait(void)
{
    char key;

    while (g_dwDebugOverlayKey_00469648 == 0 && PumpWindowMessages() != 0)
        SDL_Delay(1);
    if (g_dwDebugOverlayKey_00469648 == 0)
        return 0x1b;
    key = (char)g_dwDebugOverlayKey_00469648;
    g_dwDebugOverlayKey_00469648 = 0;
    return key;
}

unsigned char TakeDebugStepFlag(void)
{
    unsigned char value;

    value = (unsigned char)g_dwDebugOverlayKeyLatch_0046964c;
    g_dwDebugOverlayKeyLatch_0046964c = 0;
    return value;
}

void ClearDebugPauseFlags(void)
{
    g_dwDebugOverlayKeyLatch_0046964c = 0;
    g_dwDebugOverlayKey_00469648 = 0;
}
