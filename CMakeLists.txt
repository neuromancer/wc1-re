cmake_minimum_required(VERSION 3.16)

project(WingCommanderSDL LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 CONFIG QUIET)
if(TARGET SDL2::SDL2)
    set(WC1_SDL_TARGET SDL2::SDL2)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
    set(WC1_SDL_TARGET PkgConfig::SDL2)
endif()

add_library(wc1-sdl-host STATIC
    src/sdl/compat.c
)
target_compile_definitions(wc1-sdl-host PUBLIC
    WC1_SDL=1
)
target_include_directories(wc1-sdl-host PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(wc1-sdl-host PUBLIC
    ${WC1_SDL_TARGET}
)

# Platform-neutral recovered units.  This list grows as each Win32 boundary is
# isolated; keeping it explicit makes native compile progress measurable.
add_library(wc1-sdl-gameplay OBJECT
    src/auto.c
    src/barracks.c
    src/brains.c
    src/cmpgn.c
    src/cockpt.c
    src/disk.c
    src/eventmgr.c
    src/geom.c
    src/globals.c
    src/killbrd.c
    src/mathfp.c
    src/mathutil.c
    src/music.c
    src/nav.c
    src/ship.c
    src/smart.c
    src/spc.c
    src/strdos.c
    src/system.c
    src/text.c
)
target_link_libraries(wc1-sdl-gameplay PRIVATE
    wc1-sdl-host
)

add_executable(wc1-sdl-compat-smoke
    tests/sdl_compat_smoke.c
)
target_compile_definitions(wc1-sdl-compat-smoke PRIVATE
    WC1_ANALYSIS=1
    WC1_SDL=1
)
target_include_directories(wc1-sdl-compat-smoke PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(wc1-sdl-compat-smoke PRIVATE
    wc1-sdl-host
)

add_executable(wc1-sdl-ix-compat-smoke
    tests/sdl_ix_compat_smoke.cpp
)
target_compile_definitions(wc1-sdl-ix-compat-smoke PRIVATE
    WC1_SDL=1
)
target_include_directories(wc1-sdl-ix-compat-smoke PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ix
)
target_link_libraries(wc1-sdl-ix-compat-smoke PRIVATE
    wc1-sdl-host
)

add_executable(wc1-sdl-crt-compat
    tests/sdl_crt_compat.c
)
target_link_libraries(wc1-sdl-crt-compat PRIVATE
    wc1-sdl-host
)

enable_testing()
add_test(NAME sdl-compat-smoke COMMAND wc1-sdl-compat-smoke)
add_test(NAME sdl-ix-compat-smoke COMMAND wc1-sdl-ix-compat-smoke)
add_test(NAME sdl-crt-compat COMMAND wc1-sdl-crt-compat)
