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
    g_cCurrentSeries_0059ca6a = 1;
    DAT_0046b168 = 0x13;
    DAT_004700ca = 1;
    g_cCurrentMission_0059ca69 = 0;
    DAT_004700c9 = 0;
    LoadOriginFxDrivers();
    DAT_0046b168 = 0x13;

    LoadVolumeSettingsFromRegistry();
    SetSoundEffectsVolume(DAT_00469fc8[DAT_00469fbc / 2]);
    SetMusicStreamVolume((unsigned short)DAT_00469fc8[DAT_00469fc0 / 2]);

    DAT_0059ab58 = 0;
    FrameStartHook();
    DAT_004688e0 = 1;
    Title_Sequence();
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

/* Function start: 0x427A00 */
unsigned int initialize_view_buffer(void)
{
    if (DAT_005a7510.pixels == 0) {
        if (AllocateViewport(&DAT_005a7510, (short)DAT_004699d8,
                             0x20) == 0)
            ReportOutOfMemoryAndExit();
    }
    return 0;
}

/* Function start: 0x427A40 */
void dump_buffer_to_screen(void)
{
    CopyViewportContents(&DAT_005a7510, &DAT_005a6ba0);
    ShowMemoryStatusDebug();
}

/* Function start: 0x427B00 */
unsigned int clear_view_buffer(void)
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

/* Function start: 0x427C50 */
void Update_3Space(void)
{
    house_keep();
    house_keep_objects();
    update_objects_in_space();
    set_eye_direction_and_position();
    g_nSpaceFrame_0059b420++;
}

/* Function start: 0x427C80 */
unsigned int SetDefaultCommDelay(void)
{
    if (DAT_0046c03c < 4)
        DAT_005a7780 = 0x30;
    return 0;
}

/* Function start: 0x427CD0 */
unsigned int UpdateSpacePaletteFade(void)
{
    if (DAT_005a7780 != 0) {
        if (DAT_0046b168 == 9 || DAT_0046b168 == 13) {
            ClearViewport(&DAT_005a7510, (short)DAT_004699ac);
            g_bViewportDirty_00469fc4 = 1;
            DAT_005a7780 = 0;
            return 0;
        }
        if (DAT_0046b168 != 0x13)
            return 0;
        DAT_005a7780 = (unsigned short)(DAT_005a7780 - 4);
        SetPaletteEntry((short)DAT_004699d8, (short *)&DAT_005a7780);
    }
    return 0;
}

/* Function start: 0x427D40 */
void house_keep(void)
{
    if (g_nCannedSceneMode_00469fac == 0 &&
        g_nTrainSimActive_00469e2c == 0 &&
        (g_nSpaceFrame_0059b420 & 0x1f) == 0)
        ReleaseStaleNavTarget();
}
