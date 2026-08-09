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

/* Original address: 0x00447CD8 */   /* source line(s) 103: sound already started! */
/* TODO: ix_sound_start */

/* Original address: 0x0044801E */   /* source line(s) 151;152: can't release non playing sound! | can't release non looping sound! */
/* TODO: ix_sound_release */

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
/* TODO: ix_sound_reprioritise */

/* Function start: 0x00448645 */
/* TODO: ix_sound_set_looping */

/* Function start: 0x00448678 */
/* TODO: ix_sound_is_playing */

/* Function start: 0x004486B0 */
/* TODO: ix_sound_sample_link */

/* Function start: 0x0044870F */
/* TODO: ix_sound_sample_unlink */
