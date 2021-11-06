#include "MeshWriter.h"

#include <set>
#include <sstream>

#include "RCPState.h"
#include "DisplayListGenerator.h"
#include "StringUtils.h"

void collectMaterialResources(const aiScene* scene, std::vector<RenderChunk>& renderChunks, std::vector<std::shared_ptr<MaterialResource>>& resources, std::map<std::string, int>& materialUsecount, DisplayListSettings& settings) {
    std::set<std::shared_ptr<MaterialResource>> resourceAsSet;

    for (auto chunk = renderChunks.begin(); chunk != renderChunks.end(); ++chunk) {
        std::string materialName = scene->mMaterials[chunk->mMesh->mMesh->mMaterialIndex]->GetName().C_Str();

        auto materialDL = settings.mMaterials.find(materialName);

        if (materialDL == settings.mMaterials.end()) {
            continue;
        }

        auto prevCount = materialUsecount.find(materialName);

        if (prevCount == materialUsecount.end()) {
            materialUsecount[materialName] = 1;
        } else {
            materialUsecount[materialName] = prevCount->second + 1;
        }

        for (auto resource = materialDL->second.mUsedResources.begin(); resource != materialDL->second.mUsedResources.end(); ++resource) {
            resourceAsSet.insert(*resource);
        }
    }

    resources.insert(resources.begin(), resourceAsSet.begin(), resourceAsSet.end());
}


void generateMeshIntoDL(const aiScene* scene, CFileDefinition& fileDefinition, std::vector<RenderChunk>& renderChunks, DisplayListSettings& settings, DisplayList &displayList, std::ostream& output) {
    RCPState rcpState(settings.mVertexCacheSize, settings.mMaxMatrixDepth, settings.mCanPopMultipleMatrices);

    std::vector<std::shared_ptr<MaterialResource>> resources;
    std::map<std::string, int> materialUsecount;

    collectMaterialResources(scene, renderChunks, resources, materialUsecount, settings);

    std::map<std::string, std::string> nameMapping;

    Material::WriteResources(resources, nameMapping, fileDefinition, output);

    std::map<std::string, std::string> materialNameMapping;

    for (auto useCount = materialUsecount.begin(); useCount != materialUsecount.end(); ++useCount) {
        if (useCount->second > 1) {
            DisplayList materailDL(fileDefinition.GetUniqueName(useCount->first));
            settings.mMaterials.find(useCount->first)->second.WriteToDL(nameMapping, materailDL);
            materialNameMapping[useCount->first, materailDL.GetName()];
            
            materailDL.Generate(fileDefinition, output);
        }
    }

    for (auto chunk = renderChunks.begin(); chunk != renderChunks.end(); ++chunk) {
        std::string materialName = scene->mMaterials[chunk->mMesh->mMesh->mMaterialIndex]->GetName().C_Str();
        displayList.AddCommand(std::unique_ptr<DisplayListCommand>(new CommentCommand("Material " + materialName)));
        auto mappedMaterialName = materialNameMapping.find(materialName);

        if (mappedMaterialName != materialNameMapping.end()) {
            displayList.AddCommand(std::unique_ptr<DisplayListCommand>(new CallDisplayListByNameCommand(mappedMaterialName->second)));
        } else {
            auto material = settings.mMaterials.find(materialName);

            if (material != settings.mMaterials.end()) {
                material->second.WriteToDL(nameMapping, displayList);
            }
        }

        displayList.AddCommand(std::unique_ptr<DisplayListCommand>(new CommentCommand("End Material " + materialName)));
        
        int vertexBuffer = fileDefinition.GetVertexBuffer(chunk->mMesh, chunk->mVertexType);
        generateGeometry(*chunk, rcpState, vertexBuffer, displayList, settings.mHasTri2);
    }
    rcpState.TraverseToBone(nullptr, displayList);
    fileDefinition.GenerateVertexBuffers(output, settings.mScale, settings.mRotateModel);
}

std::string generateMesh(const aiScene* scene, CFileDefinition& fileDefinition, std::vector<RenderChunk>& renderChunks, DisplayListSettings& settings, std::ostream& output) {
    DisplayList displayList(fileDefinition.GetUniqueName("model_gfx"));
    generateMeshIntoDL(scene, fileDefinition, renderChunks, settings, displayList, output);
    displayList.Generate(fileDefinition, output);
    return displayList.GetName();
}