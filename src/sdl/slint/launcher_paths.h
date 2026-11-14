#ifndef WC1_SLINT_LAUNCHER_PATHS_H
#define WC1_SLINT_LAUNCHER_PATHS_H

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

enum class LauncherMessage;

namespace wc1_launcher {

struct DirectoryStatus {
    bool valid;
    LauncherMessage message;
    bool swcDemo = false;
    std::string detail = {};
    bool sm1 = false;
    bool sm2 = false;
    bool swcCredits = false;
};

struct GameInstallation {
    std::string directory;
    LauncherMessage edition;
    bool segaCdVoices = false;
};

std::string path_to_utf8(const std::filesystem::path &path);
std::filesystem::path path_from_utf8(std::string_view value);
// The game resolves names without regard to ASCII case (src/sdl/compat.c).
std::optional<std::filesystem::path> find_child_case_insensitive(
    const std::filesystem::path &directory, std::string_view expected);
DirectoryStatus validate_game_directory(std::string_view directoryText);
// Add only validated data roots, resolving aliases to avoid duplicate choices.
// Returns the existing/new entry's index, or -1 for an invalid directory.
int add_game_installation(std::vector<GameInstallation> &installations,
                          std::string_view directory);
std::vector<GameInstallation> discover_game_installations();

} // namespace wc1_launcher

#endif
