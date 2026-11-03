/* SDL replacement for QuickDraw text, shared by the SWC cockpit and rooms. */
#include "swc.h"
#include "wc1sdl.h"
#include "swc_hud_font.h"

#include <string.h>
#ifdef __APPLE__
#include <CoreText/CoreText.h>
#include <CoreGraphics/CoreGraphics.h>
#endif

SDL_Texture *SdlCreateSwcFont(SDL_Renderer *renderer)
{
    SDL_Surface *surface;
    SDL_Texture *font;
    uint32_t *row;
    int character;
    int x;
    int y;

    surface = SDL_CreateRGBSurfaceWithFormat(0, 16 * SWC_HUD_FONT_WIDTH,
        6 * SWC_HUD_FONT_HEIGHT, 32, SDL_PIXELFORMAT_ARGB8888);
    if (surface == NULL)
        return NULL;
    SDL_FillRect(surface, NULL, 0);
    for (character = 0; character < 95; character++) {
        for (y = 0; y < SWC_HUD_FONT_HEIGHT; y++) {
            row = (uint32_t *)((uint8_t *)surface->pixels +
                (character / 16 * SWC_HUD_FONT_HEIGHT + y) * surface->pitch);
            for (x = 0; x < SWC_HUD_FONT_WIDTH; x++) {
                if (abSwcHudFont[character][y] & (0x80 >> x))
                    row[character % 16 * SWC_HUD_FONT_WIDTH + x] = UINT32_C(0xffffffff);
            }
        }
    }
    font = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (font != NULL && SDL_SetTextureBlendMode(font, SDL_BLENDMODE_BLEND) != 0) {
        SDL_DestroyTexture(font);
        return NULL;
    }
    return font;
}

int SdlDrawSwcText(SDL_Renderer *renderer, SDL_Texture *font, int x,
                    int baseline, SDL_Color ink, const char *text)
{
    SDL_Rect source = {0, 0, SWC_HUD_FONT_WIDTH, SWC_HUD_FONT_HEIGHT};
    SDL_Rect destination = {x - SWC_HUD_FONT_BEARING,
        baseline - SWC_HUD_FONT_BASELINE, SWC_HUD_FONT_WIDTH, SWC_HUD_FONT_HEIGHT};
    unsigned char character;

    if (SDL_SetTextureColorMod(font, ink.r, ink.g, ink.b) != 0)
        return -1;
    while (*text != 0 && destination.x < SWC_FRAME_WIDTH) {
        character = *text++;
        if (character < 32 || character > 126)
            character = '?';
        character -= 32;
        source.x = character % 16 * SWC_HUD_FONT_WIDTH;
        source.y = character / 16 * SWC_HUD_FONT_HEIGHT;
        if (SDL_RenderCopy(renderer, font, &source, &destination) != 0)
            return -1;
        destination.x += SWC_HUD_FONT_ADVANCE;
    }
    return 0;
}

int SdlDrawSwcSubtitle(SDL_Renderer *renderer, SDL_Texture *font,
                        const char *text)
{
    char lines[5][61];
    SDL_Color ink = {245, 245, 230, 255};
    SDL_Rect panel;
    size_t length;
    size_t count;
    int line = 0;
    int index;

    while (*text != 0 && line < (int)SDL_arraysize(lines)) {
        while (*text == ' ' || *text == '\n' || *text == '\r')
            text++;
        if (*text == 0)
            break;
        length = strcspn(text, "\r\n");
        count = SDL_min(length, sizeof(lines[0]) - 1);
        if (length > count) {
            while (count > 0 && text[count] != ' ')
                count--;
            if (count == 0)
                count = sizeof(lines[0]) - 1;
        }
        memcpy(lines[line], text, count);
        lines[line++][count] = 0;
        text += count;
    }
    panel = (SDL_Rect){4, 236 - line * 12, 312, line * 12 + 2};
    if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) != 0 ||
        SDL_RenderFillRect(renderer, &panel) != 0)
        return -1;
    for (index = 0; index < line; index++) {
        if (SdlDrawSwcText(renderer, font, 10, panel.y + 9 + index * 12,
                           ink, lines[index]) != 0)
            return -1;
    }
    return 0;
}

/* QuickDraw FontRec/NFNT, documented in Inside Macintosh: Text, 4-62.
 * Read the supplied monochrome strike verbatim: no outline rasterization
 * or scaling. A missing strike returns 1 so the host can use its fallback. */
static int SwcLoadClassicFont(SDL_Renderer *renderer, SwcTextFont *font, int size)
{
    const char *resourcePath = SDL_getenv("SWC_MAC_FONT_RESOURCE");
    char path[PATH_MAX];
    SwcBuffer family = {0};
    SwcBuffer bitmap = {0};
    SDL_RWops *reader = NULL;
    SDL_Surface *surface = NULL;
    uint32_t *row;
    size_t locations;
    size_t widths;
    size_t stride;
    size_t offset;
    int id = 21 * 128 + size;
    int count;
    int index;
    int first;
    int last;
    int height;
    int ascent;
    int kern;
    int highOffset;
    int character;
    int glyph;
    int left;
    int right;
    int metrics;
    int x;
    int y;
    int result = 1;

    if (resourcePath == NULL || *resourcePath == 0) {
        if (!SdlResolvePath("Fonts/Helvetica.rsrc", path, sizeof(path)))
            return 1;
        resourcePath = path;
    }
    /* NFNT IDs are mapped by the FOND association table; FONT resources
     * without a FOND use the older family*128 + point-size convention. */
    if (SwcReadResource(resourcePath, "FOND", 21, &family) == 0) {
        if (family.size < 54)
            goto malformed;
        reader = SDL_RWFromConstMem(family.data, family.size);
        if (reader == NULL)
            goto done;
        SDL_RWseek(reader, 52, RW_SEEK_SET);
        count = SDL_ReadBE16(reader) + 1;
        if ((size_t)count > (family.size - 54) / 6)
            goto malformed;
        id = -1;
        for (index = 0; index < count; index++) {
            int pointSize = SDL_ReadBE16(reader);
            int style = SDL_ReadBE16(reader);
            int fontId = (int16_t)SDL_ReadBE16(reader);

            if (pointSize == size && style == 0)
                id = fontId;
        }
        SDL_RWclose(reader);
        reader = NULL;
        if (id == -1)
            goto done;
    }
    if (SwcReadResource(resourcePath, "NFNT", id, &bitmap) != 0 &&
        SwcReadResource(resourcePath, "FONT", id, &bitmap) != 0)
        goto done;
    if (bitmap.size < 26)
        goto malformed;
    reader = SDL_RWFromConstMem(bitmap.data, bitmap.size);
    if (reader == NULL)
        goto done;
    if (SDL_ReadBE16(reader) & 0xc)
        goto malformed; /* Only the classic 1-bit Helvetica strikes. */
    first = SDL_ReadBE16(reader);
    last = SDL_ReadBE16(reader);
    if (first > last || last > 255)
        goto malformed;
    SDL_RWseek(reader, 8, RW_SEEK_SET);
    kern = (int16_t)SDL_ReadBE16(reader);
    highOffset = (int16_t)SDL_ReadBE16(reader);
    SDL_RWseek(reader, 14, RW_SEEK_SET);
    height = SDL_ReadBE16(reader);
    widths = SDL_ReadBE16(reader);
    if (highOffset > 0)
        widths += (size_t)highOffset << 16;
    widths = 16 + widths * 2; /* owTLoc is in words from its own field. */
    ascent = SDL_ReadBE16(reader);
    SDL_RWseek(reader, 24, RW_SEEK_SET);
    stride = (size_t)SDL_ReadBE16(reader) * 2;
    if (height < 1 || height > 128 || ascent > height || stride == 0 || stride > 8192)
        goto malformed;
    locations = 26 + stride * height;
    if (locations > bitmap.size ||
        (size_t)(last - first + 3) * 2 > bitmap.size - locations ||
        widths < locations + (size_t)(last - first + 3) * 2 || widths > bitmap.size ||
        (size_t)(last - first + 3) * 2 > bitmap.size - widths)
        goto malformed;
    /* One atlas row is the original font bitmap. Glyph source rectangles
     * share it directly; their individual bearings live beside advances. */
    surface = SDL_CreateRGBSurfaceWithFormat(0, (int)stride * 8, height, 32,
        SDL_PIXELFORMAT_ARGB8888);
    if (surface == NULL)
        goto done;
    for (y = 0; y < height; y++) {
        row = (uint32_t *)((uint8_t *)surface->pixels + y * surface->pitch);
        for (x = 0; x < surface->w; x++) {
            offset = 26 + (size_t)y * stride + (size_t)x / 8;
            row[x] = bitmap.data[offset] & (0x80 >> (x & 7)) ? UINT32_C(0xffffffff) : 0;
        }
    }
    for (character = 0; character < 95; character++) {
        glyph = character + 32;
        glyph = glyph >= first && glyph <= last ? glyph - first : last - first + 1;
        SDL_RWseek(reader, (widths + (size_t)glyph * 2), RW_SEEK_SET);
        metrics = SDL_ReadBE16(reader);
        if (metrics == 0xffff) {
            glyph = last - first + 1;
            SDL_RWseek(reader, (widths + (size_t)glyph * 2), RW_SEEK_SET);
            metrics = SDL_ReadBE16(reader);
        }
        SDL_RWseek(reader, (locations + (size_t)glyph * 2), RW_SEEK_SET);
        left = SDL_ReadBE16(reader);
        right = SDL_ReadBE16(reader);
        if (right < left || right > surface->w || metrics == 0xffff)
            goto malformed;
        font->glyphs[character] = (SDL_Rect){left, 0, right - left, height};
        font->advances[character] = metrics & 255;
        font->bearings[character] = -kern - (metrics >> 8);
    }
    font->baseline = ascent;
    font->texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (font->texture == NULL)
        goto done;
    result = 0;
    goto done;
malformed:
    fprintf(stderr, "SWC classic Helvetica %d: unsupported or malformed font in %s\n", size, resourcePath);
done:
    if (reader != NULL)
        SDL_RWclose(reader);
    SDL_FreeSurface(surface);
    SDL_free(family.data);
    SDL_free(bitmap.data);
    if (result != 0)
        SDL_ClearError();
    return result;
}

/* CODE_06 AllocTextLayer +0x1d9a selects Helvetica (QuickDraw ID 21).
 * CODE_14 init_text +0x1fec selects size 9 for cockpit readouts;
 * CODE_03 print_subtitle +0x1122 selects size 14. The demo contains no
 * FONT/NFNT resources. Prefer a local classic font resource; absent that,
 * macOS uses system Helvetica and other hosts use the bundled bitmap. */
int SdlCreateSwcTextFont(SDL_Renderer *renderer, SwcTextFont *font, int size)
{
    int character;

    SDL_zero(*font);
    if (SwcLoadClassicFont(renderer, font, size) == 0)
        goto ready;
#ifdef __APPLE__
    SDL_Surface *surface;
    CTFontRef face;
    CGColorSpaceRef colorSpace;
    CGContextRef context;
    UniChar code;
    CGGlyph glyph;
    CGSize advance;
    CGPoint origin;

    memset(font, 0, sizeof(*font));
    surface = SDL_CreateRGBSurfaceWithFormat(0, 16 * 24, 6 * 24, 32,
        SDL_PIXELFORMAT_ARGB8888);
    if (surface == NULL)
        return -1;
    SDL_FillRect(surface, NULL, 0);
    face = CTFontCreateWithName(CFSTR("Helvetica"), size, NULL);
    colorSpace = CGColorSpaceCreateDeviceRGB();
    context = colorSpace == NULL ? NULL : CGBitmapContextCreate(surface->pixels,
        surface->w, surface->h, 8, surface->pitch, colorSpace,
        kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Host);
    if (colorSpace != NULL)
        CGColorSpaceRelease(colorSpace);
    if (face == NULL || context == NULL) {
        if (face != NULL)
            CFRelease(face);
        if (context != NULL)
            CGContextRelease(context);
        SDL_FreeSurface(surface);
        return SDL_SetError("Cannot create SWC Helvetica text atlas");
    }
    CGContextSetShouldAntialias(context, false);
    CGContextSetShouldSmoothFonts(context, false);
    CGContextSetRGBFillColor(context, 1, 1, 1, 1);
    font->baseline = size + 4;
    for (character = 0; character < 95; character++) {
        font->bearings[character] = 2;
        code = (character + 32);
        CTFontGetGlyphsForCharacters(face, &code, &glyph, 1);
        CTFontGetAdvancesForGlyphs(face, kCTFontOrientationHorizontal, &glyph, &advance, 1);
        font->glyphs[character] = (SDL_Rect){character % 16 * 24,
            character / 16 * 24, 24, 24};
        font->advances[character] = (advance.width + 0.5);
        origin = CGPointMake(font->glyphs[character].x + 2,
            surface->h - font->glyphs[character].y - font->baseline);
        CTFontDrawGlyphs(face, &glyph, &origin, 1, context);
    }
    CGContextRelease(context);
    CFRelease(face);
    font->texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
#else
    (void)size;
    memset(font, 0, sizeof(*font));
    font->texture = SdlCreateSwcFont(renderer);
    font->baseline = SWC_HUD_FONT_BASELINE;
    for (character = 0; character < 95; character++) {
        font->bearings[character] = SWC_HUD_FONT_BEARING;
        font->glyphs[character] = (SDL_Rect){character % 16 * SWC_HUD_FONT_WIDTH,
            character / 16 * SWC_HUD_FONT_HEIGHT, SWC_HUD_FONT_WIDTH, SWC_HUD_FONT_HEIGHT};
        font->advances[character] = SWC_HUD_FONT_ADVANCE;
    }
#endif
ready:
    if (font->texture == NULL)
        return -1;
    if (SDL_SetTextureBlendMode(font->texture, SDL_BLENDMODE_BLEND) != 0) {
        SDL_DestroyTexture(font->texture);
        font->texture = NULL;
        return -1;
    }
    return 0;
}

/* CODE_03 GetLineLength and CODE_06 tprintf: integer glyph advances, with
 * the supplied origin on the baseline. Color changes keep the same pen. */
int SdlSwcTextWidth(const SwcTextFont *font, const char *text)
{
    int width = 0;
    unsigned char character;

    while (*text != 0) {
        character = *text++;
        character = character < 32 || character > 126 ? '?' - 32 : character - 32;
        width += font->advances[character];
    }
    return width;
}

int SdlDrawSwcProportionalText(SDL_Renderer *renderer, const SwcTextFont *font,
                                int x, int baseline, SDL_Color ink, const char *text)
{
    SDL_Rect destination;
    unsigned char character;

    if (SDL_SetTextureColorMod(font->texture, ink.r, ink.g, ink.b) != 0)
        return -1;
    while (*text != 0 && x < SWC_FRAME_WIDTH) {
        character = *text++;
        character = character < 32 || character > 126 ? '?' - 32 : character - 32;
        destination = font->glyphs[character];
        destination.x = x - font->bearings[character];
        destination.y = baseline - font->baseline;
        if (destination.w > 0 &&
            SDL_RenderCopy(renderer, font->texture, &font->glyphs[character], &destination) != 0)
            return -1;
        x += font->advances[character];
    }
    return 0;
}

/* CODE_03 print_subtitle/GetLineLength and CODE_06 CenterPrint: remove
 * script newlines, wrap at layer width minus 25, center each line, and
 * advance by 15 pixels. init_text gives this layer a 300x200 extent and
 * print_subtitle moves it to (10,120). The Mac draws white text directly
 * over the picture; the record's textColour also identifies the speaker. */
int SdlDrawSwcRoomSubtitle(SDL_Renderer *renderer, const SwcTextFont *font,
                            const char *text)
{
    char clean[256];
    size_t starts[256];
    size_t ends[256];
    int widths[256];
    size_t length = 0;
    size_t start = 0;
    size_t end;
    size_t space;
    size_t cursor;
    int lines = 0;
    int line;
    int width;
    int wordWidth;
    int character;
    int x;
    int baseline;
    SDL_Rect destination;

    while (*text != 0 && length < sizeof(clean) - 1) {
        if (*text != '\n' && *text != '\r')
            clean[length++] = *text;
        text++;
    }
    clean[length] = 0;
    while (start < length) {
        while (start < length && clean[start] == ' ')
            start++;
        if (start == length)
            break;
        width = wordWidth = 0;
        space = start;
        for (end = start; end < length; end++) {
            character = (unsigned char)clean[end];
            character = character < 32 || character > 126 ? '?' - 32 : character - 32;
            if (clean[end] == ' ') {
                space = end;
                wordWidth = width;
            }
            if (width + font->advances[character] > 275 && end > start)
                break;
            width += font->advances[character];
        }
        if (end < length && space > start) {
            end = space;
            width = wordWidth;
        }
        starts[lines] = start;
        ends[lines] = end;
        widths[lines++] = width;
        start = end;
    }
    baseline = 220 - (14 * lines + 1) / 2;
    /* Keep longer expanded pilot-name lines inside the 240-pixel host
     * picture while retaining the original position for shorter captions. */
    baseline = SDL_min(baseline, 234 - (lines - 1) * 15);
    if (SDL_SetTextureColorMod(font->texture, 255, 255, 255) != 0)
        return -1;
    for (line = 0; line < lines; line++) {
        x = 160 - widths[line] / 2;
        for (cursor = starts[line]; cursor < ends[line]; cursor++) {
            character = (unsigned char)clean[cursor];
            character = character < 32 || character > 126 ? '?' - 32 : character - 32;
            destination = font->glyphs[character];
            destination.x = x - font->bearings[character];
            destination.y = baseline - font->baseline;
            if (destination.w > 0 &&
                SDL_RenderCopy(renderer, font->texture, &font->glyphs[character], &destination) != 0)
                return -1;
            x += font->advances[character];
        }
        baseline += 15;
    }
    return 0;
}
