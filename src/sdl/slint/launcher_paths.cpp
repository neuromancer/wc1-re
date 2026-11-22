#include "launcher.h"
#include "launcher_paths.h"
#include "launcher_api.h"

#include <SDL_filesystem.h>

#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <optional>
#include <system_error>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
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

std::optional<fs::path> find_child_case_insensitive(
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
        return {false, LauncherMessage::ChooseDirectory};
    if (directoryText.size() >= SDL_LAUNCHER_DIRECTORY_CAPACITY)
        return {false, LauncherMessage::PathTooLong};

    const fs::path directory = path_from_utf8(directoryText);
    std::error_code error;

    if (!fs::is_directory(directory, error))
        return {false, LauncherMessage::NotDirectory};
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
                return {false, LauncherMessage::MissingSwcFile, true, name};
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
                return {false, LauncherMessage::InvalidSwcArchive, true, name};
        }
        // The game's own search: raw forks, the named fork, AppleDouble or a
        // MacBinary application such as discmaster serves.
        if (!SdlHasSwcResources(std::string(directoryText).c_str()))
            return {false, LauncherMessage::MissingSwcResources, true};
        DirectoryStatus status {true, has_readable_file(*cmfs, "Simulator.CMF") ?
            LauncherMessage::MacRetail : LauncherMessage::MacDemo, true};
        // The original opened these from the Apple menu's About item.
        status.swcCredits = has_readable_file(*cmfs, "credits.swc");
        return status;
    }
    if (!gameData || !fs::is_directory(*gameData, error))
        return {false, LauncherMessage::ChooseDataDirectory};

    const auto module = find_child_case_insensitive(*gameData, "MODULE.000");
    if (!module || !fs::is_regular_file(*module, error))
        return {false, LauncherMessage::MissingModule};
    std::ifstream moduleFile(*module, std::ios::binary);
    std::array<unsigned char, 8> header = {};

    moduleFile.read(reinterpret_cast<char *>(header.data()), header.size());
    if (moduleFile.gcount() != static_cast<std::streamsize>(header.size()))
        return {false, LauncherMessage::UnreadableModule};
    if (header[7] == 2 && has_readable_file(*gameData, "COMMUNIC.JAT")) {
        for (const auto name : {"CAMP.000", "BRIEFING.000", "FONTS.FNT", "TITLES.VGA"}) {
            if (!has_readable_file(*gameData, name))
                return {false, LauncherMessage::MissingGameFile, false, name};
        }
        return {true, LauncherMessage::FmTowns};
    }
    for (const auto name : {"CAMP.000", "INSTALL.DAT"}) {
        if (!has_readable_file(*gameData, name))
            return {false, LauncherMessage::MissingGameFile, false, name};
    }

    // Match SdlUsingDosData: DOS archives are LZW-compressed (flag 1 in byte 7).
    // Fan translations such as the German one rewrite MODULE.000 raw.
    const auto lzwArchive = [&](const char *name) {
        const auto path = find_child_case_insensitive(*gameData, name);
        std::array<unsigned char, 8> bytes = {};
        if (!path)
            return false;
        std::ifstream file(*path, std::ios::binary);
        file.read(reinterpret_cast<char *>(bytes.data()), bytes.size());
        return file.gcount() == static_cast<std::streamsize>(bytes.size()) && bytes[7] == 1;
    };
    DirectoryStatus status {true, header[7] == 1 || lzwArchive("CAMP.000") ?
                                      LauncherMessage::Dos : LauncherMessage::KilrathiSaga};
    if (status.message == LauncherMessage::Dos) {
        const auto executable = find_child_case_insensitive(directory, "WC.EXE");
        status.germanText = executable && SdlCountGermanText(path_to_utf8(*executable).c_str()) > 0;
    } else {
        const auto streams = find_child_case_insensitive(directory, "STREAMS");
        if (!streams || !fs::is_directory(*streams, error))
            return {false, LauncherMessage::NeedsStreams};
        for (const auto name : {"PREFLITE.STR", "POSFLITE.STR", "MISSION.STR"}) {
            if (!has_readable_file(*streams, name))
                return {false, LauncherMessage::MissingStreamFile, false, name};
        }
    }
    // Both releases ship the same expansion files. DOS installs take the
    // Kilrathi Saga install records from SdlCompleteDosInstallTable.
    status.sm1 = true;
    for (const auto name : {"CAMP.001", "MODULE.001", "BRIEFING.001", "MIDGAME.V04", "MIDGAME.V05"})
        status.sm1 &= has_readable_file(*gameData, name);
    status.sm2 = true;
    for (const auto name : {"CAMP.002", "MODULE.002", "BRIEFING.002", "MIDGAME.V06", "MIDGAME.V07",
                            "MIDGAME.V08", "TALKING1.VGA", "WINGMEN1.VGA", "PCSHIP.V05", "COMMSM2.DAT", "TITLE1.VGA"})
        status.sm2 &= has_readable_file(*gameData, name);
    // SM1.5 brings four files of its own and plays on SM2's others.
    status.sm15 = true;
    for (const auto name : {"CAMP.015", "MODULE.015", "BRIEFING.015", "TITL15.VGA", "MIDGAME.V06", "MIDGAME.V07",
                            "MIDGAME.V08", "TALKING1.VGA", "WINGMEN1.VGA", "PCSHIP.V05", "COMMSM2.DAT"})
        status.sm15 &= has_readable_file(*gameData, name);
    return status;
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
    char voiceDirectory[SDL_LAUNCHER_DIRECTORY_CAPACITY];
    const bool voices = !status.swcDemo && SdlFindVoiceDirectory(text.c_str(), voiceDirectory, sizeof(voiceDirectory));
    for (size_t index = 0; index < installations.size(); ++index) {
        if (fs::equivalent(canonical, path_from_utf8(installations[index].directory), error)) {
            installations[index].segaCdVoices = voices;
            return static_cast<int>(index);
        }
    }
    installations.push_back({text, status.message, voices});
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
    return name == "wing" || name == "wc" || name == "wcks" || name == "wingloader" ||
        name == "kilrathisaga" || name == "thekilrathisaga";
}

// The directory's installation and those in its known subfolders, such as
// the 35th Anniversary Edition's Kilrathi_Saga and DOS; returns the first one
// found, or -1.
static int scan_game_directory(std::vector<GameInstallation> &installations,
                               const fs::path &directory, int depth = 0)
{
    if (directory.empty())
        return -1;
    int first = add_game_installation(installations, path_to_utf8(directory));
    if (depth == 3)
        return first;
    // Only descend through known package layouts, never through arbitrary
    // user folders, Wine prefixes, or another game's data.
    for (const auto name : {"Kilrathi_Saga", "Kilrathi Saga", "DOS", "WC", "WC1", "WING", "Games"}) {
        const auto child = find_child_case_insensitive(directory, name);
        if (!child)
            continue;
        const int found = scan_game_directory(installations, *child, depth + 1);
        if (first < 0)
            first = found;
    }
    return first;
}

// The installations in a library's WC1-named folders, such as Steam's
// steamapps/common/Wing Commander; returns the first one found, or -1.
static int scan_game_library(std::vector<GameInstallation> &installations,
                             const fs::path &library)
{
    std::error_code error;
    fs::directory_iterator iterator(library, error);
    const fs::directory_iterator end;
    std::vector<fs::path> directories;
    int first = -1;
    while (!error && iterator != end) {
        if (is_wc1_installation_name(iterator->path()))
            directories.push_back(iterator->path());
        iterator.increment(error);
    }
    std::sort(directories.begin(), directories.end());
    for (const auto &directory : directories) {
        const int found = scan_game_directory(installations, directory);
        if (first < 0)
            first = found;
    }
    return first;
}

int add_game_installations(std::vector<GameInstallation> &installations,
                           std::string_view directory)
{
    const fs::path path = path_from_utf8(directory);
    const int index = scan_game_directory(installations, path);
    return index >= 0 ? index : scan_game_library(installations, path);
}

// A Steam installation's default library, and every library its
// steamapps/libraryfolders.vdf names: as "path" values since 2021, as numbered
// values before. Libraries on other drives, such as a Steam Deck's microSD
// card, are listed only there.
static void add_steam_libraries(std::vector<fs::path> &libraries,
                                const fs::path &steamRoot)
{
    libraries.push_back(steamRoot / "steamapps" / "common");
    std::ifstream file(steamRoot / "steamapps" / "libraryfolders.vdf",
                       std::ios::binary);
    std::string key;
    bool haveKey = false;
    char character;

    while (file.get(character)) {
        if (character == '{' || character == '}') {
            haveKey = false;
            continue;
        }
        if (character != '"')
            continue;
        std::string text;
        // VDF escapes backslashes, as in Windows paths, and quotes.
        while (file.get(character) && character != '"') {
            if (character == '\\' && !file.get(character))
                break;
            text += character;
        }
        if (!haveKey) {
            key = std::move(text);
            haveKey = true;
            continue;
        }
        haveKey = false;
        // Numbered keys also count each app's size; only paths are kept.
        const bool numbered = !key.empty() &&
            std::all_of(key.begin(), key.end(), [](unsigned char digit) {
                return digit >= '0' && digit <= '9';
            });
        const auto library = path_from_utf8(text);
        if ((numbered || equal_ascii_case(key, "path")) && library.is_absolute())
            libraries.push_back(library / "steamapps" / "common");
    }
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

#ifdef _WIN32
// A folder the registry names, such as where Steam is installed.
static fs::path registry_directory(HKEY root, const wchar_t *key, const wchar_t *value)
{
    wchar_t text[4096];
    DWORD size = sizeof(text);

    if (RegGetValueW(root, key, value, RRF_RT_REG_SZ, nullptr, text, &size) != ERROR_SUCCESS)
        return {};
    return fs::path(text);
}
#endif

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
                                    "EA Games", "Origin Games", "Origin Systems"})
            libraries.push_back(programFiles / relative);
        add_steam_libraries(libraries, programFiles / "Steam");
    }
    // Steam can be installed on any drive; it records where in the registry.
    for (const auto &steamRoot : {
             registry_directory(HKEY_CURRENT_USER, L"Software\\Valve\\Steam", L"SteamPath"),
             registry_directory(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Valve\\Steam", L"InstallPath"),
             registry_directory(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam", L"InstallPath")}) {
        if (!steamRoot.empty())
            add_steam_libraries(libraries, steamRoot);
    }
#elif defined(__APPLE__)
    if (!home.empty())
        add_steam_libraries(libraries, home / "Library/Application Support/Steam");
#else
    if (!home.empty()) {
        // Native Steam, then the Steam Flatpak's, in the home it keeps in
        // ~/.var/app or in its Flatpak data folder.
        for (const auto relative : {".steam/steam", ".local/share/Steam",
                                    ".var/app/com.valvesoftware.Steam/.local/share/Steam",
                                    ".var/app/com.valvesoftware.Steam/data/Steam"})
            add_steam_libraries(libraries, home / relative);
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
