/* SDL replacement for QuickDraw text, shared by the SWC cockpit and rooms. */
#include "swc.h"
#include "swc_hud_font.h"

#include <string.h>

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
        character = (unsigned char)*text++;
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
