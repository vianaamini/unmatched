#include "../include/hero.hpp"

hero::hero(const string& name, int maxhp, int movement) : character(name, maxhp, movement), actions(2) {}

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

bool hero::maneuver(int targetx, int targety, Board& board) {
    if (actions <= 0) return false;
    if (!board.isWalkable(targetx, targety)) return false;

    setposition(targetx, targety);
    drawcard();

    actions--;
    return true;
}

bool hero::scheme(card& schemeCard, hero& target) {
    if (actions <= 0) return false;
    if (schemeCard.gettype() != cardtype::scheme) return false;

    // اجرای اثر کارت طرح (به کارت‌های خاص واگذار شده)
    actions--;
    return true;
}

bool hero::attack(hero& target, card& attackCard) {
    if (actions <= 0) return false;
    if (!target.isalive()) return false;

    int damage = attackCard.getattack(); 

    if (damage > 0) {
        target.takedamage(damage);
    }

    actions--;
    return true;
}

bool hero::canact() const {
    return actions > 0;
}