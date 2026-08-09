/*
 *  ix / D:\Rnd\prj\ix\src\sound\system.cpp
 *
 *  Original address range: 0x00447200 - 0x00447CD7   (27 functions)
 *  Range is exact: recovered from this module's own assert __FILE__ anchors in
 *  the shipped debug build (see docs/ORDER.md).
 *
 *  Implement in ADDRESS ORDER -- MSVC emits functions in source order, so the
 *  list below is the original source order of this file.
 */
#include "ix.h"

IxSound *g_pFreeSoundList_0059860c;
int g_nActiveVoices_00598614;
IxSound *g_pActiveSoundList_0059861c;
IxSound *g_pWaitingSoundList_00598620;

/* Original address: 0x00447200 */   /* source line(s) 33: Failed to init DSP */
/* TODO: ix_system_init */

/* Function start: 0x004472A7 */
extern "C" void ix_system_service_sounds(void)
{
    if (g_pWaitingSoundList_00598620 != 0) {
        IxSound *sound = g_pWaitingSoundList_00598620;
        unsigned int now = ix_dsp_get_tick();

        while (sound != 0) {
            IxSound *next = sound->next;

            if (sound->stopTime != (unsigned int)-1 &&
                sound->stopTime < now)
                ix_sound_stop(sound);
            sound = next;
        }
    }

    if (g_pActiveSoundList_0059861c != 0) {
        IxSound *sound = g_pActiveSoundList_0059861c;

        while (sound != 0) {
            IxSound *next = sound->next;

            if ((sound->flags & (IX_SOUND_VOLUME_DIRTY |
                                 IX_SOUND_FREQUENCY_DIRTY |
                                 IX_SOUND_PAN_DIRTY)) != 0) {
                unsigned int flags = sound->flags;
                int voice = sound->voice;

                sound->flags &= ~(IX_SOUND_VOLUME_DIRTY |
                                  IX_SOUND_FREQUENCY_DIRTY |
                                  IX_SOUND_PAN_DIRTY);
                if ((flags & IX_SOUND_VOLUME_DIRTY) != 0)
                    ix_dspv_set_volume(voice, sound->volume);
                if ((flags & IX_SOUND_PAN_DIRTY) != 0)
                    ix_dspv_set_pan(voice, sound->pan);
                if ((flags & IX_SOUND_FREQUENCY_DIRTY) != 0)
                    ix_dspv_set_frequency(
                        voice, sound->sample->frequency + sound->pitchOffset);
            }
            if ((ix_dspv_get_flags(sound->voice) & IX_VOICE_ACTIVE) == 0)
                ix_sound_stop(sound);
            sound = next;
        }
    }
}

/* Original address: 0x004473F3 */
/* TODO: ix_system_shutdown */

/* Original address: 0x00447426 */
/* TODO: ForwardIxThreadSignalEventv1 */

/* Original address: 0x00447446 */
/* TODO: ix_system_get_master_volume */

/* Function start: 0x0044745B */
extern "C" void ix_system_set_master_volume(unsigned short volume)
{
    ix_dsp_set_master_volume(volume);
}

/* Original address: 0x00447477 */
/* TODO: ReadDAT00598618 */

/* Original address: 0x0044748C */   /* source line(s) 137: Not a good ideal to change the number of voices while some are playing. */
/* TODO: ix_system_set_voice_count */

/* Original address: 0x004474E3 */
/* TODO: ix_system_new_sample */

/* Original address: 0x00447519 */
/* TODO: ix_system_delete_sample */

/* Original address: 0x00447548 */
/* TODO: ix_system_delete_all_samples */

/* Original address: 0x0044758D */
/* TODO: ix_system_new_sound */

/* Function start: 0x004475C7 */
void ix_system_delete_sound(IxSound *sound)
{
    if (sound != 0) {
        if ((sound->flags & IX_SOUND_PLAYING) != 0) {
            sound->flags &= ~IX_SOUND_DELETE_ON_STOP;
            ix_sound_stop(sound);
        }
        ix_sound_unlink_from_free_list(sound);
        g_pIxFree_00471994(sound);
    }
}

/* Original address: 0x00447610 */
/* TODO: ix_system_delete_all_sounds */

/* Function start: 0x004476B9 */
int ix_system_release_voice(IxSound *sound)
{
    int voice = sound->voice;

    ix_dspv_clear_active(voice);
    sound->voice = -1;
    sound->flags &= ~IX_SOUND_HAS_VOICE;
    g_nActiveVoices_00598614--;
    return voice;
}

/* Function start: 0x004476FA */
void ix_system_assign_voice(IxSound *sound, int voice)
{
    IxSample *sample = sound->sample;
    unsigned int elapsed;
    unsigned int position;
    unsigned int loopLength;

    elapsed = ix_dsp_get_tick() - sound->startTime;
    position = ((sound->pitchOffset + sample->frequency) * elapsed) / 15;
    if ((sound->flags & IX_SOUND_LOOPING) != 0) {
        loopLength = sample->loopEnd - sample->loopStart;
        if (sample->loopEnd < position)
            position = sample->loopStart +
                       (position - sample->loopStart) % loopLength;
        if (sample->rateNumerator == sample->rateDenominator)
            position = sample->bytesPerFrame * position;
        else
            position = (sample->bytesPerFrame * sample->rateNumerator *
                        position) / sample->rateDenominator;
        ix_dspv_set_buffer(
            voice, sample->buffer + sample->loopStart * sample->bytesPerFrame,
            sample->bytesPerFrame * loopLength);
        ix_dspv_set_position(voice, position);
        ix_dspv_set_flag4(voice, 1);
    } else {
        if (sample->rateNumerator == sample->rateDenominator)
            position = sample->bytesPerFrame * position;
        else
            position = (sample->bytesPerFrame * sample->rateNumerator *
                        position) / sample->rateDenominator;
        ix_dspv_set_buffer(voice, sample->buffer,
                           sample->bytesPerFrame * sample->sampleCount);
        ix_dspv_set_position(voice, position);
        ix_dspv_set_flag4(voice, 0);
    }
    ix_dspv_set_channels(voice, sample->channels);
    ix_dspv_set_bits_per_sample(voice, sample->bitsPerSample);
    ix_dspv_set_frequency(voice, sample->frequency + sound->pitchOffset);
    ix_dspv_set_pan(voice, sound->pan);
    ix_dspv_set_volume(voice, sound->volume);
    ix_dspv_set_active(voice);
    sound->voice = voice;
    sound->flags |= IX_SOUND_HAS_VOICE;
    sound->flags &= ~(IX_SOUND_VOLUME_DIRTY |
                      IX_SOUND_FREQUENCY_DIRTY |
                      IX_SOUND_PAN_DIRTY);
    g_nActiveVoices_00598614++;
}

/* Function start: 0x00447921 */
IxSound *ix_system_find_highest_waiting(IxSound *sound,
                                        unsigned int minimumPriority)
{
    IxSound *best = 0;

    while (sound != 0) {
        if (sound->priority > minimumPriority) {
            minimumPriority = sound->priority;
            best = sound;
        }
        sound = sound->next;
    }
    return best;
}

/* Original address: 0x00447974 */
/* TODO: ix_system_find_lowest_playing */

/* Original address: 0x004479C7 */
/* TODO: ix_system_find_free_voice */

/* Original address: 0x00447A20 */
/* TODO: ix_system_sound_init */

/* Original address: 0x00447B17 */
/* TODO: ix_system_sound_construct */

/* Original address: 0x00447B48 */
/* TODO: ix_system_sound_construct_centred */

/* Function start: 0x00447B7A */
void __fastcall ix_sound_unlink_from_free_list(IxSound *sound)
{
    if (sound == g_pFreeSoundList_0059860c)
        g_pFreeSoundList_0059860c = sound->next;
    if (sound->next != 0)
        sound->next->previous = sound->previous;
    if (sound->previous != 0)
        sound->previous->next = sound->next;
    sound->flags &= ~IX_SOUND_ALLOCATED;
}

/* Original address: 0x00447BE7 */
/* TODO: ix_system_sound_set_volume */

/* Original address: 0x00447C18 */
/* TODO: ix_system_sound_set_pan */

/* Original address: 0x00447C44 */
/* TODO: ix_system_sound_set_frequency */

/* Original address: 0x00447CAF */
/* TODO: ix_system_sound_set_priority */
