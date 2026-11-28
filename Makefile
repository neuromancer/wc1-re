# Wing Commander (Kilrathi Saga, Win32): the reconstructed source.
#
# The original game's sources (src/*.c, src/*.cpp, src/*.asm and src/ix/)
# rebuild WC1.EXE with Microsoft Visual C++ 4.20 under wibo, byte-identical to
# the 1996 retail executable. With the SDL2 port (src/port/) they also build
# the native game.
#
#   make                build WC1.EXE and check it against the retail hash
#   make verify-hash    the same check, by name
#   make run            run the rebuilt WC1.EXE in DREAMM
#   make run-original   run the retail WC1.EXE in DREAMM
#   make modern         build the SDL2 port, out-modern/wc1-modern
#   make modern-gui     build the SDL2 port with its launcher
#   make modern-swc     build the Super Wing Commander demo viewer
#   make run-modern     run the SDL2 port in MODERN_RUN_DIR (data/full)
#   make site           build the source browser, out/site/html
#   make check-fuzz     build and run the flight fuzzer

UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# ---------------------------------------------------------------------------
# Original toolchain
# ---------------------------------------------------------------------------

WIBO = ./wibo
CC = $(WIBO) compilers/msvc420/bin/CL.EXE
# The MSVC rules expand $(CC) themselves. When the shell exports CC, as
# nix-shell does, make would hand this value to every recipe's environment,
# where CMake and cargo take it for the host compiler (issue #39).
unexport CC
LINK = $(WIBO) compilers/msvc420/bin/LINK.EXE
LIBRARIAN = $(WIBO) compilers/msvc420/bin/LIB.EXE

# The raster library and the ix sample mixer were hand-written assembly.
# UASM, a MASM-compatible assembler built from its submodule, reproduces them.
UASM_DIR = compilers/uasm
UASM = out/uasm/uasm
HOST_CC ?= cc

# MSVC takes Windows paths. The recipes pass /I paths through the shell, so
# those double their backslashes; the INCLUDE and LIB variables do not.
MSVC_INC = compilers\msvc420\include
MSVC_LIB = compilers\msvc420\lib

# These flags reproduce the retail executable:
#   /MTd   the static debug multithreaded CRT (LIBCMTD): the debug heap,
#          _CrtCheckMemory and the leak report are in the original.
#   /Oy    no frame pointer: game functions read their arguments at [esp+N].
#   no /Gf identical literals stay apart, like the two
#          "DIBsetWholePalette   SetEntries" at 0x0046b6e0 and 0x0046b71c.
#   /GX    only for pilot.cpp (see its rule).
CFLAGS_COMMON = \
	/nologo \
	/c \
	/MTd \
	/I include

# The game core is optimised. The ix library members are not (/Od): each
# saves EBX, ESI and EDI, spills to [EBP-n] temporaries and jumps to one
# shared epilogue. ixlog.cpp, linked as an object, takes the core's flags.
CFLAGS_CORE = $(CFLAGS_COMMON) /Og /Oi /Ot /Oy /Ob1 /Gs
CFLAGS_IX   = $(CFLAGS_COMMON) /Od /Oi

# No /ENTRY: without it LINK adds WinMainCRTStartup as an undefined symbol
# after reading every object, which places wincrt0.obj from LIBCMTD right
# after strncmp.obj, as in the original.
LINKFLAGS = /nologo /SUBSYSTEM:WINDOWS /ALIGN:4096

# The libraries in the original's search order: the import descriptors run
# KERNEL32, USER32, GDI32, ADVAPI32, WINMM, DSOUND, DDRAW, and the
# DirectSound and DirectDraw thunks follow the last ix member. The toolchain
# ships DDRAW.LIB and DSOUND.LIB.
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
# WC1.EXE
# ---------------------------------------------------------------------------

TARGET = WC1.EXE
OUT_DIR = out

# LINK lays out the objects in command-line order, so the game core is listed
# in address order. The ix units were five libraries, searched in turn. LINK
# pulls a member when one of its symbols is first needed: the streamer, DSP
# and sound members follow the core, while the LZO decoder and the file
# module, first needed by those members, come on the second pass, after the
# CRT. ixlog.cpp is linked as an object.
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

SRCS = $(SRCS_ORDERED_CORE) $(SRCS_IX_LOG) $(SRCS_IX_LIBRARIES)

objects_of = $(patsubst src/%,$(OUT_DIR)/%, \
               $(patsubst %.c,%.obj, $(patsubst %.cpp,%.obj, \
               $(patsubst %.asm,%.obj, $(1)))))

# The ix library members enter through their libraries, every other unit as
# an object.
LINK_OBJS = $(call objects_of,$(filter-out $(SRCS_IX_LIBRARIES),$(SRCS)))

IX_LZO_LIB = $(OUT_DIR)/ix/lzo.lib
IX_FILE_LIB = $(OUT_DIR)/ix/file.lib
IX_STREAMER_LIB = $(OUT_DIR)/ix/streamer.lib
IX_DSP_LIB = $(OUT_DIR)/ix/dsp.lib
IX_SOUND_LIB = $(OUT_DIR)/ix/sound.lib

IX_LIBS = $(IX_LZO_LIB) $(IX_FILE_LIB) $(IX_STREAMER_LIB) $(IX_DSP_LIB) \
          $(IX_SOUND_LIB)

REFERENCE_HEADERS = include/wc1.h include/wc1base.h include/wcdata.h \
	include/globals.h include/main.h include/winmain.h include/wc1funcs.h \
	include/wc1extern.h

# Each unit declares its functions in include/funcs/. CL 4.20's output depends
# on how many headers a unit reads, so the units read them joined, in link
# order, as include/wc1funcs.h; the ix wrappers and two gr.c decoders come last.
GAME_FUNC_HEADERS = \
	$(foreach unit,$(notdir $(basename $(SRCS_ORDERED_CORE))), \
		$(wildcard include/funcs/$(unit).h)) \
	$(addprefix include/funcs/,ix_system.h ix_sound.h ix_streamer.h gr_decoders.h)

# LINK writes the current time into the header; the build stamps the
# original's link time (1996-09-24 16:33:00 UTC) instead.
ORIGINAL_TIMESTAMP = 0x32480D3C
ORIGINAL_SHA256 = d517169e57f1d6254682c03dec5c254b5101ccd787d191f639b0cac1e8a616c4

ifeq ($(UNAME_S),Linux)
WIBO_PRESET = release64-clang
WIBO_BIN = wibo-src/build/release64-clang/wibo
else
WIBO_PRESET = release-macos
WIBO_BIN = wibo-src/build/release/wibo
endif

# wibo needs this compatible MSVC runtime beside CL.EXE and LINK.EXE. Keep a
# cached copy outside the submodule, then place it in the toolchain on demand.
MSVCRT40_URL = https://raw.githubusercontent.com/neuromancer/my-teacher-is-an-alien-re/3d1bfe60522ae05b86bbd2252fd01c8d0a11c3df/3rdparty/msvcrt40.dll
MSVCRT40_SHA256 = ab55a2de2b6faf3daacd3e69473d385ceaead8033f7c79beb6bbf802f230f030
MSVCRT_SOURCE = 3rdparty/msvcrt40.dll
MSVCRT_DLL = compilers/msvc420/bin/msvcrt40.dll

all: $(TARGET)

verify-hash: $(TARGET)

# The first rule links when an input changed; the second always checks the
# hash, so even an up-to-date WC1.EXE that was changed is rejected.
$(TARGET):: $(LINK_OBJS) $(IX_LIBS) Makefile | $(MSVCRT_DLL)
	env LIB='$(GAME_LIBPATH)' $(LINK) $(LINKFLAGS) $(LINK_OBJS) $(GAME_LIBS) /OUT:$@
	@python3 bin/stampPeTimestamp.py $@ $(ORIGINAL_TIMESTAMP)

$(TARGET)::
	@printf '%s  %s\n' '$(ORIGINAL_SHA256)' '$(TARGET)' | shasum -a 256 -c -

include/wc1funcs.h: $(GAME_FUNC_HEADERS)
	@{ printf '%s\n' '/**' ' * @file' ' * @ingroup headers' \
		' * @brief Prototypes of every reconstructed function, in link order.' \
		' *' \
		' * Generated by make from include/funcs/, one header per unit: edit' \
		' * those. The units must read them as this one file to rebuild WC1.EXE.' \
		' */' '#ifndef GAME_FUNCS_H' '#define GAME_FUNCS_H' ''; \
	  for header in $^; do cat $$header; echo; done; \
	  echo '#endif /* GAME_FUNCS_H */'; } > $@

$(WIBO):
	cd wibo-src && cmake --preset $(WIBO_PRESET) && cmake --build --preset $(WIBO_PRESET)
	ln -sf $(WIBO_BIN) $@

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

$(IX_LZO_LIB): $(call objects_of,$(SRCS_IX_LZO))
$(IX_FILE_LIB): $(call objects_of,$(SRCS_IX_FILE))
$(IX_STREAMER_LIB): $(call objects_of,$(SRCS_IX_STREAMER))
$(IX_DSP_LIB): $(call objects_of,$(SRCS_IX_DSP))
$(IX_SOUND_LIB): $(call objects_of,$(SRCS_IX_SOUND))

$(IX_LIBS): | $(WIBO) $(MSVCRT_DLL)
	@rm -f $@
	$(LIBRARIAN) /nologo /OUT:$@ $^

# CL prints the name of each file it compiles; its output is shown only when
# it fails.
#
# sound.c calls IxSample and IxSound member functions (the ECX calls at
# 0x42B4A0 and 0x42B680), so it was compiled as C++ under its .c name.
$(OUT_DIR)/sound.obj: src/sound.c $(REFERENCE_HEADERS) Makefile | $(WIBO) $(MSVCRT_DLL)
	@mkdir -p $(dir $@)
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS_CORE) /TP $< /Fo$@ \
		> $(@:.obj=.stdout) || (cat $(@:.obj=.stdout) >&2; exit 1)

$(OUT_DIR)/%.obj: src/%.c $(REFERENCE_HEADERS) Makefile | $(WIBO) $(MSVCRT_DLL)
	@mkdir -p $(dir $@)
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS_CORE) $< /Fo$@ \
		> $(@:.obj=.stdout) || (cat $(@:.obj=.stdout) >&2; exit 1)

# The ix library members are unoptimised; the logger is not. GNU make 3.81
# takes the first pattern rule that fits, so this one stays above the generic
# .cpp rule.
$(OUT_DIR)/ix/ixlog.obj: CFLAGS_IX = $(CFLAGS_CORE)

$(OUT_DIR)/ix/%.obj: src/ix/%.cpp src/ix/ix.h Makefile | $(WIBO) $(MSVCRT_DLL)
	@mkdir -p $(dir $@)
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS_IX) /I src\\ix $< /Fo$@ \
		> $(@:.obj=.stdout) || (cat $(@:.obj=.stdout) >&2; exit 1)

# pilot.cpp's debug-console new expression at 0x425B00 has a C++ unwind map,
# which /GX reproduces; debug.cpp has none.
$(OUT_DIR)/pilot.obj: CFLAGS_CORE_CPP_EXTRA = /GX

$(OUT_DIR)/%.obj: src/%.cpp $(REFERENCE_HEADERS) Makefile | $(WIBO) $(MSVCRT_DLL)
	@mkdir -p $(dir $@)
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS_CORE) $(CFLAGS_CORE_CPP_EXTRA) $< /Fo$@ \
		> $(@:.obj=.stdout) || (cat $(@:.obj=.stdout) >&2; exit 1)

$(OUT_DIR)/%.obj: src/%.asm | $(UASM)
	@mkdir -p $(dir $@)
	@$(UASM) -q -coff -Fo$@ -Fw$(@:.obj=.err) $< > $(@:.obj=.stdout) || \
		(cat $(@:.obj=.stdout) >&2; exit 1)

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
# Running WC1.EXE in DREAMM
# ---------------------------------------------------------------------------
#
# DREAMM runs the rebuilt and the retail executable alike. The 1996 binary
# drives DirectDraw and DirectSound directly and expects Windows 95, which
# DREAMM emulates; Wine's own versions of those APIs change its behaviour.
# DREAMM is downloaded into .dreamm/ on first use.

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

# The retail executable is not vendored; run-original copies it in from
# ORIGINAL_SRC.
ORIGINAL_EXE = $(RUN_DIR)/WC1.ORI.EXE
ORIGINAL_SRC ?= ../releases/win32/WC1.EXE

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

# The rebuilt WC1.EXE under DREAMM's debugger.
debug: $(TARGET) run-check | $(DREAMM_BIN)
	cp -f $(TARGET) "$(RUN_DIR)/WC1.EXE"
	cd "$(RUN_DIR)" && $(DREAMM) $(DREAMM_MOUNTS) $(DREAMM_PROPS) -debug \
		-launch WC1.EXE > debug.log

$(ORIGINAL_EXE):
	@test -f "$(ORIGINAL_SRC)" || \
		(echo "Error: original not found at $(ORIGINAL_SRC). Set ORIGINAL_SRC=/path/to/WC1.EXE" >&2 && exit 1)
	@mkdir -p $(dir $@)
	@cp -f "$(ORIGINAL_SRC)" $@
	@echo "Staged original -> $@"

# ---------------------------------------------------------------------------
# SDL2 port
# ---------------------------------------------------------------------------

MODERN_EXE_SUFFIX :=
MODERN_PLATFORM_LIBS :=
ifeq ($(UNAME_S),Darwin)
MODERN_PLATFORM_LIBS += -framework CoreText -framework CoreGraphics -framework CoreFoundation
endif
ifneq (,$(filter MINGW% MSYS% CYGWIN%,$(UNAME_S)))
MODERN_EXE_SUFFIX := .exe
MODERN_PLATFORM_LIBS := -limm32
endif

# The SDL2 port builds with the host compiler, in its own tree.
MODERN_OUT_DIR = out-modern
MODERN_TARGET = $(MODERN_OUT_DIR)/wc1-modern$(MODERN_EXE_SUFFIX)
MODERN_GUI_TARGET = $(MODERN_OUT_DIR)/wc1-modern-gui$(MODERN_EXE_SUFFIX)
SWC_MUSIC_SOUNDFONT = $(MODERN_OUT_DIR)/swc-music.sf2
MODERN_CMAKE ?= cmake
MODERN_GUI_SOURCE_DIR = src/port/launcher
MODERN_GUI_BUILD_DIR = $(MODERN_OUT_DIR)/slint-gui
MODERN_GUI_BUILD_TYPE ?= Release
MODERN_GUI_CMAKE_ARGS ?=
MODERN_GUI_SRCS = $(wildcard $(MODERN_GUI_SOURCE_DIR)/* translations/*/LC_MESSAGES/*.po \
	third_party/sc-controller/images/*.svg third_party/sc-controller/images/*/*.svg \
	third_party/steam-deck/*.svg)
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
MODERN_CPPFLAGS = -DSDL_PORT=1 -DSDL_MAIN_HANDLED -Iinclude -Isrc/port \
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
ifeq ($(MODERN_FUZZING),1)
override MODERN_SANITIZER_FLAGS = -g -O1 -fsanitize=fuzzer-no-link,address,undefined \
	-fsanitize-recover=undefined -fno-omit-frame-pointer
else ifeq ($(MODERN_RELEASE),1)
override MODERN_SANITIZER_FLAGS =
else
override MODERN_SANITIZER_FLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer
endif
MODERN_DEAD_STRIP_DARWIN = -Wl,-dead_strip -Wl,-no_fixup_chains
MODERN_DEAD_STRIP_OTHER = -Wl,--gc-sections
MODERN_DEAD_STRIP_FLAGS = $(if $(filter Darwin,$(UNAME_S)),\
	$(MODERN_DEAD_STRIP_DARWIN),$(MODERN_DEAD_STRIP_OTHER))

# The original game's units, built with SDL_PORT. globals.c defines the
# raster tables that WC1.EXE takes from raster.asm.
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
	src/port/platform/compat.c \
	src/port/input/input.c \
	src/port/game/resources.c \
	src/port/game/campaign.c \
	src/port/game/data_patches.c \
	src/port/game/translation.c \
	src/port/audio/speech.c \
	src/port/swc/swc_mission.c \
	src/port/swc/cmf.c \
	src/port/platform/registry.c \
	src/port/platform/thread.c \
	src/port/platform/timer.c \
	src/port/graphics/video_state.c
MODERN_GAME_HOST_SRCS = \
	src/port/game/fmtowns.c \
	src/port/game/towns_text.c \
	src/port/game/autosave.c \
	src/port/swc/swc_save.c \
	src/port/swc/swc_flight.c \
	src/port/swc/swc_raster.c \
	src/port/swc/swc_cockpit.c \
	src/port/swc/swc_nav.c \
	src/port/swc/swc_sound.c \
	src/port/swc/swc_music.c \
	src/port/swc/swc_text.c \
	src/port/swc/swc_rooms.c \
	src/port/swc/swc_options.c \
	src/port/swc/swc_movie.c \
	src/port/swc/speech.c \
	src/port/swc/sound.c \
	src/port/swc/movie.c \
	src/port/swc/music.c \
	src/port/swc/shape.c \
	src/port/swc/rsrc.c \
	src/port/audio/audio.c \
	src/port/game/dos_intro.c \
	src/port/input/events.c \
	src/port/graphics/gl_renderer.c \
	src/port/graphics/hires_hud.c \
	src/port/graphics/interpolation.c \
	src/port/input/joystick.c \
	src/port/input/keymap.c \
	src/port/audio/mt32.c \
	src/port/audio/music.c \
	src/port/graphics/video.c
MODERN_GAME_HOST_CXX_SRCS = \
	src/port/audio/originfx.cpp \
	src/port/audio/wave.cpp
MODERN_YMFM_SRCS = \
	third_party/ymfm/ymfm_adpcm.cpp \
	third_party/ymfm/ymfm_opl.cpp \
	third_party/ymfm/ymfm_pcm.cpp
# libmt32emu emulates the MT-32 for DOS music (see third_party/munt/README.wc1).
MODERN_MUNT_SRCS = \
	third_party/munt/mt32emu/Analog.cpp \
	third_party/munt/mt32emu/BReverbModel.cpp \
	third_party/munt/mt32emu/Display.cpp \
	third_party/munt/mt32emu/File.cpp \
	third_party/munt/mt32emu/FileStream.cpp \
	third_party/munt/mt32emu/LA32FloatWaveGenerator.cpp \
	third_party/munt/mt32emu/LA32Ramp.cpp \
	third_party/munt/mt32emu/LA32WaveGenerator.cpp \
	third_party/munt/mt32emu/MidiStreamParser.cpp \
	third_party/munt/mt32emu/Part.cpp \
	third_party/munt/mt32emu/Partial.cpp \
	third_party/munt/mt32emu/PartialManager.cpp \
	third_party/munt/mt32emu/Poly.cpp \
	third_party/munt/mt32emu/ROMInfo.cpp \
	third_party/munt/mt32emu/SampleRateConverter.cpp \
	third_party/munt/mt32emu/Synth.cpp \
	third_party/munt/mt32emu/Tables.cpp \
	third_party/munt/mt32emu/TVA.cpp \
	third_party/munt/mt32emu/TVF.cpp \
	third_party/munt/mt32emu/TVP.cpp \
	third_party/munt/mt32emu/c_interface/c_interface.cpp \
	third_party/munt/mt32emu/sha1/sha1.cpp \
	third_party/munt/mt32emu/srchelper/InternalResampler.cpp \
	third_party/munt/mt32emu/srchelper/srctools/src/FIRResampler.cpp \
	third_party/munt/mt32emu/srchelper/srctools/src/IIR2xResampler.cpp \
	third_party/munt/mt32emu/srchelper/srctools/src/LinearResampler.cpp \
	third_party/munt/mt32emu/srchelper/srctools/src/ResamplerModel.cpp \
	third_party/munt/mt32emu/srchelper/srctools/src/SincResampler.cpp
MODERN_LAUNCHER_SRC = src/port/platform/launcher.c

MODERN_GAMEPLAY_OBJS = \
	$(patsubst src/%.c,$(MODERN_OUT_DIR)/obj/%.o,$(MODERN_GAMEPLAY_SRCS)) \
	$(patsubst src/%.cpp,$(MODERN_OUT_DIR)/obj/%.o,$(MODERN_GAMEPLAY_CXX_SRCS)) \
	$(patsubst src/%.c,$(MODERN_OUT_DIR)/obj/%.o,$(MODERN_GAMEPLAY_CXX_C_SRCS))
MODERN_IX_OBJS = \
	$(patsubst src/%.cpp,$(MODERN_OUT_DIR)/obj/%.o,$(MODERN_IX_SRCS))
MODERN_BASE_HOST_OBJS = $(patsubst src/%.c,$(MODERN_OUT_DIR)/obj/%.o,$(MODERN_BASE_HOST_SRCS))
MODERN_YMFM_OBJS = $(patsubst %.cpp,$(MODERN_OUT_DIR)/obj/%.o,$(MODERN_YMFM_SRCS))
MODERN_MUNT_OBJS = $(patsubst %.cpp,$(MODERN_OUT_DIR)/obj/%.o,$(MODERN_MUNT_SRCS))
MODERN_GAME_HOST_OBJS = \
	$(MODERN_OUT_DIR)/obj/towns_font.o \
	$(MODERN_OUT_DIR)/obj/hires_cockpit_font.o \
	$(patsubst src/%.c,$(MODERN_OUT_DIR)/obj/%.o,$(MODERN_GAME_HOST_SRCS)) \
	$(patsubst src/%.cpp,$(MODERN_OUT_DIR)/obj/%.o,$(MODERN_GAME_HOST_CXX_SRCS)) \
	$(MODERN_YMFM_OBJS) $(MODERN_MUNT_OBJS)
MODERN_LAUNCHER_OBJ = $(patsubst src/%.c,$(MODERN_OUT_DIR)/obj/%.o,$(MODERN_LAUNCHER_SRC))
MODERN_STATIC_GUI_LAUNCHER_OBJ = $(MODERN_OUT_DIR)/obj/port/platform/launcher_gui.o
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
SWC_SRCS = src/port/swc/cmf.c src/port/swc/shape.c src/port/swc/rsrc.c
SWC_OBJS = $(patsubst src/%.c,$(MODERN_OUT_DIR)/obj/%.o,$(SWC_SRCS))
SWC_HOST_OBJ = $(MODERN_OUT_DIR)/obj/port/swc/swc_demo.o
MODERN_DEPFILES += $(SWC_OBJS:.o=.d) $(SWC_HOST_OBJ:.o=.d)

MODERN_TRANSLATIONS = $(addprefix $(MODERN_OUT_DIR)/translations/,es.dat ja.dat fr.dat)
MODERN_VOICES = $(MODERN_OUT_DIR)/voices/segacd.dat

$(MODERN_VOICES): translations/bin/compileVoices.py translations/bin/compileTranslations.py voices/segacd.po $(wildcard translations/es/*.po)
	python3 translations/bin/compileVoices.py --output $@

$(MODERN_OUT_DIR)/translations/es.dat: translations/bin/compileTranslations.py $(wildcard translations/es/*.po) $(wildcard translations/es/*.json)
	python3 translations/bin/compileTranslations.py --source translations/es --output $@

$(MODERN_OUT_DIR)/translations/fr.dat: translations/bin/compileTranslations.py $(wildcard translations/fr/*.po) $(wildcard translations/fr/*.json)
	python3 translations/bin/compileTranslations.py --source translations/fr --encoding cp437 --output $@

$(MODERN_OUT_DIR)/translations/ja.dat: translations/bin/compileTranslations.py translations/ja/ui.po
	python3 translations/bin/compileTranslations.py --source translations/ja --encoding cp932 --output $@

.PHONY: update-translations
update-translations:
	python3 translations/bin/updateUiTranslations.py

.PHONY: update-launcher-translations
SLINT_TR_EXTRACTOR ?= slint-tr-extractor
update-launcher-translations:
	@mkdir -p $(MODERN_OUT_DIR)
	$(SLINT_TR_EXTRACTOR) --package-name wc1-re -o $(MODERN_OUT_DIR)/launcher.pot $(MODERN_GUI_SOURCE_DIR)/launcher.slint
	msgmerge --update --backup=none --no-fuzzy-matching --no-wrap translations/es/LC_MESSAGES/launcher.po $(MODERN_OUT_DIR)/launcher.pot
	msgmerge --update --backup=none --no-fuzzy-matching --no-wrap translations/fr/LC_MESSAGES/launcher.po $(MODERN_OUT_DIR)/launcher.pot

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
$(MODERN_GUI_TARGET): $(MODERN_GUI_GAME_OBJS) $(MODERN_GUI_SRCS) Makefile | $(SWC_MUSIC_SOUNDFONT) $(MODERN_TRANSLATIONS) $(MODERN_VOICES)
	@command -v $(MODERN_CMAKE) >/dev/null 2>&1 || { \
		echo "CMake 3.21 or newer is required for modern-gui." >&2; \
		exit 1; \
	}
	@command -v cargo >/dev/null 2>&1 || { \
		echo "Rust 1.92 or newer is required to build Slint." >&2; \
		exit 1; \
	}
	@if test -f $(MODERN_GUI_BUILD_DIR)/CMakeCache.txt && \
	   ! grep -qx 'CMAKE_HOME_DIRECTORY:INTERNAL=$(abspath $(MODERN_GUI_SOURCE_DIR))' \
		$(MODERN_GUI_BUILD_DIR)/CMakeCache.txt; then \
		echo "The launcher's sources moved; configuring its build again."; \
		sed -n 's/^Rust_TOOLCHAIN:STRING=\(..*\)$$/-DRust_TOOLCHAIN=\1/p' \
			$(MODERN_GUI_BUILD_DIR)/CMakeCache.txt \
			> $(MODERN_GUI_BUILD_DIR)/moved-cmake-args; \
		rm -f $(MODERN_GUI_BUILD_DIR)/CMakeCache.txt; \
	fi
	$(MODERN_CMAKE) -S $(MODERN_GUI_SOURCE_DIR) \
		-B $(MODERN_GUI_BUILD_DIR) \
		-DCMAKE_CXX_COMPILER="$(MODERN_CXX)" \
		-DCMAKE_BUILD_TYPE=$(MODERN_GUI_BUILD_TYPE) \
		-DWC1_GAME_OBJECTS="$(subst $(MODERN_SPACE),;,$(MODERN_GUI_CMAKE_OBJECTS))" \
		-DWC1_GAME_SANITIZERS=$(if $(MODERN_SANITIZER_FLAGS),ON,OFF) \
		-DWC1_GUI_OUTPUT_DIRECTORY="$(abspath $(MODERN_OUT_DIR))" \
		$$(cat $(MODERN_GUI_BUILD_DIR)/moved-cmake-args 2>/dev/null) \
		$(MODERN_GUI_CMAKE_ARGS)
	@rm -f $(MODERN_GUI_BUILD_DIR)/moved-cmake-args
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

$(MODERN_OUT_DIR)/towns_font.c: third_party/misaki/misaki.bin translations/bin/compileTownsFont.py
	@mkdir -p $(dir $@)
	python3 translations/bin/compileTownsFont.py $< $@

$(MODERN_OUT_DIR)/obj/towns_font.o: $(MODERN_OUT_DIR)/towns_font.c | modern-check-deps
	@mkdir -p $(dir $@)
	$(MODERN_CC) $(MODERN_CFLAGS) $(MODERN_SANITIZER_FLAGS) -c $< -o $@

# The hi-res HUD's double-size cockpit font (CC BY 4.0, see its README.md).
$(MODERN_OUT_DIR)/hires_cockpit_font.c: third_party/double-resolution-cockpit-font/cockpit-font-2x.bin bin/compileHiResFont.py
	@mkdir -p $(dir $@)
	python3 bin/compileHiResFont.py $< $@ --height 12 \
		--symbol abHiResCockpitFont --size-symbol nHiResCockpitFontSize

$(MODERN_OUT_DIR)/obj/hires_cockpit_font.o: $(MODERN_OUT_DIR)/hires_cockpit_font.c | modern-check-deps
	@mkdir -p $(dir $@)
	$(MODERN_CC) $(MODERN_CFLAGS) $(MODERN_SANITIZER_FLAGS) -c $< -o $@

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

$(MODERN_OUT_DIR)/obj/port/audio/originfx.o: src/port/audio/originfx.cpp | modern-check-deps
	@mkdir -p $(dir $@)
	$(MODERN_CXX) $(MODERN_CPPFLAGS) -Ithird_party/ymfm \
		$(MODERN_CXXFLAGS) -std=c++14 \
		$(MODERN_SECTION_FLAGS) $(MODERN_SANITIZER_FLAGS) \
		$(MODERN_DEPFLAGS) -c $< -o $@

# Upstream's CMake build defines these for every platform the port builds on.
# Its DAC emulation shifts negative samples left on purpose.
$(MODERN_MUNT_OBJS): $(MODERN_OUT_DIR)/obj/third_party/munt/%.o: third_party/munt/%.cpp | modern-check-deps
	@mkdir -p $(dir $@)
	$(MODERN_CXX) -DMT32EMU_WITH_INTERNAL_RESAMPLER -DMT32EMU_WITH_SNPRINTF \
		-DMT32EMU_WITH_EXP2F $(MODERN_CXXFLAGS) \
		$(MODERN_SECTION_FLAGS) $(MODERN_SANITIZER_FLAGS) -fno-sanitize=shift-base \
		$(MODERN_DEPFLAGS) -c $< -o $@

$(MODERN_OUT_DIR)/obj/port/audio/mt32.o: MODERN_CPPFLAGS += -Ithird_party/munt

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

# Keep the original wave player intact while its native IxSound objects enter
# the SDL positional-audio bridge.
$(MODERN_OUT_DIR)/obj/sound.o: MODERN_CPPFLAGS += \
	-Dix_system_new_sound=SdlNewWaveSound
$(MODERN_OUT_DIR)/obj/sound.o: Makefile

$(MODERN_TARGET): \
		$(MODERN_LAUNCHER_OBJ) \
		$(MODERN_BASE_HOST_OBJS) \
		$(MODERN_GAME_HOST_OBJS) \
		$(MODERN_GAMEPLAY_OBJS) \
		$(MODERN_IX_OBJS) | $(SWC_MUSIC_SOUNDFONT) $(MODERN_TRANSLATIONS) $(MODERN_VOICES)
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

# ---------------------------------------------------------------------------
# Source browser
# ---------------------------------------------------------------------------

# Static, cross-referenced browser of the Win32 build's sources, with the SDL
# port's code stripped, as deployed to GitHub Pages by .github/workflows/
# pages.yml. Needs clang, doxygen, unifdef and pygmentize; writes out/site/html.
site:
	@sh site/build.sh $(SRCS)

# ---------------------------------------------------------------------------
# Fuzzing
# ---------------------------------------------------------------------------

# libFuzzer flight fuzzers (fuzz/): flight-fuzzer flies WC1, SM1, SM2 and the
# training simulator from a Kilrathi Saga installation, swc-flight-fuzzer
# Super Wing Commander. FUZZ_ARGS passes options to fuzz/runFuzzer.py, such as
# FUZZ_ARGS="--case wc1:1:0 -- -max_total_time=600"; libFuzzer flags follow --.
FUZZ_CC ?= clang
FUZZ_CXX ?= clang++
FUZZ_OUT_DIR ?= out-modern/fuzz

# Compile every game object with coverage, separately from normal SDL builds.
.PHONY: fuzz-flight fuzz-swc-flight check-fuzz check-fuzz-sm1 check-fuzz-sm2 check-fuzz-swc
fuzz-flight:
	$(MAKE) MODERN_OUT_DIR="$(FUZZ_OUT_DIR)" MODERN_CC="$(FUZZ_CC)" \
		MODERN_CXX="$(FUZZ_CXX)" MODERN_FUZZING=1 "$(FUZZ_OUT_DIR)/flight-fuzzer"

fuzz-swc-flight:
	$(MAKE) MODERN_OUT_DIR="$(FUZZ_OUT_DIR)" MODERN_CC="$(FUZZ_CC)" \
		MODERN_CXX="$(FUZZ_CXX)" MODERN_FUZZING=1 "$(FUZZ_OUT_DIR)/swc-flight-fuzzer"

check-fuzz: fuzz-flight
	python3 fuzz/runFuzzer.py "$(FUZZ_OUT_DIR)/flight-fuzzer" "$(if $(GAME_DATA),$(GAME_DATA),$(MODERN_RUN_DIR))" $(FUZZ_ARGS)

check-fuzz-sm1: fuzz-flight
	python3 fuzz/runFuzzer.py "$(FUZZ_OUT_DIR)/flight-fuzzer" "$(if $(SM1_DATA),$(SM1_DATA),$(MODERN_RUN_DIR))" --campaign sm1 $(FUZZ_ARGS)

check-fuzz-sm2: fuzz-flight
	python3 fuzz/runFuzzer.py "$(FUZZ_OUT_DIR)/flight-fuzzer" "$(if $(SM2_DATA),$(SM2_DATA),$(MODERN_RUN_DIR))" --campaign sm2 $(FUZZ_ARGS)

check-fuzz-swc: fuzz-swc-flight
	python3 fuzz/runFuzzer.py "$(FUZZ_OUT_DIR)/swc-flight-fuzzer" "$(if $(SWC_DATA),$(SWC_DATA),$(SWC_DATA_DIR))" $(FUZZ_ARGS)

ifeq ($(MODERN_FUZZING),1)
FUZZ_OBJS = $(addprefix $(MODERN_OUT_DIR)/obj/fuzz/,flight.o wc1Flight.o swcFlight.o)

$(FUZZ_OBJS): $(MODERN_OUT_DIR)/obj/fuzz/%.o: fuzz/%.c fuzz/flight.h $(REFERENCE_HEADERS) include/wc1sdl.h | modern-check-sdl
	@mkdir -p $(dir $@)
	$(MODERN_CC) $(MODERN_CPPFLAGS) $(MODERN_CFLAGS) $(MODERN_SANITIZER_FLAGS) -c $< -o $@

# The SWC fuzzer includes swc_flight.c to reach its per-tick flight step.
$(MODERN_OUT_DIR)/obj/fuzz/swcFlight.o: src/port/swc/swc_flight.c src/port/swc/swc.h

$(MODERN_OUT_DIR)/flight-fuzzer: $(addprefix $(MODERN_OUT_DIR)/obj/fuzz/,flight.o wc1Flight.o) \
		$(MODERN_BASE_HOST_OBJS) $(MODERN_GAME_HOST_OBJS) $(MODERN_GAMEPLAY_OBJS) $(MODERN_IX_OBJS)
	$(MODERN_CXX) $(MODERN_CXXFLAGS) $(MODERN_SANITIZER_FLAGS) $^ \
		$(MODERN_SDL_LIBS) $(MODERN_LZO_LIBS) $(MODERN_PLATFORM_LIBS) \
		$(MODERN_DEAD_STRIP_FLAGS) -fsanitize=fuzzer -o $@

$(MODERN_OUT_DIR)/swc-flight-fuzzer: $(addprefix $(MODERN_OUT_DIR)/obj/fuzz/,flight.o swcFlight.o) \
		$(MODERN_BASE_HOST_OBJS) $(filter-out $(MODERN_OUT_DIR)/obj/port/swc/swc_flight.o,$(MODERN_GAME_HOST_OBJS)) \
		$(MODERN_GAMEPLAY_OBJS) $(MODERN_IX_OBJS)
	$(MODERN_CXX) $(MODERN_CXXFLAGS) $(MODERN_SANITIZER_FLAGS) $^ \
		$(MODERN_SDL_LIBS) $(MODERN_LZO_LIBS) $(MODERN_PLATFORM_LIBS) \
		$(MODERN_DEAD_STRIP_FLAGS) -fsanitize=fuzzer -o $@

# Sanitizer flag changes must not leave fatal handlers in cached objects.
$(FUZZ_OBJS) $(MODERN_BASE_HOST_OBJS) $(MODERN_GAME_HOST_OBJS) \
		$(MODERN_GAMEPLAY_OBJS) $(MODERN_IX_OBJS): Makefile
endif

# ---------------------------------------------------------------------------
# Cleanup
# ---------------------------------------------------------------------------

clean:
	rm -rf $(OUT_DIR)/*.obj $(OUT_DIR)/*.stdout $(OUT_DIR)/*.err \
	       $(OUT_DIR)/ix $(TARGET)

clean-run:
	rm -f "$(RUN_DIR)/WC1.EXE" "$(RUN_DIR)/debug.log"

clean-dreamm:
	rm -rf $(DREAMM_DIR)

clean-modern:
	rm -rf $(MODERN_OUT_DIR)

.PHONY: all verify-hash \
	dreamm run run-check run-original debug \
	modern modern-gui modern-swc modern-check-sdl modern-check-deps \
	run-modern run-modern-gui run-modern-dos run-modern-mission run-modern-swc \
	site clean clean-run clean-dreamm clean-modern
