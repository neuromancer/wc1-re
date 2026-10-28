/*
 *  Music state machine and the streaming music script.
 *
 *  The provisional address range is recorded in docs/ORDER.md.
 *  Boundary evidence: PROVEN by the names the routines print: StopMusic, FadeMusic, SetMusicOn, ....
 */
#include "wc1.h"

/* The DAT_ words in this block are zero-initialized original data that
 * nothing in the shipped executable references. */
short nMusicPlaybackMode = 0; /* 0x0046a9f8 */
static int bMusicCommandSuppressed = 0; /* 0x0046a9fc */
static int DAT_0046aa00 = 0; /* 0x0046aa00 */
short nMusicTrackComplete = 1; /* 0x0046aa04 */
static int DAT_0046aa08 = 0; /* 0x0046aa08 */
static short nMusicWaitForInput = 1; /* 0x0046aa0c */
short nFuneralSequenceActive = 0; /* 0x0046aa10 */
int nCurrentMusicTrack = -1; /* 0x0046aa14 */
static int nMusicStreamSet = -1; /* 0x0046aa18 */
static int DAT_0046aa1c = 0; /* 0x0046aa1c */
static int DAT_0046aa20 = 0; /* 0x0046aa20 */
int nInFlightMusicSlotA = -1; /* 0x0046aa24 */
static int DAT_0046aa28 = 0; /* 0x0046aa28 */
int nInFlightMusicSlotB = -1; /* 0x0046aa2c */
int nWaitForMusicEnabled = 1; /* 0x0046aa30 */
int nFlightSoundEffectsEnabled = 1; /* 0x0046aa34 */
int nInitialFlightMusicPending = 1; /* 0x0046aa38 */
int nCombatMusicActive = 0; /* 0x0046aa3c */
int nInFlightMusicActive = 0; /* 0x0046aa40 */
static int DAT_0046aa44 = 0; /* 0x0046aa44 */
static short nPassingShipSoundObject = -1; /* 0x0046aa48 */
static short nPassingShipSoundCountdown = 0; /* 0x0046aa4c */

/* Legacy sound records; the last byte is the sequential effect index.
 * Only the final record is still passed to SoundFxTick in this port. */
/* 0x0046aa50 */
unsigned char aabSoundEffectDescriptors[37][8] = {
    { 0x00, 0x01, 0x40, 0x40, 0x3c, 0x00, 0x00, 0x00 },
    { 0x02, 0x02, 0x40, 0x40, 0x01, 0x00, 0x00, 0x01 },
    { 0x00, 0x27, 0x40, 0x40, 0x3c, 0x00, 0x00, 0x02 },
    { 0x00, 0x04, 0x40, 0x40, 0x3c, 0x00, 0x00, 0x03 },
    { 0x00, 0x07, 0x40, 0x40, 0x06, 0x00, 0x00, 0x04 },
    { 0x00, 0x08, 0x40, 0x40, 0x1e, 0x00, 0x00, 0x05 },
    { 0x00, 0x09, 0x40, 0x40, 0x0a, 0x00, 0x00, 0x06 },
    { 0x00, 0x0a, 0x40, 0x40, 0x3c, 0x00, 0x00, 0x07 },
    { 0x00, 0x0b, 0x40, 0x40, 0x06, 0x00, 0x00, 0x08 },
    { 0x00, 0x0c, 0x40, 0x40, 0x0a, 0x00, 0x00, 0x09 },
    { 0x04, 0x0d, 0x40, 0x40, 0x3c, 0x00, 0x00, 0x0a },
    { 0x04, 0x0e, 0x40, 0x40, 0x3c, 0x00, 0x00, 0x0b },
    { 0x00, 0x0f, 0x40, 0x40, 0x06, 0x00, 0x00, 0x0c },
    { 0x08, 0x13, 0x40, 0x40, 0x3c, 0x00, 0x00, 0x0d },
    { 0x04, 0x14, 0x40, 0x40, 0x3c, 0x00, 0x00, 0x0e },
    { 0x00, 0x15, 0x40, 0x40, 0x3c, 0x00, 0x00, 0x0f },
    { 0x04, 0x16, 0x40, 0x40, 0x3c, 0x00, 0x00, 0x10 },
    { 0x02, 0x17, 0x54, 0x40, 0x06, 0x00, 0x39, 0x11 },
    { 0x00, 0x18, 0x40, 0x40, 0x3c, 0x00, 0x00, 0x12 },
    { 0x02, 0x29, 0x18, 0x40, 0x02, 0x00, 0x7f, 0x13 },
    { 0x00, 0x2a, 0x40, 0x40, 0x28, 0x00, 0x00, 0x14 },
    { 0x00, 0x2b, 0x40, 0x40, 0x28, 0x00, 0x00, 0x15 },
    { 0x00, 0x2d, 0x40, 0x40, 0x05, 0x00, 0x00, 0x16 },
    { 0x00, 0x2e, 0x40, 0x40, 0x05, 0x00, 0x00, 0x17 },
    { 0x00, 0x2f, 0x40, 0x40, 0x05, 0x00, 0x00, 0x18 },
    { 0x00, 0x30, 0x40, 0x40, 0x28, 0x00, 0x00, 0x19 },
    { 0x00, 0x40, 0x40, 0x40, 0x28, 0x00, 0x00, 0x1a },
    { 0x00, 0x7d, 0x40, 0x40, 0x28, 0x00, 0x00, 0x1b },
    { 0x00, 0x3e, 0x40, 0x40, 0x28, 0x00, 0x00, 0x1c },
    { 0x00, 0x3f, 0x40, 0x40, 0x28, 0x00, 0x00, 0x1d },
    { 0x00, 0x6a, 0x40, 0x40, 0x3c, 0x00, 0x00, 0x1e },
    { 0x08, 0x6b, 0x40, 0x40, 0x28, 0x00, 0x00, 0x1f },
    { 0x00, 0x6d, 0x40, 0x40, 0x3c, 0x00, 0x00, 0x20 },
    { 0x00, 0x6e, 0x40, 0x40, 0x50, 0x00, 0x00, 0x21 },
    { 0x00, 0x6f, 0x40, 0x40, 0x28, 0x00, 0x00, 0x22 },
    { 0x00, 0x70, 0x40, 0x40, 0x3c, 0x00, 0x00, 0x23 },
    { 0x00, 0x80, 0x40, 0x40, 0x3c, 0x00, 0x00, 0x24 }
};

/* The SDL audio adapter also uses this filename buffer. */
#ifndef SDL_PORT
static
#endif
char szSfxWavePath[0xc8]; /* 0x00476558 */


/* Linker commons defined here; LINK orders them by identifier hash. */
unsigned short awObjectScales[SPACE_OBJECT_COUNT]; /* 0x0059de40 */
signed char acVduModeStackDepth[16]; /* 0x0059dec0 */
signed char acShipSpawnNavPt[16]; /* 0x0059ded0 */
short asShipAccumulatedDmg[SPACE_OBJECT_COUNT]; /* 0x0059dee0 */
short nCurrentNavigationPoint; /* 0x0059df60 */
int nJoystickCalibrationMinimumY; /* 0x0059df64 */
int nJoystickCalibrationMinimumX; /* 0x0059df68 */
int nJoystickCalibrationMaximumX; /* 0x0059df6c */
int nJoystickCalibrationMaxY; /* 0x0059df70 */
unsigned short awPalEntryAllocation[256]; /* 0x0059df80 */
unsigned char *apCommunicationPortraitShapesTable[16]; /* 0x0059e180 */
char szHudMessageBuf[0x60]; /* 0x0059e1c0 */
char *aapszPilotSpeeches[14][11]; /* 0x0059e220 */
signed char abCommMenuChoiceCommands[7]; /* 0x0059e488 */

/* Function start: 0x42E320 */
void FadeMusic(void)
{
    SoundDebugPrintf("FadeMusic\n");
}

/* Function start: 0x42E330 */
void SetMusicOn(short enabled)
{
    SoundDebugPrintf("SetMusicOn %d\n", (int)enabled);
}

/* Function start: 0x42E350 */
void StopMusic(short unused)
{
    (void)unused;
    SoundDebugPrintf("StopMusic ");
    nCurrentMusicTrack = -1;
    Streamer_stop();
    SoundDebugPrintf("\n");
}

/* Function start: 0x42E380 */
void SetMusBreakpt(int first, int second)
{
    (void)first;
    (void)second;
    SoundDebugPrintf("SetMusBreakpt\n");
}

/* Function start: 0x42E390 */
void PaletteFadeHook(void)
{
}

/* Function start: 0x42E3A0 */
void FlushSoundEffect(void)
{
    stop_all_sounds();
    SoundDebugPrintf("FlushSoundEffect\n");
}

/* Function start: 0x42E3C0 */
void FlushSoundEffects(void)
{
    stop_all_sounds();
    SoundDebugPrintf("FlushSoundEffects\n");
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
        SoundDebugPrintf("being tailed");
        streamSet = 2;
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

    if (streamSet == nMusicStreamSet)
        return;
    if (streamSet == -1) {
        if (nMusicStreamSet >= 0)
            Streamer_close();
        return;
    }

    switch (streamSet) {
    case 0:
        streamName = "preflite.str";
        break;
    case 1:
        streamName = "posflite.str";
        break;
    case 2:
        streamName = "mission.str";
        ClearStreamerTrigger();
        break;
    default:
        streamName = 0;
        break;
    }

    if (streamName != 0) {
        Streamer_open(streamName);
        nMusicStreamSet = streamSet;
    } else {
        Streamer_close();
        nMusicStreamSet = -1;
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
    default:
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
        return -1;
    case 28:
        return -1;
    case 29:
        return 0;
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
    }
}

/* Function start: 0x42E6F0 */
int ProcessMusicScriptCommand(int track, int command, short enabled)
{
    int streamerCommand;

    if (track == -1 || bMusicCommandSuppressed != 0)
        return 0;
    if (command == 4) {
        SoundDebugPrintf("queue_stop\n");
        StopMusic(enabled);
        nCurrentMusicTrack = -1;
        return;
    }

    SoundDebugPrintf("track_%02d ", track);
    if ((nCurrentMusicTrack == 25 && track == 25) ||
        (nCurrentMusicTrack == 38 && track == 38) ||
        (nCurrentMusicTrack == 39 && track == 39) ||
        (nCurrentMusicTrack == 40 && track == 40)) {
        SoundDebugPrintf("skipping for QA\n");
        return;
    }

    nCurrentMusicTrack = track;
    SelectFlightMusicTrack(track);
    if (nMusicStreamSet == 2) {
        if ((track >= 0 && track <= 5) ||
            (track >= 12 && track <= 18)) {
            SoundDebugPrintf("flight_intensity %d ", track);
            SetStreamerIntensity((unsigned char)track);
        } else {
            SoundDebugPrintf("flight_trigger %d ", track);
            Streamer_trigger(track);
        }
    } else {
        /* A compiled-out trace: nothing references its literal at
         * 0x0046ac98, which precedes the live one below. */
        if (0)
            SoundDebugPrintf(" queue_start ");
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
    }
    SoundDebugPrintf("\n");
    return 1;
}

/* Function start: 0x42E880 */
unsigned int spacetrack(int track, int mode, short enabled)
{
    if (nMusicPlaybackMode != 0 && nMusicPlaybackMode != 3)
        ProcessMusicScriptCommand(track, mode, enabled);
    return 1;
}

/* Function start: 0x42E8B0 */
void StopMusicUnlessSuppressed(void)
{
    if (nMusicPlaybackMode != 0 && nMusicPlaybackMode != 3)
        StopMusic(0);
}

/* Function start: 0x42E8D0 */
unsigned short GetMusicMode(void)
{
    if (nMusicPlaybackMode != 0 && nMusicPlaybackMode != 3 &&
        nMusicTrackComplete != 0)
        return 1;
    return 0;
}

/* Function start: 0x42E900 */
void wait_for_end_of_music(void)
{
    if (nMusicPlaybackMode != 0 && nMusicPlaybackMode != 3) {
        if (nWaitForMusicEnabled == 0) {
            StopMusic(0);
            return;
        }
        SetMusBreakpt(0, 0);
        do {
            if (nMusicTrackComplete != 0)
                return;
        } while (bEscapePressedFlag == 0 && CheckEscaped() == 0);
        StopMusic(0);
    }
}

/* Function start: 0x42E970 */
void WaitForMusicOrInput(void)
{
    if (nMusicPlaybackMode != 0 && nMusicPlaybackMode != 3) {
        if (nWaitForMusicEnabled == 0) {
            StopMusicUnlessSuppressed();
            return;
        }
        for (;;) {
            if (nMusicWaitForInput == 0 && nMusicTrackComplete == 0)
                return;
            if (bEscapePressedFlag != 0 || CheckEscaped() != 0)
                break;
        }
        StopMusic(0);
    }
}

/* Function start: 0x42E9E0 */
void new_space_music_changes(short attacker, short victim)
{
    enum Side side;

    if (nInFlightMusicActive != 0 &&
        nTrainSimActive == 0) {
        side = aeShipSide[victim];
        if (side == SIDE_KILRATHI) {
            if (report_kilrathi_rout(1) == 0) {
                spacetrack(10, 1, 0);
                return;
            }
            if (attacker == 0) {
                if (acShipRating[victim] == -1 &&
                    RandomInRange(0, 3) != 0) {
                    spacetrack(6, 3, 0);
                    return;
                }
                spacetrack(9, 3, 0);
                return;
            }
        } else {
            if (nYourWingman == victim) {
                spacetrack(8, 3, 0);
                return;
            }
            if (side == SIDE_IMPERIAL) {
                if ((aeShipMissionType[0] ==
                         MISSION_TYPE_DEFEND ||
                     aeShipMissionType[0] ==
                         MISSION_TYPE_ESCORT) &&
                    nShipMissionIndices[victim] ==
                        anShipMissionShips[0]) {
                    spacetrack(11, 3, 0);
                    return;
                }
                spacetrack(7, 3, 0);
            }
        }
    }
}

/* Function start: 0x42EAD0 */
int changetrack(void)
{
    int track;

    switch (aeShipMissionType[0]) {
    case MISSION_TYPE_ESCORT:
        track = 18;
        break;
    case MISSION_TYPE_STRIKE:
        track = 17;
        break;
    case MISSION_TYPE_DEFEND:
    case MISSION_TYPE_RENDEZVOUS:
        track = 16;
        break;
    default:
        track = 15;
        break;
    }
    if (aMissionObjectivesArray[
            cCurrentObjective].type == OBJECTIVE_HOME_BASE) {
        if (triumph(0) != 0) {
            if (aeShipMissionType[0] == MISSION_TYPE_PATROL)
                return 13;
            return 14;
        }
        track = 12;
    }
    return track;
}

/* Function start: 0x42EB60 */
void gametrack(void)
{
    int track;
    short damage;

    track = -1;
    if (nInFlightMusicActive != 0) {
        if (nTrainSimActive != 0) {
            int currentTrack;
            int streamSet;

            streamSet = nMusicStreamSet;
            if (streamSet != 0 ||
                (currentTrack = nCurrentMusicTrack) != 20) {
                spacetrack(20, 1, 0);
                currentTrack = nCurrentMusicTrack;
                streamSet = nMusicStreamSet;
            }
            SoundDebugPrintf("%d %d\n", streamSet, currentTrack);
            return;
        }
        if (nCombatMusicActive != 0) {
            if ((nCurrentSpaceFrameValue & 0xf) == 0 ||
                nMusicTrackComplete != 0) {
                if (nInitialFlightMusicPending != 0)
                    nInitialFlightMusicPending = 0;
                if (missile_on_tail(0) != 0) {
                    track = 3;
                } else if (any_enemy_tail(0) != 0) {
                    track = 1;
                } else if (is_ship_tailing_player_target(0) != 0) {
                    track = 2;
                } else {
                    damage = (short)calculate_damage_level();
                    if (damage >= 2)
                        track = 4;
                    else
                        track = damage == 1 ? 5 : 0;
                }
                if (report_kilrathi_rout(1) == 0)
                    nCombatMusicActive = 0;
            }
        } else if ((nCurrentSpaceFrameValue & 0xf) == 0 ||
                   nMusicTrackComplete != 0) {
            track = changetrack();
            if (report_kilrathi_rout(2) != 0)
                nCombatMusicActive = 1;
        }
        spacetrack(track, 1, 0);
    }
}

/* Function start: 0x42ECB0 */
void servicetrack(void)
{
    short object;
    FixedVector futurePosition;
    FixedVector travel;

    gametrack();
    if (nFlightSoundEffectsEnabled != 0) {
        for (object = 0; object < SPACE_OBJECT_COUNT; object++) {
            if (object == nPassingShipSoundObject) {
                if (aeObjectClass[object] !=
                        OBJECT_CLASS_SHIP ||
                    aeObjectClass[object] !=
                        OBJECT_CLASS_CAPITAL_SHIP)
                    nPassingShipSoundObject = -1;
            }
            switch (aeObjectClass[object]) {
            case OBJECT_CLASS_ASTEROID:
                if (asObjDistancesTable[object] == 0 &&
                    (unsigned short)
                        asPreviousObjectDistance[object] < 50 &&
                    aiSoundEffectSourceActiveArray[object] == 0)
                    PlaySfxWaveFileByNumber(6, object, 0);
                break;
            case OBJECT_CLASS_SHIP:
            case OBJECT_CLASS_CAPITAL_SHIP:
                if (asObjectScreenXList[object] != (short)0x8001 &&
                    (unsigned short)asObjDistancesTable[object] < 0x55a) {
                    if (nPassingShipSoundObject == -1) {
                        ScaleFixedVector(&aShipVelocity[object],
                                         0x1400, &travel);
                        AddFixedVectors(&aShipPositionsArray[object],
                                        &travel, &futurePosition);
                        ComputeVectorDelta(
                            &aShipPositionsArray[EYE_OBJECT],
                            &futurePosition, &travel);
                        ComputeVectorDelta(
                            &aShipPositionsArray[EYE_OBJECT],
                            &aShipPositionsArray[object],
                            &futurePosition);
                        if (dot_product(&travel, &futurePosition) < 0xdd) {
                            nPassingShipSoundCountdown = 10;
                            nPassingShipSoundObject = object;
                            if (nPassingShipAudioCooldown <
                                nCurrentSpaceFrameValue) {
                                nPassingShipAudioCooldown =
                                    nCurrentSpaceFrameValue + 6;
                                PlaySfxWaveFileByNumber(2, object, 0);
                            }
                        }
                    } else if (object ==
                                   nPassingShipSoundObject) {
                        nPassingShipSoundCountdown--;
                        if (nPassingShipSoundCountdown == 0)
                            nPassingShipSoundObject = -1;
                    }
                }
                break;
            }
        }
    }
}

/* Function start: 0x42EE80 */
void ResetSoundState(void)
{
    FlushSoundEffects();
    bAfterburnerSfxEnabled = 0;
    nCurrentDamageAlarmSfxHandle = 0;
}

/* Function start: 0x42EEA0 */
void ResetSoundStateForScene(void)
{
    ResetSoundState();
    nFlightSoundEffectsEnabled = 0;
}

/* Function start: 0x42EEB0 */
void ResetSoundStateForFlight(void)
{
    ResetSoundState();
    nFlightSoundEffectsEnabled = 1;
}

/* Function start: 0x42EEC0 */
/* Nothing in the shipped executable calls this routine. */
void DisableMusicForScene(void)
{
    StopMusicUnlessSuppressed();
    nWaitForMusicEnabled = 0;
    SetMusicOn(0);
}

/* Function start: 0x42EEE0 */
void EnableMusicForScene(void)
{
    nWaitForMusicEnabled = 1;
    SetMusicOn(1);
}

/* Function start: 0x42EF00 */
unsigned int SoundFxTick(void)
{
    WriteDebugString("soundFX\n");
    return 0;
}

/* Function start: 0x42EF10 */
void FlushSoundEffectsAndLog(int soundHandle, short fadeTicks)
{
    (void)fadeTicks;
    FlushSoundEffects();
    return;
    /* Unreachable: only its wave-name literal survives, at 0x0046ad04. */
    sprintf(szSfxWavePath, "sfx%02i.wav", soundHandle - 1);
}

/* Function start: 0x42EF20 */
void sound_effect(void)
{
    WriteDebugString("sound_effect");
    return;
    /* Unreachable: only its wave-name literal survives, at 0x0046ad20. */
    sprintf(szSfxWavePath, "sfx%02i.wav", 0);
}

/* Function start: 0x42EF30 */
void PlaySfxWaveFileByNumber(int soundNumber, int sourceObject, int looping)
{
    FixedVector delta;
    int distance;

#ifdef SDL_PORT
    if (SdlHandlesGameSoundEffects()) {
        SdlPlayGameSoundEffect(soundNumber, sourceObject, looping);
        return;
    }
    if (SdlUsingDosData()) {
        int volume;

        volume = 127;
        if (sourceObject != -1) {
            if (sourceObject < 0 ||
                sourceObject >= SPACE_OBJECT_COUNT)
                return;
            ComputeVectorDelta(
                &aShipPositionsArray[EYE_OBJECT],
                &aShipPositionsArray[sourceObject], &delta);
            distance = (int)((Vector_magnitude(&delta) / 500L) >> 8);
            volume -= distance;
            if (volume < 0)
                volume = 0;
        }
        if (volume >= 10 && SdlPlayDosSoundEffect(
                soundNumber, volume, 64, sourceObject, looping)) {
            aiSoundEffectSourceActiveArray[sourceObject] = 1;
            if (sourceObject == -1)
                bAfterburnerSfxEnabled = soundNumber == 12;
        }
        return;
    }
#endif

    if (sourceObject != -1) {
        long magnitude;

        ComputeVectorDelta(&aShipPositionsArray[EYE_OBJECT],
                           &aShipPositionsArray[sourceObject],
                           &delta);
        magnitude = Vector_magnitude(&delta);
        distance = magnitude;
        if (magnitude > 32000)
            distance = 32000;
    } else {
        distance = 32000;
    }
    if (distance >= 10) {
        aiSoundEffectSourceActiveArray[sourceObject] = 1;
        sprintf(szSfxWavePath, "sfx%02i.wav",
                soundNumber - 1);
        playWAVE(szSfxWavePath, looping, distance);
    }
}
