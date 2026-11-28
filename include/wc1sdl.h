/* The SDL2 port's declarations, for the original game's files. Each port
 * file declares what it offers in a header beside it. */
#ifndef SDL_PORT_H
#define SDL_PORT_H

#ifndef SDL_PORT
#error "wc1sdl.h is only for the SDL_PORT build"
#endif

#include "platform/win32.h"
#include "platform/compat.h"
#include "input/input.h"
#include "audio/audio.h"
#include "audio/speech.h"
#include "input/events.h"
#include "game/original_globals.h"
#include "game/autosave.h"
#include "swc/swc_save.h"
#include "graphics/video.h"
#include "graphics/hires_hud.h"
#include "graphics/interpolation.h"
#include "graphics/video_state.h"
#include "input/joystick.h"
#include "swc/swc_options.h"
#include "input/keymap.h"
#include "game/resources.h"
#include "game/campaign.h"
#include "game/fmtowns.h"
#include "game/towns_text.h"
#include "swc/swc_mission.h"
#include "swc/swc_flight.h"
#include "swc/swc_raster.h"
#include "swc/swc_sound.h"
#include "swc/swc_cockpit.h"
#include "swc/swc_music.h"
#include "game/dos_intro.h"
#include "game/translation.h"
#include "game/data_patches.h"
#include "audio/mt32.h"
#include "audio/originfx.h"
#include "audio/music.h"
#include "audio/wave.h"

#endif /* SDL_PORT_H */
