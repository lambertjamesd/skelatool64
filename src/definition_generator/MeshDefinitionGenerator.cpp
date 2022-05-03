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

void MeshDefinitionGenerator::AppendRenderChunks(const aiScene* scene, aiNode* node, CFileDefinition& fileDefinition, DisplayListSettings& settings, std::vector<RenderChunk>& renderChunks) {
    for (unsigned meshIndex = 0; meshIndex < node->mNumMeshes; ++meshIndex) {
        ExtendedMesh* mesh = fileDefinition.GetExtendedMesh(scene->mMeshes[node->mMeshes[meshIndex]]);

        mesh->Transform(node->mTransformation);

        std::string materialName = scene->mMaterials[mesh->mMesh->mMaterialIndex]->GetName().C_Str();

        auto material = settings.mMaterials.find(materialName);

        Material* materialPtr = NULL;

        if (material != settings.mMaterials.end()) {
            materialPtr = material->second.get();
        }

        renderChunks.push_back(RenderChunk(
            std::pair<Bone*, Bone*>(NULL, NULL),
            mesh,
            materialPtr
        ));
    }
}

void MeshDefinitionGenerator::GenerateDefinitions(const aiScene* scene, CFileDefinition& fileDefinition) {
    std::vector<RenderChunk> renderChunks;

    for (auto node = mIncludedNodes.begin(); node != mIncludedNodes.end(); ++node) {
        AppendRenderChunks(scene, *node, fileDefinition, mSettings, renderChunks);
    }

    generateMesh(scene, fileDefinition, renderChunks, mSettings, "_geo");
}