#include "../include/game.hpp"
#include "../include/deracula_cards.hpp"
#include "../include/sherlock_card.hpp"
#include "../include/hero.hpp"  // اضافه کردن این خط
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
    
    card defendCard("Empty", cardtype::defense, 0, 0, 0, cardowner::any, "No effect");
    if (opponent->handsize() > 0) {
        defendCard = opponent->gethand()[0];
        opponent->gethand().erase(opponent->gethand().begin());
    }
    
    resolve(*player, playedCard, *opponent, defendCard);
    
    action--;
    checkWinCondition();
}

void Game::resolve(hero& attacker, card& atkcard, hero& defender, card& defendCard) {
    std::cout << "\n=== Combat ===" << std::endl;
    std::cout << attacker.getname() << " plays: " << atkcard.get_name() << std::endl;
    std::cout << defender.getname() << " plays: " << defendCard.get_name() << std::endl;
    
    int attackValue = atkcard.getattack();
    int defenseValue = defendCard.getdefense();
    
    // استفاده از Board از GameManager
    Board* board = gameManager ? &gameManager->getBoard() : nullptr;
    if (!board) {
        // اگر GameManager نداشتیم، یک Board موقت
        static Board tempBoard;
        board = &tempBoard;
    }
    
    bool isAdjacent = board->isAdjacent(attacker.getx(), defender.getx());
    
    // کارت‌های شرلوک
    if (atkcard.get_name() == "Counter Punch") {
        card_counter_punch cp;
        cp.execute_effect(attacker, defender, defendCard, isAdjacent, false);
    }
    else if (atkcard.get_name() == "Feint") {
        card_feint feint;
        feint.execute_effect(attacker, defender, defendCard, isAdjacent, false);
    }
    else if (atkcard.get_name() == "Fixed Point") {
        card_fixed_point fp;
        fp.execute_effect(attacker, defender, defendCard, isAdjacent, false);
    }
    else if (atkcard.get_name() == "Master of Disguise") {
        card_master_of_disguise md;
        md.execute_effect(attacker, defender, defendCard, isAdjacent, false);
    }
    else if (atkcard.get_name() == "The Game is Afoot") {
        card_game_is_afoot ga;
        ga.execute_effect(attacker, defender, defendCard, isAdjacent, false);
    }
    
    std::vector<sidekick*> allSisters;
    for (int i = 0; i < 3; i++) {
        allSisters.push_back(sisters[i]);
    }
    
    // کارت‌های دراکولا
    if (atkcard.get_name() == "Feeding Frenzy" || 
        atkcard.get_name() == "Ambush" ||
        atkcard.get_name() == "Beastform" ||
        atkcard.get_name() == "Dash" ||
        atkcard.get_name() == "Thirst for Sustenance" ||
        atkcard.get_name() == "Exploit") {
        dcards::resolve_combat_effects(atkcard, attacker, defendCard, defender, allSisters, *board);
    }
    
    int damage = attackValue - defenseValue;
    if (damage > 0) {
        defender.takedamage(damage);
        std::cout << defender.getname() << " took " << damage << " damage" << std::endl;
    } else {
        std::cout << defender.getname() << " defended successfully" << std::endl;
    }
    
    if (atkcard.gettype() == cardtype::scheme) {
        std::vector<character*> enemyList;
        enemyList.push_back(&defender);
        std::vector<sidekick*> allSisters2;
        for (int i = 0; i < 3; i++) {
            allSisters2.push_back(sisters[i]);
        }
        dcards::resolve_scheme(atkcard, attacker, defender, enemyList, allSisters2, *board);
    }
    
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