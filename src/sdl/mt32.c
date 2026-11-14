/* SDL2-only Roland MT-32 output for WC1's DOS music and sound effects,
 * through Munt's libmt32emu and the player's own control and PCM ROMs.
 *
 * STRAX.DRV is three OriginFX drivers in one Origin packet file, and the
 * configured device picks a section: 0 drives an MT-32 through an MPU-401,
 * 1 an AdLib and 2 the PC speaker. WINGLDR.TIM holds their instruments in
 * the same order. Section 0 adds nothing to the MIDI it is given; it only
 * initializes the MT-32, which SdlCreateMt32Synth reproduces.
 *
 * Without libmt32emu (SDL_PORT_MT32 undefined) every request fails and the
 * game keeps its AdLib music. */
#include "wc1sdl.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#ifdef SDL_PORT_MT32
#include <mt32emu.h>
#endif

#define SDL_MT32_MAX_ROM_FILES 64

static char g_szSdlMt32RomDirectory[PATH_MAX];
static char g_szSdlMt32Machine[32];

void SdlSetMt32Roms(const char *directory, const char *machine)
{
    SDL_strlcpy(g_szSdlMt32RomDirectory, directory != 0 ? directory : "",
                sizeof(g_szSdlMt32RomDirectory));
    SDL_strlcpy(g_szSdlMt32Machine, machine != 0 ? machine : "",
                sizeof(g_szSdlMt32Machine));
}

int SdlUsingMt32(void)
{
    return g_szSdlMt32RomDirectory[0] != '\0';
}

const char *SdlGetMt32RomDirectory(void)
{
    return g_szSdlMt32RomDirectory;
}

const char *SdlGetMt32Machine(void)
{
    return g_szSdlMt32Machine;
}

#ifndef SDL_PORT_MT32

int SdlMt32Supported(void)
{
    return 0;
}

int SdlDescribeMt32Roms(const char *directory, const char *machine,
                        char *text, size_t textSize)
{
    (void)directory;
    (void)machine;
    SDL_strlcpy(text, "This build has no MT-32 support.", textSize);
    return 0;
}

SdlMt32Synth *SdlCreateMt32Synth(unsigned int sampleRate,
                                 const unsigned char *timbres,
                                 size_t timbreSize)
{
    (void)sampleRate;
    (void)timbres;
    (void)timbreSize;
    SDL_SetError("This build has no MT-32 support.");
    return 0;
}

void SdlDestroyMt32Synth(SdlMt32Synth *synth)
{
    (void)synth;
}

void SdlMt32ShortMessage(SdlMt32Synth *synth, unsigned int status,
                         unsigned int data1, unsigned int data2)
{
    (void)synth;
    (void)status;
    (void)data1;
    (void)data2;
}

void SdlMt32AllNotesOff(SdlMt32Synth *synth)
{
    (void)synth;
}

void SdlMt32Render(SdlMt32Synth *synth, short *samples,
                   unsigned int frameCount)
{
    (void)synth;
    memset(samples, 0, (size_t)frameCount * 2 * sizeof(short));
}

#else

struct SdlMt32Synth {
    mt32emu_context context;
};

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

int SdlMt32Supported(void)
{
    return 1;
}

static int SdlMt32EndsWith(const char *text, const char *suffix)
{
    size_t textLength = strlen(text);
    size_t suffixLength = strlen(suffix);

    return textLength >= suffixLength &&
        strcmp(text + textLength - suffixLength, suffix) == 0;
}

/* Identifies the files of a directory that are MT-32 family ROMs. */
static int SdlScanMt32Roms(const char *directory, SdlMt32RomFile *files,
                           int capacity)
{
    DIR *handle;
    struct dirent *entry;
    struct stat status;
    mt32emu_rom_info info;
    int count;

    count = 0;
    handle = opendir(directory);
    if (handle == 0)
        return -1;
    while ((entry = readdir(handle)) != 0 && count < capacity) {
        SdlMt32RomFile *file = &files[count];

        if (entry->d_name[0] == '.')
            continue;
        if (SDL_snprintf(file->path, sizeof(file->path), "%s/%s", directory,
                         entry->d_name) >= (int)sizeof(file->path) ||
            stat(file->path, &status) != 0 || !S_ISREG(status.st_mode) ||
            status.st_size < 32768 || status.st_size > 1048576)
            continue;
        if (mt32emu_identify_rom_file(&info, file->path, 0) != MT32EMU_RC_OK)
            continue;
        if (info.control_rom_id != 0) {
            SDL_strlcpy(file->id, info.control_rom_id, sizeof(file->id));
            SDL_strlcpy(file->description, info.control_rom_description,
                        sizeof(file->description));
        } else if (info.pcm_rom_id != 0) {
            SDL_strlcpy(file->id, info.pcm_rom_id, sizeof(file->id));
            SDL_strlcpy(file->description, info.pcm_rom_description,
                        sizeof(file->description));
        } else {
            continue;
        }
        count++;
    }
    closedir(handle);
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
    static SdlMt32RomFile files[SDL_MT32_MAX_ROM_FILES];
    SdlMt32RomSet set;

    if (!SdlFindMt32RomSet(directory, machine, files, &set, text, textSize))
        return 0;
    SDL_snprintf(text, textSize, "%s%s and %s%s (%s)",
                 set.control[0]->description,
                 set.control[1] != 0 ? ", two halves," : "",
                 set.pcm[0]->description,
                 set.pcm[1] != 0 ? ", two halves" : "", set.machine);
    return 1;
}

static int SdlAddMt32Rom(mt32emu_context context, const SdlMt32RomFile *const files[2])
{
    mt32emu_return_code result;

    if (files[1] != 0)
        result = mt32emu_merge_and_add_rom_files(context, files[0]->path, files[1]->path);
    else
        result = mt32emu_add_rom_file(context, files[0]->path);
    return result > 0;
}

/* STRAX.DRV section 0, 0000:0161: a Roland DT1 message to the MT-32 (device
 * 0x10, model 0x16), at most 256 data bytes each, the 21-bit address
 * advancing in 7-bit bytes. */
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

/* Section 0's initialization, 0000:02c5-0381: reset every parameter, show
 * the system's name, then install each WINGLDR.TIM section 0 instrument. A
 * record is a 246-byte timbre and the program, plus one, that plays it. */
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
        /* 0000:0267 and the patch at 0000:003d: memory timbre, no key
         * shift, no fine tuning, a 24-semitone bend range, reverb off. */
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

SdlMt32Synth *SdlCreateMt32Synth(unsigned int sampleRate,
                                 const unsigned char *timbres,
                                 size_t timbreSize)
{
    static SdlMt32RomFile files[SDL_MT32_MAX_ROM_FILES];
    mt32emu_report_handler_i handler;
    SdlMt32RomSet set;
    SdlMt32Synth *synth;
    char error[PATH_MAX + 128];
    mt32emu_return_code result;

    if (!SdlUsingMt32()) {
        SDL_SetError("No MT-32 ROM folder was given.");
        return 0;
    }
    if (!SdlFindMt32RomSet(g_szSdlMt32RomDirectory, g_szSdlMt32Machine, files,
                           &set, error, sizeof(error))) {
        SDL_SetError("%s", error);
        return 0;
    }
    synth = (SdlMt32Synth *)SDL_calloc(1, sizeof(*synth));
    if (synth == 0) {
        SDL_OutOfMemory();
        return 0;
    }
    handler.v0 = 0;
    synth->context = mt32emu_create_context(handler, 0);
    if (synth->context == 0 || !SdlAddMt32Rom(synth->context, set.control) ||
        !SdlAddMt32Rom(synth->context, set.pcm)) {
        SDL_SetError("libmt32emu rejected the %s ROMs.", set.machine);
        SdlDestroyMt32Synth(synth);
        return 0;
    }
    mt32emu_set_stereo_output_samplerate(synth->context, sampleRate);
    mt32emu_set_samplerate_conversion_quality(synth->context, MT32EMU_SRCQ_GOOD);
    result = mt32emu_open_synth(synth->context);
    if (result != MT32EMU_RC_OK) {
        SDL_SetError("libmt32emu could not start the %s (error %d).", set.machine,
                     (int)result);
        SdlDestroyMt32Synth(synth);
        return 0;
    }
    SdlInitializeMt32Driver(synth->context, timbres, timbreSize);
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
    SDL_free(synth);
}

void SdlMt32ShortMessage(SdlMt32Synth *synth, unsigned int status,
                         unsigned int data1, unsigned int data2)
{
    if (synth == 0)
        return;
    mt32emu_play_msg(synth->context, (status & 0xff) | ((data1 & 0x7f) << 8) |
                     ((data2 & 0x7f) << 16));
}

/* Section 0, 0000:0400: All Notes Off on MIDI channels 2-10. */
void SdlMt32AllNotesOff(SdlMt32Synth *synth)
{
    unsigned int channel;

    for (channel = 1; channel <= 9; channel++)
        SdlMt32ShortMessage(synth, 0xb0 | channel, 0x7b, 0);
}

void SdlMt32Render(SdlMt32Synth *synth, short *samples,
                   unsigned int frameCount)
{
    if (synth == 0) {
        memset(samples, 0, (size_t)frameCount * 2 * sizeof(short));
        return;
    }
    mt32emu_render_bit16s(synth->context, samples, frameCount);
}

#endif
