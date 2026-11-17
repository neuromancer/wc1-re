#include "launcher_axes.h"

#include <SDL.h>

#include <algorithm>

#if defined(__linux__)

#include <fcntl.h>
#include <linux/input.h>
#include <linux/joystick.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <array>
#include <cstring>

namespace wc1_launcher {
namespace {

constexpr unsigned int bitsPerLong = sizeof(unsigned long) * 8;

constexpr unsigned int longs_for(unsigned int bits)
{
    return (bits + bitsPerLong - 1) / bitsPerLong;
}

bool test_bit(unsigned int bit, const unsigned long *bits)
{
    return ((bits[bit / bitsPerLong] >> (bit % bitsPerLong)) & 1UL) != 0;
}

// The kernel's absolute axis codes; hid-input gives sliders ABS_THROTTLE
// and dials ABS_RUDDER.
AxisKind evdev_axis_kind(unsigned int code)
{
    switch (code) {
    case ABS_X: return AxisKind::X;
    case ABS_Y: return AxisKind::Y;
    case ABS_Z: return AxisKind::Z;
    case ABS_RX: return AxisKind::RotationX;
    case ABS_RY: return AxisKind::RotationY;
    case ABS_RZ: return AxisKind::RotationZ;
    case ABS_THROTTLE: return AxisKind::Throttle;
    case ABS_RUDDER: return AxisKind::Rudder;
    case ABS_WHEEL: return AxisKind::Wheel;
    case ABS_GAS: return AxisKind::Accelerator;
    case ABS_BRAKE: return AxisKind::Brake;
    default: return AxisKind::Unknown;
    }
}

// SDL2's GuessIfAxesAreDigitalHat (src/joystick/linux/SDL_sysjoystick.c),
// without SDL_HINT_LINUX_DIGITAL_HATS.
bool digital_hat(const input_absinfo *x, const input_absinfo *y)
{
    if (x == nullptr && y == nullptr)
        return false;
    if ((x == nullptr || (x->minimum == -1 && x->maximum == 1)) &&
        (y == nullptr || (y->minimum == -1 && y->maximum == 1)))
        return true;
    return (x == nullptr || (!x->fuzz && !x->flat && !x->resolution)) &&
           (y == nullptr || (!y->fuzz && !y->flat && !y->resolution));
}

// As SDL2's ConfigJoystick numbers an evdev device's axes.
JoystickDescription describe_evdev(int fd)
{
    JoystickDescription description;
    unsigned long keys[longs_for(KEY_MAX + 1)] = {};
    unsigned long axes[longs_for(ABS_MAX + 1)] = {};
    bool digital[4] = {};

    if (ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keys)), keys) < 0 ||
        ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(axes)), axes) < 0)
        return description;
    // hid-input numbers a joystick's buttons from BTN_TRIGGER and a
    // gamepad's from BTN_SOUTH.
    description.joystick = test_bit(BTN_JOYSTICK, keys);
    description.gamepad = test_bit(BTN_GAMEPAD, keys);
    for (unsigned int code = ABS_HAT0X; code <= ABS_HAT3Y; code += 2) {
        input_absinfo x {};
        input_absinfo y {};
        const bool hasX = test_bit(code, axes) && ioctl(fd, EVIOCGABS(code), &x) >= 0;
        const bool hasY = test_bit(code + 1, axes) && ioctl(fd, EVIOCGABS(code + 1), &y) >= 0;
        digital[(code - ABS_HAT0X) / 2] = digital_hat(hasX ? &x : nullptr, hasY ? &y : nullptr);
    }
    for (unsigned int code = 0; code < ABS_MAX; ++code) {
        input_absinfo info {};
        if (code >= ABS_HAT0X && code <= ABS_HAT3Y && digital[(code - ABS_HAT0X) / 2])
            continue;
        if (test_bit(code, axes) && ioctl(fd, EVIOCGABS(code), &info) >= 0)
            description.axes.push_back(evdev_axis_kind(code));
    }
    return description;
}

// As SDL2 numbers a classic /dev/input/js device's axes, its hats aside.
JoystickDescription describe_classic(int fd)
{
    JoystickDescription description;
    unsigned char axisCount = 0;
    unsigned char buttonCount = 0;
    std::array<unsigned char, ABS_CNT> axisMap {};
    std::array<unsigned short, KEY_MAX - BTN_MISC + 1> buttonMap {};

    if (ioctl(fd, JSIOCGAXES, &axisCount) < 0 || ioctl(fd, JSIOCGAXMAP, axisMap.data()) < 0)
        return description;
    if (ioctl(fd, JSIOCGBUTTONS, &buttonCount) >= 0 && ioctl(fd, JSIOCGBTNMAP, buttonMap.data()) >= 0) {
        for (unsigned int button = 0; button < buttonCount && button < buttonMap.size(); ++button) {
            description.joystick = description.joystick || buttonMap[button] == BTN_JOYSTICK;
            description.gamepad = description.gamepad || buttonMap[button] == BTN_GAMEPAD;
        }
    }
    for (unsigned int axis = 0; axis < axisCount && axis < axisMap.size(); ++axis) {
        if (axisMap[axis] < ABS_HAT0X || axisMap[axis] > ABS_HAT3Y)
            description.axes.push_back(evdev_axis_kind(axisMap[axis]));
    }
    return description;
}

} // namespace

JoystickDescription describe_joystick(int deviceIndex, int axisCount)
{
    const char *path = SDL_JoystickPathForIndex(deviceIndex);
    JoystickDescription description;

    if (path == nullptr)
        return description;
    const int fd = open(path, O_RDONLY | O_NONBLOCK | O_CLOEXEC);
    if (fd < 0)
        return description;
    description = std::strncmp(path, "/dev/input/js", 13) == 0 ? describe_classic(fd) : describe_evdev(fd);
    close(fd);
    if (static_cast<int>(description.axes.size()) != axisCount)
        description.axes.clear();
    return description;
}

} // namespace wc1_launcher

#elif defined(_WIN32)

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
/* Define DirectInput's GUIDs here rather than linking dxguid, whose single
   object also defines the DirectDraw GUIDs that the game's dib.c does. */
#include <initguid.h>
#include <dinput.h>

namespace wc1_launcher {
namespace {

struct FoundDevice {
    WORD vendor = 0;
    WORD product = 0;
    bool found = false;
    GUID instance {};
    DWORD type = 0;
};

BOOL CALLBACK find_device(LPCDIDEVICEINSTANCEW instance, LPVOID context)
{
    auto *device = static_cast<FoundDevice *>(context);

    if (LOWORD(instance->guidProduct.Data1) != device->vendor ||
        HIWORD(instance->guidProduct.Data1) != device->product)
        return DIENUM_CONTINUE;
    device->found = true;
    device->instance = instance->guidInstance;
    device->type = instance->dwDevType;
    return DIENUM_STOP;
}

// An axis's HID usage, when DirectInput gives one, says more than its class.
AxisKind usage_kind(WORD page, WORD usage, AxisKind fallback)
{
    if (page == 0x02) {
        switch (usage) {
        case 0xBA: return AxisKind::Rudder;
        case 0xBB: return AxisKind::Throttle;
        case 0xC4: return AxisKind::Accelerator;
        case 0xC5: return AxisKind::Brake;
        default: break;
        }
    } else if (page == 0x01) {
        switch (usage) {
        case 0x36: return AxisKind::Slider;
        case 0x37: return AxisKind::Dial;
        case 0x38: return AxisKind::Wheel;
        default: break;
        }
    }
    return fallback;
}

struct DirectInputAxis {
    int order;
    AxisKind kind;
};

// SDL2's EnumDevObjectsCallback and SortDevObjects
// (src/joystick/windows/SDL_dinputjoystick.c) number the axes it can read in
// DIJOYSTATE2's order: X, Y, Z, their rotations, then the sliders as found.
BOOL CALLBACK collect_axis(LPCDIDEVICEOBJECTINSTANCEW object, LPVOID context)
{
    static const struct {
        const GUID *type;
        AxisKind kind;
    } classes[] = {
        {&GUID_XAxis, AxisKind::X},          {&GUID_YAxis, AxisKind::Y},
        {&GUID_ZAxis, AxisKind::Z},          {&GUID_RxAxis, AxisKind::RotationX},
        {&GUID_RyAxis, AxisKind::RotationY}, {&GUID_RzAxis, AxisKind::RotationZ},
        {&GUID_Slider, AxisKind::Slider},
    };
    auto *axes = static_cast<std::vector<DirectInputAxis> *>(context);

    for (int order = 0; order < static_cast<int>(sizeof(classes) / sizeof(classes[0])); ++order) {
        if (IsEqualGUID(object->guidType, *classes[order].type)) {
            axes->push_back({order, usage_kind(object->wUsagePage, object->wUsage, classes[order].kind)});
            break;
        }
    }
    return DIENUM_CONTINUE;
}

} // namespace

JoystickDescription describe_joystick(int deviceIndex, int axisCount)
{
    JoystickDescription description;
    FoundDevice device;
    IDirectInput8W *input = nullptr;

    device.vendor = SDL_JoystickGetDeviceVendor(deviceIndex);
    device.product = SDL_JoystickGetDeviceProduct(deviceIndex);
    // A virtual joystick or one that gives no IDs would match others.
    if (device.vendor == 0 && device.product == 0)
        return description;
    if (FAILED(DirectInput8Create(GetModuleHandleW(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8W,
                                  reinterpret_cast<void **>(&input), nullptr)))
        return description;
    input->EnumDevices(DI8DEVCLASS_GAMECTRL, find_device, &device, DIEDFL_ATTACHEDONLY);
    if (device.found) {
        const BYTE type = GET_DIDEVICE_TYPE(device.type);
        IDirectInputDevice8W *joystick = nullptr;

        description.joystick = type == DI8DEVTYPE_JOYSTICK || type == DI8DEVTYPE_FLIGHT;
        description.gamepad = type == DI8DEVTYPE_GAMEPAD;
        if (SUCCEEDED(input->CreateDevice(device.instance, &joystick, nullptr))) {
            std::vector<DirectInputAxis> axes;

            joystick->EnumObjects(collect_axis, &axes, DIDFT_AXIS);
            joystick->Release();
            std::stable_sort(axes.begin(), axes.end(),
                             [](const DirectInputAxis &a, const DirectInputAxis &b) { return a.order < b.order; });
            for (const auto &axis : axes)
                description.axes.push_back(axis.kind);
        }
    }
    input->Release();
    if (static_cast<int>(description.axes.size()) != axisCount)
        description.axes.clear();
    return description;
}

} // namespace wc1_launcher

#elif defined(__APPLE__)

#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDUsageTables.h>

namespace wc1_launcher {
namespace {

struct MacAxis {
    uint32_t usage;
    IOHIDElementCookie cookie;
    AxisKind kind;
};

AxisKind hid_axis_kind(uint32_t page, uint32_t usage)
{
    if (page == kHIDPage_GenericDesktop) {
        switch (usage) {
        case kHIDUsage_GD_X: return AxisKind::X;
        case kHIDUsage_GD_Y: return AxisKind::Y;
        case kHIDUsage_GD_Z: return AxisKind::Z;
        case kHIDUsage_GD_Rx: return AxisKind::RotationX;
        case kHIDUsage_GD_Ry: return AxisKind::RotationY;
        case kHIDUsage_GD_Rz: return AxisKind::RotationZ;
        case kHIDUsage_GD_Slider: return AxisKind::Slider;
        case kHIDUsage_GD_Dial: return AxisKind::Dial;
        case kHIDUsage_GD_Wheel: return AxisKind::Wheel;
        default: break;
        }
    } else if (page == kHIDPage_Simulation) {
        switch (usage) {
        case kHIDUsage_Sim_Rudder: return AxisKind::Rudder;
        case kHIDUsage_Sim_Throttle: return AxisKind::Throttle;
        case kHIDUsage_Sim_Accelerator: return AxisKind::Accelerator;
        case kHIDUsage_Sim_Brake: return AxisKind::Brake;
        default: break;
        }
    }
    return AxisKind::Unknown;
}

void add_elements(CFArrayRef elements, std::vector<MacAxis> &axes);

// As SDL2's AddHIDElement (src/joystick/darwin/SDL_iokitjoystick.c): each
// axis once, after those with the same or a lower usage, and the elements
// of collections as they are met.
void add_element(IOHIDElementRef element, std::vector<MacAxis> &axes)
{
    const IOHIDElementType type = IOHIDElementGetType(element);

    if (type == kIOHIDElementTypeCollection) {
        if (CFArrayRef children = IOHIDElementGetChildren(element))
            add_elements(children, axes);
        return;
    }
    if (type != kIOHIDElementTypeInput_Misc && type != kIOHIDElementTypeInput_Button &&
        type != kIOHIDElementTypeInput_Axis)
        return;
    const uint32_t usage = IOHIDElementGetUsage(element);
    const AxisKind kind = hid_axis_kind(IOHIDElementGetUsagePage(element), usage);
    const IOHIDElementCookie cookie = IOHIDElementGetCookie(element);
    if (kind == AxisKind::Unknown ||
        std::any_of(axes.begin(), axes.end(), [cookie](const MacAxis &axis) { return axis.cookie == cookie; }))
        return;
    const auto after = std::find_if(axes.begin(), axes.end(),
                                    [usage](const MacAxis &axis) { return usage < axis.usage; });
    axes.insert(after, {usage, cookie, kind});
}

void add_elements(CFArrayRef elements, std::vector<MacAxis> &axes)
{
    for (CFIndex index = 0; index < CFArrayGetCount(elements); ++index) {
        const void *value = CFArrayGetValueAtIndex(elements, index);
        if (value != nullptr && CFGetTypeID(value) == IOHIDElementGetTypeID())
            add_element(static_cast<IOHIDElementRef>(const_cast<void *>(value)), axes);
    }
}

int device_number(IOHIDDeviceRef device, CFStringRef key)
{
    const CFTypeRef value = IOHIDDeviceGetProperty(device, key);
    int number = 0;

    if (value != nullptr && CFGetTypeID(value) == CFNumberGetTypeID())
        CFNumberGetValue(static_cast<CFNumberRef>(value), kCFNumberIntType, &number);
    return number;
}

} // namespace

JoystickDescription describe_joystick(int deviceIndex, int axisCount)
{
    JoystickDescription description;
    const int vendor = SDL_JoystickGetDeviceVendor(deviceIndex);
    const int product = SDL_JoystickGetDeviceProduct(deviceIndex);

    // A virtual joystick or one that gives no IDs would match others.
    if (vendor == 0 && product == 0)
        return description;
    IOHIDManagerRef manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    if (manager == nullptr)
        return description;
    CFNumberRef vendorNumber = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &vendor);
    CFNumberRef productNumber = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &product);
    const void *keys[] = {CFSTR(kIOHIDVendorIDKey), CFSTR(kIOHIDProductIDKey)};
    const void *values[] = {vendorNumber, productNumber};
    CFDictionaryRef matching = CFDictionaryCreate(kCFAllocatorDefault, keys, values, 2,
                                                  &kCFTypeDictionaryKeyCallBacks,
                                                  &kCFTypeDictionaryValueCallBacks);
    IOHIDManagerSetDeviceMatching(manager, matching);
    if (CFSetRef devices = IOHIDManagerCopyDevices(manager)) {
        std::vector<const void *> found(static_cast<size_t>(CFSetGetCount(devices)));
        CFSetGetValues(devices, found.data());
        for (const void *value : found) {
            IOHIDDeviceRef device = static_cast<IOHIDDeviceRef>(const_cast<void *>(value));
            std::vector<MacAxis> axes;
            if (CFArrayRef elements = IOHIDDeviceCopyMatchingElements(device, nullptr, kIOHIDOptionsTypeNone)) {
                add_elements(elements, axes);
                CFRelease(elements);
            }
            // Of identical devices, any one with SDL's axes.
            if (static_cast<int>(axes.size()) != axisCount)
                continue;
            const int page = device_number(device, CFSTR(kIOHIDPrimaryUsagePageKey));
            const int usage = device_number(device, CFSTR(kIOHIDPrimaryUsageKey));
            description.joystick = page == kHIDPage_GenericDesktop &&
                (usage == kHIDUsage_GD_Joystick || usage == kHIDUsage_GD_MultiAxisController);
            description.gamepad = page == kHIDPage_GenericDesktop && usage == kHIDUsage_GD_GamePad;
            for (const auto &axis : axes)
                description.axes.push_back(axis.kind);
            break;
        }
        CFRelease(devices);
    }
    CFRelease(matching);
    CFRelease(productNumber);
    CFRelease(vendorNumber);
    CFRelease(manager);
    return description;
}

} // namespace wc1_launcher

#else

namespace wc1_launcher {

JoystickDescription describe_joystick(int, int)
{
    return {};
}

} // namespace wc1_launcher

#endif
