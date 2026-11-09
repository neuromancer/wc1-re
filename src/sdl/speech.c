/* Optional, user-supplied recordings indexed by PO context/text ID.
 * Loaded packet addresses identify scenes even when subtitles are translated. */
#include "wc1sdl.h"
#include <stdlib.h>
#include <string.h>

typedef struct VoiceEntry {
    const char *context;
    const char *id;
    const char *path;
} VoiceEntry;

typedef struct VoicePacket {
    const unsigned char *data;
    unsigned int pairs;
    short section;
    struct VoicePacket *next;
} VoicePacket;

static unsigned char *voiceCatalog;
static const char *voiceRoot;
static VoiceEntry *voiceEntries;
static size_t voiceCount;
static VoicePacket *voicePackets;
static char installedVoiceRoot[4096];
static int radioVoice;

static unsigned int Voice16(const unsigned char *p)
{
    return p[0] | (unsigned int)p[1] << 8;
}

static unsigned int Voice32(const unsigned char *p)
{
    return Voice16(p) | Voice16(p + 2) << 16;
}

void SdlShutdownVoices(void)
{
    SdlStopVoice();
    while (voicePackets) {
        VoicePacket *next = voicePackets->next;
        free(voicePackets);
        voicePackets = next;
    }
    free(voiceEntries);
    free(voiceCatalog);
    voiceEntries = 0;
    voiceCatalog = 0;
    voiceRoot = 0;
    voiceCount = 0;
}

int SdlLoadVoiceCatalog(const char *path)
{
    SDL_RWops *file;
    Sint64 size;
    size_t position, index;
    unsigned int length;

    SdlShutdownVoices();
    file = SDL_RWFromFile(path, "rb");
    if (!file)
        return 0;
    size = SDL_RWsize(file);
    if (size < 12 || size > 4 * 1024 * 1024) {
        SDL_RWclose(file);
        goto invalid;
    }
    voiceCatalog = malloc((size_t)size);
    if (!voiceCatalog || SDL_RWread(file, voiceCatalog, 1, (size_t)size) != (size_t)size) {
        SDL_RWclose(file);
        goto invalid;
    }
    SDL_RWclose(file);
    if (memcmp(voiceCatalog, "WCV1", 4))
        goto invalid;
    voiceCount = Voice32(voiceCatalog + 4);
    length = Voice16(voiceCatalog + 8);
    if (!voiceCount || voiceCount > 16384 || length < 2 || length > 4096 ||
        length > (size_t)size - 10 || voiceCatalog[10 + length - 1] ||
        memchr(voiceCatalog + 10, 0, length - 1))
        goto invalid;
    voiceRoot = (const char *)voiceCatalog + 10;
    position = 10 + length;
    voiceEntries = calloc(voiceCount, sizeof(*voiceEntries));
    if (!voiceEntries)
        goto invalid;
    for (index = 0; index < voiceCount; index++) {
        unsigned int field, lengths[3];
        const char *fields[3];
        if ((size_t)size - position < 6)
            goto invalid;
        for (field = 0; field < 3; field++)
            lengths[field] = Voice16(voiceCatalog + position + field * 2);
        position += 6;
        for (field = 0; field < 3; field++) {
            length = lengths[field];
            if (length < 2 || length > 4096 || length > (size_t)size - position ||
                voiceCatalog[position + length - 1] || memchr(voiceCatalog + position, 0, length - 1))
                goto invalid;
            fields[field] = (const char *)voiceCatalog + position;
            position += length;
        }
        if ((strncmp(fields[0], "wc1/BRIEFING.000/packet/", sizeof("wc1/BRIEFING.000/packet/") - 1) &&
             strncmp(fields[0], "common/COMMUNIC.DAT/speaker/", sizeof("common/COMMUNIC.DAT/speaker/") - 1)) ||
            lengths[1] != 21 || strncmp(fields[1], "wc1:", 4) ||
            strspn(fields[1] + 4, "0123456789abcdef") != 16 ||
            fields[2][0] == '/' || strstr(fields[2], "..") ||
            strspn(fields[2], "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_./ -") != lengths[2] - 1)
            goto invalid;
        voiceEntries[index].context = fields[0];
        voiceEntries[index].id = fields[1];
        voiceEntries[index].path = fields[2];
        if (index) {
            int order = strcmp(voiceEntries[index - 1].context, fields[0]);
            if (order > 0 || (!order && strcmp(voiceEntries[index - 1].id, fields[1]) >= 0))
                goto invalid;
        }
    }
    if (position != (size_t)size)
        goto invalid;
    return 1;
invalid:
    SdlShutdownVoices();
    SDL_SetError("Invalid WC1 voice catalog: %s", path);
    return 0;
}

int SdlFindVoiceDirectory(const char *directory, char *root, unsigned long capacity)
{
    const char *layouts[] = {"Data/Voice", "WingLoader/Data/Voice", "../../Data/Voice"};
    char game[4096], path[8192], resolved[4096];
    size_t length, i;
    if (!directory || !root || !capacity || SDL_strlcpy(game, directory, sizeof(game)) >= sizeof(game))
        return 0;
    root[0] = 0;
    for (i = 0; game[i]; i++)
        if (game[i] == '\\')
            game[i] = '/';
    length = strlen(game);
    while (length > 1 && game[length - 1] == '/')
        game[--length] = 0;
    for (i = 0; i < SDL_arraysize(layouts); i++) {
        SDL_AudioSpec format;
        SDL_RWops *file;
        Uint8 *samples;
        Uint32 bytes;
        Sint64 size;
        int valid;
        /* Only follow parents for WingLoader's documented Games/WC1 layout. */
        if (i == 2 && (length < 10 || SDL_strcasecmp(game + length - 10, "/Games/WC1")))
            continue;
        if (SDL_snprintf(path, sizeof(path), "%s/%s/Angel/rig0000.wav", game, layouts[i]) >= (int)sizeof(path) ||
            !SdlResolvePath(path, resolved, sizeof(resolved)))
            continue;
        file = SDL_RWFromFile(resolved, "rb");
        if (!file)
            continue;
        size = SDL_RWsize(file);
        if (size < 44 || size > 32 * 1024 * 1024) {
            SDL_RWclose(file);
            continue;
        }
        if (!SDL_LoadWAV_RW(file, 1, &format, &samples, &bytes))
            continue;
        valid = bytes > 0 && (format.channels == 1 || format.channels == 2) &&
                (format.format == AUDIO_U8 || format.format == AUDIO_S16LSB) &&
                format.freq >= 4000 && format.freq <= 96000;
        SDL_FreeWAV(samples);
        if (!valid)
            continue;
        resolved[strlen(resolved) - strlen("/Angel/rig0000.wav")] = 0;
        if (SDL_strlcpy(root, resolved, capacity) >= capacity) {
            root[0] = 0;
            break;
        }
        SDL_ClearError();
        return 1;
    }
    SDL_ClearError();
    return 0;
}

int SdlLoadInstalledVoices(const char *directory)
{
    char root[4096], catalog[4096];
    char *base;
    int size;
    SdlShutdownVoices();
    if (!SdlFindVoiceDirectory(directory, root, sizeof(root)))
        return 1;
    base = SDL_GetBasePath();
    if (!base)
        return 0;
    size = SDL_snprintf(catalog, sizeof(catalog), "%svoices/segacd.dat", base);
    SDL_free(base);
    if (size < 0 || size >= (int)sizeof(catalog) || !SdlLoadVoiceCatalog(catalog))
        return 0;
    SDL_strlcpy(installedVoiceRoot, root, sizeof(installedVoiceRoot));
    voiceRoot = installedVoiceRoot;
    fprintf(stderr, "Detected Sega CD voices: %s\n", voiceRoot);
    return 1;
}

void SdlForgetVoicePacket(const void *data)
{
    VoicePacket **link = &voicePackets;
    while (*link) {
        VoicePacket *packet = *link;
        if (packet->data == data) {
            *link = packet->next;
            free(packet);
            return;
        }
        link = &packet->next;
    }
}

void SdlRegisterVoicePacket(const char *filename, short section,
                            const unsigned char *data, size_t size)
{
    const char *name = filename;
    const char *cursor;
    VoicePacket *packet;
    unsigned int pairs, pair;

    SdlForgetVoicePacket(data);
    if (!voiceCount || !data || section < 0)
        return;
    for (cursor = filename; *cursor; cursor++)
        if (*cursor == '/' || *cursor == '\\')
            name = cursor + 1;
    if (SDL_strcasecmp(name, "BRIEFING.000"))
        return;
    pairs = section == 0 ? 7 : section < 3 ? 1 : 5;
    if (size < pairs * 8 || size > 16 * 1024 * 1024)
        return;
    for (pair = 0; pair < pairs; pair++) {
        size_t scene = Voice32(data + pair * 8);
        size_t text = Voice32(data + pair * 8 + 4);
        size_t end = pair + 1 < pairs ? Voice32(data + (pair + 1) * 8) : size;
        size_t record;
        if (scene < pairs * 8 || scene > text || text > end || end > size || (text - scene) % 13)
            return;
        for (record = scene; record < text; record += 13) {
            size_t offset = Voice16(data + record + 7);
            if ((signed char)data[record] == -2)
                continue;
            if (offset >= end - text || !memchr(data + text + offset, 0, end - text - offset))
                return;
        }
    }
    packet = malloc(sizeof(*packet));
    if (!packet)
        return;
    packet->data = data;
    packet->pairs = pairs;
    packet->section = section;
    packet->next = voicePackets;
    voicePackets = packet;
}

static int PlayVoiceMapping(const char *context, const unsigned char *line)
{
    size_t first = 0, end = voiceCount;
    Uint64 hash = 14695981039346656037ULL;
    char id[21], path[8192], resolved[4096];
    for (; *line; line++) {
        hash ^= *line;
        hash *= 1099511628211ULL;
    }
    SDL_snprintf(id, sizeof(id), "wc1:%08x%08x", (unsigned int)(hash >> 32), (unsigned int)hash);
    while (first < end) {
        size_t middle = first + (end - first) / 2;
        int order = strcmp(context, voiceEntries[middle].context);
        if (!order)
            order = strcmp(id, voiceEntries[middle].id);
        if (order < 0)
            end = middle;
        else if (order > 0)
            first = middle + 1;
        else {
            if (SDL_snprintf(path, sizeof(path), "%s/%s", voiceRoot, voiceEntries[middle].path) >= (int)sizeof(path))
                return 0;
            return SdlResolvePath(path, resolved, sizeof(resolved)) && SdlPlayVoiceWav(resolved);
        }
    }
    return 0;
}

int SdlSceneVoicePlaying(void)
{
    return !radioVoice && SdlVoicePlaying();
}

int SdlRadioVoicePlaying(void)
{
    if (SdlUsingTownsData())
        return SdlTownsRadioPlaying();
    return radioVoice && SdlVoicePlaying();
}

void SdlStopRadioVoice(void)
{
    SdlStopTownsRadio();
    if (radioVoice)
        SdlStopVoice();
}

int SdlPlayRadioVoice(const char *filename, short speaker, short record, short rank, const char *text)
{
    const char *name = filename, *cursor;
    char context[128];
    int length;
    SdlStopVoice();
    radioVoice = 1;
    if (SdlUsingTownsData())
        return SdlPlayTownsRadio(speaker, record, rank);
    for (cursor = filename; *cursor; cursor++)
        if (*cursor == '/' || *cursor == '\\')
            name = cursor + 1;
    if (SDL_strcasecmp(name, "COMMUNIC.DAT") || speaker < 0 || speaker >= 14 || record < 0 || record >= 11 || !text)
        return 0;
    length = SDL_snprintf(context, sizeof(context), "common/COMMUNIC.DAT/speaker/%d/record/%d", speaker, record);
    if (rank >= 0 && rank < 5) {
        SDL_snprintf(context + length, sizeof(context) - length, "/rank/%d", rank);
        if (PlayVoiceMapping(context, (const unsigned char *)text))
            return 1;
        context[length] = 0;
    }
    return PlayVoiceMapping(context, (const unsigned char *)text);
}

int SdlPlaySceneVoice(const unsigned char *scene, const unsigned char *record)
{
    VoicePacket *packet;
    unsigned int pair;

    SdlStopVoice();
    radioVoice = 0;
    SdlStopTownsRadio();
    for (packet = voicePackets; packet; packet = packet->next) {
        for (pair = 0; pair < packet->pairs; pair++) {
            size_t start = Voice32(packet->data + pair * 8);
            size_t text = Voice32(packet->data + pair * 8 + 4);
            size_t index;
            const unsigned char *line;
            char context[128];
            if (scene != packet->data + start || (uintptr_t)record < (uintptr_t)scene ||
                (uintptr_t)record >= (uintptr_t)(packet->data + text))
                continue;
            index = (uintptr_t)record - (uintptr_t)scene;
            if (index % 13 || (signed char)record[0] == -2)
                return 0;
            line = packet->data + text + Voice16(record + 7);
            SDL_snprintf(context, sizeof(context), "wc1/BRIEFING.000/packet/%d/scene/%u/record/%u",
                         packet->section, pair, (unsigned int)(index / 13));
            return PlayVoiceMapping(context, line);
        }
    }
    return 0;
}
