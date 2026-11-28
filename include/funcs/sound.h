/* sound.c */
void InitializeAudioSystem(HWND window);                              /* 0x0042B160 */
void ServiceAudioStream(void);                                        /* 0x0042B1B0 */
WaveTableEntry *AllocateWaveTableEntry(void);                         /* 0x0042B1F0 */
WaveTableEntry *FindWaveTableEntryByName(const char *name);           /* 0x0042B240 */
void RemoveWaveTableEntry(WaveTableEntry *entry);                     /* 0x0042B290 */
void FreeWaveTable(void);                                           /* 0x0042B300 */
ActiveSoundEntry *AllocateActiveSoundEntry(void);                     /* 0x0042B340 */
void RemoveActiveSoundEntry(ActiveSoundEntry *entry);                 /* 0x0042B390 */
ActiveSoundEntry *FindActiveSoundEntryBySample(IxSample *sample);    /* 0x0042B3F0 */
void ReleaseFinishedSoundEntries(void);                              /* 0x0042B410 */
void StopSoundsUsingWave(const char *name);                          /* 0x0042B450 */
void playWAVE(const char *filename, int looping, int volume);         /* 0x0042B4A0 */
void stop_all_sounds(void);                                         /* 0x0042B640 */
void PlaySnowStaticSound(void);                                     /* 0x0042B680 */
void ServiceSoundSystem(void);                                         /* 0x0042B7D0 */
void SetSoundEffectsVolume(int volume);                               /* 0x0042B7E0 */
LONG RegistryQueryValue(HKEY key, LPCSTR name, DWORD type,
                        LPBYTE data, DWORD size);                     /* 0x0042B810 */
void RegistryStoreValue(HKEY key, LPCSTR name, DWORD type,
                        const BYTE *data, DWORD size);                  /* 0x0042B840 */
void LoadVolumeSettingsFromRegistry(void);                            /* 0x0042B870 */
void SaveVolumeSettingsToRegistry(void);                               /* 0x0042B930 */
