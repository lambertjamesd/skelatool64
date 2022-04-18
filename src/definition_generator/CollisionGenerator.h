#ifndef __COLLISION_GENERATOR_H__
#define __COLLISION_GENERATOR_H__

#include "DefinitionGenerator.h"
#include "../DisplayListSettings.h"

struct CollisionQuad {
    CollisionQuad(aiMesh* mesh, const aiMatrix4x4& transform);

    aiVector3D corner;
    aiVector3D edgeA;
    float edgeALength;
    aiVector3D edgeB;
    float edgeBLength;
    aiVector3D normal;

    std::unique_ptr<DataChunk> Generate();
};

class CollisionGenerator : public DefinitionGenerator {
public:
    CollisionGenerator(const DisplayListSettings& settings);

    virtual bool ShouldIncludeNode(aiNode* node);
    virtual void GenerateDefinitions(const aiScene* scene, CFileDefinition& fileDefinition);
private:
    DisplayListSettings mSettings;
};

#endif