#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include "map.hpp"
#include "movement.hpp"
#include "turn_manager.hpp"
#include "character.hpp"
#include "card.hpp"
#include <vector>
#include <string>

struct SaveSlotInfo {
    int slotNumber;
    std::string filename;
    bool exists;
    std::string details;
};

class GameManager {
private:
    Board board;
    TurnManager turnManager;
    Movement movement;
    std::vector<character*> allCharacters;
    std::vector<character*> team1;
    std::vector<character*> team2;

public:
    GameManager();

    void addCharacter(character* c, int team);
    void removeCharacter(character* c);

    std::vector<character*> getAllies(character* c) const;
    std::vector<character*> getEnemies(character* c) const;
    std::vector<character*> getAllCharacters() const;

    std::vector<std::string> getValidMoves(character* c);
    bool moveCharacter(character* c, const std::string& targetSpace, const card* boostCard = nullptr);

    bool resurrectSister(const std::string& sisterName, int heroNode);
    character* findCharacterByName(const std::string& name);
    bool handleMove(const std::string& charIdentifier, const std::string& targetNodeStr);
    bool handleMove(character* actor, const std::string& targetNodeStr);

    void startGame(int firstTeam);
    void nextTurn();
    void nextPhase();

    void forceEndTurn();

    character* getCurrentCharacter() const;
    TurnPhase getCurrentPhase() const;
    int getTurnNumber() const;
    int getCurrentTeam() const;
    int getActionsRemaining() const;
    bool isGameOver() const;
    character* getWinner() const;

    Board& getBoard() { return board; }
    const Board& getBoard() const { return board; }
    TurnManager& getTurnManager() { return turnManager; }
    const TurnManager& getTurnManager() const { return turnManager; }

    bool saveGame(const std::string& filename) const;
    bool peekHeroTypes(const std::string& filename, std::string& team1Type, std::string& team2Type) const;
    bool loadGame(const std::string& filename);
    std::vector<SaveSlotInfo> getRecentSaveSlots() const;
};

#endif