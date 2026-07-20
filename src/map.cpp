#include "../include/map.hpp"
#include <algorithm>
#include <cmath>

Board::Board(int width, int height) : width(width), height(height) {
    addSpace("n1", 16, 3, {NodeColor::RED});
    addSpace("n2", 24, 3, {NodeColor::NONE});
    addSpace("n3", 32, 3, {NodeColor::NONE});
    addSpace("n4", 8,  5, {NodeColor::NONE});
    addSpace("n5", 16, 5, {NodeColor::NONE});
    addSpace("n6", 24, 5, {NodeColor::YELLOW, NodeColor::GREEN});
    addSpace("n7", 32, 5, {NodeColor::NONE});
    addSpace("n8", 40, 5, {NodeColor::NONE});
    addSpace("n9",  2,  7, {NodeColor::NONE});         
    addSpace("n10", 13, 7, {NodeColor::NONE});    
    addSpace("n11", 24, 7, {NodeColor::BLUE});             
    addSpace("n12", 35, 7, {NodeColor::NONE});    
    addSpace("n13", 46, 7, {NodeColor::NONE});       
    addSpace("n14", 8,  9, {NodeColor::NONE});
    addSpace("n15", 16, 9, {NodeColor::NONE});
    addSpace("n16", 24, 9, {NodeColor::NONE});
    addSpace("n17", 32, 9, {NodeColor::GREEN});
    addSpace("n18", 40, 9, {NodeColor::NONE});
    addSpace("n19", 16, 11, {NodeColor::NONE});
    addSpace("n20", 24, 11, {NodeColor::NONE});
    addSpace("n21", 32, 11, {NodeColor::NONE});

    addEdge("n1", "n2"); addEdge("n2", "n3"); addEdge("n1", "n4");
    addEdge("n3", "n8"); addEdge("n4", "n9"); addEdge("n9", "n14");
    addEdge("n9", "n10"); addEdge("n10", "n11"); addEdge("n11", "n12"); 
    addEdge("n12", "n13"); addEdge("n14", "n19"); addEdge("n13", "n18");
    addEdge("n19", "n20"); addEdge("n20", "n21"); addEdge("n1", "n5"); 
    addEdge("n2", "n6"); addEdge("n5", "n11"); addEdge("n6", "n11"); 
    addEdge("n7", "n11"); addEdge("n11", "n15"); addEdge("n11", "n16"); 
    addEdge("n11", "n17"); addEdge("n15", "n19"); addEdge("n16", "n20");
    addEdge("n17", "n21"); addEdge("n18", "n21");
    addEdge("n10", "n14"); addEdge("n12", "n17");
    addEdge("n3", "n7"); addEdge("n4", "n5");
    addEdge("n5", "n6"); addEdge("n6", "n7");
    addEdge("n7", "n8");
    
    // تلپورت‌ها (گذرگاه‌های مخفی)
    addTeleport("n1", "n32");
    addTeleport("n10", "n22");
    addTeleport("n16", "n6");
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

bool Board::isAdjacent(int node1, int node2) const {
    string n1 = "n" + to_string(node1);
    string n2 = "n" + to_string(node2);
    return isConnected(n1, n2);
}

vector<int> Board::getNeighborIds(int node) const {
    string name = "n" + to_string(node);
    vector<int> result;
    for (const auto& neighbor : getNeighbors(name)) {
        result.push_back(stoi(neighbor.substr(1)));
    }
    return result;
}

vector<string> Board::getNeighbors(const string& spaceName) const {
    auto it = adjacencyList.find(spaceName);
    return (it != adjacencyList.end()) ? it->second : vector<string>{};
}

pair<int, int> Board::getCoordinates(const string& spaceName) const {
    auto it = tuiCoordinates.find(spaceName);
    return (it != tuiCoordinates.end()) ? it->second : pair<int, int>{-1, -1};
}

bool Board::hasSpace(const string& spaceName) const {
    return tuiCoordinates.find(spaceName) != tuiCoordinates.end();
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

bool Board::isValid(int x, int y) const {
    return x >= 0 && x < 10 && y >= 0 && y < 10;
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
    if (it != zoneMap.end()) {
        for (const auto& name : it->second) {
            int id = stoi(name.substr(1));
            result.push_back({id, 0});
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

vector<string> Board::getAllSpaceIds() const {
    vector<string> result;
    for (const auto& pair : tuiCoordinates) {
        result.push_back(pair.first);
    }
    return result;
}

int Board::getNodeId(const string& nodeName) const {
    return stoi(nodeName.substr(1));
}

string Board::getNodeName(int id) const {
    return "n" + to_string(id);
}

NodeColor Board::getNodeColorByName(const string& nodeName) const {
    for (const auto& pair : zoneMap) {
        NodeColor color = pair.first;
        const vector<string>& spaces = pair.second;
        if (find(spaces.begin(), spaces.end(), nodeName) != spaces.end()) {
            return color;
        }
    }
    return NodeColor::NONE;
}