#ifndef _PATHFINDING_H
#define _PATHFINDING_H

#include <assimp/scene.h>
#include <vector>
#include <map>
#include <set>

class Pathfinding {
public:
    std::vector<aiVector3D> mPathingNodes;
    // each element specifices a connection between to of the nodes in mPathingNodes as specified by its index
    std::set<std::pair<unsigned, unsigned>> mNodeConnections;
};

struct basesDistance{
    int fromBase;
    int toBase;
    float distance;
};

class PathfindingDefinition {
public:
    std::vector<aiVector3D> mNodePositions;
    std::vector<int> baseNodes;
    std::vector<basesDistance> baseDistances;
    // to be interpreted as a 2D array with mNodePositions.size() * mNodePositions.size() entries
    // If an AI agent wanted to go from node an index n to node at index m it would do so by looking up
    // the entry in mNextNode[from * mNodePositions.size() + to] and the result would be another index
    // into mNodePositions that specifies the index of mNodePositions the AI agent should goto to eventually reach
    // the destination node. Each time an AI agent reaches an intermediate node it should update the from node
    // and lookup in mNextNode to get to the next node 
    std::vector<int> mNextNode;
    std::vector<float> mDistToNode;
};

class NavPath {
    public:
    std::vector<int> points;
    float len = 0.f;
    bool reachesEndPoint = false;
};

//will return integers that appear to be in the given pairs set as the 'second' value
//std::vector<int> getAdjacentIndices(const std::set<std::pair<unsigned, unsigned>>& nodeConnections, const unsigned& adjTo); 

void buildPathingFromMesh(aiMesh* mesh, Pathfinding& result, const aiMatrix4x4& transform);
void buildPathfindingDefinition(const Pathfinding& from, PathfindingDefinition& result, const std::vector<aiVector3D>& basePositions);
unsigned getNextPathPointIndex(unsigned from, unsigned to, const std::vector<aiVector3D>& nodePositions, const std::set<std::pair<unsigned, unsigned>>& nodeConnections, std::set<int>& nodesToIgnore, float& distance);

#endif