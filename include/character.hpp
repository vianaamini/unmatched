#pragma once
#include <string>
#include <utility>

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

    string getname() const;
    int gethealth() const;
    int getmovement() const;
    int getMaxHp() const { return maxhp; }

    void setposition(int node);
    int getposition() const;
    string getPositionString() const;
    int getx() const { return nodeId; }
    int gety() const { return 0; }

    void takedamage(int amount);
    void heal(int amount);
    void sethealth(int amount);
    void setnewmovement(int newmovement);
    void resetmovement();

    bool isalive() const;
    virtual void useability() {};
};