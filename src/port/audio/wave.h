/* Positional playback of wave sound effects (wave.cpp). */
#ifndef SDL_PORT_WAVE_H
#define SDL_PORT_WAVE_H

#ifdef __cplusplus
extern "C" {
#endif

void SdlPlayWaveWithPan(const char *filename, int looping,
                        int volume, int pan);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_WAVE_H */
