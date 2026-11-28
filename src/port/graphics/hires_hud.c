/* Hi-res HUD text (issues #29 and #37): in flight, the enhanced renderer
 * draws cockpit text, nav labels and HUD messages on a 640x400 layer, in
 * Trov's double-resolution cockpit font or FONTS.FNT's conversation font.
 * Each letter the game draws is recorded and followed through copies; a
 * present moves the letters the frame still shows onto the layer. */
#include "wc1.h"

#include "video_internal.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Generated from third_party/double-resolution-cockpit-font (CC BY 4.0). */
extern const unsigned char abHiResCockpitFont[];
extern const unsigned int nHiResCockpitFontSize;

#define HIRES_HUD_GLYPH_CAPACITY 1024
/* Text drawn off screen until it is copied: HUD messages in the space
   buffer and the nav map's labels. */
#define HIRES_HUD_OFFSCREEN_GLYPH_CAPACITY 512
#define HIRES_HUD_CELL_WIDTH 16
#define HIRES_HUD_CELL_HEIGHT 8
#define HIRES_HUD_TEXT_FONT 0
/* FONTS.FNT's cockpit font, as the Kilrathi Saga and DOS releases ship it:
   its size and SDL_crc32. The Spanish and French catalogs add glyphs to it
   and the German patch redraws some, so only English text matches. */
#define HIRES_HUD_COCKPIT_FONT 2
#define HIRES_HUD_COCKPIT_FONT_SIZE 4342
#define HIRES_HUD_COCKPIT_FONT_CRC 0xb2f668c0u
/* How many presents a hidden glyph whose whole line is gone is kept. */
#define HIRES_HUD_HIDDEN_FRAME_LIMIT 15

typedef struct HiResGlyph {
    const unsigned char *buffer;
    unsigned short stride;
    short x;
    short y;
    unsigned char width;
    unsigned char height;
    unsigned char character;
    unsigned char colour;
    unsigned char background;
    unsigned char centered;
    /* 1 when the game drew the cell's first row elsewhere. */
    unsigned char skipped;
    /* Presents in a row that have not shown the letter. */
    unsigned char misses;
    /* The present when this letter first appeared at this place. */
    unsigned int stamp;
    unsigned char before[HIRES_HUD_CELL_WIDTH * HIRES_HUD_CELL_HEIGHT];
    unsigned char after[HIRES_HUD_CELL_WIDTH * HIRES_HUD_CELL_HEIGHT];
} HiResGlyph;

/* The layer gl_renderer.c draws: an index and a coverage byte per pixel. */
static unsigned char g_abHiResLayer[SDL_PORT_HIRES_WIDTH *
                                    SDL_PORT_HIRES_HEIGHT * 2];
static int g_bHiResLayerVisible;
static int g_bHiResLayerDirty;
/* The frame without the mouse cursor, which letters are checked against. */
static unsigned char g_abHiResCheckFrame[SDL_PORT_FRAME_WIDTH *
                                         SDL_PORT_FRAME_HEIGHT];
static int g_bHiResHudFonts;
static int g_nHiResHudFlightDepth;
static unsigned char *g_pHiResFont;
static size_t g_nHiResFontSize;
static int g_bHiResFontUnavailable;
/* 1 when the game's cockpit font is the original, -1 when it is not, 0
   until checked. */
static int g_nHiResCockpitFontOriginal;
static HiResGlyph g_aHiResGlyphs[HIRES_HUD_GLYPH_CAPACITY];
static unsigned int g_nHiResGlyphCount;
static HiResGlyph g_aHiResOffscreenGlyphs[HIRES_HUD_OFFSCREEN_GLYPH_CAPACITY];
static unsigned int g_nHiResOffscreenGlyphCount;
static HiResGlyph g_stPendingHiResGlyph;
static int g_bHiResGlyphPending;
static unsigned int g_nHiResPresentCount;
static unsigned char g_abHiResPointerMask[SDL_PORT_FRAME_WIDTH *
                                          SDL_PORT_FRAME_HEIGHT];
static unsigned char g_abHiResPointerColours[SDL_PORT_FRAME_WIDTH *
                                             SDL_PORT_FRAME_HEIGHT];
static const unsigned char *g_pHiResPointerBuffer;
static int g_nHiResPointerOffsetX;
static int g_nHiResPointerOffsetY;
static int g_bHiResPointerDrawn;
static int g_bHiResPointerShown;

/* Keep a private copy, loaded through the same patched and translated packet
   path as the game's fonts, so its lifetime does not depend on scene fonts.
   PacketLoad only fills buffers from the game's own allocator. */
static int LoadHiResFont(void)
{
    const char *filename;
    unsigned char *packet;
    void *savedAllocation;
    unsigned int size;
    short savedError;

    if (g_pHiResFont != 0)
        return 1;
    if (g_bHiResFontUnavailable || pDiskFileRecords == 0)
        return 0;
    g_bHiResFontUnavailable = 1;
    filename = pDiskFileRecords[0].name;
    savedError = nPacketError;
    savedAllocation = pLastPacketAllocation;
    size = GetPacketSize(filename, HIRES_HUD_TEXT_FONT);
    if (size >= 772 && size <= 65535) {
        packet = PacketLoad(filename, HIRES_HUD_TEXT_FONT, 0, 0, 0);
        if (packet != 0) {
            g_pHiResFont = malloc(size);
            if (g_pHiResFont != 0) {
                memcpy(g_pHiResFont, packet, size);
                g_nHiResFontSize = size;
                g_bHiResFontUnavailable = 0;
            }
            ReleasePacketHandle(packet);
        }
    }
    pLastPacketAllocation = savedAllocation;
    nPacketError = savedError;
    return g_pHiResFont != 0;
}

/* Whether the game's cockpit font, loaded as the game loads it with the
   selected catalog, is the original that the double-resolution font
   redraws: with any other, its characters may not be the redrawn ones. */
static int HiResCockpitFontIsOriginal(void)
{
    const char *filename;
    unsigned char *packet;
    void *savedAllocation;
    unsigned int size;
    short savedError;

    if (g_nHiResCockpitFontOriginal != 0)
        return g_nHiResCockpitFontOriginal > 0;
    if (pDiskFileRecords == 0)
        return 0;
    g_nHiResCockpitFontOriginal = -1;
    filename = pDiskFileRecords[0].name;
    savedError = nPacketError;
    savedAllocation = pLastPacketAllocation;
    size = GetPacketSize(filename, HIRES_HUD_COCKPIT_FONT);
    if (size == HIRES_HUD_COCKPIT_FONT_SIZE) {
        packet = PacketLoad(filename, HIRES_HUD_COCKPIT_FONT, 0, 0, 0);
        if (packet != 0) {
            if (SDL_crc32(0, packet, size) == HIRES_HUD_COCKPIT_FONT_CRC)
                g_nHiResCockpitFontOriginal = 1;
            ReleasePacketHandle(packet);
        }
    }
    pLastPacketAllocation = savedAllocation;
    nPacketError = savedError;
    return g_nHiResCockpitFontOriginal > 0;
}

/* The hi-res font for text the game draws in cells this many rows tall, or
   NULL: the double-resolution cockpit font for the cockpit font's 6-row
   cells, when it applies, and otherwise the conversation font. */
static const unsigned char *HiResFontFor(int cellHeight, size_t *size)
{
    if (cellHeight == 6 && HiResCockpitFontIsOriginal()) {
        *size = nHiResCockpitFontSize;
        return abHiResCockpitFont;
    }
    *size = 0;
    if (!LoadHiResFont())
        return 0;
    *size = g_nHiResFontSize;
    return g_pHiResFont;
}

/* A catalog change must discard both the cached font and recorded glyphs.
   Keep the player's option, so the next draw loads the newly selected font. */
void SdlResetHiResHudFont(void)
{
    free(g_pHiResFont);
    g_pHiResFont = 0;
    g_nHiResFontSize = 0;
    g_bHiResFontUnavailable = 0;
    g_nHiResCockpitFontOriginal = 0;
    g_nHiResGlyphCount = 0;
    g_nHiResOffscreenGlyphCount = 0;
    g_bHiResGlyphPending = 0;
    g_bHiResLayerVisible = 0;
}

/* A hi-res font's bitmap of a character, or NULL when it has none. */
static const unsigned char *FindHiResGlyph(const unsigned char *data,
                                           size_t size,
                                           unsigned char character,
                                           int *width, int *height)
{
    size_t offset;
    size_t pixel;

    if (data == 0)
        return 0;
    *width = data[4 + character];
    *height = data[0] | (data[1] << 8);
    offset = (size_t)data[0x104 + character] |
             ((size_t)data[0x204 + character] << 8);
    if (*width == 0 || *height <= 0 || offset < 772 ||
        offset + (size_t)*width * (size_t)*height > size)
        return 0;
    /* Unused code-page slots can have a width and a valid offset but no
       ink. Keep the game's glyph instead of covering it with a blank one.
       Spaces intentionally have no ink. */
    if (character == ' ' || character == 0xa0)
        return data + offset;
    for (pixel = 0; pixel < (size_t)*width * (size_t)*height; pixel++) {
        if (data[offset + pixel] != 0xff && data[offset + pixel] != data[3])
            return data + offset;
    }
    return 0;
}

void SdlSetHiResHudFonts(int enabled)
{
    g_bHiResHudFonts = enabled != 0;
    if (!g_bHiResHudFonts) {
        g_nHiResGlyphCount = 0;
        g_nHiResOffscreenGlyphCount = 0;
        g_bHiResGlyphPending = 0;
    }
}

/* Marks the flight session, which GameFlow widens to the launch and the
   simulator's screens. Sessions nest. */
void SdlBeginHiResHudFlight(void)
{
    g_nHiResHudFlightDepth++;
}

void SdlEndHiResHudFlight(void)
{
    if (g_nHiResHudFlightDepth > 0)
        g_nHiResHudFlightDepth--;
    if (g_nHiResHudFlightDepth == 0) {
        g_nHiResGlyphCount = 0;
        g_nHiResOffscreenGlyphCount = 0;
        g_bHiResGlyphPending = 0;
        g_bHiResPointerDrawn = 0;
        g_bHiResPointerShown = 0;
    }
}

static int SdlHiResHudFontsActive(void)
{
    /* The FM Towns release draws its own Japanese text. */
    return g_bHiResHudFonts && g_nHiResHudFlightDepth > 0 &&
        SdlGlOverlayReady() && !SdlUsingTownsData();
}

/* Called by DrawFontGlyph before drawing a HUD-font character: records the
   cell. Off-screen text keeps its buffer's coordinates until copied. */
int SdlBeginHiResGlyph(const Viewport *view, int cursorX, int y, int width,
                       int height, unsigned char character,
                       unsigned char colour, unsigned char background,
                       int centered)
{
    HiResGlyph *glyph;
    const HiResGlyph *previous;
    const HiResGlyph *recorded;
    const unsigned char *screen;
    const unsigned char *font;
    size_t fontSize;
    unsigned int count;
    unsigned int index;
    unsigned int kept;
    long offset;
    long stride;
    int hiResWidth;
    int hiResHeight;
    int skipped;
    int pixel;
    int row;

    g_bHiResGlyphPending = 0;
    screen = stScreenView.pixels;
    if (!SdlHiResHudFontsActive() || view == 0 || view->pixels == 0 ||
        view->rowOffsets == 0 || screen == 0 || width <= 0 ||
        height < 2 || width > HIRES_HUD_CELL_WIDTH ||
        height > HIRES_HUD_CELL_HEIGHT || y < 0 || y < view->top ||
        y + height - 1 > view->bottom)
        return 0;
    if (height != 6) {
        /* Only HUD messages and the simulator's score lines; other 8-pixel
           text keeps the game's font. */
        if (height != 8 || background != 0xff ||
            view->pixels != stSpaceBufferViewport.pixels ||
            (view != &stSpaceBufferViewport &&
             (!centered || view->bottom - view->top != 60 ||
              view->left + view->right != 319)))
            return 0;
        /* SetHudTextColour erases a message by drawing it again in the
           space clear colour. Those glyphs are not text. */
        if (colour == cPrimaryViewBufferColour) {
            kept = 0;
            for (index = 0; index < g_nHiResOffscreenGlyphCount; index++) {
                if (g_aHiResOffscreenGlyphs[index].height == 8)
                    continue;
                if (kept != index)
                    g_aHiResOffscreenGlyphs[kept] =
                        g_aHiResOffscreenGlyphs[index];
                kept++;
            }
            g_nHiResOffscreenGlyphCount = kept;
            return 0;
        }
    }
    font = HiResFontFor(height, &fontSize);
    if (FindHiResGlyph(font, fontSize, character, &hiResWidth,
                       &hiResHeight) == 0)
        return 0;
    /* Past buffer offset 0x8000 DrawFontGlyph wraps the first row, which is
       blank in this font; keep the cell with the rest of the letter. */
    skipped = view->top == y && (view->rowOffsets[y] & 0x8000) != 0;
    offset = (long)view->rowOffsets[y] + cursorX;
    stride = (long)view->rowOffsets[y + 1] - view->rowOffsets[y];
    if (view->pixels == screen && stride != SDL_PORT_FRAME_WIDTH)
        return 0;
    if (offset < 0 || stride < width || stride > 0xffff ||
        offset % stride + width > stride ||
        (view->pixels == screen &&
         offset / stride + height > SDL_PORT_FRAME_HEIGHT))
        return 0;
    for (row = 1; row < height; row++) {
        if ((long)view->rowOffsets[y + row] + cursorX !=
            offset + (long)row * stride)
            return 0;
    }
    glyph = &g_stPendingHiResGlyph;
    glyph->buffer = view->pixels;
    glyph->stride = (unsigned short)stride;
    glyph->x = (short)(offset % stride);
    glyph->y = (short)(offset / stride);
    glyph->width = (unsigned char)width;
    glyph->height = (unsigned char)height;
    glyph->character = character;
    glyph->colour = colour;
    glyph->background = background;
    glyph->centered = centered != 0;
    glyph->skipped = (unsigned char)skipped;
    glyph->misses = 0;
    for (row = 0; row < height; row++)
        memcpy(glyph->before + row * width,
               view->pixels + offset + (long)row * stride, (size_t)width);
    /* Transparent text redrawn over itself would record its old letter as
       background; reuse the clean background under surviving ink. */
    if (background == 0xff) {
        recorded = view->pixels == screen ? g_aHiResGlyphs : g_aHiResOffscreenGlyphs;
        count = view->pixels == screen ? g_nHiResGlyphCount : g_nHiResOffscreenGlyphCount;
        for (index = 0; index < count; index++) {
            previous = &recorded[index];
            if (previous->buffer != glyph->buffer || previous->stride != glyph->stride ||
                previous->x != glyph->x || previous->y != glyph->y ||
                previous->width != width || previous->height != height ||
                previous->character != character || previous->background != 0xff)
                continue;
            for (pixel = 0; pixel < width * height; pixel++) {
                if (previous->after[pixel] != previous->before[pixel] &&
                    glyph->before[pixel] == previous->after[pixel])
                    glyph->before[pixel] = previous->before[pixel];
            }
            break;
        }
    }
    g_bHiResGlyphPending = 1;
    return 1;
}

/* Text cells do not overlap, so a glyph drawn over part of a recorded one's
   cell replaces it. A full list starts over. */
static void StoreHiResGlyph(HiResGlyph *glyphs, unsigned int *count,
                            unsigned int capacity, const HiResGlyph *glyph)
{
    unsigned int index;
    unsigned int kept;
    unsigned int slot;
    unsigned int stamp;
    int removed;

    slot = *count;
    stamp = g_nHiResPresentCount;
    removed = 0;
    for (index = 0; index < *count; index++) {
        if (glyphs[index].buffer != glyph->buffer ||
            glyphs[index].x >= glyph->x + glyph->width ||
            glyph->x >= glyphs[index].x + glyphs[index].width ||
            glyphs[index].y >= glyph->y + glyph->height ||
            glyph->y >= glyphs[index].y + glyphs[index].height)
            continue;
        if (slot == *count) {
            slot = index;
            /* The same letter drawn again keeps its age. */
            if (glyphs[index].x == glyph->x && glyphs[index].y == glyph->y &&
                glyphs[index].width == glyph->width &&
                glyphs[index].height == glyph->height &&
                glyphs[index].character == glyph->character)
                stamp = glyphs[index].stamp;
        } else {
            glyphs[index].width = 0;
            removed = 1;
        }
    }
    if (slot == *count) {
        if (*count == capacity)
            *count = slot = 0;
        (*count)++;
    }
    glyphs[slot] = *glyph;
    glyphs[slot].stamp = stamp;
    if (!removed)
        return;
    kept = 0;
    for (index = 0; index < *count; index++) {
        if (glyphs[index].width == 0)
            continue;
        if (kept != index)
            glyphs[kept] = glyphs[index];
        kept++;
    }
    *count = kept;
}

/* Whether the game's glyph drew its letter at this pixel of its cell. */
static int HiResGlyphInk(const HiResGlyph *glyph, int pixel)
{
    if (pixel < glyph->skipped * glyph->width)
        return 0;
    return glyph->after[pixel] !=
        (glyph->background != 0xff ? glyph->background : glyph->before[pixel]);
}

static int HiResGlyphHasInk(const HiResGlyph *glyph)
{
    int pixel;

    for (pixel = 0; pixel < glyph->width * glyph->height; pixel++) {
        if (HiResGlyphInk(glyph, pixel))
            return 1;
    }
    return 0;
}

/* Notes the flight pointer's pixels in the space buffer, so the letters
   under it keep matching and it is drawn back above the text (#37). */

void SdlNoteHiResFlightPointer(void)
{
    g_bHiResPointerDrawn = 1;
}

/* Whether the flight pointer covers pixel (x, y) of the space buffer, or of
   the screen. */
static int HiResPointerCovers(int x, int y, int screen)
{
    if (!g_bHiResPointerShown)
        return 0;
    if (screen) {
        x -= g_nHiResPointerOffsetX;
        y -= g_nHiResPointerOffsetY;
    }
    if (x < 0 || y < 0 || x >= SDL_PORT_FRAME_WIDTH ||
        y >= SDL_PORT_FRAME_HEIGHT)
        return 0;
    return g_abHiResPointerMask[y * SDL_PORT_FRAME_WIDTH + x] != 0;
}

/* Whether a cell still shows the game's letter: its ink, a few pixels of
   which may have turned back to background (#37), less those under the
   flight pointer (1: in the space buffer, 2: on the screen). */
static int HiResGlyphMatches(const HiResGlyph *glyph,
                             const unsigned char *pixels, long stride,
                             int x, int y, int pointer)
{
    const unsigned char *cell;
    int column;
    int erased;
    int ink;
    int pixel;
    int row;

    erased = 0;
    ink = 0;
    for (row = glyph->skipped; row < glyph->height; row++) {
        cell = pixels + (long)(y + row) * stride + x;
        for (column = 0; column < glyph->width; column++) {
            pixel = row * glyph->width + column;
            if (!HiResGlyphInk(glyph, pixel) ||
                (pointer != 0 &&
                 HiResPointerCovers(x + column, y + row, pointer == 2)))
                continue;
            ink++;
            if (cell[column] == glyph->after[pixel])
                continue;
            if (cell[column] != (glyph->background != 0xff
                                     ? glyph->background
                                     : glyph->before[pixel]))
                return 0;
            erased++;
        }
    }
    return erased * 3 <= ink;
}

/* Buffers, then rows top to bottom and left to right, so a line's glyphs
   are adjacent. */
static int CompareHiResGlyphs(const void *left, const void *right)
{
    const HiResGlyph *first = (const HiResGlyph *)left;
    const HiResGlyph *second = (const HiResGlyph *)right;

    if (first->buffer != second->buffer)
        return (uintptr_t)first->buffer < (uintptr_t)second->buffer ? -1 : 1;
    if (first->y != second->y)
        return first->y - second->y;
    return first->x - second->x;
}

/* Whether the second glyph continues the first's run of text. */
static int HiResGlyphsAdjacent(const HiResGlyph *first,
                               const HiResGlyph *second)
{
    return second->buffer == first->buffer && second->y == first->y &&
        second->height == first->height &&
        second->x == first->x + first->width;
}

/* Sorts the glyphs into runs of adjacent cells and drops the runs without a
   letter: a space only stays with the text it separates. */
static void DropHiResSpaceRuns(HiResGlyph *glyphs, unsigned int *count)
{
    unsigned int first;
    unsigned int last;
    unsigned int index;
    unsigned int kept;
    int ink;

    qsort(glyphs, *count, sizeof(glyphs[0]), CompareHiResGlyphs);
    kept = 0;
    for (first = 0; first < *count; first = last) {
        ink = HiResGlyphHasInk(&glyphs[first]);
        for (last = first + 1; last < *count &&
             HiResGlyphsAdjacent(&glyphs[last - 1], &glyphs[last]); last++)
            ink |= HiResGlyphHasInk(&glyphs[last]);
        if (!ink)
            continue;
        for (index = first; index < last; index++) {
            if (kept != index)
                glyphs[kept] = glyphs[index];
            kept++;
        }
    }
    *count = kept;
}

/* After the game drew the glyph: keep the cell's new pixels and replace any
   earlier glyph recorded at the same place. */
void SdlEndHiResGlyph(void)
{
    HiResGlyph *glyph;
    const unsigned char *cell;
    int row;

    if (!g_bHiResGlyphPending)
        return;
    g_bHiResGlyphPending = 0;
    glyph = &g_stPendingHiResGlyph;
    cell = glyph->buffer + (long)glyph->y * glyph->stride + glyph->x;
    for (row = 0; row < glyph->height; row++)
        memcpy(glyph->after + row * glyph->width,
               cell + (long)row * glyph->stride, glyph->width);
    if (glyph->buffer == stScreenView.pixels)
        StoreHiResGlyph(g_aHiResGlyphs, &g_nHiResGlyphCount,
                        HIRES_HUD_GLYPH_CAPACITY, glyph);
    else
        StoreHiResGlyph(g_aHiResOffscreenGlyphs, &g_nHiResOffscreenGlyphCount,
                        HIRES_HUD_OFFSCREEN_GLYPH_CAPACITY, glyph);
}

/* After a buffer is copied to the screen, its glyphs in the copied area
   move with it; those it no longer shows follow it hidden (#37). */
static void CopyHiResGlyphsToScreen(const Viewport *source, int left, int top,
                                    int right, int bottom, int offsetX,
                                    int offsetY)
{
    const unsigned char *screen;
    HiResGlyph copied;
    const HiResGlyph *glyph;
    long stride;
    unsigned int index;
    unsigned int kept;
    int inside;
    int x;
    int y;

    screen = stScreenView.pixels;
    if (!SdlHiResHudFontsActive() || source == 0 || screen == 0 ||
        source->pixels == 0 || source->pixels == screen ||
        source->rowOffsets == 0 || source->top < 0 ||
        source->top >= source->bottom || g_nHiResOffscreenGlyphCount == 0)
        return;
    stride = (long)source->rowOffsets[source->top + 1] -
             source->rowOffsets[source->top];
    kept = 0;
    for (index = 0; index < g_nHiResOffscreenGlyphCount; index++) {
        glyph = &g_aHiResOffscreenGlyphs[index];
        if (glyph->buffer == source->pixels && glyph->stride == stride &&
            glyph->x >= left && glyph->y >= top &&
            glyph->x + glyph->width - 1 <= right &&
            glyph->y + glyph->height - 1 <= bottom) {
            if (HiResGlyphMatches(glyph, source->pixels, stride, glyph->x,
                                  glyph->y,
                                  g_bHiResPointerShown &&
                                  source->pixels == g_pHiResPointerBuffer))
                g_aHiResOffscreenGlyphs[index].misses = 0;
            else if (++g_aHiResOffscreenGlyphs[index].misses >
                     HIRES_HUD_HIDDEN_FRAME_LIMIT)
                continue;
        }
        if (kept != index)
            g_aHiResOffscreenGlyphs[kept] = *glyph;
        kept++;
    }
    g_nHiResOffscreenGlyphCount = kept;
    DropHiResSpaceRuns(g_aHiResOffscreenGlyphs, &g_nHiResOffscreenGlyphCount);
    for (index = 0; index < g_nHiResOffscreenGlyphCount; index++) {
        glyph = &g_aHiResOffscreenGlyphs[index];
        inside = glyph->buffer == source->pixels && glyph->stride == stride &&
            glyph->x >= left && glyph->y >= top &&
            glyph->x + glyph->width - 1 <= right &&
            glyph->y + glyph->height - 1 <= bottom;
        if (!inside)
            continue;
        x = glyph->x + offsetX;
        y = glyph->y + offsetY;
        if (x < 0 || y < 0 || x + glyph->width > SDL_PORT_FRAME_WIDTH ||
            y + glyph->height > SDL_PORT_FRAME_HEIGHT)
            continue;
        copied = *glyph;
        copied.buffer = screen;
        copied.stride = SDL_PORT_FRAME_WIDTH;
        copied.x = (short)x;
        copied.y = (short)y;
        StoreHiResGlyph(g_aHiResGlyphs, &g_nHiResGlyphCount,
                        HIRES_HUD_GLYPH_CAPACITY, &copied);
    }
}

/* CopyViewportContents puts the source's corner on the destination's. */
void SdlCopyHiResGlyphs(const Viewport *source, const Viewport *destination)
{
    if (source == 0 || destination == 0 ||
        destination->pixels != stScreenView.pixels)
        return;
    CopyHiResGlyphsToScreen(source, source->left, source->top, source->right,
                            source->bottom, destination->left - source->left,
                            destination->top - source->top);
}

/* Where the flight pointer the HUD drew this frame covers the space buffer
   being copied: the pointer's own pixels, as RestoreSpriteBackground puts a
   save area back. Only for a buffer laid out as the screen is. */
static void NoteHiResFlightPointer(const Viewport *source, int originX,
                                   int originY)
{
    unsigned char coverage[sizeof(abCursorSaveArea)];
    Viewport viewport;

    g_bHiResPointerShown = 0;
    if (!g_bHiResPointerDrawn)
        return;
    g_bHiResPointerDrawn = 0;
    if (!SdlHiResHudFontsActive() || stMouseCursorState.viewport == 0 ||
        stMouseCursorState.shape == 0 ||
        stMouseCursorState.viewport->pixels != source->pixels ||
        source->rowOffsets == 0 || source->top < 0 ||
        source->top >= source->bottom ||
        source->bottom >= SDL_PORT_FRAME_HEIGHT ||
        source->rowOffsets[source->top] !=
            (long)source->top * SDL_PORT_FRAME_WIDTH ||
        source->rowOffsets[source->top + 1] !=
            (long)(source->top + 1) * SDL_PORT_FRAME_WIDTH ||
        source->rowOffsets[source->bottom] !=
            (long)source->bottom * SDL_PORT_FRAME_WIDTH)
        return;
    memset(g_abHiResPointerMask, 0, sizeof(g_abHiResPointerMask));
    memset(coverage, 0xff, sizeof(coverage));
    viewport = *source;
    viewport.pixels = g_abHiResPointerMask;
    viewport.allocation = g_abHiResPointerMask;
    RestoreSpriteBackground(&viewport, coverage, (short)nStoredMouseCursorPosX,
                            (short)nSavedMouseCursorY,
                            stMouseCursorState.shape,
                            stMouseCursorState.frame);
    memcpy(g_abHiResPointerColours, source->pixels,
           (size_t)(source->bottom + 1) * SDL_PORT_FRAME_WIDTH);
    g_pHiResPointerBuffer = source->pixels;
    g_nHiResPointerOffsetX = originX;
    g_nHiResPointerOffsetY = originY;
    g_bHiResPointerShown = 1;
}

/* BlitViewportScanlineRuns copies the space buffer's view window, shifted
   by the geometry's origin. Text it leaves under the cockpit no longer
   matches the presented frame and is dropped there. */
void SdlCopyHiResGlyphRuns(const Viewport *source, const Viewport *destination,
                           int originX, int originY)
{
    if (source == 0 || destination == 0 ||
        destination->pixels != stScreenView.pixels)
        return;
    NoteHiResFlightPointer(source, originX, originY);
    CopyHiResGlyphsToScreen(source, 0, 0, 0x7fff, 0x7fff, originX, originY);
}

static int HiResGlyphStillShown(const HiResGlyph *glyph,
                                const unsigned char *frame)
{
    return HiResGlyphMatches(glyph, frame, SDL_PORT_FRAME_WIDTH, glyph->x,
                             glyph->y, 2);
}


/* The hi-res font keeps its letter spacing in each glyph's leading columns. */
static int HiResLeadingBlank(const unsigned char *bitmap, int width,
                             int height, unsigned char background)
{
    int column;
    int row;
    unsigned char value;

    for (column = 0; column < width; column++) {
        for (row = 0; row < height; row++) {
            value = bitmap[row * width + column];
            if (value != 0xff && value != background)
                return column;
        }
    }
    return width;
}

static void SetHiResPixel(int x, int y, unsigned char index)
{
    unsigned char *pixel;

    if (x < 0 || y < 0 || x >= SDL_PORT_HIRES_WIDTH || y >= SDL_PORT_HIRES_HEIGHT)
        return;
    pixel = g_abHiResLayer + ((size_t)y * SDL_PORT_HIRES_WIDTH + x) * 2;
    pixel[0] = index;
    pixel[1] = 0xff;
}

/* Draws the mouse cursor above the text at 2x, masked by its restore spans. */
static void CompositeHiResMouseCursor(const unsigned char *pixels)
{
    unsigned char coverage[sizeof(abCursorSaveArea)];
    Viewport viewport;
    int x;
    int y;
    int column;
    int row;

    if (bEventMouseCursorDrawnFlag == 0 ||
        stMouseCursorState.viewport == 0 ||
        stMouseCursorState.shape == 0 ||
        stMouseCursorState.viewport->pixels != pixels)
        return;
    /* Glyph validation has finished with this scratch frame. */
    memset(g_abHiResCheckFrame, 0, sizeof(g_abHiResCheckFrame));
    memset(coverage, 0xff, sizeof(coverage));
    viewport = *stMouseCursorState.viewport;
    viewport.pixels = g_abHiResCheckFrame;
    viewport.allocation = g_abHiResCheckFrame;
    RestoreSpriteBackground(&viewport, coverage, nMousePointerDrawnX,
                            nMousePointerDrawnY, stMouseCursorState.shape,
                            stMouseCursorState.frame);
    for (y = 0; y < SDL_PORT_FRAME_HEIGHT; y++) {
        for (x = 0; x < SDL_PORT_FRAME_WIDTH; x++) {
            if (g_abHiResCheckFrame[y * SDL_PORT_FRAME_WIDTH + x] == 0)
                continue;
            for (row = 0; row < SDL_PORT_HIRES_SCALE; row++) {
                for (column = 0; column < SDL_PORT_HIRES_SCALE; column++)
                    SetHiResPixel(x * SDL_PORT_HIRES_SCALE + column,
                                  y * SDL_PORT_HIRES_SCALE + row,
                                  pixels[y * SDL_PORT_FRAME_WIDTH + x]);
            }
        }
    }
}

/* The flight pointer back above the text, where the screen still shows it:
   the window shows the space buffer, the cockpit around it does not. */
static void CompositeHiResFlightPointer(const unsigned char *pixels)
{
    int column;
    int row;
    int x;
    int y;
    int screenX;
    int screenY;

    if (!g_bHiResPointerShown)
        return;
    g_bHiResPointerShown = 0;
    for (y = 0; y < SDL_PORT_FRAME_HEIGHT; y++) {
        for (x = 0; x < SDL_PORT_FRAME_WIDTH; x++) {
            if (g_abHiResPointerMask[y * SDL_PORT_FRAME_WIDTH + x] == 0)
                continue;
            screenX = x + g_nHiResPointerOffsetX;
            screenY = y + g_nHiResPointerOffsetY;
            if (screenX < 0 || screenY < 0 ||
                screenX >= SDL_PORT_FRAME_WIDTH ||
                screenY >= SDL_PORT_FRAME_HEIGHT ||
                pixels[screenY * SDL_PORT_FRAME_WIDTH + screenX] !=
                    g_abHiResPointerColours[y * SDL_PORT_FRAME_WIDTH + x])
                continue;
            for (row = 0; row < SDL_PORT_HIRES_SCALE; row++) {
                for (column = 0; column < SDL_PORT_HIRES_SCALE; column++)
                    SetHiResPixel(screenX * SDL_PORT_HIRES_SCALE + column,
                                  screenY * SDL_PORT_HIRES_SCALE + row,
                                  pixels[screenY * SDL_PORT_FRAME_WIDTH +
                                         screenX]);
            }
        }
    }
}

/* A covered letter stays hidden in its line until newer text is drawn
   there or the line has been gone for a while. */
static void DropHiddenHiResGlyphs(void)
{
    const HiResGlyph *glyph;
    unsigned int first;
    unsigned int last;
    unsigned int index;
    unsigned int kept;
    unsigned int newest;
    int shown;

    kept = 0;
    for (first = 0; first < g_nHiResGlyphCount; first = last) {
        shown = 0;
        newest = 0;
        for (last = first; last < g_nHiResGlyphCount &&
             (last == first || HiResGlyphsAdjacent(&g_aHiResGlyphs[last - 1],
                                                   &g_aHiResGlyphs[last]));
             last++) {
            if (g_aHiResGlyphs[last].misses == 0) {
                shown = 1;
                if (g_aHiResGlyphs[last].stamp > newest)
                    newest = g_aHiResGlyphs[last].stamp;
            }
        }
        for (index = first; index < last; index++) {
            glyph = &g_aHiResGlyphs[index];
            if (glyph->misses != 0 &&
                (shown ? glyph->stamp < newest
                       : glyph->misses > HIRES_HUD_HIDDEN_FRAME_LIMIT))
                continue;
            if (kept != index)
                g_aHiResGlyphs[kept] = *glyph;
            kept++;
        }
    }
    g_nHiResGlyphCount = kept;
}

/* Drops or hides the glyphs the frame no longer shows, erases the shown
   ones from the frame and draws them on the layer, each run with the
   hi-res font's spacing. */
void SdlBuildHiResHudLayer(const unsigned char *pixels, unsigned char *frame)
{
    const HiResGlyph *glyph;
    const unsigned char *bitmap;
    const unsigned char *font;
    size_t fontSize;
    unsigned int index;
    unsigned int kept;
    unsigned char value;
    unsigned char fontInk;
    unsigned char fontBackground;
    int width;
    int height;
    int left;
    int top;
    int total;
    int overflow;
    int lead;
    int row;
    int column;
    unsigned int first;
    unsigned int last;

    g_bHiResLayerVisible = 0;
    if (!SdlHiResHudFontsActive()) {
        g_nHiResGlyphCount = 0;
        g_nHiResOffscreenGlyphCount = 0;
        g_bHiResPointerShown = 0;
        return;
    }
    g_nHiResPresentCount++;
    SdlCopyFrameWithoutMouse(g_abHiResCheckFrame, pixels);
    for (index = 0; index < g_nHiResGlyphCount; index++) {
        if (HiResGlyphStillShown(&g_aHiResGlyphs[index],
                                 g_abHiResCheckFrame))
            g_aHiResGlyphs[index].misses = 0;
        else if (g_aHiResGlyphs[index].misses < 0xff)
            g_aHiResGlyphs[index].misses++;
    }
    DropHiResSpaceRuns(g_aHiResGlyphs, &g_nHiResGlyphCount);
    DropHiddenHiResGlyphs();
    kept = g_nHiResGlyphCount;
    if (kept == 0) {
        g_bHiResPointerShown = 0;
        return;
    }
    memset(g_abHiResLayer, 0, sizeof(g_abHiResLayer));
    for (index = 0; index < kept; index++) {
        glyph = &g_aHiResGlyphs[index];
        if (glyph->misses != 0)
            continue;
        for (row = 0; row < glyph->height; row++) {
            for (column = 0; column < glyph->width; column++) {
                if (!HiResGlyphInk(glyph, row * glyph->width + column))
                    continue;
                frame[(glyph->y + row) * SDL_PORT_FRAME_WIDTH + glyph->x +
                      column] =
                    glyph->background != 0xff
                        ? glyph->background
                        : glyph->before[row * glyph->width + column];
            }
        }
    }
    for (first = 0; first < kept; first = last) {
        last = first + 1;
        while (last < kept && HiResGlyphsAdjacent(&g_aHiResGlyphs[last - 1],
                                                  &g_aHiResGlyphs[last]))
            last++;
        /* A run's cells share their height, and so their font. */
        font = HiResFontFor(g_aHiResGlyphs[first].height, &fontSize);
        if (font == 0)
            continue;
        fontInk = font[2];
        fontBackground = font[3];
        total = 0;
        for (index = first; index < last; index++) {
            if (FindHiResGlyph(font, fontSize, g_aHiResGlyphs[index].character,
                               &width, &height) != 0)
                total += width;
        }
        left = g_aHiResGlyphs[first].x * SDL_PORT_HIRES_SCALE;
        overflow = total -
            ((g_aHiResGlyphs[last - 1].x + g_aHiResGlyphs[last - 1].width) *
                 SDL_PORT_HIRES_SCALE - left);
        if (g_aHiResGlyphs[first].centered) {
            left -= overflow / 2;
            overflow = 0;
        } else if (overflow > 0) {
            bitmap = FindHiResGlyph(font, fontSize,
                                    g_aHiResGlyphs[first].character, &width,
                                    &height);
            lead = bitmap != 0
                ? HiResLeadingBlank(bitmap, width, height, fontBackground)
                : 0;
            if (lead > overflow)
                lead = overflow;
            left -= lead;
            overflow -= lead;
        }
        for (index = first; index < last; index++) {
            glyph = &g_aHiResGlyphs[index];
            bitmap = FindHiResGlyph(font, fontSize, glyph->character, &width,
                                    &height);
            if (bitmap == 0)
                continue;
            if (overflow > 0 && index > first &&
                HiResLeadingBlank(bitmap, width, height, fontBackground) >= 2) {
                left--;
                overflow--;
            }
            top = glyph->y * SDL_PORT_HIRES_SCALE;
            if (height < glyph->height * SDL_PORT_HIRES_SCALE)
                top += (glyph->height * SDL_PORT_HIRES_SCALE - height) / 2;
            for (row = 0; row < height && glyph->misses == 0; row++) {
                for (column = 0; column < width; column++) {
                    value = bitmap[row * width + column];
                    if (value == 0xff || value == fontBackground)
                        continue;
                    SetHiResPixel(left + column, top + row,
                                  value == fontInk ? glyph->colour : value);
                }
            }
            left += width;
        }
    }
    CompositeHiResMouseCursor(pixels);
    CompositeHiResFlightPointer(pixels);
    g_bHiResLayerVisible = 1;
    g_bHiResLayerDirty = 1;
}

/* The layer to draw over this frame, or NULL when it has none; *changed is 1
   when it changed since it was last taken. */
const unsigned char *SdlTakeHiResHudLayer(int *changed)
{
    if (!g_bHiResLayerVisible)
        return 0;
    *changed = g_bHiResLayerDirty;
    g_bHiResLayerDirty = 0;
    return g_abHiResLayer;
}
