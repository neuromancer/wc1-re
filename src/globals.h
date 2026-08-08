/*
 *  Recovered global state.
 *
 *  Rule (see AGENTS.md): a renamed global MUST keep its original address in the
 *  name, so a symbol can always be traced back to the binary.  Layout order in
 *  globals.c matters -- the linker emits these in declaration order and
 *  binary-comp compares the resulting data layout against the original.
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
extern short g_nFrameSkip_00469fb8;          /* int _FrameSkip = 1  (clamped 1..5) */
/* TODO: locate _FrameCount, Game_Mode, Game_Status, window_colored, version */

/* --------------------------------------------------------------------------
 * Main-module state confirmed from the reconstruction of main() and the
 * input/banner functions.
 * -------------------------------------------------------------------------- */
extern unsigned char g_bCurrentKey_0046c014;     /* key code from PollKeyboardState */
extern unsigned char g_bMessageSpeed_0046af68;   /* cycled (x+1)%5 */
extern short g_nOriginDevUnlock_00469ff4;        /* argv element == "Origin" */

#endif /* WC1_GLOBALS_H */
