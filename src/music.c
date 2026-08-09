/*
 *  Music state machine and the streaming music script.
 *
 *  Address range 0x42d000-0x42efff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: PROVEN by the names the routines print: StopMusic, FadeMusic, SetMusicOn, ....
 */
#include "wc1.h"

/* Function start: 0x42CDB0 */
unsigned int parse_view_script(void)
{
    const short *command;
    FixedVector direction;
    FixedVector velocity;
    short obj;

    if (*g_pViewScript_005a6b58 == 13)
        return 0;
    while (*g_pViewScript_005a6b58 != 14) {
        command = g_pViewScript_005a6b58++;
        switch (*command) {
        case -1:
            g_bScriptedView_0046a8d4 = 0;
            g_nScriptedViewObject_0046a8d0 = -1;
            return 0;
        case 0:
            g_aShipPosition_0059c490[61].x =
                (int)g_pViewScript_005a6b58[0] << 8;
            g_aShipPosition_0059c490[61].y =
                (int)g_pViewScript_005a6b58[1] << 8;
            g_aShipPosition_0059c490[61].z =
                (int)g_pViewScript_005a6b58[2] << 8;
            g_pViewScript_005a6b58 += 3;
            break;
        case 1:
            alter_yaw(g_pViewScript_005a6b58[0], 61);
            alter_pitch(g_pViewScript_005a6b58[1], 61);
            alter_roll(g_pViewScript_005a6b58[2], 61);
            g_pViewScript_005a6b58 += 3;
            break;
        case 2:
            ScaleFixedVector(&g_aShipForwardVector_0059bce0[61],
                             (int)*g_pViewScript_005a6b58 << 8,
                             &g_aShipVelocity_0059c010[61]);
            g_pViewScript_005a6b58++;
            break;
        case 3:
            force_view(*g_pViewScript_005a6b58++,
                       g_nScriptedViewObject_0046a8d0);
            break;
        case 4:
            g_nEyePitchGoal_0059d61c =
                (short)-g_pViewScript_005a6b58[0];
            g_nEyePitchRate_0046c004 = g_pViewScript_005a6b58[1];
            g_pViewScript_005a6b58 += 2;
            break;
        case 5:
            g_nEyePitchGoal_0059d61c = g_pViewScript_005a6b58[0];
            g_nEyePitchRate_0046c004 = g_pViewScript_005a6b58[1];
            g_pViewScript_005a6b58 += 2;
            break;
        case 6:
            g_nEyeYawGoal_0059c944 = g_pViewScript_005a6b58[0];
            g_nEyeYawRate_0046c008 = g_pViewScript_005a6b58[1];
            g_pViewScript_005a6b58 += 2;
            break;
        case 7:
            g_nEyeYawGoal_0059c944 =
                (short)-g_pViewScript_005a6b58[0];
            g_nEyeYawRate_0046c008 = g_pViewScript_005a6b58[1];
            g_pViewScript_005a6b58 += 2;
            break;
        case 8:
            g_nEyeRollGoal_0059c8f0 = g_pViewScript_005a6b58[0];
            g_nEyeRollRate_0046c00c = g_pViewScript_005a6b58[1];
            g_pViewScript_005a6b58 += 2;
            break;
        case 9:
            copy_frame(61, 63);
            alter_yaw(g_pViewScript_005a6b58[0], 63);
            alter_pitch(g_pViewScript_005a6b58[1], 63);
            alter_roll(g_pViewScript_005a6b58[2], 63);
            ScaleFixedVector(&g_aShipForwardVector_0059bce0[63],
                             (int)g_pViewScript_005a6b58[3] << 8,
                             &velocity);
            AddFixedVectors(&g_aShipVelocity_0059c010[61], &velocity,
                            &g_aShipVelocity_0059c010[61]);
            g_pViewScript_005a6b58 += 4;
            break;
        case 10:
            if (g_nScriptedViewObject_0046a8d0 >= 0)
                g_aShipVelocity_0059c010[61] =
                    g_aShipVelocity_0059c010[g_nScriptedViewObject_0046a8d0];
            break;
        case 11:
            if (g_nScriptedViewObject_0046a8d0 >= 0)
                copy_frame(g_nScriptedViewObject_0046a8d0, 61);
            break;
        case 12:
            if (g_nScriptedViewObject_0046a8d0 >= 0)
                g_aShipPosition_0059c490[61] =
                    g_aShipPosition_0059c490[g_nScriptedViewObject_0046a8d0];
            break;
        case 15:
            if (g_nScriptedViewObject_0046a8d0 >= 0) {
                ComputeVectorDelta(&g_aShipPosition_0059c490[61],
                    &g_aShipPosition_0059c490[g_nScriptedViewObject_0046a8d0],
                    &direction);
                if (NormalizeFixedVector(&direction) != 0) {
                    g_aShipForwardVector_0059bce0[61] = direction;
                    fix_objects_ijk(61);
                }
            }
            break;
        case 16:
            obj = 0;
            while (obj < 10) {
                if (g_asShipMissionIndex_0059c830[obj] ==
                    *g_pViewScript_005a6b58)
                    break;
                obj++;
            }
            if (obj < 10)
                g_nScriptedViewObject_0046a8d0 = obj;
            g_pViewScript_005a6b58++;
            break;
        }
        if (*g_pViewScript_005a6b58 == 13)
            return 0;
    }
    g_asObjectCounter_0059c330[61] = g_pViewScript_005a6b58[1];
    return 0;
}

/* Function start: 0x42D1C0 */
unsigned int update_scripted_view(void)
{
    short counter;

    counter = g_asObjectCounter_0059c330[61];
    if (*g_pViewScript_005a6b58 == 13) {
        if (g_nEyePitchGoal_0059d61c == 0 &&
            g_nEyeYawGoal_0059c944 == 0 &&
            g_nEyeRollGoal_0059c8f0 == 0) {
            g_pViewScript_005a6b58++;
            parse_view_script();
        }
    } else if (*g_pViewScript_005a6b58 == 14) {
        g_asObjectCounter_0059c330[61]--;
        if (counter < 1) {
            g_pViewScript_005a6b58 += 2;
            parse_view_script();
        }
    }
    return 0;
}

/* Function start: 0x42D230 */
void initialize_scripted_view(const short *script)
{
    g_bScriptedView_0046a8d4 = 1;
    g_nScriptedViewObject_0046a8d0 = -1;
    zero_vector(&g_aShipVelocity_0059c010[61]);
    init_ijk(61);
    g_pViewScript_005a6b58 = script;
    parse_view_script();
    g_asObjectCollisionRadius_0059d710[61] = 100;
}

/* Function start: 0x42D270 */
unsigned int GetVictoryScreenId(void)
{
    int empty = -1;
    short i = 0;

    do {
        DAT_005a6900[i * 3] = empty;
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
void LogDisplayMode(const char *mode)
{
    SystemDebugPrintf("display_mode == '%s'\n", mode);
    ClearDebugPauseFlags();
    PumpMessagesDuringWait();
    exit(1);
}

/* Function start: 0x42E090 */
unsigned short __stdcall AllocateViewport(Viewport *viewport,
                                          short clearColour, short flags)
{
    unsigned short *rowOffsets;
    unsigned short top;
    short left;
    unsigned short width;
    unsigned short height;
    unsigned short row;
    short offset;

    top = (unsigned short)viewport->top;
    height = (unsigned short)(viewport->bottom - top + 1);
    left = viewport->left;
    width = (unsigned short)(viewport->right - left + 1);
    g_nAllocateViewportCalls_005a68ec++;
    if (DAT_0046b168 != 0x13)
        LogDisplayMode("not MCGA");
    viewport->allocation = (unsigned char *)AllocateTaggedMemory(
        (unsigned int)width * height, (unsigned short)(flags + 2));
    g_apViewportAllocations_005a7f10[
        g_nViewportAllocationCount_005a7f0c++] = viewport->allocation;
    if (viewport->allocation == 0)
        return 0;
    viewport->pixels = viewport->allocation;

    rowOffsets = (unsigned short *)AllocateTaggedMemory(
        (top + (unsigned int)height) * sizeof(unsigned short) + 4,
        0);
    viewport->rowOffsets = rowOffsets;
    if (rowOffsets == 0) {
        if (DAT_0046b168 != 0x13)
            ReleasePacketHandle((int)viewport->allocation);
        return 0;
    }

    row = 0;
    offset = 0;
    do {
        rowOffsets[top + row] = (unsigned short)(offset - left);
        row++;
        offset = (short)(offset + width);
    } while ((unsigned int)row < (unsigned int)height + 2);
    if (clearColour != -1)
        ClearViewport(viewport, (unsigned char)clearColour);
    return 1;
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

/* Function start: 0x42E880 */
unsigned int StartMusicTrack(short track, short mode)
{
    (void)mode;
    if (DAT_0046a9f8 != 0 && DAT_0046a9f8 != 3)
        DAT_0046aa14 = (unsigned short)track;
    return 1;
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
