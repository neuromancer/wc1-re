/* Smooth spaceflight's in-between frames (interpolation.c). */
#ifndef SDL_PORT_INTERPOLATION_H
#define SDL_PORT_INTERPOLATION_H

#ifdef __cplusplus
extern "C" {
#endif

/* Spaceflight frame interpolation (src/port/graphics/interpolation.c). */
void SdlSetFrameInterpolation(int enabled);
void SdlResetFrameInterpolation(void);
void SdlCaptureFlightPoses(void);
void SdlSnapshotFlightTransform(void);
void SdlSnapshotFlightChildren(void);
void SdlCaptureFlightView(void);
void SdlDeferFlightFrame(int flightLoop);
void SdlSetViewOverlaySubtitle(short colour, const char *text);
void SdlSetViewOverlayLogo(short fromDistance, short toDistance, short y);
void SdlSetViewOverlayZoomText(const char *text, short x, short y,
                               short fromScale, short toScale);
struct Shape;
void SdlSetViewOverlaySprite(int id, struct Shape *shape, short frame,
                             short flip, short x, short y, short scale);
void SdlSetFlightStepPresents(int presents);
void SdlClearViewOverlay(void);
int SdlFlightFrameDeferred(void);
int SdlDrawingInterpolatedFrame(void);
void SdlDrawInterpolatedFlightFrame(int alpha);
void SdlPresentInterpolatedFlightFrames(long deadline, long interval);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_INTERPOLATION_H */
