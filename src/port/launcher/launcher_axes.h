#ifndef WC1_SLINT_LAUNCHER_AXES_H
#define WC1_SLINT_LAUNCHER_AXES_H

#include <vector>

namespace wc1_launcher {

// What a joystick's axis is, as its HID usage or the driver says. The order
// is the launcher's (controls.slint names them).
enum class AxisKind {
    Unknown,
    X,
    Y,
    Z,
    RotationX,
    RotationY,
    RotationZ,
    Slider,
    Dial,
    Wheel,
    Rudder,
    Throttle,
    Accelerator,
    Brake,
};

// What each joystick axis is, from the platform's input API, in the order
// SDL2's backend numbers them.
struct JoystickDescription {
    bool joystick = false;
    bool gamepad = false;
    // Empty unless the platform listed exactly as many axes as SDL opened.
    std::vector<AxisKind> axes;
};

// The SDL device at deviceIndex, opened with axisCount axes.
JoystickDescription describe_joystick(int deviceIndex, int axisCount);

} // namespace wc1_launcher

#endif
