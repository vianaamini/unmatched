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
    void set_actions(int new_actions);

    void reset_actions();


    void drawcard();


    deck& getdeck();
    const deck& getdeck() const;

    vector<card>& gethand();
    const vector<card>& gethand() const;
     int get_actions() const;

    int handsize() const;

    virtual void useability() = 0;
};