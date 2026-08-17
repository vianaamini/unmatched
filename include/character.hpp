#pragma once
#include <string>

using namespace std;

class character {
private:
    string name;
    int health;
    int maxhp;
    int movement;
    int nodeId;

public:
    character(string name, int maxhp, int movement);
    virtual ~character() = default;

    string getname() const;
    int gethealth() const;
    int getMaxHp() const;
    int getmovement() const;
    int getposition() const;
    int getx() const;

    void setposition(int node);

    virtual void takedamage(int amount);
    void heal(int amount);
    void sethealth(int amount);

    bool isalive() const;

    void setnewmovement(int newmovement);
    void resetmovement();

    std::string getPositionString() const;

    virtual void useability() {}
};