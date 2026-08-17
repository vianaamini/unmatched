#pragma once
#include "hero.hpp"
#include <vector>
#include <string>

// Invisible Man - solo fighter (no sidekicks), built the same way as
// dracula/sherlock: constructor loads invisiblemandeck() from
// deck-builder.hpp, shuffles it into the hero's deck, then draws an
// opening hand of 5. Position is set externally via setposition(), same
// as every other character (see main.cpp).
class InvisibleMan : public hero {
private:
    std::vector<int> fogPositions;
    std::vector<character*>* allCharacters = nullptr;

public:
    InvisibleMan();

    // Call once after setposition() to lay the 3 starting fog tokens
    // around the Invisible Man's starting space.
    void initializeFogTokens(int startNode);

    void setAllCharacters(std::vector<character*>* chars);

    bool isOnFog() const;
    std::vector<int> getFogPositions() const;
    void setFogPosition(int index, int node);

    void takedamage(int amount) override;
    void useability() override;

    // Called from hero::scheme() for scheme-type cards. Returns true if
    // the card name was recognized and handled (caller then does
    // actions-- and returns true); returns false for any card name it
    // doesn't own, so hero::scheme()'s normal chain still applies.
    bool executeSchemeCard(card& schemeCard, hero& target);

    // Called from hero::attack() for both the attacker's card (when this
    // Invisible Man is attacking) and the defender's card (when this
    // Invisible Man is defending) - mirrors the inline Dracula/Sherlock
    // card checks already living in hero::attack().
    void executeAttackCardEffects(card& attackCard, character& target, int& attackValue, bool& attackerWon, bool& effectsCanceled, const card& defenseCard);
    void executeDefenseCardEffects(card& defenseCard, const card& attackCard, int& defenseValue, bool& effectsCanceled);
};