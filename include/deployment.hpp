#ifndef DEPLOYMENT_HPP
#define DEPLOYMENT_HPP

#include "board.hpp"
#include "character.hpp"
#include <vector>
#include <utility>

class Deployment {
private:
    Board* board;
    std::vector<std::pair<int, int>> occupiedSpaces;
    
    bool isOccupied(int x, int y) const;
    void markOccupied(int x, int y);
    void clearOccupied();
    
public:
    Deployment(Board* board);
    
    struct PlacementResult {
        bool success;
        std::string message;
        std::vector<std::pair<int, int>> positions;
    };
    
    PlacementResult placeHero(character* hero, int x, int y);
    PlacementResult placeSidekick(character* sidekick, int heroX, int heroY);
    PlacementResult placeHeroWithSidekicks(
        character* hero,
        std::vector<character*> sidekicks,
        int heroX, int heroY
    );
    
    bool isValidPlacement(int x, int y) const;
    bool isSameZone(int x1, int y1, int x2, int y2) const;
    std::vector<std::pair<int, int>> getDeploymentZone(int heroX, int heroY) const;
};

#endif