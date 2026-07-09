#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
using namespace std;

enum class NodeColor {
    RED,
    BLUE,
    YELLOW,
    GREEN,
    PURPLE,
    BROWN,
    NONE
};

class GraphBoard {
private:
    unordered_map<string, vector<string>> adjacencyList;
    unordered_map<NodeColor, vector<string>> zoneMap;
    unordered_map<string, pair<int, int>> tuiCoordinates;
    unordered_map<string, string> teleportpairs;

public:
    GraphBoard();

    void addSpace(const string& name, int tuiX, int tuiY, const vector<NodeColor>& colors);
    void addEdge(const string& spaceA, const string& spaceB);

    bool isConnected(const string& spaceA, const string& spaceB) const;
    vector<string> getNeighbors(const string& spaceName) const;
    pair<int, int> getCoordinates(const string& spaceName) const;

    void addTeleport(const string& from, const string& to);
    string getTeleportDestination(const string& spaceName) const;
    bool isTeleport(const string& spaceName) const;
    
    const unordered_map<string, vector<string>>& getGraph() const { return adjacencyList; }
}; // this class might need some changes we should check it again but the logic is correct.