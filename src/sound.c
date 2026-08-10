/*
 *  Wave playback, volume settings and INSTALL.DAT.
 *
 *  Address range 0x42b400-0x42cfff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: playWAVE/PlaySfxWaveByIndex/LoadInstallDat; string band 0x46A46C-0x46A710.
 */
#include "wc1.h"

/* Function start: 0x42B410 */
void ReleaseFinishedSoundEntries(void)
{
    ActiveSoundEntry *entry = g_pActiveSoundHead_0046a438;

    while (entry != 0) {
        ActiveSoundEntry *next = entry->next;

        if (ix_sound_is_playing(entry->sound) == 0) {
            ix_system_delete_sound(entry->sound);
            RemoveActiveSoundEntry(entry);
        }
        entry = next;
    }
}

/* Function start: 0x42B450 */
void StopSoundsUsingWave(const char *name)
{
    WaveTableEntry *wave;
    ActiveSoundEntry *entry;

    if (DAT_00465058 != 0) {
        wave = FindWaveTableEntryByName(name);
        if (wave != 0) {
            for (;;) {
                entry = FindActiveSoundEntryBySample(wave->sample);
                if (entry == 0)
                    break;
                ix_system_delete_sound(entry->sound);
                RemoveActiveSoundEntry(entry);
            }
        }
    }
}

/* Function start: 0x42B4A0 */
void playWAVE(unsigned char *filename, int looping, int volume)
{
    WaveTableEntry *wave;
    unsigned char *fileData;
    long fileSize;
    int file;
    DWORD flags;

    if (DAT_00465058 == 0)
        return;
    ReleaseFinishedSoundEntries();
    wave = FindWaveTableEntryByName((const char *)filename);
    if (wave != 0) {
        flags = SND_ASYNC | SND_FILENAME | SND_NODEFAULT;
        if (looping != 0)
            flags |= SND_LOOP;
        if (volume > 0)
            PlaySoundA((const char *)filename, 0, flags);
        return;
    }

    file = _open((const char *)filename, 0x8000);
    if (file == -1) {
        MessageBoxA(0, g_szPlayWaveOpenError_0046a46c,
                    (const char *)filename, MB_ICONHAND);
        _exit(1);
    }
    fileSize = _filelength(file);
    fileData = (unsigned char *)malloc((unsigned int)fileSize);
    _read(file, fileData, (unsigned int)fileSize);
    _close(file);

    wave = AllocateWaveTableEntry();
    wave->sample = 0;
    flags = SND_ASYNC | SND_FILENAME | SND_NODEFAULT;
    if (looping != 0)
        flags |= SND_LOOP;
    if (volume > 0)
        PlaySoundA((const char *)filename, 0, flags);

    wave->name = (char *)malloc(strlen((const char *)filename) + 1);
    strcpy(wave->name, (const char *)filename);
    free(fileData);
}

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

/* Function start: 0x42B840 */
void RegistryStoreValue(HKEY key, LPCSTR name, DWORD type,
                        const BYTE *data, DWORD size)
{
    RegSetValueExA(key, name, 0, type, data, size);
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
        size = sizeof(g_nMusicVolumeSetting_00469fc0);
        if (RegQueryValueExA(key, "MusicVolume", 0, &type,
                             (LPBYTE)&g_nMusicVolumeSetting_00469fc0,
                             &size) != ERROR_SUCCESS) {
            g_nMusicVolumeSetting_00469fc0 = 0x14;
            RegSetValueExA(key, "MusicVolume", 0, REG_DWORD,
                           (const BYTE *)&g_nMusicVolumeSetting_00469fc0,
                           sizeof(g_nMusicVolumeSetting_00469fc0));
        }
        type = REG_DWORD;
        size = sizeof(g_nSfxVolumeSetting_00469fbc);
        if (RegQueryValueExA(key, "SFXVolume", 0, &type,
                             (LPBYTE)&g_nSfxVolumeSetting_00469fbc,
                             &size) != ERROR_SUCCESS) {
            g_nSfxVolumeSetting_00469fbc = 0x14;
            RegSetValueExA(key, "SFXVolume", 0, REG_DWORD,
                           (const BYTE *)&g_nSfxVolumeSetting_00469fbc,
                           sizeof(g_nSfxVolumeSetting_00469fbc));
        }
        RegCloseKey(key);
    }
}

/* Function start: 0x42B930 */
void SaveVolumeSettingsToRegistry(void)
{
    HKEY key;

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "Software\\Origin Systems\\WC: Kilrathi Saga",
                      0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS) {
        RegistryStoreValue(key, "MusicVolume", REG_DWORD,
                           (const BYTE *)&g_nMusicVolumeSetting_00469fc0,
                           sizeof(g_nMusicVolumeSetting_00469fc0));
        RegistryStoreValue(key, "SFXVolume", REG_DWORD,
                           (const BYTE *)&g_nSfxVolumeSetting_00469fbc,
                           sizeof(g_nSfxVolumeSetting_00469fbc));
    }
    RegCloseKey(key);
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
