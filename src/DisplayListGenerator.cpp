#include <set>
#include <vector>
#include <algorithm>
#include <map>

#include "./DisplayListGenerator.h"

bool doesFaceFit(std::set<int>& indices, aiFace* face, unsigned int maxVertices) {
    unsigned int misses = 0;

    for (unsigned int i = 0; i < face->mNumIndices; ++i) {
        if (indices.find(face->mIndices[i]) == indices.end()) {
            ++misses;
        }
    }

    return indices.size() + misses <= maxVertices;
}

void flushVertices(std::set<int>& currentVertices, std::vector<aiFace*>& currentFaces, RCPState& state, int vertexBuffer, DisplayList& output, bool hasTri2) {
    std::vector<int> verticesAsVector(currentVertices.begin(), currentVertices.end());
    std::sort(verticesAsVector.begin(), verticesAsVector.end());

    VertexData vertexData[MAX_VERTEX_CACHE_SIZE];

    for (auto vertexIndex = verticesAsVector.begin(); vertexIndex != verticesAsVector.end(); ++vertexIndex) {
        vertexData[*vertexIndex] = VertexData(vertexBuffer, verticesAsVector[*vertexIndex], -1);
    }
    
    unsigned int cacheLocation[MAX_VERTEX_CACHE_SIZE];

    state.AssignSlots(vertexData, cacheLocation, verticesAsVector.size());
    int lastVertexIndex = -1;
    int lastCacheLocation = MAX_VERTEX_CACHE_SIZE;
    int vertexCount = 0;
    std::map<int, int> vertexMapping;

    for (unsigned int index = 0; index <= verticesAsVector.size(); ++index) {
        int vertexIndex;
        int cacheIndex;
        
        if (index < verticesAsVector.size()) {
            vertexIndex = cacheLocation[vertexIndex];
            cacheIndex = verticesAsVector[vertexIndex];
            vertexMapping[vertexIndex] = cacheIndex;
        }

        if (index == verticesAsVector.size() || vertexIndex != lastVertexIndex + 1 || cacheIndex != lastCacheLocation + 1) {
            output.AddCommand(std::unique_ptr<DisplayListCommand>(new VTXCommand(
                vertexCount + 1, 
                lastCacheLocation + 1 - vertexCount, 
                vertexBuffer, 
                lastVertexIndex + 1 - vertexCount
            )));

            vertexCount = 0;
        } else {
            ++vertexCount;
        }

        lastVertexIndex = vertexIndex;
        lastCacheLocation = cacheIndex;
    }

    for (unsigned int faceIndex = 0; faceIndex < currentFaces.size(); ++faceIndex) {
        if (hasTri2 && faceIndex + 1 < currentFaces.size()) {
            aiFace* currFace = currentFaces[faceIndex + 0];
            aiFace* nextFace = currentFaces[faceIndex + 1];

            output.AddCommand(std::unique_ptr<DisplayListCommand>(new TRI2Command(
                vertexMapping.at(currFace->mIndices[0]), vertexMapping.at(currFace->mIndices[1]), vertexMapping.at(currFace->mIndices[2]),
                vertexMapping.at(nextFace->mIndices[0]), vertexMapping.at(nextFace->mIndices[1]), vertexMapping.at(nextFace->mIndices[2])
            )));

            ++faceIndex;
        } else {
            aiFace* currFace = currentFaces[faceIndex + 0];

            output.AddCommand(std::unique_ptr<DisplayListCommand>(new TRI1Command(
                vertexMapping.at(currFace->mIndices[0]), vertexMapping.at(currFace->mIndices[1]), vertexMapping.at(currFace->mIndices[2])
            )));
        }
    }
}

void generateGeometry(aiMesh* mesh, RCPState& state, int vertexBuffer, DisplayList& output, bool hasTri2) {
    std::set<int> currentVertices;
    std::vector<aiFace*> currentFaces;

    for (unsigned int faceIndex = 0; faceIndex <= mesh->mNumFaces; ++faceIndex) {
        if (faceIndex == mesh->mNumFaces || !doesFaceFit(currentVertices, &mesh->mFaces[faceIndex], state.GetMaxVertices())) {
            flushVertices(currentVertices, currentFaces, state, vertexBuffer, output, hasTri2);

            currentVertices.clear();
            currentFaces.clear();

            if (faceIndex == mesh->mNumFaces) {
                break;
            }
        }

        for (unsigned int vertexIndex = 0; vertexIndex < mesh->mFaces[faceIndex].mNumIndices; ++vertexIndex) {
            currentVertices.insert(mesh->mFaces[faceIndex].mIndices[vertexIndex]);
        }

        currentFaces.push_back(&mesh->mFaces[faceIndex]);
    }
}