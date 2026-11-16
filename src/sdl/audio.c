#include "wc1sdl.h"

static SDL_AudioDeviceID g_wAudioDevice;
static SdlAudioMixer g_pAudioMixer;
static CRITICAL_SECTION *g_pAudioCriticalSection;
static unsigned int *g_pAudioTick;
static Uint8 *voicePcm;
static Uint32 voiceBytes;
static Uint32 voicePosition;
static Uint32 voiceBufferStart;
static Uint32 voiceBufferTick;
/* Cumulative voiced 20 ms windows, measured before music/effects are mixed. */
static Uint32 *voiceMouthTimes;
static Uint32 voiceMouthCount;

/* WingLoader's Sega CD recordings are normalized to full scale: dialogue
 * averages about -9.5 dBFS RMS and radio messages -11.5, 12 to 18 dB above
 * the DOS music and 15 to 25 dB above its effects at their full default
 * settings, on the AdLib and the MT-32 alike. Mixed 8 dB lower, dialogue
 * sits a few dB above the briefing music, radio messages level with the
 * loudest flight music, and full-scale peaks no longer clip over it. */
#define SDL_VOICE_VOLUME 51 /* of SDL_MIX_MAXVOLUME's 128: -8 dB */

static void SdlAudioCallback(void *userData, Uint8 *stream, int bytes)
{
    (void)userData;
    EnterCriticalSection(g_pAudioCriticalSection);
    g_pAudioMixer(stream, (unsigned int)bytes);
    SdlMixOriginFxMusic(
        (short *)stream,
        (unsigned int)bytes / (sizeof(short) * 2U));
    if (voicePosition < voiceBytes) {
        Uint32 count = SDL_min((Uint32)bytes, voiceBytes - voicePosition);
        voiceBufferStart = voicePosition;
        voiceBufferTick = SDL_GetTicks();
        SDL_MixAudioFormat(stream, voicePcm + voicePosition, AUDIO_S16SYS,
                           count, SDL_VOICE_VOLUME);
        voicePosition += count;
    }
    if (g_pAudioTick != 0)
        *g_pAudioTick = *g_pAudioTick + 1;
    LeaveCriticalSection(g_pAudioCriticalSection);
}

int SdlStartAudio(SdlAudioMixer mixer,
                     CRITICAL_SECTION *criticalSection,
                     unsigned int *tick)
{
    SDL_AudioSpec desired;

    if (g_wAudioDevice != 0)
        return TRUE;
    if (mixer == 0 || criticalSection == 0 || criticalSection->mutex == 0)
        return FALSE;
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
        return FALSE;
    SDL_zero(desired);
    desired.freq = 22050;
    desired.format = AUDIO_S16SYS;
    desired.channels = 2;
    desired.samples = 1470;
    desired.callback = SdlAudioCallback;
    g_pAudioMixer = mixer;
    g_pAudioCriticalSection = criticalSection;
    g_pAudioTick = tick;
    g_wAudioDevice = SDL_OpenAudioDevice(0, 0, &desired, 0, 0);
    if (g_wAudioDevice == 0) {
        g_pAudioMixer = 0;
        g_pAudioCriticalSection = 0;
        g_pAudioTick = 0;
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return FALSE;
    }
    SDL_PauseAudioDevice(g_wAudioDevice, 0);
    return TRUE;
}

void SdlStopAudio(void)
{
    if (g_wAudioDevice != 0) {
        SDL_CloseAudioDevice(g_wAudioDevice);
        g_wAudioDevice = 0;
    }
    SdlStopVoice();
    g_pAudioMixer = 0;
    g_pAudioCriticalSection = 0;
    g_pAudioTick = 0;
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void SdlStopVoice(void)
{
    if (g_wAudioDevice)
        SDL_LockAudioDevice(g_wAudioDevice);
    SDL_free(voicePcm);
    SDL_free(voiceMouthTimes);
    voicePcm = 0;
    voiceMouthTimes = 0;
    voiceMouthCount = 0;
    voiceBytes = voicePosition = 0;
    voiceBufferStart = voiceBufferTick = 0;
    if (g_wAudioDevice)
        SDL_UnlockAudioDevice(g_wAudioDevice);
}

/* The callback cursor is ahead of playback by up to one output buffer. */
static Uint32 SdlVoicePlaybackPosition(void)
{
    Uint64 elapsed = (Uint64)(Uint32)(SDL_GetTicks() - voiceBufferTick) * 22050 / 1000 * 4;
    return voiceBufferStart + (Uint32)SDL_min(elapsed, voicePosition - voiceBufferStart);
}

int SdlVoicePlaying(void)
{
    int playing;
    if (!g_wAudioDevice)
        return 0;
    SDL_LockAudioDevice(g_wAudioDevice);
    playing = SdlVoicePlaybackPosition() < voiceBytes;
    SDL_UnlockAudioDevice(g_wAudioDevice);
    return playing;
}

short SdlVoiceMouthFrame(const short *commands, short fallback)
{
    Uint32 position, window, total = 0, target, i;
    short frame = 9; /* '$' in the original mouth script: closed/resting. */
    if (!g_wAudioDevice)
        return fallback;
    SDL_LockAudioDevice(g_wAudioDevice);
    if (!voiceBytes) {
        frame = fallback;
    } else {
        position = SdlVoicePlaybackPosition() / 4;
        window = position / 441;
        if (voicePosition && position < voiceBytes / 4 && window < voiceMouthCount &&
            voiceMouthTimes[window + 1] > voiceMouthTimes[window]) {
            /* Keep the original phoneme shapes, retimed over audible speech.
             * Silence comes from the recording, not the old PC pause lengths. */
            for (i = 0; i < 158 && commands[i] >= 0; i += 2)
                if (commands[i] < 9)
                    total += SDL_max(1, commands[i + 1]);
            target = (Uint64)(voiceMouthTimes[window] * 441 + position % 441) * total /
                     (voiceMouthTimes[voiceMouthCount] * 441);
            frame = 0;
            for (i = 0; i < 158 && commands[i] >= 0; i += 2) {
                if (commands[i] >= 9)
                    continue;
                frame = commands[i];
                if (target < (Uint32)SDL_max(1, commands[i + 1]))
                    break;
                target -= SDL_max(1, commands[i + 1]);
            }
        }
    }
    SDL_UnlockAudioDevice(g_wAudioDevice);
    return frame;
}

/* WingLoader's Sega CD recordings keep 8 bytes of the extracted files' header
 * in their sound data, as 89 CF 89 CF 7F xx 83 7F or 87 DB 87 DB 7F xx 83 7F.
 * Played, its CF and DB bytes are spikes of 62% of full scale, a click at the
 * start of every line (#33). Two 16-bit files carry the same bytes widened.
 * Returns how many bytes of sound data the header takes, or 0. */
static Uint32 SdlVoiceHeaderBytes(const Uint8 *data, Uint32 length, const SDL_AudioSpec *format)
{
    const int wide = format->format != AUDIO_U8;
    int value[8];
    int i;

    if (format->channels != 1 || length < (wide ? 32U : 16U))
        return 0;
    for (i = 0; i < 8; i++)
        value[i] = wide ? ((Sint16)(data[i * 2] | data[i * 2 + 1] << 8) + 128) >> 8 : data[i] - 128;
    if (value[0] != value[2] || value[1] != value[3] || (value[0] != 7 && value[0] != 9) ||
        value[1] < 79 || value[1] > 91 || value[7] != -1)
        return 0;
    return wide ? 16 : 8;
}

int SdlPlayVoiceWav(const char *path)
{
    SDL_AudioSpec format;
    SDL_AudioCVT conversion;
    SDL_RWops *file;
    Uint8 *source;
    Uint32 length, header;
    Uint32 *mouthTimes, windows, i, threshold, voiced = 0;
    Uint64 power = 0;
    Sint64 size;

    if (!g_wAudioDevice)
        return 0;
    file = SDL_RWFromFile(path, "rb");
    if (!file)
        return 0;
    size = SDL_RWsize(file);
    if (size < 44 || size > 32 * 1024 * 1024) {
        SDL_RWclose(file);
        return 0;
    }
    if (!SDL_LoadWAV_RW(file, 1, &format, &source, &length))
        return 0;
    if (!length || length > 32 * 1024 * 1024 ||
        format.freq < 4000 || format.freq > 96000 ||
        (format.channels != 1 && format.channels != 2) ||
        (format.format != AUDIO_U8 && format.format != AUDIO_S16LSB) ||
        SDL_BuildAudioCVT(&conversion, format.format, format.channels, format.freq,
                          AUDIO_S16SYS, 2, 22050) < 0 ||
        conversion.len_mult < 1 || length > (32 * 1024 * 1024) / conversion.len_mult) {
        SDL_FreeWAV(source);
        return 0;
    }
    header = SdlVoiceHeaderBytes(source, length, &format);
    conversion.len = (int)(length - header);
    conversion.buf = SDL_malloc(length * conversion.len_mult);
    if (!conversion.buf) {
        SDL_FreeWAV(source);
        return 0;
    }
    SDL_memcpy(conversion.buf, source + header, length - header);
    SDL_FreeWAV(source);
    if (SDL_ConvertAudio(&conversion) < 0) {
        SDL_free(conversion.buf);
        return 0;
    }
    {
        /* Fade each end in or out over about 3 ms: some recordings were cut
         * from longer ones and start or stop mid-waveform, a click too. */
        Sint16 *samples = (Sint16 *)conversion.buf;
        Uint32 frames = (Uint32)conversion.len_cvt / 4;
        Uint32 ramp = SDL_min(64U, frames / 2), k;

        for (k = 0; k < ramp; k++) {
            samples[k * 2] = (Sint16)(samples[k * 2] * (Sint32)k / (Sint32)ramp);
            samples[k * 2 + 1] = (Sint16)(samples[k * 2 + 1] * (Sint32)k / (Sint32)ramp);
            samples[(frames - 1 - k) * 2] = (Sint16)(samples[(frames - 1 - k) * 2] * (Sint32)k / (Sint32)ramp);
            samples[(frames - 1 - k) * 2 + 1] =
                (Sint16)(samples[(frames - 1 - k) * 2 + 1] * (Sint32)k / (Sint32)ramp);
        }
    }
    windows = ((Uint32)conversion.len_cvt / 4 + 440) / 441;
    mouthTimes = SDL_calloc(windows + 1, sizeof(*mouthTimes));
    if (!windows || !mouthTimes) {
        SDL_free(mouthTimes);
        SDL_free(conversion.buf);
        return 0;
    }
    for (i = 0; i < windows; i++) {
        Uint32 start = i * 882, end = SDL_min(start + 882, (Uint32)conversion.len_cvt / 2), j;
        const Sint16 *samples = (const Sint16 *)conversion.buf;
        Sint64 sum = 0;
        Uint64 squares = 0;
        for (j = start; j < end; j++) {
            sum += samples[j];
            squares += (Sint64)samples[j] * samples[j];
        }
        /* Remove DC offset so unsigned PCM silence does not animate a mouth. */
        mouthTimes[i] = (squares * (end - start) - sum * sum) /
                        ((Uint64)(end - start) * (end - start));
        power += mouthTimes[i];
    }
    threshold = SDL_max(128 * 128, power / windows / 64);
    for (i = 0; i < windows; i++) {
        Uint32 active = mouthTimes[i] > threshold;
        mouthTimes[i] = voiced;
        voiced += active;
    }
    mouthTimes[windows] = voiced;
    SDL_LockAudioDevice(g_wAudioDevice);
    SDL_free(voicePcm);
    SDL_free(voiceMouthTimes);
    voicePcm = conversion.buf;
    voiceMouthTimes = mouthTimes;
    voiceMouthCount = windows;
    voiceBytes = conversion.len_cvt;
    voicePosition = 0;
    voiceBufferStart = voiceBufferTick = 0;
    SDL_UnlockAudioDevice(g_wAudioDevice);
    return voiceBytes != 0;
}
