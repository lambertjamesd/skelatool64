#ifndef _SCENE_WRITER_H
#define _SCENE_WRITER_H

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <sstream>
#include <string>
#include <map>
#include "./Material.h"

struct DisplayListSettings {
    DisplayListSettings();
    std::string mPrefix;
    int mVertexCacheSize;
    bool mHasTri2;
    float mScale;
    int mMaxMatrixDepth;
    bool mCanPopMultipleMatrices;
    unsigned short mTicksPerSecond;
    std::map<std::string, Material> mMaterials;
    aiQuaternion mRotateModel;
    bool mExportAnimation;
    bool mExportGeometry;
};

void generateMeshFromScene(const aiScene* scene, std::ostream& output, std::ostream& headerFile, std::ostream& animationFile, DisplayListSettings& settings);
void generateMeshFromSceneToFile(const aiScene* scene, std::string filename, DisplayListSettings& settings);

#endif