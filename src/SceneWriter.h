#ifndef _SCENE_WRITER_H
#define _SCENE_WRITER_H

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <sstream>
#include <string>

struct DisplayListSettings {
    DisplayListSettings();
    std::string mPrefix;
    int mVertexCacheSize;
    bool mHasTri2;
    float mScale;
    int mMaxMatrixDepth;
    bool mCanPopMultipleMatrices;
};

void generateMeshFromScene(const aiScene* scene, std::ostream& output, DisplayListSettings& settings);
void generateMeshFromSceneToFile(const aiScene* scene, std::string filename, DisplayListSettings& settings);

#endif