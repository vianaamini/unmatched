#include "../include/movement.hpp"
#include <queue>
#include <unordered_set>
#include <functional>
#include <algorithm>
#include <iostream>

Movement::Movement(const Board* board) : board(board) {}

bool Movement::isPositionOccupiedByEnemy(int node,
                                         const std::vector<character*>& enemiesList) const {
    for (const auto& enemy : enemiesList) {
        if (!enemy || !enemy->isalive()) continue;
        if (enemy->getx() == node) {
            return true;
        }
    }
    return false;
}

bool Movement::isPositionOccupiedByAlly(int node,
                                        const std::vector<character*>& alliesList,
                                        character* currentChar) const {
    for (const auto& ally : alliesList) {
        if (!ally || ally == currentChar || !ally->isalive()) continue;
        if (ally->getx() == node) {
            return true;
        }
    }
    return false;
}

bool Movement::isPositionOccupied(int node,
                                  const std::vector<character*>& alliesList,
                                  const std::vector<character*>& enemiesList,
                                  character* currentChar) const {
    return isPositionOccupiedByEnemy(node, enemiesList) ||
           isPositionOccupiedByAlly(node, alliesList, currentChar);
}

bool Movement::canMoveThrough(int from, int to,
                              const std::vector<character*>& alliesList,
                              const std::vector<character*>& enemiesList,
                              character* currentChar) const {
    if (!board->hasSpace("n" + to_string(to))) return false;
    
    if (isPositionOccupiedByEnemy(to, enemiesList)) return false;
    
    string fromName = "n" + to_string(from);
    string toName = "n" + to_string(to);
    
    if (board->isConnected(fromName, toName)) return true;
    if (board->isTeleport(fromName) && board->getTeleportDestination(fromName) == toName) {
        return true;
    }
    return false;
}

bool Movement::canStopAt(int node,
                         const std::vector<character*>& alliesList,
                         const std::vector<character*>& enemiesList,
                         character* currentChar) const {
    if (!board->hasSpace("n" + to_string(node))) return false;
    if (isPositionOccupied(node, alliesList, enemiesList, currentChar)) return false;
    return true;
}

int Movement::getBaseMovement(const character* c) const {
    if (!c) return 0;
    return c->getmovement();
}

std::vector<std::string> Movement::getPossibleMoves(
    character* c,
    int steps,
    const std::vector<character*>& alliesList,
    const std::vector<character*>& enemiesList) const {
    
    if (!c || steps <= 0) return {};
    
    int startNode = c->getx();
    if (!board->hasSpace("n" + to_string(startNode))) return {};
    
    std::vector<std::string> validMoves;
    std::queue<int> q;
    std::unordered_set<int> visited;
    
    q.push(startNode);
    visited.insert(startNode);
    
    for (int step = 0; step < steps; ++step) {
        int levelSize = q.size();
        for (int i = 0; i < levelSize; ++i) {
            int current = q.front();
            q.pop();
            
            auto neighbors = board->getNeighborIds(current);
            
            string currentName = "n" + to_string(current);
            if (board->isTeleport(currentName)) {
                string dest = board->getTeleportDestination(currentName);
                int destId = board->getNodeId(dest);
                if (destId != current && visited.find(destId) == visited.end()) {
                    neighbors.push_back(destId);
                }
            }
            
            for (int neighbor : neighbors) {
                if (visited.find(neighbor) != visited.end()) continue;
                if (canMoveThrough(current, neighbor, alliesList, enemiesList, c)) {
                    if (canStopAt(neighbor, alliesList, enemiesList, c)) {
                        validMoves.push_back("n" + to_string(neighbor));
                    }
                    q.push(neighbor);
                    visited.insert(neighbor);
                }
            }
        }
    }
    
    return validMoves;
}

bool Movement::canReach(const std::string& start,
                        const std::string& target,
                        int steps,
                        const std::vector<character*>& alliesList,
                        const std::vector<character*>& enemiesList) const {
    
    if (!board->hasSpace(start) || !board->hasSpace(target)) return false;
    
    int startNode = board->getNodeId(start);
    int targetNode = board->getNodeId(target);
    
    std::queue<int> q;
    std::unordered_set<int> visited;
    
    q.push(startNode);
    visited.insert(startNode);
    
    for (int step = 0; step < steps; ++step) {
        int levelSize = q.size();
        for (int i = 0; i < levelSize; ++i) {
            int current = q.front();
            q.pop();
            
            if (current == targetNode) return true;
            
            auto neighbors = board->getNeighborIds(current);
            
            string currentName = "n" + to_string(current);
            if (board->isTeleport(currentName)) {
                string dest = board->getTeleportDestination(currentName);
                int destId = board->getNodeId(dest);
                if (destId != current && visited.find(destId) == visited.end()) {
                    neighbors.push_back(destId);
                }
            }
            
            for (int neighbor : neighbors) {
                if (visited.find(neighbor) != visited.end()) continue;
                if (canMoveThrough(current, neighbor, alliesList, enemiesList, nullptr)) {
                    q.push(neighbor);
                    visited.insert(neighbor);
                }
            }
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
    if (!board->hasSpace(start) || !board->hasSpace(target)) return result;
    
    int startNode = board->getNodeId(start);
    int targetNode = board->getNodeId(target);
    
    std::function<void(int, int, std::vector<int>&)> dfs =
        [&](int current, int steps, std::vector<int>& path) {
            if (current == targetNode) {
                std::vector<std::string> stringPath;
                for (int node : path) {
                    stringPath.push_back("n" + to_string(node));
                }
                result.push_back(stringPath);
                return;
            }
            if (steps >= maxSteps) return;
            
            auto neighbors = board->getNeighborIds(current);
            
            string currentName = "n" + to_string(current);
            if (board->isTeleport(currentName)) {
                string dest = board->getTeleportDestination(currentName);
                int destId = board->getNodeId(dest);
                if (destId != current) {
                    neighbors.push_back(destId);
                }
            }
            
            for (int neighbor : neighbors) {
                if (std::find(path.begin(), path.end(), neighbor) != path.end()) continue;
                if (canMoveThrough(current, neighbor, alliesList, enemiesList, nullptr)) {
                    path.push_back(neighbor);
                    dfs(neighbor, steps + 1, path);
                    path.pop_back();
                }
            }
        };
    
    std::vector<int> path = {startNode};
    dfs(startNode, 0, path);
    return result;
}

void Movement::boost(character* c, const card* playedCard, ActionType currentAction) const {
    if (!c || !playedCard || currentAction != ActionType::MANEUVER) return;
    c->setnewmovement(c->getmovement() + playedCard->getboost());
}