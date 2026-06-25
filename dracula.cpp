#include "dracula.hpp"
#include <iostream>

dracula::dracula() : hero("Dracula", 13 , 2) {}

void dracula::useability() {
    std::cout << "dracula ability activated" << std::endl;
}

