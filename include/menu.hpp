#ifndef MENU_HPP
#define MENU_HPP

enum class MenuResult
{
    Start,
    Load,
    Exit
};

class Menu
{
public:
    MenuResult show();
};

#endif