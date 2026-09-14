#ifndef WC1_SLINT_LAUNCHER_PATHS_H
#define WC1_SLINT_LAUNCHER_PATHS_H

#include <filesystem>
#include <string>
#include <string_view>

namespace wc1_launcher {

struct DirectoryStatus {
    bool valid;
    std::string message;
};

std::string path_to_utf8(const std::filesystem::path &path);
std::filesystem::path path_from_utf8(std::string_view value);
DirectoryStatus validate_game_directory(std::string_view directoryText);

} // namespace wc1_launcher

#endif
