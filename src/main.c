/*
 *  WINGLEADER main module.
 *
 *  Address range 0x4274e0-0x427fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: main() at 0x004274E0, confirmed against the leaked DOS source screenshot.
 */
#include "wc1.h"

/* Function start: 0x4274E0 */
int main(short argc, char **argv)
{
    (void)argc;
    (void)argv;

    GetShutdownErrorCode();
    ExitCleanupHook();
    StartupHook();
    DAT_0059ab4c = GetStartupErrorCode();
    ShutdownHook();

    _chdir("gamedat");
    LoadInstallDat();
    _chdir("..");
    if (GetDiskChangeCount() > 0x42)
        DAT_0059ab34 = 1;

    SystemDebugPrintf("Loading WING COMMANDER.  Please wait...\n");
    DAT_005a7d9c = 1;
    DAT_0046a9f8 = 4;
    DAT_0059a856 = 1;

    SetFpsCapDisabled();
    DAT_0059ca6a = 1;
    DAT_0046b168 = 0x13;
    DAT_004700ca = 1;
    DAT_0059ca69 = 0;
    DAT_004700c9 = 0;
    LoadOriginFxDrivers();
    DAT_0046b168 = 0x13;

    LoadVolumeSettingsFromRegistry();
    SetSoundEffectsVolume(DAT_00469fc8[DAT_00469fbc / 2]);
    SetMusicStreamVolume((unsigned short)DAT_00469fc8[DAT_00469fc0 / 2]);

    DAT_0059ab58 = 0;
    FrameStartHook();
    DAT_004688e0 = 1;
    RunIntroAndMainMenu();
    return 0;
}

/* Function start: 0x4279D0 */
unsigned int GetScreenUpdateFlag(void)
{
    if (IsAutopilotEngaged())
        EndCommMenu();
    if (DAT_005a7510.pixels != 0)
        free_viewport(&DAT_005a7510);
    return 0;
}

/* Function start: 0x427B00 */
unsigned int DrawStatusBarBackdrop(void)
{
    ClearViewport(&DAT_005a7510, (unsigned char)DAT_004699d8);
    return 0;
}

/* Function start: 0x427BA0 */
unsigned int ResetScreenClipToFullHeight(void)
{
    free_viewport(&DAT_005a76b0);
    DAT_005a6baa = 0;
    DAT_005a6bae = 199;
    return 0;
}

/* Function start: 0x427C30 */
unsigned int RefreshMemoryStatusOverlay(void)
{
    WaitForVerticalBlankThunk();
    CopyViewportContents(&DAT_005a76b0, &DAT_005a6ba0);
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
