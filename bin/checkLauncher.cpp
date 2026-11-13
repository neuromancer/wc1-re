/* Exercise the real launcher language handler on an offscreen Slint window. */
#include "../src/sdl/slint/slint_static.h"
#include "../src/sdl/slint/launcher.cpp"
#include <slint-platform.h>
#include <SDL.h>
#include "../src/sdl/keymap_actions.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <thread>

// The fixture doesn't open SWC data; directory discovery stays outside the test.
extern "C" int SdlHasSwcResources(const char *)
{
    return 0;
}

// Detection/playback are tested with real files by check-voices.
extern "C" int SdlFindVoiceDirectory(const char *, char *, unsigned long)
{
    return 0;
}

struct TestWindow : slint::platform::WindowAdapter {
    slint::platform::SoftwareRenderer pixels {slint::platform::SoftwareRenderer::RepaintBufferType::NewBuffer};
    slint::PhysicalSize dimensions {{760, 660}};
    slint::LogicalSize minimum;
    void update_window_properties(const WindowProperties &properties) override
    {
        minimum = properties.layout_constraints().min.value_or(slint::LogicalSize());
    }
    slint::PhysicalSize size() override
    {
        return dimensions;
    }
    slint::platform::AbstractRenderer &renderer() override
    {
        return pixels;
    }
};

struct TestPlatform : slint::platform::Platform {
    TestWindow *view = nullptr;
    std::unique_ptr<slint::platform::WindowAdapter> create_window_adapter() override {
        auto adapter = std::make_unique<TestWindow>();
        view = adapter.get();
        return adapter;
    }
};

static void require(bool condition, const char *message)
{
    if (!condition) {
        std::fprintf(stderr, "FAIL: %s\n", message);
        std::exit(1);
    }
}

int main(int argc, char **argv)
{
    auto backend = std::make_unique<TestPlatform>();
    auto *platform = backend.get();
    slint::platform::set_platform(std::move(backend));
    auto window = LauncherWindow::create();
    std::vector<GameInstallation> installations = {
        {"/Games/Edición española", LauncherMessage::MacDemo},
        {"/Games/WC1", LauncherMessage::Dos, true}
    };
    window->set_title_artwork(slint::Image::load_from_data(
        wc1_launcher_artwork::kWingCommanderLogoPng, "png"));
    window->set_installation_index(1);
    window->set_game_directory(slint::SharedString(installations[0].directory));
    window->set_start_fullscreen(true);
    window->set_autosave(true);
    window->set_joystick_mode_index(2);
    window->set_joystick_axes_index(6);
    window->set_picker_error(true);
    window->set_status(LauncherMessage::MissingStreamFile);
    window->set_status_detail("MISSION.STR");
    require(window->get_language_choices()->row_count() == 3 &&
            *window->get_language_choices()->row_data(2) == "Français", "Japanese hidden without Towns data");
    set_language(*window, installations, 2);
    require(window->get_language_index() == 0, "saved Japanese selection falls back outside Towns");
    window->show();
    window->window().dispatch_resize_event(slint::LogicalSize {{760, 660}});
    std::vector<slint::Rgb8Pixel> english(760 * 660), spanish(760 * 660), restored(760 * 660);
    set_language(*window, installations, 0);
    platform->view->pixels.render(english, 760);
    const auto originalError = window->get_status_text();
    for (int attempt = 0; attempt < 3; attempt++) {
        set_language(*window, installations, 1);
        require(window->get_status_text() == "STREAMS/MISSION.STR no existe o no se puede leer.", "live error translation");
        require(window->get_installation_prompt() == "Selecciona los datos del juego…", "live installation prompt");
        require(std::string(*window->get_installations()->row_data(1)).starts_with("Demo de SWC para Mac"), "live edition label");
        require(std::string(*window->get_installations()->row_data(2)).find("Voces de Sega CD") != std::string::npos, "live voice detection label");
        platform->view->pixels.render(spanish, 760);
        set_language(*window, installations, 0);
        require(window->get_status_text() == originalError, "English restored");
        require(std::string(*window->get_installations()->row_data(2)).find("Sega CD voices") != std::string::npos, "English voice label restored");
        platform->view->pixels.render(restored, 760);
    }
    require(std::memcmp(english.data(), spanish.data(), english.size() * sizeof(english[0])) != 0, "window redraws in Spanish");
    require(std::memcmp(english.data(), restored.data(), english.size() * sizeof(english[0])) == 0, "window restores English pixels");
    require(window->get_installation_index() == 1 && std::string(window->get_game_directory()) == installations[0].directory, "selected path preserved");
    require(window->get_start_fullscreen() && window->get_autosave() && window->get_joystick_mode_index() == 2 && window->get_joystick_axes_index() == 6, "settings preserved");
    require(window->get_picker_error() && window->get_status_detail() == "MISSION.STR", "error details preserved");
    window->set_swc_demo(true);
    set_language(*window, installations, 1);
    require(window->get_language_index() == 1, "Spanish launcher supports SWC selection");
    require(validate_game_directory("").message == LauncherMessage::ChooseDirectory, "directory validation returns translatable status");
    window->set_status(LauncherMessage::MissingSwcResources);
    platform->view->dimensions = slint::PhysicalSize {{640, 660}};
    window->window().dispatch_resize_event(slint::LogicalSize {{640, 660}});
    std::vector<slint::Rgb8Pixel> small(640 * 660);
    platform->view->pixels.render(small, 640);
    const auto height = static_cast<unsigned>(std::ceil(platform->view->minimum.height));
    platform->view->dimensions.height = height;
    window->window().dispatch_resize_event(slint::LogicalSize {{640, static_cast<float>(height)}});
    small.resize(640 * height);
    platform->view->pixels.render(small, 640);
    if (argc > 1) {
        SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(spanish.data(), 760, 660, 24, 760 * 3, SDL_PIXELFORMAT_RGB24);
        require(surface && SDL_SaveBMP(surface, argv[1]) == 0, "save launcher preview");
        SDL_FreeSurface(surface);
        surface = SDL_CreateRGBSurfaceWithFormatFrom(small.data(), 640, height, 24, 640 * 3, SDL_PIXELFORMAT_RGB24);
        const std::string smallPath = std::string(argv[1]) + ".small.bmp";
        require(surface && SDL_SaveBMP(surface, smallPath.c_str()) == 0, "save minimum-size preview");
        SDL_FreeSurface(surface);
    }
    window->hide();
    const auto fixture = fs::temp_directory_path() /
        ("wc1-towns-check-" + std::to_string(SDL_GetPerformanceCounter()));
    fs::create_directories(fixture / "GAMEDAT");
    const auto gameData = fixture / "GAMEDAT";
    const char moduleHeader[8] = {28, 0, 0, 0, 28, 0, 0, 2};
    std::ofstream(gameData / "MODULE.000", std::ios::binary).write(moduleHeader, 8);
    for (const auto name : {"COMMUNIC.JAT", "CAMP.000", "BRIEFING.000", "FONTS.FNT", "TITLES.VGA"})
        std::ofstream(gameData / name).put('x');
    const auto towns = validate_game_directory(path_to_utf8(fixture));
    require(towns.valid && towns.message == LauncherMessage::FmTowns,
            "Towns detected without INSTALL.DAT or STREAMS");
    window->set_game_directory(slint::SharedString(path_to_utf8(fixture)));
    refresh_window(*window, installations);
    require(!window->get_controls_available(), "controller bindings cover only DOS/Win32 WC1 for now");
    require(window->get_language_choices()->row_count() == 4 &&
            *window->get_language_choices()->row_data(2) == "Japanese (FM Towns)", "Japanese offered for detected Towns data");
    set_language(*window, installations, 1);
    require(window->get_status_text() == "FM Towns: texto en inglés/japonés.",
            "Towns status translated in launcher");
    set_language(*window, installations, 2);
    require(window->get_language_index() == 2, "Japanese selection retained");
    SdlLauncherOptions saved = {}, loaded = {};
    saved.language = window->get_language_index();
    require(save_launcher_config(fixture / "launcher.ini", saved), "save Japanese selection");
    load_launcher_config(fixture / "launcher.ini", loaded);
    require(loaded.language == 2, "restore Japanese selection");
    for (const bool dos : {true, false}) {
        const auto pc = fixture / (dos ? "dos" : "saga");
        fs::create_directories(pc / "GAMEDAT");
        char pcHeader[8] = {28, 0, 0, 0, 28, 0, 0, static_cast<char>(dos ? 1 : 0)};
        std::ofstream(pc / "GAMEDAT/MODULE.000", std::ios::binary).write(pcHeader, 8);
        for (const auto name : {"CAMP.000", "INSTALL.DAT"})
            std::ofstream(pc / "GAMEDAT" / name).put('x');
        fs::create_directories(pc / "STREAMS");
        for (const auto name : {"PREFLITE.STR", "POSFLITE.STR", "MISSION.STR"})
            std::ofstream(pc / "STREAMS" / name).put('x');
        window->set_game_directory(slint::SharedString(path_to_utf8(pc)));
        refresh_window(*window, installations);
        require(window->get_path_valid() && window->get_status() ==
                (dos ? LauncherMessage::Dos : LauncherMessage::KilrathiSaga), "PC edition detected");
        require(window->get_controls_available(), "controller bindings offered for DOS/Win32 WC1");
        require(window->get_language_choices()->row_count() == 3 && window->get_language_index() == 0,
                "switching from Towns removes Japanese and selects English");
        set_language(*window, installations, loaded.language);
        require(window->get_language_index() == 0, "Japanese setting rejected for PC edition");
        set_language(*window, installations, 3);
        window->set_game_directory(slint::SharedString(path_to_utf8(fixture)));
        refresh_window(*window, installations);
        require(window->get_language_index() == 3, "French ID survives language-list change");
        set_language(*window, installations, 2);
        require(window->get_language_index() == 2, "Japanese available again after returning to Towns");
    }
    set_language(*window, installations, 3);
    require(window->get_language_index() == 3, "French selection retained");
    require(window->get_installation_prompt() == "Sélectionner les données du jeu…", "live French prompt");
    require(std::string(*window->get_installations()->row_data(2)).find("Voix Sega CD") != std::string::npos,
            "live French voice label");
    platform->view->pixels.render(small, 640);
    saved.language = 3;
    require(save_launcher_config(fixture / "launcher.ini", saved), "save French selection");
    load_launcher_config(fixture / "launcher.ini", loaded);
    require(loaded.language == 3, "restore French selection");
    set_language(*window, installations, 0);
    require(window->get_installation_prompt() == "Select game data…", "English restored after French");
    fs::remove(gameData / "FONTS.FNT");
    require(!validate_game_directory(path_to_utf8(fixture)).valid, "incomplete Towns data rejected");
    fs::remove(gameData / "COMMUNIC.JAT");
    require(validate_game_directory(path_to_utf8(fixture)).message != LauncherMessage::FmTowns,
            "raw packet flag alone does not identify Towns");

    const auto saga = fixture / "saga";
    const auto expansionData = saga / "GAMEDAT";
    const std::array<const char *, 5> sm1Files = {
        "CAMP.001", "MODULE.001", "BRIEFING.001", "MIDGAME.V04", "MIDGAME.V05"
    };
    const std::array<const char *, 11> sm2Files = {
        "CAMP.002", "MODULE.002", "BRIEFING.002", "MIDGAME.V06", "MIDGAME.V07",
        "MIDGAME.V08", "TALKING1.VGA", "WINGMEN1.VGA", "PCSHIP.V05", "COMMSM2.DAT", "TITLE1.VGA"
    };
    int launched = 0, transferred = 0;
    window->on_launch_requested([&](int campaign, bool transfer) {
        (transfer ? transferred : launched) |= 1 << campaign;
    });
    window->show();
    platform->view->dimensions = slint::PhysicalSize {{760, 660}};
    window->window().dispatch_resize_event(slint::LogicalSize {{760, 660}});
    int launcherHeight = 660;
    auto clickLaunchRows = [&]() {
        window->show(); // Refresh native size constraints without an event loop.
        platform->view->pixels.render(english, 760);
        launcherHeight = std::max(660, static_cast<int>(std::ceil(platform->view->minimum.height)));
        platform->view->dimensions = slint::PhysicalSize {{760, static_cast<unsigned>(launcherHeight)}};
        window->window().dispatch_resize_event(slint::LogicalSize {{760, static_cast<float>(launcherHeight)}});
        english.resize(760 * launcherHeight);
        platform->view->pixels.render(english, 760);
        launched = transferred = 0;
        // Hit the launch and transfer rows through Slint's real input dispatch.
        for (const float y : {launcherHeight - 81.f, launcherHeight - 35.f}) {
            for (float x = 8; x < 760; x += 8) {
                window->window().dispatch_pointer_press_event(slint::LogicalPosition {{x, y}}, slint::PointerEventButton::Left);
                window->window().dispatch_pointer_release_event(slint::LogicalPosition {{x, y}}, slint::PointerEventButton::Left);
            }
        }
    };
    auto checkLaunchButtons = [&](const fs::path &directory, int expected) {
        window->set_game_directory(slint::SharedString(path_to_utf8(directory)));
        refresh_window(*window, installations);
        clickLaunchRows();
        if (launched != expected || transferred != (expected & 6))
            std::fprintf(stderr, "Launch %d / transfer %d, expected %d / %d; minimum height %.0f\n",
                         launched, transferred, expected, expected & 6, platform->view->minimum.height);
        require(launched == expected, "launcher buttons match detected campaigns and route clicks");
        require(transferred == (expected & 6), "transfer buttons only target detected expansions");
    };
    checkLaunchButtons(saga, 1);
    for (const auto name : sm1Files)
        std::ofstream(expansionData / name).put('x');
    checkLaunchButtons(saga, 3);
    for (const auto name : sm2Files)
        std::ofstream(expansionData / name).put('x');
    checkLaunchButtons(saga, 7);
    if (argc > 1) {
        platform->view->pixels.render(english, 760);
        SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(english.data(), 760, launcherHeight, 24, 760 * 3, SDL_PIXELFORMAT_RGB24);
        const std::string path = std::string(argv[1]) + ".expansions.bmp";
        require(surface && SDL_SaveBMP(surface, path.c_str()) == 0, "save expansion button preview");
        SDL_FreeSurface(surface);
    }
    for (const auto name : sm1Files) {
        fs::remove(expansionData / name);
        checkLaunchButtons(saga, 5);
        std::ofstream(expansionData / name).put('x');
    }
    for (const auto name : sm2Files) {
        // An empty or missing required file must hide only that expansion.
        std::ofstream(expansionData / name, std::ios::trunc);
        checkLaunchButtons(saga, 3);
        fs::remove(expansionData / name);
        checkLaunchButtons(saga, 3);
        std::ofstream(expansionData / name).put('x');
    }
    fs::rename(expansionData / "CAMP.001", expansionData / "camp.001");
    checkLaunchButtons(saga, 7);
    for (const auto name : sm1Files)
        std::ofstream(fixture / "dos/GAMEDAT" / name).put('x');
    for (const auto name : sm2Files)
        std::ofstream(fixture / "dos/GAMEDAT" / name).put('x');
    checkLaunchButtons(fixture / "dos", 1);
    checkLaunchButtons(fixture / "missing", 0);
    // SWC detection needs the game's stubbed resource search, so set the
    // detected state directly and click through the real layout.
    int credits = 0;
    window->on_credits_requested([&]() { credits++; });
    for (const bool available : {false, true}) {
        window->set_path_valid(true);
        window->set_status(LauncherMessage::MacDemo);
        window->set_swc_demo(true);
        window->set_sm1_data(false);
        window->set_sm2_data(false);
        window->set_swc_credits(available);
        credits = 0;
        clickLaunchRows();
        require(launched == 1 && !transferred, "SWC offers only its own start button");
        require(window->get_controls_available() && window->get_controls_game() == 1,
                "SWC data offers its own controls");
        require((credits != 0) == available, "SWC credits button follows detected credits data");
    }
    saved.campaign = 2;
    saved.transferPilot = 1;
    saved.swcCredits = 1;
    loaded.campaign = 0;
    require(save_launcher_config(fixture / "launcher.ini", saved), "save settings after SM2 selection");
    load_launcher_config(fixture / "launcher.ini", loaded);
    require(loaded.campaign == 0 && !loaded.transferPilot && !loaded.swcCredits,
            "campaign, transfer and credits choices are per launch");
    std::strcpy(saved.keyBindings, "pitch-down=k\nlock-target=\nroll-left=%2c,%c3%b1\n");
    require(save_launcher_config(fixture / "launcher.ini", saved), "save changed keys");
    load_launcher_config(fixture / "launcher.ini", loaded);
    require(std::string(loaded.keyBindings) == saved.keyBindings, "changed keys survive a restart");
    {
        std::ofstream settings(fixture / "launcher.ini", std::ios::app);
        settings << "pitch-up=UP\nfire-guns=space\n\"bad\"=k\n";
    }
    load_launcher_config(fixture / "launcher.ini", loaded);
    require(std::string(loaded.keyBindings) == std::string(saved.keyBindings) + "fire-guns=space\n",
            "malformed key lines are skipped");
    saved.keyBindings[0] = '\0';

    // The controls window starts from the launcher's joystick selectors.
    window->set_joystick_mode_index(1);
    window->set_joystick_axes_index(0);
    auto controlsWindow = create_controls_window(window);
    require(controlsWindow->get_joystick_mode() == 1 && controlsWindow->get_joystick_axes() == 0,
            "controls window starts from the launcher's selectors");
    const auto binding = [&controlsWindow](PadControl control) {
        return std::string(controlsWindow->invoke_wc1_binding(control));
    };
    const auto entry = [](const std::shared_ptr<slint::Model<slint::SharedString>> &model, size_t row) {
        return std::string(model->row_data(row).value_or(slint::SharedString()));
    };
    controlsWindow->set_joystick_mode(0);
    require(binding(PadControl::A) == "fire guns" && binding(PadControl::B) == "hold to roll and change speed" &&
            binding(PadControl::RightTrigger) == "fire guns" &&
            binding(PadControl::LeftTrigger) == "hold to roll and change speed" &&
            binding(PadControl::DpadUp).empty() && binding(PadControl::Start) == "pause" &&
            binding(PadControl::Back) == "cycle views",
            "original two-button controller bindings, the triggers standing in for A and B");
    controlsWindow->set_joystick_mode(1);
    require(binding(PadControl::B) == "fire missile" && binding(PadControl::Y) == "hold to roll and change speed" &&
            binding(PadControl::DpadUp) == "full speed" && binding(PadControl::RightStick).empty(),
            "WCAT two-axis controller bindings");
    controlsWindow->set_joystick_mode(2);
    controlsWindow->set_joystick_axes(2);
    require(binding(PadControl::Y) == "cycle targets" && binding(PadControl::LeftStick) == "roll and pitch" &&
            binding(PadControl::RightStick) == "yaw, throttle position" &&
            binding(PadControl::LeftTrigger) == "fire missile" &&
            binding(PadControl::RightTrigger) == "fire guns", "WCAT four-axis HOTAS-yaw controller bindings");
    require(controlsWindow->get_mode_names()->row_count() == 3 && controlsWindow->get_axis_names()->row_count() == 7 &&
            entry(controlsWindow->get_mode_names(), 2) == "WCAT — four buttons / four axes" &&
            entry(controlsWindow->get_axis_names(), 2) == "HOTAS yaw", "controls window layout names");
    require(slint::select_bundled_translation("es") && binding(PadControl::A) == "disparar cañones" &&
            entry(controlsWindow->get_axis_names(), 2) == "HOTAS: guiñada",
            "controls window follows the Spanish launcher");
    require(slint::select_bundled_translation("en") && binding(PadControl::A) == "fire guns",
            "controls window returns to English");
    require(controlsWindow->get_page() == 0, "controls window opens on the game controller");
    controlsWindow->show();
    platform->view->dimensions = slint::PhysicalSize {{1040, 660}};
    controlsWindow->window().dispatch_resize_event(slint::LogicalSize {{1040, 660}});
    const auto save_controls = [&platform, argc, argv](const char *suffix, const char *what) {
        // Let the page buttons finish their 150 ms colour animations.
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        slint::platform::update_timers_and_animations();
        std::vector<slint::Rgb8Pixel> controlsPixels(1040 * 660);
        platform->view->pixels.render(controlsPixels, 1040);
        if (argc > 1) {
            SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(controlsPixels.data(), 1040, 660, 24, 1040 * 3, SDL_PIXELFORMAT_RGB24);
            const std::string path = std::string(argv[1]) + suffix;
            require(surface && SDL_SaveBMP(surface, path.c_str()) == 0, what);
            SDL_FreeSurface(surface);
        }
    };
    // Choosing a layout in the window's own selectors changes the launcher's.
    controlsWindow->set_joystick_mode(0);
    controlsWindow->set_joystick_axes(0);
    window->set_joystick_mode_index(0);
    window->set_joystick_axes_index(0);
    const auto choose = [&controlsWindow](float x, int steps) {
        controlsWindow->window().dispatch_pointer_press_event(slint::LogicalPosition {{x, 66}}, slint::PointerEventButton::Left);
        controlsWindow->window().dispatch_pointer_release_event(slint::LogicalPosition {{x, 66}}, slint::PointerEventButton::Left);
        for (int step = 0; step < steps; ++step) {
            controlsWindow->window().dispatch_key_press_event(slint::SharedString(slint::platform::key_codes::DownArrow));
            controlsWindow->window().dispatch_key_release_event(slint::SharedString(slint::platform::key_codes::DownArrow));
        }
        controlsWindow->window().dispatch_key_press_event(slint::SharedString(slint::platform::key_codes::Return));
        controlsWindow->window().dispatch_key_release_event(slint::SharedString(slint::platform::key_codes::Return));
    };
    choose(250, 2);
    require(controlsWindow->get_joystick_mode() == 2 && window->get_joystick_mode_index() == 2,
            "controls window's mode selector changes the launcher's");
    choose(640, 1);
    require(controlsWindow->get_joystick_axes() == 1 && window->get_joystick_axes_index() == 1 &&
            window->get_joystick_mode_index() == 2, "controls window's axis selector changes the launcher's");
    save_controls(".controls.bmp", "save controls window preview");
    // The "Keyboard and mouse" button sits at the top right of the window.
    controlsWindow->window().dispatch_pointer_press_event(slint::LogicalPosition {{1040 - 16 - 60, 30}}, slint::PointerEventButton::Left);
    controlsWindow->window().dispatch_pointer_release_event(slint::LogicalPosition {{1040 - 16 - 60, 30}}, slint::PointerEventButton::Left);
    require(controlsWindow->get_page() == 1, "keyboard and mouse button switches pages");

    // One table row per game input, each with its keys and mouse inputs.
    const auto rows = controlsWindow->get_keyboard_rows();
    const auto find_row = [&rows](const char *id) {
        for (size_t index = 0; index < rows->row_count(); ++index) {
            const auto row = rows->row_data(index);
            if (row && row->id == id)
                return static_cast<int>(index);
        }
        return -1;
    };
    const auto row_at = [&rows](int index) {
        return index >= 0 ? rows->row_data(index).value_or(InputRow {}) : InputRow {};
    };
    const auto binding_at = [](const InputRow &row, size_t index) {
        return row.bindings ? row.bindings->row_data(index).value_or(InputBinding {}) : InputBinding {};
    };
    const auto inputs = [](const InputBinding &binding) {
        std::string text;
        for (size_t index = 0; binding.inputs && index < binding.inputs->row_count(); ++index)
            text += (index ? "|" : "") + std::string(binding.inputs->row_data(index).value_or(slint::SharedString()));
        return text;
    };
    const int gunsRow = find_row("mouse-guns");
    const int afterburnerRow = find_row("mouse-afterburner");
    const int missileRow = find_row("mouse-missile");
    const int rollRow = find_row("mouse-roll");
    const auto eject = row_at(find_row("eject"));
    const auto pause = row_at(find_row("pause"));
    const auto afterburner = row_at(find_row("afterburner"));
    const auto gunsButton = row_at(gunsRow);
    require(rows->row_count() == 56 && eject.bindings && eject.bindings->row_count() == 1 &&
            inputs(binding_at(eject, 0)) == "ctrl|e" && binding_at(eject, 0).joiner == "+" &&
            pause.bindings && pause.bindings->row_count() == 2 &&
            inputs(binding_at(pause, 0)) == "p" && inputs(binding_at(pause, 1)) == "escape" &&
            binding_at(pause, 1).fixed && afterburner.hint == "hold" && afterburner.bindings->row_count() == 1 &&
            inputs(binding_at(gunsButton, 0)) == "mouse-left" && gunsButton.mouse_only && gunsButton.limit == 1 &&
            inputs(binding_at(row_at(afterburnerRow), 0)) == "mouse-right" &&
            row_at(find_row("choose-message")).fixed && find_row("mouse-steer") < 0 &&
            row_at(missileRow).fixed && row_at(rollRow).fixed,
            "keyboard and mouse table names each game input's keys and mouse inputs");
    // The window changes a copy of those bindings that the launcher keeps.
    const auto current = [&controlsWindow, &inputs](int row) {
        std::string text;
        const auto table = controlsWindow->get_keyboard_bindings();
        const auto bindings = table && row >= 0 && static_cast<size_t>(row) < table->row_count()
            ? table->row_data(row).value_or(nullptr) : nullptr;
        for (size_t slot = 0; bindings && slot < bindings->row_count(); ++slot)
            text += (slot ? "/" : "") + inputs(bindings->row_data(slot).value_or(InputBinding {}));
        return text;
    };
    require(controlsWindow->get_keyboard_bindings()->row_count() == 56 && current(1) == "up" &&
            current(find_row("roll-left")) == ",/insert" && current(find_row("speed-up")) == "+/wheel-up" &&
            current(missileRow) == "mouse-left|mouse-right" && current(rollRow) == "mouse-right|mouse-move",
            "the launcher copies the original bindings for the window");
    // The rows the game can rebind, in src/sdl/keymap_actions.h's order.
    {
#define LAUNCHER_KEYMAP_ID(id, ...) id,
        static const char *const gameActions[] = {SDL_KEYMAP_ACTION_LIST(LAUNCHER_KEYMAP_ID)};
#undef LAUNCHER_KEYMAP_ID
        size_t next = 0;
        bool matches = true;
        for (size_t index = 0; index < rows->row_count(); ++index) {
            const auto row = rows->row_data(index);
            if (!row || row->id.empty() || row->fixed || row->id == "full-screen" ||
                row->id == "mouse-guns" || row->id == "mouse-afterburner")
                continue;
            matches = matches && next < std::size(gameActions) && row->id == gameActions[next];
            ++next;
        }
        require(matches && next == std::size(gameActions), "the game rebinds every changeable row by its id");
#define LAUNCHER_SWC_KEYMAP_ID(id, ...) id,
        static const char *const swcActions[] = {SDL_KEYMAP_SWC_ACTION_LIST(LAUNCHER_SWC_KEYMAP_ID)};
#undef LAUNCHER_SWC_KEYMAP_ID
        const auto swcRows = controlsWindow->get_swc_rows();
        next = 0;
        matches = true;
        for (size_t index = 0; index < swcRows->row_count(); ++index) {
            const auto row = swcRows->row_data(index);
            if (!row || row->id.empty() || row->fixed || row->id == "full-screen" ||
                row->id == "swc-mouse-guns" || row->id == "swc-mouse-afterburner")
                continue;
            matches = matches && next < std::size(swcActions) && row->id == swcActions[next];
            ++next;
        }
        require(matches && next == std::size(swcActions), "SWC rebinds every changeable row by its id");
    }
    for (size_t index = 0; index < rows->row_count(); ++index) {
        const auto row = rows->row_data(index);
        const auto limit = static_cast<size_t>(row && row->limit > 0 ? row->limit : controlsWindow->get_max_bindings());
        require(row && (!row->bindings || row->bindings->row_count() <= limit),
                "no game input starts with more bindings than it takes");
    }

    // The table scrolls; save it a page at a time, down to its end.
    const auto save_keyboard = [&controlsWindow, &save_controls](const std::string &name) {
        for (int page = 1; page <= 5; ++page) {
            const std::string suffix = page == 1 ? "" : "-" + std::to_string(page);
            save_controls((name + suffix + ".bmp").c_str(), "save keyboard and mouse preview");
            controlsWindow->window().dispatch_pointer_scroll_event(slint::LogicalPosition {{500, 300}}, 0, -440);
        }
        controlsWindow->window().dispatch_pointer_scroll_event(slint::LogicalPosition {{500, 300}}, 0, 8000);
    };
#ifdef __APPLE__
    require(controlsWindow->get_apple_keys(), "macOS shows Command-Return for full screen");
#else
    require(!controlsWindow->get_apple_keys(), "Alt-Enter toggles full screen");
#endif
    const int fullScreen = find_row("full-screen");
    // Command or Control in the original bindings follows the platform; reset
    // both games' tables after switching it, as only a test does.
    const auto set_apple_keys = [&controlsWindow](bool apple) {
        const int game = controlsWindow->get_game();
        controlsWindow->set_apple_keys(apple);
        controlsWindow->set_game(1);
        controlsWindow->invoke_bindings_reset();
        controlsWindow->set_game(0);
        controlsWindow->invoke_bindings_reset();
        controlsWindow->set_game(game);
    };
    set_apple_keys(false);
    require(current(fullScreen) == "alt|enter", "Alt-Enter toggles full screen outside macOS");
    save_keyboard(".keyboard");
    set_apple_keys(true);
    require(current(fullScreen) == "meta|enter", "Command-Return toggles full screen on macOS");
    save_keyboard(".keyboard-mac");
    for (const std::string language : {"es", "fr"}) {
        require(slint::select_bundled_translation(language), "translated controls window");
        controlsWindow->set_page(1);
        save_keyboard(".keyboard-" + language);
        controlsWindow->set_page(0);
        save_controls((".controls-" + language + ".bmp").c_str(), "save translated game controller preview");
    }
    require(slint::select_bundled_translation("en"), "controls window returns to English");

    // Clicking a binding captures the next key, mouse button or wheel step;
    // any key works because inputs are names, not pictures.
    set_apple_keys(false);
    controlsWindow->set_page(1);
    // The list places its rows as it draws, and its scrolling moves over
    // time, so let it settle and draw before clicking, as a screen would.
    const auto settle = [&platform](int milliseconds) {
        std::vector<slint::Rgb8Pixel> pixels(1040 * 660);
        if (milliseconds > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        slint::platform::update_timers_and_animations();
        platform->view->pixels.render(pixels, 1040);
    };
    const auto click = [&controlsWindow, &settle](float x, float y, slint::PointerEventButton button) {
        settle(0);
        controlsWindow->window().dispatch_pointer_press_event(slint::LogicalPosition {{x, y}}, button);
        controlsWindow->window().dispatch_pointer_release_event(slint::LogicalPosition {{x, y}}, button);
    };
    // A capture dialog takes the keyboard when it first draws.
    const auto press = [&controlsWindow, &settle](const slint::SharedString &key) {
        settle(0);
        controlsWindow->window().dispatch_key_press_event(key);
    };
    const auto release = [&controlsWindow](const slint::SharedString &key) {
        controlsWindow->window().dispatch_key_release_event(key);
    };
    // Rows are 40 pixels from y 105 in a 490-pixel list, and bindings start
    // at x 396. show_row scrolls a row into view and gives its y.
    const auto row_y = [](int row) {
        return 105.f + 40.f * row + 20.f;
    };
    const auto show_row = [&controlsWindow, &rows, &settle](int row) {
        // Slint animates wheel scrolling, so place the list directly.
        const float target = std::clamp(40.f * row - 200.f, 0.f, 40.f * rows->row_count() - 490.f);
        controlsWindow->set_table_scroll(-target);
        settle(0);
        return 105.f + 40.f * row - target + 20.f;
    };
    const auto left = slint::PointerEventButton::Left;
    click(410, row_y(1), left);
    require(controlsWindow->get_capture_row() == 1, "clicking a binding asks for a new one");
    save_controls(".keyboard-capture.bmp", "save binding capture preview");
    press("ñ");
    release("ñ");
    require(controlsWindow->get_capture_row() == -1 && current(1) == "ñ", "a key replaces the binding");
    click(410, row_y(2), left);
    press(slint::SharedString(slint::platform::key_codes::Control));
    press("x");
    release("x");
    release(slint::SharedString(slint::platform::key_codes::Control));
    require(current(2) == "ctrl|x", "a modifier and a key make one binding");
    click(410, row_y(3), left);
    press(slint::SharedString(slint::platform::key_codes::Shift));
    release(slint::SharedString(slint::platform::key_codes::Shift));
    require(current(3) == "shift", "a modifier alone is a binding");
    click(410, row_y(4), left);
    click(520, 330, slint::PointerEventButton::Middle);
    require(controlsWindow->get_capture_row() == -1 && current(4) == "mouse-middle",
            "a mouse button in the capture box is a binding");
    click(410, row_y(5), left);
    controlsWindow->window().dispatch_pointer_scroll_event(slint::LogicalPosition {{520, 330}}, 0, 40);
    require(current(5) == "wheel-up", "a wheel step in the capture box is a binding");
    click(446, row_y(1), left);
    press("k");
    release("k");
    require(current(1) == "ñ/k", "+ adds another binding");
    // Two bindings is the limit: the + is gone, and the launcher refuses more.
    click(493, row_y(1), left);
    require(controlsWindow->get_capture_row() == -1, "a game input with two bindings offers no +");
    InputBinding third;
    third.inputs = std::make_shared<slint::VectorModel<slint::SharedString>>(std::vector<slint::SharedString> {"j"});
    controlsWindow->invoke_binding_changed(1, 2, third);
    require(current(1) == "ñ/k", "the launcher keeps two bindings at most");
    click(410, row_y(6), left);
    press(slint::SharedString(slint::platform::key_codes::Escape));
    release(slint::SharedString(slint::platform::key_codes::Escape));
    require(controlsWindow->get_capture_row() == -1 && current(6) == "page-up", "Escape cancels");
    click(410, row_y(7), left);
    click(100, 100, left);
    require(controlsWindow->get_capture_row() == -1 && current(7) == "end", "a click outside the box cancels");
    click(430, show_row(missileRow), left);
    require(controlsWindow->get_capture_row() == -1, "fixed rows cannot be changed");

    // The game's two mouse buttons take only mouse buttons; the missile and
    // roll rows follow them, and giving one the other's button swaps them.
    click(430, show_row(gunsRow), left);
    require(controlsWindow->get_capture_row() == gunsRow, "the guns button row asks for a button");
    save_controls(".keyboard-capture-mouse.bmp", "save mouse button capture preview");
    press("k");
    release("k");
    controlsWindow->window().dispatch_pointer_scroll_event(slint::LogicalPosition {{520, 330}}, 0, 40);
    require(controlsWindow->get_capture_row() == gunsRow, "the guns button row ignores keys and the wheel");
    click(520, 330, slint::PointerEventButton::Middle);
    require(current(gunsRow) == "mouse-middle" && current(missileRow) == "mouse-middle|mouse-right",
            "the missile row follows the guns button");
    click(430, show_row(afterburnerRow), left);
    click(520, 330, slint::PointerEventButton::Back);
    require(current(afterburnerRow) == "mouse-back" && current(rollRow) == "mouse-back|mouse-move",
            "the roll row follows the afterburner button");
    click(430, show_row(gunsRow), left);
    click(520, 330, slint::PointerEventButton::Back);
    require(current(gunsRow) == "mouse-back" && current(afterburnerRow) == "mouse-middle",
            "giving the guns the afterburner's button swaps them");
    save_controls(".keyboard-mouse.bmp", "save changed mouse buttons preview");
    show_row(0);
    save_controls(".keyboard-edited.bmp", "save changed bindings preview");
    click(410, row_y(8), left);
    click(617, 409, left);
    require(controlsWindow->get_capture_row() == -1 && current(8).empty(), "Remove clears a binding");
    save_controls(".keyboard-removed.bmp", "save removed binding preview");
    click(1024 - 40, 620, left);
    require(current(1) == "up" && current(2) == "down" && current(8) == "page-down" &&
            current(gunsRow) == "mouse-left" && current(missileRow) == "mouse-left|mouse-right",
            "Restore defaults brings back the original bindings");
    require(changed_key_bindings(*controlsWindow).empty(), "restored defaults save no keys");
    click(410, row_y(1), left);
    press("ñ");
    release("ñ");
    click(410, row_y(find_row("roll-left")), left);
    press(slint::SharedString(slint::platform::key_codes::Control));
    press(",");
    release(",");
    release(slint::SharedString(slint::platform::key_codes::Control));
    click(430, show_row(gunsRow), left);
    click(520, 330, slint::PointerEventButton::Right);
    show_row(0);
    if (changed_key_bindings(*controlsWindow) !=
        "pitch-down=%c3%b1\nroll-left=ctrl+%2c,insert\nmouse-guns=mouse-right\nmouse-afterburner=mouse-left\n")
        std::fprintf(stderr, "Kept:\n%s", changed_key_bindings(*controlsWindow).c_str());
    require(changed_key_bindings(*controlsWindow) ==
                "pitch-down=%c3%b1\nroll-left=ctrl+%2c,insert\nmouse-guns=mouse-right\nmouse-afterburner=mouse-left\n",
            "only changed rows are kept, percent-encoded");
    {
        // A second window gets its own adapter; keep rendering the first.
        auto *const controlsView = platform->view;
        auto reopened = create_controls_window(window, changed_key_bindings(*controlsWindow) +
                                                   "lock-target=l,mouse-forward\nchoose-message=x\n"
                                                   "next-target=\nmouse-steer=k\n");
        const auto reloaded = changed_key_bindings(*reopened);
        if (reloaded != "pitch-down=%c3%b1\nroll-left=ctrl+%2c,insert\nmouse-guns=mouse-right\n"
                        "mouse-afterburner=mouse-left\nnext-target=\nlock-target=l,mouse-forward\n")
            std::fprintf(stderr, "Reloaded keys:\n%s", reloaded.c_str());
        require(reloaded == "pitch-down=%c3%b1\nroll-left=ctrl+%2c,insert\nmouse-guns=mouse-right\n"
                            "mouse-afterburner=mouse-left\nnext-target=\nlock-target=l,mouse-forward\n",
                "saved keys and mouse inputs come back, and fixed rows stay");
        reopened->hide();
        platform->view = controlsView;
    }
    {
        auto *const controlsView = platform->view;
        auto reopened = create_controls_window(window, "mouse-afterburner=k\nfire-guns=ctrl+mouse-left\n");
        require(changed_key_bindings(*reopened).empty(),
                "a key for a mouse button row, or a modifier with the mouse, is not kept");
        reopened->hide();
        platform->view = controlsView;
    }
    controlsWindow->invoke_bindings_reset();
    require(slint::select_bundled_translation("fr"), "French capture dialog");
    click(410, row_y(1), left);
    save_controls(".keyboard-capture-fr.bmp", "save French binding capture preview");
    press(slint::SharedString(slint::platform::key_codes::Escape));
    require(slint::select_bundled_translation("en") && controlsWindow->get_capture_row() == -1,
            "the capture dialog closes in French too");

    // SWC: its own gamepad bindings and keys, shown as they are.
    controlsWindow->set_game(1);
    controlsWindow->set_joystick_mode(2);
    controlsWindow->set_joystick_axes(0);
    const auto pad = [&controlsWindow](PadControl control) {
        return std::string(controlsWindow->invoke_pad_binding(control));
    };
    require(pad(PadControl::DpadLeft) == "in-flight computer" && pad(PadControl::DpadUp).empty() &&
            pad(PadControl::DpadRight).empty() && pad(PadControl::LeftStickClick).empty() &&
            pad(PadControl::Back) == "cycle views" && pad(PadControl::B) == "fire missile" &&
            pad(PadControl::X) == "afterburner" && pad(PadControl::Y) == "cycle targets" &&
            pad(PadControl::RightStickClick) == "autopilot", "SWC's WCAT gamepad bindings");
    controlsWindow->set_joystick_mode(0);
    require(pad(PadControl::DpadLeft).empty() && pad(PadControl::B) == "hold to roll and change speed",
            "SWC's original joystick bindings");
    controlsWindow->set_joystick_mode(2);
    require(controlsWindow->get_swc_rows()->row_count() == 62, "SWC lists all its keys");
    controlsWindow->set_page(0);
    save_controls(".swc-controls.bmp", "save SWC game controller preview");
    controlsWindow->set_page(1);
    save_keyboard(".swc-keyboard");
    // SWC's keys and mouse can be rebound like WC1's.
    const auto swc_current = [&controlsWindow, &inputs](int row) {
        std::string text;
        const auto table = controlsWindow->get_swc_bindings();
        const auto bindings = table && row >= 0 && static_cast<size_t>(row) < table->row_count()
            ? table->row_data(row).value_or(nullptr) : nullptr;
        for (size_t slot = 0; bindings && slot < bindings->row_count(); ++slot)
            text += (slot ? "/" : "") + inputs(bindings->row_data(slot).value_or(InputBinding {}));
        return text;
    };
    const auto swc_row = [&controlsWindow](const char *id) {
        const auto swcRows = controlsWindow->get_swc_rows();
        for (size_t index = 0; index < swcRows->row_count(); ++index) {
            const auto row = swcRows->row_data(index);
            if (row && row->id == id)
                return static_cast<int>(index);
        }
        return -1;
    };
    const auto swcRows = controlsWindow->get_swc_rows();
    const auto swcPause = swcRows->row_data(swc_row("swc-pause")).value_or(InputRow {});
    require(swcPause.bindings && swcPause.bindings->row_count() == 2 &&
                inputs(binding_at(swcPause, 1)) == "escape" && binding_at(swcPause, 1).fixed &&
                !binding_at(swcPause, 0).fixed,
            "SWC pauses on its menu key, and on Escape as WC1 does");
    click(410, show_row(1), left);
    require(controlsWindow->get_capture_row() == 1, "SWC's keys can be changed");
    press("w");
    release("w");
    require(swc_current(1) == "w/i", "a key replaces an SWC binding");
    click(430, show_row(swc_row("swc-mouse-guns")), left);
    click(520, 330, slint::PointerEventButton::Middle);
    require(swc_current(swc_row("swc-mouse-guns")) == "mouse-middle" &&
            swc_current(swc_row("swc-mouse-missile")) == "mouse-middle|mouse-right",
            "SWC's mouse buttons can be changed too");
    click(430, show_row(swc_row("full-screen")), left);
    press("f");
    release("f");
    show_row(0);
    require(swc_current(swc_row("full-screen")) == "f" && current(find_row("full-screen")) == "f",
            "both games share the full-screen shortcut");
    require(changed_key_bindings(*controlsWindow) ==
                "full-screen=f\nswc-pitch-down=w,i\nswc-mouse-guns=mouse-middle\n",
            "SWC's changes are kept with WC1's, and full screen once");
    controlsWindow->set_page(0);
    controlsWindow->set_page(1);
    save_controls(".swc-keyboard-edited.bmp", "save changed SWC bindings preview");
    click(1024 - 40, 620, left);
    require(swc_current(1) == "up/i" && changed_key_bindings(*controlsWindow).empty(),
            "Restore defaults resets SWC, with the shared full screen");
    controlsWindow->set_game(0);
    controlsWindow->hide();
    window->hide();
    fs::remove_all(fixture);
    std::puts("PASS: controls window: WC1 and SWC bindings, joystick selectors, rebindable keys and mouse for both.");
    std::puts("PASS: WC1/SM1/SM2 and SWC credits buttons, independent detection, missing data, edition switching and click routing.");
    std::puts("PASS: live English/Spanish/French switching, redraw, errors, installation labels and language settings.");
}
