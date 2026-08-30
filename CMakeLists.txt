cmake_minimum_required(VERSION 3.20)

project(PokemonJourney
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.20
)

FetchContent_MakeAvailable(SDL3)

add_executable(PokemonJourney
    game/main.cpp
    engine/App.cpp
)

target_include_directories(PokemonJourney PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(PokemonJourney PRIVATE
    SDL3::SDL3
)
