#ifndef DERACULA_CARDS_HPP
#define DERACULA_CARDS_HPP

#include "hero.hpp"
#include "card.hpp"
#include "character.hpp"
#include "dracula.hpp"
#include "sidekick.hpp"
#include "map.hpp"
#include "game_manager.hpp"
#include <vector>
#include <iostream>

using namespace std;

class dcards {
public:  
    static bool are_adjacent(const character& a, const character& b);
    static bool are_in_same_zone(const character& a, const character& b, Board& board);
    static void resolve_scheme(const card& played_card, hero& dracula, hero& opponent, 
                               const vector<character*>& all_enemies, 
                               vector<sidekick*>& all_sisters, Board& board, GameManager* gm = nullptr);
    static void resolve_combat_effects(const card& attacker_card, hero& attacker, 
                                       const card& defender_card, hero& defender, 
                                       vector<sidekick*>& all_sisters, Board& board);
};

#endif