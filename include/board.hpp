#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include <unordered_map>
#include <utility>
#include <string>

struct Space {
    int x, y;
    std::vector<std::string> zones;
    bool isWalkable;
    bool hasObstacle;
    bool hasteleport;
    
    Space(int x = 0, int y = 0) 
        : x(x), y(y), isWalkable(true), hasObstacle(false) {}
};

class Board {
private:
    int width, height;
    std::vector<std::vector<Space>> grid;
    std::unordered_map<int , int> teleportpairs;
    std::unordered_map<std::string, std::vector<std::pair<int, int>>> zoneMap;
    
    bool isValid(int x, int y) const;
    
public:
    Board(int width = 10, int height = 10);
    
    bool isWalkable(int x, int y) const;
    bool hasObstacle(int x, int y) const;
    
    void setWalkable(int x, int y, bool walkable);
    void setObstacle(int x, int y, bool hasObstacle);
    void addteleport(int x1,int y1,int x2, int y2);
    //void setteleport(int x,int y);
    void addZone(const std::string& zoneName, const std::vector<std::pair<int, int>>& spaces);
    void addSpaceToZone(int x, int y, const std::string& zoneName);
    std::vector<std::string> getZonesAt(int x, int y) const;
    std::vector<std::pair<int, int>> getSpacesInZone(const std::string& zoneName) const;
    
    bool isAdjacent(int x1, int y1, int x2, int y2) const;
    bool isteleport(int x,int y) const;
    std::vector<std::pair<int, int>> getAdjacentSpaces(int x, int y) const;
    std::vector<std::pair<int, int>> getAllSpaces() const;
    std::vector<std::pair<int, int>> getWalkableSpaces() const;
    
    int destination(int x,int y) const;
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

#endif