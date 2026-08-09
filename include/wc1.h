/*
 *  ============================================================================
 *  |                            WINGLEADER                                    |
 *  |                  The 3D space combat simulator                           |
 *  |                                                                          |
 *  |          A game of interstellar fighter conflict.                        |
 *  |                                                                          |
 *  |          (c)1989,1990 Chris Roberts.  All rights reserved.               |
 *  ============================================================================
 *
 *  Shared declarations for the Wing Commander (Kilrathi Saga, Win32)
 *  reconstruction.  The banner above is reproduced from the leaked original
 *  main-module header; the module name in that file was
 *  "MAIN GAME C SOURCE MODULE (Main loop etc.)".
 *
 *  The core is C, ported from 16-bit DOS sources.  `int` was 16 bits in the
 *  original, so nearly all game state is `short` here.  Using `int` where the
 *  original used a 16-bit type produces 32-bit operations and breaks the
 *  instruction comparison -- see AGENTS.md.
 */
#ifndef WC1_H
#define WC1_H

#ifndef WC1_ANALYSIS
#include <windows.h>
#include <direct.h>
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#endif

/* Degrees are the angular unit throughout the game core (the constant lives at
 * DAT_004631b0 in the original); the trig shims convert on the way in. */
#define WC1_DEG2RAD 0.017453292519943295

/* The DirectDraw back end releases every interface through this one shape. */
#define COM_RELEASE(p) \
    do { if ((p) != 0) { (**(void (**)(void *))(*(int *)(p) + 8))(p); (p) = 0; } } while (0)

/* --------------------------------------------------------------------------
 * Original 16-bit-era type spellings.
 * -------------------------------------------------------------------------- */
typedef short          INT16;
typedef unsigned short UINT16;
typedef unsigned char  UINT8;
typedef signed char    INT8;

/*
 * The DOS source spelled `BOOLEAN window_colored = FALSE;`, but <windows.h>
 * already provides BOOLEAN (as BYTE) and MSVC 4.2 rejects a redefinition with a
 * different base type.  The Win32 port therefore has to be using the windows.h
 * one, so do not redeclare it here.  TRUE/FALSE likewise come from windows.h.
 */
#ifdef WC1_ANALYSIS
typedef unsigned char BOOLEAN;
#ifndef TRUE
#define FALSE 0
#define TRUE  1
#endif
#endif

/* --------------------------------------------------------------------------
 * Enumerations named by the leaked main-module source.  The tag names and the
 * first enumerator of each are original:
 *     GAME_MODE   Game_Mode   = Player;
 *     GAME_STATUS Game_Status = ALIVE;
 * The remaining enumerators are NOT yet recovered -- do not invent values that
 * the assembly has not confirmed.
 * -------------------------------------------------------------------------- */
typedef enum {
    Player = 0
    /* TODO: remaining GAME_MODE enumerators unknown */
} GAME_MODE;

typedef enum {
    ALIVE = 0
    /* TODO: remaining GAME_STATUS enumerators unknown */
} GAME_STATUS;

/* --------------------------------------------------------------------------
 * Shared utility layer.  These were recovered by ranking the call graph by
 * fan-in; the addresses are the originals.
 * -------------------------------------------------------------------------- */
/* Variadic printers cannot be generated mechanically, so they live here. */
void ShowOnScreenMessage(short flags, short duration, const char *fmt, ...);
void SoundDebugPrintf(const char *fmt, ...);   /* 0x00403DB0 */
void SystemDebugPrintf(const char *fmt, ...);  /* 0x00425BB0 */

#ifndef WC1_ANALYSIS
#include "globals.h"
#include "wc1funcs.h"
#include "wc1extern.h"
#endif

#endif /* WC1_H */
