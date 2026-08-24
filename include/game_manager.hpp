#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include <vector>
#include <string>
#include "map.hpp"
#include "movement.hpp"
#include "turn_manager.hpp"
#include "deployment.hpp"
#include "character.hpp"
#include "invisible_man.hpp"

class GameManager
{
private:
    Board board;
    Movement movement;
    TurnManager turnManager;
    std::vector<character *> allCharacters;
    std::vector<character *> team1;
    std::vector<character *> team2;

public:
    GameManager();

    void addCharacter(character *character, int team = 1);
    void removeCharacter(character *character);

    character *findCharacterByName(const std::string &name);

    std::vector<std::string> getValidMoves(character *character);
    bool moveCharacter(character *character, const std::string &targetSpace, const card *boostCard = nullptr);

    bool handleMove(const std::string &charIdentifier, const std::string &targetNodeStr);
    bool handleMove(character *actor, const std::string &targetNodeStr);

    void startGame(int firstTeam);
    void nextTurn();
    void nextPhase();

    character *getCurrentCharacter() const;
    TurnPhase getCurrentPhase() const;
    int getTurnNumber() const;
    int getCurrentTeam() const;
    int getActionsRemaining() const;

    bool isGameOver() const;
    character *getWinner() const;

    std::vector<character *> getAllCharacters() const;
    Board &getBoard() { return board; }
    const Board &getBoard() const { return board; }
    TurnManager &getTurnManager() { return turnManager; }

    std::vector<character *> getAllies(character *c) const;
    std::vector<character *> getEnemies(character *c) const;

    bool resurrectSister(const std::string &sisterName, int heroNode);
    bool saveGame(const std::string &filename) const;
    bool loadGame(const std::string &filename);
    bool peekHeroTypes(const std::string &filename, std::string &team1Type, std::string &team2Type) const;
};

#endif