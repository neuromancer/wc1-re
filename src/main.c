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
 *          **  MAIN GAME C SOURCE MODULE (Main loop etc.)  **
 *
 *  Header reproduced from the leaked DOS-era source of this module.  The DOS
 *  build declared:
 *
 *      char *version = "F3.2";
 *      #include <game.h>
 *      #include <dos.h>
 *      extern unsigned _ovrbuffer = 0x1370;   // was 1020, 1150 worked -KLD
 *      extern void auto_pilot_sequence();
 *
 *  In this Win32 port `_ovrbuffer` is gone (it configured Borland's VROOMM
 *  overlay manager, which has no counterpart here) and the "F3.2" literal is
 *  not present in WC1.EXE -- the port was re-versioned.
 *
 *  Recovered layout of this compilation unit: code roughly 0x00427000-0x0042A800,
 *  statics banded around 0x00469E00-0x0046A400.  See docs/ORDER.md.
 */
#include "wc1.h"
#include "globals.h"

#include <stdlib.h>
#include <string.h>
#include <direct.h>

/* ---------------------------------------------------------------------------
 * Not yet implemented -- declared extern so this module links while the rest of
 * the reconstruction catches up.  Do not guess bodies for these (AGENTS.md).
 * --------------------------------------------------------------------------- */
extern short LoadWingCmdrCfgFile(short argc, char **argv);   /* 0x0042C580 */
extern void  LoadInstallDat(void);                           /* 0x0042C660 */
extern void  auto_pilot_sequence(void);                      /* named by the leaked header */

/* ---------------------------------------------------------------------------
 * main()   original address 0x004274E0
 *
 * Confirmed structure from the decompilation:
 *   - runs a short fixed sequence of subsystem init calls
 *   - LoadWingCmdrCfgFile(argc, argv)
 *   - chdir("gamedat"); LoadInstallDat(); chdir("..")
 *   - walks argv, string-comparing each element against "Origin" (7 bytes); a
 *     match sets g_nOriginDevUnlock_00469ff4, which gates the -b/-f/-k/-q
 *     developer switches
 *   - switch on argv[i][0] handling '?', '-', 'A'/'a', 'E'/'e', 'P'/'p', 'R'/'r'
 *   - prints "Bye!" on the way out
 *
 * Left unimplemented on purpose: writing the body before the init callees are
 * recovered would mean inventing call order, and call order is exactly what the
 * comparison is meant to verify.  Implement top-down as those functions land.
 * --------------------------------------------------------------------------- */

/* Function start: 0x4274E0 */
/* TODO: implement main(). See code-full/FUN_004274E0.disassembled.txt */
