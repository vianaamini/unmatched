#include "../include/hero.hpp"
#include <queue>
#include <unordered_set>
#include <iostream>

hero::hero(const string& name, int maxhp, int movement) 
    : character(name, maxhp, movement), actions(2), board(nullptr) {}

deck& hero::getdeck() { return dk; }
const deck& hero::getdeck() const { return dk; }

vector<card>& hero::gethand() { return hand; }
const vector<card>& hero::gethand() const { return hand; }

int hero::handsize() const { return hand.size(); }

void hero::drawcard() {
    if (dk.getsize() > 0) {
        hand.push_back(dk.drawcard());
    } else {
        takedamage(2);
        std::cout << getname() << " is exhausted! Took 2 damage." << std::endl;
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

bool hero::maneuver(int targetNode, Board& board, const card* boostCard) {
    if (actions <= 0) return false;
    
    drawcard();
    
    if (targetNode == -1) {
        actions--;
        return true;
    }
    
    if (!board.hasSpace("n" + to_string(targetNode))) return false;
    
    int startNode = getx();
    if (startNode == targetNode) {
        actions--;
        return true;
    }
    
    int maxSteps = getmovement();
    if (boostCard) {
        maxSteps += boostCard->getboost();
    }
    
    string startName = "n" + to_string(startNode);
    string targetName = "n" + to_string(targetNode);
    
    std::queue<int> q;
    std::unordered_set<int> visited;
    q.push(startNode);
    visited.insert(startNode);
    
    bool reachable = false;
    for (int step = 0; step < maxSteps; ++step) {
        int levelSize = q.size();
        for (int i = 0; i < levelSize; ++i) {
            int current = q.front();
            q.pop();
            
            if (current == targetNode) {
                reachable = true;
                break;
            }
            
            auto neighbors = board.getNeighborIds(current);
            
            string currentName = "n" + to_string(current);
            if (board.isTeleport(currentName)) {
                string dest = board.getTeleportDestination(currentName);
                int destId = board.getNodeId(dest);
                if (destId != current && visited.find(destId) == visited.end()) {
                    neighbors.push_back(destId);
                }
            }
            
            for (int neighbor : neighbors) {
                if (visited.find(neighbor) != visited.end()) continue;
                if (board.hasSpace("n" + to_string(neighbor))) {
                    q.push(neighbor);
                    visited.insert(neighbor);
                }
            }
        }
        if (reachable) break;
    }
    
    if (!reachable) return false;
    
    setposition(targetNode);
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

    int attackValue = attackcard.getattack();
    int defenseValue = 0;
    card defendCard("Empty", cardtype::defense, 0, 0, 0, cardowner::any, "No effect");
    
    if (target.handsize() > 0) {
        defendCard = target.gethand()[0];
        target.gethand().erase(target.gethand().begin());
    }
    
    std::cout << "\n=== Combat ===" << std::endl;
    std::cout << getname() << " plays: " << attackcard.get_name() << " (ATK: " << attackValue << ")" << std::endl;
    std::cout << target.getname() << " plays: " << defendCard.get_name() << " (DEF: " << defendCard.getdefense() << ")" << std::endl;
    
    bool attacker_won = false;
    int damage = attackValue - defendCard.getdefense();
    if (damage > 0) {
        attacker_won = true;
    }
    
    if (attackcard.get_name() == "Feint" || defendCard.get_name() == "Feint") {
        std::cout << "Feint: All card effects are canceled!" << std::endl;
        if (damage > 0) {
            target.takedamage(damage);
            std::cout << target.getname() << " took " << damage << " damage." << std::endl;
        } else {
            std::cout << target.getname() << " defended successfully!" << std::endl;
        }
        actions--;
        return true;
    }
    
    if (defendCard.get_name() == "Look Into My Eyes") {
        int boost = attackcard.getboost();
        defenseValue = defendCard.getdefense() + boost;
        std::cout << "Look Into My Eyes: Defense boosted by " << boost << " (Total: " << defenseValue << ")" << std::endl;
        damage = attackValue - defenseValue;
        if (damage > 0) {
            attacker_won = true;
        } else {
            attacker_won = false;
        }
    }
    
    if (attackcard.get_name() == "Elementary") {
        std::cout << "Elementary: Predict opponent's attack value (1-6): ";
        int predicted;
        std::cin >> predicted;
        int actualAttack = attackValue;
        if (predicted == actualAttack) {
            std::cout << "Correct prediction! All effects canceled and attack ignored." << std::endl;
            damage = 0;
            attacker_won = false;
        } else {
            std::cout << "Wrong prediction! Attack value: " << actualAttack << std::endl;
        }
    }
    
    if (attackcard.get_name() == "Deduce Strategy") {
        int boost = defendCard.getboost();
        if (boost > 0) {
            const_cast<card*>(&defendCard)->setdefense(boost);
            std::cout << "Deduce Strategy: Defense changed to boost value (" << boost << ")" << std::endl;
            damage = attackValue - boost;
            if (damage > 0) {
                attacker_won = true;
            } else {
                attacker_won = false;
            }
        }
    }
    
    if (attackcard.get_name() == "Counter Punch") {
        if (adjacent) {
            target.takedamage(2);
            std::cout << "Counter Punch: Dealt 2 damage (adjacent)" << std::endl;
        } else {
            std::cout << "Counter Punch: No damage (not adjacent)" << std::endl;
        }
    }
    
    if (attackcard.get_name() == "Fixed Point") {
        if (adjacent) {
            heal(1);
            target.heal(1);
            std::cout << "Fixed Point: Healed both fighters for 1" << std::endl;
        }
    }
    
    if (damage > 0) {
        target.takedamage(damage);
        std::cout << target.getname() << " took " << damage << " damage" << std::endl;
    } else {
        std::cout << target.getname() << " defended successfully" << std::endl;
    }
    
    if (attackcard.get_name() == "Education Never Ends") {
        if (attacker_won) {
            target.drawcard();
            std::cout << "Education Never Ends: Opponent draws 1 card" << std::endl;
        } else {
            drawcard();
            drawcard();
            std::cout << "Education Never Ends: You draw 2 cards" << std::endl;
        }
    }
    
    if (attackcard.get_name() == "Study Methods") {
        if (attacker_won) {
            std::cout << "Study Methods: Opponent's hand:" << std::endl;
            for (const auto& c : target.gethand()) {
                std::cout << "  " << c.get_name() << std::endl;
            }
        }
    }
    
    if (attackcard.get_name() == "The Game is Afoot") {
        auto neighbors = board.getNeighborIds(getx());
        if (neighbors.size() >= 3) {
            setposition(neighbors[2]);
            std::cout << "The Game is Afoot: Moved 3 spaces to n" << neighbors[2] << std::endl;
        }
    }
    
    if (attackcard.get_name() == "Master of Disguise") {
        int tempPos = getposition();
        setposition(target.getx());
        target.setposition(tempPos);
        target.takedamage(1);
        std::cout << "Master of Disguise: Swapped positions and dealt 1 damage" << std::endl;
    }
    
    if (attackcard.get_name() == "Administer Aid") {
        heal(1);
        drawcard();
        std::cout << "Administer Aid: Healed 1 HP and drew 1 card" << std::endl;
    }
    
    if (attackcard.get_name() == "Ambush") {
        if (target.handsize() > 0) {
            size_t random_index = rand() % target.handsize();
            card discardcard = target.gethand()[random_index];
            damage += discardcard.getboost();
            target.gethand().erase(target.gethand().begin() + random_index);
            std::cout << "Ambush: Opponent discarded " << discardcard.get_name() << " (Boost: " << discardcard.getboost() << ")" << std::endl;
            target.takedamage(discardcard.getboost());
        }
    }
    
    if (attackcard.get_name() == "Beastform") {
        size_t cards_in_hand = hand.size();
        if (cards_in_hand > 0) {
            int discardcards = 0;
            std::cout << "How many cards to discard? (Max: " << cards_in_hand << "): ";
            std::cin >> discardcards;
            if (discardcards > static_cast<int>(cards_in_hand)) discardcards = cards_in_hand;
            if (discardcards < 0) discardcards = 0;
            
            for (int i = 0; i < discardcards; i++) {
                hand.erase(hand.begin() + i);
                damage += 1;
            }
            std::cout << "Beastform: Discarded " << discardcards << " cards, attack +" << discardcards << std::endl;
            target.takedamage(discardcards);
        }
    }
    
    if (attackcard.get_name() == "Feeding Frenzy") {
        std::cout << "Feeding Frenzy: +1 attack for each Sister adjacent to opponent" << std::endl;
    }
    
    if (attackcard.get_name() == "Dash") {
        auto neighbors = board.getNeighborIds(getx());
        if (neighbors.size() >= 3) {
            setposition(neighbors[2]);
            std::cout << "Dash: Moved to n" << neighbors[2] << std::endl;
        }
    }
    
    if (attackcard.get_name() == "Thirst for Sustenance") {
        if (attacker_won) {
            auto defNeighbors = board.getNeighborIds(target.getx());
            if (!defNeighbors.empty()) {
                setposition(defNeighbors[0]);
                std::cout << "Thirst for Sustenance: Placed adjacent to opponent at n" << defNeighbors[0] << std::endl;
            }
        } else {
            std::cout << "Thirst for Sustenance: You did not win the combat!" << std::endl;
        }
    }
    
    if (attackcard.get_name() == "Exploit") {
        drawcard();
        std::cout << "Exploit: Drew 1 card" << std::endl;
    }
    
    if (defendCard.get_name() == "Exploit") {
        target.drawcard();
        std::cout << "Exploit: Opponent drew 1 card" << std::endl;
    }
    
    if (attackcard.get_name() == "Confirm Suspicion") {
        std::cout << "Confirm Suspicion: Choose a value (1-6): ";
        int value;
        std::cin >> value;
        bool found = false;
        auto& targetHand = target.gethand();
        for (size_t i = 0; i < targetHand.size(); i++) {
            if (targetHand[i].getattack() == value || targetHand[i].getdefense() == value) {
                int boostValue = targetHand[i].getboost();
                target.takedamage(boostValue);
                std::cout << "Card with value " << value << " found! Dealt " << boostValue << " damage." << std::endl;
                targetHand.erase(targetHand.begin() + i);
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << "No card with value " << value << ". Opponent reveals hand!" << std::endl;
            for (const auto& c : targetHand) {
                std::cout << "  " << c.get_name() << " (ATK:" << c.getattack() << " DEF:" << c.getdefense() << ")" << std::endl;
            }
        }
    }
    
    if (attackcard.get_name() == "Eliminate the Impossible") {
        auto& targetHand = target.gethand();
        std::cout << "Eliminate the Impossible: Opponent's hand:" << std::endl;
        for (size_t i = 0; i < targetHand.size(); i++) {
            std::cout << "  [" << i << "] " << targetHand[i].get_name() << std::endl;
        }
        std::cout << "Choose a card to burn (index): ";
        int idx;
        std::cin >> idx;
        if (idx >= 0 && idx < (int)targetHand.size()) {
            std::cout << "Burned: " << targetHand[idx].get_name() << std::endl;
            targetHand.erase(targetHand.begin() + idx);
        }
    }

    actions--;
    return true;
}

bool hero::canact() const {
    return actions > 0;
}