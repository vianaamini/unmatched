#pragma once
#include "sherlock.hpp"
#include "dracula.hpp"
#include "watson.hpp"
#include "sister.hpp"

class Game {
private:
    sherlock* player1;
    dracula* player2;
    watson* w;
    sister* sister[3];

    int curturn;
    int action;
    bool gameover;
    std::vector<character*> allEnemies;

    public:
    game();
    ~game();

    void startgame();
    void nextturn();

    void playcard(hero* player,int index, hero* opponent);
    void resolve(hero& attacker, card& atkcard, hero& defender,card& defendcard);

    void winner();
    void state() const;

    bool isgameover() const;
    hero* curplayer() const;
};