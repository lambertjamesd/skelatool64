#include "MeshWriter.h"

#include "RCPState.h"
#include "DisplayListGenerator.h"

std::string generateMesh(CFileDefinition& fileDefinition, std::vector<RenderChunk>& renderChunks, DisplayListSettings& settings, std::ostream& output) {
    RCPState rcpState(settings.mVertexCacheSize, settings.mMaxMatrixDepth, settings.mCanPopMultipleMatrices);
    DisplayList displayList(fileDefinition.GetUniqueName("model_gfx"));
    for (auto chunk = renderChunks.begin(); chunk != renderChunks.end(); ++chunk) {
        // todo apply material
        int vertexBuffer = fileDefinition.GetVertexBuffer(chunk->mMesh, chunk->mVertexType);
        generateGeometry(*chunk, rcpState, vertexBuffer, displayList, settings.mHasTri2);
    }
    rcpState.TraverseToBone(nullptr, displayList);
    fileDefinition.GenerateVertexBuffers(output, settings.mScale, settings.mRotateModel);
    displayList.Generate(fileDefinition, output);
    return displayList.GetName();
}