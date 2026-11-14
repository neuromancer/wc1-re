/* Player-patched DOS text: the German fan translation of WC.EXE B2.4 and a
 * replaced FONTS.FNT. Needs the English DOS WC.EXE and GAMEDAT, which it only
 * reads; it patches copies in a scratch directory. No translated text is used:
 * in-place edits use placeholders carrying a few CP437 umlauts.
 * Usage: patched-text-probe WC.EXE GAMEDAT SCRATCH [GERMAN-WC.EXE] */
#include "wc1.h"
#include <stdio.h>
#include <stdlib.h>

static void Require(int condition, const char *what)
{
    if (!condition) {
        fprintf(stderr, "FAIL: %s (%s)\n", what, SDL_GetError());
        exit(1);
    }
}

static void Save(const char *path, const void *data, size_t size)
{
    FILE *file = fopen(path, "wb");

    Require(file && fwrite(data, 1, size, file) == size && fclose(file) == 0, path);
}

/* Rewrite one NUL-delimited English string in place, as the fan patches do. */
static void Rewrite(unsigned char *image, size_t size, const char *english, const char *text)
{
    size_t length = strlen(english);
    size_t offset;

    for (offset = 1; offset + length + 1 < size; offset++) {
        if (image[offset - 1] == 0 && image[offset + length] == 0 &&
            memcmp(image + offset, english, length) == 0) {
            memcpy(image + offset, text, strlen(text) + 1);
            return;
        }
    }
    Require(0, english);
}

int main(int argc, char **argv)
{
    /* CP437 a-, o- and u-umlaut; a French patch would use e-acute and the like. */
    static const char germanLoad[] = "placeholder \x84\x94\x81";
    static const char frenchLoad[] = "placeholder \x82\x8a\x85";
    char path[4096];
    unsigned char *image;
    unsigned char *font;
    size_t imageSize;
    size_t fontSize;
    int count;

    Require(argc == 4 || argc == 5, "usage: WC.EXE GAMEDAT SCRATCH [GERMAN-WC.EXE]");
    Require(SdlLoadGermanText(argv[1]) == 0 && !SdlUsingGermanText(),
            "the English WC.EXE is not a translation");
    image = SDL_LoadFile(argv[1], &imageSize);
    Require(image != NULL, argv[1]);
    Rewrite(image, imageSize, "Loading Game...", frenchLoad);
    Rewrite(image, imageSize, "Replace %s? (Y/N)", "%s placeholder");
    Rewrite(image, imageSize, "Awaken %s? (Y/N)", "%d placeholder");
    /* Followed by zero padding, like the German patch's longer text. */
    Rewrite(image, imageSize, "Need Lock", "placeholder lock");
    SDL_snprintf(path, sizeof(path), "%s/WC.EXE", argv[3]);
    Save(path, image, imageSize);
    Require(SdlCountGermanText(path) == 0 && SdlLoadGermanText(path) == 0 && !SdlUsingGermanText(),
            "other translations, such as the French one, are left alone");
    Rewrite(image, imageSize, frenchLoad, germanLoad);
    Save(path, image, imageSize);
    Require(SdlCountGermanText(path) == 4 && !SdlUsingGermanText(),
            "the launcher's count keeps no text");
    count = SdlLoadGermanText(path);
    Require(count == 4 && SdlUsingGermanText(), "four in-place translations detected");
    Require(SdlCountGermanText(argv[1]) == 0 && SdlUsingGermanText(),
            "counting another file keeps the loaded text");
    Require(!strcmp(C_("font0", "Loading Game..."), germanLoad), "translated UI text");
    Require(!strcmp(C_("font0", "Replace %s? (Y/N)"), "%s placeholder"), "moved %s accepted");
    Require(!strcmp(C_("font0", "Awaken %s? (Y/N)"), "Awaken %s? (Y/N)"),
            "a changed conversion keeps the English text");
    Require(!strcmp(_("Need Lock"), "placeholder lock"), "text may run past its English length");
    Require(!strcmp(C_("font0", "Mission Hangar"), "Mission Hangar"), "untranslated text stays English");
    puts("PASS: German WC.EXE text");
    image[0x40] ^= 1;
    Save(path, image, imageSize);
    Require(SdlLoadGermanText(path) == 0 && !SdlUsingGermanText() &&
            !strcmp(C_("font0", "Loading Game..."), "Loading Game..."),
            "only the B2.4 executable's layout is read");
    SDL_free(image);
    puts("PASS: other executables ignored");
    if (argc == 5) {
        count = SdlLoadGermanText(argv[4]);
        Require(count >= 100, "a complete German WC.EXE");
        printf("PASS: %d German strings in %s\n", count, argv[4]);
        SdlShutdownGermanText();
    }

    /* Catalogs extend the original fonts; a replaced font keeps its own text. */
    SDL_snprintf(path, sizeof(path), "%s/FONTS.FNT", argv[2]);
    font = SDL_LoadFile(path, &fontSize);
    Require(font != NULL && fontSize > 64, path);
    Require(chdir(argv[3]) == 0, "enter scratch directory (with a GAMEDAT folder)");
    Save("GAMEDAT/FONTS.FNT", font, fontSize);
    Require(SdlSetLanguage(1) && strcmp(C_("font0", "Loading Game..."), "Loading Game..."),
            "Spanish applies with the original fonts");
    font[fontSize / 2] ^= 0x5a;
    Save("GAMEDAT/FONTS.FNT", font, fontSize);
    Require(SdlSetLanguage(1) && !strcmp(C_("font0", "Loading Game..."), "Loading Game..."),
            "a replaced FONTS.FNT keeps the installed text");
    SdlSetLanguage(0);
    SDL_free(font);
    puts("PASS: patched fonts fall back to the installed text");
    return 0;
}
