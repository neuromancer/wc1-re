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
                if (g_nShipMissionIndices_0059c830[obj] ==
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
    int command;

    command = *g_pViewScript_005a6b58;
    switch (command) {
    case 13:
        if ((g_nEyeYawGoal_0059c944 == g_nEyePitchGoal_0059d61c) !=
            g_nEyeRollGoal_0059c8f0) {
            g_pViewScript_005a6b58++;
            parse_view_script();
        }
        break;
    case 14:
        counter = g_asObjectCounter_0059c330[61];
        g_asObjectCounter_0059c330[61]--;
        if (counter < 1) {
            g_pViewScript_005a6b58 += 2;
            parse_view_script();
        }
        break;
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

/* Function start: 0x42DB90 */
void show_target_disp(void)
{
    short target;
    const char *name;

    set_new_vdu(1);
    DrawTextAt(&DAT_005a7700, DAT_005a7530.left, DAT_005a7530.top,
               "TARGET DISPLAY", 2);
    DrawFormattedText(g_nTargetLockMode_0046c078 == 0
                          ? "AUTO TARGETING\n"
                          : "LOCKED TARGET\n");
    target = (short)g_acShipTarget_0059ce60[0];
    if (target < 0 || target >= WC1_SPACE_OBJECT_COUNT ||
        g_aeObjectClass_0059d100[target] < OBJECT_CLASS_SHIP ||
        g_aeSpecialManeuver_0059c3c0[target] ==
            SPECIAL_MANEUVER_UNKNOWN_9) {
        target = -1;
        g_acShipTarget_0059ce60[0] = -1;
    }
    g_cTargetDisplayObject_0046c06c = (signed char)target;
    if (target == -1) {
        DrawFormattedText("Target: None\n");
        return;
    }
    name = g_aObjectTypeData_00466458[
        g_aeObjectType_0059b560[target]].displayName;
    DrawFormattedText("Target: %s\n", name == 0 ? "" : name);
    if (g_asObjectScreenX_0059d9b0[target] == (short)0x8001)
        DrawFormattedText("Range: -----\n");
    else if ((unsigned short)g_asObjectDistance_0059b4a0[target] > 30000)
        DrawFormattedText("Range: TOO FAR\n");
    else
        DrawFormattedText("Range: %u M\n",
                          (unsigned short)g_asObjectDistance_0059b4a0[target]);
}

/* Function start: 0x42DEA0 */
void DrawTargetRangeReadout(void)
{
    short target;

    target = (short)g_acShipTarget_0059ce60[0];
    if (target < 0 || target >= WC1_SPACE_OBJECT_COUNT ||
        g_aeObjectClass_0059d100[target] < OBJECT_CLASS_SHIP ||
        g_aeSpecialManeuver_0059c3c0[target] ==
            SPECIAL_MANEUVER_UNKNOWN_9) {
        if (g_cTargetDisplayObject_0046c06c != -1) {
            g_acShipTarget_0059ce60[0] = -1;
            show_target_disp();
        }
        return;
    }
    if (g_cTargetDisplayObject_0046c06c != target ||
        (g_nSpaceFrame_0059b420 & 7) == 0)
        show_target_disp();
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
    stop_all_sounds();
    SoundDebugPrintf("FlushSoundEffect");
}

/* Function start: 0x42E3C0 */
void FlushSoundEffects(void)
{
    stop_all_sounds();
    SoundDebugPrintf("FlushSoundEffects");
}

/* Function start: 0x42E3E0 */
void SceneLeaveHook(void)
{
}

/* Function start: 0x42E3F0 */
void SelectFlightMusicTrack(int track)
{
    const char *streamName;
    int streamSet;

    switch (track) {
    case 0:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 27:
    case 31:
    case 32:
        streamSet = 2;
        break;
    case 1:
        streamSet = 2;
        SoundDebugPrintf("being tailed");
        break;
    case 19:
        SoundDebugPrintf("ofx music");
        break;
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 30:
    case 35:
        streamSet = 0;
        break;
    case 28:
    case 29:
    case 33:
    case 34:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
        streamSet = 1;
        break;
    default:
        streamSet = -1;
        break;
    }

    if (streamSet == g_nMusicStreamSet_0046aa18)
        return;
    if (streamSet == -1) {
        if (g_nMusicStreamSet_0046aa18 >= 0)
            Streamer_close();
        return;
    }

    if (streamSet == 0) {
        streamName = "preflite.str";
    } else if (streamSet == 1) {
        streamName = "posflite.str";
    } else if (streamSet == 2) {
        streamName = "mission.str";
        ClearStreamerTrigger();
    } else {
        streamName = 0;
    }

    if (streamName != 0) {
        Streamer_open(streamName);
        g_nMusicStreamSet_0046aa18 = streamSet;
    } else {
        Streamer_close();
        g_nMusicStreamSet_0046aa18 = -1;
    }
}

/* Function start: 0x42E520 */
int MapMusicTrackToStreamerCommand(int track)
{
    switch (track) {
    case 0:
        return 5;
    case 1:
        SoundDebugPrintf("being tailed");
        return 7;
    case 2:
        return 7;
    case 3:
        return 8;
    case 4:
        return 9;
    case 5:
        return 6;
    case 6:
        return 15;
    case 7:
        return 13;
    case 8:
        return 16;
    case 9:
        return 14;
    case 10:
        return 17;
    case 11:
        return 18;
    case 12:
        return 10;
    case 13:
        return 12;
    case 14:
        return 11;
    case 15:
        return 4;
    case 16:
        return 3;
    case 17:
        return 1;
    case 18:
        return 2;
    case 19:
        SoundDebugPrintf("ofx music");
        return -1;
    case 20:
        return 1;
    case 21:
        return 4;
    case 22:
        return 3;
    case 23:
        return -1;
    case 24:
        return 5;
    case 25:
        return 6;
    case 26:
        return 7;
    case 27:
    case 28:
        return -1;
    case 29:
    case 30:
        return 0;
    case 31:
        return 19;
    case 32:
        return 20;
    case 33:
        return 2;
    case 34:
        return 1;
    case 35:
        return 2;
    case 36:
        return 3;
    case 37:
        return 4;
    case 38:
        return 5;
    case 39:
        return 7;
    case 40:
        return 6;
    default:
        return -1;
    }
}

/* Function start: 0x42E6F0 */
void ProcessMusicScriptCommand(int track, int command, int enabled)
{
    int streamerCommand;

    (void)enabled;
    if (track == -1 || g_bMusicCommandSuppressed_0046a9fc != 0)
        return;
    if (command == 4) {
        SoundDebugPrintf("queue_stop\n");
        StopMusic();
        DAT_0046aa14 = 0xffffffff;
        return;
    }

    SoundDebugPrintf("track_%02d ", track);
    if ((DAT_0046aa14 == 25 && track == 25) ||
        (DAT_0046aa14 == 38 && track == 38) ||
        (DAT_0046aa14 == 39 && track == 39) ||
        (DAT_0046aa14 == 40 && track == 40)) {
        SoundDebugPrintf("skipping for QA\n");
        return;
    }

    DAT_0046aa14 = track;
    SelectFlightMusicTrack(track);
    if (g_nMusicStreamSet_0046aa18 == 2) {
        if ((track >= 0 && track <= 5) ||
            (track >= 12 && track <= 18)) {
            SoundDebugPrintf("flight_intensity %d ", track);
            SetStreamerIntensity((unsigned char)track);
        } else {
            SoundDebugPrintf("flight_trigger %d ", track);
            Streamer_trigger(track);
        }
        SoundDebugPrintf("\n");
        return;
    }

    switch (command) {
    case 0:
        SoundDebugPrintf(" queue_start ");
        streamerCommand = MapMusicTrackToStreamerCommand(track);
        Streamer_trigger(streamerCommand);
        break;
    case 1:
        SoundDebugPrintf(" queue_break ");
        streamerCommand = MapMusicTrackToStreamerCommand(track);
        ForceStreamerTrigger(streamerCommand);
        break;
    case 2:
        SoundDebugPrintf(" queue_switch ");
        streamerCommand = MapMusicTrackToStreamerCommand(track);
        Streamer_trigger(streamerCommand);
        break;
    case 3:
        SoundDebugPrintf(" queue_interrupt ");
        streamerCommand = MapMusicTrackToStreamerCommand(track);
        ForceStreamerTrigger(streamerCommand);
        break;
    }
    SoundDebugPrintf("\n");
}

/* Function start: 0x42E880 */
unsigned int StartMusicTrack(int track, int mode, int enabled)
{
    if (DAT_0046a9f8 != 0 && DAT_0046a9f8 != 3)
        ProcessMusicScriptCommand(track, mode, enabled);
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
    stop_all_sounds();
    SoundDebugPrintf("FlushSoundEffects");
}

/* Function start: 0x42EF20 */
void sound_effect(void)
{
    WriteDebugString("sound_effect");
}

/* Function start: 0x42EF30 */
void PlaySfxWaveFileByNumber(int soundNumber, int sourceObject, int looping)
{
    FixedVector delta;
    int distance;

    if (sourceObject != -1) {
        ComputeVectorDelta(&g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                           &g_aShipPosition_0059c490[sourceObject],
                           &delta);
        distance = ComputeFixedVectorMagnitude(&delta);
        if (distance > 32000)
            distance = 32000;
    } else {
        distance = 32000;
    }
    if (distance >= 10) {
        g_aiSoundEffectSourceActive_005a66ec[sourceObject + 1] = 1;
        sprintf(g_szSfxWavePath_00476558, g_szSfxWaveFormat_0046ad2c,
                soundNumber - 1);
        playWAVE((unsigned char *)g_szSfxWavePath_00476558,
                 looping, distance);
    }
}
