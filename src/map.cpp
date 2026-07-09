#include "../include/map.hpp"
#include <algorithm>
#include <stdexcept>
#include <cmath>

Board::Board(int width, int height) : width(width), height(height) {}

bool Board::isValid(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool Board::isWalkable(int x, int y) const {
    string name = "n" + to_string(x);
    return hasSpace(name) && obstacleSpaces.find(name) == obstacleSpaces.end();
}

bool Board::hasObstacle(int x, int y) const {
    string name = "n" + to_string(x);
    return obstacleSpaces.find(name) != obstacleSpaces.end();
}

void Board::setWalkable(int x, int y, bool walkable) {
    string name = "n" + to_string(x);
    if (!walkable) {
        obstacleSpaces.insert(name);
    } else {
        obstacleSpaces.erase(name);
    }
}

void Board::setObstacle(int x, int y, bool hasObstacle) {
    string name = "n" + to_string(x);
    if (hasObstacle) {
        obstacleSpaces.insert(name);
    } else {
        obstacleSpaces.erase(name);
    }
}

bool Board::isAdjacent(int x1, int y1, int x2, int y2) const {
    string name1 = "n" + to_string(x1);
    string name2 = "n" + to_string(x2);
    return isConnected(name1, name2);
}

vector<pair<int, int>> Board::getAdjacentSpaces(int x, int y) const {
    string name = "n" + to_string(x);
    vector<pair<int, int>> result;
    for (const auto& neighbor : getNeighbors(name)) {
        int id = stoi(neighbor.substr(1));
        result.push_back({id, 0});
    }
    return result;
}

vector<pair<int, int>> Board::getAllSpaces() const {
    vector<pair<int, int>> result;
    for (const auto& pair : tuiCoordinates) {
        result.push_back(pair.second);
    }
    return result;
}

vector<pair<int, int>> Board::getWalkableSpaces() const {
    vector<pair<int, int>> result;
    for (const auto& pair : tuiCoordinates) {
        if (obstacleSpaces.find(pair.first) == obstacleSpaces.end()) {
            result.push_back(pair.second);
        }
    }
    return result;
}

void Board::addZone(const string& zoneName, const vector<pair<int, int>>& spaces) {
    for (const auto& pos : spaces) {
        addSpaceToZone(pos.first, pos.second, zoneName);
    }
}

void Board::addSpaceToZone(int x, int y, const string& zoneName) {
    string name = "n" + to_string(x);
    NodeColor color = NodeColor::NONE;
    if (zoneName == "RED") color = NodeColor::RED;
    else if (zoneName == "BLUE") color = NodeColor::BLUE;
    else if (zoneName == "YELLOW") color = NodeColor::YELLOW;
    else if (zoneName == "GREEN") color = NodeColor::GREEN;
    else if (zoneName == "PURPLE") color = NodeColor::PURPLE;
    else if (zoneName == "BROWN") color = NodeColor::BROWN;
    zoneMap[color].push_back(name);
}

vector<string> Board::getZonesAt(int x, int y) const {
    string name = "n" + to_string(x);
    vector<string> result;
    for (const auto& pair : zoneMap) {
        if (find(pair.second.begin(), pair.second.end(), name) != pair.second.end()) {
            string zoneName;
            switch (pair.first) {
                case NodeColor::RED: zoneName = "RED"; break;
                case NodeColor::BLUE: zoneName = "BLUE"; break;
                case NodeColor::YELLOW: zoneName = "YELLOW"; break;
                case NodeColor::GREEN: zoneName = "GREEN"; break;
                case NodeColor::PURPLE: zoneName = "PURPLE"; break;
                case NodeColor::BROWN: zoneName = "BROWN"; break;
                default: zoneName = "NONE"; break;
            }
            result.push_back(zoneName);
        }
    }
    return result;
}

vector<pair<int, int>> Board::getSpacesInZone(const string& zoneName) const {
    NodeColor color = NodeColor::NONE;
    if (zoneName == "RED") color = NodeColor::RED;
    else if (zoneName == "BLUE") color = NodeColor::BLUE;
    else if (zoneName == "YELLOW") color = NodeColor::YELLOW;
    else if (zoneName == "GREEN") color = NodeColor::GREEN;
    else if (zoneName == "PURPLE") color = NodeColor::PURPLE;
    else if (zoneName == "BROWN") color = NodeColor::BROWN;
    
    vector<pair<int, int>> result;
    auto it = zoneMap.find(color);
    if (it !=zoneMap.end()) {
        for (const auto& name : it->second) {
            int id = stoi(name.substr(1));
            result.push_back({id, 0});
        }
    }
    return result;
}

void Board::addSpace(const string& name, int tuiX, int tuiY, const vector<NodeColor>& colors) {
    tuiCoordinates[name] = {tuiX, tuiY};
    for (const auto& color : colors) {
        zoneMap[color].push_back(name);
    }
    if (adjacencyList.find(name) == adjacencyList.end()) {
        adjacencyList[name] = vector<string>();
    }
}

void Board::addEdge(const string& spaceA, const string& spaceB) {
    adjacencyList[spaceA].push_back(spaceB);
    adjacencyList[spaceB].push_back(spaceA);
}

bool Board::isConnected(const string& spaceA, const string& spaceB) const {
    auto it = adjacencyList.find(spaceA);
    if (it != adjacencyList.end()) {
        return find(it->second.begin(), it->second.end(), spaceB) != it->second.end();
    }
    return false;
}

vector<string> Board::getNeighbors(const string& spaceName) const {
    auto it = adjacencyList.find(spaceName);
    if (it != adjacencyList.end()) {
        return it->second;
    }
    return {};
}

pair<int, int> Board::getCoordinates(const string& spaceName) const {
    auto it = tuiCoordinates.find(spaceName);
    if (it != tuiCoordinates.end()) {
        return it->second;
    }
    return {-1, -1};
}

bool Board::hasSpace(const string& spaceName) const {
    return tuiCoordinates.find(spaceName) != tuiCoordinates.end();
}

vector<string> Board::getAllSpaceIds() const {
    vector<string> result;
    for (const auto& pair : tuiCoordinates) {
        result.push_back(pair.first);
    }
    return result;
}

void Board::addTeleport(const string& from, const string& to) {
    teleportPairs[from] = to;
    teleportPairs[to] = from;
    teleportSpaces.insert(from);
    teleportSpaces.insert(to);
}

string Board::getTeleportDestination(const string& spaceName) const {
    auto it = teleportPairs.find(spaceName);
    if (it != teleportPairs.end()) {
        return it->second;
    }
    return spaceName;
}

bool Board::isTeleport(const string& spaceName) const {
    return teleportSpaces.find(spaceName) != teleportSpaces.end();
}