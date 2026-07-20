#include "../include/game_manager.hpp"
#include "../include/hero.hpp"
#include <algorithm>
#include <iostream>

GameManager::GameManager() : board(), movement(&board) {}

void GameManager::addCharacter(character* c, int team) {
    if (!c) return;
    allCharacters.push_back(c);
    turnManager.addCharacter(c, team);
    
    if (team == 1) {
        team1.push_back(c);
    } else {
        team2.push_back(c);
    }
}

void GameManager::removeCharacter(character* c) {
    if (!c) return;
    
    auto it = std::find(allCharacters.begin(), allCharacters.end(), c);
    if (it != allCharacters.end()) {
        allCharacters.erase(it);
    }
    
    auto it1 = std::find(team1.begin(), team1.end(), c);
    if (it1 != team1.end()) {
        team1.erase(it1);
    }
    
    auto it2 = std::find(team2.begin(), team2.end(), c);
    if (it2 != team2.end()) {
        team2.erase(it2);
    }
    
    turnManager.removeCharacter(c);
}

std::vector<character*> GameManager::getAllies(character* c) const {
    std::vector<character*> allies;
    if (!c) return allies;
    
    int team = -1;
    if (std::find(team1.begin(), team1.end(), c) != team1.end()) {
        team = 1;
    } else if (std::find(team2.begin(), team2.end(), c) != team2.end()) {
        team = 2;
    }
    
    if (team == -1) return allies;
    
    const auto& teammates = (team == 1) ? team1 : team2;
    for (const auto& cc : teammates) {
        if (cc != c && cc->isalive()) {
            allies.push_back(cc);
        }
    }
    
    return allies;
}

std::vector<character*> GameManager::getEnemies(character* c) const {
    std::vector<character*> enemies;
    if (!c) return enemies;
    
    int team = -1;
    if (std::find(team1.begin(), team1.end(), c) != team1.end()) {
        team = 1;
    } else if (std::find(team2.begin(), team2.end(), c) != team2.end()) {
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

std::vector<std::string> GameManager::getValidMoves(character* c) {
    if (!c) return {};
  
    auto allies = getAllies(c);
    auto enemies = getEnemies(c);
    
    return movement.getPossibleMoves(c, c->getmovement(), allies, enemies);
}

bool GameManager::moveCharacter(character* c, const std::string& targetSpace) {
    if (!c) return false;
    
    if (turnManager.getCurrentCharacter() != c) {
        std::cout << "Not your turn!" << std::endl;
        return false;
    }
    
    hero* h = dynamic_cast<hero*>(c);
    if (!h || !h->canact()) {
        std::cout << "No actions remaining!" << std::endl;
        return false;
    }
    
    int targetNode = board.getNodeId(targetSpace);
    if (!board.hasSpace(targetSpace)) {
        std::cout << "Invalid node!" << std::endl;
        return false;
    }
    
    for (character* other : allCharacters) {
        if (other != c && other->isalive() && other->getx() == targetNode) {
            std::cout << "Node " << targetSpace << " is occupied!" << std::endl;
            return false;
        }
    }
    
    int startNode = c->getx();
    string startName = "n" + to_string(startNode);
    
    bool isConnected = board.isConnected(startName, targetSpace);
    bool isTeleport = board.isTeleport(startName) && 
                      board.getTeleportDestination(startName) == targetSpace;
    
    if (!isConnected && !isTeleport) {
        std::cout << "Cannot move to " << targetSpace << " (not adjacent)" << std::endl;
        return false;
    }
    
    h->setposition(targetNode);
    h->drawcard();
    h->useAction();
    turnManager.endTurn();
    return true;
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