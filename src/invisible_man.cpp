#include "../include/invisible_man.hpp"

InvisibleMan::InvisibleMan(std::string name, int startNode)
    : hero(name, 15, 2)
{
    this->setposition(startNode);
    fogPositions = {startNode, startNode, startNode};
    initializeDeck();
}

const std::vector<int> &InvisibleMan::getFogPositions() const
{
    return fogPositions;
}

void InvisibleMan::setFogPosition(int index, int nodeID)
{
    if (index >= 0 && index < static_cast<int>(fogPositions.size()))
    {
        fogPositions[index] = nodeID;
    }
}

bool InvisibleMan::isFogOnNode(int nodeID) const
{
    for (int fogNode : fogPositions)
    {
        if (fogNode == nodeID)
        {
            return true;
        }
    }
    return false;
}

bool InvisibleMan::isOnFog() const
{
    return isFogOnNode(this->getx());
}

int InvisibleMan::getDefenseBonus() const
{
    if (isOnFog())
    {
        return 1;
    }
    return 0;
}

void InvisibleMan::initializeDeck()
{
    getdeck().reset();

    for (int i = 0; i < 2; ++i)
        getdeck().addcard(card("VANISH", cardtype::scheme, 0, 0, 3, cardowner::any, "Recover 1 health. Remove Invisible Man from the board. At the start of your next turn, place Invisible Man in any space."));

    for (int i = 0; i < 2; ++i)
        getdeck().addcard(card("REIGN OF TERROR", cardtype::scheme, 0, 0, 1, cardowner::any, "If Invisible Man is on a space with a fog token, deal 2 damage to any one opposing fighter."));

    for (int i = 0; i < 2; ++i)
        getdeck().addcard(card("ROLLING FOG", cardtype::scheme, 0, 0, 1, cardowner::any, "Move 1 fog token to another space. Gain 1 action."));

    for (int i = 0; i < 2; ++i)
        getdeck().addcard(card("STEP LIGHTLY", cardtype::scheme, 0, 0, 1, cardowner::any, "Deal 1 damage to one adjacent fighter. If Invisible Man is on a space with a fog token, deal 3 damage instead. Your opponent then moves a fog token up to 2 spaces."));

    for (int i = 0; i < 2; ++i)
        getdeck().addcard(card("CONFOUND", cardtype::multipurpose, 3, 3, 2, cardowner::any, "AFTER COMBAT: Your opponent may choose to discard 1 card. If they do not, you may move each fog token to any other space."));

    for (int i = 0; i < 3; ++i)
        getdeck().addcard(card("COVERT PREPARATION", cardtype::multipurpose, 2, 2, 1, cardowner::any, "AFTER COMBAT: Draw 1 card. Move 1 fog token up to 2 spaces, then your opponent moves a different fog token up to 2 spaces."));

    for (int i = 0; i < 2; ++i)
        getdeck().addcard(card("DREAMING OF REVENGE", cardtype::multipurpose, 3, 3, 1, cardowner::any, "AFTER COMBAT: If Invisible Man is on a space with a fog token, all opposing fighters on spaces with fog tokens take 1 damage."));

    for (int i = 0; i < 2; ++i)
        getdeck().addcard(card("IMPOSSIBLE TO SEE", cardtype::multipurpose, 2, 2, 2, cardowner::any, "IMMEDIATELY: The value of your opponent's attack or defense is 0 and cannot be changed by card effects."));

    for (int i = 0; i < 2; ++i)
        getdeck().addcard(card("CODED NOTES", cardtype::defense, 0, 3, 2, cardowner::any, "AFTER COMBAT: Draw 3 cards, then choose 2 cards from your hand and put them on top of your deck in any order."));

    for (int i = 0; i < 2; ++i)
        getdeck().addcard(card("INTO THIN AIR", cardtype::defense, 0, 4, 1, cardowner::any, "AFTER COMBAT: Move Invisible Man up to 1 space. Your opponent then moves a fog token up to 3 spaces."));

    for (int i = 0; i < 2; ++i)
        getdeck().addcard(card("LURKING", cardtype::defense, 0, 2, 2, cardowner::any, "AFTER COMBAT: Draw 1 card and choose 1 effect: move Invisible Man to a space with a fog token OR move 1 fog token up to 3 spaces."));

    for (int i = 0; i < 2; ++i)
        getdeck().addcard(card("EMERGE FROM MIST", cardtype::attack, 3, 0, 2, cardowner::any, "DURING COMBAT: If Invisible Man started this turn on a space with a fog token, this card's value is 5 instead."));

    for (int i = 0; i < 3; ++i)
        getdeck().addcard(card("SLIP AWAY", cardtype::attack, 3, 0, 2, cardowner::any, "AFTER COMBAT: Move 1 fog token to a space without a fighter, then place Invisible Man on that space."));
}

void InvisibleMan::useability()
{
}