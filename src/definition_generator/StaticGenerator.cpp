#include "StaticGenerator.h"

#include "../StringUtils.h"
#include "../MeshWriter.h"

StaticGenerator::StaticGenerator(const DisplayListSettings& settings) {

}

bool StaticGenerator::ShouldIncludeNode(aiNode* node) {
    return StartsWith(node->mName.C_Str(), "@static");
}

void StaticGenerator::GenerateDefinitions(const aiScene* scene, CFileDefinition& fileDefinition) {
    for (auto node = mIncludedNodes.begin(); node != mIncludedNodes.end(); ++node) {
        std::vector<RenderChunk> renderChunks;

        

        generateMesh(scene, fileDefinition, renderChunks, mSettings, "_geo");
    }

}