/*
 *  WINGLEADER main module.
 *
 *  Address range 0x4274e0-0x427fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: main() at 0x004274E0, confirmed against the leaked DOS source screenshot.
 */
#include "wc1.h"

/*
 *  main() opens this module.  It is deliberately not written yet: its body is
 *  the init call order, and inventing that order would assert the very thing
 *  the comparison exists to check.  Its address annotation is omitted on
 *  purpose -- binary-comp binds an annotation to the next function definition,
 *  so an annotation with no function under it silently scores the following
 *  function against main().  See code-full/FUN_004274E0.disassembled.txt.
 */

/* Function start: 0x4279D0 */
unsigned int GetScreenUpdateFlag(void)
{
    if (IsAutopilotEngaged())
        EndCommMenu();
    if (DAT_005a7510 != 0)
        ReleaseTbl005a7f10FnF940(&DAT_005a7510);
    return 0;
}

/* Function start: 0x427B00 */
unsigned int DrawStatusBarBackdrop(void) { BlitUiFn1AE0(&DAT_005a7510, DAT_004699d8); return 0; }

/* Function start: 0x427BA0 */
unsigned int ResetScreenClipToFullHeight(void)
{
    ReleaseTbl005a7f10FnF940(&DAT_005a76b0);
    DAT_005a6baa = 0;
    DAT_005a6bae = 199;
    return 0;
}

/* Function start: 0x427C30 */
unsigned int RefreshMemoryStatusOverlay(void)
{
    ReleaseDirectDrawPalette();
    DoLocalFn1A90(&DAT_005a76b0, &DAT_005a6ba0);
    ShowMemoryStatusDebug();
    return 0;
}

/* Function start: 0x427C80 */
unsigned int SetDefaultCommDelay(void)
{
    if (DAT_0046c03c < 4)
        DAT_005a7780 = 0x30;
    return 0;
}
