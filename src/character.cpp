#include "../include/character.hpp"

character::character (string name , int maxhp , int movement) :
    name(name) , maxhp(maxhp) , movement(movement) ,
     health(maxhp) , position({0 , 0}) {}

string character::getname() const{return name;}
int character::gethealth() const{return health;}
//int character::getmovement() const{return movement;}

void character::setposition(int x ,int y) {
    position.first = x;
    position.second = y;
}

pair<int ,int> character::getposition() const {return position;}

int character::getx() const {return position.first;}
int character::gety() const {return position.second;}

void character::takedamage(int amount) {
    health -= amount;

    if(health < 0) health = 0;
}

void character::heal(int amount) {
    health += amount;

    if(health > maxhp ) health = maxhp;
}

bool character::isalive() const{return health>0;}
