/*
 *  ix diagnostic printer.
 *
 *  ix_log_printf sits immediately before streamer.cpp. Unlike the rest of
 *  ix it was linked as an object, not from a library: its CRT
 *  references (_CrtDbgReport, strncmp) are resolved before the entry point,
 *  and those of every ix library member after it (docs/ORDER.md). Its short
 *  branches, scheduled loads and POP EBP epilogue match the core optimizer
 *  profile, unlike the unoptimized ix library members.
 *
 *  Every ix diagnostic goes through here, 107 call sites, always in pairs:
 *      ix_log_printf("Fatal [%s - %d]:\n", __FILE__, __LINE__);
 *      ix_log_printf("human readable message");
 */
#include "ix.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#ifndef SDL_PORT
#include <crtdbg.h>
#endif


/* Formatting scratch buffer. */
char szLogBuf[1024];  /* 0x005977D0 */

/* Mono-monitor debug printer in the game core (C linkage). */
extern "C" void SoundDebugPrintf(const char *fmt, ...);   /* 0x00403DB0 */

/* Function start: 0x4426A0 */
void ix_log_printf(const char *fmt, ...)
{
    if (fmt != 0) {
        va_list arguments;

        va_start(arguments, fmt);
        vsprintf(szLogBuf, fmt, arguments);
        va_end(arguments);
    } else {
        strcpy(szLogBuf, "(null)");
    }

    if (strncmp(szLogBuf, "sleep_for", 9) != 0) {
        SoundDebugPrintf("%s", szLogBuf);
#ifdef SDL_PORT
        SdlOutputDebugString(szLogBuf);
#else
        if (_CrtDbgReport(0, 0, 0, 0, "%s", szLogBuf) == 1)
            _CrtDbgBreak();
#endif
    }
}

/* Function start: 0x442740 */
/* Returns 1. Nothing in WC1 calls it; the log module's initializer
 * reporting success, by analogy with the other ix modules' init entries. */
int ix_log_init_hook(void)
{
    return 1;
}
