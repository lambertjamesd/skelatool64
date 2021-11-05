#ifndef _THEME_WRITER_H
#define _THEME_WRITER_H

#include <assimp/scene.h>
#include <string>
#include <vector>
#include <map>
#include "DisplayListSettings.h"
#include "ThemeDefinition.h"
#include "ExtendedMesh.h"

class ThemeMesh {
public:
    ExtendedMesh* mesh;
    std::string materialName;
    unsigned index;
    std::vector<aiVector3D> boundary;
};

class LevelTheme {
public:
    aiScene* scene;
    LevelThemeDefinition definition;
};

class ThemeWriter {
public:
    ThemeWriter(const std::string& themeName);
    bool GetDecorName(const std::string& nodeName, std::string& output);
    void AppendContentFromScene(const aiScene* scene, DisplayListSettings& settings);
    std::string GetDecorID(const std::string& name);
    unsigned GetMaterialIndex(const std::string& name);
    void WriteTheme(const std::string& output, DisplayListSettings& settings);
    void WriteThemeHeader(const std::string& output, DisplayListSettings& settings);
private:
    void AppendContentFromNode(const aiNode* node, DisplayListSettings& settings);

    std::string mThemeName;
    std::map<std::string, ThemeMesh> mDecorMeshes;
    std::vector<std::string> mUsedMaterials;
};

void generateThemeDefiniton(ThemeDefinition& themeDef, DisplayListSettings& settings);

#endif