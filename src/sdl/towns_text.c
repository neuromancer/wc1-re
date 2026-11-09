/* FM Towns Shift-JIS text. WC.EXP's renderer (load image 0x9f2ec) uses
 * 8/4 logical pixels per full/half-width glyph and 10 pixels per line.
 * Misaki replaces the machine's font ROM; game strings stay on the disc. */
#include "wc1.h"
#include "video_internal.h"

extern const unsigned char abTownsFont[];
extern const unsigned short awTownsUnicode[256 + 94 * 94];
static int townsJapanese;
static unsigned char *townsTextImage;
static const char *townsNames[34][2];
static unsigned char pendingLead;
static TextContext *pendingContext;
static short pendingX, pendingY;
static char inputText[40], inputComposition[80];
static int inputActive, inputResult, inputLimit, inputMode, inputCursor;
static int inputSelected, inputComposing, inputCompositionChanged;

static const char *TownsImageString(size_t pointer)
{
    unsigned int offset;
    const unsigned char *p = townsTextImage + 0x200 + pointer;
    offset = p[0] | (unsigned int)p[1] << 8 | (unsigned int)p[2] << 16 | (unsigned int)p[3] << 24;
    if (offset >= 743037 - 0x200 ||
        !memchr(townsTextImage + 0x200 + offset, 0, 743037 - 0x200 - offset))
        return "";
    return (const char *)townsTextImage + 0x200 + offset;
}

int SdlSetTownsLanguage(int japanese)
{
    char path[PATH_MAX];
    size_t size;
    int index;
    townsJapanese = 0;
    pendingLead = 0;
    SDL_free(townsTextImage);
    townsTextImage = 0;
    if (!japanese)
        return 1;
    if (!SdlUsingTownsData()) {
        SDL_SetError("Japanese requires FM Towns game data");
        return 0;
    }
    if (!SdlResolvePath("WC.EXP", path, sizeof(path)) &&
        !SdlResolvePath("../WC.EXP", path, sizeof(path))) {
        SDL_SetError("Japanese requires the FM Towns WC.EXP");
        return 0;
    }
    townsTextImage = SDL_LoadFile(path, &size);
    if (!townsTextImage || size != 743037 || memcmp(townsTextImage, "P3", 2))
        goto invalid;
    /* Original bilingual rank/medal/system tables, not copied game text. */
    for (index = 0; index < 34; index++) {
        size_t english = index < 5 ? 0x10a34 + index * 16 :
            index < 10 ? 0x6b150 + (index - 5) * 8 : 0x6b178 + (index - 10) * 4;
        size_t japanese = index < 10 ? english + 4 : english + 0x64;
        /* The Towns edition uses a different rank ladder at the same indices. */
        townsNames[index][0] = index < 5 ? apszPilotRankNames[index] : TownsImageString(english);
        townsNames[index][1] = TownsImageString(japanese);
        if (!*townsNames[index][0] || !*townsNames[index][1])
            goto invalid;
    }
    townsJapanese = 1;
    return 1;

invalid:
    SDL_free(townsTextImage);
    townsTextImage = 0;
    SDL_SetError("Unsupported FM Towns WC.EXP (expected F1.0)");
    return 0;
}

int SdlUsingTownsJapanese(void)
{
    return townsJapanese;
}

const char *SdlTranslateTownsText(const char *text)
{
    int index;
    if (townsJapanese) {
        for (index = 0; index < 34; index++) {
            if (*townsNames[index][0] && !SDL_strcasecmp(text, townsNames[index][0]))
                return townsNames[index][1];
        }
    }
    return text;
}

const char *SdlTownsFilename(const char *path)
{
    static char translated[PATH_MAX];
    char *name;
    if (!townsJapanese)
        return path;
    if (SDL_strlcpy(translated, path, sizeof(translated)) >= sizeof(translated))
        return path;
    name = translated + strlen(translated);
    while (name > translated && name[-1] != '/' && name[-1] != '\\')
        name--;
    if (!SDL_strncasecmp(name, "BRIEFING.00", 11) && strlen(name) == 12 &&
        name[11] >= '0' && name[11] <= '2')
        name[9] = 'J';
    else if (!SDL_strncasecmp(name, "MIDGAME.V0", 10) && strlen(name) == 11 &&
             name[10] >= '0' && name[10] <= '8')
        name[9] = 'J';
    else if (!SDL_strcasecmp(name, "COMMUNIC.DAT"))
        strcpy(name, nCampaignDataSet == 2 ? "COMMSM2.JAT" : "COMMUNIC.JAT");
    return translated;
}

static int TownsLead(unsigned char value)
{
    return (value >= 0x81 && value <= 0x9f) || (value >= 0xe0 && value <= 0xef);
}

static int TownsTrail(unsigned char value)
{
    return value >= 0x40 && value <= 0xfc && value != 0x7f;
}

static unsigned int TownsGlyph(const unsigned char **text, int *width)
{
    unsigned int first = *(*text)++, second, row, column;
    *width = 4;
    if (!TownsLead(first))
        return first;
    *width = 8;
    if (!TownsTrail(**text))
        return 0; /* Visible replacement; never swallow a newline or NUL. */
    second = *(*text)++;
    row = (first - (first < 0xa0 ? 0x81 : 0xc1)) * 2;
    if (second >= 0x9f) {
        row++;
        column = second - 0x9f;
    } else {
        column = second - (second < 0x7f ? 0x40 : 0x41);
    }
    return 256 + row * 94 + column;
}

static int TownsJapaneseText(const char *text)
{
    if (!townsJapanese)
        return 0;
    for (; *text; text++) {
        if ((unsigned char)*text >= 0x80)
            return 1;
    }
    return 0;
}

static void TownsDrawGlyph(unsigned int glyph, int width)
{
    TextContext *context = pCurTextContext;
    Viewport *viewport = context->viewport;
    int x, y;
    for (y = 0; y < 8; y++) {
        int targetY = context->cursorY + y;
        unsigned int offset;
        if (targetY < viewport->top || targetY > viewport->bottom)
            continue;
        offset = viewport->rowOffsets[targetY];
        if (targetY == viewport->top && (offset & 0x8000))
            offset = 0;
        for (x = 0; x < width; x++) {
            int targetX = context->cursorX + x;
            unsigned char colour = abTownsFont[glyph * 8 + y] & (0x80 >> x) ?
                context->colour : context->backgroundColour;
            if (targetX >= viewport->left && targetX <= viewport->right && colour != 0xff)
                viewport->pixels[offset + targetX] = colour;
        }
    }
    context->cursorX += width;
}

int SdlMeasureTownsText(const char *text)
{
    const unsigned char *cursor = (const unsigned char *)text;
    int width = 0, maximum = 0, glyphWidth;
    if (!TownsJapaneseText(text))
        return -1;
    while (*cursor) {
        if (*cursor == '\n' || *cursor == '\r') {
            maximum = SDL_max(maximum, width);
            width = 0;
            cursor++;
            continue;
        }
        TownsGlyph(&cursor, &glyphWidth);
        if (width + glyphWidth >= 320)
            break;
        width += glyphWidth;
    }
    return SDL_max(maximum, width);
}

int SdlDrawTownsText(const char *text)
{
    const unsigned char *cursor = (const unsigned char *)text;
    TextContext *context = pCurTextContext;
    Viewport *viewport = context->viewport;
    int origin = context->cursorX;
    pendingLead = 0;
    if (!TownsJapaneseText(text))
        return 0;
    while (*cursor && context->cursorY <= viewport->bottom) {
        const unsigned char *start = cursor;
        int width = 0, glyphWidth;
        while (*cursor && *cursor != '\n' && *cursor != '\r') {
            const unsigned char *next = cursor;
            TownsGlyph(&next, &glyphWidth);
            if (width && origin + width + glyphWidth > viewport->right + 1)
                break;
            width += glyphWidth;
            cursor = next;
        }
        context->cursorX = context->alignment == 2 ?
            viewport->left + (viewport->right - viewport->left + 1 - width) / 2 :
            context->alignment == 1 ? viewport->right + 1 - width : origin;
        while (start < cursor) {
            unsigned int glyph = TownsGlyph(&start, &glyphWidth);
            TownsDrawGlyph(glyph, glyphWidth);
        }
        if (*cursor) {
            if (*cursor == '\r')
                cursor++;
            if (*cursor == '\n')
                cursor++;
            context->cursorY += 10;
            origin = viewport->left;
            context->cursorX = origin;
        }
    }
    return 1;
}

int SdlDrawTownsCharacter(unsigned char character)
{
    TextContext *context = pCurTextContext;
    if (!townsJapanese)
        return 0;
    if (pendingLead) {
        unsigned char pair[3] = {pendingLead, character, 0};
        const unsigned char *cursor = pair;
        int width;
        pendingLead = 0;
        if (context == pendingContext && context->cursorX == pendingX &&
            context->cursorY == pendingY && TownsTrail(character)) {
            unsigned int glyph = TownsGlyph(&cursor, &width);
            TownsDrawGlyph(glyph, width);
            return 1;
        }
    }
    if (TownsLead(character)) {
        pendingLead = character;
        pendingContext = context;
        pendingX = context->cursorX;
        pendingY = context->cursorY;
        return 1;
    }
    if (character >= 0x80) {
        TownsDrawGlyph(character, 4);
        return 1;
    }
    return 0;
}

static int TownsEncodeInput(Uint32 character, char encoded[2])
{
    unsigned int glyph, row, column;
    if (character < 128) {
        if (inputMode == 2 ? character < '0' || character > '9' :
            !((character >= 'A' && character <= 'Z') ||
              (character >= 'a' && character <= 'z') ||
              (character >= '0' && character <= '9') || character == ' '))
            return 0;
        encoded[0] = inputMode == 1 ? toupper(character) : character;
        return 1;
    }
    if (inputMode == 2)
        return 0;
    for (glyph = 0xa1; glyph < SDL_arraysize(awTownsUnicode); glyph++) {
        if (awTownsUnicode[glyph] != character)
            continue;
        if (glyph < 256) {
            if (glyph > 0xdf)
                return 0;
            encoded[0] = glyph;
            return 1;
        }
        row = (glyph - 256) / 94;
        column = (glyph - 256) % 94;
        encoded[0] = row / 2 + (row < 62 ? 0x81 : 0xc1);
        encoded[1] = column + (row % 2 ? 0x9f : column < 63 ? 0x40 : 0x41);
        return 2;
    }
    return 0;
}

static void TownsInsertInput(const char *utf8, int composition)
{
    Uint32 *unicode = (Uint32 *)SDL_iconv_string(
        SDL_BYTEORDER == SDL_LIL_ENDIAN ? "UTF-32LE" : "UTF-32BE",
        "UTF-8", utf8, strlen(utf8) + 1);
    int length = strlen(inputText), previewLength = 0;
    unsigned int index;
    if (!unicode)
        return;
    if (composition)
        inputComposition[0] = 0;
    for (index = 0; unicode[index]; index++) {
        char encoded[2];
        int size = TownsEncodeInput(unicode[index], encoded);
        if (!size)
            continue;
        if (composition) {
            if (previewLength + size >= sizeof(inputComposition))
                break;
            memcpy(inputComposition + previewLength, encoded, size);
            previewLength += size;
            inputComposition[previewLength] = 0;
        } else {
            if (inputSelected) {
                inputText[0] = 0;
                inputCursor = length = inputSelected = 0;
            }
            if (!length && (unicode[index] == ' ' || unicode[index] == 0x3000))
                continue;
            if (length + size > inputLimit)
                break;
            memmove(inputText + inputCursor + size, inputText + inputCursor,
                    length - inputCursor + 1);
            memcpy(inputText + inputCursor, encoded, size);
            inputCursor += size;
            length += size;
        }
    }
    SDL_free(unicode);
}

static int TownsPreviousInputCharacter(void)
{
    int position = 0, previous = 0;
    while (position < inputCursor) {
        previous = position;
        position += TownsLead((unsigned char)inputText[position]) ? 2 : 1;
    }
    return previous;
}

int SdlHandleTownsTextInputEvent(const SDL_Event *event)
{
    int length, next;
    SDL_Keycode key;
    if (!inputActive)
        return 0;
    if (event->type == SDL_TEXTEDITING) {
        inputComposing = event->edit.text[0] != 0;
        inputCompositionChanged = 1;
        TownsInsertInput(event->edit.text, 1);
        return 1;
    }
    if (event->type == SDL_TEXTINPUT) {
        inputCompositionChanged |= inputComposing;
        inputComposing = 0;
        inputComposition[0] = 0;
        if (!inputResult)
            TownsInsertInput(event->text.text, 0);
        return 1;
    }
    if (event->type != SDL_KEYDOWN && event->type != SDL_KEYUP)
        return 0;
    key = event->key.keysym.sym;
    /* Keep the host's quit/fullscreen shortcuts available during entry. */
    if ((key == SDLK_q && (event->key.keysym.mod & KMOD_GUI)) ||
        (key == SDLK_RETURN && (event->key.keysym.mod & (KMOD_GUI | KMOD_ALT))))
        return 0;
    if (event->type == SDL_KEYUP || inputResult || inputComposing || inputCompositionChanged)
        return 1;
    length = strlen(inputText);
    if ((event->key.keysym.mod & (KMOD_CTRL | KMOD_GUI)) && key == SDLK_a) {
        inputSelected = 1;
        return 1;
    }
    if ((event->key.keysym.mod & (KMOD_CTRL | KMOD_GUI)) && key == SDLK_v) {
        char *clipboard = SDL_GetClipboardText();
        if (clipboard) {
            TownsInsertInput(clipboard, 0);
            SDL_free(clipboard);
        }
        return 1;
    }
    switch (key) {
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
        if (!event->key.repeat && length)
            inputResult = 1;
        break;
    case SDLK_ESCAPE:
        if (!event->key.repeat)
            inputResult = -1;
        break;
    case SDLK_BACKSPACE:
    case SDLK_DELETE:
        if (inputSelected) {
            inputText[0] = 0;
            inputCursor = inputSelected = 0;
        } else if (key == SDLK_BACKSPACE && inputCursor) {
            next = TownsPreviousInputCharacter();
            memmove(inputText + next, inputText + inputCursor, length - inputCursor + 1);
            inputCursor = next;
        } else if (key == SDLK_DELETE && inputCursor < length) {
            next = inputCursor + (TownsLead((unsigned char)inputText[inputCursor]) ? 2 : 1);
            memmove(inputText + inputCursor, inputText + next, length - next + 1);
        }
        break;
    case SDLK_HOME:
        inputCursor = inputSelected = 0;
        break;
    case SDLK_END:
        inputCursor = length;
        inputSelected = 0;
        break;
    case SDLK_LEFT:
        inputCursor = inputSelected ? 0 : TownsPreviousInputCharacter();
        inputSelected = 0;
        break;
    case SDLK_RIGHT:
        if (inputSelected)
            inputCursor = length;
        else if (inputCursor < length)
            inputCursor += TownsLead((unsigned char)inputText[inputCursor]) ? 2 : 1;
        inputSelected = 0;
        break;
    }
    return 1;
}

short SdlReadTownsTextInput(char *destination, short maximumLength, short mode)
{
    TextContext *savedContext = pCurTextContext;
    TextContext context = *savedContext;
    Viewport field = *context.viewport, marker;
    SDL_Rect ime;
    void (*savedPump)(void) = pCurrentEventManagerPump;
    int savedCursor = nMouseCursorShowCount;
    SDL_bool wasTextInputActive = SDL_IsTextInputActive();
    const char *hint = SDL_GetHint("SDL_IME_SHOW_UI");
    char *savedHint = hint ? SDL_strdup(hint) : NULL;
    int size, result, imeRight, imeBottom;

    if (!townsJapanese || maximumLength <= 0) {
        SDL_free(savedHint);
        return 0;
    }
    inputLimit = SDL_min(maximumLength, sizeof(inputText) - 1);
    inputLimit = SDL_min(inputLimit, (field.right - context.cursorX - 3) / 4);
    if (inputLimit <= 0) {
        SDL_free(savedHint);
        return 0;
    }
    /* Limits and caret positions are bytes; each Shift-JIS character is atomic. */
    inputCursor = 0;
    while (destination[inputCursor]) {
        size = TownsLead((unsigned char)destination[inputCursor]) ? 2 : 1;
        if (inputCursor + size > inputLimit || (size == 2 && !destination[inputCursor + 1]))
            break;
        memcpy(inputText + inputCursor, destination + inputCursor, size);
        inputCursor += size;
    }
    inputText[inputCursor] = inputComposition[0] = 0;
    inputMode = mode;
    inputResult = inputSelected = inputComposing = inputCompositionChanged = 0;
    field.left = context.cursorX;
    field.top = context.cursorY;
    field.right = field.left + inputLimit * 4 + 3;
    field.bottom = SDL_min(field.bottom, field.top + SDL_max(9, FontHeight(context.font)) - 1);
    context.viewport = &field;
    context.backgroundColour = context.backgroundColour == 0xff ? cBlackColour : context.backgroundColour;
    pCurTextContext = &context;
    nMouseCursorShowCount = 0;
    SetEventManagerPump(NULL);
    SdlSuspendMouseGrab();
    FlushInputEvents();
    SDL_StopTextInput();
    SDL_FlushEvents(SDL_TEXTEDITING, SDL_TEXTINPUT);
    SDL_SetHint("SDL_IME_SHOW_UI", "1");
    ime = (SDL_Rect){field.left, field.top, 4, 9};
    imeRight = field.left + 4;
    imeBottom = field.top + 9;
    SdlMapLogicalToWindow(hMainWindow, field.left, field.top, &ime.x, &ime.y);
    SdlMapLogicalToWindow(hMainWindow, field.left + 4, field.top + 9, &imeRight, &imeBottom);
    ime.w = SDL_max(1, imeRight - ime.x);
    ime.h = SDL_max(1, imeBottom - ime.y);
    SDL_SetTextInputRect(&ime);
    inputActive = 1;
    SDL_StartTextInput();
    do {
        char display[sizeof(inputText) + sizeof(inputComposition)];
        const unsigned char *cursor;
        int compositionStart = inputSelected && inputComposing ? 0 : inputCursor;

        ClearViewport(&field, context.backgroundColour);
        if (inputResult < 0) {
            SDL_strlcpy(display, destination, sizeof(display));
        } else if (inputSelected && !inputComposing) {
            SDL_strlcpy(display, inputText, sizeof(display));
        } else {
            memcpy(display, inputText, compositionStart);
            strcpy(display + compositionStart, inputComposition);
            strcat(display, inputSelected ? "" : inputText + inputCursor);
        }
        context.cursorX = field.left;
        context.cursorY = field.top;
        cursor = (const unsigned char *)display;
        if (inputSelected && !inputComposing && !inputResult) {
            context.colour = savedContext->backgroundColour == 0xff ? cBlackColour : savedContext->backgroundColour;
            context.backgroundColour = savedContext->colour;
        }
        while (*cursor && context.cursorX <= field.right) {
            unsigned int glyph = TownsGlyph(&cursor, &size);
            TownsDrawGlyph(glyph, size);
        }
        context.colour = savedContext->colour;
        context.backgroundColour = savedContext->backgroundColour == 0xff ? cBlackColour : savedContext->backgroundColour;
        marker = field;
        marker.left += compositionStart * 4;
        marker.right = SDL_min(field.right, marker.left + SDL_max(1, strlen(inputComposition) * 4) - 1);
        if (inputComposing)
            marker.top = marker.bottom = SDL_min(field.bottom, field.top + 8);
        if (!inputResult && (inputComposing || !(SDL_GetTicks() / 500 % 2)))
            ClearViewport(&marker, context.colour);
        if (SdlMapLogicalToWindow(hMainWindow, marker.left, field.top, &ime.x, &ime.y))
            SDL_SetTextInputRect(&ime);
        DIBslam();
        DIBslamReal();
        if (inputResult)
            break;
        PumpWindowMessages();
        /* Committing a composition and accepting the field need separate presses. */
        inputCompositionChanged = 0;
        SDL_Delay(10);
    } while (1);
    result = inputResult > 0;
    if (result)
        strcpy(destination, inputText);
    inputActive = 0;
    SDL_StopTextInput();
    if (savedHint)
        SDL_SetHint("SDL_IME_SHOW_UI", savedHint);
    else
        SDL_ResetHint("SDL_IME_SHOW_UI");
    SDL_free(savedHint);
    if (wasTextInputActive)
        SDL_StartTextInput();
    FlushInputEvents();
    ClearInputKeyStatePreservingModifiers();
    /* Key releases are consumed by the editor, so refresh held modifiers. */
    SetInputKeyState(SCAN_CONTROL, (SDL_GetModState() & KMOD_CTRL) != 0);
    SetInputKeyState(SCAN_ALT, (SDL_GetModState() & KMOD_ALT) != 0);
    nSystemKeyDown = (SDL_GetModState() & KMOD_ALT) ? VK_MENU : 0;
    pCurTextContext = savedContext;
    if (result)
        savedContext->cursorX = field.left + strlen(inputText) * 4;
    nMouseCursorShowCount = savedCursor;
    SetEventManagerPump(savedPump);
    SdlResumeMouseGrab();
    return result;
}
