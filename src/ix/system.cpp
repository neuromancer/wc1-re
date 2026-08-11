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

unsigned int g_dwIxSystemFlags_00598608;
IxSound *g_pFreeSoundList_0059860c;
IxSample *g_pSampleList_00598610;
int g_nActiveVoices_00598614;
int g_nSystemVoiceCount_00598618;
IxSound *g_pActiveSoundList_0059861c;
IxSound *g_pWaitingSoundList_00598620;

#define IX_SYSTEM_FILE "D:\\Rnd\\prj\\ix\\src\\sound\\system.cpp"

/* Function start: 0x00447200 */   /* source line 33 */
extern "C" int ix_system_init(void)
{
    if ((g_dwIxSystemFlags_00598608 & 1) == 0) {
        if (ix_dsp_init() != 0) {
            ix_log_printf("Warning [%s - %d]:\n", IX_SYSTEM_FILE, 33);
            ix_log_printf("Failed to init DSP");
            return -1;
        }
        g_nActiveVoices_00598614 = 0;
        g_nSystemVoiceCount_00598618 = ix_dsp_get_voice_count();
        g_pWaitingSoundList_00598620 = 0;
        g_pActiveSoundList_0059861c = g_pWaitingSoundList_00598620;
        g_pFreeSoundList_0059860c = g_pActiveSoundList_0059861c;
        g_pSampleList_00598610 = 0;
        ix_system_set_master_volume(0xffff);
        g_dwIxSystemFlags_00598608 |= 1;
    }
    return 0;
}

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

/* Function start: 0x004473F3 */
extern "C" void ix_system_shutdown(void)
{
    if ((g_dwIxSystemFlags_00598608 & 1) != 0) {
        ix_system_delete_all_sounds();
        ix_system_delete_all_samples();
        ix_dsp_shutdown();
        g_dwIxSystemFlags_00598608 &= ~1U;
    }
}

/* Function start: 0x00447426 */
extern "C" void ix_system_configure(int option, void *value)
{
    ix_dsp_configure(option, value);
}

/* Function start: 0x00447446 */
extern "C" unsigned short ix_system_get_master_volume(void)
{
    return ix_dsp_get_master_volume();
}

/* Function start: 0x0044745B */
extern "C" void ix_system_set_master_volume(unsigned short volume)
{
    ix_dsp_set_master_volume(volume);
}

/* Function start: 0x00447477 */
extern "C" int ix_system_get_voice_count(void)
{
    return g_nSystemVoiceCount_00598618;
}

/* Function start: 0x0044748C */   /* source line 137 */
extern "C" void ix_system_set_voice_count(int voiceCount)
{
    if (g_nActiveVoices_00598614 != 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_SYSTEM_FILE, 137);
        ix_log_printf("Not a good ideal to change the number of voices while some are playing.");
    }
    ix_dsp_set_voice_count(voiceCount);
    g_nSystemVoiceCount_00598618 = ix_dsp_get_voice_count();
}

/* Function start: 0x004474E3 */
extern "C" IxSample *ix_system_new_sample(void)
{
    IxSample *sample;

    sample = (IxSample *)g_pIxMalloc_00471990(sizeof(IxSample));
    if (sample != 0)
        sample->ix_sample_construct();
    return sample;
}

/* Function start: 0x00447519 */
extern "C" void ix_system_delete_sample(IxSample *sample)
{
    if (sample != 0) {
        sample->ix_sample_destruct();
        g_pIxFree_00471994(sample);
    }
}

/* Function start: 0x00447548 */
extern "C" void ix_system_delete_all_samples(void)
{
    IxSample *sample;
    IxSample *next;

    sample = g_pSampleList_00598610;
    while (sample != 0) {
        next = sample->next;
        ix_system_delete_sample(sample);
        sample = next;
    }
}

/* Function start: 0x0044758D */
extern "C" IxSound *ix_system_new_sound(IxSample *sample)
{
    IxSound *sound;

    sound = (IxSound *)g_pIxMalloc_00471990(sizeof(IxSound));
    if (sound != 0)
        sound->ix_system_sound_construct_centred(sample);
    return sound;
}

/* Function start: 0x004475C7 */
extern "C" void ix_system_delete_sound(IxSound *sound)
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

/* Function start: 0x00447610 */
extern "C" void ix_system_delete_all_sounds(void)
{
    IxSound *sound;
    IxSound *nextActive;
    IxSound *nextWaiting;
    IxSound *nextFree;

    sound = g_pActiveSoundList_0059861c;
    while (sound != 0) {
        nextActive = sound->next;
        ix_system_delete_sound(sound);
        sound = nextActive;
    }
    sound = g_pWaitingSoundList_00598620;
    while (sound != 0) {
        nextWaiting = sound->next;
        ix_system_delete_sound(sound);
        sound = nextWaiting;
    }
    sound = g_pFreeSoundList_0059860c;
    while (sound != 0) {
        nextFree = sound->next;
        ix_system_delete_sound(sound);
        sound = nextFree;
    }
}

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

/* Function start: 0x00447974 */
IxSound *ix_system_find_lowest_playing(IxSound *sound,
                                       unsigned int maximumPriority)
{
    IxSound *best = 0;

    while (sound != 0) {
        if (sound->priority < maximumPriority) {
            maximumPriority = sound->priority;
            best = sound;
        }
        sound = sound->next;
    }
    return best;
}

/* Function start: 0x004479C7 */
int ix_system_find_free_voice(void)
{
    int voice = 0;

    while (voice < g_nSystemVoiceCount_00598618) {
        if ((ix_dspv_get_flags(voice) & IX_VOICE_ACTIVE) == 0)
            return voice;
        voice++;
    }
    return -1;
}

/* Function start: 0x00447A20 */
void IxSound::ix_system_sound_init(IxSample *newSample,
                                   unsigned char newBasePriority,
                                   unsigned int initialVolume,
                                   int newPitchOffset,
                                   unsigned short newPan)
{
    flags = IX_SOUND_ALLOCATED;
    voice = -1;
    sample = newSample;
    pitchOffset = newPitchOffset;
    volume = 0xffff;
    pan = newPan;
    basePriority = newBasePriority;
    previous = 0;
    next = previous;
    stopTime = (unsigned int)-1;
    startTime = stopTime;
    priority = ((int)volume << 8) / 0xffff +
               (pitchOffset << 8) / 0xac44 + basePriority;
    next = g_pFreeSoundList_0059860c;
    previous = 0;
    if (g_pFreeSoundList_0059860c != 0)
        g_pFreeSoundList_0059860c->previous = this;
    g_pFreeSoundList_0059860c = this;
}

/* Function start: 0x00447B17 */
void IxSound::ix_system_sound_construct(IxSample *newSample,
                                        unsigned char newBasePriority)
{
    ix_system_sound_init(newSample, newBasePriority, 0xffff, 0, 0);
}

/* Function start: 0x00447B48 */
void IxSound::ix_system_sound_construct_centred(IxSample *newSample)
{
    ix_system_sound_init(newSample, 0x80, 0xffff, 0, 0);
}

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

/* Function start: 0x00447BE7 */
void IxSound::ix_system_sound_set_volume(unsigned short newVolume)
{
    flags |= IX_SOUND_VOLUME_DIRTY;
    volume = newVolume;
    ix_sound_reprioritise();
}

/* Function start: 0x00447C18 */
void IxSound::ix_system_sound_set_pan(unsigned short newPan)
{
    flags |= IX_SOUND_PAN_DIRTY;
    pan = newPan;
}

/* Function start: 0x00447C44 */
void IxSound::ix_system_sound_set_frequency(int newPitchOffset)
{
    flags |= IX_SOUND_FREQUENCY_DIRTY;
    if (-(int)(sample->frequency - 1) <= newPitchOffset) {
        pitchOffset = newPitchOffset < 0xac44 ? newPitchOffset : 0xac44;
    } else {
        pitchOffset = -(int)(sample->frequency - 1);
    }
    ix_sound_reprioritise();
}

/* Function start: 0x00447CAF */
void IxSound::ix_system_sound_set_priority(unsigned char newBasePriority)
{
    basePriority = newBasePriority;
    ix_sound_reprioritise();
}
