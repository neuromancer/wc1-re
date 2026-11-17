#ifndef WC1_SLINT_LAUNCHER_JOYSTICK_H
#define WC1_SLINT_LAUNCHER_JOYSTICK_H

#include "launcher_axes.h"
#include "../pad_bindings.h"

#include <SDL.h>

#include <array>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

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

// A joystick as the Joystick tab shows it: a mapped game controller's
// controls, or another joystick's buttons, hats and axes, numbered as
// src/sdl/pad_bindings.h numbers them.
struct PadSnapshot {
    bool connected = false;
    // SDL maps it as a game controller.
    bool mapped = false;
    // It calls itself a joystick, or SDL knows it as a flight stick or a
    // throttle.
    bool joystick = false;
    std::string name;
    int buttons = 0;
    int hats = 0;
    int axes = 0;
    // What each axis is; empty when the platform does not say.
    std::vector<AxisKind> axisKinds;
    // Each control held now; a mapped controller's sticks count as held
    // past half their travel.
    std::array<bool, SDL_PAD_CONTROLS> held {};
    // Another joystick's axes, -1 to 1.
    std::array<float, SDL_PAD_STICK_AXES> axisValues {};
};

// The first two SDL joysticks, numbered and read as the game does in
// src/sdl/joystick.c, for the calibration window and the Joystick tab.
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
    PadSnapshot snapshot(int slot) const;

private:
    struct Device {
        SDL_GameController *controller = nullptr;
        SDL_Joystick *joystick = nullptr;
        SDL_JoystickID instance = -1;
        JoystickDescription description;
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
