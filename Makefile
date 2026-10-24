# Wing Commander (Kilrathi Saga, Win32) source reconstruction Makefile.
#
# This build intentionally uses the original Microsoft Visual C++ 4.20
# toolchain under wibo.  Matching that compiler, its flags, and the linker
# input order is part of the recovery process: binary-comp checks the rebuilt
# executable against the original at the instruction and data-layout level.
#
# The toolchain identification is evidence-based; see docs/COMPILER.md for the
# full derivation (linker stamp 4.20, no Rich header, separate .idata,
# _amsg_exit instead of fast_error_exit, no __set_app_type, aw_*.c CRT names).
#
# Common entry points:
#   make                 # build WC1.EXE
#   make WC1.EXE         # build WC1.EXE
#   make run             # build and launch in DREAMM
#   make report          # per-function similarity report
#   make order           # compilation-unit boundary hints
#   make verify          # primary recovery verification checklist

# ---------------------------------------------------------------------------
# Original toolchain
# ---------------------------------------------------------------------------

WIBO = ./wibo
CC = $(WIBO) compilers/msvc420/bin/CL.EXE
LINK = $(WIBO) compilers/msvc420/bin/LINK.EXE
LIBRARIAN = $(WIBO) compilers/msvc420/bin/LIB.EXE

# The raster library was hand-written assembly (docs/COMPILER.md).  UASM, a
# MASM-compatible assembler built from the submodule, reproduces it.
UASM_DIR = compilers/uasm
UASM = out/uasm/uasm
HOST_CC ?= cc

# MSVC expects Windows-style include/library search paths.  The recipes pass
# these through the host shell to wibo, so command-line /I paths use doubled
# backslashes while env vars keep normal Windows separators.
MSVC_INC = compilers\msvc420\include
MSVC_LIB = compilers\msvc420\lib

# Keep these flags synchronized with the recovered binary.  Changing optimizer
# or codegen flags will usually invalidate binary-comp comparisons.
#
# Evidence for each choice (see docs/COMPILER.md):
#   /MTd   static DEBUG multithreaded CRT.  The binary links LIBCMTD: dbgheap.c,
#          dbgrpt.c, dbgdel.cpp, _CrtCheckMemory, "DAMAGE: after %hs block",
#          "Detected memory leaks!", plus tidtable.c/_mtinit for the MT variant.
#   /Oy    frame pointers omitted.  Almost every game function reads arguments
#          as [esp+N] with no `push ebp` prologue.
#   NO /Gf string pooling.  Two byte-identical "DIBsetWholePalette   SetEntries"
#          literals exist at 0x0046b6e0 and 0x0046b71c, so identical strings
#          were NOT merged.
#   /GX is off for the core by default.  The one proven exception is pilot.cpp:
#          its debug-console new expression at 0x425B00 has a C++ unwind map and
#          jumps to __CxxFrameHandler.  A target-specific flag below reproduces it.
CFLAGS_COMMON = \
	/nologo \
	/c \
	/MTd \
	/I include

# IMPORTANT: the two halves of this program were built with DIFFERENT optimizer
# settings.  This is not a guess -- it is visible in every function:
#
#   Game core = OPTIMIZED.  GetShiftKeyState (0x00403060) is four instructions
#   with no prologue; MinShort (0x0041D0C0) reads its arguments straight off ESP
#   with no frame pointer; RandomBelowOrEqual (0x00434D50) schedules `POP ESI`
#   between CDQ and IDIV and tail-duplicates its epilogue.
#
#   ix library = UNOPTIMIZED (/Od).  The library functions open with
#   `PUSH EBP / MOV EBP,ESP / PUSH EBX / PUSH ESI / PUSH EDI` -- saving all three
#   registers whether or not they are used -- spill intermediates to stack
#   temporaries such as [EBP-4], and jump to one shared
#   `POP EDI / POP ESI / POP EBX / LEAVE / RET` epilogue.
#
# The separately linked ixlog.cpp uses the core profile: its short branches,
# scheduled argument loads and POP EBP epilogue match that profile exactly.
# Keep the profiles separate; the logger override is next to the ix rule.
CFLAGS_CORE = $(CFLAGS_COMMON) /Og /Oi /Ot /Oy /Ob1 /Gs
CFLAGS_IX   = $(CFLAGS_COMMON) /Od /Oi

# Default for anything not covered by a more specific rule.
CFLAGS = $(CFLAGS_CORE)

# No /ENTRY.  Without it LINK adds WinMainCRTStartup as an undefined symbol
# after reading every command-line object, so wincrt0.obj is pulled from
# LIBCMTD between the CRT references of the objects and those of the library
# members.  The original has it exactly there, right after strncmp.obj; an
# explicit /ENTRY makes it the first CRT member instead.
LINKFLAGS = /nologo /SUBSYSTEM:WINDOWS /ALIGN:4096

# DDRAW.DLL and DSOUND.DLL are bound statically through the import table, so
# import libraries are required at link time.  The MSVC420 submodule already
# ships DDRAW.LIB/DSOUND.LIB and DDRAW.H/DSOUND.H, so no extra SDK is needed.
#
# The order is the original's (docs/ORDER.md):
#   - the import descriptors run KERNEL32, USER32, GDI32, ADVAPI32, WINMM,
#     DSOUND, DDRAW, the order in which the import libraries were searched;
#   - the DirectSound and DirectDraw call thunks follow the last ix member, so
#     dsound.lib and ddraw.lib follow the ix libraries;
#   - the LZO decoder and the file module follow the RtlUnwind thunk that
#     kernel32.lib supplies on the second search pass, in that order, so their
#     libraries follow kernel32.lib, decoder first, and precede the ix
#     libraries that reference them.
GAME_LIBPATH = $(MSVC_LIB)
GAME_LIBS = \
	kernel32.lib \
	user32.lib \
	gdi32.lib \
	advapi32.lib \
	winmm.lib \
	$(IX_LIBS) \
	dsound.lib \
	ddraw.lib

# ---------------------------------------------------------------------------
# Project inputs and generated outputs
# ---------------------------------------------------------------------------

TARGET = WC1.EXE
MAPFILE = WC1.map
OUT_DIR = out
LINKMAPFILE = $(OUT_DIR)/WC1.link.map

UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

MODERN_EXE_SUFFIX :=
MODERN_PLATFORM_LIBS :=
ifeq ($(UNAME_S),Darwin)
MODERN_PLATFORM_LIBS += -framework CoreText -framework CoreGraphics -framework CoreFoundation
endif
ifneq (,$(filter MINGW% MSYS% CYGWIN%,$(UNAME_S)))
MODERN_EXE_SUFFIX := .exe
MODERN_PLATFORM_LIBS := -limm32
endif

# The native build uses the host compiler and never contributes objects to the
# MSVC 4.20 reference executable.
MODERN_OUT_DIR = out-modern
MODERN_TARGET = $(MODERN_OUT_DIR)/wc1-modern$(MODERN_EXE_SUFFIX)
MODERN_GUI_TARGET = $(MODERN_OUT_DIR)/wc1-modern-gui$(MODERN_EXE_SUFFIX)
SWC_MUSIC_SOUNDFONT = $(MODERN_OUT_DIR)/swc-music.sf2
MODERN_CMAKE ?= cmake
MODERN_GUI_SOURCE_DIR = src/sdl/slint
MODERN_GUI_BUILD_DIR = $(MODERN_OUT_DIR)/slint-gui
MODERN_GUI_BUILD_TYPE ?= Release
MODERN_GUI_CMAKE_ARGS ?=
MODERN_GUI_SRCS = $(wildcard $(MODERN_GUI_SOURCE_DIR)/*)
MODERN_EMPTY :=
MODERN_SPACE := $(MODERN_EMPTY) $(MODERN_EMPTY)
MODERN_RUN_DIR ?= data/full
MODERN_ARGS ?=
SERIES ?= 1
MISSION ?= 0
NAV ?=
MISSION_FLAGS ?=

MODERN_CC ?= cc
MODERN_CXX ?= c++
MODERN_SDL2_CONFIG ?= sdl2-config
MODERN_SDL_CFLAGS = $(shell \
	$(MODERN_SDL2_CONFIG) --cflags 2>/dev/null || \
	pkg-config --cflags sdl2 2>/dev/null)
MODERN_SDL_LIBS = $(shell \
	$(MODERN_SDL2_CONFIG) --libs 2>/dev/null || \
	pkg-config --libs sdl2 2>/dev/null)
MODERN_LZO_CFLAGS = $(shell pkg-config --cflags lzo2 2>/dev/null)
MODERN_LZO_INCLUDEDIR = $(shell pkg-config --variable=includedir lzo2 2>/dev/null)
MODERN_LZO_LIBS = $(shell pkg-config --libs lzo2 2>/dev/null)
# SDL_MAIN_HANDLED keeps SDL's headers from renaming main() to SDL_main on
# Windows, which needs SDL2main.a to supply a WinMain wrapper.  The port has
# its own main and calls SDL_SetMainReady() itself, so the rename is only a
# way to fail the link.
MODERN_CPPFLAGS = -DSDL_PORT=1 -DSDL_MAIN_HANDLED -Iinclude \
	$(MODERN_SDL_CFLAGS) \
	$(MODERN_LZO_CFLAGS) $(addprefix -I,$(MODERN_LZO_INCLUDEDIR))
SWC_CPPFLAGS = -DSDL_PORT=1 -DSDL_MAIN_HANDLED -Iinclude \
	$(MODERN_SDL_CFLAGS)
MODERN_CFLAGS ?= -O2 -std=c11 -Wno-return-type -Wno-return-mismatch \
	-Wno-error=incompatible-pointer-types
MODERN_CXXFLAGS ?= -O2 -std=c++11
MODERN_DEPFLAGS = -MMD -MP
MODERN_SECTION_FLAGS = -ffunction-sections -fdata-sections
# Developer builds retain runtime diagnostics.  Tagged release jobs
# explicitly select the optimized, uninstrumented host build.
ifeq ($(MODERN_RELEASE),1)
override MODERN_SANITIZER_FLAGS =
else
override MODERN_SANITIZER_FLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer
endif
MODERN_DEAD_STRIP_DARWIN = -Wl,-dead_strip -Wl,-no_fixup_chains
MODERN_DEAD_STRIP_OTHER = -Wl,--gc-sections
MODERN_DEAD_STRIP_FLAGS = $(if $(filter Darwin,$(UNAME_S)),\
	$(MODERN_DEAD_STRIP_DARWIN),$(MODERN_DEAD_STRIP_OTHER))

# Where the retail executable lives.  `make data/full/WC1.ORI.EXE` copies it out
# of the sibling analysis tree so this repo never has to vendor the binary.
ORIGINAL_EXE = data/full/WC1.ORI.EXE
ORIGINAL_SRC ?= ../releases/win32/WC1.EXE

VERIFY_CONFIG = config/binary-comp.json
CODE_DIR = code-full
# Every FUN_*.disassembled.txt in code-full/ also acts as a function boundary
# for the next export, so exports left behind by another binary truncate our
# functions at their addresses and silently wreck `make report`.  Always
# regenerate the directory from scratch.
EXPORT_ASM_FLAGS ?= --clean --discover
# Optional verification dependency; install with pip as documented in README.md.
BINARY_COMP ?= binary-comp

VALUE_MIN_SIMILARITY ?= 80
STACK_LOCAL_VALUE_MIN_SIMILARITY ?= 90
STACK_LOCAL_VALUES_FLAGS ?= --no-offsets
VALUES_FLAGS ?=

# Address windows for the globals audit.  The developer/CRT boundary is at
# 0x00449100: the last ix assert anchor (sample.cpp) sits at 0x00449044 and the
# first CRT __FILE__ anchor (fclose.c) at 0x004492e0.  Data above the game's own
# statics belongs to the CRT and is not recovered game state.
GLOBALS_MISSING_MIN_ADDRESS = 0x00465000
GLOBALS_MISSING_MAX_ADDRESS = 0x004751ff

# Global definitions are migrating back to their original compilation units.
# binary-comp currently accepts one definition source, so generate a parser-only
# manifest from globals.c and the declaration block at the top of each owner.
GLOBALS_DISTRIBUTED_SOURCES = src/winmain.c src/winsys.c src/mono.c src/auto.c src/eject.c src/cmpgn.c \
	src/fl.c src/cutscene.c src/brains.c src/mission.c \
	src/objdata.c src/nav.c src/nearheap.c src/gameflow.c src/title.c src/spc.c src/cockpt.c src/geom.c src/version.c \
	src/barracks.c src/fade.c src/debug.cpp src/mathutil.c \
	src/disk.c src/ship.c src/init.c src/logic.c src/pilot.cpp src/trainsim.c src/main.c src/timer.c \
	src/sound.c src/launch.c src/damage.c src/view.c src/music.c src/screen.c src/ending.c src/input.c \
	src/comm.c src/video.c src/joycal.c src/dib.c src/game.c src/data.c src/eventmgr.c src/screens.c \
	src/killbrd.c src/gr.c \
	src/ix/ixlog.cpp src/ix/streamer.cpp src/ix/dsp.cpp src/ix/dsps.cpp src/ix/dspv.cpp \
	src/ix/system.cpp
GLOBALS_DEFINITION_SOURCES = src/globals.c $(GLOBALS_DISTRIBUTED_SOURCES)
GLOBALS_AUDIT_SOURCE = $(OUT_DIR)/globals-audit.c
GLOBALS_DATA_AUDIT_SOURCE = $(OUT_DIR)/globals-data-audit.c

# ---------------------------------------------------------------------------
# Host platform and DREAMM runtime
# ---------------------------------------------------------------------------
#
# DREAMM is the only supported way to run the rebuilt executable.  Wine is not
# used: the Kilrathi Saga port is a 1996 Win32 binary that drives DirectDraw and
# DirectSound directly and expects a real Windows 95 environment, which is what
# DREAMM emulates.  Wine's own reimplementation of those APIs changes exactly
# the behaviour this project is trying to observe.
#
# DREAMM is downloaded on demand into .dreamm/ so the repository does not need
# to vendor platform-specific runtime binaries.

DREAMM_DIR = .dreamm
DREAMM_VERSION = 4.0x47
DREAMM_BASE_URL = https://dreamm.aarongiles.com/releases/4.0x

# 8-bit, not 16: the DIB layer creates a DirectDraw palette and pushes entries
# into it (DIBcascade -> CreatePalette, DIBsetPalette/DIBramPalette ->
# SetEntries).  Those calls only succeed against a palettized primary surface,
# so the game expects an 8-bit display mode; in 16-bit it takes the
# DIBerror("DIBmakeDIB   CreatePalette") path.  Override with DREAMM_PROPS=.
DREAMM_PROPS ?= -prop winres=640x480x8

ifeq ($(UNAME_S),Darwin)
DREAMM_ARCHIVE = dreamm-$(DREAMM_VERSION)-macos.dmg
DREAMM_BIN = $(DREAMM_DIR)/DREAMM.app/Contents/MacOS/dreamm
else
ifeq ($(UNAME_M),aarch64)
DREAMM_ARCHIVE = dreamm-$(DREAMM_VERSION)-linux-arm64.tgz
else
DREAMM_ARCHIVE = dreamm-$(DREAMM_VERSION)-linux-x64.tgz
endif
DREAMM_BIN = $(DREAMM_DIR)/dreamm
endif
DREAMM = $(CURDIR)/$(DREAMM_BIN)
DREAMM_STAMP = $(DREAMM_DIR)/.$(DREAMM_ARCHIVE).stamp

# The game runs out of the installed data directory.  C: is mounted writable
# from data/full/hd because DREAMM otherwise discards every write to C:, and the
# game rewrites INSTALL.DAT and its save slots.
#
# The disc is mounted at D: when present: the binary really does look for it
# (LocateStreamsDirOnDisc, FindCdRomDriveByVolumeLabel, PromptInsertCorrectCd),
# and the streaming music lives there.  Point GAME_ISO at an image or a directory.
RUN_DIR = data/full
# Any disc image dropped in data/ or data/full/ is picked up automatically.
# The recipes cd into RUN_DIR, so the mount path is made relative to it.
GAME_ISO ?= $(firstword $(wildcard data/*.iso data/*.ISO data/full/*.iso data/full/*.ISO))
DREAMM_MOUNTS = -mount rw:C=hd \
                $(if $(GAME_ISO),-mount d=$(patsubst $(RUN_DIR)/%,%,$(patsubst data/%,../%,$(GAME_ISO))))

# ---------------------------------------------------------------------------
# Source order
# ---------------------------------------------------------------------------
#
# The object link order fixes addresses and therefore every binary comparison.
# MSVC emits functions in source order and the linker concatenates objects in
# command-line order, so address order == link order.
#
# The ix function order follows live assert __FILE__ anchors in the shipped
# debug build. INT3 fill establishes the streamer/sound object boundaries;
# the DSP initializer ownership remains provisional (see docs/ORDER.md):
#     streamer.cpp 0x00442750   thread.cpp 0x00443b10   dsp.cpp    0x00444910
#     streamfile.cpp 0x00444700 (between thread.cpp and dsp.cpp)
#     dsps.cpp     0x004451b5   mixer.cpp  0x00445f60   dspv.cpp   0x004467c5
#     system.cpp   0x00447200   sound.cpp  0x00447a20   sample.cpp 0x004486b0
# The anchors name three source trees, and each was a separate library:
#     D:\Rnd\Prj\ix\Src\Streamer\   streamer.cpp thread.cpp stream-file methods
#     D:\rnd\prj\ix\win95\dsp\      dsp.cpp dsps.cpp mixer.cpp dspv.cpp
#     D:\Rnd\prj\ix\src\sound\      system.cpp sound.cpp sample.cpp
# LINK pulls library members in the order their symbols are first needed, which
# gives the address order above only with these three libraries searched in
# turn.  The LZO decoder (0x004614C0) and the file module (0x00461630) came
# from two more libraries searched before them, each pulled only on the second
# pass, after the first round of CRT members; INT3 padding up to a 16-byte
# boundary separates the two objects.  ixlog.cpp is the one ix unit linked as
# an object: its CRT references precede the entry point's, those of every
# library member follow it.  See docs/ORDER.md.
#
# SRCS_ORDERED_CORE is NOT yet known: no source-file anchors exist below
# 0x00442750, so game-core module boundaries have to be recovered incrementally
# with `make order`.  Add files here as boundaries are established; anything not
# listed is appended automatically so new work still builds.
# Game-core order below is by ADDRESS, which is the best available proxy for
# link order until `make order` proves the real boundaries.  Each file states
# its range and the evidence for it in its header comment.
SRCS_ORDERED_CORE = \
	src/winmain.c \
	src/winsys.c \
	src/mono.c \
	src/auto.c \
	src/eject.c \
	src/cmpgn.c \
	src/fl.c \
	src/cutscene.c \
	src/brains.c \
	src/mission.c \
	src/objdata.c \
	src/nav.c \
	src/nearheap.c \
	src/gameflow.c \
	src/title.c \
	src/joystick.c \
	src/spc.c \
	src/cockpt.c \
	src/version.c \
	src/geom.c \
	src/barracks.c \
	src/fade.c \
	src/debug.cpp \
	src/mathutil.c \
	src/disk.c \
	src/ship.c \
	src/init.c \
	src/logic.c \
	src/pilot.cpp \
	src/trainsim.c \
	src/main.c \
	src/targ.c \
	src/select.c \
	src/timer.c \
	src/pload.c \
	src/sound.c \
	src/launch.c \
	src/install.c \
	src/damage.c \
	src/view.c \
	src/music.c \
	src/screen.c \
	src/ending.c \
	src/input.c \
	src/comm.c \
	src/video.c \
	src/joycal.c \
	src/dib.c \
	src/game.c \
	src/smart.c \
	src/data.c \
	src/mathfp.c \
	src/strdos.c \
	src/eventmgr.c \
	src/screens.c \
	src/raster.asm \
	src/killbrd.c \
	src/gr.c

SRCS_IX_LOG = src/ix/ixlog.cpp

SRCS_IX_LZO = src/ix/lzo1x.cpp

SRCS_IX_FILE = src/ix/ixfile.cpp

SRCS_IX_STREAMER = \
	src/ix/streamer.cpp \
	src/ix/thread.cpp \
	src/ix/streamfile.cpp

SRCS_IX_DSP = \
	src/ix/dsp.cpp \
	src/ix/dsps.cpp \
	src/ix/mixer.cpp \
	src/ix/dspv.cpp \
	src/ix/dspmix.asm

SRCS_IX_SOUND = \
	src/ix/system.cpp \
	src/ix/sound.cpp \
	src/ix/sample.cpp

SRCS_IX_LIBRARIES = \
	$(SRCS_IX_LZO) \
	$(SRCS_IX_FILE) \
	$(SRCS_IX_STREAMER) \
	$(SRCS_IX_DSP) \
	$(SRCS_IX_SOUND)

SRCS_ORDERED_IX = $(SRCS_IX_LOG) $(SRCS_IX_LIBRARIES)

SRCS_ORDERED = $(SRCS_ORDERED_CORE) $(SRCS_ORDERED_IX)

# globals.c supplies the SDL raster tables; native storage is in raster.asm.
ALL_SRCS = $(filter-out src/globals.c,$(wildcard src/*.c)) \
           $(wildcard src/*.cpp) $(wildcard src/*.asm) \
           $(wildcard src/ix/*.cpp) $(wildcard src/ix/*.asm)
SRCS = $(filter $(ALL_SRCS), $(SRCS_ORDERED)) \
       $(filter-out $(SRCS_ORDERED), $(ALL_SRCS))

objects_of = $(patsubst src/%,$(OUT_DIR)/%, \
               $(patsubst %.c,%.obj, $(patsubst %.cpp,%.obj, \
               $(patsubst %.asm,%.obj, $(1)))))

# Everything except the ix library members is an object on the command line.
LINK_OBJS = $(call objects_of,$(filter-out $(SRCS_IX_LIBRARIES),$(SRCS)))

IX_LZO_LIB = $(OUT_DIR)/ix/lzo.lib
IX_FILE_LIB = $(OUT_DIR)/ix/file.lib
IX_STREAMER_LIB = $(OUT_DIR)/ix/streamer.lib
IX_DSP_LIB = $(OUT_DIR)/ix/dsp.lib
IX_SOUND_LIB = $(OUT_DIR)/ix/sound.lib

IX_LIBS = $(IX_LZO_LIB) $(IX_FILE_LIB) $(IX_STREAMER_LIB) $(IX_DSP_LIB) \
          $(IX_SOUND_LIB)

# Platform-neutral recovered units.  This list grows as each Win32 boundary is
# isolated; keeping it explicit makes native compile progress measurable.
MODERN_GAMEPLAY_SRCS = \
	src/fl.c \
	src/cutscene.c \
	src/mission.c \
	src/auto.c \
	src/barracks.c \
	src/brains.c \
	src/winsys.c \
	src/eject.c \
	src/cmpgn.c \
	src/cockpt.c \
	src/comm.c \
	src/damage.c \
	src/view.c \
	src/dib.c \
	src/disk.c \
	src/ending.c \
	src/eventmgr.c \
	src/fade.c \
	src/geom.c \
	src/gr.c \
	src/globals.c \
	src/init.c \
	src/input.c \
	src/joycal.c \
	src/launch.c \
	src/install.c \
	src/joystick.c \
	src/killbrd.c \
	src/logic.c \
	src/main.c \
	src/targ.c \
	src/select.c \
	src/timer.c \
	src/mathfp.c \
	src/mathutil.c \
	src/mono.c \
	src/music.c \
	src/nav.c \
	src/nearheap.c \
	src/objdata.c \
	src/gameflow.c \
	src/title.c \
	src/pload.c \
	src/screen.c \
	src/screens.c \
	src/ship.c \
	src/smart.c \
	src/data.c \
	src/spc.c \
	src/strdos.c \
	src/trainsim.c \
	src/game.c \
	src/version.c \
	src/video.c \
	src/winmain.c

MODERN_GAMEPLAY_CXX_SRCS = \
	src/debug.cpp \
	src/pilot.cpp

MODERN_GAMEPLAY_CXX_C_SRCS = \
	src/sound.c

MODERN_IX_SRCS = \
	src/ix/ixlog.cpp \
	src/ix/streamer.cpp \
	src/ix/thread.cpp \
	src/ix/streamfile.cpp \
	src/ix/dsp.cpp \
	src/ix/dsps.cpp \
	src/ix/mixer.cpp \
	src/ix/dspv.cpp \
	src/ix/system.cpp \
	src/ix/sound.cpp \
	src/ix/sample.cpp \
	src/ix/lzo1x.cpp \
	src/ix/ixfile.cpp

MODERN_BASE_HOST_SRCS = \
	src/sdl/compat.c \
	src/sdl/input.c \
	src/sdl/resources.c \
	src/sdl/swc_mission.c \
	src/swc/cmf.c \
	src/sdl/registry.c \
	src/sdl/thread.c \
	src/sdl/timer.c \
	src/sdl/video_state.c
MODERN_GAME_HOST_SRCS = \
	src/sdl/swc_flight.c \
	src/sdl/swc_raster.c \
	src/sdl/swc_cockpit.c \
	src/sdl/swc_nav.c \
	src/sdl/swc_sound.c \
	src/sdl/swc_music.c \
	src/sdl/swc_text.c \
	src/sdl/swc_rooms.c \
	src/sdl/swc_movie.c \
	src/swc/speech.c \
	src/swc/sound.c \
	src/swc/movie.c \
	src/swc/music.c \
	src/swc/shape.c \
	src/swc/rsrc.c \
	src/sdl/audio.c \
	src/sdl/dos_intro.c \
	src/sdl/events.c \
	src/sdl/gl_renderer.c \
	src/sdl/joystick.c \
	src/sdl/music.c \
	src/sdl/video.c
MODERN_GAME_HOST_CXX_SRCS = \
	src/sdl/originfx.cpp \
	src/sdl/wave.cpp
MODERN_YMFM_SRCS = \
	third_party/ymfm/ymfm_adpcm.cpp \
	third_party/ymfm/ymfm_opl.cpp \
	third_party/ymfm/ymfm_pcm.cpp
MODERN_LAUNCHER_SRC = src/sdl/launcher.c

MODERN_GAMEPLAY_OBJS = \
	$(patsubst src/%.c,$(MODERN_OUT_DIR)/obj/%.o,$(MODERN_GAMEPLAY_SRCS)) \
	$(patsubst src/%.cpp,$(MODERN_OUT_DIR)/obj/%.o,$(MODERN_GAMEPLAY_CXX_SRCS)) \
	$(patsubst src/%.c,$(MODERN_OUT_DIR)/obj/%.o,$(MODERN_GAMEPLAY_CXX_C_SRCS))
MODERN_IX_OBJS = \
	$(patsubst src/%.cpp,$(MODERN_OUT_DIR)/obj/%.o,$(MODERN_IX_SRCS))
MODERN_BASE_HOST_OBJS = $(patsubst src/%.c,$(MODERN_OUT_DIR)/obj/%.o,$(MODERN_BASE_HOST_SRCS))
MODERN_YMFM_OBJS = $(patsubst %.cpp,$(MODERN_OUT_DIR)/obj/%.o,$(MODERN_YMFM_SRCS))
MODERN_GAME_HOST_OBJS = \
	$(patsubst src/%.c,$(MODERN_OUT_DIR)/obj/%.o,$(MODERN_GAME_HOST_SRCS)) \
	$(patsubst src/%.cpp,$(MODERN_OUT_DIR)/obj/%.o,$(MODERN_GAME_HOST_CXX_SRCS)) \
	$(MODERN_YMFM_OBJS)
MODERN_LAUNCHER_OBJ = $(patsubst src/%.c,$(MODERN_OUT_DIR)/obj/%.o,$(MODERN_LAUNCHER_SRC))
MODERN_STATIC_GUI_LAUNCHER_OBJ = $(MODERN_OUT_DIR)/obj/sdl/launcher_gui.o
MODERN_GUI_GAME_OBJS = $(MODERN_STATIC_GUI_LAUNCHER_OBJ) \
	$(MODERN_BASE_HOST_OBJS) $(MODERN_GAME_HOST_OBJS) \
	$(MODERN_GAMEPLAY_OBJS) $(MODERN_IX_OBJS)
MODERN_GUI_CMAKE_OBJECTS = $(abspath $(MODERN_GUI_GAME_OBJS))
ifneq (,$(filter MINGW% MSYS% CYGWIN%,$(UNAME_S)))
MODERN_GUI_CMAKE_OBJECTS = $(shell cygpath -m $(abspath $(MODERN_GUI_GAME_OBJS)))
endif
MODERN_DEPFILES = \
	$(MODERN_GAMEPLAY_OBJS:.o=.d) \
	$(MODERN_IX_OBJS:.o=.d) \
	$(MODERN_BASE_HOST_OBJS:.o=.d) \
	$(MODERN_GAME_HOST_OBJS:.o=.d) \
	$(MODERN_LAUNCHER_OBJ:.o=.d) \
	$(MODERN_STATIC_GUI_LAUNCHER_OBJ:.o=.d)

# SWC Mac demo asset viewer.
SWC_TARGET = $(MODERN_OUT_DIR)/swc-demo$(MODERN_EXE_SUFFIX)
SWC_DATA_DIR ?= data/swc-demo/SuperWing DEMO
SWC_ARGS ?=
SWC_SRCS = src/swc/cmf.c src/swc/shape.c src/swc/rsrc.c
SWC_OBJS = $(patsubst src/%.c,$(MODERN_OUT_DIR)/obj/%.o,$(SWC_SRCS))
SWC_HOST_OBJ = $(MODERN_OUT_DIR)/obj/sdl/swc_demo.o
MODERN_DEPFILES += $(SWC_OBJS:.o=.d) $(SWC_HOST_OBJ:.o=.d)

# ---------------------------------------------------------------------------
# Build targets and tool bootstrap
# ---------------------------------------------------------------------------

all: $(TARGET)

build: $(TARGET)

# Naming parity with the sibling project, which has separate full/demo builds.
# WC1 shipped no demo, so there is only one target here and the `*-demo`
# counterparts intentionally do not exist.
build-full: $(TARGET)

# The native port is deliberately built in a separate output tree.  It must
# never supply objects to the assembly-comparison target above.
modern: $(MODERN_TARGET)

modern-swc: $(SWC_TARGET)

# No LZO, ix, Slint, or Win32 reference objects enter this bootstrap.
$(SWC_OBJS) $(SWC_HOST_OBJ): $(MODERN_OUT_DIR)/obj/%.o: src/%.c | modern-check-sdl
	@mkdir -p $(dir $@)
	$(MODERN_CC) $(SWC_CPPFLAGS) $(MODERN_CFLAGS) \
		$(MODERN_SANITIZER_FLAGS) $(MODERN_DEPFLAGS) -c $< -o $@

$(SWC_TARGET): $(SWC_OBJS) $(SWC_HOST_OBJ)
	$(MODERN_CC) $(MODERN_CFLAGS) $(MODERN_SANITIZER_FLAGS) \
		$^ $(MODERN_SDL_LIBS) -o $@

run-modern-swc: modern-swc
	@$(SWC_TARGET) "$(SWC_DATA_DIR)" $(SWC_ARGS)

# Optional native launcher; the ordinary modern target does not need Slint.
modern-gui: $(MODERN_GUI_TARGET)

$(SWC_MUSIC_SOUNDFONT): third_party/generaluser/GeneralUser-GS.sf2
	@mkdir -p $(dir $@)
	cp $< $@

# Slint enables full Rust LTO upstream; the small launcher does not need its
# substantial clean-build cost.
$(MODERN_GUI_TARGET): $(MODERN_GUI_GAME_OBJS) $(MODERN_GUI_SRCS) Makefile | $(SWC_MUSIC_SOUNDFONT)
	@command -v $(MODERN_CMAKE) >/dev/null 2>&1 || { \
		echo "CMake 3.21 or newer is required for modern-gui." >&2; \
		exit 1; \
	}
	@command -v cargo >/dev/null 2>&1 || { \
		echo "Rust 1.88 or newer is required to build Slint." >&2; \
		exit 1; \
	}
	$(MODERN_CMAKE) -S $(MODERN_GUI_SOURCE_DIR) \
		-B $(MODERN_GUI_BUILD_DIR) \
		-DCMAKE_CXX_COMPILER="$(MODERN_CXX)" \
		-DCMAKE_BUILD_TYPE=$(MODERN_GUI_BUILD_TYPE) \
		-DWC1_GAME_OBJECTS="$(subst $(MODERN_SPACE),;,$(MODERN_GUI_CMAKE_OBJECTS))" \
		-DWC1_GAME_SANITIZERS=$(if $(MODERN_SANITIZER_FLAGS),ON,OFF) \
		-DWC1_GUI_OUTPUT_DIRECTORY="$(abspath $(MODERN_OUT_DIR))" \
		$(MODERN_GUI_CMAKE_ARGS)
	CARGO_PROFILE_RELEASE_LTO=false \
		$(MODERN_CMAKE) --build $(MODERN_GUI_BUILD_DIR) \
		--config $(MODERN_GUI_BUILD_TYPE) --target wc1-modern-gui
	@test -s $@

modern-check-sdl:
	@if test -z "$(strip $(MODERN_SDL_CFLAGS))" || \
	   test -z "$(strip $(MODERN_SDL_LIBS))"; then \
		echo "SDL2 development files were not found." >&2; \
		echo "Install SDL2 development files." >&2; \
		exit 1; \
	fi

modern-check-deps: modern-check-sdl
	@if test -z "$(strip $(MODERN_LZO_LIBS))"; then \
		echo "LZO2 development files were not found." >&2; \
		echo "Install LZO2 development files." >&2; \
		exit 1; \
	fi

$(MODERN_STATIC_GUI_LAUNCHER_OBJ): $(MODERN_LAUNCHER_SRC) Makefile | modern-check-deps
	@mkdir -p $(dir $@)
	$(MODERN_CC) $(MODERN_CPPFLAGS) -DWC1_STATIC_GUI=1 $(MODERN_CFLAGS) \
		$(MODERN_SECTION_FLAGS) $(MODERN_SANITIZER_FLAGS) \
		$(MODERN_DEPFLAGS) -c $< -o $@

$(MODERN_OUT_DIR)/obj/%.o: src/%.c | modern-check-deps
	@mkdir -p $(dir $@)
	$(MODERN_CC) $(MODERN_CPPFLAGS) $(MODERN_CFLAGS) \
		$(MODERN_SECTION_FLAGS) $(MODERN_SANITIZER_FLAGS) \
		$(MODERN_DEPFLAGS) -c $< -o $@

$(MODERN_OUT_DIR)/obj/%.o: src/%.cpp | modern-check-deps
	@mkdir -p $(dir $@)
	$(MODERN_CXX) $(MODERN_CPPFLAGS) -Isrc/ix $(MODERN_CXXFLAGS) \
		$(MODERN_SECTION_FLAGS) $(MODERN_SANITIZER_FLAGS) \
		$(MODERN_DEPFLAGS) -c $< -o $@

$(MODERN_OUT_DIR)/obj/sdl/originfx.o: src/sdl/originfx.cpp | modern-check-deps
	@mkdir -p $(dir $@)
	$(MODERN_CXX) $(MODERN_CPPFLAGS) -Ithird_party/ymfm \
		$(MODERN_CXXFLAGS) -std=c++14 \
		$(MODERN_SECTION_FLAGS) $(MODERN_SANITIZER_FLAGS) \
		$(MODERN_DEPFLAGS) -c $< -o $@

$(MODERN_OUT_DIR)/obj/third_party/ymfm/%.o: third_party/ymfm/%.cpp | modern-check-deps
	@mkdir -p $(dir $@)
	$(MODERN_CXX) $(MODERN_CPPFLAGS) -Ithird_party/ymfm \
		$(MODERN_CXXFLAGS) -std=c++14 \
		$(MODERN_SECTION_FLAGS) $(MODERN_SANITIZER_FLAGS) \
		$(MODERN_DEPFLAGS) -c $< -o $@

$(MODERN_OUT_DIR)/obj/sound.o: src/sound.c | modern-check-deps
	@mkdir -p $(dir $@)
	$(MODERN_CXX) $(MODERN_CPPFLAGS) -Isrc/ix $(MODERN_CXXFLAGS) \
		$(MODERN_SECTION_FLAGS) $(MODERN_SANITIZER_FLAGS) \
		$(MODERN_DEPFLAGS) -x c++ -c $< -o $@

# Keep the recovered wave player intact while its native IxSound objects enter
# the SDL positional-audio bridge.
$(MODERN_OUT_DIR)/obj/sound.o: MODERN_CPPFLAGS += \
	-Dix_system_new_sound=SdlNewWaveSound
$(MODERN_OUT_DIR)/obj/sound.o: Makefile

$(MODERN_TARGET): \
		$(MODERN_LAUNCHER_OBJ) \
		$(MODERN_BASE_HOST_OBJS) \
		$(MODERN_GAME_HOST_OBJS) \
		$(MODERN_GAMEPLAY_OBJS) \
		$(MODERN_IX_OBJS) | $(SWC_MUSIC_SOUNDFONT)
	@mkdir -p $(dir $@)
	$(MODERN_CXX) $(MODERN_CXXFLAGS) $(MODERN_SANITIZER_FLAGS) \
		$^ $(MODERN_SDL_LIBS) $(MODERN_LZO_LIBS) \
		$(MODERN_PLATFORM_LIBS) \
		$(MODERN_DEAD_STRIP_FLAGS) -o $@

run-modern: modern
	@case "$(MODERN_RUN_DIR)" in \
		/*) modern_run_dir="$(MODERN_RUN_DIR)" ;; \
		*) modern_run_dir="$(CURDIR)/$(MODERN_RUN_DIR)" ;; \
	esac; \
	test -d "$$modern_run_dir" || { \
		echo "Modern run directory does not exist: $$modern_run_dir" >&2; \
		exit 1; \
	}; \
	cd "$$modern_run_dir" && "$(CURDIR)/$(MODERN_TARGET)" $(MODERN_ARGS)

run-modern-gui: modern-gui
	@case "$(MODERN_RUN_DIR)" in \
		/*) modern_run_dir="$(MODERN_RUN_DIR)" ;; \
		*) modern_run_dir="$(CURDIR)/$(MODERN_RUN_DIR)" ;; \
	esac; \
	test -d "$$modern_run_dir" || { \
		echo "Modern run directory does not exist: $$modern_run_dir" >&2; \
		exit 1; \
	}; \
	cd "$$modern_run_dir" && "$(CURDIR)/$(MODERN_GUI_TARGET)" --gui $(MODERN_ARGS)

# The SDL2 host recognizes the compressed resources in an installed DOS copy
# and plays its OriginFX music and synthesized effects through an embedded
# YM3812 emulator.
run-modern-dos: MODERN_RUN_DIR = data/dos
run-modern-dos: run-modern

# The original startup has a hidden direct-flight path selected by the ordered
# tokens "Origin sN mN l".  The modern build supplies the normal shared
# LaunchPlayerShip setup before entering flight so the cockpit is initialized.
# The option loader exposes one fewer token than it reads, so retain a sentinel.
run-modern-mission: MODERN_ARGS = Origin s$(SERIES) m$(MISSION) \
	$(if $(strip $(NAV)),as$(NAV)) l $(MISSION_FLAGS) ignored
run-modern-mission: run-modern

-include $(MODERN_DEPFILES)

ifeq ($(UNAME_S),Linux)
WIBO_PRESET = release64-clang
WIBO_BIN = wibo-src/build/release64-clang/wibo
else
WIBO_PRESET = release-macos
WIBO_BIN = wibo-src/build/release/wibo
endif

$(WIBO):
	cd wibo-src && cmake --preset $(WIBO_PRESET) && cmake --build --preset $(WIBO_PRESET)
	ln -sf $(WIBO_BIN) $@

# wibo needs this compatible MSVC runtime beside CL.EXE and LINK.EXE. Keep a
# cached copy outside the submodule, then place it in the toolchain on demand.
MSVCRT40_URL = https://raw.githubusercontent.com/neuromancer/my-teacher-is-an-alien-re/3d1bfe60522ae05b86bbd2252fd01c8d0a11c3df/3rdparty/msvcrt40.dll
MSVCRT40_SHA256 = ab55a2de2b6faf3daacd3e69473d385ceaead8033f7c79beb6bbf802f230f030
MSVCRT_SOURCE = 3rdparty/msvcrt40.dll
MSVCRT_DLL = compilers/msvc420/bin/msvcrt40.dll

$(MSVCRT_DLL): $(MSVCRT_SOURCE)
	cp -f $< $@

$(MSVCRT_SOURCE):
	@mkdir -p $(dir $@)
	@echo "Downloading wibo-compatible msvcrt40.dll..."
	@curl -fL --retry 3 -o "$@.tmp" "$(MSVCRT40_URL)"
	@printf '%s  %s\n' "$(MSVCRT40_SHA256)" "$@.tmp" | \
		shasum -a 256 -c - >/dev/null || \
		(rm -f "$@.tmp"; echo "Error: msvcrt40.dll checksum mismatch." >&2; exit 1)
	@mv "$@.tmp" "$@"

# LINK writes the current time into the header; stamp the original's link time
# (1996-09-24 16:33:00 UTC) so a matching build is byte-identical.
ORIGINAL_TIMESTAMP = 0x32480D3C

REFERENCE_HEADERS = include/wc1.h include/wcdata.h include/globals.h \
	include/wc1funcs.h include/wc1extern.h

$(TARGET): $(LINK_OBJS) $(IX_LIBS) Makefile bin/appendStaticSymbols.py | $(MSVCRT_DLL)
	env LIB='$(GAME_LIBPATH)' $(LINK) $(LINKFLAGS) /MAP:$(LINKMAPFILE) $(LINK_OBJS) $(GAME_LIBS) /OUT:$@
	@python3 bin/stampPeTimestamp.py $@ $(ORIGINAL_TIMESTAMP)
	@python3 bin/appendStaticSymbols.py --map $(LINKMAPFILE) \
		--rebuilt $@ --output $(MAPFILE) $(OUT_DIR)/winmain.obj $(OUT_DIR)/winsys.obj $(OUT_DIR)/mono.obj $(OUT_DIR)/auto.obj $(OUT_DIR)/eject.obj $(OUT_DIR)/cmpgn.obj $(OUT_DIR)/fl.obj $(OUT_DIR)/cutscene.obj $(OUT_DIR)/mission.obj $(OUT_DIR)/objdata.obj $(OUT_DIR)/nav.obj $(OUT_DIR)/nearheap.obj $(OUT_DIR)/gameflow.obj $(OUT_DIR)/title.obj $(OUT_DIR)/spc.obj $(OUT_DIR)/cockpt.obj $(OUT_DIR)/trainsim.obj $(OUT_DIR)/main.obj $(OUT_DIR)/sound.obj $(OUT_DIR)/launch.obj $(OUT_DIR)/damage.obj $(OUT_DIR)/view.obj $(OUT_DIR)/music.obj $(OUT_DIR)/ending.obj $(OUT_DIR)/comm.obj $(OUT_DIR)/joycal.obj $(OUT_DIR)/dib.obj $(OUT_DIR)/game.obj $(OUT_DIR)/eventmgr.obj $(OUT_DIR)/screens.obj $(OUT_DIR)/killbrd.obj $(OUT_DIR)/gr.obj

$(IX_LZO_LIB): $(call objects_of,$(SRCS_IX_LZO))
$(IX_FILE_LIB): $(call objects_of,$(SRCS_IX_FILE))
$(IX_STREAMER_LIB): $(call objects_of,$(SRCS_IX_STREAMER))
$(IX_DSP_LIB): $(call objects_of,$(SRCS_IX_DSP))
$(IX_SOUND_LIB): $(call objects_of,$(SRCS_IX_SOUND))

$(IX_LIBS): | $(WIBO) $(MSVCRT_DLL)
	@rm -f $@
	$(LIBRARIAN) /nologo /OUT:$@ $^

# The game-side wave module calls directly into IxSample and IxSound member
# functions.  Those ECX-based calls at 0x42B4A0 and 0x42B680 prove this one
# source unit was compiled as C++ while retaining its original .c filename.
$(OUT_DIR)/sound.obj $(OUT_DIR)/sound.asm: src/sound.c $(REFERENCE_HEADERS) Makefile | $(WIBO) $(MSVCRT_DLL)
	@mkdir -p $(dir $(OUT_DIR)/sound)
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS_CORE) /TP $< \
		/Fo$(OUT_DIR)/sound.obj \
		/Fa$(OUT_DIR)/sound.asm \
		> $(OUT_DIR)/sound.stdout

$(OUT_DIR)/%.obj $(OUT_DIR)/%.asm: src/%.c $(REFERENCE_HEADERS) Makefile | $(WIBO) $(MSVCRT_DLL)
	@mkdir -p $(dir $(OUT_DIR)/$*)
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS_CORE) $< \
		/Fo$(OUT_DIR)/$*.obj \
		/Fa$(OUT_DIR)/$*.asm \
		> $(OUT_DIR)/$*.stdout

# The standalone logger shares the core optimizer settings, unlike the libraries.
$(OUT_DIR)/ix/ixlog.obj $(OUT_DIR)/ix/ixlog.asm: CFLAGS_IX = $(CFLAGS_CORE)

# ix library members are built unoptimised; see the CFLAGS_IX note above.
$(OUT_DIR)/ix/%.obj $(OUT_DIR)/ix/%.asm: src/ix/%.cpp src/ix/ix.h Makefile | $(WIBO) $(MSVCRT_DLL)
	@mkdir -p $(OUT_DIR)/ix
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS_IX) /I src\\ix $< \
		/Fo$(OUT_DIR)/ix/$*.obj \
		/Fa$(OUT_DIR)/ix/$*.asm \
		> $(OUT_DIR)/ix/$*.stdout

# The console-owner wrapper at 0x425B00 contains compiler-generated C++
# construction cleanup.  /GX on this unit reproduces it exactly; debug.cpp has
# no such unwind records and stays on the core defaults.
$(OUT_DIR)/pilot.obj $(OUT_DIR)/pilot.asm: CFLAGS_CORE_CPP_EXTRA = /GX

$(OUT_DIR)/%.obj $(OUT_DIR)/%.asm: src/%.cpp $(REFERENCE_HEADERS) Makefile | $(WIBO) $(MSVCRT_DLL)
	@mkdir -p $(dir $(OUT_DIR)/$*)
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS_CORE) $(CFLAGS_CORE_CPP_EXTRA) $< \
		/Fo$(OUT_DIR)/$*.obj \
		/Fa$(OUT_DIR)/$*.asm \
		> $(OUT_DIR)/$*.stdout

# The raster library and ix sample mixer are assembled (docs/COMPILER.md).
$(OUT_DIR)/%.obj: src/%.asm | $(UASM)
	@mkdir -p $(dir $(OUT_DIR)/$*)
	@$(UASM) -q -coff -Fo$@ -Fw$(OUT_DIR)/$*.err $< > $(OUT_DIR)/$*.stdout || \
		(cat $(OUT_DIR)/$*.stdout >&2; exit 1)

# UASM's own makefile links with GNU ld's -Map option, which Apple's linker
# rejects, so build its objects there and link them here.
$(UASM):
	@mkdir -p $(dir $@)
	@test -f $(UASM_DIR)/gccmod.inc || \
		(echo "Error: $(UASM_DIR) is empty; run 'git submodule update --init'." >&2 && exit 1)
	@echo "Building UASM..."
	@$(MAKE) -C $(UASM_DIR) CC=$(HOST_CC) OUTD=$(abspath $(dir $@)) \
		$(addprefix $(abspath $(dir $@))/,main.o $(UASM_OBJS)) \
		> $(dir $@)build.log 2>&1 || (cat $(dir $@)build.log >&2; exit 1)
	@$(HOST_CC) $(addprefix $(dir $@),main.o $(UASM_OBJS)) -o $@

UASM_OBJS = $(shell sed -n 's|^\$$(OUTD)/\([A-Za-z0-9_]*\.o\).*|\1|p' \
	$(UASM_DIR)/gccmod.inc)

# ---------------------------------------------------------------------------
# Recovery reports and binary-comp audits
# ---------------------------------------------------------------------------

BC = --config $(VERIFY_CONFIG) --target full

CALLS_FLAGS ?=
GLOBAL_ACCESS_FLAGS ?=
ORDER_FLAGS ?=

# binary-comp command coverage (mirrors the sibling project):
#   calls         -> verify-calls
#   compare       -> compare-func FUNC=Name
#   data          -> globals-data, globals-data-verbose, missing-data
#   exe           -> compare, compare-functions
#   export-asm    -> export-asm
#   global-access -> verify-global-access
#   globals       -> verify-globals, verify-globals-code, globals-missing,
#                    audit-auto-complete-globals, audit-rebuilt-global-layout
#   order         -> order
#   report        -> report
#   seh           -> seh
#   triage        -> triage
#   values        -> verify-values, verify-values-stack-locals
#   vtables       -> verify-vtables

sort:
	@python3 bin/sortByAddress.py
	@python3 bin/expandOneLiners.py --check

# Rewrite `T f(void) { body; }` onto separate lines.  A body folded onto the
# signature hides how many statements the original has (AGENTS.md).
expand-one-liners:
	@python3 bin/expandOneLiners.py

# Verify every `Function start:` annotation names the function really at that
# address.  A wrong annotation makes `report` compare against the wrong original.
audit-addresses:
	@python3 bin/auditAddresses.py

# Compiler-generated tail calls and calling-convention adapters must come from
# ordinary C source, never from hand-written jump/call assembly.
audit-compiler-glue:
	@python3 bin/auditCompilerGlue.py

$(GLOBALS_AUDIT_SOURCE): Makefile bin/collectGlobalDefinitions.py include/wcdata.h $(GLOBALS_DEFINITION_SOURCES)
	@python3 bin/collectGlobalDefinitions.py \
		--output $@ \
		--constants-header include/wcdata.h \
		$(GLOBALS_DEFINITION_SOURCES)

# Literal-pool spans are comparison metadata, not source-level globals.
$(GLOBALS_DATA_AUDIT_SOURCE): $(GLOBALS_AUDIT_SOURCE) config/data-literals.h
	@cat $(GLOBALS_AUDIT_SOURCE) config/data-literals.h > $@

# --bytes adds each function's byte-level status (EXACT, BYTES n/m, SIZE +-n)
# to the mnemonic similarity.
REPORT_FLAGS ?= --bytes

report: $(TARGET) $(GLOBALS_AUDIT_SOURCE) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) report $(BC) $(REPORT_FLAGS) $(if $(FILTER),--filter $(FILTER))

# Compare a single rebuilt function against the original.  This is the inner
# loop while implementing:  make compare-func FUNC=MinShort
# Exports are named FUN_<ADDRESS>.disassembled.txt (see ExportToCompile.java),
# so resolve the file from the `Function:` header rather than the symbol name.
compare-func: $(TARGET) $(GLOBALS_AUDIT_SOURCE) | code-full $(ORIGINAL_EXE)
	@test -n "$(FUNC)" || (echo "usage: make compare-func FUNC=<FunctionName>" >&2 && exit 1)
	@f=$$(grep -lE "^Function: $(FUNC)$$" $(CODE_DIR)/*.disassembled.txt 2>/dev/null | head -1); \
	test -n "$$f" || (echo "no export for $(FUNC) in $(CODE_DIR)/ -- run 'make export-asm'" >&2 && exit 1); \
	$(BINARY_COMP) compare $(BC) --no-build $(FUNC) "$$f"

# Regenerate code-full/ straight from the original PE with Capstone.
export-asm: | $(ORIGINAL_EXE)
	@$(BINARY_COMP) export-asm $(BC) --map "$(abspath config/original-boundaries.map.txt)" $(EXPORT_ASM_FLAGS)

# Split near-miss functions into source-reachable vs allocator churn.
triage: $(TARGET) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) triage $(BC) $(if $(FILTER),--filter $(FILTER))

# Original-address compilation-unit ordering and boundary evidence.  This is the
# main tool for recovering game-core module boundaries, which are still unknown.
order: $(TARGET) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) order $(BC) --no-build $(if $(FILTER),--filter $(FILTER)) $(ORDER_FLAGS)

seh: $(TARGET) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) seh $(BC) --report $(if $(FILTER),--filter $(FILTER)) || true

globals-data: $(TARGET) $(GLOBALS_DATA_AUDIT_SOURCE) | $(ORIGINAL_EXE)
	@$(BINARY_COMP) data $(BC) --globals-source $(GLOBALS_DATA_AUDIT_SOURCE)

globals-data-verbose: $(TARGET) $(GLOBALS_DATA_AUDIT_SOURCE) | $(ORIGINAL_EXE)
	@$(BINARY_COMP) data $(BC) --globals-source $(GLOBALS_DATA_AUDIT_SOURCE) --verbose

missing-data: $(GLOBALS_DATA_AUDIT_SOURCE) | $(ORIGINAL_EXE)
	@$(BINARY_COMP) data $(BC) \
		--globals-source $(GLOBALS_DATA_AUDIT_SOURCE) \
		--find-missing \
		--min-address $(GLOBALS_MISSING_MIN_ADDRESS) \
		--max-address $(GLOBALS_MISSING_MAX_ADDRESS)

compare: $(TARGET) | $(ORIGINAL_EXE)
	@$(BINARY_COMP) exe $(BC)

compare-functions: $(TARGET) | $(ORIGINAL_EXE)
	@$(BINARY_COMP) exe $(BC) --functions

compare-full: compare

compare-full-functions: compare-functions

# ---------------------------------------------------------------------------
# Verification
# ---------------------------------------------------------------------------

verify:
	@$(MAKE) audit-compiler-glue
	@$(MAKE) verify-array-initializers
	@$(MAKE) verify-code-pointers
	@$(MAKE) verify-mixer-references
	@$(MAKE) verify-unit-data
	@$(MAKE) verify-globals
	@$(MAKE) verify-globals-data
	@$(MAKE) verify-globals-code
	@$(MAKE) audit-rebuilt-global-layout
	@$(MAKE) verify-calls
	@$(MAKE) verify-global-access
	@$(MAKE) verify-values
	@$(MAKE) verify-values-stack-locals
	@$(MAKE) verify-vtables

verify-array-initializers: $(TARGET) | $(ORIGINAL_EXE)
	@python3 bin/verifyArrayInitializers.py \
		--original "$(ORIGINAL_EXE)" --rebuilt "$(TARGET)" \
		--map "$(MAPFILE)" --binary-comp "$(BINARY_COMP)"

verify-code-pointers: $(TARGET) | code-full $(ORIGINAL_EXE)
	@python3 bin/verifyCodePointers.py $(BC)

verify-mixer-references: $(TARGET) | code-full $(ORIGINAL_EXE)
	@python3 bin/verifyMixerReferences.py $(BC)

verify-barracks-data: $(TARGET) out/barracks.obj | $(ORIGINAL_EXE)
	@python3 bin/verifyUnitData.py --unit barracks \
		--original "$(ORIGINAL_EXE)" --rebuilt "$(TARGET)" \
		--map "$(MAPFILE)" --objects-root "$(OUT_DIR)"

verify-unit-data: $(TARGET) | $(ORIGINAL_EXE)
	@python3 bin/verifyUnitData.py \
		--original "$(ORIGINAL_EXE)" --rebuilt "$(TARGET)" \
		--map "$(MAPFILE)" --objects-root "$(OUT_DIR)"

verify-globals: $(TARGET) $(GLOBALS_AUDIT_SOURCE) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) globals $(BC) --no-source-order --fail-on-issues --fail-on-warnings

# The source-level globals audit cannot serialize every nested or symbolic
# initializer.  Compare the linked bytes too, but keep the normal verify output
# to the actionable mismatches and summary.
verify-globals-data: $(TARGET) $(GLOBALS_DATA_AUDIT_SOURCE) | $(ORIGINAL_EXE)
	@audit_output=$$(mktemp "$${TMPDIR:-/tmp}/wc1-globals-data.XXXXXX"); \
		trap 'rm -f "$$audit_output"' 0 1 2 15; \
		status=0; \
		$(BINARY_COMP) data $(BC) --globals-source $(GLOBALS_DATA_AUDIT_SOURCE) \
			> "$$audit_output" || status=$$?; \
		awk '/ MISMATCH / || / not in rebuilt map/ || /^Summary:/' \
			"$$audit_output"; \
		exit $$status

verify-globals-code: $(TARGET) $(GLOBALS_AUDIT_SOURCE) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) globals $(BC) \
		--no-source-order \
		--include-code-globals \
		--fail-on-issues \
		--fail-on-warnings \
		--min-address $(GLOBALS_MISSING_MIN_ADDRESS) \
		--max-address $(GLOBALS_MISSING_MAX_ADDRESS)

globals-missing: verify-globals-code

# Compatibility aliases used by the sibling project's notes and scripts.
globals: globals-data

globals-verbose: globals-data-verbose

audit-auto-complete-globals: $(TARGET) $(GLOBALS_AUDIT_SOURCE) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) globals $(BC) --no-source-order --show-auto-complete-reviewed

# The generated manifest concatenates independent owners, so its order is not
# an original declaration order. Check source order in each real unit instead.
audit-rebuilt-global-layout: $(TARGET) $(GLOBALS_AUDIT_SOURCE) | code-full $(ORIGINAL_EXE)
	@set -e; for global_source in $(GLOBALS_DISTRIBUTED_SOURCES); do \
		$(BINARY_COMP) globals $(BC) \
			--globals-source $$global_source \
			--globals-h $$global_source \
			--check-rebuilt-layout \
			--no-address-warnings \
			--fail-on-issues; \
	done

# WC1's own code is C and has no vtables, but the DirectDraw/DirectSound COM
# interfaces are dispatched through vtables, so this stays in the checklist.
verify-vtables: $(TARGET) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) vtables $(BC)

verify-calls: | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) calls $(BC) $(CALLS_FLAGS) $(if $(FILTER),$(FILTER))

verify-global-access: $(GLOBALS_AUDIT_SOURCE) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) global-access $(BC) $(GLOBAL_ACCESS_FLAGS) $(if $(FILTER),$(FILTER))

verify-values: $(TARGET) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) values $(BC) --min-similarity $(VALUE_MIN_SIMILARITY) $(VALUES_FLAGS)

verify-values-stack-locals: $(TARGET) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) values $(BC) \
		--min-similarity $(STACK_LOCAL_VALUE_MIN_SIMILARITY) \
		--include-stack-locals \
		$(STACK_LOCAL_VALUES_FLAGS) \
		$(VALUES_FLAGS)

# ---------------------------------------------------------------------------
# Original binary and Ghidra export
# ---------------------------------------------------------------------------

# The retail executable is not vendored.  Copy it in from the analysis tree.
$(ORIGINAL_EXE):
	@test -f "$(ORIGINAL_SRC)" || \
		(echo "Error: original not found at $(ORIGINAL_SRC). Set ORIGINAL_SRC=/path/to/WC1.EXE" >&2 && exit 1)
	@mkdir -p $(dir $@)
	@cp -f "$(ORIGINAL_SRC)" $@
	@echo "Staged original -> $@"

# code-full/ holds the Ghidra-exported disassembly, strings and globals used by
# binary-comp.  Export it from the WC Ghidra project; never overwrite silently.
code-full:
	@echo "Error: code-full/ missing." >&2
	@echo "Export the Ghidra disassembly for WC1.EXE into code-full/ (see docs/EXPORT.md)." >&2
	@exit 1

# ---------------------------------------------------------------------------
# DREAMM launch targets
# ---------------------------------------------------------------------------

$(DREAMM_STAMP):
	@mkdir -p $(DREAMM_DIR)
	@echo "Downloading DREAMM $(DREAMM_VERSION)..."
	@curl -L -o $(DREAMM_DIR)/$(DREAMM_ARCHIVE) $(DREAMM_BASE_URL)/$(DREAMM_ARCHIVE)
ifeq ($(UNAME_S),Darwin)
	@rm -rf $(DREAMM_DIR)/mnt
	@hdiutil attach $(DREAMM_DIR)/$(DREAMM_ARCHIVE) -mountpoint $(DREAMM_DIR)/mnt -nobrowse -quiet
	@rm -rf $(DREAMM_DIR)/DREAMM.app
	@cp -R $(DREAMM_DIR)/mnt/DREAMM.app $(DREAMM_DIR)/
	@hdiutil detach $(DREAMM_DIR)/mnt -quiet
	@xattr -dr com.apple.quarantine $(DREAMM_DIR)/DREAMM.app
else
	@rm -rf $(DREAMM_DIR)/dreamm
	@tar xzf $(DREAMM_DIR)/$(DREAMM_ARCHIVE) -C $(DREAMM_DIR) --strip-components=1
endif
	@rm $(DREAMM_DIR)/$(DREAMM_ARCHIVE)
	@rm -f $(DREAMM_DIR)/.dreamm-*.stamp
	@touch "$(DREAMM_STAMP)"

$(DREAMM_BIN): $(DREAMM_STAMP)
	@test -x "$(DREAMM_BIN)" || \
		(echo "Error: DREAMM did not unpack to $(DREAMM_BIN)." >&2 && exit 1)

dreamm: $(DREAMM_BIN)

# The Kilrathi Saga disc carries a ready-to-run WC1 tree at /WC1 (WC1.EXE,
# GAMEDAT with the MODULE/CAMP/BRIEFING files, STREAMS, WINGCMDR.CFG).  Extract
# just that -- 142 MB of the disc's 634 -- rather than asking for a separate
# install.  bsdtar reads ISO9660 directly and ships with macOS and most Linuxes.
$(RUN_DIR)/GAMEDAT:
	@test -n "$(GAME_ISO)" || \
		(echo "Error: no disc image found. Put the Kilrathi Saga ISO in data/ or set GAME_ISO=." >&2 && exit 1)
	@command -v bsdtar >/dev/null 2>&1 || \
		(echo "Error: bsdtar not found; needed to read the ISO." >&2 && exit 1)
	@echo "Extracting WC1 from $(GAME_ISO)..."
	@mkdir -p "$(RUN_DIR)"
	@bsdtar -xf "$(GAME_ISO)" -C "$(RUN_DIR)" --strip-components=1 WC1

run-check: $(RUN_DIR)/GAMEDAT
	@mkdir -p "$(RUN_DIR)/hd"

run: $(TARGET) run-check | $(DREAMM_BIN)
	cp -f $(TARGET) "$(RUN_DIR)/WC1.EXE"
	cd "$(RUN_DIR)" && $(DREAMM) $(DREAMM_MOUNTS) $(DREAMM_PROPS) -launch WC1.EXE

run-original: run-check $(ORIGINAL_EXE) | $(DREAMM_BIN)
	cd "$(RUN_DIR)" && $(DREAMM) $(DREAMM_MOUNTS) $(DREAMM_PROPS) -launch WC1.ORI.EXE

# DREAMM's own debugger, the same target the sibling project uses.
debug: $(TARGET) run-check | $(DREAMM_BIN)
	cp -f $(TARGET) "$(RUN_DIR)/WC1.EXE"
	cd "$(RUN_DIR)" && $(DREAMM) $(DREAMM_MOUNTS) $(DREAMM_PROPS) -debug \
		-launch WC1.EXE > debug.log

# ---------------------------------------------------------------------------
# Cleanup and phony declarations
# ---------------------------------------------------------------------------

clean:
	rm -rf $(OUT_DIR)/*.obj $(OUT_DIR)/*.asm $(OUT_DIR)/*.stdout \
	       $(OUT_DIR)/ix $(TARGET) $(MAPFILE)

clean-run:
	rm -f "$(RUN_DIR)/WC1.EXE" "$(RUN_DIR)/debug.log"

clean-dreamm:
	rm -rf $(DREAMM_DIR)

clean-modern:
	rm -rf $(MODERN_OUT_DIR)

.PHONY: \
	modern-swc run-modern-swc \
	all \
	audit-addresses \
	audit-compiler-glue \
	audit-auto-complete-globals \
	audit-rebuilt-global-layout \
	build \
	build-full \
	clean-modern \
	compare-full \
	compare-full-functions \
	compare-func \
	debug \
	run-original \
	export-asm \
	globals \
	globals-verbose \
	triage \
	verify-vtables \
	clean \
	clean-run \
	clean-dreamm \
	dreamm \
	compare \
	compare-functions \
	globals-data \
	globals-data-verbose \
	globals-missing \
	missing-data \
	modern \
	modern-gui \
	modern-check-deps \
	modern-check-sdl \
	order \
	report \
	run \
	run-check \
	run-modern \
	run-modern-gui \
	run-modern-dos \
	run-modern-mission \
	seh \
	sort \
	verify \
	verify-array-initializers \
	verify-code-pointers \
	verify-mixer-references \
	verify-barracks-data \
	verify-unit-data \
	verify-calls \
	verify-global-access \
	verify-globals \
	verify-globals-data \
	verify-globals-code \
	verify-values \
	verify-values-stack-locals \
