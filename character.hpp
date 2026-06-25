#pragma once

#include<string>
#include "deck.hpp"

using namespace std;

class character {
    string name;
    int health;
    int maxhp;
    int movement;
    deck dk;

    public:
    character(string name , int maxhp , int movement);

    string getname() const;
    int gethealth() const;

    void takedamage(int amount);
    void heal(int amount);

    bool isalive() const;

    deck& getdeck();
};