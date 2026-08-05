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

    std::cout
    << "Dracula Ability: Blood Drain\n";


    if(!allCharactersPtr)
    {
        std::cout<<"No characters available\n";
        return;
    }


    Board* board = getBoard();


    if(!board)
    {
        std::cout<<"No board\n";
        return;
    }



    character* target=nullptr;


    for(auto* c:*allCharactersPtr)
    {

        if(c==this)
            continue;


        if(!c->isalive())
            continue;



        if(board->isAdjacent(
            getposition(),
            c->getposition()))
        {
            target=c;
            break;
        }

    }



    if(!target)
    {
        std::cout
        <<"No adjacent enemy\n";

        return;
    }



    target->takedamage(1);


    drawcard();



    std::cout
    <<"Dracula damaged "
    <<target->getname()
    <<" and drew a card\n";

}