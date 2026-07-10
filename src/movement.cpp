#include "../include/movement.hpp"
#include <queue>
#include <unordered_set>
#include <functional>
#include <algorithm>

Movement::Movement(const Board* board) : board(board) {}

bool Movement::isPositionOccupiedByEnemy(const std::string& space,
                                         const std::vector<character*>& enemiesList) const {
    for (const auto& enemy : enemiesList) {
        if (!enemy) continue;
        if (enemy->getPosition() == space) {
            return true;
        }
    }
    return false;
}

bool Movement::isPositionOccupiedByAlly(const std::string& space,
                                        const std::vector<character*>& alliesList) const {
    for (const auto& ally : alliesList) {
        if (!ally) continue;
        if (ally->getPosition() == space) {
            return true;
        }
    }
    return false;
}

bool Movement::canMoveThrough(const std::string& from, const std::string& to,
                              const std::vector<character*>& alliesList,
                              const std::vector<character*>& enemiesList) const {
    if (!board->hasSpace(to)) return false;
    
    if (isPositionOccupiedByEnemy(to, enemiesList)) return false;
    
    if (from != to) {
        if (!board->isConnected(from, to) && 
            !(board->isTeleport(from) && board->getTeleportDestination(from) == to)) {
            return false;
        }
    }
    
    return true;
}

int Movement::getBaseMovement(const character* character) const {
    if (!character) return 0;
    return character->getmovement();  
}

std::vector<std::string> Movement::getPossibleMoves(
    character* character,
    int steps,
    const std::vector<character*>& alliesList,
    const std::vector<character*>& enemiesList) const {
    
    if (!character || steps <= 0) return {};
    
    std::string start = character->getPosition();
    if (!board->hasSpace(start)) return {};
    
    std::vector<std::string> validMoves;
    std::queue<std::string> queue;
    std::unordered_set<std::string> visited;
    
    queue.push(start);
    visited.insert(start);
    
    for (int step = 0; step < steps; ++step) {
        int levelSize = queue.size();
        for (int i = 0; i < levelSize; ++i) {
            std::string current = queue.front();
            queue.pop();
            
            auto neighbors = board->getNeighbors(current);
            
            if (board->isTeleport(current)) {
                std::string dest = board->getTeleportDestination(current);
                if (!dest.empty() && dest != current) {
                    neighbors.push_back(dest);
                }
            }
            
            for (const auto& neighbor : neighbors) {
                if (visited.find(neighbor) != visited.end()) continue;
                
                if (canMoveThrough(current, neighbor, alliesList, enemiesList)) {
                    if (!isPositionOccupiedByAlly(neighbor, alliesList)) {
                        validMoves.push_back(neighbor);
                    }
                    queue.push(neighbor);
                    visited.insert(neighbor);
                }
            }
        }
    }

    std::vector<std::pair<int, int>> finalMoves;
    for (const auto& pos : validMoves) {
        if (board->isTeleport(pos.first, pos.second)) {
            int destId = board->getTeleportDestination(pos.first, pos.second);
            int dx = destId / 1000;
            int dy = destId % 1000;
            finalMoves.push_back({dx, dy});
        } else {
            finalMoves.push_back(pos);
        }
    }
    return finalMoves;
}

bool Movement::canReach(const std::string& start,
                        const std::string& target,
                        int steps,
                        const std::vector<character*>& alliesList,
                        const std::vector<character*>& enemiesList) const {
    
    if (!board->hasSpace(start) || !board->hasSpace(target)) {
        return false;
    }
    
    std::queue<std::string> queue;
    std::unordered_set<std::string> visited;
    
    queue.push(start);
    visited.insert(start);
    
    for (int step = 0; step < steps; ++step) {
        int levelSize = queue.size();
        for (int i = 0; i < levelSize; ++i) {
            std::string current = queue.front();
            queue.pop();
            
            if (current == target) {
                return true;
            }
            
            auto neighbors = board->getNeighbors(current);
            
            if (board->isTeleport(current)) {
                std::string dest = board->getTeleportDestination(current);
                if (!dest.empty() && dest != current) {
                    neighbors.push_back(dest);
                }
            }
            
            for (const auto& neighbor : neighbors) {
                if (visited.find(neighbor) != visited.end()) continue;
                
                if (canMoveThrough(current, neighbor, alliesList, enemiesList)) {
                    queue.push(neighbor);
                    visited.insert(neighbor);
                }
            }
        }
    }
    
    while (!queue.empty()) {
        std::string pos = queue.front();
        queue.pop();
        if (pos == target) {
            return true;
        }
    }
    
    return false;
}

std::vector<std::vector<std::string>> Movement::findPaths(
    const std::string& start,
    const std::string& target,
    int maxSteps,
    const std::vector<character*>& alliesList,
    const std::vector<character*>& enemiesList) const {
    
    std::vector<std::vector<std::string>> result;
    if (!board->hasSpace(start) || !board->hasSpace(target)) {
        return result;
    }
    
    std::function<void(std::string, int, std::vector<std::string>&)> dfs =
        [&](std::string current, int steps, std::vector<std::string>& path) {
            if (current == target) {
                result.push_back(path);
                return;
            }
            
            if (steps >= maxSteps) return;
            
            auto neighbors = board->getNeighbors(current);
            
            if (board->isTeleport(current)) {
                std::string dest = board->getTeleportDestination(current);
                if (!dest.empty() && dest != current) {
                    neighbors.push_back(dest);
                }
            }
            
            for (const auto& neighbor : neighbors) {
                if (std::find(path.begin(), path.end(), neighbor) != path.end()) continue;
                
                if (canMoveThrough(current, neighbor, alliesList, enemiesList)) {
                    path.push_back(neighbor);
                    dfs(neighbor, steps + 1, path);
                    path.pop_back();
                }
            }
        };
    
    std::vector<std::string> path = {start};
    dfs(start, 0, path);
    
    return result;
}

void Movement::boost(character* character, card* playedCard, ActionType currentAction) const {
    if (!character || !playedCard || currentAction != ActionType::MANEUVER) {
        return;
    }
    if (currentAction == ActionType::MANEUVER) {
        character->setnewmovement(character->getmovement() + playedCard->getboost());
    }
}