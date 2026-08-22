#include "../include/deployment.hpp"
#include "../include/invisible_man.hpp"
#include <iostream>
#include <queue>
#include <unordered_set>

Deployment::Deployment(Board* board) : board(board) {}

bool Deployment::isValidPlacement(int node) const {
    string nodeName = "n" + to_string(node);
    return board->hasSpace(nodeName) && !isOccupied(node);
}

bool Deployment::isOccupied(int node) const {
    for (int n : occupiedNodes) {
        if (n == node) return true;
    }
    return false;
}

void Deployment::markOccupied(int node) {
    occupiedNodes.push_back(node);
}

void Deployment::clearOccupied() {
    occupiedNodes.clear();
}

bool Deployment::isSameZone(int node1, int node2) const {
    auto zones1 = board->getZonesAt(node1, 0);
    auto zones2 = board->getZonesAt(node2, 0);
    
    for (const auto& z1 : zones1) {
        for (const auto& z2 : zones2) {
            if (z1 == z2) return true;
        }
    }
    return false;
}

std::vector<int> Deployment::getDeploymentZone(int heroNode) const {
    std::vector<int> result;
    auto zones = board->getZonesAt(heroNode, 0);
    
    for (const auto& zone : zones) {
        auto spaces = board->getSpacesInZone(zone);
        for (const auto& space : spaces) {
            int node = space.first;
            if (!isOccupied(node) && node != heroNode) {
                result.push_back(node);
            }
        }
    }
    
    return result;
}

Deployment::PlacementResult Deployment::placeHero(character* hero, int node) {
    PlacementResult result;
    result.success = false;
    
    if (!hero) {
        result.message = "Hero is null";
        return result;
    }
    
    if (!isValidPlacement(node)) {
        result.message = "Invalid placement position";
        return result;
    }
    
    hero->setposition(node);
    markOccupied(node);

    // The Invisible Man has no Sidekick figure. Instead, at deployment he
    // places his three fog tokens on separate spaces within his own
    // starting Zone -- this is the one place every hero gets placed, so
    // it's the right spot to set that up automatically instead of relying
    // on every call site to remember to do it.
    if (InvisibleMan* invMan = dynamic_cast<InvisibleMan*>(hero)) {
        invMan->setBoard(board); // make sure the board is wired even if setBoard() wasn't called yet
        invMan->initializeFogTokens(node);
    }

    result.success = true;
    result.message = "Hero placed successfully";
    result.positions.push_back(node);
    
    return result;
}

Deployment::PlacementResult Deployment::placeSidekick(character* sidekick, int heroNode) {
    PlacementResult result;
    result.success = false;
    
    if (!sidekick) {
        result.message = "Sidekick is null";
        return result;
    }
    
    auto zone = getDeploymentZone(heroNode);
    
    for (int node : zone) {
        if (isValidPlacement(node)) {
            sidekick->setposition(node);
            markOccupied(node);
            result.success = true;
            result.message = "Sidekick placed successfully";
            result.positions.push_back(node);
            return result;
        }
    }
    
    result.message = "No valid position for sidekick in hero's zone";
    return result;
}

Deployment::PlacementResult Deployment::placeHeroWithSidekicks(
    character* hero,
    std::vector<character*> sidekicks,
    int heroNode) {
    
    PlacementResult result;
    result.success = false;
    size_t occupiedBefore = occupiedNodes.size();
    
    auto heroResult = placeHero(hero, heroNode);
    if (!heroResult.success) {
        result.message = "Failed to place hero: " + heroResult.message;
        return result;
    }
    
    result.positions.push_back(heroNode);
    
    for (auto sidekick : sidekicks) {
        auto sidekickResult = placeSidekick(sidekick, heroNode);
        if (!sidekickResult.success) {
            occupiedNodes.resize(occupiedBefore);
            result.message = "Failed to place sidekick: " + sidekickResult.message;
            return result;
        }
        result.positions.push_back(sidekickResult.positions[0]);
    }
    
    result.success = true;
    result.message = "All characters placed successfully";
    return result;
}