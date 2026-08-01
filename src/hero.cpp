#include "../include/hero.hpp"
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <cstdlib>

using namespace std;

// ======================================================
// Constructor
// ======================================================

hero::hero(const string& name, int maxhp, int movement)
    : character(name, maxhp, movement), actions(2), board(nullptr) {}

// ======================================================
// Getters
// ======================================================

deck& hero::getdeck() { return dk; }
const deck& hero::getdeck() const { return dk; }

vector<card>& hero::gethand() { return hand; }
const vector<card>& hero::gethand() const { return hand; }

int hero::handsize() const {
    return static_cast<int>(hand.size());
}

int hero::get_actions() const {
    return actions;
}

// ======================================================
// Actions
// ======================================================

void hero::set_actions(int new_actions) {
    actions = new_actions;
}

void hero::reset_actions() {
    actions = 2;
}

void hero::useAction() {
    if (actions > 0)
        --actions;
}

bool hero::canact() const {
    return actions > 0;
}

// ======================================================
// Card Drawing
// ======================================================

void hero::drawcard() {

    if (!dk.isempty()) {
        hand.push_back(dk.drawcard());
    } else {
        takedamage(2);
        cout << getname()
             << " is exhausted and takes 2 damage!"
             << endl;
    }
}

void hero::drawhand() {

    for (int i = 0; i < 5; ++i) {
        drawcard();
    }
}

// ======================================================
// Choose Defense
// ======================================================

card hero::chooseDefense() {

    for (size_t i = 0; i < hand.size(); ++i) {

        if (hand[i].gettype() == cardtype::defense ||
            hand[i].gettype() == cardtype::multipurpose) {

            card c = hand[i];
            hand.erase(hand.begin() + i);
            return c;
        }
    }

    return card("No Defense",
                cardtype::defense,
                0, 0, 0,
                cardowner::any,
                "");
}

// ======================================================
// Maneuver
// ======================================================

bool hero::maneuver(int targetNode,
                    Board& board,
                    const card* boostCard) {

    if (actions <= 0)
        return false;

    // Draw is mandatory
    drawcard();

    // No movement
    if (targetNode == -1) {
        --actions;
        return true;
    }

    int maxSteps = getmovement();

    if (boostCard)
        maxSteps += boostCard->getboost();

    int start = getposition();

    if (start == targetNode) {
        --actions;
        return true;
    }

    queue<pair<int,int>> q;
    unordered_set<int> visited;

    q.push({start, 0});
    visited.insert(start);

    while (!q.empty()) {

        auto [current, dist] = q.front();
        q.pop();

        if (dist >= maxSteps)
            continue;

        auto neighbors = board.getNeighborIds(current);

        for (int next : neighbors) {

            if (visited.count(next))
                continue;

            visited.insert(next);

            if (next == targetNode) {
                setposition(targetNode);
                --actions;
                return true;
            }

            q.push({next, dist + 1});
        }
    }

    return false;
}

// ======================================================
// Scheme
// ======================================================

bool hero::scheme(card& schemeCard, hero& target) {

    if (actions <= 0)
        return false;

    if (schemeCard.gettype() != cardtype::scheme)
        return false;

    string name = schemeCard.get_name();

    // ================= Dracula =================

    if (name == "Mistform") {
        set_actions(get_actions() + 1);
    }

    else if (name == "Baptism of Blood") {
        heal(2);
    }

    else if (name == "Prey Upon") {

        if (board && board->isAdjacent(getx(), target.getx())) {
            target.takedamage(1);
            heal(1);
        }
    }

    else if (name == "Ravening Seduction") {

        if (board) {

            auto neighbors = board->getNeighborIds(target.getx());

            if (!neighbors.empty()) {
                target.setposition(neighbors.front());
            }
        }

        if (board && board->isAdjacent(getx(), target.getx())) {
            target.takedamage(1);
        }
    }

    // ================= Sherlock =================

    else if (name == "Administer Aid") {
        heal(1);
        drawcard();
    }

    else if (name == "Master of Disguise") {

        int myPos = getposition();
        int enemyPos = target.getposition();

        setposition(enemyPos);
        target.setposition(myPos);

        target.takedamage(1);
    }

    else if (name == "Eliminate the Impossible") {

        auto& enemyHand = target.gethand();

        if (!enemyHand.empty()) {
            enemyHand.erase(enemyHand.begin());
        }
    }

    else if (name == "Confirm Suspicion") {

        auto& enemyHand = target.gethand();

        if (!enemyHand.empty()) {

            int guessed = 3; // temporary AI guess

            for (size_t i = 0; i < enemyHand.size(); ++i) {

                if (enemyHand[i].getattack() == guessed ||
                    enemyHand[i].getdefense() == guessed) {

                    target.takedamage(enemyHand[i].getboost());
                    enemyHand.erase(enemyHand.begin() + i);
                    break;
                }
            }
        }
    }

    --actions;
    return true;
}

// ======================================================
// Attack
// ======================================================

bool hero::attack(hero& target, card& attackCard, Board& board) {

    if (actions <= 0)
        return false;

    if (!target.isalive())
        return false;

    if (!board.isAdjacent(getx(), target.getx()))
        return false;

    // --------------------------------------------------
    // Choose defense automatically
    // --------------------------------------------------

    card defenseCard = target.chooseDefense();

    cout << "\\n=== COMBAT ===\\n";
    cout << getname() << " attacks with "
         << attackCard.get_name()
         << " (" << attackCard.getattack() << ")\\n";

    cout << target.getname() << " defends with "
         << defenseCard.get_name()
         << " (" << defenseCard.getdefense() << ")\\n";

    // --------------------------------------------------
    // Feint cancels all effects
    // --------------------------------------------------

    bool cancelEffects =
        attackCard.get_name() == "Feint" ||
        defenseCard.get_name() == "Feint";

    int attackValue = attackCard.getattack();
    int defenseValue = defenseCard.getdefense();

    // --------------------------------------------------
    // DURING COMBAT
    // --------------------------------------------------

    if (!cancelEffects) {

        // Look Into My Eyes

        if (defenseCard.get_name() == "Look Into My Eyes") {
            defenseValue += attackCard.getboost();
        }

        // Deduce Strategy

        if (attackCard.get_name() == "Deduce Strategy") {
            defenseValue = defenseCard.getboost();
        }

        // Elementary

        if (defenseCard.get_name() == "Elementary") {

            int predicted = attackCard.getattack();

            if (predicted == attackCard.getattack()) {
                attackValue = 0;
                cancelEffects = true;
                cout << "Elementary cancels the attack!\\n";
            }
        }

        // Beastform

        if (attackCard.get_name() == "Beastform") {

            int discardCount = min(2, static_cast<int>(hand.size()));

            for (int i = 0; i < discardCount; ++i)
                hand.pop_back();

            attackValue += discardCount;

            cout << "Beastform gains +"
                 << discardCount
                 << " attack\\n";
        }

        // Feeding Frenzy

        if (attackCard.get_name() == "Feeding Frenzy") {
            attackValue += 1;
        }
    }

    // --------------------------------------------------
    // DAMAGE
    // --------------------------------------------------

    int damage = max(0, attackValue - defenseValue);

    bool attackerWon = damage > 0;

    if (damage > 0) {

        target.takedamage(damage);

        cout << target.getname()
             << " takes "
             << damage
             << " damage\\n";
    }
    else {
        cout << target.getname()
             << " blocks the attack\\n";
    }

    // --------------------------------------------------
    // AFTER COMBAT
    // --------------------------------------------------

    if (!cancelEffects) {

        // Counter Punch

        if (attackCard.get_name() == "Counter Punch" &&
            board.isAdjacent(getx(), target.getx())) {

            target.takedamage(2);
        }

        if (defenseCard.get_name() == "Counter Punch" &&
            board.isAdjacent(getx(), target.getx())) {

            takedamage(2);
        }

        // Exploit

        if (attackCard.get_name() == "Exploit")
            drawcard();

        if (defenseCard.get_name() == "Exploit")
            target.drawcard();

        // Education Never Ends

        if (attackCard.get_name() == "Education Never Ends") {

            if (attackerWon)
                target.drawcard();
            else {
                drawcard();
                drawcard();
            }
        }

        // Study Methods

        if (attackCard.get_name() == "Study Methods" && attackerWon) {

            cout << "Opponent hand:" << endl;

            for (const auto& c : target.gethand())
                cout << " - " << c.get_name() << endl;
        }

        // Fixed Point

        if (attackCard.get_name() == "Fixed Point" &&
            board.isAdjacent(getx(), target.getx())) {

            heal(1);
            target.heal(1);
        }

        // Dash

        if (attackCard.get_name() == "Dash") {

            auto neighbors = board.getNeighborIds(getx());

            if (!neighbors.empty()) {
                setposition(neighbors.front());
            }
        }

        // The Game is Afoot

        if (attackCard.get_name() == "The Game is Afoot") {

            auto neighbors = board.getNeighborIds(getx());

            if (!neighbors.empty()) {
                setposition(neighbors.front());
            }
        }

        if (attackCard.get_name() == "Thirst for Sustenance" && attackerWon) {

            auto neighbors = board.getNeighborIds(target.getx());

            if (!neighbors.empty()) {
                setposition(neighbors.front());
            }
        }


        if (attackCard.get_name() == "Ambush" &&
            !target.gethand().empty()) {

            size_t idx = rand() % target.gethand().size();

            int bonus = target.gethand()[idx].getboost();

            target.gethand().erase(target.gethand().begin() + idx);

            target.takedamage(bonus);
        }
    }

    --actions;
    return true;
}