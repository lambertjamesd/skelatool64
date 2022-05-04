#ifndef __LEVEL_GENERATOR_H__
#define __LEVEL_GENERATOR_H__

#include "DefinitionGenerator.h"
#include "StaticGenerator.h"
#include "CollisionGenerator.h"

class LevelGenerator : public DefinitionGenerator {
public:
    LevelGenerator(
        const StaticGeneratorOutput& staticOutput,
        const CollisionGeneratorOutput& collisionOutput
    );

    virtual bool ShouldIncludeNode(aiNode* node);
    virtual void GenerateDefinitions(const aiScene* scene, CFileDefinition& fileDefinition);

private:
    StaticGeneratorOutput mStaticOutput;
    CollisionGeneratorOutput mCollisionOutput;
};

#endif