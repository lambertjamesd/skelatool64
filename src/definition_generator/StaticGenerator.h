#ifndef __STATIC_GENERATOR_H__
#define __STATIC_GENERATOR_H__

#include "DefinitionGenerator.h"
#include "../DisplayListSettings.h"

class StaticGenerator : public DefinitionGenerator {
public:
    StaticGenerator(const DisplayListSettings& settings);

    virtual bool ShouldIncludeNode(aiNode* node);
    virtual void GenerateDefinitions(const aiScene* scene, CFileDefinition& fileDefinition);
private:
    DisplayListSettings mSettings;
};

#endif