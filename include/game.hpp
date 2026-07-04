#pragma once
#include "sherlock.hpp"
#include "dracula.hpp"
#include "watson.hpp"
#include "sister.hpp"
#include "deracula_cards.hpp"
#include "sherlock_card.hpp"
#include <vector>

class Game {
private:
    sherlock* player1;
    dracula* player2;
    watson* watsonChar;
    sister* sisters[3];
    
    int curturn;
    int action;
    bool gameover;
    std::vector<character*> allEnemies;

public:
    Game();
    ~Game();
    
    void startGame();
    void nextTurn();
    void playCard(hero* player, int index, hero* opponent);
    
    void checkWinCondition();
    void printGameState() const;
    
    bool isGameOver() const;
    hero* getCurrentPlayer() const;
};