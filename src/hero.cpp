#include "../include/hero.hpp"
#include "../include/game_manager.hpp"
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

hero::hero(const string& name, int maxhp, int movement)
    : character(name, maxhp, movement), actions(2), board(nullptr), gameManager(nullptr),
      predictedAttackValue(0), beastformDiscardCount(0), mistformTarget(""),
      raveningTargetFighter(""), raveningTargetSpace(""), eliminateCardIndex(-1),
      confirmSuspicionGuess(0), dashTargetNode(-1), gameIsAfootTargetNode(-1) {}

void hero::setBoard(Board* b) { board = b; }
Board* hero::getBoard() { return board; }
const Board* hero::getBoard() const { return board; }

void hero::set_actions(int new_actions) { actions = new_actions; }
void hero::reset_actions() { actions = 2; }
void hero::useAction() { if (actions > 0) actions--; }
int hero::get_actions() const { return actions; }
bool hero::canact() const { return actions > 0; }

deck& hero::getdeck() { return dk; }
const deck& hero::getdeck() const { return dk; }
vector<card>& hero::gethand() { return hand; }
const vector<card>& hero::gethand() const { return hand; }
int hero::handsize() const { return hand.size(); }

void hero::drawcard() {
    if (!dk.isempty()) {
        hand.push_back(dk.drawcard());
    } else {
        takedamage(2);
    }
}

void hero::drawhand() {
    for (int i = 0; i < 5; i++) drawcard();
}

void hero::discardToLimit(int limit) {
    while (hand.size() > limit) hand.pop_back();
}

bool hero::removeCardFromHand(const string& cardName) {
    for (size_t i = 0; i < hand.size(); i++) {
        if (hand[i].get_name() == cardName) {
            hand.erase(hand.begin() + i);
            return true;
        }
    }
    return false;
}

card hero::chooseDefense() {
    for (size_t i = 0; i < hand.size(); i++) {
        if (hand[i].gettype() == cardtype::defense || hand[i].gettype() == cardtype::multipurpose) {
            card result = hand[i];
            hand.erase(hand.begin() + i);
            return result;
        }
    }
    return card("No Defense", cardtype::defense, 0, 0, 0, cardowner::any);
}

bool hero::moveWithRules(int targetNode, int steps, Board& board) {
    if (!gameManager) return false;
    int start = getposition();
    if (start == targetNode) return true;

    vector<character*> allChars = gameManager->getAllCharacters();
    vector<character*> allies = gameManager->getAllies(this);
    vector<character*> enemies = gameManager->getEnemies(this);

    auto isOccupiedByEnemy = [&](int node) -> bool {
        for (auto* e : enemies) {
            if (e->isalive() && e->getx() == node) return true;
        }
        return false;
    };
    auto isOccupiedByAlly = [&](int node) -> bool {
        for (auto* a : allies) {
            if (a->isalive() && a->getx() == node && a != this) return true;
        }
        return false;
    };
    auto isOccupied = [&](int node) -> bool {
        return isOccupiedByEnemy(node) || isOccupiedByAlly(node);
    };

    queue<pair<int,int>> q;
    unordered_set<int> visited;
    q.push({start, 0});
    visited.insert(start);

    while (!q.empty()) {
        auto [node, dist] = q.front();
        q.pop();
        if (node == targetNode) {
            if (isOccupied(targetNode)) {
                return false;
            }
            setposition(targetNode);
            return true;
        }
        if (dist >= steps) continue;

        vector<int> neighbors = board.getNeighborIds(node);
        string nodeName = "n" + to_string(node);
        if (board.isTeleport(nodeName)) {
            string dest = board.getTeleportDestination(nodeName);
            int destId = board.getNodeId(dest);
            if (destId != node) {
                neighbors.push_back(destId);
            }
        }

        for (int next : neighbors) {
            if (visited.count(next)) continue;
            if (isOccupiedByEnemy(next)) continue;
            visited.insert(next);
            q.push({next, dist+1});
        }
    }
    return false;
}

bool hero::maneuver(int targetNode, Board& board, const card* boostCard) {
    if (actions <= 0) return false;

    drawcard();

    int steps = getmovement();
    if (boostCard) {
        steps += boostCard->getboost();
        for (auto it = hand.begin(); it != hand.end(); ++it) {
            if (it->get_name() == boostCard->get_name() && it->getboost() == boostCard->getboost()) {
                hand.erase(it);
                break;
            }
        }
    }

    bool success = moveWithRules(targetNode, steps, board);
    if (success) {
        actions--;
        return true;
    }
    return false;
}

bool hero::scheme(card& schemeCard, hero& target) {
    if (actions <= 0) return false;
    if (schemeCard.gettype() != cardtype::scheme) return false;

    string name = schemeCard.get_name();

    if (name == "Mistform") {
        actions++;
        if (!mistformTarget.empty() && board && board->hasSpace(mistformTarget)) {
            int id = board->getNodeId(mistformTarget);
            bool occupied = false;
            if (gameManager) {
                for (auto* c : gameManager->getAllCharacters()) {
                    if (c->isalive() && c->getx() == id && c != this) {
                        occupied = true;
                        break;
                    }
                }
            }
            if (!occupied) {
                setposition(id);
                cout << "Mistform: Dracula moved to " << mistformTarget << endl;
            } else {
                cout << "Mistform: target occupied!" << endl;
            }
        } else {
            cout << "Mistform: no valid target selected." << endl;
        }
        mistformTarget = "";
    }
    else if (name == "Baptism of Blood") {
        heal(2);
        if (gameManager) {
            for (auto* c : gameManager->getAllCharacters()) {
                if (c->getname().find("Sister") != string::npos && !c->isalive()) {
                    bool revived = gameManager->resurrectSister(c->getname(), getx());
                    if (revived) {
                        cout << "Baptism of Blood: " << c->getname() << " revived." << endl;
                    }
                    break;
                }
            }
        }
        cout << "Baptism of Blood: Healed 2." << endl;
    }
    else if (name == "Prey Upon") {
        if (board && gameManager) {
            auto enemies = gameManager->getEnemies(this);
            int damageDealt = 0;
            for (auto* enemy : enemies) {
                if (enemy->isalive() && board->isAdjacent(getx(), enemy->getx())) {
                    enemy->takedamage(1);
                    damageDealt++;
                }
            }
            heal(damageDealt);
            cout << "Prey Upon: Dealt " << damageDealt << " damage and healed." << endl;
        }
    }
    else if (name == "Ravening Seduction") {
        if (!raveningTargetFighter.empty() && !raveningTargetSpace.empty() && board && gameManager) {
            character* fighter = nullptr;
            for (auto* c : gameManager->getAllCharacters()) {
                if (c->getname() == raveningTargetFighter && c->isalive()) {
                    fighter = c;
                    break;
                }
            }
            if (fighter && board->hasSpace(raveningTargetSpace)) {
                int targetNode = board->getNodeId(raveningTargetSpace);
                bool occupied = false;
                for (auto* c : gameManager->getAllCharacters()) {
                    if (c->isalive() && c->getx() == targetNode && c != fighter) {
                        occupied = true;
                        break;
                    }
                }
                if (!occupied) {
                    fighter->setposition(targetNode);
                    cout << "Ravening Seduction: " << fighter->getname() << " moved to " << raveningTargetSpace << endl;
                    int sisters = 0;
                    auto allies = gameManager->getAllies(this);
                    for (auto* ally : allies) {
                        if (ally->isalive() && ally->getname().find("Sister") != string::npos) {
                            if (board->isAdjacent(fighter->getx(), ally->getx())) {
                                sisters++;
                            }
                        }
                    }
                    if (sisters > 0) {
                        fighter->takedamage(sisters);
                        cout << "Ravening Seduction: " << sisters << " damage dealt." << endl;
                    }
                } else {
                    cout << "Ravening Seduction: target occupied." << endl;
                }
            } else {
                cout << "Ravening Seduction: invalid fighter or space." << endl;
            }
        } else {
            cout << "Ravening Seduction: targets not set." << endl;
        }
        raveningTargetFighter = "";
        raveningTargetSpace = "";
    }
    else if (name == "Administer Aid") {
        heal(1);
        drawcard();
        if (board && gameManager) {
            character* watson = nullptr;
            for (auto* c : gameManager->getAllies(this)) {
                if (c->getname() == "Watson" && c->isalive()) {
                    watson = c;
                    break;
                }
            }
            if (watson) {
                auto neighbors = board->getNeighborIds(getx());
                for (int node : neighbors) {
                    bool occupied = false;
                    for (auto* c : gameManager->getAllCharacters()) {
                        if (c->isalive() && c->getx() == node && c != watson) {
                            occupied = true;
                            break;
                        }
                    }
                    if (!occupied) {
                        watson->setposition(node);
                        cout << "Administer Aid: Watson placed at n" << node << endl;
                        break;
                    }
                }
            }
        }
        cout << "Administer Aid: Holmes healed 1, drew 1 card." << endl;
    }
    else if (name == "Master of Disguise") {
        int myPos = getposition();
        int enemyPos = target.getposition();
        setposition(enemyPos);
        target.setposition(myPos);
        target.takedamage(1);
        cout << "Master of Disguise: Swapped positions and dealt 1 damage." << endl;
    }
    else if (name == "Eliminate the Impossible") {
        auto& enemyHand = target.gethand();
        if (!enemyHand.empty() && eliminateCardIndex >= 0 && eliminateCardIndex < (int)enemyHand.size()) {
            cout << "Eliminate the Impossible: discarded " << enemyHand[eliminateCardIndex].get_name() << endl;
            enemyHand.erase(enemyHand.begin() + eliminateCardIndex);
        } else {
            cout << "Eliminate the Impossible: no valid card selected." << endl;
        }
        eliminateCardIndex = -1;
    }
    else if (name == "Confirm Suspicion") {
        auto& enemyHand = target.gethand();
        if (!enemyHand.empty()) {
            bool found = false;
            int guess = confirmSuspicionGuess;
            for (size_t i = 0; i < enemyHand.size(); i++) {
                if (enemyHand[i].getattack() == guess || enemyHand[i].getdefense() == guess) {
                    target.takedamage(enemyHand[i].getboost());
                    cout << "Confirm Suspicion: card with value " << guess << " discarded, damage " << enemyHand[i].getboost() << endl;
                    enemyHand.erase(enemyHand.begin() + i);
                    found = true;
                    break;
                }
            }
            if (!found) {
                cout << "Confirm Suspicion: no card with value " << guess << ". Opponent reveals hand:" << endl;
                for (const auto& c : enemyHand) {
                    cout << "  " << c.get_name() << " (ATK:" << c.getattack() << " DEF:" << c.getdefense() << ")" << endl;
                }
            }
        } else {
            cout << "Confirm Suspicion: opponent's hand is empty." << endl;
        }
        confirmSuspicionGuess = 0;
    }

    actions--;
    return true;
}

bool hero::canAttack(const hero& target, const Board& board, bool ranged) const {
    if (!target.isalive()) return false;
    if (ranged) return true;
    return board.isAdjacent(getposition(), target.getposition());
}

bool hero::attack(hero& target, card& attackCard, Board& board) {
    if (actions <= 0) return false;
    if (!canAttack(target, board)) return false;

    card defenseCard = target.chooseDefense();

    int attackValue = attackCard.getattack();
    int defenseValue = defenseCard.getdefense();
    bool attackerWon = false;
    bool effectsCanceled = false;

    bool isProtected = false;
    if (attackCard.getowner() == cardowner::sherlock || attackCard.getowner() == cardowner::watson ||
        defenseCard.getowner() == cardowner::sherlock || defenseCard.getowner() == cardowner::watson) {
        isProtected = true;
    }

    if (defenseCard.get_name() == "Feint" && !isProtected) {
        effectsCanceled = true;
    }
    if (attackCard.get_name() == "Feint" && !isProtected) {
        effectsCanceled = true;
    }
    if (defenseCard.get_name() == "Do My Bidding") {
        effectsCanceled = true;
    }

    if (!effectsCanceled) {
        if (defenseCard.get_name() == "Look Into My Eyes") {
            defenseValue += attackCard.getboost();
        }
        if (defenseCard.get_name() == "Elementary") {
            if (predictedAttackValue == attackCard.getattack()) {
                attackValue = 0;
                effectsCanceled = true;
                cout << "Elementary: correct prediction! Attack ignored." << endl;
            } else {
                cout << "Elementary: wrong prediction." << endl;
            }
            predictedAttackValue = 0;
        }

        if (attackCard.get_name() == "Deduce Strategy") {
            defenseValue = defenseCard.getboost();
            cout << "Deduce Strategy: defense changed to " << defenseValue << endl;
        }
        if (attackCard.get_name() == "Beastform") {
            int count = beastformDiscardCount;
            for (int i = 0; i < count; i++) {
                if (!hand.empty()) {
                    hand.pop_back();
                    attackValue++;
                }
            }
            beastformDiscardCount = 0;
            cout << "Beastform: discarded " << count << " cards, attack now " << attackValue << endl;
        }
        if (attackCard.get_name() == "Feeding Frenzy") {
            if (gameManager) {
                auto allies = gameManager->getAllies(this);
                int sistersInZone = 0;
                for (auto* ally : allies) {
                    if (ally->isalive() && ally->getname().find("Sister") != string::npos) {
                        auto zonesTarget = board.getZonesAt(target.getx(), 0);
                        auto zonesAlly = board.getZonesAt(ally->getx(), 0);
                        for (const auto& z1 : zonesTarget) {
                            for (const auto& z2 : zonesAlly) {
                                if (z1 == z2) { sistersInZone++; break; }
                            }
                        }
                    }
                }
                attackValue += sistersInZone;
                cout << "Feeding Frenzy: +" << sistersInZone << " attack." << endl;
            }
        }
        if (attackCard.get_name() == "Ambush") {
            auto& enemyHand = target.gethand();
            if (!enemyHand.empty()) {
                int idx = rand() % enemyHand.size();
                attackValue += enemyHand[idx].getboost();
                cout << "Ambush: discarded " << enemyHand[idx].get_name() << " (boost " << enemyHand[idx].getboost() << ")" << endl;
                enemyHand.erase(enemyHand.begin() + idx);
            }
        }
    }

    int damage = std::max(0, attackValue - defenseValue);
    if (damage > 0) {
        target.takedamage(damage);
        attackerWon = true;
    }

    if (!effectsCanceled) {
        if (defenseCard.get_name() == "Exploit") target.drawcard();
        if (defenseCard.get_name() == "Dash") {
            if (dashTargetNode >= 0) {
                if (moveWithRules(dashTargetNode, 3, board)) {
                    cout << "Dash: moved." << endl;
                } else {
                    cout << "Dash: cannot move to target." << endl;
                }
                dashTargetNode = -1;
            }
        }
        if (defenseCard.get_name() == "Fixed Point") {
            if (gameManager) {
                character* watson = nullptr;
                character* holmes = nullptr;
                for (auto* c : gameManager->getAllCharacters()) {
                    if (c->getname() == "Watson" && c->isalive()) watson = c;
                    if (c->getname() == "Sherlock Holmes" && c->isalive()) holmes = c;
                }
                if (watson && holmes && board.isAdjacent(watson->getx(), holmes->getx())) {
                    watson->heal(1);
                    holmes->heal(1);
                    cout << "Fixed Point: healed both." << endl;
                }
            }
        }

        if (attackCard.get_name() == "Exploit") drawcard();
        if (attackCard.get_name() == "Dash") {
            if (dashTargetNode >= 0) {
                if (moveWithRules(dashTargetNode, 3, board)) {
                    cout << "Dash: moved." << endl;
                } else {
                    cout << "Dash: cannot move to target." << endl;
                }
                dashTargetNode = -1;
            }
        }
        if (attackCard.get_name() == "Education Never Ends") {
            if (attackerWon) {
                target.drawcard();
                cout << "Education Never Ends: opponent draws 1." << endl;
            } else {
                drawcard();
                drawcard();
                cout << "Education Never Ends: you draw 2." << endl;
            }
        }
        if (attackCard.get_name() == "Counter Punch") {
            if (board.isAdjacent(getposition(), target.getposition())) {
                target.takedamage(2);
                cout << "Counter Punch: dealt 2 damage." << endl;
            }
        }
        if (attackCard.get_name() == "Fixed Point") {
            if (gameManager) {
                character* watson = nullptr;
                character* holmes = nullptr;
                for (auto* c : gameManager->getAllCharacters()) {
                    if (c->getname() == "Watson" && c->isalive()) watson = c;
                    if (c->getname() == "Sherlock Holmes" && c->isalive()) holmes = c;
                }
                if (watson && holmes && board.isAdjacent(watson->getx(), holmes->getx())) {
                    watson->heal(1);
                    holmes->heal(1);
                    cout << "Fixed Point: healed both." << endl;
                }
            }
        }
        if (attackCard.get_name() == "Thirst for Sustenance" && attackerWon) {
            auto neighbors = board.getNeighborIds(target.getx());
            for (int node : neighbors) {
                bool occupied = false;
                if (gameManager) {
                    for (auto* c : gameManager->getAllCharacters()) {
                        if (c->isalive() && c->getx() == node && c != this) {
                            occupied = true;
                            break;
                        }
                    }
                }
                if (!occupied) {
                    setposition(node);
                    cout << "Thirst for Sustenance: Dracula placed adjacent to opponent." << endl;
                    break;
                }
            }
        }
        if (attackCard.get_name() == "The Game Is Afoot") {
            if (gameIsAfootTargetNode >= 0) {
                if (moveWithRules(gameIsAfootTargetNode, 3, board)) {
                    cout << "The Game is Afoot: moved." << endl;
                } else {
                    cout << "The Game is Afoot: cannot move to target." << endl;
                }
                gameIsAfootTargetNode = -1;
            }
        }
        if (attackCard.get_name() == "Study Methods" && attackerWon) {
            cout << "Study Methods: you may look at opponent's hand." << endl;
        }
    }

    actions--;
    return true;
}