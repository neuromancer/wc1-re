/* Hi-res HUD text (issues #29 and #37), with the enhanced renderer: in
 * flight, cockpit text, nav labels and HUD messages are shown in FONTS.FNT's
 * 11-pixel conversation font (section 0) at its native size, on a 640x400
 * layer that gl_renderer.c draws over the game's 320x200 frame.
 *
 * The game draws text once into a buffer, then copies it, draws over it or
 * erases it by drawing what was under it again. The overlay follows each
 * letter through that:
 *
 * - Recorded: DrawFontGlyph wraps each HUD-font character in
 *   SdlBeginHiResGlyph and SdlEndHiResGlyph, which keep the character, its
 *   colour and place, and its cell before and after the draw. Text drawn off
 *   screen, such as radio messages in the space buffer and the nav map's
 *   labels, keeps its buffer's coordinates until a copy to the screen moves
 *   it there (SdlCopyHiResGlyphs, SdlCopyHiResGlyphRuns).
 * - Shown: on each present, SdlBuildHiResHudLayer checks every letter
 *   against the frame (HiResGlyphMatches). A letter stays while its cell
 *   still shows the letter's own pixels, allowing up to a third of them to
 *   turn back into the background and the flight pointer to cover some. A
 *   shown letter is erased from the presented frame and drawn on the layer,
 *   each run of adjacent cells set with the hi-res font's own spacing.
 * - Hidden: a letter the frame stops showing, as when the weapon display's
 *   missile crosses it, keeps its place in its line (misses counts the
 *   presents) and shows again when the game restores the cell without
 *   drawing the letter again.
 * - Dropped: a hidden letter is forgotten once newer text is drawn in its
 *   line, or once its whole line has been gone for
 *   HIRES_HUD_HIDDEN_FRAME_LIMIT presents. Spaces go with their line, a cell
 *   drawn over replaces the letter there, and a full list starts over.
 *
 * The menus' mouse cursor and the flight pointer are drawn back above the
 * letters, at the base frame's 2x scale. */
#include "wc1.h"

#include "video_internal.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define HIRES_HUD_GLYPH_CAPACITY 1024
/* Text drawn off screen until it is copied: HUD messages in the space
   buffer and the nav map's labels. */
#define HIRES_HUD_OFFSCREEN_GLYPH_CAPACITY 512
#define HIRES_HUD_CELL_WIDTH 16
#define HIRES_HUD_CELL_HEIGHT 8
#define HIRES_HUD_TEXT_FONT 0
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

/* A catalog change must discard both the cached font and recorded glyphs.
   Keep the player's option, so the next draw loads the newly selected font. */
void SdlResetHiResHudFont(void)
{
    free(g_pHiResFont);
    g_pHiResFont = 0;
    g_nHiResFontSize = 0;
    g_bHiResFontUnavailable = 0;
    g_nHiResGlyphCount = 0;
    g_nHiResOffscreenGlyphCount = 0;
    g_bHiResGlyphPending = 0;
    g_bHiResLayerVisible = 0;
}

/* The hi-res font's bitmap of a character, or NULL when it has none. */
static const unsigned char *FindHiResGlyph(unsigned char character,
                                           int *width, int *height)
{
    const unsigned char *data = g_pHiResFont;
    size_t offset;
    size_t pixel;

    if (data == 0)
        return 0;
    *width = data[4 + character];
    *height = data[0] | (data[1] << 8);
    offset = (size_t)data[0x104 + character] |
             ((size_t)data[0x204 + character] << 8);
    if (*width == 0 || *height <= 0 || offset < 772 ||
        offset + (size_t)*width * (size_t)*height > g_nHiResFontSize)
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

/* RunSpaceFlight marks the flight session. GameFlow widens it to the launch
   before it, which draws the cockpit's displays, and the training simulator
   to its Get Ready, Victory and Game Over screens around it: screens outside
   keep the game's own fonts. Sessions nest. */
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

/* Called by DrawFontGlyph before it draws a HUD-font character: keeps what
   the cell showed. Only a whole cell laid out in plain rows of its buffer is
   taken. Text drawn off screen keeps its buffer's coordinates until it is
   copied. */
int SdlBeginHiResGlyph(const Viewport *view, int cursorX, int y, int width,
                       int height, unsigned char character,
                       unsigned char colour, unsigned char background,
                       int centered)
{
    HiResGlyph *glyph;
    const HiResGlyph *previous;
    const HiResGlyph *recorded;
    const unsigned char *screen;
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
        /* print_message_text copies the HUD message context, which draws
           into the space buffer, and clips it to the cockpit's 60-row
           message area. The simulator's score and bonus lines use the
           context itself. Other users of the 8-pixel font, such as nav
           descriptions and modal panels, keep the game's own text. */
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
    if (!LoadHiResFont() ||
        FindHiResGlyph(character, &hiResWidth, &hiResHeight) == 0)
        return 0;
    /* At a viewport's top row past buffer offset 0x8000, as the VDU
       headings are, DrawFontGlyph puts the first row at the top of the
       buffer. That row is blank in the cockpit font: the cell stays where
       the rest of the letter is. */
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
    /* Transparent text redrawn over pixels a partial redraw left in place
       would capture its own old letter as background. Reuse the clean
       background under surviving ink instead, per pixel, because such a
       redraw can cut through a glyph's cell. */
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

/* Keeps a glyph in a list. Text cells tile without overlapping, so a glyph
   drawn over part of a recorded one's cell replaces it. A full list starts
   over: text still shown is recorded again when next drawn, and until then
   it simply shows in the game's own font. */
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

/* The flight pointer, which overlay_head_up_display draws into the space
   buffer after the radio messages: the buffer's pixels it covers and their
   colours, as that buffer was last copied to the screen with this offset,
   before the HUD restores what it covered. The letters it
   covers keep matching, and SdlBuildHiResHudLayer draws it back above the text,
   as SdlCopyFrameWithoutMouse and CompositeHiResMouseCursor do for the menus'
   cursor (#37). */

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

/* Whether a cell still shows the game's letter. Only the letter's own pixels
   count, so a heading's underline, a sprite's edge or a target bracket drawn
   over the cell's background keeps it. A few of them may also have turned
   back into that background: the damage report's first erase draws its
   black leader line from the top corner of the display, through the
   heading's G (#37). Erased text loses all of them, and a letter crossed
   by anything else changes colour. Pixels under the flight pointer do not
   count; pointer is 1 when pixels is the space buffer it was drawn into, 2
   when it is the screen. A space has nothing to compare: it stays with the
   letters next to it (DropHiResSpaceRuns). */
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

/* After an off-screen buffer was copied to the screen: the glyphs it shows
   within the copied area follow it, moved by the copy's offset. One it no
   longer shows there, such as a radio message's letter under the flight
   pointer, which is drawn into the space buffer after the text, follows it
   hidden, keeping its place and age, until it has been gone for
   HIRES_HUD_HIDDEN_FRAME_LIMIT copies (#37). Forgotten sooner, it came back
   as new text and the stale-run rule dropped its line's other hidden
   letters, so the line split and slid. */
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
                             int height)
{
    int column;
    int row;
    unsigned char value;

    for (column = 0; column < width; column++) {
        for (row = 0; row < height; row++) {
            value = bitmap[row * width + column];
            if (value != 0xff && value != g_pHiResFont[3])
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

/* Reuse the cursor's restore spans as an opacity mask, including clipping
   and its hotspot: comparing the frame before and after drawing would miss
   opaque cursor pixels whose colour matches the background. Copy the drawn
   cursor above the text at the base frame's 2x scale. */
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

/* A letter something is drawn over, as the weapon display's missile crosses
   its text, is hidden but keeps its place in its line: the game restores
   what it covered without drawing the letter again. It is forgotten once
   newer text is drawn in its line, or once the whole line has been gone
   for a while. */
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

/* Forgets the glyphs the frame has stopped showing and hides those it does
   not show now. The shown glyphs' letters are erased from the presented
   frame, so recorded sprites still show between transparent HUD letters,
   and the layer gets the hi-res glyphs. A
   run of adjacent cells is set with the hi-res font's own spacing, centred
   when the game centred it and otherwise from its left edge. A run a few
   pixels wider than its cells, as digits are, starts in its first glyph's
   leading blank columns and then closes gaps of two or more blank columns;
   ink is never cut. */
void SdlBuildHiResHudLayer(const unsigned char *pixels, unsigned char *frame)
{
    const HiResGlyph *glyph;
    const unsigned char *bitmap;
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
    if (kept == 0 || g_pHiResFont == 0) {
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
    fontInk = g_pHiResFont[2];
    fontBackground = g_pHiResFont[3];
    for (first = 0; first < kept; first = last) {
        last = first + 1;
        while (last < kept && HiResGlyphsAdjacent(&g_aHiResGlyphs[last - 1],
                                                  &g_aHiResGlyphs[last]))
            last++;
        total = 0;
        for (index = first; index < last; index++) {
            if (FindHiResGlyph(g_aHiResGlyphs[index].character, &width,
                               &height) != 0)
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
            bitmap = FindHiResGlyph(g_aHiResGlyphs[first].character, &width,
                                    &height);
            lead = bitmap != 0 ? HiResLeadingBlank(bitmap, width, height) : 0;
            if (lead > overflow)
                lead = overflow;
            left -= lead;
            overflow -= lead;
        }
        for (index = first; index < last; index++) {
            glyph = &g_aHiResGlyphs[index];
            bitmap = FindHiResGlyph(glyph->character, &width, &height);
            if (bitmap == 0)
                continue;
            if (overflow > 0 && index > first &&
                HiResLeadingBlank(bitmap, width, height) >= 2) {
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
