#include "../include/invisible_man.hpp"
#include "../include/deck-builder.hpp"
#include "../include/map.hpp"
#include <iostream>
#include <algorithm>
#include <queue>
#include <unordered_set>

// ---------------------------------------------------------------------------
// Small local helpers. Kept file-local so no header changes are required.
//
// isWithinSteps: BFS reachability check used to validate "move a fog token
// up to N spaces" style effects (Covert Preparation, Into Thin Air, ...).
//
// otherFogNodes: every fog card that moves ONE token must never drop it onto
// a space already held by one of the OTHER two fog tokens -- the rulebook is
// explicit that "at any moment, only one fog token can be on a space." This
// returns the positions to avoid when moving the token at excludeIdx.
//
// pickFogMoveDestination: none of the fog/positioning cards below have a
// real player-facing target picker yet (there's no TargetPrompt wired up
// for them, unlike Mistform/Ravening Seduction). The previous
// implementation used blocking std::cin >> calls to ask for a choice on
// the console -- but this is a raylib GUI app with no visible console
// input for the player, so every one of those calls froze the whole
// window the instant the card was played. This BFS helper picks a
// deterministic destination (the farthest reachable, optionally
// unoccupied, space within maxSteps) instead, so these cards always
// resolve immediately without ever blocking. It's a simplification --
// ideally each of these gets a proper click-to-target flow like Mistform
// has -- but it keeps the game playable and non-blocking in the meantime.
//
// pickAnyOtherSpace: a handful of cards (Rolling Fog, and the auto-pick
// fallback for Confound) move a fog token "to another space" / "to any
// other space" with NO distance limit at all -- unlike the "up to N
// spaces" cards above, which do use step-limited BFS movement. This picks
// any legal space on the board other than the token's current one.
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

static std::vector<int> otherFogNodes(const std::vector<int>& fogPositions, int excludeIdx) {
    std::vector<int> out;
    for (size_t i = 0; i < fogPositions.size(); ++i) {
        if (static_cast<int>(i) != excludeIdx) out.push_back(fogPositions[i]);
    }
    return out;
}

static int pickFogMoveDestination(const Board* b, const std::vector<character*>* allChars,
                                   int fromNode, int maxSteps, bool requireUnoccupied,
                                   const std::vector<int>* avoidNodes = nullptr) {
    if (!b) return -1;

    std::queue<int> q;
    std::unordered_set<int> visited;
    q.push(fromNode);
    visited.insert(fromNode);

    int best = -1;
    int steps = std::max(1, maxSteps);

    for (int s = 0; s < steps; ++s) {
        int levelSize = static_cast<int>(q.size());
        if (levelSize == 0) break;

        for (int i = 0; i < levelSize; ++i) {
            int current = q.front();
            q.pop();

            for (int neighbor : b->getNeighborIds(current)) {
                if (visited.count(neighbor)) continue;
                visited.insert(neighbor);
                q.push(neighbor);

                bool occupied = false;
                if (requireUnoccupied && allChars) {
                    for (character* c : *allChars) {
                        if (c && c->isalive() && c->getx() == neighbor) { occupied = true; break; }
                    }
                }

                // Rulebook: only one fog token may occupy a space at a time,
                // so a space already held by a *different* fog token is
                // never a legal final destination. It can still be passed
                // through during the BFS above -- that's why this only
                // disqualifies `neighbor` from becoming `best`, it doesn't
                // stop traversal through it.
                bool blockedByOtherFog = false;
                if (avoidNodes) {
                    for (int avoid : *avoidNodes) {
                        if (avoid == neighbor) { blockedByOtherFog = true; break; }
                    }
                }

                if (!occupied && !blockedByOtherFog) {
                    best = neighbor; // keeps overwriting -> ends up as the farthest valid pick found
                }
            }
        }
    }
    return best;
}

static int pickAnyOtherSpace(Board* b, int excludeNode, const std::vector<int>* avoidNodes = nullptr) {
    if (!b) return -1;

    for (const std::string& spaceId : b->getAllSpaceIds()) {
        int node = b->getNodeId(spaceId);
        if (node == excludeNode) continue;

        bool blocked = false;
        if (avoidNodes) {
            for (int avoid : *avoidNodes) {
                if (avoid == node) { blocked = true; break; }
            }
        }
        if (!blocked) return node;
    }
    return -1;
}

InvisibleMan::InvisibleMan()
    // Rulebook: Invisible Man has 15 Health / 2 Movement.
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
    // like a Sidekick deployment).
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
    if (currentPos < 0) return false; // off the board (Vanish) -- can't be "on" anything
    for (int fogPos : fogPositions) {
        if (currentPos == fogPos) {
            return true;
        }
    }
    return false;
}

void InvisibleMan::onTurnStart() {
    if (vanished) {
        // Rulebook: place Invisible Man on ANY space you choose. That now
        // happens for real via TargetPrompt::VanishNode -- raylib.cpp opens
        // that prompt right after this call returns (checking
        // isVanished()), and resolveVanish() below is what actually
        // finishes the job once the player clicks a space. Position stays
        // off-board (-1) and startedTurnOnFog is intentionally NOT touched
        // here; both are settled in resolveVanish() once we know where he
        // actually ends up.
        return;
    }
    startedTurnOnFog = isOnFog();
}

void InvisibleMan::resolveVanish(int node) {
    if (!vanished) return;
    setposition(node);
    vanished = false;
    startedTurnOnFog = isOnFog();
    std::cout << getname() << " reappears at n" << node << "." << std::endl;
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
    // defenseValue inside executeDefenseCardEffects() below. This override
    // is a pure passthrough (no flat damage reduction here).
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
        // Rulebook: deal 2 damage to EACH of the opponent's fighters (not
        // just one). Invisible Man never has Sidekicks of his own, so
        // "everyone else on the board" and "the opponent's fighters" are
        // the same set for him -- this mirrors the same allCharacters-minus-
        // self pattern already used by Dreaming of Revenge below.
        if (isOnFog()) {
            bool dealtAny = false;
            if (allCharacters) {
                for (character* c : *allCharacters) {
                    if (!c || c == this || !c->isalive()) continue;
                    c->takedamage(2);
                    std::cout << c->getname() << " took 2 damage (Reign of Terror)." << std::endl;
                    dealtAny = true;
                }
            }
            if (!dealtAny) {
                // allCharacters isn't wired up at this call site -- fall
                // back to just the single target we were given so the card
                // still does *something* instead of silently fizzling.
                target.takedamage(2);
                std::cout << getname() << " dealt 2 damage to " << target.getname() << " from the fog!" << std::endl;
            }
        } else {
            std::cout << "Reign of Terror: " << getname() << " must be on a fog token." << std::endl;
        }
        return true;
    }

    if (name == "Rolling Fog") {
        // Rulebook: "Move 1 fog token to another space" -- no distance
        // limit is printed on the card, so this isn't a step-limited BFS
        // move like the "up to N spaces" cards; any other legal space on
        // the board works.
        if (!fogPositions.empty() && b) {
            int idx = 0;
            auto avoid = otherFogNodes(fogPositions, idx);
            int dest = pickAnyOtherSpace(b, fogPositions[idx], &avoid);
            if (dest >= 0) {
                setFogPosition(idx, dest);
                std::cout << getname() << " moved the fog token to n" << dest << "." << std::endl;
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

        if (!fogPositions.empty() && b) {
            int idx = 0;
            auto avoid = otherFogNodes(fogPositions, idx);
            int dest = pickFogMoveDestination(b, allCharacters, fogPositions[idx], 2, false, &avoid);
            if (dest >= 0) {
                setFogPosition(idx, dest);
            }
        }
        return true;
    }

    if (name == "Vanish") {
        // Rulebook: recover 1 health, remove Invisible Man from the board
        // entirely, then place him on any space of your choosing at the
        // start of your NEXT turn. He is now genuinely taken off the board
        // (position -1, via onTurnStart()/isVanished() below) for the rest
        // of this turn and the opponent's whole turn -- he can't be
        // targeted, attacked, or treated as adjacent to anything while
        // vanished. The "any space of your choosing" part is now a real
        // click-to-target prompt (TargetPrompt::VanishNode, opened by
        // raylib.cpp right after onTurnStart() at the start of the next
        // turn) resolved by resolveVanish() -- see invisible_man.hpp/.cpp
        // and actionbar.cpp. The "end your turn if played as your first
        // action" clause still isn't enforced -- that needs access to the
        // action-economy/turn-ending code in the main game loop.
        heal(1);
        vanished = true;
        setposition(-1);
        std::cout << getname() << " recovered 1 HP and vanished from the board." << std::endl;
        return true;
    }

    if (name == "Confound") {
        // Rulebook: "Opponent may discard 1 card. If they don't, you may
        // move any fog token to any other space." There's no opponent-AI
        // here to make that discard choice, so this always takes the
        // "they didn't discard" branch and lets the player move a fog
        // token -- to wherever they clicked (getConfoundFogTarget()), with
        // no distance limit, matching the card text. Falls back to an
        // unrestricted (still no distance limit -- "any other space") pick
        // only if no target was set, e.g. Confound played as an
        // attack/defense card, which doesn't have this click-to-target
        // flow wired up yet.
        if (b) {
            std::string picked = getConfoundFogTarget();
            auto avoid = otherFogNodes(fogPositions, 0);
            bool clashesWithOtherFog = false;
            if (!picked.empty() && b->hasSpace(picked)) {
                int dest = b->getNodeId(picked);
                for (int a : avoid) { if (a == dest) { clashesWithOtherFog = true; break; } }
                if (!clashesWithOtherFog) {
                    setFogPosition(0, dest);
                    std::cout << getname() << " moved a fog token to " << picked << " (Confound)." << std::endl;
                }
            }
            if (picked.empty() || !b->hasSpace(picked) || clashesWithOtherFog) {
                if (!fogPositions.empty()) {
                    int dest = pickAnyOtherSpace(b, fogPositions[0], &avoid);
                    if (dest >= 0) {
                        setFogPosition(0, dest);
                        std::cout << getname() << " moved a fog token to n" << dest << " (Confound)." << std::endl;
                    }
                }
            }
            setConfoundFogTarget("");
        }
        return true;
    }

    if (name == "Covert Preparation") {
        drawcard();

        int idx = 0;
        if (!fogPositions.empty() && b) {
            auto avoid = otherFogNodes(fogPositions, idx);
            int dest = pickFogMoveDestination(b, allCharacters, fogPositions[idx], 2, false, &avoid);
            if (dest >= 0) {
                setFogPosition(idx, dest);
            }
        }

        if (b) {
            // Rulebook: the opponent is pulled toward "another" fog token --
            // i.e. one distinct from the one that was just moved above --
            // so idx is skipped here.
            for (size_t i = 0; i < fogPositions.size(); ++i) {
                if (static_cast<int>(i) == idx) continue;
                int fogPos = fogPositions[i];
                if (isWithinSteps(b, target.getx(), fogPos, 2)) {
                    target.setposition(fogPos);
                    std::cout << target.getname() << " was pulled toward the fog to n" << fogPos << "." << std::endl;
                    break;
                }
            }
        }

        std::cout << getname() << " drew a card and shifted the fog." << std::endl;
        return true;
    }

    if (name == "Dreaming of Revenge") {
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
        return true;
    }

    if (name == "Impossible to See") {
        return true;
    }

    return false;
}

void InvisibleMan::executeAttackCardEffects(card& attackCard, character& target, int& attackValue, int& defenseValue, bool& attackerWon, bool& effectsCanceled, const card& defenseCard, bool defenseCardProtected) {
    std::string name = attackCard.get_name();
    Board* b = getBoard();

    if (name == "Emerge from Mist") {
        // Rulebook: this is a 5 if Invisible Man started THIS turn on a fog
        // token -- not just whether he's on one right now (he could have
        // moved mid-turn, e.g. via Slip Away). startedTurnOnFog is recorded
        // by onTurnStart(), which the game loop calls exactly once when his
        // turn begins (see RunGameUI in raylib.cpp).
        if (startedTurnOnFog) {
            attackValue = 5;
            std::cout << getname() << " strikes from the fog with extra power (ATK: 5)!" << std::endl;
        }
    }
    else if (name == "Slip Away") {
        // Rulebook: move a fog token to a space without a fighter, then
        // place Invisible Man on that space. No distance limit is printed
        // on the card, so any unoccupied space on the board is valid --
        // but it still can't be a space already held by one of the OTHER
        // two fog tokens (only one fog token per space, ever).
        if (!fogPositions.empty() && b) {
            int idx = 0;
            auto avoid = otherFogNodes(fogPositions, idx);
            int dest = -1;
            for (const std::string& spaceId : b->getAllSpaceIds()) {
                int node = b->getNodeId(spaceId);
                bool occupied = false;
                if (allCharacters) {
                    for (character* c : *allCharacters) {
                        if (c && c->isalive() && c->getx() == node) { occupied = true; break; }
                    }
                }
                if (occupied) continue;

                bool clashesWithOtherFog = false;
                for (int a : avoid) { if (a == node) { clashesWithOtherFog = true; break; } }
                if (clashesWithOtherFog) continue;

                dest = node;
                break;
            }

            if (dest >= 0) {
                setFogPosition(idx, dest);
                setposition(dest);
                std::cout << getname() << " slipped away to n" << dest << "." << std::endl;
            } else {
                std::cout << "Slip Away: no unoccupied space found." << std::endl;
            }
        }
    }
    else if (name == "Confound") {
        // Rulebook: "Opponent may discard 1 card. If they don't, you may
        // move any fog token to any other space." No opponent-AI to make
        // that discard choice, so this always takes the "they didn't
        // discard" branch. Uses the space the player actually clicked
        // (getConfoundFogTarget()) if one was set; falls back to an
        // unrestricted (no distance limit) auto-pick otherwise. Either way
        // the destination can't collide with one of the other fog tokens.
        if (b) {
            std::string picked = getConfoundFogTarget();
            auto avoid = otherFogNodes(fogPositions, 0);
            bool clashesWithOtherFog = false;
            if (!picked.empty() && b->hasSpace(picked)) {
                int dest = b->getNodeId(picked);
                for (int a : avoid) { if (a == dest) { clashesWithOtherFog = true; break; } }
                if (!clashesWithOtherFog) {
                    setFogPosition(0, dest);
                    std::cout << getname() << " moved a fog token to " << picked << " (Confound)." << std::endl;
                }
            }
            if (picked.empty() || !b->hasSpace(picked) || clashesWithOtherFog) {
                if (!fogPositions.empty()) {
                    int dest = pickAnyOtherSpace(b, fogPositions[0], &avoid);
                    if (dest >= 0) {
                        setFogPosition(0, dest);
                        std::cout << getname() << " moved a fog token to n" << dest << " (Confound)." << std::endl;
                    }
                }
            }
            setConfoundFogTarget("");
        }
    }
    else if (name == "Impossible to See") {
        // Rulebook: "your OPPONENT's attack or defense value is 0 and can't
        // be changed by card effects. Other card effects still happen."
        // As the attacker, that means the opponent's *defense* value is
        // zeroed -- not our own attack value. Sherlock's passive makes a
        // Sherlock/Watson-owned defense card immune to this.
        if (!defenseCardProtected) {
            defenseValue = 0;
            std::cout << getname() << " renders the opponent's defense meaningless (value: 0)!" << std::endl;
        } else {
            std::cout << "Impossible to See has no effect on a Sherlock/Watson card." << std::endl;
        }
    }
}

void InvisibleMan::executeDefenseCardEffects(card& defenseCard, const card& attackCard, int& defenseValue, int& attackValue, bool& effectsCanceled, bool attackCardProtected) {
    // Fog +1 defense bonus is applied unconditionally in hero::attack()
    // now, before this function is even reached, since the rulebook says
    // it's not a card effect and can't be Feint-canceled -- this function
    // only runs at all when effects weren't canceled, so applying it here
    // too would double it up on every non-Feint turn.

    std::string name = defenseCard.get_name();
    Board* b = getBoard();

    if (name == "Coded Notes") {
        drawcard();
        drawcard();
        drawcard();
        std::cout << getname() << " drew 3 cards with Coded Notes." << std::endl;

        auto& hand = gethand();
        int handSize = static_cast<int>(hand.size());
        int toReturn = std::min(2, handSize);

        for (int picked = 0; picked < toReturn; ++picked) {
            int idx = static_cast<int>(hand.size()) - 1;
            card chosen = hand[idx];
            hand.erase(hand.begin() + idx);
            getdeck().addcard(chosen);
            std::cout << "Placed " << chosen.get_name() << " on top of your deck." << std::endl;
        }
    }
    else if (name == "Into Thin Air") {
        if (b) {
            auto neighbors = b->getNeighborIds(getposition());
            for (int neighbor : neighbors) {
                bool occupied = false;
                if (allCharacters) {
                    for (character* c : *allCharacters) {
                        if (c && c->isalive() && c->getx() == neighbor) { occupied = true; break; }
                    }
                }
                if (!occupied) {
                    setposition(neighbor);
                    break;
                }
            }
        }

        if (!fogPositions.empty() && b) {
            int idx = 0;
            auto avoid = otherFogNodes(fogPositions, idx);
            int dest = pickFogMoveDestination(b, allCharacters, fogPositions[idx], 3, false, &avoid);
            if (dest >= 0) {
                setFogPosition(idx, dest);
            }
        }
        std::cout << getname() << " fades into thin air." << std::endl;
    }
    else if (name == "Lurking") {
        drawcard();

        if (!isOnFog() && !fogPositions.empty()) {
            int fogNode = fogPositions[0];
            setposition(fogNode);
            std::cout << getname() << " drew a card and moved to n" << fogNode << "." << std::endl;
        } else if (!fogPositions.empty() && b) {
            int idx = 0;
            auto avoid = otherFogNodes(fogPositions, idx);
            int dest = pickFogMoveDestination(b, allCharacters, fogPositions[idx], 3, false, &avoid);
            if (dest >= 0) {
                setFogPosition(idx, dest);
                std::cout << getname() << " drew a card and shifted the fog." << std::endl;
            }
        }
    }
    else if (name == "Impossible to See") {
        // As the defender, this zeroes the opponent's *attack* value --
        // unless it's a Sherlock/Watson-owned attack card, which Sherlock's
        // passive makes immune to being zeroed by an opponent's effect.
        if (!attackCardProtected) {
            attackValue = 0;
            std::cout << getname() << " renders the attack meaningless (value: 0)!" << std::endl;
        } else {
            std::cout << "Impossible to See has no effect on a Sherlock/Watson card." << std::endl;
        }
    }
    else if (name == "Confound") {
        // Same real-target-first, auto-pick-fallback behavior as the
        // attack-side version above.
        if (b) {
            std::string picked = getConfoundFogTarget();
            auto avoid = otherFogNodes(fogPositions, 0);
            bool clashesWithOtherFog = false;
            if (!picked.empty() && b->hasSpace(picked)) {
                int dest = b->getNodeId(picked);
                for (int a : avoid) { if (a == dest) { clashesWithOtherFog = true; break; } }
                if (!clashesWithOtherFog) {
                    setFogPosition(0, dest);
                    std::cout << getname() << " moved a fog token to " << picked << " (Confound)." << std::endl;
                }
            }
            if (picked.empty() || !b->hasSpace(picked) || clashesWithOtherFog) {
                if (!fogPositions.empty()) {
                    int dest = pickAnyOtherSpace(b, fogPositions[0], &avoid);
                    if (dest >= 0) {
                        setFogPosition(0, dest);
                        std::cout << getname() << " moved a fog token to n" << dest << " (Confound)." << std::endl;
                    }
                }
            }
            setConfoundFogTarget("");
        }
    }
}