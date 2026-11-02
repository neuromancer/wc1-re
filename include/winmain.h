/**
 * @file
 * @ingroup headers
 * @brief Declarations for the Win32 shell.
 *
 * The shell (winmain.c) includes the common headers itself instead of wc1.h.
 * It does not use main.h, because its configuration reader stores single
 * bytes into three of main.c's option flags and declares those flags as
 * bytes, as the original port did.
 */
#ifndef WINMAIN_H
#define WINMAIN_H

#include "wc1base.h"
#include "wcdata.h"
#include "globals.h"

/* Shell state defined in winmain.c. */
extern HDC hMainWindowDC; /* 0x005a8a30 */

/* main.c option flags, as the shell's configuration reader stores them. */
extern char nOriginDevUnlock; /* 0x00469ff4 */
extern char bPlayerVulnerable; /* 0x00469ffc */
extern char bPlayerCollisionResponse; /* 0x0046a000 */

#include "wc1funcs.h"
#include "wc1extern.h"

#endif /* WINMAIN_H */
