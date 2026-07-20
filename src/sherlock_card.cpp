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
    std::cout << "Confirm Suspicion: Choose a value (1-6)" << std::endl;
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
    std::cout << "Deduce Strategy: Card value changed to boost value" << std::endl;
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
    std::cout << "Elementary: Predict opponent's attack value (1-6)" << std::endl;
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
    std::cout << "The Game is Afoot: Movement handled by Game" << std::endl;
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
        std::cout << "Study Methods: You may look at opponent's hand" << std::endl;
    } else {
        std::cout << "Study Methods: No effect (you lost the combat)" << std::endl;
    }
}