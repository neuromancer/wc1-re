/* Keyboard rebinding for WC1's flight keys.
 *
 * The launcher's controls window passes the bindings that differ from the
 * original keys as "action=bindings" lines. Each value lists up to two
 * bindings separated by commas, each a key or "modifier+key". A key is what
 * it types on the player's layout ("e", "ñ") or a name for other keys ("up",
 * "f1", "ctrl"); bytes outside a-z, 0-9 and "-" are percent-encoded.
 *
 * Actions the player did not change keep the game's own handling, which
 * follows key positions. During flight, the keys of a changed action are
 * looked up on the current layout and stand in for the key the game reads,
 * and the action's original keys stop working. Mouse buttons and Escape keep
 * their original meanings. */
#include "wc1sdl.h"
#include "keymap_actions.h"

#include <string.h>

#define SDL_KEYMAP_BINDINGS 2
#define SDL_KEYMAP_NAME 24

typedef struct SdlKeyBinding {
    SDL_Scancode key;
    /* The other side of a modifier key, or the keypad Enter. */
    SDL_Scancode otherKey;
    Uint16 modifier;
} SdlKeyBinding;

typedef struct SdlKeyAction {
    const char *id;
    /* The key, and the modifier, that the game's own handler reads. */
    SDL_Scancode gameKey;
    Uint16 gameModifier;
    /* The original keys, by position. */
    SdlKeyBinding defaults[SDL_KEYMAP_BINDINGS];
} SdlKeyAction;

#define SDL_KEYMAP_ACTION(id, key, modifier, ...) {id, key, modifier, {__VA_ARGS__}},
static const SdlKeyAction g_aSdlKeyActions[] = {
    SDL_KEYMAP_ACTION_LIST(SDL_KEYMAP_ACTION)
};
#undef SDL_KEYMAP_ACTION

#define SDL_KEYMAP_ACTIONS \
    ((int)(sizeof(g_aSdlKeyActions) / sizeof(g_aSdlKeyActions[0])))

typedef struct SdlKeyNames {
    char modifier[SDL_KEYMAP_NAME];
    char key[SDL_KEYMAP_NAME];
} SdlKeyNames;

typedef struct SdlChangedAction {
    int changed;
    int count;
    SdlKeyNames names[SDL_KEYMAP_BINDINGS];
    SdlKeyBinding bindings[SDL_KEYMAP_BINDINGS];
} SdlChangedAction;

static SdlChangedAction g_aSdlChangedActions[SDL_KEYMAP_ACTIONS];
static SdlChangedAction g_stSdlFullscreenBindings;
static int g_bSdlKeymapChanged;
static int g_bSdlKeymapResolved;
/* What each pressed key was turned into: -1 untouched, 0 dropped. */
static int g_aSdlKeyTranslations[SDL_NUM_SCANCODES];
static int g_bSdlKeyTranslationsReady;
static Uint8 g_abSdlVirtualKeys[SDL_NUM_SCANCODES];

typedef struct SdlNamedKey {
    const char *name;
    SDL_Scancode key;
    SDL_Scancode otherKey;
    Uint16 modifier;
} SdlNamedKey;

static const SdlNamedKey g_aSdlNamedKeys[] = {
    {"up", SDL_SCANCODE_UP}, {"down", SDL_SCANCODE_DOWN},
    {"left", SDL_SCANCODE_LEFT}, {"right", SDL_SCANCODE_RIGHT},
    {"home", SDL_SCANCODE_HOME}, {"end", SDL_SCANCODE_END},
    {"page-up", SDL_SCANCODE_PAGEUP}, {"page-down", SDL_SCANCODE_PAGEDOWN},
    {"insert", SDL_SCANCODE_INSERT}, {"delete", SDL_SCANCODE_DELETE},
    {"backspace", SDL_SCANCODE_BACKSPACE}, {"tab", SDL_SCANCODE_TAB},
    {"space", SDL_SCANCODE_SPACE},
    {"enter", SDL_SCANCODE_RETURN, SDL_SCANCODE_KP_ENTER},
    {"escape", SDL_SCANCODE_ESCAPE},
    {"shift", SDL_SCANCODE_LSHIFT, SDL_SCANCODE_RSHIFT, KMOD_SHIFT},
    {"ctrl", SDL_SCANCODE_LCTRL, SDL_SCANCODE_RCTRL, KMOD_CTRL},
    {"alt", SDL_SCANCODE_LALT, SDL_SCANCODE_RALT, KMOD_ALT},
    {"alt-gr", SDL_SCANCODE_RALT, SDL_SCANCODE_UNKNOWN, KMOD_RALT},
    {"meta", SDL_SCANCODE_LGUI, SDL_SCANCODE_RGUI, KMOD_GUI},
    {"caps-lock", SDL_SCANCODE_CAPSLOCK}, {"scroll-lock", SDL_SCANCODE_SCROLLLOCK},
    {"pause", SDL_SCANCODE_PAUSE}, {"print-screen", SDL_SCANCODE_PRINTSCREEN},
    {"menu", SDL_SCANCODE_APPLICATION},
    {"f1", SDL_SCANCODE_F1}, {"f2", SDL_SCANCODE_F2}, {"f3", SDL_SCANCODE_F3},
    {"f4", SDL_SCANCODE_F4}, {"f5", SDL_SCANCODE_F5}, {"f6", SDL_SCANCODE_F6},
    {"f7", SDL_SCANCODE_F7}, {"f8", SDL_SCANCODE_F8}, {"f9", SDL_SCANCODE_F9},
    {"f10", SDL_SCANCODE_F10}, {"f11", SDL_SCANCODE_F11}, {"f12", SDL_SCANCODE_F12}
};

/* The original keys' symbols where a layout lacks them as unshifted keys,
 * such as "+" on a US keyboard: keep the key the game has always read. */
static const SdlNamedKey g_aSdlOriginalSymbols[] = {
    {"+", SDL_SCANCODE_EQUALS}, {"-", SDL_SCANCODE_MINUS},
    {",", SDL_SCANCODE_COMMA}, {".", SDL_SCANCODE_PERIOD},
    {"\\", SDL_SCANCODE_BACKSLASH}
};

/* Symbols typed with Shift on a US keyboard, and their unshifted keys. */
static const char *const g_aSdlShiftedSymbols[][2] = {
    {"!", "1"}, {"@", "2"}, {"#", "3"}, {"$", "4"}, {"%", "5"}, {"^", "6"},
    {"&", "7"}, {"*", "8"}, {"(", "9"}, {")", "0"}, {"_", "-"}, {"{", "["},
    {"}", "]"}, {"|", "\\"}, {":", ";"}, {"\"", "'"}, {"<", ","},
    {">", "."}, {"?", "/"}, {"~", "`"}
};

static int SdlKeymapHexDigit(char digit)
{
    if (digit >= '0' && digit <= '9')
        return digit - '0';
    if (digit >= 'a' && digit <= 'f')
        return digit - 'a' + 10;
    if (digit >= 'A' && digit <= 'F')
        return digit - 'A' + 10;
    return -1;
}

/* Decodes one percent-encoded name of length bytes; 0 when malformed. */
static int SdlKeymapDecodeName(const char *text, size_t length, char *name)
{
    size_t used = 0;
    size_t index;
    int high;
    int low;

    for (index = 0; index < length; index++) {
        char character = text[index];

        if (character == '%') {
            if (index + 2 >= length)
                return 0;
            high = SdlKeymapHexDigit(text[index + 1]);
            low = SdlKeymapHexDigit(text[index + 2]);
            if (high < 0 || low < 0)
                return 0;
            character = (char)(high * 16 + low);
            index += 2;
        } else if (!((character >= 'a' && character <= 'z') ||
                     (character >= '0' && character <= '9') || character == '-')) {
            return 0;
        }
        if (used + 1 >= SDL_KEYMAP_NAME)
            return 0;
        name[used++] = character;
    }
    name[used] = '\0';
    return used > 0;
}

static int SdlKeymapIsMouse(const char *name)
{
    return strncmp(name, "mouse-", 6) == 0 || strncmp(name, "wheel-", 6) == 0;
}

static int SdlKeymapFindAction(const char *id, size_t length)
{
    int action;

    for (action = 0; action < SDL_KEYMAP_ACTIONS; action++) {
        if (strlen(g_aSdlKeyActions[action].id) == length &&
            memcmp(g_aSdlKeyActions[action].id, id, length) == 0)
            return action;
    }
    return -1;
}

/* Reads one "binding,binding" value into names; mouse bindings are skipped. */
static void SdlKeymapParseValue(const char *value, size_t length,
                                SdlChangedAction *action)
{
    size_t start = 0;

    memset(action, 0, sizeof(*action));
    action->changed = 1;
    while (start <= length && action->count < SDL_KEYMAP_BINDINGS) {
        size_t end = start;
        size_t plus;
        SdlKeyNames names;

        while (end < length && value[end] != ',')
            end++;
        memset(&names, 0, sizeof(names));
        for (plus = start; plus < end && value[plus] != '+'; plus++)
            ;
        if (plus < end) {
            if (SdlKeymapDecodeName(value + start, plus - start, names.modifier) &&
                SdlKeymapDecodeName(value + plus + 1, end - plus - 1, names.key) &&
                !SdlKeymapIsMouse(names.key))
                action->names[action->count++] = names;
        } else if (end > start &&
                   SdlKeymapDecodeName(value + start, end - start, names.key) &&
                   !SdlKeymapIsMouse(names.key)) {
            action->names[action->count++] = names;
        }
        if (end >= length)
            break;
        start = end + 1;
    }
}

void SdlSetKeyBindings(const char *text)
{
    const char *line = text;

    memset(g_aSdlChangedActions, 0, sizeof(g_aSdlChangedActions));
    memset(&g_stSdlFullscreenBindings, 0, sizeof(g_stSdlFullscreenBindings));
    g_bSdlKeymapChanged = 0;
    g_bSdlKeymapResolved = 0;
    while (line != NULL && *line != '\0') {
        const char *end = strchr(line, '\n');
        const char *separator;
        size_t length = end != NULL ? (size_t)(end - line) : strlen(line);
        int action;

        separator = memchr(line, '=', length);
        if (separator != NULL) {
            size_t idLength = (size_t)(separator - line);
            const char *value = separator + 1;
            size_t valueLength = length - idLength - 1;

            if (idLength == strlen("full-screen") &&
                memcmp(line, "full-screen", idLength) == 0) {
                SdlKeymapParseValue(value, valueLength, &g_stSdlFullscreenBindings);
            } else if ((action = SdlKeymapFindAction(line, idLength)) >= 0) {
                SdlKeymapParseValue(value, valueLength, &g_aSdlChangedActions[action]);
                g_bSdlKeymapChanged = 1;
            }
        }
        line = end != NULL ? end + 1 : NULL;
    }
}

static const SdlNamedKey *SdlKeymapFindNamed(const SdlNamedKey *keys, size_t count,
                                             const char *name)
{
    size_t index;

    for (index = 0; index < count; index++) {
        if (strcmp(keys[index].name, name) == 0)
            return &keys[index];
    }
    return NULL;
}

/* One UTF-8 character's code point; 0 unless name is exactly one. */
static Uint32 SdlKeymapCodePoint(const char *name)
{
    const unsigned char *bytes = (const unsigned char *)name;
    Uint32 codePoint;
    int extra;
    int index;

    if (bytes[0] < 0x80) {
        codePoint = bytes[0];
        extra = 0;
    } else if ((bytes[0] & 0xe0) == 0xc0) {
        codePoint = bytes[0] & 0x1f;
        extra = 1;
    } else if ((bytes[0] & 0xf0) == 0xe0) {
        codePoint = bytes[0] & 0x0f;
        extra = 2;
    } else if ((bytes[0] & 0xf8) == 0xf0) {
        codePoint = bytes[0] & 0x07;
        extra = 3;
    } else {
        return 0;
    }
    for (index = 1; index <= extra; index++) {
        if ((bytes[index] & 0xc0) != 0x80)
            return 0;
        codePoint = (codePoint << 6) | (bytes[index] & 0x3f);
    }
    return bytes[extra + 1] == '\0' ? codePoint : 0;
}

/* Finds the key that types name on the current layout. */
static int SdlKeymapResolveKey(const char *name, SdlKeyBinding *binding)
{
    const SdlNamedKey *named;
    Uint32 codePoint;
    size_t index;

    named = SdlKeymapFindNamed(g_aSdlNamedKeys,
                               sizeof(g_aSdlNamedKeys) / sizeof(g_aSdlNamedKeys[0]), name);
    if (named != NULL) {
        binding->key = named->key;
        binding->otherKey = named->otherKey;
        return 1;
    }
    codePoint = SdlKeymapCodePoint(name);
    if (codePoint == 0)
        return 0;
    binding->key = SDL_GetScancodeFromKey((SDL_Keycode)codePoint);
    if (binding->key != SDL_SCANCODE_UNKNOWN)
        return 1;
    named = SdlKeymapFindNamed(g_aSdlOriginalSymbols,
                               sizeof(g_aSdlOriginalSymbols) / sizeof(g_aSdlOriginalSymbols[0]),
                               name);
    if (named != NULL) {
        binding->key = named->key;
        return 1;
    }
    for (index = 0; index < sizeof(g_aSdlShiftedSymbols) / sizeof(g_aSdlShiftedSymbols[0]);
         index++) {
        if (strcmp(g_aSdlShiftedSymbols[index][0], name) == 0) {
            binding->key = SDL_GetScancodeFromKey(
                (SDL_Keycode)(unsigned char)g_aSdlShiftedSymbols[index][1][0]);
            binding->modifier |= KMOD_SHIFT;
            return binding->key != SDL_SCANCODE_UNKNOWN;
        }
    }
    return 0;
}

static void SdlKeymapResolveAction(const char *id, SdlChangedAction *action)
{
    int index;

    for (index = 0; index < action->count; index++) {
        SdlKeyBinding *binding = &action->bindings[index];
        const SdlNamedKey *modifier;

        memset(binding, 0, sizeof(*binding));
        if (action->names[index].modifier[0] != '\0') {
            modifier = SdlKeymapFindNamed(g_aSdlNamedKeys,
                                          sizeof(g_aSdlNamedKeys) / sizeof(g_aSdlNamedKeys[0]),
                                          action->names[index].modifier);
            if (modifier == NULL || modifier->modifier == 0) {
                fprintf(stderr, "Key binding for %s: %s is not a modifier.\n",
                        id, action->names[index].modifier);
                continue;
            }
            binding->modifier = modifier->modifier;
        }
        if (!SdlKeymapResolveKey(action->names[index].key, binding)) {
            fprintf(stderr, "Key binding for %s: no key types \"%s\" on this keyboard.\n",
                    id, action->names[index].key);
            memset(binding, 0, sizeof(*binding));
        } else if (binding->key == SDL_SCANCODE_ESCAPE) {
            /* Escape keeps all its meanings; it is never rebound. */
            memset(binding, 0, sizeof(*binding));
        }
    }
}

static void SdlKeymapResolve(void)
{
    int action;

    if (g_bSdlKeymapResolved)
        return;
    g_bSdlKeymapResolved = 1;
    for (action = 0; action < SDL_KEYMAP_ACTIONS; action++) {
        if (g_aSdlChangedActions[action].changed)
            SdlKeymapResolveAction(g_aSdlKeyActions[action].id, &g_aSdlChangedActions[action]);
    }
    if (g_stSdlFullscreenBindings.changed)
        SdlKeymapResolveAction("full-screen", &g_stSdlFullscreenBindings);
}

void SdlKeymapInvalidate(void)
{
    g_bSdlKeymapResolved = 0;
}

static int SdlKeymapActive(void)
{
    if (!g_bSdlKeymapChanged || !SdlSpaceflightInputActive())
        return 0;
    SdlKeymapResolve();
    return 1;
}

static int SdlKeymapMatches(const SdlKeyBinding *binding, SDL_Scancode key,
                            Uint16 modifiers)
{
    if (binding->key == SDL_SCANCODE_UNKNOWN ||
        (binding->key != key && binding->otherKey != key))
        return 0;
    return binding->modifier == 0 || (modifiers & binding->modifier) != 0;
}

/* The changed action a key press gives, preferring bindings with a held
 * modifier over plain ones; -1 when none. */
static int SdlKeymapChangedAction(SDL_Scancode key, Uint16 modifiers)
{
    int pass;
    int action;
    int index;

    for (pass = 0; pass < 2; pass++) {
        for (action = 0; action < SDL_KEYMAP_ACTIONS; action++) {
            const SdlChangedAction *changed = &g_aSdlChangedActions[action];

            if (!changed->changed)
                continue;
            for (index = 0; index < changed->count; index++) {
                const SdlKeyBinding *binding = &changed->bindings[index];

                if ((pass == 0) == (binding->modifier != 0) &&
                    SdlKeymapMatches(binding, key, modifiers))
                    return action;
            }
        }
    }
    return -1;
}

/* The action a key press has in the original game, preferring bindings with
 * a held modifier (Ctrl+M) over plain ones (M); -1 when none. */
static int SdlKeymapOriginalAction(SDL_Scancode key, Uint16 modifiers)
{
    int pass;
    int action;
    int index;

    for (pass = 0; pass < 2; pass++) {
        for (action = 0; action < SDL_KEYMAP_ACTIONS; action++) {
            for (index = 0; index < SDL_KEYMAP_BINDINGS; index++) {
                const SdlKeyBinding *binding = &g_aSdlKeyActions[action].defaults[index];

                if ((pass == 0) == (binding->modifier != 0) &&
                    SdlKeymapMatches(binding, key, modifiers))
                    return action;
            }
        }
    }
    return -1;
}

SDL_Scancode SdlKeymapTranslateKey(SDL_Scancode key, Uint16 modifiers, int pressed)
{
    int action;
    int translation;

    if (key <= SDL_SCANCODE_UNKNOWN || key >= SDL_NUM_SCANCODES)
        return key;
    if (!g_bSdlKeyTranslationsReady) {
        for (translation = 0; translation < SDL_NUM_SCANCODES; translation++)
            g_aSdlKeyTranslations[translation] = -1;
        g_bSdlKeyTranslationsReady = 1;
    }
    if (!pressed) {
        /* Release what the press became, even if flight ended meanwhile. */
        translation = g_aSdlKeyTranslations[key];
        g_aSdlKeyTranslations[key] = -1;
        return translation < 0 ? key : (SDL_Scancode)translation;
    }
    if (!SdlKeymapActive() || key == SDL_SCANCODE_ESCAPE) {
        g_aSdlKeyTranslations[key] = -1;
        return key;
    }
    action = SdlKeymapChangedAction(key, modifiers);
    if (action >= 0) {
        g_aSdlKeyTranslations[key] = g_aSdlKeyActions[action].gameKey;
        return g_aSdlKeyActions[action].gameKey;
    }
    action = SdlKeymapOriginalAction(key, modifiers);
    if (action >= 0 && g_aSdlChangedActions[action].changed) {
        g_aSdlKeyTranslations[key] = SDL_SCANCODE_UNKNOWN;
        return SDL_SCANCODE_UNKNOWN;
    }
    g_aSdlKeyTranslations[key] = -1;
    return key;
}

static Uint16 SdlKeymapHeldModifiers(const Uint8 *keys)
{
    Uint16 modifiers = 0;

    if (keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT])
        modifiers |= KMOD_SHIFT;
    if (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL])
        modifiers |= KMOD_CTRL;
    if (keys[SDL_SCANCODE_LALT])
        modifiers |= KMOD_LALT;
    if (keys[SDL_SCANCODE_RALT])
        modifiers |= KMOD_RALT;
    if (keys[SDL_SCANCODE_LGUI] || keys[SDL_SCANCODE_RGUI])
        modifiers |= KMOD_GUI;
    return modifiers;
}

static int SdlKeymapHeld(const SdlKeyBinding *binding, const Uint8 *keys,
                         Uint16 modifiers)
{
    if (binding->key == SDL_SCANCODE_UNKNOWN)
        return 0;
    if (!keys[binding->key] &&
        (binding->otherKey == SDL_SCANCODE_UNKNOWN || !keys[binding->otherKey]))
        return 0;
    return binding->modifier == 0 || (modifiers & binding->modifier) != 0;
}

/* A held binding counts unless it is a plain key whose binding with a held
 * modifier wins, as Ctrl+K does over K. */
static int SdlKeymapActiveBinding(const SdlKeyBinding *binding, const Uint8 *keys,
                                  Uint16 modifiers)
{
    int action;
    int index;

    if (!SdlKeymapHeld(binding, keys, modifiers))
        return 0;
    if (binding->modifier != 0)
        return 1;
    for (action = 0; action < SDL_KEYMAP_ACTIONS; action++) {
        const SdlChangedAction *changed = &g_aSdlChangedActions[action];

        for (index = 0; changed->changed && index < changed->count; index++) {
            const SdlKeyBinding *other = &changed->bindings[index];

            if (other->modifier != 0 && other->key == binding->key &&
                SdlKeymapHeld(other, keys, modifiers))
                return 0;
        }
    }
    return 1;
}

static void SdlKeymapReleaseModifier(Uint16 modifier)
{
    if (modifier & KMOD_SHIFT) {
        g_abSdlVirtualKeys[SDL_SCANCODE_LSHIFT] = 0;
        g_abSdlVirtualKeys[SDL_SCANCODE_RSHIFT] = 0;
    }
    if (modifier & KMOD_CTRL) {
        g_abSdlVirtualKeys[SDL_SCANCODE_LCTRL] = 0;
        g_abSdlVirtualKeys[SDL_SCANCODE_RCTRL] = 0;
    }
    if (modifier & KMOD_LALT)
        g_abSdlVirtualKeys[SDL_SCANCODE_LALT] = 0;
    if (modifier & KMOD_RALT)
        g_abSdlVirtualKeys[SDL_SCANCODE_RALT] = 0;
    if (modifier & KMOD_GUI) {
        g_abSdlVirtualKeys[SDL_SCANCODE_LGUI] = 0;
        g_abSdlVirtualKeys[SDL_SCANCODE_RGUI] = 0;
    }
}

/* The keys the game should see held: a changed action's keys hold the key
 * the game reads, keys whose action moved away read as released, and a
 * modifier used by a binding stands only for that binding. */
const Uint8 *SdlKeymapKeyboardState(const Uint8 *keys)
{
    Uint16 modifiers;
    Uint16 consumed = 0;
    Uint16 required = 0;
    int key;
    int action;
    int index;

    if (!SdlKeymapActive())
        return keys;
    memcpy(g_abSdlVirtualKeys, keys, sizeof(g_abSdlVirtualKeys));
    modifiers = SdlKeymapHeldModifiers(keys);
    for (key = SDL_SCANCODE_UNKNOWN + 1; key < SDL_NUM_SCANCODES; key++) {
        if (!keys[key] || key == SDL_SCANCODE_ESCAPE)
            continue;
        action = SdlKeymapOriginalAction((SDL_Scancode)key, modifiers);
        if (action >= 0 && g_aSdlChangedActions[action].changed)
            g_abSdlVirtualKeys[key] = 0;
    }
    for (action = 0; action < SDL_KEYMAP_ACTIONS; action++) {
        const SdlChangedAction *changed = &g_aSdlChangedActions[action];

        for (index = 0; changed->changed && index < changed->count; index++) {
            const SdlKeyBinding *binding = &changed->bindings[index];

            if (!SdlKeymapActiveBinding(binding, keys, modifiers))
                continue;
            g_abSdlVirtualKeys[binding->key] = 0;
            if (binding->otherKey != SDL_SCANCODE_UNKNOWN)
                g_abSdlVirtualKeys[binding->otherKey] = 0;
            consumed |= binding->modifier;
        }
    }
    SdlKeymapReleaseModifier(consumed);
    for (action = 0; action < SDL_KEYMAP_ACTIONS; action++) {
        const SdlChangedAction *changed = &g_aSdlChangedActions[action];

        for (index = 0; changed->changed && index < changed->count; index++) {
            if (!SdlKeymapActiveBinding(&changed->bindings[index], keys, modifiers))
                continue;
            g_abSdlVirtualKeys[g_aSdlKeyActions[action].gameKey] = 1;
            required |= g_aSdlKeyActions[action].gameModifier;
        }
    }
    if (required & KMOD_CTRL)
        g_abSdlVirtualKeys[SDL_SCANCODE_LCTRL] = 1;
    return g_abSdlVirtualKeys;
}

int SdlKeymapFullscreenShortcut(SDL_Scancode key, Uint16 modifiers)
{
    int index;

    if (!g_stSdlFullscreenBindings.changed)
        return -1;
    SdlKeymapResolve();
    for (index = 0; index < g_stSdlFullscreenBindings.count; index++) {
        if (SdlKeymapMatches(&g_stSdlFullscreenBindings.bindings[index], key, modifiers))
            return 1;
    }
    return 0;
}
