#include "../include/dracula.hpp"
#include "../include/deck-builder.hpp"
#include "../include/hero.hpp"
#include "../include/card.hpp"
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
