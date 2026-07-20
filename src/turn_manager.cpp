#include "../include/turn_manager.hpp"
#include "../include/hero.hpp"
#include <algorithm>
#include <iostream>

TurnManager::TurnManager() 
    : currentCharacter(nullptr),
      currentPhase(TurnPhase::DRAW),
      currentAction(ActionType::NONE),
      turnNumber(0),
      actionsRemaining(2),
      currentTeam(1) {}

void TurnManager::addCharacter(character* character, int team) {
    if (!character) return;
    
    if (team == 1) {
        team1.push_back(character);
    } else {
        team2.push_back(character);
    }
    
    updateTurnOrder();
}

void TurnManager::removeCharacter(character* character) {
    if (!character) return;
    
    auto it1 = std::find(team1.begin(), team1.end(), character);
    if (it1 != team1.end()) {
        team1.erase(it1);
    }
    
    auto it2 = std::find(team2.begin(), team2.end(), character);
    if (it2 != team2.end()) {
        team2.erase(it2);
    }
    
    updateTurnOrder();
}

void TurnManager::updateTurnOrder() {
    while (!turnOrder.empty()) {
        turnOrder.pop();
    }
    
    for (const auto& c : team1) {
        if (isCharacterAlive(c)) {
            turnOrder.push(c);
        }
    }
    
    for (const auto& c : team2) {
        if (isCharacterAlive(c)) {
            turnOrder.push(c);
        }
    }
}

bool TurnManager::isCharacterAlive(character* character) const {
    return character && character->isalive();
}

void TurnManager::startGame() {
    turnNumber = 0;
    currentTeam = 1;
    currentPhase = TurnPhase::DRAW;
    actionsRemaining = 2;
    updateTurnOrder();
    nextTurn();
}

void TurnManager::nextTurn() {
    if (turnOrder.empty()) {
        updateTurnOrder();
    }
    
    if (turnOrder.empty()) {
        return;
    }
    
    currentCharacter = turnOrder.front();
    turnOrder.pop();
    
    if (!isCharacterAlive(currentCharacter)) {
        nextTurn();
        return;
    }
    
    turnOrder.push(currentCharacter);
    
    turnNumber++;
    currentPhase = TurnPhase::DRAW;
    actionsRemaining = 2;
    currentAction = ActionType::NONE;
    
    if (std::find(team1.begin(), team1.end(), currentCharacter) != team1.end()) {
        currentTeam = 1;
    } else {
        currentTeam = 2;
    }
    
    std::cout << "\n=== Turn " << turnNumber << " ===" << std::endl;
    std::cout << currentCharacter->getname() << " (Team " << currentTeam << ")" << std::endl;
    std::cout << "Actions remaining: " << actionsRemaining << std::endl;
}

void TurnManager::nextPhase() {
    switch (currentPhase) {
        case TurnPhase::DRAW:
            currentPhase = TurnPhase::MANEUVER;
            currentAction = ActionType::MANEUVER;
            std::cout << "Phase: MANEUVER (Draw 1 card + optional move)" << std::endl;
            break;
        case TurnPhase::MANEUVER:
            currentPhase = TurnPhase::SCHEME;
            currentAction = ActionType::SCHEME;
            std::cout << "Phase: SCHEME (Play a scheme card)" << std::endl;
            break;
        case TurnPhase::SCHEME:
            currentPhase = TurnPhase::ATTACK;
            currentAction = ActionType::ATTACK;
            std::cout << "Phase: ATTACK (Choose attacker and target)" << std::endl;
            break;
        case TurnPhase::ATTACK:
            currentPhase = TurnPhase::END;
            std::cout << "Phase: END" << std::endl;
            break;
        case TurnPhase::END:
            currentPhase = TurnPhase::DRAW;
            endTurn();
            break;
    }
}

void TurnManager::endTurn() {
    actionsRemaining--;
    
    if (actionsRemaining > 0) {
        std::cout << "Actions remaining: " << actionsRemaining << std::endl;
        currentPhase = TurnPhase::MANEUVER;
        currentAction = ActionType::MANEUVER;
    } else {
        currentAction = ActionType::NONE;
        std::cout << "Turn ended. Discard down to 7 cards." << std::endl;
        nextTurn();
    }
}

bool TurnManager::canPerformManeuver() const {
    return currentPhase == TurnPhase::MANEUVER && 
           actionsRemaining > 0 &&
           currentCharacter &&
           isCharacterAlive(currentCharacter);
}

bool TurnManager::canPerformScheme() const {
    return currentPhase == TurnPhase::SCHEME && 
           actionsRemaining > 0 &&
           currentCharacter &&
           isCharacterAlive(currentCharacter);
}

bool TurnManager::canPerformAttack() const {
    return currentPhase == TurnPhase::ATTACK && 
           actionsRemaining > 0 &&
           currentCharacter &&
           isCharacterAlive(currentCharacter);
}

character* TurnManager::getCurrentCharacter() const {
    return currentCharacter;
}

TurnPhase TurnManager::getCurrentPhase() const {
    return currentPhase;
}

ActionType TurnManager::getCurrentAction() const {
    return currentAction;
}

int TurnManager::getTurnNumber() const {
    return turnNumber;
}

int TurnManager::getCurrentTeam() const {
    return currentTeam;
}

int TurnManager::getActionsRemaining() const {
    return actionsRemaining;
}

bool TurnManager::isCharacterTurn(character* character) const {
    return currentCharacter == character;
}

bool TurnManager::isTeamTurn(int team) const {
    return currentTeam == team;
}

std::vector<character*> TurnManager::getAliveCharacters() const {
    std::vector<character*> alive;
    
    for (const auto& c : team1) {
        if (isCharacterAlive(c)) {
            alive.push_back(c);
        }
    }
    
    for (const auto& c : team2) {
        if (isCharacterAlive(c)) {
            alive.push_back(c);
        }
    }
    
    return alive;
}

std::vector<character*> TurnManager::getTeamCharacters(int team) const {
    if (team == 1) {
        return team1;
    } else {
        return team2;
    }
}

bool TurnManager::isGameOver() const {
    bool team1HeroDead = true;
    bool team2HeroDead = true;

    for (const auto& c : team1) {
        hero* h = dynamic_cast<hero*>(c);
        if (h) team1HeroDead = !isCharacterAlive(h);
    }
    for (const auto& c : team2) {
        hero* h = dynamic_cast<hero*>(c);
        if (h) team2HeroDead = !isCharacterAlive(h);
    }

    return team1HeroDead || team2HeroDead;
}

character* TurnManager::getWinner() const {
    if (!isGameOver()) return nullptr;

    hero* team1Hero = nullptr;
    hero* team2Hero = nullptr;

    for (const auto& c : team1) {
        hero* h = dynamic_cast<hero*>(c);
        if (h) team1Hero = h;
    }
    for (const auto& c : team2) {
        hero* h = dynamic_cast<hero*>(c);
        if (h) team2Hero = h;
    }

    bool team1HeroDead = team1Hero ? !isCharacterAlive(team1Hero) : true;
    bool team2HeroDead = team2Hero ? !isCharacterAlive(team2Hero) : true;

    if (team1HeroDead && !team2HeroDead) return team2Hero;
    if (team2HeroDead && !team1HeroDead) return team1Hero;
    return nullptr;
}