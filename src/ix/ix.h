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

struct IxVoice {                /* 32 bytes */
    unsigned int   flags;       /* +0x00 */
    unsigned char *cursor;      /* +0x04  current read pointer */
    unsigned char *start;       /* +0x08  buffer start */
    unsigned char *end;         /* +0x0C  buffer end */
    short          field_10;    /* +0x10 */
    short          rate;        /* +0x12  (freq << 8) / 22050 */
    short          volume;      /* +0x14 */
    short          field_16;    /* +0x16 */
    short          field_18;    /* +0x18 */
    short          field_1a;    /* +0x1A */
    short          panAngle;    /* +0x1C  0..0x7F, wraps at +-0x40 */
    short          panPos;      /* +0x1E  0..0xFF */
};

#define IX_MIXER_BASE_RATE 22050    /* 0x5622, the divisor in set_frequency */

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
void ix_dspv_recalc_mix(int voice);                            /* 0x00446EBF */

/* --------------------------------------------------------------------------
 * DSP stream.  Array at 0x00598138, stride 0x38; count at 0x00598130.
 * Streams occupy voice slots AFTER the regular voices:
 * voice[voice_count + stream_index].
 * -------------------------------------------------------------------------- */
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
int  ix_mixer_init(void);                        /* 0x00445F60 */
void ix_mixer_service(void);                     /* 0x004463FC */
const char *ix_dsp_result_to_text(int hr);       /* 0x00444F97 */

int  ix_dsp_init(void);                          /* 0x00444910 */
void ix_dsp_set_voice_count(int n);              /* 0x00444C96 */
void ix_dsp_set_stream_count(int n);             /* 0x00444D2D */

int  ix_system_init(void);                       /* 0x00447200 */
void ix_system_set_voice_count(int n);           /* 0x0044748C */

int  ix_sound_start(void);                       /* 0x00447CD8 */
void ix_sound_release(void);                     /* 0x0044801E */
void ix_sound_stop(void);                        /* 0x004480CF */

int  ix_sample_load_aiff(void);                  /* 0x0044879C  FORM/AIFF */
int  ix_sample_load_wav(void);                   /* 0x00448C8B  RIFF/WAVE */
int  ix_sample_load_raw(void);                   /* 0x00449044 */

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
void ix_streamer_set_volume(unsigned short vol); /* 0x004435BE */
void ix_streamer_seek_chunk(unsigned int chunk); /* 0x00443666 */
void ix_streamer_service_audio(void);            /* 0x00443CC0 */
void *ix_streamer_open_file(unsigned char *name, unsigned char mode); /* 0x004437E3 */

void ix_thread_handle_file_chunk(void);          /* 0x00443DA6 */
void ix_thread_lock_stream_buffer(void);         /* 0x004445C9 */
void ix_thread_wait_event(void *h);              /* 0x0044489B */
void ix_thread_shutdown(void);                   /* 0x004449CF */
void ix_thread_signal_event(void);               /* 0x00444A62 */

#endif /* IX_H */
