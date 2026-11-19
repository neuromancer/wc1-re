#include "launcher.h"
#include "launcher_api.h"
#include "launcher_config.h"
#include "launcher_joystick.h"
#include "launcher_paths.h"
#include "wc1_logo.h"
#include "../pad_bindings.h"

#include <SDL.h>
#include <nfd.h>
#include <slint.h>

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace {

namespace fs = std::filesystem;

using namespace wc1_launcher;

void refresh_installations(LauncherWindow &window,
                            const std::vector<GameInstallation> &installations)
{
    const int selected = window.get_installation_index();
    window.set_has_installations(!installations.empty());
    std::vector<slint::SharedString> choices;
    choices.emplace_back(window.get_installation_prompt());
    for (const auto &installation : installations)
        choices.emplace_back(std::string(window.invoke_edition_text(installation.edition))
                             + (installation.segaCdVoices ? " · " + std::string(window.get_voice_label()) : "")
                             + " — " + installation.directory);

    window.set_installations(std::make_shared<slint::VectorModel<slint::SharedString>>(choices));
    window.set_installation_index(selected);
}

void set_language(LauncherWindow &window,
                  const std::vector<GameInstallation> &installations, int language);

/* The music choice that shows the device: a DOS install lists only the
 * AdLib and the MT-32. */
void show_music_device(LauncherWindow &window)
{
    const int device = std::clamp(window.get_music_device(), 0, 2);

    window.set_music_index(window.get_dos_data() ? (device == SDL_LAUNCHER_MUSIC_MT32 ? 1 : 0)
                                                 : device);
}

void refresh_window(LauncherWindow &window,
                    std::vector<GameInstallation> &installations)
{
    const std::string directory(window.get_game_directory());
    const DirectoryStatus status = validate_game_directory(directory);
    window.set_installation_index(add_game_installation(installations, directory) + 1);
    refresh_installations(window, installations);
    window.set_picker_error(false);
    window.set_path_valid(status.valid);
    window.set_swc_demo(status.swcDemo);
    window.set_sm1_data(status.sm1);
    window.set_sm2_data(status.sm2);
    window.set_swc_credits(status.swcCredits);
    window.set_towns_data(status.valid && status.message == LauncherMessage::FmTowns);
    // FM Towns data has its own CD and FM audio, the SWC demo its own music.
    window.set_dos_data(status.valid && status.message == LauncherMessage::Dos);
    window.set_music_available(status.valid && !status.swcDemo &&
                               status.message != LauncherMessage::FmTowns);
    show_music_device(window);
    if (!window.get_towns_data() && window.get_language_index() == 2)
        window.set_language_index(0);
    // The German fan translation replaces the text Spanish and French translate.
    window.set_german_text(status.valid && status.germanText);
    if (window.get_german_text() && window.get_language_index() != 0)
        set_language(window, installations, 0);
    window.set_status(status.message);
    window.set_status_detail(slint::SharedString(status.detail));
}

void set_language(LauncherWindow &window,
                  const std::vector<GameInstallation> &installations, int language)
{
    if ((language == 2 && !window.get_towns_data()) || (language != 0 && window.get_german_text()))
        language = 0;
    if (slint::select_bundled_translation(language == 1 ? "es" : language == 3 ? "fr" : "en")) {
        window.set_language_index(std::clamp(language, 0, 3));
        refresh_installations(window, installations);
    }
}

void report_picker_error(LauncherWindow &window, LauncherMessage message,
                         const char *details)
{
    window.set_picker_error(true);
    window.set_status(message);
    window.set_status_detail(details ? details : "");
}

/* The folder the player picks, starting from the current one; empty when
 * the player cancels or the picker fails, which it reports. */
std::string pick_folder(LauncherWindow &window, const std::string &currentDirectory)
{
    std::string defaultDirectory;
    std::error_code error;

    if (!currentDirectory.empty()) {
        const fs::path candidate = path_from_utf8(currentDirectory);
        if (fs::is_directory(candidate, error)) {
            const fs::path absolute = fs::absolute(candidate, error);
            if (!error)
                defaultDirectory = path_to_utf8(absolute);
        }
    }
    nfdu8char_t *selectedDirectory = nullptr;
    const nfdresult_t result = NFD_PickFolderU8(
        &selectedDirectory,
        defaultDirectory.empty() ? nullptr : defaultDirectory.c_str());
    if (result == NFD_CANCEL)
        return {};
    if (result != NFD_OKAY || selectedDirectory == nullptr) {
        report_picker_error(window, LauncherMessage::PickerFailed, NFD_GetError());
        return {};
    }
    const std::string selection(selectedDirectory);
    NFD_FreePathU8(selectedDirectory);
    return selection;
}

/* What the ROM folder holds, for the MT-32 line under the music choice. */
void refresh_mt32_roms(LauncherWindow &window, const std::string &roms,
                       const std::string &model)
{
    char detail[SDL_LAUNCHER_DIRECTORY_CAPACITY + 256] = "";

    window.set_mt32_roms_set(!roms.empty());
    window.set_mt32_ready(!roms.empty() &&
                          SdlDescribeMt32Roms(roms.c_str(), model.c_str(), detail, sizeof(detail)));
    window.set_mt32_detail(slint::SharedString(detail));
}

void browse_for_game_directory(LauncherWindow &window,
                               std::vector<GameInstallation> &installations)
{
    const std::string selection = pick_folder(window, std::string(window.get_game_directory()));
    if (selection.empty())
        return;
    // A Steam or GOG copy keeps GAMEDAT in Kilrathi_Saga or DOS, below the
    // folder players usually pick; take the first installation inside it.
    const int index = add_game_installations(installations, selection);
    window.set_game_directory(slint::SharedString(
        index >= 0 ? installations[index].directory : selection));
    refresh_window(window, installations);
}

using KeyboardBindings = slint::VectorModel<std::shared_ptr<slint::Model<InputBinding>>>;

/* One game's rows in the keyboard and mouse tab and the bindings shown for
   them; its mouse rows are named with prefix ("" for WC1, "swc-" for SWC). */
struct BindingTable {
    std::shared_ptr<slint::Model<InputRow>> rows;
    std::shared_ptr<KeyboardBindings> bindings;
    std::string prefix;
};

BindingTable binding_table(const LauncherWindow &controls, int game)
{
    if (game == 1)
        return {controls.get_swc_rows(),
                std::dynamic_pointer_cast<KeyboardBindings>(controls.get_swc_bindings()), "swc-"};
    return {controls.get_keyboard_rows(),
            std::dynamic_pointer_cast<KeyboardBindings>(controls.get_keyboard_bindings()), ""};
}

/* A copy of a table's original bindings, row by row, that the keyboard
   and mouse tab can change. */
std::shared_ptr<KeyboardBindings> default_bindings(const slint::Model<InputRow> &rows)
{
    auto table = std::make_shared<KeyboardBindings>();
    for (size_t row = 0; row < rows.row_count(); ++row) {
        std::vector<InputBinding> bindings;
        const auto data = rows.row_data(row);
        if (data && data->bindings) {
            for (size_t slot = 0; slot < data->bindings->row_count(); ++slot)
                bindings.push_back(data->bindings->row_data(slot).value_or(InputBinding {}));
        }
        table->push_back(std::make_shared<slint::VectorModel<InputBinding>>(std::move(bindings)));
    }
    return table;
}

/* Input names are stored percent-encoded outside a-z, 0-9 and "-", so "+"
   joins a modifier to its key and "," separates bindings (see
   src/sdl/keymap.c). */
std::string encode_input(std::string_view input)
{
    static constexpr char digits[] = "0123456789abcdef";
    std::string text;
    for (const unsigned char character : input) {
        if ((character >= 'a' && character <= 'z') || (character >= '0' && character <= '9') ||
            character == '-') {
            text += static_cast<char>(character);
        } else {
            text += '%';
            text += digits[character >> 4];
            text += digits[character & 15];
        }
    }
    return text;
}

std::optional<std::string> decode_input(std::string_view text)
{
    std::string input;
    for (size_t index = 0; index < text.size(); ++index) {
        if (text[index] != '%') {
            input += text[index];
            continue;
        }
        unsigned int byte = 0;
        if (index + 2 >= text.size() ||
            std::from_chars(text.data() + index + 1, text.data() + index + 3, byte, 16).ptr !=
                text.data() + index + 3)
            return std::nullopt;
        input += static_cast<char>(byte);
        index += 2;
    }
    if (input.empty())
        return std::nullopt;
    return input;
}

std::string encode_binding(const InputBinding &binding)
{
    std::string text;
    for (size_t part = 0; binding.inputs && part < binding.inputs->row_count(); ++part) {
        if (part > 0)
            text += '+';
        text += encode_input(std::string_view(binding.inputs->row_data(part).value_or(
            slint::SharedString())));
    }
    return text;
}

std::string encode_bindings(const slint::Model<InputBinding> &bindings)
{
    std::string text;
    for (size_t slot = 0; slot < bindings.row_count(); ++slot) {
        if (slot > 0)
            text += ',';
        text += encode_binding(bindings.row_data(slot).value_or(InputBinding {}));
    }
    return text;
}

/* The rows whose bindings differ from the original keys, as the launcher
   settings' [keys] lines: WC1's, then SWC's. Both show the shared full-screen
   shortcut; WC1's line stands for it. */
std::string changed_key_bindings(const LauncherWindow &controls)
{
    std::string text;
    for (const int game : {0, 1}) {
        const auto table = binding_table(controls, game);
        for (size_t row = 0; table.bindings && row < table.rows->row_count() &&
                             row < table.bindings->row_count(); ++row) {
            const auto data = table.rows->row_data(row);
            const auto current = table.bindings->row_data(row).value_or(nullptr);
            if (!data || data->id.empty() || data->fixed || !data->bindings || !current ||
                (game == 1 && data->id == "full-screen"))
                continue;
            const auto value = encode_bindings(*current);
            if (value != encode_bindings(*data->bindings))
                text += std::string(std::string_view(data->id)) + '=' + value + '\n';
        }
    }
    return text;
}

/* How many bindings a row takes: its own limit, or the tab's. */
size_t binding_limit(const LauncherWindow &controls, const InputRow &row)
{
    return static_cast<size_t>(row.limit > 0 ? row.limit : controls.get_max_bindings());
}

/* The index of the row with this id in a table, or -1. */
int table_row(const BindingTable &table, std::string_view id)
{
    for (size_t row = 0; row < table.rows->row_count(); ++row) {
        const auto data = table.rows->row_data(row);
        if (data && std::string_view(data->id) == id)
            return static_cast<int>(row);
    }
    return -1;
}

/* One row of a table's current bindings, if it exists. */
std::shared_ptr<slint::VectorModel<InputBinding>> table_bindings(const BindingTable &table, int row)
{
    if (!table.bindings || row < 0 || static_cast<size_t>(row) >= table.bindings->row_count())
        return nullptr;
    return std::dynamic_pointer_cast<slint::VectorModel<InputBinding>>(
        table.bindings->row_data(row).value_or(nullptr));
}

/* The missile and roll rows show the game's two mouse buttons: whatever the
   guns and afterburner rows hold now. */
void refresh_mouse_rows(const BindingTable &table)
{
    const auto button = [&table](std::string_view id) -> std::optional<slint::SharedString> {
        const auto bindings = table_bindings(table, table_row(table, table.prefix + std::string(id)));
        if (!bindings || bindings->row_count() == 0)
            return std::nullopt;
        const auto binding = bindings->row_data(0).value_or(InputBinding {});
        if (!binding.inputs || binding.inputs->row_count() == 0)
            return std::nullopt;
        return binding.inputs->row_data(0);
    };
    const auto gesture = [](std::vector<slint::SharedString> inputs) {
        InputBinding binding;
        binding.inputs = std::make_shared<slint::VectorModel<slint::SharedString>>(std::move(inputs));
        binding.joiner = "+";
        binding.fixed = true;
        return std::vector<InputBinding> {binding};
    };
    const auto guns = button("mouse-guns");
    const auto afterburner = button("mouse-afterburner");
    const auto missile = table_row(table, table.prefix + "mouse-missile");
    const auto roll = table_row(table, table.prefix + "mouse-roll");
    if (!table.bindings || missile < 0 || roll < 0)
        return;
    table.bindings->set_row_data(missile, std::make_shared<slint::VectorModel<InputBinding>>(
        guns && afterburner ? gesture({*guns, *afterburner}) : std::vector<InputBinding> {}));
    table.bindings->set_row_data(roll, std::make_shared<slint::VectorModel<InputBinding>>(
        afterburner ? gesture({*afterburner, "mouse-move"}) : std::vector<InputBinding> {}));
}

/* Puts saved [keys] lines into the window's current bindings. A saved
   binding that matches a fixed original one keeps it, with its hint. */
void apply_table_bindings(const LauncherWindow &controls, const BindingTable &table,
                          std::string_view saved)
{
    const auto &rows = table.rows;
    while (table.bindings && !saved.empty()) {
        const auto end = saved.find('\n');
        const auto line = saved.substr(0, end);
        saved = end == std::string_view::npos ? std::string_view() : saved.substr(end + 1);
        const auto separator = line.find('=');
        if (separator == std::string_view::npos)
            continue;
        const auto id = line.substr(0, separator);
        auto value = line.substr(separator + 1);
        for (size_t row = 0; row < rows->row_count() && row < table.bindings->row_count(); ++row) {
            const auto data = rows->row_data(row);
            if (!data || data->fixed || std::string_view(data->id) != id)
                continue;
            const auto limit = binding_limit(controls, *data);
            const bool unbound = value.empty();
            std::vector<InputBinding> bindings;
            while (!value.empty() && bindings.size() < limit) {
                const auto comma = value.find(',');
                auto text = value.substr(0, comma);
                value = comma == std::string_view::npos ? std::string_view() : value.substr(comma + 1);
                std::vector<slint::SharedString> inputs;
                while (!text.empty()) {
                    const auto plus = text.find('+');
                    if (const auto input = decode_input(text.substr(0, plus)))
                        inputs.emplace_back(*input);
                    text = plus == std::string_view::npos ? std::string_view() : text.substr(plus + 1);
                }
                const auto mouse = [](const slint::SharedString &input) {
                    const std::string_view name(input);
                    return name.starts_with("mouse-") || name.starts_with("wheel-");
                };
                if (inputs.empty() || inputs.size() > 2 ||
                    (data->mouse_only && (inputs.size() > 1 || !std::string_view(inputs[0]).starts_with("mouse-"))) ||
                    (inputs.size() > 1 && (mouse(inputs[0]) || mouse(inputs[1]))))
                    continue;
                InputBinding binding;
                binding.inputs = std::make_shared<slint::VectorModel<slint::SharedString>>(inputs);
                binding.joiner = inputs.size() > 1 ? "+" : "";
                for (size_t slot = 0; data->bindings && slot < data->bindings->row_count(); ++slot) {
                    const auto original = data->bindings->row_data(slot).value_or(InputBinding {});
                    if (original.fixed && encode_binding(original) == encode_binding(binding))
                        binding = original;
                }
                bindings.push_back(binding);
            }
            /* An empty value leaves the input unbound; nothing valid in a
               value leaves the row as it was. */
            if (unbound || !bindings.empty())
                table.bindings->set_row_data(row, std::make_shared<slint::VectorModel<InputBinding>>(std::move(bindings)));
            break;
        }
    }
    refresh_mouse_rows(table);
}

void apply_key_bindings(const LauncherWindow &controls, std::string_view saved)
{
    for (const int game : {0, 1})
        apply_table_bindings(controls, binding_table(controls, game), saved);
}

/* The keyboard and mouse tab's bindings start from the saved ones, and
   changed_key_bindings() reads them back for the game. */
void prepare_controls(const slint::ComponentHandle<LauncherWindow> &window,
                      std::string_view savedBindings = {})
{
#ifdef __APPLE__
    window->set_apple_keys(true);
#endif
    window->set_keyboard_bindings(default_bindings(*window->get_keyboard_rows()));
    window->set_swc_bindings(default_bindings(*window->get_swc_rows()));
    apply_key_bindings(*window, savedBindings);
    /* Both games' tables show the shared full-screen shortcut. */
    const auto sync_full_screen = [](const LauncherWindow &window, int game) {
        const auto from = binding_table(window, game);
        const auto to = binding_table(window, 1 - game);
        const auto bindings = table_bindings(from, table_row(from, "full-screen"));
        const auto row = table_row(to, "full-screen");
        if (!bindings || row < 0)
            return;
        std::vector<InputBinding> copy;
        for (size_t slot = 0; slot < bindings->row_count(); ++slot)
            copy.push_back(bindings->row_data(slot).value_or(InputBinding {}));
        to.bindings->set_row_data(row, std::make_shared<slint::VectorModel<InputBinding>>(std::move(copy)));
    };
    window->on_binding_changed(
        [self = slint::ComponentWeakHandle<LauncherWindow>(window), sync_full_screen](
            int row, int slot, InputBinding binding) {
            const auto launcher = self.lock();
            if (!launcher)
                return;
            const int game = (*launcher)->get_controls_game();
            const auto table = binding_table(**launcher, game);
            const auto bindings = table_bindings(table, row);
            const auto data = table.rows->row_data(row);
            if (!bindings || !data || slot < 0)
                return;
            /* The guns and afterburner buttons cannot share a button: giving
               one the other's swaps them. */
            const std::string id(std::string_view(data->id));
            const auto guns = table.prefix + "mouse-guns";
            const auto afterburner = table.prefix + "mouse-afterburner";
            if (id == guns || id == afterburner) {
                const auto other = table_bindings(table, table_row(table, id == guns ? afterburner : guns));
                if (other && other->row_count() > 0 &&
                    encode_binding(other->row_data(0).value_or(InputBinding {})) == encode_binding(binding)) {
                    if (bindings->row_count() > 0)
                        other->set_row_data(0, bindings->row_data(0).value_or(InputBinding {}));
                    else
                        other->erase(0);
                }
            }
            if (static_cast<size_t>(slot) < bindings->row_count())
                bindings->set_row_data(slot, binding);
            else if (bindings->row_count() < binding_limit(**launcher, *data))
                bindings->push_back(binding);
            refresh_mouse_rows(table);
            if (id == "full-screen")
                sync_full_screen(**launcher, game);
        });
    window->on_binding_removed(
        [self = slint::ComponentWeakHandle<LauncherWindow>(window), sync_full_screen](int row, int slot) {
            const auto launcher = self.lock();
            if (!launcher)
                return;
            const int game = (*launcher)->get_controls_game();
            const auto table = binding_table(**launcher, game);
            const auto bindings = table_bindings(table, row);
            const auto data = table.rows->row_data(row);
            if (bindings && slot >= 0 && static_cast<size_t>(slot) < bindings->row_count())
                bindings->erase(slot);
            refresh_mouse_rows(table);
            if (data && data->id == "full-screen")
                sync_full_screen(**launcher, game);
        });
    /* Restore defaults resets the shown game's table. */
    window->on_bindings_reset(
        [self = slint::ComponentWeakHandle<LauncherWindow>(window), sync_full_screen]() {
            const auto launcher = self.lock();
            if (!launcher)
                return;
            const int game = (*launcher)->get_controls_game();
            if (game == 1)
                (*launcher)->set_swc_bindings(default_bindings(*(*launcher)->get_swc_rows()));
            else
                (*launcher)->set_keyboard_bindings(default_bindings(*(*launcher)->get_keyboard_rows()));
            sync_full_screen(**launcher, game);
        });
}

/* The Custom joystick mode's bindings (src/sdl/pad_bindings.h) as the
   Joystick tab codes them (controls.slint): the index of a keyboard and mouse
   row for a flight key's action, -1 for none, -2 to cycle the views, -3 to
   roll and change speed, and -10 less twice an axis function's number, less
   one more when reversed. Nothing when the control cannot take the function. */
std::optional<int> pad_code(const LauncherWindow &window, int control, std::string_view function)
{
    if (function.empty())
        return -1;
    if (SdlPadControlTakesAxis(control)) {
        const bool reversed = function.front() == '-';
        const auto axis = reversed ? function.substr(1) : function;
        for (int index = 0; index < SDL_PAD_AXIS_FUNCTIONS; ++index) {
            if (axis == g_apszSdlPadAxisFunctions[index])
                return -10 - 2 * index - (reversed ? 1 : 0);
        }
    }
    if (!SdlPadControlTakesButton(control))
        return std::nullopt;
    if (function == "cycle-views")
        return -2;
    if (function == "roll-and-speed")
        return -3;
    const auto rows = window.get_keyboard_rows();
    for (size_t row = 0; row < rows->row_count(); ++row) {
        const auto data = rows->row_data(row);
        if (data && std::string_view(data->id) == function && !data->fixed && !data->mouse_only &&
            data->id != "full-screen")
            return static_cast<int>(row);
    }
    return std::nullopt;
}

std::string pad_function(const LauncherWindow &window, int control, int code)
{
    if (code == -2 && SdlPadControlTakesButton(control))
        return "cycle-views";
    if (code == -3 && SdlPadControlTakesButton(control))
        return "roll-and-speed";
    if (code <= -10 && code > -10 - 2 * SDL_PAD_AXIS_FUNCTIONS && SdlPadControlTakesAxis(control)) {
        const int offset = -10 - code;
        return std::string(offset % 2 != 0 ? "-" : "") + g_apszSdlPadAxisFunctions[offset / 2];
    }
    const auto rows = window.get_keyboard_rows();
    if (code >= 0 && SdlPadControlTakesButton(control) && static_cast<size_t>(code) < rows->row_count()) {
        if (const auto data = rows->row_data(code))
            return std::string(std::string_view(data->id));
    }
    return {};
}

/* The defaults for an axis layout, with the saved "control=function" lines
   over them. */
/* A raw joystick's axes by what they are: the stick's X turns as the axis
   layout's left stick does and its Y pitches, a twist or rudder takes the
   other turn, and the first throttle lever, or else Z, the throttle. Each is
   reversed where its pair reads it the other way: the first of a pair grows
   to the right, the second upwards (src/sdl/pad_bindings.h), and a lever is
   taken to be forward at SDL's negative end. */
std::vector<std::string> named_axis_functions(const std::vector<AxisKind> &kinds, int axes)
{
    const bool leftRolls = axes == 1 || axes == 2 || axes == 5;
    const auto lever = [](AxisKind kind) {
        return kind == AxisKind::Throttle || kind == AxisKind::Slider || kind == AxisKind::Dial ||
               kind == AxisKind::Accelerator;
    };
    const bool hasLever = std::any_of(kinds.begin(), kinds.end(), lever);
    std::vector<std::string> functions(kinds.size());
    bool stickX = false;
    bool stickY = false;
    bool twist = false;
    bool throttle = false;

    for (size_t axis = 0; axis < kinds.size(); ++axis) {
        const AxisKind kind = kinds[axis];
        std::string function;
        bool upwards = false;
        if (kind == AxisKind::X && !stickX) {
            function = leftRolls ? "roll" : "yaw";
            stickX = true;
        } else if (kind == AxisKind::Y && !stickY) {
            function = "pitch";
            upwards = true;
            stickY = true;
        } else if ((kind == AxisKind::RotationZ || kind == AxisKind::Rudder || kind == AxisKind::Wheel) && !twist) {
            function = leftRolls ? "yaw" : "roll";
            twist = true;
        } else if ((lever(kind) || (kind == AxisKind::Z && !hasLever)) && !throttle) {
            function = "throttle";
            upwards = true;
            throttle = true;
        }
        if (!function.empty() && upwards != ((axis & 1) != 0))
            function = "-" + function;
        functions[axis] = function;
    }
    return functions;
}

std::shared_ptr<slint::VectorModel<int>> pad_codes(const LauncherWindow &window, int axes,
                                                   std::string_view saved)
{
    std::vector<int> codes(SDL_PAD_CONTROLS);
    std::vector<std::string> named;
    /* A raw joystick on the tab whose axes the platform names starts with
       them by what they are. */
    const auto device = window.get_pad_device();
    const auto kinds = window.get_pad_axis_kinds();
    if (device.connected && !device.mapped && kinds && kinds->row_count() > 0) {
        std::vector<AxisKind> list;
        for (size_t axis = 0; axis < kinds->row_count(); ++axis)
            list.push_back(static_cast<AxisKind>(kinds->row_data(axis).value_or(0)));
        named = named_axis_functions(list, axes);
    }
    for (int control = 0; control < SDL_PAD_CONTROLS; ++control) {
        const size_t axis = static_cast<size_t>(control - SDL_PAD_STICK_AXIS);
        const std::string function = control >= SDL_PAD_STICK_AXIS && !named.empty()
            ? (axis < named.size() ? named[axis] : std::string())
            : SdlDefaultPadFunction(control, axes);
        codes[control] = pad_code(window, control, function).value_or(-1);
    }
    while (!saved.empty()) {
        const auto end = saved.find('\n');
        const auto line = saved.substr(0, end);
        saved = end == std::string_view::npos ? std::string_view() : saved.substr(end + 1);
        const auto separator = line.find('=');
        if (separator == std::string_view::npos)
            continue;
        for (int control = 0; control < SDL_PAD_CONTROLS; ++control) {
            if (line.substr(0, separator) != g_apszSdlPadControls[control])
                continue;
            if (const auto code = pad_code(window, control, line.substr(separator + 1)))
                codes[control] = *code;
            break;
        }
    }
    return std::make_shared<slint::VectorModel<int>>(std::move(codes));
}

/* The Joystick tab's Custom bindings start from the saved ones, or from when
   the mode is first chosen, from the defaults for the axis layout then; kept
   says whether the settings keep them. Restore defaults takes the axis layout
   chosen last. */
void prepare_pad(const slint::ComponentHandle<LauncherWindow> &window, std::string_view saved, bool &kept)
{
    window->set_pad_codes(pad_codes(*window, window->get_joystick_axes_index(), saved));
    window->on_custom_pad_requested([self = slint::ComponentWeakHandle<LauncherWindow>(window), &kept]() {
        const auto launcher = self.lock();
        if (!launcher || kept)
            return;
        (*launcher)->set_pad_codes(pad_codes(**launcher, (*launcher)->get_joystick_axes_index(), {}));
        kept = true;
    });
    window->on_pad_code_changed([self = slint::ComponentWeakHandle<LauncherWindow>(window)](int control, int code) {
        const auto launcher = self.lock();
        if (!launcher)
            return;
        const auto codes = std::dynamic_pointer_cast<slint::VectorModel<int>>((*launcher)->get_pad_codes());
        if (codes && control >= 0 && static_cast<size_t>(control) < codes->row_count())
            codes->set_row_data(control, code);
    });
    window->on_pad_reset([self = slint::ComponentWeakHandle<LauncherWindow>(window)]() {
        if (const auto launcher = self.lock())
            (*launcher)->set_pad_codes(pad_codes(**launcher, (*launcher)->get_joystick_axes_index(), {}));
    });
}

/* The Joystick tab's Custom bindings as the launcher settings' [pad] lines. */
std::string pad_bindings(const LauncherWindow &window)
{
    const auto codes = window.get_pad_codes();
    std::string text;
    for (int control = 0; control < SDL_PAD_CONTROLS && static_cast<size_t>(control) < codes->row_count();
         ++control) {
        text += g_apszSdlPadControls[control];
        text += '=';
        text += pad_function(window, control, codes->row_data(control).value_or(-1));
        text += '\n';
    }
    return text;
}

#ifdef __linux__
/* Gamescope, the compositor of Steam's Game Mode and other gaming sessions,
   shows one window at a time, scaled to fit its output. A launcher larger
   than the output was shrunk and left beyond the X pointer's reach, so the
   cursor missed the buttons (issue #30); there the launcher fills the output.
   Gamescope sets both variables for the programs it runs. */
bool running_under_gamescope()
{
    const char *desktop = std::getenv("XDG_CURRENT_DESKTOP");

    return std::getenv("GAMESCOPE_WAYLAND_DISPLAY") != nullptr ||
           (desktop != nullptr && std::string_view(desktop) == "gamescope");
}

/* Winit takes the scale from the panel's physical size, about 2.25 on the
   Steam Deck, where the pages would not fit its 1280x800 screen. The scale
   that fits the launcher's size (launcher.slint) to the output replaces it,
   unless SLINT_SCALE_FACTOR already chooses one. */
void fit_scale_to_output()
{
    constexpr double pageWidth = 800.0;
    constexpr double pageHeight = 660.0;
    SDL_DisplayMode mode;

    if (std::getenv("SLINT_SCALE_FACTOR") != nullptr || SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
        return;
    if (SDL_GetDesktopDisplayMode(0, &mode) == 0 && mode.w > 0 && mode.h > 0) {
        const double scale = std::clamp(std::min(mode.w / pageWidth, mode.h / pageHeight), 0.5, 4.0);
        char text[16];

        std::snprintf(text, sizeof(text), "%.2f", std::floor(scale * 20.0) / 20.0);
        setenv("SLINT_SCALE_FACTOR", text, 0);
    }
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
#endif

int dead_zone_percent(float value)
{
    return std::clamp(static_cast<int>(std::lround(value)), 0, SDL_JOYSTICK_DEAD_ZONE_MAXIMUM);
}

/* The calibration page: CalibrateJoystickInteractive's upper-left,
   lower-right and centre positions (0x4102b0), read from SDL while the page
   shows the stick, the calibration in use and what the game reads through the
   dead zone. A corner takes the furthest position reached before the button,
   so a round gate's edges count; the original took the position at the press. */
class CalibrationSession {
public:
    explicit CalibrationSession(const slint::ComponentHandle<LauncherWindow> &launcher)
        : window(launcher)
    {
        window->on_calibration_start([this]() { calibrate(); });
        window->on_calibration_record([this]() { record(); });
        window->on_calibration_cancel([this]() {
            window->set_calibration_step(CalibrationStep::Idle);
            window->set_calibration_message(CalibrationMessage::None);
            show_view();
        });
        window->on_calibration_reset([this]() {
            std::string error;
            if (remove_joystick_calibration(path, error)) {
                stored.reset();
                window->set_calibration_message(CalibrationMessage::Removed);
            } else {
                window->set_calibration_message(CalibrationMessage::CannotRemove);
                window->set_calibration_message_detail(slint::SharedString(error));
            }
            show_view();
        });
        window->on_calibration_device_selected([this](int) {
            window->set_calibration_message(CalibrationMessage::None);
            buttonHeld = true;
            show_view();
        });
    }

    void show(const fs::path &gameDirectory)
    {
        path = joystick_calibration_path(gameDirectory);
        stored = path.empty() ? std::nullopt : read_joystick_calibration(path);
        window->set_calibration_step(CalibrationStep::Idle);
        window->set_calibration_message(CalibrationMessage::None);
        if (stored && (*stored)[0] < 2)
            window->set_calibration_device((*stored)[0]);
        if (!joysticks.start()) {
            window->set_calibration_message(CalibrationMessage::Unavailable);
            window->set_calibration_message_detail(slint::SharedString(SDL_GetError()));
        }
        buttonHeld = true;
        tick();
        timer.start(slint::TimerMode::Repeated, std::chrono::milliseconds(16), [this]() { tick(); });
        window->set_page(2);
    }

    // Stops reading the joysticks and leaves the page for the settings.
    void hide()
    {
        timer.stop();
        window->set_calibration_step(CalibrationStep::Idle);
        joysticks.stop();
        if (window->get_page() == 2)
            window->set_page(0);
    }

    // One reading of the joystick; the page's timer calls it.
    void tick()
    {
        joysticks.update();
        const auto names = joysticks.names();
        if (names != shownNames) {
            shownNames = names;
            window->set_calibration_devices(std::make_shared<slint::VectorModel<slint::SharedString>>(
                std::vector<slint::SharedString> {slint::SharedString(names[0]), slint::SharedString(names[1])}));
        }
        position = joysticks.read(slot());
        window->set_calibration_connected(position.connected);
        if (!position.connected) {
            window->set_calibration_step(CalibrationStep::Idle);
            buttonHeld = true;
            show_view();
            return;
        }
        const auto step = window->get_calibration_step();
        if (step == CalibrationStep::UpperLeft) {
            minimumX = std::min(minimumX, position.x);
            minimumY = std::min(minimumY, position.y);
        } else if (step == CalibrationStep::LowerRight) {
            maximumX = std::max(maximumX, position.x);
            maximumY = std::max(maximumY, position.y);
        }
        // As WaitForJoystickButtonPress, after WaitForJoystickButtonRelease.
        const bool pressed = position.button && !buttonHeld;
        buttonHeld = position.button;
        if (pressed && step != CalibrationStep::Idle)
            record();
        else
            show_view();
    }

    void calibrate()
    {
        if (!position.connected)
            return;
        window->set_calibration_message(CalibrationMessage::None);
        minimumX = position.x;
        minimumY = position.y;
        buttonHeld = position.button;
        window->set_calibration_step(CalibrationStep::UpperLeft);
        show_view();
    }

    void record()
    {
        switch (window->get_calibration_step()) {
        case CalibrationStep::UpperLeft:
            minimumX = std::min(minimumX, position.x);
            minimumY = std::min(minimumY, position.y);
            maximumX = position.x;
            maximumY = position.y;
            window->set_calibration_step(CalibrationStep::LowerRight);
            break;
        case CalibrationStep::LowerRight:
            maximumX = std::max(maximumX, position.x);
            maximumY = std::max(maximumY, position.y);
            window->set_calibration_step(CalibrationStep::Centre);
            break;
        case CalibrationStep::Centre:
            window->set_calibration_step(CalibrationStep::Idle);
            save(position.x, position.y);
            break;
        default:
            break;
        }
        show_view();
    }

private:
    slint::ComponentHandle<LauncherWindow> window;
    JoystickReader joysticks;
    slint::Timer timer;
    fs::path path;
    std::optional<JoystickCalibration> stored;
    std::array<std::string, 2> shownNames;
    JoystickPosition position;
    int minimumX = 0;
    int minimumY = 0;
    int maximumX = 0;
    int maximumY = 0;
    bool buttonHeld = true;

    int slot() const
    {
        return std::clamp(window->get_calibration_device(), 0, 1);
    }

    // The original checked only that each maximum exceeds its minimum.
    void save(int centreX, int centreY)
    {
        if (!SdlJoystickAxisCalibrationValid(minimumX, centreX, maximumX) ||
            !SdlJoystickAxisCalibrationValid(minimumY, centreY, maximumY)) {
            window->set_calibration_message(CalibrationMessage::Failed);
            return;
        }
        const JoystickCalibration calibration = {
            static_cast<std::uint16_t>(slot()),
            static_cast<std::uint16_t>(minimumX), static_cast<std::uint16_t>(minimumY),
            static_cast<std::uint16_t>(maximumX), static_cast<std::uint16_t>(maximumY),
            static_cast<std::uint16_t>(centreX), static_cast<std::uint16_t>(centreY)
        };
        std::string error;
        if (path.empty() || !write_joystick_calibration(path, calibration, error)) {
            window->set_calibration_message(CalibrationMessage::CannotSave);
            window->set_calibration_message_detail(slint::SharedString(error));
            return;
        }
        stored = calibration;
        window->set_calibration_message(CalibrationMessage::Saved);
    }

    // SdlAcceptJoystickCalibration's test, for the joystick shown.
    bool calibration_in_use() const
    {
        return stored && (*stored)[0] == slot() &&
               SdlJoystickAxisCalibrationValid((*stored)[1], (*stored)[5], (*stored)[3]) &&
               SdlJoystickAxisCalibrationValid((*stored)[2], (*stored)[6], (*stored)[4]);
    }

    void show_view()
    {
        SdlJoystickAxis x;
        SdlJoystickAxis y;
        const bool calibrated = calibration_in_use();
        if (calibrated) {
            const auto &words = *stored;
            SdlCalibrateJoystickAxis(&x, words[1], words[5], words[3], SDL_JOYSTICK_STEPS);
            SdlCalibrateJoystickAxis(&y, words[2], words[6], words[4], SDL_JOYSTICK_STEPS);
        } else {
            SdlDefaultJoystickAxis(&x);
            SdlDefaultJoystickAxis(&y);
        }
        const int percent = dead_zone_percent(window->get_joystick_dead_zone());
        const int steps = SdlJoystickDeadZoneSteps(percent);
        const auto step = window->get_calibration_step();
        StickView view;
        view.x = position.x;
        view.y = position.y;
        // While recording, the outline follows the extremes reached.
        view.left = step != CalibrationStep::Idle ? minimumX : x.minimum;
        view.top = step != CalibrationStep::Idle ? minimumY : y.minimum;
        view.right = step == CalibrationStep::LowerRight || step == CalibrationStep::Centre ? maximumX : x.maximum;
        view.bottom = step == CalibrationStep::LowerRight || step == CalibrationStep::Centre ? maximumY : y.maximum;
        view.centre_x = x.centre;
        view.centre_y = y.centre;
        view.zone_left = x.centre - SdlJoystickDeadZoneEdge(x.centre - x.minimum, x.lowScale, percent);
        view.zone_right = x.centre + SdlJoystickDeadZoneEdge(x.maximum - x.centre, x.highScale, percent);
        view.zone_top = y.centre - SdlJoystickDeadZoneEdge(y.centre - y.minimum, y.lowScale, percent);
        view.zone_bottom = y.centre + SdlJoystickDeadZoneEdge(y.maximum - y.centre, y.highScale, percent);
        view.step_x = SdlJoystickAxisStep(&x, SdlShapeJoystickPosition(&x, position.x, percent), steps);
        view.step_y = SdlJoystickAxisStep(&y, SdlShapeJoystickPosition(&y, position.y, percent), steps);
        window->set_calibration_stick(view);
        window->set_calibration_stored(calibrated);
    }
};

/* The Joystick tab's live view. While the tab shows, the joysticks are read
   about thirty times a second, and the tab shows the one in pad-slot, or the
   other when only that is connected. A button, trigger or hat direction
   pressed since the last reading goes to the Custom mode's chooser. */
class PadSession {
public:
    explicit PadSession(const slint::ComponentHandle<LauncherWindow> &launcher)
        : window(launcher)
    {
        window->on_pad_slot_chosen([this](int) {
            if (shown)
                tick();
        });
    }

    void show()
    {
        if (shown)
            return;
        shown = true;
        joysticks.start();
        previous = {};
        tick();
        timer.start(slint::TimerMode::Repeated, std::chrono::milliseconds(33), [this]() { tick(); });
    }

    void hide()
    {
        if (!shown)
            return;
        shown = false;
        timer.stop();
        joysticks.stop();
        previous = {};
        shownNames = {};
        publish({});
        window->set_pad_devices(std::make_shared<slint::VectorModel<slint::SharedString>>(
            std::vector<slint::SharedString> {"", ""}));
    }

private:
    void tick()
    {
        joysticks.update();
        const auto names = joysticks.names();
        if (names != shownNames) {
            shownNames = names;
            window->set_pad_devices(std::make_shared<slint::VectorModel<slint::SharedString>>(
                std::vector<slint::SharedString> {slint::SharedString(names[0]), slint::SharedString(names[1])}));
        }
        int slot = std::clamp(window->get_pad_slot(), 0, 1);
        if (names[slot].empty() && !names[1 - slot].empty()) {
            slot = 1 - slot;
            window->set_pad_slot(slot);
        }
        const PadSnapshot next = joysticks.snapshot(slot);
        const bool sameDevice = next.connected && previous.connected && slot == previousSlot &&
                                next.mapped == previous.mapped && next.name == previous.name;
        publish(next);
        const PadSnapshot earlier = previous;
        previous = next;
        previousSlot = slot;
        if (!sameDevice)
            return;
        for (int control = 0; control < SDL_PAD_CONTROLS; ++control) {
            if (SdlPadControlTakesButton(control) && next.held[control] && !earlier.held[control]) {
                window->invoke_pad_pressed(control);
                break;
            }
        }
    }

    // Hands the tab what changed.
    void publish(const PadSnapshot &next)
    {
        PadDevice device;
        device.connected = next.connected;
        device.mapped = next.mapped;
        device.joystick = next.joystick;
        device.name = slint::SharedString(next.name);
        device.buttons = next.buttons;
        device.hats = next.hats;
        device.axes = next.axes;
        if (!(device == shownDevice)) {
            shownDevice = device;
            std::vector<int> kinds;
            for (const auto kind : next.axisKinds)
                kinds.push_back(static_cast<int>(kind));
            window->set_pad_axis_kinds(std::make_shared<slint::VectorModel<int>>(std::move(kinds)));
            window->set_pad_device(device);
        }
        if (next.held != shownHeld || !heldModel) {
            shownHeld = next.held;
            heldModel = std::make_shared<slint::VectorModel<bool>>(
                std::vector<bool>(shownHeld.begin(), shownHeld.end()));
            window->set_pad_held(heldModel);
        }
        bool moved = !valuesModel;
        for (size_t axis = 0; axis < next.axisValues.size(); ++axis)
            moved = moved || std::fabs(next.axisValues[axis] - shownValues[axis]) >= 0.01f;
        if (moved) {
            shownValues = next.axisValues;
            valuesModel = std::make_shared<slint::VectorModel<float>>(
                std::vector<float>(shownValues.begin(), shownValues.end()));
            window->set_pad_axis_values(valuesModel);
        }
    }

    slint::ComponentHandle<LauncherWindow> window;
    JoystickReader joysticks;
    slint::Timer timer;
    bool shown = false;
    PadSnapshot previous;
    int previousSlot = 0;
    std::array<std::string, 2> shownNames;
    PadDevice shownDevice;
    std::array<bool, SDL_PAD_CONTROLS> shownHeld {};
    std::array<float, SDL_PAD_STICK_AXES> shownValues {};
    std::shared_ptr<slint::VectorModel<bool>> heldModel;
    std::shared_ptr<slint::VectorModel<float>> valuesModel;
};

} // namespace

int SdlRunLauncherGui(SdlLauncherOptions *options)
{
    bool accepted;
    bool pickerInitialized;

    if (options == nullptr)
        return SDL_LAUNCHER_ERROR;
    /* First GUI launch defaults to autosave; saved choices and explicit
     * command-line options take precedence. */
    if (!(options->explicitOptions & SDL_LAUNCHER_EXPLICIT_AUTOSAVE))
        options->autosave = 1;
    const auto configPath = launcher_config_path();
    load_launcher_config(configPath, *options);
    auto installations = discover_game_installations();

    std::string initialDirectory(options->gameDirectory);
    if (!validate_game_directory(initialDirectory).valid && !installations.empty()) {
        initialDirectory = installations.front().directory;
    } else if (initialDirectory.empty()) {
        std::error_code error;
        const auto currentDirectory = fs::current_path(error);
        initialDirectory = error ? "." : path_to_utf8(currentDirectory);
    }

#ifdef __linux__
    const bool gamescope = running_under_gamescope();
    if (gamescope)
        fit_scale_to_output();
#endif
    auto window = LauncherWindow::create();
#ifdef __linux__
    if (gamescope)
        window->window().set_fullscreen(true);
#endif
    // The keyboard and mouse tab takes the saved bindings when it first opens.
    bool controlsReady = false;
    // The calibration page and the Joystick tab read joysticks only while
    // they show; the game opens its own.
    std::unique_ptr<CalibrationSession> calibration;
    const auto stop_calibration = [&calibration]() {
        if (calibration)
            calibration->hide();
    };
    PadSession pad(window);
    window->on_pad_live_changed([&pad](bool live) {
        if (live)
            pad.show();
        else
            pad.hide();
    });
    const auto stop_joysticks = [&pad, stop_calibration]() {
        stop_calibration();
        pad.hide();
    };
    accepted = false;
    pickerInitialized = false;
    window->set_title_artwork(slint::Image::load_from_data(
        wc1_launcher_artwork::kWingCommanderLogoPng, "png"));
    window->set_game_directory(slint::SharedString(initialDirectory));
    window->set_start_fullscreen(options->fullscreen != 0);
    window->set_autosave(options->autosave != 0);
    window->set_enhanced_renderer(options->enhancedRenderer != 0);
    window->set_ega_dither(options->egaDither != 0);
    window->set_hires_hud_fonts(options->hiResHudFonts != 0);
    window->set_frame_interpolation(options->frameInterpolation != 0);
    window->set_cockpitless_view(options->cockpitlessView != 0);
    window->set_cheat_mode(options->cheatMode != 0);
    window->set_joystick_rumble(options->joystickRumble != 0);
    window->set_joystick_mode_index(options->joystickMode);
    window->set_joystick_axes_index(options->joystickAxes);
    window->set_joystick_dead_zone(static_cast<float>(options->joystickDeadZone));
    bool padBindingsKept = options->padBindings[0] != '\0' ||
        options->joystickMode == SDL_LAUNCHER_JOYSTICK_CUSTOM;
    prepare_pad(window, options->padBindings, padBindingsKept);
    refresh_window(*window, installations);
    set_language(*window, installations, options->language);
    window->set_music_device(std::clamp(options->musicDevice, 0, 2));
    show_music_device(*window);
    std::string mt32Roms(options->mt32Roms);
    const std::string mt32Model(options->mt32Model);
    refresh_mt32_roms(*window, mt32Roms, mt32Model);
    window->on_music_selected([&window](int index) {
        int device = std::clamp(index, 0, 2);

        /* A DOS install lists only the AdLib and the MT-32; its AdLib
         * keeps whichever AdLib choice applies to Kilrathi Saga installs. */
        if (window->get_dos_data())
            device = index == 1 ? SDL_LAUNCHER_MUSIC_MT32
                : window->get_music_device() == SDL_LAUNCHER_MUSIC_MT32
                ? SDL_LAUNCHER_MUSIC_DEFAULT : window->get_music_device();
        window->set_music_device(device);
        show_music_device(*window);
    });

    window->on_language_selected([&window, &installations](int language) {
        set_language(*window, installations, language);
    });
    window->on_installation_selected([&window, &installations, stop_calibration](int index) {
        if (index > 0 && static_cast<size_t>(index) <= installations.size())
            window->set_game_directory(slint::SharedString(installations[index - 1].directory));
        refresh_window(*window, installations);
        // Its j.cal belongs to the installation it was opened for.
        stop_calibration();
    });
    window->on_controls_requested([&window, &controlsReady, options]() {
        if (controlsReady)
            return;
        prepare_controls(window, options->keyBindings);
        controlsReady = true;
    });
    window->on_calibration_requested([&window, &calibration]() {
        if (!calibration)
            calibration = std::make_unique<CalibrationSession>(window);
        calibration->show(path_from_utf8(std::string(window->get_game_directory())));
    });
    window->on_calibration_closed(stop_calibration);
    window->window().on_close_requested([stop_joysticks]() {
        stop_joysticks();
        return slint::CloseRequestResponse::HideWindow;
    });
    const auto launch = [&window, &accepted, &installations, &controlsReady, options, stop_joysticks](
                            int campaign, bool transfer, bool credits) {
        refresh_window(*window, installations);
        if (!window->get_path_valid() || campaign < 0 || campaign > 2 || (transfer && !campaign) ||
            (campaign == 1 && !window->get_sm1_data()) ||
            (campaign == 2 && !window->get_sm2_data()) ||
            (credits && !window->get_swc_credits()))
            return;
        std::error_code error;
        const std::string directory(window->get_game_directory());
        const auto absoluteDirectory = fs::absolute(path_from_utf8(directory), error);
        if (error) {
            report_picker_error(*window, LauncherMessage::CannotResolveDirectory,
                                error.message().c_str());
            return;
        }
        const auto absoluteText = path_to_utf8(absoluteDirectory);
        if (absoluteText.size() >= SDL_LAUNCHER_DIRECTORY_CAPACITY) {
            report_picker_error(*window, LauncherMessage::PathTooLong, nullptr);
            return;
        }
        fs::current_path(absoluteDirectory, error);
        if (error) {
            report_picker_error(*window, LauncherMessage::CannotOpenDirectory,
                                error.message().c_str());
            return;
        }
        window->set_game_directory(slint::SharedString(absoluteText));
        options->campaign = campaign;
        options->transferPilot = transfer;
        options->swcCredits = credits;
        if (controlsReady) {
            const auto keyBindings = changed_key_bindings(*window);
            if (keyBindings.size() < sizeof(options->keyBindings))
                std::memcpy(options->keyBindings, keyBindings.c_str(), keyBindings.size() + 1);
            else
                std::fprintf(stderr, "Too many changed keys to keep.\n");
        }
        accepted = true;
        /* Hide while the event loop still runs: on Linux the request takes
           effect only when the loop processes it, so hiding after the loop
           left the launcher on screen during the game. */
        stop_joysticks();
        window->hide();
        slint::quit_event_loop();
    };
    window->on_launch_requested([launch](int campaign, bool transfer) {
        launch(campaign, transfer, false);
    });
    window->on_credits_requested([launch]() {
        launch(0, false, true);
    });
    window->on_cancel_requested([stop_joysticks]() {
        stop_joysticks();
        slint::quit_event_loop();
    });

    window->show();
    /* NFD_Init records Cocoa's activation policy. Initialize it from the
     * callbacks, after winit launches NSApplication, so NFD_Quit cannot restore
     * the temporary pre-launch Prohibited policy before SDL takes over. */
    const auto start_picker = [&window, &pickerInitialized]() {
        if (pickerInitialized)
            return true;
        if (NFD_Init() != NFD_OKAY) {
            report_picker_error(*window, LauncherMessage::PickerInitFailed, NFD_GetError());
            return false;
        }
        pickerInitialized = true;
        return true;
    };
    window->on_mt32_browse_requested([&window, &mt32Roms, &mt32Model, start_picker]() {
        if (!start_picker())
            return;
        const std::string selection = pick_folder(*window, mt32Roms);
        if (selection.empty() || selection.size() >= SDL_LAUNCHER_DIRECTORY_CAPACITY)
            return;
        mt32Roms = selection;
        refresh_mt32_roms(*window, mt32Roms, mt32Model);
        // Choosing ROMs asks for the MT-32.
        window->set_music_device(SDL_LAUNCHER_MUSIC_MT32);
        show_music_device(*window);
    });
    window->on_browse_requested([&window, &installations, stop_calibration, start_picker]() {
        if (!start_picker())
            return;
        browse_for_game_directory(*window, installations);
        stop_calibration();
    });
    slint::run_event_loop();
    if (pickerInitialized)
        NFD_Quit();
    stop_joysticks();
    window->hide();
    if (!accepted)
        return SDL_LAUNCHER_CANCELLED;

    const std::string gameDirectory(window->get_game_directory());
    if (gameDirectory.size() >= sizeof(options->gameDirectory))
        return SDL_LAUNCHER_ERROR;
    std::memcpy(options->gameDirectory, gameDirectory.c_str(),
                gameDirectory.size() + 1);
    options->fullscreen = window->get_start_fullscreen();
    options->autosave = window->get_autosave();
    options->language = std::clamp(window->get_language_index(), 0, 3);
    options->enhancedRenderer = window->get_enhanced_renderer();
    options->egaDither = window->get_ega_dither();
    options->hiResHudFonts = window->get_hires_hud_fonts();
    options->frameInterpolation = window->get_frame_interpolation();
    options->cockpitlessView = window->get_cockpitless_view();
    options->cheatMode = window->get_cheat_mode();
    options->joystickRumble = window->get_joystick_rumble();
    options->joystickMode = std::max(0, window->get_joystick_mode_index());
    options->joystickAxes = std::max(0, window->get_joystick_axes_index());
    if (padBindingsKept) {
        const auto padBindings = pad_bindings(*window);
        if (padBindings.size() < sizeof(options->padBindings))
            std::memcpy(options->padBindings, padBindings.c_str(), padBindings.size() + 1);
    }
    options->joystickDeadZone = dead_zone_percent(window->get_joystick_dead_zone());
    options->musicDevice = std::clamp(window->get_music_device(), 0, 2);
    if (mt32Roms.size() < sizeof(options->mt32Roms))
        std::memcpy(options->mt32Roms, mt32Roms.c_str(), mt32Roms.size() + 1);
    options->swcDemo = window->get_swc_demo();
    if (!configPath.empty() && !save_launcher_config(configPath, *options))
        std::fprintf(stderr, "Cannot save launcher settings: %s\n",
                     path_to_utf8(configPath).c_str());
    return SDL_LAUNCHER_ACCEPTED;
}
