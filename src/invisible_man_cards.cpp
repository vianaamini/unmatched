#include "../include/invisible_man_cards.hpp"
#include "../include/hero.hpp"
#include <iostream>

void card_vanish::execute_effect(character& attacker, character& defender) {
    attacker.heal(1);
    attacker.setposition(-1);
    std::cout << "Vanish: Healed 1 HP and removed Invisible Man from the board." << std::endl;
}

void card_reign_of_terror::execute_effect(character& attacker, character& defender, bool is_on_fog) {
    if (is_on_fog) {
        defender.takedamage(2);
        std::cout << "Reign of Terror: Dealt 2 damage to opponent." << std::endl;
    } else {
        std::cout << "Reign of Terror: No effect (not on fog space)." << std::endl;
    }
}

void card_rolling_fog::execute_effect(character& attacker) {
    hero* h_ptr = dynamic_cast<hero*>(&attacker);
    if (h_ptr) {
        h_ptr->set_actions(h_ptr->get_actions() + 1);
    }
    std::cout << "Rolling Fog: Move 1 fog token and gained 1 action." << std::endl;
}

void card_step_lightly::execute_effect(character& attacker, character& defender, bool is_on_fog) {
    int damage = is_on_fog ? 3 : 1;
    defender.takedamage(damage);
    std::cout << "Step Lightly: Dealt " << damage << " damage. Opponent moves a fog token up to 2 spaces." << std::endl;
}

void card_confound::execute_effect(character& attacker, character& defender) {
    std::cout << "Confound: Opponent may choose to discard 1 card. Otherwise you move all fog tokens." << std::endl;
}

void card_covert_preparation::execute_effect(character& attacker) {
    hero* h_ptr = dynamic_cast<hero*>(&attacker);
    if (h_ptr) {
        h_ptr->drawcard();
    }
    std::cout << "Covert Preparation: Drew 1 card. Move 1 fog token (you) and 1 fog token (opponent)." << std::endl;
}

void card_dreaming_of_revenge::execute_effect(character& attacker, character& defender, bool is_on_fog, bool enemy_on_fog) {
    if (is_on_fog && enemy_on_fog) {
        defender.takedamage(1);
        std::cout << "Dreaming of Revenge: Dealt 1 damage to opponent on fog." << std::endl;
    } else {
        std::cout << "Dreaming of Revenge: No damage dealt." << std::endl;
    }
}

void card_impossible_to_see::execute_effect(const card& enemy_card) {
    card* enemyCard = const_cast<card*>(&enemy_card);
    enemyCard->setattack(0);
    enemyCard->setdefense(0);
    std::cout << "Impossible to See: Opponent's card value reduced to 0!" << std::endl;
}

void card_coded_notes::execute_effect(character& attacker) {
    hero* h_ptr = dynamic_cast<hero*>(&attacker);
    if (h_ptr) {
        h_ptr->drawcard();
        h_ptr->drawcard();
        h_ptr->drawcard();
        std::cout << "Coded Notes: Drew 3 cards. Choose 2 cards to place back on top of deck." << std::endl;
    }
}

void card_into_thin_air::execute_effect(character& attacker) {
    std::cout << "Into Thin Air: Move Invisible Man up to 1 space and move a fog token up to 3 spaces." << std::endl;
}

void card_lurking::execute_effect(character& attacker) {
    hero* h_ptr = dynamic_cast<hero*>(&attacker);
    if (h_ptr) {
        h_ptr->drawcard();
    }
    std::cout << "Lurking: Drew 1 card. Choose effect: move Invisible Man to fog OR move 1 fog up to 3 spaces." << std::endl;
}

void card_emerge_from_mist::execute_effect(card& my_card, bool started_on_fog) {
    if (started_on_fog) {
        my_card.setattack(5);
        std::cout << "Emerge From Mist: Attack value boosted to 5!" << std::endl;
    }
}

void card_slip_away::execute_effect(character& attacker) {
    std::cout << "Slip Away: Move 1 fog token to empty space, then move Invisible Man there." << std::endl;
}