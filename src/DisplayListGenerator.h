#ifndef _DISPLAY_LIST_GENERATOR_H
#define _DISPLAY_LIST_GENERATOR_H

#include <assimp/mesh.h>
#include "./RCPState.h"
#include "./DisplayList.h"
#include "./CFileDefinition.h"

void generateGeometry(aiMesh* mesh, RCPState& state, int vertexBuffer, DisplayList& output, bool hasTri2);

#endif