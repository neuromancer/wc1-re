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
    {"cockpitless_view", &SdlLauncherOptions::cockpitlessView,
     SDL_LAUNCHER_EXPLICIT_COCKPITLESS, 1},
    {"cheat_mode", &SdlLauncherOptions::cheatMode,
     SDL_LAUNCHER_EXPLICIT_CHEATS, 1},
    {"joystick_rumble", &SdlLauncherOptions::joystickRumble,
     SDL_LAUNCHER_EXPLICIT_RUMBLE, 1},
    {"joystick_mode", &SdlLauncherOptions::joystickMode,
     SDL_LAUNCHER_EXPLICIT_JOYSTICK_MODE,
     SDL_LAUNCHER_JOYSTICK_FOUR_BUTTON_FOUR_AXIS},
    {"joystick_axes", &SdlLauncherOptions::joystickAxes,
     SDL_LAUNCHER_EXPLICIT_JOYSTICK_AXES, SDL_LAUNCHER_AXES_RUDDER_ROLL},
    {"joystick_dead_zone", &SdlLauncherOptions::joystickDeadZone,
     SDL_LAUNCHER_EXPLICIT_JOYSTICK_DEAD_ZONE, SDL_JOYSTICK_DEAD_ZONE_MAXIMUM}
};

std::string_view trim_setting(std::string_view text)
{
    const auto first = text.find_first_not_of(" \t\r");
    if (first == std::string_view::npos)
        return {};
    const auto last = text.find_last_not_of(" \t\r");
    return text.substr(first, last - first + 1);
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
    bool firstLine = true;
    std::string keyBindings;

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
            continue;
        }
        const auto separator = text.find('=');
        if ((!inLauncherSection && !inKeysSection) || separator == std::string_view::npos)
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
        if (value.empty())
            continue;
        if (key == "game_directory") {
            /* Only the surrounding quotes have syntax; backslashes and
             * punctuation inside the path are literal, including on Windows. */
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
                value = value.substr(1, value.size() - 2);
            if (!value.empty() && value.size() < sizeof(options.gameDirectory) &&
                value.find_first_of("\r\n") == std::string_view::npos &&
                valid_setting_utf8(value) &&
                path_from_utf8(value).is_absolute()) {
                std::memcpy(options.gameDirectory, value.data(), value.size());
                options.gameDirectory[value.size()] = '\0';
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
}

bool save_launcher_config(const std::filesystem::path &path,
                          const SdlLauncherOptions &options)
{
    if (path.empty() || std::strpbrk(options.gameDirectory, "\r\n") != nullptr)
        return false;
    auto temporaryPath = path;
    temporaryPath += ".tmp";
    std::ofstream output(temporaryPath, std::ios::binary | std::ios::trunc);
    if (!output)
        return false;
    output << "[launcher]\n"
           << "game_directory=\"" << options.gameDirectory << "\"\n";
    for (const auto &setting : integerSettings)
        output << setting.name << '=' << options.*setting.member << '\n';
    const std::string_view keyBindings(options.keyBindings,
                                       strnlen(options.keyBindings, sizeof(options.keyBindings)));
    if (!keyBindings.empty()) {
        output << "[keys]\n" << keyBindings;
        if (keyBindings.back() != '\n')
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
