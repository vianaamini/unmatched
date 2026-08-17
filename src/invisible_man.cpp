#include "../include/invisible_man.hpp"
#include "../include/deck-builder.hpp"
#include <iostream>
#include <algorithm>

InvisibleMan::InvisibleMan()
    : hero("Invisible Man", 14, 2)
{
    deck d = invisiblemandeck();
    d.shuffle();

    while (!d.isempty()) {
        getdeck().addcard(d.drawcard());
    }

    drawhand();
}

void InvisibleMan::initializeFogTokens(int startNode) {
    fogPositions = { startNode, startNode + 1, startNode + 2 };
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

    if (name == "Reign of Terror") {
        if (isOnFog()) {
            target.takedamage(2);
            std::cout << getname() << " dealt 2 damage to " << target.getname() << " from the fog!" << std::endl;
        } else {
            std::cout << "Reign of Terror: " << getname() << " must be on a fog token." << std::endl;
        }
        return true;
    }

    if (name == "Rolling Fog") {
        if (!fogPositions.empty()) {
            setFogPosition(0, getposition());
            std::cout << getname() << " rolled the fog to n" << getposition() << "." << std::endl;
        }
        return true;
    }

    if (name == "Step Lightly") {
        if (allCharacters) {
            for (character* c : *allCharacters) {
                if (!c || c == this || !c->isalive()) continue;
                Board* b = getBoard();
                if (b && b->isAdjacent(getposition(), c->getx())) {
                    int dmg = isOnFog() ? 3 : 1;
                    c->takedamage(dmg);
                    std::cout << getname() << " dealt " << dmg << " damage to " << c->getname() << " with Step Lightly." << std::endl;
                    break;
                }
            }
        }
        if (!fogPositions.empty()) {
            setFogPosition(fogPositions.size() > 1 ? 1 : 0, getposition());
        }
        return true;
    }

    if (name == "Vanish") {
        heal(1);
        if (!fogPositions.empty()) {
            setposition(fogPositions[0]);
            std::cout << getname() << " recovered 1 HP and vanished to n" << fogPositions[0] << "." << std::endl;
        }
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
            int dest = fogPositions[0];
            setFogPosition(0, target.getx());
            setposition(dest);
            std::cout << getname() << " slipped away to n" << dest << "." << std::endl;
        }
    }
    else if (name == "Confound") {
        hero* targetHero = dynamic_cast<hero*>(&target);
        if (targetHero && !targetHero->gethand().empty()) {
            auto& enemyHand = targetHero->gethand();
            enemyHand.pop_back();
            std::cout << targetHero->getname() << " discarded a card (Confound)." << std::endl;
        } else if (!fogPositions.empty()) {
            setFogPosition(0, getposition());
            std::cout << getname() << " moved a fog token (Confound)." << std::endl;
        }
    }
    else if (name == "Covert Preparation") {
        drawcard();
        if (fogPositions.size() > 1) {
            setFogPosition(1, getposition());
        }
        std::cout << getname() << " drew a card and shifted the fog." << std::endl;
    }
    else if (name == "Dreaming of Revenge") {
        if (isOnFog() && allCharacters) {
            for (character* c : *allCharacters) {
                if (!c || c == this || !c->isalive()) continue;
                for (int fogPos : fogPositions) {
                    if (c->getx() == fogPos) {
                        c->takedamage(1);
                        std::cout << c->getname() << " took 1 damage (Dreaming of Revenge)." << std::endl;
                        break;
                    }
                }
            }
        }
    }
    else if (name == "Impossible to See") {
        attackValue = 0;
        effectsCanceled = true;
        std::cout << getname() << " renders the attack invisible (value: 0)!" << std::endl;
    }
}

void InvisibleMan::executeDefenseCardEffects(card& defenseCard, const card& attackCard, int& defenseValue, bool& effectsCanceled) {
    std::string name = defenseCard.get_name();

    if (name == "Coded Notes") {
        drawcard();
        drawcard();
        drawcard();
        std::cout << getname() << " drew 3 cards with Coded Notes." << std::endl;
    }
    else if (name == "Into Thin Air") {
        if (!fogPositions.empty()) {
            setFogPosition(0, getposition());
        }
        std::cout << getname() << " fades into thin air." << std::endl;
    }
    else if (name == "Lurking") {
        drawcard();
        if (!fogPositions.empty()) {
            setposition(fogPositions[0]);
            std::cout << getname() << " drew a card and moved to n" << fogPositions[0] << "." << std::endl;
        }
    }
    else if (name == "Impossible to See") {
        defenseValue = 0;
        effectsCanceled = true;
        std::cout << getname() << " renders the defense irrelevant (value: 0)!" << std::endl;
    }
}