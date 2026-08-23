#include "../include/dracula.hpp"
#include "../include/deck-builder.hpp"

#include <iostream>


dracula::dracula()
    :
    hero("Dracula",13,2),
    allCharactersPtr(nullptr)
{

    deck d = draculadeck();

    d.shuffle();


    while(!d.isempty())
    {
        getdeck().addcard(d.drawcard());
    }


    drawhand();
}



void dracula::setAllCharacters(std::vector<character*>* chars)
{
    allCharactersPtr = chars;
}



void dracula::useability()
{
    // Rulebook: "at the start of your turn, you MAY deal 1 damage to a
    // fighter -- even your own Sisters -- adjacent to Dracula. If you do,
    // draw a card." That's the player's choice of WHICH fighter, so this no
    // longer auto-picks the first one found and applies it -- it just
    // reports whether the ability is available. The actual damage+draw now
    // happens through useAbilityOn(), called once the player clicks a
    // target via the "SPECIAL ABILITY" button in the UI.
    Board* board = getBoard();
    if (!allCharactersPtr || !board) return;

    for (auto* c : *allCharactersPtr) {
        if (c == this || !c->isalive()) continue;
        if (board->isAdjacent(getposition(), c->getposition())) {
            std::cout << "Dracula's Blood Drain is available this turn.\n";
            return;
        }
    }
}

bool dracula::useAbilityOn(character* target)
{
    Board* board = getBoard();
    if (!target || !board || !target->isalive()) return false;
    if (target == this) return false;
    if (!board->isAdjacent(getposition(), target->getposition())) return false;

    target->takedamage(1);
    drawcard();

    std::cout << "Dracula damaged " << target->getname() << " and drew a card\n";
    return true;
}