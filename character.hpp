#pragma once

#include<string>
#include<utility>

using namespace std;

class character {
    string name;
    int health;
    int maxhp;
    int movement;
    pair<int ,int> position;
    int current_combat_attack;
    int current_combat_defense;

    public:
    character(string name , int maxhp , int movement);
    int get_combat_attack() const;
    void set_combat_attack(int value);
    void reset_combat_values();
    string getname() const;
    int gethealth() const;

    void setposition(int x ,int y);
    pair<int ,int> getposition() const;
    int getx() const;
    int gety() const;

    void takedamage(int amount);
    void heal(int amount);

    bool isalive() const;
    virtual void useability() {};
};