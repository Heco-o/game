cmake_minimum_required(VERSION 3.28)
project(maze_game LANGUAGES CXX)

include(FetchContent)

# Keep third-party projects from building their own examples/tests.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)

# SFML 3.1.0: official Android setup.
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW TRUE
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

# EnTT is header-only. Populate it without configuring its own project.
FetchContent_Declare(
    EnTT
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.15.0
    GIT_SHALLOW TRUE
)
FetchContent_GetProperties(EnTT)
if(NOT entt_POPULATED)
    FetchContent_Populate(EnTT)
endif()

add_library(EnTT INTERFACE)
target_include_directories(EnTT INTERFACE "${entt_SOURCE_DIR}/src")
target_compile_features(EnTT INTERFACE cxx_std_20)

# GLM is also header-only.
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glm)

# Android loads this shared library through NativeActivity.
add_library(maze_game SHARED main.cpp)

target_compile_features(maze_game PRIVATE cxx_std_20)
target_compile_options(maze_game PRIVATE
    $<$<CONFIG:Release>:-O3>
)

target_link_libraries(maze_game PRIVATE
    SFML::Graphics
    SFML::Main
    EnTT
    glm::glm
)
