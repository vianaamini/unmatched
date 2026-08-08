#ifndef MAP_HPP
#define MAP_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>

using namespace std;

enum class NodeColor {
    BLUE, Red ,YELLOW, GREEN, PURPLE, BROWN, GRAY ,NONE
};

class Board {
private:
    unordered_map<string, vector<string>> adjacencyList;
    unordered_map<NodeColor, vector<string>> zoneMap;
    unordered_map<string, pair<int, int>> tuiCoordinates;
    unordered_map<string, string> teleportPairs;
    unordered_set<string> teleportSpaces;
    unordered_set<string> obstacleSpaces;
    int width, height;

public:
    Board(int width = 10, int height = 10);

    bool isValid(int x, int y) const;
    bool isWalkable(int x, int y) const;
    bool hasObstacle(int x, int y) const;
    void setWalkable(int x, int y, bool walkable);
    void setObstacle(int x, int y, bool hasObstacle);
    
    // برای گره‌ها (فقط با ID)
    bool isAdjacent(int node1, int node2) const;
    vector<int> getNeighborIds(int node) const;
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    vector<pair<int, int>> getAllSpaces() const;
    vector<pair<int, int>> getWalkableSpaces() const;
    void addZone(const string& zoneName, const vector<pair<int, int>>& spaces);
    void addSpaceToZone(int x, int y, const string& zoneName);
    vector<string> getZonesAt(int x, int y) const;
    vector<pair<int, int>> getSpacesInZone(const string& zoneName) const;

    void addSpace(const string& name, int tuiX, int tuiY, const vector<NodeColor>& colors);
    void addEdge(const string& spaceA, const string& spaceB);
    bool isConnected(const string& spaceA, const string& spaceB) const;
    vector<string> getNeighbors(const string& spaceName) const;
    pair<int, int> getCoordinates(const string& spaceName) const;
    bool hasSpace(const string& spaceName) const;
    vector<string> getAllSpaceIds() const;

    void addTeleport(const string& from, const string& to);
    string getTeleportDestination(const string& spaceName) const;
    bool isTeleport(const string& spaceName) const;

    int getNodeId(const string& nodeName) const;
    string getNodeName(int id) const;

    const unordered_map<string, vector<string>>& getGraph() const { return adjacencyList; }
    NodeColor getNodeColorByName(const string& nodeName) const;
};

#endif