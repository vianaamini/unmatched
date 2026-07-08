#include "board.hpp"
#include <algorithm>
using namespace std;

Board::Board() {
    addSpace("n1",  15, 1,  {NodeColor::RED});
    addSpace("n2",  21, 1,  {NodeColor::RED, NodeColor::BLUE});
    addSpace("n3",  27, 1,  {NodeColor::BLUE});
    addSpace("n4",  33, 1,  {NodeColor::BLUE});
    addSpace("n5",  9,  3,  {NodeColor::RED});
    addSpace("n6",  18, 3,  {NodeColor::RED, NodeColor::PURPLE});
    addSpace("n7",  24, 3,  {NodeColor::BLUE, NodeColor::PURPLE});
    addSpace("n8",  36, 3,  {NodeColor::BLUE});

    addSpace("n9",  3,  5,  {NodeColor::GREEN});
    addSpace("n10", 9,  5,  {NodeColor::GREEN, NodeColor::RED});
    addSpace("n11", 15, 5,  {NodeColor::RED, NodeColor::BROWN});
    addSpace("n12", 21, 5,  {NodeColor::PURPLE, NodeColor::BROWN});
    addSpace("n13", 27, 5,  {NodeColor::YELLOW, NodeColor::BROWN});
    addSpace("n14", 33, 5,  {NodeColor::YELLOW, NodeColor::BLUE});
    addSpace("n15", 39, 5,  {NodeColor::YELLOW});

    addSpace("n16", 3,  7,  {NodeColor::GREEN});
    addSpace("n17", 9,  7,  {NodeColor::GREEN, NodeColor::PURPLE});
    addSpace("n18", 15, 7,  {NodeColor::PURPLE});
    addSpace("n19", 21, 7,  {NodeColor::BROWN});
    addSpace("n20", 27, 7,  {NodeColor::YELLOW, NodeColor::BROWN});
    addSpace("n21", 33, 7,  {NodeColor::YELLOW, NodeColor::BLUE});
    addSpace("n22", 39, 7,  {NodeColor::YELLOW});

    addSpace("n23", 9,  9,  {NodeColor::GREEN});
    addSpace("n24", 15, 9,  {NodeColor::GREEN, NodeColor::BROWN});
    addSpace("n25", 21, 9,  {NodeColor::PURPLE});
    addSpace("n26", 27, 9,  {NodeColor::YELLOW});
    addSpace("n27", 33, 9,  {NodeColor::YELLOW});
    addSpace("n28", 12, 11, {NodeColor::GREEN});
    addSpace("n29", 18, 11, {NodeColor::GREEN, NodeColor::YELLOW});
    addSpace("n30", 24, 11, {NodeColor::YELLOW});
    addSpace("n31", 30, 11, {NodeColor::YELLOW});
    addSpace("n32", 36, 11, {NodeColor::YELLOW});

    addEdge("n1", "n2"); addEdge("n2", "n3"); addEdge("n3", "n4");
    addEdge("n1", "n5"); addEdge("n2", "n6"); addEdge("n3", "n7"); addEdge("n4", "n8");
    addEdge("n5", "n6"); addEdge("n6", "n7"); addEdge("n7", "n8");
    addEdge("n5", "n9"); addEdge("n6", "n11"); addEdge("n7", "n13"); addEdge("n8", "n15");
    addEdge("n9", "n10"); addEdge("n10", "n11"); addEdge("n11", "n12");
    addEdge("n12", "n13"); addEdge("n13", "n14"); addEdge("n14", "n15");
    addEdge("n9", "n16"); addEdge("n10", "n17"); addEdge("n12", "n19");
    addEdge("n14", "n21"); addEdge("n15", "n22");
    addEdge("n16", "n17"); addEdge("n17", "n18"); addEdge("n18", "n19");
    addEdge("n19", "n20"); addEdge("n20", "n21"); addEdge("n21", "n22");
    addEdge("n17", "n23"); addEdge("n18", "n24"); addEdge("n19", "n25");
    addEdge("n20", "n26"); addEdge("n21", "n27");
    addEdge("n23", "n24"); addEdge("n24", "n25"); addEdge("n25", "n26"); addEdge("n26", "n27");
    addEdge("n23", "n28"); addEdge("n24", "n29"); addEdge("n25", "n30");
    addEdge("n26", "n31"); addEdge("n27", "n32");
    addEdge("n28", "n29"); addEdge("n29", "n30"); addEdge("n30", "n31"); addEdge("n31", "n32");
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
} // in tui we have 21 nodes but in map we have 32 so i wrote 32 
// // this class might need some changes we should check it again but the logic is correct.