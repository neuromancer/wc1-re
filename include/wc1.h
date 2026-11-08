/*
 *  ============================================================================
 *  |                            WINGLEADER                                    |
 *  |                  The 3D space combat simulator                           |
 *  |                                                                          |
 *  |          A game of interstellar fighter conflict.                        |
 *  |                                                                          |
 *  |          (c)1989,1990 Chris Roberts.  All rights reserved.               |
 *  ============================================================================
 */
/**
 * @file
 * @ingroup headers
 * @brief Umbrella header for the game core.
 *
 * The banner at the top of this file is reproduced from the leaked original
 * main-module header, whose module name was "MAIN GAME C SOURCE MODULE (Main
 * loop etc.)".
 *
 * The core is C, ported from 16-bit DOS sources. `int` was 16 bits in the
 * original, so nearly all game state is `short` here: using `int` where the
 * original used a 16-bit type produces 32-bit operations and breaks the
 * instruction comparison.
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

/* Gettext-style UI markers. Keep these in this existing header: introducing
 * another include changes VC++ 4.20's symbol keys and register allocation.
 * N_/NC_ only mark static initializers; translate those when displaying them.
 * The reference build sees exactly the original string expressions. */
#ifdef SDL_PORT
#define _(text) SdlTranslateText("", (text))
#define C_(context, text) SdlTranslateText((context), (text))
#else
#define _(text) text
#define C_(context, text) text
#endif
#define N_(text) text
#define NC_(context, text) text

#endif /* GAME_H */
