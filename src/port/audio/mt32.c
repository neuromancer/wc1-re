/* Roland MT-32 output for WC1's DOS music and sound effects, through Munt's
 * libmt32emu and the player's ROMs. It follows STRAX.DRV's MT-32 section and
 * WC.EXE's sequencer, which shares MIDI channels 2-9 between music and
 * effects; channels here are numbered 1-16. */
#include "wc1sdl.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#include <wchar.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

#include "mt32emu/mt32emu.h"

#define SDL_MT32_MAX_ROM_FILES 64
#define SDL_MT32_MAX_FOLDER_FILES 512
#define SDL_MT32_MIN_ROM_SIZE 32768
#define SDL_MT32_MAX_ROM_SIZE 1048576
#define SDL_MT32_VOICE_COUNT 32
#define SDL_MT32_CHANNEL_COUNT 17
/* 035:0007's channel list: nodes 2-9 are the effect channels, 11 heads the
 * free list and 12 the busy list, oldest first. */
#define SDL_MT32_FREE_LIST 11
#define SDL_MT32_BUSY_LIST 12
#define SDL_MT32_LIST_NODES 13
#define SDL_MT32_SERVICE_RATE 60U
#define SDL_MT32_RENDER_BLOCK 512
#define SDL_MT32_OUTPUT_GAIN 0.5f

enum {
    SDL_MT32_UNOWNED = 0,
    SDL_MT32_MUSIC = 1,
    SDL_MT32_EFFECT = 2
};

/* A playing sound effect (035's 14-byte Voice). */
typedef struct SdlMt32Voice {
    const unsigned char *step;
    int id;
    int ticks;
    unsigned char flags;
    unsigned char channel;
    unsigned char rightChannel;
    unsigned char note;
    unsigned char volume;
    unsigned char pan;
} SdlMt32Voice;

struct SdlMt32Synth {
    mt32emu_context context;
    /* libmt32emu reads a full ROM in place for as long as the context
     * lives; merged halves it copies. */
    unsigned char *romData[2];
    const unsigned char *records;
    unsigned int recordCount;
    /* While SdlMixMt32 runs: the output, the frames rendered into it, and
     * the frame whose messages are being sent. */
    short *output;
    unsigned int renderedFrames;
    unsigned int frame;
    int addToOutput;
    unsigned int sampleRate;
    unsigned int serviceAccumulator;
    /* 035's channel state (DGROUP 790c-7952). */
    unsigned char links[SDL_MT32_LIST_NODES];
    signed char effectPriority[SDL_MT32_LIST_NODES];
    unsigned char owner[SDL_MT32_CHANNEL_COUNT];
    unsigned char sentProgram[SDL_MT32_CHANNEL_COUNT];
    unsigned char musicProgram[SDL_MT32_CHANNEL_COUNT];
    unsigned char musicVolume[SDL_MT32_CHANNEL_COUNT];
    unsigned char musicPan[SDL_MT32_CHANNEL_COUNT];
    int volume;        /* g_musicVolume: 256 is full */
    int effectsVolume; /* the port's own effects volume, 256 is full */
    int voicesReady;
    SdlMt32Voice voices[SDL_MT32_VOICE_COUNT];
};

static char g_szSdlMt32RomDirectory[PATH_MAX];
static char g_szSdlMt32Machine[32];

void SdlSetMt32Roms(const char *directory, const char *machine)
{
    SDL_strlcpy(g_szSdlMt32RomDirectory, directory != 0 ? directory : "",
                sizeof(g_szSdlMt32RomDirectory));
    SDL_strlcpy(g_szSdlMt32Machine, machine != 0 ? machine : "",
                sizeof(g_szSdlMt32Machine));
}

const char *SdlGetMt32RomDirectory(void)
{
    return g_szSdlMt32RomDirectory;
}

const char *SdlGetMt32Machine(void)
{
    return g_szSdlMt32Machine;
}

/* ------------------------------------------------------------------------
 * The player's ROMs */

/* A ROM file and what libmt32emu identified it as. */
typedef struct SdlMt32RomFile {
    char path[PATH_MAX];
    char id[40];
    char description[80];
} SdlMt32RomFile;

/* The files one machine needs: a full ROM, or the two halves of one. */
typedef struct SdlMt32RomSet {
    char machine[32];
    const SdlMt32RomFile *control[2];
    const SdlMt32RomFile *pcm[2];
} SdlMt32RomSet;

/* The game was written for the MT-32 and WCAT recommends the CM-32L, whose
 * extra samples the MT-32 lacks. Prefer the CM-32L family, then the
 * MT-32s from the newest first-generation version. */
static const char *const g_apszSdlMt32MachinePreference[] = {
    "cm32l_1_02", "cm32l_1_00", "mt32_1_07", "mt32_1_06", "mt32_1_05",
    "mt32_1_04", "mt32_bluer", "mt32_2_07", "mt32_2_06", "mt32_2_04",
    "mt32_2_03", "cm32ln_1_00"
};

static int SdlMt32EndsWith(const char *text, const char *suffix)
{
    size_t textLength = strlen(text);
    size_t suffixLength = strlen(suffix);

    return textLength >= suffixLength &&
        strcmp(text + textLength - suffixLength, suffix) == 0;
}

/* Reads a whole ROM candidate; SDL handles UTF-8 names on every host. */
static unsigned char *SdlReadMt32File(const char *path, size_t *size)
{
    SDL_RWops *file;
    unsigned char *data;
    Sint64 length;

    file = SDL_RWFromFile(path, "rb");
    if (file == 0)
        return 0;
    length = SDL_RWsize(file);
    if (length < SDL_MT32_MIN_ROM_SIZE || length > SDL_MT32_MAX_ROM_SIZE) {
        SDL_RWclose(file);
        return 0;
    }
    data = (unsigned char *)SDL_malloc((size_t)length);
    if (data != 0 && SDL_RWread(file, data, 1, (size_t)length) != (size_t)length) {
        SDL_free(data);
        data = 0;
    }
    SDL_RWclose(file);
    *size = (size_t)length;
    return data;
}

static void SdlIdentifyMt32File(const char *path, SdlMt32RomFile *files,
                                int capacity, int *count)
{
    mt32emu_rom_info info;
    unsigned char *data;
    size_t size;
    SdlMt32RomFile *file;
    const char *id;
    const char *description;

    if (*count >= capacity)
        return;
    data = SdlReadMt32File(path, &size);
    if (data == 0)
        return;
    id = 0;
    description = 0;
    if (mt32emu_identify_rom_data(&info, data, size, 0) == MT32EMU_RC_OK) {
        id = info.control_rom_id != 0 ? info.control_rom_id : info.pcm_rom_id;
        description = info.control_rom_id != 0 ? info.control_rom_description
                                               : info.pcm_rom_description;
    }
    if (id != 0) {
        file = &files[(*count)++];
        SDL_strlcpy(file->path, path, sizeof(file->path));
        SDL_strlcpy(file->id, id, sizeof(file->id));
        SDL_strlcpy(file->description, description != 0 ? description : id,
                    sizeof(file->description));
    }
    SDL_free(data);
}

/* Identifies the files of a directory that are MT-32 family ROMs. */
static int SdlScanMt32Roms(const char *directory, SdlMt32RomFile *files,
                           int capacity)
{
    char path[PATH_MAX];
    int examined;
    int count;
#ifdef _WIN32
    struct _wfinddata_t entry;
    intptr_t handle;
    wchar_t *pattern;
    char *name;
#else
    DIR *handle;
    struct dirent *entry;
    struct stat status;
#endif

    count = 0;
    examined = 0;
#ifdef _WIN32
    if (SDL_snprintf(path, sizeof(path), "%s\\*", directory) >= (int)sizeof(path))
        return -1;
    pattern = (wchar_t *)SDL_iconv_string("UTF-16LE", "UTF-8", path, strlen(path) + 1);
    if (pattern == 0)
        return -1;
    handle = _wfindfirst(pattern, &entry);
    SDL_free(pattern);
    if (handle == -1)
        return -1;
    do {
        if ((entry.attrib & _A_SUBDIR) != 0 || entry.size < SDL_MT32_MIN_ROM_SIZE ||
            entry.size > SDL_MT32_MAX_ROM_SIZE)
            continue;
        name = SDL_iconv_string("UTF-8", "UTF-16LE", (const char *)entry.name,
                                (wcslen(entry.name) + 1) * sizeof(wchar_t));
        if (name == 0)
            continue;
        if (SDL_snprintf(path, sizeof(path), "%s\\%s", directory, name) < (int)sizeof(path))
            SdlIdentifyMt32File(path, files, capacity, &count);
        SDL_free(name);
    } while (++examined < SDL_MT32_MAX_FOLDER_FILES && count < capacity &&
             _wfindnext(handle, &entry) == 0);
    _findclose(handle);
#else
    handle = opendir(directory);
    if (handle == 0)
        return -1;
    while ((entry = readdir(handle)) != 0 && examined < SDL_MT32_MAX_FOLDER_FILES &&
           count < capacity) {
        if (entry->d_name[0] == '.')
            continue;
        examined++;
        if (SDL_snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name) >=
                (int)sizeof(path) ||
            stat(path, &status) != 0 || !S_ISREG(status.st_mode) ||
            status.st_size < SDL_MT32_MIN_ROM_SIZE || status.st_size > SDL_MT32_MAX_ROM_SIZE)
            continue;
        SdlIdentifyMt32File(path, files, capacity, &count);
    }
    closedir(handle);
#endif
    return count;
}

static const SdlMt32RomFile *SdlFindMt32RomFile(const SdlMt32RomFile *files,
                                                 int count, const char *id)
{
    int index;

    for (index = 0; index < count; index++) {
        if (strcmp(files[index].id, id) == 0)
            return &files[index];
    }
    return 0;
}

/* A machine is playable with its full control and PCM ROMs, or with both
 * halves where the ROMs were dumped from two chips. */
static int SdlMatchMt32Machine(const char *machine, const SdlMt32RomFile *files,
                               int count, SdlMt32RomSet *set)
{
    const char *ids[16];
    const SdlMt32RomFile *halves[4];
    size_t idCount;
    size_t index;

    memset(set, 0, sizeof(*set));
    memset(halves, 0, sizeof(halves));
    SDL_strlcpy(set->machine, machine, sizeof(set->machine));
    idCount = mt32emu_get_rom_ids(ids, SDL_arraysize(ids), machine);
    if (idCount > SDL_arraysize(ids))
        idCount = SDL_arraysize(ids);
    for (index = 0; index < idCount; index++) {
        const SdlMt32RomFile *file = SdlFindMt32RomFile(files, count, ids[index]);
        int control = strncmp(ids[index], "ctrl_", 5) == 0;

        if (file == 0)
            continue;
        if (control && SdlMt32EndsWith(ids[index], "_a"))
            halves[0] = file;
        else if (control && SdlMt32EndsWith(ids[index], "_b"))
            halves[1] = file;
        else if (!control && SdlMt32EndsWith(ids[index], "_l"))
            halves[2] = file;
        else if (!control && SdlMt32EndsWith(ids[index], "_h"))
            halves[3] = file;
        else if (control)
            set->control[0] = file;
        else
            set->pcm[0] = file;
    }
    if (set->control[0] == 0 && halves[0] != 0 && halves[1] != 0) {
        set->control[0] = halves[0];
        set->control[1] = halves[1];
    }
    if (set->pcm[0] == 0 && halves[2] != 0 && halves[3] != 0) {
        set->pcm[0] = halves[2];
        set->pcm[1] = halves[3];
    }
    return set->control[0] != 0 && set->pcm[0] != 0;
}

/* The preferred machine the directory has complete ROMs for, or the
 * requested one. */
static int SdlFindMt32RomSet(const char *directory, const char *machine,
                             SdlMt32RomFile *files, SdlMt32RomSet *set,
                             char *error, size_t errorSize)
{
    int count;
    size_t index;

    if (directory == 0 || directory[0] == '\0') {
        SDL_strlcpy(error, "No MT-32 ROM folder is set.", errorSize);
        return 0;
    }
    count = SdlScanMt32Roms(directory, files, SDL_MT32_MAX_ROM_FILES);
    if (count < 0) {
        SDL_snprintf(error, errorSize, "Cannot open the ROM folder %s.", directory);
        return 0;
    }
    if (count == 0) {
        SDL_snprintf(error, errorSize, "No MT-32 or CM-32L ROMs in %s.", directory);
        return 0;
    }
    if (machine != 0 && machine[0] != '\0') {
        if (SdlMatchMt32Machine(machine, files, count, set))
            return 1;
        SDL_snprintf(error, errorSize,
                     "%s needs a control ROM and a PCM ROM that %s lacks.",
                     machine, directory);
        return 0;
    }
    for (index = 0; index < SDL_arraysize(g_apszSdlMt32MachinePreference); index++) {
        if (SdlMatchMt32Machine(g_apszSdlMt32MachinePreference[index], files,
                                count, set))
            return 1;
    }
    SDL_snprintf(error, errorSize,
                 "%s has no matching pair of a control ROM and a PCM ROM.",
                 directory);
    return 0;
}

int SdlDescribeMt32Roms(const char *directory, const char *machine,
                        char *text, size_t textSize)
{
    SdlMt32RomFile *files;
    SdlMt32RomSet set;
    int found;

    files = (SdlMt32RomFile *)SDL_calloc(SDL_MT32_MAX_ROM_FILES, sizeof(*files));
    if (files == 0) {
        SDL_strlcpy(text, "Out of memory.", textSize);
        return 0;
    }
    found = SdlFindMt32RomSet(directory, machine, files, &set, text, textSize);
    if (found)
        SDL_snprintf(text, textSize, "%s%s and %s%s (%s)",
                     set.control[0]->description,
                     set.control[1] != 0 ? ", two halves," : "",
                     set.pcm[0]->description,
                     set.pcm[1] != 0 ? ", two halves" : "", set.machine);
    SDL_free(files);
    return found;
}

/* Adds a ROM, or its two halves merged, from the files' contents; the
 * synth keeps a full ROM's data. */
static int SdlAddMt32Rom(SdlMt32Synth *synth, const SdlMt32RomFile *const files[2],
                         unsigned int slot)
{
    unsigned char *data[2];
    size_t size[2];
    mt32emu_return_code result;

    data[0] = SdlReadMt32File(files[0]->path, &size[0]);
    data[1] = files[1] != 0 ? SdlReadMt32File(files[1]->path, &size[1]) : 0;
    if (data[0] == 0 || (files[1] != 0 && data[1] == 0)) {
        result = MT32EMU_RC_FILE_NOT_FOUND;
    } else if (files[1] != 0) {
        result = mt32emu_merge_and_add_rom_data(synth->context, data[0], size[0], 0,
                                                data[1], size[1], 0);
    } else {
        result = mt32emu_add_rom_data(synth->context, data[0], size[0], 0);
        if (result > 0) {
            synth->romData[slot] = data[0];
            data[0] = 0;
        }
    }
    SDL_free(data[0]);
    SDL_free(data[1]);
    return result > 0;
}

/* ------------------------------------------------------------------------
 * Messages and rendering */

/* Renders the frames before the one whose message is next, so that the
 * synth plays every message at the frame the game sent it. */
static void SdlRenderMt32Until(SdlMt32Synth *synth, unsigned int frame)
{
    short block[2 * SDL_MT32_RENDER_BLOCK];
    unsigned int count;
    unsigned int index;
    int sample;

    while (synth->renderedFrames < frame) {
        count = frame - synth->renderedFrames;
        if (count > SDL_MT32_RENDER_BLOCK)
            count = SDL_MT32_RENDER_BLOCK;
        if (synth->addToOutput == 0) {
            mt32emu_render_bit16s(synth->context,
                                  synth->output + synth->renderedFrames * 2, count);
        } else {
            mt32emu_render_bit16s(synth->context, block, count);
            for (index = 0; index < count * 2; index++) {
                sample = synth->output[synth->renderedFrames * 2 + index] + block[index];
                synth->output[synth->renderedFrames * 2 + index] = (short)
                    (sample < -32768 ? -32768 : sample > 32767 ? 32767 : sample);
            }
        }
        synth->renderedFrames += count;
    }
}

static void SdlSendMt32(SdlMt32Synth *synth, unsigned int status,
                        unsigned int data1, unsigned int data2)
{
    if (synth->output != 0)
        SdlRenderMt32Until(synth, synth->frame);
    mt32emu_play_msg(synth->context, (status & 0xff) | ((data1 & 0x7f) << 8) |
                     ((data2 & 0x7f) << 16));
}

/* The driver's entry points, for channels 1-16. */
static void SdlMt32Note(SdlMt32Synth *synth, unsigned int channel,
                        unsigned int note, unsigned int velocity)
{
    SdlSendMt32(synth, 0x90 | ((channel - 1) & 0x0f), note, velocity);
}

void SdlMt32Controller(SdlMt32Synth *synth, unsigned int channel,
                       unsigned int controller, unsigned int value)
{
    if (synth != 0)
        SdlSendMt32(synth, 0xb0 | ((channel - 1) & 0x0f), controller, value);
}

static void SdlMt32Program(SdlMt32Synth *synth, unsigned int channel,
                           unsigned int program)
{
    SdlSendMt32(synth, 0xc0 | ((channel - 1) & 0x0f), program, 0);
}

static void SdlMt32PitchBend(SdlMt32Synth *synth, unsigned int channel,
                             unsigned int low, unsigned int high)
{
    SdlSendMt32(synth, 0xe0 | ((channel - 1) & 0x0f), low, high);
}

/* STRAX.DRV 0000:0161: Roland DT1 messages to the MT-32, at most 256 data
 * bytes each. */
static void SdlMt32SendDataSet(mt32emu_context context, unsigned long address,
                               const unsigned char *data, size_t size)
{
    unsigned char message[8 + 256 + 2];

    while (size != 0) {
        size_t chunk = size > 256 ? 256 : size;
        unsigned int sum;
        size_t index;

        message[0] = 0xf0;
        message[1] = 0x41;
        message[2] = 0x10;
        message[3] = 0x16;
        message[4] = 0x12;
        message[5] = (address >> 14) & 0x7f;
        message[6] = (address >> 7) & 0x7f;
        message[7] = address & 0x7f;
        sum = message[5] + message[6] + message[7];
        for (index = 0; index < chunk; index++) {
            message[8 + index] = data[index];
            sum += data[index];
        }
        message[8 + chunk] = (unsigned char)(-sum & 0x7f);
        message[9 + chunk] = 0xf7;
        mt32emu_play_sysex(context, message, (mt32emu_bit32u)(chunk + 10));
        mt32emu_flush_midi_queue(context);
        data += chunk;
        size -= chunk;
        address += chunk;
    }
}

/* A 21-bit address from its three 7-bit bytes, as the driver writes them. */
static unsigned long SdlMt32Address(unsigned int high, unsigned int middle,
                                    unsigned int low)
{
    return ((unsigned long)high << 14) | ((unsigned long)middle << 7) | low;
}

/* STRAX.DRV's bank loader (0000:02c5): reset the MT-32, show the name, then
 * install each WINGLDR.TIM timbre (246 bytes and its program + 1). */
static void SdlInitializeMt32Driver(mt32emu_context context,
                                    const unsigned char *timbres,
                                    size_t timbreSize)
{
    static const unsigned char reset[1] = {0};
    static const char display[] = "Origin Sound System!";
    unsigned int count;
    unsigned int slot;

    SdlMt32SendDataSet(context, SdlMt32Address(0x7f, 0x00, 0x7f), reset, sizeof(reset));
    SdlMt32SendDataSet(context, SdlMt32Address(0x20, 0x00, 0x00),
                       (const unsigned char *)display, sizeof(display) - 1);
    if (timbres == 0 || timbreSize == 0)
        return;
    count = timbres[0];
    if ((size_t)count * 247 + 1 > timbreSize)
        count = (unsigned int)((timbreSize - 1) / 247);
    for (slot = 0; slot < count; slot++) {
        const unsigned char *record = timbres + 1 + (size_t)slot * 247;
        /* The patch at 0000:003d: memory timbre, no key shift, no fine
         * tuning, a 24-semitone bend range, reverb off. */
        unsigned char patch[8] = {2, 0, 0x18, 0x32, 0x18, 0, 0, 0};
        unsigned int timbre = slot & 0x3f;
        unsigned int program = (record[246] - 1U) & 0x7f;

        patch[1] = (unsigned char)timbre;
        SdlMt32SendDataSet(context, SdlMt32Address(0x08, timbre << 1, 0x00), record, 246);
        SdlMt32SendDataSet(context,
                           SdlMt32Address(0x05, program >> 4, (program << 3) & 0x7f),
                           patch, sizeof(patch));
    }
}

/* ------------------------------------------------------------------------
 * Channels shared by the music and the sound effects (module 035) */

/* LinkMidiChannel (035:0007): appends the channel to the list that ends at
 * the given head. */
static void SdlLinkMt32Channel(SdlMt32Synth *synth, unsigned int channel,
                               unsigned int head)
{
    unsigned int node;

    for (node = 0; node < SDL_MT32_LIST_NODES; node++) {
        if (synth->links[node] == head) {
            synth->links[node] = (unsigned char)channel;
            synth->links[channel] = (unsigned char)head;
            break;
        }
    }
}

static void SdlUnlinkMt32Channel(SdlMt32Synth *synth, unsigned int channel)
{
    unsigned int node;

    for (node = 0; node < SDL_MT32_LIST_NODES; node++) {
        if (synth->links[node] == channel) {
            synth->links[node] = synth->links[channel];
            synth->links[channel] = (unsigned char)channel;
            break;
        }
    }
}

/* ClaimEffectChannel (035:0038): the channel becomes the newest busy one,
 * at the given priority, and an effect still on it loses it. */
static void SdlClaimMt32Channel(SdlMt32Synth *synth, unsigned int channel, int priority)
{
    unsigned int index;

    if (channel < 2 || channel > 9)
        return;
    SdlUnlinkMt32Channel(synth, channel);
    SdlLinkMt32Channel(synth, channel, SDL_MT32_BUSY_LIST);
    synth->effectPriority[channel] = (signed char)priority;
    if (synth->voicesReady) {
        for (index = 0; index < SDL_MT32_VOICE_COUNT; index++) {
            if (synth->voices[index].channel == channel)
                synth->voices[index].channel = 0;
        }
    }
}

/* FreeEffectChannel (035:00d1). */
static void SdlFreeMt32Channel(SdlMt32Synth *synth, unsigned int channel)
{
    if (channel < 2 || channel > 9)
        return;
    SdlMt32Controller(synth, channel, 0x7b, 0);
    synth->owner[channel] = SDL_MT32_UNOWNED;
    SdlUnlinkMt32Channel(synth, channel);
    SdlLinkMt32Channel(synth, channel, SDL_MT32_FREE_LIST);
}

/* AllocEffectChannel (035:0133): a free channel, else the oldest busy one
 * whose priority is not higher. Returns 0 when there is none. */
static unsigned int SdlAllocateMt32Channel(SdlMt32Synth *synth, int priority)
{
    unsigned int channel;
    unsigned int node;

    channel = 0;
    if (synth->links[SDL_MT32_FREE_LIST] != SDL_MT32_FREE_LIST) {
        channel = synth->links[SDL_MT32_FREE_LIST];
    } else {
        for (node = synth->links[SDL_MT32_BUSY_LIST]; node != SDL_MT32_BUSY_LIST;
             node = synth->links[node]) {
            if (synth->effectPriority[node] <= priority) {
                channel = node;
                break;
            }
        }
    }
    SdlClaimMt32Channel(synth, channel, priority);
    return channel;
}

/* The music's program, volume and pan on a channel it takes (back). */
static void SdlRestoreMt32Music(SdlMt32Synth *synth, unsigned int channel)
{
    SdlMt32Controller(synth, channel, 0x7b, 0);
    if (synth->sentProgram[channel] != synth->musicProgram[channel]) {
        if (channel != 10)
            SdlMt32Program(synth, channel, synth->musicProgram[channel]);
        synth->sentProgram[channel] = synth->musicProgram[channel];
    }
    SdlMt32Controller(synth, channel, 7,
                      synth->musicVolume[channel] * synth->volume >> 8);
    SdlMt32Controller(synth, channel, 10, synth->musicPan[channel]);
}

/* ReturnChannelToMusic (035:07f4). */
static void SdlReturnMt32Channel(SdlMt32Synth *synth, unsigned int channel)
{
    if (synth->owner[channel] != SDL_MT32_EFFECT)
        return;
    SdlFreeMt32Channel(synth, channel);
    SdlMt32Controller(synth, channel, 0x7b, 0);
    SdlMt32Program(synth, channel, synth->musicProgram[channel]);
    synth->sentProgram[channel] = synth->musicProgram[channel];
    SdlMt32Controller(synth, channel, 7,
                      synth->musicVolume[channel] * synth->volume >> 8);
    SdlMt32Controller(synth, channel, 10, synth->musicPan[channel]);
    synth->owner[channel] = SDL_MT32_MUSIC;
}

/* A channel event of the music, as MidiSequencer (035:0bda) plays it: the
 * music takes a free channel, or one whose effect has a negative priority,
 * back with its own settings, and sends nothing while an effect has it. */
void SdlMt32MusicEvent(SdlMt32Synth *synth, unsigned int status,
                       unsigned int data1, unsigned int data2)
{
    unsigned int channel;
    int send;

    if (synth == 0)
        return;
    channel = (status & 0x0f) + 1;
    if (synth->owner[channel] == SDL_MT32_UNOWNED ||
        (channel < SDL_MT32_LIST_NODES && synth->effectPriority[channel] < 0)) {
        SdlRestoreMt32Music(synth, channel);
        synth->owner[channel] = SDL_MT32_MUSIC;
    }
    if (synth->owner[channel] == SDL_MT32_MUSIC)
        SdlClaimMt32Channel(synth, channel, 0);
    /* The Kilrathi Saga's music switch sets the volume instead of
     * WC.EXE's EnableMusic, so the music always sends. */
    send = synth->owner[channel] == SDL_MT32_MUSIC;
    switch (status & 0xf0) {
    case 0x80:
        if (send)
            SdlMt32Note(synth, channel, data1, 0);
        break;
    case 0x90:
        if (send)
            SdlMt32Note(synth, channel, data1, data2);
        break;
    case 0xb0:
        if (send)
            SdlMt32Controller(synth, channel, data1,
                              data1 == 7 ? data2 * synth->volume >> 8 : data2);
        if (data1 == 7)
            synth->musicVolume[channel] = (unsigned char)data2;
        if (data1 == 10)
            synth->musicPan[channel] = (unsigned char)data2;
        break;
    case 0xc0:
        if (send) {
            SdlMt32Program(synth, channel, data1);
            synth->sentProgram[channel] = (unsigned char)data1;
        }
        synth->musicProgram[channel] = (unsigned char)data1;
        break;
    case 0xe0:
        if (send)
            SdlMt32PitchBend(synth, channel, data1, data2);
        break;
    }
}

/* RewindSong (035:13fa) resets the controllers of channels 2-10. */
void SdlMt32RewindSong(SdlMt32Synth *synth)
{
    unsigned int channel;

    if (synth == 0)
        return;
    for (channel = 2; channel <= 10; channel++)
        SdlMt32Controller(synth, channel, 0x79, 0);
}

/* StopSong (035:1265): the notes of the music's channels stop. */
void SdlMt32StopSong(SdlMt32Synth *synth)
{
    unsigned int channel;

    if (synth == 0)
        return;
    for (channel = 1; channel < SDL_MT32_CHANNEL_COUNT; channel++) {
        if (synth->owner[channel] == SDL_MT32_MUSIC)
            SdlMt32Controller(synth, channel, 0x7b, 0);
    }
}

/* SetMusicVolumeSetting (035:1b35) for the music, 256 being full; the
 * effects scale their own volume. */
void SdlSetMt32Volumes(SdlMt32Synth *synth, int musicVolume, int effectsVolume)
{
    unsigned int channel;

    if (synth == 0)
        return;
    synth->effectsVolume = effectsVolume;
    if (musicVolume == synth->volume)
        return;
    synth->volume = musicVolume;
    for (channel = 2; channel <= 10; channel++) {
        if (synth->owner[channel] == SDL_MT32_MUSIC)
            SdlMt32Controller(synth, channel, 7,
                              synth->musicVolume[channel] * synth->volume >> 8);
    }
}

/* ------------------------------------------------------------------------
 * Sound effects (035:017b-091f) */

/* StartEffectStep (035:017b): a step with no program ends the effect. A
 * stereo effect plays on two channels panned hard left and right, at
 * volumes taken from its pan. */
static void SdlStartMt32Step(SdlMt32Synth *synth, SdlMt32Voice *voice)
{
    const unsigned char *step;
    unsigned int leftVolume;
    unsigned int rightVolume;
    unsigned int leftPan;
    unsigned int rightPan;

    step = voice->step;
    if (step[1] == 0 || voice->channel == 0) {
        SdlFreeMt32Channel(synth, voice->channel);
        if (voice->flags & 1)
            SdlFreeMt32Channel(synth, voice->rightChannel);
        if (!(voice->flags & 2))
            voice->channel = 0;
        return;
    }
    if (voice->flags & 1) {
        leftPan = 0;
        rightPan = 0x7f;
        leftVolume = (voice->pan < 0x20 ? 0x7f : 0x9f - voice->pan) * voice->volume >> 7;
        rightVolume = (voice->pan > 0x5f ? 0x7f : voice->pan + 0x20) * voice->volume >> 7;
    } else {
        leftVolume = rightVolume = voice->volume;
        leftPan = rightPan = voice->pan;
    }
    if (synth->owner[voice->channel] != SDL_MT32_EFFECT) {
        SdlMt32Controller(synth, voice->channel, 0x7b, 0);
        synth->owner[voice->channel] = SDL_MT32_EFFECT;
    }
    SdlMt32Program(synth, voice->channel, step[1] - 1U);
    synth->sentProgram[voice->channel] = (unsigned char)(step[1] - 1U);
    SdlMt32Controller(synth, voice->channel, 7, leftVolume);
    SdlMt32Controller(synth, voice->channel, 10, leftPan);
    SdlMt32Note(synth, voice->channel, step[2], step[3]);
    if ((voice->flags & 1) && voice->rightChannel != 0) {
        if (synth->owner[voice->rightChannel] != SDL_MT32_EFFECT) {
            SdlMt32Controller(synth, voice->rightChannel, 0x7b, 0);
            synth->owner[voice->rightChannel] = SDL_MT32_EFFECT;
        }
        SdlMt32Program(synth, voice->rightChannel, step[1] - 1U);
        synth->sentProgram[voice->rightChannel] = (unsigned char)(step[1] - 1U);
        SdlMt32Controller(synth, voice->rightChannel, 7, rightVolume);
        SdlMt32Controller(synth, voice->rightChannel, 10, rightPan);
        SdlMt32Note(synth, voice->rightChannel, step[2], step[3]);
    }
    voice->note = step[2];
    voice->ticks = step[4] | (step[5] << 8);
}

/* UpdateEffectVoices (035:0396), at 60 Hz: a step whose time is up glides
 * or repeats its note, moves on to the next step, or ends the effect. */
static void SdlServiceMt32Effects(SdlMt32Synth *synth)
{
    const unsigned char *step;
    SdlMt32Voice *voice;
    unsigned int index;
    int changed;

    if (!synth->voicesReady)
        return;
    for (index = 0; index < SDL_MT32_VOICE_COUNT; index++) {
        voice = &synth->voices[index];
        if (voice->step == 0 || voice->channel == 0)
            continue;
        step = voice->step;
        if ((step[0] & 4) != 0 || --voice->ticks != 0)
            continue;
        changed = 0;
        SdlMt32Note(synth, voice->channel, voice->note, 0);
        if (voice->flags & 1)
            SdlMt32Note(synth, voice->rightChannel, voice->note, 0);
        if (step[0] & 2) {
            if (step[6] > voice->note) {
                voice->note++;
                changed = 1;
            } else if (step[6] < voice->note) {
                voice->note--;
                changed = 1;
            } else {
                voice->note = step[2];
            }
        }
        if (step[0] & 8)
            changed = 1;
        if (changed) {
            /* As WC.EXE, only the left channel of a stereo effect sounds
             * again. */
            SdlMt32Note(synth, voice->channel, voice->note, step[3]);
            voice->ticks = step[4] | (step[5] << 8);
        } else if ((step[0] & 1) &&
                   step + 8 < synth->records + (size_t)synth->recordCount * 8) {
            voice->step = step + 8;
            SdlStartMt32Step(synth, voice);
        } else {
            SdlFreeMt32Channel(synth, voice->channel);
            if (voice->flags & 1)
                SdlFreeMt32Channel(synth, voice->rightChannel);
            if (!(voice->flags & 2))
                voice->channel = 0;
        }
    }
}

/* StartSoundEffect (035:04f3) for game sound 1-36 at the given volume
 * (0-127) and pan. An effect with the same id stops and gives its channels
 * to the new one; otherwise the effect claims a channel at its priority. */
int SdlStartMt32Effect(SdlMt32Synth *synth, unsigned int soundNumber, int volume,
                       int pan, int id, int priority)
{
    SdlMt32Voice *voice;
    unsigned int leftChannel;
    unsigned int rightChannel;
    int freeVoice;
    int index;

    if (synth == 0 || soundNumber == 0 || soundNumber > synth->recordCount)
        return 0;
    volume = volume * synth->effectsVolume >> 8;
    volume = volume < 0 ? 0 : volume > 127 ? 127 : volume;
    pan = pan < 0 ? 0 : pan > 127 ? 127 : pan;
    leftChannel = 0;
    rightChannel = 0;
    freeVoice = -1;
    for (index = 0; index < SDL_MT32_VOICE_COUNT; index++) {
        voice = &synth->voices[index];
        if (voice->channel != 0 && voice->id == id) {
            leftChannel = voice->channel;
            rightChannel = voice->rightChannel;
            SdlMt32Note(synth, leftChannel, voice->note, 0);
            if (voice->flags & 1)
                SdlMt32Note(synth, rightChannel, voice->note, 0);
            voice->channel = 0;
            break;
        }
        if (voice->channel == 0)
            freeVoice = index;
    }
    if (leftChannel == 0) {
        if (freeVoice < 0)
            return 0;
        index = freeVoice;
        leftChannel = SdlAllocateMt32Channel(synth, priority);
    }
    if (leftChannel == 0)
        return 0;
    voice = &synth->voices[index];
    voice->flags = 0;
    voice->channel = (unsigned char)leftChannel;
    voice->rightChannel = (unsigned char)rightChannel;
    voice->volume = (unsigned char)volume;
    voice->pan = (unsigned char)pan;
    voice->id = id;
    voice->step = synth->records + (size_t)(soundNumber - 1) * 8;
    SdlStartMt32Step(synth, voice);
    return 1;
}

/* StopAllSoundEffects (035:091f): every channel an effect has goes back to
 * the music. */
void SdlStopMt32Effects(SdlMt32Synth *synth)
{
    unsigned int index;
    unsigned int channel;

    if (synth == 0 || !synth->voicesReady)
        return;
    synth->voicesReady = 0;
    for (index = 0; index < SDL_MT32_VOICE_COUNT; index++)
        synth->voices[index].channel = 0;
    for (channel = 2; channel < 10; channel++)
        SdlReturnMt32Channel(synth, channel);
    synth->voicesReady = 1;
}

/* ------------------------------------------------------------------------
 * The synth */

static mt32emu_report_handler_version MT32EMU_C_CALL SdlMt32ReportVersion(
    mt32emu_report_handler_i handler)
{
    (void)handler;
    return MT32EMU_REPORT_HANDLER_VERSION_0;
}

/* libmt32emu's notes, such as rhythm keys that the MT-32 has no sound for,
 * and the text the driver shows on the MT-32's display stay off the
 * console. */
static void MT32EMU_C_CALL SdlMt32IgnoreDebug(void *data, const char *format,
                                             va_list arguments)
{
    (void)data;
    (void)format;
    (void)arguments;
}

static void MT32EMU_C_CALL SdlMt32IgnoreDisplay(void *data, const char *message)
{
    (void)data;
    (void)message;
}

static const mt32emu_report_handler_i_v0 g_stSdlMt32Report = {
    SdlMt32ReportVersion, SdlMt32IgnoreDebug, 0, 0, SdlMt32IgnoreDisplay,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

SdlMt32Synth *SdlCreateMt32Synth(unsigned int sampleRate,
                                 const unsigned char *timbres,
                                 size_t timbreSize)
{
    /* 035's initial channel list: 2-9 free, nothing busy. */
    static const unsigned char links[SDL_MT32_LIST_NODES] = {
        0, 1, 3, 4, 5, 6, 7, 8, 9, 11, 10, 2, 12
    };
    SdlMt32RomFile *files;
    mt32emu_report_handler_i handler;
    SdlMt32RomSet set;
    SdlMt32Synth *synth;
    char error[PATH_MAX + 128];
    mt32emu_return_code result;
    unsigned int channel;

    files = (SdlMt32RomFile *)SDL_calloc(SDL_MT32_MAX_ROM_FILES, sizeof(*files));
    synth = (SdlMt32Synth *)SDL_calloc(1, sizeof(*synth));
    if (files == 0 || synth == 0) {
        SDL_free(files);
        SDL_free(synth);
        SDL_OutOfMemory();
        return 0;
    }
    if (!SdlFindMt32RomSet(g_szSdlMt32RomDirectory, g_szSdlMt32Machine, files,
                           &set, error, sizeof(error))) {
        SDL_SetError("%s", error);
        SDL_free(files);
        SDL_free(synth);
        return 0;
    }
    handler.v0 = &g_stSdlMt32Report;
    synth->context = mt32emu_create_context(handler, 0);
    if (synth->context == 0 || !SdlAddMt32Rom(synth, set.control, 0) ||
        !SdlAddMt32Rom(synth, set.pcm, 1)) {
        SDL_SetError("libmt32emu rejected the %s ROMs.", set.machine);
        SDL_free(files);
        SdlDestroyMt32Synth(synth);
        return 0;
    }
    SDL_free(files);
    mt32emu_set_stereo_output_samplerate(synth->context, sampleRate);
    mt32emu_set_samplerate_conversion_quality(synth->context, MT32EMU_SRCQ_GOOD);
    result = mt32emu_open_synth(synth->context);
    if (result != MT32EMU_RC_OK) {
        SDL_SetError("libmt32emu could not start the %s (error %d).", set.machine,
                     (int)result);
        SdlDestroyMt32Synth(synth);
        return 0;
    }
    /* At the hardware's own level the MT-32 is up to three times as loud
     * as the AdLib music and clips the briefing and title scores; half of
     * it keeps the port's music, effects and speech balance. */
    mt32emu_set_output_gain(synth->context, SDL_MT32_OUTPUT_GAIN);
    mt32emu_set_reverb_output_gain(synth->context, SDL_MT32_OUTPUT_GAIN);
    SdlInitializeMt32Driver(synth->context, timbres, timbreSize);
    synth->sampleRate = sampleRate;
    synth->records = SdlGetOriginFxSoundRecords(&synth->recordCount);
    memcpy(synth->links, links, sizeof(links));
    for (channel = 0; channel < SDL_MT32_CHANNEL_COUNT; channel++) {
        synth->sentProgram[channel] = 0xff;
        synth->musicVolume[channel] = 0x7f;
        synth->musicPan[channel] = 0x40;
    }
    synth->volume = 256;
    synth->effectsVolume = 256;
    synth->voicesReady = 1;
    return synth;
}

void SdlDestroyMt32Synth(SdlMt32Synth *synth)
{
    if (synth == 0)
        return;
    if (synth->context != 0) {
        mt32emu_close_synth(synth->context);
        mt32emu_free_context(synth->context);
    }
    SDL_free(synth->romData[0]);
    SDL_free(synth->romData[1]);
    SDL_free(synth);
}

/* Mixes the MT-32 into the samples, or replaces them. Each frame first
 * lets the caller step its song, then runs the 60 Hz effects service, as
 * WC.EXE's timer interrupt does; whatever they send plays at that frame. */
void SdlMixMt32(SdlMt32Synth *synth, short *samples, unsigned int frameCount,
                int addToOutput, void (*stepMusic)(void *context), void *context)
{
    unsigned int frame;

    if (synth == 0 || samples == 0)
        return;
    synth->output = samples;
    synth->renderedFrames = 0;
    synth->addToOutput = addToOutput;
    for (frame = 0; frame < frameCount; frame++) {
        synth->frame = frame;
        if (stepMusic != 0)
            stepMusic(context);
        synth->serviceAccumulator += SDL_MT32_SERVICE_RATE;
        if (synth->serviceAccumulator >= synth->sampleRate) {
            synth->serviceAccumulator -= synth->sampleRate;
            SdlServiceMt32Effects(synth);
        }
    }
    SdlRenderMt32Until(synth, frameCount);
    synth->output = 0;
}
