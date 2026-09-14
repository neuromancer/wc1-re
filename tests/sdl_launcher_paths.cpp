#include "../src/sdl/slint/launcher_paths.h"
#include "../src/sdl/slint/launcher_api.h"

#include <array>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using namespace wc1_launcher;

static void write_module(const fs::path &path, unsigned char compression)
{
    std::array<unsigned char, 8> header = {8, 0, 0, 0, 8, 0, 0, compression};
    std::ofstream file(path, std::ios::binary | std::ios::trunc);

    file.write(reinterpret_cast<const char *>(header.data()), header.size());
    assert(file.good());
}

int main(int argumentCount, char **arguments)
{
    const fs::path root = fs::temp_directory_path() /
        ("wc1-launcher-" + std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()));
    const fs::path installation = root / path_from_utf8("Sirius — 遊戲");
    const fs::path gameData = installation / "GameDat";
    const fs::path streams = installation / "Streams";
    const std::string directory = path_to_utf8(installation);

    assert(path_from_utf8(directory) == installation);
    assert(!validate_game_directory("").valid);
    assert(!validate_game_directory(directory).valid);
    assert(!validate_game_directory(
        std::string(SDL_LAUNCHER_DIRECTORY_CAPACITY, 'x')).valid);
    fs::create_directories(gameData);
    // A WC2 installation has COCKPIT.VGA but no WC1 MODULE.000.
    std::ofstream(gameData / "COCKPIT.VGA").put('x');
    assert(!validate_game_directory(directory).valid);
    std::ofstream(gameData / "Module.000").put('x');
    assert(!validate_game_directory(directory).valid);
    write_module(gameData / "Module.000", 1);
    assert(!validate_game_directory(directory).valid);
    std::ofstream(gameData / "Camp.000").put('x');
    std::ofstream(gameData / "Install.dat").put('x');
    const auto dos = validate_game_directory(directory);
    assert(dos.valid && dos.message.find("DOS") != std::string::npos);
    assert(!validate_game_directory(path_to_utf8(gameData)).valid);

    // The shipped Kilrathi Saga MODULE.000 uses raw packet flag 0xe0.
    write_module(gameData / "Module.000", 0xe0);
    assert(!validate_game_directory(directory).valid);
    fs::create_directory(streams);
    for (const auto name : {"Preflite.str", "Posflite.str", "Mission.str"}) {
        assert(!validate_game_directory(directory).valid);
        std::ofstream(streams / name).put('x');
    }
    const auto saga = validate_game_directory(directory);
    assert(saga.valid && saga.message.find("Kilrathi Saga") != std::string::npos);
    fs::remove(streams / "Mission.str");
    fs::create_directory(streams / "Mission.str");
    assert(!validate_game_directory(directory).valid);
    fs::remove(streams / "Mission.str");
    std::ofstream(streams / "Mission.str");
    assert(!validate_game_directory(directory).valid);
    fs::remove_all(root);

    for (int argument = 1; argument < argumentCount; argument++) {
        const auto status = validate_game_directory(arguments[argument]);
        std::cout << arguments[argument] << ": " << status.message << '\n';
        if (!status.valid)
            return 1;
    }
    std::cout << "Launcher directory checks passed.\n";
    return 0;
}
