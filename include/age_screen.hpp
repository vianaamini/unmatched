#ifndef AGE_SCREEN_HPP
#define AGE_SCREEN_HPP

enum class AgeScreenResult
{
    Player1First,
    Player2First,
    EqualAge
};

class AgeScreen
{
public:
    AgeScreenResult show();
};

#endif