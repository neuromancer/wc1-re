/* Rendering/resource regression probe, linked with the SDL game itself.
 * Requires a player's installation; never writes to it. */
#include "wc1.h"

/* Private game storage observed only by this integration probe. */
extern const char *apszSaveCampaignMenuLabels[2];

static unsigned int Read16(const unsigned char *p)
{
    return p[0] | ((unsigned int)p[1] << 8);
}

static unsigned int Read32(const unsigned char *p)
{
    return Read16(p) | ((unsigned int)Read16(p + 2) << 16);
}

static void Require(int condition, const char *what)
{
    if (!condition) {
        fprintf(stderr, "FAIL: %s (%s)\n", what, SDL_GetError());
        exit(1);
    }
}

static unsigned char *ReadResource(const char *root, const char *name, size_t *size)
{
    char path[4096];
    unsigned char *bytes;
    SDL_snprintf(path, sizeof(path), "%s/%s", root, name);
    bytes = SDL_LoadFile(path, size);
    Require(bytes != 0, path);
    return bytes;
}

static void SavePreview(const char *path, SDL_Surface *surface)
{
    Require(SDL_SaveBMP(surface, path) == 0, "save preview");
}

static void CheckUiTranslations(const char *catalogPath,
                                 TextContext *context, unsigned char **fontData,
                                 SDL_Surface *surface)
{
    size_t size, position = 12;
    unsigned char *bytes = SDL_LoadFile(catalogPath, &size);
    unsigned int count;
    unsigned int translated = 0;
    unsigned int index;
    char formatted[52];
    static const char *calibration[] = {
        "Turn AUTO FIRE off if present, press a button",
        "Move stick to the UPPER LEFT, press a button",
        "Move stick to the LOWER RIGHT, press a button",
        "Center Joystick, press a button",
        "FAILED! Center Joystick, press a button"
    };

    Require(bytes != 0, "UI catalog fixture");
    count = Read32(bytes + 4);
    for (index = 0; index < count; index++) {
        const char *fields[3];
        unsigned int field, lengths[3];
        for (field = 0; field < 3; field++)
            lengths[field] = Read16(bytes + position + field * 2);
        position += 6;
        for (field = 0; field < 3; field++) {
            fields[field] = (const char *)bytes + position;
            position += lengths[field];
        }
        if (*fields[0] && strcmp(fields[0], "font0") && strcmp(fields[0], "font3") && strcmp(fields[0], "intro"))
            continue;
        Require(strcmp(SdlTranslateText(fields[0], fields[1]), fields[2]) == 0, "UI context and source lookup");
        if (strcmp(fields[0], "intro") == 0) {
            const char *line = fields[2];
            Require(pIntroFont != 0, "title sprite alphabet loaded");
            do {
                Require(GetLineLength(line) <= 320, fields[1]);
                line = strchr(line, '\n');
                if (line)
                    line++;
            } while (line && *line);
            SDL_memset(surface->pixels, 0, 64000);
            print_subtitle(context->viewport, 56, fields[2]);
            translated++;
            continue;
        }
        context->font = fontData[*fields[0] ? fields[0][4] - '0' : 1];
        for (const unsigned char *glyph = (const unsigned char *)fields[2]; *glyph; glyph++)
            Require(*glyph == '\n' || *glyph == '\r' || context->font[4 + *glyph] != 0, "UI glyph exists");
        SDL_memset(surface->pixels, 0, 64000);
        /* Format-template glyph coverage; concrete formatted examples below. */
        DrawTextAt(context, 0, 0, fields[2], 2);
        Require(context->cursorY + Read16(context->font) <= 80, "UI template wrapping");
        translated++;
    }
    SDL_free(bytes);
    Require(translated >= 217, "UI catalog coverage");
    Require(strcmp(_("Not a catalog message"), "Not a catalog message") == 0, "missing UI fallback");
    Require(strcmp(C_("wrong-context", "GAME PAUSED"), "GAME PAUSED") == 0, "context mismatch fallback");
    Require(strcmp(_("Center Joystick, press a button"), "Centre la palanca y pulse un bot\xf3n") == 0, "shared accent encoding through macro");
    Require(strcmp(N_("GAME PAUSED"), "GAME PAUSED") == 0, "static marker stays English");
    Require(strcmp(_(N_("GAME PAUSED")), "GAME PAUSED") != 0, "static marker translated at use");
    Require(snprintf(formatted, sizeof(formatted), _("Error: Game %d not loaded."), 32767) < (int)sizeof(formatted), "integer error fits modal buffer");
    Require(snprintf(formatted, sizeof(formatted), _("Error: Game %s not saved."), "WWWWWWWWWWWWWWWW") < (int)sizeof(formatted), "save error fits modal buffer");
    Require(snprintf(formatted, sizeof(formatted), C_("font0", "Replace %s? (Y/N)"), "WWWWWWWWWWWWWWWW") < (int)sizeof(formatted), "confirmation fits modal buffer");
    Require(strstr(formatted, "(Y/N)") != 0, "confirmation matches accepted keys");
    for (index = 0; index < SDL_arraysize(calibration); index++)
        Require(strlen(_(calibration[index])) < 52, "calibration fits modal buffer");
    printf("PASS: %u UI messages, static/runtime/context macros, accents, formatting and key labels.\n", translated);
}

static void CheckExtraResources(const char *root,
                                TextContext *context, SDL_Surface *surface)
{
    static const char *files[] = {"TITLE.VGA", "TITLE.VGA", "TITLE1.VGA", "COCKPIT.VGA", "RECROOM.VGA", "SERIES.VGA"};
    static const short sections[] = {1, 4, 0, 7, 0, 0};
    unsigned int i, translatedLines = 0;
    char path[4096];

    apViewportAllocations[0] = surface->pixels;
    nViewAllocTotal = 1;
    for (i = 0; i < SDL_arraysize(files); i++) {
        size_t archiveSize, originalSize, translatedSize, ignoredSize;
        unsigned char *archive = ReadResource(root, files[i], &archiveSize);
        unsigned char *original, *translated, *ignored;
        PacketSectionHandle handle;
        Require(SdlExtractOriginPacketSection(archive, archiveSize, sections[i], &original, &originalSize), "decode translated artwork");
        Require(SdlTranslatePacketData(files[i], sections[i], original, originalSize, &translated, &translatedSize) && translated, "apply artwork delta");
        Require(Read32(translated) == translatedSize, "translated shape allocation size");
        SDL_snprintf(path, sizeof(path), "%s/%s", root, files[i]);
        Require(OpenPacketSection(path, sections[i], &handle) && handle.patchedData, "artwork through game packet loader");
        Require(handle.dataSize == translatedSize && memcmp(handle.patchedData, translated, translatedSize) == 0, "game loads same translated artwork");
        CloseDataFileByHandle(&handle);
        /* Edition guard must fail closed; no delta on unknown source bytes. */
        original[originalSize - 1] ^= 1;
        Require(SdlTranslatePacketData(files[i], sections[i], original, originalSize, &ignored, &ignoredSize) && !ignored, "artwork CRC fallback");
        original[originalSize - 1] ^= 1;
        {
            unsigned char *tagged = AllocateTaggedMemory((int)translatedSize, 0x40);
            Require(tagged != 0, "tagged sprite allocation");
            memcpy(tagged, translated, translatedSize);
            free(translated);
            translated = tagged;
        }
        if (i == 0) {
            Require(strcmp(C_("intro", "Meanwhile..."), "Meanwhile...") == 0, "unknown title font keeps English encoding");
            Require(SdlTranslatePacketData(files[i], sections[i], original, originalSize, &ignored, &ignoredSize) && ignored, "restore title alphabet");
            free(ignored);
            pIntroFont = (struct Shape *)translated;
        } else {
            short frame, count = Read32(translated + 4) / 4 - 1;
            for (frame = 0; frame < count; frame++) {
                SDL_memset(surface->pixels, 0, 64000);
                DrawSpriteDefault(context->viewport, 0, 0, (struct Shape *)translated, frame);
            }
            ReleasePacketHandle(translated);
        }
        free(original);
        SDL_free(archive);
    }
    for (i = 0; i < 9; i++) {
        char name[20];
        size_t archiveSize;
        unsigned char *archive;
        short section;
        SDL_snprintf(name, sizeof(name), "MIDGAME.V0%u", i);
        archive = ReadResource(root, name, &archiveSize);
        for (section = 5; section < 7; section++) {
            unsigned char *original, *translated;
            size_t originalSize, translatedSize, scene, text, newScene, newText, record;
            size_t start = Read32(archive + 4 + section * 4) & 0xffffff;
            size_t end = section == 6 ? archiveSize : Read32(archive + 8 + section * 4) & 0xffffff;
            if (start == end)
                continue;
            Require(SdlExtractOriginPacketSection(archive, archiveSize, section, &original, &originalSize), "decode midgame captions");
            Require(SdlTranslatePacketData(name, section, original, originalSize, &translated, &translatedSize), "translate midgame captions");
            if (!translated) {
                free(original);
                continue;
            }
            scene = Read32(original); text = Read32(original + 4);
            newScene = Read32(translated); newText = Read32(translated + 4);
            Require(scene == newScene && memcmp(original + 8, translated + 8, scene - 8) == 0, "preserve midgame header and metadata");
            Require(memcmp(original + text, translated + newText, originalSize - text) == 0, "preserve midgame scripts");
            for (record = 0; record < (text - scene) / 13; record++) {
                const unsigned char *a = original + scene + record * 13;
                const unsigned char *b = translated + newScene + record * 13;
                const char *en, *es;
                Require(memcmp(a, b, 7) == 0 && memcmp(a + 9, b + 9, 4) == 0, "preserve midgame timing/conditions/animation");
                if ((signed char)a[0] == -2)
                    continue;
                en = (const char *)original + text + Read16(a + 7);
                es = (const char *)translated + newText + Read16(b + 7);
                if (strcmp(en, es) == 0)
                    continue;
                translatedLines++;
                SDL_memset(surface->pixels, 0, 64000);
                DrawTextAt(context, 0, 0, AddPCName(es), 2);
                if (context->cursorY + Read16(context->font) > 40)
                    fprintf(stderr, "MIDGAME OVERFLOW %s/%d/%zu: %s\n", name, section, record, es);
                Require(context->cursorY + Read16(context->font) <= 40, "midgame caption fits");
            }
            free(original);
            free(translated);
        }
        SDL_free(archive);
    }
    Require(translatedLines >= 80, "midgame coverage");
    printf("PASS: 6 artwork packets, CRC fallback, title alphabet and %u midgame captions.\n", translatedLines);
}

static void CheckSimulatorDisplays(TextContext *context, unsigned char **fontData,
                                   SDL_Surface *surface)
{
    char leftText[200], rightText[200], message[100];
    short cockpit;
    static const char *commands[] = {
        "Never mind...", "Attack my target!", "Help me out here", "Return to base.",
        "Die furball!", "Slag off!", "Bite it cat face.", "Break and attack.",
        "Keep formation!", "Form on my wing.", "Keep radio silence", "Broadcast freely", "Request Landing"
    };
    unsigned int command;
    for (command = 0; command < SDL_arraysize(commands); command++) {
        const unsigned char *text;
        unsigned int width = 0;
        snprintf(message, sizeof(message), "7 %s", _(commands[command]));
        for (text = (unsigned char *)message; *text; text++)
            width += fontData[2][4 + *text];
        Require(width < 72, commands[command]);
    }
    for (command = OBJECT_TYPE_LASER_CANNON; command <= OBJECT_TYPE_SPACE_MINE; command++) {
        const unsigned char *text;
        unsigned int width = 0;
        snprintf(message, sizeof(message), command <= OBJECT_TYPE_MASS_DRIVER_CANNON ? _("\nGun: %s") : _("\nWeapon: %s"), _(aObjectTypeData[command].displayName));
        for (text = (unsigned char *)message; *text; text++) {
            if (*text != '\n')
                width += fontData[2][4 + *text];
        }
        Require(width < 72, aObjectTypeData[command].displayName);
    }
    cBlackColour = 0;
    cPrimaryTextColour = cDefaultTextColour = 1;
    cRedColour = 2;
    context->font = fontData[2];
    stLeftVduTextContext = stRightVduTxtContext = *context;
    stLeftVduTextContext.text = leftText;
    stRightVduTxtContext.text = rightText;
    stLeftVduTextContext.viewport = &stLeftVdu;
    stRightVduTxtContext.viewport = &stRightVduViewport;
    stLeftVdu = stRightVduViewport = *context->viewport;
    ausVduModeStack[0] = ausVduModeStack[8] = 1;
    nSelectedReleaseWeaponIndex = 0;
    ((ShipWeaponSlot *)&aShipWeaponsTable[0][1])[0].type = OBJECT_TYPE_DUMB_FIRE_MISSILE;
    eSelectedGunType = OBJECT_TYPE_LASER_CANNON;
    nTrainSimActive = 1;
    acShipTargetTable[0] = -1;
    for (cockpit = 0; cockpit < 5; cockpit++) {
        cCurrentCockpitView = cockpit;
        *(ShortRect *)&stLeftVdu.left = stCockpitLayout.leftVduBounds[cockpit];
        *(ShortRect *)&stRightVduViewport.left = stCockpitLayout.rightVduBounds[cockpit];
        SDL_memset(surface->pixels, 0, 64000);
        show_weapon_disp();
        Require(stLeftVduTextContext.cursorY == stLeftVdu.top + 2 * Read16(fontData[2]), "weapon heading stays on one line above divider");
        Require(strcmp(_(aObjectTypeData[OBJECT_TYPE_LASER_CANNON].displayName), "L\xe1ser") == 0, "translated weapon name");
        Require(stLeftVduTextContext.cursorY + Read16(fontData[2]) <= stLeftVdu.bottom, "weapon display fits every cockpit");
        show_target_disp();
        Require(strcmp(_(" None"), " Ninguno") == 0, "translated target fallback");
        Require(stRightVduTxtContext.cursorY + Read16(fontData[2]) <= stRightVduViewport.bottom, "target display fits every cockpit");
        show_damage_disp();
        Require(strstr(_("NO INTERNAL\n\nDAMAGE"), "INTERNOS") != 0, "translated damage report");
        Require(stLeftVduTextContext.cursorY + Read16(fontData[2]) <= stLeftVdu.bottom, "damage display fits every cockpit");
    }
    nTrainSimActive = 0;
    context->font = fontData[0];
    context->text = szDefaultTxtBuf;
    DrawRoomMenuLabel(context, apszSaveCampaignMenuLabels[0]);
    Require(strstr(szTxtWorkspaceBuffer, "Guardar") != 0, "shared save label translates at display");
    Require(strcmp(apszSaveCampaignMenuLabels[0], "Save this campaign  ") == 0, "shared save-label ownership remains intact");
    context->font = fontData[1];
    printf("PASS: real simulator weapon/target/damage screens in all 5 cockpit layouts and shared room labels.\n");
}

int main(int argc, char **argv)
{
    SDL_Surface *surface;
    SDL_Color palette[256];
    unsigned short rows[200];
    Viewport viewport;
    TextContext context;
    PilotRecord pilot = {"WWWWWWWWWWWWW", "WWWWWWWWWWWWW", 0, 0, 0, 0, 0};
    CampaignDate date = {100, 2654};
    unsigned char *fontData[4];
    unsigned char *archive;
    size_t archiveSize;
    size_t fontSize[4];
    unsigned int index, campaign, section;
    unsigned int translatedCount = 0;
    unsigned int packetsChecked = 0;
    unsigned int tallCount = 0;
    unsigned int maxHeight = 0;
    char sample[256] = "";
    char radio[84] = "";

    if (argc != 4) {
        fprintf(stderr, "Usage: %s CATALOG GAMEDAT PREVIEW.bmp\n", argv[0]);
        return 1;
    }
    Require(SdlLoadTranslationCatalog(argv[1]), "load catalog");
    surface = SDL_CreateRGBSurfaceWithFormat(0, 320, 200, 8, SDL_PIXELFORMAT_INDEX8);
    Require(surface != 0 && surface->pitch == 320, "indexed game canvas");
    for (index = 0; index < 256; index++) {
        palette[index].r = palette[index].g = palette[index].b = index;
        palette[index].a = 255;
    }
    palette[1].r = palette[1].g = palette[1].b = 255;
    palette[2].r = 255; palette[2].g = 224; palette[2].b = 32;
    SDL_SetPaletteColors(surface->format->palette, palette, 0, 256);
    for (index = 0; index < 200; index++)
        rows[index] = (unsigned short)(index * 320);
    memset(&viewport, 0, sizeof(viewport));
    viewport.pixels = surface->pixels;
    viewport.rowOffsets = rows;
    viewport.right = 319;
    viewport.bottom = 199;
    memset(&context, 0, sizeof(context));
    context.viewport = &viewport;
    context.colour = 1;
    context.backgroundColour = 0;
    stCampaignState.currentPilot = &pilot;
    pCurrentCampaignDate = &date;
    pElapsedCampaignDatePtr = &date;
    for (index = 0; index < 8; index++)
        apWingmanPilotsList[index] = &pilot;
    memcpy(abSeriesAuxData, "Gimle", 6);
    archive = ReadResource(argv[2], "FONTS.FNT", &archiveSize);
    for (index = 0; index < 4; index++) {
        unsigned char *original;
        size_t size;
        Require(SdlExtractOriginPacketSection(archive, archiveSize, index, &original, &size), "decode font");
        Require(SdlTranslatePacketData("FONTS.FNT", index, original, size, &fontData[index], &fontSize[index]) && fontData[index], "translate font");
        free(original);
    }
    SDL_free(archive);
    /* Shared objective names and UI literals must render ñ/ó/ú in either
     * font. Compare the normalized menu slots with the imported glyphs. */
    for (index = 0; index < 3; index++) {
        static const unsigned char originalSlots[3] = {0xe0, 0xeb, 0xef};
        static const unsigned char normalizedSlots[3] = {0xf1, 0xf3, 0xfa};
        unsigned char *font = fontData[1];
        unsigned int a = originalSlots[index], b = normalizedSlots[index];
        unsigned int offsetA = font[260 + a] + 256 * font[516 + a];
        unsigned int offsetB = font[260 + b] + 256 * font[516 + b];
        Require(font[4 + a] == font[4 + b] &&
                memcmp(font + offsetA, font + offsetB, font[4 + a] * Read16(font)) == 0,
                "same Spanish accent glyph in shared font encoding");
    }
    context.font = fontData[0];
    for (campaign = 0; campaign < 3; campaign++) {
        char name[32];
        unsigned int count;
        SDL_snprintf(name, sizeof(name), "BRIEFING.00%u", campaign);
        archive = ReadResource(argv[2], name, &archiveSize);
        count = (Read32(archive + 4) & 0xffffff) / 4 - 1;
        for (section = 0; section < count; section++) {
            unsigned char *original;
            unsigned char *translated;
            size_t size, newSize;
            size_t pair, pairs = section == 0 ? 7 : section < 3 ? 1 : 5;
            if ((Read32(archive + 4 + section * 4) & 0xffffff) ==
                (section + 1 < count ? (Read32(archive + 8 + section * 4) & 0xffffff) : Read32(archive)))
                continue;
            if (!SdlExtractOriginPacketSection(archive, archiveSize, section, &original, &size)) {
                fprintf(stderr, "%s section %u\n", name, section);
                Require(0, "decode briefing");
            }
            Require(SdlTranslatePacketData(name, section, original, size, &translated, &newSize), "translate briefing");
            packetsChecked++;
            if (!translated) {
                free(original);
                continue;
            }
            for (pair = 0; pair < pairs; pair++) {
                size_t scene = Read32(original + pair * 8);
                size_t text = Read32(original + pair * 8 + 4);
                size_t end = pair + 1 < pairs ? Read32(original + (pair + 1) * 8) : size;
                size_t newScene = Read32(translated + pair * 8);
                size_t newText = Read32(translated + pair * 8 + 4);
                size_t record;
                Require(memcmp(original + text, translated + newText, end - text) == 0, "preserve original scripts/text blob");
                for (record = 0; record < (text - scene) / 13; record++) {
                    const unsigned char *a = original + scene + record * 13;
                    const unsigned char *b = translated + newScene + record * 13;
                    const char *en;
                    const char *es;
                    unsigned int height;
                    Require(memcmp(a, b, 7) == 0 && memcmp(a + 9, b + 9, 4) == 0, "preserve scene timing/conditions/animation");
                    if ((signed char)a[0] == -2)
                        continue;
                    en = (const char *)original + text + Read16(a + 7);
                    es = (const char *)translated + newText + Read16(b + 7);
                    if (strcmp(en, es) == 0)
                        continue;
                    translatedCount++;
                    for (const unsigned char *glyph = (const unsigned char *)es; *glyph; glyph++)
                        Require(*glyph == '\n' || *glyph == '\r' || context.font[4 + *glyph] != 0, "dialogue glyph exists");
                    SDL_memset(surface->pixels, 0, 64000);
                    DrawTextAt(&context, 0, 0, AddPCName(es), 2);
                    height = context.cursorY + Read16(context.font);
                    if (height > maxHeight)
                        maxHeight = height;
                    if (height > 40) {
                        tallCount++;
                        fprintf(stderr, "OVERFLOW %s packet %u scene %zu record %zu (%u px)\n", name, section, pair, record, height);
                    }
                    if (!sample[0] && strlen(es) > 60 && strlen(es) < 130 && strchr(es, (char)0xf3))
                        SDL_strlcpy(sample, AddPCName(es), sizeof(sample));
                }
            }
            free(translated);
            free(original);
        }
        SDL_free(archive);
        SDL_snprintf(name, sizeof(name), "MODULE.00%u", campaign);
        archive = ReadResource(argv[2], name, &archiveSize);
        for (section = 0; section < 6; section++) {
            unsigned char *original, *translated;
            size_t size, newSize;
            Require(SdlExtractOriginPacketSection(archive, archiveSize, section, &original, &size), "decode module");
            Require(SdlTranslatePacketData(name, section, original, size, &translated, &newSize), "translate module");
            Require(newSize == size, "fixed module storage");
            free(translated);
            free(original);
            packetsChecked++;
        }
        SDL_free(archive);
    }
    bInflightComputerActive = 1; /* Exercise message storage without a game timer. */
    for (campaign = 0; campaign < 2; campaign++) {
        const char *radioName = campaign ? "COMMSM2.DAT" : "COMMUNIC.DAT";
        archive = ReadResource(argv[2], radioName, &archiveSize);
        for (index = 0; index < archiveSize / 80; index++) {
            char line[84];
            memcpy(line, archive + index * 80, 80);
            line[80] = 0;
            char prefixed[256];
            char *expanded;
            SdlTranslateRadio(radioName, index / 11, index % 11, line, sizeof(line));
            for (const unsigned char *glyph = (const unsigned char *)line; *glyph; glyph++)
                Require(*glyph == '\n' || *glyph == '\r' || fontData[1][4 + *glyph] != 0, "radio glyph exists");
            SDL_snprintf(prefixed, sizeof(prefixed), "%s: %s", pilot.callsign, line);
            expanded = ExpandCommMessageTokens(prefixed);
            Require(strlen(expanded) < sizeof(szHudMessageBuf), "radio fits persistent HUD buffer");
            ShowCentredPrompt(expanded, 0);
            Require(strcmp(szHudMessageBuf, expanded) == 0, "radio survives message storage");
            if (strcmp(line, (const char *)archive + index * 80) && !radio[0] && strlen(line) > 20)
                SDL_strlcpy(radio, line, sizeof(radio));
        }
        SDL_free(archive);
    }
    /* Exercise the actual archive handle used by GetPacketSize/PacketLoad. */
    {
        PacketSectionHandle handle;
        char path[4096];
        unsigned char *bytes;
        SDL_snprintf(path, sizeof(path), "%s/BRIEFING.000", argv[2]);
        Require(OpenPacketSection(path, 4, &handle), "open translated packet handle");
        Require(handle.patchedData != 0 && handle.compression == 0, "translated packet size/encoding");
        bytes = malloc(handle.dataSize);
        Require(bytes != 0 && ReadPacketSectionData(&handle, bytes, handle.dataSize), "read translated packet handle");
        Require(memcmp(bytes, handle.patchedData, handle.dataSize) == 0, "handle bytes match size query");
        free(bytes);
        CloseDataFileByHandle(&handle);
    }
    SDL_memset(surface->pixels, 0, 64000);
    DrawTextAt(&context, 0, 4, "ESPANOL - WC1 TEXT RENDERER", 2);
    context.colour = 2;
    DrawTextAt(&context, 0, 24, "\241Atenci\363n, se\361or! \277Listo?", 2);
    DrawTextAt(&context, 0, 38, "\301 \311 \315 \323 \332 \321 / \341 \351 \355 \363 \372 \361", 2);
    context.colour = 1;
    DrawTextAt(&context, 0, 62, sample, 2);
    context.font = fontData[1];
    context.colour = 2;
    DrawTextAt(&context, 0, 112, "RADIO / NAV - FONT 1", 2);
    DrawTextAt(&context, 0, 126, "\241Ca\361\363n! \277D\363nde est\341s t\372?", 2);
    context.colour = 1;
    DrawTextAt(&context, 0, 144, ExpandCommMessageTokens(radio), 2);
    SavePreview(argv[3], surface);
    printf("PASS: %u packets; %u translated dialogue lines rendered; max height %u; %u exceed 40px.\n", packetsChecked, translatedCount, maxHeight, tallCount);
    printf("Preview: %s\n", argv[3]);
    Require(translatedCount > 5000 && sample[0] && radio[0], "substantial Spanish coverage");
    Require(tallCount == 0, "dialogue fits with maximum-width pilot names");
    context.font = fontData[0];
    CheckExtraResources(argv[2], &context, surface);
    CheckUiTranslations(argv[1], &context, fontData, surface);
    CheckSimulatorDisplays(&context, fontData, surface);
    ReleasePacketHandle(pIntroFont);
    pIntroFont = 0;
    SdlSetLanguage(0);
    Require(strcmp(_("GAME PAUSED"), "GAME PAUSED") == 0, "English macro bypass");
    {
        unsigned char *unchanged;
        size_t size;
        Require(SdlTranslatePacketData("FONTS.FNT", 0, fontData[0], fontSize[0], &unchanged, &size) && unchanged == 0, "English bypass");
    }
    for (index = 0; index < 4; index++)
        free(fontData[index]);
    SDL_FreeSurface(surface);
    return 0;
}
