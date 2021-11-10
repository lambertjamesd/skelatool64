#include "Pathfinding.h"
#include <math.h>

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
        if (std::abs(minY - transformed[i].y < 0.5f)) {
            mIndexMapping[i] = result.mPathingNodes.size();
            result.mPathingNodes.push_back(transformed[i]);
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

void buildPathfindingDefinition(const Pathfinding& from, PathfindingDefinition& result) {
    // TODO
}