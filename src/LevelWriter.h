#ifndef _LEVEL_WRITER_H
#define _LEVEL_WRITER_H

#include <vector>
#include <assimp/scene.h>
#include "./DisplayListSettings.h"

class BaseDefinition {
public:
    aiVector3D position;
    unsigned team;
};

class LevelDefinition {
public:
    std::vector<BaseDefinition> bases;
    std::vector<aiMesh*> geometryMeshes;
};

void generateLevelFromSceneToFile(const aiScene* scene, std::string filename, DisplayListSettings& settings);

#endif