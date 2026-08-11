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
void ServiceSoundSystem(void)
{
    ix_system_service_sounds();
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

/* Function start: 0x42B9A0 */
void DrawLaunchDoorFrame(short distance)
{
    short bounds[4];
    short scale;

    if (distance > 10) {
        scale = (short)(0x1a00L / distance);
        GetTransformedShapeBounds(
            &DAT_005a7510,
            (short)((short)g_nScreenWidth_0046daa4 >> 1),
            (short)((short)g_nScreenHeight_0046daa8 >> 1),
            g_pLaunchDoorShape_005a77e8, 1, 0, scale, 0, bounds);
        DrawSpriteScaled(
            &DAT_005a7510, (short)(bounds[0] - 1),
            (short)((short)g_nScreenHeight_0046daa8 >> 1),
            g_pLaunchDoorShape_005a77e8, 0, 0, scale, 0);
        DrawSpriteScaled(
            &DAT_005a7510,
            (short)((short)g_nScreenWidth_0046daa4 >> 1),
            (short)((short)g_nScreenHeight_0046daa8 >> 1),
            g_pLaunchDoorShape_005a77e8, 1, 0, scale, 0);
        DrawSpriteScaled(
            &DAT_005a7510, bounds[2],
            (short)((short)g_nScreenHeight_0046daa8 >> 1),
            g_pLaunchDoorShape_005a77e8, 2, 0, scale, 0);
    }
}

/* Function start: 0x42BA90 */
void LaunchPlayerShip(void)
{
    short doorDistances[4];
    signed char distanceStep;
    signed char frame;
    signed char door;

    doorDistances[0] = 50;
    doorDistances[1] = 40;
    doorDistances[2] = 30;
    doorDistances[3] = 20;
    distanceStep = 1;

    spacetrack(changetrack(), 1, 0);
    if (DAT_0059ab58 == 0) {
        g_pLaunchDoorShape_005a77e8 = FetchDiskPacketRetrying(1, 7, 0);
        g_nCannedSceneMode_00469fac = 1;
        force_view(0, 0);
        PlaySfxWaveFileByNumber(20, -1, 0);
        DAT_00469fb4 = 1;
        DAT_0059ab58 = 0;
        frame = 0;
        do {
            PumpWindowMessages();
            if (RefreshCockpitStatus() != 0) {
                door = 0;
                do {
                    DrawLaunchDoorFrame(doorDistances[door]);
                    doorDistances[door] =
                        (short)(doorDistances[door] - distanceStep);
                    door++;
                } while (door < 4);
                dump_buffer_to_screen();
                update_cockpit();
            }
            DIBslam();
            DIBslamReal();
            if (DAT_0059ab58 == 1)
                break;
            if (frame % 5 == 0)
                distanceStep++;
            frame++;
        } while (frame < 25);

        if (DAT_0059ab58 != 0) {
            StopMusicUnlessSuppressed();
            spacetrack(changetrack(), 1, 0);
        }
        ReleasePacketHandle((int)g_pLaunchDoorShape_005a77e8);
    } else {
        force_view(0, 0);
    }

    DIBslam();
    DIBslamReal();
    clear_view_buffer();
    g_nCannedSceneMode_00469fac = 0;
    ResetSoundState();
    DAT_0059ab58 = 0;
}

/* Function start: 0x42C410 */
void FxDriverShutdownHook(void)
{
}

/* Function start: 0x42C580 */
short LoadWingCmdrCfgFile(short argc, char **argv)
{
    FILE *file;
    short argumentCount;
    char *destination;
    short argumentIndex;

    argumentIndex = 1;
    argumentCount = 0;
    destination = g_szTextScratchBuffer_00598b00;
    file = fopen("WINGCMDR.CFG", "rt");
    while (file != 0 &&
           (short)fscanf(file, "%s", destination) != -1) {
        g_pStartupArguments_005a7b10[argumentCount++] =
            destination;
        destination = strchr(destination, 0) + 1;
    }
    if (file != 0)
        fclose(file);

    while (argc-- != 0) {
        strcpy(destination, argv[argumentIndex]);
        g_pStartupArguments_005a7b10[argumentCount++] = destination;
        argumentIndex++;
        destination = strchr(destination, 0) + 1;
    }
    return (short)(argumentCount - 1);
}

/* Function start: 0x42C660 */
unsigned int LoadInstallDat(void)
{
    DiskFileRecord *records;
    DiskFileRecord *record;
    DiskFileRecord *entry;
    unsigned int size;
    short file;
    short maximumId;

    maximumId = 0;
    SystemDebugPrintf("Loading INSTALL.DAT\n");
    file = OpenDataFileOrDie("install.dat");
    if (file == -1) {
        SystemDebugPrintf("Unable to open INSTALL.DAT\n");
        SystemDebugPrintf(
            "[SYSTEM]: Exiting Prematurely (LoadInstallData)\n");
        ClearDebugPauseFlags();
        PumpMessagesDuringWait();
        exit(0);
    }
    size = (unsigned int)_filelength(file);
    records = (DiskFileRecord *)AllocateTaggedMemory(size, 0);
    if (records == 0) {
        SystemDebugPrintf("Unable to load INSTALL.DAT\n");
        SystemDebugPrintf(
            "[SYSTEM]: Exiting Prematurely (LoadInstallData)\n");
        ClearDebugPauseFlags();
        PumpMessagesDuringWait();
        exit(0);
    }
    ReadDataFileAtOffset(file, 0, size, records);
    CloseDataFile(file);

    record = records;
    while (record->name[0] != 0) {
        if (maximumId < record->logicalFile &&
            record->logicalFile != 0xff)
            maximumId = record->logicalFile;
        record++;
    }
    maximumId++;

    g_pDiskFileRecords_005a7cf0 =
        (DiskFileRecord *)AllocateTaggedMemory(0x4b0, 0);
    memset(g_pDiskFileRecords_005a7cf0, 0,
           (maximumId + 1) * sizeof(DiskFileRecord));
    if (g_pDiskFileRecords_005a7cf0 == 0) {
        SystemDebugPrintf("Unable to copy INSTALL.DAT\n");
        SystemDebugPrintf(
            "[SYSTEM]: Exiting Prematurely (LoadInstallData)\n");
        ClearDebugPauseFlags();
        PumpMessagesDuringWait();
        exit(0);
    }

    entry = g_pDiskFileRecords_005a7cf0;
    while (maximumId > 0) {
        entry->name[0] = ' ';
        entry++;
        maximumId--;
    }

    record = records;
    while (record->name[0] != 0) {
        if (record->logicalFile != 0xff)
            g_pDiskFileRecords_005a7cf0[record->logicalFile] = *record;
        record++;
    }
    ReleasePacketHandle((int)records);
    g_pDiskFileRecords_005a7cf0++;
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
        InvalidateVduMode(0);
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
