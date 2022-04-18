#ifndef _EXTENDED_MESH_H
#define _EXTENDED_MESH_H

#include <assimp/mesh.h>
#include "BoneHierarchy.h"
#include <vector>
#include <map>
#include <set>

enum class VertexType {
    PosUVNormal,
    PosUVColor,
};

class ExtendedMesh {
public:
    ExtendedMesh(aiMesh* mesh, BoneHierarchy& boneHierarchy);
    ~ExtendedMesh();
    aiMesh* mMesh;
    std::vector<aiMatrix4x4*> mPointInverseTransform;
    std::vector<aiMatrix3x3*> mNormalInverseTransform;
    std::vector<Bone*> mVertexBones;
    std::map<Bone*, std::vector<aiFace*>> mFacesForBone;
    // first bone in pair is always the parent of the second
    std::map<std::pair<Bone*, Bone*>, std::vector<aiFace*>> mBoneSpanningFaces;
    aiVector3D bbMin;
    aiVector3D bbMax;

    void RecalcBB();

    void Transform(const aiMatrix4x4& transform);
    void ReplaceColor(const aiColor4D& color);

    bool isFaceOneBone(aiFace* face);
    std::pair<Bone*, Bone*> findTransitionPairForFace(aiFace* face);
private:
    void PopulateFacesForBone();
};

aiMesh* copyMesh(aiMesh* mesh);

void findAdjacentVertices(aiMesh* mesh, unsigned fromIndex, std::set<int>& result);

#endif