#include "../include/game_manager.hpp"
#include <algorithm>
#include <iostream>

GameManager::GameManager() : board(), movement(&board) {}

void GameManager::addCharacter(character* character, int team) {
    if (!character) return;
    allCharacters.push_back(character);
    turnManager.addCharacter(character, team);
    
    if (team == 1) {
        team1.push_back(character);
    } else {
        team2.push_back(character);
    }
}

void GameManager::removeCharacter(character* character) {
    if (!character) return;
    
    auto it = std::find(allCharacters.begin(), allCharacters.end(), character);
    if (it != allCharacters.end()) {
        allCharacters.erase(it);
    }
    
    auto it1 = std::find(team1.begin(), team1.end(), character);
    if (it1 != team1.end()) {
        team1.erase(it1);
    }
    
    auto it2 = std::find(team2.begin(), team2.end(), character);
    if (it2 != team2.end()) {
        team2.erase(it2);
    }
    
    turnManager.removeCharacter(character);
}

std::vector<character*> GameManager::getAllies(character* character) const {
    std::vector<character*> allies;
    if (!character) return allies;
    
    int team = -1;
    if (std::find(team1.begin(), team1.end(), character) != team1.end()) {
        team = 1;
    } else if (std::find(team2.begin(), team2.end(), character) != team2.end()) {
        team = 2;
    }
    
    if (team == -1) return allies;
    
    const auto& teammates = (team == 1) ? team1 : team2;
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
    if (std::find(team1.begin(), team1.end(), character) != team1.end()) {
        team = 1;
    } else if (std::find(team2.begin(), team2.end(), character) != team2.end()) {
        team = 2;
    }
    
    if (team == -1) return enemies;
    
    const auto& enemyTeam = (team == 1) ? team2 : team1;
    for (const auto& c : enemyTeam) {
        if (c->isalive()) {
            enemies.push_back(c);
        }
    }
    
    return enemies;
}

std::vector<std::string> GameManager::getValidMoves(character* character) {
    if (!character) return {};
  
    auto pos = character->getposition();
    std::string currentSpace = "n" + std::to_string(pos.first);
    
    auto allies = getAllies(character);
    auto enemies = getEnemies(character);
    
    std::vector<std::string> allySpaces, enemySpaces;
    for (auto c : allies) {
        auto p = c->getposition();
        allySpaces.push_back("n" + std::to_string(p.first));
    }
    for (auto c : enemies) {
        auto p = c->getposition();
        enemySpaces.push_back("n" + std::to_string(p.first));
    }
    
    int baseMovement = character->getmovement();
    return movement.getPossibleMoves(currentSpace, baseMovement, allySpaces, enemySpaces);
}

bool GameManager::moveCharacter(character* character, const std::string& targetSpace) {
    if (!character) return false;
    
    auto moves = getValidMoves(character);
    if (std::find(moves.begin(), moves.end(), targetSpace) != moves.end()) {
        auto coords = board.getCoordinates(targetSpace);
        character->setposition(coords.first, coords.second);
        return true;
    }
    return false;
}

std::vector<std::string> GameManager::getValidMovesWithBoost(
    character* character, int boostValue) {
    
    if (!character) return {};
    
    auto pos = character->getposition();
    std::string currentSpace = "n" + std::to_string(pos.first);
    
    auto allies = getAllies(character);
    auto enemies = getEnemies(character);
    
    std::vector<std::string> allySpaces, enemySpaces;
    for (auto c : allies) {
        auto p = c->getposition();
        allySpaces.push_back("n" + std::to_string(p.first));
    }
    for (auto c : enemies) {
        auto p = c->getposition();
        enemySpaces.push_back("n" + std::to_string(p.first));
    }int baseMovement = character->getmovement();
    return movement.getPossibleMovesWithBoost(currentSpace, baseMovement, boostValue, allySpaces, enemySpaces);
}

bool GameManager::moveCharacterWithBoost(character* character, const std::string& targetSpace, int boostValue) {
    if (!character) return false;
    
    auto moves = getValidMovesWithBoost(character, boostValue);
    if (std::find(moves.begin(), moves.end(), targetSpace) != moves.end()) {
        auto coords = board.getCoordinates(targetSpace);
        character->setposition(coords.first, coords.second);
        return true;
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