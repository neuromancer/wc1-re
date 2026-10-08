/* SDL carrier presentation for the Mac demo's first mission.
 * Room routing: CODE_09 GameFlow +0x1b8e; CODE_07 RecRoom +0x059a;
 * CODE_03 BarracksScreen +0x5c70 and SceneDirector +0x254c.
 * WC1 owns pilot/campaign state, hit testing, scene tests, substitutions and
 * animation parsing. Mac sprites, palettes, audio and events are adapted here.
 * This is not a replacement for WC1's disk-backed room/scene director. */
#include "wc1.h"
#include "swc.h"

#include <string.h>

enum SwcRoomPackId {
    SWC_RECROOM, SWC_BARRACKS, SWC_MEDALS, SWC_SHOTGLASS, SWC_BAR_HEADS,
    SWC_BRIEFING, SWC_COMMANDER, SWC_WINGMEN, SWC_BRIEFING_MAP, SWC_ROOM_PACKS
};

typedef struct SwcRoomArt {
    char type[4];
    uint32_t id;
    SwcBuffer shape;
    uint32_t count;
    SDL_Texture **textures;
    SDL_Rect *bounds;
} SwcRoomArt;

typedef struct SwcRoomPack {
    SwcCmf cmf;
    SDL_Color colors[256];
    SwcRoomArt art[8];
    int count;
} SwcRoomPack;

typedef struct SwcCarrier {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *font;
    const char *resourceFork;
    SwcRoomPack packs[SWC_ROOM_PACKS];
    SwcBuffer script;
    SDL_AudioDeviceID audio;
    SDL_AudioSpec audioSpec;
    SDL_Scancode armedKey;
    int audioInitialized;
    int audioAttempted;
    int mouseDown;
    int mouseX;
    int mouseY;
    int focused;
    int paused;
    int drawResult;
} SwcCarrier;

typedef struct SwcRoomInput {
    int quit;
    int back;
    int activate;
    int clicked;
    int moved;
    int step;
} SwcRoomInput;

typedef struct SwcRoomScene {
    ConversationSceneRecord records[256];
    size_t count;
    unsigned char *text;
    size_t textSize;
} SwcRoomScene;

/* CODE_07 RecRoom: characters override the door/board regions. WC1's
 * FindMenuRegionAtPoint returns the first matching inclusive rectangle. */
static const TitleMenuRegion swcBarRegions[] = {
    {0, 173, 76, 216, 106}, {1, 137, 99, 182, 191}, {2, 254, 103, 307, 193},
    {3, 268, 26, 320, 65}, {4, 250, 65, 318, 127}, {5, 0, 90, 70, 145},
    {-1, 0, 0, 0, 0}
};
static const char *const swcBarLabels[] = {
    "Talk to SHOTGLASS", "Talk to PALADIN", "Talk to ANGEL",
    "Enter BARRACKS", "Check Pilot Ranking", "Enter SIMULATOR"
};
/* CODE_03 BarracksScreen +0x5c70, original Mac rectangles. */
static const TitleMenuRegion swcBunkRegions[] = {
    {0, 235, 57, 260, 114}, {1, 50, 56, 85, 113}, {2, 150, 110, 183, 147},
    {3, 0, 71, 59, 92}, {4, 0, 139, 68, 160}, {5, 43, 74, 90, 90},
    {6, 11, 114, 88, 131}, {7, 250, 73, 320, 90}, {8, 244, 142, 320, 156},
    {9, 224, 75, 255, 88}, {10, 226, 115, 320, 132}, {-1, 0, 0, 0, 0}
};
static const char *const swcBunkLabels[] = {
    "Enter BRIEFING ROOM", "Enter RECROOM", "View MEDALS",
    "BUNK 1", "BUNK 2", "BUNK 3", "BUNK 4",
    "BUNK 5", "BUNK 6", "BUNK 7", "BUNK 8"
};

static SwcRoomArt *SwcGetRoomArt(SwcCarrier *carrier, enum SwcRoomPackId packId,
                                 const char type[4], uint32_t id)
{
    /* Expanded DATA/0 +0x4046 (filenames), +0x3fd2 (clut IDs). */
    static const char *const files[SWC_ROOM_PACKS] = {
        "RecRoomScene.CMF", "Barracks.CMF", "ViewMedals.CMF", "Head10.CMF",
        "RRTalkingHeads.CMF", "Briefing.CMF", "BriefingCommander.CMF",
        "BRTH1.CMF", "BRInterface.CMF"
    };
    static const short palettes[SWC_ROOM_PACKS] = {135, 150, 151, 137, 136, 170, 171, 173, 172};
    SwcRoomPack *pack = &carrier->packs[packId];
    SwcRoomArt *art;
    SwcFrame frame;
    char name[96];
    char path[PATH_MAX];
    uint32_t index;
    int entry;

    if (carrier->drawResult != 0)
        return NULL;
    if (pack->cmf.data == NULL) {
        SDL_snprintf(name, sizeof(name), "CMFs/%s", files[packId]);
        if (!SdlResolvePath(name, path, sizeof(path))) {
            SDL_SetError("Cannot resolve SWC carrier asset %s", name);
            goto fail;
        }
        if (SwcCMOpen(path, &pack->cmf) != 0 ||
            SwcReadPalette(carrier->resourceFork, palettes[packId], pack->colors) != 0)
            goto fail;
    }
    for (entry = 0; entry < pack->count; entry++) {
        art = &pack->art[entry];
        if (art->id == id && memcmp(art->type, type, 4) == 0)
            return art;
    }
    if (pack->count == (int)SDL_arraysize(pack->art)) {
        SDL_SetError("Too many SWC carrier shapes in %s", files[packId]);
        goto fail;
    }
    art = &pack->art[pack->count++];
    art->id = id;
    memcpy(art->type, type, 4);
    if (SwcCMGetChunk(&pack->cmf, type, id, &art->shape) != 0 ||
        SwcGetFrameCount(&art->shape, &art->count) != 0)
        goto fail;
    if (art->count == 0 || art->count > 128) {
        SDL_SetError("Invalid SWC carrier frame count");
        goto fail;
    }
    art->textures = SDL_calloc(art->count, sizeof(*art->textures));
    art->bounds = SDL_calloc(art->count, sizeof(*art->bounds));
    if (art->textures == NULL || art->bounds == NULL) {
        SDL_OutOfMemory();
        goto fail;
    }
    for (index = 0; index < art->count; index++) {
        if (SwcGetFramePtr(&art->shape, index, &frame) != 0)
            goto fail;
        if (frame.width > 640 || frame.height > 480) {
            SDL_SetError("Oversized SWC carrier frame");
            goto fail;
        }
        art->bounds[index] = (SDL_Rect){frame.x, frame.y, frame.width, frame.height};
    }
    return art;
fail:
    carrier->drawResult = -1;
    return NULL;
}

static void SwcDrawRoomArt(SwcCarrier *carrier, enum SwcRoomPackId packId,
                            const char type[4], uint32_t id, int frame, int x, int y)
{
    SwcRoomArt *art = SwcGetRoomArt(carrier, packId, type, id);
    SDL_Rect destination;

    if (art == NULL)
        return;
    if (frame < 0 || (uint32_t)frame >= art->count) {
        carrier->drawResult = SDL_SetError("Invalid SWC carrier frame %.4s/%u:%d", type, id, frame);
        return;
    }
    destination = art->bounds[frame];
    if (destination.w == 0 || destination.h == 0)
        return;
    if (art->textures[frame] == NULL) {
        art->textures[frame] = SdlCreateSwcTexture(carrier->renderer, &art->shape,
            (uint32_t)frame, carrier->packs[packId].colors);
        if (art->textures[frame] == NULL) {
            carrier->drawResult = -1;
            return;
        }
    }
    destination.x += x;
    destination.y += y;
    carrier->drawResult = SDL_RenderCopy(carrier->renderer, art->textures[frame],
                                         NULL, &destination);
}

static SwcRoomInput SwcPollCarrier(SwcCarrier *carrier)
{
    SwcRoomInput input = {0};
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || (event.type == SDL_WINDOWEVENT &&
            event.window.event == SDL_WINDOWEVENT_CLOSE))
            input.quit = 1;
        if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST ||
                event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                carrier->focused = 0;
                carrier->armedKey = SDL_SCANCODE_UNKNOWN;
                carrier->mouseDown = 0;
            } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED ||
                       event.window.event == SDL_WINDOWEVENT_RESTORED) {
                carrier->focused = (SDL_GetWindowFlags(carrier->window) & SDL_WINDOW_INPUT_FOCUS) != 0;
            }
        }
        if (!carrier->focused)
            continue;
        if (event.type == SDL_MOUSEMOTION) {
            carrier->mouseX = event.motion.x;
            carrier->mouseY = event.motion.y;
            input.moved = 1;
        }
        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            carrier->mouseDown = 1;
        if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
            carrier->mouseX = event.button.x;
            carrier->mouseY = event.button.y;
            input.clicked = carrier->mouseDown;
            carrier->mouseDown = 0;
        }
        if (event.type == SDL_KEYDOWN && !event.key.repeat) {
            switch (event.key.keysym.sym) {
            case SDLK_RETURN:
            case SDLK_KP_ENTER:
            case SDLK_SPACE:
            case SDLK_ESCAPE:
                carrier->armedKey = event.key.keysym.scancode;
                break;
            case SDLK_TAB:
                input.step = (event.key.keysym.mod & KMOD_SHIFT) ? -1 : 1;
                break;
            case SDLK_RIGHT:
            case SDLK_DOWN:
                input.step = 1;
                break;
            case SDLK_LEFT:
            case SDLK_UP:
                input.step = -1;
                break;
            case SDLK_p:
                carrier->paused = !carrier->paused;
                break;
            default:
                break;
            }
        }
        /* Finish activation on release: opening a scene must not also skip
         * its first line, and launch input must not fire the player's guns. */
        if (event.type == SDL_KEYUP && carrier->armedKey != SDL_SCANCODE_UNKNOWN &&
            event.key.keysym.scancode == carrier->armedKey) {
            input.back = event.key.keysym.sym == SDLK_ESCAPE;
            input.activate = !input.back;
            carrier->armedKey = SDL_SCANCODE_UNKNOWN;
        }
    }
    if (carrier->audio != 0)
        SDL_PauseAudioDevice(carrier->audio, !carrier->focused || carrier->paused);
    return input;
}

static uint32_t SwcSceneOffset(const uint8_t *bytes)
{
    return (uint32_t)bytes[0] | (uint32_t)bytes[1] << 8 |
        (uint32_t)bytes[2] << 16 | (uint32_t)bytes[3] << 24;
}

static short SwcSceneWord(const uint8_t *bytes)
{
    return (short)(bytes[0] | (unsigned int)bytes[1] << 8);
}

static int SwcValidateSceneTests(const unsigned char *text, size_t count)
{
    unsigned int command;
    unsigned int value;
    unsigned int first;
    int arguments;
    int argument;
    int digits;

    /* Validate before entering WC1's original, unbounded int_value parser.
     * The first-mission bar uses command 4; retain goto and score tests too.
     * Other campaign scripts need their own state/operand validation. */
    while ((command = *text++) != 0) {
        if (command < 1 || command > 5)
            return SDL_SetError("Unsupported SWC carrier branch %u", command);
        arguments = command == 1 ? 1 : 2;
        first = 0;
        for (argument = 0; argument < arguments; argument++) {
            value = 0;
            digits = 0;
            while (*text >= '0' && *text <= '9') {
                if (++digits > 5)
                    return SDL_SetError("Oversized SWC scene branch operand");
                value = value * 10 + *text++ - '0';
            }
            if (digits == 0 || value > INT16_MAX || (*text != ',' && *text != ')'))
                return SDL_SetError("Invalid SWC scene branch operand");
            text++;
            if (argument == 0)
                first = value;
        }
        if (value >= count || ((command == 4 || command == 5) && first >= 8))
            return SDL_SetError("SWC scene branch target or pilot is out of range");
    }
    return 0;
}

static int SwcValidateAnimation(const char *text, int face)
{
    int digits;

    while (*text != 0) {
        if (face && *text == 'R') {
            text++;
            continue;
        }
        if (face) {
            if (!((*text >= '0' && *text <= '9') || (*text >= 'A' && *text <= 'F')))
                return SDL_SetError("Invalid SWC face command");
        } else if (*text != '$' && !(*text >= 'a' && *text <= 'z')) {
            return SDL_SetError("Invalid SWC mouth command");
        }
        text++;
        digits = 0;
        while (*text >= '0' && *text <= '9') {
            if (++digits > 4)
                return SDL_SetError("Oversized SWC animation duration");
            text++;
        }
        if (face) {
            if (digits == 0 || *text != ',')
                return SDL_SetError("Unterminated SWC face command");
            text++;
        }
    }
    return 0;
}

static int SwcLoadRoomScene(SwcCarrier *carrier, int section, SwcRoomScene *scene)
{
    ConversationSceneRecord *record;
    const uint8_t *bytes;
    const unsigned char *end;
    const char *text;
    uint32_t offsets[11];
    short strings[4];
    size_t index;
    size_t length;
    size_t expanded;
    int field;

    /* LoadBriefingData +0x1556 and scene_ptr +0x39c6: unlike Mac sprites,
     * these directory entries and the five record shorts are little-endian. */
    if (carrier->script.size < 40 || section < 0 || section > 8 || (section & 1))
        return SDL_SetError("Invalid SWC briefing section");
    for (index = 0; index < 10; index++) {
        offsets[index] = SwcSceneOffset(carrier->script.data + index * 4);
        if (offsets[index] > carrier->script.size || offsets[index] < 40 ||
            (index > 0 && offsets[index] < offsets[index - 1]))
            return SDL_SetError("Invalid SWC briefing directory");
    }
    offsets[10] = (uint32_t)carrier->script.size;
    length = offsets[section + 1] - offsets[section];
    scene->count = length / 13;
    scene->text = carrier->script.data + offsets[section + 1];
    scene->textSize = offsets[section + 2] - offsets[section + 1];
    if (offsets[0] != 40 || length % 13 != 0 || scene->count == 0 ||
        scene->count > SDL_arraysize(scene->records))
        return SDL_SetError("Invalid SWC scene record count");
    for (index = 0; index < scene->count; index++) {
        bytes = carrier->script.data + offsets[section] + index * 13;
        record = &scene->records[index];
        record->shot = (signed char)bytes[0];
        record->textColour = (signed char)bytes[1];
        record->talker = (signed char)bytes[2];
        record->duration = SwcSceneWord(bytes + 3);
        record->testsOffset = strings[0] = SwcSceneWord(bytes + 5);
        record->textOffset = strings[1] = SwcSceneWord(bytes + 7);
        record->mouthAnimationOffset = strings[2] = SwcSceneWord(bytes + 9);
        record->faceAnimationOffset = strings[3] = SwcSceneWord(bytes + 11);
        for (field = 0; field < 4; field++) {
            if (strings[field] < 0 || (size_t)strings[field] >= scene->textSize)
                return SDL_SetError("SWC scene string offset is out of range");
            text = (const char *)scene->text + strings[field];
            end = memchr(text, 0, scene->textSize - strings[field]);
            if (end == NULL || end - (const unsigned char *)text > 255)
                return SDL_SetError("Unterminated or oversized SWC scene string");
        }
        if (SwcValidateSceneTests(scene->text + record->testsOffset, scene->count) != 0 ||
            SwcValidateAnimation((char *)scene->text + record->mouthAnimationOffset, 0) != 0 ||
            SwcValidateAnimation((char *)scene->text + record->faceAnimationOffset, 1) != 0)
            return -1;
        text = (const char *)scene->text + record->textOffset;
        expanded = 0;
        while (*text != 0) {
            if (*text++ == '$') {
                /* All Enyo 1 substitutions are $C. Validate both the marker
                 * and output capacity before sharing WC1's AddPCName. */
                if (*text++ != 'C')
                    return SDL_SetError("Unsupported SWC carrier text substitution");
                expanded += strlen(stCampaignState.currentPilot->callsign);
            } else {
                expanded++;
            }
        }
        if (expanded >= 256)
            return SDL_SetError("Expanded SWC subtitle is too long");
    }
    if (scene->records[scene->count - 1].shot != -2)
        return SDL_SetError("SWC scene has no end command");
    return 0;
}

static int SwcSceneAnimationFrame(const short *commands, Uint32 ticks)
{
    const short *cursor = commands;
    const short *repeat = NULL;
    Uint32 period = 0;
    Uint32 prefix = 0;
    Uint32 duration;

    while (*cursor != -1) {
        if (*cursor == -2) {
            repeat = cursor + 2;
            prefix = period;
        } else {
            period += (Uint32)SDL_max(1, cursor[1]);
        }
        cursor += 2;
    }
    if (ticks >= period) {
        if (repeat == NULL || period == prefix)
            return -1;
        ticks = prefix + (ticks - prefix) % (period - prefix);
    }
    for (cursor = commands; *cursor != -1; cursor += 2) {
        if (*cursor == -2)
            continue;
        duration = (Uint32)SDL_max(1, cursor[1]);
        if (ticks < duration)
            return *cursor == 10 ? -1 : *cursor;
        ticks -= duration;
    }
    return -1;
}

static int SwcStartSpeech(SwcCarrier *carrier, int section, int line, Uint32 *duration)
{
    SwcBuffer pcm = {0};
    SDL_AudioSpec desired = {0};
    SDL_RWops *file;
    char relative[96];
    char path[PATH_MAX];
    Uint32 speechDuration;
    int result = 0;

    if (carrier->audio != 0) {
        SDL_PauseAudioDevice(carrier->audio, 1);
        SDL_ClearQueuedAudio(carrier->audio);
    }
    /* SceneDirector +0x2cb6; DATA/0 +0x4322/+0x4326:
     * "%s%d:%2.2d%2.2d%2.2d%2.2d.AIF", "camp.". The archive groups
     * the mission's files under AIFF/camp.0/04. Keep original record indices. */
    SDL_snprintf(relative, sizeof(relative), "AIFF/camp.0/04/04%02d%02d00.AIF", section, line);
    if (!SdlResolvePath(relative, path, sizeof(path)))
        return SDL_SetError("Cannot resolve SWC speech %s", relative);
    file = SDL_RWFromFile(path, "rb");
    if (file == NULL) {
        SDL_ClearError();
        return 0; /* Optional speech: subtitles retain a readable duration. */
    }
    SDL_RWclose(file);
    if (SwcReadSpeech(path, &pcm) != 0)
        return -1;
    speechDuration = (Uint32)((uint64_t)pcm.size * 1000 / (11025 * 2)) + 250;
    if (!carrier->audioAttempted) {
        carrier->audioAttempted = 1;
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) == 0) {
            carrier->audioInitialized = 1;
            desired.freq = 11025;
            desired.format = AUDIO_S16MSB;
            desired.channels = 1;
            desired.samples = 1024;
            carrier->audio = SDL_OpenAudioDevice(NULL, 0, &desired, &carrier->audioSpec, 0);
        }
        if (carrier->audio == 0) {
            fprintf(stderr, "SWC speech audio unavailable; using subtitles: %s\n", SDL_GetError());
            SDL_ClearError();
        }
    }
    if (carrier->audio != 0) {
        result = SDL_QueueAudio(carrier->audio, pcm.data, (Uint32)pcm.size);
        *duration = speechDuration + (Uint32)carrier->audioSpec.samples * 1000 / 11025;
    } else {
        *duration = SDL_max(*duration, speechDuration);
    }
    SDL_free(pcm.data);
    return result;
}

static void SwcDrawBriefingMap(SwcCarrier *carrier, int objective)
{
    SDL_Renderer *renderer = carrier->renderer;
    SDL_Rect panel = {80, 25, 160, 146};
    SDL_Rect marker;
    SDL_Color ink = {100, 245, 165, 255};
    short x;
    short y;
    short nextX;
    short nextY;
    int index;
    int next;
    char label[12];

    SwcDrawRoomArt(carrier, SWC_BRIEFING_MAP, "BRFG", 3, 0, 0, 0);
    if (carrier->drawResult != 0)
        return;
    SetScale();
    if (SDL_SetRenderDrawColor(renderer, 0, 5, 10, 255) != 0 ||
        SDL_RenderFillRect(renderer, &panel) != 0 ||
        SDL_SetRenderDrawColor(renderer, 75, 210, 140, 255) != 0) {
        carrier->drawResult = -1;
        return;
    }
    /* Share WC1's mission objective list and coordinate transform. The
     * original Mac map's panning, globe and transition animation are pending. */
    for (index = 0; index < cMissionObjectiveCount; index++) {
        nav_getxy(&x, &y, aMissionObjectives[index].position.x, aMissionObjectives[index].position.z);
        next = (index + 1) % cMissionObjectiveCount;
        nav_getxy(&nextX, &nextY, aMissionObjectives[next].position.x, aMissionObjectives[next].position.z);
        marker = (SDL_Rect){85 + x - 2, 30 + y - 2, index == objective ? 7 : 4,
                            index == objective ? 7 : 4};
        SDL_snprintf(label, sizeof(label), "%d", index + 1);
        if (SDL_RenderDrawLine(renderer, 85 + x, 30 + y, 85 + nextX, 30 + nextY) != 0 ||
            SDL_RenderDrawRect(renderer, &marker) != 0 ||
            SdlDrawSwcText(renderer, carrier->font, 91 + x, 30 + y, ink, label) != 0) {
            carrier->drawResult = -1;
            return;
        }
    }
}

static void SwcDrawScene(SwcCarrier *carrier, int shot, int textColour, int talker,
                          int mouth, int face)
{
    enum SwcRoomPackId pack;
    const char *background;
    uint32_t backgroundId;
    int backgroundFrame;

    if (shot >= 20 && shot <= 30) {
        background = "RCRM";
        backgroundId = 2;
        if (shot == 30) {
            pack = SWC_SHOTGLASS;
            backgroundFrame = 0;
        } else if (shot == 25 || shot == 26) {
            pack = SWC_BAR_HEADS;
            backgroundFrame = talker == 4 ? 2 : 1;
        } else if (shot == 20) {
            pack = SWC_COMMANDER;
            background = "BRFG";
            backgroundId = 6;
            backgroundFrame = 0;
        } else if (shot == 21 || shot == 29) {
            pack = SWC_WINGMEN;
            background = "BRFG";
            backgroundId = 6;
            backgroundFrame = textColour == 9 ? 2 : 1;
        } else {
            carrier->drawResult = SDL_SetError("Unsupported SWC carrier portrait %d", shot);
            return;
        }
        SwcDrawRoomArt(carrier, pack, background, backgroundId, backgroundFrame, 0, 0);
        SwcDrawRoomArt(carrier, pack, "TKHD", (uint32_t)(shot - 19), 0, 0, 0);
        if (face >= 0 && face < 10)
            SwcDrawRoomArt(carrier, pack, "TKHD", (uint32_t)(shot - 19), face + 11, 0, 0);
        if (mouth >= 0)
            SwcDrawRoomArt(carrier, pack, "TKHD", (uint32_t)(shot - 19), mouth + 1, 0, 0);
    } else if (shot == 0 || shot == 1 || shot == 5) {
        SwcDrawRoomArt(carrier, SWC_BRIEFING, "BRFG", 1, 0, 0, 0);
        /* DrawBriefingLongShot +0x1e54; DATA/0 +0x4306/+0x430a. */
        SwcDrawRoomArt(carrier, SWC_BRIEFING, "BRFG", 2, SDL_max(0, mouth), 90, 90);
    } else if (shot == 2) {
        /* DrawPodiumShot +0x1e02: background 1; body at (25,50). */
        SwcDrawRoomArt(carrier, SWC_BRIEFING, "BRFG", 3, 1, 0, 0);
        SwcDrawRoomArt(carrier, SWC_BRIEFING, "BRFG", 4, mouth + 1, 25, 50);
    } else if (shot == 3 || shot == 4) {
        SwcDrawBriefingMap(carrier, talker < 0 ? -talker : talker);
    } else {
        carrier->drawResult = SDL_SetError("Unsupported SWC carrier shot %d", shot);
    }
}

static int SwcPlayRoomScene(SwcCarrier *carrier, int section)
{
    SwcRoomScene scene;
    ConversationSceneRecord *record;
    ConversationSceneRecord *selected;
    SwcRoomInput input;
    short mouth[513];
    short face[513];
    char text[256];
    Uint32 previous;
    Uint32 current;
    Uint32 elapsed;
    Uint32 duration;
    size_t index = 0;
    size_t commands = 0;
    int shot = 0;
    int talker = -1;
    int colour = 0;
    int wasPlaying;
    int result = -1;

    if (SwcLoadRoomScene(carrier, section, &scene) != 0)
        return -1;
    SDL_SetCursor(SDL_GetDefaultCursor());
    carrier->armedKey = SDL_SCANCODE_UNKNOWN;
    carrier->mouseDown = 0;
    carrier->paused = 0;
    SDL_SetWindowTitle(carrier->window, section == 0
        ? "Super Wing Commander - briefing | Click/Space/Enter: next | Esc: launch | P: pause"
        : "Super Wing Commander - conversation | Click/Space/Enter: next | Esc: bar | P: pause");
    while (index < scene.count) {
        record = &scene.records[index];
        if (record->shot == -2) {
            result = 0;
            break;
        }
        if (++commands > scene.count * 4) {
            SDL_SetError("SWC carrier scene branch loop");
            break;
        }
        selected = ParseTests(record, scene.records, scene.text);
        if (selected != record) {
            index = (size_t)(selected - scene.records);
            continue;
        }
        if (record->shot != -1)
            shot = record->shot & 0x3f;
        if (record->talker != -2)
            talker = record->talker;
        if (record->textColour != -1)
            colour = record->textColour;
        ParseMouthAnimation((char *)scene.text + record->mouthAnimationOffset, mouth);
        ParseFaceAnimation((char *)scene.text + record->faceAnimationOffset, face);
        SDL_strlcpy(text, AddPCName((char *)scene.text + record->textOffset), sizeof(text));
        duration = (Uint32)SDL_max(2000, (int)strlen(text) * 50 + 1000);
        if (SwcStartSpeech(carrier, section, (int)index, &duration) != 0)
            break;
        previous = SDL_GetTicks();
        elapsed = 0;
        wasPlaying = 0;
        for (;;) {
            current = SDL_GetTicks();
            if (wasPlaying)
                elapsed += current - previous;
            previous = current;
            input = SwcPollCarrier(carrier);
            if (input.quit) {
                result = SWC_MOVIE_QUIT;
                goto done;
            }
            if (input.back) {
                result = 0;
                goto done;
            }
            wasPlaying = carrier->focused && !carrier->paused;
            if (wasPlaying && (input.activate || input.clicked ||
                (elapsed >= duration && (carrier->audio == 0 || SDL_GetQueuedAudioSize(carrier->audio) == 0) &&
                 carrier->armedKey == SDL_SCANCODE_UNKNOWN && !carrier->mouseDown)))
                break;
            if (SDL_SetRenderDrawColor(carrier->renderer, 0, 0, 0, 255) != 0 ||
                SDL_RenderClear(carrier->renderer) != 0)
                goto done;
            SwcDrawScene(carrier, shot, colour, talker,
                SwcSceneAnimationFrame(mouth, elapsed / 50),
                SwcSceneAnimationFrame(face, elapsed / 50));
            if (carrier->drawResult != 0 || SdlDrawSwcSubtitle(carrier->renderer, carrier->font,
                carrier->paused ? "Paused - P to resume" : text) != 0)
                goto done;
            SDL_RenderPresent(carrier->renderer);
            SDL_Delay(16);
        }
        index++;
    }
done:
    if (carrier->audio != 0) {
        SDL_PauseAudioDevice(carrier->audio, 1);
        SDL_ClearQueuedAudio(carrier->audio);
    }
    carrier->paused = 0;
    return result;
}

static int SwcBarIdleFrame(int character, Uint32 ticks)
{
    /* Original byte programs: DATA/0 +0x27ae, +0x2816, +0x27da. A high
     * bit encodes a delay, 255 repeats. WC1's idle frame sets differ. */
    static const uint8_t shotglass[] = {0, 1, 0, 1, 0, 1, 0, 1, 2, 1, 133, 2, 2, 1, 255};
    static const uint8_t paladin[] = {1, 2, 3, 178, 2, 1, 2, 3, 148, 2, 1, 2, 3, 255};
    static const uint8_t angel[] = {2, 138, 3, 4, 148, 3, 4, 3, 4, 138, 0, 1, 255};
    static const uint8_t *const programs[] = {shotglass, paladin, angel};
    const uint8_t *cursor;
    Uint32 period = 0;
    Uint32 duration;
    int frame = 0;

    for (cursor = programs[character]; *cursor != 255; cursor++)
        period += (*cursor & 128) ? (*cursor & 127) : 1;
    ticks %= period;
    for (cursor = programs[character]; *cursor != 255; cursor++) {
        duration = (*cursor & 128) ? (*cursor & 127) : 1;
        if (!(*cursor & 128))
            frame = *cursor;
        if (ticks < duration)
            return frame;
        ticks -= duration;
    }
    return frame;
}

static void SwcDrawRankings(SwcCarrier *carrier)
{
    /* Same ranking rule as WC1 ShowChalkBoard, also present in CODE_07
     * +0x111a. Read the shared pilot records; only drawing is host-specific. */
    static const int rowY[9] = {54, 76, 97, 117, 136, 157, 177, 197, 215};
    static const char *const ranks[5] = {"2LT", "1LT", "CPT", "MAJ", "LTC"};
    SDL_Color ink = {215, 220, 199, 255};
    int order[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    int index;
    int other;
    int swap;
    PilotRecord *pilot;
    char text[64];

    for (index = 0; index < 9; index++) {
        for (other = index + 1; other < 9; other++) {
            if (aPilotRecords[order[index]].kills * 1000 - aPilotRecords[order[index]].missions + 1 <
                aPilotRecords[order[other]].kills * 1000 - aPilotRecords[order[other]].missions + 1) {
                swap = order[index];
                order[index] = order[other];
                order[other] = swap;
            }
        }
    }
    SwcDrawRoomArt(carrier, SWC_RECROOM, "RCRM", 3, 0, 0, 0);
    if (carrier->drawResult != 0)
        return;
    carrier->drawResult = SdlDrawSwcText(carrier->renderer, carrier->font, 46, 34, ink, "PILOT");
    if (carrier->drawResult == 0)
        carrier->drawResult = SdlDrawSwcText(carrier->renderer, carrier->font, 217, 34, ink, "M");
    if (carrier->drawResult == 0)
        carrier->drawResult = SdlDrawSwcText(carrier->renderer, carrier->font, 252, 34, ink, "K");
    for (index = 0; index < 9 && carrier->drawResult == 0; index++) {
        pilot = &aPilotRecords[order[index]];
        SDL_snprintf(text, sizeof(text), "%s %-14s", ranks[SDL_clamp(pilot->rank, 0, 4)], pilot->name);
        carrier->drawResult = SdlDrawSwcText(carrier->renderer, carrier->font, 46,
                                             rowY[index], ink, text);
        if (order[index] != 8 && stCampaignState.personalityDeathMission[order[index]] != 0)
            SDL_strlcpy(text, "KIA", sizeof(text));
        else
            SDL_snprintf(text, sizeof(text), "%3d    %3d", pilot->missions, pilot->kills);
        if (carrier->drawResult == 0)
            carrier->drawResult = SdlDrawSwcText(carrier->renderer, carrier->font, 212,
                                                 rowY[index], ink, text);
    }
}

static void SwcDrawMedalCase(SwcCarrier *carrier)
{
    /* DrawMedals CODE_03 +0x5a9c; DATA/0 +0x42f2. */
    static const int medalFrames[5] = {13, 16, 19, 22, 23};
    int index;
    int count;

    SwcDrawRoomArt(carrier, SWC_MEDALS, "RCRM", 46, 0, 0, 0);
    for (index = 0; index < 12; index++) {
        if (stCampaignState.badges[index] != 0)
            SwcDrawRoomArt(carrier, SWC_MEDALS, "RCRM", 46, index + 1, -109, -77);
    }
    for (index = 0; index < 5; index++) {
        for (count = 0; count < SDL_min(3, stCampaignState.medals[index]); count++)
            SwcDrawRoomArt(carrier, SWC_MEDALS, "RCRM", 46, medalFrames[index] + count, -109, -77);
    }
}

int SdlRunSwcCarrier(SDL_Window *window, SDL_Renderer *renderer,
                      const char *missionPath, const char *resourceFork)
{
    SwcCarrier carrier = {0};
    SwcCmf data = {0};
    SwcBuffer roster = {0};
    SwcRoomInput input;
    SwcRoomArt *art;
    SDL_Cursor *savedCursor = SDL_GetCursor();
    SDL_Cursor *hand = NULL;
    const TitleMenuRegion *regions;
    const char *const *labels;
    const char *notice = NULL;
    SDL_Rect highlight;
    Uint32 previous;
    Uint32 current;
    Uint32 elapsed = 0;
    char failure[256];
    int room = 0; /* 0: bar, 1: barracks, 2: ranking, 3: medals. */
    int selected = -1;
    int keyboard = 0;
    int count;
    int pack;
    int entry;
    int sceneResult;
    int oldCursorVisible = SDL_ShowCursor(SDL_QUERY);
    uint32_t frame;
    int result = -1;

    carrier.window = window;
    if (savedCursor == NULL)
        savedCursor = SDL_GetDefaultCursor();
    carrier.renderer = renderer;
    carrier.resourceFork = resourceFork;
    carrier.focused = (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
    carrier.mouseX = carrier.mouseY = -1;
    if (SwcCMOpen(missionPath, &data) != 0 ||
        SwcCMGetChunk(&data, "BRF0", 5, &carrier.script) != 0 ||
        SwcCMGetChunk(&data, "CMP0", 3, &roster) != 0)
        goto done;
    /* RecRoom indexes CMP0/3 by (series-1)*8 + mission*2. This host is
     * deliberately Enyo 1 only: validate its Paladin/Angel roster. */
    if (roster.size < 2 || roster.data[0] != 5 || roster.data[1] != 4) {
        SDL_SetError("Unsupported SWC carrier roster (expected Enyo 1)");
        goto done;
    }
    carrier.font = SdlCreateSwcFont(renderer);
    if (carrier.font == NULL)
        goto done;
    hand = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    SDL_ShowCursor(SDL_ENABLE);
    SDL_SetWindowMouseGrab(window, SDL_FALSE);
    previous = SDL_GetTicks();
    while (1) {
        current = SDL_GetTicks();
        if (carrier.focused)
            elapsed += current - previous;
        previous = current;
        input = SwcPollCarrier(&carrier);
        if (input.quit || (input.back && room == 0 && notice == NULL)) {
            result = SWC_MOVIE_QUIT;
            break;
        }
        if (notice != NULL) {
            if (input.back || input.activate || input.clicked)
                notice = NULL;
        } else if (room >= 2) {
            if (input.back || input.activate || input.clicked) {
                room = room == 2 ? 0 : 1;
                selected = -1;
            }
        } else {
            regions = room == 0 ? swcBarRegions : swcBunkRegions;
            count = room == 0 ? 6 : 11;
            if (input.back) {
                room = 0;
                selected = -1;
            } else {
                if (input.moved || input.clicked) {
                    selected = FindMenuRegionAtPoint((short)carrier.mouseX, (short)carrier.mouseY, regions);
                    keyboard = 0;
                }
                if (input.step != 0) {
                    selected = selected < 0 ? (input.step > 0 ? 0 : count - 1) :
                        (selected + input.step + count) % count;
                    keyboard = 1;
                }
                if ((input.activate || input.clicked) && selected >= 0) {
                    if (room == 0 && selected < 3) {
                        static const int sections[3] = {4, 8, 6};

                        sceneResult = SwcPlayRoomScene(&carrier, sections[selected]);
                        previous = SDL_GetTicks();
                        if (sceneResult != 0) {
                            result = sceneResult;
                            break;
                        }
                    } else if (room == 0) {
                        if (selected == 3)
                            room = 1;
                        else if (selected == 4)
                            room = 2;
                        else
                            notice = "Sorry! The flight simulator is not available in this demo.";
                    } else if (selected == 0) {
                        result = SwcPlayRoomScene(&carrier, 0);
                        break;
                    } else if (selected == 1) {
                        room = 0;
                    } else if (selected == 2) {
                        room = 3;
                    } else {
                        notice = "Bunks hold saved games. SWC saving and loading are not implemented yet.";
                    }
                    selected = -1;
                }
            }
        }
        SDL_SetWindowTitle(window, room == 0
            ? "Super Wing Commander - bar | Click: select | Tab/arrows, Enter | Esc: exit"
            : "Super Wing Commander - carrier | Click: select | Tab/arrows, Enter | Esc: back");
        if (hand != NULL)
            SDL_SetCursor(selected >= 0 && room < 2 && notice == NULL ? hand : savedCursor);
        if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) != 0 || SDL_RenderClear(renderer) != 0)
            break;
        if (room == 0) {
            SwcDrawRoomArt(&carrier, SWC_RECROOM, "RCRM", 1, 0, 0, 0);
            SwcDrawRoomArt(&carrier, SWC_RECROOM, "RCRM", 1, 1, 0, 0);
            SwcDrawRoomArt(&carrier, SWC_RECROOM, "RCRM", 12, 0, 0, 85);
            SwcDrawRoomArt(&carrier, SWC_RECROOM, "RCRM", 34, SwcBarIdleFrame(0, elapsed / 20), 0, 0);
            SwcDrawRoomArt(&carrier, SWC_RECROOM, "RCRM", 29, SwcBarIdleFrame(1, elapsed / 20), 0, 0);
            SwcDrawRoomArt(&carrier, SWC_RECROOM, "RCRM", 28, SwcBarIdleFrame(2, elapsed / 20), 0, 0);
        } else if (room == 1) {
            SwcDrawRoomArt(&carrier, SWC_BARRACKS, "RCRM", 13, 0, 0, 0);
        } else if (room == 2) {
            SwcDrawRankings(&carrier);
        } else {
            SwcDrawMedalCase(&carrier);
        }
        if (carrier.drawResult != 0)
            break;
        if (notice != NULL) {
            if (SdlDrawSwcSubtitle(renderer, carrier.font, notice) != 0)
                break;
        } else if (room < 2) {
            regions = room == 0 ? swcBarRegions : swcBunkRegions;
            labels = room == 0 ? swcBarLabels : swcBunkLabels;
            if (selected >= 0 && keyboard) {
                highlight = (SDL_Rect){regions[selected].left, regions[selected].top,
                    regions[selected].right - regions[selected].left,
                    regions[selected].bottom - regions[selected].top};
                if (SDL_SetRenderDrawColor(renderer, 245, 225, 135, 255) != 0 ||
                    SDL_RenderDrawRect(renderer, &highlight) != 0)
                    break;
            }
            if (SdlDrawSwcSubtitle(renderer, carrier.font, selected >= 0 ? labels[selected] :
                room == 0 ? "Tiger's Claw - bar. Visit the barracks to reach your briefing."
                          : "Barracks - enter the briefing room when ready to fly.") != 0)
                break;
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(20);
    }
done:
    SDL_strlcpy(failure, SDL_GetError(), sizeof(failure));
    if (carrier.audio != 0)
        SDL_CloseAudioDevice(carrier.audio);
    if (carrier.audioInitialized)
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_SetCursor(savedCursor);
    SDL_FreeCursor(hand);
    SDL_ShowCursor(oldCursorVisible);
    SDL_DestroyTexture(carrier.font);
    for (pack = 0; pack < SWC_ROOM_PACKS; pack++) {
        for (entry = 0; entry < carrier.packs[pack].count; entry++) {
            art = &carrier.packs[pack].art[entry];
            if (art->textures != NULL) {
                for (frame = 0; frame < art->count; frame++)
                    SDL_DestroyTexture(art->textures[frame]);
            }
            SDL_free(art->textures);
            SDL_free(art->bounds);
            SDL_free(art->shape.data);
        }
        SwcCMClose(&carrier.packs[pack].cmf);
    }
    SwcCMClose(&data);
    SDL_free(roster.data);
    SDL_free(carrier.script.data);
    if (result == -1)
        SDL_SetError("SWC carrier: %s", failure);
    return result;
}
