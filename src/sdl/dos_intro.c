/* SDL2 restoration of the Origin FX sequence omitted from Kilrathi Saga. */
#include "wc1.h"

#include "video_internal.h"

#define SDL_PORT_DOS_INTRO_ACTOR_COUNT 10
#define SDL_PORT_DOS_INTRO_TITLE_SECTION_COUNT 12
#define SDL_PORT_DOS_INTRO_FIREWORK_COUNT 30
#define SDL_PORT_DOS_INTRO_INITIAL_FIREWORK_COUNT 5

typedef struct SdlDosIntroActor {
    short x;
    short y;
    short velocityX;
    short velocityY;
    char frames[33];
} SdlDosIntroActor;

typedef struct SdlDosIntroFirework {
    short frame;
    short x;
    short y;
    short variant;
} SdlDosIntroFirework;

/* Coordinates, motion, and animation strings from the DOS VROOMM overlay. */
static const SdlDosIntroActor g_aSdlDosIntroActors[
    SDL_PORT_DOS_INTRO_ACTOR_COUNT] = {
    {  58,  94, -1, 1, "abcdefghijkaakkkkaaaalllllllmmll" },
    { 186,  94,  0, 1, "aaaaaaaabcddeeddccffgghhgghhiiih" },
    { 278,  94,  1, 1, "aaaaaaaaaabbccbbaaccccccccccaccc" },
    {  58, 102, -3, 2, "aaaabbbcccddeeddccffggaaggffcaaa" },
    { 186, 102,  0, 2, "aabbbaacccddddddccddeeeeeeddccca" },
    { 278, 102,  3, 2, "aaaaaabbbaaabbaaaabbbbaabbbbaccc" },
    {  58, 110, -5, 3, "aabbcdbbbeaaaaaaeeaaaaaaaaaabaaa" },
    { 186, 110,  0, 3, "abbbaacccdeeaaeeddaaaaaaaaaadacc" },
    { 278, 110,  5, 3, "aaaaaaaaabbbccbbbbccddddddccaaaa" },
    { 158,  74,  0, 4, "abccdefgghiijjiihhkkllmmllkkhnnn" }
};

static const char g_szSdlDosIntroConductorFrames[] =
    "opoqopoqopoqopoqqrstrq";

static void SdlDrawDosIntroSky(Viewport *viewport,
                                   struct Shape *titleShape,
                                   struct Shape *planetShape,
                                   int planetY)
{
    ClearViewport(viewport, cBlackColour);
    DrawSpriteDefault(viewport, 0, 0, titleShape, 0);
    if (planetShape != 0)
        DrawSpriteDefault(viewport, 160, planetY, planetShape, 0);
}

static void SdlDrawDosIntroLogo(Viewport *viewport,
                                    struct Shape *titleShape,
                                    short y, short scale)
{
    short left;
    short right;

    /* Frame 2 is 118 pixels wide around an origin 57 pixels from its left. */
    left = (161 - 57 * scale / 0x100);
    right = (162 + 61 * scale / 0x100);
    DrawSpriteScaled(viewport, left, y, titleShape, 1, 0, scale, 0);
    DrawSpriteScaled(viewport, 162, y, titleShape, 2, 0, scale, 0);
    DrawSpriteScaled(viewport, right, y, titleShape, 3, 0, scale, 0);
}

static int SdlPresentDosIntroFrame(Viewport *viewport)
{
    Viewport destination;

    /* WC.EXE 13be:064b gives the DOS intro a 320x128 buffer and clips the
     * displayed viewport to screen rows 24-151. */
    destination = stScreenView;
    destination.top = 24;
    destination.bottom = 151;
    CopyViewportContents(viewport, &destination);
    DIBslam();
    DIBslamReal();
    return CheckEscaped() == 0;
}

static int SdlDrawDosIntroOrchestra(
    Viewport *viewport, struct Shape **titleSections,
    struct Shape *planetShape, int sequenceFrame)
{
    const SdlDosIntroActor *actor;
    int actorIndex;
    int frame;

    SdlDrawDosIntroSky(viewport, titleSections[0], planetShape, 24);
    actorIndex = 0;
    while (actorIndex < SDL_PORT_DOS_INTRO_ACTOR_COUNT) {
        actor = &g_aSdlDosIntroActors[actorIndex];
        frame = actor->frames[sequenceFrame] - 'a';
        DrawSpriteDefault(viewport, actor->x, actor->y,
                          titleSections[actorIndex + 1], frame);
        actorIndex++;
    }
    return SdlPresentDosIntroFrame(viewport);
}

static int SdlDrawDosIntroConductorCue(
    Viewport *viewport, struct Shape **titleSections,
    struct Shape *planetShape, int cueFrame)
{
    const SdlDosIntroActor *actor;
    int actorIndex;
    int frame;

    SdlDrawDosIntroSky(viewport, titleSections[0], planetShape, 24);
    actorIndex = 0;
    while (actorIndex < SDL_PORT_DOS_INTRO_ACTOR_COUNT - 1) {
        actor = &g_aSdlDosIntroActors[actorIndex];
        frame = actor->frames[31] - 'a';
        DrawSpriteDefault(viewport, actor->x, actor->y,
                          titleSections[actorIndex + 1], frame);
        actorIndex++;
    }
    actor = &g_aSdlDosIntroActors[actorIndex];
    frame = g_szSdlDosIntroConductorFrames[cueFrame] - 'a';
    DrawSpriteDefault(viewport, actor->x, actor->y,
                      titleSections[actorIndex + 1], frame);
    return SdlPresentDosIntroFrame(viewport);
}

static int SdlDrawDosIntroOrchestraPush(
    Viewport *viewport, struct Shape **titleSections,
    struct Shape *planetShape, int distance)
{
    const SdlDosIntroActor *actor;
    int actorIndex;
    int frame;
    int scale;
    int x;
    int y;

    SdlDrawDosIntroSky(viewport, titleSections[0], planetShape, 24);
    actorIndex = 0;
    while (actorIndex < SDL_PORT_DOS_INTRO_ACTOR_COUNT) {
        actor = &g_aSdlDosIntroActors[actorIndex];
        if (actorIndex == SDL_PORT_DOS_INTRO_ACTOR_COUNT - 1) {
            frame = g_szSdlDosIntroConductorFrames[21] - 'a';
        } else {
            frame = actor->frames[31] - 'a';
        }
        x = actor->x + actor->velocityX * distance;
        y = actor->y + actor->velocityY * distance;
        scale = 0x100 + actor->velocityY * distance * 4;
        DrawSpriteTransformed(viewport, x, y,
                              titleSections[actorIndex + 1], frame,
                              0, scale, scale, 0, 0);
        actorIndex++;
    }
    return SdlPresentDosIntroFrame(viewport);
}

static int SdlDrawDosIntroLogoReveal(
    Viewport *viewport, struct Shape *titleShape,
    struct Shape *planetShape, short logoY, int distance)
{
    int logoBottom;
    int planetY;
    int scale;

    planetY = 120000 / distance;
    scale = 256000 / distance;
    logoBottom = logoY + 34 * scale / 0x100;
    ClearViewport(viewport, cBlackColour);
    DrawSpriteDefault(viewport, 0, 0, titleShape, 0);
    if (logoBottom < planetY) {
        DrawSpriteDefault(viewport, 160, planetY, planetShape, 0);
        SdlDrawDosIntroLogo(viewport, titleShape, logoY, scale);
    } else {
        SdlDrawDosIntroLogo(viewport, titleShape, logoY, scale);
        DrawSpriteDefault(viewport, 160, planetY, planetShape, 0);
    }
    return SdlPresentDosIntroFrame(viewport);
}

static void SdlResetDosIntroFireworks(
    SdlDosIntroFirework *fireworks)
{
    int index;

    index = 0;
    while (index < SDL_PORT_DOS_INTRO_FIREWORK_COUNT) {
        fireworks[index].frame = -1;
        index++;
    }
}

static void SdlStartDosIntroFirework(
    SdlDosIntroFirework *fireworks, int count)
{
    int index;

    index = 0;
    while (index < count && fireworks[index].frame != -1)
        index++;
    if (index == count)
        return;
    fireworks[index].frame = 0;
    fireworks[index].x = RandomInRange(0, 319);
    fireworks[index].y = RandomInRange(0, 127);
    fireworks[index].variant = RandomInRange(0, 2);
}

static int SdlDrawDosIntroFireworks(
    Viewport *viewport, struct Shape *titleShape,
    struct Shape *fireworkShape, SdlDosIntroFirework *fireworks,
    short logoY)
{
    SdlDosIntroFirework *firework;
    int index;

    ClearViewport(viewport, cBlackColour);
    DrawSpriteDefault(viewport, 0, 0, titleShape, 0);
    SdlDrawDosIntroLogo(viewport, titleShape, logoY, 0x100);
    index = 0;
    while (index < SDL_PORT_DOS_INTRO_FIREWORK_COUNT) {
        firework = &fireworks[index];
        if (firework->frame >= 0) {
            DrawSpriteDefault(viewport, firework->x, firework->y,
                              fireworkShape,
                              (firework->frame +
                                      firework->variant * 8));
            firework->frame++;
            if (firework->frame == 8)
                firework->frame = -1;
        }
        index++;
    }
    return SdlPresentDosIntroFrame(viewport);
}

void SdlPlayDosStartupIntro(void)
{
    struct Shape *titleSections[SDL_PORT_DOS_INTRO_TITLE_SECTION_COUNT];
    struct Shape *planetShape;
    SdlDosIntroFirework fireworks[SDL_PORT_DOS_INTRO_FIREWORK_COUNT];
    Viewport introViewport;
    int actorDirection;
    int actorFrame;
    int actorMinimumFrame;
    int cueFrame;
    int distance;
    int emptyFireworks;
    int fireworkFrame;
    int fireworkIndex;
    int finishingFireworks;
    int introMusic;
    short logoY;
    int musicPosition;
    int previousMusicTrack;
    int running;
    int sectionIndex;
    int synchronizedMusic;

    if (SdlUsingTownsData() || (!SdlUsingDosData() && !SdlUsingGlRenderer()))
        return;

    memset(&introViewport, 0, sizeof(introViewport));
    introViewport.left = 0;
    introViewport.top = 0;
    introViewport.right = 319;
    introViewport.bottom = 127;
    if (AllocateViewport(&introViewport, cBlackColour, 0) == 0) {
        free_viewport(&introViewport);
        return;
    }

    memset(titleSections, 0, sizeof(titleSections));
    planetShape = 0;
    sectionIndex = 0;
    while (sectionIndex < SDL_PORT_DOS_INTRO_TITLE_SECTION_COUNT) {
        titleSections[sectionIndex] =
            FetchDiskPacketRetrying(9, (sectionIndex + 6), 0);
        sectionIndex++;
    }
    planetShape = FetchDiskPacketRetrying(9, 3, 0);

    DrawFilledViewportRect(&stScreenView, 0, 0, 319, 199,
                           cBlackColour);
    ClearInputKeyStatePreservingModifiers();
    FlushInputEvents();
    previousMusicTrack = nCurrentMusicTrack;
    introMusic = nMusicPlaybackMode != 0 && nMusicPlaybackMode != 3;
    if (introMusic) {
        nCurrentMusicTrack = 19;
        SdlServiceOriginFxMusic();
    }
    synchronizedMusic = introMusic &&
        SdlGetOriginFxMusicSequencePosition() >= 0;

    running = SdlDrawDosIntroOrchestra(
        &introViewport, titleSections, planetShape, 0);
    if (synchronizedMusic) {
        actorDirection = 1;
        actorFrame = 0;
        actorMinimumFrame = 0;
        musicPosition = SdlGetOriginFxMusicSequencePosition();
        while (running && musicPosition >= 0 && musicPosition < 1) {
            running = SdlDrawDosIntroOrchestra(
                &introViewport, titleSections, planetShape, actorFrame);
            if (actorDirection > 0) {
                actorFrame++;
                if (actorFrame == 32) {
                    actorMinimumFrame = RandomInRange(0, 13) + 9;
                    actorFrame = 31;
                    actorDirection = -1;
                }
            } else {
                actorFrame--;
                if (actorFrame < actorMinimumFrame) {
                    actorFrame = actorMinimumFrame;
                    actorDirection = 1;
                }
            }
            musicPosition = SdlGetOriginFxMusicSequencePosition();
        }
        if (musicPosition < 0)
            synchronizedMusic = 0;
    }
    if (!synchronizedMusic) {
        actorFrame = 0;
        while (running && actorFrame < 32) {
            running = SdlDrawDosIntroOrchestra(
                &introViewport, titleSections, planetShape, actorFrame);
            actorFrame++;
        }
        actorFrame = 31;
        while (running && actorFrame >= 12) {
            running = SdlDrawDosIntroOrchestra(
                &introViewport, titleSections, planetShape, actorFrame);
            actorFrame--;
        }
        actorFrame = 12;
        while (running && actorFrame < 32) {
            running = SdlDrawDosIntroOrchestra(
                &introViewport, titleSections, planetShape, actorFrame);
            actorFrame++;
        }
    }

    cueFrame = 0;
    while (running && cueFrame < 20) {
        running = SdlDrawDosIntroConductorCue(
            &introViewport, titleSections, planetShape, cueFrame);
        cueFrame++;
    }
    if (running && synchronizedMusic) {
        musicPosition = SdlGetOriginFxMusicSequencePosition();
        while (running && musicPosition >= 0 && musicPosition < 2) {
            if (CheckEscaped() != 0) {
                running = 0;
            } else {
                SdlSleep(1);
                musicPosition = SdlGetOriginFxMusicSequencePosition();
            }
        }
        if (musicPosition < 0) {
            synchronizedMusic = 0;
        } else if (running) {
            running = SdlDrawDosIntroConductorCue(
                &introViewport, titleSections, planetShape, 20);
        }
    }

    distance = 1;
    while (running && distance < 120) {
        running = SdlDrawDosIntroOrchestraPush(
            &introViewport, titleSections, planetShape, distance);
        distance += distance / 4 + 1;
    }

    logoY = 59;
    distance = 5000;
    while (running && distance >= 1000) {
        if (distance > 3000)
            logoY -= 2;
        else
            logoY += 2;
        running = SdlDrawDosIntroLogoReveal(
            &introViewport, titleSections[0], planetShape, logoY,
            distance);
        distance -= 100;
    }
    if (running && synchronizedMusic) {
        musicPosition = SdlGetOriginFxMusicSequencePosition();
        while (running && musicPosition >= 0 && musicPosition < 3) {
            if (CheckEscaped() != 0) {
                running = 0;
            } else {
                SdlSleep(1);
                musicPosition = SdlGetOriginFxMusicSequencePosition();
            }
        }
        if (musicPosition < 0)
            synchronizedMusic = 0;
    }

    SdlResetDosIntroFireworks(fireworks);
    fireworkFrame = 0;
    finishingFireworks = 0;
    while (running) {
        musicPosition = synchronizedMusic
            ? SdlGetOriginFxMusicSequencePosition() : -1;
        if (synchronizedMusic && musicPosition < 0)
            synchronizedMusic = 0;
        if (!finishingFireworks) {
            if (!synchronizedMusic || musicPosition >= 4 ||
                RandomInRange(0, 5) == 0) {
                SdlStartDosIntroFirework(
                    fireworks,
                    SDL_PORT_DOS_INTRO_INITIAL_FIREWORK_COUNT);
            }
        }
        running = SdlDrawDosIntroFireworks(
            &introViewport, titleSections[0], titleSections[11],
            fireworks, logoY);
        fireworkFrame++;
        if (synchronizedMusic) {
            musicPosition = SdlGetOriginFxMusicSequencePosition();
            if (musicPosition >= 5)
                finishingFireworks = 1;
        } else if (fireworkFrame > 10) {
            finishingFireworks = 1;
        }
        if (finishingFireworks) {
            emptyFireworks = 0;
            fireworkIndex = 0;
            while (fireworkIndex <
                   SDL_PORT_DOS_INTRO_INITIAL_FIREWORK_COUNT) {
                if (fireworks[fireworkIndex].frame == -1)
                    emptyFireworks++;
                fireworkIndex++;
            }
            if (emptyFireworks ==
                SDL_PORT_DOS_INTRO_INITIAL_FIREWORK_COUNT)
                break;
        }
    }
    if (running) {
        fireworkIndex = 0;
        while (fireworkIndex < SDL_PORT_DOS_INTRO_FIREWORK_COUNT) {
            fireworks[fireworkIndex].frame = 0;
            fireworks[fireworkIndex].x = RandomInRange(0, 319);
            fireworks[fireworkIndex].y = RandomInRange(0, 127);
            fireworks[fireworkIndex].variant = RandomInRange(0, 2);
            fireworkIndex++;
        }
        fireworkFrame = 0;
        while (running && fireworkFrame < 8) {
            running = SdlDrawDosIntroFireworks(
                &introViewport, titleSections[0], titleSections[11],
                fireworks, logoY);
            fireworkFrame++;
        }
    }

    if (introMusic) {
        nCurrentMusicTrack = previousMusicTrack;
        SdlServiceOriginFxMusic();
    }
    ClearViewport(&stScreenView, cBlackColour);
    free_viewport(&introViewport);
    ReleasePacketHandle(planetShape);
    sectionIndex = SDL_PORT_DOS_INTRO_TITLE_SECTION_COUNT;
    while (sectionIndex > 0) {
        sectionIndex--;
        ReleasePacketHandle(titleSections[sectionIndex]);
    }
    ClearInputKeyStatePreservingModifiers();
    FlushInputEvents();
}
