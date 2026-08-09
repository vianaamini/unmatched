cmake_minimum_required(VERSION 3.20)

project(unmatched)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
    GIT_TAG v3.0.0
)

FetchContent_MakeAvailable(ftxui)

find_package(raylib REQUIRED)

add_executable(unmatched
    main.cpp

    src/menu.cpp
    src/age_screen.cpp
    src/hero_selection.cpp

    src/card.cpp
    src/character.cpp
    src/deck.cpp
    src/deck-builder.cpp
    src/deployment.cpp
    src/dracula_cards.cpp
    src/dracula.cpp
    src/game.cpp
    src/game_manager.cpp
    src/hero.cpp
    src/map.cpp
    src/movement.cpp
    src/sherlock.cpp
    src/sherlock_card.cpp
    src/sidekick.cpp
    src/sister.cpp
    src/tui_controller.cpp
    src/turn_manager.cpp
    src/watson.cpp
)

target_include_directories(unmatched PRIVATE include)

target_link_libraries(unmatched PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
    raylib
)