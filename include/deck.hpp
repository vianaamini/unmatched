#pragma once
#include<vector>
#include "card.hpp"

using namespace std;

class deck {
    vector<card> cards;

public:
    deck();

    void addcard(const card& card);
    card drawcard();

    bool isempty() const;
    int getsize() const;

    void shuffle();
    void reset();

    const vector<card>& getCards() const;
    void setCards(const vector<card>& newCards);
};