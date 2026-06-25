#include "character.hpp"

character::character (string name , int maxhp , int movement) :
    name(name) , maxhp(maxhp) , movement(movement) {}

string character::getname() const{return name;}
int character::gethealth() const{return health;}
//int character::getmovement() const{return movement;}

void character::takedamage(int amount) {
    health -= amount;

    if(health < 0) health = 0;
}

void character::heal(int amount) {
    health += amount;

    if(health > maxhp ) health = maxhp;
}

bool character::isalive() const{return health>0;}

deck& character::getdeck() {return dk;}