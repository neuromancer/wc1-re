/*
 *  Recovered global state.
 *
 *  Rule (see AGENTS.md): a renamed global MUST keep its original address in the
 *  name, so a symbol can always be traced back to the binary.  Definitions and
 *  declaration order belong to their evidence-backed compilation units;
 *  src/globals.c contains only the ownership still to be recovered.
 */
#ifndef WC1_GLOBALS_H
#define WC1_GLOBALS_H

#include "wc1.h"

/* --------------------------------------------------------------------------
 * From the leaked WINGLEADER main-module source:
 *
 *     GAME_MODE   Game_Mode   = Player;
 *     GAME_STATUS Game_Status = ALIVE;
 *     int _FrameCount = 1;
 *     int _FrameSkip  = 1;
 *     BOOLEAN window_colored = FALSE;
 *
 * Addresses are filled in as each is located.  _FrameSkip is confirmed:
 * ReportFramesSkipped clamps it with MinShort(MaxShort(v + d, 1), 5).
 * -------------------------------------------------------------------------- */
extern short g_nFrameSkip_0049d764;          /* int _FrameSkip = 1  (clamped 1..5) */
/* TODO: locate _FrameCount, Game_Mode, Game_Status, window_colored, version */

/* --------------------------------------------------------------------------
 * Main-module state confirmed from the reconstruction of main() and the
 * input/banner functions.
 * -------------------------------------------------------------------------- */
extern unsigned char g_bCurrentKey_0046c014;     /* key code from PollKeyboardState */
extern signed char g_cPreviousKey_0046c018;
extern signed char g_cMessageSpeed_0049b778;     /* cycled (x+1)%5 */
extern short g_nOriginDevUnlock_0049d774;        /* argv element == "Origin" */
extern unsigned long g_dwGameClockBase_005a89a8; /* GetTickCount epoch, randomised at startup */

#endif /* WC1_GLOBALS_H */
