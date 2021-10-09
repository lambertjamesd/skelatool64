#ifndef _DISPLAY_LIST_GENERATOR_H
#define _DISPLAY_LIST_GENERATOR_H

#include <assimp/mesh.h>
#include "./RCPState.h"
#include "./DisplayList.h"
#include "./CFileDefinition.h"
#include "./RenderChunk.h"

void generateGeometry(RenderChunk& mesh, RCPState& state, int vertexBuffer, DisplayList& output, bool hasTri2);

#endif