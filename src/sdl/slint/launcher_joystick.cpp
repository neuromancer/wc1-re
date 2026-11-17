#include "launcher_joystick.h"
#include "launcher_paths.h"

#include "../joystick_calibration.h"

#include <algorithm>
#include <cerrno>
#include <cstdlib>
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
    // As SdlRefreshJoysticks: free slots take sticks and gamepads first, then
    // the other devices, each group in SDL's order.
    const int count = SDL_NumJoysticks();
    for (int preference = 0; preference < SDL_JOYSTICK_PREFERENCES; ++preference) {
        for (int index = 0; index < count; ++index) {
            if (SdlJoystickPreference(index) != preference)
                continue;
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
            if (joystick != nullptr) {
                *slot = {controller, joystick, SDL_JoystickInstanceID(joystick), {}};
                if (controller == nullptr)
                    slot->description = describe_joystick(index, SDL_JoystickNumAxes(joystick));
            }
        }
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

PadSnapshot JoystickReader::snapshot(int slot) const
{
    static const SDL_GameControllerButton buttons[SDL_PAD_BUTTONS] = {
        SDL_CONTROLLER_BUTTON_A, SDL_CONTROLLER_BUTTON_B,
        SDL_CONTROLLER_BUTTON_X, SDL_CONTROLLER_BUTTON_Y,
        SDL_CONTROLLER_BUTTON_LEFTSHOULDER, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
        SDL_CONTROLLER_BUTTON_INVALID, SDL_CONTROLLER_BUTTON_INVALID,
        SDL_CONTROLLER_BUTTON_BACK, SDL_CONTROLLER_BUTTON_START,
        SDL_CONTROLLER_BUTTON_LEFTSTICK, SDL_CONTROLLER_BUTTON_RIGHTSTICK,
        SDL_CONTROLLER_BUTTON_DPAD_UP, SDL_CONTROLLER_BUTTON_DPAD_DOWN,
        SDL_CONTROLLER_BUTTON_DPAD_LEFT, SDL_CONTROLLER_BUTTON_DPAD_RIGHT
    };
    static const SDL_GameControllerAxis sticks[4] = {
        SDL_CONTROLLER_AXIS_LEFTX, SDL_CONTROLLER_AXIS_LEFTY,
        SDL_CONTROLLER_AXIS_RIGHTX, SDL_CONTROLLER_AXIS_RIGHTY
    };
    static const Uint8 hatDirections[4] = {SDL_HAT_UP, SDL_HAT_DOWN, SDL_HAT_LEFT, SDL_HAT_RIGHT};
    PadSnapshot snapshot;

    if (slot < 0 || static_cast<size_t>(slot) >= devices.size() || devices[slot].joystick == nullptr)
        return snapshot;
    const auto &device = devices[slot];
    snapshot.connected = true;
    snapshot.mapped = device.controller != nullptr;
    snapshot.name = names()[slot];
    snapshot.buttons = SDL_JoystickNumButtons(device.joystick);
    snapshot.hats = SDL_JoystickNumHats(device.joystick);
    snapshot.axes = SDL_JoystickNumAxes(device.joystick);
    const SDL_JoystickType type = SDL_JoystickGetType(device.joystick);
    snapshot.joystick = device.description.joystick || type == SDL_JOYSTICK_TYPE_FLIGHT_STICK ||
                        type == SDL_JOYSTICK_TYPE_THROTTLE;
    snapshot.axisKinds = device.description.axes;
    if (snapshot.mapped) {
        // A trigger presses past half its travel, as the game latches it.
        for (int control = 0; control < SDL_PAD_BUTTONS; ++control) {
            if (control == SDL_PAD_LEFT_TRIGGER || control == SDL_PAD_RIGHT_TRIGGER)
                snapshot.held[control] = SDL_GameControllerGetAxis(device.controller,
                    control == SDL_PAD_LEFT_TRIGGER ? SDL_CONTROLLER_AXIS_TRIGGERLEFT
                                                    : SDL_CONTROLLER_AXIS_TRIGGERRIGHT) >= 16384;
            else
                snapshot.held[control] = SDL_GameControllerGetButton(device.controller, buttons[control]) != 0;
        }
        for (int axis = 0; axis < 4; ++axis)
            snapshot.held[SDL_PAD_LEFT_STICK_X + axis] =
                std::abs(static_cast<int>(SDL_GameControllerGetAxis(device.controller, sticks[axis]))) >= 16384;
        return snapshot;
    }
    for (int button = 0; button < std::min(snapshot.buttons, SDL_PAD_STICK_BUTTONS); ++button)
        snapshot.held[SDL_PAD_STICK_BUTTON + button] = SDL_JoystickGetButton(device.joystick, button) != 0;
    for (int hat = 0; hat < std::min(snapshot.hats, SDL_PAD_STICK_HATS); ++hat) {
        const Uint8 value = SDL_JoystickGetHat(device.joystick, hat);
        for (int direction = 0; direction < 4; ++direction)
            snapshot.held[SDL_PAD_STICK_HAT + hat * 4 + direction] = (value & hatDirections[direction]) != 0;
    }
    for (int axis = 0; axis < std::min(snapshot.axes, SDL_PAD_STICK_AXES); ++axis)
        snapshot.axisValues[axis] =
            std::clamp(SDL_JoystickGetAxis(device.joystick, axis) / 32767.0f, -1.0f, 1.0f);
    return snapshot;
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
