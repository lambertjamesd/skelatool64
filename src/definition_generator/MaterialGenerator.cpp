#include "MaterialGenerator.h"

MaterialGenerator::MaterialGenerator(const DisplayListSettings& settings): mSettings(settings) {}


bool MaterialGenerator::ShouldIncludeNode(aiNode* node) {
    return false;
}

void MaterialGenerator::GenerateDefinitions(const aiScene* scene, CFileDefinition& fileDefinition) {
    std::set<std::shared_ptr<TextureDefinition>> textures;

    for (auto& entry : mSettings.mMaterials) {
        for (int i = 0; i < 8; ++i) {
            if (entry.second.mState.tiles[i].texture) {
                textures.insert(entry.second.mState.tiles[i].texture);
            }
        }
    }

    for (auto& texture : textures) {
        fileDefinition.AddDefinition(std::move(texture->GenerateDefinition(fileDefinition.GetUniqueName(texture->Name()), "_mat")));
    }

    for (auto& entry : mSettings.mMaterials) {
        std::string name = fileDefinition.GetUniqueName(entry.first);

        DisplayList dl(name);
        entry.second.Write(fileDefinition, mSettings.mDefaultMaterialState, dl.GetDataChunk());
        fileDefinition.AddDefinition(std::move(dl.Generate("_mat")));
    }
}