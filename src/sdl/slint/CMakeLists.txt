cmake_minimum_required(VERSION 3.21)

project(wc1_slint_gui VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# The launcher is linked into the modern-gui executable. Keep Slint itself
# static and enable only the window backend and software renderer it needs.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SLINT_FEATURE_BACKEND_QT OFF CACHE BOOL "" FORCE)
set(SLINT_FEATURE_BACKEND_WINIT ON CACHE BOOL "" FORCE)
set(SLINT_FEATURE_RENDERER_SKIA OFF CACHE BOOL "" FORCE)
set(SLINT_FEATURE_ACCESSIBILITY OFF CACHE BOOL "" FORCE)
set(SLINT_FEATURE_INTERPRETER OFF CACHE BOOL "" FORCE)
set(SLINT_FEATURE_RENDERER_FEMTOVG OFF CACHE BOOL "" FORCE)
set(SLINT_FEATURE_RENDERER_SOFTWARE ON CACHE BOOL "" FORCE)
set(SLINT_FEATURE_TESTING OFF CACHE BOOL "" FORCE)
set(NFD_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(NFD_BUILD_SDL2_TESTS OFF CACHE BOOL "" FORCE)
set(NFD_INSTALL OFF CACHE BOOL "" FORCE)
set(WC1_GAME_OBJECTS "" CACHE STRING
    "Semicolon-separated game object files for the integrated launcher")
option(WC1_GAME_SANITIZERS
    "Link the integrated executable with the development sanitizers" OFF)
if(UNIX AND NOT APPLE)
    # The portal backend avoids pulling GTK into the optional launcher.
    set(NFD_PORTAL ON CACHE BOOL "" FORCE)
endif()

include(FetchContent)
FetchContent_Declare(
    Slint
    GIT_REPOSITORY https://github.com/slint-ui/slint.git
    GIT_TAG v1.16.1
    GIT_SHALLOW TRUE
    SOURCE_SUBDIR api/cpp
)
FetchContent_MakeAvailable(Slint)
FetchContent_Declare(
    NativeFileDialogExtended
    GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
    GIT_TAG v1.3.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(NativeFileDialogExtended)

add_library(wc1-slint-gui STATIC
    launcher.cpp
    slint_static.h
    launcher_paths.cpp
    launcher_paths.h
)
target_link_libraries(wc1-slint-gui PRIVATE Slint::Slint nfd::nfd)
slint_target_sources(wc1-slint-gui launcher.slint)

if(UNIX AND NOT APPLE)
    # Slint's static Rust library uses Fontconfig to find system fonts.
    find_package(Fontconfig REQUIRED)
    target_link_libraries(wc1-slint-gui PRIVATE Fontconfig::Fontconfig)
endif()

if(WIN32)
    # Slint's Windows headers assume a DLL even with BUILD_SHARED_LIBS=OFF.
    # With section garbage collection, lld can discard the imported accessors
    # and leave calls pointing at the PE image base. Correct the declarations
    # before either launcher.cpp or Slint's generated source includes them.
    if(MSVC)
        target_compile_options(wc1-slint-gui PRIVATE
            "/FI${CMAKE_CURRENT_SOURCE_DIR}/slint_static.h"
        )
    else()
        target_compile_options(wc1-slint-gui PRIVATE
            -include "${CMAKE_CURRENT_SOURCE_DIR}/slint_static.h"
        )
    endif()
    target_link_libraries(wc1-slint-gui PRIVATE
        imm32
        ole32
        shell32
        uuid
        uxtheme
    )
endif()

# Corrosion cannot propagate framework metadata out of a Rust static library.
# These are the native frameworks used by Slint's macOS winit backend.
if(APPLE)
    find_library(APPKIT_FRAMEWORK AppKit REQUIRED)
    find_library(CARBON_FRAMEWORK Carbon REQUIRED)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
    find_library(COREGRAPHICS_FRAMEWORK CoreGraphics REQUIRED)
    find_library(CORESERVICES_FRAMEWORK CoreServices REQUIRED)
    find_library(CORETEXT_FRAMEWORK CoreText REQUIRED)
    find_library(COREVIDEO_FRAMEWORK CoreVideo REQUIRED)
    find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
    find_library(QUARTZCORE_FRAMEWORK QuartzCore REQUIRED)
    target_link_libraries(wc1-slint-gui PRIVATE
        ${APPKIT_FRAMEWORK}
        ${CARBON_FRAMEWORK}
        ${COREFOUNDATION_FRAMEWORK}
        ${COREGRAPHICS_FRAMEWORK}
        ${CORESERVICES_FRAMEWORK}
        ${CORETEXT_FRAMEWORK}
        ${COREVIDEO_FRAMEWORK}
        ${FOUNDATION_FRAMEWORK}
        ${QUARTZCORE_FRAMEWORK}
    )
endif()

if(MSVC)
    target_compile_options(wc1-slint-gui PRIVATE /W4)
else()
    target_compile_options(wc1-slint-gui PRIVATE -Wall -Wextra)
endif()

if(NOT WC1_GUI_OUTPUT_DIRECTORY)
    set(WC1_GUI_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()
get_filename_component(
    WC1_GUI_OUTPUT_DIRECTORY
    "${WC1_GUI_OUTPUT_DIRECTORY}"
    ABSOLUTE
)
set_target_properties(wc1-slint-gui PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${WC1_GUI_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${WC1_GUI_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY "${WC1_GUI_OUTPUT_DIRECTORY}"
)
foreach(CONFIG DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set_target_properties(wc1-slint-gui PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} "${WC1_GUI_OUTPUT_DIRECTORY}"
        LIBRARY_OUTPUT_DIRECTORY_${CONFIG} "${WC1_GUI_OUTPUT_DIRECTORY}"
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${WC1_GUI_OUTPUT_DIRECTORY}"
    )
endforeach()
if(WIN32)
    set_target_properties(wc1-slint-gui PROPERTIES PREFIX "")
endif()

if(WC1_GAME_OBJECTS)
    # Make compiles the game while CMake carries Slint's native link closure.
    # Join the two only for the integrated launcher executable.
    foreach(WC1_GAME_OBJECT IN LISTS WC1_GAME_OBJECTS)
        if(NOT IS_ABSOLUTE "${WC1_GAME_OBJECT}")
            message(FATAL_ERROR
                "WC1_GAME_OBJECTS entries must be absolute: ${WC1_GAME_OBJECT}")
        endif()
    endforeach()
    set_source_files_properties(${WC1_GAME_OBJECTS} PROPERTIES
        EXTERNAL_OBJECT TRUE
        GENERATED TRUE
    )

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(WC1_SDL2 REQUIRED IMPORTED_TARGET sdl2)
    pkg_check_modules(WC1_LZO2 REQUIRED IMPORTED_TARGET lzo2)

    add_executable(wc1-modern-gui ${WC1_GAME_OBJECTS})
    set_target_properties(wc1-modern-gui PROPERTIES LINKER_LANGUAGE CXX)
    target_link_libraries(wc1-modern-gui PRIVATE
        wc1-slint-gui
        PkgConfig::WC1_SDL2
        PkgConfig::WC1_LZO2
    )
    if(WC1_GAME_SANITIZERS AND NOT MSVC)
        target_link_options(wc1-modern-gui PRIVATE
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
        )
    endif()
    if(APPLE)
        target_link_options(wc1-modern-gui PRIVATE
            "LINKER:-dead_strip"
            "LINKER:-no_fixup_chains"
        )
    elseif(MSVC)
        target_link_options(wc1-modern-gui PRIVATE /OPT:REF)
    elseif(WIN32)
        target_link_options(wc1-modern-gui PRIVATE
            "LINKER:--gc-sections"
        )
    else()
        target_link_options(wc1-modern-gui PRIVATE
            "LINKER:--exclude-libs,ALL"
            "LINKER:--gc-sections"
        )
    endif()

    set_target_properties(wc1-modern-gui PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${WC1_GUI_OUTPUT_DIRECTORY}"
    )
    foreach(CONFIG DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
        set_target_properties(wc1-modern-gui PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${WC1_GUI_OUTPUT_DIRECTORY}"
        )
    endforeach()
endif()
