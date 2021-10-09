#include "SceneWriter.h"

#include <fstream>
#include <sstream>

#include "./DisplayList.h"
#include "./DisplayListGenerator.h"
#include "./BoneHierarchy.h"
#include "./ExtendedMesh.h"
#include "./RenderChunk.h"

DisplayListSettings::DisplayListSettings():
    mPrefix(""),
    mVertexCacheSize(MAX_VERTEX_CACHE_SIZE),
    mHasTri2(true),
    mScale(256.0f),
    mMaxMatrixDepth(10),
    mCanPopMultipleMatrices(true) {

}

void generateMeshFromScene(const aiScene* scene, std::ostream& output, DisplayListSettings& settings) {
    RCPState rcpState(settings.mVertexCacheSize, settings.mMaxMatrixDepth, settings.mCanPopMultipleMatrices);
    CFileDefinition fileDefinition(settings.mPrefix);
    DisplayList displayList(fileDefinition.GetUniqueName("model_gfx"));
    BoneHierarchy bones;

    bones.SearchForBonesInScene(scene);

    std::vector<std::unique_ptr<ExtendedMesh>> extendedMeshes;

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        extendedMeshes.push_back(std::unique_ptr<ExtendedMesh>(new ExtendedMesh(scene->mMeshes[i], bones)));
    }

    std::vector<RenderChunk> renderChunks;

    extractChunks(extendedMeshes, renderChunks);
    orderChunks(renderChunks);

    for (auto chunk = renderChunks.begin(); chunk != renderChunks.end(); ++chunk) {
        // todo apply material
        int vertexBuffer = fileDefinition.GetVertexBuffer(chunk->mMesh->mMesh, VertexType::PosUVColor);
        generateGeometry(*chunk, rcpState, vertexBuffer, displayList, settings.mHasTri2);
    }

    fileDefinition.GenerateVertexBuffers(output, settings.mScale);

    displayList.Generate(fileDefinition, output);
}

void generateMeshFromSceneToFile(const aiScene* scene, std::string filename, DisplayListSettings& settings) {
    std::ofstream output;
    output.open(filename, std::ios_base::out | std::ios_base::trunc);
    generateMeshFromScene(scene, output, settings);
    output.close();
}