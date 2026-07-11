#include "../include/map.hpp"
#include <algorithm>
#include <cmath>

Board::Board(int width, int height) : width(width), height(height) {
    addSpace("n16", 18, 7, {NodeColor::NONE});

    addSpace("n1", 2, 7, {NodeColor::RED});
    addSpace("n2", 6, 7, {NodeColor::NONE});
    addSpace("n10", 10, 7, {NodeColor::YELLOW, NodeColor::GREEN});
    addSpace("n22", 26, 7, {NodeColor::YELLOW, NodeColor::GREEN});
    addSpace("n30", 30, 7, {NodeColor::NONE});
    addSpace("n32", 34, 7, {NodeColor::BLUE});

    addSpace("n13", 24, 3, {NodeColor::GREEN});
    addSpace("n6", 36, 3, {NodeColor::NONE});

    addSpace("n5", 3, 5, {NodeColor::NONE});
    addSpace("n7", 9, 5, {NodeColor::NONE});
    addSpace("n3", 15, 5, {NodeColor::NONE});
    addSpace("n4", 21, 5, {NodeColor::NONE});
    addSpace("n8", 27, 5, {NodeColor::NONE});
    addSpace("n9", 33, 5, {NodeColor::NONE});
    addSpace("n11", 39, 5, {NodeColor::NONE});

    addSpace("n12", 9, 7, {NodeColor::NONE});
    addSpace("n14", 15, 7, {NodeColor::NONE});
    addSpace("n15", 21, 7, {NodeColor::NONE});
    addSpace("n19", 27, 7, {NodeColor::NONE});
    addSpace("n27", 33, 7, {NodeColor::NONE});
    addSpace("n26", 39, 7, {NodeColor::NONE});

    addSpace("n28", 9, 9, {NodeColor::NONE});
    addSpace("n29", 15, 9, {NodeColor::NONE});
    addSpace("n25", 21, 9, {NodeColor::NONE});
    addSpace("n24", 27, 9, {NodeColor::NONE});
    addSpace("n31", 33, 9, {NodeColor::NONE});
    addSpace("n21", 12, 11, {NodeColor::NONE});
    addSpace("n20", 18, 11, {NodeColor::NONE});
    addSpace("n18", 24, 11, {NodeColor::NONE});
    addSpace("n23", 30, 11, {NodeColor::NONE});

    addEdge("n1", "n2"); addEdge("n2", "n10"); addEdge("n10", "n16");
    addEdge("n16", "n22"); addEdge("n22", "n30"); addEdge("n30", "n32");

    addEdge("n16", "n13");
    addEdge("n16", "n19"); 
    addEdge("n16", "n12"); 
    addEdge("n16", "n14");
    addEdge("n16", "n18"); 
    addEdge("n1", "n3"); addEdge("n3", "n5"); addEdge("n5", "n6");
    addEdge("n6", "n7"); addEdge("n7", "n9"); addEdge("n9", "n32");
    addEdge("n1", "n29"); addEdge("n29", "n26"); addEdge("n26", "n27");
    addEdge("n27", "n28"); addEdge("n28", "n31"); addEdge("n31", "n32");
  
    addEdge("n2", "n11"); addEdge("n11", "n12"); addEdge("n13", "n4"); addEdge("n4", "n5");
    addEdge("n7", "n8"); addEdge("n8", "n14"); addEdge("n15", "n30"); addEdge("n14", "n15");
    addEdge("n2", "n21"); addEdge("n21", "n20"); addEdge("n19", "n25"); addEdge("n25", "n26");
    addEdge("n27", "n24"); addEdge("n24", "n18"); addEdge("n18", "n23"); addEdge("n23", "n30");

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