#include "launcher_config.h"
#include "launcher_paths.h"

#include <SDL_error.h>
#include <SDL_filesystem.h>

#include <charconv>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <string_view>
#include <system_error>

namespace wc1_launcher {
namespace {

struct IntegerSetting {
    std::string_view name;
    int SdlLauncherOptions::*member;
    unsigned int explicitOption;
    int maximum;
};

constexpr IntegerSetting integerSettings[] = {
    {"language", &SdlLauncherOptions::language,
     SDL_LAUNCHER_EXPLICIT_LANGUAGE, 3},
    {"fullscreen", &SdlLauncherOptions::fullscreen,
     SDL_LAUNCHER_EXPLICIT_FULLSCREEN, 1},
    {"autosave", &SdlLauncherOptions::autosave,
     SDL_LAUNCHER_EXPLICIT_AUTOSAVE, 1},
    {"enhanced_renderer", &SdlLauncherOptions::enhancedRenderer,
     SDL_LAUNCHER_EXPLICIT_ENHANCED, 1},
    {"ega_dither", &SdlLauncherOptions::egaDither,
     SDL_LAUNCHER_EXPLICIT_EGA, 1},
    {"hires_hud_fonts", &SdlLauncherOptions::hiResHudFonts,
     SDL_LAUNCHER_EXPLICIT_HIRES_HUD, 1},
    {"frame_interpolation", &SdlLauncherOptions::frameInterpolation,
     SDL_LAUNCHER_EXPLICIT_INTERPOLATION, 1},
    {"cockpitless_view", &SdlLauncherOptions::cockpitlessView,
     SDL_LAUNCHER_EXPLICIT_COCKPITLESS, 1},
    {"cheat_mode", &SdlLauncherOptions::cheatMode,
     SDL_LAUNCHER_EXPLICIT_CHEATS, 1},
    {"joystick_rumble", &SdlLauncherOptions::joystickRumble,
     SDL_LAUNCHER_EXPLICIT_RUMBLE, 1},
    {"joystick_mode", &SdlLauncherOptions::joystickMode,
     SDL_LAUNCHER_EXPLICIT_JOYSTICK_MODE, SDL_LAUNCHER_JOYSTICK_STEAM_DECK},
    {"joystick_source", &SdlLauncherOptions::joystickSource, 0, SDL_PAD_SOURCE_JOYSTICK},
    {"joystick_axes", &SdlLauncherOptions::joystickAxes,
     SDL_LAUNCHER_EXPLICIT_JOYSTICK_AXES, SDL_LAUNCHER_AXES_RUDDER_ROLL},
    {"joystick_dead_zone", &SdlLauncherOptions::joystickDeadZone,
     SDL_LAUNCHER_EXPLICIT_JOYSTICK_DEAD_ZONE, SDL_JOYSTICK_DEAD_ZONE_MAXIMUM},
    {"music_device", &SdlLauncherOptions::musicDevice,
     SDL_LAUNCHER_EXPLICIT_MUSIC, SDL_LAUNCHER_MUSIC_MT32}
};

std::string_view trim_setting(std::string_view text)
{
    const auto first = text.find_first_not_of(" \t\r");
    if (first == std::string_view::npos)
        return {};
    const auto last = text.find_last_not_of(" \t\r");
    return text.substr(first, last - first + 1);
}

/* A name in the settings: lowercase letters, digits and dashes. */
bool valid_name(std::string_view name)
{
    for (const char character : name) {
        if (!((character >= 'a' && character <= 'z') ||
              (character >= '0' && character <= '9') || character == '-'))
            return false;
    }
    return true;
}

/* A [keys] line: an action name and its bindings, both plain ASCII with
 * other bytes percent-encoded (see src/sdl/keymap.c). */
bool valid_key_binding(std::string_view action, std::string_view bindings)
{
    if (action.empty())
        return false;
    for (const char character : action) {
        if (!((character >= 'a' && character <= 'z') ||
              (character >= '0' && character <= '9') || character == '-'))
            return false;
    }
    for (const char character : bindings) {
        if (!((character >= 'a' && character <= 'z') ||
              (character >= '0' && character <= '9') ||
              character == '-' || character == '%' || character == '+' || character == ','))
            return false;
    }
    return true;
}

bool valid_setting_utf8(std::string_view text)
{
    /* Slint rejects invalid UTF-8. SDL may skip or replace bad bytes, so a
     * conversion must reproduce the complete value before it reaches the GUI. */
    char *converted = SDL_iconv_string("UTF-8", "UTF-8", text.data(), text.size());
    if (converted == nullptr)
        return false;
    const bool valid = text == std::string_view(converted);
    SDL_free(converted);
    return valid;
}

/* A quoted absolute path setting, such as the game or ROM folder. */
bool read_path_setting(std::string_view value, char *target, size_t capacity)
{
    /* Only the surrounding quotes have syntax; backslashes and punctuation
     * inside the path are literal, including on Windows. */
    if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
        value = value.substr(1, value.size() - 2);
    if (value.empty() || value.size() >= capacity ||
        value.find_first_of("\r\n") != std::string_view::npos ||
        !valid_setting_utf8(value) || !path_from_utf8(value).is_absolute())
        return false;
    std::memcpy(target, value.data(), value.size());
    target[value.size()] = '\0';
    return true;
}

/* A libmt32emu machine id: lowercase letters, digits and underscores. */
bool valid_machine_id(std::string_view value)
{
    if (value.empty() || value.size() >= SDL_LAUNCHER_MT32_MODEL_CAPACITY)
        return false;
    for (const char character : value) {
        if (!((character >= 'a' && character <= 'z') ||
              (character >= '0' && character <= '9') || character == '_'))
            return false;
    }
    return true;
}

} // namespace

std::filesystem::path launcher_config_path()
{
    /* Share the existing SDL registry emulation's preference directory. */
    char *directory = SDL_GetPrefPath("Origin Systems", "WC Kilrathi Saga");
    if (directory == nullptr) {
        std::fprintf(stderr, "Cannot locate launcher settings: %s\n", SDL_GetError());
        return {};
    }
    const auto path = path_from_utf8(directory) / "launcher.ini";
    SDL_free(directory);
    return path;
}

void load_launcher_config(const std::filesystem::path &path,
                          SdlLauncherOptions &options)
{
    if (path.empty())
        return;
    std::ifstream input(path, std::ios::binary);
    std::string line;
    bool inLauncherSection = false;
    bool inKeysSection = false;
    bool inPadSection = false;
    bool firstLine = true;
    std::string keyBindings;
    std::string padBindings;

    while (std::getline(input, line)) {
        std::string_view text(line);
        if (firstLine && text.starts_with("\xef\xbb\xbf"))
            text.remove_prefix(3);
        firstLine = false;
        if (text.find('\0') != std::string_view::npos)
            continue;
        text = trim_setting(text);
        if (text.empty() || text.front() == ';' || text.front() == '#')
            continue;
        if (text.front() == '[') {
            inLauncherSection = text == "[launcher]";
            inKeysSection = text == "[keys]";
            inPadSection = text == "[pad]";
            continue;
        }
        const auto separator = text.find('=');
        if ((!inLauncherSection && !inKeysSection && !inPadSection) ||
            separator == std::string_view::npos)
            continue;
        const auto key = trim_setting(text.substr(0, separator));
        auto value = trim_setting(text.substr(separator + 1));
        if (inKeysSection) {
            /* An empty value is an action with no keys left. */
            if (valid_key_binding(key, value) &&
                keyBindings.size() + key.size() + value.size() + 2 < sizeof(options.keyBindings))
                keyBindings.append(key).append("=").append(value).append("\n");
            continue;
        }
        if (inPadSection) {
            /* A game controller input and its function (src/sdl/pad_bindings.h);
               an empty value leaves the input unbound. */
            if (!key.empty() && valid_name(key) && valid_name(value) &&
                padBindings.size() + key.size() + value.size() + 2 < sizeof(options.padBindings))
                padBindings.append(key).append("=").append(value).append("\n");
            continue;
        }
        if (value.empty())
            continue;
        if (key == "game_directory") {
            read_path_setting(value, options.gameDirectory, sizeof(options.gameDirectory));
            continue;
        }
        if (key == "mt32_roms") {
            if (!(options.explicitOptions & SDL_LAUNCHER_EXPLICIT_MT32_ROMS))
                read_path_setting(value, options.mt32Roms, sizeof(options.mt32Roms));
            continue;
        }
        if (key == "mt32_model") {
            if (!(options.explicitOptions & SDL_LAUNCHER_EXPLICIT_MT32_MODEL) &&
                valid_machine_id(value)) {
                std::memcpy(options.mt32Model, value.data(), value.size());
                options.mt32Model[value.size()] = '\0';
            }
            continue;
        }
        for (const auto &setting : integerSettings) {
            if (key != setting.name || (options.explicitOptions & setting.explicitOption))
                continue;
            int number;
            const auto result = std::from_chars(
                value.data(), value.data() + value.size(), number);
            if (result.ec == std::errc() && result.ptr == value.data() + value.size() &&
                number >= 0 && number <= setting.maximum)
                options.*setting.member = number;
            break;
        }
    }
    std::memcpy(options.keyBindings, keyBindings.c_str(), keyBindings.size() + 1);
    std::memcpy(options.padBindings, padBindings.c_str(), padBindings.size() + 1);
}

bool save_launcher_config(const std::filesystem::path &path,
                          const SdlLauncherOptions &options)
{
    if (path.empty() || std::strpbrk(options.gameDirectory, "\r\n") != nullptr ||
        std::strpbrk(options.mt32Roms, "\r\n") != nullptr)
        return false;
    auto temporaryPath = path;
    temporaryPath += ".tmp";
    std::ofstream output(temporaryPath, std::ios::binary | std::ios::trunc);
    if (!output)
        return false;
    output << "[launcher]\n"
           << "game_directory=\"" << options.gameDirectory << "\"\n";
    if (options.mt32Roms[0] != '\0')
        output << "mt32_roms=\"" << options.mt32Roms << "\"\n";
    if (valid_machine_id(options.mt32Model))
        output << "mt32_model=" << options.mt32Model << '\n';
    for (const auto &setting : integerSettings)
        output << setting.name << '=' << options.*setting.member << '\n';
    const std::string_view keyBindings(options.keyBindings,
                                       strnlen(options.keyBindings, sizeof(options.keyBindings)));
    if (!keyBindings.empty()) {
        output << "[keys]\n" << keyBindings;
        if (keyBindings.back() != '\n')
            output << '\n';
    }
    const std::string_view padBindings(options.padBindings,
                                       strnlen(options.padBindings, sizeof(options.padBindings)));
    if (!padBindings.empty()) {
        output << "[pad]\n" << padBindings;
        if (padBindings.back() != '\n')
            output << '\n';
    }
    output.close();

    std::error_code error;
    if (output) {
        std::filesystem::rename(temporaryPath, path, error);
        if (!error)
            return true;
    }
    std::filesystem::remove(temporaryPath, error);
    return false;
}

} // namespace wc1_launcher
