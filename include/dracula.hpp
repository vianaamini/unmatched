#pragma once

#include "hero.hpp"
#include <vector>

class dracula : public hero
{
private:

    std::vector<character*>* allCharactersPtr;

public:

    dracula();

    void setAllCharacters(std::vector<character*>* chars);

    virtual void useability() override;

    bool useAbilityOn(character* target);
};