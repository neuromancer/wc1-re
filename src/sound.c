/* Wave/sample ownership, playback, and persisted volume settings.
 * Address range 42b160-42b99f. The following launch data establishes
 * the unit boundary; see docs/ORDER.md.
 */
#include "wc1.h"
#ifndef SDL_PORT
#include <malloc.h>
#endif

static ActiveSoundEntry *pActiveSoundHead = 0; /* 0x0046a438 */
static ActiveSoundEntry *pActiveSoundTail = 0; /* 0x0046a43c */
static int bAudioSystemInitialized = 0; /* 0x0046a440 */
static WaveTableEntry *pWaveTableHead = 0; /* 0x0046a444 */
static WaveTableEntry *pWaveTableTail = 0; /* 0x0046a448 */
static char szMusicVolumeValueName[] = "MusicVolume"; /* 0x0046a450 */
static char szSfxVolumeValueName[] = "SFXVolume"; /* 0x0046a460 */

static WaveTableEntry *pSnowStaticWave; /* 0x0047654c */
static IxSound *pSnowStaticSound; /* 0x00476550 */

/* Function start: 0x42B160 */
void InitializeAudioSystem(HWND window)
{
    if (bIxAudioEnabled != 0 && bAudioSystemInitialized == 0) {
        ix_system_configure(3, (void *)1);
        ix_system_configure(0, window);
        ix_system_init();
        ix_system_set_voice_count(0x10);
        bAudioSystemInitialized = 1;
    }
}

/* Function start: 0x42B1B0 */
void ServiceAudioStream(void)
{
    if (bIxAudioEnabled != 0 && bAudioSystemInitialized != 0) {
        ix_system_delete_all_sounds();
        ix_system_delete_all_samples();
        ix_system_shutdown();
        FreeWaveTable();
        bAudioSystemInitialized = 0;
    }
}

/* Function start: 0x42B1F0 */
WaveTableEntry *AllocateWaveTableEntry(void)
{
    if (pWaveTableHead == 0) {
        pWaveTableHead =
            (WaveTableEntry *)malloc(sizeof(WaveTableEntry));
        pWaveTableTail = pWaveTableHead;
    } else {
        pWaveTableTail->next =
            (WaveTableEntry *)malloc(sizeof(WaveTableEntry));
        pWaveTableTail = pWaveTableTail->next;
    }
    pWaveTableTail->next = 0;
    return pWaveTableTail;
}

/* Function start: 0x42B240 */
WaveTableEntry *FindWaveTableEntryByName(const char *name)
{
    WaveTableEntry *entry = pWaveTableHead;

    while (entry != 0) {
        if (strcmp(entry->name, name) == 0)
            return entry;
        entry = entry->next;
    }
    return 0;
}

/* Function start: 0x42B290 */
void RemoveWaveTableEntry(WaveTableEntry *target)
{
    WaveTableEntry *previous = 0;
    WaveTableEntry *entry = pWaveTableHead;

    if (entry != 0) {
        WaveTableEntry *searchTarget = target;

        target = entry;
        while (target != 0 && target != searchTarget) {
            previous = target;
            target = target->next;
        }
        entry = target;
    }
    if (entry != 0) {
        if (previous != 0)
            previous->next = entry->next;
        if (pWaveTableTail == entry && previous != 0)
            pWaveTableTail = previous;
        if (pWaveTableHead == entry)
            pWaveTableHead = entry->next;
        free(entry->name);
        free(entry);
    }
}

/* Function start: 0x42B300 */
void FreeWaveTable(void)
{
    WaveTableEntry *entry = pWaveTableHead;

    while (entry != 0) {
        WaveTableEntry *next = entry->next;

        free(entry->name);
        free(entry);
        entry = next;
    }
    pWaveTableTail = 0;
    pWaveTableHead = 0;
}

/* Function start: 0x42B340 */
ActiveSoundEntry *AllocateActiveSoundEntry(void)
{
    if (pActiveSoundHead == 0) {
        pActiveSoundHead =
            (ActiveSoundEntry *)malloc(sizeof(ActiveSoundEntry));
        pActiveSoundTail = pActiveSoundHead;
    } else {
        pActiveSoundTail->next =
            (ActiveSoundEntry *)malloc(sizeof(ActiveSoundEntry));
        pActiveSoundTail = pActiveSoundTail->next;
    }
    pActiveSoundTail->next = 0;
    return pActiveSoundTail;
}

/* Function start: 0x42B390 */
void RemoveActiveSoundEntry(ActiveSoundEntry *target)
{
    ActiveSoundEntry *entry = pActiveSoundHead;
    ActiveSoundEntry *previous = 0;

    if (entry != 0) {
        ActiveSoundEntry *searchTarget = target;

        target = entry;
        while (target != 0 && target != searchTarget) {
            previous = target;
            target = target->next;
        }
        entry = target;
    }
    if (entry != 0) {
        if (previous != 0)
            previous->next = entry->next;
        if (pActiveSoundTail == entry && previous != 0)
            pActiveSoundTail = previous;
        if (pActiveSoundHead == entry)
            pActiveSoundHead = entry->next;
        free(entry);
    }
}

/* Function start: 0x42B3F0 */
ActiveSoundEntry *FindActiveSoundEntryBySample(IxSample *sample)
{
    ActiveSoundEntry *entry = pActiveSoundHead;

    for (;;) {
        if (entry == 0)
            return 0;
        if (entry->sound->sample == sample)
            return entry;
        entry = entry->next;
    }
}

/* Function start: 0x42B410 */
void ReleaseFinishedSoundEntries(void)
{
    ActiveSoundEntry *entry = pActiveSoundHead;

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

    if (bIxAudioEnabled != 0) {
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
void playWAVE(const char *filename, int looping, int volume)
{
    unsigned char *fileData;
    WaveTableEntry *wave;
    ActiveSoundEntry *active;
    IxSound *sound;
    long fileSize;
    int file;

    if (bIxAudioEnabled != 0) {
        ReleaseFinishedSoundEntries();
        wave = FindWaveTableEntryByName(filename);
        if (wave != 0) {
            if (looping != 0) {
                active = AllocateActiveSoundEntry();
                active->sound = ix_system_new_sound(wave->sample);
                active->sound->ix_system_sound_set_volume(volume);
                ix_sound_start(active->sound);
                return;
            }
            sound = ix_system_new_sound(wave->sample);
            sound->ix_sound_set_delete_on_stop(1);
            sound->ix_system_sound_set_volume(volume);
            ix_sound_start(sound);
            return;
        }

        file = _open(filename, 0x8000);
        if (file == -1) {
            MessageBoxA(0, "playWAVE Unable to open file '%s'",
                        filename, MB_ICONHAND);
            exit(1);
        }
        fileSize = filelength(file);
        fileData = (unsigned char *)malloc((unsigned int)fileSize);
        _read(file, fileData, (unsigned int)fileSize);
        _close(file);

        wave = AllocateWaveTableEntry();
        wave->sample = ix_system_new_sample();
        wave->sample->ix_sample_load_wav(fileData, fileSize);
        if (looping != 0) {
            wave->sample->flags |= 2;
            active = AllocateActiveSoundEntry();
            active->sound = ix_system_new_sound(wave->sample);
            active->sound->ix_system_sound_set_volume(volume);
            ix_sound_start(active->sound);
        } else {
            sound = ix_system_new_sound(wave->sample);
            sound->ix_sound_set_delete_on_stop(1);
            sound->ix_system_sound_set_volume(volume);
            ix_sound_start(sound);
        }

        wave->name = (char *)malloc(strlen(filename) + 1);
        strcpy(wave->name, filename);
        free(fileData);
    }
}

/* Function start: 0x42B640 */
void stop_all_sounds(void)
{
#ifdef SDL_PORT
    SdlStopDosSoundEffects();
#endif
    ix_system_delete_all_sounds();
    ix_system_delete_all_samples();
    FreeWaveTable();
    if (pSnowStaticSound != 0) {
        /* The bulk delete already stops, unlinks, and frees every IxSound.
           The original's following calls therefore use a stale pointer. */
#ifndef SDL_PORT
        ix_sound_stop(pSnowStaticSound);
        ix_sound_release(pSnowStaticSound);
#endif
        pSnowStaticSound = 0;
        pSnowStaticWave = 0;
    }
}

/* Function start: 0x42B680 */
void PlaySnowStaticSound(void)
{
    unsigned char *fileData;
    int file;
    long fileSize;

    if (bIxAudioEnabled != 0) {
        ReleaseFinishedSoundEntries();
        if (pSnowStaticSound == 0) {
            file = _open("sfx22.wav", 0x8000);
            if (file == -1) {
                MessageBoxA(0,
                            "playWAVE Unable to open file 'sfx23",
                            "Notice", MB_ICONHAND);
                exit(1);
            }
            fileSize = filelength(file);
            fileData = (unsigned char *)malloc((unsigned int)fileSize);
            _read(file, fileData, (unsigned int)fileSize);
            _close(file);

            pSnowStaticWave = AllocateWaveTableEntry();
            pSnowStaticWave->sample = ix_system_new_sample();
            pSnowStaticWave->sample->ix_sample_load_wav(
                fileData, fileSize);
            pSnowStaticSound = ix_system_new_sound(
                pSnowStaticWave->sample);
            pSnowStaticSound->ix_system_sound_set_volume(50000);
            ix_sound_start(pSnowStaticSound);

            pSnowStaticWave->name =
                (char *)malloc(strlen("SNOW") + 1);
            strcpy(pSnowStaticWave->name, "SNOW");
            free(fileData);
        } else if (ix_sound_is_playing(
                       pSnowStaticSound) == 0) {
            ix_sound_start(pSnowStaticSound);
        }
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
    SoundDebugPrintf("Setting SFX Volume to %d\n", volume);
}

/* Function start: 0x42B810 */
LONG RegistryQueryValue(HKEY key, LPCSTR name, DWORD type,
                        LPBYTE data, DWORD size)
{
    return RegQueryValueExA(key, name, 0, &type, data, &size);
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
    HKEY key;

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "Software\\Origin Systems\\WC: Kilrathi Saga",
                      0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS) {
        if (RegistryQueryValue(key, szMusicVolumeValueName, REG_DWORD,
                               (LPBYTE)&nMusicVolumeSetting,
                               sizeof(nMusicVolumeSetting)) !=
            ERROR_SUCCESS) {
            nMusicVolumeSetting = 0x14;
            RegistryStoreValue(key, szMusicVolumeValueName, REG_DWORD,
                               (const BYTE *)&nMusicVolumeSetting,
                               sizeof(nMusicVolumeSetting));
        }
        if (RegistryQueryValue(key, szSfxVolumeValueName, REG_DWORD,
                               (LPBYTE)&nSfxVolumeSetting,
                               sizeof(nSfxVolumeSetting)) !=
            ERROR_SUCCESS) {
            nSfxVolumeSetting = 0x14;
            RegistryStoreValue(key, szSfxVolumeValueName, REG_DWORD,
                               (const BYTE *)&nSfxVolumeSetting,
                               sizeof(nSfxVolumeSetting));
        }
    }
    RegCloseKey(key);
}

/* Function start: 0x42B930 */
void SaveVolumeSettingsToRegistry(void)
{
    HKEY key;

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "Software\\Origin Systems\\WC: Kilrathi Saga",
                      0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS) {
        RegistryStoreValue(key, szMusicVolumeValueName, REG_DWORD,
                           (const BYTE *)&nMusicVolumeSetting,
                           sizeof(nMusicVolumeSetting));
        RegistryStoreValue(key, szSfxVolumeValueName, REG_DWORD,
                           (const BYTE *)&nSfxVolumeSetting,
                           sizeof(nSfxVolumeSetting));
    }
    RegCloseKey(key);
}
