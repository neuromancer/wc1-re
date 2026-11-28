#ifndef WC1_SDL_SLINT_LAUNCHER_CONFIG_H
#define WC1_SDL_SLINT_LAUNCHER_CONFIG_H

#include "launcher_api.h"

#include <filesystem>
#include <string>

namespace wc1_launcher {

std::filesystem::path launcher_config_path();
void load_launcher_config(const std::filesystem::path &path,
                          SdlLauncherOptions &options);
bool save_launcher_config(const std::filesystem::path &path,
                          const SdlLauncherOptions &options);
/* The settings' frame rate text, "20" or "18.5". */
std::string flight_fps_text(int halves);
/* Rewrites only the settings' frame rate, as the game keeps a rate set in
 * flight. */
bool save_launcher_flight_fps(const std::filesystem::path &path, int halves);

} // namespace wc1_launcher

#endif
