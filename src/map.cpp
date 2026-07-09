#include "map.hpp"
#include <algorithm>
using namespace std;

GraphBoard::GraphBoard() { 
    addSpace("n1", 2, 7, {NodeColor::RED});
    addSpace("n2", 6, 7, {NodeColor::NONE});
    addSpace("n10", 10, 7, {NodeColor::YELLOW, NodeColor::GREEN});
    addSpace("n16", 18, 7, {NodeColor::NONE});
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
    addSpace("n17", 22, 7, {NodeColor::NONE});

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

void GraphBoard::addSpace(const string& name, int tuiX, int tuiY, const vector<NodeColor>& colors) {
    tuiCoordinates[name] = {tuiX, tuiY};
    for (const auto& color : colors) {
        zoneMap[color].push_back(name);
    }
    if (adjacencyList.find(name) == adjacencyList.end()) {
        adjacencyList[name] = vector<string>();
    }
}

void GraphBoard::addEdge(const string& spaceA, const string& spaceB) {
    adjacencyList[spaceA].push_back(spaceB);
    adjacencyList[spaceB].push_back(spaceA);
}

bool GraphBoard::isConnected(const string& spaceA, const string& spaceB) const {
    auto it = adjacencyList.find(spaceA);
    if (it != adjacencyList.end()) {
        return find(it->second.begin(), it->second.end(), spaceB) != it->second.end();
    }
    return false;
}

vector<string> GraphBoard::getNeighbors(const string& spaceName) const {
    auto it = adjacencyList.find(spaceName);
    return (it != adjacencyList.end()) ? it->second : vector<string>{};
}

pair<int, int> GraphBoard::getCoordinates(const string& spaceName) const {
    auto it = tuiCoordinates.find(spaceName);
    return (it != tuiCoordinates.end()) ? it->second : pair<int, int>{-1, -1};
}

void GraphBoard::addTeleport(const string& from, const string& to) {
    teleportpairs[from] = to;
    teleportpairs[to] = from; 
}

string GraphBoard::getTeleportDestination(const string& spaceName) const {
    auto it = teleportpairs.find(spaceName);
    if (it != teleportpairs.end()) {
        return it->second;
    }
    return spaceName; 
}

bool GraphBoard::isTeleport(const string& spaceName) const {
    return teleportpairs.find(spaceName) != teleportpairs.end();
}