#pragma once
#include<string>
#include<utility>

using namespace std;

class character {
private:
    string name;
    int health;
    int maxhp;
    int movement;
    pair<int ,int> position;
   

public:
    character(string name, int maxhp, int movement);

    string getname() const;
    int gethealth() const;
    int getmovement() const;

    void setposition(int x, int y);
    pair<int, int> getposition() const;
    string getPositionString() const;  
    int getx() const;
    int gety() const;

    void takedamage(int amount);
    void heal(int amount);
    void sethealth(int amount);
    void setnewmovement(int newmovement);
    void resetmovement();

    bool isalive() const;
    virtual void useability() {};
};