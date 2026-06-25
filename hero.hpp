#pragma once

#include <vector>

#include "card.hpp"
#include "deck.hpp"
#include "character.hpp"

class hero : public character {

    deck dk;
    vector<card> hand;

    public:
    hero( const string& name, int maxhp, int movement);

    virtual ~hero() = default;

    deck& getdeck();
    const deck& getdeck() const;

    vector<card>& gethand();
    const vector<card>& gethand() const;

    void drawcard();

    int handsize() const;

    virtual void useability() = 0;
};