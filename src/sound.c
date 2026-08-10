/*
 *  Wave playback, volume settings and INSTALL.DAT.
 *
 *  Address range 0x42b400-0x42cfff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: playWAVE/PlaySfxWaveByIndex/LoadInstallDat; string band 0x46A46C-0x46A710.
 */
#include "wc1.h"

/* Function start: 0x42B640 */
void stop_all_sounds(void)
{
    ix_system_delete_all_sounds();
    ix_system_delete_all_samples();
    FreeWaveTable();
    if (g_pLoopingWaveSound_00476550 != 0) {
        ix_sound_stop(g_pLoopingWaveSound_00476550);
        ix_sound_release(g_pLoopingWaveSound_00476550);
        g_pLoopingWaveSound_00476550 = 0;
        g_pLoopingWaveSample_0047654c = 0;
    }
}

/* Function start: 0x42B7D0 */
__declspec(naked) void ServiceSoundSystem(void)
{
    __asm { jmp ix_system_service_sounds }
}

/* Function start: 0x42B7E0 */
void SetSoundEffectsVolume(int volume)
{
    if (volume >= 0 && volume < 65000)
        ix_system_set_master_volume((unsigned short)volume);
    SoundDebugPrintf("Setting SFX Volume to %d", volume);
}

/* Function start: 0x42B870 */
void LoadVolumeSettingsFromRegistry(void)
{
    HKEY key = 0;
    DWORD type;
    DWORD size;

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "Software\\Origin Systems\\WC: Kilrathi Saga",
                      0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS) {
        type = REG_DWORD;
        size = sizeof(DAT_00469fc0);
        if (RegQueryValueExA(key, "MusicVolume", 0, &type,
                             (LPBYTE)&DAT_00469fc0, &size) != ERROR_SUCCESS) {
            DAT_00469fc0 = 0x14;
            RegSetValueExA(key, "MusicVolume", 0, REG_DWORD,
                           (const BYTE *)&DAT_00469fc0,
                           sizeof(DAT_00469fc0));
        }
        type = REG_DWORD;
        size = sizeof(DAT_00469fbc);
        if (RegQueryValueExA(key, "SFXVolume", 0, &type,
                             (LPBYTE)&DAT_00469fbc, &size) != ERROR_SUCCESS) {
            DAT_00469fbc = 0x14;
            RegSetValueExA(key, "SFXVolume", 0, REG_DWORD,
                           (const BYTE *)&DAT_00469fbc,
                           sizeof(DAT_00469fbc));
        }
        RegCloseKey(key);
    }
}

/* Function start: 0x42C410 */
void FxDriverShutdownHook(void)
{
}

/* Function start: 0x42C660 */
unsigned int LoadInstallDat(void)
{
    FILE *file;
    unsigned char *records;
    unsigned char *record;
    unsigned char *table;
    long size;
    unsigned short maximumId = 0;

    SystemDebugPrintf("Loading INSTALL.DAT\n");
    file = fopen("install.dat", "rb");
    if (file == 0)
        FatalErrorAndExit("Unable to open INSTALL.DAT");

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    records = (unsigned char *)malloc((unsigned int)size);
    if (records == 0) {
        fclose(file);
        FatalErrorAndExit("Unable to allocate INSTALL.DAT table");
    }
    fread(records, 1, (unsigned int)size, file);
    fclose(file);

    record = records;
    while (record < records + size && record[0] != 0) {
        if (record[15] != 0xff && maximumId < record[15])
            maximumId = record[15];
        record += 16;
    }

    table = (unsigned char *)malloc((maximumId + 2) * 16);
    if (table == 0) {
        free(records);
        FatalErrorAndExit("Unable to allocate packet-name table");
    }
    memset(table, 0, (maximumId + 2) * 16);
    for (record = table; record < table + (maximumId + 1) * 16;
         record += 16)
        record[0] = ' ';

    record = records;
    while (record < records + size && record[0] != 0) {
        if (record[15] != 0xff)
            memcpy(table + record[15] * 16, record, 16);
        record += 16;
    }
    free(records);
    DAT_005a7cf0 = table + 16;
    return 0;
}

/* Function start: 0x42CDA0 */
unsigned short GetJoystickPresentUnused(void)
{
    return 1;
}
