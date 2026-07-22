#include "../include/dracula.hpp"
#include "../include/deck-builder.hpp"
#include "../include/deracula_cards.hpp"
#include <iostream>
#include <vector>

dracula::dracula() : hero("Dracula", 13, 2), allCharactersPtr(nullptr) {
    deck draculadk = draculadeck();
    draculadk.shuffle();
    
    while (!draculadk.isempty()) {
        getdeck().addcard(draculadk.drawcard());
    }
}

void dracula::useability() {
    std::cout << "=== Dracula Ability ===" << std::endl;
    std::cout << "Choose an adjacent fighter to deal 1 damage and draw a card." << std::endl;
    std::cout << "Enter target node (n1-n32) or 'skip': ";
    string input;
    cin >> input;
    
    if (input == "skip") {
        std::cout << "Ability skipped." << std::endl;
        return;
    }
    
    if (input.empty() || input[0] != 'n') {
        std::cout << "Invalid target!" << std::endl;
        return;
    }
    
    try {
        int targetId = std::stoi(input.substr(1));
        character* target = nullptr;
        
        if (allCharactersPtr) {
            for (character* c : *allCharactersPtr) {
                if (c->getx() == targetId && c->isalive() && c != this) {
                    target = c;
                    break;
                }
            }
        }
        
        if (!target) {
            std::cout << "No character at node " << input << "!" << std::endl;
            return;
        }
        
        if (!dcards::are_adjacent(*this, *target)) {
            std::cout << "Target is not adjacent to Dracula!" << std::endl;
            return;
        }
        
        target->takedamage(1);
        drawcard();
        std::cout << "Dealt 1 damage to " << target->getname() << " and drew a card." << std::endl;
        
    } catch (...) {
        std::cout << "Invalid target!" << std::endl;
    }
}