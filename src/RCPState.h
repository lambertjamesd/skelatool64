#ifndef _RCP_STATE_H
#define _RCP_STATE_H

struct VertexData {
    VertexData();
    VertexData(int vertexBuffer, int vertexIndex, int matrixIndex);

    int mVertexBuffer;
    int mVertexIndex;
    int mMatrixIndex;
    
    const bool operator==(const VertexData& other);
};

#define MAX_VERTEX_CACHE_SIZE   32

class RCPState {
public:
    RCPState(int maxVertexCount);
    void AssignSlots(VertexData* newVertices, unsigned int* slotIndex, unsigned int vertexCount);
    const unsigned int GetMaxVertices();
private:
    unsigned int mMaxVertices;
    VertexData mVertices[MAX_VERTEX_CACHE_SIZE];
};

#endif