#ifndef _C_FILE_DEFINITION_H
#define _C_FILE_DEFINITION_H

#include <assimp/mesh.h>
#include <map>
#include <string>
#include <set>
#include <ostream>

#include "./ErrorCode.h"

enum class VertexType {
    PosUVNormal,
    PosUVColor,
};

class VertexBufferDefinition {
public:
    VertexBufferDefinition(aiMesh* targetMesh, std::string name, VertexType vertexType);

    aiMesh* mTargetMesh;
    std::string mName;
    VertexType mVertexType;

    ErrorCode Generate(std::ostream& output, float scale);
private:
};

class CFileDefinition {
public:
    CFileDefinition(std::string prefix);
    int GetVertexBuffer(aiMesh* mesh, VertexType vertexType);

    const std::string GetVertexBufferName(int vertexBufferID);
    std::string GetUniqueName(std::string requestedName);

    ErrorCode GenerateVertexBuffers(std::ostream& output, float scale);
private:
    int GetNextID();

    std::string mPrefix;
    std::set<std::string> mUsedNames;
    std::map<int, VertexBufferDefinition> mVertexBuffers;
    int mNextID;
};

#endif