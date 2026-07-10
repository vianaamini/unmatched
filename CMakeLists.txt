cmake_minimum_required(VERSION 3.20.0)
project(
    Unmatched
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG v5.0.0
)
FetchContent_MakeAvailable(ftxui)

add_executable(unmatched
    main.cpp
    src/card.cpp
    src/character.cpp
    src/deck-builder.cpp
    src/deck.cpp
    src/deracula_cards.cpp
    src/dracula.cpp
    src/hero.cpp
    src/sherlock_card.cpp
    src/sherlock.cpp
    src/sidekick.cpp
    src/sister.cpp
    src/watson.cpp
    # src/board.cpp
    src/movement.cpp
    src/deployment.cpp
    src/turn_manager.cpp
    src/game_manager.cpp
    src/map.cpp
    src/tui_controller.cpp
)


target_include_directories(unmatched PUBLIC include)

target_link_libraries(unmatched PRIVATE
    ftxui::screen
    ftxui::component
    ftxui::dom
)