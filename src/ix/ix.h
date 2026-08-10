/*
 *  ix -- Origin Systems audio library (C++), original dev tree D:\Rnd\Prj\ix.
 *
 *  Unlike the game core, this library's module layout is EXACTLY known: the
 *  shipped debug build still contains its assert __FILE__/__LINE__ anchors, and
 *  the linker laid each object file out contiguously:
 *
 *      streamer.cpp  0x00442750 - 0x00443DA5
 *      thread.cpp    0x00443DA6 - 0x0044490F
 *      dsp.cpp       0x00444910 - 0x004451B4
 *      dsps.cpp      0x004451B5 - 0x00445F5F
 *      mixer.cpp     0x00445F60 - 0x004467C4
 *      dspv.cpp      0x004467C5 - 0x004471FF
 *      system.cpp    0x00447200 - 0x00447CD7
 *      sound.cpp     0x00447CD8 - 0x0044879B
 *      sample.cpp    0x0044879C - 0x004492DF
 *
 *  Everything from 0x004492E0 upward is the MSVC 4.2 static debug CRT.
 */
#ifndef IX_H
#define IX_H

#include <windows.h>   /* CRITICAL_SECTION, used by the stream/voice state below */

/* --------------------------------------------------------------------------
 * Diagnostics.  Every ix module reports through this one variadic printer, and
 * the calls come in pairs:
 *      ix_log_printf("Fatal [%s - %d]:\n", __FILE__, __LINE__);
 *      ix_log_printf("human readable message");
 * Fatal paths then call exit(-1).  107 call sites; keep __FILE__/__LINE__
 * accurate, they are the module map.
 * -------------------------------------------------------------------------- */
void ix_log_printf(const char *fmt, ...);      /* 0x004426A0 */

/* --------------------------------------------------------------------------
 * DSP voice.  Array at 0x005981A8, stride 32 bytes; count at 0x00598600.
 * Field offsets are confirmed from the accessor bodies.
 * -------------------------------------------------------------------------- */
enum IxVoiceFlags {
    IX_VOICE_ACTIVE   = 0x02,
    IX_VOICE_FLAG4    = 0x04,   /* set elsewhere; meaning not yet established */
    IX_VOICE_16BIT    = 0x08,   /* set by ix_dspv_set_bits_per_sample(16) */
    IX_VOICE_STEREO   = 0x10    /* set by ix_dspv_set_channels(2) */
};

struct IxVoice {                  /* 32 bytes; offsets confirmed from the accessors */
    unsigned int   flags;         /* +0x00 */
    unsigned char *cursor;        /* +0x04  current read pointer */
    unsigned char *start;         /* +0x08  buffer start */
    unsigned char *end;           /* +0x0C  buffer end */
    short          field_10;      /* +0x10  not yet identified */
    short          rate;          /* +0x12  (freq << 8) / 22050 */
    unsigned short volume;        /* +0x14 */
    unsigned char  leftGainHi;    /* +0x16  high byte of leftGain */
    unsigned char  rightGainHi;   /* +0x17  high byte of rightGain */
    short          leftGain;      /* +0x18 */
    short          rightGain;     /* +0x1A */
    short          panAngle;      /* +0x1C  0..0x7F, wraps at +-0x40 */
    short          panPos;        /* +0x1E  0..0xFF, indexes the pan table */
};

/* Sample and sound layouts are fixed by system.cpp/sound.cpp field accesses. */
struct IxSample {
    unsigned int flags;           /* +0x00 */
    unsigned char *buffer;        /* +0x04 */
    unsigned int sampleCount;     /* +0x08 */
    unsigned int bitsPerSample;   /* +0x0C */
    unsigned int channels;        /* +0x10 */
    unsigned int bytesPerFrame;   /* +0x14 */
    unsigned int rateNumerator;   /* +0x18 */
    unsigned int rateDenominator; /* +0x1C */
    unsigned int frequency;       /* +0x20 */
    unsigned int loopStart;       /* +0x24 */
    unsigned int loopEnd;         /* +0x28 */
    IxSample *previous;           /* +0x2C */
    IxSample *next;               /* +0x30 */

    void ix_sample_construct(void);
    void ix_sample_destruct(void);
    int ix_sample_load_aiff(void *data, int bytes);
    int ix_sample_load_wav(void *data, int bytes);
    int ix_sample_load_raw(void *data, int bytes, int frequency,
                           int bitsPerSample, int channels);
    long ix_sample_convert_from_ieee_extended(unsigned char *bytes);
};

enum IxSampleFlags {
    IX_SAMPLE_READY   = 0x00000001,
    IX_SAMPLE_LOOPING = 0x00000002,
    IX_SAMPLE_LOADING = 0x80000000
};

enum IxSoundFlags {
    IX_SOUND_ALLOCATED       = 0x001,
    IX_SOUND_PLAYING         = 0x002,
    IX_SOUND_LOOPING         = 0x004,
    IX_SOUND_DELETE_ON_STOP  = 0x008,
    IX_SOUND_HAS_VOICE       = 0x010,
    IX_SOUND_VOLUME_DIRTY    = 0x020,
    IX_SOUND_FREQUENCY_DIRTY = 0x040,
    IX_SOUND_PAN_DIRTY       = 0x080,
    IX_SOUND_RELEASE_PENDING = 0x100
};

struct IxSound {
    unsigned int flags;           /* +0x00 */
    IxSample *sample;             /* +0x04 */
    int voice;                    /* +0x08 */
    unsigned int startTime;       /* +0x0C */
    unsigned int stopTime;        /* +0x10 */
    unsigned int priority;        /* +0x14 */
    int pitchOffset;              /* +0x18 */
    unsigned short volume;        /* +0x1C */
    unsigned short pan;           /* +0x1E */
    unsigned char basePriority;   /* +0x20 */
    IxSound *previous;            /* +0x24 */
    IxSound *next;                /* +0x28 */

    void ix_system_sound_init(IxSample *newSample,
                              unsigned char newBasePriority,
                              unsigned int initialVolume,
                              int newPitchOffset,
                              unsigned short newPan);
    void ix_system_sound_construct(IxSample *newSample,
                                   unsigned char newBasePriority);
    void ix_system_sound_construct_centred(IxSample *newSample);
    void ix_system_sound_set_volume(unsigned short newVolume);
    void ix_system_sound_set_pan(unsigned short newPan);
    void ix_system_sound_set_frequency(int newPitchOffset);
    void ix_system_sound_set_priority(unsigned char newBasePriority);
    void ix_sound_reprioritise(void);
    void ix_sound_set_delete_on_stop(int enabled);
    int ix_sound_is_playing(void);
};

extern unsigned int g_dwIxSystemFlags_00598608;
extern IxSound *g_pFreeSoundList_0059860c;
extern IxSample *g_pSampleList_00598610;
extern int g_nActiveVoices_00598614;
extern int g_nSystemVoiceCount_00598618;
extern IxSound *g_pActiveSoundList_0059861c;
extern IxSound *g_pWaitingSoundList_00598620;
extern void *(__cdecl *g_pIxMalloc_00471990)(unsigned int);
extern void (__cdecl *g_pIxFree_00471994)(void *);

/* Master volume and the stereo pan table (two shorts per position). */
extern unsigned short g_nMasterVolume_0047198c;
extern short          g_anPanTable_00597d28[];
extern CRITICAL_SECTION g_csMixer_005985e8;

#define IX_MIXER_BASE_RATE 22050    /* 0x5622, the divisor in set_frequency */

/* Globals keep their original address in the name (see AGENTS.md). */
extern IxVoice g_voices_005981a8[];
extern int     g_nVoiceCount_00598600;

/* The ix source tree lived on D: -- each assert site carries its own copy of
 * this literal (the build did not use /Gf, so identical strings are not
 * merged; writing it out per site is what matches). */
#define IX_DSPV_FILE "D:\\rnd\\prj\\ix\\win95\\dsp\\dspv.cpp"

void ix_dspv_set_active(int voice);              /* 0x004467C5  flags |= ACTIVE */
void ix_dspv_clear_active(int voice);            /* 0x00446829  flags &= ~ACTIVE */
void ix_dspv_clear_flag4(int voice);             /* 0x0044688E */
unsigned int ix_dspv_get_flags(int voice);       /* 0x004468F2 */
void ix_dspv_set_buffer(int voice, unsigned char *p, int len); /* 0x0044698F */
int  ix_dspv_get_position(int voice);            /* 0x00446A2B  cursor - start */
void ix_dspv_set_position(int voice, int off);   /* 0x00446AA0  cursor = start + off */
void ix_dspv_set_volume(int voice, short vol);   /* 0x00446B18 */
void ix_dspv_set_pan(int voice, unsigned int a); /* 0x00446B8C */
void ix_dspv_set_frequency(int voice, unsigned int hz);        /* 0x00446CB1 */
void ix_dspv_set_bits_per_sample(int voice, int bps);          /* 0x00446D2C  8 or 16 */
void ix_dspv_set_channels(int voice, int ch);                  /* 0x00446DF5  1 or 2 */
void ix_dspv_set_flag4(int voice, int on);                     /* 0x00446956 */
void ix_dspv_recalc_mix(int voice);                            /* 0x00446EBF */
void ix_dspv_mix(void *outputBuffer, unsigned int outputBytes); /* 0x00446F74 */

/* --------------------------------------------------------------------------
 * DSP stream.  Array at 0x00598138, stride 0x38; count at 0x00598130.
 * Streams occupy voice slots AFTER the regular voices:
 * voice[voice_count + stream_index].
 * -------------------------------------------------------------------------- */
enum IxStreamFlags {
    IX_STREAM_ALLOCATED = 0x01,
    IX_STREAM_PLAYING   = 0x02,
    IX_STREAM_LOCKED    = 0x04
};

struct IxStream {                       /* 0x38 bytes */
    unsigned int   flags;               /* +0x00 */
    unsigned char *buffer;              /* +0x04 */
    unsigned int   size;                /* +0x08 */
    unsigned int   writePos;            /* +0x0C */
    unsigned int   playPos;             /* +0x10 */
    unsigned int   pending;             /* +0x14 */
    unsigned char *lockPtr;             /* +0x18 */
    unsigned int   lockLen;             /* +0x1C */
    CRITICAL_SECTION cs;                /* +0x20, 24 bytes */
};

extern IxStream g_streams_00598138[];
extern int      g_nStreamCount_00598130;
extern int      g_nStreamsAllocated_00598134;

#define IX_DSPS_FILE "D:\\rnd\\prj\\ix\\win95\\dsp\\dsps.cpp"
void ix_dsps_alloc(int stream, int bytes);       /* 0x004451B5 */
void ix_dsps_free(int stream);                   /* 0x0044546B */
void ix_dsps_prepare(int stream);                /* 0x00445582 */
void ix_dsps_play(int stream);                   /* 0x004456D8 */
void ix_dsps_stop(int stream);                   /* 0x00445808 */
void ix_dsps_set_volume(int stream, unsigned int vol);  /* 0x00445906 */
unsigned int ix_dsps_get_flags(int stream);      /* 0x00445A0B */
int  ix_dsps_get_buffer_free(int stream);        /* 0x00445A6F */
void ix_dsps_lock(int stream);                   /* 0x00445CDB */
void ix_dsps_unlock(int stream);                 /* 0x00445E3C */

/* --------------------------------------------------------------------------
 * Mixer (DirectSound), DSP core, system, sound, sample.
 * -------------------------------------------------------------------------- */
DWORD WINAPI ix_mixer_thread_proc(void *parameter); /* 0x00445F60 */
int  ix_mixer_service(void);                     /* 0x004463FC */
const char *ix_dsp_result_to_text(int hr);       /* 0x00444F97 */

int  ix_dsp_init(void);                          /* 0x00444910 */
void ix_dsp_shutdown(void);                      /* 0x004449CF */
void ix_dsp_configure(int option, void *value);  /* 0x00444A62 */
unsigned int ix_dsp_get_tick(void);              /* 0x00444BFD */
unsigned int ix_dsp_get_flags(void);             /* 0x00444C12 */
void ix_dsp_set_config_bit1(int enabled);        /* 0x00444C27 */
void ix_dsp_set_config_bit3(int enabled);        /* 0x00444C54 */
int  ix_dsp_get_voice_count(void);               /* 0x00444C81 */
void ix_dsp_set_voice_count(int n);              /* 0x00444C96 */
int  ix_dsp_get_stream_count(void);              /* 0x00444D18 */
void ix_dsp_set_stream_count(int n);             /* 0x00444D2D */
unsigned short ix_dsp_get_master_volume(void);   /* 0x00444DAF */
void ix_dsp_set_master_volume(unsigned short vol); /* 0x00444DC5 */
void ix_dsp_build_pan_tables(void);              /* 0x00444E52 */
void *ix_dsp_alloc(unsigned int bytes);           /* 0x00445123 */
void *ix_dsp_copy(void *destination, const void *source,
                  unsigned int bytes);           /* 0x00445140 */
void ix_dsp_free(void *memory);                  /* 0x00445164 */

extern "C" int  ix_system_init(void);            /* 0x00447200 */
extern "C" void ix_system_service_sounds(void); /* 0x004472A7 */
extern "C" void ix_system_shutdown(void);        /* 0x004473F3 */
extern "C" void ix_system_configure(int option, void *value); /* 0x00447426 */
extern "C" unsigned short ix_system_get_master_volume(void); /* 0x00447446 */
extern "C" void ix_system_set_master_volume(unsigned short vol); /* 0x0044745B */
extern "C" int ix_system_get_voice_count(void);  /* 0x00447477 */
extern "C" void ix_system_set_voice_count(int n); /* 0x0044748C */
extern "C" IxSample *ix_system_new_sample(void); /* 0x004474E3 */
extern "C" void ix_system_delete_sample(IxSample *sample); /* 0x00447519 */
extern "C" void ix_system_delete_all_samples(void); /* 0x00447548 */
extern "C" IxSound *ix_system_new_sound(IxSample *sample); /* 0x0044758D */
void ix_system_delete_sound(IxSound *sound);     /* 0x004475C7 */
extern "C" void ix_system_delete_all_sounds(void); /* 0x00447610 */
int ix_system_release_voice(IxSound *sound);     /* 0x004476B9 */
void ix_system_assign_voice(IxSound *sound, int voice); /* 0x004476FA */
IxSound *ix_system_find_highest_waiting(IxSound *sound,
                                        unsigned int minimumPriority); /* 0x00447921 */
IxSound *ix_system_find_lowest_playing(IxSound *sound,
                                       unsigned int maximumPriority); /* 0x00447974 */
int ix_system_find_free_voice(void);             /* 0x004479C7 */

void __fastcall ix_sound_start(IxSound *sound);  /* 0x00447CD8 */
void __fastcall ix_sound_release(IxSound *sound); /* 0x0044801E */
void __fastcall ix_sound_unlink_from_free_list(IxSound *sound); /* 0x00447B7A */
void __fastcall ix_sound_stop(IxSound *sound);   /* 0x004480CF */

/* --------------------------------------------------------------------------
 * Streamer.  Global state word at 0x00597CD0; wake event at 0x00597CD4.
 *   bit2 0x004 playing      bit3 0x008 paused
 *   bit7 0x080 has audio    bit10 0x400 request flag (meaning TBD)
 * -------------------------------------------------------------------------- */
int  ix_streamer_init(void);                     /* 0x00442750 */
void ix_streamer_destroy(void);                  /* 0x0044286F */
void ix_streamer_set_dev_mode(int mode);         /* 0x0044293E */
int  ix_streamer_open_stream_file(const char *path);  /* 0x004429B6 */
void ix_streamer_close_stream_file(void);        /* 0x0044307A */
void ix_streamer_audio_play(void);               /* 0x004431F3 */
void ix_streamer_audio_stop(void);               /* 0x00443253 */
void ix_streamer_audio_pause(void);              /* 0x004432B6 */
void ix_streamer_audio_set_flag400(void);        /* 0x0044330F */
void ix_streamer_audio_branch_to_tag(char tag);  /* 0x0044342E  branching music */
extern "C" void ix_streamer_set_volume(unsigned short vol); /* 0x004435BE */
void ix_streamer_seek_chunk(unsigned int chunk); /* 0x00443666 */
void ix_streamer_service_audio(void);            /* 0x00443CC0 */
void *ix_streamer_open_file(unsigned char *name, unsigned char mode); /* 0x004437E3 */

void ix_thread_handle_file_chunk(void);          /* 0x00443DA6 */
void ix_thread_lock_stream_buffer(void);         /* 0x004445C9 */
void ix_thread_wait_event(void *h);              /* 0x0044489B */
extern "C" unsigned int g_dwStreamerState_00597cd0;
extern unsigned short g_nStreamerVolume_00470e84;
extern CRITICAL_SECTION g_csStreamer_00597ce0;

#endif /* IX_H */
