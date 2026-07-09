#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include "board.hpp"
#include "movement.hpp"
#include "turn_manager.hpp"
#include "deployment.hpp"
#include "character.hpp"
#include <vector>

class GameManager {
private:
    Board board;
    Movement movement;
    TurnManager turnManager;
    std::vector<character*> allCharacters;
    
public:
    GameManager();
    
    void addCharacter(character* character, int team = 1);
    void removeCharacter(character* character);
    
    std::vector<std::pair<int, int>> getValidMoves(character* character);
    
    bool moveCharacter(character* character, const std::string& targetSpace);
    
    void startGame();
    void nextTurn();
    void nextPhase();
    
    character* getCurrentCharacter() const;
    TurnPhase getCurrentPhase() const;
    int getTurnNumber() const;
    int getCurrentTeam() const;
    int getActionsRemaining() const;
    
    bool isGameOver() const;
    character* getWinner() const;
    
    std::vector<character*> getAllCharacters() const;
    Board& getBoard() { return board; }
    
private:
    std::vector<character*> getAllies(character* character) const;
    std::vector<character*> getEnemies(character* character) const;
};

#endif