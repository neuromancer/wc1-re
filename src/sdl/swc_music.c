/* SDL replacement for CODE_12 QTSound +0x0930 and CODE_09's music player.
 * QuickTime note sequences retain their original timing and GM instruments;
 * TinySoundFont supplies a portable substitute for the Mac synthesizer. */
#include "wc1.h"
#include "swc.h"

#include <stdio.h>
#include <string.h>

#define TSF_IMPLEMENTATION
#define TSF_NO_STDIO
#include "../../third_party/tinysoundfont/tsf.h"

typedef struct SwcMusicTrack {
    const char *name;
    SwcMusic music;
    int presets[32];
    int warned;
    char failure[192];
} SwcMusicTrack;

static SwcMusicTrack swcMusicTracks[] = {
    {.name = "ARCADETH.MooV"}, {.name = "MEDALCE1.MooV"},
    {.name = "ARMOR.MooV"}, {.name = "BARRACKS.MooV"},
    {.name = "BEINGTAI.MooV"}, {.name = "BRIEFIN0.MooV"},
    {.name = "DEBRIEF0.MooV"}, {.name = "DEBRIEF1.MooV"},
    {.name = "DEBRIEFI.MooV"}, {.name = "DOCKING.MooV"},
    {.name = "DULLPATR.MooV"}, {.name = "EJECT-IM.MooV"},
    {.name = "FLOUNDER.MooV"}, {.name = "FLYINGTO.MooV"},
    {.name = "FUNERAL.MooV"}, {.name = "GOALLINE.MooV"},
    {.name = "GRIMORES.MooV"}, {.name = "INTRO.MooV"},
    {.name = "LANDING.MooV"}, {.name = "LASTRITE.MooV"},
    {.name = "MISSLETR.MooV"}, {.name = "RECROOM.MooV"},
    {.name = "REGULARC.MooV"}, {.name = "RETURNI0.MooV"},
    {.name = "RETURNI1.MooV"}, {.name = "RETURNIN.MooV"},
    {.name = "REVIEW.MooV"}, {.name = "SUPRWNG1.MooV"},
    {.name = "SUPRWNG2.MooV"}, {.name = "SUPRWNG3.MooV"},
    {.name = "SUPRWNG4.MooV"}, {.name = "VICTORY.MooV"}
};

static tsf *swcSynth;
static SwcMusicTrack *swcMusicTrack;
/* Main thread only: the scene's track and level, kept while music is off. */
static SwcMusicTrack *swcMusicRequest;
static int swcMusicLevel;
static size_t swcMusicEvent;
static uint64_t swcMusicFrame;
static int swcMusicNav = -1;
static int swcFlightTrack;

/* Called only while the device is closed, by SdlFreeSwcSound. */
void SdlFreeSwcMusic(void)
{
    size_t index;

    swcMusicTrack = NULL;
    swcMusicRequest = NULL;
    if (swcSynth != NULL)
        tsf_close(swcSynth);
    swcSynth = NULL;
    for (index = 0; index < SDL_arraysize(swcMusicTracks); index++) {
        SDL_free(swcMusicTracks[index].music.events);
        SDL_zero(swcMusicTracks[index].music);
        swcMusicTracks[index].warned = 0;
        swcMusicTracks[index].failure[0] = '\0';
    }
    swcMusicNav = -1;
    swcFlightTrack = 0;
    swcMusicEvent = 0;
    swcMusicFrame = 0;
}

int SdlInitSwcMusic(void)
{
    const char *override = SDL_getenv("WC1_SWC_SOUNDFONT");
    char *base;
    char path[4096];
    char relative[64];
    SwcBuffer bank = {0};
    SDL_RWops *reader;
    Sint64 length;
    size_t index;
    int part;
    int instrument;
    SwcMusicTrack *track;

    if (swcAudioDevice == 0)
        return 0;
    if (override != NULL && override[0] != '\0') {
        if (SDL_strlcpy(path, override, sizeof(path)) >= sizeof(path))
            return SDL_SetError("SWC SoundFont path is too long");
    } else {
        base = SDL_GetBasePath();
        if (base == NULL)
            goto unavailable;
        length = SDL_snprintf(path, sizeof(path), "%sswc-music.sf2", base);
        SDL_free(base);
        if (length < 0 || length >= (Sint64)sizeof(path))
            return SDL_SetError("SWC SoundFont path is too long");
    }
    reader = SDL_RWFromFile(path, "rb");
    if (reader == NULL)
        goto unavailable;
    length = SDL_RWsize(reader);
    if (length < 12 || length > 128 * 1024 * 1024) {
        SDL_RWclose(reader);
        SDL_SetError("Unsupported SWC SoundFont size: %s", path);
        goto unavailable;
    }
    bank.data = SDL_LoadFile_RW(reader, &bank.size, 1);
    if (bank.data == NULL)
        goto unavailable;
    swcSynth = tsf_load_memory(bank.data, bank.size);
    SDL_free(bank.data);
    if (swcSynth == NULL) {
        SDL_SetError("Cannot load SWC SoundFont: %s", path);
        goto unavailable;
    }
    /* Preallocate every channel and voice on the main thread. In particular,
     * tsf_reset frees channels and must never be used by the loop callback. */
    if (!tsf_set_max_voices(swcSynth, 256) ||
        !tsf_channel_set_presetindex(swcSynth, 31, 0))
        return SDL_OutOfMemory();
    tsf_set_output(swcSynth, TSF_STEREO_INTERLEAVED, 48000, -12.0f);
    for (index = 0; index < SDL_arraysize(swcMusicTracks); index++) {
        track = &swcMusicTracks[index];
        SDL_snprintf(relative, sizeof(relative), "MIDI/%s", track->name);
        if (!SdlResolvePath(relative, path, sizeof(path)))
            return SDL_SetError("SWC music path is too long");
        if (SwcReadMusic(path, &track->music) == 0) {
            for (part = 0; part < 32; part++) {
                instrument = track->music.instruments[part];
                if (instrument == 0)
                    continue;
                track->presets[part] = tsf_get_presetindex(swcSynth,
                    instrument >= 0x4001 ? 128 : 0, (instrument & 0x3fff) - 1);
                if (track->presets[part] < 0) {
                    SDL_SetError("SoundFont lacks GM instrument %d", instrument);
                    SDL_free(track->music.events);
                    SDL_zero(track->music);
                    break;
                }
            }
        }
        if (track->music.events == NULL)
            SDL_strlcpy(track->failure, SDL_GetError(), sizeof(track->failure));
    }
    SDL_ClearError();
    return 0;
unavailable:
    fprintf(stderr, "SWC music unavailable: %s\n", SDL_GetError());
    SDL_ClearError();
    return 0;
}

/* CODE_09 servicetrack +0x13d0 restarts completed QuickTime movies.
 * Both the main thread (under the audio lock) and the callback can restart. */
static void SwcRestartMusic(void)
{
    int part;

    swcMusicEvent = 0;
    swcMusicFrame = 0;
    for (part = 0; part < 32; part++) {
        tsf_channel_sounds_off_all(swcSynth, part);
        tsf_channel_midi_control(swcSynth, part, 121, 0);
        tsf_channel_set_sustain(swcSynth, part, 0);
        tsf_channel_set_pitchwheel(swcSynth, part, 8192);
        if (swcMusicTrack->music.instruments[part] != 0)
            tsf_channel_set_presetindex(swcSynth, part, swcMusicTrack->presets[part]);
    }
}

void SdlStopSwcMusic(void)
{
    if (swcAudioDevice != 0)
        SDL_LockAudioDevice(swcAudioDevice);
    swcMusicTrack = NULL;
    swcMusicRequest = NULL;
    if (swcAudioDevice != 0)
        SDL_UnlockAudioDevice(swcAudioDevice);
}

/* gametrack +0x1168/spacetrack +0x0d1e retain an already selected track.
 * ChangeMusicVolume +0x159e doubles a 0..50 scene level to a percentage. */
void SdlPlaySwcMusic(const char *name, int volume)
{
    SwcMusicTrack *track = NULL;
    size_t index;

    if (swcSynth == NULL || swcAudioDevice == 0)
        return;
    for (index = 0; index < SDL_arraysize(swcMusicTracks); index++) {
        if (strcmp(name, swcMusicTracks[index].name) == 0) {
            track = &swcMusicTracks[index];
            break;
        }
    }
    if (track == NULL) {
        SdlStopSwcMusic();
        return;
    }
    if (track->music.events == NULL && !track->warned) {
        fprintf(stderr, "SWC music %s unavailable: %s\n", name, track->failure);
        track->warned = 1;
    }
    SDL_LockAudioDevice(swcAudioDevice);
    swcMusicRequest = track;
    swcMusicLevel = volume;
    /* Leave headroom for the four effects and foreground speech/movie.
     * DoAdjustVolumes +0x5464 scales the scene level by DATA/0 +0xa7f6. */
    tsf_set_volume(swcSynth, 0.25f * SDL_clamp(volume, 0, 50) / 50.0f *
                   SDL_clamp(nMusicVolumeSetting, 0, 20) / 20.0f *
                   swcOptions.musicVolume / 100.0f);
    /* gametrack +0x1190 and spacetrack +0x0d42 play nothing while music is off. */
    if (!swcOptions.music) {
        swcMusicTrack = NULL;
    } else if (swcMusicTrack != track) {
        swcMusicTrack = track->music.events != NULL ? track : NULL;
        if (swcMusicTrack != NULL)
            SwcRestartMusic();
    }
    SDL_UnlockAudioDevice(swcAudioDevice);
}

/* DoOptionsMenu +0x2db8 disposes of the movie for Turn Music Off; Turn
 * Music On reopens the current track (QTSound) from its start. Volume
 * changes keep the playing position. */
void SdlRefreshSwcMusic(void)
{
    if (swcMusicRequest != NULL)
        SdlPlaySwcMusic(swcMusicRequest->name, swcMusicLevel);
}

/* Audio callback only: render up to each event boundary at 48 kHz. All event
 * storage, instruments, channels and voices already exist before playback. */
void SdlMixSwcMusic(float *samples, int frames)
{
    const SwcMusic *music;
    const SwcMusicEvent *event;
    uint64_t boundary;
    uint64_t end;
    int count;

    if (swcMusicTrack == NULL)
        return;
    music = &swcMusicTrack->music;
    end = (uint64_t)music->duration * 48000 / music->timeScale;
    while (frames > 0) {
        while (swcMusicEvent < music->count &&
            (uint64_t)music->events[swcMusicEvent].tick * 48000 / music->timeScale <= swcMusicFrame) {
            event = &music->events[swcMusicEvent++];
            if (event->type == SWC_MUSIC_NOTE_ON)
                tsf_channel_note_on(swcSynth, event->part, event->key, event->value / 127.0f);
            else if (event->type == SWC_MUSIC_NOTE_OFF)
                tsf_channel_note_off(swcSynth, event->part, event->key);
            else if (event->key == 32)
                /* QT controller 32 is signed 8.8 semitones, not MIDI bank LSB. */
                tsf_channel_set_tuning(swcSynth, event->part, (int16_t)event->value / 256.0f);
            else if (event->key != 0)
                tsf_channel_midi_control(swcSynth, event->part, event->key,
                                        SDL_min(event->value >> 8, 127));
        }
        if (swcMusicFrame >= end) {
            SwcRestartMusic();
            continue;
        }
        boundary = swcMusicEvent < music->count
            ? (uint64_t)music->events[swcMusicEvent].tick * 48000 / music->timeScale : end;
        count = SDL_min((uint64_t)frames, boundary - swcMusicFrame);
        tsf_render_float(swcSynth, samples, count, 1);
        samples += count * 2;
        frames -= count;
        swcMusicFrame += count;
    }
}

/* CODE_09 new_space_music_changes +0x0e88: initial mission type, then changes
 * of action sphere. WC1's function with this name has different arguments
 * and kill-triggered rules, so it cannot select the SWC soundtrack. */
void SdlUpdateSwcFlightMusic(void)
{
    static const char *const names[] = {
        "DULLPATR.MooV", "FLYINGTO.MooV", "FLOUNDER.MooV", "REGULARC.MooV",
        "GRIMORES.MooV", "GOALLINE.MooV", "MISSLETR.MooV", "RETURNI0.MooV",
        "RETURNI1.MooV", "RETURNIN.MooV"
    };
    int part;
    int complete;
    int objective;
    enum ObjectType type;

    /* Retail CODE_03 init_inflight_music +0x0cfa: the arcade track is
     * independent of navigation and combat music changes. */
    if (nTrainSimActive) {
        SdlPlaySwcMusic("ARCADETH.MooV", 25);
        return;
    }
    if (swcSynth == NULL || nCurrentNavigationPoint < 0 ||
        nCurrentNavigationPoint >= ACTIVE_MISSION_NAV_POINT_COUNT)
        return;
    if (swcMusicNav == -1) {
        swcFlightTrack = aeShipMissionType[0] == MISSION_TYPE_STRIKE ? 1 :
            (aeShipMissionType[0] == MISSION_TYPE_DEFEND ||
             aeShipMissionType[0] == MISSION_TYPE_RENDEZVOUS) ? 2 : 0;
    } else if (swcMusicNav != nCurrentNavigationPoint) {
        swcFlightTrack = 0;
        if (cCurrentObjective >= 0 && cCurrentObjective < cMissionObjectiveCountValue &&
            aMissionObjectivesArray[cCurrentObjective].type == OBJECTIVE_HOME_BASE) {
            if (aeShipMissionType[0] == MISSION_TYPE_PATROL) {
                /* CODE_05 triumph +0x69a2..+0x69ce returns zero at the
                 * first unvisited objective; WC1's patrol branch differs. */
                complete = 0;
                for (part = 0; part < (int)SDL_arraysize(abFlightPathList); part++) {
                    objective = abFlightPathList[part];
                    if (objective == -1) {
                        complete = 1;
                        break;
                    }
                    if (objective < 0 || objective >= cMissionObjectiveCountValue || !visited(objective))
                        break;
                }
            } else {
                complete = triumph(0);
            }
            swcFlightTrack = !complete ? 9 :
                aeShipMissionType[0] == MISSION_TYPE_PATROL ? 7 : 8;
        } else {
            for (part = 0; part < 2; part++) {
                type = aMissionNavPoints[nCurrentNavigationPoint].preloadObjectTypes[part];
                if (type == OBJECT_TYPE_ASTEROID_FIELD || type == OBJECT_TYPE_MINE_FIELD) {
                    /* Random3DO(5,6) uses an exclusive upper bound. */
                    swcFlightTrack = 5;
                    break;
                }
            }
            if (swcFlightTrack == 0 && hostile_sphere(0, nCurrentNavigationPoint))
                swcFlightTrack = 2 + RandomBelowOrEqual(1);
        }
    }
    swcMusicNav = nCurrentNavigationPoint;
    /* GameFlow +0x1d3a: ChangeMusicVolume(25) before FlyingLoop. */
    SdlPlaySwcMusic(names[swcFlightTrack], 25);
}
