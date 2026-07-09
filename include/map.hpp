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

class Board {
private:
    
    unordered_map<string, vector<string>> adjacencyList; // to have neighbors
    unordered_map<NodeColor, vector<string>> zoneMap; // to have nodes which have same colors
    unordered_map<string, pair<int, int>> tuiCoordinates; // to have each node place 

public:
    Board();

    void addSpace(const string& name, int tuiX, int tuiY, const vector<NodeColor>& colors);
    void addEdge(const string& spaceA, const string& spaceB);

    bool isConnected(const string& spaceA, const string& spaceB) const;
    vector<string> getNeighbors(const string& spaceName) const;
    pair<int, int> getCoordinates(const string& spaceName) const;

    const unordered_map<string, vector<string>>& getGraph() const { return adjacencyList; }
}; // this class might need some changes we should check it again but the logic is correct.