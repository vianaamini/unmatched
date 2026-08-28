#ifndef MENU_HPP
#define MENU_HPP

struct MenuResult {
    enum Type {
        Start,
        Load,
        Exit
    } type;
    int slotNumber;
};

class Menu {
public:
    MenuResult show();
};

#endif