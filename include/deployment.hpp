#ifndef DEPLOYMENT_HPP
#define DEPLOYMENT_HPP

#include "map.hpp"
#include "character.hpp"
#include <vector>
#include <utility>

class Deployment {
private:
    Board* board;
    std::vector<int> occupiedNodes;
    
    bool isOccupied(int node) const;
    void markOccupied(int node);
    void clearOccupied();
    
public:
    Deployment(Board* board);
    
    struct PlacementResult {
        bool success;
        std::string message;
        std::vector<int> positions;
    };
    
    PlacementResult placeHero(character* hero, int node);
    PlacementResult placeSidekick(character* sidekick, int heroNode);
    PlacementResult placeHeroWithSidekicks(
        character* hero,
        std::vector<character*> sidekicks,
        int heroNode
    );
    
    bool isValidPlacement(int node) const;
    bool isSameZone(int node1, int node2) const;
    std::vector<int> getDeploymentZone(int heroNode) const;
};

#endif