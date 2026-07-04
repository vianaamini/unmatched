#include "dracula.hpp"
#include "deck-builder.hpp"
#include <iostream>

dracula::dracula() : hero("Dracula", 13 , 2) {
    deck draculadk = draculadeck();
    draculadk.shuffle();
    
    while (!draculadk.isempty()) {
        getdeck().addcard(draculadk.drawcard());
    }
}

void dracula::useability() {
    std::cout << "dracula ability activated" << std::endl;
}

