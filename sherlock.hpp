#pragma once

#include "hero.hpp"

class sherlock : public hero {
    public:
        sherlock();

        void useability() override;
};