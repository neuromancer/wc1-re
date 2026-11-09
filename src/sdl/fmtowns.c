/* SDL2 adapter for FM Towns WC.EXP F1.0. Addresses refer to its load image
 * (file offset +0x200). Audio and filename tables come from the user's disc. */
#include "wc1.h"

#define TOWNS_RATE 44100
#define TOWNS_RING 32768
#define TOWNS_EFFECT_HANDLE 0x40000000U

typedef struct TownsSample {
    short *pcm;
    unsigned int count, rate, loopStart, loopEnd;
} TownsSample;

typedef struct TownsChannel {
    TownsSample *sample;
    Uint64 position;
    int effect, source, loop, volume, pan, handle;
} TownsChannel;

typedef struct TownsCdTrack {
    char path[PATH_MAX];
    Sint64 start, end;
} TownsCdTrack;

static SDL_AudioDeviceID townsDevice;
static char townsData[PATH_MAX];
static char effectNames[39][20], radioNames[2][14][11][6][20];
static TownsSample effects[39], radioSample;
static TownsChannel channels[8]; /* Seven effects, one reserved radio channel. */
static TownsCdTrack cdTracks[100];
static SDL_RWops *cdFile;
static Sint64 cdStart, cdEnd, cdPosition;
static short cdRing[TOWNS_RING * 2];
static unsigned int cdRead, cdCount;
static int cdLoop, cdFinished, cdCue = -1, cdRequest = -1;
static int musicGain, effectGain;
static unsigned int nextEffectHandle;

int SdlUsingTownsData(void)
{
    static int detected = -1;
    unsigned char header[8];
    int file;
    int count;

    if (detected >= 0)
        return detected;
    file = _open("GAMEDAT/MODULE.000", 0x8000);
    if (file == -1)
        file = _open("MODULE.000", 0x8000);
    if (file == -1)
        return 0;
    count = _read(file, header, sizeof(header));
    _close(file);
    detected = 0;
    if (count == sizeof(header) && header[7] == 2) {
        file = _open("GAMEDAT/COMMUNIC.JAT", 0x8000);
        if (file == -1)
            file = _open("COMMUNIC.JAT", 0x8000);
        if (file != -1) {
            detected = filelength(file) > 0;
            _close(file);
        }
    }
    return detected;
}

void SdlLoadTownsInstallTable(void)
{
    /* Towns opens filenames directly (WC.EXP load-image 0x6ab9c etc.).
     * Supply the PC engine's logical slots without requiring INSTALL.DAT.
     * Empty slots must retain a name until the final table terminator. */
    static const char *const names[] = {
        "FONTS.FNT", "SCRAMBLE.VGA", "PILOTANM.VGA", "OBJECTS.VGA",
        "BRIEFING.VGA", "RECROOM.VGA", "TALKING.VGA", "MUSIC.MID",
        "COCKPIT.VGA", "TITLES.VGA", "BRIEFING.000", "WINGMEN.VGA",
        "PLANETS.VGA", "COMMUNIC.DAT", "ARROW.VGA", "MODULE.000",
        "SAVEGAME.WLD", "PCSHIP.V00", "PCSHIP.V01", "PCSHIP.V02",
        "PCSHIP.V03", "PCSHIP.V04", "SHIPTYPE.V00", "SHIPTYPE.V01",
        "SHIPTYPE.V02", "SHIPTYPE.V03", "SHIP.V04", "SHIP.V05",
        "SHIP.V06", "SHIP.V07", "SHIP.V08", "SHIPTYPE.V09",
        "SHIPTYPE.V10", "SHIPTYPE.V11", "SHIPTYPE.V12", "SHIPTYPE.V13",
        "SHIP.V14", "SHIP.V15", "SHIP.V16", "SHIP.V17", "SHIP.V18",
        " ", "SHIP.V20", "SHIP.V21", " ", " ", " ", " ", " ", " ", " ",
        "SHIPTYPE.V29", "MODULE.001", " ", " ", " ", "SUPERTM.DRV",
        "TM.DRV", "CAMP.000", "WINGLDR.TIM", "INTRO1.DAT", "CAMP.001",
        "BRIEFING.001", "MIDGAME.V00", "MIDGAME.V01", "MIDGAME.V02",
        "MIDGAME.V03", "MIDGAME.V04", "MIDGAME.V05", " ", " ",
        "SERIES.VGA", "MODULE.002", "BRIEFING.002", "CAMP.002", "TITLE1.VGA"
    };
    unsigned int index;
    size_t size = (sizeof(names) / sizeof(names[0]) + 2) * sizeof(DiskFileRecord);

    pDiskFileRecords = AllocateTaggedMemory(size, 0);
    if (!pDiskFileRecords)
        exit_squadron("Unable to allocate FM Towns file table");
    memset(pDiskFileRecords, 0, size);
    pDiskFileRecords++;
    for (index = 0; index < sizeof(names) / sizeof(names[0]); index++) {
        strcpy(pDiskFileRecords[index].name, names[index]);
        pDiskFileRecords[index].diskNumber = 1;
        pDiskFileRecords[index].logicalFile = index + 1;
    }
    bCurrentRunningFromHardDiskFlag = 1;
}

static Uint32 Towns32(const unsigned char *p)
{
    return p[0] | (Uint32)p[1] << 8 | (Uint32)p[2] << 16 | (Uint32)p[3] << 24;
}

static int TownsDecodeSample(const unsigned char *data, size_t size, TownsSample *sample)
{
    unsigned int count, index, rate, loopStart, loopLength;
    if (size < 32 || size > 4 * 1024 * 1024)
        return 0;
    count = Towns32(data + 12);
    rate = (data[24] | data[25] << 8) * 1000U / 98;
    loopStart = Towns32(data + 16);
    loopLength = Towns32(data + 20);
    /* Several shipped effects overstate their payload by two bytes. */
    if (!count || count > size - 32 + 2 || rate < 1000 || rate > 48000 ||
        data[26] || data[27] || data[28] != 60)
        return 0;
    count = SDL_min(count, size - 32);
    if (!count || loopStart > count + 2 || loopLength > count + 2 - loopStart)
        return 0;
    sample->pcm = SDL_malloc(count * sizeof(short));
    if (!sample->pcm)
        return 0;
    sample->count = count;
    sample->rate = rate;
    sample->loopStart = SDL_min(loopStart, count);
    sample->loopEnd = loopLength ? SDL_min(loopStart + loopLength, count) : 0;
    for (index = 0; index < count; index++) {
        int value = data[32 + index];
        sample->pcm[index] = (value & 0x80 ? -(value & 127) : value) * 256;
    }
    return 1;
}

static int TownsLoadSample(const char *name, TownsSample *sample)
{
    char path[PATH_MAX], resolved[PATH_MAX];
    unsigned char *data;
    size_t size;
    int result;
    /* Validate the fixed-size filename slots read from WC.EXP. */
    if (!*name || strnlen(name, 20) == 20 || strstr(name, ".."))
        return 0;
    if (SDL_snprintf(path, sizeof(path), "%s/%s", townsData, name) >= sizeof(path) ||
        !SdlResolvePath(path, resolved, sizeof(resolved)))
        return 0;
    data = SDL_LoadFile(resolved, &size);
    if (!data)
        return 0;
    result = TownsDecodeSample(data, size, sample);
    SDL_free(data);
    return result;
}

static void TownsMix(void *unused, Uint8 *stream, int bytes)
{
    short *output = (short *)stream;
    unsigned int frame, count = bytes / 4;
    int channel;
    (void)unused;
    for (frame = 0; frame < count; frame++) {
        int left = 0, right = 0;
        if (cdCount) {
            left = cdRing[cdRead * 2] * musicGain / 128;
            right = cdRing[cdRead * 2 + 1] * musicGain / 128;
            cdRead = (cdRead + 1) % TOWNS_RING;
            cdCount--;
        } else if (cdFile && cdPosition == cdEnd && !cdLoop) {
            cdFinished = 1;
        }
        for (channel = 0; channel < 8; channel++) {
            TownsChannel *voice = &channels[channel];
            TownsSample *sample = voice->sample;
            unsigned int position;
            int value;
            if (!sample)
                continue;
            position = voice->position / TOWNS_RATE;
            if (voice->loop && sample->loopEnd > sample->loopStart && position >= sample->loopEnd) {
                Uint64 start = (Uint64)sample->loopStart * TOWNS_RATE;
                voice->position = start + (voice->position - start) %
                    ((Uint64)(sample->loopEnd - sample->loopStart) * TOWNS_RATE);
                position = voice->position / TOWNS_RATE;
            }
            if (position >= sample->count) {
                if (!voice->loop) {
                    voice->sample = 0;
                    continue;
                }
                voice->position %= (Uint64)sample->count * TOWNS_RATE;
                position = voice->position / TOWNS_RATE;
            }
            value = sample->pcm[position] * voice->volume / 127 * effectGain / 128;
            left += value * SDL_min(127 - voice->pan, 64) / 64;
            right += value * SDL_min(voice->pan, 64) / 64;
            voice->position += sample->rate;
        }
        output[frame * 2] = SDL_clamp(left, -32768, 32767);
        output[frame * 2 + 1] = SDL_clamp(right, -32768, 32767);
    }
}

/* CUE INDEX 01 positions address raw 2352-byte sectors, including the data
 * track. Keep physical track numbers; the game's first audio cue is track 2. */
static int TownsReadCue(const char *cue)
{
    char resolved[PATH_MAX], directory[PATH_MAX], line[PATH_MAX + 64];
    char name[4096], path[PATH_MAX], kind[32];
    char *separator;
    FILE *file;
    SDL_RWops *image;
    Sint64 imageSize = 0;
    int track = 0, previous = 0, minute, second, frame, number, cueIndex;
    if (!SdlResolvePath(cue, resolved, sizeof(resolved)) || !(file = fopen(resolved, "r")))
        return 0;
    SDL_strlcpy(directory, resolved, sizeof(directory));
    separator = strrchr(directory, '/');
    if (strrchr(directory, '\\') && (!separator || strrchr(directory, '\\') > separator))
        separator = strrchr(directory, '\\');
    if (separator)
        *separator = 0;
    else
        SDL_strlcpy(directory, ".", sizeof(directory));
    path[0] = 0;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, " FILE \"%4095[^\"]\" %31s", name, kind) == 2) {
            previous = track = 0;
            imageSize = 0;
            if (strcmp(kind, "BINARY"))
                continue;
            if (name[0] == '/' || (strlen(name) > 1 && name[1] == ':'))
                SDL_strlcpy(path, name, sizeof(path));
            else if (SDL_snprintf(path, sizeof(path), "%s/%s", directory, name) >= sizeof(path))
                continue;
            if (SdlResolvePath(path, resolved, sizeof(resolved)) && (image = SDL_RWFromFile(resolved, "rb"))) {
                imageSize = SDL_RWsize(image);
                SDL_RWclose(image);
                SDL_strlcpy(path, resolved, sizeof(path));
            }
        } else if (sscanf(line, " TRACK %d %31s", &number, kind) == 2) {
            track = number > 0 && number < 100 && !strcmp(kind, "AUDIO") ? number : 0;
        } else if (sscanf(line, " INDEX %d %d:%d:%d", &cueIndex, &minute, &second, &frame) == 4 &&
                   cueIndex == 1 && track && imageSize > 0 && minute >= 0 && minute < 100 &&
                   second >= 0 && second < 60 && frame >= 0 && frame < 75) {
            Sint64 start = ((Sint64)minute * 60 * 75 + second * 75 + frame) * 2352;
            if (start >= imageSize)
                continue;
            cdTracks[track].start = start;
            cdTracks[track].end = imageSize / 2352 * 2352;
            SDL_strlcpy(cdTracks[track].path, path, sizeof(cdTracks[track].path));
            if (previous && start > cdTracks[previous].start)
                cdTracks[previous].end = start;
            previous = track;
        }
    }
    fclose(file);
    return cdTracks[2].end > cdTracks[2].start;
}

int SdlInitializeTownsAudio(void)
{
    char root[PATH_MAX], path[PATH_MAX], cuePath[PATH_MAX];
    SDL_RWops *exe;
    unsigned char header[2];
    SDL_AudioSpec desired;
    const char *cue = SDL_getenv("WC1_TOWNS_CUE");
    if (!GetCurrentDirectoryA(sizeof(root), root))
        return 0;
    if (SDL_snprintf(path, sizeof(path), "%s/GAMEDAT", root) >= sizeof(path) ||
        !SdlResolvePath(path, townsData, sizeof(townsData)) ||
        !SdlResolvePath("WC.EXP", path, sizeof(path)) || !(exe = SDL_RWFromFile(path, "rb")))
        return SDL_SetError("FM Towns audio needs WC.EXP beside GAMEDAT"), 0;
    if (SDL_RWsize(exe) != 743037 || SDL_RWread(exe, header, 1, 2) != 2 || memcmp(header, "P3", 2) ||
        SDL_RWseek(exe, 0x1347c, RW_SEEK_SET) < 0 || SDL_RWread(exe, effectNames, 1, sizeof(effectNames)) != sizeof(effectNames) ||
        SDL_RWseek(exe, 0x148b8, RW_SEEK_SET) < 0 || SDL_RWread(exe, radioNames, 1, sizeof(radioNames)) != sizeof(radioNames)) {
        SDL_RWclose(exe);
        return SDL_SetError("Unsupported FM Towns WC.EXP revision"), 0;
    }
    SDL_RWclose(exe);
    if (!cue || !*cue)
        cue = "TOWNS.CUE";
    if (cue[0] == '/' || (strlen(cue) > 1 && cue[1] == ':'))
        SDL_strlcpy(cuePath, cue, sizeof(cuePath));
    else
        SDL_snprintf(cuePath, sizeof(cuePath), "%s/%s", root, cue);
    if (!TownsReadCue(cuePath))
        fprintf(stderr, "FM Towns CD music unavailable: supply TOWNS.CUE and its referenced image.\n");
    if (SDL_InitSubSystem(SDL_INIT_AUDIO))
        return 0;
    SDL_zero(desired);
    desired.freq = TOWNS_RATE;
    desired.format = AUDIO_S16SYS;
    desired.channels = 2;
    desired.samples = 1024;
    desired.callback = TownsMix;
    townsDevice = SDL_OpenAudioDevice(0, 0, &desired, 0, 0);
    if (!townsDevice)
        return 0;
    SDL_PauseAudioDevice(townsDevice, 0);
    return 1;
}

int SdlPlayTownsEffect(int effect, int volume, int pan, int source, int looping)
{
    int index, slot = 0;
    if (!townsDevice || effect <= 0 || effect >= 39)
        return 0;
    if (!effects[effect].pcm && !TownsLoadSample(effectNames[effect], &effects[effect]))
        return 0;
    /* The Towns driver passes the SND loop points to the PCM hardware.
     * Repeated afterburner/alarm requests must not restart a running sample. */
    looping |= effects[effect].loopEnd > effects[effect].loopStart;
    SDL_LockAudioDevice(townsDevice);
    for (index = 0; index < 7; index++) {
        if ((looping || effect == 23 || effect == 32) && channels[index].sample &&
            channels[index].effect == effect && channels[index].source == source) {
            slot = index;
            break;
        }
        if (!channels[index].sample || (channels[slot].sample && channels[index].position > channels[slot].position))
            slot = index;
    }
    if (index == 7) {
        channels[slot].position = 0;
        nextEffectHandle = (nextEffectHandle + 1) & 0x3fffffffU;
        channels[slot].handle = TOWNS_EFFECT_HANDLE | nextEffectHandle;
    }
    channels[slot].sample = &effects[effect];
    channels[slot].effect = effect;
    channels[slot].source = source;
    channels[slot].loop = looping;
    channels[slot].volume = effect == 13 ? volume * 63 / 127 : volume; /* WC.EXP 0x1359c */
    channels[slot].pan = SDL_clamp(pan, 0, 127);
    index = channels[slot].handle;
    SDL_UnlockAudioDevice(townsDevice);
    if (source >= 0 && source < SDL_arraysize(aiSoundEffectSourceActiveArray))
        aiSoundEffectSourceActiveArray[source] = 1;
    if (source == -1 && effect == 12)
        bAfterburnerSfxEnabled = index;
    if (source == -1 && effect == 32)
        nCurrentDamageAlarmSfxHandle = index;
    return index;
}

int SdlStopTownsEffect(int handle)
{
    int index;
    if (!townsDevice || !(handle & TOWNS_EFFECT_HANDLE))
        return 0;
    SDL_LockAudioDevice(townsDevice);
    for (index = 0; index < 7; index++) {
        if (channels[index].handle == handle)
            channels[index].sample = 0;
    }
    SDL_UnlockAudioDevice(townsDevice);
    return 1; /* Stale Towns handles must not stop unrelated effects either. */
}

void SdlStopTownsEffects(void)
{
    if (!townsDevice)
        return;
    SDL_LockAudioDevice(townsDevice);
    memset(channels, 0, sizeof(channels[0]) * 7);
    SDL_UnlockAudioDevice(townsDevice);
}

void SdlStopTownsRadio(void)
{
    if (!townsDevice)
        return;
    SDL_LockAudioDevice(townsDevice);
    channels[7].sample = 0;
    SDL_free(radioSample.pcm);
    SDL_zero(radioSample);
    SDL_UnlockAudioDevice(townsDevice);
}

int SdlTownsRadioPlaying(void)
{
    int playing;
    if (!townsDevice)
        return 0;
    SDL_LockAudioDevice(townsDevice);
    playing = channels[7].sample != 0;
    SDL_UnlockAudioDevice(townsDevice);
    return playing;
}

int SdlPlayTownsRadio(short speaker, short record, short rank)
{
    const char *name;
    int bank = nCampaignDataSet == 2;
    SdlStopTownsRadio();
    if (!townsDevice || speaker < 0 || speaker >= 14 || record < 0 || record >= 11)
        return 0;
    /* WC.EXP 0x812fc prefers the generic slot, then current rank + 1. */
    name = radioNames[bank][speaker][record][0];
    if (!*name && rank >= 0 && rank < 5)
        name = radioNames[bank][speaker][record][rank + 1];
    if (strnlen(name, 20) == 20)
        return 0;
    if (!SDL_strcasecmp(name, "s2\\ja_11_00.fmp"))
        name = "s2/ja/ja_11_00.fmp";
    if (!TownsLoadSample(name, &radioSample))
        return 0;
    SDL_LockAudioDevice(townsDevice);
    channels[7] = (TownsChannel){&radioSample, 0, 0, -1, 0, 127, 64};
    SDL_UnlockAudioDevice(townsDevice);
    return 1;
}

static void TownsStopCd(void)
{
    if (cdFile)
        SDL_RWclose(cdFile);
    cdFile = 0;
    cdCount = cdRead = 0;
    cdCue = cdRequest = -1;
    cdFinished = 1;
    nMusicTrackComplete = 1;
}

void SdlTownsMusicCommand(int track, int command)
{
    /* PC event IDs -> Towns IDs, from corresponding calls to 0x7a958.
     * Towns 0x816d0 omits the short 16/17/23/24/25 stingers entirely. */
    static const signed char cues[] = {
        20, 17, 25, 24, 19, 22, 18, 16, 26, 23, 21, 44,
        31, 29, 30, 11, 15, 13, 14, 0, 4, 2, 3, 1,
        8, 7, 9, 10, 32, 33, 5, 40, 42, 36, 35, 6, 45, 36, 37, 38, 39
    };
    int cue, physical, loop;
    TownsCdTrack *cd;
    if (!townsDevice || track < 0 || track >= SDL_arraysize(cues))
        return;
    cue = cues[track];
    if (track == 15 && !nInitialFlightMusicPending)
        cue = 12;
    if (track == 25 && command == 1)
        cue = 8;
    if (cue == 16 || cue == 17 || cue == 23 || cue == 24 || cue == 25 || cue == 18)
        return;
    SDL_LockAudioDevice(townsDevice);
    if (cue == 2 || cue == 3) {
        TownsStopCd(); /* No CD equivalent for the simulator end stingers. */
        SDL_UnlockAudioDevice(townsDevice);
        return;
    }
    if (cue == cdCue && !cdFinished) {
        SDL_UnlockAudioDevice(townsDevice);
        return;
    }
    TownsStopCd();
    physical = cue <= 1 ? cue + 2 : cue <= 17 ? cue : cue <= 22 ? cue - 1 : cue - 2;
    cd = &cdTracks[physical];
    loop = track <= 5 || (track >= 12 && track <= 18) || track == 20 ||
           track == 30 || track == 35 || (track == 25 && command == 1);
    if (cd->end > cd->start && (cdFile = SDL_RWFromFile(cd->path, "rb"))) {
        cdStart = cdPosition = cd->start;
        cdEnd = cd->end;
        cdLoop = loop;
        cdCue = cue;
        cdRequest = track;
        cdFinished = 0;
        nMusicTrackComplete = 0;
        SDL_RWseek(cdFile, cdStart, RW_SEEK_SET);
    }
    SDL_UnlockAudioDevice(townsDevice);
}

void SdlServiceTownsAudio(void)
{
    short buffer[4096 * 2];
    unsigned int count, index, write;
    if (!townsDevice)
        return;
    SDL_LockAudioDevice(townsDevice);
    musicGain = anVolumeLevels[SDL_clamp(nMusicVolumeSetting / 2, 0, 10)] * 128 / 64000;
    effectGain = anVolumeLevels[SDL_clamp(nSfxVolumeSetting / 2, 0, 10)] * 128 / 64000;
    if (nCurrentMusicTrack < 0 || nMusicPlaybackMode == 0 || nMusicPlaybackMode == 3)
        TownsStopCd();
    if (cdFinished && cdRequest == nCurrentMusicTrack)
        nMusicTrackComplete = 1;
    while (cdFile && TOWNS_RING - cdCount >= 4096) {
        if (cdPosition == cdEnd) {
            if (!cdLoop)
                break;
            cdPosition = cdStart;
            SDL_RWseek(cdFile, cdStart, RW_SEEK_SET);
        }
        count = SDL_min(4096, (cdEnd - cdPosition) / 4);
        if (!count || SDL_RWread(cdFile, buffer, 4, count) != count) {
            TownsStopCd();
            break;
        }
        cdPosition += count * 4;
        write = (cdRead + cdCount) % TOWNS_RING;
        for (index = 0; index < count; index++) {
            cdRing[write * 2] = SDL_SwapLE16(buffer[index * 2]);
            cdRing[write * 2 + 1] = SDL_SwapLE16(buffer[index * 2 + 1]);
            write = (write + 1) % TOWNS_RING;
        }
        cdCount += count;
    }
    SDL_UnlockAudioDevice(townsDevice);
}

void SdlShutdownTownsAudio(void)
{
    unsigned int index;
    if (!townsDevice)
        return;
    SDL_CloseAudioDevice(townsDevice);
    townsDevice = 0;
    TownsStopCd();
    for (index = 0; index < SDL_arraysize(effects); index++) {
        SDL_free(effects[index].pcm);
        SDL_zero(effects[index]);
    }
    SDL_free(radioSample.pcm);
    SDL_zero(radioSample);
    SDL_zero(channels);
    SDL_zero(cdTracks);
}
