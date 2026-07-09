#pragma once

#include "map.hpp"
#include "character.hpp"
#include "turn_manager.hpp"
#include <vector>
#include <utility>

class Movement {
private:
    Board* board;
    
    bool canMoveThrough(int fromX, int fromY, int toX, int toY,
                        const std::vector<character*>& allies,
                        const std::vector<character*>& enemies) const;
    
    bool isPositionOccupiedByEnemy(int x, int y, 
                                   const std::vector<character*>& enemies) const;
    
    bool isPositionOccupiedByAlly(int x, int y,
                                  const std::vector<character*>& allies) const;

public:
    Movement(Board* board);
    
    
    std::vector<std::pair<int, int>> getPossibleMoves(
        character* character,
        int steps,
        const std::vector<character*>& allies,
        const std::vector<character*>& enemies) const;
    
    std::vector<std::vector<std::pair<int, int>>> findPaths(
        int startX, int startY,
        int targetX, int targetY,
        int maxSteps,
        const std::vector<character*>& allies,
        const std::vector<character*>& enemies) const;
    
    bool canReach(
        int startX, int startY,
        int targetX, int targetY,
        int steps,
        const std::vector<character*>& allies,
        const std::vector<character*>& enemies) const;
    
    int getBaseMovement(const character* character) const;
    void boost(character* character, card* playedCard , ActionType currentAction) const;
};

