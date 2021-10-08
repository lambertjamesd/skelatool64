#ifndef _C_FILE_DEFINITION_H
#define _C_FILE_DEFINITION_H

#include <assimp/mesh.h>

class CFileDefinition {
public:
    int GetVertexBuffer(aiMesh* mesh, bool useNormals);

    const std::string GetVertexBufferName(int vertexBufferID);
private:
};

#endif