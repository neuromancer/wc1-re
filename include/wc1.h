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
#ifndef GAME_H
#define GAME_H

#include "wc1base.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ANALYSIS_BUILD
#include "wcdata.h"
#include "globals.h"
#include "main.h"
#include "wc1funcs.h"
#include "wc1extern.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* GAME_H */
