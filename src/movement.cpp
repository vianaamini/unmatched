#include "movement.hpp"
#include <queue>
#include <unordered_set>
#include <functional>
#include <algorithm>

Movement::Movement(Board* board) : board(board) {}

bool Movement::isPositionOccupiedByEnemy(int x, int y,
                                         const std::vector<character*>& enemies) const {
    for (const auto& enemy : enemies) {
        if (!enemy) continue;
        auto pos = enemy->getposition();
        if (pos.first == x && pos.second == y) {
            return true;
        }
    }
    return false;
}

bool Movement::isPositionOccupiedByAlly(int x, int y,
                                        const std::vector<character*>& allies) const {
    for (const auto& ally : allies) {
        if (!ally) continue;
        auto pos = ally->getposition();
        if (pos.first == x && pos.second == y) {
            return true;
        }
    }
    return false;
}

bool Movement::canMoveThrough(int fromX, int fromY, int toX, int toY,
                              const std::vector<character*>& allies,
                              const std::vector<character*>& enemies) const {
    if (!board->isWalkable(toX, toY)) return false;
    
    if (isPositionOccupiedByEnemy(toX, toY, enemies)) return false;
    
    if (!(fromX == toX && fromY == toY)) {
        if (!board->isAdjacent(fromX, fromY, toX, toY)) {
            return false;
        }
    }
    
    return true;
}

int Movement::getBaseMovement(const character* character) const {return 2;}

std::vector<std::pair<int, int>> Movement::getPossibleMoves(
    character* character,
    int steps,
    const std::vector<character*>& allies,
    const std::vector<character*>& enemies) const {
    
    if (!character || steps <= 0) return {};
    
    auto start = character->getposition();
    int startX = start.first;
    int startY = start.second;
    
    if (!board->isValid(startX, startY)) return {};
    
    std::vector<std::pair<int, int>> validMoves;
    std::queue<std::pair<int, int>> queue;
    std::unordered_set<int> visited;
    
    queue.push({startX, startY});
    visited.insert(startY * board->getWidth() + startX);
    
    for (int step = 0; step < steps; ++step) {
        int levelSize = queue.size();
        for (int i = 0; i < levelSize; ++i) {
            auto current = queue.front();
            queue.pop();
            
            auto neighbors = board->getAdjacentSpaces(current.first, current.second);
            for (const auto& neighbor : neighbors) {
                int index = neighbor.second * board->getWidth() + neighbor.first;
                if (visited.find(index) != visited.end()) continue;
                
                if (canMoveThrough(current.first, current.second,
                                   neighbor.first, neighbor.second,
                                   allies, enemies)) {
                    
                    if (!isPositionOccupiedByAlly(neighbor.first, neighbor.second, allies)) {
                        validMoves.push_back(neighbor);
                    }
                    
                    queue.push(neighbor);
                    visited.insert(index);
                }
            }
        }
    }

    std::vector<std::pair<int ,int>> finalMoves;
    for (const auto& pos : validMoves) {
        if (board->isteleport(pos.first, pos.second)) {
            int dId = board->destination(pos.first, pos.second);
            int dx = dId / 1000;
            int dy = dId % 1000;

            if (board->isWalkable(dx ,dy) &&
                !isPositionOccupiedByAlly(dx,dy,allies) &&
                !isPositionOccupiedByEnemy(dx , dy ,enemies)) {
                    finalMoves.push_back({dx , dy});
                }
        }
        else {finalMoves.push_back(pos);}
    }
    return finalMoves;
}

bool Movement::canReach(int startX, int startY,
                        int targetX, int targetY,
                        int steps,
                        const std::vector<character*>& allies,
                        const std::vector<character*>& enemies) const {
    
    if (!board->isValid(startX, startY) || !board->isValid(targetX, targetY)) {
        return false;
    }
    
    std::queue<std::pair<int, int>> queue;
    std::unordered_set<int> visited;
    
    queue.push({startX, startY});
    visited.insert(startY * board->getWidth() + startX);
    
    for (int step = 0; step < steps; ++step) {
        int levelSize = queue.size();
        for (int i = 0; i < levelSize; ++i) {
            auto current = queue.front();
            queue.pop();
            
            if (current.first == targetX && current.second == targetY) {
                return true;}
            
            auto neighbors = board->getAdjacentSpaces(current.first, current.second);
            for (const auto& neighbor : neighbors) {
                int index = neighbor.second * board->getWidth() + neighbor.first;
                if (visited.find(index) != visited.end()) continue;
                
                if (canMoveThrough(current.first, current.second,
                                   neighbor.first, neighbor.second,
                                   allies, enemies)) {
                    queue.push(neighbor);
                    visited.insert(index);
                }
            }
        }
    }
    
    while (!queue.empty()) {
        auto pos = queue.front();
        queue.pop();
        if (pos.first == targetX && pos.second == targetY) {
            return true;
        }
    }
    
    return false;
}

std::vector<std::vector<std::pair<int, int>>> Movement::findPaths(
    int startX, int startY,
    int targetX, int targetY,
    int maxSteps,
    const std::vector<character*>& allies,
    const std::vector<character*>& enemies) const {
    
    std::vector<std::vector<std::pair<int, int>>> result;
    if (!board->isValid(startX, startY) || !board->isValid(targetX, targetY)) {
        return result;
    }
    
    std::function<void(int, int, int, std::vector<std::pair<int, int>>&)> dfs =
        [&](int x, int y, int steps, std::vector<std::pair<int, int>>& path) {
            if (x == targetX && y == targetY) {
                result.push_back(path);
                return;
            }
            
            if (steps >= maxSteps) return;
            
            auto neighbors = board->getAdjacentSpaces(x, y);
            for (const auto& neighbor : neighbors) {
                if (std::find(path.begin(), path.end(), neighbor) != path.end()) continue;
                
                if (canMoveThrough(x, y, neighbor.first, neighbor.second, allies, enemies)) {
                    path.push_back(neighbor);
                    dfs(neighbor.first, neighbor.second, steps + 1, path);
                    path.pop_back();
                }
            }
        };
    
    std::vector<std::pair<int, int>> path = {{startX, startY}};
    dfs(startX, startY, 0, path);
    
    return result;
}

void  Movement::boost(character* character, card*playedCard , ActionType currentAction)const
{
    if (!character || !playCard ||currentAction != ActionType::MANEUVER ){return;}
    if (currentAction == ActionType::MANEUVER)
    {
        character->setnewmovement(character->getmovement() + playCard->getboost());
    }
    // This boost is just for one round, and we need to call resetmovement() in our game controller right after using it.
}

/*std::vector<std::pair<int ,int>> finalMoves;
for (const auto& pos : validMoves) {
    if (board->isteleport(pos.first, pos.second)) {
        int dId = board->destination(pos.first, pos.second);
        int dx = dID / 1000;
        int dy = dId % 1000;

        if (board->isWalkable(dx ,dy) &&
            !isPositionOccupiedByAlly(dx,dy,allies) &&
            !isPositionOccupiedByEnemy(dx , dy ,enemies)) {
                finalMoves.push_back({dx , dy});
            }
    }
    else {finalMoves.push_back(pos);}
}
return finalMoves;
}*/

