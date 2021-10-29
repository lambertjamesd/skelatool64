#ifndef _LEVEL_WRITER_H
#define _LEVEL_WRITER_H

#include <vector>
#include <assimp/scene.h>
#include "./DisplayListSettings.h"

#define MAX_PLAYERS 4

class BaseDefinition {
public:
    aiVector3D position;
    unsigned team;
};

class LevelDefinition {
public:
    std::vector<BaseDefinition> bases;
    std::vector<aiMesh*> geometryMeshes;
    aiVector3D startPosition[MAX_PLAYERS];
    unsigned maxPlayerCount;
    aiVector3D minBoundary;
    aiVector3D maxBoundary;
};

void generateLevelFromSceneToFile(const aiScene* scene, std::string filename, DisplayListSettings& settings);

#endif