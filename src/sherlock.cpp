#include "../include/sherlock.hpp"
#include "../include/deck-builder.hpp"
#include <iostream>

sherlock::sherlock() : hero("Sherlock Holmes", 16, 2) {
    deck sherlockdk = sherlockdeck();
    sherlockdk.shuffle();

    while(!sherlockdk.isempty()) {
        getdeck().addcard(sherlockdk.drawcard());
    }
}

void sherlock::useability() {
    cout << "Sherlock ability activated" << endl;
}
