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
void __fastcall ix_sound_start(IxSound *sound)
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

    if (g_nActiveVoices_00598614 < g_nSystemVoiceCount_00598618) {
        voice = ix_system_find_free_voice();

        if (sound == g_pFreeSoundList_0059860c)
            g_pFreeSoundList_0059860c = sound->next;
        if (sound->next != 0)
            sound->next->previous = sound->previous;
        if (sound->previous != 0)
            sound->previous->next = sound->next;
        sound->next = g_pActiveSoundList_0059861c;
        sound->previous = 0;
        if (g_pActiveSoundList_0059861c != 0)
            g_pActiveSoundList_0059861c->previous = sound;
        g_pActiveSoundList_0059861c = sound;
        ix_system_assign_voice(sound, voice);
    } else {
        lowestPlaying = ix_system_find_lowest_playing(
            g_pActiveSoundList_0059861c, sound->priority);
        if (lowestPlaying != 0) {
            releasedVoice = ix_system_release_voice(lowestPlaying);

            if (lowestPlaying == g_pActiveSoundList_0059861c)
                g_pActiveSoundList_0059861c = lowestPlaying->next;
            if (lowestPlaying->next != 0)
                lowestPlaying->next->previous = lowestPlaying->previous;
            if (lowestPlaying->previous != 0)
                lowestPlaying->previous->next = lowestPlaying->next;
            lowestPlaying->next = g_pWaitingSoundList_00598620;
            lowestPlaying->previous = 0;
            if (g_pWaitingSoundList_00598620 != 0)
                g_pWaitingSoundList_00598620->previous = lowestPlaying;
            g_pWaitingSoundList_00598620 = lowestPlaying;

            if (sound == g_pFreeSoundList_0059860c)
                g_pFreeSoundList_0059860c = sound->next;
            if (sound->next != 0)
                sound->next->previous = sound->previous;
            if (sound->previous != 0)
                sound->previous->next = sound->next;
            sound->next = g_pActiveSoundList_0059861c;
            sound->previous = 0;
            if (g_pActiveSoundList_0059861c != 0)
                g_pActiveSoundList_0059861c->previous = sound;
            g_pActiveSoundList_0059861c = sound;
            ix_system_assign_voice(sound, releasedVoice);
        } else {
            if (sound == g_pFreeSoundList_0059860c)
                g_pFreeSoundList_0059860c = sound->next;
            if (sound->next != 0)
                sound->next->previous = sound->previous;
            if (sound->previous != 0)
                sound->previous->next = sound->next;
            sound->next = g_pWaitingSoundList_00598620;
            sound->previous = 0;
            if (g_pWaitingSoundList_00598620 != 0)
                g_pWaitingSoundList_00598620->previous = sound;
            g_pWaitingSoundList_00598620 = sound;
        }
    }
}

/* Function start: 0x0044801E */   /* source lines 151, 152 */
void __fastcall ix_sound_release(IxSound *sound)
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
void __fastcall ix_sound_stop(IxSound *sound)
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
            g_pWaitingSoundList_00598620, 0);

        if (sound == g_pActiveSoundList_0059861c)
            g_pActiveSoundList_0059861c = sound->next;
        if (sound->next != 0)
            sound->next->previous = sound->previous;
        if (sound->previous != 0)
            sound->previous->next = sound->next;
        sound->next = g_pFreeSoundList_0059860c;
        sound->previous = 0;
        if (g_pFreeSoundList_0059860c != 0)
            g_pFreeSoundList_0059860c->previous = sound;
        g_pFreeSoundList_0059860c = sound;

        if (replacement != 0) {
            ix_system_assign_voice(replacement, voice);
            if (replacement == g_pWaitingSoundList_00598620)
                g_pWaitingSoundList_00598620 = replacement->next;
            if (replacement->next != 0)
                replacement->next->previous = replacement->previous;
            if (replacement->previous != 0)
                replacement->previous->next = replacement->next;
            replacement->next = g_pActiveSoundList_0059861c;
            replacement->previous = 0;
            if (g_pActiveSoundList_0059861c != 0)
                g_pActiveSoundList_0059861c->previous = replacement;
            g_pActiveSoundList_0059861c = replacement;
        }
    } else {
        if (sound == g_pWaitingSoundList_00598620)
            g_pWaitingSoundList_00598620 = sound->next;
        if (sound->next != 0)
            sound->next->previous = sound->previous;
        if (sound->previous != 0)
            sound->previous->next = sound->next;
        sound->next = g_pFreeSoundList_0059860c;
        sound->previous = 0;
        if (g_pFreeSoundList_0059860c != 0)
            g_pFreeSoundList_0059860c->previous = sound;
        g_pFreeSoundList_0059860c = sound;
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

    newPriority = ((unsigned int)volume << 8) / 0xffff +
                  (pitchOffset << 8) / 0xac44 + basePriority;
    if ((flags & IX_SOUND_HAS_VOICE) != 0) {
        if (priority > newPriority) {
            waitingSound = ix_system_find_highest_waiting(
                g_pWaitingSoundList_00598620, newPriority);
            if (waitingSound != 0) {
                releasedVoice = ix_system_release_voice(this);

                if (this == g_pActiveSoundList_0059861c)
                    g_pActiveSoundList_0059861c = next;
                if (next != 0)
                    next->previous = previous;
                if (previous != 0)
                    previous->next = next;
                next = g_pWaitingSoundList_00598620;
                previous = 0;
                if (g_pWaitingSoundList_00598620 != 0)
                    g_pWaitingSoundList_00598620->previous = this;
                g_pWaitingSoundList_00598620 = this;

                if (waitingSound == g_pWaitingSoundList_00598620)
                    g_pWaitingSoundList_00598620 = waitingSound->next;
                if (waitingSound->next != 0)
                    waitingSound->next->previous = waitingSound->previous;
                if (waitingSound->previous != 0)
                    waitingSound->previous->next = waitingSound->next;
                waitingSound->next = g_pActiveSoundList_0059861c;
                waitingSound->previous = 0;
                if (g_pActiveSoundList_0059861c != 0)
                    g_pActiveSoundList_0059861c->previous = waitingSound;
                g_pActiveSoundList_0059861c = waitingSound;
                ix_system_assign_voice(waitingSound, releasedVoice);
            }
        }
    } else if ((flags & IX_SOUND_PLAYING) != 0 && priority < newPriority) {
        playingSound = ix_system_find_lowest_playing(
            g_pActiveSoundList_0059861c, newPriority);
        if (playingSound != 0) {
            reassignedVoice = ix_system_release_voice(playingSound);

            if (playingSound == g_pActiveSoundList_0059861c)
                g_pActiveSoundList_0059861c = playingSound->next;
            if (playingSound->next != 0)
                playingSound->next->previous = playingSound->previous;
            if (playingSound->previous != 0)
                playingSound->previous->next = playingSound->next;
            playingSound->next = g_pWaitingSoundList_00598620;
            playingSound->previous = 0;
            if (g_pWaitingSoundList_00598620 != 0)
                g_pWaitingSoundList_00598620->previous = playingSound;
            g_pWaitingSoundList_00598620 = playingSound;

            if (this == g_pWaitingSoundList_00598620)
                g_pWaitingSoundList_00598620 = next;
            if (next != 0)
                next->previous = previous;
            if (previous != 0)
                previous->next = next;
            next = g_pActiveSoundList_0059861c;
            previous = 0;
            if (g_pActiveSoundList_0059861c != 0)
                g_pActiveSoundList_0059861c->previous = this;
            g_pActiveSoundList_0059861c = this;
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
int IxSound::ix_sound_is_playing(void)
{
    return (flags & IX_SOUND_PLAYING) != 0;
}

/* Function start: 0x004486B0 */
void IxSample::ix_sample_construct(void)
{
    flags = 0;
    buffer = 0;
    next = g_pSampleList_00598610;
    previous = 0;
    if (g_pSampleList_00598610 != 0)
        g_pSampleList_00598610->previous = this;
    g_pSampleList_00598610 = this;
}

/* Function start: 0x0044870F */
void IxSample::ix_sample_destruct(void)
{
    if (buffer != 0) {
        ix_dsp_free(buffer);
        buffer = 0;
    }
    if (this == g_pSampleList_00598610)
        g_pSampleList_00598610 = next;
    if (next != 0)
        next->previous = previous;
    if (previous != 0)
        previous->next = next;
}
