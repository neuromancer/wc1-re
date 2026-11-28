/* OriginFX music and sound effects (originfx.cpp). */
#ifndef SDL_PORT_ORIGINFX_H
#define SDL_PORT_ORIGINFX_H

#include <stddef.h>
#include "audio/mt32.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SdlOriginFxPlayer SdlOriginFxPlayer;
SdlOriginFxPlayer *SdlCreateOriginFxPlayer(
    const unsigned char *midi, size_t midiSize,
    const unsigned char *timbres, size_t timbreSize);
/* A song that drives an MT-32 instead of the OPL2, stepped a frame at a
 * time by SdlMixMt32; 0 once it has ended. */
SdlOriginFxPlayer *SdlCreateOriginFxMt32Player(
    const unsigned char *midi, size_t midiSize, SdlMt32Synth *synth);
int SdlStepOriginFxPlayer(SdlOriginFxPlayer *player);
const unsigned char *SdlGetOriginFxSoundRecords(unsigned int *count);
SdlOriginFxPlayer *SdlCreateOriginFxSoundPlayer(
    const unsigned char *timbres, size_t timbreSize);
void SdlDestroyOriginFxPlayer(SdlOriginFxPlayer *player);
int SdlOriginFxPlayerFinished(const SdlOriginFxPlayer *player);
unsigned int SdlOriginFxPlayerSequencePosition(
    const SdlOriginFxPlayer *player);
void SdlLeaveOriginFxPlayerAtLoop(SdlOriginFxPlayer *player);
void SdlSetOriginFxPlayerBranch(SdlOriginFxPlayer *player, int branch);
int SdlPlayOriginFxSoundEffect(
    SdlOriginFxPlayer *player, unsigned int soundNumber,
    int volume, int pan, int tag, int priority);
void SdlStopOriginFxSoundEffects(SdlOriginFxPlayer *player);
void SdlMixOriginFxSoundEffects(
    SdlOriginFxPlayer *player, short *samples,
    unsigned int frameCount, unsigned int gain);
unsigned int SdlMixOriginFxPlayer(SdlOriginFxPlayer *player,
                                     short *samples,
                                     unsigned int frameCount,
                                     unsigned int gain);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_ORIGINFX_H */
