#include "MeshWriter.h"

#include <set>
#include <sstream>

#include "RCPState.h"
#include "DisplayListGenerator.h"
#include "StringUtils.h"

MaterialCollector::MaterialCollector(): mSceneCount(0) {}

void MaterialCollector::UseMaterial(const std::string& material, DisplayListSettings& settings) {
    auto materialDL = settings.mMaterials.find(material);

    if (materialDL == settings.mMaterials.end()) {
        return;
    }

    auto prevCount = mMaterialUseCount.find(material);

    if (prevCount == mMaterialUseCount.end()) {
        mMaterialUseCount[material] = 1;

        for (int i = 0; i < MAX_TILE_COUNT; ++i) {
            auto tile = &materialDL->second.mState.tiles[i];
            if (tile->isOn && tile->texture) {
                mUsedTextures.insert(tile->texture);
            }
        }
    } else {
        mMaterialUseCount[material] = prevCount->second + 1;
    }
}

void MaterialCollector::CollectMaterialResources(const aiScene* scene, std::vector<RenderChunk>& renderChunks, DisplayListSettings& settings) {
    for (auto chunk = renderChunks.begin(); chunk != renderChunks.end(); ++chunk) {
        std::string materialName = scene->mMaterials[chunk->mMesh->mMesh->mMaterialIndex]->GetName().C_Str();
        UseMaterial(materialName, settings);
    }
    ++mSceneCount;
}

void MaterialCollector::GenerateMaterials(DisplayListSettings& settings, CFileDefinition& fileDefinition, const std::string& fileSuffix) {
    std::vector<std::shared_ptr<MaterialResource>> resources(mUsedResources.begin(), mUsedResources.end());

    for (auto image : mUsedTextures) {
        fileDefinition.AddDefinition(std::move(image->GenerateDefinition(fileDefinition.GetUniqueName(image->Name()), fileSuffix)));
    }
    
    for (auto useCount = mMaterialUseCount.begin(); useCount != mMaterialUseCount.end(); ++useCount) {
        if (useCount->second > 1 || mSceneCount > 1) {
            DisplayList materialDL(fileDefinition.GetUniqueName(useCount->first));
            settings.mMaterials.find(useCount->first)->second.Write(settings.mDefaultMaterialState, materialDL.GetDataChunk());
            mMaterialNameMapping[useCount->first] = materialDL.GetName();
            
            auto dl = materialDL.Generate(fileSuffix);

            fileDefinition.AddDefinition(std::move(dl));
        }
    }
}

void generateMeshIntoDLWithMaterials(const aiScene* scene, CFileDefinition& fileDefinition, MaterialCollector* materials, std::vector<RenderChunk>& renderChunks, DisplayListSettings& settings, DisplayList &displayList, const std::string& modelSuffix) {
    RCPState rcpState(settings.mDefaultMaterialState, settings.mVertexCacheSize, settings.mMaxMatrixDepth, settings.mCanPopMultipleMatrices);
    for (auto chunk = renderChunks.begin(); chunk != renderChunks.end(); ++chunk) {
        if (materials) {
            std::string materialName = scene->mMaterials[chunk->mMesh->mMesh->mMaterialIndex]->GetName().C_Str();
            displayList.AddCommand(std::unique_ptr<DisplayListCommand>(new CommentCommand("Material " + materialName)));
            auto mappedMaterialName = materials->mMaterialNameMapping.find(materialName);

            if (mappedMaterialName != materials->mMaterialNameMapping.end()) {
                displayList.AddCommand(std::unique_ptr<DisplayListCommand>(new CallDisplayListByNameCommand(mappedMaterialName->second)));
            } else {
                auto material = settings.mMaterials.find(materialName);

                if (material != settings.mMaterials.end()) {
                    material->second.Write(rcpState.GetMaterialState(), displayList.GetDataChunk());
                }
            }
            
            displayList.AddCommand(std::unique_ptr<DisplayListCommand>(new CommentCommand("End Material " + materialName)));
        }

        
        std::string vertexBuffer = fileDefinition.GetVertexBuffer(chunk->mMesh, chunk->mMaterial, modelSuffix);
        generateGeometry(*chunk, rcpState, vertexBuffer, displayList, settings.mHasTri2);
    }
    rcpState.TraverseToBone(nullptr, displayList);
}


void generateMeshIntoDL(const aiScene* scene, CFileDefinition& fileDefinition, std::vector<RenderChunk>& renderChunks, DisplayListSettings& settings, DisplayList &displayList, const std::string& fileSuffix) {
    MaterialCollector materials;

    materials.CollectMaterialResources(scene, renderChunks, settings);
    materials.GenerateMaterials(settings, fileDefinition, fileSuffix);

    generateMeshIntoDLWithMaterials(scene, fileDefinition, &materials, renderChunks, settings, displayList, fileSuffix);
}

std::string generateMesh(const aiScene* scene, CFileDefinition& fileDefinition, std::vector<RenderChunk>& renderChunks, DisplayListSettings& settings, const std::string& fileSuffix) {
    DisplayList displayList(fileDefinition.GetUniqueName("model_gfx"));
    generateMeshIntoDL(scene, fileDefinition, renderChunks, settings, displayList, fileSuffix);
    std::unique_ptr<FileDefinition> dlResult = displayList.Generate(fileSuffix);
    fileDefinition.AddDefinition(std::move(dlResult));

    return displayList.GetName();
}