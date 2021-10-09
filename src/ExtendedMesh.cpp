
#include "ExtendedMesh.h"

#include <algorithm>

ExtendedMesh::ExtendedMesh(aiMesh* mesh, BoneHierarchy& boneHierarchy) :
    mMesh(mesh) {
    mVertexBones.resize(mMesh->mNumVertices);

    std::set<Bone*> bonesAsSet;

    for (unsigned int boneIndex = 0; boneIndex < mMesh->mNumBones; ++boneIndex) {
        aiBone* bone = mMesh->mBones[boneIndex];
        Bone* hierarchyBone = boneHierarchy.BoneForName(bone->mName.C_Str());
        bonesAsSet.insert(hierarchyBone);
        for (unsigned int vertexIndex = 0; vertexIndex < bone->mNumWeights; ++vertexIndex) {
            mVertexBones[bone->mWeights[vertexIndex].mVertexId] = hierarchyBone;
        }
    }

    mUsesBones = std::vector<Bone*>(bonesAsSet.begin(), bonesAsSet.end());

    std::sort(mUsesBones.begin(), mUsesBones.end(), Bone::CompareBones);

    PopulateFacesForBone();
}


bool ExtendedMesh::isFaceOneBone(aiFace* face) {
    Bone* bone = mVertexBones[face->mIndices[0]];

    for (unsigned int i = 1; i < face->mNumIndices; ++i) {
        if (mVertexBones[face->mIndices[i]] != bone) {
            return false;
        }
    }

    return true;
}

std::pair<Bone*, Bone*> ExtendedMesh::findTransitionPairForFace(aiFace* face) {
    Bone* ancestor = mVertexBones[face->mIndices[0]];

    for (unsigned int i = 1; i < face->mNumIndices; ++i) {
        ancestor = Bone::FindCommonAncestor(ancestor, mVertexBones[face->mIndices[i]]);
    }

    Bone* second = ancestor;

    for (unsigned int i = 0; i < face->mNumIndices; ++i) {
        if (mVertexBones[face->mIndices[i]] != ancestor) {
            second = Bone::StepDownTowards(ancestor, mVertexBones[face->mIndices[i]]);
            break;
        }
    }

    return std::make_pair(ancestor, second);
}

void ExtendedMesh::PopulateFacesForBone() {
    for (unsigned int faceIndex = 0; faceIndex < mMesh->mNumFaces; ++faceIndex) {
        aiFace* face = &mMesh->mFaces[faceIndex];
        if (isFaceOneBone(face)) {
            mFacesForBone[mVertexBones[face->mIndices[0]]].push_back(face);
        } else {
            mBoneSpanningFaces[findTransitionPairForFace(face)].push_back(face);
        }
    }
}