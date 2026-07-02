#pragma once

#include "character.hpp"

class sidekick : public character {
    public:
        sidekick(const std::string& name, int maxhp, int movement);

        virtual ~sidekick() = default;

        void useability() override;
};