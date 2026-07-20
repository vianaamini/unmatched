#include "../include/hero.hpp"
#include <queue>
#include <unordered_set>
#include <iostream>

hero::hero(const string& name, int maxhp, int movement) 
    : character(name, maxhp, movement), actions(2) {}

deck& hero::getdeck() { return dk; }
const deck& hero::getdeck() const { return dk; }

vector<card>& hero::gethand() { return hand; }
const vector<card>& hero::gethand() const { return hand; }

int hero::handsize() const { return hand.size(); }

void hero::drawcard() {
    if (dk.getsize() > 0) {
        hand.push_back(dk.drawcard());
    }
}

void hero::drawhand() {
    for (int i = 0; i < 5; i++) {
        drawcard();
    }
}

int hero::get_actions() const { return actions; }
void hero::set_actions(int new_actions) { actions = new_actions; }
void hero::reset_actions() { actions = 2; }

void hero::useAction() { 
    if (actions > 0) actions--; 
}

bool hero::maneuver(int targetNode, Board& board) {
    if (actions <= 0) return false;
    if (!board.hasSpace("n" + to_string(targetNode))) return false;
    
    int startNode = getx();
    if (startNode == targetNode) return false;
    
    string startName = "n" + to_string(startNode);
    string targetName = "n" + to_string(targetNode);
    
    bool isConnected = board.isConnected(startName, targetName);
    bool isTeleport = board.isTeleport(startName) && 
                      board.getTeleportDestination(startName) == targetName;
    
    if (!isConnected && !isTeleport) {
        return false;
    }
    
    setposition(targetNode);
    drawcard();
    actions--;
    return true;
}

bool hero::scheme(card& schemeCard, hero& target) {
    if (actions <= 0) return false;
    if (schemeCard.gettype() != cardtype::scheme) return false;

    string name = schemeCard.get_name();
    
    if (name == "Mistform") {
        set_actions(get_actions() + 1);
    }
    else if (name == "Baptism of Blood") {
        heal(2);
    }
    else if (name == "Prey Upon") {
        target.takedamage(1);
        heal(1);
    }
    else if (name == "Ravening Seduction") {
        target.setposition(target.getx() + 2);
    }
    else if (name == "Administer Aid") {
        heal(1);
        drawcard();
    }
    else if (name == "Confirm Suspicion") {
        cout << "Confirm Suspicion: Choose a value (1-6)" << endl;
    }
    else if (name == "Eliminate the Impossible") {
        cout << "Eliminate the Impossible: Look at opponent's hand" << endl;
    }
    else if (name == "Master of Disguise") {
        int tempPos = getposition();
        setposition(target.getx());
        target.setposition(tempPos);
        target.takedamage(1);
    }

    actions--;
    return true;
}

bool hero::attack(hero& target, card& attackcard, Board& board) {
    if (actions <= 0) return false;
    if (!target.isalive()) return false;

    bool adjacent = board.isAdjacent(getx(), target.getx());
    if (!adjacent) return false;

    int damage = attackcard.getattack();
    if (damage > 0) {
        target.takedamage(damage);
    }

    actions--;
    return true;
}

bool hero::canact() const {
    return actions > 0;
}