#include "../include/game_manager.hpp"
#include <algorithm>
#include <iostream>

GameManager::GameManager() : board(8, 8), movement(&board) {}

void GameManager::addCharacter(character* character, int team) {
    if (!character) return;
    allCharacters.push_back(character);
    turnManager.addCharacter(character, team);
}

void GameManager::removeCharacter(character* character) {
    if (!character) return;
    
    auto it = std::find(allCharacters.begin(), allCharacters.end(), character);
    if (it != allCharacters.end()) {
        allCharacters.erase(it);
    }
    
    turnManager.removeCharacter(character);
}

std::vector<character*> GameManager::getAllies(character* character) const {
    std::vector<character*> allies;
    if (!character) return allies;
    
    int team = -1;
    auto team1 = turnManager.getTeamCharacters(1);
    auto team2 = turnManager.getTeamCharacters(2);
    
    if (std::find(team1.begin(), team1.end(), character) != team1.end()) {
        team = 1;
    } else if (std::find(team2.begin(), team2.end(), character) != team2.end()) {
        team = 2;
    }
    
    if (team == -1) return allies;
    
    auto teammates = turnManager.getTeamCharacters(team);
    for (const auto& c : teammates) {
        if (c != character && c->isalive()) {
            allies.push_back(c);
        }
    }
    
    return allies;
}

std::vector<character*> GameManager::getEnemies(character* character) const {
    std::vector<character*> enemies;
    if (!character) return enemies;
    
    int team = -1;
    auto team1 = turnManager.getTeamCharacters(1);
    auto team2 = turnManager.getTeamCharacters(2);
    
    if (std::find(team1.begin(), team1.end(), character) != team1.end()) {
        team = 1;
    } else if (std::find(team2.begin(), team2.end(), character) != team2.end()) {
        team = 2;
    }
    
    if (team == -1) return enemies;
    
    int enemyTeam = (team == 1) ? 2 : 1;
    auto enemiesList = turnManager.getTeamCharacters(enemyTeam);
    for (const auto& c : enemiesList) {
        if (c->isalive()) {
            enemies.push_back(c);
        }
    }
    
    return enemies;
}

std::vector<std::pair<int, int>> GameManager::getValidMoves(character* character) {
    if (!character) return {};
    
    auto allies = getAllies(character);
    auto enemies = getEnemies(character);
    
    int baseMovement = 2;
    return movement.getPossibleMoves(character, baseMovement, allies, enemies);
}

bool GameManager::moveCharacter(character* character, int targetX, int targetY) {
    if (!character) return false;
    
    auto moves = getValidMoves(character);
    for (const auto& pos : moves) {
        if (pos.first == targetX && pos.second == targetY) {
            character->setposition(targetX, targetY);
            return true;
        }
    }
    return false;
}

void GameManager::startGame() {
    turnManager.startGame();
}

void GameManager::nextTurn() {
    turnManager.nextTurn();
}

void GameManager::nextPhase() {
    turnManager.nextPhase();
}

character* GameManager::getCurrentCharacter() const {
    return turnManager.getCurrentCharacter();
}

TurnPhase GameManager::getCurrentPhase() const {
    return turnManager.getCurrentPhase();
}

int GameManager::getTurnNumber() const {
    return turnManager.getTurnNumber();
}

int GameManager::getCurrentTeam() const {
    return turnManager.getCurrentTeam();
}

int GameManager::getActionsRemaining() const {
    return turnManager.getActionsRemaining();
}

bool GameManager::isGameOver() const {
    return turnManager.isGameOver();
}

character* GameManager::getWinner() const {
    return turnManager.getWinner();
}

std::vector<character*> GameManager::getAllCharacters() const {
    return allCharacters;
}

Board& GameManager::getBoard() {
    return board;
}