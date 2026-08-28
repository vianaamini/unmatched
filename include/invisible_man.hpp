#pragma once
#include "hero.hpp"
#include <vector>
#include <string>

class InvisibleMan : public hero {
private:
    std::vector<int> fogPositions;
    std::vector<character*>* allCharacters = nullptr;

    bool vanished = false;
    bool startedTurnOnFog = false;

public:
    InvisibleMan();

    
    
    void initializeFogTokens(int startNode);

    void setAllCharacters(std::vector<character*>* chars);

    bool isOnFog() const;
    std::vector<int> getFogPositions() const;
    void setFogPosition(int index, int node);

    void onTurnStart();
    bool isVanished() const { return vanished; }
    bool startedTurnOnFogToken() const { return startedTurnOnFog; }
    // Called once the player clicks a space in response to
    // TargetPrompt::VanishNode (see actionbar.cpp), which only opens while
    // isVanished() is still true after onTurnStart() runs. Places him
    // there, clears vanished, and records whether that counts as
    // "started this turn on a fog token" for Emerge from Mist -- mirrors
    // what onTurnStart() used to do by itself before placement needed a
    // real click.
    void resolveVanish(int node);

    void takedamage(int amount) override;
    void useability() override;

    
    
    
    
    bool executeSchemeCard(card& schemeCard, hero& target);

 
    void executeAttackCardEffects(card& attackCard, character& target, int& attackValue, int& defenseValue, bool& attackerWon, bool& effectsCanceled, const card& defenseCard, bool defenseCardProtected);
    void executeDefenseCardEffects(card& defenseCard, const card& attackCard, int& defenseValue, int& attackValue, bool& effectsCanceled, bool attackCardProtected);
};