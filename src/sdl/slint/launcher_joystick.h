#ifndef WC1_SLINT_LAUNCHER_JOYSTICK_H
#define WC1_SLINT_LAUNCHER_JOYSTICK_H

#include <SDL.h>

#include <array>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>

namespace wc1_launcher {

// j.cal's words, in src/sdl/joystick_calibration.h's order.
using JoystickCalibration = std::array<std::uint16_t, 7>;

struct JoystickPosition {
    bool connected = false;
    // The game's positions, 0 to 65535.
    int x = 0;
    int y = 0;
    bool button = false;
};

// The first two SDL joysticks, numbered and read as the game does in
// src/sdl/joystick.c, for the calibration window.
class JoystickReader {
public:
    JoystickReader() = default;
    JoystickReader(const JoystickReader &) = delete;
    JoystickReader &operator=(const JoystickReader &) = delete;
    ~JoystickReader();

    // Starts SDL's joystick support; on failure SDL_GetError() says why.
    bool start();
    void stop();
    // Reads the devices, opening new ones and closing those unplugged.
    void update();
    // Each slot's device name; empty for a free slot.
    std::array<std::string, 2> names() const;
    JoystickPosition read(int slot) const;

private:
    struct Device {
        SDL_GameController *controller = nullptr;
        SDL_Joystick *joystick = nullptr;
        SDL_JoystickID instance = -1;
    };

    static void close(Device &device);

    std::array<Device, 2> devices {};
    bool active = false;
};

// GAMEDAT/j.cal in an installation, matching an existing file's case;
// empty without GAMEDAT.
std::filesystem::path joystick_calibration_path(const std::filesystem::path &gameDirectory);
std::optional<JoystickCalibration> read_joystick_calibration(const std::filesystem::path &path);
bool write_joystick_calibration(const std::filesystem::path &path,
                                const JoystickCalibration &calibration, std::string &error);
bool remove_joystick_calibration(const std::filesystem::path &path, std::string &error);

} // namespace wc1_launcher

#endif
