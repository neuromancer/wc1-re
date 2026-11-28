/* The emulated Roland MT-32 (mt32.c). */
#ifndef SDL_PORT_MT32_H
#define SDL_PORT_MT32_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Roland MT-32 output through Munt's libmt32emu (src/port/audio/mt32.c). */
typedef struct SdlMt32Synth SdlMt32Synth;
/* The folder holding the player's ROMs and, optionally, a libmt32emu
 * machine id such as "mt32_1_07" (empty: the best the folder has). */
void SdlSetMt32Roms(const char *directory, const char *machine);
const char *SdlGetMt32RomDirectory(void);
const char *SdlGetMt32Machine(void);
/* 1 when the folder has a playable control and PCM ROM pair, describing
 * it; otherwise 0 and the reason. */
int SdlDescribeMt32Roms(const char *directory, const char *machine,
                        char *text, size_t textSize);
SdlMt32Synth *SdlCreateMt32Synth(unsigned int sampleRate,
                                 const unsigned char *timbres,
                                 size_t timbreSize);
void SdlDestroyMt32Synth(SdlMt32Synth *synth);
/* WC.EXE's sequencer on the MT-32: a channel event of the music, a raw
 * controller for channel 1-16, the start and the end of a song. */
void SdlMt32MusicEvent(SdlMt32Synth *synth, unsigned int status,
                       unsigned int data1, unsigned int data2);
void SdlMt32Controller(SdlMt32Synth *synth, unsigned int channel,
                       unsigned int controller, unsigned int value);
void SdlMt32RewindSong(SdlMt32Synth *synth);
void SdlMt32StopSong(SdlMt32Synth *synth);
/* Music and effects volumes, 256 being full. */
void SdlSetMt32Volumes(SdlMt32Synth *synth, int musicVolume, int effectsVolume);
int SdlStartMt32Effect(SdlMt32Synth *synth, unsigned int soundNumber, int volume,
                       int pan, int id, int priority);
void SdlStopMt32Effects(SdlMt32Synth *synth);
void SdlMixMt32(SdlMt32Synth *synth, short *samples, unsigned int frameCount,
                int addToOutput, void (*stepMusic)(void *context), void *context);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_MT32_H */
