#include "../include/map.hpp"
#include <algorithm>
#include <cmath>

Board::Board(int width, int height) : width(width), height(height) {

    addSpace("n1",  104,  121, {NodeColor::BLUE});                   
    addSpace("n2",  281,  75,  {NodeColor::BLUE});                    
    addSpace("n3",  86,   275, {NodeColor::BLUE});                    
    addSpace("n4",  245,  291, {NodeColor::BLUE});                 
    addSpace("n5",  380,  229, {NodeColor::BLUE, NodeColor::BROWN});  

    addSpace("n6",  518,  108, {NodeColor::BROWN});
    addSpace("n7",  684,  227, {NodeColor::BROWN});
    addSpace("n8",  774,  113, {NodeColor::BROWN});
    addSpace("n9",  923,  222, {NodeColor::BROWN, NodeColor::GREEN, NodeColor::YELLOW}); 
    addSpace("n10", 1068, 118, {NodeColor::YELLOW});
    addSpace("n11", 1148, 230, {NodeColor::YELLOW});
    addSpace("n12", 1242, 121, {NodeColor::YELLOW});                  

    addSpace("n13", 126,  494, {NodeColor::Red});
    addSpace("n14", 281,  453, {NodeColor::Red});
    addSpace("n15", 430,  391, {NodeColor::Red});

    addSpace("n16", 209,  677, {NodeColor::GRAY, NodeColor::Red});
    addSpace("n17", 286,  796, {NodeColor::GRAY});                  
    addSpace("n18", 429,  762, {NodeColor::GRAY});
    addSpace("n19", 574,  786, {NodeColor::GRAY});
    addSpace("n20", 732,  804, {NodeColor::GRAY});

    addSpace("n21", 668,  669, {NodeColor::GREEN, NodeColor::GRAY}); 
    addSpace("n22", 846,  701, {NodeColor::GRAY});
    addSpace("n23", 1024, 712, {NodeColor::GRAY});
    addSpace("n24", 1148, 794, {NodeColor::GRAY});
    addSpace("n25", 1253, 688, {NodeColor::GRAY, NodeColor::PURPLE});

    addSpace("n26", 1233, 502, {NodeColor::PURPLE});
    addSpace("n27", 1134, 393, {NodeColor::PURPLE});
    addSpace("n28", 1068, 530, {NodeColor::PURPLE});
    addSpace("n29", 923,  493, {NodeColor::GREEN, NodeColor::PURPLE}); 

    addSpace("n30", 746,  421, {NodeColor::GREEN});        
    addSpace("n31", 616,  504, {NodeColor::GREEN});
    addSpace("n32", 431,  599, {NodeColor::Red, NodeColor::GREEN}); 

    
    addEdge("n1", "n2"); addEdge("n2", "n1");
    addEdge("n1", "n3"); addEdge("n3", "n1");
    addEdge("n2", "n5"); addEdge("n5", "n2");
    addEdge("n3", "n4"); addEdge("n4", "n3");
    addEdge("n4", "n5"); addEdge("n5", "n4");
    addEdge("n4", "n15"); addEdge("n15", "n4"); 
    addEdge("n3", "n13"); addEdge("n13", "n3"); 

    addEdge("n5", "n6"); addEdge("n6", "n5");
    addEdge("n6", "n7"); addEdge("n7", "n6");
    addEdge("n6", "n8"); addEdge("n8", "n6");
    addEdge("n7", "n8"); addEdge("n8", "n7");
    addEdge("n7", "n9"); addEdge("n9", "n7");
    addEdge("n8", "n9"); addEdge("n9", "n8");

    addEdge("n8", "n10"); addEdge("n10", "n8");
    addEdge("n9", "n11"); addEdge("n11", "n9");
    addEdge("n10", "n11"); addEdge("n11", "n10");
    addEdge("n10", "n12"); addEdge("n12", "n10");
    addEdge("n11", "n12"); addEdge("n12", "n11");

    addEdge("n13", "n14"); addEdge("n14", "n13");
    addEdge("n14", "n15"); addEdge("n15", "n14");
    addEdge("n13", "n16"); addEdge("n16", "n13");
    addEdge("n14", "n16"); addEdge("n16", "n14");
    addEdge("n15", "n32"); addEdge("n32", "n15"); 

    addEdge("n16", "n17"); addEdge("n17", "n16");
    addEdge("n17", "n18"); addEdge("n18", "n17");
    addEdge("n18", "n19"); addEdge("n19", "n18");
    addEdge("n19", "n20"); addEdge("n20", "n19");
    addEdge("n20", "n21"); addEdge("n21", "n20");
    addEdge("n21", "n22"); addEdge("n22", "n21");
    addEdge("n22", "n23"); addEdge("n23", "n22");
    addEdge("n23", "n24"); addEdge("n24", "n23");
    addEdge("n24", "n25"); addEdge("n25", "n24");

    addEdge("n7", "n30"); addEdge("n30", "n7");   
    addEdge("n30", "n31"); addEdge("n31", "n30");
    addEdge("n31", "n32"); addEdge("n32", "n31");
    addEdge("n31", "n16"); addEdge("n16", "n31"); 
    addEdge("n32", "n21"); addEdge("n21", "n32"); 
    addEdge("n30", "n29"); addEdge("n29", "n30"); 

    addEdge("n9", "n29"); addEdge("n29", "n9");  
    addEdge("n29", "n28"); addEdge("n28", "n29");
    addEdge("n28", "n27"); addEdge("n27", "n28");
    addEdge("n28", "n26"); addEdge("n26", "n28");
    addEdge("n27", "n26"); addEdge("n26", "n27");
    addEdge("n27", "n11"); addEdge("n11", "n27"); 
    addEdge("n26", "n12"); addEdge("n12", "n26"); 
    addEdge("n26", "n25"); addEdge("n25", "n26"); 
    addEdge("n29", "n22"); addEdge("n22", "n29"); 

   
    addTeleport("n1", "n17");  addTeleport("n1", "n30");  
    addTeleport("n1", "n12");  addTeleport("n17", "n30");  
    addTeleport("n17", "n1");  addTeleport("n17", "n12");  
    addTeleport("n30", "n1");  addTeleport("n30", "n12");  
    addTeleport("n30", "n17"); addTeleport("n12", "n30");  
    addTeleport("n12", "n1");  addTeleport("n12", "n17");  
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
    return x >= 0 && x < 55 && y >= 0 && y < 16;
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
                case NodeColor::Red: zoneName = "Red"; break;
                case NodeColor::BLUE: zoneName = "BLUE"; break;
                case NodeColor::YELLOW: zoneName = "YELLOW"; break;
                case NodeColor::GREEN: zoneName = "GREEN"; break;
                case NodeColor::PURPLE: zoneName = "PURPLE"; break;
                case NodeColor::BROWN: zoneName = "BROWN"; break;
                case NodeColor::GRAY: zoneName = "GRAY"; break;
                default: zoneName = "NONE"; break;
            }
            result.push_back(zoneName);
        }
    }
    return result;
}

vector<pair<int, int>> Board::getSpacesInZone(const string& zoneName) const {
    NodeColor color = NodeColor::NONE;
    if (zoneName == "Red") color = NodeColor::Red;
    else if (zoneName == "BLUE") color = NodeColor::BLUE;
    else if (zoneName == "YELLOW") color = NodeColor::YELLOW;
    else if (zoneName == "GREEN") color = NodeColor::GREEN;
    else if (zoneName == "PURPLE") color = NodeColor::PURPLE;
    else if (zoneName == "BROWN") color = NodeColor::BROWN;
    else if (zoneName == "GRAY") color = NodeColor::GRAY;
    
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
    if (zoneName == "Red") color = NodeColor::Red;
    else if (zoneName == "BLUE") color = NodeColor::BLUE;
    else if (zoneName == "YELLOW") color = NodeColor::YELLOW;
    else if (zoneName == "GREEN") color = NodeColor::GREEN;
    else if (zoneName == "PURPLE") color = NodeColor::PURPLE;
    else if (zoneName == "BROWN") color = NodeColor::BROWN;
    else if (zoneName == "GRAY") color = NodeColor::GRAY;
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