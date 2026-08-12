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
    ${WC1_SDL_TARGET}
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
    ${WC1_SDL_TARGET}
)

enable_testing()
add_test(NAME sdl-compat-smoke COMMAND wc1-sdl-compat-smoke)
add_test(NAME sdl-ix-compat-smoke COMMAND wc1-sdl-ix-compat-smoke)
