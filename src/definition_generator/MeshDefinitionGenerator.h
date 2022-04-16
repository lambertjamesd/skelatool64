#ifndef __MESH_DEFINTION_GENERATOR_H__
#define __MESH_DEFINTION_GENERATOR_H__

#include "DefinitionGenerator.h"
#include "../DisplayListSettings.h"

class MeshDefinitionGenerator : public DefinitionGenerator {
public:
    MeshDefinitionGenerator(const DisplayListSettings& settings);

    virtual bool ShouldIncludeNode(aiNode* node);
    virtual void GenerateDefinitions(const aiScene* scene, CFileDefinition& fileDefinition);
private:
    DisplayListSettings mSettings;
};

#endif