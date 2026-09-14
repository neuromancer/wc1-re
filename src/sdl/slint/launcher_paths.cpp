#include "launcher_paths.h"
#include "launcher_api.h"

#include <algorithm>
#include <array>
#include <fstream>
#include <optional>
#include <system_error>

namespace wc1_launcher {

namespace fs = std::filesystem;

std::string path_to_utf8(const fs::path &path)
{
    const auto utf8 = path.u8string();
    return std::string(utf8.begin(), utf8.end());
}

fs::path path_from_utf8(std::string_view value)
{
    std::u8string utf8;

    utf8.reserve(value.size());
    for (const unsigned char character : value)
        utf8.push_back(static_cast<char8_t>(character));
    return fs::path(utf8);
}

static bool equal_ascii_case(std::string_view left, std::string_view right)
{
    if (left.size() != right.size())
        return false;
    return std::equal(
        left.begin(), left.end(), right.begin(),
        [](unsigned char leftCharacter, unsigned char rightCharacter) {
            if (leftCharacter >= 'A' && leftCharacter <= 'Z')
                leftCharacter += 'a' - 'A';
            if (rightCharacter >= 'A' && rightCharacter <= 'Z')
                rightCharacter += 'a' - 'A';
            return leftCharacter == rightCharacter;
        });
}

static std::optional<fs::path> find_child_case_insensitive(
    const fs::path &directory, std::string_view expected)
{
    std::error_code error;
    fs::directory_iterator iterator(directory, error);
    const fs::directory_iterator end;

    while (!error && iterator != end) {
        if (equal_ascii_case(path_to_utf8(iterator->path().filename()), expected))
            return iterator->path();
        iterator.increment(error);
    }
    return std::nullopt;
}

static bool has_readable_file(const fs::path &directory, std::string_view name)
{
    const auto path = find_child_case_insensitive(directory, name);
    std::error_code error;

    return path && fs::is_regular_file(*path, error) &&
        std::ifstream(*path, std::ios::binary).peek() != std::ifstream::traits_type::eof();
}

DirectoryStatus validate_game_directory(std::string_view directoryText)
{
    if (directoryText.empty())
        return {false, "Choose a Wing Commander directory."};
    if (directoryText.size() >= SDL_LAUNCHER_DIRECTORY_CAPACITY)
        return {false, "The directory path is too long."};

    const fs::path directory = path_from_utf8(directoryText);
    std::error_code error;

    if (!fs::is_directory(directory, error))
        return {false, "The selected path is not a directory."};
    const auto gameData = find_child_case_insensitive(directory, "GAMEDAT");
    if (!gameData || !fs::is_directory(*gameData, error))
        return {false, "Choose the parent directory containing GAMEDAT."};

    const auto module = find_child_case_insensitive(*gameData, "MODULE.000");
    if (!module || !fs::is_regular_file(*module, error))
        return {false, "GAMEDAT/MODULE.000 is missing. Select a WC1 installation."};
    std::ifstream moduleFile(*module, std::ios::binary);
    std::array<unsigned char, 8> header = {};

    moduleFile.read(reinterpret_cast<char *>(header.data()), header.size());
    if (moduleFile.gcount() != static_cast<std::streamsize>(header.size()))
        return {false, "GAMEDAT/MODULE.000 is unreadable or too small to identify."};
    for (const auto name : {"CAMP.000", "INSTALL.DAT"}) {
        if (!has_readable_file(*gameData, name))
            return {false, std::string("GAMEDAT/") + name + " is missing or unreadable."};
    }

    // Match SdlUsingDosData: compressed DOS MODULE.000 has flag 1 in byte 7.
    if (header[7] == 1)
        return {true, "Wing Commander DOS data found (partial support)."};
    const auto streams = find_child_case_insensitive(directory, "STREAMS");
    if (!streams || !fs::is_directory(*streams, error))
        return {false, "Kilrathi Saga data needs a sibling STREAMS directory."};
    for (const auto name : {"PREFLITE.STR", "POSFLITE.STR", "MISSION.STR"}) {
        if (!has_readable_file(*streams, name))
            return {false, std::string("STREAMS/") + name + " is missing or unreadable."};
    }
    return {true, "Wing Commander Kilrathi Saga data found."};
}

} // namespace wc1_launcher
