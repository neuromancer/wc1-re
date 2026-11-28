/* The renderer's choice and the ships' thruster positions (video_state.c). */
#ifndef SDL_PORT_VIDEO_STATE_H
#define SDL_PORT_VIDEO_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

void SdlSetThrusterScreenPosition(short object, float x, float y);
void SdlGetThrusterScreenPosition(short object, float *x, float *y);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_VIDEO_STATE_H */
