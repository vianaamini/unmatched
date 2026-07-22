#include "../include/sherlock_card.hpp"
#include <iostream>

void card_administer_aid::execute_effect(character& attacker, character& defender, 
                                          const card& enemy_card, 
                                          bool is_adjacent, 
                                          bool attacker_won) {
    attacker.heal(1);
    hero* h_ptr = dynamic_cast<hero*>(&attacker);
    if (h_ptr) {
        h_ptr->drawcard();
    }
    std::cout << "Administer Aid: Healed 1 HP and drew 1 card" << std::endl;
}

void card_confirm_suspicion::execute_effect(character& attacker, character& defender, 
                                             const card& enemy_card, 
                                             bool is_adjacent, 
                                             bool attacker_won) {
    std::cout << "Confirm Suspicion: Choose a value (1-6): ";
    int value;
    std::cin >> value;
    
    hero* defHero = dynamic_cast<hero*>(&defender);
    if (defHero) {
        bool found = false;
        auto& hand = defHero->gethand();
        for (size_t i = 0; i < hand.size(); i++) {
            if (hand[i].getattack() == value || hand[i].getdefense() == value) {
                int boostValue = hand[i].getboost();
                defender.takedamage(boostValue);
                std::cout << "Card with value " << value << " found! Dealt " << boostValue << " damage." << std::endl;
                hand.erase(hand.begin() + i);
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << "No card with value " << value << ". Opponent reveals hand!" << std::endl;
            for (const auto& c : hand) {
                std::cout << "  " << c.get_name() << " (ATK:" << c.getattack() << " DEF:" << c.getdefense() << ")" << std::endl;
            }
        }
    }
}

void card_counter_punch::execute_effect(character& attacker, character& defender, 
                                         const card& enemy_card, 
                                         bool is_adjacent, 
                                         bool attacker_won) {
    if (is_adjacent) {
        defender.takedamage(2);
        std::cout << "Counter Punch: Dealt 2 damage (adjacent)" << std::endl;
    } else {
        std::cout << "Counter Punch: No damage (not adjacent)" << std::endl;
    }
}

void card_deduce_strategy::execute_effect(character& attacker, character& defender, 
                                           const card& enemy_card, 
                                           bool is_adjacent, 
                                           bool attacker_won) {
    card* enemyCard = const_cast<card*>(&enemy_card);
    int boost = enemyCard->getboost();
    if (boost > 0) {
        enemyCard->setattack(boost);
        enemyCard->setdefense(boost);
        std::cout << "Deduce Strategy: Card value changed to boost value (" << boost << ")" << std::endl;
    } else {
        std::cout << "Deduce Strategy: Card has no boost, set to 0" << std::endl;
        enemyCard->setattack(0);
        enemyCard->setdefense(0);
    }
}

void card_education_never_ends::execute_effect(character& attacker, character& defender, 
                                                const card& enemy_card, 
                                                bool is_adjacent, 
                                                bool attacker_won) {
    hero* h_ptr = dynamic_cast<hero*>(&attacker);
    hero* h_def = dynamic_cast<hero*>(&defender);
    
    if (attacker_won) {
        if (h_def) {
            h_def->drawcard();
            std::cout << "Education Never Ends: Opponent draws 1 card" << std::endl;
        }
    } else {
        if (h_ptr) {
            h_ptr->drawcard();
            h_ptr->drawcard();
            std::cout << "Education Never Ends: You draw 2 cards" << std::endl;
        }
    }
}

void card_elementary::execute_effect(character& attacker, character& defender, 
                                      const card& enemy_card, 
                                      bool is_adjacent, 
                                      bool attacker_won) {
    std::cout << "Elementary: Predict opponent's attack value (1-6): ";
    int predicted;
    std::cin >> predicted;
    
    int actualAttack = enemy_card.getattack();
    if (predicted == actualAttack) {
        std::cout << "Correct prediction! All effects canceled and attack ignored." << std::endl;
        const_cast<card*>(&enemy_card)->setattack(0);
    } else {
        std::cout << "Wrong prediction! Attack value: " << actualAttack << std::endl;
    }
}

void card_eliminate_impossible::execute_effect(character& attacker, character& defender, 
                                                const card& enemy_card, 
                                                bool is_adjacent, 
                                                bool attacker_won) {
    hero* defHero = dynamic_cast<hero*>(&defender);
    if (defHero) {
        auto& hand = defHero->gethand();
        std::cout << "Eliminate the Impossible: Opponent's hand:" << std::endl;
        for (size_t i = 0; i < hand.size(); i++) {
            std::cout << "  [" << i << "] " << hand[i].get_name() << std::endl;
        }
        std::cout << "Choose a card to burn (index): ";
        int idx;
        std::cin >> idx;
        if (idx >= 0 && idx < (int)hand.size()) {
            std::cout << "Burned: " << hand[idx].get_name() << std::endl;
            hand.erase(hand.begin() + idx);
        }
    }
}

void card_feint::execute_effect(character& attacker, character& defender, 
                                 const card& enemy_card, 
                                 bool is_adjacent, 
                                 bool attacker_won) {
    std::cout << "Feint: All effects on opponent's card are cancelled!" << std::endl;
}

void card_fixed_point::execute_effect(character& attacker, character& defender, 
                                       const card& enemy_card, 
                                       bool is_adjacent, 
                                       bool attacker_won) {
    if (is_adjacent) {
        attacker.heal(1);
        defender.heal(1);
        std::cout << "Fixed Point: Healed both fighters for 1" << std::endl;
    } else {
        std::cout << "Fixed Point: No heal (not adjacent)" << std::endl;
    }
}

void card_master_of_disguise::execute_effect(character& attacker, character& defender, 
                                              const card& enemy_card, 
                                              bool is_adjacent, 
                                              bool attacker_won) {
    int tempPos = attacker.getposition();
    attacker.setposition(defender.getx());
    defender.setposition(tempPos);
    defender.takedamage(1);
    std::cout << "Master of Disguise: Swapped positions and dealt 1 damage" << std::endl;
}

void card_game_is_afoot::execute_effect(character& attacker, character& defender, 
                                         const card& enemy_card, 
                                         bool is_adjacent, 
                                         bool attacker_won) {
    hero* h = dynamic_cast<hero*>(&attacker);
    if (h) {
        Board& board = h->getBoard();
        auto neighbors = board.getNeighborIds(attacker.getx());
        if (neighbors.size() >= 3) {
            attacker.setposition(neighbors[2]);
            std::cout << "The Game is Afoot: Moved 3 spaces to n" << neighbors[2] << std::endl;
        } else {
            std::cout << "The Game is Afoot: Not enough neighbors!" << std::endl;
        }
    }
}

void card_service_revolver::execute_effect(character& attacker, character& defender, 
                                            const card& enemy_card, 
                                            bool is_adjacent, 
                                            bool attacker_won) {
    std::cout << "Service Revolver: No effect (Attack 5)" << std::endl;
}

void card_study_methods::execute_effect(character& attacker, character& defender, 
                                         const card& enemy_card, 
                                         bool is_adjacent, 
                                         bool attacker_won) {
    if (attacker_won) {
        hero* defHero = dynamic_cast<hero*>(&defender);
        if (defHero) {
            std::cout << "Study Methods: Opponent's hand:" << std::endl;
            for (const auto& c : defHero->gethand()) {
                std::cout << "  " << c.get_name() << std::endl;
            }
        }
        std::cout << "Study Methods: You may look at opponent's hand" << std::endl;
    } else {
        std::cout << "Study Methods: No effect (you lost the combat)" << std::endl;
    }
}