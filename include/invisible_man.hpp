#pragma once
#include "hero.hpp"
#include <vector>
#include <string>






class InvisibleMan : public hero {
private:
    std::vector<int> fogPositions;
    std::vector<character*>* allCharacters = nullptr;

public:
    InvisibleMan();

    
    
    void initializeFogTokens(int startNode);

    void setAllCharacters(std::vector<character*>* chars);

    bool isOnFog() const;
    std::vector<int> getFogPositions() const;
    void setFogPosition(int index, int node);

    void takedamage(int amount) override;
    void useability() override;

    
    
    
    
    bool executeSchemeCard(card& schemeCard, hero& target);

    
    
    
    
    void executeAttackCardEffects(card& attackCard, character& target, int& attackValue, int& defenseValue, bool& attackerWon, bool& effectsCanceled, const card& defenseCard);
    void executeDefenseCardEffects(card& defenseCard, const card& attackCard, int& defenseValue, int& attackValue, bool& effectsCanceled);
};