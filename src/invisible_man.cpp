#include "../include/invisible_man.hpp"
#include "../include/invisible_man_cards.hpp"
#include <iostream>

InvisibleMan::InvisibleMan(std::string name, int startNode)
    : hero(name, 15, 2)
{
    this->setposition(startNode);
    fogPositions = {startNode, startNode + 1, startNode + 2};
    initializeDeck();
}

void InvisibleMan::initializeDeck() {
    deck& d = getdeck();

    for (int i = 0; i < 3; ++i) {
        d.addcard(card("Impossible to Catch", cardtype::multipurpose, 2, 2, 2, cardowner::any, "Move a fog token to any space."));
        d.addcard(card("Vanish", cardtype::defense, 0, 3, 1, cardowner::any, "Recover 1 health and move to any fog token."));
        d.addcard(card("Emerging from Mist", cardtype::attack, 3, 0, 2, cardowner::any, "If on fog, attack value is 5."));
        d.addcard(card("Covert Preparation", cardtype::scheme, 0, 0, 1, cardowner::any, "Draw 2 cards."));
    }

    for (int i = 0; i < 2; ++i) {
        d.addcard(card("Slip Away", cardtype::multipurpose, 3, 3, 3, cardowner::any, "Place Invisible Man on any fog token."));
        d.addcard(card("Fog Vision", cardtype::attack, 3, 0, 1, cardowner::any, "Move a fog token."));
        d.addcard(card("Into Thin Air", cardtype::defense, 0, 4, 2, cardowner::any, "Cancel all effects on opponent card."));
        d.addcard(card("Lurking in Shadows", cardtype::scheme, 0, 0, 2, cardowner::any, "Deal 1 damage to an adjacent enemy on fog."));
    }

    d.shuffle();
}

void InvisibleMan::setAllCharacters(std::vector<character*>* chars) {
    allCharacters = chars;
}

bool InvisibleMan::isOnFog() const {
    int currentPos = getposition();
    for (int fogPos : fogPositions) {
        if (currentPos == fogPos) {
            return true;
        }
    }
    return false;
}

std::vector<int> InvisibleMan::getFogPositions() const {
    return fogPositions;
}

void InvisibleMan::setFogPosition(int index, int node) {
    if (index >= 0 && index < static_cast<int>(fogPositions.size())) {
        fogPositions[index] = node;
    }
}

void InvisibleMan::takedamage(int amount) {
    if (isOnFog() && amount > 0) {
        amount -= 1;
        std::cout << getname() << " absorbed 1 damage using Fog Bonus!" << std::endl;
    }
    character::takedamage(amount);
}

void InvisibleMan::useability() {
    if (isOnFog()) {
        std::cout << getname() << " is on a fog token (+1 Defense Bonus active)!" << std::endl;
    }
}