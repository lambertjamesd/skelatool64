#ifndef _MESH_WRITER_H
#define _MESH_WRITER_H

#include <ostream>
#include <string>
#include <assimp/scene.h>

#include "RenderChunk.h"
#include "DisplayListSettings.h"
#include "CFileDefinition.h"

std::string generateMesh(const aiScene* scene, CFileDefinition& fileDefinition, std::vector<RenderChunk>& renderChunks, DisplayListSettings& settings, std::ostream& output);

#endif