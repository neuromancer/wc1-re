/*
 *  Music state machine and the streaming music script.
 *
 *  Address range 0x42d000-0x42efff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: PROVEN by the names the routines print: StopMusic, FadeMusic, SetMusicOn, ....
 */
#include "wc1.h"

/* Function start: 0x42D270 */
unsigned int GetVictoryScreenId(void)
{
    short i = 0;

    do {
        DAT_005a6900[i * 3] = 0xffff;
        i = i + 1;
    } while (i < 0x1e);
    return 0;
}

/* Function start: 0x42D870 */
void CloseDataFileByHandle(unsigned short *p)
{
    CloseDataFile(*p);
}

/* Function start: 0x42DB70 */
short GetTargetColourIndex(void)
{
    short v = (short)DAT_0046b168;

    if ((short)DAT_0046b168 == -1)
        v = 0x13;
    return v;
}

/* Function start: 0x42E020 */
void LogDisplayMode(void)
{
    SystemDebugPrintf(0);
    ClearDebugPauseFlags();
    PumpMessagesDuringWait();
    exit(1);
}

/* Function start: 0x42E320 */
void FadeMusic(void)
{
    SoundDebugPrintf("FadeMusic");
}

/* Function start: 0x42E330 */
void SetMusicOn(void)
{
    SoundDebugPrintf("SetMusicOn %d");
}

/* Function start: 0x42E350 */
void StopMusic(void)
{
    SoundDebugPrintf("StopMusic");
    DAT_0046aa14 = 0xffffffff;
    Streamer_stop();
    SoundDebugPrintf("");
}

/* Function start: 0x42E380 */
void SetMusBreakpt(void)
{
    SoundDebugPrintf("SetMusBreakpt");
}

/* Function start: 0x42E390 */
void PaletteFadeHook(void)
{
}

/* Function start: 0x42E3A0 */
void FlushSoundEffect(void)
{
    StopAllSounds();
    SoundDebugPrintf("FlushSoundEffect");
}

/* Function start: 0x42E3C0 */
void FlushSoundEffects(void)
{
    StopAllSounds();
    SoundDebugPrintf("FlushSoundEffects");
}

/* Function start: 0x42E3E0 */
void SceneLeaveHook(void)
{
}

/* Function start: 0x42E8B0 */
void StopMusicUnlessSuppressed(void)
{
    if (DAT_0046a9f8 != 0 && DAT_0046a9f8 != 3)
        StopMusic();
}

/* Function start: 0x42E8D0 */
unsigned short GetMusicMode(void)
{
    if (DAT_0046a9f8 != 0 && DAT_0046a9f8 != 3 && DAT_0046aa04 != 0)
        return 1;
    return 0;
}

/* Function start: 0x42EE80 */
void ResetSoundState(void)
{
    FlushSoundEffects();
    DAT_005a7cec = 0;
    DAT_005a7ec0 = 0;
}

/* Function start: 0x42EEA0 */
void ResetSoundStateForScene(void)
{
    ResetSoundState();
    DAT_0046aa34 = 0;
}

/* Function start: 0x42EEB0 */
void ResetSoundStateForFlight(void)
{
    ResetSoundState();
    DAT_0046aa34 = 1;
}

/* Function start: 0x42EEE0 */
void EnableMusicForScene(void)
{
    DAT_0046aa30 = 1;
    SetMusicOn();
}

/* Function start: 0x42EF00 */
unsigned int SoundFxTick(void)
{
    WriteDebugString("soundFX");
    return 0;
}

/* Function start: 0x42EF10 */
void FlushSoundEffectsAndLog(void)
{
    StopAllSounds();
    SoundDebugPrintf("FlushSoundEffects");
}

/* Function start: 0x42EF20 */
void sound_effect(void)
{
    WriteDebugString("sound_effect");
}
