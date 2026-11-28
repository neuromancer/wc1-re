/* Hi-res HUD text (hires_hud.c). */
#ifndef SDL_PORT_HIRES_HUD_H
#define SDL_PORT_HIRES_HUD_H

struct Viewport;

#ifdef __cplusplus
extern "C" {
#endif

/* The enhanced renderer's hi-res HUD text: RunSpaceFlight, and the
   simulator around it, marks the flight, DrawFontGlyph records HUD-font
   glyphs, and the viewport copies move the ones drawn off screen. */
void SdlBeginHiResHudFlight(void);
void SdlEndHiResHudFlight(void);
void SdlResetHiResHudFont(void);
int SdlBeginHiResGlyph(const struct Viewport *view, int cursorX, int y,
                       int width, int height, unsigned char character,
                       unsigned char colour, unsigned char background,
                       int centered);
void SdlEndHiResGlyph(void);
void SdlCopyHiResGlyphs(const struct Viewport *source,
                        const struct Viewport *destination);
/* The HUD drew the flight pointer into the space buffer. */
void SdlNoteHiResFlightPointer(void);
void SdlCopyHiResGlyphRuns(const struct Viewport *source,
                           const struct Viewport *destination, int originX,
                           int originY);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_HIRES_HUD_H */
