#include "deck.hpp"
#include <algorithm>
#include <random>    // برای random_device و mt19937
#include <stdexcept> // برای runtime_error

deck::deck() {}

void deck::addcard(const card& card) {
    cards.push_back(card);
}

card deck::drawcard() {
    if (cards.empty()) {
        throw std::runtime_error("Deck is empty!");
    }
    card top = cards.back();
    cards.pop_back();
    return top;
}

bool deck::isempty() const {
    return cards.empty();
}

int deck::getsize() const {
    return cards.size();
}

void deck::shuffle() {
    random_device rd;
    mt19937 g(rd());
    std::shuffle(cards.begin(), cards.end(), g);
}