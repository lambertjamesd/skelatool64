#ifndef _RCP_STATE_H
#define _RCP_STATE_H

#include <vector>

#include "BoneHierarchy.h"
#include "DisplayList.h"
#include "ErrorCode.h"

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
    RCPState(unsigned int maxVertexCount, unsigned int maxMatrixDepth, bool canPopMultiple);
    ErrorCode TraverseToBone(Bone* bone, DisplayList& output);
    void AssignSlots(VertexData* newVertices, unsigned int* slotIndex, unsigned int vertexCount);
    const unsigned int GetMaxVertices();
private:
    unsigned int mMaxVertices;
    unsigned int mMaxMatrixDepth;
    bool mCanPopMultiple;
    VertexData mVertices[MAX_VERTEX_CACHE_SIZE];
    std::vector<Bone*> mBoneMatrixStack;
};

#endif