
#include "../include/game.hpp"
#include "deracula_cards.hpp"
#include "sherlock_card.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

Game::Game() : curturn(0), action(2), gameover(false) {
    srand(time(nullptr));
    
    player1 = new sherlock();
    player2 = new dracula();
    watsonChar = new watson();
    
    for (int i = 0; i < 3; i++) {
        sisters[i] = new sister(i + 1);
    }
    
    allEnemies.push_back(player2);
    for (int i = 0; i < 3; i++) {
        allEnemies.push_back(sisters[i]);
    }
}

Game::~Game() {
    delete player1;
    delete player2;
    delete watsonChar;
    for (int i = 0; i < 3; i++) {
        delete sisters[i];
    }
}

void Game::startGame() {
    std::cout << "=== Game Started ===" << std::endl;
    
    for (int i = 0; i < 5; i++) {
        player1->drawcard();
        player2->drawcard();
    }
    
    printGameState();
}

void Game::nextTurn() {
    if (gameover) return;
    
    action = 2;
    curturn = (curturn == 0) ? 1 : 0;
    
    std::cout << "\n=== Turn " << curturn + 1 << " ===" << std::endl;
    
    if (curturn == 0) {
        player1->useability();
    } else {
        player2->useability();
    }
    
    printGameState();
}

void Game::playCard(hero* player, int index, hero* opponent) {
    if (gameover) return;
    if (action <= 0) {
        std::cout << "No actions remaining" << std::endl;
        return;
    }
    if (index < 0 || index >= player->handsize()) {
        std::cout << "Invalid card index" << std::endl;
        return;
    }
    
    card playedCard = player->gethand()[index];
    player->gethand().erase(player->gethand().begin() + index);
    
    card defendCard("Empty", cardtype::defense, 0, 0, 0, cardowner::any, "No effect");
    if (opponent->handsize() > 0) {
        defendCard = opponent->gethand()[0];
        opponent->gethand().erase(opponent->gethand().begin());
    }
    
<<<<<<< HEAD
    // Combat Resolution
=======
    resolve(*player, playedCard, *opponent, defendCard);
    
    action--;
    checkWinCondition();
}

void Game::resolve(hero& attacker, card& atkcard, hero& defender, card& defendCard) {
>>>>>>> origin/zahra
    std::cout << "\n=== Combat ===" << std::endl;
    std::cout << player->getname() << " plays: " << playedCard.get_name() << std::endl;
    std::cout << opponent->getname() << " plays: " << defendCard.get_name() << std::endl;
    
    int attackValue = playedCard.getattack();
    int defenseValue = defendCard.getdefense();
    bool isAdjacent = dcards::are_adjacent(*player, *opponent);
    
    // Sherlock card effects
    if (playedCard.get_name() == "Counter Punch") {
        card_counter_punch cp;
        cp.execute_effect(*player, *opponent, defendCard, isAdjacent, false);
    }
    else if (playedCard.get_name() == "Feint") {
        card_feint feint;
        feint.execute_effect(*player, *opponent, defendCard, isAdjacent, false);
    }
    else if (playedCard.get_name() == "Fixed Point") {
        card_fixed_point fp;
        fp.execute_effect(*player, *opponent, defendCard, isAdjacent, false);
    }
    else if (playedCard.get_name() == "Master of Disguise") {
        card_master_of_disguise md;
        md.execute_effect(*player, *opponent, defendCard, isAdjacent, false);
    }
    else if (playedCard.get_name() == "The Game is Afoot") {
        card_game_is_afoot ga;
        ga.execute_effect(*player, *opponent, defendCard, isAdjacent, false);
    }
    
    // Dracula card effects
    std::vector<sidekick*> allSisters;
    for (int i = 0; i < 3; i++) {
        allSisters.push_back(sisters[i]);
    }
    
    if (playedCard.get_name() == "Feeding Frenzy" || 
        playedCard.get_name() == "Ambush" ||
        playedCard.get_name() == "Beastform" ||
        playedCard.get_name() == "Dash" ||
        playedCard.get_name() == "Thirst for Sustenance" ||
        playedCard.get_name() == "Exploit") {
        dcards::resolve_combat_effects(playedCard, *player, defendCard, *opponent, allSisters);
    }
    
    int damage = attackValue - defenseValue;
    if (damage > 0) {
        opponent->takedamage(damage);
        std::cout << opponent->getname() << " took " << damage << " damage" << std::endl;
    } else {
        std::cout << opponent->getname() << " defended successfully" << std::endl;
    }
    
<<<<<<< HEAD
    if (playedCard.gettype() == cardtype::scheme) {
        std::vector<character*> enemies;
        enemies.push_back(opponent);
=======
    if (atkcard.gettype() == cardtype::scheme) {
        std::vector<character*> enemyList;
        enemyList.push_back(&defender);
>>>>>>> origin/zahra
        std::vector<sidekick*> allSisters2;
        for (int i = 0; i < 3; i++) {
            allSisters2.push_back(sisters[i]);
        }
<<<<<<< HEAD
        dcards::resolve_scheme(playedCard, *player, *opponent, enemies, allSisters2);
=======
        dcards::resolve_scheme(atkcard, attacker, defender, enemyList, allSisters2);
>>>>>>> origin/zahra
    }
    
    action--;
    checkWinCondition();
}

void Game::checkWinCondition() {
    if (!player1->isalive()) {
        std::cout << "\nDracula Wins!" << std::endl;
        gameover = true;
    }
    if (!player2->isalive()) {
        std::cout << "\nSherlock Wins!" << std::endl;
        gameover = true;
    }
}

void Game::printGameState() const {
    std::cout << "\n=== Game State ===" << std::endl;
    std::cout << "Sherlock: HP " << player1->gethealth() 
              << " at (" << player1->getx() << ", " << player1->gety() << ")" << std::endl;
    std::cout << "Watson: HP " << watsonChar->gethealth() 
              << " at (" << watsonChar->getx() << ", " << watsonChar->gety() << ")" << std::endl;
    std::cout << "Dracula: HP " << player2->gethealth() 
              << " at (" << player2->getx() << ", " << player2->gety() << ")" << std::endl;
    for (int i = 0; i < 3; i++) {
        std::cout << "Sister " << i+1 << ": HP " << sisters[i]->gethealth()
                  << " at (" << sisters[i]->getx() << ", " << sisters[i]->gety() << ")" << std::endl;
    }
    std::cout << "Actions remaining: " << action << std::endl;
    std::cout << "Turn: " << (curturn == 0 ? "Sherlock" : "Dracula") << std::endl;
}

bool Game::isGameOver() const {
    return gameover;
}

hero* Game::getCurrentPlayer() const {
    return (curturn == 0) ? static_cast<hero*>(player1) : static_cast<hero*>(player2);
}