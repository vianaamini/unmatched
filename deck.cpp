#include "deck.hpp"
#include <vector>

deck::deck() {};

void deck::addcard(const card& card) {
    cards.push_back(card);
}

card deck::drawcard() {
    card top = cards.back();
    cards.pop_back();
    return top;
} 

bool deck::isempty() const{
    return cards.empty();
}

int deck::getsize() const{
    return cards.size();
}