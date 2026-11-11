#include "launcher.h"
#include "launcher_api.h"
#include "launcher_config.h"
#include "launcher_paths.h"
#include "wc1_logo.h"

#include <nfd.h>
#include <slint.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <string>
#include <system_error>

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
    window->on_installation_selected([&window, &installations](int index) {
        if (index > 0 && static_cast<size_t>(index) <= installations.size())
            window->set_game_directory(slint::SharedString(installations[index - 1].directory));
        refresh_window(*window, installations);
    });
    window->on_launch_requested([&window, &accepted, &installations, options](int campaign, bool transfer) {
        refresh_window(*window, installations);
        if (!window->get_path_valid() || campaign < 0 || campaign > 2 || (transfer && !campaign) ||
            (campaign == 1 && !window->get_sm1_data()) ||
            (campaign == 2 && !window->get_sm2_data()))
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
        accepted = true;
        /* Hide while the event loop still runs: on Linux the request takes
           effect only when the loop processes it, so hiding after the loop
           left the launcher on screen during the game. */
        window->hide();
        slint::quit_event_loop();
    });
    window->on_cancel_requested([]() {
        slint::quit_event_loop();
    });

    window->show();
    /* NFD_Init records Cocoa's activation policy. Initialize it from the
     * callback, after winit launches NSApplication, so NFD_Quit cannot restore
     * the temporary pre-launch Prohibited policy before SDL takes over. */
    window->on_browse_requested([&window, &pickerInitialized, &installations]() {
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
    });
    slint::run_event_loop();
    if (pickerInitialized)
        NFD_Quit();
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
