#include "launcher_paths.h"
#include "launcher_api.h"

#include <SDL_filesystem.h>

#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <optional>
#include <system_error>

#ifndef _WIN32
#include <pwd.h>
#include <unistd.h>
#endif

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
    const auto cmfs = find_child_case_insensitive(directory, "CMFs");
    // WC1 data (GAMEDAT/MODULE.000) wins over the demo's CMFs in a mixed
    // directory; the command line applies the same rule to its directory.
    const bool wc1Data = gameData && fs::is_directory(*gameData, error) &&
        has_readable_file(*gameData, "MODULE.000");
    if (!wc1Data && cmfs && fs::is_directory(*cmfs, error)) {
        for (const auto name : {"Data.CMF", "Spaceflight.CMF", "PCShipV00.CMF",
                                "RecRoomScene.CMF", "Barracks.CMF", "ViewMedals.CMF",
                                "Head10.CMF", "RRTalkingHeads.CMF", "Briefing.CMF",
                                "BriefingCommander.CMF", "BRTH1.CMF", "BRInterface.CMF",
                                "DeBriefing.CMF", "DeBriefingHeads.CMF", "Funeral.CMF"}) {
            const auto path = find_child_case_insensitive(*cmfs, name);
            if (!path || !has_readable_file(*cmfs, name))
                return {false, std::string("SWC needs CMFs/") + name + ".", true};
            std::ifstream file(*path, std::ios::binary);
            std::array<char, 156> header = {};
            file.read(header.data(), header.size());
            // Some disc extractions wrap every file in MacBinary; its data
            // fork follows the 128-byte header (the game checks the rest).
            const auto read = file.gcount();
            const bool plain = read >= 28 && std::string_view(header.data(), 4) == "CMF1";
            const bool macBinary = read == static_cast<std::streamsize>(header.size()) &&
                header[0] == 0 && std::string_view(header.data() + 128, 4) == "CMF1";
            if (!plain && !macBinary)
                return {false, std::string("Invalid SWC archive: ") + name + ".", true};
        }
        // The game's own search: raw forks, the named fork, AppleDouble or a
        // MacBinary application such as discmaster serves.
        if (!SdlHasSwcResources(std::string(directoryText).c_str()))
            return {false, "SWC resources missing: keep the demo application with its resource fork (a MacBinary copy also works) or add SuperWingCommanderDemo.rsrc.", true};
        return {true, "Mac demo", true};
    }
    if (!gameData || !fs::is_directory(*gameData, error))
        return {false, "Choose a WC1 directory containing GAMEDAT or the SWC demo directory containing CMFs."};

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
        return {true, "DOS"};
    const auto streams = find_child_case_insensitive(directory, "STREAMS");
    if (!streams || !fs::is_directory(*streams, error))
        return {false, "Kilrathi Saga data needs a sibling STREAMS directory."};
    for (const auto name : {"PREFLITE.STR", "POSFLITE.STR", "MISSION.STR"}) {
        if (!has_readable_file(*streams, name))
            return {false, std::string("STREAMS/") + name + " is missing or unreadable."};
    }
    return {true, "Kilrathi Saga"};
}

int add_game_installation(std::vector<GameInstallation> &installations,
                          std::string_view directory)
{
    const auto status = validate_game_directory(directory);
    if (!status.valid)
        return -1;
    std::error_code error;
    const auto canonical = fs::canonical(path_from_utf8(directory), error);
    if (error)
        return -1;
    const auto text = path_to_utf8(canonical);
    if (text.size() >= SDL_LAUNCHER_DIRECTORY_CAPACITY)
        return -1;
    for (size_t index = 0; index < installations.size(); ++index) {
        if (fs::equivalent(canonical, path_from_utf8(installations[index].directory), error))
            return static_cast<int>(index);
    }
    installations.push_back({text, status.swcDemo ? "SWC Mac demo" : status.message});
    return static_cast<int>(installations.size() - 1);
}

static std::string normalized_installation_name(const fs::path &directory)
{
    std::string result;
    // Ignore spaces, punctuation and trademark symbols used by store titles.
    for (unsigned char character : path_to_utf8(directory.filename())) {
        if (character >= 'A' && character <= 'Z')
            character += 'a' - 'A';
        if ((character >= 'a' && character <= 'z') ||
            (character >= '0' && character <= '9'))
            result += character;
    }
    return result;
}

static bool is_wc1_installation_name(const fs::path &directory)
{
    auto name = normalized_installation_name(directory);
    for (const std::string_view prefix : {"wingcommander", "wc1"}) {
        if (!name.starts_with(prefix))
            continue;
        const std::string_view suffix = std::string_view(name).substr(prefix.size());
        // WC2 has similar data filenames: never search the other games simply
        // because their directory names also begin with Wing Commander.
        for (const auto expected : {"", "1", "i", "1and2", "12", "iandii",
                                   "35thanniversaryedition", "i35thanniversaryedition",
                                   "135thanniversaryedition", "dos", "dosversion",
                                   "legacy", "1990", "thekilrathisaga", "kilrathisaga"}) {
            if (suffix == expected)
                return true;
        }
    }
    return name == "wing" || name == "wc" || name == "wcks" ||
        name == "kilrathisaga" || name == "thekilrathisaga";
}

static void scan_game_directory(std::vector<GameInstallation> &installations,
                                const fs::path &directory, int depth = 0)
{
    if (directory.empty())
        return;
    add_game_installation(installations, path_to_utf8(directory));
    if (depth == 3)
        return;
    // Only descend through known package layouts, never through arbitrary
    // user folders, Wine prefixes, or another game's data.
    for (const auto name : {"Kilrathi_Saga", "Kilrathi Saga", "DOS", "WC", "WC1", "WING"}) {
        const auto child = find_child_case_insensitive(directory, name);
        if (child)
            scan_game_directory(installations, *child, depth + 1);
    }
}

static void scan_game_library(std::vector<GameInstallation> &installations,
                              const fs::path &library)
{
    std::error_code error;
    fs::directory_iterator iterator(library, error);
    const fs::directory_iterator end;
    std::vector<fs::path> directories;
    while (!error && iterator != end) {
        if (is_wc1_installation_name(iterator->path()))
            directories.push_back(iterator->path());
        iterator.increment(error);
    }
    std::sort(directories.begin(), directories.end());
    for (const auto &directory : directories)
        scan_game_directory(installations, directory);
}

static fs::path environment_directory(const char *name)
{
#ifdef _WIN32
    // Windows environment strings are UTF-16, not the active ANSI codepage.
    const std::wstring wideName(name, name + std::char_traits<char>::length(name));
    const wchar_t *value = _wgetenv(wideName.c_str());
    return value != nullptr ? fs::path(value) : fs::path();
#else
    const char *value = std::getenv(name);
    return value != nullptr ? fs::path(value) : fs::path();
#endif
}

std::vector<GameInstallation> discover_game_installations()
{
    std::vector<fs::path> dataDirectories;
    std::vector<fs::path> libraries;
    std::error_code error;
    const auto workingDirectory = fs::current_path(error);
    if (!error)
        dataDirectories.push_back(workingDirectory);
    // Desktop shortcuts/Finder need not use the executable's directory as cwd.
    if (char *base = SDL_GetBasePath()) {
        dataDirectories.push_back(path_from_utf8(base));
        SDL_free(base);
    }

#ifdef _WIN32
    const auto home = environment_directory("USERPROFILE");
#else
    auto home = environment_directory("HOME");
    if (home.empty()) {
        if (const auto *user = getpwuid(getuid()))
            home = user->pw_dir;
    }
#endif
    if (!home.empty()) {
        libraries.push_back(home / "Games" / "Heroic");
        libraries.push_back(home / "GOG Games");
    }

#ifdef _WIN32
    auto systemDrive = environment_directory("SystemDrive");
    if (systemDrive.empty())
        systemDrive = L"C:";
    const auto driveRoot = systemDrive / L"\\";
    libraries.push_back(driveRoot); // WING, WC, WC1, WCKS
    libraries.push_back(driveRoot / "GOG Games");
    libraries.push_back(driveRoot / "Games");
    libraries.push_back(driveRoot / "Origin");
    libraries.push_back(driveRoot / "Origin Games");
    libraries.push_back(driveRoot / "EA Games");
    for (const auto &programFiles : {
             environment_directory("ProgramFiles"),
             environment_directory("ProgramFiles(x86)"),
             environment_directory("ProgramW6432"),
             driveRoot / "Program Files", driveRoot / "Program Files (x86)"}) {
        if (programFiles.empty())
            continue;
        libraries.push_back(programFiles);
        for (const auto relative : {"GOG.com", "GOG Games", "GOG Galaxy/Games",
                                    "Steam/steamapps/common", "EA Games", "Origin Games",
                                    "Origin Systems"})
            libraries.push_back(programFiles / relative);
    }
#elif defined(__APPLE__)
    if (!home.empty())
        libraries.push_back(home / "Library/Application Support/Steam/steamapps/common");
#else
    if (!home.empty()) {
        libraries.push_back(home / ".steam/steam/steamapps/common");
        libraries.push_back(home / ".local/share/Steam/steamapps/common");
        libraries.push_back(home / ".var/app/com.valvesoftware.Steam/data/Steam/steamapps/common");
    }
#endif
    std::vector<GameInstallation> installations;
    for (const auto &directory : dataDirectories)
        scan_game_directory(installations, directory);
    for (const auto &library : libraries)
        scan_game_library(installations, library);
    return installations;
}

} // namespace wc1_launcher
