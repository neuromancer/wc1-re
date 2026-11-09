/* Exercise the real launcher language handler on an offscreen Slint window. */
#include "../src/sdl/slint/slint_static.h"
#include "../src/sdl/slint/launcher.cpp"
#include <slint-platform.h>
#include <SDL.h>
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
    window->set_status(towns.message);
    set_language(*window, installations, 1);
    require(window->get_status_text() == "POC de FM Towns: texto en inglés; audio experimental.",
            "Towns limitations translated in launcher");
    fs::remove(gameData / "FONTS.FNT");
    require(!validate_game_directory(path_to_utf8(fixture)).valid, "incomplete Towns data rejected");
    fs::remove(gameData / "COMMUNIC.JAT");
    require(validate_game_directory(path_to_utf8(fixture)).message != LauncherMessage::FmTowns,
            "raw packet flag alone does not identify Towns");
    fs::remove_all(fixture);
    std::puts("PASS: live English/Spanish switching, redraw, errors, installation labels, settings and SWC selection.");
}
