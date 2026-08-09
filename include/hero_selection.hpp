#pragma once

#include <string>

enum class HeroSelectionResultType
{
    EnterGame,
    Retreat
};

struct HeroSelectionResult
{
    HeroSelectionResultType type;
    int firstPlayer;
    std::string player1Hero;
    std::string player2Hero;
};

class HeroSelection
{
public:
    HeroSelectionResult show(int firstPlayer);
};