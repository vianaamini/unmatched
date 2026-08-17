#include "../include/invisible_man.hpp"
#include "../include/deck-builder.hpp"
#include <iostream>

InvisibleMan::InvisibleMan(std::string name, int startNode)
    : hero(name, 15, 2)
{
    this->setposition(startNode);
    fogPositions = {startNode, startNode + 1, startNode + 2};
    initializeDeck();
}

void InvisibleMan::initializeDeck() {
    deck d = invisiblemandeck();
    d.shuffle();

    while (!d.isempty()) {
        getdeck().addcard(d.drawcard());
    }

    drawhand();
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

bool InvisibleMan::executeSchemeCard(card& schemeCard, hero& target) {
    std::string name = schemeCard.get_name();

    if (name == "Covert Preparation") {
        drawcard();
        std::cout << getname() << " drew 1 card." << std::endl;
        return true;
    }
    else if (name == "Rolling Fog") {
        std::cout << getname() << " repositioned a fog token and gains 1 action." << std::endl;
        return true;
    }
    else if (name == "Reign of Terror") {
        if (isOnFog()) {
            target.takedamage(2);
            std::cout << getname() << " dealt 2 shadow damage from fog to " << target.getname() << "!" << std::endl;
        }
        return true;
    }
    else if (name == "Step Lightly") {
        int dmg = isOnFog() ? 3 : 1;
        target.takedamage(dmg);
        std::cout << getname() << " dealt " << dmg << " damage to " << target.getname() << "!" << std::endl;
        return true;
    }
    else if (name == "Vanish") {
        heal(1);
        std::cout << getname() << " recovered 1 HP and vanished from the board." << std::endl;
        return true;
    }
    return false;
}

void InvisibleMan::executeAttackCardEffects(card& attackCard, character& target, int& attackValue, bool& attackerWon, bool& effectsCanceled, const card& defenseCard) {
    std::string name = attackCard.get_name();

    if (name == "Emerge from Mist") {
        if (isOnFog()) {
            attackValue = 5;
            std::cout << getname() << " strikes from the fog with extra power (ATK: 5)!" << std::endl;
        }
    }
    else if (name == "Slip Away") {
        if (!fogPositions.empty()) {
            setposition(fogPositions[0]);
            std::cout << getname() << " moved to fog space " << fogPositions[0] << std::endl;
        }
    }
}

void InvisibleMan::executeDefenseCardEffects(card& defenseCard, const card& attackCard, int& defenseValue, bool& effectsCanceled) {
    std::string name = defenseCard.get_name();

    if (name == "Coded Notes") {
        std::cout << getname() << " draws 3 and reorders the top of their deck (Coded Notes)." << std::endl;
    }
    else if (name == "Into Thin Air") {
        if (!fogPositions.empty()) {
            std::cout << getname() << " moves 1 space, then a fog token moves 3 spaces (Into Thin Air)." << std::endl;
        }
    }
    else if (name == "Lurking") {
        drawcard();
        std::cout << getname() << " draws 1 card (Lurking)." << std::endl;
    }
}
