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

int SwcMovieOpen(const char *path, SwcMovie *movie);
void SwcMovieClose(SwcMovie *movie);
/* Advances the retained indexed image: 1 = frame, 0 = end, -1 = error. */
int SwcMovieNextFrame(SwcMovie *movie);
/* Hornet launch for the Enyo 1 host; returns a SwcMovieResult. */
int SdlPlaySwcLaunch(SDL_Window *window, SDL_Renderer *renderer);
int SdlPlaySwcLanding(SDL_Window *window, SDL_Renderer *renderer, int health);
/* Original funeral clips, before the eulogy (0) or before the farewell (1). */
int SdlPlaySwcFuneralMovies(SDL_Window *window, SDL_Renderer *renderer, int farewell);

enum SwcCarrierVisit {
    SWC_CARRIER_PREFLIGHT,
    SWC_CARRIER_DEBRIEFING,
    SWC_CARRIER_PLAYER_FUNERAL,
    SWC_CARRIER_WINGMAN_FUNERAL
};

/* First-mission carrier visit; returns a SwcMovieResult. */
int SdlRunSwcCarrier(SDL_Window *window, SDL_Renderer *renderer,
                      const char *missionPath, const char *resourceFork, enum SwcCarrierVisit visit);
/* Demo speech is AIFF, mono signed 16-bit big-endian PCM at 11025 Hz. */
int SwcReadSpeech(const char *path, SwcBuffer *pcm);

/* Shared SDL text presentation for the cockpit and carrier. */
SDL_Texture *SdlCreateSwcFont(SDL_Renderer *renderer);
int SdlDrawSwcText(SDL_Renderer *renderer, SDL_Texture *font, int x,
                    int baseline, SDL_Color ink, const char *text);
int SdlDrawSwcSubtitle(SDL_Renderer *renderer, SDL_Texture *font,
                        const char *text);

/* SDL presentation of the Mac demo's cockpit resources. */
SDL_Texture *SdlCreateSwcTexture(SDL_Renderer *renderer, const SwcBuffer *set,
                                 uint32_t index, const SDL_Color colors[256]);
int SdlInitSwcCockpit(SDL_Renderer *renderer, const SwcCmf *cockpit,
                       const SwcCmf *space, const SDL_Color colors[256]);
void SdlUpdateSwcCockpit(void);
int SdlDrawSwcCockpit(void);
int SdlDrawSwcCockpitDamage(void);
int SdlDrawSwcSpaceHud(const SDL_Rect *targetBounds);
int SdlDrawSwcHud(void);
void SdlFreeSwcCockpit(void);

#endif
