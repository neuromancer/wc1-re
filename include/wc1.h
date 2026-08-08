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
#endif

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
short MinShort(short a, short b);              /* 0x0041D0C0, 39 callers */
short MaxShort(short a, short b);              /* 0x0041D0E0, 55 callers */
short RandomBelowOrEqual(short n);             /* 0x00434D50, rand() % (n+1) */

/* __stdcall, not __cdecl: the original ends in `RET 8` (callee cleanup). */
short __stdcall RandomInRange(short lo, short hi); /* 0x00434D20 */

/* Variadic on-screen message printer used by every status banner. */
void ShowOnScreenMessage(short flags, short duration, const char *fmt, ...);

#endif /* WC1_H */
