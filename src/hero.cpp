#include "../include/hero.hpp"
#include "../include/game_manager.hpp"
#include "../include/invisible_man.hpp"
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

hero::hero(const string& name, int maxhp, int movement)
    : character(name, maxhp, movement),
      actions(2),
      board(nullptr),
      gameManager(nullptr),
      predictedAttackValue(0),
      beastformDiscardCount(0),
      mistformTarget(""),
      raveningTargetFighter(""),
      raveningTargetSpace(""),
      eliminateCardIndex(-1),
      confirmSuspicionGuess(-1),
      dashTargetNode(-1),
      gameIsAfootTargetNode(-1)
{
}

void hero::setBoard(Board* b) {
    board = b;
}

Board* hero::getBoard() {
    return board;
}

const Board* hero::getBoard() const {
    return board;
}

void hero::set_actions(int new_actions) {
    actions = new_actions;
}

void hero::reset_actions() {
    actions = 2;
}

void hero::useAction() {
    if (actions > 0)
        actions--;
}

int hero::get_actions() const {
    return actions;
}

bool hero::canact() const {
    return actions > 0;
}

deck& hero::getdeck() {
    return dk;
}

const deck& hero::getdeck() const {
    return dk;
}

vector<card>& hero::gethand() {
    return hand;
}

const vector<card>& hero::gethand() const {
    return hand;
}

int hero::handsize() const {
    return static_cast<int>(hand.size());
}

void hero::drawcard() {
    if (!dk.isempty()) {
        hand.push_back(dk.drawcard());
    } else {
        takedamage(2);
    }
}

void hero::drawhand() {
    for (int i = 0; i < 5; i++)
        drawcard();
}

void hero::discardToLimit(int limit) {
    while (static_cast<int>(hand.size()) > limit)
        hand.pop_back();
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
        if (hand[i].gettype() == cardtype::defense ||
            hand[i].gettype() == cardtype::multipurpose) {

            card result = hand[i];
            hand.erase(hand.begin() + i);
            return result;
        }
    }

    return card(
        "No Defense",
        cardtype::defense,
        0,
        0,
        0,
        cardowner::any
    );
}

bool hero::moveWithRules(int targetNode, int steps, Board& board) {
    if (!gameManager)
        return false;

    int start = getposition();

    if (start == targetNode)
        return true;

    vector<character*> allies = gameManager->getAllies(this);
    vector<character*> enemies = gameManager->getEnemies(this);

    auto isOccupiedByEnemy = [&](int node) -> bool {
        for (auto* e : enemies) {
            if (e->isalive() && e->getx() == node)
                return true;
        }

        return false;
    };

    auto isOccupiedByAlly = [&](int node) -> bool {
        for (auto* a : allies) {
            if (a->isalive() &&
                a->getx() == node &&
                a != this) {
                return true;
            }
        }

        return false;
    };

    auto isOccupied = [&](int node) -> bool {
        return isOccupiedByEnemy(node) || isOccupiedByAlly(node);
    };

    queue<pair<int, int>> q;
    unordered_set<int> visited;

    q.push({start, 0});
    visited.insert(start);

    while (!q.empty()) {
        auto [node, dist] = q.front();
        q.pop();

        if (node == targetNode) {
            if (isOccupied(targetNode))
                return false;

            setposition(targetNode);
            return true;
        }

        if (dist >= steps)
            continue;

        vector<int> neighbors = board.getNeighborIds(node);
        string nodeName = board.getNodeName(node);

        if (!nodeName.empty() && board.isTeleport(nodeName)) {
            string dest = board.getTeleportDestination(nodeName);
            int destId = board.getNodeId(dest);

            if (destId != node)
                neighbors.push_back(destId);
        }

        for (int next : neighbors) {
            if (visited.count(next))
                continue;

            if (isOccupiedByEnemy(next))
                continue;

            visited.insert(next);
            q.push({next, dist + 1});
        }
    }

    return false;
}

bool hero::maneuver(
    int targetNode,
    Board& board,
    const card* boostCard
) {
    if (actions <= 0) {
        cout << "Cannot move: no actions remaining. Try again." << endl;
        return false;
    }

    drawcard();

    int steps = getmovement();

    if (boostCard)
        steps += boostCard->getboost();

    bool success = moveWithRules(
        targetNode,
        steps,
        board
    );

    if (boostCard) {
        for (auto it = hand.begin(); it != hand.end(); ++it) {
            if (it->get_name() == boostCard->get_name() &&
                it->getboost() == boostCard->getboost()) {

                hand.erase(it);
                break;
            }
        }
    }

    actions--;

    if (!success) {
        cout << "Cannot move to that space." << endl;
    }

    return true;
}

bool hero::scheme(card& schemeCard, hero& target) {
    if (actions <= 0) {
        cout << "Cannot use this card: no actions remaining. Try again." << endl;
        return false;
    }

    if (schemeCard.gettype() != cardtype::scheme) {
        cout << "Cannot use this card: not a scheme card. Try again." << endl;
        return false;
    }

    if (!board || !gameManager) {
        cout << "Cannot use this card: game state unavailable. Try again." << endl;
        return false;
    }

    string name = schemeCard.get_name();

    if (InvisibleMan* imSelf = dynamic_cast<InvisibleMan*>(this)) {
        if (imSelf->executeSchemeCard(schemeCard, target)) {
            actions--;
            return true;
        }
    }

    if (name == "Mistform") {
        if (mistformTarget.empty() ||
            !board->hasSpace(mistformTarget)) {

            cout << "Cannot use Mistform: no valid target selected. Try again." << endl;
            mistformTarget = "";
            return false;
        }

        int id = board->getNodeId(mistformTarget);
        bool occupied = false;

        for (auto* c : gameManager->getAllCharacters()) {
            if (c->isalive() &&
                c->getx() == id &&
                c != this) {

                occupied = true;
                break;
            }
        }

        if (occupied) {
            cout << "Cannot use Mistform: target space is occupied. Try again." << endl;
            mistformTarget = "";
            return false;
        }

        setposition(id);
        actions++;
        cout << "Mistform: Dracula moved to "
             << mistformTarget << endl;

        mistformTarget = "";
    }
    else if (name == "Baptism of Blood") {
        heal(2);

        for (auto* c : gameManager->getAllCharacters()) {
            if (c->getname().find("Sister") != string::npos &&
                !c->isalive()) {

                bool revived =
                    gameManager->resurrectSister(
                        c->getname(),
                        getx()
                    );

                if (revived) {
                    cout << "Baptism of Blood: "
                         << c->getname()
                         << " revived." << endl;
                }

                break;
            }
        }

        cout << "Baptism of Blood: Healed 2." << endl;
    }
    else if (name == "Prey Upon") {
        auto enemies = gameManager->getEnemies(this);
        int damageDealt = 0;

        for (auto* enemy : enemies) {
            if (enemy->isalive() &&
                board->isAdjacent(getx(), enemy->getx())) {

                enemy->takedamage(1);
                damageDealt++;
            }
        }

        if (damageDealt > 0)
            heal(damageDealt);

        cout << "Prey Upon: Dealt "
             << damageDealt
             << " damage and healed." << endl;
    }
    else if (name == "Ravening Seduction") {
        if (raveningTargetFighter.empty() ||
            raveningTargetSpace.empty()) {

            cout << "Cannot use Ravening Seduction: targets not set. Try again." << endl;
            raveningTargetFighter = "";
            raveningTargetSpace = "";
            return false;
        }

        character* fighter = nullptr;

        for (auto* c : gameManager->getAllCharacters()) {
            if (c->getname() == raveningTargetFighter &&
                c->isalive()) {

                fighter = c;
                break;
            }
        }

        if (!fighter ||
            !board->hasSpace(raveningTargetSpace)) {

            cout << "Cannot use Ravening Seduction: invalid fighter or space. Try again." << endl;
            raveningTargetFighter = "";
            raveningTargetSpace = "";
            return false;
        }

        int startNode = fighter->getx();
        int targetNode = board->getNodeId(raveningTargetSpace);

        auto isOccupied = [&](int node) -> bool {
            for (auto* c : gameManager->getAllCharacters()) {
                if (c->isalive() &&
                    c != fighter &&
                    c->getx() == node) {

                    return true;
                }
            }

            return false;
        };

        bool reachable = false;

        if (startNode == targetNode) {
            reachable = true;
        }
        else {
            queue<pair<int, int>> q;
            unordered_set<int> visited;

            q.push({startNode, 0});
            visited.insert(startNode);

            while (!q.empty()) {
                auto [node, dist] = q.front();
                q.pop();

                if (node == targetNode) {
                    if (!isOccupied(targetNode))
                        reachable = true;

                    break;
                }

                if (dist >= 2)
                    continue;

                for (int next : board->getNeighborIds(node)) {
                    if (visited.count(next))
                        continue;

                    if (isOccupied(next) &&
                        next != targetNode) {
                        continue;
                    }

                    visited.insert(next);
                    q.push({next, dist + 1});
                }
            }
        }

        if (!reachable) {
            cout << "Cannot use Ravening Seduction: target unreachable within 2 spaces or occupied. Try again." << endl;
            raveningTargetFighter = "";
            raveningTargetSpace = "";
            return false;
        }

        fighter->setposition(targetNode);

        cout << "Ravening Seduction: "
             << fighter->getname()
             << " moved to "
             << raveningTargetSpace << endl;

        int sisters = 0;

        for (auto* c : gameManager->getAllCharacters()) {
            if (c->isalive() &&
                c->getname().find("Sister") != string::npos &&
                board->isAdjacent(
                    fighter->getx(),
                    c->getx()
                )) {

                sisters++;
            }
        }

        if (sisters > 0) {
            fighter->takedamage(sisters);

            cout << "Ravening Seduction: "
                 << sisters
                 << " damage dealt." << endl;
        }

        raveningTargetFighter = "";
        raveningTargetSpace = "";
    }
    else if (name == "Administer Aid") {
        drawcard();

        character* holmes = nullptr;
        character* watson = nullptr;

        for (auto* c : gameManager->getAllCharacters()) {
            if (c->getname() == "Sherlock Holmes" &&
                c->isalive()) {
                holmes = c;
            }

            if (c->getname() == "Watson" &&
                c->isalive()) {
                watson = c;
            }
        }

        if (holmes) {
            holmes->heal(1);
            cout << "Administer Aid: Holmes healed 1." << endl;
        }

        if (watson && holmes) {
            auto neighbors =
                board->getNeighborIds(holmes->getx());

            for (int node : neighbors) {
                bool occupied = false;

                for (auto* c : gameManager->getAllCharacters()) {
                    if (c->isalive() &&
                        c->getx() == node &&
                        c != watson) {

                        occupied = true;
                        break;
                    }
                }

                if (!occupied) {
                    watson->setposition(node);

                    cout << "Administer Aid: Watson placed at n"
                         << node << endl;

                    break;
                }
            }
        }

        cout << "Administer Aid: drew 1 card." << endl;
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

        if (!enemyHand.empty()) {
            if (eliminateCardIndex < 0 ||
                eliminateCardIndex >= static_cast<int>(enemyHand.size())) {

                cout << "Cannot use Eliminate the Impossible: no valid card selected. Try again." << endl;

                eliminateCardIndex = -1;
                return false;
            }

            cout << "Eliminate the Impossible: discarded "
                 << enemyHand[eliminateCardIndex].get_name()
                 << endl;

            enemyHand.erase(
                enemyHand.begin() + eliminateCardIndex
            );
        }
        else {
            cout << "Eliminate the Impossible: opponent's hand is empty, nothing to discard." << endl;
        }

        eliminateCardIndex = -1;
    }
    else if (name == "Confirm Suspicion") {
        if (confirmSuspicionGuess < 0) {
            cout << "Cannot use Confirm Suspicion: no value named. Try again." << endl;
            return false;
        }

        auto& enemyHand = target.gethand();
        bool found = false;
        int guess = confirmSuspicionGuess;

        for (size_t i = 0; i < enemyHand.size(); i++) {
            if (enemyHand[i].getattack() == guess ||
                enemyHand[i].getdefense() == guess) {

                target.takedamage(
                    enemyHand[i].getboost()
                );

                cout << "Confirm Suspicion: card with value "
                     << guess
                     << " discarded, damage "
                     << enemyHand[i].getboost()
                     << endl;

                enemyHand.erase(enemyHand.begin() + i);
                found = true;
                break;
            }
        }

        if (!found) {
            cout << "Confirm Suspicion: no card with value "
                 << guess
                 << ". Opponent reveals hand:" << endl;

            for (const auto& c : enemyHand) {
                cout << "  "
                     << c.get_name()
                     << " (ATK:"
                     << c.getattack()
                     << " DEF:"
                     << c.getdefense()
                     << ")" << endl;
            }
        }

        confirmSuspicionGuess = -1;
    }
    else {
        cout << "Cannot use this card: unrecognized scheme card. Try again." << endl;
        return false;
    }

    actions--;

    return true;
}

bool hero::canAttack(const hero& target, const Board& board, bool ranged) const {
    if (!target.isalive()) return false;

    if (board.isAdjacent(getposition(), target.getposition()))
        return true;

    if (ranged) {
        auto myZones = board.getZonesAt(getposition(), 0);
        auto targetZones = board.getZonesAt(target.getposition(), 0);
        for (const auto& z1 : myZones)
            for (const auto& z2 : targetZones)
                if (z1 == z2) return true;
    }
    return false;
}

bool hero::canAttack(const character& target, const Board& board, bool ranged) const {
    if (!target.isalive()) return false;

    if (board.isAdjacent(getposition(), target.getposition()))
        return true;

    if (ranged) {
        auto myZones = board.getZonesAt(getposition(), 0);
        auto targetZones = board.getZonesAt(target.getposition(), 0);
        for (const auto& z1 : myZones)
            for (const auto& z2 : targetZones)
                if (z1 == z2) return true;
    }
    return false;
}

bool hero::attack(character& target,card& attackCard,Board& board) {

    hero* heroTarget = dynamic_cast<hero*>(&target);
    if (heroTarget)
        return attack(*heroTarget, attackCard, board);

    if (actions <= 0) {
        cout << "Cannot attack: no actions remaining. Try again." << endl;
        return false;
    }

    if (!canAttack(target, board)) {
        cout << "Cannot attack: target is not in range. Try again." << endl;
        return false;
    }
    int damage = std::max(0, attackCard.getattack());

    if (damage > 0) {
        target.takedamage(damage);

        cout << getname()
             << " attacked " << target.getname()
             << " for " << damage << " damage." << endl;
    }

    actions--;

    return true;
}

bool hero::attack(
    hero& target,
    card& attackCard,
    Board& board,
    const card* selectedDefense
) {
    if (actions <= 0) {
        cout << "Cannot attack: no actions remaining. Try again." << endl;
        return false;
    }

    if (!canAttack(target, board)) {
        cout << "Cannot attack: target is not in range. Try again." << endl;
        return false;
    }

    card noDefense(
        "No Defense",
        cardtype::defense,
        0,
        0,
        0,
        cardowner::any
    );

    card defenseCard;

    if (selectedDefense) {
        defenseCard = *selectedDefense;
    }
    else {
        defenseCard = target.chooseDefense();
    }

    int attackValue = attackCard.getattack();
    int defenseValue = defenseCard.getdefense();

    bool attackerWon = false;
    bool effectsCanceled = false;

    if (defenseCard.get_name() == "Feint")
        effectsCanceled = true;

    if (attackCard.get_name() == "Feint")
        effectsCanceled = true;

    if (!effectsCanceled) {
        if (InvisibleMan* imDef = dynamic_cast<InvisibleMan*>(&target)) {
            imDef->executeDefenseCardEffects(defenseCard, attackCard, defenseValue, attackValue, effectsCanceled);
        }
        if (InvisibleMan* imAtk = dynamic_cast<InvisibleMan*>(this)) {
            imAtk->executeAttackCardEffects(attackCard, target, attackValue, defenseValue, attackerWon, effectsCanceled, defenseCard);
        }

        if (defenseCard.get_name() == "Look Into My Eyes") {
            defenseValue += attackCard.getboost();
        }

        if (defenseCard.get_name() == "Deduce Strategy") {
            int newAtk = attackCard.getboost();

            if (newAtk < attackValue) {
                attackValue = newAtk;

                cout << "Deduce Strategy: attack changed to "
                     << attackValue << endl;
            }
        }

        if (defenseCard.get_name() == "Elementary") {
            if (target.predictedAttackValue == attackCard.getattack()) {
                attackValue = 0;
                effectsCanceled = true;

                cout << "Elementary: correct prediction! Attack ignored." << endl;
            }
            else {
                cout << "Elementary: wrong prediction." << endl;
            }

            target.predictedAttackValue = 0;
        }

        if (attackCard.get_name() == "Deduce Strategy") {
            int newDef = defenseCard.getboost();

            if (newDef < defenseValue) {
                defenseValue = newDef;

                cout << "Deduce Strategy: defense changed to "
                     << defenseValue << endl;
            }
        }

        if (attackCard.get_name() == "Beastform") {
            int count = beastformDiscardCount;

            if (count > static_cast<int>(hand.size()))
                count = static_cast<int>(hand.size());

            for (int i = 0; i < count; i++) {
                hand.pop_back();
                attackValue++;
            }

            beastformDiscardCount = 0;

            cout << "Beastform: discarded "
                 << count
                 << " cards, attack now "
                 << attackValue
                 << endl;
        }

        if (attackCard.get_name() == "Feeding Frenzy") {
            if (gameManager) {
                auto allies = gameManager->getAllies(this);
                int sistersInZone = 0;

                auto zonesTarget =
                    board.getZonesAt(target.getx(), 0);

                for (auto* ally : allies) {
                    if (ally->isalive() &&
                        ally->getname().find("Sister") != string::npos) {

                        auto zonesAlly =
                            board.getZonesAt(ally->getx(), 0);

                        bool sharesZone = false;

                        for (const auto& z1 : zonesTarget) {
                            for (const auto& z2 : zonesAlly) {
                                if (z1 == z2) {
                                    sharesZone = true;
                                    break;
                                }
                            }

                            if (sharesZone)
                                break;
                        }

                        if (sharesZone)
                            sistersInZone++;
                    }
                }

                attackValue += sistersInZone;

                cout << "Feeding Frenzy: +"
                     << sistersInZone
                     << " attack." << endl;
            }
        }

        if (attackCard.get_name() == "Ambush") {
            auto& enemyHand = target.gethand();

            if (!enemyHand.empty()) {
                int idx =
                    rand() % static_cast<int>(enemyHand.size());

                attackValue += enemyHand[idx].getboost();

                cout << "Ambush: discarded "
                     << enemyHand[idx].get_name()
                     << " (boost "
                     << enemyHand[idx].getboost()
                     << ")" << endl;

                enemyHand.erase(
                    enemyHand.begin() + idx
                );
            }
        }
    }

    int damage =
        std::max(0, attackValue - defenseValue);

    if (damage > 0) {
        target.takedamage(damage);
        attackerWon = true;
    }

    if (!effectsCanceled) {
        if (defenseCard.get_name() == "Exploit")
            target.drawcard();

        if (defenseCard.get_name() == "Dash") {
            if (target.dashTargetNode >= 0) {
                if (target.moveWithRules(
                        target.dashTargetNode,
                        3,
                        board)) {

                    cout << "Dash: moved." << endl;
                }
                else {
                    cout << "Dash: cannot move to target." << endl;
                }

                target.dashTargetNode = -1;
            }
        }

        if (defenseCard.get_name() == "Fixed Point") {
            if (gameManager) {
                character* watson = nullptr;
                character* holmes = nullptr;

                for (auto* c :
                     gameManager->getAllCharacters()) {

                    if (c->getname() == "Watson" &&
                        c->isalive()) {
                        watson = c;
                    }

                    if (c->getname() == "Sherlock Holmes" &&
                        c->isalive()) {
                        holmes = c;
                    }
                }

                if (watson &&
                    holmes &&
                    board.isAdjacent(
                        watson->getx(),
                        holmes->getx()
                    )) {

                    watson->heal(1);
                    holmes->heal(1);

                    cout << "Fixed Point: healed both." << endl;
                }
            }
        }

        if (defenseCard.get_name() == "Education Never Ends") {
            if (!attackerWon) {
                drawcard();
                cout << "Education Never Ends: opponent draws 1." << endl;
            }
            else {
                target.drawcard();
                target.drawcard();

                cout << "Education Never Ends: you draw 2." << endl;
            }
        }

        if (defenseCard.get_name() == "Counter Punch") {
            if (board.isAdjacent(
                    target.getposition(),
                    getposition())) {

                takedamage(2);

                cout << "Counter Punch: dealt 2 damage." << endl;
            }
        }

        if (defenseCard.get_name() == "Study Methods" &&
            !attackerWon) {

            for (const auto& c : hand) {
                cout << "Study Methods: "
                     << c.get_name()
                     << endl;
            }
        }

        if (attackCard.get_name() == "Exploit")
            drawcard();

        if (attackCard.get_name() == "Dash") {
            if (dashTargetNode >= 0) {
                if (moveWithRules(
                        dashTargetNode,
                        3,
                        board)) {

                    cout << "Dash: moved." << endl;
                }
                else {
                    cout << "Dash: cannot move to target." << endl;
                }

                dashTargetNode = -1;
            }
        }

        if (attackCard.get_name() == "Education Never Ends") {
            if (attackerWon) {
                target.drawcard();

                cout << "Education Never Ends: opponent draws 1." << endl;
            }
            else {
                drawcard();
                drawcard();

                cout << "Education Never Ends: you draw 2." << endl;
            }
        }

        if (attackCard.get_name() == "Counter Punch") {
            if (board.isAdjacent(
                    getposition(),
                    target.getposition())) {

                target.takedamage(2);

                cout << "Counter Punch: dealt 2 damage." << endl;
            }
        }

        if (attackCard.get_name() == "Fixed Point") {
            if (gameManager) {
                character* watson = nullptr;
                character* holmes = nullptr;

                for (auto* c :
                     gameManager->getAllCharacters()) {

                    if (c->getname() == "Watson" &&
                        c->isalive()) {
                        watson = c;
                    }

                    if (c->getname() == "Sherlock Holmes" &&
                        c->isalive()) {
                        holmes = c;
                    }
                }

                if (watson &&
                    holmes &&
                    board.isAdjacent(
                        watson->getx(),
                        holmes->getx()
                    )) {

                    watson->heal(1);
                    holmes->heal(1);

                    cout << "Fixed Point: healed both." << endl;
                }
            }
        }

        if (attackCard.get_name() == "Thirst for Sustenance" &&
            attackerWon) {

            if (gameManager) {
                character* dracula = nullptr;

                for (auto* c :
                     gameManager->getAllCharacters()) {

                    if (c->getname() == "Dracula" &&
                        c->isalive()) {

                        dracula = c;
                        break;
                    }
                }

                if (dracula) {
                    auto neighbors =
                        board.getNeighborIds(target.getx());

                    for (int node : neighbors) {
                        bool occupied = false;

                        for (auto* c :
                             gameManager->getAllCharacters()) {

                            if (c->isalive() &&
                                c->getx() == node &&
                                c != dracula) {

                                occupied = true;
                                break;
                            }
                        }

                        if (!occupied) {
                            dracula->setposition(node);

                            cout << "Thirst for Sustenance: Dracula placed adjacent to opponent." << endl;
                            break;
                        }
                    }
                }
            }
        }

        if (attackCard.get_name() == "The Game is Afoot") {
            if (gameIsAfootTargetNode >= 0) {
                if (moveWithRules(
                        gameIsAfootTargetNode,
                        3,
                        board)) {

                    cout << "The Game is Afoot: moved." << endl;
                }
                else {
                    cout << "The Game is Afoot: cannot move to target." << endl;
                }

                gameIsAfootTargetNode = -1;
            }
        }

        if (attackCard.get_name() == "Study Methods" &&
            attackerWon) {

            for (const auto& c : target.gethand()) {
                cout << "Study Methods: "
                     << c.get_name()
                     << endl;
            }
        }
    }

    actions--;

    return true;
}