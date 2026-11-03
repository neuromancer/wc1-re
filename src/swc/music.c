/* Host reader for the QuickTime 'musi' movies loaded by CODE_12 QTSound
 * +0x0930. The demo uses moov/128 resource forks and 600-tick note streams.
 * Container/event definitions follow Apple's QuickTime file format and
 * QuickTimeMusic.h; no QuickTime runtime or persisted Mac pointers are used. */
#include "swc.h"

#include <stdlib.h>
#include <string.h>

static uint32_t SwcMusicLong(const uint8_t *bytes)
{
    return ((uint32_t)bytes[0] << 24) | ((uint32_t)bytes[1] << 16) |
           ((uint32_t)bytes[2] << 8) | bytes[3];
}

/* Borrow the payload of one atom; duplicate containers are outside the
 * demo's single-track format. Atom sizes include their eight-byte header. */
static int SwcMusicAtom(const SwcBuffer *parent, const char type[4], SwcBuffer *child)
{
    size_t position = 0;
    size_t length;
    int found = 0;

    SDL_zero(*child);
    while (position < parent->size) {
        if (parent->size - position < 8)
            return -1;
        length = SwcMusicLong(parent->data + position);
        if (length == 0)
            length = parent->size - position;
        if (length < 8 || length > parent->size - position)
            return -1;
        if (memcmp(parent->data + position + 4, type, 4) == 0) {
            if (found)
                return -1;
            child->data = parent->data + position + 8;
            child->size = length - 8;
            found = 1;
        }
        position += length;
    }
    return found;
}

static int SwcReadMusicEvents(const SwcBuffer *bytes, SwcMusic *music,
                               size_t capacity, uint32_t *tick, int header)
{
    size_t position = 0;
    uint32_t word;
    uint32_t tail;
    uint32_t length;
    uint32_t duration;
    uint32_t instrument;
    uint32_t part;
    uint32_t pitch;
    uint32_t velocity;
    uint32_t kind;
    SwcMusicEvent *event;

    while (position < bytes->size) {
        if (bytes->size - position < 4)
            return -1;
        word = SwcMusicLong(bytes->data + position);
        kind = word >> 28;
        position += 4;
        if (kind == 15) {
            length = word & 0xffff;
            if (!header || length != 23 || (length - 1) * 4 > bytes->size - position)
                return -1;
            tail = SwcMusicLong(bytes->data + position + (length - 2) * 4);
            if (tail != UINT32_C(0xc0010017))
                return -1;
            part = (word >> 16) & 0xfff;
            instrument = SwcMusicLong(bytes->data + position + 80);
            if (part >= SDL_arraysize(music->instruments) ||
                music->instruments[part] != 0 ||
                !((instrument >= 1 && instrument <= 128) ||
                  (instrument >= 0x4001 && instrument <= 0x4080)))
                return -1;
            music->instruments[part] = instrument;
            position += (length - 1) * 4;
            continue;
        }
        if (header) {
            /* Tune headers end with kEndMarkerValue after their note
             * requests. Several original descriptions retain zero padding
             * after it; neither the marker nor padding is a timed event. */
            if (word != UINT32_C(0x60000000))
                return -1;
            while (position < bytes->size) {
                if (bytes->data[position++] != 0)
                    return -1;
            }
            return 0;
        }
        if (kind < 2) {
            duration = word & 0xffffff;
            if (duration > music->duration - *tick)
                return -1;
            *tick += duration;
            continue;
        }
        if (kind == 6 || kind == 7)
            continue; /* End-of-sample/editing marker, not an all-notes-off. */
        part = (word >> 24) & 31;
        if (kind == 9) {
            if (bytes->size - position < 4)
                return -1;
            tail = SwcMusicLong(bytes->data + position);
            position += 4;
            if ((tail >> 30) != 2)
                return -1;
            part = (word >> 16) & 0xfff;
            pitch = word & 0xffff;
            /* Apple encodes pitches below 128 as MIDI integers; larger
             * values are 8.8 pitches. The demo has no fractional notes. */
            if (pitch >= 128) {
                if ((pitch & 255) != 0)
                    return -1;
                pitch >>= 8;
            }
            velocity = (tail >> 22) & 127;
            duration = tail & 0x3fffff;
        } else if (kind == 2 || kind == 3) {
            pitch = ((word >> 18) & 63) + 32;
            velocity = (word >> 11) & 127;
            duration = word & 2047;
        } else if (kind == 4 || kind == 5) {
            if (part >= SDL_arraysize(music->instruments) ||
                music->instruments[part] == 0 || music->count >= capacity)
                return -1;
            event = &music->events[music->count];
            *event = (SwcMusicEvent){*tick, (uint32_t)music->count, (uint16_t)part,
                (uint16_t)word, SWC_MUSIC_CONTROL, (uint8_t)(word >> 16)};
            music->count++;
            continue;
        } else {
            return -1;
        }
        if (part >= SDL_arraysize(music->instruments) || music->instruments[part] == 0 ||
            pitch > 127 || duration > music->duration - *tick ||
            music->count > capacity - 2)
            return -1;
        event = &music->events[music->count];
        *event = (SwcMusicEvent){*tick, (uint32_t)music->count, (uint16_t)part,
            (uint16_t)velocity, velocity ? SWC_MUSIC_NOTE_ON : SWC_MUSIC_NOTE_OFF,
            (uint8_t)pitch};
        music->count++;
        if (velocity != 0) {
            event = &music->events[music->count];
            *event = (SwcMusicEvent){*tick + duration, (uint32_t)music->count,
                (uint16_t)part, 0, SWC_MUSIC_NOTE_OFF, (uint8_t)pitch};
            music->count++;
        }
    }
    return header ? -1 : 0;
}

static int SwcCompareMusicEvents(const void *left, const void *right)
{
    const SwcMusicEvent *a = left;
    const SwcMusicEvent *b = right;

    if (a->tick != b->tick)
        return a->tick < b->tick ? -1 : 1;
    if (a->order != b->order)
        return a->order < b->order ? -1 : 1;
    return 0;
}

int SwcReadMusic(const char *path, SwcMusic *music)
{
    SwcBuffer file = {0};
    SwcBuffer resource = {0};
    SwcBuffer movie, track, media, info, table, header, description;
    SwcBuffer sizes, chunks, mapping, timing, edits, references, dataInfo, sample;
    SDL_RWops *reader;
    Sint64 fileSize;
    const char *base;
    const char *separator;
    const char *section = "movie atoms";
    char forkPath[4096];
    uint32_t count;
    uint32_t fixedSize;
    uint32_t index;
    uint32_t tick = 0;
    uint32_t sampleDuration;
    uint32_t timeEntry = 0;
    uint32_t timeRemaining = 0;
    uint32_t timeCount;
    uint32_t offset;
    uint32_t length;
    size_t capacity;
    int candidate;
    int written;
    int found;
    int result = -1;

    SDL_zero(*music);
    reader = SDL_RWFromFile(path, "rb");
    if (reader == NULL)
        return -1;
    fileSize = SDL_RWsize(reader);
    SDL_RWclose(reader);
    if (fileSize < 8 || fileSize > 8 * 1024 * 1024)
        goto malformed;
    if (SwcLoadDataFork(path, &file) != 0)
        return -1;
    found = SwcMusicAtom(&file, "moov", &movie);
    if (found < 0)
        goto malformed;
    if (!found) {
        /* Flattened movies can carry moov in the data fork. Original demo
         * files instead keep it in resource 128. Portable copies keep that
         * fork in the file itself as MacBinary (archives of the HFS disc),
         * a raw .rsrc sidecar, macOS's named fork or an AppleDouble ._ file. */
        base = strrchr(path, '/');
        separator = strrchr(path, '\\');
        if (separator != NULL && (base == NULL || separator > base))
            base = separator;
        base = base == NULL ? path : base + 1;
        for (candidate = 0; candidate < 4; candidate++) {
            written = candidate == 0 ? SDL_snprintf(forkPath, sizeof(forkPath), "%s", path) :
                candidate == 1 ? SDL_snprintf(forkPath, sizeof(forkPath), "%s.rsrc", path) :
                candidate == 2 ? SDL_snprintf(forkPath, sizeof(forkPath), "%s/..namedfork/rsrc", path) :
                SDL_snprintf(forkPath, sizeof(forkPath), "%.*s._%s", (int)(base - path), path, base);
            if (written >= (int)sizeof(forkPath))
                goto malformed;
            if (SwcReadResource(forkPath, "moov", 128, &resource) == 0)
                break;
        }
        if (candidate == 4)
            goto done;
        if (SwcMusicAtom(&resource, "moov", &movie) != 1)
            goto malformed;
    }
    section = "media header";
    if (SwcMusicAtom(&movie, "trak", &track) != 1 ||
        SwcMusicAtom(&track, "mdia", &media) != 1 ||
        SwcMusicAtom(&media, "hdlr", &header) != 1 || header.size < 12 ||
        memcmp(header.data + 8, "musi", 4) != 0 ||
        SwcMusicAtom(&media, "mdhd", &header) != 1 || header.size != 24 ||
        SwcMusicLong(header.data) != 0)
        goto malformed;
    music->timeScale = SwcMusicLong(header.data + 12);
    music->duration = SwcMusicLong(header.data + 16);
    if (music->timeScale == 0 || music->timeScale > 1000000 ||
        (uint64_t)music->duration * 48000 < music->timeScale ||
        (uint64_t)music->duration > (uint64_t)music->timeScale * 1800)
        goto malformed;
    section = "edit list";
    found = SwcMusicAtom(&track, "edts", &edits);
    if (found < 0 || (found && (SwcMusicAtom(&edits, "elst", &header) != 1 ||
        header.size != 20 || SwcMusicLong(header.data) != 0 ||
        SwcMusicLong(header.data + 4) != 1 || SwcMusicLong(header.data + 12) != 0 ||
        SwcMusicLong(header.data + 16) != 0x10000)))
        goto malformed;
    section = "sample tables";
    if (SwcMusicAtom(&media, "minf", &info) != 1 ||
        SwcMusicAtom(&info, "dinf", &dataInfo) != 1 ||
        SwcMusicAtom(&dataInfo, "dref", &references) != 1 || references.size != 20 ||
        SwcMusicLong(references.data) != 0 || SwcMusicLong(references.data + 4) != 1 ||
        SwcMusicLong(references.data + 8) != 12 ||
        memcmp(references.data + 12, "alis", 4) != 0 ||
        SwcMusicLong(references.data + 16) != 1 ||
        SwcMusicAtom(&info, "stbl", &table) != 1 ||
        SwcMusicAtom(&table, "stsd", &description) != 1 || description.size < 28 ||
        SwcMusicLong(description.data) != 0 || SwcMusicLong(description.data + 4) != 1 ||
        SwcMusicLong(description.data + 8) != description.size - 8 ||
        memcmp(description.data + 12, "musi", 4) != 0 ||
        SwcMusicLong(description.data + 20) != 1 ||
        SwcMusicLong(description.data + 24) != 0 ||
        SwcMusicAtom(&table, "stsz", &sizes) != 1 || sizes.size < 12 ||
        SwcMusicLong(sizes.data) != 0 ||
        SwcMusicAtom(&table, "stco", &chunks) != 1 || chunks.size < 8 ||
        SwcMusicLong(chunks.data) != 0 ||
        SwcMusicAtom(&table, "stsc", &mapping) != 1 || mapping.size != 20 ||
        SwcMusicLong(mapping.data) != 0 || SwcMusicLong(mapping.data + 4) != 1 ||
        SwcMusicLong(mapping.data + 8) != 1 || SwcMusicLong(mapping.data + 12) != 1 ||
        SwcMusicLong(mapping.data + 16) != 1 ||
        SwcMusicAtom(&table, "stts", &timing) != 1 || timing.size < 16 ||
        SwcMusicLong(timing.data) != 0)
        goto malformed;
    count = SwcMusicLong(sizes.data + 8);
    /* BRIEFIN0 has one 840-byte sample described by the fixed-size field.
     * Other supplied tracks use the per-sample sizes that follow it. */
    fixedSize = SwcMusicLong(sizes.data + 4);
    timeCount = SwcMusicLong(timing.data + 4);
    if (count == 0 || count > 8192 || count != SwcMusicLong(chunks.data + 4) ||
        sizes.size != 12 + (fixedSize ? 0 : (size_t)count * 4) ||
        chunks.size != 8 + (size_t)count * 4 ||
        timeCount == 0 || timeCount > count || timing.size != 8 + (size_t)timeCount * 8)
        goto malformed;
    capacity = 0;
    for (index = 0; index < count; index++) {
        offset = SwcMusicLong(chunks.data + 8 + index * 4);
        length = fixedSize ? fixedSize : SwcMusicLong(sizes.data + 12 + index * 4);
        if (length == 0 || length % 4 != 0 || offset < 8 || offset > file.size ||
            length > file.size - offset || length / 2 > 500000 - capacity)
            goto malformed;
        capacity += length / 2;
    }
    music->events = SDL_calloc(capacity, sizeof(*music->events));
    if (music->events == NULL) {
        SDL_OutOfMemory();
        goto done;
    }
    sample = (SwcBuffer){description.data + 28, description.size - 28};
    section = "instrument requests";
    if (SwcReadMusicEvents(&sample, music, capacity, &tick, 1) != 0)
        goto malformed;
    section = "timed events";
    sampleDuration = 0;
    for (index = 0; index < count; index++) {
        if (timeRemaining == 0) {
            if (timeEntry >= timeCount)
                goto malformed;
            timeRemaining = SwcMusicLong(timing.data + 8 + timeEntry * 8);
            sampleDuration = SwcMusicLong(timing.data + 12 + timeEntry * 8);
            timeEntry++;
            if (timeRemaining == 0 || timeRemaining > count - index)
                goto malformed;
        }
        if (sampleDuration > music->duration - tick)
            goto malformed;
        offset = SwcMusicLong(chunks.data + 8 + index * 4);
        length = fixedSize ? fixedSize : SwcMusicLong(sizes.data + 12 + index * 4);
        sample = (SwcBuffer){file.data + offset, length};
        offset = tick + sampleDuration;
        if (SwcReadMusicEvents(&sample, music, capacity, &tick, 0) != 0 || tick != offset)
            goto malformed;
        timeRemaining--;
    }
    if (timeEntry != timeCount || timeRemaining != 0 || tick != music->duration || music->count == 0)
        goto malformed;
    qsort(music->events, music->count, sizeof(*music->events), SwcCompareMusicEvents);
    result = 0;
    SDL_ClearError();
    goto done;
malformed:
    SDL_SetError("Unsupported or malformed SWC QuickTime music (%s): %s", section, path);
done:
    SDL_free(file.data);
    SDL_free(resource.data);
    if (result != 0) {
        SDL_free(music->events);
        SDL_zero(*music);
    }
    return result;
}
