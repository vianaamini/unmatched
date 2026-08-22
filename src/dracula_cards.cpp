#include "../include/dracula_cards.hpp"
#include "../include/map.hpp"
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <limits>

bool dcards::are_adjacent(const character& a, const character& b, Board& board) {
    return board.isAdjacent(a.getx(), b.getx());
}

bool dcards::are_in_same_zone(const character& a, const character& b, Board& board) {
    auto zonesA = board.getZonesAt(a.getx(), 0);
    auto zonesB = board.getZonesAt(b.getx(), 0);
    for (const auto& z1 : zonesA) {
        for (const auto& z2 : zonesB) {
            if (z1 == z2) return true;
        }
    }
    return false;
}

void dcards::resolve_scheme(const card& played_card, hero& dracula, hero& opponent, 
                            const vector<character*>& all_enemies, 
                            vector<sidekick*>& all_sisters, Board& board, GameManager* gm) {
    string name = played_card.get_name();
    
    if (name == "Mistform") {
        dracula.set_actions(dracula.get_actions() + 1);
        // NOTE: this whole file (dcards::resolve_scheme /
        // resolve_combat_effects) is dead code -- nothing in the real game
        // (GameManager/raylib/actionbar path) ever calls it or even
        // includes dracula_cards.hpp; the actual Mistform/Ravening
        // Seduction/Beastform logic used in real games lives in hero.cpp.
        // This cin was removed anyway as a defensive cleanup, since a
        // blocking console read here would freeze the whole raylib window
        // if this code path were ever wired in by mistake in the future.
        string node = "n1";
        if (board.hasSpace(node)) {
            int id = board.getNodeId(node);
            // Check if occupied
            bool occupied = false;
            if (gm) {
                for (auto* c : gm->getAllCharacters()) {
                    if (c->isalive() && c->getx() == id && c != &dracula) {
                        occupied = true;
                        break;
                    }
                }
            }
            if (!occupied) {
                dracula.setposition(id);
                cout << "Mistform: Dracula moved to " << node << endl;
            } else {
                cout << "Node " << node << " is occupied!" << endl;
            }
        } else {
            cout << "Invalid node!" << endl;
        }
    }
    else if (name == "Baptism of Blood") {
        dracula.heal(2);
        cout << "Baptism of Blood: Dracula healed 2 HP" << endl;
        
        for (auto &b : all_sisters) {
            if (!b->isalive()) {
                auto zones = board.getZonesAt(dracula.getx(), 0);
                bool revived = false;
                for (const auto& zone : zones) {
                    auto spaces = board.getSpacesInZone(zone);
                    for (const auto& space : spaces) {
                        int node = space.first;
                        bool occupied = false;
                        if (gm) {
                            for (auto* c : gm->getAllCharacters()) {
                                if (c->isalive() && c->getx() == node) {
                                    occupied = true;
                                    break;
                                }
                            }
                        }
                        if (!occupied && node != dracula.getx()) {
                            b->sethealth(1);
                            b->setposition(node);
                            revived = true;
                            cout << "Baptism of Blood: Sister revived at n" << node << endl;
                            break;
                        }
                    }
                    if (revived) break;
                }
                if (!revived) {
                    cout << "Baptism of Blood: No valid node to revive Sister!" << endl;
                }
                break;
            }
        }
    }
    else if (name == "Prey Upon") {
        int sumtargethits = 0;
        for (auto &e : all_enemies) {
            if (e != nullptr && are_adjacent(dracula, *e, board) && e->isalive()) {
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
        
        // Get opponent's neighbors and let player choose
        auto oppNeighbors = board.getNeighborIds(opponent.getx());
        if (!oppNeighbors.empty()) {
            cout << "Ravening Seduction: Choose a node to move opponent to:" << endl;
            for (size_t i = 0; i < oppNeighbors.size(); i++) {
                bool occupied = false;
                if (gm) {
                    for (auto* c : gm->getAllCharacters()) {
                        if (c->isalive() && c->getx() == oppNeighbors[i] && c != &opponent) {
                            occupied = true;
                            break;
                        }
                    }
                }
                if (!occupied) {
                    cout << "  [" << i << "] n" << oppNeighbors[i] << endl;
                }
            }
            int choice = 0; // defensive cleanup: was a blocking cin >>, see note near Mistform above
            if (choice >= 0 && choice < (int)oppNeighbors.size()) {
                bool occupied = false;
                if (gm) {
                    for (auto* c : gm->getAllCharacters()) {
                        if (c->isalive() && c->getx() == oppNeighbors[choice] && c != &opponent) {
                            occupied = true;
                            break;
                        }
                    }
                }
                if (!occupied) {
                    opponent.setposition(oppNeighbors[choice]);
                    cout << "Ravening Seduction: Opponent moved to n" << oppNeighbors[choice] << endl;
                }
            }
        }
        
        // Move sisters
        for (auto &sister : all_sisters) {
            if (sister->isalive()) {
                auto sisNeighbors = board.getNeighborIds(sister->getx());
                if (!sisNeighbors.empty()) {
                    for (int node : sisNeighbors) {
                        bool occupied = false;
                        if (gm) {
                            for (auto* c : gm->getAllCharacters()) {
                                if (c->isalive() && c->getx() == node && c != sister) {
                                    occupied = true;
                                    break;
                                }
                            }
                        }
                        if (!occupied) {
                            sister->setposition(node);
                            break;
                        }
                    }
                }
            }
            
            if (sister->isalive() && are_adjacent(*sister, opponent, board)) {
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
    bool attacker_won = false;
    
    // Feint - cancel all effects
    if (atkname == "Feint" || defname == "Feint") {
        cout << "Feint: All card effects are canceled!" << endl;
        int base_damage = final_attack - final_defense;
        if (base_damage > 0) {
            defender.takedamage(base_damage);
            cout << defender.getname() << " took " << base_damage << " base damage." << endl;
            attacker_won = true;
        } else {
            cout << defender.getname() << " defended successfully!" << endl;
        }
        return;
    }
    
    // Attacker effects
    if (atkname == "Feeding Frenzy") {
        int sisternearenemy = 0;
        for (auto &s : all_sisters) {
            if (s->isalive() && are_in_same_zone(*s, defender, board)) {
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
            cout << "Ambush: Opponent discarded " << discardcard.get_name() 
                 << " (Boost: " << discardcard.getboost() << ")" << endl;
            defender_hand.erase(defender_hand.begin() + random_index);
        }
    }
    else if (atkname == "Beastform") {
        auto &attacker_hand = attacker.gethand();
        size_t cards_in_hand = attacker_hand.size();
        if (cards_in_hand > 0) {
            int discardcards = 0; // defensive cleanup: was a blocking cin >>, see note near Mistform above
            if (discardcards > static_cast<int>(cards_in_hand)) discardcards = cards_in_hand;
            if (discardcards < 0) discardcards = 0;
            
            // Erase from end to beginning to avoid index issues
            for (int i = discardcards - 1; i >= 0; i--) {
                attacker_hand.erase(attacker_hand.begin() + i);
                final_attack += 1;
            }
            cout << "Beastform: Discarded " << discardcards << " cards, attack +" << discardcards << endl;
        }
    }
    
    // Defender effects
    if (defname == "Look Into My Eyes") {
        final_defense += attacker_card.getboost();
        cout << "Look Into My Eyes: Defense boosted by " << attacker_card.getboost() 
             << " (Total: " << final_defense << ")" << endl;
    }
    
    // Calculate damage
    int damage = final_attack - final_defense;
    if (damage > 0) {
        attacker_won = true;
        defender.takedamage(damage);
        cout << defender.getname() << " took " << damage << " damage" << endl;
    } else {
        cout << defender.getname() << " defended successfully" << endl;
    }
    
    // After combat effects
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
                // Find empty neighbor
                for (int node : defNeighbors) {
                    bool occupied = false;
                    // Check if occupied by any character
                    // This is simplified - should check all characters
                    attacker.setposition(node);
                    cout << "Thirst for Sustenance: Dracula placed adjacent to opponent at n" 
                         << node << endl;
                    break;
                }
            }
        } else {
            cout << "Thirst for Sustenance: You did not win the combat!" << endl;
        }
    }
}