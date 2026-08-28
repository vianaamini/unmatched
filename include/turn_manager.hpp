#ifndef TURN_MANAGER_HPP
#define TURN_MANAGER_HPP

#include <vector>
#include <queue>
#include "character.hpp"

enum class TurnPhase {
    DRAW,
    MANEUVER,
    SCHEME,
    ATTACK,
    END
};

enum class ActionType {
    NONE,
    MANEUVER,
    SCHEME,
    ATTACK
};

class TurnManager {
private:
    std::vector<character*> team1;
    std::vector<character*> team2;
    std::queue<character*> turnOrder;
    
    character* currentCharacter;
    TurnPhase currentPhase;
    ActionType currentAction;
    int turnNumber;
    int actionsRemaining;
    int currentTeam;

    void updateTurnOrder();
    bool isCharacterAlive(character* character) const;

public:
    TurnManager();

    void addCharacter(character* character, int team);
    void removeCharacter(character* character);

    void startGame(int firstTeam);
    void nextTurn();
    void nextPhase();
    void endTurn();

    void restoreState(int turnNum, int team, TurnPhase phase, int actionsRem, character* currentChar);

    // اضافه شده برای Vanish
    void setActionsRemaining(int actions);

    bool canPerformManeuver() const;
    bool canPerformScheme() const;
    bool canPerformAttack() const;

    character* getCurrentCharacter() const;
    TurnPhase getCurrentPhase() const;
    ActionType getCurrentAction() const;
    int getTurnNumber() const;
    int getCurrentTeam() const;
    int getActionsRemaining() const;

    bool isCharacterTurn(character* character) const;
    bool isTeamTurn(int team) const;

    std::vector<character*> getAliveCharacters() const;
    std::vector<character*> getTeamCharacters(int team) const;

    bool isGameOver() const;
    character* getWinner() const;
};

#endif