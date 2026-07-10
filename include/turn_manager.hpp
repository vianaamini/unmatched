#ifndef TURN_MANAGER_HPP
#define TURN_MANAGER_HPP

#include "character.hpp"
#include <vector>
#include <queue>

enum class TurnPhase {
    DRAW,
    MANEUVER,
    SCHEME,
    ATTACK,
    END
};

enum class ActionType {
    MANEUVER,
    SCHEME,
    ATTACK,
    NONE
};

class TurnManager {
private:
    std::queue<character*> turnOrder;
    character* currentCharacter;
    TurnPhase currentPhase;
    ActionType currentAction;
    int turnNumber;
    int actionsRemaining;
    
    std::vector<character*> team1;
    std::vector<character*> team2;
    int currentTeam;
    
public:
    TurnManager();
    
    void addCharacter(character* character, int team = 1);
    void removeCharacter(character* character);
    
    void startGame();
    void nextTurn();
    void nextPhase();
    void endTurn();
    
    character* getCurrentCharacter() const;
    TurnPhase getCurrentPhase() const;
    ActionType getCurrentAction() const;
    int getTurnNumber() const;
    int getCurrentTeam() const;
    int getActionsRemaining() const;
    
    bool canPerformManeuver() const;
    bool canPerformScheme() const;
    bool canPerformAttack() const;
    
    bool isCharacterTurn(character* character) const;
    bool isTeamTurn(int team) const;
    
    std::vector<character*> getAliveCharacters() const;
    std::vector<character*> getTeamCharacters(int team) const;
    
    bool isGameOver() const;
    character* getWinner() const;
    
private:
    void updateTurnOrder();
    bool isCharacterAlive(character* character) const;
};

#endif