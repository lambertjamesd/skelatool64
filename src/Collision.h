#ifndef COLLISION_H
#define COLLISION_H

#include <vector>
#include <assimp/mesh.h>

void extractMeshBoundary(aiMesh* mesh, const aiMatrix4x4& transform, std::vector<aiVector3D>& result);

#endif