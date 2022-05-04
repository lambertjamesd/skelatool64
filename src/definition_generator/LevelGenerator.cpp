#include "LevelGenerator.h"

LevelGenerator::LevelGenerator(
    const StaticGeneratorOutput& staticOutput,
    const CollisionGeneratorOutput& collisionOutput
) : DefinitionGenerator(), mStaticOutput(staticOutput), mCollisionOutput(collisionOutput) {}

bool LevelGenerator::ShouldIncludeNode(aiNode* node) {
    return false;
}

void LevelGenerator::GenerateDefinitions(const aiScene* scene, CFileDefinition& fileDefinition) {
    std::unique_ptr<StructureDataChunk> levelDef(new StructureDataChunk());

    levelDef->AddPrimitive(mCollisionOutput.quadsName);
    levelDef->AddPrimitive(mStaticOutput.staticContentName);
    levelDef->AddPrimitive(mCollisionOutput.quadCount);
    levelDef->AddPrimitive(mStaticOutput.staticContentCount);

    fileDefinition.AddDefinition(std::unique_ptr<FileDefinition>(new DataFileDefinition("struct LevelDefinition", fileDefinition.GetUniqueName("level"), false, "_geo", std::move(levelDef))));
}