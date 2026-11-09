/* SDL-only localization. Editable UTF-8 PO files are compiled to WC1 bytes by
 * bin/compileTranslations.py. Resources are translated in memory: original
 * scene commands, conditions, timing and animation remain byte-for-byte intact. */
#include "wc1.h"

#define TRANSLATION_LIMIT (16U * 1024U * 1024U)

typedef struct TranslationMessage {
    const char *context;
    const char *id;
    const char *text;
} TranslationMessage;

typedef struct TranslationFont {
    const unsigned char *glyphs;
    Uint32 crc;
    unsigned short count;
    unsigned char height;
    unsigned char ink;
    unsigned char background;
} TranslationFont;

typedef struct TranslationArtwork {
    const char *filename;
    const unsigned char *edits;
    Uint32 sourceSize, sourceCrc, targetSize, targetCrc, count;
    unsigned short section;
} TranslationArtwork;

static unsigned char *catalog;
static TranslationMessage *messages;
static size_t messageCount;
static TranslationFont fonts[4];
static TranslationArtwork *artwork;
static size_t artworkCount;
static int introFontUnavailable;

static unsigned int ReadTranslation16(const unsigned char *p)
{
    return p[0] | ((unsigned int)p[1] << 8);
}

static Uint32 ReadTranslation32(const unsigned char *p)
{
    return ReadTranslation16(p) | ((Uint32)ReadTranslation16(p + 2) << 16);
}

static void WriteTranslation32(unsigned char *p, size_t value)
{
    p[0] = (unsigned char)value;
    p[1] = (unsigned char)(value >> 8);
    p[2] = (unsigned char)(value >> 16);
    p[3] = (unsigned char)(value >> 24);
}

static const char *TranslationBasename(const char *filename)
{
    const char *name = filename;
    for (; *filename; filename++) {
        if (*filename == '/' || *filename == '\\')
            name = filename + 1;
    }
    return name;
}

void SdlShutdownTranslations(void)
{
    free(messages);
    free(artwork);
    SDL_free(catalog);
    messages = 0;
    catalog = 0;
    messageCount = 0;
    artwork = 0;
    artworkCount = 0;
    introFontUnavailable = 0;
    memset(fonts, 0, sizeof(fonts));
}

int SdlLoadTranslationCatalog(const char *path)
{
    SDL_RWops *file;
    Sint64 size;
    size_t position;
    size_t index;
    size_t fontCount;

    SdlShutdownTranslations();
    file = SDL_RWFromFile(path, "rb");
    if (!file)
        return 0;
    size = SDL_RWsize(file);
    if (size < 12 || size > TRANSLATION_LIMIT) {
        SDL_RWclose(file);
        SDL_SetError("Invalid translation catalog size");
        return 0;
    }
    catalog = SDL_malloc((size_t)size);
    if (!catalog) {
        SDL_RWclose(file);
        return 0;
    }
    if (SDL_RWread(file, catalog, 1, (size_t)size) != (size_t)size) {
        SDL_RWclose(file);
        goto invalid;
    }
    SDL_RWclose(file);
    if (memcmp(catalog, "WCT2", 4) != 0)
        goto invalid;
    messageCount = ReadTranslation32(catalog + 4);
    fontCount = ReadTranslation32(catalog + 8);
    if (!messageCount || messageCount > (size_t)size / 12 || fontCount > 4)
        goto invalid;
    messages = calloc(messageCount, sizeof(*messages));
    if (!messages)
        goto invalid;
    position = 12;
    for (index = 0; index < messageCount; index++) {
        unsigned int lengths[3];
        const char *fields[3];
        size_t field;
        if ((size_t)size - position < 6)
            goto invalid;
        for (field = 0; field < 3; field++)
            lengths[field] = ReadTranslation16(catalog + position + field * 2);
        position += 6;
        for (field = 0; field < 3; field++) {
            size_t length = lengths[field];
            if (length < (field == 0 ? 1U : 2U) || length > (size_t)size - position ||
                memchr(catalog + position, 0, length) != catalog + position + length - 1)
                goto invalid;
            fields[field] = (const char *)catalog + position;
            position += length;
        }
        if (lengths[1] != 21 || strncmp(fields[1], "wc1:", 4) != 0 ||
            strspn(fields[1] + 4, "0123456789abcdef") != 16)
            goto invalid;
        messages[index].context = fields[0];
        messages[index].id = fields[1];
        messages[index].text = fields[2];
        if (index) {
            int order = strcmp(messages[index - 1].context, fields[0]);
            if (order > 0 || (order == 0 && strcmp(messages[index - 1].id, fields[1]) >= 0))
                goto invalid;
        }
    }
    for (index = 0; index < fontCount; index++) {
        TranslationFont *font;
        size_t glyph;
        unsigned char seen[256] = {0};
        if ((size_t)size - position < 10 || catalog[position] >= 4)
            goto invalid;
        font = &fonts[catalog[position]];
        if (font->glyphs)
            goto invalid;
        font->height = catalog[position + 1];
        font->ink = catalog[position + 2];
        font->background = catalog[position + 3];
        font->crc = ReadTranslation32(catalog + position + 4);
        font->count = ReadTranslation16(catalog + position + 8);
        position += 10;
        if (!font->height || font->height > 32 || font->count > 256)
            goto invalid;
        font->glyphs = catalog + position;
        for (glyph = 0; glyph < font->count; glyph++) {
            size_t pixels;
            if ((size_t)size - position < 2 || seen[catalog[position]]++)
                goto invalid;
            pixels = catalog[position + 1] * font->height;
            if (catalog[position + 1] > 32 || pixels > (size_t)size - position - 2)
                goto invalid;
            position += pixels + 2;
        }
    }
    /* Optional sparse image changes. Validate every splice before retaining
     * pointers into the file. */
    if (position != (size_t)size) {
        if ((size_t)size - position < 8 || memcmp(catalog + position, "WCA1", 4))
            goto invalid;
        artworkCount = ReadTranslation32(catalog + position + 4);
        position += 8;
        if (!artworkCount || artworkCount > 256)
            goto invalid;
        artwork = calloc(artworkCount, sizeof(*artwork));
        if (!artwork)
            goto invalid;
        for (index = 0; index < artworkCount; index++) {
            TranslationArtwork *patch = &artwork[index];
            size_t nameLength, edit, previous = 0;
            Sint64 resultSize;
            if ((size_t)size - position < 24)
                goto invalid;
            nameLength = ReadTranslation16(catalog + position);
            patch->section = ReadTranslation16(catalog + position + 2);
            patch->sourceSize = ReadTranslation32(catalog + position + 4);
            patch->sourceCrc = ReadTranslation32(catalog + position + 8);
            patch->targetSize = ReadTranslation32(catalog + position + 12);
            patch->targetCrc = ReadTranslation32(catalog + position + 16);
            patch->count = ReadTranslation32(catalog + position + 20);
            position += 24;
            if (nameLength < 2 || nameLength > 64 || nameLength > (size_t)size - position ||
                memchr(catalog + position, 0, nameLength) != catalog + position + nameLength - 1 ||
                !patch->sourceSize || patch->sourceSize > TRANSLATION_LIMIT ||
                !patch->targetSize || patch->targetSize > TRANSLATION_LIMIT ||
                !patch->count || patch->count > 65536 || patch->section >= 32768)
                goto invalid;
            patch->filename = (const char *)catalog + position;
            position += nameLength;
            patch->edits = catalog + position;
            resultSize = patch->sourceSize;
            for (edit = 0; edit < patch->count; edit++) {
                size_t offset, removed, inserted;
                if ((size_t)size - position < 12)
                    goto invalid;
                offset = ReadTranslation32(catalog + position);
                removed = ReadTranslation32(catalog + position + 4);
                inserted = ReadTranslation32(catalog + position + 8);
                position += 12;
                if (offset < previous || offset > patch->sourceSize ||
                    removed > patch->sourceSize - offset || inserted > (size_t)size - position)
                    goto invalid;
                previous = offset + removed;
                resultSize += (Sint64)inserted - removed;
                position += inserted;
            }
            if (resultSize != patch->targetSize)
                goto invalid;
        }
    }
    if (position != (size_t)size)
        goto invalid;
    return 1;
invalid:
    SdlShutdownTranslations();
    SDL_SetError("Invalid WC1 translation catalog: %s", path);
    return 0;
}

int SdlSetLanguage(int language)
{
    char *base;
    char path[4096];
    int result;

    if (language == 0) {
        SdlShutdownTranslations();
        return 1;
    }
    if (language != 1 && language != 3 && (language != 2 || !SdlUsingTownsJapanese()))
        return 0;
    base = SDL_GetBasePath();
    if (!base)
        return 0;
    result = SDL_snprintf(path, sizeof(path), "%stranslations/%s.dat", base,
                          language == 1 ? "es" : language == 3 ? "fr" : "ja");
    SDL_free(base);
    if (result < 0 || result >= (int)sizeof(path))
        return 0;
    return SdlLoadTranslationCatalog(path);
}

static const char *FindTranslation(const char *context, const char *source)
{
    size_t first = 0;
    size_t end = messageCount;
    Uint64 hash = 14695981039346656037ULL;
    const unsigned char *byte = (const unsigned char *)source;
    char id[21];
    if (!messageCount || !source || (introFontUnavailable && strcmp(context, "intro") == 0))
        return source;
    /* FNV-1a over the original CP437 bytes, matching message_key(). */
    for (; *byte; byte++) {
        hash ^= *byte;
        hash *= 1099511628211ULL;
    }
    SDL_snprintf(id, sizeof(id), "wc1:%08x%08x",
                 (unsigned int)(hash >> 32), (unsigned int)hash);
    while (first < end) {
        size_t middle = first + (end - first) / 2;
        int order = strcmp(context, messages[middle].context);
        if (order == 0)
            order = strcmp(id, messages[middle].id);
        if (order < 0)
            end = middle;
        else if (order > 0)
            first = middle + 1;
        else
            return messages[middle].text;
    }
    return source;
}

char *SdlTranslateText(const char *context, const char *source)
{
    return (char *)SdlTranslateTownsText(FindTranslation(context, source));
}

static int TranslateFont(short section, const unsigned char *source, size_t size,
                          unsigned char **output, size_t *outputSize)
{
    TranslationFont *font = &fonts[section];
    const unsigned char *glyph = font->glyphs;
    unsigned char *result;
    size_t total = size;
    size_t index;

    if (!font->glyphs)
        return 1;
    if (size < 772 || ReadTranslation16(source) != font->height ||
        source[2] != font->ink || source[3] != font->background ||
        SDL_crc32(0, source, size) != font->crc) {
        SDL_SetError("Translation requires the original WC1 fonts");
        return 0;
    }
    for (index = 0; index < font->count; index++) {
        total += glyph[1] * font->height;
        glyph += 2 + glyph[1] * font->height;
    }
    if (total > 65535)
        return 0;
    result = malloc(total);
    if (!result)
        return 0;
    memcpy(result, source, size);
    glyph = font->glyphs;
    for (index = 0; index < font->count; index++) {
        size_t pixels = glyph[1] * font->height;
        unsigned char character = glyph[0];
        result[4 + character] = glyph[1];
        result[260 + character] = (unsigned char)size;
        result[516 + character] = (unsigned char)(size >> 8);
        memcpy(result + size, glyph + 2, pixels);
        size += pixels;
        glyph += pixels + 2;
    }
    *output = result;
    *outputSize = total;
    return 1;
}

static int TranslateBriefing(const char *domain, const char *name, short section,
                             const unsigned char *source, size_t size,
                             unsigned char **output, size_t *outputSize)
{
    size_t pairs = SDL_strncasecmp(name, "MIDGAME.", 8) == 0 ? 1 :
        section == 0 ? 7 : section < 3 ? 1 : 5;
    size_t pair;
    size_t used;
    size_t capacity;
    unsigned char *result;

    /* Unknown layouts (including nested archives in some installations) are
     * passed through. Check every offset before rebuilding any scene. */
    if (size < pairs * 8)
        return 1;
    used = ReadTranslation32(source);
    if (used < pairs * 8 || used > size)
        return 1;
    for (pair = 0; pair < pairs; pair++) {
        size_t scene = ReadTranslation32(source + pair * 8);
        size_t text = ReadTranslation32(source + pair * 8 + 4);
        size_t end = pair + 1 < pairs ? ReadTranslation32(source + (pair + 1) * 8) : size;
        size_t record;
        if (scene > text || text > end || end > size || (text - scene) % 13)
            return 1;
        for (record = scene; record < text; record += 13) {
            size_t offset = ReadTranslation16(source + record + 7);
            if ((signed char)source[record] == -2)
                continue;
            if (offset >= end - text || !memchr(source + text + offset, 0, end - text - offset))
                return 1;
        }
    }
    capacity = size;
    result = malloc(capacity);
    if (!result)
        return 0;
    memcpy(result, source, used);
    for (pair = 0; pair < pairs; pair++) {
        size_t scene = ReadTranslation32(source + pair * 8);
        size_t text = ReadTranslation32(source + pair * 8 + 4);
        size_t end = pair + 1 < pairs ? ReadTranslation32(source + (pair + 1) * 8) : size;
        size_t newScene = used;
        size_t newText = used + text - scene;
        size_t record;
        size_t reserve = end - scene + ((text - scene) / 13) * 256;
        unsigned char *grown;
        if (reserve > TRANSLATION_LIMIT - used) {
            free(result);
            return 0;
        }
        if (used + reserve > capacity) {
            capacity = used + reserve;
            grown = realloc(result, capacity);
            if (!grown) {
                free(result);
                return 0;
            }
            result = grown;
        }
        WriteTranslation32(result + pair * 8, newScene);
        WriteTranslation32(result + pair * 8 + 4, newText);
        memcpy(result + used, source + scene, end - scene);
        used += end - scene;
        for (record = 0; record < (text - scene) / 13; record++) {
            const unsigned char *command = source + scene + record * 13;
            const char *original;
            const char *translated;
            char context[128];
            size_t length;
            size_t offset;
            if ((signed char)command[0] == -2)
                continue;
            original = (const char *)source + text + ReadTranslation16(command + 7);
            SDL_snprintf(context, sizeof(context), "%s/%s/packet/%d/scene/%u/record/%u",
                         domain, name, section, (unsigned int)pair, (unsigned int)record);
            translated = FindTranslation(context, original);
            length = strlen(translated) + 1;
            offset = used - newText;
            if (translated == original || length > 256 || offset + length > 32768)
                continue;
            memcpy(result + used, translated, length);
            used += length;
            result[newScene + record * 13 + 7] = (unsigned char)offset;
            result[newScene + record * 13 + 8] = (unsigned char)(offset >> 8);
        }
    }
    *output = result;
    *outputSize = used;
    return 1;
}

static int TranslateArtwork(const char *name, short section,
                            const unsigned char *source, size_t size,
                            unsigned char **output, size_t *outputSize)
{
    size_t index;
    for (index = 0; index < artworkCount; index++) {
        const TranslationArtwork *patch = &artwork[index];
        const unsigned char *edit = patch->edits;
        unsigned char *result;
        size_t i, input = 0, used = 0;
        if (patch->section != section || SDL_strcasecmp(name, patch->filename))
            continue;
        if (size != patch->sourceSize || SDL_crc32(0, source, size) != patch->sourceCrc) {
            if (section == 1 && SDL_strcasecmp(name, "TITLE.VGA") == 0)
                introFontUnavailable = size != patch->targetSize ||
                    SDL_crc32(0, source, size) != patch->targetCrc;
            return 1;
        }
        result = malloc(patch->targetSize);
        if (!result)
            return 0;
        for (i = 0; i < patch->count; i++) {
            size_t offset = ReadTranslation32(edit);
            size_t removed = ReadTranslation32(edit + 4);
            size_t inserted = ReadTranslation32(edit + 8);
            size_t copied = offset - input;
            edit += 12;
            if (copied > patch->targetSize - used || inserted > patch->targetSize - used - copied) {
                free(result);
                return 0;
            }
            memcpy(result + used, source + input, copied);
            used += copied;
            memcpy(result + used, edit, inserted);
            used += inserted;
            input = offset + removed;
            edit += inserted;
        }
        if (size - input != patch->targetSize - used) {
            free(result);
            return 0;
        }
        memcpy(result + used, source + input, size - input);
        if (SDL_crc32(0, result, patch->targetSize) != patch->targetCrc) {
            free(result);
            return 0;
        }
        if (section == 1 && SDL_strcasecmp(name, "TITLE.VGA") == 0)
            introFontUnavailable = 0;
        *output = result;
        *outputSize = patch->targetSize;
        return 1;
    }
    return 1;
}

/* Public decoded-resource entry point also used by the rendering regression
 * probe. The caller owns *output; NULL means the input is unchanged. */
int SdlTranslatePacketData(const char *filename, short section,
                          const unsigned char *source, size_t size,
                          unsigned char **output, size_t *outputSize)
{
    const char *name = TranslationBasename(filename);
    const char *domain;
    char canonical[20];
    int campaign;
    int briefing;
    size_t stride, field, capacity, index;
    const char *label;
    unsigned char *result;

    *output = 0;
    *outputSize = size;
    if (!catalog || section < 0 || size > TRANSLATION_LIMIT)
        return 1;
    if (SDL_strcasecmp(name, "FONTS.FNT") == 0 && section < 4)
        return TranslateFont(section, source, size, output, outputSize);
    if (SDL_strncasecmp(name, "MIDGAME.V0", 10) == 0 && strlen(name) == 11 &&
        name[10] >= '0' && name[10] <= '8' && (section == 5 || section == 6)) {
        SDL_snprintf(canonical, sizeof(canonical), "MIDGAME.V0%c", name[10]);
        return TranslateBriefing("common", canonical, section, source, size, output, outputSize);
    }
    if (SDL_strcasecmp(name, "TITLE.VGA") == 0 || SDL_strcasecmp(name, "TITLE1.VGA") == 0 ||
        SDL_strcasecmp(name, "COCKPIT.VGA") == 0 || SDL_strcasecmp(name, "RECROOM.VGA") == 0 ||
        SDL_strcasecmp(name, "SERIES.VGA") == 0)
        return TranslateArtwork(name, section, source, size, output, outputSize);
    briefing = SDL_strncasecmp(name, "BRIEFING.", 9) == 0;
    if ((!briefing && SDL_strncasecmp(name, "MODULE.", 7) != 0) ||
        strlen(name) != (briefing ? 12 : 10))
        return 1;
    name += briefing ? 9 : 7;
    if (name[0] != '0' || name[1] != '0' || name[2] < '0' || name[2] > '2')
        return 1;
    campaign = name[2] - '0';
    domain = campaign == 0 ? "wc1" : campaign == 1 ? "sm1" : "sm2";
    SDL_snprintf(canonical, sizeof(canonical), "%s.00%d", briefing ? "BRIEFING" : "MODULE", campaign);
    if (briefing)
        return TranslateBriefing(domain, canonical, section, source, size, output, outputSize);
    switch (section) {
    case 1:
        stride = 77; field = 0; capacity = 30; label = "nav";
        break;
    case 2:
        stride = 64; field = 4; capacity = 60; label = "objective";
        break;
    case 4:
        stride = 40; field = 0; capacity = 40; label = "mission-name";
        break;
    case 5:
        stride = 40; field = 0; capacity = 40; label = "series-name";
        break;
    default:
        return 1;
    }
    if (!size || size % stride)
        return 1;
    result = malloc(size);
    if (!result)
        return 0;
    memcpy(result, source, size);
    for (index = 0; index < size / stride; index++) {
        char context[128];
        const char *original = (const char *)source + index * stride + field;
        const char *translated;
        if (!memchr(original, 0, capacity))
            continue;
        if (section < 3)
            SDL_snprintf(context, sizeof(context), "%s/%s/%s/mission/%u/record/%u",
                         domain, canonical, label, (unsigned int)(index / 16), (unsigned int)(index % 16));
        else
            SDL_snprintf(context, sizeof(context), "%s/%s/%s/record/%u",
                         domain, canonical, label, (unsigned int)index);
        translated = FindTranslation(context, original);
        if (translated != original && strlen(translated) < capacity)
            memcpy(result + index * stride + field, translated, strlen(translated) + 1);
    }
    *output = result;
    return 1;
}

int SdlTranslatePacketSection(const char *filename, short section,
                              PacketSectionHandle *handle)
{
    const char *name = TranslationBasename(filename);
    unsigned char *stored = 0;
    unsigned char *decoded = 0;
    unsigned char *result = 0;
    size_t size = handle->dataSize;
    size_t resultSize = size;
    size_t readSize = 0;
    size_t written;
    int success = 0;

    if (!catalog || (SDL_strncasecmp(name, "BRIEFING.", 9) &&
                     SDL_strncasecmp(name, "MODULE.", 7) &&
                     SDL_strcasecmp(name, "FONTS.FNT") &&
                     SDL_strncasecmp(name, "MIDGAME.V0", 10) &&
                     SDL_strcasecmp(name, "TITLE.VGA") &&
                     SDL_strcasecmp(name, "TITLE1.VGA") &&
                     SDL_strcasecmp(name, "COCKPIT.VGA") &&
                     SDL_strcasecmp(name, "RECROOM.VGA") &&
                     SDL_strcasecmp(name, "SERIES.VGA")))
        return 1;
    if (!size)
        return 1;
    if (size > TRANSLATION_LIMIT)
        goto done;
    if (handle->patchedData) {
        decoded = handle->patchedData;
    } else {
        stored = malloc(size);
        if (!stored || _lseek(handle->file, handle->dataOffset, SEEK_SET) == -1)
            goto done;
        while (readSize < size) {
            int count = _read(handle->file, stored + readSize, size - readSize);
            if (count <= 0)
                goto done;
            readSize += count;
        }
        if (handle->compression == 1) {
            if (size < 4)
                goto done;
            size = ReadTranslation32(stored);
            if (!size || size > TRANSLATION_LIMIT)
                goto done;
            decoded = malloc(size);
            if (!decoded || !SdlDecompressOriginLzw(stored + 4, readSize - 4, decoded, size, &written) || written != size)
                goto done;
        } else {
            decoded = stored;
        }
    }
    success = SdlTranslatePacketData(filename, section, decoded, size, &result, &resultSize);
done:
    if (decoded != stored && decoded != handle->patchedData)
        free(decoded);
    free(stored);
    if (_lseek(handle->file, handle->dataOffset, SEEK_SET) == -1)
        success = 0;
    if (!success) {
        free(result);
        nPacketError = 6;
        return 0;
    }
    if (result) {
        free(handle->patchedData);
        handle->patchedData = result;
        handle->dataSize = (unsigned int)resultSize;
        handle->compression = 0;
    }
    return 1;
}

void SdlTranslateRadio(const char *filename, short speaker, short line,
                       char *text, size_t capacity)
{
    const char *name = TranslationBasename(filename);
    const char *domain;
    const char *translated;
    char context[96];

    if (!catalog || !memchr(text, 0, capacity))
        return;
    if (SDL_strcasecmp(name, "COMMUNIC.DAT") == 0) {
        name = "COMMUNIC.DAT";
        domain = "common";
    } else if (SDL_strcasecmp(name, "COMMSM2.DAT") == 0) {
        name = "COMMSM2.DAT";
        domain = "sm2";
    } else {
        return;
    }
    SDL_snprintf(context, sizeof(context), "%s/%s/speaker/%d/record/%d", domain, name, speaker, line);
    translated = FindTranslation(context, text);
    if (translated != text && strlen(translated) < capacity)
        memcpy(text, translated, strlen(translated) + 1);
}
