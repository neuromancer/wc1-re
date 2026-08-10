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

/* Function start: 0x42C800 */
void show_damage_disp(void)
{
    signed char component;
    char message[40];

    g_nDamageDisplayState_005a77e0 = 0;
    g_cDamagedComponentCount_005a77de = 0;
    component = 0;
    do {
        if (g_acPlayerComponentDamage_0059bff0[component] >= 1)
            g_cDamagedComponentCount_005a77de++;
        component++;
    } while (component < 9);

    set_new_vdu(0);
    DrawTextAt(&DAT_005a74f0, DAT_005a6b80.left, DAT_005a6b80.top,
               (char *)g_szDamageReport_0046a878, 2);
    DrawViewportLine(&DAT_005a6b80,
                     (short)(DAT_005a6b80.left + 2),
                     (short)(DAT_005a6b80.top + 6),
                     (short)(DAT_005a6b80.right - 2),
                     (short)(DAT_005a6b80.top + 6), DAT_004699b4);
    if (g_cDamagedComponentCount_005a77de == 0) {
        DrawTextAt(&DAT_005a74f0, DAT_005a6b80.left,
                   (short)(DAT_005a6b80.top + 20),
                   (char *)g_szNoInternalDamage_0046a888, 2);
        return;
    }

    component = g_cCockpitView_0059dab0;
    g_nWeaponDisplayOriginX_005a7788 =
        (short)(g_aWeaponDisplayOrigins_004684c0[component].x +
                DAT_005a6b80.left);
    g_nWeaponDisplayOriginY_005a778a =
        (short)(g_aWeaponDisplayOrigins_004684c0[component].y +
                DAT_005a6b80.top);
    DrawSpriteDefault(&DAT_005a6b80, g_nWeaponDisplayOriginX_005a7788,
                      g_nWeaponDisplayOriginY_005a778a,
                      g_pCockpitWeaponShape_005a7564, 0);
    sprintf(message, g_szDamagedUnitCountFormat_0046a89c,
            (int)g_cDamagedComponentCount_005a77de,
            (int)(g_cDamagedComponentCount_005a77de == 1 ? ' ' : 's'));
    ShowComponentHitHudMessage(message, DAT_004699b4, -1);
}

/* Function start: 0x42C970 */
void UpdateDamageDisplay(void)
{
    signed char componentCount;
    signed char component;
    signed char attempts;
    signed char damage;

    componentCount = 0;
    component = 0;
    do {
        if (g_acPlayerComponentDamage_0059bff0[component] >= 1)
            componentCount++;
        component++;
    } while (component < 9);

    if ((short)componentCount !=
        (short)g_cDamagedComponentCount_005a77de) {
        g_cDamagedComponentCount_005a77de = componentCount;
        ClearMessageSlot(0);
        return;
    }
    g_cDamagedComponentCount_005a77de = componentCount;
    if (componentCount == 0)
        return;

    if (DAT_0046a008 == 0) {
        g_nDamageDisplayTicks_005a7786--;
        if (g_nDamageDisplayTicks_005a7786 > 0)
            return;
        if (g_nDamageDisplayPhase_005a77e4 == 1) {
            component = g_cDamageDisplayComponent_005a77dc;
            attempts = 0;
            g_nDamageDisplayTicks_005a7786 = 50;
            do {
                component++;
                if (component >= 9)
                    component = 0;
                damage = g_acPlayerComponentDamage_0059bff0[component];
                g_nDisplayedComponentDamage_00476554 = (int)damage;
                if (g_nDisplayedComponentDamage_00476554 >= 1) {
                    g_cDamageDisplayComponent_005a77dc = component;
                    break;
                }
                attempts++;
            } while (attempts < 9);

            sprintf(
                g_szDamageStatusText_005a7790,
                g_szDamageStatusFormat_0046a8b0,
                g_apszComponentNames_0046a778[
                    g_cDamageDisplayComponent_005a77dc],
                g_apszDamageSeverityNames_0046a7b0[(int)damage]);
            DrawTextAt(&DAT_005a74f0, (short)(DAT_005a6b80.left + 1),
                       (short)(DAT_005a6b80.top + 7),
                       g_szDamageStatusText_005a7790, 2);
            g_cDamageDisplayFrame_005a77dd =
                (signed char)g_abDamageDisplayFrames_0046a7a0[
                    g_cDamageDisplayComponent_005a77dc];
            g_nDamageSpriteX_005a77d8 =
                g_aDamageDisplayPositions_0046a750[
                    g_cDamageDisplayComponent_005a77dc].x;
            g_nDamageSpriteY_005a77da =
                g_aDamageDisplayPositions_0046a750[
                    g_cDamageDisplayComponent_005a77dc].y;
            g_nDamageSpriteX_005a77d8 =
                (short)(g_nDamageSpriteX_005a77d8 +
                        g_nWeaponDisplayOriginX_005a7788);
            g_nDamageSpriteY_005a77da =
                (short)(g_nDamageSpriteY_005a77da +
                        g_nWeaponDisplayOriginY_005a778a);
            CaptureSpriteBackground(
                &DAT_005a6b80, g_pDamageDisplayBackground_0046a748,
                g_nDamageSpriteX_005a77d8, g_nDamageSpriteY_005a77da,
                g_pCockpitWeaponShape_005a7564,
                (short)g_cDamageDisplayFrame_005a77dd);
            DrawViewportLine(
                &DAT_005a6b80, (short)(DAT_005a6b80.left + 36),
                (short)(DAT_005a6b80.top + 22),
                g_nDamageSpriteX_005a77d8, g_nDamageSpriteY_005a77da,
                0xa9);
            DrawSpriteDefault(
                &DAT_005a6b80, g_nDamageSpriteX_005a77d8,
                g_nDamageSpriteY_005a77da,
                g_pCockpitWeaponShape_005a7564,
                (short)g_cDamageDisplayFrame_005a77dd);
        } else {
            RestoreSpriteBackground(
                &DAT_005a6b80, g_pDamageDisplayBackground_0046a748,
                g_nDamageSpriteX_005a77d8, g_nDamageSpriteY_005a77da,
                g_pCockpitWeaponShape_005a7564,
                (short)g_cDamageDisplayFrame_005a77dd);
            DAT_005a74f0.colour = DAT_0046999c;
            DrawTextAt(&DAT_005a74f0, (short)(DAT_005a6b80.left + 1),
                       (short)(DAT_005a6b80.top + 7),
                       g_szDamageStatusText_005a7790, 2);
            DAT_005a74f0.colour = DAT_004699b4;
            DrawViewportLine(
                &DAT_005a6b80, (short)(DAT_005a6b80.left + 36),
                (short)(DAT_005a6b80.top + 22),
                g_nDamageSpriteX_005a77d8, g_nDamageSpriteY_005a77da,
                DAT_0046999c);
            g_nDamageDisplayTicks_005a7786 = 2;
        }
        g_nDamageDisplayPhase_005a77e4 =
            g_nDamageDisplayPhase_005a77e4 == 0;
        return;
    }

    g_nDamageDisplayTicks_005a7786--;
    if (g_nDamageDisplayTicks_005a7786 <= 0) {
        component = g_cDamageDisplayComponent_005a77dc;
        attempts = 0;
        g_nDamageDisplayTicks_005a7786 = 50;
        for (;;) {
            component++;
            if (component >= 9)
                component = 0;
            g_nDisplayedComponentDamage_00476554 =
                (int)g_acPlayerComponentDamage_0059bff0[component];
            if (g_nDisplayedComponentDamage_00476554 >= 1) {
                g_cDamageDisplayComponent_005a77dc = component;
                return;
            }
            attempts++;
            if (attempts >= 9)
                return;
        }
    }

    sprintf(
        g_szDamageStatusText_005a7790,
        g_szDamageStatusFormatHighRes_0046a8c0,
        g_apszComponentNames_0046a778[g_cDamageDisplayComponent_005a77dc],
        g_apszDamageSeverityNames_0046a7b0[
            g_nDisplayedComponentDamage_00476554]);
    DrawTextAt(&DAT_005a74f0, (short)(DAT_005a6b80.left + 1),
               (short)(DAT_005a6b80.top + 7),
               g_szDamageStatusText_005a7790, 2);
    g_cDamageDisplayFrame_005a77dd =
        (signed char)g_abDamageDisplayFrames_0046a7a0[
            g_cDamageDisplayComponent_005a77dc];
    g_nDamageSpriteX_005a77d8 =
        g_aDamageDisplayPositions_0046a750[
            g_cDamageDisplayComponent_005a77dc].x;
    g_nDamageSpriteY_005a77da =
        g_aDamageDisplayPositions_0046a750[
            g_cDamageDisplayComponent_005a77dc].y;
    g_nDamageSpriteX_005a77d8 =
        (short)(g_nDamageSpriteX_005a77d8 +
                g_nWeaponDisplayOriginX_005a7788);
    g_nDamageSpriteY_005a77da =
        (short)(g_nDamageSpriteY_005a77da +
                g_nWeaponDisplayOriginY_005a778a);
    CaptureSpriteBackground(
        &DAT_005a6b80, g_pDamageDisplayBackground_0046a748,
        g_nDamageSpriteX_005a77d8, g_nDamageSpriteY_005a77da,
        g_pCockpitWeaponShape_005a7564,
        (short)g_cDamageDisplayFrame_005a77dd);
    DrawViewportLine(&DAT_005a6b80,
                     (short)(DAT_005a6b80.left + 36),
                     (short)(DAT_005a6b80.top + 22),
                     g_nDamageSpriteX_005a77d8,
                     g_nDamageSpriteY_005a77da, 0xa9);
    DrawSpriteDefault(&DAT_005a6b80, g_nDamageSpriteX_005a77d8,
                      g_nDamageSpriteY_005a77da,
                      g_pCockpitWeaponShape_005a7564,
                      (short)g_cDamageDisplayFrame_005a77dd);
}

/* Function start: 0x42CDA0 */
unsigned short GetJoystickPresentUnused(void)
{
    return 1;
}
