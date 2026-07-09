#include "../include/board.hpp"
#include <cmath>
#include <algorithm>

Board::Board(int width, int height) : width(width), height(height) {
    grid.resize(height, std::vector<Space>(width));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            grid[y][x] = Space(x, y);
        }
    }
}

bool Board::isValid(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool Board::isWalkable(int x, int y) const {
    return isValid(x, y) && grid[y][x].isWalkable && !grid[y][x].hasObstacle;
}

bool Board::hasObstacle(int x, int y) const {
    return isValid(x, y) && grid[y][x].hasObstacle;
}

void Board::setWalkable(int x, int y, bool walkable) {
    if (isValid(x, y)) {
        grid[y][x].isWalkable = walkable;
    }
}

void Board::setObstacle(int x, int y, bool hasObstacle) {
    if (isValid(x, y)) {
        grid[y][x].hasObstacle = hasObstacle;
    }
}

// ================================================
// تله‌پورت
// ================================================

void Board::addTeleport(int x1, int y1, int x2, int y2) {
    if (!isValid(x1, y1) || !isValid(x2, y2)) return;
    
    int id1 = x1 * 1000 + y1;
    int id2 = x2 * 1000 + y2;

    teleportpairs[id1] = id2;
    teleportpairs[id2] = id1;

    grid[y1][x1].hasTeleport = true;
    grid[y2][x2].hasTeleport = true;
}

int Board::getTeleportDestination(int x, int y) const {
    int id = x * 1000 + y;
    auto it = teleportpairs.find(id);
    if (it != teleportpairs.end()) {
        return it->second;
    }
    return id;
}

bool Board::isTeleport(int x, int y) const {
    int id = x * 1000 + y;
    return teleportpairs.find(id) != teleportpairs.end();
}

// ================================================
// زون‌ها
// ================================================

void Board::addZone(const std::string& zoneName, const std::vector<std::pair<int, int>>& spaces) {
    for (const auto& pos : spaces) {
        addSpaceToZone(pos.first, pos.second, zoneName);
    }
}

void Board::addSpaceToZone(int x, int y, const std::string& zoneName) {
    if (isValid(x, y)) {
        grid[y][x].zones.push_back(zoneName);
        zoneMap[zoneName].push_back({x, y});
    }
}

std::vector<std::string> Board::getZonesAt(int x, int y) const {
    if (!isValid(x, y)) return {};
    return grid[y][x].zones;
}

std::vector<std::pair<int, int>> Board::getSpacesInZone(const std::string& zoneName) const {
    auto it = zoneMap.find(zoneName);
    if (it != zoneMap.end()) {
        return it->second;
    }
    return {};
}

// ================================================
// مجاورت و حرکت
// ================================================

bool Board::isAdjacent(int x1, int y1, int x2, int y2) const {
    if (!isValid(x1, y1) || !isValid(x2, y2)) return false;
    if (!isWalkable(x2, y2)) return false;
    
    int dx = std::abs(x1 - x2);
    int dy = std::abs(y1 - y2);
    
    return (dx == 1 && dy == 0) || (dx == 0 && dy == 1);
}

std::vector<std::pair<int, int>> Board::getAdjacentSpaces(int x, int y) const {
    std::vector<std::pair<int, int>> result;
    if (!isValid(x, y)) return result;
    
    std::vector<std::pair<int, int>> directions = {
        {x - 1, y}, {x + 1, y},
        {x, y - 1}, {x, y + 1}
    };
    
    for (const auto& dir : directions) {
        if (isWalkable(dir.first, dir.second)) {
            result.push_back(dir);
        }
    }
    
    return result;
}

// ================================================
// گرفتن اطلاعات
// ================================================

std::vector<std::pair<int, int>> Board::getAllSpaces() const {
    std::vector<std::pair<int, int>> result;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            result.push_back({x, y});
        }
    }
    return result;
}

std::vector<std::pair<int, int>> Board::getWalkableSpaces() const {
    std::vector<std::pair<int, int>> result;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (isWalkable(x, y)) {
                result.push_back({x, y});
            }
        }
    }
    return result;
}