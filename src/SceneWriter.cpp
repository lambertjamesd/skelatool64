#include "SceneWriter.h"

#include <fstream>
#include <sstream>

#include "./DisplayList.h"
#include "./DisplayListGenerator.h"

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

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        // todo apply material
        int vertexBuffer = fileDefinition.GetVertexBuffer(scene->mMeshes[i], VertexType::PosUVColor);
        generateGeometry(scene->mMeshes[i], rcpState, vertexBuffer, displayList, settings.mHasTri2);
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