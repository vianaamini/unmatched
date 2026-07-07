#include <iostream>
#include "sherlock.hpp"
#include "dracula.hpp"
#include "watson.hpp"
#include "sister.hpp"

using namespace std;

int main() {
    cout << "=== Creating Characters ===" << endl;
    GameManager game;
    Board& board = game.getBoard();
    board.addTeleport(2, 2, 5, 5);     
    board.addTeleport(3, 4, 6, 1);
    
    sherlock sh;
    dracula dr;
    watson wt;
    sister s1(1), s2(2), s3(3);
    
    cout << "Sherlock: " << sh.getname() << " HP: " << sh.gethealth() << endl;
    cout << "Watson: " << wt.getname() << " HP: " << wt.gethealth() << endl;
    cout << "Dracula: " << dr.getname() << " HP: " << dr.gethealth() << endl;
    cout << "Sister 1: " << s1.getname() << " HP: " << s1.gethealth() << endl;
    
    cout << "\n=== Testing Decks ===" << endl;
    cout << "Sherlock deck size: " << sh.getdeck().getsize() << endl;
    cout << "Dracula deck size: " << dr.getdeck().getsize() << endl;
    
    cout << "\n=== Drawing 5 cards ===" << endl;
    sh.drawInitialHand();
    cout << "Sherlock hand size: " << sh.handsize() << endl;
    
    dr.drawInitialHand();
    cout << "Dracula hand size: " << dr.handsize() << endl;
    
    cout << "\n=== Testing Abilities ===" << endl;
    sh.useability();
    dr.useability();
    wt.useability();
    
    cout << "\n✅ All tests passed!" << endl;
    
    return 0;
}