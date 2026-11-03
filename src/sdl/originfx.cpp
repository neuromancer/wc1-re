/* SDL2-only OriginFX MIDI/AdLib replayer for WC1 DOS data. */
#include "wc1sdl.h"

#include "ymfm_opl.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <new>

#define ORIGINFX_OUTPUT_RATE 22050U
#define ORIGINFX_OPL_CLOCK 3579545U
#define ORIGINFX_SERVICE_RATE 60U
#define ORIGINFX_MELODIC_VOICE_COUNT 9
#define ORIGINFX_VOICE_STATE_COUNT 11
#define ORIGINFX_CHANNEL_COUNT 26
#define ORIGINFX_SOUND_CHANNEL_COUNT 8
#define ORIGINFX_FIRST_SOUND_CHANNEL 1
#define ORIGINFX_SOUND_SLOT_COUNT 32
#define ORIGINFX_PERCUSSION_NOTE_COUNT 77
#define ORIGINFX_TIMBRE_SIZE 48U

/* STRAX.DRV 0000:01dc-020e.  Logical voices 6-10 are the five OPL2
 * rhythm voices after the driver reduces the melodic allocator to six
 * channels.  The second half of each operator table is selected in that
 * mode; snare, tom, cymbal, and hi-hat each program only one operator. */
static const unsigned char g_abOriginFxRhythmBits[5] = {
    0x10, 0x08, 0x04, 0x02, 0x01
};

static const unsigned char g_abOriginFxCarrierOffsets[20] = {
    3, 4, 5, 11, 12, 13, 19, 20, 21,
    3, 4, 5, 11, 12, 13, 19, 20, 18, 21, 17
};

static const unsigned char g_abOriginFxModulatorOffsets[20] = {
    0, 1, 2, 8, 9, 10, 16, 17, 18,
    0, 1, 2, 8, 9, 10, 16, 20, 18, 21, 17
};

/* WC.EXE 2231:051e-063d.  FUN_16ac_07f2 indexes the table with the
 * one-based game sound number.  Each OriginFX record is flags, program + 1,
 * note, velocity, a little-endian 60 Hz duration, glide target, and an
 * unused byte.  The preceding eight bytes at 2231:0516 are not a sound. */
static const unsigned char g_aabOriginFxSoundRecords[36][8] = {
    { 0, 1, 64, 64, 60, 0, 0, 0 },
    { 2, 2, 64, 64, 1, 0, 0, 0 },
    { 0, 39, 64, 64, 60, 0, 0, 0 },
    { 0, 4, 64, 64, 60, 0, 0, 0 },
    { 0, 7, 64, 64, 6, 0, 0, 0 },
    { 0, 8, 64, 64, 30, 0, 0, 0 },
    { 0, 9, 64, 64, 10, 0, 0, 0 },
    { 0, 10, 64, 64, 60, 0, 0, 0 },
    { 0, 11, 64, 64, 6, 0, 0, 0 },
    { 0, 12, 64, 64, 10, 0, 0, 0 },
    { 4, 13, 64, 64, 60, 0, 0, 0 },
    { 4, 14, 64, 64, 60, 0, 0, 0 },
    { 0, 15, 64, 64, 6, 0, 0, 0 },
    { 8, 19, 64, 64, 60, 0, 0, 0 },
    { 4, 20, 64, 64, 60, 0, 0, 0 },
    { 0, 21, 64, 64, 60, 0, 0, 0 },
    { 4, 22, 64, 64, 60, 0, 0, 0 },
    { 2, 23, 84, 64, 6, 0, 57, 0 },
    { 0, 24, 64, 64, 60, 0, 0, 0 },
    { 2, 41, 24, 64, 2, 0, 127, 0 },
    { 0, 42, 64, 64, 40, 0, 0, 0 },
    { 0, 43, 64, 64, 40, 0, 0, 0 },
    { 0, 45, 64, 64, 5, 0, 0, 0 },
    { 0, 46, 64, 64, 5, 0, 0, 0 },
    { 0, 47, 64, 64, 5, 0, 0, 0 },
    { 0, 48, 64, 64, 40, 0, 0, 0 },
    { 0, 64, 64, 64, 40, 0, 0, 0 },
    { 0, 125, 64, 64, 40, 0, 0, 0 },
    { 0, 62, 64, 64, 40, 0, 0, 0 },
    { 0, 63, 64, 64, 40, 0, 0, 0 },
    { 0, 106, 64, 64, 60, 0, 0, 0 },
    { 8, 107, 64, 64, 40, 0, 0, 0 },
    { 0, 109, 64, 64, 60, 0, 0, 0 },
    { 0, 110, 64, 64, 80, 0, 0, 0 },
    { 0, 111, 64, 64, 40, 0, 0, 0 },
    { 0, 112, 64, 64, 60, 0, 0, 0 }
};

/* WC.EXE 2231:7967 and 2231:79b4 translate MIDI channel 10 into
 * OriginFX's percussion pseudo-channels and their fixed pitches. */
static const unsigned char g_abOriginFxPercussionChannels[
    ORIGINFX_PERCUSSION_NOTE_COUNT] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 10, 10, 18, 11, 0, 12, 13, 17, 13, 16, 13, 14, 13,
    13, 15, 13, 19, 0, 0, 0, 0, 21, 0, 0, 0, 26, 26, 25, 20,
    20, 0, 0, 21, 21, 22, 23, 0, 0, 24, 0, 20, 0
};

static const unsigned char g_abOriginFxPercussionPitches[
    ORIGINFX_PERCUSSION_NOTE_COUNT] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 48, 48, 48, 48, 0, 48, 42, 71, 42, 71, 47, 71, 47,
    52, 79, 52, 77, 0, 0, 0, 0, 71, 0, 0, 0, 72, 79, 79, 64,
    58, 0, 0, 89, 84, 48, 72, 0, 0, 36, 0, 96, 0
};

/* WC.EXE 19c5:1682-174a selects these timbres for channels 10-26 when
 * STRAX.DRV's AdLib output is initialized. */
static const unsigned char g_abOriginFxPercussionPrograms[17] = {
    0x80, 0x72, 0x83, 0x71, 0x86, 0x87, 0x85, 0x84, 0x81,
    0x88, 0x8d, 0x8f, 0x90, 0x91, 0x93, 0x8c, 0x8b
};

enum OriginFxEventType {
    ORIGINFX_CHANNEL_EVENT,
    ORIGINFX_TEMPO_EVENT,
    ORIGINFX_SEQUENCE_EVENT
};

typedef struct OriginFxEvent {
    uint64_t tick;
    uint64_t frame;
    uint32_t order;
    uint32_t tempo;
    unsigned char type;
    unsigned char status;
    unsigned char data1;
    unsigned char data2;
} OriginFxEvent;

typedef struct OriginFxChannel {
    unsigned short pitchBend;
    unsigned short volume;
    unsigned short modulationDepth;
    unsigned char program;
    unsigned char modulationRate;
    unsigned char pan;
} OriginFxChannel;

typedef struct OriginFxVoice {
    const unsigned char *timbre;
    uint64_t age;
    unsigned char channel;
    unsigned char note;
    unsigned char velocity;
    unsigned char frequencyHigh;
    unsigned char envelopeState;
    unsigned char modulationPhase;
    int envelopePitch;
    int modulationPitch;
    int active;
} OriginFxVoice;

typedef struct OriginFxSoundEffect {
    const unsigned char *record;
    uint64_t age;
    int tag;
    int priority;
    unsigned short remainingTicks;
    unsigned char channel;
    unsigned char currentNote;
    unsigned char volume;
    unsigned char pan;
    int active;
} OriginFxSoundEffect;

class OriginFxYmfmInterface : public ymfm::ymfm_interface {
};

struct SdlOriginFxPlayer {
    OriginFxYmfmInterface oplInterface;
    ymfm::ym3812 oplChip;
    OriginFxYmfmInterface oplRightInterface;
    ymfm::ym3812 oplRightChip;
    OriginFxEvent *events;
    unsigned char *timbres;
    size_t eventCount;
    size_t eventCapacity;
    size_t nextEvent;
    size_t timbreSize;
    uint64_t endFrame;
    uint64_t currentFrame;
    uint64_t nextVoiceAge;
    uint64_t nextSoundEffectAge;
    uint64_t nativeSampleAccumulator;
    uint64_t serviceAccumulator;
    uint32_t nativeSampleRate;
    unsigned int sequencePosition;
    int32_t lastNativeSample;
    int32_t lastNativeRightSample;
    unsigned char melodicVoiceCount;
    unsigned char rhythmRegister;
    int stereoPanningEnabled;
    int finished;
    OriginFxChannel channels[ORIGINFX_CHANNEL_COUNT];
    OriginFxVoice voices[ORIGINFX_VOICE_STATE_COUNT];
    OriginFxSoundEffect soundEffects[ORIGINFX_SOUND_SLOT_COUNT];

    SdlOriginFxPlayer() :
        oplChip(oplInterface),
        oplRightChip(oplRightInterface),
        events(0),
        timbres(0),
        eventCount(0),
        eventCapacity(0),
        nextEvent(0),
        timbreSize(0),
        endFrame(0),
        currentFrame(0),
        nextVoiceAge(1),
        nextSoundEffectAge(1),
        nativeSampleAccumulator(0),
        serviceAccumulator(0),
        nativeSampleRate(oplChip.sample_rate(ORIGINFX_OPL_CLOCK)),
        sequencePosition(0),
        lastNativeSample(0),
        lastNativeRightSample(0),
        melodicVoiceCount(ORIGINFX_MELODIC_VOICE_COUNT),
        rhythmRegister(0),
        stereoPanningEnabled(0),
        finished(0)
    {
        memset(channels, 0, sizeof(channels));
        memset(voices, 0, sizeof(voices));
        memset(soundEffects, 0, sizeof(soundEffects));
    }

    ~SdlOriginFxPlayer()
    {
        free(events);
        free(timbres);
    }
};

static uint16_t OriginFxReadBigEndian16(const unsigned char *bytes)
{
    return (uint16_t)(((uint16_t)bytes[0] << 8) | bytes[1]);
}

static uint32_t OriginFxReadBigEndian32(const unsigned char *bytes)
{
    return ((uint32_t)bytes[0] << 24) |
        ((uint32_t)bytes[1] << 16) |
        ((uint32_t)bytes[2] << 8) |
        bytes[3];
}

static int16_t OriginFxReadLittleEndian16(const unsigned char *bytes)
{
    return (int16_t)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8));
}

static int OriginFxReadVariableLength(const unsigned char **cursor,
                                         const unsigned char *end,
                                         uint32_t *value)
{
    uint32_t result;
    unsigned char byte;
    unsigned int count;

    result = 0;
    count = 0;
    do {
        if (*cursor >= end || count == 4)
            return 0;
        byte = *(*cursor)++;
        result = (result << 7) | (byte & 0x7fU);
        count++;
    } while ((byte & 0x80U) != 0);
    *value = result;
    return 1;
}

static int OriginFxAppendEvent(SdlOriginFxPlayer *player,
                                  const OriginFxEvent *event)
{
    OriginFxEvent *resized;
    size_t capacity;

    if (player->eventCount == player->eventCapacity) {
        capacity = player->eventCapacity == 0
            ? 256 : player->eventCapacity * 2;
        if (capacity < player->eventCapacity ||
            capacity > SIZE_MAX / sizeof(OriginFxEvent))
            return 0;
        resized = (OriginFxEvent *)realloc(
            player->events, capacity * sizeof(OriginFxEvent));
        if (resized == 0)
            return 0;
        player->events = resized;
        player->eventCapacity = capacity;
    }
    player->events[player->eventCount++] = *event;
    return 1;
}

static int OriginFxParseTrack(SdlOriginFxPlayer *player,
                                 const unsigned char *track,
                                 size_t trackSize,
                                 uint32_t *eventOrder,
                                 uint64_t *maximumTick)
{
    OriginFxEvent event;
    const unsigned char *cursor;
    const unsigned char *end;
    uint64_t tick;
    uint32_t delta;
    uint32_t eventLength;
    unsigned char runningStatus;
    unsigned char status;
    unsigned char subtype;
    unsigned int dataSize;
    unsigned int index;

    cursor = track;
    end = track + trackSize;
    tick = 0;
    runningStatus = 0;
    while (cursor < end) {
        if (!OriginFxReadVariableLength(&cursor, end, &delta))
            return 0;
        tick += delta;
        if (cursor >= end)
            return 0;
        if (*cursor >= 0x80U) {
            status = *cursor++;
        } else {
            if (runningStatus == 0)
                return 0;
            status = runningStatus;
        }

        if (status >= 0x80U && status <= 0xefU) {
            dataSize = (status & 0xe0U) == 0xc0U ? 1U : 2U;
            if ((size_t)(end - cursor) < dataSize)
                return 0;
            index = 0;
            while (index < dataSize) {
                if (cursor[index] >= 0x80U)
                    return 0;
                index++;
            }
            memset(&event, 0, sizeof(event));
            event.tick = tick;
            event.order = (*eventOrder)++;
            event.type = ORIGINFX_CHANNEL_EVENT;
            event.status = status;
            event.data1 = cursor[0];
            event.data2 = dataSize == 2 ? cursor[1] : 0;
            if (!OriginFxAppendEvent(player, &event))
                return 0;
            cursor += dataSize;
            runningStatus = status;
            continue;
        }

        if (status == 0xfeU) {
            if ((size_t)(end - cursor) < 2)
                return 0;
            subtype = *cursor++;
            eventLength = *cursor++;
            if ((size_t)(end - cursor) < eventLength)
                return 0;
            if (subtype == 3U && eventLength != 0) {
                memset(&event, 0, sizeof(event));
                event.tick = tick;
                event.order = (*eventOrder)++;
                event.type = ORIGINFX_SEQUENCE_EVENT;
                event.data1 = cursor[0];
                if (!OriginFxAppendEvent(player, &event))
                    return 0;
            }
            cursor += eventLength;
            continue;
        }

        if (status == 0xffU) {
            if (cursor >= end)
                return 0;
            subtype = *cursor++;
            if (!OriginFxReadVariableLength(
                    &cursor, end, &eventLength) ||
                (size_t)(end - cursor) < eventLength)
                return 0;
            if (subtype == 0x51U && eventLength == 3) {
                memset(&event, 0, sizeof(event));
                event.tick = tick;
                event.order = (*eventOrder)++;
                event.type = ORIGINFX_TEMPO_EVENT;
                event.tempo = ((uint32_t)cursor[0] << 16) |
                    ((uint32_t)cursor[1] << 8) | cursor[2];
                if (event.tempo == 0 ||
                    !OriginFxAppendEvent(player, &event))
                    return 0;
            }
            cursor += eventLength;
            runningStatus = 0;
            continue;
        }

        if (status == 0xf0U || status == 0xf7U) {
            if (!OriginFxReadVariableLength(
                    &cursor, end, &eventLength) ||
                (size_t)(end - cursor) < eventLength)
                return 0;
            cursor += eventLength;
            runningStatus = 0;
            continue;
        }
        return 0;
    }
    if (tick > *maximumTick)
        *maximumTick = tick;
    return 1;
}

static int OriginFxCompareEvents(const void *left, const void *right)
{
    const OriginFxEvent *leftEvent;
    const OriginFxEvent *rightEvent;

    leftEvent = (const OriginFxEvent *)left;
    rightEvent = (const OriginFxEvent *)right;
    if (leftEvent->tick < rightEvent->tick)
        return -1;
    if (leftEvent->tick > rightEvent->tick)
        return 1;
    if (leftEvent->order < rightEvent->order)
        return -1;
    if (leftEvent->order > rightEvent->order)
        return 1;
    return 0;
}

static int OriginFxLoadMidi(SdlOriginFxPlayer *player,
                               const unsigned char *midi,
                               size_t midiSize)
{
    const unsigned char *cursor;
    const unsigned char *track;
    size_t headerSize;
    size_t playbackEventCount;
    uint64_t maximumTick;
    uint64_t previousTick;
    uint32_t eventOrder;
    uint32_t headerLength;
    uint32_t trackLength;
    uint32_t tempo;
    uint16_t format;
    uint16_t trackCount;
    uint16_t division;
    uint16_t trackIndex;
    long double exactFrame;
    size_t index;

    if (midi == 0 || midiSize < 14 ||
        memcmp(midi, "MThd", 4) != 0)
        return 0;
    headerLength = OriginFxReadBigEndian32(midi + 4);
    headerSize = (size_t)headerLength + 8;
    if (headerLength < 6 || headerSize > midiSize)
        return 0;
    format = OriginFxReadBigEndian16(midi + 8);
    trackCount = OriginFxReadBigEndian16(midi + 10);
    division = OriginFxReadBigEndian16(midi + 12);
    if (format > 1 || trackCount == 0 || (division & 0x8000U) != 0 ||
        division == 0)
        return 0;

    cursor = midi + headerSize;
    eventOrder = 0;
    maximumTick = 0;
    trackIndex = 0;
    while (trackIndex < trackCount) {
        if ((size_t)(midi + midiSize - cursor) < 8 ||
            memcmp(cursor, "MTrk", 4) != 0)
            return 0;
        trackLength = OriginFxReadBigEndian32(cursor + 4);
        cursor += 8;
        if ((size_t)(midi + midiSize - cursor) < trackLength)
            return 0;
        track = cursor;
        if (!OriginFxParseTrack(
                player, track, trackLength, &eventOrder, &maximumTick))
            return 0;
        cursor += trackLength;
        trackIndex++;
    }
    if (cursor != midi + midiSize)
        return 0;

    qsort(player->events, player->eventCount,
          sizeof(OriginFxEvent), OriginFxCompareEvents);
    exactFrame = 0;
    previousTick = 0;
    tempo = 500000;
    playbackEventCount = 0;
    index = 0;
    while (index < player->eventCount) {
        exactFrame +=
            (long double)(player->events[index].tick - previousTick) *
            ORIGINFX_OUTPUT_RATE * tempo /
            ((long double)division * 1000000.0L);
        previousTick = player->events[index].tick;
        if (player->events[index].type == ORIGINFX_TEMPO_EVENT) {
            tempo = player->events[index].tempo;
        } else {
            player->events[index].frame =
                (uint64_t)(exactFrame + 0.5L);
            player->events[playbackEventCount++] = player->events[index];
        }
        index++;
    }
    exactFrame += (long double)(maximumTick - previousTick) *
        ORIGINFX_OUTPUT_RATE * tempo /
        ((long double)division * 1000000.0L);
    player->eventCount = playbackEventCount;
    player->endFrame = (uint64_t)(exactFrame + 0.5L);
    if (player->eventCount != 0 &&
        player->endFrame <= player->events[player->eventCount - 1].frame)
        player->endFrame = player->events[player->eventCount - 1].frame + 1;
    if (player->endFrame == 0)
        player->endFrame = 1;
    return 1;
}

static int OriginFxLoadTimbres(SdlOriginFxPlayer *player,
                                  const unsigned char *timbres,
                                  size_t timbreSize)
{
    size_t requiredSize;
    unsigned int count;

    if (timbres == 0 || timbreSize < ORIGINFX_TIMBRE_SIZE + 1)
        return 0;
    count = timbres[0];
    requiredSize = 1 + (size_t)count * ORIGINFX_TIMBRE_SIZE;
    if (count == 0 || requiredSize > timbreSize)
        return 0;
    player->timbres = (unsigned char *)malloc(requiredSize);
    if (player->timbres == 0)
        return 0;
    memcpy(player->timbres, timbres, requiredSize);
    player->timbreSize = requiredSize;
    return 1;
}

static void OriginFxWriteRegister(SdlOriginFxPlayer *player,
                                     unsigned int address,
                                     unsigned int value)
{
    player->oplChip.write(0, (uint8_t)address);
    player->oplChip.write(1, (uint8_t)value);
    player->oplRightChip.write(0, (uint8_t)address);
    player->oplRightChip.write(1, (uint8_t)value);
}

static void OriginFxWriteStereoRegister(
    SdlOriginFxPlayer *player, unsigned int address,
    unsigned int leftValue, unsigned int rightValue)
{
    player->oplChip.write(0, (uint8_t)address);
    player->oplChip.write(1, (uint8_t)leftValue);
    player->oplRightChip.write(0, (uint8_t)address);
    player->oplRightChip.write(1, (uint8_t)rightValue);
}

static void OriginFxResetOpl(SdlOriginFxPlayer *player)
{
    unsigned int voice;

    player->oplChip.reset();
    player->oplRightChip.reset();
    player->melodicVoiceCount = ORIGINFX_MELODIC_VOICE_COUNT;
    player->rhythmRegister = 0;
    OriginFxWriteRegister(player, 0x01, 0x20);
    OriginFxWriteRegister(player, 0x08, 0);
    OriginFxWriteRegister(player, 0xbd, 0);
    voice = 0;
    while (voice < ORIGINFX_MELODIC_VOICE_COUNT) {
        OriginFxWriteRegister(player, 0xa0 + voice, 0);
        OriginFxWriteRegister(player, 0xb0 + voice, 0);
        voice++;
    }
}

static unsigned int OriginFxGetOperatorTableIndex(
    const SdlOriginFxPlayer *player, unsigned int voiceIndex)
{
    if (player->melodicVoiceCount < ORIGINFX_MELODIC_VOICE_COUNT)
        voiceIndex += ORIGINFX_MELODIC_VOICE_COUNT;
    return voiceIndex;
}

static unsigned int OriginFxGetCarrierOffset(
    const SdlOriginFxPlayer *player, unsigned int voiceIndex)
{
    return g_abOriginFxCarrierOffsets[
        OriginFxGetOperatorTableIndex(player, voiceIndex)];
}

static unsigned int OriginFxGetModulatorOffset(
    const SdlOriginFxPlayer *player, unsigned int voiceIndex)
{
    return g_abOriginFxModulatorOffsets[
        OriginFxGetOperatorTableIndex(player, voiceIndex)];
}

static unsigned int OriginFxGetOplVoice(unsigned int voiceIndex)
{
    if (voiceIndex > 8)
        return 17 - voiceIndex;
    return voiceIndex;
}

static unsigned int OriginFxGetRhythmBit(unsigned int voiceIndex)
{
    if (voiceIndex < 6 || voiceIndex >= ORIGINFX_VOICE_STATE_COUNT)
        return 0;
    return g_abOriginFxRhythmBits[voiceIndex - 6];
}

static void OriginFxEnableRhythmMode(SdlOriginFxPlayer *player)
{
    unsigned int voiceIndex;

    if (player->melodicVoiceCount < ORIGINFX_MELODIC_VOICE_COUNT)
        return;
    OriginFxWriteRegister(player, 0xa6, 0);
    OriginFxWriteRegister(player, 0xb6, 0);
    OriginFxWriteRegister(player, 0xa7, 0);
    OriginFxWriteRegister(player, 0xb7, 0x0a);
    OriginFxWriteRegister(player, 0xa8, 0x54);
    OriginFxWriteRegister(player, 0xb8, 0x09);
    player->melodicVoiceCount = 6;
    player->rhythmRegister = 0x20;
    voiceIndex = 6;
    while (voiceIndex < ORIGINFX_VOICE_STATE_COUNT) {
        memset(&player->voices[voiceIndex], 0,
               sizeof(player->voices[voiceIndex]));
        voiceIndex++;
    }
    OriginFxWriteRegister(
        player, 0xbd, player->rhythmRegister);
}

static const unsigned char *OriginFxFindTimbre(
    const SdlOriginFxPlayer *player, unsigned int program)
{
    const unsigned char *timbre;
    unsigned int count;
    unsigned int index;

    count = player->timbres[0];
    timbre = player->timbres + 1;
    index = 0;
    while (index < count) {
        if (timbre[47] == program)
            return timbre;
        timbre += ORIGINFX_TIMBRE_SIZE;
        index++;
    }
    return player->timbres + 1;
}

static const unsigned char *OriginFxNextTimbre(
    const SdlOriginFxPlayer *player, const unsigned char *timbre)
{
    size_t offset;

    if (timbre[38] == 0)
        return 0;
    offset = (timbre - player->timbres);
    if (offset > player->timbreSize ||
        player->timbreSize - offset < ORIGINFX_TIMBRE_SIZE * 2U)
        return 0;
    return timbre + ORIGINFX_TIMBRE_SIZE;
}

static int OriginFxArithmeticShiftRight(int value,
                                           unsigned int count)
{
    uint64_t roundedMagnitude;
    unsigned int magnitude;

    if (count == 0)
        return value;
    if (count >= sizeof(value) * 8U)
        return value < 0 ? -1 : 0;
    if (value >= 0)
        return value >> count;
    magnitude = (unsigned int)(-(value + 1)) + 1U;
    roundedMagnitude = (uint64_t)magnitude +
        ((UINT64_C(1) << count) - 1U);
    return -(int)(roundedMagnitude >> count);
}

static unsigned int OriginFxClampTotalLevel(
    unsigned int registerValue, int totalLevel)
{
    if (totalLevel < 0)
        totalLevel = 0;
    else if (totalLevel > 0x3f)
        totalLevel = 0x3f;
    return (registerValue & 0xc0U) | (unsigned int)totalLevel;
}

static unsigned int OriginFxCalculateVelocityLevel(
    unsigned int registerValue, unsigned int sensitivity,
    unsigned int velocity)
{
    int attenuation;
    int totalLevel;

    if (sensitivity == 0)
        return registerValue;
    if (sensitivity > 7)
        sensitivity = 7;
    attenuation = OriginFxArithmeticShiftRight(
        63 - (int)velocity, 7 - sensitivity);
    totalLevel = (int)(registerValue & 0x3fU) + attenuation;
    return OriginFxClampTotalLevel(registerValue, totalLevel);
}

static unsigned int OriginFxCalculateCarrierLevel(
    unsigned int registerValue, unsigned int sensitivity,
    unsigned int velocity, unsigned int volume)
{
    int attenuation;
    int totalLevel;

    if (sensitivity == 0 && volume >= 0x100U)
        return registerValue;
    if (sensitivity > 7)
        sensitivity = 7;
    attenuation = OriginFxArithmeticShiftRight(
        63 - (int)velocity, 7 - sensitivity);
    totalLevel = (int)(registerValue & 0x3fU) + attenuation;
    totalLevel = 63 - OriginFxArithmeticShiftRight(
        (int)volume * (63 - totalLevel), 8);
    return OriginFxClampTotalLevel(registerValue, totalLevel);
}

static unsigned int OriginFxCalculatePannedLevel(
    unsigned int registerValue, unsigned int pan, int rightChannel)
{
    unsigned int scale;
    unsigned int divisor;
    int totalLevel;

    if (pan > 127)
        pan = 127;
    if (rightChannel != 0) {
        if (pan >= 64)
            return registerValue;
        scale = pan;
        divisor = 64;
    } else {
        if (pan <= 64)
            return registerValue;
        scale = 127 - pan;
        divisor = 63;
    }
    totalLevel = (registerValue & 0x3fU);
    totalLevel = 63 - (int)(scale * (63 - totalLevel) / divisor);
    return OriginFxClampTotalLevel(registerValue, totalLevel);
}

static void OriginFxWriteVoiceLevels(SdlOriginFxPlayer *player,
                                        unsigned int voiceIndex)
{
    OriginFxVoice *voice;
    OriginFxChannel *channel;
    const unsigned char *timbre;
    unsigned int carrierOffset;
    unsigned int carrierLevel;
    unsigned int modulatorOffset;
    unsigned int modulatorLevel;

    voice = &player->voices[voiceIndex];
    channel = &player->channels[voice->channel];
    timbre = voice->timbre;
    carrierOffset = OriginFxGetCarrierOffset(player, voiceIndex);
    modulatorOffset = OriginFxGetModulatorOffset(player, voiceIndex);
    if (timbre[12] != 0 || channel->volume < 0x100U) {
        carrierLevel = OriginFxCalculateCarrierLevel(
            timbre[6], timbre[12], voice->velocity, channel->volume);
        OriginFxWriteStereoRegister(
            player, 0x40 + carrierOffset,
            OriginFxCalculatePannedLevel(
                carrierLevel, channel->pan, 0),
            OriginFxCalculatePannedLevel(
                carrierLevel, channel->pan, 1));
    }
    if (timbre[13] != 0 ||
        (player->stereoPanningEnabled != 0 &&
         (timbre[10] & 1U) != 0)) {
        if (timbre[13] != 0) {
            modulatorLevel = OriginFxCalculateVelocityLevel(
                timbre[1], timbre[13], voice->velocity);
        } else {
            modulatorLevel = timbre[1];
        }
        if ((timbre[10] & 1U) != 0) {
            OriginFxWriteStereoRegister(
                player, 0x40 + modulatorOffset,
                OriginFxCalculatePannedLevel(
                    modulatorLevel, channel->pan, 0),
                OriginFxCalculatePannedLevel(
                    modulatorLevel, channel->pan, 1));
        } else {
            OriginFxWriteRegister(
                player, 0x40 + modulatorOffset, modulatorLevel);
        }
    }
}

static void OriginFxWriteVoiceFrequency(SdlOriginFxPlayer *player,
                                           unsigned int voiceIndex,
                                           int keyOn)
{
    static const unsigned short frequencyNumbers[13] = {
        485, 514, 544, 577, 611, 647, 686,
        727, 770, 816, 864, 915, 970
    };
    OriginFxVoice *voice;
    OriginFxChannel *channel;
    int pitch;
    int note;
    int fraction;
    int tableIndex;
    int block;
    int frequency;
    int bend;
    int keyPitch;
    int keyTracking;
    unsigned int trackingShift;
    unsigned int oplVoice;
    const unsigned char *channelTimbre;

    voice = &player->voices[voiceIndex];
    channel = &player->channels[voice->channel];
    bend = channel->pitchBend - 0x2000;
    keyPitch = (voice->note - 60) * 256;
    keyTracking = (signed char)voice->timbre[39];
    if (keyTracking < 0) {
        keyPitch = -keyPitch;
        trackingShift = (unsigned char)~keyTracking;
    } else {
        trackingShift = keyTracking;
    }
    keyPitch = OriginFxArithmeticShiftRight(
        keyPitch, trackingShift) + 60 * 256;
    channelTimbre = OriginFxFindTimbre(
        player, channel->program);
    pitch = keyPitch +
        voice->envelopePitch +
        OriginFxReadLittleEndian16(voice->timbre + 36) +
        voice->modulationPitch +
        OriginFxArithmeticShiftRight(
            bend * channelTimbre[14], 8);
    note = pitch / 256;
    fraction = pitch % 256;
    if (fraction < 0) {
        fraction += 256;
        note--;
    }
    tableIndex = (note + 6) % 12;
    if (tableIndex < 0)
        tableIndex += 12;
    block = (note + 6) / 12 - 2;
    if (note + 6 < 0 && (note + 6) % 12 != 0)
        block--;
    if (block < 0)
        block = 0;
    else if (block > 7)
        block = 7;
    frequency = frequencyNumbers[tableIndex] +
        (frequencyNumbers[tableIndex + 1] -
         frequencyNumbers[tableIndex]) * fraction / 256;
    if (frequency > 0x3ff)
        frequency = 0x3ff;

    voice->frequencyHigh = (
        ((frequency >> 8) & 3) | (block << 2));
    oplVoice = OriginFxGetOplVoice(voiceIndex);
    OriginFxWriteRegister(player, 0xa0 + oplVoice, frequency & 0xff);
    OriginFxWriteRegister(
        player, 0xb0 + oplVoice,
        voice->frequencyHigh | (keyOn != 0 ? 0x20 : 0));
}

static void OriginFxProgramVoice(SdlOriginFxPlayer *player,
                                    unsigned int voiceIndex)
{
    const unsigned char *timbre;
    unsigned int carrierOffset;
    unsigned int modulatorOffset;
    unsigned int oplVoice;

    timbre = player->voices[voiceIndex].timbre;
    carrierOffset = OriginFxGetCarrierOffset(player, voiceIndex);
    modulatorOffset = OriginFxGetModulatorOffset(player, voiceIndex);
    oplVoice = OriginFxGetOplVoice(voiceIndex);
    OriginFxWriteRegister(
        player, 0x20 + modulatorOffset, timbre[0]);
    OriginFxWriteRegister(
        player, 0x40 + modulatorOffset, timbre[1]);
    OriginFxWriteRegister(
        player, 0x60 + modulatorOffset, timbre[2]);
    OriginFxWriteRegister(
        player, 0x80 + modulatorOffset, timbre[3]);
    OriginFxWriteRegister(
        player, 0xe0 + modulatorOffset, timbre[4]);
    if (player->melodicVoiceCount == ORIGINFX_MELODIC_VOICE_COUNT ||
        timbre[11] < 7) {
        OriginFxWriteRegister(
            player, 0x20 + carrierOffset, timbre[5]);
        OriginFxWriteRegister(
            player, 0x40 + carrierOffset, timbre[6]);
        OriginFxWriteRegister(
            player, 0x60 + carrierOffset, timbre[7]);
        OriginFxWriteRegister(
            player, 0x80 + carrierOffset, timbre[8]);
        OriginFxWriteRegister(
            player, 0xe0 + carrierOffset, timbre[9]);
        OriginFxWriteRegister(
            player, 0xc0 + oplVoice, timbre[10]);
    }
    OriginFxWriteVoiceLevels(player, voiceIndex);
}

static unsigned int OriginFxChooseVoice(SdlOriginFxPlayer *player)
{
    unsigned int oldestFreeVoice;
    unsigned int oldestVoice;
    unsigned int voiceIndex;

    oldestFreeVoice = player->melodicVoiceCount;
    oldestVoice = 0;
    voiceIndex = 0;
    while (voiceIndex < player->melodicVoiceCount) {
        if (player->voices[voiceIndex].active == 0) {
            if (oldestFreeVoice == player->melodicVoiceCount ||
                player->voices[voiceIndex].age <
                    player->voices[oldestFreeVoice].age)
                oldestFreeVoice = voiceIndex;
        } else if (player->voices[voiceIndex].age <
                   player->voices[oldestVoice].age ||
                   player->voices[oldestVoice].active == 0) {
            oldestVoice = voiceIndex;
        }
        voiceIndex++;
    }
    if (oldestFreeVoice != player->melodicVoiceCount)
        return oldestFreeVoice;
    return oldestVoice;
}

static void OriginFxStartTimbre(SdlOriginFxPlayer *player,
                                   unsigned int channelIndex,
                                   unsigned int note,
                                   unsigned int velocity,
                                   const unsigned char *timbre)
{
    OriginFxVoice *voice;
    unsigned int voiceIndex;
    unsigned int oplVoice;
    unsigned int rhythmBit;

    if (timbre[11] != 0) {
        OriginFxEnableRhythmMode(player);
        voiceIndex = timbre[11];
        if (voiceIndex < 6 ||
            voiceIndex >= ORIGINFX_VOICE_STATE_COUNT)
            return;
        rhythmBit = OriginFxGetRhythmBit(voiceIndex);
        player->rhythmRegister = (
            player->rhythmRegister & ~rhythmBit);
        OriginFxWriteRegister(
            player, 0xbd, player->rhythmRegister);
    } else {
        voiceIndex = OriginFxChooseVoice(player);
        rhythmBit = 0;
    }
    voice = &player->voices[voiceIndex];
    if (voice->active != 0 && rhythmBit == 0) {
        oplVoice = OriginFxGetOplVoice(voiceIndex);
        OriginFxWriteRegister(player, 0xa0 + oplVoice, 0);
        OriginFxWriteRegister(player, 0xb0 + oplVoice, 0);
    }
    voice->timbre = timbre;
    voice->age = player->nextVoiceAge++;
    voice->channel = channelIndex;
    voice->note = note;
    voice->velocity = velocity;
    voice->envelopeState = 2;
    voice->modulationPhase = 0;
    voice->envelopePitch =
        OriginFxReadLittleEndian16(timbre + 18);
    voice->modulationPitch = 0;
    voice->active = 1;
    OriginFxProgramVoice(player, voiceIndex);
    if (rhythmBit == 0) {
        OriginFxWriteVoiceFrequency(player, voiceIndex, 1);
    } else {
        if (voiceIndex == 6)
            OriginFxWriteVoiceFrequency(player, voiceIndex, 0);
        player->rhythmRegister = (
            player->rhythmRegister | rhythmBit);
        OriginFxWriteRegister(
            player, 0xbd, player->rhythmRegister);
    }
}

static void OriginFxNoteOn(SdlOriginFxPlayer *player,
                              unsigned int channelIndex,
                              unsigned int note,
                              unsigned int velocity)
{
    const unsigned char *timbre;

    timbre = OriginFxFindTimbre(
        player, player->channels[channelIndex].program);
    while (timbre != 0) {
        OriginFxStartTimbre(
            player, channelIndex, note, velocity, timbre);
        timbre = OriginFxNextTimbre(player, timbre);
    }
}

static void OriginFxStopTimbre(SdlOriginFxPlayer *player,
                                  unsigned int channelIndex,
                                  unsigned int note,
                                  const unsigned char *timbre)
{
    unsigned int voiceIndex;
    unsigned int rhythmBit;
    unsigned int oplVoice;

    voiceIndex = 0;
    while (voiceIndex < ORIGINFX_VOICE_STATE_COUNT) {
        if (player->voices[voiceIndex].active != 0 &&
            player->voices[voiceIndex].channel == channelIndex &&
            player->voices[voiceIndex].note == note &&
            player->voices[voiceIndex].timbre == timbre) {
            rhythmBit = OriginFxGetRhythmBit(voiceIndex);
            if (timbre[11] != 0 && rhythmBit != 0) {
                player->rhythmRegister = (
                    player->rhythmRegister & ~rhythmBit);
                OriginFxWriteRegister(
                    player, 0xbd, player->rhythmRegister);
            } else {
                oplVoice = OriginFxGetOplVoice(voiceIndex);
                OriginFxWriteRegister(
                    player, 0xb0 + oplVoice,
                    player->voices[voiceIndex].frequencyHigh);
            }
            player->voices[voiceIndex].active = 0;
            player->voices[voiceIndex].envelopeState = 0;
            player->voices[voiceIndex].age = player->nextVoiceAge++;
            return;
        }
        voiceIndex++;
    }
}

static void OriginFxNoteOff(SdlOriginFxPlayer *player,
                               unsigned int channelIndex,
                               unsigned int note)
{
    const unsigned char *timbre;

    timbre = OriginFxFindTimbre(
        player, player->channels[channelIndex].program);
    while (timbre != 0) {
        OriginFxStopTimbre(
            player, channelIndex, note, timbre);
        timbre = OriginFxNextTimbre(player, timbre);
    }
}

static void OriginFxAllNotesOff(SdlOriginFxPlayer *player,
                                   int channelIndex)
{
    unsigned int voiceIndex;
    unsigned int rhythmBit;
    unsigned int oplVoice;

    voiceIndex = 0;
    while (voiceIndex < ORIGINFX_VOICE_STATE_COUNT) {
        if (player->voices[voiceIndex].active != 0 &&
            (channelIndex < 0 ||
             player->voices[voiceIndex].channel == channelIndex)) {
            rhythmBit = OriginFxGetRhythmBit(voiceIndex);
            if (player->voices[voiceIndex].timbre != 0 &&
                player->voices[voiceIndex].timbre[11] != 0 &&
                rhythmBit != 0) {
                player->rhythmRegister = (
                    player->rhythmRegister & ~rhythmBit);
                OriginFxWriteRegister(
                    player, 0xbd, player->rhythmRegister);
            } else {
                oplVoice = OriginFxGetOplVoice(voiceIndex);
                OriginFxWriteRegister(
                    player, 0xb0 + oplVoice,
                    player->voices[voiceIndex].frequencyHigh);
            }
            player->voices[voiceIndex].active = 0;
            player->voices[voiceIndex].envelopeState = 0;
            player->voices[voiceIndex].age = player->nextVoiceAge++;
        }
        voiceIndex++;
    }
    if (channelIndex < 0 &&
        player->melodicVoiceCount < ORIGINFX_MELODIC_VOICE_COUNT) {
        player->rhythmRegister = 0;
        player->melodicVoiceCount = ORIGINFX_MELODIC_VOICE_COUNT;
        OriginFxWriteRegister(player, 0xbd, 0);
    }
}

static void OriginFxDisableRhythmModeIfIdle(
    SdlOriginFxPlayer *player)
{
    unsigned int voiceIndex;

    if (player->melodicVoiceCount == ORIGINFX_MELODIC_VOICE_COUNT)
        return;
    voiceIndex = player->melodicVoiceCount;
    while (voiceIndex < ORIGINFX_VOICE_STATE_COUNT) {
        if (player->voices[voiceIndex].active != 0)
            return;
        voiceIndex++;
    }
    player->rhythmRegister = 0;
    OriginFxWriteRegister(player, 0xbd, 0);
    player->melodicVoiceCount = ORIGINFX_MELODIC_VOICE_COUNT;
}

static void OriginFxUpdateChannelVoices(
    SdlOriginFxPlayer *player, unsigned int channelIndex,
    int updateFrequency, int updateLevel)
{
    unsigned int voiceIndex;

    voiceIndex = 0;
    while (voiceIndex < player->melodicVoiceCount) {
        if (player->voices[voiceIndex].active != 0 &&
            player->voices[voiceIndex].channel == channelIndex) {
            if (updateFrequency != 0)
                OriginFxWriteVoiceFrequency(player, voiceIndex, 1);
            if (updateLevel != 0)
                OriginFxWriteVoiceLevels(player, voiceIndex);
        }
        voiceIndex++;
    }
}

static void OriginFxSetProgram(SdlOriginFxPlayer *player,
                                  unsigned int channelIndex,
                                  unsigned int program)
{
    OriginFxChannel *channel;
    const unsigned char *timbre;
    unsigned int voiceIndex;

    OriginFxAllNotesOff(player, (int)channelIndex);
    voiceIndex = 0;
    while (voiceIndex < ORIGINFX_VOICE_STATE_COUNT) {
        if (player->voices[voiceIndex].timbre != 0 &&
            player->voices[voiceIndex].channel == channelIndex) {
            player->voices[voiceIndex].timbre = 0;
            player->voices[voiceIndex].channel = 0xff;
        }
        voiceIndex++;
    }
    channel = &player->channels[channelIndex];
    channel->program = program;
    timbre = OriginFxFindTimbre(player, program);
    channel->modulationRate = timbre[16];
    channel->modulationDepth = timbre[17];
    if (timbre[11] != 0)
        OriginFxEnableRhythmMode(player);
}

static void OriginFxService(SdlOriginFxPlayer *player)
{
    static const unsigned char envelopeRateOffsets[5] = {
        32, 0, 20, 24, 28
    };
    static const unsigned char envelopeTargetOffsets[5] = {
        34, 0, 22, 26, 30
    };
    OriginFxVoice *voice;
    OriginFxChannel *channel;
    const unsigned char *timbre;
    unsigned int voiceIndex;
    unsigned int state;
    unsigned int rate;
    unsigned int phase;
    int target;
    int distance;
    int triangle;
    int changed;

    voiceIndex = 0;
    while (voiceIndex < player->melodicVoiceCount) {
        voice = &player->voices[voiceIndex];
        timbre = voice->timbre;
        if (timbre == 0 || voice->channel >= ORIGINFX_CHANNEL_COUNT) {
            voiceIndex++;
            continue;
        }
        changed = 0;
        state = voice->envelopeState;
        if (state < sizeof(envelopeTargetOffsets) &&
            envelopeTargetOffsets[state] != 0) {
            target = OriginFxReadLittleEndian16(
                timbre + envelopeTargetOffsets[state]);
            rate = (uint16_t)
                OriginFxReadLittleEndian16(
                    timbre + envelopeRateOffsets[state]);
            distance = target - voice->envelopePitch;
            if (distance < 0)
                distance = -distance;
            if ((unsigned int)distance < rate) {
                voice->envelopePitch = target;
                voice->envelopeState++;
            } else if (voice->envelopePitch < target) {
                voice->envelopePitch += (int)rate;
            } else {
                voice->envelopePitch -= (int)rate;
            }
            changed = 1;
        }

        channel = &player->channels[voice->channel];
        if (channel->modulationRate != 0) {
            phase = (voice->modulationPhase +
                     channel->modulationRate) & 0xffU;
            voice->modulationPhase = phase;
            triangle = phase < 0x80U ? (int)phase : (int)phase - 0x100;
            if (triangle > 63 || triangle < -64) {
                phase = (0x80U - phase) & 0xffU;
                triangle = phase < 0x80U
                    ? (int)phase : (int)phase - 0x100;
            }
            voice->modulationPitch = OriginFxArithmeticShiftRight(
                channel->modulationDepth * triangle, 4);
            changed = 1;
        }
        if (changed != 0)
            OriginFxWriteVoiceFrequency(
                player, voiceIndex, voice->envelopeState > 1);
        voiceIndex++;
    }
}

static void OriginFxServiceSoundEffects(
    SdlOriginFxPlayer *player);

static void OriginFxAdvanceService(SdlOriginFxPlayer *player)
{
    player->serviceAccumulator += ORIGINFX_SERVICE_RATE;
    while (player->serviceAccumulator >= ORIGINFX_OUTPUT_RATE) {
        player->serviceAccumulator -= ORIGINFX_OUTPUT_RATE;
        OriginFxServiceSoundEffects(player);
        OriginFxService(player);
    }
}

static int OriginFxMapPercussionNote(unsigned int note,
                                        unsigned int *channelIndex,
                                        unsigned int *mappedNote)
{
    unsigned int channel;

    if (note >= ORIGINFX_PERCUSSION_NOTE_COUNT)
        return 0;
    channel = g_abOriginFxPercussionChannels[note];
    if (channel == 0)
        return 0;
    *channelIndex = channel - 1;
    *mappedNote = g_abOriginFxPercussionPitches[note];
    return 1;
}

static void OriginFxApplyControlChange(
    SdlOriginFxPlayer *player, unsigned int channelIndex,
    unsigned int controller, unsigned int value)
{
    OriginFxChannel *channel;
    const unsigned char *timbre;

    channel = &player->channels[channelIndex];
    if (controller == 1) {
        timbre = OriginFxFindTimbre(player, channel->program);
        channel->modulationDepth = (
            ((unsigned int)timbre[15] * value >> 7) + timbre[17]);
    } else if (controller == 7) {
        channel->volume = (value + 0x80U);
    } else if (controller == 10 && player->stereoPanningEnabled != 0) {
        if (value > 127)
            value = 127;
        channel->pan = value;
        OriginFxUpdateChannelVoices(
            player, channelIndex, 0, 1);
    } else if (controller == 123) {
        OriginFxAllNotesOff(player, (int)channelIndex);
        OriginFxDisableRhythmModeIfIdle(player);
    } else if (controller == 121) {
        timbre = OriginFxFindTimbre(player, channel->program);
        channel->modulationDepth = timbre[17];
        channel->volume = 0xff;
        channel->pitchBend = 0x2000;
        OriginFxUpdateChannelVoices(
            player, channelIndex, 1, 0);
    }
}

static void OriginFxControlChange(SdlOriginFxPlayer *player,
                                     unsigned int channelIndex,
                                     unsigned int controller,
                                     unsigned int value)
{
    unsigned int percussionChannel;

    if (channelIndex == 9) {
        percussionChannel = 10;
        while (percussionChannel < ORIGINFX_CHANNEL_COUNT) {
            OriginFxApplyControlChange(
                player, percussionChannel, controller, value);
            percussionChannel++;
        }
    }
    OriginFxApplyControlChange(
        player, channelIndex, controller, value);
}

static void OriginFxStopSoundEffect(
    SdlOriginFxPlayer *player, unsigned int effectIndex,
    int releaseChannel)
{
    OriginFxSoundEffect *effect;

    effect = &player->soundEffects[effectIndex];
    if (effect->active == 0)
        return;
    if (releaseChannel != 0) {
        OriginFxControlChange(
            player, effect->channel, 123, 0);
    } else {
        OriginFxNoteOff(
            player, effect->channel, effect->currentNote);
    }
    effect->active = 0;
    effect->record = 0;
}

static int OriginFxStartSoundEffectRecord(
    SdlOriginFxPlayer *player, OriginFxSoundEffect *effect)
{
    const unsigned char *record;
    unsigned int program;

    record = effect->record;
    if (record == 0 || record[1] == 0) {
        effect->active = 0;
        effect->record = 0;
        return 0;
    }
    program = record[1] - 1U;
    OriginFxSetProgram(player, effect->channel, program);
    OriginFxControlChange(
        player, effect->channel, 7, effect->volume);
    OriginFxControlChange(
        player, effect->channel, 10, effect->pan);
    effect->currentNote = record[2];
    effect->remainingTicks = (
        record[4] | ((unsigned int)record[5] << 8));
    OriginFxNoteOn(
        player, effect->channel, effect->currentNote, record[3]);
    return 1;
}

static int OriginFxChooseSoundEffectChannel(
    SdlOriginFxPlayer *player, int priority)
{
    int channelUsed[ORIGINFX_SOUND_CHANNEL_COUNT];
    int victim;
    unsigned int effectIndex;
    unsigned int channel;

    memset(channelUsed, 0, sizeof(channelUsed));
    effectIndex = 0;
    while (effectIndex < ORIGINFX_SOUND_SLOT_COUNT) {
        if (player->soundEffects[effectIndex].active != 0) {
            channel = player->soundEffects[effectIndex].channel -
                ORIGINFX_FIRST_SOUND_CHANNEL;
            if (channel < ORIGINFX_SOUND_CHANNEL_COUNT)
                channelUsed[channel] = 1;
        }
        effectIndex++;
    }
    channel = 0;
    while (channel < ORIGINFX_SOUND_CHANNEL_COUNT) {
        if (channelUsed[channel] == 0)
            return (channel + ORIGINFX_FIRST_SOUND_CHANNEL);
        channel++;
    }

    victim = -1;
    effectIndex = 0;
    while (effectIndex < ORIGINFX_SOUND_SLOT_COUNT) {
        if (player->soundEffects[effectIndex].active != 0 &&
            player->soundEffects[effectIndex].priority <= priority &&
            (victim < 0 ||
             player->soundEffects[effectIndex].age <
                 player->soundEffects[victim].age))
            victim = effectIndex;
        effectIndex++;
    }
    if (victim < 0)
        return -1;
    channel = player->soundEffects[victim].channel;
    OriginFxStopSoundEffect(player, (unsigned int)victim, 0);
    return channel;
}

static void OriginFxServiceSoundEffects(
    SdlOriginFxPlayer *player)
{
    OriginFxSoundEffect *effect;
    const unsigned char *record;
    const unsigned char *recordsBegin;
    const unsigned char *recordsEnd;
    unsigned int effectIndex;
    unsigned int flags;
    unsigned int targetNote;
    int restart;

    recordsBegin = &g_aabOriginFxSoundRecords[0][0];
    recordsEnd = recordsBegin + sizeof(g_aabOriginFxSoundRecords);
    effectIndex = 0;
    while (effectIndex < ORIGINFX_SOUND_SLOT_COUNT) {
        effect = &player->soundEffects[effectIndex];
        record = effect->record;
        if (effect->active == 0 || record == 0) {
            effectIndex++;
            continue;
        }
        flags = record[0];
        if ((flags & 4U) != 0) {
            effectIndex++;
            continue;
        }
        effect->remainingTicks--;
        if (effect->remainingTicks != 0) {
            effectIndex++;
            continue;
        }

        OriginFxNoteOff(
            player, effect->channel, effect->currentNote);
        restart = 0;
        if ((flags & 2U) != 0) {
            targetNote = record[6];
            if (targetNote > effect->currentNote) {
                effect->currentNote++;
                restart = 1;
            } else if (targetNote < effect->currentNote) {
                effect->currentNote--;
                restart = 1;
            } else {
                effect->currentNote = record[2];
            }
        } else {
            effect->currentNote = record[2];
        }
        if ((flags & 8U) != 0)
            restart = 1;
        if (restart != 0) {
            OriginFxNoteOn(
                player, effect->channel, effect->currentNote, record[3]);
            effect->remainingTicks = (
                record[4] | ((unsigned int)record[5] << 8));
        } else if ((flags & 1U) != 0 &&
                   record + 8 < recordsEnd &&
                   record + 8 >= recordsBegin) {
            effect->record = record + 8;
            OriginFxStartSoundEffectRecord(player, effect);
        } else {
            effect->active = 0;
            effect->record = 0;
            OriginFxControlChange(
                player, effect->channel, 123, 0);
        }
        effectIndex++;
    }
}

static void OriginFxDispatchEvent(SdlOriginFxPlayer *player,
                                     const OriginFxEvent *event)
{
    OriginFxChannel *channel;
    unsigned int channelIndex;
    unsigned int command;
    unsigned int note;

    if (event->type == ORIGINFX_SEQUENCE_EVENT) {
        player->sequencePosition = event->data1;
        return;
    }
    channelIndex = event->status & 0x0fU;
    command = event->status & 0xf0U;
    channel = &player->channels[channelIndex];
    switch (command) {
    case 0x80:
        note = event->data1;
        if (channelIndex != 9 || OriginFxMapPercussionNote(
                note, &channelIndex, &note))
            OriginFxNoteOff(player, channelIndex, note);
        break;
    case 0x90:
        note = event->data1;
        if (channelIndex != 9 || OriginFxMapPercussionNote(
                note, &channelIndex, &note)) {
            if (event->data2 == 0)
                OriginFxNoteOff(player, channelIndex, note);
            else
                OriginFxNoteOn(
                    player, channelIndex, note, event->data2);
        }
        break;
    case 0xb0:
        OriginFxControlChange(
            player, channelIndex, event->data1, event->data2);
        break;
    case 0xc0:
        OriginFxSetProgram(player, channelIndex, event->data1);
        break;
    case 0xe0:
        channel->pitchBend = (
            event->data1 | ((unsigned int)event->data2 << 7));
        OriginFxUpdateChannelVoices(player, channelIndex, 1, 0);
        break;
    }
}

static void OriginFxProcessDueEvents(SdlOriginFxPlayer *player)
{
    while (player->nextEvent < player->eventCount &&
           player->events[player->nextEvent].frame <=
               player->currentFrame) {
        OriginFxDispatchEvent(
            player, &player->events[player->nextEvent]);
        player->nextEvent++;
    }
}

static void OriginFxGenerateOutputSample(
    SdlOriginFxPlayer *player, int32_t *leftSample,
    int32_t *rightSample)
{
    ymfm::ym3812::output_data leftOutput;
    ymfm::ym3812::output_data rightOutput;
    int64_t leftSampleTotal;
    int64_t rightSampleTotal;
    unsigned int sampleCount;

    leftSampleTotal = 0;
    rightSampleTotal = 0;
    sampleCount = 0;
    player->nativeSampleAccumulator += player->nativeSampleRate;
    while (player->nativeSampleAccumulator >= ORIGINFX_OUTPUT_RATE) {
        player->oplChip.generate(&leftOutput);
        player->lastNativeSample = leftOutput.data[0];
        if (player->stereoPanningEnabled != 0) {
            player->oplRightChip.generate(&rightOutput);
            player->lastNativeRightSample = rightOutput.data[0];
        } else {
            player->lastNativeRightSample = player->lastNativeSample;
        }
        leftSampleTotal += player->lastNativeSample;
        rightSampleTotal += player->lastNativeRightSample;
        sampleCount++;
        player->nativeSampleAccumulator -= ORIGINFX_OUTPUT_RATE;
    }
    if (sampleCount == 0) {
        *leftSample = player->lastNativeSample;
        *rightSample = player->lastNativeRightSample;
    } else {
        *leftSample = (leftSampleTotal / sampleCount);
        *rightSample = (rightSampleTotal / sampleCount);
    }
}

static short OriginFxScaleOutputSample(int32_t sample,
                                         unsigned int gain)
{
    int64_t scaled;

    if (gain > 0x7fffU)
        gain = 0x7fffU;
    scaled = (int64_t)sample * gain / 0x7fff;
    if (scaled < -32768)
        scaled = -32768;
    else if (scaled > 32767)
        scaled = 32767;
    return scaled;
}

static short OriginFxMixOutputSample(short destination, short source)
{
    int mixed;

    mixed = destination + source;
    if (mixed < -32768)
        mixed = -32768;
    else if (mixed > 32767)
        mixed = 32767;
    return mixed;
}

static int OriginFxInitializePlayer(
    SdlOriginFxPlayer *player,
    const unsigned char *timbres, size_t timbreSize)
{
    unsigned int channelIndex;
    unsigned int percussionIndex;
    unsigned char defaultProgram;

    if (!OriginFxLoadTimbres(player, timbres, timbreSize))
        return 0;
    OriginFxResetOpl(player);
    defaultProgram = player->timbres[1 + 47];
    channelIndex = 0;
    while (channelIndex < ORIGINFX_CHANNEL_COUNT) {
        player->channels[channelIndex].pitchBend = 0x2000;
        player->channels[channelIndex].volume = 0xff;
        player->channels[channelIndex].pan = 64;
        OriginFxSetProgram(player, channelIndex, defaultProgram);
        channelIndex++;
    }
    percussionIndex = 0;
    while (percussionIndex <
           sizeof(g_abOriginFxPercussionPrograms) /
               sizeof(g_abOriginFxPercussionPrograms[0])) {
        OriginFxSetProgram(
            player, 9 + percussionIndex,
            g_abOriginFxPercussionPrograms[percussionIndex]);
        percussionIndex++;
    }
    return 1;
}

SdlOriginFxPlayer *SdlCreateOriginFxPlayer(
    const unsigned char *midi, size_t midiSize,
    const unsigned char *timbres, size_t timbreSize)
{
    SdlOriginFxPlayer *player;

    player = new (std::nothrow) SdlOriginFxPlayer;
    if (player == 0)
        return 0;
    if (!OriginFxInitializePlayer(
            player, timbres, timbreSize) ||
        !OriginFxLoadMidi(player, midi, midiSize)) {
        delete player;
        return 0;
    }
    return player;
}

SdlOriginFxPlayer *SdlCreateOriginFxSoundPlayer(
    const unsigned char *timbres, size_t timbreSize)
{
    SdlOriginFxPlayer *player;

    player = new (std::nothrow) SdlOriginFxPlayer;
    if (player == 0)
        return 0;
    if (!OriginFxInitializePlayer(player, timbres, timbreSize)) {
        delete player;
        return 0;
    }
    player->stereoPanningEnabled = 1;
    return player;
}

int SdlPlayOriginFxSoundEffect(
    SdlOriginFxPlayer *player, unsigned int soundNumber,
    int volume, int pan, int tag, int priority)
{
    OriginFxSoundEffect *effect;
    uint64_t channelAge;
    unsigned int effectIndex;
    unsigned int freeEffectIndex;
    int channel;
    int channelPriority;

    if (player == 0 || soundNumber == 0 ||
        soundNumber > sizeof(g_aabOriginFxSoundRecords) /
            sizeof(g_aabOriginFxSoundRecords[0]))
        return 0;
    if (volume < 0)
        volume = 0;
    else if (volume > 127)
        volume = 127;
    if (pan < 0)
        pan = 0;
    else if (pan > 127)
        pan = 127;

    channel = -1;
    channelAge = 0;
    channelPriority = priority;
    freeEffectIndex = ORIGINFX_SOUND_SLOT_COUNT;
    effectIndex = 0;
    while (effectIndex < ORIGINFX_SOUND_SLOT_COUNT) {
        effect = &player->soundEffects[effectIndex];
        if (effect->active != 0 && effect->tag == tag) {
            channel = effect->channel;
            channelAge = effect->age;
            channelPriority = effect->priority;
            OriginFxStopSoundEffect(player, effectIndex, 0);
            freeEffectIndex = effectIndex;
            break;
        }
        if (effect->active == 0)
            freeEffectIndex = effectIndex;
        effectIndex++;
    }
    if (channel < 0)
        channel = OriginFxChooseSoundEffectChannel(player, priority);
    if (channel < 0)
        return 0;

    if (freeEffectIndex == ORIGINFX_SOUND_SLOT_COUNT)
        return 0;
    effect = &player->soundEffects[freeEffectIndex];
    memset(effect, 0, sizeof(*effect));
    effect->record = g_aabOriginFxSoundRecords[soundNumber - 1];
    if (channelAge != 0) {
        effect->age = channelAge;
    } else {
        effect->age = player->nextSoundEffectAge++;
    }
    effect->tag = tag;
    effect->priority = channelPriority;
    effect->channel = channel;
    effect->volume = volume;
    effect->pan = pan;
    effect->active = 1;
    return OriginFxStartSoundEffectRecord(player, effect);
}

void SdlStopOriginFxSoundEffects(SdlOriginFxPlayer *player)
{
    unsigned int effectIndex;

    if (player == 0)
        return;
    effectIndex = 0;
    while (effectIndex < ORIGINFX_SOUND_SLOT_COUNT) {
        OriginFxStopSoundEffect(player, effectIndex, 1);
        effectIndex++;
    }
    OriginFxAllNotesOff(player, -1);
}

void SdlMixOriginFxSoundEffects(
    SdlOriginFxPlayer *player, short *samples,
    unsigned int frameCount, unsigned int gain)
{
    int32_t generatedLeft;
    int32_t generatedRight;
    short leftOutput;
    short rightOutput;
    unsigned int frame;

    if (player == 0 || samples == 0)
        return;
    frame = 0;
    while (frame < frameCount) {
        OriginFxAdvanceService(player);
        OriginFxGenerateOutputSample(
            player, &generatedLeft, &generatedRight);
        leftOutput = OriginFxScaleOutputSample(generatedLeft, gain);
        rightOutput = OriginFxScaleOutputSample(generatedRight, gain);
        samples[frame * 2] = OriginFxMixOutputSample(
            samples[frame * 2], leftOutput);
        samples[frame * 2 + 1] = OriginFxMixOutputSample(
            samples[frame * 2 + 1], rightOutput);
        player->currentFrame++;
        frame++;
    }
}

void SdlDestroyOriginFxPlayer(SdlOriginFxPlayer *player)
{
    delete player;
}

int SdlOriginFxPlayerFinished(const SdlOriginFxPlayer *player)
{
    return player == 0 || player->finished != 0;
}

unsigned int SdlOriginFxPlayerSequencePosition(
    const SdlOriginFxPlayer *player)
{
    if (player == 0)
        return 0;
    return player->sequencePosition;
}

void SdlRenderOriginFxPlayer(SdlOriginFxPlayer *player,
                                short *samples,
                                unsigned int frameCount,
                                unsigned int gain)
{
    if (samples == 0)
        return;
    memset(samples, 0, (size_t)frameCount * sizeof(short) * 2);
    SdlMixOriginFxPlayer(player, samples, frameCount, gain);
}

void SdlMixOriginFxPlayer(SdlOriginFxPlayer *player,
                             short *samples,
                             unsigned int frameCount,
                             unsigned int gain)
{
    int32_t generatedLeft;
    int32_t generatedRight;
    short leftOutput;
    short rightOutput;
    unsigned int frame;

    if (samples == 0 || player == 0 || player->finished != 0)
        return;

    frame = 0;
    while (frame < frameCount) {
        OriginFxProcessDueEvents(player);
        if (player->nextEvent == player->eventCount &&
            player->currentFrame >= player->endFrame) {
            OriginFxAllNotesOff(player, -1);
            player->finished = 1;
            return;
        }
        OriginFxAdvanceService(player);
        OriginFxGenerateOutputSample(
            player, &generatedLeft, &generatedRight);
        leftOutput = OriginFxScaleOutputSample(generatedLeft, gain);
        rightOutput = OriginFxScaleOutputSample(generatedRight, gain);
        samples[frame * 2] = OriginFxMixOutputSample(
            samples[frame * 2], leftOutput);
        samples[frame * 2 + 1] = OriginFxMixOutputSample(
            samples[frame * 2 + 1], rightOutput);
        player->currentFrame++;
        frame++;
    }
    if (player->nextEvent == player->eventCount &&
        player->currentFrame >= player->endFrame) {
        OriginFxAllNotesOff(player, -1);
        player->finished = 1;
    }
}
