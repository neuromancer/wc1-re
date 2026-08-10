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
#   make progress        # reimplementation progress summary

# ---------------------------------------------------------------------------
# Original toolchain
# ---------------------------------------------------------------------------

WIBO = ./wibo
CC = $(WIBO) compilers/msvc420/bin/CL.EXE
LINK = $(WIBO) compilers/msvc420/bin/LINK.EXE

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
#   ix library = UNOPTIMIZED (/Od).  Every single ix function opens with
#   `PUSH EBP / MOV EBP,ESP / PUSH EBX / PUSH ESI / PUSH EDI` -- saving all three
#   registers whether or not they are used -- spills intermediates to stack
#   temporaries such as [EBP-4], and jumps to one shared
#   `POP EDI / POP ESI / POP EBX / LEAVE / RET` epilogue.
#
# Compiling ix with optimization on (or the core with it off) makes matching
# impossible, so keep these separate.
CFLAGS_CORE = $(CFLAGS_COMMON) /Og /Oi /Ot /Oy /Ob1 /Gs
CFLAGS_IX   = $(CFLAGS_COMMON) /Od /Oi

# Default for anything not covered by a more specific rule.
CFLAGS = $(CFLAGS_CORE)

LINKFLAGS = /nologo /SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup /ALIGN:4096

# DDRAW.DLL and DSOUND.DLL are bound statically through the import table, so
# import libraries are required at link time.  The MSVC420 submodule already
# ships DDRAW.LIB/DSOUND.LIB and DDRAW.H/DSOUND.H, so no extra SDK is needed.
GAME_LIBPATH = $(MSVC_LIB)
GAME_LIBS = \
	ddraw.lib \
	dsound.lib \
	winmm.lib \
	kernel32.lib \
	user32.lib \
	gdi32.lib \
	advapi32.lib

# ---------------------------------------------------------------------------
# Project inputs and generated outputs
# ---------------------------------------------------------------------------

TARGET = WC1.EXE
MAPFILE = WC1.map
OUT_DIR = out

# Where the retail executable lives.  `make data/full/WC1.ORI.EXE` copies it out
# of the sibling analysis tree so this repo never has to vendor the binary.
ORIGINAL_EXE = data/full/WC1.ORI.EXE
ORIGINAL_SRC ?= ../releases/win32/WC1.EXE

VERIFY_CONFIG = config/binary-comp.json
CODE_DIR = code-full
EXPORT_ASM_FLAGS ?=
BINARY_COMP ?= env PYTHONPATH=binary-comp/src python3 -m binary_comp.cli

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

UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

DREAMM_DIR = .dreamm
DREAMM_VERSION = 4.0x21
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
# and the streaming music lives there.  Point WC1_ISO at an image or a directory.
RUN_DIR = data/full
# Any disc image dropped in data/ or data/full/ is picked up automatically.
# The recipes cd into RUN_DIR, so the mount path is made relative to it.
WC1_ISO ?= $(firstword $(wildcard data/*.iso data/*.ISO data/full/*.iso data/full/*.ISO))
DREAMM_MOUNTS = -mount rw:C=hd \
                $(if $(WC1_ISO),-mount d=$(patsubst $(RUN_DIR)/%,%,$(patsubst data/%,../%,$(WC1_ISO))))

# ---------------------------------------------------------------------------
# Source order
# ---------------------------------------------------------------------------
#
# The object link order fixes addresses and therefore every binary comparison.
# MSVC emits functions in source order and the linker concatenates objects in
# command-line order, so address order == link order.
#
# SRCS_ORDERED_IX is EXACT, recovered from live assert __FILE__ anchors in the
# shipped debug build; each module occupies one contiguous address range:
#     streamer.cpp 0x00442750   thread.cpp 0x00443da6   dsp.cpp    0x00444910
#     dsps.cpp     0x004451b5   mixer.cpp  0x00445f60   dspv.cpp   0x004467c5
#     system.cpp   0x00447200   sound.cpp  0x00447cd8   sample.cpp 0x0044879c
# The ix library links AFTER the whole game core (it sits at higher addresses).
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
	src/sysinput.c \
	src/cdrom.c \
	src/mono.c \
	src/auto.c \
	src/shipai.c \
	src/nav.c \
	src/joystick.c \
	src/hud.c \
	src/geom.c \
	src/debug.cpp \
	src/mathutil.c \
	src/disk.c \
	src/damage.c \
	src/mission.c \
	src/pilot.cpp \
	src/system.c \
	src/main.c \
	src/hudmsg.c \
	src/pload.c \
	src/sound.c \
	src/music.c \
	src/screen.c \
	src/dib.c \
	src/text.c \
	src/smart.c \
	src/mathfp.c \
	src/strdos.c \
	src/eventmgr.c \
	src/screens.c \
	src/killbrd.c \
	src/gr.c \
	src/globals.c

SRCS_ORDERED_IX = \
	src/ix/ixlog.cpp \
	src/ix/streamer.cpp \
	src/ix/thread.cpp \
	src/ix/dsp.cpp \
	src/ix/dsps.cpp \
	src/ix/mixer.cpp \
	src/ix/dspv.cpp \
	src/ix/system.cpp \
	src/ix/sound.cpp \
	src/ix/sample.cpp

SRCS_ORDERED = $(SRCS_ORDERED_CORE) $(SRCS_ORDERED_IX)

ALL_SRCS = $(wildcard src/*.c) $(wildcard src/*.cpp) $(wildcard src/ix/*.cpp)
SRCS = $(filter $(ALL_SRCS), $(SRCS_ORDERED)) \
       $(filter-out $(SRCS_ORDERED), $(ALL_SRCS))

OBJS = $(patsubst src/%,$(OUT_DIR)/%, \
         $(patsubst %.c,%.obj, $(patsubst %.cpp,%.obj, $(SRCS))))

# ---------------------------------------------------------------------------
# Build targets and tool bootstrap
# ---------------------------------------------------------------------------

all: $(TARGET)

build: $(TARGET)

# Naming parity with the sibling project, which has separate full/demo builds.
# WC1 shipped no demo, so there is only one target here and the `*-demo`
# counterparts (build-demo, report-demo, seh-demo, compare-demo, run-demo,
# progress-demo) intentionally do not exist.
build-full: $(TARGET)

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

# The MSVC420 submodule ships an msvcrt40.dll that doesn't work with wibo;
# overwrite it with the vendored copy in 3rdparty/ before invoking CL/LINK.
MSVCRT_DLL = compilers/msvc420/bin/msvcrt40.dll

$(MSVCRT_DLL): 3rdparty/msvcrt40.dll
	cp -f $< $@

3rdparty/msvcrt40.dll:
	@echo "Error: 3rdparty/msvcrt40.dll missing." >&2
	@echo "The MSVC420 submodule ships an msvcrt40.dll that does not work under wibo." >&2
	@echo "Place a working copy at 3rdparty/msvcrt40.dll (see README.md, Setup step 2)." >&2
	@exit 1

$(TARGET): $(OBJS) | $(MSVCRT_DLL)
	env LIB='$(GAME_LIBPATH)' $(LINK) $(LINKFLAGS) /MAP:$(MAPFILE) $^ $(GAME_LIBS) /OUT:$@

$(OUT_DIR)/%.obj $(OUT_DIR)/%.asm: src/%.c | $(WIBO) $(MSVCRT_DLL)
	@mkdir -p $(dir $(OUT_DIR)/$*)
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS_CORE) $< \
		/Fo$(OUT_DIR)/$*.obj \
		/Fa$(OUT_DIR)/$*.asm \
		> $(OUT_DIR)/$*.stdout

# ix/ is built unoptimised; see the CFLAGS_IX note above.
$(OUT_DIR)/ix/%.obj $(OUT_DIR)/ix/%.asm: src/ix/%.cpp | $(WIBO) $(MSVCRT_DLL)
	@mkdir -p $(OUT_DIR)/ix
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS_IX) /I src\\ix $< \
		/Fo$(OUT_DIR)/ix/$*.obj \
		/Fa$(OUT_DIR)/ix/$*.asm \
		> $(OUT_DIR)/ix/$*.stdout

# The console-owner wrapper at 0x425B00 contains compiler-generated C++
# construction cleanup.  /GX on this unit reproduces it exactly; debug.cpp has
# no such unwind records and stays on the core defaults.
$(OUT_DIR)/pilot.obj $(OUT_DIR)/pilot.asm: CFLAGS_CORE_CPP_EXTRA = /GX

$(OUT_DIR)/%.obj $(OUT_DIR)/%.asm: src/%.cpp | $(WIBO) $(MSVCRT_DLL)
	@mkdir -p $(dir $(OUT_DIR)/$*)
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS_CORE) $(CFLAGS_CORE_CPP_EXTRA) $< \
		/Fo$(OUT_DIR)/$*.obj \
		/Fa$(OUT_DIR)/$*.asm \
		> $(OUT_DIR)/$*.stdout

# ---------------------------------------------------------------------------
# Recovery reports and binary-comp audits
# ---------------------------------------------------------------------------

BC = --config $(VERIFY_CONFIG) --target full

CALLS_FLAGS ?=
GLOBAL_ACCESS_FLAGS ?=
ORDER_FLAGS ?=
ANALYZE_FILES ?= all

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

progress:
	@python3 bin/showProgress.py

report: $(TARGET) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) report $(BC) $(if $(FILTER),--filter $(FILTER))

# Compare a single rebuilt function against the original.  This is the inner
# loop while implementing:  make compare-func FUNC=MinShort
# Exports are named FUN_<ADDRESS>.disassembled.txt (see ExportToCompile.java),
# so resolve the file from the `Function:` header rather than the symbol name.
compare-func: $(TARGET) | code-full $(ORIGINAL_EXE)
	@test -n "$(FUNC)" || (echo "usage: make compare-func FUNC=<FunctionName>" >&2 && exit 1)
	@f=$$(grep -lE "^Function: $(FUNC)$$" $(CODE_DIR)/*.disassembled.txt 2>/dev/null | head -1); \
	test -n "$$f" || (echo "no export for $(FUNC) in $(CODE_DIR)/ -- run 'make export-asm'" >&2 && exit 1); \
	$(BINARY_COMP) compare $(BC) --no-build $(FUNC) "$$f"

# Regenerate code-full/ straight from the original PE with Capstone.  Preferred
# over scraping Ghidra; bin/exportGhidra.py remains for names Ghidra knows and
# the PE does not.
export-asm: | $(ORIGINAL_EXE)
	@$(BINARY_COMP) export-asm $(BC) $(EXPORT_ASM_FLAGS)

# Split near-miss functions into source-reachable vs allocator churn.
triage: $(TARGET) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) triage $(BC) $(if $(FILTER),--filter $(FILTER))

# Original-address compilation-unit ordering and boundary evidence.  This is the
# main tool for recovering game-core module boundaries, which are still unknown.
order: $(TARGET) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) order $(BC) --no-build $(if $(FILTER),--filter $(FILTER)) $(ORDER_FLAGS)

seh: $(TARGET) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) seh $(BC) --report $(if $(FILTER),--filter $(FILTER)) || true

globals-data: $(TARGET) | $(ORIGINAL_EXE)
	@$(BINARY_COMP) data $(BC)

globals-data-verbose: $(TARGET) | $(ORIGINAL_EXE)
	@$(BINARY_COMP) data $(BC) --verbose

missing-data: | $(ORIGINAL_EXE)
	@$(BINARY_COMP) data $(BC) \
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
	@$(MAKE) verify-globals
	@$(MAKE) verify-globals-code
	@$(MAKE) verify-calls
	@$(MAKE) verify-global-access
	@$(MAKE) verify-values
	@$(MAKE) verify-values-stack-locals
	@$(MAKE) verify-vtables

verify-globals: $(TARGET) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) globals $(BC) --fail-on-issues --fail-on-warnings

verify-globals-code: $(TARGET) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) globals $(BC) \
		--include-code-globals \
		--fail-on-issues \
		--fail-on-warnings \
		--min-address $(GLOBALS_MISSING_MIN_ADDRESS) \
		--max-address $(GLOBALS_MISSING_MAX_ADDRESS)

globals-missing: verify-globals-code

# Compatibility aliases used by the sibling project's notes and scripts.
globals: globals-data

globals-verbose: globals-data-verbose

audit-auto-complete-globals: $(TARGET) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) globals $(BC) --show-auto-complete-reviewed

audit-rebuilt-global-layout: $(TARGET) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) globals $(BC) --check-rebuilt-layout

# WC1's own code is C and has no vtables, but the DirectDraw/DirectSound COM
# interfaces are dispatched through vtables, so this stays in the checklist.
verify-vtables: $(TARGET) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) vtables $(BC)

verify-calls: | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) calls $(BC) $(CALLS_FLAGS) $(if $(FILTER),$(FILTER))

verify-global-access: | code-full $(ORIGINAL_EXE)
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
# Analysis-only Clang diagnostics
# ---------------------------------------------------------------------------

analyze:
	@$(MAKE) analyze-clang ANALYZE_FILES="$(ANALYZE_FILES)"
	@$(MAKE) analyze-static ANALYZE_FILES="$(ANALYZE_FILES)"

analyze-clang:
	@tools/analyze_clang.sh $(ANALYZE_FILES)

analyze-static:
	@tools/analyze_static.sh $(ANALYZE_FILES)

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
	@test -n "$(WC1_ISO)" || \
		(echo "Error: no disc image found. Put the Kilrathi Saga ISO in data/ or set WC1_ISO=." >&2 && exit 1)
	@command -v bsdtar >/dev/null 2>&1 || \
		(echo "Error: bsdtar not found; needed to read the ISO." >&2 && exit 1)
	@echo "Extracting WC1 from $(WC1_ISO)..."
	@mkdir -p "$(RUN_DIR)"
	@bsdtar -xf "$(WC1_ISO)" -C "$(RUN_DIR)" --strip-components=1 WC1

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

.PHONY: \
	all \
	analyze \
	analyze-clang \
	analyze-static \
	audit-addresses \
	audit-auto-complete-globals \
	audit-rebuilt-global-layout \
	build \
	build-full \
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
	order \
	progress \
	report \
	run \
	run-check \
	seh \
	sort \
	verify \
	verify-calls \
	verify-global-access \
	verify-globals \
	verify-globals-code \
	verify-values \
	verify-values-stack-locals \
