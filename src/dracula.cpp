#include "../include/dracula.hpp"
#include "../include/deck-builder.hpp"
#include <iostream>

dracula::dracula() : hero("Dracula", 13, 2) {
    deck draculadk = draculadeck();
    draculadk.shuffle();
    
    while (!draculadk.isempty()) {
        getdeck().addcard(draculadk.drawcard());
    }
}

void dracula::useability() {
    std::cout << "Dracula ability activated" << std::endl;
}