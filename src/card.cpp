#include "../include/card.hpp"


card::card(
    string name,
    cardtype type,
    int attack,
    int defense,
    int boost,
    cardowner owner,
    string effect
)
:
name(name),
type(type),
attackvalue(attack),
defensevalue(defense),
boostvalue(boost),
owner(owner),
effectTiming(timing::none),
effect(effect)
{
}



card::card(
    string name,
    cardtype type,
    int attack,
    int defense,
    int boost,
    cardowner owner,
    timing effectTiming,
    string effect
)
:
name(name),
type(type),
attackvalue(attack),
defensevalue(defense),
boostvalue(boost),
owner(owner),
effectTiming(effectTiming),
effect(effect)
{
}



string card::get_name() const
{
    return name;
}


cardtype card::gettype() const
{
    return type;
}


int card::getattack() const
{
    return attackvalue;
}


int card::getdefense() const
{
    return defensevalue;
}


int card::getboost() const
{
    return boostvalue;
}


cardowner card::getowner() const
{
    return owner;
}


timing card::getTiming() const
{
    return effectTiming;
}


string card::geteffect() const
{
    return effect;
}


void card::setattack(int value)
{
    attackvalue = value;
}


void card::setdefense(int value)
{
    defensevalue = value;
}