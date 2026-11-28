/* music.c */
void FadeMusic(void);                                                  /* 0x0042E320 */
void SetMusicOn(short enabled);                                         /* 0x0042E330 */
void StopMusic(short unused);                                          /* 0x0042E350 */
void SetMusBreakpt(int first, int second);                             /* 0x0042E380 */
void PaletteFadeHook(void);                                            /* 0x0042E390 */
void FlushSoundEffect(void);                                               /* 0x0042E3A0 */
void FlushSoundEffects(void);                                               /* 0x0042E3C0 */
void SceneLeaveHook();                                                /* 0x0042E3E0 */
void SelectFlightMusicTrack(int track);                              /* 0x0042E3F0 */
int MapMusicTrackToStreamerCommand(int track);                       /* 0x0042E520 */
int ProcessMusicScriptCommand(int track, int command,
                               short enabled);                       /* 0x0042E6F0 */
unsigned int spacetrack(int track, int mode,
                        short enabled);                               /* 0x0042E880 */
void StopMusicUnlessSuppressed(void);                                        /* 0x0042E8B0 */
unsigned short GetMusicMode(void);                                /* 0x0042E8D0 */
void wait_for_end_of_music(void);                                     /* 0x0042E900 */
void WaitForMusicOrInput(void);                                       /* 0x0042E970 */
void new_space_music_changes(short attacker, short victim);          /* 0x0042E9E0 */
int changetrack(void);                                                /* 0x0042EAD0 */
void gametrack(void);                                                 /* 0x0042EB60 */
void servicetrack(void);                                              /* 0x0042ECB0 */
void ResetSoundState(void);                                             /* 0x0042EE80 */
void ResetSoundStateForScene(void);                                               /* 0x0042EEA0 */
void ResetSoundStateForFlight(void);                                               /* 0x0042EEB0 */
void EnableMusicForScene(void);                                               /* 0x0042EEE0 */
unsigned int SoundFxTick(void);                                       /* 0x0042EF00 */
void FlushSoundEffectsAndLog(int soundHandle, short fadeTicks);        /* 0x0042EF10 */
void sound_effect(void);                                                /* 0x0042EF20 */
#ifdef __cplusplus
void PlaySfxWaveFileByNumber(int soundNumber, int sourceObject,
                             int looping);                            /* 0x0042EF30 */
#else
void PlaySfxWaveFileByNumber();                                      /* 0x0042EF30 */
#endif
