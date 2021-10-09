#ifndef _BONE_HIERARCHY_H
#define _BONE_HIERARCHY_H

#include <assimp/vector3.h>
#include <assimp/quaternion.h>
#include <assimp/scene.h>

#include <vector>
#include <memory>
#include <string>
#include <set>
#include "ErrorCode.h"

class Bone {
public:
    Bone(int index, std::string name, Bone* parent, const aiVector3D& restPosition, const aiQuaternion& restRotation);

    int GetIndex();
    const std::string& GetName();
    Bone* GetParent();
private:
    int mIndex;
    std::string mName;
    Bone* mParent;
    aiVector3D mRestPosition;
    aiQuaternion mRestRotation;

    std::vector<Bone*> mChildren;
};

class BoneHierarchy {
public:
    void SearchForBones(aiNode* node, Bone* currentBoneParent, std::set<std::string>& knownBones);
private:
    std::vector<std::unique_ptr<Bone>> mBones;
};

#endif