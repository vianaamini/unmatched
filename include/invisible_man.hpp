#pragma once
#include "hero.hpp"
#include <vector>
#include <string>

class InvisibleMan : public hero {
private:
    std::vector<int> fogPositions;
    std::vector<character*>* allCharacters = nullptr;

public:
    InvisibleMan(std::string name, int startNode);

    void initializeDeck();
    void setAllCharacters(std::vector<character*>* chars);
    bool isOnFog() const;
    std::vector<int> getFogPositions() const;
    void setFogPosition(int index, int node);

    void takedamage(int amount);
    void useability() override;

    bool executeSchemeCard(card& schemeCard, hero& target);
    void executeAttackCardEffects(card& attackCard, character& target, int& attackValue, bool& attackerWon, bool& effectsCanceled, const card& defenseCard);
    void executeDefenseCardEffects(card& defenseCard, const card& attackCard, int& defenseValue, bool& effectsCanceled);
};