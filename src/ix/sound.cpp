/*
 *  ix / D:\Rnd\prj\ix\src\sound\sound.cpp
 *
 *  Original address range: 0x00447A20 - 0x004486AF   (14 functions)
 *  Assert paths identify the source; linker INT3 fill establishes the object
 *  boundaries (see docs/ORDER.md).
 *
 *  Implement in ADDRESS ORDER -- MSVC emits functions in source order, so the
 *  list below is the original source order of this file.
 */
#include "ix.h"
#include <assert.h>

#define IX_SOUND_FILE "D:\\Rnd\\prj\\ix\\src\\sound\\sound.cpp"

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
    next = pIdleSoundList;
    previous = 0;
    if (pIdleSoundList != 0)
        pIdleSoundList->previous = this;
    pIdleSoundList = this;
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
    if (sound == pIdleSoundList)
        pIdleSoundList = sound->next;
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

/* Function start: 0x00447CD8 */   /* source line 103 */
extern "C" void __fastcall ix_sound_start(IxSound *sound)
{
    int voice;
    IxSound *lowestPlaying;
    int releasedVoice;

    if ((sound->flags & IX_SOUND_PLAYING) != 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_SOUND_FILE, 103);
        ix_log_printf("sound already started!");
        return;
    }

    sound->flags |= IX_SOUND_PLAYING;
    sound->startTime = ix_dsp_get_tick();
    if ((sound->sample->flags & IX_SAMPLE_LOOPING) != 0) {
        sound->flags |= IX_SOUND_LOOPING;
        sound->flags &= ~IX_SOUND_RELEASE_PENDING;
        sound->stopTime = (unsigned int)-1;
    } else {
        sound->stopTime = sound->startTime +
            (sound->sample->sampleCount * 15) /
            (sound->sample->frequency + sound->pitchOffset);
    }

    if (nActiveVoices < nSystemVoices) {
        voice = ix_system_find_free_voice();

        if (sound == pIdleSoundList)
            pIdleSoundList = sound->next;
        if (sound->next != 0)
            sound->next->previous = sound->previous;
        if (sound->previous != 0)
            sound->previous->next = sound->next;
        sound->next = pPlayingSoundList;
        sound->previous = 0;
        if (pPlayingSoundList != 0)
            pPlayingSoundList->previous = sound;
        pPlayingSoundList = sound;
        ix_system_assign_voice(sound, voice);
    } else {
        lowestPlaying = ix_system_find_lowest_playing(
            pPlayingSoundList, sound->priority);
        if (lowestPlaying != 0) {
            releasedVoice = ix_system_release_voice(lowestPlaying);

            if (lowestPlaying == pPlayingSoundList)
                pPlayingSoundList = lowestPlaying->next;
            if (lowestPlaying->next != 0)
                lowestPlaying->next->previous = lowestPlaying->previous;
            if (lowestPlaying->previous != 0)
                lowestPlaying->previous->next = lowestPlaying->next;
            lowestPlaying->next = pQueuedSoundList;
            lowestPlaying->previous = 0;
            if (pQueuedSoundList != 0)
                pQueuedSoundList->previous = lowestPlaying;
            pQueuedSoundList = lowestPlaying;

            if (sound == pIdleSoundList)
                pIdleSoundList = sound->next;
            if (sound->next != 0)
                sound->next->previous = sound->previous;
            if (sound->previous != 0)
                sound->previous->next = sound->next;
            sound->next = pPlayingSoundList;
            sound->previous = 0;
            if (pPlayingSoundList != 0)
                pPlayingSoundList->previous = sound;
            pPlayingSoundList = sound;
            ix_system_assign_voice(sound, releasedVoice);
        } else {
            if (sound == pIdleSoundList)
                pIdleSoundList = sound->next;
            if (sound->next != 0)
                sound->next->previous = sound->previous;
            if (sound->previous != 0)
                sound->previous->next = sound->next;
            sound->next = pQueuedSoundList;
            sound->previous = 0;
            if (pQueuedSoundList != 0)
                pQueuedSoundList->previous = sound;
            pQueuedSoundList = sound;
        }
    }
}

/* Function start: 0x0044801E */   /* source lines 151, 152 */
extern "C" void __fastcall ix_sound_release(IxSound *sound)
{
    if ((sound->flags & IX_SOUND_PLAYING) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_SOUND_FILE, 151);
        ix_log_printf("can't release non playing sound!");
        return;
    }
    if ((sound->flags & IX_SOUND_LOOPING) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_SOUND_FILE, 152);
        ix_log_printf("can't release non looping sound!");
        return;
    }
    if ((sound->flags & IX_SOUND_HAS_VOICE) != 0) {
        ix_dspv_clear_flag4(sound->voice);
        sound->flags |= IX_SOUND_RELEASE_PENDING;
    } else {
        ix_sound_stop(sound);
    }
}

/* Function start: 0x004480CF */   /* source line(s) 165: can't stop sound that's not playing! */
extern "C" void __fastcall ix_sound_stop(IxSound *sound)
{
    IxSound *replacement;
    int voice;

    if ((sound->flags & IX_SOUND_PLAYING) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_SOUND_FILE, 165);
        ix_log_printf("can't stop sound that's not playing!");
        return;
    }

    sound->flags &= ~IX_SOUND_PLAYING;
    if ((sound->flags & IX_SOUND_HAS_VOICE) != 0) {
        voice = ix_system_release_voice(sound);
        replacement = ix_system_find_highest_waiting(
            pQueuedSoundList, 0);

        if (sound == pPlayingSoundList)
            pPlayingSoundList = sound->next;
        if (sound->next != 0)
            sound->next->previous = sound->previous;
        if (sound->previous != 0)
            sound->previous->next = sound->next;
        sound->next = pIdleSoundList;
        sound->previous = 0;
        if (pIdleSoundList != 0)
            pIdleSoundList->previous = sound;
        pIdleSoundList = sound;

        if (replacement != 0) {
            ix_system_assign_voice(replacement, voice);
            if (replacement == pQueuedSoundList)
                pQueuedSoundList = replacement->next;
            if (replacement->next != 0)
                replacement->next->previous = replacement->previous;
            if (replacement->previous != 0)
                replacement->previous->next = replacement->next;
            replacement->next = pPlayingSoundList;
            replacement->previous = 0;
            if (pPlayingSoundList != 0)
                pPlayingSoundList->previous = replacement;
            pPlayingSoundList = replacement;
        }
    } else {
        if (sound == pQueuedSoundList)
            pQueuedSoundList = sound->next;
        if (sound->next != 0)
            sound->next->previous = sound->previous;
        if (sound->previous != 0)
            sound->previous->next = sound->next;
        sound->next = pIdleSoundList;
        sound->previous = 0;
        if (pIdleSoundList != 0)
            pIdleSoundList->previous = sound;
        pIdleSoundList = sound;
    }

    if ((sound->flags & IX_SOUND_DELETE_ON_STOP) != 0)
        ix_system_delete_sound(sound);
}

/* Function start: 0x0044831A */
void IxSound::ix_sound_reprioritise(void)
{
    unsigned int newPriority;

    newPriority = (volume << 8) / 0xffff +
                  (pitchOffset << 8) / 0xac44 + basePriority;
    if ((flags & IX_SOUND_HAS_VOICE) != 0) {
        if (priority > newPriority) {
            IxSound *waitingSound = ix_system_find_highest_waiting(
                pQueuedSoundList, newPriority);
            if (waitingSound != 0) {
                int releasedVoice = ix_system_release_voice(this);

                if (this == pPlayingSoundList)
                    pPlayingSoundList = next;
                if (next != 0)
                    next->previous = previous;
                if (previous != 0)
                    previous->next = next;
                next = pQueuedSoundList;
                previous = 0;
                if (pQueuedSoundList != 0)
                    pQueuedSoundList->previous = this;
                pQueuedSoundList = this;

                if (waitingSound == pQueuedSoundList)
                    pQueuedSoundList = waitingSound->next;
                if (waitingSound->next != 0)
                    waitingSound->next->previous = waitingSound->previous;
                if (waitingSound->previous != 0)
                    waitingSound->previous->next = waitingSound->next;
                waitingSound->next = pPlayingSoundList;
                waitingSound->previous = 0;
                if (pPlayingSoundList != 0)
                    pPlayingSoundList->previous = waitingSound;
                pPlayingSoundList = waitingSound;
                ix_system_assign_voice(waitingSound, releasedVoice);
            }
        }
    } else if ((flags & IX_SOUND_PLAYING) != 0 && priority < newPriority) {
        IxSound *playingSound = ix_system_find_lowest_playing(
            pPlayingSoundList, newPriority);
        if (playingSound != 0) {
            int reassignedVoice = ix_system_release_voice(playingSound);

            if (playingSound == pPlayingSoundList)
                pPlayingSoundList = playingSound->next;
            if (playingSound->next != 0)
                playingSound->next->previous = playingSound->previous;
            if (playingSound->previous != 0)
                playingSound->previous->next = playingSound->next;
            playingSound->next = pQueuedSoundList;
            playingSound->previous = 0;
            if (pQueuedSoundList != 0)
                pQueuedSoundList->previous = playingSound;
            pQueuedSoundList = playingSound;

            if (this == pQueuedSoundList)
                pQueuedSoundList = next;
            if (next != 0)
                next->previous = previous;
            if (previous != 0)
                previous->next = next;
            next = pPlayingSoundList;
            previous = 0;
            if (pPlayingSoundList != 0)
                pPlayingSoundList->previous = this;
            pPlayingSoundList = this;
            ix_system_assign_voice(this, reassignedVoice);
        }
    }
    priority = newPriority;
}

/* Function start: 0x00448645 */
void IxSound::ix_sound_set_delete_on_stop(int enabled)
{
    if (enabled != 0)
        flags |= IX_SOUND_DELETE_ON_STOP;
    else
        flags &= ~IX_SOUND_DELETE_ON_STOP;
}

/* Function start: 0x00448678 */
extern "C" int __fastcall ix_sound_is_playing(IxSound *sound)
{
    return (sound->flags & IX_SOUND_PLAYING) != 0;
}
