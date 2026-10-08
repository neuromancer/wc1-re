/* SDL2 replacement for CODE_12 MovieDisplay/LMovieTask presentation and
 * Sound Manager playback. The recovered LMov codec lives in src/swc/movie.c. */
#include "wc1sdl.h"
#include "swc.h"

#include <string.h>

static int SdlPlaySwcMovie(SDL_Window *window, SDL_Renderer *renderer,
                           const char *path)
{
    SwcMovie movie = {0};
    SDL_Surface *indexed = NULL;
    SDL_Surface *rgba = NULL;
    SDL_Texture *texture = NULL;
    SDL_AudioDeviceID audio = 0;
    SDL_AudioSpec desired = {0};
    SDL_AudioSpec obtained = {0};
    SDL_Event event;
    SDL_Scancode skipKey = SDL_SCANCODE_UNKNOWN;
    Uint64 frequency = SDL_GetPerformanceFrequency();
    Uint64 previous;
    Uint64 current;
    Uint64 elapsed = 0;
    Uint64 duration;
    Uint64 audioDuration;
    Uint64 targetFrame;
    uint32_t frame = 1;
    char failure[256];
    int audioInitialized = 0;
    int focused;
    int paused = 0;
    int playing = 0;
    int skipMouse = 0;
    int changed = 1;
    int redraw = 1;
    int result = SWC_MOVIE_ERROR;

    if (SwcMovieOpen(path, &movie) != 0)
        goto done;
    if (SwcMovieNextFrame(&movie) != 1)
        goto done;
    indexed = SDL_CreateRGBSurfaceWithFormatFrom(movie.pixels, movie.width,
                  movie.height, 8, movie.width, SDL_PIXELFORMAT_INDEX8);
    rgba = SDL_CreateRGBSurfaceWithFormat(0, movie.width, movie.height, 32,
                                         SDL_PIXELFORMAT_ARGB8888);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING, movie.width, movie.height);
    if (indexed == NULL || rgba == NULL || texture == NULL)
        goto done;
    if (SDL_SetPaletteColors(indexed->format->palette, movie.colors, 0, 256) != 0)
        goto done;
    duration = (Uint64)movie.frameCount * frequency / movie.framesPerSecond;
    if (movie.audio.size != 0) {
        audioDuration = (Uint64)movie.audio.size * frequency / movie.sampleRate;
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) == 0) {
            audioInitialized = 1;
            desired.freq = movie.sampleRate;
            desired.format = AUDIO_U8;
            desired.channels = 1;
            desired.samples = 1024;
            audio = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);
        }
        if (audio == 0) {
            fprintf(stderr, "SWC movie audio unavailable; playing silently: %s\n",
                    SDL_GetError());
            SDL_ClearError();
        } else {
            if (SDL_QueueAudio(audio, movie.audio.data, (Uint32)movie.audio.size) != 0)
                goto done;
            /* Queued-byte counts exclude the device's current buffer. Give
             * the final buffer time to finish before closing the device. */
            audioDuration += (Uint64)obtained.samples * frequency / obtained.freq;
        }
        duration = SDL_max(duration, audioDuration);
    }
    focused = (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
    previous = SDL_GetPerformanceCounter();
    while (1) {
        current = SDL_GetPerformanceCounter();
        if (playing)
            elapsed += current - previous;
        previous = current;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_WINDOWEVENT &&
                 event.window.event == SDL_WINDOWEVENT_CLOSE)) {
                result = SWC_MOVIE_QUIT;
                goto done;
            }
            if (event.type == SDL_WINDOWEVENT) {
                redraw = 1;
                if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST ||
                    event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                    focused = 0;
                    skipKey = SDL_SCANCODE_UNKNOWN;
                    skipMouse = 0;
                } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED ||
                           event.window.event == SDL_WINDOWEVENT_RESTORED) {
                    focused = (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
                }
            }
            if (event.type == SDL_KEYDOWN && !event.key.repeat && focused) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                case SDLK_SPACE:
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    skipKey = event.key.keysym.scancode;
                    break;
                case SDLK_p:
                    paused = !paused;
                    break;
                default:
                    break;
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && focused &&
                event.button.button == SDL_BUTTON_LEFT)
                skipMouse = 1;
            /* Complete skips on release so Space/Enter/click cannot also
             * fire a weapon when the first flight frame starts. */
            if ((event.type == SDL_KEYUP && skipKey != SDL_SCANCODE_UNKNOWN &&
                 event.key.keysym.scancode == skipKey) ||
                (event.type == SDL_MOUSEBUTTONUP && skipMouse &&
                 event.button.button == SDL_BUTTON_LEFT)) {
                result = SWC_MOVIE_SKIPPED;
                goto done;
            }
        }
        if (playing != (focused && !paused) || redraw) {
            playing = focused && !paused;
            if (audio != 0)
                SDL_PauseAudioDevice(audio, !playing);
            SDL_SetWindowTitle(window, playing
                ? "Super Wing Commander - launch | Space/Enter/Esc/click: skip | P: pause"
                : "Super Wing Commander - launch (paused) | Space/Enter/Esc/click: skip | P: resume");
        }
        targetFrame = elapsed * movie.framesPerSecond / frequency + 1;
        while (frame < movie.frameCount && frame < targetFrame) {
            if (SwcMovieNextFrame(&movie) != 1)
                goto done;
            frame++;
            changed = 1;
        }
        if (changed) {
            if (SDL_BlitSurface(indexed, NULL, rgba, NULL) != 0 ||
                SDL_UpdateTexture(texture, NULL, rgba->pixels, rgba->pitch) != 0)
                goto done;
            redraw = 1;
            changed = 0;
        }
        if (redraw) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            if (SDL_RenderClear(renderer) != 0 ||
                SDL_RenderCopy(renderer, texture, NULL, NULL) != 0)
                goto done;
            SDL_RenderPresent(renderer);
            redraw = 0;
        }
        if (elapsed >= duration && (audio == 0 || SDL_GetQueuedAudioSize(audio) == 0) &&
            skipKey == SDL_SCANCODE_UNKNOWN && !skipMouse) {
            result = SWC_MOVIE_FINISHED;
            goto done;
        }
        SDL_Delay(8);
    }

done:
    SDL_strlcpy(failure, SDL_GetError(), sizeof(failure));
    if (audio != 0)
        SDL_CloseAudioDevice(audio);
    if (audioInitialized)
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(rgba);
    SDL_FreeSurface(indexed);
    SwcMovieClose(&movie);
    if (result == SWC_MOVIE_ERROR)
        SDL_SetError("%s: %s", path, failure);
    return result;
}

/* CODE_03 +0x45d0 scramble: original order and ship-zero filename suffixes.
 * Unlike WC1's sprite-based scramble, SWC calls MovieDisplay nine times.
 * The separate ARMOR.MooV music stream and palette fades remain pending. */
int SdlPlaySwcLaunch(SDL_Window *window, SDL_Renderer *renderer)
{
    static const char *const names[] = {
        "ARMOR", "HALL", "LAUNCH01", "LAUNCH02", "LAUNCH03",
        "LAUNCH04.00", "LAUNCH06.00", "LAUNCH07.00", "LAUNCH08.00"
    };
    char paths[SDL_arraysize(names)][PATH_MAX];
    char relative[64];
    SDL_RWops *reader;
    size_t index;
    int cursorVisible;
    int result = SWC_MOVIE_FINISHED;

    /* Movies are optional for an existing minimal flight installation. Check
     * the entire sequence before showing it; malformed present movies still
     * report a decoding error instead of silently starting partial playback. */
    for (index = 0; index < SDL_arraysize(names); index++) {
        SDL_snprintf(relative, sizeof(relative), "Movies/%s.dcMov", names[index]);
        if (!SdlResolvePath(relative, paths[index], sizeof(paths[index])))
            return SDL_SetError("SWC movie path is too long: %s", relative);
        reader = SDL_RWFromFile(paths[index], "rb");
        if (reader == NULL) {
            fprintf(stderr, "Skipping SWC launch: cannot read %s (%s)\n",
                    relative, SDL_GetError());
            SDL_ClearError();
            return SWC_MOVIE_FINISHED;
        }
        SDL_RWclose(reader);
    }
    cursorVisible = SDL_ShowCursor(SDL_QUERY);
    SDL_ShowCursor(SDL_DISABLE);
    for (index = 0; index < SDL_arraysize(names); index++) {
        result = SdlPlaySwcMovie(window, renderer, paths[index]);
        if (result != SWC_MOVIE_FINISHED)
            break;
    }
    SDL_ShowCursor(cursorVisible);
    SDL_SetWindowTitle(window, "Super Wing Commander - Enyo 1 (experimental flight)");
    return result;
}
