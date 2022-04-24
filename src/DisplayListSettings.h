#ifndef _DISPLAY_LIST_SETTINGS_H
#define _DISPLAY_LIST_SETTINGS_H

#include <string>
#include <map>
#include <assimp/scene.h>
#include "./materials/Material.h"

struct DisplayListSettings {
    DisplayListSettings();
    std::string mPrefix;
    int mVertexCacheSize;
    bool mHasTri2;
    float mGraphicsScale;
    float mCollisionScale;
    int mMaxMatrixDepth;
    bool mCanPopMultipleMatrices;
    unsigned short mTicksPerSecond;
    std::map<std::string, Material> mMaterials;
    aiQuaternion mRotateModel;
    bool mExportAnimation;
    bool mExportGeometry;
    bool mIncludeCulling;

    aiMatrix4x4 CreateGlobalTransform();
};

#endif