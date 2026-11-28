/* Globals of the original game's units that the port shares. The Win32
 * build keeps them private to their units. */
#ifndef SDL_PORT_ORIGINAL_GLOBALS_H
#define SDL_PORT_ORIGINAL_GLOBALS_H

#include "platform/win32.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Spaceflight's frame rate cap (src/dib.c), which the game's speed follows:
 * 20 in the Kilrathi Saga, 8 to 32 in half steps. */
extern float fSpaceFlightFrameRate;
extern int nInsertKeyDown; /* 0x00475b68 */
extern int nOemPeriodKeyStatus; /* 0x00475bf0 */
extern int nHomeState; /* 0x00475bf4 */
extern int nPageDownKeyHeld; /* 0x00475bf8 */
extern int nOemCommaKeyPressed; /* 0x00475bfc */
extern int nRightArrowKeyHeld; /* 0x00475c00 */
extern int nEndKeyDown; /* 0x00475c04 */
extern int nDeleteKeyDown; /* 0x00475c0c */
extern int nUpArrowKeyState; /* 0x00475c10 */
extern int nDownArrowKeyState; /* 0x00475c14 */
extern int nPageUpKeyState; /* 0x00475d18 */
extern int nLeftArrowKeyState; /* 0x00475d1c */
extern HWND hDIBParentWindow; /* 0x00486074 */
extern char szSfxWavePath[0xc8]; /* 0x00476558 */
extern unsigned char abCursorSaveArea[0x1000]; /* 0x004865a8 */
extern char *apszShipMissionTypeNames[11]; /* 0x00468728 */
extern short nPendingMedalIndex; /* 0x004688e4 */
extern short nPostSeriesSequence; /* 0x004688e8 */
extern int bAlternateChaseView; /* 0x00468ffc */
extern int bInitialFormationSetup; /* 0x00465c84 */
extern int anPilotTurnInterval[18]; /* 0x00465fc8 */
extern int bMouseAfterburnerControl; /* 0x0046a02c */
extern unsigned char bFlightRollLatch; /* 0x0046a050 */
extern short nMouseYawInput; /* 0x0046a058 */
extern short nMousePitchInput; /* 0x0046a05c */
extern int anVolumeLevels[11]; /* 0x00469fc8 */

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_ORIGINAL_GLOBALS_H */
