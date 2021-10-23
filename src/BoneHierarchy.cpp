
#include "BoneHierarchy.h"

Bone::Bone(int index, std::string name, Bone* parent, const aiVector3D& restPosition, const aiQuaternion& restRotation, const aiVector3D& restScale):
    mIndex(index),
    mName(name),
    mParent(parent),
    mRestPosition(restPosition),
    mRestRotation(restRotation),
    mRestScale(restScale) {

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

void Bone::GenerateRestPosiitonData(std::ostream& output, float scale, const aiQuaternion& rotation) {
    aiVector3D restPosition = rotation.Rotate(mRestPosition);
    aiQuaternion restRotation = rotation * mRestRotation;

    output <<
        "{{" << (restPosition.x * scale) << ", " << (restPosition.y * scale) << ", " << (restPosition.z * scale) << "}, {" <<
        restRotation.x << ", " << restRotation.y << ", " << restRotation.z << ", " << restRotation.w << "}, {" <<
        mRestScale.x << ", " << mRestScale.y << ", " << mRestScale.z << "}}";
}

Bone* Bone::FindCommonAncestor(Bone* a, Bone* b) {
    std::set<Bone*> hierarchyDifference;

    Bone* curr = a;

    while (curr) {
        hierarchyDifference.insert(curr);
        curr = curr->mParent;
    }

    curr = b;
    
    while (curr) {
        hierarchyDifference.erase(curr);
        curr = curr->mParent;
    }

    curr = a;

    while (hierarchyDifference.find(curr) != hierarchyDifference.end()) {
        curr = curr->mParent;
    }

    return curr;
}

Bone* Bone::StepDownTowards(Bone* ancestor, Bone* decendant) {
    Bone* curr = decendant;

    while (curr && curr->mParent != ancestor) {
        curr = curr->mParent;
    }

    return curr;
}

bool Bone::CompareBones(Bone* a, Bone* b) {
    if (a == nullptr && b == nullptr) {
        return false;
    } else if (a == nullptr) {
        return true;
    } else if (b == nullptr) {
        return false;
    } else {
        return a->mIndex < b->mIndex;
    }
}

int Bone::GetBoneIndex(Bone* a) {
    if (a == nullptr) {
        return -1;
    } else {
        return a->mIndex;
    }
}

void BoneHierarchy::SearchForBones(aiNode* node, Bone* currentBoneParent, std::set<std::string>& knownBones) {
    if (knownBones.find(node->mName.C_Str()) != knownBones.end()) {
        aiVector3D restPosition;
        aiQuaternion restRotation;
        aiVector3D restScale;
        node->mTransformation.Decompose(restScale, restRotation, restPosition);

        mBones.push_back(std::unique_ptr<Bone>(new Bone(
            mBones.size(),
            node->mName.C_Str(),
            currentBoneParent,
            restPosition,
            restRotation,
            restScale
        )));

        currentBoneParent = mBones[mBones.size() - 1].get();
        mBoneByName.insert(std::pair<std::string, Bone*>(node->mName.C_Str(), currentBoneParent));
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        SearchForBones(node->mChildren[i], currentBoneParent, knownBones);
    }
}

void BoneHierarchy::SearchForBonesInScene(const aiScene* scene) {
    std::set<std::string> knownBones;

    for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
        aiMesh* mesh = scene->mMeshes[meshIndex];

        for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
            knownBones.insert(mesh->mBones[boneIndex]->mName.C_Str());
        }
    }

    SearchForBones(scene->mRootNode, nullptr, knownBones);
}

Bone* BoneHierarchy::BoneForName(std::string name) {
    auto result = mBoneByName.find(name);

    if (result == mBoneByName.end()) {
        return nullptr;
    } else {
        return result->second;
    }
}

void BoneHierarchy::GenerateRestPosiitonData(const std::string& variableName, std::ostream& output, float scale, aiQuaternion rotation) {
    if (mBones.size() == 0) return;

    output << "struct Transform " << variableName << "[] = {" << std::endl;

    for (unsigned int boneIndex = 0; boneIndex < mBones.size(); ++boneIndex) {
        output << "    ";
        if (mBones[boneIndex]->GetParent()) {
            mBones[boneIndex]->GenerateRestPosiitonData(output, scale, aiQuaternion());
        } else {
            mBones[boneIndex]->GenerateRestPosiitonData(output, scale, rotation);
        }
        output << "," << std::endl;
    }

    output << "};" << std::endl;
}

bool BoneHierarchy::HasData() const {
    return mBones.size() > 0;
}

unsigned int BoneHierarchy::GetBoneCount() const {
    return mBones.size();
}