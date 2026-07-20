#include "../include/deracula_cards.hpp"
#include "../include/map.hpp"
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>

bool dcards::are_adjacent(const character& a, const character& b) {
    int dx = abs(a.getx() - b.getx());
    return dx <= 1 && dx != 0;
}

void dcards::resolve_scheme(const card& played_card, hero& dracula, hero& opponent, 
                            const vector<character*>& all_enemies, 
                            vector<sidekick*>& all_sisters, Board& board) {
    string name = played_card.get_name();
    
    if (name == "Mistform") {
        dracula.set_actions(dracula.get_actions() + 1);
        cout << "Enter target node (n1 to n32): ";
        string node; 
        cin >> node;
        if (board.hasSpace(node)) {
            int id = board.getNodeId(node);
            dracula.setposition(id);
            cout << "Mistform: Dracula moved to " << node << endl;
        } else {
            cout << "Invalid node!" << endl;
        }
    }
    else if (name == "Baptism of Blood") {
        dracula.heal(2);
        cout << "Baptism of Blood: Dracula healed 2 HP" << endl;
        
        for (auto &b : all_sisters) {
            if (b->gethealth() <= 0) {
                b->heal(1);
                // قرار دادن در کنار دراکولا
                auto neighbors = board.getNeighborIds(dracula.getx());
                if (!neighbors.empty()) {
                    b->setposition(neighbors[0]);
                    cout << "Baptism of Blood: Sister revived at n" << neighbors[0] << endl;
                }
                break;
            }
        }
    }
    else if (name == "Prey Upon") {
        int sumtargethits = 0;
        for (auto &e : all_enemies) {
            if (e != nullptr && are_adjacent(dracula, *e) && e->isalive()) {
                e->takedamage(1);
                sumtargethits++;
                cout << "Prey Upon: " << e->getname() << " took 1 damage" << endl;
            }
        }
    
        if (sumtargethits > 0) {
            dracula.heal(sumtargethits);
            cout << "Prey Upon: Dracula healed " << sumtargethits << " HP" << endl;
        }
    }
    else if (name == "Ravening Seduction") {
        int adjacent_sisters = 0;
        
        // حرکت حریف
        auto oppNeighbors = board.getNeighborIds(opponent.getx());
        if (!oppNeighbors.empty()) {
            opponent.setposition(oppNeighbors[0]);
            cout << "Ravening Seduction: Opponent moved to n" << oppNeighbors[0] << endl;
        }
        
        // حرکت Sisters
        for (auto &sister : all_sisters) {
            auto sisNeighbors = board.getNeighborIds(sister->getx());
            if (!sisNeighbors.empty()) {
                sister->setposition(sisNeighbors[0]);
            }
            
            if (sister->gethealth() > 0 && are_adjacent(*sister, opponent)) {
                adjacent_sisters++;
            }
        }
        
        if (adjacent_sisters > 0) {
            opponent.takedamage(adjacent_sisters);
            cout << "Ravening Seduction: " << opponent.getname() 
                 << " took " << adjacent_sisters << " damage!" << endl;
        } else {
            cout << "Ravening Seduction: No Sisters were near the target. 0 damage dealt." << endl;
        }
    }
}

void dcards::resolve_combat_effects(const card& attacker_card, hero& attacker, 
                                    const card& defender_card, hero& defender, 
                                    vector<sidekick*>& all_sisters, Board& board) {
    string atkname = attacker_card.get_name();
    string defname = defender_card.get_name();
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
    }
    else if (atkname == "Feeding Frenzy") {
        int sisternearenemy = 0;
        for (auto &s : all_sisters) {
            if (s->gethealth() > 0 && are_adjacent(*s, defender)) {
                sisternearenemy++;
            }
        }
        final_attack += sisternearenemy;
        cout << "Feeding Frenzy: Attack increased by " << sisternearenemy << endl;
    }
    else if (atkname == "Ambush") {
        auto &defender_hand = defender.gethand();
        if (!defender_hand.empty()) {
            size_t random_index = rand() % defender_hand.size();
            card discardcard = defender_hand[random_index];
            final_attack += discardcard.getboost();
            defender_hand.erase(defender_hand.begin() + random_index);
            cout << "Ambush: Opponent discarded " << discardcard.get_name() << endl;
        }
    }
    else if (atkname == "Beastform") {
        auto &attacker_hand = attacker.gethand();
        size_t cards_in_hand = attacker_hand.size();
        if (cards_in_hand > 0) {
            int discardcards = 0;
            cout << "How many cards to discard? (Max: " << cards_in_hand << "): ";
            cin >> discardcards;
            if (discardcards > static_cast<int>(cards_in_hand)) discardcards = cards_in_hand;
            if (discardcards < 0) discardcards = 0;
            
            for (int i = 0; i < discardcards; i++) {
                attacker_hand.erase(attacker_hand.begin() + i);
                final_attack += 1;
            }
            cout << "Beastform: Discarded " << discardcards << " cards, attack +" << discardcards << endl;
        }
    }
    else if (defname == "Look Into My Eyes") {
        look_into_eyes_active = true;
        final_defense += attacker_card.getboost();
        cout << "Look Into My Eyes: Defense boosted by " << attacker_card.getboost() << endl;
    }

    int damage = 0;
    if (!look_into_eyes_active) {
        damage = final_attack - final_defense;
    }

    bool attacker_won = (damage > 0);

    if (damage > 0) {
        defender.takedamage(damage);
        cout << defender.getname() << " took " << damage << " damage" << endl;
    }
    
    if (atkname == "Exploit") {
        attacker.drawcard();
        cout << "Exploit: Drew 1 card" << endl;
    }
    if (defname == "Exploit") {
        defender.drawcard();
        cout << "Exploit: Drew 1 card" << endl;
    }

    if (atkname == "Dash") {
        auto neighbors = board.getNeighborIds(attacker.getx());
        if (neighbors.size() >= 3) {
            attacker.setposition(neighbors[2]);
            cout << "Dash: Moved to n" << neighbors[2] << endl;
        } else {
            cout << "Dash: Not enough neighbors!" << endl;
        }
    }
    if (defname == "Dash") {
        auto neighbors = board.getNeighborIds(defender.getx());
        if (neighbors.size() >= 3) {
            defender.setposition(neighbors[2]);
            cout << "Dash: Moved to n" << neighbors[2] << endl;
        } else {
            cout << "Dash: Not enough neighbors!" << endl;
        }
    }

    if (atkname == "Thirst for Sustenance") {
        if (attacker_won) {
            auto defNeighbors = board.getNeighborIds(defender.getx());
            if (!defNeighbors.empty()) {
                attacker.setposition(defNeighbors[0]);
                cout << "Thirst for Sustenance: Dracula placed adjacent to opponent at n" 
                     << defNeighbors[0] << endl;
            }
        } else {
            cout << "You did not win the combat!" << endl;
        }
    }
}