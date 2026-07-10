#include "../include/deployment.hpp"
#include <iostream>

Deployment::Deployment(Board* board) : board(board) {}

bool Deployment::isValidPlacement(int x, int y) const {
    return board->isWalkable(x, y) && !isOccupied(x, y);
}

bool Deployment::isOccupied(int x, int y) const {
    for (const auto& pos : occupiedSpaces) {
        if (pos.first == x && pos.second == y) {
            return true;
        }
    }
    return false;
}

void Deployment::markOccupied(int x, int y) {
    occupiedSpaces.push_back({x, y});
}

void Deployment::clearOccupied() {
    occupiedSpaces.clear();
}

bool Deployment::isSameZone(int x1, int y1, int x2, int y2) const {
    auto zones1 = board->getZonesAt(x1, y1);
    auto zones2 = board->getZonesAt(x2, y2);
    
    for (const auto& z1 : zones1) {
        for (const auto& z2 : zones2) {
            if (z1 == z2) {
                return true;
            }
        }
    }
    return false;
}

std::vector<std::pair<int, int>> Deployment::getDeploymentZone(int heroX, int heroY) const {
    std::vector<std::pair<int, int>> result;
    auto zones = board->getZonesAt(heroX, heroY);
    
    for (const auto& zone : zones) {
        auto spaces = board->getSpacesInZone(zone);
        for (const auto& space : spaces) {
            if (board->isWalkable(space.first, space.second) && 
                !isOccupied(space.first, space.second)) {
                result.push_back(space);
            }
        }
    }
    
    return result;
}

Deployment::PlacementResult Deployment::placeHero(character* hero, int x, int y) {
    PlacementResult result;
    result.success = false;
    
    if (!hero) {
        result.message = "Hero is null";
        return result;
    }
    
    if (!isValidPlacement(x, y)) {
        result.message = "Invalid placement position";
        return result;
    }
    
    hero->setposition(x, y);
    markOccupied(x, y);
    result.success = true;
    result.message = "Hero placed successfully";
    result.positions.push_back({x, y});
    
    return result;
}

Deployment::PlacementResult Deployment::placeSidekick(character* sidekick, int heroX, int heroY) {
    PlacementResult result;
    result.success = false;
    
    if (!sidekick) {
        result.message = "Sidekick is null";
        return result;
    }
    
    auto zone = getDeploymentZone(heroX, heroY);
    
    for (const auto& pos : zone) {
        if (isValidPlacement(pos.first, pos.second) && 
            isSameZone(heroX, heroY, pos.first, pos.second)) {
            
            sidekick->setposition(pos.first, pos.second);
            markOccupied(pos.first, pos.second);
            result.success = true;
            result.message = "Sidekick placed successfully";
            result.positions.push_back(pos);
            return result;
        }
    }
    
    result.message = "No valid position for sidekick in hero's zone";
    return result;
}

Deployment::PlacementResult Deployment::placeHeroWithSidekicks(
    character* hero,
    std::vector<character*> sidekicks,
    int heroX, int heroY) {
    
    clearOccupied();
    PlacementResult result;
    result.success = false;
    
    auto heroResult = placeHero(hero, heroX, heroY);
    if (!heroResult.success) {
        result.message = "Failed to place hero: " + heroResult.message;
        return result;
    }
    
    result.positions.push_back({heroX, heroY});
    
    for (auto sidekick : sidekicks) {
        auto sidekickResult = placeSidekick(sidekick, heroX, heroY);
        if (!sidekickResult.success) {
            result.message = "Failed to place sidekick: " + sidekickResult.message;
            clearOccupied();
            return result;
        }
        result.positions.push_back(sidekickResult.positions[0]);
    }
    
    result.success = true;
    result.message = "All characters placed successfully";
    return result;
}