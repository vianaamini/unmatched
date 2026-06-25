#include <iostream>
#include "Deck.hpp"

using namespace std;

int main() {
    deck d;

    card c1("Feint", cardtype::Attack, 2);
    card c2("Bite", cardtype::Attack, 4);

    d.addcard(c1);
    d.addcard(c2);

    cout << "Size: " << d.getsize() << endl;

    card drawn = d.drawcard();
    cout << "Drawn: " << drawn.get_name() << endl;

    cout << "Size after draw: " << d.getsize() << endl;
}