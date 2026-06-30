#include "../include/hero.hpp"

hero::hero(const string& name, int maxhp, int movement) : character(name, maxhp, movement) {}

deck& hero::getdeck() {return dk;}
const deck& hero::getdeck() const {return dk;}

vector<card>& hero::gethand() {return hand;}
const vector<card>& hero::gethand() const {return hand;}

int hero::handsize() const {
    return hand.size();
}

void hero::drawcard() {
    if (dk.getsize() > 0) {
        hand.push_back(dk.drawcard());
    }
}

int hero::get_actions() const {
    return actions;
}

void hero::set_actions(int new_actions) {
    actions = new_actions;
}

void hero::reset_actions() {
    actions = 2;
}