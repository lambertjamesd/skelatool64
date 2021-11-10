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

class PathfindingDefinition {
public:
    std::vector<aiVector3D> mNodePositions;
    // to be interpreted as a 2D array with mNodePositions.size() * mNodePositions.size() entries
    // If an AI agent wanted to go from node an index n to node at index m it would do so by looking up
    // the entry in mNextNode[from * mNodePositions.size() + to] and the result would be another index
    // into mNodePositions that specifies the index of mNodePositions the AI agent should goto to eventually reach
    // the destination node. Each time an AI agent reaches an intermediate node it should update the from node
    // and lookup in mNextNode to get to the next node 
    std::vector<int> mNextNode;
};

void buildPathingFromMesh(aiMesh* mesh, Pathfinding& result, const aiMatrix4x4& transform);
void buildPathfindingDefinition(const Pathfinding& from, PathfindingDefinition& result);

#endif