#include "../include/character.hpp"

character::character(string name, int maxhp, int movement)
    : name(name), maxhp(maxhp), movement(movement),
      health(maxhp), nodeId(0) {}

string character::getname() const { return name; }
int character::gethealth() const { return health; }
int character::getmovement() const { return movement; }

void character::setposition(int node) {
    nodeId = node;
}

int character::getposition() const { return nodeId; }

string character::getPositionString() const {
    return "n" + to_string(nodeId);
}

void character::takedamage(int amount) {
    health -= amount;
    if (health < 0) health = 0;
}

void character::heal(int amount) {
    health += amount;
    if (health > maxhp) health = maxhp;
}

void character::sethealth(int amount) {
    health = amount;
    if (health > maxhp) health = maxhp;
    if (health < 0) health = 0;
}

void character::setnewmovement(int newmovement) {
    movement = newmovement;
}

void character::resetmovement() {
    movement = 2;
}

bool character::isalive() const {
    return health > 0;
}