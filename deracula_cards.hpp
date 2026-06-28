#pragma once 
#include "hero.hpp"
#include "card.hpp"
#include "character.hpp"
#include "dracula.hpp"
#include <iostream>
using namespace std;

class dcards
{
    static bool are_adjacent(const character& a, const character& b  );
    public:
    static void resolve_scheme(const card& played_card, hero& deracula, hero& opponent, const vector<character*>& all_enemies, vector<sidekick*>& all_sisters); // managing scheme cards
    static void resolve_combat_effects(const card& attacker_card, hero& attacker, 
                                       const card& defender_card, hero& defender , vector<sidekick*>& all_sisters); // managing other cards

};