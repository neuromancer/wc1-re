#include "launcher_joystick.h"
#include "launcher_paths.h"

#include "../joystick_calibration.h"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <system_error>

namespace wc1_launcher {

namespace fs = std::filesystem;

JoystickReader::~JoystickReader()
{
    stop();
}

bool JoystickReader::start()
{
    if (active)
        return true;
    // The game also declares its own main; without this, SDL refuses to start.
    SDL_SetMainReady();
    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0)
        return false;
    active = true;
    update();
    return true;
}

void JoystickReader::stop()
{
    if (!active)
        return;
    for (auto &device : devices)
        close(device);
    SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK);
    active = false;
}

void JoystickReader::close(Device &device)
{
    if (device.controller != nullptr)
        SDL_GameControllerClose(device.controller);
    else if (device.joystick != nullptr)
        SDL_JoystickClose(device.joystick);
    device = {};
}

void JoystickReader::update()
{
    if (!active)
        return;
    SDL_JoystickUpdate();
    for (auto &device : devices) {
        if (device.joystick != nullptr && !SDL_JoystickGetAttached(device.joystick))
            close(device);
    }
    // As SdlRefreshJoysticks: free slots take devices in SDL's order.
    for (int index = 0; index < SDL_NumJoysticks(); ++index) {
        const auto slot = std::find_if(devices.begin(), devices.end(),
                                       [](const Device &device) { return device.joystick == nullptr; });
        if (slot == devices.end())
            break;
        const SDL_JoystickID instance = SDL_JoystickGetDeviceInstanceID(index);
        if (instance < 0 || std::any_of(devices.begin(), devices.end(), [instance](const Device &device) {
                return device.joystick != nullptr && device.instance == instance;
            }))
            continue;
        SDL_GameController *controller = SDL_IsGameController(index) ? SDL_GameControllerOpen(index) : nullptr;
        SDL_Joystick *joystick = controller != nullptr ? SDL_GameControllerGetJoystick(controller) : nullptr;
        if (joystick == nullptr) {
            if (controller != nullptr)
                SDL_GameControllerClose(controller);
            controller = nullptr;
            joystick = SDL_JoystickOpen(index);
        }
        if (joystick != nullptr)
            *slot = {controller, joystick, SDL_JoystickInstanceID(joystick)};
    }
    // Nothing else in the launcher reads SDL's events.
    SDL_FlushEvents(SDL_JOYAXISMOTION, SDL_FINGERDOWN - 1);
}

std::array<std::string, 2> JoystickReader::names() const
{
    std::array<std::string, 2> names;
    for (size_t slot = 0; slot < devices.size(); ++slot) {
        const auto &device = devices[slot];
        if (device.joystick == nullptr)
            continue;
        const char *name = device.controller != nullptr ? SDL_GameControllerName(device.controller)
                                                        : SDL_JoystickName(device.joystick);
        names[slot] = name != nullptr && *name != '\0' ? name : "?";
    }
    return names;
}

JoystickPosition JoystickReader::read(int slot) const
{
    JoystickPosition position;
    if (slot < 0 || static_cast<size_t>(slot) >= devices.size() || devices[slot].joystick == nullptr)
        return position;
    const auto &device = devices[slot];
    position.connected = true;
    // SdlReadJoystickAxis: a mapped controller's left stick, else axes 0 and 1.
    if (device.controller != nullptr) {
        position.x = SDL_GameControllerGetAxis(device.controller, SDL_CONTROLLER_AXIS_LEFTX) + 32768;
        position.y = SDL_GameControllerGetAxis(device.controller, SDL_CONTROLLER_AXIS_LEFTY) + 32768;
    } else {
        const int axes = SDL_JoystickNumAxes(device.joystick);
        position.x = (axes > 0 ? SDL_JoystickGetAxis(device.joystick, 0) : 0) + 32768;
        position.y = (axes > 1 ? SDL_JoystickGetAxis(device.joystick, 1) : 0) + 32768;
    }
    for (int button = 0; button < SDL_JoystickNumButtons(device.joystick); ++button)
        position.button = position.button || SDL_JoystickGetButton(device.joystick, button) != 0;
    return position;
}

fs::path joystick_calibration_path(const fs::path &gameDirectory)
{
    const auto gameData = find_child_case_insensitive(gameDirectory, "GAMEDAT");
    if (!gameData)
        return {};
    if (const auto existing = find_child_case_insensitive(*gameData, SDL_JOYSTICK_CALIBRATION_FILE))
        return *existing;
    return *gameData / SDL_JOYSTICK_CALIBRATION_FILE;
}

std::optional<JoystickCalibration> read_joystick_calibration(const fs::path &path)
{
    JoystickCalibration calibration;
    std::ifstream input(path, std::ios::binary);
    if (!input.read(reinterpret_cast<char *>(calibration.data()),
                    static_cast<std::streamsize>(sizeof(calibration))))
        return std::nullopt;
    return calibration;
}

bool write_joystick_calibration(const fs::path &path, const JoystickCalibration &calibration,
                                std::string &error)
{
    errno = 0;
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    if (output)
        output.write(reinterpret_cast<const char *>(calibration.data()),
                     static_cast<std::streamsize>(sizeof(calibration)));
    output.close();
    if (output)
        return true;
    error = std::generic_category().message(errno != 0 ? errno : EIO);
    return false;
}

bool remove_joystick_calibration(const fs::path &path, std::string &error)
{
    std::error_code code;
    fs::remove(path, code);
    if (!code)
        return true;
    error = code.message();
    return false;
}

} // namespace wc1_launcher
