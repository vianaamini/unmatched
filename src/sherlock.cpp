#include "../include/sherlock.hpp"
#include "../include/deck-builder.hpp"
#include <iostream>

sherlock::sherlock() : hero("Sherlock Holmes", 16, 2) {
    deck sherlockdk = sherlockdeck();
    sherlockdk.shuffle();

    while (!sherlockdk.isempty()) {
        getdeck().addcard(sherlockdk.drawcard());
    }

    this->drawhand();
}

void sherlock::useability() {
    std::cout << "=== Sherlock Ability ===" << std::endl;
    std::cout << "Cards related to Watson or Sherlock cannot be canceled by other cards." << std::endl;
}