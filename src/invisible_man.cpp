#include "../include/invisible_man.hpp"
#include "../include/deck-builder.hpp"
#include "../include/map.hpp"
#include <iostream>
#include <algorithm>
#include <queue>
#include <unordered_set>

// ---------------------------------------------------------------------------
// Small local helper: BFS distance check used to validate "move a fog token
// up to N spaces" style card effects (Covert Preparation, Into Thin Air,
// Lurking, ...). Kept file-local so no header changes are required for it.
// ---------------------------------------------------------------------------
static bool isWithinSteps(const Board* b, int from, int to, int steps) {
    if (!b) return false;
    if (from == to) return true;
    if (steps <= 0) return false;

    std::queue<int> q;
    std::unordered_set<int> visited;
    q.push(from);
    visited.insert(from);

    for (int s = 0; s < steps; ++s) {
        int levelSize = static_cast<int>(q.size());
        for (int i = 0; i < levelSize; ++i) {
            int current = q.front();
            q.pop();

            for (int neighbor : b->getNeighborIds(current)) {
                if (neighbor == to) return true;
                if (visited.count(neighbor)) continue;
                visited.insert(neighbor);
                q.push(neighbor);
            }
        }
    }
    return false;
}

InvisibleMan::InvisibleMan()
    // Rulebook: Invisible Man has 15 Health / 2 Movement.
    // (Previously this was set to 14, which under-set his starting HP by 1.)
    : hero("Invisible Man", 15, 2)
{
    deck d = invisiblemandeck();
    d.shuffle();

    while (!d.isempty()) {
        getdeck().addcard(d.drawcard());
    }

    drawhand();
}

void InvisibleMan::initializeFogTokens(int startNode) {
    // Rulebook: at the start of the game, the Invisible Man places his three
    // fog tokens on separate spaces within his own starting Zone (exactly
    // like a Sidekick deployment). The previous implementation just used
    // startNode+1 / startNode+2, which are arbitrary node IDs that may not
    // even belong to the same Zone (or may not exist on the board at all).
    fogPositions.clear();

    Board* b = getBoard();
    if (b && b->hasSpace("n" + std::to_string(startNode))) {
        std::vector<int> candidates;
        auto zones = b->getZonesAt(startNode, 0);
        for (const auto& zone : zones) {
            for (const auto& space : b->getSpacesInZone(zone)) {
                int node = space.first;
                if (node != startNode) {
                    candidates.push_back(node);
                }
            }
        }
        std::sort(candidates.begin(), candidates.end());
        candidates.erase(std::unique(candidates.begin(), candidates.end()), candidates.end());

        for (size_t i = 0; i < candidates.size() && fogPositions.size() < 3; ++i) {
            fogPositions.push_back(candidates[i]);
        }
    }

    // Fallback: if the board isn't wired up yet, or the starting Zone is too
    // small to hold three distinct spaces, don't leave fog tokens undefined.
    while (fogPositions.size() < 3) {
        fogPositions.push_back(startNode);
    }
}

void InvisibleMan::setAllCharacters(std::vector<character*>* chars) {
    allCharacters = chars;
}

bool InvisibleMan::isOnFog() const {
    int currentPos = getposition();
    for (int fogPos : fogPositions) {
        if (currentPos == fogPos) {
            return true;
        }
    }
    return false;
}

std::vector<int> InvisibleMan::getFogPositions() const {
    return fogPositions;
}

void InvisibleMan::setFogPosition(int index, int node) {
    if (index >= 0 && index < static_cast<int>(fogPositions.size())) {
        fogPositions[index] = node;
    }
}

void InvisibleMan::takedamage(int amount) {
    // The Invisible Man's fog bonus is "+1 to the value of a defense card
    // played while standing on a fog token" -- it is applied directly to
    // defenseValue inside executeDefenseCardEffects() below, exactly the way
    // the rulebook describes it (a modifier on the card, not a flat damage
    // reduction on every hit). Reducing incoming damage by 1 here was wrong:
    // it silently soaked 1 damage from *any* source (Dreaming of Revenge,
    // Reign of Terror, deck-exhaustion damage, etc.), not just from a
    // defended combat, and it worked even with "No Defense". So this
    // override is now a pure passthrough.
    character::takedamage(amount);
}

void InvisibleMan::useability() {
    if (isOnFog()) {
        std::cout << getname() << " is on a fog token (+1 to defense card value while defending)." << std::endl;
    }
}

bool InvisibleMan::executeSchemeCard(card& schemeCard, hero& target) {
    std::string name = schemeCard.get_name();
    Board* b = getBoard();

    if (name == "Reign of Terror") {
        // Rulebook: if on a fog token, deal 2 damage to *any* opponent
        // fighter. The scheme-card API only gives us the opposing hero here,
        // so sidekicks (e.g. Dracula's Sisters) can't be targeted through
        // this hook without a wider signature change.
        if (isOnFog()) {
            target.takedamage(2);
            std::cout << getname() << " dealt 2 damage to " << target.getname() << " from the fog!" << std::endl;
        } else {
            std::cout << "Reign of Terror: " << getname() << " must be on a fog token." << std::endl;
        }
        return true;
    }

    if (name == "Rolling Fog") {
        // Rulebook: move a fog token to another space and gain 1 action.
        if (!fogPositions.empty() && b) {
            std::cout << "Rolling Fog - choose a fog token (0-" << (fogPositions.size() - 1) << "): ";
            int idx = 0;
            std::cin >> idx;
            if (idx < 0 || idx >= static_cast<int>(fogPositions.size())) idx = 0;

            std::cout << "Enter destination node (e.g. n5): ";
            std::string node;
            std::cin >> node;

            if (b->hasSpace(node)) {
                setFogPosition(idx, b->getNodeId(node));
                std::cout << getname() << " moved the fog token to " << node << "." << std::endl;
            } else {
                std::cout << "Invalid node, fog token unchanged." << std::endl;
            }
        }
        set_actions(get_actions() + 1);
        return true;
    }

    if (name == "Step Lightly") {
        character* victim = nullptr;
        if (allCharacters && b) {
            for (character* c : *allCharacters) {
                if (!c || c == this || !c->isalive()) continue;
                if (b->isAdjacent(getposition(), c->getx())) {
                    victim = c;
                    break;
                }
            }
        }

        if (victim) {
            int dmg = isOnFog() ? 3 : 1;
            victim->takedamage(dmg);
            std::cout << getname() << " dealt " << dmg << " damage to " << victim->getname() << " with Step Lightly." << std::endl;
        } else {
            std::cout << "Step Lightly: no adjacent fighter to hit." << std::endl;
        }

        // Rulebook: afterwards, the opponent moves one fog token up to 2 spaces.
        if (!fogPositions.empty() && b) {
            std::cout << "Opponent - choose a fog token to move up to 2 spaces (0-" << (fogPositions.size() - 1) << "): ";
            int idx = 0;
            std::cin >> idx;
            std::cout << "Destination node: ";
            std::string node;
            std::cin >> node;

            if (idx >= 0 && idx < static_cast<int>(fogPositions.size()) && b->hasSpace(node)) {
                int dest = b->getNodeId(node);
                if (isWithinSteps(b, fogPositions[idx], dest, 2)) {
                    setFogPosition(idx, dest);
                } else {
                    std::cout << "Destination too far (max 2 spaces), fog token unchanged." << std::endl;
                }
            }
        }
        return true;
    }

    if (name == "Vanish") {
        // Rulebook: recover 1 HP, remove Invisible Man from the board, then
        // place him on any space at the start of your next turn. Placing him
        // straight onto a fog token here is a simplification (the board has
        // no "off-board" character state without a header/turn-manager
        // change), but it keeps him safe from attacks for the rest of this
        // turn, which is the main practical effect of the card.
        heal(1);
        if (!fogPositions.empty()) {
            setposition(fogPositions[0]);
            std::cout << getname() << " recovered 1 HP and vanished to n" << fogPositions[0] << "." << std::endl;
        }
        return true;
    }

    return false;
}

void InvisibleMan::executeAttackCardEffects(card& attackCard, character& target, int& attackValue, int& defenseValue, bool& attackerWon, bool& effectsCanceled, const card& defenseCard) {
    std::string name = attackCard.get_name();
    Board* b = getBoard();

    if (name == "Emerge from Mist") {
        // NOTE: the rulebook checks whether Invisible Man *started this
        // turn* on a fog token, not whether he's on one right now (he could
        // have moved mid-turn, e.g. via Slip Away). There's no turn-start
        // hook exposed to this file, so this checks his current position as
        // a best-effort approximation.
        if (isOnFog()) {
            attackValue = 5;
            std::cout << getname() << " strikes from the fog with extra power (ATK: 5)!" << std::endl;
        }
    }
    else if (name == "Slip Away") {
        // Rulebook: move a fog token to an unoccupied space, then place
        // Invisible Man there.
        if (!fogPositions.empty() && b) {
            std::cout << "Slip Away - choose a fog token (0-" << (fogPositions.size() - 1) << "): ";
            int idx = 0;
            std::cin >> idx;
            std::cout << "Enter an unoccupied destination node: ";
            std::string node;
            std::cin >> node;

            if (idx >= 0 && idx < static_cast<int>(fogPositions.size()) && b->hasSpace(node)) {
                int dest = b->getNodeId(node);
                bool occupied = false;
                if (allCharacters) {
                    for (character* c : *allCharacters) {
                        if (c && c->isalive() && c->getx() == dest) { occupied = true; break; }
                    }
                }
                if (!occupied) {
                    setFogPosition(idx, dest);
                    setposition(dest);
                    std::cout << getname() << " slipped away to n" << dest << "." << std::endl;
                } else {
                    std::cout << "Slip Away: destination is occupied." << std::endl;
                }
            }
        }
    }
    else if (name == "Confound") {
        // Rulebook: opponent MAY discard 1 card. If they don't, you may move
        // any fog token to any space. The previous version discarded
        // automatically (no real choice) and never granted the fog move.
        hero* targetHero = dynamic_cast<hero*>(&target);
        bool opponentDiscarded = false;

        if (targetHero && !targetHero->gethand().empty()) {
            std::cout << targetHero->getname() << " - discard 1 card? (1 = yes, 0 = no): ";
            int choice = 0;
            std::cin >> choice;
            if (choice == 1) {
                auto& enemyHand = targetHero->gethand();
                std::cout << "Choose card index to discard (0-" << (enemyHand.size() - 1) << "): ";
                int idx = 0;
                std::cin >> idx;
                if (idx >= 0 && idx < static_cast<int>(enemyHand.size())) {
                    std::cout << targetHero->getname() << " discarded " << enemyHand[idx].get_name() << " (Confound)." << std::endl;
                    enemyHand.erase(enemyHand.begin() + idx);
                    opponentDiscarded = true;
                }
            }
        }

        if (!opponentDiscarded && !fogPositions.empty() && b) {
            std::cout << "Choose a fog token (0-" << (fogPositions.size() - 1) << ") and a destination node: ";
            int idx = 0;
            std::cin >> idx;
            std::string node;
            std::cin >> node;
            if (idx >= 0 && idx < static_cast<int>(fogPositions.size()) && b->hasSpace(node)) {
                setFogPosition(idx, b->getNodeId(node));
                std::cout << getname() << " moved a fog token (Confound)." << std::endl;
            }
        }
    }
    else if (name == "Covert Preparation") {
        // Rulebook: draw 1 card. Move a fog token up to 2 spaces, then your
        // opponent's fighter is moved up to 2 spaces towards another fog
        // token.
        drawcard();

        if (!fogPositions.empty() && b) {
            std::cout << "Covert Preparation - choose a fog token (0-" << (fogPositions.size() - 1) << ") to move up to 2 spaces: ";
            int idx = 0;
            std::cin >> idx;
            std::cout << "Destination node: ";
            std::string node;
            std::cin >> node;

            if (idx >= 0 && idx < static_cast<int>(fogPositions.size()) && b->hasSpace(node)) {
                int dest = b->getNodeId(node);
                if (isWithinSteps(b, fogPositions[idx], dest, 2)) {
                    setFogPosition(idx, dest);
                } else {
                    std::cout << "Destination too far (max 2 spaces), fog token unchanged." << std::endl;
                }
            }
        }

        if (b) {
            for (int fogPos : fogPositions) {
                if (isWithinSteps(b, target.getx(), fogPos, 2)) {
                    target.setposition(fogPos);
                    std::cout << target.getname() << " was pulled toward the fog to n" << fogPos << "." << std::endl;
                    break;
                }
            }
        }

        std::cout << getname() << " drew a card and shifted the fog." << std::endl;
    }
    else if (name == "Dreaming of Revenge") {
        if (isOnFog() && allCharacters) {
            for (character* c : *allCharacters) {
                if (!c || c == this || !c->isalive()) continue;
                for (int fogPos : fogPositions) {
                    if (c->getx() == fogPos) {
                        c->takedamage(1);
                        std::cout << c->getname() << " took 1 damage (Dreaming of Revenge)." << std::endl;
                        break;
                    }
                }
            }
        }
    }
    else if (name == "Impossible to See") {
        // Rulebook: "your OPPONENT's attack or defense value is 0 and can't
        // be changed by card effects. Other card effects still happen."
        // As the attacker, that means the opponent's *defense* value is
        // zeroed -- not our own attack value -- and other effects are NOT
        // canceled wholesale.
        defenseValue = 0;
        std::cout << getname() << " renders the opponent's defense meaningless (value: 0)!" << std::endl;
    }
}

void InvisibleMan::executeDefenseCardEffects(card& defenseCard, const card& attackCard, int& defenseValue, int& attackValue, bool& effectsCanceled) {
    // Passive ability (not a card effect, so it is applied unconditionally
    // here and is never subject to effectsCanceled / Feint-style
    // cancellation): while defending from a fog token, the defense card's
    // value gets +1.
    if (isOnFog()) {
        defenseValue += 1;
    }

    std::string name = defenseCard.get_name();
    Board* b = getBoard();

    if (name == "Coded Notes") {
        // Rulebook: draw 3 cards, then choose 2 of them to place back on top
        // of your deck (in any order). The previous version just drew 3 and
        // kept them all.
        drawcard();
        drawcard();
        drawcard();
        std::cout << getname() << " drew 3 cards with Coded Notes." << std::endl;

        auto& hand = gethand();
        int handSize = static_cast<int>(hand.size());
        int toReturn = std::min(2, handSize);

        for (int picked = 0; picked < toReturn; ++picked) {
            std::cout << "Coded Notes - choose a card from your hand to place on top of your deck (0-" << (static_cast<int>(hand.size()) - 1) << "): ";
            int idx = 0;
            std::cin >> idx;
            if (idx < 0 || idx >= static_cast<int>(hand.size())) idx = static_cast<int>(hand.size()) - 1;

            card chosen = hand[idx];
            hand.erase(hand.begin() + idx);
            getdeck().addcard(chosen); // deck::drawcard() pops from the back, so this puts it on top.
            std::cout << "Placed " << chosen.get_name() << " on top of your deck." << std::endl;
        }
    }
    else if (name == "Into Thin Air") {
        // Rulebook: move Invisible Man 1 space, then move a fog token 3 spaces.
        if (b) {
            auto neighbors = b->getNeighborIds(getposition());
            if (!neighbors.empty()) {
                std::cout << "Into Thin Air - move 1 space. Enter destination node: ";
                std::string node;
                std::cin >> node;
                if (b->hasSpace(node)) {
                    int dest = b->getNodeId(node);
                    if (std::find(neighbors.begin(), neighbors.end(), dest) != neighbors.end()) {
                        setposition(dest);
                    } else {
                        std::cout << "That space isn't adjacent, move skipped." << std::endl;
                    }
                }
            }
        }

        if (!fogPositions.empty() && b) {
            std::cout << "Choose a fog token (0-" << (fogPositions.size() - 1) << ") to move up to 3 spaces: ";
            int idx = 0;
            std::cin >> idx;
            std::cout << "Destination node: ";
            std::string node;
            std::cin >> node;
            if (idx >= 0 && idx < static_cast<int>(fogPositions.size()) && b->hasSpace(node)) {
                int dest = b->getNodeId(node);
                if (isWithinSteps(b, fogPositions[idx], dest, 3)) {
                    setFogPosition(idx, dest);
                } else {
                    std::cout << "Destination too far (max 3 spaces), fog token unchanged." << std::endl;
                }
            }
        }
        std::cout << getname() << " fades into thin air." << std::endl;
    }
    else if (name == "Lurking") {
        // Rulebook: draw 1 card, then choose ONE of: move Invisible Man to a
        // fog token, OR move a fog token up to 3 spaces. Previously this
        // always moved him to fogPositions[0] with no choice offered.
        drawcard();

        std::cout << "Lurking - choose effect (1 = move to a fog token, 2 = move a fog token up to 3 spaces): ";
        int choice = 0;
        std::cin >> choice;

        if (choice == 1 && !fogPositions.empty()) {
            std::cout << "Choose which fog token (0-" << (fogPositions.size() - 1) << "): ";
            int idx = 0;
            std::cin >> idx;
            if (idx < 0 || idx >= static_cast<int>(fogPositions.size())) idx = 0;
            setposition(fogPositions[idx]);
            std::cout << getname() << " drew a card and moved to n" << fogPositions[idx] << "." << std::endl;
        } else if (!fogPositions.empty() && b) {
            std::cout << "Choose a fog token (0-" << (fogPositions.size() - 1) << ") to move up to 3 spaces: ";
            int idx = 0;
            std::cin >> idx;
            std::cout << "Destination node: ";
            std::string node;
            std::cin >> node;
            if (idx >= 0 && idx < static_cast<int>(fogPositions.size()) && b->hasSpace(node)) {
                int dest = b->getNodeId(node);
                if (isWithinSteps(b, fogPositions[idx], dest, 3)) {
                    setFogPosition(idx, dest);
                    std::cout << getname() << " drew a card and shifted the fog." << std::endl;
                } else {
                    std::cout << "Destination too far (max 3 spaces), fog token unchanged." << std::endl;
                }
            }
        }
    }
    else if (name == "Impossible to See") {
        // As the defender, "opponent's value" means the attacker's value.
        attackValue = 0;
        std::cout << getname() << " renders the attack meaningless (value: 0)!" << std::endl;
    }
}