#ifndef _BONE_HIERARCHY_H
#define _BONE_HIERARCHY_H

#include <assimp/vector3.h>
#include <assimp/quaternion.h>
#include <assimp/scene.h>

#include <vector>
#include <memory>
#include <string>
#include <set>
#include <map>
#include "ErrorCode.h"

class Bone {
public:
    Bone(int index, std::string name, Bone* parent, const aiVector3D& restPosition, const aiQuaternion& restRotation);

    int GetIndex();
    const std::string& GetName();
    Bone* GetParent();

    static Bone* FindCommonAncestor(Bone* a, Bone* b);
    /**
     *  Assumes ancestor is an ancestor of decendant
     *  Returns the child bone of ancestor that is an ancestor
     *  of decendant or is decendant
     */
    static Bone* StepDownTowards(Bone* ancestor, Bone* decendant);

    static bool CompareBones(Bone* a, Bone* b);
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
    void SearchForBonesInScene(const aiScene* scene);
    Bone* BoneForName(std::string name);
private:
    std::vector<std::unique_ptr<Bone>> mBones;
    std::map<std::string, Bone*> mBoneByName;
};

#endif