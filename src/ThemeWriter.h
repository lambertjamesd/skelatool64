#ifndef _THEME_WRITER_H
#define _THEME_WRITER_H

#include <assimp/scene.h>
#include <string>
#include <vector>
#include <map>
#include "DisplayListSettings.h"
#include "ThemeDefinition.h"
#include "MeshWriter.h"
#include "ExtendedMesh.h"

class ThemeMesh {
public:
    ThemeMesh();
    std::string objectName;
    ExtendedMesh* mesh;
    ExtendedMesh* wireMesh;
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
    ThemeWriter(const std::string& themeName, const std::string& themeHeader);
    bool GetDecorName(const std::string& nodeName, std::string& output);
    void AppendContentFromScene(const aiScene* scene, DisplayListSettings& settings);
    std::string GetDecorID(const std::string& name);
    unsigned GetMaterialIndex(const std::string& name);
    void WriteTheme(const std::string& output, DisplayListSettings& settings);
    void WriteThemeHeader(const std::string& output, DisplayListSettings& settings);
    const std::string& GetThemeHeader() const;
    const std::string& GetThemeName() const;
    std::string GetDecorMaterial(const std::string& decorName);
    std::string GetDecorGeo(const std::string& decorName); 
    MaterialCollector mMaterialCollector;

    const ThemeMesh* GetThemeMesh(const std::string& id);
private:
    std::string WriteMaterials(std::ostream& cfile, std::vector<ThemeMesh*>& meshList, CFileDefinition& fileDef, DisplayListSettings& settings);
    void AppendContentFromNode(const aiScene* scene, const aiNode* node, DisplayListSettings& settings);

    std::string mThemeName;
    std::string mThemeHeader;
    std::map<std::string, ThemeMesh> mDecorMeshes;
    std::vector<std::string> mUsedMaterials;

    std::vector<std::string> mDecorGeoNames;
    std::vector<std::string> mDecorMaterialNames;
};

void generateThemeDefiniton(ThemeDefinition& themeDef, DisplayListSettings& settings);

#endif