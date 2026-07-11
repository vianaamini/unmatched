#ifndef DERACULA_CARDS_HPP
#define DERACULA_CARDS_HPP

#include "hero.hpp"
#include "card.hpp"
#include "character.hpp"
#include "dracula.hpp"
#include "sidekick.hpp"
#include "map.hpp"
#include <vector>
#include <iostream>

using namespace std;

class dcards
{
public:  
    static bool are_adjacent(const character& a, const character& b);
    static void resolve_scheme(const card& played_card, hero& deracula, hero& opponent, const vector<character*>& all_enemies, vector<sidekick*>& all_sisters, Board& board);
    static void resolve_combat_effects(const card& attacker_card, hero& attacker, 
                                       const card& defender_card, hero& defender, vector<sidekick*>& all_sisters, Board& board);
};

#endif