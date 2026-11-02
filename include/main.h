/**
 * @file
 * @ingroup headers
 * @brief Option flags set from the command line, the configuration file and
 * the registry.
 *
 * main.c defines them, and wc1.h includes this header after globals.h for
 * every game module. The Win32 shell (winmain.c) does not: its configuration
 * reader stores single bytes into these three flags, so it declares them as
 * bytes itself, as the original port did. main.c's collision flag, which the
 * shell never touches, is declared in globals.h.
 */
#ifndef MAIN_H
#define MAIN_H

extern int nOriginDevUnlock; /* 0x00469ff4 */
extern int bPlayerVulnerable; /* 0x00469ffc */
extern int bPlayerCollisionResponse; /* 0x0046a000 */

#endif /* MAIN_H */
