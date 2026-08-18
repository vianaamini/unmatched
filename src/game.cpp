#include "../include/game.hpp"
#include "../include/dracula_cards.hpp"
#include "../include/sherlock_card.hpp"
#include "../include/hero.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

Game::Game(GameManager* gm) : curturn(0), action(2), gameover(false), gameManager(gm) {
    srand(time(nullptr));
    
    player1 = new sherlock();
    player2 = new dracula();
    watsonChar = new watson();
    
    for (int i = 0; i < 3; i++) {
        sisters[i] = new sister(i + 1);
    }
    
    enemy.push_back(player2);
    for (int i = 0; i < 3; i++) {
        enemy.push_back(sisters[i]);
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
    
    // Let the hero's attack/scheme method handle the resolution
    Board* board = gameManager ? &gameManager->getBoard() : nullptr;
    if (!board) {
        static Board tempBoard;
        board = &tempBoard;
    }
    
    bool success = false;
    if (playedCard.gettype() == cardtype::attack) {
        success = player->attack(*opponent, playedCard, *board);
    } else if (playedCard.gettype() == cardtype::scheme) {
        success = player->scheme(playedCard, *opponent);
    } else {
        // Multipurpose or defense - just play it
        success = true;
        std::cout << player->getname() << " played: " << playedCard.get_name() << std::endl;
    }
    
    if (success) {
        action--;
        checkWinCondition();
    } else {
        // Return card to hand if failed
        player->gethand().push_back(playedCard);
    }
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
              << " at n" << player1->getx() << std::endl;
    std::cout << "Watson: HP " << watsonChar->gethealth() 
              << " at n" << watsonChar->getx() << std::endl;
    std::cout << "Dracula: HP " << player2->gethealth() 
              << " at n" << player2->getx() << std::endl;
    for (int i = 0; i < 3; i++) {
        std::cout << "Sister " << i+1 << ": HP " << sisters[i]->gethealth()
                  << " at n" << sisters[i]->getx() << std::endl;
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

void Game::saveGameToSlot(int slot) const {
    if (slot < 1 || slot > 3) return;
    std::string filename = "save_slot_" + std::to_string(slot) + ".txt";
    if (gameManager) {
        gameManager->saveGame(filename);
    }
}

void Game::loadGameFromSlot(int slot) {
    if (slot < 1 || slot > 3) return;
    std::string filename = "save_slot_" + std::to_string(slot) + ".txt";
    if (gameManager) {
        gameManager->loadGame(filename);
    }
}