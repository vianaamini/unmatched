#include "sherlock_card.hpp"
#include <iostream>

void card_confirming_suspicion::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) {
    cout << "Executing Confirming Suspicion" << endl;
}

void card_administer_aid::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) {
    if (attacker.gethealth() > 0) {
        attacker.sethealth(attacker.gethealth() + 1);
    }
    hero* h_ptr = dynamic_cast<hero*>(&attacker);
    if (h_ptr) {
        h_ptr->drawcard();
    }
}

void card_counterpunch::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) {
    if (is_watson_adjacent && defender.gethealth() > 0) {
        defender.sethealth(defender.gethealth() - 2);
    }
}

void card_deduce_strategy::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) {
    if (defender.gethealth() > 0) {
        defender.sethealth(defender.gethealth() - enemy_card.getboost());
    }
}

void card_education_never_ends::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) {
    if (!sherlock_won) {
        hero* h_ptr = dynamic_cast<hero*>(&attacker);
        if (h_ptr) {
            h_ptr->drawcard();
            h_ptr->drawcard();
        }
    }
}

void card_elementary::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) {
    cout << "Executing Elementary" << endl;
}

void card_eliminate_impossible::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) {
    cout << "Executing Eliminate the Impossible" << endl;
}

void card_feint::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) {
    cout << "Executing Feint" << endl;
}

void card_fixed_point::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) {
    if (is_watson_adjacent) {
        if (attacker.gethealth() > 0) attacker.sethealth(attacker.gethealth() + 1);
        if (defender.gethealth() > 0) defender.sethealth(defender.gethealth() + 1);
    }
}

void card_master_of_disguise::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) {
    if (defender.gethealth() > 0) {
        defender.sethealth(defender.gethealth() - 1);
    }
}

void card_game_is_afoot::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) {
    cout << "Executing The Game is Afoot" << endl;
}

void card_service_revolver::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) {
    if (defender.gethealth() > 0) {
        defender.sethealth(defender.gethealth() - 5);
    }
}

void card_study_methods::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_watson_adjacent, bool sherlock_won) {
    if (sherlock_won) {
        cout << "Executing Study Methods" << endl;
    }
}