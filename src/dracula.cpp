#include "../include/dracula.hpp"
#include "../include/deck-builder.hpp"
// #include "../include/dracula_cards.hpp"  // حذف شده
#include "../include/map.hpp"

#include <iostream>
#include <vector>

using namespace std;

dracula::dracula()
    : hero("Dracula", 13, 2),
      allCharactersPtr(nullptr)
{
    deck draculadk = draculadeck();
    draculadk.shuffle();

    while(!draculadk.isempty())
    {
        getdeck().addcard(draculadk.drawcard());
    }
}

void dracula::useability()
{
    cout << "=== Dracula Ability ===" << endl;
    cout << "Choose an adjacent fighter to deal 1 damage and draw a card." << endl;
    cout << "Enter target node (n1-n21) or 'skip': ";

    string input;
    cin >> input;

    if(input=="skip")
    {
        cout << "Ability skipped." << endl;
        return;
    }

    if(input.empty() || input[0]!='n')
    {
        cout << "Invalid target!" << endl;
        return;
    }

    try
    {
        int targetId = stoi(input.substr(1));
        character* target=nullptr;
        Board* board = getBoard();

        if(board==nullptr)
        {
            cout << "Board not set!" << endl;
            return;
        }

        if(allCharactersPtr)
        {
            for(character* c : *allCharactersPtr)
            {
                if(c && c->getx()==targetId && c->isalive() && c!=this)
                {
                    target=c;
                    break;
                }
            }
        }

        if(!target)
        {
            cout << "No character at node " << input << "!" << endl;
            return;
        }

        if(!board->isAdjacent(getx(), target->getx()))
        {
            cout << "Target is not adjacent to Dracula!" << endl;
            return;
        }

        target->takedamage(1);
        drawcard();

        cout << "Dealt 1 damage to " << target->getname() << " and drew a card." << endl;
    }
    catch(...)
    {
        cout << "Invalid target!" << endl;
    }
}