/*
 *  ix / D:\Rnd\prj\ix\src\sound\sound.cpp
 *
 *  Original address range: 0x00447CD8 - 0x0044879B   (8 functions)
 *  Range is exact: recovered from this module's own assert __FILE__ anchors in
 *  the shipped debug build (see docs/ORDER.md).
 *
 *  Implement in ADDRESS ORDER -- MSVC emits functions in source order, so the
 *  list below is the original source order of this file.
 */
#include "ix.h"

#define IX_SOUND_FILE "D:\\Rnd\\prj\\ix\\src\\sound\\sound.cpp"

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

    if (nActiveVoices < nSystemVoiceCount) {
        voice = ix_system_find_free_voice();

        if (sound == pFreeSoundList)
            pFreeSoundList = sound->next;
        if (sound->next != 0)
            sound->next->previous = sound->previous;
        if (sound->previous != 0)
            sound->previous->next = sound->next;
        sound->next = pActiveSoundList;
        sound->previous = 0;
        if (pActiveSoundList != 0)
            pActiveSoundList->previous = sound;
        pActiveSoundList = sound;
        ix_system_assign_voice(sound, voice);
    } else {
        lowestPlaying = ix_system_find_lowest_playing(
            pActiveSoundList, sound->priority);
        if (lowestPlaying != 0) {
            releasedVoice = ix_system_release_voice(lowestPlaying);

            if (lowestPlaying == pActiveSoundList)
                pActiveSoundList = lowestPlaying->next;
            if (lowestPlaying->next != 0)
                lowestPlaying->next->previous = lowestPlaying->previous;
            if (lowestPlaying->previous != 0)
                lowestPlaying->previous->next = lowestPlaying->next;
            lowestPlaying->next = pWaitingSoundList;
            lowestPlaying->previous = 0;
            if (pWaitingSoundList != 0)
                pWaitingSoundList->previous = lowestPlaying;
            pWaitingSoundList = lowestPlaying;

            if (sound == pFreeSoundList)
                pFreeSoundList = sound->next;
            if (sound->next != 0)
                sound->next->previous = sound->previous;
            if (sound->previous != 0)
                sound->previous->next = sound->next;
            sound->next = pActiveSoundList;
            sound->previous = 0;
            if (pActiveSoundList != 0)
                pActiveSoundList->previous = sound;
            pActiveSoundList = sound;
            ix_system_assign_voice(sound, releasedVoice);
        } else {
            if (sound == pFreeSoundList)
                pFreeSoundList = sound->next;
            if (sound->next != 0)
                sound->next->previous = sound->previous;
            if (sound->previous != 0)
                sound->previous->next = sound->next;
            sound->next = pWaitingSoundList;
            sound->previous = 0;
            if (pWaitingSoundList != 0)
                pWaitingSoundList->previous = sound;
            pWaitingSoundList = sound;
        }
    }
}

/* Function start: 0x0044801E */   /* source lines 151, 152 */
extern "C" void __fastcall ix_sound_release(IxSound *sound)
{
    if ((sound->flags & IX_SOUND_PLAYING) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_SOUND_FILE, 151);
        ix_log_printf("can't release non playing sound!");
    } else if ((sound->flags & IX_SOUND_LOOPING) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_SOUND_FILE, 152);
        ix_log_printf("can't release non looping sound!");
    } else if ((sound->flags & IX_SOUND_HAS_VOICE) != 0) {
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
            pWaitingSoundList, 0);

        if (sound == pActiveSoundList)
            pActiveSoundList = sound->next;
        if (sound->next != 0)
            sound->next->previous = sound->previous;
        if (sound->previous != 0)
            sound->previous->next = sound->next;
        sound->next = pFreeSoundList;
        sound->previous = 0;
        if (pFreeSoundList != 0)
            pFreeSoundList->previous = sound;
        pFreeSoundList = sound;

        if (replacement != 0) {
            ix_system_assign_voice(replacement, voice);
            if (replacement == pWaitingSoundList)
                pWaitingSoundList = replacement->next;
            if (replacement->next != 0)
                replacement->next->previous = replacement->previous;
            if (replacement->previous != 0)
                replacement->previous->next = replacement->next;
            replacement->next = pActiveSoundList;
            replacement->previous = 0;
            if (pActiveSoundList != 0)
                pActiveSoundList->previous = replacement;
            pActiveSoundList = replacement;
        }
    } else {
        if (sound == pWaitingSoundList)
            pWaitingSoundList = sound->next;
        if (sound->next != 0)
            sound->next->previous = sound->previous;
        if (sound->previous != 0)
            sound->previous->next = sound->next;
        sound->next = pFreeSoundList;
        sound->previous = 0;
        if (pFreeSoundList != 0)
            pFreeSoundList->previous = sound;
        pFreeSoundList = sound;
    }

    if ((sound->flags & IX_SOUND_DELETE_ON_STOP) != 0)
        ix_system_delete_sound(sound);
}

/* Function start: 0x0044831A */
void IxSound::ix_sound_reprioritise(void)
{
    unsigned int newPriority;
    IxSound *waitingSound;
    int releasedVoice;
    IxSound *playingSound;
    int reassignedVoice;

    newPriority = (volume << 8) / 0xffff +
                  (pitchOffset << 8) / 0xac44 + basePriority;
    if ((flags & IX_SOUND_HAS_VOICE) != 0) {
        if (priority > newPriority) {
            waitingSound = ix_system_find_highest_waiting(
                pWaitingSoundList, newPriority);
            if (waitingSound != 0) {
                releasedVoice = ix_system_release_voice(this);

                if (this == pActiveSoundList)
                    pActiveSoundList = next;
                if (next != 0)
                    next->previous = previous;
                if (previous != 0)
                    previous->next = next;
                next = pWaitingSoundList;
                previous = 0;
                if (pWaitingSoundList != 0)
                    pWaitingSoundList->previous = this;
                pWaitingSoundList = this;

                if (waitingSound == pWaitingSoundList)
                    pWaitingSoundList = waitingSound->next;
                if (waitingSound->next != 0)
                    waitingSound->next->previous = waitingSound->previous;
                if (waitingSound->previous != 0)
                    waitingSound->previous->next = waitingSound->next;
                waitingSound->next = pActiveSoundList;
                waitingSound->previous = 0;
                if (pActiveSoundList != 0)
                    pActiveSoundList->previous = waitingSound;
                pActiveSoundList = waitingSound;
                ix_system_assign_voice(waitingSound, releasedVoice);
            }
        }
    } else if ((flags & IX_SOUND_PLAYING) != 0 && priority < newPriority) {
        playingSound = ix_system_find_lowest_playing(
            pActiveSoundList, newPriority);
        if (playingSound != 0) {
            reassignedVoice = ix_system_release_voice(playingSound);

            if (playingSound == pActiveSoundList)
                pActiveSoundList = playingSound->next;
            if (playingSound->next != 0)
                playingSound->next->previous = playingSound->previous;
            if (playingSound->previous != 0)
                playingSound->previous->next = playingSound->next;
            playingSound->next = pWaitingSoundList;
            playingSound->previous = 0;
            if (pWaitingSoundList != 0)
                pWaitingSoundList->previous = playingSound;
            pWaitingSoundList = playingSound;

            if (this == pWaitingSoundList)
                pWaitingSoundList = next;
            if (next != 0)
                next->previous = previous;
            if (previous != 0)
                previous->next = next;
            next = pActiveSoundList;
            previous = 0;
            if (pActiveSoundList != 0)
                pActiveSoundList->previous = this;
            pActiveSoundList = this;
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

/* Function start: 0x004486B0 */
void IxSample::ix_sample_construct(void)
{
    flags = 0;
    buffer = 0;
    next = pSampleList;
    previous = 0;
    if (pSampleList != 0)
        pSampleList->previous = this;
    pSampleList = this;
}

/* Function start: 0x0044870F */
void IxSample::ix_sample_destruct(void)
{
    if (buffer != 0) {
        ix_dsp_free(buffer);
        buffer = 0;
    }
    if (this == pSampleList)
        pSampleList = next;
    if (next != 0)
        next->previous = previous;
    if (previous != 0)
        previous->next = next;
}
