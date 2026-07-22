#pragma once
#include "hero.hpp"
#include <vector>

class dracula : public hero {
private:
    std::vector<character*>* allCharactersPtr = nullptr;

public:
    dracula();
    void useability() override;
    void setAllCharacters(std::vector<character*>* chars) { allCharactersPtr = chars; }
};