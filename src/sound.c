/*
 *  Wave playback, volume settings and INSTALL.DAT.
 *
 *  Address range 0x42b400-0x42cfff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: playWAVE/PlaySfxWaveByIndex/LoadInstallDat; string band 0x46A46C-0x46A710.
 */
#include "wc1.h"

/* Function start: 0x42C410 */
void FxDriverShutdownHook(void)
{
}

/* Function start: 0x42CDA0 */
unsigned short GetJoystickPresentUnused(void)
{
    return 1;
}
