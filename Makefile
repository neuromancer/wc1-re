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
#   make run-wine        # build and launch under Wine
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
#   NO /GX C++ exception handling.  There is no __CxxFrameHandler and no RTTI
#          type descriptors (.?AV) anywhere in the image.  SEH in the game code
#          uses _except_handler3 scope tables, which needs no flag in C.
CFLAGS_COMMON = \
	/nologo \
	/c \
	/MTd \
	/Og /Oi /Ot /Oy /Ob1 \
	/Gs \
	/I include \
	/I 3rdparty\\dx\\include

# The game core is C (the leaked WINGLEADER main module is a .c file including
# <game.h>); the ix audio library is C++.  CL picks the language from the file
# extension, so both use the same flag set.
CFLAGS = $(CFLAGS_COMMON)

LINKFLAGS = /nologo /SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup /ALIGN:4096

# DDRAW.DLL and DSOUND.DLL are bound statically through the import table, so
# import libraries are required at link time.  MSVC 4.2 predates them; drop
# ddraw.lib and dsound.lib from a DirectX 2/3-era SDK into 3rdparty/dx/lib.
GAME_LIBPATH = $(MSVC_LIB);3rdparty\dx\lib
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
# Host platform and Wine runtime
# ---------------------------------------------------------------------------

UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

WINE ?= wine
WINEPREFIX ?= $(CURDIR)/.wine-wc1
WINE_RUN_DIR = data/full

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
SRCS_ORDERED_CORE = \
	src/main.c \
	src/globals.c

SRCS_ORDERED_IX = \
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
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS) $< \
		/Fo$(OUT_DIR)/$*.obj \
		/Fa$(OUT_DIR)/$*.asm \
		> $(OUT_DIR)/$*.stdout

$(OUT_DIR)/%.obj $(OUT_DIR)/%.asm: src/%.cpp | $(WIBO) $(MSVCRT_DLL)
	@mkdir -p $(dir $(OUT_DIR)/$*)
	@env INCLUDE='$(MSVC_INC)' $(CC) $(CFLAGS) $< \
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

sort:
	@python3 bin/sortByAddress.py

progress:
	@python3 bin/showProgress.py

report: $(TARGET) | code-full $(ORIGINAL_EXE)
	@$(BINARY_COMP) report $(BC) $(if $(FILTER),--filter $(FILTER))

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
# Wine launch targets
# ---------------------------------------------------------------------------

wine-check:
	@command -v $(WINE) >/dev/null 2>&1 || \
		(echo "Error: wine not found. Install Wine or set WINE=/path/to/wine." >&2 && exit 1)
	@test -d "$(WINE_RUN_DIR)" || \
		(echo "Error: expected installed game data at $(WINE_RUN_DIR)." >&2 && exit 1)

wine-stage: $(TARGET) wine-check
	cp -f $(TARGET) "$(WINE_RUN_DIR)/WC1.EXE"

run-wine: wine-stage
	cd "$(WINE_RUN_DIR)" && WINEPREFIX="$(WINEPREFIX)" $(WINE) WC1.EXE

run-wine-original: wine-check $(ORIGINAL_EXE)
	cd "$(WINE_RUN_DIR)" && WINEPREFIX="$(WINEPREFIX)" $(WINE) WC1.ORI.EXE

run: run-wine

# ---------------------------------------------------------------------------
# Cleanup and phony declarations
# ---------------------------------------------------------------------------

clean:
	rm -rf $(OUT_DIR)/*.obj $(OUT_DIR)/*.asm $(OUT_DIR)/*.stdout \
	       $(OUT_DIR)/ix $(TARGET) $(MAPFILE)

clean-wine:
	rm -f "$(WINE_RUN_DIR)/WC1.EXE"

.PHONY: \
	all \
	analyze \
	analyze-clang \
	analyze-static \
	build \
	clean \
	clean-wine \
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
	run-wine \
	run-wine-original \
	seh \
	sort \
	verify \
	verify-calls \
	verify-global-access \
	verify-globals \
	verify-globals-code \
	verify-values \
	verify-values-stack-locals \
	wine-check \
	wine-stage
