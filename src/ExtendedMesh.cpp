
#include "ExtendedMesh.h"

#include <algorithm>
#include "MathUtl.h"

aiMesh* copyMesh(aiMesh* mesh) {
    aiMesh* result = new aiMesh();
    result->mNumVertices = mesh->mNumVertices;

    result->mVertices = new aiVector3D[result->mNumVertices];
    std::copy(mesh->mVertices, mesh->mVertices + result->mNumVertices, result->mVertices);

    if (mesh->mNormals) {
        result->mNormals = new aiVector3D[result->mNumVertices];
        std::copy(mesh->mNormals, mesh->mNormals + result->mNumVertices, result->mNormals);
    }

    result->mMaterialIndex = mesh->mMaterialIndex;

    result->mNumFaces = mesh->mNumFaces;
    result->mFaces = new aiFace[mesh->mNumFaces];
    std::copy(mesh->mFaces, mesh->mFaces + result->mNumFaces, result->mFaces);

    return result;
}

ExtendedMesh::ExtendedMesh(aiMesh* mesh, BoneHierarchy& boneHierarchy) :
    mMesh(mesh) {
    mVertexBones.resize(mMesh->mNumVertices);
    mPointInverseTransform.resize(mMesh->mNumVertices);
    mNormalInverseTransform.resize(mMesh->mNumVertices);

    std::set<Bone*> bonesAsSet;

    for (unsigned int boneIndex = 0; boneIndex < mMesh->mNumBones; ++boneIndex) {
        aiBone* bone = mMesh->mBones[boneIndex];
        Bone* hierarchyBone = boneHierarchy.BoneForName(bone->mName.C_Str());
        bonesAsSet.insert(hierarchyBone);

        aiMatrix3x3 normalTransform(bone->mOffsetMatrix);
        normalTransform = normalTransform.Transpose().Inverse();

        for (unsigned int vertexIndex = 0; vertexIndex < bone->mNumWeights; ++vertexIndex) {
            unsigned int vertexId = bone->mWeights[vertexIndex].mVertexId;
            mVertexBones[vertexId] = hierarchyBone;
            mPointInverseTransform[vertexId] = &bone->mOffsetMatrix;
            mNormalInverseTransform[vertexId] = new aiMatrix3x3(normalTransform);
        }
    }

    PopulateFacesForBone();
    RecalcBB();
}

ExtendedMesh::~ExtendedMesh() {
    for (unsigned int i = 0; i < mNormalInverseTransform.size(); ++i) {
        if (mNormalInverseTransform[i]) {
            delete mNormalInverseTransform[i];
        }
    }
}

void ExtendedMesh::RecalcBB() {
    bbMin = mMesh->mVertices[0];
    bbMax = mMesh->mVertices[0];

    for (unsigned i = 1; i < mMesh->mNumVertices; ++i) {
        bbMin = min(bbMin, mMesh->mVertices[i]);
        bbMax = max(bbMax, mMesh->mVertices[i]);
    }
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

void ExtendedMesh::Transform(const aiMatrix4x4& transform) {
    aiMatrix3x3 rotationOnly(transform);
    for (unsigned i = 0; i < mMesh->mNumVertices; ++i) {
        mMesh->mVertices[i] = transform * mMesh->mVertices[i];

        if (mMesh->mNormals) {
            mMesh->mNormals[i] = rotationOnly * mMesh->mNormals[i];
            mMesh->mNormals[i].NormalizeSafe();
        }
    }
    RecalcBB();
}

void ExtendedMesh::ReplaceColor(const aiColor4D& color) {
    if (mMesh->mColors[0]) {
        delete [] mMesh->mColors[0];
    }

    mMesh->mColors[0] = new aiColor4D[mMesh->mNumVertices];

    for (unsigned i = 0; i < mMesh->mNumVertices; ++i) {
        mMesh->mColors[0][i] = color;
    }
}

void findAdjacentVertices(aiMesh* mesh, unsigned fromIndex, std::set<int>& result) {
    for (unsigned faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
        aiFace* face = &mesh->mFaces[faceIndex];

        for (unsigned index = 0; index < face->mNumIndices; ++index) {
            if (face->mIndices[index] == fromIndex) {
                result.insert(face->mIndices[(index + 1) % face->mNumIndices]);
                result.insert(face->mIndices[(index + face->mNumIndices - 1) % face->mNumIndices]);
                break;
            }
        }
    }
}