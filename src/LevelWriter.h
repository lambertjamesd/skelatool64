#ifndef _LEVEL_WRITER_H
#define _LEVEL_WRITER_H

#include <vector>
#include <assimp/scene.h>
#include "./DisplayListSettings.h"
#include "Pathfinding.h"

class ThemeWriter;

#define MAX_PLAYERS 4

class BaseDefinition {
public:
    aiVector3D position;
    unsigned team;
};

class DecorDefinition {
public:
    aiVector3D position;
    aiQuaternion rotation;
    std::string decorID;
};

class LevelDefinition {
public:
    std::vector<BaseDefinition> bases;
    std::vector<aiMesh*> geometryMeshes;
    aiVector3D startPosition[MAX_PLAYERS];
    unsigned maxPlayerCount;
    aiVector3D minBoundary;
    aiVector3D maxBoundary;
    std::vector<aiVector3D> boundary;
    std::vector<DecorDefinition> decor;
    PathfindingDefinition pathfinding;
};

void generateLevelFromSceneToFile(const aiScene* scene, std::string filename, ThemeWriter* theme, DisplayListSettings& settings);

#endif