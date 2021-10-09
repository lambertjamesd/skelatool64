
#include "BoneHierarchy.h"

Bone::Bone(int index, std::string name, Bone* parent, const aiVector3D& restPosition, const aiQuaternion& restRotation):
    mIndex(index),
    mName(name),
    mParent(parent),
    mRestPosition(restPosition),
    mRestRotation(restRotation) {

    if (mParent) {
        mParent->mChildren.push_back(this);
    }
}

int Bone::GetIndex() {
    return mIndex;
}

const std::string& Bone::GetName() {
    return mName;
}

Bone* Bone::GetParent() {
    return mParent;
}

void BoneHierarchy::SearchForBones(aiNode* node, Bone* currentBoneParent, std::set<std::string>& knownBones) {
    if (knownBones.find(node->mName.C_Str()) != knownBones.end()) {
        aiVector3D restPosition;
        aiQuaternion restRotation;
        node->mTransformation.DecomposeNoScaling(restRotation, restPosition);

        mBones.push_back(std::unique_ptr<Bone>(new Bone(
            mBones.size(),
            node->mName.C_Str(),
            currentBoneParent,
            restPosition,
            restRotation
        )));

        currentBoneParent = mBones[mBones.size() - 1].get();
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        SearchForBones(node->mChildren[i], currentBoneParent, knownBones);
    }
}