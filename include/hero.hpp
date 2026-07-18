#pragma once

#include <vector>
#include "card.hpp"
#include "deck.hpp"
#include "map.hpp"
#include "character.hpp"

class hero : public character {

    deck dk;
    vector<card> hand;
    int actions;

    public:
    hero( const string& name, int maxhp, int movement);

    virtual ~hero() = default;
    
    void set_actions(int new_actions);

    void reset_actions();


    void drawcard();
    void drawhand();

    deck& getdeck();
    const deck& getdeck() const;

    vector<card>& gethand();
    const vector<card>& gethand() const;

    int get_actions() const;
    int handsize() const;

    virtual void useability() = 0;

    bool maneuver(int targetx , int targety , Board& board);
    bool scheme(card& scheme, hero& target);
    bool attack(hero& target, card& attackcard, Board& board);
    bool canact() const;
};