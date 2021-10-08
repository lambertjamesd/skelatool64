
#include "./RCPState.h"

VertexData::VertexData() :
    mVertexBuffer(-1),
    mVertexIndex(-1),
    mMatrixIndex(-1) {

}

VertexData::VertexData(int vertexBuffer, int vertexIndex, int matrixIndex) :
    mVertexBuffer(vertexBuffer),
    mVertexIndex(vertexIndex),
    mMatrixIndex(matrixIndex) {

}

const bool VertexData::operator==(const VertexData& other) {
    return mVertexBuffer == other.mVertexBuffer &&
        mVertexIndex == other.mVertexIndex &&
        mMatrixIndex == other.mMatrixIndex;
}

RCPState::RCPState(int maxVertexCount) :
    mMaxVertices(maxVertexCount) {

}

void RCPState::AssignSlots(VertexData* newVertices, unsigned int* slotIndex, unsigned int vertexCount) {
    bool usedSlots[MAX_VERTEX_CACHE_SIZE];
    bool assignedVertices[MAX_VERTEX_CACHE_SIZE];
    for (unsigned int i = 0; i < MAX_VERTEX_CACHE_SIZE; ++i) {
        usedSlots[i] = false;
        assignedVertices[i] = false;
    }

    for (unsigned int currentVertex = 0; currentVertex < mMaxVertices; ++currentVertex) {
        for (unsigned int newVertex = 0; newVertex < vertexCount; ++newVertex) {
            if (mVertices[currentVertex] == newVertices[newVertex]) {
                usedSlots[currentVertex] = true;
                assignedVertices[newVertex] = true;

                slotIndex[newVertex] = currentVertex;
            }
        }
    }

    unsigned int nextTarget = 0;
    unsigned int nextSource = 0;

    while (nextTarget < mMaxVertices || nextSource < vertexCount) {
        if (nextTarget < mMaxVertices && usedSlots[nextTarget]) {
            ++nextTarget;
        } else if (nextSource < vertexCount && assignedVertices[nextSource]) {
            ++nextSource;
        } else {
            slotIndex[nextSource] = nextTarget;
            ++nextTarget;
            ++nextSource;
        }
    }
}

const unsigned int RCPState::GetMaxVertices() {
    return mMaxVertices;
}