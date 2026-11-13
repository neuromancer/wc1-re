#include "launcher.h"
#include "launcher_api.h"
#include "launcher_config.h"
#include "launcher_paths.h"
#include "wc1_logo.h"

#include <nfd.h>
#include <slint.h>

#include <algorithm>
#include <charconv>
#include <cstdio>
#include <cstring>
#include <filesystem>
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
    if (!window.get_towns_data() && window.get_language_index() == 2)
        window.set_language_index(0);
    window.set_status(status.message);
    window.set_status_detail(slint::SharedString(status.detail));
}

void set_language(LauncherWindow &window,
                  const std::vector<GameInstallation> &installations, int language)
{
    if (language == 2 && !window.get_towns_data())
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

void browse_for_game_directory(LauncherWindow &window,
                               std::vector<GameInstallation> &installations)
{
    const std::string currentDirectory(window.get_game_directory());
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
        return;
    if (result != NFD_OKAY || selectedDirectory == nullptr) {
        report_picker_error(window, LauncherMessage::PickerFailed,
                            NFD_GetError());
        return;
    }

    const std::string selection(selectedDirectory);
    NFD_FreePathU8(selectedDirectory);
    window.set_game_directory(slint::SharedString(selection));
    refresh_window(window, installations);
}

using KeyboardBindings = slint::VectorModel<std::shared_ptr<slint::Model<InputBinding>>>;

/* One game's rows in the controls window and the bindings shown for them;
   its mouse rows are named with prefix ("" for WC1, "swc-" for SWC). */
struct BindingTable {
    std::shared_ptr<slint::Model<InputRow>> rows;
    std::shared_ptr<KeyboardBindings> bindings;
    std::string prefix;
};

BindingTable binding_table(const ControlsWindow &controls, int game)
{
    if (game == 1)
        return {controls.get_swc_rows(),
                std::dynamic_pointer_cast<KeyboardBindings>(controls.get_swc_bindings()), "swc-"};
    return {controls.get_keyboard_rows(),
            std::dynamic_pointer_cast<KeyboardBindings>(controls.get_keyboard_bindings()), ""};
}

/* A copy of a table's original bindings, row by row, that the controls
   window can change. */
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
std::string changed_key_bindings(const ControlsWindow &controls)
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

/* How many bindings a row takes: its own limit, or the window's. */
size_t binding_limit(const ControlsWindow &controls, const InputRow &row)
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
void apply_table_bindings(const ControlsWindow &controls, const BindingTable &table,
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

void apply_key_bindings(const ControlsWindow &controls, std::string_view saved)
{
    for (const int game : {0, 1})
        apply_table_bindings(controls, binding_table(controls, game), saved);
}

/* The controls window starts from the launcher's joystick selectors and has
   its own; choosing a layout there changes the launcher's selectors too. Its
   keyboard bindings start from the saved ones, and changed_key_bindings()
   reads them back for the game. */
slint::ComponentHandle<ControlsWindow> create_controls_window(
    const slint::ComponentHandle<LauncherWindow> &window, std::string_view savedBindings = {})
{
    auto controls = ControlsWindow::create();
#ifdef __APPLE__
    controls->set_apple_keys(true);
#endif
    controls->set_joystick_mode(window->get_joystick_mode_index());
    controls->set_joystick_axes(window->get_joystick_axes_index());
    controls->set_keyboard_bindings(default_bindings(*controls->get_keyboard_rows()));
    controls->set_swc_bindings(default_bindings(*controls->get_swc_rows()));
    apply_key_bindings(*controls, savedBindings);
    /* Both games' tables show the shared full-screen shortcut. */
    const auto sync_full_screen = [](const ControlsWindow &window, int game) {
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
    controls->on_binding_changed(
        [self = slint::ComponentWeakHandle<ControlsWindow>(controls), sync_full_screen](
            int row, int slot, InputBinding binding) {
            const auto controlsWindow = self.lock();
            if (!controlsWindow)
                return;
            const int game = (*controlsWindow)->get_game();
            const auto table = binding_table(**controlsWindow, game);
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
            else if (bindings->row_count() < binding_limit(**controlsWindow, *data))
                bindings->push_back(binding);
            refresh_mouse_rows(table);
            if (id == "full-screen")
                sync_full_screen(**controlsWindow, game);
        });
    controls->on_binding_removed(
        [self = slint::ComponentWeakHandle<ControlsWindow>(controls), sync_full_screen](int row, int slot) {
            const auto controlsWindow = self.lock();
            if (!controlsWindow)
                return;
            const int game = (*controlsWindow)->get_game();
            const auto table = binding_table(**controlsWindow, game);
            const auto bindings = table_bindings(table, row);
            const auto data = table.rows->row_data(row);
            if (bindings && slot >= 0 && static_cast<size_t>(slot) < bindings->row_count())
                bindings->erase(slot);
            refresh_mouse_rows(table);
            if (data && data->id == "full-screen")
                sync_full_screen(**controlsWindow, game);
        });
    /* Restore defaults resets the shown game's table. */
    controls->on_bindings_reset(
        [self = slint::ComponentWeakHandle<ControlsWindow>(controls), sync_full_screen]() {
            const auto controlsWindow = self.lock();
            if (!controlsWindow)
                return;
            const int game = (*controlsWindow)->get_game();
            if (game == 1)
                (*controlsWindow)->set_swc_bindings(default_bindings(*(*controlsWindow)->get_swc_rows()));
            else
                (*controlsWindow)->set_keyboard_bindings(default_bindings(*(*controlsWindow)->get_keyboard_rows()));
            sync_full_screen(**controlsWindow, game);
        });
    controls->on_layout_changed(
        [launcher = slint::ComponentWeakHandle<LauncherWindow>(window),
         self = slint::ComponentWeakHandle<ControlsWindow>(controls)]() {
            const auto launcherWindow = launcher.lock();
            const auto controlsWindow = self.lock();
            if (!launcherWindow || !controlsWindow)
                return;
            (*launcherWindow)->set_joystick_mode_index((*controlsWindow)->get_joystick_mode());
            (*launcherWindow)->set_joystick_axes_index((*controlsWindow)->get_joystick_axes());
        });
    return controls;
}

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

    auto window = LauncherWindow::create();
    /* The controls window opens on request and follows the launcher's
       joystick selectors, which its own selectors also change; it never
       outlives the launcher. */
    std::optional<slint::ComponentHandle<ControlsWindow>> controls;
    const auto sync_controls = [&window, &controls]() {
        if (!controls)
            return;
        (*controls)->set_joystick_mode(window->get_joystick_mode_index());
        (*controls)->set_joystick_axes(window->get_joystick_axes_index());
        (*controls)->set_game(window->get_controls_game());
    };
    const auto hide_controls = [&controls]() {
        if (controls)
            (*controls)->hide();
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
    window->set_cockpitless_view(options->cockpitlessView != 0);
    window->set_cheat_mode(options->cheatMode != 0);
    window->set_joystick_rumble(options->joystickRumble != 0);
    window->set_joystick_mode_index(options->joystickMode);
    window->set_joystick_axes_index(options->joystickAxes);
    refresh_window(*window, installations);
    set_language(*window, installations, options->language);

    window->on_language_selected([&window, &installations](int language) {
        set_language(*window, installations, language);
    });
    window->on_installation_selected([&window, &installations, hide_controls](int index) {
        if (index > 0 && static_cast<size_t>(index) <= installations.size())
            window->set_game_directory(slint::SharedString(installations[index - 1].directory));
        refresh_window(*window, installations);
        if (!window->get_controls_available())
            hide_controls();
    });
    window->on_controls_requested([&window, &controls, options, sync_controls]() {
        if (!controls)
            controls = create_controls_window(window, options->keyBindings);
        sync_controls();
        (*controls)->show();
    });
    window->on_controls_layout_changed(sync_controls);
    window->window().on_close_requested([hide_controls]() {
        hide_controls();
        return slint::CloseRequestResponse::HideWindow;
    });
    const auto launch = [&window, &accepted, &installations, &controls, options, hide_controls](
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
        if (controls) {
            const auto keyBindings = changed_key_bindings(**controls);
            if (keyBindings.size() < sizeof(options->keyBindings))
                std::memcpy(options->keyBindings, keyBindings.c_str(), keyBindings.size() + 1);
            else
                std::fprintf(stderr, "Too many changed keys to keep.\n");
        }
        accepted = true;
        /* Hide while the event loop still runs: on Linux the request takes
           effect only when the loop processes it, so hiding after the loop
           left the launcher on screen during the game. */
        hide_controls();
        window->hide();
        slint::quit_event_loop();
    };
    window->on_launch_requested([launch](int campaign, bool transfer) {
        launch(campaign, transfer, false);
    });
    window->on_credits_requested([launch]() {
        launch(0, false, true);
    });
    window->on_cancel_requested([hide_controls]() {
        hide_controls();
        slint::quit_event_loop();
    });

    window->show();
    /* NFD_Init records Cocoa's activation policy. Initialize it from the
     * callback, after winit launches NSApplication, so NFD_Quit cannot restore
     * the temporary pre-launch Prohibited policy before SDL takes over. */
    window->on_browse_requested([&window, &pickerInitialized, &installations, hide_controls]() {
        if (!pickerInitialized) {
            if (NFD_Init() != NFD_OKAY) {
                report_picker_error(
                    *window, LauncherMessage::PickerInitFailed,
                    NFD_GetError());
                return;
            }
            pickerInitialized = true;
        }
        browse_for_game_directory(*window, installations);
        if (!window->get_controls_available())
            hide_controls();
    });
    slint::run_event_loop();
    if (pickerInitialized)
        NFD_Quit();
    hide_controls();
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
    options->cockpitlessView = window->get_cockpitless_view();
    options->cheatMode = window->get_cheat_mode();
    options->joystickRumble = window->get_joystick_rumble();
    options->joystickMode = std::max(0, window->get_joystick_mode_index());
    options->joystickAxes = std::max(0, window->get_joystick_axes_index());
    options->swcDemo = window->get_swc_demo();
    if (!configPath.empty() && !save_launcher_config(configPath, *options))
        std::fprintf(stderr, "Cannot save launcher settings: %s\n",
                     path_to_utf8(configPath).c_str());
    return SDL_LAUNCHER_ACCEPTED;
}
