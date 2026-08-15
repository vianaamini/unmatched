#include "../include/invisible_man_cards.hpp"
#include "../include/invisible_man.hpp"
#include <iostream>

void card_impossible_to_catch::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_adjacent, bool attacker_won) {
    auto* invMan = dynamic_cast<InvisibleMan*>(&attacker);
    if (invMan) {
        invMan->setFogPosition(0, invMan->getposition());
        std::cout << invMan->getname() << " moved a fog token to space " << invMan->getposition() << std::endl;
    }
}

void card_vanish::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_adjacent, bool attacker_won) {
    auto* invMan = dynamic_cast<InvisibleMan*>(&attacker);
    if (invMan) {
        invMan->heal(1);
        auto fogs = invMan->getFogPositions();
        if (!fogs.empty()) {
            invMan->setposition(fogs[0]);
            std::cout << invMan->getname() << " recovered 1 HP and vanished to fog space " << fogs[0] << std::endl;
        }
    }
}

void card_emerging_from_mist::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_adjacent, bool attacker_won) {
    auto* invMan = dynamic_cast<InvisibleMan*>(&attacker);
    if (invMan && invMan->isOnFog()) {
        std::cout << invMan->getname() << " strikes from the fog with extra power!" << std::endl;
    }
}

void card_covert_preparation::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_adjacent, bool attacker_won) {
    auto* invMan = dynamic_cast<InvisibleMan*>(&attacker);
    if (invMan) {
        invMan->drawcard();
        invMan->drawcard();
        std::cout << invMan->getname() << " drew 2 cards." << std::endl;
    }
}

void card_slip_away::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_adjacent, bool attacker_won) {
    auto* invMan = dynamic_cast<InvisibleMan*>(&attacker);
    if (invMan) {
        auto fogs = invMan->getFogPositions();
        if (!fogs.empty()) {
            invMan->setposition(fogs[0]);
            std::cout << invMan->getname() << " slipped away to fog space " << fogs[0] << std::endl;
        }
    }
}

void card_fog_vision::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_adjacent, bool attacker_won) {
    auto* invMan = dynamic_cast<InvisibleMan*>(&attacker);
    if (invMan) {
        invMan->setFogPosition(1, invMan->getposition());
        std::cout << invMan->getname() << " repositioned a fog token." << std::endl;
    }
}

void card_into_thin_air::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_adjacent, bool attacker_won) {
    std::cout << attacker.getname() << " cancels opponent card effects with Into Thin Air!" << std::endl;
}

void card_lurking_in_shadows::execute_effect(character& attacker, character& defender, const card& enemy_card, bool is_adjacent, bool attacker_won) {
    auto* invMan = dynamic_cast<InvisibleMan*>(&attacker);
    if (invMan && invMan->isOnFog()) {
        defender.takedamage(1);
        std::cout << invMan->getname() << " dealt 1 shadow damage from fog to " << defender.getname() << "!" << std::endl;
    }
}