#include "../include/deracula_cards.hpp"
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib> 
using namespace std;

bool dcards::are_adjacent(const character& a, const character& b)
{
    int dx = abs(a.getx() - b.getx());
    int dy = abs(a.gety() - b.gety());
    return (dx <= 1 && dy <= 1) && !(dx == 0 && dy == 0);
}

void dcard::resolve_scheme(const card& played_card, hero& deracula, hero& opponent , const vector<character*>& all_enemies, vector<sidekick*>& all_sisters )
{
    string name = played_card.get_name();
    if (name == "Mistform")
    {
        deracula.set_action(get_action()+1);
        // need map for changing location for setting deracula 
    }

    else if (name == "Baptism of Blood")
    {
        deracula.heal(2);
        for(auto &b : all_sisters)
        {
            if (b->gethealth() <= 0)
            {
                b->heal(1);
                // b.setposition(deracula.getx() , deracula.gety())
                // needs maps for zone so i put it in comments
                break;
            }
        }
    }

    else if( name == "Prey Upon")
    {
        int sumtargethits = 0;
        for(auto &e : all_enemies)
        {
            if (e != nullptr && are_adjacent(user, *e) && enemy->gethealth() > 0)
            {
                e->takedamage(1);
                sumtargethits++;
            }
        }
    
        if (sumtargethits > 0)
        {
            deracula.heal(sumtargethits);
        }
    }

    else if (name == "Ravening Seduction")
    {
        
        int adjacent_sisters = 0;

        for (auto &sister : all_sisters) {
            sister->setposition(sister->getx() + 2 , sister->gety())
            // needs map

            if (sister->gethealth() > 0 && are_adjacent(*sister, opponent)) {
                adjacent_sisters++;
            }
        }
        if (adjacent_sisters > 0) {
            opponent.takedamage(adjacent_sisters);
            cout << "Ravening Seduction: " << opponent.getname() 
                    << " was surrounded by " << adjacent_sisters 
                    << " Sister(s) and took " << adjacent_sisters << " damage!" << endl;
        }
        else{
            cout << "Ravening Seduction: No Sisters were near the target after movement. 0 damage dealt." << endl;
        }
    }

}

void dcard::resolve_combat_effects(const card& attacker_card, hero& attacker,const card& defender_card, hero& defender , vector<sidekick*>& all_sisters)
{
    string atkname = attacker_card.getname();
    string defname = defender_card.getname();
    int final_attack = attacker_card.getattack();
    int final_defense = defender_card.getdefense();
    bool look_into_eyes_active = false;
    if (atkname == "Feint" || defname == "Feint") {
    
        cout << "Feint: All card effects are canceled!" << endl;
        int base_damage = final_attack - final_defense;
        if (base_damage > 0) {
            defender.takedamage(base_damage);
            cout << defender.getname() << " took " << base_damage << " base damage." << endl;
        } else {
            cout << defender.getname() << " defended successfully!" << endl;
        }
        return; 
    } // not sure about this one

    else if(atkname == "Feeding Frenzy")
    {
        int sisternearenemy = 0;
        for (auto &s : all_sisters)
        {
            if (s->gethealth() > 0 && are_adjacent(*s, defender))
            {
                sisternearenemy++;
            }

        }
        final_attack += sisternearenemy;
    }

    else if(atkname == "Ambush")
    {
        auto &defender_hand = defender.gethand();
        if (!defender_hand.empty())
        {
            size_t random_index = rand() % defender_hand.handsize();
            card discardcard = defender_hand[random_index];
            final_attack += discardcard.getboost();
            defender_hand.erase(defender_hand.begin() + random_index);
        }
    }

    else if (atkname == "Beastform")
    {
        auto & attacker_hand = attacker.gethand();
        size_t cards_in_hand = attacker_hand.size();
        if (cards_in_hand > 0)
        {
            int discardcards = 0;
            cout << "How many cards do you want to discard ?" << endl;
            cin >> discardcards;
            if (discardcards > static_cast<int>(cards_in_hand)) {discardcards = cards_in_hand;}
            if (discardcards < 0) { discardcards = 0;}
            for (int i = 0; i < discardcards; i++)
            {
                attacker_hand.erase(attacker_hand.begin() + i);
                final_attack += 1;
            }
        }
    }

    else if (defname == "Look Into My Eyes"){
        look_into_eyes_active = true;
        final_defense += attacker_card.getboost();
    }

    int damage = 0;
    if (!look_into_eyes_active) {
        damage = final_attack - final_defense;
    }

    bool attacker_won = (damage > 0); 

    if (damage > 0) {
        defender.takedamage(damage);
    }
    
    else if (atkname == "Exploit") { attacker.drawcard();}
    else if(defname == "Exploit") {defender.drawcard();}

    else if (atkname == "Dash")
    {
        
        attacker.setposition(attacker.getx() + 3 , attacker.gety())
        
    }
    else if (defname == "Dash")
    {
        defender.setposition(defender.getx() + 3 , defender.gety())
    }

    if (atkname == "Thirst for Sustenance") {
        if (attacker_won) {
            //attacker.setx(defender.getx() + 1);
            //attacker.sety(defender.gety());
            attacker.setposition(attacker.getx() + 1, attacker.gety())
        }
        else {
            cout << "you did not win " << endl;
        }
    }
         
}
// note and warning !!! : all of the setposition needs to change after handeling map . we should change the position 
// based on our map .
