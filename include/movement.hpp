#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

#include <vector>
#include <string>
#include <utility>

#include "character.hpp"   
#include "map.hpp"
#include "turn_manager.hpp"
#include "card.hpp"

class Movement {
private:
    const Board* board;
    
    bool canMoveThrough(const std::string& from, const std::string& to,
                        const std::vector<character*>& alliesList,
                        const std::vector<character*>& enemiesList) const;
    
    bool isPositionOccupiedByEnemy(const std::string& space,
                                   const std::vector<character*>& enemiesList) const;
    
    bool isPositionOccupiedByAlly(const std::string& space,
                                  const std::vector<character*>& alliesList) const;

public:
    Movement(const Board* board);
    
    std::vector<std::string> getPossibleMoves(
        character* c,
        int steps,
        const std::vector<character*>& alliesList,
        const std::vector<character*>& enemiesList
    ) const;
    
    std::vector<std::vector<std::string>> findPaths(
        const std::string& start,
        const std::string& target,
        int maxSteps,
        const std::vector<character*>& alliesList,
        const std::vector<character*>& enemiesList
    ) const;
    
    bool canReach(
        const std::string& start,
        const std::string& target,
        int steps,
        const std::vector<character*>& alliesList,
        const std::vector<character*>& enemiesList
    ) const;
    
    int getBaseMovement(const character* c) const;
    void boost(character* c, const card* playedCard, ActionType currentAction) const;
};

#endif