#include "ThemeWriter.h"

#include <string.h>

bool ThemeWriter::GetDecorName(const std::string& nodeName, std::string& output) {
    if (nodeName.rfind("Decor ") != 0) {
        return false;
    }

    unsigned nameStart = strlen("Decor ");
    unsigned nameEnd = nameStart;

    while (nameEnd < nodeName.length() && nodeName[nameEnd] != '.') {
        ++nameEnd;
    }

    output = nodeName.substr(nameStart, nameEnd - nameStart);

    return true;
}

void ThemeWriter::AppendContentFromScene(const aiScene* scene, DisplayListSettings& settings) {
    for (unsigned meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
        aiMesh* mesh = scene->mMeshes[meshIndex];
        std::string decorName;
        if (GetDecorName(mesh->mName.C_Str(), decorName)) {

        }
    }
}