/* Exercise Japanese entry through the game's reader and SDL event pump.
 * Only presentation is replaced, to inject deterministic IME/keyboard events. */
#define DIBslamReal TestInputFrame
#include "../src/sdl/towns_text.c"

typedef struct InputStep {
    int frame;
    Uint32 type;
    const char *text;
    SDL_Keycode key;
    SDL_Keymod modifiers;
} InputStep;

static const InputStep *steps;
static int stepIndex, frameIndex, previews;
static int completed;

static void CheckCompleted(void)
{
    if (!completed) {
        fputs("FAIL: game terminated before the input checks completed\n", stderr);
        _Exit(1);
    }
}

static void Require(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "FAIL: %s (%s)\n", message, SDL_GetError());
        exit(1);
    }
}

void TestInputFrame(void)
{
    frameIndex++;
    if (frameIndex == 20)
        fprintf(stderr, "Input script: %s; step=%d length=%zu composition=%d result=%d\n",
                steps[0].text ? steps[0].text : "key", stepIndex, strlen(inputText), inputComposing, inputResult);
    Require(frameIndex < 20, "input finishes without another keyboard press");
    if (inputComposing) {
        Viewport *view = pCurTextContext->viewport;
        Require(view->pixels[view->rowOffsets[view->top + 8] + view->left] ==
                pCurTextContext->colour, "composition preview is underlined");
        previews++;
    }
    while (steps[stepIndex].frame == frameIndex) {
        const InputStep *step = &steps[stepIndex++];
        SDL_Event event = {0};
        event.type = step->type;
        if (event.type == SDL_TEXTINPUT)
            SDL_strlcpy(event.text.text, step->text, sizeof(event.text.text));
        else if (event.type == SDL_TEXTEDITING)
            SDL_strlcpy(event.edit.text, step->text, sizeof(event.edit.text));
        else {
            event.key.keysym.sym = step->key;
            event.key.keysym.scancode = SDL_GetScancodeFromKey(step->key);
            event.key.keysym.mod = step->modifiers;
        }
        Require(SDL_PushEvent(&event) == 1, "queue IME/keyboard input");
    }
}

static void RunInput(const InputStep *script, const char *initial, int limit,
                     int mode, int accepted, const char *expected)
{
    char destination[44], guard[44];
    TextContext *context = pCurTextContext;
    TextContext saved;
    SDL_Event event = {0};

    memset(destination, 0x55, sizeof(destination));
    strcpy(destination, initial);
    memcpy(guard, destination, sizeof(guard));
    context->cursorX = context->cursorY = 0;
    saved = *context;
    steps = script;
    frameIndex = stepIndex = 0;
    SetEventManagerPump(PollMenuInputDevices);
    nMouseCursorShowCount = 3;
    SDL_StopTextInput();
    SDL_SetHint("SDL_IME_SHOW_UI", "0");
    Require(ReadTextInput(destination, limit, mode) == accepted, "field accept/cancel result");
    Require(strcmp(destination, expected) == 0, "stored text matches expected Shift-JIS bytes");
    Require(!memcmp(destination + limit + 1, guard + limit + 1, sizeof(guard) - limit - 1),
            "field limit never overwrites the following storage");
    Require(steps[stepIndex].frame == 0, "IME commit does not also accept the field");
    Require(pCurTextContext == context && context->viewport == saved.viewport &&
            context->text == saved.text && context->backgroundColour == saved.backgroundColour &&
            pCurrentEventManagerPump == PollMenuInputDevices && nMouseCursorShowCount == 3,
            "restore game input and text context on every exit");
    Require(!SDL_IsTextInputActive() && !strcmp(SDL_GetHint("SDL_IME_SHOW_UI"), "0"),
            "restore IME activation and UI preference");
    event.type = SDL_TEXTINPUT;
    Require(!SdlHandleTownsTextInputEvent(&event), "IME hook is inactive outside text fields");
    SetEventManagerPump(NULL);
}

static void CheckSavedText(void)
{
    char directory[] = "/tmp/wc1-input-XXXXXX", previous[4096];
    DiskFileRecord records[17] = {0};
    SaveGameRecord original = {0}, restored = {0};
    FILE *file;

    Require(getcwd(previous, sizeof(previous)) != NULL && mkdtemp(directory) != NULL &&
            chdir(directory) == 0, "create isolated save test directory");
    file = fopen("SAVEGAME.WLD", "wb");
    Require(file != NULL && fclose(file) == 0, "create isolated save file");
    pDiskFileRecords = records;
    bCurrentRunningFromHardDiskFlag = 1;
    original.occupied = 1;
    strcpy(original.description, "\x93\xfa\x96\x7b\x8c\xea");
    strcpy(original.pilots[0].name, "\x8e\x52\x93\x63\x91\xbe\x98\x59");
    strcpy(original.pilots[0].callsign, "\x83\x5c\x81\x5b\x83\x68");
    Require(SaveGame(0, &original) && LoadGame(0, &restored), "save/load Japanese text");
    Require(!strcmp(original.description, restored.description) &&
            !memcmp(original.pilots, restored.pilots, sizeof(original.pilots)),
            "save format preserves Japanese description, name and callsign bytes");
    pDiskFileRecords = NULL;
    Require(unlink("SAVEGAME.WLD") == 0 && chdir(previous) == 0 && rmdir(directory) == 0,
            "remove isolated save test");
}

static void CheckInputIsolation(void)
{
    SDL_Event event = {0};
    int japanese;
    for (japanese = 0; japanese < 2; japanese++) {
        townsJapanese = japanese;
        event.type = SDL_KEYDOWN;
        event.key.keysym.sym = SDLK_a;
        event.key.keysym.scancode = SDL_SCANCODE_A;
        Require(!SdlHandleTownsTextInputEvent(&event), "inactive editor passes keyboard events");
        Require(SDL_PushEvent(&event) == 1, "queue ordinary game key");
        SdlPumpEvents();
        Require(abInputKeyStateArray[SCAN_A] && pInputEventHead &&
                pInputEventHead->type == INPUT_EVENT_KEY_DOWN,
                "ordinary gameplay key reaches the legacy input queue");
        FlushInputEvents();
        event.type = SDL_KEYUP;
        Require(SDL_PushEvent(&event) == 1, "queue ordinary game key release");
        SdlPumpEvents();
        Require(!abInputKeyStateArray[SCAN_A], "ordinary gameplay key release is preserved");
        FlushInputEvents();
    }
    townsJapanese = 0;
}

int main(void)
{
    unsigned char pixels[64000] = {0}, font[2] = {8, 0};
    unsigned short rows[200];
    Viewport viewport = {pixels, rows, 0, 0, 319, 199, 0};
    TextContext context = {0};
    int index;
    const InputStep composition[] = {
        {1, SDL_TEXTEDITING, "やまだ"},
        {2, SDL_TEXTEDITING, "山田"},
        {3, SDL_TEXTINPUT, "山田"},
        {3, SDL_KEYDOWN, NULL, SDLK_RETURN},
        {4, SDL_TEXTINPUT, "太郎"},
        {5, SDL_KEYDOWN, NULL, SDLK_RETURN}, {0}
    };
    const InputStep editing[] = {
        {1, SDL_TEXTINPUT, "山ソA"},
        {2, SDL_KEYDOWN, NULL, SDLK_LEFT},
        {3, SDL_KEYDOWN, NULL, SDLK_BACKSPACE},
        {4, SDL_KEYDOWN, NULL, SDLK_HOME},
        {5, SDL_KEYDOWN, NULL, SDLK_DELETE},
        {6, SDL_TEXTINPUT, "翼"},
        {7, SDL_KEYDOWN, NULL, SDLK_END},
        {7, SDL_KEYDOWN, NULL, SDLK_BACKSPACE},
        {8, SDL_KEYDOWN, NULL, SDLK_RETURN}, {0}
    };
    const InputStep limit[] = {
        {1, SDL_TEXTINPUT, "山田太郎山田太"},
        {2, SDL_TEXTINPUT, "A"},
        {3, SDL_KEYDOWN, NULL, SDLK_RETURN}, {0}
    };
    const InputStep cancel[] = {
        {1, SDL_KEYDOWN, NULL, SDLK_a, KMOD_CTRL},
        {2, SDL_TEXTINPUT, "山田"},
        {3, SDL_KEYDOWN, NULL, SDLK_ESCAPE}, {0}
    };
    const InputStep empty[] = {
        {1, SDL_KEYDOWN, NULL, SDLK_RETURN},
        {2, SDL_TEXTINPUT, "山"},
        {3, SDL_KEYDOWN, NULL, SDLK_RETURN}, {0}
    };
    const InputStep uppercase[] = {
        {1, SDL_TEXTINPUT, "abcヂdef"},
        {2, SDL_KEYDOWN, NULL, SDLK_RETURN}, {0}
    };
    const InputStep numeric[] = {
        {1, SDL_TEXTINPUT, "山１２3a45"},
        {2, SDL_KEYDOWN, NULL, SDLK_RETURN}, {0}
    };
    const InputStep unsupported[] = {
        {1, SDL_TEXTINPUT, "日😀%$本\n"},
        {2, SDL_KEYDOWN, NULL, SDLK_RETURN}, {0}
    };
    const InputStep replace[] = {
        {1, SDL_KEYDOWN, NULL, SDLK_a, KMOD_GUI},
        {2, SDL_TEXTINPUT, "ｶﾅ"},
        {3, SDL_KEYDOWN, NULL, SDLK_RETURN}, {0}
    };
    const InputStep paste[] = {
        {1, SDL_KEYDOWN, NULL, SDLK_v, KMOD_CTRL},
        {2, SDL_KEYDOWN, NULL, SDLK_RETURN}, {0}
    };
    const InputStep cancelledComposition[] = {
        {1, SDL_TEXTEDITING, "やま"},
        {2, SDL_TEXTEDITING, ""},
        {2, SDL_KEYDOWN, NULL, SDLK_ESCAPE},
        {3, SDL_TEXTINPUT, "山"},
        {4, SDL_KEYDOWN, NULL, SDLK_RETURN}, {0}
    };

    SDL_setenv("SDL_VIDEODRIVER", "dummy", 1);
    atexit(CheckCompleted);
    Require(SDL_Init(SDL_INIT_VIDEO) == 0, "initialize headless SDL input");
    for (index = 0; index < 200; index++)
        rows[index] = index * 320;
    context.viewport = &viewport;
    context.font = font;
    context.colour = 1;
    context.backgroundColour = 0xff;
    apViewportAllocations[0] = pixels;
    nViewAllocTotal = 1;
    SetTextContext(&context);
    /* Synthetic Japanese session: no proprietary executable/font is needed. */
    townsJapanese = 1;
    RunInput(composition, "", 13, 0, 1, "\x8e\x52\x93\x63\x91\xbe\x98\x59");
    Require(previews >= 2, "show IME reading and converted candidate before commit");
    RunInput(editing, "", 13, 0, 1, "\x97\x83");
    RunInput(limit, "", 13, 0, 1, "\x8e\x52\x93\x63\x91\xbe\x98\x59\x8e\x52\x93\x63" "A");
    RunInput(cancel, "Blair", 13, 0, 0, "Blair");
    RunInput(empty, "", 13, 0, 1, "\x8e\x52");
    RunInput(uppercase, "", 13, 1, 1, "ABC\x83\x61" "DEF");
    RunInput(numeric, "", 13, 2, 1, "345");
    RunInput(unsupported, "", 13, 0, 1, "\x93\xfa\x96\x7b");
    RunInput(replace, "Maverick", 13, 0, 1, "\xb6\xc5");
    SDL_SetClipboardText("山田");
    RunInput(paste, "", 13, 0, 1, "\x8e\x52\x93\x63");
    RunInput(cancelledComposition, "", 13, 0, 1, "\x8e\x52");
    SetInputKeyState(SCAN_CONTROL, 1);
    SetInputKeyState(SCAN_ALT, 1);
    nSystemKeyDown = VK_MENU;
    SDL_SetModState(KMOD_NONE); /* Both modifiers were released during the field. */
    RunInput(empty, "", 13, 0, 1, "\x8e\x52");
    Require(!abInputKeyStateArray[SCAN_CONTROL] && !abInputKeyStateArray[SCAN_ALT] &&
            !GetKeyboardModifiers(),
            "leaving Japanese entry cannot leave Ctrl/Alt stuck");
    SDL_SetModState(KMOD_CTRL | KMOD_ALT);
    RunInput(empty, "", 13, 0, 1, "\x8e\x52");
    Require(abInputKeyStateArray[SCAN_CONTROL] && abInputKeyStateArray[SCAN_ALT] &&
            GetKeyboardModifiers(),
            "leaving Japanese entry retains physically held modifiers");
    SDL_SetModState(KMOD_NONE);
    ClearInputKeyState();
    nSystemKeyDown = 0;
    townsJapanese = 0;
    Require(!SdlReadTownsTextInput(inputText, 13, 0) && !SDL_IsTextInputActive(),
            "Japanese editor cannot activate in other languages");
    CheckInputIsolation();
    CheckSavedText();
    SDL_Quit();
    completed = 1;
    puts("PASS: Japanese IME preview/commit, editing, byte limits, paste, cancellation, language gate and save/load.");
    return 0;
}
