/*
 *  Nav map, location readouts and the virtual screen.
 *
 *  Address range 0x40d000-0x40ffff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: DrawNav* family; string band 0x4687AC-0x4688F4.
 */
#include "wc1.h"

typedef struct IntroArchive {
    unsigned char *data;
    unsigned int size;
    short sectionCount;
    unsigned char *sections[64];
    unsigned int sectionSizes[64];
} IntroArchive;

static const unsigned char g_abIntroFontWidths[60] = {
    13, 13, 13, 13, 12, 10, 13, 13, 5, 11, 13, 10, 17,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 17, 13, 13, 11,
    0, 0, 0, 0, 0, 0,
    11, 11, 11, 11, 11, 9, 11, 11, 5, 7, 11, 5, 15,
    11, 11, 11, 11, 9, 10, 7, 11, 11, 15, 11, 11, 13,
    5, 7
};

static const char *g_aszIntroCredits[] = {
    "Design\nby\nChris Roberts",
    "Software Engineers\nChris Roberts\nKen Demarest III\nPaul C. Isaac\nSteve Muchow\nHerman Miller\nSteve Beeman",
    "Dogfight Intelligence\nKen Demarest III\n\nDogfight Choreography\nSteve Beeman\nErin Roberts",
    "3Space System\nby\nChris Roberts\n\nOriginFX Graphic System\nChris Roberts\nJohn Miles",
    "OriginFX Sound System\nby\nHerman Miller",
    "Artwork\nDenis Loubet\nGlen Johnson\nDaniel Bourbonnais\nKeith Berdak\nJohn Watson",
    "Screenplay by Jeff George\n\nAdditional Writing\nSteve Cantrell\nPhilip Brogden",
    "Soundtrack by\nGeorge A. Sanger and Dave Govett",
    "Sound Effects by Marc Schaefgen",
    "Produced by\nChris Roberts and Warren Spector",
    "Directed by\nChris Roberts"
};

static unsigned int ReadIntroDword(const unsigned char *p)
{
    return (unsigned int)p[0] |
           (unsigned int)p[1] << 8 |
           (unsigned int)p[2] << 16 |
           (unsigned int)p[3] << 24;
}

static int LoadIntroArchive(const char *name, IntroArchive *archive)
{
    FILE *file;
    long fileSize;
    unsigned int declaredSize;
    unsigned int firstOffset;
    short count;
    short i;

    memset(archive, 0, sizeof(*archive));
    file = fopen(name, "rb");
    if (file == 0)
        return 0;
    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (fileSize < 8 || fileSize > 0x1000000L) {
        fclose(file);
        return 0;
    }

    archive->data = (unsigned char *)malloc((unsigned int)fileSize);
    if (archive->data == 0) {
        fclose(file);
        return 0;
    }
    if (fread(archive->data, 1, (unsigned int)fileSize, file) !=
        (unsigned int)fileSize) {
        fclose(file);
        free(archive->data);
        memset(archive, 0, sizeof(*archive));
        return 0;
    }
    fclose(file);

    declaredSize = ReadIntroDword(archive->data);
    firstOffset = ReadIntroDword(archive->data + 4) & 0xffffff;
    if (declaredSize > (unsigned int)fileSize || firstOffset < 8 ||
        (firstOffset & 3) != 0) {
        free(archive->data);
        memset(archive, 0, sizeof(*archive));
        return 0;
    }

    count = (short)(firstOffset / 4 - 1);
    if (count < 1 || count > 64) {
        free(archive->data);
        memset(archive, 0, sizeof(*archive));
        return 0;
    }
    archive->size = declaredSize;
    archive->sectionCount = count;
    i = 0;
    do {
        unsigned int start = ReadIntroDword(archive->data + 4 + i * 4) & 0xffffff;
        unsigned int end;

        if (i + 1 < count)
            end = ReadIntroDword(archive->data + 8 + i * 4) & 0xffffff;
        else
            end = declaredSize;
        if (start < firstOffset || start > end || end > declaredSize) {
            free(archive->data);
            memset(archive, 0, sizeof(*archive));
            return 0;
        }
        archive->sections[i] = archive->data + start;
        archive->sectionSizes[i] = end - start;
        i = i + 1;
    } while (i < count);
    return 1;
}

static void UnloadIntroArchive(IntroArchive *archive)
{
    if (archive->data != 0)
        free(archive->data);
    memset(archive, 0, sizeof(*archive));
}

static int LoadIntroPalette(unsigned char *palette)
{
    FILE *file = fopen("GAME.PAL", "rb");
    int loaded = 0;

    if (file != 0) {
        if (fseek(file, 0x30, SEEK_SET) == 0 &&
            fread(palette, 1, 0x300, file) == 0x300)
            loaded = 1;
        fclose(file);
    }
    return loaded;
}

static void SetIntroPaletteLevel(const unsigned char *palette, short level)
{
    unsigned char scaled[0x300];
    short i = 0;

    do {
        scaled[i] = (unsigned char)((unsigned int)palette[i] * level >> 8);
        i = i + 1;
    } while (i < 0x300);
    DIBwholePaletteFromTriplets(scaled);
}

static int PresentIntroFrame(unsigned int delay)
{
    DWORD until;

    CopyViewportContents(&DAT_005a7510, &DAT_005a6ba0);
    DIBslamReal();
    until = timeGetTime() + delay;
    do {
        if (PumpWindowMessages() == 0)
            return 0;
        if (DAT_0059ab58 != 0 || DAT_005a8964 != 0)
            return 0;
        Sleep(1);
    } while ((long)(until - timeGetTime()) > 0);
    return 1;
}

static int FadeIntroPalette(const unsigned char *palette, int fadeIn)
{
    short step = 0;

    do {
        short level = fadeIn ? step * 16 : 0x100 - step * 16;

        SetIntroPaletteLevel(palette, level);
        if (!PresentIntroFrame(20))
            return 0;
        step = step + 1;
    } while (step < 17);
    return 1;
}

static void ClearIntroFrame(void)
{
    ClearViewport(&DAT_005a7510, 0);
}

static void DrawIntroShape(const IntroArchive *archive, short section,
                           short frame, short x, short y)
{
    unsigned char *shape;
    unsigned short tableEnd;

    if (section < 0 || section >= archive->sectionCount)
        return;
    if (archive->sectionSizes[section] < 8)
        return;
    shape = archive->sections[section];
    tableEnd = *(unsigned short *)(shape + 4);
    if (frame < 0 || frame * 4 + 4 >= tableEnd)
        return;
    DrawSpriteDefault(&DAT_005a7510, x, y, shape, frame);
}

static int IntroTextLineWidth(const char *text)
{
    int width = 0;

    while (*text != 0 && *text != '\n') {
        unsigned char c = (unsigned char)*text++;

        if (c == ' ')
            width += 6;
        else if (c == '.')
            width += g_abIntroFontWidths[58];
        else if (c == ',')
            width += g_abIntroFontWidths[59];
        else if (c >= 'A' && c <= 'z' &&
                 g_abIntroFontWidths[c - 'A'] != 0)
            width += g_abIntroFontWidths[c - 'A'];
        else
            width += 6;
    }
    return width;
}

static void DrawCenteredIntroText(const IntroArchive *archive, const char *text)
{
    const unsigned char *font;
    const char *p;
    short lineCount = 1;
    short y;
    short x;

    if (archive->sectionCount < 2)
        return;
    font = archive->sections[1];
    p = text;
    while (*p != 0) {
        if (*p == '\n')
            lineCount = lineCount + 1;
        p = p + 1;
    }
    y = (short)((200 - lineCount * 16) / 2);
    p = text;
    x = (short)((320 - IntroTextLineWidth(p)) / 2);
    while (*p != 0) {
        unsigned char c = (unsigned char)*p++;
        short frame = -1;
        short advance = 6;

        if (c == '\n') {
            y = y + 16;
            x = (short)((320 - IntroTextLineWidth(p)) / 2);
            continue;
        }
        if (c == '.')
            frame = 58;
        else if (c == ',')
            frame = 59;
        else if (c >= 'A' && c <= 'z' &&
                 g_abIntroFontWidths[c - 'A'] != 0)
            frame = (short)(c - 'A');
        if (frame >= 0) {
            advance = g_abIntroFontWidths[frame];
            DrawSpriteDefault(&DAT_005a7510, x, y,
                              (unsigned char *)font, frame);
        }
        x = x + advance;
    }
}

static int StartIntroMusic(char *temporaryName, int *opened)
{
    IntroArchive music;
    char temporaryPath[MAX_PATH];
    char command[MAX_PATH * 2];
    FILE *file;

    *opened = 0;
    temporaryName[0] = 0;
    if (!LoadIntroArchive("MUSIC.MID", &music))
        return 0;
    if (music.sectionCount <= 0x17 ||
        GetTempPathA(MAX_PATH, temporaryPath) == 0 ||
        GetTempFileNameA(temporaryPath, "WCI", 0, temporaryName) == 0) {
        UnloadIntroArchive(&music);
        return 0;
    }

    file = fopen(temporaryName, "wb");
    if (file == 0 ||
        fwrite(music.sections[0x17], 1, music.sectionSizes[0x17], file) !=
        music.sectionSizes[0x17]) {
        if (file != 0)
            fclose(file);
        DeleteFileA(temporaryName);
        temporaryName[0] = 0;
        UnloadIntroArchive(&music);
        return 0;
    }
    fclose(file);
    UnloadIntroArchive(&music);

    sprintf(command, "open \"%s\" type sequencer alias WCIntro", temporaryName);
    if (mciSendStringA(command, 0, 0, 0) != 0) {
        DeleteFileA(temporaryName);
        temporaryName[0] = 0;
        return 0;
    }
    *opened = 1;
    mciSendStringA("play WCIntro from 0", 0, 0, 0);
    return 1;
}

static void StopIntroMusic(char *temporaryName, int opened)
{
    if (opened) {
        mciSendStringA("stop WCIntro", 0, 0, 0);
        mciSendStringA("close WCIntro", 0, 0, 0);
    }
    if (temporaryName[0] != 0)
        DeleteFileA(temporaryName);
}

/* Function start: 0x40D1D0 */
void ClearNavHazardFlag(void)
{
    DAT_00468710 = 0;
}

/* Function start: 0x40D240 */
void ClearNavLegendFlag(void)
{
    DAT_0046870c = 0;
}

/* Function start: 0x40D8C0 */
void SetScreenClipRect(unsigned short a, unsigned short b, unsigned short c, unsigned short d)
{
    SetRectBounds((int)&DAT_005a76b0, a, b, c, d);
}

/* Function start: 0x40DE70 */
void FormatNavCoordinates(unsigned char *out)
{
    unsigned char tmp[4];

    SplitGameClockTicks(tmp);
    out[1] = tmp[2];
    out[0] = tmp[3];
}

/* Function start: 0x40DF50 */
char *GetNavNameSkippingMarker(short i)
{
    char *p = g_aMissionObjectives_0059dac5[i].name;

    if (*p == '?')
        p = p + 1;
    return p;
}

/* Function start: 0x40EFE0 */
void NudgeObjectX(short i, short dx, short dy)
{
    short *p = (short *)(DAT_00598a30[i] + 0x20);

    (void)dy;
    *p = *p + dx;
}

/* Function start: 0x40F940 */
void __stdcall free_viewport(Viewport *viewport)
{
    unsigned char *allocation;
    int i;
    int oldCount;

    g_nFreeViewportCalls_005a8110++;
    allocation = viewport->allocation;
    if (allocation != 0) {
        i = 0;
        while (i < g_nViewportAllocationCount_005a7f0c) {
            if (g_apViewportAllocations_005a7f10[i] == allocation) {
                oldCount = g_nViewportAllocationCount_005a7f0c;
                g_nViewportAllocationCount_005a7f0c--;
                g_apViewportAllocations_005a7f10[i] =
                    g_apViewportAllocations_005a7f10[oldCount];
                break;
            }
            i++;
        }
        if (viewport->rowOffsets != 0) {
            ReleasePacketHandle((int)viewport->rowOffsets);
            viewport->rowOffsets = 0;
        }
        if (DAT_0046b168 != 0x13)
            printf("free_viewport not mcga\n");
        ReleasePacketHandle((int)allocation);
        viewport->pixels = 0;
        viewport->allocation = 0;
        if (DAT_005a6ba0.pixels == allocation)
            DAT_005a6ba0.pixels = 0;
        if (DAT_005a6ba0.allocation == allocation)
            DAT_005a6ba0.allocation = 0;
        if (DAT_005a76b0.pixels == allocation)
            DAT_005a76b0.pixels = 0;
        if (DAT_005a76b0.allocation == allocation)
            DAT_005a76b0.allocation = 0;
    }
}

/* Function start: 0x40FA30 */
unsigned short GetPaletteReadyUnused(void)
{
    return 1;
}

/* Function start: 0x40FB70 */
int RunIntroAndMainMenu(void)
{
    IntroArchive title;
    unsigned char palette[0x300];
    char temporaryMidi[MAX_PATH];
    int midiOpened;
    short frame;
    short credit;
    int keepPlaying = 1;

    if (!LoadIntroArchive("TITLE.VGA", &title))
        FatalErrorAndExit("Unable to load TITLE.VGA");
    if (title.sectionCount < 18 || !LoadIntroPalette(palette)) {
        UnloadIntroArchive(&title);
        FatalErrorAndExit("The intro resources are incomplete");
    }

    DAT_0059ab58 = 0;
    DAT_005a8964 = 0;
    StartIntroMusic(temporaryMidi, &midiOpened);

    SetIntroPaletteLevel(palette, 0);
    ClearIntroFrame();
    DrawIntroShape(&title, 6, 0, 0, 36);
    PresentIntroFrame(0);
    keepPlaying = FadeIntroPalette(palette, 1);

    frame = 0;
    while (keepPlaying && frame < 75) {
        ClearIntroFrame();
        DrawIntroShape(&title, 6, 0, 0, 36);
        if (frame >= 8)
            DrawIntroShape(&title, 6, 1, 85, 90);
        if (frame >= 16)
            DrawIntroShape(&title, 6, 2, 143, 90);
        if (frame >= 24)
            DrawIntroShape(&title, 6, 3, 204, 90);
        keepPlaying = PresentIntroFrame(30);
        frame = frame + 1;
    }
    if (keepPlaying)
        keepPlaying = FadeIntroPalette(palette, 0);

    if (keepPlaying) {
        ClearIntroFrame();
        DrawIntroShape(&title, 6, 0, 0, 36);
        SetIntroPaletteLevel(palette, 0);
        keepPlaying = FadeIntroPalette(palette, 1);
    }
    frame = 0;
    while (keepPlaying && frame < 90) {
        ClearIntroFrame();
        DrawIntroShape(&title, 6, 0, 0, 36);
        if (frame >= 6)
            DrawIntroShape(&title, 0, 0, 104, 60);
        if (frame >= 14)
            DrawIntroShape(&title, 0, 1, 160, 60);
        if (frame >= 22)
            DrawIntroShape(&title, 0, 2, 216, 60);
        keepPlaying = PresentIntroFrame(30);
        frame = frame + 1;
    }
    if (keepPlaying)
        keepPlaying = FadeIntroPalette(palette, 0);

    if (keepPlaying) {
        ClearIntroFrame();
        DrawIntroShape(&title, 6, 0, 0, 36);
        DrawIntroShape(&title, 3, 0, 85, 36);
        SetIntroPaletteLevel(palette, 0);
        keepPlaying = FadeIntroPalette(palette, 1);
    }
    frame = 0;
    while (keepPlaying && frame < 180) {
        ClearIntroFrame();
        DrawIntroShape(&title, 6, 0, 0, 36);
        DrawIntroShape(&title, 3, 0, 85, 36);
        if (frame < 110)
            DrawIntroShape(&title, 2, 0, 190, 100);
        if (frame >= 35 && frame < 125)
            DrawIntroShape(&title, 7, (short)((frame / 3) % 13),
                           (short)(75 + frame / 2), 70);
        if (frame >= 70 && frame < 145)
            DrawIntroShape(&title, 8, (short)((frame / 4) % 9), 230, 120);
        if (frame >= 120 && frame < 141)
            DrawIntroShape(&title, 16, (short)(frame - 120), 215, 95);
        if (frame < 80)
            DrawCenteredIntroText(&title,
                "In the distant future,\nmankind is locked in a deadly war...");
        keepPlaying = PresentIntroFrame(30);
        frame = frame + 1;
    }
    if (keepPlaying)
        keepPlaying = FadeIntroPalette(palette, 0);

    credit = 0;
    while (keepPlaying && credit <
           (short)(sizeof(g_aszIntroCredits) / sizeof(g_aszIntroCredits[0]))) {
        ClearIntroFrame();
        DrawIntroShape(&title, 6, 0, 0, 36);
        DrawCenteredIntroText(&title, g_aszIntroCredits[credit]);
        SetIntroPaletteLevel(palette, 0);
        keepPlaying = FadeIntroPalette(palette, 1);
        if (keepPlaying)
            keepPlaying = PresentIntroFrame(600);
        if (keepPlaying)
            keepPlaying = FadeIntroPalette(palette, 0);
        credit = credit + 1;
    }

    if (keepPlaying) {
        SetIntroPaletteLevel(palette, 0x100);
        frame = 17;
        while (keepPlaying && frame > 0) {
            ClearIntroFrame();
            DrawIntroShape(&title, 5, frame, 0, 0);
            keepPlaying = PresentIntroFrame(55);
            frame = frame - 1;
        }
    }
    if (keepPlaying) {
        ClearIntroFrame();
        DrawIntroShape(&title, 5, 0, 0, 0);
        DrawIntroShape(&title, 0, 0, 104, 55);
        DrawIntroShape(&title, 0, 1, 160, 55);
        DrawIntroShape(&title, 0, 2, 216, 55);
        keepPlaying = PresentIntroFrame(1500);
    }
    if (keepPlaying)
        FadeIntroPalette(palette, 0);

    StopIntroMusic(temporaryMidi, midiOpened);
    SetIntroPaletteLevel(palette, 0);
    ClearIntroFrame();
    PresentIntroFrame(0);
    UnloadIntroArchive(&title);
    free_viewport(&DAT_005a7510);

    /* Milestone boundary: the retail function continues into the campaign menu.
     * Returning here leaves the unfinished game loop unreachable while preserving
     * the complete startup intro as the executable's first production path. */
    return 0;
}
