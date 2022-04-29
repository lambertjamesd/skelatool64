#ifndef __MATERIAL_GENERATOR_H__
#define __MATERIAL_GENERATOR_H__

#include "DefinitionGenerator.h"
#include "../DisplayListSettings.h"

class MaterialGenerator : public DefinitionGenerator {
public:
    MaterialGenerator(const DisplayListSettings& settings);

    virtual bool ShouldIncludeNode(aiNode* node);
    virtual void GenerateDefinitions(const aiScene* scene, CFileDefinition& fileDefinition);
private:
    DisplayListSettings mSettings;
};

#endif