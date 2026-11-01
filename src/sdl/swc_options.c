/* SDL host for the SWC demo's Options menu. CODE_01 DoOptionsMenu +0x2b30
 * toggles music (item 11, Cmd-U), sound (item 12, Cmd-N) and text messages
 * (item 15) and opens Volume Controls (item 13: DoAdjustVolumes +0x4f80 and
 * DRAWVOLUMECONTROLS +0x5522). The Mac menu bar is not reproduced: Cmd (Ctrl
 * on other hosts) with U and N keeps the original keys, B and V are host
 * keys for the items that had none, and a short notice confirms toggles.
 * CODE_12 SavePreferences +0x3cdc keeps these settings; the host stores them
 * beside WC1's in the SDL preference directory. */
#include "wc1.h"
#include "swc.h"

#include <stdio.h>
#include <string.h>

/* LoadPreferences +0x38be..+0x39b0 starts with music and sound on at full
 * volume. A fresh Mac file also turns text off (+0x39e0); the host keeps it on. */
SwcOptions swcOptions = {1, 1, 1, 100, 100};

/* DoAdjustVolumes +0x52c4..+0x5430: a click selects the first step whose
 * limit exceeds its height in the control; DRAWVOLUMECONTROLS
 * +0x55d4..+0x5648 places the knob's top edge for each step. */
static const int swcVolumeSteps[8] = {100, 84, 70, 56, 42, 28, 14, 0};
static const int swcVolumeLimits[7] = {12, 24, 35, 47, 59, 71, 83};
static const int swcKnobTops[8] = {6, 18, 29, 41, 53, 65, 77, 89};

/* DLOG/132's 193x202 content and its DITL/132 items 1, 2, 3 and 6. */
enum { SWC_VOLUME_WIDTH = 193, SWC_VOLUME_HEIGHT = 202 };
static const SDL_Rect swcVolumeOk = {123, 163, 60, 20};
static const SDL_Rect swcVolumeCancel = {20, 162, 60, 20};
static const SDL_Rect swcVolumeSliders[2] = {{20, 39, 47, 105}, {122, 39, 47, 105}};

typedef struct SwcVolumeDialog {
    SDL_Renderer *renderer;
    SDL_Texture *art;
    SwcTextFont font;
    SDL_Point origin;
    SDL_Point mouse;
    int dragging;
    int pressed;
    int focus;
    int keyboard;
} SwcVolumeDialog;

static char swcOptionsResourceFork[PATH_MAX];
static const char *swcOptionNotice;
static Uint32 swcOptionNoticeEnd;
static SDL_Renderer *swcNoticeRenderer;
static SwcTextFont swcNoticeFont;
static int swcVolumeDialogOpen;

static int SwcVolumeStep(int volume)
{
    int step = 0;

    while (step < 7 && swcVolumeSteps[step] > volume)
        step++;
    return step;
}

static int SwcVolumeAt(int y)
{
    int step = 0;

    while (step < 7 && y >= swcVolumeLimits[step])
        step++;
    return swcVolumeSteps[step];
}

static char *SwcOptionsPath(void)
{
    char *directory = SDL_GetPrefPath("Origin Systems", "WC Kilrathi Saga");
    char *path;
    size_t size;

    if (directory == NULL)
        return NULL;
    size = strlen(directory) + sizeof("swc-demo.cfg");
    path = SDL_malloc(size);
    if (path != NULL)
        SDL_snprintf(path, size, "%sswc-demo.cfg", directory);
    SDL_free(directory);
    return path;
}

static void SwcSaveOptions(void)
{
    char *path = SwcOptionsPath();
    char contents[128];
    SDL_RWops *file;
    int length;

    if (path == NULL)
        return;
    length = SDL_snprintf(contents, sizeof(contents),
        "Music=%d\nSound=%d\nText=%d\nMusicVolume=%d\nSoundVolume=%d\n",
        swcOptions.music, swcOptions.sound, swcOptions.text,
        swcOptions.musicVolume, swcOptions.soundVolume);
    file = SDL_RWFromFile(path, "wb");
    if (file != NULL) {
        SDL_RWwrite(file, contents, 1, (size_t)length);
        SDL_RWclose(file);
    }
    SDL_free(path);
}

void SdlLoadSwcOptions(const char *resourceFork)
{
    char *path = SwcOptionsPath();
    char *contents = NULL;
    int music;
    int sound;
    int text;
    int musicVolume;
    int soundVolume;

    SDL_strlcpy(swcOptionsResourceFork, resourceFork, sizeof(swcOptionsResourceFork));
    if (path != NULL)
        contents = SDL_LoadFile(path, NULL);
    if (contents != NULL &&
        sscanf(contents, "Music=%d\nSound=%d\nText=%d\nMusicVolume=%d\nSoundVolume=%d",
               &music, &sound, &text, &musicVolume, &soundVolume) == 5) {
        swcOptions.music = music != 0;
        swcOptions.sound = sound != 0;
        swcOptions.text = text != 0;
        swcOptions.musicVolume = swcVolumeSteps[SwcVolumeStep(musicVolume)];
        swcOptions.soundVolume = swcVolumeSteps[SwcVolumeStep(soundVolume)];
    }
    SDL_free(contents);
    SDL_free(path);
    SDL_ClearError(); /* A first run has no settings file. */
    SdlApplySwcSoundOptions();
}

void SdlFreeSwcOptions(void)
{
    SDL_DestroyTexture(swcNoticeFont.texture);
    SDL_zero(swcNoticeFont);
    swcNoticeRenderer = NULL;
    swcOptionNotice = NULL;
}

static void SwcShowOptionNotice(const char *text)
{
    swcOptionNotice = text;
    swcOptionNoticeEnd = SDL_GetTicks() + 2000;
}

/* Host confirmation in place of the Mac menu item's changed label. It is
 * best effort: a missing font leaves the frame unchanged. */
void SdlDrawSwcOptionNotice(SDL_Renderer *renderer)
{
    static const SDL_Color ink = {255, 255, 255, 255};
    SDL_BlendMode blend;
    SDL_Rect box;

    if (swcOptionNotice == NULL)
        return;
    if (SDL_TICKS_PASSED(SDL_GetTicks(), swcOptionNoticeEnd)) {
        swcOptionNotice = NULL;
        return;
    }
    if (swcNoticeRenderer != renderer) {
        SDL_DestroyTexture(swcNoticeFont.texture);
        SDL_zero(swcNoticeFont);
        swcNoticeRenderer = renderer;
        SdlCreateSwcTextFont(renderer, &swcNoticeFont, 10);
    }
    if (swcNoticeFont.texture == NULL || SDL_GetRenderDrawBlendMode(renderer, &blend) != 0)
        return;
    box = (SDL_Rect){4, 4, SdlSwcTextWidth(&swcNoticeFont, swcOptionNotice) + 8, 16};
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 192);
    SDL_RenderFillRect(renderer, &box);
    SdlDrawSwcProportionalText(renderer, &swcNoticeFont, 8, 16, ink, swcOptionNotice);
    SDL_SetRenderDrawBlendMode(renderer, blend);
}

/* A QuickDraw-style rounded rectangle outline with small corners. */
static int SwcFrameRoundRect(SDL_Renderer *renderer, SDL_Rect rect)
{
    const int right = rect.x + rect.w - 1;
    const int bottom = rect.y + rect.h - 1;
    const SDL_Point corners[8] = {
        {rect.x + 1, rect.y + 2}, {rect.x + 2, rect.y + 1},
        {right - 1, rect.y + 2}, {right - 2, rect.y + 1},
        {rect.x + 1, bottom - 2}, {rect.x + 2, bottom - 1},
        {right - 1, bottom - 2}, {right - 2, bottom - 1}
    };

    if (SDL_RenderDrawLine(renderer, rect.x + 3, rect.y, right - 3, rect.y) != 0 ||
        SDL_RenderDrawLine(renderer, rect.x + 3, bottom, right - 3, bottom) != 0 ||
        SDL_RenderDrawLine(renderer, rect.x, rect.y + 3, rect.x, bottom - 3) != 0 ||
        SDL_RenderDrawLine(renderer, right, rect.y + 3, right, bottom - 3) != 0)
        return -1;
    return SDL_RenderDrawPoints(renderer, corners, 8);
}

static int SwcDrawVolumeButton(SwcVolumeDialog *dialog, SDL_Rect rect,
                               const char *label, int pressed, int isDefault)
{
    SDL_Renderer *renderer = dialog->renderer;
    SDL_Color ink = pressed ? (SDL_Color){255, 255, 255, 255} : (SDL_Color){0, 0, 0, 255};
    SDL_Rect fill;
    int ring;

    rect.x += dialog->origin.x;
    rect.y += dialog->origin.y;
    if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) != 0)
        return -1;
    /* A held button is inverted, as Control Manager tracking shows it. */
    if (pressed) {
        fill = (SDL_Rect){rect.x + 1, rect.y + 3, rect.w - 2, rect.h - 6};
        if (SDL_RenderFillRect(renderer, &fill) != 0)
            return -1;
        fill = (SDL_Rect){rect.x + 3, rect.y + 1, rect.w - 6, rect.h - 2};
        if (SDL_RenderFillRect(renderer, &fill) != 0)
            return -1;
    }
    if (SwcFrameRoundRect(renderer, rect) != 0)
        return -1;
    /* SetDialogDefaultItem(1) at DoAdjustVolumes +0x4fee outlines OK. */
    for (ring = isDefault ? 2 : 5; ring <= 4; ring++) {
        fill = (SDL_Rect){rect.x - ring, rect.y - ring, rect.w + ring * 2, rect.h + ring * 2};
        if (SwcFrameRoundRect(renderer, fill) != 0)
            return -1;
    }
    return SdlDrawSwcProportionalText(renderer, &dialog->font,
        rect.x + (rect.w - SdlSwcTextWidth(&dialog->font, label)) / 2,
        rect.y + 14, ink, label);
}

/* DRAWVOLUMECONTROLS composes each 47x105 control from PICT/128: its face,
 * the clean track from x=48 over the old knob (+0x55a2), then the knob from
 * (23,107) at the step's height or, while dragging, 5 pixels above the
 * pointer within 3..89 (+0x564a..+0x5672). */
static int SwcDrawVolumeSlider(SwcVolumeDialog *dialog, int slider, int volume)
{
    static const SDL_Rect face = {0, 0, 47, 105};
    static const SDL_Rect track = {48, 1, 18, 104};
    static const SDL_Rect knob = {23, 107, 23, 11};
    SDL_Rect item = swcVolumeSliders[slider];
    SDL_Rect target;
    int top = swcKnobTops[SwcVolumeStep(volume)];

    item.x += dialog->origin.x;
    item.y += dialog->origin.y;
    if (dialog->dragging == slider)
        top = SDL_clamp(dialog->mouse.y - item.y - 5, 3, 89);
    target = item;
    if (SDL_RenderCopy(dialog->renderer, dialog->art, &face, &target) != 0)
        return -1;
    target = (SDL_Rect){item.x + 28, item.y + 1, track.w, track.h};
    if (SDL_RenderCopy(dialog->renderer, dialog->art, &track, &target) != 0)
        return -1;
    target = (SDL_Rect){item.x + 23, item.y + top, knob.w, knob.h};
    if (SDL_RenderCopy(dialog->renderer, dialog->art, &knob, &target) != 0)
        return -1;
    if (!dialog->keyboard || dialog->focus != slider)
        return 0;
    /* Host keyboard focus, in the carrier rooms' highlight colour. */
    target = (SDL_Rect){item.x - 2, item.y - 2, item.w + 4, item.h + 4};
    if (SDL_SetRenderDrawColor(dialog->renderer, 245, 225, 135, 255) != 0)
        return -1;
    return SDL_RenderDrawRect(dialog->renderer, &target);
}

static int SwcInsideItem(const SwcVolumeDialog *dialog, SDL_Rect item)
{
    item.x += dialog->origin.x;
    item.y += dialog->origin.y;
    return SDL_PointInRect(&dialog->mouse, &item);
}

static int SwcDrawVolumeControls(SwcVolumeDialog *dialog)
{
    static const SDL_Color ink = {0, 0, 0, 255};
    SDL_Renderer *renderer = dialog->renderer;
    SDL_Rect content = {dialog->origin.x, dialog->origin.y, SWC_VOLUME_WIDTH, SWC_VOLUME_HEIGHT};
    SDL_Rect frame;

    /* SDL does not retain the presented game picture behind the dialog. */
    if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) != 0 ||
        SDL_RenderClear(renderer) != 0)
        return -1;
    /* A double-bordered modal frame around the white content. */
    frame = (SDL_Rect){content.x - 6, content.y - 6, content.w + 12, content.h + 12};
    if (SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255) != 0 ||
        SDL_RenderFillRect(renderer, &frame) != 0 ||
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) != 0)
        return -1;
    frame = (SDL_Rect){content.x - 4, content.y - 4, content.w + 8, content.h + 8};
    if (SDL_RenderDrawRect(renderer, &frame) != 0)
        return -1;
    frame = (SDL_Rect){content.x - 3, content.y - 3, content.w + 6, content.h + 6};
    if (SDL_RenderDrawRect(renderer, &frame) != 0)
        return -1;
    /* DITL/132 items 4 and 5: static text at (20,10) and (91,10). */
    if (SdlDrawSwcProportionalText(renderer, &dialog->font, content.x + 20,
            content.y + 22, ink, "Music") != 0 ||
        SdlDrawSwcProportionalText(renderer, &dialog->font, content.x + 91,
            content.y + 22, ink, "Sound Effects") != 0 ||
        SwcDrawVolumeSlider(dialog, 0, swcOptions.musicVolume) != 0 ||
        SwcDrawVolumeSlider(dialog, 1, swcOptions.soundVolume) != 0 ||
        SwcDrawVolumeButton(dialog, swcVolumeCancel, "Cancel",
            dialog->pressed == 2 && SwcInsideItem(dialog, swcVolumeCancel), 0) != 0 ||
        SwcDrawVolumeButton(dialog, swcVolumeOk, "OK",
            dialog->pressed == 1 && SwcInsideItem(dialog, swcVolumeOk), 1) != 0)
        return -1;
    return 0;
}

/* DoAdjustVolumes +0x5436..+0x54b6: a new sound level plays selection_bing
 * (snd/31 at 255) at that level; a new music level reaches the movie at once. */
static void SwcSetVolume(int slider, int volume)
{
    if (slider == 0 && swcOptions.musicVolume != volume) {
        swcOptions.musicVolume = volume;
        SdlRefreshSwcMusic();
    } else if (slider == 1 && swcOptions.soundVolume != volume) {
        swcOptions.soundVolume = volume;
        SdlApplySwcSoundOptions();
        SdlPlaySwcSound(31, 255);
    }
}

/* DoAdjustVolumes: ModalDialog with OK as default and Cancel as cancel
 * item (+0x4fea..+0x500a); Cancel restores both levels (+0x518c..+0x519c).
 * Escape cancels as in HAPPYDIALOGFILTER. Arrow keys and Tab are host
 * additions. Running inside the caller's event poll suspends its loop as
 * the Mac dialog did; the caller's frame clock is capped after it returns. */
static void SwcRunVolumeControls(SDL_Window *window)
{
    SwcVolumeDialog dialog = {0};
    SwcBuffer pixels = {0};
    SDL_Surface *surface = NULL;
    SDL_Texture *target;
    SDL_Cursor *cursor = SDL_GetCursor();
    SDL_BlendMode blend;
    SDL_Rect clip;
    SDL_Event event;
    Uint8 color[4];
    Uint32 *row;
    SDL_bool grabbed = SDL_GetWindowMouseGrab(window);
    SDL_bool clipped;
    int cursorVisible = SDL_ShowCursor(SDL_QUERY);
    int wasPaused = swcAudioDevice != 0 &&
        SDL_GetAudioDeviceStatus(swcAudioDevice) == SDL_AUDIO_PAUSED;
    int savedMusic = swcOptions.musicVolume;
    int savedSound = swcOptions.soundVolume;
    int accepted = 0;
    int done = 0;
    int step;
    int x;
    int y;

    dialog.renderer = SDL_GetRenderer(window);
    if (dialog.renderer == NULL)
        return;
    if (SwcReadSoundControl(swcOptionsResourceFork, &pixels) != 0)
        goto fail;
    surface = SDL_CreateRGBSurfaceWithFormat(0, 66, 118, 32, SDL_PIXELFORMAT_ARGB8888);
    if (surface == NULL)
        goto fail;
    for (y = 0; y < 118; y++) {
        row = (Uint32 *)((Uint8 *)surface->pixels + y * surface->pitch);
        for (x = 0; x < 66; x++)
            row[x] = pixels.data[y * 66 + x] ? UINT32_C(0xff000000) : UINT32_C(0xffffffff);
    }
    dialog.art = SDL_CreateTextureFromSurface(dialog.renderer, surface);
    if (dialog.art == NULL || SdlCreateSwcTextFont(dialog.renderer, &dialog.font, 12) != 0)
        goto fail;
    target = SDL_GetRenderTarget(dialog.renderer);
    clipped = SDL_RenderIsClipEnabled(dialog.renderer);
    SDL_RenderGetClipRect(dialog.renderer, &clip);
    SDL_GetRenderDrawColor(dialog.renderer, &color[0], &color[1], &color[2], &color[3]);
    SDL_GetRenderDrawBlendMode(dialog.renderer, &blend);
    SDL_SetRenderTarget(dialog.renderer, NULL);
    SDL_RenderSetClipRect(dialog.renderer, NULL);
    SDL_SetRenderDrawBlendMode(dialog.renderer, SDL_BLENDMODE_NONE);
    swcVolumeDialogOpen = 1;
    SdlHoldSwcSound(1);
    SdlPauseSwcSound(0);
    SDL_SetWindowMouseGrab(window, SDL_FALSE);
    SDL_SetCursor(SDL_GetDefaultCursor());
    SDL_ShowCursor(SDL_ENABLE);
    /* Event positions are already in the 320x240 logical space. */
    dialog.mouse = (SDL_Point){-1, -1};
    dialog.origin = (SDL_Point){(SWC_FRAME_WIDTH - SWC_VOLUME_WIDTH) / 2,
                                (SWC_FRAME_HEIGHT - SWC_VOLUME_HEIGHT) / 2};
    dialog.dragging = -1;
    while (!done) {
        while (!done && SDL_PollEvent(&event)) {
            if (SdlHandleSwcEvent(window, &event, 0))
                continue;
            if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE)) {
                /* Cancel, then let the suspended loop see the quit. */
                SDL_PushEvent(&event);
                done = 1;
            } else if (event.type == SDL_MOUSEMOTION) {
                dialog.mouse = (SDL_Point){event.motion.x, event.motion.y};
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                dialog.mouse = (SDL_Point){event.button.x, event.button.y};
                for (step = 0; step < 2; step++) {
                    if (SwcInsideItem(&dialog, swcVolumeSliders[step])) {
                        dialog.dragging = step;
                        dialog.focus = step;
                        dialog.keyboard = 0;
                    }
                }
                dialog.pressed = SwcInsideItem(&dialog, swcVolumeOk) ? 1 :
                    SwcInsideItem(&dialog, swcVolumeCancel) ? 2 : 0;
            } else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
                dialog.mouse = (SDL_Point){event.button.x, event.button.y};
                if (dialog.dragging >= 0)
                    SwcSetVolume(dialog.dragging, SwcVolumeAt(dialog.mouse.y -
                        dialog.origin.y - swcVolumeSliders[dialog.dragging].y));
                if (dialog.pressed == 1 && SwcInsideItem(&dialog, swcVolumeOk))
                    accepted = done = 1;
                else if (dialog.pressed == 2 && SwcInsideItem(&dialog, swcVolumeCancel))
                    done = 1;
                dialog.dragging = -1;
                dialog.pressed = 0;
            } else if (event.type == SDL_KEYDOWN && dialog.dragging < 0) {
                switch (event.key.keysym.sym) {
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    accepted = done = 1;
                    break;
                case SDLK_ESCAPE:
                case SDLK_BACKSPACE:
                    done = 1;
                    break;
                case SDLK_UP:
                case SDLK_DOWN:
                    step = SwcVolumeStep(dialog.focus == 0 ? swcOptions.musicVolume :
                        swcOptions.soundVolume) + (event.key.keysym.sym == SDLK_UP ? -1 : 1);
                    SwcSetVolume(dialog.focus, swcVolumeSteps[SDL_clamp(step, 0, 7)]);
                    dialog.keyboard = 1;
                    break;
                case SDLK_LEFT:
                case SDLK_RIGHT:
                case SDLK_TAB:
                    dialog.focus = !dialog.focus;
                    dialog.keyboard = 1;
                    break;
                default:
                    break;
                }
            }
        }
        if (SwcDrawVolumeControls(&dialog) != 0)
            break;
        SDL_RenderPresent(dialog.renderer);
        SDL_Delay(16);
    }
    if (!accepted) {
        swcOptions.musicVolume = savedMusic;
        swcOptions.soundVolume = savedSound;
        SdlApplySwcSoundOptions();
        SdlRefreshSwcMusic();
    }
    swcVolumeDialogOpen = 0;
    SdlHoldSwcSound(0);
    SdlPauseSwcSound(wasPaused);
    SDL_SetCursor(cursor);
    SDL_ShowCursor(cursorVisible);
    SDL_SetWindowMouseGrab(window, grabbed);
    SDL_SetRenderTarget(dialog.renderer, target);
    SDL_RenderSetClipRect(dialog.renderer, clipped ? &clip : NULL);
    SDL_SetRenderDrawColor(dialog.renderer, color[0], color[1], color[2], color[3]);
    SDL_SetRenderDrawBlendMode(dialog.renderer, blend);
    /* Loops that repaint only for new frames or window events do so now. */
    SDL_zero(event);
    event.type = SDL_WINDOWEVENT;
    event.window.event = SDL_WINDOWEVENT_EXPOSED;
    event.window.windowID = SDL_GetWindowID(window);
    SDL_PushEvent(&event);
    goto done;
fail:
    fprintf(stderr, "SWC Volume Controls unavailable: %s\n", SDL_GetError());
done:
    SDL_DestroyTexture(dialog.font.texture);
    SDL_DestroyTexture(dialog.art);
    SDL_FreeSurface(surface);
    SDL_free(pixels.data);
}

/* DoOptionsMenu +0x2db8: Turn Music Off disposes of the playing movie and
 * Turn Music On reopens the current track. */
static void SwcToggleMusic(void)
{
    swcOptions.music = !swcOptions.music;
    SdlRefreshSwcMusic();
    SwcShowOptionNotice(swcOptions.music ? "Music on" : "Music off");
}

/* DoOptionsMenu +0x2d32: Turn Sound Off stops the flight channels; later
 * effects, speech and movie audio play at zero volume until it is on. */
static void SwcToggleSound(void)
{
    swcOptions.sound = !swcOptions.sound;
    if (!swcOptions.sound)
        SdlStopSwcSounds();
    SdlApplySwcSoundOptions();
    SwcShowOptionNotice(swcOptions.sound ? "Sound on" : "Sound off");
}

/* DoOptionsMenu +0x2f26: DATA/0 +0x8c4c gates print_subtitle (+0x112e),
 * which draws dialogue and the bar and barracks hotspot names. */
static void SwcToggleText(void)
{
    swcOptions.text = !swcOptions.text;
    SwcShowOptionNotice(swcOptions.text ? "Text messages on" : "Text messages off");
}

int SdlHandleSwcOptionKey(SDL_Window *window, const SDL_KeyboardEvent *key)
{
#ifdef __APPLE__
    const Uint16 command = KMOD_GUI;
#else
    const Uint16 command = KMOD_CTRL;
#endif
    SDL_Keycode code = key->keysym.sym;

    if (!(key->keysym.mod & command) ||
        (code != SDLK_u && code != SDLK_n && code != SDLK_b && code != SDLK_v))
        return 0;
    /* The modal dialog ignores menu commands, as ModalDialog does. */
    if (key->type != SDL_KEYDOWN || key->repeat || swcVolumeDialogOpen)
        return 1;
    if (code == SDLK_u)
        SwcToggleMusic();
    else if (code == SDLK_n)
        SwcToggleSound();
    else if (code == SDLK_b)
        SwcToggleText();
    else
        SwcRunVolumeControls(window);
    SwcSaveOptions();
    return 1;
}
