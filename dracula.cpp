#include "dracula.hpp"
#include "deck-builder.hpp"
#include "hero.hpp"
#include "card.hpp"
#include <iostream>

dracula::dracula() : hero("Dracula", 13 , 2) {
    deck draculadk = draculadeck();
    draculadk.shuffle();
    
    while (!draculadk.isempty()) {
        getdeck().addcard(draculadk.drawcard());
    }
}

void dracula::useability() {
    std::cout << "dracula ability activated" << std::endl;
}

void dracula::resolvecard(const card& played_card, hero& opponent)
{
    string card_name = played_card.get_name();
    if(card_name == "Feeding Frenzy")
    {   // need map 
        // int count = map.count_sisters_in_zone(opponent.get_position());
        // this->current_attack += count;
    }
    else if (card_name == "Mistform")
    {
       
        // need map 
        this->set_actions(this->get_actions() + 1); 
        std::cout << "Mistform executed: Gained 1 action!" << std::endl;
    
    }
    else if (card_name == "Ambush")
    {
        if (!oppenent.getdeck().isempty())
        {
            card discarded = opponent.getdeck().drawcard();
            int finalattack = played_card.getattackvalue() + discarded.getboost();
            this->set_combat_attack(final_attack);
            int enemydefense = opponent.get_combat_defense(); 
            int damage_to_inflict = finalattack - enemydefense;
            if (damage_to_inflict < 0) damage_to_inflict = 0;
            opponent.takedamage(damage_to_inflict);
            cout << "Ambush executed: Inflicted " << damage_to_inflict << " damage to opponent!" << endl;
        }
    }
}
