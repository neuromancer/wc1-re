/* SDL2-only, pragmatic OriginFX MIDI/AdLib replayer for WC1 DOS data. */
#include "wc1sdl.h"

#include "ymfm_opl.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <new>

#define WC1_ORIGINFX_OUTPUT_RATE 22050U
#define WC1_ORIGINFX_OPL_CLOCK 3579545U
#define WC1_ORIGINFX_SERVICE_RATE 60U
#define WC1_ORIGINFX_VOICE_COUNT 9
#define WC1_ORIGINFX_CHANNEL_COUNT 26
#define WC1_ORIGINFX_PERCUSSION_NOTE_COUNT 77
#define WC1_ORIGINFX_TIMBRE_SIZE 48U

/* WC.EXE 2231:7967 and 2231:79b4 translate MIDI channel 10 into
 * OriginFX's percussion pseudo-channels and their fixed pitches. */
static const unsigned char g_abWc1OriginFxPercussionChannels[
    WC1_ORIGINFX_PERCUSSION_NOTE_COUNT] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 10, 10, 18, 11, 0, 12, 13, 17, 13, 16, 13, 14, 13,
    13, 15, 13, 19, 0, 0, 0, 0, 21, 0, 0, 0, 26, 26, 25, 20,
    20, 0, 0, 21, 21, 22, 23, 0, 0, 24, 0, 20, 0
};

static const unsigned char g_abWc1OriginFxPercussionPitches[
    WC1_ORIGINFX_PERCUSSION_NOTE_COUNT] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 48, 48, 48, 48, 0, 48, 42, 71, 42, 71, 47, 71, 47,
    52, 79, 52, 77, 0, 0, 0, 0, 71, 0, 0, 0, 72, 79, 79, 64,
    58, 0, 0, 89, 84, 48, 72, 0, 0, 36, 0, 96, 0
};

/* WC.EXE 19c5:1682-174a selects these timbres for channels 10-26 when
 * STRAX.DRV's AdLib output is initialized. */
static const unsigned char g_abWc1OriginFxPercussionPrograms[17] = {
    0x80, 0x72, 0x83, 0x71, 0x86, 0x87, 0x85, 0x84, 0x81,
    0x88, 0x8d, 0x8f, 0x90, 0x91, 0x93, 0x8c, 0x8b
};

enum Wc1OriginFxEventType {
    WC1_ORIGINFX_CHANNEL_EVENT,
    WC1_ORIGINFX_TEMPO_EVENT
};

typedef struct Wc1OriginFxEvent {
    uint64_t tick;
    uint64_t frame;
    uint32_t order;
    uint32_t tempo;
    unsigned char type;
    unsigned char status;
    unsigned char data1;
    unsigned char data2;
} Wc1OriginFxEvent;

typedef struct Wc1OriginFxChannel {
    unsigned short pitchBend;
    unsigned short volume;
    unsigned short modulationDepth;
    unsigned char program;
    unsigned char modulationRate;
} Wc1OriginFxChannel;

typedef struct Wc1OriginFxVoice {
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
} Wc1OriginFxVoice;

class Wc1OriginFxYmfmInterface : public ymfm::ymfm_interface {
};

struct Wc1SdlOriginFxPlayer {
    Wc1OriginFxYmfmInterface oplInterface;
    ymfm::ym3812 oplChip;
    Wc1OriginFxEvent *events;
    unsigned char *timbres;
    size_t eventCount;
    size_t eventCapacity;
    size_t nextEvent;
    size_t timbreSize;
    uint64_t endFrame;
    uint64_t currentFrame;
    uint64_t nextVoiceAge;
    uint64_t nativeSampleAccumulator;
    uint64_t serviceAccumulator;
    uint32_t nativeSampleRate;
    int32_t lastNativeSample;
    int finished;
    Wc1OriginFxChannel channels[WC1_ORIGINFX_CHANNEL_COUNT];
    Wc1OriginFxVoice voices[WC1_ORIGINFX_VOICE_COUNT];

    Wc1SdlOriginFxPlayer() :
        oplChip(oplInterface),
        events(0),
        timbres(0),
        eventCount(0),
        eventCapacity(0),
        nextEvent(0),
        timbreSize(0),
        endFrame(0),
        currentFrame(0),
        nextVoiceAge(1),
        nativeSampleAccumulator(0),
        serviceAccumulator(0),
        nativeSampleRate(oplChip.sample_rate(WC1_ORIGINFX_OPL_CLOCK)),
        lastNativeSample(0),
        finished(0)
    {
        memset(channels, 0, sizeof(channels));
        memset(voices, 0, sizeof(voices));
    }

    ~Wc1SdlOriginFxPlayer()
    {
        free(events);
        free(timbres);
    }
};

static uint16_t Wc1OriginFxReadBigEndian16(const unsigned char *bytes)
{
    return (uint16_t)(((uint16_t)bytes[0] << 8) | bytes[1]);
}

static uint32_t Wc1OriginFxReadBigEndian32(const unsigned char *bytes)
{
    return ((uint32_t)bytes[0] << 24) |
        ((uint32_t)bytes[1] << 16) |
        ((uint32_t)bytes[2] << 8) |
        bytes[3];
}

static int16_t Wc1OriginFxReadLittleEndian16(const unsigned char *bytes)
{
    return (int16_t)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8));
}

static int Wc1OriginFxReadVariableLength(const unsigned char **cursor,
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

static int Wc1OriginFxAppendEvent(Wc1SdlOriginFxPlayer *player,
                                  const Wc1OriginFxEvent *event)
{
    Wc1OriginFxEvent *resized;
    size_t capacity;

    if (player->eventCount == player->eventCapacity) {
        capacity = player->eventCapacity == 0
            ? 256 : player->eventCapacity * 2;
        if (capacity < player->eventCapacity ||
            capacity > SIZE_MAX / sizeof(Wc1OriginFxEvent))
            return 0;
        resized = (Wc1OriginFxEvent *)realloc(
            player->events, capacity * sizeof(Wc1OriginFxEvent));
        if (resized == 0)
            return 0;
        player->events = resized;
        player->eventCapacity = capacity;
    }
    player->events[player->eventCount++] = *event;
    return 1;
}

static int Wc1OriginFxParseTrack(Wc1SdlOriginFxPlayer *player,
                                 const unsigned char *track,
                                 size_t trackSize,
                                 uint32_t *eventOrder,
                                 uint64_t *maximumTick)
{
    Wc1OriginFxEvent event;
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
        if (!Wc1OriginFxReadVariableLength(&cursor, end, &delta))
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
            event.type = WC1_ORIGINFX_CHANNEL_EVENT;
            event.status = status;
            event.data1 = cursor[0];
            event.data2 = dataSize == 2 ? cursor[1] : 0;
            if (!Wc1OriginFxAppendEvent(player, &event))
                return 0;
            cursor += dataSize;
            runningStatus = status;
            continue;
        }

        if (status == 0xfeU) {
            if ((size_t)(end - cursor) < 2)
                return 0;
            cursor++;
            eventLength = *cursor++;
            if ((size_t)(end - cursor) < eventLength)
                return 0;
            cursor += eventLength;
            continue;
        }

        if (status == 0xffU) {
            if (cursor >= end)
                return 0;
            subtype = *cursor++;
            if (!Wc1OriginFxReadVariableLength(
                    &cursor, end, &eventLength) ||
                (size_t)(end - cursor) < eventLength)
                return 0;
            if (subtype == 0x51U && eventLength == 3) {
                memset(&event, 0, sizeof(event));
                event.tick = tick;
                event.order = (*eventOrder)++;
                event.type = WC1_ORIGINFX_TEMPO_EVENT;
                event.tempo = ((uint32_t)cursor[0] << 16) |
                    ((uint32_t)cursor[1] << 8) | cursor[2];
                if (event.tempo == 0 ||
                    !Wc1OriginFxAppendEvent(player, &event))
                    return 0;
            }
            cursor += eventLength;
            runningStatus = 0;
            continue;
        }

        if (status == 0xf0U || status == 0xf7U) {
            if (!Wc1OriginFxReadVariableLength(
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

static int Wc1OriginFxCompareEvents(const void *left, const void *right)
{
    const Wc1OriginFxEvent *leftEvent;
    const Wc1OriginFxEvent *rightEvent;

    leftEvent = (const Wc1OriginFxEvent *)left;
    rightEvent = (const Wc1OriginFxEvent *)right;
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

static int Wc1OriginFxLoadMidi(Wc1SdlOriginFxPlayer *player,
                               const unsigned char *midi,
                               size_t midiSize)
{
    const unsigned char *cursor;
    const unsigned char *track;
    size_t headerSize;
    size_t channelEventCount;
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
    headerLength = Wc1OriginFxReadBigEndian32(midi + 4);
    headerSize = (size_t)headerLength + 8;
    if (headerLength < 6 || headerSize > midiSize)
        return 0;
    format = Wc1OriginFxReadBigEndian16(midi + 8);
    trackCount = Wc1OriginFxReadBigEndian16(midi + 10);
    division = Wc1OriginFxReadBigEndian16(midi + 12);
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
        trackLength = Wc1OriginFxReadBigEndian32(cursor + 4);
        cursor += 8;
        if ((size_t)(midi + midiSize - cursor) < trackLength)
            return 0;
        track = cursor;
        if (!Wc1OriginFxParseTrack(
                player, track, trackLength, &eventOrder, &maximumTick))
            return 0;
        cursor += trackLength;
        trackIndex++;
    }
    if (cursor != midi + midiSize)
        return 0;

    qsort(player->events, player->eventCount,
          sizeof(Wc1OriginFxEvent), Wc1OriginFxCompareEvents);
    exactFrame = 0;
    previousTick = 0;
    tempo = 500000;
    channelEventCount = 0;
    index = 0;
    while (index < player->eventCount) {
        exactFrame +=
            (long double)(player->events[index].tick - previousTick) *
            WC1_ORIGINFX_OUTPUT_RATE * tempo /
            ((long double)division * 1000000.0L);
        previousTick = player->events[index].tick;
        if (player->events[index].type == WC1_ORIGINFX_TEMPO_EVENT) {
            tempo = player->events[index].tempo;
        } else {
            player->events[index].frame =
                (uint64_t)(exactFrame + 0.5L);
            player->events[channelEventCount++] = player->events[index];
        }
        index++;
    }
    exactFrame += (long double)(maximumTick - previousTick) *
        WC1_ORIGINFX_OUTPUT_RATE * tempo /
        ((long double)division * 1000000.0L);
    player->eventCount = channelEventCount;
    player->endFrame = (uint64_t)(exactFrame + 0.5L);
    if (player->eventCount != 0 &&
        player->endFrame <= player->events[player->eventCount - 1].frame)
        player->endFrame = player->events[player->eventCount - 1].frame + 1;
    if (player->endFrame == 0)
        player->endFrame = 1;
    return 1;
}

static int Wc1OriginFxLoadTimbres(Wc1SdlOriginFxPlayer *player,
                                  const unsigned char *timbres,
                                  size_t timbreSize)
{
    size_t requiredSize;
    unsigned int count;

    if (timbres == 0 || timbreSize < WC1_ORIGINFX_TIMBRE_SIZE + 1)
        return 0;
    count = timbres[0];
    requiredSize = 1 + (size_t)count * WC1_ORIGINFX_TIMBRE_SIZE;
    if (count == 0 || requiredSize > timbreSize)
        return 0;
    player->timbres = (unsigned char *)malloc(requiredSize);
    if (player->timbres == 0)
        return 0;
    memcpy(player->timbres, timbres, requiredSize);
    player->timbreSize = requiredSize;
    return 1;
}

static void Wc1OriginFxWriteRegister(Wc1SdlOriginFxPlayer *player,
                                     unsigned int address,
                                     unsigned int value)
{
    player->oplChip.write(0, (uint8_t)address);
    player->oplChip.write(1, (uint8_t)value);
}

static void Wc1OriginFxResetOpl(Wc1SdlOriginFxPlayer *player)
{
    unsigned int voice;

    player->oplChip.reset();
    Wc1OriginFxWriteRegister(player, 0x01, 0x20);
    Wc1OriginFxWriteRegister(player, 0x08, 0);
    Wc1OriginFxWriteRegister(player, 0xbd, 0);
    voice = 0;
    while (voice < WC1_ORIGINFX_VOICE_COUNT) {
        Wc1OriginFxWriteRegister(player, 0xa0 + voice, 0);
        Wc1OriginFxWriteRegister(player, 0xb0 + voice, 0);
        voice++;
    }
}

static const unsigned char *Wc1OriginFxFindTimbre(
    const Wc1SdlOriginFxPlayer *player, unsigned int program)
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
        timbre += WC1_ORIGINFX_TIMBRE_SIZE;
        index++;
    }
    return player->timbres + 1;
}

static const unsigned char *Wc1OriginFxNextTimbre(
    const Wc1SdlOriginFxPlayer *player, const unsigned char *timbre)
{
    size_t offset;

    if (timbre[38] == 0)
        return 0;
    offset = (size_t)(timbre - player->timbres);
    if (offset > player->timbreSize ||
        player->timbreSize - offset < WC1_ORIGINFX_TIMBRE_SIZE * 2U)
        return 0;
    return timbre + WC1_ORIGINFX_TIMBRE_SIZE;
}

static int Wc1OriginFxArithmeticShiftRight(int value,
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

static unsigned int Wc1OriginFxClampTotalLevel(
    unsigned int registerValue, int totalLevel)
{
    if (totalLevel < 0)
        totalLevel = 0;
    else if (totalLevel > 0x3f)
        totalLevel = 0x3f;
    return (registerValue & 0xc0U) | (unsigned int)totalLevel;
}

static unsigned int Wc1OriginFxCalculateVelocityLevel(
    unsigned int registerValue, unsigned int sensitivity,
    unsigned int velocity)
{
    int attenuation;
    int totalLevel;

    if (sensitivity == 0)
        return registerValue;
    if (sensitivity > 7)
        sensitivity = 7;
    attenuation = Wc1OriginFxArithmeticShiftRight(
        63 - (int)velocity, 7 - sensitivity);
    totalLevel = (int)(registerValue & 0x3fU) + attenuation;
    return Wc1OriginFxClampTotalLevel(registerValue, totalLevel);
}

static unsigned int Wc1OriginFxCalculateCarrierLevel(
    unsigned int registerValue, unsigned int sensitivity,
    unsigned int velocity, unsigned int volume)
{
    int attenuation;
    int totalLevel;

    if (sensitivity == 0 && volume >= 0x100U)
        return registerValue;
    if (sensitivity > 7)
        sensitivity = 7;
    attenuation = Wc1OriginFxArithmeticShiftRight(
        63 - (int)velocity, 7 - sensitivity);
    totalLevel = (int)(registerValue & 0x3fU) + attenuation;
    totalLevel = 63 - Wc1OriginFxArithmeticShiftRight(
        (int)volume * (63 - totalLevel), 8);
    return Wc1OriginFxClampTotalLevel(registerValue, totalLevel);
}

static void Wc1OriginFxWriteVoiceLevels(Wc1SdlOriginFxPlayer *player,
                                        unsigned int voiceIndex)
{
    static const unsigned char modulatorOffsets[9] = {
        0, 1, 2, 8, 9, 10, 16, 17, 18
    };
    static const unsigned char carrierOffsets[9] = {
        3, 4, 5, 11, 12, 13, 19, 20, 21
    };
    Wc1OriginFxVoice *voice;
    Wc1OriginFxChannel *channel;
    const unsigned char *timbre;

    voice = &player->voices[voiceIndex];
    channel = &player->channels[voice->channel];
    timbre = voice->timbre;
    Wc1OriginFxWriteRegister(
        player, 0x40 + carrierOffsets[voiceIndex],
        Wc1OriginFxCalculateCarrierLevel(
            timbre[6], timbre[12], voice->velocity, channel->volume));
    Wc1OriginFxWriteRegister(
        player, 0x40 + modulatorOffsets[voiceIndex],
        Wc1OriginFxCalculateVelocityLevel(
            timbre[1], timbre[13], voice->velocity));
}

static void Wc1OriginFxWriteVoiceFrequency(Wc1SdlOriginFxPlayer *player,
                                           unsigned int voiceIndex,
                                           int keyOn)
{
    static const unsigned short frequencyNumbers[13] = {
        485, 514, 544, 577, 611, 647, 686,
        727, 770, 816, 864, 915, 970
    };
    Wc1OriginFxVoice *voice;
    Wc1OriginFxChannel *channel;
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
    const unsigned char *channelTimbre;

    voice = &player->voices[voiceIndex];
    channel = &player->channels[voice->channel];
    bend = (int)channel->pitchBend - 0x2000;
    keyPitch = ((int)voice->note - 60) * 256;
    keyTracking = (signed char)voice->timbre[39];
    if (keyTracking < 0) {
        keyPitch = -keyPitch;
        trackingShift = (unsigned int)(unsigned char)~keyTracking;
    } else {
        trackingShift = (unsigned int)keyTracking;
    }
    keyPitch = Wc1OriginFxArithmeticShiftRight(
        keyPitch, trackingShift) + 60 * 256;
    channelTimbre = Wc1OriginFxFindTimbre(
        player, channel->program);
    pitch = keyPitch +
        voice->envelopePitch +
        Wc1OriginFxReadLittleEndian16(voice->timbre + 36) +
        voice->modulationPitch +
        Wc1OriginFxArithmeticShiftRight(
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

    voice->frequencyHigh = (unsigned char)(
        ((frequency >> 8) & 3) | (block << 2));
    Wc1OriginFxWriteRegister(player, 0xa0 + voiceIndex, frequency & 0xff);
    Wc1OriginFxWriteRegister(
        player, 0xb0 + voiceIndex,
        voice->frequencyHigh | (keyOn != 0 ? 0x20 : 0));
}

static void Wc1OriginFxProgramVoice(Wc1SdlOriginFxPlayer *player,
                                    unsigned int voiceIndex)
{
    static const unsigned char modulatorOffsets[9] = {
        0, 1, 2, 8, 9, 10, 16, 17, 18
    };
    static const unsigned char carrierOffsets[9] = {
        3, 4, 5, 11, 12, 13, 19, 20, 21
    };
    const unsigned char *timbre;

    timbre = player->voices[voiceIndex].timbre;
    Wc1OriginFxWriteRegister(player, 0xb0 + voiceIndex, 0);
    Wc1OriginFxWriteRegister(
        player, 0x20 + modulatorOffsets[voiceIndex], timbre[0]);
    Wc1OriginFxWriteRegister(
        player, 0x40 + modulatorOffsets[voiceIndex], timbre[1]);
    Wc1OriginFxWriteRegister(
        player, 0x60 + modulatorOffsets[voiceIndex], timbre[2]);
    Wc1OriginFxWriteRegister(
        player, 0x80 + modulatorOffsets[voiceIndex], timbre[3]);
    Wc1OriginFxWriteRegister(
        player, 0xe0 + modulatorOffsets[voiceIndex], timbre[4]);
    Wc1OriginFxWriteRegister(
        player, 0x20 + carrierOffsets[voiceIndex], timbre[5]);
    Wc1OriginFxWriteRegister(
        player, 0x40 + carrierOffsets[voiceIndex], timbre[6]);
    Wc1OriginFxWriteRegister(
        player, 0x60 + carrierOffsets[voiceIndex], timbre[7]);
    Wc1OriginFxWriteRegister(
        player, 0x80 + carrierOffsets[voiceIndex], timbre[8]);
    Wc1OriginFxWriteRegister(
        player, 0xe0 + carrierOffsets[voiceIndex], timbre[9]);
    Wc1OriginFxWriteRegister(player, 0xc0 + voiceIndex, timbre[10]);
    Wc1OriginFxWriteVoiceLevels(player, voiceIndex);
}

static unsigned int Wc1OriginFxChooseVoice(Wc1SdlOriginFxPlayer *player)
{
    unsigned int oldestFreeVoice;
    unsigned int oldestVoice;
    unsigned int voiceIndex;

    oldestFreeVoice = WC1_ORIGINFX_VOICE_COUNT;
    oldestVoice = 0;
    voiceIndex = 0;
    while (voiceIndex < WC1_ORIGINFX_VOICE_COUNT) {
        if (player->voices[voiceIndex].active == 0) {
            if (oldestFreeVoice == WC1_ORIGINFX_VOICE_COUNT ||
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
    if (oldestFreeVoice != WC1_ORIGINFX_VOICE_COUNT)
        return oldestFreeVoice;
    return oldestVoice;
}

static void Wc1OriginFxStartTimbre(Wc1SdlOriginFxPlayer *player,
                                   unsigned int channelIndex,
                                   unsigned int note,
                                   unsigned int velocity,
                                   const unsigned char *timbre)
{
    Wc1OriginFxVoice *voice;
    unsigned int voiceIndex;

    voiceIndex = Wc1OriginFxChooseVoice(player);
    voice = &player->voices[voiceIndex];
    if (voice->active != 0) {
        Wc1OriginFxWriteRegister(player, 0xa0 + voiceIndex, 0);
        Wc1OriginFxWriteRegister(player, 0xb0 + voiceIndex, 0);
    }
    voice->timbre = timbre;
    voice->age = player->nextVoiceAge++;
    voice->channel = (unsigned char)channelIndex;
    voice->note = (unsigned char)note;
    voice->velocity = (unsigned char)velocity;
    voice->envelopeState = 2;
    voice->modulationPhase = 0;
    voice->envelopePitch =
        Wc1OriginFxReadLittleEndian16(timbre + 18);
    voice->modulationPitch = 0;
    voice->active = 1;
    Wc1OriginFxProgramVoice(player, voiceIndex);
    Wc1OriginFxWriteVoiceFrequency(player, voiceIndex, 1);
}

static void Wc1OriginFxNoteOn(Wc1SdlOriginFxPlayer *player,
                              unsigned int channelIndex,
                              unsigned int note,
                              unsigned int velocity)
{
    const unsigned char *timbre;

    timbre = Wc1OriginFxFindTimbre(
        player, player->channels[channelIndex].program);
    while (timbre != 0) {
        Wc1OriginFxStartTimbre(
            player, channelIndex, note, velocity, timbre);
        timbre = Wc1OriginFxNextTimbre(player, timbre);
    }
}

static void Wc1OriginFxStopTimbre(Wc1SdlOriginFxPlayer *player,
                                  unsigned int channelIndex,
                                  unsigned int note,
                                  const unsigned char *timbre)
{
    unsigned int voiceIndex;

    voiceIndex = 0;
    while (voiceIndex < WC1_ORIGINFX_VOICE_COUNT) {
        if (player->voices[voiceIndex].active != 0 &&
            player->voices[voiceIndex].channel == channelIndex &&
            player->voices[voiceIndex].note == note &&
            player->voices[voiceIndex].timbre == timbre) {
            Wc1OriginFxWriteRegister(
                player, 0xb0 + voiceIndex,
                player->voices[voiceIndex].frequencyHigh);
            player->voices[voiceIndex].active = 0;
            player->voices[voiceIndex].envelopeState = 0;
            player->voices[voiceIndex].age = player->nextVoiceAge++;
            return;
        }
        voiceIndex++;
    }
}

static void Wc1OriginFxNoteOff(Wc1SdlOriginFxPlayer *player,
                               unsigned int channelIndex,
                               unsigned int note)
{
    const unsigned char *timbre;

    timbre = Wc1OriginFxFindTimbre(
        player, player->channels[channelIndex].program);
    while (timbre != 0) {
        Wc1OriginFxStopTimbre(
            player, channelIndex, note, timbre);
        timbre = Wc1OriginFxNextTimbre(player, timbre);
    }
}

static void Wc1OriginFxAllNotesOff(Wc1SdlOriginFxPlayer *player,
                                   int channelIndex)
{
    unsigned int voiceIndex;

    voiceIndex = 0;
    while (voiceIndex < WC1_ORIGINFX_VOICE_COUNT) {
        if (player->voices[voiceIndex].active != 0 &&
            (channelIndex < 0 ||
             player->voices[voiceIndex].channel == channelIndex)) {
            Wc1OriginFxWriteRegister(
                player, 0xb0 + voiceIndex,
                player->voices[voiceIndex].frequencyHigh);
            player->voices[voiceIndex].active = 0;
            player->voices[voiceIndex].envelopeState = 0;
            player->voices[voiceIndex].age = player->nextVoiceAge++;
        }
        voiceIndex++;
    }
}

static void Wc1OriginFxUpdateChannelVoices(
    Wc1SdlOriginFxPlayer *player, unsigned int channelIndex,
    int updateFrequency, int updateLevel)
{
    unsigned int voiceIndex;

    voiceIndex = 0;
    while (voiceIndex < WC1_ORIGINFX_VOICE_COUNT) {
        if (player->voices[voiceIndex].active != 0 &&
            player->voices[voiceIndex].channel == channelIndex) {
            if (updateFrequency != 0)
                Wc1OriginFxWriteVoiceFrequency(player, voiceIndex, 1);
            if (updateLevel != 0)
                Wc1OriginFxWriteVoiceLevels(player, voiceIndex);
        }
        voiceIndex++;
    }
}

static void Wc1OriginFxSetProgram(Wc1SdlOriginFxPlayer *player,
                                  unsigned int channelIndex,
                                  unsigned int program)
{
    Wc1OriginFxChannel *channel;
    const unsigned char *timbre;
    unsigned int voiceIndex;

    Wc1OriginFxAllNotesOff(player, (int)channelIndex);
    voiceIndex = 0;
    while (voiceIndex < WC1_ORIGINFX_VOICE_COUNT) {
        if (player->voices[voiceIndex].timbre != 0 &&
            player->voices[voiceIndex].channel == channelIndex) {
            player->voices[voiceIndex].timbre = 0;
            player->voices[voiceIndex].channel = 0xff;
        }
        voiceIndex++;
    }
    channel = &player->channels[channelIndex];
    channel->program = (unsigned char)program;
    timbre = Wc1OriginFxFindTimbre(player, program);
    channel->modulationRate = timbre[16];
    channel->modulationDepth = timbre[17];
}

static void Wc1OriginFxService(Wc1SdlOriginFxPlayer *player)
{
    static const unsigned char envelopeTargetOffsets[5] = {
        32, 0, 20, 24, 28
    };
    static const unsigned char envelopeRateOffsets[5] = {
        34, 0, 22, 26, 30
    };
    Wc1OriginFxVoice *voice;
    Wc1OriginFxChannel *channel;
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
    while (voiceIndex < WC1_ORIGINFX_VOICE_COUNT) {
        voice = &player->voices[voiceIndex];
        timbre = voice->timbre;
        if (timbre == 0 || voice->channel >= WC1_ORIGINFX_CHANNEL_COUNT) {
            voiceIndex++;
            continue;
        }
        changed = 0;
        state = voice->envelopeState;
        if (state < sizeof(envelopeTargetOffsets) &&
            envelopeTargetOffsets[state] != 0) {
            target = Wc1OriginFxReadLittleEndian16(
                timbre + envelopeTargetOffsets[state]);
            rate = (unsigned int)(uint16_t)
                Wc1OriginFxReadLittleEndian16(
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
            voice->modulationPhase = (unsigned char)phase;
            triangle = phase < 0x80U ? (int)phase : (int)phase - 0x100;
            if (triangle > 63 || triangle < -64) {
                phase = (0x80U - phase) & 0xffU;
                triangle = phase < 0x80U
                    ? (int)phase : (int)phase - 0x100;
            }
            voice->modulationPitch = Wc1OriginFxArithmeticShiftRight(
                (int)channel->modulationDepth * triangle, 4);
            changed = 1;
        }
        if (changed != 0)
            Wc1OriginFxWriteVoiceFrequency(
                player, voiceIndex, voice->envelopeState > 1);
        voiceIndex++;
    }
}

static void Wc1OriginFxAdvanceService(Wc1SdlOriginFxPlayer *player)
{
    player->serviceAccumulator += WC1_ORIGINFX_SERVICE_RATE;
    while (player->serviceAccumulator >= WC1_ORIGINFX_OUTPUT_RATE) {
        player->serviceAccumulator -= WC1_ORIGINFX_OUTPUT_RATE;
        Wc1OriginFxService(player);
    }
}

static int Wc1OriginFxMapPercussionNote(unsigned int note,
                                        unsigned int *channelIndex,
                                        unsigned int *mappedNote)
{
    unsigned int channel;

    if (note >= WC1_ORIGINFX_PERCUSSION_NOTE_COUNT)
        return 0;
    channel = g_abWc1OriginFxPercussionChannels[note];
    if (channel == 0)
        return 0;
    *channelIndex = channel - 1;
    *mappedNote = g_abWc1OriginFxPercussionPitches[note];
    return 1;
}

static void Wc1OriginFxApplyControlChange(
    Wc1SdlOriginFxPlayer *player, unsigned int channelIndex,
    unsigned int controller, unsigned int value)
{
    Wc1OriginFxChannel *channel;
    const unsigned char *timbre;

    channel = &player->channels[channelIndex];
    if (controller == 1) {
        timbre = Wc1OriginFxFindTimbre(player, channel->program);
        channel->modulationDepth = (unsigned short)(
            ((unsigned int)timbre[15] * value >> 7) + timbre[17]);
    } else if (controller == 7) {
        channel->volume = (unsigned short)(value + 0x80U);
    } else if (controller == 120 || controller == 123) {
        Wc1OriginFxAllNotesOff(player, (int)channelIndex);
    } else if (controller == 121) {
        timbre = Wc1OriginFxFindTimbre(player, channel->program);
        channel->modulationDepth = timbre[17];
        channel->volume = 0xff;
        channel->pitchBend = 0x2000;
        Wc1OriginFxUpdateChannelVoices(
            player, channelIndex, 1, 0);
    }
}

static void Wc1OriginFxControlChange(Wc1SdlOriginFxPlayer *player,
                                     unsigned int channelIndex,
                                     unsigned int controller,
                                     unsigned int value)
{
    unsigned int percussionChannel;

    if (channelIndex == 9) {
        percussionChannel = 10;
        while (percussionChannel < WC1_ORIGINFX_CHANNEL_COUNT) {
            Wc1OriginFxApplyControlChange(
                player, percussionChannel, controller, value);
            percussionChannel++;
        }
    }
    Wc1OriginFxApplyControlChange(
        player, channelIndex, controller, value);
}

static void Wc1OriginFxDispatchEvent(Wc1SdlOriginFxPlayer *player,
                                     const Wc1OriginFxEvent *event)
{
    Wc1OriginFxChannel *channel;
    unsigned int channelIndex;
    unsigned int command;
    unsigned int note;

    channelIndex = event->status & 0x0fU;
    command = event->status & 0xf0U;
    channel = &player->channels[channelIndex];
    switch (command) {
    case 0x80:
        note = event->data1;
        if (channelIndex != 9 || Wc1OriginFxMapPercussionNote(
                note, &channelIndex, &note))
            Wc1OriginFxNoteOff(player, channelIndex, note);
        break;
    case 0x90:
        note = event->data1;
        if (channelIndex != 9 || Wc1OriginFxMapPercussionNote(
                note, &channelIndex, &note)) {
            if (event->data2 == 0)
                Wc1OriginFxNoteOff(player, channelIndex, note);
            else
                Wc1OriginFxNoteOn(
                    player, channelIndex, note, event->data2);
        }
        break;
    case 0xb0:
        Wc1OriginFxControlChange(
            player, channelIndex, event->data1, event->data2);
        break;
    case 0xc0:
        Wc1OriginFxSetProgram(player, channelIndex, event->data1);
        break;
    case 0xe0:
        channel->pitchBend = (unsigned short)(
            event->data1 | ((unsigned int)event->data2 << 7));
        Wc1OriginFxUpdateChannelVoices(player, channelIndex, 1, 0);
        break;
    }
}

static void Wc1OriginFxProcessDueEvents(Wc1SdlOriginFxPlayer *player)
{
    while (player->nextEvent < player->eventCount &&
           player->events[player->nextEvent].frame <=
               player->currentFrame) {
        Wc1OriginFxDispatchEvent(
            player, &player->events[player->nextEvent]);
        player->nextEvent++;
    }
}

static int32_t Wc1OriginFxGenerateOutputSample(
    Wc1SdlOriginFxPlayer *player)
{
    ymfm::ym3812::output_data output;
    int64_t sampleTotal;
    unsigned int sampleCount;

    sampleTotal = 0;
    sampleCount = 0;
    player->nativeSampleAccumulator += player->nativeSampleRate;
    while (player->nativeSampleAccumulator >= WC1_ORIGINFX_OUTPUT_RATE) {
        player->oplChip.generate(&output);
        player->lastNativeSample = output.data[0];
        sampleTotal += player->lastNativeSample;
        sampleCount++;
        player->nativeSampleAccumulator -= WC1_ORIGINFX_OUTPUT_RATE;
    }
    if (sampleCount == 0)
        return player->lastNativeSample;
    return (int32_t)(sampleTotal / sampleCount);
}

static short Wc1OriginFxScaleOutputSample(int32_t sample,
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
    return (short)scaled;
}

Wc1SdlOriginFxPlayer *Wc1SdlCreateOriginFxPlayer(
    const unsigned char *midi, size_t midiSize,
    const unsigned char *timbres, size_t timbreSize)
{
    Wc1SdlOriginFxPlayer *player;
    unsigned int channelIndex;
    unsigned int percussionIndex;
    unsigned char defaultProgram;

    player = new (std::nothrow) Wc1SdlOriginFxPlayer;
    if (player == 0)
        return 0;
    if (!Wc1OriginFxLoadTimbres(
            player, timbres, timbreSize) ||
        !Wc1OriginFxLoadMidi(player, midi, midiSize)) {
        delete player;
        return 0;
    }
    defaultProgram = player->timbres[1 + 47];
    channelIndex = 0;
    while (channelIndex < WC1_ORIGINFX_CHANNEL_COUNT) {
        player->channels[channelIndex].pitchBend = 0x2000;
        player->channels[channelIndex].volume = 0xff;
        Wc1OriginFxSetProgram(player, channelIndex, defaultProgram);
        channelIndex++;
    }
    percussionIndex = 0;
    while (percussionIndex <
           sizeof(g_abWc1OriginFxPercussionPrograms) /
               sizeof(g_abWc1OriginFxPercussionPrograms[0])) {
        Wc1OriginFxSetProgram(
            player, 9 + percussionIndex,
            g_abWc1OriginFxPercussionPrograms[percussionIndex]);
        percussionIndex++;
    }
    Wc1OriginFxResetOpl(player);
    return player;
}

void Wc1SdlDestroyOriginFxPlayer(Wc1SdlOriginFxPlayer *player)
{
    delete player;
}

int Wc1SdlOriginFxPlayerFinished(const Wc1SdlOriginFxPlayer *player)
{
    return player == 0 || player->finished != 0;
}

void Wc1SdlRenderOriginFxPlayer(Wc1SdlOriginFxPlayer *player,
                                short *samples,
                                unsigned int frameCount,
                                unsigned int gain)
{
    int32_t generated;
    short output;
    unsigned int frame;

    if (samples == 0)
        return;
    if (player == 0 || player->finished != 0) {
        memset(samples, 0, (size_t)frameCount * sizeof(short) * 2);
        return;
    }

    frame = 0;
    while (frame < frameCount) {
        Wc1OriginFxProcessDueEvents(player);
        if (player->nextEvent == player->eventCount &&
            player->currentFrame >= player->endFrame) {
            Wc1OriginFxAllNotesOff(player, -1);
            player->finished = 1;
            memset(samples + frame * 2, 0,
                   (size_t)(frameCount - frame) * sizeof(short) * 2);
            return;
        }
        Wc1OriginFxAdvanceService(player);
        generated = Wc1OriginFxGenerateOutputSample(player);
        output = Wc1OriginFxScaleOutputSample(generated, gain);
        samples[frame * 2] = output;
        samples[frame * 2 + 1] = output;
        player->currentFrame++;
        frame++;
    }
    if (player->nextEvent == player->eventCount &&
        player->currentFrame >= player->endFrame) {
        Wc1OriginFxAllNotesOff(player, -1);
        player->finished = 1;
    }
}
