#include "../include/character.hpp"

character::character(string name, int maxhp, int movement)
    : name(name), health(maxhp), maxhp(maxhp), movement(movement), baseMovement(movement), nodeId(0) {}

string character::getname() const { return name; }
int character::gethealth() const { return health; }
int character::getMaxHp() const { return maxhp; }
int character::getmovement() const { return movement; }
int character::getposition() const { return nodeId; }
int character::getx() const { return nodeId; }

void character::setposition(int node) { nodeId = node; }

void character::takedamage(int amount) {
    if (amount <= 0) return;
    health -= amount;
    if (health < 0) health = 0;
}

void character::heal(int amount) {
    if (amount <= 0) return;
    health += amount;
    if (health > maxhp) health = maxhp;
}

void character::sethealth(int amount) {
    health = amount;
    if (health < 0) health = 0;
    if (health > maxhp) health = maxhp;
}

bool character::isalive() const { return health > 0; }

void character::setnewmovement(int newmovement) { movement = newmovement; }

void character::resetmovement() { movement = baseMovement; }

std::string character::getPositionString() const {
    return "n" + std::to_string(nodeId);
}