/* Exercise the real launcher language handler on an offscreen Slint window. */
#include "../src/sdl/slint/slint_static.h"
#include "../src/sdl/slint/launcher.cpp"
#include <slint-platform.h>
#include <SDL.h>
#include <array>
#include <cmath>
#include <cstdlib>
#include <fstream>

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
    auto checkLaunchButtons = [&](const fs::path &directory, int expected) {
        window->set_game_directory(slint::SharedString(path_to_utf8(directory)));
        refresh_window(*window, installations);
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
    saved.campaign = 2;
    saved.transferPilot = 1;
    loaded.campaign = 0;
    require(save_launcher_config(fixture / "launcher.ini", saved), "save settings after SM2 selection");
    load_launcher_config(fixture / "launcher.ini", loaded);
    require(loaded.campaign == 0 && !loaded.transferPilot, "campaign and transfer choices are per launch");
    window->hide();
    fs::remove_all(fixture);
    std::puts("PASS: WC1/SM1/SM2 buttons, independent detection, missing data, edition switching and click routing.");
    std::puts("PASS: live English/Spanish/French switching, redraw, errors, installation labels and language settings.");
}
