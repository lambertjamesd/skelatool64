#ifndef __STATIC_GENERATOR_H__
#define __STATIC_GENERATOR_H__

#include "DefinitionGenerator.h"
#include "../DisplayListSettings.h"

struct StaticContentElement {
    std::string meshName;
    std::string materialName;
};

struct StaticGeneratorOutput {
    std::string staticContentName;
    int staticContentCount;
};

class StaticGenerator : public DefinitionGenerator {
public:
    StaticGenerator(const DisplayListSettings& settings);

    virtual bool ShouldIncludeNode(aiNode* node);
    virtual void GenerateDefinitions(const aiScene* scene, CFileDefinition& fileDefinition);

    const StaticGeneratorOutput& GetOutput() const;
private:
    DisplayListSettings mSettings;

    StaticGeneratorOutput mOutput;
};

#endif