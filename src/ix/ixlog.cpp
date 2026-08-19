/*
 *  ix diagnostic printer.
 *
 *  MODULE BOUNDARY UNVERIFIED: ix_log_printf sits at 0x004426A0, below
 *  streamer.cpp (0x00442750), so it belongs to an earlier ix object file whose
 *  extent is not yet known.  Built /Od like the rest of ix.
 *
 *  Every ix diagnostic goes through here, 107 call sites, always in pairs:
 *      ix_log_printf("Fatal [%s - %d]:\n", __FILE__, __LINE__);
 *      ix_log_printf("human readable message");
 */
#include "ix.h"
#ifdef WC1_SDL
#include <stdarg.h>
#endif
#include <stdio.h>
#include <string.h>
#ifndef WC1_SDL
#include <crtdbg.h>
#endif


/* Formatting scratch buffer. */
char szLogBuf[1024];

/* Mono-monitor debug printer in the game core (C linkage). */
extern "C" void SoundDebugPrintf(const char *fmt, ...);   /* 0x00403DB0 */

/* Function start: 0x4426A0 */
void ix_log_printf(const char *fmt, ...)
{
    if (fmt != 0) {
#ifdef WC1_SDL
        va_list arguments;

        va_start(arguments, fmt);
        vsprintf(szLogBuf, fmt, arguments);
        va_end(arguments);
#else
        vsprintf(szLogBuf, fmt, (char *)(&fmt + 1));
#endif
    } else {
        strcpy(szLogBuf, "(null)");
    }

    if (strncmp(szLogBuf, "sleep_for", 9) != 0) {
        SoundDebugPrintf("%s", szLogBuf);
#ifdef WC1_SDL
        Wc1SdlOutputDebugString(szLogBuf);
#else
        if (_CrtDbgReport(0, 0, 0, 0, "%s", szLogBuf) == 1)
            _CrtDbgBreak();
#endif
    }
}
