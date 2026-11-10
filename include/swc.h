#ifndef WC1_SWC_H
#define WC1_SWC_H

#ifndef SDL_PORT
#error SWC support is available only in the SDL2 build.
#endif

#include <SDL.h>
#include <stddef.h>
#include <stdint.h>

#define SWC_FRAME_WIDTH 320
#define SWC_FRAME_HEIGHT 240
#define SWC_CMF_COMPRESSED UINT32_C(0x10000000)

/* Host representations, not overlays of classic Mac memory or disk records.
 * All owned buffers use SDL_malloc/SDL_free. Functions return 0 on success
 * and -1 with SDL_GetError() on failure. Frame payloads borrow their set. */
typedef struct SwcBuffer {
    uint8_t *data;
    size_t size;
} SwcBuffer;

/* Owns a converted WC1 campaign packet, with one leading guard record;
 * publishes pMissionCampaignInfo on success. Restore that pointer on exit. */
int SdlLoadSwcCampaignData(const char *path, SwcBuffer *campaign);
/* SDL-created SWC saves; original Mac preferences are not modified. */
int SdlLoadSwcSaves(const char *missionPath, int retail);
int SdlLoadSwcGame(short slot);
extern int nSwcContinueSlot;
extern short nSwcBossmanDeath;

typedef struct SwcChunkInfo {
    char type[4];
    uint32_t id;
    uint32_t offset;
    uint32_t size;
    uint32_t flags;
} SwcChunkInfo;

typedef struct SwcCmf {
    uint8_t *data;
    size_t size;
    SwcChunkInfo *chunks;
    size_t chunkCount;
} SwcCmf;

/* Standard Sound Manager header, decoded to unsigned 8-bit mono samples. */
typedef struct SwcSound {
    SwcBuffer samples;
    uint32_t rate; /* 16.16 samples per second. */
} SwcSound;

typedef struct SwcFrame {
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t encoding;
    const uint8_t *pixels;
    size_t size;
} SwcFrame;

typedef struct SwcMovieRecord {
    uint32_t flags;
    uint32_t size;
    uint32_t offset;
} SwcMovieRecord;

typedef struct SwcMovie {
    SwcBuffer file;
    SwcBuffer audio;
    SwcMovieRecord *records;
    uint8_t *pixels;
    SDL_Color colors[256];
    uint32_t recordCount;
    uint32_t nextRecord;
    uint32_t frameCount;
    uint32_t framesPerSecond;
    uint32_t damagedFrames;
    int sampleRate;
    uint16_t width;
    uint16_t height;
} SwcMovie;

enum SwcMovieResult {
    SWC_MOVIE_ERROR = -1,
    SWC_MOVIE_FINISHED = 0,
    SWC_MOVIE_SKIPPED = 1,
    SWC_MOVIE_QUIT = 2
};

int SwcCMOpen(const char *path, SwcCmf *cmf);
void SwcCMClose(SwcCmf *cmf);
int SwcCMGetChunk(const SwcCmf *cmf, const char type[4], uint32_t id,
                  SwcBuffer *chunk);
int SwcDecompressRLE(const uint8_t *source, size_t sourceSize,
                     uint8_t *destination, size_t destinationSize);
int SwcGetFrameCount(const SwcBuffer *set, uint32_t *count);
int SwcGetFramePtr(const SwcBuffer *set, uint32_t index, SwcFrame *frame);
int SwcDecodeFrame(const SwcFrame *frame, uint8_t *pixels, size_t capacity);
int SwcReadPalette(const char *resourceFork, int16_t id, SDL_Color colors[256]);
/* Original startup PICT/129: one indexed 320x240 PackBits rectangle. */
int SwcReadSplash(const char *resourceFork, SwcBuffer *pixels, SDL_Color colors[256]);
/* Original Volume Controls art PICT/128: 66x118 one-bit pixels, 1 = black. */
int SwcReadSoundControl(const char *resourceFork, SwcBuffer *pixels);
int SwcReadResource(const char *resourceFork, const char type[4], int16_t id,
                    SwcBuffer *resource);
/* Loads a file's data fork, unwrapping MacBinary copies of forked files. */
int SwcLoadDataFork(const char *path, SwcBuffer *data);
/* Places copies of the demo keep the application's resource fork, relative
 * to the demo directory: raw .rsrc files, macOS's named fork, AppleDouble,
 * then the application itself as MacBinary (as served by discmaster). */
#define SWC_APPLICATION_FORKS 5
extern const char *const swcApplicationForks[SWC_APPLICATION_FORKS];
/* Discs also renamed the application (MacFormat 4 in Spain: "SuperWing
 * Commander Demo"). Searches the directory's top-level files, in any of the
 * forms above, for the demo's MoeT/0 signature and clut/251. */
int SwcFindApplicationFork(const char *directory, char *path, size_t capacity);
int SwcReadSound(const SwcBuffer *resource, SwcSound *sound);

/* QuickTime music events, timed in the source media's ticks. */
enum SwcMusicEventType {
    SWC_MUSIC_NOTE_ON, SWC_MUSIC_NOTE_OFF, SWC_MUSIC_CONTROL
};

typedef struct SwcMusicEvent {
    uint32_t tick;
    uint32_t order;
    uint16_t part;
    uint16_t value;
    uint8_t type;
    uint8_t key;
} SwcMusicEvent;

typedef struct SwcMusic {
    SwcMusicEvent *events;
    size_t count;
    uint32_t timeScale;
    uint32_t duration;
    uint16_t instruments[32];
} SwcMusic;

int SwcReadMusic(const char *path, SwcMusic *music);
int SdlInitSwcMusic(void);
void SdlFreeSwcMusic(void);
void SdlPlaySwcMusic(const char *name, int volume);
void SdlStopSwcMusic(void);
/* Called only by the shared SWC audio callback: interleaved stereo float. */
void SdlMixSwcMusic(float *samples, int frames);

/* One SDL device owns music, flight effects and foreground movie/speech PCM.
 * Main-thread music changes lock this device against its callback. */
extern SDL_AudioDeviceID swcAudioDevice;
int SdlInitSwcSound(const char *resourceFork);
void SdlFreeSwcSound(void);
void SdlPauseSwcSound(int paused);
void SdlStopSwcSounds(void);
int SdlQueueSwcPcm(const SwcBuffer *pcm, int rate, SDL_AudioFormat format);
void SdlClearSwcPcm(void);
int SdlSwcPcmPlaying(void);
Uint32 SdlSwcPcmTicks(void);
/* Play returns 1 for a playing sound, 0 for busy channels, -1 unavailable. */
int SdlPlaySwcSound(int sound, int volume);
void SdlStopSwcSound(int sound);
int SdlSwcSoundPlaying(int sound);
void SdlStopSwcAfterburnerSound(void);
/* Applies swcOptions' sound switch and volume to effects and PCM. */
void SdlApplySwcSoundOptions(void);
/* Freezes playing effects and PCM while the volume dialog is open. */
void SdlHoldSwcSound(int held);
/* Applies swcOptions' music switch and volume; turning music on restarts
 * the last requested track. */
void SdlRefreshSwcMusic(void);

/* CODE_01 DoOptionsMenu +0x2b30 state, saved by CODE_12 SavePreferences:
 * DATA/0 +0x4b7f music, +0x4b7e sound, +0x8c4c text (print_subtitle), and
 * +0xa7f6/+0xa7f2 music/sound volume percentages in DoAdjustVolumes' steps. */
typedef struct SwcOptions {
    int music;
    int sound;
    int text;
    int musicVolume;
    int soundVolume;
} SwcOptions;
extern SwcOptions swcOptions;
void SdlLoadSwcOptions(const char *resourceFork);
void SdlFreeSwcOptions(void);
/* Draws a recent toggle notice; callers use it just before presenting. */
void SdlDrawSwcOptionNotice(SDL_Renderer *renderer);

int SwcMovieOpen(const char *path, SwcMovie *movie);
void SwcMovieClose(SwcMovie *movie);
/* Advances the retained indexed image: 1 = frame (damaged ones count in
 * damagedFrames and keep what they drew), 0 = end. */
int SwcMovieNextFrame(SwcMovie *movie);
/* Splash, logo and prologue before the title menu; replayIntro omits splash/logo. */
int SdlPlaySwcStartup(SDL_Window *window, SDL_Renderer *renderer,
                        const char *resourceFork, int replayIntro);
int SdlShowSwcSplash(SDL_Window *window, SDL_Renderer *renderer, const char *resourceFork);
int SdlPlaySwcIntroSpeech(SDL_Window *window, SDL_Renderer *renderer,
                           const char *resourceFork, int afterAttack);
/* New-campaign opening before the carrier; returns a SwcMovieResult. */
int SdlPlaySwcOpening(SDL_Window *window, SDL_Renderer *renderer);
/* Retail HandleMidGame / next_campaign_movie; sequence -1 enters a campaign. */
int SdlPlaySwcCampaignTransition(SDL_Window *window, SDL_Renderer *renderer,
                                  int sequence, int outcome, int campaign);
/* Ship-specific launch/landing sequences; returns a SwcMovieResult. */
int SdlPlaySwcLaunch(SDL_Window *window, SDL_Renderer *renderer, int ship);
int SdlPlaySwcLanding(SDL_Window *window, SDL_Renderer *renderer, int ship, int health);
/* Original funeral clips, before the eulogy (0) or before the farewell (1). */
int SdlPlaySwcFuneralMovies(SDL_Window *window, SDL_Renderer *renderer, int farewell);
int SdlPlaySwcStrandedMovie(SDL_Window *window, SDL_Renderer *renderer);
int SdlShowSwcEnding(SDL_Window *window, SDL_Renderer *renderer,
                       const char *resourceFork, int stranded);

enum SwcCarrierVisit {
    SWC_CARRIER_PREFLIGHT,
    SWC_CARRIER_DEBRIEFING,
    SWC_CARRIER_PLAYER_FUNERAL,
    SWC_CARRIER_WINGMAN_FUNERAL,
    SWC_CARRIER_OFFICE,
    SWC_CARRIER_MEDAL,
    SWC_CARRIER_SIMULATOR_RESULTS
};

/* Carrier action, distinct from the movie results used by room scenes. */
enum { SWC_CARRIER_SIMULATOR = 3, SWC_CARRIER_TITLE = 4, SWC_CARRIER_LOADED = 5 };
/* Carrier/story visit; returns a SwcMovieResult or carrier action.
 * Medal visits use nCurrentConversationMedalIndex and award it on completion/skip. */
int SdlRunSwcCarrier(SDL_Window *window, SDL_Renderer *renderer,
                      const char *missionPath, const char *resourceFork, enum SwcCarrierVisit visit);
/* Demo speech is mono AIFF, 16-bit or 8-bit, at 11025 or 22254 Hz. Returns
 * signed 16-bit big-endian PCM and its sample rate. */
int SwcReadSpeech(const char *path, SwcBuffer *pcm, int *rate);
/* CODE_12 ParseMarks/GetMouthPos: 60 Hz mouth frames from AIFF MARK words. */
typedef struct SwcSpeechMark {
    uint32_t tick;
    int frame;
} SwcSpeechMark;
/* rate is the recording's, from SwcReadSpeech. */
int SwcReadSpeechMarks(const char *path, int rate, SwcSpeechMark **marks, size_t *count);

/* Shared SDL text presentation for the cockpit and carrier. */
SDL_Texture *SdlCreateSwcFont(SDL_Renderer *renderer);
int SdlDrawSwcText(SDL_Renderer *renderer, SDL_Texture *font, int x,
                    int baseline, SDL_Color ink, const char *text);
int SdlDrawSwcSubtitle(SDL_Renderer *renderer, SDL_Texture *font,
                        const char *text);

/* QuickDraw Helvetica: size 9 in the cockpit, 14 in carrier dialogue. */
typedef struct SwcTextFont {
    SDL_Texture *texture;
    SDL_Rect glyphs[95];
    int advances[95];
    int baseline;
    int bearings[95];
} SwcTextFont;
int SdlCreateSwcTextFont(SDL_Renderer *renderer, SwcTextFont *font, int size);
int SdlSwcTextWidth(const SwcTextFont *font, const char *text);
int SdlDrawSwcProportionalText(SDL_Renderer *renderer, const SwcTextFont *font,
                                int x, int baseline, SDL_Color ink, const char *text);
int SdlDrawSwcRoomSubtitle(SDL_Renderer *renderer, const SwcTextFont *font,
                            const char *text);

/* SDL presentation of the Mac demo's cockpit resources. */
typedef struct SwcRasterImage {
    uint8_t *pixels;
    int width;
    int height;
    SDL_Texture *texture; /* Optional OpenGL sprite cache, owned with pixels. */
} SwcRasterImage;
SDL_Renderer *SdlCreateSwcRenderer(SDL_Window *window);
int SdlLoadSwcRasterImage(const SwcBuffer *set, uint32_t index, SwcRasterImage *image);
void SdlFreeSwcRasterImage(SwcRasterImage *image);
/* Polygon corners are Q8 screen coordinates; the complete frame supplies UV. */
int SdlDrawSwcPolygon(SDL_Renderer *renderer, SDL_Texture *canvas,
                        SwcRasterImage *image, const SDL_Color colors[256],
                        const SDL_Point corners[4]);
int SdlDrawSwcRotatedImage(SDL_Renderer *renderer, SDL_Texture *canvas,
                            SwcRasterImage *image, const SDL_Color colors[256],
                            int x, int y, int angle, int scale, int flip, SDL_Rect *bounds);
SDL_Texture *SdlCreateSwcTexture(SDL_Renderer *renderer, const SwcBuffer *set,
                                 uint32_t index, const SDL_Color colors[256]);
int SdlInitSwcCockpit(SDL_Renderer *renderer, const SwcCmf *cockpit,
                       const SwcCmf *space, const SDL_Color colors[256], SDL_Texture *canvas,
                       int model);
void SdlUpdateSwcCockpit(void);
void SdlSwcLockOff(void);
int SdlDrawSwcCockpit(void);
int SdlDrawSwcCockpitDamage(void);
int SdlDrawSwcSpaceHud(const SDL_Rect objectBounds[10], int cockpitless);
int SdlDrawSwcSpaceGlare(void);
int SdlDrawSwcHud(void);
extern SDL_Point swcCockpitOffset; /* SWC DATA/A5+0x47d4/+0x47d8. */
extern SDL_Point swcCockpitViewCenter; /* Retail DATA/0 +0x07e2, selected row. */
void SdlSwcCockpitMessage(const char *text, unsigned char color, int flash, int voice);
void SdlFreeSwcCockpit(void);

/* Original five-page in-flight computer; flight is suspended until it closes. */
int SdlShowSwcComputer(SDL_Window *window, SDL_Renderer *renderer, const char *resourceFork);
extern const short aSwcComputerWeaponLayout[40][3]; /* DATA/0 +0x0030: x/y/Q8 scale. */

#endif
