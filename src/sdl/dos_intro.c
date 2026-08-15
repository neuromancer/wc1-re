/* SDL2 restoration of the Origin FX sequence omitted from Kilrathi Saga. */
#include "wc1.h"

#include "video_internal.h"

#define WC1_SDL_DOS_INTRO_ACTOR_COUNT 10
#define WC1_SDL_DOS_INTRO_TITLE_SECTION_COUNT 12
#define WC1_SDL_DOS_INTRO_FIREWORK_COUNT 30
#define WC1_SDL_DOS_INTRO_INITIAL_FIREWORK_COUNT 5

typedef struct Wc1SdlDosIntroActor {
    short x;
    short y;
    short velocityX;
    short velocityY;
    char frames[33];
} Wc1SdlDosIntroActor;

typedef struct Wc1SdlDosIntroFirework {
    short frame;
    short x;
    short y;
    short variant;
} Wc1SdlDosIntroFirework;

/* Coordinates, motion, and animation strings from the DOS VROOMM overlay. */
static const Wc1SdlDosIntroActor g_aWc1SdlDosIntroActors[
    WC1_SDL_DOS_INTRO_ACTOR_COUNT] = {
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

static const char g_szWc1SdlDosIntroConductorFrames[] =
    "opoqopoqopoqopoqqrstrq";

static void Wc1SdlDrawDosIntroSky(Viewport *viewport,
                                   unsigned char *titleShape,
                                   unsigned char *planetShape,
                                   int planetY)
{
    ClearViewport(viewport, (unsigned char)DAT_0046999c);
    DrawSpriteDefault(viewport, 0, 0, titleShape, 0);
    if (planetShape != 0)
        DrawSpriteDefault(viewport, 160, (short)planetY, planetShape, 0);
}

static void Wc1SdlDrawDosIntroLogo(Viewport *viewport,
                                    unsigned char *titleShape,
                                    short y, short scale)
{
    short left;
    short right;

    /* Frame 2 is 118 pixels wide around an origin 57 pixels from its left. */
    left = (short)(161 - 57 * scale / 0x100);
    right = (short)(162 + 61 * scale / 0x100);
    DrawSpriteScaled(viewport, left, y, titleShape, 1, 0, scale, 0);
    DrawSpriteScaled(viewport, 162, y, titleShape, 2, 0, scale, 0);
    DrawSpriteScaled(viewport, right, y, titleShape, 3, 0, scale, 0);
}

static int Wc1SdlPresentDosIntroFrame(Viewport *viewport)
{
    Viewport destination;

    /* WC.EXE 13be:064b gives the DOS intro a 320x128 buffer and clips the
     * displayed viewport to screen rows 24-151. */
    destination = DAT_005a6ba0;
    destination.top = 24;
    destination.bottom = 151;
    CopyViewportContents(viewport, &destination);
    DIBslam();
    DIBslamReal();
    return CheckEscaped() == 0;
}

static int Wc1SdlDrawDosIntroOrchestra(
    Viewport *viewport, unsigned char **titleSections,
    unsigned char *planetShape, int sequenceFrame)
{
    const Wc1SdlDosIntroActor *actor;
    int actorIndex;
    int frame;

    Wc1SdlDrawDosIntroSky(viewport, titleSections[0], planetShape, 24);
    actorIndex = 0;
    while (actorIndex < WC1_SDL_DOS_INTRO_ACTOR_COUNT) {
        actor = &g_aWc1SdlDosIntroActors[actorIndex];
        frame = actor->frames[sequenceFrame] - 'a';
        DrawSpriteDefault(viewport, actor->x, actor->y,
                          titleSections[actorIndex + 1], (short)frame);
        actorIndex++;
    }
    return Wc1SdlPresentDosIntroFrame(viewport);
}

static int Wc1SdlDrawDosIntroConductorCue(
    Viewport *viewport, unsigned char **titleSections,
    unsigned char *planetShape, int cueFrame)
{
    const Wc1SdlDosIntroActor *actor;
    int actorIndex;
    int frame;

    Wc1SdlDrawDosIntroSky(viewport, titleSections[0], planetShape, 24);
    actorIndex = 0;
    while (actorIndex < WC1_SDL_DOS_INTRO_ACTOR_COUNT - 1) {
        actor = &g_aWc1SdlDosIntroActors[actorIndex];
        frame = actor->frames[31] - 'a';
        DrawSpriteDefault(viewport, actor->x, actor->y,
                          titleSections[actorIndex + 1], (short)frame);
        actorIndex++;
    }
    actor = &g_aWc1SdlDosIntroActors[actorIndex];
    frame = g_szWc1SdlDosIntroConductorFrames[cueFrame] - 'a';
    DrawSpriteDefault(viewport, actor->x, actor->y,
                      titleSections[actorIndex + 1], (short)frame);
    return Wc1SdlPresentDosIntroFrame(viewport);
}

static int Wc1SdlDrawDosIntroOrchestraPush(
    Viewport *viewport, unsigned char **titleSections,
    unsigned char *planetShape, int distance)
{
    const Wc1SdlDosIntroActor *actor;
    int actorIndex;
    int frame;
    int scale;
    int x;
    int y;

    Wc1SdlDrawDosIntroSky(viewport, titleSections[0], planetShape, 24);
    actorIndex = 0;
    while (actorIndex < WC1_SDL_DOS_INTRO_ACTOR_COUNT) {
        actor = &g_aWc1SdlDosIntroActors[actorIndex];
        if (actorIndex == WC1_SDL_DOS_INTRO_ACTOR_COUNT - 1) {
            frame = g_szWc1SdlDosIntroConductorFrames[21] - 'a';
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
    return Wc1SdlPresentDosIntroFrame(viewport);
}

static int Wc1SdlDrawDosIntroLogoReveal(
    Viewport *viewport, unsigned char *titleShape,
    unsigned char *planetShape, short logoY, int distance)
{
    int logoBottom;
    int planetY;
    int scale;

    planetY = 120000 / distance;
    scale = 256000 / distance;
    logoBottom = logoY + 34 * scale / 0x100;
    ClearViewport(viewport, (unsigned char)DAT_0046999c);
    DrawSpriteDefault(viewport, 0, 0, titleShape, 0);
    if (logoBottom < planetY) {
        DrawSpriteDefault(viewport, 160, (short)planetY, planetShape, 0);
        Wc1SdlDrawDosIntroLogo(viewport, titleShape, logoY, (short)scale);
    } else {
        Wc1SdlDrawDosIntroLogo(viewport, titleShape, logoY, (short)scale);
        DrawSpriteDefault(viewport, 160, (short)planetY, planetShape, 0);
    }
    return Wc1SdlPresentDosIntroFrame(viewport);
}

static void Wc1SdlResetDosIntroFireworks(
    Wc1SdlDosIntroFirework *fireworks)
{
    int index;

    index = 0;
    while (index < WC1_SDL_DOS_INTRO_FIREWORK_COUNT) {
        fireworks[index].frame = -1;
        index++;
    }
}

static void Wc1SdlStartDosIntroFirework(
    Wc1SdlDosIntroFirework *fireworks, int count)
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

static int Wc1SdlDrawDosIntroFireworks(
    Viewport *viewport, unsigned char *titleShape,
    unsigned char *fireworkShape, Wc1SdlDosIntroFirework *fireworks,
    short logoY)
{
    Wc1SdlDosIntroFirework *firework;
    int index;

    ClearViewport(viewport, (unsigned char)DAT_0046999c);
    DrawSpriteDefault(viewport, 0, 0, titleShape, 0);
    Wc1SdlDrawDosIntroLogo(viewport, titleShape, logoY, 0x100);
    index = 0;
    while (index < WC1_SDL_DOS_INTRO_FIREWORK_COUNT) {
        firework = &fireworks[index];
        if (firework->frame >= 0) {
            DrawSpriteDefault(viewport, firework->x, firework->y,
                              fireworkShape,
                              (short)(firework->frame +
                                      firework->variant * 8));
            firework->frame++;
            if (firework->frame == 8)
                firework->frame = -1;
        }
        index++;
    }
    return Wc1SdlPresentDosIntroFrame(viewport);
}

void Wc1SdlPlayDosStartupIntro(void)
{
    unsigned char *titleSections[WC1_SDL_DOS_INTRO_TITLE_SECTION_COUNT];
    unsigned char *planetShape;
    Wc1SdlDosIntroFirework fireworks[WC1_SDL_DOS_INTRO_FIREWORK_COUNT];
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

    if (!Wc1SdlUsingDosData() && !Wc1SdlUsingGlRenderer())
        return;

    memset(&introViewport, 0, sizeof(introViewport));
    introViewport.left = 0;
    introViewport.top = 0;
    introViewport.right = 319;
    introViewport.bottom = 127;
    if (AllocateViewport(&introViewport, (short)DAT_0046999c, 0) == 0) {
        free_viewport(&introViewport);
        return;
    }

    memset(titleSections, 0, sizeof(titleSections));
    planetShape = 0;
    sectionIndex = 0;
    while (sectionIndex < WC1_SDL_DOS_INTRO_TITLE_SECTION_COUNT) {
        titleSections[sectionIndex] =
            FetchDiskPacketRetrying(9, (short)(sectionIndex + 6), 0);
        sectionIndex++;
    }
    planetShape = FetchDiskPacketRetrying(9, 3, 0);

    DrawFilledViewportRect(&DAT_005a6ba0, 0, 0, 319, 199,
                           (short)DAT_0046999c);
    ClearInputKeyStatePreservingModifiers();
    FlushInputEvents();
    previousMusicTrack = g_nCurrentMusicTrack_0046aa14;
    introMusic = Wc1SdlUsingDosData() &&
                 DAT_0046a9f8 != 0 && DAT_0046a9f8 != 3;
    if (introMusic) {
        g_nCurrentMusicTrack_0046aa14 = 19;
        Wc1SdlServiceDosAdlibMusic();
    }
    synchronizedMusic = introMusic &&
        Wc1SdlGetDosMusicSequencePosition() >= 0;

    running = Wc1SdlDrawDosIntroOrchestra(
        &introViewport, titleSections, planetShape, 0);
    if (synchronizedMusic) {
        actorDirection = 1;
        actorFrame = 0;
        actorMinimumFrame = 0;
        musicPosition = Wc1SdlGetDosMusicSequencePosition();
        while (running && musicPosition >= 0 && musicPosition < 1) {
            running = Wc1SdlDrawDosIntroOrchestra(
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
            musicPosition = Wc1SdlGetDosMusicSequencePosition();
        }
        if (musicPosition < 0)
            synchronizedMusic = 0;
    }
    if (!synchronizedMusic) {
        actorFrame = 0;
        while (running && actorFrame < 32) {
            running = Wc1SdlDrawDosIntroOrchestra(
                &introViewport, titleSections, planetShape, actorFrame);
            actorFrame++;
        }
        actorFrame = 31;
        while (running && actorFrame >= 12) {
            running = Wc1SdlDrawDosIntroOrchestra(
                &introViewport, titleSections, planetShape, actorFrame);
            actorFrame--;
        }
        actorFrame = 12;
        while (running && actorFrame < 32) {
            running = Wc1SdlDrawDosIntroOrchestra(
                &introViewport, titleSections, planetShape, actorFrame);
            actorFrame++;
        }
    }

    cueFrame = 0;
    while (running && cueFrame < 20) {
        running = Wc1SdlDrawDosIntroConductorCue(
            &introViewport, titleSections, planetShape, cueFrame);
        cueFrame++;
    }
    if (running && synchronizedMusic) {
        musicPosition = Wc1SdlGetDosMusicSequencePosition();
        while (running && musicPosition >= 0 && musicPosition < 2) {
            if (CheckEscaped() != 0) {
                running = 0;
            } else {
                Wc1SdlSleep(1);
                musicPosition = Wc1SdlGetDosMusicSequencePosition();
            }
        }
        if (musicPosition < 0) {
            synchronizedMusic = 0;
        } else if (running) {
            running = Wc1SdlDrawDosIntroConductorCue(
                &introViewport, titleSections, planetShape, 20);
        }
    }

    distance = 1;
    while (running && distance < 120) {
        running = Wc1SdlDrawDosIntroOrchestraPush(
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
        running = Wc1SdlDrawDosIntroLogoReveal(
            &introViewport, titleSections[0], planetShape, logoY,
            distance);
        distance -= 100;
    }
    if (running && synchronizedMusic) {
        musicPosition = Wc1SdlGetDosMusicSequencePosition();
        while (running && musicPosition >= 0 && musicPosition < 3) {
            if (CheckEscaped() != 0) {
                running = 0;
            } else {
                Wc1SdlSleep(1);
                musicPosition = Wc1SdlGetDosMusicSequencePosition();
            }
        }
        if (musicPosition < 0)
            synchronizedMusic = 0;
    }

    Wc1SdlResetDosIntroFireworks(fireworks);
    fireworkFrame = 0;
    finishingFireworks = 0;
    while (running) {
        musicPosition = synchronizedMusic
            ? Wc1SdlGetDosMusicSequencePosition() : -1;
        if (synchronizedMusic && musicPosition < 0)
            synchronizedMusic = 0;
        if (!finishingFireworks) {
            if (!synchronizedMusic || musicPosition >= 4 ||
                RandomInRange(0, 5) == 0) {
                Wc1SdlStartDosIntroFirework(
                    fireworks,
                    WC1_SDL_DOS_INTRO_INITIAL_FIREWORK_COUNT);
            }
        }
        running = Wc1SdlDrawDosIntroFireworks(
            &introViewport, titleSections[0], titleSections[11],
            fireworks, logoY);
        fireworkFrame++;
        if (synchronizedMusic) {
            musicPosition = Wc1SdlGetDosMusicSequencePosition();
            if (musicPosition >= 5)
                finishingFireworks = 1;
        } else if (fireworkFrame > 10) {
            finishingFireworks = 1;
        }
        if (finishingFireworks) {
            emptyFireworks = 0;
            fireworkIndex = 0;
            while (fireworkIndex <
                   WC1_SDL_DOS_INTRO_INITIAL_FIREWORK_COUNT) {
                if (fireworks[fireworkIndex].frame == -1)
                    emptyFireworks++;
                fireworkIndex++;
            }
            if (emptyFireworks ==
                WC1_SDL_DOS_INTRO_INITIAL_FIREWORK_COUNT)
                break;
        }
    }
    if (running) {
        fireworkIndex = 0;
        while (fireworkIndex < WC1_SDL_DOS_INTRO_FIREWORK_COUNT) {
            fireworks[fireworkIndex].frame = 0;
            fireworks[fireworkIndex].x = RandomInRange(0, 319);
            fireworks[fireworkIndex].y = RandomInRange(0, 127);
            fireworks[fireworkIndex].variant = RandomInRange(0, 2);
            fireworkIndex++;
        }
        fireworkFrame = 0;
        while (running && fireworkFrame < 8) {
            running = Wc1SdlDrawDosIntroFireworks(
                &introViewport, titleSections[0], titleSections[11],
                fireworks, logoY);
            fireworkFrame++;
        }
    }

    if (introMusic) {
        g_nCurrentMusicTrack_0046aa14 = previousMusicTrack;
        Wc1SdlServiceDosAdlibMusic();
    }
    ClearViewport(&DAT_005a6ba0, (unsigned char)DAT_0046999c);
    free_viewport(&introViewport);
    ReleasePacketHandle(planetShape);
    sectionIndex = WC1_SDL_DOS_INTRO_TITLE_SECTION_COUNT;
    while (sectionIndex > 0) {
        sectionIndex--;
        ReleasePacketHandle(titleSections[sectionIndex]);
    }
    ClearInputKeyStatePreservingModifiers();
    FlushInputEvents();
}
