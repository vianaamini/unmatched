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
}

void card_confirm_suspicion::execute_effect(character& attacker, character& defender, 
                                             const card& enemy_card, 
                                             bool is_adjacent, 
                                             bool attacker_won) {
    std::cout << "Confirm Suspicion: Choose a value" << std::endl;
}

void card_counter_punch::execute_effect(character& attacker, character& defender, 
                                         const card& enemy_card, 
                                         bool is_adjacent, 
                                         bool attacker_won) {
    if (is_adjacent) {
        defender.takedamage(2);
    }
}

void card_deduce_strategy::execute_effect(character& attacker, character& defender, 
                                           const card& enemy_card, 
                                           bool is_adjacent, 
                                           bool attacker_won) {
    std::cout << "Deduce Strategy: Card value changed to boost" << std::endl;
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
        }
    } else {
        if (h_ptr) {
            h_ptr->drawcard();
            h_ptr->drawcard();
        }
    }
}

void card_elementary::execute_effect(character& attacker, character& defender, 
                                      const card& enemy_card, 
                                      bool is_adjacent, 
                                      bool attacker_won) {
    std::cout << "Elementary: Predict opponent's attack" << std::endl;
}

void card_eliminate_impossible::execute_effect(character& attacker, character& defender, 
                                                const card& enemy_card, 
                                                bool is_adjacent, 
                                                bool attacker_won) {
    std::cout << "Eliminate the Impossible: Look at opponent's hand" << std::endl;
}

void card_feint::execute_effect(character& attacker, character& defender, 
                                 const card& enemy_card, 
                                 bool is_adjacent, 
                                 bool attacker_won) {
    std::cout << "Feint: Effects cancelled" << std::endl;
}

void card_fixed_point::execute_effect(character& attacker, character& defender, 
                                       const card& enemy_card, 
                                       bool is_adjacent, 
                                       bool attacker_won) {
    if (is_adjacent) {
        attacker.heal(1);
        defender.heal(1);
    }
}

void card_master_of_disguise::execute_effect(character& attacker, character& defender, 
                                              const card& enemy_card, 
                                              bool is_adjacent, 
                                              bool attacker_won) {
    std::pair<int, int> tempPos = attacker.getposition(); 
    attacker.setposition(defender.getx(), defender.gety());
    defender.setposition(tempPos.first, tempPos.second);
    defender.takedamage(1);
}

void card_game_is_afoot::execute_effect(character& attacker, character& defender, 
                                         const card& enemy_card, 
                                         bool is_adjacent, 
                                         bool attacker_won) {
    int newX = attacker.getx() + 3;
    int newY = attacker.gety();
    attacker.setposition(newX, newY);
}

void card_service_revolver::execute_effect(character& attacker, character& defender, 
                                            const card& enemy_card, 
                                            bool is_adjacent, 
                                            bool attacker_won) {
    // No effect
}

void card_study_methods::execute_effect(character& attacker, character& defender, 
                                         const card& enemy_card, 
                                         bool is_adjacent, 
                                         bool attacker_won) {
    if (attacker_won) {
        std::cout << "Study Methods: You may look at opponent's hand" << std::endl;
    }
}