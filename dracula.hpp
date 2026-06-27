#pragma once
#include "hero.hpp"

class dracula : public hero {
    public:
    dracula();

    void useability() override;
    void resolvecard(const card& played_card, hero& opponent);
};