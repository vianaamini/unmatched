#include "sherlock.hpp"
#include "deck-builder.hpp"
#include <iostream>

sherlock::sherlock() : hero("Sherlock holmes", 16, 2) {
    deck sherlockdk = sherlockdeck();
    sherlockdk.shuffle();

    while(!sherlockdk.isempty()) {
        getdeck().addcard(sherlockdk.drawcard());
    }
}

void sherlock::useability() {
    cout << "Sherlock ability activated" << endl;
}
