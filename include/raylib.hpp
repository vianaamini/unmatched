#pragma once

#include "raylib.h"
#include "../include/map.hpp"
#include "character.hpp"
#include "hero.hpp"
#include "../include/game_manager.hpp"

void RunGameUI(GameManager& gm, character* dracula, character* sis1Obj, character* sis2Obj, character* sis3Obj,
               character* sherlock, character* watson, int firstPlayer, hero* draculaHero = nullptr, hero* sherlockHero = nullptr);
