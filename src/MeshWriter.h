#ifndef _MESH_WRITER_H
#define _MESH_WRITER_H

#include <set>
#include <map>
#include <ostream>
#include <string>
#include <assimp/scene.h>

#include "RenderChunk.h"
#include "DisplayListSettings.h"
#include "CFileDefinition.h"

class MaterialCollector {
public:
    MaterialCollector();
    void UseMaterial(const std::string& material, DisplayListSettings& settings);
    void CollectMaterialResources(const aiScene* scene, std::vector<RenderChunk>& renderChunks, DisplayListSettings& settings);
    void GenerateMaterials(CFileDefinition& fileDefinition, DisplayListSettings& settings, std::ostream& output);

    unsigned mSceneCount;
    std::set<std::shared_ptr<MaterialResource>> mUsedResources;
    std::map<std::string, int> mMaterialUseCount;
    std::map<std::string, std::string> mMaterialNameMapping;
    std::map<std::string, std::string> mResourceNameMapping;
};

void generateMeshIntoDLWithMaterials(const aiScene* scene, CFileDefinition& fileDefinition, MaterialCollector& materials, std::vector<RenderChunk>& renderChunks, DisplayListSettings& settings, DisplayList &displayList);
void generateMeshIntoDL(const aiScene* scene, CFileDefinition& fileDefinition, std::vector<RenderChunk>& renderChunks, DisplayListSettings& settings, DisplayList &displayList, std::ostream& output);
void generateWireframeIntoDL(const aiScene* scene, CFileDefinition& fileDefinition, std::vector<RenderChunk>& renderChunks, DisplayListSettings& settings, DisplayList &displayList);
std::string generateMesh(const aiScene* scene, CFileDefinition& fileDefinition, std::vector<RenderChunk>& renderChunks, DisplayListSettings& settings, std::ostream& output);

#endif