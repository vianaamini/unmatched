cmake_minimum_required(VERSION 3.20.0)

project(
    Unmatched
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)

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
)

target_include_directories(unmatched PUBLIC include)
