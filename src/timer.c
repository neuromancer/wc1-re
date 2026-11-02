/**
 * @file
 * @ingroup shell
 * @brief Win32 frame timer and developer step waiting.
 *
 * Address range 42afa0-42b04f.
 */
#include "wc1.h"


/* Linker commons defined here; LINK orders them by identifier hash. */
TextContext stLeftVduTextContext; /* 0x005a74f0 */
Viewport stSpaceBufferViewport; /* 0x005a7510 */
Viewport stRightVduViewport; /* 0x005a7530 */
Viewport stPilotHandBackdropView; /* 0x005a7550 */
struct Shape *pCockpitWeaponShapePtr; /* 0x005a7564 */
Viewport stCurrentConvTextViewport; /* 0x005a7570 */
char szDefaultTxtBuf[0xc8]; /* 0x005a7590 */
struct Shape *pCockpitIndicatorShape; /* 0x005a7658 */
struct Shape *pCurrentConstellationShape; /* 0x005a765c */

/* Function start: 0x42AFA0 */
void WaitForDebugStep(void)
{
    while (TakeDebugStepFlag() == 0) ;
}

/* Function start: 0x42AFB0 */
void CALLBACK FrameTimerCallback(UINT timerId, UINT message,
                                 DWORD user, DWORD first, DWORD second)
{
    (void)timerId;
    (void)message;
    (void)user;
    (void)first;
    (void)second;
    bFrameTickPendingFlag = 0;
}

/* Function start: 0x42AFC0 */
void SetMultimediaTimerCallback(int period)
{
    int milliseconds = period * 1000 / 60;

    if (period == 0) {
        bFrameTickPendingFlag = 0;
        if (nCurrentFrameTimerIdValue != 0) {
            timeKillEvent(nCurrentFrameTimerIdValue);
            nCurrentFrameTimerIdValue = 0;
        }
    } else {
        if (nCurrentFrameTimerIdValue != 0) {
            timeKillEvent(nCurrentFrameTimerIdValue);
            nCurrentFrameTimerIdValue = 0;
        }
        bFrameTickPendingFlag = 1;
        nCurrentFrameTimerIdValue = timeSetEvent(milliseconds, milliseconds,
                                    FrameTimerCallback, 0, 0);
    }
}
