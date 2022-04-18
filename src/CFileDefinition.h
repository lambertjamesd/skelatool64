#ifndef _C_FILE_DEFINITION_H
#define _C_FILE_DEFINITION_H

#include <assimp/mesh.h>
#include <map>
#include <string>
#include <set>
#include <ostream>
#include <memory>

#include "./ErrorResult.h"
#include "./ExtendedMesh.h"
#include "./definitions/FileDefinition.h"

class VertexBufferDefinition {
public:
    VertexBufferDefinition(ExtendedMesh* targetMesh, std::string name, VertexType vertexType);

    ExtendedMesh* mTargetMesh;
    std::string mName;
    VertexType mVertexType;

    ErrorResult Generate(float scale, aiQuaternion rotate, std::unique_ptr<FileDefinition>& output, const std::string& fileSuffix);
private:
};

class CFileDefinition {
public:
    CFileDefinition(std::string prefix, float modelScale, aiQuaternion modelRotate);

    void AddDefinition(std::unique_ptr<FileDefinition> definition);
    void AddMacro(const std::string& name, const std::string& value);

    std::string GetVertexBuffer(ExtendedMesh* mesh, VertexType vertexType, const std::string& modelSuffix);
    std::string GetCullingBuffer(const std::string& name, const aiVector3D& min, const aiVector3D& max, const std::string& modelSuffix);

    std::string GetUniqueName(std::string requestedName);

    std::string GetMacroName(std::string requestedName);

    std::set<std::string> GetDefinitionTypes();

    void GenerateAll(const std::string& headerFileLocation);

    void Generate(std::ostream& output, const std::string& location, const std::string& headerFileName);
    void GenerateHeader(std::ostream& output, const std::string& headerFileName);

    bool HasDefinitions(const std::string& location);
private:
    std::string mPrefix;
    float mModelScale;
    aiQuaternion mModelRotate;
    std::set<std::string> mUsedNames;
    std::map<std::string, VertexBufferDefinition> mVertexBuffers;
    std::vector<std::unique_ptr<FileDefinition>> mDefinitions;
    std::vector<std::string> mMacros;
};

#endif