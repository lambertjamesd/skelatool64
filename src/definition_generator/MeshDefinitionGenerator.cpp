#include "MeshDefinitionGenerator.h"

#include "../RenderChunk.h"
#include "../MeshWriter.h"

MeshDefinitionGenerator::MeshDefinitionGenerator(const DisplayListSettings& settings) :
    DefinitionGenerator(),
    mSettings(settings) {

    }

bool MeshDefinitionGenerator::ShouldIncludeNode(aiNode* node) {
    return node->mName.C_Str()[0] != '@' && node->mNumMeshes > 0;
}

void MeshDefinitionGenerator::GenerateDefinitions(const aiScene* scene, CFileDefinition& fileDefinition) {
    std::vector<RenderChunk> renderChunks;

    std::vector<std::unique_ptr<ExtendedMesh>> extendedMeshes;

    BoneHierarchy bones;

    for (auto node = mIncludedNodes.begin(); node != mIncludedNodes.end(); ++node) {

        for (unsigned meshIndex = 0; meshIndex < (*node)->mNumMeshes; ++meshIndex) {
            extendedMeshes.push_back(std::unique_ptr<ExtendedMesh>(new ExtendedMesh(
                scene->mMeshes[(*node)->mMeshes[meshIndex]], 
                bones))
            );

            ExtendedMesh* lastMesh = (*extendedMeshes.rbegin()).get();

            lastMesh->Transform((*node)->mTransformation);

            renderChunks.push_back(RenderChunk(
                std::pair<Bone*, Bone*>(NULL, NULL),
                lastMesh,
                VertexType::PosUVNormal
            ));
        }
    }

    generateMesh(scene, fileDefinition, renderChunks, mSettings, "_geo");
}