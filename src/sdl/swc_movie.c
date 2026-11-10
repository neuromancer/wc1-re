/* SDL2 replacement for CODE_12 MovieDisplay/LMovieTask presentation and
 * Sound Manager playback. The recovered LMov codec lives in src/swc/movie.c. */
#include "wc1sdl.h"
#include "swc.h"

#include <string.h>

static int SdlPlaySwcMovie(SDL_Window *window, SDL_Renderer *renderer,
                           const char *path, int dimOnFinish)
{
    SwcMovie movie = {0};
    SDL_Surface *indexed = NULL;
    SDL_Surface *rgba = NULL;
    SDL_Texture *texture = NULL;
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
    int focused;
    int paused = 0;
    int playing = 0;
    int skipMouse = 0;
    int changed = 1;
    int redraw = 1;
    int result = SWC_MOVIE_ERROR;
    int index;

    SdlPauseSwcSound(1);
    SdlClearSwcPcm();
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
        if (SdlQueueSwcPcm(&movie.audio, movie.sampleRate, AUDIO_U8) != 0)
            goto done;
        /* The mixer cursor excludes its current hardware buffer. */
        if (swcAudioDevice != 0)
            audioDuration += 512 * frequency / 48000;
        duration = SDL_max(duration, audioDuration);
    }
    focused = (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
    previous = SDL_GetPerformanceCounter();
    while (1) {
        current = SDL_GetPerformanceCounter();
        /* Cap a stall, such as the modal Volume Controls dialog, which
         * also holds the movie's audio. */
        if (playing)
            elapsed += SDL_min(current - previous, frequency / 4);
        previous = current;
        while (SDL_PollEvent(&event)) {
            if (SdlHandleSwcEvent(window, &event, 0))
                continue;
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
                case SDLK_BACKSPACE:
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
            SdlPauseSwcSound(!playing);
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
            SdlDrawSwcOptionNotice(renderer);
            SDL_RenderPresent(renderer);
            redraw = 0;
        }
        if (elapsed >= duration && !SdlSwcPcmPlaying() &&
            skipKey == SDL_SCANCODE_UNKNOWN && !skipMouse) {
            result = SWC_MOVIE_FINISHED;
            goto done;
        }
        SDL_Delay(8);
    }

done:
    /* CODE_15 movie_dim_screen +0x046c halves all palette entries after
     * a movie group. CODE_12 MovieDisplay +0x0738 does the same on skip,
     * leaving entries 0 and 255 unchanged. Both are immediate palette
     * changes; the next scene owns the subsequent palette. */
    if ((result == SWC_MOVIE_FINISHED && dimOnFinish) || result == SWC_MOVIE_SKIPPED) {
        for (index = result == SWC_MOVIE_SKIPPED ? 1 : 0;
             index < (result == SWC_MOVIE_SKIPPED ? 255 : 256); index++) {
            movie.colors[index].r >>= 1;
            movie.colors[index].g >>= 1;
            movie.colors[index].b >>= 1;
        }
        if (SDL_SetPaletteColors(indexed->format->palette, movie.colors, 0, 256) != 0 ||
            SDL_BlitSurface(indexed, NULL, rgba, NULL) != 0 ||
            SDL_UpdateTexture(texture, NULL, rgba->pixels, rgba->pitch) != 0 ||
            SDL_RenderCopy(renderer, texture, NULL, NULL) != 0)
            result = SWC_MOVIE_ERROR;
        else
            SDL_RenderPresent(renderer);
    }
    SDL_strlcpy(failure, SDL_GetError(), sizeof(failure));
    if (movie.damagedFrames != 0)
        fprintf(stderr, "SWC movie %s: %u damaged frame(s) shown as decoded\n",
                path, movie.damagedFrames);
    SdlClearSwcPcm();
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(rgba);
    SDL_FreeSurface(indexed);
    SwcMovieClose(&movie);
    if (result == SWC_MOVIE_ERROR)
        SDL_SetError("%s: %s", path, failure);
    return result;
}

/* CODE_12 SetupSWStuff +0x4ad2/+0x4ad6, followed by CODE_09 squadron
 * +0x5e -> CODE_07 IntroFilm +0x1442. BUC belongs to stranded_sequence,
 * not startup. MovieDisplay's music index is the high word of its flags. */
int SdlPlaySwcStartup(SDL_Window *window, SDL_Renderer *renderer,
                        const char *resourceFork, int replayIntro)
{
    static const char *const names[] = {
        "Lion", "INTRO00", "INTRO01", "INTRO02", "INTRO03", "INTRO04",
        "INTRO05", "INTRO06", "INTRO07", "INTRO08", "INTRO09", "INTRO10"
    };
    char paths[SDL_arraysize(names)][PATH_MAX];
    char relative[64];
    char path[PATH_MAX];
    SDL_RWops *reader;
    size_t index;
    int line;
    int cursorVisible;
    int result = SWC_MOVIE_FINISHED;

    /* Preserve the optional-movie contract for minimal flight installations.
     * Check the entire prologue, including its two live dialogue scenes. */
    for (index = replayIntro ? 1 : 0; index < SDL_arraysize(names); index++) {
        SDL_snprintf(relative, sizeof(relative), "Movies/%s.dcMov", names[index]);
        if (!SdlResolvePath(relative, paths[index], sizeof(paths[index])))
            return SDL_SetError("SWC movie path is too long: %s", relative);
        reader = SDL_RWFromFile(paths[index], "rb");
        if (reader == NULL)
            goto unavailable;
        SDL_RWclose(reader);
    }
    for (line = 1; line <= 11; line++) {
        if (line <= 2)
            SDL_snprintf(relative, sizeof(relative), "CMFs/IntroFilm%d.CMF", line);
        else
            SDL_snprintf(relative, sizeof(relative), "AIFF/intro.%02d", line);
        if (!SdlResolvePath(relative, path, sizeof(path)))
            return SDL_SetError("SWC intro path is too long: %s", relative);
        reader = SDL_RWFromFile(path, "rb");
        if (reader == NULL)
            goto unavailable;
        SDL_RWclose(reader);
    }
    cursorVisible = SDL_ShowCursor(SDL_QUERY);
    SDL_ShowCursor(SDL_DISABLE);
    if (!replayIntro)
        result = SdlShowSwcSplash(window, renderer, resourceFork);
    /* Dismissing ShowSplash advances to Lion. MacIntroFilm clears the skip
     * latch through CODE_15 movie_dim_screen before squadron's prologue. */
    if (result == SWC_MOVIE_QUIT || result == SWC_MOVIE_ERROR)
        goto done;
    /* CODE_09 squadron replays IntroFilm after Title_Sequence returns 3.
     * SetupSWStuff's splash and Lion run only once, before squadron. */
    for (index = replayIntro ? 1 : 0; index < SDL_arraysize(names); index++) {
        SdlPauseSwcSound(1);
        if (index == 1)
            SdlPlaySwcMusic("SUPRWNG1.MooV", 50);
        else if (index == 3)
            SdlPlaySwcMusic("SUPRWNG1.MooV", 30);
        else if (index == 4)
            SdlPlaySwcMusic("SUPRWNG2.MooV", 30);
        else if (index == 6) {
            SdlPlaySwcMusic("SUPRWNG3.MooV", 30);
            result = SdlPlaySwcIntroSpeech(window, renderer, resourceFork, 0);
            if (result != SWC_MOVIE_FINISHED)
                break;
            SdlPauseSwcSound(1);
            SdlPlaySwcMusic("SUPRWNG4.MooV", 50);
        } else if (index == 9) {
            SdlPlaySwcMusic("SUPRWNG4.MooV", 30);
            result = SdlPlaySwcIntroSpeech(window, renderer, resourceFork, 1);
            if (result != SWC_MOVIE_FINISHED)
                break;
            SdlPauseSwcSound(1);
            SdlPlaySwcMusic("SUPRWNG4.MooV", 50);
        }
        result = SdlPlaySwcMovie(window, renderer, paths[index],
            index == 0 || index + 1 == SDL_arraysize(names));
        if (index == 0 && result == SWC_MOVIE_SKIPPED)
            continue;
        if (result != SWC_MOVIE_FINISHED)
            break;
    }
done:
    SdlClearSwcPcm();
    SdlStopSwcMusic();
    SDL_ShowCursor(cursorVisible);
    return result;
unavailable:
    fprintf(stderr, "Skipping SWC startup: cannot read %s (%s)\n", relative, SDL_GetError());
    SDL_ClearError();
    return SWC_MOVIE_FINISHED;
}

/* Shared SDL presentation for CODE_03 opening_scene, scramble, landing and
 * funeral MovieDisplay calls. Palette fades remain pending. */
enum SwcMovieSequence {
    SWC_MOVIES_OPENING, SWC_MOVIES_LAUNCH, SWC_MOVIES_LANDING,
    SWC_MOVIES_FUNERAL_START, SWC_MOVIES_FUNERAL_FAREWELL, SWC_MOVIES_STRANDED
};

static int SdlPlaySwcMovieSequence(SDL_Window *window, SDL_Renderer *renderer,
                                   enum SwcMovieSequence sequence, int ship, int health)
{
    static const char *const openingNames[] = {"OPEN0", "OPEN1", "OPEN2", "OPEN3"};
    static const char *const launchNames[] = {
        "ARMOR", "HALL", "LAUNCH01", "LAUNCH02", "LAUNCH03",
        "LAUNCH04.%02d", "LAUNCH06.%02d", "LAUNCH07.%02d", "LAUNCH08.%02d"
    };
    const char *landingNames[] = {
        "LANDING1.%02d", "LANDING2.%02d", "LANDING1", "LANDING4.%02d",
        "LANDING5.%02d", "LANDING6.%02d", NULL
    };
    static const char *const funeralStartNames[] = {"FUNSERV0", "FUNSERVA"};
    static const char *const funeralFarewellNames[] = {"FUNSERVB"};
    static const char *const strandedNames[] = {"BUC"};
    const char *const *names;
    const char *title;
    size_t count;
    char paths[SDL_arraysize(launchNames)][PATH_MAX];
    char finalLanding[16];
    char clip[32];
    char relative[64];
    SDL_RWops *reader;
    size_t index;
    size_t available;
    int cursorVisible;
    int result = SWC_MOVIE_FINISHED;

    if (ship == 10)
        ship = 5; /* Retail GameFlow 0x010920b8: captured Salthi clips. */
    if (ship < 0 || ship > 5 || ship == 4)
        return SDL_SetError("Unsupported SWC cinematic ship %d", ship);
    /* CODE_03 landing +0x447e: damage variant = 2 - health*2/100. */
    SDL_snprintf(finalLanding, sizeof(finalLanding), "LANDING7.%02d",
                 2 - SDL_clamp(health, 0, 100) * 2 / 100);
    landingNames[6] = finalLanding;
    switch (sequence) {
    case SWC_MOVIES_OPENING:
        names = openingNames;
        count = SDL_arraysize(openingNames);
        title = "opening";
        break;
    case SWC_MOVIES_LAUNCH:
        names = launchNames;
        count = SDL_arraysize(launchNames);
        title = "launch";
        break;
    case SWC_MOVIES_LANDING:
        names = landingNames;
        count = SDL_arraysize(landingNames);
        title = "landing";
        break;
    case SWC_MOVIES_FUNERAL_START:
        names = funeralStartNames;
        count = SDL_arraysize(funeralStartNames);
        title = "funeral";
        break;
    case SWC_MOVIES_FUNERAL_FAREWELL:
        names = funeralFarewellNames;
        count = SDL_arraysize(funeralFarewellNames);
        title = "funeral";
        break;
    case SWC_MOVIES_STRANDED:
        names = strandedNames;
        count = SDL_arraysize(strandedNames);
        title = "stranded";
        break;
    default:
        return SDL_SetError("Unsupported SWC movie sequence");
    }
    /* CODE_12 MovieDisplay +0x0580/+0x08c6 returns quietly when a clip is
     * missing from the folder and from STR/128's disc path, so the sequence
     * goes on: the March 1995 data (IMG 25, MacFormat 4) lacks LANDING1.dcMov.
     * With no clip at all (a minimal flight installation) music and cursor
     * stay untouched. Malformed present movies still report a decoding error. */
    available = 0;
    for (index = 0; index < count; index++) {
        /* Retail CODE_03 scramble +0x46f8 / landing +0x455a format the
         * cockpit model, rather than always selecting the demo's Hornet. */
        SDL_snprintf(clip, sizeof(clip), names[index], ship);
        SDL_snprintf(relative, sizeof(relative), "Movies/%s.dcMov", clip);
        if (!SdlResolvePath(relative, paths[available], sizeof(paths[available])))
            return SDL_SetError("SWC movie path is too long: %s", relative);
        reader = SDL_RWFromFile(paths[available], "rb");
        if (reader == NULL) {
            fprintf(stderr, "Skipping SWC %s clip: cannot read %s (%s)\n",
                    title, relative, SDL_GetError());
            SDL_ClearError();
            continue;
        }
        SDL_RWclose(reader);
        available++;
    }
    if (available == 0)
        return SWC_MOVIE_FINISHED;
    cursorVisible = SDL_ShowCursor(SDL_QUERY);
    SDL_ShowCursor(SDL_DISABLE);
    SdlPauseSwcSound(1);
    if (sequence == SWC_MOVIES_OPENING)
        SdlPlaySwcMusic("DOCKING.MooV", 30);
    else if (sequence == SWC_MOVIES_LAUNCH)
        SdlPlaySwcMusic("ARMOR.MooV", 50);
    else if (sequence == SWC_MOVIES_LANDING)
        SdlPlaySwcMusic("LANDING.MooV", 50);
    else if (sequence == SWC_MOVIES_STRANDED)
        SdlPlaySwcMusic("LASTRITE.MooV", 50);
    /* Funeral music spans the movies and both room speeches. Its lifetime
     * belongs to SdlRunSwcCarrier, as in CODE_03 funeral_sequence. */
    for (index = 0; index < available; index++) {
        result = SdlPlaySwcMovie(window, renderer, paths[index],
            index + 1 == available && sequence != SWC_MOVIES_FUNERAL_START &&
            sequence != SWC_MOVIES_FUNERAL_FAREWELL);
        if (result != SWC_MOVIE_FINISHED)
            break;
    }
    SDL_ShowCursor(cursorVisible);
    if (sequence <= SWC_MOVIES_LANDING)
        SdlStopSwcMusic();
    return result;
}

/* Retail CODE_03 HandleMidGame 0x010351d2 and next_campaign_movie
 * 0x010344a4. Outcome is the next system for warps, otherwise series failure.
 * GameFlow handles 64/65 by moving to the next campaign, not by playing
 * HandleMidGame's otherwise-unreachable victory montage. */
int SdlPlaySwcCampaignTransition(SDL_Window *window, SDL_Renderer *renderer,
                                  int sequence, int outcome, int campaign)
{
    static const char *const sideWin[] = {"SIDE", "SIDE2", "SINODRATH", "SKK", NULL};
    static const char *const sideLose[] = {"SIDE", "SIDE2", "SIDEBGS", "SIDEFIN", "SIDEXP", NULL};
    static const char *const baseWin[] = {
        "MID3", "MID4", "MID5", "MIDFLY", "SPIN", "VGUN", "VSHOOT", "WMIDFLY2", NULL
    };
    static const char *const baseLose[] = {
        "MID1", "MID2", "MID3", "MID31", "MID4", "MID41", "MID5", "MLIFT", "MYFINAL", NULL
    };
    const char *warps[] = {"WARP", NULL, NULL};
    static const char *const fleet[] = {"LASTCAMP", "LFLEET", NULL};
    const char *const *names;
    const char *music;
    char warp[24];
    char relative[64];
    char path[PATH_MAX];
    int index;
    int cursor;
    int result = SWC_MOVIE_FINISHED;

    if (sequence == 64 || sequence == 65 || (sequence == 3 && outcome))
        return result;
    music = "FLYBY.MooV";
    switch (sequence) {
    case -1:
        warps[1] = "WARPCLAW";
        names = campaign == 1 ? fleet : warps;
        music = "VICTORY.MooV";
        break;
    case 0:
        names = outcome ? sideLose : sideWin;
        music = outcome ? "MID2LOSE.MooV" : "MID2WIN.MooV";
        break;
    case 1:
        names = outcome ? baseLose : baseWin;
        music = outcome ? "KILRATHI.MooV" : "KILLBASE.MooV";
        break;
    case 2:
    case 4:
        if (outcome < 1 || outcome > 13 || campaign < 0 || campaign > 2)
            return SDL_SetError("Invalid SWC system transition");
        SDL_snprintf(warp, sizeof(warp), "WARPIN.%02d", outcome + (campaign == 2 ? 20 : campaign == 1 ? 12 : 0));
        warps[1] = warp;
        names = warps;
        break;
    case 3:
        names = fleet;
        music = "SEARCH.MooV";
        break;
    default:
        return SDL_SetError("Unsupported SWC campaign sequence %d", sequence);
    }
    cursor = SDL_ShowCursor(SDL_QUERY);
    SDL_ShowCursor(SDL_DISABLE);
    SdlPlaySwcMusic(music, 30);
    for (index = 0; names[index] != NULL; index++) {
        /* The losing base sequence changes music twice within the montage. */
        if (sequence == 1 && outcome && index == 2)
            SdlPlaySwcMusic("FLYBY.MooV", 30);
        if (sequence == 1 && outcome && index == 5)
            SdlPlaySwcMusic("ATTACK.MooV", 30);
        SDL_snprintf(relative, sizeof(relative), "Movies/%s.dcMov", names[index]);
        if (!SdlResolvePath(relative, path, sizeof(path))) {
            result = SDL_SetError("Cannot resolve SWC campaign movie %s", relative);
            break;
        }
        result = SdlPlaySwcMovie(window, renderer, path, names[index + 1] == NULL);
        if (result != SWC_MOVIE_FINISHED)
            break;
    }
    SdlStopSwcMusic();
    SDL_ShowCursor(cursor);
    return result;
}

/* CODE_03 opening_scene +0x4374: four clips before a new campaign. */
int SdlPlaySwcOpening(SDL_Window *window, SDL_Renderer *renderer)
{
    return SdlPlaySwcMovieSequence(window, renderer, SWC_MOVIES_OPENING, 0, 100);
}

/* CODE_14 stranded_sequence +0x0a06: MovieDisplay("BUC", "LASTRITE.Moov"). */
int SdlPlaySwcStrandedMovie(SDL_Window *window, SDL_Renderer *renderer)
{
    return SdlPlaySwcMovieSequence(window, renderer, SWC_MOVIES_STRANDED, 0, 100);
}

/* Retail CODE_03 scramble +0x4684: nine clips, ship suffix 00..03. */
int SdlPlaySwcLaunch(SDL_Window *window, SDL_Renderer *renderer, int ship)
{
    return SdlPlaySwcMovieSequence(window, renderer, SWC_MOVIES_LAUNCH, ship, 100);
}

/* CODE_03 landing +0x447e: seven clips, ending selected by evaluate_damage. */
int SdlPlaySwcLanding(SDL_Window *window, SDL_Renderer *renderer, int ship, int health)
{
    return SdlPlaySwcMovieSequence(window, renderer, SWC_MOVIES_LANDING, ship, health);
}

/* CODE_03 funeral_sequence +0x4a08: two clips precede the eulogy,
 * FUNSERVB separates it from the farewell. */
int SdlPlaySwcFuneralMovies(SDL_Window *window, SDL_Renderer *renderer, int farewell)
{
    return SdlPlaySwcMovieSequence(window, renderer, farewell
        ? SWC_MOVIES_FUNERAL_FAREWELL : SWC_MOVIES_FUNERAL_START, 0, 100);
}
