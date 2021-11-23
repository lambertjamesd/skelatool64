#include "Pathfinding.h"
#include <math.h>

bool shouldUsePathingPoint(float minY, const aiVector3D& point) {
    return std::abs(minY - point.y) < 0.5f;
}

std::vector<int> getAdjacentIndices(const std::set<std::pair<unsigned, unsigned>>& nodeConnections, const unsigned& adjTo){
    std::vector<int> out;
    for(auto node : nodeConnections){
        if(node.first == adjTo) out.push_back(node.second);
    }
    return out;
}

int getShortestPath(const std::vector<NavPath*>& allPaths, float& outDist){
    if(allPaths.size() == 0) return -1;
    else{
        float minDist = allPaths[0]->len;
        int minInd = 0;
        for(unsigned i = 1; i < allPaths.size(); ++i){
            if(allPaths[i]->reachesEndPoint){
               if(allPaths[i]->len < minDist){
                   minInd = i;
                   minDist = allPaths[i]->len;
               }
            }
        }
        outDist = minDist;
        return minInd;
    }
}

void getPossiblePaths(unsigned from, unsigned to, const std::vector<aiVector3D>& nodePositions, const std::set<std::pair<unsigned, unsigned>>& nodeConnections, std::vector<NavPath*>& outPaths, const int& initialPath = -1){

    if(from == to){
        if(initialPath != -1){
            outPaths[initialPath]->reachesEndPoint = true;
        }
        return;
    }

    std::vector<int> branches = getAdjacentIndices(nodeConnections, from);
    for(unsigned i = 0; i < branches.size(); ++i){
        int adjInd = branches[i];

        float distance = (nodePositions[adjInd] - nodePositions[from]).Length();

        //when the initialPath value is equal to 1, it means that we have just started to observe paths, and thus don't have any branches in the outPaths array yet.
        if(initialPath == -1){
            NavPath* newPath = new NavPath();
            newPath->points.push_back(from);
            newPath->points.push_back(adjInd);
            newPath->len = distance;
            outPaths.push_back(newPath);
            getPossiblePaths(adjInd, to, nodePositions, nodeConnections, outPaths, (outPaths.size()-1));
        } 
        else{
            //the first branch will be just a continuation of the given path
            if(i == 0){
                if(!std::count(outPaths[initialPath]->points.begin(), outPaths[initialPath]->points.end(), adjInd)){

                    outPaths[initialPath]->points.push_back(adjInd);
                    outPaths[initialPath]->len += distance;
                    
                    getPossiblePaths(adjInd, to, nodePositions, nodeConnections, outPaths, initialPath);
                }
            }
            //while all further branches will be observed as completely separate paths
            else{
                if(!std::count(outPaths[initialPath]->points.begin(), outPaths[initialPath]->points.end(), adjInd)){ //making sure that we're not moving in a circle
                    //first, we duplicate the existing branch
                    NavPath* newPath = new NavPath();
                    newPath->points = outPaths[initialPath]->points;
                    newPath->len = outPaths[initialPath]->len;

                    //and then append the data that we've just collected about this new branch
                    newPath->points.push_back(adjInd);
                    newPath->len += distance;
                    outPaths.push_back(newPath);

                    getPossiblePaths(adjInd, to, nodePositions, nodeConnections, outPaths, (outPaths.size()-1));
                }
            }
        }
        
    }
    
}

void buildPathingFromMesh(aiMesh* mesh, Pathfinding& result, const aiMatrix4x4& transform) {
    std::vector<aiVector3D> transformed;

    for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
        transformed.push_back(transform * mesh->mVertices[i]);
    }

    float minY = transformed[0].y;
    for (unsigned i = 1; i < mesh->mNumVertices; ++i) {
        minY = std::min(minY, transformed[i].y);
    }

    std::map<unsigned, unsigned> mIndexMapping;

    for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
        if (shouldUsePathingPoint(minY, transformed[i])) {
            bool didFindDuplicate = false;

            for (unsigned dupeCheck = 0; dupeCheck < i; ++dupeCheck) {
                if (!shouldUsePathingPoint(minY, transformed[dupeCheck])) {
                    continue;
                }

                float dist = (transformed[i] - transformed[dupeCheck]).SquareLength();

                if (dist < 0.001f) {
                    mIndexMapping[i] = mIndexMapping[dupeCheck];
                    didFindDuplicate = true;
                    break;
                }
            }
            
            if (!didFindDuplicate) {
                mIndexMapping[i] = result.mPathingNodes.size();
                result.mPathingNodes.push_back(transformed[i]);
            }
        }
    }

    for (unsigned faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
        aiFace* face = &mesh->mFaces[faceIndex];

        for (unsigned vertexIndex = 0; vertexIndex < face->mNumIndices; ++vertexIndex) {
            unsigned curr = face->mIndices[vertexIndex];
            unsigned next = face->mIndices[(vertexIndex + 1) % face->mNumIndices];

            auto currMapping = mIndexMapping.find(curr);
            auto nextMapping = mIndexMapping.find(next);

            if (currMapping != mIndexMapping.end() && nextMapping != mIndexMapping.end()) {
                result.mNodeConnections.insert(std::make_pair(currMapping->second, nextMapping->second));
            }
        }
    }
}

unsigned getClosestLocationInd(const aiVector3D& closestTo, const std::vector<aiVector3D>& allPos){
    if(allPos.size() == 0) return -1;
    else if(allPos.size() == 1) return 0;
    else{
        unsigned result = 0;
        float minDist = (closestTo - allPos[0]).Length();
        for(unsigned i = 1; i  < allPos.size(); ++i){
            float thisDist = (closestTo - allPos[i]).Length();
            if(thisDist < minDist){
                minDist = thisDist;
                result = i;
            }
        }
        return result;
    }
}

void buildPathToNode(const Pathfinding& pathfinding, unsigned baseIndex, std::vector<int>& nextBase, std::vector<float>& distanceToBase) {
    for (unsigned i = 0; i < pathfinding.mPathingNodes.size(); ++i) {
        nextBase.push_back(-1);
        distanceToBase.push_back(std::numeric_limits<float>::max());
    }


    nextBase[baseIndex] = baseIndex;
    distanceToBase[baseIndex] = 0.0f;
    bool hasMore = true;

    while (hasMore) {
        hasMore = false;

        for (auto connection : pathfinding.mNodeConnections) {
            float distanceBetweenConnections = std::sqrt(
                (pathfinding.mPathingNodes[connection.first] - pathfinding.mPathingNodes[connection.second]).SquareLength()
            );

            if (nextBase[connection.first] != -1) {
                float newDistance = distanceToBase[connection.first] + distanceBetweenConnections;

                if (nextBase[connection.second] == -1 || newDistance < distanceToBase[connection.second]) {
                    nextBase[connection.second] = connection.first;
                    distanceToBase[connection.second] = newDistance;
                    hasMore = true;
                }
            }

            if (nextBase[connection.second] != -1) {
                float newDistance = distanceToBase[connection.second] + distanceBetweenConnections;

                if (nextBase[connection.first] == -1 || newDistance < distanceToBase[connection.first]) {
                    nextBase[connection.first] = connection.second;
                    distanceToBase[connection.first] = newDistance;
                    hasMore = true;
                }
            }
        }
    }
}

void buildPathfindingDefinition(const Pathfinding& from, PathfindingDefinition& result, const std::vector<aiVector3D>& basePositions) {

    unsigned int numPoints = from.mPathingNodes.size();

    for(unsigned i = 0; i < numPoints; ++i){
        result.mNodePositions.push_back(from.mPathingNodes[i]);
    }

    for(unsigned baseI = 0; baseI < basePositions.size(); ++baseI)
        result.baseNodes.push_back(getClosestLocationInd(basePositions[baseI], result.mNodePositions));
    
    result.mNextNode.reserve(numPoints * numPoints);
    result.mDistToNode.reserve(numPoints * numPoints);

    for (unsigned toNode = 0; toNode < numPoints; ++toNode) {
        std::vector<int> moveToNode;
        std::vector<float> distancetoNode;
        buildPathToNode(from, toNode, moveToNode, distancetoNode);

        for (unsigned fromNode = 0; fromNode < numPoints; ++fromNode) {
            result.mNextNode[fromNode * numPoints + toNode] = moveToNode[fromNode];
            result.mDistToNode[fromNode * numPoints + toNode] = distancetoNode[fromNode];
        }
    }
}