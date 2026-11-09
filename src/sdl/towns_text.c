/* FM Towns Shift-JIS text. WC.EXP's renderer (load image 0x9f2ec) uses
 * 8/4 logical pixels per full/half-width glyph and 10 pixels per line.
 * Misaki replaces the machine's font ROM; game strings stay on the disc. */
#include "wc1.h"

extern const unsigned char abTownsFont[];
static int townsJapanese;
static unsigned char *townsTextImage;
static const char *townsNames[34][2];
static unsigned char pendingLead;
static TextContext *pendingContext;
static short pendingX, pendingY;

static const char *TownsImageString(size_t pointer)
{
    unsigned int offset;
    const unsigned char *p = townsTextImage + 0x200 + pointer;
    offset = p[0] | (unsigned int)p[1] << 8 | (unsigned int)p[2] << 16 | (unsigned int)p[3] << 24;
    if (offset >= 743037 - 0x200 ||
        !memchr(townsTextImage + 0x200 + offset, 0, 743037 - 0x200 - offset))
        return "";
    return (const char *)townsTextImage + 0x200 + offset;
}

int SdlSetTownsLanguage(int japanese)
{
    char path[PATH_MAX];
    size_t size;
    int index;
    townsJapanese = 0;
    pendingLead = 0;
    SDL_free(townsTextImage);
    townsTextImage = 0;
    if (!japanese)
        return 1;
    if (!SdlUsingTownsData()) {
        SDL_SetError("Japanese requires FM Towns game data");
        return 0;
    }
    if (!SdlResolvePath("WC.EXP", path, sizeof(path)) &&
        !SdlResolvePath("../WC.EXP", path, sizeof(path))) {
        SDL_SetError("Japanese requires the FM Towns WC.EXP");
        return 0;
    }
    townsTextImage = SDL_LoadFile(path, &size);
    if (!townsTextImage || size != 743037 || memcmp(townsTextImage, "P3", 2))
        goto invalid;
    /* Original bilingual rank/medal/system tables, not copied game text. */
    for (index = 0; index < 34; index++) {
        size_t english = index < 5 ? 0x10a34 + index * 16 :
            index < 10 ? 0x6b150 + (index - 5) * 8 : 0x6b178 + (index - 10) * 4;
        size_t japanese = index < 10 ? english + 4 : english + 0x64;
        /* The Towns edition uses a different rank ladder at the same indices. */
        townsNames[index][0] = index < 5 ? apszPilotRankNames[index] : TownsImageString(english);
        townsNames[index][1] = TownsImageString(japanese);
        if (!*townsNames[index][0] || !*townsNames[index][1])
            goto invalid;
    }
    townsJapanese = 1;
    return 1;

invalid:
    SDL_free(townsTextImage);
    townsTextImage = 0;
    SDL_SetError("Unsupported FM Towns WC.EXP (expected F1.0)");
    return 0;
}

int SdlUsingTownsJapanese(void)
{
    return townsJapanese;
}

const char *SdlTranslateTownsText(const char *text)
{
    int index;
    if (townsJapanese) {
        for (index = 0; index < 34; index++) {
            if (*townsNames[index][0] && !SDL_strcasecmp(text, townsNames[index][0]))
                return townsNames[index][1];
        }
    }
    return text;
}

const char *SdlTownsFilename(const char *path)
{
    static char translated[PATH_MAX];
    char *name;
    if (!townsJapanese)
        return path;
    if (SDL_strlcpy(translated, path, sizeof(translated)) >= sizeof(translated))
        return path;
    name = translated + strlen(translated);
    while (name > translated && name[-1] != '/' && name[-1] != '\\')
        name--;
    if (!SDL_strncasecmp(name, "BRIEFING.00", 11) && strlen(name) == 12 &&
        name[11] >= '0' && name[11] <= '2')
        name[9] = 'J';
    else if (!SDL_strncasecmp(name, "MIDGAME.V0", 10) && strlen(name) == 11 &&
             name[10] >= '0' && name[10] <= '8')
        name[9] = 'J';
    else if (!SDL_strcasecmp(name, "COMMUNIC.DAT"))
        strcpy(name, nCampaignDataSet == 2 ? "COMMSM2.JAT" : "COMMUNIC.JAT");
    return translated;
}

static int TownsLead(unsigned char value)
{
    return (value >= 0x81 && value <= 0x9f) || (value >= 0xe0 && value <= 0xef);
}

static int TownsTrail(unsigned char value)
{
    return value >= 0x40 && value <= 0xfc && value != 0x7f;
}

static unsigned int TownsGlyph(const unsigned char **text, int *width)
{
    unsigned int first = *(*text)++, second, row, column;
    *width = 4;
    if (!TownsLead(first))
        return first;
    *width = 8;
    if (!TownsTrail(**text))
        return 0; /* Visible replacement; never swallow a newline or NUL. */
    second = *(*text)++;
    row = (first - (first < 0xa0 ? 0x81 : 0xc1)) * 2;
    if (second >= 0x9f) {
        row++;
        column = second - 0x9f;
    } else {
        column = second - (second < 0x7f ? 0x40 : 0x41);
    }
    return 256 + row * 94 + column;
}

static int TownsJapaneseText(const char *text)
{
    if (!townsJapanese)
        return 0;
    for (; *text; text++) {
        if ((unsigned char)*text >= 0x80)
            return 1;
    }
    return 0;
}

static void TownsDrawGlyph(unsigned int glyph, int width)
{
    TextContext *context = pCurTextContext;
    Viewport *viewport = context->viewport;
    int x, y;
    for (y = 0; y < 8; y++) {
        int targetY = context->cursorY + y;
        unsigned int offset;
        if (targetY < viewport->top || targetY > viewport->bottom)
            continue;
        offset = viewport->rowOffsets[targetY];
        if (targetY == viewport->top && (offset & 0x8000))
            offset = 0;
        for (x = 0; x < width; x++) {
            int targetX = context->cursorX + x;
            unsigned char colour = abTownsFont[glyph * 8 + y] & (0x80 >> x) ?
                context->colour : context->backgroundColour;
            if (targetX >= viewport->left && targetX <= viewport->right && colour != 0xff)
                viewport->pixels[offset + targetX] = colour;
        }
    }
    context->cursorX += width;
}

int SdlMeasureTownsText(const char *text)
{
    const unsigned char *cursor = (const unsigned char *)text;
    int width = 0, maximum = 0, glyphWidth;
    if (!TownsJapaneseText(text))
        return -1;
    while (*cursor) {
        if (*cursor == '\n' || *cursor == '\r') {
            maximum = SDL_max(maximum, width);
            width = 0;
            cursor++;
            continue;
        }
        TownsGlyph(&cursor, &glyphWidth);
        if (width + glyphWidth >= 320)
            break;
        width += glyphWidth;
    }
    return SDL_max(maximum, width);
}

int SdlDrawTownsText(const char *text)
{
    const unsigned char *cursor = (const unsigned char *)text;
    TextContext *context = pCurTextContext;
    Viewport *viewport = context->viewport;
    int origin = context->cursorX;
    pendingLead = 0;
    if (!TownsJapaneseText(text))
        return 0;
    while (*cursor && context->cursorY <= viewport->bottom) {
        const unsigned char *start = cursor;
        int width = 0, glyphWidth;
        while (*cursor && *cursor != '\n' && *cursor != '\r') {
            const unsigned char *next = cursor;
            TownsGlyph(&next, &glyphWidth);
            if (width && origin + width + glyphWidth > viewport->right + 1)
                break;
            width += glyphWidth;
            cursor = next;
        }
        context->cursorX = context->alignment == 2 ?
            viewport->left + (viewport->right - viewport->left + 1 - width) / 2 :
            context->alignment == 1 ? viewport->right + 1 - width : origin;
        while (start < cursor) {
            unsigned int glyph = TownsGlyph(&start, &glyphWidth);
            TownsDrawGlyph(glyph, glyphWidth);
        }
        if (*cursor) {
            if (*cursor == '\r')
                cursor++;
            if (*cursor == '\n')
                cursor++;
            context->cursorY += 10;
            origin = viewport->left;
            context->cursorX = origin;
        }
    }
    return 1;
}

int SdlDrawTownsCharacter(unsigned char character)
{
    TextContext *context = pCurTextContext;
    if (!townsJapanese)
        return 0;
    if (pendingLead) {
        unsigned char pair[3] = {pendingLead, character, 0};
        const unsigned char *cursor = pair;
        int width;
        pendingLead = 0;
        if (context == pendingContext && context->cursorX == pendingX &&
            context->cursorY == pendingY && TownsTrail(character)) {
            unsigned int glyph = TownsGlyph(&cursor, &width);
            TownsDrawGlyph(glyph, width);
            return 1;
        }
    }
    if (TownsLead(character)) {
        pendingLead = character;
        pendingContext = context;
        pendingX = context->cursorX;
        pendingY = context->cursorY;
        return 1;
    }
    if (character >= 0x80) {
        TownsDrawGlyph(character, 4);
        return 1;
    }
    return 0;
}
